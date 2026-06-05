# Dynamic Module Shelf — Phase B Implementation Plan (runtime miss / eviction)

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** When a `lib_call` requests a module that isn't resident in the XRAM shelf, the 6502 loader streams it from SD into an evicted slot (host handshake), updates the directory, and dispatches — so more modules can exist on disk than there are physical slots.

**Architecture:** The Phase A directory (`shelf_tag[4]`@$0418, `shelf_lru[4]`@$041C) becomes a true cache. On a miss, the 6502 picks a victim slot (empty-first, else LRU back), issues a **FIO `LOAD_MODULE`** command (write FIO register bank at `$B9A0`, poll `$B9A1`), the host streams the module SD→XRAM at the slot base, then the 6502 updates the directory and falls through to the existing page-in. **Zero RTL** — `fio.sv` is a pure register bank that already forwards a generic command+params to the ESP (confirmed `fio.sv:25`, `fio_dispatcher.cpp:741`), and `streamSdramAsset` is runtime-callable. The 6502 owns the cache; the host is a dumb "stream id→slot" servant.

**Tech Stack:** ca65 6502 asm (`runtime/asm/libcall.s`), C# emulator + MSTest (`e6502.Avalonia` FileIoController/CompositeBusDevice, `e6502UnitTests`), Arduino C++ firmware (`e6502.ESP32/novahost/fio_dispatcher.*`). Design: `docs/plans/2026-06-05-dynamic-module-shelf-design.md` §5/§7.

**Locked decisions:**
- **FIO `LOAD_MODULE` opcode = `$2C`** (next free after NVGLOAD `$2B`). Params in the FIO bank: module id → `OFF_SRC_LO` (`$B9A4`); dest slot → `OFF_END_LO` (`$B9A6`). Status read at `$B9A1` (`0`=idle, `2`=OK, `3`=err).
- **6502 owns the directory.** The host only streams bytes into the slot; the 6502 sets `shelf_tag[victim]` and touches `shelf_lru` after a successful load. On load failure it sets `shelf_tag[victim]=0` (empty/untrusted) and returns `LERR_BAD_MODULE`.
- **Victim selection:** first slot with `shelf_tag==0` (empty); if none, `shelf_lru[SHELF_N-1]` (LRU back).
- **Identity:** host resolves id→`/lib/file` from `boot.json libraries[]` (re-read per load; cheap, SD mounted). Validates `(id, version)` from the streamed header is done by the 6502's existing `lc_validate` after page-in.
- N=4 unchanged. Loader must stay **≤ 248 bytes** (below `SHELF_TAG` $0418).

**Pre-flight:** `export PATH=/Users/barry/Git/cc65/bin:$PATH`.

---

### Task B1: Emulator-side FIO `LOAD_MODULE` (test substrate + Avalonia demand-load)

The 6502 miss-handler (Task B2) can only be TDD'd if the emulator answers the FIO `LOAD_MODULE` command the way the board's ESP will. Mirror it in the emulator's `FileIoController`, sourcing module bytes from a test-injected store.

**Files:**
- Modify: `e6502.Avalonia/Hardware/FileIoController.cs` (add the command)
- Modify: `e6502.Avalonia/Hardware/CompositeBusDevice.cs` (wire a module store + XRAM write, if not already reachable)
- Test: `e6502UnitTests/` (a focused FileIoController test)

**Step 1 — investigate first.** Read `FileIoController.cs`: find the command opcode constants + the dispatch switch, and how an existing XRAM-targeting command (e.g. XLOAD `$18` / XPAGE `$29`) writes into XRAM (`_xmc.TryWriteLinear` via a callback). Confirm the FIO register offsets match the firmware (`OFF_CMD=0, OFF_STATUS=1, OFF_SRC_LO=4, OFF_END_LO=6`, base `$B9A0`). Report the dispatch pattern before coding.

**Step 2 — failing test.** In a new `e6502UnitTests/ShelfMissTests.cs`, write a test that: constructs a `CompositeBusDevice`, injects a module store `{ id 1 → graphics.bin bytes }` (via a new `internal` test hook, e.g. `bus.SetShelfModuleStore(IDictionary<byte,byte[]>)`), writes the FIO bank to request `LOAD_MODULE(id=1, slot=2)` (poke `$B9A4=1`, `$B9A6=2`, `$B9A0=$2C`), pumps the controller, then asserts XRAM at `$068000` (slot 2) now equals the graphics image and FIO status `$B9A1==2` (OK). Run → FAIL (command unhandled).

**Step 3 — implement.** Add `CMD_LOAD_MODULE=0x2C` to FileIoController: read id (`SRC_LO`) + slot (`END_LO`), look up the store, write bytes to XRAM at `SHELF_BASE + slot*0x4000` (reuse the existing XRAM-write path), set status OK; unknown id / no store → status ERR. Provide `CompositeBusDevice.SetShelfModuleStore(...)` test hook. Run → PASS.

**Step 4 — commit:** `feat(shelf): emulator FIO LOAD_MODULE (demand-load stub for miss path)`.

---

### Task B2: 6502 miss-handshake in `libcall.s` (the core)

Replace the Phase A miss stub (`mt_miss: sec / rts`) with the load handshake + eviction. TDD through the emulator (now that B1 answers the command).

**Files:**
- Modify: `runtime/asm/libcall.s` (and `libabi.inc` for FIO + opcode constants if not already present)
- Rebuild/install: `runtime/asm/libcall.bin` → `e6502.Avalonia/Resources/libcall.bin` (+ `tests/asm` build)
- Test: `e6502UnitTests/ShelfMissTests.cs` (driving real `lib_call`)

**Step 1 — failing tests** (in ShelfMissTests.cs, using the `StageShelfModule`/`SetShelfModuleStore` hooks + the `CallLib` pattern from CompositeLoaderTests):
- `LibCall_Miss_LoadsFromHostThenDispatches`: stage NOTHING (empty shelf, all tags 0) but set the store `{1→graphics.bin}`; `CallLib(GRAPHICS, GCLS)` → asserts `STATUS==LERR_OK`, `RESIDENT==1`, and `shelf_tag` now has id 1 in some slot (the loaded one), `PageInCount==1`.
- `LibCall_Miss_EvictsLruWhenFull`: fill all 4 slots with dummy tags (ids 0x10–0x13) and seed an LRU order; store `{1→graphics.bin}`; call GRAPHICS → asserts the **LRU-back** slot's tag became 1 and graphics dispatched.
- `LibCall_Miss_HostError_ReturnsBadModule`: empty shelf, store does NOT contain the id → host returns ERR → assert `STATUS==LERR_BAD_MODULE` and the victim slot tag is left 0.
Run → FAIL (current miss path just returns LERR_BAD_MODULE).

**Step 2 — implement** the miss handler in `libcall.s`. Add FIO constants to `libabi.inc` (`FIO_CMD=$B9A0`, `FIO_STATUS=$B9A1`, `FIO_SRC_LO=$B9A4`, `FIO_END_LO=$B9A6`, `FIO_CMD_LOAD_MODULE=$2C`, `FIO_ST_OK=2`, `FIO_ST_ERR=3`). Replace `mt_miss`:
```
mt_miss:
      ; A still = wanted id. Pick victim slot -> X.
      jsr     shelf_pick_victim        ; X = empty slot, else LRU back
      ; issue FIO LOAD_MODULE(id=A, slot=X)
      sta     FIO_SRC_LO               ; module id
      stx     FIO_END_LO               ; dest slot
      stx     LIB_SCRATCH+1            ; remember victim across the call
      sta     LIB_SCRATCH              ; remember id
      lda     #$00
      sta     FIO_STATUS               ; clear before issuing
      lda     #FIO_CMD_LOAD_MODULE
      sta     FIO_CMD                  ; fire (CPU stalls in the poll below)
mt_poll:
      lda     FIO_STATUS
      beq     mt_poll                  ; 0 = still busy
      cmp     #FIO_ST_OK
      bne     mt_load_fail
      ; success: tag[victim]=id, then program PGD like a hit
      ldx     LIB_SCRATCH+1
      lda     LIB_SCRATCH
      sta     SHELF_TAG,x
      ; (reuse the hit body: compute base from X, program PGD, shelf_touch, clc, rts)
      jmp     mt_program_pgd           ; refactor mt_hit body to a shared entry taking X
mt_load_fail:
      ldx     LIB_SCRATCH+1
      lda     #$00
      sta     SHELF_TAG,x              ; slot now empty/untrusted
      sec
      rts
```
Refactor the Phase A `mt_hit` so the "program PGD from slot X + shelf_touch + clc/rts" tail is reachable by both the hit and the post-load path (e.g. label `mt_program_pgd`). Add `shelf_pick_victim` (scan tags for 0; else load `SHELF_LRU+SHELF_N-1`). Keep `LIB_SCRATCH` usage within the lookup (it's free there); use `LIB_SCRATCH` (id) + `LIB_SCRATCH+1` (victim) — confirm both are inside the 7-byte `LIB_SCRATCH` band `$0319-$031F` and unused by `lc_validate`.

**Step 3 — rebuild + install + size check** (`make` + `make install`; `wc -c < libcall.bin` ≤ 248; rebuild `tests/asm`). Run the ShelfMissTests + the full CompositeLoaderTests/NovaLogo suites → all PASS.

**Step 4 — commit:** `feat(shelf): runtime miss handler — load-on-demand + LRU eviction`.

---

### Task B3: Firmware `CMD_LOAD_MODULE` handler

Mirror B1's behavior on the real board.

**Files:**
- Modify: `e6502.ESP32/novahost/fio_dispatcher.h` (opcode + handler decl)
- Modify: `e6502.ESP32/novahost/fio_dispatcher.cpp` (dispatch case + `handle_load_module`)

**Step 1 — implement** `CMD_LOAD_MODULE = 0x2C`. Handler: read `module_id = _bank[OFF_SRC_LO]`, `dest_slot = _bank[OFF_END_LO]`; validate `dest_slot < SHELF_N`; re-read `/config/boot.json`, find `libraries[]` entry with matching `id` → path/size; `streamSdramAsset(SHELF_BASE + dest_slot*SHELF_SLOT_SIZE, label, &path, 1, size)`; `respond_ok()` or `respond_err(ERR_NOT_FOUND/ERR_IO)`. Do NOT touch the shelf directory (`$0418`) — the 6502 owns it. Include `shelf_alloc.h` for `SHELF_BASE`/`SHELF_SLOT_SIZE`/`SHELF_N`.

**Step 2 — verify** `make test` (host suite still green — `handle_load_module` itself needs SD/bridge so it isn't host-unit-tested, but keep the pure parts factored if practical) and `arduino-cli compile --fqbn 'esp32:esp32:lolin32:PartitionScheme=min_spiffs' --build-path build/shelfb .` succeeds.

**Step 3 — commit:** `feat(shelf): firmware CMD_LOAD_MODULE handler (SD->XRAM slot stream)`.

---

### Task B4: Hardware verification (NO synth — OTA + nova put only)

**Mutates the board; needs user.** Board `novahost.local`.

1. `nova put runtime/asm/libcall.bin /roms/libcall.bin --remote=novahost.local` (new loader with miss handler).
2. `arduino-cli compile ... --build-path build/shelfb .` then OTA `espota.py -i 192.168.1.65 -p 3232 -f build/shelfb/novahost.ino.bin`; board reboots + re-stages.
3. **Provoke a miss on hardware.** The default boot stages graphics into slot 0 (a hit). To exercise the miss path, add a second library to `boot.json` that is NOT auto-staged, OR temporarily unstage graphics (`POST /api/modules/graphics.nmod/staged {staged:false}` then reboot so `shelf_tag` is empty) and then call graphics from NovaLogo: `vm enter "CS"` + `vm enter "REPEAT 4 [FD 50 RT 90]"`. With graphics unstaged, the first turtle call MISSES → triggers `LOAD_MODULE` → host streams `/lib/graphics.nmod` into a slot → square draws. Verify `vm peek 0x0318` (`LIB_RESIDENT`) `==$01` and `vm peek-block 0x0418 8` shows id 1 now occupying the loaded slot.
4. Re-stage graphics / restore boot.json to the desired end state. Confirm `defaultRuntime=novalogo` intact.

---

## Done criteria (Phase B)
- A `lib_call` for a non-resident module loads it from SD on demand and dispatches (emulator `LibCall_Miss_LoadsFromHostThenDispatches`).
- A 5th distinct module evicts the LRU slot (emulator `LibCall_Miss_EvictsLruWhenFull`).
- Host load failure → `LERR_BAD_MODULE`, victim slot left empty (no corrupt-slot trust).
- Firmware `CMD_LOAD_MODULE` streams SD→slot; verified on hardware by provoking a real miss (square draws after an unstaged-graphics call). **No bitstream/synth required.**
- All emulator suites green; `libcall.bin` ≤ 248 bytes, mirrors in sync.

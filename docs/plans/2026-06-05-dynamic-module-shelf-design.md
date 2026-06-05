# Dynamic module shelf — XRAM paged-library cache

**Date:** 2026-06-05
**Status:** Design (validated via brainstorming)
**Author:** Barry Walker (with Claude)
**Related:** [`2026-06-05-nmod-self-documenting-modules-design.md`](2026-06-05-nmod-self-documenting-modules-design.md)

## Goal

Make the XRAM library shelf a **dynamic cache** rather than a static, hand-mapped
set of slots. A module's *id* becomes its permanent identity; its *slot* becomes a
transient residency assignment made when it is loaded. This:

- dissolves the latent **slot collision** in the current static map (`id $7F TEST`
  and `id $01 GRAPHICS` are both hard-wired to slot 0 `$060000` and can never be
  co-resident);
- removes `base` from `boot.json` and from the web staging surface (the firmware /
  runtime own slot allocation, so nothing upstream has to know an address);
- scales to **many on-disk modules over few physical slots** — the shelf is a fast
  cache, and a module not resident is streamed in on demand (slower, but it works).

## 1. Background — the current static scheme and why it must change

`runtime/asm/libcall.s : modtab_lookup` is a compile-time `cmp/beq` chain mapping a
module id to a fixed XRAM base. Its own comment is the contract:

> *"The compile-time map below IS the contract — NovaHost's boot manifest MUST stage
> each module to the same slot base (single source of truth) … Phase 1/System-B
> replaces this with an XRAM-directory lookup by name (modtab filled at boot)."*

Shelf constants (`runtime/asm/libabi.inc`): `SHELF_BASE = $060000`,
`SHELF_SLOT_WORDS = $2000` (16 KB/slot). The firmware
(`novahost.ino : stageConfiguredLibraries`) reads each `boot.json` `libraries[]`
entry's `base`/`size` and streams the file to that XRAM address at boot. An entry
with `size == 0` (or no `base`) is silently skipped.

Problems: (a) the id→slot map is hand-maintained and collision-prone; (b) `base`
is duplicated between asm and JSON with no enforcement; (c) total modules are
capped at the number of compile-time map rows; (d) nothing upstream (CLI, web) can
correctly invent a `base`, which blocks the web staged-toggle.

## 2. Architecture — a three-tier cache

| Tier | Holds | Tracked by | Fill cost |
|------|-------|-----------|-----------|
| Bank-1 `$C000` window | **1** active module | `LIB_RESIDENT` (`$0318`) | XRAM slot → bank (PGD, every call) |
| **XRAM shelf** | **N=4** cached modules | **new RAM directory** | SD → XRAM slot (miss only) |
| SD `/lib/*.nmod` | all modules | host `libraries[]` catalog | — |

`id` is decoupled from slot. The **6502 owns the cache** (directory in RAM, LRU
policy, allocation); the **host is the backing store** (resolves `id → /lib/file`
and streams SD → XRAM on request). Cache *hits never touch the host*.

**Identity = `(MOD_MOD_ID $C005, MOD_VERSION $C006)`** — both bytes live in every
module image and are runtime-checkable (`LERR_BAD_VER` already exists). The NDOC
semantic version ("1.0") stays display-only. The host validates `(id, version)`
against the file whenever it streams a slot; the 6502 cache tag stores **id only**
(1 byte/slot).

## 3. The directory

For `N = 4` slots, in the resident loader band (`$0320–$041F`, 99 bytes free after
the 157-byte loader):

- `shelf_tag[N]` — module id resident in each slot (`$00` = empty): **4 bytes**.
- `shelf_lru[N]` — slot indices in MRU→LRU order; for tiny N this *is* exact LRU,
  no timestamps, no saturation: **4 bytes**.
- `base` is **not stored**: slot *i* ⇒ `base = SHELF_BASE + i*$4000` (computed).

**Total: 8 bytes.** Cost is `2N`; even N=8 is 16 bytes. New symbols go in
`libabi.inc` (`SHELF_N`, `SHELF_TAG`, `SHELF_LRU`). N is a single constant,
trivially raised later (the region `$060000–$07BFFF` holds up to 7 slots before
`HOST_EXT_XRAM $07C000`; `SID_CURVE $080000` is untouched).

## 4. Hit path (every `lib_call`)

`lib_call` (`JSR $0320`) reads the mailbox (`$0300`: mod id, fn id, args), then:

1. **Bank fast-path:** if requested id `== LIB_RESIDENT`, the module is already in
   the `$C000` window → skip straight to the JSR. (No directory work, no page-in.)
2. **Shelf scan:** linear-scan `shelf_tag[0..N)` for the id (≤4 compares).
   - **Hit:** slot *i* found → move *i* to front of `shelf_lru[]` (≤4-byte shuffle,
     ~20 cycles) → PGD page-in (`base = SHELF_BASE + i*$4000` → bank-1) → set
     `LIB_RESIDENT` → JSR module dispatch.
   - **Miss:** §5 (Phase B) or `LERR_BAD_MODULE` (Phase A stub).

LRU bookkeeping is the only added per-call work and is negligible against the
PGD page-in. Modules are immutable code, so there is **no write-back** — eviction
is a plain overwrite.

## 5. Miss / eviction handshake (Phase B)

On a shelf miss the working set has exceeded N and the module must be streamed from
SD. Reuses the existing FIO async-event substrate (`streamSdramAsset`,
`HOST_STATUS_FIO_ACTIVE/ERROR` = `0x02`/`0x04`, "FIO events execute immediately"):

1. Victim = back of `shelf_lru[]`; `dest_slot = victim`,
   `target_base = SHELF_BASE + victim*$4000`.
2. 6502 writes FIO `LOAD_MODULE{ id, dest_slot }` to the FIO registers (`$B9A0+`).
3. FPGA raises the async FIO event → ESP services it: resolves `id → /lib/file`
   from its `libraries[]` catalog, streams the first 16 KB SD → SDRAM@`target_base`
   via `streamSdramAsset`, validating magic / id / **version** as it goes. Sets
   `FIO_ACTIVE` while busy; `FIO_ERROR` on any failure.
4. 6502 spins on `HOST_STATUS` until done. `FIO_ERROR` → `LIB_STATUS =
   LERR_BAD_MODULE`, RTS. (Busy-wait stall = the accepted miss-path cost.)
5. Commit: `shelf_tag[victim] = id`; move victim to front of `shelf_lru[]`.
6. Fall through to the hit path (PGD page-in, `LIB_RESIDENT`, JSR).

**RTL boundary (resolve in planning):** determine whether `LOAD_MODULE` can ride
`fio.sv`'s existing command passthrough (firmware-only, no synth) or needs a new
register/decode (one RTL change + synth). Spec the command either way.

## 6. `boot.json` schema change

`libraries[]` **drops `base`**; entries become:

```json
{ "name": "graphics", "id": 1, "version": 1, "path": "/lib/graphics.nmod", "size": 16384 }
```

- Firmware **Phase A**: stage `libraries[i]` to **slot = i** at boot, then seed the
  6502 `shelf_tag[]` (`= [id0, id1, …, 0]`) and `shelf_lru[]` via `CMD_POKE_BLOCK`
  (the same path that installs the loader, during CPU reset-hold).
- Firmware **Phase B**: also keep the `id → path/size/version` catalog in RAM to
  service `LOAD_MODULE` at runtime.
- Backward compatibility: a legacy entry that still carries `base` is accepted and
  ignored (slot index wins). `version` defaults to the binary's `$C006` byte if
  absent.

## 7. Phase split

**Phase A — dynamic directory, boot-time allocation (zero RTL):**
- `libabi.inc`: `SHELF_N`, `SHELF_TAG`, `SHELF_LRU`.
- `libcall.s`: replace `modtab_lookup` cmp/beq with the §4 scan + LRU reorder;
  miss → `LERR_BAD_MODULE` stub.
- `novahost.ino`: slot = array index; seed `shelf_tag[]`/`shelf_lru[]`; drop `base`.
- Web (`NovaWebServer.cs`): `SetLibraryEntry` writes `{name,id,version,path,size}`
  (no base). PUT / DELETE / unstage already built this session.
- Emulator (`CompositeBusDevice`): mirror the shelf in the page-in path so the
  directory/LRU logic is exercised without hardware.

Delivers: collision gone, web toggle correct, no synth. Valid because the initial
runtimes' working set ≤ N.

**Phase B — runtime miss/eviction (the grand vision):**
- `libcall.s`: implement the §5 miss handler + eviction.
- FIO `LOAD_MODULE` command (firmware ± a small `fio.sv` touch).
- `novahost.ino`: runtime load handler + `id→file` catalog.
- Verilator + HW verification if RTL is touched.

Purely additive — the Phase-A directory already supports it.

## 8. Error handling

| Condition | Detection | Result |
|-----------|-----------|--------|
| Unknown id, no SD entry (Phase A) | shelf miss, no catalog | `LIB_STATUS = LERR_BAD_MODULE` |
| SD file missing / short (Phase B) | host stream fails | `FIO_ERROR` → `LERR_BAD_MODULE` |
| Magic ≠ `NL` | host validates `$C003` | `FIO_ERROR` → `LERR_BAD_MAGIC` (host maps) |
| id / version mismatch | host validates `$C005/$C006` | `FIO_ERROR` → `LERR_BAD_VER` |
| Web stage of invalid `.nmod` | `NovaModule.ValidateForStaging` | HTTP 400, never written (built) |

Graceful degradation is preserved end-to-end: a failed load never wedges the CPU
(it returns an error code); a failed boot pre-stage logs and continues
(unchanged firmware contract).

## 9. Testing

- **Directory/LRU (Phase A):** TDD in the emulator. Add a shelf model to
  `CompositeBusDevice` (it already has `PokeResidentLoader`); unit-test scan, hit
  reorder, victim selection, seed-from-host. MSTest.
- **6502 loader:** assemble `libcall.s`, drive `lib_call` sequences through the
  emulator, assert slot residency + `LIB_RESIDENT` + result mailbox.
- **Web:** `NovaModule` + endpoint shape (`ValidateForStaging` tests already green);
  HW round-trip against `novahost.local` with a throwaway module (snapshot + restore
  `boot.json`).
- **Phase B:** verilator for `LOAD_MODULE` if `fio.sv` changes; HW miss/eviction
  with a working set > N.

## 10. Non-goals / YAGNI

- No write-back (modules are immutable code).
- No approximate LRU (exact is 4 bytes for N=4).
- No per-call host round-trip (hits stay on-chip).
- No module hot-reload / versioned coexistence beyond `(id, version)` validation.
- N stays 4; raising it is a one-constant change, deferred until a runtime needs it.

# BASIC-on-Modules — Phase 0 (Infra + Coexistence Bridge) Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Wire `lib_call` into the BASIC boot and make BASIC's extension-ROM trampoline survive a module page-in, so converted (`lib_call`) and un-converted (`EXT_CODE`) keywords can coexist during the incremental migration — with ZERO keywords converted yet and the full BASIC suite still green.

**Architecture:** BASIC keeps its static extension ROM for now. A new ROM routine `ensure_ext_resident` re-pages the extension ROM from XRAM `$07C000` into bank 1 whenever a prior `lib_call(MODULE)` clobbered it (tracked by `LIB_RESIDENT`). The `EXT_CODE` RAM trampoline calls it before swapping to the extension ROM. Boot arms `LIB_HOME_BANK=ROMSWAP_BASIC` and `LIB_RESIDENT=$FF`. The `lib_call` loader (`libcall.bin` @ `$0320`) and module shelf are already staged unconditionally by `CompositeBusDevice`, so no host changes are needed for Avalonia.

**Tech Stack:** ca65/ld65 (6502 asm), `ehbasic/` ROM sources, MSTest (`e6502UnitTests`), `make -C ehbasic` build.

**Design doc:** `docs/plans/2026-06-06-basic-on-modules-design.md`

**Reference facts (verified):**
- `libabi.inc`: `PGD_CMD=$BA76`, `PGD_SRCL/M/H=$BA78/79/7A`, `PGD_WORDSL/H=$BA7B/7C`, `PGD_START=$01`; `HOST_EXT_XRAM_L/M/H=$00/$C0/$07` ($07C000); `LIB_RESIDENT` mailbox byte, `LIB_RESIDENT_HOSTEXT=$FF`; `LIB_HOME_BANK`; extension ROM = 16384 B = `$2000` words.
- `nova.inc`: `REG_ROMSWAP=$A03F`, `ROMSWAP_BASIC=$02`, `ROMSWAP_EXTENSION=$04`.
- `ehbasic/basic.asm:302`: `ExtCmdId=$E4`. `LAB_COLD` @ `basic.asm:630`. `EXT_vec=$0226`.
- `ehbasic/min_mon.asm:65` `EXT_CODE` trampoline is exactly 18 bytes (`EXT_RST=EXT_vec+$12`). Bridges follow at `+$08` then `+$0E` each.
- `ehbasic/extension.s:43-47` hardcodes the bridge RAM addresses as literals: `EXT_RESET_VEC=$0233`, `EXT_GTBY_VEC=$0240`, `EXT_GTWRD_VEC=$024E`, `EXT_GTSW_VEC=$025C`, `EXT_SNERR_VEC=$026A`. These MUST stay in lockstep with `min_mon.asm`'s layout.
- `libcall.s` loader band is FULL (248 B, ends `$0417`) — `ensure_ext_resident` must NOT go there; it lives in the BASIC ROM.
- Build: `make -C ehbasic` produces `basic.bin` + `extension.bin`; the Avalonia build copies them to `Resources/`. Tests do NOT auto-rebuild the ROMs — must `make -C ehbasic` then copy to `e6502.Avalonia/Resources/` before `dotnet test`.
- Test harness pattern: `e6502UnitTests/BasicRegressionTests.cs:676` `RunProgram(string[] lines)` → returns screen text.

---

## Task 0: Lock the baseline

**Step 1:** Build the ROMs and run the full BASIC suite to confirm a green starting point.

Run:
```bash
make -C ehbasic && \
cp ehbasic/basic.bin e6502.Avalonia/Resources/ehbasic.bin && \
cp ehbasic/extension.bin e6502.Avalonia/Resources/extension.bin && \
dotnet test --filter "FullyQualifiedName~Basic" 2>&1 | tail -5
```
Expected: PASS, 0 failed. Record the pass count (the no-regression gate).

**Step 2:** Capture current ROM free space (the metric we improve later):
```bash
grep -nE "CODE " ehbasic/basic.map ehbasic/extension.map | head
```
Expected: main `CODE Size=$3FD3` (≈4 bytes free). Note both ROMs' sizes.

**Step 3:** Commit nothing (baseline only).

---

## Task 1: Add `ensure_ext_resident` to the BASIC ROM (re-page routine)

**Files:**
- Modify: `ehbasic/basic.asm` (Nova handler area — place near the other Nova `LAB_*`/VGC helpers, e.g. just before `LAB_vec`-adjacent code around `basic.asm:9017`; it must be in the BASIC ROM CODE segment, NOT in the RAM-copied `LAB_vec..END_CODE` block in min_mon).
- Verify include: `ehbasic/basic.asm` must `.include "libabi.inc"` (for `PGD_*`, `HOST_EXT_XRAM_*`, `LIB_RESIDENT`, `LIB_RESIDENT_HOSTEXT`). If not present, add it next to the existing `.include "nova.inc"`.

**Step 1: Confirm the symbols resolve.** Grep that `basic.asm` (directly or transitively) sees `libabi.inc`:
```bash
grep -nE '\.include "(libabi|nova)\.inc"' ehbasic/basic.asm
```
If `libabi.inc` is missing, add `      .include "libabi.inc"` immediately after the `nova.inc` include.

**Step 2: Write the routine.** Insert into the Nova area of `basic.asm`:
```asm
; ---------------------------------------------------------------------
; ensure_ext_resident — re-page the host extension ROM into bank 1 from
;   XRAM $07C000 if a prior lib_call(MODULE) paged a module over it.
;   Called from the EXT_CODE RAM trampoline while BASIC ROM is mapped, before
;   the swap to ROMSWAP_EXTENSION. The PGD page-in writes the bank-1 overlay
;   regardless of what is currently mapped at $C000.
;   LIB_RESIDENT == $FF means the ext ROM is already resident -> no-op.
;   Clobbers A. (ExtCmdId is already saved in ZP by the caller.)
; ---------------------------------------------------------------------
ensure_ext_resident:
      LDA   LIB_RESIDENT
      CMP   #LIB_RESIDENT_HOSTEXT     ; $FF = host ext already in bank 1
      BEQ   @eer_done
      LDA   #HOST_EXT_XRAM_L          ; $00
      STA   PGD_SRCL
      LDA   #HOST_EXT_XRAM_M          ; $C0
      STA   PGD_SRCM
      LDA   #HOST_EXT_XRAM_H          ; $07  -> source $07C000
      STA   PGD_SRCH
      LDA   #$00                      ; 16KB ext ROM = $2000 words
      STA   PGD_WORDSL
      LDA   #$20
      STA   PGD_WORDSH
      LDA   #PGD_START
      STA   PGD_CMD                   ; page-in; CPU stalls until copied
      LDA   #LIB_RESIDENT_HOSTEXT
      STA   LIB_RESIDENT              ; bank 1 now holds the host ext again
@eer_done:
      RTS
```

**Step 3: Assemble — verify it builds and the symbol resolves.**
```bash
make -C ehbasic 2>&1 | tail -5 && grep -nE "ensure_ext_resident" ehbasic/basic.map
```
Expected: build OK; `ensure_ext_resident` present in the map at a `$C000`-range address.

**Step 4: Commit.**
```bash
git add ehbasic/basic.asm && git commit -m "feat(basic): add ensure_ext_resident re-page routine (dormant)"
```
Note: dormant — nothing calls it yet, so the suite is unaffected. (Quick sanity: `dotnet test --filter "FullyQualifiedName~Basic"` should still pass after copying the new `basic.bin`.)

---

## Task 2: Arm `lib_call` in BASIC's cold start

**Files:**
- Modify: `ehbasic/basic.asm` `LAB_COLD` boot path (around `basic.asm:630`–`710`). Find the Nova-specific init that runs once at cold start (e.g. where VGC/device state is set up after the PG2_TABS bulk-init). Add the arming there.

**Step 1: Locate the Nova cold-start init block.**
```bash
sed -n '630,712p' ehbasic/basic.asm
```
Identify a point AFTER the page-2 bulk init (`PG2_TABS`) where it is safe to write the mailbox bytes `LIB_HOME_BANK`/`LIB_RESIDENT` (both in the `$0300` band, which is carved cross-runtime).

**Step 2: Add the arming code** at that point:
```asm
      ; Arm the paged-library ABI: BASIC is the home bank, and at boot the static
      ; extension ROM is resident in bank 1 (host staged it). lib_call swaps back
      ; to LIB_HOME_BANK after each module call; ensure_ext_resident uses
      ; LIB_RESIDENT to know when a module has displaced the ext ROM.
      LDA   #ROMSWAP_BASIC
      STA   LIB_HOME_BANK
      LDA   #LIB_RESIDENT_HOSTEXT
      STA   LIB_RESIDENT
```

**Step 3: Build + copy + run the full suite (no-regression gate).**
```bash
make -C ehbasic && \
cp ehbasic/basic.bin e6502.Avalonia/Resources/ehbasic.bin && \
cp ehbasic/extension.bin e6502.Avalonia/Resources/extension.bin && \
dotnet test --filter "FullyQualifiedName~Basic" 2>&1 | tail -5
```
Expected: PASS, same count as Task 0 (arming is invisible — no keyword calls `lib_call` yet).

**Step 4: Commit.**
```bash
git add ehbasic/basic.asm e6502.Avalonia/Resources/ehbasic.bin && \
git commit -m "feat(basic): arm lib_call (LIB_HOME_BANK/LIB_RESIDENT) at cold start"
```

---

## Task 3: Make `EXT_CODE` re-page-safe (grow trampoline + shift bridges in lockstep)

**Files:**
- Modify: `ehbasic/min_mon.asm:65` (`EXT_CODE`) and the layout offset defs `min_mon.asm:15-19`.
- Modify: `ehbasic/extension.s:43-47` (the hardcoded bridge VEC literals) — must match the new layout.

**Background:** `EXT_CODE` is exactly 18 bytes; adding `JSR ensure_ext_resident` (3 bytes) overflows into the bridge slots. The bridges live at fixed RAM addresses hardcoded in BOTH `min_mon.asm` (relative) and `extension.s` (absolute). We bump the layout by a clean `+$08` (8 bytes of headroom for EXT_CODE) and update both files in lockstep.

**Step 1: Bump the layout offsets in `min_mon.asm:15-19`.** Change `EXT_RST` base from `EXT_vec+$12` to `EXT_vec+$1A` (the rest are relative and follow):
```asm
EXT_RST     = EXT_vec+$1A     ; extension ROM reset recovery in RAM (was +$12; +8 for larger EXT_CODE)
EXT_GTBY    = EXT_RST+$08     ; bridge: extension → BASIC LAB_GTBY → extension
EXT_GTWRD   = EXT_GTBY+$0E    ; bridge: extension → BASIC LAB_GTWRD → extension
EXT_GTSW    = EXT_GTWRD+$0E   ; bridge: extension → BASIC LAB_GTSW → extension
EXT_SNERR   = EXT_GTSW+$0E    ; bridge: extension → BASIC LAB_15D9 (syntax error)
```
New addresses: `EXT_RST=$0240`, `EXT_GTBY=$0248`, `EXT_GTWRD=$0256`, `EXT_GTSW=$0264`, `EXT_SNERR=$0272`.

**Step 2: Add the `JSR` to `EXT_CODE` (`min_mon.asm:65`).** New body (21 bytes, fits the $1A=26-byte budget):
```asm
EXT_CODE
      STA   ExtCmdId          ; save command ID to ZP
      JSR   ensure_ext_resident   ; re-page host ext if a module clobbered bank 1
      LDA   #ROMSWAP_EXTENSION
      STA   REG_ROMSWAP       ; swap to extension ROM
      JSR   $C000             ; call extension entry point
      PHA                     ; save handler return value
      LDA   #ROMSWAP_BASIC
      STA   REG_ROMSWAP       ; swap back to BASIC ROM
      PLA                     ; restore handler return value
      RTS
```

**Step 3: Update `extension.s:43-47` to the new addresses (lockstep):**
```asm
EXT_RESET_VEC   = $023B         ; reset recovery routine in RAM (was $0233; +8)
EXT_GTBY_VEC    = $0248         ; bridge: extension → BASIC LAB_GTBY → extension
EXT_GTWRD_VEC   = $0256         ; bridge: extension → BASIC LAB_GTWRD → extension
EXT_GTSW_VEC    = $0264         ; bridge: extension → BASIC LAB_GTSW → extension
EXT_SNERR_VEC   = $026A         ; (recompute) bridge: extension → BASIC LAB_15D9
```
NOTE: derive each literal from the new `min_mon` layout, not by blindly adding 8 — `EXT_RESET_VEC`/`EXT_SNERR_VEC` in `extension.s` and `EXT_RST`/`EXT_SNERR` in `min_mon` currently differ in base; reconcile them by computing the actual RAM address of each trampoline label from the new layout (assemble `min_mon` with a listing and read the real addresses — Step 5).

**Step 4: Verify the RAM block still fits below `Ibuffs`.** `Ibuffs=VEC_SV+$67`. Confirm `EXT_SNERR` + its 14-byte body ends below `Ibuffs`:
```bash
make -C ehbasic 2>&1 | tail -8
```
Expected: assembles with no overlap/range error.

**Step 5: Read the REAL trampoline addresses from the listing and reconcile `extension.s`.**
```bash
grep -nE "EXT_CODE|EXT_RESET_CODE|EXT_GTBY_CODE|EXT_GTWRD_CODE|EXT_GTSW_CODE|EXT_SNERR_CODE" ehbasic/min_mon.lst
```
Set each `*_VEC` literal in `extension.s` to the matching `*_CODE` RAM address. Rebuild until `extension.s`'s literals exactly equal the assembled trampoline addresses.

**Step 6: Build + copy + run the full suite.**
```bash
make -C ehbasic && \
cp ehbasic/basic.bin e6502.Avalonia/Resources/ehbasic.bin && \
cp ehbasic/extension.bin e6502.Avalonia/Resources/extension.bin && \
dotnet test --filter "FullyQualifiedName~Basic" 2>&1 | tail -5
```
Expected: PASS, same count as Task 0. (Every extension-ROM keyword still works AND `EXT_CODE` now re-pages — but since no `lib_call` has run, `LIB_RESIDENT` is still `$FF`, so the re-page is a no-op. The suite proves we didn't break the bridge layout.)

**Step 7: Commit.**
```bash
git add ehbasic/min_mon.asm ehbasic/extension.s e6502.Avalonia/Resources/ehbasic.bin e6502.Avalonia/Resources/extension.bin && \
git commit -m "feat(basic): EXT_CODE re-pages the ext ROM after a module page-in"
```

---

## Task 4: Prove the bridge with a dedicated test (lib_call then ext-ROM keyword)

This is the load-bearing test: it forces a real module page-in (clobbering `_extBank`), then exercises an extension-ROM keyword, and asserts the keyword still works — proving `ensure_ext_resident` restored the ext ROM.

**Files:**
- Create/Test: `e6502UnitTests/BasicModuleBridgeTests.cs`

**Step 1: Pick an extension-ROM keyword with a deterministic, screen-visible result.** Use a math-coprocessor function (extension ROM, no module) — e.g. `PRINT MUL(6,7)` or whichever math token exists. Confirm the token + expected output:
```bash
grep -nE "MUL|MDIV|MSIN|MCOS|ATAN2|^; .*math" ehbasic/extension.s | head
```
Pick one whose result is exact (integer), e.g. an integer multiply. Determine the exact BASIC syntax + expected printed value from the handler.

**Step 2: Write the failing test.** Adapt the shared-machine pattern (one `CompositeBusDevice`+`Cpu`+`ScreenEditor` instance) from `BasicRegressionTests.cs:676`. The test must, on a single machine instance:
  (a) drive a `lib_call(GRAPHICS, GFN_GCLS)` — the simplest module call — to force a page-in that displaces the ext ROM and sets `LIB_RESIDENT=$01`;
  (b) then run a one-line program using the chosen ext-ROM keyword;
  (c) assert the printed result is correct.

Mechanism for (a): POKE a tiny stub into free RAM that fills the mailbox and `JSR LIB_LOADER_BAND`, then run the CPU from it (follow the raw-routine execution pattern used in the CPU/`*Suite*` tests). Concretely:
```
; stub @ e.g. $0820
LDA #MODULE_ID_GRAPHICS : STA LIB_MOD_ID    ; $0300
LDA #GFN_GCLS           : STA LIB_FN_ID      ; $0301
JSR LIB_LOADER_BAND                          ; $0320
RTS
```
Assert after (a): `ReadRam(0x0318) (LIB_RESIDENT) == 0x01` (a module is now resident — the ext ROM has been displaced).

```csharp
[TestMethod]
public void ExtKeyword_StillWorks_AfterModulePageIn()
{
    var bus = NewBasicMachineAtReadyPrompt();              // shared helper: boot BASIC to "Ready"
    ForceLibCall(bus, MODULE_ID_GRAPHICS, GFN_GCLS);       // POKE stub + run -> page-in
    Assert.AreEqual(0x01, bus.ReadRam(0x0318), "module should be resident, ext displaced");

    string screen = RunLineOnMachine(bus, "PRINT MUL(6,7)"); // ext-ROM keyword via EXT_CODE
    StringAssert.Contains(screen, "42", "ext ROM result wrong -> ensure_ext_resident failed to re-page");
}
```

**Step 3: Run it — verify it FAILS on a build WITHOUT the fix.** To prove the test has teeth, temporarily revert Task 3's `JSR ensure_ext_resident` (or stub `ensure_ext_resident` to an immediate `RTS`), rebuild, run:
```bash
dotnet test --filter "FullyQualifiedName~ExtKeyword_StillWorks_AfterModulePageIn" 2>&1 | tail -5
```
Expected: FAIL (the ext keyword runs the leftover GRAPHICS module instead of the ext ROM → wrong/garbage output). Restore the fix.

**Step 4: Run it with the fix — verify PASS.**
```bash
make -C ehbasic && cp ehbasic/basic.bin e6502.Avalonia/Resources/ehbasic.bin && cp ehbasic/extension.bin e6502.Avalonia/Resources/extension.bin && \
dotnet test --filter "FullyQualifiedName~ExtKeyword_StillWorks_AfterModulePageIn" 2>&1 | tail -5
```
Expected: PASS.

**Step 5: Run the full BASIC suite — confirm still green.**
```bash
dotnet test --filter "FullyQualifiedName~Basic" 2>&1 | tail -5
```
Expected: PASS, baseline count + 1.

**Step 6: Commit.**
```bash
git add e6502UnitTests/BasicModuleBridgeTests.cs && \
git commit -m "test(basic): prove ext ROM survives a module page-in (coexistence bridge)"
```

---

## Phase 0 Done — Definition of Done

- `ensure_ext_resident` exists in the BASIC ROM; `EXT_CODE` calls it; boot arms `LIB_HOME_BANK`/`LIB_RESIDENT`.
- The bridge test proves an extension-ROM keyword works AFTER a real `lib_call` page-in (and fails without the fix).
- Full BASIC suite green; no keywords converted yet (the bridge is invisible to existing behavior).
- ROM sizes recorded as the baseline for the per-domain savings in Phases 1+.

**Next:** Phase 1 (graphics) — add the shared `basic_lib_call` marshalling helper, convert PLOT/LINE/CIRCLE/etc. to `lib_call(GRAPHICS,…)`, drop the `vgc.s`+`sprite.s` includes from `basic.asm`, report bytes freed. Also start `docs/plans/ndk-harvest-candidates.md` (verify vsprite collision / MML reuse).

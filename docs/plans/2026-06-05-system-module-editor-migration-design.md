# System Module + Editor Migration (Phase A)

**Date:** 2026-06-05
**Status:** Approved, implementing
**Part of:** Logo ROM consolidation (evict non-Logo code to shared modules, repatriate the turtle, delete the extension ROM).

## Goal

Move the interactive text editor (`editui` + `editbuf` + `vtext` + `blitter`) out of the
NovaLogo extension ROM and into a new shared **`system` module** (`MODULE_ID_SYSTEM = $03`,
already reserved in `runtime/asm/libabi.inc`). Re-point Logo's `EDIT`/`TO` flow from the
extension trampoline to `lib_call(SYSTEM, SYS_FN_EDIT)`. Behavior-preserving; tests green throughout.

This is Phase A of the consolidation. The extension ROM is **not** deleted here — it still serves
TONE/NOISE/VOLUME/WAIT/WAITVBL/TIMER until Phases B/D. Only the editor leaves and only the EDIT
call site moves.

## Why this is clean (no new ABI)

- The editor is already **language-neutral** (`editbuf.inc`: "knows nothing about Logo, BASIC,
  or any document model"). It operates on a flat text buffer in low RAM via a pointer, with
  language behavior supplied through hook vectors.
- Logo's SAVE hook (`extension.s:282 ext_edit_save_hook`) is a **no-op flag-setter**. The Logo
  procedure record is built in the **base ROM after the editor returns**
  (`procedures.s:380-384`: `proc_extract_body` / `proc_build_record`).
- Therefore the editor never calls back into the runtime during editing. It runs entirely within
  one `lib_call` (keyboard + screen are MMIO, which survive the bank swap) and returns
  `exit-reason + save-flag + final-length` via the mailbox. The existing buffer-in/buffer-out
  `lib_call` contract fits exactly — **no module→runtime callback ABI is required.**
- The module/shelf/`lib_call` stack is already built and tested (`LibLoaderBusTests`,
  `CompositeLoaderTests`, `ShelfMissTests`, `BootLibrariesTests`, `GraphicsModuleTests`,
  `NovaLogoLibCallGraphicsTests`). The `graphics` module is the working template; Logo already
  marshals lib_calls. We add one module and re-point one call site.

## Change set

### New files (`modules/system/`, mirroring `modules/graphics/`)
- `system.cfg` — linker config: CODE @ `$C000`, MODBSS band, VECROM stub.
- `Makefile` — `ca65` → `ld65` → `system.bin`, then `nmod_pack.py` → `system.nmod`.
- `system.s` — module header (`lib_module_header MODULE_ID_SYSTEM, LIB_ABI_VERSION, SYS_FN_COUNT`)
  + fn-id dispatch; `.include`s the editor stack. `SYS_FN_EDIT` is a near-verbatim port of
  `ext_edit` (snapshot display → copper off → full 80×25 text window → editor colors → configure
  `editbuf` from `LIB_ARG0..3` → `editbuf_run` → restore display). The no-op SAVE-hook stub lives
  inside the module.
  - **Includes:** `nova.inc` + `vgc.inc` (VGC/TEXTWIN register *equates* — constants only),
    `copper.s` (for `copper_off`), `editui.s` (→ `vtext.s` → `blitter.s`), `editbuf.s`.
    **NOT `vgc.s`** — verified the editor calls zero `vgc.s` routines and writes VGC MMIO
    registers directly. The only NVR user in the path is a transient `NVR7L` ($2E) inside
    `copper_off`, well clear of the editor ZP window.

### New include
- `runtime/asm/libsystem.inc` — `SYS_FN_EDIT = $00`, `SYS_FN_COUNT = $01`, NDOC/arg contract:
  - In:  `ARG0` = buffer ptr, `ARG1` = current length, `ARG2` = capacity,
         `ARG3` = NUL-terminated title ptr; initial cursor offset in the spare arg bytes.
  - Out: `RESULT` lo = editbuf exit reason; a save-requested flag; `ARG1` = final length.

### Logo wiring (`novalogo/`)
- `procedures.s:369-373`: replace `ensure_ext_resident` + `EXT_TRAMPOLINE` (cmd `$4B`) with the
  `lib_call(MODULE_ID_SYSTEM, SYS_FN_EDIT)` sequence, mapping the editor args from the
  `EXT_ARG*`/`ext_iface` mailbox onto `LIB_ARG0..3` using the same marshalling shim Logo already
  uses for graphics lib_calls. Everything after (`proc_extract_body` / `proc_build_record` /
  `proc_edit_restore_split`) is unchanged.
- `novalogo/extension.s`: drop `.include "editui.s"` / `.include "editbuf.s"` (and the now-unused
  `ext_edit` + `ext_edit_save_hook` + `ee_saved_*` BSS); point cmd `$4B` at `ext_unsupported`.
  Extension shrinks ~4 KB.

### Registration / harness (decision: auto-stage in the bus — tests + GUI)
- **`CompositeBusDevice` pre-stages modules at boot** (`StageConfiguredModules`), mirroring
  firmware `boot.json` `libraries[]`: graphics (`$01`) → shelf slot 0, system (`$03`) → slot 1,
  loaded from `Resources/{graphics,system}.bin` (absent → silently skipped). This makes
  `lib_call(GRAPHICS)`/`lib_call(SYSTEM)` resolve in both unit tests AND the real Avalonia GUI —
  it also closes the graphics module's pre-existing GUI-staging gap.
- **Build plumbing:** `modules/{graphics,system}/Makefile` gain an `install` target copying the
  16 K `.bin` into `e6502.Avalonia/Resources/`; the Avalonia `AssembleRom` target rebuilds both
  modules and copies them; `<None CopyToOutputDirectory=Always>` + `CopyEhBasicToOutput` push them
  to the app output, and they flow to the **test** output transitively via the project reference
  (same as `novalogo.bin`). `system.bin`/`graphics.bin` get committed under `Resources/` (the
  `novalogo.bin` model) so Windows CI needs no module rebuild.
- `boot.json` `libraries[]`: add `{ "name": "system", "id": 3, "version": 1,
  "path": "/lib/system.nmod", "size": 16384 }` for the real firmware/HW (separate from the
  emulator path above).

## Test strategy (TDD)
1. **`SystemModuleTests` (format):** assert `system.bin` header — magic `"NL"`, id `$03`, ABI
   version, `fn_count`, dispatch reachable. Mirrors `ModuleFormatTests`.
2. **`NovaLogoLibCallSystemTests` (round-trip):** drive `TO FOO … END` / `EDIT "FOO`, inject keys,
   assert the buffer round-trips through `lib_call(SYSTEM, SYS_FN_EDIT)` and the proc record builds.
   Mirrors `NovaLogoLibCallGraphicsTests`.
3. **Regression:** existing NovaLogo editor tests pass unchanged through the new path.

## Implementation order
1. `libsystem.inc` (contract) + `SystemModuleTests` format test (red → build skeleton → green).
2. `modules/system/{system.cfg,Makefile,system.s}` skeleton with header + dispatch + a stub
   `SYS_FN_EDIT` returning a sentinel; format test green.
3. Port `ext_edit` body into `SYS_FN_EDIT`; `.include` editor stack; resolve ZP band + MODBSS fit.
4. `NovaLogoLibCallSystemTests` (red), then re-point `procedures.s` EDIT → `lib_call`; green.
5. Strip editor from `extension.s`; full suite green.
6. Register in boot.json + harness; build both `.bin` and `.nmod`.

## ZP plan (resolved)
The editor's ZP is exactly **25 bytes** (editbuf 14 + editui 6 + vtext 5; blitter 0; extension.s
itself 0 — the extension's 25-byte $A3–$BB ZEROPAGE *is* the editor). Place the module's ZEROPAGE
at **$A3–$BB**, mirroring the proven extension layout: clear of the loader band ($14–$17), clear of
NVR scratch ($20–$2F), and volatile-across-`lib_call` for Logo. The editor path uses no NVR except a
transient `NVR7L` ($2E) in `copper_off`, so there is no collision. (The formal cross-runtime module
ZP band remains deferred, exactly as graphics' provisional $14–$1C placement is.)

## Risks (resolve at build, not design blockers)
- **MODBSS fit:** editor BSS (~466 B in the extension, ex-`ee_saved_*`) moves into the MODBSS band
  (`$0420-$08FF`, ~1.2 KB) — *better* than the extension's $9800 (which sat in Logo heap space).
  Fits; confirm via the linker map.
- **Include guards:** `system.s` must `.include` `nova.inc`, `vgc.inc`, `copper.s`, `editui.s`,
  `editbuf.s` with guards intact (no double-definition vs the module header includes). NOT `vgc.s`.

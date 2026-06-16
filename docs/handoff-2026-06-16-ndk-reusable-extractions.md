# Handoff — NDK reusable-code extractions (remaining)

Date: 2026-06-16
Branch: `fix/65c02-audit-bugs-1-5`
Related: `docs/plans/ndk-harvest-candidates.md` (existing harvest list), the
65C02 audit memory (`project-65c02-audit-2026-06-16`).

## What this is

The 65C02 audit produced a list of duplicated/reusable 6502 code. The first
reusable extraction shipped this session (`vtext_print_at`). This doc hands off
the **remaining** reusable-into-NDK extractions, the harness each needs, and the
rule that decides which dedups are worth doing.

## Already shipped (don't redo)

- `aa40c9f` — 5 bug fixes (editbuf goto-line + cut, vsprite Y-underflow, NIC
  DMAERR, audio LOADING). TDD fixtures: `editbuf/vsprite/nicdma/audioload_runtime`.
- `dd8374a` — **`vtext_print_at`** added to the NDK (`runtime/asm/vtext.s`): the
  reusable "set cursor to VTEXT_CURX/CURY, print a NUL-terminated region-aware
  string" primitive (A/Y = string ptr). `editui_print_ptr` now delegates to it.
  Also adds the **`VgcVramBus`** C# test double
  (`e6502UnitTests/VtextPrintAssemblyRuntimeTests.cs`) modelling the VGC VRAM
  plane interface ($A0E0–$A0E4) — reusable for any vtext-output test.

## Test foundation in place

- **Flat-bus asm fixtures**: `tests/integration/asm/<name>_runtime.s` (+ `.cfg`,
  Makefile rule) → `tests/integration/fixtures/<name>_runtime.bin`, driven by
  `e6502UnitTests/<Name>AssemblyRuntimeTests.cs`. Pattern: stub at $0200
  (`LDX #$FF;TXS;JSR entry;JMP $0206`), assert `RESULT_BASE` ($2200) bytes. Build:
  `PATH="/Users/barry/Git/cc65/bin:$PATH" make ../fixtures/<name>_runtime.bin`.
- **Fault/MMIO bus doubles** (custom `IBusDevice` over `byte[65536]`):
  `FaultNicBus`, `AudioLoadingBus`, `VgcVramBus`. Reuse / extend these.

## THE RULE (read before deduping anything)

The NDK (`runtime/asm/*.s`) is strip-based (ca65 reference-tracking): a program
includes only the `.s` it needs and emits only referenced routines, so every
`.s` must stay **independently includable**. Duplication between two peer
modules is often intentional — deduping by making module A call module B's
helper forces every A-user to link B.

- Only extract a shared helper if A already `.include`s B, or the shared code
  goes into a module both already include. Check with `grep '\.include' A.s`.
- Within-file dedup is always fine.
- Good pattern: promote a reusable primitive INTO a common base module callers
  already use (that's why `vtext_print_at` went into `vtext.s`).
- Already REJECTED for coupling (do not "fix"): `copper_set_sprite_reg` →
  `sprite_reg_offset` (copper.s doesn't include sprite.s); `nic_copy_name` ≈
  `fio_copy_name` (different subsystems); `msprite`→`sprite_reg_offset`
  (marginal 3-byte idiom).

## Pre-existing WIP caveat

At session start these had uncommitted WIP NOT ours (a "scroll hook" feature):
`runtime/asm/{fio,nui,vtext,nova,novavm,xram,libsystem,runtime_labels}*`.
`dd8374a` already folded the `vtext.s`/`vtext.inc` part in. **Before editing any
of those files, run `git status`** — edits there entangle with that WIP in the
same diff hunks. `editui.s`, `sprite.s`, `copper.s`, `msprite.s`, `blitter.s`,
`vgc.s` and the app dirs were clean.

---

## Remaining extractions

### A. Chess `print_at` → `vtext_print_at`  (highest value)

- **Where**: `examples/novachess/src/runtime.s:1747` `print_at` (its own
  `msg_ptr` walk + `vtext_put_char` loop), ~12 `JSR print_at` call sites.
- **Plan**: delete the local loop; `print_at` sets `VTEXT_CURX/CURY` then
  `JMP vtext_print_at`. Note the current `print_at` ignores the put_char error
  return — `vtext_print_at` propagates it (skips on bad cursor), a minor
  improvement; confirm chess never prints out of region.
- **Coupling**: fine — chess already includes `vtext.s`.
- **Test**: chess is an app → needs the chess Smoke build (`make -C
  examples/novachess test`: smoke/mode-test/castle-test). Confirm board/status
  text still renders (existing `--expect-screen` assertions cover this).
- **Risk**: low. Files clean (not pre-modified).

### B. Chess `draw_square_outline` → `vgc_rect` + `vgc_wait_cmd`

- **Where**: `examples/novachess/src/runtime.s:2783` hand-pokes `VGC_P*` +
  `VCMD_RECT`/`VGC_CMD` and spins on `VGC_CMD AND #$01` (2816–2820) — exactly
  `vgc.s:401 vgc_rect` + `vgc_wait.s:21 vgc_wait_cmd`.
- **Plan**: `JSR vgc_wait_cmd` for the spin (cheap, link `vgc_wait.s`);
  optionally call `vgc_rect` for the rect issue.
- **Coupling**: chess does NOT currently include `vgc.s`/`vgc_wait.s` — adding
  `vgc_wait.s` (tiny, 27 lines) is acceptable; full `vgc.s` is heavier, judge.
- **Test**: chess Smoke `render-test` (gfx-color expectations on the outline).
- **Risk**: low–medium.

### C. NUI `nui_print_ptr` → new `vtext_put_block` primitive

- **Where**: `runtime/asm/nui.s:831` `nui_print_ptr` — manual char loop where
  CR/LF returns the cursor to `NUI_PRINT_X` (column-anchored multi-line block),
  unlike `vtext_puts` whose newline goes to region-left.
- **Plan**: add `vtext_put_block` to `vtext.s` (NDK): print a NUL string,
  CR/LF returns to a caller-supplied start column (pass column in a VTEXT field
  or reuse VTEXT_CURX as the anchor). `nui_print_ptr` then delegates; chess
  multi-line text can adopt too.
- **Coupling**: fine (nui already includes vtext.s). **CAVEAT**: `nui.s`/`vtext.s`
  carry pre-existing WIP — combined-commit or coordinate.
- **Test**: `VgcVramBus` — print a 2-line block at column N, assert both lines
  start at column N.
- **Risk**: medium (semantics; verify against NUI dialogs).

### D. Extract `uisave.s` into the NDK  (biggest LOC win, ~150 lines)

- **Where**: `runtime/asm/editui.s:950–1100`
  (`editui_menu_save_under`/`editui_save_under`/`editui_menu_save_plane`/
  `restore_*`/`editui_menu_select_saved_region`) ↔ `runtime/asm/nui.s:153–316`
  (`nui_save_plane`/`nui_save_under`/`nui_restore_under`/...). Same 3-plane
  (CHAR/COLOR/TEXTATTR) VGC↔XRAM blit-staging algorithm; differ only in BSS var
  names and the XRAM save base.
- **Plan**: new `runtime/asm/uisave.s` + `uisave.inc` exposing
  `uisave_capture` / `uisave_restore` parameterised by (rect fields, XRAM save
  base). editui and nui set the params then call it. Both already depend on
  vtext/blitter/xram, so the shared module is includable by both.
- **Test**: BLIT-HEAVY — the XRAM saves no-op on the flat `BusDevice`. **Needs a
  new bus double**: model the blitter `BLT_*` regs + a backing XRAM array so a
  capture-then-restore round-trips a known plane pattern. Build this double FIRST
  (it also unblocks other blit/XRAM tests). Alternatively drive via
  `CompositeBusDevice` (real blitter + XRAM) — check `AvaloniaBlitterTests` for
  the setup.
- **Risk**: highest (visual save/restore; subtle). Do last, with the blit
  harness + a round-trip test.

---

## Suggested order (next session)

1. **A** (chess print_at) — high value, low risk, clean files, existing Smoke
   coverage. Warm-up.
2. **B** (chess vgc_rect/wait) — small, same Smoke harness.
3. Build the **blitter/XRAM bus double** (needed for D, reusable).
4. **D** (uisave.s) — the big one, with the new harness + round-trip test.
5. **C** (nui block-print) — coordinate with the nui/vtext WIP.

## Also next session (shares the heavier harness)

Bugs 6 & 7 (deferred): NovaZ `save_load_overlay.s` write paths skip the
`FIO_SIZE` check the read paths have (`save_load_write_file_block` /
`save_load_write_xram_file_block` `JMP fio_fwrite` with no check — mirror
`save_load_read_file_block`); `zobject.s:715` `zstory_read8` return unchecked
before `LDA XRAM_DATA`. Both are NovaZ-overlay-bound (pinned `runtime_abi.inc`),
so they need the full NovaZ build + `Nova.NovaZ.Smoke` harness with a
fault-injecting `FileIoController` (short write) / XRAM read fault — not the
flat-bus fixtures. Fixes themselves are trivial (mirror existing verified
patterns in the same files).

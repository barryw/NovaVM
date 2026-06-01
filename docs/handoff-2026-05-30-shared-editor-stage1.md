# NovaLogo Procedure Editor — Stage 1 Delivered + Stage 2/3 Plan

Date: 2026-05-30
Supersedes the open items in `docs/handoff-2026-05-30-novalogo-procedure-editor.md`.

## What shipped this session (Stage 1: the shared editor — DONE + PROVEN)

A complete, **language-neutral** text-editing engine layered on the EDITUI
chrome shell, with all language-specific behavior behind host hook vectors.
Built and proven in isolation on the real VGC (no Logo, no banks).

New / changed files:

- `runtime/asm/editbuf.s` / `editbuf.inc` — the editing engine + 3-choice modal.
- `runtime/asm/editui.s` / `editui.inc` — generalized the menu save/restore-under
  into reusable `editui_save_under` / `editui_restore_under` (rect in
  `EDITUI_MENU_SAVE_X/Y/W/H`); fixed the dirty-marker so clearing dirty erases a
  stale `*`; added key constants (`HOME/END/PGUP/PGDN/CTRL_V`).
- `assembly/apps/editbuf_test/` — standalone SYS harness (links editui+editbuf,
  test hooks, observable RAM) + `assembly/Makefile` `editbuf-test` target.
- `e6502UnitTests/Editor/EditBufVmTests.cs` — 14 Logo-independent execution
  tests (load the harness into RAM, drive keys, read VRAM). **14/14 green.**
- `e6502UnitTests/RuntimeLibraryAbiTests.cs` — 2 new ABI pins (editbuf engine +
  generalized save-under). **All green; no regressions (59→ updated suites pass).**
- `docs/assembly/editui.md` — documents EDITBUF + hooks + overlay save/restore.

### Capabilities (all tested)

- Flat `$0A`-delimited buffer; cursor as a 16-bit byte offset.
- Insert / newline / backspace (incl. line-join) / delete.
- Navigation: left/right/up/down/home/end/pgup/pgdn with sticky goal column.
- Horizontal + vertical scrolling within the framed body region.
- Selection + cut / copy / paste (clipboard).
- Dirty marker (`EDITUI_DIRTY`, shared) set on edit, cleared on successful save.
- `Ctrl-S` → SAVE hook (tri-state: installed-clean / draft-not-callable / error).
- `Ctrl-Q` → clean exit, or a true overlapping 3-choice restore-under dialog
  (Exit Anyway / Save First / Cancel) when dirty.

### Hook vectors (the language-specific seam — default to no-ops)

| Vector | When | Contract |
|---|---|---|
| `EDITBUF_SAVE_VECL/H` | Ctrl-S, dialog "Save First" | host validates/installs; returns A status + optional `EDITBUF_STATUS` message |
| `EDITBUF_INDENT_VECL/H` | after newline | host returns A = leading spaces (auto-indent) |
| `EDITBUF_HILITE_VECL/H` | per visible line, in render | host fills `EDITBUF_HL_COLORS` (syntax highlighting) |

### Bugs found + fixed during bring-up (worth remembering)

1. **Standalone apps must set the stack** (`LDX #$FF / TXS`) — `Cpu.Boot(pc)`
   sets PC but not SP; the first `JSR` corrupted the stack.
2. **Inverted 16-bit pointer-increment idiom** — `LDA L / BNE :+ / INC H / : INC L`
   bumps the high byte when L *is* 0 (before inc) instead of when it *wraps*.
   Triggered because `BUF=$0400` (low byte `$00`). Correct form is
   `INC L / BNE :+ / INC H`. (The decrement idiom was already correct.)
3. **`count_lines` clobbers `EB_SCRATCH`** which `move_down`/`page_down` were
   using to hold the target line — reorder so the target is set last.
4. **Dialog button byte-tables indexed by `index*2`** — dropped "Save First".
5. **Copy clamp `CMP #<CLIP_CAP+1` parsed as `CMP #1`** → clamped cliplen to 0.
6. **Dirty `*` not erased on clean redraw** — title band only stamped `*` when
   dirty; now always stamps `*` or space.

Tests use a fixed "settle" after queuing keys (the editor renders incrementally:
title first, body last) rather than polling an intermediate signal.

## Locked architecture for Stage 2/3 (per user direction)

1. **Editor is shared + reusable across runtimes.** No runtime-specific code in
   editbuf/editui. Logo, BASIC, Forth, etc. all reach it the same way.
2. **Heavy scratch lives in XRAM, baked into the editor.** The 3×1280 B
   save-under buffers (and clipboard / future undo history) move from main-RAM
   BSS to fixed XRAM addresses via the blitter (`BLT_SPACE_XRAM` dest) / DMA.
   This drops the editor's main-RAM footprint to ~150 B of scalar state + ~24 B
   ZP, so it is usable in any tight-RAM environment without per-host plumbing.
   - TODO: add `editui_save_under`/`restore_under` XRAM variants (or make the
     save space configurable); reserve an XRAM region (e.g. `$06_0000+`, which
     `xram.inc` lists as free staging). Verify the C# `VirtualBlitterController`
     supports VGC↔XRAM copies (or fall back to DMA via `xram.s`).
3. **Disk paging is the fallback, authorized as "lightning fast."** If a runtime
   is even tighter, page the editor code overlay in/out around the modal session
   (`runtime/asm/overlay.s`/`pager.s`). With XRAM scratch + `proc_body_buf` reuse
   the editor code (~9 KB) already fits the extension ROM's 11.4 KB free, so
   paging is not required for Logo specifically.

### RAM map facts (why XRAM/paging are needed)

- Base ROM ZP `$30-$7D`, BSS `$0280-$0C60`, code to `$FE96`, **320 B free**.
- `HEAP_START=$0400`, `HEAP_END=$9C00`; base BSS overlaps the bottom of the heap
  (pre-existing). Turtle state is pinned at `$9F00` (tests/SV/smoke hardcode it).
  MMIO begins at `$A000`. Extension BSS region `$9C00-$9FFF` has ~179 B free.
- => no free contiguous main RAM for ~4 KB of editor buffers; hence XRAM.

## Stage 2 — wire Logo `TO` to the shared editor

1. **Link** editui+vtext+editbuf into the **extension ROM** (`extension.s`
   includes; grow `extension.cfg` ZP region to ~`$20`; keep editor scalar BSS
   ≤ the ~150 B that fit, with heavy buffers in XRAM per above).
2. **`EXT_CMD_EDIT = $4B`** dispatch + `ext_edit` handler (next free slot; the
   dispatch table has no bounds check, so append with no gaps). `ext_edit`
   reads the request from the `$80-$A2` mailbox (buffer ptr = `proc_body_buf`,
   cap, len, title ptr), sets editbuf config + hook vectors, runs `editbuf_run`,
   returns the result in the mailbox.
3. **Cross-bank hooks.** The SAVE hook must run Logo (base ROM) validation. The
   host points `EDITBUF_SAVE_VECL/H` at a RAM trampoline (like the existing
   `EXT_TRAMPOLINE` at `$0270`, but ext→logo: swap to Logo bank, `JSR` validator,
   swap back, `RTS`). INDENT/HILITE hooks similarly, or keep them simple Logo
   tokenizer calls. editbuf publishes the current length to the mailbox before
   each hook so the Logo side can read the buffer.
4. **Source model includes `END`.** On `TO NAME :params`, base ROM builds the
   editor buffer as `TO NAME :params\n<body>\nEND` (reconstruct from the existing
   record for re-open, or a template for new). Validation (Logo side): first
   nonblank line is a matching `TO <name> ...`; an `END` line exists; brackets
   balanced; `tokenize_line` accepts every line; name/param names valid. Install
   the callable record (`proc_build_record`) **only** on success; always keep the
   draft.
5. **Per-procedure drafts in XRAM.** Keyed by name; reload on re-open so invalid
   text is never lost. (Single active draft is the minimum; user chose per-proc.)
6. **Display restoration (hard req).** Snapshot `VGC_MODE`, cursor x/y/enable,
   copper on/off + active list, `TEXTWIN_*`. Switch to text mode via `ext_ts`
   semantics (`copper_off` + `STZ VGC_MODE`) **without** GCLS/SYSRESET so gfx +
   sprites + turtle are untouched. On exit restore exactly (re-`setup_copper` +
   `prepare_split_text` if it was split). Never clear graphics.
7. **Replace** the compact editor (`proc_editor_*` in `procedures.s`) and rewrite
   the migrated NovaLogo VM tests to assert the EDITUI shell (menu/title+`*`/
   status/dialog/validation/draft-preservation), keeping the Ctrl-S/Ctrl-Q
   queue-driven flow already in `QueueProcedureDefinition`.

## Stage 3 — hardware-facing tests + docs

- Update `e6502.FPGA/test/test_novalogo_draw_top.sv` to confirm EDITUI markers in
  text VRAM + display/turtle restored after `TO`.
- Update `tools/run-novalogo-hardware-smoke.py` (NovaHost VRAM/cursor reads only;
  do **not** trust HDMI capture).
- Do not synth/flash unless a separate hardware bug is proven.

## How to build/test Stage 1

```bash
cd assembly && make editbuf-test            # rebuild the harness bin
cd .. && dotnet test e6502UnitTests/e6502UnitTests.csproj \
  --filter "FullyQualifiedName~EditBufVmTests"           # 14/14
dotnet test e6502UnitTests/e6502UnitTests.csproj \
  --filter "FullyQualifiedName~RuntimeLibraryAbiTests"   # ABI pins
```

---

## Progress update — 2026-05-30 (continued): XRAM save-under DONE

The "bake heavy scratch into XRAM" step is **implemented and verified** (all 14
EditBufVmTests still green). `editui_save_under`/`restore_under` now blit the
char/color/attr planes to/from XRAM staging at flat `$05_4000`
(`EDITUI_XRAM_SAVE_BASE` + plane*`EDITUI_MENU_SAVE_BYTES`; offset tables in
editui RODATA) instead of 3×1280 B of CPU BSS. The VM blitter is space-agnostic
and supports VGC↔XRAM (`BLT_SPACE_XRAM=$05`). Editor main-RAM BSS is now ~470 B.

### Concrete RAM-layout recipe for the extension link (next step)

Validated against maps; do these together (they interlock):

1. `novalogo/heap.s`: `HEAP_END` `$9C00` → `$9800` (reclaim 1 KB for editor BSS).
2. `e6502UnitTests/NovaLogoTests.cs`: `LogoHeapEnd = 0x9C00` → `0x9800`
   (BootShowsBanner checks free bytes against it).
3. `novalogo/extension.s` lines 55-96: convert the turtle `.segment "BSS"` block
   to **fixed equates** so it stays put while editor BSS is added:
   `turtle_source_shape=$9C00`, `_rotated=$9D00`, `_saved_bg=$9E00`,
   `TURTLE_X_FRAC=$9F00`, `_X_LO=$9F01 … turtle_bg_saved=$9F10`
   (each `TURTLE_SPR_SIZE*TURTLE_SPR_SIZE`=256). No BSS-clear loop exists, so
   this is purely a placement change — addresses are identical to today.
4. `novalogo/extension.cfg`: `BSS: start=$9800, size=$0400` (editor + lib BSS
   land in `$9800-$9BFF`; turtle is pinned out at `$9C00+`). Grow ZP region
   `ZP: start=$00A3, size=$0019` (vsprite 3 + vtext 5 + editui 6 + editbuf ~11
   ≈ 25 B; `$A6-$E6` is free ZP).
5. `novalogo/extension.s` bottom (after line 1660, alongside the other `.s`
   includes): `.include "editui.s"` then `.include "editbuf.s"` (all guarded;
   `blitter.s`/`vtext.s` already present via vsprite — no dup symbols).
6. `novalogo/Makefile` line 15: add `$(NOVA_ASM)/editui.s $(NOVA_ASM)/editui.inc
   $(NOVA_ASM)/editbuf.s $(NOVA_ASM)/editbuf.inc $(NOVA_ASM)/vtext.s` to the
   `extension.o:` prerequisites.
7. Build base+ext; run `FullyQualifiedName~NovaLogoTests` — they must stay green
   (proves turtle pin + RAM layout preserve existing behavior, and the editor
   code links + fits the extension ROM's ~11.5 KB free).

### Then: EXT_CMD_EDIT + ext_edit + Logo TO (the user-facing wiring)

- `ext_iface.inc`: `EXT_CMD_EDIT = $4B`; reuse ARG0=buf ptr, ARG1=len, ARG2=cap,
  ARG3=title ptr (or fixed shared addrs). `extension.s:190`: append
  `.word ext_edit-1`.
- `ext_edit`: snapshot the 16 display regs (VGC_MODE/CURS*/SCROLL*/BORDER/
  PALETTE/GFXTRANS/SCROLLCTL + TEXTWIN_*), set editbuf config + hook vectors,
  `JSR editbuf_run`, restore (re-`setup_copper`+`prepare_split_text` if split;
  never GCLS), return result in EXT_RESULT.
- SAVE hook = a RAM trampoline the base ROM installs: swap to Logo bank, `JSR`
  the Logo validate+install routine, swap back to ext, `RTS`. editbuf publishes
  current length to the mailbox before calling it.
- `novalogo/procedures.s`: replace `proc_editor_*` (the compact editor) — on
  `TO name :params`, build `TO name :params\n<draft or blank>\nEND` into
  `proc_body_buf`, install the trampoline, snapshot display, call EXT_CMD_EDIT
  via the existing base→ext trampoline, on return restore display + validate +
  install-only-if-valid + write per-proc XRAM draft.
- Rewrite the NovaLogo VM procedure tests to assert the EDITUI shell (menu/
  title+`*`/status/dialog/validation/draft-preservation) instead of the compact
  editor; the `QueueProcedureDefinition` Ctrl-S/Ctrl-Q flow stays.

---

## Progress update — 2026-05-30 (cont. 2): editor LINKED into extension ROM ✅

The RAM-layout + extension-link step from the recipe above is **done + verified**:

- `novalogo/heap.s`: `HEAP_END` → `$9800`.
- `novalogo/extension.s`: turtle block (sprite bufs + state) converted to **fixed
  equates** at `$9C00`/`$9F00` (byte-identical addresses, no behavior change);
  `.include "editui.s"` + `.include "editbuf.s"` added after the vsprite include.
- `novalogo/extension.cfg`: `BSS start=$9800`, `ZP size=$0028` (editor needs
  26 ZP bytes → region $A3-$CA).
- `novalogo/Makefile`: editor sources added to `extension.o` prereqs.
- `e6502UnitTests/NovaLogoTests.cs`: `LogoHeapEnd = 0x9800`.

Result: both ROMs build (16 KB each). Extension CODE `$C000-$EB2B` (~11 KB, fits
the 16 KB ROM); editor BSS `$9800-$9A03` (516 B, fits `$9800-$9BFF`); ZP
`$A3-$BC`. **All 78 NovaLogo tests + 14 editor tests pass — no regressions.**
The turtle stays at `$9F00`; graphics/heap behavior unchanged.

### Text-mode switch + display restore (design confirmed)

- The editor switches to text mode itself: `editbuf_run`→`editui_init` does
  `STZ VGC_MODE`. Generic; no Logo code needed for that.
- `ext_edit` (the Logo host wiring, still TODO) snapshots the 16 display regs +
  re-issues copper on exit to return to the exact prior split/full-graphics
  state without clearing the graphics plane.

### Remaining (the user-facing wiring): EXT_CMD_EDIT=$4B + ext_edit + TO rewrite
+ cross-bank validate trampoline + per-proc XRAM drafts + display save/restore +
NovaLogo test rewrites. (Editor proven, linked, and fits — this is now plumbing.)

---

## Progress update — 2026-05-30 (cont. 3): editor-owned MENU config seam ✅

Per user: the menu belongs to the editor; runtimes customize it via editor-owned
functions + a config hook (Logo wants a "mostly dumb" menu: Save/Quit, maybe
Load — not File→New/Open).

Implemented + verified (14/14 editor tests green, Logo ROM rebuilds):
- **`EDITBUF_MENU_VECL/H`** — MENU-config hook, fires in `editbuf_run` right after
  `editui_init` (which installs the default File/Edit/Help table) and before the
  shell draws. Defaults to `editbuf_default_menu` (RTS = keep defaults). The host
  points it at a routine that tailors the menu.
- **`editui_set_menus`** (A=lo, Y=hi) — editor-owned entry point to install a
  host menu table (or A=Y=0 to fall back to `editui_default_menus`).

So a runtime's menu hook does: `LDA #<my_menus / LDY #>my_menus / JSR
editui_set_menus`. For Logo, `my_menus` is a minimal table (e.g. File→Save,Quit).

### Still TODO for full menu management (needs a RAM menu model)
The current menu tables are RODATA (static). To support runtime add/remove/edit
and **ghosting** (grey out + skip-on-select items that don't apply), add:
- a RAM working-copy menu the editor owns (editui BSS), seeded from a table;
- per-item `enabled` flag (extend the 4-byte item record or a parallel bitset);
- editor-owned fns: `editui_menu_item_set_enabled(cmd, on)` /
  `editui_menu_add_item` / `editui_menu_remove_item`;
- render greyed items in `EDITUI_COLOR_DIM` and skip them in the dropdown
  up/down + Enter selection (`editui_menu_open_current`).
This is the next editor enhancement; the config hook above is the seam it hangs
off, so hosts already have the customization entry point.

---

## Progress update — 2026-05-30 (cont. 4): Stage 2 Increment 1 — `TO` now uses the shared editor ✅

The compact `proc_editor_*` in `procedures.s` is **gone**; `TO` now drives the
shared EDITUI/EDITBUF editor through a real extension command. Verified end to
end: 107/107 impacted tests green (NovaLogoTests + NovaLogoHarnessTests +
EditBufVmTests + RuntimeLibraryAbiTests).

What shipped:
- **`EXT_CMD_EDIT = $4B`** (`ext_iface.inc`) + dispatch entry + **`ext_edit`**
  handler (`extension.s`). `ext_edit` snapshots the VGC display regs editui_init
  clobbers (MODE/PALETTE/BGCOL/BORDER/FGCOL/CURSX/CURSY/CURSEN), `copper_off` +
  full 80×25 text window for the session, wires `EDITBUF_*` config from the
  mailbox (ARG0=buf, ARG1=len, ARG2=cap, ARG3=title), installs an ext-ROM save
  hook, runs `editbuf_run`, then restores the display **exactly** — re-issues
  `setup_copper`+`prepare_split_text` when `TURTLE_INITED` (split was active),
  else a full-text restore + `$0C` clear. **Graphics/turtle are never cleared.**
- **`ext_edit_save_hook`** (ext ROM): records that the user asked to save and
  returns `EDITBUF_SAVE_OK`. The base ROM builds the record on editor exit. This
  keeps Increment 1 free of the cross-bank trampoline.
- **`proc_open_editor`** (`procedures.s`) replaces `proc_editor_body`: builds the
  `"TO <name>"` title in RAM, fills the mailbox, calls `EXT_CMD_EDIT` via the
  existing base→ext `EXT_TRAMPOLINE`, reads final len + the save flag back, and
  calls the **unchanged** `proc_build_record` only when the user saved. Buffer is
  body-only (name/params still pre-parsed from the `TO` command line as before).
- Procedure tests rewritten for the EDITUI shell: `QueueProcedureDefinition` now
  blocks until `<NAME> DEFINED` (the editor re-renders per keystroke, so fixed
  cycle budgets were too small); Ctrl-Q-dirty drives the 3-choice dialog
  (`ENTER` = Exit Anyway); the dirty test asserts the trailing-`*` title format.

### Bug found + fixed (pre-existing, NOT editor-related)
`tok_parse_number`'s `*10` recomputed `val*2` fused with the add to `val*8` and
did `ASL` then **`CLC`**, throwing away the carry out of the low byte. Any value
whose intermediate `*10` accumulator had bit 7 set lost 256 — e.g. `4242`
parsed as `3986`. The old compact editor masked it: it never cleared the screen
on exit, so the echoed body text `"PRINT 4242"` stayed visible and the test's
`Contains("4242")` matched the *echo*, not the (wrong) output. The new editor
clears the screen properly, exposing it. Fixed the multiply (separate 16-bit
shift then add) + added `PrintLargeNumbersParseCorrectly` (red without the fix).

### Still TODO — Stage 2 Increment 2 (the richer UX)
- Source model includes `TO/END`: build the buffer as `TO NAME :params\n<body>\nEND`,
  cursor on the body line; re-parse the header from the buffer on save (lets the
  user edit name/params in the editor).
- **Live cross-bank validation on Ctrl-S**: ext-ROM save shim publishes
  `EDITBUF_LEN` to the mailbox, JSRs a RAM ext→Logo trampoline (swap bank, JSR
  Logo validator, swap back, preserve A) → `logo_save_validate` (find `END`,
  balanced brackets, every line tokenizes, valid name/params); install only on
  success, always keep the draft; tri-state status messages.
- Per-procedure XRAM drafts keyed by name; reopen reconstructs from the record.
- Editor menu hook for Logo (minimal Save/Quit menu via `editui_set_menus`).
- RAM menu model + ghosting (from cont. 3).

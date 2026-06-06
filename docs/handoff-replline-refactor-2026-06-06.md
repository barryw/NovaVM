# Handoff: extract the REPL line-reader + history into a shared `replline.s`

**Date:** 2026-06-06
**Branch with the working source:** `feat/turtle-logo-coords` (pushed to origin)
**Goal:** make the NovaLogo command-line reader (with shell-style up/down history)
reusable so **Forth, Pascal, and the assembler dev env** inherit it. **BASIC is
excluded** — it uses EhBASIC's own ROM input routine.

## Why

The line reader + history currently lives inline in `novalogo/novalogo.s`. It's a
generic facility (key poll, backspace, history ring, up/down recall, line repaint)
with exactly **one runtime-specific decision**: "is the typed line complete, or
keep reading?" (Logo continues while `[`/`(` brackets are open, for multi-line
entry). Every other Nova-native runtime wants the same reader.

## What exists now (the source to extract)

All in `novalogo/novalogo.s` (committed on `feat/turtle-logo-coords`):

- **`read_line`** — the input loop. Captures the input-start cursor
  (`VGC_CURSX`/`VGC_CURSY`) after the prompt into `HIST_SX`/`HIST_SY`, resets
  `HIST_NAV`, polls `VGC_CHARIN`, handles CR (with the bracket-depth continuation),
  backspace, intercepts Up `$1E`/Down `$1F` → `hist_up`/`hist_down`, swallows
  Left `$1C`/Right `$1D`, stores+echoes printables. Calls `hist_save` on CR.
- **History subroutines:** `hist_save`, `hist_up`, `hist_down`, `hist_show`
  (repaint at the origin — blank `HIST_OLDLEN` cols, reprint `input_buf`),
  `hist_copy_in`/`hist_copy_out`, `hist_slot_ptr`/`hist_slot_addr`
  (idx = (HEAD−K) mod 6, addr = `$9890` + idx*128).
- **State (hardcoded, NOT `.res`):** `HIST_BASE = $9800` … `$9BFF`. This band is
  free (BSS ends `$1304`, heap ends `$9800`, turtle state is `$9C00+`). A plain
  `.res` in the BSS segment would land in the heap-overlap zone — that's why it's
  hardcoded like the turtle state at `$9F00`. Layout: header `$9800-$980F`
  (COUNT/HEAD/NAV/SX/SY/OLDLEN/K/IDX), live-line `$9810-$988F` (128),
  6 slots `$9890-$9B8F` (6×128).
- **`line_bracket_depth`** — Logo's continuation check (the bit that must become a
  hook).
- **`cold_start`** zeros `HIST_COUNT`/`HEAD`/`NAV`.
- **`input_buf`** (128) + `buf_idx` (ZP) — the line buffer the reader fills.

## The refactor

1. **New file `runtime/asm/replline.s`** (co-assembled via `.include`, like
   `vsprite.s`). Move `read_line` (rename → **`repl_read_line`**) and all `hist_*`
   routines + the `HIST_*` equates into it. Keep `input_buf`/`buf_idx` as the
   shared contract (or pass a buffer pointer — see note).
2. **Hook for the continuation check.** Where `read_line` currently does
   `JSR line_bracket_depth` on CR, call **`repl_line_complete`** instead (returns
   Z=line is complete). Each runtime provides its own:
   - NovaLogo: `repl_line_complete: JMP line_bracket_depth` (keep the existing logic).
   - Forth/Pascal/assembler: `repl_line_complete: LDA #0; RTS` (always complete).
   Pick a hook mechanism that's clean in ca65 — simplest is "the runtime defines a
   label `repl_line_complete` that `replline.s` references; the include `.import`s
   it or relies on co-assembly." Verify whether ca65 wants `.global`/`.import` here.
3. **Repoint NovaLogo:** `novalogo.s` `.include "replline.s"`, defines
   `repl_line_complete` → `line_bracket_depth`, calls `repl_read_line` from
   `main_loop`, and drops the moved code. `cold_start` calls a `repl_init` (zeros
   the header) instead of the inline STZs.
4. **History-buffer ownership.** `$9800-$9BFF` is the agreed band. It's safe for
   any single resident runtime. Confirm Forth/Pascal/assembler don't already use
   `$9800-$9BFF` before adopting (none do today).

### Note: buffer contract

`repl_read_line` fills `input_buf`/`buf_idx`. If a runtime wants a different
buffer, parameterize via a ZP pointer + a max-length byte rather than the fixed
`input_buf` symbol. For a first cut, keeping the shared `input_buf`/`buf_idx`
symbols is fine (one runtime resident at a time).

## Verification

- NovaLogo behavior must be **byte-identical** after the move: build `novalogo.bin`,
  run `dotnet test --filter "FullyQualifiedName~NovaLogo"` (110 tests, must stay
  green), and GUI-smoke the history (up to oldest holds; down to empty live line
  holds; no cursor drift). Driver pattern: `NOVA_BOOT_ROM=logo dotnet run --project
  e6502.Avalonia`, TCP port 6502, `send_key key=""` (up) / `""` (down),
  `screenshot type:"frame"`.
- When wiring a second runtime (Forth/Pascal/asm), the only new code is its
  `repl_line_complete` stub + the `.include` + calling `repl_read_line`.

## Gotchas

- The GUI (`EmulatorCanvas.cs:171`) moves the VGC cursor on arrow keys *before* the
  6502 sees the byte, so the reader must repaint on **every** up/down (even at
  boundaries) to reset the cursor — `hist_up`/`hist_down` already do this; preserve
  it.
- `read_line`'s `@`-local labels and the global `hist_*` labels must keep their
  scoping when moved (the dispatch `BEQ @key_up` and `@key_up` must stay in the same
  local scope inside `repl_read_line`, before the first global `hist_*` label).
- `hist_slot_addr` reuses `HIST_K` as scratch — fine for current callers; keep that
  invariant or give it its own scratch byte when generalizing.

## Out of scope (leave for later)

- In-line editing with Left/Right (currently swallowed).
- Multi-row line recall (history repaint assumes a single prompt row).
- Persisting history across the procedure editor (the editor doesn't touch `$9800`,
  so history already survives an edit — good).

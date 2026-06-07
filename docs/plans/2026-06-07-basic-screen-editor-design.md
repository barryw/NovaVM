# BASIC C64-Style Screen Editor — Design (DEFERRED plan)

**Date:** 2026-06-07
**Status:** ✅ SHIPPED 2026-06-07 (branch phase10-screen-editor). Implemented per
`docs/plans/2026-06-07-basic-screen-editor-impl.md` as SYSTEM-module fn
`SYS_SCREEN_READLINE` ($12) with multi-row wrapped-line read on ENTER; BASIC
`LAB_1357` routes through it. Faithful-MVP scope (deferred: full multi-row
*interactive editing* of previously-listed long lines; INSERT mode).

## The idea (user)

BASIC is **not** a REPL language. Logo/Forth/Pascal/assembler are REPL-based: you
edit a *source file* in the Nova editor, then the REPL assembles/compiles/runs it.
BASIC wants the **Commodore 64 screen editor**:

> "i should be able to navigate around the screen with arrow keys.. if i make a
> mistake, i arrow back up to the line, fix whatever was fucked up, hit enter and
> it just works. that's part of what made the 64 so cool.. apple didnt have that."

The screen *is* the editor. `LIST`, arrow up to any line, retype the broken part
in place, press RETURN — BASIC re-ingests whatever logical line the cursor sits on.
No edit/run mode split.

Decision: this replaces the rejected idea of putting BASIC on the REPL line reader
(`replline.s`). That reader stays for the REPL languages only.

## Current state (the gap)

BASIC input today is a **keystroke-buffer** model, not a screen model:
- `EmulatorCanvas` (host key handler) sends arrows as both a control byte (28–31)
  into the input queue AND a `ScreenEditor.CursorUp/Down/...` that moves the VGC
  hardware cursor.
- BASIC `LAB_1357` (`ehbasic/basic.asm:1195`) reads bytes via `V_INPT`→`RegCharIn`
  ($A00F) into the linear buffer `Ibuffs` until CR, then submits *that buffer*.
- `ScreenEditor.ReadLineFromScreen()` (`e6502.Avalonia/Input/ScreenEditor.cs:53`)
  — the C64 "scan the logical line off the screen" primitive — **exists but has
  zero callers (dead)**.

Result: the cursor roams visually and BASIC echoes typed chars at the cursor (so an
edit *looks* applied), but **RETURN submits the linear keystroke buffer, not the
screen line** — buffer and screen diverge the instant you arrow up. And all the
cursor handling is **C# / Avalonia-only**: on FPGA/NovaHost there is no C# editor,
so none of it works on the real hardware target.

## Placement decision (locked, AskUserQuestion 2026-06-07): the SYSTEM module ($03)

A new SYSTEM-module fn (working name `SYS_SCREEN_READLINE`) runs the **entire** C64
edit loop against **VGC MMIO** (screen char RAM, cursor registers, key input) and
returns the submitted logical line. Rationale:
- **Cross-target:** the VGC exists on both Avalonia and FPGA, so one implementation
  serves both — unlike the host-only C# path. This is the real C64 model (the editor
  was KERNAL ROM reading/writing screen RAM).
- **Page-in cost is irrelevant:** the call is interactive (blocks on the user); one
  16KB page-in per line entered, not per keystroke.
- **Budget:** the BASIC main ROM has only ~382 bytes free; a full editor (~600–1500B)
  cannot bake in there, and baking it would fight the ext-ROM-elimination goal. The
  SYSTEM module has ~2.2KB free.
- **Reusable:** any runtime wanting a live screen-editor input mode can call it.

Note: SYSTEM already hosts the *modal document editor* `SYS_FN_EDIT` (for REPL-lang
source files). The screen editor is a *different* fn — a live per-line screen reader
tied to a host's main loop, not a modal full-buffer editor. Check whether editbuf/
editui primitives can be partially reused, but expect this to be its own routine.

## Proposed shape

**Module fn `SYS_SCREEN_READLINE`:**
- In: `ARG0` b0/b1 = destination buffer ptr (BASIC's `Ibuffs`, in low RAM — survives
  page-out), b2 = max length.
- Loop against VGC MMIO:
  - Read a key (RegCharIn / key FIFO).
  - Printable → write char to screen char RAM at cursor, advance cursor.
  - Arrows → move cursor non-destructively (clamp to screen).
  - INST/DEL → insert/delete on the current line (open question: full C64 INST/DEL).
  - RETURN → determine the **logical line** bounds the cursor is on, read those
    chars off screen char RAM (`ReadLineFromScreen` logic, ROM-side), trim trailing
    blanks, copy into the dest buffer, advance cursor to next line.
- Out: `LIB_RESULT` b0 = line length; `LIB_STATUS = LERR_OK`. Line bytes in the buffer.

**BASIC side:** replace the `LAB_1357` body (or its `V_INPT` accumulation) with a
`lib_call(SYSTEM, SYS_SCREEN_READLINE)` that fills `Ibuffs`, then continue into the
existing tokenize/store/execute path (a numbered line replaces/inserts in the program;
an unnumbered line executes immediately) — unchanged.

**Host/FPGA:** editing happens via VGC MMIO, so both targets work. The C# arrow→cursor
code becomes redundant for BASIC (the module owns cursor motion); the host just needs
to deliver raw keystrokes (incl. distinct arrow/INST/DEL codes) to the key input on
BOTH targets. Verify arrow/INST/DEL keys reach the 6502 as distinct, stable codes on
Avalonia and on FPGA/NovaHost before building.

## Open questions (resolve at plan time, YAGNI by default)

1. **Logical-line wrapping.** C64 logical lines span up to 2 physical 40-col rows via
   a line-link table. At 80 cols a single physical row is usually enough. MVP: treat
   one physical row as the logical line; add multi-row link tracking only if needed.
2. **INST/DEL semantics.** Full C64 insert/delete-with-shift vs simple overwrite +
   backspace. MVP could be overwrite + destructive backspace; true INST/DEL is a
   follow-on.
3. **Quote mode / PETSCII control codes.** C64 had quote mode for embedding cursor
   controls in strings. Almost certainly skip.
4. **Screen scroll.** When RETURN is on the bottom row, the screen scrolls; the editor
   must track the cursor/line through a scroll.
5. **Key source.** RegCharIn polling vs the VGC key FIFO; confirm which gives clean
   arrow/edit-key codes on both targets.
6. **Reuse of editbuf/editui** vs a purpose-built routine.

## Why deferred

The active priority is migrating BASIC's remaining ext-ROM domains to modules and
eliminating the extension ROM. The screen editor is a sizable, independent feature
(its own SYSTEM fn + BASIC main-loop change + cross-target key-input verification) and
should not interleave with the migration. Pick it up afterward.

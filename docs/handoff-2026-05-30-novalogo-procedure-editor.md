# NovaLogo Procedure Editor Handoff

Date: 2026-05-30

## Problem Statement

The current `TO` implementation is not the desired user experience. It opens a
compact Logo-local editor in `novalogo/procedures.s` with only append,
backspace, Ctrl-S, and Ctrl-Q. That was useful as a proof that the interpreter
can enter an editor-like mode, but it is the wrong architecture and should not
be polished into the final feature.

The final `TO` flow must use the shared editor shell. Logo should not own editor
chrome, dirty indicators, cursor movement, cut/copy/paste, or modal dialogs.
Those belong in shared editor/TUI code so every native tool gets the same
behavior.

## Required User Experience

When a user types:

```logo
TO SQUARE
```

or any other new procedure name and presses Enter:

1. NovaLogo switches to a proper text editor experience.
2. If the user was in split-screen or graphics mode, the system temporarily
   switches to text mode for editing.
3. The shared editor shell is displayed:
   - menu bar at the top
   - title/filename area showing the procedure name, for example `SQUARE`
   - shared dirty marker `*` next to the filename/title when modified
   - editor body region
   - status/help bar at the bottom
4. The user edits the full procedure source. Procedure definitions include an
   `END` line in the source.
5. Ctrl-S saves/validates the source but does not exit.
6. Ctrl-Q exits the editor.
7. If Ctrl-Q is pressed while the workspace is dirty, show a modal dialog with
   exactly three choices:
   - exit anyway
   - save first
   - cancel
8. On exit, return to exactly where the user was before editing:
   - same display mode
   - same split-screen state
   - same graphics contents
   - same turtle position, heading, visibility, and pen state
   - same prompt area and cursor behavior
9. User work must never be lost. Invalid procedure text may remain in memory
   and be reopened/fixed.
10. A procedure must never become callable unless it validates cleanly.

## Current State

Relevant files:

- `novalogo/procedures.s`
  - owns procedure records, invocation, and the current wrong compact editor
    flow.
  - current local editor symbols include `proc_editor_body`,
    `proc_editor_draw`, `proc_editor_dirty`, `proc_editor_saved`, and related
    helpers.
  - this code should be replaced by a shared-editor entry path.
- `runtime/asm/editui.inc` / `runtime/asm/editui.s`
  - current shared TUI shell.
  - owns menu bar, title/help band, status bar, panels, menu dropdowns, and
    `EDITUI_DIRTY`.
  - menu dropdowns already save/restore overlapped text-plane areas using
    `EDITUI_MENU_SAVE_*` buffers and helper routines.
- `runtime/asm/nui.inc` / `runtime/asm/nui.s`
  - first-pass modal dialog helpers.
  - currently draws dialog chrome and waits for a key.
  - likely needs work or replacement for proper EDITUI modal overlays with
    three choices and restore-under behavior.
- `docs/assembly/editui.md`
  - documents the current shared editor shell ABI.
- `docs/plans/2026-05-27-novalogo-language-design.md`
  - already describes `TO` opening EDITUI.
- `docs/plans/2026-05-27-novalogo-implementation-plan.md`
  - has an older Phase 8 note for EDITUI integration, but it is less strict
    than the current requirement.

Known ROM layout constraint:

- Base NovaLogo ROM is tight. Recent maps showed only a few hundred bytes free
  near the monitor region.
- The extension ROM has much more free space.
- Do not blindly link all of `editui.s` into the base ROM unless the map proves
  it fits. Prefer an editor overlay/extension design that keeps large editor
  code out of the base interpreter.

## Architecture Direction

### Ownership

Logo owns:

- recognizing `TO <name> [:params...]`
- procedure metadata and callable procedure records
- validating Logo procedure source
- installing only valid procedures into the callable procedure table
- preserving/restoring Logo runtime state around editor entry

Shared editor owns:

- editor shell chrome
- dirty marker behavior
- text buffer cursor movement
- insert/delete/backspace
- line navigation
- cut/copy/paste
- menu dispatch
- Ctrl-S / Ctrl-Q key conventions
- dirty-exit dialog
- modal overlay save/restore behavior

### Recommended Flow

1. User enters `TO SQUARE`.
2. Base Logo parses the procedure name and parameter list.
3. Base Logo prepares an editor request structure in RAM:
   - procedure name/title pointer
   - source buffer pointer
   - source buffer capacity
   - existing draft source length, if any
   - flags for new vs existing procedure
4. Base Logo saves the current UI/runtime state:
   - `VGC_MODE`
   - cursor x/y/enabled
   - text window/split state if applicable
   - any Logo state needed to redraw/continue the prompt
   - turtle state is already persistent, but must not be reinitialized
   - graphics VRAM must not be cleared
5. Base Logo calls a shared editor entry point. Because base ROM is tight, this
   should probably be in the extension ROM or a dedicated editor overlay.
6. The shared editor:
   - switches to text mode
   - initializes EDITUI
   - sets title to the procedure name
   - loads source text into its editable buffer
   - lets the user edit
   - marks `EDITUI_DIRTY` when modified
   - validates/saves on Ctrl-S by returning source text to the shared buffer
     and asking Logo validation code to run after editor exit, or by returning
     a `SAVE_REQUESTED` result to the caller
7. On Ctrl-Q:
   - if clean, exit
   - if dirty, show the three-choice dialog
   - exit/cancel/save-first behavior must be explicit
8. Base Logo restores the exact previous display state.
9. Base Logo validates the buffer:
   - if valid, build/install the procedure record and make it callable
   - if invalid, keep the source draft, do not install/callable-enable it, and
     show a useful error that allows reopening the editor

Important: a save action and an install action are not the same thing. Source
can be preserved even when invalid. Callable procedure records must only be
created/replaced after validation succeeds.

## Procedure Source Model

The editor should contain real Logo source including `END`, for example:

```logo
TO SQUARE
REPEAT 4 [FD 50 RT 90]
END
```

or with parameters:

```logo
TO SQUARE :SIZE
REPEAT 4 [FD :SIZE RT 90]
END
```

Validation must check at least:

- first nonblank line is a matching `TO <name> ...`
- body eventually has `END`
- bracket/list nesting is balanced
- tokenizer accepts every line
- procedure name and parameter names are valid
- no malformed Logo syntax is accepted into callable state

Future validation can add deeper semantic checks, but syntax/tokenization must
be strict before installation.

## Dirty State

Dirty state is shared editor behavior.

Required behavior:

- Loading existing text starts clean.
- Typing, deleting, cutting, pasting, or changing source marks dirty.
- Ctrl-S clears dirty only if the source is successfully saved/preserved.
- If Ctrl-S validation fails, the text remains dirty or enters a clearly
  visible "saved draft but not callable" state. Do not silently clear dirty if
  the user reasonably expects the procedure to be usable.
- The dirty marker is displayed as `*` next to the filename/title.

There is already an `EDITUI_DIRTY` byte in `runtime/asm/editui.s`; use that
for assembly EDITUI. Host-side `ScreenTextEditor` also centralizes dirty title
rendering for Avalonia-hosted editors.

## Modal Dialog Requirements

The dirty-exit dialog must be a true overlapping modal, not a full-screen clear.

Dialog behavior:

- Save the covered area before drawing.
- Draw over the editor.
- Let the user choose one of three options:
  - Exit Anyway
  - Save First
  - Cancel
- Restore the covered area when dismissed.
- Return a stable result code to the caller.

Existing EDITUI menu code already saves/restores overlapping text-plane areas
with `EDITUI_MENU_SAVE_CHAR`, `EDITUI_MENU_SAVE_COLOR`, and
`EDITUI_MENU_SAVE_ATTR`. Generalize that mechanism for dialogs or add an
EDITUI dialog subsystem that uses the same approach.

`runtime/asm/nui.s` currently provides first-pass modal dialogs, but it should
be audited before use. If it cannot preserve/restore the underlying editor
surface, extend EDITUI instead of accepting destructive dialog behavior.

## Display Restoration

This is a hard requirement.

If the user enters the editor from split screen, exits, and returns to Logo:

- the graphics plane must be unchanged
- the turtle must still be visible if it was visible
- turtle position and heading must be unchanged
- the prompt region must return to the same split mode
- no editor remnants may remain on the visible prompt area

If the user enters from full text mode:

- text screen state should be restored or intentionally redrawn to a correct
  Logo prompt without stale editor artifacts.

If the user enters from full graphics mode:

- graphics must remain intact
- the runtime should return to full graphics mode
- command input behavior must remain consistent with the mode's design

Do not clear graphics as part of opening or closing the editor.

## Testing Requirements

Unit/integration tests should cover both editor behavior and Logo behavior.

### Logo/Avalonia VM Tests

Add or update tests that drive the Logo ROM through `CompositeBusDevice`:

- `TO SQUARE` opens shared editor shell, not the compact local editor.
- Title/filename shows `SQUARE`.
- Editing marks the shell dirty with `*`.
- Ctrl-S saves/validates without exiting.
- Ctrl-Q exits cleanly when not dirty.
- Ctrl-Q while dirty opens a three-choice dialog.
- Cancel returns to the editor with the text intact.
- Save First attempts validation and remains in editor on validation error.
- Exit Anyway preserves draft text but does not install a callable procedure.
- Valid `SQUARE` becomes callable and draws a square:
  `REPEAT 4 [FD 50 RT 90]`.
- Invalid `SQUARE` is not callable.
- Reopening an invalid draft does not lose text.
- Split-screen graphics/turtle state is preserved across editor entry/exit.

### Shared Editor Tests

Add editor-shell tests independent of Logo:

- dirty marker on edit
- dirty marker clears on save
- cursor movement
- insert/delete/backspace across lines
- cut/copy/paste
- menu commands return expected command IDs
- dirty-exit dialog result codes
- dialog save/restore-under behavior

### Verilator / Hardware-Facing Tests

Update `e6502.FPGA/test/test_novalogo_draw_top.sv` or add a separate top-level
test:

- boot Logo
- enter `DRAW`
- record turtle state and graphics pixels
- enter `TO SQUARE`
- confirm shared editor markers in text VRAM
- type valid source including `END`
- Ctrl-S, then Ctrl-Q
- confirm display mode and turtle/graphics state restored
- run `SQUARE`
- verify square edges in graphics VRAM

Update `tools/run-novalogo-hardware-smoke.py` similarly, but do not rely on HDMI
frame capture as the source of truth until the capture distortion issue is
fixed. Use NovaHost screen/cursor/VRAM reads for pass/fail.

## Hardware Capture Note

The current HDMI frame capture path can badly distort Nova output. During the
2026-05-30 hardware test, captured frames showed severe horizontal corruption
and green banding while NovaHost text/cursor reads were coherent. Do not use
captured HDMI frames as proof of Logo/editor correctness until capture timing,
pixel format, or scaler behavior is fixed.

Recommended separate follow-up:

- enumerate AVFoundation capture modes
- test alternate `ffmpeg` pixel formats and frame sizes
- verify whether the capture device expects 720x480, 640x480, or another mode
- compare against direct VRAM/debug screenshots

## Acceptance Criteria

This work is complete only when all of these are true:

- The compact Logo-local procedure editor is removed or bypassed.
- `TO <name>` opens the shared EDITUI-based editor shell.
- The filename/title is the procedure name.
- Dirty `*` is owned by shared editor code.
- Ctrl-S saves/validates and stays in editor.
- Ctrl-Q exits only after handling dirty state correctly.
- Dirty-exit dialog has Exit Anyway / Save First / Cancel.
- Editor supports cursor movement and cut/copy/paste through shared code.
- Procedure source includes and requires `END`.
- Invalid source is preserved but not callable.
- Valid source becomes callable only after validation succeeds.
- Editor exit restores the exact previous Logo display/graphics/turtle state.
- Avalonia tests pass.
- Verilator/top-level tests pass.
- Hardware smoke passes using direct debug reads.

## Do Not Do

- Do not keep improving the compact Logo-local editor as the final path.
- Do not implement editor features inside Logo-specific code.
- Do not define callable procedures from invalid source.
- Do not discard invalid source.
- Do not clear the graphics plane on editor entry or exit.
- Do not rely on distorted HDMI captures for pass/fail.
- Do not synthesize or flash a bitstream for this feature unless a separate
  hardware bug is proven.


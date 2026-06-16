# nova.lib — NDK library linking design

Date: 2026-06-16
Branch: `fix/65c02-audit-bugs-1-5`

## Problem

NDK modules (`runtime/asm/*.s`) are consumed by `.include` (source inclusion).
All code in an included `.s` is assembled into the app's single object; `ld65`
has **no dead-code elimination**, so an app pays ROM for every routine in every
module it includes — even routines it never calls.

Proven case: novachess `.include`s `nui.s` for its dialog API
(`nui_show_dialog`/`nui_show_error`/`nui_dialog_defaults`) and thereby links the
entire +621-line nui widget WIP (list-picker, save-under, ANSI input) it never
calls — ~1213 bytes of dead weight, overflowing its 16 KB ROM budget.

Evidence: `od65` shows `nui_pick_list`/`nui_save_under`/`nui_read_key` defined in
`runtime.o` despite 0 call sites; `ld65 --help` has no strip/gc option; stashing
the WIP dropped novachess ROM overflow by exactly ~1213 bytes.

## Key enabler (already in place)

The NDK `.inc` files already declare routines and shared vars as `.global`
(e.g. `nui.inc`: `.global nui_show_dialog`, `.global NUI_DIALOG_LEFT`;
`vtext.inc`: `.global VTEXT_CURX`; `blitter.inc`: `.global blitter_start_fill`).
`.global` = export-if-defined-here, import-if-not. Today they resolve locally
because everything is one object. Split modules into separate objects and the
same `.global`s auto-resolve as cross-object imports/exports. The import/export
surface for library linking is therefore **already declared**.

`ar65` (cc65 librarian) is available.

## Approach

- **Granularity: feature-group objects.** Split a module along cohesion lines so
  optional features are separate objects that `ld65` strips when unreferenced.
  Routines always used together stay in one object and keep sharing local
  helpers/state with no import churn.
- **Sequencing: PoC on nui, then incremental.** Validate the whole chain on the
  motivating case before migrating the other 27 modules.

## PoC: strip nui widgets out of novachess

Split the (uncommitted WIP) `nui.s` into feature-group files, each
`.include "nui.inc"`:

| New file        | Contents                                              | novachess |
|-----------------|-------------------------------------------------------|-----------|
| `nui_dialog.s`  | dialog core: defaults/style/validate/chrome/print_*/show_dialog/show_error/wait_key | linked |
| `nui_input.s`   | `nui_read_key` + `nui_read_key_wait_byte` (ANSI)      | linked (wait_key→read_key) |
| `nui_list.s`    | list-picker: `nui_pick_list`/`nui_list_*`             | **stripped** |
| `nui_uisave.s`  | save/restore-under: `nui_save_*`/`nui_restore_*`/`nui_select_saved_region` | **stripped** |
| `nui_state.s`   | all `NUI_*` BSS/ZP `.res` definitions (the only allocator) | linked, 0 ROM |

Rules:
- Only `nui_state.s` *allocates* the shared vars (`.res`); routine files import
  them via the `.inc` `.global`s. ZP vars use `.globalzp`.
- Cross-group calls (e.g. dialog `wait_key`→`nui_read_key`) resolve via
  `.global` once `nui_read_key` is globalized in `nui.inc`.
- **Deps stay source-included** in novachess (vtext/vsprite/blitter). nui
  objects import those symbols via existing `.global`s; `ld65` resolves a lib
  object's imports against `runtime.o`'s exports. No ripple to deps.

### Build / consume

- `runtime/asm/Makefile`: assemble the 5 nui objects, `ar65 a nova.lib *.o`.
- novachess: remove `.include "nui.s"` from `runtime.s`, keep `.include
  "nui.inc"`, add `nova.lib` to the `ld65` invocation.

### Success metric

1. novachess links against `nova.lib`.
2. `nui_pick_list`/`nui_save_under`/`nui_list_render` **absent** from the link map.
3. novachess ROM usage drops ~700+ bytes vs the source-include build.
4. Existing novachess smoke (`make -C examples/novachess test`) still passes.

If the metric fails, revert: the original `nui.s` content is preserved in git as
the split's starting point.

## Rollout (after PoC)

Repeat per module, validated each step. Most modules need only: assemble as
`.o`, add to `nova.lib`, switch consumers from `.include "<m>.s"` to lib-link.
Feature-group splitting applies only where a module carries optional bulk
(nui, editbuf, audio, gameserver, xmc). Apps migrate one at a time; the
source-include and lib models coexist during the transition because `.global`
resolves across both.

## Out of scope (YAGNI)

- Literal one-routine-per-object. Feature-group captures the real dead-weight
  cases; per-routine adds churn without proportional benefit.
- Migrating all 28 modules up front. Incremental, value-first.

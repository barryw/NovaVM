# Handoff — Stage 4c.2 hardware bring-up (emulator side COMPLETE)

**Start here.** Then skim memory `project_stage4c_replan_2026_06_04.md`. Everything emulator-side is DONE,
reviewed, and the full suite is 100% green. **The only remaining work is step iv: hardware verification.**

## State at handoff
- **HEAD = `20bcfb7`** on `main`. Full unit suite: **1517 passed / 0 failed / 12 skipped / 1529 total** (green).
- The NovaLogo turtle now lives **entirely in the GRAPHICS module** and works end-to-end **on the emulator**.
  It has **never run on hardware** (see step iv — the module isn't even staged on the board yet).
- Build artifacts (`*.o/.lst/.map` for novalogo/extension) may show dirty in `git status` — toolchain
  rebuild drift, non-load-bearing. The committed `.bin` files are canonical. Unrelated pre-existing dirty
  files also exist (`ehbasic/extension.o`, `novavm.inc`, `boot.json`, `demo.*`, the PDF, `assembly/apps/**`,
  `.claude/settings.local.json`) — DO NOT stage them.

## What shipped this session (commits `d02b758..20bcfb7`, all on `main`)
| Commit | What |
|--------|------|
| `d02b758` | R1 — foundation slim (1-arg reporter arity flip), freed 77 B |
| `20e4314` | **Step i** — turtle command engine `GFN_TURTLE_OP` ($B3) in `modules/graphics/graphics.s` (26 ops) |
| `aaab619` | **Step ii** — foundation thin adapter `logo_adapt_turtle` + `ext_cmd_table` reroute |
| `3c4936b` | Fix — `turtle_render` erases the old turtle BEFORE the pen line (BOB restore was gapping lines) |
| `5f245a4` | review comment nit |
| `b785c12` | **Step iii** — deleted turtle drivers from `novalogo/extension.s` (−4570 B) |
| `10e0ddd` | Fix — restore split-screen after EDIT exit (`proc_edit_restore_split` in procedures.s) |
| `a80ef12` | Fix — re-page test uses still-legacy TIMER (rerouted SETXY broke its premise) |
| `20bcfb7` | Fix — DMA test asserts XMC page INCREMENT, not a stale absolute (host-ext shelf = 64 legit pages) |

## Architecture you must understand (turtle-in-module)
The plan's "turtle resident in the Logo foundation" was **measured at 469 B over the foundation ROM budget**
(probe in session). So the turtle moved INTO the GRAPHICS module instead (module had ~7.6 KB ROM free +
already includes the NDK drivers). Net effect: turtle is now a **shared, runtime-agnostic capability**.

- **Module side** (`modules/graphics/graphics.s`): one op-dispatched fn **`GFN_TURTLE_OP = $B3`**
  (`runtime/asm/libgraphics.inc`). Op-code in `LIB_ARG2` byte0 (= the EXT_CMD value, FD=$10…SETPOS=$29).
  Value args Logo 16.8 in `LIB_ARG0/ARG1` (**byte0=FRAC, byte1=LO, byte2=HI** — fraction PRESERVED for
  sub-pixel; SETPC/SETBG/SETPOS read byte0). Move-math + render are in the module; render reuses the shipped
  `$B0/$B1/$B2` turtle-render subs + `vgc_line` directly (no nested lib_call). Turtle state at **`$9F00`**
  (shared RAM, module writes / tests read / `cold_start` zeros).
- **Foundation side** (`novalogo/eval.s`): `MODULE_ID_TURTLE = $FD`; `ext_invoke` early-branches to
  `logo_adapt_turtle` (FRAC-preserving marshal, distinct from the s16 `@convert_args`) which lib_calls
  `GFN_TURTLE_OP`. `logo_turtle_textwin` carves the split text band the module defers. The turtle commands'
  `ext_cmd_table` entries are `MODULE_ID_TURTLE`.
- **Extension** (`novalogo/extension.s`): turtle drivers DELETED; now just editor + sound + timing.
  `EXT_CMD_*` enum stays (foundation uses the values as op-codes). EDIT-exit re-applies the split via the
  foundation (`procedures.s proc_edit_restore_split`), since the extension can't lib_call.

## ⚠️ STEP iv — hardware bring-up (the remaining work; needs the physical board)
**The GRAPHICS module is NOT staged on the board.** `e6502.ESP32/novahost/build/sd/config/boot.json`
`libraries[]` lists only `testmod` (id 127, base 393216=$060000). So **neither the already-shipped 4c.1-3
graphics NOR the new turtle can run on HW** until the module is staged. Steps:

1. **Determine the `.mod` packaging.** The SD `/lib/` holds `testmod.mod`; the module binary is
   `modules/graphics/graphics.bin` (16384 B). Find how `testmod.mod` is produced (is `.mod` just the raw
   `.bin`, or a header+payload?) — check the module Makefiles / NovaHost firmware loader — and produce
   `graphics.mod` the same way.
2. **Stage the module:** put `graphics.mod` on the SD `/lib/` and add a `libraries[]` entry
   `{ id: 1, base: 393216, size: 16384 }` (id 1 = `MODULE_ID_GRAPHICS`, base $060000 = `SHELF_BASE`).
3. **Stage the runtime:** push `novalogo.bin` + `novalogo_ext.bin` to SD `/roms/` via
   `nova rom put <file> [--remote=novahost.local]` (`e6502.Nova/Program.cs` `DoRom`). **nova CLI is TCP-ONLY**
   ([[feedback_novahost_tcp_only]]) — rebuild the AOT binary if stale (`e6502.Nova/bin/Release/.../native/nova`);
   use the `nova` CLI, never raw TCP ([[feedback_use_nova_cli]]).
4. **Firmware follow-on:** NovaHost must stage the active runtime's `extensionRom` at
   `HOST_EXT_XRAM = $07C000` so the legacy re-page path (sound/timing/EDIT) works on HW. Emulator side is done;
   confirm firmware does this.
5. **Verify on the board** (≈192.168.1.65 / novahost.local): boot Logo, run a turtle program
   (`CS` then `REPEAT 4 [FD 50 RT 90]`), and look — the square should draw with no gaps at the corners
   (that gap was the `3c4936b` bug; it's fixed on the emulator, confirm on HW). Also try `EDIT` then Save+Quit
   with a drawing up — the split screen must come back (the `10e0ddd` fix).

## Build / test
- cc65 on PATH: `export PATH=/Users/barry/Git/cc65/bin:$PATH`
- Module: `cd modules/graphics && make`  •  Logo: `cd novalogo && make && make install`
- Tests: `dotnet test e6502UnitTests/e6502UnitTests.csproj --filter "FullyQualifiedName~GraphicsModule"`
  (134) and `~NovaLogo` (112). Full suite ~6.5 min.

## Lesson from this session (apply it)
Four real defects hid behind passing tests (line-eating render, EDIT-exit display, a re-page test using a
rerouted command, a stale page-count). **None were caught by the original tests** — all four surfaced via
adversarial spec review (verify behavior with the feature ENABLED, not hidden/disabled) and a **full-suite
run after the broad reroute** (per-filter green hid a `~CompositeLoader` breakage). Keep the two-stage
review + falsification discipline.

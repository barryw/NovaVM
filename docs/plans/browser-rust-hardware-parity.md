# Browser Rust Hardware Parity

The browser Rust core is the target browser hardware implementation. It should
not be a fast partial path. This file tracks the remaining parity work against
the Avalonia hardware model.

## Current Status

- CPU: functional-test coverage passes in Rust (`AllSuiteA`, Klaus functional,
  65C02 extended opcodes, interrupt suite).
- VGC basics: text, color, attr, gfx plane, primitive plot/line/circle/rect/fill,
  sprite shape/register mapping, palette mode, and memory commands exist.
- VGC copper: Rust now owns copper lists, add/clear/list/use/enable/disable, and
  vblank activation. The browser renderer consumes active copper programs for
  scanline mode splits.
- FIO: browser host path handles runtime/module load, directory and file words,
  NDI/local media mounting, xload/xsave, and graphics load/save.
- XMC/XRAM: register and window access are implemented for the browser core.
- DMA/blitter: synchronous memory-space copies/fills are implemented.
- Math: scalar and vector math registers are implemented enough for turtle
  graphics and current NDK users.
- Timer and board input: basic registers and IRQ state exist.

## Known Gaps

- VGC renderer parity is incomplete:
  - Copper is scanline-display-state aware, but not yet pixel-exact for
    mid-scanline changes.
  - Copper writes to sprite registers, scroll registers, foreground/background,
    and border need renderer-side application tests.
  - Collision registers are not populated by the Rust/browser renderer.
  - Text scroll/window behavior needs parity tests against Avalonia.
  - `GTEXT` and any remaining VGC command IDs need an explicit command matrix.
- Audio is not full Rust hardware:
  - SID/WTS/music registers are mostly stubs or passive registers in Rust.
  - The WebAudio path currently belongs to the .NET worker path; Rust needs
    timed SID/WTS event output with the same register contract.
- NIC is not full hardware:
  - Rust currently reports ready/send-ready style status, not real browser
    networking queues or sockets.
- DMA/blitter timing is not hardware-faithful:
  - Transfers complete synchronously. Avalonia models busy/status progression
    over cycles. Decide whether browser Rust must model busy timing exactly, then
    test it.
- FIO coverage is broad but not complete:
  - SID/MID/music load/play commands and host media edge cases need parity tests.
- USB HID diagnostics are stubbed.
- Help/compiler/meta registers are partially RAM-backed or ignored.
- Host controls are still runtime-focused:
  - Board input switches/buttons, network controls, and audio enablement need
    page-level controls once Rust devices exist.

## Definition Of Done

- Every memory-mapped hardware range in `VgcConstants.cs` has a Rust equivalent
  with an explicit status: implemented, intentionally unsupported in browser, or
  removed from the platform contract.
- Each implemented range has Rust unit tests for register semantics and at least
  one browser integration smoke when rendering, host I/O, audio, or networking is
  involved.
- BASIC, Logo, Forth, NovaZ, Pascal, assembler, and chess boot and exercise their
  expected hardware paths under Rust at the target browser speed.

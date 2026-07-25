# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project

NovaVM is a custom 6502/65C02 computer platform kept aligned across three implementation targets:

- **Avalonia** (`e6502.Avalonia`) — reference desktop machine and development UI
- **Verilator** (`e6502.FPGA/test`) — RTL simulation used to validate hardware before flashing
- **ULX3S FPGA + ESP32 NovaHost** (`e6502.FPGA`, `e6502.ESP32/novahost`) — the hardware target

The design rule: custom hardware is not an emulator trick. The same register model and memory
behavior should exist in Avalonia, Verilator, and FPGA. When you change a device, check whether
the other targets need the matching change.

`AGENTS.md` also applies to this repo and is not repeated here.

## Nova CLI — required reading

Repo operations belong in the `nova` CLI (`e6502.Nova`). Do **not** add `.sh` or `.py` entry
points; add or reuse a `nova` command. See `docs/script-inventory.md` for the short list of
allowed exceptions.

Before choosing or inventing a workflow, read `docs/books/nova-cli-guide/chapters/nova-cli.md`.
It covers NDI disk images, drive slots, remote NovaHost operations, codegen, and CI bootstrap.

Every `nova` command change must update three things together:
1. the command's `help` output,
2. `docs/books/nova-cli-guide/chapters/nova-cli.md`,
3. `e6502UnitTests/NovaCliCoverageTests.cs`.

## Build & Test

```bash
make binaries                                    # build+stage ALL ROM/module/app .bin from source
dotnet restore e6502.sln
dotnet build e6502.sln -c Release -maxcpucount:1  # serialized: shared ROM/lib artifacts race in parallel
dotnet test e6502.sln -c Release --no-build
```

`make binaries` is the canonical resource entrypoint. Several `.csproj` files reference generated
`.bin` resources (`petscii_*.bin`, `ehbasic.bin`, module/app blobs) as pre-existing files, so a
clean checkout fails to build without it. It also stages copies into `e6502.CLI/Resources`,
`e6502.FPGA/rom`, and the web emulator dirs — create those dirs first on a clean tree
(`mkdir -p e6502.CLI/Resources e6502.FPGA/rom`).

`e6502.Avalonia` runs `make` for the ROM and NDK modules during build, so `ca65`/`ld65` (cc65)
must be on `PATH`. Pass `-p:SkipRomBuild=true` to build against pre-staged resources instead.

Running targets:

```bash
dotnet run --project e6502.Avalonia        # desktop reference host
dotnet run --project e6502.CLI             # headless BASIC host
dotnet run --project e6502.MCP             # MCP bridge (needs Avalonia host running)
dotnet run --project e6502.Nova -- --help  # Nova CLI
```

Tests are MSTest in `e6502UnitTests` (plus xUnit + Avalonia.Headless in `e6502.NovaPanel.Tests`):

```bash
dotnet test --filter "FullyQualifiedName~Vgc"    # pattern
dotnet test --filter "RunAllSuiteTest"           # single test
```

FPGA:

```bash
make -C e6502.FPGA                           # build Verilator sim
make -C e6502.FPGA run
make -C e6502.FPGA/test                      # all Verilator RTL tests
make -C e6502.FPGA/test test_vgc_sprites     # one RTL test
make -C e6502.FPGA/boards/ulx3s bitstream    # needs yosys/nextpnr-ecp5/ecppack
```

CI is Woodpecker on Linux (`.woodpecker/`), not GitHub Actions: it runs
`nova ci install-linux-deps build`, `make binaries`, then a serialized Release build and test.

## Solution Layout

| Project | Purpose |
|---|---|
| `e6502` | Core CPU/opcode library (net10.0) — knows nothing about hardware |
| `e6502.Avalonia` | Reference machine: VGC, audio, storage, network, editors, TCP bridge |
| `e6502.CLI` | Terminal NovaBASIC host |
| `e6502.MCP` | MCP server bridging AI clients to the Avalonia host over TCP |
| `e6502.Nova` | The `nova` CLI: NDI images, NovaHost assets, codegen, CI, web admin |
| `e6502.Storage` | NDI and host-directory storage abstractions |
| `e6502.NovaHost` | Client library for NovaHost management (6504 CBOR) and debug (6503 JSON) |
| `e6502.NovaPanel` (+ `.Tests`) | Avalonia remote-control app for Nova hardware |
| `e6502.GameServer` | TCP game server speaking the Nova NIC message framing |
| `e6502.Browser` / `.RustCore` | WASM host + Rust `no_std` core |
| `e6502.Tools` | Host-side utilities (SID relocation) |
| `e6502Debugger` | WinForms debugger, net6.0-windows, Windows only |
| `e6502UnitTests` | MSTest suite (CPU ROM suites + device/storage/editor/compiler tests) |

Non-.NET trees: `e6502.FPGA/` (SystemVerilog RTL, Verilator tests, board flows),
`e6502.ESP32/novahost/` (ESP32 firmware), `software/` (65C02 source), `docs/books/` (PDF sources).

Namespace prefix: `KDS.e6502`.

## Architecture

### CPU (`e6502/CPU.cs`)

Sealed class, full 6502 + 65C02 instruction set. Two-phase execution: `ClocksForNext()`
(prefetch + cycle count) then `ExecuteNext()`. Simple runners can call `ExecuteNext()` alone.
The CPU takes an `IBusDevice` (`Read(ushort)` / `Write(ushort, byte)`) and knows nothing about
ROM or I/O. `E6502Type` selects NMOS vs CMOS (CMOS adds instructions, fixes the JMP-indirect
page-wrap bug, clears decimal on BRK/IRQ). Interrupts are `IrqWaiting`/`NmiWaiting` flags set
externally and checked at the start of each execute cycle; NMI wins over IRQ.

Bus implementations: `BusDevice` (flat 64KB, tests), `BasicBusDevice` (CLI),
`CompositeBusDevice` (Avalonia — full map with register interception over a flat `byte[]`).

### OpCodes (`e6502/OpCodes/`)

The opcode table is parsed at startup from a fixed-width text resource (`OpCodeList.txt`), not
hardcoded in C#. `OpCodeTable` is a 256-entry array; `OpCodeRecord.Dasm()` disassembles;
`AddressModes` includes 65C02 modes (`ZeroPage0` for `(zp)`, `BranchExt` for BBRx/BBSx).
Edit the text file, not the execution logic, to change instruction metadata.

### Memory map

`docs/help/guides/memory-map.md` is the authoritative map — read it rather than trusting a
summary here. Broad shape: zero page/stack, vectors at `$0200`, BASIC RAM to `$9FFF`, VGC and
sprite/NIC/WTS registers in `$A000-$A1FF`, FIO/XMC/timer/music/DMA/blitter/board-input/math-copro
registers in `$B9A0-$BBCD`, four 256-byte XRAM windows at `$BC00-$BFFF`, ROM at `$C000-$FFFF`
with SID registers intercepted at `$D400-$D43F`.

### Avalonia devices (`e6502.Avalonia/Hardware/`)

- **VGC** (`VirtualGraphicsController.cs`) — 80x50 text + attributes, 320x200 4-bit graphics,
  16 16x16 multicolor sprites, tiles, copper lists. Register writes to the command port trigger
  immediate primitives (plot/line/circle/rect/fill/paint), sprite ops, and bulk memory I/O.
  Renders at 60Hz: background → priority-0 sprites → text/gfx (order by mode) → priority-1/2
  sprites → collision detection.
- **DMA / Blitter** (`VirtualDmaController.cs`, `VirtualBlitterController.cs`) — bulk copy/fill
  across CPU RAM, VGC memory spaces, sprites, tiles, and XRAM; blitter adds stride and color key.
- **XMC** (`VirtualExpansionMemoryController.cs`) — 512KB XRAM as a flat 24-bit space, exposed to
  the CPU through the four mapped windows.
- **SID** (`SidChip.cs`) — software 6581: 3 voices, ADSR, 4 waveforms, ring mod, sync, filter.
  Extended with per-voice volume at `$1D-$1F` (not on real hardware). Rendered on a background
  OpenAL thread at 44100Hz, decoupled from CPU clock.
- **MusicEngine** (`MusicEngine.cs`) — 14-voice sequencer at 60Hz: voices 0–5 → SID, 6–13 → WTS.
  MML parsed by `MmlParser`. `I<n>` = SID instrument, `@I<n>` = WTS GM program, `@D<n>` = WTS drum.
- **WavetableSynth** (`WavetableSynth.cs`) — 8-voice SF2 sample playback (samples stay in host
  memory), reverb/chorus, stereo PCM16 via OpenAL.
- **NIC** (`VirtualNetworkController.cs`) — 4 slots, connect or listen/accept, length-prefixed
  messages, DMA to/from 6502 RAM, optional per-slot IRQ.
- **FIO** (`FileIoController.cs`) — save/load of CPU RAM and VGC memory spaces, directory
  enumeration, MIDI playback, music forwarding. User files live on mounted NDI devices.

### TCP / MCP

`Ipc/EmulatorTcpServer.cs` listens on port 6502 (`EMULATOR_PORT` overrides) with a
newline-delimited JSON protocol exposing screen I/O, graphics, sprites, audio, and file
management. `e6502.MCP` is a separate process mapping `[McpServerTool]` methods onto those
commands; `EnterBasicLine` uppercases outside quotes, `RunProgram` types RUN and waits for Ready.

### Runtimes (`software/languages/`)

A non-BASIC runtime replaces NovaBASIC in the ROM slot rather than running alongside it.
NovaBASIC (`ehbasic/`) is the default and the only one verified on FPGA. NovaLogo, NovaForth,
and NovaPascal build their own 16KB ROM images. Shared 65C02 libraries live in
`software/runtime/asm/` (XRAM, vtext, NDK primitives); `software/ndk/` packages them for
out-of-tree development. Generated headers (`novavm.inc`, runtime ABI, NDK reference) come from
`nova codegen` — regenerate rather than hand-editing.

## FPGA / ULX3S gotcha

Boards silkscreened **v3.0.8 can actually carry the v3.1.x pinout** (`wifi_en=J5`,
`wifi_gpio0=F1` instead of `F1`/`L2`). If ESP32 serial flashing fails silently ("No serial data
received"), check this first — see `e6502.FPGA/boards/ulx3s/BOARD.md`. After the first serial
flash, NovaHost updates go over OTA (`--port novahost.local`).

Debugging workflow: `docs/fpga-debugging-workflow.md`.

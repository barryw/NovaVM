# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project

MOS 6502 CPU emulator in C#/.NET. Supports both NMOS 6502 and CMOS 65C02 variants with full instruction set coverage including decimal mode and interrupt handling. Includes an Avalonia GUI with a virtual graphics controller, SID sound chip emulation, sprite engine, music sequencer, and EhBASIC interpreter.

## Build & Test Commands

```bash
dotnet build                                    # Build all projects
dotnet test                                     # Run all tests
dotnet test --filter "RunAllSuiteTest"           # Run a single test by name
dotnet test --filter "FullyQualifiedName~Vgc"    # Run tests matching a pattern
dotnet test --verbosity normal                  # Verbose test output
dotnet run --project e6502.Avalonia             # Run Avalonia GUI
dotnet run --project e6502.CLI                  # Run CLI (console BASIC)
dotnet run --project e6502.MCP                  # Run MCP server (connects to GUI via TCP)
```

CI runs on Windows with `dotnet build -c Release` then `dotnet test -c Release`.

## Solution Structure

| Project | Target | Purpose |
|---------|--------|---------|
| `e6502` | net10.0 | Core emulator library (CPU, bus, opcodes) |
| `e6502UnitTests` | net10.0 | MSTest tests using real 6502 test ROMs + hardware tests |
| `e6502.Avalonia` | net10.0 | Avalonia GUI with VGC, SID, sprites, music, TCP server |
| `e6502.MCP` | net10.0 | MCP server bridging AI clients to the emulator via TCP |
| `e6502.CLI` | net10.0 | Console-based BASIC interpreter |
| `e6502.Tools` | net10.0 | CLI tool for SID file relocation |
| `e6502Debugger` | net6.0-windows | WinForms debugger (Windows only) |

## Namespace

`KDS.e6502` — all core types use this namespace prefix.

## Architecture

### CPU (`e6502/CPU.cs`)

Sealed class implementing the full 6502 instruction set. Key design:
- Two-phase execution: `ClocksForNext()` (prefetch + cycle count) then `ExecuteNext()` (run instruction). Simple runners can call `ExecuteNext()` alone; cycle-accurate systems use both.
- Receives an `IBusDevice` via constructor — the CPU knows nothing about ROM, I/O, or hardware.
- CPU type selected via `E6502Type` enum (NMOS vs CMOS). CMOS adds new instructions, fixes JMP indirect page-wrap bug, clears decimal flag on BRK/IRQ.
- Interrupts: `IrqWaiting` and `NmiWaiting` bool flags set externally; checked at start of each execute cycle. NMI has priority over IRQ. IRQ is maskable via the I flag.

### Bus (`IBusDevice` / `BusDevice`)

`IBusDevice` has two methods: `Read(ushort)` and `Write(ushort, byte)`. Every CPU memory access routes through this interface. Implementations:
- `BusDevice` — flat 64KB RAM, used by tests
- `BasicBusDevice` (CLI) — 64KB RAM with ROM at $C000+ (writes dropped) and console I/O at $F001/$F004
- `CompositeBusDevice` (Avalonia) — full memory map with hardware register interception

### OpCodes (`e6502/OpCodes/`)

Immutable opcode table parsed at startup from a fixed-width text file embedded as a .resx resource (`OpCodeList.txt`). Not hardcoded in C# — easy to audit and modify without touching execution logic.
- `OpCodeTable` — 256-entry array indexed by opcode byte
- `OpCodeRecord` — instruction metadata with `Dasm()` for disassembly
- `AddressModes` — 15-value enum including 65C02 modes (`ZeroPage0` for `(zp)`, `BranchExt` for BBRx/BBSx)

### Avalonia Memory Map (`CompositeBusDevice`)

Hardware registers are intercepted from a flat 64KB `byte[]` array at Read/Write time:

```
$0000-$01FF  Zero page + stack
$0200-$027F  Vector table (device base addresses, written at boot)
$0280-$9FFF  BASIC program space (~40KB)
$A000-$A01F  VGC registers + command interface
$AA00-$B1CF  Character RAM (2000 bytes, 80x25)
$B1D0-$B99F  Color RAM (2000 bytes, 80x25)
$B9A0-$B9EF  FileIoController registers
$BA00-$BA3F  Expansion Memory Controller (XMC) registers
$BA40-$BA4F  Timer Controller registers
$BA50-$BA53  Music status + voice note readback
$BC00-$BFFF  XMC window (4x256-byte pages into 512KB XRAM)
$C000-$FFFF  ROM (EhBASIC, write-protected)
$D400-$D41C  SID chip registers (inside ROM range, intercepted on Write)
```

### Virtual Graphics Controller (`VGC`)

Manages three layers: 80x25 text, 320x200 block graphics (4-bit color per pixel), and 16 multicolor 16x16 sprites. Register writes to $A010 trigger immediate commands for graphics primitives (plot, line, circle, rect, fill, paint), sprite manipulation (define, position, enable, flip, priority), and memory I/O (bulk read/write of screen/color/gfx/sprite data).

Rendering at 60Hz: background fill → priority-0 sprites → text/gfx layers (order depends on mode) → priority-1 sprites → priority-2 sprites → collision detection.

### SID Chip & Music Engine

**SidChip** (`Hardware/SidChip.cs`): Software MOS 6581 emulation with 3 voices, ADSR envelopes, 4 waveforms, ring modulation, sync, and state-variable filter. Audio rendered in a background thread via OpenAL at 44100Hz, fully decoupled from CPU clock.

**MusicEngine** (`Hardware/MusicEngine.cs`): 3-voice MML sequencer on top of SidChip, ticked at 60Hz. Supports 16 instrument slots, one-shot SFX with voice stealing, and per-frame effects (arpeggio, vibrato, portamento, PWM/filter sweep). MML parsed by `MmlParser` into `MmlEvent` lists.

**SidPlayer** (`Hardware/SidPlayer.cs`): Plays `.sid` files by injecting an IRQ trampoline into CPU RAM that calls init/play routines at 60Hz.

### FileIoController (`Hardware/FileIoController.cs`)

6502-accessible file I/O at $B9A0-$B9EF. Commands: Save/Load (CPU RAM ↔ `.bas` files), GSave/GLoad (VGC memory spaces ↔ `.gfx` files), DirOpen/DirRead, Delete, and music/sound forwarding to MusicEngine. Save directory: `~/e6502-programs`.

### TCP Server & MCP

**TCP** (`Ipc/EmulatorTcpServer.cs`): Listens on port 6502 (or `EMULATOR_PORT` env var). Newline-delimited JSON request/response protocol. Exposes the full emulator surface: screen I/O, graphics, sprites, SID playback, music engine, file management.

**MCP** (`e6502.MCP/`): Separate process bridging Model Context Protocol (stdio) to the TCP server. `EmulatorClient` is a singleton TCP client. All `[McpServerTool]` methods in `EmulatorTools` map to TCP commands. Higher-level helpers: `EnterBasicLine` uppercases outside quotes; `RunProgram` types RUN and waits for Ready.

### EhBASIC Integration

ROM (`ehbasic.bin`, 16KB) loaded at $C000. Reset vector at $FFFC points to monitor code at $FF80 which sets up the vector table and jumps to EhBASIC cold start. BASIC I/O wired to VGC: output via `STA $A00E` (RegCharOut), input via `LDA $A00F` (RegCharIn) polling the `ScreenEditor` key queue.

## Tests

MSTest framework. CPU correctness validated by running real 6502 test suite binaries (Klaus Dormann's functional/interrupt tests, AllSuiteA, 65C02 extended opcodes) to completion — each loops on itself when done, test asserts final PC value.

Hardware tests cover VGC commands, SID registers, timer interrupts, composite bus routing, sprite rendering/collision, SID file parsing/relocation, MML parsing, and music engine sequencing.

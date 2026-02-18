# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project

MOS 6502 CPU emulator in C#/.NET. Supports both NMOS 6502 and CMOS 65C02 variants with full instruction set coverage including decimal mode and interrupt handling.

## Build & Test Commands

```bash
dotnet build                                    # Build all projects
dotnet test                                     # Run all tests
dotnet test --filter "RunAllSuiteTest"           # Run a single test by name
dotnet test --verbosity normal                  # Verbose test output
dotnet run --project e6502.CLI                  # Run CLI (Enhanced BASIC)
dotnet run --project e6502.Avalonia             # Run Avalonia GUI
```

CI runs on Windows with `dotnet build -c Release` then `dotnet test -c Release`.

## Solution Structure

| Project | Target | Purpose |
|---------|--------|---------|
| `e6502` | net10.0 | Core emulator library (CPU, bus, opcodes) |
| `e6502UnitTests` | net10.0 | MSTest tests using real 6502 test ROMs |
| `e6502.CLI` | net7.0 | Console-based BASIC interpreter |
| `e6502.Avalonia` | net10.0 | Avalonia GUI with pixel rendering, sprites, TCP server |
| `e6502.MCP` | net10.0 | MCP server for AI-driven interaction |
| `e6502Debugger` | net6.0-windows | WinForms debugger (Windows only) |

## Architecture

**CPU** (`e6502/CPU.cs`) — The core. Sealed class implementing the full 6502 instruction set. Key design:
- Two-phase execution: `ClocksForNext()` (prefetch/cycle count) then `ExecuteNext()` (run instruction)
- Receives an `IBusDevice` via constructor for memory access
- Registers: A, X, Y, PC (16-bit), SP (stack at 0x0100-0x01FF, descending)
- Status flags: N, V, B, D, I, Z, C
- CPU type selected via `E6502Type` enum (NMOS vs CMOS)
- Interrupts: `IrqWaiting` and `NmiWaiting` flags trigger on next execute

**Bus** (`e6502/BusDevice.cs`) — Implements `IBusDevice` interface (Read/Write). Default is a flat 64KB RAM array. Avalonia uses `CompositeBusDevice` routing through VGC, VSC, FileIO with ROM protection at $C000+.

**OpCodes** (`e6502/OpCodes/`) — Immutable opcode table loaded from embedded resource strings:
- `OpCodeTable` — 256-entry array indexed by opcode byte
- `OpCodeRecord` — Instruction metadata (mnemonic, addressing mode, bytes, cycles) with `Dasm()` for disassembly
- `OpCodeReader` — Parses the fixed-format resource text
- `AddressModes` — Enum of 13 addressing modes

**Utilities** — `CPUMath` (BCD conversion, sign extension), `Int` extensions (`.Hex2()`, `.Hex4()` formatting)

## Namespace

`KDS.e6502` — all core types use this namespace prefix.

## Tests

MSTest framework. Tests load real 6502 test suite binaries from `e6502UnitTests/Resources/` and run them to completion (PC loops on itself), then assert final PC and memory values. The interrupt test manually triggers IRQ/NMI at specific PC addresses during execution.

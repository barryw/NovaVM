# SID Emulation Design

## Overview

Replace the current Virtual Sound Controller (VSC) with a real MOS 6581 SID chip emulation, add periodic interrupt infrastructure, build a SID file loader with BASIC and MCP interfaces, and create a standalone relocator CLI tool.

## SID Chip Emulation

Replace `VirtualSoundController` with a new `SidChip` class based on [cadaver's SID.cs](https://github.com/cadaver/oldschoolengine2/blob/master/Assets/Scripts/SID.cs) (MIT license, ~410 lines). Based on Hermit's jsSID. Implements all core SID features: 3 oscillators (triangle, saw, pulse with variable width, noise), ADSR envelopes with rate tables, LP/BP/HP filter with resonance, oscillator hard sync, ring modulation, and waveform combinations.

### Register Mapping

SID registers at $D400-$D41C (29 bytes), matching the real C64 layout:

| Range | Purpose |
|-------|---------|
| $D400-$D406 | Voice 1 (freq lo/hi, pulse width lo/hi, control, AD, SR) |
| $D407-$D40D | Voice 2 |
| $D40E-$D414 | Voice 3 |
| $D415-$D417 | Filter (cutoff lo/hi, res/routing) |
| $D418 | Mode/volume (filter mode + master volume) |

### Bus Routing

$D400-$D41C falls inside the ROM region ($C000-$FFFF). In `CompositeBusDevice`:
- **Writes** to $D400-$D41C route to the SID emulation
- **Reads** from $D400-$D41C return ROM bytes as normal

This means SID player code from .sid files works unmodified — it writes to the same addresses as on the real C64.

### Audio Output

Strip Unity dependencies from cadaver's code. Keep `Clock()` and sample generation. The SID emulation clocks at the CPU rate (~1MHz) and downsamples to 44.1kHz. Feed samples into the existing `OpenAlRenderer` via the same callback pattern the VSC uses today.

### What Changes

- **Deleted:** `VirtualSoundController.cs` entirely, VSC constants in `VgcConstants.cs` ($A100 block), the command-based interface (VscCmd, VscCmdSound, etc.)
- **Extracted:** `OpenAlRenderer` inner class moves to its own file — it's audio-backend-agnostic and reusable
- **Created:** `SidChip` class that owns the renderer and maps registers at $D400-$D41C

## Timer Infrastructure

Two interrupt sources, both generating IRQs through the CPU's existing `IrqWaiting` flag.

### VGC Raster Interrupt

A single enable bit added to the VGC. When enabled, the existing 60Hz render tick in `MainWindow.axaml.cs` sets `_cpu.IrqWaiting = true` alongside the frame counter increment. One new VGC register (e.g., `RegIrqEnable`). Read to get status, write to enable/disable. Simple, zero-config 60Hz interrupt.

### Programmable Timer

A new `VirtualTimerController` mapped into the free space at $BA40-$BFFF. Minimal register set:

| Register | Purpose |
|----------|---------|
| Frequency divisor (16-bit) | Interrupt rate = base clock / divisor. 50Hz: 20000, 60Hz: 16667 |
| Control | Enable/disable, acknowledge IRQ |
| Status | IRQ pending flag |

The timer runs on a dedicated background thread. When it fires, it sets `_cpu.IrqWaiting = true`.

### IRQ Routing

Both sources share the same IRQ line. The 6502 IRQ handler checks status registers to determine the source and acknowledges by reading/writing the appropriate register.

### Rationale

The raster interrupt is dead simple for common cases (60Hz game loops, basic SID playback). The programmable timer handles PAL tunes (50Hz), precise musical timing, or any other rate.

## SID File Loading

### PSID Format

The PSID header is 124 bytes (v2+). Key fields: magic ("PSID"), version, load address, init address, play address, song count, default song, speed flags. The data payload is raw 6502 machine code loaded at the specified address.

### BASIC Commands

- `SIDPLAY "filename"` — loads a .sid file, calls init, starts background playback via the timer interrupt
- `SIDSTOP` — stops playback, disables the timer IRQ

Under the hood: parse the PSID header, copy payload into RAM at the load address, JSR to the init routine with song number in A register, set up the IRQ vector ($FFFE) to point to a small trampoline, enable the timer at the correct rate (speed flags: 0 = 50Hz VBI, 1 = CIA timing from header).

### IRQ Trampoline

A small block of 6502 code (~20-30 bytes) injected into a known safe location (e.g., top of the free space around $BA40). It saves registers, JSRs to the play address, acknowledges the timer IRQ, restores registers, RTI.

### MCP Tools

- `sid_play` / `sid_stop` — same logic as BASIC commands, exposed over TCP
- `sid_info` — reads metadata (title, author, copyright from the PSID header)

### SID File Parser

`SidFileParser` utility class in `e6502.Avalonia`. Returns a struct with header fields and payload byte array. Shared by both BASIC command path and MCP tools.

## SID Relocator CLI Tool

### Purpose

Standalone CLI tool that takes a .sid file and outputs a new .sid file with the code relocated to a target address that fits safely in the e6502 RAM ($0280-$9FFF).

### The Hard Problem

Distinguishing absolute addresses (need patching) from data bytes or immediate values (must not touch). `LDA $D400` clearly references an address, but `.byte $D4, $00` in a data table does not — and they look identical in the binary.

### Approach

Use the `e6502` core library's `OpCodeTable` to do a control-flow-following disassembly starting from the init and play entry points. Follow branches and jumps to identify code paths. Anything reachable is code — patch its absolute operands. Anything unreachable is assumed to be data — leave it alone. Not perfect (self-modifying code, computed jumps), but handles the vast majority of SID players.

The PSID v2 header includes `startPage` and `pageLength` fields describing the player's free memory region, which helps validate the target address choice.

### CLI Interface

New project `e6502.Tools`:

```
sidreloc input.sid output.sid --target 0x1000
sidreloc input.sid --info
```

### Priority

Last in the build order. Most SID files from HVSC load into $0800-$3FFF and work without relocation.

## Implementation Order

### Phase 1 — SID Chip
- Port cadaver's SID.cs, strip Unity dependencies
- Extract `OpenAlRenderer` into its own file
- Create `SidChip` class with register mapping at $D400-$D41C
- Update `CompositeBusDevice` bus routing
- Delete `VirtualSoundController` and VSC constants
- Test: poke SID registers via MCP tools, hear sound

### Phase 2 — Timer Infrastructure
- Add raster interrupt enable register to VGC
- Wire 60Hz render tick to `IrqWaiting` when enabled
- Create `VirtualTimerController` in $BA40 free space
- Update `CompositeBusDevice` to route timer addresses
- Test: 6502 program that enables the timer and counts IRQs

### Phase 3 — SID File Loading
- Create `SidFileParser` utility
- Add `SIDPLAY`/`SIDSTOP` BASIC commands
- Add `sid_play`/`sid_stop`/`sid_info` MCP tools
- Test: load a .sid file, hear it play

### Phase 4 — Relocator
- Create `e6502.Tools` project
- Control-flow-following disassembler using `OpCodeTable`
- Address patcher for absolute operands
- CLI with `--target` and `--info` flags
- Test: relocate a known .sid file, verify it plays identically

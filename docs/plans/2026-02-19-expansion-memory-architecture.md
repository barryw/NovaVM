# Expansion Memory Architecture Plan (512 KB+)

## Goal
Add expansion RAM (starting at 512 KB, scalable to multi-MB) that feels natural for both BASIC users and assembly programmers, without breaking the 6502 16-bit address model.

## Design Summary
Use a new virtual hardware device: **XMC** (Expansion Memory Controller).

- Layer 1 (low-level): banked + linear addressing, DMA copy engine, byte data port.
- Layer 2 (high-level): optional named blocks/handles (`STASH`/`FETCH`) with usage reporting.
- Optional fast path: map small expansion windows into CPU space for tight assembly loops.

This keeps advanced power available while preserving a simple BASIC workflow.

## Current Memory Constraints
Existing map already reserves:
- `$A000-$B99F`: VGC/VSC/FIO/char/color RAM
- `$BA00-$BFFF`: currently free (best place for XMC)
- `$C000-$FFFF`: ROM

## XMC Hardware Proposal

### Register Block
Place XMC registers at **`$BA00-$BA3F`**.

Core registers:
- `$BA00 XCMD` command trigger
- `$BA01 XSTAT` status (`busy`, `error`, `alloc-enabled`, `mapped`)
- `$BA02 XERR` error code
- `$BA03 XCFG` config flags
- `$BA04 XAL` linear addr low
- `$BA05 XAM` linear addr mid
- `$BA06 XAH` linear addr high (supports up to 16 MB with 24-bit)
- `$BA07 XRL` CPU RAM addr low
- `$BA08 XRH` CPU RAM addr high
- `$BA09 XLL` length low
- `$BA0A XLH` length high
- `$BA0B XDATA` data port (read/write with optional auto-inc)
- `$BA0C XBANK` current 64 KB bank (for bank:offset mode)
- `$BA0D XBANKS` total bank count (read-only)
- `$BA0E XPAGES` used pages (read-only, allocator mode)
- `$BA0F XFREEP` free pages (read-only, allocator mode)

### Commands
- `01`: read byte at `XADDR` -> `XDATA`
- `02`: write `XDATA` -> `XADDR`
- `03`: copy RAM -> XRAM (`XR`/`XLEN`/`XADDR`)
- `04`: copy XRAM -> RAM
- `05`: fill XRAM (`XDATA` value, `XLEN`)
- `06`: copy XRAM -> XRAM
- `07`: query stats refresh
- `08`: clear allocator metadata

### Optional Mapped Windows
Map four 256-byte windows for direct CPU access:
- `$BC00-$BCFF` window 0
- `$BD00-$BDFF` window 1
- `$BE00-$BEFF` window 2
- `$BF00-$BFFF` window 3

Window page selectors live in `XMC` registers. This gives assembly code fast indexed access (`LDA $BC00,X`) without full DMA setup.

## Addressing Model
Support both at once:

1. **Linear mode** (recommended API): `0..N-1` byte addresses.
2. **Bank:offset mode** (retro-friendly): `bank (0..63) + 16-bit offset`.

Linear mode is simpler for BASIC, bank mode is familiar for assembly/monitor workflows.

## BASIC API (User-Facing)

### Phase 1: Raw Power
- `XMEM` -> prints total/used/free expansion RAM + bank count
- `XBANK n` -> select default bank for bank:offset ops
- `XPOKE xaddr, value`
- `XPEEK(xaddr)`
- `STASH ramAddr, xAddr, len`  (RAM -> expansion)
- `FETCH ramAddr, xAddr, len`  (expansion -> RAM)
- `XFILL xAddr, len, value`

### Phase 2: Seamless Named Storage
- `STASH "name", ramAddr, len` (allocate/update named block)
- `FETCH "name", ramAddr`
- `XDEL "name"`
- `XDIR` (list named blocks, size, bank/page usage)
- `XFREE()` returns free expansion bytes

This gives users easy data persistence semantics inside RAM, without manual bank math.

### Phase 3: Structured Data Helpers
- `XPUT ramAddr, "name", offset, len`
- `XGET "name", offset, ramAddr, len`
- `XALLOC(len)` / `XREL(handle)` / `XHANDLE("name")`

## Assembly API

### Register-Level Interface
Assembly can talk to `$BA00` directly for full control.

### ROM Helper Routines (recommended)
Add small ROM-call ABI wrappers:
- `XM_SETADDR` (load 24-bit XADDR)
- `XM_GETBYTE`
- `XM_PUTBYTE`
- `XM_STASH` (RAM->XRAM)
- `XM_FETCH` (XRAM->RAM)
- `XM_FILL`
- `XM_STATUS`

Call convention example:
- `A/X/Y` for pointers and lengths
- zero-page workspace for extended args
- carry set on error, `A = XERR`

This keeps assembly code compact and avoids repeated register boilerplate.

## Use Cases

### BASIC
- Huge level/map tables beyond base RAM
- Sprite/frame caches
- Music pattern banks and instrument tables
- Undo/redo buffers for editors
- Multi-program workspace snapshots

### Assembly
- Asset streaming into working RAM
- Double-buffered graphics data staging
- Fast decompression targets in XRAM
- Banked code/data overlays
- Runtime memory pools for large engines

## “Mostly Seamless” UX Features
- `XMEM` one-line dashboard on boot or command (`banks`, `used`, `free`, `%`).
- `XDIR` shows named allocations like a RAM-disk catalog.
- Deterministic allocator (fixed-size page bitmap, e.g. 256-byte pages).
- Optional `AUTOSTASH ON` profile mode for transparent BASIC variable snapshots (future).

## Capacity Targets
- v1: 512 KB (`8 x 64 KB banks`)
- v2: 2 MB (`32 banks`)
- v3: 4 MB+ (`24-bit linear`, no BASIC syntax changes)

## Implementation Plan

### Step 1: Device + Raw Commands
- Implement `VirtualExpansionMemoryController` in Avalonia host.
- Add XMC constants in `VgcConstants.cs`.
- Route `$BA00-$BFFF` in `CompositeBusDevice`.
- Add EhBASIC commands: `XMEM`, `STASH`, `FETCH`, `XPOKE`, `XPEEK`, `XBANK`.

### Step 2: Named Block Layer
- Add metadata table in controller (host-side first).
- Add `XDIR`, `XDEL`, name-based `STASH`/`FETCH`.

### Step 3: Assembly Helpers
- Add ROM helper routine entry points and docs/macros.
- Expose XMC base through page-2 vector table (next free slot, e.g. `+0x0C`).

### Step 4: Window Mapping (optional fast path)
- Add 4 x 256-byte mapped windows at `$BC00-$BFFF`.
- Add map/unmap commands and tests.

## Testing Strategy
- Unit tests for register semantics, DMA boundaries, error codes.
- BASIC integration tests for `STASH/FETCH` round-trip.
- Stress test with random copies and checksums across full 512 KB.
- Performance tests: DMA throughput and windowed access loops.

## Compatibility / Safety
- Keep existing RAM/ROM map intact for default behavior.
- XMC is opt-in; no mapping enabled unless requested.
- Invalid range operations return `XERR` and never crash emulator state.

## Recommended First Cut
Build **raw linear STASH/FETCH + XMEM + XPEEK/XPOKE** first.

That delivers immediate value, minimal ROM token pressure, and a clean path to named objects and assembly quality-of-life features afterward.

# XRAM Flat Addressing And Runtime ABI Design

*Date: 2026-04-30*

## Summary

Standardize Nova XRAM as a flat 24-bit byte-addressed memory space. Assembly
programs and language runtimes should treat XRAM as one contiguous data region,
normally `$000000` through `$07FFFF` for the current 512 KB implementation.
BASIC may keep its `XBANK` plus 16-bit offset model as a user-facing
convenience, but that should compile down to the same flat address model.

The goal is to avoid a proliferation of one-off XRAM conventions. Programs
should use one shared software ABI that chooses the right primitive underneath:
mapped windows for small/random access, DMA for bulk movement, and FIO
`XLOAD`/`XSAVE` commands for file-to-XRAM and XRAM-to-file transfers.

This contract must behave the same in Avalonia, Verilator simulation, and FPGA
hardware.

## Goals

- Define XRAM as a flat 24-bit address space for assembly and runtime code.
- Keep `XBANK` plus 16-bit offset as a BASIC compatibility layer only.
- Establish one common 65C02-facing XRAM runtime ABI for languages, tools, and
  standalone assembly programs.
- Use the four XMC windows as the canonical random-access mechanism.
- Use DMA as the canonical bulk-transfer mechanism.
- Add direct `XLOAD` and `XSAVE` paths for streaming files to and from XRAM.
- Make backend behavior identical across Avalonia, Verilator, and FPGA.
- Document edge cases: range checks, page alignment, window ownership, and
  blocking/asynchronous transfer behavior.

## Non-goals

- Do not remove existing BASIC commands such as `XPEEK`, `XPOKE`, `STASH`,
  `FETCH`, `XMAP`, or `XUNMAP`.
- Do not force every use case through one hardware path. Windows and DMA both
  remain necessary.
- Do not expose backend-specific shortcuts as public language/runtime APIs.
- Do not define a full memory allocator or garbage collector in this document.
  Allocation policy can be layered above the flat-address ABI.

## Addressing Model

The canonical XRAM address is a 24-bit little-endian byte address:

```
xaddr = high:mid:low
range = $000000-$07FFFF   ; 512 KB
```

For the current 512 KB machine, only 19 address bits are valid. Address bytes
above that range must be treated as out of range. Backends should not wrap
`$080000` to `$000000`.

This is enough for standard Z-machine story files through Z8: the standard
limits V1-V3 story files to 128 KB, V4-V5 to 256 KB, and V6-V8 to 512 KB.
The 24-bit ABI gives Nova headroom if a future board exposes more XRAM, while
the current 512 KB implementation is already large enough for the maximum
standard story image.

Useful derived values:

```
page   = xaddr >> 8       ; 256-byte page number
offset = xaddr & $FF      ; byte within page
bank   = xaddr >> 16      ; BASIC-visible 64 KB bank
```

The BASIC model maps cleanly onto the flat model:

```
xaddr = (XBANK << 16) | offset16
```

That rule should be the only meaning of `XBANK`. Assembly code should prefer
explicit 24-bit addresses and should not rely on global bank state except when
calling BASIC compatibility routines.

## Canonical Access Paths

### Random Access: XMC Windows

Small reads, writes, table walks, parsers, and interpreters should use the four
mapped XRAM windows:

| Window | CPU Range | Size |
| --- | ---: | ---: |
| 0 | `$BC00-$BCFF` | 256 bytes |
| 1 | `$BD00-$BDFF` | 256 bytes |
| 2 | `$BE00-$BEFF` | 256 bytes |
| 3 | `$BF00-$BFFF` | 256 bytes |

Window base registers accept a 24-bit address, but the effective mapping is
page-aligned:

```
mapped_page_base = xaddr & $FFFF00
cpu_byte         = window_base_cpu + (xaddr & $FF)
```

The low byte of the window base should be written as `$00` by new code. Existing
code that writes a non-zero low byte should still work because the effective
address is rounded down to the page boundary.

Windows are the right primitive for:

- byte and word reads/writes,
- interpreter program counters,
- object/table/property traversal,
- dictionary scans,
- format parsers,
- and random-access language runtime data.

### Bulk Access: DMA

Block movement should use DMA space `5` for XRAM. Blitter support can remain for
2D graphics-specific operations, but generic XRAM runtime copies should use DMA
so every language and assembly program gets one predictable flat-copy path.

Use this path for:

- CPU RAM to XRAM copies,
- XRAM to CPU RAM copies,
- XRAM fills,
- XRAM to VGC memory transfers,
- VGC memory to XRAM snapshots,
- and large contiguous runtime buffers.

Assembly-level APIs should accept 24-bit XRAM addresses for bulk operations.
BASIC wrappers may continue to accept a 16-bit offset plus current `XBANK`, but
they should translate that to the same 24-bit address internally.

The public runtime ABI should provide blocking routines by default. DMA is
asynchronous in Avalonia, and FPGA implementations may stall or expose busy
status. The wrapper routine must wait or poll until the transfer is complete
before returning. Optional async routines can exist later, but they should be
explicit.

### File Access: FIO XLOAD / XSAVE

File transfer should be handled by FIO commands that can target XRAM directly:

- `XLOAD`: file to XRAM
- `XSAVE`: XRAM to file
- optionally a streaming block read/write API for runtimes that want manual
  control

These commands should also use flat 24-bit XRAM addresses. They should not be
specified in terms of `XBANK` except in BASIC syntax sugar.

`XLOAD`/`XSAVE` are part of the platform contract, not Z-machine-specific
features. They are required for Z-machine story files, large graphics assets,
compiler inputs, save-state images, and any language runtime that handles data
larger than CPU RAM.

## Proposed 65C02 Runtime ABI

The exact symbol names can change, but all language runtimes should converge on
one small API shape.

The ABI should be packaged as a reusable assembly runtime, not as BASIC-private
or Z-machine-private support code. The first deliverable should be:

```
xram.inc    ; constants, zero-page contract, macros, public symbols
xram.s      ; linkable ca65 implementation of the public routines
```

Standalone assembly programs link `xram.s`. Language runtimes such as NCC emit
calls to the same routines or inline the documented macros where appropriate.
BASIC keeps its commands, but the BASIC command implementations should be
refactored into parser wrappers over the same runtime rather than remaining a
separate XRAM implementation.

The runtime must not require EhBASIC parser state, `FAC1`, token pointers, or
the global `XBANK` register except in explicit BASIC compatibility entry points.
It also should not require hidden allocator metadata in XRAM unless the caller
opts into an allocator layer.

Recommended packaging:

- Put hardware constants in the generated `novavm.inc`.
- Put reusable XRAM call declarations, scratch symbols, and macros in
  `xram.inc`.
- Put reusable implementation code in `xram.s`.
- Link the same `xram.s` into the extension ROM for BASIC and into standalone
  assembly programs that need XRAM access.
- Keep BASIC argument parsing in `basic.asm`, but have it populate the shared
  ABI scratch block and call the common routines.

### Call Contract

The runtime needs a stable call convention so code generators and handwritten
assembly can share it. A practical first version:

```
xram_addr0..2   ; 24-bit XRAM address, little-endian
xram_addr1..3   ; second 24-bit XRAM address for copies
xram_ram0..1    ; 16-bit CPU RAM address
xram_len0..2    ; 24-bit length for DMA/file helpers, low 16 bits for v1 loops
xram_value      ; byte value or return byte
xram_status     ; 0=ok, nonzero=error
```

The exact zero-page locations should be assigned once and documented in
`xram.inc`. Every public routine should document clobbered registers, clobbered
windows, and whether it waits for DMA completion.

### Window Helpers

```
xram_map_window     ; map xaddr page into window N
xram_read8          ; read byte at 24-bit xaddr
xram_read16le       ; read little-endian word
xram_read16be       ; read big-endian word, useful for Z-machine data
xram_write8         ; write byte at 24-bit xaddr
xram_write16le      ; write little-endian word
xram_write16be      ; write big-endian word
xram_fetch_pc8      ; read byte from a caller-managed 24-bit PC and increment it
xram_fetch_pc16be   ; read big-endian word from PC and increment by 2
```

These routines may clobber documented scratch registers and window mappings.
Callers that need stable mappings must either reserve a window or save/restore
mapping state.

`xram_map_window` should cache the current page mapped into each window. Tight
loops such as opcode fetch, string decode, and dictionary scans should avoid
rewriting XMC window registers when the requested address is already mapped.

### Bulk Helpers

```
xram_copy_from_ram  ; CPU RAM -> XRAM
xram_copy_to_ram    ; XRAM -> CPU RAM
xram_copy_to_vgc    ; XRAM -> VGC memory space
xram_copy_from_vgc  ; VGC memory space -> XRAM
xram_fill           ; fill XRAM range with byte value
xram_copy           ; XRAM -> XRAM
```

All helpers use flat XRAM addresses. Lengths should be at least 16-bit in the
first version. If a larger transfer is requested through a higher-level API, the
runtime can split it into chunks.

These helpers should program the DMA controller, wait for completion, and map
DMA errors into the shared `xram_status` value. BASIC `STASH`, `FETCH`, raw
screen snapshots, compiler staging, and Z-machine story movement should all use
these helpers instead of direct per-command copies.

### File Helpers

```
xram_load_file      ; file -> XRAM
xram_save_file      ; XRAM -> file
```

`xram_load_file` and `xram_save_file` should call FIO `XLOAD` and `XSAVE` once
those commands exist. Until then, only the Z-machine smoke-test packer should use
chunked CPU-RAM staging as a temporary bootstrap path.

Chunking should not be replicated in every application. For a Z-machine MVP the
packer can emit `STORYnn.bin` chunks; later `xram_load_file` should transparently
switch to FIO `XLOAD` without changing the interpreter.

## Z-Machine Required Subset

The Z-machine should be treated as the first demanding client of this ABI. It
needs the following subset before interpreter work can move quickly:

- Flat 24-bit story addressing, with no dependence on `XBANK`.
- Fast cached window reads for opcode fetch, packed strings, dictionary tables,
  object tables, property tables, and globals.
- Big-endian word helpers, because Z-machine memory words are big-endian.
- Page-crossing-safe reads and writes at every 256-byte boundary.
- Bulk load from disk into XRAM, initially chunked and eventually direct
  FIO-to-XRAM.
- Bulk save from XRAM/CPU state to disk for save/restore.
- A runtime-owned window convention so the interpreter can keep PC, table,
  write, and scan windows stable.

The first engine should not use named XRAM blocks for the story image. The story
layout is deterministic and should be owned by the interpreter:

| Region | Suggested Owner |
| --- | --- |
| `$000000..story_len-1` | Story image and mutable dynamic memory |
| next aligned page | Interpreter scratch or decompression staging |
| high XRAM pages | Optional undo, scrollback, or save staging |

If an allocator is added later, it should accept a caller-provided arena range so
it cannot collide with a runtime-owned story image or with BASIC's interactive
named-block state.

## Window Ownership Rules

XRAM window mappings are global hardware state. There is no automatic process
isolation.

Recommended convention:

- A standalone runtime owns all four windows while it is running.
- A subroutine must document which windows it clobbers.
- IRQ/NMI handlers that use XRAM must save and restore any window registers they
  touch, or reserve a dedicated window by convention.
- BASIC `XMAP`/`XUNMAP` is an interactive/manual feature. Assembly runtimes
  should not assume BASIC-managed mappings remain intact after runtime calls.

For complex runtimes such as a Z-machine interpreter, reserve windows by role:

| Window | Suggested Use |
| --- | --- |
| 0 | instruction stream / current PC |
| 1 | general reads |
| 2 | writes / mutable structures |
| 3 | scans, strings, dictionary, scratch |

This is a convention, not a hardware requirement.

## Backend Consistency Requirements

### Avalonia

Avalonia's virtual XMC should enforce the same flat-address behavior:

- valid range is `$000000-$07FFFF`,
- out-of-range reads return a documented harmless value and writes have no
  effect or report an error through command status,
- window mappings are page-aligned,
- DMA space `5` reads and writes the same XRAM array,
- `XBANK` wrappers translate to `(bank << 16) | offset`.

### Verilator

The Verilator path should match FPGA synthesis behavior, not just approximate
it:

- same window register behavior,
- same page alignment semantics,
- same initial `XmcBanks` value,
- same `XmcWinCtl` enable behavior,
- same range failure behavior,
- same DMA XRAM space behavior,
- same CPU stall or wait semantics where observable.

Simulation-only shortcuts are fine only if tests prove they are behaviorally
equivalent.

### FPGA

FPGA hardware should provide the same contract through SDRAM-backed XRAM:

- CPU window reads/writes go through the XRAM SDRAM wrapper.
- DMA XRAM reads/writes use the same 24-bit logical address space.
- CPU stalls or waits while XRAM access is in flight.
- Back-pressure must not drop DMA writes.
- Debug/ESP SDRAM preload paths must not define a separate user-facing XRAM
  model.

## BASIC Compatibility Layer

BASIC keeps its existing mental model:

```
XBANK n
XPEEK(offset)
XPOKE offset,value
STASH ramaddr,offset,length
FETCH ramaddr,offset,length
XMAP window,offset
```

Those commands should be documented as bank-relative wrappers over flat XRAM:

```
flat_xaddr = (XmcBank << 16) | offset
```

Named blocks can continue to allocate from the full flat XRAM address space.
They should return or store flat addresses internally even if the user-facing
display remains bank-oriented.

BASIC command implementation should be split into two parts:

- BASIC parser wrappers: parse strings, offsets, lengths, and the current
  `XBANK` state.
- Shared XRAM calls: perform the actual byte access, DMA copy, fill, mapping,
  allocation, and file streaming work.

That keeps the existing BASIC surface while making the underlying routines
usable by assembly programs, tools, compilers, and game runtimes.

## Required Platform Work

### 1. Split Reusable Runtime From BASIC

Current XRAM support is mostly exposed through BASIC commands and ROM helper
labels. That is useful for interactive use, but it is not a reusable language
runtime boundary. The reusable layer should be callable from a booting assembly
program even if BASIC parser state is irrelevant or unavailable.

Required work:

- Add `xram.inc` and `xram.s` with the ABI above.
- Move common register setup, window mapping, byte/word access, and blocking
  DMA transfer loops into that runtime.
- Keep BASIC parsing code in BASIC, but have BASIC-compatible helpers translate
  `XBANK + offset` into the same 24-bit calls.
- Document which routines require the BASIC ROM, which require the extension
  ROM, and which are pure hardware-register routines.

### 2. Decide XMC Command Ownership

Nova currently has register-level XMC behavior in Avalonia and BASIC-side
extension routines that also know how to process XMC-like operations. That split
is risky as a cross-language contract because different clients may accidentally
depend on different command owners or metadata schemes.

Required work:

- Define which operations are hardware commands and which are firmware/library
  routines.
- Ensure Avalonia, Verilator, and FPGA execute the same command set with the
  same status/error behavior.
- Avoid two independent allocation/named-block metadata implementations for the
  same XRAM bytes.
- Treat the shared `xram.s` layer as the public assembly API even if it calls
  hardware commands underneath.

Recommended split:

- XMC hardware owns capacity, status, byte data port, window registers, and
  optional allocator bookkeeping.
- DMA owns all generic bulk copies involving XRAM.
- FIO owns direct file streaming through `XLOAD` and `XSAVE`.
- `xram.s` owns the public call convention and chooses the right hardware path.
- BASIC owns only parsing and bank-relative user syntax.

### 3. Add Direct File To XRAM Paths

Chunked story files are acceptable for the first Z-machine smoke test, but they
should not become the permanent asset-loading model.

Required work:

- Add FIO `XLOAD` and `XSAVE`.
- Add 24-bit destination/source XRAM registers and 24- or 32-bit result size
  registers.
- Extend `FileIoController` so it can read/write XRAM through injected
  delegates, similar to the existing VGC memory callbacks.
- Preserve exact file length for checksums and Z-machine header validation.
- Add NDI and host-directory tests for files larger than 64 KB.

### 4. Expand Tests Beyond BASIC Commands

BASIC integration tests prove the interactive surface, but a reusable ABI needs
assembly-level coverage.

Required work:

- Add integration tests that link or include the shared `xram.s` runtime.
- Cover reads/writes across `$0000FF`, `$00FFFF`, and `$07FFFF`.
- Cover out-of-range `$080000` behavior.
- Cover window low-byte rounding and window clobber rules.
- Cover DMA XRAM copies with flat 24-bit addresses.
- Add a Z-machine-style checksum smoke test that reads a story-sized binary from
  XRAM without using BASIC commands.

## Test Matrix

Every backend should have equivalent coverage for:

- byte read/write at `$000000`,
- byte read/write at `$0000FF` and `$000100`,
- byte read/write across a 64 KB boundary, e.g. `$00FFFF` to `$010000`,
- byte read/write at `$07FFFF`,
- out-of-range access at `$080000` does not wrap,
- window mapping ignores/rounds down the low base byte,
- DMA CPU RAM -> XRAM at a 24-bit address,
- DMA XRAM -> CPU RAM at a 24-bit address,
- XRAM fill across at least one 256-byte page boundary,
- BASIC `XBANK n` plus offset produces the same flat address as assembly,
- Verilator and FPGA agree on initial `XmcWinCtl` and `XmcBanks`.

Future tests should add:

- FIO `XLOAD` into XRAM,
- FIO `XSAVE` from XRAM,
- large file load near 512 KB,
- and concurrent/runtime window ownership tests.

## Implementation Notes

- Prefer adding common assembly include/runtime files instead of duplicating
  ad-hoc XRAM routines in every program.
- Keep register constants synchronized between `ehbasic/novavm.inc`,
  `VgcConstants.cs`, FPGA RTL, and generated docs.
- Treat direct writes to XMC registers as low-level implementation detail for
  the shared runtime, not as the recommended app-level interface.
- Use flat 24-bit addresses in new docs, tests, and APIs. Mention `XBANK` only
  when describing BASIC syntax.

## Open Questions

- Which hot routines should be mirrored in the extension ROM versus linked into
  each standalone assembly program?
- Should `XmcWinCtl` power-on state enable all windows or require explicit
  mapping before use?
- Should command-status error behavior be identical for window accesses, or are
  unmapped/out-of-range window reads allowed to return a harmless value?
- How much of the allocator should be standardized now versus deferred until a
  language runtime needs it?

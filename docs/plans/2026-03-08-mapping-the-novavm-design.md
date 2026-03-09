# Mapping the NovaVM — Design Document

## Overview

A complete address-by-address reference for the e6502 virtual computer, modeled on Sheldon Leemon's "Mapping the Commodore 64." Covers every byte from $0000 to $FFFF with register names, R/W status, default values, bit-field tables, prose explanations, and practical BASIC examples. Styled as a companion volume to the NovaBASIC User Guide.

## Key Design Decisions

1. **Full Sheldon Leemon style** — every address gets its own entry with name, type, default, prose, bit tables for flags, and POKE/PEEK examples.
2. **Sequential with chapter grouping** — addresses in order from $0000 to $FFFF, organized into chapters by logical subsystem.
3. **SID in its own chapter** — pulled out of the ROM range to avoid interrupting ROM documentation. Cross-referenced at $D400.
4. **Off-bus memory in appendices** — VGC bitmap, sprite shape RAM, XRAM, and WTS samples documented as appendices.
5. **Full VGC command reference inline at $A010** — all 30+ commands with parameter layouts and examples at the register entry.
6. **Full EhBASIC internals** — every zero page location, tokenized line format, variable storage, string heap, ROM entry points.
7. **Matched companion styling** — same retro blue cover, cream paper, callout boxes, and 1980s handbook tone as the User Guide.

## Chapter Structure

| Ch | Title | Address Range | Est. Pages |
|----|-------|---------------|------------|
| 1 | Zero Page | $0000–$00FF | ~30 |
| 2 | The Stack | $0100–$01FF | ~5 |
| 3 | The Vector Table | $0200–$027F | ~5 |
| 4 | BASIC Program Space | $0280–$9FFF | ~20 |
| 5 | The Virtual Graphics Controller | $A000–$A0BF | ~60 |
| 6 | The Network Interface Controller | $A100–$A13F | ~20 |
| 7 | The Wavetable Synthesizer | $A140–$A1DF | ~25 |
| 8 | Screen Memory | $AA00–$B99F | ~10 |
| 9 | The File I/O Controller | $B9A0–$B9EF | ~25 |
| 10 | The Expansion Memory System | $BA00–$BFFF | ~40 |
| 11 | The ROM | $C000–$FFFF | ~15 |
| 12 | The SID Sound Chips | $D400–$D43F | ~30 |
| — | Front matter, index | — | ~10 |
| **Total** | | | **~295** |

### Appendices

- **A: VGC Bitmap & Sprite Shape RAM** — 64,000-byte graphics bitmap layout, 256-slot sprite shape format, access via DMA and VGC MemRead/MemWrite commands.
- **B: Expansion RAM (XRAM)** — 512KB layout, bank structure, named block allocation, window mapping.
- **C: WTS Sample Memory** — SF2 soundfont loading, sample storage, host-side memory model.
- **D: Color Table** — All 16 NovaVM colors with names, indices, and RGB values.
- **E: ASCII & Screen Code Table** — Character codes, control characters, and screen editor behavior.

## Per-Address Entry Format

### Standard Entry

```
$A001  RegBgCol  (R/W)  Default: 0
Background Color

Sets the background color for the entire text/graphics display.
Write a value from 0-15 to select from the NovaVM color palette.
This register can be read back at any time.

The BASIC command COLOR fg,bg writes this register directly.

  POKE $A001, 6     : REM blue background
  PRINT PEEK($A001) : REM read current color
```

### Bitfield Entry

For flag/status registers, include an 8-bit diagram:

```
$A01F  RegIrqCtrl  (R/W)  Default: 0
IRQ Control Register

  Bit  7   6   5   4   3   2   1   0
       -   -   -   -   -   -   -  RSTR

  RSTR (bit 0): Raster IRQ enable
    0 = disabled (default)
    1 = fire IRQ when raster reaches trigger line

  Bits 7-1: Reserved, always read 0

  POKE $A01F, 1  : REM enable raster IRQ
```

### Command Trigger Entry

For registers like $A010 (RegCmd), $B9A0 (FioCmd), $BA00 (XmcCmd):
- Command summary table listing all codes
- Sub-entry per command code with parameter register layout and BASIC example

### RAM Range Entry

For contiguous RAM (stack, BASIC space, CharRAM):
- Prose overview with address math
- Representative entries showing individual byte usage

### Cross-References

Every entry gets `\label{addr:XXXX}` for cross-referencing. Entries reference related addresses with "see $XXXX" hyperlinks.

## EhBASIC Internals Coverage

### Zero Page ($00–$7E)

Every byte documented with its interpreter role:
- Key 16-bit pointer pairs (start of program, start of variables, start of arrays, end of arrays, bottom of string heap, top of memory)
- Interpreter workspace (current line number, DATA pointer, FOR/NEXT depth, expression temporaries, I/O state)

### Program Storage Format (Chapter 4)

- **Line format:** next-line pointer (2 bytes) + line number (2 bytes) + tokenized content + $00 terminator
- **Program terminator:** $00 $00
- **Token table:** byte values mapped to keywords
- **Variable storage:** 2-byte name (with type encoding) + 4-byte float value
- **String variables:** length + pointer into string heap
- **String heap:** grows downward from top of memory, garbage collected
- **Array storage:** dimension count, sizes, packed values

### ROM Entry Points (Chapter 11)

- Cold start, warm start addresses
- I/O vectors (character in, character out)
- Key interpreter entry points
- Monitor initialization code at $FF80

## LaTeX Implementation

### Visual Identity

Reuses the User Guide's LaTeX preamble:
- Book class, 11pt, A4
- Colors: `retroblue`, `retrocyan`, `retrogray`
- Cream paper background, blue chapter header bars
- Callout box environments: Note, Warning, Tip, Try It
- Monospace code blocks with syntax highlighting

### Cover Page

Adapted from User Guide `cover.tex`:
- Title: "MAPPING THE NOVAVM"
- Subtitle: "A Complete Address-by-Address Reference"
- Tagline: "For the e6502 Virtual Computer"
- Bottom feature line: "64KB Address Space | 12 Hardware Controllers | 512KB Expansion"

### New LaTeX Commands

- **`\mementry{addr}{name}{rw}{default}`** — Address entry header with monospace hex address, register name, R/W badge, default value. Auto-creates `\label{addr:XXXX}` and index entry.
- **`\bitfield{...}`** — 8-bit register diagram with named bit fields in a ruled grid.
- **`\cmdentry{code}{name}`** — Sub-entry for command codes within trigger registers.
- **Address index** — Back-of-book index sorted by hex address using `imakeidx` with custom formatting.

### File Structure

```
docs/mapping/
  mapping_the_novavm.tex          (master document + preamble)
  cover.tex                       (TikZ cover page)
  copyright.tex                   (copyright page)
  chapters/
    ch01-zero-page.tex
    ch02-stack.tex
    ch03-vector-table.tex
    ch04-basic-program-space.tex
    ch05-vgc.tex
    ch06-nic.tex
    ch07-wts.tex
    ch08-screen-memory.tex
    ch09-file-io.tex
    ch10-expansion-memory.tex
    ch11-rom.tex
    ch12-sid.tex
    appendix-a-vgc-offbus.tex
    appendix-b-xram.tex
    appendix-c-wts-samples.tex
    appendix-d-color-table.tex
    appendix-e-ascii-table.tex
```

## What We Skip

- Host-side implementation details (C# classes, threading)
- MCP/TCP protocol (developer API, not hardware)
- NCC compiler internals (registers documented, compiler logic out of scope)
- Re-teaching BASIC syntax (cross-reference User Guide instead)

## Cross-References Between Documents

- This document references: "see NovaBASIC User Guide, Chapter N"
- User Guide can reference: "see Mapping the NovaVM, $XXXX"

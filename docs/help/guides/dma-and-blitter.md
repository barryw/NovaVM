---
title: "DMA and Blitter"
type: guide
category: DMA
---

> Speed is not a luxury -- it is the difference between a program that feels alive and one that feels like it is apologising.

-- e6502 Virtual Computer Design Notes

Every POKE loop that copies a screen buffer byte-by-byte is burning CPU cycles
that your program could be spending on game logic, animation, or music.
NovaBASIC includes two hardware accelerators that move data without occupying
the CPU: the **DMA controller** for flat bulk transfers and the
**blitter** for two-dimensional rectangular copies and fills.
Both work across six unified memory spaces -- CPU RAM, character RAM, color
RAM, the graphics bitmap, sprite shape memory, and expansion RAM -- so data
can be moved between any combination of those regions in a single command.
This chapter covers both controllers, their BASIC keywords, and the memory
space model they share.

## The DMA Controller

**Direct Memory Access (DMA)** is a hardware technique that transfers
a block of bytes between two memory addresses without executing a load/store
loop on the CPU. The CPU hands a source address, destination address, and
length to the controller and the transfer happens autonomously. From the
perspective of a BASIC program the operation appears instantaneous.

The DMA controller understands six **unified memory spaces**. Each space
is identified by a small integer that you pass as a parameter:

| **ID** | **Space** | **Size** | **Notes** |
| --- | --- | --- | --- |
| 0 | CPU RAM | 64 KB | Full 6502 address space |
| 1 | Character RAM | 2,000 bytes | 80x25 text cells |
| 2 | Color RAM | 2,000 bytes | 80x25 color attributes |
| 3 | Graphics Bitmap | 64,000 bytes | 320x200 pixels, 4-bit color |
| 4 | Sprite Shapes | 32,768 bytes | 256 shape slots x 128 bytes each |
| 5 | Expansion RAM | up to 512 KB | Uses current XBANK |

For spaces 1 through 4, the address parameter is a zero-based byte offset
into that hardware memory region -- so address 0 in space 1 is the first
character cell, address 80 is the start of the second row, and so on.

### `DMACOPY srcSpace, srcAddr, dstSpace, dstAddr, length`

Copies *length* bytes from offset *srcAddr* in *srcSpace* to
offset *dstAddr* in *dstSpace*.

```basic
10 REM Copy 2000 bytes from CPU RAM at $6000 to character RAM
20 DMACOPY 0, 24576, 1, 0, 2000
```

Here space 0 is CPU RAM, `24576` is $6000 decimal, space 1 is
character RAM, and the destination offset is 0 (top-left cell). After line 20
executes, the 2000-byte buffer you prepared in CPU RAM has been written
directly into the text screen.

```basic
10 REM Load graphics data from XRAM bank 0 into the bitmap
20 XBANK 0
30 DMACOPY 5, 0, 3, 0, 64000
```

Space 5 is expansion RAM. The current XBANK (set on line 20) determines which
512 KB bank is addressed. Line 30 copies all 64,000 bytes of the graphics
bitmap from the start of XRAM bank 0.

### `DMAFILL dstSpace, dstAddr, length, value`

Fills *length* bytes starting at offset *dstAddr* in
*dstSpace* with the constant byte *value*.

```basic
10 REM Clear character screen with space characters (ASCII 32)
20 DMAFILL 1, 0, 2000, 32
```

```basic
10 REM Fill color RAM with color 7 (white-on-black attribute)
20 DMAFILL 2, 0, 2000, 7
```

### Status functions

| **Function** | **Returns** |
| --- | --- |
| `DMASTATUS` | 0 = idle, 1 = busy, 2 = done, 3 = error |
| `DMAERR` | 0 = no error; non-zero = hardware error code |
| `DMACOUNT` | Number of bytes transferred by the last operation |

::: note
`DMACOPY` and `DMAFILL` are asynchronous: they start the transfer and
return immediately. Poll `DMASTATUS` until it changes from busy (1) to done
(2) or error (3) when you need to wait for completion.
:::

## The Blitter

The DMA controller treats memory as a flat one-dimensional sequence of bytes.
The **blitter** adds a second dimension: it understands that memory is
arranged as rows of a fixed width and can copy or fill rectangular regions
without touching the bytes between rows.

The key concept is **stride**: the number of bytes from the beginning of
one row to the beginning of the next. For the 80-column text screen, stride is
80. For the 320-pixel-wide graphics bitmap, stride is 160 (320 pixels at 4
bits each, packed into 2 nibbles per byte). If source and destination have
different strides -- for example when copying a narrow tile into a wider
canvas -- you specify each independently.

### `BLITCOPY srcSpace, srcAddr, srcStride, dstSpace, dstAddr, dstStride, width, height`

Copies a rectangle *width* bytes wide by *height* rows tall from
the source region to the destination region.

```basic
10 REM Scroll the character screen up by one row.
20 REM Copy rows 1-24 (offset 80) to rows 0-23 (offset 0),
30 REM then blank the last row.
40 BLITCOPY 1, 80, 80, 1, 0, 80, 80, 24
50 DMAFILL 1, 1920, 80, 32
```

Line 40 copies 24 rows of 80 characters, shifting the entire screen up.
Line 50 fills the vacated bottom row (offset 1920 = 24x80) with spaces.

```basic
10 REM Copy a 10x8 tile from CPU RAM at $3000 into color RAM
20 REM at column 5, row 3 (offset = 3*80+5 = 245).
30 BLITCOPY 0, 12288, 10, 2, 245, 80, 10, 8
```

The source stride is 10 because the tile is stored compactly (row-by-row,
10 bytes per row) in CPU RAM. The destination stride is 80 because color RAM
is 80 columns wide.

### `BLITFILL dstSpace, dstAddr, dstStride, width, height, value`

Fills a rectangle *width* bytes wide by *height* rows tall in
*dstSpace* with the constant byte *value*, respecting row stride.

```basic
10 REM Draw a 20x5 colored rectangle starting at column 10, row 2.
20 REM offset = 2*80+10 = 170. Stride = 80.
30 BLITFILL 2, 170, 80, 20, 5, 9
```

Without the blitter this would require a nested FOR/NEXT loop: one outer loop
over five rows and an inner loop over twenty columns, each issuing a POKE.
`BLITFILL` replaces all of that with a single hardware call.

### Color-key transparency

At the register level, bit 1 of the blitter mode register (`$BA93`)
enables **color-key** mode: source bytes that equal the color key value
(stored in `$BA95`) are skipped, leaving the destination byte
unchanged. This is sprite-style transparency for arbitrary memory regions.
The `BLITCOPY` keyword does not expose this flag directly; use
`POKE $BA93, 2` before issuing a raw blitter start
(`POKE $BA80, 1`) to enable it.

### Status functions

| **Function** | **Returns** |
| --- | --- |
| `BLITSTATUS` | 0 = idle, 1 = busy, 2 = done, 3 = error |
| `BLITERR` | 0 = no error; non-zero = hardware error code |
| `BLITCOUNT` | Number of bytes written by the last operation |

::: tip
Use `DMACOPY` and `DMAFILL` for flat, one-dimensional transfers --
copying a saved screen buffer, filling a region of expansion RAM, or moving a
block of arbitrary bytes. Use `BLITCOPY` and `BLITFILL` when your data
is inherently two-dimensional and you need to respect row boundaries, such as
scrolling text, drawing rectangles, or working with tile graphics. Choosing the
right tool avoids writing stride arithmetic in BASIC loops.
:::

::: note
`BLITCOPY` and `BLITFILL` are also asynchronous. Use
`BLITSTATUS`/`BLITERR`/`BLITCOUNT` to monitor progress and final
result.
:::

## Memory Spaces Reference

The table below summarises all six space IDs accepted by both the DMA
controller and the blitter. Address offsets within each space are always
zero-based.

| **ID** | **Space** | **Size** | **Notes** |
| --- | --- | --- | --- |
| 0 | CPU RAM | 64 KB | Full 6502 address space. Offsets are absolute CPU addresses. Writes to ROM (C000--FFFF) are silently ignored. |
| 1 | Character RAM | 2,000 bytes | 80x25 text cells, row-major. Offset 0 is top-left; offset 79 is top-right; offset 80 is the start of row 1. |
| 2 | Color RAM | 2,000 bytes | One color attribute byte per text cell, same layout as character RAM. Low nibble = foreground, high nibble = background. |
| 3 | Graphics Bitmap | 64,000 bytes | 320x200 pixels, two 4-bit pixels packed per byte (low nibble = left pixel). Row stride is 160 bytes. |
| 4 | Sprite Shapes | 32,768 bytes | 256 shape slots, 128 bytes each. Each slot holds a 16x16 pixel image, two pixels per byte. Sprites reference slots via shape index registers. |
| 5 | Expansion RAM | up to 512 KB | The current XBANK setting is automatically applied as the high address byte. Set `XBANK` before any DMA or blitter operation that uses space 5. |

::: note
When space 5 (XRAM) is the source or destination, the DMA and blitter
controllers read the current XBANK register at the moment the transfer starts.
Changing XBANK mid-transfer is not safe; always set the desired bank before
issuing the command.
:::

## DMA and Sprite Shape Memory

`DMACOPY` can transfer shape data directly into sprite shape memory
(space 4). This is the fastest way to swap sprite animations at runtime:
pre-store multiple animation frames in expansion RAM and copy them on demand.

```basic
10 REM Copy one 128-byte shape from XRAM offset 0 to shape slot 0
20 XBANK 0
30 DMACOPY 5, 0, 4, 0, 128
```

Shape slot addresses are computed as `slot x 128`. For example,
shape slot 5 starts at byte offset 640 in sprite shape space:

```basic
10 REM Copy shape data from XRAM into slot 5
20 DMACOPY 5, 0, 4, 640, 128
```

::: tip
DMA writes to sprite shape memory are **glitch-free** -- the renderer
snapshots shape data at the start of each frame, so transfers can happen at
any time without waiting for vblank or disabling sprites. This makes
DMA-driven sprite animation safe and straightforward.
:::

## Try It Now

::: tryit
The program below combines all four commands to produce a continuously
scrolling rainbow of color bars using only hardware accelerators -- no inner
loops, no per-byte POKEs.

```basic
10  REM -- DMA & Blitter Demo -- 20  REM Clear screen 30  DMAFILL 1, 0, 2000, 32 40  DMAFILL 2, 0, 2000, 0 50  REM Draw 25 horizontal color bars, one per row 60  FOR I = 0 TO 24 70    BLITFILL 2, I*80, 80, 80, 1, I MOD 16 80  NEXT I 90  REM Scroll colors up continuously 100 BLITCOPY 2, 80, 80, 2, 0, 80, 80, 24 110 BLITFILL 2, 24*80, 80, 80, 1, RND(1)*16 120 VSYNC 130 GOTO 100
```

Lines 30--40 clear both character and color RAM in two hardware calls.
Lines 60--80 paint 25 horizontal bars of different colors by calling
`BLITFILL` once per row. Lines 100--130 form an animation loop:
`BLITCOPY` shifts the entire color layer up by one row, `BLITFILL`
inserts a new random-color bar at the bottom, and `VSYNC` holds the
frame rate at 60 Hz.

**Experiments to try:**
- Change the fill value on line 30 to something other than 32 (ASCII
space) to see different characters fill the screen -- try 42
(asterisk) or 64 (at-sign).
- Replace space 2 on lines 70, 100, and 110 with space 1 to scroll
character codes instead of colors.
- Try `DMACOPY 1, 0, 2, 0, 2000` to mirror the character RAM
layout into color RAM and see the effect on the display.
- Use space 5 (XRAM) as a destination: stash a full color-screen
snapshot with `DMACOPY 2, 0, 5, 0, 2000` and restore it later.

:::

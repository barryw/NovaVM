---
title: "Graphics and Sprites"
type: guide
category: Graphics
---

> A blank screen is a canvas waiting to be claimed.

-- e6502 Virtual Computer Design Notes

NovaBASIC gives you direct access to a 320x200 pixel bitmap and a
hardware sprite layer. Drawing commands operate on 16 colors; sprites add
independently positioned, independently animated 16x16 objects on top
of or behind the bitmap and text layers. This chapter covers the full
graphics pipeline from mode selection to collision detection.

## Display Modes

The virtual display has two independent layers: a text layer and a graphics
bitmap layer. `MODE` selects how they are composited.

| **Mode** | **Description** |
| --- | --- |
| 0 | Text only. The graphics bitmap is not rendered. |
| 1 | Graphics over text. Bitmap is drawn on top of text characters. |
| 2 | Text over graphics. Text characters are drawn on top of the bitmap. |
| 3 | Graphics and sprites only. No text layer is rendered. |

The typical starting sequence for any graphics program is:

```basic
10 MODE 1
20 GCLS
30 GCOLOR 7
```

`MODE 1` activates pixel rendering. `GCLS` clears the bitmap to
transparent (color 0). `GCOLOR` sets the active drawing color for all
subsequent drawing commands.

::: note
Color 0 is transparent in the graphics layer. Setting a pixel to color 0
with `PLOT 0` or `FILL` erases it, letting the text layer or
background show through. This is equivalent to `UNPLOT`.
:::

To return to plain text output, switch back to `MODE 0`. You do not
need to clear the bitmap when switching modes; the pixel data is preserved
and will reappear if you switch back to `MODE 1` or `MODE 2`.

## Drawing Commands

All drawing commands use the color set by `GCOLOR`. Coordinates must
fall within the screen boundaries of X = 0--319 and Y = 0--199; pixels
outside that range are silently clipped and no error is raised.

### Command reference

| **Command** | **Description** |
| --- | --- |
| GCOLOR c | Set the active drawing color. `c` is 0--15. If `c` = 0, NovaBASIC uses the current text foreground color instead of transparent. |
| GCLS | Clear the entire graphics bitmap to transparent (color 0). Does not affect the text layer. |
| PLOT x,y | Set the pixel at (`x`,`y`) to the current drawing color. |
| UNPLOT x,y | Set the pixel at (`x`,`y`) to transparent (color 0), effectively erasing it. |
| LINE x0,y0,x1,y1 | Draw a straight line from (`x0`,`y0`) to (`x1`,`y1`) in the current drawing color. |
| RECT x0,y0,x1,y1 | Draw a rectangle outline. (`x0`,`y0`) is the top-left corner; (`x1`,`y1`) is the bottom-right corner. |
| FILL x0,y0,x1,y1 | Draw a solid filled rectangle using the same corner convention as `RECT`. |
| CIRCLE cx,cy,r | Draw a circle outline centered at (`cx`,`cy`) with radius `r` pixels. |
| PAINT x,y | Flood-fill from seed point (`x`,`y`), replacing all connected pixels of the same color with the current drawing color. |

### A drawing example

The following program draws a diagonal cross, a circle, and then fills the
circle interior:

```basic
10 MODE 1 : GCLS
20 GCOLOR 9
30 LINE 0,0,319,199
40 LINE 319,0,0,199
50 GCOLOR 14
60 CIRCLE 160,100,60
70 GCOLOR 10
80 PAINT 160,100
90 VSYNC
```

Line 30--40 draws a white cross from corner to corner. Lines 60--80 add a
yellow circle outline and then flood-fill the interior with green. `VSYNC`
on line 90 holds the image for one frame before the program ends; without it
the display may update before you see the result.

::: tip
`PAINT` stops at pixel boundaries of a different color. Make sure the
circle or region you want to fill has no gaps, otherwise the fill will leak
out into the surrounding area. If in doubt, draw the boundary in one step and
fill immediately after.
:::

## Animation with VSYNC

The virtual display runs at 60 Hz. `VSYNC` suspends program execution
until the start of the next video frame. One `VSYNC` call therefore
consumes exactly one frame period ( 16.7 ms). This is the correct
tool for controlling animation speed.

A minimal animation loop that moves a point across the screen:

```basic
10 MODE 1 : GCLS : GCOLOR 11
20 X = 0 : Y = 100
30 VSYNC
40 UNPLOT X, Y
50 X = X + 2
60 IF X > 319 THEN X = 0
70 PLOT X, Y
80 GOTO 30
```

The pattern is always: wait for VSYNC, erase the old position, update
coordinates, draw the new position. Erasing before moving eliminates the
ghost trail that builds up if you draw without erasing.

::: tip
For smooth movement, do all erase operations for a frame, update all
positions, and do all draw operations -- all within a single VSYNC period.
Never call `VSYNC` between the erase and redraw steps for the same
object; that produces a one-frame flicker every cycle.
:::

## Sprites

Sprites are hardware-accelerated 16x16 pixel objects that move
independently of the bitmap. NovaBASIC supports 16 sprites (indices 0--15),
each with its own shape, position, priority, and flip state. Sprites do not
modify the bitmap; they are composited at render time.

### Enabling and positioning sprites

A sprite must be enabled before it becomes visible:

```basic
10 SPRITE 0, ON
20 SPRITE 0, 160, 100
```

`SPRITE n,ON` activates sprite `n`. `SPRITE n,x,y` sets its
screen position. Positions are in the same coordinate space as the bitmap
(X = 0--319, Y = 0--199). Sprites may be positioned partially or fully
off-screen; they are simply clipped without error.

To hide a sprite, use `SPRITE n,OFF`. This makes the sprite invisible
without erasing its shape data. You can re-enable it later with
`SPRITE n,ON` and it will reappear at its last recorded position.

### Sprite priority

Priority controls which layer a sprite is drawn on:

| **Priority** | **Layer position** |
| --- | --- |
| 0 | Behind all layers (below text and graphics) |
| 1 | Between the text and graphics layers |
| 2 | In front of all layers (above text and graphics) |

Priority is set via the MCP sprite tools when building shapes interactively;
it can also be arranged by designing your program so that background sprites
are enabled first and foreground sprites last.

### Defining sprite pixels with SPRITEDATA

Each sprite is 16 pixels wide by 16 pixels tall. Pixel data is loaded one
row at a time using `SPRITEDATA`:

`SPRITEDATA n, row, b1, b2, b3, b4, b5, b6, b7, b8`

- `n` is the sprite index (0--15).
- `row` is the row to define (0--15, top to bottom).
- `b1`--`b8` are eight byte values (0--255).

Each byte encodes *two* pixels. The high nibble (upper four bits) is the
left pixel; the low nibble (lower four bits) is the right pixel. Color 0 is
transparent; colors 1--15 are the standard 16-color palette. With eight bytes
per row and two pixels per byte, each row is exactly 16 pixels wide.

### How sprite byte encoding works

The diagram below shows one row of sprite data -- 8 bytes producing 16 pixels.
Each byte is split into two 4-bit nibbles, each selecting a color index:

In the example above, `$BB` (decimal 187) fills both nibbles with color
index `B` (11 = cyan), producing two solid cyan pixels. `$0B`
has a transparent left pixel and a cyan right pixel. `$00` leaves both
pixels transparent.

The full 16x16 sprite is 16 such rows stacked vertically, each defined
by a separate `SPRITEDATA` call. The complete shape occupies 128 bytes
(8 bytes x 16 rows):

Each pair of columns shares one byte. The vertical lines mark byte boundaries.
Colored cells are non-transparent pixels; gray cells are transparent (color 0).

The following example defines a simple 16x16 diamond shape in color 11
(cyan) and displays it:

```basic
10 REM DEFINE A DIAMOND SPRITE
20 SPRITEDATA 0,  0, 0,   0,   0,  0,  0,   0,   0,  0
30 SPRITEDATA 0,  1, 0,   0,   0, 11,  0,   0,   0,  0
40 SPRITEDATA 0,  2, 0,   0, 177,177,  0,   0,   0,  0
50 SPRITEDATA 0,  3, 0, 187, 187,187,187,   0,   0,  0
60 SPRITEDATA 0,  4, 0, 187, 187,187,187,   0,   0,  0
70 SPRITEDATA 0,  5, 0,   0, 177,177,  0,   0,   0,  0
80 SPRITEDATA 0,  6, 0,   0,   0, 11,  0,   0,   0,  0
90 SPRITEDATA 0,  7, 0,   0,   0,  0,  0,   0,   0,  0
100 REM ROWS 8-15 REMAIN TRANSPARENT (NO SPRITEDATA = NO CHANGE)
110 SPRITE 0, ON
120 SPRITE 0, 152, 92
130 VSYNC
```

::: note
Any row not explicitly defined by `SPRITEDATA` retains its previous pixel
data. If you are reusing a sprite slot for a new shape, define all 16 rows
(or clear the slot first). Rows you intentionally leave all-zero produce a
fully transparent row.
:::

The bytes in lines 30--80 use decimal notation. Working with hex notation is
often more readable: `0xBB` = decimal 187, which encodes color 11 in both
nibbles (solid cyan on both pixels of that byte). In NovaBASIC you can write
hex literals directly in expressions using `&HBB` notation.

## Sprite Collision Detection

NovaBASIC provides two collision functions that report when sprites overlap
each other or touch non-transparent pixels on the background bitmap.

| **Function** | **Returns** |
| --- | --- |
| COLLISION(n) | Bitmask of other sprites currently overlapping sprite `n`. |
| BUMPED(n) | Bitmask indicating that sprite `n` has touched a non-transparent pixel in the graphics bitmap. |

Both functions return an integer bitmask. Bit `k` being set means sprite
`k` is involved in the collision. For `COLLISION(n)`, if the result
is non-zero then at least one other sprite overlaps sprite `n`; use
`AND` with the appropriate bit to test for a specific sprite. For
`BUMPED(n)`, a non-zero result means sprite `n` is touching a
non-transparent pixel in the graphics bitmap.

::: warning
Both collision registers clear automatically when read. Read each register
exactly once per frame and store the result in a variable. If you call
`COLLISION(n)` or `BUMPED(n)` a second time in the same frame you
will get zero, missing collisions that occurred between reads.
:::

A practical collision loop pattern:

```basic
100 VSYNC
110 C = COLLISION(0)
120 B = BUMPED(0)
130 IF C <> 0 THEN GOSUB 500
140 IF B <> 0 THEN GOSUB 600
150 REM UPDATE POSITIONS HERE
160 GOTO 100
```

Lines 110--120 read both registers once and store them. Lines 130--140 branch
to handler routines only if a collision has occurred. All position updates
happen after the collision check so that the same frame's register values are
used consistently.

To test whether sprite `n` specifically collided with sprite 2, check bit
2 of the `COLLISION` result:

```basic
200 C = COLLISION(0)
210 IF (C AND 4) <> 0 THEN PRINT "HIT SPRITE 2"
```

## Color Palette

NovaBASIC uses a fixed 16-color palette inspired by the Commodore 64.
All graphics, text, sprite, background, and border colors use the same
indices.

| **Index** | **Color** | **Index** | **Color** |
| --- | --- | --- | --- |
| 0 | Black | 8 | Orange |
| 1 | White | 9 | Brown |
| 2 | Red | 10 | Light Red |
| 3 | Cyan | 11 | Dark Grey |
| 4 | Purple | 12 | Medium Grey |
| 5 | Green | 13 | Light Green |
| 6 | Blue | 14 | Light Blue |
| 7 | Yellow | 15 | Light Grey |

The background color defaults to 6 (blue), the text foreground to 1 (white),
and the border to 6 (blue). Use `COLOR fg[,bg]` for text colors.
The border color can be changed via `POKE 40973, c` where `c` is
0--15 ($A00D is the border color register).

## The Copper (Raster Effects)

The copper is a per-pixel register-write system inspired by the Amiga's
Copper coprocessor. It lets you change display registers at precise screen
positions, enabling effects like color gradient backgrounds, split-screen
modes, and parallax scrolling -- all without any CPU involvement.

### How it works

The VGC stores up to 128 independent copper *lists*. Each list is a
program of up to 256 events, each specifying:

- A screen position (X = 0--319, Y = 0--199).
- A target register.
- A value to write to that register.

When copper is enabled, the renderer checks for copper events at every pixel
position. When it reaches a pixel that has a scheduled event, the register
write fires immediately and affects all subsequent pixels on that frame.

### Writable registers

The copper can write to four VGC core registers and the full sprite register
block:

| **Register** | **Address** | **Effect** |
| --- | --- | --- |
| RegMode | $A000 | Change display mode mid-screen |
| RegBgCol | $A001 | Change background color mid-screen |
| RegScrollX | $A005 | Shift horizontal scroll offset |
| RegScrollY | $A006 | Shift vertical scroll offset |
| A040--A0BF | (sprite regs) | All sprite register fields (see below) |

### COPPER BASIC keyword

The `COPPER` keyword provides direct access to the copper system:

| **Syntax** | **Purpose** |
| --- | --- |
| COPPER ADD x, y, BGCOL, value | Add event: set background color at position. |
| COPPER ADD x, y, MODE, value | Add event: set display mode at position. |
| COPPER ADD x, y, SCROLLX, value | Add event: set horizontal scroll at position. |
| COPPER ADD x, y, SCROLLY, value | Add event: set vertical scroll at position. |
| COPPER ADD x, y, SPRX(n), value | Set sprite *n* X low byte at position. |
| COPPER ADD x, y, SPRXH(n), value | Set sprite *n* X high byte at position. |
| COPPER ADD x, y, SPRY(n), value | Set sprite *n* Y low byte at position. |
| COPPER ADD x, y, SPRYH(n), value | Set sprite *n* Y high byte at position. |
| COPPER ADD x, y, SPRSHAPE(n), value | Set sprite *n* shape slot at position. |
| COPPER ADD x, y, SPRFLAGS(n), value | Set sprite *n* flags at position. |
| COPPER ADD x, y, SPRPRI(n), value | Set sprite *n* priority at position. |
| COPPER CLEAR | Remove all events from the current target list. |
| COPPER ON | Enable copper execution each frame. |
| COPPER OFF | Disable copper execution. |
| COPPER LIST n | Set target list to *n* (0--127). Subsequent ADD/CLEAR edit this list. |
| COPPER LIST END | Reset target list back to the active list. |
| COPPER USE n | Switch the active list to *n* at the next vblank. |

For sprite register names, *n* is the sprite index (0--15). Each name
maps to the absolute address `$A040 + n*8 + field_offset`. All eight
sprite register fields, including `TransColor` (+7), are copper-writable.

If an event at the same position for the same register already exists, the
value is replaced. Each list holds up to 256 events.

**Multiple lists.**
The VGC stores 128 copper lists (0--127). By default everything operates on
list 0. Use `COPPER LIST n` to direct subsequent `ADD`/`CLEAR`
commands to list *n*. Use `COPPER USE n` to tell the renderer to
switch to list *n* at the next vertical blank -- this avoids tearing
because the swap happens atomically between frames. Editing a list and
swapping the active pointer are independent, so you can build one list while
the renderer displays another (double buffering).

### Sprite multiplexing with the copper

The VGC has 16 hardware sprites, but the copper can make them appear as many
more by rewriting sprite registers between rows. The technique is called
**sprite multiplexing**: after a sprite's row of pixels has been drawn,
the copper repositions it further down the screen with a different shape slot,
effectively reusing the same hardware sprite for a second (or third) object.

The basic recipe:

1. Set initial sprite positions and shapes for the top of the screen.
2. Use `COPPER ADD` to rewrite `SPRY(n)` and
`SPRSHAPE(n)` at scanlines between the rows of sprites, moving
the sprite to its next position and swapping its shape.
3. At scanline 0 of the next frame, restore the original values so the
top-of-screen sprites reappear.

For example, to display sprite 0 at both Y=20 and Y=120 with different shapes:

```basic
10 REM Set initial position (top instance)
20 SPRITE 0, 100, 20
30 SPRITESHAPE 0, 0
40 SPRITE 0, ON
50 REM Copper: at scanline 70, reposition to Y=120, shape 1
60 COPPER ADD 0, 70, SPRY(0), 120
70 COPPER ADD 0, 70, SPRSHAPE(0), 1
80 REM Restore at scanline 0 for next frame
90 COPPER ADD 0, 0, SPRY(0), 20
100 COPPER ADD 0, 0, SPRSHAPE(0), 0
110 COPPER ON
```

The demo program `docs/programs/sprite_multiplex.bas` shows a complete
working example with multiple multiplexed sprites.

### Low-level copper control via POKE

You can also control the copper by writing directly to the VGC command
registers (A010--A016) and then writing the command byte to $A010.

| **Code** | **Command** |
| --- | --- |
| $1B | **Copper Add** -- add an event to the target list. P0/P1 = X (16-bit), P2 = Y, P3/P4 = register (0--15 or A000--A00F), P5 = value. |
| $1C | **Copper Clear** -- remove all events from the target list. |
| $1D | **Copper Enable** -- start executing the active copper list each frame. |
| $1E | **Copper Disable** -- stop executing the copper program. |
| $20 | **Copper List** -- set target list. P0 = list index (0--127). |
| $21 | **Copper Use** -- set pending active list. P0 = list index (0--127). Takes effect at next vblank. |
| $22 | **Copper List End** -- reset target list back to the active list. |

### Example: background color gradient

The following program creates a vertical color gradient by scheduling a
background color change at the start of each group of rows:

```basic
10 COPPER CLEAR
20 FOR I = 0 TO 12
30   COPPER ADD 0, I * 15, BGCOL, I + 2
40 NEXT I
50 COPPER ON
```

The loop on lines 20--40 adds 13 events, each changing the background color
at Y positions 0, 15, 30, ..., 180 to successive color indices. Line 50
enables the copper. Each frame the background will display as a gradient of
bands.

::: tip
Copper events fire at per-pixel granularity. Setting X = 0 for all events
produces clean horizontal bands. Setting different X values within the same
row creates vertical split effects.
:::

::: note
Each copper list holds up to 256 events. Events are sorted by position
automatically. Multiple events at the same position are applied in
register-index order. The VGC stores 128 lists; all data is host-side and
consumes no 6502 address space.
:::

## Sprite Shape Slots

The VGC provides 256 shape slots (indices 0--255), each storing a
16x16 pixel sprite image (128 bytes). By default, sprite *n*
uses shape slot *n*. The `SPRITESHAPE` command lets you reassign
which slot a sprite renders:

`SPRITESHAPE n, shape`

- `n` is the sprite index (0--15).
- `shape` is the shape slot (0--255) to assign to that sprite.

Multiple sprites can share the same shape slot, so you can display several
identical objects without duplicating pixel data. Shape slots beyond 15 must
be populated via `DMACOPY` into sprite shape memory (space 4) or
register-level commands.

`SPRITESHAPE` writes to the memory-mapped sprite register at
`$A044 + n*8` (the shape field in the sprite register block).

## Sprite Registers

Each of the 16 sprites has an 8-byte register block starting at $A040.
The register layout per sprite is:

| **Offset** | **Field** | **Description** |
| --- | --- | --- |
| +0 | XLo | X position, low byte |
| +1 | XHi | X position, high byte |
| +2 | YLo | Y position, low byte |
| +3 | YHi | Y position, high byte |
| +4 | Shape | Shape slot index (0--255) |
| +5 | Flags | Bit 0: horizontal flip, Bit 1: vertical flip, Bit 2: enable |
| +6 | Priority | 0 = behind all, 1 = between layers, 2 = in front |
| +7 | TransColor | Per-sprite transparent color index (0--15). Default 0 (black is transparent). Values >15 disable transparency (all 16 colors opaque). |

Sprite 0 occupies A040--A047, sprite 1 occupies A048--A04F, and so on
up to sprite 15 at A0B8--A0BF. These registers can be written directly
with `POKE` or targeted by the copper for per-scanline updates.

## SPRITESET -- Per-Register Sprite Control

`SPRITESET` provides direct write access to any field in a sprite's
8-byte register block. The write is synchronised to the vertical blank
interval, so no mid-frame visual glitching occurs.

`SPRITESET sprite, field, value`

- `sprite` is the sprite index (0--15).
- `field` is the register offset within the sprite block (0--7).
- `value` is the byte to write (0--255).

The eight field offsets map directly to the sprite register layout described
in Section \refsec:sprite-registers:

| **Field** | **Register** |
| --- | --- |
| 0 | X position, low byte |
| 1 | X position, high byte |
| 2 | Y position, low byte |
| 3 | Y position, high byte |
| 4 | Shape slot index (0--255) |
| 5 | Flags (bit 0: H-flip, bit 1: V-flip, bit 2: enable) |
| 6 | Priority (0=behind all, 1=between layers, 2=in front) |
| 7 | Transparent color index (0--15; values >15 disable transparency) |

For example, to set sprite 3's transparent color to color index 5:

```basic
10 SPRITESET 3, 7, 5
```

::: note
`SPRITESET` waits for the next vblank before writing to the register.
This ensures the change takes effect at a frame boundary and avoids
partial-frame artifacts when changing position or shape fields.
:::

## Reading Sprite Position

`SPRITEX(n)` and `SPRITEY(n)` return the current screen position of
sprite `n` by reading directly from the sprite register block.

| **Function** | **Returns** |
| --- | --- |
| SPRITEX(n) | Current X position of sprite `n` as a signed 16-bit value. |
| SPRITEY(n) | Current Y position of sprite `n` as a signed 16-bit value. |

Both functions combine the low and high byte registers to produce a signed
16-bit result, which correctly represents positions set by `SPRITE n,x,y`,
`SPRITESET`, or the copper.

```basic
10 SPRITE 0, 160, 80
20 PRINT SPRITEX(0), SPRITEY(0)
```

This prints `160` and `80`, confirming the position set on
line 10.

## Try It Now

::: tryit
Type and run the following program to see `MODE`, `GCLS`,
`GCOLOR`, `RECT`, `CIRCLE`, and `PAINT` working together:

```basic
10 MODE 1 : GCLS : GCOLOR 10
20 RECT 10, 10, 309, 189
30 GCOLOR 14 : CIRCLE 160, 100, 50
40 GCOLOR 12 : PAINT 160, 100
```

Expected result: a green rectangle border frames the screen; inside it a
yellow circle outline encloses a solid red filled region.

Try modifying `GCOLOR` values (1--15) and the `CIRCLE` radius to
explore the coordinate system. Then add a second `CIRCLE` call on a new
line and re-run to see both circles on the same canvas.
:::

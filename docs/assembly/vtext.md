# NovaVM VTEXT Runtime

`software/runtime/asm/vtext.inc` and `software/runtime/asm/vtext.s` provide a shared ca65 text
region layer over the VGC character, color, and text-attribute planes.

VTEXT is deliberately not a window manager. It does not know about BASIC,
NovaZ, or Z-machine windows. Callers define rectangular regions and decide what
those regions mean.

## ABI

Include the interface:

```asm
.include "vtext.inc"
```

Set the current region and attributes through the Nova pseudo-registers:

```asm
VTEXT_LEFT      ; region left column
VTEXT_TOP       ; region top row
VTEXT_WIDTH     ; region width in columns
VTEXT_HEIGHT    ; region height in rows
VTEXT_CURX      ; cursor X relative to region
VTEXT_CURY      ; cursor Y relative to region
VTEXT_COLOR     ; packed color: high nibble background, low nibble foreground
VTEXT_ATTR      ; text attr bits, e.g. VTEXT_ATTR_FLASH
VTEXT_CHAR      ; character for vtext_put_char
VTEXT_FLAGS     ; VTEXT_FLAG_WRAP, VTEXT_FLAG_SCROLL
```

All public routines return `A=0` on success and `A=1` on error.

## Routines

```asm
jsr vtext_validate_region
jsr vtext_set_cursor
jsr vtext_home
jsr vtext_put_char
jsr vtext_put_run    ; A/Y = bytes pointer, X = length
jsr vtext_puts       ; A/Y = zero-terminated string pointer
jsr vtext_put_hex_nibble
jsr vtext_put_hex_byte
jsr vtext_newline
jsr vtext_clear_region
jsr vtext_fill_style_region
jsr vtext_clear_line
jsr vtext_scroll_up
jsr vtext_scroll_mixed_up ; A = rows, X = gfx fill colour
jsr vtext_fill_gfx_region ; X = gfx fill colour under current text region
jsr vtext_scroll_gfx_pixels_up ; A = pixel rows, X = gfx fill colour
```

Optional region tables are caller-owned. Set `VTEXT_TABLEL/H` to the table
base and `VTEXT_REGION_ID` to the slot, then call:

```asm
jsr vtext_define_region
jsr vtext_select_region
jsr vtext_store_region
```

Each region slot is `VTEXT_REGION_SIZE` bytes. This lets NovaZ define a fixed
status region and a scrolling story region without making Nova understand
Z-machine windows.

## Example Boundary

For a Z3-style Infocom screen, NovaZ can define:

```asm
; Region 0: row 0 status line, fixed
; Region 1: rows 1..49 story output, wrap + scroll
```

VTEXT handles row-local output, clear, and scroll. NovaZ remains responsible
for interpreting story-file window rules and formatting the status line.

## Row Runs

Use `vtext_put_run` when you already have a short run of printable bytes that
all share the same `VTEXT_COLOR` and `VTEXT_ATTR`.

```asm
        lda #<text
        ldy #>text
        ldx #text_end-text
        jsr vtext_put_run
```

`vtext_put_run` is deliberately row-local. It does not parse control
characters, wrap to the next line, or scroll. The run must fit from the current
cursor to the right edge of the current region. If it exactly fills the row,
the cursor remains on the final cell because Nova has no valid one-past-right
cursor position; callers that want a newline should call their own newline
logic after the run.

## Mode 2 Backgrounds

In `VGC_MODE_TEXT_OVER_GFX`, `VTEXT_COLOR` still uses the normal packed VGC
color byte: high nibble background, low nibble foreground.

The high nibble also controls whether blank glyph pixels cover the graphics
plane:

- If the high nibble equals `VGC_BGCOL`, blank glyph pixels are transparent
  and the graphics pixel underneath shows through.
- If the high nibble differs from `VGC_BGCOL`, blank glyph pixels are opaque
  and draw the cell background.

Use the transparent key for cells reserved for pictures. Use an opaque
background for ordinary solid text boxes; that lets the VGC paint the text
background without a software graphics fill behind every character.

If a cell rectangle changes from picture/transparent ownership back to normal
background ownership, clear the graphics pixels under it before restyling the
text cells:

```asm
        ldx #9      ; gfx colour index for the normal background
        jsr vtext_fill_gfx_region
```

`vtext_fill_gfx_region` uses the current `VTEXT_LEFT/TOP/WIDTH/HEIGHT` and
fills the matching 4-pixel-per-cell graphics rectangle. This prevents stale
picture pixels from showing through text cells whose glyph pixels are blank or
transparent.

## Mixed Text And Graphics Scroll

`vtext_scroll_up` scrolls only the VGC text planes: characters, colors, and text
attributes. Applications that draw text over the graphics plane sometimes need
framebuffer-style scrolling instead. A Z-machine V6 `scroll_window` is the
classic case: SDL Frotz scrolls one pixel rectangle, so already-rendered text
and pictures move together.

Use `vtext_scroll_mixed_up` for that case:

```asm
        lda #6      ; text rows to scroll upward
        ldx #9      ; gfx colour index for the exposed bottom strip
        jsr vtext_scroll_mixed_up
```

The routine uses the current `VTEXT_LEFT/TOP/WIDTH/HEIGHT` rectangle. It waits
for a frame boundary, scrolls the matching 4-pixel-per-cell graphics rectangle,
then scrolls the text planes by the same row count. `VTEXT_COLOR` and
`VTEXT_ATTR` fill the exposed text rows; `X` fills the exposed graphics strip.

If your graphics rectangle is not exactly the same as the text-cell rectangle,
scroll the two surfaces separately. This matters for runtimes with pixel-based
windows, such as Z-machine V6: the graphics window may start at any pixel, but
the text plane can only safely own whole 4x4 cells.

Set the pixel rectangle, then call `vtext_scroll_gfx_pixels_up`:

```asm
        lda #<38
        sta VTEXT_GFX_LEFTL
        stz VTEXT_GFX_LEFTH
        lda #56
        sta VTEXT_GFX_TOP
        lda #<244
        sta VTEXT_GFX_WIDTHL
        lda #>244
        sta VTEXT_GFX_WIDTHH
        lda #140
        sta VTEXT_GFX_HEIGHT

        lda #4      ; pixel rows to scroll upward
        ldx #9      ; gfx colour index for the exposed bottom strip
        jsr vtext_scroll_gfx_pixels_up
```

`VTEXT_GFX_LEFTL/H`, `VTEXT_GFX_TOP`, `VTEXT_GFX_WIDTHL/H`, and
`VTEXT_GFX_HEIGHT` are graphics pixels, not text cells. The rectangle must fit
inside the 320x200 graphics plane. After this call, scroll or clear the text
planes using the normal VTEXT region routines for whatever text-cell rectangle
your application owns.

Keep `move_window`-style metadata changes separate from scrolling. Moving or
resizing a logical window should not move old contents by itself. If contents
must move, issue an explicit scroll or blit.

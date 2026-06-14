# NovaVM VTEXT Runtime

`runtime/asm/vtext.inc` and `runtime/asm/vtext.s` provide a shared ca65 text
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

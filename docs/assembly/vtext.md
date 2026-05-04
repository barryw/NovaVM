# NovaVM VTEXT Runtime

`ehbasic/lib/vtext.inc` and `ehbasic/lib/vtext.s` provide a shared ca65 text
region layer over the VGC character, color, and text-attribute planes.

VTEXT is deliberately not a window manager. It does not know about BASIC,
NovaZ, or Z-machine windows. Callers define rectangular regions and decide what
those regions mean.

## ABI

Include the interface:

```asm
.include "lib/vtext.inc"
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
jsr vtext_newline
jsr vtext_clear_region
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

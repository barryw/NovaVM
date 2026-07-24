; =====================================================================
;  Nova NDK — vtext.inc
;
;  NovaVM shared text-region runtime interface. Intentionally
;  language-neutral: it knows about rectangular regions over the VGC text
;  planes, but not about BASIC, NovaZ, or Z-machine window semantics.
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

; Calling convention:
;   Region:      VTEXT_LEFT/TOP/WIDTH/HEIGHT state bytes
;   Cursor:      VTEXT_CURX/CURY state bytes, relative to the current region.
;                vtext_set_cursor and character output mirror this to the
;                absolute VGC visible cursor registers.
;   Cell attrs:  VTEXT_COLOR (packed bg/fg), VTEXT_ATTR (style bits)
;   Character:   VTEXT_CHAR input byte
;   Flags:       VTEXT_FLAGS state byte (wrap/scroll)
;   Scroll hook: optional subroutine pointer installed with
;                vtext_set_scroll_hook. When nonzero, VTEXT calls it instead
;                of vtext_scroll_up for automatic region scrolls. The hook is
;                responsible for scrolling the current text planes too; this
;                lets mixed text/graphics apps batch all visible planes.
;
; Routines return A=0 on success and A=1 on error. X/Y are scratch unless a
; routine explicitly documents otherwise.
; I_VTEXT_PUTS and I_VTEXT_PRINT_AT consume zero-terminated text stored
; immediately after their JSR and advance the saved return address past it.

.ifndef VTEXT_INC_INCLUDED
VTEXT_INC_INCLUDED = 1

.include "nova.inc"

VTEXT_SCREEN_COLS = NOVA_SCREEN_COLS
VTEXT_SCREEN_ROWS = NOVA_SCREEN_ROWS
VTEXT_GFX_CELL_PIXELS = 4
VTEXT_GFX_ROW_PIXELS = VTEXT_SCREEN_COLS * VTEXT_GFX_CELL_PIXELS

VTEXT_PLANE_CHAR = VGC_PLANE_CHAR
VTEXT_PLANE_COLOR = VGC_PLANE_COLOR
VTEXT_PLANE_TEXTATTR = VGC_PLANE_TEXTATTR

VTEXT_VRAM_AUTOINC = VGC_VRAM_AUTOINC

VTEXT_FLAG_WRAP = $01
VTEXT_FLAG_SCROLL = $02

VTEXT_ATTR_NONE = $00
VTEXT_ATTR_FLASH = VTXT_ATTR_FLASH
VTEXT_ATTR_REVERSE = VTXT_ATTR_REV
VTEXT_ATTR_BOLD = VTXT_ATTR_BOLD
VTEXT_ATTR_BGTRANS = VTXT_ATTR_BGTRANS

VTEXT_OK = $00
VTEXT_ERR = $01

; Assembly-facing state ABI. These are linker-owned storage symbols defined by
; vtext.s, not shared NVR pseudo-registers. NVR remains caller scratch/mailbox.
.global VTEXT_LEFT
.global VTEXT_TOP
.global VTEXT_WIDTH
.global VTEXT_HEIGHT
.global VTEXT_CURX
.global VTEXT_CURY
.global VTEXT_COLOR
.global VTEXT_ATTR
.global VTEXT_CHAR
.global VTEXT_FLAGS
.global VTEXT_TOPROW
.global VTEXT_SCROLL_TOP
.global VTEXT_SCROLL_ROWS
.global VTEXT_SCROLL_HOOKL
.global VTEXT_SCROLL_HOOKH

; Optional region-table ABI. Callers own the table storage and set these state
; bytes before define/select/store.
.global VTEXT_TABLEL
.global VTEXT_TABLEH
.global VTEXT_REGION_ID

VTEXT_REGION_LEFT = 0
VTEXT_REGION_TOP = 1
VTEXT_REGION_WIDTH = 2
VTEXT_REGION_HEIGHT = 3
VTEXT_REGION_CURX = 4
VTEXT_REGION_CURY = 5
VTEXT_REGION_COLOR = 6
VTEXT_REGION_ATTR = 7
VTEXT_REGION_FLAGS = 8
VTEXT_REGION_RESERVED = 9
VTEXT_REGION_SIZE = 10

; Public helper routines.
.global vtext_define_region
.global vtext_select_region
.global vtext_store_region
.global vtext_validate_region
.global vtext_set_cursor
.global vtext_sync_cursor
.global vtext_home
.global vtext_put_char
.global vtext_put_run
.global vtext_puts
.global vtext_print_at
.global i_vtext_puts
.global i_vtext_print_at
.global vtext_put_hex_nibble
.global vtext_put_hex_byte
.global vtext_read_char
.global vtext_newline
.global vtext_set_scroll_hook
.global vtext_clear_scroll_hook
.global vtext_clear_region
.global vtext_fill_style_region
.global vtext_expose_gfx_spaces_region
.global vtext_clear_line
.global vtext_scroll_up
.global vtext_scroll_down
.global vtext_scroll_mixed_up
.global vtext_scroll_composite_up
.global vtext_fill_gfx_region
.global vtext_scroll_gfx_pixels_up
.global vtext_wait_blitter

; Pixel-rectangle state for vtext_scroll_gfx_pixels_up. Coordinates are gfx
; pixels, not text cells. Height and scroll amount are pixel rows.
.global VTEXT_GFX_LEFTL
.global VTEXT_GFX_LEFTH
.global VTEXT_GFX_TOP
.global VTEXT_GFX_WIDTHL
.global VTEXT_GFX_WIDTHH
.global VTEXT_GFX_HEIGHT

; Internal region-addressing helper and its zero-page output, exposed so
; separately-linked consumers (nui save/restore-under) can compute and read a
; region's VRAM address. Not part of the stable text-output ABI.
.global vtext_calc_region_addr
.globalzp VTEXT_ADDRL
.globalzp VTEXT_ADDRH

.endif

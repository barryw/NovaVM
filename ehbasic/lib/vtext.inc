; NovaVM shared text-region runtime interface.
;
; This layer is intentionally language-neutral. It knows about rectangular
; regions over the VGC text planes, but not about BASIC, Ozmoo, or Z-machine
; window semantics.
;
; Calling convention:
;   Region:      VTEXT_LEFT/TOP/WIDTH/HEIGHT
;   Cursor:      VTEXT_CURX/CURY, relative to the current region
;   Cell attrs:  VTEXT_COLOR (packed bg/fg), VTEXT_ATTR (style bits)
;   Character:   VTEXT_CHAR
;   Flags:       VTEXT_FLAGS (wrap/scroll)
;
; Routines return A=0 on success and A=1 on error. X/Y are scratch unless a
; routine explicitly documents otherwise.

.ifndef VTEXT_INC_INCLUDED
VTEXT_INC_INCLUDED = 1

.include "nova.inc"

VTEXT_SCREEN_COLS = NOVA_SCREEN_COLS
VTEXT_SCREEN_ROWS = NOVA_SCREEN_ROWS

VTEXT_PLANE_CHAR = VGC_PLANE_CHAR
VTEXT_PLANE_COLOR = VGC_PLANE_COLOR
VTEXT_PLANE_TEXTATTR = VGC_PLANE_TEXTATTR

VTEXT_VRAM_AUTOINC = VGC_VRAM_AUTOINC

VTEXT_FLAG_WRAP = $01
VTEXT_FLAG_SCROLL = $02

VTEXT_ATTR_NONE = $00
VTEXT_ATTR_FLASH = VTXT_ATTR_FLASH

VTEXT_OK = $00
VTEXT_ERR = $01

; Assembly-facing pseudo-register ABI.
VTEXT_LEFT      = NVR0L
VTEXT_TOP       = NVR0H
VTEXT_WIDTH     = NVR1L
VTEXT_HEIGHT    = NVR1H
VTEXT_CURX      = NVR2L
VTEXT_CURY      = NVR2H
VTEXT_COLOR     = NVR3L
VTEXT_ATTR      = NVR3H
VTEXT_CHAR      = NVR4L
VTEXT_FLAGS     = NVR4H

; Optional region-table ABI. Callers own the table storage and set
; VTEXT_TABLEL/H plus VTEXT_REGION_ID before define/select/store.
VTEXT_TABLEL    = NVR5L
VTEXT_TABLEH    = NVR5H
VTEXT_REGION_ID = NVR6L

; Internal scratch used by vtext.s.
VTEXT_ADDRL     = NVR6H
VTEXT_ADDRH     = NVR7L
VTEXT_TMP       = NVR7H

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

.endif

; NovaVM shared text-region runtime interface.
;
; This layer is intentionally language-neutral. It knows about rectangular
; regions over the VGC text planes, but not about BASIC, Ozmoo, or Z-machine
; window semantics.
;
; Calling convention:
;   Region:      VTEXT_LEFT/TOP/WIDTH/HEIGHT state bytes
;   Cursor:      VTEXT_CURX/CURY state bytes, relative to the current region
;   Cell attrs:  VTEXT_COLOR (packed bg/fg), VTEXT_ATTR (style bits)
;   Character:   VTEXT_CHAR input byte
;   Flags:       VTEXT_FLAGS state byte (wrap/scroll)
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

.endif

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

.ifndef NVR0L
NVR0L           = $20
NVR0H           = $21
NVR1L           = $22
NVR1H           = $23
NVR2L           = $24
NVR2H           = $25
NVR3L           = $26
NVR3H           = $27
NVR4L           = $28
NVR4H           = $29
NVR5L           = $2A
NVR5H           = $2B
NVR6L           = $2C
NVR6H           = $2D
NVR7L           = $2E
NVR7H           = $2F
.endif

.ifndef VGC_VRAM_PLANE
VGC_VRAM_PLANE  = $A0E0
VGC_VRAM_ADDRL  = $A0E1
VGC_VRAM_ADDRH  = $A0E2
VGC_VRAM_DATA   = $A0E3
VGC_VRAM_CTRL   = $A0E4
.endif

.ifndef BLT_CMD_REG
BLT_CMD_REG     = $BA83
BLT_STATUS_REG  = $BA84
BLT_SRCSPACE    = $BA86
BLT_DSTSPACE    = $BA87
BLT_SRCL        = $BA88
BLT_SRCM        = $BA89
BLT_SRCH        = $BA8A
BLT_DSTL        = $BA8B
BLT_DSTM        = $BA8C
BLT_DSTH        = $BA8D
BLT_WIDTHL      = $BA8E
BLT_WIDTHH      = $BA8F
BLT_HEIGHTL     = $BA90
BLT_HEIGHTH     = $BA91
BLT_SRCSTRL     = $BA92
BLT_SRCSTRH     = $BA93
BLT_DSTSTRL     = $BA94
BLT_DSTSTRH     = $BA95
BLT_MODE_REG    = $BA96
BLT_FILLVALUE   = $BA97
BLT_CKEY        = $BA98
.endif

VTEXT_SCREEN_COLS = 80
VTEXT_SCREEN_ROWS = 50

VTEXT_PLANE_CHAR = $01
VTEXT_PLANE_COLOR = $02
VTEXT_PLANE_TEXTATTR = $07

VTEXT_VRAM_AUTOINC = $01

VTEXT_FLAG_WRAP = $01
VTEXT_FLAG_SCROLL = $02

VTEXT_ATTR_NONE = $00
VTEXT_ATTR_FLASH = $01

VTEXT_OK = $00
VTEXT_ERR = $01

BLT_CMD_START = $01
BLT_STATUS_BUSY = $01
BLT_STATUS_OK = $02
BLT_MODE_FILL = $01

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

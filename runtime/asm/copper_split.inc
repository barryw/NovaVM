; Shared Nova copper split helper declarations.

.ifndef COPPER_SPLIT_INC_INCLUDED
COPPER_SPLIT_INC_INCLUDED = 1

.include "copper.inc"
.include "vgc.inc"

COPPER_SPLIT_RESULT_OK    = COPPER_RESULT_OK
COPPER_SPLIT_RESULT_ERROR = COPPER_RESULT_ERROR

; @label COPPER.SPLIT_MODE
; @kind routine
; @symbol copper_split_mode
; @summary Configure one copper list to switch VGC mode at a scanline.
; @requires VGC_P0 VGC_P1 VGC_P2 VGC_P3
; @in VGC_P0: Copper list index.
; @in VGC_P1: Split scanline Y.
; @in VGC_P2: Mode from scanline 0 up to split Y-1.
; @in VGC_P3: Mode from split Y onward.
.global copper_split_mode

.endif

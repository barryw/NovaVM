; Shared Nova sprite helper declarations.
;
; Command helpers use VGC_P* registers directly. Direct sprite-register helpers
; use the public sprite attribute register block at VGC_SPR_BASE.

.ifndef SPRITE_INC_INCLUDED
SPRITE_INC_INCLUDED = 1

.include "vgc.inc"

SPRITE_RESULT_OK    = $00
SPRITE_RESULT_ERROR = $01

.endif

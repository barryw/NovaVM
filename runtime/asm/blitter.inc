; Shared Nova blitter helper declarations.

.ifndef BLITTER_INC_INCLUDED
BLITTER_INC_INCLUDED = 1

.include "nova.inc"

BLITTER_RESULT_OK    = $00
BLITTER_RESULT_ERROR = $01

.global blitter_copy
.global blitter_fill
.global blitter_start_copy
.global blitter_start_fill
.global blitter_wait

.endif

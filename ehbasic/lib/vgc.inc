; Shared Nova VGC helper declarations.
;
; The command helpers use the VGC hardware parameter registers directly:
; callers load VGC_P0..VGC_P14 as needed, then call a vgc_* routine.
; Routines return through the hardware status model; command completion can be
; checked with vgc_wait_cmd when the caller needs a synchronous operation.

.ifndef VGC_INC_INCLUDED
VGC_INC_INCLUDED = 1

.include "nova.inc"

VGC_RESULT_OK    = $00
VGC_RESULT_ERROR = $01

.endif

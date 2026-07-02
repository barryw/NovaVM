; Minimal VGC frame synchronization helper.
;
; Link this file when a small app only needs a frame tick wait. Full-featured
; callers can link vgc.s instead.

.include "vgc.inc"

.ifndef VGC_VSYNC_IMPLEMENTATION_INCLUDED
VGC_VSYNC_IMPLEMENTATION_INCLUDED = 1

      .segment "CODE"

      .export vgc_vsync

; @label VGC.VSYNC
; @kind routine
; @symbol vgc_vsync
; @summary Wait until the VGC frame counter advances.
; @out A: Last observed frame counter value.
vgc_vsync:
      LDA   VGC_FRAME
@wait:
      CMP   VGC_FRAME
      BEQ   @wait
      RTS

.endif

; Minimal VGC command synchronization helper.
;
; Full-featured callers can link vgc.s. Small standalone apps and overlays can
; link this file when they only need the graphics command wait primitive without
; pulling in the complete VGC helper implementation.

.include "vgc.inc"

.ifndef VGC_WAIT_IMPLEMENTATION_INCLUDED
VGC_WAIT_IMPLEMENTATION_INCLUDED = 1

      .segment "CODE"

      .export vgc_wait_cmd

; @label VGC.WAIT_COMMAND
; @kind routine
; @symbol vgc_wait_cmd
; @summary Wait for the active VGC hardware command to complete.
vgc_wait_cmd:
@wait:
      LDA   VGC_CMD
      AND   #$01
      BNE   @wait
      RTS

.endif

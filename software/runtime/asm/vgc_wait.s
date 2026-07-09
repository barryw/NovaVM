; =====================================================================
;  Nova NDK — vgc_wait.s
;
;  Minimal VGC command-completion wait primitive. Standalone apps and
;  overlays link this instead of the full vgc.s helper when they only
;  need to wait for a graphics command to finish.
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

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
      LDY   #$20
@arm:
      LDA   VGC_CMD
      AND   #$01
      BNE   @wait
      DEY
      BNE   @arm
      RTS
@wait:
      LDA   VGC_CMD
      AND   #$01
      BNE   @wait
      RTS

.endif

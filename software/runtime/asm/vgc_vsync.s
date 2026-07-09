; =====================================================================
;  Nova NDK — vgc_vsync.s
;
;  Minimal VGC frame-sync helper (vgc_vsync only): waits for the frame
;  counter to advance. Link this when a small app just needs a frame-tick
;  wait; full-featured callers link vgc.s instead.
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

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

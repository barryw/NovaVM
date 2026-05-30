; Shared Nova copper split helper routines.

.include "copper_split.inc"
.include "vgc.s"
.include "copper.s"

.ifndef COPPER_SPLIT_IMPLEMENTATION_INCLUDED
COPPER_SPLIT_IMPLEMENTATION_INCLUDED = 1

      .segment "CODE"

      .export copper_split_mode

; Configure one copper list with two VGC_MODE rules:
;   line 0       -> VGC_P2 mode
;   VGC_P1 line -> VGC_P3 mode
; Clobbers A and NVR4L..NVR5H.
copper_split_mode:
      LDA   VGC_P0
      STA   NVR4L
      LDA   VGC_P1
      STA   NVR4H
      LDA   VGC_P2
      STA   NVR5L
      LDA   VGC_P3
      STA   NVR5H

      JSR   vgc_wait_cmd
      JSR   copper_off

      JSR   vgc_wait_cmd
      LDA   NVR4L
      STA   VGC_P0
      JSR   copper_list

      JSR   vgc_wait_cmd
      JSR   copper_clear

      STZ   VGC_P0
      STZ   VGC_P1
      STZ   VGC_P2
      LDA   #COPPER_REG_MODE
      JSR   copper_set_reg_index
      LDA   NVR5L
      STA   VGC_P5
      JSR   vgc_wait_cmd
      JSR   copper_add

      STZ   VGC_P0
      STZ   VGC_P1
      LDA   NVR4H
      STA   VGC_P2
      LDA   #COPPER_REG_MODE
      JSR   copper_set_reg_index
      LDA   NVR5H
      STA   VGC_P5
      JSR   vgc_wait_cmd
      JSR   copper_add

      JSR   vgc_wait_cmd
      LDA   NVR4L
      STA   VGC_P0
      JSR   copper_use

      JSR   vgc_wait_cmd
      JSR   copper_on
      LDA   #COPPER_SPLIT_RESULT_OK
      RTS

.endif

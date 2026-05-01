; Shared Nova copper helper routines.
;
; Assembly ABI:
;   copper_add expects VGC_P0/P1=x, VGC_P2=y, VGC_P3/P4=register/index,
;   VGC_P5=value. COPPER_REG_* constants cover common VGC register indices.

.include "copper.inc"

.ifndef COPPER_IMPLEMENTATION_INCLUDED
COPPER_IMPLEMENTATION_INCLUDED = 1

      .segment "CODE"

      .export copper_command
      .export copper_clear
      .export copper_on
      .export copper_off
      .export copper_list
      .export copper_list_end
      .export copper_use
      .export copper_add
      .export copper_set_reg_index
      .export copper_set_sprite_reg

; Input: A = copper VGC command. Does not wait for completion.
copper_command:
      STA   VGC_CMD
      LDA   #COPPER_RESULT_OK
      RTS

copper_clear:
      LDA   #VCMD_COPPERCLR
      JMP   copper_command

copper_on:
      LDA   #VCMD_COPPERENA
      JMP   copper_command

copper_off:
      LDA   #VCMD_COPPERDIS
      JMP   copper_command

copper_list:
      LDA   #VCMD_COPPERLIST
      JMP   copper_command

copper_list_end:
      LDA   #VCMD_COPPEREND
      JMP   copper_command

copper_use:
      LDA   #VCMD_COPPERUSE
      JMP   copper_command

copper_add:
      LDA   #VCMD_COPPERADD
      JMP   copper_command

; Input: A = direct VGC register index. Output: VGC_P3/P4 updated.
copper_set_reg_index:
      STA   VGC_P3
      STZ   VGC_P4
      RTS

; Input: X = sprite index, A = VGC_SPR_*_OFF field.
; Output: VGC_P3/P4 = absolute sprite register address.
; Clobbers: A, NVR7L.
copper_set_sprite_reg:
      STA   NVR7L
      TXA
      ASL
      ASL
      ASL
      CLC
      ADC   NVR7L
      CLC
      ADC   #<VGC_SPR_BASE
      STA   VGC_P3
      LDA   #>VGC_SPR_BASE
      ADC   #$00
      STA   VGC_P4
      RTS

.endif

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
; @label COPPER.COMMAND
; @kind routine
; @symbol copper_command
; @summary Issue a raw copper VGC command.
; @in A: Copper VGC command byte.
copper_command:
      STA   VGC_CMD
      LDA   #COPPER_RESULT_OK
      RTS

; @label COPPER.CLEAR
; @kind routine
; @symbol copper_clear
; @summary Clear the active copper list.
copper_clear:
      LDA   #VCMD_COPPERCLR
      JMP   copper_command

; @label COPPER.ON
; @kind routine
; @symbol copper_on
; @summary Enable copper execution.
copper_on:
      LDA   #VCMD_COPPERENA
      JMP   copper_command

; @label COPPER.OFF
; @kind routine
; @symbol copper_off
; @summary Disable copper execution.
copper_off:
      LDA   #VCMD_COPPERDIS
      JMP   copper_command

; @label COPPER.LIST
; @kind routine
; @symbol copper_list
; @summary Select a copper list using VGC.P0.
; @requires VGC_P0
copper_list:
      LDA   #VCMD_COPPERLIST
      JMP   copper_command

; @label COPPER.LIST_END
; @kind routine
; @symbol copper_list_end
; @summary Finish the current copper list.
copper_list_end:
      LDA   #VCMD_COPPEREND
      JMP   copper_command

; @label COPPER.USE
; @kind routine
; @symbol copper_use
; @summary Activate a copper list using VGC.P0.
; @requires VGC_P0
copper_use:
      LDA   #VCMD_COPPERUSE
      JMP   copper_command

; @label COPPER.ADD
; @kind routine
; @symbol copper_add
; @summary Add a copper operation using VGC.P0..VGC.P5.
; @requires VGC_P0 VGC_P1 VGC_P2 VGC_P3 VGC_P4 VGC_P5
copper_add:
      LDA   #VCMD_COPPERADD
      JMP   copper_command

; Input: A = direct VGC register index. Output: VGC_P3/P4 updated.
; @label COPPER.SET_REG_INDEX
; @kind routine
; @symbol copper_set_reg_index
; @summary Set VGC.P3/P4 to a direct VGC register index.
; @in A: VGC register index.
copper_set_reg_index:
      STA   VGC_P3
      STZ   VGC_P4
      RTS

; Input: X = sprite index, A = VGC_SPR_*_OFF field.
; Output: VGC_P3/P4 = absolute sprite register address.
; Clobbers: A, NVR7L.
; @label COPPER.SET_SPRITE_REG
; @kind routine
; @symbol copper_set_sprite_reg
; @summary Set VGC.P3/P4 to a sprite attribute register address.
; @in X: Sprite index.
; @in A: Sprite field offset.
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

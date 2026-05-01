; Shared Nova sprite helper routines.
;
; Assembly callers can load VGC_P* and call the command helpers, or use the
; direct register helpers for the VGC sprite attribute block.

.include "sprite.inc"

.ifndef SPRITE_IMPLEMENTATION_INCLUDED
SPRITE_IMPLEMENTATION_INCLUDED = 1

      .segment "CODE"

      .export sprite_command
      .export sprite_pos
      .export sprite_enable
      .export sprite_disable
      .export sprite_row
.ifndef SPRITE_NO_EXTRA
      .export sprite_clear
      .export sprite_copy
      .export sprite_flip
      .export sprite_priority
.endif
      .export sprite_reg_offset
      .export sprite_set_shape
      .export sprite_set_reg8
      .export sprite_set_reg16
      .export sprite_get_x
      .export sprite_get_y

; Input: A = sprite VGC command. Does not wait for completion.
sprite_command:
      STA   VGC_CMD
      RTS

; Command wrappers. Callers load VGC_P* per the hardware command ABI.
sprite_pos:
      LDA   #VCMD_SPRPOS
      JMP   sprite_command

sprite_enable:
      LDA   #VCMD_SPRENA
      JMP   sprite_command

sprite_disable:
      LDA   #VCMD_SPRDIS
      JMP   sprite_command

sprite_row:
      LDA   #VCMD_SPRROW
      JMP   sprite_command

.ifndef SPRITE_NO_EXTRA
sprite_clear:
      LDA   #VCMD_SPRCLR
      JMP   sprite_command

sprite_copy:
      LDA   #VCMD_SPRCOPY
      JMP   sprite_command

sprite_flip:
      LDA   #VCMD_SPRFLIP
      JMP   sprite_command

sprite_priority:
      LDA   #VCMD_SPRPRI
      JMP   sprite_command
.endif

; Input: X = sprite index, Y = sprite register field offset.
; Output: Y = VGC_SPR_BASE-relative register offset.
; Clobbers: A, NVR7L.
sprite_reg_offset:
      STY   NVR7L
      TXA
      ASL
      ASL
      ASL
      CLC
      ADC   NVR7L
      TAY
      RTS

; Input: X = sprite index, A = shape.
sprite_set_shape:
      LDY   #VGC_SPR_SHAPE_OFF
      ; fall through

; Input: X = sprite index, Y = field offset, A = value.
; Clobbers: A, Y, NVR7L/NVR7H.
sprite_set_reg8:
      STA   NVR7H
      JSR   sprite_reg_offset
      LDA   NVR7H
      STA   VGC_SPR_BASE,Y
      RTS

; Input: X = sprite index, Y = field offset, NVR0L/NVR0H = little-endian value.
; Clobbers: A, Y, NVR7L.
sprite_set_reg16:
      JSR   sprite_reg_offset
      LDA   NVR0L
      STA   VGC_SPR_BASE,Y
      INY
      LDA   NVR0H
      STA   VGC_SPR_BASE,Y
      RTS

; Input: X = sprite index. Output: A = X high byte, Y = X low byte.
sprite_get_x:
      LDY   #VGC_SPR_XL_OFF
      JSR   sprite_reg_offset
      LDA   VGC_SPR_BASE,Y
      STA   NVR7L
      INY
      LDA   VGC_SPR_BASE,Y
      LDY   NVR7L
      RTS

; Input: X = sprite index. Output: Y = Y low byte.
sprite_get_y:
      LDY   #VGC_SPR_YL_OFF
      JSR   sprite_reg_offset
      LDA   VGC_SPR_BASE,Y
      TAY
      RTS

.endif

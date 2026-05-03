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
      .export sprite_collision_status
      .export sprite_background_collision_status

; Input: A = sprite VGC command. Does not wait for completion.
; @label SPRITE.COMMAND
; @kind routine
; @symbol sprite_command
; @summary Issue a raw sprite VGC command without waiting for completion.
; @in A: VGC sprite command byte.
sprite_command:
      STA   VGC_CMD
      RTS

; Command wrappers. Callers load VGC_P* per the hardware command ABI.
; @label SPRITE.POS
; @kind routine
; @symbol sprite_pos
; @summary Move a sprite using VGC.P0=sprite, VGC.P1/P2=x, VGC.P3=y.
; @requires VGC_P0 VGC_P1 VGC_P2 VGC_P3
sprite_pos:
      LDA   #VCMD_SPRPOS
      JMP   sprite_command

; @label SPRITE.ENABLE
; @kind routine
; @symbol sprite_enable
; @summary Enable the sprite in VGC.P0.
; @requires VGC_P0
sprite_enable:
      LDA   #VCMD_SPRENA
      JMP   sprite_command

; @label SPRITE.DISABLE
; @kind routine
; @symbol sprite_disable
; @summary Disable the sprite in VGC.P0.
; @requires VGC_P0
sprite_disable:
      LDA   #VCMD_SPRDIS
      JMP   sprite_command

; @label SPRITE.ROW
; @kind routine
; @symbol sprite_row
; @summary Write one sprite bitmap row using VGC.P0..VGC.P9.
; @requires VGC_P0 VGC_P1 VGC_P2 VGC_P3 VGC_P4 VGC_P5 VGC_P6 VGC_P7 VGC_P8 VGC_P9
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
; @label SPRITE.REG_OFFSET
; @kind routine
; @symbol sprite_reg_offset
; @summary Compute the VGC sprite attribute block offset for a sprite field.
; @in X: Sprite index.
; @in Y: Sprite field offset.
; @out Y: VGC_SPR_BASE-relative byte offset.
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
; @label SPRITE.SET_SHAPE
; @kind routine
; @symbol sprite_set_shape
; @summary Set a sprite's shape index.
; @in X: Sprite index.
; @in A: Shape index.
sprite_set_shape:
      LDY   #VGC_SPR_SHAPE_OFF
      ; fall through

; Input: X = sprite index, Y = field offset, A = value.
; Clobbers: A, Y, NVR7L/NVR7H.
; @label SPRITE.SET_REG8
; @kind routine
; @symbol sprite_set_reg8
; @summary Write one 8-bit sprite attribute field.
; @in X: Sprite index.
; @in Y: Sprite field offset.
; @in A: Field value.
sprite_set_reg8:
      STA   NVR7H
      JSR   sprite_reg_offset
      LDA   NVR7H
      STA   VGC_SPR_BASE,Y
      RTS

; Input: X = sprite index, Y = field offset, NVR0L/NVR0H = little-endian value.
; Clobbers: A, Y, NVR7L.
; @label SPRITE.SET_REG16
; @kind routine
; @symbol sprite_set_reg16
; @summary Write one 16-bit sprite attribute field.
; @in X: Sprite index.
; @in Y: Sprite field offset.
; @in NVR0L: Low byte.
; @in NVR0H: High byte.
sprite_set_reg16:
      JSR   sprite_reg_offset
      LDA   NVR0L
      STA   VGC_SPR_BASE,Y
      INY
      LDA   NVR0H
      STA   VGC_SPR_BASE,Y
      RTS

; Input: X = sprite index. Output: A = X high byte, Y = X low byte.
; @label SPRITE.GET_X
; @kind routine
; @symbol sprite_get_x
; @summary Read a sprite's 16-bit X coordinate.
; @in X: Sprite index.
; @out A: X high byte.
; @out Y: X low byte.
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
; @label SPRITE.GET_Y
; @kind routine
; @symbol sprite_get_y
; @summary Read a sprite's Y coordinate.
; @in X: Sprite index.
; @out Y: Y coordinate byte.
sprite_get_y:
      LDY   #VGC_SPR_YL_OFF
      JSR   sprite_reg_offset
      LDA   VGC_SPR_BASE,Y
      TAY
      RTS

; @label SPRITE.COLLISION_STATUS
; @kind routine
; @symbol sprite_collision_status
; @summary Read the sprite-sprite collision status register.
; @out A: Collision status byte.
sprite_collision_status:
      LDA   VGC_COLLST
      RTS

; @label SPRITE.BACKGROUND_STATUS
; @kind routine
; @symbol sprite_background_collision_status
; @summary Read the sprite-background collision status register.
; @out A: Background collision status byte.
sprite_background_collision_status:
      LDA   VGC_COLLBG
      RTS

.endif

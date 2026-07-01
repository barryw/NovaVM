; Shared Nova sprite helper declarations.
;
; Command helpers use VGC_P* registers directly. Direct sprite-register helpers
; use the public sprite attribute register block at VGC_SPR_BASE.

.ifndef SPRITE_INC_INCLUDED
SPRITE_INC_INCLUDED = 1

.include "vgc.inc"

SPRITE_RESULT_OK    = $00
SPRITE_RESULT_ERROR = $01

.global sprite_command
.global sprite_pos
.global sprite_enable
.global sprite_disable
.global sprite_row
.global sprite_clear
.global sprite_copy
.global sprite_flip
.global sprite_priority
.global sprite_reg_offset
.global sprite_set_shape
.global sprite_set_reg8
.global sprite_set_reg16
.global sprite_get_x
.global sprite_get_y
.global sprite_collision_status
.global sprite_collision_mask
.global sprite_collision_clear
.global sprite_collision_read_clear
.global sprite_collision_irq_enable
.global sprite_collision_irq_disable
.global sprite_background_collision_status
.global sprite_background_collision_mask
.global sprite_background_clear
.global sprite_background_read_clear
.global sprite_background_irq_enable
.global sprite_background_irq_disable

.endif

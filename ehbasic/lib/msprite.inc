; Shared Nova hardware meta-sprite declarations.
;
; A meta-sprite object owns one or more hardware sprites and updates them as a
; single visual object. Shape data is not rewritten for animation; descriptors
; point parts at preloaded shape-slot ranges and animation frames change the
; shape index assigned to each hardware sprite.

.ifndef MSPRITE_INC_INCLUDED
MSPRITE_INC_INCLUDED = 1

.include "sprite.inc"

MSPRITE_RESULT_OK         = $00
MSPRITE_RESULT_ERROR      = $01
MSPRITE_RESULT_NO_OBJECT  = $02
MSPRITE_RESULT_NO_SPRITES = $03
MSPRITE_RESULT_BAD_HANDLE = $04

MSPRITE_INVALID_HANDLE    = $FF
MSPRITE_MAX_OBJECTS       = $08
MSPRITE_MAX_PARTS         = VGC_MAX_SPRITES

MSPRITE_VIS_PART_COUNT    = $00
MSPRITE_VIS_FLAGS         = $01
MSPRITE_VIS_PARTS         = $02
MSPRITE_VIS_PART_SIZE     = $04
MSPRITE_PART_DX           = $00
MSPRITE_PART_DY           = $01
MSPRITE_PART_SHAPE_BASE   = $02
MSPRITE_PART_FLAGS        = $03

MSPRITE_ANIM_FRAME_COUNT  = $00
MSPRITE_ANIM_TICKS        = $01
MSPRITE_ANIM_FLAGS        = $02
MSPRITE_ANIM_RESERVED     = $03
MSPRITE_ANIM_SIZE         = $04
MSPRITE_ANIM_LOOP         = $01

MSPRITE_OBJ_FLAG_ACTIVE   = $80
MSPRITE_OBJ_FLAG_VISIBLE  = $40
MSPRITE_OBJ_FLAG_DIRTY    = $20

; @label MSPRITE.DESC_L
; @kind u8
; @symbol MSPRITE_DESC_L
; @summary Low byte of the visual descriptor pointer used by MSPRITE.SPAWN.
.global MSPRITE_DESC_L
; @label MSPRITE.DESC_H
; @kind u8
; @symbol MSPRITE_DESC_H
; @summary High byte of the visual descriptor pointer used by MSPRITE.SPAWN.
.global MSPRITE_DESC_H
; @label MSPRITE.ANIM_L
; @kind u8
; @symbol MSPRITE_ANIM_L
; @summary Low byte of the animation descriptor pointer used by MSPRITE.SET_ANIM.
.global MSPRITE_ANIM_L
; @label MSPRITE.ANIM_H
; @kind u8
; @symbol MSPRITE_ANIM_H
; @summary High byte of the animation descriptor pointer used by MSPRITE.SET_ANIM.
.global MSPRITE_ANIM_H
; @label MSPRITE.RESULT
; @kind u8
; @symbol MSPRITE_RESULT
; @summary Result code from the last meta-sprite operation.
.global MSPRITE_RESULT

.global msprite_init
.global msprite_spawn
.global msprite_destroy
.global msprite_show
.global msprite_hide
.global msprite_set_pos
.global msprite_set_frame
.global msprite_set_anim
.global msprite_set_priority
.global msprite_set_transcolor
.global msprite_tick
.global msprite_commit
.global msprite_commit_one

.endif

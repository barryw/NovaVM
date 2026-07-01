; Shared Nova sprite animation declarations.
;
; The animation runtime owns timing/state for hardware-sprite and meta-sprite
; animation tracks. Shape bytes are loaded in bulk; per-frame animation changes
; only shape/frame indexes.

.ifndef ANIM_INC_INCLUDED
ANIM_INC_INCLUDED = 1

.include "dma.inc"
.include "msprite.inc"
.include "pager.inc"
.include "xram.inc"

ANIM_RESULT_OK         = $00
ANIM_RESULT_ERROR      = $01
ANIM_RESULT_NO_TRACK   = $02
ANIM_RESULT_BAD_HANDLE = $03
ANIM_RESULT_BAD_DESC   = $04
ANIM_RESULT_LOAD_ERROR = $05

ANIM_INVALID_HANDLE    = $FF
ANIM_MAX_TRACKS        = $08
ANIM_SHAPE_BYTES       = $80

ANIM_TARGET_SPRITE     = $00
ANIM_TARGET_MSPRITE    = $01

ANIM_DESC_FRAME_COUNT  = $00
ANIM_DESC_TICKS        = $01
ANIM_DESC_FLAGS        = $02
ANIM_DESC_FIRST        = $03
ANIM_DESC_STRIDE       = $04
ANIM_DESC_RESERVED     = $05
ANIM_DESC_FRAMES       = $06

ANIM_DESC_LOOP         = $01
ANIM_DESC_PINGPONG     = $02
ANIM_DESC_FRAME_TABLE  = $04
ANIM_DESC_DEFER_COMMIT = $08

ANIM_TRACK_FLAG_ACTIVE  = $80
ANIM_TRACK_FLAG_RUNNING = $40
ANIM_TRACK_FLAG_REVERSE = $20
ANIM_TRACK_FLAG_MSPRITE = $01

; @label ANIM.DESC_L
; @kind u8
; @symbol ANIM_DESC_L
; @summary Low byte of the animation descriptor pointer used by ANIM.START.
.global ANIM_DESC_L
; @label ANIM.DESC_H
; @kind u8
; @symbol ANIM_DESC_H
; @summary High byte of the animation descriptor pointer used by ANIM.START.
.global ANIM_DESC_H
; @label ANIM.TARGET
; @kind u8
; @symbol ANIM_TARGET
; @summary Hardware sprite index or meta-sprite handle used by ANIM.START.
.global ANIM_TARGET
; @label ANIM.TARGET_TYPE
; @kind u8
; @symbol ANIM_TARGET_TYPE
; @summary Target type selector: ANIM_TARGET_SPRITE or ANIM_TARGET_MSPRITE.
.global ANIM_TARGET_TYPE
; @label ANIM.RESULT
; @kind u8
; @symbol ANIM_RESULT
; @summary Result code from the last animation operation.
.global ANIM_RESULT

; Shape loading pseudo-registers.
; @label ANIM.SHAPE
; @kind u8
; @symbol ANIM_SHAPE
; @summary Destination shape slot for bulk shape loading.
.global ANIM_SHAPE
; @label ANIM.COUNT
; @kind u8
; @symbol ANIM_COUNT
; @summary Number of 128-byte shape slots to load.
.global ANIM_COUNT
; @label ANIM.XADDRL
; @kind u8
; @symbol ANIM_XADDRL
; @summary Low byte of flat XRAM source address for ANIM.LOAD_XRAM_SHAPES.
.global ANIM_XADDRL
; @label ANIM.XADDRM
; @kind u8
; @symbol ANIM_XADDRM
; @summary Middle byte of flat XRAM source address for ANIM.LOAD_XRAM_SHAPES.
.global ANIM_XADDRM
; @label ANIM.XADDRH
; @kind u8
; @symbol ANIM_XADDRH
; @summary High byte of flat XRAM source address for ANIM.LOAD_XRAM_SHAPES.
.global ANIM_XADDRH
; @label ANIM.FILEL
; @kind u8
; @symbol ANIM_FILEL
; @summary Low byte of source file offset for ANIM.LOAD_DISK_SHAPES.
.global ANIM_FILEL
; @label ANIM.FILEM
; @kind u8
; @symbol ANIM_FILEM
; @summary Middle byte of source file offset for ANIM.LOAD_DISK_SHAPES.
.global ANIM_FILEM
; @label ANIM.FILEH
; @kind u8
; @symbol ANIM_FILEH
; @summary High byte of source file offset for ANIM.LOAD_DISK_SHAPES.
.global ANIM_FILEH
; @label ANIM.NAMELEN
; @kind u8
; @symbol ANIM_NAMELEN
; @summary Filename length for ANIM.LOAD_DISK_SHAPES.
.global ANIM_NAMELEN
; @label ANIM.NAMEPTR_L
; @kind u8
; @symbol ANIM_NAMEPTR_L
; @summary Low byte of filename pointer for ANIM.LOAD_DISK_SHAPES.
.global ANIM_NAMEPTR_L
; @label ANIM.NAMEPTR_H
; @kind u8
; @symbol ANIM_NAMEPTR_H
; @summary High byte of filename pointer for ANIM.LOAD_DISK_SHAPES.
.global ANIM_NAMEPTR_H

.global anim_init
.global anim_start
.global anim_stop
.global anim_tick
.global anim_tick_one
.global anim_set_frame
.global anim_load_xram_shapes
.global anim_load_disk_shapes

.endif

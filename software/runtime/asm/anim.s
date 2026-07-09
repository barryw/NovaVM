; =====================================================================
;  Nova NDK — anim.s
;
;  Shared sprite animation engine: a fixed table of animation tracks that
;  bind an animation descriptor to a sprite or multi-sprite target, advance
;  them per frame, and load shape frames from XRAM or disk.
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

.include "anim.inc"

.ifndef ANIM_IMPLEMENTATION_INCLUDED
ANIM_IMPLEMENTATION_INCLUDED = 1

      .segment "BSS"

ANIM_DESC_L:           .res 1
ANIM_DESC_H:           .res 1
ANIM_TARGET:           .res 1
ANIM_TARGET_TYPE:      .res 1
ANIM_RESULT:           .res 1

ANIM_SHAPE:            .res 1
ANIM_COUNT:            .res 1
ANIM_XADDRL:           .res 1
ANIM_XADDRM:           .res 1
ANIM_XADDRH:           .res 1
ANIM_FILEL:            .res 1
ANIM_FILEM:            .res 1
ANIM_FILEH:            .res 1
ANIM_NAMELEN:          .res 1
ANIM_NAMEPTR_L:        .res 1
ANIM_NAMEPTR_H:        .res 1

ANIM_TRACK_FLAGS:      .res ANIM_MAX_TRACKS
ANIM_TRACK_TARGET:     .res ANIM_MAX_TRACKS
ANIM_TRACK_DESC_L:     .res ANIM_MAX_TRACKS
ANIM_TRACK_DESC_H:     .res ANIM_MAX_TRACKS
ANIM_TRACK_FRAME:      .res ANIM_MAX_TRACKS
ANIM_TRACK_TICK:       .res ANIM_MAX_TRACKS

ANIM_HANDLE_TMP:       .res 1
ANIM_FRAME_TMP:        .res 1
ANIM_FRAME_VALUE:      .res 1
ANIM_FRAME_COUNT_TMP:  .res 1
ANIM_TICK_TMP:         .res 1
ANIM_FLAGS_TMP:        .res 1
ANIM_VALUE_TMP:        .res 1
ANIM_LOOP_INDEX:       .res 1
ANIM_ADDRL_TMP:        .res 1
ANIM_ADDRH_TMP:        .res 1
ANIM_LENL_TMP:         .res 1
ANIM_LENH_TMP:         .res 1

      .segment "ZEROPAGE"

ANIM_DESC_PTR:         .res 2

      .segment "CODE"

      .export anim_init
      .export anim_start
      .export anim_stop
      .export anim_tick
      .export anim_tick_one
      .export anim_set_frame
      .export anim_load_xram_shapes
      .export anim_load_disk_shapes

; @label ANIM.INIT
; @kind routine
; @symbol anim_init
; @summary Clear the animation track table.
; @out A: 0 on success.
anim_init:
      LDX   #$00
@loop:
      STZ   ANIM_TRACK_FLAGS,X
      STZ   ANIM_TRACK_TARGET,X
      STZ   ANIM_TRACK_DESC_L,X
      STZ   ANIM_TRACK_DESC_H,X
      STZ   ANIM_TRACK_FRAME,X
      STZ   ANIM_TRACK_TICK,X
      INX
      CPX   #ANIM_MAX_TRACKS
      BNE   @loop
      JMP   anim_ok

; @label ANIM.START
; @kind routine
; @symbol anim_start
; @summary Attach ANIM.DESC to ANIM.TARGET and return a track handle.
; @requires ANIM_DESC_L ANIM_DESC_H ANIM_TARGET ANIM_TARGET_TYPE
; @out A: Track handle on success, $FF on failure.
anim_start:
      LDA   ANIM_DESC_L
      STA   ANIM_DESC_PTR
      LDA   ANIM_DESC_H
      STA   ANIM_DESC_PTR+1
      ORA   ANIM_DESC_L
      BEQ   @bad_desc

      LDY   #ANIM_DESC_FRAME_COUNT
      LDA   (ANIM_DESC_PTR),Y
      BEQ   @bad_desc

      LDA   ANIM_TARGET_TYPE
      CMP   #ANIM_TARGET_MSPRITE + 1
      BCS   @bad_desc

      JSR   anim_find_track
      BCC   @track_ok
      LDA   #ANIM_RESULT_NO_TRACK
      JMP   anim_fail

@track_ok:
      STX   ANIM_HANDLE_TMP
      LDA   #(ANIM_TRACK_FLAG_ACTIVE | ANIM_TRACK_FLAG_RUNNING)
      LDY   ANIM_TARGET_TYPE
      BEQ   @store_flags
      ORA   #ANIM_TRACK_FLAG_MSPRITE
@store_flags:
      STA   ANIM_TRACK_FLAGS,X
      LDA   ANIM_TARGET
      STA   ANIM_TRACK_TARGET,X
      LDA   ANIM_DESC_L
      STA   ANIM_TRACK_DESC_L,X
      LDA   ANIM_DESC_H
      STA   ANIM_TRACK_DESC_H,X
      STZ   ANIM_TRACK_FRAME,X
      STZ   ANIM_TRACK_TICK,X

      JSR   anim_apply_selected
      BEQ   @ok
      LDX   ANIM_HANDLE_TMP
      STZ   ANIM_TRACK_FLAGS,X
      LDA   #ANIM_RESULT_BAD_HANDLE
      JMP   anim_fail

@ok:
      LDA   #ANIM_RESULT_OK
      STA   ANIM_RESULT
      LDA   ANIM_HANDLE_TMP
      RTS

@bad_desc:
      LDA   #ANIM_RESULT_BAD_DESC
      JMP   anim_fail

; @label ANIM.STOP
; @kind routine
; @symbol anim_stop
; @summary Stop and release one animation track.
; @in A: Track handle.
; @out A: Result code.
anim_stop:
      JSR   anim_select_handle
      BCC   @valid
      JMP   anim_bad_handle
@valid:
      STZ   ANIM_TRACK_FLAGS,X
      JMP   anim_ok

; @label ANIM.TICK
; @kind routine
; @symbol anim_tick
; @summary Advance every running animation track and apply changed frames.
; @out A: 0 on success.
anim_tick:
      LDX   #$00
@loop:
      STX   ANIM_LOOP_INDEX
      LDA   ANIM_TRACK_FLAGS,X
      AND   #(ANIM_TRACK_FLAG_ACTIVE | ANIM_TRACK_FLAG_RUNNING)
      CMP   #(ANIM_TRACK_FLAG_ACTIVE | ANIM_TRACK_FLAG_RUNNING)
      BNE   @next
      JSR   anim_tick_selected
@next:
      LDX   ANIM_LOOP_INDEX
      INX
      CPX   #ANIM_MAX_TRACKS
      BNE   @loop
      JMP   anim_ok

; @label ANIM.TICK_ONE
; @kind routine
; @symbol anim_tick_one
; @summary Advance one running animation track.
; @in A: Track handle.
; @out A: Result code.
anim_tick_one:
      JSR   anim_select_handle
      BCC   @valid
      JMP   anim_bad_handle
@valid:
      LDA   ANIM_TRACK_FLAGS,X
      AND   #ANIM_TRACK_FLAG_RUNNING
      BEQ   @ok
      JSR   anim_tick_selected
      RTS
@ok:
      JMP   anim_ok

; @label ANIM.SET_FRAME
; @kind routine
; @symbol anim_set_frame
; @summary Set one animation track to a frame index and apply it immediately.
; @in A: Track handle.
; @in X: Frame index.
; @out A: Result code.
anim_set_frame:
      STX   ANIM_FRAME_TMP
      JSR   anim_select_handle
      BCC   @valid
      JMP   anim_bad_handle
@valid:
      STX   ANIM_HANDLE_TMP
      JSR   anim_load_desc_selected
      LDY   #ANIM_DESC_FRAME_COUNT
      LDA   ANIM_FRAME_TMP
      CMP   (ANIM_DESC_PTR),Y
      BCC   @store
      LDA   #ANIM_RESULT_BAD_DESC
      JMP   anim_set_error
@store:
      LDX   ANIM_HANDLE_TMP
      LDA   ANIM_FRAME_TMP
      STA   ANIM_TRACK_FRAME,X
      STZ   ANIM_TRACK_TICK,X
      LDA   ANIM_TRACK_FLAGS,X
      AND   #$DF
      ORA   #ANIM_TRACK_FLAG_RUNNING
      STA   ANIM_TRACK_FLAGS,X
      JMP   anim_apply_selected

; @label ANIM.LOAD_XRAM_SHAPES
; @kind routine
; @symbol anim_load_xram_shapes
; @summary DMA ANIM.COUNT shape slots from ANIM.XADDR into VGC sprite shape RAM at ANIM.SHAPE.
; @requires ANIM_XADDRL ANIM_XADDRM ANIM_XADDRH ANIM_SHAPE ANIM_COUNT
; @out A: 0 on success, 1 on error.
anim_load_xram_shapes:
      JSR   anim_prepare_shape_transfer
      BEQ   @ok
      RTS
@ok:
      LDA   #DMA_SPACE_XRAM
      STA   DMA_SRCSPACE
      LDA   ANIM_XADDRL
      STA   DMA_SRCL
      LDA   ANIM_XADDRM
      STA   DMA_SRCM
      LDA   ANIM_XADDRH
      STA   DMA_SRCH

      LDA   #DMA_SPACE_VGC_SPRITE
      STA   DMA_DSTSPACE
      LDA   ANIM_ADDRL_TMP
      STA   DMA_DSTL
      LDA   ANIM_ADDRH_TMP
      STA   DMA_DSTM
      STZ   DMA_DSTH

      LDA   ANIM_LENL_TMP
      STA   DMA_LENL
      LDA   ANIM_LENH_TMP
      STA   DMA_LENM
      STZ   DMA_LENH

      JSR   dma_start_copy
      CMP   #DMA_RESULT_OK
      BNE   @load_error
      JMP   anim_ok
@load_error:
      LDA   #ANIM_RESULT_LOAD_ERROR
      JMP   anim_set_error

; @label ANIM.LOAD_DISK_SHAPES
; @kind routine
; @symbol anim_load_disk_shapes
; @summary Stream ANIM.COUNT shape slots from a file slice directly into VGC sprite shape RAM at ANIM.SHAPE.
; @requires ANIM_NAMEPTR_L ANIM_NAMEPTR_H ANIM_NAMELEN ANIM_FILEL ANIM_FILEM ANIM_FILEH ANIM_SHAPE ANIM_COUNT
; @out A: 0 on success, 1 on error.
anim_load_disk_shapes:
      JSR   anim_prepare_shape_transfer
      BEQ   @ok
      RTS
@ok:
      LDA   ANIM_NAMELEN
      STA   PAGER_NAMELEN
      LDA   ANIM_NAMEPTR_L
      STA   PAGER_NAMEPTR_L
      LDA   ANIM_NAMEPTR_H
      STA   PAGER_NAMEPTR_H
      LDA   ANIM_FILEL
      STA   PAGER_FILEL
      LDA   ANIM_FILEM
      STA   PAGER_FILEM
      LDA   ANIM_FILEH
      STA   PAGER_FILEH
      LDA   ANIM_ADDRL_TMP
      STA   PAGER_ADDRL
      LDA   ANIM_ADDRH_TMP
      STA   PAGER_ADDRM
      LDA   #VGC_PLANE_SPRITE
      STA   PAGER_ADDRH
      LDA   ANIM_LENL_TMP
      STA   PAGER_LENL
      LDA   ANIM_LENH_TMP
      STA   PAGER_LENH
      LDA   #PAGER_TARGET_VGC
      STA   PAGER_TARGET

      JSR   pager_load_file_page
      BNE   @load_error
      JMP   anim_ok
@load_error:
      LDA   #ANIM_RESULT_LOAD_ERROR
      JMP   anim_set_error

anim_tick_selected:
      STX   ANIM_HANDLE_TMP
      JSR   anim_load_desc_selected

      LDY   #ANIM_DESC_FRAME_COUNT
      LDA   (ANIM_DESC_PTR),Y
      BEQ   @bad_desc
      STA   ANIM_FRAME_COUNT_TMP

      INC   ANIM_TRACK_TICK,X
      LDY   #ANIM_DESC_TICKS
      LDA   ANIM_TRACK_TICK,X
      CMP   (ANIM_DESC_PTR),Y
      BCC   @ok
      STZ   ANIM_TRACK_TICK,X
      JSR   anim_advance_selected
      JSR   anim_apply_selected
      RTS

@bad_desc:
      LDA   #ANIM_RESULT_BAD_DESC
      JMP   anim_set_error
@ok:
      JMP   anim_ok

anim_advance_selected:
      LDX   ANIM_HANDLE_TMP
      LDA   ANIM_TRACK_FLAGS,X
      AND   #ANIM_TRACK_FLAG_REVERSE
      BEQ   @forward

      LDA   ANIM_TRACK_FRAME,X
      BNE   @reverse_step
      LDA   ANIM_TRACK_FLAGS,X
      AND   #$DF
      STA   ANIM_TRACK_FLAGS,X
      LDA   ANIM_FRAME_COUNT_TMP
      CMP   #$02
      BCC   @done
      LDA   #$01
      STA   ANIM_TRACK_FRAME,X
      RTS
@reverse_step:
      DEC   ANIM_TRACK_FRAME,X
@done:
      RTS

@forward:
      INC   ANIM_TRACK_FRAME,X
      LDA   ANIM_TRACK_FRAME,X
      CMP   ANIM_FRAME_COUNT_TMP
      BCC   @done

      JSR   anim_desc_flags
      AND   #ANIM_DESC_LOOP
      BEQ   @clamp_stop

      JSR   anim_desc_flags
      AND   #ANIM_DESC_PINGPONG
      BEQ   @loop_restart

      LDA   ANIM_FRAME_COUNT_TMP
      CMP   #$02
      BCC   @loop_restart
      SEC
      SBC   #$02
      LDX   ANIM_HANDLE_TMP
      STA   ANIM_TRACK_FRAME,X
      LDA   ANIM_TRACK_FLAGS,X
      ORA   #ANIM_TRACK_FLAG_REVERSE
      STA   ANIM_TRACK_FLAGS,X
      RTS

@loop_restart:
      LDX   ANIM_HANDLE_TMP
      STZ   ANIM_TRACK_FRAME,X
      RTS

@clamp_stop:
      LDA   ANIM_FRAME_COUNT_TMP
      SEC
      SBC   #$01
      LDX   ANIM_HANDLE_TMP
      STA   ANIM_TRACK_FRAME,X
      LDA   ANIM_TRACK_FLAGS,X
      AND   #$BF
      STA   ANIM_TRACK_FLAGS,X
      RTS

anim_apply_selected:
      STX   ANIM_HANDLE_TMP
      JSR   anim_load_desc_selected
      JSR   anim_resolve_frame
      STA   ANIM_FRAME_VALUE

      LDX   ANIM_HANDLE_TMP
      LDA   ANIM_TRACK_FLAGS,X
      AND   #ANIM_TRACK_FLAG_MSPRITE
      BNE   @msprite

      LDA   ANIM_TRACK_TARGET,X
      CMP   #VGC_MAX_SPRITES
      BCS   @bad_target
      TAX
      LDA   ANIM_FRAME_VALUE
      JSR   sprite_set_shape
      JMP   anim_ok

@msprite:
      LDA   ANIM_TRACK_TARGET,X
      STA   ANIM_VALUE_TMP
      LDA   ANIM_FRAME_VALUE
      TAX
      LDA   ANIM_VALUE_TMP
      JSR   msprite_set_frame
      BNE   @bad_target

      JSR   anim_desc_flags
      AND   #ANIM_DESC_DEFER_COMMIT
      BNE   @ok
      LDX   ANIM_HANDLE_TMP
      LDA   ANIM_TRACK_TARGET,X
      JSR   msprite_commit_one
      BNE   @bad_target
@ok:
      JMP   anim_ok

@bad_target:
      LDA   #ANIM_RESULT_BAD_HANDLE
      JMP   anim_set_error

anim_resolve_frame:
      JSR   anim_desc_flags
      AND   #ANIM_DESC_FRAME_TABLE
      BEQ   @linear
      LDX   ANIM_HANDLE_TMP
      LDA   ANIM_TRACK_FRAME,X
      CLC
      ADC   #ANIM_DESC_FRAMES
      TAY
      LDA   (ANIM_DESC_PTR),Y
      RTS

@linear:
      LDY   #ANIM_DESC_FIRST
      LDA   (ANIM_DESC_PTR),Y
      STA   ANIM_VALUE_TMP
      LDY   #ANIM_DESC_STRIDE
      LDA   (ANIM_DESC_PTR),Y
      STA   ANIM_FRAME_TMP
      LDX   ANIM_HANDLE_TMP
      LDA   ANIM_TRACK_FRAME,X
      TAX
      BEQ   @done
@loop:
      LDA   ANIM_VALUE_TMP
      CLC
      ADC   ANIM_FRAME_TMP
      STA   ANIM_VALUE_TMP
      DEX
      BNE   @loop
@done:
      LDA   ANIM_VALUE_TMP
      RTS

anim_prepare_shape_transfer:
      LDA   ANIM_COUNT
      BEQ   @bad
      LSR
      STA   ANIM_LENH_TMP
      LDA   #$00
      BCC   @len_low
      LDA   #$80
@len_low:
      STA   ANIM_LENL_TMP

      LDA   ANIM_SHAPE
      LSR
      STA   ANIM_ADDRH_TMP
      LDA   #$00
      BCC   @addr_low
      LDA   #$80
@addr_low:
      STA   ANIM_ADDRL_TMP
      JMP   anim_ok

@bad:
      LDA   #ANIM_RESULT_BAD_DESC
      JMP   anim_set_error

anim_find_track:
      LDX   #$00
@loop:
      LDA   ANIM_TRACK_FLAGS,X
      AND   #ANIM_TRACK_FLAG_ACTIVE
      BEQ   @found
      INX
      CPX   #ANIM_MAX_TRACKS
      BNE   @loop
      SEC
      RTS
@found:
      CLC
      RTS

anim_select_handle:
      CMP   #ANIM_MAX_TRACKS
      BCS   @bad
      TAX
      LDA   ANIM_TRACK_FLAGS,X
      AND   #ANIM_TRACK_FLAG_ACTIVE
      BEQ   @bad
      CLC
      RTS
@bad:
      SEC
      RTS

anim_load_desc_selected:
      LDA   ANIM_TRACK_DESC_L,X
      STA   ANIM_DESC_PTR
      LDA   ANIM_TRACK_DESC_H,X
      STA   ANIM_DESC_PTR+1
      RTS

anim_desc_flags:
      LDY   #ANIM_DESC_FLAGS
      LDA   (ANIM_DESC_PTR),Y
      RTS

anim_ok:
      LDA   #ANIM_RESULT_OK
      STA   ANIM_RESULT
      RTS

anim_bad_handle:
      LDA   #ANIM_RESULT_BAD_HANDLE
      JMP   anim_set_error

anim_set_error:
      STA   ANIM_RESULT
      LDA   #ANIM_RESULT_ERROR
      RTS

anim_fail:
      STA   ANIM_RESULT
      LDA   #ANIM_INVALID_HANDLE
      RTS

.endif

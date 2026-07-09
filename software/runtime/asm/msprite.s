; =====================================================================
;  Nova NDK — msprite.s
;
;  Shared Nova hardware meta-sprite routines: allocate ranges of hardware
;  sprites, treat each range as one animated object, and update only
;  sprite registers during animation. Shape frames must already be loaded
;  into VGC sprite shape RAM.
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

.include "msprite.inc"

.ifndef MSPRITE_IMPLEMENTATION_INCLUDED
MSPRITE_IMPLEMENTATION_INCLUDED = 1

      .segment "BSS"

MSPRITE_DESC_L:          .res 1
MSPRITE_DESC_H:          .res 1
MSPRITE_ANIM_L:          .res 1
MSPRITE_ANIM_H:          .res 1
MSPRITE_RESULT:          .res 1

MSPRITE_OBJ_FLAGS:       .res MSPRITE_MAX_OBJECTS
MSPRITE_OBJ_DESC_L:      .res MSPRITE_MAX_OBJECTS
MSPRITE_OBJ_DESC_H:      .res MSPRITE_MAX_OBJECTS
MSPRITE_OBJ_FIRST:       .res MSPRITE_MAX_OBJECTS
MSPRITE_OBJ_PARTS:       .res MSPRITE_MAX_OBJECTS
MSPRITE_OBJ_XL:          .res MSPRITE_MAX_OBJECTS
MSPRITE_OBJ_XH:          .res MSPRITE_MAX_OBJECTS
MSPRITE_OBJ_Y:           .res MSPRITE_MAX_OBJECTS
MSPRITE_OBJ_FRAME:       .res MSPRITE_MAX_OBJECTS
MSPRITE_OBJ_ANIM_L:      .res MSPRITE_MAX_OBJECTS
MSPRITE_OBJ_ANIM_H:      .res MSPRITE_MAX_OBJECTS
MSPRITE_OBJ_ANIM_TICK:   .res MSPRITE_MAX_OBJECTS
MSPRITE_OBJ_PRIORITY:    .res MSPRITE_MAX_OBJECTS
MSPRITE_OBJ_TRANS:       .res MSPRITE_MAX_OBJECTS
MSPRITE_HW_OWNER:        .res VGC_MAX_SPRITES

MSPRITE_HANDLE_TMP:      .res 1
MSPRITE_FRAME_TMP:       .res 1
MSPRITE_VALUE_TMP:       .res 1
MSPRITE_PART_COUNT_TMP:  .res 1
MSPRITE_PART_INDEX:      .res 1
MSPRITE_PART_REMAIN:     .res 1
MSPRITE_SEARCH_START:    .res 1
MSPRITE_HW_INDEX:        .res 1
MSPRITE_REG_OFF:         .res 1
MSPRITE_X_TMP_L:         .res 1
MSPRITE_X_TMP_H:         .res 1
MSPRITE_Y_TMP:           .res 1
MSPRITE_SHAPE_TMP:       .res 1
MSPRITE_FLAGS_TMP:       .res 1
MSPRITE_LOOP_INDEX:      .res 1

      .segment "ZEROPAGE"

MSPRITE_DESC_PTR:        .res 2
MSPRITE_PART_PTR:        .res 2
MSPRITE_ANIM_PTR:        .res 2

      .segment "CODE"

      .export msprite_init
      .export msprite_spawn
      .export msprite_destroy
      .export msprite_show
      .export msprite_hide
      .export msprite_set_pos
      .export msprite_set_frame
      .export msprite_set_anim
      .export msprite_set_priority
      .export msprite_set_transcolor
      .export msprite_tick
      .export msprite_commit
      .export msprite_commit_one

; @label MSPRITE.INIT
; @kind routine
; @symbol msprite_init
; @summary Clear the meta-sprite object table, release all hardware sprites, and disable every hardware sprite.
; @out A: 0 on success.
msprite_init:
      LDX   #$00
@object_loop:
      STZ   MSPRITE_OBJ_FLAGS,X
      STZ   MSPRITE_OBJ_DESC_L,X
      STZ   MSPRITE_OBJ_DESC_H,X
      STZ   MSPRITE_OBJ_FIRST,X
      STZ   MSPRITE_OBJ_PARTS,X
      STZ   MSPRITE_OBJ_XL,X
      STZ   MSPRITE_OBJ_XH,X
      STZ   MSPRITE_OBJ_Y,X
      STZ   MSPRITE_OBJ_FRAME,X
      STZ   MSPRITE_OBJ_ANIM_L,X
      STZ   MSPRITE_OBJ_ANIM_H,X
      STZ   MSPRITE_OBJ_ANIM_TICK,X
      STZ   MSPRITE_OBJ_PRIORITY,X
      STZ   MSPRITE_OBJ_TRANS,X
      INX
      CPX   #MSPRITE_MAX_OBJECTS
      BNE   @object_loop

      LDX   #$00
@sprite_loop:
      STZ   MSPRITE_HW_OWNER,X
      TXA
      ASL
      ASL
      ASL
      TAY
      LDA   #$00
      STA   VGC_SPR_BASE + VGC_SPR_FLAGS_OFF,Y
      INX
      CPX   #VGC_MAX_SPRITES
      BNE   @sprite_loop

      JMP   msprite_ok

; @label MSPRITE.SPAWN
; @kind routine
; @symbol msprite_spawn
; @summary Allocate hardware sprites for MSPRITE.DESC and return a new object handle.
; @requires MSPRITE_DESC_L MSPRITE_DESC_H
; @out A: Object handle on success, $FF on failure.
; @out MSPRITE_RESULT Result code.
msprite_spawn:
      LDA   MSPRITE_DESC_L
      STA   MSPRITE_DESC_PTR
      LDA   MSPRITE_DESC_H
      STA   MSPRITE_DESC_PTR+1
      ORA   MSPRITE_DESC_L
      BNE   @read_parts
      LDA   #MSPRITE_RESULT_ERROR
      JMP   msprite_fail

@read_parts:
      LDY   #MSPRITE_VIS_PART_COUNT
      LDA   (MSPRITE_DESC_PTR),Y
      BEQ   @bad_desc
      CMP   #(MSPRITE_MAX_PARTS + 1)
      BCC   @parts_ok
@bad_desc:
      LDA   #MSPRITE_RESULT_ERROR
      JMP   msprite_fail

@parts_ok:
      STA   MSPRITE_PART_COUNT_TMP
      JSR   msprite_find_object
      BCC   @object_ok
      LDA   #MSPRITE_RESULT_NO_OBJECT
      JMP   msprite_fail

@object_ok:
      STX   MSPRITE_HANDLE_TMP
      JSR   msprite_find_sprite_range
      BCC   @sprites_ok
      LDA   #MSPRITE_RESULT_NO_SPRITES
      JMP   msprite_fail

@sprites_ok:
      JSR   msprite_mark_sprite_range

      LDX   MSPRITE_HANDLE_TMP
      LDA   #(MSPRITE_OBJ_FLAG_ACTIVE | MSPRITE_OBJ_FLAG_VISIBLE | MSPRITE_OBJ_FLAG_DIRTY)
      STA   MSPRITE_OBJ_FLAGS,X
      LDA   MSPRITE_DESC_L
      STA   MSPRITE_OBJ_DESC_L,X
      LDA   MSPRITE_DESC_H
      STA   MSPRITE_OBJ_DESC_H,X
      LDA   MSPRITE_SEARCH_START
      STA   MSPRITE_OBJ_FIRST,X
      LDA   MSPRITE_PART_COUNT_TMP
      STA   MSPRITE_OBJ_PARTS,X
      STZ   MSPRITE_OBJ_XL,X
      STZ   MSPRITE_OBJ_XH,X
      STZ   MSPRITE_OBJ_Y,X
      STZ   MSPRITE_OBJ_FRAME,X
      STZ   MSPRITE_OBJ_ANIM_L,X
      STZ   MSPRITE_OBJ_ANIM_H,X
      STZ   MSPRITE_OBJ_ANIM_TICK,X
      LDA   #VGC_SPR_PRI_FRONT
      STA   MSPRITE_OBJ_PRIORITY,X
      STZ   MSPRITE_OBJ_TRANS,X

      LDA   #MSPRITE_RESULT_OK
      STA   MSPRITE_RESULT
      LDA   MSPRITE_HANDLE_TMP
      RTS

; @label MSPRITE.DESTROY
; @kind routine
; @symbol msprite_destroy
; @summary Destroy one meta-sprite object, disable its hardware sprites, and release its sprite allocation.
; @in A: Object handle.
; @out A: Result code.
msprite_destroy:
      JSR   msprite_select_handle
      BCC   @valid
      JMP   msprite_bad_handle
@valid:
      STX   MSPRITE_HANDLE_TMP
      JSR   msprite_disable_object_parts
      JSR   msprite_free_sprite_range
      LDX   MSPRITE_HANDLE_TMP
      STZ   MSPRITE_OBJ_FLAGS,X
      JMP   msprite_ok

; @label MSPRITE.SHOW
; @kind routine
; @symbol msprite_show
; @summary Mark one object visible and dirty so its child hardware sprites are enabled on the next commit.
; @in A: Object handle.
; @out A: Result code.
msprite_show:
      JSR   msprite_select_handle
      BCC   @valid
      JMP   msprite_bad_handle
@valid:
      LDA   MSPRITE_OBJ_FLAGS,X
      ORA   #(MSPRITE_OBJ_FLAG_VISIBLE | MSPRITE_OBJ_FLAG_DIRTY)
      STA   MSPRITE_OBJ_FLAGS,X
      JMP   msprite_ok

; @label MSPRITE.HIDE
; @kind routine
; @symbol msprite_hide
; @summary Mark one object hidden and immediately disable its child hardware sprites.
; @in A: Object handle.
; @out A: Result code.
msprite_hide:
      JSR   msprite_select_handle
      BCC   @valid
      JMP   msprite_bad_handle
@valid:
      STX   MSPRITE_HANDLE_TMP
      JSR   msprite_disable_object_parts
      LDX   MSPRITE_HANDLE_TMP
      LDA   MSPRITE_OBJ_FLAGS,X
      AND   #$9F
      ORA   #MSPRITE_OBJ_FLAG_ACTIVE
      STA   MSPRITE_OBJ_FLAGS,X
      JMP   msprite_ok

; @label MSPRITE.SET_POS
; @kind routine
; @symbol msprite_set_pos
; @summary Set an object's position and mark it dirty.
; @in A: Object handle.
; @in NVR0L: X low byte.
; @in NVR0H: X high byte.
; @in Y: Y coordinate byte.
; @out A: Result code.
msprite_set_pos:
      STY   MSPRITE_Y_TMP
      JSR   msprite_select_handle
      BCC   @valid
      JMP   msprite_bad_handle
@valid:
      LDA   NVR0L
      STA   MSPRITE_OBJ_XL,X
      LDA   NVR0H
      STA   MSPRITE_OBJ_XH,X
      LDA   MSPRITE_Y_TMP
      STA   MSPRITE_OBJ_Y,X
      JSR   msprite_mark_dirty_x
      JMP   msprite_ok

; @label MSPRITE.SET_FRAME
; @kind routine
; @symbol msprite_set_frame
; @summary Set an object's animation frame and mark it dirty.
; @in A: Object handle.
; @in X: Frame index.
; @out A: Result code.
msprite_set_frame:
      STX   MSPRITE_FRAME_TMP
      JSR   msprite_select_handle
      BCC   @valid
      JMP   msprite_bad_handle
@valid:
      LDA   MSPRITE_FRAME_TMP
      STA   MSPRITE_OBJ_FRAME,X
      STZ   MSPRITE_OBJ_ANIM_TICK,X
      JSR   msprite_mark_dirty_x
      JMP   msprite_ok

; @label MSPRITE.SET_ANIM
; @kind routine
; @symbol msprite_set_anim
; @summary Attach an optional linear animation descriptor to an object and reset its frame timer.
; @in A: Object handle.
; @requires MSPRITE_ANIM_L MSPRITE_ANIM_H
; @out A: Result code.
msprite_set_anim:
      JSR   msprite_select_handle
      BCC   @valid
      JMP   msprite_bad_handle
@valid:
      LDA   MSPRITE_ANIM_L
      STA   MSPRITE_OBJ_ANIM_L,X
      LDA   MSPRITE_ANIM_H
      STA   MSPRITE_OBJ_ANIM_H,X
      STZ   MSPRITE_OBJ_ANIM_TICK,X
      STZ   MSPRITE_OBJ_FRAME,X
      JSR   msprite_mark_dirty_x
      JMP   msprite_ok

; @label MSPRITE.SET_PRIORITY
; @kind routine
; @symbol msprite_set_priority
; @summary Set the shared priority used for all parts of an object.
; @in A: Object handle.
; @in X: Sprite priority, 0 behind, 1 between, 2 front.
; @out A: Result code.
msprite_set_priority:
      STX   MSPRITE_VALUE_TMP
      JSR   msprite_select_handle
      BCC   @valid
      JMP   msprite_bad_handle
@valid:
      LDA   MSPRITE_VALUE_TMP
      CMP   #VGC_SPR_PRI_FRONT
      BCC   @store
      BEQ   @store
      LDA   #VGC_SPR_PRI_FRONT
@store:
      STA   MSPRITE_OBJ_PRIORITY,X
      JSR   msprite_mark_dirty_x
      JMP   msprite_ok

; @label MSPRITE.SET_TRANSCOLOR
; @kind routine
; @symbol msprite_set_transcolor
; @summary Set the transparent color used for all parts of an object.
; @in A: Object handle.
; @in X: Transparent color index.
; @out A: Result code.
msprite_set_transcolor:
      STX   MSPRITE_VALUE_TMP
      JSR   msprite_select_handle
      BCC   @valid
      JMP   msprite_bad_handle
@valid:
      LDA   MSPRITE_VALUE_TMP
      STA   MSPRITE_OBJ_TRANS,X
      JSR   msprite_mark_dirty_x
      JMP   msprite_ok

; @label MSPRITE.TICK
; @kind routine
; @symbol msprite_tick
; @summary Advance animation timers for every active object with an attached animation descriptor.
; @out A: 0 on success.
msprite_tick:
      LDX   #$00
@loop:
      STX   MSPRITE_LOOP_INDEX
      LDA   MSPRITE_OBJ_FLAGS,X
      AND   #MSPRITE_OBJ_FLAG_ACTIVE
      BEQ   @next
      LDA   MSPRITE_OBJ_ANIM_L,X
      STA   MSPRITE_ANIM_PTR
      LDA   MSPRITE_OBJ_ANIM_H,X
      STA   MSPRITE_ANIM_PTR+1
      ORA   MSPRITE_ANIM_PTR
      BEQ   @next

      LDY   #MSPRITE_ANIM_FRAME_COUNT
      LDA   (MSPRITE_ANIM_PTR),Y
      BEQ   @next

      INC   MSPRITE_OBJ_ANIM_TICK,X
      LDY   #MSPRITE_ANIM_TICKS
      LDA   MSPRITE_OBJ_ANIM_TICK,X
      CMP   (MSPRITE_ANIM_PTR),Y
      BCC   @next
      STZ   MSPRITE_OBJ_ANIM_TICK,X

      INC   MSPRITE_OBJ_FRAME,X
      LDY   #MSPRITE_ANIM_FRAME_COUNT
      LDA   MSPRITE_OBJ_FRAME,X
      CMP   (MSPRITE_ANIM_PTR),Y
      BCC   @mark

      LDY   #MSPRITE_ANIM_FLAGS
      LDA   (MSPRITE_ANIM_PTR),Y
      AND   #MSPRITE_ANIM_LOOP
      BEQ   @clamp
      STZ   MSPRITE_OBJ_FRAME,X
      BRA   @mark

@clamp:
      LDY   #MSPRITE_ANIM_FRAME_COUNT
      LDA   (MSPRITE_ANIM_PTR),Y
      SEC
      SBC   #$01
      STA   MSPRITE_OBJ_FRAME,X

@mark:
      JSR   msprite_mark_dirty_x

@next:
      LDX   MSPRITE_LOOP_INDEX
      INX
      CPX   #MSPRITE_MAX_OBJECTS
      BNE   @loop
      JMP   msprite_ok

; @label MSPRITE.COMMIT
; @kind routine
; @symbol msprite_commit
; @summary Write all dirty visible meta-sprite objects to VGC hardware sprite registers.
; @out A: 0 on success.
msprite_commit:
      LDX   #$00
@loop:
      STX   MSPRITE_LOOP_INDEX
      LDA   MSPRITE_OBJ_FLAGS,X
      AND   #(MSPRITE_OBJ_FLAG_ACTIVE | MSPRITE_OBJ_FLAG_VISIBLE | MSPRITE_OBJ_FLAG_DIRTY)
      CMP   #(MSPRITE_OBJ_FLAG_ACTIVE | MSPRITE_OBJ_FLAG_VISIBLE | MSPRITE_OBJ_FLAG_DIRTY)
      BNE   @next
      JSR   msprite_apply_selected
@next:
      LDX   MSPRITE_LOOP_INDEX
      INX
      CPX   #MSPRITE_MAX_OBJECTS
      BNE   @loop
      JMP   msprite_ok

; @label MSPRITE.COMMIT_ONE
; @kind routine
; @symbol msprite_commit_one
; @summary Write one visible meta-sprite object to VGC hardware sprite registers even if it is not dirty.
; @in A: Object handle.
; @out A: Result code.
msprite_commit_one:
      JSR   msprite_select_handle
      BCC   @valid
      JMP   msprite_bad_handle
@valid:
      LDA   MSPRITE_OBJ_FLAGS,X
      AND   #MSPRITE_OBJ_FLAG_VISIBLE
      BEQ   @ok
      JSR   msprite_apply_selected
@ok:
      JMP   msprite_ok

msprite_find_object:
      LDX   #$00
@loop:
      LDA   MSPRITE_OBJ_FLAGS,X
      AND   #MSPRITE_OBJ_FLAG_ACTIVE
      BEQ   @found
      INX
      CPX   #MSPRITE_MAX_OBJECTS
      BNE   @loop
      SEC
      RTS
@found:
      CLC
      RTS

msprite_find_sprite_range:
      STZ   MSPRITE_SEARCH_START
@start_loop:
      LDA   MSPRITE_SEARCH_START
      CLC
      ADC   MSPRITE_PART_COUNT_TMP
      CMP   #(VGC_MAX_SPRITES + 1)
      BCC   @check_range
      SEC
      RTS

@check_range:
      LDX   MSPRITE_SEARCH_START
      LDA   MSPRITE_PART_COUNT_TMP
      STA   MSPRITE_PART_REMAIN
@check_loop:
      LDA   MSPRITE_HW_OWNER,X
      BNE   @next_start
      INX
      DEC   MSPRITE_PART_REMAIN
      BNE   @check_loop
      CLC
      RTS

@next_start:
      INC   MSPRITE_SEARCH_START
      BRA   @start_loop

msprite_mark_sprite_range:
      LDA   MSPRITE_HANDLE_TMP
      CLC
      ADC   #$01
      STA   MSPRITE_VALUE_TMP
      LDX   MSPRITE_SEARCH_START
      LDA   MSPRITE_PART_COUNT_TMP
      STA   MSPRITE_PART_REMAIN
@loop:
      LDA   MSPRITE_VALUE_TMP
      STA   MSPRITE_HW_OWNER,X
      INX
      DEC   MSPRITE_PART_REMAIN
      BNE   @loop
      RTS

msprite_free_sprite_range:
      LDX   MSPRITE_HANDLE_TMP
      LDA   MSPRITE_OBJ_FIRST,X
      TAX
      LDY   MSPRITE_HANDLE_TMP
      LDA   MSPRITE_OBJ_PARTS,Y
      STA   MSPRITE_PART_REMAIN
@loop:
      STZ   MSPRITE_HW_OWNER,X
      INX
      DEC   MSPRITE_PART_REMAIN
      BNE   @loop
      RTS

msprite_disable_object_parts:
      LDX   MSPRITE_HANDLE_TMP
      STZ   MSPRITE_PART_INDEX
      LDA   MSPRITE_OBJ_PARTS,X
      STA   MSPRITE_PART_COUNT_TMP
      BEQ   @done
@loop:
      LDX   MSPRITE_HANDLE_TMP
      LDA   MSPRITE_OBJ_FIRST,X
      CLC
      ADC   MSPRITE_PART_INDEX
      JSR   msprite_disable_hw_sprite
      INC   MSPRITE_PART_INDEX
      LDA   MSPRITE_PART_INDEX
      CMP   MSPRITE_PART_COUNT_TMP
      BNE   @loop
@done:
      RTS

msprite_disable_hw_sprite:
      ASL
      ASL
      ASL
      TAY
      LDA   #$00
      STA   VGC_SPR_BASE + VGC_SPR_FLAGS_OFF,Y
      RTS

msprite_select_handle:
      CMP   #MSPRITE_MAX_OBJECTS
      BCS   @bad
      TAX
      LDA   MSPRITE_OBJ_FLAGS,X
      AND   #MSPRITE_OBJ_FLAG_ACTIVE
      BEQ   @bad
      CLC
      RTS
@bad:
      SEC
      RTS

msprite_mark_dirty_x:
      LDA   MSPRITE_OBJ_FLAGS,X
      ORA   #MSPRITE_OBJ_FLAG_DIRTY
      STA   MSPRITE_OBJ_FLAGS,X
      RTS

msprite_apply_selected:
      STX   MSPRITE_HANDLE_TMP
      LDA   MSPRITE_OBJ_DESC_L,X
      STA   MSPRITE_DESC_PTR
      LDA   MSPRITE_OBJ_DESC_H,X
      STA   MSPRITE_DESC_PTR+1

      LDA   MSPRITE_DESC_PTR
      CLC
      ADC   #MSPRITE_VIS_PARTS
      STA   MSPRITE_PART_PTR
      LDA   MSPRITE_DESC_PTR+1
      ADC   #$00
      STA   MSPRITE_PART_PTR+1

      STZ   MSPRITE_PART_INDEX
      LDA   MSPRITE_OBJ_PARTS,X
      STA   MSPRITE_PART_COUNT_TMP
      BNE   @part_loop
      JMP   @finish

@part_loop:
      LDX   MSPRITE_HANDLE_TMP
      LDA   MSPRITE_OBJ_FIRST,X
      CLC
      ADC   MSPRITE_PART_INDEX
      STA   MSPRITE_HW_INDEX
      ASL
      ASL
      ASL
      STA   MSPRITE_REG_OFF

      LDY   #MSPRITE_PART_DX
      LDA   (MSPRITE_PART_PTR),Y
      STA   MSPRITE_VALUE_TMP
      CLC
      ADC   MSPRITE_OBJ_XL,X
      STA   MSPRITE_X_TMP_L
      LDA   MSPRITE_OBJ_XH,X
      ADC   #$00
      STA   MSPRITE_X_TMP_H
      LDA   MSPRITE_VALUE_TMP
      BPL   @x_done
      DEC   MSPRITE_X_TMP_H
@x_done:
      LDY   #MSPRITE_PART_DY
      LDA   (MSPRITE_PART_PTR),Y
      CLC
      ADC   MSPRITE_OBJ_Y,X
      STA   MSPRITE_Y_TMP

      LDY   #MSPRITE_PART_SHAPE_BASE
      LDA   (MSPRITE_PART_PTR),Y
      CLC
      ADC   MSPRITE_OBJ_FRAME,X
      STA   MSPRITE_SHAPE_TMP

      LDY   #MSPRITE_PART_FLAGS
      LDA   (MSPRITE_PART_PTR),Y
      AND   #VGC_SPR_FLAG_FLIPMASK
      ORA   #VGC_SPR_FLAG_ENABLE
      STA   MSPRITE_FLAGS_TMP

      LDY   MSPRITE_REG_OFF
      LDA   MSPRITE_X_TMP_L
      STA   VGC_SPR_BASE + VGC_SPR_XL_OFF,Y
      LDA   MSPRITE_X_TMP_H
      STA   VGC_SPR_BASE + VGC_SPR_XH_OFF,Y
      LDA   MSPRITE_Y_TMP
      STA   VGC_SPR_BASE + VGC_SPR_YL_OFF,Y
      LDA   #$00
      STA   VGC_SPR_BASE + VGC_SPR_YH_OFF,Y
      LDA   MSPRITE_SHAPE_TMP
      STA   VGC_SPR_BASE + VGC_SPR_SHAPE_OFF,Y
      LDA   MSPRITE_FLAGS_TMP
      STA   VGC_SPR_BASE + VGC_SPR_FLAGS_OFF,Y
      LDX   MSPRITE_HANDLE_TMP
      LDA   MSPRITE_OBJ_PRIORITY,X
      STA   VGC_SPR_BASE + VGC_SPR_PRI_OFF,Y
      LDA   MSPRITE_OBJ_TRANS,X
      STA   VGC_SPR_BASE + VGC_SPR_TRANS_OFF,Y

      LDA   MSPRITE_PART_PTR
      CLC
      ADC   #MSPRITE_VIS_PART_SIZE
      STA   MSPRITE_PART_PTR
      LDA   MSPRITE_PART_PTR+1
      ADC   #$00
      STA   MSPRITE_PART_PTR+1

      INC   MSPRITE_PART_INDEX
      LDA   MSPRITE_PART_INDEX
      CMP   MSPRITE_PART_COUNT_TMP
      BEQ   @finish
      JMP   @part_loop

@finish:
      LDX   MSPRITE_HANDLE_TMP
      LDA   MSPRITE_OBJ_FLAGS,X
      AND   #$DF
      STA   MSPRITE_OBJ_FLAGS,X
      RTS

msprite_ok:
      LDA   #MSPRITE_RESULT_OK
      STA   MSPRITE_RESULT
      RTS

msprite_bad_handle:
      LDA   #MSPRITE_RESULT_BAD_HANDLE
      STA   MSPRITE_RESULT
      RTS

msprite_fail:
      STA   MSPRITE_RESULT
      LDA   #MSPRITE_INVALID_HANDLE
      RTS

.endif

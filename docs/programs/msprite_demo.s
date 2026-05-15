; msprite_demo.s
;
; Hardware meta-sprite smoke demo.
;
; Build:
;   ca65 --cpu 65c02 -I ehbasic -o /tmp/msprite_demo.o docs/programs/msprite_demo.s
;   ld65 -C docs/programs/diag7000.cfg -o /tmp/msprite_demo.bin /tmp/msprite_demo.o
;
; Load at $7000 and run with: SYS $7000
;
; Visible behavior:
; - Six sprite objects share the same object API.
; - Objects include 16x32, 32x16, 32x32, and plain 16x16 visuals.
; - Shape slots are prebuilt once; animation changes only sprite shape indexes.

.setcpu "65c02"

.include "lib/msprite.inc"

OBJ_COUNT       = 6
SHAPE_COUNT     = 16
FRAME_COUNT     = 4
SCREEN_MIN_X    = 20
SCREEN_MIN_Y    = 20

SHAPE_TL_BASE   = 0
SHAPE_TR_BASE   = 4
SHAPE_BL_BASE   = 8
SHAPE_BR_BASE   = 12

TALL_TOP_BASE   = SHAPE_TL_BASE
TALL_BOT_BASE   = SHAPE_BL_BASE
WIDE_LEFT_BASE  = SHAPE_TL_BASE
WIDE_RIGHT_BASE = SHAPE_TR_BASE
BIG_TL_BASE     = SHAPE_TL_BASE
BIG_TR_BASE     = SHAPE_TR_BASE
BIG_BL_BASE     = SHAPE_BL_BASE
BIG_BR_BASE     = SHAPE_BR_BASE
PROBE_BASE      = SHAPE_TL_BASE

      .segment "BSS"

obj_handle:     .res OBJ_COUNT
obj_xl:         .res OBJ_COUNT
obj_xh:         .res OBJ_COUNT
obj_y:          .res OBJ_COUNT
obj_vx:         .res OBJ_COUNT
obj_vy:         .res OBJ_COUNT
obj_maxxl:      .res OBJ_COUNT
obj_maxxh:      .res OBJ_COUNT
obj_maxy:       .res OBJ_COUNT

shape_idx:      .res 1
shape_slot:     .res 1
row_idx:        .res 1
byte_col:       .res 1
fill_byte:      .res 1
border_byte:    .res 1
hilite_byte:    .res 1
shadow_byte:    .res 1
loop_idx:       .res 1
temp:           .res 1

      .segment "CODE"

start:
      STZ   VGC_CURSEN
      LDA   #$03
      STA   VGC_MODE
      LDA   #$06
      STA   VGC_BGCOL
      STA   VGC_BORDER

      JSR   wait_cmd
      LDA   #VCMD_COPPERDIS
      STA   VGC_CMD
      JSR   wait_cmd
      LDA   #VCMD_COPPERCLR
      STA   VGC_CMD

      JSR   msprite_init
      JSR   define_shapes
      JSR   spawn_objects
      JSR   wait_frame

main_loop:
      JSR   wait_frame
      JSR   msprite_tick

      STZ   loop_idx
@update_loop:
      LDX   loop_idx
      JSR   update_object
      INC   loop_idx
      LDA   loop_idx
      CMP   #OBJ_COUNT
      BNE   @update_loop

      JSR   msprite_commit
      BRA   main_loop

define_shapes:
      LDA   #$FF
      STA   hilite_byte
      LDA   #$BB
      STA   border_byte
      LDA   #$88
      STA   shadow_byte
      STZ   shape_idx
@shape_loop:
      LDX   shape_idx
      STX   shape_slot

      LDA   shape_fill,x
      TAX
      LDA   pair_table,x
      STA   fill_byte

      STZ   row_idx
@row_loop:
      JSR   write_shape_row
      INC   row_idx
      LDA   row_idx
      CMP   #16
      BNE   @row_loop

      INC   shape_idx
      LDA   shape_idx
      CMP   #SHAPE_COUNT
      BNE   @shape_loop
      RTS

write_shape_row:
      JSR   wait_cmd
      LDA   shape_slot
      STA   VGC_P0
      LDA   row_idx
      STA   VGC_P1

      LDX   #$00
@byte_loop:
      STX   byte_col
      JSR   choose_shape_byte
      LDX   byte_col
      STA   VGC_P2,x
      INX
      CPX   #$08
      BNE   @byte_loop

      JSR   wait_cmd
      LDA   #VCMD_SPRROW
      STA   VGC_CMD
      RTS

choose_shape_byte:
      LDA   row_idx
      BEQ   @border
      CMP   #$0F
      BEQ   @border

      LDA   byte_col
      BEQ   @border
      CMP   #$07
      BEQ   @border

      LDA   row_idx
      CMP   #$04
      BCS   @check_shadow
      CMP   #$01
      BCC   @check_shadow
      LDA   byte_col
      CMP   #$01
      BEQ   @hilite

@check_shadow:
      LDA   row_idx
      CMP   #$0D
      BCC   @fill
      LDA   byte_col
      CMP   #$06
      BEQ   @shadow

@fill:
      LDA   fill_byte
      RTS
@hilite:
      LDA   hilite_byte
      RTS
@shadow:
      LDA   shadow_byte
      RTS
@border:
      LDA   border_byte
      RTS

spawn_objects:
      STZ   loop_idx
@loop:
      LDX   loop_idx

      LDA   visual_l,x
      STA   MSPRITE_DESC_L
      LDA   visual_h,x
      STA   MSPRITE_DESC_H
      JSR   msprite_spawn
      CMP   #MSPRITE_INVALID_HANDLE
      BNE   @spawned
      JMP   spawn_failed

@spawned:
      LDX   loop_idx
      STA   obj_handle,x

      LDA   init_xl,x
      STA   obj_xl,x
      STA   NVR0L
      LDA   init_xh,x
      STA   obj_xh,x
      STA   NVR0H
      LDA   init_y,x
      STA   obj_y,x
      TAY
      LDA   init_vx,x
      STA   obj_vx,x
      LDA   init_vy,x
      STA   obj_vy,x
      LDA   init_maxxl,x
      STA   obj_maxxl,x
      LDA   init_maxxh,x
      STA   obj_maxxh,x
      LDA   init_maxy,x
      STA   obj_maxy,x

      LDA   obj_handle,x
      JSR   msprite_set_pos

      LDX   loop_idx
      LDA   anim_l,x
      STA   MSPRITE_ANIM_L
      LDA   anim_h,x
      STA   MSPRITE_ANIM_H
      LDA   obj_handle,x
      JSR   msprite_set_anim

      INC   loop_idx
      LDA   loop_idx
      CMP   #OBJ_COUNT
      BNE   @loop
      RTS

spawn_failed:
      LDA   #$02
      STA   VGC_BGCOL
      STA   VGC_BORDER
      BRA   spawn_failed

update_object:
      STX   loop_idx
      JSR   update_x
      JSR   update_y

      LDX   loop_idx
      LDA   obj_xl,x
      STA   NVR0L
      LDA   obj_xh,x
      STA   NVR0H
      LDY   obj_y,x
      LDA   obj_handle,x
      JMP   msprite_set_pos

update_x:
      LDX   loop_idx
      LDA   obj_vx,x
      BMI   @negative

      CLC
      ADC   obj_xl,x
      STA   obj_xl,x
      LDA   obj_xh,x
      ADC   #$00
      STA   obj_xh,x

      LDA   obj_xh,x
      CMP   obj_maxxh,x
      BCC   @done
      BEQ   @check_lo
      BRA   @over
@check_lo:
      LDA   obj_xl,x
      CMP   obj_maxxl,x
      BCC   @done
      BEQ   @done
@over:
      LDA   obj_maxxl,x
      STA   obj_xl,x
      LDA   obj_maxxh,x
      STA   obj_xh,x
      JSR   invert_vx
@done:
      RTS

@negative:
      EOR   #$FF
      INC
      STA   temp
      LDA   obj_xl,x
      SEC
      SBC   temp
      STA   obj_xl,x
      LDA   obj_xh,x
      SBC   #$00
      STA   obj_xh,x
      CMP   #$80
      BCS   @under
      BNE   @done
      LDA   obj_xl,x
      CMP   #SCREEN_MIN_X
      BCS   @done
@under:
      LDA   #SCREEN_MIN_X
      STA   obj_xl,x
      STZ   obj_xh,x
      JSR   invert_vx
      RTS

update_y:
      LDX   loop_idx
      LDA   obj_vy,x
      BMI   @negative

      CLC
      ADC   obj_y,x
      STA   obj_y,x
      LDA   obj_maxy,x
      CMP   obj_y,x
      BCS   @done
      STA   obj_y,x
      JSR   invert_vy
@done:
      RTS

@negative:
      EOR   #$FF
      INC
      STA   temp
      LDA   obj_y,x
      SEC
      SBC   temp
      STA   obj_y,x
      BCC   @under
      CMP   #SCREEN_MIN_Y
      BCS   @done
@under:
      LDA   #SCREEN_MIN_Y
      STA   obj_y,x
      JSR   invert_vy
      RTS

invert_vx:
      LDA   obj_vx,x
      EOR   #$FF
      INC
      STA   obj_vx,x
      RTS

invert_vy:
      LDA   obj_vy,x
      EOR   #$FF
      INC
      STA   obj_vy,x
      RTS

wait_frame:
      LDA   VGC_FRAME
:
      CMP   VGC_FRAME
      BEQ   :-
      RTS

wait_cmd:
      LDA   VGC_CMD
      AND   #$01
      BNE   wait_cmd
      RTS

      .segment "RODATA"

visual_l:
      .byte <tall_visual,<wide_visual,<big_visual,<probe_visual,<wide_visual,<tall_visual
visual_h:
      .byte >tall_visual,>wide_visual,>big_visual,>probe_visual,>wide_visual,>tall_visual

anim_l:
      .byte <anim_medium,<anim_fast,<anim_slow,<anim_fast,<anim_medium,<anim_slow
anim_h:
      .byte >anim_medium,>anim_fast,>anim_slow,>anim_fast,>anim_medium,>anim_slow

init_xl:
      .byte <40,<230,<125,<300,<70,<285
init_xh:
      .byte >40,>230,>125,>300,>70,>285
init_y:
      .byte 42,66,126,96,170,32
init_vx:
      .byte 2,$FE,1,$FD,3,$FF
init_vy:
      .byte 1,2,$FE,1,$FF,2
init_maxxl:
      .byte <324,<308,<308,<324,<308,<324
init_maxxh:
      .byte >324,>308,>308,>324,>308,>324
init_maxy:
      .byte 188,204,188,204,204,188

pair_table:
      .byte $00,$11,$22,$33,$44,$55,$66,$77
      .byte $88,$99,$AA,$BB,$CC,$DD,$EE,$FF

shape_fill:
      .byte 2,4,10,12
      .byte 3,5,13,7
      .byte 8,9,14,15
      .byte 4,11,12,1

tall_visual:
      .byte 2,0
      .byte 0,0,TALL_TOP_BASE,0
      .byte 0,16,TALL_BOT_BASE,0

wide_visual:
      .byte 2,0
      .byte 0,0,WIDE_LEFT_BASE,0
      .byte 16,0,WIDE_RIGHT_BASE,0

big_visual:
      .byte 4,0
      .byte 0,0,BIG_TL_BASE,0
      .byte 16,0,BIG_TR_BASE,0
      .byte 0,16,BIG_BL_BASE,0
      .byte 16,16,BIG_BR_BASE,0

probe_visual:
      .byte 1,0
      .byte 0,0,PROBE_BASE,0

anim_fast:
      .byte FRAME_COUNT,3,MSPRITE_ANIM_LOOP,0
anim_medium:
      .byte FRAME_COUNT,6,MSPRITE_ANIM_LOOP,0
anim_slow:
      .byte FRAME_COUNT,10,MSPRITE_ANIM_LOOP,0

      .include "lib/msprite.s"

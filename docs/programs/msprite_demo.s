; msprite_demo.s
;
; Hardware meta-sprite smoke demo.
;
; Build:
;   ca65 --cpu 65c02 -I runtime/asm -o /tmp/msprite_demo.o docs/programs/msprite_demo.s
;   ld65 -C docs/programs/diag7000.cfg -o /tmp/msprite_demo.bin /tmp/msprite_demo.o
;
; Load at $7000 and run with: SYS $7000
;
; Visible behavior:
; - Six sprite objects share the same object API.
; - Objects include 16x32, 32x16, 32x32, and plain 16x16 visuals.
; - Shape slots are prebuilt once; animation changes only sprite shape indexes.

.setcpu "65c02"

.include "anim.inc"
.include "math_copro.inc"
.include "math_fixed8.inc"

OBJ_COUNT       = 6
SHAPE_COUNT     = 16
FRAME_COUNT     = 4
SCREEN_MIN_X    = 20
SCREEN_MIN_Y    = 20
GRAVITY_LO      = $18       ; +0.09375 px/frame^2
GRAVITY_HI      = $00
REBOUND_LO      = $E6       ; Q8.8 0.8984375
REBOUND_HI      = $00

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
obj_xf:         .res OBJ_COUNT
obj_xl:         .res OBJ_COUNT
obj_xh:         .res OBJ_COUNT
obj_yf:         .res OBJ_COUNT
obj_yl:         .res OBJ_COUNT
obj_yh:         .res OBJ_COUNT
obj_vxf:        .res OBJ_COUNT
obj_vxi:        .res OBJ_COUNT
obj_vyf:        .res OBJ_COUNT
obj_vyi:        .res OBJ_COUNT
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
pair_a:         .res 1
pair_b:         .res 1
delta_x:        .res 1
delta_y:        .res 1
order_x:        .res 1
order_y:        .res 1
mag_lo:         .res 1
mag_hi:         .res 1
rad_sum:        .res 1
center_l:       .res 1
center_h:       .res 1

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
      JSR   anim_init
      JSR   define_shapes
      JSR   spawn_objects
      JSR   wait_frame

main_loop:
      JSR   wait_frame

      STZ   loop_idx
@update_loop:
      LDX   loop_idx
      JSR   update_object
      INC   loop_idx
      LDA   loop_idx
      CMP   #OBJ_COUNT
      BNE   @update_loop

      JSR   collide_objects
      JSR   anim_tick

      STZ   loop_idx
@publish_loop:
      LDX   loop_idx
      JSR   publish_object
      INC   loop_idx
      LDA   loop_idx
      CMP   #OBJ_COUNT
      BNE   @publish_loop

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

      STZ   obj_xf,x
      LDA   init_xl,x
      STA   obj_xl,x
      STA   NVR0L
      LDA   init_xh,x
      STA   obj_xh,x
      STA   NVR0H
      STZ   obj_yf,x
      LDA   init_y,x
      STA   obj_yl,x
      TAY
      STZ   obj_yh,x
      LDA   init_vxf,x
      STA   obj_vxf,x
      LDA   init_vxi,x
      STA   obj_vxi,x
      LDA   init_vyf,x
      STA   obj_vyf,x
      LDA   init_vyi,x
      STA   obj_vyi,x
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
      STA   ANIM_DESC_L
      LDA   anim_h,x
      STA   ANIM_DESC_H
      LDA   obj_handle,x
      STA   ANIM_TARGET
      LDA   #ANIM_TARGET_MSPRITE
      STA   ANIM_TARGET_TYPE
      JSR   anim_start
      CMP   #ANIM_INVALID_HANDLE
      BNE   @anim_ok
      JMP   spawn_failed
@anim_ok:

      INC   loop_idx
      LDA   loop_idx
      CMP   #OBJ_COUNT
      BEQ   @done
      JMP   @loop
@done:
      RTS

spawn_failed:
      LDA   #$02
      STA   VGC_BGCOL
      STA   VGC_BORDER
      BRA   spawn_failed

update_object:
      STX   loop_idx
      FIX8_ADD_CONST_X obj_vyf, obj_vyi, GRAVITY_LO, GRAVITY_HI
      FIX16_8_ADD_FIX8_X obj_xf, obj_xl, obj_xh, obj_vxf, obj_vxi
      FIX16_8_ADD_FIX8_X obj_yf, obj_yl, obj_yh, obj_vyf, obj_vyi
      JSR   collide_x
      JSR   collide_y
      RTS

publish_object:
      LDX   loop_idx
      LDA   obj_xl,x
      STA   NVR0L
      LDA   obj_xh,x
      STA   NVR0H
      LDY   obj_yl,x
      LDA   obj_handle,x
      JMP   msprite_set_pos

collide_x:
      LDX   loop_idx
      LDA   obj_xh,x
      BMI   @hit_left
      BNE   @check_right
      LDA   obj_xl,x
      CMP   #SCREEN_MIN_X
      BCC   @hit_left
      BNE   @check_right
      LDA   obj_xf,x
      BEQ   @check_right

@hit_left:
      STZ   obj_xf,x
      LDA   #SCREEN_MIN_X
      STA   obj_xl,x
      STZ   obj_xh,x
      JSR   obj_abs_vx_to_mag
      JSR   damp_mag
      JSR   ensure_min_vx
      LDA   mag_lo
      STA   obj_vxf,x
      LDA   mag_hi
      STA   obj_vxi,x
      RTS

@check_right:
      LDA   obj_xh,x
      CMP   obj_maxxh,x
      BCC   @done
      BNE   @hit_right
      LDA   obj_xl,x
      CMP   obj_maxxl,x
      BCC   @done
      BNE   @hit_right
      LDA   obj_xf,x
      BEQ   @done

@hit_right:
      STZ   obj_xf,x
      LDA   obj_maxxl,x
      STA   obj_xl,x
      LDA   obj_maxxh,x
      STA   obj_xh,x
      JSR   obj_abs_vx_to_mag
      JSR   damp_mag
      JSR   ensure_min_vx
      JSR   negate_mag
      LDA   mag_lo
      STA   obj_vxf,x
      LDA   mag_hi
      STA   obj_vxi,x
@done:
      RTS

collide_y:
      LDX   loop_idx
      LDA   obj_yh,x
      BMI   @hit_ceiling
      BNE   @hit_floor
      LDA   obj_yl,x
      CMP   #SCREEN_MIN_Y
      BCC   @hit_ceiling
      BNE   @check_floor
      LDA   obj_yf,x
      BEQ   @check_floor

@hit_ceiling:
      STZ   obj_yf,x
      LDA   #SCREEN_MIN_Y
      STA   obj_yl,x
      STZ   obj_yh,x
      JSR   obj_abs_vy_to_mag
      JSR   damp_mag
      LDA   mag_lo
      STA   obj_vyf,x
      LDA   mag_hi
      STA   obj_vyi,x
      RTS

@check_floor:
      LDA   obj_yl,x
      CMP   obj_maxy,x
      BCC   @done
      BNE   @hit_floor
      LDA   obj_yf,x
      BEQ   @done

@hit_floor:
      STZ   obj_yf,x
      LDA   obj_maxy,x
      STA   obj_yl,x
      STZ   obj_yh,x
      JSR   obj_abs_vy_to_mag
      JSR   damp_mag
      JSR   ensure_floor_vy
      JSR   negate_mag
      LDA   mag_lo
      STA   obj_vyf,x
      LDA   mag_hi
      STA   obj_vyi,x
@done:
      RTS

obj_abs_vx_to_mag:
      FIX8_ABS_TO_X obj_vxf, obj_vxi, mag_lo, mag_hi
      RTS

obj_abs_vy_to_mag:
      FIX8_ABS_TO_X obj_vyf, obj_vyi, mag_lo, mag_hi
      RTS

damp_mag:
      LDA   mag_lo
      STA   MATH_MULFX_A_LO
      LDA   mag_hi
      STA   MATH_MULFX_A_HI
      LDA   #REBOUND_LO
      STA   MATH_MULFX_B_LO
      LDA   #REBOUND_HI
      STA   MATH_MULFX_B_HI
      LDA   MATH_RES0
      STA   mag_lo
      LDA   MATH_RES1
      STA   mag_hi
      RTS

ensure_min_vx:
      LDA   mag_hi
      CMP   min_vx_int,x
      BCC   @copy
      BNE   @done
      LDA   mag_lo
      CMP   min_vx_frac,x
      BCS   @done
@copy:
      LDA   min_vx_frac,x
      STA   mag_lo
      LDA   min_vx_int,x
      STA   mag_hi
@done:
      RTS

ensure_floor_vy:
      LDA   mag_hi
      CMP   floor_vy_int,x
      BCC   @copy
      BNE   @done
      LDA   mag_lo
      CMP   floor_vy_frac,x
      BCS   @done
@copy:
      LDA   floor_vy_frac,x
      STA   mag_lo
      LDA   floor_vy_int,x
      STA   mag_hi
@done:
      RTS

negate_mag:
      FIX8_NEGATE_MEM mag_lo, mag_hi
      RTS

collide_objects:
      STZ   pair_a
@outer:
      LDA   pair_a
      CMP   #(OBJ_COUNT - 1)
      BCS   @done
      CLC
      ADC   #$01
      STA   pair_b
@inner:
      JSR   collide_pair
      INC   pair_b
      LDA   pair_b
      CMP   #OBJ_COUNT
      BNE   @inner
      INC   pair_a
      BRA   @outer
@done:
      RTS

collide_pair:
      JSR   pair_radius_sum
      JSR   pair_delta_x
      LDA   delta_x
      CMP   rad_sum
      BCS   @done
      JSR   pair_delta_y
      LDA   delta_y
      CMP   rad_sum
      BCS   @done

      MATHC_DIST_APPROX_U8 delta_x, delta_y, mag_lo, mag_hi
      LDA   mag_hi
      BNE   @done
      LDA   mag_lo
      CMP   rad_sum
      BCS   @done

      LDA   delta_x
      CMP   delta_y
      BCC   @vertical
      JSR   separate_pair_x
      JSR   swap_pair_vx
      RTS

@vertical:
      JSR   separate_pair_y
      JSR   swap_pair_vy
@done:
      RTS

pair_radius_sum:
      LDX   pair_a
      LDA   obj_radius,x
      LDX   pair_b
      CLC
      ADC   obj_radius,x
      STA   rad_sum
      RTS

pair_delta_x:
      LDX   pair_a
      LDA   obj_xl,x
      CLC
      ADC   obj_center_x,x
      STA   mag_lo
      LDA   obj_xh,x
      ADC   #$00
      STA   mag_hi

      LDX   pair_b
      LDA   obj_xl,x
      CLC
      ADC   obj_center_x,x
      STA   center_l
      LDA   obj_xh,x
      ADC   #$00
      STA   center_h

      LDA   center_h
      CMP   mag_hi
      BCC   @a_right
      BNE   @b_right
      LDA   center_l
      CMP   mag_lo
      BCC   @a_right

@b_right:
      STZ   order_x
      LDA   center_l
      SEC
      SBC   mag_lo
      STA   delta_x
      LDA   center_h
      SBC   mag_hi
      BEQ   @done
      LDA   #$FF
      STA   delta_x
      RTS

@a_right:
      LDA   #$01
      STA   order_x
      LDA   mag_lo
      SEC
      SBC   center_l
      STA   delta_x
      LDA   mag_hi
      SBC   center_h
      BEQ   @done
      LDA   #$FF
      STA   delta_x
@done:
      RTS

pair_delta_y:
      LDX   pair_a
      LDA   obj_yl,x
      CLC
      ADC   obj_center_y,x
      STA   mag_lo

      LDX   pair_b
      LDA   obj_yl,x
      CLC
      ADC   obj_center_y,x
      STA   center_l
      CMP   mag_lo
      BCC   @a_below

      STZ   order_y
      LDA   center_l
      SEC
      SBC   mag_lo
      STA   delta_y
      RTS

@a_below:
      LDA   #$01
      STA   order_y
      LDA   mag_lo
      SEC
      SBC   center_l
      STA   delta_y
      RTS

separate_pair_x:
      LDA   order_x
      BNE   @a_right

      LDX   pair_a
      LDA   obj_xl,x
      CLC
      ADC   obj_width,x
      STA   mag_lo
      LDA   obj_xh,x
      ADC   #$00
      STA   mag_hi
      LDX   pair_b
      BRA   @store

@a_right:
      LDX   pair_b
      LDA   obj_xl,x
      CLC
      ADC   obj_width,x
      STA   mag_lo
      LDA   obj_xh,x
      ADC   #$00
      STA   mag_hi
      LDX   pair_a

@store:
      STZ   obj_xf,x
      LDA   mag_lo
      STA   obj_xl,x
      LDA   mag_hi
      STA   obj_xh,x
      STX   loop_idx
      JSR   collide_x
      RTS

separate_pair_y:
      LDA   order_y
      BNE   @a_below

      LDX   pair_a
      LDA   obj_yl,x
      CLC
      ADC   obj_height,x
      STA   mag_lo
      LDX   pair_b
      BRA   @store

@a_below:
      LDX   pair_b
      LDA   obj_yl,x
      CLC
      ADC   obj_height,x
      STA   mag_lo
      LDX   pair_a

@store:
      STZ   obj_yf,x
      LDA   mag_lo
      STA   obj_yl,x
      STZ   obj_yh,x
      STX   loop_idx
      JSR   collide_y
      RTS

swap_pair_vx:
      LDX   pair_a
      LDA   obj_vxf,x
      STA   mag_lo
      LDA   obj_vxi,x
      STA   mag_hi

      LDX   pair_b
      LDA   obj_vxf,x
      STA   delta_x
      LDA   obj_vxi,x
      STA   delta_y
      LDA   mag_lo
      STA   obj_vxf,x
      LDA   mag_hi
      STA   obj_vxi,x

      LDX   pair_a
      LDA   delta_x
      STA   obj_vxf,x
      LDA   delta_y
      STA   obj_vxi,x
      RTS

swap_pair_vy:
      LDX   pair_a
      LDA   obj_vyf,x
      STA   mag_lo
      LDA   obj_vyi,x
      STA   mag_hi

      LDX   pair_b
      LDA   obj_vyf,x
      STA   delta_x
      LDA   obj_vyi,x
      STA   delta_y
      LDA   mag_lo
      STA   obj_vyf,x
      LDA   mag_hi
      STA   obj_vyi,x

      LDX   pair_a
      LDA   delta_x
      STA   obj_vyf,x
      LDA   delta_y
      STA   obj_vyi,x
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
init_vxf:
      .byte $80,$40,$C0,$80,$20,$A0
init_vxi:
      .byte $01,$FE,$00,$FD,$02,$FE
init_vyf:
      .byte $40,$C0,$80,$20,$00,$A0
init_vyi:
      .byte $FF,$FE,$FD,$FE,$FF,$FD
init_maxxl:
      .byte <324,<308,<308,<324,<308,<324
init_maxxh:
      .byte >324,>308,>308,>324,>308,>324
init_maxy:
      .byte 188,204,188,204,204,188
obj_width:
      .byte 16,32,32,16,32,16
obj_height:
      .byte 32,16,32,16,16,32
obj_center_x:
      .byte 8,16,16,8,16,8
obj_center_y:
      .byte 16,8,16,8,8,16
obj_radius:
      .byte 14,14,22,8,14,14
min_vx_frac:
      .byte $40,$80,$40,$80,$40,$80
min_vx_int:
      .byte $01,$01,$01,$01,$01,$01
floor_vy_frac:
      .byte $80,$00,$80,$00,$40,$C0
floor_vy_int:
      .byte $03,$03,$04,$02,$03,$03

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
      .byte FRAME_COUNT,3,ANIM_DESC_LOOP | ANIM_DESC_DEFER_COMMIT,0,1,0
anim_medium:
      .byte FRAME_COUNT,6,ANIM_DESC_LOOP | ANIM_DESC_DEFER_COMMIT,0,1,0
anim_slow:
      .byte FRAME_COUNT,10,ANIM_DESC_LOOP | ANIM_DESC_DEFER_COMMIT,0,1,0

      .include "anim.s"

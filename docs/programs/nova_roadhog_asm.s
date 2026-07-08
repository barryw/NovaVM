; nova_roadhog_asm.s
;
; Nova-native driving demo inspired by early 8-bit road games.
;
; This is not a line-by-line port of any existing program. It keeps the
; simple "survive a winding road" idea and rebuilds it around Nova hardware:
; - VGC bitmap fills draw a fast generated road.
; - The player car is a 16x32 hardware meta-sprite.
; - Traffic and fuel pickups are independent hardware sprites.
; - The math coprocessor supplies the road curve and spawn randomness.
; - The shared audio runtime plays event sounds.
;
; Build:
;   ca65 --cpu w65c02 -I runtime/asm -o /tmp/nova_roadhog_asm.o docs/programs/nova_roadhog_asm.s
;   ld65 -C docs/programs/diag7000.cfg -o /tmp/nova_roadhog_asm.bin /tmp/nova_roadhog_asm.o
;
; Load at $7000 and run with: SYS $7000
;
; Controls:
;   A/D or 4/6 steer
;   W/S or 8/2 speed up / slow down
;   Q resets the run

.setcpu "w65c02"

.include "msprite.inc"
.include "audio.inc"
.include "math_copro.inc"

OBJ_COUNT           = 8
TRAFFIC_FIRST_SPR   = 2
SHAPE_COUNT         = 5
BAND_COUNT          = 14
SIM_FRAME_DELAY     = 2

PLAYER_Y            = 162
PLAYER_MIN_X        = 48
PLAYER_MAX_X        = 248
PLAYER_START_X      = 152
STEER_STEP          = 3

FUEL_MAX            = 100
FUEL_WARN           = 25
SPEED_MIN           = 1
SPEED_MAX           = 4

OBJ_NONE            = 0
OBJ_CAR             = 1
OBJ_TRUCK           = 2
OBJ_FUEL            = 3

SHAPE_PLAYER_TOP    = 0
SHAPE_PLAYER_BOTTOM = 1
SHAPE_TRAFFIC       = 2
SHAPE_TRUCK         = 3
SHAPE_FUEL          = 4

COLOR_SKY           = 9
COLOR_GRASS         = 5
COLOR_ROAD          = 8
COLOR_ROAD_DARK     = 11
COLOR_LINE          = 14
COLOR_HUD           = 0
COLOR_HUD_DIM       = 1
COLOR_FUEL          = 10
COLOR_FUEL_LOW      = 2
COLOR_WHITE         = 15
COLOR_CRASH         = 2

      .segment "BSS"

player_handle:      .res 1
player_x:           .res 1
fuel:               .res 1
speed:              .res 1
road_phase:         .res 1
road_curve:         .res 1
road_target:        .res 1
road_change_timer:  .res 1
frame_tick:         .res 1
spawn_timer:        .res 1
crash_timer:        .res 1

obj_active:         .res OBJ_COUNT
obj_x:              .res OBJ_COUNT
obj_y:              .res OBJ_COUNT
obj_draw_x:         .res OBJ_COUNT
obj_draw_y:         .res OBJ_COUNT
obj_lane:           .res OBJ_COUNT
obj_type:           .res OBJ_COUNT
obj_speed:          .res OBJ_COUNT

shape_slot:         .res 1
row_idx:            .res 1
byte_col:           .res 1
fill_pair:          .res 1
accent_pair:        .res 1
shadow_pair:        .res 1

band_idx:           .res 1
road_center:        .res 1
temp0:              .res 1
temp1:              .res 1
temp2:              .res 1
obj_index:          .res 1
spr_offset:         .res 1

rect_x0_l:          .res 1
rect_x0_h:          .res 1
rect_y0:            .res 1
rect_x1_l:          .res 1
rect_x1_h:          .res 1
rect_y1:            .res 1

      .segment "CODE"

start:
      STZ   VGC_CURSEN
      STZ   VGC_CURSX
      STZ   VGC_CURSY
      LDA   #COLOR_SKY
      STA   VGC_BGCOL
      LDA   #COLOR_WHITE
      STA   VGC_FGCOL
      LDA   #$0C
      STA   VGC_CHAROUT
      LDA   #$03
      STA   VGC_MODE
      LDA   #COLOR_HUD_DIM
      STA   VGC_BORDER
      LDA   #$0F
      STA   VGC_DIMMER

      JSR   wait_cmd
      LDA   #VCMD_COPPERDIS
      STA   VGC_CMD
      JSR   wait_cmd
      LDA   #VCMD_COPPERCLR
      STA   VGC_CMD

      JSR   msprite_init
      JSR   define_shapes
      JSR   spawn_player
      JSR   reset_run
      JSR   wait_frame

main_loop:
      JSR   wait_game_frame
      JSR   read_controls

      LDA   crash_timer
      BEQ   @live
      JSR   crash_frame
      BRA   main_loop

@live:
      JSR   update_world
      JSR   update_objects
      JSR   check_collisions
      JSR   draw_scene
      JSR   publish_player
      JSR   msprite_commit
      BRA   main_loop

reset_run:
      LDA   #FUEL_MAX
      STA   fuel
      LDA   #$02
      STA   speed
      STZ   road_phase
      STZ   road_curve
      STZ   road_target
      STZ   frame_tick
      LDA   #$10
      STA   road_change_timer
      LDA   #$28
      STA   spawn_timer
      STZ   crash_timer
      LDA   #PLAYER_START_X
      STA   player_x
      JSR   clear_objects
      JSR   publish_player
      JSR   msprite_commit
      RTS

spawn_player:
      LDA   #<player_visual
      STA   MSPRITE_DESC_L
      LDA   #>player_visual
      STA   MSPRITE_DESC_H
      JSR   msprite_spawn
      CMP   #MSPRITE_INVALID_HANDLE
      BNE   @ok
@failed:
      LDA   #COLOR_CRASH
      STA   VGC_BGCOL
      BRA   @failed
@ok:
      STA   player_handle
      RTS

read_controls:
      LDA   VGC_CHARIN
      BEQ   @done

      CMP   #'a'
      BEQ   steer_left
      CMP   #'A'
      BEQ   steer_left
      CMP   #'4'
      BEQ   steer_left

      CMP   #'d'
      BEQ   steer_right
      CMP   #'D'
      BEQ   steer_right
      CMP   #'6'
      BEQ   steer_right

      CMP   #'w'
      BEQ   speed_up
      CMP   #'W'
      BEQ   speed_up
      CMP   #'8'
      BEQ   speed_up

      CMP   #'s'
      BEQ   slow_down
      CMP   #'S'
      BEQ   slow_down
      CMP   #'2'
      BEQ   slow_down

      CMP   #'q'
      BEQ   @reset
      CMP   #'Q'
      BNE   @done
@reset:
      JMP   reset_run
@done:
      RTS

steer_left:
      LDA   player_x
      CMP   #(PLAYER_MIN_X + STEER_STEP)
      BCS   @move
      LDA   #PLAYER_MIN_X
      STA   player_x
      RTS
@move:
      SEC
      SBC   #STEER_STEP
      STA   player_x
      RTS

steer_right:
      LDA   player_x
      CMP   #(PLAYER_MAX_X - STEER_STEP)
      BCC   @move
      LDA   #PLAYER_MAX_X
      STA   player_x
      RTS
@move:
      CLC
      ADC   #STEER_STEP
      STA   player_x
      RTS

speed_up:
      LDA   speed
      CMP   #SPEED_MAX
      BCS   @done
      INC   speed
@done:
      RTS

slow_down:
      LDA   speed
      CMP   #SPEED_MIN
      BEQ   @done
      DEC   speed
@done:
      RTS

update_world:
      LDA   road_phase
      CLC
      ADC   speed
      STA   road_phase
      JSR   update_random_road

      INC   frame_tick
      LDA   frame_tick
      AND   #$0F
      BNE   @spawn
      LDA   fuel
      BEQ   @empty
      DEC   fuel
      BNE   @spawn
@empty:
      JMP   start_crash

@spawn:
      LDA   spawn_timer
      BEQ   @do_spawn
      DEC   spawn_timer
      RTS
@do_spawn:
      JSR   spawn_object
      LDX   speed
      LDA   spawn_delay_by_speed,X
      STA   spawn_timer
      RTS

clear_objects:
      LDX   #$00
@loop:
      STZ   obj_active,X
      TXA
      CLC
      ADC   #TRAFFIC_FIRST_SPR
      JSR   disable_sprite_a
      INX
      CPX   #OBJ_COUNT
      BNE   @loop
      RTS

spawn_object:
      LDX   #$00
@find:
      LDA   obj_active,X
      BEQ   @slot
      INX
      CPX   #OBJ_COUNT
      BNE   @find
      RTS

@slot:
      STX   obj_index
      LDA   #$01
      STA   obj_active,X
      LDA   #$1C
      STA   obj_y,X
      STA   obj_draw_y,X

      MATHC_RNG_READ temp0
      LDA   temp0
      AND   #$03
      TAY
      LDA   lane_table,Y
      LDX   obj_index
      STA   obj_lane,X
      LDA   obj_y,X
      JSR   compute_road_center_for_y
      LDX   obj_index
      LDA   road_center
      CLC
      ADC   obj_lane,X
      STA   obj_x,X
      STA   obj_draw_x,X

      MATHC_RNG_READ temp0
      LDA   temp0
      AND   #$07
      CMP   #$01
      BNE   @not_fuel
      LDA   #OBJ_FUEL
      BRA   @type_ok
@not_fuel:
      CMP   #$04
      BCC   @car
      LDA   #OBJ_TRUCK
      BRA   @type_ok
@car:
      LDA   #OBJ_CAR
@type_ok:
      LDX   obj_index
      STA   obj_type,X

      MATHC_RNG_READ temp0
      LDA   temp0
      AND   #$01
      CLC
      ADC   speed
      STA   obj_speed,X
      RTS

update_objects:
      STZ   obj_index
@loop:
      LDX   obj_index
      LDA   obj_active,X
      BEQ   @next

      LDA   obj_y,X
      CLC
      ADC   obj_speed,X
      STA   obj_y,X
      CMP   #$C8
      BCC   @position
      STZ   obj_active,X
      TXA
      CLC
      ADC   #TRAFFIC_FIRST_SPR
      JSR   disable_sprite_a
      BRA   @next

@position:
      LDA   obj_y,X
      JSR   compute_road_center_for_y
      LDX   obj_index
      LDA   road_center
      CLC
      ADC   obj_lane,X
      STA   obj_x,X
      JSR   smooth_object_motion
      JSR   publish_object

@next:
      INC   obj_index
      LDA   obj_index
      CMP   #OBJ_COUNT
      BNE   @loop
      RTS

publish_player:
      LDA   player_x
      STA   NVR0L
      STZ   NVR0H
      LDY   #PLAYER_Y
      LDA   player_handle
      JMP   msprite_set_pos

publish_object:
      LDX   obj_index
      TXA
      CLC
      ADC   #TRAFFIC_FIRST_SPR
      ASL
      ASL
      ASL
      STA   spr_offset
      TAY

      LDA   obj_draw_x,X
      STA   VGC_SPR_BASE + VGC_SPR_XL_OFF,Y
      LDA   #$00
      STA   VGC_SPR_BASE + VGC_SPR_XH_OFF,Y
      LDA   obj_draw_y,X
      STA   VGC_SPR_BASE + VGC_SPR_YL_OFF,Y
      LDA   #$00
      STA   VGC_SPR_BASE + VGC_SPR_YH_OFF,Y

      LDA   obj_type,X
      CMP   #OBJ_FUEL
      BEQ   @fuel
      CMP   #OBJ_TRUCK
      BEQ   @truck
      LDA   #SHAPE_TRAFFIC
      BRA   @shape
@truck:
      LDA   #SHAPE_TRUCK
      BRA   @shape
@fuel:
      LDA   #SHAPE_FUEL
@shape:
      LDY   spr_offset
      STA   VGC_SPR_BASE + VGC_SPR_SHAPE_OFF,Y
      LDA   #VGC_SPR_PRI_FRONT
      STA   VGC_SPR_BASE + VGC_SPR_PRI_OFF,Y
      LDA   #$00
      STA   VGC_SPR_BASE + VGC_SPR_TRANS_OFF,Y
      LDA   #VGC_SPR_FLAG_ENABLE
      STA   VGC_SPR_BASE + VGC_SPR_FLAGS_OFF,Y
      RTS

smooth_object_motion:
      LDX   obj_index
      LDA   obj_x,X
      STA   temp0
      LDA   obj_draw_x,X
      STA   temp1
      JSR   ease_quarter
      LDX   obj_index
      STA   obj_draw_x,X

      LDA   obj_y,X
      STA   temp0
      LDA   obj_draw_y,X
      STA   temp1
      JSR   ease_half
      LDX   obj_index
      STA   obj_draw_y,X
      RTS

ease_quarter:
      LDA   temp0
      CMP   temp1
      BEQ   @same
      BCS   @increase

      LDA   temp1
      SEC
      SBC   temp0
      LSR
      LSR
      BNE   @dec_step
      LDA   #$01
@dec_step:
      STA   temp2
      LDA   temp1
      SEC
      SBC   temp2
      CMP   temp0
      BCS   @done
      LDA   temp0
@done:
      RTS

@increase:
      LDA   temp0
      SEC
      SBC   temp1
      LSR
      LSR
      BNE   @inc_step
      LDA   #$01
@inc_step:
      STA   temp2
      LDA   temp1
      CLC
      ADC   temp2
      CMP   temp0
      BCC   @done
      LDA   temp0
      RTS

@same:
      LDA   temp1
      RTS

ease_half:
      LDA   temp0
      CMP   temp1
      BEQ   @same
      BCS   @increase

      LDA   temp1
      SEC
      SBC   temp0
      LSR
      BNE   @dec_step
      LDA   #$01
@dec_step:
      STA   temp2
      LDA   temp1
      SEC
      SBC   temp2
      CMP   temp0
      BCS   @done
      LDA   temp0
@done:
      RTS

@increase:
      LDA   temp0
      SEC
      SBC   temp1
      LSR
      BNE   @inc_step
      LDA   #$01
@inc_step:
      STA   temp2
      LDA   temp1
      CLC
      ADC   temp2
      CMP   temp0
      BCC   @done
      LDA   temp0
      RTS

@same:
      LDA   temp1
      RTS

disable_sprite_a:
      ASL
      ASL
      ASL
      TAY
      LDA   #$00
      STA   VGC_SPR_BASE + VGC_SPR_FLAGS_OFF,Y
      RTS

check_collisions:
      STZ   obj_index
@loop:
      LDX   obj_index
      LDA   obj_active,X
      BEQ   @next

      LDA   obj_draw_y,X
      CMP   #(PLAYER_Y - 12)
      BCC   @next
      CMP   #(PLAYER_Y + 30)
      BCS   @next

      LDA   obj_draw_x,X
      SEC
      SBC   player_x
      BCS   @abs_ok
      EOR   #$FF
      CLC
      ADC   #$01
@abs_ok:
      CMP   #$12
      BCS   @next

      LDA   obj_type,X
      CMP   #OBJ_FUEL
      BEQ   @pickup
      JMP   start_crash

@pickup:
      STZ   obj_active,X
      TXA
      CLC
      ADC   #TRAFFIC_FIRST_SPR
      JSR   disable_sprite_a
      LDA   fuel
      CLC
      ADC   #$1E
      CMP   #FUEL_MAX
      BCC   @fuel_ok
      LDA   #FUEL_MAX
@fuel_ok:
      STA   fuel
      JSR   play_pickup_sound

@next:
      INC   obj_index
      LDA   obj_index
      CMP   #OBJ_COUNT
      BNE   @loop
      RTS

start_crash:
      LDA   crash_timer
      BNE   @done
      LDA   #$28
      STA   crash_timer
      JSR   play_crash_sound
@done:
      RTS

crash_frame:
      JSR   draw_scene
      LDA   crash_timer
      AND   #$02
      BEQ   @dark
      LDA   #COLOR_CRASH
      STA   VGC_BORDER
      BRA   @tick
@dark:
      LDA   #COLOR_HUD_DIM
      STA   VGC_BORDER
@tick:
      JSR   publish_player
      JSR   msprite_commit
      DEC   crash_timer
      BNE   @done
      JSR   reset_run
@done:
      RTS

draw_scene:
      STZ   VGC_CURSEN
      LDA   #$03
      STA   VGC_MODE
      JSR   wait_cmd
      LDA   #VCMD_GCLS
      STA   VGC_CMD
      JSR   wait_cmd

      LDA   crash_timer
      BEQ   @normal_bg
      LDA   #COLOR_CRASH
      BRA   @sky
@normal_bg:
      LDA   #COLOR_SKY
@sky:
      JSR   set_gcolor
      STZ   rect_x0_l
      STZ   rect_x0_h
      STZ   rect_y0
      LDA   #<319
      STA   rect_x1_l
      LDA   #>319
      STA   rect_x1_h
      LDA   #$1F
      STA   rect_y1
      JSR   fill_rect

      LDA   #COLOR_GRASS
      JSR   set_gcolor
      STZ   rect_x0_l
      STZ   rect_x0_h
      LDA   #$20
      STA   rect_y0
      LDA   #<319
      STA   rect_x1_l
      LDA   #>319
      STA   rect_x1_h
      LDA   #199
      STA   rect_y1
      JSR   fill_rect

      STZ   band_idx
@band_loop:
      LDX   band_idx
      LDA   band_curve_shift,X
      JSR   compute_road_center
      JSR   draw_road_band

      INC   band_idx
      LDA   band_idx
      CMP   #BAND_COUNT
      BNE   @band_loop

      JSR   draw_hud
      RTS

draw_road_band:
      LDX   band_idx
      LDA   band_half_width,X
      STA   temp0

      LDA   road_center
      SEC
      SBC   temp0
      STA   rect_x0_l
      STZ   rect_x0_h
      LDA   road_center
      CLC
      ADC   temp0
      STA   rect_x1_l
      LDA   #$00
      ADC   #$00
      STA   rect_x1_h

      LDA   band_y0,X
      STA   rect_y0
      LDA   band_y1,X
      STA   rect_y1
      LDA   #COLOR_ROAD
      JSR   set_gcolor
      JSR   fill_rect

      JSR   draw_road_edges
      JSR   maybe_draw_lane_marker
      RTS

draw_road_edges:
      LDA   #COLOR_ROAD_DARK
      JSR   set_gcolor
      LDA   rect_x1_l
      STA   temp1
      LDA   rect_x1_h
      STA   temp2

      LDA   rect_x0_l
      CLC
      ADC   #$01
      STA   rect_x1_l
      LDA   rect_x0_h
      ADC   #$00
      STA   rect_x1_h
      JSR   fill_rect

      LDA   temp1
      STA   rect_x1_l
      LDA   temp2
      STA   rect_x1_h
      LDA   temp1
      SEC
      SBC   #$01
      STA   rect_x0_l
      LDA   temp2
      SBC   #$00
      STA   rect_x0_h
      JSR   fill_rect
      RTS

maybe_draw_lane_marker:
      LDA   band_idx
      CLC
      ADC   band_idx
      ADC   band_idx
      SEC
      SBC   road_phase
      AND   #$18
      BNE   @done

      LDX   band_idx
      LDA   road_center
      SEC
      SBC   #$02
      STA   rect_x0_l
      STZ   rect_x0_h
      LDA   road_center
      CLC
      ADC   #$02
      STA   rect_x1_l
      STZ   rect_x1_h
      LDA   band_y0,X
      CLC
      ADC   #$02
      STA   rect_y0
      LDA   band_y1,X
      SEC
      SBC   #$02
      STA   rect_y1
      LDA   #COLOR_LINE
      JSR   set_gcolor
      JSR   fill_rect
@done:
      RTS

draw_hud:
      LDA   #COLOR_HUD
      JSR   set_gcolor
      STZ   rect_x0_l
      STZ   rect_x0_h
      STZ   rect_y0
      LDA   #<319
      STA   rect_x1_l
      LDA   #>319
      STA   rect_x1_h
      LDA   #$0F
      STA   rect_y1
      JSR   fill_rect

      LDA   fuel
      CMP   #FUEL_WARN
      BCS   @fuel_color
      LDA   #COLOR_FUEL_LOW
      BRA   @fuel_draw
@fuel_color:
      LDA   #COLOR_FUEL
@fuel_draw:
      JSR   set_gcolor
      LDA   #$0A
      STA   rect_x0_l
      STZ   rect_x0_h
      LDA   #$05
      STA   rect_y0
      LDA   fuel
      CLC
      ADC   #$0A
      STA   rect_x1_l
      STZ   rect_x1_h
      LDA   #$0B
      STA   rect_y1
      JSR   fill_rect

      LDA   #COLOR_WHITE
      JSR   set_gcolor
      LDA   #$DC
      STA   rect_x0_l
      STZ   rect_x0_h
      LDA   #$05
      STA   rect_y0
      LDA   speed
      ASL
      ASL
      ASL
      CLC
      ADC   #$DC
      STA   rect_x1_l
      LDA   #$00
      ADC   #$00
      STA   rect_x1_h
      LDA   #$0B
      STA   rect_y1
      JSR   fill_rect
      RTS

update_random_road:
      LDA   road_change_timer
      BEQ   @new_target
      DEC   road_change_timer
      BRA   @ease

@new_target:
      MATHC_RNG_READ temp0
      LDA   temp0
      AND   #$07
      CMP   #$02
      BCC   @straight
      CMP   #$05
      BCC   @left_curve

@right_curve:
      JSR   random_curve_magnitude
      STA   road_target
      BRA   @new_timer

@left_curve:
      JSR   random_curve_magnitude
      EOR   #$FF
      CLC
      ADC   #$01
      STA   road_target
      BRA   @new_timer

@straight:
      STZ   road_target

@new_timer:
      MATHC_RNG_READ temp0
      LDA   temp0
      AND   #$1F
      CLC
      ADC   #$30
      STA   road_change_timer

@ease:
      LDA   road_curve
      EOR   #$80
      STA   temp1
      LDA   road_target
      EOR   #$80
      CMP   temp1
      BEQ   @done
      BCS   @increase
      DEC   road_curve
      RTS
@increase:
      INC   road_curve
@done:
      RTS

random_curve_magnitude:
      MATHC_RNG_READ temp0
      LDA   temp0
      AND   #$1F
      CLC
      ADC   #$28
      RTS

compute_road_center_for_y:
      CMP   #$40
      BCC   @far
      CMP   #$68
      BCC   @mid_far
      CMP   #$90
      BCC   @mid_near
      CMP   #$B0
      BCC   @near
      LDA   #$04
      BRA   compute_road_center
@far:
      LDA   #$00
      BRA   compute_road_center
@mid_far:
      LDA   #$01
      BRA   compute_road_center
@mid_near:
      LDA   #$02
      BRA   compute_road_center
@near:
      LDA   #$03

compute_road_center:
      TAX
      LDA   road_curve
@shift:
      CPX   #$00
      BEQ   @add_center
      CMP   #$80
      ROR
      DEX
      BRA   @shift
@add_center:
      CLC
      ADC   #160
      STA   road_center
      RTS

set_gcolor:
      STA   VGC_P0
      JSR   wait_cmd
      LDA   #VCMD_GCOLOR
      STA   VGC_CMD
      JSR   wait_cmd
      RTS

fill_rect:
      LDA   rect_x0_l
      STA   VGC_P0
      LDA   rect_x0_h
      STA   VGC_P1
      LDA   rect_y0
      STA   VGC_P2
      STZ   VGC_P3
      LDA   rect_x1_l
      STA   VGC_P4
      LDA   rect_x1_h
      STA   VGC_P5
      LDA   rect_y1
      STA   VGC_P6
      STZ   VGC_P7
      JSR   wait_cmd
      LDA   #VCMD_FILL
      STA   VGC_CMD
      JSR   wait_cmd
      RTS

play_pickup_sound:
      LDA   #$4F
      LDX   #$05
      LDY   #$00
      JMP   audio_play_sound_async

play_crash_sound:
      LDA   #$24
      LDX   #$18
      LDY   #$00
      JMP   audio_play_sound_async

define_shapes:
      STZ   shape_slot
@shape_loop:
      STZ   row_idx
@row_loop:
      JSR   write_shape_row
      INC   row_idx
      LDA   row_idx
      CMP   #$10
      BNE   @row_loop

      INC   shape_slot
      LDA   shape_slot
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
      STA   VGC_P2,X
      INX
      CPX   #$08
      BNE   @byte_loop

      JSR   wait_cmd
      LDA   #VCMD_SPRROW
      STA   VGC_CMD
      RTS

choose_shape_byte:
      LDY   shape_slot
      LDA   shape_fill_pairs,Y
      STA   fill_pair
      LDA   shape_accent_pairs,Y
      STA   accent_pair
      LDA   #$11
      STA   shadow_pair

      LDA   shape_slot
      CMP   #SHAPE_PLAYER_TOP
      BNE   @not_player_top
      JMP   choose_player_top_byte
@not_player_top:
      CMP   #SHAPE_PLAYER_BOTTOM
      BNE   @not_player_bottom
      JMP   choose_player_bottom_byte
@not_player_bottom:
      LDA   shape_slot
      CMP   #SHAPE_FUEL
      BNE   @not_fuel
      JMP   choose_fuel_byte
@not_fuel:

      LDA   row_idx
      BEQ   @transparent
      CMP   #$0F
      BEQ   @transparent
      LDA   byte_col
      BEQ   @transparent
      CMP   #$07
      BEQ   @transparent

      LDA   row_idx
      CMP   #$04
      BCS   @body
      LDA   byte_col
      CMP   #$03
      BEQ   @accent
      CMP   #$04
      BEQ   @accent
      CMP   #$02
      BCC   @transparent
      CMP   #$06
      BCS   @transparent
      BRA   @fill

@body:
      LDA   byte_col
      CMP   #$01
      BEQ   @shadow
      CMP   #$06
      BEQ   @shadow
@fill:
      LDA   fill_pair
      RTS
@accent:
      LDA   accent_pair
      RTS
@shadow:
      LDA   shadow_pair
      RTS
@transparent:
      LDA   #$00
      RTS

choose_player_top_byte:
      LDA   row_idx
      CMP   #$02
      BCC   @nose
      CMP   #$05
      BCC   @hood
      CMP   #$09
      BCC   @glass_band
      BRA   @body

@nose:
      LDA   byte_col
      CMP   #$03
      BEQ   @accent
      CMP   #$04
      BEQ   @fill
      BRA   @transparent

@hood:
      LDA   byte_col
      CMP   #$02
      BCC   @transparent
      CMP   #$06
      BCS   @transparent
      CMP   #$03
      BEQ   @accent
      BRA   @fill

@glass_band:
      LDA   byte_col
      CMP   #$02
      BCC   @transparent
      CMP   #$06
      BCS   @transparent
      CMP   #$03
      BCC   @fill
      CMP   #$05
      BCC   @glass
      BRA   @fill

@body:
      LDA   byte_col
      BEQ   @transparent
      CMP   #$07
      BEQ   @transparent
      CMP   #$01
      BEQ   @shadow
      CMP   #$06
      BEQ   @shadow
      LDA   row_idx
      CMP   #$0D
      BCC   @fill
      LDA   byte_col
      CMP   #$03
      BEQ   @accent
      CMP   #$04
      BEQ   @accent
      BRA   @fill

@glass:
      LDA   #$99
      RTS
@fill:
      LDA   fill_pair
      RTS
@accent:
      LDA   accent_pair
      RTS
@shadow:
      LDA   shadow_pair
      RTS
@transparent:
      LDA   #$00
      RTS

choose_player_bottom_byte:
      LDA   byte_col
      BEQ   @transparent
      CMP   #$07
      BEQ   @transparent

      LDA   row_idx
      CMP   #$08
      BCC   @body
      CMP   #$0D
      BCC   @rear
      BRA   @bumper

@body:
      LDA   byte_col
      CMP   #$01
      BEQ   @shadow
      CMP   #$06
      BEQ   @shadow
      BRA   @fill

@rear:
      LDA   byte_col
      CMP   #$01
      BEQ   @shadow
      CMP   #$06
      BEQ   @shadow
      CMP   #$02
      BCC   @transparent
      CMP   #$06
      BCS   @transparent
      BRA   @fill

@bumper:
      LDA   byte_col
      CMP   #$02
      BCC   @shadow
      CMP   #$06
      BCS   @shadow
      LDA   row_idx
      CMP   #$0F
      BEQ   @shadow
      BRA   @fill

@fill:
      LDA   fill_pair
      RTS
@shadow:
      LDA   shadow_pair
      RTS
@transparent:
      LDA   #$00
      RTS

choose_fuel_byte:
      LDA   row_idx
      CMP   #$02
      BCC   @transparent
      CMP   #$0E
      BCS   @transparent
      LDA   byte_col
      CMP   #$02
      BCC   @transparent
      CMP   #$06
      BCS   @transparent
      LDA   row_idx
      CMP   #$03
      BEQ   @accent_check
      CMP   #$08
      BEQ   @accent_check
      BRA   @fill
@accent_check:
      LDA   byte_col
      CMP   #$03
      BEQ   @accent
      CMP   #$04
      BEQ   @accent
@fill:
      LDA   fill_pair
      RTS
@accent:
      LDA   accent_pair
      RTS
@transparent:
      LDA   #$00
      RTS

wait_frame:
      LDA   VGC_FRAME
:
      CMP   VGC_FRAME
      BEQ   :-
      RTS

wait_game_frame:
      LDY   #SIM_FRAME_DELAY
@loop:
      JSR   wait_frame
      DEY
      BNE   @loop
      RTS

wait_cmd:
      LDA   VGC_CMD
      AND   #$01
      BNE   wait_cmd
      RTS

      .segment "RODATA"

player_visual:
      .byte 2,0
      .byte 0,0,SHAPE_PLAYER_TOP,0
      .byte 0,16,SHAPE_PLAYER_BOTTOM,0

shape_fill_pairs:
      .byte $CC,$CC,$99,$DD,$AA
shape_accent_pairs:
      .byte $FF,$FF,$FF,$EE,$FF

lane_table:
      .byte $E8,$00,$18,$00

spawn_delay_by_speed:
      .byte 0,52,44,36,28

band_y0:
      .byte 28,40,52,64,76,88,100,112,124,136,148,160,172,184
band_y1:
      .byte 41,53,65,77,89,101,113,125,137,149,161,173,185,199
band_half_width:
      .byte 34,38,43,48,54,60,67,74,82,90,99,108,118,128
band_curve_shift:
      .byte 0,0,0,1,1,1,2,2,2,3,3,4,4,4

      .include "msprite.s"
      .include "audio.s"

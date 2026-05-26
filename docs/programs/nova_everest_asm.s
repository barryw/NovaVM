; nova_everest_asm.s
;
; Nova-native mountain climbing game inspired by EVEREST from
; Commodore 64 Fun and Games. This is an original implementation, not a
; line-by-line conversion of the book listing.
;
; Build:
;   ca65 --cpu 65c02 -I runtime/asm -o /tmp/nova_everest_asm.o docs/programs/nova_everest_asm.s
;   ld65 -C docs/programs/diag7000.cfg -o /tmp/nova_everest_asm.bin /tmp/nova_everest_asm.o
;
; Load at $7000 and run with: SYS $7000
;
; Controls:
;   W/A/S/D or 8/4/2/6 move
;   Space drops supplies, up to three times
;   Q resets the climb after a win or failure

.setcpu "65c02"

.include "nova.inc"
.include "math_copro.inc"
.include "audio.inc"

BAND_COUNT          = 18
HAZ_COUNT           = 6
SIM_FRAME_DELAY     = 2

SPR_CLIMBER         = 0
SPR_PLANE           = 1
SPR_SUPPLY          = 2
SPR_HAZ_FIRST       = 3

SHAPE_CLIMBER       = 0
SHAPE_PLANE         = 1
SHAPE_SUPPLY        = 2
SHAPE_ROCK          = 3
SHAPE_AVALANCHE     = 4
SHAPE_COUNT         = 5

PLAYER_MIN_X        = 28
PLAYER_MAX_X        = 250
PLAYER_START_X      = 154
PLAYER_START_Y      = 182
SUMMIT_Y            = 28

ENERGY_MAX          = 100
ENERGY_WARN         = 28

GAME_PLAYING        = 0
GAME_WON            = 1
GAME_LOST           = 2

COLOR_BLACK         = 0
COLOR_WHITE         = 1
COLOR_RED           = 2
COLOR_CYAN          = 3
COLOR_GREEN         = 5
COLOR_BLUE          = 6
COLOR_YELLOW        = 7
COLOR_ORANGE        = 8
COLOR_BROWN         = 9
COLOR_LIGHT_RED     = 10
COLOR_DARK_GRAY     = 11
COLOR_GRAY          = 12
COLOR_LIGHT_GREEN   = 13
COLOR_LIGHT_BLUE    = 14
COLOR_LIGHT_GRAY    = 15

      .segment "BSS"

player_x:           .res 1
player_y:           .res 1
energy:             .res 1
supplies_left:      .res 1
game_state:         .res 1
frame_tick:         .res 1
hazard_timer:       .res 1
storm_ticks:        .res 1
storm_timer:        .res 1
wind:               .res 1
redraw_scene:       .res 1

plane_active:       .res 1
plane_x:            .res 1
plane_y:            .res 1
plane_drop_done:    .res 1
supply_active:      .res 1
supply_x:           .res 1
supply_y:           .res 1

haz_active:         .res HAZ_COUNT
haz_x:              .res HAZ_COUNT
haz_y:              .res HAZ_COUNT
haz_vx:             .res HAZ_COUNT
haz_vy:             .res HAZ_COUNT
haz_shape:          .res HAZ_COUNT

shape_slot:         .res 1
row_idx:            .res 1
byte_col:           .res 1
shape_addr_l:       .res 1
shape_addr_h:       .res 1
band_idx:           .res 1
temp0:              .res 1
temp1:              .res 1
temp2:              .res 1
haz_idx:            .res 1
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
      STZ   VGC_PALETTE
      STZ   VGC_GFXTRANS
      LDA   #COLOR_WHITE
      STA   VGC_FGCOL
      LDA   #COLOR_BLUE
      STA   VGC_BGCOL
      LDA   #$0C
      STA   VGC_CHAROUT
      LDA   #$03
      STA   VGC_MODE
      LDA   #COLOR_DARK_GRAY
      STA   VGC_BORDER
      LDA   #$0F
      STA   VGC_DIMMER

      JSR   wait_cmd
      LDA   #VCMD_COPPERDIS
      STA   VGC_CMD
      JSR   wait_cmd
      LDA   #VCMD_COPPERCLR
      STA   VGC_CMD

      JSR   clear_all_sprites
      JSR   define_shapes
      JSR   reset_game

main_loop:
      JSR   wait_game_frame
      JSR   read_controls

      LDA   game_state
      BNE   @show_end

      JSR   update_world
@show_end:
      LDA   redraw_scene
      BEQ   @dynamic_draw
      JSR   draw_scene
      STZ   redraw_scene
      BRA   @publish
@dynamic_draw:
      JSR   draw_hud
@publish:
      JSR   publish_sprites
      BRA   main_loop

reset_game:
      LDA   #PLAYER_START_X
      STA   player_x
      LDA   #PLAYER_START_Y
      STA   player_y
      LDA   #ENERGY_MAX
      STA   energy
      LDA   #$03
      STA   supplies_left
      STZ   game_state
      STZ   frame_tick
      STZ   storm_ticks
      STZ   plane_active
      STZ   supply_active
      STZ   plane_drop_done
      LDA   #$01
      STA   redraw_scene
      LDA   #$90
      STA   hazard_timer
      LDA   #$C0
      STA   storm_timer
      STZ   wind
      JSR   clear_hazards
      JSR   clear_all_sprites
      RTS

read_controls:
      LDA   VGC_CHARIN
      BEQ   @done

      CMP   #'q'
      BEQ   @reset
      CMP   #'Q'
      BEQ   @reset

      LDX   game_state
      BNE   @done

      CMP   #'a'
      BEQ   move_left
      CMP   #'A'
      BEQ   move_left
      CMP   #'4'
      BEQ   move_left

      CMP   #'d'
      BEQ   move_right
      CMP   #'D'
      BEQ   move_right
      CMP   #'6'
      BEQ   move_right

      CMP   #'w'
      BEQ   move_up
      CMP   #'W'
      BEQ   move_up
      CMP   #'8'
      BEQ   move_up

      CMP   #'s'
      BEQ   move_down
      CMP   #'S'
      BEQ   move_down
      CMP   #'2'
      BEQ   move_down

      CMP   #' '
      BEQ   call_supply
      RTS

@reset:
      JMP   reset_game
@done:
      RTS

move_left:
      LDA   player_x
      CMP   #(PLAYER_MIN_X + 2)
      BCS   @move
      LDA   #PLAYER_MIN_X
      STA   player_x
      RTS
@move:
      SEC
      SBC   #$02
      STA   player_x
      LDA   #$01
      JMP   spend_energy

move_right:
      LDA   player_x
      CMP   #(PLAYER_MAX_X - 2)
      BCC   @move
      LDA   #PLAYER_MAX_X
      STA   player_x
      RTS
@move:
      CLC
      ADC   #$02
      STA   player_x
      LDA   #$01
      JMP   spend_energy

move_up:
      LDA   player_y
      CMP   #(SUMMIT_Y + 2)
      BCS   @move
      JMP   win_game
@move:
      SEC
      SBC   #$02
      STA   player_y
      JSR   climb_cost
      JMP   spend_energy

move_down:
      LDA   player_y
      CMP   #PLAYER_START_Y
      BCS   @done
      CLC
      ADC   #$02
      STA   player_y
      LDA   #$01
      JMP   spend_energy
@done:
      RTS

call_supply:
      LDA   supplies_left
      BEQ   @done
      LDA   plane_active
      ORA   supply_active
      BNE   @done
      DEC   supplies_left
      LDA   #$01
      STA   plane_active
      STZ   plane_x
      LDA   #$18
      STA   plane_y
      STZ   plane_drop_done
      JSR   play_supply_sound
@done:
      RTS

update_world:
      INC   frame_tick
      JSR   update_storm
      JSR   update_plane_supply
      JSR   update_hazards
      JSR   passive_energy_drain
      JSR   check_game_end
      RTS

update_storm:
      LDA   storm_ticks
      BEQ   @countdown
      DEC   storm_ticks
      LDA   frame_tick
      AND   #$03
      BNE   @wind
      LDA   #$01
      JSR   spend_energy
      LDA   player_y
      CMP   #PLAYER_START_Y
      BCS   @wind
      INC   player_y
@wind:
      LDA   frame_tick
      AND   #$07
      BNE   @done
      MATHC_RNG_READ temp0
      LDA   temp0
      AND   #$03
      TAX
      LDA   wind_table,X
      STA   wind
      RTS

@countdown:
      LDA   storm_timer
      BEQ   @start_storm
      DEC   storm_timer
      RTS
@start_storm:
      MATHC_RNG_READ temp0
      LDA   temp0
      AND   #$1F
      CLC
      ADC   #$20
      STA   storm_ticks
      MATHC_RNG_READ temp0
      LDA   temp0
      AND   #$3F
      CLC
      ADC   #$50
      STA   storm_timer
      JSR   play_storm_sound
@done:
      RTS

update_plane_supply:
      LDA   plane_active
      BEQ   @supply
      LDA   plane_x
      CLC
      ADC   #$04
      STA   plane_x
      CMP   #$F0
      BCC   @drop_check
      STZ   plane_active
      BRA   @supply
@drop_check:
      LDA   plane_drop_done
      BNE   @supply
      LDA   plane_x
      CMP   player_x
      BCC   @supply
      LDA   #$01
      STA   plane_drop_done
      STA   supply_active
      LDA   player_x
      STA   supply_x
      LDA   #$22
      STA   supply_y

@supply:
      LDA   supply_active
      BEQ   @done
      LDA   supply_y
      CMP   #$BC
      BCS   @pickup
      CLC
      ADC   #$02
      STA   supply_y
@pickup:
      LDA   supply_y
      SEC
      SBC   player_y
      JSR   abs_a
      CMP   #$0C
      BCS   @done
      LDA   supply_x
      SEC
      SBC   player_x
      JSR   abs_a
      CMP   #$0C
      BCS   @done
      STZ   supply_active
      LDA   energy
      CLC
      ADC   #$2D
      CMP   #ENERGY_MAX
      BCC   @store
      LDA   #ENERGY_MAX
@store:
      STA   energy
      JSR   play_pickup_sound
@done:
      RTS

update_hazards:
      LDA   hazard_timer
      BEQ   @spawn
      DEC   hazard_timer
      BRA   @move
@spawn:
      JSR   spawn_hazard
      MATHC_RNG_READ temp0
      LDA   temp0
      AND   #$3F
      CLC
      ADC   #$70
      STA   hazard_timer

@move:
      STZ   haz_idx
@loop:
      LDX   haz_idx
      LDA   haz_active,X
      BEQ   @next

      LDA   haz_x,X
      CLC
      ADC   haz_vx,X
      STA   haz_x,X

      LDA   storm_ticks
      BEQ   @no_wind
      LDA   haz_x,X
      CLC
      ADC   wind
      STA   haz_x,X
@no_wind:
      LDA   haz_y,X
      CLC
      ADC   haz_vy,X
      STA   haz_y,X

      LDA   haz_y,X
      CMP   #$C8
      BCS   @disable
      LDA   haz_x,X
      CMP   #$10
      BCC   @disable
      CMP   #$F2
      BCC   @collision
@disable:
      STZ   haz_active,X
      BRA   @next

@collision:
      JSR   check_hazard_collision
@next:
      INC   haz_idx
      LDA   haz_idx
      CMP   #HAZ_COUNT
      BNE   @loop
      RTS

spawn_hazard:
      LDX   #$00
@find:
      LDA   haz_active,X
      BEQ   @slot
      INX
      CPX   #HAZ_COUNT
      BNE   @find
      RTS

@slot:
      STX   haz_idx
      LDA   #$01
      STA   haz_active,X
      MATHC_RNG_READ temp0
      LDA   temp0
      AND   #$7F
      CLC
      ADC   #$40
      STA   haz_x,X
      LDA   #$20
      STA   haz_y,X
      MATHC_RNG_READ temp0
      LDA   temp0
      AND   #$03
      TAY
      LDA   hazard_vx_table,Y
      LDX   haz_idx
      STA   haz_vx,X
      MATHC_RNG_READ temp0
      LDA   temp0
      AND   #$01
      CLC
      ADC   #$02
      STA   haz_vy,X
      MATHC_RNG_READ temp0
      LDA   temp0
      AND   #$03
      CMP   #$01
      BNE   @rock
      LDA   #SHAPE_AVALANCHE
      STA   haz_shape,X
      LDA   #$03
      STA   haz_vy,X
      RTS
@rock:
      LDA   #SHAPE_ROCK
      STA   haz_shape,X
      RTS

check_hazard_collision:
      LDX   haz_idx
      LDA   haz_y,X
      SEC
      SBC   player_y
      JSR   abs_a
      CMP   #$0C
      BCS   @done
      LDX   haz_idx
      LDA   haz_x,X
      SEC
      SBC   player_x
      JSR   abs_a
      CMP   #$0C
      BCS   @done
      LDX   haz_idx
      STZ   haz_active,X
      LDA   player_y
      CLC
      ADC   #$0A
      CMP   #PLAYER_START_Y
      BCC   @store_y
      LDA   #PLAYER_START_Y
@store_y:
      STA   player_y
      LDA   #$12
      JSR   spend_energy
      JSR   play_hit_sound
@done:
      RTS

passive_energy_drain:
      LDA   frame_tick
      AND   #$3F
      BNE   @done
      LDA   #$01
      JMP   spend_energy
@done:
      RTS

check_game_end:
      LDA   player_y
      CMP   #SUMMIT_Y
      BCC   win_game
      LDA   energy
      BEQ   lose_game
      RTS

win_game:
      LDA   #GAME_WON
      STA   game_state
      LDA   #COLOR_GREEN
      STA   VGC_BORDER
      JSR   play_win_sound
      RTS

lose_game:
      LDA   #GAME_LOST
      STA   game_state
      LDA   #COLOR_DARK_GRAY
      STA   VGC_BORDER
      JSR   play_hit_sound
      RTS

spend_energy:
      STA   temp0
      LDA   energy
      CMP   temp0
      BCS   @ok
      STZ   energy
      RTS
@ok:
      SEC
      SBC   temp0
      STA   energy
      RTS

climb_cost:
      JSR   player_on_rock
      BCS   @rock
      LDA   #$02
      BRA   @storm
@rock:
      LDA   #$01
@storm:
      STA   temp2
      LDA   storm_ticks
      BEQ   @done
      INC   temp2
@done:
      LDA   temp2
      RTS

player_on_rock:
      JSR   player_band
      LDA   rock1_x,X
      JSR   near_player_x
      BCS   @yes
      LDX   band_idx
      LDA   rock2_x,X
      JSR   near_player_x
      BCS   @yes
      CLC
      RTS
@yes:
      SEC
      RTS

player_band:
      LDX   #$00
@loop:
      LDA   player_y
      CMP   band_y1,X
      BCC   @found
      INX
      CPX   #BAND_COUNT
      BNE   @loop
      LDX   #(BAND_COUNT - 1)
@found:
      STX   band_idx
      RTS

near_player_x:
      STA   temp0
      LDA   player_x
      SEC
      SBC   temp0
      JSR   abs_a
      CMP   #$0C
      BCC   @near
      CLC
      RTS
@near:
      SEC
      RTS

abs_a:
      BCS   @positive
      EOR   #$FF
      CLC
      ADC   #$01
@positive:
      RTS

clear_hazards:
      LDX   #$00
@loop:
      STZ   haz_active,X
      INX
      CPX   #HAZ_COUNT
      BNE   @loop
      RTS

draw_scene:
      STZ   VGC_CURSEN
      LDA   #$03
      STA   VGC_MODE
      JSR   wait_cmd
      LDA   #VCMD_GCLS
      STA   VGC_CMD
      JSR   wait_cmd

      LDA   #COLOR_LIGHT_BLUE
      JSR   set_gcolor
      STZ   rect_x0_l
      STZ   rect_x0_h
      STZ   rect_y0
      LDA   #<319
      STA   rect_x1_l
      LDA   #>319
      STA   rect_x1_h
      LDA   #199
      STA   rect_y1
      JSR   fill_rect

      STZ   band_idx
@band_loop:
      JSR   draw_mountain_band
      INC   band_idx
      LDA   band_idx
      CMP   #BAND_COUNT
      BNE   @band_loop

      STZ   band_idx
@rock_loop:
      JSR   draw_rock_patches
      INC   band_idx
      LDA   band_idx
      CMP   #BAND_COUNT
      BNE   @rock_loop

      JSR   draw_summit_details

      LDA   storm_ticks
      BEQ   @hud
      JSR   draw_storm
@hud:
      JSR   draw_hud
      RTS

draw_mountain_band:
      LDX   band_idx
      LDA   mountain_half_width,X
      STA   temp0
      LDA   #160
      SEC
      SBC   temp0
      STA   rect_x0_l
      STZ   rect_x0_h
      LDA   #160
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
      LDX   band_idx
      LDA   mountain_color,X
      JSR   set_gcolor
      JMP   fill_rect

draw_rock_patches:
      LDA   #COLOR_DARK_GRAY
      JSR   set_gcolor
      LDX   band_idx
      LDA   rock1_x,X
      JSR   draw_rock_patch_at_a
      LDA   #COLOR_BROWN
      JSR   set_gcolor
      LDX   band_idx
      LDA   rock2_x,X
      JMP   draw_rock_patch_at_a

draw_rock_patch_at_a:
      STA   temp0
      SEC
      SBC   #$04
      STA   rect_x0_l
      STZ   rect_x0_h
      LDA   temp0
      CLC
      ADC   #$05
      STA   rect_x1_l
      STZ   rect_x1_h
      LDX   band_idx
      LDA   band_y0,X
      CLC
      ADC   #$03
      STA   rect_y0
      CLC
      ADC   #$04
      STA   rect_y1
      JMP   fill_rect

draw_summit_details:
      LDA   #COLOR_DARK_GRAY
      JSR   set_gcolor
      LDA   #160
      STA   rect_x0_l
      STZ   rect_x0_h
      LDA   #$10
      STA   rect_y0
      LDA   #161
      STA   rect_x1_l
      STZ   rect_x1_h
      LDA   #$22
      STA   rect_y1
      JSR   fill_rect

      LDA   #COLOR_ORANGE
      JSR   set_gcolor
      LDA   #162
      STA   rect_x0_l
      STZ   rect_x0_h
      LDA   #$11
      STA   rect_y0
      LDA   #176
      STA   rect_x1_l
      STZ   rect_x1_h
      LDA   #$18
      STA   rect_y1
      JMP   fill_rect

draw_storm:
      LDA   #COLOR_WHITE
      JSR   set_gcolor
      LDX   #$00
@loop:
      LDA   storm_x,X
      STA   rect_x0_l
      STZ   rect_x0_h
      CLC
      ADC   #$18
      STA   rect_x1_l
      STZ   rect_x1_h
      LDA   storm_y,X
      CLC
      ADC   frame_tick
      AND   #$7F
      CLC
      ADC   #$20
      STA   rect_y0
      CLC
      ADC   #$01
      STA   rect_y1
      JSR   fill_rect
      INX
      CPX   #$06
      BNE   @loop
      RTS

draw_hud:
      LDA   #COLOR_DARK_GRAY
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
      JSR   draw_title

      LDA   energy
      CMP   #ENERGY_WARN
      BCS   @energy_ok
      LDA   #COLOR_RED
      BRA   @energy_color
@energy_ok:
      LDA   #COLOR_GREEN
@energy_color:
      JSR   set_gcolor
      LDA   #$08
      STA   rect_x0_l
      STZ   rect_x0_h
      LDA   #$04
      STA   rect_y0
      LDA   energy
      CLC
      ADC   #$08
      STA   rect_x1_l
      STZ   rect_x1_h
      LDA   #$0A
      STA   rect_y1
      JSR   fill_rect

      LDA   #COLOR_YELLOW
      JSR   set_gcolor
      LDA   #$D0
      STA   rect_x0_l
      STZ   rect_x0_h
      LDA   #$04
      STA   rect_y0
      LDA   #PLAYER_START_Y
      SEC
      SBC   player_y
      LSR
      CLC
      ADC   #$D0
      STA   rect_x1_l
      LDA   #$00
      ADC   #$00
      STA   rect_x1_h
      LDA   #$0A
      STA   rect_y1
      JSR   fill_rect

      LDA   #COLOR_CYAN
      JSR   set_gcolor
      LDX   supplies_left
      BEQ   @state
@sup_loop:
      TXA
      ASL
      ASL
      CLC
      ADC   #$C0
      STA   rect_x0_l
      STZ   rect_x0_h
      LDA   #$03
      STA   rect_y0
      LDA   rect_x0_l
      CLC
      ADC   #$02
      STA   rect_x1_l
      STZ   rect_x1_h
      LDA   #$0C
      STA   rect_y1
      JSR   fill_rect
      DEX
      BNE   @sup_loop

@state:
      LDA   game_state
      BEQ   @done
      CMP   #GAME_WON
      BEQ   @won
      LDA   #COLOR_RED
      BRA   @state_color
@won:
      LDA   #COLOR_GREEN
@state_color:
      JSR   set_gcolor
      LDA   #$8A
      STA   rect_x0_l
      STZ   rect_x0_h
      LDA   #$03
      STA   rect_y0
      LDA   #$A6
      STA   rect_x1_l
      STZ   rect_x1_h
      LDA   #$0C
      STA   rect_y1
      JSR   fill_rect
@done:
      RTS

draw_title:
      LDA   #COLOR_WHITE
      JSR   set_gcolor
      LDX   #$00
@copy:
      LDA   title_text,X
      BEQ   @copied
      STA   FIO_NAME,X
      INX
      BNE   @copy
@copied:
      STX   FIO_NAMELEN
      LDA   #$82
      STA   VGC_P0
      STZ   VGC_P1
      LDA   #$04
      STA   VGC_P2
      STZ   VGC_P3
      STZ   VGC_P4
      LDA   #$01
      STA   VGC_P5
      JSR   wait_cmd
      LDA   #VCMD_GTEXT
      STA   VGC_CMD
      JSR   wait_cmd
      RTS

publish_sprites:
      LDA   player_x
      STA   temp0
      LDA   player_y
      STA   temp1
      LDA   #SHAPE_CLIMBER
      STA   temp2
      LDA   #SPR_CLIMBER
      JSR   publish_sprite

      LDA   plane_active
      BEQ   @hide_plane
      LDA   plane_x
      STA   temp0
      LDA   plane_y
      STA   temp1
      LDA   #SHAPE_PLANE
      STA   temp2
      LDA   #SPR_PLANE
      JSR   publish_sprite
      BRA   @supply
@hide_plane:
      LDA   #SPR_PLANE
      JSR   disable_sprite_a

@supply:
      LDA   supply_active
      BEQ   @hide_supply
      LDA   supply_x
      STA   temp0
      LDA   supply_y
      STA   temp1
      LDA   #SHAPE_SUPPLY
      STA   temp2
      LDA   #SPR_SUPPLY
      JSR   publish_sprite
      BRA   @hazards
@hide_supply:
      LDA   #SPR_SUPPLY
      JSR   disable_sprite_a

@hazards:
      STZ   haz_idx
@loop:
      LDX   haz_idx
      LDA   haz_active,X
      BEQ   @hide_haz
      LDA   haz_x,X
      STA   temp0
      LDA   haz_y,X
      STA   temp1
      LDA   haz_shape,X
      STA   temp2
      TXA
      CLC
      ADC   #SPR_HAZ_FIRST
      JSR   publish_sprite
      BRA   @next
@hide_haz:
      TXA
      CLC
      ADC   #SPR_HAZ_FIRST
      JSR   disable_sprite_a
@next:
      INC   haz_idx
      LDA   haz_idx
      CMP   #HAZ_COUNT
      BNE   @loop
      RTS

; Entry: A=sprite index, temp0=x, temp1=y, temp2=shape.
publish_sprite:
      ASL
      ASL
      ASL
      TAY
      LDA   temp0
      STA   VGC_SPR_BASE + VGC_SPR_XL_OFF,Y
      LDA   #$00
      STA   VGC_SPR_BASE + VGC_SPR_XH_OFF,Y
      LDA   temp1
      STA   VGC_SPR_BASE + VGC_SPR_YL_OFF,Y
      LDA   #$00
      STA   VGC_SPR_BASE + VGC_SPR_YH_OFF,Y
      LDA   temp2
      STA   VGC_SPR_BASE + VGC_SPR_SHAPE_OFF,Y
      LDA   #VGC_SPR_PRI_FRONT
      STA   VGC_SPR_BASE + VGC_SPR_PRI_OFF,Y
      LDA   #$00
      STA   VGC_SPR_BASE + VGC_SPR_TRANS_OFF,Y
      LDA   #VGC_SPR_FLAG_ENABLE
      STA   VGC_SPR_BASE + VGC_SPR_FLAGS_OFF,Y
      RTS

disable_sprite_a:
      ASL
      ASL
      ASL
      TAY
      LDA   #$00
      STA   VGC_SPR_BASE + VGC_SPR_FLAGS_OFF,Y
      RTS

clear_all_sprites:
      LDA   #$00
@loop:
      PHA
      JSR   disable_sprite_a
      PLA
      INC
      CMP   #$10
      BNE   @loop
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

define_shapes:
      STZ   shape_addr_l
      STZ   shape_addr_h
      STZ   shape_slot
@shape_loop:
      STZ   row_idx
@row_loop:
      LDX   #$00
@byte_loop:
      STX   byte_col
      JSR   choose_shape_byte
      STA   temp0
      LDA   #$04
      STA   VGC_P0
      LDA   shape_addr_l
      STA   VGC_P1
      LDA   shape_addr_h
      STA   VGC_P2
      LDA   temp0
      STA   VGC_P3
      STZ   VGC_P4
      JSR   wait_cmd
      LDA   #VCMD_MEMWRITE
      STA   VGC_CMD
      JSR   wait_cmd
      INC   shape_addr_l
      BNE   @next_byte
      INC   shape_addr_h
@next_byte:
      LDX   byte_col
      INX
      CPX   #$08
      BNE   @byte_loop
      INC   row_idx
      LDA   row_idx
      CMP   #$10
      BNE   @row_loop
      INC   shape_slot
      LDA   shape_slot
      CMP   #SHAPE_COUNT
      BNE   @shape_loop
      RTS

choose_shape_byte:
      LDA   row_idx
      ASL
      ASL
      ASL
      CLC
      ADC   byte_col
      TAX
      LDA   shape_slot
      CMP   #SHAPE_CLIMBER
      BNE   @not_climber
      LDA   climber_shape,X
      RTS
@not_climber:
      CMP   #SHAPE_PLANE
      BNE   @not_plane
      LDA   plane_shape,X
      RTS
@not_plane:
      CMP   #SHAPE_SUPPLY
      BNE   @not_supply
      LDA   supply_shape,X
      RTS
@not_supply:
      CMP   #SHAPE_AVALANCHE
      BNE   @rock
      LDA   avalanche_shape,X
      RTS
@rock:
      LDA   rock_shape,X
      RTS

play_supply_sound:
      LDA   #$48
      LDX   #$08
      LDY   #$00
      JMP   play_note

play_pickup_sound:
      LDA   #$54
      LDX   #$06
      LDY   #$00
      JMP   play_note

play_hit_sound:
      LDA   #$28
      LDX   #$12
      LDY   #$00
      JMP   play_note

play_storm_sound:
      LDA   #$34
      LDX   #$0C
      LDY   #$00
      JMP   play_note

play_win_sound:
      LDA   #$60
      LDX   #$18
      LDY   #$00
      JMP   play_note

play_note:
      STA   AUDIO_NOTE
      STX   AUDIO_DURATION
      STY   AUDIO_INSTRUMENT
      STZ   FIO_STATUS
      STZ   FIO_ERRCODE
      LDA   #FIO_CMD_SOUND
      STA   FIO_CMD
      RTS

wait_game_frame:
      LDY   #SIM_FRAME_DELAY
@loop:
      JSR   wait_frame
      DEY
      BNE   @loop
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

band_y0:
      .byte 22,32,42,52,62,72,82,92,102,112,122,132,142,152,162,172,182,192
band_y1:
      .byte 31,41,51,61,71,81,91,101,111,121,131,141,151,161,171,181,191,199
mountain_half_width:
      .byte 10,18,28,38,50,62,74,86,98,110,122,132,140,146,151,154,156,158
mountain_color:
      .byte 1,1,15,15,15,15,15,12,15,15,15,12,15,15,15,15,15,15
rock1_x:
      .byte 156,151,146,141,136,131,126,121,116,111,106,101,96,91,86,81,76,72
rock2_x:
      .byte 166,171,176,181,187,193,199,205,211,217,223,229,235,240,244,247,249,250

hazard_vx_table:
      .byte $FF,$00,$01,$00
wind_table:
      .byte $FF,$00,$01,$00
storm_x:
      .byte 20,64,108,152,196,230
storm_y:
      .byte 8,34,60,18,46,72
title_text:
      .byte "EVEREST",0

climber_shape:
      .byte $00,$00,$00,$00,$00,$00,$00,$00
      .byte $00,$00,$00,$77,$77,$00,$00,$00
      .byte $00,$00,$07,$77,$77,$70,$00,$00
      .byte $00,$00,$09,$99,$99,$90,$00,$00
      .byte $00,$00,$88,$88,$88,$88,$00,$00
      .byte $00,$01,$88,$88,$88,$88,$10,$00
      .byte $00,$00,$88,$33,$33,$88,$00,$00
      .byte $00,$00,$08,$33,$33,$80,$00,$00
      .byte $00,$00,$00,$33,$33,$00,$00,$00
      .byte $00,$00,$01,$33,$33,$10,$00,$00
      .byte $00,$00,$10,$00,$00,$01,$00,$00
      .byte $00,$01,$10,$00,$00,$11,$00,$00
      .byte $00,$11,$00,$00,$00,$01,$10,$00
      .byte $00,$11,$00,$00,$00,$01,$10,$00
      .byte $00,$BB,$00,$00,$00,$00,$BB,$00
      .byte $00,$00,$00,$00,$00,$00,$00,$00

plane_shape:
      .byte $00,$00,$00,$00,$00,$00,$00,$00
      .byte $00,$00,$00,$00,$11,$00,$00,$00
      .byte $00,$00,$00,$01,$11,$10,$00,$00
      .byte $00,$00,$00,$11,$11,$11,$00,$00
      .byte $00,$00,$11,$11,$CC,$11,$11,$00
      .byte $00,$11,$11,$CC,$CC,$CC,$11,$10
      .byte $11,$11,$CC,$CC,$CC,$CC,$CC,$11
      .byte $00,$00,$00,$11,$11,$11,$00,$00
      .byte $00,$00,$11,$10,$00,$11,$10,$00
      .byte $00,$11,$10,$00,$00,$01,$11,$00
      .byte $00,$00,$00,$00,$00,$00,$00,$00
      .byte $00,$00,$00,$00,$00,$00,$00,$00
      .byte $00,$00,$00,$00,$00,$00,$00,$00
      .byte $00,$00,$00,$00,$00,$00,$00,$00
      .byte $00,$00,$00,$00,$00,$00,$00,$00
      .byte $00,$00,$00,$00,$00,$00,$00,$00

supply_shape:
      .byte $00,$00,$00,$00,$00,$00,$00,$00
      .byte $00,$00,$33,$33,$33,$33,$00,$00
      .byte $00,$33,$33,$11,$11,$33,$33,$00
      .byte $03,$33,$11,$11,$11,$11,$33,$30
      .byte $00,$03,$30,$00,$00,$03,$30,$00
      .byte $00,$03,$00,$00,$00,$00,$30,$00
      .byte $00,$30,$00,$00,$00,$00,$03,$00
      .byte $00,$30,$00,$88,$88,$00,$03,$00
      .byte $00,$00,$08,$88,$88,$80,$00,$00
      .byte $00,$00,$88,$99,$99,$88,$00,$00
      .byte $00,$00,$88,$99,$99,$88,$00,$00
      .byte $00,$00,$88,$88,$88,$88,$00,$00
      .byte $00,$00,$08,$88,$88,$80,$00,$00
      .byte $00,$00,$00,$00,$00,$00,$00,$00
      .byte $00,$00,$00,$00,$00,$00,$00,$00
      .byte $00,$00,$00,$00,$00,$00,$00,$00

rock_shape:
      .byte $00,$00,$00,$00,$00,$00,$00,$00
      .byte $00,$00,$00,$BB,$00,$00,$00,$00
      .byte $00,$00,$BB,$99,$BB,$00,$00,$00
      .byte $00,$0B,$99,$99,$99,$B0,$00,$00
      .byte $00,$B9,$99,$BB,$99,$9B,$00,$00
      .byte $0B,$99,$BB,$99,$BB,$99,$B0,$00
      .byte $0B,$99,$99,$99,$99,$99,$B0,$00
      .byte $BB,$99,$BB,$99,$99,$BB,$99,$B0
      .byte $0B,$99,$99,$99,$BB,$99,$B0,$00
      .byte $00,$BB,$99,$BB,$99,$BB,$00,$00
      .byte $00,$0B,$BB,$99,$BB,$B0,$00,$00
      .byte $00,$00,$BB,$BB,$BB,$00,$00,$00
      .byte $00,$00,$00,$BB,$00,$00,$00,$00
      .byte $00,$00,$00,$00,$00,$00,$00,$00
      .byte $00,$00,$00,$00,$00,$00,$00,$00
      .byte $00,$00,$00,$00,$00,$00,$00,$00

avalanche_shape:
      .byte $00,$00,$00,$00,$00,$00,$00,$00
      .byte $00,$00,$0F,$F0,$00,$00,$00,$00
      .byte $00,$0F,$FF,$FC,$F0,$00,$00,$00
      .byte $00,$FF,$CC,$FF,$FF,$00,$00,$00
      .byte $0F,$FC,$FF,$CC,$FF,$F0,$00,$00
      .byte $0F,$FF,$FF,$FF,$CC,$FF,$00,$00
      .byte $FF,$CC,$FF,$FF,$FF,$FF,$F0,$00
      .byte $0F,$FF,$FF,$CC,$FF,$FC,$00,$00
      .byte $00,$FF,$CC,$FF,$FF,$F0,$00,$00
      .byte $00,$0F,$FF,$FF,$FC,$00,$00,$00
      .byte $00,$00,$FF,$CC,$F0,$00,$00,$00
      .byte $00,$00,$0F,$F0,$00,$00,$00,$00
      .byte $00,$00,$00,$00,$00,$00,$00,$00
      .byte $00,$00,$00,$00,$00,$00,$00,$00
      .byte $00,$00,$00,$00,$00,$00,$00,$00
      .byte $00,$00,$00,$00,$00,$00,$00,$00

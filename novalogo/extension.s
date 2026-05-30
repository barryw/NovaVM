; extension.s — NovaLogo extension ROM
; Loaded at $C000 when swapped via REG_ROMSWAP = ROMSWAP_EXTENSION ($04).
;
; Entry point at $C000: reads EXT_CMD from zero page and dispatches.
; Handlers receive pre-evaluated arguments in EXT_ARG0..2 and return
; results in EXT_RESULT. No callbacks into the base ROM interpreter.

      .include "nova.inc"
NOVALOGO_TURTLE_STATE_EXTERNAL = 1
      .include "ext_iface.inc"
      .include "vgc.inc"
      .include "copper.inc"
      .include "copper_split.inc"
      .include "vsprite.inc"

; --- Turtle virtual-sprite configuration ---
TURTLE_SPR_SIZE   = 16
TURTLE_SPR_HALF   = 8
TURTLE_MAX_X      = 304        ; 320 - 16
TURTLE_MAX_Y      = 184        ; 200 - 16
TURTLE_CENTER_X   = 160        ; center of 320-wide graphics
TURTLE_CENTER_Y   = 80         ; center of split graphics viewport
SPLIT_Y           = 160        ; copper split: gfx rows 0-159, text below
SPLIT_TEXT_ROW    = 40         ; 160px / 4px text cell height
SPLIT_TEXT_HEIGHT = 10
MODE_GFX_SPRITES  = 3          ; VGC mode: graphics + sprites
MODE_TEXT_ONLY    = 0
COL_WHITE         = 1
COL_RED           = 2
COL_GREEN         = 5

; --- Base-ROM list cell format, used by SETPOS [x y] ---
CONS_CAR_TYPE     = 1
CONS_CAR_HI       = 2
CONS_CAR_LO       = 3
CONS_CDR_LO       = 5
CONS_CDR_HI       = 6

; --- Local ZP temporaries (reuse NVR scratch) ---
tmp0              = NVR0L
tmp1              = NVR0H
tmp2              = NVR1L
tmp3              = NVR1H
old_x_lo          = NVR2L
old_x_hi          = NVR2H
old_y_lo          = NVR3L
old_y_hi          = NVR3H
dx_lo             = NVR4L
dx_hi             = NVR4H
dy_lo             = NVR5L
dy_hi             = NVR5H
sin_val           = NVR6L
cos_val           = NVR6H

      .segment "BSS"

turtle_source_shape:
      .res TURTLE_SPR_SIZE * TURTLE_SPR_SIZE
turtle_rotated_shape:
      .res TURTLE_SPR_SIZE * TURTLE_SPR_SIZE
turtle_saved_bg:
      .res TURTLE_SPR_SIZE * TURTLE_SPR_SIZE
TURTLE_X_FRAC:
      .res 1
TURTLE_X_LO:
      .res 1
TURTLE_X_HI:
      .res 1
TURTLE_Y_FRAC:
      .res 1
TURTLE_Y_LO:
      .res 1
TURTLE_Y_HI:
      .res 1
TURTLE_HEADING_LO:
      .res 1
TURTLE_HEADING_HI:
      .res 1
TURTLE_PEN:
      .res 1
TURTLE_SHOWN:
      .res 1
TURTLE_COLOR:
      .res 1
TURTLE_SPRITE:
      .res 1
TURTLE_INITED:
      .res 1
turtle_bg_x_lo:
      .res 1
turtle_bg_x_hi:
      .res 1
turtle_bg_y:
      .res 1
turtle_bg_saved:
      .res 1

      .segment "CODE"

; =====================================================================
; Extension entry point — called via JSR $C000 from RAM trampoline
; =====================================================================
ext_entry:
      CLD                       ; all extension math is binary, never BCD
      LDA   EXT_CMD
      ASL                       ; * 2 for word-sized table
      TAX
      LDA   ext_dispatch+1,X   ; high byte of handler-1
      PHA
      LDA   ext_dispatch,X     ; low byte of handler-1
      PHA
      RTS                       ; dispatch via RTS trick

; --- Dispatch table (entries are handler_address - 1) ---
ext_dispatch:
      .word ext_unsupported-1   ; cmd $00: reserved
      .word ext_test-1          ; cmd $01: EXT.TEST
      .word ext_unsupported-1   ; cmd $02
      .word ext_unsupported-1   ; cmd $03
      .word ext_unsupported-1   ; cmd $04
      .word ext_unsupported-1   ; cmd $05
      .word ext_unsupported-1   ; cmd $06
      .word ext_unsupported-1   ; cmd $07
      .word ext_unsupported-1   ; cmd $08
      .word ext_unsupported-1   ; cmd $09
      .word ext_unsupported-1   ; cmd $0A
      .word ext_unsupported-1   ; cmd $0B
      .word ext_unsupported-1   ; cmd $0C
      .word ext_unsupported-1   ; cmd $0D
      .word ext_unsupported-1   ; cmd $0E
      .word ext_unsupported-1   ; cmd $0F
      .word ext_fd-1            ; cmd $10: FD
      .word ext_bk-1            ; cmd $11: BK
      .word ext_rt-1            ; cmd $12: RT
      .word ext_lt-1            ; cmd $13: LT
      .word ext_cs-1            ; cmd $14: CS
      .word ext_pu-1            ; cmd $15: PU
      .word ext_pd-1            ; cmd $16: PD
      .word ext_st-1            ; cmd $17: ST
      .word ext_ht-1            ; cmd $18: HT
      .word ext_home-1          ; cmd $19: HOME
      .word ext_ts-1            ; cmd $1A: TEXTSCREEN
      .word ext_ss-1            ; cmd $1B: SPLITSCREEN
      .word ext_fs-1            ; cmd $1C: FULLSCREEN
      .word ext_setxy-1         ; cmd $1D: SETXY
      .word ext_setx-1          ; cmd $1E: SETX
      .word ext_sety-1          ; cmd $1F: SETY
      .word ext_seth-1          ; cmd $20: SETHEADING
      .word ext_xcor-1          ; cmd $21: XCOR
      .word ext_ycor-1          ; cmd $22: YCOR
      .word ext_heading-1       ; cmd $23: HEADING
      .word ext_pendownp-1      ; cmd $24: PENDOWN?
      .word ext_shownp-1        ; cmd $25: SHOWN?
      .word ext_setpc-1         ; cmd $26: SETPC
      .word ext_setbg-1         ; cmd $27: SETBG
      .word ext_towards-1       ; cmd $28: TOWARDS
      .word ext_setpos-1        ; cmd $29: SETPOS
      .word ext_unsupported-1   ; cmd $2A
      .word ext_unsupported-1   ; cmd $2B
      .word ext_unsupported-1   ; cmd $2C
      .word ext_unsupported-1   ; cmd $2D
      .word ext_unsupported-1   ; cmd $2E
      .word ext_unsupported-1   ; cmd $2F
      .word ext_setcolor-1      ; cmd $30: SETCOLOR
      .word ext_plot-1          ; cmd $31: PLOT
      .word ext_unplot-1        ; cmd $32: UNPLOT
      .word ext_line-1          ; cmd $33: LINE
      .word ext_circle-1        ; cmd $34: CIRCLE
      .word ext_rect-1          ; cmd $35: RECT
      .word ext_fillrect-1      ; cmd $36: FILL
      .word ext_paint-1         ; cmd $37: PAINT
      .word ext_unsupported-1   ; cmd $38
      .word ext_unsupported-1   ; cmd $39
      .word ext_unsupported-1   ; cmd $3A
      .word ext_unsupported-1   ; cmd $3B
      .word ext_unsupported-1   ; cmd $3C
      .word ext_unsupported-1   ; cmd $3D
      .word ext_unsupported-1   ; cmd $3E
      .word ext_unsupported-1   ; cmd $3F
      .word ext_sprite-1        ; cmd $40: SPRITE
      .word ext_spritepos-1     ; cmd $41: SPRITEPOS
      .word ext_spriteon-1      ; cmd $42: SPRITEON
      .word ext_spriteoff-1     ; cmd $43: SPRITEOFF
      .word ext_sprcollp-1      ; cmd $44: SPRITECOLLISION?
      .word ext_tone-1          ; cmd $45: TONE
      .word ext_noise-1         ; cmd $46: NOISE
      .word ext_volume-1        ; cmd $47: VOLUME
      .word ext_wait-1          ; cmd $48: WAIT
      .word ext_waitvbl-1       ; cmd $49: WAITVBL
      .word ext_timer-1         ; cmd $4A: TIMER

; =====================================================================
; ext_unsupported — unknown command, just return
; =====================================================================
ext_unsupported:
      RTS

; =====================================================================
; ext_test — test command: returns arg0 + 1
; =====================================================================
ext_test:
      CLC
      LDA   EXT_ARG0_LO
      ADC   #1
      STA   EXT_RESULT_LO
      LDA   EXT_ARG0_HI
      ADC   #0
      STA   EXT_RESULT_HI
      LDA   EXT_ARG0_FRAC
      STA   EXT_RESULT_FRAC
      STZ   EXT_RESULT_TYPE    ; VAL_NUMBER
      RTS

; =====================================================================
; ext_cs — clear screen, set up graphics mode, init turtle
; =====================================================================
ext_cs:
      STZ   VGC_BGCOL
      LDA   #COL_WHITE
      STA   VGC_FGCOL
      STZ   VGC_BORDER
      STZ   VGC_GFXTRANS
      STZ   VGC_SCROLLX
      STZ   VGC_SCROLLY
      STZ   VGC_SCROLLCTL
      LDA   #MODE_TEXT_ONLY
      STA   VGC_MODE
      JSR   clear_gfx_plane

      ; Set up copper split: graphics+sprites top, text bottom
      JSR   setup_copper
      JSR   prepare_split_text

      ; Initialize turtle state
      JSR   turtle_init

      ; Show centered virtual sprite
      JSR   draw_turtle_sprite

      RTS

; =====================================================================
; ext_fd — move forward by distance
; =====================================================================
ext_fd:
      ; Ensure turtle is initialized
      LDA   TURTLE_INITED
      BNE   @go
      JSR   turtle_init
@go:
      ; Save old integer position for line drawing
      LDA   TURTLE_X_LO
      STA   old_x_lo
      LDA   TURTLE_X_HI
      STA   old_x_hi
      LDA   TURTLE_Y_LO
      STA   old_y_lo
      LDA   TURTLE_Y_HI
      STA   old_y_hi
      JSR   erase_turtle_sprite

      ; Convert heading degrees to the exact NDK/blitter angle byte.
      JSR   turtle_heading_u8
      STA   tmp0

      ; Get sin/cos from coprocessor
      ; Logo heading: 0=north, 90=east, 180=south, 270=west
      ; sin(heading) gives dx, -cos(heading) gives dy (screen y goes down)
      STA   MATH_SINCOS_ANGLE  ; triggers sincos
      LDA   MATH_RES0          ; sin (1.7 signed: -128..+127)
      STA   sin_val
      LDA   MATH_RES1          ; cos (1.7 signed)
      STA   cos_val

      ; Compute dx = distance * sin / 128
      ; distance is 16.8 fixed point in EXT_ARG0 (hi:lo:frac)
      ; For simplicity, use integer part only for the multiply
      LDA   EXT_ARG0_LO
      STA   MATH_MUL16_A_LO
      LDA   EXT_ARG0_HI
      STA   MATH_MUL16_A_HI
      ; Sign-extend sin to 16-bit
      LDA   sin_val
      STA   MATH_MUL16_B_LO
      ORA   #$7F
      BMI   @sin_neg
      LDA   #$00
      BRA   @sin_ext
@sin_neg:
      LDA   #$FF
@sin_ext:
      STA   MATH_MUL16_B_HI    ; triggers multiply

      ; dx = round(result / 128). The math coprocessor exposes 1.7 signed
      ; sin/cos, so +1.0 is 127/128; rounding keeps cardinal moves exact.
      CLC
      LDA   MATH_RES0
      ADC   #64
      STA   tmp2
      LDA   MATH_RES1
      ADC   #0
      STA   tmp3
      LDA   MATH_RES2
      ADC   #0
      STA   tmp1
      LDA   tmp2
      ASL                       ; bit 7 → carry
      LDA   tmp3
      ROL
      STA   dx_lo
      LDA   tmp1
      ROL
      STA   dx_hi

      ; Compute dy = -(distance * cos / 128)
      ; (negative because screen y increases downward)
      LDA   EXT_ARG0_LO
      STA   MATH_MUL16_A_LO
      LDA   EXT_ARG0_HI
      STA   MATH_MUL16_A_HI
      ; Sign-extend cos to 16-bit
      LDA   cos_val
      STA   MATH_MUL16_B_LO
      ORA   #$7F
      BMI   @cos_neg
      LDA   #$00
      BRA   @cos_ext
@cos_neg:
      LDA   #$FF
@cos_ext:
      STA   MATH_MUL16_B_HI    ; triggers multiply

      ; raw_dy = round(result / 128)
      CLC
      LDA   MATH_RES0
      ADC   #64
      STA   tmp2
      LDA   MATH_RES1
      ADC   #0
      STA   tmp3
      LDA   MATH_RES2
      ADC   #0
      STA   tmp1
      LDA   tmp2
      ASL
      LDA   tmp3
      ROL
      STA   tmp2                ; raw_dy lo
      LDA   tmp1
      ROL
      STA   tmp3                ; raw_dy hi

      ; dy = -raw_dy (negate 16-bit)
      SEC
      LDA   #0
      SBC   tmp2
      STA   dy_lo
      LDA   #0
      SBC   tmp3
      STA   dy_hi

      ; Add dx to turtle X (24-bit: hi:lo:frac)
      ; dx is integer pixels, add to lo:hi (frac stays)
      CLC
      LDA   TURTLE_X_LO
      ADC   dx_lo
      STA   TURTLE_X_LO
      LDA   TURTLE_X_HI
      ADC   dx_hi
      STA   TURTLE_X_HI

      ; Add dy to turtle Y
      CLC
      LDA   TURTLE_Y_LO
      ADC   dy_lo
      STA   TURTLE_Y_LO
      LDA   TURTLE_Y_HI
      ADC   dy_hi
      STA   TURTLE_Y_HI

      ; If pen is down, draw line from old to new position
      LDA   TURTLE_PEN
      BNE   @skip_draw          ; pen up = $01, skip
      JSR   draw_line
@skip_draw:

      ; Redraw virtual sprite at the new position
      JSR   draw_turtle_sprite
      RTS

; =====================================================================
; ext_bk — move backward by distance (negate and forward)
; =====================================================================
ext_bk:
      ; Negate arg0 (16.8 two's complement)
      SEC
      LDA   #0
      SBC   EXT_ARG0_FRAC
      STA   EXT_ARG0_FRAC
      LDA   #0
      SBC   EXT_ARG0_LO
      STA   EXT_ARG0_LO
      LDA   #0
      SBC   EXT_ARG0_HI
      STA   EXT_ARG0_HI
      JMP   ext_fd

; =====================================================================
; ext_rt — turn right by degrees
; =====================================================================
ext_rt:
      ; heading = (heading + degrees) mod 360
      CLC
      LDA   TURTLE_HEADING_LO
      ADC   EXT_ARG0_LO
      STA   TURTLE_HEADING_LO
      LDA   TURTLE_HEADING_HI
      ADC   EXT_ARG0_HI
      STA   TURTLE_HEADING_HI

      ; Reduce mod 360
      JSR   heading_mod360

      ; Redraw virtual sprite with the new heading
      JSR   draw_turtle_sprite
      RTS

; =====================================================================
; ext_lt — turn left by degrees
; =====================================================================
ext_lt:
      ; heading = (heading - degrees) mod 360
      ; If result negative, add 360
      SEC
      LDA   TURTLE_HEADING_LO
      SBC   EXT_ARG0_LO
      STA   TURTLE_HEADING_LO
      LDA   TURTLE_HEADING_HI
      SBC   EXT_ARG0_HI
      STA   TURTLE_HEADING_HI

      ; If negative (hi bit 7 set), add 360
      BMI   @add360
      ; Also do mod 360 in case it's >=360
      JSR   heading_mod360
      JSR   draw_turtle_sprite
      RTS

@add360:
      CLC
      LDA   TURTLE_HEADING_LO
      ADC   #<360
      STA   TURTLE_HEADING_LO
      LDA   TURTLE_HEADING_HI
      ADC   #>360
      STA   TURTLE_HEADING_HI
      ; Could still be negative if turned more than 360, recurse
      BMI   @add360
      JSR   heading_mod360
      JSR   draw_turtle_sprite
      RTS

; =====================================================================
; ext_pu — pen up
; =====================================================================
ext_pu:
      LDA   #$01
      STA   TURTLE_PEN
      RTS

; =====================================================================
; ext_pd — pen down
; =====================================================================
ext_pd:
      STZ   TURTLE_PEN
      RTS

; =====================================================================
; ext_st — show turtle sprite
; =====================================================================
ext_st:
      LDA   #$01
      STA   TURTLE_SHOWN
      JMP   draw_turtle_sprite

; =====================================================================
; ext_ht — hide turtle sprite
; =====================================================================
ext_ht:
      JSR   erase_turtle_sprite
      STZ   TURTLE_SHOWN
      RTS

; =====================================================================
; ext_home — move to center, heading 0 (draw line if pen down)
; =====================================================================
ext_home:
      ; Ensure initialized
      LDA   TURTLE_INITED
      BNE   @go
      JSR   turtle_init
      JSR   draw_turtle_sprite
      RTS
@go:
      ; Save old position
      LDA   TURTLE_X_LO
      STA   old_x_lo
      LDA   TURTLE_X_HI
      STA   old_x_hi
      LDA   TURTLE_Y_LO
      STA   old_y_lo
      LDA   TURTLE_Y_HI
      STA   old_y_hi
      JSR   erase_turtle_sprite

      ; Move to center
      LDA   #TURTLE_CENTER_X
      STA   TURTLE_X_LO
      STZ   TURTLE_X_HI
      STZ   TURTLE_X_FRAC
      LDA   #TURTLE_CENTER_Y
      STA   TURTLE_Y_LO
      STZ   TURTLE_Y_HI
      STZ   TURTLE_Y_FRAC

      ; Heading = 0
      STZ   TURTLE_HEADING_LO
      STZ   TURTLE_HEADING_HI

      ; Draw line if pen down
      LDA   TURTLE_PEN
      BNE   @no_draw
      JSR   draw_line
@no_draw:
      JSR   draw_turtle_sprite
      RTS

; =====================================================================
; turtle_init — initialize turtle state and sprite
; =====================================================================
turtle_init:
      ; Position to center (160, 100) as 16.8
      LDA   #TURTLE_CENTER_X
      STA   TURTLE_X_LO
      STZ   TURTLE_X_HI
      STZ   TURTLE_X_FRAC
      LDA   #TURTLE_CENTER_Y
      STA   TURTLE_Y_LO
      STZ   TURTLE_Y_HI
      STZ   TURTLE_Y_FRAC
      ; Heading 0 (north)
      STZ   TURTLE_HEADING_LO
      STZ   TURTLE_HEADING_HI
      ; Pen down, shown, white
      STZ   TURTLE_PEN
      LDA   #$01
      STA   TURTLE_SHOWN
      LDA   #1                  ; white in Nova palette
      STA   TURTLE_COLOR
      STZ   TURTLE_SPRITE
      LDA   #$01
      STA   TURTLE_INITED

      LDX   #0
@copy_shape:
      LDA   turtle_shape_data,X
      STA   turtle_source_shape,X
      INX
      BNE   @copy_shape

      STZ   turtle_bg_saved
      RTS

; =====================================================================
; heading_mod360 — reduce TURTLE_HEADING to 0-359
; =====================================================================
heading_mod360:
@loop:
      ; If heading >= 360, subtract 360
      LDA   TURTLE_HEADING_HI
      BNE   @check_hi
      BRA   @done               ; hi = 0 is always < 360
@check_hi:
      ; hi > 1 → definitely >= 360 (since 360 = $0168)
      CMP   #>360               ; >360 hi = $01
      BNE   @subtract           ; hi > 1
      ; hi = 1, compare lo
      LDA   TURTLE_HEADING_LO
      CMP   #<360
      BCC   @done
@subtract:
      SEC
      LDA   TURTLE_HEADING_LO
      SBC   #<360
      STA   TURTLE_HEADING_LO
      LDA   TURTLE_HEADING_HI
      SBC   #>360
      STA   TURTLE_HEADING_HI
      BRA   @loop
@done:
      RTS

; =====================================================================
; draw_line — VGC LINE from old position to current position
; =====================================================================
draw_line:
      ; Set color
      JSR   vgc_wait_cmd
      LDA   TURTLE_COLOR
      STA   VGC_P0
      JSR   vgc_gcolor

      ; Draw line
      JSR   vgc_wait_cmd
      LDA   old_x_lo
      STA   VGC_P0
      LDA   old_x_hi
      STA   VGC_P1
      LDA   old_y_lo
      STA   VGC_P2
      LDA   old_y_hi
      STA   VGC_P3
      LDA   TURTLE_X_LO
      STA   VGC_P4
      LDA   TURTLE_X_HI
      STA   VGC_P5
      LDA   TURTLE_Y_LO
      STA   VGC_P6
      LDA   TURTLE_Y_HI
      STA   VGC_P7
      JSR   vgc_line
      RTS

; =====================================================================
; clear_gfx_plane — clear full graphics plane through the NDK VGC helper
; =====================================================================
clear_gfx_plane:
      JSR   vgc_wait_cmd
      JSR   vgc_gcls
      JMP   vgc_wait_cmd

; =====================================================================
; configure_turtle_vsprite — configure NDK virtual-sprite state
; =====================================================================
configure_turtle_vsprite:
      LDA   #TURTLE_SPR_SIZE
      STA   VSPRITE_WIDTHL
      STZ   VSPRITE_WIDTHH
      STA   VSPRITE_HEIGHTL
      STZ   VSPRITE_HEIGHTH
      STA   VSPRITE_ORIGSTRL
      STZ   VSPRITE_ORIGSTRH
      STA   VSPRITE_ROTSTRL
      STZ   VSPRITE_ROTSTRH
      STA   VSPRITE_BGSTRL
      STZ   VSPRITE_BGSTRH

      LDA   #BLT_SPACE_CPU
      STA   VSPRITE_ORIGSPACE
      STA   VSPRITE_ROTSPACE
      STA   VSPRITE_BGSPACE

      LDA   #<turtle_source_shape
      STA   VSPRITE_ORIGADDRL
      LDA   #>turtle_source_shape
      STA   VSPRITE_ORIGADDRM
      STZ   VSPRITE_ORIGADDRH

      LDA   #<turtle_rotated_shape
      STA   VSPRITE_ROTADDRL
      LDA   #>turtle_rotated_shape
      STA   VSPRITE_ROTADDRM
      STZ   VSPRITE_ROTADDRH

      LDA   #<turtle_saved_bg
      STA   VSPRITE_BGADDRL
      LDA   #>turtle_saved_bg
      STA   VSPRITE_BGADDRM
      STZ   VSPRITE_BGADDRH

      STZ   VSPRITE_COLORKEY
      LDA   #VSPRITE_FLAG_COLORKEY
      STA   VSPRITE_FLAGS
      RTS

; =====================================================================
; set_turtle_vsprite_pos — center the 16x16 sprite on the turtle position
; =====================================================================
set_turtle_vsprite_pos:
      SEC
      LDA   TURTLE_X_LO
      SBC   #TURTLE_SPR_HALF
      STA   tmp0
      LDA   TURTLE_X_HI
      SBC   #0
      STA   tmp1
      BMI   @x_zero
      CMP   #>TURTLE_MAX_X
      BCC   @x_ok
      BNE   @x_max
      LDA   tmp0
      CMP   #<TURTLE_MAX_X
      BCC   @x_ok
      BEQ   @x_ok
@x_max:
      LDA   #<TURTLE_MAX_X
      STA   VSPRITE_XL
      LDA   #>TURTLE_MAX_X
      STA   VSPRITE_XH
      BRA   @y
@x_zero:
      STZ   VSPRITE_XL
      STZ   VSPRITE_XH
      BRA   @y
@x_ok:
      LDA   tmp0
      STA   VSPRITE_XL
      LDA   tmp1
      STA   VSPRITE_XH

@y:
      SEC
      LDA   TURTLE_Y_LO
      SBC   #TURTLE_SPR_HALF
      STA   tmp0
      LDA   TURTLE_Y_HI
      SBC   #0
      BMI   @y_zero
      BNE   @y_max
      LDA   tmp0
      CMP   #(TURTLE_MAX_Y + 1)
      BCC   @y_ok
@y_max:
      LDA   #TURTLE_MAX_Y
      STA   VSPRITE_Y
      RTS
@y_zero:
      STZ   VSPRITE_Y
      RTS
@y_ok:
      LDA   tmp0
      STA   VSPRITE_Y
      RTS

; =====================================================================
; turtle_heading_u8 — convert Logo degrees to the NDK/blitter angle byte.
;   A = floor((heading * 256) / 360). Cardinal headings must land exactly:
;   90 -> 64, 180 -> 128, 270 -> 192.
; =====================================================================
turtle_heading_u8:
      STZ   MATH_DIV_N_LO
      LDA   TURTLE_HEADING_LO
      STA   MATH_DIV_N_1
      LDA   TURTLE_HEADING_HI
      STA   MATH_DIV_N_2
      STZ   MATH_DIV_N_HI
      LDA   #<360
      STA   MATH_DIV_D_LO
      LDA   #>360
      STA   MATH_DIV_D_HI
      LDA   MATH_RES0
      RTS

; =====================================================================
; erase_turtle_sprite — restore the background saved by the NDK vsprite path
; =====================================================================
erase_turtle_sprite:
      LDA   turtle_bg_saved
      BEQ   @done
      JSR   configure_turtle_vsprite
      LDA   turtle_bg_x_lo
      STA   VSPRITE_XL
      LDA   turtle_bg_x_hi
      STA   VSPRITE_XH
      LDA   turtle_bg_y
      STA   VSPRITE_Y
      JSR   vsprite_gfx_restore_bg
      STZ   turtle_bg_saved
@done:
      RTS

; =====================================================================
; draw_turtle_sprite — draw rotated NDK virtual sprite
; =====================================================================
draw_turtle_sprite:
      LDA   TURTLE_SHOWN
      BEQ   @done
      JSR   erase_turtle_sprite
      JSR   configure_turtle_vsprite
      JSR   set_turtle_vsprite_pos
      LDA   VSPRITE_XL
      STA   turtle_bg_x_lo
      LDA   VSPRITE_XH
      STA   turtle_bg_x_hi
      LDA   VSPRITE_Y
      STA   turtle_bg_y
      JSR   vsprite_gfx_save_bg
      CMP   #VSPRITE_RESULT_OK
      BNE   @done
      LDA   #$01
      STA   turtle_bg_saved
      JSR   turtle_heading_u8
      STA   VSPRITE_ROTANGLE
      JSR   vsprite_gfx_rotate_blit_keyed
@done:
      RTS

; =====================================================================
; setup_copper — copper split for gfx+sprites top, text bottom
; =====================================================================
setup_copper:
      STZ   VGC_P0
      LDA   #SPLIT_Y
      STA   VGC_P1
      LDA   #MODE_GFX_SPRITES
      STA   VGC_P2
      LDA   #MODE_TEXT_ONLY
      STA   VGC_P3
      JMP   copper_split_mode

; =====================================================================
; wait_vgc — wait until VGC is not busy
; =====================================================================
wait_vgc:
      JMP   vgc_wait_cmd

; =====================================================================
; ext_ts — TEXTSCREEN: full text mode, disable copper, mode=0
; =====================================================================
ext_ts:
      JSR   vgc_wait_cmd
      JSR   copper_off
      STZ   VGC_MODE
      RTS

; =====================================================================
; ext_ss — SPLITSCREEN: copper split (gfx+sprites top, text bottom)
; =====================================================================
ext_ss:
      JSR   setup_copper
      JSR   prepare_split_text
      RTS

; =====================================================================
; ext_fs — FULLSCREEN: full graphics mode, no text area
; =====================================================================
ext_fs:
      ; Disable copper first
      JSR   wait_vgc
      LDA   #VCMD_COPPERDIS
      STA   VGC_CMD
      ; Set mode to graphics + sprites
      LDA   #MODE_GFX_SPRITES
      STA   VGC_MODE
      RTS

; =====================================================================
; prepare_split_text — keep Logo's prompt in the visible split text area
; =====================================================================
prepare_split_text:
      LDA   #$0C
      STA   VGC_CHAROUT
      JSR   wait_vgc
      STZ   TEXTWIN_LEFT
      LDA   #SPLIT_TEXT_ROW
      STA   TEXTWIN_TOP
      LDA   #80
      STA   TEXTWIN_WIDTH
      LDA   #SPLIT_TEXT_HEIGHT
      STA   TEXTWIN_HEIGHT
      STZ   VGC_CURSX
      LDA   #SPLIT_TEXT_ROW
      STA   VGC_CURSY
      STZ   VGC_CURSEN
      RTS

; =====================================================================
; ext_setxy — move to (x, y). Draw line if pen down.
;   ARG0 = x, ARG1 = y
; =====================================================================
ext_setxy:
      ; Ensure turtle is initialized
      LDA   TURTLE_INITED
      BNE   @go
      JSR   turtle_init
@go:
      ; Save old position for line drawing
      LDA   TURTLE_X_LO
      STA   old_x_lo
      LDA   TURTLE_X_HI
      STA   old_x_hi
      LDA   TURTLE_Y_LO
      STA   old_y_lo
      LDA   TURTLE_Y_HI
      STA   old_y_hi
      JSR   erase_turtle_sprite

      ; Set new position
      LDA   EXT_ARG0_LO
      STA   TURTLE_X_LO
      LDA   EXT_ARG0_HI
      STA   TURTLE_X_HI
      STZ   TURTLE_X_FRAC
      LDA   EXT_ARG1_LO
      STA   TURTLE_Y_LO
      LDA   EXT_ARG1_HI
      STA   TURTLE_Y_HI
      STZ   TURTLE_Y_FRAC

      ; Draw line if pen down
      LDA   TURTLE_PEN
      BNE   @skip_draw
      JSR   draw_line
@skip_draw:
      JSR   draw_turtle_sprite
      RTS

; =====================================================================
; ext_setpos — move to position from a Logo list. Draw line if pen down.
;   ARG0 = [x y]
; =====================================================================
ext_setpos:
      LDA   EXT_ARG0_TYPE
      CMP   #VAL_LIST
      BNE   @done

      LDA   EXT_ARG0_LO
      STA   tmp0
      LDA   EXT_ARG0_HI
      STA   tmp1
      ORA   tmp0
      BEQ   @done

      ; First list element is X.
      LDY   #CONS_CAR_TYPE
      LDA   (tmp0),Y
      CMP   #VAL_NUMBER
      BNE   @done
      LDY   #CONS_CAR_LO
      LDA   (tmp0),Y
      STA   EXT_ARG0_LO
      LDY   #CONS_CAR_HI
      LDA   (tmp0),Y
      STA   EXT_ARG0_HI
      STZ   EXT_ARG0_FRAC

      ; Second list element is Y.
      LDY   #CONS_CDR_LO
      LDA   (tmp0),Y
      STA   tmp2
      LDY   #CONS_CDR_HI
      LDA   (tmp0),Y
      STA   tmp3
      ORA   tmp2
      BEQ   @done

      LDA   tmp2
      STA   tmp0
      LDA   tmp3
      STA   tmp1
      LDY   #CONS_CAR_TYPE
      LDA   (tmp0),Y
      CMP   #VAL_NUMBER
      BNE   @done
      LDY   #CONS_CAR_LO
      LDA   (tmp0),Y
      STA   EXT_ARG1_LO
      LDY   #CONS_CAR_HI
      LDA   (tmp0),Y
      STA   EXT_ARG1_HI
      STZ   EXT_ARG1_FRAC

      JMP   ext_setxy

@done:
      RTS

; =====================================================================
; ext_setx — set X only. Draw line if pen down.
;   ARG0 = x
; =====================================================================
ext_setx:
      LDA   TURTLE_INITED
      BNE   @go
      JSR   turtle_init
@go:
      ; Save old position
      LDA   TURTLE_X_LO
      STA   old_x_lo
      LDA   TURTLE_X_HI
      STA   old_x_hi
      LDA   TURTLE_Y_LO
      STA   old_y_lo
      LDA   TURTLE_Y_HI
      STA   old_y_hi
      JSR   erase_turtle_sprite

      ; Update X only
      LDA   EXT_ARG0_LO
      STA   TURTLE_X_LO
      LDA   EXT_ARG0_HI
      STA   TURTLE_X_HI
      STZ   TURTLE_X_FRAC

      ; Draw line if pen down
      LDA   TURTLE_PEN
      BNE   @skip_draw
      JSR   draw_line
@skip_draw:
      JSR   draw_turtle_sprite
      RTS

; =====================================================================
; ext_sety — set Y only. Draw line if pen down.
;   ARG0 = y
; =====================================================================
ext_sety:
      LDA   TURTLE_INITED
      BNE   @go
      JSR   turtle_init
@go:
      ; Save old position
      LDA   TURTLE_X_LO
      STA   old_x_lo
      LDA   TURTLE_X_HI
      STA   old_x_hi
      LDA   TURTLE_Y_LO
      STA   old_y_lo
      LDA   TURTLE_Y_HI
      STA   old_y_hi
      JSR   erase_turtle_sprite

      ; Update Y only
      LDA   EXT_ARG0_LO
      STA   TURTLE_Y_LO
      LDA   EXT_ARG0_HI
      STA   TURTLE_Y_HI
      STZ   TURTLE_Y_FRAC

      ; Draw line if pen down
      LDA   TURTLE_PEN
      BNE   @skip_draw
      JSR   draw_line
@skip_draw:
      JSR   draw_turtle_sprite
      RTS

; =====================================================================
; ext_seth — set heading to degrees. Normalize mod 360.
;   ARG0 = degrees
; =====================================================================
ext_seth:
      LDA   EXT_ARG0_LO
      STA   TURTLE_HEADING_LO
      LDA   EXT_ARG0_HI
      STA   TURTLE_HEADING_HI

      ; Handle negative heading by adding 360 until positive
      LDA   TURTLE_HEADING_HI
      BMI   @add360
      JSR   heading_mod360
      JMP   @update
@add360:
      CLC
      LDA   TURTLE_HEADING_LO
      ADC   #<360
      STA   TURTLE_HEADING_LO
      LDA   TURTLE_HEADING_HI
      ADC   #>360
      STA   TURTLE_HEADING_HI
      BMI   @add360
      JSR   heading_mod360
@update:
      JSR   draw_turtle_sprite
      RTS

; =====================================================================
; ext_xcor — return turtle X position (integer part)
; =====================================================================
ext_xcor:
      LDA   TURTLE_X_LO
      STA   EXT_RESULT_LO
      LDA   TURTLE_X_HI
      STA   EXT_RESULT_HI
      STZ   EXT_RESULT_FRAC
      STZ   EXT_RESULT_TYPE      ; VAL_NUMBER
      RTS

; =====================================================================
; ext_ycor — return turtle Y position (integer part)
; =====================================================================
ext_ycor:
      LDA   TURTLE_Y_LO
      STA   EXT_RESULT_LO
      LDA   TURTLE_Y_HI
      STA   EXT_RESULT_HI
      STZ   EXT_RESULT_FRAC
      STZ   EXT_RESULT_TYPE      ; VAL_NUMBER
      RTS

; =====================================================================
; ext_heading — return current heading in degrees
; =====================================================================
ext_heading:
      LDA   TURTLE_HEADING_LO
      STA   EXT_RESULT_LO
      LDA   TURTLE_HEADING_HI
      STA   EXT_RESULT_HI
      STZ   EXT_RESULT_FRAC
      STZ   EXT_RESULT_TYPE      ; VAL_NUMBER
      RTS

; =====================================================================
; ext_pendownp — return 1 if pen is down, 0 if up
; =====================================================================
ext_pendownp:
      LDA   TURTLE_PEN
      BNE   @up
      ; Pen down ($00) → return 1
      LDA   #1
      STA   EXT_RESULT_LO
      BRA   @done
@up:
      ; Pen up ($01) → return 0
      STZ   EXT_RESULT_LO
@done:
      STZ   EXT_RESULT_HI
      STZ   EXT_RESULT_FRAC
      STZ   EXT_RESULT_TYPE
      RTS

; =====================================================================
; ext_shownp — return 1 if turtle shown, 0 if hidden
; =====================================================================
ext_shownp:
      LDA   TURTLE_SHOWN
      STA   EXT_RESULT_LO
      STZ   EXT_RESULT_HI
      STZ   EXT_RESULT_FRAC
      STZ   EXT_RESULT_TYPE
      RTS

; =====================================================================
; ext_setpc — set pen color (palette index)
;   ARG0 = color
; =====================================================================
ext_setpc:
      LDA   EXT_ARG0_LO
      STA   TURTLE_COLOR
      RTS

; =====================================================================
; ext_setbg — set background color (VGC background)
;   ARG0 = color
; =====================================================================
ext_setbg:
      JSR   erase_turtle_sprite
      LDA   EXT_ARG0_LO
      STA   VGC_BGCOL
      ; Also clear graphics plane to this color
      JSR   wait_vgc
      LDA   EXT_ARG0_LO
      STA   VGC_P0
      LDA   #VCMD_GCLS
      STA   VGC_CMD
      JSR   wait_vgc
      STZ   turtle_bg_saved
      JMP   draw_turtle_sprite

; =====================================================================
; ext_towards — return heading angle towards point (x, y)
;   ARG0 = target_x, ARG1 = target_y
;   Uses ATAN2 coprocessor
; =====================================================================
ext_towards:
      ; dx = target_x - turtle_x (signed 16-bit)
      SEC
      LDA   EXT_ARG0_LO
      SBC   TURTLE_X_LO
      STA   MATH_ATAN_DX_LO
      LDA   EXT_ARG0_HI
      SBC   TURTLE_X_HI
      STA   MATH_ATAN_DX_HI

      ; dy = -(target_y - turtle_y) (negate because screen Y is inverted)
      SEC
      LDA   TURTLE_Y_LO
      SBC   EXT_ARG1_LO
      STA   MATH_ATAN_DY_LO
      LDA   TURTLE_Y_HI
      SBC   EXT_ARG1_HI
      STA   MATH_ATAN_DY_HI      ; writing DY_HI triggers ATAN2

      ; Result in MATH_RES0 is a u8 angle (0-255)
      ; Convert to degrees: degrees = u8 * 360 / 256 ≈ u8 * 45 / 32
      ; Or more precisely: u8 * 360 / 256 = u8 * 1.40625
      ; Use MUL16: u8 * 360, then take hi byte of result (>>8)
      LDA   MATH_RES0
      STA   MATH_MUL16_A_LO
      STZ   MATH_MUL16_A_HI
      LDA   #<360
      STA   MATH_MUL16_B_LO
      LDA   #>360
      STA   MATH_MUL16_B_HI      ; triggers multiply

      ; Result is u8*360, which is 16-bit. >> 8 = MATH_RES1
      LDA   MATH_RES1
      STA   EXT_RESULT_LO
      LDA   MATH_RES2
      STA   EXT_RESULT_HI
      STZ   EXT_RESULT_FRAC
      STZ   EXT_RESULT_TYPE
      RTS

; =====================================================================
; ext_setcolor — set VGC drawing color
;   ARG0 = color (palette index 0-15)
; =====================================================================
ext_setcolor:
      JSR   wait_vgc
      LDA   EXT_ARG0_LO
      STA   VGC_P0
      LDA   #VCMD_GCOLOR
      STA   VGC_CMD
      RTS

; =====================================================================
; ext_plot — plot a pixel at (x, y) in current draw color
;   ARG0 = x, ARG1 = y
; =====================================================================
ext_plot:
      JSR   erase_turtle_sprite
      JSR   wait_vgc
      LDA   EXT_ARG0_LO
      STA   VGC_P0
      LDA   EXT_ARG0_HI
      STA   VGC_P1
      LDA   EXT_ARG1_LO
      STA   VGC_P2
      LDA   EXT_ARG1_HI
      STA   VGC_P3
      LDA   #VCMD_PLOT
      STA   VGC_CMD
      JSR   wait_vgc
      JMP   draw_turtle_sprite

; =====================================================================
; ext_unplot — clear a pixel at (x, y)
;   ARG0 = x, ARG1 = y
; =====================================================================
ext_unplot:
      JSR   erase_turtle_sprite
      JSR   wait_vgc
      LDA   EXT_ARG0_LO
      STA   VGC_P0
      LDA   EXT_ARG0_HI
      STA   VGC_P1
      LDA   EXT_ARG1_LO
      STA   VGC_P2
      LDA   EXT_ARG1_HI
      STA   VGC_P3
      LDA   #VCMD_UNPLOT
      STA   VGC_CMD
      JSR   wait_vgc
      JMP   draw_turtle_sprite

; =====================================================================
; ext_line — draw line from (x1,y1) to (x2,y2)
;   ARG0 = x1, ARG1 = y1, ARG2 = x2, ARG3 = y2
; =====================================================================
ext_line:
      JSR   erase_turtle_sprite
      JSR   wait_vgc
      LDA   EXT_ARG0_LO
      STA   VGC_P0
      LDA   EXT_ARG0_HI
      STA   VGC_P1
      LDA   EXT_ARG1_LO
      STA   VGC_P2
      LDA   EXT_ARG1_HI
      STA   VGC_P3
      LDA   EXT_ARG2_LO
      STA   VGC_P4
      LDA   EXT_ARG2_HI
      STA   VGC_P5
      LDA   EXT_ARG3_LO
      STA   VGC_P6
      LDA   EXT_ARG3_HI
      STA   VGC_P7
      LDA   #VCMD_LINE
      STA   VGC_CMD
      JSR   wait_vgc
      JMP   draw_turtle_sprite

; =====================================================================
; ext_circle — draw circle at (x, y) with radius r
;   ARG0 = x, ARG1 = y, ARG2 = r
; =====================================================================
ext_circle:
      JSR   erase_turtle_sprite
      JSR   wait_vgc
      LDA   EXT_ARG0_LO
      STA   VGC_P0
      LDA   EXT_ARG0_HI
      STA   VGC_P1
      LDA   EXT_ARG1_LO
      STA   VGC_P2
      LDA   EXT_ARG1_HI
      STA   VGC_P3
      LDA   EXT_ARG2_LO
      STA   VGC_P4
      LDA   EXT_ARG2_HI
      STA   VGC_P5
      STZ   VGC_P6              ; ry_lo = 0 → use rx as both radii
      STZ   VGC_P7              ; ry_hi = 0
      LDA   #VCMD_CIRCLE
      STA   VGC_CMD
      JSR   wait_vgc
      JMP   draw_turtle_sprite

; =====================================================================
; ext_rect — draw rectangle outline from (x1,y1) to (x2,y2)
;   ARG0 = x1, ARG1 = y1, ARG2 = x2, ARG3 = y2
; =====================================================================
ext_rect:
      JSR   erase_turtle_sprite
      JSR   wait_vgc
      LDA   EXT_ARG0_LO
      STA   VGC_P0
      LDA   EXT_ARG0_HI
      STA   VGC_P1
      LDA   EXT_ARG1_LO
      STA   VGC_P2
      LDA   EXT_ARG1_HI
      STA   VGC_P3
      LDA   EXT_ARG2_LO
      STA   VGC_P4
      LDA   EXT_ARG2_HI
      STA   VGC_P5
      LDA   EXT_ARG3_LO
      STA   VGC_P6
      LDA   EXT_ARG3_HI
      STA   VGC_P7
      LDA   #VCMD_RECT
      STA   VGC_CMD
      JSR   wait_vgc
      JMP   draw_turtle_sprite

; =====================================================================
; ext_fillrect — draw filled rectangle from (x1,y1) to (x2,y2)
;   ARG0 = x1, ARG1 = y1, ARG2 = x2, ARG3 = y2
; =====================================================================
ext_fillrect:
      JSR   erase_turtle_sprite
      JSR   wait_vgc
      LDA   EXT_ARG0_LO
      STA   VGC_P0
      LDA   EXT_ARG0_HI
      STA   VGC_P1
      LDA   EXT_ARG1_LO
      STA   VGC_P2
      LDA   EXT_ARG1_HI
      STA   VGC_P3
      LDA   EXT_ARG2_LO
      STA   VGC_P4
      LDA   EXT_ARG2_HI
      STA   VGC_P5
      LDA   EXT_ARG3_LO
      STA   VGC_P6
      LDA   EXT_ARG3_HI
      STA   VGC_P7
      LDA   #VCMD_FILL
      STA   VGC_CMD
      JSR   wait_vgc
      JMP   draw_turtle_sprite

; =====================================================================
; ext_paint — flood fill at (x, y) with current draw color
;   ARG0 = x, ARG1 = y
; =====================================================================
ext_paint:
      JSR   erase_turtle_sprite
      JSR   wait_vgc
      LDA   EXT_ARG0_LO
      STA   VGC_P0
      LDA   EXT_ARG0_HI
      STA   VGC_P1
      LDA   EXT_ARG1_LO
      STA   VGC_P2
      LDA   EXT_ARG1_HI
      STA   VGC_P3
      LDA   #VCMD_PAINT
      STA   VGC_CMD
      JSR   wait_vgc
      JMP   draw_turtle_sprite

; =====================================================================
; ext_sprite — SPRITE n x y: position and enable sprite N
;   ARG0 = slot, ARG1 = x (16-bit), ARG2 = y (16-bit)
; =====================================================================
ext_sprite:
      ; SPRPOS: P0=slot, P1=x_lo, P2=x_hi, P3=y_lo, P4=y_hi
      JSR   wait_vgc
      LDA   EXT_ARG0_LO
      STA   VGC_P0
      LDA   EXT_ARG1_LO
      STA   VGC_P1
      LDA   EXT_ARG1_HI
      STA   VGC_P2
      LDA   EXT_ARG2_LO
      STA   VGC_P3
      LDA   EXT_ARG2_HI
      STA   VGC_P4
      LDA   #VCMD_SPRPOS
      STA   VGC_CMD
      ; SPRENA: P0=slot
      JSR   wait_vgc
      LDA   EXT_ARG0_LO
      STA   VGC_P0
      LDA   #VCMD_SPRENA
      STA   VGC_CMD
      RTS

; =====================================================================
; ext_spritepos — SPRITEPOS n x y: reposition sprite N
;   ARG0 = slot, ARG1 = x, ARG2 = y
; =====================================================================
ext_spritepos:
      JSR   wait_vgc
      LDA   EXT_ARG0_LO
      STA   VGC_P0
      LDA   EXT_ARG1_LO
      STA   VGC_P1
      LDA   EXT_ARG1_HI
      STA   VGC_P2
      LDA   EXT_ARG2_LO
      STA   VGC_P3
      LDA   EXT_ARG2_HI
      STA   VGC_P4
      LDA   #VCMD_SPRPOS
      STA   VGC_CMD
      RTS

; =====================================================================
; ext_spriteon — SPRITEON n: enable sprite N
;   ARG0 = slot
; =====================================================================
ext_spriteon:
      JSR   wait_vgc
      LDA   EXT_ARG0_LO
      STA   VGC_P0
      LDA   #VCMD_SPRENA
      STA   VGC_CMD
      RTS

; =====================================================================
; ext_spriteoff — SPRITEOFF n: disable sprite N
;   ARG0 = slot
; =====================================================================
ext_spriteoff:
      JSR   wait_vgc
      LDA   EXT_ARG0_LO
      STA   VGC_P0
      LDA   #VCMD_SPRDIS
      STA   VGC_CMD
      RTS

; =====================================================================
; ext_sprcollp — SPRITECOLLISION? n: return 1 if sprite N collided
;   ARG0 = slot (0-15)
;   Reads VGC_COLLST (bits 0-7) / VGC_COLLST_HI (bits 8-15)
; =====================================================================
ext_sprcollp:
      LDA   EXT_ARG0_LO
      CMP   #8
      BCS   @hi_byte
      ; Sprite 0-7: read low byte, test bit
      TAX
      LDA   VGC_COLLST
      BRA   @test_bit
@hi_byte:
      ; Sprite 8-15: read high byte, adjust bit index
      SEC
      SBC   #8
      TAX
      LDA   VGC_COLLST_HI
@test_bit:
      ; Shift right X times to get bit into carry
      CPX   #0
      BEQ   @check
@shift:
      LSR
      DEX
      BNE   @shift
@check:
      AND   #$01
      STA   EXT_RESULT_LO
      STZ   EXT_RESULT_HI
      STZ   EXT_RESULT_FRAC
      STZ   EXT_RESULT_TYPE
      RTS

; =====================================================================
; ext_tone — TONE freq dur: play a tone on SID voice 0
;   ARG0 = frequency (16-bit SID freq word), ARG1 = duration (frames)
; =====================================================================
ext_tone:
      ; Set SID master volume to max
      LDA   #$0F
      STA   SID_BASE + $18         ; $D418 filter/volume, bits 0-3 = volume
      ; Set frequency
      LDA   EXT_ARG0_LO
      STA   SID_BASE + $00         ; freq lo
      LDA   EXT_ARG0_HI
      STA   SID_BASE + $01         ; freq hi
      ; Set ADSR: attack=0, decay=9, sustain=$A, release=0
      LDA   #$09
      STA   SID_BASE + $05         ; attack/decay
      LDA   #$A0
      STA   SID_BASE + $06         ; sustain/release
      ; Gate on with sawtooth waveform
      LDA   #$21                   ; sawtooth ($20) + gate ($01)
      STA   SID_BASE + $04         ; control register
      ; Wait duration frames
      LDA   EXT_ARG1_LO
      JSR   wait_frames
      ; Gate off
      LDA   #$20                   ; sawtooth, no gate
      STA   SID_BASE + $04
      RTS

; =====================================================================
; ext_noise — NOISE dur: noise burst on SID voice 0
;   ARG0 = duration (frames)
; =====================================================================
ext_noise:
      ; Set SID master volume to max
      LDA   #$0F
      STA   SID_BASE + $18
      ; Set a mid-range frequency for noise
      LDA   #$00
      STA   SID_BASE + $00
      LDA   #$20
      STA   SID_BASE + $01
      ; ADSR: fast attack, medium sustain
      LDA   #$09
      STA   SID_BASE + $05
      LDA   #$A0
      STA   SID_BASE + $06
      ; Gate on with noise waveform
      LDA   #$81                   ; noise ($80) + gate ($01)
      STA   SID_BASE + $04
      ; Wait duration frames
      LDA   EXT_ARG0_LO
      JSR   wait_frames
      ; Gate off
      LDA   #$80                   ; noise, no gate
      STA   SID_BASE + $04
      RTS

; =====================================================================
; ext_volume — VOLUME vol: set SID master volume (0-15)
;   ARG0 = volume
; =====================================================================
ext_volume:
      LDA   EXT_ARG0_LO
      AND   #$0F                   ; clamp to 0-15
      STA   SID_BASE + $18
      RTS

; =====================================================================
; ext_wait — WAIT n: wait N video frames
;   ARG0 = count
; =====================================================================
ext_wait:
      LDA   EXT_ARG0_LO
      JSR   wait_frames
      RTS

; =====================================================================
; ext_waitvbl — WAITVBL: wait for next vertical blank (1 frame)
; =====================================================================
ext_waitvbl:
      LDA   VGC_FRAME
@w:   CMP   VGC_FRAME
      BEQ   @w
      RTS

; =====================================================================
; ext_timer — TIMER: return current frame counter (8-bit)
; =====================================================================
ext_timer:
      LDA   VGC_FRAME
      STA   EXT_RESULT_LO
      STZ   EXT_RESULT_HI
      STZ   EXT_RESULT_FRAC
      STZ   EXT_RESULT_TYPE
      RTS

; =====================================================================
; wait_frames — wait A video frames. Reused by TONE, NOISE, WAIT.
; =====================================================================
wait_frames:
      TAX
      BEQ   @done
      LDA   VGC_FRAME
@wait:
      CMP   VGC_FRAME
      BEQ   @wait
      LDA   VGC_FRAME
      DEX
      BNE   @wait
@done:
      RTS

; =====================================================================
; RODATA segment — turtle sprite shape data
; =====================================================================
      .segment "RODATA"

; 16x16 turtle arrow shape pointing UP (north), matching the NDK turtle demo.
turtle_shape_data:
.repeat 16, yy
  .repeat 16, xx
    .if yy < 1
      .byte 0
    .elseif yy > 14
      .byte 0
    .elseif yy = 14
      .if xx >= 1 && xx <= 5
        .byte COL_RED
      .elseif xx >= 10 && xx <= 14
        .byte COL_GREEN
      .elseif xx >= 1 && xx <= 14
        .byte COL_WHITE
      .else
        .byte 0
      .endif
    .elseif xx = (8 - ((((yy - 1) * 7) + 6) / 13))
      .if yy >= 9
        .byte COL_RED
      .else
        .byte COL_WHITE
      .endif
    .elseif xx = (8 + ((((yy - 1) * 6) + 6) / 13))
      .if yy >= 9
        .byte COL_GREEN
      .else
        .byte COL_WHITE
      .endif
    .else
      .byte 0
    .endif
  .endrepeat
.endrepeat

      .include "vgc.s"
      .include "copper.s"
      .include "copper_split.s"
      .include "vsprite.s"

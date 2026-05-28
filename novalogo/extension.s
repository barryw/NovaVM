; extension.s — NovaLogo extension ROM
; Loaded at $C000 when swapped via REG_ROMSWAP = ROMSWAP_EXTENSION ($04).
;
; Entry point at $C000: reads EXT_CMD from zero page and dispatches.
; Handlers receive pre-evaluated arguments in EXT_ARG0..2 and return
; results in EXT_RESULT. No callbacks into the base ROM interpreter.

      .include "nova.inc"
      .include "ext_iface.inc"

; --- Fixed BSS addresses (extension has no linker BSS segment) ---
TURTLE_SHAPE_BUF  = $7E00      ; 256 bytes: source shape copied from RODATA
TURTLE_ROT_BUF    = $7F00      ; 256 bytes: rotated shape output

; --- Sprite configuration ---
TURTLE_SPR_SLOT   = 15
TURTLE_SPR_SIZE   = 16
TURTLE_CENTER_X   = 160        ; center of 320-wide graphics
TURTLE_CENTER_Y   = 100        ; center of 200-high graphics
SPLIT_Y           = 160        ; copper split: gfx rows 0-159, text below
MODE_GFX_SPRITES  = 3          ; VGC mode: graphics + sprites
MODE_TEXT_ONLY    = 0

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

      .segment "CODE"

; =====================================================================
; Extension entry point — called via JSR $C000 from RAM trampoline
; =====================================================================
ext_entry:
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
      ; Clear graphics plane
      JSR   wait_vgc
      STZ   VGC_P0              ; color 0 = black
      LDA   #VCMD_GCLS
      STA   VGC_CMD

      ; Set up copper split: graphics+sprites top, text bottom
      JSR   setup_copper

      ; Initialize turtle state
      JSR   turtle_init

      ; Show turtle sprite
      JSR   update_sprite_pos
      JSR   update_sprite_rotation

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

      ; Convert heading (degrees 0-359) to u8 angle (0-255)
      ; u8_angle = heading * 256 / 360 ≈ heading * 91 / 128
      ; Use MUL16: heading * 91
      LDA   TURTLE_HEADING_LO
      STA   MATH_MUL16_A_LO
      LDA   TURTLE_HEADING_HI
      STA   MATH_MUL16_A_HI
      LDA   #91
      STA   MATH_MUL16_B_LO
      STZ   MATH_MUL16_B_HI    ; triggers multiply

      ; u8_angle = byte 1 of result (effectively >> 7, close to /128)
      ; Actually: (heading*91) / 128 = result >> 7
      ; Result byte 0 >> 7 gives carry bit, byte 1 = main value
      LDA   MATH_RES0
      ASL                       ; bit 7 of byte 0 → carry
      LDA   MATH_RES1
      ROL                       ; result / 128 with rounding bit
      STA   tmp0                ; u8_angle

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

      ; dx = result / 128 (shift right 7, preserving sign)
      LDA   MATH_RES0
      ASL                       ; bit 7 → carry
      LDA   MATH_RES1
      ROL
      STA   dx_lo
      LDA   MATH_RES2
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

      ; raw_dy = result / 128
      LDA   MATH_RES0
      ASL
      LDA   MATH_RES1
      ROL
      STA   tmp2                ; raw_dy lo
      LDA   MATH_RES2
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

      ; Update sprite position
      JSR   update_sprite_pos
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

      ; Update sprite rotation
      JSR   update_sprite_rotation
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
      JSR   update_sprite_rotation
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
      JSR   update_sprite_rotation
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
      ; Enable hardware sprite
      JSR   wait_vgc
      LDA   TURTLE_SPRITE
      STA   VGC_P0
      LDA   #VCMD_SPRENA
      STA   VGC_CMD
      RTS

; =====================================================================
; ext_ht — hide turtle sprite
; =====================================================================
ext_ht:
      STZ   TURTLE_SHOWN
      ; Disable hardware sprite
      JSR   wait_vgc
      LDA   TURTLE_SPRITE
      STA   VGC_P0
      LDA   #VCMD_SPRDIS
      STA   VGC_CMD
      RTS

; =====================================================================
; ext_home — move to center, heading 0 (draw line if pen down)
; =====================================================================
ext_home:
      ; Ensure initialized
      LDA   TURTLE_INITED
      BNE   @go
      JSR   turtle_init
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
      JSR   update_sprite_pos
      JSR   update_sprite_rotation
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
      LDA   #TURTLE_SPR_SLOT
      STA   TURTLE_SPRITE
      LDA   #$01
      STA   TURTLE_INITED

      ; Copy source shape from RODATA to RAM buffer
      LDX   #0
@copy_shape:
      LDA   turtle_shape_data,X
      STA   TURTLE_SHAPE_BUF,X
      INX
      BNE   @copy_shape         ; 256 bytes

      ; Define hardware sprite shape from the source buffer
      JSR   define_sprite_shape

      ; Position and enable sprite
      JSR   update_sprite_pos

      ; Enable it
      JSR   wait_vgc
      LDA   TURTLE_SPRITE
      STA   VGC_P0
      LDA   #VCMD_SPRENA
      STA   VGC_CMD

      RTS

; =====================================================================
; heading_mod360 — reduce TURTLE_HEADING to 0-359
; =====================================================================
heading_mod360:
@loop:
      ; If heading >= 360, subtract 360
      LDA   TURTLE_HEADING_HI
      BNE   @check_hi
      LDA   TURTLE_HEADING_LO
      CMP   #<360               ; 360 = $0168
      BCC   @done               ; < 360, done
      BRA   @subtract
@check_hi:
      ; hi > 1 → definitely >= 360 (since 360 = $0168)
      CMP   #>360               ; >360 hi = $01
      BCC   @done               ; hi = 0, already handled above
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
      JSR   wait_vgc
      LDA   TURTLE_COLOR
      STA   VGC_P0
      LDA   #VCMD_GCOLOR
      STA   VGC_CMD

      ; Draw line
      JSR   wait_vgc
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
      LDA   #VCMD_LINE
      STA   VGC_CMD
      RTS

; =====================================================================
; update_sprite_pos — position the turtle hardware sprite
; =====================================================================
update_sprite_pos:
      LDA   TURTLE_SHOWN
      BEQ   @done
      JSR   wait_vgc
      LDA   TURTLE_SPRITE
      STA   VGC_P0
      ; Sprite X needs centering offset: sprite is 16px wide, center at X-8
      SEC
      LDA   TURTLE_X_LO
      SBC   #8
      STA   VGC_P1
      LDA   TURTLE_X_HI
      SBC   #0
      STA   VGC_P2
      ; Sprite Y: center at Y-8
      SEC
      LDA   TURTLE_Y_LO
      SBC   #8
      STA   VGC_P3
      LDA   TURTLE_Y_HI
      SBC   #0
      STA   VGC_P4
      LDA   #VCMD_SPRPOS
      STA   VGC_CMD
@done:
      RTS

; =====================================================================
; update_sprite_rotation — rotate sprite shape and update
; Uses the blitter hardware rotation
; =====================================================================
update_sprite_rotation:
      LDA   TURTLE_SHOWN
      BEQ   @done

      ; Convert heading to u8 angle for the blitter
      ; u8 = heading * 91 / 128
      LDA   TURTLE_HEADING_LO
      STA   MATH_MUL16_A_LO
      LDA   TURTLE_HEADING_HI
      STA   MATH_MUL16_A_HI
      LDA   #91
      STA   MATH_MUL16_B_LO
      STZ   MATH_MUL16_B_HI    ; triggers
      LDA   MATH_RES0
      ASL
      LDA   MATH_RES1
      ROL
      STA   tmp0                ; u8 angle

      ; Use blitter rotate: src=TURTLE_SHAPE_BUF, dst=TURTLE_ROT_BUF
      ; Configure blitter for rotation
      LDA   #BLT_SPACE_CPU
      STA   BLT_SRCSPACE
      STA   BLT_DSTSPACE
      LDA   #<TURTLE_SHAPE_BUF
      STA   BLT_SRCL
      LDA   #>TURTLE_SHAPE_BUF
      STA   BLT_SRCM
      STZ   BLT_SRCH
      LDA   #<TURTLE_ROT_BUF
      STA   BLT_DSTL
      LDA   #>TURTLE_ROT_BUF
      STA   BLT_DSTM
      STZ   BLT_DSTH
      LDA   #TURTLE_SPR_SIZE
      STA   BLT_WIDTHL
      STZ   BLT_WIDTHH
      LDA   #TURTLE_SPR_SIZE
      STA   BLT_HEIGHTL
      STZ   BLT_HEIGHTH
      LDA   #TURTLE_SPR_SIZE
      STA   BLT_SRCSTRL
      STZ   BLT_SRCSTRH
      LDA   #TURTLE_SPR_SIZE
      STA   BLT_DSTSTRL
      STZ   BLT_DSTSTRH
      LDA   tmp0
      STA   BLT_ROTANGLE
      LDA   #BLT_MODE_ROTATE
      STA   BLT_MODE
      LDA   #BLT_CMD_START
      STA   BLT_CMD

      ; Wait for blitter to finish
@wait_blt:
      LDA   BLT_STATUS
      CMP   #BLT_STATUS_BUSY
      BEQ   @wait_blt

      ; Now copy rotated shape to sprite hardware via SPRDEF/SPRROW
      JSR   define_sprite_from_rot

@done:
      RTS

; =====================================================================
; define_sprite_shape — define sprite from TURTLE_SHAPE_BUF
; =====================================================================
define_sprite_shape:
      ; SPRDEF slot, width=16, height=16
      JSR   wait_vgc
      LDA   TURTLE_SPRITE
      STA   VGC_P0
      LDA   #TURTLE_SPR_SIZE
      STA   VGC_P1              ; width
      STA   VGC_P2              ; height
      LDA   #VCMD_SPRDEF
      STA   VGC_CMD

      ; Send 16 rows via SPRROW
      LDX   #0                  ; row counter
      STZ   tmp0                ; source offset low
      STZ   tmp1                ; source offset high (starts at 0)
@row_loop:
      JSR   wait_vgc
      LDA   TURTLE_SPRITE
      STA   VGC_P0              ; slot
      STX   VGC_P1              ; row index
      ; Copy 16 pixels to P2..P14 and use MEMWRITE via SPRROW
      ; SPRROW takes: P0=slot, P1=row, P2=src_addr_lo, P3=src_addr_hi
      CLC
      LDA   #<TURTLE_SHAPE_BUF
      ADC   tmp0
      STA   VGC_P2
      LDA   #>TURTLE_SHAPE_BUF
      ADC   tmp1
      STA   VGC_P3
      LDA   #VCMD_SPRROW
      STA   VGC_CMD

      ; Advance source offset by 16
      CLC
      LDA   tmp0
      ADC   #TURTLE_SPR_SIZE
      STA   tmp0
      LDA   tmp1
      ADC   #0
      STA   tmp1

      INX
      CPX   #TURTLE_SPR_SIZE
      BCC   @row_loop
      RTS

; =====================================================================
; define_sprite_from_rot — define sprite from TURTLE_ROT_BUF
; =====================================================================
define_sprite_from_rot:
      ; SPRDEF slot
      JSR   wait_vgc
      LDA   TURTLE_SPRITE
      STA   VGC_P0
      LDA   #TURTLE_SPR_SIZE
      STA   VGC_P1
      STA   VGC_P2
      LDA   #VCMD_SPRDEF
      STA   VGC_CMD

      ; Send 16 rows from rotated buffer
      LDX   #0
      STZ   tmp0
      STZ   tmp1
@row_loop:
      JSR   wait_vgc
      LDA   TURTLE_SPRITE
      STA   VGC_P0
      STX   VGC_P1
      CLC
      LDA   #<TURTLE_ROT_BUF
      ADC   tmp0
      STA   VGC_P2
      LDA   #>TURTLE_ROT_BUF
      ADC   tmp1
      STA   VGC_P3
      LDA   #VCMD_SPRROW
      STA   VGC_CMD

      CLC
      LDA   tmp0
      ADC   #TURTLE_SPR_SIZE
      STA   tmp0
      LDA   tmp1
      ADC   #0
      STA   tmp1

      INX
      CPX   #TURTLE_SPR_SIZE
      BCC   @row_loop
      RTS

; =====================================================================
; setup_copper — copper split for gfx+sprites top, text bottom
; =====================================================================
setup_copper:
      JSR   wait_vgc
      LDA   #VCMD_COPPERDIS
      STA   VGC_CMD

      JSR   wait_vgc
      STZ   VGC_P0
      LDA   #VCMD_COPPERLIST
      STA   VGC_CMD

      JSR   wait_vgc
      LDA   #VCMD_COPPERCLR
      STA   VGC_CMD

      ; Rule 1: at line 0, mode = gfx+sprites
      STZ   VGC_P0
      STZ   VGC_P1
      STZ   VGC_P2
      STZ   VGC_P3
      STZ   VGC_P4
      LDA   #MODE_GFX_SPRITES
      STA   VGC_P5
      JSR   wait_vgc
      LDA   #VCMD_COPPERADD
      STA   VGC_CMD

      ; Rule 2: at SPLIT_Y, mode = text only
      STZ   VGC_P0
      STZ   VGC_P1
      LDA   #SPLIT_Y
      STA   VGC_P2
      STZ   VGC_P3
      STZ   VGC_P4
      LDA   #MODE_TEXT_ONLY
      STA   VGC_P5
      JSR   wait_vgc
      LDA   #VCMD_COPPERADD
      STA   VGC_CMD

      ; Use list 0 and enable copper
      JSR   wait_vgc
      STZ   VGC_P0
      LDA   #VCMD_COPPERUSE
      STA   VGC_CMD

      JSR   wait_vgc
      LDA   #VCMD_COPPERENA
      STA   VGC_CMD
      RTS

; =====================================================================
; wait_vgc — wait until VGC is not busy
; =====================================================================
wait_vgc:
      LDA   VGC_CMD
      AND   #$01
      BNE   wait_vgc
      RTS

; =====================================================================
; RODATA segment — turtle sprite shape data
; =====================================================================
      .segment "RODATA"

; 16x16 turtle arrow shape pointing UP (north).
; Colors: 0=transparent, 1=white(outline), 5=green(fill)
turtle_shape_data:
      ;         0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
      .byte     0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0  ; row 0
      .byte     0, 0, 0, 0, 0, 0, 1, 5, 5, 1, 0, 0, 0, 0, 0, 0  ; row 1
      .byte     0, 0, 0, 0, 0, 0, 1, 5, 5, 1, 0, 0, 0, 0, 0, 0  ; row 2
      .byte     0, 0, 0, 0, 0, 1, 5, 5, 5, 5, 1, 0, 0, 0, 0, 0  ; row 3
      .byte     0, 0, 0, 0, 0, 1, 5, 5, 5, 5, 1, 0, 0, 0, 0, 0  ; row 4
      .byte     0, 0, 0, 0, 1, 5, 5, 5, 5, 5, 5, 1, 0, 0, 0, 0  ; row 5
      .byte     0, 0, 0, 0, 1, 5, 5, 5, 5, 5, 5, 1, 0, 0, 0, 0  ; row 6
      .byte     0, 0, 0, 1, 5, 5, 5, 5, 5, 5, 5, 5, 1, 0, 0, 0  ; row 7
      .byte     0, 0, 0, 1, 5, 5, 5, 5, 5, 5, 5, 5, 1, 0, 0, 0  ; row 8
      .byte     0, 0, 1, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 1, 0, 0  ; row 9
      .byte     0, 0, 1, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 1, 0, 0  ; row 10
      .byte     0, 1, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 1, 0  ; row 11
      .byte     0, 1, 1, 1, 1, 1, 1, 5, 5, 1, 1, 1, 1, 1, 1, 0  ; row 12
      .byte     0, 0, 0, 0, 0, 0, 1, 5, 5, 1, 0, 0, 0, 0, 0, 0  ; row 13
      .byte     0, 0, 0, 0, 0, 0, 1, 5, 5, 1, 0, 0, 0, 0, 0, 0  ; row 14
      .byte     0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0  ; row 15

; turtle.s -- 16x16 hardware-rotated virtual-sprite turtle demo for NovaVM
;
; Load address: $7200   Invoke: SYS $7200
; Controls: left/right rotate by small steps, up/down rotate by coarse steps.
; The copper splits the display: rows 0-39 are graphics, rows 40-49 are text.

.include "nova.inc"
.include "vsprite.inc"

SPRITE_SIZE     = 16
SPRITE_X        = 152
SPRITE_Y        = 72
ANGLE_FINE      = 4
ANGLE_COARSE    = 16
SPLIT_Y         = 160
SPLIT_RULE_Y    = 159
CONSOLE_LEFT    = 0
CONSOLE_TOP     = 40
CONSOLE_WIDTH   = 80
CONSOLE_HEIGHT  = 10
CONSOLE_HINT_ROW = 41
CONSOLE_ANGLE_ROW = 42
CONSOLE_EXIT_ROW = 44
CONSOLE_INPUT_ROW = 49

KEY_LEFT        = $1C
KEY_RIGHT       = $1D
KEY_UP          = $1E
KEY_DOWN        = $1F
KEY_Q           = 'q'
KEY_Q_UPPER     = 'Q'

COL_WHITE       = 1
COL_RED         = 2
COL_CYAN        = 3
COL_GREEN       = 5

MODE_TEXT_ONLY  = 0
MODE_GFX_SPRITES = 3

.segment "ZEROPAGE"

zp_last_frame:  .res 1
zp_angle:       .res 1
zp_ptr:         .res 2

.segment "BSS"

rotated_shape:  .res SPRITE_SIZE * SPRITE_SIZE

.segment "HEADER"
      .byte $00, $72

.segment "CODE"

start:
      JSR   init_display
      JSR   init_vsprite
      STZ   zp_angle
      JSR   draw_sprite

main_loop:
      JSR   wait_vsync
      LDA   VGC_CHARIN
      BEQ   main_loop
      CMP   #KEY_LEFT
      BEQ   rotate_left
      CMP   #KEY_RIGHT
      BEQ   rotate_right
      CMP   #KEY_UP
      BEQ   rotate_up
      CMP   #KEY_DOWN
      BEQ   rotate_down
      CMP   #KEY_Q
      BEQ   done
      CMP   #KEY_Q_UPPER
      BEQ   done
      BRA   main_loop

rotate_left:
      LDA   zp_angle
      SEC
      SBC   #ANGLE_FINE
      STA   zp_angle
      JSR   draw_sprite
      BRA   main_loop

rotate_right:
      LDA   zp_angle
      CLC
      ADC   #ANGLE_FINE
      STA   zp_angle
      JSR   draw_sprite
      BRA   main_loop

rotate_up:
      LDA   zp_angle
      CLC
      ADC   #ANGLE_COARSE
      STA   zp_angle
      JSR   draw_sprite
      BRA   main_loop

rotate_down:
      LDA   zp_angle
      SEC
      SBC   #ANGLE_COARSE
      STA   zp_angle
      JSR   draw_sprite
      BRA   main_loop

done:
      JSR   configure_text_window
      JSR   print_exit_status
      LDA   #$01
      STA   VGC_CURSEN
      STZ   VGC_CURSX
      LDA   #CONSOLE_INPUT_ROW
      STA   VGC_CURSY
      RTS

init_display:
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
      LDA   #$0C
      STA   VGC_CHAROUT
      JSR   wait_vgc_cmd
      LDA   #0
      STA   VSPRITE_XL
      STA   VSPRITE_XH
      STA   VSPRITE_Y
      LDA   #<320
      STA   VSPRITE_WIDTHL
      LDA   #>320
      STA   VSPRITE_WIDTHH
      LDA   #200
      STA   VSPRITE_HEIGHTL
      STZ   VSPRITE_HEIGHTH
      STZ   VSPRITE_FILLVALUE
      JSR   vsprite_gfx_fill
      JSR   draw_split_rule
      JSR   setup_copper_split
      JSR   configure_text_window
      JSR   print_console_header
      LDA   VGC_STATUS
      STA   zp_last_frame
      RTS

configure_text_window:
      LDA   #CONSOLE_LEFT
      STA   TEXTWIN_LEFT
      LDA   #CONSOLE_TOP
      STA   TEXTWIN_TOP
      LDA   #CONSOLE_WIDTH
      STA   TEXTWIN_WIDTH
      LDA   #CONSOLE_HEIGHT
      STA   TEXTWIN_HEIGHT
      RTS

wait_vgc_cmd:
@wait:
      LDA   VGC_CMD
      AND   #$01
      BNE   @wait
      RTS

setup_copper_split:
      JSR   wait_vgc_cmd
      LDA   #VCMD_COPPERDIS
      STA   VGC_CMD

      JSR   wait_vgc_cmd
      STZ   VGC_P0
      LDA   #VCMD_COPPERLIST
      STA   VGC_CMD

      JSR   wait_vgc_cmd
      LDA   #VCMD_COPPERCLR
      STA   VGC_CMD

      STZ   VGC_P0
      STZ   VGC_P1
      STZ   VGC_P2
      STZ   VGC_P3
      STZ   VGC_P4
      LDA   #MODE_GFX_SPRITES
      STA   VGC_P5
      JSR   wait_vgc_cmd
      LDA   #VCMD_COPPERADD
      STA   VGC_CMD

      STZ   VGC_P0
      STZ   VGC_P1
      LDA   #SPLIT_Y
      STA   VGC_P2
      STZ   VGC_P3
      STZ   VGC_P4
      LDA   #MODE_TEXT_ONLY
      STA   VGC_P5
      JSR   wait_vgc_cmd
      LDA   #VCMD_COPPERADD
      STA   VGC_CMD

      JSR   wait_vgc_cmd
      STZ   VGC_P0
      LDA   #VCMD_COPPERUSE
      STA   VGC_CMD

      JSR   wait_vgc_cmd
      LDA   #VCMD_COPPERENA
      STA   VGC_CMD
      JMP   wait_vgc_cmd

draw_split_rule:
      JSR   wait_vgc_cmd
      LDA   #COL_CYAN
      STA   VGC_P0
      LDA   #VCMD_GCOLOR
      STA   VGC_CMD

      JSR   wait_vgc_cmd
      STZ   VGC_P0
      STZ   VGC_P1
      LDA   #SPLIT_RULE_Y
      STA   VGC_P2
      STZ   VGC_P3
      LDA   #<319
      STA   VGC_P4
      LDA   #>319
      STA   VGC_P5
      LDA   #SPLIT_RULE_Y
      STA   VGC_P6
      STZ   VGC_P7
      LDA   #VCMD_LINE
      STA   VGC_CMD
      JMP   wait_vgc_cmd

init_vsprite:
      LDA   #SPRITE_SIZE
      STA   VSPRITE_WIDTHL
      STZ   VSPRITE_WIDTHH
      STA   VSPRITE_HEIGHTL
      STZ   VSPRITE_HEIGHTH
      STA   VSPRITE_ORIGSTRL
      STZ   VSPRITE_ORIGSTRH
      STA   VSPRITE_ROTSTRL
      STZ   VSPRITE_ROTSTRH

      LDA   #BLT_SPACE_CPU
      STA   VSPRITE_ORIGSPACE
      STA   VSPRITE_ROTSPACE
      LDA   #<source_shape
      STA   VSPRITE_ORIGADDRL
      LDA   #>source_shape
      STA   VSPRITE_ORIGADDRM
      STZ   VSPRITE_ORIGADDRH
      LDA   #<rotated_shape
      STA   VSPRITE_ROTADDRL
      LDA   #>rotated_shape
      STA   VSPRITE_ROTADDRM
      STZ   VSPRITE_ROTADDRH

      LDA   #<SPRITE_X
      STA   VSPRITE_XL
      LDA   #>SPRITE_X
      STA   VSPRITE_XH
      LDA   #SPRITE_Y
      STA   VSPRITE_Y
      STZ   VSPRITE_COLORKEY
      STZ   VSPRITE_FLAGS
      RTS

draw_sprite:
      LDA   zp_angle
      STA   VSPRITE_ROTANGLE
      JSR   vsprite_gfx_rotate_blit
      BNE   show_rotate_error
      JSR   print_angle
      RTS

show_rotate_error:
      LDA   #CONSOLE_EXIT_ROW
      STA   VGC_CURSY
      STZ   VGC_CURSX
      LDA   #COL_RED
      STA   VGC_FGCOL
      LDA   #<msg_error
      STA   zp_ptr
      LDA   #>msg_error
      STA   zp_ptr + 1
      JMP   print_str

wait_vsync:
@wait:
      LDA   VGC_STATUS
      CMP   zp_last_frame
      BEQ   @wait
      STA   zp_last_frame
      RTS

print_angle:
      LDA   #CONSOLE_ANGLE_ROW
      STA   VGC_CURSY
      STZ   VGC_CURSX
      LDA   #COL_WHITE
      STA   VGC_FGCOL
      LDA   #<msg_angle
      STA   zp_ptr
      LDA   #>msg_angle
      STA   zp_ptr + 1
      JSR   print_str
      LDA   zp_angle
      PHA
      LSR
      LSR
      LSR
      LSR
      TAX
      LDA   hex_digits,X
      STA   VGC_CHAROUT
      PLA
      AND   #$0F
      TAX
      LDA   hex_digits,X
      STA   VGC_CHAROUT
      LDA   #' '
      STA   VGC_CHAROUT
      RTS

print_console_header:
      LDA   #CONSOLE_TOP
      STA   VGC_CURSY
      STZ   VGC_CURSX
      LDA   #COL_WHITE
      STA   VGC_FGCOL
      LDA   #<msg_title
      STA   zp_ptr
      LDA   #>msg_title
      STA   zp_ptr + 1
      JSR   print_str

      LDA   #CONSOLE_HINT_ROW
      STA   VGC_CURSY
      STZ   VGC_CURSX
      LDA   #COL_CYAN
      STA   VGC_FGCOL
      LDA   #<msg_hint
      STA   zp_ptr
      LDA   #>msg_hint
      STA   zp_ptr + 1
      JSR   print_str
      RTS

print_exit_status:
      LDA   #CONSOLE_EXIT_ROW
      STA   VGC_CURSY
      STZ   VGC_CURSX
      LDA   #COL_WHITE
      STA   VGC_FGCOL
      LDA   #<msg_exit
      STA   zp_ptr
      LDA   #>msg_exit
      STA   zp_ptr + 1
      JMP   print_str

print_str:
      LDY   #0
@loop:
      LDA   (zp_ptr),Y
      BEQ   @done
      STA   VGC_CHAROUT
      INY
      BRA   @loop
@done:
      RTS

.segment "RODATA"

msg_title:
      .byte "TURTLE SPLIT: TOP 40 ROWS GRAPHICS, BOTTOM 10 ROWS TEXT", 0
msg_hint:
      .byte "ARROWS ROTATE THE TURTLE. Q RETURNS WITH THIS TEXT BAND ACTIVE.", 0
msg_angle:
      .byte "ANGLE $", 0
msg_exit:
      .byte "TURTLE CONSOLE READY. TYPE BELOW.", 0
msg_error:
      .byte "TURTLE ROTATE ERROR", 0
hex_digits:
      .byte "0123456789ABCDEF"

source_shape:
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

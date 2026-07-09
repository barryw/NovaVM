; =====================================================================
;  Nova NDK — nui_dialog.s
;
;  Always-used nui core: status/result helpers, dialog geometry and
;  style, graphics-plane chrome, text clears, centered text rendering,
;  and the nui_show_dialog entry point. The list picker, save/
;  restore-under, and key-input objects layer on top of this.
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

.include "nui.inc"
.include "blitter.inc"

NUI_CH_TL = $C9
NUI_CH_TR = $BB
NUI_CH_BL = $C8
NUI_CH_BR = $BC
NUI_CH_H  = $CD
NUI_CH_V  = $BA
NUI_GFX_STRIDE = 320

      .segment "BSS"

NUI_GFX_Y:   .res 1
NUI_GFX_TMP: .res 1

      .segment "CODE"

nui_ok:
      STZ   NUI_RESULT
      LDA   #NUI_OK
      RTS

nui_error:
      LDA   #NUI_ERR
      STA   NUI_RESULT
      RTS

; @label NUI.DIALOG_DEFAULTS
; @kind routine
; @symbol nui_dialog_defaults
; @summary Set the standard centered modal-dialog geometry.
nui_dialog_defaults:
      LDA   #NUI_DIALOG_DEFAULT_LEFT
      STA   NUI_DIALOG_LEFT
      LDA   #NUI_DIALOG_DEFAULT_TOP
      STA   NUI_DIALOG_TOP
      LDA   #NUI_DIALOG_DEFAULT_WIDTH
      STA   NUI_DIALOG_WIDTH
      LDA   #NUI_DIALOG_DEFAULT_HEIGHT
      STA   NUI_DIALOG_HEIGHT
      STZ   NUI_FOOTERL
      STZ   NUI_FOOTERH
      JSR   nui_style_defaults
      JMP   nui_ok

nui_style_defaults:
      LDA   #NUI_COLOR_SHADOW
      STA   NUI_STYLE_SHADOW
      LDA   #NUI_COLOR_BORDER
      STA   NUI_STYLE_BORDER
      LDA   #NUI_COLOR_PANEL
      STA   NUI_STYLE_PANEL
      LDA   #NUI_TEXT_NORMAL
      STA   NUI_STYLE_TEXT
      LDA   #$01
      STA   NUI_STYLE_VALID
      RTS

nui_ensure_style:
      LDA   NUI_STYLE_VALID
      BNE   @done
      JSR   nui_style_defaults
@done:
      RTS

nui_validate_dialog:
      LDA   NUI_DIALOG_WIDTH
      BEQ   nui_error
      LDA   NUI_DIALOG_HEIGHT
      BEQ   nui_error
      LDA   NUI_DIALOG_LEFT
      CLC
      ADC   NUI_DIALOG_WIDTH
      BCS   nui_error
      CMP   #VTEXT_SCREEN_COLS + 1
      BCS   nui_error
      LDA   NUI_DIALOG_TOP
      CLC
      ADC   NUI_DIALOG_HEIGHT
      BCS   nui_error
      CMP   #VTEXT_SCREEN_ROWS + 1
      BCS   nui_error
      JMP   nui_ok

nui_set_x_pixels:
      STZ   BLT_DSTM
      STZ   BLT_DSTH
      ASL
      ROL   BLT_DSTM
      ASL
      ROL   BLT_DSTM
      STA   BLT_DSTL
      RTS

nui_set_width_pixels:
      STZ   BLT_WIDTHH
      ASL
      ROL   BLT_WIDTHH
      ASL
      ROL   BLT_WIDTHH
      STA   BLT_WIDTHL
      RTS

nui_set_y_pixels:
      ASL
      ASL
      STA   NUI_GFX_Y
      RTS

nui_set_height_pixels:
      ASL
      ASL
      STA   BLT_HEIGHTL
      STZ   BLT_HEIGHTH
      RTS

nui_set_dialog_rect_pixels:
      LDA   NUI_DIALOG_LEFT
      JSR   nui_set_x_pixels
      LDA   NUI_DIALOG_TOP
      JSR   nui_set_y_pixels
      LDA   NUI_DIALOG_WIDTH
      JSR   nui_set_width_pixels
      LDA   NUI_DIALOG_HEIGHT
      JMP   nui_set_height_pixels

nui_inc_x_pixel:
      INC   BLT_DSTL
      BNE   @done
      INC   BLT_DSTM
      BNE   @done
      INC   BLT_DSTH
@done:
      RTS

nui_gfx_fill:
      LDA   NUI_GFX_Y
      CLC
      ADC   BLT_DSTM
      STA   BLT_DSTM
      BCC   @add_y64
      INC   BLT_DSTH

@add_y64:
      LDA   NUI_GFX_Y
      LSR
      LSR
      STA   NUI_GFX_TMP
      LDA   NUI_GFX_Y
      ASL
      ASL
      ASL
      ASL
      ASL
      ASL
      CLC
      ADC   BLT_DSTL
      STA   BLT_DSTL
      LDA   BLT_DSTM
      ADC   NUI_GFX_TMP
      STA   BLT_DSTM
      BCC   @setup
      INC   BLT_DSTH

@setup:
      LDA   #BLT_SPACE_VGC_GFX
      STA   BLT_DSTSPACE
      LDA   #<NUI_GFX_STRIDE
      STA   BLT_DSTSTRL
      LDA   #>NUI_GFX_STRIDE
      STA   BLT_DSTSTRH
      JMP   blitter_fill

nui_draw_chrome:
      LDA   NUI_STYLE_SHADOW
      STA   BLT_FILLVALUE
      JSR   nui_set_dialog_rect_pixels
      JSR   nui_inc_x_pixel
      JSR   nui_inc_x_pixel
      JSR   nui_inc_x_pixel
      JSR   nui_inc_x_pixel
      INC   NUI_GFX_Y
      INC   NUI_GFX_Y
      INC   NUI_GFX_Y
      INC   NUI_GFX_Y
      JSR   nui_gfx_fill

      LDA   NUI_STYLE_PANEL
      STA   BLT_FILLVALUE
      JSR   nui_set_dialog_rect_pixels
      JSR   nui_gfx_fill

      LDA   NUI_STYLE_BORDER
      STA   BLT_FILLVALUE
      JSR   nui_set_dialog_rect_pixels
      LDA   #$02
      STA   BLT_HEIGHTL
      STZ   BLT_HEIGHTH
      JSR   nui_gfx_fill

      LDA   NUI_STYLE_BORDER
      STA   BLT_FILLVALUE
      JSR   nui_set_dialog_rect_pixels
      LDA   #$02
      STA   BLT_WIDTHL
      STZ   BLT_WIDTHH
      JMP   nui_gfx_fill

nui_clear_text_region:
      LDA   NUI_DIALOG_LEFT
      STA   VTEXT_LEFT
      LDA   NUI_DIALOG_TOP
      STA   VTEXT_TOP
      LDA   NUI_DIALOG_WIDTH
      STA   VTEXT_WIDTH
      LDA   NUI_DIALOG_HEIGHT
      STA   VTEXT_HEIGHT
      JSR   nui_clear_text_style_region
      BNE   @done

      LDA   NUI_DIALOG_LEFT
      CLC
      ADC   NUI_DIALOG_WIDTH
      CMP   #VTEXT_SCREEN_COLS
      BCS   @bottom
      STA   VTEXT_LEFT
      LDA   NUI_DIALOG_TOP
      STA   VTEXT_TOP
      LDA   #$01
      STA   VTEXT_WIDTH
      LDA   NUI_DIALOG_HEIGHT
      STA   VTEXT_HEIGHT
      JSR   nui_clear_transparent_text_region
      BNE   @done

@bottom:
      LDA   NUI_DIALOG_TOP
      CLC
      ADC   NUI_DIALOG_HEIGHT
      CMP   #VTEXT_SCREEN_ROWS
      BCS   @ok
      STA   VTEXT_TOP
      LDA   NUI_DIALOG_LEFT
      STA   VTEXT_LEFT
      LDA   NUI_DIALOG_WIDTH
      STA   VTEXT_WIDTH
      LDA   NUI_DIALOG_LEFT
      CLC
      ADC   NUI_DIALOG_WIDTH
      CMP   #VTEXT_SCREEN_COLS
      BCS   :+
      INC   VTEXT_WIDTH
:
      LDA   #$01
      STA   VTEXT_HEIGHT
      JMP   nui_clear_transparent_text_region
@ok:
      LDA   #NUI_OK
@done:
      RTS

nui_clear_text_style_region:
      STZ   VTEXT_CURX
      STZ   VTEXT_CURY
      LDA   NUI_STYLE_TEXT
      STA   VTEXT_COLOR
      STZ   VTEXT_ATTR
      STZ   VTEXT_FLAGS
      JMP   vtext_clear_region

nui_clear_transparent_text_region:
      STZ   VTEXT_CURX
      STZ   VTEXT_CURY
      LDA   NUI_STYLE_TEXT
      AND   #$0F
      STA   VTEXT_COLOR
      STZ   VTEXT_ATTR
      STZ   VTEXT_FLAGS
      JMP   vtext_clear_region

nui_set_screen_text:
      STZ   VTEXT_LEFT
      STZ   VTEXT_TOP
      LDA   #VTEXT_SCREEN_COLS
      STA   VTEXT_WIDTH
      LDA   #VTEXT_SCREEN_ROWS
      STA   VTEXT_HEIGHT
      LDA   NUI_STYLE_TEXT
      STA   VTEXT_COLOR
      STZ   VTEXT_ATTR
      STZ   VTEXT_FLAGS
      RTS

nui_frame_putc:
      STA   VTEXT_CHAR
      JMP   vtext_put_char

nui_draw_text_frame:
      JSR   nui_set_screen_text
      LDA   #NUI_TEXT_FRAME
      STA   VTEXT_COLOR
      LDA   NUI_DIALOG_LEFT
      STA   VTEXT_CURX
      LDA   NUI_DIALOG_TOP
      STA   VTEXT_CURY
      JSR   vtext_set_cursor
      LDA   #NUI_CH_TL
      JSR   nui_frame_putc
      LDA   NUI_DIALOG_WIDTH
      SEC
      SBC   #2
      STA   NUI_TEXT_LEN
@top:
      LDA   #NUI_CH_H
      JSR   nui_frame_putc
      DEC   NUI_TEXT_LEN
      BNE   @top
      LDA   #NUI_CH_TR
      JSR   nui_frame_putc

      LDA   NUI_DIALOG_HEIGHT
      SEC
      SBC   #2
      STA   NUI_TEXT_LEN
      LDA   NUI_DIALOG_TOP
      CLC
      ADC   #1
      STA   VTEXT_CURY
@sides:
      LDA   NUI_TEXT_LEN
      BEQ   @bottom
      LDA   NUI_DIALOG_LEFT
      STA   VTEXT_CURX
      JSR   vtext_set_cursor
      LDA   #NUI_CH_V
      JSR   nui_frame_putc
      LDA   NUI_DIALOG_LEFT
      CLC
      ADC   NUI_DIALOG_WIDTH
      SEC
      SBC   #1
      STA   VTEXT_CURX
      JSR   vtext_set_cursor
      LDA   #NUI_CH_V
      JSR   nui_frame_putc
      INC   VTEXT_CURY
      DEC   NUI_TEXT_LEN
      BRA   @sides

@bottom:
      LDA   NUI_DIALOG_LEFT
      STA   VTEXT_CURX
      LDA   NUI_DIALOG_TOP
      CLC
      ADC   NUI_DIALOG_HEIGHT
      SEC
      SBC   #1
      STA   VTEXT_CURY
      JSR   vtext_set_cursor
      LDA   #NUI_CH_BL
      JSR   nui_frame_putc
      LDA   NUI_DIALOG_WIDTH
      SEC
      SBC   #2
      STA   NUI_TEXT_LEN
@bottom_line:
      LDA   #NUI_CH_H
      JSR   nui_frame_putc
      DEC   NUI_TEXT_LEN
      BNE   @bottom_line
      LDA   #NUI_CH_BR
      JMP   nui_frame_putc

nui_print_ptr:
      LDA   NUI_PRINT_X
      STA   VTEXT_CURX
      LDA   VTEXT_CURY
      JSR   vtext_set_cursor
@loop:
      LDY   #$00
      LDA   (NUI_PRINTL),Y
      BEQ   @done
      CMP   #$0D
      BEQ   @newline
      CMP   #$0A
      BEQ   @newline
      STA   VTEXT_CHAR
      JSR   vtext_put_char
      BRA   @next
@newline:
      LDA   NUI_PRINT_X
      STA   VTEXT_CURX
      INC   VTEXT_CURY
      JSR   vtext_set_cursor
@next:
      INC   NUI_PRINTL
      BNE   @loop
      INC   NUI_PRINTH
      BRA   @loop
@done:
      RTS

nui_measure_line:
      STZ   NUI_TEXT_LEN
      LDY   #$00
@loop:
      LDA   (NUI_PRINTL),Y
      BEQ   @done
      CMP   #$0D
      BEQ   @done
      CMP   #$0A
      BEQ   @done
      INC   NUI_TEXT_LEN
      INY
      BNE   @loop
@done:
      RTS

nui_center_print_x:
      JSR   nui_measure_line
      LDA   NUI_DIALOG_WIDTH
      SEC
      SBC   NUI_TEXT_LEN
      BCC   @left
      LSR
      CLC
      ADC   NUI_DIALOG_LEFT
      STA   NUI_PRINT_X
      RTS
@left:
      LDA   NUI_DIALOG_LEFT
      CLC
      ADC   #2
      STA   NUI_PRINT_X
      RTS

nui_print_title:
      LDA   NUI_TITLEL
      ORA   NUI_TITLEH
      BEQ   @done
      LDA   NUI_TITLEL
      STA   NUI_PRINTL
      LDA   NUI_TITLEH
      STA   NUI_PRINTH
      JSR   nui_center_print_x
      LDA   NUI_PRINT_X
      CMP   NUI_DIALOG_LEFT
      BEQ   :+
      DEC   NUI_PRINT_X
:
      LDA   NUI_DIALOG_TOP
      STA   VTEXT_CURY
      JSR   nui_set_screen_text
      LDA   #NUI_TEXT_FRAME
      STA   VTEXT_COLOR
      LDA   NUI_PRINT_X
      STA   VTEXT_CURX
      JSR   vtext_set_cursor
      LDA   #' '
      JSR   nui_frame_putc
      INC   NUI_PRINT_X
      LDA   NUI_DIALOG_TOP
      STA   VTEXT_CURY
      JSR   nui_set_screen_text
      LDA   #NUI_TEXT_FRAME
      STA   VTEXT_COLOR
      LDA   NUI_TITLEL
      STA   NUI_PRINTL
      LDA   NUI_TITLEH
      STA   NUI_PRINTH
      JSR   nui_print_ptr
      LDA   #' '
      JMP   nui_frame_putc
@done:
      RTS

nui_print_message:
      LDA   NUI_MSGL
      ORA   NUI_MSGH
      BEQ   @done
      LDA   NUI_MSGL
      STA   NUI_PRINTL
      LDA   NUI_MSGH
      STA   NUI_PRINTH
      LDA   NUI_DIALOG_LEFT
      CLC
      ADC   #2
      STA   NUI_PRINT_X
      STA   VTEXT_CURX
      JSR   nui_set_screen_text
      LDA   NUI_STYLE_TEXT
      STA   VTEXT_COLOR
      LDA   NUI_DIALOG_TOP
      CLC
      ADC   #3
      STA   VTEXT_CURY
      JMP   nui_print_ptr
@done:
      RTS

; @label NUI.SHOW_DIALOG
; @kind routine
; @symbol nui_show_dialog
; @summary Draw the configured modal dialog without waiting for input.
nui_show_dialog:
      JSR   nui_validate_dialog
      BNE   @done
      JSR   nui_ensure_style
      JSR   nui_draw_chrome
      JSR   nui_clear_text_region
      JSR   nui_draw_text_frame
      JSR   nui_print_title
      JSR   nui_print_message
      JMP   nui_ok
@done:
      RTS

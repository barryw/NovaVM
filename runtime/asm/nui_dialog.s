; nui modal dialog core.
;
; Feature-group object: status/result helpers, dialog geometry and style,
; graphics-plane chrome, text clears, centered text rendering, and the
; show_dialog/show_error/wait_key entry points. This is the always-used core of
; nui; the list picker, save/restore-under, and key-input objects layer on top.

.include "nui.inc"

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
      LDA   #<nui_footer_any_key
      STA   NUI_FOOTERL
      LDA   #>nui_footer_any_key
      STA   NUI_FOOTERH
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
      STZ   VSPRITE_XH
      ASL
      ROL   VSPRITE_XH
      ASL
      ROL   VSPRITE_XH
      STA   VSPRITE_XL
      RTS

nui_set_width_pixels:
      STZ   VSPRITE_WIDTHH
      ASL
      ROL   VSPRITE_WIDTHH
      ASL
      ROL   VSPRITE_WIDTHH
      STA   VSPRITE_WIDTHL
      RTS

nui_set_y_pixels:
      ASL
      ASL
      STA   VSPRITE_Y
      RTS

nui_set_height_pixels:
      ASL
      ASL
      STA   VSPRITE_HEIGHTL
      STZ   VSPRITE_HEIGHTH
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
      INC   VSPRITE_XL
      BNE   @done
      INC   VSPRITE_XH
@done:
      RTS

nui_fill_dialog_rect:
      JSR   nui_set_x_pixels
      LDA   NUI_DIALOG_TOP
      JSR   nui_set_y_pixels
      LDA   NUI_DIALOG_WIDTH
      JSR   nui_set_width_pixels
      LDA   NUI_DIALOG_HEIGHT
      JSR   nui_set_height_pixels
      JMP   vsprite_gfx_fill

nui_draw_chrome:
      LDA   NUI_STYLE_SHADOW
      STA   VSPRITE_FILLVALUE
      JSR   nui_set_dialog_rect_pixels
      JSR   nui_inc_x_pixel
      JSR   nui_inc_x_pixel
      INC   VSPRITE_Y
      INC   VSPRITE_Y
      JSR   vsprite_gfx_fill

      LDA   NUI_STYLE_PANEL
      STA   VSPRITE_FILLVALUE
      JSR   nui_set_dialog_rect_pixels
      JSR   vsprite_gfx_fill

      LDA   NUI_STYLE_BORDER
      STA   VSPRITE_FILLVALUE
      JSR   nui_set_dialog_rect_pixels
      LDA   #$02
      STA   VSPRITE_HEIGHTL
      STZ   VSPRITE_HEIGHTH
      JSR   vsprite_gfx_fill

      LDA   NUI_STYLE_BORDER
      STA   VSPRITE_FILLVALUE
      JSR   nui_set_dialog_rect_pixels
      LDA   #$02
      STA   VSPRITE_WIDTHL
      STZ   VSPRITE_WIDTHH
      JMP   vsprite_gfx_fill

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

nui_print_ptr:
      JSR   nui_set_screen_text
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
      STA   VTEXT_CURX
      LDA   NUI_DIALOG_TOP
      CLC
      ADC   #1
      STA   VTEXT_CURY
      JMP   nui_print_ptr
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
      LDA   NUI_DIALOG_TOP
      CLC
      ADC   #3
      STA   VTEXT_CURY
      JMP   nui_print_ptr
@done:
      RTS

nui_print_footer:
      LDA   NUI_FOOTERL
      ORA   NUI_FOOTERH
      BEQ   @done
      LDA   NUI_FOOTERL
      STA   NUI_PRINTL
      LDA   NUI_FOOTERH
      STA   NUI_PRINTH
      LDA   NUI_DIALOG_LEFT
      CLC
      ADC   #2
      STA   NUI_PRINT_X
      STA   VTEXT_CURX
      LDA   NUI_DIALOG_TOP
      CLC
      ADC   NUI_DIALOG_HEIGHT
      SEC
      SBC   #2
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
      JSR   nui_print_title
      JSR   nui_print_message
      JSR   nui_print_footer
      JMP   nui_ok
@done:
      RTS

; @label NUI.SHOW_DIALOG_WAIT
; @kind routine
; @symbol nui_show_dialog_wait
; @summary Draw the configured modal dialog and wait for one key.
nui_show_dialog_wait:
      JSR   nui_show_dialog
      BNE   @done
      JSR   nui_wait_key
      JMP   nui_ok
@done:
      RTS

; @label NUI.SHOW_ERROR
; @kind routine
; @symbol nui_show_error
; @summary Draw a modal error dialog and wait for one key.
nui_show_error:
      LDA   NUI_DIALOG_WIDTH
      ORA   NUI_DIALOG_HEIGHT
      BNE   @have_geometry
      JSR   nui_dialog_defaults
@have_geometry:
      JMP   nui_show_dialog_wait

; @label NUI.WAIT_KEY
; @kind routine
; @symbol nui_wait_key
; @summary Wait for one keyboard byte and return it in A.
nui_wait_key:
      LDA   #$01
      STA   VGC_CURSEN
      JSR   nui_read_key
      PHA
      STZ   VGC_CURSEN
      PLA
      RTS

nui_footer_any_key:
      .byte "PRESS ANY KEY", 0

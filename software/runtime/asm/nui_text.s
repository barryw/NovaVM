; =====================================================================
;  Nova NDK — nui_text.s
;
;  nui text input dialog: a Borland-style modal text field with
;  OK/Cancel buttons and Tab focus traversal.
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

.include "nui.inc"

NUI_INPUT_FOCUS_FIELD  = $00
NUI_INPUT_FOCUS_OK     = $01
NUI_INPUT_FOCUS_CANCEL = $02
NUI_INPUT_FIELD_WIDTH  = 26

      .segment "BSS"

nt_focus: .res 1
nt_tmp:   .res 1

      .segment "CODE"

; @label NUI.TEXT_INPUT
; @kind routine
; @symbol nui_text_input
; @summary Draw a modal text field with OK/Cancel buttons and Tab focus traversal.
; @requires NUI_TITLEL/H NUI_FOOTERL/H NUI_DIALOG_* NUI_INPUT_OUTL/H NUI_INPUT_OUT_MAX NUI_INPUT_LABELL/H
; @out A: 0 on success, 1 on error. NUI_RESULT = OK/CANCEL; NUI_INPUT_OUT_LEN = typed text length.
nui_text_input:
      LDA   NUI_INPUT_OUT_MAX
      BNE   :+
      LDA   #NUI_ERR
      RTS
:     JSR   nui_text_init_output_len
      STZ   nt_focus
      JSR   nui_show_dialog
      BEQ   :+
      LDA   #NUI_ERR
      RTS
:     JSR   nui_text_render
@key:
      JSR   nui_read_key
      STA   nt_tmp
      CMP   #NUI_KEY_ESCAPE
      BNE   :+
      JMP   @cancel
:     CMP   #NUI_KEY_TAB
      BEQ   @tab
      CMP   #NUI_KEY_RIGHT
      BEQ   @tab
      CMP   #NUI_KEY_LEFT
      BEQ   @tab
      CMP   #NUI_KEY_ENTER
      BEQ   @enter
      LDA   nt_focus
      BNE   @key
      LDA   nt_tmp
      CMP   #$08
      BEQ   @backspace
      CMP   #$7F
      BEQ   @backspace
      CMP   #$20
      BCC   @key
      CMP   #$7F
      BCS   @key
      LDA   NUI_INPUT_OUT_LEN
      CLC
      ADC   #1
      CMP   NUI_INPUT_OUT_MAX
      BCS   @key
      JSR   nui_text_output_ptr
      LDY   NUI_INPUT_OUT_LEN
      LDA   nt_tmp
      STA   (NUI_PRINTL),Y
      INC   NUI_INPUT_OUT_LEN
      LDY   NUI_INPUT_OUT_LEN
      LDA   #0
      STA   (NUI_PRINTL),Y
      JSR   nui_text_render
      JMP   @key
@tab:
      INC   nt_focus
      LDA   nt_focus
      CMP   #$03
      BCC   :+
      STZ   nt_focus
:     JSR   nui_text_render
      JMP   @key
@backspace:
      LDA   NUI_INPUT_OUT_LEN
      BEQ   @key
      DEC   NUI_INPUT_OUT_LEN
      JSR   nui_text_output_ptr
      LDY   NUI_INPUT_OUT_LEN
      LDA   #0
      STA   (NUI_PRINTL),Y
      JSR   nui_text_render
      JMP   @key
@enter:
      LDA   nt_focus
      CMP   #NUI_INPUT_FOCUS_CANCEL
      BEQ   @cancel
      LDA   NUI_INPUT_OUT_LEN
      BNE   :+
      JMP   @key
:     LDA   #NUI_RESULT_OK
      STA   NUI_RESULT
      STZ   VGC_CURSEN
      LDA   #NUI_OK
      RTS
@cancel:
      STZ   NUI_INPUT_OUT_LEN
      LDA   #NUI_RESULT_CANCEL
      STA   NUI_RESULT
      STZ   VGC_CURSEN
      LDA   #NUI_OK
      RTS

nui_text_output_ptr:
      LDA   NUI_INPUT_OUTL
      STA   NUI_PRINTL
      LDA   NUI_INPUT_OUTH
      STA   NUI_PRINTH
      RTS

nui_text_init_output_len:
      JSR   nui_text_output_ptr
      LDA   NUI_INPUT_OUT_MAX
      SEC
      SBC   #1
      STA   nt_tmp
      STZ   NUI_INPUT_OUT_LEN
      LDY   #0
@loop:
      CPY   nt_tmp
      BCS   @terminate
      LDA   (NUI_PRINTL),Y
      BEQ   @done
      INY
      BRA   @loop
@terminate:
      LDA   #0
      STA   (NUI_PRINTL),Y
@done:
      STY   NUI_INPUT_OUT_LEN
      RTS

nui_text_render:
      JSR   nui_set_screen_text
      STZ   VTEXT_ATTR
      LDA   NUI_DIALOG_LEFT
      CLC
      ADC   #2
      STA   VTEXT_CURX
      LDA   NUI_DIALOG_TOP
      CLC
      ADC   #4
      STA   VTEXT_CURY
      LDA   #<nui_text_blank
      LDY   #>nui_text_blank
      JSR   nui_text_blank_len
      JSR   vtext_put_run
      BEQ   :+
      RTS
:     LDA   NUI_DIALOG_LEFT
      CLC
      ADC   #2
      STA   VTEXT_CURX
      LDA   NUI_DIALOG_TOP
      CLC
      ADC   #7
      STA   VTEXT_CURY
      LDA   #<nui_text_blank
      LDY   #>nui_text_blank
      JSR   nui_text_blank_len
      JSR   vtext_put_run
      BEQ   :+
      RTS
:     LDA   NUI_DIALOG_LEFT
      CLC
      ADC   #2
      STA   VTEXT_CURX
      LDA   NUI_DIALOG_TOP
      CLC
      ADC   #4
      STA   VTEXT_CURY
      LDA   NUI_INPUT_LABELL
      LDY   NUI_INPUT_LABELH
      LDX   #8
      JSR   vtext_put_run
      BEQ   :+
      RTS
:     LDA   NUI_DIALOG_LEFT
      CLC
      ADC   #10
      STA   VTEXT_CURX
      LDA   NUI_DIALOG_TOP
      CLC
      ADC   #4
      STA   VTEXT_CURY
      LDA   #NUI_TEXT_FIELD
      STA   VTEXT_COLOR
      LDA   nt_focus
      BNE   :+
      LDA   #VTEXT_ATTR_REVERSE
      STA   VTEXT_ATTR
:     LDA   #<nui_text_field_blank
      LDY   #>nui_text_field_blank
      LDX   #NUI_INPUT_FIELD_WIDTH
      JSR   vtext_put_run
      BEQ   :+
      JMP   @done
:     LDA   NUI_DIALOG_LEFT
      CLC
      ADC   #10
      STA   VTEXT_CURX
      LDA   NUI_DIALOG_TOP
      CLC
      ADC   #4
      STA   VTEXT_CURY
      LDA   NUI_INPUT_OUTL
      LDY   NUI_INPUT_OUTH
      LDX   NUI_INPUT_OUT_LEN
      CPX   #NUI_INPUT_FIELD_WIDTH + 1
      BCC   :+
      LDX   #NUI_INPUT_FIELD_WIDTH
:     JSR   vtext_put_run
      BEQ   :+
      JMP   @done
:     STZ   VTEXT_ATTR
      LDA   #NUI_TEXT_SHADOW
      STA   VTEXT_COLOR
      LDA   NUI_DIALOG_LEFT
      CLC
      ADC   #18
      STA   VTEXT_CURX
      LDA   NUI_DIALOG_TOP
      CLC
      ADC   #8
      STA   VTEXT_CURY
      LDA   #<nui_text_ok_shadow
      LDY   #>nui_text_ok_shadow
      LDX   #6
      JSR   vtext_put_run
      BEQ   :+
      JMP   @done
:     LDA   NUI_DIALOG_LEFT
      CLC
      ADC   #29
      STA   VTEXT_CURX
      LDA   NUI_DIALOG_TOP
      CLC
      ADC   #8
      STA   VTEXT_CURY
      LDA   #<nui_text_cancel_shadow
      LDY   #>nui_text_cancel_shadow
      LDX   #8
      JSR   vtext_put_run
      BEQ   :+
      JMP   @done
:     LDA   NUI_DIALOG_LEFT
      CLC
      ADC   #17
      STA   VTEXT_CURX
      LDA   NUI_DIALOG_TOP
      CLC
      ADC   #7
      STA   VTEXT_CURY
      LDA   #NUI_TEXT_BUTTON
      STA   VTEXT_COLOR
      LDA   nt_focus
      CMP   #NUI_INPUT_FOCUS_OK
      BNE   :+
      LDA   #VTEXT_ATTR_REVERSE
      STA   VTEXT_ATTR
:     LDA   #<nui_text_ok
      LDY   #>nui_text_ok
      LDX   #6
      JSR   vtext_put_run
      BEQ   :+
      JMP   @done
:     STZ   VTEXT_ATTR
      LDA   NUI_DIALOG_LEFT
      CLC
      ADC   #28
      STA   VTEXT_CURX
      LDA   NUI_DIALOG_TOP
      CLC
      ADC   #7
      STA   VTEXT_CURY
      LDA   #NUI_TEXT_BUTTON
      STA   VTEXT_COLOR
      LDA   nt_focus
      CMP   #NUI_INPUT_FOCUS_CANCEL
      BNE   :+
      LDA   #VTEXT_ATTR_REVERSE
      STA   VTEXT_ATTR
:     LDA   #<nui_text_cancel
      LDY   #>nui_text_cancel
      LDX   #8
      JSR   vtext_put_run
      STZ   VTEXT_ATTR
      JSR   nui_text_place_cursor
@done:
      RTS

nui_text_place_cursor:
      LDA   nt_focus
      BEQ   :+
      STZ   VGC_CURSEN
      RTS
:     LDA   #1
      STA   VGC_CURSEN
      LDA   NUI_INPUT_OUT_LEN
      CMP   #NUI_INPUT_FIELD_WIDTH
      BCC   :+
      LDA   #NUI_INPUT_FIELD_WIDTH
:     CLC
      ADC   NUI_DIALOG_LEFT
      CLC
      ADC   #10
      STA   VGC_CURSX
      LDA   NUI_DIALOG_TOP
      CLC
      ADC   #4
      STA   VGC_CURSY
      RTS

nui_text_blank_len:
      LDA   NUI_DIALOG_WIDTH
      CMP   #5
      BCS   :+
      LDA   #5
:     SEC
      SBC   #4
      CMP   #49
      BCC   :+
      LDA   #48
:     TAX
      RTS

nui_text_ok:          .byte "  OK  "
nui_text_cancel:      .byte " Cancel "
nui_text_ok_shadow:   .byte $DC,$DC,$DC,$DC,$DC,$DC
nui_text_cancel_shadow: .byte $DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC
nui_text_field_blank: .byte "                                "
nui_text_blank:       .byte "                                                "

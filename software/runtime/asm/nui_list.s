; nui scrollable list picker.
;
; Feature-group object: draws an interactive fixed-width list inside a dialog and
; returns the chosen row. Pulled only when a caller uses nui_pick_list; dropped
; otherwise.

.include "nui.inc"

NUI_LIST_FOCUS_LIST   = $00
NUI_LIST_FOCUS_OK     = $01
NUI_LIST_FOCUS_CANCEL = $02

      .segment "BSS"

nl_focus: .res 1

      .segment "CODE"

nui_list_visible_rows:
      LDA   NUI_DIALOG_HEIGHT
      CMP   #$08
      BCC   @error
      SEC
      SBC   #$07
      STA   NUI_LIST_VISIBLE
      LDA   #NUI_OK
      RTS
@error:
      LDA   #NUI_ERR
      RTS

nui_validate_list:
      JSR   nui_validate_dialog
      BNE   @done
      LDA   NUI_LIST_ROW_COUNT
      BEQ   @error
      LDA   NUI_LIST_ROW_WIDTH
      BEQ   @error
      LDA   NUI_DIALOG_WIDTH
      CMP   #$05
      BCC   @error
      SEC
      SBC   #$04
      CMP   NUI_LIST_ROW_WIDTH
      BCC   @error
      JSR   nui_list_visible_rows
      BNE   @done
      LDA   NUI_LIST_SELECTED
      CMP   NUI_LIST_ROW_COUNT
      BCC   @selected_ok
      STZ   NUI_LIST_SELECTED
@selected_ok:
      STZ   NUI_LIST_FIRST
      LDA   #NUI_OK
      RTS
@error:
      LDA   #NUI_ERR
@done:
      RTS

nui_list_ensure_visible:
      JSR   nui_list_visible_rows
      BNE   @done
      LDA   NUI_LIST_SELECTED
      CMP   NUI_LIST_FIRST
      BCS   @check_bottom
      STA   NUI_LIST_FIRST
      LDA   #NUI_OK
      RTS
@check_bottom:
      LDA   NUI_LIST_FIRST
      CLC
      ADC   NUI_LIST_VISIBLE
      CMP   NUI_LIST_SELECTED
      BEQ   @adjust
      BCS   @ok
@adjust:
      LDA   NUI_LIST_SELECTED
      SEC
      SBC   NUI_LIST_VISIBLE
      CLC
      ADC   #$01
      STA   NUI_LIST_FIRST
@ok:
      LDA   #NUI_OK
@done:
      RTS

nui_list_row_ptr:
      STA   NUI_LIST_TMP
      LDA   NUI_LIST_ITEMSL
      STA   NUI_LIST_ROWL
      LDA   NUI_LIST_ITEMSH
      STA   NUI_LIST_ROWH
@loop:
      LDA   NUI_LIST_TMP
      BEQ   @done
      LDA   NUI_LIST_ROWL
      CLC
      ADC   NUI_LIST_ROW_WIDTH
      STA   NUI_LIST_ROWL
      BCC   @next
      INC   NUI_LIST_ROWH
@next:
      DEC   NUI_LIST_TMP
      BRA   @loop
@done:
      RTS

nui_list_clear_area:
      LDA   NUI_DIALOG_LEFT
      CLC
      ADC   #$02
      STA   VTEXT_LEFT
      LDA   NUI_DIALOG_TOP
      CLC
      ADC   #$03
      STA   VTEXT_TOP
      LDA   NUI_DIALOG_WIDTH
      SEC
      SBC   #$04
      STA   VTEXT_WIDTH
      LDA   NUI_LIST_VISIBLE
      STA   VTEXT_HEIGHT
      STZ   VTEXT_CURX
      STZ   VTEXT_CURY
      LDA   #NUI_TEXT_FIELD
      STA   VTEXT_COLOR
      STZ   VTEXT_ATTR
      STZ   VTEXT_FLAGS
      JMP   vtext_clear_region

nui_list_print_row:
      JSR   nui_set_screen_text
      LDA   #NUI_TEXT_FIELD
      STA   VTEXT_COLOR
      STZ   VTEXT_ATTR
      LDA   NUI_LIST_FIRST
      CLC
      ADC   NUI_LIST_INDEX
      CMP   NUI_LIST_SELECTED
      BNE   @normal
      LDA   #VTEXT_ATTR_REVERSE
      STA   VTEXT_ATTR
@normal:
      LDA   NUI_DIALOG_LEFT
      CLC
      ADC   #$02
      STA   VTEXT_CURX
      LDA   NUI_DIALOG_TOP
      CLC
      ADC   #$03
      CLC
      ADC   NUI_LIST_INDEX
      STA   VTEXT_CURY
      LDA   NUI_LIST_ROWL
      LDY   NUI_LIST_ROWH
      LDX   NUI_LIST_ROW_WIDTH
      JMP   vtext_put_run

nui_list_render:
      JSR   nui_list_visible_rows
      BNE   @done
      JSR   nui_list_clear_area
      BNE   @done
      JSR   nui_list_clear_spacer
      BNE   @done
      JSR   nui_list_render_buttons
      BNE   @done
      STZ   NUI_LIST_INDEX
@loop:
      LDA   NUI_LIST_INDEX
      CMP   NUI_LIST_VISIBLE
      BCS   @ok
      LDA   NUI_LIST_FIRST
      CLC
      ADC   NUI_LIST_INDEX
      CMP   NUI_LIST_ROW_COUNT
      BCS   @ok
      PHA
      JSR   nui_list_row_ptr
      PLA
      JSR   nui_list_print_row
      BNE   @done
      INC   NUI_LIST_INDEX
      BRA   @loop
@ok:
      LDA   #NUI_OK
@done:
      RTS

nui_list_clear_spacer:
      JSR   nui_set_screen_text
      STZ   VTEXT_ATTR
      LDA   NUI_DIALOG_LEFT
      CLC
      ADC   #$02
      STA   VTEXT_CURX
      LDA   NUI_DIALOG_TOP
      CLC
      ADC   #$03
      CLC
      ADC   NUI_LIST_VISIBLE
      STA   VTEXT_CURY
      LDA   NUI_DIALOG_WIDTH
      SEC
      SBC   #$04
      TAX
      LDA   #<nui_space_row
      LDY   #>nui_space_row
      JMP   vtext_put_run

nui_list_render_buttons:
      JSR   nui_set_screen_text
      STZ   VTEXT_ATTR
      LDA   NUI_DIALOG_TOP
      CLC
      ADC   #$03
      CLC
      ADC   NUI_LIST_VISIBLE
      STA   VTEXT_CURY
      INC   VTEXT_CURY
      INC   VTEXT_CURY
      LDA   NUI_DIALOG_LEFT
      CLC
      ADC   #$03
      STA   VTEXT_CURX
      LDA   #NUI_TEXT_SHADOW
      STA   VTEXT_COLOR
      LDA   #<nui_list_ok_shadow
      LDY   #>nui_list_ok_shadow
      LDX   #6
      JSR   vtext_put_run
      BEQ   :+
      JMP   @done
:     LDA   NUI_DIALOG_LEFT
      CLC
      ADC   NUI_DIALOG_WIDTH
      SEC
      SBC   #10
      STA   VTEXT_CURX
      LDA   #<nui_list_cancel_shadow
      LDY   #>nui_list_cancel_shadow
      LDX   #8
      JSR   vtext_put_run
      BEQ   :+
      JMP   @done
:     LDA   NUI_DIALOG_TOP
      CLC
      ADC   #$03
      CLC
      ADC   NUI_LIST_VISIBLE
      STA   VTEXT_CURY
      INC   VTEXT_CURY
      LDA   NUI_DIALOG_LEFT
      CLC
      ADC   #$02
      STA   VTEXT_CURX
      LDA   #NUI_TEXT_BUTTON
      STA   VTEXT_COLOR
      LDA   nl_focus
      CMP   #NUI_LIST_FOCUS_OK
      BNE   :+
      LDA   #VTEXT_ATTR_REVERSE
      STA   VTEXT_ATTR
:     LDA   #<nui_list_ok
      LDY   #>nui_list_ok
      LDX   #6
      JSR   vtext_put_run
      BEQ   :+
      JMP   @done
:     STZ   VTEXT_ATTR
      LDA   NUI_DIALOG_LEFT
      CLC
      ADC   NUI_DIALOG_WIDTH
      SEC
      SBC   #11
      STA   VTEXT_CURX
      LDA   NUI_DIALOG_TOP
      CLC
      ADC   #$03
      CLC
      ADC   NUI_LIST_VISIBLE
      STA   VTEXT_CURY
      INC   VTEXT_CURY
      LDA   #NUI_TEXT_BUTTON
      STA   VTEXT_COLOR
      LDA   nl_focus
      CMP   #NUI_LIST_FOCUS_CANCEL
      BNE   :+
      LDA   #VTEXT_ATTR_REVERSE
      STA   VTEXT_ATTR
:     LDA   #<nui_list_cancel
      LDY   #>nui_list_cancel
      LDX   #8
      JSR   vtext_put_run
      STZ   VTEXT_ATTR
@done:
      RTS

; @label NUI.PICK_LIST
; @kind routine
; @symbol nui_pick_list
; @summary Draw an interactive fixed-width list picker and return OK or Cancel.
; @requires NUI_DIALOG_* NUI_TITLEL/H NUI_FOOTERL/H NUI_LIST_ITEMSL/H NUI_LIST_ROW_WIDTH NUI_LIST_ROW_COUNT NUI_LIST_SELECTED
; @out A: 0 on success, 1 on error. NUI_RESULT = OK/CANCEL; NUI_LIST_SELECTED = selected row.
nui_pick_list:
      STZ   VGC_CURSEN
      JSR   nui_validate_list
      BEQ   :+
      JMP   @error
:
      STZ   nl_focus
      JSR   nui_show_dialog
      BEQ   :+
      JMP   @error
:
      JSR   nui_list_ensure_visible
      BEQ   :+
      JMP   @error
:
      JSR   nui_list_render
      BEQ   :+
      JMP   @error
:
@key:
      JSR   nui_read_key
      STA   NUI_LIST_TMP
      CMP   #NUI_KEY_ENTER
      BEQ   @enter
      CMP   #NUI_KEY_ESCAPE
      BNE   :+
      JMP   @cancel
:
      CMP   #NUI_KEY_TAB
      BEQ   @focus_next
      CMP   #NUI_KEY_RIGHT
      BEQ   @focus_next
      CMP   #NUI_KEY_LEFT
      BEQ   @focus_prev
      LDA   nl_focus
      BNE   @key
      LDA   NUI_LIST_TMP
      CMP   #NUI_KEY_UP
      BEQ   @up
      CMP   #NUI_KEY_DOWN
      BEQ   @down
      BRA   @key
@up:
      LDA   NUI_LIST_SELECTED
      BEQ   @key
      DEC   NUI_LIST_SELECTED
      JSR   nui_list_ensure_visible
      BNE   @error
      JSR   nui_list_render
      BNE   @error
      JMP   @key
@down:
      LDA   NUI_LIST_SELECTED
      CLC
      ADC   #$01
      CMP   NUI_LIST_ROW_COUNT
      BCS   @key
      STA   NUI_LIST_SELECTED
      JSR   nui_list_ensure_visible
      BNE   @error
      JSR   nui_list_render
      BNE   @error
      JMP   @key
@focus_next:
      INC   nl_focus
      LDA   nl_focus
      CMP   #$03
      BCC   :+
      STZ   nl_focus
:     JSR   nui_list_render
      BNE   @error
      JMP   @key
@focus_prev:
      LDA   nl_focus
      BNE   :+
      LDA   #$03
:     DEC   A
      STA   nl_focus
      JSR   nui_list_render
      BNE   @error
      JMP   @key
@enter:
      LDA   nl_focus
      CMP   #NUI_LIST_FOCUS_CANCEL
      BEQ   @cancel
@ok:
      LDA   #NUI_RESULT_OK
      STA   NUI_RESULT
      LDA   #NUI_OK
      RTS
@cancel:
      LDA   #NUI_RESULT_CANCEL
      STA   NUI_RESULT
      LDA   #NUI_OK
      RTS
@error:
      JMP   nui_error

nui_space_row:
      .byte "                                                                                "
nui_list_ok:
      .byte "  OK  "
nui_list_cancel:
      .byte " Cancel "
nui_list_ok_shadow:
      .byte $DC,$DC,$DC,$DC,$DC,$DC
nui_list_cancel_shadow:
      .byte $DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC

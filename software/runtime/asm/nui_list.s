; nui scrollable list picker.
;
; Feature-group object: draws an interactive fixed-width list inside a dialog and
; returns the chosen row. Pulled only when a caller uses nui_pick_list; dropped
; otherwise.

.include "nui.inc"

      .segment "CODE"

nui_list_visible_rows:
      LDA   NUI_DIALOG_HEIGHT
      CMP   #$07
      BCC   @error
      SEC
      SBC   #$06
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
      LDA   NUI_STYLE_TEXT
      STA   VTEXT_COLOR
      STZ   VTEXT_ATTR
      STZ   VTEXT_FLAGS
      JMP   vtext_clear_region

nui_list_print_row:
      JSR   nui_set_screen_text
      LDA   NUI_STYLE_TEXT
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

; @label NUI.PICK_LIST
; @kind routine
; @symbol nui_pick_list
; @summary Draw an interactive fixed-width list picker and return OK or Cancel.
; @requires NUI_DIALOG_* NUI_TITLEL/H NUI_FOOTERL/H NUI_LIST_ITEMSL/H NUI_LIST_ROW_WIDTH NUI_LIST_ROW_COUNT NUI_LIST_SELECTED
; @out A: 0 on success, 1 on error. NUI_RESULT = OK/CANCEL; NUI_LIST_SELECTED = selected row.
nui_pick_list:
      JSR   nui_validate_list
      BNE   @error
      JSR   nui_show_dialog
      BNE   @error
      JSR   nui_list_ensure_visible
      BNE   @error
      JSR   nui_list_render
      BNE   @error
@key:
      JSR   nui_read_key
      CMP   #NUI_KEY_ENTER
      BEQ   @ok
      CMP   #NUI_KEY_ESCAPE
      BEQ   @cancel
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
      BRA   @key
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
      BRA   @key
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

; =====================================================================
;  Nova NDK — nui.s
;
;  Shared Nova application UI helpers: modal dialogs, error boxes, an
;  interactive fixed-width list picker, save/restore-under for text and
;  graphics planes, and ANSI-aware key input.
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

.include "nui.inc"
.include "vtext.s"
.include "vsprite.s"

NUI_CH_TL = $C9
NUI_CH_TR = $BB
NUI_CH_BL = $C8
NUI_CH_BR = $BC
NUI_CH_H  = $CD
NUI_CH_V  = $BA

NUI_LIST_FOCUS_LIST   = $00
NUI_LIST_FOCUS_OK     = $01
NUI_LIST_FOCUS_CANCEL = $02

.ifndef NUI_IMPLEMENTATION_INCLUDED
NUI_IMPLEMENTATION_INCLUDED = 1

      .segment "BSS"

NUI_DIALOG_LEFT:    .res 1
NUI_DIALOG_TOP:     .res 1
NUI_DIALOG_WIDTH:   .res 1
NUI_DIALOG_HEIGHT:  .res 1
NUI_TITLEL:         .res 1
NUI_TITLEH:         .res 1
NUI_MSGL:           .res 1
NUI_MSGH:           .res 1
NUI_FOOTERL:        .res 1
NUI_FOOTERH:        .res 1
NUI_RESULT:         .res 1
NUI_PRINT_X:        .res 1
NUI_TEXT_LEN:       .res 1
NUI_SAVE_LEFT:      .res 1
NUI_SAVE_TOP:       .res 1
NUI_SAVE_WIDTH:     .res 1
NUI_SAVE_HEIGHT:    .res 1
NUI_SAVE_VALID:     .res 1
NUI_SAVE_ADDRL:     .res 4       ; char, color, textattr, gfx XRAM low bytes
NUI_SAVE_ADDRM:     .res 4       ; char, color, textattr, gfx XRAM mid bytes
NUI_SAVE_ADDRH:     .res 4       ; char, color, textattr, gfx XRAM high bytes
NUI_LIST_ITEMSL:    .res 1
NUI_LIST_ITEMSH:    .res 1
NUI_LIST_ROW_WIDTH: .res 1
NUI_LIST_ROW_COUNT: .res 1
NUI_LIST_SELECTED:  .res 1
NUI_LIST_FIRST:     .res 1
NUI_LIST_VISIBLE:   .res 1
NUI_LIST_INDEX:     .res 1
NUI_LIST_ROWL:      .res 1
NUI_LIST_ROWH:      .res 1
NUI_LIST_TMP:       .res 1
NUI_LIST_FOCUS:     .res 1
NUI_STYLE_SHADOW:   .res 1
NUI_STYLE_BORDER:   .res 1
NUI_STYLE_PANEL:    .res 1
NUI_STYLE_TEXT:     .res 1
NUI_STYLE_VALID:    .res 1

      .segment "ZEROPAGE"

NUI_PRINTL:         .res 1
NUI_PRINTH:         .res 1

      .segment "CODE"

      .export nui_dialog_defaults
      .export nui_show_dialog
      .export nui_show_dialog_wait
      .export nui_show_error
      .export nui_read_key
      .export nui_drain_keys
      .export nui_wait_key
      .export nui_save_under
      .export nui_restore_under
      .export nui_save_under_full
      .export nui_restore_under_full
      .export nui_pick_list

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

nui_select_saved_region:
      LDA   NUI_SAVE_LEFT
      STA   VTEXT_LEFT
      LDA   NUI_SAVE_TOP
      STA   VTEXT_TOP
      LDA   NUI_SAVE_WIDTH
      STA   VTEXT_WIDTH
      LDA   NUI_SAVE_HEIGHT
      STA   VTEXT_HEIGHT
      STZ   VTEXT_CURX
      STZ   VTEXT_CURY
      JMP   vtext_validate_region

; nui_save_plane — A = VGC plane space, X = saved-plane index (0/1/2).
; Copies the current VTEXT rectangle into caller-owned XRAM staging.
nui_save_plane:
      STA   BLT_SRCSPACE
      LDA   #BLT_SPACE_XRAM
      STA   BLT_DSTSPACE
      LDA   VTEXT_ADDRL
      STA   BLT_SRCL
      LDA   VTEXT_ADDRH
      STA   BLT_SRCM
      STZ   BLT_SRCH
      LDA   NUI_SAVE_ADDRL,X
      STA   BLT_DSTL
      LDA   NUI_SAVE_ADDRM,X
      STA   BLT_DSTM
      LDA   NUI_SAVE_ADDRH,X
      STA   BLT_DSTH
      LDA   VTEXT_WIDTH
      STA   BLT_WIDTHL
      STZ   BLT_WIDTHH
      LDA   VTEXT_HEIGHT
      STA   BLT_HEIGHTL
      STZ   BLT_HEIGHTH
      LDA   #<VTEXT_SCREEN_COLS
      STA   BLT_SRCSTRL
      LDA   #>VTEXT_SCREEN_COLS
      STA   BLT_SRCSTRH
      LDA   VTEXT_WIDTH
      STA   BLT_DSTSTRL
      STZ   BLT_DSTSTRH
      JMP   blitter_start_copy

; nui_restore_plane — A = VGC plane space, X = saved-plane index (0/1/2).
; Copies caller-owned XRAM staging back over the current VTEXT rectangle.
nui_restore_plane:
      STA   BLT_DSTSPACE
      LDA   #BLT_SPACE_XRAM
      STA   BLT_SRCSPACE
      LDA   NUI_SAVE_ADDRL,X
      STA   BLT_SRCL
      LDA   NUI_SAVE_ADDRM,X
      STA   BLT_SRCM
      LDA   NUI_SAVE_ADDRH,X
      STA   BLT_SRCH
      LDA   VTEXT_ADDRL
      STA   BLT_DSTL
      LDA   VTEXT_ADDRH
      STA   BLT_DSTM
      STZ   BLT_DSTH
      LDA   VTEXT_WIDTH
      STA   BLT_WIDTHL
      STZ   BLT_WIDTHH
      LDA   VTEXT_HEIGHT
      STA   BLT_HEIGHTL
      STZ   BLT_HEIGHTH
      LDA   VTEXT_WIDTH
      STA   BLT_SRCSTRL
      STZ   BLT_SRCSTRH
      LDA   #<VTEXT_SCREEN_COLS
      STA   BLT_DSTSTRL
      LDA   #>VTEXT_SCREEN_COLS
      STA   BLT_DSTSTRH
      JMP   blitter_start_copy

; @label NUI.SAVE_UNDER
; @kind routine
; @symbol nui_save_under
; @summary Save the text planes under NUI_SAVE_* into caller-provided XRAM buffers.
; @requires NUI_SAVE_LEFT NUI_SAVE_TOP NUI_SAVE_WIDTH NUI_SAVE_HEIGHT NUI_SAVE_ADDRL NUI_SAVE_ADDRM NUI_SAVE_ADDRH
; @out A: 0 on success, 1 on error.
nui_save_under:
      STZ   NUI_SAVE_VALID
      JSR   nui_select_saved_region
      BNE   @error
      JSR   vtext_calc_region_addr

      LDX   #0
      LDA   #BLT_SPACE_VGC_CHAR
      JSR   nui_save_plane
      BNE   @error
      LDX   #1
      LDA   #BLT_SPACE_VGC_COLOR
      JSR   nui_save_plane
      BNE   @error
      LDX   #2
      LDA   #BLT_SPACE_VGC_TEXTATTR
      JSR   nui_save_plane
      BNE   @error

      LDA   #1
      STA   NUI_SAVE_VALID
      JMP   nui_ok
@error:
      JMP   nui_error

; @label NUI.RESTORE_UNDER
; @kind routine
; @symbol nui_restore_under
; @summary Restore text planes previously saved by NUI.SAVE_UNDER.
; @requires NUI_SAVE_LEFT NUI_SAVE_TOP NUI_SAVE_WIDTH NUI_SAVE_HEIGHT NUI_SAVE_ADDRL NUI_SAVE_ADDRM NUI_SAVE_ADDRH
; @out A: 0 on success, 1 on error.
nui_restore_under:
      LDA   NUI_SAVE_VALID
      BEQ   @error
      JSR   nui_select_saved_region
      BNE   @error_clear
      JSR   vtext_calc_region_addr

      LDX   #0
      LDA   #BLT_SPACE_VGC_CHAR
      JSR   nui_restore_plane
      BNE   @error_clear
      LDX   #1
      LDA   #BLT_SPACE_VGC_COLOR
      JSR   nui_restore_plane
      BNE   @error_clear
      LDX   #2
      LDA   #BLT_SPACE_VGC_TEXTATTR
      JSR   nui_restore_plane
      BNE   @error_clear

      STZ   NUI_SAVE_VALID
      JMP   nui_ok
@error_clear:
      STZ   NUI_SAVE_VALID
@error:
      JMP   nui_error

; @label NUI.SAVE_UNDER_FULL
; @kind routine
; @symbol nui_save_under_full
; @summary Save the text and graphics planes under NUI_SAVE_* into caller-provided XRAM buffers.
; @requires NUI_SAVE_LEFT NUI_SAVE_TOP NUI_SAVE_WIDTH NUI_SAVE_HEIGHT NUI_SAVE_ADDRL NUI_SAVE_ADDRM NUI_SAVE_ADDRH
; @out A: 0 on success, 1 on error.
nui_save_under_full:
      JSR   nui_save_under
      BNE   @error
      JSR   nui_set_save_gfx_args
      JSR   vsprite_gfx_save_bg
      BNE   @error_clear
      LDA   #1
      STA   NUI_SAVE_VALID
      JMP   nui_ok
@error_clear:
      STZ   NUI_SAVE_VALID
@error:
      JMP   nui_error

; @label NUI.RESTORE_UNDER_FULL
; @kind routine
; @symbol nui_restore_under_full
; @summary Restore text and graphics planes previously saved by NUI.SAVE_UNDER_FULL.
; @requires NUI_SAVE_LEFT NUI_SAVE_TOP NUI_SAVE_WIDTH NUI_SAVE_HEIGHT NUI_SAVE_ADDRL NUI_SAVE_ADDRM NUI_SAVE_ADDRH
; @out A: 0 on success, 1 on error.
nui_restore_under_full:
      LDA   NUI_SAVE_VALID
      BEQ   @error
      JSR   nui_set_save_gfx_args
      JSR   vsprite_gfx_restore_bg
      BNE   @error_clear
      JMP   nui_restore_under
@error_clear:
      STZ   NUI_SAVE_VALID
@error:
      JMP   nui_error

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
      BNE   @done
      LDA   NUI_DIALOG_TOP
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
      LDA   NUI_LIST_FOCUS
      CMP   #NUI_LIST_FOCUS_OK
      BNE   :+
      LDA   #VTEXT_ATTR_REVERSE
      STA   VTEXT_ATTR
:     LDA   #<nui_list_ok
      LDY   #>nui_list_ok
      LDX   #6
      JSR   vtext_put_run
      BNE   @done
      STZ   VTEXT_ATTR
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
      LDA   NUI_LIST_FOCUS
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

nui_read_key_wait_byte:
      LDY   #$20
@outer:
      LDX   #$FF
@inner:
      LDA   VGC_CHARIN
      BNE   @done
      DEX
      BNE   @inner
      DEY
      BNE   @outer
      LDA   #$00
@done:
      RTS

nui_read_key:
@loop:
      LDA   VGC_CHARIN
      BEQ   @loop
      CMP   #NUI_KEY_ESCAPE
      BEQ   @escape
      RTS
@escape:
      JSR   nui_read_key_wait_byte
      BEQ   @plain_escape
      CMP   #'['
      BEQ   @csi
      CMP   #'O'
      BEQ   @csi
@plain_escape:
      LDA   #NUI_KEY_ESCAPE
      RTS
@csi:
      JSR   nui_read_key_wait_byte
      BEQ   @plain_escape
      CMP   #'A'
      BEQ   @ansi_up
      CMP   #'a'
      BEQ   @ansi_up
      CMP   #'B'
      BEQ   @ansi_down
      CMP   #'b'
      BEQ   @ansi_down
      CMP   #'C'
      BEQ   @ansi_right
      CMP   #'c'
      BEQ   @ansi_right
      CMP   #'D'
      BEQ   @ansi_left
      CMP   #'d'
      BEQ   @ansi_left
      CMP   #'Z'
      BEQ   @ansi_backtab
      CMP   #'z'
      BEQ   @ansi_backtab
      BRA   @plain_escape
@ansi_up:
      LDA   #NUI_KEY_UP
      RTS
@ansi_down:
      LDA   #NUI_KEY_DOWN
      RTS
@ansi_right:
      LDA   #NUI_KEY_RIGHT
      RTS
@ansi_left:
      LDA   #NUI_KEY_LEFT
      RTS
@ansi_backtab:
      LDA   #NUI_KEY_BACKTAB
      RTS

; @label NUI.DRAIN_KEYS
; @kind routine
; @symbol nui_drain_keys
; @summary Drain pending keyboard bytes before handing focus to a new modal control.
nui_drain_keys:
@loop:
      LDA   VGC_CHARIN
      BNE   @loop
      LDA   #NUI_OK
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
:     STZ   NUI_LIST_FOCUS
      JSR   nui_show_dialog
      BEQ   :+
      JMP   @error
:     JSR   nui_list_ensure_visible
      BEQ   :+
      JMP   @error
:     JSR   nui_list_render
      BEQ   :+
      JMP   @error
@key:
      JSR   nui_read_key
      STA   NUI_LIST_TMP
      CMP   #NUI_KEY_ENTER
      BEQ   @enter
      CMP   #NUI_KEY_ESCAPE
      BNE   :+
      JMP   @cancel
:     CMP   #NUI_KEY_TAB
      BEQ   @focus_next
      CMP   #NUI_KEY_RIGHT
      BEQ   @focus_next
      CMP   #NUI_KEY_LEFT
      BEQ   @focus_prev
      LDA   NUI_LIST_FOCUS
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
      BRA   @key
@focus_next:
      INC   NUI_LIST_FOCUS
      LDA   NUI_LIST_FOCUS
      CMP   #$03
      BCC   :+
      STZ   NUI_LIST_FOCUS
:     JSR   nui_list_render
      BNE   @error
      BRA   @key
@focus_prev:
      LDA   NUI_LIST_FOCUS
      BNE   :+
      LDA   #$03
:     DEC   A
      STA   NUI_LIST_FOCUS
      JSR   nui_list_render
      BNE   @error
      JMP   @key
@enter:
      LDA   NUI_LIST_FOCUS
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

nui_set_save_gfx_args:
      LDA   NUI_SAVE_LEFT
      JSR   nui_set_x_pixels
      LDA   NUI_SAVE_TOP
      JSR   nui_set_y_pixels
      LDA   NUI_SAVE_WIDTH
      JSR   nui_set_width_pixels
      LDA   NUI_SAVE_HEIGHT
      JSR   nui_set_height_pixels
      LDA   #BLT_SPACE_XRAM
      STA   VSPRITE_BGSPACE
      LDA   NUI_SAVE_ADDRL+3
      STA   VSPRITE_BGADDRL
      LDA   NUI_SAVE_ADDRM+3
      STA   VSPRITE_BGADDRM
      LDA   NUI_SAVE_ADDRH+3
      STA   VSPRITE_BGADDRH
      LDA   VSPRITE_WIDTHL
      STA   VSPRITE_BGSTRL
      LDA   VSPRITE_WIDTHH
      STA   VSPRITE_BGSTRH
      RTS

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
      JSR   nui_inc_x_pixel
      JSR   nui_inc_x_pixel
      INC   VSPRITE_Y
      INC   VSPRITE_Y
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
      JSR   nui_draw_text_frame
      JSR   nui_print_title
      JSR   nui_print_message
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

.endif

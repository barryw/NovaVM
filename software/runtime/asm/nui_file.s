; =====================================================================
;  Nova NDK — nui_file.s
;
;  Modal Turbo Vision-style file selector built over FIO directory
;  primitives. Owns navigation and selection; callers still perform
;  the actual load/save.
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

.include "nui.inc"
.include "fio.inc"

NUI_FILE_ROW_WIDTH = 36
NUI_FILE_MAX_ROWS  = 24
NUI_FILE_VISIBLE_ROWS = 14

NUI_FILE_DIALOG_LEFT = 20
NUI_FILE_DIALOG_TOP = 10
NUI_FILE_DIALOG_WIDTH = 40
NUI_FILE_DIALOG_HEIGHT = 28
NUI_FILE_FIELD_X = 32
NUI_FILE_FIELD_Y = 13
NUI_FILE_FIELD_WIDTH = 24
NUI_FILE_LIST_X = 22
NUI_FILE_LIST_Y = 16
NUI_FILE_BUTTON_Y = 32
NUI_FILE_BUTTON_SHADOW_Y = 33
NUI_FILE_PATH_Y = 35
NUI_FILE_OK_X = 31
NUI_FILE_CANCEL_X = 43
NUI_FILE_TEXT_DISABLED = $3C

NUI_FILE_ACTION_DRIVE  = $01
NUI_FILE_ACTION_PARENT = $02
NUI_FILE_ACTION_DIR    = $03
NUI_FILE_ACTION_FILE   = $04

NUI_FILE_ACT_NAV  = $00
NUI_FILE_ACT_DONE = $01
NUI_FILE_ACT_ERR  = $FF

NUI_FILE_FOCUS_NAME   = $00
NUI_FILE_FOCUS_LIST   = $01
NUI_FILE_FOCUS_OK     = $02
NUI_FILE_FOCUS_CANCEL = $03

      .segment "ZEROPAGE"

nf_srcL:      .res 1
nf_srcH:      .res 1
nf_dstL:      .res 1
nf_dstH:      .res 1
nf_rowL:      .res 1
nf_rowH:      .res 1

      .segment "BSS"

nf_name_pos:  .res 1
nf_src_len:   .res 1
nf_action:    .res 1
nf_tmp:       .res 1
nf_name_start:.res 1
nf_volume_view:.res 1
nf_focus:     .res 1
nf_cursor_pos:.res 1
nf_prev_selected:.res 1
nf_render_row:.res 1

nui_file_actions:  .res NUI_FILE_MAX_ROWS
nui_file_available:.res NUI_FILE_MAX_ROWS
nui_file_name_off: .res NUI_FILE_MAX_ROWS
nui_file_name_len: .res NUI_FILE_MAX_ROWS
nui_file_names:    .res 255

      .segment "CODE"

; @label NUI.FILE_PICKER
; @kind routine
; @symbol nui_file_picker
; @summary Draw a modal file picker with drive, parent-directory, directory, and file rows.
; @requires NUI_TITLEL/H NUI_FOOTERL/H NUI_DIALOG_* NUI_FILE_OUTL/H NUI_FILE_OUT_MAX NUI_FILE_TYPE_MASK/H
; @out A: 0 on success, 1 on error. NUI_RESULT = OK/CANCEL; NUI_FILE_OUT_LEN = selected name length.
nui_file_picker:
      STZ   NUI_FILE_OUT_LEN
      STZ   nf_cursor_pos
      STZ   nf_volume_view
      JSR   nui_file_set_geometry
      LDA   NUI_FILE_MODE
      CMP   #NUI_FILE_MODE_SAVE
      BNE   @open_focus
      LDA   #NUI_FILE_FOCUS_NAME
      BRA   @store_focus
@open_focus:
      LDA   #NUI_FILE_FOCUS_LIST
@store_focus:
      STA   nf_focus
@again:
      JSR   nui_file_build_rows
      BEQ   :+
      JMP   @error
:
      LDA   #NUI_FILE_VISIBLE_ROWS
      STA   NUI_LIST_VISIBLE
      JSR   nui_file_ensure_visible
      BEQ   :+
      JMP   @error
:
      LDA   NUI_FILE_MODE
      CMP   #NUI_FILE_MODE_SAVE
      BEQ   :+
      JSR   nui_file_selection_to_field
:
      JSR   nui_file_render
      BEQ   :+
      JMP   @error
:
@key:
      JSR   nui_read_key
      STA   nf_tmp
      CMP   #NUI_KEY_ESCAPE
      BNE   :+
      JMP   @cancel
:
      CMP   #NUI_KEY_TAB
      BNE   :+
      JMP   @focus_next
:     CMP   #NUI_KEY_BACKTAB
      BNE   :+
      JMP   @focus_prev
:
      CMP   #NUI_KEY_LEFT
      BNE   :+
      JMP   @left
:
      CMP   #NUI_KEY_RIGHT
      BNE   :+
      JMP   @right
:
      CMP   #NUI_KEY_UP
      BNE   :+
      JMP   @up
:     CMP   #NUI_KEY_DOWN
      BNE   :+
      JMP   @down
:     CMP   #NUI_KEY_ENTER
      BNE   :+
      JMP   @enter
:
      LDA   nf_focus
      CMP   #NUI_FILE_FOCUS_NAME
      BNE   @key
      LDA   nf_tmp
      CMP   #$08
      BNE   :+
      JMP   @backspace
:     CMP   #$7F
      BNE   :+
      JMP   @backspace
:
      CMP   #$20
      BCC   @key
      CMP   #$7F
      BCS   @key
      JSR   nui_file_field_char
      BEQ   :+
      JMP   @error
:
      JSR   nui_file_render
      BEQ   :+
      JMP   @error
:
      JMP   @key
@left:
      LDA   nf_focus
      CMP   #NUI_FILE_FOCUS_NAME
      BEQ   :+
      JMP   @key
:     LDA   nf_cursor_pos
      BNE   :+
      JMP   @key
:     DEC   nf_cursor_pos
      JSR   nui_file_render
      BEQ   :+
      JMP   @error
:
      JMP   @key
@right:
      LDA   nf_focus
      CMP   #NUI_FILE_FOCUS_NAME
      BEQ   :+
      JMP   @key
:     LDA   nf_cursor_pos
      CMP   NUI_FILE_OUT_LEN
      BCC   :+
      JMP   @key
:     INC   nf_cursor_pos
      JSR   nui_file_render
      BEQ   :+
      JMP   @error
:
      JMP   @key
@focus_next:
      INC   nf_focus
      LDA   nf_focus
      CMP   #NUI_FILE_FOCUS_CANCEL + 1
      BCC   :+
      STZ   nf_focus
:     JSR   nui_file_render
      BEQ   :+
      JMP   @error
:
      JMP   @key
@focus_prev:
      LDA   nf_focus
      BNE   :+
      LDA   #NUI_FILE_FOCUS_CANCEL + 1
:     DEC   A
      STA   nf_focus
      JSR   nui_file_render
      BEQ   :+
      JMP   @error
:
      JMP   @key
@up:
      LDA   nf_focus
      CMP   #NUI_FILE_FOCUS_LIST
      BEQ   :+
      JMP   @key
:
      LDA   NUI_LIST_SELECTED
      BNE   :+
      JMP   @key
:
      STA   nf_prev_selected
      DEC   NUI_LIST_SELECTED
      JSR   nui_file_skip_unavailable_up
      JSR   nui_file_ensure_visible
      BEQ   :+
      JMP   @error
:
      JSR   nui_file_selection_to_field
      JSR   nui_file_render
      BEQ   :+
      JMP   @error
:
      JMP   @key
@down:
      LDA   nf_focus
      CMP   #NUI_FILE_FOCUS_LIST
      BEQ   :+
      JMP   @key
:
      LDA   NUI_LIST_SELECTED
      STA   nf_prev_selected
      LDA   NUI_LIST_SELECTED
      CLC
      ADC   #1
      CMP   NUI_LIST_ROW_COUNT
      BCC   :+
      JMP   @key
:
      STA   NUI_LIST_SELECTED
      JSR   nui_file_skip_unavailable_down
      JSR   nui_file_ensure_visible
      BEQ   :+
      JMP   @error
:
      JSR   nui_file_selection_to_field
      JSR   nui_file_render
      BEQ   :+
      JMP   @error
:
      JMP   @key
@backspace:
      LDA   NUI_FILE_OUT_LEN
      BNE   :+
      JMP   @key
:
      LDA   nf_cursor_pos
      BNE   :+
      JMP   @key
:
      JSR   nui_file_output_ptr
      DEC   nf_cursor_pos
      LDX   nf_cursor_pos
@backspace_shift:
      TXA
      TAY
      INY
      LDA   (nf_dstL),Y
      DEY
      STA   (nf_dstL),Y
      INX
      CPX   NUI_FILE_OUT_LEN
      BCC   @backspace_shift
      DEC   NUI_FILE_OUT_LEN
      JSR   nui_file_render
      BEQ   :+
      JMP   @error
:
      JMP   @key
@enter:
      LDA   nf_focus
      CMP   #NUI_FILE_FOCUS_CANCEL
      BNE   :+
      JMP   @cancel
:
      CMP   #NUI_FILE_FOCUS_OK
      BEQ   @ok_button
      CMP   #NUI_FILE_FOCUS_NAME
      BEQ   @ok_button
      JSR   nui_file_activate_selected
      CMP   #NUI_FILE_ACT_DONE
      BEQ   @done
      CMP   #NUI_FILE_ACT_NAV
      BNE   :+
      JMP   @again
:
      JMP   @error
@ok_button:
      LDA   NUI_FILE_OUT_LEN
      BNE   @done
      JSR   nui_file_activate_selected
      CMP   #NUI_FILE_ACT_DONE
      BEQ   @done
      CMP   #NUI_FILE_ACT_NAV
      BNE   :+
      JMP   @again
:
      JMP   @error
@done:
      LDA   #NUI_RESULT_OK
      STA   NUI_RESULT
      STZ   VGC_CURSEN
@ok:
      LDA   #NUI_OK
      RTS
@cancel:
      STZ   NUI_FILE_OUT_LEN
      LDA   #NUI_RESULT_CANCEL
      STA   NUI_RESULT
      STZ   VGC_CURSEN
      LDA   #NUI_OK
      RTS
@error:
      STZ   VGC_CURSEN
      LDA   #NUI_ERR
      RTS

nui_file_set_geometry:
      STZ   VGC_CURSEN
      LDA   #NUI_FILE_DIALOG_LEFT
      STA   NUI_DIALOG_LEFT
      LDA   #NUI_FILE_DIALOG_TOP
      STA   NUI_DIALOG_TOP
      LDA   #NUI_FILE_DIALOG_WIDTH
      STA   NUI_DIALOG_WIDTH
      LDA   #NUI_FILE_DIALOG_HEIGHT
      STA   NUI_DIALOG_HEIGHT
      RTS

nui_file_build_rows:
      STZ   NUI_LIST_ROW_COUNT
      STZ   nf_name_pos

      LDA   nf_volume_view
      BEQ   @directory
      JSR   nui_file_append_drives
      BRA   @done

@directory:
      LDA   #NUI_FILE_ACTION_PARENT
      STA   nf_action
      LDA   #<nui_file_parent
      LDY   #>nui_file_parent
      LDX   #2
      JSR   nui_file_append_name

      STZ   FIO_NAMELEN
      JSR   fio_dir_open
      BNE   @done
@next:
      JSR   fio_dir_read
      BNE   @done
@entry:
      LDA   FIO_DIRTYPE
      CMP   #FIO_TYPE_DIR
      BEQ   @dir
      JSR   nui_file_type_allowed
      BEQ   @next
      LDA   #NUI_FILE_ACTION_FILE
      BRA   @append_fio
@dir:
      LDA   #NUI_FILE_ACTION_DIR
@append_fio:
      STA   nf_action
      LDA   #<FIO_NAME
      LDY   #>FIO_NAME
      LDX   FIO_NAMELEN
      JSR   nui_file_append_name
      BRA   @next
@done:
      LDA   NUI_LIST_SELECTED
      CMP   NUI_LIST_ROW_COUNT
      BCC   @ok
      STZ   NUI_LIST_SELECTED
@ok:
      JSR   nui_file_select_available
      LDA   #NUI_OK
      RTS

nui_file_append_drives:
      LDA   #<nui_file_drive_fd0
      LDY   #>nui_file_drive_fd0
      LDX   #4
      JSR   nui_file_append_drive
      LDA   #<nui_file_drive_fd1
      LDY   #>nui_file_drive_fd1
      LDX   #4
      JSR   nui_file_append_drive
      LDA   #<nui_file_drive_hd0
      LDY   #>nui_file_drive_hd0
      LDX   #4
      JSR   nui_file_append_drive
      LDA   #<nui_file_drive_hd1
      LDY   #>nui_file_drive_hd1
      LDX   #4
      JSR   nui_file_append_drive
      RTS

nui_file_append_drive:
      STA   nf_srcL
      STY   nf_srcH
      STX   nf_src_len
      LDA   #NUI_FILE_ACTION_DRIVE
      STA   nf_action
      LDA   nf_srcL
      LDY   nf_srcH
      LDX   nf_src_len
      JSR   nui_file_append_name
      LDA   NUI_LIST_ROW_COUNT
      BEQ   @done
      SEC
      SBC   #1
      STA   nf_render_row
      JSR   nui_file_probe_drive
      LDX   nf_render_row
      STA   nui_file_available,X
@done:
      RTS

nui_file_probe_drive:
      LDA   nf_src_len
      CMP   #FIO_NAME_LIMIT + 1
      BCS   @unavailable
      STA   FIO_NAMELEN
      LDY   #0
@copy:
      CPY   nf_src_len
      BCS   @query
      LDA   (nf_srcL),Y
      STA   FIO_NAME,Y
      INY
      BRA   @copy
@query:
      JSR   fio_devstatus
      BEQ   @available
@unavailable:
      LDA   #0
      RTS
@available:
      LDA   #1
      RTS

nui_file_parent_nav:
      JSR   fio_pwd
      BNE   @error
      LDA   FIO_NAMELEN
      CMP   #6
      BCC   @volumes
      LDX   FIO_NAMELEN
      DEX
@find_slash:
      CPX   #4
      BCC   @root
      LDA   FIO_NAME,X
      CMP   #'/'
      BEQ   @slash
      DEX
      BRA   @find_slash
@slash:
      CPX   #5
      BCC   @root
      STX   FIO_NAMELEN
      BRA   @cd
@root:
      LDA   #'/'
      STA   FIO_NAME+4
      LDA   #5
      STA   FIO_NAMELEN
@cd:
      JSR   fio_cd
      BNE   @error
      STZ   nf_volume_view
      LDA   #NUI_OK
      RTS
@volumes:
      LDA   #1
      STA   nf_volume_view
      STZ   NUI_LIST_FIRST
      LDA   #NUI_OK
      RTS
@error:
      LDA   #NUI_ERR
      RTS

nui_file_dir_nav:
      JSR   nui_file_selected_name
      BNE   @error
      JSR   fio_pwd
      BNE   @error
      LDA   FIO_NAMELEN
      CLC
      ADC   nf_src_len
      CLC
      ADC   #1
      CMP   #FIO_NAME_LIMIT + 1
      BCS   @error
      LDX   FIO_NAMELEN
      CPX   #5
      BNE   @slash
      LDA   FIO_NAME+4
      CMP   #'/'
      BEQ   @copy
@slash:
      LDA   #'/'
      STA   FIO_NAME,X
      INX
@copy:
      LDY   #0
@copy_loop:
      CPY   nf_src_len
      BCS   @cd
      LDA   (nf_srcL),Y
      STA   FIO_NAME,X
      INX
      INY
      BRA   @copy_loop
@cd:
      STX   FIO_NAMELEN
      JSR   fio_cd
      BNE   @error
      STZ   nf_volume_view
      LDA   #NUI_OK
      RTS
@error:
      LDA   #NUI_ERR
      RTS

nui_file_output_ptr:
      LDA   NUI_FILE_OUTL
      STA   nf_dstL
      LDA   NUI_FILE_OUTH
      STA   nf_dstH
      RTS

nui_file_field_char:
      LDA   NUI_FILE_OUT_LEN
      CLC
      ADC   #1
      CMP   NUI_FILE_OUT_MAX
      BCS   @ok
      JSR   nui_file_output_ptr
      LDX   NUI_FILE_OUT_LEN
@shift:
      CPX   nf_cursor_pos
      BEQ   @insert
      DEX
      TXA
      TAY
      LDA   (nf_dstL),Y
      INY
      STA   (nf_dstL),Y
      BRA   @shift
@insert:
      LDY   nf_cursor_pos
      LDA   nf_tmp
      STA   (nf_dstL),Y
      INC   NUI_FILE_OUT_LEN
      INC   nf_cursor_pos
      LDY   NUI_FILE_OUT_LEN
      LDA   #0
      STA   (nf_dstL),Y
@ok:
      LDA   #NUI_OK
      RTS

nui_file_selection_to_field:
      LDX   NUI_LIST_SELECTED
      LDA   nui_file_actions,X
      CMP   #NUI_FILE_ACTION_FILE
      BEQ   :+
      LDA   NUI_FILE_MODE
      CMP   #NUI_FILE_MODE_SAVE
      BEQ   @ok
      STZ   NUI_FILE_OUT_LEN
      STZ   nf_cursor_pos
      BRA   @ok
:     JSR   nui_file_selected_to_out
      BNE   @error
      LDA   NUI_FILE_OUT_LEN
      STA   nf_cursor_pos
@ok:
      LDA   #NUI_OK
      RTS
@error:
      LDA   #NUI_ERR
      RTS

nui_file_activate_selected:
      LDX   NUI_LIST_SELECTED
      LDA   nui_file_actions,X
      CMP   #NUI_FILE_ACTION_FILE
      BEQ   @file
      CMP   #NUI_FILE_ACTION_PARENT
      BEQ   @parent
      CMP   #NUI_FILE_ACTION_DIR
      BEQ   @dir
      LDA   nui_file_available,X
      BEQ   @nav
      JSR   nui_file_selected_to_fio
      BNE   @error
      JSR   fio_cd
      BNE   @nav
      STZ   nf_volume_view
      STZ   NUI_LIST_SELECTED
@nav:
      LDA   #NUI_FILE_ACT_NAV
      RTS
@parent:
      JSR   nui_file_parent_nav
      BNE   @error
      STZ   NUI_LIST_SELECTED
      LDA   #NUI_FILE_ACT_NAV
      RTS
@dir:
      JSR   nui_file_dir_nav
      BNE   @error
      STZ   NUI_LIST_SELECTED
      LDA   #NUI_FILE_ACT_NAV
      RTS
@file:
      JSR   nui_file_selected_to_out
      BNE   @error
      LDA   #NUI_FILE_ACT_DONE
      RTS
@error:
      LDA   #NUI_FILE_ACT_ERR
      RTS

nui_file_select_available:
      LDA   nf_volume_view
      BNE   @volume
      LDA   #NUI_OK
      RTS
@volume:
      LDX   NUI_LIST_SELECTED
      CPX   NUI_LIST_ROW_COUNT
      BCS   @scan
      LDA   nui_file_available,X
      BNE   @ok
@scan:
      LDX   #0
@scan_loop:
      CPX   NUI_LIST_ROW_COUNT
      BCS   @none
      LDA   nui_file_available,X
      BNE   @store
      INX
      BRA   @scan_loop
@store:
      STX   NUI_LIST_SELECTED
      BRA   @ok
@none:
      STZ   NUI_LIST_SELECTED
@ok:
      LDA   #NUI_OK
      RTS

nui_file_skip_unavailable_up:
      LDA   nf_volume_view
      BNE   @loop
      LDA   #NUI_OK
      RTS
@loop:
      LDX   NUI_LIST_SELECTED
      LDA   nui_file_available,X
      BNE   @ok
      TXA
      BEQ   @restore
      DEC   NUI_LIST_SELECTED
      BRA   @loop
@restore:
      LDA   nf_prev_selected
      STA   NUI_LIST_SELECTED
@ok:
      LDA   #NUI_OK
      RTS

nui_file_skip_unavailable_down:
      LDA   nf_volume_view
      BNE   @loop
      LDA   #NUI_OK
      RTS
@loop:
      LDX   NUI_LIST_SELECTED
      LDA   nui_file_available,X
      BNE   @ok
      INX
      CPX   NUI_LIST_ROW_COUNT
      BCS   @restore
      STX   NUI_LIST_SELECTED
      BRA   @loop
@restore:
      LDA   nf_prev_selected
      STA   NUI_LIST_SELECTED
@ok:
      LDA   #NUI_OK
      RTS

nui_file_ensure_visible:
      LDA   nf_volume_view
      BEQ   @directory
      LDA   NUI_LIST_SELECTED
      CMP   NUI_LIST_FIRST
      BCS   @check_bottom
      STA   NUI_LIST_FIRST
      LDA   #NUI_OK
      RTS
@check_bottom:
      LDA   NUI_LIST_FIRST
      CLC
      ADC   #NUI_FILE_VISIBLE_ROWS
      CMP   NUI_LIST_SELECTED
      BEQ   @adjust
      BCS   @ok
@adjust:
      LDA   NUI_LIST_SELECTED
      SEC
      SBC   #NUI_FILE_VISIBLE_ROWS
      CLC
      ADC   #1
      STA   NUI_LIST_FIRST
@ok:
      LDA   #NUI_OK
      RTS
@directory:
      LDA   NUI_LIST_SELECTED
      BNE   :+
      LDA   #1
      STA   NUI_LIST_FIRST
      LDA   #NUI_OK
      RTS
:     LDA   NUI_LIST_FIRST
      CMP   #1
      BCS   :+
      LDA   #1
      STA   NUI_LIST_FIRST
:     LDA   NUI_LIST_SELECTED
      CMP   NUI_LIST_FIRST
      BCS   @check_directory_bottom
      STA   NUI_LIST_FIRST
      LDA   #NUI_OK
      RTS
@check_directory_bottom:
      LDA   NUI_LIST_FIRST
      CLC
      ADC   #NUI_FILE_VISIBLE_ROWS - 1
      CMP   NUI_LIST_SELECTED
      BEQ   @adjust_directory
      BCS   @ok
@adjust_directory:
      LDA   NUI_LIST_SELECTED
      SEC
      SBC   #NUI_FILE_VISIBLE_ROWS - 1
      CLC
      ADC   #1
      CMP   #1
      BCS   :+
      LDA   #1
:     STA   NUI_LIST_FIRST
      LDA   #NUI_OK
      RTS

nui_file_render:
      JSR   nui_show_dialog
      BNE   @done
      JSR   nui_set_screen_text
      STZ   VTEXT_ATTR
      LDA   #NUI_TEXT_NORMAL
      STA   VTEXT_COLOR
      LDA   #NUI_FILE_LIST_X
      STA   VTEXT_CURX
      LDA   #NUI_FILE_FIELD_Y
      STA   VTEXT_CURY
      LDA   #<nui_file_label_filename
      LDY   #>nui_file_label_filename
      LDX   #8
      JSR   vtext_put_run
      BNE   @done
      JSR   nui_file_render_field
      BNE   @done
      JSR   nui_file_render_list
      BNE   @done
      JSR   nui_file_render_buttons
      BNE   @done
      JSR   nui_file_render_path
      BNE   @done
      JSR   nui_file_place_cursor
@done:
      RTS

nui_file_render_field:
      LDA   #NUI_FILE_FIELD_X
      STA   VTEXT_CURX
      LDA   #NUI_FILE_FIELD_Y
      STA   VTEXT_CURY
      LDA   #NUI_TEXT_FIELD
      STA   VTEXT_COLOR
      LDA   nf_focus
      CMP   #NUI_FILE_FOCUS_NAME
      BNE   :+
      LDA   #VTEXT_ATTR_REVERSE
      STA   VTEXT_ATTR
:     LDA   #<nui_file_field_blank
      LDY   #>nui_file_field_blank
      LDX   #NUI_FILE_FIELD_WIDTH
      JSR   vtext_put_run
      BNE   @done
      LDA   #NUI_FILE_FIELD_X
      STA   VTEXT_CURX
      LDA   #NUI_FILE_FIELD_Y
      STA   VTEXT_CURY
      LDA   NUI_FILE_OUTL
      LDY   NUI_FILE_OUTH
      LDX   NUI_FILE_OUT_LEN
      CPX   #NUI_FILE_FIELD_WIDTH + 1
      BCC   :+
      LDX   #NUI_FILE_FIELD_WIDTH
:     JSR   vtext_put_run
      STZ   VTEXT_ATTR
@done:
      RTS

nui_file_render_list:
      STZ   NUI_LIST_INDEX
@clear:
      LDA   NUI_LIST_INDEX
      CMP   #NUI_FILE_VISIBLE_ROWS
      BCS   @rows
      LDA   #NUI_FILE_LIST_X
      STA   VTEXT_CURX
      LDA   #NUI_FILE_LIST_Y
      CLC
      ADC   NUI_LIST_INDEX
      STA   VTEXT_CURY
      LDA   #NUI_TEXT_FIELD
      STA   VTEXT_COLOR
      STZ   VTEXT_ATTR
      LDA   #<nui_file_row_blank
      LDY   #>nui_file_row_blank
      LDX   #NUI_FILE_ROW_WIDTH
      JSR   vtext_put_run
      BEQ   :+
      JMP   @done
:
      INC   NUI_LIST_INDEX
      BRA   @clear
@rows:
      STZ   NUI_LIST_INDEX
@row_loop:
      LDA   NUI_LIST_INDEX
      CMP   #NUI_FILE_VISIBLE_ROWS
      BCS   @ok
      LDA   nf_volume_view
      BNE   @volume_row
      LDA   NUI_LIST_INDEX
      BNE   @directory_row
      LDA   #0
      BRA   @row_ready
@directory_row:
      LDA   NUI_LIST_FIRST
      CLC
      ADC   NUI_LIST_INDEX
      SEC
      SBC   #1
      BRA   @row_ready
@volume_row:
      LDA   NUI_LIST_FIRST
      CLC
      ADC   NUI_LIST_INDEX
@row_ready:
      CMP   NUI_LIST_ROW_COUNT
      BCS   @ok
      PHA
      TAX
      JSR   nui_file_row_ptr
      PLA
      STA   nf_render_row
      LDA   #NUI_TEXT_FIELD
      STA   VTEXT_COLOR
      LDA   nf_volume_view
      BEQ   @selectable
      LDX   nf_render_row
      LDA   nui_file_available,X
      BNE   @selectable
      LDA   #NUI_FILE_TEXT_DISABLED
      STA   VTEXT_COLOR
      STZ   VTEXT_ATTR
      BRA   @print_row
@selectable:
      LDA   nf_render_row
      CMP   NUI_LIST_SELECTED
      BNE   :+
      LDA   #VTEXT_ATTR_REVERSE
      STA   VTEXT_ATTR
      BRA   @print_row
:     STZ   VTEXT_ATTR
@print_row:
      LDA   #NUI_FILE_LIST_X
      STA   VTEXT_CURX
      LDA   #NUI_FILE_LIST_Y
      CLC
      ADC   NUI_LIST_INDEX
      STA   VTEXT_CURY
      LDA   nf_rowL
      LDY   nf_rowH
      LDX   #NUI_FILE_ROW_WIDTH
      JSR   vtext_put_run
      BNE   @done
      INC   NUI_LIST_INDEX
      JMP   @row_loop
@ok:
      STZ   VTEXT_ATTR
      LDA   #NUI_OK
@done:
      RTS

nui_file_render_buttons:
      JSR   nui_set_screen_text
      STZ   VTEXT_ATTR
      LDA   #NUI_TEXT_SHADOW
      STA   VTEXT_COLOR
      LDA   #NUI_FILE_OK_X + 1
      STA   VTEXT_CURX
      LDA   #NUI_FILE_BUTTON_SHADOW_Y
      STA   VTEXT_CURY
      LDA   #<nui_file_button6_shadow
      LDY   #>nui_file_button6_shadow
      LDX   #6
      JSR   vtext_put_run
      BNE   @done
      LDA   #NUI_FILE_CANCEL_X + 1
      STA   VTEXT_CURX
      LDA   #<nui_file_button8_shadow
      LDY   #>nui_file_button8_shadow
      LDX   #8
      JSR   vtext_put_run
      BNE   @done
      LDA   #NUI_FILE_OK_X
      LDY   #NUI_FILE_FOCUS_OK
      JSR   nui_file_render_button6
      BNE   @done
      LDA   #NUI_FILE_CANCEL_X
      LDY   #NUI_FILE_FOCUS_CANCEL
      JSR   nui_file_render_cancel
@done:
      RTS

nui_file_render_button6:
      STA   VTEXT_CURX
      LDA   #NUI_FILE_BUTTON_Y
      STA   VTEXT_CURY
      LDA   #NUI_TEXT_BUTTON
      STA   VTEXT_COLOR
      CPY   nf_focus
      BNE   :+
      LDA   #VTEXT_ATTR_REVERSE
      STA   VTEXT_ATTR
:     LDA   #<nui_file_ok
      LDY   #>nui_file_ok
      LDX   #6
      JSR   vtext_put_run
      STZ   VTEXT_ATTR
      RTS

nui_file_render_cancel:
      STA   VTEXT_CURX
      LDA   #NUI_FILE_BUTTON_Y
      STA   VTEXT_CURY
      LDA   #NUI_TEXT_BUTTON
      STA   VTEXT_COLOR
      CPY   nf_focus
      BNE   :+
      LDA   #VTEXT_ATTR_REVERSE
      STA   VTEXT_ATTR
:     LDA   #<nui_file_cancel
      LDY   #>nui_file_cancel
      LDX   #8
      JSR   vtext_put_run
      STZ   VTEXT_ATTR
      RTS

nui_file_render_path:
      JSR   nui_set_screen_text
      STZ   VTEXT_ATTR
      LDA   #NUI_TEXT_NORMAL
      STA   VTEXT_COLOR
      LDA   #NUI_FILE_LIST_X
      STA   VTEXT_CURX
      LDA   #NUI_FILE_PATH_Y
      STA   VTEXT_CURY
      LDA   #<nui_file_row_blank
      LDY   #>nui_file_row_blank
      LDX   #NUI_FILE_ROW_WIDTH
      JSR   vtext_put_run
      BNE   @done
      LDA   #NUI_FILE_LIST_X
      STA   VTEXT_CURX
      LDA   #NUI_FILE_PATH_Y
      STA   VTEXT_CURY
      LDA   #<nui_file_label_path
      LDY   #>nui_file_label_path
      LDX   #6
      JSR   vtext_put_run
      BNE   @done
      LDA   nf_volume_view
      BEQ   @pwd
      LDA   #<nui_file_volumes
      LDY   #>nui_file_volumes
      LDX   #7
      JMP   vtext_put_run
@pwd:
      JSR   fio_pwd
      BNE   @done
      LDA   FIO_NAMELEN
      CMP   #31
      BCC   :+
      LDA   #30
:     TAX
      LDA   #<FIO_NAME
      LDY   #>FIO_NAME
      JSR   vtext_put_run
@done:
      RTS

nui_file_place_cursor:
      LDA   nf_focus
      CMP   #NUI_FILE_FOCUS_NAME
      BEQ   :+
      STZ   VGC_CURSEN
      LDA   #NUI_OK
      RTS
:     LDA   #1
      STA   VGC_CURSEN
      LDA   nf_cursor_pos
      CMP   #NUI_FILE_FIELD_WIDTH
      BCC   :+
      LDA   #NUI_FILE_FIELD_WIDTH - 1
:     CLC
      ADC   #NUI_FILE_FIELD_X
      STA   VGC_CURSX
      LDA   #NUI_FILE_FIELD_Y
      STA   VGC_CURSY
      LDA   #NUI_OK
      RTS

; A/Y=source ptr, X=len, nf_action set.
nui_file_append_name:
      CPX   #1
      BCS   :+
      LDA   #NUI_OK
      RTS
:     STA   nf_srcL
      STY   nf_srcH
      STX   nf_src_len
      LDA   NUI_LIST_ROW_COUNT
      CMP   #NUI_FILE_MAX_ROWS
      BCC   :+
      LDA   #NUI_OK
      RTS
:     LDA   nf_name_pos
      STA   nf_name_start
      CLC
      ADC   nf_src_len
      BCC   :+
      LDA   #NUI_OK
      RTS
:     STA   nf_name_pos
      LDX   NUI_LIST_ROW_COUNT
      LDA   nf_name_start
      STA   nui_file_name_off,X
      LDA   nf_src_len
      STA   nui_file_name_len,X
      LDA   nf_action
      STA   nui_file_actions,X
      LDA   #1
      STA   nui_file_available,X
      LDA   nf_name_start
      CLC
      ADC   #<nui_file_names
      STA   nf_dstL
      LDA   #>nui_file_names
      ADC   #0
      STA   nf_dstH
      LDY   #0
@copy_name:
      CPY   nf_src_len
      BCS   @render
      LDA   (nf_srcL),Y
      STA   (nf_dstL),Y
      INY
      BRA   @copy_name
@render:
      LDX   NUI_LIST_ROW_COUNT
      JSR   nui_file_row_ptr
      LDY   #0
      LDA   #' '
@clear:
      STA   (nf_rowL),Y
      INY
      CPY   #NUI_FILE_ROW_WIDTH
      BCC   @clear
      LDY   #0
@display:
      CPY   nf_src_len
      BCS   @maybe_slash
      CPY   #(NUI_FILE_ROW_WIDTH-1)
      BCS   @maybe_slash
      LDA   (nf_srcL),Y
      STA   (nf_rowL),Y
      INY
      BRA   @display
@maybe_slash:
      LDA   nf_action
      CMP   #NUI_FILE_ACTION_DIR
      BNE   @inc
      CPY   #NUI_FILE_ROW_WIDTH
      BCS   @inc
      LDA   #'/'
      STA   (nf_rowL),Y
@inc:
      INC   NUI_LIST_ROW_COUNT
@done:
      LDA   #NUI_OK
      RTS

; X=row index -> nf_rowL/H.
nui_file_row_ptr:
      STX   nf_tmp
      LDA   NUI_FILE_ROWSL
      STA   nf_rowL
      LDA   NUI_FILE_ROWSH
      STA   nf_rowH
@loop:
      LDA   nf_tmp
      BEQ   @done
      CLC
      LDA   nf_rowL
      ADC   #NUI_FILE_ROW_WIDTH
      STA   nf_rowL
      BCC   :+
      INC   nf_rowH
:     DEC   nf_tmp
      BRA   @loop
@done:
      RTS

nui_file_selected_name:
      LDX   NUI_LIST_SELECTED
      LDA   nui_file_name_len,X
      BEQ   @error
      STA   nf_src_len
      LDA   nui_file_name_off,X
      CLC
      ADC   #<nui_file_names
      STA   nf_srcL
      LDA   #>nui_file_names
      ADC   #0
      STA   nf_srcH
      LDA   #NUI_OK
      RTS
@error:
      LDA   #NUI_ERR
      RTS

nui_file_selected_to_fio:
      JSR   nui_file_selected_name
      BNE   @done
      LDA   nf_src_len
      CMP   #FIO_NAME_LIMIT + 1
      BCS   @error
      STA   FIO_NAMELEN
      LDY   #0
@copy:
      CPY   nf_src_len
      BCS   @ok
      LDA   (nf_srcL),Y
      STA   FIO_NAME,Y
      INY
      BRA   @copy
@ok:
      LDA   #NUI_OK
@done:
      RTS
@error:
      LDA   #NUI_ERR
      RTS

nui_file_selected_to_out:
      JSR   nui_file_selected_name
      BNE   @done
      LDA   nf_src_len
      CMP   NUI_FILE_OUT_MAX
      BCS   @error
      STA   NUI_FILE_OUT_LEN
      LDA   NUI_FILE_OUTL
      STA   nf_dstL
      LDA   NUI_FILE_OUTH
      STA   nf_dstH
      LDY   #0
@copy:
      CPY   nf_src_len
      BCS   @nul
      LDA   (nf_srcL),Y
      STA   (nf_dstL),Y
      INY
      BRA   @copy
@nul:
      CPY   NUI_FILE_OUT_MAX
      BCS   @ok
      LDA   #0
      STA   (nf_dstL),Y
@ok:
      LDA   #NUI_OK
@done:
      RTS
@error:
      STZ   NUI_FILE_OUT_LEN
      LDA   #NUI_ERR
      RTS

nui_file_type_allowed:
      LDA   NUI_FILE_TYPE_MASK
      ORA   NUI_FILE_TYPE_MASKH
      BEQ   @include
      LDX   FIO_DIRTYPE
      CPX   #8
      BCS   @high
      LDA   nui_file_type_bits,X
      AND   NUI_FILE_TYPE_MASK
      RTS
@high:
      CPX   #16
      BCS   @exclude
      TXA
      SEC
      SBC   #8
      TAX
      LDA   nui_file_type_bits,X
      AND   NUI_FILE_TYPE_MASKH
      RTS
@include:
      LDA   #1
      RTS
@exclude:
      LDA   #0
      RTS

nui_file_type_bits:
      .byte $01, $02, $04, $08, $10, $20, $40, $80

nui_file_drive_fd0: .byte "FD0:"
nui_file_drive_fd1: .byte "FD1:"
nui_file_drive_hd0: .byte "HD0:"
nui_file_drive_hd1: .byte "HD1:"
nui_file_parent:    .byte ".."
nui_file_label_filename: .byte "Filename"
nui_file_label_path:     .byte "Path: "
nui_file_volumes:        .byte "Volumes"
nui_file_ok:             .byte "  OK  "
nui_file_cancel:         .byte " Cancel "
nui_file_button6_shadow: .byte $DC,$DC,$DC,$DC,$DC,$DC
nui_file_button8_shadow: .byte $DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC
nui_file_field_blank:    .byte "                        "
nui_file_row_blank:      .byte "                                    "

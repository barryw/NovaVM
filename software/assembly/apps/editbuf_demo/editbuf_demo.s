; editbuf_demo.s -- standalone native editor + shared NUI dialog demo.

.include "nova.inc"
.include "editui.inc"
.include "editbuf.inc"
.include "nui.inc"

TEXT_BUF = $0400
UNDO_BUF = $0C00
REDO_BUF = $1400
TEXT_CAP = 2048

.segment "HEADER"
      .byte $00, $50

.segment "BSS"

demo_file_out:   .res 64
demo_file_rows:  .res 36 * 24
demo_name_out:   .res 32

.segment "CODE"

start:
      SEI
      CLD
      LDX   #$FF
      TXS

      LDX   #0
@seed:
      CPX   #demo_text_len
      BCS   @seeded
      LDA   demo_text,X
      STA   TEXT_BUF,X
      INX
      BRA   @seed
@seeded:
      LDA   #<TEXT_BUF
      STA   EDITBUF_BUFL
      LDA   #>TEXT_BUF
      STA   EDITBUF_BUFH
      LDA   #<TEXT_CAP
      STA   EDITBUF_CAPL
      LDA   #>TEXT_CAP
      STA   EDITBUF_CAPH
      LDA   #<UNDO_BUF
      STA   EDITBUF_UNDOBUFL
      LDA   #>UNDO_BUF
      STA   EDITBUF_UNDOBUFH
      LDA   #<REDO_BUF
      STA   EDITBUF_REDOBUFL
      LDA   #>REDO_BUF
      STA   EDITBUF_REDOBUFH
      LDA   #<demo_text_len
      STA   EDITBUF_LENL
      LDA   #>demo_text_len
      STA   EDITBUF_LENH
      LDA   #<demo_title
      STA   EDITBUF_TITLEL
      LDA   #>demo_title
      STA   EDITBUF_TITLEH
      LDA   #<demo_type
      STA   EDITBUF_TYPEL
      LDA   #>demo_type
      STA   EDITBUF_TYPEH
      STZ   EDITBUF_STATUSL
      STZ   EDITBUF_STATUSH

      LDA   #<demo_save_hook
      STA   EDITBUF_SAVE_VECL
      LDA   #>demo_save_hook
      STA   EDITBUF_SAVE_VECH
      LDA   #<demo_indent_hook
      STA   EDITBUF_INDENT_VECL
      LDA   #>demo_indent_hook
      STA   EDITBUF_INDENT_VECH
      STZ   EDITBUF_HILITE_VECL
      STZ   EDITBUF_HILITE_VECH
      LDA   #<demo_command_hook
      STA   EDITBUF_COMMAND_VECL
      LDA   #>demo_command_hook
      STA   EDITBUF_COMMAND_VECH
      STZ   EDITBUF_MENU_VECL
      STZ   EDITBUF_MENU_VECH

      JSR   editbuf_reset_state
      JSR   editbuf_run
      JMP   start

demo_indent_hook:
      LDA   #2
      RTS

demo_save_hook:
      JSR   demo_show_save_picker
      LDA   NUI_RESULT
      CMP   #NUI_RESULT_OK
      BNE   @cancel
      LDA   #EDITBUF_SAVE_OK
      RTS
@cancel:
      LDA   #EDITBUF_SAVE_ERROR
      RTS

demo_command_hook:
      CMP   #EDITUI_CMD_OPEN
      BEQ   demo_show_open_picker
      CMP   #EDITUI_CMD_NEW
      BEQ   demo_show_new_dialog
      CMP   #EDITUI_CMD_SAVE_AS
      BEQ   demo_show_save_picker
      RTS

demo_show_open_picker:
      LDA   #<demo_open_title
      LDY   #>demo_open_title
      LDX   #NUI_FILE_MODE_OPEN
      BRA   demo_show_file_picker

demo_show_save_picker:
      LDA   #<demo_save_title
      LDY   #>demo_save_title
      LDX   #NUI_FILE_MODE_SAVE
demo_show_file_picker:
      STA   NUI_TITLEL
      STY   NUI_TITLEH
      STX   NUI_FILE_MODE
      STZ   NUI_FOOTERL
      STZ   NUI_FOOTERH
      LDA   #<demo_file_out
      STA   NUI_FILE_OUTL
      LDA   #>demo_file_out
      STA   NUI_FILE_OUTH
      LDA   #64
      STA   NUI_FILE_OUT_MAX
      LDA   #<NUI_FILE_TYPE_PASCAL
      STA   NUI_FILE_TYPE_MASK
      LDA   #>NUI_FILE_TYPE_PASCAL
      STA   NUI_FILE_TYPE_MASKH
      LDA   #<demo_file_rows
      STA   NUI_FILE_ROWSL
      LDA   #>demo_file_rows
      STA   NUI_FILE_ROWSH
      STZ   NUI_LIST_SELECTED
      LDA   #8
      STA   NUI_DIALOG_LEFT
      LDA   #6
      STA   NUI_DIALOG_TOP
      LDA   #56
      STA   NUI_DIALOG_WIDTH
      LDA   #14
      STA   NUI_DIALOG_HEIGHT
      JMP   nui_file_picker

demo_show_new_dialog:
      LDA   #<demo_new_title
      STA   NUI_TITLEL
      LDA   #>demo_new_title
      STA   NUI_TITLEH
      STZ   NUI_MSGL
      STZ   NUI_MSGH
      LDA   #<demo_name_label
      STA   NUI_INPUT_LABELL
      LDA   #>demo_name_label
      STA   NUI_INPUT_LABELH
      LDA   #<demo_name_out
      STA   NUI_INPUT_OUTL
      LDA   #>demo_name_out
      STA   NUI_INPUT_OUTH
      LDA   #32
      STA   NUI_INPUT_OUT_MAX
      STZ   NUI_FOOTERL
      STZ   NUI_FOOTERH
      LDA   #14
      STA   NUI_DIALOG_LEFT
      LDA   #12
      STA   NUI_DIALOG_TOP
      LDA   #52
      STA   NUI_DIALOG_WIDTH
      LDA   #12
      STA   NUI_DIALOG_HEIGHT
      JMP   nui_text_input

.segment "RODATA"

demo_title:
      .byte "demo/NOVAPASCAL.PAS", 0
demo_type:
      .byte "Pascal", 0
demo_open_title:
      .byte "Open", 0
demo_save_title:
      .byte "Save File As", 0
demo_new_title:
      .byte "New Source", 0
demo_name_label:
      .byte "Name:   "
demo_text:
      .byte "program Demo;", $0A
      .byte "begin", $0A
      .byte "  writeln('Nova Pascal editor demo');", $0A
      .byte "end.", $0A
demo_text_end:
demo_text_len = demo_text_end - demo_text

; editui_shell.s -- visual smoke demo for the shared Nova editor shell.
;
; Load address: $7200   Invoke: SYS $7200

.include "nova.inc"
.include "editui.inc"

.macro SET_PTR lo, hi, label
      LDA   #<label
      STA   lo
      LDA   #>label
      STA   hi
.endmacro

.macro DRAW_BOX xpos, ypos, bw, bh, bstyle, btitle
      LDA   #xpos
      STA   EDITUI_BOXX
      LDA   #ypos
      STA   EDITUI_BOXY
      LDA   #bw
      STA   EDITUI_BOXW
      LDA   #bh
      STA   EDITUI_BOXH
      LDA   #bstyle
      STA   EDITUI_BOX_STYLE
      SET_PTR EDITUI_BOX_TITLEL, EDITUI_BOX_TITLEH, btitle
      JSR   editui_draw_box
.endmacro

.macro PRINT_AT xpos, ypos, pcolor, ptext
      LDA   #xpos
      STA   EDITUI_PRINTX
      LDA   #ypos
      STA   EDITUI_PRINTY
      LDA   #pcolor
      STA   VTEXT_COLOR
      SET_PTR EDITUI_PRINTL, EDITUI_PRINTH, ptext
      JSR   editui_print_ptr
.endmacro

.macro SELECT_BODY xpos, ypos, bw, bh
      LDA   #xpos
      STA   EDITUI_BOXX
      LDA   #ypos
      STA   EDITUI_BOXY
      LDA   #bw
      STA   EDITUI_BOXW
      LDA   #bh
      STA   EDITUI_BOXH
      JSR   editui_select_box_body
.endmacro

.macro PRINT_BODY pcolor, ptext
      LDA   #pcolor
      STA   VTEXT_COLOR
      LDA   #<ptext
      LDY   #>ptext
      JSR   vtext_puts
.endmacro

KEY_CTRL_Q = $11

.segment "HEADER"
      .byte $00, $72

.segment "CODE"

start:
      JSR   editui_init
      SET_PTR EDITUI_TITLEL, EDITUI_TITLEH, msg_title
      SET_PTR EDITUI_HELPL, EDITUI_HELPH, msg_help
      SET_PTR EDITUI_STATUSL, EDITUI_STATUSH, msg_status
      JSR   draw_screen

main_loop:
      LDA   VGC_CHARIN
      BEQ   main_loop
      CMP   #EDITUI_KEY_ALT_PREFIX
      BEQ   handle_alt_or_escape
      CMP   #KEY_CTRL_Q
      BNE   main_loop
done:
      JMP   editui_shutdown

draw_screen:
      JSR   editui_draw_shell
      JSR   draw_panels
      RTS

handle_alt_or_escape:
      LDA   VGC_CHARIN
      BEQ   main_loop
      JSR   editui_menu_open_hotkey
      CMP   #EDITUI_CMD_QUIT
      BEQ   done
      BRA   main_loop

draw_panels:
      DRAW_BOX 1, 7, 15, 14, EDITUI_BOX_SINGLE, msg_tools_title
      DRAW_BOX 17, 7, 37, 25, EDITUI_BOX_SINGLE, msg_work_title
      DRAW_BOX 55, 7, 24, 14, EDITUI_BOX_SINGLE, msg_preview_title
      DRAW_BOX 55, 22, 24, 10, EDITUI_BOX_SINGLE, msg_meta_title
      DRAW_BOX 1, 33, 53, 9, EDITUI_BOX_SINGLE, msg_palette_title
      DRAW_BOX 55, 33, 24, 9, EDITUI_BOX_SINGLE, msg_status_title

      SELECT_BODY 1, 7, 15, 14
      PRINT_BODY EDITUI_COLOR_TITLE, msg_tool_1
      JSR   vtext_newline
      PRINT_BODY EDITUI_COLOR_PANEL, msg_tool_2
      JSR   vtext_newline
      PRINT_BODY EDITUI_COLOR_PANEL, msg_tool_3

      SELECT_BODY 17, 7, 37, 25
      PRINT_BODY EDITUI_COLOR_PANEL, msg_canvas_1
      JSR   vtext_newline
      JSR   vtext_newline
      PRINT_BODY EDITUI_COLOR_DIM, msg_canvas_2

      SELECT_BODY 55, 7, 24, 14
      PRINT_BODY EDITUI_COLOR_PANEL, msg_preview_1

      SELECT_BODY 55, 22, 24, 10
      PRINT_BODY EDITUI_COLOR_PANEL, msg_meta_1

      SELECT_BODY 1, 33, 53, 9
      PRINT_BODY EDITUI_COLOR_TITLE, msg_palette_1

      SELECT_BODY 55, 33, 24, 9
      PRINT_BODY EDITUI_COLOR_PANEL, msg_state_1
      RTS

.segment "RODATA"

msg_title:
      .byte "N O V A   T U I   S H E L L",0
msg_help:
      .byte "File: UNTITLED.TUI   Mode: Text UI   Device: Nova",0
msg_status:
      .byte "&F1: Help   Alt+&F File   Alt+&E Edit   Alt+&H Help   Ctrl+&Q Quit",0

msg_tools_title:
      .byte "TOOLS",0
msg_work_title:
      .byte "WORKSPACE",0
msg_preview_title:
      .byte "PREVIEW",0
msg_meta_title:
      .byte "INSPECTOR",0
msg_palette_title:
      .byte "PALETTE / OPTIONS",0
msg_status_title:
      .byte "STATUS",0

msg_tool_1:
      .byte $10," Primary",0
msg_tool_2:
      .byte $10," Secondary",0
msg_tool_3:
      .byte $10," Transform",0
msg_canvas_1:
      .byte "Document surface",0
msg_canvas_2:
      .byte "Selection: none",0
msg_preview_1:
      .byte "Rendered view",0
msg_meta_1:
      .byte "X: 00  Y: 00",0
msg_palette_1:
      .byte $DB,$DB," ",$DB,$DB," ",$DB,$DB," ",$DB,$DB," ",$DB,$DB," ",$DB,$DB," ",$DB,$DB," ",$DB,$DB,0
msg_state_1:
      .byte "Ready",0

; editbuf_test.s — standalone harness that exercises the shared editbuf engine.
;
; Loaded into RAM by a C# VM test (EditBufVmTests). It seeds a known buffer,
; installs observable test hooks, runs the modal editor, and records the exit
; result so the host test can assert on editor behavior — with no Logo, no
; bank switching, and no document model of its own.

.include "nova.inc"
.include "editui.inc"
.include "editbuf.inc"
.include "docbuf.inc"
.include "nui.inc"

; Fixed RAM the C# test reads/writes (outside ZP/stack/BSS).
TEXT_BUF       = $0400          ; editable text buffer (cap 2048)
UNDO_BUF       = $0C00          ; one-snapshot undo buffer (same cap)
REDO_BUF       = $1400          ; one-snapshot redo buffer (same cap)
TEST_RESULT    = $0300          ; editbuf_run return value
TEST_DONE      = $0301          ; $AA once editbuf_run returns
TEST_SAVECNT   = $0302          ; number of times the SAVE hook fired
TEST_SAVERET   = $0303          ; status the SAVE hook should return (test sets)
TEST_SAVELENL  = $0304          ; buffer length captured by SAVE hook
TEST_SAVELENH  = $0305
TEST_INDENT    = $0306          ; spaces the INDENT hook returns (test sets)
TEST_HLMARK    = $0307          ; sentinel color the HILITE hook stamps (test sets)
TEST_SEEDLENL  = $0308          ; injected-buffer length lo (0 => use built-in seed)
TEST_SEEDLENH  = $0309          ; injected-buffer length hi
TEST_CMDCNT    = $030A          ; number of document-command hook calls
TEST_LASTCMD   = $030B          ; last EDITUI_CMD_* seen by document-command hook
TEST_DOCMODE   = $030C          ; nonzero => install the XRAM docbuf harness
TEST_ACTIVE_DOC= $030D          ; active docbuf slot reported to C# tests
TEST_FULLLENL  = $030E          ; full canonical XRAM document length lo
TEST_FULLLENH  = $030F
TEST_WINDOWLENL= $0310          ; current RAM window length lo
TEST_WINDOWLENH= $0311
TEST_WINDOWOFFL= $0312          ; current RAM window offset lo
TEST_WINDOWOFFH= $0313
TEST_MENUMODE  = $0314          ; nonzero => install a test menu with a disabled row
TEST_HLCNT     = $0315          ; number of highlighted rows (detects repaints)
TEXT_CAP       = 2048
WINDOW_LEN     = 1024
LARGE_B_OFF    = WINDOW_LEN
LARGE_C_OFF    = WINDOW_LEN * 2
LARGE_TAIL_LEN = 900
LARGE_TAIL_REM = LARGE_TAIL_LEN - $0300

.segment "HEADER"
      .byte $00, $50            ; .prg load address = $5000

.segment "CODE"

start:
      SEI
      CLD
      LDX   #$FF
      TXS                       ; standalone entry: set up our own stack

      ; Seed the editable buffer + EDITBUF_LEN. If the host test injected its own
      ; buffer (TEST_SEEDLEN nonzero), use it as-is; else copy the built-in source.
      LDA   TEST_SEEDLENL
      ORA   TEST_SEEDLENH
      BNE   @injected
      LDX   #0
@seed:
      CPX   #init_text_len
      BCS   @builtin_len
      LDA   init_text,X
      STA   TEXT_BUF,X
      INX
      BRA   @seed
@builtin_len:
      LDA   #init_text_len
      STA   EDITBUF_LENL
      STZ   EDITBUF_LENH
      BRA   @seeded
@injected:
      LDA   TEST_SEEDLENL
      STA   EDITBUF_LENL
      LDA   TEST_SEEDLENH
      STA   EDITBUF_LENH
@seeded:

      ; --- editbuf config ---
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
      LDA   #<title_str
      STA   EDITBUF_TITLEL
      LDA   #>title_str
      STA   EDITBUF_TITLEH
      LDA   #<type_str
      STA   EDITBUF_TYPEL
      LDA   #>type_str
      STA   EDITBUF_TYPEH
      STZ   EDITBUF_STATUSL
      STZ   EDITBUF_STATUSH

      ; --- hooks ---
      LDA   #<test_save_hook
      STA   EDITBUF_SAVE_VECL
      LDA   #>test_save_hook
      STA   EDITBUF_SAVE_VECH
      LDA   #<test_indent_hook
      STA   EDITBUF_INDENT_VECL
      LDA   #>test_indent_hook
      STA   EDITBUF_INDENT_VECH
      LDA   #<test_hilite_hook
      STA   EDITBUF_HILITE_VECL
      LDA   #>test_hilite_hook
      STA   EDITBUF_HILITE_VECH
      LDA   #<test_command_hook
      STA   EDITBUF_COMMAND_VECL
      LDA   #>test_command_hook
      STA   EDITBUF_COMMAND_VECH
      LDA   #<test_changed_hook
      STA   EDITBUF_CHANGED_VECL
      LDA   #>test_changed_hook
      STA   EDITBUF_CHANGED_VECH
      LDA   TEST_MENUMODE
      BEQ   :+
      LDA   #<test_menu_hook
      STA   EDITBUF_MENU_VECL
      LDA   #>test_menu_hook
      STA   EDITBUF_MENU_VECH
:

      LDA   TEST_DOCMODE
      BEQ   :+
      JSR   test_docmode_init
:
      JSR   editbuf_reset_state
      JSR   editbuf_run

      STA   TEST_RESULT
      LDA   #$AA
      STA   TEST_DONE
done:
      BRA   done

; SAVE hook: count calls, capture length, return test-controlled status.
test_save_hook:
      INC   TEST_SAVECNT
      LDA   TEST_DOCMODE
      CMP   #2
      BNE   @ram_save
      JSR   test_window_changed_hook
      LDA   TEST_FULLLENL
      STA   TEST_SAVELENL
      LDA   TEST_FULLLENH
      STA   TEST_SAVELENH
      LDA   TEST_SAVERET
      RTS
@ram_save:
      LDA   EDITBUF_LENL
      STA   TEST_SAVELENL
      LDA   EDITBUF_LENH
      STA   TEST_SAVELENH
      LDA   TEST_SAVERET
      RTS

; INDENT hook: return the test-controlled indent width.
test_indent_hook:
      LDA   TEST_INDENT
      RTS

; HILITE hook: stamp the first character of every line with a sentinel color.
test_hilite_hook:
      INC   TEST_HLCNT
      LDA   EDITBUF_HL_LEN
      BEQ   @done
      LDA   TEST_HLMARK
      STA   EDITBUF_HL_COLORS
@done:
      RTS

; COMMAND hook: count file/document-level commands that editbuf delegates.
test_command_hook:
      INC   TEST_CMDCNT
      STA   TEST_LASTCMD
      LDA   TEST_DOCMODE
      BNE   :+
      CLC
      RTS
:     CMP   #2
      BNE   :+
      JMP   test_window_command
:     LDA   TEST_LASTCMD
      CMP   #EDITUI_CMD_BUFFER_NEXT
      BEQ   test_doc_next
      CMP   #EDITUI_CMD_BUFFER_PREVIOUS
      BEQ   test_doc_previous
      CMP   #EDITUI_CMD_BUFFER_LIST
      BEQ   test_doc_list
      CLC
      RTS

test_doc_next:
      JSR   test_editbuf_to_docbuf
      JSR   docbuf_next
      BNE   @done
      JSR   test_docbuf_to_editbuf
@done:
      RTS

test_window_command:
      LDA   TEST_LASTCMD
      CMP   #EDITUI_CMD_OPEN
      BNE   :+
      JMP   test_window_docmode_init
:     CMP   #EDITUI_CMD_WINDOW_NEXT
      BNE   :+
      JMP   test_window_page_next
:     CMP   #EDITUI_CMD_BUFFER_NEXT
      BNE   :+
      JMP   test_window_page_next
:     CMP   #EDITUI_CMD_WINDOW_PREVIOUS
      BNE   :+
      JMP   test_window_page_previous
:     CMP   #EDITUI_CMD_BUFFER_PREVIOUS
      BNE   :+
      JMP   test_window_page_previous
:     CLC
      RTS

test_doc_previous:
      JSR   test_editbuf_to_docbuf
      JSR   docbuf_previous
      BNE   @done
      JSR   test_docbuf_to_editbuf
@done:
      RTS

test_doc_list:
      JSR   test_editbuf_to_docbuf
      JSR   docbuf_store_active
      BNE   @done
      JSR   docbuf_build_list_rows
      BNE   @done
      JSR   nui_dialog_defaults
      LDA   #20
      STA   NUI_DIALOG_LEFT
      LDA   #18
      STA   NUI_DIALOG_TOP
      LDA   #40
      STA   NUI_DIALOG_WIDTH
      LDA   #12
      STA   NUI_DIALOG_HEIGHT
      LDA   #<doc_list_title
      STA   NUI_TITLEL
      LDA   #>doc_list_title
      STA   NUI_TITLEH
      STZ   NUI_FOOTERL
      STZ   NUI_FOOTERH
      LDA   #<DOCBUF_LIST_ROWS
      STA   NUI_LIST_ITEMSL
      LDA   #>DOCBUF_LIST_ROWS
      STA   NUI_LIST_ITEMSH
      LDA   #DOCBUF_ROW_WIDTH
      STA   NUI_LIST_ROW_WIDTH
      LDA   DOCBUF_COUNT
      STA   NUI_LIST_ROW_COUNT
      LDA   DOCBUF_ACTIVE
      STA   NUI_LIST_SELECTED
      JSR   nui_pick_list
      BNE   @done
      LDA   NUI_RESULT
      CMP   #NUI_RESULT_OK
      BNE   @done
      LDA   NUI_LIST_SELECTED
      STA   DOCBUF_SELECTED
      JSR   docbuf_select
      BNE   @done
      JSR   test_docbuf_to_editbuf
@done:
      RTS

; CHANGED hook: when doc mode is active, make XRAM the canonical active copy.
test_changed_hook:
      LDA   TEST_DOCMODE
      BNE   :+
      RTS
:     CMP   #2
      BNE   :+
      JMP   test_window_changed_hook
:     JSR   test_editbuf_to_docbuf
      JSR   docbuf_store_active
      RTS

test_menu_hook:
      LDA   #<test_menus
      LDY   #>test_menus
      JMP   editui_set_menus

test_docmode_init:
      LDA   TEST_DOCMODE
      CMP   #2
      BNE   :+
      JMP   test_window_docmode_init
:
      JSR   docbuf_init
      JSR   test_docbuf_config

      JSR   test_seed_doc_one
      JSR   test_editbuf_to_docbuf
      JSR   docbuf_add_from_ram

      JSR   test_seed_doc_two
      JSR   test_editbuf_to_docbuf
      JSR   docbuf_add_from_ram

      JSR   test_seed_doc_three
      JSR   test_editbuf_to_docbuf
      JSR   docbuf_add_from_ram

      STZ   DOCBUF_ACTIVE
      JSR   docbuf_load_active
      JSR   test_docbuf_to_editbuf
      RTS

test_window_page_next:
      JSR   test_window_changed_hook
      CLC
      LDA   TEST_WINDOWOFFL
      ADC   TEST_WINDOWLENL
      STA   DOCBUF_WINDOW_OFFL
      LDA   TEST_WINDOWOFFH
      ADC   TEST_WINDOWLENH
      STA   DOCBUF_WINDOW_OFFH
      LDA   DOCBUF_WINDOW_OFFL
      CMP   TEST_FULLLENL
      LDA   DOCBUF_WINDOW_OFFH
      SBC   TEST_FULLLENH
      BCS   @done
      LDA   DOCBUF_WINDOW_OFFL
      STA   TEST_WINDOWOFFL
      LDA   DOCBUF_WINDOW_OFFH
      STA   TEST_WINDOWOFFH
      JSR   test_window_load_current
      STZ   EDITBUF_CURL
      STZ   EDITBUF_CURH
      SEC
      RTS
@done:
      CLC
      RTS

test_window_page_previous:
      JSR   test_window_changed_hook
      LDA   TEST_WINDOWOFFL
      ORA   TEST_WINDOWOFFH
      BEQ   @done
      SEC
      LDA   TEST_WINDOWOFFL
      SBC   #<WINDOW_LEN
      STA   TEST_WINDOWOFFL
      LDA   TEST_WINDOWOFFH
      SBC   #>WINDOW_LEN
      STA   TEST_WINDOWOFFH
      BCS   :+
      STZ   TEST_WINDOWOFFL
      STZ   TEST_WINDOWOFFH
:     JSR   test_window_load_current
      LDA   EDITBUF_LENL
      STA   EDITBUF_CURL
      LDA   EDITBUF_LENH
      STA   EDITBUF_CURH
      SEC
      RTS
@done:
      CLC
      RTS

test_window_changed_hook:
      JSR   test_docbuf_config
      LDA   TEST_WINDOWOFFL
      STA   DOCBUF_RANGE_OFFL
      LDA   TEST_WINDOWOFFH
      STA   DOCBUF_RANGE_OFFH
      LDA   TEST_WINDOWLENL
      STA   DOCBUF_DELETE_LENL
      LDA   TEST_WINDOWLENH
      STA   DOCBUF_DELETE_LENH
      LDA   EDITBUF_LENL
      STA   DOCBUF_INSERT_LENL
      LDA   EDITBUF_LENH
      STA   DOCBUF_INSERT_LENH
      CLC
      LDA   TEST_WINDOWOFFL
      ADC   EDITBUF_CURL
      STA   DOCBUF_CURL
      LDA   TEST_WINDOWOFFH
      ADC   EDITBUF_CURH
      STA   DOCBUF_CURH
      LDA   EDITUI_DIRTY
      STA   DOCBUF_DIRTY
      JSR   docbuf_replace_range
      BNE   @done
      LDA   EDITBUF_LENL
      STA   TEST_WINDOWLENL
      LDA   EDITBUF_LENH
      STA   TEST_WINDOWLENH
      LDA   DOCBUF_LENL
      STA   TEST_FULLLENL
      LDA   DOCBUF_LENH
      STA   TEST_FULLLENH
@done:
      RTS

test_window_load_current:
      JSR   test_docbuf_config
      LDA   TEST_WINDOWOFFL
      STA   DOCBUF_WINDOW_OFFL
      LDA   TEST_WINDOWOFFH
      STA   DOCBUF_WINDOW_OFFH
      LDA   #<WINDOW_LEN
      STA   DOCBUF_WINDOW_LENL
      LDA   #>WINDOW_LEN
      STA   DOCBUF_WINDOW_LENH
      JSR   docbuf_load_window
      LDA   DOCBUF_WINDOW_LENL
      STA   EDITBUF_LENL
      STA   TEST_WINDOWLENL
      LDA   DOCBUF_WINDOW_LENH
      STA   EDITBUF_LENH
      STA   TEST_WINDOWLENH
      LDA   DOCBUF_LENL
      STA   TEST_FULLLENL
      LDA   DOCBUF_LENH
      STA   TEST_FULLLENH
      LDA   DOCBUF_TITLEL
      STA   EDITBUF_TITLEL
      LDA   DOCBUF_TITLEH
      STA   EDITBUF_TITLEH
      LDA   DOCBUF_TYPEL
      STA   EDITBUF_TYPEL
      LDA   DOCBUF_TYPEH
      STA   EDITBUF_TYPEH
      LDA   DOCBUF_DIRTY
      STA   EDITUI_DIRTY
      LDA   DOCBUF_ACTIVE
      STA   TEST_ACTIVE_DOC
      RTS

test_window_docmode_init:
      JSR   docbuf_init
      JSR   test_docbuf_config
      JSR   test_fill_large_a
      LDA   #<WINDOW_LEN
      STA   EDITBUF_LENL
      LDA   #>WINDOW_LEN
      STA   EDITBUF_LENH
      STZ   EDITBUF_CURL
      STZ   EDITBUF_CURH
      STZ   EDITUI_DIRTY
      LDA   #<doc_large_title
      STA   EDITBUF_TITLEL
      LDA   #>doc_large_title
      STA   EDITBUF_TITLEH
      LDA   #<doc_pascal_type
      STA   EDITBUF_TYPEL
      LDA   #>doc_pascal_type
      STA   EDITBUF_TYPEH
      JSR   test_editbuf_to_docbuf
      JSR   docbuf_add_from_ram

      JSR   test_fill_large_b
      LDA   #<WINDOW_LEN
      STA   DOCBUF_RANGE_OFFL
      LDA   #>WINDOW_LEN
      STA   DOCBUF_RANGE_OFFH
      STZ   DOCBUF_DELETE_LENL
      STZ   DOCBUF_DELETE_LENH
      LDA   #<WINDOW_LEN
      STA   DOCBUF_INSERT_LENL
      LDA   #>WINDOW_LEN
      STA   DOCBUF_INSERT_LENH
      STZ   DOCBUF_CURL
      STZ   DOCBUF_CURH
      STZ   DOCBUF_DIRTY
      JSR   docbuf_replace_range

      JSR   test_fill_large_c
      LDA   #<LARGE_C_OFF
      STA   DOCBUF_RANGE_OFFL
      LDA   #>LARGE_C_OFF
      STA   DOCBUF_RANGE_OFFH
      STZ   DOCBUF_DELETE_LENL
      STZ   DOCBUF_DELETE_LENH
      LDA   #<LARGE_TAIL_LEN
      STA   DOCBUF_INSERT_LENL
      LDA   #>LARGE_TAIL_LEN
      STA   DOCBUF_INSERT_LENH
      STZ   DOCBUF_CURL
      STZ   DOCBUF_CURH
      STZ   DOCBUF_DIRTY
      JSR   docbuf_replace_range

      STZ   TEST_WINDOWOFFL
      STZ   TEST_WINDOWOFFH
      JSR   test_window_load_current
      STZ   EDITBUF_CURL
      STZ   EDITBUF_CURH
      STZ   EDITUI_DIRTY
      LDA   DOCBUF_LENL
      STA   TEST_FULLLENL
      LDA   DOCBUF_LENH
      STA   TEST_FULLLENH
      LDA   DOCBUF_ACTIVE
      STA   TEST_ACTIVE_DOC
      RTS

test_docbuf_config:
      LDA   #<TEXT_BUF
      STA   DOCBUF_RAML
      LDA   #>TEXT_BUF
      STA   DOCBUF_RAMH
      LDA   #<TEXT_CAP
      STA   DOCBUF_CAPL
      LDA   #>TEXT_CAP
      STA   DOCBUF_CAPH
      RTS

test_editbuf_to_docbuf:
      JSR   test_docbuf_config
      LDA   EDITBUF_LENL
      STA   DOCBUF_LENL
      LDA   EDITBUF_LENH
      STA   DOCBUF_LENH
      LDA   EDITBUF_CURL
      STA   DOCBUF_CURL
      LDA   EDITBUF_CURH
      STA   DOCBUF_CURH
      LDA   EDITUI_DIRTY
      STA   DOCBUF_DIRTY
      LDA   EDITBUF_TITLEL
      STA   DOCBUF_TITLEL
      LDA   EDITBUF_TITLEH
      STA   DOCBUF_TITLEH
      LDA   EDITBUF_TYPEL
      STA   DOCBUF_TYPEL
      LDA   EDITBUF_TYPEH
      STA   DOCBUF_TYPEH
      RTS

test_docbuf_to_editbuf:
      LDA   DOCBUF_LENL
      STA   EDITBUF_LENL
      LDA   DOCBUF_LENH
      STA   EDITBUF_LENH
      LDA   DOCBUF_CURL
      STA   EDITBUF_CURL
      LDA   DOCBUF_CURH
      STA   EDITBUF_CURH
      LDA   DOCBUF_DIRTY
      STA   EDITUI_DIRTY
      LDA   DOCBUF_TITLEL
      STA   EDITBUF_TITLEL
      LDA   DOCBUF_TITLEH
      STA   EDITBUF_TITLEH
      LDA   DOCBUF_TYPEL
      STA   EDITBUF_TYPEL
      LDA   DOCBUF_TYPEH
      STA   EDITBUF_TYPEH
      LDA   DOCBUF_ACTIVE
      STA   TEST_ACTIVE_DOC
      RTS

test_fill_large_a:
      LDX   #0
@loop:
      LDA   #'A'
      STA   TEXT_BUF,X
      STA   TEXT_BUF+$0100,X
      STA   TEXT_BUF+$0200,X
      STA   TEXT_BUF+$0300,X
      INX
      BNE   @loop
      RTS

test_fill_large_b:
      LDX   #0
@loop:
      LDA   #'B'
      STA   TEXT_BUF,X
      STA   TEXT_BUF+$0100,X
      STA   TEXT_BUF+$0200,X
      STA   TEXT_BUF+$0300,X
      INX
      BNE   @loop
      RTS

test_fill_large_c:
      LDX   #0
@full_pages:
      LDA   #'C'
      STA   TEXT_BUF,X
      STA   TEXT_BUF+$0100,X
      STA   TEXT_BUF+$0200,X
      INX
      BNE   @full_pages
      LDX   #0
@tail:
      CPX   #LARGE_TAIL_REM
      BCS   @done
      LDA   #'C'
      STA   TEXT_BUF+$0300,X
      INX
      BRA   @tail
@done:
      RTS

test_seed_doc_one:
      LDX   #0
@copy:
      CPX   #doc_one_text_len
      BCS   @done
      LDA   doc_one_text,X
      STA   TEXT_BUF,X
      INX
      BRA   @copy
@done:
      LDA   #doc_one_text_len
      STA   EDITBUF_LENL
      STZ   EDITBUF_LENH
      STZ   EDITBUF_CURL
      STZ   EDITBUF_CURH
      STZ   EDITUI_DIRTY
      LDA   #<doc_one_title
      STA   EDITBUF_TITLEL
      LDA   #>doc_one_title
      STA   EDITBUF_TITLEH
      LDA   #<doc_pascal_type
      STA   EDITBUF_TYPEL
      LDA   #>doc_pascal_type
      STA   EDITBUF_TYPEH
      RTS

test_seed_doc_two:
      LDX   #0
@copy:
      CPX   #doc_two_text_len
      BCS   @done
      LDA   doc_two_text,X
      STA   TEXT_BUF,X
      INX
      BRA   @copy
@done:
      LDA   #doc_two_text_len
      STA   EDITBUF_LENL
      STZ   EDITBUF_LENH
      STZ   EDITBUF_CURL
      STZ   EDITBUF_CURH
      STZ   EDITUI_DIRTY
      LDA   #<doc_two_title
      STA   EDITBUF_TITLEL
      LDA   #>doc_two_title
      STA   EDITBUF_TITLEH
      LDA   #<doc_logo_type
      STA   EDITBUF_TYPEL
      LDA   #>doc_logo_type
      STA   EDITBUF_TYPEH
      RTS

test_seed_doc_three:
      LDX   #0
@copy:
      CPX   #doc_three_text_len
      BCS   @done
      LDA   doc_three_text,X
      STA   TEXT_BUF,X
      INX
      BRA   @copy
@done:
      LDA   #doc_three_text_len
      STA   EDITBUF_LENL
      STZ   EDITBUF_LENH
      STZ   EDITBUF_CURL
      STZ   EDITBUF_CURH
      STZ   EDITUI_DIRTY
      LDA   #<doc_three_title
      STA   EDITBUF_TITLEL
      LDA   #>doc_three_title
      STA   EDITBUF_TITLEH
      LDA   #<doc_forth_type
      STA   EDITBUF_TYPEL
      LDA   #>doc_forth_type
      STA   EDITBUF_TYPEH
      RTS

.segment "RODATA"
title_str:
      .byte "forth/test/SQUARE.4th", 0
type_str:
      .byte "Test", 0
doc_list_title:
      .byte "Open Buffers", 0
doc_one_title:
      .byte "pascal/test/ONE.PAS", 0
doc_two_title:
      .byte "logo/test/TWO.LOGO", 0
doc_three_title:
      .byte "forth/test/THREE.4th", 0
doc_large_title:
      .byte "pascal/test/LARGE.PAS", 0
doc_pascal_type:
      .byte "Pascal", 0
doc_logo_type:
      .byte "Logo", 0
doc_forth_type:
      .byte "Forth", 0
test_menus:
      .byte 1
      .byte 't', 18, <test_menu_title, >test_menu_title, <test_menu_items, >test_menu_items
test_menu_items:
      .byte 3
      .byte EDITUI_CMD_SAVE_AS, 'f', <test_item_first, >test_item_first
      .byte EDITUI_CMD_NONE,    'd', <test_item_disabled, >test_item_disabled
      .byte EDITUI_CMD_OPEN,    's', <test_item_second, >test_item_second
test_menu_title:
      .byte "&Test", 0
test_item_first:
      .byte "&First", 0
test_item_disabled:
      .byte "&Disabled", 0
test_item_second:
      .byte "&Second", 0
doc_one_text:
      .byte "ONE", $0A
doc_one_text_end:
doc_one_text_len = doc_one_text_end - doc_one_text
doc_two_text:
      .byte "TWO", $0A
doc_two_text_end:
doc_two_text_len = doc_two_text_end - doc_two_text
doc_three_text:
      .byte "THREE", $0A
doc_three_text_end:
doc_three_text_len = doc_three_text_end - doc_three_text
init_text:
      .byte "AB", $0A, "CD"
init_text_end:
init_text_len = init_text_end - init_text

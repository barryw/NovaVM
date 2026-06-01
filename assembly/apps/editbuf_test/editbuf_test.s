; editbuf_test.s — standalone harness that exercises the shared editbuf engine.
;
; Loaded into RAM by a C# VM test (EditBufVmTests). It seeds a known buffer,
; installs observable test hooks, runs the modal editor, and records the exit
; result so the host test can assert on editor behavior — with no Logo, no
; bank switching, and no document model of its own.

.include "nova.inc"
.include "editui.inc"
.include "editbuf.inc"

; Fixed RAM the C# test reads/writes (outside ZP/stack/BSS).
TEXT_BUF       = $0400          ; editable text buffer (cap 2048)
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
TEXT_CAP       = 2048

.segment "HEADER"
      .byte $00, $72            ; .prg load address = $7200

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
      LDA   #<title_str
      STA   EDITBUF_TITLEL
      LDA   #>title_str
      STA   EDITBUF_TITLEH
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
      LDA   EDITBUF_HL_LEN
      BEQ   @done
      LDA   TEST_HLMARK
      STA   EDITBUF_HL_COLORS
@done:
      RTS

.segment "RODATA"
title_str:
      .byte "SQUARE", 0
init_text:
      .byte "AB", $0A, "CD"
init_text_end:
init_text_len = init_text_end - init_text

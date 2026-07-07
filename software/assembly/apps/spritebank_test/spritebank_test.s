; spritebank_test.s -- headless harness for spritebank_open.
;
; The C# test pokes an NSPR buffer at $6000, loads this at $7200, runs it, then
; reads the parse results the harness stashes at $7F00..$7F06.
;
;   Load address: $7200   Linker: ld65 -C spritebank_test.cfg <o> nova.lib

.include "nova.inc"
.include "spritebank.inc"

RESULTS = $7F00

      .segment "HEADER"
      .byte $00, $72

      .segment "CODE"
start:
      SEI
      CLD
      LDX   #$FF
      TXS

      LDA   #$00                 ; spritebank_src = $6000 (test poked the buffer here)
      STA   spritebank_src
      LDA   #$60
      STA   spritebank_src+1

      JSR   spritebank_open

      LDA   spritebank_result
      STA   RESULTS + 0
      LDA   spritebank_shape_count
      STA   RESULTS + 1
      LDA   spritebank_char_count
      STA   RESULTS + 2
      LDA   spritebank_shapes_ptr
      STA   RESULTS + 3
      LDA   spritebank_shapes_ptr+1
      STA   RESULTS + 4
      LDA   spritebank_chars_ptr
      STA   RESULTS + 5
      LDA   spritebank_chars_ptr+1
      STA   RESULTS + 6

halt:
      BRA   halt

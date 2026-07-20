; Resident NovaPascal compiler: XRAM Pascal source -> textual 65C02 assembly.

      .setcpu "w65c02"
      .include "nptool.inc"
      .include "xram.inc"
      .include "docbuf.inc"

SOURCE_CAP = DOCBUF_SLOT_BYTES
ASM_CAP    = 3072

PASCAL_OK         = 0
PASCAL_ERR_SYNTAX = 1
PASCAL_ERR_OUTPUT = 2

      .segment "ZEROPAGE"
p_src:       .res 3
p_left:      .res 2
p_word:      .res 2
p_out:       .res 2
p_out_left:  .res 2
p_expected:  .res 1
p_char:      .res 1

      .segment "BSS"
source_len:        .res 2
generated_asm:     .res ASM_CAP
generated_asm_len: .res 2
pascal_error:      .res 1
p_line:            .res 2
p_decimal:         .res 2
p_remainder:       .res 2
p_digit:           .res 1
p_started:         .res 1

      .segment "CODE"
      .export npc_compile_file
      .export generated_asm
      .export generated_asm_len
      .export pascal_error

npc_compile_file:
      STZ   NPTOOL_STATUS
      STZ   NPTOOL_DETAIL
      LDA   #<npc_banner
      LDX   #>npc_banner
      JSR   nptool_print_z
      LDA   NPTOOL_ARG0_LEN
      BNE   :+
      JMP   tool_bad_args
:
      LDA   NPTOOL_ARG1_LEN
      BNE   :+
      JMP   tool_bad_args
:
      LDA   #<npc_compiling
      LDX   #>npc_compiling
      JSR   nptool_print_z
      LDA   #<NPTOOL_ARG0
      LDX   #>NPTOOL_ARG0
      JSR   nptool_print_z
      JSR   nptool_newline
      LDA   #DOCBUF_XRAM_BASE_L
      STA   XRAM_ADDRL
      LDA   #DOCBUF_XRAM_BASE_M
      STA   XRAM_ADDRM
      LDA   #DOCBUF_XRAM_BASE_H
      STA   XRAM_ADDRH
      LDA   #<SOURCE_CAP
      STA   XRAM_LENL
      LDA   #>SOURCE_CAP
      STA   XRAM_LENH
      LDA   NPTOOL_ARG0_LEN
      STA   XRAM_NAMELEN
      LDA   #<NPTOOL_ARG0
      STA   XRAM_NAMEPTR_L
      LDA   #>NPTOOL_ARG0
      STA   XRAM_NAMEPTR_H
      JSR   xram_xload
      BNE   @io_error
      LDA   XRAM_LENL
      STA   source_len+0
      LDA   XRAM_LENH
      STA   source_len+1
      JSR   pascal_compile
      BNE   @compile_error
      LDA   #<generated_asm
      STA   NPTOOL_IO_ADDR+0
      LDA   #>generated_asm
      STA   NPTOOL_IO_ADDR+1
      LDA   generated_asm_len+0
      STA   NPTOOL_IO_LEN+0
      LDA   generated_asm_len+1
      STA   NPTOOL_IO_LEN+1
      LDA   #<npc_writing
      LDX   #>npc_writing
      JSR   nptool_print_z
      LDA   #<NPTOOL_ARG1
      LDX   #>NPTOOL_ARG1
      JSR   nptool_print_z
      JSR   nptool_newline
      JSR   nptool_save_arg1
      BNE   @fail
      LDA   #<npc_ok
      LDX   #>npc_ok
      JSR   nptool_print_z
      LDA   #0
      RTS
@io_error:
      LDA   FIO_ERRCODE
      STA   NPTOOL_DETAIL
      LDA   #NPTOOL_ERR_IO
      STA   NPTOOL_STATUS
      BRA   @fail
@compile_error:
      LDA   pascal_error
      STA   NPTOOL_DETAIL
      LDA   #NPTOOL_ERR_COMPILE
      STA   NPTOOL_STATUS
@fail:
      LDA   #1
      RTS

tool_bad_args:
      LDA   #NPTOOL_ERR_ARGS
      STA   NPTOOL_STATUS
      LDA   #1
      RTS

      .segment "RODATA"
npc_banner:    .byte "Nova Pascal Compiler v1.0", $0D, $0A, 0
npc_compiling: .byte "Compiling ", 0
npc_writing:   .byte "Writing ", 0
npc_ok:        .byte "Compile successful", $0D, $0A, 0

      .segment "CODE"
; Accepted grammar:
;   program <identifier>; begin writeln('<ASCII>'); end.
pascal_compile:
      STZ   pascal_error
      STZ   generated_asm_len
      STZ   generated_asm_len+1
      LDA   #1
      STA   p_line
      STZ   p_line+1
      LDA   #DOCBUF_XRAM_BASE_L
      STA   p_src
      LDA   #DOCBUF_XRAM_BASE_M
      STA   p_src+1
      LDA   #DOCBUF_XRAM_BASE_H
      STA   p_src+2
      LDA   source_len
      STA   p_left
      LDA   source_len+1
      STA   p_left+1
      LDA   #<generated_asm
      STA   p_out
      LDA   #>generated_asm
      STA   p_out+1
      LDA   #<ASM_CAP
      STA   p_out_left
      LDA   #>ASM_CAP
      STA   p_out_left+1
      LDA   #<asm_import
      STA   p_word
      LDA   #>asm_import
      STA   p_word+1
      JSR   p_emit_text
      BCC   :+
      JMP   p_output_error
:

      LDA   #<kw_program
      STA   p_word
      LDA   #>kw_program
      STA   p_word+1
      JSR   p_expect_word
      BCC   :+
      JMP   p_syntax_error
:
      JSR   p_identifier
      BCC   :+
      JMP   p_syntax_error
:
      LDA   #';'
      JSR   p_expect_char
      BCC   :+
      JMP   p_syntax_error
:
      LDA   #<kw_begin
      STA   p_word
      LDA   #>kw_begin
      STA   p_word+1
      JSR   p_expect_word
      BCS   p_syntax_error
      LDA   #<kw_writeln
      STA   p_word
      LDA   #>kw_writeln
      STA   p_word+1
      JSR   p_expect_word
      BCS   p_syntax_error
      JSR   p_emit_source_comment
      BCS   p_output_error
      LDA   #'('
      JSR   p_expect_char
      BCS   p_syntax_error
      LDA   #$27
      JSR   p_expect_char
      BCS   p_syntax_error

@string:
      JSR   p_next
      BCC   p_syntax_error
      CMP   #$27
      BEQ   @string_done
      CMP   #$20
      BCC   p_syntax_error
      CMP   #$7F
      BCS   p_syntax_error
      JSR   p_emit_print_char
      BCS   p_output_error
      BRA   @string
@string_done:
      LDA   #')'
      JSR   p_expect_char
      BCS   p_syntax_error
      LDA   #';'
      JSR   p_expect_char
      BCS   p_syntax_error
      LDA   #<kw_end
      STA   p_word
      LDA   #>kw_end
      STA   p_word+1
      JSR   p_expect_word
      BCS   p_syntax_error
      LDA   #'.'
      JSR   p_expect_char
      BCS   p_syntax_error
      JSR   p_skip_ws
      LDA   p_left
      ORA   p_left+1
      BNE   p_syntax_error

      LDA   #$0A
      JSR   p_emit_print_char
      BCS   p_output_error
      LDA   #<asm_rts
      STA   p_word
      LDA   #>asm_rts
      STA   p_word+1
      JSR   p_emit_text
      BCS   p_output_error
      LDA   #PASCAL_OK
      RTS

p_syntax_error:
      LDA   #PASCAL_ERR_SYNTAX
      BRA   p_compile_fail
p_output_error:
      LDA   #PASCAL_ERR_OUTPUT
p_compile_fail:
      STA   pascal_error
      LDA   #1
      RTS

p_emit_print_char:
      STA   p_char
      LDA   #<asm_lda
      STA   p_word
      LDA   #>asm_lda
      STA   p_word+1
      JSR   p_emit_text
      BCS   @fail
      LDA   p_char
      LSR
      LSR
      LSR
      LSR
      TAX
      LDA   hex_digits,X
      JSR   p_emit
      BCS   @fail
      LDA   p_char
      AND   #$0F
      TAX
      LDA   hex_digits,X
      JSR   p_emit
      BCS   @fail
      LDA   #$0A
      JSR   p_emit
      BCS   @fail
      LDA   #<asm_jsr
      STA   p_word
      LDA   #>asm_jsr
      STA   p_word+1
      JSR   p_emit_text
@fail:
      RTS

p_emit_source_comment:
      LDA   #<asm_comment
      STA   p_word
      LDA   #>asm_comment
      STA   p_word+1
      JSR   p_emit_text
      BCS   @fail
      LDA   #<NPTOOL_ARG0
      STA   p_word
      LDA   #>NPTOOL_ARG0
      STA   p_word+1
      JSR   p_emit_text
      BCS   @fail
      LDA   #':'
      JSR   p_emit
      BCS   @fail
      JSR   p_emit_line_number
      BCS   @fail
      LDA   #<asm_writeln_comment
      STA   p_word
      LDA   #>asm_writeln_comment
      STA   p_word+1
      JMP   p_emit_text
@fail:
      SEC
      RTS

; Emit the current 16-bit Pascal line number as ordinary decimal text.
p_emit_line_number:
      LDA   p_line
      STA   p_decimal
      LDA   p_line+1
      STA   p_decimal+1
      STZ   p_started
      LDX   #0
@place:
      STZ   p_digit
@subtract:
      SEC
      LDA   p_decimal
      SBC   decimal_place_lo,X
      STA   p_remainder
      LDA   p_decimal+1
      SBC   decimal_place_hi,X
      BCC   @emit
      STA   p_remainder+1
      LDA   p_remainder
      STA   p_decimal
      LDA   p_remainder+1
      STA   p_decimal+1
      INC   p_digit
      BRA   @subtract
@emit:
      LDA   p_digit
      BNE   @digit
      LDA   p_started
      BNE   @digit
      CPX   #4
      BNE   @next
@digit:
      LDA   #1
      STA   p_started
      LDA   p_digit
      CLC
      ADC   #'0'
      JSR   p_emit
      BCS   @fail
@next:
      INX
      CPX   #5
      BCC   @place
      CLC
@fail:
      RTS

p_emit_text:
      LDY   #0
@loop:
      LDA   (p_word),Y
      BEQ   @done
      JSR   p_emit
      BCS   @done
      INC   p_word
      BNE   @loop
      INC   p_word+1
      BRA   @loop
@done:
      RTS

p_emit:
      PHA
      LDA   p_out_left
      ORA   p_out_left+1
      BEQ   @full
      PLA
      LDY   #0
      STA   (p_out),Y
      INC   p_out
      BNE   :+
      INC   p_out+1
:     LDA   p_out_left
      BNE   :+
      DEC   p_out_left+1
:     DEC   p_out_left
      INC   generated_asm_len
      BNE   :+
      INC   generated_asm_len+1
:     CLC
      RTS
@full:
      PLA
      SEC
      RTS

p_expect_word:
      JSR   p_skip_ws
      LDY   #0
@loop:
      LDA   (p_word),Y
      BEQ   @boundary
      STA   p_expected
      JSR   p_read_upper
      BCC   @bad
      CMP   p_expected
      BNE   @bad
      INC   p_word
      BNE   @loop
      INC   p_word+1
      BRA   @loop
@boundary:
      JSR   p_peek_upper
      BCC   @ok
      JSR   p_is_ident
      BCS   @bad
@ok:
      CLC
      RTS
@bad:
      SEC
      RTS

p_identifier:
      JSR   p_skip_ws
      JSR   p_peek_upper
      BCC   @bad
      JSR   p_is_ident_start
      BCC   @bad
@loop:
      JSR   p_next
      JSR   p_peek_upper
      BCC   @ok
      JSR   p_is_ident
      BCS   @loop
@ok:
      CLC
      RTS
@bad:
      SEC
      RTS

p_expect_char:
      STA   p_expected
      JSR   p_skip_ws
      JSR   p_next
      BCC   @bad
      CMP   p_expected
      BNE   @bad
      CLC
      RTS
@bad:
      SEC
      RTS

p_skip_ws:
@loop:
      JSR   p_peek
      BCC   @done
      CMP   #' '
      BEQ   @eat
      CMP   #$09
      BEQ   @eat
      CMP   #$0D
      BEQ   @eat
      CMP   #$0A
      BNE   @done
@eat:
      JSR   p_next
      BRA   @loop
@done:
      RTS

p_peek_upper:
      JSR   p_peek
      BCC   @eof
      CMP   #'a'
      BCC   @ok
      CMP   #'z'+1
      BCS   @ok
      AND   #$DF
@ok:
      SEC
      RTS
@eof:
      CLC
      RTS

p_read_upper:
      JSR   p_next
      BCC   @eof
      CMP   #'a'
      BCC   @ok
      CMP   #'z'+1
      BCS   @ok
      AND   #$DF
@ok:
      SEC
      RTS
@eof:
      CLC
      RTS

p_is_ident_start:
      CMP   #'A'
      BCC   @underscore
      CMP   #'Z'+1
      BCC   @yes
@underscore:
      CMP   #'_'
      BEQ   @yes
      CLC
      RTS
@yes:
      SEC
      RTS

p_is_ident:
      JSR   p_is_ident_start
      BCS   @yes
      CMP   #'0'
      BCC   @no
      CMP   #'9'+1
      BCC   @yes
@no:
      CLC
      RTS
@yes:
      SEC
      RTS

p_peek:
      LDA   p_left
      ORA   p_left+1
      BEQ   @eof
      LDA   p_src
      STA   XRAM_ADDRL
      LDA   p_src+1
      STA   XRAM_ADDRM
      LDA   p_src+2
      STA   XRAM_ADDRH
      JSR   xram_read8
      BNE   @eof
      LDA   XRAM_DATA
      SEC
      RTS
@eof:
      CLC
      RTS

p_next:
      JSR   p_peek
      BCC   @eof
      PHA
      INC   p_src
      BNE   :+
      INC   p_src+1
      BNE   :+
      INC   p_src+2
:     LDA   p_left
      BNE   :+
      DEC   p_left+1
:     DEC   p_left
      PLA
      CMP   #$0A
      BNE   @read
      INC   p_line
      BNE   @read
      INC   p_line+1
@read:
      SEC
@eof:
      RTS

      .segment "RODATA"
kw_program: .byte "PROGRAM", 0
kw_begin:   .byte "BEGIN", 0
kw_writeln: .byte "WRITELN", 0
kw_end:     .byte "END", 0
hex_digits: .byte "0123456789ABCDEF"
decimal_place_lo: .byte <10000, <1000, <100, <10, <1
decimal_place_hi: .byte >10000, >1000, >100, >10, >1
asm_comment: .byte "; ", 0
asm_import:  .byte ".IMPORT P_WRITE_CHAR", $0A, 0
asm_writeln_comment: .byte " WRITELN", $0A, 0
asm_lda:    .byte "LDA #$", 0
asm_jsr:    .byte "JSR P_WRITE_CHAR", $0A, 0
asm_rts:    .byte "RTS", $0A, 0

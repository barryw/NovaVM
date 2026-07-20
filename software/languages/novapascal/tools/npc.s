; Resident NovaPascal compiler: XRAM Pascal source -> textual 65C02 assembly.

      .setcpu "w65c02"
      .include "nptool.inc"
      .include "xram.inc"
      .include "docbuf.inc"

SOURCE_CAP = DOCBUF_SLOT_BYTES
ASM_BASE   = $2000
ASM_CAP    = $4000
SYMBOL_BASE = $6000
SYMBOL_CAP  = 64
SYMBOL_NAME_CAP = 32
SYMBOL_SIZE = SYMBOL_NAME_CAP + 2
P_IDENT_CAP = 32
P_UNIT_CAP = 4
P_UNIT_NAME_CAP = 12
P_UNIT_STORAGE = P_UNIT_CAP * P_UNIT_NAME_CAP
; ponytail: the first NDK ABI is one-byte values in A plus named byte storage;
; add wider values only when a Pascal program needs them.

TYPE_NONE    = 0
TYPE_BYTE    = 1
TYPE_BOOLEAN = 2

PASCAL_OK         = 0
PASCAL_ERR_SYNTAX = 1
PASCAL_ERR_OUTPUT = 2

.macro long_bcs target
      BCC   :+
      JMP   target
:
.endmacro

.macro long_bcc target
      BCS   :+
      JMP   target
:
.endmacro

.macro long_bne target
      BEQ   :+
      JMP   target
:
.endmacro

.macro long_beq target
      BNE   :+
      JMP   target
:
.endmacro

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
generated_asm_len: .res 2
pascal_error:      .res 1
p_line:            .res 2
p_column:          .res 2
p_decimal:         .res 2
p_remainder:       .res 2
p_digit:           .res 1
p_started:         .res 1
p_string_len:      .res 1
p_ident:           .res P_IDENT_CAP
p_ident_len:       .res 1
p_saved_ident_len: .res 1
p_call_ident_len:  .res 1
p_ident_hash:      .res 3
p_saved_hash:      .res 3
p_call_hash:       .res 3
p_in_argument:     .res 1
p_unit_count:      .res 1
p_unit_len:        .res P_UNIT_CAP
p_unit_iter:       .res 1
p_emit_error:      .res 1
p_saved_ident:     .res P_IDENT_CAP
p_call_ident:      .res P_IDENT_CAP
p_unit_names:      .res P_UNIT_STORAGE
p_symbol_count:    .res 1
p_symbol_group:    .res 1
p_symbol_iter:     .res 1
p_expr_type:       .res 1
p_operator:        .res 1
p_operator_depth:  .res 1
p_operator_stack:  .res 8
p_label:           .res 2
p_label_saved:     .res 2
p_mark_src:        .res 3
p_mark_left:       .res 2
p_mark_line:       .res 2
p_mark_column:     .res 2
p_temp_emitted:    .res 1
p_control_depth:   .res 1
p_control_a_lo:    .res 8
p_control_a_hi:    .res 8
p_control_b_lo:    .res 8
p_control_b_hi:    .res 8

; The compiler and disk-loaded tools execute sequentially. Reusing their lower
; RAM slot gives NPC a useful output/symbol workspace without consuming the
; resident shell's scarce BSS or changing NPC's $C000 entry point.
generated_asm = ASM_BASE

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
; NovaPascal's frontend is a conventional recursive-descent compiler. It emits
; typed accumulator code while parsing, so the native compiler needs no AST.
; This first language slice deliberately implements the grammar exercised by
; FizzBuzz: Byte storage, nested compound statements, expressions, IF/ELSE,
; WHILE, MOD, comparisons, and string/numeric WRITELN.
pascal_compile:
      STZ   pascal_error
      STZ   generated_asm_len
      STZ   generated_asm_len+1
      STZ   p_unit_count
      STZ   p_in_argument
      STZ   p_symbol_count
      STZ   p_label
      STZ   p_label+1
      STZ   p_temp_emitted
      STZ   p_control_depth
      STZ   p_operator_depth
      LDA   #1
      STA   p_line
      STZ   p_line+1
      STA   p_column
      STZ   p_column+1
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
      JSR   p_capture_identifier
      long_bcs p_syntax_error
      LDA   #<kw_uses
      LDX   #>kw_uses
      JSR   p_ident_is
      BCC   @expect_begin
@unit:
      JSR   p_capture_identifier
      long_bcs p_syntax_error
      JSR   p_store_unit
      BCC   @unit_delimiter
      LDA   p_emit_error
      long_bne p_output_error
      JMP   p_syntax_error
@unit_delimiter:
      JSR   p_skip_ws
      JSR   p_peek
      long_bcc p_syntax_error
      CMP   #','
      BNE   @units_done
      JSR   p_next
      BRA   @unit
@units_done:
      LDA   #';'
      JSR   p_expect_char
      long_bcs p_syntax_error
      JSR   p_capture_identifier
      long_bcs p_syntax_error
@expect_begin:
      LDA   #<kw_begin
      LDX   #>kw_begin
      JSR   p_ident_is
      long_bcs @body
      LDA   #<kw_var
      LDX   #>kw_var
      JSR   p_ident_is
      long_bcc p_syntax_error
      LDA   #<asm_bss
      STA   p_word
      LDA   #>asm_bss
      STA   p_word+1
      JSR   p_emit_text
      long_bcs p_output_error
      JSR   p_capture_identifier
      long_bcs p_syntax_error

@var_group:
      LDA   p_symbol_count
      STA   p_symbol_group
@var_name:
      JSR   p_symbol_add
      long_bcs p_syntax_error
      JSR   p_skip_ws
      JSR   p_peek
      long_bcc p_syntax_error
      CMP   #','
      BNE   @var_type
      JSR   p_next
      JSR   p_capture_identifier
      long_bcs p_syntax_error
      BRA   @var_name
@var_type:
      LDA   #':'
      JSR   p_expect_char
      long_bcs p_syntax_error
      LDA   #<kw_byte
      STA   p_word
      LDA   #>kw_byte
      STA   p_word+1
      JSR   p_expect_word
      long_bcs p_syntax_error
      JSR   p_emit_symbol_group
      long_bcs p_output_error
      LDA   #';'
      JSR   p_expect_char
      long_bcs p_syntax_error
      JSR   p_capture_identifier
      long_bcs p_syntax_error
      LDA   #<kw_begin
      LDX   #>kw_begin
      JSR   p_ident_is
      BCS   @vars_done
      BRA   @var_group
@vars_done:
@body:
      LDA   #<asm_code
      STA   p_word
      LDA   #>asm_code
      STA   p_word+1
      JSR   p_emit_text
      long_bcs p_output_error
      JSR   p_parse_statement_list
      BCS   @parse_fail
      LDA   #'.'
      JSR   p_expect_char
      long_bcs p_syntax_error
      JSR   p_skip_ws
      LDA   p_left
      ORA   p_left+1
      long_bne p_syntax_error

      LDA   #<asm_rts
      STA   p_word
      LDA   #>asm_rts
      STA   p_word+1
      JSR   p_emit_text
      long_bcs p_output_error
      JSR   p_emit_unit_sources
      long_bcs p_output_error
      LDA   #PASCAL_OK
      RTS

@parse_fail:
      LDA   p_emit_error
      long_bne p_output_error
      JMP   p_syntax_error

p_syntax_error:
      LDA   #PASCAL_ERR_SYNTAX
      BRA   p_compile_fail
p_output_error:
      LDA   #PASCAL_ERR_OUTPUT
p_compile_fail:
      STA   pascal_error
      LDA   p_line
      STA   NPTOOL_DIAG_LINE
      LDA   p_line+1
      STA   NPTOOL_DIAG_LINE+1
      LDA   p_column
      STA   NPTOOL_DIAG_COL
      LDA   p_column+1
      STA   NPTOOL_DIAG_COL+1
      LDA   #1
      RTS

; Parse statements through END. The caller has already consumed BEGIN.
p_parse_statement_list:
@next:
      JSR   p_skip_ws
      JSR   p_peek
      BCC   @fail
      CMP   #';'
      BNE   :+
      JSR   p_next
      BRA   @next
:
      LDA   #<kw_end
      LDX   #>kw_end
      JSR   p_peek_keyword
      BCC   @statement
      LDA   #<kw_end
      STA   p_word
      LDA   #>kw_end
      STA   p_word+1
      JSR   p_expect_word
      RTS
@statement:
      JSR   p_parse_statement
      BCS   @fail
      JSR   p_skip_ws
      JSR   p_peek
      BCC   @fail
      CMP   #';'
      BEQ   @separator
      LDA   #<kw_end
      LDX   #>kw_end
      JSR   p_peek_keyword
      BCS   @next
      BRA   @fail
@separator:
      JSR   p_next
      BRA   @next
@fail:
      SEC
      RTS

p_parse_statement:
      JSR   p_capture_identifier
      long_bcs @fail
      JSR   p_emit_source_comment
      long_bcs @output
      LDA   #<kw_begin
      LDX   #>kw_begin
      JSR   p_ident_is
      long_bcs p_parse_statement_list
      LDA   #<kw_if
      LDX   #>kw_if
      JSR   p_ident_is
      long_bcs p_parse_if
      LDA   #<kw_while
      LDX   #>kw_while
      JSR   p_ident_is
      long_bcs p_parse_while
      LDA   #<kw_writeln
      LDX   #>kw_writeln
      JSR   p_ident_is
      long_bcs p_parse_writeln

      JSR   p_save_identifier
      JSR   p_skip_ws
      JSR   p_peek
      BCC   @fail
      CMP   #'('
      BEQ   @call
      CMP   #':'
      BNE   @bare_call
      JSR   p_next
      LDA   #'='
      JSR   p_expect_char
      BCS   @fail
      JSR   p_parse_expression
      BCS   @fail
      JSR   p_emit_store_saved
      BCS   @output
      CLC
      RTS
@call:
      STZ   p_started
      JSR   p_next
      JSR   p_skip_ws
      JSR   p_peek
      BCC   @fail
      CMP   #')'
      BEQ   @call_close
      JSR   p_parse_expression
      BCS   @fail
      LDA   #1
      STA   p_started
@call_close:
      LDA   #')'
      JSR   p_expect_char
      BCS   @fail
      LDA   p_started
      JSR   p_emit_sig_saved
      BCS   @output
      JSR   p_emit_call_saved
      BCS   @output
      CLC
      RTS
@bare_call:
      LDA   #0
      JSR   p_emit_sig_saved
      BCS   @output
      JSR   p_emit_call_saved
      BCS   @output
      CLC
      RTS
@output:
      INC   p_emit_error
@fail:
      SEC
      RTS

p_parse_writeln:
      LDA   #'('
      JSR   p_expect_char
      BCS   @fail
      JSR   p_skip_ws
      JSR   p_peek
      BCC   @fail
      CMP   #')'
      BEQ   @close
      CMP   #$27
      BNE   @value
      JSR   p_next
      JSR   p_scan_string_length
      BCS   @fail
      LDA   p_string_len
      CMP   #3
      BCC   @short_string
      JSR   p_emit_line_string
      BCS   @output
      BRA   @string_close
@short_string:
      JSR   p_emit_short_string
      BCS   @output
@string_close:
      LDA   #')'
      JSR   p_expect_char
      BCS   @fail
      CLC
      RTS
@value:
      JSR   p_parse_expression
      BCS   @fail
      LDA   #<asm_jsr_write_byte_ln
      STA   p_word
      LDA   #>asm_jsr_write_byte_ln
      STA   p_word+1
      JSR   p_emit_text
      BCS   @output
      BRA   @string_close
@close:
      LDA   #')'
      JSR   p_expect_char
      BCS   @fail
      LDA   #$0A
      JSR   p_emit_print_char
      BCS   @output
      CLC
      RTS
@output:
      INC   p_emit_error
@fail:
      SEC
      RTS

; Count a Pascal string without consuming it. The opening quote is already gone.
p_scan_string_length:
      STZ   p_string_len
      JSR   p_mark_position
@string:
      JSR   p_next
      BCC   @invalid
      CMP   #$27
      BNE   @string_char
      JSR   p_peek
      BCC   @done
      CMP   #$27
      BNE   @done
      JSR   p_next
      LDA   #$27
@string_char:
      CMP   #$20
      BCC   @invalid
      CMP   #$7F
      BCS   @invalid
      INC   p_string_len
      BNE   @string
@invalid:
      SEC
      PHP
      BRA   @restore
@done:
      CLC
      PHP
@restore:
      JSR   p_restore_position
      PLP
      RTS

p_emit_short_string:
@string:
      JSR   p_next
      BCC   @fail
      CMP   #$27
      BNE   @string_char
      JSR   p_peek
      BCC   @done
      CMP   #$27
      BNE   @done
      JSR   p_next
      LDA   #$27
@string_char:
      CMP   #$20
      BCC   @fail
      CMP   #$7F
      BCS   @fail
      JSR   p_emit_print_char
      BCS   @fail
      BRA   @string
@done:
      LDA   #$0A
      JMP   p_emit_print_char
@fail:
      SEC
      RTS

p_emit_line_string:
      LDA   #<asm_jsr_write_line
      LDX   #>asm_jsr_write_line
      JSR   p_emit_ax_text
      BCS   @fail
@string:
      JSR   p_next
      BCC   @fail
      CMP   #$27
      BNE   @string_char
      JSR   p_peek
      BCC   @done
      CMP   #$27
      BNE   @done
      JSR   p_next
      LDA   #$27
@string_char:
      STA   p_char
      LDA   #'$'
      JSR   p_emit
      BCS   @fail
      LDA   p_char
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   #','
      JSR   p_emit
      BCS   @fail
      BRA   @string
@done:
      LDA   #<asm_line_end
      LDX   #>asm_line_end
      JMP   p_emit_ax_text
@fail:
      SEC
      RTS

p_parse_if:
      JSR   p_parse_expression
      BCS   @fail
      LDA   #<kw_then
      STA   p_word
      LDA   #>kw_then
      STA   p_word+1
      JSR   p_expect_word
      BCS   @fail
      LDX   p_control_depth
      CPX   #8
      BCS   @fail
      JSR   p_new_label
      JSR   p_store_control_a
      JSR   p_emit_branch_false
      BCS   @output
      INC   p_control_depth
      JSR   p_parse_statement
      DEC   p_control_depth
      BCS   @fail
      LDA   #<kw_else
      LDX   #>kw_else
      JSR   p_peek_keyword
      BCC   @no_else
      LDA   #<kw_else
      STA   p_word
      LDA   #>kw_else
      STA   p_word+1
      JSR   p_expect_word
      BCS   @fail
      JSR   p_new_label
      LDX   p_control_depth
      JSR   p_store_control_b
      JSR   p_emit_jump
      BCS   @output
      LDX   p_control_depth
      JSR   p_load_control_a
      JSR   p_emit_label
      BCS   @output
      INC   p_control_depth
      JSR   p_parse_statement
      DEC   p_control_depth
      BCS   @fail
      LDX   p_control_depth
      JSR   p_load_control_b
      JSR   p_emit_label
      RTS
@no_else:
      LDX   p_control_depth
      JSR   p_load_control_a
      JSR   p_emit_label
      RTS
@output:
      INC   p_emit_error
@fail:
      SEC
      RTS

p_parse_while:
      LDX   p_control_depth
      CPX   #8
      BCS   @fail
      JSR   p_new_label
      JSR   p_store_control_a
      JSR   p_emit_label
      BCS   @output
      JSR   p_parse_expression
      BCS   @fail
      LDA   #<kw_do
      STA   p_word
      LDA   #>kw_do
      STA   p_word+1
      JSR   p_expect_word
      BCS   @fail
      JSR   p_new_label
      LDX   p_control_depth
      JSR   p_store_control_b
      JSR   p_emit_branch_false
      BCS   @output
      INC   p_control_depth
      JSR   p_parse_statement
      DEC   p_control_depth
      BCS   @fail
      LDX   p_control_depth
      JSR   p_load_control_a
      JSR   p_emit_jump
      BCS   @output
      LDX   p_control_depth
      JSR   p_load_control_b
      JSR   p_emit_label
      RTS
@output:
      INC   p_emit_error
@fail:
      SEC
      RTS

; expression = additive [ (= | <> | < | <= | > | >=) additive ]
p_parse_expression:
      JSR   p_parse_additive
      BCS   @fail
      JSR   p_skip_ws
      JSR   p_peek
      BCC   @ok
      CMP   #'='
      BEQ   @equal
      CMP   #'<'
      BEQ   @less
      CMP   #'>'
      BNE   @ok
      LDA   #5
      STA   p_operator
      JSR   p_next
      JSR   p_peek
      BCC   @relation
      CMP   #'='
      BNE   @relation
      JSR   p_next
      INC   p_operator
      BRA   @relation
@equal:
      LDA   #1
      STA   p_operator
      JSR   p_next
      BRA   @relation
@less:
      LDA   #3
      STA   p_operator
      JSR   p_next
      JSR   p_peek
      BCC   @relation
      CMP   #'>'
      BNE   :+
      JSR   p_next
      LDA   #2
      STA   p_operator
      BRA   @relation
:     CMP   #'='
      BNE   @relation
      JSR   p_next
      LDA   #4
      STA   p_operator
@relation:
      LDA   p_operator
      JSR   p_push_operator
      BCS   @fail
      JSR   p_emit_binary_prep
      BCS   @drop_fail
      JSR   p_parse_additive
      BCS   @drop_fail
      JSR   p_pop_operator
      JSR   p_emit_compare
      BCS   @fail
      LDA   #TYPE_BOOLEAN
      STA   p_expr_type
@ok:
      CLC
@fail:
      RTS
@drop_fail:
      DEC   p_operator_depth
      SEC
      RTS

p_parse_additive:
      JSR   p_parse_term
      BCS   @fail
@operator:
      JSR   p_skip_ws
      JSR   p_peek
      BCC   @ok
      CMP   #'+'
      BEQ   @have
      CMP   #'-'
      BNE   @ok
@have:
      STA   p_operator
      JSR   p_next
      LDA   p_operator
      JSR   p_push_operator
      BCS   @fail
      JSR   p_emit_binary_prep
      BCS   @drop_fail
      JSR   p_parse_term
      BCS   @drop_fail
      JSR   p_pop_operator
      LDA   p_operator
      CMP   #'+'
      BNE   @subtract
      LDA   #<asm_add
      LDX   #>asm_add
      BRA   @emit
@subtract:
      LDA   #<asm_subtract
      LDX   #>asm_subtract
@emit:
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   #TYPE_BYTE
      STA   p_expr_type
      BRA   @operator
@ok:
      CLC
@fail:
      RTS
@drop_fail:
      DEC   p_operator_depth
      SEC
      RTS

p_parse_term:
      JSR   p_parse_factor
      BCS   @fail
@operator:
      LDA   #<kw_mod
      LDX   #>kw_mod
      JSR   p_peek_keyword
      BCC   @ok
      LDA   #<kw_mod
      STA   p_word
      LDA   #>kw_mod
      STA   p_word+1
      JSR   p_expect_word
      BCS   @fail
      JSR   p_emit_binary_prep
      BCS   @fail
      JSR   p_parse_factor
      BCS   @fail
      LDA   #<asm_mod
      LDX   #>asm_mod
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   #TYPE_BYTE
      STA   p_expr_type
      BRA   @operator
@ok:
      CLC
@fail:
      RTS

p_parse_factor:
      JSR   p_skip_ws
      JSR   p_peek
      BCC   @fail
      CMP   #'('
      BNE   @value
      JSR   p_next
      JSR   p_parse_expression
      BCS   @fail
      LDA   #')'
      JSR   p_expect_char
      RTS
@value:
      JSR   p_parse_byte_value
      BCS   @fail
      LDA   #TYPE_BYTE
      STA   p_expr_type
      CLC
@fail:
      RTS

; Add the current identifier to the transient symbol table. Names remain in
; source form for diagnostics/code generation; comparisons are already folded
; to uppercase by p_capture_identifier.
p_symbol_add:
      LDX   #0
@duplicate:
      CPX   p_symbol_count
      BCS   @store
      STX   p_symbol_iter
      JSR   p_symbol_pointer
      LDY   #0
      LDA   (p_word),Y
      CMP   p_ident_len
      BNE   @next
      LDY   #0
@compare:
      CPY   p_ident_len
      BCS   @fail
      LDA   p_ident,Y
      PHY
      INY
      INY
      CMP   (p_word),Y
      PLY
      BNE   @next
      INY
      BRA   @compare
@next:
      LDX   p_symbol_iter
      INX
      BRA   @duplicate
@store:
      CPX   #SYMBOL_CAP
      BCS   @fail
      JSR   p_symbol_pointer
      LDY   #0
      LDA   p_ident_len
      STA   (p_word),Y
      INY
      LDA   #TYPE_NONE
      STA   (p_word),Y
      LDX   #0
@copy:
      CPX   p_ident_len
      BCS   @stored
      INY
      LDA   p_ident,X
      STA   (p_word),Y
      INX
      BRA   @copy
@stored:
      INC   p_symbol_count
      CLC
      RTS
@fail:
      SEC
      RTS

; X is a symbol index; return its lower-RAM address in p_word.
p_symbol_pointer:
      TXA
      ASL
      STA   p_remainder
      STZ   p_remainder+1
      TXA
      STA   p_decimal
      STZ   p_decimal+1
      LDY   #5
@times32:
      ASL   p_decimal
      ROL   p_decimal+1
      DEY
      BNE   @times32
      CLC
      LDA   p_decimal
      ADC   p_remainder
      ADC   #<SYMBOL_BASE
      STA   p_word
      LDA   p_decimal+1
      ADC   p_remainder+1
      ADC   #>SYMBOL_BASE
      STA   p_word+1
      RTS

; Finish all names in the current declaration group as Byte symbols and emit
; their BSS definitions only after the type has been parsed successfully.
p_emit_symbol_group:
      LDX   p_symbol_group
@symbol:
      CPX   p_symbol_count
      BCS   @done
      STX   p_symbol_iter
      JSR   p_symbol_pointer
      LDY   #1
      LDA   #TYPE_BYTE
      STA   (p_word),Y
      LDY   #0
      LDA   (p_word),Y
      STA   p_digit
      CLC
      LDA   p_word
      ADC   #2
      STA   p_word
      BCC   :+
      INC   p_word+1
:
@name:
      LDA   p_digit
      BEQ   @suffix
      LDY   #0
      LDA   (p_word),Y
      JSR   p_emit
      BCS   @fail
      INC   p_word
      BNE   :+
      INC   p_word+1
:     DEC   p_digit
      BRA   @name
@suffix:
      LDA   #<asm_byte_res
      STA   p_word
      LDA   #>asm_byte_res
      STA   p_word+1
      JSR   p_emit_text
      BCS   @fail
      LDX   p_symbol_iter
      INX
      BRA   @symbol
@done:
      CLC
@fail:
      RTS

p_emit_ax_text:
      STA   p_word
      STX   p_word+1
      JMP   p_emit_text

p_push_operator:
      LDX   p_operator_depth
      CPX   #8
      BCS   @fail
      STA   p_operator_stack,X
      INC   p_operator_depth
      CLC
@fail:
      RTS

p_pop_operator:
      DEC   p_operator_depth
      LDX   p_operator_depth
      LDA   p_operator_stack,X
      STA   p_operator
      RTS

p_require_temp:
      LDA   p_temp_emitted
      BNE   @done
      LDA   #<asm_temp_decl
      LDX   #>asm_temp_decl
      JSR   p_emit_ax_text
      BCS   @fail
      INC   p_temp_emitted
@done:
      CLC
@fail:
      RTS

p_emit_binary_prep:
      JSR   p_require_temp
      BCS   @fail
      LDA   #<asm_pha
      LDX   #>asm_pha
      JMP   p_emit_ax_text
@fail:
      RTS

; p_operator selects =, <>, <, <=, >, >= as 1..6.
p_emit_compare:
      LDA   #<asm_compare_start
      LDX   #>asm_compare_start
      JSR   p_emit_ax_text
      BCS   @fail
      LDX   p_operator
      DEX
      BMI   @fail
      CPX   #6
      BCS   @fail
      LDA   compare_suffix_lo,X
      STA   p_word
      LDA   compare_suffix_hi,X
      STA   p_word+1
      JMP   p_emit_text
@fail:
      SEC
      RTS

p_new_label:
      INC   p_label
      BNE   :+
      INC   p_label+1
:     LDA   p_label
      STA   p_label_saved
      LDA   p_label+1
      STA   p_label_saved+1
      RTS

p_store_control_a:
      LDA   p_label_saved
      STA   p_control_a_lo,X
      LDA   p_label_saved+1
      STA   p_control_a_hi,X
      RTS

p_store_control_b:
      LDA   p_label_saved
      STA   p_control_b_lo,X
      LDA   p_label_saved+1
      STA   p_control_b_hi,X
      RTS

p_load_control_a:
      LDA   p_control_a_lo,X
      STA   p_label_saved
      LDA   p_control_a_hi,X
      STA   p_label_saved+1
      RTS

p_load_control_b:
      LDA   p_control_b_lo,X
      STA   p_label_saved
      LDA   p_control_b_hi,X
      STA   p_label_saved+1
      RTS

p_emit_label_id:
      LDA   p_label_saved+1
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   p_label_saved
      JMP   p_emit_hex_byte
@fail:
      RTS

p_emit_label:
      LDA   #<asm_label_prefix
      LDX   #>asm_label_prefix
      JSR   p_emit_ax_text
      BCS   @fail
      JSR   p_emit_label_id
      BCS   @fail
      LDA   #<asm_label_end
      LDX   #>asm_label_end
      JMP   p_emit_ax_text
@fail:
      RTS

p_emit_branch_false:
      LDA   #<asm_branch_false
      LDX   #>asm_branch_false
      JSR   p_emit_ax_text
      BCS   @fail
      JSR   p_emit_label_id
      BCS   @fail
      LDA   #<asm_branch_end
      LDX   #>asm_branch_end
      JMP   p_emit_ax_text
@fail:
      RTS

p_emit_jump:
      LDA   #<asm_jump
      LDX   #>asm_jump
      JSR   p_emit_ax_text
      BCS   @fail
      JSR   p_emit_label_id
      BCS   @fail
      LDA   #$0A
      JMP   p_emit
@fail:
      RTS

p_emit_print_char:
      JSR   p_emit_load_byte
      BCS   @fail
      LDA   #<asm_jsr
      STA   p_word
      LDA   #>asm_jsr
      STA   p_word+1
      JSR   p_emit_text
@fail:
      RTS

p_emit_load_byte:
      STA   p_char
      LDA   #<asm_lda
      STA   p_word
      LDA   #>asm_lda
      STA   p_word+1
      JSR   p_emit_text
      BCS   @fail
      LDA   p_char
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   #$0A
      JSR   p_emit
@fail:
      RTS

p_emit_hex_byte:
      STA   p_char
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
      RTS
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
      LDA   #' '
      JSR   p_emit
      BCS   @fail
      JSR   p_emit_identifier
      BCS   @fail
      LDA   #$0A
      JMP   p_emit
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

; Non-consuming keyword lookahead. A/X points to an uppercase Z string.
p_peek_keyword:
      STA   p_word
      STX   p_word+1
      JSR   p_mark_position
      JSR   p_capture_identifier
      BCS   @no
      LDA   p_word
      LDX   p_word+1
      JSR   p_ident_is
      PHP
      BRA   @restore
@no:
      CLC
      PHP
@restore:
      JSR   p_restore_position
      PLP
      RTS

p_mark_position:
      LDA   p_src
      STA   p_mark_src
      LDA   p_src+1
      STA   p_mark_src+1
      LDA   p_src+2
      STA   p_mark_src+2
      LDA   p_left
      STA   p_mark_left
      LDA   p_left+1
      STA   p_mark_left+1
      LDA   p_line
      STA   p_mark_line
      LDA   p_line+1
      STA   p_mark_line+1
      LDA   p_column
      STA   p_mark_column
      LDA   p_column+1
      STA   p_mark_column+1
      RTS

p_restore_position:
      LDA   p_mark_src
      STA   p_src
      LDA   p_mark_src+1
      STA   p_src+1
      LDA   p_mark_src+2
      STA   p_src+2
      LDA   p_mark_left
      STA   p_left
      LDA   p_mark_left+1
      STA   p_left+1
      LDA   p_mark_line
      STA   p_line
      LDA   p_mark_line+1
      STA   p_line+1
      LDA   p_mark_column
      STA   p_column
      LDA   p_mark_column+1
      STA   p_column+1
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

; Capture one case-insensitive Pascal identifier for keyword comparison or
; direct NDK symbol emission.
p_capture_identifier:
      JSR   p_skip_ws
      JSR   p_peek_upper
      BCC   @bad
      JSR   p_is_ident_start
      BCC   @bad
      STZ   p_ident_len
      LDA   #$05
      STA   p_ident_hash+0
      LDA   #$15
      STA   p_ident_hash+1
      STZ   p_ident_hash+2
@loop:
      JSR   p_read_upper
      BCC   @bad
      LDX   p_ident_len
      CPX   #P_IDENT_CAP
      BCS   @bad
      STA   p_ident,X
      JSR   p_hash_char
      INC   p_ident_len
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

; A/X points to an uppercase Z string. Carry is set on an exact match.
p_ident_is:
      STA   p_word
      STX   p_word+1
      LDY   #0
@char:
      LDA   (p_word),Y
      BEQ   @end
      CPY   p_ident_len
      BCS   @no
      CMP   p_ident,Y
      BNE   @no
      INY
      BRA   @char
@end:
      CPY   p_ident_len
      BNE   @no
      SEC
      RTS
@no:
      CLC
      RTS

p_save_identifier:
      LDA   p_ident_len
      STA   p_saved_ident_len
      TAX
@copy:
      DEX
      BMI   @done
      LDA   p_ident,X
      STA   p_saved_ident,X
      BRA   @copy
@done:
      LDX   #2
@hash:
      LDA   p_ident_hash,X
      STA   p_saved_hash,X
      DEX
      BPL   @hash
      RTS

p_save_call_identifier:
      LDA   p_ident_len
      STA   p_call_ident_len
      TAX
@copy:
      DEX
      BMI   @hash_start
      LDA   p_ident,X
      STA   p_call_ident,X
      BRA   @copy
@hash_start:
      LDX   #2
@hash:
      LDA   p_ident_hash,X
      STA   p_call_hash,X
      DEX
      BPL   @hash
      RTS

; DJB2 truncated to 24 bits; the host generator rejects binding collisions.
p_hash_char:
      STA   p_expected
      LDA   p_ident_hash+0
      STA   p_remainder+0
      LDA   p_ident_hash+1
      STA   p_remainder+1
      LDA   p_ident_hash+2
      STA   p_char
      LDX   #5
@shift:
      ASL   p_ident_hash+0
      ROL   p_ident_hash+1
      ROL   p_ident_hash+2
      DEX
      BNE   @shift
      CLC
      LDA   p_ident_hash+0
      ADC   p_remainder+0
      STA   p_ident_hash+0
      LDA   p_ident_hash+1
      ADC   p_remainder+1
      STA   p_ident_hash+1
      LDA   p_ident_hash+2
      ADC   p_char
      STA   p_ident_hash+2
      CLC
      LDA   p_ident_hash+0
      ADC   p_expected
      STA   p_ident_hash+0
      BCC   @done
      INC   p_ident_hash+1
      BNE   @done
      INC   p_ident_hash+2
@done:
      LDA   p_expected
      RTS

p_emit_saved_identifier:
      LDX   #0
@char:
      CPX   p_saved_ident_len
      BCS   @ok
      LDA   p_saved_ident,X
      JSR   p_emit
      BCS   @fail
      INX
      BRA   @char
@ok:
      CLC
      RTS
@fail:
      SEC
      RTS

p_emit_call_identifier_name:
      LDX   #0
@char:
      CPX   p_call_ident_len
      BCS   @ok
      LDA   p_call_ident,X
      JSR   p_emit
      BCS   @fail
      INX
      BRA   @char
@ok:
      CLC
      RTS
@fail:
      SEC
      RTS

; NovaFoo maps mechanically to canonical FOO.INC/FOO.S files. No NDK API or
; hardware address is embedded in NPC; routine statements retain their symbol.
p_store_unit:
      STZ   p_emit_error
      LDX   p_unit_count
      CPX   #P_UNIT_CAP
      BCS   @bad
      LDA   p_ident_len
      CMP   #5
      BCC   @bad
      SEC
      SBC   #4
      CMP   #P_UNIT_NAME_CAP+1
      BCS   @bad
      STA   p_unit_len,X
      LDA   p_ident+0
      CMP   #'N'
      BNE   @bad
      LDA   p_ident+1
      CMP   #'O'
      BNE   @bad
      LDA   p_ident+2
      CMP   #'V'
      BNE   @bad
      LDA   p_ident+3
      CMP   #'A'
      BNE   @bad
      TXA
      ASL
      ASL
      STA   p_expected
      ASL
      CLC
      ADC   p_expected
      TAX
      LDY   #4
@copy:
      CPY   p_ident_len
      BCS   @stored
      LDA   p_ident,Y
      STA   p_unit_names,X
      INX
      INY
      BRA   @copy
@stored:
      LDX   p_unit_count
      INC   p_unit_count
      LDA   #0
      JSR   p_emit_unit_line
      BCS   @emit_bad
      LDX   p_unit_count
      DEX
      LDA   #2
      JSR   p_emit_unit_line
      BCC   @ok
@emit_bad:
      INC   p_emit_error
@bad:
      SEC
      RTS
@ok:
      CLC
      RTS

; X selects a stored unit. A=0 emits declarations, A=1 source, A=2 Pascal ABI.
p_emit_unit_line:
      STA   p_char
      STX   p_unit_iter
      LDA   #<asm_include_start
      STA   p_word
      LDA   #>asm_include_start
      STA   p_word+1
      JSR   p_emit_text
      BCS   @fail
      LDX   p_unit_iter
      TXA
      ASL
      ASL
      STA   p_expected
      ASL
      CLC
      ADC   p_expected
      CLC
      ADC   #<p_unit_names
      STA   p_word
      LDA   #>p_unit_names
      ADC   #0
      STA   p_word+1
      LDX   p_unit_iter
      LDA   p_unit_len,X
      STA   p_digit
@name:
      LDA   p_digit
      BEQ   @suffix
      LDY   #0
      LDA   (p_word),Y
      JSR   p_emit
      BCS   @fail
      INC   p_word
      BNE   :+
      INC   p_word+1
:     DEC   p_digit
      BRA   @name
@suffix:
      LDA   p_char
      BEQ   @declarations
      CMP   #1
      BEQ   @source
      LDA   #<asm_pascal_end
      STA   p_word
      LDA   #>asm_pascal_end
      BRA   @emit_suffix
@declarations:
      LDA   #<asm_inc_end
      STA   p_word
      LDA   #>asm_inc_end
      BRA   @emit_suffix
@source:
      LDA   #<asm_source_end
      STA   p_word
      LDA   #>asm_source_end
@emit_suffix:
      STA   p_word+1
      JMP   p_emit_text
@fail:
      SEC
      RTS

p_emit_unit_sources:
      LDX   #0
@unit:
      CPX   p_unit_count
      BCS   @done
      LDA   #1
      JSR   p_emit_unit_line
      BCS   @fail
      LDX   p_unit_iter
      INX
      BRA   @unit
@done:
      CLC
@fail:
      RTS

p_emit_identifier:
      LDX   #0
@char:
      CPX   p_ident_len
      BCS   @ok
      LDA   p_ident,X
      JSR   p_emit
      BCS   @fail
      INX
      BRA   @char
@ok:
      CLC
      RTS
@fail:
      SEC
      RTS

p_emit_call_saved:
      LDA   #<asm_jsr_prefix
      STA   p_word
      LDA   #>asm_jsr_prefix
      STA   p_word+1
      JSR   p_emit_text
      BCS   @fail
      JSR   p_emit_saved_identifier
      BCS   @fail
      LDA   #$0A
      JMP   p_emit
@fail:
      SEC
      RTS

p_emit_call_buffered:
      LDA   #<asm_jsr_prefix
      STA   p_word
      LDA   #>asm_jsr_prefix
      STA   p_word+1
      JSR   p_emit_text
      BCS   @fail
      JSR   p_emit_call_identifier_name
      BCS   @fail
      LDA   #$0A
      JMP   p_emit
@fail:
      SEC
      RTS

; A is 0/1 for a statement's argument count, or 2/3 for a Byte function's
; exact no-argument/one-argument signature.
p_emit_sig_saved:
      STA   p_digit
      JSR   p_emit_sig_start
      BCS   @fail
      LDX   #<p_saved_hash
      LDY   #>p_saved_hash
      JSR   p_emit_hash
      BCS   @fail
      JMP   p_emit_sig_end
@fail:
      SEC
      RTS

p_emit_sig_call:
      STA   p_digit
      JSR   p_emit_sig_start
      BCS   @fail
      LDX   #<p_call_hash
      LDY   #>p_call_hash
      JSR   p_emit_hash
      BCS   @fail
      JMP   p_emit_sig_end
@fail:
      SEC
      RTS

p_emit_sig_start:
      LDA   #<asm_sig_start
      STA   p_word
      LDA   #>asm_sig_start
      STA   p_word+1
      JMP   p_emit_text

; X/Y points to a three-byte little-endian hash.
p_emit_hash:
      STX   p_word
      STY   p_word+1
      LDY   #2
      LDA   (p_word),Y
      JSR   p_emit_hex_byte
      BCS   @fail
      LDY   #1
      LDA   (p_word),Y
      JSR   p_emit_hex_byte
      BCS   @fail
      LDY   #0
      LDA   (p_word),Y
      JMP   p_emit_hex_byte
@fail:
      SEC
      RTS

p_emit_sig_end:
      LDX   p_digit
      LDA   sig_suffix_lo,X
      STA   p_word
      LDA   sig_suffix_hi,X
      STA   p_word+1
      JMP   p_emit_text

p_emit_constant_identifier:
      LDA   #<asm_const_start
      STA   p_word
      LDA   #>asm_const_start
      STA   p_word+1
      JSR   p_emit_text
      BCS   @fail
      LDX   #<p_ident_hash
      LDY   #>p_ident_hash
      JSR   p_emit_hash
      BCS   @fail
      LDA   #<asm_const_end
      STA   p_word
      LDA   #>asm_const_end
      STA   p_word+1
      JSR   p_emit_text
      BCS   @fail
      JSR   p_emit_identifier
      BCS   @fail
      LDA   #$0A
      JMP   p_emit
@fail:
      SEC
      RTS

p_emit_load_identifier:
      LDA   #<asm_lda_direct
      STA   p_word
      LDA   #>asm_lda_direct
      STA   p_word+1
      JSR   p_emit_text
      BCS   @fail
      JSR   p_emit_identifier
      BCS   @fail
      LDA   #$0A
      JMP   p_emit
@fail:
      SEC
      RTS

p_emit_store_saved:
      LDA   #<asm_sta_direct
      STA   p_word
      LDA   #>asm_sta_direct
      STA   p_word+1
      JSR   p_emit_text
      BCS   @fail
      JSR   p_emit_saved_identifier
      BCS   @fail
      LDA   #$0A
      JMP   p_emit
@fail:
      SEC
      RTS

; Parse a byte value into A in generated code. Byte(NDK_CONSTANT) is an
; immediate typed constant; function arguments and results use A.
p_parse_byte_value:
      JSR   p_skip_ws
      JSR   p_peek
      long_bcc @bad
      CMP   #$27
      long_beq @character
      CMP   #'$'
      long_beq @hex
      CMP   #'0'
      BCC   @identifier
      CMP   #'9'+1
      long_bcc @decimal
@identifier:
      JSR   p_capture_identifier
      long_bcs @bad
      LDA   #<kw_byte
      LDX   #>kw_byte
      JSR   p_ident_is
      BCS   @constant
      JSR   p_skip_ws
      JSR   p_peek
      BCC   @load_identifier
      CMP   #'('
      BNE   @load_identifier
      LDA   p_in_argument
      long_bne @bad
      JSR   p_save_call_identifier
      STZ   p_started
      JSR   p_next
      JSR   p_skip_ws
      JSR   p_peek
      long_bcc @bad
      CMP   #')'
      BEQ   @function_close
      INC   p_in_argument
      JSR   p_parse_byte_value
      STZ   p_in_argument
      long_bcs @bad
      LDA   #1
      STA   p_started
@function_close:
      LDA   #')'
      JSR   p_expect_char
      long_bcs @bad
      LDA   p_started
      CLC
      ADC   #2
      JSR   p_emit_sig_call
      long_bcs @bad
      JSR   p_emit_call_buffered
      RTS
@constant:
      LDA   #'('
      JSR   p_expect_char
      long_bcs @bad
      JSR   p_capture_identifier
      long_bcs @bad
      LDA   #')'
      JSR   p_expect_char
      long_bcs @bad
      JSR   p_emit_constant_identifier
      RTS
@load_identifier:
      JSR   p_emit_load_identifier
      RTS
@character:
      JSR   p_next
      JSR   p_next
      long_bcc @bad
      STA   p_char
      LDA   #$27
      JSR   p_expect_char
      long_bcs @bad
      LDA   p_char
      JSR   p_emit_load_byte
      RTS
@hex:
      JSR   p_next
      STZ   p_digit
      STZ   p_char
@hex_digit:
      JSR   p_peek_upper
      BCC   @hex_done
      JSR   p_hex_nibble
      BCC   @hex_done
      LDX   p_digit
      CPX   #2
      BCS   @bad
      ASL   p_char
      ASL   p_char
      ASL   p_char
      ASL   p_char
      ORA   p_char
      STA   p_char
      INC   p_digit
      JSR   p_next
      BRA   @hex_digit
@hex_done:
      LDA   p_digit
      BEQ   @bad
      LDA   p_char
      JSR   p_emit_load_byte
      RTS
@decimal:
      STZ   p_decimal
      STZ   p_decimal+1
@decimal_digit:
      JSR   p_peek
      BCC   @decimal_done
      CMP   #'0'
      BCC   @decimal_done
      CMP   #'9'+1
      BCS   @decimal_done
      SEC
      SBC   #'0'
      STA   p_digit
      LDA   p_decimal
      ASL
      STA   p_remainder
      LDA   p_decimal+1
      ROL
      STA   p_remainder+1
      ASL   p_decimal
      ROL   p_decimal+1
      ASL   p_decimal
      ROL   p_decimal+1
      ASL   p_decimal
      ROL   p_decimal+1
      CLC
      LDA   p_decimal
      ADC   p_remainder
      ADC   p_digit
      STA   p_decimal
      LDA   p_decimal+1
      ADC   p_remainder+1
      STA   p_decimal+1
      BNE   @bad
      JSR   p_next
      BRA   @decimal_digit
@decimal_done:
      LDA   p_decimal
      JSR   p_emit_load_byte
      RTS
@bad:
      SEC
      RTS

; A is an uppercase hexadecimal character. Carry set and A=0..15 if valid.
p_hex_nibble:
      CMP   #'0'
      BCC   @bad
      CMP   #'9'+1
      BCS   @alpha
      SEC
      SBC   #'0'
      SEC
      RTS
@alpha:
      CMP   #'A'
      BCC   @bad
      CMP   #'F'+1
      BCS   @bad
      SEC
      SBC   #'A'-10
      SEC
      RTS
@bad:
      CLC
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
      BNE   @column
      INC   p_line
      BNE   :+
      INC   p_line+1
:     LDA   #1
      STA   p_column
      STZ   p_column+1
      BRA   @read
@column:
      INC   p_column
      BNE   @read
      INC   p_column+1
@read:
      SEC
@eof:
      RTS

      .segment "RODATA"
kw_program: .byte "PROGRAM", 0
kw_begin:   .byte "BEGIN", 0
kw_uses:    .byte "USES", 0
kw_var:     .byte "VAR", 0
kw_byte:    .byte "BYTE", 0
kw_writeln: .byte "WRITELN", 0
kw_end:     .byte "END", 0
kw_if:      .byte "IF", 0
kw_then:    .byte "THEN", 0
kw_else:    .byte "ELSE", 0
kw_while:   .byte "WHILE", 0
kw_do:      .byte "DO", 0
kw_mod:     .byte "MOD", 0
hex_digits: .byte "0123456789ABCDEF"
decimal_place_lo: .byte <10000, <1000, <100, <10, <1
decimal_place_hi: .byte >10000, >1000, >100, >10, >1
asm_comment: .byte "; ", 0
asm_import:  .byte ".SEGMENT ", 34, "CODE", 34, $0A, ".IMPORT P_WRITE_CHAR", $0A, ".IMPORT P_WRITE_BYTE", $0A, ".IMPORT P_WRITE_BYTE_LN", $0A, ".IMPORT I_P_WRITE_LINE", $0A, 0
asm_bss:    .byte ".SEGMENT ", 34, "BSS", 34, $0A, 0
asm_code:   .byte ".SEGMENT ", 34, "CODE", 34, $0A, 0
asm_byte_res: .byte ": .RES 1", $0A, 0
asm_lda:    .byte "LDA #$", 0
asm_lda_direct: .byte "LDA ", 0
asm_sta_direct: .byte "STA ", 0
asm_jsr:    .byte "JSR P_WRITE_CHAR", $0A, 0
asm_jsr_write_byte: .byte "JSR P_WRITE_BYTE", $0A, 0
asm_jsr_write_byte_ln: .byte "JSR P_WRITE_BYTE_LN", $0A, 0
asm_jsr_write_line: .byte "JSR I_P_WRITE_LINE", $0A, ".BYTE ", 0
asm_line_end: .byte "$00", $0A, 0
asm_rts:    .byte "RTS", $0A, 0
asm_temp_decl: .byte ".SEGMENT ", 34, "BSS", 34, $0A, "__NP_RHS: .RES 1", $0A, ".SEGMENT ", 34, "CODE", 34, $0A, 0
asm_pha:    .byte "PHA", $0A, 0
asm_add:    .byte "STA __NP_RHS", $0A, "PLA", $0A, "CLC", $0A, "ADC __NP_RHS", $0A, 0
asm_subtract: .byte "STA __NP_RHS", $0A, "PLA", $0A, "SEC", $0A, "SBC __NP_RHS", $0A, 0
asm_mod:    .byte "STA __NP_RHS", $0A, "BNE :+", $0A, "PLA", $0A, "STP", $0A, ":", $0A, "PLA", $0A, ":", $0A, "CMP __NP_RHS", $0A, "BCC :+", $0A, "SEC", $0A, "SBC __NP_RHS", $0A, "BRA :-", $0A, ":", $0A, 0
asm_compare_start: .byte "STA __NP_RHS", $0A, "PLA", $0A, "CMP __NP_RHS", $0A, 0
asm_compare_eq: .byte "BNE :+", $0A, "LDA #$01", $0A, "BRA :++", $0A, ":", $0A, "LDA #$00", $0A, ":", $0A, 0
asm_compare_ne: .byte "BEQ :+", $0A, "LDA #$01", $0A, "BRA :++", $0A, ":", $0A, "LDA #$00", $0A, ":", $0A, 0
asm_compare_lt: .byte "BCS :+", $0A, "LDA #$01", $0A, "BRA :++", $0A, ":", $0A, "LDA #$00", $0A, ":", $0A, 0
asm_compare_le: .byte "BCC :+", $0A, "BEQ :+", $0A, "LDA #$00", $0A, "BRA :++", $0A, ":", $0A, "LDA #$01", $0A, ":", $0A, 0
asm_compare_gt: .byte "BCC :+", $0A, "BEQ :+", $0A, "LDA #$01", $0A, "BRA :++", $0A, ":", $0A, "LDA #$00", $0A, ":", $0A, 0
asm_compare_ge: .byte "BCC :+", $0A, "LDA #$01", $0A, "BRA :++", $0A, ":", $0A, "LDA #$00", $0A, ":", $0A, 0
compare_suffix_lo: .byte <asm_compare_eq, <asm_compare_ne, <asm_compare_lt, <asm_compare_le, <asm_compare_gt, <asm_compare_ge
compare_suffix_hi: .byte >asm_compare_eq, >asm_compare_ne, >asm_compare_lt, >asm_compare_le, >asm_compare_gt, >asm_compare_ge
asm_label_prefix: .byte "__NP_L", 0
asm_label_end: .byte ":", $0A, 0
asm_branch_false: .byte "CMP #$00", $0A, "BNE :+", $0A, "JMP __NP_L", 0
asm_branch_end: .byte $0A, ":", $0A, 0
asm_jump: .byte "JMP __NP_L", 0
asm_include_start: .byte ".INCLUDE ", 34, 0
asm_inc_end: .byte ".INC", 34, $0A, 0
asm_source_end: .byte ".S", 34, $0A, 0
asm_pascal_end: .byte ".NPI", 34, $0A, 0
asm_jsr_prefix: .byte "JSR ", 0
asm_sig_start: .byte ".ASSERT (__S", 0
asm_sig0_end: .byte " & $01) = $00", $0A, 0
asm_sig1_end: .byte " & $01) = $01", $0A, 0
asm_sig2_end: .byte ") = $02", $0A, 0
asm_sig3_end: .byte ") = $03", $0A, 0
asm_const_start: .byte ".ASSERT __C", 0
asm_const_end: .byte " = 1", $0A, "LDA #", 0
sig_suffix_lo: .byte <asm_sig0_end, <asm_sig1_end, <asm_sig2_end, <asm_sig3_end
sig_suffix_hi: .byte >asm_sig0_end, >asm_sig1_end, >asm_sig2_end, >asm_sig3_end

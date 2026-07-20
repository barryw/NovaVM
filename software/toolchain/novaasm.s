; Nova-native W65C02 assembler core.
;
; NOBJ v2 foundation implemented here:
;   - two passes
;   - case-insensitive mnemonics, directives, and symbols
;   - indexed symbols and ABS8/ABS16/LO8/HI8/REL8 relocations
;   - named initialized and zerofill sections
;   - nested XRAM-backed .INCLUDE sources
;   - JSR/JMP numeric or symbolic operands
;   - all W65C02 relative branches
;   - .BYTE, .WORD, .RES, and .ALIGN data directives
;
; NAS remains language-neutral. Undefined symbols stay undefined in the object
; for NL to resolve from another object or library.

      .setcpu "w65c02"
      .include "novaasm.inc"
      .include "nas_core.inc"
      .include "nova.inc"
      .include "nas_backend.inc"

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

NASM_SYMBOL_CAP       = 32
NASM_SYMBOL_TABLE_CAP = 32
NASM_RELOC_CAP        = 768
NASM_PASS_DEFINE      = 1
NASM_PASS_EMIT        = 2
NASM_PASS_RESOLVE     = 3
NASM_EXPR_DEPTH       = 8
NASM_DECL_IMPORT      = $01
NASM_DECL_EXPORT      = $02
NASM_SECTION_CAP      = 8
NASM_SECTION_NAME_CAP = 16

EXPR_OR  = 1
EXPR_XOR = 2
EXPR_AND = 3
EXPR_SHL = 4
EXPR_SHR = 5
EXPR_ADD = 6
EXPR_SUB = 7
EXPR_MUL = 8
EXPR_DIV = 9
EXPR_MOD = 10
EXPR_BOOL_OR  = 11
EXPR_BOOL_AND = 12
EXPR_BOOL_XOR = 13
EXPR_EQ = 14
EXPR_NE = 15
EXPR_LT = 16
EXPR_LE = 17
EXPR_GT = 18
EXPR_GE = 19

.ifdef NASM_CORE_OVERLAY
a_src             = NASCORE_ZP_BASE+0
a_left            = NASCORE_ZP_BASE+3
a_dst             = NASCORE_ZP_BASE+5
a_cap             = NASCORE_ZP_BASE+7
a_header          = NASCORE_ZP_BASE+9
a_code_len        = NASCORE_ZP_BASE+11
a_reloc_ptr       = NASCORE_ZP_BASE+13
a_reloc_len       = NASCORE_ZP_BASE+15
a_reloc_count     = NASCORE_ZP_BASE+17
a_word            = NASCORE_ZP_BASE+19
a_symbol_ptr      = NASCORE_ZP_BASE+21
a_symbol_len      = NASCORE_ZP_BASE+23
a_symbol_count    = NASCORE_ZP_BASE+24
a_symbol_index    = NASCORE_ZP_BASE+25
a_pass            = NASCORE_ZP_BASE+26
a_opcode          = NASCORE_ZP_BASE+27
a_section         = NASCORE_ZP_BASE+28
a_section_iter    = NASCORE_ZP_BASE+29
a_tmp0            = NASCORE_ZP_BASE+30
a_tmp1            = NASCORE_ZP_BASE+31
.else
      .segment "ZEROPAGE"
a_src:            .res 3
a_left:           .res 2
a_dst:            .res 2
a_cap:            .res 2
a_header:         .res 2
a_code_len:       .res 2
a_reloc_ptr:      .res 2
a_reloc_len:      .res 2
a_reloc_count:    .res 2
a_word:           .res 2
a_symbol_ptr:     .res 2
a_symbol_len:     .res 1
a_symbol_count:   .res 1
a_symbol_index:   .res 1
a_pass:           .res 1
a_opcode:         .res 1
a_section:        .res 1
a_section_iter:   .res 1
a_tmp0:           .res 1
a_tmp1:           .res 1
.endif

      .segment "BSS"
a_symbol:       .res NASM_SYMBOL_CAP
a_scope:        .res NASM_SYMBOL_CAP
a_scope_len:    .res 1
a_symbol_local: .res 1
a_local_total:  .res 1
a_anon_count:   .res 1
a_anon_target:  .res 1
a_reloc_buf:    .res NASM_RELOC_CAP
a_sym_names:    .res NASM_SYMBOL_TABLE_CAP * NASM_SYMBOL_CAP
a_sym_name_len: .res NASM_SYMBOL_TABLE_CAP
a_sym_value_l:  .res NASM_SYMBOL_TABLE_CAP
a_sym_value_h:  .res NASM_SYMBOL_TABLE_CAP
a_sym_section:  .res NASM_SYMBOL_TABLE_CAP
a_sym_flags:    .res NASM_SYMBOL_TABLE_CAP
a_sym_decl:     .res NASM_SYMBOL_TABLE_CAP
a_sym_forward:  .res NASM_SYMBOL_TABLE_CAP
a_mnemonic:     .res NASM_SYMBOL_CAP
a_mnemonic_len: .res 1
a_operand_symbol: .res 1
a_operand_index:  .res 1
a_operand_bytes:  .res 1
a_operand_reloc:  .res 1
a_relative_size:  .res 1
a_reloc_addend_l: .res 1
a_reloc_addend_h: .res 1
a_expr_depth:     .res 1
a_expr_rhs_l:     .res 1
a_expr_rhs_h:     .res 1
a_expr_rhs_sym:   .res 1
a_expr_rhs_index: .res 1
a_expr_acc_l:     .res 1
a_expr_acc_h:     .res 1
a_expr_rem_l:     .res 1
a_expr_rem_h:     .res 1
a_expr_digit:     .res 1
a_expr_min:       .res NASM_EXPR_DEPTH
a_expr_prec:      .res NASM_EXPR_DEPTH
a_expr_op:        .res NASM_EXPR_DEPTH
a_expr_lhs_l:     .res NASM_EXPR_DEPTH
a_expr_lhs_h:     .res NASM_EXPR_DEPTH
a_expr_lhs_sym:   .res NASM_EXPR_DEPTH
a_expr_lhs_index: .res NASM_EXPR_DEPTH
a_repeat_l:       .res 1
a_repeat_h:       .res 1
a_repeat_value:   .res 1
a_repeat_mask:    .res 1
a_resolve_changed:.res 1
a_resolve_pending:.res 1
a_assignment_expr:.res 1
a_section_count:  .res 1
a_sec_names:      .res NASM_SECTION_CAP * NASM_SECTION_NAME_CAP
a_sec_name_len:   .res NASM_SECTION_CAP
a_sec_flags:      .res NASM_SECTION_CAP
a_sec_align:      .res NASM_SECTION_CAP
a_sec_pos_l:      .res NASM_SECTION_CAP
a_sec_pos_h:      .res NASM_SECTION_CAP
a_sec_size_l:     .res NASM_SECTION_CAP
a_sec_size_h:     .res NASM_SECTION_CAP
a_sec_data_l:     .res NASM_SECTION_CAP
a_sec_data_h:     .res NASM_SECTION_CAP
a_include_depth:  .res 1
a_include_error:  .res 1
a_binary_ptr:     .res 3
a_binary_left:    .res 2
a_file_ptr:       .res 2
a_file_len:       .res 1
a_stack_src_l:    .res NASM_INCLUDE_DEPTH
a_stack_src_m:    .res NASM_INCLUDE_DEPTH
a_stack_src_h:    .res NASM_INCLUDE_DEPTH
a_stack_left_l:   .res NASM_INCLUDE_DEPTH
a_stack_left_h:   .res NASM_INCLUDE_DEPTH
a_stack_line_l:   .res NASM_INCLUDE_DEPTH
a_stack_line_h:   .res NASM_INCLUDE_DEPTH
a_stack_col_l:    .res NASM_INCLUDE_DEPTH
a_stack_col_h:    .res NASM_INCLUDE_DEPTH
a_stack_file_l:   .res NASM_INCLUDE_DEPTH
a_stack_file_h:   .res NASM_INCLUDE_DEPTH
a_stack_file_len: .res NASM_INCLUDE_DEPTH

.ifndef NASM_CORE_OVERLAY
      .export nasm_source_ptr
      .export nasm_source_len
      .export nasm_object_ptr
      .export nasm_object_cap
      .export nasm_object_len
      .export nasm_error
      .export nasm_error_line
      .export nasm_error_column
      .export nasm_source_name_ptr
      .export nasm_source_name_len
      .export nasm_include_name
      .export nasm_include_name_len
      .export nasm_include_ptr
      .export nasm_include_len
      .export nasm_include_display_ptr
      .export nasm_error_name_ptr
      .export nasm_error_name_len
nasm_source_ptr: .res 3
nasm_source_len: .res 2
nasm_source_name_ptr:.res 2
nasm_source_name_len:.res 1
nasm_object_ptr: .res 2
nasm_object_cap: .res 2
nasm_object_len: .res 2
nasm_error:      .res 1
nasm_error_line: .res 2
nasm_error_column:.res 2
nasm_include_name:.res NASM_FILENAME_CAP
nasm_include_name_len:.res 1
nasm_include_ptr:.res 3
nasm_include_len:.res 2
nasm_include_display_ptr:.res 2
nasm_error_name_ptr:.res 2
nasm_error_name_len:.res 1
.endif

      .segment "CODE"
      .export nasm_assemble
.ifdef NASM_CORE_OVERLAY
      .import nasbe_main
.endif

; Return A=0 on success, A=1 on error; nasm_error carries the reason.
nasm_assemble:
      STZ   nasm_error
      STZ   nasm_object_len
      STZ   nasm_object_len+1
      STZ   a_symbol_count
      JSR   a_init_sections
      LDA   #NASM_PASS_DEFINE
      STA   a_pass

@begin_pass:
      STZ   a_include_depth
      STZ   a_include_error
      STZ   a_scope_len
      STZ   a_anon_count
      LDA   nasm_source_name_ptr
      STA   a_file_ptr
      LDA   nasm_source_name_ptr+1
      STA   a_file_ptr+1
      LDA   nasm_source_name_len
      STA   a_file_len
      LDA   nasm_source_ptr
      STA   a_src
      LDA   nasm_source_ptr+1
      STA   a_src+1
      LDA   nasm_source_ptr+2
      STA   a_src+2
      LDA   XMC_WINCTL
      ORA   #$08
      STA   XMC_WINCTL
      LDA   a_src+1
      STA   WIN3_MI
      LDA   a_src+2
      STA   WIN3_HI
      LDA   nasm_source_len
      STA   a_left
      LDA   nasm_source_len+1
      STA   a_left+1
      LDA   #1
      STA   nasm_error_line
      STA   nasm_error_column
      STZ   nasm_error_line+1
      STZ   nasm_error_column+1
      JSR   a_reset_section_positions
      LDA   a_pass
      CMP   #NASM_PASS_RESOLVE
      BNE   :+
      STZ   a_resolve_changed
      STZ   a_resolve_pending
:
      LDA   a_pass
      CMP   #NASM_PASS_EMIT
      BNE   @statement
      JSR   a_begin_output
      long_bcs a_fail_output

@statement:
      JSR   a_skip_layout
      long_bcc @pass_done
      JSR   a_peek
      CMP   #':'
      long_beq @anonymous_label
      CMP   #'.'
      long_beq @directive
      JSR   a_parse_identifier
      long_bcs a_fail_syntax
      JSR   a_peek
      long_bcc @dispatch
      CMP   #':'
      BEQ   @label
      JSR   a_skip_hspace
      JSR   a_peek
      long_bcc @dispatch
      CMP   #'='
      long_beq @assignment
      JMP   @dispatch
@anonymous_label:
      JSR   a_next
      INC   a_anon_count
      long_beq a_fail_symbol
      LDA   a_anon_count
      JSR   a_make_anon_symbol
      LDA   a_pass
      CMP   #NASM_PASS_DEFINE
      BNE   @statement
      JSR   a_define_symbol
      long_bcs a_fail_symbol
      JMP   @statement
@label:
      JSR   a_next
      LDA   a_symbol_local
      BNE   :+
      JSR   a_set_scope
:
      LDA   a_pass
      CMP   #NASM_PASS_DEFINE
      BNE   @statement
      JSR   a_define_symbol
      long_bcs a_fail_symbol
      BRA   @statement
@assignment:
      JSR   a_save_mnemonic
      JSR   a_next
      LDA   #1
      STA   a_assignment_expr
      JSR   a_parse_expression
      long_bcs a_fail_syntax
      STZ   a_assignment_expr
      LDA   a_operand_symbol
      BEQ   @assignment_value
      JSR   a_restore_mnemonic
      LDA   a_pass
      CMP   #NASM_PASS_EMIT
      long_beq a_fail_symbol
      JSR   a_declare_pending_absolute
      long_bcs a_fail_symbol
      INC   a_resolve_pending
      JMP   @statement
@assignment_value:
      JSR   a_restore_mnemonic
      JSR   a_define_absolute
      long_bcs a_fail_symbol
      JMP   @statement

@directive:
      JSR   a_next
      JSR   a_parse_identifier
      long_bcs a_fail_syntax
      LDA   #<a_kw_byte
      LDX   #>a_kw_byte
      JSR   a_symbol_is
      long_bcs @byte
      LDA   #<a_kw_word
      LDX   #>a_kw_word
      JSR   a_symbol_is
      long_bcs @word
      LDA   #<a_kw_import
      LDX   #>a_kw_import
      JSR   a_symbol_is
      long_bcs @import
      LDA   #<a_kw_export
      LDX   #>a_kw_export
      JSR   a_symbol_is
      long_bcs @export
      LDA   #<a_kw_res
      LDX   #>a_kw_res
      JSR   a_symbol_is
      long_bcs @reserve
      LDA   #<a_kw_align
      LDX   #>a_kw_align
      JSR   a_symbol_is
      long_bcs @align
      LDA   #<a_kw_segment
      LDX   #>a_kw_segment
      JSR   a_symbol_is
      long_bcs @segment
      LDA   #<a_kw_include
      LDX   #>a_kw_include
      JSR   a_symbol_is
      long_bcs @include
      LDA   #<a_kw_incbin
      LDX   #>a_kw_incbin
      JSR   a_symbol_is
      long_bcs @incbin
      LDA   #<a_kw_assert
      LDX   #>a_kw_assert
      JSR   a_symbol_is
      long_bcs @assert
      JMP   a_fail_syntax

@import:
      LDA   #NASM_DECL_IMPORT
      JSR   a_declare_symbols
      long_bcs a_fail_symbol
      JMP   @statement
@export:
      LDA   #NASM_DECL_EXPORT
      JSR   a_declare_symbols
      long_bcs a_fail_symbol
      JMP   @statement
@reserve:
      JSR   a_parse_count_fill
      long_bcs a_fail_syntax
      JSR   a_emit_repeat
      long_bcs a_fail_output
      JMP   @statement
@align:
      JSR   a_parse_count_fill
      long_bcs a_fail_syntax
      JSR   a_prepare_alignment
      long_bcs a_fail_range
      JSR   a_emit_alignment
      long_bcs a_fail_output
      JMP   @statement
@segment:
      JSR   a_select_section
      long_bcs a_fail_symbol
      JMP   @statement
@include:
      JSR   a_parse_include_name
      long_bcs a_fail_syntax
      JSR   a_enter_include
      long_bcs a_fail_include
      JMP   @statement
@incbin:
      JSR   a_parse_include_name
      long_bcs a_fail_syntax
      JSR   a_emit_binary_file
      CMP   #1
      long_beq a_fail_include
      CMP   #2
      long_beq a_fail_output
      JMP   @statement
@assert:
      JSR   a_parse_expression
      long_bcs a_fail_syntax
      LDA   a_pass
      CMP   #NASM_PASS_EMIT
      long_bne @statement
      LDA   a_operand_symbol
      long_bne a_fail_symbol
      LDA   a_tmp0
      ORA   a_tmp1
      long_beq a_fail_assert
      JMP   @statement

@dispatch:
      JSR   a_save_mnemonic
      JSR   a_skip_hspace
      JSR   a_peek
      long_bcc a_fail_syntax
      CMP   #'#'
      BNE   @dispatch_named
      JSR   a_immediate_opcode
      long_bcc a_fail_syntax
      STA   a_opcode
      JSR   a_next
      JSR   a_parse_expression
      long_bcs a_fail_syntax
      LDA   a_operand_symbol
      BNE   @immediate_symbol
      LDA   a_tmp1
      long_bne a_fail_range
      LDA   a_opcode
      JSR   a_emit
      long_bcs a_fail_output
      LDA   a_tmp0
      JSR   a_emit
      long_bcs a_fail_output
      JMP   @statement
@immediate_symbol:
      LDA   a_opcode
      JSR   a_emit
      long_bcs a_fail_output
      LDA   a_pass
      CMP   #NASM_PASS_EMIT
      BNE   @immediate_zero
      JSR   a_byte_reloc_type
      LDX   #0
      JSR   a_add_relocation
      long_bcs a_fail_output
@immediate_zero:
      LDA   #0
      JSR   a_emit
      long_bcs a_fail_output
      JMP   @statement

@dispatch_named:
      JSR   a_peek_upper
      long_bcc a_fail_syntax
      CMP   #'A'
      BNE   @dispatch_special
      JSR   a_accumulator_opcode
      BCC   @dispatch_special
      STA   a_opcode
      JSR   a_next
      JSR   a_peek_upper
      BCC   @emit_accumulator
      JSR   a_is_ident
      long_bcs a_fail_syntax
@emit_accumulator:
      LDA   a_opcode
      JSR   a_emit
      long_bcs a_fail_output
      JMP   @statement

@dispatch_special:
      JSR   a_bit_opcode
      long_bcs @bit
      JSR   a_implied_opcode
      long_bcs @implied
      JSR   a_branch_opcode
      long_bcs @branch
      JMP   @direct

@direct:
      STZ   a_operand_symbol
      STZ   a_operand_index
      JSR   a_skip_hspace
      JSR   a_peek
      long_bcc a_fail_syntax
      CMP   #'('
      long_beq @indirect
      JSR   a_parse_expression
      long_bcs a_fail_syntax
@direct_suffix:
      JSR   a_skip_hspace
      JSR   a_peek
      BCC   @direct_select
      CMP   #','
      BNE   @direct_select
      JSR   a_next
      JSR   a_skip_hspace
      JSR   a_read_upper
      long_bcc a_fail_syntax
      CMP   #'X'
      BEQ   @direct_x
      CMP   #'Y'
      long_bne a_fail_syntax
      LDA   #2
      BRA   @direct_index
@direct_x:
      LDA   #1
@direct_index:
      STA   a_operand_index

@direct_select:
      LDA   a_operand_symbol
      BEQ   @direct_value
      LDA   a_operand_reloc
      BEQ   @direct_absolute
      BRA   @direct_byte
@direct_value:
      LDA   a_tmp1
      BNE   @direct_absolute
@direct_byte:
      LDA   a_operand_index
      BEQ   @lookup_zp
      CMP   #1
      BEQ   @lookup_zpx
      LDA   #NASBE_ZPY
      BRA   @lookup_zp_mode
@lookup_zpx:
      LDA   #NASBE_ZPX
      BRA   @lookup_zp_mode
@lookup_zp:
      LDA   #NASBE_ZP
@lookup_zp_mode:
      JSR   a_backend_opcode
      BCS   @direct_zp_found
      LDA   a_operand_symbol
      long_bne a_fail_syntax
      JMP   @direct_absolute       ; numeric operand can use an absolute form
@direct_zp_found:
      STA   a_opcode
      LDA   #1
      STA   a_operand_bytes
      BRA   @direct_emit

@direct_absolute:
      LDA   a_operand_index
      BEQ   @lookup_abs
      CMP   #1
      BEQ   @lookup_absx
      LDA   #NASBE_ABSY
      BRA   @lookup_abs_mode
@lookup_absx:
      LDA   #NASBE_ABSX
      BRA   @lookup_abs_mode
@lookup_abs:
      LDA   #NASBE_ABS
@lookup_abs_mode:
      JSR   a_backend_opcode
      long_bcc a_fail_syntax
      STA   a_opcode
      LDA   #2
      STA   a_operand_bytes

@direct_emit:
      LDA   a_opcode
      JSR   a_emit
      long_bcs a_fail_output
      LDA   a_operand_symbol
      BEQ   @direct_numeric
      LDA   a_pass
      CMP   #NASM_PASS_EMIT
      BNE   @direct_zero
      LDA   a_operand_bytes
      CMP   #1
      BNE   @direct_abs16
      JSR   a_byte_reloc_type
      BRA   @direct_reloc
@direct_abs16:
      LDA   #NOBJ_RELOC_ABS16
@direct_reloc:
      LDX   #0
      JSR   a_add_relocation
      long_bcs a_fail_output
@direct_zero:
      LDA   #0
      JSR   a_emit
      long_bcs a_fail_output
      LDA   a_operand_bytes
      CMP   #2
      BNE   @direct_done
      LDA   #0
      JSR   a_emit
      long_bcs a_fail_output
      JMP   @statement
@direct_numeric:
      LDA   a_tmp0
      JSR   a_emit
      long_bcs a_fail_output
      LDA   a_operand_bytes
      CMP   #2
      BNE   @direct_done
      LDA   a_tmp1
      JSR   a_emit
      long_bcs a_fail_output
@direct_done:
      JMP   @statement

@indirect:
      STZ   a_operand_symbol
      STZ   a_operand_index
      JSR   a_next                 ; '('
      JSR   a_skip_hspace
      JSR   a_peek
      long_bcc a_fail_syntax
      JSR   a_parse_expression
      long_bcs a_fail_syntax
@indirect_close:
      JSR   a_skip_hspace
      JSR   a_peek
      long_bcc a_fail_syntax
      CMP   #','
      BEQ   @indirect_pre_x
      CMP   #')'
      long_bne a_fail_syntax
      JSR   a_next
      JSR   a_skip_hspace
      JSR   a_peek
      BCC   @indirect_select
      CMP   #','
      BNE   @indirect_select
      JSR   a_next
      JSR   a_skip_hspace
      JSR   a_read_upper
      long_bcc a_fail_syntax
      CMP   #'Y'
      long_bne a_fail_syntax
      LDA   #2                    ; (operand),Y
      STA   a_operand_index
      BRA   @indirect_select
@indirect_pre_x:
      JSR   a_next
      JSR   a_skip_hspace
      JSR   a_read_upper
      long_bcc a_fail_syntax
      CMP   #'X'
      long_bne a_fail_syntax
      JSR   a_skip_hspace
      JSR   a_next
      long_bcc a_fail_syntax
      CMP   #')'
      long_bne a_fail_syntax
      LDA   #1                    ; (operand,X)
      STA   a_operand_index

@indirect_select:
      LDA   a_operand_symbol
      BNE   @indirect_absolute
      LDA   a_tmp1
      BNE   @indirect_absolute
      LDA   a_operand_index
      BEQ   @lookup_zpind
      CMP   #1
      BEQ   @lookup_indx
      LDA   #NASBE_INDY
      BRA   @lookup_indirect_zp
@lookup_indx:
      LDA   #NASBE_INDX
      BRA   @lookup_indirect_zp
@lookup_zpind:
      LDA   #NASBE_ZPIND
@lookup_indirect_zp:
      JSR   a_backend_opcode
      BCC   @indirect_absolute
      STA   a_opcode
      LDA   #1
      STA   a_operand_bytes
      JMP   @direct_emit

@indirect_absolute:
      LDA   a_operand_index
      CMP   #2
      long_beq a_fail_syntax
      CMP   #1
      BEQ   @lookup_absindx
      LDA   #NASBE_ABSIND
      BRA   @lookup_indirect_abs
@lookup_absindx:
      LDA   #NASBE_ABSINDX
@lookup_indirect_abs:
      JSR   a_backend_opcode
      long_bcc a_fail_syntax
      STA   a_opcode
      LDA   #2
      STA   a_operand_bytes
      JMP   @direct_emit

@bit:
      STA   a_opcode
      JSR   a_skip_hspace
      JSR   a_parse_expression
      long_bcs a_fail_syntax
      LDA   a_operand_symbol
      long_bne a_fail_symbol
      LDA   a_tmp1
      long_bne a_fail_range
      LDA   a_tmp0
      STA   a_operand_index       ; preserve the zero-page operand
      LDA   a_operand_bytes
      CMP   #1
      BEQ   @emit_bit
      JSR   a_skip_hspace
      JSR   a_next
      long_bcc a_fail_syntax
      CMP   #','
      long_bne a_fail_syntax
      JSR   a_skip_hspace
      JSR   a_parse_identifier
      long_bcs a_fail_syntax
      JSR   a_get_symbol
      long_bcs a_fail_symbol
      LDA   #3
      JSR   a_relative_delta
      long_bcs a_fail
@emit_bit:
      LDA   a_opcode
      JSR   a_emit
      long_bcs a_fail_output
      LDA   a_operand_index
      JSR   a_emit
      long_bcs a_fail_output
      LDA   a_operand_bytes
      CMP   #2
      BNE   @bit_done
      LDA   a_tmp0
      JSR   a_emit
      long_bcs a_fail_output
@bit_done:
      JMP   @statement

@implied:
      JSR   a_emit
      long_bcs a_fail_output
      JMP   @statement

@branch:
      STA   a_opcode
      JSR   a_skip_hspace
      JSR   a_peek
      long_bcc a_fail_syntax
      CMP   #':'
      BNE   @branch_named
      JSR   a_parse_anonymous
      long_bcs a_fail_syntax
      BRA   @branch_symbol
@branch_named:
      JSR   a_parse_identifier
      long_bcs a_fail_syntax
@branch_symbol:
      JSR   a_get_symbol
      long_bcs a_fail_symbol
      LDA   #2
      JSR   a_relative_delta
      long_bcs a_fail
      LDA   a_opcode
      JSR   a_emit
      long_bcs a_fail_output
      LDA   a_tmp0
      JSR   a_emit
      long_bcs a_fail_output
      JMP   @statement

@byte:
      JSR   a_skip_hspace
@byte_value:
      JSR   a_peek
      long_bcc a_fail_syntax
      CMP   #'"'
      BNE   @byte_expression
      JSR   a_emit_string
      BCC   @byte_next
      CMP   #2
      long_beq a_fail_output
      JMP   a_fail_syntax
@byte_expression:
      JSR   a_parse_expression
      long_bcs a_fail_syntax
      LDA   a_operand_symbol
      BEQ   @byte_number
      LDA   a_pass
      CMP   #NASM_PASS_EMIT
      BNE   @byte_zero
      JSR   a_byte_reloc_type
      LDX   #0
      JSR   a_add_relocation
      long_bcs a_fail_output
@byte_zero:
      LDA   #0
      BRA   @byte_emit
@byte_number:
      LDA   a_tmp1
      long_bne a_fail_range
      LDA   a_tmp0
@byte_emit:
      JSR   a_emit
      long_bcs a_fail_output
@byte_next:
      JSR   a_skip_hspace
      JSR   a_peek
      long_bcc @statement
      CMP   #','
      long_bne @statement
      JSR   a_next
      JSR   a_skip_hspace
      BRA   @byte_value

@word:
      JSR   a_skip_hspace
@word_value:
      JSR   a_parse_expression
      long_bcs a_fail_syntax
      LDA   a_operand_symbol
      BEQ   @word_number
      LDA   a_pass
      CMP   #NASM_PASS_EMIT
      BNE   @word_zero
      LDA   a_operand_reloc
      BNE   :+
      LDA   #NOBJ_RELOC_ABS16
:     LDX   #0
      JSR   a_add_relocation
      long_bcs a_fail_output
@word_zero:
      LDA   #0
      JSR   a_emit
      long_bcs a_fail_output
      LDA   #0
      JSR   a_emit
      long_bcs a_fail_output
      BRA   @word_next
@word_number:
      LDA   a_tmp0
      JSR   a_emit
      long_bcs a_fail_output
      LDA   a_tmp1
      JSR   a_emit
      long_bcs a_fail_output
@word_next:
      JSR   a_skip_hspace
      JSR   a_peek
      long_bcc @statement
      CMP   #','
      long_bne @statement
      JSR   a_next
      JSR   a_skip_hspace
      BRA   @word_value

@pass_done:
      LDA   a_include_error
      long_bne a_fail_include
      JSR   a_save_section_position
      LDA   a_pass
      CMP   #NASM_PASS_DEFINE
      BNE   @resolve_done
      JSR   a_save_section_sizes
      LDA   #NASM_PASS_RESOLVE
      STA   a_pass
      JMP   @begin_pass

@resolve_done:
      CMP   #NASM_PASS_RESOLVE
      BNE   @finish
      LDA   a_resolve_changed
      long_bne @begin_pass
      LDA   a_resolve_pending
      long_bne a_fail_symbol
      LDA   #NASM_PASS_EMIT
      STA   a_pass
      JMP   @begin_pass

@finish:
      JSR   a_check_section_sizes
      long_bcs a_fail_symbol
      JSR   a_validate_symbols
      long_bcs a_fail_symbol
      JSR   a_finish_object
      long_bcs a_fail_output
      LDA   #NASM_OK
      RTS

a_fail_syntax:
      LDA   #NASM_ERR_SYNTAX
      BRA   a_fail
a_fail_output:
      LDA   #NASM_ERR_OUTPUT
      BRA   a_fail
a_fail_symbol:
      LDA   #NASM_ERR_SYMBOL
      BRA   a_fail
a_fail_range:
      LDA   #NASM_ERR_RANGE
      BRA   a_fail
a_fail_include:
      LDA   #NASM_ERR_INCLUDE
      BRA   a_fail
a_fail_assert:
      LDA   #NASM_ERR_ASSERT
a_fail:
      PHA
      LDA   a_file_ptr
      STA   nasm_error_name_ptr
      LDA   a_file_ptr+1
      STA   nasm_error_name_ptr+1
      LDA   a_file_len
      STA   nasm_error_name_len
      PLA
      STA   nasm_error
      LDA   #1
      RTS

; Emit one double-quoted ASCII string. Common source escapes stay explicit;
; NAS performs no target character-map conversion.
a_emit_string:
      JSR   a_next                 ; opening quote
@char:
      JSR   a_next
      BCC   @syntax
      CMP   #'"'
      BEQ   @done
      CMP   #$0A
      BEQ   @syntax
      CMP   #$0D
      BEQ   @syntax
      CMP   #$5C                  ; backslash
      BNE   @emit
      JSR   a_next
      BCC   @syntax
      STA   a_expr_digit
      AND   #$DF
      CMP   #'N'
      BEQ   @newline
      CMP   #'R'
      BEQ   @return
      CMP   #'T'
      BEQ   @tab
      LDA   a_expr_digit
      CMP   #'0'
      BEQ   @zero
      CMP   #'"'
      BEQ   @emit
      CMP   #$5C
      BNE   @syntax
      BRA   @emit
@newline:
      LDA   #$0A
      BRA   @emit
@return:
      LDA   #$0D
      BRA   @emit
@tab:
      LDA   #$09
      BRA   @emit
@zero:
      LDA   #0
@emit:
      JSR   a_emit
      BCS   @output
      BRA   @char
@done:
      LDA   #0
      CLC
      RTS
@syntax:
      LDA   #1
      SEC
      RTS
@output:
      LDA   #2
      SEC
      RTS

; Anonymous labels use generated names that source identifiers cannot spell.
; A is the one-based definition ordinal for this pass.
a_make_anon_symbol:
      STA   a_anon_target
      LDA   #4
      STA   a_symbol_len
      LDA   #1
      STA   a_symbol_local
      LDA   #'_'
      STA   a_symbol+0
      LDA   #'@'
      STA   a_symbol+1
      LDA   a_anon_target
      LSR
      LSR
      LSR
      LSR
      JSR   a_encode_hex
      STA   a_symbol+2
      LDA   a_anon_target
      AND   #$0F
      JSR   a_encode_hex
      STA   a_symbol+3
      CLC
      RTS

a_encode_hex:
      CMP   #10
      BCC   @digit
      CLC
      ADC   #'A'-10
      RTS
@digit:
      CLC
      ADC   #'0'
      RTS

; Parse :+, :++, :-, :--, and longer runs into the generated symbol name.
a_parse_anonymous:
      JSR   a_next                 ; ':'
      JSR   a_next
      BCC   @bad
      CMP   #'+'
      BEQ   @forward
      CMP   #'-'
      BNE   @bad
      LDA   a_anon_count
      BEQ   @bad
      STA   a_anon_target
@back_more:
      JSR   a_peek
      BCC   @make
      CMP   #'-'
      BNE   @make
      JSR   a_next
      LDA   a_anon_target
      CMP   #2
      BCC   @bad
      DEC   a_anon_target
      BRA   @back_more
@forward:
      LDA   a_anon_count
      STA   a_anon_target
@forward_one:
      LDA   a_anon_target
      CMP   #$FF
      BEQ   @bad
      INC   a_anon_target
      JSR   a_peek
      BCC   @make
      CMP   #'+'
      BNE   @make
      JSR   a_next
      BRA   @forward_one
@make:
      LDA   a_anon_target
      JSR   a_make_anon_symbol
      CLC
      RTS
@bad:
      SEC
      RTS

a_include_open:
.ifdef NASM_CORE_OVERLAY
      JMP   (NASCORE_INCLUDE_OPEN)
.else
      JMP   nasm_include_open
.endif

a_include_close:
.ifdef NASM_CORE_OVERLAY
      JMP   (NASCORE_INCLUDE_CLOSE)
.else
      JMP   nasm_include_close
.endif

a_parse_include_name:
      JSR   a_skip_hspace
      JSR   a_next
      BCC   @bad
      CMP   #'"'
      BNE   @bad
      STZ   nasm_include_name_len
@char:
      JSR   a_next
      BCC   @bad
      CMP   #'"'
      BEQ   @done
      CMP   #$20
      BCC   @bad
      LDX   nasm_include_name_len
      CPX   #NASM_FILENAME_CAP-1
      BCS   @bad
      STA   nasm_include_name,X
      INC   nasm_include_name_len
      BRA   @char
@done:
      LDX   nasm_include_name_len
      BEQ   @bad
      STZ   nasm_include_name,X
      CLC
      RTS
@bad:
      SEC
      RTS

; Emit a raw file through the same frontend-owned XRAM callback as .INCLUDE.
; A=0 success, A=1 file/callback failure, A=2 section/output overflow.
a_emit_binary_file:
      JSR   a_include_open
      BEQ   @loaded
      JSR   a_restore_source_window
      LDA   #1
      RTS
@loaded:
      LDX   #2
@copy_ptr:
      LDA   nasm_include_ptr,X
      STA   a_binary_ptr,X
      DEX
      BPL   @copy_ptr
      LDA   nasm_include_len
      STA   a_binary_left
      LDA   nasm_include_len+1
      STA   a_binary_left+1
      LDA   a_binary_ptr+1
      STA   WIN3_MI
      LDA   a_binary_ptr+2
      STA   WIN3_HI
@byte:
      LDA   a_binary_left
      ORA   a_binary_left+1
      BEQ   @done
      LDY   a_binary_ptr
      LDA   WIN3_BASE,Y
      JSR   a_emit
      BCS   @output_bad
      INC   a_binary_ptr
      BNE   @left
      INC   a_binary_ptr+1
      LDA   a_binary_ptr+1
      STA   WIN3_MI
      BNE   @left
      INC   a_binary_ptr+2
      LDA   a_binary_ptr+2
      STA   WIN3_HI
@left:
      LDA   a_binary_left
      BNE   :+
      DEC   a_binary_left+1
:     DEC   a_binary_left
      BRA   @byte
@done:
      JSR   a_close_binary
      BEQ   @ok
      LDA   #1
      RTS
@ok:
      LDA   #0
      RTS
@output_bad:
      JSR   a_close_binary
      LDA   #2
      RTS

a_close_binary:
      JSR   a_include_close
      PHA
      JSR   a_restore_source_window
      PLA
      RTS

a_restore_source_window:
      LDA   a_src+1
      STA   WIN3_MI
      LDA   a_src+2
      STA   WIN3_HI
      RTS

; Save the current stream and switch to the frontend-loaded include source.
a_enter_include:
      LDX   a_include_depth
      CPX   #NASM_INCLUDE_DEPTH
      BCC   :+
      JMP   @open_bad
:
      JSR   a_include_open
      BEQ   :+
      JMP   @open_bad
:
      LDX   a_include_depth
      LDA   a_src
      STA   a_stack_src_l,X
      LDA   a_src+1
      STA   a_stack_src_m,X
      LDA   a_src+2
      STA   a_stack_src_h,X
      LDA   a_left
      STA   a_stack_left_l,X
      LDA   a_left+1
      STA   a_stack_left_h,X
      LDA   nasm_error_line
      STA   a_stack_line_l,X
      LDA   nasm_error_line+1
      STA   a_stack_line_h,X
      LDA   nasm_error_column
      STA   a_stack_col_l,X
      LDA   nasm_error_column+1
      STA   a_stack_col_h,X
      LDA   a_file_ptr
      STA   a_stack_file_l,X
      LDA   a_file_ptr+1
      STA   a_stack_file_h,X
      LDA   a_file_len
      STA   a_stack_file_len,X
      INC   a_include_depth
      LDA   nasm_include_ptr
      STA   a_src
      LDA   nasm_include_ptr+1
      STA   a_src+1
      STA   WIN3_MI
      LDA   nasm_include_ptr+2
      STA   a_src+2
      STA   WIN3_HI
      LDA   nasm_include_len
      STA   a_left
      LDA   nasm_include_len+1
      STA   a_left+1
      LDA   nasm_include_display_ptr
      STA   a_file_ptr
      LDA   nasm_include_display_ptr+1
      STA   a_file_ptr+1
      LDA   nasm_include_name_len
      STA   a_file_len
      LDA   #1
      STA   nasm_error_line
      STA   nasm_error_column
      STZ   nasm_error_line+1
      STZ   nasm_error_column+1
      CLC
      RTS
@open_bad:
      LDA   #<nasm_include_name
      STA   a_file_ptr
      LDA   #>nasm_include_name
      STA   a_file_ptr+1
      LDA   nasm_include_name_len
      STA   a_file_len
      SEC
      RTS

a_leave_include:
      JSR   a_include_close
      BNE   @bad
      DEC   a_include_depth
      LDX   a_include_depth
      LDA   a_stack_src_l,X
      STA   a_src
      LDA   a_stack_src_m,X
      STA   a_src+1
      STA   WIN3_MI
      LDA   a_stack_src_h,X
      STA   a_src+2
      STA   WIN3_HI
      LDA   a_stack_left_l,X
      STA   a_left
      LDA   a_stack_left_h,X
      STA   a_left+1
      LDA   a_stack_line_l,X
      STA   nasm_error_line
      LDA   a_stack_line_h,X
      STA   nasm_error_line+1
      LDA   a_stack_col_l,X
      STA   nasm_error_column
      LDA   a_stack_col_h,X
      STA   nasm_error_column+1
      LDA   a_stack_file_l,X
      STA   a_file_ptr
      LDA   a_stack_file_h,X
      STA   a_file_ptr+1
      LDA   a_stack_file_len,X
      STA   a_file_len
      CLC
      RTS
@bad:
      SEC
      RTS

; CODE is always section zero so a source without .SEGMENT remains compatible.
a_init_sections:
      LDX   #NASM_SECTION_CAP-1
      LDA   #0
@clear:
      STA   a_sec_name_len,X
      STA   a_sec_flags,X
      STA   a_sec_align,X
      STA   a_sec_pos_l,X
      STA   a_sec_pos_h,X
      STA   a_sec_size_l,X
      STA   a_sec_size_h,X
      DEX
      BPL   @clear
      LDA   #1
      STA   a_section_count
      STZ   a_section
      LDA   #4
      STA   a_sec_name_len
      LDA   #NOBJ_SEC_ALLOC | NOBJ_SEC_EXEC
      STA   a_sec_flags
      LDA   #'C'
      STA   a_sec_names
      LDA   #'O'
      STA   a_sec_names+1
      LDA   #'D'
      STA   a_sec_names+2
      LDA   #'E'
      STA   a_sec_names+3
      RTS

a_reset_section_positions:
      LDX   #NASM_SECTION_CAP-1
      LDA   #0
@clear:
      STA   a_sec_pos_l,X
      STA   a_sec_pos_h,X
      DEX
      BPL   @clear
      STZ   a_section
      STZ   a_code_len
      STZ   a_code_len+1
      RTS

a_save_section_position:
      LDX   a_section
      LDA   a_code_len
      STA   a_sec_pos_l,X
      LDA   a_code_len+1
      STA   a_sec_pos_h,X
      RTS

a_save_section_sizes:
      LDX   #0
@copy:
      CPX   a_section_count
      BCS   @done
      LDA   a_sec_pos_l,X
      STA   a_sec_size_l,X
      LDA   a_sec_pos_h,X
      STA   a_sec_size_h,X
      INX
      BRA   @copy
@done:
      RTS

a_check_section_sizes:
      LDX   #0
@check:
      CPX   a_section_count
      BCS   @ok
      LDA   a_sec_pos_l,X
      CMP   a_sec_size_l,X
      BNE   @bad
      LDA   a_sec_pos_h,X
      CMP   a_sec_size_h,X
      BNE   @bad
      INX
      BRA   @check
@ok:
      CLC
      RTS
@bad:
      SEC
      RTS

; Parse and select a case-insensitive .SEGMENT "name".
a_select_section:
      JSR   a_parse_section_name
      BCS   @bad
      JSR   a_find_section
      BCS   @switch
      LDA   a_pass
      CMP   #NASM_PASS_DEFINE
      BNE   @bad
      LDX   a_section_count
      CPX   #NASM_SECTION_CAP
      BCS   @bad
      STX   a_section_iter
      LDA   a_symbol_len
      STA   a_sec_name_len,X
      JSR   a_section_name_ptr
      LDY   #0
@copy:
      CPY   a_symbol_len
      BCS   @flags
      LDA   a_symbol,Y
      STA   (a_symbol_ptr),Y
      INY
      BRA   @copy
@flags:
      LDA   #NOBJ_SEC_ALLOC
      LDX   a_section_iter
      STA   a_sec_flags,X
      LDA   #<a_kw_code
      LDX   #>a_kw_code
      JSR   a_symbol_is
      BCC   @data
      LDA   #NOBJ_SEC_ALLOC | NOBJ_SEC_EXEC
      BRA   @store_flags
@data:
      LDA   #<a_kw_data
      LDX   #>a_kw_data
      JSR   a_symbol_is
      BCC   @bss
      LDA   #NOBJ_SEC_ALLOC | NOBJ_SEC_WRITE
      BRA   @store_flags
@bss:
      LDA   #<a_kw_bss
      LDX   #>a_kw_bss
      JSR   a_symbol_is
      BCC   @created
      LDA   #NOBJ_SEC_ALLOC | NOBJ_SEC_WRITE | NOBJ_SEC_ZEROFILL
@store_flags:
      LDX   a_section_iter
      STA   a_sec_flags,X
@created:
      INC   a_section_count
      LDX   a_section_iter
@switch:
      STX   a_section_iter
      JSR   a_save_section_position
      LDX   a_section_iter
      STX   a_section
      LDA   a_sec_pos_l,X
      STA   a_code_len
      LDA   a_sec_pos_h,X
      STA   a_code_len+1
      CLC
      RTS
@bad:
      SEC
      RTS

a_parse_section_name:
      JSR   a_skip_hspace
      JSR   a_next
      BCC   @bad
      CMP   #'"'
      BNE   @bad
      STZ   a_symbol_len
@char:
      JSR   a_read_upper
      BCC   @bad
      CMP   #'"'
      BEQ   @done
      CMP   #$20
      BCC   @bad
      LDX   a_symbol_len
      CPX   #NASM_SECTION_NAME_CAP
      BCS   @bad
      STA   a_symbol,X
      INC   a_symbol_len
      BRA   @char
@done:
      LDA   a_symbol_len
      BEQ   @bad
      CLC
      RTS
@bad:
      SEC
      RTS

a_find_section:
      LDX   #0
@entry:
      CPX   a_section_count
      BCS   @missing
      LDA   a_sec_name_len,X
      CMP   a_symbol_len
      BNE   @next
      STX   a_section_iter
      JSR   a_section_name_ptr
      LDY   #0
@name:
      CPY   a_symbol_len
      BCS   @found
      LDA   (a_symbol_ptr),Y
      CMP   a_symbol,Y
      BNE   @next_saved
      INY
      BRA   @name
@next_saved:
      LDX   a_section_iter
@next:
      INX
      BRA   @entry
@found:
      LDX   a_section_iter
      SEC
      RTS
@missing:
      CLC
      RTS

; X = section index -> fixed-size section-name slot.
a_section_name_ptr:
      STX   a_section_iter
      TXA
      ASL
      ASL
      ASL
      ASL
      CLC
      ADC   #<a_sec_names
      STA   a_symbol_ptr
      LDA   #>a_sec_names
      ADC   #0
      STA   a_symbol_ptr+1
      LDX   a_section_iter
      RTS

; A is instruction size; resolve current symbol to a signed byte in a_tmp0.
; On failure carry is set and A is the public NASM error code.
a_relative_delta:
      STA   a_relative_size
      STZ   a_tmp1
      LDA   a_pass
      CMP   #NASM_PASS_EMIT
      BNE   @define
      LDX   a_symbol_index
      LDA   a_sym_section,X
      CMP   a_section
      BNE   @symbol
      CLC
      LDA   a_code_len
      ADC   a_relative_size
      STA   a_tmp0
      LDA   a_code_len+1
      ADC   #0
      STA   a_tmp1
      SEC
      LDA   a_sym_value_l,X
      SBC   a_tmp0
      STA   a_tmp0
      LDA   a_sym_value_h,X
      SBC   a_tmp1
      STA   a_tmp1
      BEQ   @positive
      CMP   #$FF
      BNE   @range
      LDA   a_tmp0
      CMP   #$80
      BCC   @range
      CLC
      RTS
@positive:
      LDA   a_tmp0
      CMP   #$80
      BCS   @range
      CLC
      RTS
@define:
      STZ   a_tmp0
      CLC
      RTS
@symbol:
      STZ   a_tmp0
      STZ   a_tmp1
      LDX   a_relative_size
      DEX
      LDA   #NOBJ_RELOC_REL8
      JSR   a_add_relocation
      BCC   @define
      LDA   #NASM_ERR_OUTPUT
      SEC
      RTS
@range:
      LDA   #NASM_ERR_RANGE
      SEC
      RTS

; Initialize the v2 header and reserve every section's serialized data area.
a_begin_output:
      LDA   nasm_object_ptr
      STA   a_header
      STA   a_dst
      LDA   nasm_object_ptr+1
      STA   a_header+1
      STA   a_dst+1
      LDA   nasm_object_cap
      STA   a_cap
      LDA   nasm_object_cap+1
      STA   a_cap+1
      LDA   a_cap+1
      BNE   @fits
      LDA   a_cap
      CMP   #NOBJ_HEADER_SIZE
      long_bcc @bad
@fits:
      LDY   #0
      LDA   #0
@clear_header:
      STA   (a_header),Y
      INY
      CPY   #NOBJ_HEADER_SIZE
      BCC   @clear_header
      LDY   #NOBJ_MAGIC
      LDA   #NOBJ_MAGIC0
      STA   (a_header),Y
      INY
      LDA   #NOBJ_MAGIC1
      STA   (a_header),Y
      INY
      LDA   #NOBJ_MAGIC2
      STA   (a_header),Y
      INY
      LDA   #NOBJ_MAGIC3
      STA   (a_header),Y
      LDY   #NOBJ_VERSION_OFF
      LDA   #NOBJ_VERSION
      STA   (a_header),Y
      LDY   #NOBJ_SECTION_COUNT
      LDA   a_section_count
      STA   (a_header),Y
      INY
      LDA   #0
      STA   (a_header),Y          ; entry section = CODE
      CLC
      LDA   a_dst
      ADC   #NOBJ_HEADER_SIZE
      STA   a_dst
      BCC   :+
      INC   a_dst+1
:     SEC
      LDA   a_cap
      SBC   #NOBJ_HEADER_SIZE
      STA   a_cap
      LDA   a_cap+1
      SBC   #0
      STA   a_cap+1

      LDX   #0
@section:
      CPX   a_section_count
      long_bcs @sections_done
      STX   a_section_iter
      LDA   a_sec_flags,X
      JSR   a_emit_tail
      long_bcs @bad
      LDX   a_section_iter
      LDA   a_sec_align,X
      JSR   a_emit_tail
      long_bcs @bad
      LDX   a_section_iter
      LDA   a_sec_name_len,X
      JSR   a_emit_tail
      long_bcs @bad
      LDA   #0
      JSR   a_emit_tail
      long_bcs @bad
      LDX   a_section_iter
      LDA   a_sec_size_l,X
      JSR   a_emit_tail
      long_bcs @bad
      LDX   a_section_iter
      LDA   a_sec_size_h,X
      JSR   a_emit_tail
      long_bcs @bad
      LDX   a_section_iter
      LDA   a_sec_flags,X
      AND   #NOBJ_SEC_ZEROFILL
      BNE   @zero_data_len
      LDA   a_sec_size_l,X
      JSR   a_emit_tail
      long_bcs @bad
      LDX   a_section_iter
      LDA   a_sec_size_h,X
      BRA   @data_len_high
@zero_data_len:
      LDA   #0
      JSR   a_emit_tail
      long_bcs @bad
      LDA   #0
@data_len_high:
      JSR   a_emit_tail
      long_bcs @bad

      LDX   a_section_iter
      JSR   a_section_name_ptr
      LDY   #0
@name:
      LDX   a_section_iter
      TYA
      CMP   a_sec_name_len,X
      BCS   @name_done
      LDA   (a_symbol_ptr),Y
      JSR   a_emit_tail
      long_bcs @bad
      INY
      BRA   @name
@name_done:
      LDX   a_section_iter
      LDA   a_dst
      STA   a_sec_data_l,X
      LDA   a_dst+1
      STA   a_sec_data_h,X
      LDA   a_sec_flags,X
      AND   #NOBJ_SEC_ZEROFILL
      BNE   @next_section
      LDA   a_sec_size_l,X
      STA   a_tmp0
      LDA   a_sec_size_h,X
      STA   a_tmp1
      JSR   a_reserve_output
      long_bcs @bad
@next_section:
      LDX   a_section_iter
      INX
      JMP   @section
@sections_done:
      STZ   a_reloc_len
      STZ   a_reloc_len+1
      STZ   a_reloc_count
      STZ   a_reloc_count+1
      LDA   #<a_reloc_buf
      STA   a_reloc_ptr
      LDA   #>a_reloc_buf
      STA   a_reloc_ptr+1
      CLC
      RTS
@bad:
      SEC
      RTS

; Reserve a_tmp1:a_tmp0 bytes at the object tail without writing them.
a_reserve_output:
      LDA   a_cap+1
      CMP   a_tmp1
      BCC   @bad
      BNE   @room
      LDA   a_cap
      CMP   a_tmp0
      BCC   @bad
@room:
      SEC
      LDA   a_cap
      SBC   a_tmp0
      STA   a_cap
      LDA   a_cap+1
      SBC   a_tmp1
      STA   a_cap+1
      CLC
      LDA   a_dst
      ADC   a_tmp0
      STA   a_dst
      LDA   a_dst+1
      ADC   a_tmp1
      STA   a_dst+1
      CLC
      RTS
@bad:
      SEC
      RTS

; Append symbol and relocation tables, then publish their header offsets.
a_finish_object:
      SEC
      LDA   a_dst
      SBC   a_header
      STA   a_tmp0
      LDA   a_dst+1
      SBC   a_header+1
      STA   a_tmp1
      LDY   #NOBJ_SYMBOL_OFFSET
      LDA   a_tmp0
      STA   (a_header),Y
      INY
      LDA   a_tmp1
      STA   (a_header),Y
      LDY   #NOBJ_SYMBOL_COUNT
      LDA   a_symbol_count
      STA   (a_header),Y
      INY
      LDA   #0
      STA   (a_header),Y
      LDX   #0
@symbol:
      CPX   a_symbol_count
      BCS   @symbols_done
      STX   a_symbol_index
      LDA   a_sym_value_l,X
      JSR   a_emit_tail
      long_bcs @bad
      LDX   a_symbol_index
      LDA   a_sym_value_h,X
      JSR   a_emit_tail
      long_bcs @bad
      LDX   a_symbol_index
      LDA   a_sym_section,X
      JSR   a_emit_tail
      long_bcs @bad
      LDX   a_symbol_index
      LDA   a_sym_flags,X
      JSR   a_emit_tail
      long_bcs @bad
      LDX   a_symbol_index
      LDA   a_sym_name_len,X
      JSR   a_emit_tail
      long_bcs @bad
      LDX   a_symbol_index
      JSR   a_symbol_name_ptr
      LDY   #0
@symbol_name:
      LDX   a_symbol_index
      TYA
      CMP   a_sym_name_len,X
      BCS   @next_symbol
      LDA   (a_symbol_ptr),Y
      JSR   a_emit_tail
      BCS   @bad
      INY
      BRA   @symbol_name
@next_symbol:
      LDX   a_symbol_index
      INX
      BRA   @symbol
@symbols_done:
      SEC
      LDA   a_dst
      SBC   a_header
      STA   a_tmp0
      LDA   a_dst+1
      SBC   a_header+1
      STA   a_tmp1
      LDY   #NOBJ_RELOC_OFFSET
      LDA   a_tmp0
      STA   (a_header),Y
      INY
      LDA   a_tmp1
      STA   (a_header),Y
      LDY   #NOBJ_RELOC_COUNT
      LDA   a_reloc_count
      STA   (a_header),Y
      INY
      LDA   a_reloc_count+1
      STA   (a_header),Y
      LDA   #<a_reloc_buf
      STA   a_reloc_ptr
      LDA   #>a_reloc_buf
      STA   a_reloc_ptr+1
@copy_reloc:
      LDA   a_reloc_len
      ORA   a_reloc_len+1
      BEQ   @length
      LDY   #0
      LDA   (a_reloc_ptr),Y
      JSR   a_emit_tail
      BCS   @bad
      INC   a_reloc_ptr
      BNE   :+
      INC   a_reloc_ptr+1
:     LDA   a_reloc_len
      BNE   :+
      DEC   a_reloc_len+1
:     DEC   a_reloc_len
      BRA   @copy_reloc
@length:
      SEC
      LDA   a_dst
      SBC   a_header
      STA   nasm_object_len
      LDA   a_dst+1
      SBC   a_header+1
      STA   nasm_object_len+1
      CLC
      RTS
@bad:
      SEC
      RTS

; Skip whitespace and comments. Carry set means a statement remains.
a_skip_layout:
@again:
      JSR   a_peek
      BCC   @eof
      CMP   #' '
      BEQ   @eat
      CMP   #$09
      BEQ   @eat
      CMP   #$0D
      BEQ   @eat
      CMP   #$0A
      BEQ   @eat
      CMP   #';'
      BEQ   @comment
      SEC
      RTS
@eat:
      JSR   a_next
      BRA   @again
@comment:
      JSR   a_next
@comment_loop:
      JSR   a_next
      BCC   @eof
      CMP   #$0A
      BNE   @comment_loop
      BRA   @again
@eof:
      CLC
      RTS

a_skip_hspace:
@loop:
      JSR   a_peek
      BCC   @done
      CMP   #' '
      BEQ   @eat
      CMP   #$09
      BNE   @done
@eat:
      JSR   a_next
      BRA   @loop
@done:
      RTS

a_parse_identifier:
      STZ   a_symbol_len
      STZ   a_symbol_local
      JSR   a_peek_upper
      BCC   @bad
      CMP   #'@'
      BNE   @global
      LDA   a_scope_len
      BEQ   @bad
      INC   a_symbol_local
      JSR   a_read_upper
      STA   a_symbol
      INC   a_symbol_len
      JSR   a_peek_upper
      BCC   @bad
@global:
      JSR   a_is_ident_start
      BCC   @bad
@loop:
      LDA   a_symbol_len
      CMP   #NASM_SYMBOL_CAP
      BCS   @bad
      JSR   a_read_upper
      LDX   a_symbol_len
      STA   a_symbol,X
      INC   a_symbol_len
      JSR   a_peek_upper
      BCC   @ok
      JSR   a_is_ident
      BCS   @loop
@ok:
      LDA   a_symbol_local
      BEQ   @done
      JSR   a_qualify_local
      BCS   @bad
@done:
      CLC
      RTS
@bad:
      SEC
      RTS

; Cheap local labels are stored as GLOBAL@LOCAL. The source-facing `@` name
; therefore remains reusable while the ordinary symbol table stays unchanged.
a_qualify_local:
      CLC
      LDA   a_scope_len
      ADC   a_symbol_len
      CMP   #NASM_SYMBOL_CAP+1
      BCS   @bad
      STA   a_local_total
      LDX   a_symbol_len
@move:
      DEX
      TXA
      CLC
      ADC   a_scope_len
      TAY
      LDA   a_symbol,X
      STA   a_symbol,Y
      CPX   #0
      BNE   @move
      LDY   #0
@prefix:
      CPY   a_scope_len
      BCS   @done
      LDA   a_scope,Y
      STA   a_symbol,Y
      INY
      BRA   @prefix
@done:
      LDA   a_local_total
      STA   a_symbol_len
      CLC
      RTS
@bad:
      SEC
      RTS

a_set_scope:
      LDA   a_symbol_len
      STA   a_scope_len
      LDY   #0
@copy:
      CPY   a_symbol_len
      BCS   @done
      LDA   a_symbol,Y
      STA   a_scope,Y
      INY
      BRA   @copy
@done:
      RTS

; A/X points to a NUL-terminated uppercase word. Carry set means equal.
a_symbol_is:
      STA   a_word
      STX   a_word+1
      LDY   #0
@compare:
      CPY   a_symbol_len
      BCS   @end
      LDA   (a_word),Y
      BEQ   @no
      CMP   a_symbol,Y
      BNE   @no
      INY
      BRA   @compare
@end:
      LDA   (a_word),Y
      BNE   @no
      SEC
      RTS
@no:
      CLC
      RTS

a_save_mnemonic:
      LDA   a_symbol_len
      STA   a_mnemonic_len
      LDY   #0
@copy:
      CPY   a_symbol_len
      BCS   @done
      LDA   a_symbol,Y
      STA   a_mnemonic,Y
      INY
      BRA   @copy
@done:
      RTS

a_restore_mnemonic:
      LDA   a_mnemonic_len
      STA   a_symbol_len
      LDY   #0
@copy:
      CPY   a_mnemonic_len
      BCS   @done
      LDA   a_mnemonic,Y
      STA   a_symbol,Y
      INY
      BRA   @copy
@done:
      RTS

; Recognize RMB0-7, SMB0-7, BBR0-7, and BBS0-7.
; Carry set returns A=opcode and a_operand_bytes=1 (bit op) or 2 (bit branch).
a_bit_opcode:
      LDA   a_mnemonic_len
      CMP   #4
      BNE   @no
      LDA   a_mnemonic+3
      SEC
      SBC   #'0'
      BCC   @no
      CMP   #8
      BCS   @no
      ASL
      ASL
      ASL
      ASL
      STA   a_opcode
      LDA   a_mnemonic
      CMP   #'R'
      BEQ   @rmb
      CMP   #'S'
      BEQ   @smb
      CMP   #'B'
      BNE   @no
      LDA   a_mnemonic+1
      CMP   #'B'
      BNE   @no
      LDA   a_mnemonic+2
      CMP   #'R'
      BEQ   @bbr
      CMP   #'S'
      BNE   @no
      LDA   #$8F
      BRA   @branch
@bbr:
      LDA   #$0F
@branch:
      LDX   #2
      BRA   @found
@rmb:
      LDA   a_mnemonic+1
      CMP   #'M'
      BNE   @no
      LDA   a_mnemonic+2
      CMP   #'B'
      BNE   @no
      LDA   #$07
      BRA   @memory
@smb:
      LDA   a_mnemonic+1
      CMP   #'M'
      BNE   @no
      LDA   a_mnemonic+2
      CMP   #'B'
      BNE   @no
      LDA   #$87
@memory:
      LDX   #1
@found:
      STX   a_operand_bytes
      CLC
      ADC   a_opcode
      SEC
      RTS
@no:
      CLC
      RTS

; Return an implied opcode in A with carry set, or carry clear if not found.
a_implied_opcode:
      LDA   #NASBE_IMPLIED
      BRA   a_backend_opcode

; Return an immediate opcode in A with carry set, or carry clear if not found.
a_immediate_opcode:
      LDA   #NASBE_IMMEDIATE
      BRA   a_backend_opcode

; Return an accumulator opcode in A with carry set, or carry clear if absent.
a_accumulator_opcode:
      LDA   #NASBE_ACCUMULATOR

; A selects an overlay opcode table. Carry set returns the opcode in A.
a_backend_opcode:
      STA   NASBE_MODE
      LDA   a_mnemonic_len
      CMP   #3
      BNE   @no
      LDA   a_mnemonic
      STA   NASBE_MNEMONIC
      LDA   a_mnemonic+1
      STA   NASBE_MNEMONIC+1
      LDA   a_mnemonic+2
      STA   NASBE_MNEMONIC+2
.ifdef NASM_CORE_OVERLAY
      JSR   nasbe_main
      RTS
.else
      JMP   NASBE_ENTRY
.endif
@no:
      CLC
      RTS

; Return branch opcode in A with carry set, or carry clear if not a branch.
a_branch_opcode:
      LDA   #<a_kw_bcc
      LDX   #>a_kw_bcc
      JSR   a_symbol_is
      BCC   :+
      LDA   #$90
      SEC
      RTS
:     LDA   #<a_kw_bcs
      LDX   #>a_kw_bcs
      JSR   a_symbol_is
      BCC   :+
      LDA   #$B0
      SEC
      RTS
:     LDA   #<a_kw_beq
      LDX   #>a_kw_beq
      JSR   a_symbol_is
      BCC   :+
      LDA   #$F0
      SEC
      RTS
:     LDA   #<a_kw_bmi
      LDX   #>a_kw_bmi
      JSR   a_symbol_is
      BCC   :+
      LDA   #$30
      SEC
      RTS
:     LDA   #<a_kw_bne
      LDX   #>a_kw_bne
      JSR   a_symbol_is
      BCC   :+
      LDA   #$D0
      SEC
      RTS
:     LDA   #<a_kw_bpl
      LDX   #>a_kw_bpl
      JSR   a_symbol_is
      BCC   :+
      LDA   #$10
      SEC
      RTS
:     LDA   #<a_kw_bra
      LDX   #>a_kw_bra
      JSR   a_symbol_is
      BCC   :+
      LDA   #$80
      SEC
      RTS
:     LDA   #<a_kw_bvc
      LDX   #>a_kw_bvc
      JSR   a_symbol_is
      BCC   :+
      LDA   #$50
      SEC
      RTS
:     LDA   #<a_kw_bvs
      LDX   #>a_kw_bvs
      JSR   a_symbol_is
      BCC   @no
      LDA   #$70
      SEC
      RTS
@no:
      CLC
      RTS

a_peek_upper:
      JSR   a_peek
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

a_is_ident_start:
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

a_is_ident:
      JSR   a_is_ident_start
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

; Find temp a_symbol. Carry set if found; a_symbol_index contains the index.
a_find_symbol:
      LDX   #0
@entry:
      CPX   a_symbol_count
      BCS   @missing
      LDA   a_sym_name_len,X
      CMP   a_symbol_len
      BNE   @next
      STX   a_symbol_index
      JSR   a_symbol_name_ptr
      LDY   #0
@name:
      CPY   a_symbol_len
      BCS   @found
      LDA   (a_symbol_ptr),Y
      CMP   a_symbol,Y
      BNE   @next_saved
      INY
      BRA   @name
@next_saved:
      LDX   a_symbol_index
@next:
      INX
      BRA   @entry
@found:
      SEC
      RTS
@missing:
      CLC
      RTS

; Find or create an undefined symbol. Carry set means table full.
a_get_symbol:
      JSR   a_find_symbol
      BCS   @ok
      LDX   a_symbol_count
      CPX   #NASM_SYMBOL_TABLE_CAP
      BCS   @bad
      STX   a_symbol_index
      LDA   a_symbol_len
      STA   a_sym_name_len,X
      STZ   a_sym_value_l,X
      STZ   a_sym_value_h,X
      LDA   #NOBJ_SYM_UNDEFINED
      STA   a_sym_section,X
      STZ   a_sym_flags,X
      STZ   a_sym_decl,X
      STZ   a_sym_forward,X
      JSR   a_symbol_name_ptr
      LDY   #0
@copy:
      CPY   a_symbol_len
      BCS   @created
      LDA   a_symbol,Y
      STA   (a_symbol_ptr),Y
      INY
      BRA   @copy
@created:
      INC   a_symbol_count
@ok:
      CLC
      RTS
@bad:
      SEC
      RTS

; Define temp symbol at the current section-relative code offset.
a_define_symbol:
      JSR   a_find_symbol
      BCC   @create
      LDX   a_symbol_index
      LDA   a_sym_section,X
      CMP   #NOBJ_SYM_UNDEFINED
      BNE   @bad
      BRA   @check_decl
@create:
      JSR   a_get_symbol
      BCS   @bad
      LDX   a_symbol_index
@check_decl:
      LDA   a_sym_decl,X
      AND   #NASM_DECL_IMPORT
      BNE   @bad
@set:
      LDA   a_code_len
      STA   a_sym_value_l,X
      LDA   a_code_len+1
      STA   a_sym_value_h,X
      LDA   a_section
      STA   a_sym_section,X
      CLC
      RTS
@bad:
      SEC
      RTS

; Predeclare an assignment whose right-hand side is not absolute yet.
a_declare_pending_absolute:
      JSR   a_find_symbol
      BCC   @create
      LDX   a_symbol_index
      LDA   a_sym_section,X
      CMP   #NOBJ_SYM_UNDEFINED
      BNE   @bad
      BRA   @check_decl
@create:
      JSR   a_get_symbol
      BCS   @bad
      LDX   a_symbol_index
@check_decl:
      LDA   a_sym_decl,X
      AND   #NASM_DECL_IMPORT
      BNE   @bad
      CLC
      RTS
@bad:
      SEC
      RTS

; Define or verify a case-insensitive absolute symbol from NAME = expression.
a_define_absolute:
      LDA   a_tmp0
      STA   a_repeat_l
      LDA   a_tmp1
      STA   a_repeat_h
      LDA   a_pass
      CMP   #NASM_PASS_DEFINE
      BEQ   @define
      CMP   #NASM_PASS_RESOLVE
      BNE   @verify
      JSR   a_find_symbol
      BCC   @bad
      LDX   a_symbol_index
      LDA   a_sym_section,X
      CMP   #NOBJ_SYM_ABSOLUTE
      BEQ   @verify_found
      CMP   #NOBJ_SYM_UNDEFINED
      BNE   @bad
      INC   a_resolve_changed
      BRA   @check_decl
@define:
      JSR   a_find_symbol
      BCC   @create
      LDX   a_symbol_index
      LDA   a_sym_section,X
      CMP   #NOBJ_SYM_UNDEFINED
      BNE   @bad
      BRA   @check_decl
@create:
      JSR   a_get_symbol
      BCS   @bad
      LDX   a_symbol_index
@check_decl:
      LDA   a_sym_decl,X
      AND   #NASM_DECL_IMPORT
      BNE   @bad
      LDA   a_repeat_l
      STA   a_sym_value_l,X
      LDA   a_repeat_h
      STA   a_sym_value_h,X
      LDA   #NOBJ_SYM_ABSOLUTE
      STA   a_sym_section,X
      CLC
      RTS
@verify:
      JSR   a_find_symbol
      BCC   @bad
      LDX   a_symbol_index
@verify_found:
      LDA   a_sym_section,X
      CMP   #NOBJ_SYM_ABSOLUTE
      BNE   @bad
      LDA   a_sym_value_l,X
      CMP   a_repeat_l
      BNE   @bad
      LDA   a_sym_value_h,X
      CMP   a_repeat_h
      BNE   @bad
      CLC
      RTS
@bad:
      SEC
      RTS

; Declare a comma-separated list of imports or exports. A contains the state.
a_declare_symbols:
      STA   a_expr_digit
@symbol:
      JSR   a_skip_hspace
      JSR   a_parse_identifier
      BCS   @bad
      JSR   a_get_symbol
      BCS   @bad
      LDX   a_symbol_index
      LDA   a_expr_digit
      CMP   #NASM_DECL_IMPORT
      BEQ   @check_export
      LDA   a_sym_decl,X
      AND   #NASM_DECL_IMPORT
      BNE   @bad
      BRA   @mark
@check_export:
      LDA   a_sym_decl,X
      AND   #NASM_DECL_EXPORT
      BNE   @bad
@mark:
      LDA   a_sym_decl,X
      ORA   a_expr_digit
      STA   a_sym_decl,X
      LDA   a_sym_flags,X
      ORA   #NOBJ_SYM_GLOBAL
      STA   a_sym_flags,X
      JSR   a_skip_hspace
      JSR   a_peek
      BCC   @done
      CMP   #','
      BNE   @done
      JSR   a_next
      BRA   @symbol
@done:
      CLC
      RTS
@bad:
      SEC
      RTS

; Parse count[,fill] into a_repeat_l/h and a_repeat_value.
a_parse_count_fill:
      JSR   a_skip_hspace
      JSR   a_parse_expression
      BCS   @bad
      LDA   a_operand_symbol
      BNE   @bad
      LDA   a_tmp0
      STA   a_repeat_l
      LDA   a_tmp1
      STA   a_repeat_h
      STZ   a_repeat_value
      JSR   a_skip_hspace
      JSR   a_peek
      BCC   @ok
      CMP   #','
      BNE   @ok
      JSR   a_next
      JSR   a_skip_hspace
      JSR   a_parse_expression
      BCS   @bad
      LDA   a_operand_symbol
      BNE   @bad
      LDA   a_tmp1
      BNE   @bad
      LDA   a_tmp0
      STA   a_repeat_value
@ok:
      CLC
      RTS
@bad:
      SEC
      RTS

a_emit_repeat:
@loop:
      LDA   a_repeat_l
      ORA   a_repeat_h
      BEQ   @done
      LDA   a_repeat_value
      JSR   a_reserve_emit
      BCS   @bad
      LDA   a_repeat_l
      BNE   :+
      DEC   a_repeat_h
:     DEC   a_repeat_l
      BRA   @loop
@done:
      CLC
      RTS
@bad:
      SEC
      RTS

; Validate a 1..256 power-of-two boundary and retain its low-byte mask/log2.
a_prepare_alignment:
      LDA   a_repeat_h
      BEQ   @byte
      CMP   #1
      BNE   @bad
      LDA   a_repeat_l
      BNE   @bad
      LDA   #$FF
      STA   a_repeat_mask
      LDX   #8
      BRA   @record
@byte:
      LDA   a_repeat_l
      BEQ   @bad
      DEC
      STA   a_repeat_mask
      AND   a_repeat_l
      BNE   @bad
      LDA   a_repeat_l
      LDX   #0
@log:
      CMP   #1
      BEQ   @record
      LSR
      INX
      BRA   @log
@record:
      TXA
      LDY   a_section
      CMP   a_sec_align,Y
      BCC   @ok
      STA   a_sec_align,Y
@ok:
      CLC
      RTS
@bad:
      SEC
      RTS

a_emit_alignment:
@loop:
      LDA   a_code_len
      AND   a_repeat_mask
      BEQ   @done
      LDA   a_repeat_value
      JSR   a_reserve_emit
      BCS   @bad
      BRA   @loop
@done:
      CLC
      RTS
@bad:
      SEC
      RTS

; Every undefined symbol must be an explicit import; every export must resolve.
a_validate_symbols:
      LDX   #0
@symbol:
      CPX   a_symbol_count
      BCS   @ok
      LDA   a_sym_section,X
      CMP   #NOBJ_SYM_UNDEFINED
      BNE   @defined
      LDA   a_sym_decl,X
      CMP   #NASM_DECL_IMPORT
      BNE   @bad
      BRA   @next
@defined:
      LDA   a_sym_decl,X
      AND   #NASM_DECL_IMPORT
      BNE   @bad
@next:
      INX
      BRA   @symbol
@ok:
      CLC
      RTS
@bad:
      SEC
      RTS

; X = symbol index -> pointer to its fixed-size name slot.
a_symbol_name_ptr:
      STX   a_symbol_index
      TXA
      STA   a_tmp0
      STZ   a_tmp1
      ASL   a_tmp0
      ROL   a_tmp1
      ASL   a_tmp0
      ROL   a_tmp1
      ASL   a_tmp0
      ROL   a_tmp1
      ASL   a_tmp0
      ROL   a_tmp1
      ASL   a_tmp0
      ROL   a_tmp1
      CLC
      LDA   #<a_sym_names
      ADC   a_tmp0
      STA   a_symbol_ptr
      LDA   #>a_sym_names
      ADC   a_tmp1
      STA   a_symbol_ptr+1
      LDX   a_symbol_index
      RTS

; Return an explicit byte transform or range-checked ABS8 by default.
a_byte_reloc_type:
      LDA   a_operand_reloc
      BNE   @done
      LDA   #NOBJ_RELOC_ABS8
@done:
      RTS

; Record relocation type A at current section offset + X.
a_add_relocation:
      STA   a_operand_reloc
      STX   a_relative_size
      LDA   a_tmp0
      STA   a_reloc_addend_l
      LDA   a_tmp1
      STA   a_reloc_addend_h
      CLC
      LDA   a_code_len
      ADC   a_relative_size
      STA   a_tmp0
      LDA   a_code_len+1
      ADC   #0
      STA   a_tmp1
      LDA   a_section
      JSR   a_reloc_emit
      BCS   @bad
      LDA   a_operand_reloc
      JSR   a_reloc_emit
      BCS   @bad
      LDA   a_tmp0
      JSR   a_reloc_emit
      BCS   @bad
      LDA   a_tmp1
      JSR   a_reloc_emit
      BCS   @bad
      LDA   a_symbol_index
      JSR   a_reloc_emit
      BCS   @bad
      LDA   #0
      JSR   a_reloc_emit
      BCS   @bad
      LDA   a_reloc_addend_l
      JSR   a_reloc_emit
      BCS   @bad
      LDA   a_reloc_addend_h
      JSR   a_reloc_emit
      BCS   @bad
      INC   a_reloc_count
      BNE   :+
      INC   a_reloc_count+1
:     CLC
      RTS
@bad:
      SEC
      RTS

a_reloc_emit:
      PHA
      LDA   a_reloc_len+1
      CMP   #>NASM_RELOC_CAP
      BCC   @room
      PLA
      SEC
      RTS
@room:
      PLA
      LDY   #0
      STA   (a_reloc_ptr),Y
      INC   a_reloc_ptr
      BNE   :+
      INC   a_reloc_ptr+1
:     INC   a_reloc_len
      BNE   :+
      INC   a_reloc_len+1
:     CLC
      RTS

; 16-bit precedence-climbing expression evaluator. Values wrap at 16 bits;
; symbol +/- constant remains relocatable through a_operand_symbol/index.
a_parse_expression:
      STZ   a_expr_depth
      STZ   a_operand_reloc
      LDA   #1

a_expr_parse:
      LDX   a_expr_depth
      CPX   #NASM_EXPR_DEPTH
      BCC   :+
      JMP   @full
:
      STA   a_expr_min,X
      INC   a_expr_depth
      JSR   a_expr_unary
      BCS   @bad
@operator:
      JSR   a_expr_peek_operator
      BCC   @done
      LDY   a_expr_depth
      DEY
      STA   a_expr_op,Y
      TXA
      CMP   a_expr_min,Y
      BCC   @done
      STA   a_expr_prec,Y
      LDA   a_tmp0
      STA   a_expr_lhs_l,Y
      LDA   a_tmp1
      STA   a_expr_lhs_h,Y
      LDA   a_operand_symbol
      STA   a_expr_lhs_sym,Y
      LDA   a_symbol_index
      STA   a_expr_lhs_index,Y
      LDA   a_expr_op,Y
      JSR   a_expr_consume_operator
      BCS   @bad
      LDY   a_expr_depth
      DEY
      LDA   a_expr_prec,Y
      INC
      JSR   a_expr_parse
      BCS   @bad
      LDA   a_tmp0
      STA   a_expr_rhs_l
      LDA   a_tmp1
      STA   a_expr_rhs_h
      LDA   a_operand_symbol
      STA   a_expr_rhs_sym
      LDA   a_symbol_index
      STA   a_expr_rhs_index
      LDY   a_expr_depth
      DEY
      LDA   a_expr_lhs_l,Y
      STA   a_tmp0
      LDA   a_expr_lhs_h,Y
      STA   a_tmp1
      LDA   a_expr_lhs_sym,Y
      STA   a_operand_symbol
      LDA   a_expr_lhs_index,Y
      STA   a_symbol_index
      LDA   a_expr_op,Y
      JSR   a_expr_apply
      BCS   @bad
      BRA   @operator
@done:
      DEC   a_expr_depth
      CLC
      RTS
@bad:
      DEC   a_expr_depth
@full:
      SEC
      RTS

; Return operator id in A and precedence in X, carry set.
a_expr_peek_operator:
      JSR   a_skip_hspace
      JSR   a_peek
      BCC   @no
      CMP   #'|'
      BEQ   @or
      CMP   #'^'
      BEQ   @xor
      CMP   #'&'
      BEQ   @and
      CMP   #'<'
      BEQ   @shl
      CMP   #'>'
      long_beq @shr
      CMP   #'+'
      long_beq @add
      CMP   #'-'
      long_beq @sub
      CMP   #'*'
      long_beq @mul
      CMP   #'/'
      long_beq @div
      CMP   #'%'
      long_beq @mod
      CMP   #'='
      long_beq @eq
@no:
      CLC
      RTS
@or:  JSR a_expr_peek_second
      BCC @bit_or
      CMP #'|'
      BNE @bit_or
      LDA #EXPR_BOOL_OR
      LDX #1
      JMP @yes
@bit_or:
      LDA #EXPR_OR
      LDX #4
      JMP @yes
@xor: JSR a_expr_peek_second
      BCC @bit_xor
      CMP #'^'
      BNE @bit_xor
      LDA #EXPR_BOOL_XOR
      LDX #2
      JMP @yes
@bit_xor:
      LDA #EXPR_XOR
      LDX #5
      BRA @yes
@and: JSR a_expr_peek_second
      BCC @bit_and
      CMP #'&'
      BNE @bit_and
      LDA #EXPR_BOOL_AND
      LDX #2
      BRA @yes
@bit_and:
      LDA #EXPR_AND
      LDX #6
      BRA @yes
@shl: JSR a_expr_peek_second
      BCC @lt
      CMP #'<'
      BEQ @shift_left
      CMP #'='
      BEQ @le
      CMP #'>'
      BEQ @ne
@lt:  LDA #EXPR_LT
      LDX #3
      BRA @yes
@le:  LDA #EXPR_LE
      LDX #3
      BRA @yes
@ne:  LDA #EXPR_NE
      LDX #3
      BRA @yes
@shift_left:
      LDA #EXPR_SHL
      LDX #7
      BRA @yes
@shr: JSR a_expr_peek_second
      BCC @gt
      CMP #'>'
      BEQ @shift_right
      CMP #'='
      BEQ @ge
@gt:  LDA #EXPR_GT
      LDX #3
      BRA @yes
@ge:  LDA #EXPR_GE
      LDX #3
      BRA @yes
@shift_right:
      LDA #EXPR_SHR
      LDX #7
      BRA @yes
@eq:  LDA #EXPR_EQ
      LDX #3
      BRA @yes
@add: LDA #EXPR_ADD
      LDX #8
      BRA @yes
@sub: LDA #EXPR_SUB
      LDX #8
      BRA @yes
@mul: LDA #EXPR_MUL
      LDX #9
      BRA @yes
@div: LDA #EXPR_DIV
      LDX #9
      BRA @yes
@mod: LDA #EXPR_MOD
      LDX #9
@yes:
      SEC
      RTS

a_expr_peek_second:
      LDA   a_left+1
      BNE   @read
      LDA   a_left
      CMP   #2
      BCC   @none
@read:
      CLC
      LDA   a_src
      ADC   #1
      TAX
      LDA   a_src+1
      ADC   #0
      STA   WIN3_MI
      LDA   a_src+2
      ADC   #0
      STA   WIN3_HI
      LDA   WIN3_BASE,X
      PHA
      LDA   a_src+1
      STA   WIN3_MI
      LDA   a_src+2
      STA   WIN3_HI
      PLA
      SEC
      RTS
@none:
      CLC
      RTS

a_expr_consume_operator:
      CMP   #EXPR_BOOL_OR
      BEQ   @double
      CMP   #EXPR_BOOL_AND
      BEQ   @double
      CMP   #EXPR_BOOL_XOR
      BEQ   @double
      CMP   #EXPR_NE
      BEQ   @double
      CMP   #EXPR_LE
      BEQ   @double
      CMP   #EXPR_GE
      BEQ   @double
      CMP   #EXPR_SHL
      BEQ   @double
      CMP   #EXPR_SHR
      BEQ   @double
@single:
      JSR   a_next
      CLC
      RTS
@double:
      JSR   a_next
      JSR   a_next
      BCC   @bad
      CLC
      RTS
@bad:
      SEC
      RTS

a_expr_unary:
      JSR   a_skip_hspace
      JSR   a_peek
      long_bcc @bad
      CMP   #'+'
      BEQ   @plus
      CMP   #'-'
      BEQ   @minus
      CMP   #'~'
      BEQ   @not
      CMP   #'!'
      BEQ   @bool_not
      CMP   #'<'
      BEQ   @low
      CMP   #'>'
      BEQ   @high
      JMP   a_expr_primary
@plus:
      JSR   a_next
      JMP   a_expr_unary
@minus:
      JSR   a_next
      JSR   a_expr_unary
      long_bcs @bad
      LDA   a_operand_symbol
      long_bne @bad
      SEC
      LDA   #0
      SBC   a_tmp0
      STA   a_tmp0
      LDA   #0
      SBC   a_tmp1
      STA   a_tmp1
      CLC
      RTS
@not:
      JSR   a_next
      JSR   a_expr_unary
      BCS   @bad
      LDA   a_operand_symbol
      BNE   @bad
      LDA   a_tmp0
      EOR   #$FF
      STA   a_tmp0
      LDA   a_tmp1
      EOR   #$FF
      STA   a_tmp1
      CLC
      RTS
@bool_not:
      JSR   a_next
      JSR   a_expr_unary
      BCS   @bad
      LDA   a_operand_symbol
      BNE   @bad
      LDA   a_tmp0
      ORA   a_tmp1
      BEQ   :+
      LDA   #0
      BRA   :++
:
      LDA   #1
:
      STA   a_tmp0
      STZ   a_tmp1
      CLC
      RTS
@low:
      JSR   a_next
      JSR   a_expr_unary
      BCS   @bad
      LDA   a_operand_symbol
      BEQ   :+
      LDA   #NOBJ_RELOC_LO8
      STA   a_operand_reloc
      CLC
      RTS
:
      STZ   a_tmp1
      CLC
      RTS
@high:
      JSR   a_next
      JSR   a_expr_unary
      BCS   @bad
      LDA   a_operand_symbol
      BEQ   :+
      LDA   #NOBJ_RELOC_HI8
      STA   a_operand_reloc
      CLC
      RTS
:
      LDA   a_tmp1
      STA   a_tmp0
      STZ   a_tmp1
      CLC
      RTS
@bad:
      SEC
      RTS

a_expr_primary:
      JSR   a_skip_hspace
      JSR   a_peek_upper
      long_bcc @bad
      CMP   #'$'
      long_beq a_expr_hex
      CMP   #'%'
      long_beq a_expr_binary
      CMP   #39
      long_beq a_expr_character
      CMP   #'('
      BEQ   @group
      CMP   #':'
      BEQ   @anonymous
      CMP   #'0'
      BCC   @identifier
      CMP   #'9'+1
      long_bcc a_expr_decimal
@identifier:
      JSR   a_is_ident_start
      BCC   @bad
      JSR   a_parse_identifier
      BCS   @bad
      BRA   @symbol
@anonymous:
      JSR   a_parse_anonymous
      BCS   @bad
@symbol:
      JSR   a_get_symbol
      BCS   @bad
      LDX   a_symbol_index
      LDA   a_sym_section,X
      CMP   #NOBJ_SYM_ABSOLUTE
      BNE   @relocatable
      LDA   a_assignment_expr
      BNE   @absolute
      LDA   a_sym_forward,X
      BNE   @relocatable
@absolute:
      LDA   a_sym_value_l,X
      STA   a_tmp0
      LDA   a_sym_value_h,X
      STA   a_tmp1
      STZ   a_operand_symbol
      CLC
      RTS
@relocatable:
      LDA   a_sym_section,X
      CMP   #NOBJ_SYM_UNDEFINED
      BNE   :+
      LDA   a_pass
      CMP   #NASM_PASS_DEFINE
      BNE   :+
      LDA   #1
      STA   a_sym_forward,X
:
      LDA   #1
      STA   a_operand_symbol
      STZ   a_tmp0
      STZ   a_tmp1
      CLC
      RTS
@group:
      JSR   a_next
      LDA   #1
      JSR   a_expr_parse
      BCS   @bad
      JSR   a_skip_hspace
      JSR   a_next
      BCC   @bad
      CMP   #')'
      BNE   @bad
      CLC
      RTS
@bad:
      SEC
      RTS

a_expr_hex:
      JSR   a_next
      STZ   a_operand_symbol
      STZ   a_tmp0
      STZ   a_tmp1
      LDX   #0
@digit:
      JSR   a_peek_upper
      BCC   @done
      JSR   a_decode_hex_nibble
      BCS   @done
      CPX   #4
      BCS   @bad
      STA   a_expr_digit
      ASL   a_tmp0
      ROL   a_tmp1
      ASL   a_tmp0
      ROL   a_tmp1
      ASL   a_tmp0
      ROL   a_tmp1
      ASL   a_tmp0
      ROL   a_tmp1
      LDA   a_tmp0
      ORA   a_expr_digit
      STA   a_tmp0
      JSR   a_next
      INX
      BRA   @digit
@done:
      CPX   #0
      BEQ   @bad
      CLC
      RTS
@bad:
      SEC
      RTS

a_expr_binary:
      JSR   a_next
      STZ   a_operand_symbol
      STZ   a_tmp0
      STZ   a_tmp1
      LDX   #0
@digit:
      JSR   a_peek
      BCC   @done
      CMP   #'0'
      BEQ   @bit
      CMP   #'1'
      BNE   @done
@bit:
      CPX   #16
      BCS   @bad
      CMP   #'1'
      PHP
      ASL   a_tmp0
      ROL   a_tmp1
      PLP
      BNE   :+
      INC   a_tmp0
:     JSR   a_next
      INX
      BRA   @digit
@done:
      CPX   #0
      BEQ   @bad
      CLC
      RTS
@bad:
      SEC
      RTS

a_expr_decimal:
      STZ   a_operand_symbol
      STZ   a_tmp0
      STZ   a_tmp1
      LDX   #0
@digit:
      JSR   a_peek
      BCC   @done
      CMP   #'0'
      BCC   @done
      CMP   #'9'+1
      BCS   @done
      SEC
      SBC   #'0'
      STA   a_expr_digit
      JSR   a_expr_mul10_add
      BCS   @bad
      JSR   a_next
      INX
      BRA   @digit
@done:
      CPX   #0
      BEQ   @bad
      CLC
      RTS
@bad:
      SEC
      RTS

a_expr_mul10_add:
      LDA   a_tmp0
      STA   a_expr_acc_l
      LDA   a_tmp1
      STA   a_expr_acc_h
      ASL   a_expr_acc_l
      ROL   a_expr_acc_h
      BCS   @bad
      LDX   #3
@times8:
      ASL   a_tmp0
      ROL   a_tmp1
      BCS   @bad
      DEX
      BNE   @times8
      CLC
      LDA   a_tmp0
      ADC   a_expr_acc_l
      STA   a_tmp0
      LDA   a_tmp1
      ADC   a_expr_acc_h
      BCS   @bad
      STA   a_tmp1
      CLC
      LDA   a_tmp0
      ADC   a_expr_digit
      STA   a_tmp0
      LDA   a_tmp1
      ADC   #0
      BCS   @bad
      STA   a_tmp1
      CLC
      RTS
@bad:
      SEC
      RTS

a_expr_character:
      JSR   a_next
      JSR   a_next
      BCC   @bad
      CMP   #$0A
      BEQ   @bad
      STA   a_expr_digit
      JSR   a_next
      BCC   @bad
      CMP   #39
      BNE   @bad
      STZ   a_operand_symbol
      LDA   a_expr_digit
      STA   a_tmp0
      STZ   a_tmp1
      CLC
      RTS
@bad:
      SEC
      RTS

a_decode_hex_nibble:
      CMP   #'0'
      BCC   @bad
      CMP   #'9'+1
      BCC   @digit
      CMP   #'A'
      BCC   @bad
      CMP   #'F'+1
      BCS   @bad
      SEC
      SBC   #'A'-10
      CLC
      RTS
@digit:
      SEC
      SBC   #'0'
      CLC
      RTS
@bad:
      SEC
      RTS

a_expr_apply:
      CMP   #EXPR_BOOL_OR
      long_beq @bool_or
      CMP   #EXPR_BOOL_AND
      long_beq @bool_and
      CMP   #EXPR_BOOL_XOR
      long_beq @bool_xor
      CMP   #EXPR_EQ
      long_beq @eq
      CMP   #EXPR_NE
      long_beq @ne
      CMP   #EXPR_LT
      long_beq @lt
      CMP   #EXPR_LE
      long_beq @le
      CMP   #EXPR_GT
      long_beq @gt
      CMP   #EXPR_GE
      long_beq @ge
      CMP   #EXPR_ADD
      long_beq @add
      CMP   #EXPR_SUB
      long_beq @sub
      CMP   #EXPR_OR
      long_beq @or
      CMP   #EXPR_XOR
      long_beq @xor
      CMP   #EXPR_AND
      long_beq @and
      CMP   #EXPR_SHL
      long_beq @shl
      CMP   #EXPR_SHR
      long_beq @shr
      CMP   #EXPR_MUL
      long_beq @mul
      CMP   #EXPR_DIV
      long_beq @div
      CMP   #EXPR_MOD
      long_beq @mod
      JMP   @bad
@bool_or:
      JSR   a_expr_constants
      long_bcs @bad
      LDA   a_tmp0
      ORA   a_tmp1
      ORA   a_expr_rhs_l
      ORA   a_expr_rhs_h
      BRA   @set_nonzero
@bool_and:
      JSR   a_expr_constants
      long_bcs @bad
      LDA   a_tmp0
      ORA   a_tmp1
      BEQ   @false
      LDA   a_expr_rhs_l
      ORA   a_expr_rhs_h
@set_nonzero:
      BEQ   @false
@true:
      LDA   #1
      BRA   @set_bool
@false:
      LDA   #0
@set_bool:
      STA   a_tmp0
      STZ   a_tmp1
      CLC
      RTS
@bool_xor:
      JSR   a_expr_constants
      long_bcs @bad
      LDA   a_tmp0
      ORA   a_tmp1
      BEQ   :+
      LDA   #1
:
      STA   a_expr_digit
      LDA   a_expr_rhs_l
      ORA   a_expr_rhs_h
      BEQ   :+
      LDA   #1
:
      EOR   a_expr_digit
      BRA   @set_bool
@eq:  LDA #0
      BRA @compare
@ne:  LDA #1
      BRA @compare
@lt:  LDA #2
      BRA @compare
@le:  LDA #3
      BRA @compare
@gt:  LDA #4
      BRA @compare
@ge:  LDA #5
@compare:
      STA   a_expr_digit
      JSR   a_expr_constants
      long_bcs @bad
      LDA   a_tmp1
      CMP   a_expr_rhs_h
      BCC   @less
      BNE   @greater
      LDA   a_tmp0
      CMP   a_expr_rhs_l
      BCC   @less
      BNE   @greater
      LDA   a_expr_digit
      CMP   #0
      BEQ   @true
      CMP   #3
      BEQ   @true
      CMP   #5
      BEQ   @true
      BRA   @false
@less:
      LDA   a_expr_digit
      CMP   #2
      BEQ   @true
      CMP   #3
      BEQ   @true
      CMP   #1
      long_beq @true
      JMP   @false
@greater:
      LDA   a_expr_digit
      CMP   #4
      long_beq @true
      CMP   #5
      long_beq @true
      CMP   #1
      long_beq @true
      JMP   @false
@add:
      LDA   a_operand_symbol
      BEQ   @add_rhs
      LDA   a_expr_rhs_sym
      long_bne @bad
      BRA   @add_values
@add_rhs:
      LDA   a_expr_rhs_sym
      BEQ   @add_values
      STA   a_operand_symbol
      LDA   a_expr_rhs_index
      STA   a_symbol_index
@add_values:
      CLC
      LDA   a_tmp0
      ADC   a_expr_rhs_l
      STA   a_tmp0
      LDA   a_tmp1
      ADC   a_expr_rhs_h
      STA   a_tmp1
      CLC
      RTS
@sub:
      LDA   a_expr_rhs_sym
      long_bne @bad
      SEC
      LDA   a_tmp0
      SBC   a_expr_rhs_l
      STA   a_tmp0
      LDA   a_tmp1
      SBC   a_expr_rhs_h
      STA   a_tmp1
      CLC
      RTS
@or:
      JSR   a_expr_constants
      long_bcs @bad
      LDA   a_tmp0
      ORA   a_expr_rhs_l
      STA   a_tmp0
      LDA   a_tmp1
      ORA   a_expr_rhs_h
      STA   a_tmp1
      CLC
      RTS
@xor:
      JSR   a_expr_constants
      long_bcs @bad
      LDA   a_tmp0
      EOR   a_expr_rhs_l
      STA   a_tmp0
      LDA   a_tmp1
      EOR   a_expr_rhs_h
      STA   a_tmp1
      CLC
      RTS
@and:
      JSR   a_expr_constants
      long_bcs @bad
      LDA   a_tmp0
      AND   a_expr_rhs_l
      STA   a_tmp0
      LDA   a_tmp1
      AND   a_expr_rhs_h
      STA   a_tmp1
      CLC
      RTS
@shl:
      JSR   a_expr_constants
      long_bcs @bad
      LDX   a_expr_rhs_l
      LDA   a_expr_rhs_h
      BNE   @zero
      CPX   #16
      BCS   @zero
@shl_loop:
      CPX   #0
      BEQ   @ok
      ASL   a_tmp0
      ROL   a_tmp1
      DEX
      BRA   @shl_loop
@shr:
      JSR   a_expr_constants
      long_bcs @bad
      LDX   a_expr_rhs_l
      LDA   a_expr_rhs_h
      BNE   @zero
      CPX   #16
      BCS   @zero
@shr_loop:
      CPX   #0
      BEQ   @ok
      LSR   a_tmp1
      ROR   a_tmp0
      DEX
      BRA   @shr_loop
@zero:
      STZ   a_tmp0
      STZ   a_tmp1
@ok:
      CLC
      RTS
@mul:
      JSR   a_expr_constants
      long_bcs @bad
      JSR   a_expr_multiply
      CLC
      RTS
@div:
      LDA   #EXPR_DIV
      BRA   @divide
@mod:
      LDA   #EXPR_MOD
@divide:
      STA   a_expr_digit
      JSR   a_expr_constants
      long_bcs @bad
      LDA   a_expr_rhs_l
      ORA   a_expr_rhs_h
      BEQ   @bad
      JSR   a_expr_divide
      CLC
      RTS
@bad:
      SEC
      RTS

a_expr_constants:
      LDA   a_operand_symbol
      ORA   a_expr_rhs_sym
      BEQ   @ok
      SEC
      RTS
@ok:
      CLC
      RTS

a_expr_multiply:
      STZ   a_expr_acc_l
      STZ   a_expr_acc_h
      LDX   #16
@loop:
      LDA   a_expr_rhs_l
      AND   #1
      BEQ   @shift
      CLC
      LDA   a_expr_acc_l
      ADC   a_tmp0
      STA   a_expr_acc_l
      LDA   a_expr_acc_h
      ADC   a_tmp1
      STA   a_expr_acc_h
@shift:
      ASL   a_tmp0
      ROL   a_tmp1
      LSR   a_expr_rhs_h
      ROR   a_expr_rhs_l
      DEX
      BNE   @loop
      LDA   a_expr_acc_l
      STA   a_tmp0
      LDA   a_expr_acc_h
      STA   a_tmp1
      RTS

a_expr_divide:
      STZ   a_expr_acc_l
      STZ   a_expr_acc_h
      STZ   a_expr_rem_l
      STZ   a_expr_rem_h
      LDX   #16
@loop:
      ASL   a_tmp0
      ROL   a_tmp1
      ROL   a_expr_rem_l
      ROL   a_expr_rem_h
      ASL   a_expr_acc_l
      ROL   a_expr_acc_h
      LDA   a_expr_rem_h
      CMP   a_expr_rhs_h
      BCC   @next
      BNE   @subtract
      LDA   a_expr_rem_l
      CMP   a_expr_rhs_l
      BCC   @next
@subtract:
      SEC
      LDA   a_expr_rem_l
      SBC   a_expr_rhs_l
      STA   a_expr_rem_l
      LDA   a_expr_rem_h
      SBC   a_expr_rhs_h
      STA   a_expr_rem_h
      INC   a_expr_acc_l
@next:
      DEX
      BNE   @loop
      LDA   a_expr_digit
      CMP   #EXPR_MOD
      BEQ   @remainder
      LDA   a_expr_acc_l
      STA   a_tmp0
      LDA   a_expr_acc_h
      STA   a_tmp1
      RTS
@remainder:
      LDA   a_expr_rem_l
      STA   a_tmp0
      LDA   a_expr_rem_h
      STA   a_tmp1
      RTS

a_read_upper:
      JSR   a_next
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

a_peek:
      LDA   a_left
      ORA   a_left+1
      BNE   @read
      LDA   a_include_depth
      BEQ   @eof
      JSR   a_leave_include
      BCC   a_peek
      INC   a_include_error
      BRA   @eof
@read:
      LDY   a_src
      LDA   WIN3_BASE,Y
      SEC
      RTS
@eof:
      CLC
      RTS

a_next:
      JSR   a_peek
      BCC   @eof
      PHA
      INC   a_src
      BNE   @left
      INC   a_src+1
      LDA   a_src+1
      STA   WIN3_MI
      BNE   @left
      INC   a_src+2
      LDA   a_src+2
      STA   WIN3_HI
@left:
      LDA   a_left
      BNE   :+
      DEC   a_left+1
:     DEC   a_left
      PLA
      PHA
      CMP   #$0A
      BEQ   @newline
      INC   nasm_error_column
      BNE   @positioned
      INC   nasm_error_column+1
      BRA   @positioned
@newline:
      INC   nasm_error_line
      BNE   :+
      INC   nasm_error_line+1
:     LDA   #1
      STA   nasm_error_column
      STZ   nasm_error_column+1
@positioned:
      PLA
      SEC
@eof:
      RTS

; Append A to the current initialized section.
a_emit:
      PHA
      LDX   a_section
      LDA   a_sec_flags,X
      AND   #NOBJ_SEC_ZEROFILL
      BNE   a_emit_full
      LDA   a_pass
      CMP   #NASM_PASS_EMIT
      BEQ   @write
      PLA
      JMP   a_advance
@write:
      LDA   a_code_len+1
      CMP   a_sec_size_h,X
      BCC   @room
      BNE   a_emit_full
      LDA   a_code_len
      CMP   a_sec_size_l,X
      BCS   a_emit_full
@room:
      CLC
      LDA   a_sec_data_l,X
      ADC   a_code_len
      STA   a_word
      LDA   a_sec_data_h,X
      ADC   a_code_len+1
      STA   a_word+1
      PLA
      LDY   #0
      STA   (a_word),Y
a_advance:
      INC   a_code_len
      BNE   :+
      INC   a_code_len+1
:     CLC
      RTS
a_emit_full:
      PLA
      SEC
      RTS

; Storage directives advance zerofill sections without serializing bytes.
a_reserve_emit:
      PHA
      LDX   a_section
      LDA   a_sec_flags,X
      AND   #NOBJ_SEC_ZEROFILL
      BNE   @advance
      PLA
      JMP   a_emit
@advance:
      PLA
      JMP   a_advance

; Append metadata without extending CODE length.
a_emit_tail:
      PHY
      PHA
      LDA   a_cap
      ORA   a_cap+1
      BEQ   @full
      PLA
      LDY   #0
      STA   (a_dst),Y
      INC   a_dst
      BNE   :+
      INC   a_dst+1
:     LDA   a_cap
      BNE   :+
      DEC   a_cap+1
:     DEC   a_cap
      PLY
      CLC
      RTS
@full:
      PLA
      PLY
      SEC
      RTS

      .segment "RODATA"
a_kw_bcc:  .byte "BCC", 0
a_kw_bcs:  .byte "BCS", 0
a_kw_beq:  .byte "BEQ", 0
a_kw_bmi:  .byte "BMI", 0
a_kw_bne:  .byte "BNE", 0
a_kw_bpl:  .byte "BPL", 0
a_kw_bra:  .byte "BRA", 0
a_kw_bvc:  .byte "BVC", 0
a_kw_bvs:  .byte "BVS", 0
a_kw_byte: .byte "BYTE", 0
a_kw_word: .byte "WORD", 0
a_kw_import: .byte "IMPORT", 0
a_kw_export: .byte "EXPORT", 0
a_kw_res: .byte "RES", 0
a_kw_align: .byte "ALIGN", 0
a_kw_segment: .byte "SEGMENT", 0
a_kw_include: .byte "INCLUDE", 0
a_kw_incbin: .byte "INCBIN", 0
a_kw_assert: .byte "ASSERT", 0
a_kw_code: .byte "CODE", 0
a_kw_data: .byte "DATA", 0
a_kw_bss: .byte "BSS", 0

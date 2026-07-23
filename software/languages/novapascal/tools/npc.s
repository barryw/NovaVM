; Resident NovaPascal compiler: XRAM Pascal source -> textual 65C02 assembly.

      .setcpu "w65c02"
      .include "nptool.inc"
      .include "longbranch.inc"
      .include "libfiles.inc"
      .include "libmemory.inc"
      .include "xram.inc"
      .include "npproj.inc"
      .include "npc_frontend.inc"

SYMBOL_CAP  = 64
SYMBOL_NAME_CAP = 29
SYMBOL_SIZE = 36
SYMBOL_OFFSET_LO = 31
SYMBOL_OFFSET_HI = 32
SYMBOL_DEPTH = 33
SYMBOL_FLAGS = 34
SYM_LOCAL = $20
SYM_PARAM = $40
SYM_BYREF = $80
P_IDENT_CAP = 32
P_UNIT_CAP = 16
P_UNIT_NAME_CAP = 16
P_UNIT_STORAGE = P_UNIT_CAP * P_UNIT_NAME_CAP
P_INCLUDE_DEPTH_CAP = 8
P_INCLUDE_COUNT_CAP = P_INCLUDE_DEPTH_CAP
P_SOURCE_NAME_CAP = 32
P_CONST_CAP = 32
P_CONST_NAME_CAP = P_IDENT_CAP
P_TYPE_CAP = 48
P_TYPE_NAME_CAP = P_IDENT_CAP
P_TYPE_USER_BASE = 16
P_FIELD_CAP = 24
P_FIELD_NAME_POOL_CAP = 255
P_DIM_CAP = 24
P_WITH_CAP = 4
P_ROUTINE_DEPTH_CAP = 8
P_ROUTINE_CAP = 64
P_ROUTINE_HAS_FRAME = $40
P_ROUTINE_HAS_NESTED = $80
P_CONTROL_CAP = 16
P_CALL_DEPTH_CAP = 8
P_STATE_SYMBOL_OFFSET = 0
P_STATE_CONST_NAME_OFFSET = P_STATE_SYMBOL_OFFSET + SYMBOL_CAP * SYMBOL_SIZE
P_STATE_TYPE_NAME_OFFSET = P_STATE_CONST_NAME_OFFSET + P_CONST_CAP * P_CONST_NAME_CAP
P_STATE_SIZE = P_STATE_TYPE_NAME_OFFSET + P_TYPE_CAP * P_TYPE_NAME_CAP
TYPE_KIND_ORDINAL = 1
TYPE_KIND_STRING = 2
TYPE_KIND_SET = 3
TYPE_KIND_RECORD = 4
TYPE_KIND_ARRAY = 5
TYPE_KIND_REAL = 6
TYPE_KIND_FILE = 7
TYPE_NONE          = 0
TYPE_BYTE          = 1
TYPE_BOOLEAN       = TYPE_BYTE
TYPE_WORD          = 2
TYPE_ARRAY_BYTE    = 3
TYPE_ARRAY_BOOLEAN = TYPE_ARRAY_BYTE
TYPE_REAL          = 4
TYPE_FILE          = 5
TYPE_STRING_REF    = 6
TYPE_SET_REF       = 7
TYPE_CHAR          = 8
TYPE_TEXT          = 9

SYSFN_SAME = 1
SYSFN_REAL = 2
SYSFN_WORD = 3

WRITE_BYTE   = 1
WRITE_CHAR   = 2
WRITE_WORD   = 3
WRITE_STRING = 4
WRITE_REAL   = 5
WRITE_FILE   = 6
WRITE_BLOCK  = 7

PASCAL_OK         = 0
PASCAL_ERR_SYNTAX = 1
PASCAL_ERR_OUTPUT = 2
PASCAL_ERR_INCLUDE = 3
PASCAL_ERR_MEMORY = 4

      .segment "ZEROPAGE"
p_src:       .res 3
p_left:      .res 2
p_word:      .res 2
p_expected:  .res 1
p_char:      .res 1

      .segment "NPCFE_BSS"
source_len:        .res 2
source_xaddr:      .res 3
generated_asm_len: .res 3
p_state_xaddr:     .res 3
p_state_allocated: .res 1
p_state_offset:    .res 2
p_output_file_id:  .res 2
p_output_open:     .res 1
p_output_used:     .res 2
p_output_buffer:   .res 256
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
p_call_stack_len:  .res P_CALL_DEPTH_CAP
p_call_stack_hash0: .res P_CALL_DEPTH_CAP
p_call_stack_hash1: .res P_CALL_DEPTH_CAP
p_call_stack_hash2: .res P_CALL_DEPTH_CAP
p_call_stack_names: .res P_CALL_DEPTH_CAP * P_IDENT_CAP
p_argument_count:  .res 1
p_argument_bytes:  .res 1
p_call_arity:      .res 1
p_statement_refmask:.res 1
p_statement_widthmask:.res 1
p_statement_force_stack:.res 1
p_read_call:       .res 1
p_read_size:       .res 2
p_val_call:        .res 1
p_val_size:        .res 1
p_scan_depth:      .res 1
p_scan_quote:      .res 1
p_unit_count:      .res 1
p_unit_len:        .res P_UNIT_CAP
p_unit_kind:       .res P_UNIT_CAP
p_unit_iter:       .res 1
p_emit_error:      .res 1
p_external_macro_emitted: .res 1
p_saved_ident:     .res P_IDENT_CAP
p_call_ident:      .res P_IDENT_CAP
p_unit_names:      .res P_UNIT_STORAGE
p_symbol_count:    .res 1
p_symbol_group:    .res 1
p_symbol_iter:     .res 1
p_symbol_flags:    .res 1
p_symbol_type:     .res 1
p_symbol_offset:   .res 2
p_symbol_depth:    .res 1
p_decl_type:       .res 1
p_array_len:       .res 2
p_target_symbol:   .res 1
p_target_type:     .res 1
p_target_flags:    .res 1
p_target_offset:   .res 2
p_target_depth:    .res 1
p_array_depth:     .res 1
p_array_symbols:   .res 8
p_expr_type:       .res 1
p_function_arg_type:.res 1
p_function_result_type:.res 1
p_left_type:       .res 1
p_operator:        .res 1
p_operator_depth:  .res 1
p_operator_stack:  .res 8
p_operand_types:   .res 8
p_label:           .res 2
p_label_saved:     .res 2
p_mark_src:        .res 3
p_mark_left:       .res 2
p_mark_line:       .res 2
p_mark_column:     .res 2
p_mark_include_depth: .res 1
p_mark_overlay:    .res 1
p_mark_name_len:   .res 1
p_mark_name:       .res P_SOURCE_NAME_CAP
p_function_active: .res 1
p_function_hash:   .res 3
p_compiling_unit:  .res 1
p_interface_count: .res 1
p_interface_index: .res 1
p_interface_sig:   .res 16
p_interface_hash0: .res 16
p_interface_hash1: .res 16
p_interface_hash2: .res 16
p_control_depth:   .res 1
p_control_a_lo:    .res P_CONTROL_CAP
p_control_a_hi:    .res P_CONTROL_CAP
p_control_b_lo:    .res P_CONTROL_CAP
p_control_b_hi:    .res P_CONTROL_CAP
p_control_symbol:  .res P_CONTROL_CAP
p_case_body_lo:    .res P_CONTROL_CAP
p_case_body_hi:    .res P_CONTROL_CAP
p_case_word:       .res P_CONTROL_CAP
p_window_m:        .res 1
p_window_h:        .res 1
p_window_valid:    .res 1
p_source_error:    .res 1
p_source_detail:   .res 1
p_include_depth:   .res 1
p_include_count:   .res 1
p_include_name_len:.res 1
p_include_name:    .res P_SOURCE_NAME_CAP
p_current_name_len:.res 1
p_current_name:    .res P_SOURCE_NAME_CAP
p_include_xaddr:   .res 3
p_include_len:     .res 2
p_include_stack_src_l: .res P_INCLUDE_DEPTH_CAP
p_include_stack_src_m: .res P_INCLUDE_DEPTH_CAP
p_include_stack_src_h: .res P_INCLUDE_DEPTH_CAP
p_include_stack_left_l: .res P_INCLUDE_DEPTH_CAP
p_include_stack_left_h: .res P_INCLUDE_DEPTH_CAP
p_include_stack_line_l: .res P_INCLUDE_DEPTH_CAP
p_include_stack_line_h: .res P_INCLUDE_DEPTH_CAP
p_include_stack_col_l: .res P_INCLUDE_DEPTH_CAP
p_include_stack_col_h: .res P_INCLUDE_DEPTH_CAP
p_include_stack_name_len: .res P_INCLUDE_DEPTH_CAP
p_include_stack_names: .res P_INCLUDE_DEPTH_CAP * P_SOURCE_NAME_CAP
p_include_alloc_l:.res P_INCLUDE_COUNT_CAP
p_include_alloc_m:.res P_INCLUDE_COUNT_CAP
p_include_alloc_h:.res P_INCLUDE_COUNT_CAP
p_release_error:   .res 1
p_frontend_status: .res 1
p_const_count:      .res 1
p_const_iter:       .res 1
p_type_count:       .res 1
p_type_iter:        .res 1
p_type_decl_len:    .res 1
p_type_outer_decl_len:.res 1
p_type_decl_name:   .res P_TYPE_NAME_CAP
p_type_work_kind:   .res 1
p_type_work_low:    .res 2
p_type_work_high:   .res 2
p_type_work_element:.res 1
p_type_total:       .res 2
p_type_extent:      .res 2
p_type_saved_size:  .res 2
p_type_group_count: .res 1
p_const_element_type:.res 1
p_const_element_size:.res 2
p_const_total_size:.res 2
p_const_bytes:      .res 2
p_const_remaining:  .res 1
p_scope_level:     .res 1
p_scope_symbol_base:.res 1
p_routine_param_count:.res 1
p_scope_param_end: .res 1
p_routine_kind:     .res 1
p_function_return_type:.res 1
p_routine_context_depth:.res 1
p_routine_context_index:.res 1
p_context_scope_base:.res P_ROUTINE_DEPTH_CAP
p_context_param_end:.res P_ROUTINE_DEPTH_CAP
p_context_param_count:.res P_ROUTINE_DEPTH_CAP
p_context_frame_l:.res P_ROUTINE_DEPTH_CAP
p_context_frame_h:.res P_ROUTINE_DEPTH_CAP
p_context_kind:.res P_ROUTINE_DEPTH_CAP
p_context_return_type:.res P_ROUTINE_DEPTH_CAP
p_context_function_active:.res P_ROUTINE_DEPTH_CAP
p_context_name_len:.res P_ROUTINE_DEPTH_CAP
p_context_hash0:.res P_ROUTINE_DEPTH_CAP
p_context_hash1:.res P_ROUTINE_DEPTH_CAP
p_context_hash2:.res P_ROUTINE_DEPTH_CAP
p_context_names:.res P_ROUTINE_DEPTH_CAP * P_IDENT_CAP
p_routine_count:    .res 1
p_current_routine:  .res 1
p_current_overlay:  .res 1
p_routine_iter:     .res 1
p_routine_name_len: .res P_ROUTINE_CAP
p_routine_hash0:    .res P_ROUTINE_CAP
p_routine_hash1:    .res P_ROUTINE_CAP
p_routine_hash2:    .res P_ROUTINE_CAP
p_routine_owner:    .res P_ROUTINE_CAP
p_routine_kind_table:.res P_ROUTINE_CAP
p_routine_return:   .res P_ROUTINE_CAP
p_routine_params:   .res P_ROUTINE_CAP
p_routine_refmask:  .res P_ROUTINE_CAP
p_routine_overlay:  .res P_ROUTINE_CAP
p_routine_param_refmask:.res 1
p_routine_param_widthmask:.res 1
p_frame_size:      .res 2
p_param_flags:     .res 1
p_for_direction:   .res 1
p_param_stack_offset:.res 1
p_write_type:       .res 1
p_write_kind:       .res 1
p_write_newline:    .res 1
p_write_file:       .res 1
p_write_file_text:  .res 1
p_write_file_element:.res 1
p_write_size:       .res 2
p_type_meta_start:  .res 1
p_type_group_start: .res 1
p_type_meta_count:  .res 1
p_field_iter:       .res 1
p_dim_iter:         .res 1
p_designator_type:  .res 1
p_designator_row:   .res 1
p_designator_dim:   .res 1
p_with_depth:       .res 1
p_with_type:        .res P_WITH_CAP
p_with_slot:        .res 1
p_target_with:      .res 1
p_target_with_slot: .res 1
p_set_start:        .res 1
p_set_end:          .res 1
p_set_large:        .res 1
p_set_bits:         .res 32

      .segment "BSS"
; Signature widths must survive frontend overlay calls, but need not consume
; the frontend's deliberately packed $2000-$9FFF image.
p_routine_widthmask:.res P_ROUTINE_CAP

; Source paging state also lives outside the packed frontend. Each active file
; owns one 16 KiB NDK allocation split into two 8 KiB pages, so lookahead can
; cross a refill boundary without overwriting the bytes a parser mark may need.
p_stream_base:      .res 3
p_stream_next:      .res 3
p_stream_more:      .res 1
p_stream_slot:      .res 1
p_mark_stream_base: .res 3
p_mark_stream_next: .res 3
p_mark_stream_more: .res 1
p_mark_stream_slot: .res 1
p_include_stack_base_l: .res P_INCLUDE_DEPTH_CAP
p_include_stack_base_m: .res P_INCLUDE_DEPTH_CAP
p_include_stack_base_h: .res P_INCLUDE_DEPTH_CAP
p_include_stack_next_l: .res P_INCLUDE_DEPTH_CAP
p_include_stack_next_m: .res P_INCLUDE_DEPTH_CAP
p_include_stack_next_h: .res P_INCLUDE_DEPTH_CAP
p_include_stack_more:   .res P_INCLUDE_DEPTH_CAP
p_include_stack_slot:   .res P_INCLUDE_DEPTH_CAP

      .segment "NPCFE_BSS"
p_const_name_len:   .res P_CONST_CAP
p_const_type:       .res P_CONST_CAP
p_const_value_l:    .res P_CONST_CAP
p_const_value_h:    .res P_CONST_CAP
p_type_name_len:    .res P_TYPE_CAP
p_type_kind:        .res P_TYPE_CAP
p_type_size_l:      .res P_TYPE_CAP
p_type_size_h:      .res P_TYPE_CAP
p_type_low_l:       .res P_TYPE_CAP
p_type_low_h:       .res P_TYPE_CAP
p_type_high_l:      .res P_TYPE_CAP
p_type_high_h:      .res P_TYPE_CAP
p_type_element:     .res P_TYPE_CAP
p_field_count:      .res 1
p_field_name_used:  .res 1
p_field_name_offset:.res P_FIELD_CAP
p_field_name_len:   .res P_FIELD_CAP
p_field_type:       .res P_FIELD_CAP
p_field_offset_l:   .res P_FIELD_CAP
p_field_offset_h:   .res P_FIELD_CAP
p_field_size_l:     .res P_FIELD_CAP
p_field_size_h:     .res P_FIELD_CAP
p_field_names:      .res P_FIELD_NAME_POOL_CAP
p_dim_count:        .res 1
p_dim_low_l:        .res P_DIM_CAP
p_dim_low_h:        .res P_DIM_CAP
p_dim_extent_l:     .res P_DIM_CAP
p_dim_extent_h:     .res P_DIM_CAP
p_dim_stride_l:     .res P_DIM_CAP
p_dim_stride_h:     .res P_DIM_CAP

      .segment "NPCFE_CODE"
      .export npcfe_main
      .export p_control_depth
      .export p_emit_error

npcfe_main:
      STZ   p_state_allocated
      STZ   generated_asm_len
      STZ   generated_asm_len+1
      STZ   generated_asm_len+2
      STZ   pascal_error
      STZ   p_source_detail
      LDX   #2
@source_address:
      LDA   LIB_ARG0,X
      STA   source_xaddr,X
      DEX
      BPL   @source_address
      LDA   LIB_ARG1
      STA   source_len
      LDA   LIB_ARG1+1
      STA   source_len+1
      JSR   p_allocate_state
      BEQ   @compile
      STA   p_source_detail
      LDA   #PASCAL_ERR_MEMORY
      STA   pascal_error
      LDA   #1
      STA   p_frontend_status
      BRA   @publish
@compile:
      JSR   pascal_compile
      STA   p_frontend_status
      JSR   p_finalize_output
      JSR   p_release_includes
      BEQ   @release_state
      LDA   p_frontend_status
      BNE   @release_state
      LDA   #PASCAL_ERR_INCLUDE
      STA   pascal_error
      LDA   #1
      STA   p_frontend_status
@release_state:
      JSR   p_release_state
      BEQ   @publish
      STA   p_source_detail
      LDA   p_frontend_status
      BNE   @publish
      LDA   #PASCAL_ERR_MEMORY
      STA   pascal_error
      LDA   #1
      STA   p_frontend_status
@publish:
      LDA   generated_asm_len
      STA   LIB_RESULT
      LDA   generated_asm_len+1
      STA   LIB_RESULT+1
      LDA   pascal_error
      STA   LIB_RESULT+2
      LDA   p_source_detail
      STA   LIB_RESULT+3
      LDA   p_frontend_status
      RTS

; NovaPascal's frontend is a conventional recursive-descent compiler. It emits
; typed accumulator code while parsing, so the native compiler needs no AST.
; Byte/Boolean/Char values use A; Word values use A=low and X=high.
pascal_compile:
      STZ   pascal_error
      STZ   generated_asm_len
      STZ   generated_asm_len+1
      STZ   generated_asm_len+2
      STZ   p_output_open
      STZ   p_output_used
      STZ   p_output_used+1
      STZ   p_unit_count
      STZ   p_external_macro_emitted
      STZ   p_in_argument
      STZ   p_symbol_count
      STZ   p_label
      STZ   p_label+1
      STZ   p_function_active
      STZ   p_compiling_unit
      STZ   p_interface_count
      STZ   p_interface_index
      STZ   p_control_depth
      STZ   p_operator_depth
      STZ   p_array_depth
      STZ   p_window_valid
      STZ   p_source_error
      STZ   p_source_detail
      STZ   p_include_depth
      STZ   p_include_count
      STZ   p_include_name_len
      STZ   p_current_overlay
      STZ   p_const_count
      STZ   p_type_count
      STZ   p_field_count
      STZ   p_field_name_used
      STZ   p_dim_count
      STZ   p_scope_level
      STZ   p_scope_symbol_base
      STZ   p_routine_context_depth
      STZ   p_routine_count
      LDA   #$FF
      STA   p_current_routine
      STZ   p_with_depth
      JSR   p_init_source_name
      LDA   #1
      STA   p_line
      STZ   p_line+1
      STA   p_column
      STZ   p_column+1
      LDA   source_xaddr
      STA   p_src
      LDA   source_xaddr+1
      STA   p_src+1
      LDA   source_xaddr+2
      STA   p_src+2
      LDA   source_len
      STA   p_left
      LDA   source_len+1
      STA   p_left+1
      JSR   p_stream_init
      JSR   p_open_output
      long_bne p_output_error
      LDA   #<asm_import
      STA   p_word
      LDA   #>asm_import
      STA   p_word+1
      JSR   p_emit_text
      BCC   :+
      JMP   p_output_error
:
      JSR   p_capture_identifier
      BCC   :+
      JMP   p_syntax_error
:
      LDA   #<kw_unit
      LDX   #>kw_unit
      JSR   p_ident_is
      BCC   @program_keyword
      JMP   p_compile_unit
@program_keyword:
      LDA   #<kw_program
      LDX   #>kw_program
      JSR   p_ident_is
      BCS   :+
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
      JSR   p_parse_uses
      BCC   @expect_begin
      LDA   p_emit_error
      long_bne p_output_error
      JMP   p_syntax_error
@expect_begin:
      LDA   #<kw_const
      LDX   #>kw_const
      JSR   p_ident_is
      BCC   @type_section
      JSR   p_parse_const_section
      long_bcs p_program_parse_fail
      BRA   @expect_begin
@type_section:
      LDA   #<kw_type
      LDX   #>kw_type
      JSR   p_ident_is
      BCC   @begin_section
      JSR   p_parse_type_section
      long_bcs p_program_parse_fail
      BRA   @expect_begin
@begin_section:
      LDA   #<kw_begin
      LDX   #>kw_begin
      JSR   p_ident_is
      long_bcs @body
      LDA   #<kw_var
      LDX   #>kw_var
      JSR   p_ident_is
      BCC   @after_vars
      JSR   p_parse_global_var_section
      long_bcs p_program_parse_fail
      BRA   @expect_begin
@after_vars:
      LDA   #<kw_begin
      LDX   #>kw_begin
      JSR   p_ident_is
      BCS   @body
      JSR   p_is_routine_keyword
      long_bcc p_syntax_error
      LDA   #<asm_code_with_routines
      LDX   #>asm_code_with_routines
      JSR   p_emit_ax_text
      long_bcs p_output_error
      JSR   p_parse_routine_declarations
      long_bcs p_program_parse_fail
      LDX   p_routine_count
@frame_scan:
      DEX
      BMI   @frameless_main
      LDA   p_routine_kind_table,X
      AND   #P_ROUTINE_HAS_FRAME
      BEQ   @frame_scan
      LDA   #<asm_main_label_frames
      LDX   #>asm_main_label_frames
      JSR   p_emit_ax_text
      long_bcs p_output_error
      BRA   @parse_body
@frameless_main:
      LDA   #<asm_main_label
      LDX   #>asm_main_label
      JSR   p_emit_ax_text
      long_bcs p_output_error
      BRA   @parse_body
@body:
      LDA   #<asm_code
      STA   p_word
      LDA   #>asm_code
      STA   p_word+1
      JSR   p_emit_text
      long_bcs p_output_error
      LDA   #<asm_main_label
      LDX   #>asm_main_label
      JSR   p_emit_ax_text
      long_bcs p_output_error
@parse_body:
      JSR   p_parse_statement_list
      BCS   p_program_parse_fail
      LDA   #'.'
      JSR   p_expect_char
      long_bcs p_syntax_error
      JSR   p_skip_ws
      JSR   p_peek
      long_bcs p_syntax_error

      LDA   #<asm_rts
      STA   p_word
      LDA   #>asm_rts
      STA   p_word+1
      JSR   p_emit_text
      long_bcs p_output_error
p_finish_compile:
      JSR   p_emit_unit_sources
      long_bcs p_output_error
      LDA   #<asm_import_footer
      LDX   #>asm_import_footer
      JSR   p_emit_ax_text
      long_bcs p_output_error
      LDA   #PASCAL_OK
      RTS

p_program_parse_fail:
      LDA   p_emit_error
      long_bne p_output_error
      JMP   p_syntax_error

; USES has already been captured. It accepts generated Nova* NDK units and
; project-owned Pascal units, then leaves the next identifier in p_ident.
p_parse_uses:
@unit:
      JSR   p_capture_identifier
      long_bcs @fail
      JSR   p_store_unit
      BCC   @delimiter
      SEC
      RTS
@delimiter:
      JSR   p_skip_ws
      JSR   p_peek
      long_bcc @fail
      CMP   #','
      BNE   @done
      JSR   p_next
      BRA   @unit
@done:
      LDA   #';'
      JSR   p_expect_char
      BCS   @fail
      JSR   p_capture_identifier
@fail:
      RTS

; unit-file = UNIT name ; INTERFACE [USES ... ;]
;             {procedure/function declarations}
;             IMPLEMENTATION [USES ... ;]
;             {procedure/function definitions} END .
;
; This first native unit ABI intentionally shares the compiler's existing
; no-argument Procedure and Byte/Boolean Function contract. Interface and
; implementation declarations must match in order, which keeps validation
; deterministic and compact on the 65C02.
p_compile_unit:
      INC   p_compiling_unit
      JSR   p_capture_identifier
      long_bcs p_syntax_error
      LDA   #';'
      JSR   p_expect_char
      long_bcs p_syntax_error
      JSR   p_capture_identifier
      long_bcs p_syntax_error
      LDA   #<kw_interface
      LDX   #>kw_interface
      JSR   p_ident_is
      long_bcc p_syntax_error
      JSR   p_capture_identifier
      long_bcs p_syntax_error
      LDA   #<kw_uses
      LDX   #>kw_uses
      JSR   p_ident_is
      BCC   @interface_item
      JSR   p_parse_uses
      long_bcs @parse_fail

@interface_item:
      LDA   #<kw_implementation
      LDX   #>kw_implementation
      JSR   p_ident_is
      long_bcs @implementation
      LDA   #<kw_procedure
      LDX   #>kw_procedure
      JSR   p_ident_is
      BCC   @interface_function
      STZ   p_digit
      BRA   @interface_name
@interface_function:
      LDA   #<kw_function
      LDX   #>kw_function
      JSR   p_ident_is
      long_bcc p_syntax_error
      LDA   #2
      STA   p_digit
@interface_name:
      JSR   p_capture_identifier
      long_bcs p_syntax_error
      JSR   p_save_identifier
      LDA   NPP_PLAN_BASE+NPP_PLAN_TARGET
      CMP   #NPP_TARGET_UNIT
      BNE   @interface_visibility_done
      JSR   p_emit_global_saved
      long_bcs p_output_error
@interface_visibility_done:
      JSR   p_skip_ws
      JSR   p_peek
      long_bcc p_syntax_error
      CMP   #'('
      BNE   @interface_type
      JSR   p_next
      LDA   #')'
      JSR   p_expect_char
      long_bcs p_syntax_error
@interface_type:
      LDA   p_digit
      BEQ   @interface_end
      LDA   #':'
      JSR   p_expect_char
      long_bcs p_syntax_error
      JSR   p_capture_identifier
      long_bcs p_syntax_error
      LDA   #<kw_byte
      LDX   #>kw_byte
      JSR   p_ident_is
      BCS   @interface_end
      LDA   #<kw_boolean
      LDX   #>kw_boolean
      JSR   p_ident_is
      long_bcc p_syntax_error
@interface_end:
      JSR   p_store_interface_routine
      long_bcs p_syntax_error
      LDA   #';'
      JSR   p_expect_char
      long_bcs p_syntax_error
      JSR   p_capture_identifier
      long_bcs p_syntax_error
      JMP   @interface_item

@implementation:
      JSR   p_capture_identifier
      long_bcs p_syntax_error
      LDA   #<kw_uses
      LDX   #>kw_uses
      JSR   p_ident_is
      BCC   @implementation_item
      JSR   p_parse_uses
      BCS   @parse_fail
@implementation_item:
      LDA   #<kw_end
      LDX   #>kw_end
      JSR   p_ident_is
      BCS   @unit_end
      JSR   p_is_routine_keyword
      long_bcc p_syntax_error
      LDA   #<asm_code
      LDX   #>asm_code
      JSR   p_emit_ax_text
      long_bcs p_output_error
      JSR   p_parse_routine_declarations
      BCS   @parse_fail
@unit_end:
      LDA   p_interface_index
      CMP   p_interface_count
      long_bne p_syntax_error
      LDA   #'.'
      JSR   p_expect_char
      long_bcs p_syntax_error
      JSR   p_skip_ws
      JSR   p_peek
      long_bcs p_syntax_error
      JMP   p_finish_compile

@parse_fail:
      LDA   p_emit_error
      long_bne p_output_error
      JMP   p_syntax_error

p_store_interface_routine:
      LDX   p_interface_count
      CPX   #16
      BCS   @fail
      LDA   p_digit
      STA   p_interface_sig,X
      LDA   p_saved_hash
      STA   p_interface_hash0,X
      LDA   p_saved_hash+1
      STA   p_interface_hash1,X
      LDA   p_saved_hash+2
      STA   p_interface_hash2,X
      INC   p_interface_count
      CLC
      RTS
@fail:
      SEC
      RTS

p_validate_interface_routine:
      LDX   p_interface_index
      CPX   p_interface_count
      BCS   @fail
      LDA   p_routine_kind
      CMP   p_interface_sig,X
      BNE   @fail
      LDA   p_saved_hash
      CMP   p_interface_hash0,X
      BNE   @fail
      LDA   p_saved_hash+1
      CMP   p_interface_hash1,X
      BNE   @fail
      LDA   p_saved_hash+2
      CMP   p_interface_hash2,X
      BNE   @fail
      INC   p_interface_index
      CLC
      RTS
@fail:
      SEC
      RTS

p_syntax_error:
      LDA   #PASCAL_ERR_SYNTAX
      BRA   p_compile_fail
p_output_error:
      LDA   #PASCAL_ERR_OUTPUT
p_compile_fail:
      LDX   p_source_error
      BEQ   :+
      TXA
:
      STA   pascal_error
      JSR   p_publish_diag_source
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

; Parse a top-level VAR section and leave the first following declaration or
; BEGIN keyword captured in p_ident. Includes may introduce these sections
; between routine declarations exactly as if their text lived in the program.
p_parse_global_var_section:
      STZ   p_scope_symbol_base
      LDA   #<asm_bss
      LDX   #>asm_bss
      JSR   p_emit_ax_text
      BCS   @output
      JSR   p_capture_identifier
      BCS   @fail
@group:
      LDA   p_symbol_count
      STA   p_symbol_group
@name:
      JSR   p_symbol_add
      BCS   @fail
      JSR   p_skip_ws
      JSR   p_peek
      long_bcc @fail
      CMP   #','
      BNE   @type
      JSR   p_next
      JSR   p_capture_identifier
      BCS   @fail
      BRA   @name
@type:
      LDA   #':'
      JSR   p_expect_char
      BCS   @fail
      JSR   p_parse_decl_type
      BCS   @fail
      JSR   p_emit_symbol_group
      BCS   @output
      LDA   #';'
      JSR   p_expect_char
      BCS   @fail
      JSR   p_capture_identifier
      BCS   @fail
      LDA   #<kw_begin
      LDX   #>kw_begin
      JSR   p_ident_is
      BCS   @done
      LDA   #<kw_const
      LDX   #>kw_const
      JSR   p_ident_is
      BCS   @done
      LDA   #<kw_type
      LDX   #>kw_type
      JSR   p_ident_is
      BCS   @done
      JSR   p_is_routine_keyword
      BCS   @done
      BRA   @group
@done:
      LDA   #<asm_code
      LDX   #>asm_code
      JSR   p_emit_ax_text
      BCS   @output
      CLC
      RTS
@output:
      INC   p_emit_error
@fail:
      SEC
      RTS

; Pascal calls subroutines procedures; functions additionally return one Byte
; in A. This slice keeps routine parameters out until a program needs them,
; while globals provide enough structure to split Life into readable phases.
p_is_routine_keyword:
      LDA   #<kw_procedure
      LDX   #>kw_procedure
      JSR   p_ident_is
      BCS   @yes
      LDA   #<kw_function
      LDX   #>kw_function
      JSR   p_ident_is
@yes:
      RTS

p_routine_add:
      LDX   p_routine_count
      CPX   #P_ROUTINE_CAP
      BCS   @fail
      LDA   p_saved_ident_len
      STA   p_routine_name_len,X
      LDA   p_saved_hash
      STA   p_routine_hash0,X
      LDA   p_saved_hash+1
      STA   p_routine_hash1,X
      LDA   p_saved_hash+2
      STA   p_routine_hash2,X
      LDA   p_current_routine
      STA   p_routine_owner,X
      LDA   p_routine_kind
      STA   p_routine_kind_table,X
      LDA   p_current_overlay
      STA   p_routine_overlay,X
      LDA   #TYPE_BYTE
      STA   p_routine_return,X
      STZ   p_routine_params,X
      STZ   p_routine_refmask,X
      STZ   p_routine_widthmask,X
      STX   p_current_routine
      INC   p_routine_count
      CLC
@fail:
      RTS

; Find the innermost declared routine matching p_call_hash. A candidate is
; visible when it is the current routine, is owned by the current routine, or
; belongs to one of the current routine's lexical ancestors.
p_routine_find_call:
      LDX   p_routine_count
@candidate:
      DEX
      BMI   @missing
      LDA   p_routine_name_len,X
      CMP   p_call_ident_len
      BNE   @candidate
      LDA   p_routine_hash0,X
      CMP   p_call_hash
      BNE   @candidate
      LDA   p_routine_hash1,X
      CMP   p_call_hash+1
      BNE   @candidate
      LDA   p_routine_hash2,X
      CMP   p_call_hash+2
      BNE   @candidate
      STX   p_routine_iter
      CPX   p_current_routine
      BEQ   @found
      LDA   p_routine_owner,X
      CMP   #$FF
      BEQ   @found
      STA   p_char
      LDX   p_current_routine
@ancestor:
      CPX   #$FF
      BEQ   @resume
      CPX   p_char
      BEQ   @found_saved
      LDA   p_routine_owner,X
      TAX
      BRA   @ancestor
@resume:
      LDX   p_routine_iter
      BRA   @candidate
@found_saved:
      LDX   p_routine_iter
@found:
      SEC
      RTS
@missing:
      CLC
      RTS

; routine-declarations = (procedure | function) name [()] [: Byte] ;
;                        begin statements end ;
; The current identifier is the first routine keyword. On return, BEGIN for
; the program body has been consumed into p_ident.
p_parse_routine_declarations:
@routine:
      LDA   #<kw_procedure
      LDX   #>kw_procedure
      JSR   p_ident_is
      BCC   @function
      STZ   p_routine_kind
      STZ   p_digit
      BRA   @name
@function:
      LDA   #<kw_function
      LDX   #>kw_function
      JSR   p_ident_is
      long_bcc @fail
      LDA   #2
      STA   p_routine_kind
      STA   p_digit
@name:
      JSR   p_capture_identifier
      long_bcs @fail
      JSR   p_save_identifier
      JSR   p_save_call_identifier
      JSR   p_routine_add
      long_bcs @fail
      LDA   p_symbol_count
      STA   p_scope_symbol_base
      INC   p_scope_level
      STZ   p_routine_param_count
      STZ   p_routine_param_refmask
      STZ   p_routine_param_widthmask
      LDA   #2
      STA   p_frame_size
      STZ   p_frame_size+1
      LDA   p_routine_kind
      BNE   @save_function
      STZ   p_function_active
      BRA   @parameters
@save_function:
      LDX   #2
@hash:
      LDA   p_saved_hash,X
      STA   p_function_hash,X
      DEX
      BPL   @hash
      LDA   #1
      STA   p_function_active
@parameters:
      JSR   p_skip_ws
      JSR   p_peek
      long_bcc @fail
      CMP   #'('
      BNE   @return_type
      JSR   p_parse_formal_parameters
      long_bcs @fail
@return_type:
      LDA   p_routine_kind
      BEQ   @header_end
      LDA   #':'
      JSR   p_expect_char
      long_bcs @fail
      JSR   p_parse_decl_type
      long_bcs @fail
      LDA   p_decl_type
      STA   p_function_return_type
      JSR   p_function_result_wide
      BCC   @header_end
      JSR   p_reserve_wide_function_result
      long_bcs @fail
@header_end:
      LDA   p_symbol_count
      STA   p_scope_param_end
      LDX   p_current_routine
      LDA   p_routine_param_count
      STA   p_routine_params,X
      LDA   p_routine_param_refmask
      STA   p_routine_refmask,X
      LDA   p_routine_param_widthmask
      STA   p_routine_widthmask,X
      LDA   p_function_return_type
      STA   p_routine_return,X
      LDA   p_compiling_unit
      BEQ   @interface_done
      JSR   p_validate_interface_routine
      long_bcs @fail
@interface_done:
      LDA   #';'
      JSR   p_expect_char
      long_bcs @fail
      JSR   p_capture_identifier
      long_bcs @fail
      JSR   p_parse_routine_declaration_part
      long_bcs @fail
      JSR   p_restore_call_identifier_to_saved
      LDA   #<kw_begin
      LDX   #>kw_begin
      JSR   p_ident_is
      long_bcc @fail
      LDX   p_current_routine
      LDA   p_routine_owner,X
      CMP   #$FF
      BNE   @signature_done
      LDA   p_routine_param_count
      CMP   #2
      BCS   @wide_signature
      CLC
      ADC   p_routine_kind
      JSR   p_emit_local_signature
      long_bcs @output
      BRA   @signature_done
@wide_signature:
      JSR   p_emit_local_pascal_signature
      long_bcs @output
@signature_done:
      JSR   p_emit_saved_label
      long_bcs @output
      JSR   p_emit_frame_enter
      long_bcs @output
      LDA   p_function_active
      BEQ   @body
      JSR   p_function_result_wide
      BCC   @byte_function_entry
      LDA   #<asm_function_entry_wide
      LDX   #>asm_function_entry_wide
      BRA   @emit_function_entry
@byte_function_entry:
      LDA   #<asm_function_entry
      LDX   #>asm_function_entry
@emit_function_entry:
      JSR   p_emit_ax_text
      long_bcs @output
@body:
      JSR   p_parse_statement_list
      long_bcs @fail
      LDA   #';'
      JSR   p_expect_char
      long_bcs @fail
      LDA   p_function_active
      BEQ   @procedure_end
      JSR   p_emit_frame_leave
      long_bcs @output
      JSR   p_function_result_wide
      BCC   @byte_function_return
      LDA   #<asm_function_return_wide
      LDX   #>asm_function_return_wide
      BRA   @emit_end
@byte_function_return:
      LDA   #<asm_function_return
      LDX   #>asm_function_return
      BRA   @emit_end
@procedure_end:
      JSR   p_emit_frame_leave
      long_bcs @output
      LDA   #<asm_routine_return
      LDX   #>asm_routine_return
@emit_end:
      JSR   p_emit_ax_text
      long_bcs @output
      STZ   p_function_active
      LDA   p_scope_symbol_base
      STA   p_symbol_count
      DEC   p_scope_level
      LDX   p_current_routine
      LDA   p_routine_owner,X
      STA   p_current_routine
      JSR   p_capture_identifier
      BCS   @fail
@declaration:
      LDA   p_compiling_unit
      BEQ   :+
      LDA   #<kw_end
      LDX   #>kw_end
      JSR   p_ident_is
      BCS   @done
:
      LDA   #<kw_begin
      LDX   #>kw_begin
      JSR   p_ident_is
      BCS   @done
      LDA   #<kw_const
      LDX   #>kw_const
      JSR   p_ident_is
      BCC   @type_declaration
      JSR   p_parse_const_section
      BCS   @fail
      BRA   @declaration
@type_declaration:
      LDA   #<kw_type
      LDX   #>kw_type
      JSR   p_ident_is
      BCC   @var_declaration
      JSR   p_parse_type_section
      BCS   @fail
      BRA   @declaration
@var_declaration:
      LDA   #<kw_var
      LDX   #>kw_var
      JSR   p_ident_is
      BCC   @next_routine
      JSR   p_parse_global_var_section
      BCS   @fail
      BRA   @declaration
@next_routine:
      JSR   p_is_routine_keyword
      long_bcs @routine
@fail:
      SEC
      RTS
@output:
      INC   p_emit_error
      SEC
      RTS
@done:
      CLC
      RTS

; Parse statements through END. The caller has already consumed BEGIN.
p_parse_statement_list:
@next:
      JSR   p_skip_ws
      JSR   p_peek
      long_bcc @fail
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
      long_bcc @fail
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
      LDA   #<kw_repeat
      LDX   #>kw_repeat
      JSR   p_ident_is
      long_bcs p_parse_repeat
      LDA   #<kw_for
      LDX   #>kw_for
      JSR   p_ident_is
      long_bcs p_parse_for
      LDA   #<kw_with
      LDX   #>kw_with
      JSR   p_ident_is
      long_bcs p_parse_with
      LDA   #<kw_case
      LDX   #>kw_case
      JSR   p_ident_is
      long_bcs p_parse_case
      LDA   #<kw_writeln
      LDX   #>kw_writeln
      JSR   p_ident_is
      long_bcs p_parse_writeln
      LDA   #<kw_write
      LDX   #>kw_write
      JSR   p_ident_is
      long_bcs p_parse_write
      LDA   #<kw_str
      LDX   #>kw_str
      JSR   p_ident_is
      long_bcs p_parse_str
      LDA   #<kw_asm
      LDX   #>kw_asm
      JSR   p_ident_is
      long_bcs p_parse_asm

      JSR   p_save_identifier
      STZ   p_target_type
      STZ   p_target_flags
      STZ   p_target_with
      JSR   p_skip_ws
      JSR   p_peek
      long_bcc @fail
      CMP   #'('
      long_beq @call
      CMP   #'['
      BEQ   @designator_assignment
      CMP   #'.'
      BEQ   @designator_assignment
      CMP   #':'
      long_bne @bare_call
      JSR   p_symbol_find
      BCC   @with_assignment
      STX   p_target_symbol
      STA   p_target_type
      JSR   p_save_target_storage
      CMP   #TYPE_ARRAY_BYTE
      long_beq @fail
      BRA   @assignment
@with_assignment:
      JSR   p_with_field_find
      BCC   @assignment
      STX   p_target_symbol
      STA   p_target_type
      LDA   p_with_slot
      STA   p_target_with_slot
      LDA   #1
      STA   p_target_with
@assignment:
      JSR   p_next
@equals:
      LDA   #'='
      JSR   p_expect_char
      long_bcs @fail
      JSR   p_parse_expression
      long_bcs @fail
      JSR   p_emit_store_saved
      long_bcs @output
      CLC
      RTS
@designator_assignment:
      JSR   p_parse_designator_address
      long_bcs @fail
      LDA   p_designator_type
      STA   p_target_type
      LDA   #<asm_phx_pha
      LDX   #>asm_phx_pha
      JSR   p_emit_ax_text
      long_bcs @output
      LDA   #':'
      JSR   p_expect_char
      long_bcs @fail
      LDA   #'='
      JSR   p_expect_char
      long_bcs @fail
      JSR   p_parse_expression
      long_bcs @fail
      JSR   p_emit_store_designator
      long_bcs @output
      CLC
      RTS
@call:
      JSR   p_next
      JSR   p_count_call_arguments
      long_bcs @fail
      STA   p_call_arity
      JSR   p_saved_call_metadata
      JSR   p_saved_call_force_stack
      STA   p_statement_force_stack
      JSR   p_saved_call_is_read
      STA   p_read_call
      JSR   p_saved_call_is_val
      STA   p_val_call
      STZ   p_read_size
      STZ   p_read_size+1
      STZ   p_val_size
      STZ   p_argument_count
      STZ   p_argument_bytes
      LDA   p_statement_force_stack
      BEQ   @default_done
      LDA   #<asm_default_kbd_push
      LDX   #>asm_default_kbd_push
      JSR   p_emit_ax_text
      long_bcs @output
      LDA   #2
      STA   p_argument_bytes
@default_done:
      JSR   p_skip_ws
      JSR   p_peek
      long_bcc @fail
      CMP   #')'
      BEQ   @call_close
@argument:
      JSR   p_parse_statement_argument
      long_bcs @fail
      JSR   p_emit_coerce_argument
      long_bcs @output
      INC   p_argument_count
      LDA   p_argument_count
      CMP   #9
      long_bcs @fail
      JSR   p_skip_ws
      JSR   p_peek
      long_bcc @fail
      CMP   #','
      BNE   @last_argument
      JSR   p_emit_push_argument
      long_bcs @output
      JSR   p_next
      BRA   @argument
@last_argument:
      LDA   p_argument_count
      CMP   #2
      BCS   @push_last
      LDA   p_statement_force_stack
      BEQ   @call_close
@push_last:
      JSR   p_emit_push_argument
      long_bcs @output
      LDA   p_read_call
      BEQ   @val_size
      JSR   p_emit_read_size_argument
      long_bcs @output
@val_size:
      LDA   p_val_call
      BEQ   @call_close
      JSR   p_emit_val_size_argument
      long_bcs @output
@call_close:
      LDA   #')'
      JSR   p_expect_char
      long_bcs @fail
      JSR   p_routine_find_saved
      BCC   @external_call
      LDA   p_routine_kind_table,X
      AND   #$3F
      BNE   @fail
      LDA   p_routine_params,X
      CMP   p_argument_count
      BNE   @fail
      STX   p_routine_iter
      JSR   p_emit_local_call_saved
      BCS   @output
@call_done:
      CLC
      RTS
@external_call:
      LDA   p_argument_count
      CMP   #2
      BCS   @pascal_call
      JSR   p_emit_sig_saved
      BCS   @output
      JSR   p_emit_call_saved
      BCS   @output
      LDA   p_statement_force_stack
      BEQ   :+
      JSR   p_emit_drop_arguments
      BCS   @output
:
      CLC
      RTS
@pascal_call:
      JSR   p_emit_pascal_sig_saved
      BCS   @output
      JSR   p_emit_call_saved
      BCS   @output
      JSR   p_emit_drop_arguments
      BCS   @output
      CLC
      RTS
@bare_call:
      JSR   p_routine_find_saved
      BCC   @external_bare_call
      LDA   p_routine_kind_table,X
      AND   #$3F
      BNE   @fail
      LDA   p_routine_params,X
      BNE   @fail
      STX   p_routine_iter
      JSR   p_emit_local_call_saved
      BCS   @output
      CLC
      RTS
@external_bare_call:
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

; Determine a call's arity before emitting any argument code. This lets the
; overloaded classic READ/READLN forms describe different VAR positions
; without buffering generated assembly.
p_count_call_arguments:
      JSR   p_mark_position
      STZ   p_call_arity
      STZ   p_scan_depth
      STZ   p_scan_quote
@scan:
      JSR   p_next
      long_bcc @bad
      LDX   p_scan_quote
      BEQ   @plain
      CMP   #$27
      long_bne @scan
      JSR   p_peek
      BCC   @quote_end
      CMP   #$27
      BNE   @quote_end
      JSR   p_next
      JMP   @scan
@quote_end:
      STZ   p_scan_quote
      JMP   @scan
@plain:
      CMP   #$27
      BEQ   @quote_start
      CMP   #' '
      long_beq @scan
      CMP   #$09
      long_beq @scan
      CMP   #$0D
      long_beq @scan
      CMP   #$0A
      long_beq @scan
      CMP   #$7B
      BEQ   @brace_comment
      CMP   #'('
      BEQ   @open_or_comment
      CMP   #'['
      BEQ   @open
      CMP   #']'
      BEQ   @nested_close
      CMP   #')'
      BEQ   @close
      CMP   #','
      BNE   @token
      LDA   p_scan_depth
      long_bne @scan
      INC   p_call_arity
      JMP   @scan
@quote_start:
      LDA   #1
      STA   p_scan_quote
      BRA   @token
@open_or_comment:
      JSR   p_peek
      long_bcc @bad
      CMP   #'*'
      BNE   @open
      JSR   p_next
@paren_comment:
      JSR   p_next
      long_bcc @bad
      CMP   #'*'
      BNE   @paren_comment
      JSR   p_peek
      long_bcc @bad
      CMP   #')'
      BNE   @paren_comment
      JSR   p_next
      JMP   @scan
@brace_comment:
      JSR   p_next
      long_bcc @bad
      CMP   #'}'
      BNE   @brace_comment
      JMP   @scan
@open:
      INC   p_scan_depth
      JMP   @token
@nested_close:
      LDA   p_scan_depth
      long_beq @bad
      DEC   p_scan_depth
      JMP   @token
@close:
      LDA   p_scan_depth
      BEQ   @done
      DEC   p_scan_depth
@token:
      LDA   p_call_arity
      long_bne @scan
      INC   p_call_arity
      JMP   @scan
@done:
      LDA   p_call_arity
      PHA
      JSR   p_restore_position
      PLA
      CLC
      RTS
@bad:
      JSR   p_restore_position
      SEC
      RTS

; Parse one actual parameter according to the selected routine's VAR mask.
; Reference arguments are restricted to assignable designators and always
; use the two-byte pointer ABI, regardless of the designated scalar's size.
p_parse_statement_argument:
      LDX   p_argument_count
      LDA   p_argument_bits,X
      AND   p_statement_refmask
      BEQ   @value
      JSR   p_capture_identifier
      BCS   @fail
      JSR   p_parse_designator_address
      BCS   @fail
      LDA   p_read_call
      BEQ   @val
      LDA   p_argument_count
      INC
      CMP   p_call_arity
      BNE   @pointer
      LDA   p_designator_type
      JSR   p_type_size
      BCS   @fail
      LDA   p_array_len
      STA   p_read_size
      LDA   p_array_len+1
      STA   p_read_size+1
@val:
      LDA   p_val_call
      BEQ   @pointer
      LDA   p_argument_count
      CMP   #1
      BNE   @pointer
      LDA   p_designator_type
      JSR   p_type_size
      BCS   @fail
      LDA   p_array_len+1
      BNE   @fail
      LDA   p_array_len
      CMP   #2
      BEQ   @save_val_size
      CMP   #4
      BNE   @fail
@save_val_size:
      STA   p_val_size
@pointer:
      LDA   #TYPE_WORD
      STA   p_expr_type
      CLC
      RTS
@value:
      JMP   p_parse_expression
@fail:
      SEC
      RTS

; Resolve reference and word-width semantics together. Keeping one metadata
; table avoids duplicate name hashes for routines such as Delete and XRamRead.
p_saved_call_metadata:
      STZ   p_statement_refmask
      STZ   p_statement_widthmask
      JSR   p_routine_find_saved
      BCC   @system
      LDA   p_routine_refmask,X
      STA   p_statement_refmask
      LDA   p_routine_widthmask,X
      STA   p_statement_widthmask
      RTS
@system:
      LDX   #0
@entry:
      CPX   #SYSTEM_CALL_METADATA_COUNT
      BCS   @done
      LDA   p_saved_hash
      CMP   system_call_hash0,X
      BNE   @next
      LDA   p_saved_hash+1
      CMP   system_call_hash1,X
      BNE   @next
      LDA   p_saved_hash+2
      CMP   system_call_hash2,X
      BNE   @next
      LDA   p_call_arity
      CMP   system_call_arity,X
      BNE   @next
      LDA   system_call_refmask,X
      STA   p_statement_refmask
      LDA   system_call_widthmask,X
      STA   p_statement_widthmask
@done:
      RTS
@next:
      INX
      BRA   @entry

p_emit_coerce_argument:
      LDX   p_argument_count
      LDA   p_argument_bits,X
      AND   p_statement_widthmask
      BEQ   @done
      LDA   p_expr_type
      JSR   p_type_size
      BCS   @fail
      LDA   p_array_len+1
      BNE   @done
      LDA   p_array_len
      CMP   #1
      BNE   @done
      LDA   #<asm_index_high_zero
      LDX   #>asm_index_high_zero
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   #TYPE_WORD
      STA   p_expr_type
@done:
      CLC
@fail:
      RTS

; The one-argument READ/READLN forms use the same stack pointer position as
; their two-argument forms. The device argument is optional, not a different
; callee ABI. Local routines named READ still retain normal Pascal calling.
p_saved_call_force_stack:
      JSR   p_routine_find_saved
      BCS   @no
      LDA   p_call_arity
      CMP   #1
      BNE   @no
      LDA   p_saved_hash
      CMP   #$C1
      BNE   @readln
      LDA   p_saved_hash+1
      CMP   #$34
      BNE   @readln
      LDA   p_saved_hash+2
      CMP   #$8B
      BEQ   @yes
@readln:
      LDA   p_saved_hash
      CMP   #$1B
      BNE   @no
      LDA   p_saved_hash+1
      CMP   #$73
      BNE   @no
      LDA   p_saved_hash+2
      CMP   #$2B
      BNE   @no
@yes:
      LDA   #1
      RTS
@no:
      LDA   #0
      RTS

p_saved_call_is_read:
      JSR   p_routine_find_saved
      BCS   @no
      LDA   p_saved_hash
      CMP   #$C1
      BNE   @readln
      LDA   p_saved_hash+1
      CMP   #$34
      BNE   @readln
      LDA   p_saved_hash+2
      CMP   #$8B
      BEQ   @yes
@readln:
      LDA   p_saved_hash
      CMP   #$1B
      BNE   @no
      LDA   p_saved_hash+1
      CMP   #$73
      BNE   @no
      LDA   p_saved_hash+2
      CMP   #$2B
      BNE   @no
@yes:
      LDA   #1
      RTS
@no:
      LDA   #0
      RTS

p_saved_call_is_val:
      JSR   p_routine_find_saved
      BCS   @no
      LDA   p_saved_hash
      CMP   #$68
      BNE   @no
      LDA   p_saved_hash+1
      CMP   #$29
      BNE   @no
      LDA   p_saved_hash+2
      CMP   #$88
      BNE   @no
      LDA   p_call_arity
      CMP   #3
      BNE   @no
      LDA   #1
      RTS
@no:
      LDA   #0
      RTS

p_emit_read_size_argument:
      LDA   p_read_size
      STA   p_decimal
      LDA   p_read_size+1
      STA   p_decimal+1
      JSR   p_emit_load_word
      BCS   @fail
      LDA   #<asm_phx_pha
      LDX   #>asm_phx_pha
      JSR   p_emit_ax_text
      BCS   @fail
      INC   p_argument_bytes
      INC   p_argument_bytes
      CLC
@fail:
      RTS

p_emit_val_size_argument:
      LDA   #<asm_lda
      LDX   #>asm_lda
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   p_val_size
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   #$0A
      JSR   p_emit
      BCS   @fail
      LDA   #<asm_pha
      LDX   #>asm_pha
      JSR   p_emit_ax_text
      BCS   @fail
      INC   p_argument_bytes
      CLC
@fail:
      RTS

; Copy a line-oriented ASM block directly to NAS. A leading space keeps each
; source line opaque to NPO2 while remaining ordinary NAS layout. END must be
; the first Pascal token on its own line; the statement-list parser owns its
; optional trailing semicolon.
p_parse_asm:
@header:
      JSR   p_peek
      BCC   @fail
      CMP   #' '
      BEQ   @header_eat
      CMP   #$09
      BEQ   @header_eat
      CMP   #$0D
      BEQ   @header_eat
      CMP   #$0A
      BNE   @fail
      JSR   p_next
      BRA   @line
@header_eat:
      JSR   p_next
      BRA   @header
@line:
      LDA   #<kw_end
      LDX   #>kw_end
      JSR   p_peek_keyword
      BCS   @end
      LDA   #' '
      JSR   p_emit
      BCS   @output
@copy:
      JSR   p_peek
      BCC   @fail
      CMP   #$0D
      BEQ   @skip
      CMP   #$0A
      BEQ   @newline
      JSR   p_next
      JSR   p_emit
      BCS   @output
      BRA   @copy
@skip:
      JSR   p_next
      BRA   @copy
@newline:
      JSR   p_next
      LDA   #$0A
      JSR   p_emit
      BCS   @output
      BRA   @line
@end:
      LDA   #<kw_end
      STA   p_word
      LDA   #>kw_end
      STA   p_word+1
      JSR   p_expect_word
      RTS
@output:
      INC   p_emit_error
@fail:
      SEC
      RTS

      .segment "NPCFE_CODE"
p_parse_writeln:
      LDA   #1
      STA   p_write_newline
      BRA   p_parse_write_common

p_parse_write:
      STZ   p_write_newline

p_parse_write_common:
      STZ   p_write_file
      STZ   p_write_file_text
      STZ   p_write_file_element
      JSR   p_skip_ws
      JSR   p_peek
      long_bcc @bare
      CMP   #'('
      long_bne @bare
      JSR   p_next
      LDA   p_write_newline
      BEQ   @first
      JSR   p_count_call_arguments
      long_bcs @fail
      CMP   #1
      BNE   @first
      JSR   p_skip_ws
      JSR   p_peek
      long_bcc @fail
      CMP   #$27
      BNE   @first
      JSR   p_next
      JSR   p_scan_string_length
      long_bcs @fail
      LDA   p_string_len
      CMP   #3
      BCC   @short_string
      JSR   p_emit_line_string
      BRA   @literal_done
@short_string:
      JSR   p_emit_short_string
@literal_done:
      long_bcs @output
      STZ   p_write_newline
      JMP   @close
@first:
      JSR   p_skip_ws
      JSR   p_peek
      long_bcc @fail
      CMP   #')'
      long_beq @close
@item:
      JSR   p_parse_expression
      long_bcs @fail
      LDA   p_expr_type
      STA   p_write_type
      LDA   p_write_file
      BEQ   @classify
      LDA   p_write_file_text
      BNE   @classify
      JSR   p_classify_typed_file_item
      long_bcs @fail
      BRA   @classified
@classify:
      JSR   p_classify_write_type
      long_bcs @fail
@classified:
      STA   p_write_kind
      CMP   #WRITE_FILE
      BNE   @format
      LDA   p_write_file
      long_bne @fail
      JSR   p_configure_write_file
      long_bcs @fail
      LDA   #<asm_phx_pha
      LDX   #>asm_phx_pha
      JSR   p_emit_ax_text
      long_bcs @output
      INC   p_write_file
      JMP   @delimiter
@format:
      JSR   p_skip_ws
      JSR   p_peek
      long_bcc @fail
      CMP   #':'
      long_bne @write_value
      LDA   p_write_file
      long_bne @fail
      LDA   p_write_kind
      CMP   #WRITE_WORD
      BEQ   @format_kind_ok
      CMP   #WRITE_REAL
      long_bne @fail
@format_kind_ok:
      LDA   #<asm_phx_pha
      LDX   #>asm_phx_pha
      JSR   p_emit_ax_text
      long_bcs @output
      JSR   p_next
      JSR   p_parse_expression
      long_bcs @fail
      JSR   p_emit_push_word_value
      long_bcs @output
      STZ   p_write_size
      JSR   p_skip_ws
      JSR   p_peek
      long_bcc @fail
      CMP   #':'
      BNE   @formatted_value
      LDA   p_write_kind
      CMP   #WRITE_REAL
      long_bne @fail
      JSR   p_next
      JSR   p_parse_expression
      long_bcs @fail
      JSR   p_emit_push_word_value
      long_bcs @output
      INC   p_write_size
@formatted_value:
      JSR   p_emit_formatted_write_value
      long_bcs @output
      BRA   @delimiter
@write_value:
      JSR   p_emit_write_value
      long_bcs @output
@delimiter:
      JSR   p_skip_ws
      JSR   p_peek
      long_bcc @fail
      CMP   #','
      BNE   @close
      JSR   p_next
      JMP   @item
@close:
      LDA   #')'
      JSR   p_expect_char
      long_bcs @fail
@bare:
      LDA   p_write_newline
      BEQ   @drop_file
      LDA   p_write_file
      BEQ   @console_newline
      LDA   p_write_file_text
      long_beq @fail
      LDA   #<asm_file_write_newline
      LDX   #>asm_file_write_newline
      JSR   p_emit_ax_text
      long_bcs @output
      BRA   @drop_file
@console_newline:
      LDA   #$0A
      JSR   p_emit_print_char
      long_bcs @output
@drop_file:
      LDA   p_write_file
      BEQ   @done
      LDA   #<asm_drop_word
      LDX   #>asm_drop_word
      JSR   p_emit_ax_text
      long_bcs @output
@done:
      CLC
      RTS
@output:
      INC   p_emit_error
@fail:
      SEC
      RTS

; Turbo Pascal STR formats a numeric value into a short string. Its colons
; are part of this statement's argument syntax, not general expressions.
p_parse_str:
      LDA   #'('
      JSR   p_expect_char
      long_bcs @fail
      JSR   p_parse_expression
      long_bcs @fail
      LDA   p_expr_type
      JSR   p_type_is_real
      long_bcc @fail
      LDA   #<asm_phx_pha
      LDX   #>asm_phx_pha
      JSR   p_emit_ax_text
      long_bcs @output
      LDA   #':'
      JSR   p_expect_char
      long_bcs @fail
      JSR   p_parse_expression
      long_bcs @fail
      JSR   p_emit_push_word_value
      long_bcs @output
      STZ   p_digit
      JSR   p_skip_ws
      JSR   p_peek
      long_bcc @fail
      CMP   #':'
      BNE   @destination
      JSR   p_next
      JSR   p_parse_expression
      long_bcs @fail
      JSR   p_emit_push_word_value
      long_bcs @output
      INC   p_digit
@destination:
      LDA   #','
      JSR   p_expect_char
      long_bcs @fail
      JSR   p_capture_identifier
      long_bcs @fail
      JSR   p_parse_designator_address
      long_bcs @fail
      LDA   p_designator_type
      JSR   p_type_is_string
      long_bcc @fail
      LDA   p_digit
      BEQ   @plain
      LDA   #<asm_str_real_precision
      LDX   #>asm_str_real_precision
      JSR   p_emit_ax_text
      long_bcs @output
      LDA   #<asm_drop_six
      LDX   #>asm_drop_six
      BRA   @finish
@plain:
      LDA   #<asm_str_real
      LDX   #>asm_str_real
      JSR   p_emit_ax_text
      long_bcs @output
      LDA   #<asm_drop_four
      LDX   #>asm_drop_four
@finish:
      JSR   p_emit_ax_text
      long_bcs @output
      LDA   #')'
      JSR   p_expect_char
      RTS
@output:
      INC   p_emit_error
@fail:
      SEC
      RTS

p_emit_push_word_value:
      LDA   p_expr_type
      CMP   #TYPE_WORD
      BEQ   @push
      LDA   #<asm_index_high_zero
      LDX   #>asm_index_high_zero
      JSR   p_emit_ax_text
      BCS   @fail
@push:
      LDA   #<asm_phx_pha
      LDX   #>asm_phx_pha
      JMP   p_emit_ax_text
@fail:
      RTS

; Collapse concrete and named Pascal types to the output operation they need.
; Named character subranges retain TYPE_CHAR as their element marker.
p_classify_write_type:
      LDA   p_write_type
      CMP   #TYPE_CHAR
      BEQ   @char
      CMP   #TYPE_WORD
      BEQ   @word
      CMP   #TYPE_STRING_REF
      BEQ   @string
      CMP   #TYPE_REAL
      BEQ   @real
      CMP   #TYPE_FILE
      BEQ   @file
      CMP   #TYPE_TEXT
      BEQ   @file
      CMP   #P_TYPE_USER_BASE
      BCC   @byte
      SEC
      SBC   #P_TYPE_USER_BASE
      TAX
      CPX   p_type_count
      BCS   @fail
      LDA   p_type_kind,X
      CMP   #TYPE_KIND_STRING
      BEQ   @string
      CMP   #TYPE_KIND_REAL
      BEQ   @real
      CMP   #TYPE_KIND_FILE
      BEQ   @file
      CMP   #TYPE_KIND_ORDINAL
      BNE   @fail
      LDA   p_type_element,X
      CMP   #TYPE_CHAR
      BEQ   @char
      LDA   p_type_size_h,X
      BNE   @word
      LDA   p_type_size_l,X
      CMP   #2
      BCS   @word
@byte:
      LDA   #WRITE_BYTE
      CLC
      RTS
@char:
      LDA   #WRITE_CHAR
      CLC
      RTS
@word:
      LDA   #WRITE_WORD
      CLC
      RTS
@string:
      LDA   #WRITE_STRING
      CLC
      RTS
@real:
      LDA   #WRITE_REAL
      CLC
      RTS
@file:
      LDA   #WRITE_FILE
      CLC
      RTS
@fail:
      SEC
      RTS

; Preserve whether the leading output file is Text or file-of-T. Typed files
; carry their element type in the ordinary user-type descriptor.
p_configure_write_file:
      LDA   p_write_type
      CMP   #TYPE_TEXT
      BEQ   @text
      CMP   #TYPE_FILE
      BEQ   @untyped
      CMP   #P_TYPE_USER_BASE
      BCC   @fail
      SEC
      SBC   #P_TYPE_USER_BASE
      TAX
      CPX   p_type_count
      BCS   @fail
      LDA   p_type_kind,X
      CMP   #TYPE_KIND_FILE
      BNE   @fail
      LDA   p_type_element,X
      STA   p_write_file_element
      CLC
      RTS
@text:
      INC   p_write_file_text
      LDA   #TYPE_CHAR
      STA   p_write_file_element
@untyped:
      CLC
      RTS
@fail:
      SEC
      RTS

; A typed-file item is emitted as its declared byte representation. MicroCalc
; relies on this for file of CellRec; scalar typed-file support can use the
; same member once scalar-address temporaries are introduced.
p_classify_typed_file_item:
      LDA   p_write_file_element
      BEQ   @size
      CMP   p_write_type
      BNE   @fail
@size:
      LDA   p_write_type
      JSR   p_type_size
      BCS   @fail
      LDA   p_array_len
      STA   p_write_size
      LDA   p_array_len+1
      STA   p_write_size+1
      ORA   p_write_size
      BEQ   @fail
      LDA   p_write_size+1
      BNE   @block
      LDA   p_write_size
      CMP   #3
      BCC   @fail
@block:
      LDA   #WRITE_BLOCK
      CLC
      RTS
@fail:
      SEC
      RTS

p_emit_formatted_write_value:
      LDA   p_write_kind
      CMP   #WRITE_WORD
      BNE   @real
      LDA   #0
      BRA   @mode
@real:
      LDA   p_write_size
      INC
@mode:
      JSR   p_emit_load_byte
      BCS   @fail
      LDA   #<asm_jsr_write_format
      LDX   #>asm_jsr_write_format
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   p_write_size
      BEQ   @drop_four
      LDA   #<asm_drop_six
      LDX   #>asm_drop_six
      JMP   p_emit_ax_text
@drop_four:
      LDA   #<asm_drop_four
      LDX   #>asm_drop_four
      JMP   p_emit_ax_text
@fail:
      SEC
      RTS

p_emit_write_value:
      LDA   p_write_kind
      CMP   #WRITE_BLOCK
      BEQ   @block
      LDA   p_write_file_text
      BNE   p_emit_text_file_value
      LDA   p_write_kind
      CMP   #WRITE_CHAR
      BEQ   @char
      CMP   #WRITE_WORD
      BEQ   @word
      CMP   #WRITE_STRING
      BEQ   @string
      CMP   #WRITE_REAL
      BEQ   @real
      CMP   #WRITE_BYTE
      BNE   @fail
      LDA   p_write_newline
      BEQ   @byte
      LDA   p_call_arity
      CMP   #1
      BNE   @byte
      STZ   p_write_newline
      LDA   #<asm_jsr_write_byte_ln
      LDX   #>asm_jsr_write_byte_ln
      JMP   p_emit_ax_text
@byte:
      LDA   #<asm_jsr_write_byte
      LDX   #>asm_jsr_write_byte
      JMP   p_emit_ax_text
@char:
      LDA   #<asm_jsr
      LDX   #>asm_jsr
      JMP   p_emit_ax_text
@word:
      LDA   #<asm_jsr_write_word
      LDX   #>asm_jsr_write_word
      JMP   p_emit_ax_text
@string:
      LDA   #<asm_jsr_write_string
      LDX   #>asm_jsr_write_string
      JMP   p_emit_ax_text
@real:
      LDA   #<asm_jsr_write_real
      LDX   #>asm_jsr_write_real
      JMP   p_emit_ax_text
@block:
      LDA   #<asm_file_write_block
      LDX   #>asm_file_write_block
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   p_write_size+1
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   p_write_size
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   #$0A
      JMP   p_emit
@fail:
      SEC
      RTS

p_emit_text_file_value:
      LDA   p_write_kind
      CMP   #WRITE_CHAR
      BEQ   @char
      CMP   #WRITE_BYTE
      BEQ   @byte
      CMP   #WRITE_WORD
      BEQ   @word
      CMP   #WRITE_STRING
      BEQ   @string
      CMP   #WRITE_REAL
      BNE   @fail
      LDA   #<asm_file_write_real
      LDX   #>asm_file_write_real
      JMP   p_emit_ax_text
@char:
      LDA   #<asm_file_write_char
      LDX   #>asm_file_write_char
      JMP   p_emit_ax_text
@byte:
      LDA   #<asm_file_write_byte
      LDX   #>asm_file_write_byte
      JMP   p_emit_ax_text
@word:
      LDA   #<asm_file_write_word
      LDX   #>asm_file_write_word
      JMP   p_emit_ax_text
@string:
      LDA   #<asm_file_write_string
      LDX   #>asm_file_write_string
      JMP   p_emit_ax_text
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

; Resolve an unqualified identifier against active WITH records, innermost
; first. Carry set returns A=field type, X=field row, and p_with_slot as the
; number of record bases above the selected one on the Pascal WITH stack.
p_with_field_find:
      LDX   p_with_depth
      BEQ   @missing
@scope:
      DEX
      STX   p_with_slot
      LDA   p_with_type,X
      CMP   #P_TYPE_USER_BASE
      BCC   @next_scope
      SEC
      SBC   #P_TYPE_USER_BASE
      TAX
      CPX   p_type_count
      BCS   @next_scope
      LDA   p_type_kind,X
      CMP   #TYPE_KIND_RECORD
      BNE   @next_scope
      LDA   p_type_low_l,X
      STA   p_field_iter
      CLC
      ADC   p_type_low_h,X
      STA   p_type_meta_count
@field:
      LDX   p_field_iter
      CPX   p_type_meta_count
      BCS   @next_scope
      LDA   p_field_name_len,X
      CMP   p_ident_len
      BNE   @next_field
      JSR   p_field_name_pointer
      LDY   #0
@compare:
      CPY   p_ident_len
      BCS   @found
      LDA   (p_word),Y
      CMP   p_ident,Y
      BNE   @next_field
      INY
      BRA   @compare
@next_field:
      INC   p_field_iter
      BRA   @field
@next_scope:
      LDX   p_with_slot
      CPX   #0
      BNE   @scope
@missing:
      CLC
      RTS
@found:
      LDA   p_with_depth
      SEC
      SBC   p_with_slot
      DEC
      STA   p_with_slot
      LDX   p_field_iter
      LDA   p_field_type,X
      SEC
      RTS

p_field_name_pointer:
      LDA   p_field_name_offset,X
      CLC
      ADC   #<p_field_names
      STA   p_word
      LDA   #>p_field_names
      ADC   #0
      STA   p_word+1
      RTS

; Emit NVR0 = address of the selected WITH field. The runtime stack is
; independent of the CPU stack, so expression temporaries and calls are safe.
p_emit_with_field_address:
      LDX   p_field_iter
      LDA   p_field_offset_l,X
      ORA   p_field_offset_h,X
      BNE   @offset
      LDA   #<asm_with_addr_head
      LDX   #>asm_with_addr_head
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   p_with_slot
      ASL
      INC
      INC
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   #<asm_with_addr_zero_mid
      LDX   #>asm_with_addr_zero_mid
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   p_with_slot
      ASL
      INC
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   #<asm_with_addr_zero_end
      LDX   #>asm_with_addr_zero_end
      JMP   p_emit_ax_text
@offset:
      LDA   #<asm_with_addr_head
      LDX   #>asm_with_addr_head
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   p_with_slot
      ASL
      INC
      INC
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   #<asm_with_addr_low
      LDX   #>asm_with_addr_low
      JSR   p_emit_ax_text
      BCS   @fail
      LDX   p_field_iter
      LDA   p_field_offset_l,X
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   #<asm_with_addr_mid
      LDX   #>asm_with_addr_mid
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   p_with_slot
      ASL
      INC
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   #<asm_with_addr_high
      LDX   #>asm_with_addr_high
      JSR   p_emit_ax_text
      BCS   @fail
      LDX   p_field_iter
      LDA   p_field_offset_h,X
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   #<asm_with_addr_end
      LDX   #>asm_with_addr_end
      JMP   p_emit_ax_text
@fail:
      RTS

p_emit_with_field_load:
      STA   p_expr_type
      STX   p_field_iter
      LDA   p_field_size_l,X
      STA   p_type_saved_size
      LDA   p_field_size_h,X
      STA   p_type_saved_size+1
      JSR   p_emit_with_field_address
      BCS   @fail
      LDA   p_type_saved_size+1
      BNE   @address
      LDA   p_type_saved_size
      CMP   #1
      BEQ   @byte
      CMP   #2
      BNE   @address
      LDA   #<asm_deref_nvr0_16
      LDX   #>asm_deref_nvr0_16
      JMP   p_emit_ax_text
@byte:
      LDA   #<asm_deref_nvr0_8
      LDX   #>asm_deref_nvr0_8
      JMP   p_emit_ax_text
@address:
      LDA   #<asm_address_result
      LDX   #>asm_address_result
      JMP   p_emit_ax_text
@fail:
      SEC
      RTS

      .segment "NPCFE_CODE"
p_parse_with:
      LDX   p_with_depth
      CPX   #P_WITH_CAP
      long_bcs @fail
      JSR   p_capture_identifier
      long_bcs @fail
      JSR   p_parse_designator_address
      long_bcs @fail
      LDA   p_designator_type
      CMP   #P_TYPE_USER_BASE
      long_bcc @fail
      SEC
      SBC   #P_TYPE_USER_BASE
      TAX
      LDA   p_type_kind,X
      CMP   #TYPE_KIND_RECORD
      long_bne @fail
      LDX   p_with_depth
      LDA   p_designator_type
      STA   p_with_type,X
      LDA   #<asm_with_push
      LDX   #>asm_with_push
      JSR   p_emit_ax_text
      long_bcs @output
      INC   p_with_depth
      LDA   #<kw_do
      STA   p_word
      LDA   #>kw_do
      STA   p_word+1
      JSR   p_expect_word
      BCS   @drop_fail
      JSR   p_parse_statement
      PHP
      DEC   p_with_depth
      LDA   #<asm_with_pop
      LDX   #>asm_with_pop
      JSR   p_emit_ax_text
      BCS   @drop_output
      PLP
      RTS
@drop_output:
      PLP
      BRA   @output
@drop_fail:
      DEC   p_with_depth
@fail:
      SEC
      RTS
@output:
      INC   p_emit_error
      SEC
      RTS

p_parse_case:
      LDX   p_control_depth
      CPX   #P_CONTROL_CAP
      long_bcs @fail
      JSR   p_parse_expression
      long_bcs @fail
      LDA   p_expr_type
      JSR   p_type_size
      long_bcs @fail
      LDX   p_control_depth
      STZ   p_case_word,X
      LDA   p_array_len+1
      long_bne @fail
      LDA   p_array_len
      CMP   #1
      BEQ   @push_byte
      CMP   #2
      long_bne @fail
      INC   p_case_word,X
@push_byte:
      LDA   #<kw_of
      STA   p_word
      LDA   #>kw_of
      STA   p_word+1
      JSR   p_expect_word
      long_bcs @fail
      JSR   p_new_label
      LDX   p_control_depth
      JSR   p_store_control_a
      INC   p_control_depth
@arm:
      LDA   #<kw_else
      LDX   #>kw_else
      JSR   p_peek_keyword
      long_bcs @else
      LDA   #<kw_end
      LDX   #>kw_end
      JSR   p_peek_keyword
      long_bcs @end
      JSR   p_new_label
      LDX   p_control_depth
      DEX
      LDA   p_label_saved
      STA   p_case_body_lo,X
      LDA   p_label_saved+1
      STA   p_case_body_hi,X
@case_label:
      LDA   #TYPE_WORD
      STA   p_decl_type
      JSR   p_parse_const_scalar
      long_bcs @fail
      JSR   p_emit_case_compare
      long_bcs @output
      JSR   p_skip_ws
      JSR   p_peek
      long_bcc @fail
      CMP   #','
      BNE   @colon
      JSR   p_next
      BRA   @case_label
@colon:
      LDA   #':'
      JSR   p_expect_char
      long_bcs @fail
      JSR   p_new_label
      LDX   p_control_depth
      DEX
      JSR   p_store_control_b
      JSR   p_emit_jump
      long_bcs @output
      LDX   p_control_depth
      DEX
      LDA   p_case_body_lo,X
      STA   p_label_saved
      LDA   p_case_body_hi,X
      STA   p_label_saved+1
      JSR   p_emit_label
      long_bcs @output
      JSR   p_parse_statement
      long_bcs @fail
      LDX   p_control_depth
      DEX
      JSR   p_load_control_a
      JSR   p_emit_jump
      long_bcs @output
      LDX   p_control_depth
      DEX
      JSR   p_load_control_b
      JSR   p_emit_label
      long_bcs @output
      JSR   p_skip_ws
      JSR   p_peek
      long_bcc @fail
      CMP   #';'
      long_bne @arm
      JSR   p_next
      JMP   @arm
@else:
      LDA   #<kw_else
      STA   p_word
      LDA   #>kw_else
      STA   p_word+1
      JSR   p_expect_word
      long_bcs @fail
@else_statement:
      LDA   #<kw_end
      LDX   #>kw_end
      JSR   p_peek_keyword
      BCS   @end
      JSR   p_parse_statement
      long_bcs @fail
      JSR   p_skip_ws
      JSR   p_peek
      long_bcc @fail
      CMP   #';'
      BNE   @end
      JSR   p_next
      BRA   @else_statement
@end:
      LDA   #<kw_end
      STA   p_word
      LDA   #>kw_end
      STA   p_word+1
      JSR   p_expect_word
      long_bcs @fail
      DEC   p_control_depth
      LDX   p_control_depth
      JSR   p_load_control_a
      JSR   p_emit_label
      long_bcs @output
      CLC
      RTS
@output:
      INC   p_emit_error
@fail:
      SEC
      RTS

p_emit_case_compare:
      LDX   p_control_depth
      DEX
      LDA   p_case_word,X
      BEQ   @byte
      LDA   #<asm_case_word_high
      LDX   #>asm_case_word_high
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   p_decimal+1
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   #<asm_case_word_low
      LDX   #>asm_case_word_low
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   p_decimal
      JSR   p_emit_hex_byte
      BCS   @fail
      BRA   @branch
@byte:
      LDA   p_decimal+1
      BNE   @fail
      LDA   #<asm_case_byte
      LDX   #>asm_case_byte
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   p_decimal
      JSR   p_emit_hex_byte
      BCS   @fail
@branch:
      LDA   #<asm_case_branch
      LDX   #>asm_case_branch
      JSR   p_emit_ax_text
      BCS   @fail
      LDX   p_control_depth
      DEX
      LDA   p_case_body_lo,X
      STA   p_label_saved
      LDA   p_case_body_hi,X
      STA   p_label_saved+1
      JSR   p_emit_label_id
      BCS   @fail
      LDX   p_control_depth
      DEX
      LDA   p_case_word,X
      BEQ   @newline
      LDA   #<asm_case_word_end
      LDX   #>asm_case_word_end
      JMP   p_emit_ax_text
@newline:
      LDA   #$0A
      JMP   p_emit
@fail:
      SEC
      RTS

; Parse selectors for the captured base symbol and return its address in A/X.
; Array dimensions carry their declared lower bounds and row-major strides.
p_parse_designator_address:
      JSR   p_symbol_find
      BCC   @with_field
      STA   p_designator_type
      STA   p_expr_type
      LDA   p_symbol_flags
      BEQ   @global
      JSR   p_emit_frame_designator_address
      long_bcs @fail
      BRA   @selector
@global:
      JSR   p_emit_address_identifier
      long_bcs @fail
      BRA   @selector
@with_field:
      JSR   p_with_field_find
      long_bcc @fail
      STA   p_designator_type
      STA   p_expr_type
      STX   p_field_iter
      JSR   p_emit_with_field_address
      long_bcs @fail
      LDA   #<asm_address_result
      LDX   #>asm_address_result
      JSR   p_emit_ax_text
      long_bcs @fail
@selector:
      JSR   p_skip_ws
      JSR   p_peek
      long_bcc @done
      CMP   #'['
      BEQ   @has_index
      CMP   #'.'
      long_beq @has_field
      JMP   @done
@has_index:
      LDA   p_designator_type
      CMP   #P_TYPE_USER_BASE
      long_bcc @fail
      SEC
      SBC   #P_TYPE_USER_BASE
      TAX
      STX   p_designator_row
      LDA   p_type_kind,X
      CMP   #TYPE_KIND_STRING
      long_beq @string_index
      CMP   #TYPE_KIND_ARRAY
      long_bne @fail
      STZ   p_designator_dim
      JSR   p_next
@dimension:
      LDA   #<asm_phx_pha
      LDX   #>asm_phx_pha
      JSR   p_emit_ax_text
      long_bcs @fail
      JSR   p_parse_expression
      long_bcs @fail
      LDA   p_expr_type
      CMP   #TYPE_WORD
      BEQ   @index_is_word
      LDA   #<asm_index_high_zero
      LDX   #>asm_index_high_zero
      JSR   p_emit_ax_text
      long_bcs @fail
@index_is_word:
      LDX   p_designator_row
      LDA   p_type_low_l,X
      CLC
      ADC   p_designator_dim
      TAX
      CPX   p_dim_count
      long_bcs @fail
      STX   p_dim_iter
      JSR   p_emit_index_dimension
      long_bcs @fail
      INC   p_designator_dim
      JSR   p_skip_ws
      JSR   p_peek
      long_bcc @fail
      CMP   #','
      BNE   @close
      JSR   p_next
      BRA   @dimension
@close:
      LDA   #']'
      JSR   p_expect_char
      long_bcs @fail
      LDX   p_designator_row
      LDA   p_designator_dim
      CMP   p_type_low_h,X
      long_bne @fail
      LDA   p_type_element,X
      STA   p_designator_type
      STA   p_expr_type
      JMP   @selector
@string_index:
      JSR   p_next
      LDA   #<asm_phx_pha
      LDX   #>asm_phx_pha
      JSR   p_emit_ax_text
      long_bcs @fail
      JSR   p_parse_expression
      long_bcs @fail
      LDA   p_expr_type
      CMP   #TYPE_WORD
      BEQ   @string_index_word
      LDA   #<asm_index_high_zero
      LDX   #>asm_index_high_zero
      JSR   p_emit_ax_text
      long_bcs @fail
@string_index_word:
      LDA   #']'
      JSR   p_expect_char
      long_bcs @fail
      LDA   #<asm_string_index_stack_call
      LDX   #>asm_string_index_stack_call
      JSR   p_emit_ax_text
      long_bcs @fail
      LDA   #TYPE_CHAR
      STA   p_designator_type
      STA   p_expr_type
      JMP   @selector
@has_field:
      JSR   p_next
      JSR   p_capture_identifier
      long_bcs @fail
      JSR   p_designator_field_find
      long_bcc @fail
      STA   p_designator_type
      STA   p_expr_type
      STX   p_field_iter
      JSR   p_emit_designator_field_address
      long_bcs @fail
      JMP   @selector
@done:
      CLC
      RTS
@fail:
      SEC
      RTS

; Find the captured field name in p_designator_type's record descriptor.
; Carry set returns A=field type and X=field row.
p_designator_field_find:
      LDA   p_designator_type
      CMP   #P_TYPE_USER_BASE
      BCC   @missing
      SEC
      SBC   #P_TYPE_USER_BASE
      TAX
      CPX   p_type_count
      BCS   @missing
      LDA   p_type_kind,X
      CMP   #TYPE_KIND_RECORD
      BNE   @missing
      LDA   p_type_low_l,X
      STA   p_field_iter
      CLC
      ADC   p_type_low_h,X
      STA   p_type_meta_count
@field:
      LDX   p_field_iter
      CPX   p_type_meta_count
      BCS   @missing
      LDA   p_field_name_len,X
      CMP   p_ident_len
      BNE   @next
      JSR   p_field_name_pointer
      LDY   #0
@compare:
      CPY   p_ident_len
      BCS   @found
      LDA   (p_word),Y
      CMP   p_ident,Y
      BNE   @next
      INY
      BRA   @compare
@next:
      INC   p_field_iter
      BRA   @field
@found:
      LDX   p_field_iter
      LDA   p_field_type,X
      SEC
      RTS
@missing:
      CLC
      RTS

; Add the selected record field's constant offset to the generated A/X
; address without using compiler-private runtime state.
p_emit_designator_field_address:
      LDX   p_field_iter
      LDA   p_field_offset_h,X
      BNE   @wide
      LDA   p_field_offset_l,X
      BEQ   @done
      STA   p_digit
      LDA   #<asm_field_addr_low
      LDX   #>asm_field_addr_low
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   p_digit
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   #<asm_field_addr_byte_end
      LDX   #>asm_field_addr_byte_end
      JMP   p_emit_ax_text
@wide:
      LDA   #<asm_field_addr_low
      LDX   #>asm_field_addr_low
      JSR   p_emit_ax_text
      BCS   @fail
      LDX   p_field_iter
      LDA   p_field_offset_l,X
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   #<asm_field_addr_high
      LDX   #>asm_field_addr_high
      JSR   p_emit_ax_text
      BCS   @fail
      LDX   p_field_iter
      LDA   p_field_offset_h,X
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   #<asm_field_addr_end
      LDX   #>asm_field_addr_end
      JMP   p_emit_ax_text
@done:
      CLC
@fail:
      RTS

; Keep the original compact typed IR for the overwhelmingly common Pascal
; array shape. O2 can then cache and window these accesses; every other shape
; falls through to the language-neutral NDK address helper.
p_simple_global_byte_array:
      CMP   #P_TYPE_USER_BASE
      BCC   @no
      SEC
      SBC   #P_TYPE_USER_BASE
      TAX
      STX   p_designator_row
      LDA   p_symbol_flags
      BNE   @no
      LDA   p_type_kind,X
      CMP   #TYPE_KIND_ARRAY
      BNE   @no
      LDA   p_type_low_h,X
      CMP   #1
      BNE   @no
      LDA   p_type_low_l,X
      TAX
      LDA   p_dim_low_l,X
      ORA   p_dim_low_h,X
      BNE   @no
      LDA   p_dim_stride_l,X
      CMP   #1
      BNE   @no
      LDA   p_dim_stride_h,X
      BNE   @no
      LDX   p_symbol_iter
      SEC
      RTS
@no:
      CLC
      RTS

p_parse_simple_array_value:
      LDY   p_array_depth
      CPY   #8
      BCS   @fail
      TXA
      STA   p_array_symbols,Y
      INC   p_array_depth
      JSR   p_next
      JSR   p_parse_expression
      BCS   @drop_fail
      LDA   p_expr_type
      CMP   #TYPE_WORD
      BEQ   @word_index
      LDA   #<asm_index_high_zero
      LDX   #>asm_index_high_zero
      JSR   p_emit_ax_text
      BCS   @drop_fail
@word_index:
      LDA   #']'
      JSR   p_expect_char
      BCS   @drop_fail
      DEC   p_array_depth
      LDX   p_array_depth
      LDA   p_array_symbols,X
      STA   p_symbol_iter
      TAX
      JSR   p_symbol_pointer
      LDY   #1
      LDA   (p_word),Y
      SEC
      SBC   #P_TYPE_USER_BASE
      TAX
      LDA   p_type_element,X
      STA   p_expr_type
      LDA   #<asm_array_get
      LDX   #>asm_array_get
      LDY   p_symbol_iter
      JMP   p_emit_array_operation
@drop_fail:
      DEC   p_array_depth
@fail:
      SEC
      RTS

; Turn a fully selected designator address into its scalar value. Structured
; values deliberately remain addresses for strings, records, sets, and files.
p_parse_designator_value:
      JSR   p_parse_designator_address
      BCS   @fail
      LDA   p_designator_type
      STA   p_expr_type
      JSR   p_type_size
      BCS   @fail
      LDA   p_array_len+1
      BNE   @address
      LDA   p_array_len
      CMP   #1
      long_beq @byte
      CMP   #2
      BNE   @address
      LDA   #<asm_deref16
      LDX   #>asm_deref16
      JMP   p_emit_ax_text
@byte:
      LDA   #<asm_deref8
      LDX   #>asm_deref8
      JMP   p_emit_ax_text
@address:
      CLC
      RTS
@fail:
      SEC
      RTS

p_emit_index_dimension:
      LDX   p_dim_iter
      LDA   p_dim_low_l,X
      ORA   p_dim_low_h,X
      ORA   p_dim_stride_h,X
      BNE   @general
      LDA   p_dim_stride_l,X
      CMP   #1
      BNE   @general
      LDA   #<asm_index_byte_stack_call
      LDX   #>asm_index_byte_stack_call
      JMP   p_emit_ax_text
@general:
      LDA   #<asm_index_stack_call
      LDX   #>asm_index_stack_call
      JSR   p_emit_ax_text
      BCS   @fail
      LDX   p_dim_iter
      LDA   p_dim_low_h,X
      JSR   p_emit_hex_byte
      BCS   @fail
      LDX   p_dim_iter
      LDA   p_dim_low_l,X
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   #<asm_index_word_separator
      LDX   #>asm_index_word_separator
      JSR   p_emit_ax_text
      BCS   @fail
      LDX   p_dim_iter
      LDA   p_dim_stride_h,X
      JSR   p_emit_hex_byte
      BCS   @fail
      LDX   p_dim_iter
      LDA   p_dim_stride_l,X
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   #$0A
      JMP   p_emit
@fail:
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
      CPX   #P_CONTROL_CAP
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
      CPX   #P_CONTROL_CAP
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

      .segment "CODE"
p_parse_repeat:
      LDX   p_control_depth
      CPX   #P_CONTROL_CAP
      BCS   @fail
      JSR   p_new_label
      JSR   p_store_control_a
      JSR   p_emit_label
      BCS   @output
      INC   p_control_depth
@next:
      JSR   p_skip_ws
      JSR   p_peek
      BCC   @fail_depth
      CMP   #';'
      BNE   @until
      JSR   p_next
      BRA   @next
@until:
      LDA   #<kw_until
      LDX   #>kw_until
      JSR   p_peek_keyword
      BCC   @statement
      LDA   #<kw_until
      STA   p_word
      LDA   #>kw_until
      STA   p_word+1
      JSR   p_expect_word
      BCS   @fail_depth
      JSR   p_parse_expression
      BCS   @fail_depth
      DEC   p_control_depth
      LDX   p_control_depth
      JSR   p_load_control_a
      JMP   p_emit_branch_false
@statement:
      JSR   p_parse_statement
      BCS   @fail_depth
      JSR   p_skip_ws
      JSR   p_peek
      BCC   @fail_depth
      CMP   #';'
      BEQ   @next
      LDA   #<kw_until
      LDX   #>kw_until
      JSR   p_peek_keyword
      BCS   @next
@fail_depth:
      DEC   p_control_depth
@fail:
      SEC
      RTS
@output:
      INC   p_emit_error
      BRA   @fail

p_parse_for:
      LDX   p_control_depth
      CPX   #P_CONTROL_CAP
      long_bcs @fail
      JSR   p_capture_identifier
      long_bcs @fail
      JSR   p_save_identifier
      JSR   p_symbol_find
      long_bcc @fail
      STX   p_target_symbol
      STA   p_target_type
      JSR   p_save_target_storage
      LDX   p_control_depth
      LDA   p_target_symbol
      STA   p_control_symbol,X
      LDA   #':'
      JSR   p_expect_char
      long_bcs @fail
      LDA   #'='
      JSR   p_expect_char
      long_bcs @fail
      JSR   p_parse_expression
      long_bcs @fail
      JSR   p_emit_store_saved
      long_bcs @output
      JSR   p_capture_identifier
      long_bcs @fail
      LDA   #4
      STA   p_for_direction
      LDA   #<kw_to
      LDX   #>kw_to
      JSR   p_ident_is
      BCS   @direction
      LDA   #<kw_downto
      LDX   #>kw_downto
      JSR   p_ident_is
      BCC   @fail
      LDA   #6
      STA   p_for_direction
@direction:
      JSR   p_new_label
      LDX   p_control_depth
      JSR   p_store_control_a
      JSR   p_emit_label
      BCS   @output
      JSR   p_emit_load_for_symbol
      BCS   @output
      LDA   p_for_direction
      JSR   p_push_operator
      BCS   @fail
      JSR   p_emit_binary_prep
      BCS   @fail_operator
      JSR   p_parse_expression
      BCS   @fail_operator
      JSR   p_pop_operator
      JSR   p_emit_compare
      BCS   @output
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
      JSR   p_emit_for_step
      BCS   @output
      LDX   p_control_depth
      JSR   p_load_control_a
      JSR   p_emit_jump
      BCS   @output
      LDX   p_control_depth
      JSR   p_load_control_b
      JMP   p_emit_label
@fail_operator:
      DEC   p_operator_depth
@fail:
      SEC
      RTS
@output:
      INC   p_emit_error
      BRA   @fail

p_emit_load_for_symbol:
      LDY   p_control_depth
      LDX   p_control_symbol,Y
      STX   p_target_symbol
      JSR   p_symbol_pointer
      LDY   #1
      LDA   (p_word),Y
      STA   p_target_type
      STA   p_expr_type
      LDY   #SYMBOL_FLAGS
      LDA   (p_word),Y
      STA   p_symbol_flags
      STA   p_target_flags
      LDY   #SYMBOL_OFFSET_LO
      LDA   (p_word),Y
      STA   p_symbol_offset
      STA   p_target_offset
      INY
      LDA   (p_word),Y
      STA   p_symbol_offset+1
      STA   p_target_offset+1
      INY
      LDA   (p_word),Y
      STA   p_symbol_depth
      STA   p_target_depth
      LDA   p_symbol_flags
      BNE   @frame
      LDA   p_expr_type
      CMP   #TYPE_WORD
      BNE   @byte
      LDA   #<asm_load_word_ir
      LDX   #>asm_load_word_ir
      BRA   @emit
@byte:
      LDA   #<asm_load_byte_ir
      LDX   #>asm_load_byte_ir
@emit:
      JSR   p_emit_ax_text
      BCS   @fail
      LDX   p_target_symbol
      JSR   p_emit_symbol_name
      BCS   @fail
      LDA   #$0A
      JMP   p_emit
@frame:
      JMP   p_emit_load_frame_value
@fail:
      SEC
      RTS

p_emit_store_for_symbol:
      LDA   p_target_flags
      long_bne p_emit_store_frame_target
      LDA   p_target_type
      CMP   #TYPE_WORD
      BNE   @byte
      LDA   p_expr_type
      CMP   #TYPE_WORD
      BNE   @extend
      LDA   #<asm_store_word_ir
      LDX   #>asm_store_word_ir
      BRA   @emit
@extend:
      LDA   #<asm_store_extend_ir
      LDX   #>asm_store_extend_ir
      BRA   @emit
@byte:
      LDA   #<asm_store_byte_ir
      LDX   #>asm_store_byte_ir
@emit:
      JSR   p_emit_ax_text
      BCS   @fail
      LDX   p_target_symbol
      JSR   p_emit_symbol_name
      BCS   @fail
      LDA   #$0A
      JMP   p_emit
@fail:
      SEC
      RTS

p_emit_for_step:
      JSR   p_emit_load_for_symbol
      BCS   @fail
      LDA   p_for_direction
      CMP   #4
      BNE   @subtract
      LDA   #'+'
      BRA   @operator
@subtract:
      LDA   #'-'
@operator:
      JSR   p_push_operator
      BCS   @fail
      JSR   p_emit_binary_prep
      BCS   @drop_fail
      LDA   #1
      JSR   p_emit_load_byte
      BCS   @drop_fail
      LDA   #TYPE_BYTE
      STA   p_expr_type
      JSR   p_pop_operator
      LDA   p_left_type
      CMP   #TYPE_WORD
      BNE   @byte
      JSR   p_emit_wide_operands
      BCS   @fail
      LDA   p_for_direction
      CMP   #4
      BNE   @word_subtract
      LDA   #<asm_add_word
      LDX   #>asm_add_word
      BRA   @emit
@word_subtract:
      LDA   #<asm_subtract_word
      LDX   #>asm_subtract_word
      BRA   @emit
@byte:
      LDA   p_for_direction
      CMP   #4
      BNE   @byte_subtract
      LDA   #<asm_add
      LDX   #>asm_add
      BRA   @emit
@byte_subtract:
      LDA   #<asm_subtract
      LDX   #>asm_subtract
@emit:
      JSR   p_emit_ax_text
      BCS   @fail
      JMP   p_emit_store_for_symbol
@drop_fail:
      DEC   p_operator_depth
@fail:
      SEC
      RTS
      .segment "NPCFE_CODE"

; expression = additive [ (= | <> | < | <= | > | >=) additive ]
p_parse_expression:
      JSR   p_parse_additive
      long_bcs @fail
      LDA   #<kw_in
      LDX   #>kw_in
      JSR   p_peek_keyword
      long_bcc @relation_operator
      LDA   #<kw_in
      STA   p_word
      LDA   #>kw_in
      STA   p_word+1
      JSR   p_expect_word
      long_bcs @fail
      LDA   #'I'
      JSR   p_push_operator
      long_bcs @fail
      JSR   p_emit_binary_prep
      long_bcs @drop_fail
      JSR   p_parse_additive
      long_bcs @drop_fail
      JSR   p_pop_operator
      LDA   p_left_type
      CMP   #TYPE_WORD
      long_beq @fail
      LDA   p_expr_type
      CMP   #TYPE_ARRAY_BYTE
      BEQ   @small_set
      CMP   #TYPE_SET_REF
      BEQ   @large_set
      CMP   #P_TYPE_USER_BASE
      long_bcc @fail
      SEC
      SBC   #P_TYPE_USER_BASE
      TAX
      CPX   p_type_count
      long_bcs @fail
      LDA   p_type_kind,X
      CMP   #TYPE_KIND_SET
      long_bne @fail
      LDA   p_type_size_h,X
      BNE   @large_set
      LDA   p_type_size_l,X
      CMP   #2
      BCC   @small_set
@large_set:
      LDA   #<asm_set_in256
      LDX   #>asm_set_in256
      BRA   @membership
@small_set:
      LDA   #<asm_set_in8
      LDX   #>asm_set_in8
@membership:
      JSR   p_emit_ax_text
      long_bcs @fail
      LDA   #TYPE_BOOLEAN
      STA   p_expr_type
      CLC
      RTS
@relation_operator:
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
      long_bcs @fail
@operator:
      LDA   #<kw_or
      LDX   #>kw_or
      JSR   p_peek_keyword
      BCC   @symbol_operator
      LDA   #<kw_or
      STA   p_word
      LDA   #>kw_or
      STA   p_word+1
      JSR   p_expect_word
      long_bcs @fail
      LDA   #'O'
      BRA   @have
@symbol_operator:
      JSR   p_skip_ws
      JSR   p_peek
      long_bcc @ok
      CMP   #'+'
      BEQ   @have
      CMP   #'-'
      long_bne @ok
@have:
      STA   p_operator
      JSR   p_next
      LDA   p_operator
      JSR   p_push_operator
      long_bcs @fail
      JSR   p_emit_binary_prep
      long_bcs @drop_fail
      JSR   p_parse_term
      long_bcs @drop_fail
      JSR   p_pop_operator
      LDA   p_operator
      CMP   #'O'
      long_beq @logical_or
      CMP   #'+'
      BNE   @set_operation
      LDA   p_left_type
      JSR   p_type_is_string
      BCS   @left_string
      LDA   p_expr_type
      JSR   p_type_is_string
      BCC   @set_operation
      LDA   p_left_type
      JSR   p_type_is_char
      long_bcc @fail
      LDA   #<asm_char_concat_string
      LDX   #>asm_char_concat_string
      BRA   @emit_string_add
@left_string:
      LDA   p_expr_type
      JSR   p_type_is_string
      BCS   @both_strings
      LDA   p_expr_type
      JSR   p_type_is_char
      long_bcc @fail
      LDA   #<asm_string_concat_char
      LDX   #>asm_string_concat_char
      BRA   @emit_string_add
@both_strings:
      LDA   #<asm_string_concat
      LDX   #>asm_string_concat
@emit_string_add:
      JSR   p_emit_ax_text
      long_bcs @fail
      LDA   #TYPE_STRING_REF
      STA   p_expr_type
      JMP   @operator
@set_operation:
      LDA   p_left_type
      JSR   p_type_is_small_set
      BCC   @ordinal
      LDA   p_expr_type
      JSR   p_type_is_small_set
      BCC   @ordinal
      LDA   p_operator
      CMP   #'+'
      BNE   @set_difference
      LDA   #<asm_set_union
      LDX   #>asm_set_union
      BRA   @set_emit
@set_difference:
      LDA   #<asm_set_difference
      LDX   #>asm_set_difference
@set_emit:
      JSR   p_emit_ax_text
      long_bcs @fail
      LDA   #TYPE_ARRAY_BYTE
      STA   p_expr_type
      JMP   @operator
@logical_or:
      LDA   p_left_type
      CMP   #TYPE_WORD
      long_beq @fail
      LDA   p_expr_type
      CMP   #TYPE_WORD
      long_beq @fail
      LDA   #<asm_or
      LDX   #>asm_or
      JSR   p_emit_ax_text
      long_bcs @fail
      LDA   #TYPE_BOOLEAN
      STA   p_expr_type
      JMP   @operator
@ordinal:
      LDA   p_left_type
      JSR   p_type_is_real
      BCC   @right_real
      LDA   p_expr_type
      JSR   p_type_is_real
      BCC   @fail
      LDA   p_operator
      CMP   #'+'
      BNE   @real_subtract
      LDA   #<asm_real_add
      LDX   #>asm_real_add
      BRA   @real_emit
@real_subtract:
      LDA   #<asm_real_subtract
      LDX   #>asm_real_subtract
@real_emit:
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   #TYPE_REAL
      STA   p_expr_type
      JMP   @operator
@right_real:
      LDA   p_expr_type
      JSR   p_type_is_real
      BCS   @fail
      LDA   p_left_type
      CMP   #TYPE_WORD
      BEQ   @wide
      LDA   p_expr_type
      CMP   #TYPE_WORD
      BEQ   @wide
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
      JMP   @operator
@wide:
      JSR   p_emit_wide_operands
      BCS   @fail
      LDA   p_operator
      CMP   #'+'
      BNE   @wide_subtract
      LDA   #<asm_add_word
      LDX   #>asm_add_word
      BRA   @wide_emit
@wide_subtract:
      LDA   #<asm_subtract_word
      LDX   #>asm_subtract_word
@wide_emit:
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   #TYPE_WORD
      STA   p_expr_type
      JMP   @operator
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
      long_bcs @fail
@operator:
      LDA   #<kw_and
      LDX   #>kw_and
      JSR   p_peek_keyword
      BCC   @modulo_keyword
      LDA   #<kw_and
      STA   p_word
      LDA   #>kw_and
      STA   p_word+1
      JSR   p_expect_word
      long_bcs @fail
      LDA   #'A'
      BRA   @have
@modulo_keyword:
      LDA   #<kw_mod
      LDX   #>kw_mod
      JSR   p_peek_keyword
      BCC   @divide_keyword
      LDA   #<kw_mod
      STA   p_word
      LDA   #>kw_mod
      STA   p_word+1
      JSR   p_expect_word
      long_bcs @fail
      LDA   #'M'
      BRA   @have
@divide_keyword:
      LDA   #<kw_div
      LDX   #>kw_div
      JSR   p_peek_keyword
      BCC   @multiply
      LDA   #<kw_div
      STA   p_word
      LDA   #>kw_div
      STA   p_word+1
      JSR   p_expect_word
      long_bcs @fail
      LDA   #'D'
      BRA   @have
@multiply:
      JSR   p_skip_ws
      JSR   p_peek
      long_bcc @ok
      CMP   #'*'
      BEQ   @symbol
      CMP   #'/'
      long_bne @ok
@symbol:
      JSR   p_next
@have:
      JSR   p_push_operator
      long_bcs @fail
      JSR   p_emit_binary_prep
      long_bcs @drop_fail
      JSR   p_parse_factor
      long_bcs @drop_fail
      JSR   p_pop_operator
      LDA   p_operator
      CMP   #'A'
      long_beq @logical_and
      CMP   #'*'
      long_beq @multiply_value
      CMP   #'/'
      long_beq @real_divide
      CMP   #'D'
      long_beq @integer_divide
      JMP   @modulo
@multiply_value:
      LDA   p_left_type
      JSR   p_type_is_small_set
      long_bcc @numeric_multiply
      LDA   p_expr_type
      JSR   p_type_is_small_set
      long_bcc @fail
      LDA   #<asm_set_intersection
      LDX   #>asm_set_intersection
      JSR   p_emit_ax_text
      long_bcs @fail
      LDA   #TYPE_ARRAY_BYTE
      STA   p_expr_type
      JMP   @operator
@numeric_multiply:
      LDA   p_left_type
      JSR   p_type_is_real
      long_bcs @left_real
      LDA   p_expr_type
      JSR   p_type_is_real
      long_bcs @right_real
      JSR   p_emit_wide_operands
      long_bcs @fail
      LDA   #<asm_multiply_word
      LDX   #>asm_multiply_word
      JSR   p_emit_ax_text
      long_bcs @fail
      LDA   #TYPE_WORD
      STA   p_expr_type
      JMP   @operator
@left_real:
      LDA   p_expr_type
      JSR   p_type_is_real
      BCS   @both_real
      LDA   p_expr_type
      CMP   #TYPE_WORD
      BEQ   @real_word_right
      LDA   #<asm_real_multiply_byte_right
      LDX   #>asm_real_multiply_byte_right
      BRA   @emit_real
@real_word_right:
      LDA   #<asm_real_multiply_word_right
      LDX   #>asm_real_multiply_word_right
      BRA   @emit_real
@right_real:
      LDA   p_left_type
      CMP   #TYPE_WORD
      BEQ   @word_real_right
      LDA   #<asm_byte_multiply_real
      LDX   #>asm_byte_multiply_real
      BRA   @emit_real
@word_real_right:
      LDA   #<asm_word_multiply_real
      LDX   #>asm_word_multiply_real
      BRA   @emit_real
@both_real:
      LDA   #<asm_real_multiply
      LDX   #>asm_real_multiply
@emit_real:
      JSR   p_emit_ax_text
      long_bcs @fail
      LDA   #TYPE_REAL
      STA   p_expr_type
      JMP   @operator
@logical_and:
      LDA   p_left_type
      CMP   #TYPE_WORD
      long_beq @fail
      LDA   p_expr_type
      CMP   #TYPE_WORD
      long_beq @fail
      LDA   #<asm_and
      LDX   #>asm_and
      JSR   p_emit_ax_text
      long_bcs @fail
      LDA   #TYPE_BOOLEAN
      STA   p_expr_type
      JMP   @operator
@integer_divide:
      LDA   p_left_type
      JSR   p_type_is_real
      long_bcs @fail
      LDA   p_expr_type
      JSR   p_type_is_real
      long_bcs @fail
      LDA   p_left_type
      CMP   #TYPE_WORD
      BEQ   @integer_divide_word
      LDA   p_expr_type
      CMP   #TYPE_WORD
      BEQ   @integer_divide_word
      LDA   #<asm_divide
      LDX   #>asm_divide
      JSR   p_emit_ax_text
      long_bcs @fail
      LDA   #TYPE_BYTE
      STA   p_expr_type
      JMP   @operator
@integer_divide_word:
      JSR   p_emit_wide_operands
      long_bcs @fail
      LDA   #<asm_divide_word
      LDX   #>asm_divide_word
      JSR   p_emit_ax_text
      long_bcs @fail
      LDA   #TYPE_WORD
      STA   p_expr_type
      JMP   @operator
@real_divide:
      LDA   p_left_type
      JSR   p_type_is_real
      long_bcs @divide_left_real
      LDA   p_expr_type
      JSR   p_type_is_real
      long_bcs @divide_right_real
      JSR   p_emit_wide_operands
      long_bcs @fail
      LDA   #<asm_ordinal_divide_real
      LDX   #>asm_ordinal_divide_real
      BRA   @emit_divide_real
@divide_left_real:
      LDA   p_expr_type
      JSR   p_type_is_real
      BCS   @divide_both_real
      LDA   p_expr_type
      CMP   #TYPE_WORD
      BEQ   @divide_real_word
      LDA   #<asm_real_divide_byte
      LDX   #>asm_real_divide_byte
      BRA   @emit_divide_real
@divide_real_word:
      LDA   #<asm_real_divide_word
      LDX   #>asm_real_divide_word
      BRA   @emit_divide_real
@divide_right_real:
      LDA   p_left_type
      CMP   #TYPE_WORD
      BEQ   @divide_word_real
      LDA   #<asm_byte_divide_real
      LDX   #>asm_byte_divide_real
      BRA   @emit_divide_real
@divide_word_real:
      LDA   #<asm_word_divide_real
      LDX   #>asm_word_divide_real
      BRA   @emit_divide_real
@divide_both_real:
      LDA   #<asm_real_divide
      LDX   #>asm_real_divide
@emit_divide_real:
      JSR   p_emit_ax_text
      long_bcs @fail
      LDA   #TYPE_REAL
      STA   p_expr_type
      JMP   @operator
@modulo:
      LDA   p_left_type
      CMP   #TYPE_WORD
      BEQ   @modulo_word
      LDA   p_expr_type
      CMP   #TYPE_WORD
      BEQ   @modulo_word
      LDA   #<asm_mod
      LDX   #>asm_mod
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   #TYPE_BYTE
      STA   p_expr_type
      JMP   @operator
@modulo_word:
      JSR   p_emit_wide_operands
      BCS   @fail
      LDA   #<asm_modulo_word
      LDX   #>asm_modulo_word
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   #TYPE_WORD
      STA   p_expr_type
      JMP   @operator
@ok:
      CLC
@fail:
      RTS
@drop_fail:
      DEC   p_operator_depth
      SEC
      RTS

; Carry set when A denotes a packed one-byte Pascal set.
p_type_is_small_set:
      CMP   #TYPE_ARRAY_BYTE
      BEQ   @yes
      CMP   #P_TYPE_USER_BASE
      BCC   @no
      SEC
      SBC   #P_TYPE_USER_BASE
      TAX
      CPX   p_type_count
      BCS   @no
      LDA   p_type_kind,X
      CMP   #TYPE_KIND_SET
      BNE   @no
      LDA   p_type_size_h,X
      BNE   @no
      LDA   p_type_size_l,X
      CMP   #1
      BNE   @no
@yes:
      SEC
      RTS
@no:
      CLC
      RTS

p_parse_factor:
      JSR   p_skip_ws
      JSR   p_peek
      long_bcc @fail
      CMP   #'+'
      BEQ   @unary_plus
      CMP   #'-'
      BNE   @not
      JSR   p_next
      JSR   p_parse_factor
      long_bcs @fail
      LDA   p_expr_type
      JSR   p_type_is_real
      BCS   @negate_real
      LDA   p_expr_type
      CMP   #TYPE_WORD
      BEQ   @negate_word
      LDA   #<asm_negate_byte
      LDX   #>asm_negate_byte
      JMP   p_emit_ax_text
@negate_word:
      LDA   #<asm_negate_word
      LDX   #>asm_negate_word
      JMP   p_emit_ax_text
@negate_real:
      LDA   #<asm_negate_real
      LDX   #>asm_negate_real
      JMP   p_emit_ax_text
@unary_plus:
      JSR   p_next
      JMP   p_parse_factor
@not:
      LDA   #<kw_not
      LDX   #>kw_not
      JSR   p_peek_keyword
      BCC   @ordinary
      LDA   #<kw_not
      STA   p_word
      LDA   #>kw_not
      STA   p_word+1
      JSR   p_expect_word
      BCS   @fail
      JSR   p_parse_factor
      BCS   @fail
      LDA   #<asm_not
      LDX   #>asm_not
      JSR   p_emit_ax_text
      RTS
@ordinary:
      JSR   p_skip_ws
      JSR   p_peek
      BCC   @fail
      CMP   #'['
      long_beq p_parse_set_literal
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
@fail:
      RTS

; Parse a Pascal set constructor into a 256-bit compile-time bitmap. Small
; enum sets are later materialized directly in A; character sets become a
; pointer to 32 read-only bytes.
p_parse_set_bits:
      LDX   #31
@clear:
      STZ   p_set_bits,X
      DEX
      BPL   @clear
      STZ   p_set_large
      LDA   #'['
      JSR   p_expect_char
      BCS   @fail
      JSR   p_skip_ws
      JSR   p_peek
      BCC   @fail
      CMP   #']'
      BEQ   @close
@member:
      LDA   #TYPE_WORD
      STA   p_decl_type
      JSR   p_parse_const_scalar
      BCS   @fail
      LDA   p_decimal+1
      BNE   @fail
      LDA   p_decimal
      STA   p_set_start
      STA   p_set_end
      JSR   p_skip_ws
      JSR   p_peek
      BCC   @fail
      CMP   #'.'
      BNE   @set_range
      JSR   p_next
      LDA   #'.'
      JSR   p_expect_char
      BCS   @fail
      LDA   #TYPE_WORD
      STA   p_decl_type
      JSR   p_parse_const_scalar
      BCS   @fail
      LDA   p_decimal+1
      BNE   @fail
      LDA   p_decimal
      CMP   p_set_start
      BCC   @fail
      STA   p_set_end
@set_range:
      LDA   p_set_start
      JSR   p_set_bit
      LDA   p_set_start
      CMP   p_set_end
      BEQ   @delimiter
      INC   p_set_start
      BRA   @set_range
@delimiter:
      JSR   p_skip_ws
      JSR   p_peek
      BCC   @fail
      CMP   #','
      BNE   @close
      JSR   p_next
      BRA   @member
@close:
      LDA   #']'
      JMP   p_expect_char
@fail:
      SEC
      RTS

; Set the bit for A in p_set_bits.
p_set_bit:
      CMP   #8
      BCC   :+
      INC   p_set_large
:     PHA
      AND   #7
      TAX
      LDA   set_bit_masks,X
      STA   p_digit
      PLA
      LSR
      LSR
      LSR
      TAX
      LDA   p_set_bits,X
      ORA   p_digit
      STA   p_set_bits,X
      RTS

p_parse_set_literal:
      JSR   p_parse_set_bits
      BCS   @fail
      LDA   p_set_large
      BNE   @large
      LDA   p_set_bits
      JSR   p_emit_load_byte
      BCS   @fail
      LDA   #TYPE_ARRAY_BYTE
      STA   p_expr_type
      CLC
      RTS
@large:
      JSR   p_new_label
      LDA   #<asm_rodata
      LDX   #>asm_rodata
      JSR   p_emit_ax_text
      BCS   @fail
      JSR   p_emit_label
      BCS   @fail
      LDA   #<asm_set_bytes
      LDX   #>asm_set_bytes
      JSR   p_emit_ax_text
      BCS   @fail
      STZ   p_set_start
@byte:
      LDX   p_set_start
      CPX   #32
      BCS   @data_done
      CPX   #0
      BEQ   :+
      LDA   #','
      JSR   p_emit
      BCS   @fail
:     LDA   #'$'
      JSR   p_emit
      BCS   @fail
      LDA   p_set_bits,X
      JSR   p_emit_hex_byte
      BCS   @fail
      INC   p_set_start
      BRA   @byte
@data_done:
      LDA   #$0A
      JSR   p_emit
      BCS   @fail
      LDA   #<asm_code
      LDX   #>asm_code
      JSR   p_emit_ax_text
      BCS   @fail
      JSR   p_emit_set_label_address
      BCS   @fail
      LDA   #TYPE_SET_REF
      STA   p_expr_type
      CLC
      RTS
@fail:
      SEC
      RTS

p_emit_set_label_address:
      LDA   #<asm_load_string_lo
      LDX   #>asm_load_string_lo
      JSR   p_emit_ax_text
      BCS   @fail
      JSR   p_emit_label_id
      BCS   @fail
      LDA   #$0A
      JSR   p_emit
      BCS   @fail
      LDA   #<asm_load_string_hi
      LDX   #>asm_load_string_hi
      JSR   p_emit_ax_text
      BCS   @fail
      JSR   p_emit_label_id
      BCS   @fail
      LDA   #$0A
      JMP   p_emit
@fail:
      SEC
      RTS

; A Turbo const section continues until the next declaration keyword. Scalar
; ordinal constants are retained for expressions, subranges, and array bounds.
p_parse_const_section:
      JSR   p_capture_identifier
      long_bcs @fail
@name:
      JSR   p_save_identifier
      LDA   #TYPE_WORD
      STA   p_decl_type
      JSR   p_skip_ws
      JSR   p_peek
      BCC   @fail
      CMP   #':'
      BNE   @equals
      JSR   p_next
      JSR   p_parse_decl_type
      BCS   @fail
@equals:
      LDA   #'='
      JSR   p_expect_char
      BCS   @fail
      JSR   p_skip_ws
      JSR   p_peek
      BCC   @fail
      CMP   #'('
      BNE   @set
      JSR   p_parse_const_aggregate
      BCS   @fail
      BRA   @semicolon
@set:
      CMP   #'['
      BNE   @scalar
      JSR   p_parse_const_set
      BCS   @fail
      BRA   @semicolon
@scalar:
      JSR   p_parse_const_scalar
      BCS   @fail
      JSR   p_const_add
      BCS   @fail
@semicolon:
      LDA   #';'
      JSR   p_expect_char
      BCS   @fail
      JSR   p_capture_identifier
      BCS   @fail
      LDA   #<kw_type
      LDX   #>kw_type
      JSR   p_ident_is
      BCS   @done
      LDA   #<kw_var
      LDX   #>kw_var
      JSR   p_ident_is
      BCS   @done
      LDA   #<kw_begin
      LDX   #>kw_begin
      JSR   p_ident_is
      BCS   @done
      JSR   p_is_routine_keyword
      BCS   @done
      BRA   @name
@done:
      ; Typed constants emit into RODATA. Restore the executable section
      ; before a following nested routine is emitted; otherwise its first
      ; inline literal becomes fall-through machine code.
      LDA   #<asm_code
      LDX   #>asm_code
      JMP   p_emit_ax_text
@fail:
      SEC
      RTS

; Turbo typed constants are addressable read-only objects. Aggregate ordinal
; initializers become ordinary assembler data and enter the symbol table so
; selector code can treat them exactly like arrays.
p_parse_const_aggregate:
      LDA   p_decl_type
      STA   p_target_type
      CMP   #P_TYPE_USER_BASE
      long_bcc @fail
      SEC
      SBC   #P_TYPE_USER_BASE
      TAX
      CPX   p_type_count
      long_bcs @fail
      LDA   p_type_kind,X
      CMP   #TYPE_KIND_ARRAY
      long_bne @fail
      LDA   p_type_element,X
      STA   p_const_element_type
      LDA   p_type_size_l,X
      STA   p_const_total_size
      LDA   p_type_size_h,X
      STA   p_const_total_size+1
      LDA   p_const_element_type
      JSR   p_type_size
      long_bcs @fail
      LDA   p_array_len
      STA   p_const_element_size
      LDA   p_array_len+1
      STA   p_const_element_size+1
      JSR   p_restore_saved_identifier
      JSR   p_symbol_add
      long_bcs @fail
      LDX   p_symbol_count
      DEX
      STX   p_symbol_iter
      JSR   p_symbol_pointer
      LDY   #1
      LDA   p_target_type
      STA   (p_word),Y
      LDA   #<asm_rodata
      LDX   #>asm_rodata
      JSR   p_emit_ax_text
      long_bcs @fail
      LDX   p_symbol_iter
      JSR   p_emit_symbol_name
      BCS   @fail
      LDA   #<asm_const_bytes
      LDX   #>asm_const_bytes
      JSR   p_emit_ax_text
      BCS   @fail
      JSR   p_next
      STZ   p_type_group_count
      STZ   p_const_bytes
      STZ   p_const_bytes+1
@value:
      LDA   p_const_element_type
      JSR   p_type_is_string
      BCS   @string
      LDA   p_const_element_type
      STA   p_decl_type
      JSR   p_parse_const_scalar
      BCS   @fail
      LDA   p_decimal
      JSR   p_emit_const_byte
      BCS   @fail
      LDA   p_const_element_size+1
      BNE   @fail
      LDA   p_const_element_size
      CMP   #1
      BEQ   @delimiter
      CMP   #2
      BNE   @fail
      LDA   p_decimal+1
      JSR   p_emit_const_byte
      BCS   @fail
      BRA   @delimiter
@string:
      JSR   p_parse_const_string
      BCS   @fail
@delimiter:
      JSR   p_skip_ws
      JSR   p_peek
      BCC   @fail
      CMP   #','
      BNE   @close
      JSR   p_next
      BRA   @value
@close:
      LDA   #')'
      JSR   p_expect_char
      BCS   @fail
      LDA   p_const_bytes
      CMP   p_const_total_size
      BNE   @fail
      LDA   p_const_bytes+1
      CMP   p_const_total_size+1
      BNE   @fail
      LDA   #$0A
      JSR   p_emit
      RTS
@fail:
      SEC
      RTS

p_emit_const_byte:
      STA   p_char
      LDA   p_type_group_count
      BEQ   @value
      CMP   #16
      BCC   @comma
      LDA   #<asm_const_cont
      LDX   #>asm_const_cont
      JSR   p_emit_ax_text
      BCS   @fail
      STZ   p_type_group_count
      BRA   @value
@comma:
      LDA   #','
      JSR   p_emit
      BCS   @fail
@value:
      LDA   #'$'
      JSR   p_emit
      BCS   @fail
      LDA   p_char
      JSR   p_emit_hex_byte
      BCS   @fail
      INC   p_type_group_count
      INC   p_const_bytes
      BNE   @ok
      INC   p_const_bytes+1
@ok:
      CLC
@fail:
      RTS

p_parse_const_string:
      LDA   p_const_element_size+1
      BNE   @fail
      LDA   p_const_element_size
      BEQ   @fail
      DEC
      STA   p_const_remaining
      JSR   p_skip_ws
      LDA   #$27
      JSR   p_expect_char
      BCS   @fail
      JSR   p_scan_string_length
      BCS   @fail
      LDA   p_string_len
      CMP   p_const_remaining
      BCC   @length
      BEQ   @length
      BRA   @fail
@length:
      JSR   p_emit_const_byte
      BCS   @fail
@char:
      JSR   p_next
      BCC   @fail
      CMP   #$27
      BNE   @emit
      JSR   p_peek
      BCC   @pad
      CMP   #$27
      BNE   @pad
      JSR   p_next
      LDA   #$27
@emit:
      JSR   p_emit_const_byte
      BCS   @fail
      DEC   p_const_remaining
      BRA   @char
@pad:
      LDA   p_const_remaining
      BEQ   @done
      LDA   #0
      JSR   p_emit_const_byte
      BCS   @fail
      DEC   p_const_remaining
      BRA   @pad
@done:
      CLC
@fail:
      RTS

; A typed set constant is emitted as its declared packed byte width and kept
; as an addressable symbol. The symbol's otherwise-unused global offset fields
; retain that width so expression loading can distinguish it from byte arrays.
p_parse_const_set:
      LDA   p_decl_type
      STA   p_target_type
      LDA   p_array_len
      STA   p_type_saved_size
      LDA   p_array_len+1
      STA   p_type_saved_size+1
      JSR   p_parse_set_bits
      BCS   @fail
      JSR   p_restore_saved_identifier
      JSR   p_symbol_add
      BCS   @fail
      LDX   p_symbol_count
      DEX
      STX   p_symbol_iter
      JSR   p_symbol_pointer
      LDY   #1
      LDA   p_target_type
      STA   (p_word),Y
      LDY   #SYMBOL_OFFSET_LO
      LDA   p_type_saved_size
      STA   (p_word),Y
      INY
      LDA   p_type_saved_size+1
      STA   (p_word),Y
      LDA   #<asm_rodata
      LDX   #>asm_rodata
      JSR   p_emit_ax_text
      BCS   @fail
      LDX   p_symbol_iter
      JSR   p_emit_symbol_name
      BCS   @fail
      LDA   #<asm_const_bytes
      LDX   #>asm_const_bytes
      JSR   p_emit_ax_text
      BCS   @fail
      STZ   p_set_start
@byte:
      LDX   p_set_start
      CPX   p_type_saved_size
      BCS   @done
      CPX   #0
      BEQ   :+
      LDA   #','
      JSR   p_emit
      BCS   @fail
:     LDA   #'$'
      JSR   p_emit
      BCS   @fail
      LDA   p_set_bits,X
      JSR   p_emit_hex_byte
      BCS   @fail
      INC   p_set_start
      BRA   @byte
@done:
      LDA   #$0A
      JMP   p_emit
@fail:
      SEC
      RTS

p_restore_saved_identifier:
      LDA   p_saved_ident_len
      STA   p_ident_len
      TAX
@copy:
      DEX
      BMI   @hash_start
      LDA   p_saved_ident,X
      STA   p_ident,X
      BRA   @copy
@hash_start:
      LDX   #2
@hash:
      LDA   p_saved_hash,X
      STA   p_ident_hash,X
      DEX
      BPL   @hash
      RTS

p_parse_const_scalar:
      JSR   p_skip_ws
      JSR   p_peek
      long_bcc @fail
      CMP   #'^'
      long_beq @control
      CMP   #'#'
      long_beq @numeric_character
      CMP   #$27
      BEQ   @character
      CMP   #'0'
      BCC   @identifier
      CMP   #'9'+1
      long_bcc @decimal
@identifier:
      JSR   p_capture_identifier
      long_bcs @fail
      LDA   #<kw_true
      LDX   #>kw_true
      JSR   p_ident_is
      BCC   @false
      LDA   #1
      STA   p_decimal
      STZ   p_decimal+1
      LDA   #TYPE_BOOLEAN
      STA   p_decl_type
      CLC
      RTS
@false:
      LDA   #<kw_false
      LDX   #>kw_false
      JSR   p_ident_is
      BCC   @named
      STZ   p_decimal
      STZ   p_decimal+1
      LDA   #TYPE_BOOLEAN
      STA   p_decl_type
      CLC
      RTS
@named:
      JSR   p_const_find
      BCC   @fail
      STX   p_const_iter
      LDA   p_const_value_l,X
      STA   p_decimal
      LDA   p_const_value_h,X
      STA   p_decimal+1
      LDA   p_const_type,X
      STA   p_decl_type
      CLC
      RTS
@character:
      JSR   p_next
      JSR   p_next
      BCC   @fail
      STA   p_decimal
      STZ   p_decimal+1
      LDA   #$27
      JSR   p_expect_char
      BCS   @fail
      LDA   #TYPE_CHAR
      STA   p_decl_type
      CLC
      RTS
@control:
      JSR   p_next
      JSR   p_next
      BCC   @fail
      AND   #$1F
      STA   p_decimal
      STZ   p_decimal+1
      LDA   #TYPE_CHAR
      STA   p_decl_type
      CLC
      RTS
@numeric_character:
      JSR   p_next
      JSR   p_parse_decimal_literal
      BCS   @fail
      LDA   p_decimal+1
      BNE   @fail
      LDA   #TYPE_CHAR
      STA   p_decl_type
      CLC
      RTS
@decimal:
      JSR   p_parse_decimal_literal
      BCS   @fail
      LDA   p_decimal+1
      BNE   @word
      LDA   p_decl_type
      CMP   #TYPE_WORD
      BNE   @ok
      LDA   #TYPE_BYTE
      STA   p_decl_type
@ok:
      CLC
      RTS
@word:
      LDA   #TYPE_WORD
      STA   p_decl_type
      CLC
      RTS
@fail:
      SEC
      RTS

p_const_add:
      LDX   #0
@duplicate:
      CPX   p_const_count
      BCS   @store
      STX   p_const_iter
      JSR   p_const_name_pointer
      LDY   #0
      LDA   p_const_name_len,X
      CMP   p_saved_ident_len
      BNE   @next
@compare:
      CPY   p_saved_ident_len
      BCS   @fail
      LDA   p_saved_ident,Y
      CMP   (p_word),Y
      BNE   @next
      INY
      BRA   @compare
@next:
      LDX   p_const_iter
      INX
      BRA   @duplicate
@store:
      CPX   #P_CONST_CAP
      BCS   @fail
      LDA   p_saved_ident_len
      STA   p_const_name_len,X
      LDA   p_decl_type
      STA   p_const_type,X
      LDA   p_decimal
      STA   p_const_value_l,X
      LDA   p_decimal+1
      STA   p_const_value_h,X
      JSR   p_const_name_pointer
      LDY   #0
@copy:
      CPY   p_saved_ident_len
      BCS   @stored
      LDA   p_saved_ident,Y
      STA   (p_word),Y
      INY
      BRA   @copy
@stored:
      INC   p_const_count
      CLC
      RTS
@fail:
      SEC
      RTS

p_const_find:
      LDX   #0
@constant:
      CPX   p_const_count
      BCS   @missing
      STX   p_const_iter
      LDA   p_const_name_len,X
      CMP   p_ident_len
      BNE   @next
      JSR   p_const_name_pointer
      LDY   #0
@compare:
      CPY   p_ident_len
      BCS   @found
      LDA   p_ident,Y
      CMP   (p_word),Y
      BNE   @next
      INY
      BRA   @compare
@next:
      LDX   p_const_iter
      INX
      BRA   @constant
@found:
      LDX   p_const_iter
      LDA   p_const_type,X
      SEC
      RTS
@missing:
      CLC
      RTS

p_const_name_pointer:
      TXA
      STA   p_remainder
      STZ   p_remainder+1
      LDY   #5
@times32:
      ASL   p_remainder
      ROL   p_remainder+1
      DEY
      BNE   @times32
      CLC
      LDA   p_remainder
      ADC   #<P_STATE_CONST_NAME_OFFSET
      STA   p_state_offset
      LDA   p_remainder+1
      ADC   #>P_STATE_CONST_NAME_OFFSET
      STA   p_state_offset+1
      JMP   p_state_pointer

; User type descriptors retain the Pascal name and the layout facts needed by
; later declaration and selector code.  IDs $10..$2F index these tables.
      .segment "CODE"
p_type_save_decl_name:
      LDA   p_ident_len
      STA   p_type_decl_len
      TAX
@copy:
      DEX
      BMI   @done
      LDA   p_ident,X
      STA   p_type_decl_name,X
      BRA   @copy
@done:
      RTS

p_type_add:
      LDX   p_type_count
      CPX   #P_TYPE_CAP
      BCS   @fail
      STX   p_type_iter
      JSR   p_type_name_pointer
      LDY   #0
@copy:
      CPY   p_type_decl_len
      BCS   @meta
      LDA   p_type_decl_name,Y
      STA   (p_word),Y
      INY
      BRA   @copy
@meta:
      LDX   p_type_iter
      LDA   p_type_decl_len
      STA   p_type_name_len,X
      LDA   p_type_work_kind
      STA   p_type_kind,X
      LDA   p_array_len
      STA   p_type_size_l,X
      LDA   p_array_len+1
      STA   p_type_size_h,X
      LDA   p_type_work_low
      STA   p_type_low_l,X
      LDA   p_type_work_low+1
      STA   p_type_low_h,X
      LDA   p_type_work_high
      STA   p_type_high_l,X
      LDA   p_type_work_high+1
      STA   p_type_high_h,X
      LDA   p_type_work_element
      STA   p_type_element,X
      TXA
      CLC
      ADC   #P_TYPE_USER_BASE
      STA   p_decl_type
      INC   p_type_count
      CLC
      RTS
@fail:
      SEC
      RTS

; Find the captured identifier. Carry set returns A=type id and X=table row.
p_type_find:
      LDX   #0
@type:
      CPX   p_type_count
      BCS   @missing
      STX   p_type_iter
      LDA   p_type_name_len,X
      CMP   p_ident_len
      BNE   @next
      JSR   p_type_name_pointer
      LDY   #0
@compare:
      CPY   p_ident_len
      BCS   @found
      LDA   (p_word),Y
      CMP   p_ident,Y
      BNE   @next
      INY
      BRA   @compare
@found:
      LDX   p_type_iter
      TXA
      CLC
      ADC   #P_TYPE_USER_BASE
      LDX   p_type_iter
      SEC
      RTS
@next:
      LDX   p_type_iter
      INX
      BRA   @type
@missing:
      CLC
      RTS

p_type_name_pointer:
      TXA
      STA   p_remainder
      STZ   p_remainder+1
      LDY   #5
@times32:
      ASL   p_remainder
      ROL   p_remainder+1
      DEY
      BNE   @times32
      CLC
      LDA   p_remainder
      ADC   #<P_STATE_TYPE_NAME_OFFSET
      STA   p_state_offset
      LDA   p_remainder+1
      ADC   #>P_STATE_TYPE_NAME_OFFSET
      STA   p_state_offset+1
      JMP   p_state_pointer

; Return the byte size for A in p_array_len.
p_type_size:
      CMP   #P_TYPE_USER_BASE
      BCS   @user
      CMP   #TYPE_WORD
      BEQ   @word
      CMP   #TYPE_STRING_REF
      BEQ   @word
      CMP   #TYPE_SET_REF
      BEQ   @word
      CMP   #TYPE_REAL
      BEQ   @real
      CMP   #TYPE_FILE
      BEQ   @file
      CMP   #TYPE_TEXT
      BEQ   @file
      LDA   #1
      BRA   @set
@word:
      LDA   #2
      BRA   @set
@real:
      LDA   #4
      BRA   @set
@file:
      LDA   #16
@set:
      STA   p_array_len
      STZ   p_array_len+1
      CLC
      RTS
@user:
      SEC
      SBC   #P_TYPE_USER_BASE
      TAX
      CPX   p_type_count
      BCS   @fail
      LDA   p_type_size_l,X
      STA   p_array_len
      LDA   p_type_size_h,X
      STA   p_array_len+1
      CLC
      RTS
@fail:
      SEC
      RTS

p_parse_type_section:
      JSR   p_capture_identifier
      BCS   @fail
@definition:
      JSR   p_type_save_decl_name
      LDA   #'='
      JSR   p_expect_char
      BCS   @fail
      JSR   p_parse_type_spec
      BCS   @fail
      JSR   p_type_add
      BCS   @fail
      LDA   #';'
      JSR   p_expect_char
      BCS   @fail
      JSR   p_capture_identifier
      BCS   @fail
      LDA   #<kw_const
      LDX   #>kw_const
      JSR   p_ident_is
      BCS   @done
      LDA   #<kw_var
      LDX   #>kw_var
      JSR   p_ident_is
      BCS   @done
      LDA   #<kw_begin
      LDX   #>kw_begin
      JSR   p_ident_is
      BCS   @done
      JSR   p_is_routine_keyword
      BCS   @done
      BRA   @definition
@done:
      CLC
      RTS
@fail:
      SEC
      RTS

; Parse a Turbo Pascal layout type. p_decl_type identifies scalar/named types;
; p_array_len always carries the concrete storage size.
p_parse_type_spec:
      STZ   p_type_work_kind
      STZ   p_type_work_low
      STZ   p_type_work_low+1
      STZ   p_type_work_high
      STZ   p_type_work_high+1
      STZ   p_type_work_element
      JSR   p_skip_ws
      JSR   p_peek
      long_bcc @fail
      CMP   #'('
      long_beq p_parse_enum_type
      CMP   #$27
      long_beq @subrange
      CMP   #'0'
      long_bcc @identifier
      CMP   #'9'+1
      long_bcc @subrange
@identifier:
      JSR   p_capture_identifier
      long_bcs @fail
      LDA   #<kw_byte
      LDX   #>kw_byte
      JSR   p_ident_is
      long_bcc @boolean
      LDA   #TYPE_BYTE
      JMP   @scalar
@boolean:
      LDA   #<kw_boolean
      LDX   #>kw_boolean
      JSR   p_ident_is
      long_bcc @char
      LDA   #TYPE_BOOLEAN
      JMP   @scalar
@char:
      LDA   #<kw_char
      LDX   #>kw_char
      JSR   p_ident_is
      long_bcc @word
      LDA   #TYPE_CHAR
      JMP   @scalar
@word:
      LDA   #<kw_word
      LDX   #>kw_word
      JSR   p_ident_is
      long_bcs @wide
      LDA   #<kw_integer
      LDX   #>kw_integer
      JSR   p_ident_is
      long_bcc @real
@wide:
      LDA   #TYPE_WORD
      JMP   @scalar
@real:
      LDA   #<kw_real
      LDX   #>kw_real
      JSR   p_ident_is
      BCC   @string
      LDA   #TYPE_REAL
      JMP   @scalar
@string:
      LDA   #<kw_string
      LDX   #>kw_string
      JSR   p_ident_is
      long_bcs p_parse_string_type
      LDA   #<kw_set
      LDX   #>kw_set
      JSR   p_ident_is
      long_bcs p_parse_set_type
      LDA   #<kw_record
      LDX   #>kw_record
      JSR   p_ident_is
      long_bcs p_parse_record_type
      LDA   #<kw_array
      LDX   #>kw_array
      JSR   p_ident_is
      long_bcs p_parse_array_type
      LDA   #<kw_text
      LDX   #>kw_text
      JSR   p_ident_is
      BCS   @text
      LDA   #<kw_file
      LDX   #>kw_file
      JSR   p_ident_is
      BCC   @named
      JSR   p_peek_keyword_of
      BCC   @untyped_file
      JSR   p_consume_of
      JSR   p_parse_type_spec
      BCS   @fail
      LDA   p_decl_type
      STA   p_type_work_element
      LDA   #TYPE_KIND_FILE
      STA   p_type_work_kind
      LDA   #16
      STA   p_array_len
      STZ   p_array_len+1
      LDA   #TYPE_ARRAY_BYTE
      STA   p_decl_type
      CLC
      RTS
@text:
      LDA   #TYPE_CHAR
      STA   p_type_work_element
      LDA   #TYPE_KIND_FILE
      STA   p_type_work_kind
      LDA   #TYPE_TEXT
      BRA   @file_scalar
@untyped_file:
      STZ   p_type_work_element
      LDA   #TYPE_KIND_FILE
      STA   p_type_work_kind
      LDA   #TYPE_FILE
@file_scalar:
      STA   p_decl_type
      JSR   p_type_size
      RTS
@named:
      JSR   p_type_find
      BCC   @fail
      STA   p_decl_type
      JSR   p_type_size
      RTS
@scalar:
      STA   p_decl_type
      JSR   p_type_size
      LDA   #TYPE_KIND_ORDINAL
      STA   p_type_work_kind
      RTS
@subrange:
      JMP   p_parse_subrange_type
@fail:
      SEC
      RTS

p_parse_subrange_type:
      LDA   #TYPE_WORD
      STA   p_decl_type
      JSR   p_parse_const_scalar
      BCS   @fail
      LDA   p_decl_type
      STA   p_type_work_element
      LDA   p_decimal
      STA   p_type_work_low
      LDA   p_decimal+1
      STA   p_type_work_low+1
      LDA   #'.'
      JSR   p_expect_char
      BCS   @fail
      LDA   #'.'
      JSR   p_expect_char
      BCS   @fail
      LDA   #TYPE_WORD
      STA   p_decl_type
      JSR   p_parse_const_scalar
      BCS   @fail
      LDA   p_decimal
      STA   p_type_work_high
      LDA   p_decimal+1
      STA   p_type_work_high+1
      LDA   #TYPE_KIND_ORDINAL
      STA   p_type_work_kind
      LDA   p_type_work_low+1
      ORA   p_type_work_high+1
      BNE   @wide
      LDA   #TYPE_BYTE
      BRA   @finish
@wide:
      LDA   #TYPE_WORD
@finish:
      STA   p_decl_type
      JSR   p_type_size
      RTS
@fail:
      SEC
      RTS

p_parse_enum_type:
      JSR   p_next
      STZ   p_type_total
      STZ   p_type_total+1
@member:
      JSR   p_capture_identifier
      BCS   @fail
      JSR   p_save_identifier
      LDA   p_type_total
      STA   p_decimal
      STZ   p_decimal+1
      LDA   #TYPE_BYTE
      STA   p_decl_type
      JSR   p_const_add
      BCS   @fail
      INC   p_type_total
      JSR   p_skip_ws
      JSR   p_peek
      BCC   @fail
      CMP   #','
      BNE   @close
      JSR   p_next
      BRA   @member
@close:
      LDA   #')'
      JSR   p_expect_char
      BCS   @fail
      STZ   p_type_work_low
      STZ   p_type_work_low+1
      LDA   p_type_total
      DEC
      STA   p_type_work_high
      STZ   p_type_work_high+1
      LDA   #TYPE_KIND_ORDINAL
      STA   p_type_work_kind
      LDA   #TYPE_BYTE
      STA   p_decl_type
      JSR   p_type_size
      RTS
@fail:
      SEC
      RTS

p_peek_keyword_of:
      LDA   #<kw_of
      LDX   #>kw_of
      JMP   p_peek_keyword

p_consume_of:
      LDA   #<kw_of
      STA   p_word
      LDA   #>kw_of
      STA   p_word+1
      JMP   p_expect_word

p_parse_string_type:
      LDA   #$00
      STA   p_array_len
      LDA   #$01
      STA   p_array_len+1
      LDA   #$FF
      STA   p_type_work_high
      STZ   p_type_work_high+1
      JSR   p_skip_ws
      JSR   p_peek
      BCC   @done
      CMP   #'['
      BNE   @done
      JSR   p_next
      LDA   #TYPE_WORD
      STA   p_decl_type
      JSR   p_parse_const_scalar
      BCS   @fail
      LDA   p_decimal
      STA   p_type_work_high
      LDA   p_decimal+1
      STA   p_type_work_high+1
      INC   p_decimal
      BNE   :+
      INC   p_decimal+1
:     LDA   p_decimal
      STA   p_array_len
      LDA   p_decimal+1
      STA   p_array_len+1
      LDA   #']'
      JSR   p_expect_char
      BCS   @fail
@done:
      LDA   #TYPE_KIND_STRING
      STA   p_type_work_kind
      LDA   #TYPE_ARRAY_BYTE
      STA   p_decl_type
      CLC
      RTS
@fail:
      SEC
      RTS

p_parse_set_type:
      JSR   p_consume_of
      long_bcs @fail
      JSR   p_capture_identifier
      long_bcs @fail
      LDA   #<kw_char
      LDX   #>kw_char
      JSR   p_ident_is
      BCC   @named
      STZ   p_type_work_low
      STZ   p_type_work_low+1
      LDA   #$FF
      STA   p_type_work_high
      STZ   p_type_work_high+1
      BRA   @size
@named:
      JSR   p_type_find
      BCC   @fail
      LDA   p_type_low_l,X
      STA   p_type_work_low
      LDA   p_type_low_h,X
      STA   p_type_work_low+1
      LDA   p_type_high_l,X
      STA   p_type_work_high
      LDA   p_type_high_h,X
      STA   p_type_work_high+1
@size:
      SEC
      LDA   p_type_work_high
      SBC   p_type_work_low
      STA   p_array_len
      LDA   p_type_work_high+1
      SBC   p_type_work_low+1
      STA   p_array_len+1
      CLC
      LDA   p_array_len
      ADC   #8
      STA   p_array_len
      BCC   :+
      INC   p_array_len+1
:     LDX   #3
@shift:
      LSR   p_array_len+1
      ROR   p_array_len
      DEX
      BNE   @shift
      LDA   #TYPE_KIND_SET
      STA   p_type_work_kind
      LDA   #TYPE_ARRAY_BYTE
      STA   p_decl_type
      CLC
      RTS
@fail:
      SEC
      RTS

p_field_add_name:
      LDX   p_field_count
      CPX   #P_FIELD_CAP
      BCS   @fail
      CLC
      LDA   p_field_name_used
      ADC   p_ident_len
      BCS   @fail
      STA   p_type_meta_count
      LDA   p_field_name_used
      STA   p_field_name_offset,X
      LDA   p_ident_len
      STA   p_field_name_len,X
      CLC
      LDA   #<p_field_names
      ADC   p_field_name_used
      STA   p_word
      LDA   #>p_field_names
      ADC   #0
      STA   p_word+1
      LDY   #0
@copy:
      CPY   p_ident_len
      BCS   @done
      LDA   p_ident,Y
      STA   (p_word),Y
      INY
      BRA   @copy
@done:
      LDA   p_type_meta_count
      STA   p_field_name_used
      INC   p_field_count
      CLC
      RTS
@fail:
      SEC
      RTS

p_dim_add:
      LDX   p_dim_count
      CPX   #P_DIM_CAP
      BCS   @fail
      LDA   p_type_work_low
      STA   p_dim_low_l,X
      LDA   p_type_work_low+1
      STA   p_dim_low_h,X
      LDA   p_type_extent
      STA   p_dim_extent_l,X
      LDA   p_type_extent+1
      STA   p_dim_extent_h,X
      INC   p_dim_count
      CLC
      RTS
@fail:
      SEC
      RTS

; Starting with the element size in p_type_total, assign row-major strides
; from the last dimension backwards and return the complete array size.
p_finalize_array_dims:
      LDX   p_dim_count
      CPX   p_type_meta_start
      BEQ   @fail
      DEX
@dimension:
      STX   p_dim_iter
      LDA   p_type_total
      STA   p_dim_stride_l,X
      LDA   p_type_total+1
      STA   p_dim_stride_h,X
      LDA   p_dim_extent_l,X
      STA   p_type_extent
      LDA   p_dim_extent_h,X
      STA   p_type_extent+1
      JSR   p_multiply_type_total
      BCS   @fail
      LDX   p_dim_iter
      CPX   p_type_meta_start
      BEQ   @done
      DEX
      BRA   @dimension
@done:
      CLC
      RTS
@fail:
      SEC
      RTS

p_parse_record_type:
      STZ   p_type_total
      STZ   p_type_total+1
      LDA   p_field_count
      STA   p_type_meta_start
@field:
      JSR   p_capture_identifier
      long_bcs @fail
      LDA   #<kw_end
      LDX   #>kw_end
      JSR   p_ident_is
      long_bcs @done
      LDA   p_field_count
      STA   p_type_group_start
      JSR   p_field_add_name
      long_bcs @fail
@more_names:
      JSR   p_skip_ws
      JSR   p_peek
      long_bcc @fail
      CMP   #','
      BNE   @field_type
      JSR   p_next
      JSR   p_capture_identifier
      long_bcs @fail
      JSR   p_field_add_name
      long_bcs @fail
      BRA   @more_names
@field_type:
      LDA   #':'
      JSR   p_expect_char
      long_bcs @fail
      JSR   p_parse_decl_type
      long_bcs @fail
      LDA   p_array_len
      STA   p_type_extent
      LDA   p_array_len+1
      STA   p_type_extent+1
      LDX   p_type_group_start
@bind_field:
      CPX   p_field_count
      BCS   @fields_bound
      LDA   p_decl_type
      STA   p_field_type,X
      LDA   p_type_total
      STA   p_field_offset_l,X
      LDA   p_type_total+1
      STA   p_field_offset_h,X
      LDA   p_type_extent
      STA   p_field_size_l,X
      LDA   p_type_extent+1
      STA   p_field_size_h,X
      CLC
      LDA   p_type_total
      ADC   p_type_extent
      STA   p_type_total
      LDA   p_type_total+1
      ADC   p_type_extent+1
      STA   p_type_total+1
      BCS   @fail
      INX
      BRA   @bind_field
@fields_bound:
      LDA   #';'
      JSR   p_expect_char
      BCS   @fail
      JMP   @field
@done:
      LDA   p_type_total
      STA   p_array_len
      LDA   p_type_total+1
      STA   p_array_len+1
      LDA   p_type_meta_start
      STA   p_type_work_low
      SEC
      LDA   p_field_count
      SBC   p_type_meta_start
      STA   p_type_work_low+1
      LDA   #TYPE_KIND_RECORD
      STA   p_type_work_kind
      LDA   #TYPE_ARRAY_BYTE
      STA   p_decl_type
      CLC
      RTS
@fail:
      SEC
      RTS

p_parse_array_type:
      LDA   #'['
      JSR   p_expect_char
      BCS   @fail
      LDA   p_dim_count
      STA   p_type_meta_start
@dimension:
      JSR   p_parse_index_extent
      BCS   @fail
      JSR   p_dim_add
      BCS   @fail
      JSR   p_skip_ws
      JSR   p_peek
      BCC   @fail
      CMP   #','
      BNE   @close
      JSR   p_next
      BRA   @dimension
@close:
      LDA   #']'
      JSR   p_expect_char
      BCS   @fail
      JSR   p_consume_of
      BCS   @fail
      JSR   p_parse_decl_type
      BCS   @fail
      LDA   p_decl_type
      STA   p_type_work_element
      LDA   p_array_len
      STA   p_type_total
      LDA   p_array_len+1
      STA   p_type_total+1
      JSR   p_finalize_array_dims
      BCS   @fail
      LDA   p_type_total
      STA   p_array_len
      LDA   p_type_total+1
      STA   p_array_len+1
      LDA   p_type_meta_start
      STA   p_type_work_low
      SEC
      LDA   p_dim_count
      SBC   p_type_meta_start
      STA   p_type_work_low+1
      LDA   #TYPE_KIND_ARRAY
      STA   p_type_work_kind
      LDA   #TYPE_ARRAY_BYTE
      STA   p_decl_type
      CLC
      RTS
@fail:
      SEC
      RTS

p_parse_index_extent:
      JSR   p_skip_ws
      JSR   p_peek
      long_bcc @fail
      CMP   #'A'
      BCC   @range
      CMP   #'Z'+1
      BCS   @range
      JSR   p_capture_identifier
      long_bcs @fail
      JSR   p_type_find
      BCC   @named_constant
      LDA   p_type_low_l,X
      STA   p_type_work_low
      LDA   p_type_low_h,X
      STA   p_type_work_low+1
      LDA   p_type_high_l,X
      STA   p_type_work_high
      LDA   p_type_high_h,X
      STA   p_type_work_high+1
      BRA   @extent
@named_constant:
      JSR   p_const_find
      BCC   @fail
      LDA   p_const_value_l,X
      STA   p_type_work_low
      LDA   p_const_value_h,X
      STA   p_type_work_low+1
      BRA   @dots
@range:
      LDA   #TYPE_WORD
      STA   p_decl_type
      JSR   p_parse_const_scalar
      BCS   @fail
      LDA   p_decimal
      STA   p_type_work_low
      LDA   p_decimal+1
      STA   p_type_work_low+1
@dots:
      LDA   #'.'
      JSR   p_expect_char
      BCS   @fail
      LDA   #'.'
      JSR   p_expect_char
      BCS   @fail
      LDA   #TYPE_WORD
      STA   p_decl_type
      JSR   p_parse_const_scalar
      BCS   @fail
      LDA   p_decimal
      STA   p_type_work_high
      LDA   p_decimal+1
      STA   p_type_work_high+1
@extent:
      SEC
      LDA   p_type_work_high
      SBC   p_type_work_low
      STA   p_type_extent
      LDA   p_type_work_high+1
      SBC   p_type_work_low+1
      STA   p_type_extent+1
      INC   p_type_extent
      BNE   :+
      INC   p_type_extent+1
:     LDA   p_type_extent
      ORA   p_type_extent+1
      BEQ   @fail
      CLC
      RTS
@fail:
      SEC
      RTS

p_multiply_type_total:
      LDA   p_type_total
      STA   p_type_saved_size
      LDA   p_type_total+1
      STA   p_type_saved_size+1
      STZ   p_decimal
      STZ   p_decimal+1
@add:
      LDA   p_type_extent
      ORA   p_type_extent+1
      BEQ   @done
      CLC
      LDA   p_decimal
      ADC   p_type_saved_size
      STA   p_decimal
      LDA   p_decimal+1
      ADC   p_type_saved_size+1
      STA   p_decimal+1
      BCS   @fail
      LDA   p_type_extent
      BNE   :+
      DEC   p_type_extent+1
:     DEC   p_type_extent
      BRA   @add
@done:
      LDA   p_decimal
      STA   p_type_total
      LDA   p_decimal+1
      STA   p_type_total+1
      CLC
      RTS
@fail:
      SEC
      RTS

; Parse and bind one Pascal formal-parameter list. Parameters occupy lexical
; frame slots; structured value parameters use a two-byte source pointer and
; are copied by the eventual frame prologue, while explicit VAR parameters
; remain references.
p_parse_formal_parameters:
      JSR   p_next
      JSR   p_skip_ws
      JSR   p_peek
      BCC   @fail
      CMP   #')'
      BEQ   @close
@group:
      LDA   #SYM_PARAM
      STA   p_param_flags
      JSR   p_capture_identifier
      BCS   @fail
      LDA   #<kw_var
      LDX   #>kw_var
      JSR   p_ident_is
      BCC   @names
      LDA   #SYM_PARAM|SYM_BYREF
      STA   p_param_flags
      JSR   p_capture_identifier
      BCS   @fail
@names:
      LDA   p_symbol_count
      STA   p_symbol_group
@name:
      JSR   p_symbol_add
      BCS   @fail
      JSR   p_skip_ws
      JSR   p_peek
      BCC   @fail
      CMP   #','
      BNE   @type
      JSR   p_next
      JSR   p_capture_identifier
      BCS   @fail
      BRA   @name
@type:
      LDA   #':'
      JSR   p_expect_char
      BCS   @fail
      JSR   p_parse_decl_type
      BCS   @fail
      JSR   p_bind_parameter_group
      BCS   @fail
      JSR   p_skip_ws
      JSR   p_peek
      BCC   @fail
      CMP   #';'
      BNE   @close
      JSR   p_next
      BRA   @group
@close:
      LDA   #')'
      JMP   p_expect_char
@fail:
      SEC
      RTS

p_bind_parameter_group:
      LDA   p_param_flags
      AND   #SYM_BYREF
      BNE   @pointer
      LDA   p_decl_type
      CMP   #TYPE_ARRAY_BYTE
      BEQ   @have_size
      JSR   p_type_size
      long_bcs @fail
@have_size:
      LDA   p_array_len+1
      BNE   @pointer
      LDA   p_array_len
      CMP   #3
      BCC   @slot
@pointer:
      LDA   #2
@slot:
      STA   p_type_extent
      STZ   p_type_extent+1
      LDX   p_symbol_group
@symbol:
      CPX   p_symbol_count
      BCS   @done
      STX   p_symbol_iter
      JSR   p_symbol_pointer
      LDY   #1
      LDA   p_decl_type
      STA   (p_word),Y
      LDY   #SYMBOL_FLAGS
      LDA   p_param_flags
      STA   (p_word),Y
      LDY   #SYMBOL_OFFSET_LO
      LDA   p_frame_size
      STA   (p_word),Y
      INY
      LDA   p_frame_size+1
      STA   (p_word),Y
      INY
      LDA   p_scope_level
      STA   (p_word),Y
      CLC
      LDA   p_frame_size
      ADC   p_type_extent
      STA   p_frame_size
      LDA   p_frame_size+1
      ADC   p_type_extent+1
      STA   p_frame_size+1
      LDA   p_routine_param_count
      CMP   #8
      BCS   @fail
      LDA   p_param_flags
      AND   #SYM_BYREF
      BEQ   :+
      LDY   p_routine_param_count
      LDA   p_argument_bits,Y
      ORA   p_routine_param_refmask
      STA   p_routine_param_refmask
:
      LDA   p_type_extent
      CMP   #2
      BNE   :+
      LDY   p_routine_param_count
      LDA   p_argument_bits,Y
      ORA   p_routine_param_widthmask
      STA   p_routine_param_widthmask
:
      INC   p_routine_param_count
      LDX   p_symbol_iter
      INX
      BRA   @symbol
@done:
      CLC
@fail:
      RTS

p_parse_routine_declaration_part:
@section:
      LDA   #<kw_begin
      LDX   #>kw_begin
      JSR   p_ident_is
      BCS   @done
      LDA   #<kw_const
      LDX   #>kw_const
      JSR   p_ident_is
      BCC   @type
      JSR   p_parse_const_section
      BCS   @fail
      BRA   @section
@type:
      LDA   #<kw_type
      LDX   #>kw_type
      JSR   p_ident_is
      BCC   @var
      JSR   p_parse_type_section
      BCS   @fail
      BRA   @section
@var:
      LDA   #<kw_var
      LDX   #>kw_var
      JSR   p_ident_is
      BCC   @nested
      JSR   p_parse_local_var_section
      BCS   @fail
      BRA   @section
@nested:
      JSR   p_is_routine_keyword
      BCC   @fail
      LDX   p_current_routine
      LDA   p_routine_kind_table,X
      ORA   #P_ROUTINE_HAS_NESTED
      STA   p_routine_kind_table,X
      JSR   p_push_routine_context
      BCS   @fail
      JSR   p_parse_routine_declarations
      PHP
      JSR   p_pop_routine_context
      PLP
      BCS   @fail
      BRA   @section
@done:
      CLC
@fail:
      RTS

; Recursive declaration parsing needs only the enclosing routine's codegen
; context. Its symbols remain in the shared lexical table and retain their
; display depth, which is what lets nested routines address outer locals.
p_push_routine_context:
      LDX   p_routine_context_depth
      CPX   #P_ROUTINE_DEPTH_CAP
      BCS   @fail
      STX   p_routine_context_index
      LDA   p_scope_symbol_base
      STA   p_context_scope_base,X
      LDA   p_scope_param_end
      STA   p_context_param_end,X
      LDA   p_routine_param_count
      STA   p_context_param_count,X
      LDA   p_frame_size
      STA   p_context_frame_l,X
      LDA   p_frame_size+1
      STA   p_context_frame_h,X
      LDA   p_routine_kind
      STA   p_context_kind,X
      LDA   p_function_return_type
      STA   p_context_return_type,X
      LDA   p_function_active
      STA   p_context_function_active,X
      LDA   p_call_ident_len
      STA   p_context_name_len,X
      LDA   p_call_hash
      STA   p_context_hash0,X
      LDA   p_call_hash+1
      STA   p_context_hash1,X
      LDA   p_call_hash+2
      STA   p_context_hash2,X
      JSR   p_routine_context_name_pointer
      LDY   #0
@name:
      CPY   p_call_ident_len
      BCS   @done
      LDA   p_call_ident,Y
      STA   (p_word),Y
      INY
      BRA   @name
@done:
      INC   p_routine_context_depth
      CLC
@fail:
      RTS

p_pop_routine_context:
      LDX   p_routine_context_depth
      BEQ   @fail
      DEX
      STX   p_routine_context_depth
      STX   p_routine_context_index
      LDA   p_context_scope_base,X
      STA   p_scope_symbol_base
      LDA   p_context_param_end,X
      STA   p_scope_param_end
      LDA   p_context_param_count,X
      STA   p_routine_param_count
      LDA   p_context_frame_l,X
      STA   p_frame_size
      LDA   p_context_frame_h,X
      STA   p_frame_size+1
      LDA   p_context_kind,X
      STA   p_routine_kind
      LDA   p_context_return_type,X
      STA   p_function_return_type
      LDA   p_context_function_active,X
      STA   p_function_active
      LDA   p_context_name_len,X
      STA   p_call_ident_len
      STA   p_saved_ident_len
      LDA   p_context_hash0,X
      STA   p_call_hash
      STA   p_saved_hash
      STA   p_function_hash
      LDA   p_context_hash1,X
      STA   p_call_hash+1
      STA   p_saved_hash+1
      STA   p_function_hash+1
      LDA   p_context_hash2,X
      STA   p_call_hash+2
      STA   p_saved_hash+2
      STA   p_function_hash+2
      JSR   p_routine_context_name_pointer
      LDY   #0
@name:
      CPY   p_call_ident_len
      BCS   @done
      LDA   (p_word),Y
      STA   p_call_ident,Y
      STA   p_saved_ident,Y
      INY
      BRA   @name
@done:
      CLC
@fail:
      RTS

p_routine_context_name_pointer:
      LDA   p_routine_context_index
      STA   p_remainder
      STZ   p_remainder+1
      LDY   #5
@times32:
      ASL   p_remainder
      ROL   p_remainder+1
      DEY
      BNE   @times32
      CLC
      LDA   p_remainder
      ADC   #<p_context_names
      STA   p_word
      LDA   p_remainder+1
      ADC   #>p_context_names
      STA   p_word+1
      RTS

p_parse_local_var_section:
      JSR   p_capture_identifier
      BCS   @fail
@group:
      LDA   p_symbol_count
      STA   p_symbol_group
@name:
      JSR   p_symbol_add
      BCS   @fail
      JSR   p_skip_ws
      JSR   p_peek
      BCC   @fail
      CMP   #','
      BNE   @type
      JSR   p_next
      JSR   p_capture_identifier
      BCS   @fail
      BRA   @name
@type:
      LDA   #':'
      JSR   p_expect_char
      BCS   @fail
      JSR   p_parse_decl_type
      BCS   @fail
      JSR   p_bind_local_group
      BCS   @fail
      LDA   #';'
      JSR   p_expect_char
      BCS   @fail
      JSR   p_capture_identifier
      BCS   @fail
      LDA   #<kw_begin
      LDX   #>kw_begin
      JSR   p_ident_is
      BCS   @done
      LDA   #<kw_const
      LDX   #>kw_const
      JSR   p_ident_is
      BCS   @done
      LDA   #<kw_type
      LDX   #>kw_type
      JSR   p_ident_is
      BCS   @done
      JSR   p_is_routine_keyword
      BCS   @done
      BRA   @group
@done:
      CLC
@fail:
      RTS

p_bind_local_group:
      LDA   p_decl_type
      CMP   #TYPE_ARRAY_BYTE
      BEQ   @have_size
      JSR   p_type_size
      BCS   @fail
@have_size:
      LDA   p_array_len
      STA   p_type_extent
      LDA   p_array_len+1
      STA   p_type_extent+1
      LDX   p_symbol_group
@symbol:
      CPX   p_symbol_count
      BCS   @done
      STX   p_symbol_iter
      JSR   p_symbol_pointer
      LDY   #1
      LDA   p_decl_type
      STA   (p_word),Y
      LDY   #SYMBOL_FLAGS
      LDA   #SYM_LOCAL
      STA   (p_word),Y
      LDY   #SYMBOL_OFFSET_LO
      LDA   p_frame_size
      STA   (p_word),Y
      INY
      LDA   p_frame_size+1
      STA   (p_word),Y
      INY
      LDA   p_scope_level
      STA   (p_word),Y
      CLC
      LDA   p_frame_size
      ADC   p_type_extent
      STA   p_frame_size
      LDA   p_frame_size+1
      ADC   p_type_extent+1
      STA   p_frame_size+1
      BCS   @fail
      LDX   p_symbol_iter
      INX
      BRA   @symbol
@done:
      CLC
@fail:
      RTS

p_frame_parameter_info:
      STZ   p_param_flags
      STZ   p_argument_bytes
      LDX   p_scope_symbol_base
@parameter:
      CPX   p_scope_param_end
      BCS   @done
      JSR   p_symbol_slot_width
      BCS   @fail
      PHA
      ASL   p_param_flags
      PLA
      CMP   #2
      BNE   :+
      INC   p_param_flags
:
      CLC
      ADC   p_argument_bytes
      STA   p_argument_bytes
      LDX   p_symbol_iter
      INX
      BRA   @parameter
@done:
      CLC
@fail:
      RTS

p_emit_frame_enter:
      LDA   p_routine_param_count
      BNE   @required
      LDA   p_frame_size+1
      BNE   @required
      LDA   p_frame_size
      CMP   #2
      long_beq @done
@required:
      LDX   p_current_routine
      LDA   p_routine_kind_table,X
      ORA   #P_ROUTINE_HAS_FRAME
      STA   p_routine_kind_table,X
      LDA   p_routine_param_count
      CMP   #1
      BNE   @enter
      LDA   #<asm_save_incoming
      LDX   #>asm_save_incoming
      JSR   p_emit_ax_text
      long_bcs @fail
@enter:
      JSR   p_frame_parameter_info
      BCS   @fail
      JMP   p_emit_frame_enter_code
@done:
      CLC
@fail:
      RTS

p_emit_frame_enter_code:
      LDA   #<asm_frame_enter_call
      LDX   #>asm_frame_enter_call
      JSR   p_emit_ax_text
      long_bcs @fail
      JSR   p_frame_display_offset
      JSR   p_emit_hex_byte
      long_bcs @fail
      LDA   #<asm_frame_enter_size
      LDX   #>asm_frame_enter_size
      JSR   p_emit_ax_text
      long_bcs @fail
      LDA   p_frame_size+1
      JSR   p_emit_hex_byte
      long_bcs @fail
      LDA   p_frame_size
      JSR   p_emit_hex_byte
      long_bcs @fail
      LDA   #<asm_frame_enter_params
      LDX   #>asm_frame_enter_params
      JSR   p_emit_ax_text
      long_bcs @fail
      LDA   p_routine_param_count
      STA   p_digit
      LDX   p_function_active
      BEQ   @emit_parameter_count
      JSR   p_function_result_wide
      BCC   @load_parameter_count
      LDA   p_digit
      ORA   #$80
      BRA   @emit_parameter_count
@load_parameter_count:
      LDA   p_digit
@emit_parameter_count:
      JSR   p_emit_hex_byte
      long_bcs @fail
      LDA   #<asm_index_word_separator
      LDX   #>asm_index_word_separator
      JSR   p_emit_ax_text
      long_bcs @fail
      LDA   p_param_flags
      JSR   p_emit_hex_byte
      long_bcs @fail
      LDA   #$0A
      JSR   p_emit
      BCC   @done
@fail:
      SEC
@done:
      RTS

p_frame_display_offset:
      LDX   p_current_routine
      LDA   p_routine_kind_table,X
      BPL   @no_display
      LDA   p_scope_level
      ASL
      RTS
@no_display:
      LDA   #$FF
      RTS

; Wide function results occupy frame bytes two and three. Parameters were
; parsed first, so shift their recorded offsets before local declarations use
; the enlarged frame.
p_reserve_wide_function_result:
      LDX   p_scope_symbol_base
@parameter:
      CPX   p_symbol_count
      BCS   @frame
      STX   p_symbol_iter
      JSR   p_symbol_pointer
      LDY   #SYMBOL_OFFSET_LO
      CLC
      LDA   (p_word),Y
      ADC   #2
      STA   (p_word),Y
      INY
      LDA   (p_word),Y
      ADC   #0
      STA   (p_word),Y
      LDX   p_symbol_iter
      INX
      BRA   @parameter
@frame:
      CLC
      LDA   p_frame_size
      ADC   #2
      STA   p_frame_size
      LDA   p_frame_size+1
      ADC   #0
      STA   p_frame_size+1
      CLC
      RTS

; Normalize a wide function expression to its A/X return representation.
p_coerce_wide_function_result:
      LDA   p_function_return_type
      JSR   p_type_is_string
      BCC   @real
      LDA   p_expr_type
      JSR   p_type_is_string
      BCS   @ready
      LDA   p_expr_type
      JSR   p_type_is_char
      BCC   @fail
      LDA   #<asm_char_to_string
      LDX   #>asm_char_to_string
      JMP   p_emit_ax_text
@real:
      LDA   p_function_return_type
      JSR   p_type_is_real
      BCC   @ordinal
      LDA   p_expr_type
      JSR   p_type_is_real
      BCS   @ready
      LDA   p_expr_type
      CMP   #TYPE_WORD
      BEQ   :+
      LDA   #<asm_real_from_byte
      LDX   #>asm_real_from_byte
      JMP   p_emit_ax_text
:
      LDA   #<asm_real_from_word
      LDX   #>asm_real_from_word
      JMP   p_emit_ax_text
@ordinal:
      LDA   p_expr_type
      JSR   p_type_size
      BCS   @fail
      LDA   p_array_len+1
      BNE   @ready
      LDA   p_array_len
      CMP   #2
      BCS   @ready
      LDA   #<asm_index_high_zero
      LDX   #>asm_index_high_zero
      JMP   p_emit_ax_text
@ready:
      CLC
      RTS
@fail:
      SEC
      RTS

p_symbol_slot_width:
      STX   p_symbol_iter
      JSR   p_symbol_pointer
      LDY   #SYMBOL_FLAGS
      LDA   (p_word),Y
      AND   #SYM_BYREF
      BNE   @pointer
      LDY   #1
      LDA   (p_word),Y
      CMP   #TYPE_ARRAY_BYTE
      BEQ   @wide
      JSR   p_type_size
      BCS   @fail
      LDA   p_array_len+1
      BNE   @wide
      LDA   p_array_len
      CMP   #3
      BCS   @wide
      CLC
      RTS
@pointer:
@wide:
      LDA   #2
      CLC
      RTS
@fail:
      SEC
      RTS

p_emit_frame_store_index:
      STX   p_symbol_iter
      JSR   p_symbol_pointer
      LDY   #SYMBOL_OFFSET_LO
      LDA   (p_word),Y
      STA   p_symbol_offset
      INY
      LDA   (p_word),Y
      STA   p_symbol_offset+1
      INY
      LDA   (p_word),Y
      STA   p_symbol_depth
      LDA   p_digit
      CMP   #1
      BNE   @wide
      JMP   p_emit_frame_store8_symbol
@wide:
      JMP   p_emit_frame_store16_symbol
@fail:
      RTS

p_emit_frame_leave:
      LDA   p_routine_param_count
      BNE   @required
      LDA   p_frame_size+1
      BNE   @required
      LDA   p_frame_size
      CMP   #2
      BEQ   @done
@required:
      JSR   p_frame_parameter_info
      BCS   @fail
      LDA   p_routine_param_count
      CMP   #2
      BCS   :+
      STZ   p_argument_bytes
:
      LDA   p_argument_bytes
      LDX   p_function_active
      BEQ   :+
      ORA   #$80
      STA   p_digit
      JSR   p_function_result_wide
      BCC   @load_metadata
      LDA   p_digit
      ORA   #$40
      BRA   @metadata_ready
@load_metadata:
      LDA   p_digit
:
@metadata_ready:
      STA   p_digit
      LDA   #<asm_frame_leave_call
      LDX   #>asm_frame_leave_call
      JSR   p_emit_ax_text
      BCS   @fail
      JSR   p_frame_display_offset
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   #<asm_index_word_separator
      LDX   #>asm_index_word_separator
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   p_digit
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   #$0A
      JMP   p_emit
@done:
      CLC
@fail:
      RTS

p_emit_load_frame_value:
      LDA   p_expr_type
      CMP   #TYPE_ARRAY_BYTE
      BEQ   @structured
      JSR   p_type_size
      BCS   @fail
      LDA   p_array_len+1
      BNE   @structured
      LDA   p_array_len
      CMP   #3
      BCS   @structured
      STA   p_digit
      LDA   p_symbol_flags
      AND   #SYM_BYREF
      BNE   @reference_scalar
      LDA   p_digit
      CMP   #1
      BNE   @direct_word
      JMP   p_emit_frame_load8_symbol
@direct_word:
      JMP   p_emit_frame_load16_symbol
@reference_scalar:
      JSR   p_emit_frame_load16_symbol
      BCS   @fail
      LDA   p_digit
      CMP   #1
      BNE   @deref_word
      LDA   #<asm_deref8
      LDX   #>asm_deref8
      JMP   p_emit_ax_text
@deref_word:
      LDA   #<asm_deref16
      LDX   #>asm_deref16
      JMP   p_emit_ax_text
@structured:
      LDA   p_symbol_flags
      AND   #SYM_PARAM
      BNE   @pointer_slot
      JSR   p_emit_frame_address_symbol
      BCS   @fail
      LDA   #<asm_address_result
      LDX   #>asm_address_result
      JMP   p_emit_ax_text
@pointer_slot:
      JMP   p_emit_frame_load16_symbol
@fail:
      SEC
      RTS

; Return the address of a local designator in A/X. Explicit VAR parameters
; and structured value parameters already hold a pointer; ordinary locals
; and scalar value parameters live directly in their lexical frame slot.
p_emit_frame_designator_address:
      LDA   p_symbol_flags
      AND   #SYM_BYREF
      BNE   @pointer_slot
      LDA   p_symbol_flags
      AND   #SYM_PARAM
      BEQ   @frame_slot
      LDA   p_expr_type
      CMP   #TYPE_ARRAY_BYTE
      BEQ   @pointer_slot
      JSR   p_type_size
      BCS   @fail
      LDA   p_array_len+1
      BNE   @pointer_slot
      LDA   p_array_len
      CMP   #3
      BCS   @pointer_slot
@frame_slot:
      ; P_FADDR already returns the resolved address in A/X as well as NVR0.
      ; Designator-address consumers do not need to reload the same pointer.
      JMP   p_emit_frame_address_symbol
@pointer_slot:
      JMP   p_emit_frame_load16_symbol
@fail:
      SEC
      RTS

p_emit_store_frame_target:
      LDA   p_target_type
      CMP   #TYPE_ARRAY_BYTE
      BEQ   @fail
      JSR   p_type_size
      BCS   @fail
      LDA   p_array_len+1
      long_bne @fail
      LDA   p_array_len
      CMP   #3
      BCS   @fail
      STA   p_digit
      CMP   #1
      BEQ   @store
      LDA   p_expr_type
      JSR   p_type_size
      BCS   @fail
      LDA   p_array_len+1
      BNE   @store
      LDA   p_array_len
      CMP   #2
      BCS   @store
      LDA   #<asm_index_high_zero
      LDX   #>asm_index_high_zero
      JSR   p_emit_ax_text
      BCS   @fail
@store:
      LDA   p_target_flags
      AND   #SYM_BYREF
      BNE   @reference
      LDA   p_digit
      CMP   #1
      BNE   @direct_word
      JMP   p_emit_frame_store8_target
@direct_word:
      JMP   p_emit_frame_store16_target
@reference:
      LDA   p_digit
      CMP   #1
      BNE   @reference_word
      JMP   p_emit_frame_store_ref8_target
@reference_word:
      JMP   p_emit_frame_store_ref16_target
@fail:
      SEC
      RTS

p_emit_frame_address_symbol:
      LDA   #<asm_frame_addr_call
      LDX   #>asm_frame_addr_call
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   p_symbol_depth
      CMP   p_scope_level
      BEQ   @current
      ASL
      BRA   @display
@current:
      LDA   #$FF
@display:
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   #<asm_frame_addr_offset
      LDX   #>asm_frame_addr_offset
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   p_symbol_offset+1
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   p_symbol_offset
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   #$0A
      JMP   p_emit
@fail:
      RTS

p_emit_frame_address_target:
      LDA   p_target_depth
      STA   p_symbol_depth
      LDA   p_target_offset
      STA   p_symbol_offset
      LDA   p_target_offset+1
      STA   p_symbol_offset+1
      JMP   p_emit_frame_address_symbol

p_frame_direct8:
      LDA   p_symbol_depth
      CMP   p_scope_level
      BNE   @no
      LDA   p_symbol_offset+1
      BNE   @no
      CLC
      RTS
@no:
      SEC
      RTS

p_frame_direct16:
      JSR   p_frame_direct8
      BCS   @no
      LDA   p_symbol_offset
      CMP   #$FF
      BCS   @no
      CLC
      RTS
@no:
      SEC
      RTS

p_emit_frame_direct_y:
      LDA   #<asm_frame_direct_y
      LDX   #>asm_frame_direct_y
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   p_symbol_offset
      JSR   p_emit_hex_byte
@fail:
      RTS

p_emit_frame_load8_symbol:
      JSR   p_frame_direct8
      BCS   @indirect
      JSR   p_emit_frame_direct_y
      BCS   @fail
      LDA   #<asm_frame_direct_load8
      LDX   #>asm_frame_direct_load8
      JMP   p_emit_ax_text
@indirect:
      JSR   p_emit_frame_address_symbol
      BCS   @fail
      LDA   #<asm_frame_load8_tail
      LDX   #>asm_frame_load8_tail
      JMP   p_emit_ax_text
@fail:
      RTS

p_emit_frame_load16_symbol:
      JSR   p_frame_direct16
      BCS   @indirect
      JSR   p_emit_frame_direct_y
      BCS   @fail
      LDA   #<asm_frame_direct_load16
      LDX   #>asm_frame_direct_load16
      JMP   p_emit_ax_text
@indirect:
      JSR   p_emit_frame_address_symbol
      BCS   @fail
      LDA   #<asm_frame_load16_tail
      LDX   #>asm_frame_load16_tail
      JMP   p_emit_ax_text
@fail:
      RTS

p_emit_frame_store8_symbol:
      JSR   p_frame_direct8
      BCS   @indirect
      JSR   p_emit_frame_direct_y
      BCS   @fail
      LDA   #<asm_frame_direct_store8
      LDX   #>asm_frame_direct_store8
      JMP   p_emit_ax_text
@indirect:
      LDA   #<asm_frame_store8_save
      LDX   #>asm_frame_store8_save
      JSR   p_emit_ax_text
      BCS   @fail
      JSR   p_emit_frame_address_symbol
      BCS   @fail
      LDA   #<asm_frame_store8_tail
      LDX   #>asm_frame_store8_tail
      JMP   p_emit_ax_text
@fail:
      RTS

p_emit_frame_store16_symbol:
      JSR   p_frame_direct16
      BCS   @indirect
      JSR   p_emit_frame_direct_y
      BCS   @fail
      LDA   #<asm_frame_direct_store16
      LDX   #>asm_frame_direct_store16
      JMP   p_emit_ax_text
@indirect:
      LDA   #<asm_frame_store16_save
      LDX   #>asm_frame_store16_save
      JSR   p_emit_ax_text
      BCS   @fail
      JSR   p_emit_frame_address_symbol
      BCS   @fail
      LDA   #<asm_frame_store16_tail
      LDX   #>asm_frame_store16_tail
      JMP   p_emit_ax_text
@fail:
      RTS

p_emit_frame_store8_target:
      JSR   p_copy_target_frame
      JMP   p_emit_frame_store8_symbol

p_emit_frame_store16_target:
      JSR   p_copy_target_frame
      JMP   p_emit_frame_store16_symbol

p_emit_frame_store_ref8_target:
      LDA   #<asm_frame_store_ref8_save
      LDX   #>asm_frame_store_ref8_save
      JSR   p_emit_ax_text
      BCS   @fail
      JSR   p_copy_target_frame
      JSR   p_emit_frame_load16_symbol
      BCS   @fail
      LDA   #<asm_frame_store_ref8_tail
      LDX   #>asm_frame_store_ref8_tail
      JMP   p_emit_ax_text
@fail:
      RTS

p_emit_frame_store_ref16_target:
      LDA   #<asm_frame_store_ref16_save
      LDX   #>asm_frame_store_ref16_save
      JSR   p_emit_ax_text
      BCS   @fail
      JSR   p_copy_target_frame
      JSR   p_emit_frame_load16_symbol
      BCS   @fail
      LDA   #<asm_frame_store_ref16_tail
      LDX   #>asm_frame_store_ref16_tail
      JMP   p_emit_ax_text
@fail:
      RTS

p_copy_target_frame:
      LDA   p_target_depth
      STA   p_symbol_depth
      LDA   p_target_offset
      STA   p_symbol_offset
      LDA   p_target_offset+1
      STA   p_symbol_offset+1
      RTS

p_restore_call_identifier_to_saved:
      LDA   p_call_ident_len
      STA   p_saved_ident_len
      TAX
@copy:
      DEX
      BMI   @hash_start
      LDA   p_call_ident,X
      STA   p_saved_ident,X
      BRA   @copy
@hash_start:
      LDX   #2
@hash:
      LDA   p_call_hash,X
      STA   p_saved_hash,X
      DEX
      BPL   @hash
      RTS

p_emit_string_literal:
      JSR   p_new_label
      LDA   #<asm_rodata
      LDX   #>asm_rodata
      JSR   p_emit_ax_text
      long_bcs @fail
      JSR   p_emit_label
      long_bcs @fail
      LDA   #<asm_string_bytes
      LDX   #>asm_string_bytes
      JSR   p_emit_ax_text
      long_bcs @fail
      LDA   p_string_len
      JSR   p_emit_hex_byte
      long_bcs @fail
      LDA   #1
      STA   p_type_group_count
@char:
      JSR   p_next
      long_bcc @fail
      CMP   #$27
      BNE   @emit_char
      JSR   p_peek
      BCC   @done
      CMP   #$27
      BNE   @done
      JSR   p_next
      LDA   #$27
@emit_char:
      STA   p_char
      LDA   p_type_group_count
      CMP   #16
      BCC   @comma
      LDA   #<asm_const_cont
      LDX   #>asm_const_cont
      JSR   p_emit_ax_text
      BCS   @fail
      STZ   p_type_group_count
      BRA   @value
@comma:
      LDA   #','
      JSR   p_emit
      BCS   @fail
@value:
      LDA   #'$'
      JSR   p_emit
      BCS   @fail
      LDA   p_char
      JSR   p_emit_hex_byte
      BCS   @fail
      INC   p_type_group_count
      BRA   @char
@done:
      LDA   #$0A
      JSR   p_emit
      BCS   @fail
      LDA   #<asm_code
      LDX   #>asm_code
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   #<asm_load_string_lo
      LDX   #>asm_load_string_lo
      JSR   p_emit_ax_text
      BCS   @fail
      JSR   p_emit_label_id
      BCS   @fail
      LDA   #$0A
      JSR   p_emit
      BCS   @fail
      LDA   #<asm_load_string_hi
      LDX   #>asm_load_string_hi
      JSR   p_emit_ax_text
      BCS   @fail
      JSR   p_emit_label_id
      BCS   @fail
      LDA   #$0A
      JSR   p_emit
      BCS   @fail
      LDA   #TYPE_STRING_REF
      STA   p_expr_type
      CLC
      RTS
@fail:
      SEC
      RTS

      .segment "NPCFE_CODE"

; Declaration and named-type definitions share one layout parser.
p_parse_decl_type:
      JSR   p_parse_type_spec
      BCS   @fail
      LDA   p_decl_type
      CMP   #TYPE_ARRAY_BYTE
      BNE   @done
      LDA   p_type_work_kind
      CMP   #TYPE_KIND_STRING
      BCC   @done
      CMP   #TYPE_KIND_FILE+1
      BCS   @done
      LDA   p_type_decl_len
      STA   p_type_outer_decl_len
      STZ   p_type_decl_len
      JSR   p_type_add
      PHP
      LDA   p_type_outer_decl_len
      STA   p_type_decl_len
      PLP
      BCS   @fail
@done:
      CLC
@fail:
      RTS

; Add the current identifier to the transient symbol table. Names remain in
; source form for diagnostics/code generation; comparisons are already folded
; to uppercase by p_capture_identifier.
p_symbol_add:
      ; Pascal parameters and locals may shadow globals. Only reject a name
      ; already declared in the current routine scope.
      LDX   p_scope_symbol_base
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
      BEQ   @same
      PLY
      BRA   @next
@same:
      PLY
      INY
      BRA   @compare
@next:
      LDX   p_symbol_iter
      INX
      BRA   @duplicate
@store:
      CPX   #SYMBOL_CAP
      BCS   @fail
      LDA   p_ident_len
      CMP   #SYMBOL_NAME_CAP+1
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
      LDY   #SYMBOL_OFFSET_LO
      LDA   #0
      STA   (p_word),Y
      INY
      STA   (p_word),Y
      INY
      STA   (p_word),Y
      LDY   #SYMBOL_FLAGS
      STA   (p_word),Y
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
      STA   p_state_offset
      LDA   p_decimal+1
      ADC   p_remainder+1
      STA   p_state_offset+1
      ; The growth-sensitive symbol/name arena lives in one allocator-backed
      ; XRAM block. Map the requested record at Window 0 so every existing
      ; caller can keep using its ordinary indirect pointer.
p_state_pointer:
      CLC
      LDA   p_state_xaddr
      ADC   p_state_offset
      STA   p_word
      LDA   p_state_xaddr+1
      ADC   p_state_offset+1
      STA   XMC_W0AM
      LDA   p_state_xaddr+2
      ADC   #0
      STA   XMC_W0AH
      STZ   XMC_W0AL
      STZ   XMC_W1AL
      CLC
      LDA   XMC_W0AM
      ADC   #1
      STA   XMC_W1AM
      LDA   XMC_W0AH
      ADC   #0
      STA   XMC_W1AH
      LDA   XMC_WINCTL
      ORA   #XRAM_WIN0_ENABLE | XRAM_WIN1_ENABLE
      STA   XMC_WINCTL
      LDA   #>XRAM_WIN0_BASE
      STA   p_word+1
      RTS

; Finish all names in the current declaration group and emit their BSS
; definitions only after the type has been parsed successfully.
p_emit_symbol_group:
      LDX   p_symbol_group
@symbol:
      CPX   p_symbol_count
      BCS   @done
      STX   p_symbol_iter
      JSR   p_symbol_pointer
      LDY   #1
      LDA   p_decl_type
      STA   (p_word),Y
      LDX   p_symbol_iter
      JSR   p_emit_symbol_name
      BCS   @fail
@suffix:
      LDA   p_decl_type
      CMP   #TYPE_ARRAY_BYTE
      BEQ   @array
      JSR   p_type_size
      BCS   @fail
      LDA   p_array_len+1
      BNE   @array
      LDA   p_array_len
      CMP   #1
      BEQ   @byte
      CMP   #2
      BNE   @array
      LDA   #<asm_word_res
      LDX   #>asm_word_res
      JSR   p_emit_ax_text
      BCS   @fail
      BRA   @next
@byte:
      LDA   #<asm_byte_res
      STA   p_word
      LDA   #>asm_byte_res
      STA   p_word+1
      JSR   p_emit_text
      BCS   @fail
      BRA   @next
@array:
      LDA   #<asm_array_res
      LDX   #>asm_array_res
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   p_array_len+1
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   p_array_len
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   #$0A
      JSR   p_emit
      BCS   @fail
@next:
      LDX   p_symbol_iter
      INX
      BRA   @symbol
@done:
      CLC
@fail:
      RTS

; Find p_ident in the declared-symbol table. Carry is set, X is the symbol
; index, and A is its type. Unknown identifiers remain valid NDK symbols.
p_symbol_find:
      LDX   p_symbol_count
@symbol:
      DEX
      BMI   @missing
      STX   p_symbol_iter
      JSR   p_symbol_pointer
      LDY   #0
      LDA   (p_word),Y
      CMP   p_ident_len
      BNE   @next
      LDY   #0
@compare:
      CPY   p_ident_len
      BCS   @found
      LDA   p_ident,Y
      PHY
      INY
      INY
      CMP   (p_word),Y
      BEQ   @same
      PLY
      BRA   @next
@same:
      PLY
      INY
      BRA   @compare
@found:
      LDY   #1
      LDA   (p_word),Y
      STA   p_symbol_type
      LDY   #SYMBOL_FLAGS
      LDA   (p_word),Y
      STA   p_symbol_flags
      LDY   #SYMBOL_OFFSET_LO
      LDA   (p_word),Y
      STA   p_symbol_offset
      INY
      LDA   (p_word),Y
      STA   p_symbol_offset+1
      INY
      LDA   (p_word),Y
      STA   p_symbol_depth
      LDA   p_symbol_type
      LDX   p_symbol_iter
      SEC
      RTS
@next:
      LDX   p_symbol_iter
      BRA   @symbol
@missing:
      CLC
      RTS

p_save_target_storage:
      LDA   p_symbol_flags
      STA   p_target_flags
      LDA   p_symbol_offset
      STA   p_target_offset
      LDA   p_symbol_offset+1
      STA   p_target_offset+1
      LDA   p_symbol_depth
      STA   p_target_depth
      LDA   p_target_type
      RTS

; Emit the name of symbol X.
p_emit_symbol_name:
      STX   p_symbol_iter
      JSR   p_symbol_pointer
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
@char:
      LDA   p_digit
      BEQ   @done
      LDY   #0
      LDA   (p_word),Y
      JSR   p_emit
      BCS   @fail
      INC   p_word
      BNE   :+
      INC   p_word+1
:     DEC   p_digit
      BRA   @char
@done:
      LDX   p_symbol_iter
      CLC
      RTS
@fail:
      LDX   p_symbol_iter
      SEC
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
      LDA   p_operand_types,X
      STA   p_left_type
      RTS

p_emit_binary_prep:
      LDX   p_operator_depth
      BEQ   @fail
      DEX
      LDA   p_expr_type
      STA   p_operand_types,X
      JSR   p_type_is_real
      BCC   @sized
      LDA   #<asm_push_real_value
      LDX   #>asm_push_real_value
      JMP   p_emit_ax_text
@sized:
      LDA   p_expr_type
      JSR   p_type_size
      BCS   @fail
      LDA   p_array_len+1
      BNE   @wide
      LDA   p_array_len
      CMP   #1
      BEQ   @byte
@wide:
      LDA   #<asm_phx_pha
      LDX   #>asm_phx_pha
      JMP   p_emit_ax_text
@byte:
      LDA   #<asm_pha
      LDX   #>asm_pha
      JMP   p_emit_ax_text
@fail:
      RTS

; p_operator selects =, <>, <, <=, >, >= as 1..6.
p_emit_compare:
      LDA   p_left_type
      JSR   p_type_is_string
      BCC   @right_string
      LDA   p_expr_type
      JSR   p_type_is_string
      BCS   @strings_ready
      LDA   p_expr_type
      JSR   p_type_is_char
      long_bcc @fail
      LDA   #<asm_char_to_string
      LDX   #>asm_char_to_string
      JSR   p_emit_ax_text
      long_bcs @fail
@strings_ready:
      LDA   #<asm_string_compare
      LDX   #>asm_string_compare
      JSR   p_emit_ax_text
      BCS   @fail
      BRA   @suffix
@right_string:
      LDA   p_expr_type
      JSR   p_type_is_string
      BCC   @ordinal
      LDA   p_left_type
      JSR   p_type_is_char
      BCC   @fail
      LDA   #<asm_char_string_compare
      LDX   #>asm_char_string_compare
      JSR   p_emit_ax_text
      BCS   @fail
      BRA   @suffix
@ordinal:
      LDA   p_left_type
      JSR   p_type_is_real
      BCC   @right_real
      LDA   p_expr_type
      JSR   p_type_is_real
      BCC   @fail
      LDA   #<asm_real_compare
      LDX   #>asm_real_compare
      JSR   p_emit_ax_text
      BCS   @fail
      BRA   @suffix
@right_real:
      LDA   p_expr_type
      JSR   p_type_is_real
      BCS   @fail
      LDA   p_left_type
      CMP   #TYPE_WORD
      BEQ   @wide
      LDA   p_expr_type
      CMP   #TYPE_WORD
      BEQ   @wide
      LDA   #<asm_compare_start
      LDX   #>asm_compare_start
      JSR   p_emit_ax_text
      BCS   @fail
      BRA   @suffix
@wide:
      JSR   p_emit_wide_operands
      BCS   @fail
      LDA   #<asm_compare_word_start
      LDX   #>asm_compare_word_start
      JSR   p_emit_ax_text
      BCS   @fail
@suffix:
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

; Normalize a binary operation's right value into __NP_RHS and restore its
; left value into A/X. Byte operands are zero-extended only on wide paths.
p_emit_wide_operands:
      LDA   p_expr_type
      CMP   #TYPE_WORD
      BNE   @right_byte
      LDA   #<asm_rhs_word
      LDX   #>asm_rhs_word
      BRA   @right
@right_byte:
      LDA   #<asm_rhs_byte
      LDX   #>asm_rhs_byte
@right:
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   p_left_type
      CMP   #TYPE_WORD
      BNE   @left_byte
      LDA   #<asm_lhs_word
      LDX   #>asm_lhs_word
      JMP   p_emit_ax_text
@left_byte:
      LDA   #<asm_lhs_byte
      LDX   #>asm_lhs_byte
      JMP   p_emit_ax_text
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
      LDA   #<p_current_name
      STA   p_word
      LDA   #>p_current_name
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
      PHX
      LDX   p_output_used
      STA   p_output_buffer,X
      INC   p_output_used
      BNE   @count
      INC   p_output_used+1
@count:
      INC   generated_asm_len
      BNE   @flush
      INC   generated_asm_len+1
      BNE   @flush
      INC   generated_asm_len+2
@flush:
      LDA   p_output_used+1
      BEQ   @done
      JSR   p_flush_output
      BNE   @full
@done:
      CLC
      PLX
      RTS
@full:
      SEC
      PLX
      RTS

; Non-consuming keyword lookahead. A/X points to an uppercase Z string.
p_peek_keyword:
      STA   p_word
      STX   p_word+1
      ; Whitespace is semantically consumed either way. Mark at the token so
      ; bounded identifier lookahead never retains arbitrarily many source
      ; pages merely because a large comment precedes it.
      JSR   p_skip_ws
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

      .segment "CODE"
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
      LDX   #2
@stream:
      LDA   p_stream_base,X
      STA   p_mark_stream_base,X
      LDA   p_stream_next,X
      STA   p_mark_stream_next,X
      DEX
      BPL   @stream
      LDA   p_stream_more
      STA   p_mark_stream_more
      LDA   p_stream_slot
      STA   p_mark_stream_slot
      LDA   p_line
      STA   p_mark_line
      LDA   p_line+1
      STA   p_mark_line+1
      LDA   p_column
      STA   p_mark_column
      LDA   p_column+1
      STA   p_mark_column+1
      LDA   p_include_depth
      STA   p_mark_include_depth
      LDA   p_current_overlay
      STA   p_mark_overlay
      LDA   p_current_name_len
      STA   p_mark_name_len
      LDX   #P_SOURCE_NAME_CAP-1
@name:
      LDA   p_current_name,X
      STA   p_mark_name,X
      DEX
      BPL   @name
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
      LDX   #2
@stream:
      LDA   p_mark_stream_base,X
      STA   p_stream_base,X
      LDA   p_mark_stream_next,X
      STA   p_stream_next,X
      DEX
      BPL   @stream
      LDA   p_mark_stream_more
      STA   p_stream_more
      LDA   p_mark_stream_slot
      STA   p_stream_slot
      LDA   p_mark_line
      STA   p_line
      LDA   p_mark_line+1
      STA   p_line+1
      LDA   p_mark_column
      STA   p_column
      LDA   p_mark_column+1
      STA   p_column+1
      LDA   p_mark_include_depth
      STA   p_include_depth
      LDA   p_mark_overlay
      STA   p_current_overlay
      LDA   p_mark_name_len
      STA   p_current_name_len
      LDX   #P_SOURCE_NAME_CAP-1
@name:
      LDA   p_mark_name,X
      STA   p_current_name,X
      DEX
      BPL   @name
      STZ   p_window_valid
      RTS

      .segment "NPCFE_CODE"
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

; Function expressions may nest, so preserve each callee while its argument
; expression is parsed. Eight levels matches the parser's routine-depth limit.
p_push_call_identifier:
      LDX   p_in_argument
      CPX   #P_CALL_DEPTH_CAP
      BCS   @fail
      LDA   p_call_ident_len
      STA   p_call_stack_len,X
      LDA   p_call_hash
      STA   p_call_stack_hash0,X
      LDA   p_call_hash+1
      STA   p_call_stack_hash1,X
      LDA   p_call_hash+2
      STA   p_call_stack_hash2,X
      JSR   p_call_stack_name_pointer
      LDY   #0
@name:
      CPY   p_call_ident_len
      BCS   @done
      LDA   p_call_ident,Y
      STA   (p_word),Y
      INY
      BRA   @name
@done:
      INC   p_in_argument
      CLC
@fail:
      RTS

p_restore_call_identifier:
      LDX   p_in_argument
      BEQ   @fail
      DEX
      LDA   p_call_stack_len,X
      STA   p_call_ident_len
      LDA   p_call_stack_hash0,X
      STA   p_call_hash
      LDA   p_call_stack_hash1,X
      STA   p_call_hash+1
      LDA   p_call_stack_hash2,X
      STA   p_call_hash+2
      JSR   p_call_stack_name_pointer
      LDY   #0
@name:
      CPY   p_call_ident_len
      BCS   @done
      LDA   (p_word),Y
      STA   p_call_ident,Y
      INY
      BRA   @name
@done:
      CLC
@fail:
      RTS

p_pop_call_identifier:
      LDA   p_in_argument
      BEQ   @fail
      DEC   p_in_argument
      CLC
@fail:
      RTS

; X selects one of eight fixed 32-byte name slots.
p_call_stack_name_pointer:
      TXA
      STA   p_remainder
      STZ   p_remainder+1
      LDY   #5
@times32:
      ASL   p_remainder
      ROL   p_remainder+1
      DEY
      BNE   @times32
      CLC
      LDA   p_remainder
      ADC   #<p_call_stack_names
      STA   p_word
      LDA   p_remainder+1
      ADC   #>p_call_stack_names
      STA   p_word+1
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

p_emit_call_identifier:
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

p_emit_global_saved:
      LDA   #<asm_global_start
      LDX   #>asm_global_start
      JSR   p_emit_ax_text
      BCS   @fail
      JSR   p_emit_saved_identifier
      BCS   @fail
      LDA   #<asm_unit_export_end
      LDX   #>asm_unit_export_end
      JMP   p_emit_ax_text
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
      LDA   p_ident_len
      long_beq @bad
      CMP   #4
      BCC   @user
      LDA   p_ident+0
      CMP   #'N'
      BNE   @user
      LDA   p_ident+1
      CMP   #'O'
      BNE   @user
      LDA   p_ident+2
      CMP   #'V'
      BNE   @user
      LDA   p_ident+3
      CMP   #'A'
      BNE   @user
      LDA   p_ident_len
      CMP   #5
      BCC   @bad
      BRA   @platform
@user:
      LDA   p_ident_len
      CMP   #P_UNIT_NAME_CAP
      BCS   @bad
      JSR   p_unit_is_local
      BCS   @ok
      LDY   #0
      BRA   @store
@platform:
      LDY   #4
@store:
      LDX   p_unit_count
      CPX   #P_UNIT_CAP
      BCS   @bad
      LDA   p_ident_len
      SEC
      STY   p_expected
      SBC   p_expected
      CMP   #P_UNIT_NAME_CAP+1
      BCS   @bad
      STA   p_unit_len,X
      TYA
      BEQ   :+
      LDA   #1
:     STA   p_unit_kind,X
      TXA
      ASL
      ASL
      ASL
      ASL
      TAX
      LDY   p_expected
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
      LDA   p_unit_kind,X
      BNE   @platform_lines
      LDA   #3
      JSR   p_emit_unit_line
      BCS   @emit_bad
      DEC   p_unit_count
      BRA   @ok
@platform_lines:
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

; Carry set when the current USES identifier names a UNIT entry compiled as
; part of this project. All other user units are resolved through UNITPATH.
p_unit_is_local:
      STZ   p_unit_iter
@unit:
      LDA   p_unit_iter
      CMP   NPP_PLAN_BASE+NPP_PLAN_LOCAL_COUNT
      BCS   @external
      ASL
      CLC
      ADC   p_unit_iter
      TAX
      LDA   p_ident_hash+0
      CMP   NPP_PLAN_BASE+NPP_PLAN_LOCAL_HASHES+0,X
      BNE   @next
      LDA   p_ident_hash+1
      CMP   NPP_PLAN_BASE+NPP_PLAN_LOCAL_HASHES+1,X
      BNE   @next
      LDA   p_ident_hash+2
      CMP   NPP_PLAN_BASE+NPP_PLAN_LOCAL_HASHES+2,X
      BNE   @next
      SEC
      RTS
@next:
      INC   p_unit_iter
      BRA   @unit
@external:
      CLC
      RTS

; X selects a stored unit. A=0 declarations, A=1 source, A=2 Pascal ABI,
; A=3 reusable Pascal typed assembly.
p_emit_unit_line:
      STA   p_char
      STX   p_unit_iter
      LDA   #<asm_include_start
      LDX   #>asm_include_start
      LDY   p_char
      CPY   #3
      BNE   :+
      LDA   #<asm_include_text_start
      LDX   #>asm_include_text_start
:
      JSR   p_emit_ax_text
      BCS   @fail
      LDX   p_unit_iter
      TXA
      ASL
      ASL
      ASL
      ASL
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
      CMP   #3
      BEQ   @typed_source
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
      BRA   @emit_suffix
@typed_source:
      LDA   #<asm_typed_source_end
      STA   p_word
      LDA   #>asm_typed_source_end
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

; Nested routines get an assembler-only identity from their declaration row.
; Source names remain untouched for diagnostics and top-level/unit linkage.
p_emit_nested_routine_name:
      STX   p_routine_iter
      LDA   #<asm_nested_routine_prefix
      LDX   #>asm_nested_routine_prefix
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   p_routine_iter
      JMP   p_emit_hex_byte
@fail:
      RTS

p_emit_local_call_saved:
      LDA   #<asm_jsr_prefix
      LDX   #>asm_jsr_prefix
      JSR   p_emit_ax_text
      BCS   @fail
      LDX   p_routine_iter
      LDA   p_routine_owner,X
      CMP   #$FF
      BEQ   @source_name
      JSR   p_emit_nested_routine_name
      BCS   @fail
      BRA   @newline
@source_name:
      JSR   p_emit_saved_identifier
      BCS   @fail
@newline:
      LDA   #$0A
      JMP   p_emit
@fail:
      SEC
      RTS

p_emit_local_call_buffered:
      LDA   #<asm_jsr_prefix
      LDX   #>asm_jsr_prefix
      JSR   p_emit_ax_text
      BCS   @fail
      LDX   p_routine_iter
      LDA   p_routine_owner,X
      CMP   #$FF
      BEQ   @source_name
      JSR   p_emit_nested_routine_name
      BCS   @fail
      BRA   @newline
@source_name:
      JSR   p_emit_call_identifier_name
      BCS   @fail
@newline:
      LDA   #$0A
      JMP   p_emit
@fail:
      SEC
      RTS

p_routine_find_saved:
      LDA   p_saved_ident_len
      STA   p_call_ident_len
      LDA   p_saved_hash
      STA   p_call_hash
      LDA   p_saved_hash+1
      STA   p_call_hash+1
      LDA   p_saved_hash+2
      STA   p_call_hash+2
      JMP   p_routine_find_call

; Define the same signature marker used by precompiled unit interfaces, so
; local and external calls share one validation path. A is $00 for a procedure
; or $02 for a no-argument Byte function.
p_emit_local_signature:
      STA   p_argument_count
      LDA   #<asm_local_sig_start
      LDX   #>asm_local_sig_start
      JSR   p_emit_ax_text
      BCS   @fail
      LDX   #<p_saved_hash
      LDY   #>p_saved_hash
      JSR   p_emit_hash
      BCS   @fail
      LDA   #<asm_local_sig_end
      LDX   #>asm_local_sig_end
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   p_argument_count
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   #$0A
      JMP   p_emit
@fail:
      SEC
      RTS

p_emit_local_pascal_signature:
      LDA   #<asm_local_pascal_sig_start
      LDX   #>asm_local_pascal_sig_start
      JSR   p_emit_ax_text
      BCS   @fail
      LDX   #<p_saved_hash
      LDY   #>p_saved_hash
      JSR   p_emit_hash
      BCS   @fail
      LDA   #<asm_local_sig_end
      LDX   #>asm_local_sig_end
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   p_routine_param_count
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   #$0A
      JMP   p_emit
@fail:
      SEC
      RTS

p_emit_saved_label:
      LDA   #<asm_routine_start
      LDX   #>asm_routine_start
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   p_function_active
      BEQ   :+
      LDA   #'F'
      BRA   :++
:
      LDA   #'P'
:
      JSR   p_emit
      BCS   @fail
      LDA   #' '
      JSR   p_emit
      BCS   @fail
      LDX   p_current_routine
      STX   p_routine_iter
      LDA   p_routine_owner,X
      CMP   #$FF
      BEQ   :+
      JSR   p_emit_nested_routine_name
      BRA   :++
:
      JSR   p_emit_saved_identifier
:
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

; Multi-byte Pascal unit procedures use caller-pushed byte arguments. The
; compiled unit interface defines __P<hash> as its exact arity.
p_emit_pascal_sig_saved:
      LDA   #<asm_pascal_sig_start
      LDX   #>asm_pascal_sig_start
      JSR   p_emit_ax_text
      BCS   @fail
      LDX   #<p_saved_hash
      LDY   #>p_saved_hash
      JSR   p_emit_hash
      BCS   @fail
      LDA   #<asm_pascal_sig_end
      LDX   #>asm_pascal_sig_end
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   p_argument_count
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   #$0A
      JMP   p_emit
@fail:
      SEC
      RTS

p_emit_drop_arguments:
      LDX   p_argument_bytes
@argument:
      CPX   #0
      BEQ   @done
      PHX
      LDA   #<asm_pla
      LDX   #>asm_pla
      JSR   p_emit_ax_text
      PLX
      BCS   @fail
      DEX
      BRA   @argument
@done:
      CLC
@fail:
      RTS

p_emit_push_argument:
      LDA   p_expr_type
      JSR   p_type_size
      BCS   @fail
      LDA   p_array_len+1
      BNE   @wide
      LDA   p_array_len
      CMP   #1
      BNE   @wide
      LDA   #<asm_pha
      LDX   #>asm_pha
      JSR   p_emit_ax_text
      BCS   @fail
      INC   p_argument_bytes
      CLC
      RTS
@wide:
      LDA   #<asm_phx_pha
      LDX   #>asm_phx_pha
      JSR   p_emit_ax_text
      BCS   @fail
      INC   p_argument_bytes
      INC   p_argument_bytes
      CLC
@fail:
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
      LDA   #<asm_load_byte_ir
      STA   p_word
      LDA   #>asm_load_byte_ir
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

; Imported .NPI metadata marks byte constants with __C<hash>. Let the
; assembler choose immediate versus storage after it has loaded the metadata.
p_emit_external_value_identifier:
      LDA   p_external_macro_emitted
      BNE   @load
      INC   p_external_macro_emitted
      LDA   #<asm_external_macro
      LDX   #>asm_external_macro
      JSR   p_emit_ax_text
      BCS   @fail
@load:
      LDA   #<asm_external_const_start
      LDX   #>asm_external_const_start
      JSR   p_emit_ax_text
      BCS   @fail
      JSR   p_emit_call_identifier
      BCS   @fail
      LDA   #<asm_external_const_else
      LDX   #>asm_external_const_else
      JSR   p_emit_ax_text
      BCS   @fail
      LDX   #<p_call_hash
      LDY   #>p_call_hash
      JSR   p_emit_hash
      BCS   @fail
      LDA   #$0A
      JMP   p_emit
@fail:
      SEC
      RTS

p_emit_load_word_identifier:
      LDA   #<asm_load_word_ir
      LDX   #>asm_load_word_ir
      JSR   p_emit_ax_text
      BCS   @fail
      JSR   p_emit_identifier
      BCS   @fail
      LDA   #$0A
      JMP   p_emit
@fail:
      SEC
      RTS

p_emit_address_identifier:
      LDA   #<asm_global_addr_low
      LDX   #>asm_global_addr_low
      JSR   p_emit_ax_text
      BCS   @fail
      JSR   p_emit_identifier
      BCS   @fail
      LDA   #$0A
      JSR   p_emit
      BCS   @fail
      LDA   #<asm_global_addr_high
      LDX   #>asm_global_addr_high
      JSR   p_emit_ax_text
      BCS   @fail
      JSR   p_emit_identifier
      BCS   @fail
      LDA   #$0A
      JMP   p_emit
@fail:
      SEC
      RTS

; Emit p_decimal as A=low, X=high.
p_emit_load_word:
      LDA   #<asm_lda
      LDX   #>asm_lda
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   p_decimal
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   #<asm_word_literal_high
      LDX   #>asm_word_literal_high
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   p_decimal+1
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   #$0A
      JMP   p_emit
@fail:
      SEC
      RTS

p_is_function_result:
      LDA   p_function_active
      BEQ   @no
      LDX   #2
@byte:
      LDA   p_saved_hash,X
      CMP   p_function_hash,X
      BNE   @no
      DEX
      BPL   @byte
      SEC
      RTS
@no:
      CLC
      RTS

; Function expressions return either one byte in A or a two-byte value in
; A/X. Real and structured values use A/X as their value address.
p_function_result_wide:
      LDA   p_function_return_type
      JSR   p_type_size
      BCS   @wide
      LDA   p_array_len+1
      BNE   @wide
      LDA   p_array_len
      CMP   #2
      RTS
@wide:
      SEC
      RTS

p_emit_store_saved:
      JSR   p_is_function_result
      long_bcc @regular_store
      JSR   p_function_result_wide
      BCC   @byte_function_store
      JSR   p_coerce_wide_function_result
      long_bcs @fail
@wide_function_store:
      LDA   #<asm_function_store_wide
      LDX   #>asm_function_store_wide
      JMP   p_emit_ax_text
@byte_function_store:
      LDA   #<asm_function_store
      LDX   #>asm_function_store
      JMP   p_emit_ax_text
@regular_store:
      LDA   p_target_with
      long_bne p_emit_store_with_target
      LDA   p_target_type
      JSR   p_type_is_string
      long_bcs p_emit_store_string_target
      LDA   p_target_type
      JSR   p_type_is_real
      long_bcs p_emit_store_real_target
      LDA   p_target_type
      JSR   p_type_size
      long_bcs @fail
      LDA   p_array_len+1
      long_bne p_emit_store_block_target
      LDA   p_array_len
      CMP   #3
      long_bcs p_emit_store_block_target
      LDA   p_target_flags
      long_bne p_emit_store_frame_target
      LDA   p_target_type
      CMP   #TYPE_WORD
      BNE   @direct_byte
      LDA   p_expr_type
      CMP   #TYPE_WORD
      BNE   @word_from_byte
      LDA   #<asm_store_word_ir
      LDX   #>asm_store_word_ir
      BRA   @direct_typed
@word_from_byte:
      LDA   #<asm_store_extend_ir
      LDX   #>asm_store_extend_ir
      BRA   @direct_typed
@direct_byte:
      LDA   #<asm_store_byte_ir
      LDX   #>asm_store_byte_ir
@direct_typed:
      JSR   p_emit_ax_text
      BCS   @fail
      JSR   p_emit_saved_identifier
      BCS   @fail
      LDA   #$0A
      JMP   p_emit
@fail:
      SEC
      RTS

; Copy an aggregate expression into a global, local, or parameter target.
; A/X is the source address; I_P_BLOCK_COPY consumes a 16-bit inline size.
p_emit_store_block_target:
      LDA   #<asm_string_copy_save
      LDX   #>asm_string_copy_save
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   p_target_flags
      BEQ   @global
      JSR   p_emit_frame_address_target
      BCS   @fail
      LDA   p_target_flags
      AND   #SYM_PARAM
      BEQ   @destination_ready
      LDA   #<asm_frame_load16_tail
      LDX   #>asm_frame_load16_tail
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   #<asm_address_to_nvr0
      LDX   #>asm_address_to_nvr0
      JSR   p_emit_ax_text
      BCS   @fail
      BRA   @destination_ready
@global:
      JSR   p_emit_address_saved_identifier
      BCS   @fail
      LDA   #<asm_address_to_nvr0
      LDX   #>asm_address_to_nvr0
      JSR   p_emit_ax_text
      BCS   @fail
@destination_ready:
      LDA   #<asm_load_saved_nvr2
      LDX   #>asm_load_saved_nvr2
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   #<asm_block_copy_call
      LDX   #>asm_block_copy_call
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   p_array_len+1
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   p_array_len
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   #$0A
      JMP   p_emit
@fail:
      SEC
      RTS

; Store through a destination address saved beneath the just-parsed RHS.
p_emit_store_designator:
      LDA   p_target_type
      JSR   p_type_is_string
      long_bcs @string
      LDA   p_target_type
      JSR   p_type_is_real
      long_bcs @real
      LDA   p_target_type
      JSR   p_type_size
      long_bcs @fail
      LDA   p_array_len+1
      long_bne @fail
      LDA   p_array_len
      CMP   #1
      long_beq @byte
      CMP   #2
      long_bne @fail
      LDA   #<asm_stack_address_store_word
      LDX   #>asm_stack_address_store_word
      JMP   p_emit_ax_text
@byte:
      LDA   #<asm_stack_address_store_byte
      LDX   #>asm_stack_address_store_byte
      JMP   p_emit_ax_text
@string:
      LDA   p_expr_type
      JSR   p_type_is_string
      BCC   @fail
      LDA   #<asm_stack_address_string_save
      LDX   #>asm_stack_address_string_save
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   #<asm_string_copy_call
      LDX   #>asm_string_copy_call
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   p_target_type
      JSR   p_type_size
      BCS   @fail
      LDA   p_array_len+1
      BNE   @fail
      LDA   p_array_len
      BEQ   @fail
      DEC
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   #$0A
      JMP   p_emit
@real:
      STZ   p_digit
      LDA   p_expr_type
      JSR   p_type_is_real
      BCC   @real_integer
      INC   p_digit
      LDA   #<asm_string_copy_save
      LDX   #>asm_string_copy_save
      BRA   @real_save
@real_integer:
      LDA   p_expr_type
      CMP   #TYPE_WORD
      BEQ   @real_word
      LDA   #<asm_real_save_byte
      LDX   #>asm_real_save_byte
      BRA   @real_save
@real_word:
      LDA   #<asm_real_save_word
      LDX   #>asm_real_save_word
@real_save:
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   #<asm_stack_address_to_nvr0
      LDX   #>asm_stack_address_to_nvr0
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   p_digit
      BEQ   @real_store_integer
      LDA   #<asm_real_copy
      LDX   #>asm_real_copy
      JMP   p_emit_ax_text
@real_store_integer:
      LDA   #<asm_real_store_integer
      LDX   #>asm_real_store_integer
      JMP   p_emit_ax_text
@fail:
      SEC
      RTS

; Carry set when A is a literal/reference string or a named short-string type.
p_type_is_string:
      CMP   #TYPE_STRING_REF
      BEQ   @yes
      CMP   #P_TYPE_USER_BASE
      BCC   @no
      SEC
      SBC   #P_TYPE_USER_BASE
      TAX
      CPX   p_type_count
      BCS   @no
      LDA   p_type_kind,X
      CMP   #TYPE_KIND_STRING
      BNE   @no
@yes:
      SEC
      RTS
@no:
      CLC
      RTS

p_type_is_char:
      CMP   #TYPE_CHAR
      BEQ   @yes
      CMP   #P_TYPE_USER_BASE
      BCC   @no
      SEC
      SBC   #P_TYPE_USER_BASE
      TAX
      CPX   p_type_count
      BCS   @no
      LDA   p_type_kind,X
      CMP   #TYPE_KIND_ORDINAL
      BNE   @no
      LDA   p_type_element,X
      CMP   #TYPE_CHAR
      BNE   @no
@yes:
      SEC
      RTS
@no:
      CLC
      RTS

p_type_is_real:
      CMP   #TYPE_REAL
      BEQ   @yes
      CMP   #P_TYPE_USER_BASE
      BCC   @no
      SEC
      SBC   #P_TYPE_USER_BASE
      TAX
      CPX   p_type_count
      BCS   @no
      LDA   p_type_kind,X
      CMP   #TYPE_KIND_REAL
      BNE   @no
@yes:
      SEC
      RTS
@no:
      CLC
      RTS

p_emit_store_string_target:
      LDA   p_expr_type
      JSR   p_type_is_string
      BCS   @source_ready
      LDA   p_expr_type
      JSR   p_type_is_char
      BCC   @fail
      LDA   #<asm_char_to_string
      LDX   #>asm_char_to_string
      JSR   p_emit_ax_text
      BCS   @fail
@source_ready:
      LDA   #<asm_string_copy_save
      LDX   #>asm_string_copy_save
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   p_target_flags
      BEQ   @global
      JSR   p_emit_frame_address_target
      BCS   @fail
      LDA   p_target_flags
      AND   #SYM_PARAM
      BEQ   @copy
      LDA   #<asm_frame_pointer_to_nvr0
      LDX   #>asm_frame_pointer_to_nvr0
      JSR   p_emit_ax_text
      BCS   @fail
      BRA   @copy
@global:
      JSR   p_emit_address_saved_identifier
      BCS   @fail
      LDA   #<asm_address_to_nvr0
      LDX   #>asm_address_to_nvr0
      JSR   p_emit_ax_text
      BCS   @fail
@copy:
      LDA   #<asm_string_copy_call
      LDX   #>asm_string_copy_call
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   p_target_type
      JSR   p_type_size
      BCS   @fail
      SEC
      LDA   p_array_len
      SBC   #1
      STA   p_decimal
      LDA   p_array_len+1
      SBC   #0
      BNE   @fail
      LDA   p_decimal
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   #$0A
      JMP   p_emit
@fail:
      SEC
      RTS

p_emit_store_real_target:
      STZ   p_digit
      LDA   p_expr_type
      JSR   p_type_is_real
      BCC   @integer
      INC   p_digit
      LDA   #<asm_string_copy_save
      LDX   #>asm_string_copy_save
      BRA   @save
@integer:
      LDA   p_expr_type
      CMP   #TYPE_WORD
      BEQ   @word
      LDA   #<asm_real_save_byte
      LDX   #>asm_real_save_byte
      BRA   @save
@word:
      LDA   #<asm_real_save_word
      LDX   #>asm_real_save_word
@save:
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   p_target_flags
      BEQ   @global
      JSR   p_emit_frame_address_target
      BCS   @fail
      LDA   p_target_flags
      AND   #SYM_PARAM
      BEQ   @store
      LDA   #<asm_frame_pointer_to_nvr0
      LDX   #>asm_frame_pointer_to_nvr0
      JSR   p_emit_ax_text
      BCS   @fail
      BRA   @store
@global:
      JSR   p_emit_address_saved_identifier
      BCS   @fail
      LDA   #<asm_address_to_nvr0
      LDX   #>asm_address_to_nvr0
      JSR   p_emit_ax_text
      BCS   @fail
@store:
      LDA   p_digit
      BEQ   @store_integer
      LDA   #<asm_real_copy
      LDX   #>asm_real_copy
      JMP   p_emit_ax_text
@store_integer:
      LDA   #<asm_real_store_integer
      LDX   #>asm_real_store_integer
      JMP   p_emit_ax_text
@fail:
      SEC
      RTS

p_emit_address_saved_identifier:
      LDA   #<asm_global_addr_low
      LDX   #>asm_global_addr_low
      JSR   p_emit_ax_text
      BCS   @fail
      JSR   p_emit_saved_identifier
      BCS   @fail
      LDA   #$0A
      JSR   p_emit
      BCS   @fail
      LDA   #<asm_global_addr_high
      LDX   #>asm_global_addr_high
      JSR   p_emit_ax_text
      BCS   @fail
      JSR   p_emit_saved_identifier
      BCS   @fail
      LDA   #$0A
      JMP   p_emit
@fail:
      SEC
      RTS

p_emit_store_with_target:
      LDA   p_target_with_slot
      STA   p_with_slot
      LDX   p_target_symbol
      STX   p_field_iter
      LDA   p_field_size_h,X
      BNE   @structured
      LDA   p_field_size_l,X
      CMP   #1
      BEQ   @byte
      CMP   #2
      BNE   @structured
      LDA   #<asm_frame_store16_save
      LDX   #>asm_frame_store16_save
      JSR   p_emit_ax_text
      BCS   @fail
      JSR   p_emit_with_field_address
      BCS   @fail
      LDA   #<asm_frame_store16_tail
      LDX   #>asm_frame_store16_tail
      JMP   p_emit_ax_text
@byte:
      LDA   #<asm_frame_store8_save
      LDX   #>asm_frame_store8_save
      JSR   p_emit_ax_text
      BCS   @fail
      JSR   p_emit_with_field_address
      BCS   @fail
      LDA   #<asm_frame_store8_tail
      LDX   #>asm_frame_store8_tail
      JMP   p_emit_ax_text
@structured:
      LDA   p_target_type
      CMP   #TYPE_ARRAY_BYTE
      BEQ   @string
      CMP   #P_TYPE_USER_BASE
      BCC   @real
      SEC
      SBC   #P_TYPE_USER_BASE
      TAX
      LDA   p_type_kind,X
      CMP   #TYPE_KIND_STRING
      BNE   @real
@string:
      LDA   p_expr_type
      CMP   #TYPE_STRING_REF
      BEQ   p_emit_store_with_string
      CMP   #P_TYPE_USER_BASE
      BCC   @fail
      SEC
      SBC   #P_TYPE_USER_BASE
      TAX
      LDA   p_type_kind,X
      CMP   #TYPE_KIND_STRING
      BNE   @fail
      JMP   p_emit_store_with_string
@real:
      LDA   p_target_type
      JSR   p_type_is_real
      BCC   @fail
      LDA   p_expr_type
      JSR   p_type_is_real
      BCS   p_emit_store_with_real_copy
      LDA   p_expr_type
      CMP   #TYPE_WORD
      BEQ   p_emit_store_with_real_word
      CMP   #TYPE_BYTE
      BEQ   p_emit_store_with_real_byte
@fail:
      SEC
      RTS

p_emit_store_with_real_copy:
      LDA   #<asm_string_copy_save
      LDX   #>asm_string_copy_save
      JSR   p_emit_ax_text
      BCS   @fail
      JSR   p_emit_with_field_address
      BCS   @fail
      LDA   #<asm_real_copy
      LDX   #>asm_real_copy
      JMP   p_emit_ax_text
@fail:
      SEC
      RTS

p_emit_store_with_string:
      LDA   #<asm_string_copy_save
      LDX   #>asm_string_copy_save
      JSR   p_emit_ax_text
      BCS   @fail
      JSR   p_emit_with_field_address
      BCS   @fail
      LDA   #<asm_string_copy_call
      LDX   #>asm_string_copy_call
      JSR   p_emit_ax_text
      BCS   @fail
      LDX   p_field_iter
      LDA   p_field_size_h,X
      BNE   @fail
      LDA   p_field_size_l,X
      BEQ   @fail
      DEC
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   #$0A
      JMP   p_emit
@fail:
      SEC
      RTS

p_emit_store_with_real_byte:
      LDA   #<asm_real_save_byte
      LDX   #>asm_real_save_byte
      BRA   p_emit_store_with_real
p_emit_store_with_real_word:
      LDA   #<asm_real_save_word
      LDX   #>asm_real_save_word
p_emit_store_with_real:
      JSR   p_emit_ax_text
      BCS   @fail
      JSR   p_emit_with_field_address
      BCS   @fail
      LDA   #<asm_real_store_integer
      LDX   #>asm_real_store_integer
      JMP   p_emit_ax_text
@fail:
      SEC
      RTS

; Emit a Pascal support macro named by A/X for array symbol Y.
p_emit_array_operation:
      STY   p_symbol_iter
      JSR   p_emit_ax_text
      BCS   @fail
      LDX   p_symbol_iter
      JSR   p_emit_symbol_name
      BCS   @fail
      LDA   #$0A
      JMP   p_emit
@fail:
      SEC
      RTS

; Classic system functions predate typed unit metadata, so retain their
; result and argument contract in one small table. Real-valued functions
; receive a normalized 16.16 pointer even when the source argument is an
; ordinal literal such as LN(10).
p_prepare_system_function:
      LDA   #TYPE_BYTE
      STA   p_function_result_type
      LDX   #0
@find:
      CPX   #SYSTEM_FUNCTION_COUNT
      BCS   @done
      LDA   p_call_hash
      CMP   system_function_hash0,X
      BNE   @next
      LDA   p_call_hash+1
      CMP   system_function_hash1,X
      BNE   @next
      LDA   p_call_hash+2
      CMP   system_function_hash2,X
      BEQ   @found
@next:
      INX
      BRA   @find
@found:
      LDA   system_function_kind,X
      CMP   #SYSFN_SAME
      BEQ   @same
      CMP   #SYSFN_WORD
      BEQ   @word
      LDA   #TYPE_REAL
      STA   p_function_result_type
      BRA   @normalize_real
@same:
      LDA   p_function_arg_type
      STA   p_function_result_type
      JSR   p_type_is_real
      BCC   @done
      BRA   @real_ready
@word:
      LDA   #TYPE_WORD
      STA   p_function_result_type
@normalize_real:
      LDA   p_function_arg_type
      JSR   p_type_is_real
      BCS   @real_ready
      LDA   p_function_arg_type
      CMP   #TYPE_WORD
      BEQ   @from_word
      LDA   #<asm_real_from_byte
      LDX   #>asm_real_from_byte
      JMP   p_emit_ax_text
@from_word:
      LDA   #<asm_real_from_word
      LDX   #>asm_real_from_word
      JMP   p_emit_ax_text
@real_ready:
@done:
      CLC
      RTS

; Parse a byte value into A in generated code. Byte(NDK_CONSTANT) is an
; immediate typed constant; function arguments and results use A.
p_parse_byte_value:
      JSR   p_skip_ws
      JSR   p_peek
      long_bcc @bad
      CMP   #'^'
      long_beq @control
      CMP   #'#'
      long_beq @numeric_character
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
      LDA   #<kw_true
      LDX   #>kw_true
      JSR   p_ident_is
      BCC   @false
      LDA   #1
      JSR   p_emit_load_byte
      long_bcs @bad
      LDA   #TYPE_BOOLEAN
      STA   p_expr_type
      CLC
      RTS
@false:
      LDA   #<kw_false
      LDX   #>kw_false
      JSR   p_ident_is
      BCC   @byte_cast
      LDA   #0
      JSR   p_emit_load_byte
      long_bcs @bad
      LDA   #TYPE_BOOLEAN
      STA   p_expr_type
      CLC
      RTS
@byte_cast:
      LDA   #<kw_byte
      LDX   #>kw_byte
      JSR   p_ident_is
      long_bcs @constant
      LDA   #<kw_copy
      LDX   #>kw_copy
      JSR   p_ident_is
      long_bcs p_parse_copy_function
      LDA   #<kw_chr
      LDX   #>kw_chr
      JSR   p_ident_is
      long_bcs p_parse_chr_function
      JSR   p_skip_ws
      JSR   p_peek
      long_bcc @load_identifier
      CMP   #'['
      long_beq @designator
      CMP   #'.'
      long_beq @designator
      CMP   #'('
      long_bne @load_identifier
      JSR   p_save_call_identifier
      JSR   p_push_call_identifier
      long_bcs @bad
      STZ   p_started
      STZ   p_function_arg_type
      JSR   p_next
      JSR   p_skip_ws
      JSR   p_peek
      long_bcc @call_bad
      CMP   #')'
      BEQ   @function_close
      JSR   p_parse_expression
      long_bcs @call_bad
      LDA   p_expr_type
      STA   p_function_arg_type
      LDA   #1
      STA   p_started
@function_close:
      LDA   #')'
      JSR   p_expect_char
      long_bcs @call_bad
      JSR   p_restore_call_identifier
      long_bcs @call_bad
      JSR   p_routine_find_call
      BCC   @external_function
      LDA   p_routine_kind_table,X
      AND   #$3F
      BEQ   @call_bad
      LDA   p_routine_params,X
      CMP   p_started
      BNE   @call_bad
      STX   p_routine_iter
      JSR   p_emit_local_call_buffered
      long_bcs @call_bad
      LDX   p_routine_iter
      LDA   p_routine_return,X
      BRA   @result_type
@external_function:
      LDA   p_started
      CLC
      ADC   #2
      JSR   p_emit_sig_call
      long_bcs @call_bad
      JSR   p_prepare_system_function
      long_bcs @call_bad
      JSR   p_emit_call_buffered
      long_bcs @call_bad
      LDA   p_function_result_type
@result_type:
      STA   p_expr_type
      JSR   p_pop_call_identifier
      CLC
      RTS

@call_bad:
      JSR   p_pop_call_identifier
      JMP   @bad
@designator:
      JSR   p_symbol_find
      BCC   @generic_designator
      JSR   p_simple_global_byte_array
      BCC   @generic_designator
      JMP   p_parse_simple_array_value
@generic_designator:
      JMP   p_parse_designator_value
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
      long_bcs @bad
      LDA   #TYPE_BYTE
      STA   p_expr_type
      CLC
      RTS
@load_identifier:
      JSR   p_symbol_find
      BCC   @with_field
      STA   p_expr_type
      LDA   p_symbol_flags
      BNE   @frame_value
      LDA   p_expr_type
      CMP   #TYPE_ARRAY_BYTE
      BNE   @sized_global
      LDA   p_symbol_offset+1
      BNE   @set_address
      LDA   p_symbol_offset
      CMP   #1
      BEQ   @known_byte
      CMP   #2
      BCS   @set_address
      BRA   @global_address
@set_address:
      LDA   #TYPE_SET_REF
      STA   p_expr_type
      BRA   @global_address
@sized_global:
      JSR   p_type_size
      long_bcs @bad
      LDA   p_array_len+1
      BNE   @global_address
      LDA   p_array_len
      CMP   #3
      BCS   @global_address
      LDA   p_expr_type
      CMP   #TYPE_WORD
      BNE   @known_byte
      JSR   p_emit_load_word_identifier
      RTS
@known_byte:
      JSR   p_emit_load_identifier
      RTS
@frame_value:
      JSR   p_emit_load_frame_value
      RTS
@global_address:
      JSR   p_emit_address_identifier
      RTS
@with_field:
      JSR   p_with_field_find
      BCC   @unknown_identifier
      JMP   p_emit_with_field_load
@unknown_identifier:
      JSR   p_const_find
      BCC   @routine_identifier
      STX   p_const_iter
      LDA   p_const_type,X
      STA   p_expr_type
      CMP   #TYPE_WORD
      BEQ   @constant_word
      LDA   p_const_value_l,X
      JSR   p_emit_load_byte
      RTS
@constant_word:
      LDA   p_const_value_l,X
      STA   p_decimal
      LDA   p_const_value_h,X
      STA   p_decimal+1
      JSR   p_emit_load_word
      RTS
@routine_identifier:
      JSR   p_save_call_identifier
      JSR   p_routine_find_call
      BCC   @external_identifier
      LDA   p_routine_kind_table,X
      AND   #$3F
      BEQ   @external_identifier
      LDA   p_routine_params,X
      BNE   @external_identifier
      STX   p_routine_iter
      JSR   p_emit_local_call_buffered
      long_bcs @bad
      LDX   p_routine_iter
      LDA   p_routine_return,X
      STA   p_expr_type
      CLC
      RTS
@external_identifier:
      LDX   #0
@system_noarg:
      CPX   #SYSTEM_NOARG_FUNCTION_COUNT
      BCS   @external_value_check
      LDA   p_call_hash
      CMP   system_noarg_hash0,X
      BNE   @system_noarg_next
      LDA   p_call_hash+1
      CMP   system_noarg_hash1,X
      BNE   @system_noarg_next
      LDA   p_call_hash+2
      CMP   system_noarg_hash2,X
      BEQ   @system_noarg_call
@system_noarg_next:
      INX
      BRA   @system_noarg
@system_noarg_call:
      LDA   #2
      JSR   p_emit_sig_call
      long_bcs @bad
      JSR   p_emit_call_buffered
      long_bcs @bad
      LDA   #TYPE_BYTE
      STA   p_expr_type
      CLC
      RTS
@external_value_check:
      LDX   #0
@system_storage:
      CPX   #SYSTEM_STORAGE_COUNT
      BCS   @kbd_check
      LDA   p_call_hash
      CMP   system_storage_hash0,X
      BNE   @system_storage_next
      LDA   p_call_hash+1
      CMP   system_storage_hash1,X
      BNE   @system_storage_next
      LDA   p_call_hash+2
      CMP   system_storage_hash2,X
      BEQ   @system_storage_load
@system_storage_next:
      INX
      BRA   @system_storage
@system_storage_load:
      LDA   #TYPE_BYTE
      STA   p_expr_type
      JSR   p_emit_load_identifier
      RTS
@kbd_check:
      LDA   p_call_ident_len
      CMP   #3
      BNE   @external_value
      LDA   p_call_ident+0
      CMP   #'K'
      BNE   @external_value
      LDA   p_call_ident+1
      CMP   #'B'
      BNE   @external_value
      LDA   p_call_ident+2
      CMP   #'D'
      BNE   @external_value
      LDA   #TYPE_WORD
      STA   p_expr_type
      JSR   p_emit_address_identifier
      RTS
@external_value:
      LDA   #TYPE_BYTE
      STA   p_expr_type
      JSR   p_emit_external_value_identifier
      RTS
@character:
      JSR   p_next
      JSR   p_scan_string_length
      long_bcs @bad
      LDA   p_string_len
      CMP   #1
      long_bne p_emit_string_literal
      JSR   p_next
      long_bcc @bad
      STA   p_char
      LDA   #$27
      JSR   p_expect_char
      long_bcs @bad
      LDA   p_char
      JSR   p_emit_load_byte
      long_bcs @bad
      LDA   #TYPE_CHAR
      STA   p_expr_type
      CLC
      RTS
@control:
      JSR   p_next
      JSR   p_next
      long_bcc @bad
      AND   #$1F
      JSR   p_emit_load_byte
      long_bcs @bad
      LDA   #TYPE_CHAR
      STA   p_expr_type
      CLC
      RTS
@numeric_character:
      JSR   p_next
      JSR   p_parse_decimal_literal
      long_bcs @bad
      LDA   p_decimal+1
      long_bne @bad
      LDA   p_decimal
      JSR   p_emit_load_byte
      long_bcs @bad
      LDA   #TYPE_CHAR
      STA   p_expr_type
      CLC
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
      long_bcs @bad
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
      long_beq @bad
      LDA   p_char
      JSR   p_emit_load_byte
      long_bcs @bad
      LDA   #TYPE_BYTE
      STA   p_expr_type
      CLC
      RTS
@decimal:
      JSR   p_parse_decimal_literal
      long_bcs @bad
      LDA   p_decimal+1
      BEQ   @decimal_byte
      JSR   p_emit_load_word
      long_bcs @bad
      LDA   #TYPE_WORD
      STA   p_expr_type
      CLC
      RTS
@decimal_byte:
      LDA   p_decimal
      JSR   p_emit_load_byte
      long_bcs @bad
      LDA   #TYPE_BYTE
      STA   p_expr_type
      CLC
      RTS
@bad:
      SEC
      RTS

; COPY(source, index, count) returns a short string in a runtime temporary.
; The source pointer and index are stacked; count remains in A/X.
p_parse_copy_function:
      LDA   #'('
      JSR   p_expect_char
      long_bcs @fail
      JSR   p_parse_expression
      long_bcs @fail
      LDA   p_expr_type
      JSR   p_type_is_string
      long_bcc @fail
      LDA   #<asm_phx_pha
      LDX   #>asm_phx_pha
      JSR   p_emit_ax_text
      long_bcs @fail
      LDA   #','
      JSR   p_expect_char
      long_bcs @fail
      JSR   p_parse_expression
      long_bcs @fail
      JSR   p_emit_push_word_value
      long_bcs @fail
      LDA   #','
      JSR   p_expect_char
      long_bcs @fail
      JSR   p_parse_expression
      long_bcs @fail
      LDA   p_expr_type
      CMP   #TYPE_WORD
      BEQ   @count_ready
      LDA   #<asm_index_high_zero
      LDX   #>asm_index_high_zero
      JSR   p_emit_ax_text
      long_bcs @fail
@count_ready:
      LDA   #')'
      JSR   p_expect_char
      long_bcs @fail
      LDA   #<asm_string_copy_part
      LDX   #>asm_string_copy_part
      JSR   p_emit_ax_text
      long_bcs @fail
      LDA   #<asm_drop_four_preserve_ax
      LDX   #>asm_drop_four_preserve_ax
      JSR   p_emit_ax_text
      long_bcs @fail
      LDA   #TYPE_STRING_REF
      STA   p_expr_type
      CLC
      RTS
@fail:
      SEC
      RTS

; CHR is an ordinal conversion, not an external routine. The low byte is
; already in A, so only its Pascal type changes.
p_parse_chr_function:
      LDA   #'('
      JSR   p_expect_char
      BCS   @fail
      JSR   p_parse_expression
      BCS   @fail
      LDA   p_expr_type
      JSR   p_type_is_real
      BCS   @fail
      LDA   #')'
      JSR   p_expect_char
      BCS   @fail
      LDA   #TYPE_CHAR
      STA   p_expr_type
      CLC
@fail:
      RTS

; Parse one unsigned decimal byte literal into p_decimal without emitting code.
p_parse_decimal_literal:
      STZ   p_decimal
      STZ   p_decimal+1
      STZ   p_started
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
      INC   p_started
      LDA   p_decimal
      ASL
      STA   p_remainder
      LDA   p_decimal+1
      ROL
      STA   p_remainder+1
      BCS   @bad
      ASL   p_decimal
      ROL   p_decimal+1
      BCS   @bad
      ASL   p_decimal
      ROL   p_decimal+1
      BCS   @bad
      ASL   p_decimal
      ROL   p_decimal+1
      BCS   @bad
      CLC
      LDA   p_decimal
      ADC   p_remainder
      STA   p_decimal
      LDA   p_decimal+1
      ADC   p_remainder+1
      STA   p_decimal+1
      BCS   @bad
      CLC
      LDA   p_decimal
      ADC   p_digit
      STA   p_decimal
      LDA   p_decimal+1
      ADC   #0
      STA   p_decimal+1
      BCS   @bad
      JSR   p_next
      BRA   @decimal_digit
@decimal_done:
      LDA   p_started
      BEQ   @bad
      CLC
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

; Source files remain ordinary disk files. Includes use the same NDK Memory
; allocator and XRAM loader as the resident primary-source path.
p_init_source_name:
      LDA   NPTOOL_ARG0_LEN
      CMP   #P_SOURCE_NAME_CAP
      BCC   :+
      LDA   #P_SOURCE_NAME_CAP-1
:
      STA   p_current_name_len
      TAX
      LDY   #0
@copy:
      CPY   p_current_name_len
      BCS   @done
      LDA   NPTOOL_ARG0,Y
      STA   p_current_name,Y
      INY
      BRA   @copy
@done:
      LDA   #0
      STA   p_current_name,Y
      RTS

p_publish_diag_source:
      LDA   p_include_depth
      BNE   @current
      LDA   p_source_error
      BEQ   @primary
      LDA   p_include_name_len
      BEQ   @primary
      STA   NPTOOL_ARG6_LEN
      LDY   #0
@include:
      CPY   p_include_name_len
      BCS   @include_done
      LDA   p_include_name,Y
      STA   NPTOOL_ARG6,Y
      INY
      BRA   @include
@include_done:
      LDA   #0
      STA   NPTOOL_ARG6,Y
      RTS
@current:
      LDA   p_current_name_len
      STA   NPTOOL_ARG6_LEN
      LDY   #0
@copy:
      CPY   p_current_name_len
      BCS   @done
      LDA   p_current_name,Y
      STA   NPTOOL_ARG6,Y
      INY
      BRA   @copy
@done:
      LDA   #0
      STA   NPTOOL_ARG6,Y
@primary:
      RTS

p_clear_lib_args:
      LDX   #15
@clear:
      STZ   LIB_ARG0,X
      DEX
      BPL   @clear
      RTS

; Generated assembly is an ordinary sequential disk stream. A single page in
; overlay RAM is enough regardless of source size, and all ownership stays in
; the NDK Files module instead of a compiler-private XRAM convention.
p_files_call:
      STA   LIB_FN_ID
      LDA   #MODULE_ID_FILES
      STA   LIB_MOD_ID
      JSR   LIB_LOADER_BAND
      STZ   p_window_valid
      LDA   LIB_STATUS
      RTS

p_open_output:
      JSR   p_clear_lib_args
      LDA   #<NPTOOL_ARG1
      STA   LIB_ARG0
      LDA   #>NPTOOL_ARG1
      STA   LIB_ARG0+1
      LDA   NPTOOL_ARG1_LEN
      STA   LIB_ARG1
      LDA   #FIO_FILE_ACCESS_WRITE
      STA   LIB_ARG2
      LDA   #FILE_FCREATE
      JSR   p_files_call
      BNE   @done
      LDA   LIB_RESULT
      STA   p_output_file_id
      LDA   LIB_RESULT+1
      STA   p_output_file_id+1
      INC   p_output_open
      LDA   #0
@done:
      RTS

p_flush_output:
      LDA   p_output_used
      ORA   p_output_used+1
      BEQ   @ok
      JSR   p_clear_lib_args
      LDA   p_output_file_id
      STA   LIB_ARG0
      LDA   p_output_file_id+1
      STA   LIB_ARG0+1
      LDA   #<p_output_buffer
      STA   LIB_ARG1
      LDA   #>p_output_buffer
      STA   LIB_ARG1+1
      LDA   p_output_used
      STA   LIB_ARG2
      LDA   p_output_used+1
      STA   LIB_ARG2+1
      LDA   #FILE_FWRITE
      JSR   p_files_call
      BNE   @fail
      LDA   LIB_RESULT
      CMP   p_output_used
      BNE   @fail
      LDA   LIB_RESULT+1
      CMP   p_output_used+1
      BNE   @fail
      STZ   p_output_used
      STZ   p_output_used+1
@ok:
      LDA   #0
      RTS
@fail:
      LDA   #1
      RTS

p_close_output:
      LDA   p_output_open
      BEQ   @ok
      STZ   p_output_open
      JSR   p_clear_lib_args
      LDA   p_output_file_id
      STA   LIB_ARG0
      LDA   p_output_file_id+1
      STA   LIB_ARG0+1
      LDA   #FILE_FCLOSE
      JMP   p_files_call
@ok:
      LDA   #0
      RTS

p_delete_output:
      JSR   p_clear_lib_args
      LDA   #<NPTOOL_ARG1
      STA   LIB_ARG0
      LDA   #>NPTOOL_ARG1
      STA   LIB_ARG0+1
      LDA   NPTOOL_ARG1_LEN
      STA   LIB_ARG1
      LDA   #FILE_FDELETE
      JMP   p_files_call

p_finalize_output:
      LDA   p_output_open
      BEQ   @done
      LDA   p_frontend_status
      BNE   @discard
      JSR   p_flush_output
      BEQ   @close
      LDA   #PASCAL_ERR_OUTPUT
      STA   pascal_error
      LDA   #1
      STA   p_frontend_status
      BRA   @close
@discard:
      STZ   p_output_used
      STZ   p_output_used+1
@close:
      JSR   p_close_output
      BEQ   @cleanup
      LDA   p_frontend_status
      BNE   @cleanup
      LDA   #PASCAL_ERR_OUTPUT
      STA   pascal_error
      LDA   #1
      STA   p_frontend_status
@cleanup:
      LDA   p_frontend_status
      BEQ   @done
      JSR   p_delete_output
@done:
      RTS

p_memory_call:
      STA   LIB_FN_ID
      LDA   #MODULE_ID_MEMORY
      STA   LIB_MOD_ID
      JSR   LIB_LOADER_BAND
      STZ   p_window_valid
      LDA   LIB_STATUS
      RTS

p_allocate_state:
      JSR   p_clear_lib_args
      LDA   #<P_STATE_SIZE
      STA   LIB_ARG2
      LDA   #>P_STATE_SIZE
      STA   LIB_ARG2+1
      LDA   #MEM_ALLOC
      JSR   p_memory_call
      BNE   @done
      LDX   #2
@copy:
      LDA   LIB_RESULT,X
      STA   p_state_xaddr,X
      DEX
      BPL   @copy
      INC   p_state_allocated
      LDA   #0
@done:
      RTS

p_release_state:
      LDA   p_state_allocated
      BEQ   @ok
      STZ   p_state_allocated
      JSR   p_clear_lib_args
      LDX   #2
@address:
      LDA   p_state_xaddr,X
      STA   LIB_ARG0,X
      DEX
      BPL   @address
      LDA   #<P_STATE_SIZE
      STA   LIB_ARG2
      LDA   #>P_STATE_SIZE
      STA   LIB_ARG2+1
      LDA   #MEM_RELEASE
      JMP   p_memory_call
@ok:
      LDA   #0
      RTS

p_open_include:
      LDA   p_include_depth
      CMP   #P_INCLUDE_DEPTH_CAP
      long_bcs @fail
      CMP   p_include_count
      BCC   @reuse
      LDA   p_include_count
      CMP   #P_INCLUDE_COUNT_CAP
      long_bcs @fail

      JSR   p_clear_lib_args
      LDA   #<NPC_INCLUDE_CAP
      STA   LIB_ARG2
      LDA   #>NPC_INCLUDE_CAP
      STA   LIB_ARG2+1
      LDA   #MEM_ALLOC
      JSR   p_memory_call
      long_bne @alloc_fail
      LDX   p_include_count
      LDA   LIB_RESULT
      STA   p_include_alloc_l,X
      STA   p_include_xaddr
      LDA   LIB_RESULT+1
      STA   p_include_alloc_m,X
      STA   p_include_xaddr+1
      LDA   LIB_RESULT+2
      STA   p_include_alloc_h,X
      STA   p_include_xaddr+2
      INC   p_include_count
      BRA   @load

      ; Finished sequential includes reuse the buffer for this nesting depth.
      ; Only simultaneously nested sources consume additional XRAM pages.
@reuse:
      TAX
      LDA   p_include_alloc_l,X
      STA   p_include_xaddr
      LDA   p_include_alloc_m,X
      STA   p_include_xaddr+1
      LDA   p_include_alloc_h,X
      STA   p_include_xaddr+2

@load:
      JSR   p_clear_lib_args
      LDA   #<p_include_name
      STA   LIB_ARG0
      LDA   #>p_include_name
      STA   LIB_ARG0+1
      LDA   p_include_name_len
      STA   LIB_ARG1
      LDX   #2
@load_address:
      LDA   p_include_xaddr,X
      STA   LIB_ARG2,X
      DEX
      BPL   @load_address
      LDA   #<NPC_SOURCE_PAGE_SIZE
      STA   LIB_ARG3
      LDA   #>NPC_SOURCE_PAGE_SIZE
      STA   LIB_ARG3+1
      LDA   #MEM_XLOAD
      JSR   p_memory_call
      long_bne @load_fail
      LDA   XRAM_LENL
      STA   p_include_len
      LDA   XRAM_LENH
      STA   p_include_len+1

      LDX   p_include_depth
      LDA   p_src
      STA   p_include_stack_src_l,X
      LDA   p_src+1
      STA   p_include_stack_src_m,X
      LDA   p_src+2
      STA   p_include_stack_src_h,X
      LDA   p_left
      STA   p_include_stack_left_l,X
      LDA   p_left+1
      STA   p_include_stack_left_h,X
      LDA   p_stream_base
      STA   p_include_stack_base_l,X
      LDA   p_stream_base+1
      STA   p_include_stack_base_m,X
      LDA   p_stream_base+2
      STA   p_include_stack_base_h,X
      LDA   p_stream_next
      STA   p_include_stack_next_l,X
      LDA   p_stream_next+1
      STA   p_include_stack_next_m,X
      LDA   p_stream_next+2
      STA   p_include_stack_next_h,X
      LDA   p_stream_more
      STA   p_include_stack_more,X
      LDA   p_stream_slot
      STA   p_include_stack_slot,X
      LDA   p_line
      STA   p_include_stack_line_l,X
      LDA   p_line+1
      STA   p_include_stack_line_h,X
      LDA   p_column
      STA   p_include_stack_col_l,X
      LDA   p_column+1
      STA   p_include_stack_col_h,X
      LDA   p_current_name_len
      STA   p_include_stack_name_len,X

      LDA   p_include_depth
      ASL
      ASL
      ASL
      ASL
      ASL
      TAX
      LDY   #0
@save_name:
      LDA   p_current_name,Y
      STA   p_include_stack_names,X
      INX
      INY
      CPY   #P_SOURCE_NAME_CAP
      BCC   @save_name

      LDA   p_include_name_len
      STA   p_current_name_len
      LDX   #P_SOURCE_NAME_CAP-1
@new_name:
      LDA   p_include_name,X
      STA   p_current_name,X
      DEX
      BPL   @new_name
      LDX   #2
@new_source:
      LDA   p_include_xaddr,X
      STA   p_src,X
      DEX
      BPL   @new_source
      LDA   p_include_len
      STA   p_left
      LDA   p_include_len+1
      STA   p_left+1
      JSR   p_stream_init
      LDA   #1
      STA   p_line
      STZ   p_line+1
      STA   p_column
      STZ   p_column+1
      INC   p_include_depth
      STZ   p_window_valid
      CLC
      RTS
@load_fail:
      LDA   #2
      STA   p_source_detail
      BRA   @fail
@alloc_fail:
      LDA   #1
      STA   p_source_detail
@fail:
      LDA   #PASCAL_ERR_INCLUDE
      STA   p_source_error
      SEC
      RTS

p_pop_include:
      LDA   p_include_depth
      long_beq @none
      DEC   p_include_depth
      LDX   p_include_depth
      LDA   p_include_stack_src_l,X
      STA   p_src
      LDA   p_include_stack_src_m,X
      STA   p_src+1
      LDA   p_include_stack_src_h,X
      STA   p_src+2
      LDA   p_include_stack_left_l,X
      STA   p_left
      LDA   p_include_stack_left_h,X
      STA   p_left+1
      LDA   p_include_stack_base_l,X
      STA   p_stream_base
      LDA   p_include_stack_base_m,X
      STA   p_stream_base+1
      LDA   p_include_stack_base_h,X
      STA   p_stream_base+2
      LDA   p_include_stack_next_l,X
      STA   p_stream_next
      LDA   p_include_stack_next_m,X
      STA   p_stream_next+1
      LDA   p_include_stack_next_h,X
      STA   p_stream_next+2
      LDA   p_include_stack_more,X
      STA   p_stream_more
      LDA   p_include_stack_slot,X
      STA   p_stream_slot
      LDA   p_include_stack_line_l,X
      STA   p_line
      LDA   p_include_stack_line_h,X
      STA   p_line+1
      LDA   p_include_stack_col_l,X
      STA   p_column
      LDA   p_include_stack_col_h,X
      STA   p_column+1
      LDA   p_include_stack_name_len,X
      STA   p_current_name_len

      LDA   p_include_depth
      ASL
      ASL
      ASL
      ASL
      ASL
      TAX
      LDY   #0
@restore_name:
      LDA   p_include_stack_names,X
      STA   p_current_name,Y
      INX
      INY
      CPY   #P_SOURCE_NAME_CAP
      BCC   @restore_name
      STZ   p_window_valid
      CLC
      RTS
@none:
      SEC
      RTS

p_release_includes:
      STZ   p_release_error
@next:
      LDA   p_include_count
      BEQ   @done
      DEC   p_include_count
      JSR   p_clear_lib_args
      LDX   p_include_count
      LDA   p_include_alloc_l,X
      STA   LIB_ARG0
      LDA   p_include_alloc_m,X
      STA   LIB_ARG0+1
      LDA   p_include_alloc_h,X
      STA   LIB_ARG0+2
      LDA   #<NPC_INCLUDE_CAP
      STA   LIB_ARG2
      LDA   #>NPC_INCLUDE_CAP
      STA   LIB_ARG2+1
      LDA   #MEM_RELEASE
      JSR   p_memory_call
      BEQ   @next
      STA   p_release_error
      BRA   @next
@done:
      LDA   p_release_error
      RTS

p_source_fail:
      LDA   #PASCAL_ERR_INCLUDE
      STA   p_source_error
      STZ   p_left
      STZ   p_left+1
      STZ   p_stream_more
      RTS

; Consume through a closing brace. The opening brace (and, for directives,
; any recognized prefix) has already been consumed.
p_skip_brace_tail:
@next:
      JSR   p_next
      BCC   @fail
      CMP   #'}'
      BNE   @next
      CLC
      RTS
@fail:
      JSR   p_source_fail
      SEC
      RTS

; Called after "{$". Turbo compiler switches are accepted and ignored;
; {$I file} changes the active source frame after consuming the directive.
p_parse_directive:
      JSR   p_read_upper
      BCC   @fail
      CMP   #'I'
      BNE   @ignore
      JSR   p_peek
      BCC   @fail
      CMP   #' '
      BEQ   @include
      CMP   #$09
      BEQ   @include
      ; {$I-} and {$I+} are I/O-check switches, not include directives.
@ignore:
      JMP   p_skip_brace_tail

@include:
@space:
      JSR   p_peek
      BCC   @fail
      CMP   #' '
      BEQ   @eat_space
      CMP   #$09
      BNE   @filename_start
@eat_space:
      JSR   p_next
      BRA   @space
@filename_start:
      STZ   p_include_name_len
@filename:
      JSR   p_peek
      BCC   @fail
      CMP   #'}'
      BEQ   @filename_done
      CMP   #' '
      BEQ   @filename_done
      CMP   #$09
      BEQ   @filename_done
      CMP   #$0D
      BEQ   @filename_done
      CMP   #$0A
      BEQ   @filename_done
      LDX   p_include_name_len
      CPX   #P_SOURCE_NAME_CAP-1
      BCS   @fail
      JSR   p_next
      LDX   p_include_name_len
      STA   p_include_name,X
      INC   p_include_name_len
      BRA   @filename
@filename_done:
      LDA   p_include_name_len
      BEQ   @fail
      TAX
      STZ   p_include_name,X
      JSR   p_skip_brace_tail
      BCS   @fail
      JMP   p_open_include
@fail:
      JSR   p_source_fail
      SEC
      RTS

p_skip_paren_comment:
@next:
      JSR   p_next
      BCC   @fail
      CMP   #'*'
      BNE   @next
      JSR   p_peek
      BCC   @fail
      CMP   #')'
      BNE   @next
      JSR   p_next
      CLC
      RTS
@fail:
      JSR   p_source_fail
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
      BEQ   @eat
      CMP   #$7B
      BEQ   @brace_comment
      CMP   #'('
      BNE   @done
      JSR   p_mark_position
      JSR   p_next
      JSR   p_peek
      BCC   @restore
      CMP   #'*'
      BNE   @restore
      JSR   p_next
      JSR   p_skip_paren_comment
      BCS   @done
      BRA   @loop
@restore:
      JSR   p_restore_position
      BRA   @done
@brace_comment:
      JSR   p_next
      JSR   p_peek
      BCC   @comment_error
      CMP   #'$'
      BNE   @ordinary_comment
      JSR   p_next
      JSR   p_parse_directive
      BCS   @done
      BRA   @loop
@ordinary_comment:
      JSR   p_skip_brace_tail
      BCS   @done
      BRA   @loop
@comment_error:
      JSR   p_source_fail
      BRA   @done
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

      .segment "CODE"
; Initialize one source stream after its first page has been loaded into the
; first half of an allocator-owned 16 KiB XRAM block.
p_stream_init:
      LDX   #2
@base:
      LDA   p_src,X
      STA   p_stream_base,X
      DEX
      BPL   @base
      LDA   p_left
      STA   p_stream_next
      LDA   p_left+1
      STA   p_stream_next+1
      STZ   p_stream_next+2
      STZ   p_stream_slot
      STZ   p_stream_more
      LDA   p_left
      BNE   @done
      LDA   p_left+1
      CMP   #>NPC_SOURCE_PAGE_SIZE
      BNE   @done
      INC   p_stream_more
@done:
      RTS

; Refill the inactive half of the source allocation through the generic NDK
; file pager. Carry set means at least one byte is available; carry clear means
; EOF or a source error recorded in p_source_error/p_source_detail.
p_stream_refill:
      STZ   p_stream_more
      LDA   p_stream_slot
      EOR   #1
      STA   p_stream_slot
      JSR   p_clear_lib_args
      LDA   #<p_current_name
      STA   LIB_ARG0
      LDA   #>p_current_name
      STA   LIB_ARG0+1
      LDA   p_current_name_len
      STA   LIB_ARG0+2
      LDX   #2
@offset:
      LDA   p_stream_next,X
      STA   LIB_ARG1,X
      LDA   p_stream_base,X
      STA   LIB_ARG2,X
      STA   p_src,X
      DEX
      BPL   @offset
      STZ   LIB_ARG1+3                 ; PAGER_TARGET_XRAM
      LDA   p_stream_slot
      BEQ   @destination_ready
      CLC
      LDA   LIB_ARG2+1
      ADC   #>NPC_SOURCE_PAGE_SIZE
      STA   LIB_ARG2+1
      STA   p_src+1
      LDA   LIB_ARG2+2
      ADC   #0
      STA   LIB_ARG2+2
      STA   p_src+2
@destination_ready:
      LDA   #<NPC_SOURCE_PAGE_SIZE
      STA   LIB_ARG3
      LDA   #>NPC_SOURCE_PAGE_SIZE
      STA   LIB_ARG3+1
      LDA   #FILE_PAGE
      JSR   p_files_call
      BNE   @error
      LDA   LIB_RESULT
      STA   p_left
      LDA   LIB_RESULT+1
      STA   p_left+1

      CLC
      LDA   p_stream_next
      ADC   p_left
      STA   p_stream_next
      LDA   p_stream_next+1
      ADC   p_left+1
      STA   p_stream_next+1
      LDA   p_stream_next+2
      ADC   #0
      STA   p_stream_next+2
      BCS   @available                 ; 24-bit maximum reached after this page
      LDA   p_left
      BNE   @available
      LDA   p_left+1
      CMP   #>NPC_SOURCE_PAGE_SIZE
      BNE   @available
      INC   p_stream_more
@available:
      LDA   p_left
      ORA   p_left+1
      BEQ   @eof
      SEC
      RTS
@error:
      STA   p_source_detail
      LDA   #PASCAL_ERR_INCLUDE
      STA   p_source_error
      STZ   p_left
      STZ   p_left+1
      STZ   p_stream_more
@eof:
      CLC
      RTS

p_peek:
@again:
      LDA   p_left
      ORA   p_left+1
      BNE   @have_source
      LDA   p_stream_more
      BEQ   @source_done
      JSR   p_stream_refill
      BCS   @have_source
      LDA   p_source_error
      BNE   @eof
@source_done:
      LDA   p_include_depth
      BEQ   @eof
      JSR   p_pop_include
      BRA   @again
@have_source:
      LDA   p_window_valid
      BEQ   @map
      LDA   p_src+1
      CMP   p_window_m
      BNE   @map
      LDA   p_src+2
      CMP   p_window_h
      BEQ   @read
@map:
      STZ   XMC_W2AL
      LDA   p_src+1
      STA   p_window_m
      STA   XMC_W2AM
      LDA   p_src+2
      STA   p_window_h
      STA   XMC_W2AH
      LDA   XMC_WINCTL
      ORA   #XRAM_WIN2_ENABLE
      STA   XMC_WINCTL
      INC   p_window_valid
@read:
      LDX   p_src
      LDA   XRAM_WIN2_BASE,X
      CMP   #$1A
      BNE   @character
      ; CP/M text files end at the first SUB byte; bytes remaining in the last
      ; 128-byte record are padding and may contain stale source fragments.
      STZ   p_left
      STZ   p_left+1
      STZ   p_stream_more
      LDA   p_include_depth
      BEQ   @eof
      JSR   p_pop_include
      BRA   @again
@character:
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

      .segment "NPCFE_RODATA"
SYSTEM_CALL_METADATA_COUNT = 21
SYSTEM_FUNCTION_COUNT = 12
SYSTEM_NOARG_FUNCTION_COUNT = 3
SYSTEM_STORAGE_COUNT = 2
p_argument_bits:  .byte $01, $02, $04, $08, $10, $20, $40, $80
; Hash bytes are little-endian DJB2-24 values. Arity disambiguates the two
; classic READ and READLN forms.
; Reference and Integer-width masks for the classic System interface plus the
; Pascal-shaped NovaMemory adapter. Hashes remain generated ABI identities;
; no NDK address or function id is embedded in NPC.
system_call_hash0: .byte $C1,$C1,$1B,$1B,$EA,$E8,$B8,$27,$BB,$68,$DA,$CF,$C8,$5F,$19,$08,$D0,$67,$21,$10,$94
system_call_hash1: .byte $34,$34,$73,$73,$0A,$19,$47,$93,$71,$29,$C5,$44,$6B,$65,$C2,$C5,$52,$8B,$E8,$AC,$A4
system_call_hash2: .byte $8B,$8B,$2B,$2B,$8B,$F2,$89,$2F,$E6,$88,$D9,$42,$E3,$B4,$BA,$74,$B3,$1F,$25,$44,$F4
system_call_arity: .byte $01,$02,$01,$02,$02,$01,$03,$01,$01,$03,$03,$02,$02,$01,$04,$04,$02,$01,$04,$04,$01
system_call_refmask: .byte $01,$02,$01,$02,$01,$01,$01,$01,$01,$06,$02,$00,$01,$01,$05,$05,$01,$01,$05,$05,$00
system_call_widthmask: .byte $00,$00,$00,$00,$00,$00,$06,$00,$00,$00,$04,$03,$02,$00,$0A,$0A,$02,$00,$0A,$0A,$01
; ABS, SQRT, SQR, SIN, COS, ARCTAN, LN, EXP, TRUNC, UPCASE, SUCC, PRED.
system_function_hash0: .byte $3B,$6F,$BB,$AF,$6A,$5E,$5F,$12,$71,$66,$73,$D0
system_function_hash1: .byte $D0,$F6,$1E,$1D,$DA,$5A,$74,$E4,$75,$07,$05,$53
system_function_hash2: .byte $87,$8B,$88,$88,$87,$70,$59,$87,$1D,$F3,$8C,$8A
system_function_kind:  .byte SYSFN_SAME,SYSFN_REAL,SYSFN_SAME,SYSFN_REAL
                       .byte SYSFN_REAL,SYSFN_REAL,SYSFN_REAL,SYSFN_REAL,SYSFN_WORD
                       .byte SYSFN_SAME,SYSFN_SAME,SYSFN_SAME
; Turbo Crt functions may be called without an empty parenthesized argument list.
; READKEY, WHEREX, WHEREY.
system_noarg_hash0: .byte $6A,$F8,$F9
system_noarg_hash1: .byte $D1,$F3,$F3
system_noarg_hash2: .byte $99,$0D,$0D
; Implicit System byte storage: KEYPRESSED, IORESULT.
system_storage_hash0: .byte $C4,$3C
system_storage_hash1: .byte $63,$53
system_storage_hash2: .byte $BD,$08
kw_program: .byte "PROGRAM", 0
kw_unit:    .byte "UNIT", 0
kw_interface: .byte "INTERFACE", 0
kw_implementation: .byte "IMPLEMENTATION", 0
kw_begin:   .byte "BEGIN", 0
kw_uses:    .byte "USES", 0
kw_const:   .byte "CONST", 0
kw_type:    .byte "TYPE", 0
kw_var:     .byte "VAR", 0
kw_byte:    .byte "BYTE", 0
kw_chr:     .byte "CHR", 0
kw_boolean: .byte "BOOLEAN", 0
kw_word:    .byte "WORD", 0
kw_integer: .byte "INTEGER", 0
kw_char:    .byte "CHAR", 0
kw_real:    .byte "REAL", 0
kw_string:  .byte "STRING", 0
kw_set:     .byte "SET", 0
kw_record:  .byte "RECORD", 0
kw_file:    .byte "FILE", 0
kw_text:    .byte "TEXT", 0
kw_true:    .byte "TRUE", 0
kw_false:   .byte "FALSE", 0
kw_array:   .byte "ARRAY", 0
kw_of:      .byte "OF", 0
kw_procedure: .byte "PROCEDURE", 0
kw_function:  .byte "FUNCTION", 0
kw_write:   .byte "WRITE", 0
kw_writeln: .byte "WRITELN", 0
kw_str:     .byte "STR", 0
kw_copy:    .byte "COPY", 0
kw_asm:     .byte "ASM", 0
kw_end:     .byte "END", 0
kw_if:      .byte "IF", 0
kw_then:    .byte "THEN", 0
kw_else:    .byte "ELSE", 0
kw_while:   .byte "WHILE", 0
kw_do:      .byte "DO", 0
kw_repeat:  .byte "REPEAT", 0
kw_until:   .byte "UNTIL", 0
kw_not:     .byte "NOT", 0
kw_for:     .byte "FOR", 0
kw_with:    .byte "WITH", 0
kw_case:    .byte "CASE", 0
kw_to:      .byte "TO", 0
kw_downto:  .byte "DOWNTO", 0
kw_or:      .byte "OR", 0
kw_and:     .byte "AND", 0
kw_mod:     .byte "MOD", 0
kw_div:     .byte "DIV", 0
kw_in:      .byte "IN", 0
hex_digits: .byte "0123456789ABCDEF"
set_bit_masks: .byte $01,$02,$04,$08,$10,$20,$40,$80
decimal_place_lo: .byte <10000, <1000, <100, <10, <1
decimal_place_hi: .byte >10000, >1000, >100, >10, >1
asm_comment: .byte "; ", 0
asm_import:
      .byte ".INCLUDE ", 34, "PASCAL.INC", 34, $0A
      .byte ".SEGMENT ", 34, "CODE", 34, $0A
      .byte 0
asm_import_footer:
      .byte ".IMPORTIFREF P_WRITE_CHAR,P_WRITE_BYTE,P_WRITE_BYTE_LN,P_WRITE_WORD,P_WRITE_FORMAT,P_WRITE_STRING,P_WRITE_REAL", $0A
      .byte ".IMPORTIFREF I_P_WRITE_LINE,I_ARRAY_INDEX_ADDR_STACK,I_ARRAY_INDEX_BYTE_STACK,I_P_BLOCK_COPY,I_P_STRING_COPY,P_STRING_COPY_PART", $0A
      .byte ".IMPORTIFREF P_STRING_CONCAT,P_STRING_CONCAT_CHAR,P_CHAR_CONCAT_STRING,P_CHAR_TO_STRING,P_STRING_COMPARE", $0A
      .byte ".IMPORTIFREF P_MULW,P_DIVW,P_MODW,P_REAL_FROM_BYTE,P_REAL_FROM_WORD,P_REAL_NEGATE,P_REAL_ADD,P_REAL_SUB,P_REAL_CMP", $0A
      .byte ".IMPORTIFREF P_REAL_MUL,P_REAL_MUL_WORD_LEFT,P_REAL_MUL_BYTE_LEFT,P_REAL_MUL_WORD_RIGHT,P_REAL_MUL_BYTE_RIGHT", $0A
      .byte ".IMPORTIFREF P_REAL_DIV,P_REAL_DIV_WORD,P_REAL_DIV_BYTE,P_WORD_DIV_REAL,P_BYTE_DIV_REAL,P_ORDINAL_DIV_REAL", $0A
      .byte ".IMPORTIFREF P_STR_REAL,P_STR_REAL_PRECISION,I_P_FILE_WRITE_BLOCK,P_FILE_WRITE_CHAR,P_FILE_WRITE_BYTE", $0A
      .byte ".IMPORTIFREF P_FILE_WRITE_WORD,P_FILE_WRITE_STRING,P_FILE_WRITE_REAL,P_FILE_WRITE_NEWLINE,P_SET_IN8,P_SET_IN256", $0A
      .byte ".IMPORTIFREF P_WITH_PUSH,P_WITH_POP,P_FENTER,P_FLEAVE,P_FADDR", $0A
      .byte ".IMPORTIFREF __NP_WITH_SP,__NP_WITH_STACK,__NP_SP,__NP_DISPLAY,__NP_STACK", $0A
      .byte ".IMPORTIFREF I_P_AGET,I_P_ASETB,I_P_ASETW,P_ADDW,P_SUBW,P_CMPW,P_EQ,P_NE,P_LT,P_LE,P_GT,P_GE", $0A
      .byte ".IMPORTIFREF GOTOXY,CLREOL,HIGHVIDEO,LOWVIDEO,DELAY,READ,UPCASE,CLRSCR,SUCC,PRED,ASSIGN,RESET,LENGTH", $0A
      .byte ".IMPORTIFREF DELETE,REWRITE,CLOSE,EOF,READLN,VAL,ABS,SQRT,SQR,SIN,COS,ARCTAN,LN,EXP,TRUNC,INSERT", $0A
      .byte ".IMPORTIFREF NORMVIDEO,HALT,READKEY,WHEREX,WHEREY,TEXTCOLOR,TEXTBACKGROUND", $0A
      .byte ".IMPORTIFREF KEYPRESSED,IORESULT,KBD", $0A
      .byte 0
asm_bss:    .byte ".SEGMENT ", 34, "BSS", 34, $0A, 0
asm_rodata: .byte ".SEGMENT ", 34, "RODATA", 34, $0A, 0
asm_code:   .byte ".SEGMENT ", 34, "CODE", 34, $0A, 0
asm_code_with_routines: .byte ".SEGMENT ", 34, "CODE", 34, $0A, "JMP __NP_MAIN", $0A, 0
asm_main_label: .byte "__NP_MAIN:", $0A, 0
asm_main_label_frames:
      .byte "__NP_MAIN:", $0A
      .byte "LDA #<__NP_STACK", $0A, "STA __NP_SP", $0A
      .byte "LDA #>__NP_STACK", $0A, "STA __NP_SP+1", $0A, 0
asm_byte_res: .byte ": .RES 1", $0A, 0
asm_word_res: .byte ": .RES 2", $0A, 0
asm_array_res: .byte ": .RES $", 0
asm_const_bytes: .byte ": .BYTE ", 0
asm_const_cont: .byte $0A, ".BYTE ", 0
asm_string_bytes: .byte ".BYTE $", 0
asm_set_bytes: .byte ".BYTE ", 0
asm_load_string_lo: .byte "LDA #<__NP_L", 0
asm_load_string_hi: .byte "LDX #>__NP_L", 0
asm_save_incoming: .byte "STA NVR1L", $0A, "STX NVR1H", $0A, 0

      .segment "RODATA"
asm_frame_enter_call: .byte "JSR P_FENTER", $0A, ".BYTE $", 0
asm_frame_enter_size: .byte $0A, ".WORD $", 0
asm_frame_enter_params: .byte $0A, ".BYTE $", 0
asm_frame_leave_call: .byte "JSR P_FLEAVE", $0A, ".BYTE $", 0
asm_frame_addr_call: .byte "JSR P_FADDR", $0A, ".BYTE $", 0
asm_frame_addr_offset: .byte $0A, ".WORD $", 0
asm_frame_load8_tail: .byte "LDY #$00", $0A, "LDA (NVR0L),Y", $0A, 0
asm_frame_load16_tail: .byte "LDY #$00", $0A, "LDA (NVR0L),Y", $0A, "STA NVR1L", $0A, "INY", $0A, "LDA (NVR0L),Y", $0A, "TAX", $0A, "LDA NVR1L", $0A, 0
asm_frame_store8_save: .byte "STA NVR1L", $0A, 0
asm_frame_store8_tail: .byte "LDY #$00", $0A, "LDA NVR1L", $0A, "STA (NVR0L),Y", $0A, 0
asm_frame_store16_save: .byte "STA NVR1L", $0A, "STX NVR1H", $0A, 0
asm_frame_store16_tail: .byte "LDY #$00", $0A, "LDA NVR1L", $0A, "STA (NVR0L),Y", $0A, "INY", $0A, "LDA NVR1H", $0A, "STA (NVR0L),Y", $0A, 0
asm_frame_direct_y: .byte "LDY #$", 0
asm_frame_direct_load8: .byte $0A, "LDA (__NP_FP),Y", $0A, 0
asm_frame_direct_load16: .byte $0A, "LDA (__NP_FP),Y", $0A, "STA NVR1L", $0A, "INY", $0A, "LDA (__NP_FP),Y", $0A, "TAX", $0A, "LDA NVR1L", $0A, 0
asm_frame_direct_store8: .byte $0A, "STA (__NP_FP),Y", $0A, 0
asm_frame_direct_store16: .byte $0A, "STA (__NP_FP),Y", $0A, "INY", $0A, "TXA", $0A, "STA (__NP_FP),Y", $0A, 0
asm_frame_store_ref8_save: .byte "STA NVR2L", $0A, 0
asm_frame_store_ref8_tail: .byte "STA NVR0L", $0A, "STX NVR0H", $0A, "LDY #$00", $0A, "LDA NVR2L", $0A, "STA (NVR0L),Y", $0A, 0
asm_frame_store_ref16_save: .byte "STA NVR2L", $0A, "STX NVR2H", $0A, 0
asm_frame_store_ref16_tail: .byte "STA NVR0L", $0A, "STX NVR0H", $0A, "LDY #$00", $0A, "LDA NVR2L", $0A, "STA (NVR0L),Y", $0A, "INY", $0A, "LDA NVR2H", $0A, "STA (NVR0L),Y", $0A, 0
      .segment "NPCFE_RODATA"

asm_deref8: .byte "STA NVR0L", $0A, "STX NVR0H", $0A, "LDY #$00", $0A, "LDA (NVR0L),Y", $0A, 0
asm_deref16: .byte "STA NVR0L", $0A, "STX NVR0H", $0A, "LDY #$00", $0A, "LDA (NVR0L),Y", $0A, "STA NVR1L", $0A, "INY", $0A, "LDA (NVR0L),Y", $0A, "TAX", $0A, "LDA NVR1L", $0A, 0
asm_deref_nvr0_8: .byte "LDY #$00", $0A, "LDA (NVR0L),Y", $0A, 0
asm_deref_nvr0_16: .byte "LDY #$00", $0A, "LDA (NVR0L),Y", $0A, "STA NVR1L", $0A, "INY", $0A, "LDA (NVR0L),Y", $0A, "TAX", $0A, "LDA NVR1L", $0A, 0
asm_address_result: .byte "LDA NVR0L", $0A, "LDX NVR0H", $0A, 0
asm_global_addr_low: .byte "LDA #<", 0
asm_global_addr_high: .byte "LDX #>", 0
asm_lda:    .byte "LDA #$", 0
asm_load_byte_ir: .byte ".O2LB ", 0
asm_load_word_ir: .byte ".O2LW ", 0
asm_store_byte_ir: .byte ".O2SB ", 0
asm_store_word_ir: .byte ".O2SW ", 0
asm_store_extend_ir: .byte ".O2SZ ", 0
asm_word_literal_high: .byte $0A, "LDX #$", 0

      .segment "RODATA"
asm_index_high_zero: .byte "LDX #$00", $0A, 0
asm_index_stack_call: .byte "JSR I_ARRAY_INDEX_ADDR_STACK", $0A, ".WORD $", 0
asm_string_index_stack_call: .byte "JSR I_ARRAY_INDEX_ADDR_STACK", $0A, ".WORD $0000,$0001", $0A, 0
asm_index_byte_stack_call: .byte "JSR I_ARRAY_INDEX_BYTE_STACK", $0A, 0
asm_index_word_separator: .byte ",$", 0

      .segment "NPCFE_RODATA"
asm_field_addr_low: .byte "CLC", $0A, "ADC #$", 0
asm_field_addr_high: .byte $0A, "PHA", $0A, "TXA", $0A, "ADC #$", 0
asm_field_addr_end: .byte $0A, "TAX", $0A, "PLA", $0A, 0
asm_field_addr_byte_end: .byte $0A, "BCC :+", $0A, "INX", $0A, ":", $0A, 0
asm_with_push: .byte "JSR P_WITH_PUSH", $0A, 0
asm_with_pop: .byte "JSR P_WITH_POP", $0A, 0
asm_with_addr_head: .byte "LDY __NP_WITH_SP", $0A, "LDA __NP_WITH_STACK-$", 0
asm_with_addr_low: .byte ",Y", $0A, "CLC", $0A, "ADC #$", 0
asm_with_addr_mid: .byte $0A, "STA NVR0L", $0A, "LDA __NP_WITH_STACK-$", 0
asm_with_addr_high: .byte ",Y", $0A, "ADC #$", 0
asm_with_addr_end: .byte $0A, "STA NVR0H", $0A, 0
asm_with_addr_zero_mid: .byte ",Y", $0A, "STA NVR0L", $0A, "LDA __NP_WITH_STACK-$", 0
asm_with_addr_zero_end: .byte ",Y", $0A, "STA NVR0H", $0A, 0
asm_string_copy_save: .byte "STA NVR2L", $0A, "STX NVR2H", $0A, 0
asm_load_saved_nvr2: .byte "LDA NVR2L", $0A, "LDX NVR2H", $0A, 0
asm_block_copy_call: .byte "JSR I_P_BLOCK_COPY", $0A, ".WORD $", 0
asm_string_copy_call: .byte "LDA NVR2L", $0A, "LDX NVR2H", $0A, "JSR I_P_STRING_COPY", $0A, ".BYTE $", 0
asm_address_to_nvr0: .byte "STA NVR0L", $0A, "STX NVR0H", $0A, 0
asm_frame_pointer_to_nvr0: .byte "LDY #$00", $0A, "LDA (NVR0L),Y", $0A, "STA NVR3L", $0A, "INY", $0A, "LDA (NVR0L),Y", $0A, "STA NVR0H", $0A, "LDA NVR3L", $0A, "STA NVR0L", $0A, 0
asm_stack_address_to_nvr0: .byte "PLA", $0A, "STA NVR0L", $0A, "PLX", $0A, "STX NVR0H", $0A, 0
asm_stack_address_store_byte: .byte "STA NVR1L", $0A, "PLA", $0A, "STA NVR0L", $0A, "PLX", $0A, "STX NVR0H", $0A, "LDY #$00", $0A, "LDA NVR1L", $0A, "STA (NVR0L),Y", $0A, 0
asm_stack_address_store_word: .byte "STA NVR1L", $0A, "STX NVR1H", $0A, "PLA", $0A, "STA NVR0L", $0A, "PLX", $0A, "STX NVR0H", $0A, "LDY #$00", $0A, "LDA NVR1L", $0A, "STA (NVR0L),Y", $0A, "INY", $0A, "LDA NVR1H", $0A, "STA (NVR0L),Y", $0A, 0
asm_stack_address_string_save: .byte "STA NVR2L", $0A, "STX NVR2H", $0A, "PLA", $0A, "STA NVR0L", $0A, "PLX", $0A, "STX NVR0H", $0A, 0
asm_real_save_byte: .byte "STA NVR2L", $0A, "STZ NVR2H", $0A, 0
asm_real_save_word: .byte "STA NVR2L", $0A, "STX NVR2H", $0A, 0
asm_real_store_integer: .byte "LDY #$00", $0A, "LDA #$00", $0A, "STA (NVR0L),Y", $0A, "INY", $0A, "STA (NVR0L),Y", $0A, "INY", $0A, "LDA NVR2L", $0A, "STA (NVR0L),Y", $0A, "INY", $0A, "LDA NVR2H", $0A, "STA (NVR0L),Y", $0A, 0
asm_real_copy: .byte "LDY #$00", $0A, "LDA (NVR2L),Y", $0A, "STA (NVR0L),Y", $0A, "INY", $0A, "LDA (NVR2L),Y", $0A, "STA (NVR0L),Y", $0A, "INY", $0A, "LDA (NVR2L),Y", $0A, "STA (NVR0L),Y", $0A, "INY", $0A, "LDA (NVR2L),Y", $0A, "STA (NVR0L),Y", $0A, 0
; Compact typed IR consumed by NPO2 before NAS sees the generated source.
; Keeping these records shorter than their baseline lowering also recovers
; scarce resident ROM while the optimizer itself runs from lower RAM.
asm_array_get: .byte ".O2G ", 0
asm_jsr:    .byte "JSR P_WRITE_CHAR", $0A, 0
asm_jsr_write_byte: .byte "JSR P_WRITE_BYTE", $0A, 0
asm_jsr_write_byte_ln: .byte "JSR P_WRITE_BYTE_LN", $0A, 0
asm_jsr_write_word: .byte "JSR P_WRITE_WORD", $0A, 0
asm_jsr_write_format: .byte "JSR P_WRITE_FORMAT", $0A, 0
asm_jsr_write_string: .byte "JSR P_WRITE_STRING", $0A, 0
asm_jsr_write_real: .byte "JSR P_WRITE_REAL", $0A, 0
asm_jsr_write_line: .byte "JSR I_P_WRITE_LINE", $0A, ".BYTE ", 0
asm_line_end: .byte "$00", $0A, 0
asm_rts:    .byte "RTS", $0A, 0
asm_routine_start: .byte ".O2R ", 0
asm_nested_routine_prefix: .byte "__NP_R", 0
asm_global_start: .byte ".GLOBAL ", 0
asm_unit_export_end: .byte " ;@NPUNIT", $0A, 0
asm_routine_return: .byte ".O2E P", $0A, 0
asm_function_entry: .byte ".O2K", $0A, 0
asm_function_entry_wide: .byte ".O2KW", $0A, 0
asm_function_store: .byte ".O2V", $0A, 0
asm_function_store_wide: .byte ".O2VW", $0A, 0
asm_function_return: .byte ".O2E F", $0A, 0
asm_function_return_wide: .byte ".O2E W", $0A, 0
asm_pha:    .byte "PHA", $0A, 0
asm_phx_pha: .byte "PHX", $0A, "PHA", $0A, 0
asm_push_real_value: .byte "STA NVR0L", $0A, "STX NVR0H", $0A, "LDY #$00", $0A, "LDA (NVR0L),Y", $0A, "PHA", $0A, "INY", $0A, "LDA (NVR0L),Y", $0A, "PHA", $0A, "INY", $0A, "LDA (NVR0L),Y", $0A, "PHA", $0A, "INY", $0A, "LDA (NVR0L),Y", $0A, "PHA", $0A, 0
asm_default_kbd_push: .byte "LDA #<KBD", $0A, "LDX #>KBD", $0A, "PHX", $0A, "PHA", $0A, 0
asm_pla:    .byte "PLA", $0A, 0
asm_drop_word: .byte "PLA", $0A, "PLX", $0A, 0
asm_drop_four: .byte "PLA", $0A, "PLA", $0A, "PLA", $0A, "PLA", $0A, 0
asm_drop_four_preserve_ax: .byte "PLY", $0A, "PLY", $0A, "PLY", $0A, "PLY", $0A, 0
asm_drop_six: .byte "PLA", $0A, "PLA", $0A, "PLA", $0A, "PLA", $0A, "PLA", $0A, "PLA", $0A, 0
asm_add:    .byte ".O2+", $0A, 0
asm_subtract: .byte ".O2-", $0A, 0
asm_or:     .byte "STA NVR0L", $0A, "PLA", $0A, "ORA NVR0L", $0A, 0
asm_and:    .byte "STA NVR0L", $0A, "PLA", $0A, "AND NVR0L", $0A, 0
asm_set_union: .byte "STA NVR0L", $0A, "PLA", $0A, "ORA NVR0L", $0A, 0
asm_set_difference: .byte "EOR #$FF", $0A, "STA NVR0L", $0A, "PLA", $0A, "AND NVR0L", $0A, 0
asm_set_intersection: .byte "STA NVR0L", $0A, "PLA", $0A, "AND NVR0L", $0A, 0
asm_string_concat: .byte "STA NVR1L", $0A, "STX NVR1H", $0A, "PLA", $0A, "STA NVR0L", $0A, "PLX", $0A, "STX NVR0H", $0A, "LDA NVR1L", $0A, "LDX NVR1H", $0A, "JSR P_STRING_CONCAT", $0A, 0
asm_string_concat_char: .byte "STA NVR1L", $0A, "PLA", $0A, "STA NVR0L", $0A, "PLX", $0A, "STX NVR0H", $0A, "LDA NVR1L", $0A, "JSR P_STRING_CONCAT_CHAR", $0A, 0
asm_char_concat_string: .byte "STA NVR1L", $0A, "STX NVR1H", $0A, "PLA", $0A, "STA NVR0L", $0A, "LDA NVR1L", $0A, "LDX NVR1H", $0A, "JSR P_CHAR_CONCAT_STRING", $0A, 0
asm_string_compare: .byte "STA NVR1L", $0A, "STX NVR1H", $0A, "PLA", $0A, "STA NVR0L", $0A, "PLX", $0A, "STX NVR0H", $0A, "LDA NVR1L", $0A, "LDX NVR1H", $0A, "JSR P_STRING_COMPARE", $0A, 0
asm_char_to_string: .byte "JSR P_CHAR_TO_STRING", $0A, 0
asm_char_string_compare: .byte "STA NVR1L", $0A, "STX NVR1H", $0A, "PLA", $0A, "JSR P_CHAR_TO_STRING", $0A, "STA NVR0L", $0A, "STX NVR0H", $0A, "LDA NVR1L", $0A, "LDX NVR1H", $0A, "JSR P_STRING_COMPARE", $0A, 0
asm_string_copy_part: .byte "JSR P_STRING_COPY_PART", $0A, 0
asm_str_real: .byte "JSR P_STR_REAL", $0A, 0
asm_str_real_precision: .byte "JSR P_STR_REAL_PRECISION", $0A, 0
asm_file_write_block: .byte "STA NVR1L", $0A, "STX NVR1H", $0A, "PLA", $0A, "STA NVR0L", $0A, "PLX", $0A, "STX NVR0H", $0A, "PHX", $0A, "PHA", $0A, "LDA NVR1L", $0A, "LDX NVR1H", $0A, "JSR I_P_FILE_WRITE_BLOCK", $0A, ".WORD $", 0
asm_file_write_char: .byte "STA NVR1L", $0A, "PLA", $0A, "STA NVR0L", $0A, "PLX", $0A, "STX NVR0H", $0A, "PHX", $0A, "PHA", $0A, "LDA NVR1L", $0A, "JSR P_FILE_WRITE_CHAR", $0A, 0
asm_file_write_byte: .byte "STA NVR1L", $0A, "PLA", $0A, "STA NVR0L", $0A, "PLX", $0A, "STX NVR0H", $0A, "PHX", $0A, "PHA", $0A, "LDA NVR1L", $0A, "JSR P_FILE_WRITE_BYTE", $0A, 0
asm_file_write_word: .byte "STA NVR1L", $0A, "STX NVR1H", $0A, "PLA", $0A, "STA NVR0L", $0A, "PLX", $0A, "STX NVR0H", $0A, "PHX", $0A, "PHA", $0A, "LDA NVR1L", $0A, "LDX NVR1H", $0A, "JSR P_FILE_WRITE_WORD", $0A, 0
asm_file_write_string: .byte "STA NVR1L", $0A, "STX NVR1H", $0A, "PLA", $0A, "STA NVR0L", $0A, "PLX", $0A, "STX NVR0H", $0A, "PHX", $0A, "PHA", $0A, "LDA NVR1L", $0A, "LDX NVR1H", $0A, "JSR P_FILE_WRITE_STRING", $0A, 0
asm_file_write_real: .byte "STA NVR1L", $0A, "STX NVR1H", $0A, "PLA", $0A, "STA NVR0L", $0A, "PLX", $0A, "STX NVR0H", $0A, "PHX", $0A, "PHA", $0A, "LDA NVR1L", $0A, "LDX NVR1H", $0A, "JSR P_FILE_WRITE_REAL", $0A, 0
asm_file_write_newline: .byte "PLA", $0A, "STA NVR0L", $0A, "PLX", $0A, "STX NVR0H", $0A, "PHX", $0A, "PHA", $0A, "JSR P_FILE_WRITE_NEWLINE", $0A, 0
asm_set_in8: .byte "JSR P_SET_IN8", $0A, 0
asm_set_in256: .byte "JSR P_SET_IN256", $0A, 0
asm_rhs_word: .byte "STA NVR0L", $0A, "STX NVR0H", $0A, 0
asm_rhs_byte: .byte "STA NVR0L", $0A, "STZ NVR0H", $0A, 0
asm_lhs_word: .byte "PLA", $0A, "PLX", $0A, 0
asm_lhs_byte: .byte "PLA", $0A, "LDX #$00", $0A, 0
asm_add_word: .byte ".O2A", $0A, 0
asm_subtract_word: .byte ".O2S", $0A, 0
asm_negate_byte: .byte "EOR #$FF", $0A, "CLC", $0A, "ADC #$01", $0A, 0
asm_negate_word: .byte "EOR #$FF", $0A, "CLC", $0A, "ADC #$01", $0A, "STA NVR0L", $0A, "TXA", $0A, "EOR #$FF", $0A, "ADC #$00", $0A, "TAX", $0A, "LDA NVR0L", $0A, 0
asm_negate_real: .byte "JSR P_REAL_NEGATE", $0A, 0
asm_real_from_byte: .byte "JSR P_REAL_FROM_BYTE", $0A, 0
asm_real_from_word: .byte "JSR P_REAL_FROM_WORD", $0A, 0
asm_real_add: .byte "STA NVR2L", $0A, "STX NVR2H", $0A, "PLA", $0A, "STA NVR1H", $0A, "PLA", $0A, "STA NVR1L", $0A, "PLA", $0A, "STA NVR0H", $0A, "PLA", $0A, "STA NVR0L", $0A, "LDA NVR2L", $0A, "LDX NVR2H", $0A, "JSR P_REAL_ADD", $0A, 0
asm_real_subtract: .byte "STA NVR2L", $0A, "STX NVR2H", $0A, "PLA", $0A, "STA NVR1H", $0A, "PLA", $0A, "STA NVR1L", $0A, "PLA", $0A, "STA NVR0H", $0A, "PLA", $0A, "STA NVR0L", $0A, "LDA NVR2L", $0A, "LDX NVR2H", $0A, "JSR P_REAL_SUB", $0A, 0
asm_real_compare: .byte "STA NVR2L", $0A, "STX NVR2H", $0A, "PLA", $0A, "STA NVR1H", $0A, "PLA", $0A, "STA NVR1L", $0A, "PLA", $0A, "STA NVR0H", $0A, "PLA", $0A, "STA NVR0L", $0A, "LDA NVR2L", $0A, "LDX NVR2H", $0A, "JSR P_REAL_CMP", $0A, 0
asm_multiply_word: .byte "JSR P_MULW", $0A, 0
asm_divide_word: .byte "JSR P_DIVW", $0A, 0
asm_modulo_word: .byte "JSR P_MODW", $0A, 0
asm_real_multiply: .byte "STA NVR2L", $0A, "STX NVR2H", $0A, "PLA", $0A, "STA NVR1H", $0A, "PLA", $0A, "STA NVR1L", $0A, "PLA", $0A, "STA NVR0H", $0A, "PLA", $0A, "STA NVR0L", $0A, "LDA NVR2L", $0A, "LDX NVR2H", $0A, "JSR P_REAL_MUL", $0A, 0
asm_word_multiply_real: .byte "STA NVR1L", $0A, "STX NVR1H", $0A, "PLA", $0A, "STA NVR0L", $0A, "PLX", $0A, "STX NVR0H", $0A, "LDA NVR1L", $0A, "LDX NVR1H", $0A, "JSR P_REAL_MUL_WORD_LEFT", $0A, 0
asm_byte_multiply_real: .byte "STA NVR1L", $0A, "STX NVR1H", $0A, "PLA", $0A, "STA NVR0L", $0A, "STZ NVR0H", $0A, "LDA NVR1L", $0A, "LDX NVR1H", $0A, "JSR P_REAL_MUL_BYTE_LEFT", $0A, 0
asm_real_multiply_word_right: .byte "STA NVR2L", $0A, "STX NVR2H", $0A, "PLA", $0A, "STA NVR1H", $0A, "PLA", $0A, "STA NVR1L", $0A, "PLA", $0A, "STA NVR0H", $0A, "PLA", $0A, "STA NVR0L", $0A, "LDA NVR2L", $0A, "LDX NVR2H", $0A, "JSR P_REAL_MUL_WORD_RIGHT", $0A, 0
asm_real_multiply_byte_right: .byte "STA NVR2L", $0A, "PLA", $0A, "STA NVR1H", $0A, "PLA", $0A, "STA NVR1L", $0A, "PLA", $0A, "STA NVR0H", $0A, "PLA", $0A, "STA NVR0L", $0A, "LDA NVR2L", $0A, "JSR P_REAL_MUL_BYTE_RIGHT", $0A, 0
asm_real_divide: .byte "STA NVR2L", $0A, "STX NVR2H", $0A, "PLA", $0A, "STA NVR1H", $0A, "PLA", $0A, "STA NVR1L", $0A, "PLA", $0A, "STA NVR0H", $0A, "PLA", $0A, "STA NVR0L", $0A, "LDA NVR2L", $0A, "LDX NVR2H", $0A, "JSR P_REAL_DIV", $0A, 0
asm_real_divide_word: .byte "STA NVR2L", $0A, "STX NVR2H", $0A, "PLA", $0A, "STA NVR1H", $0A, "PLA", $0A, "STA NVR1L", $0A, "PLA", $0A, "STA NVR0H", $0A, "PLA", $0A, "STA NVR0L", $0A, "LDA NVR2L", $0A, "LDX NVR2H", $0A, "JSR P_REAL_DIV_WORD", $0A, 0
asm_real_divide_byte: .byte "STA NVR2L", $0A, "PLA", $0A, "STA NVR1H", $0A, "PLA", $0A, "STA NVR1L", $0A, "PLA", $0A, "STA NVR0H", $0A, "PLA", $0A, "STA NVR0L", $0A, "LDA NVR2L", $0A, "JSR P_REAL_DIV_BYTE", $0A, 0
asm_word_divide_real: .byte "STA NVR1L", $0A, "STX NVR1H", $0A, "PLA", $0A, "STA NVR0L", $0A, "PLX", $0A, "STX NVR0H", $0A, "LDA NVR1L", $0A, "LDX NVR1H", $0A, "JSR P_WORD_DIV_REAL", $0A, 0
asm_byte_divide_real: .byte "STA NVR1L", $0A, "STX NVR1H", $0A, "PLA", $0A, "STA NVR0L", $0A, "STZ NVR0H", $0A, "LDA NVR1L", $0A, "LDX NVR1H", $0A, "JSR P_BYTE_DIV_REAL", $0A, 0
asm_ordinal_divide_real: .byte "JSR P_ORDINAL_DIV_REAL", $0A, 0
asm_divide: .byte "STA NVR0L", $0A, "BNE :+", $0A, "PLA", $0A, "STP", $0A, ":", $0A, "PLA", $0A, "LDX #$00", $0A, ":", $0A, "CMP NVR0L", $0A, "BCC :+", $0A, "SEC", $0A, "SBC NVR0L", $0A, "INX", $0A, "BRA :-", $0A, ":", $0A, "TXA", $0A, 0
asm_mod:    .byte "STA NVR0L", $0A, "BNE :+", $0A, "PLA", $0A, "STP", $0A, ":", $0A, "PLA", $0A, ":", $0A, "CMP NVR0L", $0A, "BCC :+", $0A, "SEC", $0A, "SBC NVR0L", $0A, "BRA :-", $0A, ":", $0A, 0
asm_not:    .byte "EOR #$01", $0A, 0
asm_compare_start: .byte ".O2Q", $0A, 0
asm_compare_word_start: .byte ".O2X", $0A, 0
asm_compare_eq: .byte ".O2C E", $0A, 0
asm_compare_ne: .byte ".O2C N", $0A, 0
asm_compare_lt: .byte ".O2C L", $0A, 0
asm_compare_le: .byte ".O2C l", $0A, 0
asm_compare_gt: .byte ".O2C G", $0A, 0
asm_compare_ge: .byte ".O2C g", $0A, 0
asm_case_byte: .byte "CMP #$", 0
asm_case_word_high: .byte "CPX #$", 0
asm_case_word_low: .byte $0A, "BNE :+", $0A, "CMP #$", 0
asm_case_branch: .byte $0A, "BEQ __NP_L", 0
asm_case_word_end: .byte $0A, ":", $0A, 0
compare_suffix_lo: .byte <asm_compare_eq, <asm_compare_ne, <asm_compare_lt, <asm_compare_le, <asm_compare_gt, <asm_compare_ge
compare_suffix_hi: .byte >asm_compare_eq, >asm_compare_ne, >asm_compare_lt, >asm_compare_le, >asm_compare_gt, >asm_compare_ge
asm_label_prefix: .byte "__NP_L", 0
asm_label_end: .byte ":", $0A, 0
asm_branch_false: .byte ".O2F __NP_L", 0
asm_branch_end: .byte $0A, 0
asm_jump: .byte "JMP __NP_L", 0
asm_include_start: .byte ".INCLUDE ", 34, 0
asm_include_text_start: .byte ".INCLUDETEXT ", 34, 0
asm_inc_end: .byte ".INC", 34, $0A, 0
asm_source_end: .byte ".S", 34, $0A, 0
asm_typed_source_end: .byte ".ASM", 34, $0A, 0
asm_pascal_end: .byte ".NPI", 34, $0A, 0
asm_jsr_prefix: .byte ".O2J ", 0
asm_sig_start: .byte ".ASSERT (__S", 0
asm_local_sig_start: .byte "__S", 0
asm_local_pascal_sig_start: .byte "__P", 0
asm_local_sig_end: .byte " = $", 0
asm_pascal_sig_start: .byte ".ASSERT __P", 0
asm_pascal_sig_end: .byte " = $", 0
asm_sig0_end: .byte " & $01) = $00", $0A, 0
asm_sig1_end: .byte " & $01) = $01", $0A, 0
asm_sig2_end: .byte ") = $02", $0A, 0
asm_sig3_end: .byte ") = $03", $0A, 0
asm_const_start: .byte ".ASSERT __C", 0
asm_const_end: .byte " = 1", $0A, "LDA #", 0
asm_external_macro:
      .byte ".IFNDEF __NP_LOAD_EXTERNAL", $0A
      .byte ".MACRO __NP_LOAD_EXTERNAL value, marker", $0A
      .byte ".IFDEF marker", $0A
      .byte "LDA #value", $0A
      .byte ".ELSE", $0A
      .byte "LDA value", $0A
      .byte ".ENDIF", $0A
      .byte ".ENDMACRO", $0A
      .byte ".ENDIF", $0A, 0
asm_external_const_start: .byte "__NP_LOAD_EXTERNAL ", 0
asm_external_const_else: .byte ", __C", 0
sig_suffix_lo: .byte <asm_sig0_end, <asm_sig1_end, <asm_sig2_end, <asm_sig3_end
sig_suffix_hi: .byte >asm_sig0_end, >asm_sig1_end, >asm_sig2_end, >asm_sig3_end

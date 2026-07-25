; Resident NovaPascal compiler: XRAM Pascal source -> textual 65C02 assembly.

      .setcpu "w65c02"
      .include "nptool.inc"
      .include "longbranch.inc"
      .include "libfiles.inc"
      .include "libmemory.inc"
      .include "xram.inc"
      .include "npproj.inc"
      .include "npc_frontend.inc"
      .include "npc_lowering.inc"

      .define p_clear_lib_args nptool_clear_args

      .import source_buf

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
SYM_CONST = $10
P_IDENT_CAP = NP_TYPED_SYMBOL_CAP
P_UNIT_CAP = 16
P_UNIT_NAME_CAP = 16
P_UNIT_STORAGE = P_UNIT_CAP * P_UNIT_NAME_CAP
P_INCLUDE_DEPTH_CAP = 8
P_INCLUDE_COUNT_CAP = P_INCLUDE_DEPTH_CAP
P_SOURCE_NAME_CAP = 32
P_INCLUDE_LOAD_CAP = NPTOOL_ARG_CAP
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
P_ROUTINE_PARAM_CAP = 8
P_INTERFACE_CAP = 16
P_ROUTINE_KIND_MASK = $0F
P_ROUTINE_EXTERNAL = $10
P_ROUTINE_FORWARD = $20
P_ROUTINE_HAS_FRAME = $40
P_ROUTINE_HAS_NESTED = $80
P_CONTROL_CAP = 16
P_CALL_DEPTH_CAP = 8
P_CALL_META_SIZE = 22
P_WORK_TYPE_NAME_LEN = $0100
P_WORK_TYPE_KIND = P_WORK_TYPE_NAME_LEN + P_TYPE_CAP
P_WORK_TYPE_SIZE_L = P_WORK_TYPE_KIND + P_TYPE_CAP
P_WORK_TYPE_SIZE_H = P_WORK_TYPE_SIZE_L + P_TYPE_CAP
P_WORK_TYPE_LOW_L = P_WORK_TYPE_SIZE_H + P_TYPE_CAP
P_WORK_TYPE_LOW_H = P_WORK_TYPE_LOW_L + P_TYPE_CAP
P_WORK_TYPE_HIGH_L = P_WORK_TYPE_LOW_H + P_TYPE_CAP
P_WORK_TYPE_HIGH_H = P_WORK_TYPE_HIGH_L + P_TYPE_CAP
P_WORK_TYPE_ELEMENT = P_WORK_TYPE_HIGH_H + P_TYPE_CAP
P_WORK_FIELD_COUNT = P_WORK_TYPE_ELEMENT + P_TYPE_CAP
P_WORK_FIELD_NAME_USED = P_WORK_FIELD_COUNT + 1
P_WORK_FIELD_NAME_OFFSET = P_WORK_FIELD_NAME_USED + 1
P_WORK_FIELD_NAME_LEN = P_WORK_FIELD_NAME_OFFSET + P_FIELD_CAP
P_WORK_FIELD_TYPE = P_WORK_FIELD_NAME_LEN + P_FIELD_CAP
P_WORK_FIELD_OFFSET_L = P_WORK_FIELD_TYPE + P_FIELD_CAP
P_WORK_FIELD_OFFSET_H = P_WORK_FIELD_OFFSET_L + P_FIELD_CAP
P_WORK_FIELD_SIZE_L = P_WORK_FIELD_OFFSET_H + P_FIELD_CAP
P_WORK_FIELD_SIZE_H = P_WORK_FIELD_SIZE_L + P_FIELD_CAP
P_WORK_DIM_COUNT = P_WORK_FIELD_SIZE_H + P_FIELD_CAP
P_WORK_DIM_LOW_L = P_WORK_DIM_COUNT + 1
P_WORK_DIM_LOW_H = P_WORK_DIM_LOW_L + P_DIM_CAP
P_WORK_DIM_EXTENT_L = P_WORK_DIM_LOW_H + P_DIM_CAP
P_WORK_DIM_EXTENT_H = P_WORK_DIM_EXTENT_L + P_DIM_CAP
P_WORK_DIM_STRIDE_L = P_WORK_DIM_EXTENT_H + P_DIM_CAP
P_WORK_DIM_STRIDE_H = P_WORK_DIM_STRIDE_L + P_DIM_CAP
P_WORK_IDENT = P_WORK_DIM_STRIDE_H + P_DIM_CAP
P_WORK_CALL_STACK = P_WORK_IDENT + P_IDENT_CAP
P_WORK_UNIT = P_WORK_CALL_STACK + P_CALL_DEPTH_CAP * 5
P_WORK_SAVED_IDENT = P_WORK_UNIT + P_UNIT_CAP * 2
P_WORK_CALL_IDENT = P_WORK_SAVED_IDENT + P_IDENT_CAP
P_WORK_COMPILED_UNIT = P_WORK_CALL_IDENT + P_IDENT_CAP
P_WORK_ARRAY_SYMBOLS = P_WORK_COMPILED_UNIT + P_IDENT_CAP
P_WORK_OPERATORS = P_WORK_ARRAY_SYMBOLS + 8
P_WORK_CONTROLS = P_WORK_OPERATORS + 16
P_WORK_INCLUDES = P_WORK_CONTROLS + P_CONTROL_CAP * 9
P_WORK_TYPE_DECL_NAME = P_WORK_INCLUDES + P_INCLUDE_DEPTH_CAP * 13
P_WORK_CONTEXTS = P_WORK_TYPE_DECL_NAME + P_TYPE_NAME_CAP
P_WORK_ROUTINES = P_WORK_CONTEXTS + P_ROUTINE_DEPTH_CAP * 14
P_WORK_ROUTINE_PARAMS = P_WORK_ROUTINES + P_ROUTINE_CAP * 5
P_WORK_WITH_TYPES = P_WORK_ROUTINE_PARAMS + P_ROUTINE_PARAM_CAP
P_WORK_END = P_WORK_WITH_TYPES + P_WITH_CAP
.assert P_WORK_END <= $1000, error, "NPC scratch exceeds the resident shell buffer"
CONTROL_IF     = 1
CONTROL_WHILE  = 2
CONTROL_REPEAT = 3
CONTROL_FOR    = 4
CONTROL_CASE   = 5
P_STATE_SYMBOL_OFFSET = 0
P_STATE_CONST_NAME_OFFSET = P_STATE_SYMBOL_OFFSET + SYMBOL_CAP * SYMBOL_SIZE
P_STATE_CONST_META_OFFSET = P_STATE_CONST_NAME_OFFSET + P_CONST_CAP * P_CONST_NAME_CAP
P_STATE_TYPE_NAME_OFFSET = P_STATE_CONST_META_OFFSET + P_CONST_CAP * 4
P_STATE_ROUTINE_WIDTH_OFFSET = P_STATE_TYPE_NAME_OFFSET + P_TYPE_CAP * P_TYPE_NAME_CAP
P_STATE_ROUTINE_NAME_META_OFFSET = P_STATE_ROUTINE_WIDTH_OFFSET + P_ROUTINE_CAP
P_STATE_INTERFACE_META_OFFSET = P_STATE_ROUTINE_NAME_META_OFFSET + P_ROUTINE_CAP * 4
P_STATE_INTERFACE_PARAM_OFFSET = P_STATE_INTERFACE_META_OFFSET + P_INTERFACE_CAP * 8
P_STATE_CALL_NAME_OFFSET = P_STATE_INTERFACE_PARAM_OFFSET + P_INTERFACE_CAP * P_ROUTINE_PARAM_CAP
P_STATE_CALL_META_OFFSET = P_STATE_CALL_NAME_OFFSET + P_CALL_DEPTH_CAP * P_IDENT_CAP
P_STATE_ROUTINE_PARAM_OFFSET = P_STATE_CALL_META_OFFSET + P_CALL_DEPTH_CAP * P_CALL_META_SIZE
P_STATE_CONTEXT_NAME_OFFSET = P_STATE_ROUTINE_PARAM_OFFSET + P_ROUTINE_CAP * P_ROUTINE_PARAM_CAP
P_STATE_UNIT_NAME_OFFSET = P_STATE_CONTEXT_NAME_OFFSET + P_ROUTINE_DEPTH_CAP * P_IDENT_CAP
P_STATE_TYPE_PARAM_OFFSET = P_STATE_UNIT_NAME_OFFSET + P_UNIT_STORAGE
P_STATE_FIELD_NAME_OFFSET = P_STATE_TYPE_PARAM_OFFSET + P_TYPE_CAP * P_ROUTINE_PARAM_CAP
P_STATE_INCLUDE_NAME_OFFSET = P_STATE_FIELD_NAME_OFFSET + P_FIELD_NAME_POOL_CAP
P_STATE_ASSIGN_NAME_OFFSET = P_STATE_INCLUDE_NAME_OFFSET + P_INCLUDE_DEPTH_CAP * P_SOURCE_NAME_CAP
; Conditional-compilation symbols. A zero first byte marks a free slot, so the
; pool needs no separate count and survives in the same allocator-backed state.
P_COND_CAP = 8
P_STATE_COND_NAME_OFFSET = P_STATE_ASSIGN_NAME_OFFSET + P_IDENT_CAP
P_STATE_SIZE = P_STATE_COND_NAME_OFFSET + P_COND_CAP * P_IDENT_CAP
TYPE_KIND_ORDINAL = 1
TYPE_KIND_STRING = 2
TYPE_KIND_SET = 3
TYPE_KIND_RECORD = 4
TYPE_KIND_ARRAY = 5
TYPE_KIND_REAL = 6
TYPE_KIND_FILE = 7
TYPE_KIND_POINTER = 8
TYPE_KIND_PROCEDURE = 9
TYPE_KIND_FUNCTION = 10
TYPE_KIND_OPEN_ARRAY = 11
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
TYPE_INTEGER       = 10
TYPE_POINTER       = 11
TYPE_NIL           = 12
TYPE_LONGINT       = 13

SYSFN_SAME = 1
SYSFN_REAL = 2
SYSFN_WORD = 3
; Takes an ordinal and returns Word without the Real normalization the other
; kinds apply to their argument.
SYSFN_ORDINAL = 4
; Takes a file variable by reference and returns Word; the argument is already
; an address, so it must not be widened the way an ordinal would be.
SYSFN_FILE = 5
; Widens an ordinal argument to a word like SYSFN_ORDINAL, but yields a Byte.
SYSFN_BYTE = 6

INTRINSIC_LOW    = 0
INTRINSIC_HIGH   = 1
INTRINSIC_SIZEOF = 2

INTRINSIC_SPECIAL_NONE    = 0
INTRINSIC_SPECIAL_BOOLEAN = 1
INTRINSIC_SPECIAL_INTEGER = 2
INTRINSIC_SPECIAL_STRING  = 3
INTRINSIC_SPECIAL_OPEN_ARRAY = 4

WRITE_BYTE   = 1
WRITE_CHAR   = 2
WRITE_WORD   = 3
WRITE_STRING = 4
WRITE_REAL   = 5
WRITE_FILE   = 6
WRITE_BLOCK  = 7
WRITE_UWORD  = 8
WRITE_LONGINT = 9

PASCAL_OK         = 0
PASCAL_ERR_SYNTAX = 1
PASCAL_ERR_OUTPUT = 2
PASCAL_ERR_INCLUDE = 3
PASCAL_ERR_MEMORY = 4

P_CHECK_RANGE    = $01
P_CHECK_OVERFLOW = $02
P_CHECK_IO       = $04
P_CHECK_STACK    = $08

      .segment "ZEROPAGE"
p_src:       .res 3
p_left:      .res 2
p_word:      .res 2
p_expected:  .res 1
p_char:      .res 1

      .segment "NPCFE_WORK"
npcfe_work_start:
source_len:        .res 2
source_xaddr:      .res 3
generated_asm_len: .res 3
p_state_xaddr:     .res 3
p_state_allocated: .res 1
p_state_offset:    .res 2
p_output_file_id:  .res 2
p_output_open:     .res 1
p_output_used:     .res 2
p_dependency_file_id:.res 2
p_dependency_open: .res 1
; The resident shell is suspended while NPC runs, and source text is streamed
; from allocator-owned XRAM. Reuse its idle file buffer for output staging.
pascal_error:      .res 1
p_line:            .res 2
p_column:          .res 2
p_decimal:         .res 4
p_remainder:       .res 4
p_digit:           .res 1
p_started:         .res 1
p_string_len:      .res 1
p_ident             = source_buf + P_WORK_IDENT
p_ident_len:       .res 1
p_saved_ident_len: .res 1
p_call_ident_len:  .res 1
p_ident_hash:      .res 3
p_saved_hash:      .res 3
p_call_hash:       .res 3
p_assignment_ident_len:.res 1
p_assignment_hash: .res 3
p_in_argument:     .res 1
p_statement_call_saved:.res 1
p_call_stack_len    = source_buf + P_WORK_CALL_STACK
p_call_stack_hash0  = p_call_stack_len + P_CALL_DEPTH_CAP
p_call_stack_hash1  = p_call_stack_hash0 + P_CALL_DEPTH_CAP
p_call_stack_hash2  = p_call_stack_hash1 + P_CALL_DEPTH_CAP
p_call_stack_argument_bytes = p_call_stack_hash2 + P_CALL_DEPTH_CAP
p_argument_count:  .res 1
p_argument_bytes:  .res 1
p_call_arity:      .res 1
p_statement_refmask:.res 1
p_statement_widthmask:.res 1
p_statement_typed: .res 1
p_statement_routine:.res 1
p_indirect_call:   .res 1
p_call_type:       .res 1
p_call_type_row:   .res 1
p_statement_force_stack:.res 1
p_statement_io_kind:.res 1
p_statement_line:   .res 2
p_insert_call:      .res 1
p_insert_capacity:  .res 1
p_read_call:       .res 1
p_read_size:       .res 2
p_val_call:        .res 1
p_val_size:        .res 1
p_scan_depth:      .res 1
p_scan_quote:      .res 1
p_unit_count:      .res 1
p_unit_len          = source_buf + P_WORK_UNIT
p_unit_kind         = p_unit_len + P_UNIT_CAP
p_unit_iter:       .res 1
p_emit_error:      .res 1
p_check_flags:     .res 1
p_external_macro_emitted: .res 1
p_heap_include_emitted:.res 1
p_saved_ident       = source_buf + P_WORK_SAVED_IDENT
p_call_ident        = source_buf + P_WORK_CALL_IDENT
p_compiled_unit_name_len:.res 1
p_compiled_unit_name = source_buf + P_WORK_COMPILED_UNIT
p_label_namespace: .res 1
p_label_namespace_compact:.res 1
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
p_array_symbols     = source_buf + P_WORK_ARRAY_SYMBOLS
p_expr_type:       .res 1
p_function_arg_type:.res 1
p_function_result_type:.res 1
p_intrinsic_kind:  .res 1
p_intrinsic_special:.res 1
p_inc_operator:    .res 1
p_left_type:       .res 1
p_operator:        .res 1
p_operator_depth:  .res 1
p_operator_stack    = source_buf + P_WORK_OPERATORS
p_operand_types     = p_operator_stack + 8
p_label:           .res 2
p_label_saved:     .res 2
p_mark_src:        .res 3
p_mark_left:       .res 2
p_mark_line:       .res 2
p_mark_column:     .res 2
p_mark_include_depth: .res 1
p_mark_overlay:    .res 1
p_mark_name_len:   .res 1
p_function_active: .res 1
p_function_hash:   .res 3
p_compiling_unit:  .res 1
p_in_unit_interface:.res 1
p_importing_interface:.res 1
p_suppress_output: .res 1
p_interface_count: .res 1
p_interface_index: .res 1
p_control_depth:   .res 1
p_control_kind      = source_buf + P_WORK_CONTROLS
p_control_a_lo      = p_control_kind + P_CONTROL_CAP
p_control_a_hi      = p_control_a_lo + P_CONTROL_CAP
p_control_b_lo      = p_control_a_hi + P_CONTROL_CAP
p_control_b_hi      = p_control_b_lo + P_CONTROL_CAP
p_control_symbol    = p_control_b_hi + P_CONTROL_CAP
p_case_body_lo      = p_control_symbol + P_CONTROL_CAP
p_case_body_hi      = p_case_body_lo + P_CONTROL_CAP
p_case_word         = p_case_body_hi + P_CONTROL_CAP
p_window_m:        .res 1
p_window_h:        .res 1
p_window_valid:    .res 1
p_source_error:    .res 1
p_source_detail:   .res 1
p_include_depth:   .res 1
p_include_count:   .res 1
p_include_name_len:.res 1
p_include_leaf_len:.res 1
p_include_path_index:.res 1
p_current_name_len:.res 1
p_include_xaddr:   .res 3
p_include_len:     .res 2
p_include_stack_src_l = source_buf + P_WORK_INCLUDES
p_include_stack_src_m = p_include_stack_src_l + P_INCLUDE_DEPTH_CAP
p_include_stack_src_h = p_include_stack_src_m + P_INCLUDE_DEPTH_CAP
p_include_stack_left_l = p_include_stack_src_h + P_INCLUDE_DEPTH_CAP
p_include_stack_left_h = p_include_stack_left_l + P_INCLUDE_DEPTH_CAP
p_include_stack_line_l = p_include_stack_left_h + P_INCLUDE_DEPTH_CAP
p_include_stack_line_h = p_include_stack_line_l + P_INCLUDE_DEPTH_CAP
p_include_stack_col_l = p_include_stack_line_h + P_INCLUDE_DEPTH_CAP
p_include_stack_col_h = p_include_stack_col_l + P_INCLUDE_DEPTH_CAP
p_include_stack_name_len = p_include_stack_col_h + P_INCLUDE_DEPTH_CAP
p_include_alloc_l   = p_include_stack_name_len + P_INCLUDE_DEPTH_CAP
p_include_alloc_m   = p_include_alloc_l + P_INCLUDE_COUNT_CAP
p_include_alloc_h   = p_include_alloc_m + P_INCLUDE_COUNT_CAP
p_release_error:   .res 1
p_frontend_status: .res 1
p_const_count:      .res 1
p_const_iter:       .res 1
p_const_sign:       .res 1
p_type_count:       .res 1
p_type_iter:        .res 1
p_type_decl_len:    .res 1
p_type_outer_decl_len:.res 1
p_type_decl_name    = source_buf + P_WORK_TYPE_DECL_NAME
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
p_context_scope_base = source_buf + P_WORK_CONTEXTS
p_context_param_end = p_context_scope_base + P_ROUTINE_DEPTH_CAP
p_context_param_count = p_context_param_end + P_ROUTINE_DEPTH_CAP
p_context_frame_l   = p_context_param_count + P_ROUTINE_DEPTH_CAP
p_context_frame_h   = p_context_frame_l + P_ROUTINE_DEPTH_CAP
p_context_kind      = p_context_frame_h + P_ROUTINE_DEPTH_CAP
p_context_return_type = p_context_kind + P_ROUTINE_DEPTH_CAP
p_context_function_active = p_context_return_type + P_ROUTINE_DEPTH_CAP
p_context_name_len  = p_context_function_active + P_ROUTINE_DEPTH_CAP
p_context_hash0     = p_context_name_len + P_ROUTINE_DEPTH_CAP
p_context_hash1     = p_context_hash0 + P_ROUTINE_DEPTH_CAP
p_context_hash2     = p_context_hash1 + P_ROUTINE_DEPTH_CAP
p_context_exit_l    = p_context_hash2 + P_ROUTINE_DEPTH_CAP
p_context_exit_h    = p_context_exit_l + P_ROUTINE_DEPTH_CAP
p_routine_count:    .res 1
p_current_routine:  .res 1
p_exit_label:       .res 2
p_current_overlay:  .res 1
p_routine_iter:     .res 1
p_routine_owner     = source_buf + P_WORK_ROUTINES
p_routine_kind_table = p_routine_owner + P_ROUTINE_CAP
p_routine_return    = p_routine_kind_table + P_ROUTINE_CAP
p_routine_params    = p_routine_return + P_ROUTINE_CAP
p_routine_refmask   = p_routine_params + P_ROUTINE_CAP
p_routine_param_refmask:.res 1
p_routine_param_widthmask:.res 1
p_routine_param_types = source_buf + P_WORK_ROUTINE_PARAMS
p_reusing_forward: .res 1
p_frame_size:      .res 2
p_param_flags:     .res 1
p_param_openmask:  .res 1
p_formal_type:     .res 1
p_for_direction:   .res 1
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
p_with_type          = source_buf + P_WORK_WITH_TYPES
p_with_slot:        .res 1
p_target_with:      .res 1
p_target_with_slot: .res 1
p_function_indirect:.res 1
p_function_call_type_row:.res 1
p_open_expected:   .res 1
p_open_element:    .res 1

      .segment "NPCFE_WORK"
p_cond_depth:       .res 1
p_cond_skip:        .res 1
p_set_start:        .res 1
p_set_end:          .res 1
p_set_large:        .res 1
p_set_bits:         .res 32
p_mark_name:        .res P_SOURCE_NAME_CAP
p_include_name:     .res P_INCLUDE_LOAD_CAP
p_include_leaf:     .res P_SOURCE_NAME_CAP
p_current_name:     .res P_SOURCE_NAME_CAP

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
npcfe_work_end:
.assert npcfe_work_end - npcfe_work_start <= NPCFE_WORK_CAP, error, "NPC frontend work exceeds the shell scratch tail"

      .segment "NPCFE_BSS"
; The shell is suspended while NPC runs. Its file buffer is therefore the
; compiler's low-RAM scratch arena; bytes $000-$0FF stage generated output.
p_type_name_len     = source_buf + P_WORK_TYPE_NAME_LEN
p_type_kind         = source_buf + P_WORK_TYPE_KIND
p_type_size_l       = source_buf + P_WORK_TYPE_SIZE_L
p_type_size_h       = source_buf + P_WORK_TYPE_SIZE_H
p_type_low_l        = source_buf + P_WORK_TYPE_LOW_L
p_type_low_h        = source_buf + P_WORK_TYPE_LOW_H
p_type_high_l       = source_buf + P_WORK_TYPE_HIGH_L
p_type_high_h       = source_buf + P_WORK_TYPE_HIGH_H
p_type_element      = source_buf + P_WORK_TYPE_ELEMENT
p_field_count       = source_buf + P_WORK_FIELD_COUNT
p_field_name_used   = source_buf + P_WORK_FIELD_NAME_USED
p_field_name_offset = source_buf + P_WORK_FIELD_NAME_OFFSET
p_field_name_len    = source_buf + P_WORK_FIELD_NAME_LEN
p_field_type        = source_buf + P_WORK_FIELD_TYPE
p_field_offset_l    = source_buf + P_WORK_FIELD_OFFSET_L
p_field_offset_h    = source_buf + P_WORK_FIELD_OFFSET_H
p_field_size_l      = source_buf + P_WORK_FIELD_SIZE_L
p_field_size_h      = source_buf + P_WORK_FIELD_SIZE_H
p_dim_count         = source_buf + P_WORK_DIM_COUNT
p_dim_low_l         = source_buf + P_WORK_DIM_LOW_L
p_dim_low_h         = source_buf + P_WORK_DIM_LOW_H
p_dim_extent_l      = source_buf + P_WORK_DIM_EXTENT_L
p_dim_extent_h      = source_buf + P_WORK_DIM_EXTENT_H
p_dim_stride_l      = source_buf + P_WORK_DIM_STRIDE_L
p_dim_stride_h      = source_buf + P_WORK_DIM_STRIDE_H

      .segment "NPCFE_CODE"
      .export npcfe_main
      .export p_control_depth
      .export p_emit_error

npcfe_main:
      LDX   #0
      LDA   #0
@clear_work:
      STA   source_buf+NPCFE_WORK_OFFSET,X
      STA   source_buf+NPCFE_WORK_OFFSET+$0100,X
      INX
      BNE   @clear_work
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
      JSR   p_finalize_dependencies
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
      LDA   #P_CHECK_IO | P_CHECK_STACK
      STA   p_check_flags
      STZ   p_compiled_unit_name_len
      STZ   p_external_macro_emitted
      STZ   p_heap_include_emitted
      STZ   p_in_argument
      STZ   p_symbol_count
      STZ   p_label
      STZ   p_label+1
      STZ   p_function_active
      STZ   p_compiling_unit
      STZ   p_in_unit_interface
      STZ   p_importing_interface
      STZ   p_suppress_output
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
      STZ   p_exit_label
      STZ   p_exit_label+1
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
      JSR   p_open_dependencies
      long_bne p_output_error
      JSR   p_open_output
      long_bne p_output_error
      LDA   #<asm_import
      LDX   #>asm_import
      JSR   p_emit_ax_text
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
      JSR   p_capture_identifier
      BCC   :+
      JMP   p_syntax_error
:
      JSR   p_save_compiled_name
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
      LDX   #>asm_code
      JSR   p_emit_ax_text
      long_bcs p_output_error
      LDA   #<asm_main_label
      LDX   #>asm_main_label
      JSR   p_emit_ax_text
      long_bcs p_output_error
@parse_body:
      JSR   p_new_exit_label
      JSR   p_emit_unit_initializers
      long_bcs p_output_error
      JSR   p_parse_statement_list
      BCS   p_program_parse_fail
      LDA   #'.'
      JSR   p_expect_char
      long_bcs p_syntax_error
      JSR   p_skip_ws
      JSR   p_peek
      long_bcs p_syntax_error

      JSR   p_emit_exit_label
      long_bcs p_output_error
      LDA   NPP_PLAN_BASE+NPP_PLAN_TARGET
      CMP   #NPP_TARGET_OVERLAY
      BNE   @emit_rts
      LDA   #<asm_overlay_return
      LDX   #>asm_overlay_return
      JSR   p_emit_ax_text
      long_bcs p_output_error
@emit_rts:
      LDA   #<asm_rts
      LDX   #>asm_rts
      JSR   p_emit_ax_text
      long_bcs p_output_error
p_finish_compile:
      JSR   p_emit_unit_sources
      long_bcs p_output_error
      JSR   p_emit_heap_source
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
      BCC   @fail
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
;             {const/type/var/procedure/function declarations}
;             IMPLEMENTATION [USES ... ;]
;             {procedure/function definitions} END .
;
; Interface and implementation routine signatures must match in order. The
; compact signature retains the exact result and parameter types plus VAR and
; word-width masks, so a bad unit cannot silently corrupt its caller's stack.
p_compile_unit:
      INC   p_compiling_unit
      INC   p_in_unit_interface
      JSR   p_parse_unit_interface
      long_bcs p_syntax_error
      DEC   p_in_unit_interface
      JSR   p_capture_identifier
      long_bcs p_syntax_error
      LDA   #<kw_uses
      LDX   #>kw_uses
      JSR   p_ident_is
      BCC   @implementation_item
      JSR   p_parse_uses
      long_bcs @parse_fail
@implementation_item:
      LDA   #<kw_end
      LDX   #>kw_end
      JSR   p_ident_is
      BCS   @empty_initialization
      LDA   #<kw_begin
      LDX   #>kw_begin
      JSR   p_ident_is
      BCS   @initialization
      JSR   p_is_routine_keyword
      long_bcc p_syntax_error
      LDA   #<asm_code
      LDX   #>asm_code
      JSR   p_emit_ax_text
      long_bcs p_output_error
      JSR   p_parse_routine_declarations
      long_bcs @parse_fail
      LDA   #<kw_begin
      LDX   #>kw_begin
      JSR   p_ident_is
      BCS   @initialization
      LDA   #<kw_end
      LDX   #>kw_end
      JSR   p_ident_is
      long_bcc p_syntax_error
@empty_initialization:
      LDA   p_interface_index
      CMP   p_interface_count
      long_bne p_syntax_error
      JSR   p_new_exit_label
      JSR   p_emit_unit_initializer_label
      long_bcs p_output_error
      JSR   p_emit_exit_label
      long_bcs p_output_error
      LDA   #<asm_routine_return
      LDX   #>asm_routine_return
      JSR   p_emit_ax_text
      long_bcs p_output_error
      BRA   @unit_end
@initialization:
      LDA   p_interface_index
      CMP   p_interface_count
      long_bne p_syntax_error
      JSR   p_new_exit_label
      JSR   p_emit_unit_initializer_label
      long_bcs p_output_error
      JSR   p_parse_statement_list
      long_bcs @parse_fail
      JSR   p_emit_exit_label
      long_bcs p_output_error
      LDA   #<asm_routine_return
      LDX   #>asm_routine_return
      JSR   p_emit_ax_text
      long_bcs p_output_error
@unit_end:
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

; Parse through IMPLEMENTATION and leave it captured. Importers use this same
; path against a unit source file, so declarations have one grammar.
p_parse_unit_interface:
      JSR   p_capture_identifier
      long_bcs @fail
      LDA   p_importing_interface
      BNE   @name_saved
      JSR   p_save_compiled_name
@name_saved:
      LDA   #';'
      JSR   p_expect_char
      long_bcs @fail
      JSR   p_capture_identifier
      long_bcs @fail
      LDA   #<kw_interface
      LDX   #>kw_interface
      JSR   p_ident_is
      long_bcc @fail
      JSR   p_capture_identifier
      long_bcs @fail
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
      BCS   @done
      LDA   #<kw_const
      LDX   #>kw_const
      JSR   p_ident_is
      BCC   @interface_type
      JSR   p_parse_const_section
      long_bcs @parse_fail
      BRA   @interface_item
@interface_type:
      LDA   #<kw_type
      LDX   #>kw_type
      JSR   p_ident_is
      BCC   @interface_var
      JSR   p_parse_type_section
      long_bcs @parse_fail
      BRA   @interface_item
@interface_var:
      LDA   #<kw_var
      LDX   #>kw_var
      JSR   p_ident_is
      BCC   @interface_routine
      JSR   p_parse_global_var_section
      long_bcs @parse_fail
      BRA   @interface_item
@interface_routine:
      JSR   p_is_routine_keyword
      long_bcc @fail
      JSR   p_parse_interface_routine
      long_bcs @parse_fail
      BRA   @interface_item
@done:
      CLC
      RTS
@parse_fail:
      LDA   p_emit_error
      BNE   @fail
@fail:
      SEC
      RTS

p_save_compiled_name:
      LDA   p_ident_len
      STA   p_compiled_unit_name_len
      LDX   #0
@copy:
      CPX   p_ident_len
      BCS   @done
      LDA   p_ident,X
      STA   p_compiled_unit_name,X
      INX
      BRA   @copy
@done:
      JMP   p_select_label_namespace

p_select_label_namespace:
      STZ   p_label_namespace_compact
      LDA   p_compiling_unit
      BNE   @local_unit
      STZ   p_label_namespace
      INC   p_label_namespace_compact
      RTS
@local_unit:
      STZ   p_unit_iter
@unit:
      LDA   p_unit_iter
      CMP   NPP_PLAN_BASE+NPP_PLAN_LOCAL_COUNT
      BCS   @external_unit
      ASL
      CLC
      ADC   p_unit_iter
      TAX
      LDA   p_ident_hash
      CMP   NPP_PLAN_BASE+NPP_PLAN_LOCAL_HASHES,X
      BNE   @next
      LDA   p_ident_hash+1
      CMP   NPP_PLAN_BASE+NPP_PLAN_LOCAL_HASHES+1,X
      BNE   @next
      LDA   p_ident_hash+2
      CMP   NPP_PLAN_BASE+NPP_PLAN_LOCAL_HASHES+2,X
      BNE   @next
      LDA   p_unit_iter
      INC   A
      STA   p_label_namespace
      INC   p_label_namespace_compact
      RTS
@next:
      INC   p_unit_iter
      BRA   @unit
@external_unit:
      RTS

; Parse one public routine header without creating executable code. The same
; formal-parameter parser is used by its implementation, avoiding a second
; Pascal type grammar at the unit boundary.
p_parse_interface_routine:
      LDA   #<kw_procedure
      LDX   #>kw_procedure
      JSR   p_ident_is
      BCC   @function
      STZ   p_routine_kind
      BRA   @name
@function:
      LDA   #2
      STA   p_routine_kind
@name:
      JSR   p_capture_identifier
      long_bcs @fail
      JSR   p_save_identifier
      LDA   NPP_PLAN_BASE+NPP_PLAN_TARGET
      CMP   #NPP_TARGET_UNIT
      BNE   @visibility_done
      JSR   p_emit_global_saved
      long_bcs @output
@visibility_done:
      LDA   p_symbol_count
      STA   p_scope_symbol_base
      INC   p_scope_level
      STZ   p_routine_param_count
      STZ   p_routine_param_refmask
      STZ   p_routine_param_widthmask
      LDA   #2
      STA   p_frame_size
      STZ   p_frame_size+1
      JSR   p_parse_routine_signature
      long_bcs @restore_fail
      LDA   p_importing_interface
      BEQ   @store
      JSR   p_register_imported_routine
      BRA   @stored
@store:
      JSR   p_store_interface_routine
@stored:
      long_bcs @restore_fail
      LDA   p_scope_symbol_base
      STA   p_symbol_count
      DEC   p_scope_level
      LDA   #';'
      JSR   p_expect_char
      long_bcs @fail
      JSR   p_capture_identifier
      long_bcs @fail
      LDA   #<kw_external
      LDX   #>kw_external
      JSR   p_ident_is
      BCC   @done
      LDA   #';'
      JSR   p_expect_char
      long_bcs @fail
      JSR   p_capture_identifier
@done:
      RTS
@restore_fail:
      LDA   p_scope_symbol_base
      STA   p_symbol_count
      DEC   p_scope_level
@fail:
      SEC
      RTS
@output:
      INC   p_emit_error
      BRA   @restore_fail

; Imported public routines enter the ordinary routine table. Existing call
; lowering then gets the same VAR, width, arity, and result metadata as a
; routine declared in the current source file.
p_register_imported_routine:
      LDA   p_current_routine
      PHA
      LDA   #$FF
      STA   p_current_routine
      JSR   p_routine_add
      BCS   @fail
      LDX   p_current_routine
      LDA   p_routine_kind_table,X
      ORA   #P_ROUTINE_EXTERNAL
      STA   p_routine_kind_table,X
      JSR   p_store_current_routine_signature
      PLA
      STA   p_current_routine
      CLC
      RTS
@fail:
      PLA
      STA   p_current_routine
      SEC
      RTS

; Growth-sensitive routine ABI metadata shares the compiler's allocator-owned
; XRAM state with symbol and type names. X remains the selected table row.
p_routine_width_pointer:
      TXA
      CLC
      ADC   #<P_STATE_ROUTINE_WIDTH_OFFSET
      STA   p_state_offset
      LDA   #>P_STATE_ROUTINE_WIDTH_OFFSET
      ADC   #0
      STA   p_state_offset+1
      JMP   p_state_pointer

p_routine_name_meta_pointer:
      TXA
      ASL
      ASL
      CLC
      ADC   #<P_STATE_ROUTINE_NAME_META_OFFSET
      STA   p_state_offset
      LDA   #>P_STATE_ROUTINE_NAME_META_OFFSET
      ADC   #0
      STA   p_state_offset+1
      JMP   p_state_pointer

p_interface_meta_pointer:
      TXA
      ASL
      ASL
      ASL
      CLC
      ADC   #<P_STATE_INTERFACE_META_OFFSET
      STA   p_state_offset
      LDA   #>P_STATE_INTERFACE_META_OFFSET
      ADC   #0
      STA   p_state_offset+1
      JMP   p_state_pointer

p_interface_param_pointer:
      TXA
      ASL
      ASL
      ASL
      CLC
      ADC   #<P_STATE_INTERFACE_PARAM_OFFSET
      STA   p_state_offset
      LDA   #>P_STATE_INTERFACE_PARAM_OFFSET
      ADC   #0
      STA   p_state_offset+1
      JMP   p_state_pointer

p_routine_param_pointer:
      TXA
      ASL
      ASL
      ASL
      CLC
      ADC   #<P_STATE_ROUTINE_PARAM_OFFSET
      STA   p_state_offset
      LDA   #>P_STATE_ROUTINE_PARAM_OFFSET
      ADC   #0
      STA   p_state_offset+1
      JMP   p_state_pointer

      .segment "NPCFE_CODE"
p_type_param_pointer:
      TXA
      ASL
      ASL
      ASL
      CLC
      ADC   #<P_STATE_TYPE_PARAM_OFFSET
      STA   p_state_offset
      LDA   #>P_STATE_TYPE_PARAM_OFFSET
      ADC   #0
      STA   p_state_offset+1
      JMP   p_state_pointer

p_store_type_signature:
      JSR   p_type_param_pointer
      LDY   #0
@param:
      CPY   p_type_work_low
      BCS   @done
      LDA   p_routine_param_types,Y
      STA   (p_word),Y
      INY
      BRA   @param
@done:
      RTS

p_store_current_routine_signature:
      LDX   p_current_routine
      LDA   p_routine_param_count
      STA   p_routine_params,X
      LDA   p_routine_param_refmask
      STA   p_routine_refmask,X
      JSR   p_routine_width_pointer
      LDY   #0
      LDA   p_routine_param_widthmask
      STA   (p_word),Y
      LDX   p_current_routine
      LDA   p_function_return_type
      STA   p_routine_return,X
      JSR   p_routine_param_pointer
      LDY   #0
@param:
      CPY   p_routine_param_count
      BCS   @done
      LDA   p_routine_param_types,Y
      STA   (p_word),Y
      INY
      BRA   @param
@done:
      CLC
      RTS

p_validate_forward_signature:
      LDX   p_current_routine
      LDA   p_routine_kind_table,X
      AND   #P_ROUTINE_KIND_MASK
      CMP   p_routine_kind
      BNE   @fail
      LDA   p_routine_return,X
      CMP   p_function_return_type
      BNE   @fail
      LDA   p_routine_params,X
      CMP   p_routine_param_count
      BNE   @fail
      LDA   p_routine_refmask,X
      CMP   p_routine_param_refmask
      BNE   @fail
      JSR   p_routine_width_pointer
      LDY   #0
      LDA   (p_word),Y
      CMP   p_routine_param_widthmask
      BNE   @fail
      LDX   p_current_routine
      JSR   p_routine_param_pointer
      LDY   #0
@param:
      CPY   p_routine_param_count
      BCS   @valid
      LDA   (p_word),Y
      CMP   p_routine_param_types,Y
      BNE   @fail
      INY
      BRA   @param
@valid:
      LDX   p_current_routine
      LDA   p_routine_kind_table,X
      AND   #$DF
      STA   p_routine_kind_table,X
      CLC
      RTS
@fail:
      SEC
      RTS

p_store_interface_routine:
      LDX   p_interface_count
      CPX   #P_INTERFACE_CAP
      BCS   @fail
      JSR   p_interface_meta_pointer
      LDY   #0
      LDA   p_routine_kind
      STA   (p_word),Y
      INY
      LDA   p_saved_hash
      STA   (p_word),Y
      INY
      LDA   p_saved_hash+1
      STA   (p_word),Y
      INY
      LDA   p_saved_hash+2
      STA   (p_word),Y
      INY
      LDA   p_function_return_type
      STA   (p_word),Y
      INY
      LDA   p_routine_param_count
      STA   (p_word),Y
      INY
      LDA   p_routine_param_refmask
      STA   (p_word),Y
      INY
      LDA   p_routine_param_widthmask
      STA   (p_word),Y
      LDX   p_interface_count
      JSR   p_interface_param_pointer
      LDY   #0
@param:
      CPY   p_routine_param_count
      BCS   @stored
      LDA   p_routine_param_types,Y
      STA   (p_word),Y
      INY
      BRA   @param
@stored:
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
      JSR   p_interface_meta_pointer
      LDY   #0
      LDA   p_routine_kind
      CMP   (p_word),Y
      BNE   @fail
      INY
      LDA   p_saved_hash
      CMP   (p_word),Y
      BNE   @fail
      INY
      LDA   p_saved_hash+1
      CMP   (p_word),Y
      BNE   @fail
      INY
      LDA   p_saved_hash+2
      CMP   (p_word),Y
      BNE   @fail
      INY
      LDA   p_function_return_type
      CMP   (p_word),Y
      BNE   @fail
      INY
      LDA   p_routine_param_count
      CMP   (p_word),Y
      BNE   @fail
      INY
      LDA   p_routine_param_refmask
      CMP   (p_word),Y
      BNE   @fail
      INY
      LDA   p_routine_param_widthmask
      CMP   (p_word),Y
      BNE   @fail
      LDX   p_interface_index
      JSR   p_interface_param_pointer
      LDY   #0
@param:
      CPY   p_routine_param_count
      BCS   @valid
      LDA   p_routine_param_types,Y
      CMP   (p_word),Y
      BNE   @fail
      INY
      BRA   @param
@valid:
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
      long_bcs @output
      JSR   p_capture_identifier
      long_bcs @fail
@group:
      LDA   p_symbol_count
      STA   p_symbol_group
@name:
      JSR   p_symbol_add
      long_bcs @fail
      JSR   p_skip_ws
      JSR   p_peek
      long_bcc @fail
      CMP   #','
      BNE   @type
      JSR   p_next
      JSR   p_capture_identifier
      long_bcs @fail
      BRA   @name
@type:
      LDA   #':'
      JSR   p_expect_char
      long_bcs @fail
      JSR   p_parse_decl_type
      long_bcs @fail
      ; `absolute <address>` binds the group to fixed storage instead of BSS.
      ; The address parks in the type-work fields, which are spent once the type
      ; is parsed; p_decimal cannot hold it because p_symbol_pointer uses that as
      ; scratch for every name in the group. The declared type goes on the stack
      ; because the constant parser types through p_decl_type.
      STZ   p_type_work_high
      LDA   #<kw_absolute
      LDX   #>kw_absolute
      JSR   p_peek_keyword
      BCC   @allocate
      LDA   #<kw_absolute
      LDX   #>kw_absolute
      JSR   p_expect_ax_word
      long_bcs @fail
      LDA   p_decl_type
      PHA
      LDA   #TYPE_WORD
      STA   p_decl_type
      JSR   p_parse_const_scalar
      PLA
      STA   p_decl_type
      long_bcs @fail
      LDA   p_decimal
      STA   p_type_work_low
      LDA   p_decimal+1
      STA   p_type_work_low+1
      LDA   #$FF
      STA   p_type_work_high
@allocate:
      JSR   p_emit_symbol_group
      BCS   @output
      LDA   #';'
      JSR   p_expect_char
      BCS   @fail
      JSR   p_capture_identifier
      BCS   @fail
      LDA   p_importing_interface
      BEQ   @classify
      LDA   #<kw_external
      LDX   #>kw_external
      JSR   p_ident_is
      BCC   @classify
      LDA   #';'
      JSR   p_expect_char
      BCS   @fail
      JSR   p_capture_identifier
      BCS   @fail
@classify:
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
      LDA   #<kw_implementation
      LDX   #>kw_implementation
      JSR   p_ident_is
      BCS   @done
      JSR   p_is_routine_keyword
      BCS   @done
      JMP   @group
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
      JSR   p_routine_name_meta_pointer
      LDY   #0
      LDA   p_saved_ident_len
      STA   (p_word),Y
      INY
      LDA   p_saved_hash
      STA   (p_word),Y
      INY
      LDA   p_saved_hash+1
      STA   (p_word),Y
      INY
      LDA   p_saved_hash+2
      STA   (p_word),Y
      LDA   p_current_routine
      STA   p_routine_owner,X
      LDA   p_routine_kind
      STA   p_routine_kind_table,X
      LDA   #TYPE_BYTE
      STA   p_routine_return,X
      STZ   p_routine_params,X
      STZ   p_routine_refmask,X
      JSR   p_routine_width_pointer
      LDY   #0
      LDA   #0
      STA   (p_word),Y
      STX   p_current_routine
      INC   p_routine_count
      CLC
@fail:
      RTS

; A declaration may reuse only a pending FORWARD in the same lexical scope.
; Carry set returns the existing row in X; carry clear means a new name.
p_routine_find_declaration:
      LDX   p_routine_count
@candidate:
      DEX
      BMI   @missing
      LDA   p_routine_owner,X
      CMP   p_current_routine
      BNE   @candidate
      JSR   p_routine_name_meta_pointer
      LDY   #0
      LDA   (p_word),Y
      CMP   p_saved_ident_len
      BNE   @candidate
      INY
      LDA   (p_word),Y
      CMP   p_saved_hash
      BNE   @candidate
      INY
      LDA   (p_word),Y
      CMP   p_saved_hash+1
      BNE   @candidate
      INY
      LDA   (p_word),Y
      CMP   p_saved_hash+2
      BNE   @candidate
      SEC
      RTS
@missing:
      CLC
      RTS

p_check_forwards_resolved:
      LDX   p_routine_count
@routine:
      DEX
      BMI   @done
      LDA   p_routine_owner,X
      CMP   p_current_routine
      BNE   @routine
      LDA   p_routine_kind_table,X
      AND   #P_ROUTINE_FORWARD
      BEQ   @routine
      SEC
      RTS
@done:
      CLC
      RTS

; Find the innermost declared routine matching p_call_hash. A candidate is
; visible when it is the current routine, is owned by the current routine, or
; belongs to one of the current routine's lexical ancestors.
p_routine_find_call:
      LDX   p_routine_count
@candidate:
      DEX
      BMI   @missing
      JSR   p_routine_name_meta_pointer
      LDY   #0
      LDA   (p_word),Y
      CMP   p_call_ident_len
      BNE   @candidate
      INY
      LDA   (p_word),Y
      CMP   p_call_hash
      BNE   @candidate
      INY
      LDA   (p_word),Y
      CMP   p_call_hash+1
      BNE   @candidate
      INY
      LDA   (p_word),Y
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
      STZ   p_reusing_forward
      JSR   p_routine_find_declaration
      BCC   @new_routine
      LDA   p_routine_kind_table,X
      AND   #P_ROUTINE_FORWARD
      long_beq @fail
      STX   p_current_routine
      INC   p_reusing_forward
      BRA   @new_scope
@new_routine:
      JSR   p_routine_add
      long_bcs @fail
@new_scope:
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
      BRA   @signature
@save_function:
      LDX   #2
@hash:
      LDA   p_saved_hash,X
      STA   p_function_hash,X
      DEX
      BPL   @hash
      LDA   #1
      STA   p_function_active
@signature:
      JSR   p_parse_routine_signature
      long_bcs @fail
      LDA   p_routine_kind
      BEQ   @header_end
      JSR   p_function_result_wide
      BCC   @header_end
      JSR   p_reserve_wide_function_result
      long_bcs @fail
@header_end:
      LDA   p_symbol_count
      STA   p_scope_param_end
      LDA   p_reusing_forward
      BEQ   @store_signature
      JSR   p_validate_forward_signature
      long_bcs @fail
      BRA   @signature_ready
@store_signature:
      JSR   p_store_current_routine_signature
@signature_ready:
      LDA   p_compiling_unit
      BEQ   @interface_done
      LDA   p_reusing_forward
      BNE   @interface_done
      JSR   p_validate_interface_routine
      long_bcs @fail
@interface_done:
      LDA   #';'
      JSR   p_expect_char
      long_bcs @fail
      JSR   p_capture_identifier
      long_bcs @fail
      LDA   #<kw_forward
      LDX   #>kw_forward
      JSR   p_ident_is
      BCC   @definition
      LDA   p_reusing_forward
      long_bne @fail
      LDX   p_current_routine
      LDA   p_routine_kind_table,X
      ORA   #P_ROUTINE_FORWARD
      STA   p_routine_kind_table,X
      LDA   p_scope_symbol_base
      STA   p_symbol_count
      DEC   p_scope_level
      LDA   p_routine_owner,X
      STA   p_current_routine
      STZ   p_function_active
      LDA   #';'
      JSR   p_expect_char
      long_bcs @fail
      JSR   p_capture_identifier
      long_bcs @fail
      JMP   @declaration
@definition:
      JSR   p_new_exit_label
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
      LDA   p_line
      STA   p_statement_line
      LDA   p_line+1
      STA   p_statement_line+1
      JSR   p_emit_frame_enter
      long_bcs @output
      JSR   p_emit_open_array_copies
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
      JSR   p_emit_exit_label
      long_bcs @output
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
      JSR   p_check_forwards_resolved
      BCS   @fail
      CLC
      RTS

; Parse the syntax shared by public interface declarations and executable
; routine definitions. Callers initialize the parameter/frame state first.
p_parse_routine_signature:
      STZ   p_function_return_type
      JSR   p_skip_ws
      JSR   p_peek
      long_bcc @fail
      CMP   #'('
      BNE   @return_type
      JSR   p_parse_formal_parameters
      BCS   @fail
@return_type:
      LDA   p_routine_kind
      BEQ   @done
      LDA   #':'
      JSR   p_expect_char
      BCS   @fail
      JSR   p_parse_decl_type
      BCS   @fail
      LDA   p_decl_type
      STA   p_function_return_type
@done:
      CLC
      RTS
@fail:
      SEC
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
      LDX   #>kw_end
      JMP   p_expect_ax_word
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
      LDA   p_line
      STA   p_statement_line
      LDA   p_line+1
      STA   p_statement_line+1
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
      LDA   #<kw_inc
      LDX   #>kw_inc
      JSR   p_ident_is
      BCC   @decrement
      LDA   #'+'
      BRA   @inc_dec
@decrement:
      LDA   #<kw_dec
      LDX   #>kw_dec
      JSR   p_ident_is
      BCC   @new
      LDA   #'-'
@inc_dec:
      STA   p_inc_operator
      JSR   p_skip_ws
      JSR   p_peek
      long_bcc @fail
      CMP   #'('
      BNE   @identifier
      JMP   p_parse_inc_dec
@new:
      LDA   #<kw_new
      LDX   #>kw_new
      JSR   p_ident_is
      long_bcs p_parse_new
      LDA   #<kw_dispose
      LDX   #>kw_dispose
      JSR   p_ident_is
      long_bcs p_parse_dispose
      LDA   #<kw_getmem
      LDX   #>kw_getmem
      JSR   p_ident_is
      long_bcs p_parse_getmem
      LDA   #<kw_freemem
      LDX   #>kw_freemem
      JSR   p_ident_is
      long_bcs p_parse_freemem
@assembly:
      LDA   #<kw_break
      LDX   #>kw_break
      JSR   p_ident_is
      long_bcs p_parse_break
      LDA   #<kw_continue
      LDX   #>kw_continue
      JSR   p_ident_is
      long_bcs p_parse_continue
      LDA   #<kw_exit
      LDX   #>kw_exit
      JSR   p_ident_is
      long_bcs p_parse_exit
      LDA   #<kw_asm
      LDX   #>kw_asm
      JSR   p_ident_is
      long_bcs p_parse_asm

@identifier:
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
      CMP   #'^'
      BEQ   @designator_assignment
      CMP   #':'
      long_bne @bare_call
      JSR   p_symbol_find
      BCC   @with_assignment
      STX   p_target_symbol
      STA   p_target_type
      JSR   p_save_target_storage
      JSR   p_type_open_element
      long_bcs @fail
      LDA   p_target_type
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
      JSR   p_save_assignment_identifier
      JSR   p_next
@equals:
      LDA   #'='
      JSR   p_expect_char
      long_bcs @fail
      JSR   p_parse_expression
      long_bcs @fail
      JSR   p_restore_assignment_identifier
      JSR   p_emit_store_saved
      long_bcs @output
      CLC
      RTS
@designator_assignment:
      JSR   p_parse_designator_address
      long_bcs @fail
      LDA   p_designator_type
      STA   p_target_type
      LDA   p_symbol_flags
      STA   p_target_flags
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
      JSR   p_prepare_statement_indirect
      long_bcs @output
      JSR   p_save_call_identifier
      JSR   p_next
      JSR   p_count_call_arguments
      long_bcs @fail
      STA   p_call_arity
      JSR   p_saved_call_metadata
      STZ   p_read_size
      STZ   p_read_size+1
      STZ   p_val_size
      STZ   p_argument_count
      STZ   p_argument_bytes
      LDA   p_indirect_call
      STA   p_function_indirect
      BEQ   :+
      LDA   p_call_type_row
      STA   p_function_call_type_row
:
      JSR   p_push_call_identifier
      long_bcs @fail
      INC   p_statement_call_saved
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
      long_beq @call_close
@argument:
      JSR   p_load_expected_argument_type
      long_bcs @fail
      JSR   p_parse_statement_argument
      long_bcs @fail
      JSR   p_emit_coerce_argument
      long_bcs @output
      LDX   p_argument_count
      LDA   p_argument_bits,X
      AND   p_statement_refmask
      BNE   @argument_valid
      JSR   p_check_assignment_compatibility
      long_bcs @fail
@argument_valid:
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
      BNE   @push_last
      LDA   p_indirect_call
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
      JSR   p_restore_call_identifier
      long_bcs @fail
      JSR   p_restore_call_identifier_to_saved
      LDA   p_indirect_call
      BEQ   @direct_call
      LDX   p_call_type_row
      LDA   p_type_low_l,X
      CMP   p_argument_count
      long_bne @fail
      JSR   p_emit_indirect_call
      long_bcs @output
      BRA   @call_done
@direct_call:
      JSR   p_routine_find_saved
      BCC   @external_call
      LDA   p_routine_kind_table,X
      AND   #P_ROUTINE_KIND_MASK
      long_bne @fail
      LDA   p_routine_params,X
      CMP   p_argument_count
      long_bne @fail
      STX   p_routine_iter
      JSR   p_emit_local_call_saved
      long_bcs @output
      LDX   p_routine_iter
      LDA   p_routine_kind_table,X
      AND   #P_ROUTINE_EXTERNAL
      BEQ   @call_done
      JSR   p_emit_drop_arguments
      long_bcs @output
@call_done:
      JSR   p_finish_statement_call
      CLC
      RTS
@external_call:
      LDA   p_argument_count
      CMP   #2
      BCS   @pascal_call
      JSR   p_emit_sig_saved
      long_bcs @output
      JSR   p_emit_call_saved
      long_bcs @output
      LDA   p_statement_force_stack
      BEQ   @direct_io
      JSR   p_emit_drop_arguments
      BCS   @output
@direct_io:
      JSR   p_emit_io_check
      BCS   @output
      BRA   @call_done
@pascal_call:
      JSR   p_emit_pascal_sig_saved
      BCS   @output
      LDA   p_insert_call
      BEQ   @insert_done
      LDA   p_insert_capacity
      JSR   p_emit_load_byte
      BCS   @output
      LDA   #<asm_insert_capacity
      LDX   #>asm_insert_capacity
      JSR   p_emit_ax_text
      BCS   @output
@insert_done:
      JSR   p_emit_call_saved
      BCS   @output
      JSR   p_emit_drop_arguments
      BCS   @output
      JSR   p_emit_io_check
      BCS   @output
      BRA   @call_done
@bare_call:
      JSR   p_prepare_statement_indirect
      BCS   @output
      LDA   p_indirect_call
      BEQ   @direct_bare_call
      LDX   p_call_type_row
      LDA   p_type_low_l,X
      BNE   @fail
      STZ   p_argument_bytes
      JSR   p_emit_indirect_call
      BCS   @output
      CLC
      RTS
@direct_bare_call:
      JSR   p_routine_find_saved
      BCC   @external_bare_call
      LDA   p_routine_kind_table,X
      AND   #P_ROUTINE_KIND_MASK
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
      JSR   p_finish_statement_call
      SEC
      RTS

p_finish_statement_call:
      LDA   p_statement_call_saved
      BEQ   @done
      STZ   p_statement_call_saved
      JMP   p_pop_call_identifier
@done:
      RTS

; Turbo-compatible low-RAM heap intrinsics use the NDK allocator directly.
; Typed Pascal pointers never expose XRAM's 24-bit address space.
p_parse_heap_designator:
      LDA   #'('
      JSR   p_expect_char
      BCS   @fail
      JSR   p_capture_identifier
      BCS   @fail
      JSR   p_parse_designator_address
      BCS   @fail
      LDA   p_designator_type
      JMP   p_type_pointer_element
@fail:
      CLC
      RTS

p_parse_new:
      JSR   p_parse_heap_designator
      long_bcc @fail
      CMP   #TYPE_NONE
      long_beq @fail
      JSR   p_type_size
      long_bcs @fail
      LDA   p_array_len
      STA   p_decimal
      LDA   p_array_len+1
      STA   p_decimal+1
      LDA   #')'
      JSR   p_expect_char
      long_bcs @fail
      LDA   #<asm_phx_pha
      LDX   #>asm_phx_pha
      JSR   p_emit_ax_text
      long_bcs @output
      JSR   p_emit_load_word
      long_bcs @output
      JSR   p_emit_heap_allocate
      long_bcs @output
      CLC
      RTS
@output:
      INC   p_emit_error
@fail:
      SEC
      RTS

p_parse_dispose:
      JSR   p_parse_heap_designator
      long_bcc @fail
      CMP   #TYPE_NONE
      long_beq @fail
      LDA   #')'
      JSR   p_expect_char
      long_bcs @fail
      LDA   #<asm_deref16
      LDX   #>asm_deref16
      JSR   p_emit_ax_text
      long_bcs @output
      JSR   p_emit_heap_free
      long_bcs @output
      CLC
      RTS
@output:
      INC   p_emit_error
@fail:
      SEC
      RTS

p_parse_getmem:
      JSR   p_parse_heap_designator
      long_bcc @fail
      LDA   #<asm_phx_pha
      LDX   #>asm_phx_pha
      JSR   p_emit_ax_text
      long_bcs @output
      LDA   #','
      JSR   p_expect_char
      long_bcs @fail
      JSR   p_parse_expression
      long_bcs @fail
      LDA   p_expr_type
      JSR   p_type_is_ordinal
      long_bcc @fail
      LDA   #')'
      JSR   p_expect_char
      long_bcs @fail
      LDA   p_expr_type
      JSR   p_type_is_word_value
      BNE   @allocate
      LDA   #<asm_index_high_zero
      LDX   #>asm_index_high_zero
      JSR   p_emit_ax_text
      long_bcs @output
@allocate:
      JSR   p_emit_heap_allocate
      long_bcs @output
      CLC
      RTS
@output:
      INC   p_emit_error
@fail:
      SEC
      RTS

p_parse_freemem:
      JSR   p_parse_heap_designator
      long_bcc @fail
      LDA   #<asm_deref16
      LDX   #>asm_deref16
      JSR   p_emit_ax_text
      long_bcs @output
      LDA   #<asm_phx_pha
      LDX   #>asm_phx_pha
      JSR   p_emit_ax_text
      long_bcs @output
      LDA   #','
      JSR   p_expect_char
      long_bcs @fail
      JSR   p_parse_expression
      long_bcs @fail
      LDA   p_expr_type
      JSR   p_type_is_ordinal
      long_bcc @fail
      LDA   #')'
      JSR   p_expect_char
      long_bcs @fail
      LDA   #<asm_drop_word
      LDX   #>asm_drop_word
      JSR   p_emit_ax_text
      long_bcs @output
      JSR   p_emit_heap_free
      long_bcs @output
      CLC
      RTS
@output:
      INC   p_emit_error
@fail:
      SEC
      RTS

p_emit_heap_allocate:
      LDA   #1
      STA   p_heap_include_emitted
      LDA   #<asm_heap_alloc_call
      LDX   #>asm_heap_alloc_call
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   #203
      STA   p_digit
      LDA   #<asm_heap_alloc_error
      LDX   #>asm_heap_alloc_error
      JSR   p_emit_heap_error
      BCS   @fail
      LDA   #<asm_heap_store
      LDX   #>asm_heap_store
      JMP   p_emit_ax_text
@fail:
      RTS

p_emit_heap_free:
      LDA   #1
      STA   p_heap_include_emitted
      LDA   #<asm_heap_free_call
      LDX   #>asm_heap_free_call
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   #204
      STA   p_digit
      LDA   #<asm_heap_free_error
      LDX   #>asm_heap_free_error
      JMP   p_emit_heap_error
@fail:
      RTS

p_emit_heap_source:
      LDA   p_heap_include_emitted
      BEQ   @done
      LDA   #<asm_heap_include
      LDX   #>asm_heap_include
      JMP   p_emit_ax_text
@done:
      CLC
      RTS

; A/X selects the conditional-error prefix; p_digit is the Turbo error code.
p_emit_heap_error:
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   p_digit
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   #<asm_heap_error_line_x
      LDX   #>asm_heap_error_line_x
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   p_statement_line
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   #<asm_heap_error_line_y
      LDX   #>asm_heap_error_line_y
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   p_statement_line+1
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   #<asm_heap_error_end
      LDX   #>asm_heap_error_end
      JMP   p_emit_ax_text
@fail:
      RTS

; INC/DEC reuse the ordinary designator address, scalar expression, and store
; paths. This keeps globals, locals, parameters, fields, and array elements on
; one implementation and leaves wrap/range policy to the selected check mode.
p_parse_inc_dec:
      LDA   #'('
      JSR   p_expect_char
      long_bcs @fail
      JSR   p_capture_identifier
      long_bcs @fail
      JSR   p_parse_designator_address
      long_bcs @fail
      LDA   p_designator_type
      STA   p_target_type
      CMP   #TYPE_BYTE
      BEQ   @sized
      CMP   #TYPE_WORD
      BEQ   @sized
      CMP   #TYPE_INTEGER
      BEQ   @sized
      CMP   #TYPE_CHAR
      BEQ   @sized
      CMP   #P_TYPE_USER_BASE
      long_bcc @fail
      SEC
      SBC   #P_TYPE_USER_BASE
      TAX
      CPX   p_type_count
      long_bcs @fail
      LDA   p_type_kind,X
      CMP   #TYPE_KIND_ORDINAL
      long_bne @fail
@sized:
      LDA   p_target_type
      JSR   p_type_size
      long_bcs @fail
      LDA   p_array_len+1
      long_bne @fail
      LDA   p_array_len
      CMP   #1
      BEQ   @push_address
      CMP   #2
      long_bne @fail
@push_address:
      LDA   #<asm_phx_pha
      LDX   #>asm_phx_pha
      JSR   p_emit_ax_text
      long_bcs @output
      LDA   p_array_len
      CMP   #1
      BNE   @load_word
      LDA   #<asm_deref8
      LDX   #>asm_deref8
      BRA   @load
@load_word:
      LDA   #<asm_deref16
      LDX   #>asm_deref16
@load:
      JSR   p_emit_ax_text
      long_bcs @output
      LDA   p_target_type
      STA   p_expr_type
      LDA   p_inc_operator
      JSR   p_push_operator
      long_bcs @fail
      JSR   p_emit_binary_prep
      long_bcs @drop_fail
      JSR   p_skip_ws
      JSR   p_peek
      long_bcc @drop_fail
      CMP   #','
      BNE   @default_amount
      JSR   p_next
      JSR   p_parse_expression
      long_bcs @drop_fail
      BRA   @close
@default_amount:
      LDA   #1
      JSR   p_emit_load_byte
      long_bcs @drop_output
      LDA   #TYPE_BYTE
      STA   p_expr_type
@close:
      LDA   #')'
      JSR   p_expect_char
      long_bcs @drop_fail
      JSR   p_pop_operator
      LDA   p_left_type
      JSR   p_type_is_word_value
      BNE   @wide
      LDA   p_expr_type
      JSR   p_type_is_word_value
      BNE   @wide
      LDA   p_inc_operator
      CMP   #'+'
      BNE   @subtract_byte
      LDA   #<asm_add
      LDX   #>asm_add
      BRA   @emit_operation
@subtract_byte:
      LDA   #<asm_subtract
      LDX   #>asm_subtract
      BRA   @emit_operation
@wide:
      JSR   p_emit_wide_operands
      long_bcs @output
      LDA   p_inc_operator
      CMP   #'+'
      BNE   @subtract_word
      LDA   #<asm_add_word
      LDX   #>asm_add_word
      BRA   @emit_operation
@subtract_word:
      LDA   #<asm_subtract_word
      LDX   #>asm_subtract_word
@emit_operation:
      JSR   p_emit_ax_text
      long_bcs @output
      LDA   p_left_type
      STA   p_expr_type
      JSR   p_emit_store_designator
      long_bcs @output
      CLC
      RTS
@drop_output:
      INC   p_emit_error
@drop_fail:
      DEC   p_operator_depth
@fail:
      SEC
      RTS
@output:
      INC   p_emit_error
      SEC
      RTS

p_find_enclosing_loop:
      LDX   p_control_depth
@control:
      CPX   #0
      BEQ   @missing
      DEX
      LDA   p_control_kind,X
      CMP   #CONTROL_WHILE
      BEQ   @found
      CMP   #CONTROL_REPEAT
      BEQ   @found
      CMP   #CONTROL_FOR
      BNE   @control
@found:
      SEC
      RTS
@missing:
      CLC
      RTS

p_parse_break:
      JSR   p_find_enclosing_loop
      BCC   @fail
      JSR   p_load_control_b
      JSR   p_emit_jump
      BCS   @output
      CLC
      RTS
@output:
      INC   p_emit_error
@fail:
      SEC
      RTS

p_parse_continue:
      JSR   p_find_enclosing_loop
      BCC   @fail
      LDA   p_control_kind,X
      CMP   #CONTROL_WHILE
      BNE   @step
      JSR   p_load_control_a
      BRA   @jump
@step:
      JSR   p_load_control_continue
@jump:
      JSR   p_emit_jump
      BCS   @output
      CLC
      RTS
@output:
      INC   p_emit_error
@fail:
      SEC
      RTS

p_parse_exit:
      JSR   p_load_exit_label
      JSR   p_emit_jump
      BCS   @output
      CLC
      RTS
@output:
      INC   p_emit_error
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
      LDA   p_target_type
      JSR   p_type_open_element
      BCS   p_parse_open_array_argument
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

; Pass an open array as {base pointer, High}. The hidden bound is pushed first
; so P_FENTER sees the descriptor bytes in their natural frame order. Fixed
; arrays are remapped to zero-based bounds; a scalar actual is one element.
p_parse_open_array_argument:
      STA   p_open_element
      LDA   p_target_type
      STA   p_open_expected
      JSR   p_capture_identifier
      long_bcs @fail
      JSR   p_parse_designator_address
      long_bcs @fail
      LDX   p_argument_count
      LDA   p_argument_bits,X
      AND   p_statement_refmask
      BEQ   @type
      LDA   p_symbol_flags
      AND   #SYM_CONST
      long_bne @fail
@type:
      LDA   p_designator_type
      JSR   p_type_open_element
      BCS   @forwarded
      LDA   p_designator_type
      CMP   #P_TYPE_USER_BASE
      BCC   @scalar
      SEC
      SBC   #P_TYPE_USER_BASE
      TAX
      CPX   p_type_count
      long_bcs @fail
      LDA   p_type_kind,X
      CMP   #TYPE_KIND_ARRAY
      BNE   @scalar
      LDA   p_type_low_h,X
      CMP   #1
      long_bne @fail
      LDA   p_type_element,X
      CMP   p_open_element
      BNE   @fail
      LDA   p_type_low_l,X
      TAX
      CPX   p_dim_count
      BCS   @fail
      SEC
      LDA   p_dim_extent_l,X
      SBC   #1
      STA   p_decimal
      LDA   p_dim_extent_h,X
      SBC   #0
      STA   p_decimal+1
      BRA   @literal_high
@scalar:
      LDA   p_designator_type
      CMP   p_open_element
      BNE   @fail
      STZ   p_decimal
      STZ   p_decimal+1
@literal_high:
      LDA   #<asm_open_base_save
      LDX   #>asm_open_base_save
      JSR   p_emit_ax_text
      BCS   @fail
      JSR   p_emit_load_word
      BCS   @fail
      BRA   @push_high
@forwarded:
      CMP   p_open_element
      BNE   @fail
      LDA   #<asm_open_base_save
      LDX   #>asm_open_base_save
      JSR   p_emit_ax_text
      BCS   @fail
      JSR   p_emit_open_array_high
      BCS   @fail
@push_high:
      LDA   #TYPE_WORD
      STA   p_expr_type
      JSR   p_emit_push_argument
      BCS   @fail
      LDA   #<asm_open_base_restore
      LDX   #>asm_open_base_restore
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   p_open_expected
      STA   p_expr_type
      INC   p_statement_force_stack
      CLC
      RTS
@fail:
      SEC
      RTS

p_prepare_statement_indirect:
      STZ   p_indirect_call
      JSR   p_symbol_find
      BCC   @direct
      STA   p_call_type
      JSR   p_type_is_procedural
      BCC   @direct
      CMP   #TYPE_KIND_PROCEDURE
      BNE   @fail
      STX   p_call_type_row
      INC   p_indirect_call
      LDA   p_call_type
      JMP   p_emit_indirect_target
@direct:
      CLC
      RTS
@fail:
      SEC
      RTS

p_prepare_function_indirect:
      STZ   p_function_indirect
      JSR   p_symbol_find
      BCC   @direct
      JSR   p_type_is_procedural
      BCC   @direct
      CMP   #TYPE_KIND_FUNCTION
      BNE   @fail
      STX   p_function_call_type_row
      INC   p_function_indirect
      TXA
      CLC
      ADC   #P_TYPE_USER_BASE
      JMP   p_emit_indirect_target
@direct:
      CLC
      RTS
@fail:
      SEC
      RTS

p_emit_indirect_target:
      STA   p_expr_type
      LDA   p_symbol_flags
      BEQ   @global
      JSR   p_emit_load_frame_value
      BCS   @fail
      BRA   @push
@global:
      JSR   p_emit_load_word_identifier
      BCS   @fail
@push:
      LDA   #<asm_phx_pha
      LDX   #>asm_phx_pha
      JMP   p_emit_ax_text
@fail:
      SEC
      RTS

p_load_expected_argument_type:
      STZ   p_target_type
      STZ   p_target_flags
      LDA   p_statement_typed
      BEQ   @done
      LDY   p_argument_count
      CPY   #P_ROUTINE_PARAM_CAP
      BCS   @fail
      CMP   #2
      BEQ   @procedural
      LDX   p_statement_routine
      JSR   p_routine_param_pointer
      BRA   @load
@procedural:
      LDX   p_call_type_row
      JSR   p_type_param_pointer
@load:
      LDA   (p_word),Y
      STA   p_target_type
@done:
      CLC
      RTS
@fail:
      SEC
      RTS

p_emit_indirect_call:
      LDA   p_argument_bytes
      LDX   p_argument_count
      CPX   #1
      BNE   @metadata
      ORA   #$80
      LDX   p_statement_widthmask
      BEQ   @metadata
      ORA   #$40
@metadata:
      JSR   p_emit_load_byte
      BCS   @fail
      LDA   #<asm_indirect_call
      LDX   #>asm_indirect_call
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   p_argument_count
      CMP   #2
      BCS   @callee_consumed
      LDA   p_statement_force_stack
      BEQ   @drop
@callee_consumed:
      STZ   p_argument_bytes
@drop:
      INC   p_argument_bytes
      INC   p_argument_bytes
      JMP   p_emit_drop_arguments
@fail:
      RTS

; Resolve reference and word-width semantics together. Keeping one metadata
; table avoids duplicate name hashes for routines such as Delete and XRamRead.
p_saved_call_metadata:
      STZ   p_statement_refmask
      STZ   p_statement_widthmask
      STZ   p_statement_typed
      STZ   p_statement_io_kind
      STZ   p_statement_force_stack
      STZ   p_read_call
      STZ   p_val_call
      STZ   p_insert_call
      STZ   p_insert_capacity
      LDA   p_indirect_call
      BEQ   @routine
      LDX   p_call_type_row
      LDA   p_type_low_h,X
      STA   p_statement_refmask
      LDA   p_type_high_l,X
      STA   p_statement_widthmask
      LDA   #2
      STA   p_statement_typed
      RTS
@routine:
      JSR   p_routine_find_saved
      BCC   @system
      STX   p_statement_routine
      INC   p_statement_typed
      LDA   p_routine_refmask,X
      STA   p_statement_refmask
      JSR   p_routine_width_pointer
      LDY   #0
      LDA   (p_word),Y
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
      CPX   #4
      BCS   @other_io
      INC   p_read_call
      LDA   p_call_arity
      CMP   #1
      BNE   @io_read
      INC   p_statement_force_stack
      BRA   @io_read
@other_io:
      CPX   #5
      BEQ   @io_read
      CPX   #7
      BCC   @io_done
      CPX   #9
      BCS   @io_done
      LDA   #2
      BRA   @io_store
@io_read:
      LDA   #1
@io_store:
      STA   p_statement_io_kind
@io_done:
      CPX   #SYSTEM_CALL_VAL
      BNE   :+
      INC   p_val_call
:     CPX   #SYSTEM_CALL_INSERT
      BNE   @done
      INC   p_insert_call
@done:
      RTS
@next:
      INX
      BRA   @entry

p_emit_coerce_argument:
      LDA   p_insert_call
      BEQ   @width
      LDX   p_argument_count
      BNE   @insert_destination
      LDA   p_expr_type
      JSR   p_type_is_string
      BCS   @width
      LDA   p_expr_type
      JSR   p_type_is_char
      BCC   @fail
      LDA   #<asm_char_to_string
      LDX   #>asm_char_to_string
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   #TYPE_STRING_REF
      STA   p_expr_type
      BRA   @width
@insert_destination:
      CPX   #1
      BNE   @width
      LDA   p_designator_type
      JSR   p_type_is_string
      BCC   @fail
      LDA   p_designator_type
      JSR   p_type_size
      BCS   @fail
      LDA   p_array_len+1
      BEQ   @insert_short
      CMP   #1
      BNE   @fail
      LDA   p_array_len
      BNE   @fail
      LDA   #$FF
      BRA   @insert_capacity
@insert_short:
      LDA   p_array_len
      BEQ   @fail
      DEC
@insert_capacity:
      STA   p_insert_capacity
@width:
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
      LDX   #>kw_end
      JMP   p_expect_ax_word
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
      CMP   #WRITE_UWORD
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
      LDA   p_write_file
      BEQ   @delimiter
      JSR   p_emit_io_write_check
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
      JSR   p_emit_io_write_check
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
      BCC   @try_longint
      LDA   #WRITE_REAL
      BRA   @scalar4
@try_longint:
      LDA   p_expr_type
      JSR   p_type_is_longint
      BCC   @ordinal
      LDA   #WRITE_LONGINT
@scalar4:
      STA   p_write_kind
      LDA   #<asm_phx_pha
      LDX   #>asm_phx_pha
      JSR   p_emit_ax_text
      long_bcs @output
      BRA   @format
@ordinal:
      LDA   p_expr_type
      CMP   #TYPE_BYTE
      BEQ   @ordinal_size
      CMP   #TYPE_WORD
      BEQ   @ordinal_size
      CMP   #TYPE_INTEGER
      BEQ   @ordinal_size
      CMP   #TYPE_CHAR
      BEQ   @ordinal_size
      CMP   #P_TYPE_USER_BASE
      long_bcc @fail
      SEC
      SBC   #P_TYPE_USER_BASE
      TAX
      CPX   p_type_count
      long_bcs @fail
      LDA   p_type_kind,X
      CMP   #TYPE_KIND_ORDINAL
      long_bne @fail
      LDA   p_expr_type
@ordinal_size:
      JSR   p_type_size
      long_bcs @fail
      LDA   p_array_len+1
      long_bne @fail
      LDA   p_array_len
      CMP   #1
      BEQ   @extend_ordinal
      CMP   #2
      long_bne @fail
      BRA   @push_ordinal
@extend_ordinal:
      LDA   #<asm_index_high_zero
      LDX   #>asm_index_high_zero
      JSR   p_emit_ax_text
      long_bcs @output
@push_ordinal:
      LDA   p_expr_type
      JSR   p_type_is_signed_ordinal
      BEQ   @unsigned_ordinal
      LDA   #WRITE_WORD
      BRA   @save_ordinal_kind
@unsigned_ordinal:
      LDA   #WRITE_UWORD
@save_ordinal_kind:
      STA   p_write_kind
      LDA   #<asm_phx_pha
      LDX   #>asm_phx_pha
      JSR   p_emit_ax_text
      long_bcs @output
@format:
      STZ   p_digit
      JSR   p_skip_ws
      JSR   p_peek
      long_bcc @fail
      CMP   #':'
      BNE   @default_width
      JSR   p_next
      JSR   p_parse_expression
      long_bcs @fail
      BRA   @push_width
@default_width:
      CMP   #','
      long_bne @fail
      LDA   #0
      JSR   p_emit_load_byte
      long_bcs @output
      LDA   #TYPE_BYTE
      STA   p_expr_type
@push_width:
      JSR   p_emit_push_word_value
      long_bcs @output
      JSR   p_skip_ws
      JSR   p_peek
      long_bcc @fail
      CMP   #':'
      BNE   @destination
      LDA   p_write_kind
      CMP   #WRITE_REAL
      long_bne @fail
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
      LDA   p_write_kind
      CMP   #WRITE_REAL
      BEQ   @real
      CMP   #WRITE_LONGINT
      BEQ   @longint
      CMP   #WRITE_UWORD
      BEQ   @unsigned
      LDA   #<asm_str_integer
      LDX   #>asm_str_integer
      BRA   @emit_ordinal
@longint:
      LDA   #<asm_str_long
      LDX   #>asm_str_long
      BRA   @emit_ordinal
@unsigned:
      LDA   #<asm_str_uword
      LDX   #>asm_str_uword
@emit_ordinal:
      JSR   p_emit_ax_text
      long_bcs @output
      LDA   #4
      BRA   @drop
@real:
      LDA   p_digit
      BEQ   @plain
      LDA   #<asm_str_real_precision
      LDX   #>asm_str_real_precision
      JSR   p_emit_ax_text
      long_bcs @output
      LDA   #6
      BRA   @drop
@plain:
      LDA   #<asm_str_real
      LDX   #>asm_str_real
      JSR   p_emit_ax_text
      long_bcs @output
      LDA   #4
@drop:
      JSR   p_emit_drop_count
      long_bcs @output
      LDA   #')'
      JMP   p_expect_char
@output:
      INC   p_emit_error
@fail:
      SEC
      RTS

p_emit_push_word_value:
      LDA   p_expr_type
      JSR   p_type_is_word_value
      BNE   @push
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
      JSR   p_type_is_longint
      BCS   @longint
      LDA   p_write_type
      CMP   #TYPE_CHAR
      BEQ   @char
      CMP   #TYPE_WORD
      BEQ   @uword
      CMP   #TYPE_INTEGER
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
      BCC   @byte
      LDA   p_write_type
      JSR   p_type_is_signed_ordinal
      BNE   @word
@uword:
      LDA   #WRITE_UWORD
      CLC
      RTS
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
@longint:
      LDA   #WRITE_LONGINT
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
      BEQ   @signed_word
      CMP   #WRITE_UWORD
      BNE   @real
      LDA   #3
      BRA   @mode
@signed_word:
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
      LDA   #6
      JMP   p_emit_drop_count
@drop_four:
      LDA   #4
      JMP   p_emit_drop_count
@fail:
      SEC
      RTS

p_emit_write_value:
      LDA   p_write_kind
      CMP   #WRITE_BLOCK
      BEQ   @block
      LDA   p_write_file_text
      long_bne p_emit_text_file_value
      LDA   p_write_kind
      CMP   #WRITE_CHAR
      BEQ   @char
      CMP   #WRITE_WORD
      BEQ   @word
      CMP   #WRITE_UWORD
      BEQ   @uword
      CMP   #WRITE_STRING
      BEQ   @string
      CMP   #WRITE_REAL
      BEQ   @real
      CMP   #WRITE_LONGINT
      BEQ   @longint
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
@uword:
      LDA   #<asm_jsr_write_uword
      LDX   #>asm_jsr_write_uword
      JMP   p_emit_ax_text
@string:
      LDA   #<asm_jsr_write_string
      LDX   #>asm_jsr_write_string
      JMP   p_emit_ax_text
@real:
      LDA   #<asm_jsr_write_real
      LDX   #>asm_jsr_write_real
      JMP   p_emit_ax_text
@longint:
      LDA   #<asm_write_long
      LDX   #>asm_write_long
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
      CMP   #WRITE_UWORD
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
      ADC   #<P_STATE_FIELD_NAME_OFFSET
      STA   p_state_offset
      LDA   #>P_STATE_FIELD_NAME_OFFSET
      ADC   #0
      STA   p_state_offset+1
      JMP   p_state_pointer

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
      LDX   #>kw_do
      JSR   p_expect_ax_word
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
      LDA   #CONTROL_CASE
      STA   p_control_kind,X
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
      LDX   #>kw_of
      JSR   p_expect_ax_word
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
      ; A lone '.' cannot follow a case constant, so seeing one means a Turbo
      ; range label. The low half is emitted before the high constant is parsed,
      ; which keeps both bounds streaming without a second constant slot.
      JSR   p_skip_ws
      JSR   p_peek
      long_bcc @fail
      CMP   #'.'
      BNE   @single_label
      JSR   p_next
      LDA   #'.'
      JSR   p_expect_char
      long_bcs @fail
      JSR   p_emit_case_range_low
      long_bcs @output
      LDA   #TYPE_WORD
      STA   p_decl_type
      JSR   p_parse_const_scalar
      long_bcs @fail
      JSR   p_emit_case_range_high
      long_bcs @output
      BRA   @label_delimiter
@single_label:
      JSR   p_emit_case_compare
      long_bcs @output
@label_delimiter:
      JSR   p_skip_ws
      JSR   p_peek
      long_bcc @fail
      CMP   #','
      BNE   @colon
      JSR   p_next
      JMP   @case_label
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
      LDX   #>kw_else
      JSR   p_expect_ax_word
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
      LDX   #>kw_end
      JSR   p_expect_ax_word
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

; Turbo range labels compare against both bounds without disturbing the
; selector, which stays live in A (byte) or A/X (word) for the remaining arms.
; Testing `<= high` directly rather than `< high+1` keeps an upper bound of 255
; or $FFFF working.
p_emit_case_range_low:
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
      LDA   #<asm_case_wrange_low
      LDX   #>asm_case_wrange_low
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   p_decimal
      JMP   p_emit_hex_byte
@byte:
      LDA   p_decimal+1
      BNE   @fail
      LDA   #<asm_case_byte
      LDX   #>asm_case_byte
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   p_decimal
      JMP   p_emit_hex_byte
@fail:
      SEC
      RTS

p_emit_case_range_high:
      LDX   p_control_depth
      DEX
      LDA   p_case_word,X
      BEQ   @byte
      LDA   #<asm_case_wrange_high
      LDX   #>asm_case_wrange_high
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   p_decimal+1
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   #<asm_case_range_branch
      LDX   #>asm_case_range_branch
      JSR   p_emit_case_arm_label
      BCS   @fail
      LDA   #<asm_case_word_low
      LDX   #>asm_case_word_low
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   p_decimal
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   #<asm_case_branch
      LDX   #>asm_case_branch
      JSR   p_emit_case_arm_label
      BCS   @fail
      BRA   @close
@byte:
      LDA   p_decimal+1
      BNE   @fail
      LDA   #<asm_case_range_mid
      LDX   #>asm_case_range_mid
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   p_decimal
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   #<asm_case_branch
      LDX   #>asm_case_branch
      JSR   p_emit_case_arm_label
      BCS   @fail
@close:
      LDA   #<asm_case_range_branch
      LDX   #>asm_case_range_branch
      JSR   p_emit_case_arm_label
      BCS   @fail
      LDA   #<asm_case_word_end
      LDX   #>asm_case_word_end
      JMP   p_emit_ax_text
@fail:
      SEC
      RTS

; Emit the branch text in A/X followed by this arm's generated label id.
p_emit_case_arm_label:
      JSR   p_emit_ax_text
      BCS   @fail
      LDX   p_control_depth
      DEX
      LDA   p_case_body_lo,X
      STA   p_label_saved
      LDA   p_case_body_hi,X
      STA   p_label_saved+1
      JMP   p_emit_label_id
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
      CMP   #'^'
      BEQ   @has_deref
      JMP   @done
@has_deref:
      LDA   p_designator_type
      JSR   p_type_pointer_element
      long_bcc @fail
      CMP   #TYPE_NONE
      long_beq @fail
      STA   p_designator_type
      STA   p_expr_type
      JSR   p_next
      LDA   #<asm_deref16
      LDX   #>asm_deref16
      JSR   p_emit_ax_text
      long_bcs @fail
      JMP   @selector
@has_index:
      LDA   p_designator_type
      JSR   p_type_open_element
      long_bcs @open_index
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
      JSR   p_type_is_word_value
      BNE   @index_is_word
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
@open_index:
      LDY   p_array_depth
      CPY   #8
      long_bcs @fail
      LDA   p_symbol_iter
      STA   p_array_symbols,Y
      INC   p_array_depth
      JSR   p_next
      LDA   #<asm_phx_pha
      LDX   #>asm_phx_pha
      JSR   p_emit_ax_text
      long_bcs @open_drop_fail
      JSR   p_parse_expression
      long_bcs @open_drop_fail
      LDA   p_expr_type
      JSR   p_type_is_word_value
      BNE   @open_word_index
      LDA   #<asm_index_high_zero
      LDX   #>asm_index_high_zero
      JSR   p_emit_ax_text
      long_bcs @open_drop_fail
@open_word_index:
      DEC   p_array_depth
      LDX   p_array_depth
      LDA   p_array_symbols,X
      TAX
      JSR   p_load_symbol_metadata
      LDA   p_symbol_type
      STA   p_designator_type
      JSR   p_emit_open_array_range_check
      long_bcs @fail
      LDA   #']'
      JSR   p_expect_char
      long_bcs @fail
      LDA   p_designator_type
      JSR   p_type_open_element
      long_bcc @fail
      STA   p_open_element
      JSR   p_type_size
      long_bcs @fail
      JSR   p_emit_open_array_index
      long_bcs @fail
      LDA   p_open_element
      STA   p_designator_type
      STA   p_expr_type
      JMP   @selector
@open_drop_fail:
      DEC   p_array_depth
      JMP   @fail
@string_index:
      JSR   p_next
      LDA   #<asm_phx_pha
      LDX   #>asm_phx_pha
      JSR   p_emit_ax_text
      long_bcs @fail
      JSR   p_parse_expression
      long_bcs @fail
      LDA   p_expr_type
      JSR   p_type_is_word_value
      BNE   @string_index_word
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
      JSR   p_type_is_word_value
      BNE   @word_index
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
      JSR   p_emit_dimension_range_check
      BCS   @fail
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

p_emit_open_array_range_check:
      LDA   p_check_flags
      AND   #P_CHECK_RANGE
      BEQ   @done
      LDA   #<asm_phx_pha
      LDX   #>asm_phx_pha
      JSR   p_emit_ax_text
      BCS   @fail
      JSR   p_emit_open_array_high
      BCS   @fail
      LDA   #<asm_rhs_word
      LDX   #>asm_rhs_word
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   #<asm_lhs_word
      LDX   #>asm_lhs_word
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   #<asm_check_range_open
      LDX   #>asm_check_range_open
      JMP   p_emit_check_line
@done:
      CLC
@fail:
      RTS

; A/X is a zero-based index and a base pointer is stacked. p_array_len is the
; element stride established by p_type_size.
p_emit_open_array_index:
      LDA   p_array_len+1
      BNE   @general
      LDA   p_array_len
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
      LDA   #0
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   #0
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   #<asm_index_word_separator
      LDX   #>asm_index_word_separator
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
      RTS

p_emit_dimension_range_check:
      LDA   p_check_flags
      AND   #P_CHECK_RANGE
      long_beq @done
      LDX   p_dim_iter
      LDA   p_dim_low_l,X
      STA   p_decimal
      CLC
      ADC   p_dim_extent_l,X
      STA   p_remainder
      LDA   p_dim_low_h,X
      STA   p_decimal+1
      ADC   p_dim_extent_h,X
      STA   p_remainder+1
      LDA   p_remainder
      BNE   :+
      DEC   p_remainder+1
:     DEC   p_remainder
      JMP   p_emit_range_check_bounds
@done:
      CLC
      RTS

p_emit_range_check_bounds:
      LDA   p_decimal+1
      BMI   @signed
      LDA   #<asm_check_range_u
      LDX   #>asm_check_range_u
      BRA   @emit
@signed:
      LDA   #<asm_check_range_s
      LDX   #>asm_check_range_s
@emit:
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   p_decimal+1
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   p_decimal
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   #<asm_check_separator
      LDX   #>asm_check_separator
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   p_remainder+1
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   p_remainder
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   #<asm_check_separator
      LDX   #>asm_check_separator
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   p_statement_line+1
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   p_statement_line
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   #$0A
      JMP   p_emit
@fail:
      RTS

; Check the accumulator value against a named ordinal target. Keeping this in
; the three shared store paths covers globals, frames, fields, array elements,
; INC/DEC, function results, and FOR control variables without parser copies.
p_emit_target_range_check:
      LDA   p_check_flags
      AND   #P_CHECK_RANGE
      BEQ   @done
      LDA   p_target_type
      CMP   #P_TYPE_USER_BASE
      BCC   @done
      SEC
      SBC   #P_TYPE_USER_BASE
      TAX
      CPX   p_type_count
      BCS   @done
      LDA   p_type_kind,X
      CMP   #TYPE_KIND_ORDINAL
      BNE   @done
      LDA   p_type_low_l,X
      STA   p_decimal
      LDA   p_type_low_h,X
      STA   p_decimal+1
      LDA   p_type_high_l,X
      STA   p_remainder
      LDA   p_type_high_h,X
      STA   p_remainder+1
      JMP   p_emit_range_check_bounds
@done:
      CLC
      RTS

; Enforce pointer compatibility once in the shared store paths. Untyped
; Pointer and nil bridge typed pointers; otherwise designated types must match.
p_check_assignment_compatibility:
      LDA   p_target_flags
      AND   #SYM_CONST
      BNE   @fail
      LDA   p_target_type
      JSR   p_type_is_procedural
      BCC   @pointer_target
      LDA   p_expr_type
      CMP   #TYPE_NIL
      BEQ   @ok
      CMP   p_target_type
      BEQ   @ok
      JSR   p_procedural_types_compatible
      BCS   @fail
@ok:
      CLC
      RTS
@pointer_target:
      LDA   p_target_type
      JSR   p_type_is_pointer
      BCC   @ordinary_target
      LDA   p_expr_type
      JSR   p_type_is_pointer
      BCC   @fail
      LDA   p_target_type
      STA   p_left_type
      JMP   p_pointer_types_compatible
@ordinary_target:
      LDA   p_expr_type
      JSR   p_type_is_procedural
      BCS   @fail
      LDA   p_expr_type
      JSR   p_type_is_pointer
      BCS   @fail
      CLC
      RTS
@fail:
      SEC
      RTS

p_procedural_types_compatible:
      LDA   p_target_type
      JSR   p_type_is_procedural
      BCC   @fail
      STX   p_type_iter
      LDA   p_expr_type
      JSR   p_type_is_procedural
      BCC   @fail
      STX   p_routine_iter
      LDY   p_type_iter
      LDA   p_type_kind,Y
      CMP   p_type_kind,X
      BNE   @fail
      LDA   p_type_low_l,Y
      CMP   p_type_low_l,X
      BNE   @fail
      STA   p_digit
      LDA   p_type_low_h,Y
      CMP   p_type_low_h,X
      BNE   @fail
      LDA   p_type_high_l,Y
      CMP   p_type_high_l,X
      BNE   @fail
      LDA   p_type_high_h,Y
      CMP   p_type_high_h,X
      BNE   @fail
      LDX   p_type_iter
      JSR   p_type_param_pointer
      LDY   #0
@copy:
      CPY   p_digit
      BCS   @compare
      LDA   (p_word),Y
      STA   p_routine_param_types,Y
      INY
      BRA   @copy
@compare:
      LDX   p_routine_iter
      JSR   p_type_param_pointer
      LDY   #0
@param:
      CPY   p_digit
      BCS   @ok
      LDA   (p_word),Y
      CMP   p_routine_param_types,Y
      BNE   @fail
      INY
      BRA   @param
@ok:
      CLC
      RTS
@fail:
      SEC
      RTS

; A one-byte ordinal assigned to any two-byte scalar must have a defined high
; byte before range checks or storage. Pointer and nil values are already wide.
p_emit_assignment_word_extension:
      LDA   p_target_type
      JSR   p_type_is_longint
      BCC   @word
      JSR   p_emit_longint_conversion
      BCS   @fail
      LDA   #TYPE_LONGINT
      STA   p_expr_type
      CLC
      RTS
@word:
      LDA   p_target_type
      JSR   p_type_size
      BCS   @fail
      LDA   p_array_len+1
      BNE   @done
      LDA   p_array_len
      CMP   #2
      BNE   @done
      LDA   p_expr_type
      JSR   p_type_size
      BCS   @fail
      LDA   p_array_len+1
      BNE   @done
      LDA   p_array_len
      CMP   #2
      BCS   @done
      LDA   #<asm_index_high_zero
      LDX   #>asm_index_high_zero
      JMP   p_emit_ax_text
@done:
      CLC
@fail:
      RTS

; Convert the current byte/word ordinal expression to the shared four-byte
; scalar ABI. Existing LongInt expressions already hold an address in A/X.
p_emit_longint_conversion:
      LDA   p_expr_type
      JSR   p_type_is_longint
      BCS   @done
      LDA   p_expr_type
      JSR   p_type_is_ordinal
      BCC   @fail
      LDA   p_expr_type
      JSR   p_type_size
      BCS   @fail
      LDA   p_array_len+1
      BNE   @fail
      LDA   p_array_len
      CMP   #1
      BNE   @word_value
      LDA   #<asm_long_from_byte
      LDX   #>asm_long_from_byte
      BRA   @emit
@word_value:
      CMP   #2
      BNE   @fail
      LDA   p_expr_type
      JSR   p_type_is_signed_ordinal
      BEQ   @uword
      LDA   #<asm_long_from_integer
      LDX   #>asm_long_from_integer
      BRA   @emit
@uword:
      LDA   #<asm_long_from_uword
      LDX   #>asm_long_from_uword
@emit:
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   #TYPE_LONGINT
      STA   p_expr_type
@done:
      CLC
      RTS
@fail:
      SEC
      RTS

; Emit one bitwise operator: and/or/xor/shl/shr for byte or word operands.
; The result keeps the left operand's ordinal type, so a Byte mask stays a Byte
; instead of collapsing to Boolean; Boolean operands still yield Boolean because
; that is their own type.
p_emit_bitwise:
      LDA   p_left_type
      JSR   p_type_is_ordinal
      BCC   @fail
      LDA   p_expr_type
      JSR   p_type_is_ordinal
      BCC   @fail
      LDA   p_left_type
      JSR   p_type_is_word_value
      BNE   @word
      LDA   p_expr_type
      JSR   p_type_is_word_value
      BNE   @word
      JSR   p_bitwise_recipe
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   p_left_type
      STA   p_expr_type
      CLC
      RTS
@word:
      JSR   p_emit_wide_operands
      BCS   @fail
      JSR   p_bitwise_recipe_word
      JSR   p_emit_ax_text
      BCS   @fail
      JSR   p_binary_is_signed
      BEQ   :+
      LDA   #TYPE_INTEGER
      BRA   :++
:     LDA   #TYPE_WORD
:     STA   p_expr_type
      CLC
      RTS
@fail:
      SEC
      RTS

; A shift keeps the left operand's width, so its count is always a byte and the
; word forms cannot reuse the symmetric wide-operand setup.
p_bitwise_recipe:
      LDA   p_operator
      CMP   #'O'
      BEQ   @or
      CMP   #'X'
      BEQ   @xor
      CMP   #'L'
      BEQ   @shl
      CMP   #'R'
      BEQ   @shr
      LDA   #<asm_and
      LDX   #>asm_and
      RTS
@or:
      LDA   #<asm_or
      LDX   #>asm_or
      RTS
@xor:
      LDA   #<asm_xor
      LDX   #>asm_xor
      RTS
@shl:
      LDA   #<asm_shl
      LDX   #>asm_shl
      RTS
@shr:
      LDA   #<asm_shr
      LDX   #>asm_shr
      RTS

p_bitwise_recipe_word:
      LDA   p_operator
      CMP   #'O'
      BEQ   @or
      CMP   #'X'
      BEQ   @xor
      CMP   #'L'
      BEQ   @shl
      CMP   #'R'
      BEQ   @shr
      LDA   #<asm_and_word
      LDX   #>asm_and_word
      RTS
@or:
      LDA   #<asm_or_word
      LDX   #>asm_or_word
      RTS
@xor:
      LDA   #<asm_xor_word
      LDX   #>asm_xor_word
      RTS
@shl:
      LDA   #<asm_shl_word
      LDX   #>asm_shl_word
      RTS
@shr:
      LDA   #<asm_shr_word
      LDX   #>asm_shr_word
      RTS

p_emit_scalar4_prologue:
      LDA   #<asm_scalar4_prologue
      LDX   #>asm_scalar4_prologue
      JMP   p_emit_ax_text

; Emit the shared four-byte operand prologue, then the call recipe in A/X.
; Used by the Real operators, whose operands are already both scalar4.
p_emit_scalar4_binary:
      PHA
      PHX
      JSR   p_emit_scalar4_prologue
      BCS   @drop
      PLX
      PLA
      JMP   p_emit_ax_text
@drop:
      PLX
      PLA
      SEC
      RTS

; Emit one LongInt binary operator whose recipe address is in A/X: promote the
; right operand to 32 bits, emit the recipe, and type the result. Every LongInt
; operator shares this shape, so the arithmetic parsers only choose a recipe.
p_emit_longint_binary:
      PHA
      PHX
      JSR   p_emit_longint_conversion
      BCS   @drop
      JSR   p_emit_scalar4_prologue
      BCS   @drop
      PLX
      PLA
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   #TYPE_LONGINT
      STA   p_expr_type
      CLC
      RTS
@drop:
      PLX
      PLA
@fail:
      SEC
      RTS

p_parse_if:
      JSR   p_parse_expression
      long_bcs @fail
      LDA   #<kw_then
      LDX   #>kw_then
      JSR   p_expect_ax_word
      BCS   @fail
      LDX   p_control_depth
      CPX   #P_CONTROL_CAP
      BCS   @fail
      LDA   #CONTROL_IF
      STA   p_control_kind,X
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
      LDX   #>kw_else
      JSR   p_expect_ax_word
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
      JMP   p_emit_label
@no_else:
      LDX   p_control_depth
      JSR   p_load_control_a
      JMP   p_emit_label
@output:
      INC   p_emit_error
@fail:
      SEC
      RTS

p_parse_while:
      LDX   p_control_depth
      CPX   #P_CONTROL_CAP
      BCS   @fail
      LDA   #CONTROL_WHILE
      STA   p_control_kind,X
      JSR   p_new_label
      JSR   p_store_control_a
      JSR   p_emit_label
      BCS   @output
      JSR   p_parse_expression
      BCS   @fail
      LDA   #<kw_do
      LDX   #>kw_do
      JSR   p_expect_ax_word
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
      JMP   p_emit_label
@output:
      INC   p_emit_error
@fail:
      SEC
      RTS

      .segment "CODE"
p_parse_repeat:
      LDX   p_control_depth
      CPX   #P_CONTROL_CAP
      long_bcs @fail
      LDA   #CONTROL_REPEAT
      STA   p_control_kind,X
      JSR   p_new_label
      JSR   p_store_control_a
      JSR   p_emit_label
      long_bcs @output
      JSR   p_new_label
      LDX   p_control_depth
      JSR   p_store_control_b
      JSR   p_new_label
      LDX   p_control_depth
      JSR   p_store_control_continue
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
      LDX   #>kw_until
      JSR   p_expect_ax_word
      BCS   @fail_depth
      LDX   p_control_depth
      DEX
      JSR   p_load_control_continue
      JSR   p_emit_label
      BCS   @output_depth
      JSR   p_parse_expression
      BCS   @fail_depth
      DEC   p_control_depth
      LDX   p_control_depth
      JSR   p_load_control_a
      JSR   p_emit_branch_false
      BCS   @output
      LDX   p_control_depth
      JSR   p_load_control_b
      JMP   p_emit_label
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
@output_depth:
      DEC   p_control_depth
@output:
      INC   p_emit_error
      BRA   @fail

p_parse_for:
      LDX   p_control_depth
      CPX   #P_CONTROL_CAP
      long_bcs @fail
      LDA   #CONTROL_FOR
      STA   p_control_kind,X
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
      long_bcc @fail
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
      LDX   #>kw_do
      JSR   p_expect_ax_word
      BCS   @fail
      JSR   p_new_label
      LDX   p_control_depth
      JSR   p_store_control_b
      JSR   p_emit_branch_false
      BCS   @output
      JSR   p_new_label
      LDX   p_control_depth
      JSR   p_store_control_continue
      INC   p_control_depth
      JSR   p_parse_statement
      DEC   p_control_depth
      BCS   @fail
      LDX   p_control_depth
      JSR   p_load_control_continue
      JSR   p_emit_label
      BCS   @output
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
      JSR   p_type_is_word_value
      BEQ   @byte
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
      JSR   p_emit_assignment_word_extension
      BCS   @fail
      JSR   p_emit_target_range_check
      BCS   @fail
      LDA   p_target_flags
      long_bne p_emit_store_frame_target
      LDA   p_target_type
      JSR   p_type_is_word_value
      BEQ   @byte
      LDA   p_expr_type
      JSR   p_type_is_word_value
      BEQ   @extend
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
      JSR   p_type_is_word_value
      BEQ   @byte
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
      JSR   p_emit_add_sub_check
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
      LDX   #>kw_in
      JSR   p_expect_ax_word
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
      JSR   p_type_is_word_value
      long_bne @fail
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
      BCC   @xor_keyword
      LDA   #<kw_or
      LDX   #>kw_or
      JSR   p_expect_ax_word
      long_bcs @fail
      LDA   #'O'
      BRA   @have
@xor_keyword:
      LDA   #<kw_xor
      LDX   #>kw_xor
      JSR   p_peek_keyword
      BCC   @symbol_operator
      LDA   #<kw_xor
      LDX   #>kw_xor
      JSR   p_expect_ax_word
      long_bcs @fail
      LDA   #'X'
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
      LDA   p_left_type
      JSR   p_type_is_pointer
      long_bcs @fail
      LDA   p_expr_type
      JSR   p_type_is_pointer
      long_bcs @fail
      LDA   p_operator
      CMP   #'O'
      long_beq @logical_or
      CMP   #'X'
      long_beq @logical_xor
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
@logical_xor:
      JSR   p_emit_bitwise
      long_bcs @fail
      JMP   @operator
@ordinal:
      LDA   p_left_type
      JSR   p_type_is_real
      BCC   @right_real
      LDA   p_expr_type
      JSR   p_type_is_real
      long_bcc @fail
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
      JSR   p_emit_scalar4_binary
      long_bcs @fail
      LDA   #TYPE_REAL
      STA   p_expr_type
      JMP   @operator
@right_real:
      LDA   p_expr_type
      JSR   p_type_is_real
      long_bcs @fail
      LDA   p_left_type
      JSR   p_type_is_longint
      long_bcs @left_longint
      LDA   p_expr_type
      JSR   p_type_is_longint
      long_bcs @fail
      LDA   p_left_type
      JSR   p_type_is_word_value
      BNE   @wide
      LDA   p_expr_type
      JSR   p_type_is_word_value
      BNE   @wide
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
      JSR   p_emit_add_sub_check
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
      JSR   p_emit_add_sub_check
      BCS   @fail
      JSR   p_binary_is_signed
      BEQ   :+
      LDA   #TYPE_INTEGER
      BRA   :++
:     LDA   #TYPE_WORD
:     STA   p_expr_type
      JMP   @operator
@left_longint:
      LDA   p_operator
      CMP   #'+'
      BNE   @longint_subtract
      LDA   #<asm_longint_add
      LDX   #>asm_longint_add
      BRA   @longint_emit
@longint_subtract:
      LDA   #<asm_longint_subtract
      LDX   #>asm_longint_subtract
@longint_emit:
      JSR   p_emit_longint_binary
      BCS   @fail
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
      LDX   #>kw_and
      JSR   p_expect_ax_word
      long_bcs @fail
      LDA   #'A'
      BRA   @have
@modulo_keyword:
      LDA   #<kw_mod
      LDX   #>kw_mod
      JSR   p_peek_keyword
      BCC   @divide_keyword
      LDA   #<kw_mod
      LDX   #>kw_mod
      JSR   p_expect_ax_word
      long_bcs @fail
      LDA   #'M'
      BRA   @have
@divide_keyword:
      LDA   #<kw_div
      LDX   #>kw_div
      JSR   p_peek_keyword
      BCC   @shl_keyword
      LDA   #<kw_div
      LDX   #>kw_div
      JSR   p_expect_ax_word
      long_bcs @fail
      LDA   #'D'
      BRA   @have
@shl_keyword:
      LDA   #<kw_shl
      LDX   #>kw_shl
      JSR   p_peek_keyword
      BCC   @shr_keyword
      LDA   #<kw_shl
      LDX   #>kw_shl
      JSR   p_expect_ax_word
      long_bcs @fail
      LDA   #'L'
      BRA   @have
@shr_keyword:
      LDA   #<kw_shr
      LDX   #>kw_shr
      JSR   p_peek_keyword
      BCC   @multiply
      LDA   #<kw_shr
      LDX   #>kw_shr
      JSR   p_expect_ax_word
      long_bcs @fail
      LDA   #'R'
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
      LDA   p_left_type
      JSR   p_type_is_pointer
      long_bcs @fail
      LDA   p_expr_type
      JSR   p_type_is_pointer
      long_bcs @fail
      LDA   p_operator
      CMP   #'A'
      long_beq @logical_and
      CMP   #'L'
      long_beq @shift
      CMP   #'R'
      long_beq @shift
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
      LDA   p_left_type
      JSR   p_type_is_longint
      long_bcs @longint_multiply
      LDA   p_expr_type
      JSR   p_type_is_longint
      long_bcs @fail
      JSR   p_emit_wide_operands
      long_bcs @fail
      LDA   #<asm_multiply_word
      LDX   #>asm_multiply_word
      JSR   p_emit_ax_text
      long_bcs @fail
      JSR   p_emit_multiply_check
      long_bcs @fail
      JSR   p_binary_is_signed
      BEQ   :+
      LDA   #TYPE_INTEGER
      BRA   :++
:     LDA   #TYPE_WORD
:     STA   p_expr_type
      JMP   @operator
@left_real:
      LDA   p_expr_type
      JSR   p_type_is_real
      BCS   @both_real
      LDA   p_expr_type
      JSR   p_type_is_word_value
      BNE   @real_word_right
      LDA   #<asm_real_multiply_byte_right
      LDX   #>asm_real_multiply_byte_right
      BRA   @emit_real
@real_word_right:
      LDA   #<asm_real_multiply_word_right
      LDX   #>asm_real_multiply_word_right
      BRA   @emit_real
@right_real:
      LDA   p_left_type
      JSR   p_type_is_word_value
      BNE   @word_real_right
      LDA   #<asm_byte_multiply_real
      LDX   #>asm_byte_multiply_real
      BRA   @emit_real
@word_real_right:
      LDA   #<asm_word_multiply_real
      LDX   #>asm_word_multiply_real
      BRA   @emit_real
@both_real:
      JSR   p_emit_scalar4_prologue
      long_bcs @fail
      LDA   #<asm_real_multiply
      LDX   #>asm_real_multiply
@emit_real:
      JSR   p_emit_ax_text
      long_bcs @fail
      LDA   #TYPE_REAL
      STA   p_expr_type
      JMP   @operator
@logical_and:
@shift:
      JSR   p_emit_bitwise
      long_bcs @fail
      JMP   @operator
@integer_divide:
      LDA   p_left_type
      JSR   p_type_is_real
      long_bcs @fail
      LDA   p_expr_type
      JSR   p_type_is_real
      long_bcs @fail
      LDA   p_left_type
      JSR   p_type_is_longint
      long_bcs @longint_divide
      LDA   p_expr_type
      JSR   p_type_is_longint
      long_bcs @fail
      JSR   p_emit_zero_check
      long_bcs @fail
      JSR   p_emit_divide_overflow_check
      long_bcs @fail
      LDA   p_left_type
      JSR   p_type_is_word_value
      BNE   @integer_divide_word
      LDA   p_expr_type
      JSR   p_type_is_word_value
      BNE   @integer_divide_word
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
      JSR   p_binary_is_signed
      BEQ   @integer_divide_unsigned
      LDA   #<asm_divide_word
      LDX   #>asm_divide_word
      JSR   p_emit_ax_text
      long_bcs @fail
      LDA   #TYPE_INTEGER
      STA   p_expr_type
      JMP   @operator
@integer_divide_unsigned:
      LDA   #<asm_divide_uword
      LDX   #>asm_divide_uword
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
      JSR   p_type_is_word_value
      BNE   @divide_real_word
      LDA   #<asm_real_divide_byte
      LDX   #>asm_real_divide_byte
      BRA   @emit_divide_real
@divide_real_word:
      JSR   p_emit_scalar4_prologue
      long_bcs @fail
      LDA   #<asm_real_divide_word
      LDX   #>asm_real_divide_word
      BRA   @emit_divide_real
@divide_right_real:
      LDA   p_left_type
      JSR   p_type_is_word_value
      BNE   @divide_word_real
      LDA   #<asm_byte_divide_real
      LDX   #>asm_byte_divide_real
      BRA   @emit_divide_real
@divide_word_real:
      LDA   #<asm_word_divide_real
      LDX   #>asm_word_divide_real
      BRA   @emit_divide_real
@divide_both_real:
      JSR   p_emit_scalar4_prologue
      long_bcs @fail
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
      JSR   p_type_is_longint
      long_bcs @longint_modulo
      LDA   p_expr_type
      JSR   p_type_is_longint
      long_bcs @fail
      JSR   p_emit_zero_check
      BCS   @fail
      JSR   p_emit_divide_overflow_check
      BCS   @fail
      LDA   p_left_type
      JSR   p_type_is_word_value
      BNE   @modulo_word
      LDA   p_expr_type
      JSR   p_type_is_word_value
      BNE   @modulo_word
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
      JSR   p_binary_is_signed
      BEQ   @modulo_unsigned
      LDA   #<asm_modulo_word
      LDX   #>asm_modulo_word
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   #TYPE_INTEGER
      STA   p_expr_type
      JMP   @operator
@modulo_unsigned:
      LDA   #<asm_modulo_uword
      LDX   #>asm_modulo_uword
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   #TYPE_WORD
      STA   p_expr_type
      JMP   @operator
; LANGRT raises the divide-by-zero status for the 32-bit forms, so the inline
; ordinal zero checks do not apply here.
@longint_multiply:
      LDA   #<asm_longint_multiply
      LDX   #>asm_longint_multiply
      BRA   @longint_emit
@longint_divide:
      LDA   #<asm_longint_divide
      LDX   #>asm_longint_divide
      BRA   @longint_emit
@longint_modulo:
      LDA   #<asm_longint_modulo
      LDX   #>asm_longint_modulo
@longint_emit:
      JSR   p_emit_longint_binary
      BCS   @fail
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
      CMP   #'@'
      long_beq @address
      CMP   #'+'
      BEQ   @unary_plus
      CMP   #'-'
      long_bne @not
      JSR   p_next
      JSR   p_parse_factor
      long_bcs @fail
      LDA   p_expr_type
      JSR   p_type_is_pointer
      long_bcs @fail
      LDA   p_expr_type
      JSR   p_type_is_real
      long_bcs @negate_real
      LDA   p_expr_type
      JSR   p_type_is_longint
      long_bcs @negate_longint
      LDA   p_expr_type
      JSR   p_type_is_word_value
      long_bne @negate_word
      LDA   #<asm_index_high_zero
      LDX   #>asm_index_high_zero
      JSR   p_emit_ax_text
      long_bcs @fail
      BRA   @emit_negate_word
@negate_word:
      JSR   p_emit_negate_check
      long_bcs @fail
@emit_negate_word:
      LDA   #<asm_negate_word
      LDX   #>asm_negate_word
      JSR   p_emit_ax_text
      long_bcs @fail
      LDA   #TYPE_INTEGER
      STA   p_expr_type
      CLC
      RTS
@negate_real:
      LDA   #<asm_negate_real
      LDX   #>asm_negate_real
      JMP   p_emit_ax_text
@negate_longint:
      LDA   #<asm_negate_longint
      LDX   #>asm_negate_longint
      JMP   p_emit_ax_text
@unary_plus:
      JSR   p_next
      JSR   p_parse_factor
      BCS   @fail
      LDA   p_expr_type
      JSR   p_type_is_pointer
      BCS   @fail
      CLC
      RTS
@address:
      JSR   p_next
      JSR   p_capture_identifier
      BCS   @fail
      JSR   p_parse_designator_address
      BCS   @fail
      LDA   #TYPE_POINTER
      STA   p_expr_type
      CLC
      RTS
@not:
      LDA   #<kw_not
      LDX   #>kw_not
      JSR   p_peek_keyword
      BCC   @ordinary
      LDA   #<kw_not
      LDX   #>kw_not
      JSR   p_expect_ax_word
      BCS   @fail
      JSR   p_parse_factor
      BCS   @fail
      LDA   p_expr_type
      JSR   p_type_is_pointer
      BCS   @fail
      LDA   #<asm_not
      LDX   #>asm_not
      JMP   p_emit_ax_text
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
      JMP   p_expect_char
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

      .segment "NPCFE_CODE"
; Materialize a four-byte scalar literal in read-only storage and return its
; address, matching the ABI used by Real and structured values.
p_emit_long_literal:
      JSR   p_new_label
      LDA   #<asm_rodata
      LDX   #>asm_rodata
      JSR   p_emit_ax_text
      BCS   @fail
      JSR   p_emit_label
      BCS   @fail
      LDA   #<asm_const_bytes
      LDX   #>asm_const_bytes
      JSR   p_emit_ax_text
      BCS   @fail
      STZ   p_set_start
@byte:
      LDX   p_set_start
      CPX   #4
      BCS   @done
      CPX   #0
      BEQ   :+
      LDA   #','
      JSR   p_emit
      BCS   @fail
:     LDA   #'$'
      JSR   p_emit
      BCS   @fail
      LDA   p_decimal,X
      JSR   p_emit_hex_byte
      BCS   @fail
      INC   p_set_start
      BRA   @byte
@done:
      LDA   #$0A
      JSR   p_emit
      BCS   @fail
      LDA   #<asm_code
      LDX   #>asm_code
      JSR   p_emit_ax_text
      BCS   @fail
      JMP   p_emit_set_label_address
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
      LDA   #<kw_implementation
      LDX   #>kw_implementation
      JSR   p_ident_is
      BCS   @done
      JSR   p_is_routine_keyword
      BCS   @done
      JMP   @name
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
      JMP   p_emit
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
      STZ   p_const_sign
      JSR   p_skip_ws
      JSR   p_peek
      long_bcc @fail
      CMP   #'+'
      BEQ   @positive
      CMP   #'-'
      BNE   @value
      LDA   #2
      BRA   @sign
@positive:
      LDA   #1
@sign:
      STA   p_const_sign
      JSR   p_next
      JSR   p_skip_ws
      JSR   p_peek
      long_bcc @fail
@value:
      CMP   #'^'
      long_beq @control
      CMP   #'#'
      long_beq @numeric_character
      CMP   #$27
      BEQ   @character
      CMP   #'$'
      long_beq @hex
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
      JMP   @apply_sign
@false:
      LDA   #<kw_false
      LDX   #>kw_false
      JSR   p_ident_is
      BCC   @named
      STZ   p_decimal
      STZ   p_decimal+1
      LDA   #TYPE_BOOLEAN
      STA   p_decl_type
      JMP   @apply_sign
@named:
      JSR   p_const_find
      long_bcc @fail
      STX   p_const_iter
      JSR   p_const_meta_pointer
      LDY   #2
      LDA   (p_word),Y
      STA   p_decimal
      INY
      LDA   (p_word),Y
      STA   p_decimal+1
      LDY   #1
      LDA   (p_word),Y
      STA   p_decl_type
      JMP   @apply_sign
@character:
      JSR   p_next
      JSR   p_next
      long_bcc @fail
      STA   p_decimal
      STZ   p_decimal+1
      LDA   #$27
      JSR   p_expect_char
      long_bcs @fail
      LDA   #TYPE_CHAR
      STA   p_decl_type
      JMP   @apply_sign
@control:
      JSR   p_next
      JSR   p_next
      long_bcc @fail
      AND   #$1F
      STA   p_decimal
      STZ   p_decimal+1
      LDA   #TYPE_CHAR
      STA   p_decl_type
      JMP   @apply_sign
@numeric_character:
      JSR   p_next
      JSR   p_parse_decimal_literal
      long_bcs @fail
      LDA   p_decimal+1
      long_bne @fail
      LDA   #TYPE_CHAR
      STA   p_decl_type
      JMP   @apply_sign
@hex:
      JSR   p_next
      JSR   p_parse_hex_literal
      long_bcs @fail
      LDA   p_decimal+2
      ORA   p_decimal+3
      long_bne @fail
      LDA   #TYPE_BYTE
      LDX   p_decimal+1
      BEQ   :+
      LDA   #TYPE_WORD
:     STA   p_decl_type
      JMP   @apply_sign
@decimal:
      JSR   p_parse_decimal_literal
      long_bcs @fail
      ; Constant metadata is still the compact 16-bit form. Reject wider
      ; values instead of silently truncating them until LongInt constants
      ; gain a four-byte metadata record.
      LDA   p_decimal+2
      ORA   p_decimal+3
      long_bne @fail
      LDA   p_decimal+1
      BNE   @word
      LDA   p_decl_type
      CMP   #TYPE_WORD
      BNE   @ok
      LDA   #TYPE_BYTE
      STA   p_decl_type
@ok:
      BRA   @apply_sign
@word:
      LDA   #TYPE_WORD
      STA   p_decl_type
@apply_sign:
      LDA   p_const_sign
      BEQ   @success
      LDA   p_decl_type
      CMP   #TYPE_BYTE
      BEQ   @numeric
      CMP   #TYPE_WORD
      BEQ   @numeric
      CMP   #TYPE_INTEGER
      BEQ   @numeric
      CMP   #P_TYPE_USER_BASE
      BCC   @fail
      SEC
      SBC   #P_TYPE_USER_BASE
      TAX
      CPX   p_type_count
      BCS   @fail
      LDA   p_type_kind,X
      CMP   #TYPE_KIND_ORDINAL
      BNE   @fail
@numeric:
      LDA   p_const_sign
      CMP   #2
      BNE   @success
      LDA   p_decimal+1
      CMP   #$80
      BCC   @negate
      BNE   @fail
      LDA   p_decimal
      BNE   @fail
@negate:
      SEC
      LDA   #0
      SBC   p_decimal
      STA   p_decimal
      LDA   #0
      SBC   p_decimal+1
      STA   p_decimal+1
      LDA   p_decl_type
      CMP   #P_TYPE_USER_BASE
      BCS   @success
      LDA   #TYPE_INTEGER
      STA   p_decl_type
@success:
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
      JSR   p_const_meta_pointer
      LDY   #0
      LDA   (p_word),Y
      CMP   p_saved_ident_len
      BNE   @next
      JSR   p_const_name_pointer
      LDY   #0
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
      JSR   p_const_meta_pointer
      LDY   #0
      LDA   p_saved_ident_len
      STA   (p_word),Y
      INY
      LDA   p_decl_type
      STA   (p_word),Y
      INY
      LDA   p_decimal
      STA   (p_word),Y
      INY
      LDA   p_decimal+1
      STA   (p_word),Y
      LDX   p_const_count
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
      JSR   p_const_meta_pointer
      LDY   #0
      LDA   (p_word),Y
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
      JSR   p_const_meta_pointer
      LDY   #1
      LDA   (p_word),Y
      LDX   p_const_iter
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

p_const_meta_pointer:
      TXA
      ASL
      ASL
      CLC
      ADC   #<P_STATE_CONST_META_OFFSET
      STA   p_state_offset
      LDA   #>P_STATE_CONST_META_OFFSET
      ADC   #0
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
      LDA   p_type_work_kind
      CMP   #TYPE_KIND_PROCEDURE
      BEQ   @alias
      CMP   #TYPE_KIND_FUNCTION
      BNE   @new
@alias:
      LDA   p_decl_type
      CMP   #P_TYPE_USER_BASE
      BCC   @new
      SEC
      SBC   #P_TYPE_USER_BASE
      TAX
      JSR   p_type_param_pointer
      LDY   #0
@alias_param:
      CPY   p_type_work_low
      BCS   @new
      LDA   (p_word),Y
      STA   p_routine_param_types,Y
      INY
      BRA   @alias_param
@new:
      STZ   p_digit
      LDA   p_type_decl_len
      BEQ   @append
      JSR   p_type_find_decl_name
      BCC   @append
      LDA   p_type_kind,X
      BNE   @fail
      STX   p_type_iter
      INC   p_digit
      BRA   @meta
@append:
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
      LDA   p_type_work_kind
      CMP   #TYPE_KIND_PROCEDURE
      BEQ   @signature
      CMP   #TYPE_KIND_FUNCTION
      BNE   @type_id
@signature:
      JSR   p_store_type_signature
      LDX   p_type_iter
@type_id:
      TXA
      CLC
      ADC   #P_TYPE_USER_BASE
      STA   p_decl_type
      LDA   p_digit
      BNE   @done
      INC   p_type_count
@done:
      CLC
      RTS
@fail:
      SEC
      RTS

; Complete an unresolved row when its real declaration arrives.
      .segment "CODE"
p_type_find_decl_name:
      LDX   #0
@type:
      CPX   p_type_count
      BCS   @missing
      STX   p_type_iter
      LDA   p_type_name_len,X
      CMP   p_type_decl_len
      BNE   @next
      JSR   p_type_name_pointer
      LDY   #0
@compare:
      CPY   p_type_decl_len
      BCS   @found
      LDA   (p_word),Y
      CMP   p_type_decl_name,Y
      BNE   @next
      INY
      BRA   @compare
@found:
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

; Reserve the captured pointer target as an unresolved user type. Its later
; declaration fills this row, which handles self-referential records without
; a second name pool or fixup table.
p_type_add_forward:
      LDX   p_type_count
      CPX   #P_TYPE_CAP
      BCS   @fail
      STX   p_type_iter
      JSR   p_type_name_pointer
      LDY   #0
@copy:
      CPY   p_ident_len
      BCS   @meta
      LDA   p_ident,Y
      STA   (p_word),Y
      INY
      BRA   @copy
@meta:
      LDX   p_type_iter
      LDA   p_ident_len
      STA   p_type_name_len,X
      STZ   p_type_kind,X
      STZ   p_type_size_l,X
      STZ   p_type_size_h,X
      STZ   p_type_low_l,X
      STZ   p_type_low_h,X
      STZ   p_type_high_l,X
      STZ   p_type_high_h,X
      STZ   p_type_element,X
      TXA
      CLC
      ADC   #P_TYPE_USER_BASE
      INC   p_type_count
      CLC
      RTS
@fail:
      SEC
      RTS

      .segment "CODE"

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

; True for scalar values carried in A/X, including named two-byte ordinals.
p_type_is_word_value:
      JSR   p_type_size
      BCS   @no
      LDA   p_array_len+1
      BNE   @no
      LDA   p_array_len
      CMP   #2
      BNE   @no
@yes:
      LDA   #1
      RTS
@no:
      LDA   #0
      RTS

; Normal Pascal pointers are two-byte low-RAM values. XRAM remains exposed
; only through the opaque NovaMemory handles.
      .segment "NPCFE_CODE"
p_type_is_pointer:
p_type_pointer_element:
      CMP   #TYPE_POINTER
      BEQ   @untyped
      CMP   #TYPE_NIL
      BEQ   @untyped
      CMP   #P_TYPE_USER_BASE
      BCC   @no
      SEC
      SBC   #P_TYPE_USER_BASE
      TAX
      CPX   p_type_count
      BCS   @no
      LDA   p_type_kind,X
      CMP   #TYPE_KIND_POINTER
      BNE   @no
      LDA   p_type_element,X
      SEC
      RTS
@untyped:
      LDA   #TYPE_NONE
      SEC
      RTS
@no:
      CLC
      RTS

; Carry set with A equal to the element type for an open-array descriptor.
p_type_open_element:
      CMP   #P_TYPE_USER_BASE
      BCC   @no
      SEC
      SBC   #P_TYPE_USER_BASE
      TAX
      CPX   p_type_count
      BCS   @no
      LDA   p_type_kind,X
      CMP   #TYPE_KIND_OPEN_ARRAY
      BNE   @no
      LDA   p_type_element,X
      SEC
      RTS
@no:
      CLC
      RTS

; Carry set with X selecting a PROCEDURE/FUNCTION type descriptor.
p_type_is_procedural:
      CMP   #P_TYPE_USER_BASE
      BCC   @no
      SEC
      SBC   #P_TYPE_USER_BASE
      TAX
      CPX   p_type_count
      BCS   @no
      LDA   p_type_kind,X
      CMP   #TYPE_KIND_PROCEDURE
      BEQ   @yes
      CMP   #TYPE_KIND_FUNCTION
      BNE   @no
@yes:
      SEC
      RTS
@no:
      CLC
      RTS

; Match a concrete routine in X against the procedural type in p_target_type.
; Parameter types are already normalized by the shared formal parser; the ABI
; contract is kind, arity, VAR mask, width mask, and result type.
p_routine_matches_target_type:
      STX   p_routine_iter
      LDA   p_target_type
      JSR   p_type_is_procedural
      BCS   @type
      SEC
      RTS
@type:
      STX   p_type_iter
      LDA   p_type_kind,X
      CMP   #TYPE_KIND_PROCEDURE
      BNE   @function
      LDA   #0
      BRA   @kind
@function:
      LDA   #2
@kind:
      LDY   p_routine_iter
      EOR   p_routine_kind_table,Y
      AND   #P_ROUTINE_KIND_MASK
      BNE   @fail
      LDA   p_type_low_l,X
      CMP   p_routine_params,Y
      BNE   @fail
      LDA   p_type_low_h,X
      CMP   p_routine_refmask,Y
      BNE   @fail
      LDA   p_type_high_l,X
      STA   p_digit
      TYA
      TAX
      JSR   p_routine_width_pointer
      LDY   #0
      LDA   (p_word),Y
      CMP   p_digit
      BNE   @fail
      LDX   p_routine_iter
      LDY   p_type_iter
      LDA   p_routine_return,X
      CMP   p_type_high_h,Y
      BNE   @fail
      LDA   p_routine_owner,X
      CMP   #$FF
      BNE   @fail
      LDX   p_type_iter
      LDA   p_type_low_l,X
      STA   p_digit
      JSR   p_type_param_pointer
      LDY   #0
@copy_param:
      CPY   p_digit
      BCS   @compare_params
      LDA   (p_word),Y
      STA   p_routine_param_types,Y
      INY
      BRA   @copy_param
@compare_params:
      LDX   p_routine_iter
      JSR   p_routine_param_pointer
      LDY   #0
@compare_param:
      CPY   p_digit
      BCS   @ok
      LDA   (p_word),Y
      CMP   p_routine_param_types,Y
      BNE   @fail
      INY
      BRA   @compare_param
@ok:
      CLC
      RTS
@fail:
      SEC
      RTS

; Carry clear means the two pointer types are assignment/equality compatible.
p_pointer_types_compatible:
      LDA   p_left_type
      JSR   p_type_pointer_element
      BCC   @fail
      STA   p_digit
      LDA   p_expr_type
      JSR   p_type_pointer_element
      BCC   @fail
      STA   p_remainder
      LDA   p_digit
      BEQ   @ok
      LDA   p_remainder
      BEQ   @ok
      CMP   p_digit
      BNE   @fail
@ok:
      CLC
      RTS
@fail:
      SEC
      RTS

; Carry set for scalar ordinal values, excluding pointers and aggregates.
p_type_is_ordinal:
      CMP   #TYPE_BYTE
      BEQ   @yes
      CMP   #TYPE_WORD
      BEQ   @yes
      CMP   #TYPE_CHAR
      BEQ   @yes
      CMP   #TYPE_INTEGER
      BEQ   @yes
      CMP   #TYPE_LONGINT
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
@yes:
      SEC
      RTS
@no:
      CLC
      RTS

      .segment "NPCFE_CODE"

; Return nonzero for signed ordinal types. Named aliases/subranges inherit
; signedness from Integer or from a negative lower bound.
p_type_is_signed_ordinal:
      CMP   #TYPE_INTEGER
      BEQ   @yes
      CMP   #TYPE_LONGINT
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
      CMP   #TYPE_INTEGER
      BEQ   @yes
      CMP   #TYPE_LONGINT
      BEQ   @yes
      LDA   p_type_low_h,X
      BPL   @no
@yes:
      LDA   #1
      RTS
@no:
      LDA   #0
      RTS

; Return the byte size for A in p_array_len.
      .segment "NPCFE_CODE"
p_type_size:
      CMP   #P_TYPE_USER_BASE
      BCS   @user
      CMP   #TYPE_WORD
      BEQ   @word
      CMP   #TYPE_INTEGER
      BEQ   @word
      CMP   #TYPE_STRING_REF
      BEQ   @word
      CMP   #TYPE_SET_REF
      BEQ   @word
      CMP   #TYPE_POINTER
      BEQ   @word
      CMP   #TYPE_NIL
      BEQ   @word
      CMP   #TYPE_REAL
      BEQ   @real
      CMP   #TYPE_LONGINT
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

      .segment "CODE"
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
      LDA   #<kw_implementation
      LDX   #>kw_implementation
      JSR   p_ident_is
      BCS   @done
      JSR   p_is_routine_keyword
      BCS   @done
      BRA   @definition
@done:
      LDX   #0
@resolved:
      CPX   p_type_count
      BCS   @success
      LDA   p_type_kind,X
      BEQ   @fail
      INX
      BRA   @resolved
@success:
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
      ; `packed` is accepted and ignored: Nova already lays structured types out
      ; without padding, so the Turbo keyword has nothing left to request.
      LDA   #<kw_packed
      LDX   #>kw_packed
      JSR   p_peek_keyword
      BCC   @unpacked
      LDA   #<kw_packed
      LDX   #>kw_packed
      JSR   p_expect_ax_word
      long_bcs @fail
@unpacked:
      JSR   p_skip_ws
      JSR   p_peek
      long_bcc @fail
      CMP   #'('
      long_beq p_parse_enum_type
      CMP   #'^'
      long_beq p_parse_pointer_type
      CMP   #'+'
      long_beq @subrange
      CMP   #'-'
      long_beq @subrange
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
      BCC   @integer
      LDA   #TYPE_WORD
      JMP   @scalar
@integer:
      LDA   #<kw_integer
      LDX   #>kw_integer
      JSR   p_ident_is
      long_bcc @longint
      LDA   #TYPE_INTEGER
      JMP   @scalar
@longint:
      LDA   #<kw_longint
      LDX   #>kw_longint
      JSR   p_ident_is
      long_bcc @real
      LDA   #TYPE_LONGINT
      JMP   @scalar
@real:
      LDA   #<kw_real
      LDX   #>kw_real
      JSR   p_ident_is
      BCC   @string
      LDA   #TYPE_REAL
      JMP   @scalar
@pointer:
      LDA   #TYPE_POINTER
      STA   p_type_work_element
      STA   p_decl_type
      LDA   #TYPE_KIND_POINTER
      STA   p_type_work_kind
      LDA   #2
      STA   p_array_len
      STZ   p_array_len+1
      CLC
      RTS
@string:
      LDA   #<kw_string
      LDX   #>kw_string
      JSR   p_ident_is
      long_bcs p_parse_string_type
      LDA   #<kw_pointer
      LDX   #>kw_pointer
      JSR   p_ident_is
      long_bcs @pointer
      LDA   #<kw_procedure
      LDX   #>kw_procedure
      JSR   p_ident_is
      BCC   @function_type
      LDA   p_routine_kind
      STA   p_type_work_element
      STZ   p_routine_kind
      LDA   #TYPE_KIND_PROCEDURE
      BRA   @procedural_type
@function_type:
      LDA   #<kw_function
      LDX   #>kw_function
      JSR   p_ident_is
      BCC   @set_type
      LDA   p_routine_kind
      STA   p_type_work_element
      LDA   #2
      STA   p_routine_kind
      LDA   #TYPE_KIND_FUNCTION
@procedural_type:
      STA   p_type_work_kind
      JMP   p_parse_procedural_type
@set_type:
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
      long_bcc @named
      JSR   p_peek_keyword_of
      long_bcc @untyped_file
      JSR   p_consume_of
      JSR   p_parse_type_spec
      long_bcs @fail
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
      JMP   p_type_size
@named:
      JSR   p_type_find
      BCC   @fail
      STA   p_decl_type
      STX   p_type_iter
      JSR   p_type_size
      BCS   @fail
      LDX   p_type_iter
      LDA   p_type_kind,X
      STA   p_type_work_kind
      LDA   p_type_low_l,X
      STA   p_type_work_low
      LDA   p_type_low_h,X
      STA   p_type_work_low+1
      LDA   p_type_high_l,X
      STA   p_type_work_high
      LDA   p_type_high_h,X
      STA   p_type_work_high+1
      LDA   p_type_element,X
      STA   p_type_work_element
      CLC
      RTS
@scalar:
      STA   p_type_work_element
      STA   p_decl_type
      JSR   p_type_size
      LDA   p_decl_type
      CMP   #TYPE_REAL
      BEQ   @scalar_real
      LDA   #TYPE_KIND_ORDINAL
      STA   p_type_work_kind
      CLC
      RTS
@scalar_real:
      LDA   #TYPE_KIND_REAL
      STA   p_type_work_kind
      CLC
      RTS
@subrange:
      JMP   p_parse_subrange_type
@fail:
      SEC
      RTS

; pointer-type = ^ type-identifier. Unknown targets reserve a descriptor row
; that must be completed before this TYPE section ends.
      .segment "NPCFE_CODE"
p_parse_pointer_type:
      JSR   p_next
      JSR   p_capture_identifier
      BCS   @fail
      JSR   p_type_find_pointer_target
      BCS   @target
      JSR   p_type_add_forward
      BCS   @fail
@target:
      STA   p_type_work_element
      LDA   #TYPE_KIND_POINTER
      STA   p_type_work_kind
      LDA   #TYPE_POINTER
      STA   p_decl_type
      LDA   #2
      STA   p_array_len
      STZ   p_array_len+1
      CLC
      RTS
@fail:
      SEC
      RTS

; Procedural types reuse the ordinary formal/signature parser, then discard
; its temporary symbols. The enclosing routine's frame metadata is restored
; so local type declarations cannot perturb code generation.
p_parse_procedural_type:
      LDA   p_symbol_count
      PHA
      LDA   p_scope_level
      PHA
      LDA   p_routine_param_count
      PHA
      LDA   p_routine_param_refmask
      PHA
      LDA   p_routine_param_widthmask
      PHA
      LDA   p_function_return_type
      PHA
      LDA   p_frame_size
      PHA
      LDA   p_frame_size+1
      PHA
      LDA   p_type_work_element
      PHA
      LDA   p_type_work_kind
      PHA
      INC   p_scope_level
      STZ   p_routine_param_count
      STZ   p_routine_param_refmask
      STZ   p_routine_param_widthmask
      LDA   #2
      STA   p_frame_size
      STZ   p_frame_size+1
      STZ   p_digit
      JSR   p_parse_routine_signature
      BCS   @parsed
      LDA   p_routine_param_count
      STA   p_type_work_low
      LDA   p_routine_param_refmask
      STA   p_type_work_low+1
      LDA   p_routine_param_widthmask
      STA   p_type_work_high
      LDA   p_function_return_type
      STA   p_type_work_high+1
      LDA   #2
      STA   p_array_len
      STZ   p_array_len+1
      LDA   #TYPE_POINTER
      STA   p_decl_type
      BRA   @restore
@parsed:
      INC   p_digit
@restore:
      PLA
      STA   p_type_work_kind
      PLA
      STA   p_routine_kind
      PLA
      STA   p_frame_size+1
      PLA
      STA   p_frame_size
      PLA
      STA   p_function_return_type
      PLA
      STA   p_routine_param_widthmask
      PLA
      STA   p_routine_param_refmask
      PLA
      STA   p_routine_param_count
      PLA
      STA   p_scope_level
      PLA
      STA   p_symbol_count
      STZ   p_type_work_element
      LDA   p_digit
      BEQ   @ok
      SEC
      RTS
@ok:
      CLC
      RTS

; Resolve a captured pointer target. Built-ins do not consume descriptor rows.
p_type_find_pointer_target:
      JSR   p_type_find
      BCS   @found
      LDA   #<kw_byte
      LDX   #>kw_byte
      JSR   p_ident_is
      BCC   @boolean
      LDA   #TYPE_BYTE
      BRA   @builtin
@boolean:
      LDA   #<kw_boolean
      LDX   #>kw_boolean
      JSR   p_ident_is
      BCC   @char
      LDA   #TYPE_BOOLEAN
      BRA   @builtin
@char:
      LDA   #<kw_char
      LDX   #>kw_char
      JSR   p_ident_is
      BCC   @word
      LDA   #TYPE_CHAR
      BRA   @builtin
@word:
      LDA   #<kw_word
      LDX   #>kw_word
      JSR   p_ident_is
      BCC   @integer
      LDA   #TYPE_WORD
      BRA   @builtin
@integer:
      LDA   #<kw_integer
      LDX   #>kw_integer
      JSR   p_ident_is
      BCC   @longint
      LDA   #TYPE_INTEGER
      BRA   @builtin
@longint:
      LDA   #<kw_longint
      LDX   #>kw_longint
      JSR   p_ident_is
      BCC   @real
      LDA   #TYPE_LONGINT
      BRA   @builtin
@real:
      LDA   #<kw_real
      LDX   #>kw_real
      JSR   p_ident_is
      BCC   @pointer
      LDA   #TYPE_REAL
      BRA   @builtin
@pointer:
      LDA   #<kw_pointer
      LDX   #>kw_pointer
      JSR   p_ident_is
      BCC   @missing
      LDA   #TYPE_POINTER
@builtin:
      LDX   #$FF
@found:
      SEC
      RTS
@missing:
      CLC
      RTS

      .segment "CODE"

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
      JMP   p_type_size
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
      JMP   p_type_size
@fail:
      SEC
      RTS

p_peek_keyword_of:
      LDA   #<kw_of
      LDX   #>kw_of
      JMP   p_peek_keyword

p_consume_of:
      LDA   #<kw_of
      LDX   #>kw_of
      JMP   p_expect_ax_word

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
      BNE   @open
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
      BRA   @done
@open:
      LDA   p_formal_type
      BEQ   @done
      LDA   #TYPE_STRING_REF
      STA   p_decl_type
      LDA   #2
      STA   p_array_len
      STZ   p_array_len+1
      CLC
      RTS
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
      JSR   p_field_name_pointer
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
      LDA   p_type_total
      PHA
      LDA   p_type_total+1
      PHA
      JSR   p_parse_decl_type
      BCC   @field_type_ready
      PLA
      PLA
      JMP   @fail
@field_type_ready:
      PLA
      STA   p_type_total+1
      PLA
      STA   p_type_total
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
      JSR   p_skip_ws
      JSR   p_peek
      long_bcc @fail
      CMP   #'['
      BEQ   @fixed
      LDA   p_formal_type
      BEQ   @fail
      JMP   p_parse_open_array_type
@fixed:
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

.segment "NPCFE_CODE"

; Turbo open arrays are canonical anonymous descriptors. Reusing one row per
; element type keeps forward declarations, unit interfaces, and callbacks on
; the ordinary structural signature path without another ABI table.
p_parse_open_array_type:
      JSR   p_consume_of
      BCS   @fail
      JSR   p_parse_decl_type
      BCS   @fail
      LDA   p_decl_type
      JSR   p_type_open_element
      BCS   @fail
      LDA   p_decl_type
      STA   p_type_work_element
      LDX   #0
@type:
      CPX   p_type_count
      BCS   @add
      LDA   p_type_kind,X
      CMP   #TYPE_KIND_OPEN_ARRAY
      BNE   @next
      LDA   p_type_element,X
      CMP   p_type_work_element
      BNE   @next
      TXA
      CLC
      ADC   #P_TYPE_USER_BASE
      STA   p_decl_type
      LDA   #4
      STA   p_array_len
      STZ   p_array_len+1
      CLC
      RTS
@next:
      INX
      BRA   @type
@add:
      LDA   #TYPE_KIND_OPEN_ARRAY
      STA   p_type_work_kind
      LDA   #4
      STA   p_array_len
      STZ   p_array_len+1
      LDA   p_type_decl_len
      STA   p_type_outer_decl_len
      STZ   p_type_decl_len
      JSR   p_type_add
      PHP
      LDA   p_type_outer_decl_len
      STA   p_type_decl_len
      PLP
      RTS
@fail:
      SEC
      RTS

.segment "CODE"

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
      JSR   p_const_meta_pointer
      LDY   #2
      LDA   (p_word),Y
      STA   p_type_work_low
      INY
      LDA   (p_word),Y
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
      long_bcc @fail
      CMP   #')'
      long_beq @close
@group:
      LDA   #SYM_PARAM
      STA   p_param_flags
      JSR   p_capture_identifier
      long_bcs @fail
      LDA   #<kw_var
      LDX   #>kw_var
      JSR   p_ident_is
      BCC   @const
      LDA   #SYM_PARAM|SYM_BYREF
      STA   p_param_flags
      JSR   p_capture_identifier
      long_bcs @fail
      BRA   @names
@const:
      LDA   #<kw_const
      LDX   #>kw_const
      JSR   p_ident_is
      BCC   @names
      LDA   #SYM_PARAM|SYM_CONST
      STA   p_param_flags
      JSR   p_capture_identifier
      long_bcs @fail
@names:
      LDA   p_symbol_count
      STA   p_symbol_group
@name:
      JSR   p_symbol_add
      long_bcs @fail
      JSR   p_skip_ws
      JSR   p_peek
      long_bcc @fail
      CMP   #','
      BNE   @type
      JSR   p_next
      JSR   p_capture_identifier
      long_bcs @fail
      BRA   @name
@type:
      JSR   p_skip_ws
      JSR   p_peek
      long_bcc @fail
      CMP   #':'
      BEQ   @typed
      LDA   p_param_flags
      AND   #SYM_BYREF
      long_beq @fail
      LDA   #TYPE_NONE             ; Turbo's untyped VAR parameter
      STA   p_decl_type
      JSR   p_bind_parameter_group
      long_bcs @fail
      BRA   @separator
@typed:
      LDA   #':'
      JSR   p_expect_char
      long_bcs @fail
      INC   p_formal_type
      JSR   p_parse_decl_type
      STZ   p_formal_type
      long_bcs @fail
      JSR   p_bind_parameter_group
      long_bcs @fail
@separator:
      JSR   p_skip_ws
      JSR   p_peek
      long_bcc @fail
      CMP   #';'
      BNE   @close
      JSR   p_next
      JMP   @group
@close:
      LDA   #')'
      JMP   p_expect_char
@fail:
      SEC
      RTS

p_bind_parameter_group:
      LDA   p_decl_type
      JSR   p_type_open_element
      BCS   @open
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
      BRA   @slot
@open:
      LDA   #4
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
      LDY   p_routine_param_count
      LDA   p_decl_type
      STA   p_routine_param_types,Y
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
      JSR   p_save_exit_context
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
      long_beq @fail
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
      JSR   p_restore_exit_context
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
      ADC   #<P_STATE_CONTEXT_NAME_OFFSET
      STA   p_state_offset
      LDA   p_remainder+1
      ADC   #>P_STATE_CONTEXT_NAME_OFFSET
      STA   p_state_offset+1
      JMP   p_state_pointer

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
      STZ   p_param_openmask
      STZ   p_argument_bytes
      LDX   p_scope_symbol_base
@parameter:
      CPX   p_scope_param_end
      BCS   @done
      JSR   p_symbol_slot_width
      BCS   @fail
      PHA
      ASL   p_param_flags
      ASL   p_param_openmask
      PLA
      CMP   #4
      BNE   @width
      INC   p_param_openmask
      BRA   @bytes
@width:
      CMP   #2
      BNE   :+
      INC   p_param_flags
:
@bytes:
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
      LDA   p_check_flags
      AND   #P_CHECK_STACK
      BEQ   @unchecked
      JSR   p_emit_stack_check
      long_bcs @fail
@unchecked:
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
      LDA   #<asm_index_word_separator
      LDX   #>asm_index_word_separator
      JSR   p_emit_ax_text
      long_bcs @fail
      LDA   p_param_openmask
      JSR   p_emit_hex_byte
      long_bcs @fail
      LDA   #$0A
      JSR   p_emit
      BCC   @done
@fail:
      SEC
@done:
      RTS

.segment "NPCFE_CODE"

; Value open arrays receive a private copy immediately beyond the fixed frame.
; CONST and VAR descriptors keep the caller's base pointer.
p_emit_open_array_copies:
      LDX   p_scope_symbol_base
@parameter:
      CPX   p_scope_param_end
      BCS   @done
      JSR   p_load_symbol_metadata
      LDA   p_symbol_type
      JSR   p_type_open_element
      BCC   @next
      STA   p_open_element
      LDA   p_symbol_flags
      AND   #SYM_BYREF|SYM_CONST
      BNE   @next
      LDA   p_open_element
      JSR   p_type_size
      BCS   @fail
      LDA   #<asm_open_copy_call
      LDX   #>asm_open_copy_call
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   p_symbol_offset+1
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   p_symbol_offset
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   #<asm_index_word_separator
      LDX   #>asm_index_word_separator
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   p_array_len+1
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   p_array_len
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   #<asm_index_word_separator
      LDX   #>asm_index_word_separator
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   p_statement_line+1
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   p_statement_line
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   #$0A
      JSR   p_emit
      BCS   @fail
@next:
      LDX   p_symbol_iter
      INX
      BRA   @parameter
@done:
      CLC
@fail:
      RTS

      .segment "NPCFE_CODE"
p_emit_stack_check:
      LDA   #<asm_stack_check
      LDX   #>asm_stack_check
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   p_frame_size+1
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   p_frame_size
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   #<asm_check_separator
      LDX   #>asm_check_separator
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   p_statement_line+1
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   p_statement_line
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   #$0A
      JMP   p_emit
@fail:
      RTS

      .segment "CODE"

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
      JSR   p_type_is_word_value
      BNE   :+
      LDA   #<asm_real_from_byte
      LDX   #>asm_real_from_byte
      JMP   p_emit_ax_text
:
      LDA   #<asm_real_from_word
      LDX   #>asm_real_from_word
      JMP   p_emit_ax_text
@ordinal:
      ; Assignment lowering has already widened one-byte ordinals before
      ; range checking. Do not emit the same extension again for a function
      ; result slot.
      BRA   @ready
@ready:
      CLC
      RTS
@fail:
      SEC
      RTS

p_symbol_slot_width:
      STX   p_symbol_iter
      JSR   p_symbol_pointer
      LDY   #1
      LDA   (p_word),Y
      JSR   p_type_open_element
      BCS   @open
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
@open:
      LDA   #4
      CLC
      RTS
@fail:
      SEC
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

p_emit_frame_direct_y_next:
      LDA   #<asm_frame_direct_y
      LDX   #>asm_frame_direct_y
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   p_symbol_offset
      INC   A
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
      JSR   p_emit_frame_direct_y_next
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

.segment "NPCFE_CODE"

; Load the hidden High word stored immediately after an open array's pointer.
p_emit_open_array_high:
      CLC
      LDA   p_symbol_offset
      ADC   #2
      STA   p_symbol_offset
      LDA   p_symbol_offset+1
      ADC   #0
      STA   p_symbol_offset+1
      JSR   p_emit_frame_load16_symbol
      PHP
      SEC
      LDA   p_symbol_offset
      SBC   #2
      STA   p_symbol_offset
      LDA   p_symbol_offset+1
      SBC   #0
      STA   p_symbol_offset+1
      PLP
      RTS

.segment "CODE"

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
      long_bcs @done
      STX   p_symbol_iter
      JSR   p_symbol_pointer
      LDY   #1
      LDA   p_decl_type
      STA   (p_word),Y
      LDX   p_symbol_iter
      JSR   p_emit_symbol_name
      long_bcs @fail
@suffix:
      LDA   p_type_work_high
      CMP   #$FF
      BNE   @sized
      LDA   #<asm_absolute_eq
      LDX   #>asm_absolute_eq
      JSR   p_emit_ax_text
      long_bcs @fail
      LDA   p_type_work_low+1
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   p_type_work_low
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   #$0A
      JSR   p_emit
      BCS   @fail
      BRA   @next
@sized:
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
      LDX   #>asm_byte_res
      JSR   p_emit_ax_text
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
      JMP   @symbol
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
      LDX   p_symbol_iter
      JMP   p_load_symbol_metadata
@next:
      LDX   p_symbol_iter
      BRA   @symbol
@missing:
      CLC
      RTS

; Load one known symbol row into the parser's shared designator metadata.
p_load_symbol_metadata:
      STX   p_symbol_iter
      JSR   p_symbol_pointer
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
      JSR   p_type_is_scalar4
      BCC   @sized
      LDA   #<asm_push_scalar4_value
      LDX   #>asm_push_scalar4_value
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
      JSR   p_type_is_pointer
      long_bcc @right_pointer
      LDA   p_expr_type
      JSR   p_type_is_pointer
      long_bcc @fail
      JSR   p_pointer_types_compatible
      long_bcs @fail
      LDA   p_operator
      CMP   #3
      long_bcs @fail
      JSR   p_emit_wide_operands
      long_bcs @fail
      LDA   #<asm_compare_word_start
      LDX   #>asm_compare_word_start
      JSR   p_emit_ax_text
      long_bcs @fail
      JMP   @suffix
@right_pointer:
      LDA   p_expr_type
      JSR   p_type_is_pointer
      long_bcs @fail
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
      long_bcs @fail
      JMP   @suffix
@right_string:
      LDA   p_expr_type
      JSR   p_type_is_string
      long_bcc @ordinal
      LDA   p_left_type
      JSR   p_type_is_char
      long_bcc @fail
      LDA   #<asm_char_string_compare
      LDX   #>asm_char_string_compare
      JSR   p_emit_ax_text
      long_bcs @fail
      JMP   @suffix
@ordinal:
      LDA   p_left_type
      JSR   p_type_is_real
      long_bcc @right_real
      LDA   p_expr_type
      JSR   p_type_is_real
      long_bcc @fail
      LDA   #<asm_real_compare
      LDX   #>asm_real_compare
      JSR   p_emit_scalar4_binary
      BCS   @fail
      BRA   @suffix
@right_real:
      LDA   p_expr_type
      JSR   p_type_is_real
      BCS   @fail
      LDA   p_left_type
      JSR   p_type_is_longint
      BCS   @left_longint
      LDA   p_expr_type
      JSR   p_type_is_longint
      BCS   @fail
      LDA   p_left_type
      JSR   p_type_is_word_value
      BNE   @wide
      LDA   p_expr_type
      JSR   p_type_is_word_value
      BNE   @wide
      LDA   #<asm_compare_start
      LDX   #>asm_compare_start
      JSR   p_emit_ax_text
      BCS   @fail
      BRA   @suffix
@wide:
      JSR   p_emit_wide_operands
      BCS   @fail
      JSR   p_binary_is_signed
      BEQ   @unsigned_word
      LDA   #<asm_compare_sword_start
      LDX   #>asm_compare_sword_start
      BRA   @emit_word_compare
@unsigned_word:
      LDA   #<asm_compare_word_start
      LDX   #>asm_compare_word_start
@emit_word_compare:
      JSR   p_emit_ax_text
      BCS   @fail
      BRA   @suffix
@left_longint:
      JSR   p_emit_longint_conversion
      BCS   @fail
      JSR   p_emit_scalar4_prologue
      BCS   @fail
      LDA   #<asm_longint_compare
      LDX   #>asm_longint_compare
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
      JSR   p_type_is_word_value
      BEQ   @right_byte
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
      JSR   p_type_is_word_value
      BEQ   @left_byte
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

p_new_exit_label:
      JSR   p_new_label
      LDA   p_label_saved
      STA   p_exit_label
      LDA   p_label_saved+1
      STA   p_exit_label+1
      RTS

p_load_exit_label:
      LDA   p_exit_label
      STA   p_label_saved
      LDA   p_exit_label+1
      STA   p_label_saved+1
      RTS

p_emit_exit_label:
      JSR   p_load_exit_label
      JMP   p_emit_label

p_save_exit_context:
      LDA   p_exit_label
      STA   p_context_exit_l,X
      LDA   p_exit_label+1
      STA   p_context_exit_h,X
      RTS

p_restore_exit_context:
      LDA   p_context_exit_l,X
      STA   p_exit_label
      LDA   p_context_exit_h,X
      STA   p_exit_label+1
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

p_store_control_continue:
      LDA   p_label_saved
      STA   p_case_body_lo,X
      LDA   p_label_saved+1
      STA   p_case_body_hi,X
      RTS

p_load_control_continue:
      LDA   p_case_body_lo,X
      STA   p_label_saved
      LDA   p_case_body_hi,X
      STA   p_label_saved+1
      RTS

p_emit_label_id:
      LDA   p_label_namespace_compact
      BEQ   @full_namespace
      LDA   p_label_namespace
      JSR   p_emit_hex_byte
      BCS   @fail
      BRA   @separator
@full_namespace:
      LDX   #0
@namespace:
      CPX   p_compiled_unit_name_len
      BCS   @separator
      LDA   p_compiled_unit_name,X
      JSR   p_emit
      BCS   @fail
      INX
      BRA   @namespace
@separator:
      LDA   #'_'
      JSR   p_emit
      BCS   @fail
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
      LDX   #>asm_jsr
      JSR   p_emit_ax_text
@fail:
      RTS

p_emit_load_byte:
      STA   p_char
      LDA   #<asm_lda
      LDX   #>asm_lda
      JSR   p_emit_ax_text
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
      JMP   p_emit
@fail:
      RTS

; A/X is a check-call prefix ending in ".WORD $". Runtime checks retain the
; Pascal statement line as two inline bytes and advance over it on return.
p_emit_check_line:
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   p_statement_line+1
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   p_statement_line
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   #$0A
      JMP   p_emit
@fail:
      RTS

p_emit_add_sub_check:
      LDA   p_check_flags
      AND   #P_CHECK_OVERFLOW
      BEQ   p_emit_check_done
      LDA   p_left_type
      JSR   p_type_is_word_value
      BNE   @wide
      LDA   p_expr_type
      JSR   p_type_is_word_value
      BNE   @wide
      LDA   p_operator
      CMP   #'+'
      BNE   @unsigned_subtract
      LDA   #<asm_check_uadd
      LDX   #>asm_check_uadd
      JMP   p_emit_check_line
@unsigned_subtract:
      LDA   #<asm_check_usub
      LDX   #>asm_check_usub
      JMP   p_emit_check_line
@wide:
      JSR   p_binary_is_signed
      BEQ   @unsigned_word
      LDA   #<asm_check_overflow
      LDX   #>asm_check_overflow
      JMP   p_emit_check_line
@unsigned_word:
      LDA   p_operator
      CMP   #'+'
      BNE   @unsigned_subtract
      LDA   #<asm_check_uadd
      LDX   #>asm_check_uadd
      JMP   p_emit_check_line

p_binary_is_signed:
      LDA   p_left_type
      JSR   p_type_is_signed_ordinal
      BNE   @done
      LDA   p_expr_type
      JSR   p_type_is_signed_ordinal
@done:
      RTS

p_emit_check_done:
      CLC
      RTS

p_emit_zero_check:
      LDA   p_expr_type
      JSR   p_type_is_word_value
      BNE   @word
      LDA   #<asm_check_zero8
      LDX   #>asm_check_zero8
      JMP   p_emit_check_line
@word:
      LDA   #<asm_check_zero16
      LDX   #>asm_check_zero16
      JMP   p_emit_check_line

p_emit_multiply_check:
      LDA   p_check_flags
      AND   #P_CHECK_OVERFLOW
      BEQ   p_emit_check_done
      JSR   p_binary_is_signed
      BEQ   @unsigned
      LDA   #<asm_check_muls
      LDX   #>asm_check_muls
      JMP   p_emit_check_line
@unsigned:
      LDA   #<asm_check_mulu
      LDX   #>asm_check_mulu
      JMP   p_emit_check_line

p_emit_negate_check:
      LDA   p_check_flags
      AND   #P_CHECK_OVERFLOW
      BEQ   p_emit_check_done
      LDA   p_expr_type
      JSR   p_type_is_signed_ordinal
      BEQ   @unsigned
      LDA   #<asm_check_negs
      LDX   #>asm_check_negs
      JMP   p_emit_check_line
@unsigned:
      LDA   #<asm_check_negu
      LDX   #>asm_check_negu
      JMP   p_emit_check_line

; Before operand normalization, A/X is the divisor and the signed dividend is
; still the two-byte value immediately beneath this generated call's return.
p_emit_divide_overflow_check:
      LDA   p_check_flags
      AND   #P_CHECK_OVERFLOW
      BEQ   p_emit_check_done
      LDA   p_left_type
      JSR   p_type_is_word_value
      BEQ   p_emit_check_done
      LDA   p_left_type
      JSR   p_type_is_signed_ordinal
      BEQ   p_emit_check_done
      LDA   p_expr_type
      JSR   p_type_is_word_value
      BEQ   p_emit_check_done
      LDA   #<asm_check_divs
      LDX   #>asm_check_divs
      JMP   p_emit_check_line

p_emit_io_check:
      LDA   p_check_flags
      AND   #P_CHECK_IO
      BEQ   p_emit_io_check_done
      LDA   p_statement_io_kind
      BEQ   p_emit_io_check_done
      CMP   #1
      BNE   p_emit_io_write_check
      LDA   #<asm_io_check_read
      LDX   #>asm_io_check_read
      JMP   p_emit_check_line
p_emit_io_write_check:
      LDA   p_check_flags
      AND   #P_CHECK_IO
      BEQ   p_emit_io_check_done
      LDA   #<asm_io_check_write
      LDX   #>asm_io_check_write
      JMP   p_emit_check_line
p_emit_io_check_done:
      CLC
      RTS

p_emit_source_comment:
      LDA   #<asm_comment
      LDX   #>asm_comment
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   #<p_current_name
      LDX   #>p_current_name
      JSR   p_emit_ax_text
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
      LDA   p_suppress_output
      BEQ   @write
      PLA
      CLC
      RTS
@write:
      PLA
      PHX
      LDX   p_output_used
      STA   source_buf,X
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
p_expect_ax_word:
      STA   p_word
      STX   p_word+1
      JMP   p_expect_word

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

; Expression parsing reuses p_saved_ident for function callees. Preserve the
; direct assignment destination so a call on the right-hand side cannot turn
; `Value := Function()` into a store over the function's code label.
p_save_assignment_identifier:
      LDA   p_saved_ident_len
      STA   p_assignment_ident_len
      LDX   #2
@hash:
      LDA   p_saved_hash,X
      STA   p_assignment_hash,X
      DEX
      BPL   @hash
      LDA   #<P_STATE_ASSIGN_NAME_OFFSET
      STA   p_state_offset
      LDA   #>P_STATE_ASSIGN_NAME_OFFSET
      STA   p_state_offset+1
      JSR   p_state_pointer
      LDY   #0
@name:
      CPY   p_assignment_ident_len
      BCS   @done
      LDA   p_saved_ident,Y
      STA   (p_word),Y
      INY
      BRA   @name
@done:
      RTS

p_restore_assignment_identifier:
      LDA   p_assignment_ident_len
      STA   p_saved_ident_len
      LDX   #2
@hash:
      LDA   p_assignment_hash,X
      STA   p_saved_hash,X
      DEX
      BPL   @hash
      LDA   #<P_STATE_ASSIGN_NAME_OFFSET
      STA   p_state_offset
      LDA   #>P_STATE_ASSIGN_NAME_OFFSET
      STA   p_state_offset+1
      JSR   p_state_pointer
      LDY   #0
@name:
      CPY   p_assignment_ident_len
      BCS   @done
      LDA   (p_word),Y
      STA   p_saved_ident,Y
      INY
      BRA   @name
@done:
      RTS

; Function expressions may nest, so preserve each callee while its argument
; expression is parsed. Eight levels matches the parser's routine-depth limit.
p_push_call_identifier:
      LDX   p_in_argument
      CPX   #P_CALL_DEPTH_CAP
      long_bcs @fail
      LDA   p_argument_bytes
      STA   p_call_stack_argument_bytes,X
      LDA   p_call_ident_len
      STA   p_call_stack_len,X
      LDA   p_function_indirect
      BEQ   @direct
      LDA   p_call_stack_len,X
      ORA   #$80
      STA   p_call_stack_len,X
      LDA   p_function_call_type_row
      BRA   @hash0
@direct:
      LDA   p_call_hash
@hash0:
      STA   p_call_stack_hash0,X
      LDA   p_call_hash+1
      STA   p_call_stack_hash1,X
      LDA   p_call_hash+2
      STA   p_call_stack_hash2,X
      JSR   p_call_stack_name_pointer
      LDY   #0
@name:
      CPY   p_call_ident_len
      BCS   @metadata
      LDA   p_call_ident,Y
      STA   (p_word),Y
      INY
      BRA   @name
@metadata:
      LDX   p_in_argument
      JSR   p_call_meta_pointer
      LDY   #0
      LDA   p_argument_count
      STA   (p_word),Y
      INY
      LDA   p_call_arity
      STA   (p_word),Y
      INY
      LDA   p_statement_refmask
      STA   (p_word),Y
      INY
      LDA   p_statement_widthmask
      STA   (p_word),Y
      INY
      LDA   p_statement_typed
      STA   (p_word),Y
      INY
      LDA   p_statement_routine
      STA   (p_word),Y
      INY
      LDA   p_statement_force_stack
      STA   (p_word),Y
      INY
      LDA   p_statement_io_kind
      STA   (p_word),Y
      INY
      LDA   p_read_call
      STA   (p_word),Y
      INY
      LDA   p_read_size
      STA   (p_word),Y
      INY
      LDA   p_read_size+1
      STA   (p_word),Y
      INY
      LDA   p_val_call
      STA   (p_word),Y
      INY
      LDA   p_val_size
      STA   (p_word),Y
      INY
      LDA   p_insert_call
      STA   (p_word),Y
      INY
      LDA   p_insert_capacity
      STA   (p_word),Y
      INY
      LDA   p_indirect_call
      STA   (p_word),Y
      INY
      LDA   p_call_type
      STA   (p_word),Y
      INY
      LDA   p_call_type_row
      STA   (p_word),Y
      INY
      LDA   p_target_type
      STA   (p_word),Y
      INY
      LDA   p_target_flags
      STA   (p_word),Y
      INY
      LDA   p_open_expected
      STA   (p_word),Y
      INY
      LDA   p_open_element
      STA   (p_word),Y
      INC   p_in_argument
      CLC
@fail:
      RTS

p_restore_call_identifier:
      LDX   p_in_argument
      BEQ   @fail
      DEX
      STZ   p_function_indirect
      LDA   p_call_stack_len,X
      BPL   @direct
      AND   #$7F
      STA   p_call_ident_len
      INC   p_function_indirect
      LDA   p_call_stack_hash0,X
      STA   p_function_call_type_row
      BRA   @hash1
@direct:
      STA   p_call_ident_len
      LDA   p_call_stack_hash0,X
      STA   p_call_hash
@hash1:
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
      long_beq @fail
      DEC   p_in_argument
      LDX   p_in_argument
      LDA   p_call_stack_argument_bytes,X
      STA   p_argument_bytes
      JSR   p_call_meta_pointer
      LDY   #0
      LDA   (p_word),Y
      STA   p_argument_count
      INY
      LDA   (p_word),Y
      STA   p_call_arity
      INY
      LDA   (p_word),Y
      STA   p_statement_refmask
      INY
      LDA   (p_word),Y
      STA   p_statement_widthmask
      INY
      LDA   (p_word),Y
      STA   p_statement_typed
      INY
      LDA   (p_word),Y
      STA   p_statement_routine
      INY
      LDA   (p_word),Y
      STA   p_statement_force_stack
      INY
      LDA   (p_word),Y
      STA   p_statement_io_kind
      INY
      LDA   (p_word),Y
      STA   p_read_call
      INY
      LDA   (p_word),Y
      STA   p_read_size
      INY
      LDA   (p_word),Y
      STA   p_read_size+1
      INY
      LDA   (p_word),Y
      STA   p_val_call
      INY
      LDA   (p_word),Y
      STA   p_val_size
      INY
      LDA   (p_word),Y
      STA   p_insert_call
      INY
      LDA   (p_word),Y
      STA   p_insert_capacity
      INY
      LDA   (p_word),Y
      STA   p_indirect_call
      INY
      LDA   (p_word),Y
      STA   p_call_type
      INY
      LDA   (p_word),Y
      STA   p_call_type_row
      INY
      LDA   (p_word),Y
      STA   p_target_type
      INY
      LDA   (p_word),Y
      STA   p_target_flags
      INY
      LDA   (p_word),Y
      STA   p_open_expected
      INY
      LDA   (p_word),Y
      STA   p_open_element
      LDA   p_in_argument
      BEQ   @done
      JSR   p_restore_call_identifier
@done:
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
      ADC   #<P_STATE_CALL_NAME_OFFSET
      STA   p_state_offset
      LDA   p_remainder+1
      ADC   #>P_STATE_CALL_NAME_OFFSET
      STA   p_state_offset+1
      JMP   p_state_pointer

; X selects one compact saved parser-state record. Function calls reuse the
; statement argument machinery, so nested calls restore the outer call's
; typed ABI and trust-boundary metadata rather than maintaining two parsers.
p_call_meta_pointer:
      STZ   p_remainder
      STZ   p_remainder+1
@record:
      CPX   #0
      BEQ   @address
      CLC
      LDA   p_remainder
      ADC   #P_CALL_META_SIZE
      STA   p_remainder
      LDA   p_remainder+1
      ADC   #0
      STA   p_remainder+1
      DEX
      BRA   @record
@address:
      CLC
      LDA   p_remainder
      ADC   #<P_STATE_CALL_META_OFFSET
      STA   p_state_offset
      LDA   p_remainder+1
      ADC   #>P_STATE_CALL_META_OFFSET
      STA   p_state_offset+1
      JMP   p_state_pointer

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

p_emit_unit_initializer_label:
      LDA   #<asm_unit_init_global_prefix
      LDX   #>asm_unit_init_global_prefix
      JSR   p_emit_ax_text
      BCS   @fail
      JSR   @name
      BCS   @fail
      LDA   #<asm_unit_export_end
      LDX   #>asm_unit_export_end
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   #<asm_unit_init_routine_prefix
      LDX   #>asm_unit_init_routine_prefix
      JSR   p_emit_ax_text
      BCS   @fail
      JSR   @name
      BCS   @fail
      LDA   #$0A
      JMP   p_emit
@name:
      LDX   #0
@char:
      CPX   p_compiled_unit_name_len
      BCS   @done
      LDA   p_compiled_unit_name,X
      JSR   p_emit
      BCS   @fail
      INX
      BRA   @char
@done:
      CLC
      RTS
@fail:
      SEC
      RTS

; NovaFoo maps mechanically to canonical FOO.INC/FOO.S files. No NDK API or
; hardware address is embedded in NPC; routine statements retain their symbol.
p_import_local_unit:
      LDA   p_ident_len
      CLC
      ADC   #4
      CMP   #P_SOURCE_NAME_CAP+1
      long_bcs @fail
      STA   p_include_name_len
      LDX   #0
@name:
      CPX   p_ident_len
      BCS   @suffix
      LDA   p_ident,X
      STA   p_include_name,X
      INX
      BRA   @name
@suffix:
      LDA   #'.'
      STA   p_include_name,X
      INX
      LDA   #'P'
      STA   p_include_name,X
      INX
      LDA   #'A'
      STA   p_include_name,X
      INX
      LDA   #'S'
      STA   p_include_name,X
      JSR   p_open_include
      BCS   @fail
      INC   p_importing_interface
      INC   p_suppress_output
      INC   p_in_unit_interface
      JSR   p_capture_identifier
      BCS   @parsed
      LDA   #<kw_unit
      LDX   #>kw_unit
      JSR   p_ident_is
      BCC   @syntax
      JSR   p_parse_unit_interface
      BRA   @parsed
@syntax:
      SEC
@parsed:
      PHP
      LDA   p_line
      PHA
      LDA   p_line+1
      PHA
      LDA   p_column
      PHA
      LDA   p_column+1
      PHA
      LDA   p_current_name_len
      STA   p_include_name_len
      LDX   #0
@restore_name:
      CPX   p_current_name_len
      BCS   @name_restored
      LDA   p_current_name,X
      STA   p_include_name,X
      INX
      BRA   @restore_name
@name_restored:
      DEC   p_in_unit_interface
      DEC   p_suppress_output
      DEC   p_importing_interface
      JSR   p_pop_include
      PLA
      STA   NVR3H
      PLA
      STA   NVR3L
      PLA
      STA   NVR2H
      PLA
      STA   NVR2L
      PLP
      BCC   @ok
      LDA   NVR2L
      STA   p_line
      LDA   NVR2H
      STA   p_line+1
      LDA   NVR3L
      STA   p_column
      LDA   NVR3H
      STA   p_column+1
      LDA   #1                    ; contract was found but did not parse
      SEC
      RTS
@ok:
      LDA   #1
      CLC
      RTS
@fail:
      LDA   #0                    ; no source contract at any UNITPATH root
      SEC
      RTS

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
      long_bcc @bad
      BRA   @platform
@user:
      LDA   p_ident_len
      CMP   #P_UNIT_NAME_CAP
      long_bcs @bad
      JSR   p_unit_is_local
      BCC   @external_user
      JSR   p_import_local_unit
      long_bcs @bad
      LDA   #2
      BRA   @imported_user
@external_user:
      JSR   p_import_local_unit
      BCC   @external_imported
      CMP   #0
      long_bne @bad
      STZ   p_char                ; precompiled .ASM-only user unit
      LDY   #0
      BRA   @store
@external_imported:
      LDA   #3                    ; parsed contract, defer typed source until after main
@imported_user:
      STA   p_char
      SEC
      LDA   p_include_name_len
      SBC   #4
      STA   p_ident_len
      TAX
@restore_local_name:
      DEX
      BMI   @local_name_ready
      LDA   p_include_name,X
      STA   p_ident,X
      BRA   @restore_local_name
@local_name_ready:
      LDY   #0
      BRA   @store
@platform:
      LDA   #1
      STA   p_char
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
      LDA   p_char
      STA   p_unit_kind,X
      JSR   p_unit_name_pointer
      LDA   p_expected
      STA   p_remainder
      LDY   #0
@copy:
      LDA   p_remainder
      CMP   p_ident_len
      BCS   @stored
      TAX
      LDA   p_ident,X
      STA   (p_word),Y
      INC   p_remainder
      INY
      BRA   @copy
@stored:
      LDX   p_unit_count
      INC   p_unit_count
      LDA   p_unit_kind,X
      CMP   #2
      BEQ   @ok
      CMP   #3
      BEQ   @ok
      CMP   #0
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
      JSR   p_emit_stored_unit_name
      BCS   @fail
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

; Emit stored unit X without adding a declaration/source suffix.
p_emit_stored_unit_name:
      STX   p_unit_iter
      JSR   p_unit_name_pointer
      LDX   p_unit_iter
      LDA   p_unit_len,X
      STA   p_digit
@name:
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
      BRA   @name
@done:
      CLC
      RTS
@fail:
      SEC
      RTS

p_unit_name_pointer:
      TXA
      ASL
      ASL
      ASL
      ASL
      CLC
      ADC   #<P_STATE_UNIT_NAME_OFFSET
      STA   p_state_offset
      LDA   #>P_STATE_UNIT_NAME_OFFSET
      ADC   #0
      STA   p_state_offset+1
      JMP   p_state_pointer

p_emit_unit_sources:
      LDX   #0
@unit:
      CPX   p_unit_count
      BCS   @done
      STX   p_unit_iter
      LDA   p_unit_kind,X
      CMP   #1
      BEQ   @source
      CMP   #3
      BNE   @next
      LDA   #3
      BRA   @emit
@source:
      LDA   #1
@emit:
      JSR   p_emit_unit_line
      BCS   @fail
@next:
      LDX   p_unit_iter
      INX
      BRA   @unit
@done:
      CLC
@fail:
      RTS

p_emit_unit_initializers:
      LDX   #0
@unit:
      CPX   p_unit_count
      BCS   @done
      STX   p_unit_iter
      LDA   p_unit_kind,X
      CMP   #2
      BNE   @next
      LDA   #<asm_unit_init_call
      LDX   #>asm_unit_init_call
      JSR   p_emit_ax_text
      BCS   @fail
      LDX   p_unit_iter
      JSR   p_emit_stored_unit_name
      BCS   @fail
      LDA   #$0A
      JSR   p_emit
      BCS   @fail
@next:
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
      LDX   #>asm_jsr_prefix
      JSR   p_emit_ax_text
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
      LDX   #>asm_jsr_prefix
      JSR   p_emit_ax_text
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
      LDA   #<asm_ply
      LDX   #>asm_ply
      JSR   p_emit_ax_text
      PLX
      BCS   @fail
      DEX
      BRA   @argument
@done:
      CLC
@fail:
      RTS

; Emit A ordinary stack-byte discards without carrying duplicate PLA strings.
p_emit_drop_count:
      STA   p_digit
@byte:
      LDA   #<asm_pla
      LDX   #>asm_pla
      JSR   p_emit_ax_text
      BCS   @fail
      DEC   p_digit
      BNE   @byte
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
      LDX   #>asm_sig_start
      JMP   p_emit_ax_text

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
      LDX   #>asm_const_start
      JSR   p_emit_ax_text
      BCS   @fail
      LDX   #<p_ident_hash
      LDY   #>p_ident_hash
      JSR   p_emit_hash
      BCS   @fail
      LDA   #<asm_const_end
      LDX   #>asm_const_end
      JSR   p_emit_ax_text
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
      LDX   #>asm_load_byte_ir
      JSR   p_emit_ax_text
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
      JSR   p_check_assignment_compatibility
      long_bcs @fail
      JSR   p_emit_assignment_word_extension
      long_bcs @fail
      JSR   p_emit_target_range_check
      long_bcs @fail
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
      LDA   p_array_len
      CMP   #2
      BNE   @direct_byte
      LDA   p_expr_type
      JSR   p_type_size
      long_bcs @fail
      LDA   p_array_len+1
      BNE   @word_value
      LDA   p_array_len
      CMP   #2
      BCC   @word_from_byte
@word_value:
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
      JSR   p_check_assignment_compatibility
      long_bcs @fail
      JSR   p_emit_assignment_word_extension
      long_bcs @fail
      JSR   p_emit_target_range_check
      long_bcs @fail
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
      long_bne p_emit_store_designator_block
      LDA   p_array_len
      CMP   #1
      long_beq @byte
      CMP   #2
      long_bne p_emit_store_designator_block
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
      JSR   p_type_is_word_value
      BNE   @real_word
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
      LDA   #<asm_scalar4_copy
      LDX   #>asm_scalar4_copy
      JMP   p_emit_ax_text
@real_store_integer:
      LDA   #<asm_real_store_integer
      LDX   #>asm_real_store_integer
      JMP   p_emit_ax_text
@fail:
      SEC
      RTS

; Copy an address-valued aggregate into the destination address saved below
; the expression. This is the designator counterpart of the shared block store.
p_emit_store_designator_block:
      LDA   p_array_len
      STA   p_type_saved_size
      LDA   p_array_len+1
      STA   p_type_saved_size+1
      LDA   #<asm_string_copy_save
      LDX   #>asm_string_copy_save
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   #<asm_stack_address_to_nvr0
      LDX   #>asm_stack_address_to_nvr0
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   #<asm_load_saved_nvr2
      LDX   #>asm_load_saved_nvr2
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   #<asm_block_copy_call
      LDX   #>asm_block_copy_call
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   p_type_saved_size+1
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   p_type_saved_size
      JSR   p_emit_hex_byte
      BCS   @fail
      LDA   #$0A
      JMP   p_emit
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

; Carry set for built-in or aliased signed 32-bit ordinals.
p_type_is_longint:
      CMP   #TYPE_LONGINT
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
      CMP   #TYPE_LONGINT
      BNE   @no
@yes:
      SEC
      RTS
@no:
      CLC
      RTS

; Four-byte numeric scalars share one address-valued expression ABI.
p_type_is_scalar4:
      PHA
      JSR   p_type_is_real
      PLA
      BCS   @yes
      JMP   p_type_is_longint
@yes:
      SEC
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
      JSR   p_type_is_word_value
      BNE   @word
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
      LDA   #<asm_scalar4_copy
      LDX   #>asm_scalar4_copy
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
      JSR   p_type_is_word_value
      BNE   p_emit_store_with_real_word
      LDA   p_expr_type
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
      LDA   #<asm_scalar4_copy
      LDX   #>asm_scalar4_copy
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
      long_bcs @done
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
      CMP   #SYSFN_ORDINAL
      BEQ   @ordinal
      CMP   #SYSFN_FILE
      BEQ   @file
      CMP   #SYSFN_BYTE
      BEQ   @byte_result
      CMP   #SYSFN_WORD
      BEQ   @word
      LDA   #TYPE_REAL
      STA   p_function_result_type
      BRA   @normalize_real
@ordinal:
      LDA   #TYPE_WORD
      BRA   @ordinal_typed
@byte_result:
      LDA   #TYPE_BYTE
@ordinal_typed:
      STA   p_function_result_type
      LDA   p_function_arg_type
      JSR   p_type_is_word_value
      BNE   @done
      LDA   #<asm_index_high_zero
      LDX   #>asm_index_high_zero
      JMP   p_emit_ax_text
@file:
      LDA   #TYPE_WORD
      STA   p_function_result_type
      BRA   @done
@same:
      LDA   p_function_arg_type
      STA   p_function_result_type
      JSR   p_type_is_real
      BCC   @done
      BRA   @real_ready
@word:
      LDA   #TYPE_INTEGER
      STA   p_function_result_type
@normalize_real:
      LDA   p_function_arg_type
      JSR   p_type_is_real
      BCS   @real_ready
      LDA   p_function_arg_type
      CMP   #TYPE_WORD
      BEQ   @from_word
      CMP   #TYPE_INTEGER
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
@nil:
      STZ   p_decimal
      STZ   p_decimal+1
      JSR   p_emit_load_word
      long_bcs @bad
      LDA   #TYPE_NIL
      STA   p_expr_type
      CLC
      RTS
@byte_cast:
      LDA   #<kw_nil
      LDX   #>kw_nil
      JSR   p_ident_is
      BCS   @nil
      LDA   #<kw_byte
      LDX   #>kw_byte
      JSR   p_ident_is
      long_bcs @constant
      JSR   p_symbol_find
      long_bcs @ordinary_identifier
      JSR   p_with_field_find
      long_bcs @ordinary_identifier
      JSR   p_const_find
      long_bcs @ordinary_identifier
      JSR   p_save_call_identifier
      JSR   p_routine_find_call
      long_bcs @ordinary_identifier
      LDA   #<kw_copy
      LDX   #>kw_copy
      JSR   p_ident_is
      long_bcs p_parse_copy_function
      LDA   #<kw_chr
      LDX   #>kw_chr
      JSR   p_ident_is
      long_bcs p_parse_chr_function
      LDA   #<kw_ord
      LDX   #>kw_ord
      JSR   p_ident_is
      long_bcs p_parse_ord_function
      LDA   #<kw_pos
      LDX   #>kw_pos
      JSR   p_ident_is
      long_bcs p_parse_pos_function
      LDA   #<kw_odd
      LDX   #>kw_odd
      JSR   p_ident_is
      long_bcs p_parse_odd_function
      LDA   #<kw_high
      LDX   #>kw_high
      JSR   p_ident_is
      BCC   @low_intrinsic
      LDA   #INTRINSIC_HIGH
      STA   p_intrinsic_kind
      JMP   p_parse_type_intrinsic
@low_intrinsic:
      LDA   #<kw_low
      LDX   #>kw_low
      JSR   p_ident_is
      BCC   @sizeof_intrinsic
      LDA   #INTRINSIC_LOW
      STA   p_intrinsic_kind
      JMP   p_parse_type_intrinsic
@sizeof_intrinsic:
      LDA   #<kw_sizeof
      LDX   #>kw_sizeof
      JSR   p_ident_is
      BCC   @ordinary_identifier
      LDA   #INTRINSIC_SIZEOF
      STA   p_intrinsic_kind
      JMP   p_parse_type_intrinsic
@ordinary_identifier:
      JSR   p_skip_ws
      JSR   p_peek
      long_bcc @load_identifier
      CMP   #'['
      long_beq @designator
      CMP   #'.'
      long_beq @designator
      CMP   #'^'
      long_beq @designator
      CMP   #'('
      long_bne @load_identifier
      JSR   p_prepare_function_indirect
      long_bcs @bad
      JSR   p_save_call_identifier
      JSR   p_push_call_identifier
      long_bcs @bad
      JSR   p_restore_call_identifier_to_saved
      LDA   p_function_indirect
      STA   p_indirect_call
      BEQ   :+
      LDA   p_function_call_type_row
      STA   p_call_type_row
:
      STZ   p_argument_bytes
      STZ   p_function_arg_type
      JSR   p_next
      JSR   p_count_call_arguments
      long_bcs @call_bad
      STA   p_call_arity
      JSR   p_saved_call_metadata
      STZ   p_argument_count
      JSR   p_skip_ws
      JSR   p_peek
      long_bcc @call_bad
      CMP   #')'
      BEQ   @function_close
@parse_function_argument:
      JSR   p_load_expected_argument_type
      long_bcs @call_bad
      JSR   p_parse_statement_argument
      long_bcs @call_bad
      JSR   p_emit_coerce_argument
      long_bcs @call_bad
      LDA   p_expr_type
      STA   p_function_arg_type
      LDX   p_argument_count
      LDA   p_argument_bits,X
      AND   p_statement_refmask
      BNE   @function_argument_valid
      JSR   p_check_assignment_compatibility
      long_bcs @call_bad
@function_argument_valid:
      INC   p_argument_count
      LDA   p_argument_count
      CMP   #9
      long_bcs @call_bad
      JSR   p_skip_ws
      JSR   p_peek
      long_bcc @call_bad
      CMP   #','
      BNE   @function_last_argument
      JSR   p_emit_push_argument
      long_bcs @call_bad
      JSR   p_next
      BRA   @parse_function_argument
@function_last_argument:
      LDA   p_argument_count
      CMP   #2
      BCS   @function_push_last
      LDA   p_statement_force_stack
      BNE   @function_push_last
      LDA   p_function_indirect
      BEQ   @function_close
@function_push_last:
      JSR   p_emit_push_argument
      long_bcs @call_bad
@function_close:
      LDA   #')'
      JSR   p_expect_char
      long_bcs @call_bad
      JSR   p_restore_call_identifier
      long_bcs @call_bad
      JSR   p_restore_call_identifier_to_saved
      LDA   p_function_indirect
      BEQ   @direct_function
      LDX   p_function_call_type_row
      LDA   p_type_low_l,X
      CMP   p_argument_count
      long_bne @call_bad
      LDA   p_type_low_h,X
      STA   p_statement_refmask
      LDA   p_type_high_l,X
      STA   p_statement_widthmask
      LDA   p_type_high_l,X
      JSR   p_emit_indirect_call
      long_bcs @call_bad
      LDX   p_function_call_type_row
      LDA   p_type_high_h,X
      BRA   @function_result
@direct_function:
      JSR   p_routine_find_call
      BCC   @external_function
      LDA   p_routine_kind_table,X
      AND   #P_ROUTINE_KIND_MASK
      BEQ   @call_bad
      LDA   p_routine_params,X
      CMP   p_argument_count
      BNE   @call_bad
      STX   p_routine_iter
      JSR   p_emit_local_call_buffered
      long_bcs @call_bad
      LDX   p_routine_iter
      LDA   p_routine_kind_table,X
      AND   #P_ROUTINE_EXTERNAL
      BEQ   @local_function_result
      JSR   p_emit_drop_arguments
      long_bcs @call_bad
      LDX   p_routine_iter
@local_function_result:
      LDA   p_routine_return,X
      BRA   @function_result
@external_function:
      JSR   p_prepare_system_function
      long_bcs @call_bad
      LDA   p_argument_count
      CMP   #2
      BCS   @external_pascal_function
      CLC
      ADC   #2
      JSR   p_emit_sig_call
      long_bcs @call_bad
      JSR   p_emit_call_buffered
      long_bcs @call_bad
      LDA   p_function_result_type
      BRA   @function_result
@external_pascal_function:
      JSR   p_emit_pascal_sig_saved
      long_bcs @call_bad
      JSR   p_emit_call_buffered
      long_bcs @call_bad
      JSR   p_emit_drop_arguments
      long_bcs @call_bad
      LDA   p_function_result_type
@function_result:
      STA   p_function_result_type
      JSR   p_pop_call_identifier
      LDA   p_function_result_type
      STA   p_expr_type
      CLC
      RTS

@call_bad:
      JSR   p_pop_call_identifier
      JMP   @bad
@designator:
      LDA   p_check_flags
      AND   #P_CHECK_RANGE
      BNE   @generic_designator
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
      CMP   #2
      BNE   @known_byte
      JMP   p_emit_load_word_identifier
@known_byte:
      JMP   p_emit_load_identifier
@frame_value:
      JMP   p_emit_load_frame_value
@global_address:
      JMP   p_emit_address_identifier
@with_field:
      JSR   p_with_field_find
      BCC   @unknown_identifier
      JMP   p_emit_with_field_load
@unknown_identifier:
      JSR   p_const_find
      BCC   @routine_identifier
      STX   p_const_iter
      JSR   p_const_meta_pointer
      LDY   #1
      LDA   (p_word),Y
      STA   p_expr_type
      INY
      LDA   (p_word),Y
      STA   p_decimal
      INY
      LDA   (p_word),Y
      STA   p_decimal+1
      LDA   p_expr_type
      JSR   p_type_size
      long_bcs @bad
      LDA   p_array_len+1
      BNE   @constant_word
      LDA   p_array_len
      CMP   #2
      BCS   @constant_word
      LDA   p_decimal
      JMP   p_emit_load_byte
@constant_word:
      JMP   p_emit_load_word
@routine_identifier:
      JSR   p_save_call_identifier
      LDA   p_target_type
      JSR   p_type_is_procedural
      BCC   @routine_call
      JSR   p_routine_find_call
      BCC   @external_identifier
      JSR   p_routine_matches_target_type
      BCS   @external_identifier
      JSR   p_emit_address_identifier
      long_bcs @bad
      LDA   p_target_type
      STA   p_expr_type
      CLC
      RTS
@routine_call:
      JSR   p_routine_find_call
      BCC   @external_identifier
      LDA   p_routine_kind_table,X
      AND   #P_ROUTINE_KIND_MASK
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
      STX   p_digit
      LDA   #2
      JSR   p_emit_sig_call
      long_bcs @bad
      JSR   p_emit_call_buffered
      long_bcs @bad
      LDA   #TYPE_BYTE
      LDX   p_digit
      CPX   #3
      BNE   :+
      LDA   #TYPE_WORD
:
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
      JMP   p_emit_load_identifier
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
      JMP   p_emit_address_identifier
@external_value:
      LDA   #TYPE_BYTE
      STA   p_expr_type
      JMP   p_emit_external_value_identifier
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
      ORA   p_decimal+2
      ORA   p_decimal+3
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
      JSR   p_parse_hex_literal
      long_bcs @bad
      LDA   p_decimal+2
      ORA   p_decimal+3
      BEQ   @hex_word
      JSR   p_emit_long_literal
      long_bcs @bad
      LDA   #TYPE_LONGINT
      STA   p_expr_type
      CLC
      RTS
@hex_word:
      LDA   p_decimal+1
      BEQ   @hex_byte
      JSR   p_emit_load_word
      long_bcs @bad
      LDA   #TYPE_WORD
      STA   p_expr_type
      CLC
      RTS
@hex_byte:
      LDA   p_decimal
      JSR   p_emit_load_byte
      long_bcs @bad
      LDA   #TYPE_BYTE
      STA   p_expr_type
      CLC
      RTS
@decimal:
      JSR   p_parse_decimal_literal
      long_bcs @bad
      LDA   p_decimal+2
      ORA   p_decimal+3
      BEQ   @decimal_word
      LDA   p_decimal+3
      BMI   @bad
      JSR   p_emit_long_literal
      long_bcs @bad
      LDA   #TYPE_LONGINT
      STA   p_expr_type
      CLC
      RTS
@decimal_word:
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

; ODD is pure ordinal arithmetic. A/X already contains the expression value,
; so the low-bit test costs one instruction and no runtime member.
p_parse_odd_function:
      LDA   #'('
      JSR   p_expect_char
      BCS   @fail
      JSR   p_parse_expression
      BCS   @fail
      LDA   p_expr_type
      CMP   #TYPE_BYTE
      BEQ   @ordinal
      CMP   #TYPE_WORD
      BEQ   @ordinal
      CMP   #TYPE_INTEGER
      BEQ   @ordinal
      CMP   #TYPE_CHAR
      BEQ   @ordinal
      CMP   #P_TYPE_USER_BASE
      BCC   @fail
      SEC
      SBC   #P_TYPE_USER_BASE
      TAX
      CPX   p_type_count
      BCS   @fail
      LDA   p_type_kind,X
      CMP   #TYPE_KIND_ORDINAL
      BNE   @fail
@ordinal:
      LDA   #')'
      JSR   p_expect_char
      BCS   @fail
      LDA   #<asm_odd
      LDX   #>asm_odd
      JSR   p_emit_ax_text
      BCS   @fail
      LDA   #TYPE_BOOLEAN
      STA   p_expr_type
      CLC
@fail:
      RTS

; HIGH, LOW, and SIZEOF are resolved entirely from the existing type table.
; Their argument is a type, variable, or typed constant identifier; arrays
; use their first index dimension, matching classic Pascal semantics.
p_parse_type_intrinsic:
      LDA   #'('
      JSR   p_expect_char
      long_bcs @fail
      JSR   p_capture_identifier
      long_bcs @fail
      STZ   p_intrinsic_special
      LDA   #<kw_byte
      LDX   #>kw_byte
      JSR   p_ident_is
      BCC   @boolean
      LDA   #TYPE_BYTE
      JMP   @resolved
@boolean:
      LDA   #<kw_boolean
      LDX   #>kw_boolean
      JSR   p_ident_is
      BCC   @char
      LDA   #INTRINSIC_SPECIAL_BOOLEAN
      STA   p_intrinsic_special
      LDA   #TYPE_BOOLEAN
      JMP   @resolved
@char:
      LDA   #<kw_char
      LDX   #>kw_char
      JSR   p_ident_is
      BCC   @word
      LDA   #TYPE_CHAR
      JMP   @resolved
@word:
      LDA   #<kw_word
      LDX   #>kw_word
      JSR   p_ident_is
      BCC   @integer
      LDA   #TYPE_WORD
      JMP   @resolved
@integer:
      LDA   #<kw_integer
      LDX   #>kw_integer
      JSR   p_ident_is
      BCC   @longint
      LDA   #INTRINSIC_SPECIAL_INTEGER
      STA   p_intrinsic_special
      LDA   #TYPE_INTEGER
      JMP   @resolved
@longint:
      LDA   #<kw_longint
      LDX   #>kw_longint
      JSR   p_ident_is
      BCC   @real
      LDA   #TYPE_LONGINT
      JMP   @resolved
@real:
      LDA   #<kw_real
      LDX   #>kw_real
      JSR   p_ident_is
      BCC   @string
      LDA   #TYPE_REAL
      JMP   @resolved
@string:
      LDA   #<kw_string
      LDX   #>kw_string
      JSR   p_ident_is
      BCC   @text
      LDA   #INTRINSIC_SPECIAL_STRING
      STA   p_intrinsic_special
      LDA   #TYPE_ARRAY_BYTE
      JMP   @resolved
@text:
      LDA   #<kw_text
      LDX   #>kw_text
      JSR   p_ident_is
      BCC   @file
      LDA   #TYPE_TEXT
      JMP   @resolved
@file:
      LDA   #<kw_file
      LDX   #>kw_file
      JSR   p_ident_is
      BCC   @named
      LDA   #TYPE_FILE
      JMP   @resolved
@named:
      JSR   p_type_find
      BCS   @resolved
      JSR   p_symbol_find
      BCC   @constant
      LDA   p_symbol_type
      JSR   p_type_open_element
      BCC   @symbol
      LDA   #INTRINSIC_SPECIAL_OPEN_ARRAY
      STA   p_intrinsic_special
@symbol:
      LDA   p_symbol_type
      JMP   @resolved
@constant:
      JSR   p_const_find
      long_bcc @fail
@resolved:
      STA   p_decl_type
      LDA   #')'
      JSR   p_expect_char
      long_bcs @fail
      LDA   p_intrinsic_special
      CMP   #INTRINSIC_SPECIAL_OPEN_ARRAY
      long_beq @open_array
      LDA   p_intrinsic_kind
      CMP   #INTRINSIC_SIZEOF
      long_bne @bounds
      LDA   p_intrinsic_special
      CMP   #INTRINSIC_SPECIAL_STRING
      BNE   @sizeof_type
      STZ   p_decimal
      LDA   #1
      STA   p_decimal+1
      JMP   @emit_word
@sizeof_type:
      LDA   p_decl_type
      JSR   p_type_size
      long_bcs @fail
      LDA   p_array_len
      STA   p_decimal
      LDA   p_array_len+1
      STA   p_decimal+1
      JMP   @emit_word
@open_array:
      LDA   p_intrinsic_kind
      CMP   #INTRINSIC_LOW
      BNE   @open_high
      STZ   p_decimal
      STZ   p_decimal+1
      JMP   @emit_word
@open_high:
      JSR   p_emit_open_array_high
      long_bcs @fail
      LDA   p_intrinsic_kind
      CMP   #INTRINSIC_HIGH
      BEQ   @open_done
      LDA   #<asm_increment_word
      LDX   #>asm_increment_word
      JSR   p_emit_ax_text
      long_bcs @fail
      LDA   p_decl_type
      JSR   p_type_open_element
      long_bcc @fail
      JSR   p_type_size
      long_bcs @fail
      LDA   p_array_len+1
      BNE   @open_multiply
      LDA   p_array_len
      CMP   #1
      BEQ   @open_done
@open_multiply:
      LDA   #<asm_rhs_word
      LDX   #>asm_rhs_word
      JSR   p_emit_ax_text
      long_bcs @fail
      LDA   p_array_len
      STA   p_decimal
      LDA   p_array_len+1
      STA   p_decimal+1
      JSR   p_emit_load_word
      long_bcs @fail
      LDA   #<asm_multiply_word
      LDX   #>asm_multiply_word
      JSR   p_emit_ax_text
      long_bcs @fail
@open_done:
      LDA   #TYPE_WORD
      STA   p_expr_type
      CLC
      RTS
@bounds:
      LDA   p_intrinsic_special
      BEQ   @ordinary_bounds
      CMP   #INTRINSIC_SPECIAL_BOOLEAN
      BNE   @integer_bounds
      STZ   p_decimal
      STZ   p_decimal+1
      LDA   p_intrinsic_kind
      long_beq @emit_bound
      INC   p_decimal
      JMP   @emit_bound
@integer_bounds:
      CMP   #INTRINSIC_SPECIAL_INTEGER
      BNE   @string_bounds
      STZ   p_decimal
      LDA   p_intrinsic_kind
      BEQ   @integer_low
      DEC   p_decimal
      LDA   #$7F
      BRA   @integer_high_byte
@integer_low:
      LDA   #$80
@integer_high_byte:
      STA   p_decimal+1
      JMP   @emit_bound
@string_bounds:
      CMP   #INTRINSIC_SPECIAL_STRING
      long_bne @fail
      STZ   p_decimal+1
      LDA   p_intrinsic_kind
      BEQ   @string_low
      LDA   #$FF
      BRA   @string_value
@string_low:
      LDA   #0
@string_value:
      STA   p_decimal
      JMP   @emit_bound
@ordinary_bounds:
      LDA   p_decl_type
      CMP   #TYPE_BYTE
      long_beq @byte_bounds
      CMP   #TYPE_CHAR
      long_beq @byte_bounds
      CMP   #TYPE_WORD
      long_beq @word_bounds
      CMP   #TYPE_INTEGER
      long_beq @integer_direct_bounds
      CMP   #P_TYPE_USER_BASE
      long_bcc @fail
      SEC
      SBC   #P_TYPE_USER_BASE
      TAX
      CPX   p_type_count
      long_bcs @fail
      LDA   p_type_kind,X
      CMP   #TYPE_KIND_ORDINAL
      BEQ   @user_ordinal
      CMP   #TYPE_KIND_STRING
      BEQ   @user_string
      CMP   #TYPE_KIND_ARRAY
      long_bne @fail
      LDA   p_type_low_l,X
      TAX
      CPX   p_dim_count
      long_bcs @fail
      LDA   p_intrinsic_kind
      BEQ   @array_low
      CLC
      LDA   p_dim_low_l,X
      ADC   p_dim_extent_l,X
      STA   p_decimal
      LDA   p_dim_low_h,X
      ADC   p_dim_extent_h,X
      STA   p_decimal+1
      LDA   p_decimal
      BNE   :+
      DEC   p_decimal+1
:     DEC   p_decimal
      JMP   @emit_bound
@array_low:
      LDA   p_dim_low_l,X
      STA   p_decimal
      LDA   p_dim_low_h,X
      STA   p_decimal+1
      BRA   @emit_bound
@user_ordinal:
      LDA   p_intrinsic_kind
      BEQ   @user_low
      LDA   p_type_high_l,X
      STA   p_decimal
      LDA   p_type_high_h,X
      STA   p_decimal+1
      BRA   @emit_bound
@user_low:
      LDA   p_type_low_l,X
      STA   p_decimal
      LDA   p_type_low_h,X
      STA   p_decimal+1
      BRA   @emit_bound
@user_string:
      STZ   p_decimal
      STZ   p_decimal+1
      LDA   p_intrinsic_kind
      BEQ   @emit_bound
      LDA   p_type_high_l,X
      STA   p_decimal
      LDA   p_type_high_h,X
      STA   p_decimal+1
      BRA   @emit_bound
@byte_bounds:
      STZ   p_decimal+1
      LDA   p_intrinsic_kind
      BEQ   @byte_low
      LDA   #$FF
      BRA   @byte_value
@byte_low:
      LDA   #0
@byte_value:
      STA   p_decimal
      BRA   @emit_bound
@word_bounds:
      STZ   p_decimal
      STZ   p_decimal+1
      LDA   p_intrinsic_kind
      BEQ   @emit_bound
      DEC   p_decimal
      DEC   p_decimal+1
      BRA   @emit_bound
@integer_direct_bounds:
      STZ   p_decimal
      LDA   p_intrinsic_kind
      BEQ   @integer_direct_low
      DEC   p_decimal
      LDA   #$7F
      BRA   @integer_direct_high
@integer_direct_low:
      LDA   #$80
@integer_direct_high:
      STA   p_decimal+1
@emit_bound:
      LDA   p_decimal+1
      BNE   @emit_word
      LDA   p_decimal
      JSR   p_emit_load_byte
      BCS   @fail
      LDA   #TYPE_BYTE
      STA   p_expr_type
      CLC
      RTS
@emit_word:
      JSR   p_emit_load_word
      BCS   @fail
      LDA   p_decl_type
      JSR   p_type_is_signed_ordinal
      BEQ   :+
      LDA   #TYPE_INTEGER
      BRA   :++
:     LDA   #TYPE_WORD
:     STA   p_expr_type
      CLC
@fail:
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
      JSR   p_type_is_word_value
      BNE   @count_ready
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

; ORD is a representation-preserving conversion for Nova's byte- and
; word-sized ordinal types.
p_parse_ord_function:
      LDA   #'('
      JSR   p_expect_char
      BCS   @fail
      JSR   p_parse_expression
      BCS   @fail
      LDA   p_expr_type
      CMP   #TYPE_BYTE
      BEQ   @sized
      CMP   #TYPE_WORD
      BEQ   @sized
      CMP   #TYPE_INTEGER
      BEQ   @sized
      CMP   #TYPE_CHAR
      BEQ   @sized
      CMP   #P_TYPE_USER_BASE
      BCC   @fail
      SEC
      SBC   #P_TYPE_USER_BASE
      TAX
      CPX   p_type_count
      BCS   @fail
      LDA   p_type_kind,X
      CMP   #TYPE_KIND_ORDINAL
      BNE   @fail
      LDA   p_expr_type
@sized:
      JSR   p_type_size
      BCS   @fail
      LDA   p_array_len+1
      BNE   @word
      LDA   p_array_len
      CMP   #2
      BCS   @word
      LDA   #TYPE_BYTE
      BRA   @result
@word:
      LDA   p_expr_type
      JSR   p_type_is_signed_ordinal
      BEQ   :+
      LDA   #TYPE_INTEGER
      BRA   :++
:     LDA   #TYPE_WORD
@result:
      STA   p_expr_type
      LDA   #')'
      JSR   p_expect_char
@fail:
      RTS

; POS(needle, haystack) normalizes Char operands to one-character strings and
; returns the classic one-based byte position, or zero.
p_parse_pos_function:
      LDA   #'('
      JSR   p_expect_char
      long_bcs @fail
      JSR   p_parse_expression
      long_bcs @fail
      LDA   p_expr_type
      JSR   p_type_is_string
      BCS   @needle_ready
      LDA   p_expr_type
      JSR   p_type_is_char
      long_bcc @fail
      LDA   #<asm_char_to_string
      LDX   #>asm_char_to_string
      JSR   p_emit_ax_text
      long_bcs @fail
@needle_ready:
      LDA   #<asm_phx_pha
      LDX   #>asm_phx_pha
      JSR   p_emit_ax_text
      long_bcs @fail
      LDA   #','
      JSR   p_expect_char
      long_bcs @fail
      JSR   p_parse_expression
      long_bcs @fail
      LDA   p_expr_type
      JSR   p_type_is_string
      BCS   @haystack_ready
      LDA   p_expr_type
      JSR   p_type_is_char
      long_bcc @fail
      LDA   #<asm_char_to_string
      LDX   #>asm_char_to_string
      JSR   p_emit_ax_text
      long_bcs @fail
@haystack_ready:
      LDA   #')'
      JSR   p_expect_char
      long_bcs @fail
      LDA   #<asm_string_pos
      LDX   #>asm_string_pos
      JSR   p_emit_ax_text
      long_bcs @fail
      LDA   #TYPE_BYTE
      STA   p_expr_type
      CLC
@fail:
      RTS

; Parse one unsigned 32-bit decimal literal into p_decimal without emitting code.
p_parse_decimal_literal:
      STZ   p_decimal
      STZ   p_decimal+1
      STZ   p_decimal+2
      STZ   p_decimal+3
      STZ   p_started
@decimal_digit:
      JSR   p_peek
      long_bcc @decimal_done
      CMP   #'0'
      long_bcc @decimal_done
      CMP   #'9'+1
      long_bcs @decimal_done
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
      LDA   p_decimal+2
      ROL
      STA   p_remainder+2
      LDA   p_decimal+3
      ROL
      STA   p_remainder+3
      long_bcs @bad
      ASL   p_decimal
      ROL   p_decimal+1
      ROL   p_decimal+2
      ROL   p_decimal+3
      long_bcs @bad
      ASL   p_decimal
      ROL   p_decimal+1
      ROL   p_decimal+2
      ROL   p_decimal+3
      long_bcs @bad
      ASL   p_decimal
      ROL   p_decimal+1
      ROL   p_decimal+2
      ROL   p_decimal+3
      long_bcs @bad
      CLC
      LDA   p_decimal
      ADC   p_remainder
      STA   p_decimal
      LDA   p_decimal+1
      ADC   p_remainder+1
      STA   p_decimal+1
      LDA   p_decimal+2
      ADC   p_remainder+2
      STA   p_decimal+2
      LDA   p_decimal+3
      ADC   p_remainder+3
      STA   p_decimal+3
      long_bcs @bad
      CLC
      LDA   p_decimal
      ADC   p_digit
      STA   p_decimal
      LDA   p_decimal+1
      ADC   #0
      STA   p_decimal+1
      LDA   p_decimal+2
      ADC   #0
      STA   p_decimal+2
      LDA   p_decimal+3
      ADC   #0
      STA   p_decimal+3
      long_bcs @bad
      JSR   p_next
      JMP   @decimal_digit
@decimal_done:
      LDA   p_started
      BEQ   @bad
      CLC
      RTS
@bad:
      SEC
      RTS

; Parse one unsigned 32-bit Turbo Pascal $hex literal into p_decimal.
p_parse_hex_literal:
      STZ   p_decimal
      STZ   p_decimal+1
      STZ   p_decimal+2
      STZ   p_decimal+3
      STZ   p_started
@digit:
      JSR   p_peek_upper
      BCC   @done
      JSR   p_hex_nibble
      BCC   @done
      STA   p_digit
      INC   p_started
      LDA   p_started
      CMP   #9
      BCS   @bad
      LDY   #4
@shift:
      ASL   p_decimal
      ROL   p_decimal+1
      ROL   p_decimal+2
      ROL   p_decimal+3
      BCS   @bad
      DEY
      BNE   @shift
      LDA   p_decimal
      ORA   p_digit
      STA   p_decimal
      JSR   p_next
      BRA   @digit
@done:
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

; Generated assembly is an ordinary sequential disk stream. A single page in
; overlay RAM is enough regardless of source size, and all ownership stays in
; the NDK Files module instead of a compiler-private XRAM convention.
p_files_call:
      JSR   nptool_files_call
      STZ   p_window_valid
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

; ARG5 optionally names a transient dependency manifest. Record only files
; the compiler actually opened; NBUILD persists their exact content hashes.
p_open_dependencies:
      STZ   p_dependency_open
      LDA   NPTOOL_FLAGS
      AND   #NPTOOL_FLAG_DEPENDENCIES
      BEQ   @ok
      LDA   NPTOOL_ARG5_LEN
      BEQ   @ok
      JSR   p_clear_lib_args
      LDA   #<NPTOOL_ARG5
      STA   LIB_ARG0
      LDA   #>NPTOOL_ARG5
      STA   LIB_ARG0+1
      LDA   NPTOOL_ARG5_LEN
      STA   LIB_ARG1
      LDA   #FIO_FILE_ACCESS_WRITE
      STA   LIB_ARG2
      LDA   #FILE_FCREATE
      JSR   p_files_call
      BNE   @bad
      LDA   LIB_RESULT
      STA   p_dependency_file_id
      LDA   LIB_RESULT+1
      STA   p_dependency_file_id+1
      INC   p_dependency_open
      LDA   #<NPTOOL_ARG0
      LDX   #>NPTOOL_ARG0
      LDY   NPTOOL_ARG0_LEN
      JSR   p_write_dependency_named
      BNE   @bad
      LDA   NPTOOL_ARG4_LEN
      BEQ   @ok
      LDA   #<NPTOOL_ARG4
      LDX   #>NPTOOL_ARG4
      LDY   NPTOOL_ARG4_LEN
      JSR   p_write_dependency_named
      BNE   @bad
@ok:
      LDA   #0
      RTS
@bad:
      LDA   #1
      RTS

p_write_include_dependency:
      LDA   #<p_include_name
      LDX   #>p_include_name
      LDY   p_include_name_len

; A/X points at one exact filename and Y is its length.
p_write_dependency_named:
      PHA
      PHX
      PHY
      LDA   p_dependency_open
      BEQ   @ok_pop
      PLY
      STY   p_expected
      PLX
      PLA
      STA   p_word
      STX   p_word+1
      JSR   p_clear_lib_args
      LDA   p_dependency_file_id
      STA   LIB_ARG0
      LDA   p_dependency_file_id+1
      STA   LIB_ARG0+1
      LDA   p_word
      STA   LIB_ARG1
      LDA   p_word+1
      STA   LIB_ARG1+1
      LDA   p_expected
      STA   LIB_ARG2
      LDA   #FILE_FWRITE
      JSR   p_files_call
      BNE   @bad
      LDA   LIB_RESULT
      CMP   p_expected
      BNE   @bad
      LDA   LIB_RESULT+1
      BNE   @bad
      JSR   p_clear_lib_args
      LDA   p_dependency_file_id
      STA   LIB_ARG0
      LDA   p_dependency_file_id+1
      STA   LIB_ARG0+1
      LDA   #<p_dependency_lf
      STA   LIB_ARG1
      LDA   #>p_dependency_lf
      STA   LIB_ARG1+1
      LDA   #1
      STA   LIB_ARG2
      LDA   #FILE_FWRITE
      JSR   p_files_call
      BNE   @bad
      LDA   LIB_RESULT
      CMP   #1
      BNE   @bad
      LDA   LIB_RESULT+1
      BNE   @bad
      LDA   #0
      RTS
@ok_pop:
      PLY
      PLX
      PLA
      LDA   #0
      RTS
@bad:
      LDA   #1
      RTS

p_close_dependencies:
      LDA   p_dependency_open
      BEQ   @ok
      STZ   p_dependency_open
      JSR   p_clear_lib_args
      LDA   p_dependency_file_id
      STA   LIB_ARG0
      LDA   p_dependency_file_id+1
      STA   LIB_ARG0+1
      LDA   #FILE_FCLOSE
      JMP   p_files_call
@ok:
      LDA   #0
      RTS

p_delete_dependencies:
      LDA   NPTOOL_ARG5_LEN
      BEQ   @done
      JSR   p_clear_lib_args
      LDA   #<NPTOOL_ARG5
      STA   LIB_ARG0
      LDA   #>NPTOOL_ARG5
      STA   LIB_ARG0+1
      LDA   NPTOOL_ARG5_LEN
      STA   LIB_ARG1
      LDA   #FILE_FDELETE
      JSR   p_files_call
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
      LDA   #<source_buf
      STA   LIB_ARG1
      LDA   #>source_buf
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

p_finalize_dependencies:
      JSR   p_close_dependencies
      BEQ   @closed
      LDA   p_frontend_status
      BNE   @discard
      LDA   #PASCAL_ERR_OUTPUT
      STA   pascal_error
      LDA   #1
      STA   p_frontend_status
@discard:
      JSR   p_delete_dependencies
      RTS
@closed:
      LDA   p_frontend_status
      BEQ   @done
      JSR   p_delete_dependencies
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
      long_bcs p_open_include_fail
      JSR   p_save_include_leaf
      LDA   p_include_depth
      CMP   p_include_count
      BCC   @reuse
      LDA   p_include_count
      CMP   #P_INCLUDE_COUNT_CAP
      long_bcs p_open_include_fail

      JSR   p_clear_lib_args
      LDA   #<NPC_INCLUDE_CAP
      STA   LIB_ARG2
      LDA   #>NPC_INCLUDE_CAP
      STA   LIB_ARG2+1
      LDA   #MEM_ALLOC
      JSR   p_memory_call
      long_bne p_open_include_alloc_fail
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
      JSR   p_load_include_name
      BEQ   @loaded
      STZ   p_include_path_index
@path:
      LDA   p_include_path_index
      CMP   NPP_PLAN_BASE+NPP_PLAN_UNIT_PATH_COUNT
      BCS   @load_fail_restore
      JSR   p_build_include_path
      BCS   @next_path
      JSR   p_load_include_name
      BEQ   @loaded
@next_path:
      INC   p_include_path_index
      BRA   @path
@load_fail_restore:
      JSR   p_restore_include_leaf
      JMP   p_open_include_load_fail
@loaded:
      JSR   p_write_include_dependency
      BNE   @dependency_fail
      JSR   p_restore_include_leaf
      LDA   XRAM_LENL
      STA   p_include_len
      LDA   XRAM_LENH
      STA   p_include_len+1
      JMP   p_open_include_activate
@dependency_fail:
      JSR   p_restore_include_leaf
      JMP   p_open_include_load_fail

p_load_include_name:
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
      JMP   p_memory_call

; Includes first probe the project directory, then the generic UNITPATH roots
; already validated by NPPROJ. The Memory module remains the sole file/XRAM
; owner; NPC only constructs the same root-relative name NAS consumes.
p_save_include_leaf:
      LDA   p_include_name_len
      STA   p_include_leaf_len
      TAX
@save_leaf:
      DEX
      BMI   @saved_leaf
      LDA   p_include_name,X
      STA   p_include_leaf,X
      BRA   @save_leaf
@saved_leaf:
      RTS

p_restore_include_leaf:
      LDA   p_include_leaf_len
      STA   p_include_name_len
      TAX
@restore_leaf:
      DEX
      BMI   @terminate_leaf
      LDA   p_include_leaf,X
      STA   p_include_name,X
      BRA   @restore_leaf
@terminate_leaf:
      LDX   p_include_name_len
      LDA   #0
      STA   p_include_name,X
      RTS

p_build_include_path:
      LDA   p_include_path_index
      BNE   @second
      LDA   #<(NPP_PLAN_BASE+NPP_PLAN_UNIT_PATHS)
      LDX   #>(NPP_PLAN_BASE+NPP_PLAN_UNIT_PATHS)
      BRA   @selected
@second:
      LDA   #<(NPP_PLAN_BASE+NPP_PLAN_UNIT_PATHS+NPP_UNIT_PATH_SIZE)
      LDX   #>(NPP_PLAN_BASE+NPP_PLAN_UNIT_PATHS+NPP_UNIT_PATH_SIZE)
@selected:
      STA   p_word
      STX   p_word+1
      LDY   #0
      LDA   (p_word),Y
      STA   p_remainder
      CLC
      ADC   p_include_leaf_len
      ADC   #2
      CMP   #P_INCLUDE_LOAD_CAP
      BCS   @path_bad
      STA   p_include_name_len
      LDX   #0
      LDA   #'/'
      STA   p_include_name,X
      INX
      LDY   #1
@copy_path:
      LDA   p_remainder
      BEQ   @separator
      LDA   (p_word),Y
      STA   p_include_name,X
      INX
      INY
      DEC   p_remainder
      BRA   @copy_path
@separator:
      LDA   #'/'
      STA   p_include_name,X
      INX
      LDY   #0
@copy_leaf:
      CPY   p_include_leaf_len
      BCS   @path_done
      LDA   p_include_leaf,Y
      STA   p_include_name,X
      INX
      INY
      BRA   @copy_leaf
@path_done:
      LDA   #0
      STA   p_include_name,X
      CLC
      RTS
@path_bad:
      SEC
      RTS

p_open_include_activate:

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

      JSR   p_include_name_pointer
      LDY   #0
@save_name:
      LDA   p_current_name,Y
      STA   (p_word),Y
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
p_open_include_load_fail:
      LDA   #2
      STA   p_source_detail
      BRA   p_open_include_fail
p_open_include_alloc_fail:
      LDA   #1
      STA   p_source_detail
p_open_include_fail:
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

      JSR   p_include_name_pointer
      LDY   #0
@restore_name:
      LDA   (p_word),Y
      STA   p_current_name,Y
      INY
      CPY   #P_SOURCE_NAME_CAP
      BCC   @restore_name
      STZ   p_window_valid
      CLC
      RTS
@none:
      SEC
      RTS

p_include_name_pointer:
      LDA   p_include_depth
      ASL
      ASL
      ASL
      ASL
      ASL
      CLC
      ADC   #<P_STATE_INCLUDE_NAME_OFFSET
      STA   p_state_offset
      LDA   #>P_STATE_INCLUDE_NAME_OFFSET
      ADC   #0
      STA   p_state_offset+1
      JMP   p_state_pointer

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

; Called after "{$". The classic R/Q/I/S switches update compiler state;
; comma-separated switches share one directive. {$I file} changes the active
; source frame after consuming the directive.
; ---------------------------------------------------------------------------
; Conditional compilation.
;
; p_cond_depth counts open {$IFDEF}/{$IFNDEF} levels. p_cond_skip is zero while
; source is being compiled, otherwise it holds the depth at which the inactive
; region began. Because Turbo has no {$ELSEIF}, that single depth is enough to
; decide when {$ELSE} and {$ENDIF} resume compilation, and nested conditionals
; inside an inactive region are tracked by depth alone without being evaluated.
;
; The directive's leading letter is already in p_char.
p_cond_word:
      LDA   p_char
      STA   p_ident
      LDA   #1
      STA   p_ident_len
@more:
      JSR   p_peek_upper
      long_bcc p_cond_dispatch
      JSR   p_is_ident
      long_bcc p_cond_dispatch
      JSR   p_read_upper
      long_bcc p_cond_fail
      LDX   p_ident_len
      CPX   #P_IDENT_CAP
      long_bcs p_cond_fail
      STA   p_ident,X
      INC   p_ident_len
      BRA   @more

; Act on the directive named in p_ident, then consume through the closing brace
; and skip an inactive region if one is now open.
p_cond_dispatch:
      LDA   #<kw_ifdef
      LDX   #>kw_ifdef
      JSR   p_ident_is
      BCS   p_cond_ifdef
      LDA   #<kw_ifndef
      LDX   #>kw_ifndef
      JSR   p_ident_is
      BCS   p_cond_ifndef
      LDA   #<kw_else
      LDX   #>kw_else
      JSR   p_ident_is
      BCS   p_cond_else
      LDA   #<kw_endif
      LDX   #>kw_endif
      JSR   p_ident_is
      BCS   p_cond_endif
      LDA   #<kw_define
      LDX   #>kw_define
      JSR   p_ident_is
      long_bcs p_cond_define
      LDA   #<kw_undef
      LDX   #>kw_undef
      JSR   p_ident_is
      long_bcs p_cond_undef
      ; Unknown directive: Turbo ignores these, so consume it and continue.
      JMP   p_cond_close

p_cond_ifdef:
      JSR   p_cond_open
      long_bcs p_cond_fail
      JSR   p_cond_symbol_defined
      long_bcs p_cond_close                 ; defined: compile this branch
      JMP   p_cond_begin_skip
p_cond_ifndef:
      JSR   p_cond_open
      long_bcs p_cond_fail
      JSR   p_cond_symbol_defined
      long_bcc p_cond_close
p_cond_begin_skip:
      ; Already inside an inactive region? The outer skip depth must win.
      LDA   p_cond_skip
      BNE   p_cond_close
      LDA   p_cond_depth
      STA   p_cond_skip
      JMP   p_cond_close

p_cond_else:
      LDA   p_cond_depth
      BEQ   p_cond_fail                  ; {$ELSE} without {$IFDEF}
      LDA   p_cond_skip
      BEQ   @start_skip
      CMP   p_cond_depth
      BNE   p_cond_close                 ; inactive region opened further out
      STZ   p_cond_skip                  ; this branch resumes compilation
      JMP   p_cond_close
@start_skip:
      LDA   p_cond_depth
      STA   p_cond_skip
      JMP   p_cond_close

p_cond_endif:
      LDA   p_cond_depth
      BEQ   p_cond_fail                  ; {$ENDIF} without {$IFDEF}
      LDA   p_cond_skip
      BEQ   @pop
      CMP   p_cond_depth
      BNE   @pop
      STZ   p_cond_skip
@pop:
      DEC   p_cond_depth
      JMP   p_cond_close

p_cond_define:
      JSR   p_capture_identifier
      long_bcs p_cond_fail
      LDA   p_cond_skip
      BNE   p_cond_close
      JSR   p_cond_find
      BCS   p_cond_close                 ; already defined
      JSR   p_cond_add
      BCS   p_cond_fail                  ; pool exhausted
      JMP   p_cond_close

p_cond_undef:
      JSR   p_capture_identifier
      long_bcs p_cond_fail
      LDA   p_cond_skip
      BNE   p_cond_close
      JSR   p_cond_find
      long_bcc p_cond_close
      JSR   p_cond_remove

; Consume the rest of the directive, then skip source while a region is
; inactive. Returning here always leaves the scanner past the closing brace.
p_cond_close:
      JSR   p_peek
      long_bcc p_cond_fail
      CMP   #'}'
      BEQ   @close
      JSR   p_next
      JMP   p_cond_close
@close:
      JSR   p_next
      LDA   p_cond_skip
      BNE   p_cond_skip_region
      CLC
      RTS
p_cond_fail:
      SEC
      RTS


; Carry set when the symbol operand is currently defined.
p_cond_symbol_defined:
      JSR   p_capture_identifier
      BCS   @no
      JMP   p_cond_find
@no:
      CLC
      RTS

; Push a conditional level. Carry set when nesting is exhausted.
p_cond_open:
      LDA   p_cond_depth
      CMP   #$FF
      BCS   @full
      INC   p_cond_depth
      CLC
      RTS
@full:
      SEC
      RTS

; Consume source until the inactive region ends, tracking nested conditionals
; but evaluating none of them.
p_cond_skip_region:
@scan:
      JSR   p_next
      BCC   p_cond_fail                  ; end of source inside {$IFDEF}
      CMP   #'{'
      BNE   @scan
      JSR   p_peek
      BCC   p_cond_fail
      CMP   #'$'
      BNE   @comment
      JSR   p_next
      JSR   p_capture_identifier
      BCS   @comment
      JSR   p_cond_skip_dispatch
      BCS   p_cond_fail
@comment:
      JSR   p_peek
      BCC   p_cond_fail
      CMP   #'}'
      BEQ   @closed
      JSR   p_next
      BRA   @comment
@closed:
      JSR   p_next
      LDA   p_cond_skip
      BNE   @scan
      CLC
      RTS

; Only the four nesting directives matter inside an inactive region.
p_cond_skip_dispatch:
      LDA   #<kw_ifdef
      LDX   #>kw_ifdef
      JSR   p_ident_is
      BCS   p_cond_open
      LDA   #<kw_ifndef
      LDX   #>kw_ifndef
      JSR   p_ident_is
      BCS   p_cond_open
      LDA   #<kw_else
      LDX   #>kw_else
      JSR   p_ident_is
      BCS   @else
      LDA   #<kw_endif
      LDX   #>kw_endif
      JSR   p_ident_is
      BCS   @endif
      CLC
      RTS
@else:
      LDA   p_cond_skip
      CMP   p_cond_depth
      BNE   @ok
      STZ   p_cond_skip
@ok:
      CLC
      RTS
@endif:
      LDA   p_cond_depth
      BEQ   @bad
      LDA   p_cond_skip
      CMP   p_cond_depth
      BNE   @pop
      STZ   p_cond_skip
@pop:
      DEC   p_cond_depth
      CLC
      RTS
@bad:
      SEC
      RTS

; --- conditional symbol pool ------------------------------------------------
; Slot X lives at P_STATE_COND_NAME_OFFSET + X * P_IDENT_CAP in the XRAM state.
p_cond_slot_pointer:
      TXA
      ASL
      ASL
      ASL
      ASL
      ASL
      CLC
      ADC   #<P_STATE_COND_NAME_OFFSET
      STA   p_state_offset
      LDA   #>P_STATE_COND_NAME_OFFSET
      ADC   #0
      STA   p_state_offset+1
      JMP   p_state_pointer

; Carry set when p_ident matches an occupied slot; X holds that slot.
p_cond_find:
      LDX   #0
@slot:
      PHX
      JSR   p_cond_slot_pointer
      LDY   #0
      LDA   (p_word),Y
      BEQ   @next                        ; free slot
      JSR   p_cond_slot_matches
      BCS   @found
@next:
      PLX
      INX
      CPX   #P_COND_CAP
      BNE   @slot
      CLC
      RTS
@found:
      PLX
      SEC
      RTS

; Compare the slot at (p_word) with p_ident/p_ident_len.
p_cond_slot_matches:
      LDY   #0
@char:
      CPY   p_ident_len
      BEQ   @end
      LDA   (p_word),Y
      BEQ   @no
      CMP   p_ident,Y
      BNE   @no
      INY
      BRA   @char
@end:
      LDA   (p_word),Y
      BNE   @no
      SEC
      RTS
@no:
      CLC
      RTS

; Store p_ident in the first free slot. Carry set when the pool is full.
p_cond_add:
      LDX   #0
@slot:
      PHX
      JSR   p_cond_slot_pointer
      LDY   #0
      LDA   (p_word),Y
      BEQ   @store
      PLX
      INX
      CPX   #P_COND_CAP
      BNE   @slot
      SEC
      RTS
@store:
      PLX
      LDY   #0
@char:
      CPY   p_ident_len
      BEQ   @terminate
      LDA   p_ident,Y
      STA   (p_word),Y
      INY
      BRA   @char
@terminate:
      LDA   #0
      STA   (p_word),Y
      CLC
      RTS

; Free the slot in X.
p_cond_remove:
      JSR   p_cond_slot_pointer
      LDY   #0
      LDA   #0
      STA   (p_word),Y
      RTS

p_parse_directive:
@switch:
      JSR   p_read_upper
      long_bcc @fail
      STA   p_char
      CMP   #'D'
      long_beq p_cond_word
      CMP   #'U'
      long_beq p_cond_word
      CMP   #'E'
      long_beq p_cond_word
      CMP   #'I'
      BNE   @classify
      JSR   p_peek_upper
      long_bcc @fail
      CMP   #'F'
      long_beq p_cond_word
      CMP   #' '
      long_beq @include
      CMP   #$09
      long_beq @include
@classify:
      STZ   p_digit
      LDA   p_char
      CMP   #'R'
      BNE   :+
      LDA   #P_CHECK_RANGE
      BRA   @mode
:     CMP   #'Q'
      BNE   :+
      LDA   #P_CHECK_OVERFLOW
      BRA   @mode
:     CMP   #'I'
      BNE   :+
      LDA   #P_CHECK_IO
      BRA   @mode
:     CMP   #'S'
      BNE   @tail
      LDA   #P_CHECK_STACK
@mode:
      STA   p_digit
      JSR   p_peek
      long_bcc @fail
      CMP   #'+'
      BEQ   @enable
      CMP   #'-'
      BNE   @tail
      JSR   p_next
      LDA   p_digit
      EOR   #$FF
      AND   p_check_flags
      STA   p_check_flags
      BRA   @tail
@enable:
      JSR   p_next
      LDA   p_check_flags
      ORA   p_digit
      STA   p_check_flags
@tail:
      JSR   p_peek
      long_bcc @fail
      CMP   #'}'
      BEQ   @done
      CMP   #','
      BEQ   @next_switch
      JSR   p_next
      BRA   @tail
@next_switch:
      JSR   p_next
@switch_space:
      JSR   p_peek
      long_bcc @fail
      CMP   #' '
      BEQ   @eat_switch_space
      CMP   #$09
      long_bne @switch
@eat_switch_space:
      JSR   p_next
      BRA   @switch_space
@done:
      JSR   p_next
      CLC
      RTS

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
p_dependency_lf: .byte $0A
SYSTEM_CALL_METADATA_COUNT = 32
SYSTEM_CALL_VAL = 9
SYSTEM_CALL_INSERT = 10
SYSTEM_FUNCTION_COUNT = 19
SYSTEM_NOARG_FUNCTION_COUNT = 4
SYSTEM_STORAGE_COUNT = 1
p_argument_bits:  .byte $01, $02, $04, $08, $10, $20, $40, $80
; Hash bytes are little-endian DJB2-24 values. Arity disambiguates the two
; classic READ and READLN forms.
; RANDOMIZE is the zero-argument entry; the file-handle operations follow it.
; Reference and Integer-width masks for the classic System interface plus the
; Pascal-shaped NovaMemory adapter. Hashes remain generated ABI identities;
; no NDK address or function id is embedded in NPC.
system_call_hash0: .byte $C1,$C1,$1B,$1B,$EA,$E8,$B8,$27,$BB,$68,$DA,$CF,$C8,$5F,$19,$08,$D0,$67,$21,$10,$94,$1C,$0A,$0E,$AD,$6B,$C7,$75,$5D,$1D,$6C,$BB
system_call_hash1: .byte $34,$34,$73,$73,$0A,$19,$47,$93,$71,$29,$C5,$44,$6B,$65,$C2,$C5,$52,$8B,$E8,$AC,$A4,$A4,$21,$23,$C1,$6B,$D4,$B1,$87,$0C,$94,$E1
system_call_hash2: .byte $8B,$8B,$2B,$2B,$8B,$F2,$89,$2F,$E6,$88,$D9,$42,$E3,$B4,$BA,$74,$B3,$1F,$25,$44,$F4,$88,$DC,$4E,$8B,$64,$1C,$0D,$32,$53,$BA,$6E
system_call_arity: .byte $01,$02,$01,$02,$02,$01,$03,$01,$01,$03,$03,$02,$02,$01,$04,$04,$02,$01,$04,$04,$01,$03,$03,$00,$02,$01,$01,$01,$02,$01,$03,$03
system_call_refmask: .byte $01,$02,$01,$02,$01,$01,$01,$01,$01,$06,$02,$00,$01,$01,$05,$05,$01,$01,$05,$05,$00,$03,$01,$00,$01,$01,$01,$01,$01,$01,$03,$03
system_call_widthmask: .byte $00,$00,$00,$00,$00,$00,$06,$00,$00,$00,$04,$03,$02,$00,$0A,$0A,$02,$00,$0A,$0A,$01,$04,$06,$00,$02,$00,$00,$00,$00,$00,$04,$04
      .segment "NPCFE_RODATA"
; ABS, SQRT, SQR, SIN, COS, ARCTAN, LN, EXP, TRUNC, ROUND, UPCASE, SUCC, PRED,
; RANDOM, FILEPOS, FILESIZE, HI, LO, SWAP.
system_function_hash0: .byte $3B,$6F,$BB,$AF,$6A,$5E,$5F,$12,$71,$4D,$66,$73,$D0,$66,$57,$00,$D6,$60,$C0
system_function_hash1: .byte $D0,$F6,$1E,$1D,$DA,$5A,$74,$E4,$75,$9F,$07,$05,$53,$32,$4B,$43,$73,$74,$0D
system_function_hash2: .byte $87,$8B,$88,$88,$87,$70,$59,$87,$1D,$F7,$F3,$8C,$8A,$EA,$FB,$66,$59,$59,$8C
system_function_kind:  .byte SYSFN_SAME,SYSFN_REAL,SYSFN_SAME,SYSFN_REAL
                       .byte SYSFN_REAL,SYSFN_REAL,SYSFN_REAL,SYSFN_REAL,SYSFN_WORD,SYSFN_WORD
                       .byte SYSFN_SAME,SYSFN_SAME,SYSFN_SAME,SYSFN_ORDINAL
                       .byte SYSFN_FILE,SYSFN_FILE
                       .byte SYSFN_BYTE,SYSFN_BYTE,SYSFN_ORDINAL
; Turbo Crt functions may be called without an empty parenthesized argument list.
; READKEY, WHEREX, WHEREY, IORESULT.
system_noarg_hash0: .byte $6A,$F8,$F9,$3C
system_noarg_hash1: .byte $D1,$F3,$F3,$53
system_noarg_hash2: .byte $99,$0D,$0D,$08
; Implicit System byte storage: KEYPRESSED.
system_storage_hash0: .byte $C4
system_storage_hash1: .byte $63
system_storage_hash2: .byte $BD
      .segment "RODATA"
kw_program: .byte "PROGRAM", 0
kw_unit:    .byte "UNIT", 0
kw_interface: .byte "INTERFACE", 0
kw_implementation: .byte "IMPLEMENTATION", 0
kw_external: .byte "EXTERNAL", 0
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
kw_pointer: .byte "POINTER", 0
kw_char:    .byte "CHAR", 0
kw_real:    .byte "REAL", 0
kw_string:  .byte "STRING", 0
kw_set:     .byte "SET", 0
kw_record:  .byte "RECORD", 0
kw_file:    .byte "FILE", 0
kw_text:    .byte "TEXT", 0
kw_true:    .byte "TRUE", 0
kw_false:   .byte "FALSE", 0
kw_nil:     .byte "NIL", 0
kw_array:   .byte "ARRAY", 0
kw_of:      .byte "OF", 0
      .segment "NPCFE_RODATA"
kw_longint: .byte "LONGINT", 0
kw_procedure: .byte "PROCEDURE", 0
kw_function:  .byte "FUNCTION", 0
kw_forward: .byte "FORWARD", 0
kw_write:   .byte "WRITE", 0
kw_writeln: .byte "WRITELN", 0
kw_str:     .byte "STR", 0
kw_inc:     .byte "INC", 0
kw_dec:     .byte "DEC", 0
kw_new:     .byte "NEW", 0
kw_dispose: .byte "DISPOSE", 0
kw_getmem:  .byte "GETMEM", 0
kw_freemem: .byte "FREEMEM", 0
kw_break:   .byte "BREAK", 0
kw_continue:.byte "CONTINUE", 0
kw_exit:    .byte "EXIT", 0
kw_copy:    .byte "COPY", 0
kw_ord:     .byte "ORD", 0
kw_pos:     .byte "POS", 0
kw_high:    .byte "HIGH", 0
kw_low:     .byte "LOW", 0
kw_odd:     .byte "ODD", 0
kw_sizeof:  .byte "SIZEOF", 0
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
kw_downto:  .byte "DOWNTO", 0
kw_to = kw_downto+4
kw_or = kw_for+1
kw_and:     .byte "AND", 0
kw_xor:     .byte "XOR", 0
kw_shl:     .byte "SHL", 0
kw_shr:     .byte "SHR", 0
kw_define:  .byte "DEFINE", 0
kw_undef:   .byte "UNDEF", 0
kw_ifdef:   .byte "IFDEF", 0
kw_ifndef:  .byte "IFNDEF", 0
kw_endif:   .byte "ENDIF", 0
kw_packed:  .byte "PACKED", 0
kw_absolute: .byte "ABSOLUTE", 0
kw_mod:     .byte "MOD", 0
kw_div:     .byte "DIV", 0
kw_in = kw_begin+3
hex_digits: .byte "0123456789ABCDEF"
set_bit_masks: .byte $01,$02,$04,$08,$10,$20,$40,$80
decimal_place_lo: .byte <10000, <1000, <100, <10, <1
decimal_place_hi: .byte >10000, >1000, >100, >10, >1
asm_comment: .byte "; ", 0
      .segment "RODATA"
asm_import:
      .byte ".INCLUDE ", 34, "PASCAL.INC", 34, $0A
      .byte ".INCLUDE ", 34, "PASCALRT.INC", 34, $0A
      .byte ".SEGMENT ", 34, "CODE", 34, $0A
      .byte 0
      .segment "NPCFE_RODATA"
asm_bss:
      np_lowering_marker NP_LOWER_SEGMENT_BSS
asm_rodata:
      np_lowering_marker NP_LOWER_SEGMENT_RODATA
asm_code:
      np_lowering_marker NP_LOWER_SEGMENT_CODE
asm_code_with_routines:
      np_lowering_marker NP_LOWER_CODE_WITH_ROUTINES
asm_main_label: .byte "__NP_MAIN:", $0A, 0
asm_overlay_return: .byte "LDA #0", $0A, 0
asm_main_label_frames:
      np_lowering_marker NP_LOWER_MAIN_LABEL_FRAMES
asm_absolute_eq: .byte " = $", 0
asm_byte_res: .byte ": .RES 1", $0A, 0
asm_word_res: .byte ": .RES 2", $0A, 0
asm_array_res: .byte ": .RES $", 0
asm_const_bytes: .byte ": .BYTE ", 0
asm_const_cont: .byte $0A, ".BYTE ", 0
asm_string_bytes: .byte ".BYTE $", 0
asm_set_bytes: .byte ".BYTE ", 0
asm_load_string_lo: .byte "LDA #<__NP_L", 0
asm_load_string_hi: .byte "LDX #>__NP_L", 0
asm_save_incoming:
      np_lowering_marker NP_LOWER_SAVE_INCOMING
asm_open_base_save:
      np_lowering_marker NP_LOWER_OPEN_BASE_SAVE
asm_open_base_restore:
      np_lowering_marker NP_LOWER_OPEN_BASE_RESTORE

      .segment "RODATA"
asm_frame_enter_call: .byte "JSR P_FENTER", $0A, ".BYTE $", 0
asm_frame_enter_size: .byte $0A, ".WORD $", 0
asm_frame_enter_params: .byte $0A, ".BYTE $", 0
asm_frame_leave_call: .byte "JSR P_FLEAVE", $0A, ".BYTE $", 0
asm_frame_addr_call: .byte "JSR P_FADDR", $0A, ".BYTE $", 0
asm_frame_addr_offset: .byte $0A, ".WORD $", 0
asm_frame_load8_tail:
      np_lowering_marker NP_LOWER_FRAME_LOAD8_TAIL
asm_frame_load16_tail:
      np_lowering_marker NP_LOWER_FRAME_LOAD16_TAIL
asm_frame_store8_save: .byte "STA NVR1L", $0A, 0
asm_frame_store8_tail:
      np_lowering_marker NP_LOWER_FRAME_STORE8_TAIL
asm_frame_store16_save: .byte "STA NVR1L", $0A, "STX NVR1H", $0A, 0
asm_frame_store16_tail:
      np_lowering_marker NP_LOWER_FRAME_STORE16_TAIL
asm_frame_direct_y: .byte "LDY #$", 0
asm_frame_direct_load8: .byte $0A, "LDA (__NP_FP),Y", $0A, 0
asm_frame_direct_load16:
      .byte $0A
      np_lowering_marker NP_LOWER_FRAME_DIRECT_LOAD16
asm_frame_direct_store8: .byte $0A, "STA (__NP_FP),Y", $0A, 0
asm_frame_direct_store16:
      .byte $0A
      np_lowering_marker NP_LOWER_FRAME_DIRECT_STORE16
asm_frame_store_ref8_save: .byte "STA NVR2L", $0A, 0
asm_frame_store_ref8_tail:
      np_lowering_marker NP_LOWER_FRAME_STORE_REF8_TAIL
asm_frame_store_ref16_save: .byte "STA NVR2L", $0A, "STX NVR2H", $0A, 0
asm_frame_store_ref16_tail:
      np_lowering_marker NP_LOWER_FRAME_STORE_REF16_TAIL
      .segment "NPCFE_RODATA"
asm_open_copy_call: .byte "JSR P_OPEN_COPY", $0A, ".WORD $", 0

asm_deref8:
      np_lowering_marker NP_LOWER_DEREF8
asm_deref16:
      np_lowering_marker NP_LOWER_DEREF16
asm_deref_nvr0_8:
      np_lowering_marker NP_LOWER_DEREF_NVR0_8
asm_deref_nvr0_16:
      np_lowering_marker NP_LOWER_DEREF_NVR0_16
asm_address_result:
      np_lowering_marker NP_LOWER_ADDRESS_RESULT
asm_heap_alloc_call:
      np_lowering_marker NP_LOWER_HEAP_ALLOC_CALL
asm_heap_free_call:
      np_lowering_marker NP_LOWER_HEAP_FREE_CALL
asm_heap_include:
      np_lowering_marker NP_LOWER_HEAP_INCLUDE
asm_heap_alloc_error: .byte "BCC :+", $0A, "PLA", $0A, "PLX", $0A, "LDA #$", 0
asm_heap_free_error: .byte "BCC :+", $0A, "LDA #$", 0
asm_heap_error_line_x: .byte $0A, "LDX #$", 0
asm_heap_error_line_y: .byte $0A, "LDY #$", 0
asm_heap_error_end: .byte $0A, "JSR P_RUNTIME_ERROR", $0A, ":", $0A, 0
asm_heap_store:
      np_lowering_marker NP_LOWER_HEAP_STORE
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
      .segment "NPCFE_RODATA"
asm_increment_word:
      np_lowering_marker NP_LOWER_INCREMENT_WORD
asm_odd: .byte "AND #$01", $0A, 0
      .segment "RODATA"
asm_index_stack_call: .byte "JSR I_ARRAY_INDEX_ADDR_STACK", $0A, ".WORD $", 0
asm_string_index_stack_call:
      np_lowering_marker NP_LOWER_STRING_INDEX_STACK_CALL
asm_index_byte_stack_call:
      np_lowering_marker NP_LOWER_INDEX_BYTE_STACK_CALL
asm_index_word_separator: .byte ",$", 0

      .segment "NPCFE_RODATA"
asm_field_addr_low: .byte "CLC", $0A, "ADC #$", 0
asm_field_addr_high: .byte $0A, "PHA", $0A, "TXA", $0A, "ADC #$", 0
asm_field_addr_end: .byte $0A, "TAX", $0A, "PLA", $0A, 0
asm_field_addr_byte_end: .byte $0A, "BCC :+", $0A, "INX", $0A, ":", $0A, 0
asm_with_push:
      np_lowering_marker NP_LOWER_WITH_PUSH
asm_with_pop:
      np_lowering_marker NP_LOWER_WITH_POP
asm_with_addr_head:
      np_lowering_marker NP_LOWER_WITH_ADDR_HEAD
asm_with_addr_low: .byte ",Y", $0A, "CLC", $0A, "ADC #$", 0
asm_with_addr_mid: .byte $0A, "STA NVR0L", $0A, "LDA __NP_WITH_STACK-$", 0
asm_with_addr_high: .byte ",Y", $0A, "ADC #$", 0
asm_with_addr_end: .byte $0A, "STA NVR0H", $0A, 0
asm_with_addr_zero_mid: .byte ",Y", $0A, "STA NVR0L", $0A, "LDA __NP_WITH_STACK-$", 0
asm_with_addr_zero_end: .byte ",Y", $0A, "STA NVR0H", $0A, 0
asm_string_copy_save:
      np_lowering_marker NP_LOWER_STRING_COPY_SAVE
asm_load_saved_nvr2:
      np_lowering_marker NP_LOWER_LOAD_SAVED_NVR2
asm_block_copy_call:
      np_lowering_marker NP_LOWER_BLOCK_COPY_CALL
asm_string_copy_call:
      np_lowering_marker NP_LOWER_STRING_COPY_CALL
asm_address_to_nvr0:
      np_lowering_marker NP_LOWER_ADDRESS_TO_NVR0
asm_frame_pointer_to_nvr0:
      np_lowering_marker NP_LOWER_FRAME_POINTER_TO_NVR0
asm_stack_address_to_nvr0:
      np_lowering_marker NP_LOWER_STACK_ADDRESS_TO_NVR0
asm_stack_address_store_byte:
      np_lowering_marker NP_LOWER_STACK_STORE_BYTE
asm_stack_address_store_word:
      np_lowering_marker NP_LOWER_STACK_STORE_WORD
asm_stack_address_string_save:
      np_lowering_marker NP_LOWER_STACK_STRING_SAVE
asm_real_save_byte:
      np_lowering_marker NP_LOWER_REAL_SAVE_BYTE
asm_real_save_word:
      np_lowering_marker NP_LOWER_REAL_SAVE_WORD
asm_real_store_integer:
      np_lowering_marker NP_LOWER_REAL_STORE_INTEGER
asm_scalar4_copy:
      np_lowering_marker NP_LOWER_SCALAR4_COPY
; Compact typed IR consumed by NPO2 before NAS sees the generated source.
; Keeping these records shorter than their baseline lowering also recovers
; scarce resident ROM while the optimizer itself runs from lower RAM.
asm_array_get: .byte ".O2G ", 0
asm_jsr: .byte "JSR P_WRITE_CHAR", $0A, 0
asm_jsr_write_byte:
      np_lowering_marker NP_LOWER_WRITE_BYTE_CALL
asm_jsr_write_byte_ln:
      np_lowering_marker NP_LOWER_WRITE_BYTE_LN_CALL
asm_jsr_write_word:
      np_lowering_marker NP_LOWER_WRITE_WORD_CALL
asm_jsr_write_uword:
      np_lowering_marker NP_LOWER_WRITE_UWORD_CALL
asm_jsr_write_format:
      np_lowering_marker NP_LOWER_WRITE_FORMAT_CALL
asm_jsr_write_string:
      np_lowering_marker NP_LOWER_WRITE_STRING_CALL
asm_jsr_write_real:
      np_lowering_marker NP_LOWER_WRITE_REAL_CALL
asm_jsr_write_line: .byte "JSR I_P_WRITE_LINE", $0A, ".BYTE ", 0
asm_line_end: .byte "$00", $0A, 0
asm_rts:    .byte "RTS", $0A, 0
asm_routine_start: .byte ".O2R ", 0
asm_nested_routine_prefix: .byte "__NP_R", 0
asm_global_start: .byte ".GLOBAL ", 0
asm_unit_export_end: .byte " ;@NPUNIT", $0A, 0
asm_unit_init_global_prefix: .byte ".GLOBAL __NP_INIT_", 0
asm_unit_init_routine_prefix: .byte ".O2R P __NP_INIT_", 0
asm_unit_init_call: .byte ".O2J __NP_INIT_", 0
asm_routine_return: .byte ".O2E P", $0A, 0
asm_function_entry: .byte ".O2K", $0A, 0
asm_function_entry_wide: .byte ".O2KW", $0A, 0
asm_function_store: .byte ".O2V", $0A, 0
asm_function_store_wide: .byte ".O2VW", $0A, 0
asm_function_return: .byte ".O2E F", $0A, 0
asm_function_return_wide: .byte ".O2E W", $0A, 0
asm_pha:    .byte "PHA", $0A, 0
asm_phx_pha: .byte "PHX", $0A, "PHA", $0A, 0
asm_indirect_call:
      np_lowering_marker NP_LOWER_INDIRECT_CALL
asm_push_scalar4_value:
      np_lowering_marker NP_LOWER_PUSH_SCALAR4
asm_default_kbd_push:
      np_lowering_marker NP_LOWER_DEFAULT_KBD_PUSH
asm_pla:    .byte "PLA", $0A, 0
asm_ply:    .byte "PLY", $0A, 0
asm_drop_word: .byte "PLA", $0A, "PLX", $0A, 0
asm_drop_four_preserve_ax:
      np_lowering_marker NP_LOWER_DROP_FOUR
asm_add:    .byte ".O2+", $0A, 0
asm_subtract: .byte ".O2-", $0A, 0
asm_or:
      np_lowering_marker NP_LOWER_OR
asm_and:
      np_lowering_marker NP_LOWER_AND
asm_xor:
      np_lowering_marker NP_LOWER_XOR
asm_or_word:
      np_lowering_marker NP_LOWER_OR_WORD
asm_and_word:
      np_lowering_marker NP_LOWER_AND_WORD
asm_xor_word:
      np_lowering_marker NP_LOWER_XOR_WORD
asm_shl:
      np_lowering_marker NP_LOWER_SHL
asm_shr:
      np_lowering_marker NP_LOWER_SHR
asm_shl_word:
      np_lowering_marker NP_LOWER_SHL_WORD
asm_shr_word:
      np_lowering_marker NP_LOWER_SHR_WORD
asm_set_union:
      np_lowering_marker NP_LOWER_OR
asm_set_difference:
      np_lowering_marker NP_LOWER_SET_DIFFERENCE
asm_set_intersection:
      np_lowering_marker NP_LOWER_AND
asm_string_concat:
      np_lowering_marker NP_LOWER_STRING_CONCAT
asm_string_concat_char:
      np_lowering_marker NP_LOWER_STRING_CONCAT_CHAR
asm_char_concat_string:
      np_lowering_marker NP_LOWER_CHAR_CONCAT_STRING
asm_string_compare:
      np_lowering_marker NP_LOWER_STRING_COMPARE
asm_char_to_string:
      np_lowering_marker NP_LOWER_CHAR_TO_STRING
asm_insert_capacity: .byte "STA NVR7L", $0A, 0
asm_char_string_compare:
      np_lowering_marker NP_LOWER_CHAR_STRING_COMPARE
asm_string_pos:
      np_lowering_marker NP_LOWER_STRING_POS
asm_string_copy_part:
      np_lowering_marker NP_LOWER_STRING_COPY_PART_CALL
asm_str_integer:
      np_lowering_marker NP_LOWER_STR_INTEGER_CALL
asm_str_uword:
      np_lowering_marker NP_LOWER_STR_UWORD_CALL
asm_str_real:
      np_lowering_marker NP_LOWER_STR_REAL_CALL
asm_str_real_precision:
      np_lowering_marker NP_LOWER_STR_REAL_PRECISION_CALL
asm_file_write_block:
      np_lowering_marker NP_LOWER_FILE_WRITE_BLOCK
asm_file_write_char:
      np_lowering_marker NP_LOWER_FILE_WRITE_CHAR
asm_file_write_byte:
      np_lowering_marker NP_LOWER_FILE_WRITE_BYTE
asm_file_write_word:
      np_lowering_marker NP_LOWER_FILE_WRITE_WORD
asm_file_write_string:
      np_lowering_marker NP_LOWER_FILE_WRITE_STRING
asm_file_write_real:
      np_lowering_marker NP_LOWER_FILE_WRITE_REAL
asm_file_write_newline:
      np_lowering_marker NP_LOWER_FILE_WRITE_NEWLINE
asm_set_in8:
      np_lowering_marker NP_LOWER_SET_IN8_CALL
asm_set_in256:
      np_lowering_marker NP_LOWER_SET_IN256_CALL
asm_check_overflow:
      np_lowering_marker NP_LOWER_CHECK_OVERFLOW
asm_check_uadd:
      np_lowering_marker NP_LOWER_CHECK_UADD
asm_check_usub:
      np_lowering_marker NP_LOWER_CHECK_USUB
asm_check_muls:
      np_lowering_marker NP_LOWER_CHECK_MULS
asm_check_mulu:
      np_lowering_marker NP_LOWER_CHECK_MULU
asm_check_negs:
      np_lowering_marker NP_LOWER_CHECK_NEGS
asm_check_negu:
      np_lowering_marker NP_LOWER_CHECK_NEGU
asm_check_divs:
      np_lowering_marker NP_LOWER_CHECK_DIVS
asm_check_zero8:
      np_lowering_marker NP_LOWER_CHECK_ZERO8
asm_check_zero16:
      np_lowering_marker NP_LOWER_CHECK_ZERO16
asm_io_check_read:
      np_lowering_marker NP_LOWER_IO_CHECK_READ
asm_io_check_write:
      np_lowering_marker NP_LOWER_IO_CHECK_WRITE
asm_check_range_u:
      np_lowering_marker NP_LOWER_CHECK_RANGE_U
asm_check_range_s:
      np_lowering_marker NP_LOWER_CHECK_RANGE_S
asm_check_range_open:
      np_lowering_marker NP_LOWER_CHECK_RANGE_OPEN
asm_stack_check:
      np_lowering_marker NP_LOWER_STACK_CHECK
asm_check_separator: .byte ",$", 0
asm_rhs_word:
      np_lowering_marker NP_LOWER_RHS_WORD
asm_rhs_byte:
      np_lowering_marker NP_LOWER_RHS_BYTE
asm_lhs_word:
      np_lowering_marker NP_LOWER_LHS_WORD
asm_lhs_byte:
      np_lowering_marker NP_LOWER_LHS_BYTE
asm_add_word: .byte ".O2A", $0A, 0
asm_subtract_word: .byte ".O2S", $0A, 0
asm_negate_word:
      np_lowering_marker NP_LOWER_NEGATE_WORD
asm_negate_real:
      np_lowering_marker NP_LOWER_REAL_NEGATE_CALL
asm_real_from_byte:
      np_lowering_marker NP_LOWER_REAL_FROM_BYTE
asm_real_from_word:
      np_lowering_marker NP_LOWER_REAL_FROM_WORD
asm_long_from_byte:
      np_lowering_marker NP_LOWER_LONG_FROM_BYTE
asm_long_from_uword:
      np_lowering_marker NP_LOWER_LONG_FROM_UWORD
asm_long_from_integer:
      np_lowering_marker NP_LOWER_LONG_FROM_INTEGER
asm_negate_longint:
      np_lowering_marker NP_LOWER_LONG_NEGATE_CALL
asm_longint_add:
      np_lowering_marker NP_LOWER_LONG_ADD
asm_longint_subtract:
      np_lowering_marker NP_LOWER_LONG_SUBTRACT
asm_longint_compare:
      np_lowering_marker NP_LOWER_LONG_COMPARE
asm_scalar4_prologue:
      np_lowering_marker NP_LOWER_SCALAR4_PROLOGUE
asm_longint_multiply:
      np_lowering_marker NP_LOWER_LONG_MULTIPLY
asm_longint_divide:
      np_lowering_marker NP_LOWER_LONG_DIVIDE
asm_longint_modulo:
      np_lowering_marker NP_LOWER_LONG_MODULO
asm_write_long:
      np_lowering_marker NP_LOWER_WRITE_LONG_CALL
asm_str_long:
      np_lowering_marker NP_LOWER_STR_LONG_CALL
asm_real_add:
      np_lowering_marker NP_LOWER_REAL_ADD
asm_real_subtract:
      np_lowering_marker NP_LOWER_REAL_SUBTRACT
asm_real_compare:
      np_lowering_marker NP_LOWER_REAL_COMPARE
asm_multiply_word: .byte "JSR P_MULW", $0A, 0
asm_divide_word: .byte "JSR P_DIVW", $0A, 0
asm_modulo_word: .byte "JSR P_MODW", $0A, 0
asm_divide_uword: .byte "JSR P_DIVUW", $0A, 0
asm_modulo_uword: .byte "JSR P_MODUW", $0A, 0
asm_real_multiply:
      np_lowering_marker NP_LOWER_REAL_MULTIPLY
asm_word_multiply_real:
      np_lowering_marker NP_LOWER_WORD_MULTIPLY_REAL
asm_byte_multiply_real:
      np_lowering_marker NP_LOWER_BYTE_MULTIPLY_REAL
asm_real_multiply_word_right:
      np_lowering_marker NP_LOWER_REAL_MULTIPLY_WORD_RIGHT
asm_real_multiply_byte_right:
      np_lowering_marker NP_LOWER_REAL_MULTIPLY_BYTE_RIGHT
asm_real_divide:
      np_lowering_marker NP_LOWER_REAL_DIVIDE
asm_real_divide_word:
      np_lowering_marker NP_LOWER_REAL_DIVIDE_WORD
asm_real_divide_byte:
      np_lowering_marker NP_LOWER_REAL_DIVIDE_BYTE
asm_word_divide_real:
      np_lowering_marker NP_LOWER_WORD_DIVIDE_REAL
asm_byte_divide_real:
      np_lowering_marker NP_LOWER_BYTE_DIVIDE_REAL
asm_ordinal_divide_real:
      np_lowering_marker NP_LOWER_ORDINAL_DIVIDE_REAL
asm_divide:
      np_lowering_marker NP_LOWER_DIVIDE_BYTE
asm_mod:
      np_lowering_marker NP_LOWER_MOD_BYTE
asm_not:    .byte "EOR #$01", $0A, 0
asm_compare_start: .byte ".O2Q", $0A, 0
asm_compare_word_start: .byte ".O2X", $0A, 0
asm_compare_sword_start:
      np_lowering_marker NP_LOWER_COMPARE_SIGNED_WORD
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
asm_case_range_mid: .byte $0A, "BCC :+", $0A, "CMP #$", 0
asm_case_range_branch: .byte $0A, "BCC __NP_L", 0
asm_case_wrange_low: .byte $0A, "BCC :++", $0A, "BNE :+", $0A, "CMP #$", 0
asm_case_wrange_high: .byte $0A, "BCC :++", $0A, ":", $0A, "CPX #$", 0
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
      .segment "RODATA"
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
      .segment "NPCFE_RODATA"
asm_external_const_start: .byte "__NP_LOAD_EXTERNAL ", 0
asm_external_const_else: .byte ", __C", 0
sig_suffix_lo: .byte <asm_sig0_end, <asm_sig1_end, <asm_sig2_end, <asm_sig3_end
sig_suffix_hi: .byte >asm_sig0_end, >asm_sig1_end, >asm_sig2_end, >asm_sig3_end

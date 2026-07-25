; Complete NPC lowering recipes live with NPO2, which runs after every compile.
; NPC emits one compact, printable .O2T record instead of carrying these
; textual 65C02 sequences in its constrained frontend overlay.

.ifndef NPC_LOWERING_INC
NPC_LOWERING_INC = 1

; Typed IR names originate in NPC identifiers. NPO2 uses the same bound for
; array-cache scratch while retaining its wider buffers for generated labels.
NP_TYPED_SYMBOL_CAP = 32

; One ordered registry of lowering recipes. `np_lowering_entries` is the single
; place an entry is added or reordered: it drives the NP_LOWER_* ids,
; NP_LOWER_COUNT, and NPO2's low/high dispatch tables, so those three can no
; longer silently drift out of step.
.macro np_lowering_entry_id name, sym
      name = np_lowering_next_id
      np_lowering_next_id .set np_lowering_next_id + 1
.endmacro

.macro np_lowering_entries emit
      emit NP_LOWER_HEAP_STORE, np_lower_heap_store
      emit NP_LOWER_DEREF16, np_lower_deref16
      emit NP_LOWER_DEREF_NVR0_16, np_lower_deref_nvr0_16
      emit NP_LOWER_FRAME_POINTER_TO_NVR0, np_lower_frame_pointer_to_nvr0
      emit NP_LOWER_STACK_ADDRESS_TO_NVR0, np_lower_stack_address_to_nvr0
      emit NP_LOWER_STACK_STORE_BYTE, np_lower_stack_store_byte
      emit NP_LOWER_STACK_STORE_WORD, np_lower_stack_store_word
      emit NP_LOWER_STACK_STRING_SAVE, np_lower_stack_string_save
      emit NP_LOWER_REAL_STORE_INTEGER, np_lower_real_store_integer
      emit NP_LOWER_SCALAR4_COPY, np_lower_scalar4_copy
      emit NP_LOWER_PUSH_SCALAR4, np_lower_push_scalar4
      emit NP_LOWER_DEFAULT_KBD_PUSH, np_lower_default_kbd_push
      emit NP_LOWER_STRING_CONCAT, np_lower_string_concat
      emit NP_LOWER_STRING_CONCAT_CHAR, np_lower_string_concat_char
      emit NP_LOWER_CHAR_CONCAT_STRING, np_lower_char_concat_string
      emit NP_LOWER_STRING_COMPARE, np_lower_string_compare
      emit NP_LOWER_CHAR_STRING_COMPARE, np_lower_char_string_compare
      emit NP_LOWER_STRING_POS, np_lower_string_pos
      emit NP_LOWER_FILE_WRITE_CHAR, np_lower_file_write_char
      emit NP_LOWER_FILE_WRITE_BYTE, np_lower_file_write_byte
      emit NP_LOWER_FILE_WRITE_WORD, np_lower_file_write_word
      emit NP_LOWER_FILE_WRITE_STRING, np_lower_file_write_string
      emit NP_LOWER_FILE_WRITE_REAL, np_lower_file_write_real
      emit NP_LOWER_FILE_WRITE_NEWLINE, np_lower_file_write_newline
      emit NP_LOWER_NEGATE_WORD, np_lower_negate_word
      emit NP_LOWER_REAL_ADD, np_lower_real_add
      emit NP_LOWER_REAL_SUBTRACT, np_lower_real_subtract
      emit NP_LOWER_REAL_COMPARE, np_lower_real_compare
      emit NP_LOWER_REAL_MULTIPLY, np_lower_real_multiply
      emit NP_LOWER_WORD_MULTIPLY_REAL, np_lower_word_multiply_real
      emit NP_LOWER_BYTE_MULTIPLY_REAL, np_lower_byte_multiply_real
      emit NP_LOWER_REAL_MULTIPLY_WORD_RIGHT, np_lower_real_multiply_word_right
      emit NP_LOWER_REAL_MULTIPLY_BYTE_RIGHT, np_lower_real_multiply_byte_right
      emit NP_LOWER_REAL_DIVIDE, np_lower_real_divide
      emit NP_LOWER_REAL_DIVIDE_WORD, np_lower_real_divide_word
      emit NP_LOWER_REAL_DIVIDE_BYTE, np_lower_real_divide_byte
      emit NP_LOWER_WORD_DIVIDE_REAL, np_lower_word_divide_real
      emit NP_LOWER_BYTE_DIVIDE_REAL, np_lower_byte_divide_real
      emit NP_LOWER_DIVIDE_BYTE, np_lower_divide_byte
      emit NP_LOWER_MOD_BYTE, np_lower_mod_byte
      emit NP_LOWER_LONG_ADD, np_lower_long_add
      emit NP_LOWER_LONG_SUBTRACT, np_lower_long_subtract
      emit NP_LOWER_LONG_COMPARE, np_lower_long_compare
      emit NP_LOWER_MAIN_LABEL_FRAMES, np_lower_main_label_frames
      emit NP_LOWER_FRAME_LOAD16_TAIL, np_lower_frame_load16_tail
      emit NP_LOWER_FRAME_STORE16_TAIL, np_lower_frame_store16_tail
      emit NP_LOWER_FRAME_STORE_REF8_TAIL, np_lower_frame_store_ref8_tail
      emit NP_LOWER_FRAME_STORE_REF16_TAIL, np_lower_frame_store_ref16_tail
      emit NP_LOWER_FILE_WRITE_BLOCK, np_lower_file_write_block
      emit NP_LOWER_CODE_WITH_ROUTINES, np_lower_code_with_routines
      emit NP_LOWER_WITH_ADDR_HEAD, np_lower_with_addr_head
      emit NP_LOWER_STRING_COPY_CALL, np_lower_string_copy_call
      emit NP_LOWER_BLOCK_COPY_CALL, np_lower_block_copy_call
      emit NP_LOWER_CHECK_OVERFLOW, np_lower_check_overflow
      emit NP_LOWER_CHECK_RANGE_U, np_lower_check_range_u
      emit NP_LOWER_CHECK_RANGE_S, np_lower_check_range_s
      emit NP_LOWER_STACK_CHECK, np_lower_stack_check
      emit NP_LOWER_IO_CHECK_READ, np_lower_io_check_read
      emit NP_LOWER_IO_CHECK_WRITE, np_lower_io_check_write
      emit NP_LOWER_STRING_INDEX_STACK_CALL, np_lower_string_index_stack_call
      emit NP_LOWER_CHECK_UADD, np_lower_check_uadd
      emit NP_LOWER_CHECK_USUB, np_lower_check_usub
      emit NP_LOWER_CHECK_MULS, np_lower_check_muls
      emit NP_LOWER_CHECK_MULU, np_lower_check_mulu
      emit NP_LOWER_CHECK_NEGS, np_lower_check_negs
      emit NP_LOWER_CHECK_NEGU, np_lower_check_negu
      emit NP_LOWER_CHECK_DIVS, np_lower_check_divs
      emit NP_LOWER_CHECK_ZERO8, np_lower_check_zero8
      emit NP_LOWER_CHECK_ZERO16, np_lower_check_zero16
      emit NP_LOWER_CHECK_RANGE_OPEN, np_lower_check_range_open
      emit NP_LOWER_SET_DIFFERENCE, np_lower_set_difference
      emit NP_LOWER_SAVE_INCOMING, np_lower_save_incoming
      emit NP_LOWER_OPEN_BASE_SAVE, np_lower_open_base_save
      emit NP_LOWER_OPEN_BASE_RESTORE, np_lower_open_base_restore
      emit NP_LOWER_FRAME_LOAD8_TAIL, np_lower_frame_load8_tail
      emit NP_LOWER_FRAME_STORE8_TAIL, np_lower_frame_store8_tail
      emit NP_LOWER_FRAME_DIRECT_LOAD16, np_lower_frame_direct_load16
      emit NP_LOWER_FRAME_DIRECT_STORE16, np_lower_frame_direct_store16
      emit NP_LOWER_DEREF8, np_lower_deref8
      emit NP_LOWER_DEREF_NVR0_8, np_lower_deref_nvr0_8
      emit NP_LOWER_ADDRESS_RESULT, np_lower_address_result
      emit NP_LOWER_INCREMENT_WORD, np_lower_increment_word
      emit NP_LOWER_STRING_COPY_SAVE, np_lower_string_copy_save
      emit NP_LOWER_LOAD_SAVED_NVR2, np_lower_load_saved_nvr2
      emit NP_LOWER_ADDRESS_TO_NVR0, np_lower_address_to_nvr0
      emit NP_LOWER_REAL_SAVE_BYTE, np_lower_real_save_byte
      emit NP_LOWER_REAL_SAVE_WORD, np_lower_real_save_word
      emit NP_LOWER_DROP_FOUR, np_lower_drop_four
      emit NP_LOWER_OR, np_lower_or
      emit NP_LOWER_AND, np_lower_and
      emit NP_LOWER_RHS_WORD, np_lower_rhs_word
      emit NP_LOWER_RHS_BYTE, np_lower_rhs_byte
      emit NP_LOWER_LHS_WORD, np_lower_lhs_word
      emit NP_LOWER_LHS_BYTE, np_lower_lhs_byte
      emit NP_LOWER_SEGMENT_BSS, np_lower_segment_bss
      emit NP_LOWER_SEGMENT_RODATA, np_lower_segment_rodata
      emit NP_LOWER_SEGMENT_CODE, np_lower_segment_code
      emit NP_LOWER_HEAP_ALLOC_CALL, np_lower_heap_alloc_call
      emit NP_LOWER_HEAP_FREE_CALL, np_lower_heap_free_call
      emit NP_LOWER_HEAP_INCLUDE, np_lower_heap_include
      emit NP_LOWER_WITH_PUSH, np_lower_with_push
      emit NP_LOWER_WITH_POP, np_lower_with_pop
      emit NP_LOWER_WRITE_CHAR_CALL, np_lower_write_char_call
      emit NP_LOWER_WRITE_BYTE_CALL, np_lower_write_byte_call
      emit NP_LOWER_WRITE_BYTE_LN_CALL, np_lower_write_byte_ln_call
      emit NP_LOWER_WRITE_WORD_CALL, np_lower_write_word_call
      emit NP_LOWER_WRITE_UWORD_CALL, np_lower_write_uword_call
      emit NP_LOWER_WRITE_FORMAT_CALL, np_lower_write_format_call
      emit NP_LOWER_WRITE_STRING_CALL, np_lower_write_string_call
      emit NP_LOWER_WRITE_REAL_CALL, np_lower_write_real_call
      emit NP_LOWER_INDIRECT_CALL, np_lower_indirect_call
      emit NP_LOWER_CHAR_TO_STRING, np_lower_char_to_string
      emit NP_LOWER_STRING_COPY_PART_CALL, np_lower_string_copy_part_call
      emit NP_LOWER_STR_INTEGER_CALL, np_lower_str_integer_call
      emit NP_LOWER_STR_UWORD_CALL, np_lower_str_uword_call
      emit NP_LOWER_STR_REAL_CALL, np_lower_str_real_call
      emit NP_LOWER_STR_REAL_PRECISION_CALL, np_lower_str_real_precision_call
      emit NP_LOWER_INDEX_BYTE_STACK_CALL, np_lower_index_byte_stack_call
      emit NP_LOWER_SET_IN8_CALL, np_lower_set_in8_call
      emit NP_LOWER_SET_IN256_CALL, np_lower_set_in256_call
      emit NP_LOWER_COMPARE_SIGNED_WORD, np_lower_compare_signed_word
      emit NP_LOWER_LONG_MULTIPLY, np_lower_long_multiply
      emit NP_LOWER_LONG_DIVIDE, np_lower_long_divide
      emit NP_LOWER_LONG_MODULO, np_lower_long_modulo
      emit NP_LOWER_WRITE_LONG_CALL, np_lower_write_long_call
      emit NP_LOWER_STR_LONG_CALL, np_lower_str_long_call
      emit NP_LOWER_SCALAR4_PROLOGUE, np_lower_scalar4_prologue
      emit NP_LOWER_REAL_NEGATE_CALL, np_lower_real_negate_call
      emit NP_LOWER_REAL_FROM_BYTE, np_lower_real_from_byte
      emit NP_LOWER_REAL_FROM_WORD, np_lower_real_from_word
      emit NP_LOWER_LONG_FROM_BYTE, np_lower_long_from_byte
      emit NP_LOWER_LONG_FROM_UWORD, np_lower_long_from_uword
      emit NP_LOWER_LONG_FROM_INTEGER, np_lower_long_from_integer
      emit NP_LOWER_LONG_NEGATE_CALL, np_lower_long_negate_call
      emit NP_LOWER_ORDINAL_DIVIDE_REAL, np_lower_ordinal_divide_real
      emit NP_LOWER_XOR, np_lower_xor
      emit NP_LOWER_OR_WORD, np_lower_or_word
      emit NP_LOWER_AND_WORD, np_lower_and_word
      emit NP_LOWER_XOR_WORD, np_lower_xor_word
      emit NP_LOWER_SHL, np_lower_shl
      emit NP_LOWER_SHR, np_lower_shr
      emit NP_LOWER_SHL_WORD, np_lower_shl_word
      emit NP_LOWER_SHR_WORD, np_lower_shr_word
.endmacro

np_lowering_next_id .set 0
      np_lowering_entries np_lowering_entry_id
NP_LOWER_COUNT = np_lowering_next_id

.macro np_lowering_marker id
      .if id < 52
        .if id < 26
      .byte ".O2T", 'A' + id, $0A, 0
        .else
      .byte ".O2T", 'a' + id - 26, $0A, 0
        .endif
      .else
        .if id < 78
      .byte ".O2U", 'A' + id - 52, $0A, 0
        .else
          .if id < 104
      .byte ".O2U", 'a' + id - 78, $0A, 0
          .else
            .if id < 130
      .byte ".O2H", 'A' + id - 104, $0A, 0
            .else
      .byte ".O2H", 'a' + id - 130, $0A, 0
            .endif
          .endif
        .endif
      .endif
.endmacro

.endif

.ifdef NPO2_LOWERING_IMPL
.ifndef NPO2_LOWERING_DEFINED
NPO2_LOWERING_DEFINED = 1
; Every Real and LongInt binary operator marshals its operands the same way, so
; NPC emits NP_LOWER_SCALAR4_PROLOGUE ahead of the call recipe and each operator
; carries only its own JSR. Twelve copies of the prologue previously sat in
; NPO2's recipe table.
.macro np_lower_scalar4_call routine
      .byte "JSR ", routine, $0A, 0
.endmacro

.macro np_lower_word_real_left routine, byte_operand
      .byte "STA NVR1L", $0A, "STX NVR1H", $0A
      .byte "PLA", $0A, "STA NVR0L", $0A
      .if byte_operand
      .byte "STZ NVR0H", $0A
      .else
      .byte "PLX", $0A, "STX NVR0H", $0A
      .endif
      .byte "LDA NVR1L", $0A, "LDX NVR1H", $0A, "JSR ", routine, $0A, 0
.endmacro

.macro np_lower_real_byte_right routine, byte_operand
      .byte "STA NVR2L", $0A
      .if byte_operand
      .else
      .byte "STX NVR2H", $0A
      .endif
      .byte "PLA", $0A, "STA NVR1H", $0A, "PLA", $0A, "STA NVR1L", $0A
      .byte "PLA", $0A, "STA NVR0H", $0A, "PLA", $0A, "STA NVR0L", $0A
      .byte "LDA NVR2L", $0A
      .if byte_operand
      .else
      .byte "LDX NVR2H", $0A
      .endif
      .byte "JSR ", routine, $0A, 0
.endmacro

; Both NPO2 dispatch tables are generated from the shared registry above.
.macro np_lowering_entry_lo name, sym
      .byte <sym
.endmacro

.macro np_lowering_entry_hi name, sym
      .byte >sym
.endmacro

np_lowering_lo:
      np_lowering_entries np_lowering_entry_lo
np_lowering_hi:
      np_lowering_entries np_lowering_entry_hi

np_lower_heap_store:
      .byte "STA NVR7L", $0A, "STX NVR7H", $0A
      .byte "PLA", $0A, "STA NVR0L", $0A, "PLX", $0A, "STX NVR0H", $0A
      .byte "LDY #$00", $0A, "LDA NVR7L", $0A, "STA (NVR0L),Y", $0A
      .byte "INY", $0A, "LDA NVR7H", $0A, "STA (NVR0L),Y", $0A, 0
np_lower_deref16:
      .byte "STA NVR0L", $0A, "STX NVR0H", $0A, "LDY #$00", $0A
      .byte "LDA (NVR0L),Y", $0A, "STA NVR1L", $0A, "INY", $0A
      .byte "LDA (NVR0L),Y", $0A, "TAX", $0A, "LDA NVR1L", $0A, 0
np_lower_deref_nvr0_16:
      .byte "LDY #$00", $0A, "LDA (NVR0L),Y", $0A, "STA NVR1L", $0A
      .byte "INY", $0A, "LDA (NVR0L),Y", $0A, "TAX", $0A, "LDA NVR1L", $0A, 0
np_lower_frame_pointer_to_nvr0:
      .byte "LDY #$00", $0A, "LDA (NVR0L),Y", $0A, "STA NVR3L", $0A
      .byte "INY", $0A, "LDA (NVR0L),Y", $0A, "STA NVR0H", $0A
      .byte "LDA NVR3L", $0A, "STA NVR0L", $0A, 0
np_lower_stack_address_to_nvr0:
      .byte "PLA", $0A, "STA NVR0L", $0A, "PLX", $0A, "STX NVR0H", $0A, 0
np_lower_stack_store_byte:
      .byte "STA NVR1L", $0A, "PLA", $0A, "STA NVR0L", $0A
      .byte "PLX", $0A, "STX NVR0H", $0A, "LDY #$00", $0A
      .byte "LDA NVR1L", $0A, "STA (NVR0L),Y", $0A, 0
np_lower_stack_store_word:
      .byte "STA NVR1L", $0A, "STX NVR1H", $0A, "PLA", $0A
      .byte "STA NVR0L", $0A, "PLX", $0A, "STX NVR0H", $0A
      .byte "LDY #$00", $0A, "LDA NVR1L", $0A, "STA (NVR0L),Y", $0A
      .byte "INY", $0A, "LDA NVR1H", $0A, "STA (NVR0L),Y", $0A, 0
np_lower_stack_string_save:
      .byte "STA NVR2L", $0A, "STX NVR2H", $0A, "PLA", $0A
      .byte "STA NVR0L", $0A, "PLX", $0A, "STX NVR0H", $0A, 0
np_lower_real_store_integer:
      .byte "LDY #$00", $0A, "LDA #$00", $0A, "STA (NVR0L),Y", $0A
      .byte "INY", $0A, "STA (NVR0L),Y", $0A, "INY", $0A
      .byte "LDA NVR2L", $0A, "STA (NVR0L),Y", $0A, "INY", $0A
      .byte "LDA NVR2H", $0A, "STA (NVR0L),Y", $0A, 0
np_lower_scalar4_copy:
      .byte "LDY #$00", $0A, "LDA (NVR2L),Y", $0A, "STA (NVR0L),Y", $0A
      .byte "INY", $0A, "LDA (NVR2L),Y", $0A, "STA (NVR0L),Y", $0A
      .byte "INY", $0A, "LDA (NVR2L),Y", $0A, "STA (NVR0L),Y", $0A
      .byte "INY", $0A, "LDA (NVR2L),Y", $0A, "STA (NVR0L),Y", $0A, 0
np_lower_push_scalar4:
      .byte "STA NVR0L", $0A, "STX NVR0H", $0A, "LDY #$00", $0A
      .byte "LDA (NVR0L),Y", $0A, "PHA", $0A, "INY", $0A
      .byte "LDA (NVR0L),Y", $0A, "PHA", $0A, "INY", $0A
      .byte "LDA (NVR0L),Y", $0A, "PHA", $0A, "INY", $0A
      .byte "LDA (NVR0L),Y", $0A, "PHA", $0A, 0
np_lower_default_kbd_push:
      .byte "LDA #<KBD", $0A, "LDX #>KBD", $0A, "PHX", $0A, "PHA", $0A, 0
np_lower_string_concat:
      .byte "STA NVR1L", $0A, "STX NVR1H", $0A, "PLA", $0A
      .byte "STA NVR0L", $0A, "PLX", $0A, "STX NVR0H", $0A
      .byte "LDA NVR1L", $0A, "LDX NVR1H", $0A, "JSR P_STRING_CONCAT", $0A, 0
np_lower_string_concat_char:
      .byte "STA NVR1L", $0A, "PLA", $0A, "STA NVR0L", $0A
      .byte "PLX", $0A, "STX NVR0H", $0A, "LDA NVR1L", $0A
      .byte "JSR P_STRING_CONCAT_CHAR", $0A, 0
np_lower_char_concat_string:
      .byte "STA NVR1L", $0A, "STX NVR1H", $0A, "PLA", $0A
      .byte "STA NVR0L", $0A, "LDA NVR1L", $0A, "LDX NVR1H", $0A
      .byte "JSR P_CHAR_CONCAT_STRING", $0A, 0
np_lower_string_compare:
      .byte "STA NVR1L", $0A, "STX NVR1H", $0A, "PLA", $0A
      .byte "STA NVR0L", $0A, "PLX", $0A, "STX NVR0H", $0A
      .byte "LDA NVR1L", $0A, "LDX NVR1H", $0A, "JSR P_STRING_COMPARE", $0A, 0
np_lower_char_string_compare:
      .byte "STA NVR1L", $0A, "STX NVR1H", $0A, "PLA", $0A
      .byte "JSR P_CHAR_TO_STRING", $0A, "STA NVR0L", $0A, "STX NVR0H", $0A
      .byte "LDA NVR1L", $0A, "LDX NVR1H", $0A, "JSR P_STRING_COMPARE", $0A, 0
np_lower_string_pos:
      .byte "STA NVR1L", $0A, "STX NVR1H", $0A, "PLA", $0A
      .byte "STA NVR0L", $0A, "PLX", $0A, "STX NVR0H", $0A
      .byte "LDA NVR1L", $0A, "LDX NVR1H", $0A, "JSR P_STRING_POS", $0A, 0
np_lower_file_write_char:
      .byte "STA NVR1L", $0A, "PLA", $0A, "STA NVR0L", $0A
      .byte "PLX", $0A, "STX NVR0H", $0A, "PHX", $0A, "PHA", $0A
      .byte "LDA NVR1L", $0A, "JSR P_FILE_WRITE_CHAR", $0A, 0
np_lower_file_write_byte:
      .byte "STA NVR1L", $0A, "PLA", $0A, "STA NVR0L", $0A
      .byte "PLX", $0A, "STX NVR0H", $0A, "PHX", $0A, "PHA", $0A
      .byte "LDA NVR1L", $0A, "JSR P_FILE_WRITE_BYTE", $0A, 0
np_lower_file_write_word:
      .byte "STA NVR1L", $0A, "STX NVR1H", $0A, "PLA", $0A
      .byte "STA NVR0L", $0A, "PLX", $0A, "STX NVR0H", $0A
      .byte "PHX", $0A, "PHA", $0A, "LDA NVR1L", $0A, "LDX NVR1H", $0A
      .byte "JSR P_FILE_WRITE_WORD", $0A, 0
np_lower_file_write_string:
      .byte "STA NVR1L", $0A, "STX NVR1H", $0A, "PLA", $0A
      .byte "STA NVR0L", $0A, "PLX", $0A, "STX NVR0H", $0A
      .byte "PHX", $0A, "PHA", $0A, "LDA NVR1L", $0A, "LDX NVR1H", $0A
      .byte "JSR P_FILE_WRITE_STRING", $0A, 0
np_lower_file_write_real:
      .byte "STA NVR1L", $0A, "STX NVR1H", $0A, "PLA", $0A
      .byte "STA NVR0L", $0A, "PLX", $0A, "STX NVR0H", $0A
      .byte "PHX", $0A, "PHA", $0A, "LDA NVR1L", $0A, "LDX NVR1H", $0A
      .byte "JSR P_FILE_WRITE_REAL", $0A, 0
np_lower_file_write_newline:
      .byte "PLA", $0A, "STA NVR0L", $0A, "PLX", $0A, "STX NVR0H", $0A
      .byte "PHX", $0A, "PHA", $0A, "JSR P_FILE_WRITE_NEWLINE", $0A, 0
np_lower_negate_word:
      .byte "EOR #$FF", $0A, "CLC", $0A, "ADC #$01", $0A, "STA NVR0L", $0A
      .byte "TXA", $0A, "EOR #$FF", $0A, "ADC #$00", $0A, "TAX", $0A
      .byte "LDA NVR0L", $0A, 0

np_lower_real_add:
      np_lower_scalar4_call "P_REAL_ADD"
np_lower_real_subtract:
      np_lower_scalar4_call "P_REAL_SUB"
np_lower_real_compare:
      np_lower_scalar4_call "P_REAL_CMP"
np_lower_real_multiply:
      np_lower_scalar4_call "P_REAL_MUL"
np_lower_real_divide:
      np_lower_scalar4_call "P_REAL_DIV"
np_lower_real_divide_word:
      np_lower_scalar4_call "P_REAL_DIV_WORD"

; Single-call scalar4 helpers. These live here rather than as literal text in
; the size-constrained NPC frontend overlay.
np_lower_real_negate_call:
      np_lower_scalar4_call "P_REAL_NEGATE"
np_lower_real_from_byte:
      np_lower_scalar4_call "P_REAL_FROM_BYTE"
np_lower_real_from_word:
      np_lower_scalar4_call "P_REAL_FROM_WORD"
np_lower_long_from_byte:
      np_lower_scalar4_call "P_LONG_FROM_BYTE"
np_lower_long_from_uword:
      np_lower_scalar4_call "P_LONG_FROM_UWORD"
np_lower_long_from_integer:
      np_lower_scalar4_call "P_LONG_FROM_INTEGER"
np_lower_long_negate_call:
      np_lower_scalar4_call "P_LONG_NEGATE"
np_lower_ordinal_divide_real:
      np_lower_scalar4_call "P_ORDINAL_DIV_REAL"

np_lower_scalar4_prologue:
      .byte "STA NVR2L", $0A, "STX NVR2H", $0A
      .byte "PLA", $0A, "STA NVR1H", $0A, "PLA", $0A, "STA NVR1L", $0A
      .byte "PLA", $0A, "STA NVR0H", $0A, "PLA", $0A, "STA NVR0L", $0A
      .byte "LDA NVR2L", $0A, "LDX NVR2H", $0A, 0
np_lower_long_add:
      np_lower_scalar4_call "P_LONG_ADD"
np_lower_long_subtract:
      np_lower_scalar4_call "P_LONG_SUB"
np_lower_long_compare:
      np_lower_scalar4_call "P_LONG_CMP"
np_lower_long_multiply:
      np_lower_scalar4_call "P_LONG_MUL"
np_lower_long_divide:
      np_lower_scalar4_call "P_LONG_DIV"
np_lower_long_modulo:
      np_lower_scalar4_call "P_LONG_MOD"

np_lower_word_multiply_real:
      np_lower_word_real_left "P_REAL_MUL_WORD_LEFT", 0
np_lower_byte_multiply_real:
      np_lower_word_real_left "P_REAL_MUL_BYTE_LEFT", 1
np_lower_real_multiply_word_right:
      np_lower_real_byte_right "P_REAL_MUL_WORD_RIGHT", 0
np_lower_real_multiply_byte_right:
      np_lower_real_byte_right "P_REAL_MUL_BYTE_RIGHT", 1
np_lower_real_divide_byte:
      np_lower_real_byte_right "P_REAL_DIV_BYTE", 1
np_lower_word_divide_real:
      np_lower_word_real_left "P_WORD_DIV_REAL", 0
np_lower_byte_divide_real:
      np_lower_word_real_left "P_BYTE_DIV_REAL", 1

np_lower_divide_byte:
      .byte "STA NVR0L", $0A, "BNE :+", $0A, "PLA", $0A, "STP", $0A
      .byte ":", $0A, "PLA", $0A, "LDX #$00", $0A, ":", $0A
      .byte "CMP NVR0L", $0A, "BCC :+", $0A, "SEC", $0A, "SBC NVR0L", $0A
      .byte "INX", $0A, "BRA :-", $0A, ":", $0A, "TXA", $0A, 0
np_lower_mod_byte:
      .byte "STA NVR0L", $0A, "BNE :+", $0A, "PLA", $0A, "STP", $0A
      .byte ":", $0A, "PLA", $0A, ":", $0A, "CMP NVR0L", $0A
      .byte "BCC :+", $0A, "SEC", $0A, "SBC NVR0L", $0A, "BRA :-", $0A
      .byte ":", $0A, 0

np_lower_main_label_frames:
      .byte "__NP_MAIN:", $0A
      .byte "STZ __NP_FP", $0A, "STZ __NP_FP+1", $0A
      .byte "LDA #<__NP_STACK", $0A, "STA __NP_SP", $0A
      .byte "LDA #>__NP_STACK", $0A, "STA __NP_SP+1", $0A, 0
np_lower_frame_load16_tail:
      .byte "LDY #$01", $0A, "LDA (NVR0L),Y", $0A, "TAX", $0A
      .byte "DEY", $0A, "LDA (NVR0L),Y", $0A, 0
np_lower_frame_store16_tail:
      .byte "LDY #$00", $0A, "LDA NVR1L", $0A, "STA (NVR0L),Y", $0A
      .byte "INY", $0A, "LDA NVR1H", $0A, "STA (NVR0L),Y", $0A, 0
np_lower_frame_store_ref8_tail:
      .byte "STA NVR0L", $0A, "STX NVR0H", $0A, "LDY #$00", $0A
      .byte "LDA NVR2L", $0A, "STA (NVR0L),Y", $0A, 0
np_lower_frame_store_ref16_tail:
      .byte "STA NVR0L", $0A, "STX NVR0H", $0A, "LDY #$00", $0A
      .byte "LDA NVR2L", $0A, "STA (NVR0L),Y", $0A, "INY", $0A
      .byte "LDA NVR2H", $0A, "STA (NVR0L),Y", $0A, 0
np_lower_file_write_block:
      .byte "STA NVR1L", $0A, "STX NVR1H", $0A, "PLA", $0A
      .byte "STA NVR0L", $0A, "PLX", $0A, "STX NVR0H", $0A
      .byte "PHX", $0A, "PHA", $0A, "LDA NVR1L", $0A, "LDX NVR1H", $0A
      .byte "JSR I_P_FILE_WRITE_BLOCK", $0A, ".WORD $", 0
np_lower_code_with_routines:
      .byte ".SEGMENT ", 34, "CODE", 34, $0A, "JMP __NP_MAIN", $0A, 0
np_lower_with_addr_head:
      .byte "LDY __NP_WITH_SP", $0A, "LDA __NP_WITH_STACK-$", 0
np_lower_string_copy_call:
      .byte "LDA NVR2L", $0A, "LDX NVR2H", $0A
      .byte "JSR I_P_STRING_COPY", $0A, ".BYTE $", 0
np_lower_block_copy_call:
      .byte "JSR I_P_BLOCK_COPY", $0A, ".WORD $", 0
np_lower_check_overflow:
      .byte "JSR I_P_CHECK_OVERFLOW", $0A, ".WORD $", 0
np_lower_check_range_u:
      .byte "JSR I_P_CHECK_RANGE_U", $0A, ".WORD $", 0
np_lower_check_range_s:
      .byte "JSR I_P_CHECK_RANGE_S", $0A, ".WORD $", 0
np_lower_stack_check:
      .byte "JSR I_P_STACK_CHECK", $0A, ".WORD $", 0
np_lower_io_check_read:
      .byte "JSR I_P_IOCHECK_READ", $0A, ".WORD $", 0
np_lower_io_check_write:
      .byte "JSR I_P_IOCHECK_WRITE", $0A, ".WORD $", 0
np_lower_string_index_stack_call:
      .byte "JSR I_ARRAY_INDEX_ADDR_STACK", $0A, ".WORD $0000,$0001", $0A, 0
np_lower_check_uadd:
      .byte "JSR I_P_CHECK_UADD", $0A, ".WORD $", 0
np_lower_check_usub:
      .byte "JSR I_P_CHECK_USUB", $0A, ".WORD $", 0
np_lower_check_muls:
      .byte "JSR I_P_CHECK_MULS", $0A, ".WORD $", 0
np_lower_check_mulu:
      .byte "JSR I_P_CHECK_MULU", $0A, ".WORD $", 0
np_lower_check_negs:
      .byte "JSR I_P_CHECK_NEGS", $0A, ".WORD $", 0
np_lower_check_negu:
      .byte "JSR I_P_CHECK_NEGU", $0A, ".WORD $", 0
np_lower_check_divs:
      .byte "JSR I_P_CHECK_DIVS", $0A, ".WORD $", 0
np_lower_check_zero8:
      .byte "JSR I_P_CHECK_ZERO8", $0A, ".WORD $", 0
np_lower_check_zero16:
      .byte "JSR I_P_CHECK_ZERO16", $0A, ".WORD $", 0
np_lower_check_range_open:
      .byte "JSR I_P_CHECK_RANGE_OPEN", $0A, ".WORD $", 0
np_lower_set_difference:
      .byte "EOR #$FF", $0A, "STA NVR0L", $0A, "PLA", $0A
      .byte "AND NVR0L", $0A, 0
np_lower_save_incoming:
      .byte "STA NVR1L", $0A, "STX NVR1H", $0A, 0
np_lower_open_base_save:
      .byte "STA NVR2L", $0A, "STX NVR2H", $0A, 0
np_lower_open_base_restore:
      .byte "LDA NVR2L", $0A, "LDX NVR2H", $0A, 0
np_lower_frame_load8_tail:
      .byte "LDY #$00", $0A, "LDA (NVR0L),Y", $0A, 0
np_lower_frame_store8_tail:
      .byte "LDY #$00", $0A, "LDA NVR1L", $0A, "STA (NVR0L),Y", $0A, 0
np_lower_frame_direct_load16:
      .byte "LDA (__NP_FP),Y", $0A, "TAX", $0A
      .byte "DEY", $0A, "LDA (__NP_FP),Y", $0A, 0
np_lower_frame_direct_store16:
      .byte "STA (__NP_FP),Y", $0A, "INY", $0A
      .byte "TXA", $0A, "STA (__NP_FP),Y", $0A, 0
np_lower_deref8:
      .byte "STA NVR0L", $0A, "STX NVR0H", $0A, "LDY #$00", $0A
      .byte "LDA (NVR0L),Y", $0A, 0
np_lower_deref_nvr0_8:
      .byte "LDY #$00", $0A, "LDA (NVR0L),Y", $0A, 0
np_lower_address_result:
      .byte "LDA NVR0L", $0A, "LDX NVR0H", $0A, 0
np_lower_increment_word:
      .byte "INC A", $0A, "BNE :+", $0A, "INX", $0A, ":", $0A, 0
np_lower_string_copy_save:
      .byte "STA NVR2L", $0A, "STX NVR2H", $0A, 0
np_lower_load_saved_nvr2:
      .byte "LDA NVR2L", $0A, "LDX NVR2H", $0A, 0
np_lower_address_to_nvr0:
      .byte "STA NVR0L", $0A, "STX NVR0H", $0A, 0
np_lower_real_save_byte:
      .byte "STA NVR2L", $0A, "STZ NVR2H", $0A, 0
np_lower_real_save_word:
      .byte "STA NVR2L", $0A, "STX NVR2H", $0A, 0
np_lower_drop_four:
      .byte "PLY", $0A, "PLY", $0A, "PLY", $0A, "PLY", $0A, 0
; Byte logic: right operand in A, left on the stack.
.macro np_lower_logic_byte op
      .byte "STA NVR0L", $0A, "PLA", $0A, op, " NVR0L", $0A, 0
.endmacro

; Word logic: left operand in A/X, right in NVR0L/NVR0H.
.macro np_lower_logic_word op
      .byte op, " NVR0L", $0A, "PHA", $0A, "TXA", $0A
      .byte op, " NVR0H", $0A, "TAX", $0A, "PLA", $0A, 0
.endmacro

; Byte shift: count in A, value on the stack. A zero count must skip the loop
; body entirely, so the guard branches past both anonymous labels.
.macro np_lower_shift_byte op
      .byte "STA NVR0L", $0A, "PLA", $0A, "LDX NVR0L", $0A, "BEQ :++", $0A
      .byte ":", $0A, op, $0A, "DEX", $0A, "BNE :-", $0A, ":", $0A, 0
.endmacro

; Word shift: value in A/X, count in NVR0L. The value moves to NVR1 because a
; 16-bit shift needs memory operands.
.macro np_lower_shift_word first, second
      .byte "STA NVR1L", $0A, "STX NVR1H", $0A, "LDY NVR0L", $0A, "BEQ :++", $0A
      .byte ":", $0A, first, $0A, second, $0A, "DEY", $0A, "BNE :-", $0A
      .byte ":", $0A, "LDA NVR1L", $0A, "LDX NVR1H", $0A, 0
.endmacro

np_lower_or:
      np_lower_logic_byte "ORA"
np_lower_and:
      np_lower_logic_byte "AND"
np_lower_xor:
      np_lower_logic_byte "EOR"
np_lower_or_word:
      np_lower_logic_word "ORA"
np_lower_and_word:
      np_lower_logic_word "AND"
np_lower_xor_word:
      np_lower_logic_word "EOR"
np_lower_shl:
      np_lower_shift_byte "ASL"
np_lower_shr:
      np_lower_shift_byte "LSR"
np_lower_shl_word:
      np_lower_shift_word "ASL NVR1L", "ROL NVR1H"
np_lower_shr_word:
      np_lower_shift_word "LSR NVR1H", "ROR NVR1L"
np_lower_rhs_word:
      .byte "STA NVR0L", $0A, "STX NVR0H", $0A, 0
np_lower_rhs_byte:
      .byte "STA NVR0L", $0A, "STZ NVR0H", $0A, 0
np_lower_lhs_word:
      .byte "PLA", $0A, "PLX", $0A, 0
np_lower_lhs_byte:
      .byte "PLA", $0A, "LDX #$00", $0A, 0
np_lower_segment_bss:
      .byte ".SEGMENT ", 34, "BSS", 34, $0A, 0
np_lower_segment_rodata:
      .byte ".SEGMENT ", 34, "RODATA", 34, $0A, 0
np_lower_segment_code:
      .byte ".SEGMENT ", 34, "CODE", 34, $0A, 0
np_lower_heap_alloc_call:
      .byte "JSR ndk_ram_alloc", $0A, 0
np_lower_heap_free_call:
      .byte "JSR ndk_ram_free", $0A, 0
np_lower_heap_include:
      .byte ".INCLUDE ", 34, "RAMHEAP.S", 34, $0A, 0
np_lower_with_push:
      .byte "JSR P_WITH_PUSH", $0A, 0
np_lower_with_pop:
      .byte "JSR P_WITH_POP", $0A, 0
np_lower_write_char_call:
      .byte "JSR P_WRITE_CHAR", $0A, 0
np_lower_write_byte_call:
      .byte "JSR P_WRITE_BYTE", $0A, 0
np_lower_write_byte_ln_call:
      .byte "JSR P_WRITE_BYTE_LN", $0A, 0
np_lower_write_word_call:
      .byte "JSR P_WRITE_WORD", $0A, 0
np_lower_write_uword_call:
      .byte "JSR P_WRITE_UWORD", $0A, 0
np_lower_write_format_call:
      .byte "JSR P_WRITE_FORMAT", $0A, 0
np_lower_write_string_call:
      .byte "JSR P_WRITE_STRING", $0A, 0
np_lower_write_real_call:
      .byte "JSR P_WRITE_REAL", $0A, 0
np_lower_indirect_call:
      .byte "JSR P_CALL_INDIRECT", $0A, 0
np_lower_char_to_string:
      .byte "JSR P_CHAR_TO_STRING", $0A, 0
np_lower_string_copy_part_call:
      .byte "JSR P_STRING_COPY_PART", $0A, 0
np_lower_str_integer_call:
      .byte "JSR P_STR_INTEGER", $0A, 0
np_lower_str_uword_call:
      .byte "JSR P_STR_UWORD", $0A, 0
np_lower_str_real_call:
      .byte "JSR P_STR_REAL", $0A, 0
np_lower_str_long_call:
      .byte "JSR P_STR_LONG", $0A, 0
np_lower_write_long_call:
      .byte "JSR P_WRITE_LONG", $0A, 0
np_lower_str_real_precision_call:
      .byte "JSR P_STR_REAL_PRECISION", $0A, 0
np_lower_index_byte_stack_call:
      .byte "JSR I_ARRAY_INDEX_BYTE_STACK", $0A, 0
np_lower_set_in8_call:
      .byte "JSR P_SET_IN8", $0A, 0
np_lower_set_in256_call:
      .byte "JSR P_SET_IN256", $0A, 0
np_lower_compare_signed_word:
      .byte "JSR P_CMPSW", $0A, 0

.endif

.endif

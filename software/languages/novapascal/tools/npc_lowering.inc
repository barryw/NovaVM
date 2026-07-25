; Complete NPC lowering recipes live with NPO2, which runs after every compile.
; NPC emits one compact, printable .O2T record instead of carrying these
; textual 65C02 sequences in its constrained frontend overlay.

.ifndef NPC_LOWERING_INC
NPC_LOWERING_INC = 1

; Typed IR names originate in NPC identifiers. NPO2 uses the same bound for
; array-cache scratch while retaining its wider buffers for generated labels.
NP_TYPED_SYMBOL_CAP = 32

NP_LOWER_HEAP_STORE               = 0
NP_LOWER_DEREF16                  = 1
NP_LOWER_DEREF_NVR0_16            = 2
NP_LOWER_FRAME_POINTER_TO_NVR0    = 3
NP_LOWER_STACK_ADDRESS_TO_NVR0    = 4
NP_LOWER_STACK_STORE_BYTE         = 5
NP_LOWER_STACK_STORE_WORD         = 6
NP_LOWER_STACK_STRING_SAVE        = 7
NP_LOWER_REAL_STORE_INTEGER       = 8
NP_LOWER_SCALAR4_COPY             = 9
NP_LOWER_PUSH_SCALAR4             = 10
NP_LOWER_DEFAULT_KBD_PUSH         = 11
NP_LOWER_STRING_CONCAT            = 12
NP_LOWER_STRING_CONCAT_CHAR       = 13
NP_LOWER_CHAR_CONCAT_STRING       = 14
NP_LOWER_STRING_COMPARE           = 15
NP_LOWER_CHAR_STRING_COMPARE      = 16
NP_LOWER_STRING_POS               = 17
NP_LOWER_FILE_WRITE_CHAR          = 18
NP_LOWER_FILE_WRITE_BYTE          = 19
NP_LOWER_FILE_WRITE_WORD          = 20
NP_LOWER_FILE_WRITE_STRING        = 21
NP_LOWER_FILE_WRITE_REAL          = 22
NP_LOWER_FILE_WRITE_NEWLINE       = 23
NP_LOWER_NEGATE_WORD              = 24
NP_LOWER_REAL_ADD                 = 25
NP_LOWER_REAL_SUBTRACT            = 26
NP_LOWER_REAL_COMPARE             = 27
NP_LOWER_REAL_MULTIPLY            = 28
NP_LOWER_WORD_MULTIPLY_REAL       = 29
NP_LOWER_BYTE_MULTIPLY_REAL       = 30
NP_LOWER_REAL_MULTIPLY_WORD_RIGHT = 31
NP_LOWER_REAL_MULTIPLY_BYTE_RIGHT = 32
NP_LOWER_REAL_DIVIDE              = 33
NP_LOWER_REAL_DIVIDE_WORD         = 34
NP_LOWER_REAL_DIVIDE_BYTE         = 35
NP_LOWER_WORD_DIVIDE_REAL         = 36
NP_LOWER_BYTE_DIVIDE_REAL         = 37
NP_LOWER_DIVIDE_BYTE              = 38
NP_LOWER_MOD_BYTE                 = 39
NP_LOWER_LONG_ADD                 = 40
NP_LOWER_LONG_SUBTRACT            = 41
NP_LOWER_LONG_COMPARE             = 42
NP_LOWER_MAIN_LABEL_FRAMES        = 43
NP_LOWER_FRAME_LOAD16_TAIL        = 44
NP_LOWER_FRAME_STORE16_TAIL       = 45
NP_LOWER_FRAME_STORE_REF8_TAIL    = 46
NP_LOWER_FRAME_STORE_REF16_TAIL   = 47
NP_LOWER_FILE_WRITE_BLOCK         = 48
NP_LOWER_CODE_WITH_ROUTINES       = 49
NP_LOWER_WITH_ADDR_HEAD           = 50
NP_LOWER_STRING_COPY_CALL         = 51
NP_LOWER_BLOCK_COPY_CALL          = 52
NP_LOWER_CHECK_OVERFLOW           = 53
NP_LOWER_CHECK_RANGE_U            = 54
NP_LOWER_CHECK_RANGE_S            = 55
NP_LOWER_STACK_CHECK              = 56
NP_LOWER_IO_CHECK_READ            = 57
NP_LOWER_IO_CHECK_WRITE           = 58
NP_LOWER_STRING_INDEX_STACK_CALL  = 59
NP_LOWER_CHECK_UADD               = 60
NP_LOWER_CHECK_USUB               = 61
NP_LOWER_CHECK_MULS               = 62
NP_LOWER_CHECK_MULU               = 63
NP_LOWER_CHECK_NEGS               = 64
NP_LOWER_CHECK_NEGU               = 65
NP_LOWER_CHECK_DIVS               = 66
NP_LOWER_CHECK_ZERO8              = 67
NP_LOWER_CHECK_ZERO16             = 68
NP_LOWER_CHECK_RANGE_OPEN         = 69
NP_LOWER_SET_DIFFERENCE           = 70
NP_LOWER_SAVE_INCOMING            = 71
NP_LOWER_OPEN_BASE_SAVE           = 72
NP_LOWER_OPEN_BASE_RESTORE        = 73
NP_LOWER_FRAME_LOAD8_TAIL         = 74
NP_LOWER_FRAME_STORE8_TAIL        = 75
NP_LOWER_FRAME_DIRECT_LOAD16      = 76
NP_LOWER_FRAME_DIRECT_STORE16     = 77
NP_LOWER_DEREF8                   = 78
NP_LOWER_DEREF_NVR0_8             = 79
NP_LOWER_ADDRESS_RESULT           = 80
NP_LOWER_INCREMENT_WORD           = 81
NP_LOWER_STRING_COPY_SAVE         = 82
NP_LOWER_LOAD_SAVED_NVR2          = 83
NP_LOWER_ADDRESS_TO_NVR0          = 84
NP_LOWER_REAL_SAVE_BYTE           = 85
NP_LOWER_REAL_SAVE_WORD           = 86
NP_LOWER_DROP_FOUR                = 87
NP_LOWER_OR                       = 88
NP_LOWER_AND                      = 89
NP_LOWER_RHS_WORD                 = 90
NP_LOWER_RHS_BYTE                 = 91
NP_LOWER_LHS_WORD                 = 92
NP_LOWER_LHS_BYTE                 = 93
NP_LOWER_SEGMENT_BSS              = 94
NP_LOWER_SEGMENT_RODATA           = 95
NP_LOWER_SEGMENT_CODE             = 96
NP_LOWER_HEAP_ALLOC_CALL          = 97
NP_LOWER_HEAP_FREE_CALL           = 98
NP_LOWER_HEAP_INCLUDE             = 99
NP_LOWER_WITH_PUSH                = 100
NP_LOWER_WITH_POP                 = 101
NP_LOWER_WRITE_CHAR_CALL          = 102
NP_LOWER_WRITE_BYTE_CALL          = 103
NP_LOWER_WRITE_BYTE_LN_CALL       = 104
NP_LOWER_WRITE_WORD_CALL          = 105
NP_LOWER_WRITE_UWORD_CALL         = 106
NP_LOWER_WRITE_FORMAT_CALL        = 107
NP_LOWER_WRITE_STRING_CALL        = 108
NP_LOWER_WRITE_REAL_CALL          = 109
NP_LOWER_INDIRECT_CALL            = 110
NP_LOWER_CHAR_TO_STRING           = 111
NP_LOWER_STRING_COPY_PART_CALL    = 112
NP_LOWER_STR_INTEGER_CALL         = 113
NP_LOWER_STR_UWORD_CALL           = 114
NP_LOWER_STR_REAL_CALL            = 115
NP_LOWER_STR_REAL_PRECISION_CALL  = 116
NP_LOWER_INDEX_BYTE_STACK_CALL    = 117
NP_LOWER_SET_IN8_CALL             = 118
NP_LOWER_SET_IN256_CALL           = 119
NP_LOWER_COMPARE_SIGNED_WORD      = 120
NP_LOWER_COUNT                    = 121

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
.macro np_lower_scalar4_binary routine
      .byte "STA NVR2L", $0A, "STX NVR2H", $0A
      .byte "PLA", $0A, "STA NVR1H", $0A, "PLA", $0A, "STA NVR1L", $0A
      .byte "PLA", $0A, "STA NVR0H", $0A, "PLA", $0A, "STA NVR0L", $0A
      .byte "LDA NVR2L", $0A, "LDX NVR2H", $0A, "JSR ", routine, $0A, 0
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

np_lowering_lo:
      .byte <np_lower_heap_store, <np_lower_deref16, <np_lower_deref_nvr0_16
      .byte <np_lower_frame_pointer_to_nvr0, <np_lower_stack_address_to_nvr0
      .byte <np_lower_stack_store_byte, <np_lower_stack_store_word
      .byte <np_lower_stack_string_save, <np_lower_real_store_integer
      .byte <np_lower_scalar4_copy, <np_lower_push_scalar4, <np_lower_default_kbd_push
      .byte <np_lower_string_concat, <np_lower_string_concat_char
      .byte <np_lower_char_concat_string, <np_lower_string_compare
      .byte <np_lower_char_string_compare, <np_lower_string_pos
      .byte <np_lower_file_write_char, <np_lower_file_write_byte
      .byte <np_lower_file_write_word, <np_lower_file_write_string
      .byte <np_lower_file_write_real, <np_lower_file_write_newline
      .byte <np_lower_negate_word, <np_lower_real_add, <np_lower_real_subtract
      .byte <np_lower_real_compare, <np_lower_real_multiply
      .byte <np_lower_word_multiply_real, <np_lower_byte_multiply_real
      .byte <np_lower_real_multiply_word_right, <np_lower_real_multiply_byte_right
      .byte <np_lower_real_divide, <np_lower_real_divide_word
      .byte <np_lower_real_divide_byte, <np_lower_word_divide_real
      .byte <np_lower_byte_divide_real, <np_lower_divide_byte, <np_lower_mod_byte
      .byte <np_lower_long_add, <np_lower_long_subtract, <np_lower_long_compare
      .byte <np_lower_main_label_frames, <np_lower_frame_load16_tail
      .byte <np_lower_frame_store16_tail, <np_lower_frame_store_ref8_tail
      .byte <np_lower_frame_store_ref16_tail, <np_lower_file_write_block
      .byte <np_lower_code_with_routines, <np_lower_with_addr_head
      .byte <np_lower_string_copy_call, <np_lower_block_copy_call
      .byte <np_lower_check_overflow
      .byte <np_lower_check_range_u, <np_lower_check_range_s, <np_lower_stack_check
      .byte <np_lower_io_check_read, <np_lower_io_check_write
      .byte <np_lower_string_index_stack_call
      .byte <np_lower_check_uadd, <np_lower_check_usub, <np_lower_check_muls
      .byte <np_lower_check_mulu, <np_lower_check_negs, <np_lower_check_negu
      .byte <np_lower_check_divs, <np_lower_check_zero8, <np_lower_check_zero16
      .byte <np_lower_check_range_open, <np_lower_set_difference
      .byte <np_lower_save_incoming, <np_lower_open_base_save
      .byte <np_lower_open_base_restore, <np_lower_frame_load8_tail
      .byte <np_lower_frame_store8_tail, <np_lower_frame_direct_load16
      .byte <np_lower_frame_direct_store16, <np_lower_deref8
      .byte <np_lower_deref_nvr0_8, <np_lower_address_result
      .byte <np_lower_increment_word, <np_lower_string_copy_save
      .byte <np_lower_load_saved_nvr2, <np_lower_address_to_nvr0
      .byte <np_lower_real_save_byte, <np_lower_real_save_word
      .byte <np_lower_drop_four, <np_lower_or, <np_lower_and
      .byte <np_lower_rhs_word, <np_lower_rhs_byte
      .byte <np_lower_lhs_word, <np_lower_lhs_byte
      .byte <np_lower_segment_bss, <np_lower_segment_rodata
      .byte <np_lower_segment_code, <np_lower_heap_alloc_call
      .byte <np_lower_heap_free_call, <np_lower_heap_include
      .byte <np_lower_with_push, <np_lower_with_pop
      .byte <np_lower_write_char_call, <np_lower_write_byte_call
      .byte <np_lower_write_byte_ln_call, <np_lower_write_word_call
      .byte <np_lower_write_uword_call, <np_lower_write_format_call
      .byte <np_lower_write_string_call, <np_lower_write_real_call
      .byte <np_lower_indirect_call, <np_lower_char_to_string
      .byte <np_lower_string_copy_part_call, <np_lower_str_integer_call
      .byte <np_lower_str_uword_call, <np_lower_str_real_call
      .byte <np_lower_str_real_precision_call, <np_lower_index_byte_stack_call
      .byte <np_lower_set_in8_call, <np_lower_set_in256_call
      .byte <np_lower_compare_signed_word
np_lowering_hi:
      .byte >np_lower_heap_store, >np_lower_deref16, >np_lower_deref_nvr0_16
      .byte >np_lower_frame_pointer_to_nvr0, >np_lower_stack_address_to_nvr0
      .byte >np_lower_stack_store_byte, >np_lower_stack_store_word
      .byte >np_lower_stack_string_save, >np_lower_real_store_integer
      .byte >np_lower_scalar4_copy, >np_lower_push_scalar4, >np_lower_default_kbd_push
      .byte >np_lower_string_concat, >np_lower_string_concat_char
      .byte >np_lower_char_concat_string, >np_lower_string_compare
      .byte >np_lower_char_string_compare, >np_lower_string_pos
      .byte >np_lower_file_write_char, >np_lower_file_write_byte
      .byte >np_lower_file_write_word, >np_lower_file_write_string
      .byte >np_lower_file_write_real, >np_lower_file_write_newline
      .byte >np_lower_negate_word, >np_lower_real_add, >np_lower_real_subtract
      .byte >np_lower_real_compare, >np_lower_real_multiply
      .byte >np_lower_word_multiply_real, >np_lower_byte_multiply_real
      .byte >np_lower_real_multiply_word_right, >np_lower_real_multiply_byte_right
      .byte >np_lower_real_divide, >np_lower_real_divide_word
      .byte >np_lower_real_divide_byte, >np_lower_word_divide_real
      .byte >np_lower_byte_divide_real, >np_lower_divide_byte, >np_lower_mod_byte
      .byte >np_lower_long_add, >np_lower_long_subtract, >np_lower_long_compare
      .byte >np_lower_main_label_frames, >np_lower_frame_load16_tail
      .byte >np_lower_frame_store16_tail, >np_lower_frame_store_ref8_tail
      .byte >np_lower_frame_store_ref16_tail, >np_lower_file_write_block
      .byte >np_lower_code_with_routines, >np_lower_with_addr_head
      .byte >np_lower_string_copy_call, >np_lower_block_copy_call
      .byte >np_lower_check_overflow
      .byte >np_lower_check_range_u, >np_lower_check_range_s, >np_lower_stack_check
      .byte >np_lower_io_check_read, >np_lower_io_check_write
      .byte >np_lower_string_index_stack_call
      .byte >np_lower_check_uadd, >np_lower_check_usub, >np_lower_check_muls
      .byte >np_lower_check_mulu, >np_lower_check_negs, >np_lower_check_negu
      .byte >np_lower_check_divs, >np_lower_check_zero8, >np_lower_check_zero16
      .byte >np_lower_check_range_open, >np_lower_set_difference
      .byte >np_lower_save_incoming, >np_lower_open_base_save
      .byte >np_lower_open_base_restore, >np_lower_frame_load8_tail
      .byte >np_lower_frame_store8_tail, >np_lower_frame_direct_load16
      .byte >np_lower_frame_direct_store16, >np_lower_deref8
      .byte >np_lower_deref_nvr0_8, >np_lower_address_result
      .byte >np_lower_increment_word, >np_lower_string_copy_save
      .byte >np_lower_load_saved_nvr2, >np_lower_address_to_nvr0
      .byte >np_lower_real_save_byte, >np_lower_real_save_word
      .byte >np_lower_drop_four, >np_lower_or, >np_lower_and
      .byte >np_lower_rhs_word, >np_lower_rhs_byte
      .byte >np_lower_lhs_word, >np_lower_lhs_byte
      .byte >np_lower_segment_bss, >np_lower_segment_rodata
      .byte >np_lower_segment_code, >np_lower_heap_alloc_call
      .byte >np_lower_heap_free_call, >np_lower_heap_include
      .byte >np_lower_with_push, >np_lower_with_pop
      .byte >np_lower_write_char_call, >np_lower_write_byte_call
      .byte >np_lower_write_byte_ln_call, >np_lower_write_word_call
      .byte >np_lower_write_uword_call, >np_lower_write_format_call
      .byte >np_lower_write_string_call, >np_lower_write_real_call
      .byte >np_lower_indirect_call, >np_lower_char_to_string
      .byte >np_lower_string_copy_part_call, >np_lower_str_integer_call
      .byte >np_lower_str_uword_call, >np_lower_str_real_call
      .byte >np_lower_str_real_precision_call, >np_lower_index_byte_stack_call
      .byte >np_lower_set_in8_call, >np_lower_set_in256_call
      .byte >np_lower_compare_signed_word

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
      np_lower_scalar4_binary "P_REAL_ADD"
np_lower_real_subtract:
      np_lower_scalar4_binary "P_REAL_SUB"
np_lower_real_compare:
      np_lower_scalar4_binary "P_REAL_CMP"
np_lower_real_multiply:
      np_lower_scalar4_binary "P_REAL_MUL"
np_lower_real_divide:
      np_lower_scalar4_binary "P_REAL_DIV"
np_lower_real_divide_word:
      np_lower_scalar4_binary "P_REAL_DIV_WORD"

np_lower_long_add:
      np_lower_scalar4_binary "P_LONG_ADD"
np_lower_long_subtract:
      np_lower_scalar4_binary "P_LONG_SUB"
np_lower_long_compare:
      np_lower_scalar4_binary "P_LONG_CMP"

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
np_lower_or:
      .byte "STA NVR0L", $0A, "PLA", $0A, "ORA NVR0L", $0A, 0
np_lower_and:
      .byte "STA NVR0L", $0A, "PLA", $0A, "AND NVR0L", $0A, 0
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

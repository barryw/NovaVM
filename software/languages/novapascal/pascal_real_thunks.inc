; Compact import thunks for the paged LANGRT 16.16 implementation. Keeping the
; Pascal symbols in one ordinary NOBJ member means NL remains language-agnostic.
; The member's shape comes from pascal_langrt_thunk.inc; this file is only the
; service list.

.macro REAL_THUNK_SET emit
      emit real_from_byte_thunk, LANGRT_REAL_FROM_BYTE, "P_REAL_FROM_BYTE"
      emit real_from_word_thunk, LANGRT_REAL_FROM_WORD, "P_REAL_FROM_WORD"
      emit real_negate_thunk, LANGRT_REAL_NEGATE, "P_REAL_NEGATE"
      emit real_add_thunk, LANGRT_REAL_ADD, "P_REAL_ADD"
      emit real_sub_thunk, LANGRT_REAL_SUB, "P_REAL_SUB"
      emit real_cmp_thunk, LANGRT_REAL_CMP, "P_REAL_CMP"
      emit real_mul_thunk, LANGRT_REAL_MUL, "P_REAL_MUL"
      emit real_mul_word_left_thunk, LANGRT_REAL_MUL_WORD_LEFT, "P_REAL_MUL_WORD_LEFT"
      emit real_mul_byte_left_thunk, LANGRT_REAL_MUL_BYTE_LEFT, "P_REAL_MUL_BYTE_LEFT"
      emit real_mul_word_right_thunk, LANGRT_REAL_MUL_WORD_RIGHT, "P_REAL_MUL_WORD_RIGHT"
      emit real_mul_byte_right_thunk, LANGRT_REAL_MUL_BYTE_RIGHT, "P_REAL_MUL_BYTE_RIGHT"
      emit real_div_thunk, LANGRT_REAL_DIV, "P_REAL_DIV"
      emit real_div_word_thunk, LANGRT_REAL_DIV_WORD, "P_REAL_DIV_WORD"
      emit real_div_byte_thunk, LANGRT_REAL_DIV_BYTE, "P_REAL_DIV_BYTE"
      emit word_div_real_thunk, LANGRT_WORD_DIV_REAL, "P_WORD_DIV_REAL"
      emit byte_div_real_thunk, LANGRT_BYTE_DIV_REAL, "P_BYTE_DIV_REAL"
      emit ordinal_div_real_thunk, LANGRT_ORDINAL_DIV_REAL, "P_ORDINAL_DIV_REAL"
      emit str_real_thunk, LANGRT_STR_REAL, "P_STR_REAL"
      emit str_real_precision_thunk, LANGRT_STR_REAL_PRECISION, "P_STR_REAL_PRECISION"
      emit real_abs_thunk, LANGRT_REAL_ABS, "ABS"
      emit real_sqrt_thunk, LANGRT_REAL_SQRT, "SQRT"
      emit real_sqr_thunk, LANGRT_REAL_SQR, "SQR"
      emit real_sin_thunk, LANGRT_REAL_SIN, "SIN"
      emit real_cos_thunk, LANGRT_REAL_COS, "COS"
      emit real_arctan_thunk, LANGRT_REAL_ARCTAN, "ARCTAN"
      emit real_ln_thunk, LANGRT_REAL_LN, "LN"
      emit real_exp_thunk, LANGRT_REAL_EXP, "EXP"
      emit real_trunc_thunk, LANGRT_REAL_TRUNC, "TRUNC"
      emit real_val_thunk, LANGRT_REAL_VAL, "VAL"
      emit real_round_thunk, LANGRT_REAL_ROUND, "ROUND"
.endmacro

langrt_thunk_n .set 0
      REAL_THUNK_SET LANGRT_THUNK_COUNT
langrt_thunk_rtcall_index .set langrt_thunk_n

      .word real_thunk_object_end-real_thunk_object
real_thunk_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 1, $FF
      .word 0
      .word langrt_thunk_rtcall_index+1     ; thunks plus the undefined P_RTCALL
      .word real_thunk_symbols-real_thunk_object
      .word langrt_thunk_rtcall_index       ; one JMP operand per thunk
      .word real_thunk_relocations-real_thunk_object
      .word 0

      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_EXEC, 0, 4, 0
      .word real_thunk_code_end-real_thunk_code
      .word real_thunk_code_end-real_thunk_code
      .byte "CODE"

.define langrt_thunk_base real_thunk_code
real_thunk_code:
      REAL_THUNK_SET LANGRT_THUNK_CODE
real_thunk_code_end:

real_thunk_symbols:
      REAL_THUNK_SET LANGRT_THUNK_SYMBOL
      .word 0
      .byte NOBJ_SYM_UNDEFINED, NOBJ_SYM_GLOBAL, 8, "P_RTCALL"

real_thunk_relocations:
      REAL_THUNK_SET LANGRT_THUNK_RELOC
real_thunk_object_end:
.undefine langrt_thunk_base

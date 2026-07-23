; Compact import thunks for the paged LANGRT 16.16 implementation. Keeping the
; Pascal symbols in one ordinary NOBJ member means NL remains language-agnostic.

.macro REAL_THUNK routine, call_site, function_id
routine:
      LDY   #function_id
      .byte $4C
call_site: .word 0                       ; JMP P_RTCALL
.endmacro

.macro REAL_THUNK_RELOC call_site
      .byte 0, NOBJ_RELOC_ABS16
      .word call_site-real_thunk_code, 29, 0
.endmacro

      .word real_thunk_object_end-real_thunk_object
real_thunk_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 1, $FF
      .word 0
      .word 30
      .word real_thunk_symbols-real_thunk_object
      .word 29
      .word real_thunk_relocations-real_thunk_object
      .word 0

      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_EXEC, 0, 4, 0
      .word real_thunk_code_end-real_thunk_code
      .word real_thunk_code_end-real_thunk_code
      .byte "CODE"
real_thunk_code:
      REAL_THUNK real_from_byte_thunk, real_from_byte_call, LANGRT_REAL_FROM_BYTE
      REAL_THUNK real_from_word_thunk, real_from_word_call, LANGRT_REAL_FROM_WORD
      REAL_THUNK real_negate_thunk, real_negate_call, LANGRT_REAL_NEGATE
      REAL_THUNK real_add_thunk, real_add_call, LANGRT_REAL_ADD
      REAL_THUNK real_sub_thunk, real_sub_call, LANGRT_REAL_SUB
      REAL_THUNK real_cmp_thunk, real_cmp_call, LANGRT_REAL_CMP
      REAL_THUNK real_mul_thunk, real_mul_call, LANGRT_REAL_MUL
      REAL_THUNK real_mul_word_left_thunk, real_mul_word_left_call, LANGRT_REAL_MUL_WORD_LEFT
      REAL_THUNK real_mul_byte_left_thunk, real_mul_byte_left_call, LANGRT_REAL_MUL_BYTE_LEFT
      REAL_THUNK real_mul_word_right_thunk, real_mul_word_right_call, LANGRT_REAL_MUL_WORD_RIGHT
      REAL_THUNK real_mul_byte_right_thunk, real_mul_byte_right_call, LANGRT_REAL_MUL_BYTE_RIGHT
      REAL_THUNK real_div_thunk, real_div_call, LANGRT_REAL_DIV
      REAL_THUNK real_div_word_thunk, real_div_word_call, LANGRT_REAL_DIV_WORD
      REAL_THUNK real_div_byte_thunk, real_div_byte_call, LANGRT_REAL_DIV_BYTE
      REAL_THUNK word_div_real_thunk, word_div_real_call, LANGRT_WORD_DIV_REAL
      REAL_THUNK byte_div_real_thunk, byte_div_real_call, LANGRT_BYTE_DIV_REAL
      REAL_THUNK ordinal_div_real_thunk, ordinal_div_real_call, LANGRT_ORDINAL_DIV_REAL
      REAL_THUNK str_real_thunk, str_real_call, LANGRT_STR_REAL
      REAL_THUNK str_real_precision_thunk, str_real_precision_call, LANGRT_STR_REAL_PRECISION
      REAL_THUNK real_abs_thunk, real_abs_call, LANGRT_REAL_ABS
      REAL_THUNK real_sqrt_thunk, real_sqrt_call, LANGRT_REAL_SQRT
      REAL_THUNK real_sqr_thunk, real_sqr_call, LANGRT_REAL_SQR
      REAL_THUNK real_sin_thunk, real_sin_call, LANGRT_REAL_SIN
      REAL_THUNK real_cos_thunk, real_cos_call, LANGRT_REAL_COS
      REAL_THUNK real_arctan_thunk, real_arctan_call, LANGRT_REAL_ARCTAN
      REAL_THUNK real_ln_thunk, real_ln_call, LANGRT_REAL_LN
      REAL_THUNK real_exp_thunk, real_exp_call, LANGRT_REAL_EXP
      REAL_THUNK real_trunc_thunk, real_trunc_call, LANGRT_REAL_TRUNC
      REAL_THUNK real_val_thunk, real_val_call, LANGRT_REAL_VAL
real_thunk_code_end:

real_thunk_symbols:
      .word real_from_byte_thunk-real_thunk_code
      .byte 0, NOBJ_SYM_GLOBAL, 16, "P_REAL_FROM_BYTE"
      .word real_from_word_thunk-real_thunk_code
      .byte 0, NOBJ_SYM_GLOBAL, 16, "P_REAL_FROM_WORD"
      .word real_negate_thunk-real_thunk_code
      .byte 0, NOBJ_SYM_GLOBAL, 13, "P_REAL_NEGATE"
      .word real_add_thunk-real_thunk_code
      .byte 0, NOBJ_SYM_GLOBAL, 10, "P_REAL_ADD"
      .word real_sub_thunk-real_thunk_code
      .byte 0, NOBJ_SYM_GLOBAL, 10, "P_REAL_SUB"
      .word real_cmp_thunk-real_thunk_code
      .byte 0, NOBJ_SYM_GLOBAL, 10, "P_REAL_CMP"
      .word real_mul_thunk-real_thunk_code
      .byte 0, NOBJ_SYM_GLOBAL, 10, "P_REAL_MUL"
      .word real_mul_word_left_thunk-real_thunk_code
      .byte 0, NOBJ_SYM_GLOBAL, 20, "P_REAL_MUL_WORD_LEFT"
      .word real_mul_byte_left_thunk-real_thunk_code
      .byte 0, NOBJ_SYM_GLOBAL, 20, "P_REAL_MUL_BYTE_LEFT"
      .word real_mul_word_right_thunk-real_thunk_code
      .byte 0, NOBJ_SYM_GLOBAL, 21, "P_REAL_MUL_WORD_RIGHT"
      .word real_mul_byte_right_thunk-real_thunk_code
      .byte 0, NOBJ_SYM_GLOBAL, 21, "P_REAL_MUL_BYTE_RIGHT"
      .word real_div_thunk-real_thunk_code
      .byte 0, NOBJ_SYM_GLOBAL, 10, "P_REAL_DIV"
      .word real_div_word_thunk-real_thunk_code
      .byte 0, NOBJ_SYM_GLOBAL, 15, "P_REAL_DIV_WORD"
      .word real_div_byte_thunk-real_thunk_code
      .byte 0, NOBJ_SYM_GLOBAL, 15, "P_REAL_DIV_BYTE"
      .word word_div_real_thunk-real_thunk_code
      .byte 0, NOBJ_SYM_GLOBAL, 15, "P_WORD_DIV_REAL"
      .word byte_div_real_thunk-real_thunk_code
      .byte 0, NOBJ_SYM_GLOBAL, 15, "P_BYTE_DIV_REAL"
      .word ordinal_div_real_thunk-real_thunk_code
      .byte 0, NOBJ_SYM_GLOBAL, 18, "P_ORDINAL_DIV_REAL"
      .word str_real_thunk-real_thunk_code
      .byte 0, NOBJ_SYM_GLOBAL, 10, "P_STR_REAL"
      .word str_real_precision_thunk-real_thunk_code
      .byte 0, NOBJ_SYM_GLOBAL, 20, "P_STR_REAL_PRECISION"
      .word real_abs_thunk-real_thunk_code
      .byte 0, NOBJ_SYM_GLOBAL, 3, "ABS"
      .word real_sqrt_thunk-real_thunk_code
      .byte 0, NOBJ_SYM_GLOBAL, 4, "SQRT"
      .word real_sqr_thunk-real_thunk_code
      .byte 0, NOBJ_SYM_GLOBAL, 3, "SQR"
      .word real_sin_thunk-real_thunk_code
      .byte 0, NOBJ_SYM_GLOBAL, 3, "SIN"
      .word real_cos_thunk-real_thunk_code
      .byte 0, NOBJ_SYM_GLOBAL, 3, "COS"
      .word real_arctan_thunk-real_thunk_code
      .byte 0, NOBJ_SYM_GLOBAL, 6, "ARCTAN"
      .word real_ln_thunk-real_thunk_code
      .byte 0, NOBJ_SYM_GLOBAL, 2, "LN"
      .word real_exp_thunk-real_thunk_code
      .byte 0, NOBJ_SYM_GLOBAL, 3, "EXP"
      .word real_trunc_thunk-real_thunk_code
      .byte 0, NOBJ_SYM_GLOBAL, 5, "TRUNC"
      .word real_val_thunk-real_thunk_code
      .byte 0, NOBJ_SYM_GLOBAL, 3, "VAL"
      .word 0
      .byte NOBJ_SYM_UNDEFINED, NOBJ_SYM_GLOBAL, 8, "P_RTCALL"

real_thunk_relocations:
      REAL_THUNK_RELOC real_from_byte_call
      REAL_THUNK_RELOC real_from_word_call
      REAL_THUNK_RELOC real_negate_call
      REAL_THUNK_RELOC real_add_call
      REAL_THUNK_RELOC real_sub_call
      REAL_THUNK_RELOC real_cmp_call
      REAL_THUNK_RELOC real_mul_call
      REAL_THUNK_RELOC real_mul_word_left_call
      REAL_THUNK_RELOC real_mul_byte_left_call
      REAL_THUNK_RELOC real_mul_word_right_call
      REAL_THUNK_RELOC real_mul_byte_right_call
      REAL_THUNK_RELOC real_div_call
      REAL_THUNK_RELOC real_div_word_call
      REAL_THUNK_RELOC real_div_byte_call
      REAL_THUNK_RELOC word_div_real_call
      REAL_THUNK_RELOC byte_div_real_call
      REAL_THUNK_RELOC ordinal_div_real_call
      REAL_THUNK_RELOC str_real_call
      REAL_THUNK_RELOC str_real_precision_call
      REAL_THUNK_RELOC real_abs_call
      REAL_THUNK_RELOC real_sqrt_call
      REAL_THUNK_RELOC real_sqr_call
      REAL_THUNK_RELOC real_sin_call
      REAL_THUNK_RELOC real_cos_call
      REAL_THUNK_RELOC real_arctan_call
      REAL_THUNK_RELOC real_ln_call
      REAL_THUNK_RELOC real_exp_call
      REAL_THUNK_RELOC real_trunc_call
      REAL_THUNK_RELOC real_val_call
real_thunk_object_end:

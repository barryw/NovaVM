; Compact imports for signed 32-bit LANGRT services. The member's shape comes
; from pascal_langrt_thunk.inc; this file is only the service list.
;
; Negate, add, subtract, and compare deliberately reuse the Q16.16 entry points
; inside LANGRT: on two's-complement 32-bit cells those four operations are
; bit-identical to their fixed-point counterparts. Multiply, divide, and mod are
; not, so they have their own services.

.macro LONG_THUNK_SET emit
      emit long_from_byte_thunk, LANGRT_LONG_FROM_BYTE, "P_LONG_FROM_BYTE"
      emit long_from_uword_thunk, LANGRT_LONG_FROM_UWORD, "P_LONG_FROM_UWORD"
      emit long_from_integer_thunk, LANGRT_LONG_FROM_INTEGER, "P_LONG_FROM_INTEGER"
      emit long_negate_thunk, LANGRT_LONG_NEGATE, "P_LONG_NEGATE"
      emit long_add_thunk, LANGRT_LONG_ADD, "P_LONG_ADD"
      emit long_sub_thunk, LANGRT_LONG_SUB, "P_LONG_SUB"
      emit long_cmp_thunk, LANGRT_LONG_CMP, "P_LONG_CMP"
      emit long_mul_thunk, LANGRT_LONG_MUL, "P_LONG_MUL"
      emit long_div_thunk, LANGRT_LONG_DIV, "P_LONG_DIV"
      emit long_mod_thunk, LANGRT_LONG_MOD, "P_LONG_MOD"
      emit str_long_thunk, LANGRT_STR_LONG, "P_STR_LONG"
.endmacro

langrt_thunk_n .set 0
      LONG_THUNK_SET LANGRT_THUNK_COUNT
langrt_thunk_rtcall_index .set langrt_thunk_n

      .word long_thunk_object_end-long_thunk_object
long_thunk_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 1, $FF
      .word 0
      .word langrt_thunk_rtcall_index+1     ; thunks plus the undefined P_RTCALL
      .word long_thunk_symbols-long_thunk_object
      .word langrt_thunk_rtcall_index       ; one JMP operand per thunk
      .word long_thunk_relocations-long_thunk_object
      .word 0

      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_EXEC, 0, 4, 0
      .word long_thunk_code_end-long_thunk_code
      .word long_thunk_code_end-long_thunk_code
      .byte "CODE"

.define langrt_thunk_base long_thunk_code
long_thunk_code:
      LONG_THUNK_SET LANGRT_THUNK_CODE
long_thunk_code_end:

long_thunk_symbols:
      LONG_THUNK_SET LANGRT_THUNK_SYMBOL
      .word 0
      .byte NOBJ_SYM_UNDEFINED, NOBJ_SYM_GLOBAL, 8, "P_RTCALL"

long_thunk_relocations:
      LONG_THUNK_SET LANGRT_THUNK_RELOC
long_thunk_object_end:
.undefine langrt_thunk_base

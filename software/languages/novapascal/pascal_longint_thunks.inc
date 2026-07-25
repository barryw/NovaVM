; Compact imports for signed 32-bit LANGRT services.

.macro LONG_THUNK routine, call_site, function_id
routine:
      LDY   #function_id
      .byte $4C
call_site: .word 0                       ; JMP P_RTCALL
.endmacro

.macro LONG_THUNK_RELOC call_site
      .byte 0, NOBJ_RELOC_ABS16
      .word call_site-long_thunk_code, 7, 0
.endmacro

      .word long_thunk_object_end-long_thunk_object
long_thunk_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 1, $FF
      .word 0
      .word 8
      .word long_thunk_symbols-long_thunk_object
      .word 7
      .word long_thunk_relocations-long_thunk_object
      .word 0

      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_EXEC, 0, 4, 0
      .word long_thunk_code_end-long_thunk_code
      .word long_thunk_code_end-long_thunk_code
      .byte "CODE"
long_thunk_code:
      LONG_THUNK long_from_byte_thunk, long_from_byte_call, LANGRT_LONG_FROM_BYTE
      LONG_THUNK long_from_uword_thunk, long_from_uword_call, LANGRT_LONG_FROM_UWORD
      LONG_THUNK long_from_integer_thunk, long_from_integer_call, LANGRT_LONG_FROM_INTEGER
      LONG_THUNK long_negate_thunk, long_negate_call, LANGRT_LONG_NEGATE
      LONG_THUNK long_add_thunk, long_add_call, LANGRT_LONG_ADD
      LONG_THUNK long_sub_thunk, long_sub_call, LANGRT_LONG_SUB
      LONG_THUNK long_cmp_thunk, long_cmp_call, LANGRT_LONG_CMP
long_thunk_code_end:

long_thunk_symbols:
      .word long_from_byte_thunk-long_thunk_code
      .byte 0, NOBJ_SYM_GLOBAL, 16, "P_LONG_FROM_BYTE"
      .word long_from_uword_thunk-long_thunk_code
      .byte 0, NOBJ_SYM_GLOBAL, 17, "P_LONG_FROM_UWORD"
      .word long_from_integer_thunk-long_thunk_code
      .byte 0, NOBJ_SYM_GLOBAL, 19, "P_LONG_FROM_INTEGER"
      .word long_negate_thunk-long_thunk_code
      .byte 0, NOBJ_SYM_GLOBAL, 13, "P_LONG_NEGATE"
      .word long_add_thunk-long_thunk_code
      .byte 0, NOBJ_SYM_GLOBAL, 10, "P_LONG_ADD"
      .word long_sub_thunk-long_thunk_code
      .byte 0, NOBJ_SYM_GLOBAL, 10, "P_LONG_SUB"
      .word long_cmp_thunk-long_thunk_code
      .byte 0, NOBJ_SYM_GLOBAL, 10, "P_LONG_CMP"
      .word 0
      .byte NOBJ_SYM_UNDEFINED, NOBJ_SYM_GLOBAL, 8, "P_RTCALL"

long_thunk_relocations:
      LONG_THUNK_RELOC long_from_byte_call
      LONG_THUNK_RELOC long_from_uword_call
      LONG_THUNK_RELOC long_from_integer_call
      LONG_THUNK_RELOC long_negate_call
      LONG_THUNK_RELOC long_add_call
      LONG_THUNK_RELOC long_sub_call
      LONG_THUNK_RELOC long_cmp_call
long_thunk_object_end:

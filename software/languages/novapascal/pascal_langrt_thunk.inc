; Shared shape for a NOBJ archive member of LANGRT import thunks.
;
; A member declares its thunks once, as a macro taking a single `emit`
; parameter, then hands that registry to LANGRT_THUNK_MEMBER. Everything the
; NOBJ format needs — symbol count, relocation count, per-symbol name lengths,
; and the P_RTCALL symbol index every relocation points at — is derived from
; that one list, so a new service is one line instead of three lines plus two
; hand-counted headers.
;
; Each registry line is:  emit <label>, <LANGRT function id>, "<PASCAL SYMBOL>"
; The JMP operand patched by the linker is the derived label <label>_call.

.ifndef PASCAL_LANGRT_THUNK_INC
PASCAL_LANGRT_THUNK_INC = 1

.macro LANGRT_THUNK_CALL_SITE label
      .ident(.concat(.string(label), "_call"))
.endmacro

.macro LANGRT_THUNK_COUNT label, function_id, name
      langrt_thunk_n .set langrt_thunk_n + 1
.endmacro

.macro LANGRT_THUNK_CODE label, function_id, name
label:
      LDY   #function_id
      .byte $4C
.ident(.concat(.string(label), "_call")): .word 0        ; JMP P_RTCALL
.endmacro

; Relocations and symbols are emitted relative to the member's own code base.
; A member publishes that base with `.define langrt_thunk_base <label>` before
; invoking the registry and drops it with `.undefine` afterwards, so several
; members can share these macros in one translation unit.
.macro LANGRT_THUNK_SYMBOL label, function_id, name
      .word label-langrt_thunk_base
      .byte 0, NOBJ_SYM_GLOBAL, .strlen(name), name
.endmacro

.macro LANGRT_THUNK_RELOC label, function_id, name
      .byte 0, NOBJ_RELOC_ABS16
      .word .ident(.concat(.string(label), "_call"))-langrt_thunk_base
      .word langrt_thunk_rtcall_index, 0
.endmacro

.endif

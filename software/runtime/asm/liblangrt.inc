; =====================================================================
;  Nova NDK — liblangrt.inc
;
;  Shared language-runtime module contract. The first service family is
;  signed 16.16 arithmetic used by NovaPascal and available to future Nova
;  language runtimes and assembly applications.
; =====================================================================

.ifndef LIBLANGRT_INC_INCLUDED
LIBLANGRT_INC_INCLUDED = 1

.include "libabi.inc"

LANGRT_REAL_FROM_BYTE       = $00
LANGRT_REAL_FROM_WORD       = $01
LANGRT_REAL_NEGATE          = $02
LANGRT_REAL_ADD             = $03
LANGRT_REAL_SUB             = $04
LANGRT_REAL_CMP             = $05
LANGRT_REAL_MUL             = $06
LANGRT_REAL_MUL_WORD_LEFT   = $07
LANGRT_REAL_MUL_BYTE_LEFT   = $08
LANGRT_REAL_MUL_WORD_RIGHT  = $09
LANGRT_REAL_MUL_BYTE_RIGHT  = $0A
LANGRT_REAL_DIV             = $0B
LANGRT_REAL_DIV_WORD        = $0C
LANGRT_REAL_DIV_BYTE        = $0D
LANGRT_WORD_DIV_REAL        = $0E
LANGRT_BYTE_DIV_REAL        = $0F
LANGRT_ORDINAL_DIV_REAL     = $10
LANGRT_STR_REAL             = $11
LANGRT_STR_REAL_PRECISION   = $12
LANGRT_REAL_ABS             = $13
LANGRT_REAL_SQRT            = $14
LANGRT_REAL_SQR             = $15
LANGRT_REAL_SIN             = $16
LANGRT_REAL_COS             = $17
LANGRT_REAL_ARCTAN          = $18
LANGRT_REAL_LN              = $19
LANGRT_REAL_EXP             = $1A
LANGRT_REAL_TRUNC           = $1B
LANGRT_REAL_VAL             = $1C
LANGRT_REAL_ROUND           = $1D
LANGRT_LONG_FROM_BYTE       = $1E
LANGRT_LONG_FROM_UWORD      = $1F
LANGRT_LONG_FROM_INTEGER    = $20
LANGRT_LONG_NEGATE          = $21
LANGRT_LONG_ADD             = $22
LANGRT_LONG_SUB             = $23
LANGRT_LONG_CMP             = $24
LANGRT_LONG_MUL             = $25
LANGRT_LONG_DIV             = $26
LANGRT_LONG_MOD             = $27
LANGRT_STR_LONG             = $28
LANGRT_FN_COUNT             = $29

; LIB_SCRATCH byte zero belongs to the resident loader. The Pascal adapter uses
; two otherwise-free bytes while translating its native ABI; LANGRT itself
; returns real values through the canonical four-byte LIB_RESULT cell.
LANGRT_FN_SAVED             = LIB_SCRATCH+1
LANGRT_FLAGS_TMP            = LIB_SCRATCH+2

.endif

; langrt.s — shared compiler-language runtime module (MODULE_ID_LANGRT = $09).
; The initial service family is Nova's signed 16.16 implementation. Calls use
; the canonical LIB mailbox; the resident Pascal adapter translates its compact
; NVR/A/X convention before paging this module in.

      .include "libabi.inc"
      .include "libmod.inc"
      .include "liblangrt.inc"
      .include "math_copro.inc"

      .segment "CODE"
      lib_module_header MODULE_ID_LANGRT, LIB_ABI_VERSION, LANGRT_FN_COUNT

;@module LANGRT
;@version 1.0
;@brief Shared compiler-language services, beginning with signed 16.16 math.
;
; Real values are four-byte little-endian signed Q16.16 cells. Operands and
; durable results use the canonical mailbox so callers never depend on volatile
; loader scratch or module RAM.
;
;@fn LANGRT_REAL_FROM_BYTE
;@brief Convert an unsigned byte to signed Q16.16.
;@arg value u8 ARG0 byte0
;@ret q16 LIB_RESULT byte0..3
;@status LERR_OK
;
;@fn LANGRT_REAL_FROM_WORD
;@brief Convert a signed word to signed Q16.16.
;@arg value i16 ARG0 byte0..1
;@ret q16 LIB_RESULT byte0..3
;@status LERR_OK
;
;@fn LANGRT_REAL_NEGATE
;@brief Negate a signed Q16.16 value.
;@arg value ptr16 ARG0 byte0..1 points to Q16.16
;@ret q16 LIB_RESULT byte0..3
;@status LERR_OK
;
;@fn LANGRT_REAL_ADD
;@brief Add two signed Q16.16 values.
;@arg left q16 ARG1 byte0..3
;@arg right ptr16 ARG0 byte0..1 points to Q16.16
;@ret q16 LIB_RESULT byte0..3
;@status LERR_OK
;
;@fn LANGRT_REAL_SUB
;@brief Subtract the right signed Q16.16 value from the left value.
;@arg left q16 ARG1 byte0..3
;@arg right ptr16 ARG0 byte0..1 points to Q16.16
;@ret q16 LIB_RESULT byte0..3
;@status LERR_OK
;
;@fn LANGRT_REAL_CMP
;@brief Compare two signed Q16.16 values.
;@arg left q16 ARG1 byte0..3
;@arg right ptr16 ARG0 byte0..1 points to Q16.16
;@ret flags N/V/Z/C comparison flags in LIB_RESULT byte2
;@status LERR_OK
;
;@fn LANGRT_REAL_MUL
;@brief Multiply two signed Q16.16 values.
;@arg left q16 ARG1 byte0..3
;@arg right ptr16 ARG0 byte0..1 points to Q16.16
;@ret q16 LIB_RESULT byte0..3
;@status LERR_OK
;
;@fn LANGRT_REAL_MUL_WORD_LEFT
;@brief Multiply a signed word by a signed Q16.16 value.
;@arg left i16 ARG1 byte0..1
;@arg right ptr16 ARG0 byte0..1 points to Q16.16
;@ret q16 LIB_RESULT byte0..3
;@status LERR_OK
;
;@fn LANGRT_REAL_MUL_BYTE_LEFT
;@brief Multiply an unsigned byte by a signed Q16.16 value.
;@arg left u8 ARG1 byte0
;@arg right ptr16 ARG0 byte0..1 points to Q16.16
;@ret q16 LIB_RESULT byte0..3
;@status LERR_OK
;
;@fn LANGRT_REAL_MUL_WORD_RIGHT
;@brief Multiply a signed Q16.16 value by a signed word.
;@arg left q16 ARG1 byte0..3
;@arg right i16 ARG0 byte0..1
;@ret q16 LIB_RESULT byte0..3
;@status LERR_OK
;
;@fn LANGRT_REAL_MUL_BYTE_RIGHT
;@brief Multiply a signed Q16.16 value by an unsigned byte.
;@arg left q16 ARG1 byte0..3
;@arg right u8 ARG0 byte0
;@ret q16 LIB_RESULT byte0..3
;@status LERR_OK
;
;@fn LANGRT_REAL_DIV
;@brief Divide one signed Q16.16 value by another.
;@arg left q16 ARG1 byte0..3
;@arg right ptr16 ARG0 byte0..1 points to Q16.16
;@ret q16 LIB_RESULT byte0..3; divide by zero saturates
;@status LERR_OK
;
;@fn LANGRT_REAL_DIV_WORD
;@brief Divide a signed Q16.16 value by a signed word.
;@arg left q16 ARG1 byte0..3
;@arg right i16 ARG0 byte0..1
;@ret q16 LIB_RESULT byte0..3; divide by zero saturates
;@status LERR_OK
;
;@fn LANGRT_REAL_DIV_BYTE
;@brief Divide a signed Q16.16 value by an unsigned byte.
;@arg left q16 ARG1 byte0..3
;@arg right u8 ARG0 byte0
;@ret q16 LIB_RESULT byte0..3; divide by zero saturates
;@status LERR_OK
;
;@fn LANGRT_WORD_DIV_REAL
;@brief Divide a signed word by a signed Q16.16 value.
;@arg left i16 ARG1 byte0..1
;@arg right ptr16 ARG0 byte0..1 points to Q16.16
;@ret q16 LIB_RESULT byte0..3; divide by zero saturates
;@status LERR_OK
;
;@fn LANGRT_BYTE_DIV_REAL
;@brief Divide an unsigned byte by a signed Q16.16 value.
;@arg left u8 ARG1 byte0
;@arg right ptr16 ARG0 byte0..1 points to Q16.16
;@ret q16 LIB_RESULT byte0..3; divide by zero saturates
;@status LERR_OK
;
;@fn LANGRT_ORDINAL_DIV_REAL
;@brief Divide two ordinal values and return signed Q16.16.
;@arg left i16 ARG0 byte0..1
;@arg right i16 ARG1 byte0..1
;@ret q16 LIB_RESULT byte0..3; divide by zero saturates
;@status LERR_OK
;
;@fn LANGRT_STR_REAL
;@brief Format signed Q16.16 into a Turbo short string with default precision.
;@arg destination ptr16 ARG0 byte0..1
;@arg value ptr16 ARG1 byte0..1 points to Q16.16
;@arg width u8 ARG2 byte0
;@ret void
;@status LERR_OK
;
;@fn LANGRT_STR_REAL_PRECISION
;@brief Format signed Q16.16 into a Turbo short string.
;@arg destination ptr16 ARG0 byte0..1
;@arg value ptr16 ARG1 byte0..1 points to Q16.16
;@arg width u8 ARG2 byte0
;@arg precision u8 ARG3 byte0, clamped to six digits
;@ret void
;@status LERR_OK
;
;@fn LANGRT_REAL_ABS
;@brief Return the absolute value of signed Q16.16.
;@arg value ptr16 ARG0 byte0..1 points to Q16.16
;@ret q16 LIB_RESULT byte0..3
;@status LERR_OK
;
;@fn LANGRT_REAL_SQRT
;@brief Return the square root of signed Q16.16, or zero for negative input.
;@arg value ptr16 ARG0 byte0..1 points to Q16.16
;@ret q16 LIB_RESULT byte0..3
;@status LERR_OK
;
;@fn LANGRT_REAL_SQR
;@brief Square a signed Q16.16 value.
;@arg value ptr16 ARG0 byte0..1 points to Q16.16
;@ret q16 LIB_RESULT byte0..3
;@status LERR_OK
;
;@fn LANGRT_REAL_SIN
;@brief Return a compact signed Q16.16 sine approximation.
;@arg radians ptr16 ARG0 byte0..1 points to Q16.16
;@ret q16 LIB_RESULT byte0..3
;@status LERR_OK
;
;@fn LANGRT_REAL_COS
;@brief Return a compact signed Q16.16 cosine approximation.
;@arg radians ptr16 ARG0 byte0..1 points to Q16.16
;@ret q16 LIB_RESULT byte0..3
;@status LERR_OK
;
;@fn LANGRT_REAL_ARCTAN
;@brief Return a compact signed Q16.16 arctangent approximation.
;@arg value ptr16 ARG0 byte0..1 points to Q16.16
;@ret q16 LIB_RESULT byte0..3
;@status LERR_OK
;
;@fn LANGRT_REAL_LN
;@brief Return a compact signed Q16.16 natural-log approximation.
;@arg value ptr16 ARG0 byte0..1 points to Q16.16
;@ret q16 LIB_RESULT byte0..3
;@status LERR_OK
;
;@fn LANGRT_REAL_EXP
;@brief Return a compact signed Q16.16 exponential approximation.
;@arg value ptr16 ARG0 byte0..1 points to Q16.16
;@ret q16 LIB_RESULT byte0..3
;@status LERR_OK
;
;@fn LANGRT_REAL_TRUNC
;@brief Truncate signed Q16.16 toward zero.
;@arg value ptr16 ARG0 byte0..1 points to Q16.16
;@ret i16 LIB_RESULT byte0..1
;@status LERR_OK
;
;@fn LANGRT_REAL_VAL
;@brief Parse a Turbo short string into a signed word or Q16.16 destination.
;@arg source ptr16 ARG0 byte0..1
;@arg destination ptr16 ARG1 byte0..1
;@arg code ptr16 ARG2 byte0..1 receives a zero-based error position or zero
;@arg width u8 ARG3 byte0 is 2 for Integer or 4 for Real
;@ret void
;@status LERR_OK

dispatch:
      LDA   LIB_FN_ID
      CMP   #LANGRT_FN_COUNT
      BCS   langrt_bad_function
      LDA   LIB_FN_ID
      ASL
      TAX
      LDA   langrt_jtable+1,X
      PHA
      LDA   langrt_jtable,X
      PHA
      STZ   LIB_STATUS
      LDX   #3
@restore_native_arg:
      LDA   LIB_ARG1,X
      STA   NVR0L,X
      DEX
      BPL   @restore_native_arg
      LDA   LIB_ARG0
      LDX   LIB_ARG0+1
      RTS

langrt_bad_function:
      LDA   #LERR_NO_FN
      STA   LIB_STATUS
      STZ   LIB_RESULT
      STZ   LIB_RESULT+1
      RTS

langrt_jtable:
      .word real_from_byte-1
      .word real_from_word-1
      .word real_negate-1
      .word real_add-1
      .word real_subtract-1
      .word real_compare-1
      .word real_multiply-1
      .word real_mul_word_left-1
      .word real_mul_byte_left-1
      .word real_mul_word_right-1
      .word real_mul_byte_right-1
      .word real_divide-1
      .word real_div_word_right-1
      .word real_div_byte_right-1
      .word real_div_word_left-1
      .word real_div_byte_left-1
      .word real_ordinal_divide-1
      .word real_str-1
      .word real_str_precision-1
      .word real_abs-1
      .word real_sqrt-1
      .word real_sqr-1
      .word real_sin-1
      .word real_cos-1
      .word real_arctan-1
      .word real_ln-1
      .word real_exp-1
      .word real_trunc-1
      .word real_val-1

PASCAL_REAL_MODULE = 1
      .include "pascal_real_runtime.inc"

      .segment "VECTORS"
      .word   $C000, $C000, $C000

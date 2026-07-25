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
;
;@fn LANGRT_REAL_ROUND
;@brief Round signed Q16.16 to the nearest integer, with halves away from zero.
;@arg value ptr16 ARG0 byte0..1 points to Q16.16
;@ret i16 LIB_RESULT byte0..1
;@status LERR_OK
;
;@fn LANGRT_LONG_FROM_BYTE
;@brief Zero-extend an unsigned byte to signed 32-bit storage.
;@arg value u8 ARG0 byte0
;@ret i32 LIB_RESULT byte0..3
;@status LERR_OK
;
;@fn LANGRT_LONG_FROM_UWORD
;@brief Zero-extend an unsigned word to signed 32-bit storage.
;@arg value u16 ARG0 byte0..1
;@ret i32 LIB_RESULT byte0..3
;@status LERR_OK
;
;@fn LANGRT_LONG_FROM_INTEGER
;@brief Sign-extend a signed word to signed 32-bit storage.
;@arg value i16 ARG0 byte0..1
;@ret i32 LIB_RESULT byte0..3
;@status LERR_OK
;
;@fn LANGRT_LONG_NEGATE
;@brief Negate a signed 32-bit integer.
;@arg value ptr16 ARG0 byte0..1 points to i32
;@ret i32 LIB_RESULT byte0..3
;@status LERR_OK
;
;@fn LANGRT_LONG_ADD
;@brief Add two signed 32-bit integers.
;@arg left i32 ARG1 byte0..3
;@arg right ptr16 ARG0 byte0..1 points to i32
;@ret i32 LIB_RESULT byte0..3
;@status LERR_OK
;
;@fn LANGRT_LONG_SUB
;@brief Subtract the right signed 32-bit integer from the left.
;@arg left i32 ARG1 byte0..3
;@arg right ptr16 ARG0 byte0..1 points to i32
;@ret i32 LIB_RESULT byte0..3
;@status LERR_OK
;
;@fn LANGRT_LONG_CMP
;@brief Compare two signed 32-bit integers.
;@arg left i32 ARG1 byte0..3
;@arg right ptr16 ARG0 byte0..1 points to i32
;@ret flags N/V/Z/C comparison flags in LIB_RESULT byte2
;@status LERR_OK
;
;@fn LANGRT_LONG_MUL
;@brief Multiply two signed 32-bit integers, keeping the low 32 bits.
;@arg left i32 ARG1 byte0..3
;@arg right ptr16 ARG0 byte0..1 points to i32
;@ret i32 LIB_RESULT byte0..3
;@status LERR_OK
;
;@fn LANGRT_LONG_DIV
;@brief Divide signed 32-bit integers, truncating the quotient toward zero.
;@arg left i32 ARG1 byte0..3
;@arg right ptr16 ARG0 byte0..1 points to i32
;@ret i32 LIB_RESULT byte0..3
;@status LERR_DIV0 when the divisor is zero
;
;@fn LANGRT_LONG_MOD
;@brief Remainder of signed 32-bit division; the result takes the dividend sign.
;@arg left i32 ARG1 byte0..3
;@arg right ptr16 ARG0 byte0..1 points to i32
;@ret i32 LIB_RESULT byte0..3
;@status LERR_DIV0 when the divisor is zero
;
;@fn LANGRT_STR_LONG
;@brief Format a signed 32-bit integer into a Turbo short string.
;@arg destination ptr16 ARG0 byte0..1
;@arg value ptr16 ARG1 byte0..1 points to i32
;@arg width u8 ARG2 byte0 right-justifies to this minimum width when non-zero
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
      .word real_round-1
      .word long_from_byte-1
      .word long_from_uword-1
      .word long_from_integer-1
      .word real_negate-1
      .word real_add-1
      .word real_subtract-1
      .word real_compare-1
      .word long_multiply-1
      .word long_divide-1
      .word long_modulo-1
      .word long_str-1

PASCAL_REAL_MODULE = 1
      .include "pascal_real_runtime.inc"

long_from_byte:
      STA   LIB_RESULT
      STZ   LIB_RESULT+1
      STZ   LIB_RESULT+2
      STZ   LIB_RESULT+3
      LDA   #<LIB_RESULT
      LDX   #>LIB_RESULT
      RTS

long_from_uword:
      STA   LIB_RESULT
      STX   LIB_RESULT+1
      STZ   LIB_RESULT+2
      STZ   LIB_RESULT+3
      LDA   #<LIB_RESULT
      LDX   #>LIB_RESULT
      RTS

long_from_integer:
      STA   LIB_RESULT
      STX   LIB_RESULT+1
      TXA
      ASL
      LDA   #0
      SBC   #0
      EOR   #$FF
      STA   LIB_RESULT+2
      STA   LIB_RESULT+3
      LDA   #<LIB_RESULT
      LDX   #>LIB_RESULT
      RTS

; The low 32 bits of a product are the same whether the operands are read as
; signed or unsigned, so the halves multiply as unsigned values and the a1*b1
; term is dropped entirely: it only reaches bit 32 and above.
long_multiply:
      REAL_LOAD_RIGHT
      REAL_RESULT long_multiply_result
      MATHC_MUL16_U NVR0L, NVR0H, NVR2L, NVR2H, LIB_ARG0, LIB_ARG0+1, LIB_ARG0+2, LIB_ARG0+3
      LDY   #0
      LDA   LIB_ARG0
      STA   (NVR4L),Y
      INY
      LDA   LIB_ARG0+1
      STA   (NVR4L),Y
      INY
      LDA   LIB_ARG0+2
      STA   (NVR4L),Y
      INY
      LDA   LIB_ARG0+3
      STA   (NVR4L),Y
      ; Each cross term lands at bit 16, so only its low word survives in a
      ; 32-bit result and the cheaper signed multiply is sufficient.
      MATHC_MUL16 NVR0L, NVR0H, NVR3L, NVR3H, LIB_ARG0, LIB_ARG0+1, LIB_ARG0+2, LIB_ARG0+3
      JSR   long_add_cross_term
      MATHC_MUL16 NVR1L, NVR1H, NVR2L, NVR2H, LIB_ARG0, LIB_ARG0+1, LIB_ARG0+2, LIB_ARG0+3
      JSR   long_add_cross_term
      LDA   NVR4L
      LDX   NVR4H
      RTS

long_add_cross_term:
      LDY   #2
      CLC
      LDA   (NVR4L),Y
      ADC   LIB_ARG0
      STA   (NVR4L),Y
      INY
      LDA   (NVR4L),Y
      ADC   LIB_ARG0+1
      STA   (NVR4L),Y
      RTS

long_divide:
      JSR   long_divide_core
      BCS   long_divide_result
      LDA   NVR7L
      AND   #$80
      BEQ   long_divide_result
      REAL_NEGATE_VALUE NVR0L, NVR0H, NVR1L, NVR1H
long_divide_result:
      REAL_RESULT long_divide_result_buffer
      LDY   #0
      LDA   NVR0L
      STA   (NVR4L),Y
      INY
      LDA   NVR0H
      STA   (NVR4L),Y
      INY
      LDA   NVR1L
      STA   (NVR4L),Y
      INY
      LDA   NVR1H
      STA   (NVR4L),Y
      LDA   NVR4L
      LDX   NVR4H
      RTS

; Pascal's mod takes the sign of the dividend, matching a div that truncates
; toward zero.
long_modulo:
      JSR   long_divide_core
      BCS   long_modulo_result
      LDA   NVR7L
      AND   #$40
      BEQ   long_modulo_result
      REAL_NEGATE_VALUE NVR5L, NVR5H, NVR6L, NVR6H
long_modulo_result:
      REAL_RESULT long_modulo_result_buffer
      LDY   #0
      LDA   NVR5L
      STA   (NVR4L),Y
      INY
      LDA   NVR5H
      STA   (NVR4L),Y
      INY
      LDA   NVR6L
      STA   (NVR4L),Y
      INY
      LDA   NVR6H
      STA   (NVR4L),Y
      LDA   NVR4L
      LDX   NVR4H
      RTS

; Restoring shift/subtract division shared by div and mod. The dividend shifts
; left out of NVR0..NVR1 and the quotient fills the bits it vacates, so no
; separate quotient cell is needed.
;
; Entry: dividend in NVR0L..NVR1H, divisor pointer in A/X.
; Exit:  quotient in NVR0L..NVR1H, remainder in NVR5L..NVR6H, and NVR7L holding
;        the quotient sign in bit 7 and the dividend sign in bit 6. Carry is set
;        when the divisor was zero, in which case both results are zero.
long_divide_core:
      REAL_LOAD_RIGHT
      LDA   NVR1H
      EOR   NVR3H
      AND   #$80
      STA   NVR7L
      LDA   NVR1H
      AND   #$80
      LSR
      ORA   NVR7L
      STA   NVR7L
      LDA   NVR1H
      BPL   :+
      REAL_NEGATE_VALUE NVR0L, NVR0H, NVR1L, NVR1H
:     LDA   NVR3H
      BPL   :+
      REAL_NEGATE_VALUE NVR2L, NVR2H, NVR3L, NVR3H
:     STZ   NVR5L
      STZ   NVR5H
      STZ   NVR6L
      STZ   NVR6H
      LDA   NVR2L
      ORA   NVR2H
      ORA   NVR3L
      ORA   NVR3H
      BNE   @start
      STZ   NVR0L
      STZ   NVR0H
      STZ   NVR1L
      STZ   NVR1H
      LDA   #LERR_DIV0
      STA   LIB_STATUS
      SEC
      RTS
@start:
      LDA   #32
      STA   NVR7H
@bit:
      ASL   NVR0L
      ROL   NVR0H
      ROL   NVR1L
      ROL   NVR1H
      ROL   NVR5L
      ROL   NVR5H
      ROL   NVR6L
      ROL   NVR6H
      ; A shifted-out 33rd bit puts the remainder above any 32-bit divisor, so
      ; that subtraction always succeeds and must not be restored.
      BCS   @force
      JSR   long_divide_subtract
      BCS   @set_bit
      CLC
      LDA   NVR5L
      ADC   NVR2L
      STA   NVR5L
      LDA   NVR5H
      ADC   NVR2H
      STA   NVR5H
      LDA   NVR6L
      ADC   NVR3L
      STA   NVR6L
      LDA   NVR6H
      ADC   NVR3H
      STA   NVR6H
      BRA   @next
@force:
      JSR   long_divide_subtract
@set_bit:
      INC   NVR0L
@next:
      DEC   NVR7H
      BNE   @bit
      CLC
      RTS

long_divide_subtract:
      SEC
      LDA   NVR5L
      SBC   NVR2L
      STA   NVR5L
      LDA   NVR5H
      SBC   NVR2H
      STA   NVR5H
      LDA   NVR6L
      SBC   NVR3L
      STA   NVR6L
      LDA   NVR6H
      SBC   NVR3H
      STA   NVR6H
      RTS

; Turbo short string: length byte, optional blank padding, optional sign, then
; digits. Digits are produced least-significant first and held on the stack.
long_str:
      LDA   LIB_ARG1
      STA   NVR4L
      LDA   LIB_ARG1+1
      STA   NVR4H
      LDY   #0
      LDA   (NVR4L),Y
      STA   NVR2L
      INY
      LDA   (NVR4L),Y
      STA   NVR2H
      INY
      LDA   (NVR4L),Y
      STA   NVR3L
      INY
      LDA   (NVR4L),Y
      STA   NVR3H
      LDA   LIB_ARG0
      STA   NVR0L
      LDA   LIB_ARG0+1
      STA   NVR0H
      STZ   NVR7L
      LDA   NVR3H
      BPL   @magnitude
      INC   NVR7L
      REAL_NEGATE_VALUE NVR2L, NVR2H, NVR3L, NVR3H
@magnitude:
      STZ   NVR1L
@divide:
      LDA   #0
      LDX   #32
@bit:
      ASL   NVR2L
      ROL   NVR2H
      ROL   NVR3L
      ROL   NVR3H
      ROL
      CMP   #10
      BCC   :+
      SBC   #10
      INC   NVR2L
:     DEX
      BNE   @bit
      CLC
      ADC   #'0'
      PHA
      INC   NVR1L
      LDA   NVR2L
      ORA   NVR2H
      ORA   NVR3L
      ORA   NVR3H
      BNE   @divide
      LDA   NVR1L
      CLC
      ADC   NVR7L
      STA   NVR6L
      LDA   LIB_ARG2
      CMP   NVR6L
      BCC   @no_padding
      STA   NVR6H
      SEC
      SBC   NVR6L
      STA   NVR5L
      BRA   @emit
@no_padding:
      LDA   NVR6L
      STA   NVR6H
      STZ   NVR5L
@emit:
      LDY   #0
      LDA   NVR6H
      STA   (NVR0L),Y
@pad:
      LDA   NVR5L
      BEQ   @sign
      INY
      LDA   #' '
      STA   (NVR0L),Y
      DEC   NVR5L
      BRA   @pad
@sign:
      LDA   NVR7L
      BEQ   @digits
      INY
      LDA   #'-'
      STA   (NVR0L),Y
@digits:
      PLA
      INY
      STA   (NVR0L),Y
      DEC   NVR1L
      BNE   @digits
      LDA   #<LIB_RESULT
      LDX   #>LIB_RESULT
      RTS

      .segment "VECTORS"
      .word   $C000, $C000, $C000

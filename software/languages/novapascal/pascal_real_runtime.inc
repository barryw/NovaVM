; NovaPascal signed 16.16 real support. Binary operators receive the left
; value in NVR0L..NVR1H and the right value by pointer in A/X. Results use one
; shared buffer because NPC preserves every live left operand by value.

.ifdef PASCAL_REAL_MODULE
.macro REAL_RESULT call_site
      JSR   real_result
      STA   NVR4L
      STX   NVR4H
.endmacro

.macro REAL_CALL call_site
      JSR   real_multiply
.endmacro
.else
.macro REAL_RESULT call_site
      .byte $20
call_site: .word 0
      STA   NVR4L
      STX   NVR4H
.endmacro

.macro REAL_CALL call_site
      .byte $20
call_site: .word 0
.endmacro

.macro REAL_RELOC call_site, symbol_index
      .byte 0, NOBJ_RELOC_ABS16
      .word call_site-real_code, symbol_index, 0
.endmacro
.endif

.macro REAL_LOAD_RIGHT
      STA   NVR4L
      STX   NVR4H
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
.endmacro

.macro REAL_NEGATE_VALUE value0, value1, value2, value3
      SEC
      LDA   #0
      SBC   value0
      STA   value0
      LDA   #0
      SBC   value1
      STA   value1
      LDA   #0
      SBC   value2
      STA   value2
      LDA   #0
      SBC   value3
      STA   value3
.endmacro

; Convert an unsigned 16x16 product into LIB_ARG0. The unsigned-multiply
; correction itself is a canonical NDK primitive; this is only the LIB_ARG0
; binding the real routines expect.
.macro REAL_MUL_U16 a_lo, a_hi, b_lo, b_hi
      MATHC_MUL16_U a_lo, a_hi, b_lo, b_hi, LIB_ARG0, LIB_ARG0+1, LIB_ARG0+2, LIB_ARG0+3
.endmacro

.ifndef PASCAL_REAL_MODULE
      .word real_object_end-real_object
real_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 1, $FF
      .word 0
      .word 31
      .word real_symbols-real_object
      .word 19
      .word real_relocations-real_object
      .word 0

      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_EXEC | NOBJ_SEC_WRITE, 0, 4, 0
      .word real_code_end-real_code
      .word real_code_end-real_code
      .byte "CODE"
.endif
real_code:
real_from_byte:
      STA   NVR1L
      REAL_RESULT real_from_byte_result
      LDY   #0
      LDA   #0
      STA   (NVR4L),Y
      INY
      STA   (NVR4L),Y
      INY
      LDA   NVR1L
      STA   (NVR4L),Y
      INY
      LDA   #0
      STA   (NVR4L),Y
      LDA   NVR4L
      LDX   NVR4H
      RTS

real_from_word:
      STA   NVR1L
      STX   NVR1H
      REAL_RESULT real_from_word_result
      LDY   #0
      LDA   #0
      STA   (NVR4L),Y
      INY
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

real_negate:
      STA   NVR4L
      STX   NVR4H
      LDY   #0
      LDA   (NVR4L),Y
      STA   NVR0L
      INY
      LDA   (NVR4L),Y
      STA   NVR0H
      INY
      LDA   (NVR4L),Y
      STA   NVR1L
      INY
      LDA   (NVR4L),Y
      STA   NVR1H
      REAL_NEGATE_VALUE NVR0L, NVR0H, NVR1L, NVR1H
      REAL_RESULT real_negate_result
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

real_add:
      REAL_LOAD_RIGHT
      REAL_RESULT real_add_result
      LDY   #0
      CLC
      LDA   NVR0L
      ADC   NVR2L
      STA   (NVR4L),Y
      INY
      LDA   NVR0H
      ADC   NVR2H
      STA   (NVR4L),Y
      INY
      LDA   NVR1L
      ADC   NVR3L
      STA   (NVR4L),Y
      INY
      LDA   NVR1H
      ADC   NVR3H
      STA   (NVR4L),Y
      LDA   NVR4L
      LDX   NVR4H
      RTS

real_subtract:
      REAL_LOAD_RIGHT
      REAL_RESULT real_subtract_result
      LDY   #0
      SEC
      LDA   NVR0L
      SBC   NVR2L
      STA   (NVR4L),Y
      INY
      LDA   NVR0H
      SBC   NVR2H
      STA   (NVR4L),Y
      INY
      LDA   NVR1L
      SBC   NVR3L
      STA   (NVR4L),Y
      INY
      LDA   NVR1H
      SBC   NVR3H
      STA   (NVR4L),Y
      LDA   NVR4L
      LDX   NVR4H
      RTS

real_compare:
      REAL_LOAD_RIGHT
      LDA   NVR1H
      EOR   #$80
      STA   NVR4L
      LDA   NVR3H
      EOR   #$80
      STA   NVR4H
      LDA   NVR4L
      CMP   NVR4H
      BNE   @compared
      LDA   NVR1L
      CMP   NVR3L
      BNE   @compared
      LDA   NVR0H
      CMP   NVR2H
      BNE   @compared
      LDA   NVR0L
      CMP   NVR2L
@compared:
.ifdef PASCAL_REAL_MODULE
      PHP
      PLA
      STA   LIB_RESULT+2
.endif
      RTS

real_mul_word_left:
      PHA
      LDA   NVR0L
      STA   NVR1L
      LDA   NVR0H
      STA   NVR1H
      STZ   NVR0L
      STZ   NVR0H
      PLA
      BRA   real_multiply
real_mul_byte_left:
      PHA
      LDA   NVR0L
      STA   NVR1L
      STZ   NVR1H
      STZ   NVR0L
      STZ   NVR0H
      PLA
      BRA   real_multiply
real_mul_word_right:
      STZ   NVR2L
      STZ   NVR2H
      STA   NVR3L
      STX   NVR3H
      BRA   real_multiply_loaded
real_mul_byte_right:
      STZ   NVR2L
      STZ   NVR2H
      STA   NVR3L
      STZ   NVR3H
      BRA   real_multiply_loaded
real_multiply:
      REAL_LOAD_RIGHT
real_multiply_loaded:
      LDA   NVR1H
      EOR   NVR3H
      STA   NVR7L
      LDA   NVR1H
      BPL   :+
      REAL_NEGATE_VALUE NVR0L, NVR0H, NVR1L, NVR1H
:     LDA   NVR3H
      BPL   :+
      REAL_NEGATE_VALUE NVR2L, NVR2H, NVR3L, NVR3H
:     REAL_RESULT real_multiply_result
      REAL_MUL_U16 NVR0L, NVR0H, NVR2L, NVR2H
      LDY   #0
      LDA   LIB_ARG0+2
      STA   (NVR4L),Y
      INY
      LDA   LIB_ARG0+3
      STA   (NVR4L),Y
      INY
      LDA   #0
      STA   (NVR4L),Y
      INY
      STA   (NVR4L),Y
      REAL_MUL_U16 NVR0L, NVR0H, NVR3L, NVR3H
      LDY   #0
      CLC
      LDA   (NVR4L),Y
      ADC   LIB_ARG0
      STA   (NVR4L),Y
      INY
      LDA   (NVR4L),Y
      ADC   LIB_ARG0+1
      STA   (NVR4L),Y
      INY
      LDA   (NVR4L),Y
      ADC   LIB_ARG0+2
      STA   (NVR4L),Y
      INY
      LDA   (NVR4L),Y
      ADC   LIB_ARG0+3
      STA   (NVR4L),Y
      REAL_MUL_U16 NVR1L, NVR1H, NVR2L, NVR2H
      LDY   #0
      CLC
      LDA   (NVR4L),Y
      ADC   LIB_ARG0
      STA   (NVR4L),Y
      INY
      LDA   (NVR4L),Y
      ADC   LIB_ARG0+1
      STA   (NVR4L),Y
      INY
      LDA   (NVR4L),Y
      ADC   LIB_ARG0+2
      STA   (NVR4L),Y
      INY
      LDA   (NVR4L),Y
      ADC   LIB_ARG0+3
      STA   (NVR4L),Y
      REAL_MUL_U16 NVR1L, NVR1H, NVR3L, NVR3H
      LDY   #2
      CLC
      LDA   (NVR4L),Y
      ADC   LIB_ARG0
      STA   (NVR4L),Y
      INY
      LDA   (NVR4L),Y
      ADC   LIB_ARG0+1
      STA   (NVR4L),Y
      LDA   NVR7L
      BPL   @mul_done
      LDY   #0
      SEC
      LDA   #0
      SBC   (NVR4L),Y
      STA   (NVR4L),Y
      INY
      LDA   #0
      SBC   (NVR4L),Y
      STA   (NVR4L),Y
      INY
      LDA   #0
      SBC   (NVR4L),Y
      STA   (NVR4L),Y
      INY
      LDA   #0
      SBC   (NVR4L),Y
      STA   (NVR4L),Y
@mul_done:
      LDA   NVR4L
      LDX   NVR4H
      RTS

real_div_word_left:
      PHA
      LDA   NVR0L
      STA   NVR1L
      LDA   NVR0H
      STA   NVR1H
      STZ   NVR0L
      STZ   NVR0H
      PLA
      BRA   real_divide
real_div_byte_left:
      PHA
      LDA   NVR0L
      STA   NVR1L
      STZ   NVR1H
      STZ   NVR0L
      STZ   NVR0H
      PLA
      BRA   real_divide
real_div_word_right:
      STZ   NVR2L
      STZ   NVR2H
      STA   NVR3L
      STX   NVR3H
      BRA   real_divide_loaded
real_div_byte_right:
      STZ   NVR2L
      STZ   NVR2H
      STA   NVR3L
      STZ   NVR3H
      BRA   real_divide_loaded
real_ordinal_divide:
      STA   NVR6L
      STX   NVR6H
      STZ   NVR2L
      STZ   NVR2H
      LDA   NVR0L
      STA   NVR3L
      LDA   NVR0H
      STA   NVR3H
      STZ   NVR0L
      STZ   NVR0H
      LDA   NVR6L
      STA   NVR1L
      LDA   NVR6H
      STA   NVR1H
      BRA   real_divide_loaded
real_divide:
      REAL_LOAD_RIGHT
real_divide_loaded:
      LDA   NVR1H
      EOR   NVR3H
      STA   NVR7L
      LDA   NVR1H
      BPL   :+
      REAL_NEGATE_VALUE NVR0L, NVR0H, NVR1L, NVR1H
:     LDA   NVR3H
      BPL   :+
      REAL_NEGATE_VALUE NVR2L, NVR2H, NVR3L, NVR3H
:     LDA   NVR0L
      STA   LIB_ARG0
      LDA   NVR0H
      STA   LIB_ARG0+1
      LDA   NVR1L
      STA   LIB_ARG0+2
      LDA   NVR1H
      STA   LIB_ARG0+3
      LDA   NVR2L
      STA   LIB_ARG1
      LDA   NVR2H
      STA   LIB_ARG1+1
      LDA   NVR3L
      STA   LIB_ARG1+2
      LDA   NVR3H
      STA   LIB_ARG1+3
      ORA   LIB_ARG1+2
      ORA   LIB_ARG1+1
      ORA   LIB_ARG1
      BNE   real_div_nonzero
      REAL_RESULT real_divide_zero_result
      LDY   #0
      LDA   #$FF
      STA   (NVR4L),Y
      INY
      STA   (NVR4L),Y
      INY
      STA   (NVR4L),Y
      INY
      LDA   #$7F
      STA   (NVR4L),Y
      JMP   real_div_done
real_div_nonzero:
      STZ   LIB_ARG2
      STZ   LIB_ARG2+1
      STZ   LIB_ARG2+2
      STZ   LIB_ARG2+3
      LDX   #32
@integer_bit:
      ASL   LIB_ARG0
      ROL   LIB_ARG0+1
      ROL   LIB_ARG0+2
      ROL   LIB_ARG0+3
      ROL   LIB_ARG2
      ROL   LIB_ARG2+1
      ROL   LIB_ARG2+2
      ROL   LIB_ARG2+3
      BCS   @integer_subtract
      LDA   LIB_ARG2+3
      CMP   LIB_ARG1+3
      BNE   @integer_compare
      LDA   LIB_ARG2+2
      CMP   LIB_ARG1+2
      BNE   @integer_compare
      LDA   LIB_ARG2+1
      CMP   LIB_ARG1+1
      BNE   @integer_compare
      LDA   LIB_ARG2
      CMP   LIB_ARG1
@integer_compare:
      BCC   @integer_next
@integer_subtract:
      SEC
      LDA   LIB_ARG2
      SBC   LIB_ARG1
      STA   LIB_ARG2
      LDA   LIB_ARG2+1
      SBC   LIB_ARG1+1
      STA   LIB_ARG2+1
      LDA   LIB_ARG2+2
      SBC   LIB_ARG1+2
      STA   LIB_ARG2+2
      LDA   LIB_ARG2+3
      SBC   LIB_ARG1+3
      STA   LIB_ARG2+3
      INC   LIB_ARG0
@integer_next:
      DEX
      BNE   @integer_bit
      REAL_RESULT real_divide_result
      LDY   #0
      LDA   #0
      STA   (NVR4L),Y
      INY
      STA   (NVR4L),Y
      INY
      LDA   LIB_ARG0
      STA   (NVR4L),Y
      INY
      LDA   LIB_ARG0+1
      STA   (NVR4L),Y
      LDX   #16
@fraction_bit:
      LDY   #0
      LDA   (NVR4L),Y
      ASL
      STA   (NVR4L),Y
      INY
      LDA   (NVR4L),Y
      ROL
      STA   (NVR4L),Y
      ASL   LIB_ARG2
      ROL   LIB_ARG2+1
      ROL   LIB_ARG2+2
      ROL   LIB_ARG2+3
      BCS   @fraction_subtract
      LDA   LIB_ARG2+3
      CMP   LIB_ARG1+3
      BNE   @fraction_compare
      LDA   LIB_ARG2+2
      CMP   LIB_ARG1+2
      BNE   @fraction_compare
      LDA   LIB_ARG2+1
      CMP   LIB_ARG1+1
      BNE   @fraction_compare
      LDA   LIB_ARG2
      CMP   LIB_ARG1
@fraction_compare:
      BCC   @fraction_next
@fraction_subtract:
      SEC
      LDA   LIB_ARG2
      SBC   LIB_ARG1
      STA   LIB_ARG2
      LDA   LIB_ARG2+1
      SBC   LIB_ARG1+1
      STA   LIB_ARG2+1
      LDA   LIB_ARG2+2
      SBC   LIB_ARG1+2
      STA   LIB_ARG2+2
      LDA   LIB_ARG2+3
      SBC   LIB_ARG1+3
      STA   LIB_ARG2+3
      LDY   #0
      LDA   (NVR4L),Y
      INC
      STA   (NVR4L),Y
@fraction_next:
      DEX
      BNE   @fraction_bit
      LDA   NVR7L
      BPL   real_div_done
      LDY   #0
      SEC
      LDA   #0
      SBC   (NVR4L),Y
      STA   (NVR4L),Y
      INY
      LDA   #0
      SBC   (NVR4L),Y
      STA   (NVR4L),Y
      INY
      LDA   #0
      SBC   (NVR4L),Y
      STA   (NVR4L),Y
      INY
      LDA   #0
      SBC   (NVR4L),Y
      STA   (NVR4L),Y
real_div_done:
      LDA   NVR4L
      LDX   NVR4H
      RTS

real_abs:
      STA   NVR4L
      STX   NVR4H
      LDY   #0
      LDA   (NVR4L),Y
      STA   NVR0L
      INY
      LDA   (NVR4L),Y
      STA   NVR0H
      INY
      LDA   (NVR4L),Y
      STA   NVR1L
      INY
      LDA   (NVR4L),Y
      STA   NVR1H
      BPL   :+
      REAL_NEGATE_VALUE NVR0L, NVR0H, NVR1L, NVR1H
:     REAL_RESULT real_abs_result
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

real_sqrt:
      STA   NVR4L
      STX   NVR4H
      LDY   #3
      LDA   (NVR4L),Y
      BMI   @sqrt_zero
      LDY   #0
@sqrt_copy:
      LDA   (NVR4L),Y
      STA   LIB_ARG0,Y
      INY
      CPY   #4
      BNE   @sqrt_copy
      STZ   NVR7L
@sqrt_normalize:
      LDA   LIB_ARG0+2
      ORA   LIB_ARG0+3
      BEQ   @sqrt_hardware
      LSR   LIB_ARG0+3
      ROR   LIB_ARG0+2
      ROR   LIB_ARG0+1
      ROR   LIB_ARG0
      LSR   LIB_ARG0+3
      ROR   LIB_ARG0+2
      ROR   LIB_ARG0+1
      ROR   LIB_ARG0
      INC   NVR7L
      BRA   @sqrt_normalize
@sqrt_hardware:
      MATHC_SQRT_U16 LIB_ARG0, LIB_ARG0+1, NVR1L, NVR1H
@sqrt_scale:
      LDA   NVR7L
      BEQ   @sqrt_store
      ASL   NVR1L
      ROL   NVR1H
      DEC   NVR7L
      BRA   @sqrt_scale
@sqrt_zero:
      STZ   NVR1L
      STZ   NVR1H
@sqrt_store:
      REAL_RESULT real_sqrt_result
      LDY   #0
      LDA   #0
      STA   (NVR4L),Y
      INY
      LDA   NVR1L
      STA   (NVR4L),Y
      INY
      LDA   NVR1H
      STA   (NVR4L),Y
      INY
      LDA   #0
      STA   (NVR4L),Y
      LDA   NVR4L
      LDX   NVR4H
      RTS

real_sqr:
      STA   NVR4L
      STX   NVR4H
      LDY   #0
      LDA   (NVR4L),Y
      STA   NVR0L
      INY
      LDA   (NVR4L),Y
      STA   NVR0H
      INY
      LDA   (NVR4L),Y
      STA   NVR1L
      INY
      LDA   (NVR4L),Y
      STA   NVR1H
      LDA   NVR4L
      LDX   NVR4H
      REAL_CALL real_sqr_multiply
      RTS

real_sin:
      STZ   NVR7L
      BRA   real_trig
real_cos:
      LDY   #1
      STY   NVR7L
real_trig:
      STA   NVR4L
      STX   NVR4H
      LDY   #0
      LDA   (NVR4L),Y
      STA   MATH_DIV_N_LO
      INY
      LDA   (NVR4L),Y
      STA   MATH_DIV_N_1
      INY
      LDA   (NVR4L),Y
      STA   MATH_DIV_N_2
      INY
      LDA   (NVR4L),Y
      STA   MATH_DIV_N_HI
      LDA   #$48
      STA   MATH_DIV_D_LO
      LDA   #$06
      STA   MATH_DIV_D_HI
      LDA   MATH_RES0
      STA   MATH_SINCOS_ANGLE
      LDX   NVR7L
      LDA   MATH_RES0,X
      STA   NVR1L
      REAL_RESULT real_trig_result
      LDY   #0
      LDA   #0
      STA   (NVR4L),Y
      INY
      LDA   NVR1L
      ASL
      STA   (NVR4L),Y
      INY
      LDA   #0
      BIT   NVR1L
      BPL   :+
      LDA   #$FF
:     STA   (NVR4L),Y
      INY
      STA   (NVR4L),Y
      LDA   NVR4L
      LDX   NVR4H
      RTS

real_arctan:
      STA   NVR4L
      STX   NVR4H
      LDY   #1
      LDA   (NVR4L),Y
      STA   MATH_ATAN_DY_LO
      INY
      LDA   (NVR4L),Y
      STA   MATH_ATAN_DY_HI
      STZ   MATH_ATAN_DX_LO
      LDA   #1
      STA   MATH_ATAN_DX_HI
      LDA   MATH_RES0
      STA   MATH_MUL16_A_LO
      ASL
      LDA   #0
      ADC   #0
      SBC   #0
      STA   MATH_MUL16_A_HI
      LDA   #$48
      STA   MATH_MUL16_B_LO
      LDA   #$06
      STA   MATH_MUL16_B_HI
      LDA   MATH_RES0
      STA   NVR0L
      LDA   MATH_RES1
      STA   NVR0H
      LDA   MATH_RES2
      STA   NVR1L
      LDA   MATH_RES3
      STA   NVR1H
      REAL_RESULT real_arctan_result
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

; Compact range-reduced approximations. They are deterministic 16.16
; implementations and use the core multiplier rather than a private API.
real_ln:
      STA   NVR4L
      STX   NVR4H
      LDY   #0
      LDA   (NVR4L),Y
      STA   NVR0L
      INY
      LDA   (NVR4L),Y
      STA   NVR0H
      INY
      LDA   (NVR4L),Y
      STA   NVR1L
      INY
      LDA   (NVR4L),Y
      STA   NVR1H
      BPL   :+
      JMP   real_ln_zero
:     ORA   NVR1L
      ORA   NVR0H
      ORA   NVR0L
      BNE   :+
      JMP   real_ln_zero
:     STZ   NVR7L
@ln_high:
      LDA   NVR1H
      BNE   @ln_shift_right
      LDA   NVR1L
      CMP   #2
      BCC   @ln_low
@ln_shift_right:
      LSR   NVR1H
      ROR   NVR1L
      ROR   NVR0H
      ROR   NVR0L
      INC   NVR7L
      BRA   @ln_high
@ln_low:
      LDA   NVR1L
      BNE   @ln_normalized
      ASL   NVR0L
      ROL   NVR0H
      ROL   NVR1L
      ROL   NVR1H
      DEC   NVR7L
      BRA   @ln_low
@ln_normalized:
      DEC   NVR1L
      LDA   #$72
      STA   LIB_ARG3
      LDA   #$B1
      STA   LIB_ARG3+1
      STZ   LIB_ARG3+2
      STZ   LIB_ARG3+3
      LDA   NVR7L
      PHA
      LDA   #<LIB_ARG3
      LDX   #>LIB_ARG3
      REAL_CALL real_ln_multiply
      PLA
      STA   MATH_MUL16_A_LO
      ASL
      LDA   #0
      ADC   #0
      SBC   #0
      STA   MATH_MUL16_A_HI
      LDA   #$B9
      STA   MATH_MUL16_B_LO
      LDA   #$58
      STA   MATH_MUL16_B_HI
      ASL   MATH_RES0
      ROL   MATH_RES1
      ROL   MATH_RES2
      ROL   MATH_RES3
      STA   NVR4L
      STX   NVR4H
      LDY   #0
      CLC
      LDA   (NVR4L),Y
      ADC   MATH_RES0
      STA   (NVR4L),Y
      INY
      LDA   (NVR4L),Y
      ADC   MATH_RES1
      STA   (NVR4L),Y
      INY
      LDA   (NVR4L),Y
      ADC   MATH_RES2
      STA   (NVR4L),Y
      INY
      LDA   (NVR4L),Y
      ADC   MATH_RES3
      STA   (NVR4L),Y
      LDA   NVR4L
      LDX   NVR4H
      RTS
real_ln_zero:
      REAL_RESULT real_ln_zero_result
      LDY   #0
      LDA   #0
      STA   (NVR4L),Y
      INY
      STA   (NVR4L),Y
      INY
      LDA   #$00
      STA   (NVR4L),Y
      INY
      LDA   #$80
      STA   (NVR4L),Y
      LDA   NVR4L
      LDX   NVR4H
      RTS

real_exp:
      STA   NVR4L
      STX   NVR4H
      LDY   #0
@exp_copy:
      LDA   (NVR4L),Y
      STA   LIB_ARG3,Y
      INY
      CPY   #4
      BNE   @exp_copy
      LDA   LIB_ARG3
      STA   LIB_RESULT
      LDA   LIB_ARG3+1
      STA   LIB_RESULT+1
      LDA   LIB_ARG3+2
      CLC
      ADC   #1
      STA   LIB_RESULT+2
      LDA   LIB_ARG3+3
      ADC   #0
      STA   LIB_RESULT+3
      LDA   LIB_ARG3
      STA   NVR0L
      LDA   LIB_ARG3+1
      STA   NVR0H
      LDA   LIB_ARG3+2
      STA   NVR1L
      LDA   LIB_ARG3+3
      STA   NVR1H
      LDA   #<LIB_ARG3
      LDX   #>LIB_ARG3
      REAL_CALL real_exp_square
      STA   NVR4L
      STX   NVR4H
      LDY   #3
      LDA   (NVR4L),Y
      CMP   #$80
      ROR
      STA   NVR1H
      DEY
      LDA   (NVR4L),Y
      ROR
      STA   NVR1L
      DEY
      LDA   (NVR4L),Y
      ROR
      STA   NVR0H
      DEY
      LDA   (NVR4L),Y
      ROR
      STA   NVR0L
      CLC
      LDA   LIB_RESULT
      ADC   NVR0L
      STA   LIB_RESULT
      LDA   LIB_RESULT+1
      ADC   NVR0H
      STA   LIB_RESULT+1
      LDA   LIB_RESULT+2
      ADC   NVR1L
      STA   LIB_RESULT+2
      LDA   LIB_RESULT+3
      ADC   NVR1H
      STA   LIB_RESULT+3
      LDY   #0
      LDA   (NVR4L),Y
      STA   NVR0L
      INY
      LDA   (NVR4L),Y
      STA   NVR0H
      INY
      LDA   (NVR4L),Y
      STA   NVR1L
      INY
      LDA   (NVR4L),Y
      STA   NVR1H
      LDA   #<LIB_ARG3
      LDX   #>LIB_ARG3
      REAL_CALL real_exp_cube
      STA   NVR4L
      STX   NVR4H
      LDY   #0
      LDA   (NVR4L),Y
      STA   MATH_DIV_N_LO
      INY
      LDA   (NVR4L),Y
      STA   MATH_DIV_N_1
      INY
      LDA   (NVR4L),Y
      STA   MATH_DIV_N_2
      INY
      LDA   (NVR4L),Y
      STA   MATH_DIV_N_HI
      LDA   #6
      STA   MATH_DIV_D_LO
      STZ   MATH_DIV_D_HI
      LDY   #0
      CLC
      LDA   LIB_RESULT
      ADC   MATH_RES0
      STA   (NVR4L),Y
      INY
      LDA   LIB_RESULT+1
      ADC   MATH_RES1
      STA   (NVR4L),Y
      INY
      LDA   LIB_RESULT+2
      ADC   MATH_RES2
      STA   (NVR4L),Y
      INY
      LDA   LIB_RESULT+3
      ADC   MATH_RES3
      STA   (NVR4L),Y
      LDA   NVR4L
      LDX   NVR4H
      RTS

real_trunc:
      STA   NVR4L
      STX   NVR4H
      LDY   #2
      LDA   (NVR4L),Y
      STA   NVR0L
      INY
      LDA   (NVR4L),Y
      TAX
      BPL   @trunc_restore
      LDY   #0
      LDA   (NVR4L),Y
      INY
      ORA   (NVR4L),Y
      BEQ   @trunc_restore
      INC   NVR0L
      BNE   @trunc_restore
      INX
@trunc_restore:
      LDA   NVR0L
@trunc_done:
.ifdef PASCAL_REAL_MODULE
      STA   LIB_RESULT
      STX   LIB_RESULT+1
.endif
      RTS

; Turbo Pascal rounds halves away from zero. For a positive Q16.16 value,
; increment the integer half at $8000 or above. A negative value already
; contains its floor in the high word, so increment only above $8000.
real_round:
      STA   NVR4L
      STX   NVR4H
      LDY   #2
      LDA   (NVR4L),Y
      STA   NVR0L
      INY
      LDA   (NVR4L),Y
      TAX
      LDY   #1
      LDA   (NVR4L),Y
      CMP   #$80
      BCC   @round_done
      BNE   @round_increment
      DEY
      LDA   (NVR4L),Y
      BNE   @round_increment
      TXA
      BMI   @round_done
@round_increment:
      INC   NVR0L
      BNE   @round_done
      INX
@round_done:
      LDA   NVR0L
.ifdef PASCAL_REAL_MODULE
      STA   LIB_RESULT
      STX   LIB_RESULT+1
.endif
      RTS

; STR value:width[:precision], destination. The value is signed 16.16 and the
; destination is a Turbo short string.
real_str:
.ifdef PASCAL_REAL_MODULE
      LDA   LIB_ARG0
      STA   NVR0L
      LDA   LIB_ARG0+1
      STA   NVR0H
      LDA   LIB_ARG2
      STA   LIB_ARG0
      LDA   #2
      STA   LIB_ARG0+1
      LDA   LIB_ARG1
      STA   NVR4L
      LDA   LIB_ARG1+1
      STA   NVR4H
.else
      STA   NVR0L
      STX   NVR0H
      TSX
      LDA   $0103,X
      STA   LIB_ARG0
      LDA   #2
      STA   LIB_ARG0+1
      LDA   $0105,X
      STA   NVR4L
      LDA   $0106,X
      STA   NVR4H
.endif
      BRA   real_str_common
real_str_precision:
.ifdef PASCAL_REAL_MODULE
      LDA   LIB_ARG0
      STA   NVR0L
      LDA   LIB_ARG0+1
      STA   NVR0H
      LDA   LIB_ARG2
      STA   LIB_ARG0
      LDA   LIB_ARG3
.else
      STA   NVR0L
      STX   NVR0H
      TSX
      LDA   $0105,X
      STA   LIB_ARG0
      LDA   $0103,X
.endif
      CMP   #7
      BCC   :+
      LDA   #6
:     STA   LIB_ARG0+1
.ifdef PASCAL_REAL_MODULE
      LDA   LIB_ARG1
      STA   NVR4L
      LDA   LIB_ARG1+1
.else
      LDA   $0107,X
      STA   NVR4L
      LDA   $0108,X
.endif
      STA   NVR4H
real_str_common:
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
      STZ   LIB_ARG0+2
      BPL   @str_magnitude
      INC   LIB_ARG0+2
      REAL_NEGATE_VALUE NVR2L, NVR2H, NVR3L, NVR3H
@str_magnitude:
      LDA   NVR3L
      STA   NVR4L
      LDA   NVR3H
      STA   NVR4H
      STZ   NVR1L
@str_divide:
      LDA   #0
      LDX   #16
@str_bit:
      ASL   NVR4L
      ROL   NVR4H
      ROL
      CMP   #10
      BCC   :+
      SBC   #10
      INC   NVR4L
:     DEX
      BNE   @str_bit
      CLC
      ADC   #'0'
      PHA
      INC   NVR1L
      LDA   NVR4L
      ORA   NVR4H
      BNE   @str_divide
      LDA   NVR1L
      CLC
      ADC   LIB_ARG0+2
      LDX   LIB_ARG0+1
      BEQ   :+
      INX
      TXA
      CLC
      ADC   NVR1L
      ADC   LIB_ARG0+2
:     STA   NVR6L
      LDA   LIB_ARG0
      CMP   NVR6L
      BCC   @str_no_padding
      STA   NVR6H
      SEC
      SBC   NVR6L
      STA   NVR7L
      BRA   @str_length
@str_no_padding:
      LDA   NVR6L
      STA   NVR6H
      STZ   NVR7L
@str_length:
      LDY   #0
      LDA   NVR6H
      STA   (NVR0L),Y
@str_padding:
      LDA   NVR7L
      BEQ   @str_sign
      INY
      LDA   #' '
      STA   (NVR0L),Y
      DEC   NVR7L
      BRA   @str_padding
@str_sign:
      LDA   LIB_ARG0+2
      BEQ   @str_digits
      INY
      LDA   #'-'
      STA   (NVR0L),Y
@str_digits:
      PLA
      INY
      STA   (NVR0L),Y
      DEC   NVR1L
      BNE   @str_digits
      LDA   LIB_ARG0+1
      BEQ   @str_done
      INY
      LDA   #'.'
      STA   (NVR0L),Y
      LDX   LIB_ARG0+1
@str_fraction:
      STZ   NVR4L
      STZ   NVR4H
      STZ   NVR5L
      LDA   #10
      STA   NVR5H
@str_times_ten:
      CLC
      LDA   NVR4L
      ADC   NVR2L
      STA   NVR4L
      LDA   NVR4H
      ADC   NVR2H
      STA   NVR4H
      LDA   NVR5L
      ADC   #0
      STA   NVR5L
      DEC   NVR5H
      BNE   @str_times_ten
      LDA   NVR4L
      STA   NVR2L
      LDA   NVR4H
      STA   NVR2H
      LDA   NVR5L
      CLC
      ADC   #'0'
      INY
      STA   (NVR0L),Y
      DEX
      BNE   @str_fraction
@str_done:
      RTS

; VAL(string, var value, var code) receives one hidden byte after the three
; source arguments: 2 for Integer and 4 for Real.
real_val:
.ifdef PASCAL_REAL_MODULE
      LDA   LIB_ARG0
      STA   NVR0L
      LDA   LIB_ARG0+1
      STA   NVR0H
      LDA   LIB_ARG1
      STA   NVR1L
      LDA   LIB_ARG1+1
      STA   NVR1H
      LDA   LIB_ARG2
      STA   NVR2L
      LDA   LIB_ARG2+1
      STA   NVR2H
      LDA   LIB_ARG3
.else
      TSX
      LDA   $0108,X
      STA   NVR0L
      LDA   $0109,X
      STA   NVR0H
      LDA   $0106,X
      STA   NVR1L
      LDA   $0107,X
      STA   NVR1H
      LDA   $0104,X
      STA   NVR2L
      LDA   $0105,X
      STA   NVR2H
      LDA   $0103,X
.endif
      STA   LIB_ARG1
      STZ   LIB_ARG0
      STZ   LIB_ARG0+1
      STZ   LIB_ARG0+2
      STZ   LIB_ARG0+3
      STZ   LIB_ARG1+1
      STZ   LIB_ARG1+2
      STZ   LIB_ARG1+3
      LDY   #0
      LDA   (NVR0L),Y
      STA   NVR7H
      BNE   :+
      JMP   @val_error
:
      INY
      LDA   (NVR0L),Y
      CMP   #'+'
      BEQ   @val_sign_used
      CMP   #'-'
      BNE   @val_digit
      INC   LIB_ARG1+1
@val_sign_used:
      INY
@val_digit:
      STZ   NVR7L
@val_integer:
      CPY   NVR7H
      BCC   :+
      BEQ   :+
      JMP   @val_finish
:     LDA   (NVR0L),Y
      CMP   #'0'
      BCC   @val_decimal
      CMP   #'9'+1
      BCS   @val_decimal
      INC   NVR7L
      SEC
      SBC   #'0'
      STA   NVR6L
      LDA   LIB_ARG0+2
      STA   NVR3L
      LDA   LIB_ARG0+3
      STA   NVR3H
      ASL   LIB_ARG0+2
      ROL   LIB_ARG0+3
      ASL   NVR3L
      ROL   NVR3H
      ASL   NVR3L
      ROL   NVR3H
      ASL   NVR3L
      ROL   NVR3H
      CLC
      LDA   LIB_ARG0+2
      ADC   NVR3L
      ADC   NVR6L
      STA   LIB_ARG0+2
      LDA   LIB_ARG0+3
      ADC   NVR3H
      STA   LIB_ARG0+3
      INY
      BRA   @val_integer
@val_decimal:
      CMP   #'.'
      BEQ   :+
      JMP   @val_exponent
:
      INY
      STZ   NVR6H
      STZ   NVR3L
      STZ   NVR3H
@val_fraction:
      CPY   NVR7H
      BCC   :+
      BEQ   :+
      LDA   #0
      BRA   @val_fraction_commit
:     LDA   (NVR0L),Y
      CMP   #'0'
      BCC   @val_fraction_commit
      CMP   #'9'+1
      BCS   @val_fraction_commit
      INC   NVR7L
      SEC
      SBC   #'0'
      STA   NVR6L
      LDA   NVR6H
      CMP   #4
      BCS   @val_fraction_next
      INC   NVR6H
      LDA   NVR3L
      STA   NVR4L
      LDA   NVR3H
      STA   NVR4H
      ASL   NVR3L                    ; numerator * 2
      ROL   NVR3H
      ASL   NVR4L                    ; saved numerator * 8
      ROL   NVR4H
      ASL   NVR4L
      ROL   NVR4H
      ASL   NVR4L
      ROL   NVR4H
      CLC
      LDA   NVR3L
      ADC   NVR4L
      ADC   NVR6L
      STA   NVR3L
      LDA   NVR3H
      ADC   NVR4H
      STA   NVR3H
@val_fraction_next:
      INY
      BRA   @val_fraction
@val_fraction_commit:
      PHA                              ; preserve terminator for exponent parsing
      LDA   NVR6H
      BNE   :+
      JMP   @val_fraction_committed
:
      CMP   #1
      BNE   :+
      LDA   #10
      STA   NVR4L
      STZ   NVR4H
      BRA   @val_fraction_divide
:     CMP   #2
      BNE   :+
      LDA   #100
      STA   NVR4L
      STZ   NVR4H
      BRA   @val_fraction_divide
:     CMP   #3
      BNE   :+
      LDA   #<$03E8
      STA   NVR4L
      LDA   #>$03E8
      STA   NVR4H
      BRA   @val_fraction_divide
:     LDA   #<$2710
      STA   NVR4L
      LDA   #>$2710
      STA   NVR4H
@val_fraction_divide:
      LDA   NVR4H                    ; round (numerator * 65536) / denominator
      LSR
      STA   NVR5H
      LDA   NVR4L
      ROR
      STA   NVR5L
      STZ   NVR6L
      LDA   NVR3H                    ; signed divider tops out at $7FFF, so
      CMP   NVR5H                    ; split fractions >= 0.5 around $8000
      BCC   @val_fraction_numerator
      BNE   @val_fraction_high_half
      LDA   NVR3L
      CMP   NVR5L
      BCC   @val_fraction_numerator
@val_fraction_high_half:
      SEC
      LDA   NVR3L
      SBC   NVR5L
      STA   NVR3L
      LDA   NVR3H
      SBC   NVR5H
      STA   NVR3H
      LDA   #$80
      STA   NVR6L
@val_fraction_numerator:
      LDA   NVR5L
      STA   MATH_DIV_N_LO
      LDA   NVR5H
      STA   MATH_DIV_N_1
      LDA   NVR3L
      STA   MATH_DIV_N_2
      LDA   NVR3H
      STA   MATH_DIV_N_HI
      LDA   NVR4L
      STA   MATH_DIV_D_LO
      LDA   NVR4H
      STA   MATH_DIV_D_HI
      LDA   MATH_RES1
      ORA   NVR6L
      STA   NVR6L
      CLC
      LDA   LIB_ARG0
      ADC   MATH_RES0
      STA   LIB_ARG0
      LDA   LIB_ARG0+1
      ADC   NVR6L
      STA   LIB_ARG0+1
      LDA   LIB_ARG0+2
      ADC   #0
      STA   LIB_ARG0+2
      LDA   LIB_ARG0+3
      ADC   #0
      STA   LIB_ARG0+3
@val_fraction_committed:
      PLA
      BNE   @val_exponent
      JMP   @val_finish
@val_exponent:
      CMP   #'E'
      BEQ   @val_exp_start
      CMP   #'e'
      BEQ   @val_exp_start
      JMP   @val_finish
@val_exp_start:
      INY
      STZ   LIB_ARG1+2
      CPY   NVR7H
      BCC   :+
      BEQ   :+
      JMP   @val_error
:     LDA   (NVR0L),Y
      CMP   #'+'
      BEQ   @val_exp_sign
      CMP   #'-'
      BNE   @val_exp_digits
      INC   LIB_ARG1+3
@val_exp_sign:
      INY
@val_exp_digits:
      CPY   NVR7H
      BCC   :+
      BEQ   :+
      JMP   @val_apply_exp
:     LDA   (NVR0L),Y
      CMP   #'0'
      BCS   :+
      JMP   @val_error
:     CMP   #'9'+1
      BCC   :+
      JMP   @val_error
:     SEC
      SBC   #'0'
      STA   NVR6L
      LDA   LIB_ARG1+2
      ASL
      STA   NVR6H
      ASL
      ASL
      CLC
      ADC   NVR6H
      ADC   NVR6L
      STA   LIB_ARG1+2
      INY
      BRA   @val_exp_digits
@val_apply_exp:
      LDA   LIB_ARG1+2
      BEQ   @val_finish
      LDA   LIB_ARG1+3
      BNE   @val_div_ten
      LDA   LIB_ARG0
      STA   NVR3L
      LDA   LIB_ARG0+1
      STA   NVR3H
      LDA   LIB_ARG0+2
      STA   NVR4L
      LDA   LIB_ARG0+3
      STA   NVR4H
      ASL   LIB_ARG0
      ROL   LIB_ARG0+1
      ROL   LIB_ARG0+2
      ROL   LIB_ARG0+3
      ASL   NVR3L
      ROL   NVR3H
      ROL   NVR4L
      ROL   NVR4H
      ASL   NVR3L
      ROL   NVR3H
      ROL   NVR4L
      ROL   NVR4H
      CLC
      LDA   LIB_ARG0
      ADC   NVR3L
      STA   LIB_ARG0
      LDA   LIB_ARG0+1
      ADC   NVR3H
      STA   LIB_ARG0+1
      LDA   LIB_ARG0+2
      ADC   NVR4L
      STA   LIB_ARG0+2
      LDA   LIB_ARG0+3
      ADC   NVR4H
      STA   LIB_ARG0+3
      BRA   @val_exp_next
@val_div_ten:
      LDA   #0
      LDX   #32
@val_div_bit:
      ASL   LIB_ARG0
      ROL   LIB_ARG0+1
      ROL   LIB_ARG0+2
      ROL   LIB_ARG0+3
      ROL
      CMP   #10
      BCC   :+
      SBC   #10
      INC   LIB_ARG0
:     DEX
      BNE   @val_div_bit
@val_exp_next:
      DEC   LIB_ARG1+2
      BRA   @val_apply_exp
@val_finish:
      LDA   NVR7L
      BEQ   @val_error
      CPY   NVR7H
      BCC   @val_error
      LDA   LIB_ARG1+1
      BEQ   :+
      REAL_NEGATE_VALUE LIB_ARG0, LIB_ARG0+1, LIB_ARG0+2, LIB_ARG0+3
:     LDY   #0
      LDA   LIB_ARG1
      CMP   #4
      BNE   @val_word
      LDA   LIB_ARG0
      STA   (NVR1L),Y
      INY
      LDA   LIB_ARG0+1
      STA   (NVR1L),Y
      INY
@val_word:
      LDA   LIB_ARG0+2
      STA   (NVR1L),Y
      INY
      LDA   LIB_ARG0+3
      STA   (NVR1L),Y
      LDY   #0
      LDA   #0
      STA   (NVR2L),Y
      INY
      STA   (NVR2L),Y
      RTS
@val_error:
      TYA
      BNE   :+
      INC
:     LDY   #0
      STA   (NVR2L),Y
      INY
      LDA   #0
      STA   (NVR2L),Y
      RTS

real_result:
.ifdef PASCAL_REAL_MODULE
      LDA   #<LIB_RESULT
      LDX   #>LIB_RESULT
.else
      .byte $A9
real_result_lo: .byte 0
      .byte $A2
real_result_hi: .byte 0
.endif
      RTS

.ifndef PASCAL_REAL_MODULE
real_buffer:
      .byte 0,0,0,0
.endif
real_code_end:

.ifndef PASCAL_REAL_MODULE
REAL_SYM_RESULT = 29
REAL_SYM_BUFFER = 30

real_symbols:
      .word real_from_byte-real_code
      .byte 0, NOBJ_SYM_GLOBAL, 16, "P_REAL_FROM_BYTE"
      .word real_from_word-real_code
      .byte 0, NOBJ_SYM_GLOBAL, 16, "P_REAL_FROM_WORD"
      .word real_negate-real_code
      .byte 0, NOBJ_SYM_GLOBAL, 13, "P_REAL_NEGATE"
      .word real_add-real_code
      .byte 0, NOBJ_SYM_GLOBAL, 10, "P_REAL_ADD"
      .word real_subtract-real_code
      .byte 0, NOBJ_SYM_GLOBAL, 10, "P_REAL_SUB"
      .word real_compare-real_code
      .byte 0, NOBJ_SYM_GLOBAL, 10, "P_REAL_CMP"
      .word real_multiply-real_code
      .byte 0, NOBJ_SYM_GLOBAL, 10, "P_REAL_MUL"
      .word real_mul_word_left-real_code
      .byte 0, NOBJ_SYM_GLOBAL, 20, "P_REAL_MUL_WORD_LEFT"
      .word real_mul_byte_left-real_code
      .byte 0, NOBJ_SYM_GLOBAL, 20, "P_REAL_MUL_BYTE_LEFT"
      .word real_mul_word_right-real_code
      .byte 0, NOBJ_SYM_GLOBAL, 21, "P_REAL_MUL_WORD_RIGHT"
      .word real_mul_byte_right-real_code
      .byte 0, NOBJ_SYM_GLOBAL, 21, "P_REAL_MUL_BYTE_RIGHT"
      .word real_divide-real_code
      .byte 0, NOBJ_SYM_GLOBAL, 10, "P_REAL_DIV"
      .word real_div_word_right-real_code
      .byte 0, NOBJ_SYM_GLOBAL, 15, "P_REAL_DIV_WORD"
      .word real_div_byte_right-real_code
      .byte 0, NOBJ_SYM_GLOBAL, 15, "P_REAL_DIV_BYTE"
      .word real_div_word_left-real_code
      .byte 0, NOBJ_SYM_GLOBAL, 15, "P_WORD_DIV_REAL"
      .word real_div_byte_left-real_code
      .byte 0, NOBJ_SYM_GLOBAL, 15, "P_BYTE_DIV_REAL"
      .word real_ordinal_divide-real_code
      .byte 0, NOBJ_SYM_GLOBAL, 18, "P_ORDINAL_DIV_REAL"
      .word real_str-real_code
      .byte 0, NOBJ_SYM_GLOBAL, 10, "P_STR_REAL"
      .word real_str_precision-real_code
      .byte 0, NOBJ_SYM_GLOBAL, 20, "P_STR_REAL_PRECISION"
      .word real_abs-real_code
      .byte 0, NOBJ_SYM_GLOBAL, 3, "ABS"
      .word real_sqrt-real_code
      .byte 0, NOBJ_SYM_GLOBAL, 4, "SQRT"
      .word real_sqr-real_code
      .byte 0, NOBJ_SYM_GLOBAL, 3, "SQR"
      .word real_sin-real_code
      .byte 0, NOBJ_SYM_GLOBAL, 3, "SIN"
      .word real_cos-real_code
      .byte 0, NOBJ_SYM_GLOBAL, 3, "COS"
      .word real_arctan-real_code
      .byte 0, NOBJ_SYM_GLOBAL, 6, "ARCTAN"
      .word real_ln-real_code
      .byte 0, NOBJ_SYM_GLOBAL, 2, "LN"
      .word real_exp-real_code
      .byte 0, NOBJ_SYM_GLOBAL, 3, "EXP"
      .word real_trunc-real_code
      .byte 0, NOBJ_SYM_GLOBAL, 5, "TRUNC"
      .word real_val-real_code
      .byte 0, NOBJ_SYM_GLOBAL, 3, "VAL"
      .word real_result-real_code
      .byte 0, 0, 11, "REAL_RESULT"
      .word real_buffer-real_code
      .byte 0, 0, 11, "REAL_BUFFER"

real_relocations:
      REAL_RELOC real_from_byte_result, REAL_SYM_RESULT
      REAL_RELOC real_from_word_result, REAL_SYM_RESULT
      REAL_RELOC real_negate_result, REAL_SYM_RESULT
      REAL_RELOC real_add_result, REAL_SYM_RESULT
      REAL_RELOC real_subtract_result, REAL_SYM_RESULT
      REAL_RELOC real_multiply_result, REAL_SYM_RESULT
      REAL_RELOC real_divide_zero_result, REAL_SYM_RESULT
      REAL_RELOC real_divide_result, REAL_SYM_RESULT
      REAL_RELOC real_abs_result, REAL_SYM_RESULT
      REAL_RELOC real_sqrt_result, REAL_SYM_RESULT
      REAL_RELOC real_sqr_multiply, 6
      REAL_RELOC real_trig_result, REAL_SYM_RESULT
      REAL_RELOC real_arctan_result, REAL_SYM_RESULT
      REAL_RELOC real_ln_multiply, 6
      REAL_RELOC real_ln_zero_result, REAL_SYM_RESULT
      REAL_RELOC real_exp_square, 6
      REAL_RELOC real_exp_cube, 6
      .byte 0, NOBJ_RELOC_LO8
      .word real_result_lo-real_code, REAL_SYM_BUFFER, 0
      .byte 0, NOBJ_RELOC_HI8
      .word real_result_hi-real_code, REAL_SYM_BUFFER, 0
real_object_end:
.endif

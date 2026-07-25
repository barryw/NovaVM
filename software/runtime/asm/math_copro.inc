; =====================================================================
;  Nova NDK — math_copro.inc
;
;  Math coprocessor macros: signed 16x16 and Q8.8 multiply, 32/16 divide,
;  integer square root, sin/cos, atan2, distance approximations, and 2D
;  vector ops. Keep call sites at this operation level so the register
;  protocol can change later without forcing every caller to know the
;  hardware layout.
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

.ifndef MATH_COPRO_INC_INCLUDED
MATH_COPRO_INC_INCLUDED = 1

.include "nova.inc"

; Signed 16x16 multiply.
; Result is written low-to-high into dst0..dst3.
.macro MATHC_MUL16 a_lo, a_hi, b_lo, b_hi, dst0, dst1, dst2, dst3
        lda a_lo
        sta MATH_MUL16_A_LO
        lda a_hi
        sta MATH_MUL16_A_HI
        lda b_lo
        sta MATH_MUL16_B_LO
        lda b_hi
        sta MATH_MUL16_B_HI
        lda MATH_RES0
        sta dst0
        lda MATH_RES1
        sta dst1
        lda MATH_RES2
        sta dst2
        lda MATH_RES3
        sta dst3
.endmacro

; Unsigned 16x16 multiply.
; The coprocessor multiplier is signed, so an operand with bit 15 set is short
; by the other operand shifted up 16; each such operand contributes one
; correction word to the high half.
.macro MATHC_MUL16_U a_lo, a_hi, b_lo, b_hi, dst0, dst1, dst2, dst3
        MATHC_MUL16 a_lo, a_hi, b_lo, b_hi, dst0, dst1, dst2, dst3
        lda a_hi
        bpl :+
        clc
        lda dst2
        adc b_lo
        sta dst2
        lda dst3
        adc b_hi
        sta dst3
:       lda b_hi
        bpl :+
        clc
        lda dst2
        adc a_lo
        sta dst2
        lda dst3
        adc a_hi
        sta dst3
:
.endmacro

; Signed Q8.8 multiply with a saturated Q8.8 result.
.macro MATHC_MULFX a_lo, a_hi, b_lo, b_hi, dst_lo, dst_hi
        lda a_lo
        sta MATH_MULFX_A_LO
        lda a_hi
        sta MATH_MULFX_A_HI
        lda b_lo
        sta MATH_MULFX_B_LO
        lda b_hi
        sta MATH_MULFX_B_HI
        lda MATH_RES0
        sta dst_lo
        lda MATH_RES1
        sta dst_hi
.endmacro

; Signed 32 / signed 16 division.
; Result quotient is copied to quot_lo/quot_hi, remainder to rem_lo/rem_hi.
.macro MATHC_DIV_S32_16 n0, n1, n2, n3, d_lo, d_hi, quot_lo, quot_hi, rem_lo, rem_hi
        lda n0
        sta MATH_DIV_N_LO
        lda n1
        sta MATH_DIV_N_1
        lda n2
        sta MATH_DIV_N_2
        lda n3
        sta MATH_DIV_N_HI
        lda d_lo
        sta MATH_DIV_D_LO
        lda d_hi
        sta MATH_DIV_D_HI
        lda MATH_RES0
        sta quot_lo
        lda MATH_RES1
        sta quot_hi
        lda MATH_RES2
        sta rem_lo
        lda MATH_RES3
        sta rem_hi
.endmacro

; Combined sine/cosine for an unsigned 8-bit angle.
; Results are signed 1.7 fixed point bytes: -128..+127.
.macro MATHC_SINCOS angle, sin_dst, cos_dst
        lda angle
        sta MATH_SINCOS_ANGLE
        lda MATH_RES0
        sta sin_dst
        lda MATH_RES1
        sta cos_dst
.endmacro

; Unsigned 16-bit integer square root. The result is floor(sqrt(value)).
.macro MATHC_SQRT_U16 value_lo, value_hi, dst_lo, dst_hi
        lda value_lo
        sta MATH_SQRT_LO
        lda value_hi
        sta MATH_SQRT_HI
        lda MATH_RES0
        sta dst_lo
        lda MATH_RES1
        sta dst_hi
.endmacro

; ATAN2 for signed 16-bit dy/dx.
; angle_dst receives u8 angle, 0..255 = full circle.
; hypot_lo/hypot_hi receive approximate u16 distance.
.macro MATHC_ATAN2 dy_lo, dy_hi, dx_lo, dx_hi, angle_dst, hypot_lo, hypot_hi
        lda dy_lo
        sta MATH_ATAN_DY_LO
        lda dy_hi
        sta MATH_ATAN_DY_HI
        lda dx_lo
        sta MATH_ATAN_DX_LO
        lda dx_hi
        sta MATH_ATAN_DX_HI
        lda MATH_RES0
        sta angle_dst
        lda MATH_RES1
        sta hypot_lo
        lda MATH_RES2
        sta hypot_hi
.endmacro

; Approximate distance for signed 16-bit dx/dy:
; max(abs(dx), abs(dy)) + 3/8 * min(abs(dx), abs(dy)).
.macro MATHC_DIST_APPROX_S16 dx_lo, dx_hi, dy_lo, dy_hi, dst_lo, dst_hi
        lda dx_lo
        sta MATH_DIST_DX_LO
        lda dx_hi
        sta MATH_DIST_DX_HI
        lda dy_lo
        sta MATH_DIST_DY_LO
        lda dy_hi
        sta MATH_DIST_DY_HI
        lda MATH_RES0
        sta dst_lo
        lda MATH_RES1
        sta dst_hi
.endmacro

; Approximate distance for unsigned 8-bit positive dx/dy.
.macro MATHC_DIST_APPROX_U8 dx, dy, dst_lo, dst_hi
        lda dx
        sta MATH_DIST_DX_LO
        stz MATH_DIST_DX_HI
        lda dy
        sta MATH_DIST_DY_LO
        stz MATH_DIST_DY_HI
        lda MATH_RES0
        sta dst_lo
        lda MATH_RES1
        sta dst_hi
.endmacro

; Signed 16-bit dot product: ax*bx + ay*by -> signed 32-bit result.
.macro MATHC_VEC_DOT_S16 ax_lo, ax_hi, ay_lo, ay_hi, bx_lo, bx_hi, by_lo, by_hi, dst0, dst1, dst2, dst3
        lda ax_lo
        sta MATH_VEC_AX_LO
        lda ax_hi
        sta MATH_VEC_AX_HI
        lda ay_lo
        sta MATH_VEC_AY_LO
        lda ay_hi
        sta MATH_VEC_AY_HI
        lda bx_lo
        sta MATH_VEC_BX_LO
        lda bx_hi
        sta MATH_VEC_BX_HI
        lda by_lo
        sta MATH_VEC_BY_LO
        lda by_hi
        sta MATH_VEC_BY_HI
        lda #MATH_VEC_OP_DOT_S16
        sta MATH_VEC_OP
        lda MATH_RES0
        sta dst0
        lda MATH_RES1
        sta dst1
        lda MATH_RES2
        sta dst2
        lda MATH_RES3
        sta dst3
.endmacro

; Signed Q8.8 dot product: ax*bx + ay*by -> saturated signed Q8.8.
.macro MATHC_VEC_DOTFX ax_lo, ax_hi, ay_lo, ay_hi, bx_lo, bx_hi, by_lo, by_hi, dst_lo, dst_hi
        lda ax_lo
        sta MATH_VEC_AX_LO
        lda ax_hi
        sta MATH_VEC_AX_HI
        lda ay_lo
        sta MATH_VEC_AY_LO
        lda ay_hi
        sta MATH_VEC_AY_HI
        lda bx_lo
        sta MATH_VEC_BX_LO
        lda bx_hi
        sta MATH_VEC_BX_HI
        lda by_lo
        sta MATH_VEC_BY_LO
        lda by_hi
        sta MATH_VEC_BY_HI
        lda #MATH_VEC_OP_DOT_FX
        sta MATH_VEC_OP
        lda MATH_RES0
        sta dst_lo
        lda MATH_RES1
        sta dst_hi
.endmacro

; Signed 16-bit 2D cross product: ax*by - ay*bx -> signed 32-bit.
.macro MATHC_VEC_CROSS_S16 ax_lo, ax_hi, ay_lo, ay_hi, bx_lo, bx_hi, by_lo, by_hi, dst0, dst1, dst2, dst3
        lda ax_lo
        sta MATH_VEC_AX_LO
        lda ax_hi
        sta MATH_VEC_AX_HI
        lda ay_lo
        sta MATH_VEC_AY_LO
        lda ay_hi
        sta MATH_VEC_AY_HI
        lda bx_lo
        sta MATH_VEC_BX_LO
        lda bx_hi
        sta MATH_VEC_BX_HI
        lda by_lo
        sta MATH_VEC_BY_LO
        lda by_hi
        sta MATH_VEC_BY_HI
        lda #MATH_VEC_OP_CROSS_S16
        sta MATH_VEC_OP
        lda MATH_RES0
        sta dst0
        lda MATH_RES1
        sta dst1
        lda MATH_RES2
        sta dst2
        lda MATH_RES3
        sta dst3
.endmacro

; Squared length for a signed 16-bit vector: ax*ax + ay*ay -> u32.
.macro MATHC_VEC_LEN2 ax_lo, ax_hi, ay_lo, ay_hi, dst0, dst1, dst2, dst3
        lda ax_lo
        sta MATH_VEC_AX_LO
        lda ax_hi
        sta MATH_VEC_AX_HI
        lda ay_lo
        sta MATH_VEC_AY_LO
        lda ay_hi
        sta MATH_VEC_AY_HI
        lda #MATH_VEC_OP_LEN2
        sta MATH_VEC_OP
        lda MATH_RES0
        sta dst0
        lda MATH_RES1
        sta dst1
        lda MATH_RES2
        sta dst2
        lda MATH_RES3
        sta dst3
.endmacro

; Scale signed Q8.8 vector A by signed Q8.8 scalar.
; Result x -> dst_x_lo/dst_x_hi, result y -> dst_y_lo/dst_y_hi.
.macro MATHC_VEC_SCALEFX ax_lo, ax_hi, ay_lo, ay_hi, scalar_lo, scalar_hi, dst_x_lo, dst_x_hi, dst_y_lo, dst_y_hi
        lda ax_lo
        sta MATH_VEC_AX_LO
        lda ax_hi
        sta MATH_VEC_AX_HI
        lda ay_lo
        sta MATH_VEC_AY_LO
        lda ay_hi
        sta MATH_VEC_AY_HI
        lda scalar_lo
        sta MATH_VEC_S_LO
        lda scalar_hi
        sta MATH_VEC_S_HI
        lda #MATH_VEC_OP_SCALE_FX
        sta MATH_VEC_OP
        lda MATH_RES0
        sta dst_x_lo
        lda MATH_RES1
        sta dst_x_hi
        lda MATH_RES2
        sta dst_y_lo
        lda MATH_RES3
        sta dst_y_hi
.endmacro

.macro MATHC_RNG_READ dst
        lda MATH_RNG
        sta dst
.endmacro

.macro MATHC_READ_CAPS0 dst
        lda MATH_CAPS0
        sta dst
.endmacro

.macro MATHC_READ_STATUS dst
        lda MATH_STATUS
        sta dst
.endmacro

.macro MATHC_READ_VERSION dst
        lda MATH_VERSION
        sta dst
.endmacro

.endif

; =====================================================================
;  Nova NDK — math_fixed8.inc
;
;  Fixed-point math helper macros. FIX8 = signed 8.8 (high byte = integer,
;  low byte = fraction); FIX16_8 = unsigned 16.8 (16-bit integer + fraction
;  byte). Kept as macros so indexed game-object math stays cheap; a future
;  math coprocessor can replace the bodies without changing game logic.
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

.ifndef MATH_FIXED8_INC_INCLUDED
MATH_FIXED8_INC_INCLUDED = 1

FIX8_ONE_LO = $00
FIX8_ONE_HI = $01

; Add a signed 8.8 constant to an indexed signed 8.8 value.
; X = object index.
.macro FIX8_ADD_CONST_X value_lo, value_hi, const_lo, const_hi
        lda value_lo,x
        clc
        adc #const_lo
        sta value_lo,x
        lda value_hi,x
        adc #const_hi
        sta value_hi,x
.endmacro

; Add an indexed signed 8.8 velocity to an indexed unsigned 16.8 position.
; X = object index.
.macro FIX16_8_ADD_FIX8_X pos_frac, pos_lo, pos_hi, vel_frac, vel_int
        .local positive, done
        lda pos_frac,x
        clc
        adc vel_frac,x
        sta pos_frac,x
        lda pos_lo,x
        adc vel_int,x
        sta pos_lo,x
        lda vel_int,x
        bpl positive
        lda pos_hi,x
        adc #$FF
        bra done
positive:
        lda pos_hi,x
        adc #$00
done:
        sta pos_hi,x
.endmacro

; Copy the absolute value of an indexed signed 8.8 value into a 16-bit temp.
; X = object index.
.macro FIX8_ABS_TO_X value_lo, value_hi, dst_lo, dst_hi
        .local positive, done
        lda value_lo,x
        sta dst_lo
        lda value_hi,x
        sta dst_hi
        bpl positive
        lda dst_lo
        eor #$FF
        clc
        adc #$01
        sta dst_lo
        lda dst_hi
        eor #$FF
        adc #$00
        sta dst_hi
positive:
done:
.endmacro

; Two's-complement negate a signed 8.8 value in memory.
.macro FIX8_NEGATE_MEM value_lo, value_hi
        lda value_lo
        eor #$FF
        clc
        adc #$01
        sta value_lo
        lda value_hi
        eor #$FF
        adc #$00
        sta value_hi
.endmacro

; Negate an indexed signed/overflowed 16.8 position in place.
; X = object index.
.macro FIX16_8_NEGATE_X pos_frac, pos_lo, pos_hi
        lda pos_frac,x
        eor #$FF
        clc
        adc #$01
        sta pos_frac,x
        lda pos_lo,x
        eor #$FF
        adc #$00
        sta pos_lo,x
        lda pos_hi,x
        eor #$FF
        adc #$00
        sta pos_hi,x
.endmacro

; Reflect an indexed 16.8 position across a max boundary:
; pos = (2 * max) - pos. The doubled max is supplied as integer bytes.
; X = object index.
.macro FIX16_8_REFLECT_MAX_X pos_frac, pos_lo, pos_hi, max2_lo, max2_hi
        lda #$00
        sec
        sbc pos_frac,x
        sta pos_frac,x
        lda #max2_lo
        sbc pos_lo,x
        sta pos_lo,x
        lda #max2_hi
        sbc pos_hi,x
        sta pos_hi,x
.endmacro

; Multiply a positive 8.8 magnitude by 3/4.
; Uses logical shifts, so src must be non-negative.
.macro FIX8_MUL_MAG_3_4 src_lo, src_hi, dst_lo, dst_hi, tmp_lo, tmp_hi
        lda src_hi
        lsr
        sta dst_hi
        lda src_lo
        ror
        sta dst_lo

        lda dst_hi
        lsr
        sta tmp_hi
        lda dst_lo
        ror
        sta tmp_lo

        lda dst_lo
        clc
        adc tmp_lo
        sta dst_lo
        lda dst_hi
        adc tmp_hi
        sta dst_hi
.endmacro

.endif

; MATAN2 assembly sprite demo. Build with:
; ca65 --cpu 65c02 -I docs/programs/math_demos -I runtime/asm -o /tmp/math_matan2_asm.o docs/programs/math_demos/math_matan2_asm.s
; ld65 -C docs/programs/diag9000_4k.cfg -o /tmp/math_matan2_asm.bin /tmp/math_matan2_asm.o

.setcpu "65c02"
.include "math_demo_common.inc"

update_demo:
        ldx #$00
@loop:
        stx SPR_INDEX
        lda TICK
        asl
        asl
        sta TMP_A
        lda SPR_INDEX
        asl
        asl
        asl
        asl
        asl
        clc
        adc TMP_A
        sta TMP_A
        MATHC_SINCOS TMP_A, VALUE0, VALUE1
        lda VALUE0             ; dy low
        sta TMP_A
        stz TMP_B
        bpl :+
        dec TMP_B
:
        lda VALUE1             ; dx low
        sta TMP_C
        stz TMP_D
        bpl :+
        dec TMP_D
:
        MATHC_ATAN2 TMP_A, TMP_B, TMP_C, TMP_D, VALUE2, VALUE0, VALUE1
        MATHC_SINCOS VALUE2, VALUE0, VALUE1
        lda #152
        sta TMP_X
        stz TMP_XH
        lda VALUE1
        bmi @neg_x
        clc
        adc TMP_X
        sta TMP_X
        bcc @y_pos
        inc TMP_XH
        bra @y_pos
@neg_x:
        clc
        adc TMP_X
        sta TMP_X
        lda TMP_XH
        adc #$FF
        sta TMP_XH
@y_pos:
        lda VALUE0
        cmp #$80              ; arithmetic shift right for signed 1.7 sine
        ror
        clc
        adc #96
        sta TMP_Y
@set_sprite:
        ldx SPR_INDEX
        jsr set_sprite_xy16
        ldx SPR_INDEX
        inx
        cpx #SPR_COUNT
        beq @done
        jmp @loop
@done:
        rts

demo_title:
        .byte "MATAN2(dy,dx) - assembly orbit",0

; MSCALX assembly sprite demo. Build with:
; ca65 --cpu 65c02 -I docs/programs/math_demos -I ehbasic/lib -I ehbasic -o /tmp/math_mscalx_asm.o docs/programs/math_demos/math_mscalx_asm.s
; ld65 -C docs/programs/diag9000_4k.cfg -o /tmp/math_mscalx_asm.bin /tmp/math_mscalx_asm.o

.setcpu "65c02"
.include "math_demo_common.inc"

update_demo:
        ldx #$00
@loop:
        stx SPR_INDEX
        lda #$00              ; vector x = 124.0
        sta TMP_A
        lda #$7C
        sta TMP_B
        lda #$00              ; vector y = 0.0
        sta TMP_C
        lda #$00
        sta TMP_D
        lda TICK
        asl
        asl
        sta VALUE0
        lda SPR_INDEX
        asl
        asl
        asl
        asl
        asl
        clc
        adc VALUE0
        sta VALUE0
        MATHC_SINCOS VALUE0, VALUE0, VALUE1
        lda VALUE0
        asl
        sta VALUE0
        lda #$00
        bcc :+
        lda #$FF
:
        sta VALUE1
        MATHC_VEC_SCALEFX TMP_A, TMP_B, TMP_C, TMP_D, VALUE0, VALUE1, VALUE0, VALUE1, VALUE2, VALUE3
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
        lda SPR_INDEX
        asl
        asl
        asl
        asl
        sta TMP_Y
        lda SPR_INDEX
        asl
        asl
        clc
        adc TMP_Y
        clc
        adc #36
        sta TMP_Y
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
        .byte "MSCALX(x,y,s) - assembly scaled X",0

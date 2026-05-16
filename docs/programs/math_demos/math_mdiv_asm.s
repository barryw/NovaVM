; MDIV assembly sprite demo. Build with:
; ca65 --cpu 65c02 -I docs/programs/math_demos -I runtime/asm -I ehbasic -o /tmp/math_mdiv_asm.o docs/programs/math_demos/math_mdiv_asm.s
; ld65 -C docs/programs/diag9000_4k.cfg -o /tmp/math_mdiv_asm.bin /tmp/math_mdiv_asm.o

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
        lda VALUE0
        sta TMP_A
        stz TMP_B
        bpl :+
        dec TMP_B
:
        stz TMP_C
        stz TMP_D
        lda #$02
        sta VALUE2
        stz VALUE3
        MATHC_DIV_S32_16 TMP_A, TMP_B, TMP_C, TMP_D, VALUE2, VALUE3, VALUE0, VALUE1, VALUE2, VALUE3
        ldx SPR_INDEX
        lda x_lo_table,x
        sta TMP_X
        lda x_hi_table,x
        sta TMP_XH
        lda #96
        clc
        adc VALUE0
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

x_lo_table:
        .byte <20,<60,<100,<140,<180,<220,<260,<300
x_hi_table:
        .byte >20,>60,>100,>140,>180,>220,>260,>300

demo_title:
        .byte "MDIV(n,d) - assembly quotient lanes",0

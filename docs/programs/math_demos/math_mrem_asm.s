; MREM assembly sprite demo. Build with:
; ca65 --cpu 65c02 -I docs/programs/math_demos -I ehbasic/lib -I ehbasic -o /tmp/math_mrem_asm.o docs/programs/math_demos/math_mrem_asm.s
; ld65 -C docs/programs/diag9000_4k.cfg -o /tmp/math_mrem_asm.bin /tmp/math_mrem_asm.o

.setcpu "65c02"
.include "math_demo_common.inc"

update_demo:
        ldx #$00
@loop:
        stx SPR_INDEX
        lda TICK
        stz TMP_B
        asl
        rol TMP_B
        asl
        rol TMP_B
        sta TMP_A
        ldx SPR_INDEX
        clc
        adc offset_lo_table,x
        sta TMP_A
        lda TMP_B
        adc offset_hi_table,x
        sta TMP_B
        stz TMP_C
        stz TMP_D
        lda #72
        sta VALUE2
        stz VALUE3
        MATHC_DIV_S32_16 TMP_A, TMP_B, TMP_C, TMP_D, VALUE2, VALUE3, VALUE0, VALUE1, VALUE2, VALUE3
        lda VALUE2
        asl
        clc
        adc #32
        sta TMP_Y
        ldx SPR_INDEX
        lda x_lo_table,x
        sta TMP_X
        lda x_hi_table,x
        sta TMP_XH
        ldx SPR_INDEX
        jsr set_sprite_xy16
        ldx SPR_INDEX
        inx
        cpx #SPR_COUNT
        beq @done
        jmp @loop
@done:
        rts

offset_lo_table:
        .byte <0,<37,<74,<111,<148,<185,<222,<259
offset_hi_table:
        .byte >0,>37,>74,>111,>148,>185,>222,>259
x_lo_table:
        .byte <20,<60,<100,<140,<180,<220,<260,<300
x_hi_table:
        .byte >20,>60,>100,>140,>180,>220,>260,>300

demo_title:
        .byte "MREM(n,d) - assembly remainder steps",0

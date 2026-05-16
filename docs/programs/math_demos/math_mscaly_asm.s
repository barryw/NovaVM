; MSCALY assembly sprite demo. Build with:
; ca65 --cpu 65c02 -I docs/programs/math_demos -I runtime/asm -I ehbasic -o /tmp/math_mscaly_asm.o docs/programs/math_demos/math_mscaly_asm.s
; ld65 -C docs/programs/diag9000_4k.cfg -o /tmp/math_mscaly_asm.bin /tmp/math_mscaly_asm.o

.setcpu "65c02"
.include "math_demo_common.inc"

update_demo:
        ldx #$00
@loop:
        stx SPR_INDEX
        lda #$00              ; vector x = 0.0
        sta TMP_A
        lda #$00
        sta TMP_B
        lda #$00              ; vector y = 64.0
        sta TMP_C
        lda #$40
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
        lda #96
        clc
        adc VALUE3
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

x_lo_table:
        .byte <20,<60,<100,<140,<180,<220,<260,<300
x_hi_table:
        .byte >20,>60,>100,>140,>180,>220,>260,>300

demo_title:
        .byte "MSCALY(x,y,s) - assembly scaled Y",0

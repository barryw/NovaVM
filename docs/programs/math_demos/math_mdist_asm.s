; MDIST assembly sprite demo. Build with:
; ca65 --cpu 65c02 -I docs/programs/math_demos -I ehbasic/lib -I ehbasic -o /tmp/math_mdist_asm.o docs/programs/math_demos/math_mdist_asm.s
; ld65 -C docs/programs/diag9000_4k.cfg -o /tmp/math_mdist_asm.bin /tmp/math_mdist_asm.o

.setcpu "65c02"
.include "math_demo_common.inc"

update_demo:
        ldx #$00
@loop:
        stx SPR_INDEX
        lda TICK
        asl
        asl
        sta TMP_C
        lda SPR_INDEX
        asl
        asl
        asl
        asl
        asl
        clc
        adc TMP_C
        sta TMP_A
        MATHC_SINCOS TMP_A, VALUE0, VALUE1
        lda VALUE0
        sta TMP_C
        stz TMP_D
        bpl :+
        dec TMP_D
:
        ldx SPR_INDEX
        lda dx_table,x
        sta TMP_A
        stz TMP_B
        bpl :+
        dec TMP_B
:
        MATHC_DIST_APPROX_S16 TMP_A, TMP_B, TMP_C, TMP_D, VALUE0, VALUE1
        lda VALUE0
        lsr
        clc
        adc VALUE0
        sta TMP_X
        stz TMP_XH
        bcc :+
        inc TMP_XH
:
        lda TMP_X
        clc
        adc #16
        sta TMP_X
        bcc :+
        inc TMP_XH
:
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

dx_table:
        .byte <-126,<-90,<-54,<-18,<18,<54,<90,<126

demo_title:
        .byte "MDIST(dx,dy) - assembly distance meter",0

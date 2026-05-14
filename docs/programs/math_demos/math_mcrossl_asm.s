; MCROSSL assembly sprite demo. Build with:
; ca65 --cpu 65c02 -I docs/programs/math_demos -I ehbasic/lib -I ehbasic -o /tmp/math_mcrossl_asm.o docs/programs/math_demos/math_mcrossl_asm.s
; ld65 -C docs/programs/diag9000_4k.cfg -o /tmp/math_mcrossl_asm.bin /tmp/math_mcrossl_asm.o

.setcpu "65c02"
.include "math_demo_common.inc"

update_demo:
        ldx #$00
@loop:
        stx SPR_INDEX
        lda TICK
        sta TMP_A
        lda SPR_INDEX
        asl
        asl
        asl
        clc
        adc TMP_A
        and #$7F
        sta TMP_A              ; ax low
        stz TMP_B
        stz TMP_C              ; ay low
        stz TMP_D
        stz VALUE0             ; bx low
        stz VALUE1
        lda #$08
        sta VALUE2             ; by low
        stz VALUE3
        MATHC_VEC_CROSS_S16 TMP_A, TMP_B, TMP_C, TMP_D, VALUE0, VALUE1, VALUE2, VALUE3, VALUE0, VALUE1, VALUE2, VALUE3
        lsr VALUE1             ; low word / 4
        ror VALUE0
        lsr VALUE1
        ror VALUE0
        lda VALUE0
        clc
        adc #16
        sta TMP_X
        stz TMP_XH
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

demo_title:
        .byte "MCROSSL(ax,ay,bx,by) - assembly cross low",0

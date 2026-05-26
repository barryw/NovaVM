; MLEN2 assembly sprite demo. Build with:
; ca65 --cpu 65c02 -I docs/programs/math_demos -I runtime/asm -o /tmp/math_mlen2_asm.o docs/programs/math_demos/math_mlen2_asm.s
; ld65 -C docs/programs/diag9000_4k.cfg -o /tmp/math_mlen2_asm.bin /tmp/math_mlen2_asm.o

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
        lda VALUE1
        sta TMP_A
        stz TMP_B
        bpl :+
        dec TMP_B
:
        ldx SPR_INDEX
        lda y_component_table,x
        sta TMP_C
        stz TMP_D
        MATHC_VEC_LEN2 TMP_A, TMP_B, TMP_C, TMP_D, VALUE0, VALUE1, VALUE2, VALUE3
        lda VALUE1             ; x = 20 + (len2 >> 7) + (len2 >> 8)
        asl
        sta TMP_X
        stz TMP_XH
        lda VALUE0
        asl
        lda #$00
        adc TMP_X
        sta TMP_X
        lda TMP_XH
        adc #$00
        sta TMP_XH
        lda VALUE1
        clc
        adc TMP_X
        sta TMP_X
        bcc :+
        inc TMP_XH
:
        lda TMP_X
        clc
        adc #20
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

y_component_table:
        .byte 0,12,24,36,48,60,72,84

demo_title:
        .byte "MLEN2(x,y) - assembly length squared",0

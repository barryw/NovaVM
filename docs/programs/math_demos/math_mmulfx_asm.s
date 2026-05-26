; MMULFX assembly sprite demo. Build with:
; ca65 --cpu 65c02 -I docs/programs/math_demos -I runtime/asm -o /tmp/math_mmulfx_asm.o docs/programs/math_demos/math_mmulfx_asm.s
; ld65 -C docs/programs/diag9000_4k.cfg -o /tmp/math_mmulfx_asm.bin /tmp/math_mmulfx_asm.o

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
        lda VALUE0            ; signed 1.7 sine -> signed Q8.8
        asl
        sta TMP_A
        lda #$00
        bcc :+
        lda #$FF
:
        sta TMP_B
        stz TMP_C
        lda #$7C              ; 124.0 in Q8.8
        sta TMP_D
        MATHC_MULFX TMP_A, TMP_B, TMP_C, TMP_D, VALUE0, VALUE1
        lda #152
        sta TMP_X
        stz TMP_XH
        lda VALUE1
        bmi @neg_offset
        clc
        adc TMP_X
        sta TMP_X
        bcc @x_done
        inc TMP_XH
        bra @x_done
@neg_offset:
        clc
        adc TMP_X
        sta TMP_X
        lda TMP_XH
        adc #$FF
        sta TMP_XH
@x_done:
        lda SPR_INDEX
        asl
        asl
        asl
        asl
        sta TMP_Y
        lda SPR_INDEX
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
        .byte "MMULFX(a,b) - assembly Q8.8 multiply",0

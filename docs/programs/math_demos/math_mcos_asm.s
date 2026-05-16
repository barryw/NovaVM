; MCOS assembly sprite demo. Build with:
; ca65 --cpu 65c02 -I docs/programs/math_demos -I runtime/asm -I ehbasic -o /tmp/math_mcos_asm.o docs/programs/math_demos/math_mcos_asm.s
; ld65 -C docs/programs/diag9000_4k.cfg -o /tmp/math_mcos_asm.bin /tmp/math_mcos_asm.o

.setcpu "65c02"
.include "math_demo_common.inc"

update_demo:
        ldx #$00
@loop:
        stx SPR_INDEX
        txa
        asl
        asl
        asl
        asl
        asl
        clc
        adc TICK
        sta TMP_A
        MATHC_SINCOS TMP_A, VALUE0, VALUE1
        lda #152
        sta TMP_X
        stz TMP_XH
        lda VALUE1
        bmi @neg_cos
        clc
        adc TMP_X
        sta TMP_X
        bcc :+
        inc TMP_XH
:
        lda VALUE1
        lsr
        lsr
        lsr
        clc
        adc TMP_X
        sta TMP_X
        bcc @x_done
        inc TMP_XH
        bra @x_done
@neg_cos:
        clc
        adc TMP_X
        sta TMP_X
        lda TMP_XH
        adc #$FF
        sta TMP_XH
        sec
        lda #$00
        sbc VALUE1
        lsr
        lsr
        lsr
        sta TMP_B
        sec
        lda TMP_X
        sbc TMP_B
        sta TMP_X
        lda TMP_XH
        sbc #$00
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
        .byte "MCOS(angle) - assembly cosine sweep",0

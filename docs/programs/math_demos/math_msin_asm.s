; MSIN assembly sprite demo. Build with:
; ca65 --cpu 65c02 -I docs/programs/math_demos -I runtime/asm -I ehbasic -o /tmp/math_msin_asm.o docs/programs/math_demos/math_msin_asm.s
; ld65 -C docs/programs/diag9000_4k.cfg -o /tmp/math_msin_asm.bin /tmp/math_msin_asm.o

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
        lda VALUE0
        cmp #$80
        ror
        clc
        adc #BASE_Y
        sta TMP_Y
        lda SPR_INDEX
        asl
        asl
        asl
        asl
        asl
        clc
        adc #BASE_X
        sta TMP_X
        ldx SPR_INDEX
        jsr set_sprite_xy
        ldx SPR_INDEX
        inx
        cpx #SPR_COUNT
        bne @loop
        rts

demo_title:
        .byte "MSIN(angle) - assembly sine wave",0


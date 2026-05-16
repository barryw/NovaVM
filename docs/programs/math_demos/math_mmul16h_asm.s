; MMUL16H assembly sprite demo. Build with:
; ca65 --cpu 65c02 -I docs/programs/math_demos -I runtime/asm -I ehbasic -o /tmp/math_mmul16h_asm.o docs/programs/math_demos/math_mmul16h_asm.s
; ld65 -C docs/programs/diag9000_4k.cfg -o /tmp/math_mmul16h_asm.bin /tmp/math_mmul16h_asm.o

.setcpu "65c02"
.include "math_demo_common.inc"

update_demo:
        ldx #$00
@loop:
        stx SPR_INDEX
        lda TICK
        sta TMP_C
        lda SPR_INDEX
        asl
        asl
        asl
        asl
        clc
        adc TMP_C
        sta TMP_C              ; phase byte, 0..255
        and #$01
        beq :+
        lda #$80
:
        sta TMP_A              ; a low = phase * 128
        lda TMP_C
        lsr
        sta TMP_B              ; a high = phase * 128
        stz TMP_C
        lda #$02
        sta TMP_D              ; b = 512, product high word = phase
        MATHC_MUL16 TMP_A, TMP_B, TMP_C, TMP_D, VALUE0, VALUE1, VALUE2, VALUE3
        lda VALUE2
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
        .byte "MMUL16H(a,b) - assembly product high word",0

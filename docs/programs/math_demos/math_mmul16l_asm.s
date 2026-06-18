; MMUL16L assembly sprite demo. Build with:
; ca65 --cpu w65c02 -I docs/programs/math_demos -I runtime/asm -o /tmp/math_mmul16l_asm.o docs/programs/math_demos/math_mmul16l_asm.s
; ld65 -C docs/programs/diag9000_4k.cfg -o /tmp/math_mmul16l_asm.bin /tmp/math_mmul16l_asm.o

.setcpu "w65c02"
.include "math_demo_common.inc"

update_demo:
        ldx #$00
@loop:
        stx SPR_INDEX
        lda TICK
        clc
        adc SPR_INDEX
        sta TMP_A
        stz TMP_B
        lda #$03
        sta TMP_C
        stz TMP_D
        MATHC_MUL16 TMP_A, TMP_B, TMP_C, TMP_D, VALUE0, VALUE1, VALUE2, VALUE3
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
        .byte "MMUL16L(a,b) - assembly product low word",0

; MDOTS16H assembly sprite demo. Build with:
; ca65 --cpu w65c02 -I docs/programs/math_demos -I runtime/asm -o /tmp/math_mdots16h_asm.o docs/programs/math_demos/math_mdots16h_asm.s
; ld65 -C docs/programs/diag9000_4k.cfg -o /tmp/math_mdots16h_asm.bin /tmp/math_mdots16h_asm.o

.setcpu "w65c02"
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
        sta TMP_A              ; ax = phase * 128
        lda TMP_C
        lsr
        sta TMP_B
        stz TMP_C
        stz TMP_D
        stz VALUE0
        lda #$02
        sta VALUE1             ; bx = 512
        stz VALUE2
        stz VALUE3
        MATHC_VEC_DOT_S16 TMP_A, TMP_B, TMP_C, TMP_D, VALUE0, VALUE1, VALUE2, VALUE3, VALUE0, VALUE1, VALUE2, VALUE3
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
        .byte "MDOTS16H(ax,ay,bx,by) - assembly dot high",0

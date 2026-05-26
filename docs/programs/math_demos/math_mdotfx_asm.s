; MDOTFX assembly sprite demo. Build with:
; ca65 --cpu 65c02 -I docs/programs/math_demos -I runtime/asm -o /tmp/math_mdotfx_asm.o docs/programs/math_demos/math_mdotfx_asm.s
; ld65 -C docs/programs/diag9000_4k.cfg -o /tmp/math_mdotfx_asm.bin /tmp/math_mdotfx_asm.o

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
        lda VALUE1             ; ax = cos(angle) * 2 -> Q8.8
        asl
        sta TMP_A
        lda #$00
        bcc :+
        lda #$FF
:
        sta TMP_B
        lda VALUE0             ; ay = sin(angle) * 2 -> Q8.8
        asl
        sta TMP_C
        lda #$00
        bcc :+
        lda #$FF
:
        sta TMP_D
        stz VALUE0             ; bx = 1.0, by = 0.0
        lda #$01
        sta VALUE1
        stz VALUE2
        stz VALUE3
        MATHC_VEC_DOTFX TMP_A, TMP_B, TMP_C, TMP_D, VALUE0, VALUE1, VALUE2, VALUE3, VALUE0, VALUE1
        lda VALUE0             ; signed Q8.8 result / 2 -> pixel offset
        sta TMP_A
        lda VALUE1
        cmp #$80
        ror
        sta TMP_B
        lda TMP_A
        ror
        sta TMP_A
        lda #152
        sta TMP_X
        stz TMP_XH
        lda TMP_A
        bmi @neg_x
        clc
        adc TMP_X
        sta TMP_X
        bcc @y_pos
        inc TMP_XH
        bra @y_pos
@neg_x:
        lda #152
        clc
        adc TMP_A
        sta TMP_X
        lda TMP_XH
        adc TMP_B
        sta TMP_XH
@y_pos:
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
        .byte "MDOTFX(ax,ay,bx,by) - assembly dot Q8.8",0

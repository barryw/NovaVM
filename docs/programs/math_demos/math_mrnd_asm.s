; MRND assembly sprite demo. Build with:
; ca65 --cpu 65c02 -I docs/programs/math_demos -I ehbasic/lib -I ehbasic -o /tmp/math_mrnd_asm.o docs/programs/math_demos/math_mrnd_asm.s
; ld65 -C docs/programs/diag9000_4k.cfg -o /tmp/math_mrnd_asm.bin /tmp/math_mrnd_asm.o

.setcpu "65c02"
.include "math_demo_common.inc"

update_demo:
        ldx #$00
@loop:
        stx SPR_INDEX
        MATHC_RNG_READ VALUE0
        lda VALUE0
        sta TMP_X
        lsr
        lsr
        lsr
        clc
        adc TMP_X
        sta TMP_X
        stz TMP_XH
        bcc :+
        inc TMP_XH
:
        lda TMP_X
        clc
        adc #8
        sta TMP_X
        bcc :+
        inc TMP_XH
:
        MATHC_RNG_READ VALUE0
        lda VALUE0
        sta TMP_Y
        lsr
        lsr
        lsr
        sta TMP_A
        lda TMP_Y
        lsr
        clc
        adc TMP_A
        clc
        adc #20
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
        .byte "MRND - assembly random sprite positions",0

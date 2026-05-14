; sprite_sincos_asm.s
;
; Hardware sprite demo driven by the math coprocessor SINCOS operation.
;
; Build:
;   ca65 --cpu 65c02 -I ehbasic/lib -I ehbasic -o /tmp/sprite_sincos_asm.o docs/programs/sprite_sincos_asm.s
;   ld65 -C docs/programs/diag9000_4k.cfg -o /tmp/sprite_sincos_asm.bin /tmp/sprite_sincos_asm.o
;
; Load at $9000 and run with: SYS $9000

.setcpu "65c02"

.include "nova.inc"
.include "math_copro.inc"

SPR_COUNT       = 16
SPR_TRANS       = 6
CENTER_X        = 152
CENTER_Y        = 92

PTR             = NVR0L
BASE_ANGLE      = NVR1L
ANGLE_WORK      = NVR1H
SIN_VALUE       = NVR2L
COS_VALUE       = NVR2H
TEMP            = NVR3L
ROW_IDX         = NVR3H

.macro STORE_SIGNED_POS center, low_addr, high_addr
        .local positive, done
        sta TEMP
        lda #<center
        clc
        adc TEMP
        sta low_addr,y
        lda TEMP
        bpl positive
        lda #>center
        adc #$FF
        bra done
positive:
        lda #>center
        adc #$00
done:
        sta high_addr,y
.endmacro

.segment "CODE"

start:
        lda #$00
        sta VGC_CURSEN
        sta VGC_BGCOL
        lda #$03
        sta VGC_MODE

        jsr wait_cmd
        lda #VCMD_COPPERDIS
        sta VGC_CMD
        jsr wait_cmd
        lda #VCMD_COPPERCLR
        sta VGC_CMD

        jsr define_shape
        jsr init_sprites
        stz BASE_ANGLE

main_loop:
        jsr wait_frame
        inc BASE_ANGLE

        ldx #$00
sincos_sprite_loop:
        lda BASE_ANGLE
        clc
        adc angle_offsets,x
        sta ANGLE_WORK

        MATHC_SINCOS ANGLE_WORK, SIN_VALUE, COS_VALUE

        txa
        asl
        asl
        asl
        tay

        lda COS_VALUE
        STORE_SIGNED_POS CENTER_X, VGC_SPR_BASE + VGC_SPR_XL_OFF, VGC_SPR_BASE + VGC_SPR_XH_OFF

        lda SIN_VALUE
        cmp #$80
        ror
        STORE_SIGNED_POS CENTER_Y, VGC_SPR_BASE + VGC_SPR_YL_OFF, VGC_SPR_BASE + VGC_SPR_YH_OFF

        inx
        cpx #SPR_COUNT
        bne sincos_sprite_loop
        bra main_loop

define_shape:
        stz ROW_IDX
@row_loop:
        jsr write_shape_row
        inc ROW_IDX
        lda ROW_IDX
        cmp #16
        bne @row_loop
        rts

write_shape_row:
        lda #<shape_data
        sta PTR
        lda #>shape_data
        sta PTR+1

        lda ROW_IDX
        asl
        asl
        asl
        tay

        jsr wait_cmd
        stz VGC_P0
        lda ROW_IDX
        sta VGC_P1

        ldx #$00
@byte_loop:
        lda (PTR),y
        sta VGC_P2,x
        iny
        inx
        cpx #8
        bne @byte_loop

        jsr wait_cmd
        lda #VCMD_SPRROW
        sta VGC_CMD
        rts

init_sprites:
        ldx #$00
@loop:
        txa
        asl
        asl
        asl
        tay

        lda #<CENTER_X
        sta VGC_SPR_BASE + VGC_SPR_XL_OFF,y
        lda #>CENTER_X
        sta VGC_SPR_BASE + VGC_SPR_XH_OFF,y
        lda #CENTER_Y
        sta VGC_SPR_BASE + VGC_SPR_YL_OFF,y
        lda #$00
        sta VGC_SPR_BASE + VGC_SPR_YH_OFF,y
        sta VGC_SPR_BASE + VGC_SPR_SHAPE_OFF,y
        lda #VGC_SPR_FLAG_ENABLE
        sta VGC_SPR_BASE + VGC_SPR_FLAGS_OFF,y
        lda #VGC_SPR_PRI_FRONT
        sta VGC_SPR_BASE + VGC_SPR_PRI_OFF,y
        lda #SPR_TRANS
        sta VGC_SPR_BASE + VGC_SPR_TRANS_OFF,y

        inx
        cpx #SPR_COUNT
        bne @loop
        rts

wait_frame:
        lda VGC_FRAME
:
        cmp VGC_FRAME
        beq :-
        rts

wait_cmd:
        lda VGC_CMD
        and #$01
        bne wait_cmd
        rts

angle_offsets:
        .byte $00,$10,$20,$30,$40,$50,$60,$70
        .byte $80,$90,$A0,$B0,$C0,$D0,$E0,$F0

shape_data:
        .byte $66,$66,$66,$66,$66,$66,$66,$66
        .byte $66,$66,$BB,$44,$44,$BB,$66,$66
        .byte $66,$6B,$44,$44,$44,$49,$B6,$66
        .byte $66,$B4,$44,$A4,$44,$49,$9B,$66
        .byte $6B,$44,$AA,$A4,$44,$49,$99,$B6
        .byte $6B,$44,$44,$44,$44,$49,$99,$B6
        .byte $64,$44,$44,$44,$44,$49,$99,$96
        .byte $64,$44,$44,$44,$44,$49,$99,$96
        .byte $64,$44,$44,$44,$44,$49,$99,$96
        .byte $64,$44,$44,$44,$44,$49,$99,$96
        .byte $6B,$99,$99,$99,$99,$99,$99,$B6
        .byte $6B,$99,$99,$99,$99,$99,$99,$B6
        .byte $66,$B9,$99,$99,$99,$99,$9B,$66
        .byte $66,$6B,$99,$99,$99,$99,$B6,$66
        .byte $66,$66,$BB,$99,$99,$BB,$66,$66
        .byte $66,$66,$66,$66,$66,$66,$66,$66

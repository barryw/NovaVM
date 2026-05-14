; Shared harness for the per-function math coprocessor assembly demos.
; Each demo must define update_demo and demo_title.

.ifndef MATH_DEMO_COMMON_INC
MATH_DEMO_COMMON_INC = 1

.include "nova.inc"
.include "math_copro.inc"

SPR_COUNT       = 8
SPR_TRANS       = 0
SPR_SHAPE       = 0
BASE_X          = 32
BASE_Y          = 96
TITLE_LIMIT     = 39

PTR             = NVR0L
TICK            = NVR1L
SPR_INDEX       = NVR1H
VALUE0          = NVR2L
VALUE1          = NVR2H
VALUE2          = NVR3L
VALUE3          = NVR3H
TMP_X           = NVR4L
TMP_Y           = NVR4H
TMP_A           = NVR5L
TMP_B           = NVR5H
TMP_C           = NVR6L
TMP_D           = NVR6H
TMP_XH          = NVR7L

.segment "CODE"

start:
        stz VGC_CURSEN
        lda #$0F
        sta VGC_DIMMER
        stz VGC_BGCOL
        lda #$03
        sta VGC_MODE
        lda #$0C
        sta VGC_CHAROUT
        jsr wait_cmd
        lda #VCMD_GCLS
        sta VGC_CMD
        jsr wait_cmd
        jsr print_title
        jsr define_shape
        jsr init_sprites
        stz TICK

main_loop:
        jsr wait_frame
        inc TICK
        jsr update_demo
        bra main_loop

print_title:
        lda #$0F
        sta VGC_P0
        stz VGC_P1
        lda #VCMD_GCOLOR
        sta VGC_CMD
        jsr wait_cmd
        ldx #$00
@loop:
        cpx #TITLE_LIMIT
        bcs @done
        lda demo_title,x
        beq @done
        sta FIO_NAME,x
        inx
        bne @loop
@done:
        stx FIO_NAMELEN
        lda #$04
        sta VGC_P0
        stz VGC_P1
        lda #$04
        sta VGC_P2
        stz VGC_P3
        stz VGC_P4
        lda #$01
        sta VGC_P5
        lda #VCMD_GTEXT
        sta VGC_CMD
        jsr wait_cmd
        rts

define_shape:
        stz SPR_INDEX
@row_loop:
        jsr wait_cmd
        lda #SPR_SHAPE
        sta VGC_P0
        lda SPR_INDEX
        sta VGC_P1
        asl
        asl
        asl
        tay
        ldx #$00
@byte_loop:
        lda ball_shape,y
        sta VGC_P2,x
        iny
        inx
        cpx #$08
        bne @byte_loop
        jsr wait_cmd
        lda #VCMD_SPRROW
        sta VGC_CMD
        inc SPR_INDEX
        lda SPR_INDEX
        cmp #16
        bne @row_loop
        rts

init_sprites:
        ldx #$00
@loop:
        txa
        asl
        asl
        asl
        tay
        txa
        asl
        asl
        asl
        asl
        asl
        clc
        adc #BASE_X
        sta VGC_SPR_BASE + VGC_SPR_XL_OFF,y
        lda #$00
        sta VGC_SPR_BASE + VGC_SPR_XH_OFF,y
        lda #BASE_Y
        sta VGC_SPR_BASE + VGC_SPR_YL_OFF,y
        lda #$00
        sta VGC_SPR_BASE + VGC_SPR_YH_OFF,y
        lda #SPR_SHAPE
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

set_sprite_xy:
        txa
        asl
        asl
        asl
        tay
        lda TMP_X
        sta VGC_SPR_BASE + VGC_SPR_XL_OFF,y
        lda #$00
        sta VGC_SPR_BASE + VGC_SPR_XH_OFF,y
        lda TMP_Y
        sta VGC_SPR_BASE + VGC_SPR_YL_OFF,y
        lda #$00
        sta VGC_SPR_BASE + VGC_SPR_YH_OFF,y
        rts

set_sprite_xy16:
        txa
        asl
        asl
        asl
        tay
        lda TMP_X
        sta VGC_SPR_BASE + VGC_SPR_XL_OFF,y
        lda TMP_XH
        sta VGC_SPR_BASE + VGC_SPR_XH_OFF,y
        lda TMP_Y
        sta VGC_SPR_BASE + VGC_SPR_YL_OFF,y
        lda #$00
        sta VGC_SPR_BASE + VGC_SPR_YH_OFF,y
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

ball_shape:
        .byte $00,$00,$55,$55,$55,$55,$00,$00
        .byte $00,$55,$5D,$DD,$DD,$D5,$55,$00
        .byte $05,$5D,$DD,$DD,$DD,$DD,$D5,$50
        .byte $05,$DD,$DD,$DD,$DD,$DD,$DD,$50
        .byte $55,$DD,$DD,$DD,$DD,$DD,$DD,$55
        .byte $55,$DD,$DD,$DD,$DD,$DD,$DD,$55
        .byte $55,$DD,$DD,$DD,$DD,$DD,$DD,$55
        .byte $55,$DD,$DD,$DD,$DD,$DD,$DD,$55
        .byte $55,$DD,$DD,$DD,$DD,$DD,$DD,$55
        .byte $55,$DD,$DD,$DD,$DD,$DD,$DD,$55
        .byte $55,$DD,$DD,$DD,$DD,$DD,$DD,$55
        .byte $55,$DD,$DD,$DD,$DD,$DD,$DD,$55
        .byte $05,$DD,$DD,$DD,$DD,$DD,$DD,$50
        .byte $05,$5D,$DD,$DD,$DD,$DD,$D5,$50
        .byte $00,$55,$5D,$DD,$DD,$D5,$55,$00
        .byte $00,$00,$55,$55,$55,$55,$00,$00

.endif

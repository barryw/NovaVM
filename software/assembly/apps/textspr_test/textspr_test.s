; textspr_test.s -- minimal, demo.s-free probe: does the board VGC show TEXT and
; SPRITES at the same time? Draw 24 labelled rows of text (paced, one per frame),
; then load a white block sprite and enable 4 of them, then idle.
; All 24 rows survive with 4 blocks visible -> text+sprites coexist. Text collapses
; when sprites turn on -> VGC bug.  Load $7200. Links nova.lib (sprite_* only).

.include "novavm.inc"
.include "vsprite.inc"
.import sprite_row, sprite_pos, sprite_enable, sprite_set_shape
.import vsprite_gfx_fill

.segment "ZEROPAGE"
zp_r:    .res 1
zp_i:    .res 1
zp_last: .res 1

.segment "HEADER"
    .byte $00, $72

.segment "CODE"
start:
    sei
    cld
    ldx #$FF
    txs

    lda #2                      ; MODE_TEXT_GFX: text + gfx (vsprites) + sprites
    sta RegMode
    stz RegBgCol
    lda #1
    sta RegFgCol
    lda #1
    sta RegFont
    stz RegCursorEnable
    lda #$0C
    sta RegCharOut              ; clear
    lda RegStatus
    sta zp_last

    ; --- 24 rows of "<letter> ====..." one per frame ---
    stz zp_r
@row:
    lda zp_r
    sta RegCursorY
    stz RegCursorX
    lda zp_r
    clc
    adc #'A'
    sta RegCharOut
    lda #' '
    sta RegCharOut
    ldx #60
@bar:
    lda #'='
    sta RegCharOut
    dex
    bne @bar
    jsr my_vsync
    inc zp_r
    lda zp_r
    cmp #24
    bne @row

    ; --- load a solid white 16x16 block into shape slot 0 ---
    stz zp_r
@shape:
    lda zp_r
    sta RegP1
    stz RegP0
    lda #$11
    sta RegP0+2
    sta RegP0+3
    sta RegP0+4
    sta RegP0+5
    sta RegP0+6
    sta RegP0+7
    sta RegP0+8
    sta RegP0+9
    jsr sprite_row
    jsr my_cmdwait
    inc zp_r
    lda zp_r
    cmp #16
    bne @shape

    ; --- enable all 16 hardware sprites in a 4x4 grid over the text ---
    ldx #0
@spr:
    stx zp_i
    lda #0
    jsr sprite_set_shape
    jsr my_cmdwait
    ldx zp_i
    stx RegP0
    lda spr_x,x
    sta RegP1
    stz RegP2
    lda spr_y,x
    sta RegP3
    jsr sprite_pos
    jsr my_cmdwait
    ldx zp_i
    stx RegP0
    jsr sprite_enable
    jsr my_cmdwait
    ldx zp_i
    inx
    cpx #16
    bne @spr

    ; --- 16 vsprites: colored gfx-layer fills via the vsprite NDK ---
    ldx #0
@vs:
    stx zp_i
    lda vs_x,x
    sta VSPRITE_XL
    stz VSPRITE_XH
    lda vs_y,x
    sta VSPRITE_Y
    lda #18
    sta VSPRITE_WIDTHL
    stz VSPRITE_WIDTHH
    lda #12
    sta VSPRITE_HEIGHTL
    stz VSPRITE_HEIGHTH
    txa
    and #$0F
    clc
    adc #1                      ; colour 1..16
    sta VSPRITE_FILLVALUE
    jsr vsprite_gfx_fill
    jsr my_cmdwait
    ldx zp_i
    inx
    cpx #16
    bne @vs

@idle:
    jsr my_vsync
    bra @idle

my_vsync:
@w: lda RegStatus
    cmp zp_last
    beq @w
    sta zp_last
    rts

my_cmdwait:
@c: lda RegCmd
    and #1
    bne @c
    rts

.segment "RODATA"
spr_x:  .byte 24,96,168,240, 24,96,168,240, 24,96,168,240, 24,96,168,240
spr_y:  .byte 20,20,20,20, 56,56,56,56, 96,96,96,96, 140,140,140,140
vs_x:   .byte 60,132,204,250, 60,132,204,250, 60,132,204,250, 60,132,204,250
vs_y:   .byte 38,38,38,38, 76,76,76,76, 118,118,118,118, 162,162,162,162

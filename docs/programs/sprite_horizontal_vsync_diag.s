; sprite_horizontal_vsync_diag.s
;
; 6502 diagnostic for ULX3S/NovaVM sprite motion.
;
; Purpose:
; - Same visible behavior as the BASIC horizontal diagnostic: one logical
;   sprite pixel per video frame, bouncing across the 320-wide sprite plane.
; - Removes BASIC interpreter timing from the test.
; - Uses VGC sprite commands for shape setup, then direct sprite-register
;   writes for per-frame X position updates.
;
; Load at $9000 and run with: SYS $9000

.setcpu "6502"

.include "nova.inc"

SPR0_XLO    = VGC_SPR_XL
SPR0_XHI    = VGC_SPR_XH
SPR0_YLO    = VGC_SPR_YL
SPR0_YHI    = VGC_SPR_YH
SPR0_SHAPE  = VGC_SPR_SHAPE
SPR0_FLAGS  = VGC_SPR_FLAGS
SPR0_PRI    = VGC_SPR_PRI
SPR0_TRANS  = VGC_SPR_TRANS

SPR_Y       = 112
SPR_X_MAX   = 304

.segment "CODE"

start:
        lda #$00
        sta VGC_CURSEN
        lda #$03
        sta VGC_MODE
        lda #$06
        sta VGC_BGCOL

        ; Define sprite 0 as a solid color-$E square, matching the BASIC diag.
        ldx #$00
row_loop:
        jsr wait_cmd
        lda #$00
        sta VGC_P0
        txa
        sta VGC_P1
        lda #$EE
        sta VGC_P2
        sta VGC_P3
        sta VGC_P4
        sta VGC_P5
        sta VGC_P6
        sta VGC_P7
        sta VGC_P8
        sta VGC_P9
        lda #VCMD_SPRROW
        sta VGC_CMD
        inx
        cpx #VGC_MAX_SPRITES
        bne row_loop

        jsr wait_cmd

        lda #$00
        sta xpos_lo
        sta xpos_hi
        sta SPR0_XLO
        sta SPR0_XHI
        sta SPR0_SHAPE
        sta SPR0_TRANS
        sta SPR0_YHI

        lda #SPR_Y
        sta SPR0_YLO
        lda #VGC_SPR_PRI_FRONT
        sta SPR0_PRI
        lda #VGC_SPR_FLAG_ENABLE
        sta SPR0_FLAGS
        lda #$01
        sta xdir

main_loop:
        jsr wait_frame

        lda xpos_lo
        sta SPR0_XLO
        lda xpos_hi
        sta SPR0_XHI

        lda xdir
        bmi move_left

move_right:
        inc xpos_lo
        bne check_right
        inc xpos_hi
check_right:
        lda xpos_hi
        cmp #>SPR_X_MAX
        bcc main_loop
        bne hit_right
        lda xpos_lo
        cmp #<SPR_X_MAX
        bcc main_loop
hit_right:
        lda #<SPR_X_MAX
        sta xpos_lo
        lda #>SPR_X_MAX
        sta xpos_hi
        lda #$FF
        sta xdir
        jmp main_loop

move_left:
        lda xpos_lo
        bne dec_lo
        lda xpos_hi
        beq hit_left
        dec xpos_hi
dec_lo:
        dec xpos_lo
        jmp main_loop

hit_left:
        lda #$00
        sta xpos_lo
        sta xpos_hi
        lda #$01
        sta xdir
        jmp main_loop

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

xpos_lo:
        .byte $00
xpos_hi:
        .byte $00
xdir:
        .byte $01

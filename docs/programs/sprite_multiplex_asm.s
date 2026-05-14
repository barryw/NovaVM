; sprite_multiplex_asm.s
;
; Hardware sprite/copper smoke test.
;
; Load at $9000 and run with: SYS $9000
;
; Visible behavior:
; - 16 hardware sprites using the same four silhouettes as
;   sprite_multiplex.bas, recolored to exercise the 4-bit palette.
; - Copper rewrites SPRY(n) to multiplex those 16 sprites over 8 rows.
; - The 16 sprite columns bounce horizontally at different speeds.

.setcpu "65c02"

.include "nova.inc"

ROW_COUNT       = 8
SHAPE_COUNT     = 4
SPR_COUNT       = 16
ROW0_Y          = 12
ROW_STEP_Y      = 23
COPPER_LEAD_Y   = 8
FRAME_DIVIDER   = 4
SPR_LEFT_LO     = <0
SPR_RIGHT_LO    = <304
SPR_RIGHT_HI    = >304

PTR             = NVR0L
SHAPE_IDX       = NVR1L
ROW_IDX         = NVR1H
SPR_IDX         = NVR2L
ROW_Y_WORK      = NVR2H
TEMP            = NVR3L
FRAME_WAIT      = NVR3H

.segment "CODE"

start:
        lda #$00
        sta VGC_CURSEN
        lda #$03
        sta VGC_MODE
        lda #$06
        sta VGC_BGCOL

        jsr wait_cmd
        lda #VCMD_COPPERDIS
        sta VGC_CMD
        jsr wait_cmd
        lda #VCMD_COPPERCLR
        sta VGC_CMD

        jsr define_shapes
        jsr init_sprites
        jsr build_copper_rows

        jsr wait_cmd
        lda #VCMD_COPPERENA
        sta VGC_CMD

main_loop:
        jsr wait_anim_tick
        jsr move_sprites
        bra main_loop

define_shapes:
        stz SHAPE_IDX
@shape_loop:
        stz ROW_IDX
@row_loop:
        jsr write_shape_row
        inc ROW_IDX
        lda ROW_IDX
        cmp #16
        bne @row_loop

        inc SHAPE_IDX
        lda SHAPE_IDX
        cmp #SHAPE_COUNT
        bne @shape_loop
        rts

write_shape_row:
        jsr load_shape_row_ptr

        jsr wait_cmd
        lda SHAPE_IDX
        sta VGC_P0
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

load_shape_row_ptr:
        lda #<shape_data
        sta PTR
        lda #>shape_data
        sta PTR+1

        lda SHAPE_IDX
        sta TEMP
@shape_loop:
        lda TEMP
        beq @row_offset
        lda PTR
        clc
        adc #$80
        sta PTR
        lda PTR+1
        adc #$00
        sta PTR+1
        dec TEMP
        bra @shape_loop

@row_offset:
        lda ROW_IDX
        asl
        asl
        asl
        tay
        rts

init_sprites:
        ldx #$00
@loop:
        txa
        sta SPR_IDX

        txa
        asl
        asl
        asl
        tay

        lda x_init_lo,x
        sta VGC_SPR_BASE + VGC_SPR_XL_OFF,y
        lda x_init_hi,x
        sta VGC_SPR_BASE + VGC_SPR_XH_OFF,y
        lda #ROW0_Y
        sta VGC_SPR_BASE + VGC_SPR_YL_OFF,y
        lda #$00
        sta VGC_SPR_BASE + VGC_SPR_YH_OFF,y
        txa
        and #$03
        sta VGC_SPR_BASE + VGC_SPR_SHAPE_OFF,y
        lda #VGC_SPR_FLAG_ENABLE
        sta VGC_SPR_BASE + VGC_SPR_FLAGS_OFF,y
        lda #VGC_SPR_PRI_FRONT
        sta VGC_SPR_BASE + VGC_SPR_PRI_OFF,y
        lda #$06
        sta VGC_SPR_BASE + VGC_SPR_TRANS_OFF,y

        lda x_init_lo,x
        sta xpos_lo,x
        lda x_init_hi,x
        sta xpos_hi,x
        stz dir_flags,x

        inx
        cpx #SPR_COUNT
        bne @loop

        ; Publish the direct sprite-register writes before copper starts.
        jsr wait_frame
        rts

build_copper_rows:
        ; Restore hardware sprite Y at the start of each frame before adding
        ; the seven visible multiplex rows. This keeps the first row stable
        ; instead of relying only on the next sprite-frame commit.
        lda #ROW0_Y
        sta ROW_Y_WORK
        stz ROW_IDX
        stz SPR_IDX
@restore_loop:
        jsr add_copper_y_write
        inc SPR_IDX
        lda SPR_IDX
        cmp #SPR_COUNT
        bne @restore_loop

        lda #ROW0_Y + ROW_STEP_Y
        sta ROW_Y_WORK
        lda #$01
        sta ROW_IDX
@row_loop:
        stz SPR_IDX
@sprite_loop:
        jsr add_copper_y_write

        inc SPR_IDX
        lda SPR_IDX
        cmp #SPR_COUNT
        bne @sprite_loop

        lda ROW_Y_WORK
        clc
        adc #ROW_STEP_Y
        sta ROW_Y_WORK
        inc ROW_IDX
        lda ROW_IDX
        cmp #ROW_COUNT
        bne @row_loop
        rts

add_copper_y_write:
        lda #$00
        sta VGC_P0
        sta VGC_P1
        lda ROW_IDX
        beq @frame_start
        lda ROW_Y_WORK
        sec
        sbc #COPPER_LEAD_Y
        bra @store_y
@frame_start:
        lda #$00
@store_y:
        sta VGC_P2

        lda SPR_IDX
        asl
        asl
        asl
        clc
        adc #(VGC_SPR_BASE & $FF) + VGC_SPR_YL_OFF
        sta VGC_P3
        lda #>VGC_SPR_BASE
        adc #$00
        sta VGC_P4
        lda ROW_Y_WORK
        sta VGC_P5

        jsr wait_cmd
        lda #VCMD_COPPERADD
        sta VGC_CMD
        rts

move_sprites:
        ldx #$00
@loop:
        lda dir_flags,x
        bne @move_left

@move_right:
        lda xpos_lo,x
        clc
        adc speeds,x
        sta xpos_lo,x
        lda xpos_hi,x
        adc #$00
        sta xpos_hi,x
        cmp #SPR_RIGHT_HI
        bcc @store
        bne @hit_right
        lda xpos_lo,x
        cmp #SPR_RIGHT_LO + 1
        bcc @store
@hit_right:
        lda #SPR_RIGHT_LO
        sta xpos_lo,x
        lda #SPR_RIGHT_HI
        sta xpos_hi,x
        lda #$01
        sta dir_flags,x
        bra @store

@move_left:
        lda xpos_lo,x
        sec
        sbc speeds,x
        sta xpos_lo,x
        lda xpos_hi,x
        sbc #$00
        sta xpos_hi,x
        bmi @hit_left
        bne @store
        lda xpos_lo,x
        cmp #SPR_LEFT_LO
        bcs @store
@hit_left:
        lda #SPR_LEFT_LO
        sta xpos_lo,x
        stz xpos_hi,x
        stz dir_flags,x

@store:
        txa
        asl
        asl
        asl
        tay
        lda xpos_lo,x
        sta VGC_SPR_BASE + VGC_SPR_XL_OFF,y
        lda xpos_hi,x
        sta VGC_SPR_BASE + VGC_SPR_XH_OFF,y

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

wait_anim_tick:
        lda #FRAME_DIVIDER
        sta FRAME_WAIT
@loop:
        jsr wait_frame
        dec FRAME_WAIT
        bne @loop
        rts

wait_cmd:
        lda VGC_CMD
        and #$01
        bne wait_cmd
        rts

speeds:
        .byte 1,2,3,4,5,6,7,8,1,2,3,4,5,6,7,8
x_init_lo:
        .byte <0,<20,<40,<60,<80,<100,<120,<140
        .byte <160,<180,<200,<220,<240,<260,<280,<300
x_init_hi:
        .byte >0,>20,>40,>60,>80,>100,>120,>140
        .byte >160,>180,>200,>220,>240,>260,>280,>300

xpos_lo:
        .res SPR_COUNT, 0
xpos_hi:
        .res SPR_COUNT, 0
dir_flags:
        .res SPR_COUNT, 0

; Four sprite silhouettes with color 6 as transparent blue background.
; The visible pixels intentionally cover every palette color except 6.
shape_data:
        .byte $66,$66,$66,$6B,$B6,$66,$66,$66
        .byte $66,$66,$66,$B5,$1B,$66,$66,$66
        .byte $66,$66,$6B,$FF,$FF,$B6,$66,$66
        .byte $66,$66,$B5,$FF,$FF,$3B,$66,$66
        .byte $66,$6B,$45,$FF,$FF,$33,$B6,$66
        .byte $66,$B4,$45,$55,$11,$33,$3B,$66
        .byte $6B,$44,$45,$55,$11,$33,$3E,$B6
        .byte $B4,$44,$45,$55,$11,$33,$3E,$EB
        .byte $6B,$44,$45,$55,$11,$33,$3E,$B6
        .byte $66,$B4,$45,$55,$11,$33,$3B,$66
        .byte $66,$6B,$45,$55,$11,$33,$B6,$66
        .byte $66,$66,$B5,$55,$11,$3B,$66,$66
        .byte $66,$66,$6B,$55,$11,$B6,$66,$66
        .byte $66,$66,$66,$B5,$1B,$66,$66,$66
        .byte $66,$66,$66,$6B,$B6,$66,$66,$66
        .byte $66,$66,$66,$66,$66,$66,$66,$66

        .byte $66,$66,$66,$66,$96,$66,$66,$66
        .byte $66,$66,$99,$99,$99,$99,$66,$66
        .byte $66,$9F,$FF,$FF,$AA,$88,$29,$66
        .byte $66,$9F,$FF,$FF,$AA,$88,$22,$96
        .byte $69,$FF,$FF,$FF,$AA,$88,$22,$96
        .byte $9A,$AA,$AA,$AA,$AA,$88,$22,$29
        .byte $9A,$AA,$AA,$AA,$AA,$88,$22,$29
        .byte $98,$88,$88,$88,$88,$88,$22,$29
        .byte $98,$88,$88,$88,$88,$88,$22,$29
        .byte $9C,$CC,$CC,$CC,$22,$22,$22,$29
        .byte $9C,$CC,$CC,$CC,$22,$22,$22,$29
        .byte $69,$CC,$CC,$CC,$22,$22,$22,$96
        .byte $66,$9C,$CC,$CC,$22,$22,$22,$96
        .byte $66,$9C,$CC,$CC,$22,$22,$29,$66
        .byte $66,$66,$99,$99,$99,$99,$66,$66
        .byte $66,$66,$66,$66,$96,$66,$66,$66

        .byte $0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B
        .byte $B0,$44,$CC,$DD,$77,$EE,$FF,$50
        .byte $0D,$77,$EE,$FF,$55,$33,$AA,$2B
        .byte $BD,$77,$EE,$FF,$55,$33,$AA,$20
        .byte $0F,$55,$33,$AA,$22,$88,$99,$BB
        .byte $BF,$55,$33,$AA,$22,$88,$99,$B0
        .byte $0A,$22,$88,$99,$BB,$11,$00,$4B
        .byte $BA,$22,$88,$99,$BB,$11,$00,$40
        .byte $09,$BB,$11,$00,$44,$CC,$DD,$7B
        .byte $B9,$BB,$11,$00,$44,$CC,$DD,$70
        .byte $00,$44,$CC,$DD,$77,$EE,$FF,$5B
        .byte $B0,$44,$CC,$DD,$77,$EE,$FF,$50
        .byte $0D,$77,$EE,$FF,$55,$33,$AA,$2B
        .byte $BD,$77,$EE,$FF,$55,$33,$AA,$20
        .byte $0F,$55,$33,$AA,$22,$88,$99,$BB
        .byte $B0,$B0,$B0,$B0,$B0,$B0,$B0,$B0

        .byte $F6,$66,$66,$66,$66,$66,$66,$68
        .byte $6E,$66,$66,$66,$66,$66,$66,$76
        .byte $66,$D6,$66,$66,$66,$66,$65,$66
        .byte $66,$6C,$66,$66,$66,$66,$46,$66
        .byte $66,$66,$B6,$66,$66,$63,$66,$66
        .byte $66,$66,$6A,$66,$66,$26,$66,$66
        .byte $66,$66,$66,$96,$61,$66,$66,$66
        .byte $66,$66,$66,$68,$06,$66,$66,$66
        .byte $66,$66,$66,$6F,$76,$66,$66,$66
        .byte $66,$66,$66,$F6,$65,$66,$66,$66
        .byte $66,$66,$6E,$66,$66,$46,$66,$66
        .byte $66,$66,$D6,$66,$66,$63,$66,$66
        .byte $66,$6C,$66,$66,$66,$66,$26,$66
        .byte $66,$B6,$66,$66,$66,$66,$61,$66
        .byte $6A,$66,$66,$66,$66,$66,$66,$06
        .byte $96,$66,$66,$66,$66,$66,$66,$6F

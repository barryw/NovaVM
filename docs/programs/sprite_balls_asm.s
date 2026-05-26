; sprite_balls_asm.s
;
; Eight shaded hardware sprites bouncing with fixed-point physics.
; Collisions are elastic: velocity reflects, but does not decay.
;
; Build:
;   ca65 --cpu 65c02 -I runtime/asm -o /tmp/sprite_balls_asm.o docs/programs/sprite_balls_asm.s
;   ld65 -C docs/programs/diag9000_4k.cfg -o /tmp/sprite_balls_asm.bin /tmp/sprite_balls_asm.o
;
; Load at $9000 and run with: SYS $9000

.setcpu "65c02"

.include "nova.inc"
.include "math_copro.inc"
.include "math_fixed8.inc"

BALL_COUNT       = 8
SHAPE_COUNT      = 8
BALL_TRANS       = 6
BALL_MAX_X       = 304
BALL_MAX_Y       = 184
BALL_MAX2_X      = BALL_MAX_X * 2
BALL_MAX2_Y      = BALL_MAX_Y * 2
BALL_DIAMETER    = 16
GRAVITY_LO       = $10       ; +0.0625 px/frame^2
GRAVITY_HI       = $00

PTR              = NVR0L
SHAPE_IDX        = NVR1L
ROW_IDX          = NVR1H
PAIR_A           = NVR0L
PAIR_B           = NVR0H
DELTA_X          = NVR1L
DELTA_Y          = NVR1H
ORDER_X          = NVR2L
ORDER_Y          = NVR2H
TEMP             = NVR2H
MAG_LO           = NVR3L
MAG_HI           = NVR3H

.segment "CODE"

start:
        lda #$00
        sta VGC_CURSEN
        lda #$03
        sta VGC_MODE
        lda #BALL_TRANS
        sta VGC_BGCOL

        jsr wait_cmd
        lda #VCMD_COPPERDIS
        sta VGC_CMD
        jsr wait_cmd
        lda #VCMD_COPPERCLR
        sta VGC_CMD

        jsr define_shapes
        jsr init_balls
        jsr wait_frame

main_loop:
        jsr wait_frame
        ldx #$00
@update_loop:
        jsr update_ball
        inx
        cpx #BALL_COUNT
        bne @update_loop

        jsr collide_balls

        ldx #$00
@publish_loop:
        jsr publish_ball
        inx
        cpx #BALL_COUNT
        bne @publish_loop
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

init_balls:
        ldx #$00
@loop:
        lda init_x_frac,x
        sta ball_x_frac,x
        lda init_x_lo,x
        sta ball_x_lo,x
        lda init_x_hi,x
        sta ball_x_hi,x
        lda init_y_frac,x
        sta ball_y_frac,x
        lda init_y_lo,x
        sta ball_y_lo,x
        lda init_y_hi,x
        sta ball_y_hi,x
        lda init_vx_frac,x
        sta ball_vx_frac,x
        lda init_vx_int,x
        sta ball_vx_int,x
        lda init_vy_frac,x
        sta ball_vy_frac,x
        lda init_vy_int,x
        sta ball_vy_int,x

        txa
        asl
        asl
        asl
        tay
        lda ball_x_lo,x
        sta VGC_SPR_BASE + VGC_SPR_XL_OFF,y
        lda ball_x_hi,x
        sta VGC_SPR_BASE + VGC_SPR_XH_OFF,y
        lda ball_y_lo,x
        sta VGC_SPR_BASE + VGC_SPR_YL_OFF,y
        lda ball_y_hi,x
        sta VGC_SPR_BASE + VGC_SPR_YH_OFF,y
        txa
        sta VGC_SPR_BASE + VGC_SPR_SHAPE_OFF,y
        lda #VGC_SPR_FLAG_ENABLE
        sta VGC_SPR_BASE + VGC_SPR_FLAGS_OFF,y
        lda #VGC_SPR_PRI_FRONT
        sta VGC_SPR_BASE + VGC_SPR_PRI_OFF,y
        lda #BALL_TRANS
        sta VGC_SPR_BASE + VGC_SPR_TRANS_OFF,y

        inx
        cpx #BALL_COUNT
        bne @loop
        rts

update_ball:
        FIX8_ADD_CONST_X ball_vy_frac, ball_vy_int, GRAVITY_LO, GRAVITY_HI
        FIX16_8_ADD_FIX8_X ball_x_frac, ball_x_lo, ball_x_hi, ball_vx_frac, ball_vx_int
        FIX16_8_ADD_FIX8_X ball_y_frac, ball_y_lo, ball_y_hi, ball_vy_frac, ball_vy_int
        jsr collide_x
        jsr collide_y
        rts

collide_balls:
        stz PAIR_A
pair_outer:
        lda PAIR_A
        cmp #(BALL_COUNT - 1)
        bcs pair_done
        clc
        adc #$01
        sta PAIR_B
pair_inner:
        jsr collide_pair
        inc PAIR_B
        lda PAIR_B
        cmp #BALL_COUNT
        bne pair_inner
        inc PAIR_A
        bra pair_outer
pair_done:
        rts

collide_pair:
        jsr pair_delta_x
        lda DELTA_X
        cmp #BALL_DIAMETER
        bcs collide_pair_done

        jsr pair_delta_y
        lda DELTA_Y
        cmp #BALL_DIAMETER
        bcs collide_pair_done

        MATHC_DIST_APPROX_U8 DELTA_X, DELTA_Y, MAG_LO, MAG_HI
        lda MAG_HI
        bne collide_pair_done
        lda MAG_LO
        cmp #BALL_DIAMETER
        bcs collide_pair_done

        lda DELTA_X
        cmp DELTA_Y
        bcc collide_pair_vertical

collide_pair_horizontal:
        jsr separate_pair_x
        jsr swap_pair_vx
        rts

collide_pair_vertical:
        jsr separate_pair_y
        jsr swap_pair_vy
collide_pair_done:
        rts

pair_delta_x:
        ldx PAIR_A
        lda ball_x_lo,x
        sta MAG_LO
        lda ball_x_hi,x
        sta MAG_HI

        ldx PAIR_B
        lda ball_x_hi,x
        cmp MAG_HI
        bcc pair_delta_x_a_right
        bne pair_delta_x_b_right
        lda ball_x_lo,x
        cmp MAG_LO
        bcc pair_delta_x_a_right

pair_delta_x_b_right:
        stz ORDER_X
        ldx PAIR_B
        lda ball_x_lo,x
        sec
        sbc MAG_LO
        sta DELTA_X
        lda ball_x_hi,x
        sbc MAG_HI
        beq pair_delta_x_done
        lda #$FF
        sta DELTA_X
        rts

pair_delta_x_a_right:
        lda #$01
        sta ORDER_X
        ldx PAIR_A
        lda ball_x_lo,x
        sec
        ldx PAIR_B
        sbc ball_x_lo,x
        sta DELTA_X
        ldx PAIR_A
        lda ball_x_hi,x
        ldx PAIR_B
        sbc ball_x_hi,x
        beq pair_delta_x_done
        lda #$FF
        sta DELTA_X
pair_delta_x_done:
        rts

pair_delta_y:
        ldx PAIR_A
        lda ball_y_lo,x
        sta MAG_LO
        ldx PAIR_B
        lda ball_y_lo,x
        cmp MAG_LO
        bcc pair_delta_y_a_below

        stz ORDER_Y
        lda ball_y_lo,x
        sec
        sbc MAG_LO
        sta DELTA_Y
        rts

pair_delta_y_a_below:
        lda #$01
        sta ORDER_Y
        ldx PAIR_A
        lda ball_y_lo,x
        sec
        ldx PAIR_B
        sbc ball_y_lo,x
        sta DELTA_Y
        rts

separate_pair_x:
        lda ORDER_X
        bne separate_pair_x_a_right

        ldx PAIR_A
        lda ball_x_lo,x
        clc
        adc #BALL_DIAMETER
        sta MAG_LO
        lda ball_x_hi,x
        adc #$00
        sta MAG_HI
        jsr mag_x_in_bounds
        bcc separate_pair_x_store_b

        ldx PAIR_B
        lda ball_x_lo,x
        sec
        sbc #BALL_DIAMETER
        sta MAG_LO
        lda ball_x_hi,x
        sbc #$00
        sta MAG_HI
        ldx PAIR_A
        bra separate_pair_x_store_x

separate_pair_x_store_b:
        ldx PAIR_B
        bra separate_pair_x_store_x

separate_pair_x_a_right:
        ldx PAIR_B
        lda ball_x_lo,x
        clc
        adc #BALL_DIAMETER
        sta MAG_LO
        lda ball_x_hi,x
        adc #$00
        sta MAG_HI
        jsr mag_x_in_bounds
        bcc separate_pair_x_store_a

        ldx PAIR_A
        lda ball_x_lo,x
        sec
        sbc #BALL_DIAMETER
        sta MAG_LO
        lda ball_x_hi,x
        sbc #$00
        sta MAG_HI
        ldx PAIR_B
        bra separate_pair_x_store_x

separate_pair_x_store_a:
        ldx PAIR_A
separate_pair_x_store_x:
        stz ball_x_frac,x
        lda MAG_LO
        sta ball_x_lo,x
        lda MAG_HI
        sta ball_x_hi,x
        rts

mag_x_in_bounds:
        lda MAG_HI
        cmp #>BALL_MAX_X
        bcc mag_x_ok
        bne mag_x_oob
        lda MAG_LO
        cmp #(<BALL_MAX_X + 1)
        bcc mag_x_ok
mag_x_oob:
        sec
        rts
mag_x_ok:
        clc
        rts

separate_pair_y:
        lda ORDER_Y
        bne separate_pair_y_a_below

        ldx PAIR_A
        lda ball_y_lo,x
        clc
        adc #BALL_DIAMETER
        cmp #(<BALL_MAX_Y + 1)
        bcs separate_pair_y_a_up
        sta MAG_LO
        ldx PAIR_B
        bra separate_pair_y_store_x

separate_pair_y_a_up:
        ldx PAIR_B
        lda ball_y_lo,x
        sec
        sbc #BALL_DIAMETER
        sta MAG_LO
        ldx PAIR_A
        bra separate_pair_y_store_x

separate_pair_y_a_below:
        ldx PAIR_B
        lda ball_y_lo,x
        clc
        adc #BALL_DIAMETER
        cmp #(<BALL_MAX_Y + 1)
        bcs separate_pair_y_b_up
        sta MAG_LO
        ldx PAIR_A
        bra separate_pair_y_store_x

separate_pair_y_b_up:
        ldx PAIR_A
        lda ball_y_lo,x
        sec
        sbc #BALL_DIAMETER
        sta MAG_LO
        ldx PAIR_B

separate_pair_y_store_x:
        stz ball_y_frac,x
        lda MAG_LO
        sta ball_y_lo,x
        stz ball_y_hi,x
        rts

swap_pair_vx:
        ldx PAIR_A
        lda ball_vx_frac,x
        sta MAG_LO
        lda ball_vx_int,x
        sta MAG_HI

        ldx PAIR_B
        lda ball_vx_frac,x
        sta DELTA_X
        lda ball_vx_int,x
        sta DELTA_Y
        lda MAG_LO
        sta ball_vx_frac,x
        lda MAG_HI
        sta ball_vx_int,x

        ldx PAIR_A
        lda DELTA_X
        sta ball_vx_frac,x
        lda DELTA_Y
        sta ball_vx_int,x
        rts

swap_pair_vy:
        ldx PAIR_A
        lda ball_vy_frac,x
        sta MAG_LO
        lda ball_vy_int,x
        sta MAG_HI

        ldx PAIR_B
        lda ball_vy_frac,x
        sta DELTA_X
        lda ball_vy_int,x
        sta DELTA_Y
        lda MAG_LO
        sta ball_vy_frac,x
        lda MAG_HI
        sta ball_vy_int,x

        ldx PAIR_A
        lda DELTA_X
        sta ball_vy_frac,x
        lda DELTA_Y
        sta ball_vy_int,x
        rts

collide_x:
        lda ball_x_hi,x
        bpl collide_x_check_right

collide_x_hit_left:
        FIX16_8_NEGATE_X ball_x_frac, ball_x_lo, ball_x_hi
        FIX8_ABS_TO_X ball_vx_frac, ball_vx_int, MAG_LO, MAG_HI
        lda MAG_LO
        sta ball_vx_frac,x
        lda MAG_HI
        sta ball_vx_int,x
        rts

collide_x_check_right:
        lda ball_x_hi,x
        cmp #>BALL_MAX_X
        bcc collide_x_done
        bne collide_x_hit_right
        lda ball_x_lo,x
        cmp #<BALL_MAX_X
        bcc collide_x_done
        bne collide_x_hit_right
        lda ball_x_frac,x
        beq collide_x_done

collide_x_hit_right:
        FIX16_8_REFLECT_MAX_X ball_x_frac, ball_x_lo, ball_x_hi, <BALL_MAX2_X, >BALL_MAX2_X
        FIX8_ABS_TO_X ball_vx_frac, ball_vx_int, MAG_LO, MAG_HI
        FIX8_NEGATE_MEM MAG_LO, MAG_HI
        lda MAG_LO
        sta ball_vx_frac,x
        lda MAG_HI
        sta ball_vx_int,x
collide_x_done:
        rts

collide_y:
        lda ball_y_hi,x
        bpl collide_y_check_floor

collide_y_hit_ceiling:
        FIX16_8_NEGATE_X ball_y_frac, ball_y_lo, ball_y_hi
        FIX8_ABS_TO_X ball_vy_frac, ball_vy_int, MAG_LO, MAG_HI
        lda MAG_LO
        sta ball_vy_frac,x
        lda MAG_HI
        sta ball_vy_int,x
        rts

collide_y_check_floor:
        lda ball_y_hi,x
        beq collide_y_check_floor_low
        bra collide_y_hit_floor
collide_y_check_floor_low:
        lda ball_y_lo,x
        cmp #<BALL_MAX_Y
        bcc collide_y_done
        bne collide_y_hit_floor
        lda ball_y_frac,x
        beq collide_y_done

collide_y_hit_floor:
        FIX16_8_REFLECT_MAX_X ball_y_frac, ball_y_lo, ball_y_hi, <BALL_MAX2_Y, >BALL_MAX2_Y
        FIX8_ABS_TO_X ball_vy_frac, ball_vy_int, MAG_LO, MAG_HI
        jsr apply_floor_rebound_energy
        FIX8_NEGATE_MEM MAG_LO, MAG_HI
        lda MAG_LO
        sta ball_vy_frac,x
        lda MAG_HI
        sta ball_vy_int,x
collide_y_done:
        rts

apply_floor_rebound_energy:
        lda floor_vy_frac,x
        sta DELTA_X
        lda floor_vy_int,x
        sta DELTA_Y

        lda MAG_HI
        cmp DELTA_Y
        bcc apply_floor_rebound_min
        bne apply_floor_rebound_done
        lda MAG_LO
        cmp DELTA_X
        bcs apply_floor_rebound_done

apply_floor_rebound_min:
        lda DELTA_X
        sta MAG_LO
        lda DELTA_Y
        sta MAG_HI
apply_floor_rebound_done:
        rts

publish_ball:
        txa
        asl
        asl
        asl
        tay
        lda ball_x_lo,x
        sta VGC_SPR_BASE + VGC_SPR_XL_OFF,y
        lda ball_x_hi,x
        sta VGC_SPR_BASE + VGC_SPR_XH_OFF,y
        lda ball_y_lo,x
        sta VGC_SPR_BASE + VGC_SPR_YL_OFF,y
        lda ball_y_hi,x
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

init_x_frac:
        .byte $00,$00,$00,$00,$00,$00,$00,$00
init_x_lo:
        .byte <12,<52,<92,<132,<174,<218,<260,<300
init_x_hi:
        .byte >12,>52,>92,>132,>174,>218,>260,>300
init_y_frac:
        .byte $00,$00,$00,$00,$00,$00,$00,$00
init_y_lo:
        .byte <18,<34,<52,<72,<96,<118,<140,<160
init_y_hi:
        .byte >18,>34,>52,>72,>96,>118,>140,>160
init_vx_frac:
        .byte $CD,$26,$80,$DA,$CD,$8D,$1A,$33
init_vx_int:
        .byte $00,$01,$FE,$01,$FD,$02,$FD,$03
init_vy_frac:
        .byte $66,$E6,$66,$E6,$66,$E6,$A6,$26
init_vy_int:
        .byte $FF,$FE,$FE,$FD,$FD,$FC,$FE,$FD

floor_vy_frac:
        .byte $80,$40,$00,$C0,$00,$80,$C0,$40
floor_vy_int:
        .byte $02,$03,$04,$04,$03,$04,$03,$04

ball_x_frac:
        .res BALL_COUNT, 0
ball_x_lo:
        .res BALL_COUNT, 0
ball_x_hi:
        .res BALL_COUNT, 0
ball_y_frac:
        .res BALL_COUNT, 0
ball_y_lo:
        .res BALL_COUNT, 0
ball_y_hi:
        .res BALL_COUNT, 0
ball_vx_frac:
        .res BALL_COUNT, 0
ball_vx_int:
        .res BALL_COUNT, 0
ball_vy_frac:
        .res BALL_COUNT, 0
ball_vy_int:
        .res BALL_COUNT, 0

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

        .byte $66,$66,$66,$66,$66,$66,$66,$66
        .byte $66,$66,$BB,$55,$55,$BB,$66,$66
        .byte $66,$6B,$55,$55,$55,$52,$B6,$66
        .byte $66,$B5,$55,$D5,$55,$52,$2B,$66
        .byte $6B,$55,$DD,$D5,$55,$52,$22,$B6
        .byte $6B,$55,$55,$55,$55,$52,$22,$B6
        .byte $65,$55,$55,$55,$55,$52,$22,$26
        .byte $65,$55,$55,$55,$55,$52,$22,$26
        .byte $65,$55,$55,$55,$55,$52,$22,$26
        .byte $65,$55,$55,$55,$55,$52,$22,$26
        .byte $6B,$22,$22,$22,$22,$22,$22,$B6
        .byte $6B,$22,$22,$22,$22,$22,$22,$B6
        .byte $66,$B2,$22,$22,$22,$22,$2B,$66
        .byte $66,$6B,$22,$22,$22,$22,$B6,$66
        .byte $66,$66,$BB,$22,$22,$BB,$66,$66
        .byte $66,$66,$66,$66,$66,$66,$66,$66

        .byte $66,$66,$66,$66,$66,$66,$66,$66
        .byte $66,$66,$BB,$77,$77,$BB,$66,$66
        .byte $66,$6B,$77,$77,$77,$78,$B6,$66
        .byte $66,$B7,$77,$F7,$77,$78,$8B,$66
        .byte $6B,$77,$FF,$F7,$77,$78,$88,$B6
        .byte $6B,$77,$77,$77,$77,$78,$88,$B6
        .byte $67,$77,$77,$77,$77,$78,$88,$86
        .byte $67,$77,$77,$77,$77,$78,$88,$86
        .byte $67,$77,$77,$77,$77,$78,$88,$86
        .byte $67,$77,$77,$77,$77,$78,$88,$86
        .byte $6B,$88,$88,$88,$88,$88,$88,$B6
        .byte $6B,$88,$88,$88,$88,$88,$88,$B6
        .byte $66,$B8,$88,$88,$88,$88,$8B,$66
        .byte $66,$6B,$88,$88,$88,$88,$B6,$66
        .byte $66,$66,$BB,$88,$88,$BB,$66,$66
        .byte $66,$66,$66,$66,$66,$66,$66,$66

        .byte $66,$66,$66,$66,$66,$66,$66,$66
        .byte $66,$66,$BB,$88,$88,$BB,$66,$66
        .byte $66,$6B,$88,$88,$88,$89,$B6,$66
        .byte $66,$B8,$88,$78,$88,$89,$9B,$66
        .byte $6B,$88,$77,$78,$88,$89,$99,$B6
        .byte $6B,$88,$88,$88,$88,$89,$99,$B6
        .byte $68,$88,$88,$88,$88,$89,$99,$96
        .byte $68,$88,$88,$88,$88,$89,$99,$96
        .byte $68,$88,$88,$88,$88,$89,$99,$96
        .byte $68,$88,$88,$88,$88,$89,$99,$96
        .byte $6B,$99,$99,$99,$99,$99,$99,$B6
        .byte $6B,$99,$99,$99,$99,$99,$99,$B6
        .byte $66,$B9,$99,$99,$99,$99,$9B,$66
        .byte $66,$6B,$99,$99,$99,$99,$B6,$66
        .byte $66,$66,$BB,$99,$99,$BB,$66,$66
        .byte $66,$66,$66,$66,$66,$66,$66,$66

        .byte $66,$66,$66,$66,$66,$66,$66,$66
        .byte $66,$66,$BB,$AA,$AA,$BB,$66,$66
        .byte $66,$6B,$AA,$AA,$AA,$A4,$B6,$66
        .byte $66,$BA,$AA,$FA,$AA,$A4,$4B,$66
        .byte $6B,$AA,$FF,$FA,$AA,$A4,$44,$B6
        .byte $6B,$AA,$AA,$AA,$AA,$A4,$44,$B6
        .byte $6A,$AA,$AA,$AA,$AA,$A4,$44,$46
        .byte $6A,$AA,$AA,$AA,$AA,$A4,$44,$46
        .byte $6A,$AA,$AA,$AA,$AA,$A4,$44,$46
        .byte $6A,$AA,$AA,$AA,$AA,$A4,$44,$46
        .byte $6B,$44,$44,$44,$44,$44,$44,$B6
        .byte $6B,$44,$44,$44,$44,$44,$44,$B6
        .byte $66,$B4,$44,$44,$44,$44,$4B,$66
        .byte $66,$6B,$44,$44,$44,$44,$B6,$66
        .byte $66,$66,$BB,$44,$44,$BB,$66,$66
        .byte $66,$66,$66,$66,$66,$66,$66,$66

        .byte $66,$66,$66,$66,$66,$66,$66,$66
        .byte $66,$66,$BB,$DD,$DD,$BB,$66,$66
        .byte $66,$6B,$DD,$DD,$DD,$D5,$B6,$66
        .byte $66,$BD,$DD,$FD,$DD,$D5,$5B,$66
        .byte $6B,$DD,$FF,$FD,$DD,$D5,$55,$B6
        .byte $6B,$DD,$DD,$DD,$DD,$D5,$55,$B6
        .byte $6D,$DD,$DD,$DD,$DD,$D5,$55,$56
        .byte $6D,$DD,$DD,$DD,$DD,$D5,$55,$56
        .byte $6D,$DD,$DD,$DD,$DD,$D5,$55,$56
        .byte $6D,$DD,$DD,$DD,$DD,$D5,$55,$56
        .byte $6B,$55,$55,$55,$55,$55,$55,$B6
        .byte $6B,$55,$55,$55,$55,$55,$55,$B6
        .byte $66,$B5,$55,$55,$55,$55,$5B,$66
        .byte $66,$6B,$55,$55,$55,$55,$B6,$66
        .byte $66,$66,$BB,$55,$55,$BB,$66,$66
        .byte $66,$66,$66,$66,$66,$66,$66,$66

        .byte $66,$66,$66,$66,$66,$66,$66,$66
        .byte $66,$66,$BB,$EE,$EE,$BB,$66,$66
        .byte $66,$6B,$EE,$EE,$EE,$E1,$B6,$66
        .byte $66,$BE,$EE,$3E,$EE,$E1,$1B,$66
        .byte $6B,$EE,$33,$3E,$EE,$E1,$11,$B6
        .byte $6B,$EE,$EE,$EE,$EE,$E1,$11,$B6
        .byte $6E,$EE,$EE,$EE,$EE,$E1,$11,$16
        .byte $6E,$EE,$EE,$EE,$EE,$E1,$11,$16
        .byte $6E,$EE,$EE,$EE,$EE,$E1,$11,$16
        .byte $6E,$EE,$EE,$EE,$EE,$E1,$11,$16
        .byte $6B,$11,$11,$11,$11,$11,$11,$B6
        .byte $6B,$11,$11,$11,$11,$11,$11,$B6
        .byte $66,$B1,$11,$11,$11,$11,$1B,$66
        .byte $66,$6B,$11,$11,$11,$11,$B6,$66
        .byte $66,$66,$BB,$11,$11,$BB,$66,$66
        .byte $66,$66,$66,$66,$66,$66,$66,$66

        .byte $66,$66,$66,$66,$66,$66,$66,$66
        .byte $66,$66,$BB,$CC,$CC,$BB,$66,$66
        .byte $66,$6B,$CC,$CC,$CC,$C0,$B6,$66
        .byte $66,$BC,$CC,$FC,$CC,$C0,$0B,$66
        .byte $6B,$CC,$FF,$FC,$CC,$C0,$00,$B6
        .byte $6B,$CC,$CC,$CC,$CC,$C0,$00,$B6
        .byte $6C,$CC,$CC,$CC,$CC,$C0,$00,$06
        .byte $6C,$CC,$CC,$CC,$CC,$C0,$00,$06
        .byte $6C,$CC,$CC,$CC,$CC,$C0,$00,$06
        .byte $6C,$CC,$CC,$CC,$CC,$C0,$00,$06
        .byte $6B,$00,$00,$00,$00,$00,$00,$B6
        .byte $6B,$00,$00,$00,$00,$00,$00,$B6
        .byte $66,$B0,$00,$00,$00,$00,$0B,$66
        .byte $66,$6B,$00,$00,$00,$00,$B6,$66
        .byte $66,$66,$BB,$00,$00,$BB,$66,$66
        .byte $66,$66,$66,$66,$66,$66,$66,$66

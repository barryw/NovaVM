; sprite_math_verify_asm.s
;
; Sprite-based math coprocessor verifier.
;
; Build:
;   ca65 --cpu 65c02 -I runtime/asm -I ehbasic -o /tmp/sprite_math_verify_asm.o docs/programs/sprite_math_verify_asm.s
;   ld65 -C docs/programs/diag9000_4k.cfg -o /tmp/sprite_math_verify_asm.bin /tmp/sprite_math_verify_asm.o
;
; Load at $9000 and run with: SYS $9000
;
; Visible behavior:
; - 11 sprites are shown.
; - Green/top row means a math test passed.
; - Red/lower row means that test failed.

.setcpu "65c02"

.include "nova.inc"

TEST_COUNT       = 11
SPR_TRANS        = 0
PASS_Y           = 60
FAIL_Y           = 136
SHAPE_PASS       = 0
SHAPE_FAIL       = 1

PTR              = NVR0L
SHAPE_IDX        = NVR1L
ROW_IDX          = NVR1H
TEMP             = NVR2L

.macro SET_PASS index
        lda #$01
        sta test_pass+index
.endmacro

.macro EXPECT_REG reg, value, fail_label
        lda reg
        cmp #value
        bne fail_label
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

        jsr define_shapes
        jsr init_sprites
        jsr clear_results
        jsr run_tests
        jsr publish_results

main_loop:
        jsr wait_frame
        jsr publish_results
        bra main_loop

clear_results:
        ldx #$00
@loop:
        stz test_pass,x
        inx
        cpx #TEST_COUNT
        bne @loop
        rts

run_tests:
        jsr test_caps
        jsr test_mul16
        jsr test_mulfx
        jsr test_sincos
        jsr test_dist_rng
        jsr test_div
        jsr test_atan2
        jsr test_vec_dotfx
        jsr test_vec_dots16
        jsr test_vec_cross
        jsr test_vec_len_scale
        rts

test_caps:
        EXPECT_REG MATH_VERSION, MATH_VERSION_CURRENT, @fail
        EXPECT_REG MATH_CAPS0, $7F, @fail
        EXPECT_REG MATH_CAPS1, $1F, @fail
        SET_PASS 0
@fail:
        rts

test_mul16:
        lda #$2C              ; 300
        sta MATH_MUL16_A_LO
        lda #$01
        sta MATH_MUL16_A_HI
        lda #$04
        sta MATH_MUL16_B_LO
        stz MATH_MUL16_B_HI   ; trigger, 300*4 = $000004B0
        EXPECT_REG MATH_RES0, $B0, @fail
        EXPECT_REG MATH_RES1, $04, @fail
        EXPECT_REG MATH_RES2, $00, @fail
        EXPECT_REG MATH_RES3, $00, @fail
        SET_PASS 1
@fail:
        rts

test_mulfx:
        lda #$80              ; $0180 * $0180 -> $0240
        sta MATH_MULFX_A_LO
        lda #$01
        sta MATH_MULFX_A_HI
        lda #$80
        sta MATH_MULFX_B_LO
        lda #$01
        sta MATH_MULFX_B_HI
        EXPECT_REG MATH_RES0, $40, @fail
        EXPECT_REG MATH_RES1, $02, @fail
        EXPECT_REG MATH_STATUS, MATH_STATUS_OK, @fail
        SET_PASS 2
@fail:
        rts

test_sincos:
        lda #$40
        sta MATH_SINCOS_ANGLE
        EXPECT_REG MATH_RES0, $7F, @fail
        EXPECT_REG MATH_RES1, $00, @fail
        lda #$C0
        sta MATH_SINCOS_ANGLE
        EXPECT_REG MATH_RES0, $80, @fail
        EXPECT_REG MATH_RES1, $00, @fail
        SET_PASS 3
@fail:
        rts

test_dist_rng:
        lda #$03
        sta MATH_DIST_DX_LO
        stz MATH_DIST_DX_HI
        lda #$04
        sta MATH_DIST_DY_LO
        stz MATH_DIST_DY_HI
        EXPECT_REG MATH_RES0, $05, @fail
        EXPECT_REG MATH_RES1, $00, @fail
        lda #$78
        sta MATH_RNG
        lda #$56
        sta MATH_RNG
        lda #$34
        sta MATH_RNG
        lda #$12
        sta MATH_RNG
        EXPECT_REG MATH_RNG, $A5, @fail
        SET_PASS 4
@fail:
        rts

test_div:
        lda #$9C              ; -100 / 7 => -14 rem -2
        sta MATH_DIV_N_LO
        lda #$FF
        sta MATH_DIV_N_1
        sta MATH_DIV_N_2
        sta MATH_DIV_N_HI
        lda #$07
        sta MATH_DIV_D_LO
        stz MATH_DIV_D_HI
        EXPECT_REG MATH_RES0, $F2, @fail
        EXPECT_REG MATH_RES1, $FF, @fail
        EXPECT_REG MATH_RES2, $FE, @fail
        EXPECT_REG MATH_RES3, $FF, @fail
        SET_PASS 5
@fail:
        rts

test_atan2:
        lda #$64              ; dy=100, dx=0 => north, hypot=100
        sta MATH_ATAN_DY_LO
        stz MATH_ATAN_DY_HI
        stz MATH_ATAN_DX_LO
        stz MATH_ATAN_DX_HI
        EXPECT_REG MATH_RES0, $40, @fail
        EXPECT_REG MATH_RES1, $64, @fail
        EXPECT_REG MATH_RES2, $00, @fail
        SET_PASS 6
@fail:
        rts

test_vec_dotfx:
        jsr load_standard_vector
        lda #MATH_VEC_OP_DOT_FX
        sta MATH_VEC_OP
        EXPECT_REG MATH_RES0, $00, @fail
        EXPECT_REG MATH_RES1, $02, @fail
        EXPECT_REG MATH_STATUS, MATH_STATUS_OK, @fail
        SET_PASS 7
@fail:
        rts

test_vec_dots16:
        jsr load_standard_vector
        lda #MATH_VEC_OP_DOT_S16
        sta MATH_VEC_OP
        EXPECT_REG MATH_RES0, $00, @fail
        EXPECT_REG MATH_RES1, $00, @fail
        EXPECT_REG MATH_RES2, $02, @fail
        EXPECT_REG MATH_RES3, $00, @fail
        SET_PASS 8
@fail:
        rts

test_vec_cross:
        jsr load_standard_vector
        lda #MATH_VEC_OP_CROSS_S16
        sta MATH_VEC_OP
        EXPECT_REG MATH_RES0, $00, @fail
        EXPECT_REG MATH_RES1, $80, @fail
        EXPECT_REG MATH_RES2, $FC, @fail
        EXPECT_REG MATH_RES3, $FF, @fail
        SET_PASS 9
@fail:
        rts

test_vec_len_scale:
        jsr load_standard_vector
        lda #MATH_VEC_OP_LEN2
        sta MATH_VEC_OP
        EXPECT_REG MATH_RES0, $00, @fail
        EXPECT_REG MATH_RES1, $40, @fail
        EXPECT_REG MATH_RES2, $03, @fail
        EXPECT_REG MATH_RES3, $00, @fail
        jsr load_standard_vector
        lda #MATH_VEC_OP_SCALE_FX
        sta MATH_VEC_OP
        EXPECT_REG MATH_RES0, $C0, @fail
        EXPECT_REG MATH_RES1, $00, @fail
        EXPECT_REG MATH_RES2, $80, @fail
        EXPECT_REG MATH_RES3, $00, @fail
        SET_PASS 10
@fail:
        rts

load_standard_vector:
        lda #$80
        sta MATH_VEC_AX_LO
        lda #$01
        sta MATH_VEC_AX_HI
        lda #$00
        sta MATH_VEC_AY_LO
        lda #$01
        sta MATH_VEC_AY_HI
        lda #$00
        sta MATH_VEC_BX_LO
        lda #$02
        sta MATH_VEC_BX_HI
        lda #$00
        sta MATH_VEC_BY_LO
        lda #$FF
        sta MATH_VEC_BY_HI
        lda #$80
        sta MATH_VEC_S_LO
        lda #$00
        sta MATH_VEC_S_HI
        rts

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
        cmp #2
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
        beq @row_offset
        lda PTR
        clc
        adc #$80
        sta PTR
        lda PTR+1
        adc #$00
        sta PTR+1
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
        asl
        asl
        asl
        tay
        lda x_init_lo,x
        sta VGC_SPR_BASE + VGC_SPR_XL_OFF,y
        lda x_init_hi,x
        sta VGC_SPR_BASE + VGC_SPR_XH_OFF,y
        lda #FAIL_Y
        sta VGC_SPR_BASE + VGC_SPR_YL_OFF,y
        lda #$00
        sta VGC_SPR_BASE + VGC_SPR_YH_OFF,y
        lda #SHAPE_FAIL
        sta VGC_SPR_BASE + VGC_SPR_SHAPE_OFF,y
        lda #VGC_SPR_FLAG_ENABLE
        sta VGC_SPR_BASE + VGC_SPR_FLAGS_OFF,y
        lda #VGC_SPR_PRI_FRONT
        sta VGC_SPR_BASE + VGC_SPR_PRI_OFF,y
        lda #SPR_TRANS
        sta VGC_SPR_BASE + VGC_SPR_TRANS_OFF,y
        inx
        cpx #TEST_COUNT
        bne @loop
        rts

publish_results:
        ldx #$00
@loop:
        txa
        asl
        asl
        asl
        tay
        lda test_pass,x
        beq @fail
        lda #PASS_Y
        sta VGC_SPR_BASE + VGC_SPR_YL_OFF,y
        lda #SHAPE_PASS
        bra @shape
@fail:
        lda #FAIL_Y
        sta VGC_SPR_BASE + VGC_SPR_YL_OFF,y
        lda #SHAPE_FAIL
@shape:
        sta VGC_SPR_BASE + VGC_SPR_SHAPE_OFF,y
        inx
        cpx #TEST_COUNT
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

x_init_lo:
        .byte <10,<38,<66,<94,<122,<150,<178,<206,<234,<262,<290
x_init_hi:
        .byte >10,>38,>66,>94,>122,>150,>178,>206,>234,>262,>290

shape_data:
        ; pass, green
        .byte $00,$00,$AA,$AA,$AA,$AA,$00,$00
        .byte $00,$AA,$AA,$AA,$AA,$AA,$AA,$00
        .byte $0A,$AA,$AA,$AA,$AA,$AA,$AA,$A0
        .byte $0A,$AA,$AA,$AA,$AA,$AA,$AA,$A0
        .byte $AA,$AA,$0A,$AA,$AA,$A0,$AA,$AA
        .byte $AA,$A0,$0A,$AA,$AA,$A0,$0A,$AA
        .byte $AA,$00,$0A,$AA,$AA,$A0,$00,$AA
        .byte $AA,$00,$00,$AA,$AA,$00,$00,$AA
        .byte $AA,$00,$00,$AA,$AA,$00,$00,$AA
        .byte $AA,$00,$0A,$AA,$AA,$A0,$00,$AA
        .byte $AA,$A0,$0A,$AA,$AA,$A0,$0A,$AA
        .byte $AA,$AA,$0A,$AA,$AA,$A0,$AA,$AA
        .byte $0A,$AA,$AA,$AA,$AA,$AA,$AA,$A0
        .byte $0A,$AA,$AA,$AA,$AA,$AA,$AA,$A0
        .byte $00,$AA,$AA,$AA,$AA,$AA,$AA,$00
        .byte $00,$00,$AA,$AA,$AA,$AA,$00,$00
        ; fail, red
        .byte $00,$00,$22,$22,$22,$22,$00,$00
        .byte $00,$22,$22,$22,$22,$22,$22,$00
        .byte $02,$22,$22,$22,$22,$22,$22,$20
        .byte $02,$22,$02,$22,$22,$20,$22,$20
        .byte $22,$20,$00,$22,$22,$00,$02,$22
        .byte $22,$22,$00,$22,$22,$00,$22,$22
        .byte $22,$22,$20,$02,$20,$02,$22,$22
        .byte $22,$22,$22,$00,$00,$22,$22,$22
        .byte $22,$22,$22,$00,$00,$22,$22,$22
        .byte $22,$22,$20,$02,$20,$02,$22,$22
        .byte $22,$22,$00,$22,$22,$00,$22,$22
        .byte $22,$20,$00,$22,$22,$00,$02,$22
        .byte $02,$22,$02,$22,$22,$20,$22,$20
        .byte $02,$22,$22,$22,$22,$22,$22,$20
        .byte $00,$22,$22,$22,$22,$22,$22,$00
        .byte $00,$00,$22,$22,$22,$22,$00,$00

test_pass:
        .res TEST_COUNT

; VSPRITE scene work-address integration fixture.
;
; Drives vsprite_scene_calc_work_addr so EditbufAssemblyRuntimeTests' sibling
; VspriteAssemblyRuntimeTests can assert the computed 24-bit work-buffer
; destination address. The key case is a sprite positioned ABOVE the scene's
; dirty-rect top (VSPRITE_Y < VSPRITE_SCENE_Y): the row delta must not underflow
; into a ~256-row stride walk that runs the destination out of the work buffer.

.setcpu "65c02"

.include "vsprite.inc"

RESULT = $2200

.segment "CODE"

start:
        jmp run

run:
        ; --- case A: sprite above the dirty rect (Y=10 < SCENE_Y=20) ----------
        ; work buffer base $010000, row stride 64. Result must stay at the base.
        jsr set_work_buffer
        lda #20
        sta VSPRITE_SCENE_Y
        lda #10
        sta VSPRITE_Y
        jsr clear_x
        jsr vsprite_scene_calc_work_addr
        lda VSPRITE_DSTADDRL
        sta RESULT + 0
        lda VSPRITE_DSTADDRM
        sta RESULT + 1
        lda VSPRITE_DSTADDRH
        sta RESULT + 2

        ; --- case B: normal (Y=13 >= SCENE_Y=10), 3 rows down ----------------
        jsr set_work_buffer
        lda #10
        sta VSPRITE_SCENE_Y
        lda #13
        sta VSPRITE_Y
        jsr clear_x
        jsr vsprite_scene_calc_work_addr
        lda VSPRITE_DSTADDRL
        sta RESULT + 3
        lda VSPRITE_DSTADDRM
        sta RESULT + 4
        lda VSPRITE_DSTADDRH
        sta RESULT + 5

        lda #$AA
        sta RESULT + 6
        rts

set_work_buffer:
        stz VSPRITE_SCENE_WORKADDRL
        stz VSPRITE_SCENE_WORKADDRM
        lda #$01
        sta VSPRITE_SCENE_WORKADDRH
        lda #64
        sta VSPRITE_SCENE_WORKSTRL
        stz VSPRITE_SCENE_WORKSTRH
        rts

clear_x:
        stz VSPRITE_SCENE_XL
        stz VSPRITE_SCENE_XH
        stz VSPRITE_XL
        stz VSPRITE_XH
        rts

.include "vsprite.s"

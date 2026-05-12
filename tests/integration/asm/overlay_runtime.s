; Fixed-address overlay manager integration fixture.
;
; This fixture does not exercise FIO loading. It verifies the resident overlay
; metadata and entry-call ABI that loaded fixed-address overlays use.

.setcpu "65c02"

.include "overlay.inc"

RESULT_BASE = $2290

      .segment "CODE"

start:
        jmp asm_overlay_entry

asm_overlay_entry:
        pha
        phx
        phy
        jsr asm_overlay_test
        ply
        plx
        pla
        rts

asm_overlay_test:
        jsr clear_results
        jsr overlay_clear_active

        lda #$01
        sta OVL_LOADED
        lda #<overlay_init_ok
        sta OVL_ENTRY_INITL
        lda #>overlay_init_ok
        sta OVL_ENTRY_INITH
        jsr overlay_call_init
        sta RESULT_BASE
        lda OVL_RESULT
        sta RESULT_BASE + 1
        lda overlay_init_count
        sta RESULT_BASE + 2

        lda #<overlay_main_ok
        sta OVL_ENTRY_MAINL
        lda #>overlay_main_ok
        sta OVL_ENTRY_MAINH
        jsr overlay_call_main
        sta RESULT_BASE + 3
        lda OVL_RESULT
        sta RESULT_BASE + 4
        lda overlay_main_count
        sta RESULT_BASE + 5

        stz OVL_ENTRY_MAINL
        stz OVL_ENTRY_MAINH
        jsr overlay_call_main
        sta RESULT_BASE + 6
        lda OVL_RESULT
        sta RESULT_BASE + 7

        lda #<overlay_tick_error
        sta OVL_ENTRY_TICKL
        lda #>overlay_tick_error
        sta OVL_ENTRY_TICKH
        jsr overlay_call_tick
        sta RESULT_BASE + 8
        lda OVL_RESULT
        sta RESULT_BASE + 9

        jsr overlay_clear_active
        jsr overlay_call_tick
        sta RESULT_BASE + 10
        lda OVL_RESULT
        sta RESULT_BASE + 11

        lda #$AA
        sta RESULT_BASE + $3F
        rts

overlay_init_ok:
        inc overlay_init_count
        lda #OVL_OK
        rts

overlay_main_ok:
        inc overlay_main_count
        lda #OVL_OK
        rts

overlay_tick_error:
        lda #$7F
        rts

clear_results:
        ldx #$00
        lda #$00
@loop:
        sta RESULT_BASE,x
        inx
        cpx #$40
        bne @loop
        stz overlay_init_count
        stz overlay_main_count
        rts

overlay_init_count:
        .byte $00
overlay_main_count:
        .byte $00

.include "overlay.s"

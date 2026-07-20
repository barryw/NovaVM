; Inline-parameter NDK integration fixture.
;
; Exercises I_VTEXT_PRINT_AT, I_VTEXT_PUTS, and I_FIO_NAME. The C# test models
; VGC VRAM and checks both rendered text and the FIO filename mailbox.

.setcpu "w65c02"

.include "vtext.inc"
.include "fio.inc"

RESULT       = $2200
REGION_TABLE = $2400

.segment "CODE"

start:
        jmp run

run:
        jsr set_table
        stz VTEXT_LEFT
        stz VTEXT_TOP
        lda #80
        sta VTEXT_WIDTH
        lda #50
        sta VTEXT_HEIGHT
        stz VTEXT_CURX
        stz VTEXT_CURY
        lda #$0F
        sta VTEXT_COLOR
        stz VTEXT_ATTR
        stz VTEXT_FLAGS
        stz VTEXT_REGION_ID
        jsr vtext_define_region

        jsr set_table
        stz VTEXT_REGION_ID
        jsr vtext_select_region

        jsr i_vtext_print_at
        .byte 3, 2, "HI", 0
        sta RESULT + 0

        jsr i_vtext_puts
        .byte "!", 0
        sta RESULT + 1

        jsr i_fio_name
        .byte "CHESSENG", 0
        sta RESULT + 2

        lda #$AA
        sta RESULT + 3
        rts

set_table:
        lda #<REGION_TABLE
        sta VTEXT_TABLEL
        lda #>REGION_TABLE
        sta VTEXT_TABLEH
        rts

.include "vtext.s"
.include "vtext_inline.s"
.include "blitter.s"
.include "fio_inline.s"
.include "fio.s"

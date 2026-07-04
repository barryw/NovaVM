; VTEXT.PRINT_AT integration fixture.
;
; Defines a full-screen region, then prints a string at (col 3, row 2) through
; the reusable vtext_print_at primitive. VtextPrintAssemblyRuntimeTests supplies
; a VgcVramBus that models the VGC VRAM plane interface, so the test can read the
; character plane back and confirm the string landed at the right cells.

.setcpu "w65c02"

.include "vtext.inc"

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

        lda #3
        sta VTEXT_CURX
        lda #2
        sta VTEXT_CURY
        lda #<msg
        ldy #>msg
        jsr vtext_print_at
        sta RESULT + 0              ; A = return code (0 = ok)

        lda #$AA
        sta RESULT + 1
        rts

set_table:
        lda #<REGION_TABLE
        sta VTEXT_TABLEL
        lda #>REGION_TABLE
        sta VTEXT_TABLEH
        rts

msg:
        .byte "HI", 0

.include "vtext.s"
.include "blitter.s"

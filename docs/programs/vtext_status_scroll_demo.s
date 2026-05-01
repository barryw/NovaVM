; vtext_status_scroll_demo.s
;
; Demonstrates a fixed inverse status line with a scrolling text region below.
;
; Build:
;   ca65 --cpu 65c02 -I ehbasic -o /tmp/vtext_demo.o docs/programs/vtext_status_scroll_demo.s
;   ld65 -C docs/programs/diag9000.cfg -o /tmp/vtext_demo.bin /tmp/vtext_demo.o

.setcpu "65c02"

.include "lib/vtext.inc"

REGION_TABLE = $2400
STR_INDEX = $23FF

.segment "CODE"

start:
        jsr set_table

        ; Status line, row 0, inverse black-on-white.
        stz VTEXT_LEFT
        stz VTEXT_TOP
        lda #80
        sta VTEXT_WIDTH
        lda #1
        sta VTEXT_HEIGHT
        stz VTEXT_CURX
        stz VTEXT_CURY
        lda #$F0
        sta VTEXT_COLOR
        stz VTEXT_ATTR
        stz VTEXT_FLAGS
        stz VTEXT_REGION_ID
        jsr vtext_define_region

        ; Story body, rows 1-49, normal white-on-black with wrap+scroll.
        stz VTEXT_LEFT
        lda #1
        sta VTEXT_TOP
        lda #80
        sta VTEXT_WIDTH
        lda #49
        sta VTEXT_HEIGHT
        stz VTEXT_CURX
        stz VTEXT_CURY
        lda #$0F
        sta VTEXT_COLOR
        stz VTEXT_ATTR
        lda #(VTEXT_FLAG_WRAP | VTEXT_FLAG_SCROLL)
        sta VTEXT_FLAGS
        lda #1
        sta VTEXT_REGION_ID
        jsr vtext_define_region

        jsr set_table
        stz VTEXT_REGION_ID
        jsr vtext_select_region
        jsr vtext_clear_region
        lda #<status_text
        sta VTEXT_TABLEL
        lda #>status_text
        sta VTEXT_TABLEH
        jsr write_string

        jsr set_table
        lda #1
        sta VTEXT_REGION_ID
        jsr vtext_select_region
        jsr vtext_clear_region
        lda #<story_text
        sta VTEXT_TABLEL
        lda #>story_text
        sta VTEXT_TABLEH
        jsr write_string
        rts

set_table:
        lda #<REGION_TABLE
        sta VTEXT_TABLEL
        lda #>REGION_TABLE
        sta VTEXT_TABLEH
        rts

write_string:
        stz STR_INDEX
@loop:
        ldy STR_INDEX
        lda (VTEXT_TABLEL),y
        beq @done
        sta VTEXT_CHAR
        jsr vtext_put_char
        bne @done
        inc STR_INDEX
        bne @loop
@done:
        rts

status_text:
        .byte "WEST OF HOUSE                                           SCORE: 0 MOVES: 1",0

story_text:
        .byte "LINE 1",$0A
        .byte "LINE 2",$0A
        .byte "LINE 3",$0A
        .byte "LINE 4",$0A
        .byte "LINE 5",$0A,0

.include "lib/vtext.s"

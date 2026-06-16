; EDITBUF shared-runtime integration fixture.
;
; Drives editbuf_parse_goto_line and editbuf_cut on the flat BusDevice used by
; EditbufAssemblyRuntimeTests, so buffer / clipboard / line-number results can
; be asserted from C#. The fixture includes the real editbuf/editui/vtext stack
; a standalone program would link against.
;
; Two fixed entry points (the C# harness JSRs each independently):
;   goto_entry ($4000): parse goto-line strings -> [validity, lineL, lineH] x N
;   cut_entry  ($4003): cut an all-selected 300-byte buffer -> len + cliplen

.setcpu "65c02"

.include "editbuf.inc"

GOTO_RESULT = $2200
CUT_RESULT  = $2240

.segment "ZEROPAGE"

src_ptr_l:  .res 1
src_ptr_h:  .res 1
cnt_l:      .res 1
cnt_h:      .res 1

.segment "CODE"

goto_entry:
        jmp run_goto
cut_entry:
        jmp run_cut

; ---------------------------------------------------------------------
; goto-line: parse four strings, record validity + 16-bit line result.
; "1"->0, "5"->4, "256"->255, "1000"->999 (results are 0-based).
; ---------------------------------------------------------------------
run_goto:
        lda #<g_one
        ldy #>g_one
        ldx #1
        jsr load_prompt
        jsr editbuf_parse_goto_line
        ldx #0
        jsr store_goto

        lda #<g_five
        ldy #>g_five
        ldx #1
        jsr load_prompt
        jsr editbuf_parse_goto_line
        ldx #3
        jsr store_goto

        lda #<g_256
        ldy #>g_256
        ldx #3
        jsr load_prompt
        jsr editbuf_parse_goto_line
        ldx #6
        jsr store_goto

        lda #<g_1000
        ldy #>g_1000
        ldx #4
        jsr load_prompt
        jsr editbuf_parse_goto_line
        ldx #9
        jsr store_goto

        lda #$AA
        sta GOTO_RESULT + $1F
        rts

; A=lo, Y=hi of digit string; X=length -> EB_PROMPTBUF / EB_PROMPTLEN
load_prompt:
        sta src_ptr_l
        sty src_ptr_h
        stx EB_PROMPTLEN
        ldy #0
@lp:    cpy EB_PROMPTLEN
        beq @done
        lda (src_ptr_l),y
        sta EB_PROMPTBUF,y
        iny
        bra @lp
@done:  rts

; X = byte offset into GOTO_RESULT; carry = parse validity; EB_SCRATCH = line.
store_goto:
        lda #0
        bcc @nc
        lda #1
@nc:    sta GOTO_RESULT,x
        lda EB_SCRATCHL
        sta GOTO_RESULT + 1,x
        lda EB_SCRATCHH
        sta GOTO_RESULT + 2,x
        rts

; ---------------------------------------------------------------------
; cut: fill a 300-byte buffer, select all, cut. A cut must never delete
; more text than it places on the (256-byte) clipboard.
; ---------------------------------------------------------------------
run_cut:
        lda #<testbuf
        sta EDITBUF_BUFL
        sta src_ptr_l
        lda #>testbuf
        sta EDITBUF_BUFH
        sta src_ptr_h
        lda #<512
        sta EDITBUF_CAPL
        lda #>512
        sta EDITBUF_CAPH

        lda #<300
        sta cnt_l
        lda #>300
        sta cnt_h
@fill:  lda cnt_l
        ora cnt_h
        beq @filled
        lda #'A'
        ldy #0
        sta (src_ptr_l),y
        inc src_ptr_l
        bne @nofc
        inc src_ptr_h
@nofc:  lda cnt_l
        bne @decc
        dec cnt_h
@decc:  dec cnt_l
        bra @fill
@filled:
        lda #<300
        sta EDITBUF_LENL
        lda #>300
        sta EDITBUF_LENH
        ; point optional editor hooks at a harmless RTS and the title/type/
        ; status strings at "" so the post-edit repaint runs on the headless
        ; bus without jumping through uninitialised ($0000) vectors.
        lda #<safe_rts
        sta EDITBUF_SAVE_VECL
        sta EDITBUF_INDENT_VECL
        sta EDITBUF_HILITE_VECL
        sta EDITBUF_MENU_VECL
        lda #>safe_rts
        sta EDITBUF_SAVE_VECH
        sta EDITBUF_INDENT_VECH
        sta EDITBUF_HILITE_VECH
        sta EDITBUF_MENU_VECH
        lda #<empty_str
        sta EDITBUF_TITLEL
        sta EDITBUF_TYPEL
        sta EDITBUF_STATUSL
        lda #>empty_str
        sta EDITBUF_TITLEH
        sta EDITBUF_TYPEH
        sta EDITBUF_STATUSH
        stz EB_SELL
        stz EB_SELH
        lda EDITBUF_LENL
        sta EDITBUF_CURL
        lda EDITBUF_LENH
        sta EDITBUF_CURH
        lda #1
        sta EDITBUF_SELACT
        jsr editbuf_cut

        lda EDITBUF_LENL
        sta CUT_RESULT
        lda EDITBUF_LENH
        sta CUT_RESULT + 1
        lda EDITBUF_CLIPLENL
        sta CUT_RESULT + 2
        lda EDITBUF_CLIPLENH
        sta CUT_RESULT + 3
        lda #$AA
        sta CUT_RESULT + 4
        rts

safe_rts:
        rts

.segment "RODATA"

empty_str:
        .byte 0
g_one:   .byte "1"
g_five:  .byte "5"
g_256:   .byte "256"
g_1000:  .byte "1000"

.segment "BSS"

testbuf: .res 512

.include "editbuf.s"
.include "editui.s"
.include "blitter.s"
.include "xram.s"

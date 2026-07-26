; EDITBUF shared-runtime integration fixture.
;
; Drives editbuf_parse_goto_line and editbuf_cut on the flat BusDevice used by
; EditbufAssemblyRuntimeTests, so buffer / clipboard / line-number results can
; be asserted from C#. The fixture includes the real editbuf/editui/vtext stack
; a standalone program would link against.
;
; Three fixed entry points (the C# harness JSRs each independently):
;   goto_entry ($4000): parse goto-line strings -> [validity, lineL, lineH] x N
;   cut_entry  ($4003): cut an all-selected 300-byte buffer -> len + cliplen
;   undo_entry ($4006): block-indent a 1700-byte document and undo it, then
;                       type UNDO_STEPS characters, undo them all and redo them
;                       all -> lengths + byte-for-byte verdicts

.setcpu "w65c02"

.include "editbuf.inc"

GOTO_RESULT = $2200
CUT_RESULT  = $2240
UNDO_RESULT = $2260
UNDO_TEXT   = 1700       ; starting document size, bytes
UNDO_STEPS  = 40         ; edits to make, undo, and redo

.segment "ZEROPAGE"

src_ptr_l:  .res 1
src_ptr_h:  .res 1
cnt_l:      .res 1
cnt_h:      .res 1
tmp_b:      .res 1
tmp_c:      .res 1

.segment "CODE"

goto_entry:
        jmp run_goto
cut_entry:
        jmp run_cut
undo_entry:
        jmp run_undo

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
; more text than it places on the shared XRAM-backed clipboard.
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
        sta EDITBUF_COMMAND_VECL
        sta EDITBUF_CHANGED_VECL
        lda #>safe_rts
        sta EDITBUF_SAVE_VECH
        sta EDITBUF_INDENT_VECH
        sta EDITBUF_HILITE_VECH
        sta EDITBUF_MENU_VECH
        sta EDITBUF_COMMAND_VECH
        sta EDITBUF_CHANGED_VECH
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
        lda TEXTSVC_CLIPLENL
        sta CUT_RESULT + 2
        lda TEXTSVC_CLIPLENH
        sta CUT_RESULT + 3
        lda #$AA
        sta CUT_RESULT + 4
        rts

; ---------------------------------------------------------------------
; undo: depth, and that a compound edit is still one step.
;
; The document is 1700 bytes, so 40 edits would be 68 KB of whole-buffer
; snapshots. No arena could have held that: every stack offset in textsvc.s is
; 16-bit, so the arena cannot reach 64 KB in the first place. As deltas the
; same history is about 600 bytes.
;
; UNDO_RESULT+0,1  length after undoing every keystroke   (want UNDO_TEXT)
; UNDO_RESULT+2    1 when the text matches the original byte for byte
; UNDO_RESULT+3,4  length after redoing them all          (want UNDO_TEXT+STEPS)
; UNDO_RESULT+5    1 when the redone text matches what was typed
; UNDO_RESULT+6,7  length after indenting every line      (want > UNDO_TEXT)
; UNDO_RESULT+8    1 when ONE undo took the whole indent back off
; UNDO_RESULT+9    sentinel
; ---------------------------------------------------------------------
run_undo:
        jsr undo_setup

        ; A block indent is one operation made of two gap moves per line. It
        ; has to commit as a single record, or Ctrl-Z would peel it off a
        ; space at a time.
        stz EB_SELL
        stz EB_SELH
        lda EDITBUF_LENL
        sta EDITBUF_CURL
        lda EDITBUF_LENH
        sta EDITBUF_CURH
        lda #1
        sta EDITBUF_SELACT
        jsr editbuf_indent_block
        lda EDITBUF_LENL
        sta UNDO_RESULT+6
        lda EDITBUF_LENH
        sta UNDO_RESULT+7
        jsr editbuf_do_undo
        jsr undo_compare_original
        sta UNDO_RESULT+8

        ; Depth: type UNDO_STEPS characters mid-document, each a distinct
        ; value so a misplaced splice cannot pass by accident.
        lda #<(UNDO_TEXT/2)
        sta EDITBUF_CURL
        lda #>(UNDO_TEXT/2)
        sta EDITBUF_CURH
        lda #0
        sta cnt_l
@type:  lda cnt_l
        cmp #UNDO_STEPS
        beq @typed
        clc
        adc #'a'
        jsr editbuf_insert_char
        inc cnt_l
        bra @type
@typed:
        lda #UNDO_STEPS
        sta cnt_l
@undo:  lda cnt_l
        beq @undone
        jsr editbuf_do_undo
        dec cnt_l
        bra @undo
@undone:
        lda EDITBUF_LENL
        sta UNDO_RESULT+0
        lda EDITBUF_LENH
        sta UNDO_RESULT+1
        jsr undo_compare_original
        sta UNDO_RESULT+2
        lda #UNDO_STEPS
        sta cnt_l
@redo:  lda cnt_l
        beq @redone
        jsr editbuf_do_redo
        dec cnt_l
        bra @redo
@redone:
        lda EDITBUF_LENL
        sta UNDO_RESULT+3
        lda EDITBUF_LENH
        sta UNDO_RESULT+4
        jsr undo_compare_typed
        sta UNDO_RESULT+5
        lda #$AA
        sta UNDO_RESULT+9
        rts

; The original document: a newline every 16 bytes so the block indent has
; lines to work on, cursor parked in the middle.
undo_setup:
        jsr undo_common_config
        lda #<UNDO_TEXT
        sta EDITBUF_LENL
        lda #>UNDO_TEXT
        sta EDITBUF_LENH
        stz cnt_l
        stz cnt_h
@fill:  lda cnt_l
        cmp #<UNDO_TEXT
        bne @store
        lda cnt_h
        cmp #>UNDO_TEXT
        beq @done
@store: jsr undo_ptr_from_cnt
        jsr undo_expect_original
        ldy #0
        sta (src_ptr_l),y
        inc cnt_l
        bne @fill
        inc cnt_h
        bra @fill
@done:  jsr editbuf_reset_state
        lda #<(UNDO_TEXT/2)
        sta EDITBUF_CURL
        lda #>(UNDO_TEXT/2)
        sta EDITBUF_CURH
        rts

; src_ptr = testbuf + cnt
undo_ptr_from_cnt:
        clc
        lda #<testbuf
        adc cnt_l
        sta src_ptr_l
        lda #>testbuf
        adc cnt_h
        sta src_ptr_h
        rts

; A = the original document's byte at offset cnt.
undo_expect_original:
        lda cnt_l
        and #15
        cmp #15
        beq @eol
        clc
        adc #'A'
        rts
@eol:   lda #$0A
        rts

; A = 1 when the buffer is the original document again.
undo_compare_original:
        lda EDITBUF_LENL
        cmp #<UNDO_TEXT
        bne @bad
        lda EDITBUF_LENH
        cmp #>UNDO_TEXT
        bne @bad
        stz cnt_l
        stz cnt_h
@byte:  lda cnt_l
        cmp #<UNDO_TEXT
        bne @check
        lda cnt_h
        cmp #>UNDO_TEXT
        beq @ok
@check: jsr undo_ptr_from_cnt
        ldy #0
        lda (src_ptr_l),y
        sta tmp_b
        jsr undo_expect_original
        cmp tmp_b
        bne @bad
        inc cnt_l
        bne @byte
        inc cnt_h
        bra @byte
@ok:    lda #1
        rts
@bad:   lda #0
        rts

; A = 1 when the buffer holds the original with the typed run spliced back in.
undo_compare_typed:
        stz cnt_l
        stz cnt_h
@byte:  lda cnt_l
        cmp #<(UNDO_TEXT+UNDO_STEPS)
        bne @check
        lda cnt_h
        cmp #>(UNDO_TEXT+UNDO_STEPS)
        beq @ok
@check: jsr undo_ptr_from_cnt
        ldy #0
        lda (src_ptr_l),y
        sta tmp_b
        jsr undo_expect_typed
        cmp tmp_b
        bne @bad
        inc cnt_l
        bne @byte
        inc cnt_h
        bra @byte
@ok:    lda #1
        rts
@bad:   lda #0
        rts

; Expected byte at offset cnt of the typed document: the original text with
; UNDO_STEPS letters inserted at UNDO_TEXT/2.
undo_expect_typed:
        lda cnt_l
        cmp #<(UNDO_TEXT/2)
        lda cnt_h
        sbc #>(UNDO_TEXT/2)
        bcs :+
        jmp undo_expect_original       ; before the insertion point
:       sec
        lda cnt_l
        sbc #<(UNDO_TEXT/2)
        sta tmp_c
        lda cnt_h
        sbc #>(UNDO_TEXT/2)
        bne @shifted
        lda tmp_c
        cmp #UNDO_STEPS
        bcs @shifted
        clc
        adc #'a'                       ; inside the typed run
        rts
@shifted:
        ; past the run: the original byte from cnt - UNDO_STEPS
        lda cnt_l
        pha
        lda cnt_h
        pha
        sec
        lda cnt_l
        sbc #UNDO_STEPS
        sta cnt_l
        lda cnt_h
        sbc #0
        sta cnt_h
        jsr undo_expect_original
        sta tmp_c
        pla
        sta cnt_h
        pla
        sta cnt_l
        lda tmp_c
        rts

undo_common_config:
        lda #<testbuf
        sta EDITBUF_BUFL
        lda #>testbuf
        sta EDITBUF_BUFH
        lda #<2048
        sta EDITBUF_CAPL
        lda #>2048
        sta EDITBUF_CAPH
        lda #<safe_rts
        sta EDITBUF_SAVE_VECL
        sta EDITBUF_INDENT_VECL
        sta EDITBUF_HILITE_VECL
        sta EDITBUF_MENU_VECL
        sta EDITBUF_COMMAND_VECL
        sta EDITBUF_CHANGED_VECL
        lda #>safe_rts
        sta EDITBUF_SAVE_VECH
        sta EDITBUF_INDENT_VECH
        sta EDITBUF_HILITE_VECH
        sta EDITBUF_MENU_VECH
        sta EDITBUF_COMMAND_VECH
        sta EDITBUF_CHANGED_VECH
        lda #<empty_str
        sta EDITBUF_TITLEL
        sta EDITBUF_TYPEL
        sta EDITBUF_STATUSL
        lda #>empty_str
        sta EDITBUF_TITLEH
        sta EDITBUF_TYPEH
        sta EDITBUF_STATUSH
        stz EDITBUF_SELACT
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

testbuf: .res 2048

.include "editbuf.s"
.include "editui.s"
.include "nui_state.s"
.include "nui_dialog.s"
.include "nui_input.s"
.include "nui_text.s"
.include "textsvc.s"
.include "vtext.s"
.include "blitter.s"
.include "xram.s"
.include "dma.s"
.include "fio.s"

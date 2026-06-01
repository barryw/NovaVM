; editbuf.s — shared text-editing engine for Nova native tools.
; See editbuf.inc for the public ABI and hook contracts. This module is
; language-neutral; all host behavior arrives through the hook vectors.

.include "editbuf.inc"
.include "editui.inc"

.ifndef EDITBUF_IMPLEMENTATION_INCLUDED
EDITBUF_IMPLEMENTATION_INCLUDED = 1

; =====================================================================
; ZEROPAGE — editor working pointers
; =====================================================================
      .segment "ZEROPAGE"

EB_SRCL:        .res 1
EB_SRCH:        .res 1
EB_DSTL:        .res 1
EB_DSTH:        .res 1
EDITBUF_CNTL:   .res 1
EDITBUF_CNTH:   .res 1
EB_PL:          .res 1
EB_PH:          .res 1
EDITBUF_HL_PTR: .res 2
EB_T0:          .res 1
EB_T1:          .res 1
EB_RUNL:        .res 1       ; VRAM-port run pointer (render_row)
EB_RUNH:        .res 1

; =====================================================================
; BSS — config, state, buffers
; =====================================================================
      .segment "BSS"

; --- config (host-set) ---
EDITBUF_BUFL:       .res 1
EDITBUF_BUFH:       .res 1
EDITBUF_CAPL:       .res 1
EDITBUF_CAPH:       .res 1
EDITBUF_LENL:       .res 1
EDITBUF_LENH:       .res 1
EDITBUF_TITLEL:     .res 1
EDITBUF_TITLEH:     .res 1
EDITBUF_STATUSL:    .res 1
EDITBUF_STATUSH:    .res 1
EDITBUF_SAVE_VECL:  .res 1
EDITBUF_SAVE_VECH:  .res 1
EDITBUF_INDENT_VECL:.res 1
EDITBUF_INDENT_VECH:.res 1
EDITBUF_HILITE_VECL:.res 1
EDITBUF_HILITE_VECH:.res 1
EDITBUF_MENU_VECL:  .res 1
EDITBUF_MENU_VECH:  .res 1

; --- state ---
EDITBUF_RESULT:     .res 1
EDITBUF_CURL:       .res 1       ; cursor byte offset (16-bit)
EDITBUF_CURH:       .res 1
EB_TOPLINEL:        .res 1       ; first visible line (16-bit)
EB_TOPLINEH:        .res 1
EB_LEFTCOL:         .res 1       ; first visible column (h-scroll)
EB_SELL:            .res 1       ; selection anchor offset (16-bit)
EB_SELH:            .res 1
EDITBUF_SELACT:     .res 1       ; selection active flag
EB_GOALCOL:         .res 1       ; sticky column for up/down
EB_CURLINEL:        .res 1       ; computed cursor line (16-bit)
EB_CURLINEH:        .res 1
EB_CURCOL:          .res 1       ; computed cursor column

; --- render / nav scratch ---
EB_ROW:             .res 1
EB_COL:             .res 1
EB_PAINTSTART:      .res 1       ; first column painted by editbuf_render_row (0 = full)
EB_EDITDELTA:       .res 1       ; cols left of cursor where an inline edit's changed run begins
EB_LINEOFFL:        .res 1       ; offset of current line start (16-bit)
EB_LINEOFFH:        .res 1
EB_LINELEN:         .res 1       ; current line length (chars before \n)
EB_VISLINEL:        .res 1       ; line number being rendered (16-bit)
EB_VISLINEH:        .res 1
EB_SELSTARTL:       .res 1       ; normalized selection start (16-bit)
EB_SELSTARTH:       .res 1
EB_SELENDL:         .res 1       ; normalized selection end (16-bit)
EB_SELENDH:         .res 1
EB_DLGSEL:          .res 1       ; dialog selected button
EB_SCRATCHL:        .res 1
EB_SCRATCHH:        .res 1
EB_PREVTOPL:        .res 1       ; scroll snapshot for single-line repaint
EB_PREVTOPH:        .res 1
EB_PREVLEFT:        .res 1
EB_PREVSEL:         .res 1       ; was a selection active before a cursor move
EB_CELLL:           .res 1       ; plane offset of first painted cell (16-bit)
EB_CELLH:           .res 1

; --- HILITE hook scratch ---
EDITBUF_HL_LEN:     .res 1
EDITBUF_HL_COLORS:  .res 80

; --- clipboard ---
EDITBUF_CLIPLENL:   .res 1
EDITBUF_CLIPLENH:   .res 1
EDITBUF_CLIP:       .res EDITBUF_CLIP_CAP

; =====================================================================
; CODE
; =====================================================================
      .segment "CODE"

      .export editbuf_run
      .export editbuf_reset_state
      .export editbuf_dialog3

; ---------------------------------------------------------------------
; editbuf_reset_state — clear cursor/scroll/selection for a new session.
; Leaves the buffer text and EDITBUF_LEN untouched.
; ---------------------------------------------------------------------
editbuf_reset_state:
      STZ   EDITBUF_CURL
      STZ   EDITBUF_CURH
      STZ   EB_TOPLINEL
      STZ   EB_TOPLINEH
      STZ   EB_LEFTCOL
      STZ   EB_SELL
      STZ   EB_SELH
      STZ   EDITBUF_SELACT
      STZ   EB_GOALCOL
      RTS

; ---------------------------------------------------------------------
; editbuf_run — run the modal editor. Returns A = EDITBUF_RESULT.
; ---------------------------------------------------------------------
editbuf_run:
      JSR   editbuf_init_vectors
      JSR   editui_init
      JSR   editbuf_call_menu          ; host customizes the menu for its runtime

      ; Wire the EDITUI chrome to our title / help / status.
      LDA   EDITBUF_TITLEL
      STA   EDITUI_TITLEL
      LDA   EDITBUF_TITLEH
      STA   EDITUI_TITLEH
      LDA   #<editbuf_help_text
      STA   EDITUI_HELPL
      LDA   #>editbuf_help_text
      STA   EDITUI_HELPH
      JSR   editbuf_apply_status

      ; Derive line/col + scroll from the host-provided initial cursor offset so
      ; the first render places the cursor correctly (EDITBUF_CUR may be nonzero;
      ; reset_state does not recompute EB_CURLINE/EB_CURCOL).
      JSR   editbuf_compute_linecol
      JSR   editbuf_adjust_scroll

      JSR   editui_draw_shell
      JSR   editbuf_render

@loop:
      LDA   VGC_CHARIN
      BEQ   @loop
      JSR   editbuf_dispatch_key
      BCC   @loop
      LDA   EDITBUF_RESULT
      RTS

; editbuf_dispatch_key — handle key in A. Returns carry set to exit the editor.
editbuf_dispatch_key:
      CMP   #EDITUI_KEY_CTRL_S
      BNE   :+
      JSR   editbuf_do_save
      CLC
      RTS
:     CMP   #EDITUI_KEY_CTRL_Q
      BNE   :+
      JMP   editbuf_do_quit          ; propagates carry (set = exit)
:     CMP   #EDITUI_KEY_CTRL_C
      BNE   :+
      JSR   editbuf_copy
      CLC
      RTS
:     CMP   #EDITUI_KEY_CTRL_X
      BNE   :+
      JSR   editbuf_cut
      CLC
      RTS
:     CMP   #EDITUI_KEY_CTRL_V
      BNE   :+
      JSR   editbuf_paste
      CLC
      RTS
:     CMP   #EDITUI_KEY_CTRL_A
      BNE   :+
      JSR   editbuf_select_all
      CLC
      RTS
:     CMP   #EDITUI_KEY_LEFT
      BNE   :+
      JSR   editbuf_move_left
      CLC
      RTS
:     CMP   #EDITUI_KEY_RIGHT
      BNE   :+
      JSR   editbuf_move_right
      CLC
      RTS
:     CMP   #EDITUI_KEY_UP
      BNE   :+
      JSR   editbuf_move_up
      CLC
      RTS
:     CMP   #EDITUI_KEY_DOWN
      BNE   :+
      JSR   editbuf_move_down
      CLC
      RTS
:     CMP   #EDITUI_KEY_HOME
      BNE   :+
      JSR   editbuf_move_home
      CLC
      RTS
:     CMP   #EDITUI_KEY_END
      BNE   :+
      JSR   editbuf_move_end
      CLC
      RTS
:     CMP   #EDITUI_KEY_PGUP
      BNE   :+
      JSR   editbuf_page_up
      CLC
      RTS
:     CMP   #EDITUI_KEY_PGDN
      BNE   :+
      JSR   editbuf_page_down
      CLC
      RTS
:     CMP   #EDITUI_KEY_ENTER
      BNE   :+
      JSR   editbuf_newline
      CLC
      RTS
:     CMP   #EDITUI_KEY_BACKSPACE
      BEQ   @bs
      CMP   #$14
      BNE   :+
@bs:  JSR   editbuf_backspace
      CLC
      RTS
:     CMP   #EDITUI_KEY_DELETE
      BNE   :+
      JSR   editbuf_delete
      CLC
      RTS
:     CMP   #$20
      BCC   @ignore
      CMP   #$7F
      BCS   @ignore
      JSR   editbuf_insert_char
@ignore:
      CLC
      RTS

; ---------------------------------------------------------------------
; editbuf_apply_status — point EDITUI status at host string, or default to the
; key-hint help text (the status bar is where help now lives).
; ---------------------------------------------------------------------
editbuf_apply_status:
      LDA   EDITBUF_STATUSL
      ORA   EDITBUF_STATUSH
      BNE   @host
      LDA   #<editbuf_help_text
      STA   EDITUI_STATUSL
      LDA   #>editbuf_help_text
      STA   EDITUI_STATUSH
      RTS
@host:
      LDA   EDITBUF_STATUSL
      STA   EDITUI_STATUSL
      LDA   EDITBUF_STATUSH
      STA   EDITUI_STATUSH
      RTS

; =====================================================================
; 16-bit pointer helpers
; =====================================================================

; EB_PL/H = EDITBUF_BUF + (A:X)  where X=lo, A=hi offset
; Input: X=offset lo, A=offset hi
editbuf_ptr_from_off:
      PHA
      TXA
      CLC
      ADC   EDITBUF_BUFL
      STA   EB_PL
      PLA
      ADC   EDITBUF_BUFH
      STA   EB_PH
      RTS

; EB_SRC = EDITBUF_BUF + EB_SCRATCH(16)
editbuf_src_from_scratch:
      CLC
      LDA   EDITBUF_BUFL
      ADC   EB_SCRATCHL
      STA   EB_SRCL
      LDA   EDITBUF_BUFH
      ADC   EB_SCRATCHH
      STA   EB_SRCH
      RTS

; =====================================================================
; Buffer mutation: make_gap / close_gap
; =====================================================================

; editbuf_make_gap — open a gap of (EB_CNTL/H) bytes at cursor (EDITBUF_CUR),
; shifting the tail [cur,len) upward. Assumes len+cnt <= cap (checked by caller).
; After: EDITBUF_LEN += cnt. Bytes in the gap are left as-is (caller fills).
editbuf_make_gap:
      ; tailcount = LEN - CUR  -> EB_SCRATCH
      SEC
      LDA   EDITBUF_LENL
      SBC   EDITBUF_CURL
      STA   EB_SCRATCHL
      LDA   EDITBUF_LENH
      SBC   EDITBUF_CURH
      STA   EB_SCRATCHH
      ; if tailcount == 0, nothing to move
      LDA   EB_SCRATCHL
      ORA   EB_SCRATCHH
      BEQ   @grow
      ; src = BUF + (LEN-1); dst = BUF + (LEN-1+cnt); copy backwards tailcount bytes
      ; src offset = LEN-1
      SEC
      LDA   EDITBUF_LENL
      SBC   #1
      STA   EB_T0
      LDA   EDITBUF_LENH
      SBC   #0
      STA   EB_T1
      ; EB_SRC = BUF + (EB_T1:EB_T0)
      CLC
      LDA   EDITBUF_BUFL
      ADC   EB_T0
      STA   EB_SRCL
      LDA   EDITBUF_BUFH
      ADC   EB_T1
      STA   EB_SRCH
      ; dst offset = LEN-1+cnt
      CLC
      LDA   EB_T0
      ADC   EDITBUF_CNTL
      STA   EB_DSTL
      LDA   EB_T1
      ADC   EDITBUF_CNTH
      STA   EB_DSTH
      CLC
      LDA   EDITBUF_BUFL
      ADC   EB_DSTL
      STA   EB_DSTL
      LDA   EDITBUF_BUFH
      ADC   EB_DSTH
      STA   EB_DSTH
@bloop:
      LDY   #0
      LDA   (EB_SRCL),Y
      STA   (EB_DSTL),Y
      ; dec src, dst ptrs
      LDA   EB_SRCL
      BNE   :+
      DEC   EB_SRCH
:     DEC   EB_SRCL
      LDA   EB_DSTL
      BNE   :+
      DEC   EB_DSTH
:     DEC   EB_DSTL
      ; dec tailcount
      LDA   EB_SCRATCHL
      BNE   :+
      DEC   EB_SCRATCHH
:     DEC   EB_SCRATCHL
      LDA   EB_SCRATCHL
      ORA   EB_SCRATCHH
      BNE   @bloop
@grow:
      CLC
      LDA   EDITBUF_LENL
      ADC   EDITBUF_CNTL
      STA   EDITBUF_LENL
      LDA   EDITBUF_LENH
      ADC   EDITBUF_CNTH
      STA   EDITBUF_LENH
      RTS

; editbuf_close_gap — remove (EB_CNTL/H) bytes starting at offset EB_SCRATCH,
; shifting the tail down. After: EDITBUF_LEN -= cnt.
; Input: EB_SCRATCH = start offset; EB_CNT = count to remove.
editbuf_close_gap:
      ; dst = BUF + start ; src = BUF + start + cnt
      LDA   EDITBUF_BUFL
      CLC
      ADC   EB_SCRATCHL
      STA   EB_DSTL
      LDA   EDITBUF_BUFH
      ADC   EB_SCRATCHH
      STA   EB_DSTH
      CLC
      LDA   EB_DSTL
      ADC   EDITBUF_CNTL
      STA   EB_SRCL
      LDA   EB_DSTH
      ADC   EDITBUF_CNTH
      STA   EB_SRCH
      ; movecount = LEN - (start + cnt)
      CLC
      LDA   EB_SCRATCHL
      ADC   EDITBUF_CNTL
      STA   EB_T0
      LDA   EB_SCRATCHH
      ADC   EDITBUF_CNTH
      STA   EB_T1
      SEC
      LDA   EDITBUF_LENL
      SBC   EB_T0
      STA   EB_SCRATCHL          ; reuse scratch as movecount
      LDA   EDITBUF_LENH
      SBC   EB_T1
      STA   EB_SCRATCHH
      LDA   EB_SCRATCHL
      ORA   EB_SCRATCHH
      BEQ   @shrink
@floop:
      LDY   #0
      LDA   (EB_SRCL),Y
      STA   (EB_DSTL),Y
      INC   EB_SRCL
      BNE   :+
      INC   EB_SRCH
:     INC   EB_DSTL
      BNE   :+
      INC   EB_DSTH
:     LDA   EB_SCRATCHL
      BNE   :+
      DEC   EB_SCRATCHH
:     DEC   EB_SCRATCHL
      LDA   EB_SCRATCHL
      ORA   EB_SCRATCHH
      BNE   @floop
@shrink:
      SEC
      LDA   EDITBUF_LENL
      SBC   EDITBUF_CNTL
      STA   EDITBUF_LENL
      LDA   EDITBUF_LENH
      SBC   EDITBUF_CNTH
      STA   EDITBUF_LENH
      RTS

; =====================================================================
; Editing operations
; =====================================================================

; editbuf_insert_char — insert A at cursor (if room). Marks dirty, re-renders.
editbuf_insert_char:
      PHA
      JSR   editbuf_clear_selection
      ; room? LEN < CAP
      JSR   editbuf_has_room
      BCC   @full
      LDA   #1
      STA   EDITBUF_CNTL
      STZ   EDITBUF_CNTH
      JSR   editbuf_make_gap
      ; store char at cursor
      LDX   EDITBUF_CURL
      LDA   EDITBUF_CURH
      JSR   editbuf_ptr_from_off
      PLA
      LDY   #0
      STA   (EB_PL),Y
      ; cursor++
      JSR   editbuf_cursor_inc
      JSR   editbuf_mark_dirty
      LDA   #1                     ; insert: changed run starts one col left of cursor
      STA   EB_EDITDELTA
      JMP   editbuf_after_inline_edit
@full:
      PLA
      RTS

; editbuf_newline — insert a \n at cursor, then auto-indent via hook.
editbuf_newline:
      JSR   editbuf_clear_selection
      JSR   editbuf_has_room
      BCC   @full
      LDA   #1
      STA   EDITBUF_CNTL
      STZ   EDITBUF_CNTH
      JSR   editbuf_make_gap
      LDX   EDITBUF_CURL
      LDA   EDITBUF_CURH
      JSR   editbuf_ptr_from_off
      LDA   #$0A
      LDY   #0
      STA   (EB_PL),Y
      JSR   editbuf_cursor_inc
      JSR   editbuf_mark_dirty
      ; auto-indent: ask hook how many spaces
      JSR   editbuf_call_indent       ; A = count
      TAX
      BEQ   @done
@indent:
      JSR   editbuf_has_room
      BCC   @done
      PHX
      LDA   #1
      STA   EDITBUF_CNTL
      STZ   EDITBUF_CNTH
      JSR   editbuf_make_gap
      LDX   EDITBUF_CURL
      LDA   EDITBUF_CURH
      JSR   editbuf_ptr_from_off
      LDA   #' '
      LDY   #0
      STA   (EB_PL),Y
      JSR   editbuf_cursor_inc
      PLX
      DEX
      BNE   @indent
@done:
      JMP   editbuf_after_change_down   ; newline shifts lines below it down
@full:
      RTS

; editbuf_backspace — delete char before cursor.
editbuf_backspace:
      LDA   EDITBUF_SELACT
      BEQ   @nosel
      JSR   editbuf_delete_selection
      JMP   editbuf_after_change
@nosel:
      LDA   EDITBUF_CURL
      ORA   EDITBUF_CURH
      BEQ   @done                 ; at start
      JSR   editbuf_cursor_dec
      ; peek the byte about to be removed (now at the cursor) to tell a line
      ; join (\n) from an in-line delete.
      LDX   EDITBUF_CURL
      LDA   EDITBUF_CURH
      JSR   editbuf_ptr_from_off
      LDY   #0
      LDA   (EB_PL),Y
      STA   EB_T1                 ; removed char
      ; close gap of 1 at cursor
      LDA   EDITBUF_CURL
      STA   EB_SCRATCHL
      LDA   EDITBUF_CURH
      STA   EB_SCRATCHH
      LDA   #1
      STA   EDITBUF_CNTL
      STZ   EDITBUF_CNTH
      JSR   editbuf_close_gap
      JSR   editbuf_mark_dirty
      LDA   EB_T1
      CMP   #$0A
      BNE   @inline
      JMP   editbuf_after_change_down      ; joined two lines -> vertical repaint
@inline:
      STZ   EB_EDITDELTA                   ; in-line delete: run starts at the cursor
      JMP   editbuf_after_inline_edit
@done:
      RTS

; editbuf_delete — delete char at cursor.
editbuf_delete:
      LDA   EDITBUF_SELACT
      BEQ   @nosel
      JSR   editbuf_delete_selection
      JMP   editbuf_after_change
@nosel:
      ; cursor < len ?
      LDA   EDITBUF_CURL
      CMP   EDITBUF_LENL
      LDA   EDITBUF_CURH
      SBC   EDITBUF_LENH
      BCS   @done                 ; cursor >= len
      ; peek the byte at the cursor to tell a line join (\n) from an in-line delete.
      LDX   EDITBUF_CURL
      LDA   EDITBUF_CURH
      JSR   editbuf_ptr_from_off
      LDY   #0
      LDA   (EB_PL),Y
      STA   EB_T1                 ; removed char
      LDA   EDITBUF_CURL
      STA   EB_SCRATCHL
      LDA   EDITBUF_CURH
      STA   EB_SCRATCHH
      LDA   #1
      STA   EDITBUF_CNTL
      STZ   EDITBUF_CNTH
      JSR   editbuf_close_gap
      JSR   editbuf_mark_dirty
      LDA   EB_T1
      CMP   #$0A
      BNE   @inline
      JMP   editbuf_after_change_down      ; deleted a newline -> vertical repaint
@inline:
      STZ   EB_EDITDELTA                   ; in-line delete: run starts at the cursor
      JMP   editbuf_after_inline_edit
@done:
      RTS

; editbuf_has_room — carry set if LEN < CAP (room for at least one byte).
editbuf_has_room:
      LDA   EDITBUF_LENL
      CMP   EDITBUF_CAPL
      LDA   EDITBUF_LENH
      SBC   EDITBUF_CAPH
      BCC   @yes                  ; LEN < CAP
      CLC
      RTS
@yes:
      SEC
      RTS

editbuf_cursor_inc:
      INC   EDITBUF_CURL
      BNE   @done
      INC   EDITBUF_CURH
@done:
      RTS

editbuf_cursor_dec:
      LDA   EDITBUF_CURL
      BNE   @done
      DEC   EDITBUF_CURH
@done:
      DEC   EDITBUF_CURL
      RTS

; =====================================================================
; Selection / clipboard
; =====================================================================

editbuf_clear_selection:
      LDA   EDITBUF_SELACT
      BEQ   @done
      ; If a selection is active, replace it (delete) before inserting.
      JSR   editbuf_delete_selection
@done:
      STZ   EDITBUF_SELACT
      RTS

; Normalize selection -> EB_SELSTART (min) .. EB_SELEND (max)
editbuf_normalize_selection:
      LDA   EB_SELL
      CMP   EDITBUF_CURL
      LDA   EB_SELH
      SBC   EDITBUF_CURH
      BCC   @anchor_lt            ; anchor < cursor
      ; cursor <= anchor
      LDA   EDITBUF_CURL
      STA   EB_SELSTARTL
      LDA   EDITBUF_CURH
      STA   EB_SELSTARTH
      LDA   EB_SELL
      STA   EB_SELENDL
      LDA   EB_SELH
      STA   EB_SELENDH
      RTS
@anchor_lt:
      LDA   EB_SELL
      STA   EB_SELSTARTL
      LDA   EB_SELH
      STA   EB_SELSTARTH
      LDA   EDITBUF_CURL
      STA   EB_SELENDL
      LDA   EDITBUF_CURH
      STA   EB_SELENDH
      RTS

editbuf_delete_selection:
      JSR   editbuf_normalize_selection
      ; count = end - start
      SEC
      LDA   EB_SELENDL
      SBC   EB_SELSTARTL
      STA   EDITBUF_CNTL
      LDA   EB_SELENDH
      SBC   EB_SELSTARTH
      STA   EDITBUF_CNTH
      LDA   EDITBUF_CNTL
      ORA   EDITBUF_CNTH
      BEQ   @done
      LDA   EB_SELSTARTL
      STA   EB_SCRATCHL
      LDA   EB_SELSTARTH
      STA   EB_SCRATCHH
      JSR   editbuf_close_gap
      ; cursor = start
      LDA   EB_SELSTARTL
      STA   EDITBUF_CURL
      LDA   EB_SELSTARTH
      STA   EDITBUF_CURH
      JSR   editbuf_mark_dirty
@done:
      STZ   EDITBUF_SELACT
      RTS

; editbuf_select_all — anchor at 0, cursor at end.
editbuf_select_all:
      STZ   EB_SELL
      STZ   EB_SELH
      LDA   #1
      STA   EDITBUF_SELACT
      LDA   EDITBUF_LENL
      STA   EDITBUF_CURL
      LDA   EDITBUF_LENH
      STA   EDITBUF_CURH
      JMP   editbuf_after_change

; editbuf_copy — copy selection (or nothing) into clipboard.
editbuf_copy:
      LDA   EDITBUF_SELACT
      BEQ   @done
      JSR   editbuf_normalize_selection
      ; count = end - start, clamp to clip cap
      SEC
      LDA   EB_SELENDL
      SBC   EB_SELSTARTL
      STA   EDITBUF_CLIPLENL
      LDA   EB_SELENDH
      SBC   EB_SELSTARTH
      STA   EDITBUF_CLIPLENH
      ; clamp cliplen to 255 (copy/paste iterate with an 8-bit index)
      LDA   EDITBUF_CLIPLENH
      BEQ   @copy
      LDA   #255
      STA   EDITBUF_CLIPLENL
      STZ   EDITBUF_CLIPLENH
@copy:
      ; src ptr = BUF + selstart
      LDA   EB_SELSTARTL
      STA   EB_SCRATCHL
      LDA   EB_SELSTARTH
      STA   EB_SCRATCHH
      JSR   editbuf_src_from_scratch
      LDX   #0                    ; clip index (<=256)
@cl:
      CPX   EDITBUF_CLIPLENL
      BNE   :+
      LDA   EDITBUF_CLIPLENH
      BEQ   @done
:     LDY   #0
      LDA   (EB_SRCL),Y
      STA   EDITBUF_CLIP,X
      INC   EB_SRCL
      BNE   :+
      INC   EB_SRCH
:     INX
      BNE   @cl
@done:
      RTS

editbuf_cut:
      LDA   EDITBUF_SELACT
      BEQ   @done
      JSR   editbuf_copy
      JSR   editbuf_delete_selection
      JMP   editbuf_after_change
@done:
      RTS

; editbuf_paste — insert clipboard bytes at cursor.
editbuf_paste:
      JSR   editbuf_clear_selection
      LDA   EDITBUF_CLIPLENL
      ORA   EDITBUF_CLIPLENH
      BEQ   @done
      ; room? LEN + cliplen <= CAP
      CLC
      LDA   EDITBUF_LENL
      ADC   EDITBUF_CLIPLENL
      STA   EB_T0
      LDA   EDITBUF_LENH
      ADC   EDITBUF_CLIPLENH
      STA   EB_T1
      LDA   EB_T0
      CMP   EDITBUF_CAPL
      LDA   EB_T1
      SBC   EDITBUF_CAPH
      BCS   @done                 ; would overflow cap
      ; make gap of cliplen at cursor
      LDA   EDITBUF_CLIPLENL
      STA   EDITBUF_CNTL
      LDA   EDITBUF_CLIPLENH
      STA   EDITBUF_CNTH
      JSR   editbuf_make_gap
      ; dst ptr = BUF + cursor
      LDA   EDITBUF_CURL
      STA   EB_SCRATCHL
      LDA   EDITBUF_CURH
      STA   EB_SCRATCHH
      LDA   EDITBUF_BUFL
      CLC
      ADC   EB_SCRATCHL
      STA   EB_DSTL
      LDA   EDITBUF_BUFH
      ADC   EB_SCRATCHH
      STA   EB_DSTH
      LDX   #0
@pl:
      CPX   EDITBUF_CLIPLENL
      BNE   :+
      LDA   EDITBUF_CLIPLENH
      BEQ   @adv
:     LDA   EDITBUF_CLIP,X
      LDY   #0
      STA   (EB_DSTL),Y
      INC   EB_DSTL
      BNE   :+
      INC   EB_DSTH
:     INX
      BNE   @pl
@adv:
      ; cursor += cliplen
      CLC
      LDA   EDITBUF_CURL
      ADC   EDITBUF_CLIPLENL
      STA   EDITBUF_CURL
      LDA   EDITBUF_CURH
      ADC   EDITBUF_CLIPLENH
      STA   EDITBUF_CURH
      JSR   editbuf_mark_dirty
      JMP   editbuf_after_change
@done:
      RTS

; =====================================================================
; Cursor navigation
; =====================================================================

editbuf_move_left:
      JSR   editbuf_begin_move
      LDA   EDITBUF_CURL
      ORA   EDITBUF_CURH
      BEQ   @done
      JSR   editbuf_cursor_dec
      JSR   editbuf_update_goalcol
      JMP   editbuf_after_move
@done:
      RTS

editbuf_move_right:
      JSR   editbuf_begin_move
      LDA   EDITBUF_CURL
      CMP   EDITBUF_LENL
      LDA   EDITBUF_CURH
      SBC   EDITBUF_LENH
      BCS   @done                 ; cursor >= len
      JSR   editbuf_cursor_inc
      JSR   editbuf_update_goalcol
      JMP   editbuf_after_move
@done:
      RTS

editbuf_move_home:
      JSR   editbuf_begin_move
      JSR   editbuf_compute_linecol
      ; cursor = cursor - curcol
      SEC
      LDA   EDITBUF_CURL
      SBC   EB_CURCOL
      STA   EDITBUF_CURL
      LDA   EDITBUF_CURH
      SBC   #0
      STA   EDITBUF_CURH
      STZ   EB_GOALCOL
      JMP   editbuf_after_move

editbuf_move_end:
      JSR   editbuf_begin_move
      JSR   editbuf_compute_linecol
      ; advance cursor to end of current line (until \n or buffer end)
@scan:
      LDA   EDITBUF_CURL
      CMP   EDITBUF_LENL
      LDA   EDITBUF_CURH
      SBC   EDITBUF_LENH
      BCS   @done                 ; at end of buffer
      LDX   EDITBUF_CURL
      LDA   EDITBUF_CURH
      JSR   editbuf_ptr_from_off
      LDY   #0
      LDA   (EB_PL),Y
      CMP   #$0A
      BEQ   @done
      JSR   editbuf_cursor_inc
      BRA   @scan
@done:
      JSR   editbuf_update_goalcol
      JMP   editbuf_after_move

editbuf_move_up:
      JSR   editbuf_begin_move
      JSR   editbuf_compute_linecol
      LDA   EB_CURLINEL
      ORA   EB_CURLINEH
      BEQ   @done                 ; already on first line
      ; target line = curline - 1
      LDA   EB_CURLINEL
      STA   EB_SCRATCHL
      LDA   EB_CURLINEH
      STA   EB_SCRATCHH
      LDA   EB_SCRATCHL
      BNE   :+
      DEC   EB_SCRATCHH
:     DEC   EB_SCRATCHL
      JSR   editbuf_goto_line_col
      JMP   editbuf_after_move
@done:
      RTS

editbuf_move_down:
      JSR   editbuf_begin_move
      JSR   editbuf_compute_linecol  ; CURLINE (clobbers EB_SCRATCH)
      JSR   editbuf_count_lines      ; EB_T0:EB_T1 = total (clobbers EB_SCRATCH)
      ; target line = curline + 1 (set EB_SCRATCH last; both helpers above use it)
      LDA   EB_CURLINEL
      STA   EB_SCRATCHL
      LDA   EB_CURLINEH
      STA   EB_SCRATCHH
      INC   EB_SCRATCHL
      BNE   :+
      INC   EB_SCRATCHH
:     LDA   EB_SCRATCHL
      CMP   EB_T0
      LDA   EB_SCRATCHH
      SBC   EB_T1
      BCS   @done                 ; target >= total -> no line below
      JSR   editbuf_goto_line_col
      JMP   editbuf_after_move
@done:
      RTS

editbuf_page_up:
      JSR   editbuf_begin_move
      JSR   editbuf_compute_linecol
      ; target = curline - VIEW_ROWS (clamp 0)
      LDA   EB_CURLINEL
      SEC
      SBC   #EDITBUF_VIEW_ROWS
      STA   EB_SCRATCHL
      LDA   EB_CURLINEH
      SBC   #0
      STA   EB_SCRATCHH
      BCS   @ok
      STZ   EB_SCRATCHL
      STZ   EB_SCRATCHH
@ok:
      JSR   editbuf_goto_line_col
      JMP   editbuf_after_move

editbuf_page_down:
      JSR   editbuf_begin_move
      JSR   editbuf_compute_linecol  ; CURLINE (clobbers EB_SCRATCH)
      JSR   editbuf_count_lines      ; EB_T0:EB_T1 = total (clobbers EB_SCRATCH)
      ; clamp total to total-1 in EB_T0:EB_T1 (last visible line index)
      LDA   EB_T0
      BNE   :+
      DEC   EB_T1
:     DEC   EB_T0
      ; target = curline + VIEW_ROWS (set EB_SCRATCH after the helpers)
      LDA   EB_CURLINEL
      CLC
      ADC   #EDITBUF_VIEW_ROWS
      STA   EB_SCRATCHL
      LDA   EB_CURLINEH
      ADC   #0
      STA   EB_SCRATCHH
      LDA   EB_SCRATCHL
      CMP   EB_T0
      LDA   EB_SCRATCHH
      SBC   EB_T1
      BCC   @ok
      LDA   EB_T0
      STA   EB_SCRATCHL
      LDA   EB_T1
      STA   EB_SCRATCHH
@ok:
      JSR   editbuf_goto_line_col
      JMP   editbuf_after_move

; editbuf_goto_line_col — set cursor to (line=EB_SCRATCH, col=EB_GOALCOL),
; clamped to the line's length.
editbuf_goto_line_col:
      JSR   editbuf_offset_of_line   ; EB_LINEOFF = start offset of EB_SCRATCH line
      ; cursor = lineoff
      LDA   EB_LINEOFFL
      STA   EDITBUF_CURL
      LDA   EB_LINEOFFH
      STA   EDITBUF_CURH
      ; advance up to GOALCOL chars, stopping at \n / buffer end
      LDX   EB_GOALCOL
      BEQ   @done
@adv:
      LDA   EDITBUF_CURL
      CMP   EDITBUF_LENL
      LDA   EDITBUF_CURH
      SBC   EDITBUF_LENH
      BCS   @done
      PHX
      LDX   EDITBUF_CURL
      LDA   EDITBUF_CURH
      JSR   editbuf_ptr_from_off
      LDY   #0
      LDA   (EB_PL),Y
      PLX
      CMP   #$0A
      BEQ   @done
      JSR   editbuf_cursor_inc
      DEX
      BNE   @adv
@done:
      RTS

editbuf_update_goalcol:
      JSR   editbuf_compute_linecol
      LDA   EB_CURCOL
      STA   EB_GOALCOL
      RTS

; =====================================================================
; Line/column computation
; =====================================================================

; editbuf_compute_linecol — scan [0,cursor) counting newlines.
;   Sets EB_CURLINE (16) and EB_CURCOL (8, clamped 255), and EB_LINEOFF =
;   offset of the current line's first byte.
editbuf_compute_linecol:
      STZ   EB_CURLINEL
      STZ   EB_CURLINEH
      STZ   EB_CURCOL
      STZ   EB_LINEOFFL
      STZ   EB_LINEOFFH
      ; iterate offset 0..cursor-1
      STZ   EB_SCRATCHL
      STZ   EB_SCRATCHH
@loop:
      LDA   EB_SCRATCHL
      CMP   EDITBUF_CURL
      LDA   EB_SCRATCHH
      SBC   EDITBUF_CURH
      BCS   @done                 ; scratch >= cursor
      JSR   editbuf_src_from_scratch
      LDY   #0
      LDA   (EB_SRCL),Y
      CMP   #$0A
      BNE   @notnl
      ; newline: line++, col=0, lineoff = scratch+1
      INC   EB_CURLINEL
      BNE   :+
      INC   EB_CURLINEH
:     STZ   EB_CURCOL
      CLC
      LDA   EB_SCRATCHL
      ADC   #1
      STA   EB_LINEOFFL
      LDA   EB_SCRATCHH
      ADC   #0
      STA   EB_LINEOFFH
      BRA   @next
@notnl:
      LDA   EB_CURCOL
      CMP   #255
      BCS   @next
      INC   EB_CURCOL
@next:
      INC   EB_SCRATCHL
      BNE   @loop
      INC   EB_SCRATCHH
      BRA   @loop
@done:
      RTS

; editbuf_offset_of_line — find first-byte offset of line number EB_SCRATCH.
;   Output: EB_LINEOFF.
editbuf_offset_of_line:
      ; if line 0 -> offset 0
      LDA   EB_SCRATCHL
      ORA   EB_SCRATCHH
      BNE   @scan
      STZ   EB_LINEOFFL
      STZ   EB_LINEOFFH
      RTS
@scan:
      ; walk buffer counting newlines until we've passed EB_SCRATCH of them
      STZ   EB_LINEOFFL           ; reuse as running offset
      STZ   EB_LINEOFFH
      STZ   EB_T0                 ; newline count lo
      STZ   EB_T1                 ; newline count hi
@loop:
      ; offset >= len? then return current offset (line past end -> end)
      LDA   EB_LINEOFFL
      CMP   EDITBUF_LENL
      LDA   EB_LINEOFFH
      SBC   EDITBUF_LENH
      BCS   @done
      ; read byte at offset
      LDA   EDITBUF_BUFL
      CLC
      ADC   EB_LINEOFFL
      STA   EB_SRCL
      LDA   EDITBUF_BUFH
      ADC   EB_LINEOFFH
      STA   EB_SRCH
      LDY   #0
      LDA   (EB_SRCL),Y
      ; advance offset
      INC   EB_LINEOFFL
      BNE   :+
      INC   EB_LINEOFFH
:     CMP   #$0A
      BNE   @loop
      ; counted a newline
      INC   EB_T0
      BNE   :+
      INC   EB_T1
:     ; reached target? (newline count == target line)
      LDA   EB_T0
      CMP   EB_SCRATCHL
      BNE   @loop
      LDA   EB_T1
      CMP   EB_SCRATCHH
      BNE   @loop
      ; EB_LINEOFF now points just past the Nth newline = line start
@done:
      RTS

; editbuf_count_lines — total line count = (newlines in buffer)+1 -> EB_T0:EB_T1
editbuf_count_lines:
      LDA   #1
      STA   EB_T0
      STZ   EB_T1
      STZ   EB_SCRATCHL
      STZ   EB_SCRATCHH
@loop:
      LDA   EB_SCRATCHL
      CMP   EDITBUF_LENL
      LDA   EB_SCRATCHH
      SBC   EDITBUF_LENH
      BCS   @done
      JSR   editbuf_src_from_scratch
      LDY   #0
      LDA   (EB_SRCL),Y
      CMP   #$0A
      BNE   @next
      INC   EB_T0
      BNE   @next
      INC   EB_T1
@next:
      INC   EB_SCRATCHL
      BNE   @loop
      INC   EB_SCRATCHH
      BRA   @loop
@done:
      RTS

; =====================================================================
; After-change: recompute, adjust scroll, re-render, place cursor.
; =====================================================================
editbuf_after_change:
      JSR   editbuf_compute_linecol
      JSR   editbuf_adjust_scroll
      JSR   editbuf_render
      RTS

; editbuf_begin_move — start a cursor move: remember whether a selection was
; active (so its highlight can be cleared on the next repaint), then drop it.
editbuf_begin_move:
      LDA   EDITBUF_SELACT
      STA   EB_PREVSEL
      STZ   EDITBUF_SELACT
      RTS

; editbuf_after_move — finish a cursor move. The buffer didn't change, so when
; the scroll didn't move and no selection highlight needs clearing, just reposition
; the hardware cursor — no body repaint (this is what made arrowing smear).
editbuf_after_move:
      JSR   editbuf_compute_linecol
      LDA   EB_TOPLINEL
      STA   EB_PREVTOPL
      LDA   EB_TOPLINEH
      STA   EB_PREVTOPH
      LDA   EB_LEFTCOL
      STA   EB_PREVLEFT
      JSR   editbuf_adjust_scroll
      LDA   EB_PREVSEL                ; had a selection -> repaint to clear it
      BNE   @full
      LDA   EB_TOPLINEL
      CMP   EB_PREVTOPL
      BNE   @full
      LDA   EB_TOPLINEH
      CMP   EB_PREVTOPH
      BNE   @full
      LDA   EB_LEFTCOL
      CMP   EB_PREVLEFT
      BNE   @full
      JMP   editbuf_place_cursor
@full:
      JMP   editbuf_render

; editbuf_after_inline_edit — for edits confined to the cursor's line (printable
; insert, mid-line delete/backspace). EB_EDITDELTA holds how many columns left of
; the cursor the changed run begins (1 after an insert, 0 after a delete). When
; the scroll didn't move, repaint ONLY the changed run [start..VIEW_COLS) of the
; cursor's row; otherwise a full repaint.
editbuf_after_inline_edit:
      JSR   editbuf_compute_linecol
      LDA   EB_TOPLINEL
      STA   EB_PREVTOPL
      LDA   EB_TOPLINEH
      STA   EB_PREVTOPH
      LDA   EB_LEFTCOL
      STA   EB_PREVLEFT
      JSR   editbuf_adjust_scroll
      LDA   EB_TOPLINEL
      CMP   EB_PREVTOPL
      BNE   @full
      LDA   EB_TOPLINEH
      CMP   EB_PREVTOPH
      BNE   @full
      LDA   EB_LEFTCOL
      CMP   EB_PREVLEFT
      BNE   @full
      ; first changed screen col = (curcol - EDITDELTA) - leftcol, clamped to 0.
      SEC
      LDA   EB_CURCOL
      SBC   EB_EDITDELTA
      SEC
      SBC   EB_LEFTCOL
      BCS   @setstart
      LDA   #0
@setstart:
      STA   EB_PAINTSTART
      JMP   editbuf_render_current_line
@full:
      STZ   EB_PAINTSTART
      JMP   editbuf_render

; editbuf_render_current_line — repaint the cursor's visible row from column
; EB_PAINTSTART (caller sets it) to the right edge of the view.
editbuf_render_current_line:
      ; single-char edits clear the selection: render with an empty range.
      STZ   EB_SELSTARTL
      STZ   EB_SELSTARTH
      STZ   EB_SELENDL
      STZ   EB_SELENDH
      ; screen row = curline - topline (cursor is on-screen, so < VIEW_ROWS)
      SEC
      LDA   EB_CURLINEL
      SBC   EB_TOPLINEL
      STA   EB_ROW
      ; running offset = first byte of the current line
      LDA   EB_CURLINEL
      STA   EB_SCRATCHL
      LDA   EB_CURLINEH
      STA   EB_SCRATCHH
      JSR   editbuf_offset_of_line
      LDA   EB_LINEOFFL
      STA   EB_VISLINEL
      LDA   EB_LINEOFFH
      STA   EB_VISLINEH
      JSR   editbuf_render_row
      JMP   editbuf_place_cursor

; editbuf_after_change_down — for edits that shift text vertically (newline,
; line-join delete/backspace): repaint from the changed line to the bottom of
; the view (a "local" repaint, not the whole screen), or full-repaint if the
; scroll moved.
editbuf_after_change_down:
      JSR   editbuf_compute_linecol
      LDA   EB_TOPLINEL
      STA   EB_PREVTOPL
      LDA   EB_TOPLINEH
      STA   EB_PREVTOPH
      LDA   EB_LEFTCOL
      STA   EB_PREVLEFT
      JSR   editbuf_adjust_scroll
      LDA   EB_TOPLINEL
      CMP   EB_PREVTOPL
      BNE   @full
      LDA   EB_TOPLINEH
      CMP   EB_PREVTOPH
      BNE   @full
      LDA   EB_LEFTCOL
      CMP   EB_PREVLEFT
      BNE   @full
      ; start row = (curline - 1) - topline, clamped to the top of the view.
      ; curline-1 covers both the split line (newline) and the joined line
      ; (delete); repainting one extra unchanged row above is harmless.
      LDA   EB_CURLINEL
      SEC
      SBC   #1
      STA   EB_SCRATCHL
      LDA   EB_CURLINEH
      SBC   #0
      STA   EB_SCRATCHH
      SEC
      LDA   EB_SCRATCHL
      SBC   EB_TOPLINEL
      STA   EB_ROW
      LDA   EB_SCRATCHH
      SBC   EB_TOPLINEH
      BCS   @render                    ; (curline-1) >= topline -> EB_ROW valid
      STZ   EB_ROW                     ; clamp to the top visible row
@render:
      JMP   editbuf_render_from_row
@full:
      JMP   editbuf_render

; editbuf_render_from_row — repaint rows [EB_ROW .. VIEW_ROWS): the line at
; (topline + EB_ROW) down to the bottom of the view.
editbuf_render_from_row:
      STZ   EB_SELSTARTL
      STZ   EB_SELSTARTH
      STZ   EB_SELENDL
      STZ   EB_SELENDH
      CLC
      LDA   EB_TOPLINEL
      ADC   EB_ROW
      STA   EB_SCRATCHL
      LDA   EB_TOPLINEH
      ADC   #0
      STA   EB_SCRATCHH
      JSR   editbuf_offset_of_line
      LDA   EB_LINEOFFL
      STA   EB_VISLINEL
      LDA   EB_LINEOFFH
      STA   EB_VISLINEH
      STZ   EB_PAINTSTART
@loop:
      LDA   EB_ROW
      CMP   #EDITBUF_VIEW_ROWS
      BCS   @done
      JSR   editbuf_render_row
      INC   EB_ROW
      BRA   @loop
@done:
      JMP   editbuf_place_cursor

editbuf_adjust_scroll:
      ; vertical: if curline < topline -> topline = curline
      LDA   EB_CURLINEL
      CMP   EB_TOPLINEL
      LDA   EB_CURLINEH
      SBC   EB_TOPLINEH
      BCS   @check_bottom
      LDA   EB_CURLINEL
      STA   EB_TOPLINEL
      LDA   EB_CURLINEH
      STA   EB_TOPLINEH
      BRA   @horizontal
@check_bottom:
      ; if curline >= topline + VIEW_ROWS -> topline = curline - VIEW_ROWS + 1
      CLC
      LDA   EB_TOPLINEL
      ADC   #EDITBUF_VIEW_ROWS
      STA   EB_T0
      LDA   EB_TOPLINEH
      ADC   #0
      STA   EB_T1
      LDA   EB_CURLINEL
      CMP   EB_T0
      LDA   EB_CURLINEH
      SBC   EB_T1
      BCC   @horizontal
      ; topline = curline - (VIEW_ROWS-1)
      SEC
      LDA   EB_CURLINEL
      SBC   #EDITBUF_VIEW_ROWS-1
      STA   EB_TOPLINEL
      LDA   EB_CURLINEH
      SBC   #0
      STA   EB_TOPLINEH
@horizontal:
      ; if curcol < leftcol -> leftcol = curcol
      LDA   EB_CURCOL
      CMP   EB_LEFTCOL
      BCS   @right
      STA   EB_LEFTCOL
      RTS
@right:
      ; if curcol >= leftcol + VIEW_COLS -> leftcol = curcol - VIEW_COLS + 1
      LDA   EB_LEFTCOL
      CLC
      ADC   #EDITBUF_VIEW_COLS
      STA   EB_T0
      LDA   EB_CURCOL
      CMP   EB_T0
      BCC   @done
      LDA   EB_CURCOL
      SEC
      SBC   #EDITBUF_VIEW_COLS-1
      STA   EB_LEFTCOL
@done:
      RTS

; =====================================================================
; Render the visible text into the body region.
; =====================================================================
editbuf_render:
      ; normalize selection once for cell tests
      LDA   EDITBUF_SELACT
      BEQ   @nosel
      JSR   editbuf_normalize_selection
      BRA   @start
@nosel:
      ; empty selection range
      STZ   EB_SELSTARTL
      STZ   EB_SELSTARTH
      STZ   EB_SELENDL
      STZ   EB_SELENDH
@start:
      ; line being rendered = topline ; its offset:
      LDA   EB_TOPLINEL
      STA   EB_SCRATCHL
      LDA   EB_TOPLINEH
      STA   EB_SCRATCHH
      JSR   editbuf_offset_of_line
      LDA   EB_LINEOFFL
      STA   EB_VISLINEL           ; reuse EB_VISLINE as running offset
      LDA   EB_LINEOFFH
      STA   EB_VISLINEH
      STZ   EB_ROW
      STZ   EB_PAINTSTART
@rowloop:
      LDA   EB_ROW
      CMP   #EDITBUF_VIEW_ROWS
      BCS   @donerows
      JSR   editbuf_render_row
      INC   EB_ROW
      BRA   @rowloop
@donerows:
      JMP   editbuf_place_cursor

; editbuf_render_row — render screen row EB_ROW, painting columns
; [EB_PAINTSTART .. VIEW_COLS) from the document. EB_VISLINE = offset of this
; line's first byte; on exit it points to the next line's first byte.
;
; Cells are written through the VGC direct screen window one plane at a time:
; the row's base cell offset is computed ONCE (no per-cell multiply, no per-cell
; cursor sync the way vtext_put_char does), the plane is selected via $B1A0, and
; each pass walks a plain (zp),Y store pointer through the window ($A200+off) —
; one store per cell instead of the VRAM port's address+data handshake.
editbuf_render_row:
      ; Determine line length (chars before \n or buffer end), and fill colors.
      JSR   editbuf_measure_line   ; EB_LINELEN set; HL colors filled
      ; EB_CELL = plane offset of the first painted cell:
      ;   (VIEW_TOP + EB_ROW) * 80 + VIEW_LEFT + EB_PAINTSTART
      LDA   #EDITBUF_VIEW_TOP
      CLC
      ADC   EB_ROW
      STA   EB_T0                 ; absolute screen row (<= 47)
      ASL
      ASL
      CLC
      ADC   EB_T0                 ; row * 5 (<= 235, fits 8 bits)
      STA   EB_CELLL
      STZ   EB_CELLH
      ASL   EB_CELLL
      ROL   EB_CELLH
      ASL   EB_CELLL
      ROL   EB_CELLH
      ASL   EB_CELLL
      ROL   EB_CELLH
      ASL   EB_CELLL
      ROL   EB_CELLH              ; row * 80
      LDA   #EDITBUF_VIEW_LEFT
      CLC
      ADC   EB_PAINTSTART
      CLC
      ADC   EB_CELLL
      STA   EB_CELLL
      BCC   :+
      INC   EB_CELLH
:
      ; ---- char plane ----
      LDA   #VGC_SCREENWIN_CHAR
      JSR   editbuf_win_begin      ; select window plane, point run ptr at EB_CELL
      LDA   EB_PAINTSTART
      STA   EB_COL
@charloop:
      LDA   EB_COL
      CMP   #EDITBUF_VIEW_COLS
      BCS   @colorpass
      LDA   EB_LEFTCOL
      CLC
      ADC   EB_COL
      STA   EB_T0                 ; src col
      CMP   EB_LINELEN
      BCS   @charblank
      ; char = BUF[EB_VISLINE + srccol]
      LDA   EB_VISLINEL
      CLC
      ADC   EB_T0
      STA   EB_PL
      LDA   EB_VISLINEH
      ADC   #0
      STA   EB_PH
      LDA   EDITBUF_BUFL
      CLC
      ADC   EB_PL
      STA   EB_PL
      LDA   EDITBUF_BUFH
      ADC   EB_PH
      STA   EB_PH
      LDY   #0
      LDA   (EB_PL),Y
      BRA   @charput
@charblank:
      LDA   #' '
@charput:
      JSR   editbuf_win_put        ; store A at the window cell, advance run ptr
      INC   EB_COL
      BRA   @charloop

      ; ---- color plane ----
@colorpass:
      LDA   #VGC_SCREENWIN_COLOR
      JSR   editbuf_win_begin
      LDA   EB_PAINTSTART
      STA   EB_COL
@colorloop:
      LDA   EB_COL
      CMP   #EDITBUF_VIEW_COLS
      BCS   @attrpass
      LDA   EB_LEFTCOL
      CLC
      ADC   EB_COL
      STA   EB_T0                 ; src col
      CMP   EB_LINELEN
      BCS   @colorblank
      ; abs offset = EB_VISLINE + srccol -> EB_SRC (for selection test)
      LDA   EB_VISLINEL
      CLC
      ADC   EB_T0
      STA   EB_SRCL
      LDA   EB_VISLINEH
      ADC   #0
      STA   EB_SRCH
      JSR   editbuf_cell_in_selection   ; carry set if in selection
      BCC   @colornormal
      LDA   #EDITBUF_COLOR_SEL
      BRA   @colorput
@colornormal:
      LDX   EB_T0
      LDA   EDITBUF_HL_COLORS,X
      BRA   @colorput
@colorblank:
      LDA   #EDITBUF_COLOR_TEXT
@colorput:
      JSR   editbuf_win_put
      INC   EB_COL
      BRA   @colorloop

      ; ---- attribute plane (always 0 in the body) ----
@attrpass:
      LDA   #VGC_SCREENWIN_ATTR
      JSR   editbuf_win_begin
      LDA   EB_PAINTSTART
      STA   EB_COL
@attrloop:
      LDA   EB_COL
      CMP   #EDITBUF_VIEW_COLS
      BCS   @done
      LDA   #0
      JSR   editbuf_win_put
      INC   EB_COL
      BRA   @attrloop
@done:
      ; advance EB_VISLINE to next line: skip to and over the \n (if any)
      JSR   editbuf_advance_to_next_line
      RTS

; editbuf_win_begin — select window plane A ($B1A0: 0=char/1=color/2=attr) and
; seed the zero-page run pointer EB_RUN at the window address of the row's first
; painted cell (VGC_SCREENWIN + EB_CELL). editbuf_win_put then writes one cell.
editbuf_win_begin:
      STA   VGC_SCREENWIN_PLANE
      LDA   EB_CELLL
      CLC
      ADC   #<VGC_SCREENWIN
      STA   EB_RUNL
      LDA   EB_CELLH
      ADC   #>VGC_SCREENWIN
      STA   EB_RUNH
      RTS

; editbuf_win_put — store A at the window cell EB_RUN points to (a plain store,
; no port handshake), then advance EB_RUN by one. A is preserved.
editbuf_win_put:
      LDY   #0
      STA   (EB_RUNL),Y
      INC   EB_RUNL
      BNE   :+
      INC   EB_RUNH
:     RTS

; editbuf_cell_in_selection — EB_SRC = absolute offset of cell.
;   carry set if SELSTART <= off < SELEND (and selection active).
editbuf_cell_in_selection:
      LDA   EDITBUF_SELACT
      BNE   :+
      CLC
      RTS
:     ; off >= selstart ?
      LDA   EB_SRCL
      CMP   EB_SELSTARTL
      LDA   EB_SRCH
      SBC   EB_SELSTARTH
      BCC   @no                   ; off < start
      ; off < selend ?
      LDA   EB_SRCL
      CMP   EB_SELENDL
      LDA   EB_SRCH
      SBC   EB_SELENDH
      BCC   @yes                  ; off < end
@no:
      CLC
      RTS
@yes:
      SEC
      RTS

; editbuf_measure_line — measure current line (offset EB_VISLINE): set EB_LINELEN
;   to chars before \n (clamped 80), pre-fill HL_COLORS with default, then call
;   the HILITE hook.
editbuf_measure_line:
      ; pre-fill colors with default
      LDX   #79
      LDA   #EDITBUF_COLOR_TEXT
:     STA   EDITBUF_HL_COLORS,X
      DEX
      BPL   :-
      ; scan line length
      LDA   EB_VISLINEL
      STA   EB_SCRATCHL
      LDA   EB_VISLINEH
      STA   EB_SCRATCHH
      LDX   #0
@scan:
      ; offset >= len ?
      LDA   EB_SCRATCHL
      CMP   EDITBUF_LENL
      LDA   EB_SCRATCHH
      SBC   EDITBUF_LENH
      BCS   @done
      JSR   editbuf_src_from_scratch
      LDY   #0
      LDA   (EB_SRCL),Y
      CMP   #$0A
      BEQ   @done
      INX
      CPX   #80
      BCS   @done
      INC   EB_SCRATCHL
      BNE   @scan
      INC   EB_SCRATCHH
      BRA   @scan
@done:
      STX   EB_LINELEN
      ; set HL hook inputs
      LDA   EDITBUF_BUFL
      CLC
      ADC   EB_VISLINEL
      STA   EDITBUF_HL_PTR
      LDA   EDITBUF_BUFH
      ADC   EB_VISLINEH
      STA   EDITBUF_HL_PTR+1
      LDA   EB_LINELEN
      STA   EDITBUF_HL_LEN
      JMP   editbuf_call_hilite

; editbuf_advance_to_next_line — move EB_VISLINE past the current line's \n.
editbuf_advance_to_next_line:
@scan:
      LDA   EB_VISLINEL
      CMP   EDITBUF_LENL
      LDA   EB_VISLINEH
      SBC   EDITBUF_LENH
      BCS   @done                 ; at end
      LDA   EB_VISLINEL
      STA   EB_SCRATCHL
      LDA   EB_VISLINEH
      STA   EB_SCRATCHH
      JSR   editbuf_src_from_scratch
      LDY   #0
      LDA   (EB_SRCL),Y
      ; advance
      INC   EB_VISLINEL
      BNE   :+
      INC   EB_VISLINEH
:     CMP   #$0A
      BNE   @scan
@done:
      RTS

; editbuf_place_cursor — set hw cursor at the on-screen cursor position.
editbuf_place_cursor:
      ; screen x = VIEW_LEFT + (curcol - leftcol)
      LDA   EB_CURCOL
      SEC
      SBC   EB_LEFTCOL
      CLC
      ADC   #EDITBUF_VIEW_LEFT
      STA   VGC_CURSX
      ; screen y = VIEW_TOP + (curline - topline)
      SEC
      LDA   EB_CURLINEL
      SBC   EB_TOPLINEL
      ; (assume within view; high bytes equal after scroll adjust)
      CLC
      ADC   #EDITBUF_VIEW_TOP
      STA   VGC_CURSY
      LDA   #1
      STA   VGC_CURSEN
      RTS

; =====================================================================
; Dirty handling
; =====================================================================
editbuf_mark_dirty:
      LDA   EDITUI_DIRTY
      BNE   @done
      LDA   #1
      STA   EDITUI_DIRTY
      JSR   editui_refresh_dirty
@done:
      RTS

; =====================================================================
; Save / quit
; =====================================================================
editbuf_do_save:
      JSR   editbuf_call_save        ; A = status
      CMP   #EDITBUF_SAVE_OK
      BNE   @notok
      STZ   EDITUI_DIRTY
      JSR   editui_refresh_dirty
@notok:
      JSR   editbuf_apply_status
      JSR   editui_draw_status
      JMP   editbuf_place_cursor

; editbuf_do_quit — returns carry set to exit the editor, clear to stay.
editbuf_do_quit:
      LDA   EDITUI_DIRTY
      BNE   @dirty
      LDA   #EDITBUF_EXIT_QUIT
      STA   EDITBUF_RESULT
      SEC
      RTS
@dirty:
      JSR   editbuf_dialog3          ; A = 0 exit / 1 save / 2 cancel
      CMP   #EDITBUF_DLG_CANCEL
      BEQ   @stay
      CMP   #EDITBUF_DLG_SAVE
      BEQ   @savefirst
      ; exit anyway
      LDA   #EDITBUF_EXIT_DISCARD
      STA   EDITBUF_RESULT
      SEC
      RTS
@savefirst:
      JSR   editbuf_call_save
      CMP   #EDITBUF_SAVE_OK
      BNE   @save_failed
      STZ   EDITUI_DIRTY
      LDA   #EDITBUF_EXIT_SAVED
      STA   EDITBUF_RESULT
      SEC
      RTS
@save_failed:
      ; stay in editor showing the host's status message
      JSR   editbuf_redraw_all
      JSR   editbuf_apply_status
      JSR   editui_draw_status
      JSR   editbuf_place_cursor
      CLC
      RTS
@stay:
      JSR   editbuf_redraw_all
      CLC
      RTS

; editbuf_redraw_all — full repaint after a dialog (chrome + body + cursor).
editbuf_redraw_all:
      JSR   editui_draw_shell
      JMP   editbuf_render

; =====================================================================
; Three-choice modal dialog (restore-under). Returns A = 0/1/2.
; =====================================================================
EB_DLG_X     = 15
EB_DLG_Y     = 20
EB_DLG_W     = 50
EB_DLG_H     = 9

editbuf_dialog3:
      STZ   EB_DLGSEL
      ; set save-under region to the dialog rect and capture it
      LDA   #EB_DLG_X
      STA   EDITUI_MENU_SAVE_X
      LDA   #EB_DLG_Y
      STA   EDITUI_MENU_SAVE_Y
      LDA   #EB_DLG_W
      STA   EDITUI_MENU_SAVE_W
      LDA   #EB_DLG_H
      STA   EDITUI_MENU_SAVE_H
      JSR   editui_save_under
      ; draw dialog frame
      LDA   #EB_DLG_X
      STA   EDITUI_BOXX
      LDA   #EB_DLG_Y
      STA   EDITUI_BOXY
      LDA   #EB_DLG_W
      STA   EDITUI_BOXW
      LDA   #EB_DLG_H
      STA   EDITUI_BOXH
      LDA   #EDITUI_BOX_DOUBLE
      STA   EDITUI_BOX_STYLE
      LDA   #<editbuf_dlg_title
      STA   EDITUI_BOX_TITLEL
      LDA   #>editbuf_dlg_title
      STA   EDITUI_BOX_TITLEH
      JSR   editui_draw_box
      ; clear the dialog interior (box vars still hold the dialog rect)
      JSR   editui_select_box_body
      LDA   #EDITUI_COLOR_PANEL
      STA   VTEXT_COLOR
      STZ   VTEXT_ATTR
      LDA   #' '
      STA   VTEXT_CHAR
      JSR   vtext_clear_region
      ; message line
      LDA   #<editbuf_dlg_msg
      STA   EDITUI_PRINTL
      LDA   #>editbuf_dlg_msg
      STA   EDITUI_PRINTH
      LDA   #EB_DLG_X+3
      STA   EDITUI_PRINTX
      LDA   #EB_DLG_Y+2
      STA   EDITUI_PRINTY
      LDA   #EDITUI_COLOR_TITLE
      STA   VTEXT_COLOR
      JSR   editui_print_ptr
      JSR   editbuf_dialog_draw_buttons
@wait:
      LDA   VGC_CHARIN
      BEQ   @wait
      CMP   #EDITUI_KEY_LEFT
      BEQ   @left
      CMP   #EDITUI_KEY_RIGHT
      BEQ   @right
      CMP   #EDITUI_KEY_TAB
      BEQ   @right
      CMP   #EDITUI_KEY_ENTER
      BEQ   @choose
      CMP   #EDITUI_KEY_ESC
      BEQ   @cancel
      ; hotkeys: e/s/c (and upper)
      ORA   #$20
      CMP   #'e'
      BNE   :+
      STZ   EB_DLGSEL
      BRA   @choose
:     CMP   #'s'
      BNE   :+
      LDA   #1
      STA   EB_DLGSEL
      BRA   @choose
:     CMP   #'c'
      BNE   @wait
      LDA   #2
      STA   EB_DLGSEL
      BRA   @choose
@left:
      LDA   EB_DLGSEL
      BNE   :+
      LDA   #3
:     DEC   A
      STA   EB_DLGSEL
      JSR   editbuf_dialog_draw_buttons
      BRA   @wait
@right:
      LDA   EB_DLGSEL
      INC   A
      CMP   #3
      BCC   :+
      LDA   #0
:     STA   EB_DLGSEL
      JSR   editbuf_dialog_draw_buttons
      BRA   @wait
@cancel:
      LDA   #2
      STA   EB_DLGSEL
@choose:
      JSR   editui_restore_under
      LDA   EB_DLGSEL
      RTS

; Draw the three buttons, highlighting EB_DLGSEL.
editbuf_dialog_draw_buttons:
      LDA   #0
      STA   EB_T0                 ; button index
      LDA   #<editbuf_dlg_btn0
      STA   EB_PL
      LDA   #>editbuf_dlg_btn0
      STA   EB_PH
@draw:
      ; X position per button
      LDX   EB_T0
      LDA   editbuf_dlg_btn_x,X
      STA   EDITUI_PRINTX
      LDA   #EB_DLG_Y+5
      STA   EDITUI_PRINTY
      ; color: selected?
      LDA   EB_T0
      CMP   EB_DLGSEL
      BNE   @normal
      LDA   #EDITUI_COLOR_MENU_SEL
      BRA   @setc
@normal:
      LDA   #EDITUI_COLOR_DIM
@setc:
      STA   VTEXT_COLOR
      ; print label (split low/high byte pointer tables, indexed by button #)
      LDX   EB_T0
      LDA   editbuf_dlg_btn_lo,X
      STA   EDITUI_PRINTL
      LDA   editbuf_dlg_btn_hi,X
      STA   EDITUI_PRINTH
      JSR   editui_print_ptr
      INC   EB_T0
      LDA   EB_T0
      CMP   #3
      BCC   @draw
      RTS

; =====================================================================
; Hook invocation (indirect JSR via vector; default no-ops)
; =====================================================================
; editbuf_init_vectors — default any unset hook vector to a no-op.
editbuf_init_vectors:
      LDA   EDITBUF_SAVE_VECL
      ORA   EDITBUF_SAVE_VECH
      BNE   :+
      LDA   #<editbuf_default_save
      STA   EDITBUF_SAVE_VECL
      LDA   #>editbuf_default_save
      STA   EDITBUF_SAVE_VECH
:     LDA   EDITBUF_INDENT_VECL
      ORA   EDITBUF_INDENT_VECH
      BNE   :+
      LDA   #<editbuf_default_indent
      STA   EDITBUF_INDENT_VECL
      LDA   #>editbuf_default_indent
      STA   EDITBUF_INDENT_VECH
:     LDA   EDITBUF_HILITE_VECL
      ORA   EDITBUF_HILITE_VECH
      BNE   :+
      LDA   #<editbuf_default_hilite
      STA   EDITBUF_HILITE_VECL
      LDA   #>editbuf_default_hilite
      STA   EDITBUF_HILITE_VECH
:     LDA   EDITBUF_MENU_VECL
      ORA   EDITBUF_MENU_VECH
      BNE   :+
      LDA   #<editbuf_default_menu
      STA   EDITBUF_MENU_VECL
      LDA   #>editbuf_default_menu
      STA   EDITBUF_MENU_VECH
:     RTS

editbuf_call_menu:
      JMP   (EDITBUF_MENU_VECL)

editbuf_call_save:
      JMP   (EDITBUF_SAVE_VECL)

editbuf_call_indent:
      JMP   (EDITBUF_INDENT_VECL)

editbuf_call_hilite:
      JMP   (EDITBUF_HILITE_VECL)

; default hook bodies (host may point vectors here)
      .export editbuf_default_save
      .export editbuf_default_indent
      .export editbuf_default_hilite
      .export editbuf_default_menu
editbuf_default_save:
      LDA   #EDITBUF_SAVE_OK
      RTS
editbuf_default_indent:
      LDA   #0
      RTS
editbuf_default_hilite:
      RTS
; Default MENU hook: keep EDITUI's default menus. A host runtime points
; EDITBUF_MENU_VECL/H at its own routine which calls editui_set_menus (and,
; later, the per-item enable/disable API) to tailor the menu for its tools.
editbuf_default_menu:
      RTS

; =====================================================================
; RODATA
; =====================================================================
      .segment "RODATA"

editbuf_help_text:
      .byte "Ctrl-S Save   Ctrl-Q Quit   Ctrl-X/C/V Cut/Copy/Paste", 0
editbuf_status_default:
      .byte "Ready", 0
editbuf_dlg_title:
      .byte "Workspace Modified", 0
editbuf_dlg_msg:
      .byte "Save changes before leaving?", 0
editbuf_dlg_btn0:
      .byte "[ Exit Anyway ]", 0
editbuf_dlg_btn1:
      .byte "[ Save First ]", 0
editbuf_dlg_btn2:
      .byte "[ Cancel ]", 0

editbuf_dlg_btn_lo:
      .byte <editbuf_dlg_btn0, <editbuf_dlg_btn1, <editbuf_dlg_btn2
editbuf_dlg_btn_hi:
      .byte >editbuf_dlg_btn0, >editbuf_dlg_btn1, >editbuf_dlg_btn2
editbuf_dlg_btn_x:
      .byte EB_DLG_X+2, EB_DLG_X+19, EB_DLG_X+34

.endif

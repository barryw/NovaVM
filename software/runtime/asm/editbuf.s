; =====================================================================
;  Nova NDK — editbuf.s
;
;  Shared modal text-editing engine for Nova native tools: cursor movement,
;  editing, selection/clipboard, find/replace, undo/redo, and rendering. The
;  module is language-neutral; all host behavior arrives through hook vectors.
;  See editbuf.inc for the public ABI and hook contracts.
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

.include "editbuf.inc"
.include "editui.inc"
.include "nui.inc"
.include "textsvc.inc"

.ifndef EDITBUF_IMPLEMENTATION_INCLUDED
EDITBUF_IMPLEMENTATION_INCLUDED = 1

EDITBUF_STATUS_TICKS = 600
EDITBUF_ALT_TIMEOUT_FRAMES = 15

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
EDITBUF_UNDOBUFL:   .res 1
EDITBUF_UNDOBUFH:   .res 1
EDITBUF_REDOBUFL:   .res 1
EDITBUF_REDOBUFH:   .res 1
EDITBUF_LENL:       .res 1
EDITBUF_LENH:       .res 1
EDITBUF_TITLEL:     .res 1
EDITBUF_TITLEH:     .res 1
EDITBUF_TYPEL:      .res 1
EDITBUF_TYPEH:      .res 1
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
EDITBUF_COMMAND_VECL:.res 1
EDITBUF_COMMAND_VECH:.res 1
EDITBUF_CHANGED_VECL:.res 1
EDITBUF_CHANGED_VECH:.res 1

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
EB_TOTAL_LINESL:    .res 1       ; cached total logical line count (16-bit)
EB_TOTAL_LINESH:    .res 1
EB_VSHIFT:          .res 1       ; 1=line inserted, $FF=line removed, 0=unknown
EB_UNDOLENL:        .res 1
EB_UNDOLENH:        .res 1
EB_UNDOCURL:        .res 1
EB_UNDOCURH:        .res 1
EB_UNDOVALID:       .res 1
EB_REDOLENL:        .res 1
EB_REDOLENH:        .res 1
EB_REDOCURL:        .res 1
EB_REDOCURH:        .res 1
EB_REDOVALID:       .res 1
EB_REPLACE_CHANGED: .res 1

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
EB_SCRATCHL:        .res 1
EB_SCRATCHH:        .res 1
EB_PREVTOPL:        .res 1       ; scroll snapshot for single-line repaint
EB_PREVTOPH:        .res 1
EB_PREVLEFT:        .res 1
EB_PREVSEL:         .res 1       ; was a selection active before a cursor move
EB_CELLL:           .res 1       ; plane offset of first painted cell (16-bit)
EB_CELLH:           .res 1
EB_TITLE_START:     .res 1
EB_TITLE_LASTSEP:   .res 1
EB_TITLE_PREVSEP:   .res 1
EB_TITLE_LEN:       .res 1
EB_STATUS_IDX:      .res 1
EB_STATUS_DIGIT:    .res 1
EB_STATUS_STARTED:  .res 1
EB_STATUS_NUML:     .res 1
EB_STATUS_NUMH:     .res 1
EB_BLT_HEIGHT:      .res 1
EB_PROMPTLEN:       .res 1
EB_FINDLEN:         .res 1
EB_PROMPT_MSGL:     .res 1
EB_PROMPT_MSGH:     .res 1
EB_PROMPT_LABELL:   .res 1
EB_PROMPT_LABELH:   .res 1

EB_TITLEBUF:        .res 64
EB_STATUSBUF:       .res 80
EB_PROMPTBUF:       .res 32
EB_FINDBUF:         .res 32

; --- HILITE hook scratch ---
EDITBUF_HL_LEN:     .res 1
EDITBUF_HL_COLORS:  .res 80

; --- transient status state (private; keep after exported hook ABI storage) ---
EB_STATUS_TIMERL:   .res 1
EB_STATUS_TIMERH:   .res 1
EB_STATUS_LASTFRAME:.res 1
EB_STATUS_ACTIVEL:  .res 1
EB_STATUS_ACTIVEH:  .res 1

; =====================================================================
; CODE
; =====================================================================
      .segment "CODE"

      .export editbuf_run
      .export editbuf_reset_state
      .export editbuf_dialog3
      .exportzp EDITBUF_HL_PTR
      .export EDITBUF_HL_LEN
      .export EDITBUF_HL_COLORS

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
      STZ   EB_VSHIFT
      STZ   EB_UNDOVALID
      STZ   EB_REDOVALID
      STZ   EB_FINDLEN
      STZ   EB_STATUS_TIMERL
      STZ   EB_STATUS_TIMERH
      STZ   EB_STATUS_ACTIVEL
      STZ   EB_STATUS_ACTIVEH
      JSR   textsvc_init
      JMP   editbuf_configure_scroll_window

; ---------------------------------------------------------------------
; editbuf_run — run the modal editor. Returns A = EDITBUF_RESULT.
; ---------------------------------------------------------------------
editbuf_run:
      JSR   editbuf_init_vectors
      JSR   editui_init
      JSR   editbuf_configure_scroll_window
      JSR   editbuf_call_menu          ; host customizes the menu for its runtime

      ; Wire the EDITUI chrome to our compact title and metadata status.
      JSR   editbuf_build_title
      LDA   #<EB_TITLEBUF
      STA   EDITUI_TITLEL
      LDA   #>EB_TITLEBUF
      STA   EDITUI_TITLEH
      STZ   EDITUI_HELPL
      STZ   EDITUI_HELPH

      ; Derive line/col + scroll from the host-provided initial cursor offset so
      ; the first render places the cursor correctly (EDITBUF_CUR may be nonzero;
      ; reset_state does not recompute EB_CURLINE/EB_CURCOL).
      JSR   editbuf_compute_linecol
      JSR   editbuf_adjust_scroll
      JSR   editbuf_recount_lines
      JSR   editbuf_apply_status

      JSR   editui_draw_shell
      JSR   editbuf_render

@loop:
      JSR   editbuf_status_tick
      LDA   VGC_CHARIN
      BEQ   @loop
      JSR   editbuf_dispatch_key
      BCC   @loop
      LDA   EDITBUF_RESULT
      RTS

; editbuf_dispatch_key — handle key in A. Returns carry set to exit the editor.
editbuf_dispatch_key:
      STA   EB_T0
      JSR   editbuf_key_to_command
      BEQ   @raw
      JMP   editbuf_dispatch_command

@raw:
      LDA   EB_T0
      CMP   #EDITUI_KEY_LEFT
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
:     CMP   #EDITUI_KEY_CTRL_HOME
      BNE   :+
      JSR   editbuf_move_file_start
      CLC
      RTS
:     CMP   #EDITUI_KEY_CTRL_END
      BNE   :+
      JSR   editbuf_move_file_end
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

editbuf_dispatch_command:
      CMP   #EDITUI_CMD_NEW
      BNE   :+
      JSR   editbuf_do_host_command
      CLC
      RTS
:     CMP   #EDITUI_CMD_OPEN
      BNE   :+
      JSR   editbuf_do_host_command
      CLC
      RTS
:     CMP   #EDITUI_CMD_SAVE_AS
      BNE   :+
      JSR   editbuf_do_host_command
      CLC
      RTS
:     CMP   #EDITUI_CMD_SAVE
      BNE   :+
      JSR   editbuf_do_save
      CLC
      RTS
:     CMP   #EDITUI_CMD_QUIT
      BNE   :+
      JMP   editbuf_do_quit          ; propagates carry (set = exit)
:     CMP   #EDITUI_CMD_UNDO
      BNE   :+
      JSR   editbuf_do_undo
      CLC
      RTS
:     CMP   #EDITUI_CMD_REDO
      BNE   :+
      JSR   editbuf_do_redo
      CLC
      RTS
:     CMP   #EDITUI_CMD_COPY
      BNE   :+
      JSR   editbuf_copy
      CLC
      RTS
:     CMP   #EDITUI_CMD_CUT
      BNE   :+
      JSR   editbuf_cut
      CLC
      RTS
:     CMP   #EDITUI_CMD_PASTE
      BNE   :+
      JSR   editbuf_paste
      CLC
      RTS
:     CMP   #EDITUI_CMD_SELECT_ALL
      BNE   :+
      JSR   editbuf_select_all
      CLC
      RTS
:     CMP   #EDITUI_CMD_FIND
      BNE   :+
      JSR   editbuf_do_find
      CLC
      RTS
:     CMP   #EDITUI_CMD_FIND_NEXT
      BNE   :+
      JSR   editbuf_do_find_next
      CLC
      RTS
:     CMP   #EDITUI_CMD_REPLACE
      BNE   :+
      JSR   editbuf_do_replace
      CLC
      RTS
:     CMP   #EDITUI_CMD_REPLACE_ALL
      BNE   :+
      JSR   editbuf_do_replace_all
      CLC
      RTS
:     CMP   #EDITUI_CMD_GOTO_LINE
      BNE   :+
      JSR   editbuf_do_goto_line
      CLC
      RTS
:     CMP   #EDITUI_CMD_BUFFER_NEXT
      BNE   :+
      JSR   editbuf_do_host_command
      CLC
      RTS
:     CMP   #EDITUI_CMD_BUFFER_PREVIOUS
      BNE   :+
      JSR   editbuf_do_host_command
      CLC
      RTS
:     CMP   #EDITUI_CMD_BUFFER_LIST
      BNE   :+
      JSR   editbuf_do_host_command
      CLC
      RTS
:
      CLC
      RTS

editbuf_key_to_command:
      CMP   #EDITUI_KEY_CTRL_Q
      BNE   :+
      LDA   #EDITUI_CMD_QUIT
      RTS
:
      CMP   #EDITUI_KEY_CTRL_B
      BNE   :+
      LDA   #EDITUI_CMD_BUFFER_LIST
      RTS
:
      CMP   #EDITUI_KEY_CTRL_N
      BNE   :+
      LDA   #EDITUI_CMD_NEW
      RTS
:     CMP   #EDITUI_KEY_CTRL_O
      BNE   :+
      LDA   #EDITUI_CMD_OPEN
      RTS
:     CMP   #EDITUI_KEY_CTRL_S
      BNE   :+
      LDA   #EDITUI_CMD_SAVE
      RTS
:     CMP   #EDITUI_KEY_CTRL_Z
      BNE   :+
      LDA   #EDITUI_CMD_UNDO
      RTS
:     CMP   #EDITUI_KEY_CTRL_Y
      BNE   :+
      LDA   #EDITUI_CMD_REDO
      RTS
:     CMP   #EDITUI_KEY_CTRL_C
      BNE   :+
      LDA   #EDITUI_CMD_COPY
      RTS
:     CMP   #EDITUI_KEY_CTRL_X
      BNE   :+
      LDA   #EDITUI_CMD_CUT
      RTS
:     CMP   #EDITUI_KEY_CTRL_V
      BNE   :+
      LDA   #EDITUI_CMD_PASTE
      RTS
:     CMP   #EDITUI_KEY_CTRL_A
      BNE   :+
      LDA   #EDITUI_CMD_SELECT_ALL
      RTS
:     CMP   #EDITUI_KEY_CTRL_F
      BNE   :+
      LDA   #EDITUI_CMD_FIND
      RTS
:     CMP   #EDITUI_KEY_CTRL_G
      BNE   :+
      LDA   #EDITUI_CMD_GOTO_LINE
      RTS
:     CMP   #EDITUI_KEY_F3
      BNE   :+
      LDA   #EDITUI_CMD_FIND_NEXT
      RTS
:     CMP   #EDITUI_KEY_F6
      BNE   :+
      LDA   #EDITUI_CMD_BUFFER_NEXT
      RTS
:     CMP   #EDITUI_KEY_SHIFT_F6
      BNE   :+
      LDA   #EDITUI_CMD_BUFFER_PREVIOUS
      RTS
:     CMP   #EDITUI_KEY_ALT_PREFIX
      BNE   @none
      JSR   editbuf_read_command_key
      JSR   editbuf_normalize_command_key
      CMP   #'x'
      BNE   :+
      LDA   #EDITUI_CMD_QUIT
      RTS
:     CMP   #'0'
      BNE   :+
      LDA   #EDITUI_CMD_BUFFER_LIST
      RTS
:     JSR   editui_menu_open_hotkey
      RTS
@none:
      LDA   #EDITUI_CMD_NONE
      RTS

editbuf_read_command_key:
      LDA   VGC_FRAME
      CLC
      ADC   #EDITBUF_ALT_TIMEOUT_FRAMES
      STA   EB_T1
@wait:
      LDA   VGC_CHARIN
      BNE   @got
      LDA   VGC_FRAME
      CMP   EB_T1
      BNE   @wait
      LDA   #0
@got:
      RTS

editbuf_normalize_command_key:
      CMP   #'A'
      BCC   @done
      CMP   #'Z' + 1
      BCS   @done
      ORA   #$20
@done:
      RTS

; ---------------------------------------------------------------------
; editbuf_apply_status — show a transient message if EDITBUF_STATUS is set,
; otherwise generate the editor metadata bar.
; ---------------------------------------------------------------------
editbuf_apply_status:
      LDA   EDITBUF_STATUSL
      ORA   EDITBUF_STATUSH
      BNE   @request
      LDA   EB_STATUS_TIMERL
      ORA   EB_STATUS_TIMERH
      BNE   @point
      JMP   editbuf_update_status
@request:
      LDA   EDITBUF_STATUSL
      STA   EB_STATUS_ACTIVEL
      LDA   EDITBUF_STATUSH
      STA   EB_STATUS_ACTIVEH
      STZ   EDITBUF_STATUSL
      STZ   EDITBUF_STATUSH
      LDA   #<EDITBUF_STATUS_TICKS
      STA   EB_STATUS_TIMERL
      LDA   #>EDITBUF_STATUS_TICKS
      STA   EB_STATUS_TIMERH
      LDA   VGC_FRAME
      STA   EB_STATUS_LASTFRAME
@point:
      LDA   EB_STATUS_ACTIVEL
      STA   EDITUI_STATUSL
      LDA   EB_STATUS_ACTIVEH
      STA   EDITUI_STATUSH
      RTS

editbuf_status_tick:
      LDA   EB_STATUS_TIMERL
      ORA   EB_STATUS_TIMERH
      BNE   :+
      RTS
:     LDA   VGC_FRAME
      CMP   EB_STATUS_LASTFRAME
      BNE   :+
      RTS
:     STA   EB_STATUS_LASTFRAME
      LDA   EB_STATUS_TIMERL
      BNE   @dec_lo
      DEC   EB_STATUS_TIMERH
@dec_lo:
      DEC   EB_STATUS_TIMERL
      LDA   EB_STATUS_TIMERL
      ORA   EB_STATUS_TIMERH
      BEQ   @expired
      RTS
@expired:
      STZ   EDITBUF_STATUSL
      STZ   EDITBUF_STATUSH
      STZ   EB_STATUS_ACTIVEL
      STZ   EB_STATUS_ACTIVEH
      JSR   editbuf_update_status
      JSR   editui_draw_status
      JMP   editbuf_place_cursor

editbuf_set_default_saved_status:
      LDA   EDITBUF_STATUSL
      ORA   EDITBUF_STATUSH
      BNE   @done
      LDA   #<editbuf_msg_saved
      STA   EDITBUF_STATUSL
      LDA   #>editbuf_msg_saved
      STA   EDITBUF_STATUSH
@done:
      RTS

editbuf_set_default_save_error_status:
      LDA   EDITBUF_STATUSL
      ORA   EDITBUF_STATUSH
      BNE   @done
      LDA   #<editbuf_msg_save_failed
      STA   EDITBUF_STATUSL
      LDA   #>editbuf_msg_save_failed
      STA   EDITBUF_STATUSH
@done:
      RTS

editbuf_build_title:
      LDA   EDITBUF_TITLEL
      ORA   EDITBUF_TITLEH
      BNE   :+
      STZ   EB_TITLEBUF
      RTS
:
      LDA   EDITBUF_TITLEL
      STA   EB_PL
      LDA   EDITBUF_TITLEH
      STA   EB_PH
      LDA   #$FF
      STA   EB_TITLE_LASTSEP
      STA   EB_TITLE_PREVSEP
      LDY   #0
@scan:
      LDA   (EB_PL),Y
      BEQ   @scan_done
      CMP   #'/'
      BEQ   @sep
      CMP   #$5C
      BEQ   @sep
      BRA   @scan_next
@sep:
      LDA   EB_TITLE_LASTSEP
      STA   EB_TITLE_PREVSEP
      STY   EB_TITLE_LASTSEP
@scan_next:
      INY
      CPY   #127
      BCC   @scan
@scan_done:
      STZ   EB_TITLE_START
      LDA   EB_TITLE_LASTSEP
      CMP   #$FF
      BEQ   @copy
      LDA   EB_TITLE_PREVSEP
      CMP   #$FF
      BEQ   @copy
      INC   A
      STA   EB_TITLE_START
@copy:
      LDX   #0
      LDY   EB_TITLE_START
@copy_loop:
      CPX   #56
      BCS   @term
      LDA   (EB_PL),Y
      BEQ   @term
      STA   EB_TITLEBUF,X
      INX
      INY
      BRA   @copy_loop
@term:
      LDA   #0
      STA   EB_TITLEBUF,X
      RTS

editbuf_recount_lines:
      JSR   editbuf_count_lines
      LDA   EB_T0
      STA   EB_TOTAL_LINESL
      LDA   EB_T1
      STA   EB_TOTAL_LINESH
      RTS

editbuf_update_status:
      LDA   #<EB_STATUSBUF
      STA   EB_PL
      LDA   #>EB_STATUSBUF
      STA   EB_PH
      LDA   #<editbuf_status_x
      LDY   #>editbuf_status_x
      JSR   editbuf_status_append_ptr
      LDA   EB_CURCOL
      CLC
      ADC   #1
      LDX   #0
      JSR   editbuf_status_append_dec16
      LDA   #<editbuf_status_y
      LDY   #>editbuf_status_y
      JSR   editbuf_status_append_ptr
      LDA   EB_CURLINEL
      CLC
      ADC   #1
      PHA
      LDA   EB_CURLINEH
      ADC   #0
      TAX
      PLA
      JSR   editbuf_status_append_dec16
      LDA   #<editbuf_status_bytes
      LDY   #>editbuf_status_bytes
      JSR   editbuf_status_append_ptr
      LDA   EDITBUF_LENL
      LDX   EDITBUF_LENH
      JSR   editbuf_status_append_dec16
      LDA   #<editbuf_status_lines
      LDY   #>editbuf_status_lines
      JSR   editbuf_status_append_ptr
      LDA   EB_TOTAL_LINESL
      LDX   EB_TOTAL_LINESH
      JSR   editbuf_status_append_dec16
      LDA   #<editbuf_status_type
      LDY   #>editbuf_status_type
      JSR   editbuf_status_append_ptr
      LDA   EDITBUF_TYPEL
      ORA   EDITBUF_TYPEH
      BEQ   @done_type
      LDA   EDITBUF_TYPEL
      LDY   EDITBUF_TYPEH
      JSR   editbuf_status_append_ptr
@done_type:
      LDA   #0
      JSR   editbuf_status_putc
      LDA   #<EB_STATUSBUF
      STA   EDITUI_STATUSL
      LDA   #>EB_STATUSBUF
      STA   EDITUI_STATUSH
      RTS

editbuf_finish_screen:
      JSR   editbuf_apply_status
      JSR   editui_draw_status
      JMP   editbuf_place_cursor

editbuf_status_append_ptr:
      STA   EB_SRCL
      STY   EB_SRCH
      LDY   #0
@loop:
      LDA   (EB_SRCL),Y
      BEQ   @done
      PHY
      JSR   editbuf_status_putc
      PLY
      INY
      BRA   @loop
@done:
      RTS

editbuf_status_putc:
      LDY   #0
      STA   (EB_PL),Y
      INC   EB_PL
      BNE   :+
      INC   EB_PH
:     RTS

editbuf_status_append_dec16:
      STA   EB_STATUS_NUML
      STX   EB_STATUS_NUMH
      STZ   EB_STATUS_STARTED
      STZ   EB_STATUS_IDX
@digit:
      STZ   EB_STATUS_DIGIT
@subtract:
      LDY   EB_STATUS_IDX
      LDA   EB_STATUS_NUML
      SEC
      SBC   editbuf_pow10_lo,Y
      STA   EB_T0
      LDA   EB_STATUS_NUMH
      SBC   editbuf_pow10_hi,Y
      BCC   @emit
      STA   EB_STATUS_NUMH
      LDA   EB_T0
      STA   EB_STATUS_NUML
      INC   EB_STATUS_DIGIT
      BRA   @subtract
@emit:
      LDA   EB_STATUS_DIGIT
      BNE   @write
      LDA   EB_STATUS_STARTED
      BNE   @write
      LDA   EB_STATUS_IDX
      CMP   #4
      BEQ   @write
      BRA   @next
@write:
      LDA   #1
      STA   EB_STATUS_STARTED
      LDA   EB_STATUS_DIGIT
      CLC
      ADC   #'0'
      JSR   editbuf_status_putc
@next:
      INC   EB_STATUS_IDX
      LDA   EB_STATUS_IDX
      CMP   #5
      BCC   @digit
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
      JSR   editbuf_capture_undo
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
      JSR   editbuf_capture_undo
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
      LDA   #1
      STA   EB_VSHIFT
      JMP   editbuf_after_change_down   ; newline shifts lines below it down
@full:
      RTS

; editbuf_backspace — delete char before cursor.
editbuf_backspace:
      LDA   EDITBUF_SELACT
      BEQ   @nosel
      JSR   editbuf_capture_undo
      JSR   editbuf_delete_selection
      JMP   editbuf_after_change
@nosel:
      LDA   EDITBUF_CURL
      ORA   EDITBUF_CURH
      BNE   @local
      LDA   #EDITUI_CMD_WINDOW_PREVIOUS
      JSR   editbuf_call_command
      LDA   EDITBUF_CURL
      ORA   EDITBUF_CURH
      BEQ   @done                 ; at start
@local:
      JSR   editbuf_capture_undo
      JSR   editbuf_cursor_dec
      ; peek the byte about to be removed (now at the cursor) to tell a line
      ; join (\n) from an in-line delete.
      LDX   EDITBUF_CURL
      LDA   EDITBUF_CURH
      JSR   editbuf_ptr_from_off
      LDY   #0
      LDA   (EB_PL),Y
      PHA                         ; removed char
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
      PLA
      CMP   #$0A
      BNE   @inline
      LDA   #$FF
      STA   EB_VSHIFT
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
      JSR   editbuf_capture_undo
      JSR   editbuf_delete_selection
      JMP   editbuf_after_change
@nosel:
      ; cursor < len ?
      LDA   EDITBUF_CURL
      CMP   EDITBUF_LENL
      LDA   EDITBUF_CURH
      SBC   EDITBUF_LENH
      BCS   @done                 ; cursor >= len
      JSR   editbuf_capture_undo
      ; peek the byte at the cursor to tell a line join (\n) from an in-line delete.
      LDX   EDITBUF_CURL
      LDA   EDITBUF_CURH
      JSR   editbuf_ptr_from_off
      LDY   #0
      LDA   (EB_PL),Y
      PHA                         ; removed char
      LDA   EDITBUF_CURL
      STA   EB_SCRATCHL
      LDA   EDITBUF_CURH
      STA   EB_SCRATCHH
      LDA   #1
      STA   EDITBUF_CNTL
      STZ   EDITBUF_CNTH
      JSR   editbuf_close_gap
      JSR   editbuf_mark_dirty
      PLA
      CMP   #$0A
      BNE   @inline
      LDA   #$FF
      STA   EB_VSHIFT
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
      BNE   :+
      LDA   #1
      RTS
:
      JSR   editbuf_normalize_selection
      ; count = end - start
      SEC
      LDA   EB_SELENDL
      SBC   EB_SELSTARTL
      STA   XMC_LENL
      LDA   EB_SELENDH
      SBC   EB_SELSTARTH
      STA   XMC_LENH
      LDA   XMC_LENL
      ORA   XMC_LENH
      BNE   :+
      LDA   #1
      RTS
:
      ; src ptr = BUF + selstart
      LDA   EB_SELSTARTL
      STA   EB_SCRATCHL
      LDA   EB_SELSTARTH
      STA   EB_SCRATCHH
      JSR   editbuf_src_from_scratch
      LDA   EB_SRCL
      STA   XMC_RAML
      LDA   EB_SRCH
      STA   XMC_RAMH
      JMP   textsvc_clip_copy_from_ram

editbuf_cut:
      LDA   EDITBUF_SELACT
      BEQ   @done
      JSR   editbuf_copy
      BNE   @done
      JSR   editbuf_capture_undo
      JSR   editbuf_delete_selection
      JMP   editbuf_after_change
@done:
      RTS

; editbuf_paste — insert clipboard bytes at cursor.
editbuf_paste:
      LDA   EDITBUF_SELACT
      BNE   @snapshot
      LDA   TEXTSVC_CLIPLENL
      ORA   TEXTSVC_CLIPLENH
      BNE   @snapshot
      RTS
@snapshot:
      JSR   editbuf_capture_undo
      JSR   editbuf_clear_selection
      LDA   TEXTSVC_CLIPLENL
      ORA   TEXTSVC_CLIPLENH
      BNE   :+
      JMP   editbuf_after_change
:
      ; room? LEN + cliplen <= CAP
      CLC
      LDA   EDITBUF_LENL
      ADC   TEXTSVC_CLIPLENL
      STA   EB_T0
      LDA   EDITBUF_LENH
      ADC   TEXTSVC_CLIPLENH
      STA   EB_T1
      LDA   EDITBUF_CAPL
      CMP   EB_T0
      LDA   EDITBUF_CAPH
      SBC   EB_T1
      BCC   @done                 ; would overflow cap
      ; make gap of cliplen at cursor
      LDA   TEXTSVC_CLIPLENL
      STA   EDITBUF_CNTL
      LDA   TEXTSVC_CLIPLENH
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
      LDA   EB_DSTL
      STA   XMC_RAML
      LDA   EB_DSTH
      STA   XMC_RAMH
      JSR   textsvc_clip_fetch_to_ram
      BEQ   @adv
      LDA   EDITBUF_CURL
      STA   EB_SCRATCHL
      LDA   EDITBUF_CURH
      STA   EB_SCRATCHH
      LDA   TEXTSVC_CLIPLENL
      STA   EDITBUF_CNTL
      LDA   TEXTSVC_CLIPLENH
      STA   EDITBUF_CNTH
      JSR   editbuf_close_gap
      RTS
@adv:
      ; cursor += cliplen
      CLC
      LDA   EDITBUF_CURL
      ADC   TEXTSVC_CLIPLENL
      STA   EDITBUF_CURL
      LDA   EDITBUF_CURH
      ADC   TEXTSVC_CLIPLENH
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
      BNE   @local
      LDA   #EDITUI_CMD_WINDOW_PREVIOUS
      JSR   editbuf_call_command
      LDA   EDITBUF_CURL
      ORA   EDITBUF_CURH
      BEQ   @done
      JSR   editbuf_cursor_dec
      JMP   editbuf_after_host_command
@local:
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
      BCC   @local
      LDA   #EDITUI_CMD_WINDOW_NEXT
      JMP   editbuf_do_host_command
@local:
      JSR   editbuf_cursor_inc
      JSR   editbuf_update_goalcol
      JMP   editbuf_after_move

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

editbuf_move_file_start:
      JSR   editbuf_begin_move
      LDA   #EDITUI_CMD_WINDOW_FIRST
      JSR   editbuf_call_command
      STZ   EDITBUF_CURL
      STZ   EDITBUF_CURH
      STZ   EB_GOALCOL
      JMP   editbuf_after_host_command

editbuf_move_file_end:
      JSR   editbuf_begin_move
      LDA   #EDITUI_CMD_WINDOW_LAST
      JSR   editbuf_call_command
      LDA   EDITBUF_LENL
      STA   EDITBUF_CURL
      LDA   EDITBUF_LENH
      STA   EDITBUF_CURH
      JSR   editbuf_update_goalcol
      JMP   editbuf_after_host_command

editbuf_move_up:
      JSR   editbuf_begin_move
      JSR   editbuf_compute_linecol
      LDA   EB_CURLINEL
      ORA   EB_CURLINEH
      BNE   @local
      LDA   #EDITUI_CMD_WINDOW_PREVIOUS
      JMP   editbuf_do_host_command
@local:
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

editbuf_move_down:
      JSR   editbuf_begin_move
      ; Find the next line by scanning forward from the cursor to the next LF.
      ; This keeps repeated down-scroll O(current-line length), not O(file
      ; position), which matters once the buffer is hundreds of lines deep.
      LDA   EDITBUF_CURL
      STA   EB_SCRATCHL
      LDA   EDITBUF_CURH
      STA   EB_SCRATCHH
@find_next:
      LDA   EB_SCRATCHL
      CMP   EDITBUF_LENL
      LDA   EB_SCRATCHH
      SBC   EDITBUF_LENH
      BCC   :+
      LDA   #EDITUI_CMD_WINDOW_NEXT
      JMP   editbuf_do_host_command
:
      LDX   EB_SCRATCHL
      LDA   EB_SCRATCHH
      JSR   editbuf_ptr_from_off
      LDY   #0
      LDA   (EB_PL),Y
      INC   EB_SCRATCHL
      BNE   :+
      INC   EB_SCRATCHH
:     CMP   #$0A
      BNE   @find_next

      ; EB_SCRATCH now points at the target line's first byte.
      LDA   EB_SCRATCHL
      STA   EB_LINEOFFL
      STA   EDITBUF_CURL
      LDA   EB_SCRATCHH
      STA   EB_LINEOFFH
      STA   EDITBUF_CURH
      INC   EB_CURLINEL
      BNE   :+
      INC   EB_CURLINEH
:     STZ   EB_CURCOL
      LDA   EB_GOALCOL
      STA   EB_T0
      BEQ   @moved
@adv:
      LDA   EDITBUF_CURL
      CMP   EDITBUF_LENL
      LDA   EDITBUF_CURH
      SBC   EDITBUF_LENH
      BCS   @moved                ; target line ended at EOF
      LDX   EDITBUF_CURL
      LDA   EDITBUF_CURH
      JSR   editbuf_ptr_from_off
      LDY   #0
      LDA   (EB_PL),Y
      CMP   #$0A
      BEQ   @moved
      JSR   editbuf_cursor_inc
      INC   EB_CURCOL
      DEC   EB_T0
      BNE   @adv
@moved:
      JMP   editbuf_after_move_known

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
      JSR   editbuf_call_changed
      JSR   editbuf_recount_lines
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
editbuf_after_move_known:
      LDA   EB_TOPLINEL
      STA   EB_PREVTOPL
      LDA   EB_TOPLINEH
      STA   EB_PREVTOPH
      LDA   EB_LEFTCOL
      STA   EB_PREVLEFT
      JSR   editbuf_adjust_scroll
      LDA   EB_PREVSEL                ; had a selection -> repaint to clear it
      BEQ   :+
      JMP   @full
:
      LDA   EB_LEFTCOL
      CMP   EB_PREVLEFT
      BEQ   :+
      JMP   @full
:
      LDA   EB_TOPLINEL
      CMP   EB_PREVTOPL
      BNE   @maybe_scroll
      LDA   EB_TOPLINEH
      CMP   EB_PREVTOPH
      BNE   @maybe_scroll
      JMP   editbuf_finish_screen
@maybe_scroll:
      ; Topline +1: document moved down, visible rows shift up.
      LDA   EB_PREVTOPL
      CLC
      ADC   #1
      STA   EB_T0
      LDA   EB_PREVTOPH
      ADC   #0
      CMP   EB_TOPLINEH
      BNE   @full
      LDA   EB_T0
      CMP   EB_TOPLINEL
      BNE   @check_scroll_down
      JSR   editbuf_ring_scroll_up_one
      LDA   #EDITBUF_VIEW_ROWS-1
      STA   EB_ROW
      LDA   EB_LINEOFFL
      STA   EB_VISLINEL
      LDA   EB_LINEOFFH
      STA   EB_VISLINEH
      STZ   EB_PAINTSTART
      JSR   editbuf_render_row
      JMP   editbuf_finish_screen
@check_scroll_down:
      ; Topline -1: document moved up, visible rows shift down.
      LDA   EB_TOPLINEL
      CLC
      ADC   #1
      STA   EB_T0
      LDA   EB_TOPLINEH
      ADC   #0
      CMP   EB_PREVTOPH
      BNE   @full
      LDA   EB_T0
      CMP   EB_PREVTOPL
      BNE   @full
      JSR   editbuf_ring_scroll_down_one
      STZ   EB_ROW
      LDA   EB_LINEOFFL
      STA   EB_VISLINEL
      LDA   EB_LINEOFFH
      STA   EB_VISLINEH
      STZ   EB_PAINTSTART
      JSR   editbuf_render_row
      JMP   editbuf_finish_screen
@full:
      JMP   editbuf_render

; editbuf_after_inline_edit — for edits confined to the cursor's line (printable
; insert, mid-line delete/backspace). EB_EDITDELTA holds how many columns left of
; the cursor the changed run begins (1 after an insert, 0 after a delete). When
; the scroll didn't move, repaint ONLY the changed run [start..VIEW_COLS) of the
; cursor's row; otherwise a full repaint.
editbuf_after_inline_edit:
      JSR   editbuf_call_changed
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
      JMP   editbuf_finish_screen

; editbuf_after_change_down — for edits that shift text vertically (newline,
; line-join delete/backspace): repaint from the changed line to the bottom of
; the view (a "local" repaint, not the whole screen), or full-repaint if the
; scroll moved.
editbuf_after_change_down:
      JSR   editbuf_call_changed
      JSR   editbuf_recount_lines
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
      BEQ   :+
      JMP   @full
:     LDA   EB_TOPLINEH
      CMP   EB_PREVTOPH
      BEQ   :+
      JMP   @full
:     LDA   EB_LEFTCOL
      CMP   EB_PREVLEFT
      BEQ   :+
      JMP   @full
:
      ; Repaint from the changed row down. This is slower than a row-copy repair,
      ; but it is correct for the text-ring mapping used by zero-copy scrolling.
      LDA   EB_CURLINEL
      ORA   EB_CURLINEH
      BEQ   @top_render
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
@top_render:
      STZ   EB_ROW                     ; clamp to the top visible row
@render:
      STZ   EB_VSHIFT
      JMP   editbuf_render_from_row
@full:
      STZ   EB_VSHIFT
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
      JMP   editbuf_finish_screen

; editbuf_render_row_count — repaint EB_BLT_HEIGHT visible rows starting at
; EB_ROW, then refresh chrome and cursor.
editbuf_render_row_count:
      JSR   editbuf_render_rows
      JMP   editbuf_finish_screen

; editbuf_render_rows — repaint EB_BLT_HEIGHT visible rows starting at EB_ROW.
; Returns without refreshing status/cursor so callers can compose sparse repairs.
editbuf_render_rows:
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
      LDA   EB_BLT_HEIGHT
      BEQ   @done
      LDA   EB_ROW
      CMP   #EDITBUF_VIEW_ROWS
      BCS   @done
      JSR   editbuf_render_row
      INC   EB_ROW
      DEC   EB_BLT_HEIGHT
      BRA   @loop
@done:
      RTS

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
      JMP   editbuf_finish_screen

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
      JSR   editbuf_visible_to_physical
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

; =====================================================================
; Blitter row shifts for editor viewport movement.
; =====================================================================

editbuf_configure_scroll_window:
      STZ   VTEXT_TOPROW
      STZ   VGC_TEXT_TOPROW
      LDA   #EDITBUF_VIEW_TOP
      STA   VTEXT_SCROLL_TOP
      STA   VGC_TEXT_SCROLL_START
      LDA   #EDITBUF_VIEW_ROWS
      STA   VTEXT_SCROLL_ROWS
      STA   VGC_TEXT_SCROLL_ROWS
      RTS

; Wait for the frame counter to advance before issuing a visible row move.
editbuf_wait_vblank:
      LDA   VGC_FRAME
@wait:
      CMP   VGC_FRAME
      BEQ   @wait
      RTS

editbuf_ring_scroll_up_one:
      JSR   editbuf_wait_vblank
      INC   VTEXT_TOPROW
      LDA   VTEXT_TOPROW
      CMP   #EDITBUF_VIEW_ROWS
      BCC   :+
      STZ   VTEXT_TOPROW
      LDA   #0
:     STA   VGC_TEXT_TOPROW
      RTS

editbuf_ring_scroll_down_one:
      JSR   editbuf_wait_vblank
      LDA   VTEXT_TOPROW
      BNE   :+
      LDA   #EDITBUF_VIEW_ROWS
      STA   VTEXT_TOPROW
:     DEC   VTEXT_TOPROW
      LDA   VTEXT_TOPROW
      STA   VGC_TEXT_TOPROW
      RTS

; A = visible display row -> A = physical text-plane row through the ring base.
editbuf_visible_to_physical:
      CMP   #EDITBUF_VIEW_TOP
      BCC   @done
      CMP   #EDITUI_STATUS_ROW
      BCS   @done
      SEC
      SBC   #EDITBUF_VIEW_TOP
      CLC
      ADC   VTEXT_TOPROW
      CMP   #EDITBUF_VIEW_ROWS
      BCC   :+
      SBC   #EDITBUF_VIEW_ROWS
:     CLC
      ADC   #EDITBUF_VIEW_TOP
@done:
      RTS

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
; Undo / redo snapshots
; =====================================================================

editbuf_capture_undo:
      LDA   EDITBUF_BUFL
      STA   XMC_RAML
      LDA   EDITBUF_BUFH
      STA   XMC_RAMH
      LDA   EDITBUF_LENL
      STA   XMC_LENL
      LDA   EDITBUF_LENH
      STA   XMC_LENH
      LDA   EDITBUF_CURL
      STA   TEXTSVC_CURL
      LDA   EDITBUF_CURH
      STA   TEXTSVC_CURH
      JMP   textsvc_undo_capture

editbuf_do_undo:
      LDA   EDITBUF_BUFL
      STA   XMC_RAML
      LDA   EDITBUF_BUFH
      STA   XMC_RAMH
      LDA   EDITBUF_LENL
      STA   XMC_LENL
      LDA   EDITBUF_LENH
      STA   XMC_LENH
      LDA   EDITBUF_CURL
      STA   TEXTSVC_CURL
      LDA   EDITBUF_CURH
      STA   TEXTSVC_CURH
      JSR   textsvc_undo_apply
      BNE   @done
      LDA   TEXTSVC_REST_LENL
      STA   EDITBUF_LENL
      LDA   TEXTSVC_REST_LENH
      STA   EDITBUF_LENH
      LDA   TEXTSVC_REST_CURL
      STA   EDITBUF_CURL
      LDA   TEXTSVC_REST_CURH
      STA   EDITBUF_CURH
      STZ   EDITBUF_SELACT
      JSR   editbuf_mark_dirty
      JMP   editbuf_after_change
@done:
      RTS

editbuf_do_redo:
      LDA   EDITBUF_BUFL
      STA   XMC_RAML
      LDA   EDITBUF_BUFH
      STA   XMC_RAMH
      LDA   EDITBUF_LENL
      STA   XMC_LENL
      LDA   EDITBUF_LENH
      STA   XMC_LENH
      LDA   EDITBUF_CURL
      STA   TEXTSVC_CURL
      LDA   EDITBUF_CURH
      STA   TEXTSVC_CURH
      JSR   textsvc_redo_apply
      BNE   @done
      LDA   TEXTSVC_REST_LENL
      STA   EDITBUF_LENL
      LDA   TEXTSVC_REST_LENH
      STA   EDITBUF_LENH
      LDA   TEXTSVC_REST_CURL
      STA   EDITBUF_CURL
      LDA   TEXTSVC_REST_CURH
      STA   EDITBUF_CURH
      STZ   EDITBUF_SELACT
      JSR   editbuf_mark_dirty
      JMP   editbuf_after_change
@done:
      RTS

; =====================================================================
; Find / goto-line requesters
; =====================================================================
editbuf_do_find:
      LDA   #<editbuf_find_msg
      STA   EB_PROMPT_MSGL
      LDA   #>editbuf_find_msg
      STA   EB_PROMPT_MSGH
      LDA   #<editbuf_find_label
      STA   EB_PROMPT_LABELL
      LDA   #>editbuf_find_label
      STA   EB_PROMPT_LABELH
      JSR   editbuf_prompt_line
      BCC   @done
      JSR   editbuf_store_prompt_find
      BCC   @done
      JSR   editbuf_find_stored_wrapped_from_cursor
      BCC   @done
      JSR   editbuf_begin_move
      JSR   editbuf_update_goalcol
      JMP   editbuf_after_move
@done:
      RTS

editbuf_do_find_next:
      LDA   EB_FINDLEN
      BNE   :+
      JMP   editbuf_do_find
:     CLC
      LDA   EDITBUF_CURL
      ADC   #1
      STA   EB_SCRATCHL
      LDA   EDITBUF_CURH
      ADC   #0
      STA   EB_SCRATCHH
      JSR   editbuf_find_stored_from_scratch
      BCS   @found
      STZ   EB_SCRATCHL
      STZ   EB_SCRATCHH
      JSR   editbuf_find_stored_from_scratch
      BCC   @done
@found:
      JSR   editbuf_begin_move
      JSR   editbuf_update_goalcol
      JMP   editbuf_after_move
@done:
      RTS

editbuf_do_replace:
      LDA   #<editbuf_replace_msg
      STA   EB_PROMPT_MSGL
      LDA   #>editbuf_replace_msg
      STA   EB_PROMPT_MSGH
      LDA   #<editbuf_find_label
      STA   EB_PROMPT_LABELL
      LDA   #>editbuf_find_label
      STA   EB_PROMPT_LABELH
      JSR   editbuf_prompt_line
      BCC   @done
      JSR   editbuf_store_prompt_find
      BCC   @done
      LDA   #<editbuf_replace_msg
      STA   EB_PROMPT_MSGL
      LDA   #>editbuf_replace_msg
      STA   EB_PROMPT_MSGH
      LDA   #<editbuf_replace_label
      STA   EB_PROMPT_LABELL
      LDA   #>editbuf_replace_label
      STA   EB_PROMPT_LABELH
      JSR   editbuf_prompt_line
      BCC   @done
      JSR   editbuf_find_stored_text
      BCC   @done
      JSR   editbuf_replace_match
@done:
      RTS

editbuf_do_replace_all:
      LDA   #<editbuf_replace_all_msg
      STA   EB_PROMPT_MSGL
      LDA   #>editbuf_replace_all_msg
      STA   EB_PROMPT_MSGH
      LDA   #<editbuf_find_label
      STA   EB_PROMPT_LABELL
      LDA   #>editbuf_find_label
      STA   EB_PROMPT_LABELH
      JSR   editbuf_prompt_line
      BCC   @done
      JSR   editbuf_store_prompt_find
      BCC   @done
      LDA   #<editbuf_replace_all_msg
      STA   EB_PROMPT_MSGL
      LDA   #>editbuf_replace_all_msg
      STA   EB_PROMPT_MSGH
      LDA   #<editbuf_replace_label
      STA   EB_PROMPT_LABELL
      LDA   #>editbuf_replace_label
      STA   EB_PROMPT_LABELH
      JSR   editbuf_prompt_line
      BCC   @done
      STZ   EB_REPLACE_CHANGED
      STZ   EB_SCRATCHL
      STZ   EB_SCRATCHH
@loop:
      JSR   editbuf_find_stored_from_scratch
      BCC   @finish
      LDA   EB_REPLACE_CHANGED
      BNE   @replace
      JSR   editbuf_capture_undo
@replace:
      JSR   editbuf_replace_match_raw
      BCC   @finish
      LDA   #1
      STA   EB_REPLACE_CHANGED
      LDA   EDITBUF_CURL
      STA   EB_SCRATCHL
      LDA   EDITBUF_CURH
      STA   EB_SCRATCHH
      BRA   @loop
@finish:
      LDA   EB_REPLACE_CHANGED
      BEQ   @done
      JSR   editbuf_mark_dirty
      JMP   editbuf_after_change
@done:
      RTS

editbuf_do_goto_line:
      LDA   #<editbuf_goto_msg
      STA   EB_PROMPT_MSGL
      LDA   #>editbuf_goto_msg
      STA   EB_PROMPT_MSGH
      LDA   #<editbuf_goto_label
      STA   EB_PROMPT_LABELL
      LDA   #>editbuf_goto_label
      STA   EB_PROMPT_LABELH
      JSR   editbuf_prompt_line
      BCC   @done
      JSR   editbuf_parse_goto_line
      BCC   @done
      JSR   editbuf_begin_move
      STZ   EB_GOALCOL
      JSR   editbuf_goto_line_col
      JMP   editbuf_after_move
@done:
      RTS

editbuf_prompt_line:
      STZ   EB_PROMPTLEN
      STZ   EB_PROMPTBUF
      JSR   nui_dialog_defaults
      LDA   #14
      STA   NUI_DIALOG_LEFT
      LDA   #17
      STA   NUI_DIALOG_TOP
      LDA   #52
      STA   NUI_DIALOG_WIDTH
      LDA   #12
      STA   NUI_DIALOG_HEIGHT
      LDA   EB_PROMPT_MSGL
      STA   NUI_TITLEL
      LDA   EB_PROMPT_MSGH
      STA   NUI_TITLEH
      STZ   NUI_MSGL
      STZ   NUI_MSGH
      STZ   NUI_FOOTERL
      STZ   NUI_FOOTERH
      LDA   EB_PROMPT_LABELL
      STA   NUI_INPUT_LABELL
      LDA   EB_PROMPT_LABELH
      STA   NUI_INPUT_LABELH
      LDA   #<EB_PROMPTBUF
      STA   NUI_INPUT_OUTL
      LDA   #>EB_PROMPTBUF
      STA   NUI_INPUT_OUTH
      LDA   #32
      STA   NUI_INPUT_OUT_MAX
      JSR   nui_text_input
      BNE   @cancel
      LDA   NUI_RESULT
      CMP   #NUI_RESULT_OK
      BNE   @cancel
      LDA   NUI_INPUT_OUT_LEN
      STA   EB_PROMPTLEN
      SEC
      BRA   @restore
@cancel:
      CLC
@restore:
      PHP
      JSR   editbuf_redraw_all
      JSR   editbuf_apply_status
      JSR   editui_draw_status
      JSR   editbuf_place_cursor
      PLP
      RTS

editbuf_store_prompt_find:
      LDA   EB_PROMPTLEN
      BNE   :+
      CLC
      RTS
:     STA   EB_FINDLEN
      LDX   #0
@copy:
      CPX   EB_FINDLEN
      BEQ   @done
      LDA   EB_PROMPTBUF,X
      STA   EB_FINDBUF,X
      INX
      BRA   @copy
@done:
      SEC
      RTS

editbuf_find_stored_wrapped_from_cursor:
      LDA   EDITBUF_CURL
      STA   EB_SCRATCHL
      LDA   EDITBUF_CURH
      STA   EB_SCRATCHH
      JSR   editbuf_find_stored_from_scratch
      BCS   @done
      STZ   EB_SCRATCHL
      STZ   EB_SCRATCHH
      JSR   editbuf_find_stored_from_scratch
@done:
      RTS

editbuf_find_stored_text:
      LDA   EDITBUF_CURL
      STA   EB_SCRATCHL
      LDA   EDITBUF_CURH
      STA   EB_SCRATCHH
      JMP   editbuf_find_stored_from_scratch

editbuf_find_stored_from_scratch:
      LDA   EB_FINDLEN
      BNE   :+
      CLC
      RTS
:
@outer:
      CLC
      LDA   EB_SCRATCHL
      ADC   EB_FINDLEN
      STA   EB_T0
      LDA   EB_SCRATCHH
      ADC   #0
      STA   EB_T1
      LDA   EDITBUF_LENL
      CMP   EB_T0
      LDA   EDITBUF_LENH
      SBC   EB_T1
      BCC   @not_found
      JSR   editbuf_src_from_scratch
      LDY   #0
@compare:
      CPY   EB_FINDLEN
      BEQ   @found
      LDA   (EB_SRCL),Y
      CMP   EB_FINDBUF,Y
      BNE   @next
      INY
      BRA   @compare
@next:
      INC   EB_SCRATCHL
      BNE   @outer
      INC   EB_SCRATCHH
      BRA   @outer
@found:
      LDA   EB_SCRATCHL
      STA   EDITBUF_CURL
      LDA   EB_SCRATCHH
      STA   EDITBUF_CURH
      SEC
      RTS
@not_found:
      CLC
      RTS

editbuf_replace_match:
      JSR   editbuf_capture_undo
      JSR   editbuf_replace_match_raw
      BCC   @done
      JSR   editbuf_mark_dirty
      JMP   editbuf_after_change
@done:
      RTS

editbuf_replace_match_raw:
      LDA   EB_FINDLEN
      BNE   :+
      CLC
      RTS
:     STZ   EDITBUF_SELACT
      LDA   EB_PROMPTLEN
      CMP   EB_FINDLEN
      BCC   @size_ok
      BEQ   @size_ok
      SEC
      SBC   EB_FINDLEN
      STA   EB_T0                 ; growth = replacement - find
      SEC
      LDA   EDITBUF_CAPL
      SBC   EDITBUF_LENL
      STA   EB_T1                 ; available low (test buffers < 64 KB)
      LDA   EDITBUF_CAPH
      SBC   EDITBUF_LENH
      BCC   @done
      BNE   @size_ok
      LDA   EB_T1
      CMP   EB_T0
      BCC   @done
@size_ok:
      LDA   EDITBUF_CURL
      STA   EB_SCRATCHL
      LDA   EDITBUF_CURH
      STA   EB_SCRATCHH
      LDA   EB_FINDLEN
      STA   EDITBUF_CNTL
      STZ   EDITBUF_CNTH
      JSR   editbuf_close_gap
      LDA   EB_PROMPTLEN
      BEQ   @mark
      STA   EDITBUF_CNTL
      STZ   EDITBUF_CNTH
      JSR   editbuf_make_gap
      LDX   EDITBUF_CURL
      LDA   EDITBUF_CURH
      JSR   editbuf_ptr_from_off
      LDY   #0
@copy:
      CPY   EB_PROMPTLEN
      BEQ   @advance
      LDA   EB_PROMPTBUF,Y
      STA   (EB_PL),Y
      INY
      BRA   @copy
@advance:
      CLC
      LDA   EDITBUF_CURL
      ADC   EB_PROMPTLEN
      STA   EDITBUF_CURL
      LDA   EDITBUF_CURH
      ADC   #0
      STA   EDITBUF_CURH
@mark:
      SEC
      RTS
@done:
      CLC
      RTS

; Parse EB_PROMPTBUF as a 1-based line number into EB_SCRATCH.
; Negative values clamp to the start of file. Empty/invalid input returns carry clear.
editbuf_parse_goto_line:
      LDA   EB_PROMPTLEN
      BNE   :+
      CLC
      RTS
:     STZ   EB_SCRATCHL
      STZ   EB_SCRATCHH
      LDX   #0
      LDA   EB_PROMPTBUF
      CMP   #'-'
      BNE   @loop
      SEC
      RTS
@loop:
      CPX   EB_PROMPTLEN
      BEQ   @done
      LDA   EB_PROMPTBUF,X
      CMP   #'0'
      BCC   @invalid
      CMP   #'9' + 1
      BCS   @invalid
      SEC
      SBC   #'0'
      STA   EB_STATUS_DIGIT
      ; EB_SCRATCH = EB_SCRATCH * 10 + digit, full 16-bit, saturating at $FFFF.
      LDA   EB_SCRATCHL          ; EB_T0:EB_T1 = SCRATCH (the *1 term)
      STA   EB_T0
      LDA   EB_SCRATCHH
      STA   EB_T1
      ASL   EB_SCRATCHL          ; SCRATCH *= 2
      ROL   EB_SCRATCHH
      BCS   @saturate
      ASL   EB_SCRATCHL          ; SCRATCH *= 4
      ROL   EB_SCRATCHH
      BCS   @saturate
      CLC                        ; SCRATCH = *4 + *1 = *5
      LDA   EB_SCRATCHL
      ADC   EB_T0
      STA   EB_SCRATCHL
      LDA   EB_SCRATCHH
      ADC   EB_T1
      STA   EB_SCRATCHH
      BCS   @saturate
      ASL   EB_SCRATCHL          ; SCRATCH = *5 * 2 = *10
      ROL   EB_SCRATCHH
      BCS   @saturate
      CLC                        ; SCRATCH += digit
      LDA   EB_SCRATCHL
      ADC   EB_STATUS_DIGIT
      STA   EB_SCRATCHL
      LDA   EB_SCRATCHH
      ADC   #0
      STA   EB_SCRATCHH
      BCS   @saturate
      BRA   @next
@saturate:
      LDA   #$FF
      STA   EB_SCRATCHL
      STA   EB_SCRATCHH
@next:
      INX
      BRA   @loop
@done:
      LDA   EB_SCRATCHL          ; 1-based input -> 0-based index (16-bit decrement)
      ORA   EB_SCRATCHH
      BEQ   @valid               ; 0 stays 0
      LDA   EB_SCRATCHL
      BNE   @no_borrow
      DEC   EB_SCRATCHH
@no_borrow:
      DEC   EB_SCRATCHL
@valid:
      SEC
      RTS
@invalid:
      CLC
      RTS

; =====================================================================
; Save / quit
; =====================================================================
editbuf_do_host_command:
      JSR   editbuf_call_command
editbuf_after_host_command:
      JSR   editbuf_clamp_cursor_len
      JSR   editbuf_build_title
      JSR   editbuf_compute_linecol
      JSR   editbuf_adjust_scroll
      JSR   editbuf_recount_lines
      JSR   editbuf_redraw_all
      JSR   editbuf_apply_status
      JSR   editui_draw_status
      JMP   editbuf_place_cursor

editbuf_clamp_cursor_len:
      LDA   EDITBUF_LENL
      CMP   EDITBUF_CURL
      LDA   EDITBUF_LENH
      SBC   EDITBUF_CURH
      BCS   @done
      LDA   EDITBUF_LENL
      STA   EDITBUF_CURL
      LDA   EDITBUF_LENH
      STA   EDITBUF_CURH
@done:
      RTS

editbuf_do_save:
      JSR   editbuf_call_save        ; A = status
      STA   EB_T0
      CMP   #EDITBUF_SAVE_OK
      BNE   :+
      JSR   editbuf_set_default_saved_status
      STZ   EDITUI_DIRTY
      JSR   editbuf_call_changed
      BRA   @after_status
:     JSR   editbuf_set_default_save_error_status
@after_status:
      LDA   EB_T0
      CMP   #EDITBUF_SAVE_OK
      BNE   @notok
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
      JSR   editbuf_call_changed
      LDA   #EDITBUF_EXIT_SAVED
      STA   EDITBUF_RESULT
      SEC
      RTS
@save_failed:
      ; stay in editor showing the host's status message
      JSR   editbuf_set_default_save_error_status
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

editbuf_dialog3:
      JSR   nui_dialog_defaults
      LDA   #14
      STA   NUI_DIALOG_LEFT
      LDA   #17
      STA   NUI_DIALOG_TOP
      LDA   #52
      STA   NUI_DIALOG_WIDTH
      LDA   #12
      STA   NUI_DIALOG_HEIGHT
      LDA   #<editbuf_dlg_title
      STA   NUI_TITLEL
      LDA   #>editbuf_dlg_title
      STA   NUI_TITLEH
      LDA   #<editbuf_dlg_msg
      STA   NUI_MSGL
      LDA   #>editbuf_dlg_msg
      STA   NUI_MSGH
      STZ   NUI_FOOTERL
      STZ   NUI_FOOTERH
      JSR   nui_show_dialog
      BNE   @cancel
      STZ   EB_PROMPTLEN
      JSR   editbuf_dialog3_render
@wait:
      JSR   nui_read_key
      CMP   #EDITUI_KEY_ENTER
      BEQ   @select
      CMP   #EDITUI_KEY_ESC
      BEQ   @cancel
      CMP   #NUI_KEY_TAB
      BEQ   @next
      CMP   #NUI_KEY_RIGHT
      BEQ   @next
      CMP   #NUI_KEY_LEFT
      BEQ   @prev
      ORA   #$20
      CMP   #'s'
      BEQ   @save
      CMP   #'e'
      BEQ   @exit
      CMP   #'d'
      BEQ   @exit
      CMP   #'c'
      BNE   @wait
@cancel:
      LDA   #2
      RTS
@next:
      INC   EB_PROMPTLEN
      LDA   EB_PROMPTLEN
      CMP   #3
      BCC   :+
      STZ   EB_PROMPTLEN
:     JSR   editbuf_dialog3_render
      BRA   @wait
@prev:
      LDA   EB_PROMPTLEN
      BNE   :+
      LDA   #3
:     DEC   A
      STA   EB_PROMPTLEN
      JSR   editbuf_dialog3_render
      BRA   @wait
@select:
      LDA   EB_PROMPTLEN
      BEQ   @save
      CMP   #1
      BEQ   @exit
      BRA   @cancel
@save:
      LDA   #1
      RTS
@exit:
      LDA   #0
      RTS

editbuf_dialog3_render:
      JSR   nui_set_screen_text
      LDA   #NUI_TEXT_SHADOW
      STA   VTEXT_COLOR
      LDA   NUI_DIALOG_TOP
      CLC
      ADC   #8
      STA   VTEXT_CURY
      LDA   NUI_DIALOG_LEFT
      CLC
      ADC   #7
      STA   VTEXT_CURX
      LDA   #<editbuf_dlg_save_shadow
      LDY   #>editbuf_dlg_save_shadow
      LDX   #6
      JSR   vtext_put_run
      BEQ   :+
      JMP   @done
:     LDA   NUI_DIALOG_LEFT
      CLC
      ADC   #21
      STA   VTEXT_CURX
      LDA   #<editbuf_dlg_discard_shadow
      LDY   #>editbuf_dlg_discard_shadow
      LDX   #9
      JSR   vtext_put_run
      BEQ   :+
      JMP   @done
:     LDA   NUI_DIALOG_LEFT
      CLC
      ADC   #38
      STA   VTEXT_CURX
      LDA   #<editbuf_dlg_cancel_shadow
      LDY   #>editbuf_dlg_cancel_shadow
      LDX   #8
      JSR   vtext_put_run
      BEQ   :+
      JMP   @done
:     LDA   #NUI_TEXT_BUTTON
      STA   VTEXT_COLOR
      LDA   NUI_DIALOG_TOP
      CLC
      ADC   #7
      STA   VTEXT_CURY
      LDA   NUI_DIALOG_LEFT
      CLC
      ADC   #6
      STA   VTEXT_CURX
      LDA   #0
      JSR   editbuf_dialog3_attr
      LDA   #<editbuf_dlg_save
      LDY   #>editbuf_dlg_save
      LDX   #6
      JSR   vtext_put_run
      BEQ   :+
      JMP   @done
:     STZ   VTEXT_ATTR
      LDA   #NUI_TEXT_BUTTON
      STA   VTEXT_COLOR
      LDA   NUI_DIALOG_LEFT
      CLC
      ADC   #20
      STA   VTEXT_CURX
      LDA   #1
      JSR   editbuf_dialog3_attr
      LDA   #<editbuf_dlg_discard
      LDY   #>editbuf_dlg_discard
      LDX   #9
      JSR   vtext_put_run
      BEQ   :+
      JMP   @done
:     STZ   VTEXT_ATTR
      LDA   #NUI_TEXT_BUTTON
      STA   VTEXT_COLOR
      LDA   NUI_DIALOG_LEFT
      CLC
      ADC   #37
      STA   VTEXT_CURX
      LDA   #2
      JSR   editbuf_dialog3_attr
      LDA   #<editbuf_dlg_cancel
      LDY   #>editbuf_dlg_cancel
      LDX   #8
      JSR   vtext_put_run
@done:
      STZ   VTEXT_ATTR
      RTS

editbuf_dialog3_attr:
      CMP   EB_PROMPTLEN
      BNE   :+
      LDA   #VTEXT_ATTR_REVERSE
      STA   VTEXT_ATTR
      RTS
:     STZ   VTEXT_ATTR
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
:     LDA   EDITBUF_COMMAND_VECL
      ORA   EDITBUF_COMMAND_VECH
      BNE   :+
      LDA   #<editbuf_default_command
      STA   EDITBUF_COMMAND_VECL
      LDA   #>editbuf_default_command
      STA   EDITBUF_COMMAND_VECH
:     LDA   EDITBUF_CHANGED_VECL
      ORA   EDITBUF_CHANGED_VECH
      BNE   :+
      LDA   #<editbuf_default_changed
      STA   EDITBUF_CHANGED_VECL
      LDA   #>editbuf_default_changed
      STA   EDITBUF_CHANGED_VECH
:     RTS

editbuf_call_menu:
      JMP   (EDITBUF_MENU_VECL)

editbuf_call_command:
      JMP   (EDITBUF_COMMAND_VECL)

editbuf_call_save:
      JMP   (EDITBUF_SAVE_VECL)

editbuf_call_indent:
      JMP   (EDITBUF_INDENT_VECL)

editbuf_call_hilite:
      JMP   (EDITBUF_HILITE_VECL)

editbuf_call_changed:
      JMP   (EDITBUF_CHANGED_VECL)

; default hook bodies (host may point vectors here)
      .export editbuf_default_save
      .export editbuf_default_indent
      .export editbuf_default_hilite
      .export editbuf_default_menu
      .export editbuf_default_command
      .export editbuf_default_changed
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
editbuf_default_command:
      RTS
editbuf_default_changed:
      RTS

; =====================================================================
; RODATA
; =====================================================================
      .segment "RODATA"

editbuf_status_x:
      .byte "X:", 0
editbuf_status_y:
      .byte " Y:", 0
editbuf_status_bytes:
      .byte " B:", 0
editbuf_status_lines:
      .byte " L:", 0
editbuf_status_type:
      .byte " T:", 0
editbuf_msg_saved:
      .byte "Saved", 0
editbuf_msg_save_failed:
      .byte "Save failed", 0
editbuf_pow10_lo:
      .byte <10000, <1000, <100, <10, <1
editbuf_pow10_hi:
      .byte >10000, >1000, >100, >10, >1
editbuf_find_msg:
      .byte "Find", 0
editbuf_find_label:
      .byte "Text:   "
editbuf_replace_msg:
      .byte "Replace", 0
editbuf_replace_all_msg:
      .byte "Replace All", 0
editbuf_replace_label:
      .byte "With:   "
editbuf_goto_msg:
      .byte "Ln", 0
editbuf_goto_label:
      .byte "Line:   "
editbuf_dlg_title:
      .byte "Modified", 0
editbuf_dlg_msg:
      .byte "Save changes before closing?", 0
editbuf_dlg_save:
      .byte " Save "
editbuf_dlg_discard:
      .byte " Discard "
editbuf_dlg_cancel:
      .byte " Cancel "
editbuf_dlg_save_shadow:
      .byte "      "
editbuf_dlg_discard_shadow:
      .byte "         "
editbuf_dlg_cancel_shadow:
      .byte "        "

.endif

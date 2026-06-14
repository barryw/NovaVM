; editor.s — EDITOR module (MODULE_ID_EDITOR = $08), the shared full-screen
; source editor. Built once, staged to the XRAM module store, paged into bank 1
; on lib_call. Header "NL"/$08 at $C000 (lib_module_header); RTS-trick dispatch
; on LIB_FN_ID.

      .include "libabi.inc"
      .include "libmod.inc"
      .include "libeditor.inc"
      .include "nova.inc"              ; VGC_*/TEXTWIN_* register equates + ROMSWAP
      .include "vgc.inc"               ; (guarded) VGC command/constant equates
      .include "copper.inc"            ; copper_off .global
      .include "editbuf.inc"           ; EDITBUF_* config + editbuf_run/_reset_state

; Editor display colors. Mirrors the extension's NovaLogo editor styling. The
; shared engine no longer dictates the active palette/colors; the caller's prior
; display is snapshotted and restored exactly. The editor colors are authored for
; the default Nova/C64 palette (0=black, 1=white).
EDITOR_BGCOL      = $00                ; black background
EDITOR_BORDER     = $00                ; black border
EDITOR_FGCOL      = $01                ; white text
SYS_HL_KEYWORD    = $03                ; cyan on black
SYS_HL_NUMBER     = $07                ; yellow on black
SYS_HL_COMMENT    = $0C                ; grey on black

      .segment "CODE"
      lib_module_header MODULE_ID_EDITOR, LIB_ABI_VERSION, EDITOR_FN_COUNT

; ===========================================================================
; NDOC documentation (extracted by tools/nmod_pack.py into editor.nmod).
; Function ids resolve symbolically from runtime/asm/libeditor.inc.
; ===========================================================================
;@module EDITOR
;@version 1.0
;@brief Shared full-screen source editor.
;
;@fn EDITOR_FN_EDIT
;@brief Run the modal EDITUI/EDITBUF text editor on a host RAM text buffer.
;@arg buffer u16 pointer to the editable text buffer (ARG0 low word)
;@arg cursor u16 initial cursor byte offset (ARG0 high word)
;@arg length u16 current text length in bytes (ARG1 low word)
;@arg capacity u16 buffer capacity in bytes (ARG2 low word)
;@arg title u16 pointer to a NUL-terminated title string (ARG3 low word)
;@arg profile u8 optional syntax/edit profile (ARG3 byte2, EDITOR_EDIT_PROFILE_*)
;@ret u8 exit reason (RESULT byte0, EDITBUF_EXIT_*); save flag (RESULT byte1); final length (ARG1)
;@effect Snapshots and restores the caller's full VGC display state; owns the
;@effect whole 80x25 text screen while running. Blocks until the user exits.
;@status LERR_OK

; ---------------------------------------------------------------------------
; dispatch — fn-id router. RTS-trick: push (target-1) hi/lo, RTS jumps to target.
; EDITOR_FN_COUNT is small so fn*2 cannot exceed 255; an 8-bit asl/tax is safe.
; ---------------------------------------------------------------------------
dispatch:
      lda     LIB_FN_ID
      cmp     #EDITOR_FN_COUNT
      bcs     @bad
      asl
      tax
      lda     sys_jtable+1,x
      pha
      lda     sys_jtable,x
      pha
      rts
@bad:
      lda     #LERR_NO_FN
      sta     LIB_STATUS
      rts

sys_jtable:
      .word   sys_edit-1               ; $00 EDITOR_FN_EDIT

; ===========================================================================
; EDITOR_FN_EDIT — port of the extension's ext_edit, reading the canonical lib_call
; mailbox instead of the legacy EXT_ARG zero-page cells.
;   In:  ARG0 lo word = buffer ptr      ARG0 hi word = initial cursor offset
;        ARG1 lo word = current length  ARG2 lo word = capacity
;        ARG3 lo word = NUL title ptr
;   Out: RESULT+0 = editbuf exit reason  RESULT+1 = save flag (nonzero => save)
;        ARG1 lo word = final buffer length   LIB_STATUS = LERR_OK
; ===========================================================================
sys_edit:
      ; --- snapshot the display registers editui_init will clobber ---
      LDA   VGC_MODE
      STA   se_saved_mode
      LDA   VGC_PALETTE
      STA   se_saved_palette
      LDA   VGC_BGCOL
      STA   se_saved_bgcol
      LDA   VGC_BORDER
      STA   se_saved_border
      LDA   VGC_FGCOL
      STA   se_saved_fgcol
      LDA   VGC_CURSX
      STA   se_saved_cursx
      LDA   VGC_CURSY
      STA   se_saved_cursy
      LDA   VGC_CURSEN
      STA   se_saved_cursen
      LDA   VGC_TEXT_TOPROW
      STA   se_saved_text_toprow
      LDA   VGC_TEXT_SCROLL_START
      STA   se_saved_text_scroll_start
      LDA   VGC_TEXT_SCROLL_ROWS
      STA   se_saved_text_scroll_rows

      ; --- the editor owns the whole screen: copper off, full text window ---
      JSR   copper_off
      STZ   TEXTWIN_LEFT
      STZ   TEXTWIN_TOP
      LDA   #80
      STA   TEXTWIN_WIDTH
      LDA   #25
      STA   TEXTWIN_HEIGHT

      ; --- editor colors (the engine no longer sets these) ---
      LDA   #EDITOR_BGCOL
      STA   VGC_BGCOL
      LDA   #EDITOR_BORDER
      STA   VGC_BORDER
      LDA   #EDITOR_FGCOL
      STA   VGC_FGCOL

      ; --- editbuf config from the mailbox (32-bit LE cells at $0303/$0307/...) ---
      LDA   LIB_ARG0+0
      STA   EDITBUF_BUFL
      LDA   LIB_ARG0+1
      STA   EDITBUF_BUFH
      LDA   LIB_ARG2+0
      STA   EDITBUF_CAPL
      LDA   LIB_ARG2+1
      STA   EDITBUF_CAPH
      LDA   LIB_ARG1+0
      STA   EDITBUF_LENL
      LDA   LIB_ARG1+1
      STA   EDITBUF_LENH
      LDA   LIB_ARG3+0
      STA   EDITBUF_TITLEL
      LDA   LIB_ARG3+1
      STA   EDITBUF_TITLEH
      LDA   #<sys_edit_type_text
      STA   EDITBUF_TYPEL
      LDA   #>sys_edit_type_text
      STA   EDITBUF_TYPEH
      STZ   EDITBUF_STATUSL
      STZ   EDITBUF_STATUSH

      ; --- install hooks explicitly (BSS is not zeroed between sessions) ---
      LDA   #<sys_edit_save_hook
      STA   EDITBUF_SAVE_VECL
      LDA   #>sys_edit_save_hook
      STA   EDITBUF_SAVE_VECH
      STZ   EDITBUF_INDENT_VECL        ; 0 => editbuf installs its no-op default
      STZ   EDITBUF_INDENT_VECH
      LDA   LIB_ARG3+2
      CMP   #EDITOR_EDIT_PROFILE_FORTH
      BEQ   @forth_hilite
      CMP   #EDITOR_EDIT_PROFILE_LOGO
      BEQ   @logo_hilite
      STZ   EDITBUF_HILITE_VECL
      STZ   EDITBUF_HILITE_VECH
      BRA   @hilite_done
@forth_hilite:
      LDA   #<sys_edit_type_forth
      STA   EDITBUF_TYPEL
      LDA   #>sys_edit_type_forth
      STA   EDITBUF_TYPEH
      LDA   #<sys_edit_indent_forth
      STA   EDITBUF_INDENT_VECL
      LDA   #>sys_edit_indent_forth
      STA   EDITBUF_INDENT_VECH
      LDA   #<sys_edit_hilite_forth
      STA   EDITBUF_HILITE_VECL
      LDA   #>sys_edit_hilite_forth
      STA   EDITBUF_HILITE_VECH
      BRA   @hilite_done
@logo_hilite:
      LDA   #<sys_edit_type_logo
      STA   EDITBUF_TYPEL
      LDA   #>sys_edit_type_logo
      STA   EDITBUF_TYPEH
      LDA   #<sys_edit_indent_logo
      STA   EDITBUF_INDENT_VECL
      LDA   #>sys_edit_indent_logo
      STA   EDITBUF_INDENT_VECH
      LDA   #<sys_edit_hilite_logo
      STA   EDITBUF_HILITE_VECL
      LDA   #>sys_edit_hilite_logo
      STA   EDITBUF_HILITE_VECH
@hilite_done:
      STZ   EDITBUF_MENU_VECL          ; keep EDITUI's default menus for now
      STZ   EDITBUF_MENU_VECH

      ; --- run the modal editor ---
      STZ   se_saved_flag
      JSR   editbuf_reset_state
      ; Cursor offset rides in ARG0's high word (buffer ptr uses only the low word).
      LDA   LIB_ARG0+2
      STA   EDITBUF_CURL
      LDA   LIB_ARG0+3
      STA   EDITBUF_CURH
      JSR   editbuf_run                ; A = exit reason

      ; --- publish results to the mailbox ---
      STA   LIB_RESULT+0               ; editbuf exit reason
      LDA   se_saved_flag
      STA   LIB_RESULT+1               ; nonzero => save requested
      STZ   LIB_RESULT+2
      STZ   LIB_RESULT+3
      LDA   EDITBUF_LENL
      STA   LIB_ARG1+0                 ; final length back to the host
      LDA   EDITBUF_LENH
      STA   LIB_ARG1+1

      ; --- restore the display exactly (never clear graphics) ---
      ; Restore the host's legacy palette register + colors FIRST: the form-feed clear below fills
      ; color RAM with the *current* text color, so the host's colors must be back
      ; in place before we clear, or the editor's panel color leaks into the
      ; restored text area.
      LDA   se_saved_palette
      STA   VGC_PALETTE
      LDA   se_saved_bgcol
      STA   VGC_BGCOL
      LDA   se_saved_border
      STA   VGC_BORDER
      LDA   se_saved_fgcol
      STA   VGC_FGCOL

      ; Plain full-text restore: clear the editor chrome and restore the saved
      ; display registers. The host re-applies any split/turtle window itself after
      ; the call returns (proc_edit_restore_split).
      STZ   TEXTWIN_LEFT
      STZ   TEXTWIN_TOP
      LDA   #80
      STA   TEXTWIN_WIDTH
      LDA   #25
      STA   TEXTWIN_HEIGHT
      LDA   se_saved_text_scroll_start
      STA   VGC_TEXT_SCROLL_START
      LDA   se_saved_text_scroll_rows
      STA   VGC_TEXT_SCROLL_ROWS
      LDA   se_saved_text_toprow
      STA   VGC_TEXT_TOPROW
      LDA   #$0C                       ; form feed: clear text plane only
      STA   VGC_CHAROUT
      LDA   se_saved_mode
      STA   VGC_MODE
      LDA   se_saved_cursx
      STA   VGC_CURSX
      LDA   se_saved_cursy
      STA   VGC_CURSY
      LDA   se_saved_cursen
      STA   VGC_CURSEN

      LDA   #LERR_OK
      STA   LIB_STATUS
      RTS

; sys_edit_save_hook — SAVE hook while the editor runs. Language-neutral: it just
; records that the user asked to save and reports success. The host builds the
; document record from the final buffer when the editor exits.
sys_edit_save_hook:
      LDA   #1
      STA   se_saved_flag
      LDA   #EDITBUF_SAVE_OK
      RTS

; sys_edit_hilite_forth — compact Forth profile highlighter: mark comments,
; numbers, definition punctuation, and word tokens without a ROM-heavy keyword
; table.
sys_edit_hilite_forth:
      LDY   #0
sys_hl_forth_loop:
      CPY   EDITBUF_HL_LEN
      BCC   :+
      RTS
:
      LDA   (EDITBUF_HL_PTR),Y
      CMP   #' '
      BEQ   @advance
      CMP   #$09
      BEQ   @advance
      CMP   #$5C                       ; \ comment to end of line
      BEQ   @comment_eol
      CMP   #$3A                       ; :
      BEQ   @keyword_char
      CMP   #$3B                       ; ;
      BEQ   @keyword_char
      JSR   sys_hl_is_digit
      BCS   @number
      JSR   sys_hl_is_alpha
      BCC   @skip_token
      LDA   #SYS_HL_KEYWORD
      JSR   sys_hl_color_token
      BRA   sys_hl_forth_loop
@skip_token:
      JSR   sys_hl_skip_token
      BRA   sys_hl_forth_loop
@advance:
      INY
      BRA   sys_hl_forth_loop
@keyword_char:
      LDA   #SYS_HL_KEYWORD
      STA   EDITBUF_HL_COLORS,Y
      INY
      BRA   sys_hl_forth_loop
@number:
      LDA   #SYS_HL_NUMBER
      JSR   sys_hl_color_token
      BRA   sys_hl_forth_loop
@comment_eol:
      LDA   #SYS_HL_COMMENT
      JSR   sys_hl_color_to_eol
      BRA   @done
@done:
      RTS

; sys_edit_hilite_logo — initial Logo profile highlighter. Logo source is more
; command-like than Forth, so color word tokens as commands/procedure names,
; numbers separately, and semicolon comments to end-of-line.
sys_edit_hilite_logo:
      LDY   #0
sys_hl_logo_loop:
      CPY   EDITBUF_HL_LEN
      BCC   :+
      RTS
:
      LDA   (EDITBUF_HL_PTR),Y
      CMP   #' '
      BEQ   @advance
      CMP   #$09
      BEQ   @advance
      CMP   #$3B                       ; ; comment to end of line
      BEQ   @comment_eol
      JSR   sys_hl_is_digit
      BCS   @number
      JSR   sys_hl_is_alpha
      BCS   @word
      JSR   sys_hl_skip_token
      BRA   sys_hl_logo_loop
@advance:
      INY
      BRA   sys_hl_logo_loop
@word:
      LDA   #SYS_HL_KEYWORD
      JSR   sys_hl_color_token
      BRA   sys_hl_logo_loop
@number:
      LDA   #SYS_HL_NUMBER
      JSR   sys_hl_color_token
      BRA   sys_hl_logo_loop
@comment_eol:
      LDA   #SYS_HL_COMMENT
      JSR   sys_hl_color_to_eol
@done:
      RTS

sys_edit_indent_forth:
      JSR   sys_indent_previous_line_summary
      LDA   se_first_char
      CMP   #$3A                       ; : NAME  -> indent body
      BNE   @maybe_dedent
      JSR   sys_indent_add2
      BRA   @done
@maybe_dedent:
      LDA   se_last_char
      CMP   #$3B                       ; ; -> next line returns one level
      BNE   @done
      JSR   sys_indent_sub2
@done:
      LDA   se_indent
      RTS

sys_edit_indent_logo:
      JSR   sys_indent_previous_line_summary
      LDA   se_bracket_delta           ; unmatched [ opens a list/block body
      BEQ   @done
      JSR   sys_indent_add2
@done:
      LDA   se_indent
      RTS

sys_indent_previous_line_summary:
      STZ   se_indent
      STZ   se_first_char
      STZ   se_last_char
      STZ   se_bracket_delta
      STZ   se_seen_text
      JSR   sys_indent_find_previous_line
      LDA   se_line_startL
      STA   se_scanL
      LDA   se_line_startH
      STA   se_scanH
@loop:
      JSR   sys_indent_scan_at_end
      BCS   @done
      JSR   sys_indent_load_scan_char
      STA   se_hl_ch
      CMP   #'['
      BNE   :+
      INC   se_bracket_delta
      BRA   @classify
:     CMP   #']'
      BNE   @classify
      LDA   se_bracket_delta
      BEQ   @classify
      DEC   se_bracket_delta
@classify:
      LDA   se_hl_ch
      CMP   #' '
      BEQ   @space
      CMP   #$09
      BEQ   @tab
      LDA   se_seen_text
      BNE   @record_last
      LDA   se_hl_ch
      STA   se_first_char
      LDA   #1
      STA   se_seen_text
@record_last:
      LDA   se_hl_ch
      STA   se_last_char
      BRA   @advance
@space:
      LDA   se_seen_text
      BNE   @advance
      LDA   se_indent
      CMP   #16
      BCS   @advance
      INC   se_indent
      BRA   @advance
@tab:
      LDA   se_seen_text
      BNE   @advance
      JSR   sys_indent_add2
@advance:
      JSR   sys_indent_inc_scan
      BRA   @loop
@done:
      RTS

sys_indent_find_previous_line:
      STZ   se_line_startL
      STZ   se_line_startH
      STZ   se_line_endL
      STZ   se_line_endH
      LDA   EDITBUF_CURL
      ORA   EDITBUF_CURH
      BEQ   @done
      LDA   EDITBUF_CURL
      STA   se_line_endL
      LDA   EDITBUF_CURH
      STA   se_line_endH
      JSR   sys_indent_dec_line_end    ; skip the newline just inserted
      LDA   se_line_endL
      ORA   se_line_endH
      BEQ   @done
      LDA   se_line_endL
      STA   se_scanL
      LDA   se_line_endH
      STA   se_scanH
      JSR   sys_indent_dec_scan
@back:
      JSR   sys_indent_load_scan_char
      CMP   #$0A
      BEQ   @after_newline
      LDA   se_scanL
      ORA   se_scanH
      BEQ   @done
      JSR   sys_indent_dec_scan
      BRA   @back
@after_newline:
      JSR   sys_indent_inc_scan
      LDA   se_scanL
      STA   se_line_startL
      LDA   se_scanH
      STA   se_line_startH
@done:
      RTS

sys_indent_scan_at_end:
      LDA   se_scanL
      CMP   se_line_endL
      LDA   se_scanH
      SBC   se_line_endH
      RTS

sys_indent_load_scan_char:
      LDA   EDITBUF_BUFL
      CLC
      ADC   se_scanL
      STA   z:EB_SRCL
      LDA   EDITBUF_BUFH
      ADC   se_scanH
      STA   z:EB_SRCH
      LDY   #0
      LDA   (EB_SRCL),Y
      RTS

sys_indent_add2:
      LDA   se_indent
      CMP   #15
      BCS   @max
      CLC
      ADC   #2
      CMP   #17
      BCC   @store
@max:
      LDA   #16
@store:
      STA   se_indent
      RTS

sys_indent_sub2:
      LDA   se_indent
      CMP   #2
      BCC   @zero
      SEC
      SBC   #2
      STA   se_indent
      RTS
@zero:
      STZ   se_indent
      RTS

sys_indent_dec_line_end:
      LDA   se_line_endL
      BNE   :+
      DEC   se_line_endH
:     DEC   se_line_endL
      RTS

sys_indent_dec_scan:
      LDA   se_scanL
      BNE   :+
      DEC   se_scanH
:     DEC   se_scanL
      RTS

sys_indent_inc_scan:
      INC   se_scanL
      BNE   :+
      INC   se_scanH
:     RTS

sys_hl_color_to_eol:
      STA   se_hl_color
@loop:
      CPY   EDITBUF_HL_LEN
      BCS   @done
      LDA   se_hl_color
      STA   EDITBUF_HL_COLORS,Y
      INY
      BRA   @loop
@done:
      RTS

sys_hl_color_token:
      STA   se_hl_color
@loop:
      CPY   EDITBUF_HL_LEN
      BCS   @done
      LDA   (EDITBUF_HL_PTR),Y
      JSR   sys_hl_is_delim
      BCS   @done
      LDA   se_hl_color
      STA   EDITBUF_HL_COLORS,Y
      INY
      BRA   @loop
@done:
      RTS

sys_hl_skip_token:
@loop:
      CPY   EDITBUF_HL_LEN
      BCS   @done
      LDA   (EDITBUF_HL_PTR),Y
      JSR   sys_hl_is_delim
      BCS   @skip_delim
      INY
      BRA   @loop
@skip_delim:
      INY
@done:
      RTS

sys_hl_is_delim:
      CMP   #' '
      BEQ   @yes
      CMP   #$09
      BEQ   @yes
      CMP   #'['
      BEQ   @yes
      CMP   #']'
      BEQ   @yes
      CLC
      RTS
@yes:
      SEC
      RTS

sys_hl_is_digit:
      CMP   #'0'
      BCC   @no
      CMP   #'9' + 1
      BCC   @yes
@no:
      CLC
      RTS
@yes:
      SEC
      RTS

sys_hl_is_alpha:
      CMP   #'A'
      BCC   @lower
      CMP   #'Z' + 1
      BCC   @yes
@lower:
      CMP   #'a'
      BCC   @no
      CMP   #'z' + 1
      BCC   @yes
@no:
      CLC
      RTS
@yes:
      SEC
      RTS

      .segment "BSS"
se_saved_mode:    .res 1
se_saved_palette: .res 1
se_saved_bgcol:   .res 1
se_saved_border:  .res 1
se_saved_fgcol:   .res 1
se_saved_cursx:   .res 1
se_saved_cursy:   .res 1
se_saved_cursen:  .res 1
se_saved_text_toprow: .res 1
se_saved_text_scroll_start: .res 1
se_saved_text_scroll_rows: .res 1
se_saved_flag:    .res 1               ; nonzero once the SAVE hook fires
se_hl_color:      .res 1               ; current syntax color while walking token text
se_hl_start:      .res 1               ; token start column in the current highlight line
se_hl_end:        .res 1               ; token end column in the current highlight line
se_hl_len:        .res 1               ; token length in the current highlight line
se_hl_tblL:       .res 1               ; syntax token table walker
se_hl_tblH:       .res 1
se_hl_entry_len:  .res 1
se_hl_ch:         .res 1
se_indent:        .res 1               ; computed auto-indent count
se_first_char:    .res 1               ; first non-space char on previous line
se_last_char:     .res 1               ; last non-space char on previous line
se_bracket_delta: .res 1               ; previous line '[' minus ']' count, clamped at 0
se_seen_text:     .res 1               ; nonzero after previous-line non-space text
se_scanL:         .res 1               ; previous-line scanner offset
se_scanH:         .res 1
se_line_startL:   .res 1
se_line_startH:   .res 1
se_line_endL:     .res 1               ; exclusive end offset of previous line text
se_line_endH:     .res 1
      .segment "CODE"

; ===========================================================================
; Shared driver + editor bodies (all include-guarded).
; copper.s -> copper_off. editui.s pulls vtext.s -> blitter.s. editbuf.s.
; ===========================================================================
      .include "copper.s"
      .include "editui.s"
      .include "editbuf.s"
      .segment "RODATA"
sys_edit_type_text:
      .byte "Text", 0
sys_edit_type_forth:
      .byte "Forth", 0
sys_edit_type_logo:
      .byte "Logo", 0

      .segment "CODE"

      .segment "VECTORS"
      ; Module runs under SEI (the loader masks IRQ across the bank swap); hardware
      ; vectors never fire from bank 1. Fill with a safe entry-point stub.
      .word   $C000, $C000, $C000      ; NMI, RESET, IRQ

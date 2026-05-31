; editbuf.inc — shared text-editing engine for Nova native tools.
;
; editbuf layers a real editable text buffer (cursor movement, insert/delete,
; line navigation, selection, cut/copy/paste, scrolling) and a three-choice
; modal dialog on top of the EDITUI chrome shell. It is intentionally
; LANGUAGE-NEUTRAL: it knows nothing about Logo, BASIC, or any document model.
;
; Anything language-specific is supplied by the host through HOOK VECTORS:
;   - SAVE hook   : invoked on Ctrl-S (and the dialog "Save First" choice).
;                   The host validates/saves/installs and returns a status.
;   - INDENT hook : invoked after Enter. The host returns how many leading
;                   spaces the new line should start with (auto-indent).
;   - HILITE hook : invoked per visible line during render. The host fills a
;                   per-character color buffer (syntax highlighting).
; Every hook defaults to a no-op, so editbuf works as a plain editor with no
; host wiring at all.
;
; Calling convention: the host populates the EDITBUF_* config bytes below, then
; calls editbuf_run. editbuf_run blocks until the user exits and returns the
; exit reason in A (also EDITBUF_RESULT). The buffer text and EDITBUF_LEN are
; left in place so the host can inspect / re-open the draft.

.ifndef EDITBUF_INC_INCLUDED
EDITBUF_INC_INCLUDED = 1

.include "editui.inc"

; ---------------------------------------------------------------------
; Editor body geometry (text area inside the framed panel).
; Screen is NOVA_SCREEN_COLS x NOVA_SCREEN_ROWS. Layout:
;   row 0       : menu bar
;   rows 2..5   : title band (title + dirty marker + help)
;   rows 6..N-2 : framed editor body
;   row N-1     : status bar
; ---------------------------------------------------------------------
EDITBUF_BODY_BOXX   = 1
EDITBUF_BODY_BOXY   = 6
EDITBUF_BODY_BOXW   = EDITUI_SCREEN_COLS - 2          ; 78
EDITBUF_BODY_BOXH   = EDITUI_SCREEN_ROWS - EDITBUF_BODY_BOXY - 1  ; up to status row
; Interior (one cell of border on each side):
EDITBUF_VIEW_LEFT   = EDITBUF_BODY_BOXX + 1           ; first text column
EDITBUF_VIEW_TOP    = EDITBUF_BODY_BOXY + 1           ; first text row
EDITBUF_VIEW_COLS   = EDITBUF_BODY_BOXW - 2           ; 76 visible columns
EDITBUF_VIEW_ROWS   = EDITBUF_BODY_BOXH - 2           ; visible text rows

; Clipboard capacity (bytes). Lives in editbuf BSS.
EDITBUF_CLIP_CAP    = 256

; ---------------------------------------------------------------------
; Exit reason returned in A by editbuf_run (also stored in EDITBUF_RESULT).
; ---------------------------------------------------------------------
EDITBUF_EXIT_QUIT    = $00      ; Ctrl-Q while clean
EDITBUF_EXIT_SAVED   = $01      ; left via dialog "Save First" after a clean save
EDITBUF_EXIT_DISCARD = $02      ; left via dialog "Exit Anyway" (draft preserved)

; ---------------------------------------------------------------------
; SAVE hook contract.
;   Called with no arguments (current text is in EDITBUF_BUF / EDITBUF_LEN).
;   Returns status in A:
; ---------------------------------------------------------------------
EDITBUF_SAVE_OK      = $00      ; saved + installed: clears dirty
EDITBUF_SAVE_DRAFT   = $01      ; draft kept but not usable: stays "dirty"-ish
EDITBUF_SAVE_ERROR   = $02      ; rejected: stays dirty
; The host may point EDITBUF_STATUS at a message string before returning; the
; editor redraws the status bar with it.

; ---------------------------------------------------------------------
; INDENT hook contract.
;   Called after a newline is inserted. The cursor sits at the start of the
;   freshly created line. Returns in A the number of leading spaces to insert
;   (0 = no auto-indent). editbuf inserts that many spaces.
; ---------------------------------------------------------------------

; ---------------------------------------------------------------------
; HILITE hook contract.
;   Called once per visible line before it is drawn.
;     EDITBUF_HL_PTR (zp) -> first byte of the line text (in EDITBUF_BUF)
;     EDITBUF_HL_LEN       = line length in bytes (0..255, excludes the \n)
;     EDITBUF_HL_COLORS    = 80-byte buffer the hook fills with one VGC color
;                            byte per character. Pre-filled with the default
;                            text color; the hook overwrites as needed.
;   The hook returns nothing. If unset, the default color is used for all.
; ---------------------------------------------------------------------

; Default editor colors (bg = high nibble, fg = low nibble; palette mode 0).
EDITBUF_COLOR_TEXT   = $01      ; white on black body
EDITBUF_COLOR_SEL    = $1F      ; selection: white bg, grey text

; --- Config (host sets before editbuf_run) -------------------------------
.global EDITBUF_BUFL          ; pointer to text buffer
.global EDITBUF_BUFH
.global EDITBUF_CAPL          ; capacity in bytes (16-bit)
.global EDITBUF_CAPH
.global EDITBUF_LENL          ; current length in bytes (16-bit)
.global EDITBUF_LENH
.global EDITBUF_TITLEL        ; NUL-terminated title string
.global EDITBUF_TITLEH
.global EDITBUF_STATUSL       ; NUL-terminated status message (may be 0)
.global EDITBUF_STATUSH
.global EDITBUF_SAVE_VECL     ; SAVE hook vector
.global EDITBUF_SAVE_VECH
.global EDITBUF_INDENT_VECL   ; INDENT hook vector
.global EDITBUF_INDENT_VECH
.global EDITBUF_HILITE_VECL   ; HILITE hook vector
.global EDITBUF_HILITE_VECH
.global EDITBUF_MENU_VECL     ; MENU-config hook vector
.global EDITBUF_MENU_VECH

; --- Result / observable state ------------------------------------------
.global EDITBUF_RESULT
.global EDITBUF_CURL          ; cursor byte offset (16-bit)
.global EDITBUF_CURH

; --- HILITE hook scratch (zp pointer + buffers) -------------------------
.global EDITBUF_HL_PTR        ; zp, 2 bytes
.global EDITBUF_HL_LEN
.global EDITBUF_HL_COLORS     ; 80-byte color buffer

; --- Entry points -------------------------------------------------------
.global editbuf_run           ; run the modal editor
.global editbuf_reset_state   ; reset cursor/scroll/selection/dirty for a new session
.global editbuf_dialog3       ; three-choice restore-under modal (A=result 0/1/2)

; Dialog choice result codes (returned by editbuf_dialog3 in A).
EDITBUF_DLG_EXIT   = $00       ; first button  (Exit Anyway)
EDITBUF_DLG_SAVE   = $01       ; second button (Save First)
EDITBUF_DLG_CANCEL = $02       ; third button  (Cancel)

.endif

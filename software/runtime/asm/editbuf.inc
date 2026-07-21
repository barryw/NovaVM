; =====================================================================
;  Nova NDK — editbuf.inc
;
;  Language-neutral text-editing engine layered on the EDITUI chrome shell:
;  cursor movement, insert/delete, selection, cut/copy/paste, scrolling, and
;  modal prompts. Host-specific behavior comes in through hook vectors.
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

; Anything language-specific is supplied by the host through HOOK VECTORS:
;   - SAVE hook   : invoked on Ctrl-S (and the dialog "Save First" choice).
;                   The host validates/saves/installs and returns a status.
;   - INDENT hook : invoked after Enter. The host returns how many leading
;                   spaces the new line should start with (auto-indent).
;   - HILITE hook : invoked per visible line during render. The host fills a
;                   per-character color buffer (syntax highlighting).
;   - COMMAND hook: invoked for document-level commands that editbuf cannot own
;                   generically (New/Open/Save As). A = EDITUI_CMD_*.
;   - CHANGED hook: invoked after buffer bytes/metadata change. Hosts with
;                   XRAM-backed documents use this to sync the active document.
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
; Editor body geometry. Minimal full-screen layout (no frame, no title band):
;   row 0          : menu bar (File/Edit/Help + title + dirty marker)
;   rows 1..N-2    : editable body, full 80 columns
;   row N-1        : status bar (cursor/size/line/type metadata)
; ---------------------------------------------------------------------
EDITBUF_VIEW_LEFT   = 0                               ; first text column
EDITBUF_VIEW_TOP    = 1                               ; first text row (below menu bar)
EDITBUF_VIEW_COLS   = EDITUI_SCREEN_COLS              ; 80 visible columns
EDITBUF_VIEW_ROWS   = EDITUI_SCREEN_ROWS - 2          ; 48 (menu bar + status bar reserved)

; Clipboard and undo/redo payloads are owned by TEXTSVC in platform-managed
; XRAM so every language editor gets the same behavior.

; ---------------------------------------------------------------------
; Exit reason returned in A by editbuf_run (also stored in EDITBUF_RESULT).
; ---------------------------------------------------------------------
EDITBUF_EXIT_QUIT    = $00      ; Alt-X while clean
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
; The host may point EDITBUF_STATUS at a message string before returning. The
; editor consumes/clears that pointer, shows the text as a transient status
; message, then restores the normal cursor/size/line/type metadata bar.

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

; Default editor colors (bg = high nibble, fg = low nibble; Nova/C64 palette).
EDITBUF_COLOR_TEXT   = $61      ; white on blue body
EDITBUF_COLOR_SEL    = $30      ; selection: black on cyan

; --- Config (host sets before editbuf_run) -------------------------------
.global EDITBUF_BUFL          ; pointer to text buffer
.global EDITBUF_BUFH
.global EDITBUF_CAPL          ; capacity in bytes (16-bit)
.global EDITBUF_CAPH
.global EDITBUF_UNDOBUFL      ; legacy config bytes; ignored by TEXTSVC-backed undo
.global EDITBUF_UNDOBUFH
.global EDITBUF_REDOBUFL      ; legacy config bytes; ignored by TEXTSVC-backed redo
.global EDITBUF_REDOBUFH
.global EDITBUF_LENL          ; current length in bytes (16-bit)
.global EDITBUF_LENH
.global EDITBUF_TITLEL        ; NUL-terminated title string
.global EDITBUF_TITLEH
.global EDITBUF_TYPEL         ; NUL-terminated file type string
.global EDITBUF_TYPEH
.global EDITBUF_STATUSL       ; transient NUL-terminated status message (may be 0)
.global EDITBUF_STATUSH
.global EDITBUF_SAVE_VECL     ; SAVE hook vector
.global EDITBUF_SAVE_VECH
.global EDITBUF_INDENT_VECL   ; INDENT hook vector
.global EDITBUF_INDENT_VECH
.global EDITBUF_HILITE_VECL   ; HILITE hook vector
.global EDITBUF_HILITE_VECH
.global EDITBUF_MENU_VECL     ; MENU-config hook vector
.global EDITBUF_MENU_VECH
.global EDITBUF_COMMAND_VECL  ; document-command hook vector (A=EDITUI_CMD_*;
                              ; window commands set C if the window changed)
.global EDITBUF_COMMAND_VECH
.global EDITBUF_CHANGED_VECL  ; called after edits/save dirty-state changes
.global EDITBUF_CHANGED_VECH

; --- Result / observable state ------------------------------------------
.global EDITBUF_RESULT
.global EDITBUF_CURL          ; cursor byte offset (16-bit)
.global EDITBUF_CURH

; --- HILITE hook scratch (zp pointer + buffers) -------------------------
.globalzp EDITBUF_HL_PTR      ; zp, 2 bytes
.global EDITBUF_HL_LEN
.global EDITBUF_HL_COLORS     ; 80-byte color buffer

; --- Entry points -------------------------------------------------------
.global editbuf_run           ; run the modal editor
.global editbuf_reset_state   ; reset cursor/scroll/selection/dirty for a new session
.global editbuf_dialog3       ; three-choice status-line prompt (A=result 0/1/2)

; Dialog choice result codes (returned by editbuf_dialog3 in A).
EDITBUF_DLG_EXIT   = $00       ; first button  (Exit Anyway)
EDITBUF_DLG_SAVE   = $01       ; second button (Save First)
EDITBUF_DLG_CANCEL = $02       ; third button  (Cancel)

.endif

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

      .forceimport EDITBUF_HL_LEN
      .forceimport EDITBUF_HL_COLORS

; Editor display colors. Mirrors the extension's NovaLogo editor styling. The
; shared engine no longer dictates the active palette/colors; the caller's prior
; display is snapshotted and restored exactly. The editor colors are authored for
; the default Nova/C64 palette (0=black, 1=white).
EDITOR_BGCOL      = $00                ; black background
EDITOR_BORDER     = $00                ; black border
EDITOR_FGCOL      = $01                ; white text

; Scratch only while applying EDITOR_FN_EDIT hooks, before editbuf_run.
SE_HOOKL = LIB_ZP
SE_HOOKH = LIB_ZP+1
SE_DSTL  = LIB_ZP+2
SE_DSTH  = LIB_ZP+3

      .segment "CODE"
      lib_module_header MODULE_ID_EDITOR, LIB_ABI_VERSION, EDITOR_FN_COUNT

; ===========================================================================
; NDOC documentation (extracted by nova module pack into editor.nmod).
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
;@arg redo u16 optional redo snapshot buffer, same capacity as buffer (ARG1 high word)
;@arg capacity u16 buffer capacity in bytes (ARG2 low word)
;@arg undo u16 optional undo snapshot buffer, same capacity as buffer (ARG2 high word)
;@arg title u16 pointer to a NUL-terminated title string (ARG3 low word)
;@arg hooks u16 pointer to an EDITOR_HOOKS_* table (ARG3 high word, 0 = plain Text)
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
;        ARG1 lo word = current length  ARG1 hi word = optional redo buffer
;        ARG2 lo word = capacity        ARG2 hi word = optional undo buffer
;        ARG3 lo word = NUL title ptr
;   Out: RESULT+0 = editbuf exit reason  RESULT+1 = save flag (nonzero => save)
;        ARG1 lo word = final buffer length   LIB_STATUS = LERR_OK
; ===========================================================================
sys_edit:
      LDA   LIB_ARG3+2
      STA   z:SE_HOOKL
      LDA   LIB_ARG3+3
      STA   z:SE_HOOKH

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
      LDA   LIB_ARG2+2
      STA   EDITBUF_UNDOBUFL
      LDA   LIB_ARG2+3
      STA   EDITBUF_UNDOBUFH
      LDA   LIB_ARG1+2
      STA   EDITBUF_REDOBUFL
      LDA   LIB_ARG1+3
      STA   EDITBUF_REDOBUFH
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
      STZ   EDITBUF_HILITE_VECL
      STZ   EDITBUF_HILITE_VECH
      STZ   EDITBUF_MENU_VECL          ; keep EDITUI's default menus for now
      STZ   EDITBUF_MENU_VECH
      STZ   EDITBUF_COMMAND_VECL       ; no file/document commands in buffer-only EDITOR_FN_EDIT
      STZ   EDITBUF_COMMAND_VECH
      STZ   EDITBUF_CHANGED_VECL       ; no active-doc sync in the buffer-only editor
      STZ   EDITBUF_CHANGED_VECH

      LDA   z:SE_HOOKL
      ORA   z:SE_HOOKH
      BEQ   @hooks_done
      JSR   sys_edit_apply_hook_table
      BRA   @hooks_done

@hooks_done:

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
      STZ   LIB_ARG1+2
      STZ   LIB_ARG1+3

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

; sys_edit_hook_save_hook — hook-table EDIT records the module-level save flag,
; then tail-calls the caller's real save hook so A remains the hook status.
sys_edit_hook_save_hook:
      LDA   #1
      STA   se_saved_flag
      JMP   (se_hook_saveL)

; sys_edit_apply_hook_table — EDITOR_FN_EDIT overlays EDITBUF_TYPEL through
; EDITBUF_CHANGED_VECH from the caller's EDITOR_HOOKS_* table. The source table
; and destination config bytes intentionally share the same order.
sys_edit_apply_hook_table:
      LDA   #<EDITBUF_TYPEL
      STA   z:SE_DSTL
      LDA   #>EDITBUF_TYPEL
      STA   z:SE_DSTH
      LDY   #0
@copy:
      LDA   (SE_HOOKL),Y
      STA   (SE_DSTL),Y
      INY
      CPY   #EDITOR_HOOKS_SIZE
      BNE   @copy
      LDA   EDITBUF_SAVE_VECL
      STA   se_hook_saveL
      LDA   EDITBUF_SAVE_VECH
      STA   se_hook_saveH
      LDA   se_hook_saveL
      ORA   se_hook_saveH
      BEQ   @default_save
      LDA   #<sys_edit_hook_save_hook
      STA   EDITBUF_SAVE_VECL
      LDA   #>sys_edit_hook_save_hook
      STA   EDITBUF_SAVE_VECH
      BRA   @type
@default_save:
      LDA   #<sys_edit_save_hook
      STA   EDITBUF_SAVE_VECL
      LDA   #>sys_edit_save_hook
      STA   EDITBUF_SAVE_VECH
@type:
      LDA   EDITBUF_TYPEL
      ORA   EDITBUF_TYPEH
      BNE   @done
      LDA   #<sys_edit_type_text
      STA   EDITBUF_TYPEL
      LDA   #>sys_edit_type_text
      STA   EDITBUF_TYPEH
@done:
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
se_hook_saveL:     .res 1               ; caller save hook from EDITOR_HOOKS_*
se_hook_saveH:     .res 1
      .segment "CODE"

; ===========================================================================
; Shared driver + editor bodies (all include-guarded).
; copper.s -> copper_off. editui.s pulls vtext.s -> blitter.s. editbuf.s.
; ===========================================================================
      .include "copper.inc"
      .include "editui.inc"
      .include "editbuf.inc"
      .segment "RODATA"
sys_edit_type_text:
      .byte "Text", 0

      .segment "CODE"

      .segment "VECTORS"
      ; Module runs under SEI (the loader masks IRQ across the bank swap); hardware
      ; vectors never fire from bank 1. Fill with a safe entry-point stub.
      .word   $C000, $C000, $C000      ; NMI, RESET, IRQ

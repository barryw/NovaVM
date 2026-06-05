; system.s — SYSTEM module (MODULE_ID_SYSTEM = $03), the shared system-services
; library. Built once, staged to the XRAM shelf, paged into bank 1 on the first
; lib_call. Header "NL"/$03 at $C000 (lib_module_header); RTS-trick dispatch on
; LIB_FN_ID.
;
; Phase A: one fn — SYS_FN_EDIT, the interactive EDITUI/EDITBUF text editor moved
; verbatim out of the NovaLogo extension ROM. The editor is language-neutral
; (editbuf.inc): it edits a flat RAM text buffer addressed through the mailbox and
; runs entirely within one lib_call (keyboard + screen are MMIO, which survive the
; bank swap). The host builds any document record from the final buffer AFTER the
; editor returns — no module->runtime callback is required.

      .include "libabi.inc"
      .include "libmod.inc"
      .include "libsystem.inc"
      .include "nova.inc"              ; VGC_*/TEXTWIN_* register equates + ROMSWAP
      .include "vgc.inc"               ; (guarded) VGC command/constant equates
      .include "copper.inc"            ; copper_off .global
      .include "editbuf.inc"           ; EDITBUF_* config + editbuf_run/_reset_state

; Editor display colors. Mirrors the extension's NovaLogo editor styling. The
; shared engine no longer dictates the global palette/colors; the caller's prior
; display is snapshotted and restored exactly. (Palette mode 0: 0=black, 1=white.)
EDITOR_BGCOL      = $00                ; black background
EDITOR_BORDER     = $00                ; black border
EDITOR_FGCOL      = $01                ; white text

      .segment "CODE"
      lib_module_header MODULE_ID_SYSTEM, LIB_ABI_VERSION, SYS_FN_COUNT

; ===========================================================================
; NDOC documentation (extracted by tools/nmod_pack.py into system.nmod).
; Function ids resolve symbolically from runtime/asm/libsystem.inc.
; ===========================================================================
;@module SYSTEM
;@version 1.0
;@brief Shared system services: the interactive full-screen text editor.
;
;@fn SYS_FN_EDIT
;@brief Run the modal EDITUI/EDITBUF text editor on a host RAM text buffer.
;@arg buffer u16 pointer to the editable text buffer (ARG0 low word)
;@arg cursor u16 initial cursor byte offset (ARG0 high word)
;@arg length u16 current text length in bytes (ARG1 low word)
;@arg capacity u16 buffer capacity in bytes (ARG2 low word)
;@arg title u16 pointer to a NUL-terminated title string (ARG3 low word)
;@ret u8 exit reason (RESULT byte0, EDITBUF_EXIT_*); save flag (RESULT byte1); final length (ARG1)
;@effect Snapshots and restores the caller's full VGC display state; owns the
;@effect whole 80x25 text screen while running. Blocks until the user exits.
;@status LERR_OK
;
;@fn SYS_FN_WAIT
;@ndk vgc_wait_frames
;@brief Busy-wait a number of video frames.
;@arg n u16 frame count (ARG0)
;@ret void
;@status LERR_OK
; (local @brief override: NDK vgc_wait_frames summary names the A register.)
;
;@fn SYS_FN_WAITVBL
;@ndk vgc_vsync
;@ret void
;@status LERR_OK
;
;@fn SYS_FN_TIMER
;@brief Read the VGC frame counter.
;@ret u8 current frame counter (RESULT byte0)
;@status LERR_OK
; (no ;@ndk: reads VGC_FRAME directly; not a wrapped NDK routine.)

; ---------------------------------------------------------------------------
; dispatch — fn-id router. RTS-trick: push (target-1) hi/lo, RTS jumps to target.
; SYS_FN_COUNT is small (1) so fn*2 cannot exceed 255; an 8-bit asl/tax is safe.
; ---------------------------------------------------------------------------
dispatch:
      lda     LIB_FN_ID
      cmp     #SYS_FN_COUNT
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
      .word   sys_edit-1               ; $00 SYS_FN_EDIT
      .word   sys_wait-1               ; $01 SYS_FN_WAIT
      .word   sys_waitvbl-1            ; $02 SYS_FN_WAITVBL
      .word   sys_timer-1              ; $03 SYS_FN_TIMER

; ===========================================================================
; SYS_FN_EDIT — port of the extension's ext_edit, reading the canonical lib_call
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
      ; Restore the host's palette + colors FIRST: the form-feed clear below fills
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

; ===========================================================================
; Timing services (moved from the NovaLogo extension ROM in Phase B). THIN wrappers
; over the NDK vgc frame-timing primitives (vgc.s); the NDK owns the wait logic.
; ===========================================================================

; --- $01 SYS_FN_WAIT: ARG0 = n frames -> NDK vgc_wait_frames ---
sys_wait:
      LDA   LIB_ARG0+0
      JSR   vgc_wait_frames
      LDA   #LERR_OK
      STA   LIB_STATUS
      RTS

; --- $02 SYS_FN_WAITVBL: wait for the next vertical blank -> NDK vgc_vsync ---
sys_waitvbl:
      JSR   vgc_vsync
      LDA   #LERR_OK
      STA   LIB_STATUS
      RTS

; --- $03 SYS_FN_TIMER: reporter -> RESULT = VGC frame counter (8-bit MMIO read) ---
sys_timer:
      LDA   VGC_FRAME
      STA   LIB_RESULT+0
      STZ   LIB_RESULT+1
      STZ   LIB_RESULT+2
      STZ   LIB_RESULT+3
      LDA   #LERR_OK
      STA   LIB_STATUS
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
se_saved_flag:    .res 1               ; nonzero once the SAVE hook fires
      .segment "CODE"

; ===========================================================================
; Shared driver + editor bodies (all include-guarded).
; copper.s -> copper_off. editui.s pulls vtext.s -> blitter.s. editbuf.s.
; ===========================================================================
      .include "copper.s"
      .include "editui.s"
      .include "editbuf.s"
      .include "vgc.s"                 ; NDK frame-timing: vgc_vsync / vgc_wait_frames

      .segment "VECTORS"
      ; Module runs under SEI (the loader masks IRQ across the bank swap); hardware
      ; vectors never fire from bank 1. Fill with a safe entry-point stub.
      .word   $C000, $C000, $C000      ; NMI, RESET, IRQ

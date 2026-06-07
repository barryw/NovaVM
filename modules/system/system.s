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
      .include "rng.inc"               ; RNG_VALUE0..3 (= FIO_RNG*) + rng_get* .globals
      .include "nui.inc"               ; NUI_DIALOG_*/NUI_TITLE*/MSG*/FOOTER* + nui_* .globals
      .include "overlay.inc"           ; OVL_NAMEPTR_*/LOAD*/MAXLEN* + overlay_* .globals

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
;
; --- host random-number service (rng.s) ---
;@fn SYS_RNG8
;@ndk rng_get8
;@ret u8 random value (RESULT byte0)
;@status LERR_OK, LERR_SYS_FAIL
;
;@fn SYS_RNG16
;@ndk rng_get16
;@ret u16 random value (RESULT byte0..1)
;@status LERR_OK, LERR_SYS_FAIL
;
;@fn SYS_RNG32
;@ndk rng_get32
;@ret u32 random value (RESULT byte0..3)
;@status LERR_OK, LERR_SYS_FAIL
;
; --- modal application UI (nui.s) ---
;@fn SYS_DIALOG_DEFAULTS
;@ndk nui_dialog_defaults
;@ret void
;@status LERR_OK, LERR_SYS_FAIL
;
;@fn SYS_DIALOG
;@ndk nui_show_dialog
;@arg title u16 NUL title ptr -> NUI_TITLEL/H (ARG0 byte0,1)
;@arg message u16 NUL message ptr -> NUI_MSGL/H (ARG1 byte0,1)
;@arg footer u16 NUL footer ptr -> NUI_FOOTERL/H (ARG2 byte0,1)
;@arg geom u32 left/top/width/height -> NUI_DIALOG_LEFT/TOP/WIDTH/HEIGHT (ARG3 byte0,1,2,3; all-zero keeps current geometry)
;@ret void
;@status LERR_OK, LERR_SYS_FAIL
;
;@fn SYS_DIALOG_WAIT
;@ndk nui_show_dialog_wait
;@arg title u16 NUL title ptr -> NUI_TITLEL/H (ARG0 byte0,1)
;@arg message u16 NUL message ptr -> NUI_MSGL/H (ARG1 byte0,1)
;@arg footer u16 NUL footer ptr -> NUI_FOOTERL/H (ARG2 byte0,1)
;@arg geom u32 left/top/width/height -> NUI_DIALOG_LEFT/TOP/WIDTH/HEIGHT (ARG3 byte0,1,2,3; all-zero keeps current geometry)
;@ret void
;@status LERR_OK, LERR_SYS_FAIL
;
;@fn SYS_DIALOG_ERROR
;@ndk nui_show_error
;@arg title u16 NUL title ptr -> NUI_TITLEL/H (ARG0 byte0,1)
;@arg message u16 NUL message ptr -> NUI_MSGL/H (ARG1 byte0,1)
;@arg footer u16 NUL footer ptr -> NUI_FOOTERL/H (ARG2 byte0,1)
;@arg geom u32 left/top/width/height -> NUI_DIALOG_LEFT/TOP/WIDTH/HEIGHT (ARG3 byte0,1,2,3; all-zero applies the default geometry)
;@ret void
;@status LERR_OK, LERR_SYS_FAIL
;
;@fn SYS_WAIT_KEY
;@ndk nui_wait_key
;@ret u8 pressed key byte (RESULT byte0)
;@status LERR_OK
;
; --- fixed-address overlay manager (overlay.s) ---
;@fn SYS_OVL_LOAD
;@ndk overlay_load_fixed
;@arg nameptr u16 overlay filename ptr -> OVL_NAMEPTR_L/H (ARG0 byte0,1)
;@arg namelen u8 filename length -> OVL_NAMELEN (ARG1 byte0)
;@arg load u16 fixed CPU RAM load address -> OVL_LOADL/H (ARG2 byte0,1)
;@arg maxlen u16 slot capacity (payload+BSS must fit) -> OVL_MAXLENL/H (ARG3 byte0,1)
;@ret u8 OVL_ERR_* detail on failure (RESULT byte1)
;@status LERR_OK, LERR_SYS_FAIL
;
;@fn SYS_OVL_UNLOAD
;@ndk overlay_unload
;@ret u8 OVL_ERR_* detail on failure (RESULT byte1)
;@status LERR_OK, LERR_SYS_FAIL
;
;@fn SYS_OVL_INIT
;@ndk overlay_call_init
;@ret u8 OVL_ERR_* detail on failure (RESULT byte1)
;@status LERR_OK, LERR_SYS_FAIL
;
;@fn SYS_OVL_MAIN
;@ndk overlay_call_main
;@ret u8 OVL_ERR_* detail on failure (RESULT byte1)
;@status LERR_OK, LERR_SYS_FAIL
;
;@fn SYS_OVL_TICK
;@ndk overlay_call_tick
;@ret u8 OVL_ERR_* detail on failure (RESULT byte1)
;@status LERR_OK, LERR_SYS_FAIL
;
; --- runtime label lookup (backs BASIC's ADDR("LABEL")) ---
;@fn SYS_ADDR_LOOKUP
;@brief Resolve a generated runtime label name to its 16-bit address.
;@arg nameptr u16 pointer to the label name bytes (ARG0 byte0,1)
;@arg namelen u8 label length in bytes (ARG0 byte2)
;@ret u16 resolved address (RESULT byte0,1) on a hit
;@effect Hashes the UPPERCASED name (x33) into the baked runtime_labels.inc
;@effect bucket table and walks the chain; returns the value as raw data.
;@status LERR_OK, LERR_SYS_FAIL
; (no ;@ndk: the hash/bucket walk is local to this module, not a wrapped NDK routine.)
;
; --- C64-style full-screen line editor (backs BASIC's LAB_1357) ---
;@fn SYS_SCREEN_READLINE
;@brief Run the full-screen edit loop; on ENTER read the row under the cursor.
;@arg buffer u16 destination buffer ptr (ARG0 byte0,1)
;@arg maxlen u8 buffer capacity in bytes (ARG0 byte2)
;@ret u8 submitted line length (RESULT byte0); buffer filled, no trailing NUL
;@effect Drives the cursor + screen directly through VGC MMIO (CHARIN, the
;@effect char-plane screen window, cursor X/Y). On ENTER copies the physical
;@effect screen row at the cursor (trailing spaces trimmed) into the buffer.
;@status LERR_OK
; (no ;@ndk: the edit loop is local to this module, not a wrapped NDK routine.)

; ---------------------------------------------------------------------------
; dispatch — fn-id router. RTS-trick: push (target-1) hi/lo, RTS jumps to target.
; SYS_FN_COUNT is small (19) so fn*2 cannot exceed 255; an 8-bit asl/tax is safe.
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
      .word   sys_rng8-1               ; $04 SYS_RNG8
      .word   sys_rng16-1              ; $05 SYS_RNG16
      .word   sys_rng32-1              ; $06 SYS_RNG32
      .word   sys_dialog_defaults-1    ; $07 SYS_DIALOG_DEFAULTS
      .word   sys_dialog-1             ; $08 SYS_DIALOG
      .word   sys_dialog_wait-1        ; $09 SYS_DIALOG_WAIT
      .word   sys_dialog_error-1       ; $0A SYS_DIALOG_ERROR
      .word   sys_wait_key-1           ; $0B SYS_WAIT_KEY
      .word   sys_ovl_load-1           ; $0C SYS_OVL_LOAD
      .word   sys_ovl_unload-1         ; $0D SYS_OVL_UNLOAD
      .word   sys_ovl_init-1           ; $0E SYS_OVL_INIT
      .word   sys_ovl_main-1           ; $0F SYS_OVL_MAIN
      .word   sys_ovl_tick-1           ; $10 SYS_OVL_TICK
      .word   sys_addr_lookup-1        ; $11 SYS_ADDR_LOOKUP
      .word   sys_screen_readline-1    ; $12 SYS_SCREEN_READLINE

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

; ===========================================================================
; Shared status epilogues for the fallible rng/nui/overlay wrappers. The NDK
; routine returns A=0 on success / A!=0 on error; sys_finish_status maps that to
; LIB_STATUS (and clears RESULT). NOT ;@ndk-mapped, so the drift guard's
; per-wrapper JSR check ignores them.
; ===========================================================================
sys_finish_status:
      CMP   #$00
      BNE   @fail
      STZ   LIB_RESULT
      STZ   LIB_RESULT+1
      STZ   LIB_RESULT+2
      STZ   LIB_RESULT+3
      LDA   #LERR_OK
      STA   LIB_STATUS
      RTS
@fail:
      STZ   LIB_RESULT
      STZ   LIB_RESULT+1
      STZ   LIB_RESULT+2
      STZ   LIB_RESULT+3
      LDA   #LERR_SYS_FAIL
      STA   LIB_STATUS
      RTS

; ===========================================================================
; Host random-number service. THIN wrappers over the rng NDK (rng.s). All three
; share one host entropy call and differ only in how many returned bytes the
; wrapper publishes to LIB_RESULT (RNG_VALUE0..3 = FIO_RNG0..3 hardware regs).
; ===========================================================================

; --- $04 SYS_RNG8: reporter -> RESULT b0 = 8 random bits -> rng_get8 ---
sys_rng8:
      JSR   rng_get8
      CMP   #$00
      BNE   @fail
      LDA   RNG_VALUE0
      STA   LIB_RESULT+0
      STZ   LIB_RESULT+1
      STZ   LIB_RESULT+2
      STZ   LIB_RESULT+3
      LDA   #LERR_OK
      STA   LIB_STATUS
      RTS
@fail:
      LDA   #LERR_SYS_FAIL
      STA   LIB_STATUS
      RTS

; --- $05 SYS_RNG16: reporter -> RESULT b0..1 = 16 random bits -> rng_get16 ---
sys_rng16:
      JSR   rng_get16
      CMP   #$00
      BNE   @fail
      LDA   RNG_VALUE0
      STA   LIB_RESULT+0
      LDA   RNG_VALUE1
      STA   LIB_RESULT+1
      STZ   LIB_RESULT+2
      STZ   LIB_RESULT+3
      LDA   #LERR_OK
      STA   LIB_STATUS
      RTS
@fail:
      LDA   #LERR_SYS_FAIL
      STA   LIB_STATUS
      RTS

; --- $06 SYS_RNG32: reporter -> RESULT b0..3 = 32 random bits -> rng_get32 ---
sys_rng32:
      JSR   rng_get32
      CMP   #$00
      BNE   @fail
      LDA   RNG_VALUE0
      STA   LIB_RESULT+0
      LDA   RNG_VALUE1
      STA   LIB_RESULT+1
      LDA   RNG_VALUE2
      STA   LIB_RESULT+2
      LDA   RNG_VALUE3
      STA   LIB_RESULT+3
      LDA   #LERR_OK
      STA   LIB_STATUS
      RTS
@fail:
      LDA   #LERR_SYS_FAIL
      STA   LIB_STATUS
      RTS

; ===========================================================================
; Modal application UI. THIN wrappers over the nui NDK (nui.s). The wrapper
; stages the title/message/footer string pointers and (optionally) the dialog
; geometry into the NUI_* BSS the NDK reads, then JSRs the NDK routine.
; ===========================================================================

; sys_marshal_nui — ARG0/1/2 b0,b1 -> NUI_TITLE/MSG/FOOTER ptrs; ARG3 bytes ->
; NUI_DIALOG_LEFT/TOP/WIDTH/HEIGHT only when any ARG3 byte is nonzero (all-zero
; ARG3 keeps the current/default geometry). NOT ;@ndk-mapped.
sys_marshal_nui:
      LDA   LIB_ARG0+0
      STA   NUI_TITLEL
      LDA   LIB_ARG0+1
      STA   NUI_TITLEH
      LDA   LIB_ARG1+0
      STA   NUI_MSGL
      LDA   LIB_ARG1+1
      STA   NUI_MSGH
      LDA   LIB_ARG2+0
      STA   NUI_FOOTERL
      LDA   LIB_ARG2+1
      STA   NUI_FOOTERH
      ; geometry: only override when ARG3 carries one (any byte nonzero)
      LDA   LIB_ARG3+0
      ORA   LIB_ARG3+1
      ORA   LIB_ARG3+2
      ORA   LIB_ARG3+3
      BEQ   @keep
      LDA   LIB_ARG3+0
      STA   NUI_DIALOG_LEFT
      LDA   LIB_ARG3+1
      STA   NUI_DIALOG_TOP
      LDA   LIB_ARG3+2
      STA   NUI_DIALOG_WIDTH
      LDA   LIB_ARG3+3
      STA   NUI_DIALOG_HEIGHT
@keep:
      RTS

; --- $07 SYS_DIALOG_DEFAULTS: () -> nui_dialog_defaults ---
sys_dialog_defaults:
      JSR   nui_dialog_defaults
      JMP   sys_finish_status

; --- $08 SYS_DIALOG: (title,msg,footer,geom) -> nui_show_dialog ---
sys_dialog:
      JSR   sys_marshal_nui
      JSR   nui_show_dialog
      JMP   sys_finish_status

; --- $09 SYS_DIALOG_WAIT: (title,msg,footer,geom) -> nui_show_dialog_wait ---
sys_dialog_wait:
      JSR   sys_marshal_nui
      JSR   nui_show_dialog_wait
      JMP   sys_finish_status

; --- $0A SYS_DIALOG_ERROR: (title,msg,footer,geom) -> nui_show_error ---
sys_dialog_error:
      JSR   sys_marshal_nui
      JSR   nui_show_error
      JMP   sys_finish_status

; --- $0B SYS_WAIT_KEY: reporter -> RESULT b0 = pressed key byte -> nui_wait_key ---
sys_wait_key:
      JSR   nui_wait_key            ; A = key byte (not a 0/1 status)
      STA   LIB_RESULT+0
      STZ   LIB_RESULT+1
      STZ   LIB_RESULT+2
      STZ   LIB_RESULT+3
      LDA   #LERR_OK
      STA   LIB_STATUS
      RTS

; ===========================================================================
; Fixed-address overlay manager. THIN wrappers over the overlay NDK (overlay.s).
; The OVL_ERR_* detail code (OVL_RESULT) is published to RESULT b1 so the caller
; can distinguish IO / magic / size / entry failures; A!=0 -> LERR_SYS_FAIL.
; ===========================================================================

; sys_finish_overlay — map the overlay NDK A result to LIB_STATUS and surface the
; OVL_RESULT detail in RESULT b1. NOT ;@ndk-mapped.
sys_finish_overlay:
      STZ   LIB_RESULT+0
      LDA   OVL_RESULT
      STA   LIB_RESULT+1
      STZ   LIB_RESULT+2
      STZ   LIB_RESULT+3
      LDA   OVL_RESULT
      CMP   #$00
      BNE   @fail
      LDA   #LERR_OK
      STA   LIB_STATUS
      RTS
@fail:
      LDA   #LERR_SYS_FAIL
      STA   LIB_STATUS
      RTS

; --- $0C SYS_OVL_LOAD: name + load addr + maxlen -> overlay_load_fixed ---
sys_ovl_load:
      LDA   LIB_ARG0+0
      STA   OVL_NAMEPTR_L
      LDA   LIB_ARG0+1
      STA   OVL_NAMEPTR_H
      LDA   LIB_ARG1+0
      STA   OVL_NAMELEN
      LDA   LIB_ARG2+0
      STA   OVL_LOADL
      LDA   LIB_ARG2+1
      STA   OVL_LOADH
      LDA   LIB_ARG3+0
      STA   OVL_MAXLENL
      LDA   LIB_ARG3+1
      STA   OVL_MAXLENH
      JSR   overlay_load_fixed
      JMP   sys_finish_overlay

; --- $0D SYS_OVL_UNLOAD: () -> overlay_unload ---
sys_ovl_unload:
      JSR   overlay_unload
      JMP   sys_finish_overlay

; --- $0E SYS_OVL_INIT: () -> overlay_call_init ---
sys_ovl_init:
      JSR   overlay_call_init
      JMP   sys_finish_overlay

; --- $0F SYS_OVL_MAIN: () -> overlay_call_main ---
sys_ovl_main:
      JSR   overlay_call_main
      JMP   sys_finish_overlay

; --- $10 SYS_OVL_TICK: () -> overlay_call_tick ---
sys_ovl_tick:
      JSR   overlay_call_tick
      JMP   sys_finish_overlay

; ===========================================================================
; SYS_ADDR_LOOKUP — runtime label lookup backing BASIC's ADDR("LABEL").
; Faithful relocation of the extension ROM's EXT_ADDR/ext_label_hash/ext_upper:
; the hash (x33 over UPPERCASED chars, masked to bucket count), the bucket-chain
; walk, the entry format (len byte + 16-bit LE value + name bytes), and the
; uppercasing are byte-for-byte identical in logic. Only the I/O changes: the
; name ptr/len arrive in ARG0 instead of EXT_ARG_*, and the result is published
; to LIB_RESULT/LIB_STATUS instead of returned in registers.
;
; ZP usage: SYSTEM's module-ZP band ($A3-$C2) is all but full (only $C1-$C2
; free), so the three live indirect pointers ALIAS the editor's ZP working-window
; cells. sys_addr_lookup and SYS_FN_EDIT never run in the same lib_call (each
; lib_call runs exactly one fn to completion), so reusing the editor's pointer
; cells is safe. This mirrors how the extension aliased its scratch onto ext_*.
; The non-pointer scratch (len/ch/hash) lives in module BSS (no ZP needed).
;   addr_entryL/H  = entry-base ptr        (EB_SRCL/EB_SRCH)
;   addr_nameL/H   = entry name-bytes ptr  (EB_DSTL/EB_DSTH)
;   addr_argL/H    = input name ptr        (EB_PL/EB_PH, seeded from ARG0)
addr_entryL = EB_SRCL
addr_entryH = EB_SRCH
addr_nameL  = EB_DSTL
addr_nameH  = EB_DSTH
addr_argL   = EB_PL
addr_argH   = EB_PH

;   In:  ARG0 b0/b1 = name ptr   ARG0 b2 = name len
;   Out (hit):  RESULT b0/b1 = 16-bit address; LIB_STATUS = LERR_OK
;   Out (miss / len 0): LIB_STATUS = LERR_SYS_FAIL
sys_addr_lookup:
      ; seed the input-name pointer from ARG0 b0/b1, length from ARG0 b2
      LDA   LIB_ARG0+0
      STA   addr_argL
      LDA   LIB_ARG0+1
      STA   addr_argH
      LDA   LIB_ARG0+2
      STA   sal_len
      BNE   @have_name              ; empty name -> miss (status set inline)
      LDA   #LERR_SYS_FAIL
      STA   LIB_STATUS
      RTS
@have_name:
      JSR   sal_label_hash
      AND   #(RUNTIME_LABEL_BUCKET_COUNT-1)
      ASL
      TAX
      LDA   RuntimeLabelBuckets,X
      STA   addr_entryL
      LDA   RuntimeLabelBuckets+1,X
      STA   addr_entryH

@entry_loop:
      LDY   #$00
      LDA   (addr_entryL),Y
      BEQ   @not_found              ; bucket terminator -> miss
      STA   sal_entrylen
      CMP   sal_len
      BNE   @next_entry

      CLC
      LDA   addr_entryL
      ADC   #$03                    ; skip len + 16-bit value
      STA   addr_nameL
      LDA   addr_entryH
      ADC   #$00
      STA   addr_nameH

      LDX   #$00
@cmp_loop:
      CPX   sal_entrylen
      BEQ   @found
      TXA
      TAY
      LDA   (addr_argL),Y
      JSR   sal_upper
      STA   sal_ch
      LDA   (addr_nameL),Y
      CMP   sal_ch
      BNE   @next_entry
      INX
      BRA   @cmp_loop

@found:
      LDY   #$01
      LDA   (addr_entryL),Y         ; value low
      STA   LIB_RESULT+0
      INY
      LDA   (addr_entryL),Y         ; value high
      STA   LIB_RESULT+1
      STZ   LIB_RESULT+2
      STZ   LIB_RESULT+3
      LDA   #LERR_OK
      STA   LIB_STATUS
      RTS

@next_entry:
      CLC
      LDA   addr_entryL
      ADC   sal_entrylen
      STA   addr_entryL
      BCC   @add_entry_header
      INC   addr_entryH
@add_entry_header:
      CLC
      LDA   addr_entryL
      ADC   #$03
      STA   addr_entryL
      BCC   @entry_loop
      INC   addr_entryH
      BRA   @entry_loop

@not_found:
      LDA   #LERR_SYS_FAIL
      STA   LIB_STATUS
      RTS

; sal_label_hash — x33 hash over the UPPERCASED input name; returns hash in A.
sal_label_hash:
      STZ   sal_hash
      LDY   #$00
@hash_loop:
      CPY   sal_len
      BEQ   @hash_done
      LDA   (addr_argL),Y
      JSR   sal_upper
      STA   sal_ch
      LDA   sal_hash
      ASL
      ASL
      ASL
      ASL
      ASL
      CLC
      ADC   sal_hash
      CLC
      ADC   sal_ch
      STA   sal_hash
      INY
      BRA   @hash_loop
@hash_done:
      LDA   sal_hash
      RTS

; sal_upper — ASCII a..z -> A..Z; passes everything else through.
sal_upper:
      CMP   #'a'
      BCC   @upper_done
      CMP   #'z'+1
      BCS   @upper_done
      AND   #$DF
@upper_done:
      RTS

; ===========================================================================
; SYS_SCREEN_READLINE — C64-style full-screen line reader (backs BASIC's
; LAB_1357). Task 2 (core mechanic): poll keys; on ENTER ($0D) copy the physical
; screen row under the cursor into the caller's buffer (trailing spaces trimmed)
; and return its length. Echo / arrows / backspace / scroll land in later chunks.
;
; ROW MAPPING (established empirically by ScreenReadline_ReadsRowUnderCursorOnEnter):
;   The $A200 screen window and VGC_CURSY both index the PHYSICAL plane row
;   directly. VGC_TEXT_TOPROW ($A0ED) only remaps display-row -> physical-row at
;   render time (the renderer's ring scroll); it does NOT shift the window's cell
;   addressing or the plane row VGC_CURSY refers to. So the window row for the
;   visible cursor row CURSY is CURSY itself — NOT (CURSY+TOPROW) mod 50. The
;   row base is therefore CURSY*80 with no TOPROW term. (Confirmed: the test
;   passes with base=CURSY*80 and leaves TOPROW at its default 0; the model is
;   self-consistent for any TOPROW since cursor and window share the plane row.)
;
; ZP usage: like sys_addr_lookup, this fn never runs in the same lib_call as the
; editor, so it ALIASES the editor's ZP pointer cells:
;   srl_winL/H  = the $A200 row-base window pointer  (EB_SRCL/EB_SRCH)
;   srl_dstL/H  = the caller's destination buffer ptr (EB_DSTL/EB_DSTH)
; Non-pointer scratch (maxlen / col / trimmed length) lives in module BSS.
;   In:  ARG0 b0/b1 = destination buffer ptr   ARG0 b2 = max length
;   Out: RESULT b0 = trimmed line length; buffer filled (no trailing NUL);
;        LIB_STATUS = LERR_OK
; ===========================================================================
srl_winL = EB_SRCL
srl_winH = EB_SRCH
srl_dstL = EB_DSTL
srl_dstH = EB_DSTH

sys_screen_readline:
      ; --- marshal the destination buffer ptr + max length from ARG0 ---
      LDA   LIB_ARG0+0
      STA   srl_dstL
      LDA   LIB_ARG0+1
      STA   srl_dstH
      LDA   LIB_ARG0+2
      STA   srl_maxlen

      ; --- the editor reads/writes through the char plane window ---
      LDA   #VGC_SCREENWIN_CHAR
      STA   VGC_SCREENWIN_PLANE

      ; --- the editor owns the cursor while it runs: make it visible at entry ---
      LDA   #$01
      STA   VGC_CURSEN

      ; --- key dispatch loop. Poll CHARIN (0 = empty -> keep polling); on a key,
      ; route to ENTER (read-row) or echo (printable >= $20). Arrows / backspace /
      ; scroll land in later chunks; unknown control bytes are ignored. ---
@poll:
      LDA   VGC_CHARIN
      BEQ   @poll
      CMP   #$0D
      BEQ   @enter                     ; ENTER -> read the row, return
      CMP   #$20
      BCS   @printable                 ; >= $20 -> echo the char
      BRA   @poll                      ; other control codes ignored for now

      ; --- echo a printable char at the cursor, then advance the cursor ---
@printable:
      PHA                              ; save the char to write
      LDA   VGC_CURSY
      JSR   screen_row_base            ; srl_winL/H = $A200 + CURSY*80
      PLA
      LDY   VGC_CURSX
      STA   (srl_winL),Y               ; write the char at the cursor cell
      ; advance CURSX; at column 80 wrap to col 0 of the next row (clamp row <= 49)
      INY
      CPY   #NOVA_SCREEN_COLS
      BCC   @echo_setx                 ; still within the row -> just store X
      LDY   #$00                       ; wrap: X = 0
      LDA   VGC_CURSY
      CMP   #(NOVA_SCREEN_ROWS-1)
      BCS   @echo_setx                 ; already on the last row -> don't advance Y
      INC   VGC_CURSY                  ; advance to the next row
@echo_setx:
      STY   VGC_CURSX
      BRA   @poll

      ; --- ENTER: read the physical row under the cursor into the buffer ---
      ; Window row base = VGC_CURSY*80 -> srl_winL/H (= $A200 + CURSY*80).
@enter:
      LDA   VGC_CURSY
      JSR   screen_row_base

      ; Pass 1: scan cols 0..min(maxlen,80)-1 for the trimmed length (last
      ; non-space col + 1). Read-only; the buffer is not touched yet so any cell
      ; past the line stays at the caller's value (no padding, no NUL).
      STZ   srl_trim                   ; trimmed length so far
      LDY   #$00                       ; column index
@scan:
      CPY   srl_maxlen
      BCS   @copy                      ; reached the buffer capacity -> stop scanning
      CPY   #NOVA_SCREEN_COLS          ; reached the right edge (80 cols)
      BCS   @copy
      LDA   (srl_winL),Y               ; row cell at $A200 + base + Y
      CMP   #$20
      BEQ   @scan_next                 ; space: does not extend the trimmed length
      INY
      STY   srl_trim                   ; non-space -> trimmed length = Y+1
      DEY
@scan_next:
      INY
      BRA   @scan

      ; Pass 2: copy exactly srl_trim bytes (the trimmed line) into the buffer.
@copy:
      LDY   #$00
@copy_loop:
      CPY   srl_trim
      BCS   @done_copy
      LDA   (srl_winL),Y
      STA   (srl_dstL),Y
      INY
      BRA   @copy_loop

@done_copy:
      LDA   srl_trim                   ; trailing spaces already trimmed off
      STA   LIB_RESULT+0
      STZ   LIB_RESULT+1               ; reserved exit reason = 0 (ENTER)
      STZ   LIB_RESULT+2
      STZ   LIB_RESULT+3
      LDA   #LERR_OK
      STA   LIB_STATUS
      RTS

; screen_row_base — A = screen row (0..49) -> srl_winL/H = VGC_SCREENWIN + row*80.
; 16-bit math: row*80 = (row<<6) + (row<<4). Reused by the read/echo/backspace/
; scroll paths in later chunks (it always yields the $A200 cell-0 address of a row).
screen_row_base:
      STA   srb_row                    ; save row
      ; row<<6 (16-bit) -> srl_winL/H
      STZ   srl_winH
      ASL   A                          ; <<1
      ROL   srl_winH
      ASL   A                          ; <<2
      ROL   srl_winH
      ASL   A                          ; <<3
      ROL   srl_winH
      ASL   A                          ; <<4
      ROL   srl_winH
      ASL   A                          ; <<5
      ROL   srl_winH
      ASL   A                          ; <<6
      ROL   srl_winH
      STA   srl_winL                   ; srl_winL/H = row*64
      ; + row<<4 (16-bit) in srb_t L/H
      LDA   srb_row
      STZ   srb_tH
      ASL   A
      ROL   srb_tH
      ASL   A
      ROL   srb_tH
      ASL   A
      ROL   srb_tH
      ASL   A
      ROL   srb_tH                     ; srb_tL(=A)/srb_tH = row*16
      CLC
      ADC   srl_winL
      STA   srl_winL
      LDA   srb_tH
      ADC   srl_winH
      STA   srl_winH                   ; srl_winL/H = row*80
      ; + VGC_SCREENWIN base ($A200). The low add is #$00 (no carry generated),
      ; but make the carry chain explicit with a defensive CLC before the hi add.
      LDA   srl_winL
      CLC
      ADC   #<VGC_SCREENWIN
      STA   srl_winL
      LDA   srl_winH
      CLC
      ADC   #>VGC_SCREENWIN
      STA   srl_winH
      RTS

      .segment "BSS"
srl_maxlen:       .res 1               ; destination buffer capacity (ARG0 b2)
srl_trim:         .res 1               ; trimmed line length (last non-space col + 1)
srb_row:          .res 1               ; screen_row_base: saved row
srb_tH:           .res 1               ; screen_row_base: row<<4 high byte
se_saved_mode:    .res 1
se_saved_palette: .res 1
se_saved_bgcol:   .res 1
se_saved_border:  .res 1
se_saved_fgcol:   .res 1
se_saved_cursx:   .res 1
se_saved_cursy:   .res 1
se_saved_cursen:  .res 1
se_saved_flag:    .res 1               ; nonzero once the SAVE hook fires
; sys_addr_lookup non-pointer scratch (the 3 live indirect pointers alias the
; editor's ZP cells; only these counters/temps need fresh storage).
sal_len:          .res 1               ; input name length (ARG0 b2)
sal_entrylen:     .res 1               ; current bucket entry's name length
sal_ch:           .res 1               ; uppercased compare byte / hash temp
sal_hash:         .res 1               ; running x33 hash accumulator
      .segment "CODE"

; ===========================================================================
; Shared driver + editor bodies (all include-guarded).
; copper.s -> copper_off. editui.s pulls vtext.s -> blitter.s. editbuf.s.
; ===========================================================================
      .include "copper.s"
      .include "editui.s"
      .include "editbuf.s"
      .include "vgc.s"                 ; NDK frame-timing: vgc_vsync / vgc_wait_frames
      ; --- Phase C: random + UI + overlay NDK bodies (all include-guarded) ---
      ; rng.s/overlay.s define FIO_EMIT_ALL_RUNTIME then pull fio.s (+pager.s for
      ; overlay); nui.s pulls vsprite.s + vtext.s (vtext already resident via editui).
      .include "rng.s"                 ; rng_get8/16/32 (-> fio_rng / fio_exec)
      .include "nui.s"                 ; nui_dialog_defaults/show_dialog[_wait]/show_error/wait_key
      .include "overlay.s"             ; overlay_load_fixed/unload/call_init/main/tick (-> pager + fio)

; Generated runtime label hash table (RuntimeLabelBuckets + RUNTIME_LABEL_BUCKET_COUNT)
; backing SYS_ADDR_LOOKUP. Placed in RODATA so it lands in the 16K ROM image.
      .segment "RODATA"
      .include "runtime_labels.inc"
      .segment "CODE"

      .segment "VECTORS"
      ; Module runs under SEI (the loader masks IRQ across the bank swap); hardware
      ; vectors never fire from bank 1. Fill with a safe entry-point stub.
      .word   $C000, $C000, $C000      ; NMI, RESET, IRQ

; system.s — SYSTEM module (MODULE_ID_SYSTEM = $03), the shared system-services
; library. Built once, staged to the XRAM shelf, paged into bank 1 on the first
; lib_call. Header "NL"/$03 at $C000 (lib_module_header); RTS-trick dispatch on
; LIB_FN_ID.

      .include "libabi.inc"
      .include "libmod.inc"
      .include "libsystem.inc"
      .include "nova.inc"              ; VGC_*/TEXTWIN_* register equates + ROMSWAP
      .include "vgc.inc"               ; (guarded) VGC command/constant equates
      .include "rng.inc"               ; RNG_VALUE0..3 (= FIO_RNG*) + rng_get* .globals
      .include "nui.inc"               ; NUI_DIALOG_*/NUI_TITLE*/MSG*/FOOTER* + nui_* .globals
      .include "overlay.inc"           ; OVL_NAMEPTR_*/LOAD*/MAXLEN* + overlay_* .globals

      .segment "ZEROPAGE"
sys_ptr0L:         .res 1
sys_ptr0H:         .res 1
sys_ptr1L:         .res 1
sys_ptr1H:         .res 1
sys_ptr2L:         .res 1
sys_ptr2H:         .res 1

      .segment "CODE"
      lib_module_header MODULE_ID_SYSTEM, LIB_ABI_VERSION, SYS_FN_COUNT

; ===========================================================================
; NDOC documentation (extracted by nova module pack into system.nmod).
; Function ids resolve symbolically from runtime/asm/libsystem.inc.
; ===========================================================================
;@module SYSTEM
;@version 1.0
;@brief Shared timing, random, UI, overlay, lookup, and screen-line services.
;
; Slot $00 is the retired SYS_FN_EDIT slot. The full-screen source editor is now
; MODULE_ID_EDITOR / EDITOR_FN_EDIT.
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
;@fn SYS_CONSOLE_CLEAR_EOL
;@ndk vtext_clear_region
;@arg geom u32 left/top/width/height (ARG0 byte0,1,2,3)
;@ret void
;@status LERR_OK, LERR_SYS_FAIL
;
;@fn SYS_CONSOLE_DELETE_LINE
;@ndk vtext_scroll_up
;@brief Delete the cursor row and scroll the remaining window rows up.
;@arg geom u32 left/top/width/height (ARG0 byte0,1,2,3)
;@ret void
;@status LERR_OK, LERR_SYS_FAIL
;
;@fn SYS_CONSOLE_INSERT_LINE
;@ndk vtext_scroll_down
;@brief Insert a blank cursor row and scroll the remaining window rows down.
;@arg geom u32 left/top/width/height (ARG0 byte0,1,2,3)
;@ret void
;@status LERR_OK, LERR_SYS_FAIL
;
;@fn SYS_CONSOLE_CLEAR_REGION
;@ndk vtext_clear_region
;@brief Clear a caller-defined text window using the active VGC text style.
;@arg geom u32 left/top/width/height (ARG0 byte0,1,2,3)
;@ret void
;@status LERR_OK, LERR_SYS_FAIL
;
;@fn SYS_CONSOLE_PUT_CHAR
;@ndk vtext_put_char
;@brief Write one character without allowing hardware wrap to escape the window.
;@arg geom u32 left/top/width/height (ARG0 byte0,1,2,3)
;@arg ch u8 character (ARG1 byte0)
;@ret void
;@status LERR_OK, LERR_SYS_FAIL
;
;@fn SYS_NUI_SAVE_UNDER
;@ndk nui_save_under
;@brief Save a text-plane rectangle to caller-owned XRAM buffers.
;@arg geom u32 left/top/width/height -> NUI_SAVE_LEFT/TOP/WIDTH/HEIGHT (ARG0 byte0,1,2,3)
;@arg charbuf u32 24-bit XRAM address for saved char plane (ARG1 byte0,1,2)
;@arg colorbuf u32 24-bit XRAM address for saved color plane (ARG2 byte0,1,2)
;@arg attrbuf u32 24-bit XRAM address for saved text-attribute plane (ARG3 byte0,1,2)
;@ret void
;@status LERR_OK, LERR_SYS_FAIL
;
;@fn SYS_NUI_RESTORE_UNDER
;@ndk nui_restore_under
;@brief Restore a text-plane rectangle from caller-owned XRAM buffers.
;@arg geom u32 left/top/width/height -> NUI_SAVE_LEFT/TOP/WIDTH/HEIGHT (ARG0 byte0,1,2,3)
;@arg charbuf u32 24-bit XRAM address for saved char plane (ARG1 byte0,1,2)
;@arg colorbuf u32 24-bit XRAM address for saved color plane (ARG2 byte0,1,2)
;@arg attrbuf u32 24-bit XRAM address for saved text-attribute plane (ARG3 byte0,1,2)
;@ret void
;@status LERR_OK, LERR_SYS_FAIL
;
;@fn SYS_NUI_SAVE_UNDER_FULL
;@ndk nui_save_under_full
;@brief Save text-plane cells and graphics-plane pixels under a modal control.
;@arg config u16 caller-owned config struct pointer (ARG0 byte0,1)
;@ret void
;@status LERR_OK, LERR_SYS_FAIL
;
;@fn SYS_NUI_RESTORE_UNDER_FULL
;@ndk nui_restore_under_full
;@brief Restore text-plane cells and graphics-plane pixels saved by SYS_NUI_SAVE_UNDER_FULL.
;@arg config u16 caller-owned config struct pointer (ARG0 byte0,1)
;@ret void
;@status LERR_OK, LERR_SYS_FAIL
;
;@fn SYS_NUI_PICK_LIST
;@ndk nui_pick_list
;@brief Draw an interactive fixed-width text list picker.
;@arg config u16 caller-owned config struct pointer (ARG0 byte0,1)
;@ret u8 selected row (RESULT byte0)
;@ret u8 command: 0 OK, 1 Cancel (RESULT byte1)
;@status LERR_OK, LERR_SYS_FAIL
;
;@fn SYS_NUI_FILE_PICKER
;@ndk nui_file_picker
;@brief Draw an interactive drive/directory/file picker.
;@arg config u16 caller-owned config struct pointer (ARG0 byte0,1)
;@ret u8 selected filename length (RESULT byte0)
;@ret u8 command: 0 OK, 1 Cancel (RESULT byte1)
;@status LERR_OK, LERR_SYS_FAIL
;
;@fn SYS_NUI_TEXT_INPUT
;@ndk nui_text_input
;@brief Draw an interactive text field with OK/Cancel buttons.
;@arg config u16 caller-owned config struct pointer (ARG0 byte0,1)
;@ret u8 typed text length (RESULT byte0)
;@ret u8 command: 0 OK, 1 Cancel (RESULT byte1)
;@status LERR_OK, LERR_SYS_FAIL
;
;@fn SYS_NUI_SET_STYLE
;@brief Set modal NUI chrome/text colors from caller-provided palette indexes.
;@arg style u32 b0 shadow color index, b1 border color index, b2 panel color index, b3 packed text color bg/fg
;@ret void
;@status LERR_OK
;
;@fn SYS_NUI_DRAIN_KEYS
;@ndk nui_drain_keys
;@brief Drain pending modal keyboard bytes before handing focus to another NUI control.
;@ret void
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
; SYS_FN_COUNT is small (33) so fn*2 cannot exceed 255; an 8-bit asl/tax is safe.
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
      .word   sys_no_fn-1              ; $00 retired SYS_FN_EDIT
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
      .word   sys_nui_save_under-1     ; $13 SYS_NUI_SAVE_UNDER
      .word   sys_nui_restore_under-1  ; $14 SYS_NUI_RESTORE_UNDER
      .word   sys_nui_pick_list-1      ; $15 SYS_NUI_PICK_LIST
      .word   sys_nui_save_under_full-1 ; $16 SYS_NUI_SAVE_UNDER_FULL
      .word   sys_nui_restore_under_full-1 ; $17 SYS_NUI_RESTORE_UNDER_FULL
      .word   sys_nui_set_style-1      ; $18 SYS_NUI_SET_STYLE
      .word   sys_nui_file_picker-1    ; $19 SYS_NUI_FILE_PICKER
      .word   sys_nui_text_input-1     ; $1A SYS_NUI_TEXT_INPUT
      .word   sys_nui_drain_keys-1     ; $1B SYS_NUI_DRAIN_KEYS
      .word   sys_console_clear_eol-1  ; $1C SYS_CONSOLE_CLEAR_EOL
      .word   sys_console_delete_line-1 ; $1D SYS_CONSOLE_DELETE_LINE
      .word   sys_console_insert_line-1 ; $1E SYS_CONSOLE_INSERT_LINE
      .word   sys_console_clear_region-1 ; $1F SYS_CONSOLE_CLEAR_REGION
      .word   sys_console_put_char-1  ; $20 SYS_CONSOLE_PUT_CHAR

sys_no_fn:
      lda     #LERR_NO_FN
      sta     LIB_STATUS
      rts

; Configure a caller-provided VTEXT region. Blank cells mirror the active VGC
; text style so clear and line operations behave like ordinary console output.
sys_console_region:
      LDA   LIB_ARG0
      STA   VTEXT_LEFT
      LDA   LIB_ARG0+1
      STA   VTEXT_TOP
      LDA   LIB_ARG0+2
      STA   VTEXT_WIDTH
      LDA   LIB_ARG0+3
      STA   VTEXT_HEIGHT
      STZ   VTEXT_CURX
      STZ   VTEXT_CURY
      LDA   #VTEXT_FLAG_WRAP | VTEXT_FLAG_SCROLL
      STA   VTEXT_FLAGS
      STZ   VTEXT_SCROLL_HOOKL
      STZ   VTEXT_SCROLL_HOOKH
      JSR   vtext_validate_region
      BNE   @bad
      LDA   VGC_FGCOL
      AND   #$0F
      STA   VTEXT_COLOR
      LDA   VGC_TEXT_BG
      CMP   #VGC_TEXT_BG_TRANSPARENT
      BEQ   @transparent
      AND   #$0F
      ASL
      ASL
      ASL
      ASL
      ORA   VTEXT_COLOR
      STA   VTEXT_COLOR
      STZ   VTEXT_ATTR
      LDA   #VTEXT_OK
      RTS
@transparent:
      LDA   #VTEXT_ATTR_BGTRANS
      STA   VTEXT_ATTR
      LDA   #VTEXT_OK
      RTS
@bad:
      LDA   #VTEXT_ERR
      RTS

; Ensure the physical VGC cursor is inside the configured caller window.
sys_console_cursor_in_region:
      LDA   VGC_CURSX
      CMP   VTEXT_LEFT
      BCC   @bad
      LDA   VTEXT_LEFT
      CLC
      ADC   VTEXT_WIDTH
      STA   sys_ptr0L
      LDA   VGC_CURSX
      CMP   sys_ptr0L
      BCS   @bad
      LDA   VGC_CURSY
      CMP   VTEXT_TOP
      BCC   @bad
      LDA   VTEXT_TOP
      CLC
      ADC   VTEXT_HEIGHT
      STA   sys_ptr0H
      LDA   VGC_CURSY
      CMP   sys_ptr0H
      BCS   @bad
      LDA   #VTEXT_OK
      RTS
@bad:
      LDA   #VTEXT_ERR
      RTS

; --- $1C SYS_CONSOLE_CLEAR_EOL: clear through the window's right edge ---
sys_console_clear_eol:
      LDA   LIB_ARG0
      ORA   LIB_ARG0+1
      BNE   @window
      LDA   LIB_ARG0+2
      CMP   #NOVA_SCREEN_COLS
      BNE   @window
      LDA   LIB_ARG0+3
      CMP   #NOVA_SCREEN_ROWS
      BNE   @window
      JSR   vgc_clear_eol
      LDA   #LERR_OK
      STA   LIB_STATUS
      RTS
@window:
      JSR   sys_console_region
      BNE   :+
      JSR   sys_console_cursor_in_region
      BNE   :+
      LDA   VGC_CURSX
      STA   VTEXT_LEFT
      LDA   sys_ptr0L
      SEC
      SBC   VGC_CURSX
      STA   VTEXT_WIDTH
      LDA   VGC_CURSY
      STA   VTEXT_TOP
      LDA   #1
      STA   VTEXT_HEIGHT
      JSR   vtext_clear_region
:     JMP   sys_finish_status

; Restrict the configured window to the cursor row through its bottom edge.
sys_console_line_region:
      JSR   sys_console_region
      BNE   @done
      JSR   sys_console_cursor_in_region
      BNE   @done
      LDA   VGC_CURSY
      STA   VTEXT_TOP
      LDA   sys_ptr0H
      SEC
      SBC   VGC_CURSY
      STA   VTEXT_HEIGHT
      LDA   #VTEXT_OK
@done:
      RTS

sys_console_delete_line:
      JSR   sys_console_line_region
      BNE   :+
      JSR   vtext_scroll_up
:     JMP   sys_finish_status

sys_console_insert_line:
      JSR   sys_console_line_region
      BNE   :+
      JSR   vtext_scroll_down
:     JMP   sys_finish_status

sys_console_clear_region:
      JSR   sys_console_region
      BNE   :+
      JSR   vtext_clear_region
:     JMP   sys_finish_status

sys_console_put_char:
      JSR   sys_console_region
      BNE   :+
      JSR   sys_console_cursor_in_region
      BNE   :+
      LDA   VGC_CURSX
      SEC
      SBC   VTEXT_LEFT
      STA   VTEXT_CURX
      LDA   VGC_CURSY
      SEC
      SBC   VTEXT_TOP
      STA   VTEXT_CURY
      LDA   LIB_ARG1
      STA   VTEXT_CHAR
      JSR   vtext_put_char
:     JMP   sys_finish_status

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

; --- $1B SYS_NUI_DRAIN_KEYS: drain pending modal keyboard bytes ---
sys_nui_drain_keys:
      JSR   nui_drain_keys
      JMP   sys_finish_status

sys_marshal_nui_save:
      LDA   LIB_ARG0+0
      STA   NUI_SAVE_LEFT
      LDA   LIB_ARG0+1
      STA   NUI_SAVE_TOP
      LDA   LIB_ARG0+2
      STA   NUI_SAVE_WIDTH
      LDA   LIB_ARG0+3
      STA   NUI_SAVE_HEIGHT

      LDA   LIB_ARG1+0
      STA   NUI_SAVE_ADDRL+0
      LDA   LIB_ARG1+1
      STA   NUI_SAVE_ADDRM+0
      LDA   LIB_ARG1+2
      STA   NUI_SAVE_ADDRH+0

      LDA   LIB_ARG2+0
      STA   NUI_SAVE_ADDRL+1
      LDA   LIB_ARG2+1
      STA   NUI_SAVE_ADDRM+1
      LDA   LIB_ARG2+2
      STA   NUI_SAVE_ADDRH+1

      LDA   LIB_ARG3+0
      STA   NUI_SAVE_ADDRL+2
      LDA   LIB_ARG3+1
      STA   NUI_SAVE_ADDRM+2
      LDA   LIB_ARG3+2
      STA   NUI_SAVE_ADDRH+2
      RTS

; --- $13 SYS_NUI_SAVE_UNDER: save text rectangle to caller-owned XRAM buffers ---
sys_nui_save_under:
      JSR   sys_marshal_nui_save
      JSR   nui_save_under
      JMP   sys_finish_status

; --- $14 SYS_NUI_RESTORE_UNDER: restore text rectangle from caller-owned XRAM buffers ---
sys_nui_restore_under:
      JSR   sys_marshal_nui_save
      JSR   nui_restore_under
      JMP   sys_finish_status

; Config layout consumed by SYS_NUI_SAVE_UNDER_FULL / RESTORE_UNDER_FULL:
; +0 left, +1 top, +2 width, +3 height,
; +4 char u24, +7 color u24, +10 textattr u24, +13 gfx u24.
; Text buffers hold width*height bytes. The graphics buffer holds
; (width*4)*(height*4) bytes because NUI geometry is text-cell based.
sys_marshal_nui_save_full:
      LDA   LIB_ARG0+0
      STA   sys_ptr0L
      LDA   LIB_ARG0+1
      STA   sys_ptr0H
      LDY   #$00
      LDA   (sys_ptr0L),Y
      STA   NUI_SAVE_LEFT
      INY
      LDA   (sys_ptr0L),Y
      STA   NUI_SAVE_TOP
      INY
      LDA   (sys_ptr0L),Y
      STA   NUI_SAVE_WIDTH
      INY
      LDA   (sys_ptr0L),Y
      STA   NUI_SAVE_HEIGHT
      INY
      LDA   (sys_ptr0L),Y
      STA   NUI_SAVE_ADDRL+0
      INY
      LDA   (sys_ptr0L),Y
      STA   NUI_SAVE_ADDRM+0
      INY
      LDA   (sys_ptr0L),Y
      STA   NUI_SAVE_ADDRH+0
      INY
      LDA   (sys_ptr0L),Y
      STA   NUI_SAVE_ADDRL+1
      INY
      LDA   (sys_ptr0L),Y
      STA   NUI_SAVE_ADDRM+1
      INY
      LDA   (sys_ptr0L),Y
      STA   NUI_SAVE_ADDRH+1
      INY
      LDA   (sys_ptr0L),Y
      STA   NUI_SAVE_ADDRL+2
      INY
      LDA   (sys_ptr0L),Y
      STA   NUI_SAVE_ADDRM+2
      INY
      LDA   (sys_ptr0L),Y
      STA   NUI_SAVE_ADDRH+2
      INY
      LDA   (sys_ptr0L),Y
      STA   NUI_SAVE_ADDRL+3
      INY
      LDA   (sys_ptr0L),Y
      STA   NUI_SAVE_ADDRM+3
      INY
      LDA   (sys_ptr0L),Y
      STA   NUI_SAVE_ADDRH+3
      RTS

; --- $16 SYS_NUI_SAVE_UNDER_FULL: save text+graphics rectangle to XRAM buffers ---
sys_nui_save_under_full:
      JSR   sys_marshal_nui_save_full
      JSR   nui_save_under_full
      JMP   sys_finish_status

; --- $17 SYS_NUI_RESTORE_UNDER_FULL: restore text+graphics rectangle from XRAM buffers ---
sys_nui_restore_under_full:
      JSR   sys_marshal_nui_save_full
      JSR   nui_restore_under_full
      JMP   sys_finish_status

; --- $18 SYS_NUI_SET_STYLE: ARG0 b0=shadow b1=top/left border b2=panel b3=text bg/fg ---
sys_nui_set_style:
      LDA   LIB_ARG0+0
      STA   NUI_STYLE_SHADOW
      LDA   LIB_ARG0+1
      STA   NUI_STYLE_BORDER
      LDA   LIB_ARG0+2
      STA   NUI_STYLE_PANEL
      LDA   LIB_ARG0+3
      STA   NUI_STYLE_TEXT
      LDA   #$01
      STA   NUI_STYLE_VALID
      LDA   #$00
      JMP   sys_finish_status

; Config layout consumed by SYS_NUI_PICK_LIST:
; +0 title ptr, +2 rows ptr, +4 row width, +5 row count, +6 selected,
; +7 left, +8 top, +9 width, +10 height, +11 footer ptr.
sys_marshal_nui_pick_list:
      LDA   LIB_ARG0+0
      STA   sys_ptr0L
      LDA   LIB_ARG0+1
      STA   sys_ptr0H
      LDY   #$00
      LDA   (sys_ptr0L),Y
      STA   NUI_TITLEL
      INY
      LDA   (sys_ptr0L),Y
      STA   NUI_TITLEH
      INY
      LDA   (sys_ptr0L),Y
      STA   NUI_LIST_ITEMSL
      INY
      LDA   (sys_ptr0L),Y
      STA   NUI_LIST_ITEMSH
      INY
      LDA   (sys_ptr0L),Y
      STA   NUI_LIST_ROW_WIDTH
      INY
      LDA   (sys_ptr0L),Y
      STA   NUI_LIST_ROW_COUNT
      INY
      LDA   (sys_ptr0L),Y
      STA   NUI_LIST_SELECTED
      INY
      LDA   (sys_ptr0L),Y
      STA   NUI_DIALOG_LEFT
      INY
      LDA   (sys_ptr0L),Y
      STA   NUI_DIALOG_TOP
      INY
      LDA   (sys_ptr0L),Y
      STA   NUI_DIALOG_WIDTH
      INY
      LDA   (sys_ptr0L),Y
      STA   NUI_DIALOG_HEIGHT
      INY
      LDA   (sys_ptr0L),Y
      STA   NUI_FOOTERL
      INY
      LDA   (sys_ptr0L),Y
      STA   NUI_FOOTERH
      STZ   NUI_MSGL
      STZ   NUI_MSGH
      RTS

; --- $15 SYS_NUI_PICK_LIST: fixed-width row picker from caller config ---
sys_nui_pick_list:
      JSR   sys_marshal_nui_pick_list
      JSR   nui_pick_list
      CMP   #$00
      BNE   @fail
      LDA   NUI_LIST_SELECTED
      STA   LIB_RESULT+0
      LDA   NUI_RESULT
      STA   LIB_RESULT+1
      STZ   LIB_RESULT+2
      STZ   LIB_RESULT+3
      LDA   #LERR_OK
      STA   LIB_STATUS
      RTS
@fail:
      STZ   LIB_RESULT+0
      STZ   LIB_RESULT+1
      STZ   LIB_RESULT+2
      STZ   LIB_RESULT+3
      LDA   #LERR_SYS_FAIL
      STA   LIB_STATUS
      RTS

; Config layout consumed by SYS_NUI_FILE_PICKER:
; +0 title ptr, +2 output buffer ptr, +4 output max, +5 mode,
;    mode 0=open/select, mode 1=save-as prompt.
; +6 low file type mask, +7 high file type mask, +8 selected, +9..+11 ignored legacy geometry,
; +12 footer ptr, +14 row buffer ptr (24*36 bytes).
sys_marshal_nui_file_picker:
      LDA   LIB_ARG0+0
      STA   sys_ptr0L
      LDA   LIB_ARG0+1
      STA   sys_ptr0H
      LDY   #$00
      LDA   (sys_ptr0L),Y
      STA   NUI_TITLEL
      INY
      LDA   (sys_ptr0L),Y
      STA   NUI_TITLEH
      INY
      LDA   (sys_ptr0L),Y
      STA   NUI_FILE_OUTL
      INY
      LDA   (sys_ptr0L),Y
      STA   NUI_FILE_OUTH
      INY
      LDA   (sys_ptr0L),Y
      STA   NUI_FILE_OUT_MAX
      INY
      LDA   (sys_ptr0L),Y
      STA   NUI_FILE_MODE
      INY
      LDA   (sys_ptr0L),Y
      STA   NUI_FILE_TYPE_MASK
      INY
      LDA   (sys_ptr0L),Y
      STA   NUI_FILE_TYPE_MASKH
      INY
      LDA   (sys_ptr0L),Y
      STA   NUI_LIST_SELECTED
      LDY   #$0C
      LDA   (sys_ptr0L),Y
      STA   NUI_FOOTERL
      INY
      LDA   (sys_ptr0L),Y
      STA   NUI_FOOTERH
      INY
      LDA   (sys_ptr0L),Y
      STA   NUI_FILE_ROWSL
      INY
      LDA   (sys_ptr0L),Y
      STA   NUI_FILE_ROWSH
      STZ   NUI_MSGL
      STZ   NUI_MSGH
      RTS

; --- $19 SYS_NUI_FILE_PICKER: drive/directory/file picker from caller config ---
sys_nui_file_picker:
      JSR   sys_marshal_nui_file_picker
      JSR   nui_file_picker
      CMP   #$00
      BNE   @fail
      LDA   NUI_FILE_OUT_LEN
      STA   LIB_RESULT+0
      LDA   NUI_RESULT
      STA   LIB_RESULT+1
      STZ   LIB_RESULT+2
      STZ   LIB_RESULT+3
      LDA   #LERR_OK
      STA   LIB_STATUS
      RTS
@fail:
      STZ   LIB_RESULT+0
      STZ   LIB_RESULT+1
      STZ   LIB_RESULT+2
      STZ   LIB_RESULT+3
      LDA   #LERR_SYS_FAIL
      STA   LIB_STATUS
      RTS

; Config layout consumed by SYS_NUI_TEXT_INPUT:
; +0 title ptr, +2 label ptr (8 bytes, space-padded), +4 output buffer ptr,
; +6 output max, +7 left, +8 top, +9 width, +10 height, +11 footer ptr.
sys_marshal_nui_text_input:
      LDA   LIB_ARG0+0
      STA   sys_ptr0L
      LDA   LIB_ARG0+1
      STA   sys_ptr0H
      LDY   #$00
      LDA   (sys_ptr0L),Y
      STA   NUI_TITLEL
      INY
      LDA   (sys_ptr0L),Y
      STA   NUI_TITLEH
      INY
      LDA   (sys_ptr0L),Y
      STA   NUI_INPUT_LABELL
      INY
      LDA   (sys_ptr0L),Y
      STA   NUI_INPUT_LABELH
      INY
      LDA   (sys_ptr0L),Y
      STA   NUI_INPUT_OUTL
      INY
      LDA   (sys_ptr0L),Y
      STA   NUI_INPUT_OUTH
      INY
      LDA   (sys_ptr0L),Y
      STA   NUI_INPUT_OUT_MAX
      INY
      LDA   (sys_ptr0L),Y
      STA   NUI_DIALOG_LEFT
      INY
      LDA   (sys_ptr0L),Y
      STA   NUI_DIALOG_TOP
      INY
      LDA   (sys_ptr0L),Y
      STA   NUI_DIALOG_WIDTH
      INY
      LDA   (sys_ptr0L),Y
      STA   NUI_DIALOG_HEIGHT
      INY
      LDA   (sys_ptr0L),Y
      STA   NUI_FOOTERL
      INY
      LDA   (sys_ptr0L),Y
      STA   NUI_FOOTERH
      STZ   NUI_MSGL
      STZ   NUI_MSGH
      RTS

; --- $1A SYS_NUI_TEXT_INPUT: Borland-style text field from caller config ---
sys_nui_text_input:
      JSR   sys_marshal_nui_text_input
      JSR   nui_text_input
      CMP   #$00
      BNE   @fail
      LDA   NUI_INPUT_OUT_LEN
      STA   LIB_RESULT+0
      LDA   NUI_RESULT
      STA   LIB_RESULT+1
      STZ   LIB_RESULT+2
      STZ   LIB_RESULT+3
      LDA   #LERR_OK
      STA   LIB_STATUS
      RTS
@fail:
      STZ   LIB_RESULT+0
      STZ   LIB_RESULT+1
      STZ   LIB_RESULT+2
      STZ   LIB_RESULT+3
      LDA   #LERR_SYS_FAIL
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
; ZP usage: three live indirect pointers use SYSTEM-owned module-ZP scratch.
; The non-pointer scratch (len/ch/hash) lives in module BSS (no ZP needed).
;   addr_entryL/H  = entry-base ptr
;   addr_nameL/H   = entry name-bytes ptr
;   addr_argL/H    = input name ptr, seeded from ARG0
addr_entryL = sys_ptr0L
addr_entryH = sys_ptr0H
addr_nameL  = sys_ptr1L
addr_nameH  = sys_ptr1H
addr_argL   = sys_ptr2L
addr_argH   = sys_ptr2H

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
; ZP usage:
;   srl_winL/H  = the $A200 row-base window pointer
;   srl_dstL/H  = the caller's destination buffer ptr
; Non-pointer scratch (maxlen / col / trimmed length) lives in module BSS.
;   In:  ARG0 b0/b1 = destination buffer ptr   ARG0 b2 = max length
;   Out: RESULT b0 = trimmed line length; buffer filled (no trailing NUL);
;        LIB_STATUS = LERR_OK
; ===========================================================================
srl_winL = sys_ptr0L
srl_winH = sys_ptr0H
srl_dstL = sys_ptr1L
srl_dstH = sys_ptr1H

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

      ; --- buffer-based line reader. Echo each key via VGC_CHAROUT (the SAME path
      ; PRINT uses, so echo is ring-scroll-consistent and visible at the cursor) and
      ; accumulate keys into the caller's buffer; on ENTER, return the buffer. This
      ; replaces the old C64-style read-the-screen-row reader, which ingested a
      ; wrapped program's leftover text and echoed via the physical $A200 window
      ; (diverging from the hardware ring-scroll display -> typing looked invisible).
      STZ   srl_dsti                   ; chars accumulated so far
@poll:
      LDA   VGC_CHARIN
      BEQ   @poll                      ; no key -> spin
      CMP   #$0D
      BEQ   @enter                     ; ENTER -> finish
      CMP   #$08
      BEQ   @backspace
      CMP   #$7F
      BEQ   @backspace
      CMP   #$20
      BCC   @poll                      ; other control byte -> ignore
      ; printable: append to the buffer (if room) + echo
      LDX   srl_dsti
      CPX   srl_maxlen
      BCS   @poll                      ; buffer full -> drop the char
      LDY   srl_dsti
      STA   (srl_dstL),Y               ; buffer[len] = char
      INC   srl_dsti
      STA   VGC_CHAROUT                ; echo: visible; HW advances cursor + wrap/scroll
      BRA   @poll
@backspace:
      LDA   srl_dsti
      BEQ   @poll                      ; nothing typed -> ignore
      DEC   srl_dsti
      LDA   #$08
      STA   VGC_CHAROUT                ; erase-left + move cursor back (HW)
      BRA   @poll
@enter:
      LDA   #$0D
      STA   VGC_CHAROUT                ; CR -> col 0
      LDA   #$0A
      STA   VGC_CHAROUT                ; LF -> next row / scroll at the bottom
      LDA   srl_dsti
      STA   LIB_RESULT+0               ; submitted line length
      STZ   LIB_RESULT+1
      STZ   LIB_RESULT+2
      STZ   LIB_RESULT+3
      STZ   VGC_CURSEN                 ; cursor off
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

; srl_read_cell — read ONE cell through the $A200 screen window, accounting for
; the plane-BRAM read latency. On the FPGA the window read is a latency-pipelined
; port (the address propagates address-reg -> port read -> latch over ~3 pixel
; cycles, exactly like the VDATA port), so a single LDA (srl_winL),Y latches the
; data for the PREVIOUSLY-read window cell, not this one. Read sequentially that
; shifts the whole line right by one with a stale leading byte -> garbled input
; (intermittent Syntax Error when the stale byte isn't a space). A priming read
; presents this cell's address into the pipeline; the second read, issued many
; CPU cycles later, returns this cell. The emulator's window is zero-latency, so
; the prime read just returns the same byte there (idempotent) and the existing
; SystemModuleTests stay green.
;   In:  Y = column, srl_winL/H = row base ($A200 + row*80).
;   Out: A = the cell. X and Y are preserved.
srl_read_cell:
      LDA   (srl_winL),Y               ; prime: present the address to the read pipeline
      LDA   (srl_winL),Y               ; settled: returns THIS cell on HW
      RTS

; srl_dst_adv — advance the multi-row read destination: bump the 16-bit dest
; pointer srl_dstL/H by one and increment the byte counter srl_dsti. Clobbers A.
srl_dst_adv:
      INC   srl_dstL
      BNE   :+
      INC   srl_dstH
:     INC   srl_dsti
      RTS

      .segment "BSS"
srl_maxlen:       .res 1               ; destination buffer capacity (ARG0 b2)
srl_trim:         .res 1               ; trimmed line length (last non-space col + 1)
srl_startrow:     .res 1               ; multi-row read: logical-line start row
srl_cur:          .res 1               ; multi-row read: current physical row
srl_dsti:         .res 1               ; multi-row read: bytes written so far
srl_pend:         .res 1               ; multi-row read: deferred (pending) spaces
srb_row:          .res 1               ; screen_row_base: saved row
srb_tH:           .res 1               ; screen_row_base: row<<4 high byte
; sys_addr_lookup non-pointer scratch (the 3 live indirect pointers are in
; SYSTEM-owned ZP; only these counters/temps need BSS storage).
sal_len:          .res 1               ; input name length (ARG0 b2)
sal_entrylen:     .res 1               ; current bucket entry's name length
sal_ch:           .res 1               ; uppercased compare byte / hash temp
sal_hash:         .res 1               ; running x33 hash accumulator
      .segment "CODE"

; ===========================================================================
; Shared system-service bodies (all include-guarded).
; ===========================================================================
      .include "vgc.inc"                 ; NDK frame-timing: vgc_vsync / vgc_wait_frames
      ; rng.s/overlay.s define FIO_EMIT_ALL_RUNTIME then pull fio.s (+pager.s for
      ; overlay); nui.s pulls vtext.s + vsprite.s.
      .include "rng.inc"                 ; rng_get8/16/32 (-> fio_rng / fio_exec)
      .include "nui.inc"                 ; nui_dialog_defaults/show_dialog[_wait]/show_error/wait_key
      .include "overlay.inc"             ; overlay_load_fixed/unload/call_init/main/tick (-> pager + fio)

; Generated runtime label hash table (RuntimeLabelBuckets + RUNTIME_LABEL_BUCKET_COUNT)
; backing SYS_ADDR_LOOKUP. Placed in RODATA so it lands in the 16K ROM image.
      .segment "RODATA"
      .include "runtime_labels.inc"
      .segment "CODE"

      .segment "VECTORS"
      ; Module runs under SEI (the loader masks IRQ across the bank swap); hardware
      ; vectors never fire from bank 1. Fill with a safe entry-point stub.
      .word   $C000, $C000, $C000      ; NMI, RESET, IRQ

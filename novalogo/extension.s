; extension.s — NovaLogo extension ROM
; Loaded at $C000 when swapped via REG_ROMSWAP = ROMSWAP_EXTENSION ($04).
;
; Entry point at $C000: reads EXT_CMD from zero page and dispatches.
; Handlers receive pre-evaluated arguments in EXT_ARG0..2 and return
; results in EXT_RESULT. No callbacks into the base ROM interpreter.

      .include "nova.inc"
NOVALOGO_TURTLE_STATE_EXTERNAL = 1   ; mailbox-ABI only; turtle state lives in the GRAPHICS module
      .include "ext_iface.inc"
      .include "vgc.inc"
      .include "editbuf.inc"

; Editor display colors. The shared editor no longer dictates the global
; background/border/foreground — this runtime owns them. Change these to
; restyle the NovaLogo editor. (Palette mode 0: 0=black, 1=white, 15=grey-light.)
EDITOR_BGCOL      = $00        ; black background
EDITOR_BORDER     = $00        ; black border
EDITOR_FGCOL      = $01        ; white text (cursor is an inverted cell -> white)

      .segment "CODE"

; =====================================================================
; Extension entry point — called via JSR $C000 from RAM trampoline
; =====================================================================
ext_entry:
      CLD                       ; all extension math is binary, never BCD
      LDA   EXT_CMD
      ASL                       ; * 2 for word-sized table
      TAX
      LDA   ext_dispatch+1,X   ; high byte of handler-1
      PHA
      LDA   ext_dispatch,X     ; low byte of handler-1
      PHA
      RTS                       ; dispatch via RTS trick

; --- Dispatch table (entries are handler_address - 1) ---
ext_dispatch:
      .word ext_unsupported-1   ; cmd $00: reserved
      .word ext_test-1          ; cmd $01: EXT.TEST
      .word ext_unsupported-1   ; cmd $02
      .word ext_unsupported-1   ; cmd $03
      .word ext_unsupported-1   ; cmd $04
      .word ext_unsupported-1   ; cmd $05
      .word ext_unsupported-1   ; cmd $06
      .word ext_unsupported-1   ; cmd $07
      .word ext_unsupported-1   ; cmd $08
      .word ext_unsupported-1   ; cmd $09
      .word ext_unsupported-1   ; cmd $0A
      .word ext_unsupported-1   ; cmd $0B
      .word ext_unsupported-1   ; cmd $0C
      .word ext_unsupported-1   ; cmd $0D
      .word ext_unsupported-1   ; cmd $0E
      .word ext_unsupported-1   ; cmd $0F
      ; cmds $10-$44: turtle, VGC graphics + sprite commands. These now live in
      ; the GRAPHICS module; the FOUNDATION routes them there (ext_cmd_table
      ; MODULE_ID_TURTLE/GRAPHICS/GFXADAPTER), never via this legacy trampoline.
      ; The drivers were deleted from this extension (4c.2-3-iii); these slots are
      ; unreachable at runtime and point at ext_unsupported so the table stays
      ; dense and every symbol resolves.
      .word ext_unsupported-1   ; cmd $10: FD            (module)
      .word ext_unsupported-1   ; cmd $11: BK            (module)
      .word ext_unsupported-1   ; cmd $12: RT            (module)
      .word ext_unsupported-1   ; cmd $13: LT            (module)
      .word ext_unsupported-1   ; cmd $14: CS            (module)
      .word ext_unsupported-1   ; cmd $15: PU            (module)
      .word ext_unsupported-1   ; cmd $16: PD            (module)
      .word ext_unsupported-1   ; cmd $17: ST            (module)
      .word ext_unsupported-1   ; cmd $18: HT            (module)
      .word ext_unsupported-1   ; cmd $19: HOME          (module)
      .word ext_unsupported-1   ; cmd $1A: TEXTSCREEN    (module)
      .word ext_unsupported-1   ; cmd $1B: SPLITSCREEN   (module)
      .word ext_unsupported-1   ; cmd $1C: FULLSCREEN    (module)
      .word ext_unsupported-1   ; cmd $1D: SETXY         (module)
      .word ext_unsupported-1   ; cmd $1E: SETX          (module)
      .word ext_unsupported-1   ; cmd $1F: SETY          (module)
      .word ext_unsupported-1   ; cmd $20: SETHEADING    (module)
      .word ext_unsupported-1   ; cmd $21: XCOR          (module)
      .word ext_unsupported-1   ; cmd $22: YCOR          (module)
      .word ext_unsupported-1   ; cmd $23: HEADING       (module)
      .word ext_unsupported-1   ; cmd $24: PENDOWN?      (module)
      .word ext_unsupported-1   ; cmd $25: SHOWN?        (module)
      .word ext_unsupported-1   ; cmd $26: SETPC         (module)
      .word ext_unsupported-1   ; cmd $27: SETBG         (module)
      .word ext_unsupported-1   ; cmd $28: TOWARDS       (module)
      .word ext_unsupported-1   ; cmd $29: SETPOS        (module)
      .word ext_unsupported-1   ; cmd $2A
      .word ext_unsupported-1   ; cmd $2B
      .word ext_unsupported-1   ; cmd $2C
      .word ext_unsupported-1   ; cmd $2D
      .word ext_unsupported-1   ; cmd $2E
      .word ext_unsupported-1   ; cmd $2F
      .word ext_unsupported-1   ; cmd $30: SETCOLOR      (module)
      .word ext_unsupported-1   ; cmd $31: PLOT          (module)
      .word ext_unsupported-1   ; cmd $32: UNPLOT        (module)
      .word ext_unsupported-1   ; cmd $33: LINE          (module)
      .word ext_unsupported-1   ; cmd $34: CIRCLE        (module)
      .word ext_unsupported-1   ; cmd $35: RECT          (module)
      .word ext_unsupported-1   ; cmd $36: FILL          (module)
      .word ext_unsupported-1   ; cmd $37: PAINT         (module)
      .word ext_unsupported-1   ; cmd $38
      .word ext_unsupported-1   ; cmd $39
      .word ext_unsupported-1   ; cmd $3A
      .word ext_unsupported-1   ; cmd $3B
      .word ext_unsupported-1   ; cmd $3C
      .word ext_unsupported-1   ; cmd $3D
      .word ext_unsupported-1   ; cmd $3E
      .word ext_unsupported-1   ; cmd $3F
      .word ext_unsupported-1   ; cmd $40: SPRITE        (module)
      .word ext_unsupported-1   ; cmd $41: SPRITEPOS     (module)
      .word ext_unsupported-1   ; cmd $42: SPRITEON      (module)
      .word ext_unsupported-1   ; cmd $43: SPRITEOFF     (module)
      .word ext_unsupported-1   ; cmd $44: SPRITECOLLISION? (module)
      .word ext_tone-1          ; cmd $45: TONE
      .word ext_noise-1         ; cmd $46: NOISE
      .word ext_volume-1        ; cmd $47: VOLUME
      .word ext_wait-1          ; cmd $48: WAIT
      .word ext_waitvbl-1       ; cmd $49: WAITVBL
      .word ext_timer-1         ; cmd $4A: TIMER
      .word ext_edit-1          ; cmd $4B: EDIT (shared editor)

; =====================================================================
; ext_unsupported — unknown command, just return
; =====================================================================
ext_unsupported:
      RTS

; =====================================================================
; ext_test — test command: returns arg0 + 1
; =====================================================================
ext_test:
      CLC
      LDA   EXT_ARG0_LO
      ADC   #1
      STA   EXT_RESULT_LO
      LDA   EXT_ARG0_HI
      ADC   #0
      STA   EXT_RESULT_HI
      LDA   EXT_ARG0_FRAC
      STA   EXT_RESULT_FRAC
      STZ   EXT_RESULT_TYPE    ; VAL_NUMBER
      RTS

; =====================================================================
; ext_edit — run the shared EDITUI/EDITBUF editor on a RAM text buffer.
;   The base ROM passes a buffer ptr/len/cap and a title in the mailbox
;   (see EXT_CMD_EDIT in ext_iface.inc). We snapshot the VGC display state,
;   switch to full-screen text (the editor owns the whole screen), run the
;   modal editor, then restore the display EXACTLY — graphics/turtle are
;   never cleared. A nonzero RESULT_HI means the user asked to save.
; =====================================================================
ext_edit:
      ; --- snapshot the display registers editui_init will clobber ---
      LDA   VGC_MODE
      STA   ee_saved_mode
      LDA   VGC_PALETTE
      STA   ee_saved_palette
      LDA   VGC_BGCOL
      STA   ee_saved_bgcol
      LDA   VGC_BORDER
      STA   ee_saved_border
      LDA   VGC_FGCOL
      STA   ee_saved_fgcol
      LDA   VGC_CURSX
      STA   ee_saved_cursx
      LDA   VGC_CURSY
      STA   ee_saved_cursy
      LDA   VGC_CURSEN
      STA   ee_saved_cursen

      ; --- the editor owns the whole screen: copper off, full text window ---
      JSR   copper_off
      STZ   TEXTWIN_LEFT
      STZ   TEXTWIN_TOP
      LDA   #80
      STA   TEXTWIN_WIDTH
      LDA   #25
      STA   TEXTWIN_HEIGHT

      ; --- runtime-chosen editor colors (the engine no longer sets these) ---
      LDA   #EDITOR_BGCOL
      STA   VGC_BGCOL
      LDA   #EDITOR_BORDER
      STA   VGC_BORDER
      LDA   #EDITOR_FGCOL
      STA   VGC_FGCOL

      ; --- editbuf config from the mailbox ---
      LDA   EXT_ARG0_LO
      STA   EDITBUF_BUFL
      LDA   EXT_ARG0_HI
      STA   EDITBUF_BUFH
      LDA   EXT_ARG2_LO
      STA   EDITBUF_CAPL
      LDA   EXT_ARG2_HI
      STA   EDITBUF_CAPH
      LDA   EXT_ARG1_LO
      STA   EDITBUF_LENL
      LDA   EXT_ARG1_HI
      STA   EDITBUF_LENH
      LDA   EXT_ARG3_LO
      STA   EDITBUF_TITLEL
      LDA   EXT_ARG3_HI
      STA   EDITBUF_TITLEH
      STZ   EDITBUF_STATUSL
      STZ   EDITBUF_STATUSH

      ; --- install hooks explicitly (BSS is not zeroed between sessions) ---
      LDA   #<ext_edit_save_hook
      STA   EDITBUF_SAVE_VECL
      LDA   #>ext_edit_save_hook
      STA   EDITBUF_SAVE_VECH
      STZ   EDITBUF_INDENT_VECL    ; 0 => editbuf installs its no-op default
      STZ   EDITBUF_INDENT_VECH
      STZ   EDITBUF_HILITE_VECL
      STZ   EDITBUF_HILITE_VECH
      STZ   EDITBUF_MENU_VECL      ; keep EDITUI's default menus for now
      STZ   EDITBUF_MENU_VECH

      ; --- run the modal editor ---
      STZ   ee_saved_flag
      JSR   editbuf_reset_state
      ; Place the cursor where the host asked (start of the body line), instead
      ; of the default offset 0. Cursor offset rides in the ARG FRAC bytes.
      LDA   EXT_ARG2_FRAC
      STA   EDITBUF_CURL
      LDA   EXT_ARG3_FRAC
      STA   EDITBUF_CURH
      JSR   editbuf_run            ; A = exit reason

      ; --- publish results to the mailbox ---
      STA   EXT_RESULT_LO          ; editbuf exit reason
      LDA   ee_saved_flag
      STA   EXT_RESULT_HI          ; nonzero => save requested
      LDA   EDITBUF_LENL
      STA   EXT_ARG1_LO            ; final length back to the host
      LDA   EDITBUF_LENH
      STA   EXT_ARG1_HI

      ; --- restore the display exactly (never clear graphics) ---
      ; Restore the host's palette + colors FIRST: the form-feed clear below
      ; fills color RAM with the *current* text color, so the host's colors
      ; must be back in place before we clear, or the editor's panel color
      ; leaks into the restored text area.
      LDA   ee_saved_palette
      STA   VGC_PALETTE
      LDA   ee_saved_bgcol
      STA   VGC_BGCOL
      LDA   ee_saved_border
      STA   VGC_BORDER
      LDA   ee_saved_fgcol
      STA   VGC_FGCOL

      ; Plain full-text restore: clear the editor chrome and restore the saved
      ; display registers. The turtle now lives in the GRAPHICS module, so the
      ; extension no longer owns the split-screen rebuild — when graphics were
      ; active, restoring ee_saved_mode brings the gfx layer back, and the
      ; foundation re-applies the split text window on the next turtle op
      ; (logo_turtle_textwin, keyed off the module's TURTLE_GFX_VISIBLE flag).
      STZ   TEXTWIN_LEFT
      STZ   TEXTWIN_TOP
      LDA   #80
      STA   TEXTWIN_WIDTH
      LDA   #25
      STA   TEXTWIN_HEIGHT
      LDA   #$0C                   ; form feed: clear text plane only
      STA   VGC_CHAROUT
      LDA   ee_saved_mode
      STA   VGC_MODE
      LDA   ee_saved_cursx
      STA   VGC_CURSX
      LDA   ee_saved_cursy
      STA   VGC_CURSY
      LDA   ee_saved_cursen
      STA   VGC_CURSEN
      RTS

; ext_edit_save_hook — SAVE hook while the editor runs. For now it just records
; that the user asked to save and reports success; the base ROM builds the
; procedure record from the final buffer when the editor exits. (Cross-bank
; live validation is the next increment.)
ext_edit_save_hook:
      LDA   #1
      STA   ee_saved_flag
      LDA   #EDITBUF_SAVE_OK
      RTS

      .segment "BSS"
ee_saved_mode:    .res 1
ee_saved_palette: .res 1
ee_saved_bgcol:   .res 1
ee_saved_border:  .res 1
ee_saved_fgcol:   .res 1
ee_saved_cursx:   .res 1
ee_saved_cursy:   .res 1
ee_saved_cursen:  .res 1
ee_saved_flag:    .res 1          ; nonzero once the SAVE hook fires
      .segment "CODE"

; =====================================================================
; ext_tone — TONE freq dur: play a tone on SID voice 0
;   ARG0 = frequency (16-bit SID freq word), ARG1 = duration (frames)
; =====================================================================
ext_tone:
      ; Set SID master volume to max
      LDA   #$0F
      STA   SID_BASE + $18         ; $D418 filter/volume, bits 0-3 = volume
      ; Set frequency
      LDA   EXT_ARG0_LO
      STA   SID_BASE + $00         ; freq lo
      LDA   EXT_ARG0_HI
      STA   SID_BASE + $01         ; freq hi
      ; Set ADSR: attack=0, decay=9, sustain=$A, release=0
      LDA   #$09
      STA   SID_BASE + $05         ; attack/decay
      LDA   #$A0
      STA   SID_BASE + $06         ; sustain/release
      ; Gate on with sawtooth waveform
      LDA   #$21                   ; sawtooth ($20) + gate ($01)
      STA   SID_BASE + $04         ; control register
      ; Wait duration frames
      LDA   EXT_ARG1_LO
      JSR   wait_frames
      ; Gate off
      LDA   #$20                   ; sawtooth, no gate
      STA   SID_BASE + $04
      RTS

; =====================================================================
; ext_noise — NOISE dur: noise burst on SID voice 0
;   ARG0 = duration (frames)
; =====================================================================
ext_noise:
      ; Set SID master volume to max
      LDA   #$0F
      STA   SID_BASE + $18
      ; Set a mid-range frequency for noise
      LDA   #$00
      STA   SID_BASE + $00
      LDA   #$20
      STA   SID_BASE + $01
      ; ADSR: fast attack, medium sustain
      LDA   #$09
      STA   SID_BASE + $05
      LDA   #$A0
      STA   SID_BASE + $06
      ; Gate on with noise waveform
      LDA   #$81                   ; noise ($80) + gate ($01)
      STA   SID_BASE + $04
      ; Wait duration frames
      LDA   EXT_ARG0_LO
      JSR   wait_frames
      ; Gate off
      LDA   #$80                   ; noise, no gate
      STA   SID_BASE + $04
      RTS

; =====================================================================
; ext_volume — VOLUME vol: set SID master volume (0-15)
;   ARG0 = volume
; =====================================================================
ext_volume:
      LDA   EXT_ARG0_LO
      AND   #$0F                   ; clamp to 0-15
      STA   SID_BASE + $18
      RTS

; =====================================================================
; ext_wait — WAIT n: wait N video frames
;   ARG0 = count
; =====================================================================
ext_wait:
      LDA   EXT_ARG0_LO
      JSR   wait_frames
      RTS

; =====================================================================
; ext_waitvbl — WAITVBL: wait for next vertical blank (1 frame)
; =====================================================================
ext_waitvbl:
      LDA   VGC_FRAME
@w:   CMP   VGC_FRAME
      BEQ   @w
      RTS

; =====================================================================
; ext_timer — TIMER: return current frame counter (8-bit)
; =====================================================================
ext_timer:
      LDA   VGC_FRAME
      STA   EXT_RESULT_LO
      STZ   EXT_RESULT_HI
      STZ   EXT_RESULT_FRAC
      STZ   EXT_RESULT_TYPE
      RTS

; =====================================================================
; wait_frames — wait A video frames. Reused by TONE, NOISE, WAIT.
; =====================================================================
wait_frames:
      TAX
      BEQ   @done
      LDA   VGC_FRAME
@wait:
      CMP   VGC_FRAME
      BEQ   @wait
      LDA   VGC_FRAME
      DEX
      BNE   @wait
@done:
      RTS

      .include "copper.s"
      ; Shared text editor (pulls vtext.s + blitter.s, all include-guarded).
      ; Its BSS lands in $9800-$9BFF, ZP in $A3+.
      .include "editui.s"
      .include "editbuf.s"

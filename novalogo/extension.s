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
      .word ext_unsupported-1   ; cmd $4B: EDIT (moved to SYSTEM module $03, SYS_FN_EDIT)

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

      ; The shared text editor moved out of this extension ROM into the SYSTEM
      ; module (modules/system/system.s, MODULE_ID_SYSTEM=$03). Logo's TO/EDIT now
      ; routes through lib_call(SYSTEM, SYS_FN_EDIT) — see novalogo/procedures.s.

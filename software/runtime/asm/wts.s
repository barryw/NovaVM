; Shared Nova wavetable-synth (WTS) note helpers — async + voice-tracked.
;
; Mirrors the SID one-shot model (audio_*_async): allocate a FREE wavetable voice
; (one whose frame countdown is 0), note it on, and return immediately. Call
; wts_tick once per video frame to count the note down and note it off when it
; expires. Nothing here ever blocks, and a still-sounding voice is never stolen
; unless all eight are busy.
;
; State lives in free scratch RAM above the SID ($BB8D) and music-meta ($BBCD)
; regions. WTS registers + NVR0 come from nova.inc (via wts.inc).

.include "wts.inc"

.ifndef WTS_IMPLEMENTATION_INCLUDED
WTS_IMPLEMENTATION_INCLUDED = 1

WTS_VOICE_DUR   = $BBD0        ; 8 per-voice frame countdowns (0 = idle)
WTS_NEXT_VOICE  = $BBD8        ; round-robin steal cursor
WTS_INIT_FLAG   = $BBD9
WTS_TMP_NOTE    = $BBDA
WTS_TMP_DUR     = $BBDB
WTS_TMP_VEL     = $BBDC
WTS_INIT_MAGIC  = $A5

      .segment "CODE"

      .export wts_init
      .export wts_note_async
      .export wts_note_off
      .export wts_tick

; @label WTS.INIT
; @kind routine
; @symbol wts_init
; @summary Initialise the wavetable synth: master volume up, every voice idle,
;          instrument 0, full per-voice volume. Idempotent (guarded by a flag).
; @out A: 0
wts_init:
      LDA   WTS_INIT_FLAG
      CMP   #WTS_INIT_MAGIC
      BEQ   @done
      LDA   #$FF
      STA   WTS_MASTER_VOLUME
      STZ   WTS_NEXT_VOICE
      LDX   #$07
@each:
      STZ   WTS_VOICE_DUR,X
      JSR   wts_voice_ptr             ; NVR0 = voice X register base
      LDA   #$00
      LDY   #WTS_VOICE_NOTE
      STA   (NVR0L),Y                 ; note off
      LDY   #WTS_VOICE_INSTRUMENT
      STA   (NVR0L),Y                 ; instrument 0 (GM program 0)
      LDA   #127
      LDY   #WTS_VOICE_VOLUME
      STA   (NVR0L),Y                 ; full per-voice volume
      DEX
      BPL   @each
      LDA   #WTS_INIT_MAGIC
      STA   WTS_INIT_FLAG
@done:
      LDA   #$00
      RTS

; @label WTS.NOTE_ASYNC
; @kind routine
; @symbol wts_note_async
; @summary Fire-and-forget wavetable note: pick a free voice, note it on, and
;          auto-release after X frames. Returns immediately; call wts_tick each
;          frame. Only steals a busy voice when all eight are in use.
; @in A: MIDI note
; @in X: Duration in 60 Hz frames
; @in Y: Velocity (0-127)
; @out A: 0
; @out X: Voice used (0-7)
wts_note_async:
      STA   WTS_TMP_NOTE
      STX   WTS_TMP_DUR
      STY   WTS_TMP_VEL
      JSR   wts_init
      ; prefer an idle voice (countdown 0); else steal round-robin.
      LDX   #$00
@find:
      LDA   WTS_VOICE_DUR,X
      BEQ   @have
      INX
      CPX   #$08
      BNE   @find
      LDX   WTS_NEXT_VOICE
      CPX   #$08
      BCC   @adv
      LDX   #$00
@adv:
      TXA
      INC   A
      CMP   #$08
      BCC   @stn
      LDA   #$00
@stn:
      STA   WTS_NEXT_VOICE
@have:
      JSR   wts_voice_ptr             ; NVR0 = voice X base (X preserved)
      LDA   WTS_TMP_VEL
      LDY   #WTS_VOICE_VELOCITY
      STA   (NVR0L),Y
      LDA   WTS_TMP_NOTE
      LDY   #WTS_VOICE_NOTE
      STA   (NVR0L),Y                 ; note-on
      LDA   WTS_TMP_DUR
      STA   WTS_VOICE_DUR,X           ; mark the voice busy for that many frames
      LDA   #$00
      RTS

; @label WTS.NOTE_OFF
; @kind routine
; @symbol wts_note_off
; @summary Note-off one wavetable voice immediately and free it.
; @in X: Voice (0-7)
; @out A: 0
wts_note_off:
      JSR   wts_voice_ptr
      LDA   #$00
      LDY   #WTS_VOICE_NOTE
      STA   (NVR0L),Y
      STZ   WTS_VOICE_DUR,X
      LDA   #$00
      RTS

; @label WTS.TICK
; @kind routine
; @symbol wts_tick
; @summary Advance every async wavetable note by one frame and note-off any that
;          reach zero. Call once per video frame. Never blocks.
; @out A: 0
wts_tick:
      LDA   WTS_INIT_FLAG
      CMP   #WTS_INIT_MAGIC
      BNE   @done
      LDX   #$07
@loop:
      LDA   WTS_VOICE_DUR,X
      BEQ   @next
      DEC   WTS_VOICE_DUR,X
      BNE   @next
      JSR   wts_voice_ptr             ; expired -> note off
      LDA   #$00
      LDY   #WTS_VOICE_NOTE
      STA   (NVR0L),Y
@next:
      DEX
      BPL   @loop
@done:
      LDA   #$00
      RTS

; internal: X = voice (0-7) -> NVR0 = WTS_VOICE_BASE + X*8. Preserves X.
wts_voice_ptr:
      TXA
      ASL   A
      ASL   A
      ASL   A                         ; X * 8 (0..56, no page cross from $40)
      CLC
      ADC   #<WTS_VOICE_BASE
      STA   NVR0L
      LDA   #>WTS_VOICE_BASE
      ADC   #$00
      STA   NVR0H
      RTS

.endif

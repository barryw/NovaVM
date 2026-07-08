; Shared Nova wavetable-synth (WTS) note helpers — async + voice-tracked.
;
; Two note models over the 8 WTS voices ($A140-$A17F), both allocating a FREE
; voice (one whose countdown is 0) and only stealing when all eight are busy:
;
;   wts_note_async  fire-and-forget: note-on + auto-release after N frames. Call
;                   wts_tick once per frame to count it down. Never blocks.
;   wts_note_on     sustained: note-on that HOLDS until wts_note_off (a held key).
;                   wts_tick leaves it alone.
;   wts_note_off    immediate note-off + free the voice.
;
; A voice's countdown byte encodes its state: 0 = idle, 1..254 = timed (counting
; down), $FF = held (sustained; wts_tick skips it).
;
; State lives in free scratch RAM above the SID ($BB8D) and music-meta ($BBCD)
; regions. WTS registers + NVR0 come from nova.inc (via wts.inc).

.include "wts.inc"

.ifndef WTS_IMPLEMENTATION_INCLUDED
WTS_IMPLEMENTATION_INCLUDED = 1

WTS_VOICE_DUR   = $BBD0        ; 8 per-voice countdowns (0 idle, 1-254 timed, $FF held)
WTS_NEXT_VOICE  = $BBD8        ; round-robin steal cursor
WTS_INIT_FLAG   = $BBD9
WTS_TMP_NOTE    = $BBDA
WTS_TMP_DUR     = $BBDB
WTS_TMP_VEL     = $BBDC
WTS_INIT_MAGIC  = $A5
WTS_HELD        = $FF          ; sustained sentinel in WTS_VOICE_DUR

      .segment "CODE"

      .export wts_init
      .export wts_note_on
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

; @label WTS.NOTE_ON
; @kind routine
; @symbol wts_note_on
; @summary Sustained wavetable note-on: pick a free voice and note it on, HELD
;          until wts_note_off (wts_tick never releases it). For held keys — use
;          wts_note_async for timed one-shots. Only steals when all eight are busy.
; @in A: MIDI note
; @in Y: Velocity (0-127)
; @out A: 0
; @out X: Voice used (0-7)
wts_note_on:
      STA   WTS_TMP_NOTE
      STY   WTS_TMP_VEL
      JSR   wts_init
      JSR   wts_alloc_voice           ; X = chosen voice
      JSR   wts_key_on                ; velocity + note-on
      LDA   #WTS_HELD
      STA   WTS_VOICE_DUR,X           ; held: wts_tick won't auto-release it
      LDA   #$00
      RTS

; @label WTS.NOTE_ASYNC
; @kind routine
; @symbol wts_note_async
; @summary Fire-and-forget wavetable note: pick a free voice, note it on, and
;          auto-release after X frames. Returns immediately; call wts_tick each
;          frame. Only steals a busy voice when all eight are in use.
; @in A: MIDI note
; @in X: Duration in 60 Hz frames (1-254)
; @in Y: Velocity (0-127)
; @out A: 0
; @out X: Voice used (0-7)
wts_note_async:
      STA   WTS_TMP_NOTE
      STX   WTS_TMP_DUR
      STY   WTS_TMP_VEL
      JSR   wts_init
      JSR   wts_alloc_voice           ; X = chosen voice
      JSR   wts_key_on                ; velocity + note-on
      LDA   WTS_TMP_DUR
      STA   WTS_VOICE_DUR,X           ; count this voice down for that many frames
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
; @summary Advance every timed wavetable note by one frame and note-off any that
;          reach zero. Held notes (wts_note_on) are skipped. Call once per frame.
; @symbol wts_tick
; @out A: 0
wts_tick:
      LDA   WTS_INIT_FLAG
      CMP   #WTS_INIT_MAGIC
      BNE   @done
      LDX   #$07
@loop:
      LDA   WTS_VOICE_DUR,X
      BEQ   @next                     ; idle
      CMP   #WTS_HELD
      BEQ   @next                     ; held (sustained) -> never auto-release
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

; internal: velocity (WTS_TMP_VEL) + note-on (WTS_TMP_NOTE) on voice X. Preserves X.
wts_key_on:
      JSR   wts_voice_ptr
      LDA   WTS_TMP_VEL
      LDY   #WTS_VOICE_VELOCITY
      STA   (NVR0L),Y
      LDA   WTS_TMP_NOTE
      LDY   #WTS_VOICE_NOTE
      STA   (NVR0L),Y                 ; note-on
      RTS

; internal: X = an idle voice (countdown 0); else steal the round-robin voice and
; advance the cursor. Returns X = chosen voice.
wts_alloc_voice:
      LDX   #$00
@find:
      LDA   WTS_VOICE_DUR,X
      BEQ   @done                     ; X = idle voice
      INX
      CPX   #$08
      BNE   @find
      LDX   WTS_NEXT_VOICE            ; all busy: steal this one
      CPX   #$08
      BCC   @ok
      LDX   #$00
@ok:
      TXA                             ; advance cursor = (X+1) mod 8
      INC   A
      CMP   #$08
      BCC   @save
      LDA   #$00
@save:
      STA   WTS_NEXT_VOICE
@done:
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

; Shared Nova audio/music helper routines.
;
; Simple SID commands are handled here with direct memory-mapped SID writes.
; File-backed playback and MML sequencing continue to use FIO because those
; paths are hosted by the ESP/wavetable synth side of the system.

.include "audio.inc"

.ifndef AUDIO_IMPLEMENTATION_INCLUDED
AUDIO_IMPLEMENTATION_INCLUDED = 1

.ifndef AUDIO_FILE_COMMANDS
AUDIO_FILE_COMMANDS = 1
.endif

      .segment "CODE"

      .export audio_play_sound
      .export audio_set_volume
      .export audio_sound
      .export audio_volume
      .export audio_instrument
      .export audio_init
      .export audio_tick
.if AUDIO_FILE_COMMANDS
      .export audio_sidplay
      .export audio_sidstop
      .export audio_midplay
      .export audio_midstop
      .export audio_sfload
.endif
      .export audio_music_sequence
      .export audio_music_play
      .export audio_music_stop
      .export audio_music_tempo
      .export audio_music_loop
      .export audio_music_priority
      .export audio_status
      .export audio_sfx_playing
      .export audio_music_playing
      .export audio_music_note

.include "fio.s"

AUDIO_INST_BASE     = $BB50   ; 16 slots * 3 bytes: waveform, AD, SR
AUDIO_VOICE_DUR     = $BB80   ; six SID sound-effect countdowns
AUDIO_VOICE_WAVE    = $BB86   ; six gate-clear control bytes
AUDIO_NEXT_VOICE    = $BB8C
AUDIO_INIT_FLAG     = $BB8D
AUDIO_INIT_MAGIC    = $A5

SID_REG_FREQ_LO     = $00
SID_REG_FREQ_HI     = $01
SID_REG_PW_LO       = $02
SID_REG_PW_HI       = $03
SID_REG_CTRL        = $04
SID_REG_AD          = $05
SID_REG_SR          = $06
SID_REG_VOLUME      = $18
SID_REG_VOICE_VOL   = $1D
SID_DEFAULT_WAVE    = $40
SID_DEFAULT_AD      = $09
SID_DEFAULT_SR      = $06
SID_DEFAULT_PW_HI   = $08

; @label AUDIO.PLAY_SOUND
; @kind routine
; @symbol audio_play_sound
; @abi register
; @summary Play a one-shot note using A/X/Y register arguments.
; @in A: MIDI note
; @in X: Duration in 60 Hz frames
; @in Y: Instrument slot
; @out A: 0 on success, nonzero on error
audio_play_sound:
      STA   AUDIO_NOTE
      STX   AUDIO_DURATION
      STY   AUDIO_INSTRUMENT
      BRA   audio_sound

; @label AUDIO.SET_VOLUME
; @kind routine
; @symbol audio_set_volume
; @abi register
; @summary Set master or per-voice volume using A/X register arguments.
; @in A: Volume level
; @in X: Voice selector, 0=master
; @out A: 0 on success, nonzero on error
audio_set_volume:
      STA   AUDIO_VOLUME
      STX   AUDIO_VOICE
      BRA   audio_volume

; @label AUDIO.SOUND
; @kind routine
; @symbol audio_sound
; @abi pseudo-register
; @summary Start a fire-and-forget SID note from AUDIO.NOTE, AUDIO.DURATION, and AUDIO.INSTRUMENT.
; @requires AUDIO.NOTE AUDIO.DURATION AUDIO.INSTRUMENT
; @out A: 0 on success, nonzero on error
audio_sound:
      JSR   audio_init
      LDA   AUDIO_DURATION
      BEQ   @done

      LDX   AUDIO_NEXT_VOICE
      CPX   #$06
      BCC   @voice_ok
      LDX   #$00
@voice_ok:
      TXA
      INC   A
      CMP   #$06
      BCC   @store_next
      LDA   #$00
@store_next:
      STA   AUDIO_NEXT_VOICE

      PHX
      JSR   audio_load_instrument
      JSR   audio_note_to_sid
      PLX
      JSR   audio_voice_ptr

      LDY   #SID_REG_FREQ_LO
      LDA   NVR1L
      STA   (NVR0L),Y
      INY
      LDA   NVR1H
      STA   (NVR0L),Y
      INY
      LDA   #$00
      STA   (NVR0L),Y
      INY
      LDA   #SID_DEFAULT_PW_HI
      STA   (NVR0L),Y
      LDY   #SID_REG_AD
      LDA   NVR2H
      STA   (NVR0L),Y
      INY
      LDA   NVR3L
      STA   (NVR0L),Y
      LDY   #SID_REG_CTRL
      LDA   NVR2L
      AND   #$FE
      STA   AUDIO_VOICE_WAVE,X
      ORA   #$01
      STA   (NVR0L),Y
      LDA   AUDIO_DURATION
      STA   AUDIO_VOICE_DUR,X
@done:
      LDA   #$00
      RTS

; @label AUDIO.VOLUME
; @kind routine
; @symbol audio_volume
; @abi pseudo-register
; @summary Set SID master or per-voice volume from AUDIO.VOLUME_LEVEL and AUDIO.VOICE.
; @requires AUDIO.VOLUME_LEVEL AUDIO.VOICE
; @out A: 0 on success, nonzero on error
audio_volume:
      LDA   AUDIO_VOLUME
      AND   #$0F
      STA   NVR1L
      LDX   AUDIO_VOICE
      BEQ   @master
      DEX
      CPX   #$06
      BCS   @done
      LDA   NVR1L
      CPX   #$03
      BCS   @sid2_voice
      STA   SID_BASE + SID_REG_VOICE_VOL,X
      BRA   @done
@sid2_voice:
      TXA
      SEC
      SBC   #$03
      TAX
      LDA   NVR1L
      STA   SID2_BASE + SID_REG_VOICE_VOL,X
      BRA   @done
@master:
      LDA   NVR1L
      STA   SID_BASE + SID_REG_VOLUME
      STA   SID2_BASE + SID_REG_VOLUME
@done:
      LDA   #$00
      RTS

; @label AUDIO.INSTRUMENT_SET
; @kind routine
; @symbol audio_instrument
; @abi pseudo-register
; @summary Define a simple SID sound-effect instrument envelope and waveform.
; @requires AUDIO.INST_ID AUDIO.WAVEFORM AUDIO.ATTACK AUDIO.DECAY AUDIO.SUSTAIN AUDIO.RELEASE
; @out A: 0 on success, nonzero on error
audio_instrument:
      JSR   audio_init
      LDA   AUDIO_INST_ID
      AND   #$0F
      STA   NVR1L
      ASL
      CLC
      ADC   NVR1L
      TAX
      LDA   AUDIO_WAVEFORM
      AND   #$FE
      STA   AUDIO_INST_BASE,X
      LDA   AUDIO_ATTACK
      AND   #$0F
      ASL
      ASL
      ASL
      ASL
      STA   NVR1L
      LDA   AUDIO_DECAY
      AND   #$0F
      ORA   NVR1L
      STA   AUDIO_INST_BASE+1,X
      LDA   AUDIO_SUSTAIN
      AND   #$0F
      ASL
      ASL
      ASL
      ASL
      STA   NVR1L
      LDA   AUDIO_RELEASE
      AND   #$0F
      ORA   NVR1L
      STA   AUDIO_INST_BASE+2,X
      LDA   #$00
      RTS

; @label AUDIO.INIT
; @kind routine
; @symbol audio_init
; @abi none
; @summary Initialize the SID sound-effect instrument table and scheduler state.
audio_init:
      LDA   AUDIO_INIT_FLAG
      CMP   #AUDIO_INIT_MAGIC
      BEQ   @done
      LDX   #$00
@inst_loop:
      LDA   #SID_DEFAULT_WAVE
      STA   AUDIO_INST_BASE,X
      INX
      LDA   #SID_DEFAULT_AD
      STA   AUDIO_INST_BASE,X
      INX
      LDA   #SID_DEFAULT_SR
      STA   AUDIO_INST_BASE,X
      INX
      CPX   #$30
      BNE   @inst_loop
      LDX   #$05
      LDA   #$00
@voice_loop:
      STA   AUDIO_VOICE_DUR,X
      STA   AUDIO_VOICE_WAVE,X
      DEX
      BPL   @voice_loop
      STZ   AUDIO_NEXT_VOICE
      LDA   #AUDIO_INIT_MAGIC
      STA   AUDIO_INIT_FLAG
@done:
      RTS

; @label AUDIO.TICK
; @kind routine
; @symbol audio_tick
; @abi none
; @summary Advance fire-and-forget SID sound-effect durations by one frame.
audio_tick:
      LDA   AUDIO_INIT_FLAG
      CMP   #AUDIO_INIT_MAGIC
      BNE   @done
      LDX   #$05
@loop:
      LDA   AUDIO_VOICE_DUR,X
      BEQ   @next
      DEC   AUDIO_VOICE_DUR,X
      BNE   @next
@gate_off:
      JSR   audio_voice_ptr
      LDY   #SID_REG_CTRL
      LDA   AUDIO_VOICE_WAVE,X
      AND   #$FE
      STA   (NVR0L),Y
@next:
      DEX
      BPL   @loop
@done:
      RTS

audio_load_instrument:
      LDA   AUDIO_INSTRUMENT
      AND   #$0F
      STA   NVR3H
      ASL
      CLC
      ADC   NVR3H
      TAY
      LDA   AUDIO_INST_BASE,Y
      AND   #$FE
      STA   NVR2L
      LDA   AUDIO_INST_BASE+1,Y
      STA   NVR2H
      LDA   AUDIO_INST_BASE+2,Y
      STA   NVR3L
      RTS

audio_note_to_sid:
      LDA   AUDIO_NOTE
      LDX   #$00
@div_loop:
      CMP   #$0C
      BCC   @have_note
      SEC
      SBC   #$0C
      INX
      BRA   @div_loop
@have_note:
      TAY
      LDA   audio_sid_freq_lo,Y
      STA   NVR1L
      LDA   #$00
      CPY   #$0B
      BNE   @store_hi
      LDA   #$01
@store_hi:
      STA   NVR1H
      TXA
      BEQ   @done
      TAX
@shift_loop:
      ASL   NVR1L
      ROL   NVR1H
      BCS   @clamp
      DEX
      BNE   @shift_loop
@done:
      RTS
@clamp:
      LDA   #$FF
      STA   NVR1L
      STA   NVR1H
      RTS

audio_voice_ptr:
      LDA   audio_voice_lo,X
      STA   NVR0L
      LDA   #>SID_BASE
      STA   NVR0H
      RTS

audio_sid_freq_lo:
      .byte $8B,$93,$9C,$A6,$AF,$BA,$C5,$D1,$DD,$EA,$F8,$07
audio_voice_lo:
      .byte <(SID_BASE), <(SID_BASE+7), <(SID_BASE+14)
      .byte <(SID2_BASE), <(SID2_BASE+7), <(SID2_BASE+14)

.if AUDIO_FILE_COMMANDS
; @label AUDIO.SIDPLAY
; @kind routine
; @symbol audio_sidplay
; @abi pseudo-register
; @summary Play the SID file named by FIO.NAME/FIO.NAMELEN. AUDIO.NOTE is reused as the 1-based song number.
; @requires FIO.NAME FIO.NAMELEN AUDIO.NOTE
; @out A: 0 on success, nonzero on error
audio_sidplay:
      LDA   #FIO_CMD_SIDPLAY
      JSR   fio_exec
      BNE   @done
      CLI                     ; playback needs timer IRQs active
@done:
      RTS

; @label AUDIO.SIDSTOP
; @kind routine
; @symbol audio_sidstop
; @abi none
; @summary Stop SID file playback.
; @out A: 0 on success, nonzero on error
audio_sidstop:
      LDA   #FIO_CMD_SIDSTOP
      JMP   fio_exec

; @label AUDIO.MIDPLAY
; @kind routine
; @symbol audio_midplay
; @abi pseudo-register
; @summary Play the MIDI file named by FIO.NAME/FIO.NAMELEN.
; @requires FIO.NAME FIO.NAMELEN
; @out A: 0 on success, nonzero on error
audio_midplay:
      LDA   #FIO_CMD_MIDPLAY
      JMP   fio_exec

; @label AUDIO.MIDSTOP
; @kind routine
; @symbol audio_midstop
; @abi none
; @summary Stop MIDI file playback.
; @out A: 0 on success, nonzero on error
audio_midstop:
      LDA   #FIO_CMD_MIDSTOP
      JMP   fio_exec

; @label AUDIO.SFLOAD
; @kind routine
; @symbol audio_sfload
; @abi pseudo-register
; @summary Load the soundfont named by FIO.NAME/FIO.NAMELEN.
; @requires FIO.NAME FIO.NAMELEN
; @out A: 0 on success, nonzero on error
audio_sfload:
      LDA   #FIO_CMD_SFLOAD
      JMP   fio_exec
.endif

; @label AUDIO.MUSIC_SEQUENCE
; @kind routine
; @symbol audio_music_sequence
; @abi pseudo-register
; @summary Queue an MML sequence for AUDIO.VOICE using AUDIO.STRL/H and AUDIO.STRLEN.
; @requires AUDIO.VOICE AUDIO.STRL AUDIO.STRH AUDIO.STRLEN
; @out A: 0 on success, nonzero on error
audio_music_sequence:
      LDA   #FIO_CMD_MSEQ
      JMP   fio_exec

; @label AUDIO.MUSIC_PLAY
; @kind routine
; @symbol audio_music_play
; @abi none
; @summary Start queued music playback.
; @out A: 0 on success, nonzero on error
audio_music_play:
      LDA   #FIO_CMD_MPLAY
      JSR   fio_exec
      BNE   @done
      CLI                     ; playback needs timer IRQs active
@done:
      RTS

; @label AUDIO.MUSIC_STOP
; @kind routine
; @symbol audio_music_stop
; @abi none
; @summary Stop music playback.
; @out A: 0 on success, nonzero on error
audio_music_stop:
      LDA   #FIO_CMD_MSTOP
      JMP   fio_exec

; @label AUDIO.MUSIC_TEMPO
; @kind routine
; @symbol audio_music_tempo
; @abi pseudo-register
; @summary Set music tempo from AUDIO.NOTE/AUDIO.DURATION as a 16-bit BPM value.
; @requires AUDIO.NOTE AUDIO.DURATION
; @out A: 0 on success, nonzero on error
audio_music_tempo:
      LDA   #FIO_CMD_MTEMPO
      JMP   fio_exec

; @label AUDIO.MUSIC_LOOP
; @kind routine
; @symbol audio_music_loop
; @abi pseudo-register
; @summary Enable or disable music looping. AUDIO.NOTE is 0=off, nonzero=on.
; @requires AUDIO.NOTE
; @out A: 0 on success, nonzero on error
audio_music_loop:
      LDA   #FIO_CMD_MLOOP
      JMP   fio_exec

; @label AUDIO.MUSIC_PRIORITY
; @kind routine
; @symbol audio_music_priority
; @abi pseudo-register
; @summary Set hosted MML priority from AUDIO.NOTE and following pseudo-register bytes.
; @requires AUDIO.NOTE
; @out A: 0 on success, nonzero on error
audio_music_priority:
      LDA   #FIO_CMD_MPRI
      JMP   fio_exec

; @label AUDIO.STATUS
; @kind routine
; @symbol audio_status
; @abi none
; @summary Return raw music status bits.
; @out A: Raw MUSIC_STATUS byte
audio_status:
      LDA   MUSIC_STATUS
      RTS

; @label AUDIO.SFX_PLAYING
; @kind routine
; @symbol audio_sfx_playing
; @abi none
; @summary Return nonzero if the hosted music engine reports a sound effect.
; @out A: 1 if playing, 0 if idle
audio_sfx_playing:
      LDA   MUSIC_STATUS
      AND   #$01
      RTS

; @label AUDIO.MUSIC_PLAYING
; @kind routine
; @symbol audio_music_playing
; @abi none
; @summary Return nonzero if music is playing.
; @out A: 1 if playing, 0 if idle
audio_music_playing:
      LDA   MUSIC_STATUS
      AND   #$02
      BEQ   @off
      LDA   #$01
@off:
      RTS

; @label AUDIO.MUSIC_NOTE
; @kind routine
; @symbol audio_music_note
; @abi register
; @summary Return the current MIDI note for a music voice.
; @in X: Voice number, 1..14
; @out A: Current MIDI note, or 0 if out of range or silent
audio_music_note:
      DEX
      CPX   #$0E
      BCS   @bad
      LDA   MUSIC_NOTE1,X
      RTS
@bad:
      LDA   #$00
      RTS

.endif

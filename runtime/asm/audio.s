; Shared Nova audio/music helper routines.
;
; These routines are BASIC-agnostic.  They wrap the FIO audio commands behind
; named entry points so BASIC, assembly programs, and future runtimes share one
; audio ABI.

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
; @summary Play a one-shot note from AUDIO.NOTE, AUDIO.DURATION, and AUDIO.INSTRUMENT.
; @requires AUDIO.NOTE AUDIO.DURATION AUDIO.INSTRUMENT
; @out A: 0 on success, nonzero on error
audio_sound:
      LDA   #FIO_CMD_SOUND
      JMP   fio_exec

; @label AUDIO.VOLUME
; @kind routine
; @symbol audio_volume
; @abi pseudo-register
; @summary Set volume from AUDIO.VOLUME_LEVEL and AUDIO.VOICE.
; @requires AUDIO.VOLUME_LEVEL AUDIO.VOICE
; @out A: 0 on success, nonzero on error
audio_volume:
      LDA   #FIO_CMD_VOLUME
      JMP   fio_exec

; @label AUDIO.INSTRUMENT_SET
; @kind routine
; @symbol audio_instrument
; @abi pseudo-register
; @summary Define an instrument envelope and waveform.
; @requires AUDIO.INST_ID AUDIO.WAVEFORM AUDIO.ATTACK AUDIO.DECAY AUDIO.SUSTAIN AUDIO.RELEASE
; @out A: 0 on success, nonzero on error
audio_instrument:
      LDA   #FIO_CMD_INSTRUMENT
      JMP   fio_exec

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
; @summary Set voice-stealing priority from AUDIO.NOTE and following pseudo-register bytes.
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
; @summary Return nonzero if a sound effect is playing.
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

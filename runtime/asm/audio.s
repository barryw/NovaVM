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
.ifndef AUDIO_POINTER_FILE_HELPERS
AUDIO_POINTER_FILE_HELPERS = 1
.endif

.ifdef NOVA_STRIP_UNUSED
AUDIO_EMIT_ALL = 0
.else
.ifdef AUDIO_STRIP_UNUSED
AUDIO_EMIT_ALL = 0
.else
.ifdef NOVA_EMIT_ALL_RUNTIME
AUDIO_EMIT_ALL = 1
.else
.ifdef AUDIO_EMIT_ALL_RUNTIME
AUDIO_EMIT_ALL = 1
.else
AUDIO_EMIT_ALL = 0
.endif
.endif
.endif
.endif

.if AUDIO_EMIT_ALL = 0
.if .referenced(audio_play_sound)
      .refto audio_sound
.endif
.if .referenced(audio_set_volume)
      .refto audio_volume
.endif
.if .referenced(audio_sound)
      .refto audio_init
      .refto audio_load_instrument
      .refto audio_note_to_sid
      .refto audio_voice_ptr
.endif
.if .referenced(audio_instrument)
      .refto audio_init
.endif
.if .referenced(audio_tick)
      .refto audio_voice_ptr
.endif
.if .referenced(audio_sidplay_file)
      .refto audio_sidplay
      .refto fio_copy_name
.endif
.if .referenced(audio_midplay_file)
      .refto audio_midplay
      .refto fio_copy_name
.endif
.if .referenced(audio_sfload_file)
      .refto audio_sfload
      .refto fio_copy_name
.endif
.if .referenced(audio_sidplay) .OR .referenced(audio_sidstop) .OR .referenced(audio_midplay) .OR .referenced(audio_midstop) .OR .referenced(audio_sfload) .OR .referenced(audio_music_sequence) .OR .referenced(audio_music_play) .OR .referenced(audio_music_stop) .OR .referenced(audio_music_tempo) .OR .referenced(audio_music_loop) .OR .referenced(audio_music_priority)
      .refto fio_exec
.endif
.endif

      .segment "CODE"

.if AUDIO_EMIT_ALL .OR .referenced(audio_play_sound)
      .export audio_play_sound
.endif
.if AUDIO_EMIT_ALL .OR .referenced(audio_set_volume)
      .export audio_set_volume
.endif
.if AUDIO_EMIT_ALL .OR .referenced(audio_sound)
      .export audio_sound
.endif
.if AUDIO_EMIT_ALL .OR .referenced(audio_volume)
      .export audio_volume
.endif
.if AUDIO_EMIT_ALL .OR .referenced(audio_instrument)
      .export audio_instrument
.endif
.if AUDIO_EMIT_ALL .OR .referenced(audio_init)
      .export audio_init
.endif
.if AUDIO_EMIT_ALL .OR .referenced(audio_tick)
      .export audio_tick
.endif
.if AUDIO_FILE_COMMANDS
.if AUDIO_EMIT_ALL .OR .referenced(audio_sidplay)
      .export audio_sidplay
.endif
.if AUDIO_EMIT_ALL .OR .referenced(audio_sidstop)
      .export audio_sidstop
.endif
.if AUDIO_EMIT_ALL .OR .referenced(audio_midplay)
      .export audio_midplay
.endif
.if AUDIO_EMIT_ALL .OR .referenced(audio_midstop)
      .export audio_midstop
.endif
.if AUDIO_EMIT_ALL .OR .referenced(audio_sfload)
      .export audio_sfload
.endif
.if AUDIO_POINTER_FILE_HELPERS
.if AUDIO_EMIT_ALL .OR .referenced(audio_sidplay_file)
      .export audio_sidplay_file
.endif
.if AUDIO_EMIT_ALL .OR .referenced(audio_midplay_file)
      .export audio_midplay_file
.endif
.if AUDIO_EMIT_ALL .OR .referenced(audio_sfload_file)
      .export audio_sfload_file
.endif
.endif
.endif
.if AUDIO_EMIT_ALL .OR .referenced(audio_music_sequence)
      .export audio_music_sequence
.endif
.if AUDIO_EMIT_ALL .OR .referenced(audio_music_play)
      .export audio_music_play
.endif
.if AUDIO_EMIT_ALL .OR .referenced(audio_music_stop)
      .export audio_music_stop
.endif
.if AUDIO_EMIT_ALL .OR .referenced(audio_music_tempo)
      .export audio_music_tempo
.endif
.if AUDIO_EMIT_ALL .OR .referenced(audio_music_loop)
      .export audio_music_loop
.endif
.if AUDIO_EMIT_ALL .OR .referenced(audio_music_priority)
      .export audio_music_priority
.endif
.if AUDIO_EMIT_ALL .OR .referenced(audio_status)
      .export audio_status
.endif
.if AUDIO_EMIT_ALL .OR .referenced(audio_sfx_playing)
      .export audio_sfx_playing
.endif
.if AUDIO_EMIT_ALL .OR .referenced(audio_music_playing)
      .export audio_music_playing
.endif
.if AUDIO_EMIT_ALL .OR .referenced(audio_music_note)
      .export audio_music_note
.endif

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
.if AUDIO_EMIT_ALL .OR .referenced(audio_play_sound)
audio_play_sound:
      STA   AUDIO_NOTE
      STX   AUDIO_DURATION
      STY   AUDIO_INSTRUMENT
      BRA   audio_sound
.endif

; @label AUDIO.SET_VOLUME
; @kind routine
; @symbol audio_set_volume
; @abi register
; @summary Set master or per-voice volume using A/X register arguments.
; @in A: Volume level
; @in X: Voice selector, 0=master
; @out A: 0 on success, nonzero on error
.if AUDIO_EMIT_ALL .OR .referenced(audio_set_volume)
audio_set_volume:
      STA   AUDIO_VOLUME
      STX   AUDIO_VOICE
      BRA   audio_volume
.endif

; @label AUDIO.SOUND
; @kind routine
; @symbol audio_sound
; @abi pseudo-register
; @summary Start a fire-and-forget SID note from AUDIO.NOTE, AUDIO.DURATION, and AUDIO.INSTRUMENT.
; @requires AUDIO.NOTE AUDIO.DURATION AUDIO.INSTRUMENT
; @out A: 0 on success, nonzero on error
.if AUDIO_EMIT_ALL .OR .referenced(audio_sound)
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
.endif

; @label AUDIO.VOLUME
; @kind routine
; @symbol audio_volume
; @abi pseudo-register
; @summary Set SID master or per-voice volume from AUDIO.VOLUME_LEVEL and AUDIO.VOICE.
; @requires AUDIO.VOLUME_LEVEL AUDIO.VOICE
; @out A: 0 on success, nonzero on error
.if AUDIO_EMIT_ALL .OR .referenced(audio_volume)
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
.endif

; @label AUDIO.INSTRUMENT_SET
; @kind routine
; @symbol audio_instrument
; @abi pseudo-register
; @summary Define a simple SID sound-effect instrument envelope and waveform.
; @requires AUDIO.INST_ID AUDIO.WAVEFORM AUDIO.ATTACK AUDIO.DECAY AUDIO.SUSTAIN AUDIO.RELEASE
; @out A: 0 on success, nonzero on error
.if AUDIO_EMIT_ALL .OR .referenced(audio_instrument)
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
.endif

; @label AUDIO.INIT
; @kind routine
; @symbol audio_init
; @abi none
; @summary Initialize the SID sound-effect instrument table and scheduler state.
.if AUDIO_EMIT_ALL .OR .referenced(audio_init)
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
.endif

; @label AUDIO.TICK
; @kind routine
; @symbol audio_tick
; @abi none
; @summary Advance fire-and-forget SID sound-effect durations by one frame.
.if AUDIO_EMIT_ALL .OR .referenced(audio_tick)
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
.endif

.if AUDIO_EMIT_ALL .OR .referenced(audio_load_instrument)
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
.endif

.if AUDIO_EMIT_ALL .OR .referenced(audio_note_to_sid)
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
.endif

.if AUDIO_EMIT_ALL .OR .referenced(audio_voice_ptr)
audio_voice_ptr:
      LDA   audio_voice_lo,X
      STA   NVR0L
      LDA   #>SID_BASE
      STA   NVR0H
      RTS
.endif

.if AUDIO_EMIT_ALL .OR .referenced(audio_note_to_sid)
audio_sid_freq_lo:
      .byte $8B,$93,$9C,$A6,$AF,$BA,$C5,$D1,$DD,$EA,$F8,$07
.endif
.if AUDIO_EMIT_ALL .OR .referenced(audio_voice_ptr)
audio_voice_lo:
      .byte <(SID_BASE), <(SID_BASE+7), <(SID_BASE+14)
      .byte <(SID2_BASE), <(SID2_BASE+7), <(SID2_BASE+14)
.endif

.if AUDIO_FILE_COMMANDS
; @label AUDIO.SIDPLAY
; @kind routine
; @symbol audio_sidplay
; @abi pseudo-register
; @summary Play the SID file named by FIO.NAME/FIO.NAMELEN. AUDIO.NOTE is reused as the 1-based song number.
; @requires FIO.NAME FIO.NAMELEN AUDIO.NOTE
; @out A: 0 on success, nonzero on error
.if AUDIO_EMIT_ALL .OR .referenced(audio_sidplay)
audio_sidplay:
      LDA   #FIO_CMD_SIDPLAY
      JSR   fio_exec
      BNE   @done
      CLI                     ; playback needs timer IRQs active
@done:
      RTS
.endif

; @label AUDIO.SIDSTOP
; @kind routine
; @symbol audio_sidstop
; @abi none
; @summary Stop SID file playback.
; @out A: 0 on success, nonzero on error
.if AUDIO_EMIT_ALL .OR .referenced(audio_sidstop)
audio_sidstop:
      LDA   #FIO_CMD_SIDSTOP
      JMP   fio_exec
.endif

; @label AUDIO.MIDPLAY
; @kind routine
; @symbol audio_midplay
; @abi pseudo-register
; @summary Play the MIDI file named by FIO.NAME/FIO.NAMELEN.
; @requires FIO.NAME FIO.NAMELEN
; @out A: 0 on success, nonzero on error
.if AUDIO_EMIT_ALL .OR .referenced(audio_midplay)
audio_midplay:
      LDA   #FIO_CMD_MIDPLAY
      JMP   fio_exec
.endif

; @label AUDIO.MIDSTOP
; @kind routine
; @symbol audio_midstop
; @abi none
; @summary Stop MIDI file playback.
; @out A: 0 on success, nonzero on error
.if AUDIO_EMIT_ALL .OR .referenced(audio_midstop)
audio_midstop:
      LDA   #FIO_CMD_MIDSTOP
      JMP   fio_exec
.endif

; @label AUDIO.SFLOAD
; @kind routine
; @symbol audio_sfload
; @abi pseudo-register
; @summary Load the soundfont named by FIO.NAME/FIO.NAMELEN.
; @requires FIO.NAME FIO.NAMELEN
; @out A: 0 on success, nonzero on error
.if AUDIO_EMIT_ALL .OR .referenced(audio_sfload)
audio_sfload:
      LDA   #FIO_CMD_SFLOAD
      JMP   fio_exec
.endif

.if AUDIO_POINTER_FILE_HELPERS
; @label AUDIO.SIDPLAY_FILE
; @kind routine
; @symbol audio_sidplay_file
; @abi pseudo-register
; @summary Copy a pointer-based SID filename into FIO.NAME and start SID playback.
; @requires FIO_ARG_NAMELEN FIO_ARG_NAMEPTR_L FIO_ARG_NAMEPTR_H AUDIO_NOTE
; @out A: 0 on success, nonzero on error
.if AUDIO_EMIT_ALL .OR .referenced(audio_sidplay_file)
audio_sidplay_file:
      JSR   fio_copy_name
      BNE   @done
      JMP   audio_sidplay
@done:
      RTS
.endif

; @label AUDIO.MIDPLAY_FILE
; @kind routine
; @symbol audio_midplay_file
; @abi pseudo-register
; @summary Copy a pointer-based MIDI filename into FIO.NAME and start MIDI playback.
; @requires FIO_ARG_NAMELEN FIO_ARG_NAMEPTR_L FIO_ARG_NAMEPTR_H
; @out A: 0 on success, nonzero on error
.if AUDIO_EMIT_ALL .OR .referenced(audio_midplay_file)
audio_midplay_file:
      JSR   fio_copy_name
      BNE   @done
      JMP   audio_midplay
@done:
      RTS
.endif

; @label AUDIO.SFLOAD_FILE
; @kind routine
; @symbol audio_sfload_file
; @abi pseudo-register
; @summary Copy a pointer-based soundfont filename into FIO.NAME and load it.
; @requires FIO_ARG_NAMELEN FIO_ARG_NAMEPTR_L FIO_ARG_NAMEPTR_H
; @out A: 0 on success, nonzero on error
.if AUDIO_EMIT_ALL .OR .referenced(audio_sfload_file)
audio_sfload_file:
      JSR   fio_copy_name
      BNE   @done
      JMP   audio_sfload
@done:
      RTS
.endif
.endif
.endif

; @label AUDIO.MUSIC_SEQUENCE
; @kind routine
; @symbol audio_music_sequence
; @abi pseudo-register
; @summary Queue an MML sequence for AUDIO.VOICE using AUDIO.STRL/H and AUDIO.STRLEN.
; @requires AUDIO.VOICE AUDIO.STRL AUDIO.STRH AUDIO.STRLEN
; @out A: 0 on success, nonzero on error
.if AUDIO_EMIT_ALL .OR .referenced(audio_music_sequence)
audio_music_sequence:
      LDA   #FIO_CMD_MSEQ
      JMP   fio_exec
.endif

; @label AUDIO.MUSIC_PLAY
; @kind routine
; @symbol audio_music_play
; @abi none
; @summary Start queued music playback.
; @out A: 0 on success, nonzero on error
.if AUDIO_EMIT_ALL .OR .referenced(audio_music_play)
audio_music_play:
      LDA   #FIO_CMD_MPLAY
      JSR   fio_exec
      BNE   @done
      CLI                     ; playback needs timer IRQs active
@done:
      RTS
.endif

; @label AUDIO.MUSIC_STOP
; @kind routine
; @symbol audio_music_stop
; @abi none
; @summary Stop music playback.
; @out A: 0 on success, nonzero on error
.if AUDIO_EMIT_ALL .OR .referenced(audio_music_stop)
audio_music_stop:
      LDA   #FIO_CMD_MSTOP
      JMP   fio_exec
.endif

; @label AUDIO.MUSIC_TEMPO
; @kind routine
; @symbol audio_music_tempo
; @abi pseudo-register
; @summary Set music tempo from AUDIO.NOTE/AUDIO.DURATION as a 16-bit BPM value.
; @requires AUDIO.NOTE AUDIO.DURATION
; @out A: 0 on success, nonzero on error
.if AUDIO_EMIT_ALL .OR .referenced(audio_music_tempo)
audio_music_tempo:
      LDA   #FIO_CMD_MTEMPO
      JMP   fio_exec
.endif

; @label AUDIO.MUSIC_LOOP
; @kind routine
; @symbol audio_music_loop
; @abi pseudo-register
; @summary Enable or disable music looping. AUDIO.NOTE is 0=off, nonzero=on.
; @requires AUDIO.NOTE
; @out A: 0 on success, nonzero on error
.if AUDIO_EMIT_ALL .OR .referenced(audio_music_loop)
audio_music_loop:
      LDA   #FIO_CMD_MLOOP
      JMP   fio_exec
.endif

; @label AUDIO.MUSIC_PRIORITY
; @kind routine
; @symbol audio_music_priority
; @abi pseudo-register
; @summary Set hosted MML priority from AUDIO.NOTE and following pseudo-register bytes.
; @requires AUDIO.NOTE
; @out A: 0 on success, nonzero on error
.if AUDIO_EMIT_ALL .OR .referenced(audio_music_priority)
audio_music_priority:
      LDA   #FIO_CMD_MPRI
      JMP   fio_exec
.endif

; @label AUDIO.STATUS
; @kind routine
; @symbol audio_status
; @abi none
; @summary Return raw music status bits.
; @out A: Raw MUSIC_STATUS byte
.if AUDIO_EMIT_ALL .OR .referenced(audio_status)
audio_status:
      LDA   MUSIC_STATUS
      RTS
.endif

; @label AUDIO.SFX_PLAYING
; @kind routine
; @symbol audio_sfx_playing
; @abi none
; @summary Return nonzero if the hosted music engine reports a sound effect.
; @out A: 1 if playing, 0 if idle
.if AUDIO_EMIT_ALL .OR .referenced(audio_sfx_playing)
audio_sfx_playing:
      LDA   MUSIC_STATUS
      AND   #AUDIO_STATUS_SFX
      RTS
.endif

; @label AUDIO.MUSIC_PLAYING
; @kind routine
; @symbol audio_music_playing
; @abi none
; @summary Return nonzero if music is playing.
; @out A: 1 if playing, 0 if idle
.if AUDIO_EMIT_ALL .OR .referenced(audio_music_playing)
audio_music_playing:
      LDA   MUSIC_STATUS
      AND   #AUDIO_STATUS_MUSIC
      BEQ   @off
      LDA   #$01
@off:
      RTS
.endif

; @label AUDIO.MUSIC_NOTE
; @kind routine
; @symbol audio_music_note
; @abi register
; @summary Return the current MIDI note for a music voice.
; @in X: Voice number, 1..14
; @out A: Current MIDI note, or 0 if out of range or silent
.if AUDIO_EMIT_ALL .OR .referenced(audio_music_note)
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

.ifndef FIO_EMIT_ALL_RUNTIME
FIO_EMIT_ALL_RUNTIME = 1
.endif
.include "fio.s"

.endif

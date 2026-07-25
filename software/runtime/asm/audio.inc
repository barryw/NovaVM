; =====================================================================
;  Nova NDK — audio.inc
;
;  Audio/music runtime ABI: sound effects, volume, SID instruments, MML
;  sequencing, hosted SID/MIDI playback, and the unified music metadata block.
;  BASIC and assembly callers share the same argument-register layout.
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

; Sound effect:
;   AUDIO_NOTE       MIDI note
;   AUDIO_DURATION   duration in 60 Hz frames
;   AUDIO_SOUND_INSTRUMENT instrument slot
;
; Volume:
;   AUDIO_VOLUME_LEVEL SID volume level
;   AUDIO_VOICE      0=master, 1..6=SID voice volume
;
; Instrument:
;   AUDIO_INST_ID    instrument slot
;   AUDIO_WAVEFORM   SID waveform/control byte
;   AUDIO_ATTACK     attack
;   AUDIO_DECAY      decay
;   AUDIO_SUSTAIN    sustain
;   AUDIO_RELEASE    release
;
; Music sequence:
;   AUDIO_VOICE      hosted MML voice number
;   AUDIO_STRLEN     MML byte length
;   AUDIO_STRL/H     pointer to MML bytes
;
; File-backed commands:
;   FIO_NAME/FIO_NAMELEN hold the filename.  Call fio_copy_name first when
;   using a pointer-based filename.
;
; Routines return A=0 on success and A=1 on error unless documented as a raw
; status query.

.ifndef AUDIO_INC_INCLUDED
AUDIO_INC_INCLUDED = 1

.include "fio.inc"
.include "vgc.inc"

; MUSIC_STATUS bit layout.
; These bits are shared by BASIC, assembly apps, and hosted playback.
.ifndef MUSIC_STATUS_SFX
MUSIC_STATUS_SFX     = $01
.endif
.ifndef MUSIC_STATUS_MUSIC
MUSIC_STATUS_MUSIC   = $02
.endif
.ifndef MUSIC_STATUS_SID
MUSIC_STATUS_SID     = $04
.endif
.ifndef MUSIC_STATUS_WTS
MUSIC_STATUS_WTS     = $08
.endif
.ifndef MUSIC_STATUS_LOADING
MUSIC_STATUS_LOADING = $10
.endif

; @label AUDIO.STATUS_SFX
; @kind const
; @symbol AUDIO_STATUS_SFX
; @summary MUSIC_STATUS bit set while a hosted sound effect is active.
AUDIO_STATUS_SFX     = MUSIC_STATUS_SFX
; @label AUDIO.STATUS_MUSIC
; @kind const
; @symbol AUDIO_STATUS_MUSIC
; @summary MUSIC_STATUS bit set while hosted music playback is active.
AUDIO_STATUS_MUSIC   = MUSIC_STATUS_MUSIC
; @label AUDIO.STATUS_SID
; @kind const
; @symbol AUDIO_STATUS_SID
; @summary MUSIC_STATUS bit set when the current music source uses SID voices.
AUDIO_STATUS_SID     = MUSIC_STATUS_SID
; @label AUDIO.STATUS_WTS
; @kind const
; @symbol AUDIO_STATUS_WTS
; @summary MUSIC_STATUS bit set when the current music source uses WTS/MIDI voices.
AUDIO_STATUS_WTS     = MUSIC_STATUS_WTS
; @label AUDIO.STATUS_LOADING
; @kind const
; @symbol AUDIO_STATUS_LOADING
; @summary MUSIC_STATUS bit set while hosted SID/MIDI/soundfont assets are loading.
AUDIO_STATUS_LOADING = MUSIC_STATUS_LOADING

; Hosted music metadata. NovaHost writes this unified block for both SID and
; MIDI/WTS playback so visualizers and apps do not need per-player ABIs.
; The first 0x6C bytes match the generic file metadata buffer; the final flag
; byte is interpreted by music consumers.
; @label AUDIO.META_TYPE
; @kind u8
; @symbol AUDIO_META_TYPE
; @summary Active music metadata file type: 1=SID, 3=MIDI/WTS.
AUDIO_META_TYPE       = $BAB0
; @label AUDIO.META_SIZE_L
; @kind u8
; @symbol AUDIO_META_SIZE_L
; @summary Low byte of active music file size.
AUDIO_META_SIZE_L     = $BAB1
; @label AUDIO.META_SIZE_H
; @kind u8
; @symbol AUDIO_META_SIZE_H
; @summary High byte of active music file size.
AUDIO_META_SIZE_H     = $BAB2
; @label AUDIO.META_TITLE
; @kind bytes
; @symbol AUDIO_META_TITLE
; @summary 32-byte null-padded active music title.
AUDIO_META_TITLE      = $BAB3
; @label AUDIO.META_AUTHOR
; @kind bytes
; @symbol AUDIO_META_AUTHOR
; @summary 32-byte null-padded active music author/composer.
AUDIO_META_AUTHOR     = $BAD3
; @label AUDIO.META_COPYRIGHT
; @kind bytes
; @symbol AUDIO_META_COPYRIGHT
; @summary 32-byte null-padded active music copyright/comment.
AUDIO_META_COPYRIGHT  = $BAF3
; @label AUDIO.META_LOAD_L
; @kind u8
; @symbol AUDIO_META_LOAD_L
; @summary SID load address low byte when applicable.
AUDIO_META_LOAD_L     = $BB13
; @label AUDIO.META_LOAD_H
; @kind u8
; @symbol AUDIO_META_LOAD_H
; @summary SID load address high byte when applicable.
AUDIO_META_LOAD_H     = $BB14
; @label AUDIO.META_INIT_L
; @kind u8
; @symbol AUDIO_META_INIT_L
; @summary SID init address low byte when applicable.
AUDIO_META_INIT_L     = $BB15
; @label AUDIO.META_INIT_H
; @kind u8
; @symbol AUDIO_META_INIT_H
; @summary SID init address high byte when applicable.
AUDIO_META_INIT_H     = $BB16
; @label AUDIO.META_PLAY_L
; @kind u8
; @symbol AUDIO_META_PLAY_L
; @summary SID play address low byte when applicable.
AUDIO_META_PLAY_L     = $BB17
; @label AUDIO.META_PLAY_H
; @kind u8
; @symbol AUDIO_META_PLAY_H
; @summary SID play address high byte when applicable.
AUDIO_META_PLAY_H     = $BB18
; @label AUDIO.META_SONGS
; @kind u8
; @symbol AUDIO_META_SONGS
; @summary SID subtune count or MIDI track/song count when known.
AUDIO_META_SONGS      = $BB19
; @label AUDIO.META_DURATION_L
; @kind u8
; @symbol AUDIO_META_DURATION_L
; @summary Duration seconds low byte when known.
AUDIO_META_DURATION_L = $BB1A
; @label AUDIO.META_DURATION_H
; @kind u8
; @symbol AUDIO_META_DURATION_H
; @summary Duration seconds high byte when known.
AUDIO_META_DURATION_H = $BB1B
; @label AUDIO.META_FLAGS
; @kind u8
; @symbol AUDIO_META_FLAGS
; @summary Music-specific flags for the active metadata block.
AUDIO_META_FLAGS      = $BB1C
; @label AUDIO.META_SOUNDFONT
; @kind bytes
; @symbol AUDIO_META_SOUNDFONT
; @summary 64-byte null-padded active MIDI/WTS soundfont name.
AUDIO_META_SOUNDFONT  = $BB8E

AUDIO_META_TYPE_SID   = $01
AUDIO_META_TYPE_MIDI  = $03
AUDIO_META_TITLE_LEN  = $20
AUDIO_META_AUTHOR_LEN = $20
AUDIO_META_COPYRIGHT_LEN = $20
AUDIO_META_SOUNDFONT_LEN = $40

AUDIO_META_FLAG_SID_6581 = $01
AUDIO_META_FLAG_SID_8580 = $02
AUDIO_META_FLAG_STEREO   = $04
AUDIO_META_FLAG_NTSC     = $08

; Compatibility aliases matching the generated novavm.inc camel-case names.
MusicMetaFlags = AUDIO_META_FLAGS

; @label AUDIO.NOTE
; @kind u8
; @symbol AUDIO_NOTE
; @summary MIDI note for AUDIO.SOUND.
AUDIO_NOTE       = FIO_SRCL
; @label AUDIO.DURATION
; @kind u8
; @symbol AUDIO_DURATION
; @summary Duration in 60 Hz frames for AUDIO.SOUND.
AUDIO_DURATION   = FIO_SRCH
; @label AUDIO.SOUND_INSTRUMENT
; @kind u8
; @symbol AUDIO_SOUND_INSTRUMENT
; @summary Instrument slot for AUDIO.SOUND.
AUDIO_SOUND_INSTRUMENT = FIO_ENDL

; @label AUDIO.VOLUME_LEVEL
; @kind u8
; @symbol AUDIO_VOLUME_LEVEL
; @summary Volume level for AUDIO.VOLUME.
AUDIO_VOLUME_LEVEL = FIO_SRCL
; @label AUDIO.VOICE
; @kind u8
; @symbol AUDIO_VOICE
; @summary Voice selector. Use 0 for SID master volume or 1..6 for SID voice volume.
AUDIO_VOICE      = FIO_SRCH

; @label AUDIO.INST_ID
; @kind u8
; @symbol AUDIO_INST_ID
; @summary Instrument slot to define for AUDIO.INSTRUMENT_SET.
AUDIO_INST_ID    = FIO_SRCL
; @label AUDIO.WAVEFORM
; @kind u8
; @symbol AUDIO_WAVEFORM
; @summary SID waveform/control byte for AUDIO.INSTRUMENT_SET.
AUDIO_WAVEFORM   = FIO_SRCH
; @label AUDIO.ATTACK
; @kind u8
; @symbol AUDIO_ATTACK
; @summary Attack nibble for AUDIO.INSTRUMENT_SET.
AUDIO_ATTACK     = FIO_ENDL
; @label AUDIO.DECAY
; @kind u8
; @symbol AUDIO_DECAY
; @summary Decay nibble for AUDIO.INSTRUMENT_SET.
AUDIO_DECAY      = FIO_ENDH
; @label AUDIO.SUSTAIN
; @kind u8
; @symbol AUDIO_SUSTAIN
; @summary Sustain nibble for AUDIO.INSTRUMENT_SET.
AUDIO_SUSTAIN    = FIO_SIZEL
; @label AUDIO.RELEASE
; @kind u8
; @symbol AUDIO_RELEASE
; @summary Release nibble for AUDIO.INSTRUMENT_SET.
AUDIO_RELEASE    = FIO_SIZEH

; @label AUDIO.STRL
; @kind u8
; @symbol AUDIO_STRL
; @summary Low byte of MML string pointer for AUDIO.MUSIC_SEQUENCE.
AUDIO_STRL       = FIO_ENDL
; @label AUDIO.STRH
; @kind u8
; @symbol AUDIO_STRH
; @summary High byte of MML string pointer for AUDIO.MUSIC_SEQUENCE.
AUDIO_STRH       = FIO_ENDH
; @label AUDIO.STRLEN
; @kind u8
; @symbol AUDIO_STRLEN
; @summary MML string length for AUDIO.MUSIC_SEQUENCE.
AUDIO_STRLEN     = FIO_NAMELEN

.global audio_play_sound_async
.global audio_set_volume
.global audio_sound_async
.global audio_volume
.global audio_tone
.global audio_tone_hz
.global audio_tone_stop
.global audio_noise
.global audio_instrument
.global audio_init
.global audio_tick
.global audio_sidplay_async
.global audio_sidstop
.global audio_midplay_async
.global audio_midstop
.global audio_sfload_async
.global audio_sidplay_file_async
.global audio_midplay_file_async
.global audio_sfload_file_async
.global audio_music_sequence
.global audio_music_play_async
.global audio_music_stop
.global audio_music_tempo
.global audio_music_loop
.global audio_music_priority
.global audio_status
.global audio_sfx_playing
.global audio_music_playing
.global audio_music_note

.endif

; Shared Nova audio/music runtime interface.
;
; BASIC parses language syntax and loads the argument registers; assembly
; callers can use the same register layout directly.
;
; Sound effect:
;   AUDIO_NOTE       MIDI note
;   AUDIO_DURATION   duration in 60 Hz frames
;   AUDIO_INSTRUMENT instrument slot
;
; Volume:
;   AUDIO_VOLUME     volume level
;   AUDIO_VOICE      0=master, 1..14=voice volume
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
;   AUDIO_VOICE      voice number, 1..14
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
; @label AUDIO.INSTRUMENT
; @kind u8
; @symbol AUDIO_INSTRUMENT
; @summary Instrument slot for AUDIO.SOUND.
AUDIO_INSTRUMENT = FIO_ENDL

; @label AUDIO.VOLUME_LEVEL
; @kind u8
; @symbol AUDIO_VOLUME
; @summary Volume level for AUDIO.VOLUME.
AUDIO_VOLUME     = FIO_SRCL
; @label AUDIO.VOICE
; @kind u8
; @symbol AUDIO_VOICE
; @summary Voice selector. Use 0 for master volume or 1..14 for music voices.
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

.endif

; libsound.inc — SOUND module (MODULE_ID_SOUND = $02) fn-id contract.
;
; The shared audio-services module. It is the home of the full audio NDK
; (runtime/asm/audio.s): simple SID tone/noise/volume, fire-and-forget SID
; sound effects, instrument definition, file-backed playback (SID/MIDI/sound-
; fonts), and the hosted MML music engine. Every fn here is a THIN lib_call
; wrapper that marshals the mailbox into an NDK routine's inputs and JSRs it.
;
; CALLING CONVENTION (caller side, see libabi.inc):
;   LIB_MOD_ID ($0300) = MODULE_ID_SOUND
;   LIB_FN_ID  ($0301) = one of the SND_* ids below
;   LIB_ARG0..3 ($0303/$0307/$030B/$030F) = 32-bit LE arg cells
;   JSR LIB_LOADER_BAND
;   -> LIB_RESULT ($0313) = 32-bit LE result (reporters only)
;   -> LIB_STATUS ($0302) = LERR_OK ($00) or an error code
;
; STATUS MAPPING: fallible NDK routines return A=0 on success / nonzero on
;   error; the wrapper maps A!=0 -> LIB_STATUS = LERR_AUDIO_FAIL, A==0 -> OK.
;   Reporter fns publish their NDK A value to LIB_RESULT byte0 (LIB_STATUS=OK).

.ifndef LIBSOUND_INC
LIBSOUND_INC = 1

.include "libabi.inc"            ; MODULE_ID_SOUND, LIB_* mailbox cells, LERR_*

; --- Status code returned when a wrapped NDK audio routine reports failure ---
LERR_AUDIO_FAIL = $89   ; audio_* NDK routine returned A!=0 (FIO error / bad arg)

; =====================================================================
; Simple SID effects (Phase B: moved out of the NovaLogo extension ROM)
; =====================================================================
SND_TONE     = $00   ; (freq:ARG0 u16 SID word, dur:ARG1 frames) -> SID voice 0 sawtooth tone
SND_NOISE    = $01   ; (dur:ARG0 frames)                         -> SID voice 0 noise burst
SND_VOLUME   = $02   ; (vol:ARG0 0-15)                           -> SID master volume

; =====================================================================
; Fire-and-forget SID sound effects + scheduler (audio.s core)
; =====================================================================
SND_PLAY_SOUND  = $03  ; (note:ARG0, dur:ARG1 frames, instr:ARG2)  -> one-shot SID note (A/X/Y ABI)
SND_SOUND       = $04  ; (note:ARG0, dur:ARG1 frames, instr:ARG2)  -> one-shot SID note (pseudo-reg ABI)
SND_SET_VOLUME  = $05  ; (vol:ARG0, voice:ARG1 0=master,1..6)      -> master/per-voice SID volume
SND_INSTRUMENT  = $06  ; (id:ARG0, wave:ARG1, a:ARG2, d:ARG3 lo,   ; sustain ARG3 byte1, release ARG3 byte2)
                       ;                                           -> define SID SFX instrument
SND_INIT        = $07  ; ()                                        -> init SID SFX instrument table + scheduler
SND_TICK        = $08  ; ()                                        -> advance SFX durations one frame

; =====================================================================
; File-backed playback (audio.s, FIO.NAME/FIO.NAMELEN ABI)
; =====================================================================
SND_SIDPLAY     = $09  ; (song:ARG0 1-based)  name in FIO.NAME     -> play SID file
SND_SIDSTOP     = $0A  ; ()                                        -> stop SID file playback
SND_MIDPLAY     = $0B  ; ()                   name in FIO.NAME     -> play MIDI file
SND_MIDSTOP     = $0C  ; ()                                        -> stop MIDI file playback
SND_SFLOAD      = $0D  ; ()                   name in FIO.NAME     -> load soundfont

; --- pointer-filename variants: ARG0 = name ptr, ARG1 byte0 = name len ---
SND_SIDPLAY_FILE = $0E ; (nameptr:ARG0, namelen:ARG1, song:ARG2)   -> copy name + play SID file
SND_MIDPLAY_FILE = $0F ; (nameptr:ARG0, namelen:ARG1)              -> copy name + play MIDI file
SND_SFLOAD_FILE  = $10 ; (nameptr:ARG0, namelen:ARG1)              -> copy name + load soundfont

; =====================================================================
; Hosted MML music engine (audio.s)
; =====================================================================
SND_MUSIC_SEQUENCE = $11 ; (voice:ARG0, mmlptr:ARG1, mmllen:ARG2)  -> queue MML for a voice
SND_MUSIC_PLAY     = $12 ; ()                                      -> start queued music
SND_MUSIC_STOP     = $13 ; ()                                      -> stop music
SND_MUSIC_TEMPO    = $14 ; (bpm:ARG0 u16)                          -> set music tempo (BPM)
SND_MUSIC_LOOP     = $15 ; (on:ARG0 0=off,nonzero=on)              -> enable/disable looping
SND_MUSIC_PRIORITY = $16 ; (pri:ARG0)                              -> set hosted MML priority

; =====================================================================
; Reporters (write LIB_RESULT byte0)
; =====================================================================
SND_STATUS        = $17  ; ()              -> RESULT = raw MUSIC_STATUS byte
SND_SFX_PLAYING   = $18  ; ()              -> RESULT = nonzero if a sound effect is active
SND_MUSIC_PLAYING = $19  ; ()              -> RESULT = 1 if music is playing, else 0
SND_MUSIC_NOTE    = $1A  ; (voice:ARG0 1..14) -> RESULT = current MIDI note, 0 if silent/range

; Highest implemented fn-id + 1. Gaps / ids >= SND_FN_COUNT -> LERR_NO_FN.
SND_FN_COUNT = $1B

.endif

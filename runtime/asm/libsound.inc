; libsound.inc — SOUND module (MODULE_ID_SOUND = $02) fn-id contract.
;
; The shared audio-services module. Phase B moves NovaLogo's simple SID tone/
; noise/volume commands out of its extension ROM into this module; it is the
; future home of the full audio NDK (SID voices, music engine, WTS).
;
; CALLING CONVENTION (caller side, see libabi.inc):
;   LIB_MOD_ID ($0300) = MODULE_ID_SOUND
;   LIB_FN_ID  ($0301) = one of the SND_* ids below
;   LIB_ARG0..3 = 32-bit LE arg cells; JSR LIB_LOADER_BAND
;   -> LIB_STATUS = LERR_OK or an error code

.ifndef LIBSOUND_INC
LIBSOUND_INC = 1

.include "libabi.inc"            ; MODULE_ID_SOUND, LIB_* mailbox cells, LERR_*

SND_TONE     = $00   ; (freq:ARG0 u16 SID word, dur:ARG1 frames) -> SID voice 0 sawtooth tone
SND_NOISE    = $01   ; (dur:ARG0 frames)                         -> SID voice 0 noise burst
SND_VOLUME   = $02   ; (vol:ARG0 0-15)                           -> SID master volume

; Highest implemented fn-id + 1. Gaps / ids >= SND_FN_COUNT -> LERR_NO_FN.
SND_FN_COUNT = $03

.endif

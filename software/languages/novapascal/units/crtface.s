; Turbo Crt sound facade over Nova's canonical SOUND module.

.include "libsound.inc"
.include "nova.inc"

.if .referenced(sound) .OR .referenced(nosound)
      .segment "CODE"

crt_sound_call:
      STA   LIB_FN_ID
      LDA   #MODULE_ID_SOUND
      STA   LIB_MOD_ID
      JMP   LIB_LOADER_BAND
.endif

.if .referenced(sound)
      .export sound
sound:
      STA   LIB_ARG0
      STX   LIB_ARG0+1
      STZ   LIB_ARG0+2
      STZ   LIB_ARG0+3
      LDA   #SND_TONE_HZ
      JMP   crt_sound_call
.endif

.if .referenced(nosound)
      .export nosound
nosound:
      LDA   #SND_TONE_STOP
      JMP   crt_sound_call
.endif

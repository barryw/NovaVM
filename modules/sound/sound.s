; sound.s — SOUND module (MODULE_ID_SOUND = $02). THIN lib_call wrappers over the
; NDK audio driver (runtime/asm/audio.s). The NDK owns all SID logic; this module
; only marshals the mailbox args into the NDK routine's inputs and JSRs it.

      .include "libabi.inc"
      .include "libmod.inc"
      .include "libsound.inc"
      .include "nova.inc"              ; NVR0L/H scratch
      .include "audio.inc"            ; AUDIO_VOLUME/AUDIO_VOICE pseudo-registers

      .segment "CODE"
      lib_module_header MODULE_ID_SOUND, LIB_ABI_VERSION, SND_FN_COUNT

; ===========================================================================
; NDOC documentation (extracted by tools/nmod_pack.py into sound.nmod).
; ===========================================================================
;@module SOUND
;@version 1.0
;@brief Shared audio services: SID tone/noise/volume (thin wrappers over NDK audio.s).
;
;@fn SND_TONE
;@brief Play a sawtooth tone on SID voice 0 for a number of frames.
;@arg freq u16 SID frequency word (ARG0)
;@arg dur u16 duration in video frames (ARG1)
;@ret void
;@effect Wraps the NDK audio_tone routine.
;@status LERR_OK
;
;@fn SND_NOISE
;@brief Play a noise burst on SID voice 0 for a number of frames.
;@arg dur u16 duration in video frames (ARG0)
;@ret void
;@effect Wraps the NDK audio_noise routine.
;@status LERR_OK
;
;@fn SND_VOLUME
;@brief Set the SID master volume.
;@arg vol u8 volume 0-15 (ARG0, clamped by the NDK)
;@ret void
;@effect Wraps the NDK audio_volume routine (AUDIO_VOICE=0 -> master).
;@status LERR_OK

; ---------------------------------------------------------------------------
; dispatch — fn-id router (RTS-trick). SND_FN_COUNT is small so fn*2 < 256.
; ---------------------------------------------------------------------------
dispatch:
      lda     LIB_FN_ID
      cmp     #SND_FN_COUNT
      bcs     @bad
      asl
      tax
      lda     snd_jtable+1,x
      pha
      lda     snd_jtable,x
      pha
      rts
@bad:
      lda     #LERR_NO_FN
      sta     LIB_STATUS
      rts

snd_jtable:
      .word   snd_tone-1               ; $00 SND_TONE
      .word   snd_noise-1              ; $01 SND_NOISE
      .word   snd_volume-1             ; $02 SND_VOLUME

; --- $00 SND_TONE: ARG0 = freq (u16), ARG1 = dur (frames) -> NDK audio_tone ---
snd_tone:
      LDA   LIB_ARG0+0
      STA   NVR0L                      ; freq lo -> audio_tone input
      LDA   LIB_ARG0+1
      STA   NVR0H                      ; freq hi
      LDA   LIB_ARG1+0                 ; duration (frames) -> A
      JSR   audio_tone
      LDA   #LERR_OK
      STA   LIB_STATUS
      RTS

; --- $01 SND_NOISE: ARG0 = dur (frames) -> NDK audio_noise ---
snd_noise:
      LDA   LIB_ARG0+0                 ; duration -> A
      JSR   audio_noise
      LDA   #LERR_OK
      STA   LIB_STATUS
      RTS

; --- $02 SND_VOLUME: ARG0 = vol (0-15) -> NDK audio_volume ---
snd_volume:
      LDA   LIB_ARG0+0
      STA   AUDIO_VOLUME               ; pseudo-register input (= FIO_SRCL)
      STZ   AUDIO_VOICE                ; = FIO_SRCH; 0 selects master volume
      JSR   audio_volume
      LDA   #LERR_OK
      STA   LIB_STATUS
      RTS

; ===========================================================================
; NDK driver bodies, included AFTER the wrappers so .referenced(audio_*) is true
; (selective emit) and audio_tone's JSR vgc_wait_frames resolves against vgc.s.
; ===========================================================================
      .include "audio.s"
      .include "vgc.s"

      .segment "VECTORS"
      .word   $C000, $C000, $C000      ; NMI, RESET, IRQ (module runs under SEI)

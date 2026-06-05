; sound.s — SOUND module (MODULE_ID_SOUND = $02), the shared audio-services
; library. Built once, staged to the XRAM shelf, paged into bank 1 on the first
; lib_call. Header "NL"/$02 at $C000 (lib_module_header); RTS-trick dispatch on
; LIB_FN_ID.
;
; Phase B: TONE/NOISE/VOLUME moved verbatim out of the NovaLogo extension ROM.
; The handlers poke SID registers (MMIO, survives the bank swap) and busy-wait on
; the VGC frame counter, so they run entirely within one lib_call.

      .include "libabi.inc"
      .include "libmod.inc"
      .include "libsound.inc"
      .include "nova.inc"              ; SID_BASE, VGC_FRAME

      .segment "CODE"
      lib_module_header MODULE_ID_SOUND, LIB_ABI_VERSION, SND_FN_COUNT

; ===========================================================================
; NDOC documentation (extracted by tools/nmod_pack.py into sound.nmod).
; ===========================================================================
;@module SOUND
;@version 1.0
;@brief Shared audio services: simple SID tone/noise/volume (the future audio NDK home).
;
;@fn SND_TONE
;@brief Play a sawtooth tone on SID voice 0 for a number of frames.
;@arg freq u16 SID frequency word (ARG0)
;@arg dur u16 duration in video frames (ARG1)
;@ret void
;@effect Sets master volume to max, gates voice 0 on (sawtooth), waits, gates off.
;@status LERR_OK
;
;@fn SND_NOISE
;@brief Play a noise burst on SID voice 0 for a number of frames.
;@arg dur u16 duration in video frames (ARG0)
;@ret void
;@status LERR_OK
;
;@fn SND_VOLUME
;@brief Set the SID master volume.
;@arg vol u8 volume 0-15 (ARG0, clamped)
;@ret void
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

; --- $00 SND_TONE: ARG0 = freq (u16), ARG1 = dur (frames) ---
snd_tone:
      LDA   #$0F
      STA   SID_BASE + $18             ; master volume max
      LDA   LIB_ARG0+0
      STA   SID_BASE + $00             ; freq lo
      LDA   LIB_ARG0+1
      STA   SID_BASE + $01             ; freq hi
      LDA   #$09
      STA   SID_BASE + $05             ; attack/decay
      LDA   #$A0
      STA   SID_BASE + $06             ; sustain/release
      LDA   #$21                       ; sawtooth + gate
      STA   SID_BASE + $04
      LDA   LIB_ARG1+0                 ; duration (frames)
      JSR   wait_frames
      LDA   #$20                       ; sawtooth, gate off
      STA   SID_BASE + $04
      LDA   #LERR_OK
      STA   LIB_STATUS
      RTS

; --- $01 SND_NOISE: ARG0 = dur (frames) ---
snd_noise:
      LDA   #$0F
      STA   SID_BASE + $18
      LDA   #$00
      STA   SID_BASE + $00
      LDA   #$20
      STA   SID_BASE + $01
      LDA   #$09
      STA   SID_BASE + $05
      LDA   #$A0
      STA   SID_BASE + $06
      LDA   #$81                       ; noise + gate
      STA   SID_BASE + $04
      LDA   LIB_ARG0+0                 ; duration (frames)
      JSR   wait_frames
      LDA   #$80                       ; noise, gate off
      STA   SID_BASE + $04
      LDA   #LERR_OK
      STA   LIB_STATUS
      RTS

; --- $02 SND_VOLUME: ARG0 = vol (0-15) ---
snd_volume:
      LDA   LIB_ARG0+0
      AND   #$0F
      STA   SID_BASE + $18
      LDA   #LERR_OK
      STA   LIB_STATUS
      RTS

; --- wait_frames — busy-wait A video frames on the VGC frame counter ---
wait_frames:
      TAX
      BEQ   @done
      LDA   VGC_FRAME
@wait:
      CMP   VGC_FRAME
      BEQ   @wait
      LDA   VGC_FRAME
      DEX
      BNE   @wait
@done:
      RTS

      .segment "VECTORS"
      .word   $C000, $C000, $C000      ; NMI, RESET, IRQ (module runs under SEI)

; sound.s — SOUND module (MODULE_ID_SOUND = $02). THIN lib_call wrappers over the
; NDK audio driver (runtime/asm/audio.s). The NDK owns all SID/music logic; this
; module only marshals the mailbox args into the NDK routine's inputs and JSRs it.
;
; Coverage: the full audio NDK — simple SID tone/noise/volume, fire-and-forget SID
; sound effects + scheduler, instrument definition, file-backed SID/MIDI/soundfont
; playback (FIO-named and pointer-named), the hosted MML music engine, and the
; music-status reporters. Per ModuleNdkContractTests this module's own code never
; pokes SID registers or busy-waits the VGC frame counter — it wraps NDK routines.

      .include "libabi.inc"
      .include "libmod.inc"
      .include "libsound.inc"
      .include "nova.inc"              ; NVR* scratch + FIO pseudo-register cells
      .include "audio.inc"            ; AUDIO_* pseudo-register aliases
      .include "fio.inc"             ; FIO_ARG_* pointer-filename pseudo-registers

      .segment "CODE"
      lib_module_header MODULE_ID_SOUND, LIB_ABI_VERSION, SND_FN_COUNT

; ===========================================================================
; NDOC documentation (extracted by tools/nmod_pack.py into sound.nmod).
;
; Each ;@ndk line maps the fn to the NDK routine it wraps; the human summary is
; single-sourced from that routine (feedback-ndk-source-of-truth). The build-time
; + CI drift guard requires the wrapper body to actually JSR that routine. Mailbox
; @arg bindings stay here — they are this wrapper's marshalling, not duplicated.
; ===========================================================================
;@module SOUND
;@version 1.0
;@brief Shared audio services: the full SID + music-engine NDK (thin wrappers over audio.s).
;
;@fn SND_TONE
;@ndk audio_tone
;@arg freq u16 SID frequency word (ARG0)
;@arg dur u16 duration in video frames (ARG1)
;@ret void
;@status LERR_OK
;
;@fn SND_NOISE
;@ndk audio_noise
;@arg dur u16 duration in video frames (ARG0)
;@ret void
;@status LERR_OK
;
;@fn SND_VOLUME
;@ndk audio_volume
;@brief Set the SID master volume.
;@arg vol u8 volume 0-15 (ARG0, clamped by the NDK)
;@ret void
;@status LERR_OK
; (local @brief override: the wrapper hardwires AUDIO_VOICE=0, so this fn is
;  master-only — narrower than the NDK audio_volume "master or per-voice" summary.)
;
;@fn SND_PLAY_SOUND
;@ndk audio_play_sound
;@arg note u8 MIDI note (ARG0)
;@arg dur u8 duration in 60 Hz frames (ARG1)
;@arg instr u8 instrument slot (ARG2)
;@ret void
;@status LERR_OK, LERR_AUDIO_FAIL
;
;@fn SND_SOUND
;@ndk audio_sound
;@arg note u8 MIDI note -> AUDIO.NOTE (ARG0)
;@arg dur u8 duration in 60 Hz frames -> AUDIO.DURATION (ARG1)
;@arg instr u8 instrument slot -> AUDIO.INSTRUMENT (ARG2)
;@ret void
;@status LERR_OK, LERR_AUDIO_FAIL
;
;@fn SND_SET_VOLUME
;@ndk audio_set_volume
;@arg vol u8 volume level (ARG0)
;@arg voice u8 voice selector, 0=master, 1..6=per-voice (ARG1)
;@ret void
;@status LERR_OK, LERR_AUDIO_FAIL
;
;@fn SND_INSTRUMENT
;@ndk audio_instrument
;@arg id u8 instrument slot -> AUDIO.INST_ID (ARG0)
;@arg wave u8 SID waveform/control byte -> AUDIO.WAVEFORM (ARG1)
;@arg attack u8 attack nibble -> AUDIO.ATTACK (ARG2)
;@arg decay u8 decay nibble -> AUDIO.DECAY (ARG3 byte0)
;@arg sustain u8 sustain nibble -> AUDIO.SUSTAIN (ARG3 byte1)
;@arg release u8 release nibble -> AUDIO.RELEASE (ARG3 byte2)
;@ret void
;@status LERR_OK, LERR_AUDIO_FAIL
;
;@fn SND_INIT
;@ndk audio_init
;@ret void
;@status LERR_OK
;
;@fn SND_TICK
;@ndk audio_tick
;@ret void
;@status LERR_OK
;
;@fn SND_SIDPLAY
;@ndk audio_sidplay
;@arg song u8 1-based subtune number -> AUDIO.NOTE (ARG0). Filename preset in FIO.NAME/FIO.NAMELEN.
;@ret void
;@status LERR_OK, LERR_AUDIO_FAIL
;
;@fn SND_SIDSTOP
;@ndk audio_sidstop
;@ret void
;@status LERR_OK, LERR_AUDIO_FAIL
;
;@fn SND_MIDPLAY
;@ndk audio_midplay
;@arg - - filename preset in FIO.NAME/FIO.NAMELEN
;@ret void
;@status LERR_OK, LERR_AUDIO_FAIL
;
;@fn SND_MIDSTOP
;@ndk audio_midstop
;@ret void
;@status LERR_OK, LERR_AUDIO_FAIL
;
;@fn SND_SFLOAD
;@ndk audio_sfload
;@arg - - soundfont filename preset in FIO.NAME/FIO.NAMELEN
;@ret void
;@status LERR_OK, LERR_AUDIO_FAIL
;
;@fn SND_SIDPLAY_FILE
;@ndk audio_sidplay_file
;@arg nameptr u16 pointer to filename bytes (ARG0)
;@arg namelen u8 filename length 1..63 (ARG1)
;@arg song u8 1-based subtune number -> AUDIO.NOTE (ARG2)
;@ret void
;@status LERR_OK, LERR_AUDIO_FAIL
;
;@fn SND_MIDPLAY_FILE
;@ndk audio_midplay_file
;@arg nameptr u16 pointer to filename bytes (ARG0)
;@arg namelen u8 filename length 1..63 (ARG1)
;@ret void
;@status LERR_OK, LERR_AUDIO_FAIL
;
;@fn SND_SFLOAD_FILE
;@ndk audio_sfload_file
;@arg nameptr u16 pointer to filename bytes (ARG0)
;@arg namelen u8 filename length 1..63 (ARG1)
;@ret void
;@status LERR_OK, LERR_AUDIO_FAIL
;
;@fn SND_MUSIC_SEQUENCE
;@ndk audio_music_sequence
;@arg voice u8 hosted MML voice number -> AUDIO.VOICE (ARG0)
;@arg mmlptr u16 pointer to MML bytes -> AUDIO.STRL/H (ARG1)
;@arg mmllen u8 MML byte length -> AUDIO.STRLEN (ARG2)
;@ret void
;@status LERR_OK, LERR_AUDIO_FAIL
;
;@fn SND_MUSIC_PLAY
;@ndk audio_music_play
;@ret void
;@status LERR_OK, LERR_AUDIO_FAIL
;
;@fn SND_MUSIC_STOP
;@ndk audio_music_stop
;@ret void
;@status LERR_OK, LERR_AUDIO_FAIL
;
;@fn SND_MUSIC_TEMPO
;@ndk audio_music_tempo
;@arg bpm u16 tempo, low byte -> AUDIO.NOTE, high byte -> AUDIO.DURATION (ARG0)
;@ret void
;@status LERR_OK, LERR_AUDIO_FAIL
;
;@fn SND_MUSIC_LOOP
;@ndk audio_music_loop
;@arg on u8 0=off, nonzero=on -> AUDIO.NOTE (ARG0)
;@ret void
;@status LERR_OK, LERR_AUDIO_FAIL
;
;@fn SND_MUSIC_PRIORITY
;@ndk audio_music_priority
;@arg pri u8 priority -> AUDIO.NOTE (ARG0)
;@ret void
;@status LERR_OK, LERR_AUDIO_FAIL
;
;@fn SND_STATUS
;@ndk audio_status
;@ret u8 raw MUSIC_STATUS byte (RESULT byte0)
;@status LERR_OK
;
;@fn SND_SFX_PLAYING
;@ndk audio_sfx_playing
;@ret u8 nonzero if a sound effect is active (RESULT byte0)
;@status LERR_OK
;
;@fn SND_MUSIC_PLAYING
;@ndk audio_music_playing
;@ret u8 1 if music is playing, else 0 (RESULT byte0)
;@status LERR_OK
;
;@fn SND_MUSIC_NOTE
;@ndk audio_music_note
;@arg voice u8 voice number 1..14 -> X (ARG0)
;@ret u8 current MIDI note, 0 if out of range or silent (RESULT byte0)
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
      .word   snd_play_sound-1         ; $03 SND_PLAY_SOUND
      .word   snd_sound-1              ; $04 SND_SOUND
      .word   snd_set_volume-1         ; $05 SND_SET_VOLUME
      .word   snd_instrument-1         ; $06 SND_INSTRUMENT
      .word   snd_init-1               ; $07 SND_INIT
      .word   snd_tick-1               ; $08 SND_TICK
      .word   snd_sidplay-1            ; $09 SND_SIDPLAY
      .word   snd_sidstop-1            ; $0A SND_SIDSTOP
      .word   snd_midplay-1            ; $0B SND_MIDPLAY
      .word   snd_midstop-1            ; $0C SND_MIDSTOP
      .word   snd_sfload-1             ; $0D SND_SFLOAD
      .word   snd_sidplay_file-1       ; $0E SND_SIDPLAY_FILE
      .word   snd_midplay_file-1       ; $0F SND_MIDPLAY_FILE
      .word   snd_sfload_file-1        ; $10 SND_SFLOAD_FILE
      .word   snd_music_sequence-1     ; $11 SND_MUSIC_SEQUENCE
      .word   snd_music_play-1         ; $12 SND_MUSIC_PLAY
      .word   snd_music_stop-1         ; $13 SND_MUSIC_STOP
      .word   snd_music_tempo-1        ; $14 SND_MUSIC_TEMPO
      .word   snd_music_loop-1         ; $15 SND_MUSIC_LOOP
      .word   snd_music_priority-1     ; $16 SND_MUSIC_PRIORITY
      .word   snd_status-1             ; $17 SND_STATUS
      .word   snd_sfx_playing-1        ; $18 SND_SFX_PLAYING
      .word   snd_music_playing-1      ; $19 SND_MUSIC_PLAYING
      .word   snd_music_note-1         ; $1A SND_MUSIC_NOTE

; ===========================================================================
; Shared epilogues. snd_finish_status maps the NDK A result (0=OK / nonzero=err)
; to LIB_STATUS and zeroes RESULT. snd_finish_result publishes the NDK A value to
; RESULT byte0 (reporters; always LERR_OK). snd_ok is the void epilogue.
; These are NOT ;@ndk-mapped, so the drift guard's per-wrapper JSR check is unaffected.
; ===========================================================================
snd_finish_status:
      cmp     #$00
      bne     @fail
      stz     LIB_RESULT
      stz     LIB_RESULT+1
      stz     LIB_RESULT+2
      stz     LIB_RESULT+3
      lda     #LERR_OK
      sta     LIB_STATUS
      rts
@fail:
      lda     #LERR_AUDIO_FAIL
      sta     LIB_STATUS
      rts

snd_finish_result:
      sta     LIB_RESULT
      stz     LIB_RESULT+1
      stz     LIB_RESULT+2
      stz     LIB_RESULT+3
      lda     #LERR_OK
      sta     LIB_STATUS
      rts

snd_ok:
      lda     #LERR_OK
      sta     LIB_STATUS
      rts

; ===========================================================================
; Simple SID effects.
; ===========================================================================

; --- $00 SND_TONE: ARG0 = freq (u16), ARG1 = dur (frames) -> NDK audio_tone ---
snd_tone:
      LDA   LIB_ARG0+0
      STA   NVR0L                      ; freq lo -> audio_tone input
      LDA   LIB_ARG0+1
      STA   NVR0H                      ; freq hi
      LDA   LIB_ARG1+0                 ; duration (frames) -> A
      JSR   audio_tone
      JMP   snd_ok

; --- $01 SND_NOISE: ARG0 = dur (frames) -> NDK audio_noise ---
snd_noise:
      LDA   LIB_ARG0+0                 ; duration -> A
      JSR   audio_noise
      JMP   snd_ok

; --- $02 SND_VOLUME: ARG0 = vol (0-15), master only -> NDK audio_volume ---
snd_volume:
      LDA   LIB_ARG0+0
      STA   AUDIO_VOLUME               ; pseudo-register input (= FIO_SRCL)
      STZ   AUDIO_VOICE                ; = FIO_SRCH; 0 selects master volume
      JSR   audio_volume
      JMP   snd_ok

; ===========================================================================
; Fire-and-forget SID sound effects + scheduler.
; ===========================================================================

; --- $03 SND_PLAY_SOUND: ARG0=note, ARG1=dur, ARG2=instr -> A/X/Y -> audio_play_sound ---
snd_play_sound:
      LDA   LIB_ARG0+0                 ; note  -> A
      LDX   LIB_ARG1+0                 ; dur   -> X
      LDY   LIB_ARG2+0                 ; instr -> Y
      JSR   audio_play_sound
      JMP   snd_finish_status

; --- $04 SND_SOUND: ARG0=note, ARG1=dur, ARG2=instr -> pseudo-regs -> audio_sound ---
snd_sound:
      LDA   LIB_ARG0+0
      STA   AUDIO_NOTE                 ; = FIO_SRCL
      LDA   LIB_ARG1+0
      STA   AUDIO_DURATION             ; = FIO_SRCH
      LDA   LIB_ARG2+0
      STA   AUDIO_INSTRUMENT           ; = FIO_ENDL
      JSR   audio_sound
      JMP   snd_finish_status

; --- $05 SND_SET_VOLUME: ARG0=vol, ARG1=voice -> A/X -> audio_set_volume ---
snd_set_volume:
      LDA   LIB_ARG0+0                 ; volume -> A
      LDX   LIB_ARG1+0                 ; voice  -> X (0=master)
      JSR   audio_set_volume
      JMP   snd_finish_status

; --- $06 SND_INSTRUMENT: ARG0=id, ARG1=wave, ARG2=attack, ARG3 bytes 0/1/2=d/s/r ---
snd_instrument:
      LDA   LIB_ARG0+0
      STA   AUDIO_INST_ID              ; = FIO_SRCL
      LDA   LIB_ARG1+0
      STA   AUDIO_WAVEFORM             ; = FIO_SRCH
      LDA   LIB_ARG2+0
      STA   AUDIO_ATTACK               ; = FIO_ENDL
      LDA   LIB_ARG3+0
      STA   AUDIO_DECAY                ; = FIO_ENDH
      LDA   LIB_ARG3+1
      STA   AUDIO_SUSTAIN              ; = FIO_SIZEL
      LDA   LIB_ARG3+2
      STA   AUDIO_RELEASE              ; = FIO_SIZEH
      JSR   audio_instrument
      JMP   snd_finish_status

; --- $07 SND_INIT: () -> NDK audio_init (no return contract) ---
snd_init:
      JSR   audio_init
      JMP   snd_ok

; --- $08 SND_TICK: () -> NDK audio_tick (no return contract) ---
snd_tick:
      JSR   audio_tick
      JMP   snd_ok

; ===========================================================================
; File-backed playback. The caller preloads FIO.NAME/FIO.NAMELEN (or uses the
; *_FILE pointer variants below). audio_sidplay reuses AUDIO.NOTE as the song #.
; ===========================================================================

; --- $09 SND_SIDPLAY: ARG0 = song (1-based) -> AUDIO.NOTE -> audio_sidplay ---
snd_sidplay:
      LDA   LIB_ARG0+0
      STA   AUDIO_NOTE                 ; = FIO_SRCL; 1-based song number
      JSR   audio_sidplay
      JMP   snd_finish_status

; --- $0A SND_SIDSTOP: () -> audio_sidstop ---
snd_sidstop:
      JSR   audio_sidstop
      JMP   snd_finish_status

; --- $0B SND_MIDPLAY: () name in FIO.NAME -> audio_midplay ---
snd_midplay:
      JSR   audio_midplay
      JMP   snd_finish_status

; --- $0C SND_MIDSTOP: () -> audio_midstop ---
snd_midstop:
      JSR   audio_midstop
      JMP   snd_finish_status

; --- $0D SND_SFLOAD: () soundfont in FIO.NAME -> audio_sfload ---
snd_sfload:
      JSR   audio_sfload
      JMP   snd_finish_status

; --- $0E SND_SIDPLAY_FILE: ARG0=nameptr, ARG1=namelen, ARG2=song -> audio_sidplay_file ---
snd_sidplay_file:
      JSR   snd_load_name_args         ; ARG0 ptr + ARG1 len -> FIO_ARG_NAME*
      LDA   LIB_ARG2+0
      STA   AUDIO_NOTE                 ; 1-based song number for the copied file
      JSR   audio_sidplay_file
      JMP   snd_finish_status

; --- $0F SND_MIDPLAY_FILE: ARG0=nameptr, ARG1=namelen -> audio_midplay_file ---
snd_midplay_file:
      JSR   snd_load_name_args
      JSR   audio_midplay_file
      JMP   snd_finish_status

; --- $10 SND_SFLOAD_FILE: ARG0=nameptr, ARG1=namelen -> audio_sfload_file ---
snd_sfload_file:
      JSR   snd_load_name_args
      JSR   audio_sfload_file
      JMP   snd_finish_status

; snd_load_name_args — marshal the pointer-filename ABI shared by the *_FILE fns:
;   ARG0 word -> FIO_ARG_NAMEPTR_L/H, ARG1 byte0 -> FIO_ARG_NAMELEN. (Not ;@ndk-
;   mapped, so the per-wrapper drift JSR check ignores it.)
snd_load_name_args:
      LDA   LIB_ARG0+0
      STA   FIO_ARG_NAMEPTR_L          ; = NVR4H
      LDA   LIB_ARG0+1
      STA   FIO_ARG_NAMEPTR_H          ; = NVR5L
      LDA   LIB_ARG1+0
      STA   FIO_ARG_NAMELEN            ; = NVR4L
      RTS

; ===========================================================================
; Hosted MML music engine.
; ===========================================================================

; --- $11 SND_MUSIC_SEQUENCE: ARG0=voice, ARG1=mmlptr, ARG2=mmllen -> audio_music_sequence ---
snd_music_sequence:
      LDA   LIB_ARG0+0
      STA   AUDIO_VOICE                ; = FIO_SRCH; hosted MML voice number
      LDA   LIB_ARG1+0
      STA   AUDIO_STRL                 ; = FIO_ENDL; MML pointer lo
      LDA   LIB_ARG1+1
      STA   AUDIO_STRH                 ; = FIO_ENDH; MML pointer hi
      LDA   LIB_ARG2+0
      STA   AUDIO_STRLEN               ; = FIO_NAMELEN; MML byte length
      JSR   audio_music_sequence
      JMP   snd_finish_status

; --- $12 SND_MUSIC_PLAY: () -> audio_music_play ---
snd_music_play:
      JSR   audio_music_play
      JMP   snd_finish_status

; --- $13 SND_MUSIC_STOP: () -> audio_music_stop ---
snd_music_stop:
      JSR   audio_music_stop
      JMP   snd_finish_status

; --- $14 SND_MUSIC_TEMPO: ARG0 = bpm (u16) -> AUDIO.NOTE/DURATION -> audio_music_tempo ---
snd_music_tempo:
      LDA   LIB_ARG0+0
      STA   AUDIO_NOTE                 ; = FIO_SRCL; BPM lo
      LDA   LIB_ARG0+1
      STA   AUDIO_DURATION             ; = FIO_SRCH; BPM hi
      JSR   audio_music_tempo
      JMP   snd_finish_status

; --- $15 SND_MUSIC_LOOP: ARG0 = on (0/nonzero) -> AUDIO.NOTE -> audio_music_loop ---
snd_music_loop:
      LDA   LIB_ARG0+0
      STA   AUDIO_NOTE                 ; = FIO_SRCL; 0=off, nonzero=on
      JSR   audio_music_loop
      JMP   snd_finish_status

; --- $16 SND_MUSIC_PRIORITY: ARG0 = pri -> AUDIO.NOTE -> audio_music_priority ---
; The host MPRI handler reads SIX consecutive FIO cells (FIO_SRCL..FIO_SIZEH) as a
; voice-priority list. This single-value fn carries only ARG0, so it MUST zero the
; other five cells — otherwise stale mailbox bytes (including FIO_END_LO, which the
; lib_call loader band writes during a page-in) would be read as bogus extra voices.
snd_music_priority:
      LDA   LIB_ARG0+0
      STA   AUDIO_NOTE                 ; = FIO_SRCL; priority value
      STZ   AUDIO_DURATION             ; = FIO_SRCH
      STZ   AUDIO_INSTRUMENT           ; = FIO_ENDL  (clobbered by page-in slot idx)
      STZ   AUDIO_DECAY                ; = FIO_ENDH
      STZ   AUDIO_SUSTAIN              ; = FIO_SIZEL
      STZ   AUDIO_RELEASE              ; = FIO_SIZEH
      JSR   audio_music_priority
      JMP   snd_finish_status

; ===========================================================================
; Reporters — publish the NDK A value to LIB_RESULT byte0 (always LERR_OK).
; ===========================================================================

; --- $17 SND_STATUS: () -> RESULT = raw MUSIC_STATUS -> audio_status ---
snd_status:
      JSR   audio_status
      JMP   snd_finish_result

; --- $18 SND_SFX_PLAYING: () -> RESULT = SFX active flag -> audio_sfx_playing ---
snd_sfx_playing:
      JSR   audio_sfx_playing
      JMP   snd_finish_result

; --- $19 SND_MUSIC_PLAYING: () -> RESULT = music playing (1/0) -> audio_music_playing ---
snd_music_playing:
      JSR   audio_music_playing
      JMP   snd_finish_result

; --- $1A SND_MUSIC_NOTE: ARG0 = voice (1..14) -> X -> audio_music_note -> RESULT ---
snd_music_note:
      LDX   LIB_ARG0+0                 ; voice number 1..14
      JSR   audio_music_note
      JMP   snd_finish_result

; ===========================================================================
; NDK driver bodies, included AFTER the wrappers so .referenced(audio_*) is true
; (selective emit) and audio_tone's JSR vgc_wait_frames resolves against vgc.s.
; audio.s pulls fio.s (fio_exec / fio_copy_name) for the file + music commands.
; ===========================================================================
      .include "audio.inc"
      .include "vgc.inc"

      .segment "VECTORS"
      .word   $C000, $C000, $C000      ; NMI, RESET, IRQ (module runs under SEI)

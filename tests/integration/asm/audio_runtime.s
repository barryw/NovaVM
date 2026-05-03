; Audio shared-runtime integration fixture.
;
; Loaded at $7000 by tests/integration/audio_asm.6502 and entered through
; BASIC SYS. The fixture includes the same lib/audio.inc/lib/audio.s library
; that standalone assembly programs are expected to reuse.

.setcpu "65c02"

FIO_NO_STREAMING = 1

.include "audio.inc"

RESULT_BASE = $2260

.segment "CODE"

start:
        jmp asm_audio_entry

asm_audio_entry:
        pha
        phx
        phy
        jsr asm_audio_test
        ply
        plx
        pla
        rts

asm_audio_test:
        jsr clear_results

        lda #12
        ldx #0
        jsr audio_set_volume
        sta RESULT_BASE
        lda FIO_STATUS
        sta RESULT_BASE + 1

        lda #8
        ldx #7
        jsr audio_set_volume
        sta RESULT_BASE + 12
        lda WTS_VOICE_BASE + WTS_VOICE_VOLUME
        sta RESULT_BASE + 13

        lda #0
        sta AUDIO_INST_ID
        lda #$21
        sta AUDIO_WAVEFORM
        stz AUDIO_ATTACK
        lda #9
        sta AUDIO_DECAY
        lda #12
        sta AUDIO_SUSTAIN
        lda #4
        sta AUDIO_RELEASE
        jsr audio_instrument
        sta RESULT_BASE + 2

        lda #60
        ldx #8
        ldy #0
        jsr audio_play_sound
        sta RESULT_BASE + 3

        lda #1
        sta AUDIO_VOICE
        lda #<mml_line
        sta AUDIO_STRL
        lda #>mml_line
        sta AUDIO_STRH
        lda #(mml_line_end - mml_line)
        sta AUDIO_STRLEN
        jsr audio_music_sequence
        sta RESULT_BASE + 4

        lda #120
        sta FIO_SRCL
        stz FIO_SRCH
        jsr audio_music_tempo
        sta RESULT_BASE + 5

        lda #1
        sta FIO_SRCL
        jsr audio_music_loop
        sta RESULT_BASE + 6

        lda #1
        sta FIO_SRCL
        lda #2
        sta FIO_SRCH
        lda #3
        sta FIO_ENDL
        lda #4
        sta FIO_ENDH
        lda #5
        sta FIO_SIZEL
        lda #6
        sta FIO_SIZEH
        jsr audio_music_priority
        sta RESULT_BASE + 7

        jsr audio_music_play
        sta RESULT_BASE + 8
        jsr audio_music_playing
        sta RESULT_BASE + 9

        jsr audio_music_stop
        sta RESULT_BASE + 10
        jsr audio_music_playing
        sta RESULT_BASE + 11

        lda #$AA
        sta RESULT_BASE + $0F
        rts

clear_results:
        ldx #$00
        lda #$00
@loop:
        sta RESULT_BASE,x
        inx
        cpx #$20
        bne @loop
        rts

mml_line:
        .byte "T120 I0 L4 C"
mml_line_end:

.include "audio.s"

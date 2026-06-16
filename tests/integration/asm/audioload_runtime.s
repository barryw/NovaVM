; AUDIO loading-poll integration fixture.
;
; Drives audio_sidplay against a bus double whose FIO accepts the play command
; and then reports MUSIC_STATUS_LOADING for a few reads before clearing. A
; file-backed play must wait for the host load to finish (poll the LOADING bit)
; before returning, instead of returning the instant the command is dispatched.
; AudioLoadingAssemblyRuntimeTests supplies the bus.

.setcpu "65c02"

.include "audio.inc"

RESULT = $2200

.segment "CODE"

start:
        jmp run

run:
        jsr audio_sidplay
        sta RESULT + 0          ; A = return code (0 = success)
        lda #$AA
        sta RESULT + 1
        rts

.include "audio.s"
.include "fio.s"

; test_music_status.s -- Music register monitor
; Polls and displays MusicStatus, per-voice notes, elapsed/total on screen.
; Run MIDPLAY or SIDPLAY from BASIC first, then SYS $9000 to observe.
;
; Load address: $9000   Invoke: SYS $9000
; Build: ca65 --cpu w65c02 -I ../../../runtime/asm test_music_status.s -o test_music_status.o
;        ld65 -C keyboard.cfg -o test_music_status.bin test_music_status.o vgc_wait.o

.include "novavm.inc"
.include "audio.inc"

.import vgc_wait_cmd

.segment "ZEROPAGE"
zp_frame:       .res 1
zp_tmp:         .res 1
zp_div_lo:      .res 1
zp_div_hi:      .res 1
zp_divisor:     .res 1
zp_quot_lo:     .res 1
zp_quot_hi:     .res 1
zp_remain:      .res 1

.segment "BSS"
save_mode:      .res 1
save_bgcol:     .res 1
save_fgcol:     .res 1
save_cursor:    .res 1
prev_status:    .res 1

.segment "HEADER"
    .byte $00, $90

.segment "CODE"

main:
    lda RegMode
    sta save_mode
    lda RegBgCol
    sta save_bgcol
    lda RegFgCol
    sta save_fgcol
    lda RegCursorEnable
    sta save_cursor

    lda #0
    sta RegMode
    sta RegBgCol
    stz RegCursorEnable
    lda #1
    sta RegFgCol

    jsr clear_text

    ; Static labels
    lda #1
    sta RegCursorY
    lda #20
    sta RegCursorX
    ldx #0
@title:
    lda str_title,x
    beq @draw_labels
    sta RegCharOut
    inx
    bra @title

@draw_labels:
    lda #4
    sta RegCursorY
    lda #2
    sta RegCursorX
    ldx #0
@l1:
    lda str_status_lbl,x
    beq @l2s
    sta RegCharOut
    inx
    bra @l1
@l2s:
    lda #6
    sta RegCursorY
    lda #2
    sta RegCursorX
    ldx #0
@l2:
    lda str_flags_lbl,x
    beq @l3s
    sta RegCharOut
    inx
    bra @l2
@l3s:
    lda #9
    sta RegCursorY
    lda #2
    sta RegCursorX
    ldx #0
@l3:
    lda str_voices_lbl,x
    beq @l4s
    sta RegCharOut
    inx
    bra @l3
@l4s:
    ; Voice number header
    lda #10
    sta RegCursorY
    lda #2
    sta RegCursorX
    ldx #0
@vhdr:
    cpx #14
    bcs @l5s
    lda #'V'
    sta RegCharOut
    txa
    clc
    adc #'1'
    cmp #'9'+1
    bcc @vhdr_ok
    ; voices 10-14: print as hex offset
    sec
    sbc #10
    clc
    adc #'A'
@vhdr_ok:
    sta RegCharOut
    lda #' '
    sta RegCharOut
    lda #' '
    sta RegCharOut
    inx
    bra @vhdr

@l5s:
    lda #13
    sta RegCursorY
    lda #2
    sta RegCursorX
    ldx #0
@l5:
    lda str_elapsed_lbl,x
    beq @l6s
    sta RegCharOut
    inx
    bra @l5
@l6s:
    lda #14
    sta RegCursorY
    lda #2
    sta RegCursorX
    ldx #0
@l6:
    lda str_total_lbl,x
    beq @l7s
    sta RegCharOut
    inx
    bra @l6
@l7s:
    lda #16
    sta RegCursorY
    lda #2
    sta RegCursorX
    ldx #0
@l7:
    lda str_time_lbl,x
    beq @exit_lbl
    sta RegCharOut
    inx
    bra @l7

@exit_lbl:
    lda #20
    sta RegCursorY
    lda #24
    sta RegCursorX
    ldx #0
@elbl:
    lda str_exit,x
    beq @init
    sta RegCharOut
    inx
    bra @elbl

@init:
    stz prev_status
    lda RegStatus
    sta zp_frame

@loop:
@wv:
    lda RegStatus
    cmp zp_frame
    beq @wv
    sta zp_frame

    ; --- MusicStatus byte ---
    lda #4
    sta RegCursorY
    lda #20
    sta RegCursorX
    lda MusicStatus
    jsr print_hex

    ; --- Decoded flags ---
    lda #6
    sta RegCursorY
    lda #20
    sta RegCursorX
    lda MusicStatus
    and #AUDIO_STATUS_SFX
    beq @no_sfx
    lda #'S'
    sta RegCharOut
    lda #'F'
    sta RegCharOut
    lda #'X'
    sta RegCharOut
    lda #' '
    sta RegCharOut
    bra @chk_music
@no_sfx:
    lda #'.'
    sta RegCharOut
    lda #'.'
    sta RegCharOut
    lda #'.'
    sta RegCharOut
    lda #' '
    sta RegCharOut
@chk_music:
    lda MusicStatus
    and #AUDIO_STATUS_MUSIC
    beq @no_music
    lda #'M'
    sta RegCharOut
    lda #'U'
    sta RegCharOut
    lda #'S'
    sta RegCharOut
    lda #' '
    sta RegCharOut
    bra @chk_sid
@no_music:
    lda #'.'
    sta RegCharOut
    lda #'.'
    sta RegCharOut
    lda #'.'
    sta RegCharOut
    lda #' '
    sta RegCharOut
@chk_sid:
    lda MusicStatus
    and #AUDIO_STATUS_SID
    beq @no_sid
    lda #'S'
    sta RegCharOut
    lda #'I'
    sta RegCharOut
    lda #'D'
    sta RegCharOut
    lda #' '
    sta RegCharOut
    bra @chk_wts
@no_sid:
    lda #'.'
    sta RegCharOut
    lda #'.'
    sta RegCharOut
    lda #'.'
    sta RegCharOut
    lda #' '
    sta RegCharOut
@chk_wts:
    lda MusicStatus
    and #AUDIO_STATUS_WTS
    beq @no_wts
    lda #'W'
    sta RegCharOut
    lda #'T'
    sta RegCharOut
    lda #'S'
    sta RegCharOut
    bra @show_voices
@no_wts:
    lda #'.'
    sta RegCharOut
    lda #'.'
    sta RegCharOut
    lda #'.'
    sta RegCharOut

@show_voices:
    ; --- Per-voice notes ---
    lda #11
    sta RegCursorY
    lda #2
    sta RegCursorX
    ldx #0
@vnote:
    cpx #14
    bcs @show_elapsed
    lda MusicNote1,x
    phx
    jsr print_hex
    lda #' '
    sta RegCharOut
    lda #' '
    sta RegCharOut
    plx
    inx
    bra @vnote

@show_elapsed:
    ; --- Elapsed frames (raw hex) ---
    lda #13
    sta RegCursorY
    lda #20
    sta RegCursorX
    lda MusicElapsedH
    jsr print_hex
    lda MusicElapsedL
    jsr print_hex

    ; --- Total frames (raw hex) ---
    lda #14
    sta RegCursorY
    lda #20
    sta RegCursorX
    lda MusicTotalH
    jsr print_hex
    lda MusicTotalL
    jsr print_hex

    ; --- Elapsed as MM:SS ---
    lda #16
    sta RegCursorY
    lda #20
    sta RegCursorX
    lda MusicElapsedL
    sta zp_div_lo
    lda MusicElapsedH
    sta zp_div_hi
    lda #60
    sta zp_divisor
    jsr div16_8
    lda zp_quot_lo
    sta zp_div_lo
    lda zp_quot_hi
    sta zp_div_hi
    lda #60
    sta zp_divisor
    jsr div16_8
    lda zp_quot_lo
    jsr print_dec2
    lda #':'
    sta RegCharOut
    lda zp_remain
    jsr print_dec2
    lda #' '
    sta RegCharOut
    lda #'/'
    sta RegCharOut
    lda #' '
    sta RegCharOut
    lda MusicTotalL
    sta zp_div_lo
    lda MusicTotalH
    sta zp_div_hi
    lda #60
    sta zp_divisor
    jsr div16_8
    lda zp_quot_lo
    sta zp_div_lo
    lda zp_quot_hi
    sta zp_div_hi
    lda #60
    sta zp_divisor
    jsr div16_8
    lda zp_quot_lo
    jsr print_dec2
    lda #':'
    sta RegCharOut
    lda zp_remain
    jsr print_dec2

    ; Check exit key
    lda RegCharIn
    bne @chk_key
    jmp @loop
@chk_key:
    cmp #$1B
    beq @exit
    cmp #'Q'
    beq @exit
    cmp #'q'
    beq @exit
    jmp @loop

@exit:
    lda save_mode
    sta RegMode
    lda save_bgcol
    sta RegBgCol
    lda save_fgcol
    sta RegFgCol
    lda save_cursor
    sta RegCursorEnable
    jsr clear_text
    rts

; 16/8 unsigned division
div16_8:
    stz zp_remain
    ldx #16
@d8:
    asl zp_div_lo
    rol zp_div_hi
    rol zp_remain
    lda zp_remain
    cmp zp_divisor
    bcc @d8s
    sbc zp_divisor
    sta zp_remain
    inc zp_div_lo
@d8s:
    dex
    bne @d8
    lda zp_div_lo
    sta zp_quot_lo
    lda zp_div_hi
    sta zp_quot_hi
    rts

print_dec2:
    ldx #0
@tens:
    cmp #10
    bcc @out
    sec
    sbc #10
    inx
    bra @tens
@out:
    pha
    txa
    clc
    adc #'0'
    sta RegCharOut
    pla
    clc
    adc #'0'
    sta RegCharOut
    rts

print_hex:
    pha
    lsr
    lsr
    lsr
    lsr
    tax
    lda hex_chars,x
    sta RegCharOut
    pla
    and #$0F
    tax
    lda hex_chars,x
    sta RegCharOut
    rts

clear_text:
    ldy #0
@row:
    sty RegCursorY
    ldx #0
@col:
    stx RegCursorX
    lda #' '
    sta RegCharOut
    inx
    cpx #80
    bcc @col
    iny
    cpy #50
    bcc @row
    stz RegCursorX
    stz RegCursorY
    rts

.segment "RODATA"

hex_chars: .byte "0123456789ABCDEF"
str_title:      .byte "MUSIC STATUS MONITOR", 0
str_status_lbl: .byte "STATUS BYTE:    ", 0
str_flags_lbl:  .byte "DECODED FLAGS:  ", 0
str_voices_lbl: .byte "VOICE NOTES (MIDI):", 0
str_elapsed_lbl:.byte "ELAPSED (FRAMES):  ", 0
str_total_lbl:  .byte "TOTAL (FRAMES):    ", 0
str_time_lbl:   .byte "ELAPSED (MM:SS):   ", 0
str_exit:       .byte "PRESS ESC/Q TO EXIT", 0

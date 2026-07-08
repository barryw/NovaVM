; Floating musical notes — ambient background for the music browser, drawn with
; the NDK sprite API (nova.lib). Eight eighth-note sprites drift slowly UP at
; priority BEHIND the text, swaying like leaves; each respawns at the bottom in a
; fresh column on reaching the top. Every VGC command is followed by mus_cmdwait
; (the browser's command-FIFO drain) — the proven board-pacing pattern.

.import sprite_row, sprite_pos, sprite_enable, sprite_priority, sprite_set_shape

NUM_NOTES = 8
NOTE_TOP  = 4
NOTE_BOT  = 184

.segment "CODE"

; init_notes -- shape slot 0 + enable N sprites (shape 0, priority BEHIND). Seeds
; positions but does NOT place the sprites (update_notes does, next frame).
init_notes:
    jsr load_note_shape
    ldx #NUM_NOTES-1
@seed:
    stx note_row
    lda note_x0,x
    sta note_x,x
    lda note_y0,x
    sta note_y,x
    lda note_ph0,x
    sta note_phase,x
    lda #0
    jsr sprite_set_shape        ; X=sprite, A=shape 0
    jsr mus_cmdwait
    ldx note_row
    stx RegP0
    stz RegP1                   ; priority 0 = behind text
    jsr sprite_priority
    jsr mus_cmdwait
    ldx note_row
    stx RegP0
    jsr sprite_enable
    jsr mus_cmdwait
    ldx note_row
    dex
    bpl @seed
    lda #$5A
    sta note_rng
    rts

; load_note_shape -- 16x16 note bitmap into shape slot 0, one paced row at a time.
load_note_shape:
    stz note_row
@row:
    lda note_row
    sta RegP1
    stz RegP0
    asl
    asl
    asl
    tay
    ldx #0
@byte:
    lda note_shape,y
    sta RegP0+2,x
    iny
    inx
    cpx #8
    bne @byte
    jsr sprite_row
    jsr mus_cmdwait
    inc note_row
    lda note_row
    cmp #16
    bne @row
    rts

; update_notes -- one drift frame for every note (call once per browser frame).
update_notes:
    ldx #NUM_NOTES-1
@up:
    stx note_row
    dec note_y,x
    lda note_y,x
    cmp #NOTE_TOP
    bcs @sway
    lda #NOTE_BOT
    sta note_y,x
    jsr note_rand
    ldx note_row
    sta note_x,x
@sway:
    inc note_phase,x
    lda note_phase,x
    lsr
    lsr
    and #31
    tay
    lda sine_tab,y
    clc
    adc note_x,x
    sec
    sbc #10
    ldx note_row
    stx RegP0
    sta RegP1
    stz RegP2
    lda note_y,x
    sta RegP3
    jsr sprite_pos
    jsr mus_cmdwait
    ldx note_row
    dex
    bpl @up
    rts

; note_rand -- 8-bit xorshift; A = a column in 12..235.
note_rand:
    lda note_rng
    asl
    bcc @nofb
    eor #$1D
@nofb:
    sta note_rng
    and #$DF
    clc
    adc #12
    rts

.segment "RODATA"
note_shape:
    .byte $00, $00, $00, $00, $00, $00, $00, $00
    .byte $00, $00, $00, $00, $00, $00, $00, $00
    .byte $00, $00, $00, $06, $66, $66, $66, $00
    .byte $00, $00, $00, $06, $66, $66, $60, $00
    .byte $00, $00, $00, $06, $66, $66, $00, $00
    .byte $00, $00, $00, $06, $66, $60, $00, $00
    .byte $00, $00, $00, $06, $66, $00, $00, $00
    .byte $00, $00, $00, $06, $60, $00, $00, $00
    .byte $00, $00, $00, $06, $60, $00, $00, $00
    .byte $00, $00, $00, $06, $60, $00, $00, $00
    .byte $00, $06, $66, $66, $60, $00, $00, $00
    .byte $00, $66, $66, $66, $60, $00, $00, $00
    .byte $00, $66, $66, $66, $60, $00, $00, $00
    .byte $00, $66, $66, $66, $00, $00, $00, $00
    .byte $00, $06, $66, $60, $00, $00, $00, $00
    .byte $00, $00, $00, $00, $00, $00, $00, $00
sine_tab:
    .byte 10,12,14,16,17,18,19,20,20,20,19,18,17,16,14,12
    .byte 10, 8, 6, 4, 3, 2, 1, 0, 0, 0, 1, 2, 3, 4, 6, 8
note_x0:  .byte 30,110,190,250, 60,150,210, 20
note_y0:  .byte 184,150,120,175, 90, 60,140,100
note_ph0: .byte  0, 20, 40, 60, 80,100,120, 30

.segment "BSS"
note_row:   .res 1
note_x:     .res NUM_NOTES
note_y:     .res NUM_NOTES
note_phase: .res NUM_NOTES
note_rng:   .res 1

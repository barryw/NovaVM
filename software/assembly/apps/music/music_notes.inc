; Floating musical notes -- ambient background for the music browser, drawn with
; the NDK sprite API (nova.lib). Eight eighth-note sprites FALL from behind the
; top border to the bottom, each a different colour and a different speed, swaying
; like leaves; each respawns at the top in a fresh column on reaching the bottom.
; Priority BEHIND the text. Every VGC command is paced with mus_cmdwait.
;
; Colour without eight separate bitmaps: one shape MASK (on-pixels = $F nibbles)
; is AND'd with C*$11 to stamp the note in colour C into shape slot per note.
; Fall speed is 8.8 fixed point so speeds can be fractional and smooth.

.import sprite_row, sprite_pos, sprite_enable, sprite_priority, sprite_set_shape

NUM_NOTES = 8
NOTE_TOP  = 0
NOTE_BOT  = 200

.segment "CODE"

; init_notes -- build 8 colour shapes, then set up 8 sprites (each its own colour
; slot, priority BEHIND, enabled) and seed their fall state.
init_notes:
    jsr load_note_shapes
    ldx #NUM_NOTES-1
@seed:
    stx note_row
    lda note_x0,x
    sta note_x,x
    lda note_ph0,x
    sta note_phase,x
    lda #0
    sta note_yl,x
    lda note_y0h,x
    sta note_yh,x
    txa                         ; sprite i uses shape slot i (its colour)
    jsr sprite_set_shape        ; X=sprite, A=shape
    jsr mus_cmdwait
    ldx note_row
    stx RegP0
    stz RegP1                   ; priority 0 = behind the text
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

; load_note_shapes -- stamp the note MASK in each note's colour into shape slots
; 0..NUM_NOTES-1, one paced row at a time.
load_note_shapes:
    ldx #0
@slot:
    stx note_slot
    lda note_col,x
    sta note_cc
    asl
    asl
    asl
    asl
    ora note_cc                 ; cc = C | C<<4  (both nibbles = colour)
    sta note_cc
    stz note_row
@row:
    lda note_slot
    sta RegP0                   ; shape slot
    lda note_row
    sta RegP1                   ; row
    asl
    asl
    asl
    tay                         ; Y = row*8 into the mask
    ldx #0
@byte:
    lda note_mask,y
    and note_cc                 ; mask -> colour
    sta RegP0+2,x               ; P2..P9
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
    ldx note_slot
    inx
    cpx #NUM_NOTES
    bne @slot
    rts

; update_notes -- one fall frame for every note (call once per browser frame).
update_notes:
    ldx #NUM_NOTES-1
@up:
    stx note_row
    clc                         ; note_y (8.8) += note_spd (8.8)
    lda note_yl,x
    adc note_spd_lo,x
    sta note_yl,x
    lda note_yh,x
    adc note_spd_hi,x
    sta note_yh,x
    cmp #NOTE_BOT
    bcc @sway                   ; still falling
    stz note_yl,x               ; hit the bottom -> respawn at the top
    lda #NOTE_TOP
    sta note_yh,x
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
    sbc #10                     ; centre the sway on the base column
    ldx note_row
    stx RegP0
    sta RegP1                   ; x low
    stz RegP2                   ; x high (columns stay < 256)
    lda note_yh,x
    sta RegP3                   ; y = integer part
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
; note MASK: on-pixels = $F nibbles (AND with colour to stamp)
note_mask:
    .byte $00,$00,$00,$00,$00,$00,$00,$00
    .byte $00,$00,$00,$00,$00,$00,$00,$00
    .byte $00,$00,$00,$0F,$FF,$FF,$FF,$00
    .byte $00,$00,$00,$0F,$FF,$FF,$F0,$00
    .byte $00,$00,$00,$0F,$FF,$FF,$00,$00
    .byte $00,$00,$00,$0F,$FF,$F0,$00,$00
    .byte $00,$00,$00,$0F,$FF,$00,$00,$00
    .byte $00,$00,$00,$0F,$F0,$00,$00,$00
    .byte $00,$00,$00,$0F,$F0,$00,$00,$00
    .byte $00,$00,$00,$0F,$F0,$00,$00,$00
    .byte $00,$0F,$FF,$FF,$F0,$00,$00,$00
    .byte $00,$FF,$FF,$FF,$F0,$00,$00,$00
    .byte $00,$FF,$FF,$FF,$F0,$00,$00,$00
    .byte $00,$FF,$FF,$FF,$00,$00,$00,$00
    .byte $00,$0F,$FF,$F0,$00,$00,$00,$00
    .byte $00,$00,$00,$00,$00,$00,$00,$00
sine_tab:
    .byte 10,12,14,16,17,18,19,20,20,20,19,18,17,16,14,12
    .byte 10, 8, 6, 4, 3, 2, 1, 0, 0, 0, 1, 2, 3, 4, 6, 8
; per-note colour (all non-black): red, cyan, purple, green, yellow, orange, lred, lblue
note_col:    .byte 2, 3, 4, 5, 7, 8, 10, 14
; per-note fall speed, 8.8 px/frame: .75 1.25 .5 1.0 1.375 .625 1.125 .875
note_spd_lo: .byte $C0,$40,$80,$00,$60,$A0,$20,$E0
note_spd_hi: .byte $00,$01,$00,$01,$01,$00,$01,$00
note_x0:     .byte 30, 90,150,210, 60,120,180,240
note_y0h:    .byte  0, 24, 48, 72, 96,120,144,168
note_ph0:    .byte  0, 20, 40, 60, 80,100,120, 30

.segment "BSS"
note_row:   .res 1
note_slot:  .res 1
note_cc:    .res 1
note_x:     .res NUM_NOTES
note_yl:    .res NUM_NOTES
note_yh:    .res NUM_NOTES
note_phase: .res NUM_NOTES
note_rng:   .res 1

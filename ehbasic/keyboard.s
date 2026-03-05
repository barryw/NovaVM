; keyboard.s -- Piano keyboard visualizer for NovaVM
; Displays an 8-octave piano keyboard and animates note indicators
; using sprites while music plays via the SID/MIDI engine.
;
; Load address: $9000   Invoke: SYS $9000
; Assembler:    ca65 --cpu 65c02
; Linker:       ld65 -C keyboard.cfg

.include "novavm.inc"

; ---------------------------------------------------------------------------
; Layout constants
; ---------------------------------------------------------------------------
KW          = 11                ; white key width (pixels)
KH          = 40                ; white key height
BW          = 7                 ; black key width
BH          = 24                ; black key height
KX          = 6                 ; keyboard X origin
Y1          = 36                ; upper row Y (octaves 0-3)
Y2          = 90                ; lower row Y (octaves 4-7)
KEYS_PER_OCT = 7                ; white keys per octave
KEYS_PER_ROW = 28               ; white keys per row (4 octaves)
ROW_WIDTH   = KEYS_PER_ROW * KW ; 308 pixels
MIDI_BASE   = 12                ; C0 = MIDI note 12
NUM_VOICES  = 6
TRANS       = $0F               ; transparent color index
BAR_X       = 6                 ; progress bar X
BAR_Y       = 145               ; progress bar Y
BAR_W       = 308               ; progress bar max fill width
BAR_H       = 6                 ; progress bar height
FPS         = 60                ; frames per second

; Palette color indices
COL_BLACK   = 0
COL_WHITE   = 1
COL_RED     = 2
COL_CYAN    = 3
COL_GREEN   = 5
COL_BLUE    = 6
COL_YELLOW  = 7
COL_ORANGE  = 8
COL_DGRAY   = 11
COL_LGRAY   = 15

; ---------------------------------------------------------------------------
; Zero page ($40-$7F -- safe user range)
; ---------------------------------------------------------------------------
.segment "ZEROPAGE"

zp_tmp:         .res 1
zp_tmp2:        .res 1
zp_voice:       .res 1          ; current voice 0-5
zp_note:        .res 1          ; current MIDI note
zp_octave:      .res 1          ; octave 0-7
zp_pitch:       .res 1          ; pitch class 0-11
zp_row:         .res 1          ; keyboard row 0=upper 1=lower
zp_key_x_lo:    .res 1          ; key X lo
zp_key_x_hi:    .res 1          ; key X hi
zp_key_y:       .res 1          ; key Y
zp_is_black:    .res 1          ; 1=black key
zp_shape:       .res 1          ; sprite shape slot
zp_spr_row:     .res 1          ; row counter for shape def
zp_spr_shape:   .res 1          ; shape being defined
zp_spr_color:   .res 1          ; voice color for shape
zp_str_ptr:     .res 2          ; string pointer
zp_prev_bar:    .res 2          ; previous bar fill width
zp_last_frame:  .res 1          ; last frame counter for vsync
zp_oct_in_row:  .res 1          ; octave within current row
; Division
zp_div_lo:      .res 1
zp_div_hi:      .res 1
zp_divisor:     .res 1
zp_quot_lo:     .res 1
zp_quot_hi:     .res 1
zp_remain:      .res 1
; Multiply
zp_mul_a_lo:    .res 1
zp_mul_a_hi:    .res 1
zp_mul_b:       .res 1          ; 8-bit multiplicand (for simple mul)
zp_mul_r_lo:    .res 1
zp_mul_r_hi:    .res 1
; 32-bit division for progress bar
zp_dvd0:        .res 1          ; dividend bytes 0-3 (LSB first)
zp_dvd1:        .res 1
zp_dvd2:        .res 1
zp_dvd3:        .res 1
zp_dvs_lo:      .res 1          ; 16-bit divisor
zp_dvs_hi:      .res 1
zp_q_lo:        .res 1          ; 16-bit quotient
zp_q_hi:        .res 1
zp_rem16_lo:    .res 1
zp_rem16_hi:    .res 1

; ---------------------------------------------------------------------------
; BSS
; ---------------------------------------------------------------------------
.segment "BSS"

prev_notes:     .res NUM_VOICES
save_mode:      .res 1
save_bgcol:     .res 1
save_fgcol:     .res 1
save_cursor:    .res 1

; ---------------------------------------------------------------------------
; Header -- 2-byte load address
; ---------------------------------------------------------------------------
.segment "HEADER"
    .byte $00, $90

; ---------------------------------------------------------------------------
; CODE
; ---------------------------------------------------------------------------
.segment "CODE"

; =====================================================================
; Entry point
; =====================================================================
main:
    ; Save VGC state
    lda RegMode
    sta save_mode
    lda RegBgCol
    sta save_bgcol
    lda RegFgCol
    sta save_fgcol
    lda RegCursorEnable
    sta save_cursor

    ; Clear previous notes
    lda #0
    ldx #NUM_VOICES - 1
@clr_prev:
    sta prev_notes,x
    dex
    bpl @clr_prev

    stz zp_prev_bar
    stz zp_prev_bar+1

    ; Set mode 1 (gfx over text)
    lda #1
    sta RegMode
    lda #COL_BLACK
    sta RegBgCol
    lda #COL_WHITE
    sta RegFgCol
    stz RegCursorEnable

    ; Clear graphics
    lda #CmdGcls
    sta RegCmd

    ; Clear text
    jsr clear_text

    ; Draw keyboards
    jsr draw_keyboards

    ; Draw octave labels
    jsr draw_labels

    ; Define sprite shapes
    jsr define_shapes

    ; Init sprites 0-5: disabled, priority in-front, trans=15
    ldx #0
    ldy #0
@init_spr:
    cpy #NUM_VOICES
    bcs @init_spr_done
    lda #SpritePriInFront
    sta SpriteRegBase + SprRegPriority,x
    lda #TRANS
    sta SpriteRegBase + SprRegTransColor,x
    stz SpriteRegBase + SprRegFlags,x
    txa
    clc
    adc #SpriteRegStride
    tax
    iny
    bra @init_spr
@init_spr_done:

    ; Draw chrome
    jsr draw_chrome

    ; Snapshot frame counter
    lda RegStatus
    sta zp_last_frame

    ; ---- Main loop ----
@main_loop:
    jsr wait_vsync
    jsr update_voices
    jsr update_progress

    ; Check keypress
    jsr V_INPT
    bcs @exit

    ; Check music playing (bit 1)
    lda MusicStatus
    and #$02
    bne @main_loop

@exit:
    ; Disable sprites
    ldx #0
    ldy #0
@dis_spr:
    cpy #NUM_VOICES
    bcs @dis_done
    stz SpriteRegBase + SprRegFlags,x
    txa
    clc
    adc #SpriteRegStride
    tax
    iny
    bra @dis_spr
@dis_done:

    ; Clear graphics
    lda #CmdGcls
    sta RegCmd

    ; Restore state
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

; =====================================================================
; Wait for next frame
; =====================================================================
wait_vsync:
@wv:
    lda RegStatus
    cmp zp_last_frame
    beq @wv
    sta zp_last_frame
    rts

; =====================================================================
; Clear text layer (spaces everywhere)
; =====================================================================
clear_text:
    ldy #0
@ct_row:
    sty RegCursorY
    ldx #0
@ct_col:
    stx RegCursorX
    lda #' '
    sta RegCharOut
    inx
    cpx #80
    bcc @ct_col
    iny
    cpy #25
    bcc @ct_row
    rts

; =====================================================================
; Draw both keyboard rows
; =====================================================================
draw_keyboards:
    lda #0
    sta zp_octave
    lda #Y1
    sta zp_key_y
    jsr draw_one_row

    lda #4
    sta zp_octave
    lda #Y2
    sta zp_key_y
    jsr draw_one_row
    rts

; =====================================================================
; Draw one keyboard row (4 octaves = 28 white keys)
; Input: zp_key_y, zp_octave (starting octave for this row)
; =====================================================================
draw_one_row:
    ; Step 1: white key fill
    lda #COL_WHITE
    sta RegP0
    lda #CmdGcolor
    sta RegCmd

    lda #<KX
    sta RegP0
    lda #>KX
    sta RegP1
    lda zp_key_y
    sta RegP2
    stz RegP3
    lda #<(KX + ROW_WIDTH - 1)
    sta RegP4
    lda #>(KX + ROW_WIDTH - 1)
    sta RegP5
    lda zp_key_y
    clc
    adc #KH - 1
    sta RegP6
    stz RegP7
    lda #CmdFill
    sta RegCmd

    ; Step 2: vertical separator lines
    lda #COL_DGRAY
    sta RegP0
    lda #CmdGcolor
    sta RegCmd

    ; Start at x = KX + KW (first separator)
    lda #<(KX + KW)
    sta zp_key_x_lo
    lda #>(KX + KW)
    sta zp_key_x_hi
    ldx #1
@sep_loop:
    cpx #KEYS_PER_ROW
    bcs @sep_done
    phx

    lda zp_key_x_lo
    sta RegP0
    sta RegP4
    lda zp_key_x_hi
    sta RegP1
    sta RegP5
    lda zp_key_y
    sta RegP2
    stz RegP3
    lda zp_key_y
    clc
    adc #KH - 1
    sta RegP6
    stz RegP7
    lda #CmdLine
    sta RegCmd

    lda zp_key_x_lo
    clc
    adc #KW
    sta zp_key_x_lo
    lda zp_key_x_hi
    adc #0
    sta zp_key_x_hi

    plx
    inx
    bra @sep_loop
@sep_done:

    ; Step 3: outline
    ; Top line
    lda #<KX
    sta RegP0
    lda #>KX
    sta RegP1
    lda zp_key_y
    sta RegP2
    stz RegP3
    lda #<(KX + ROW_WIDTH - 1)
    sta RegP4
    lda #>(KX + ROW_WIDTH - 1)
    sta RegP5
    lda zp_key_y
    sta RegP6
    stz RegP7
    lda #CmdLine
    sta RegCmd

    ; Bottom line
    lda zp_key_y
    clc
    adc #KH - 1
    sta zp_tmp
    lda #<KX
    sta RegP0
    lda #>KX
    sta RegP1
    lda zp_tmp
    sta RegP2
    stz RegP3
    lda #<(KX + ROW_WIDTH - 1)
    sta RegP4
    lda #>(KX + ROW_WIDTH - 1)
    sta RegP5
    lda zp_tmp
    sta RegP6
    stz RegP7
    lda #CmdLine
    sta RegCmd

    ; Left edge
    lda #<KX
    sta RegP0
    lda #>KX
    sta RegP1
    lda zp_key_y
    sta RegP2
    stz RegP3
    lda #<KX
    sta RegP4
    lda #>KX
    sta RegP5
    lda zp_tmp
    sta RegP6
    stz RegP7
    lda #CmdLine
    sta RegCmd

    ; Right edge
    lda #<(KX + ROW_WIDTH)
    sta RegP0
    lda #>(KX + ROW_WIDTH)
    sta RegP1
    lda zp_key_y
    sta RegP2
    stz RegP3
    lda #<(KX + ROW_WIDTH)
    sta RegP4
    lda #>(KX + ROW_WIDTH)
    sta RegP5
    lda zp_tmp
    sta RegP6
    stz RegP7
    lda #CmdLine
    sta RegCmd

    ; Step 4: black keys
    lda #COL_BLACK
    sta RegP0
    lda #CmdGcolor
    sta RegCmd

    stz zp_oct_in_row           ; 0-3
@bk_oct:
    lda zp_oct_in_row
    cmp #4
    bcs @bk_done

    ldx #0                      ; black key index 0-4
@bk_key:
    cpx #5
    bcs @bk_next_oct
    phx

    ; global_wk = oct_in_row * 7 + black_key_wk[x]
    lda zp_oct_in_row
    jsr mul_by_7
    ; A = oct_in_row * 7
    plx
    phx
    clc
    adc black_key_wk,x
    ; A = global white key index

    ; pixel X = A * KW + KX + KW - BW/2
    ; BW/2 = 3 (integer), so offset = KW - 3 = 8
    jsr mul_by_kw               ; result in zp_mul_r_lo/hi
    lda zp_mul_r_lo
    clc
    adc #(KX + KW - BW/2)
    sta zp_key_x_lo
    lda zp_mul_r_hi
    adc #0
    sta zp_key_x_hi

    ; Fill black key rectangle
    lda zp_key_x_lo
    sta RegP0
    lda zp_key_x_hi
    sta RegP1
    lda zp_key_y
    sta RegP2
    stz RegP3
    lda zp_key_x_lo
    clc
    adc #BW - 1
    sta RegP4
    lda zp_key_x_hi
    adc #0
    sta RegP5
    lda zp_key_y
    clc
    adc #BH - 1
    sta RegP6
    stz RegP7
    lda #CmdFill
    sta RegCmd

    plx
    inx
    bra @bk_key

@bk_next_oct:
    inc zp_oct_in_row
    bra @bk_oct
@bk_done:
    rts

; Black key positions: white key index (0-6) that precedes each black key
black_key_wk: .byte 0, 1, 3, 4, 5

; =====================================================================
; Multiply A by 7 -> A (assumes A < 37)
; =====================================================================
mul_by_7:
    sta zp_tmp
    asl                         ; *2
    asl                         ; *4
    asl                         ; *8
    sec
    sbc zp_tmp                  ; *7
    rts

; =====================================================================
; Multiply A by KW (11) -> zp_mul_r_lo/hi (16-bit result)
; Input: A = value (0-27)
; =====================================================================
mul_by_kw:
    sta zp_tmp
    ; val*11 = val*8 + val*2 + val
    asl                         ; *2
    sta zp_tmp2
    asl                         ; *4
    asl                         ; *8
    clc
    adc zp_tmp2                 ; *10
    clc
    adc zp_tmp                  ; *11
    sta zp_mul_r_lo
    lda #0
    adc #0                      ; carry from addition
    sta zp_mul_r_hi
    rts

; =====================================================================
; Draw octave labels C0-C7
; =====================================================================
draw_labels:
    ; Upper row: C0-C3, text row below keys = (Y1+KH)/8 ≈ row 10
    lda #10
    sta RegCursorY
    ldx #0
@upper:
    cpx #4
    bcs @lower_init
    phx
    txa
    jsr calc_label_col          ; compute column for octave-in-row
    sta RegCursorX
    lda #'C'
    sta RegCharOut
    plx
    phx
    txa
    clc
    adc #'0'
    sta RegCharOut
    plx
    inx
    bra @upper

@lower_init:
    ; Lower row: C4-C7, text row = (Y2+KH)/8 ≈ row 16
    lda #17
    sta RegCursorY
    ldx #4
@lower:
    cpx #8
    bcs @labels_done
    phx
    txa
    sec
    sbc #4                      ; oct_in_row
    jsr calc_label_col
    sta RegCursorX
    lda #'C'
    sta RegCharOut
    plx
    phx
    txa
    clc
    adc #'0'
    sta RegCharOut
    plx
    inx
    bra @lower

@labels_done:
    rts

; Calculate text column for octave label
; Input: A = octave_in_row (0-3)
; Output: A = text column
; Column = (KX + oct_in_row * 77) / 4
calc_label_col:
    ; oct * 77 via repeated addition (max 3 iterations)
    tax
    lda #0
    cpx #0
    beq @ccl_done
@ccl_add:
    clc
    adc #77
    dex
    bne @ccl_add
@ccl_done:
    clc
    adc #KX                     ; + keyboard X origin
    lsr
    lsr                         ; / 4 = text column
    rts

; =====================================================================
; Define sprite shapes (12 total)
; Shapes 0-5: white key pressed, shapes 6-11: black key pressed
; =====================================================================
define_shapes:
    ldx #0
@ds_loop:
    cpx #NUM_VOICES
    bcs @ds_done
    stx zp_voice
    lda voice_colors,x
    sta zp_spr_color

    ; White shape = voice index
    txa
    sta zp_spr_shape
    jsr def_white_shape

    ; Black shape = voice + 6
    lda zp_voice
    clc
    adc #NUM_VOICES
    sta zp_spr_shape
    jsr def_black_shape

    ldx zp_voice
    inx
    bra @ds_loop
@ds_done:
    rts

; =====================================================================
; Define white key pressed shape
; zp_spr_shape = shape slot (0-5), zp_spr_color = color
; =====================================================================
def_white_shape:
    ; Point sprite N at shape slot N (write to sprite register)
    lda zp_spr_shape
    asl
    asl
    asl
    tax
    lda zp_spr_shape
    sta SpriteRegBase + SprRegShape,x

    ; Build color byte patterns
    ; Full color: CC (two voice-color pixels)
    lda zp_spr_color
    asl
    asl
    asl
    asl
    ora zp_spr_color
    sta zp_tmp                  ; CC

    ; Half color: C|transparent
    lda zp_spr_color
    asl
    asl
    asl
    asl
    ora #TRANS
    sta zp_tmp2                 ; CF

    ; Rows 0-1: accent bar (11 px = 5 full bytes + 1 half)
    lda #0
    sta zp_spr_row
@ws_accent:
    lda zp_spr_row
    cmp #2
    bcs @ws_body

    lda zp_spr_shape
    sta RegP0
    lda zp_spr_row
    sta RegP1
    lda zp_tmp
    sta RegP2
    sta RegP3
    sta RegP4
    sta RegP5
    sta RegP6
    lda zp_tmp2
    sta RegP7
    lda #(TRANS << 4) | TRANS
    sta RegP8
    sta RegP9
    lda #CmdSprRow
    sta RegCmd

    inc zp_spr_row
    bra @ws_accent

@ws_body:
    ; Rows 2-12: white body
    lda #(COL_WHITE << 4) | COL_WHITE
    sta zp_tmp
    lda #(COL_WHITE << 4) | TRANS
    sta zp_tmp2
@ws_body_loop:
    lda zp_spr_row
    cmp #13
    bcs @ws_shadow

    lda zp_spr_shape
    sta RegP0
    lda zp_spr_row
    sta RegP1
    lda zp_tmp
    sta RegP2
    sta RegP3
    sta RegP4
    sta RegP5
    sta RegP6
    lda zp_tmp2
    sta RegP7
    lda #(TRANS << 4) | TRANS
    sta RegP8
    sta RegP9
    lda #CmdSprRow
    sta RegCmd

    inc zp_spr_row
    bra @ws_body_loop

@ws_shadow:
    ; Rows 13-15: dark gray shadow
    lda #(COL_DGRAY << 4) | COL_DGRAY
    sta zp_tmp
    lda #(COL_DGRAY << 4) | TRANS
    sta zp_tmp2
@ws_shad_loop:
    lda zp_spr_row
    cmp #16
    bcs @ws_done

    lda zp_spr_shape
    sta RegP0
    lda zp_spr_row
    sta RegP1
    lda zp_tmp
    sta RegP2
    sta RegP3
    sta RegP4
    sta RegP5
    sta RegP6
    lda zp_tmp2
    sta RegP7
    lda #(TRANS << 4) | TRANS
    sta RegP8
    sta RegP9
    lda #CmdSprRow
    sta RegCmd

    inc zp_spr_row
    bra @ws_shad_loop
@ws_done:
    rts

; =====================================================================
; Define black key pressed shape
; zp_spr_shape = shape slot (6-11), zp_spr_color = color
; =====================================================================
def_black_shape:
    ; Point sprite at shape slot
    lda zp_spr_shape
    asl
    asl
    asl
    tax
    lda zp_spr_shape
    sta SpriteRegBase + SprRegShape,x

    ; Color bytes
    lda zp_spr_color
    asl
    asl
    asl
    asl
    ora zp_spr_color
    sta zp_tmp                  ; CC

    lda zp_spr_color
    asl
    asl
    asl
    asl
    ora #TRANS
    sta zp_tmp2                 ; CF

    ; Rows 0-1: accent (7px = 3.5 bytes)
    lda #0
    sta zp_spr_row
@bs_accent:
    lda zp_spr_row
    cmp #2
    bcs @bs_body

    lda zp_spr_shape
    sta RegP0
    lda zp_spr_row
    sta RegP1
    lda zp_tmp
    sta RegP2
    sta RegP3
    sta RegP4
    lda zp_tmp2
    sta RegP5
    lda #(TRANS << 4) | TRANS
    sta RegP6
    sta RegP7
    sta RegP8
    sta RegP9
    lda #CmdSprRow
    sta RegCmd

    inc zp_spr_row
    bra @bs_accent

@bs_body:
    ; Rows 2-10: dark gray body
    lda #(COL_DGRAY << 4) | COL_DGRAY
    sta zp_tmp
    lda #(COL_DGRAY << 4) | TRANS
    sta zp_tmp2
@bs_body_loop:
    lda zp_spr_row
    cmp #11
    bcs @bs_trans

    lda zp_spr_shape
    sta RegP0
    lda zp_spr_row
    sta RegP1
    lda zp_tmp
    sta RegP2
    sta RegP3
    sta RegP4
    lda zp_tmp2
    sta RegP5
    lda #(TRANS << 4) | TRANS
    sta RegP6
    sta RegP7
    sta RegP8
    sta RegP9
    lda #CmdSprRow
    sta RegCmd

    inc zp_spr_row
    bra @bs_body_loop

@bs_trans:
    ; Rows 11-15: transparent
@bs_trans_loop:
    lda zp_spr_row
    cmp #16
    bcs @bs_done

    lda zp_spr_shape
    sta RegP0
    lda zp_spr_row
    sta RegP1
    lda #(TRANS << 4) | TRANS
    sta RegP2
    sta RegP3
    sta RegP4
    sta RegP5
    sta RegP6
    sta RegP7
    sta RegP8
    sta RegP9
    lda #CmdSprRow
    sta RegCmd

    inc zp_spr_row
    bra @bs_trans_loop
@bs_done:
    rts

; =====================================================================
; Draw player chrome (progress bar, legend, instructions)
; =====================================================================
draw_chrome:
    ; Progress bar outline
    lda #COL_DGRAY
    sta RegP0
    lda #CmdGcolor
    sta RegCmd

    lda #<BAR_X
    sta RegP0
    lda #>BAR_X
    sta RegP1
    lda #BAR_Y
    sta RegP2
    stz RegP3
    lda #<(BAR_X + BAR_W)
    sta RegP4
    lda #>(BAR_X + BAR_W)
    sta RegP5
    lda #BAR_Y + BAR_H
    sta RegP6
    stz RegP7
    lda #CmdRect
    sta RegCmd

    ; Voice legend at text row 20
    lda #20
    sta RegCursorY
    ldx #0
@legend:
    cpx #NUM_VOICES
    bcs @legend_done
    phx

    ; Column = 2 + voice * 10
    txa
    asl                         ; *2
    sta zp_tmp
    asl                         ; *4
    asl                         ; *8
    clc
    adc zp_tmp                  ; *10
    clc
    adc #2
    sta RegCursorX

    plx
    phx
    lda voice_colors,x
    sta RegFgCol

    ; Solid block character
    lda #$A0
    sta RegCharOut

    lda #COL_WHITE
    sta RegFgCol
    lda #' '
    sta RegCharOut
    lda #'V'
    sta RegCharOut

    plx
    phx
    txa
    clc
    adc #'1'
    sta RegCharOut

    plx
    inx
    bra @legend
@legend_done:

    ; "PRESS ANY KEY TO EXIT" at row 23
    lda #23
    sta RegCursorY
    lda #20
    sta RegCursorX
    lda #COL_LGRAY
    sta RegFgCol
    lda #<str_exit
    sta zp_str_ptr
    lda #>str_exit
    sta zp_str_ptr+1
    jsr print_str

    ; Time label at row 19
    lda #COL_WHITE
    sta RegFgCol
    lda #19
    sta RegCursorY
    lda #2
    sta RegCursorX
    lda #<str_time
    sta zp_str_ptr
    lda #>str_time
    sta zp_str_ptr+1
    jsr print_str

    rts

; =====================================================================
; Update voice sprites
; =====================================================================
update_voices:
    ldx #0
@uv_loop:
    cpx #NUM_VOICES
    bcs @uv_done
    stx zp_voice

    lda MusicNote1,x
    sta zp_note
    cmp prev_notes,x
    beq @uv_next

    sta prev_notes,x
    cmp #0
    bne @uv_on

    ; Note off: disable sprite
    jsr disable_sprite
    bra @uv_next

@uv_on:
    jsr position_sprite

@uv_next:
    ldx zp_voice
    inx
    bra @uv_loop
@uv_done:
    rts

; =====================================================================
; Disable sprite for zp_voice
; =====================================================================
disable_sprite:
    lda zp_voice
    asl
    asl
    asl
    tax
    lda SpriteRegBase + SprRegFlags,x
    and #<(~SprFlagEnable)
    sta SpriteRegBase + SprRegFlags,x
    rts

; =====================================================================
; Position and enable sprite for zp_voice / zp_note
; =====================================================================
position_sprite:
    ; Convert MIDI note to keyboard coordinates
    lda zp_note
    sec
    sbc #MIDI_BASE
    bcs @ps_in_range1
    jmp @ps_off                 ; below range
@ps_in_range1:
    cmp #96
    bcc @ps_in_range2
    jmp @ps_off                 ; above range (8 octaves = 96 notes)
@ps_in_range2:

    ; Divide by 12 -> octave, remainder -> pitch class
    sta zp_div_lo
    stz zp_div_hi
    lda #12
    sta zp_divisor
    jsr div16_8
    lda zp_quot_lo
    sta zp_octave
    lda zp_remain
    sta zp_pitch

    ; Key type and index
    ldx zp_pitch
    lda key_type,x
    sta zp_is_black
    lda key_index,x
    sta zp_tmp                  ; white key index in octave

    ; Row and octave-in-row
    lda zp_octave
    cmp #4
    bcs @ps_lower
    stz zp_row
    sta zp_oct_in_row
    bra @ps_calc_x
@ps_lower:
    lda #1
    sta zp_row
    lda zp_octave
    sec
    sbc #4
    sta zp_oct_in_row

@ps_calc_x:
    ; global_wk = oct_in_row * 7 + white_key_index
    lda zp_oct_in_row
    jsr mul_by_7
    clc
    adc zp_tmp                  ; + key index in octave
    ; A = global white key index (0-27)

    ; X pixel = global_wk * KW + KX
    jsr mul_by_kw               ; -> zp_mul_r_lo/hi
    lda zp_mul_r_lo
    clc
    adc #KX
    sta zp_key_x_lo
    lda zp_mul_r_hi
    adc #0
    sta zp_key_x_hi

    ; Black key offset
    lda zp_is_black
    beq @ps_white

    ; x += (KW - BW/2) = 8
    lda zp_key_x_lo
    clc
    adc #(KW - BW/2)
    sta zp_key_x_lo
    lda zp_key_x_hi
    adc #0
    sta zp_key_x_hi

    ; Shape = voice + 6
    lda zp_voice
    clc
    adc #NUM_VOICES
    sta zp_shape
    bra @ps_set_y

@ps_white:
    ; Shape = voice
    lda zp_voice
    sta zp_shape

@ps_set_y:
    lda zp_row
    beq @ps_y_upper
    lda #Y2
    bra @ps_assign
@ps_y_upper:
    lda #Y1
@ps_assign:
    sta zp_key_y

    ; Set sprite shape
    lda zp_voice
    asl
    asl
    asl
    tax
    lda zp_shape
    sta SpriteRegBase + SprRegShape,x

    ; Position via command
    lda zp_voice
    sta RegP0
    lda zp_key_x_lo
    sta RegP1
    lda zp_key_x_hi
    sta RegP2
    lda zp_key_y
    sta RegP3
    stz RegP4
    lda #CmdSprPos
    sta RegCmd

    ; Enable
    lda zp_voice
    sta RegP0
    lda #CmdSprEna
    sta RegCmd
    rts

@ps_off:
    jsr disable_sprite
    rts

; =====================================================================
; Update progress bar and time display
; =====================================================================
update_progress:
    lda MusicTotalL
    ora MusicTotalH
    bne @up_has_total
    jmp @up_done                ; total = 0, nothing to show
@up_has_total:

    ; --- Progress bar fill ---
    ; width = elapsed * (BAR_W-2) / total
    ; 16-bit multiply: elapsed * (BAR_W-2)
    lda MusicElapsedL
    sta zp_mul_a_lo
    lda MusicElapsedH
    sta zp_mul_a_hi
    lda #<(BAR_W - 2)
    sta zp_dvd0                 ; reuse as temp for multiplicand
    jsr mul16_8                 ; 16x8 -> 24-bit, but BAR_W-2=306 > 255
    ; Hmm, BAR_W-2 = 306 which doesn't fit in 8 bits.
    ; Use full 16x16 multiply instead.
    jsr mul16x16
    ; 32-bit result in zp_dvd0..3

    ; Divide 32-bit result by 16-bit total
    lda MusicTotalL
    sta zp_dvs_lo
    lda MusicTotalH
    sta zp_dvs_hi
    jsr div32_16
    ; 16-bit quotient in zp_q_lo/hi

    ; Clamp to BAR_W-2
    lda zp_q_hi
    bne @up_clamp
    lda zp_q_lo
    cmp #<(BAR_W - 2)
    bcc @up_no_clamp
    ; Check high byte of BAR_W-2 (=1, since 306=$132)
    ; Actually BAR_W-2 = 306. q_hi=0, q_lo < 256, but 306 > 255.
    ; So if q_hi=0, we're fine (under 256 < 306).
    bra @up_no_clamp
@up_clamp:
    cmp #>(BAR_W - 2)
    bcc @up_no_clamp
    bne @up_do_clamp
    lda zp_q_lo
    cmp #<(BAR_W - 2)
    bcc @up_no_clamp
@up_do_clamp:
    lda #<(BAR_W - 2)
    sta zp_q_lo
    lda #>(BAR_W - 2)
    sta zp_q_hi
@up_no_clamp:

    ; Compare with previous
    lda zp_q_lo
    cmp zp_prev_bar
    bne @up_redraw
    lda zp_q_hi
    cmp zp_prev_bar+1
    beq @up_time
@up_redraw:
    lda zp_q_lo
    sta zp_prev_bar
    lda zp_q_hi
    sta zp_prev_bar+1

    ; Skip if width = 0
    ora zp_q_lo
    beq @up_time

    ; Draw filled bar
    lda #COL_GREEN
    sta RegP0
    lda #CmdGcolor
    sta RegCmd

    lda #<(BAR_X + 1)
    sta RegP0
    lda #>(BAR_X + 1)
    sta RegP1
    lda #BAR_Y + 1
    sta RegP2
    stz RegP3
    ; x2 = BAR_X + width (16-bit add)
    lda zp_prev_bar
    clc
    adc #BAR_X
    sta RegP4
    lda zp_prev_bar+1
    adc #0
    sta RegP5
    lda #BAR_Y + BAR_H - 1
    sta RegP6
    stz RegP7
    lda #CmdFill
    sta RegCmd

@up_time:
    ; --- Time display ---
    ; elapsed seconds = elapsed frames / 60
    lda MusicElapsedL
    sta zp_div_lo
    lda MusicElapsedH
    sta zp_div_hi
    lda #FPS
    sta zp_divisor
    jsr div16_8
    ; quot = total elapsed seconds

    ; minutes = seconds / 60
    lda zp_quot_lo
    sta zp_div_lo
    lda zp_quot_hi
    sta zp_div_hi
    lda #60
    sta zp_divisor
    jsr div16_8
    ; quot = minutes, remain = seconds

    ; Display at row 19, col 8
    lda #19
    sta RegCursorY
    lda #8
    sta RegCursorX
    lda #COL_WHITE
    sta RegFgCol

    lda zp_quot_lo
    jsr print_dec2
    lda #':'
    sta RegCharOut
    lda zp_remain
    jsr print_dec2

    ; Separator
    lda #' '
    sta RegCharOut
    lda #'/'
    sta RegCharOut
    lda #' '
    sta RegCharOut

    ; Total time
    lda MusicTotalL
    sta zp_div_lo
    lda MusicTotalH
    sta zp_div_hi
    lda #FPS
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

@up_done:
    rts

; =====================================================================
; 16-bit x 16-bit unsigned multiply
; Input:  zp_mul_a_lo/hi * (BAR_W-2) constant
; Output: 32-bit result in zp_dvd0..3 (LSB first)
; Uses zp_mul_a as multiplier, hardcoded multiplicand
; =====================================================================
mul16x16:
    ; Multiply zp_mul_a (16-bit) by (BAR_W-2) = 306 = $0132
    ; Generic 16x16: result = 0, loop 16 bits of multiplier
    stz zp_dvd0
    stz zp_dvd1
    stz zp_dvd2
    stz zp_dvd3

    ; Multiplicand in tmp storage (we use zp_dvs as temp here)
    lda #<(BAR_W - 2)
    sta zp_dvs_lo
    lda #>(BAR_W - 2)
    sta zp_dvs_hi

    ldx #16
@m16_loop:
    ; Shift multiplier right
    lsr zp_mul_a_hi
    ror zp_mul_a_lo
    bcc @m16_skip

    ; Add multiplicand to upper 16 bits of result
    clc
    lda zp_dvd2
    adc zp_dvs_lo
    sta zp_dvd2
    lda zp_dvd3
    adc zp_dvs_hi
    sta zp_dvd3
@m16_skip:
    ; Shift result right
    lsr zp_dvd3
    ror zp_dvd2
    ror zp_dvd1
    ror zp_dvd0

    dex
    bne @m16_loop
    rts

; =====================================================================
; 32-bit / 16-bit unsigned division
; Input:  zp_dvd0..3 = dividend (32-bit), zp_dvs_lo/hi = divisor
; Output: zp_q_lo/hi = quotient (16-bit), zp_rem16_lo/hi = remainder
; =====================================================================
div32_16:
    stz zp_rem16_lo
    stz zp_rem16_hi
    stz zp_q_lo
    stz zp_q_hi

    ldx #32
@d32_loop:
    ; Shift dividend left, MSB into remainder
    asl zp_dvd0
    rol zp_dvd1
    rol zp_dvd2
    rol zp_dvd3
    rol zp_rem16_lo
    rol zp_rem16_hi

    ; Try subtract divisor from remainder
    lda zp_rem16_lo
    sec
    sbc zp_dvs_lo
    pha
    lda zp_rem16_hi
    sbc zp_dvs_hi
    bcc @d32_skip               ; remainder < divisor

    ; Commit subtraction
    sta zp_rem16_hi
    pla
    sta zp_rem16_lo

    ; Set quotient bit (shift into dvd0 LSB, which becomes quotient)
    inc zp_dvd0
    bra @d32_next

@d32_skip:
    pla                         ; discard
@d32_next:
    dex
    bne @d32_loop

    ; Quotient is in dvd0..1 (lower 16 bits)
    lda zp_dvd0
    sta zp_q_lo
    lda zp_dvd1
    sta zp_q_hi
    rts

; =====================================================================
; Unused stub (referenced in earlier code, kept for safety)
; =====================================================================
mul16_8:
    rts

; =====================================================================
; 16-bit / 8-bit unsigned division
; Input:  zp_div_lo/hi = dividend, zp_divisor = divisor
; Output: zp_quot_lo/hi = quotient, zp_remain = remainder
; =====================================================================
div16_8:
    stz zp_remain
    ldx #16
@d8_loop:
    asl zp_div_lo
    rol zp_div_hi
    rol zp_remain
    lda zp_remain
    cmp zp_divisor
    bcc @d8_skip
    sbc zp_divisor
    sta zp_remain
    inc zp_div_lo
@d8_skip:
    dex
    bne @d8_loop

    lda zp_div_lo
    sta zp_quot_lo
    lda zp_div_hi
    sta zp_quot_hi
    rts

; =====================================================================
; Print 2-digit decimal (A = 0-99)
; =====================================================================
print_dec2:
    ldx #0
@pd_tens:
    cmp #10
    bcc @pd_out
    sec
    sbc #10
    inx
    bra @pd_tens
@pd_out:
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

; =====================================================================
; Print null-terminated string at (zp_str_ptr)
; =====================================================================
print_str:
    ldy #0
@ps_loop:
    lda (zp_str_ptr),y
    beq @ps_end
    sta RegCharOut
    iny
    bra @ps_loop
@ps_end:
    rts

; =====================================================================
; Data tables
; =====================================================================
.segment "RODATA"

voice_colors: .byte COL_RED, COL_ORANGE, COL_YELLOW, COL_GREEN, COL_CYAN, COL_BLUE

; Pitch class tables (indexed 0-11)
;              C  C# D  D# E  F  F# G  G# A  A# B
key_type: .byte 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0
key_index:.byte 0, 0, 1, 1, 2, 3, 3, 4, 4, 5, 5, 6

str_exit: .byte "PRESS ANY KEY TO EXIT", 0
str_time: .byte "TIME: ", 0

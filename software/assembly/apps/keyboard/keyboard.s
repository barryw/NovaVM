; keyboard.s -- Piano keyboard visualizer for NovaVM
; Displays an 8-octave piano keyboard and animates note indicators
; using sprites while music plays via the SID/MIDI engine.
;
; Load address: $9000   Invoke: SYS $9000
; Assembler:    ca65 --cpu w65c02
; Linker:       ld65 -C keyboard.cfg

.include "novavm.inc"
.include "audio.inc"

.import vgc_wait_cmd

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
TEXT_COLS   = 80
MIDI_BASE   = 12                ; C0 = MIDI note 12
MAX_VOICES  = 14
SID_FIRST_VOICE = 0
SID_VOICE_COUNT = 6
WTS_FIRST_VOICE = 0             ; SID or MIDI plays alone; MIDI uses voices 0-7
WTS_VOICE_COUNT = 8
BAR_X       = 6                 ; progress bar X
BAR_Y       = 174               ; progress bar Y
BAR_W       = 308               ; progress bar max fill width
BAR_H       = 6                 ; progress bar height
TIME_ROW    = 41
EXIT_ROW    = 47
FONT_ROW    = 5
LEGEND_SID_ROW  = 37
LEGEND_MIDI_ROW = 39
LEGEND_ONE_ROW  = 38
LEGEND_ENTRY_CHARS = 5          ; two-cell swatch, space, digit, space
LEGEND_PREFIX_CHARS = 5         ; "SID  " or "MIDI "
LEGEND_SID_X    = (TEXT_COLS - (LEGEND_PREFIX_CHARS + SID_VOICE_COUNT * LEGEND_ENTRY_CHARS) + 1) / 2
LEGEND_MIDI_X   = (TEXT_COLS - (LEGEND_PREFIX_CHARS + WTS_VOICE_COUNT * LEGEND_ENTRY_CHARS) + 1) / 2
SID_TIME_CHARS  = 11            ; "TIME: MM:SS"
SID_TIME_X      = (TEXT_COLS - SID_TIME_CHARS + 1) / 2
EXIT_CHARS      = 19            ; "PRESS ESC/Q TO EXIT"
EXIT_X          = (TEXT_COLS - EXIT_CHARS + 1) / 2
TEST_TITLE_X    = 24
TITLE_ROW       = 1
SOURCE_ROW      = 3
FPS         = 60                ; frames per second
INPUT_GRACE_FRAMES = 60        ; ignore launch keystrokes for first second
INPUT_ARM_QUIET_FRAMES = 30    ; require quiet input before exit keys count
STOP_EXIT_FRAMES = 120         ; require music stopped for 2 seconds before exit
MODE_NONE   = 0
MODE_SID    = 1
MODE_WTS    = 2
KBD_ZP_BASE = $14
KBD_ZP_SAVE_LEN = $47

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
COL_KEY_BLACK = COL_BLACK

; ---------------------------------------------------------------------------
; Zero page ($14-$5A -- BASIC-safe scratch range)
; ---------------------------------------------------------------------------
.segment "ZEROPAGE"

zp_tmp:         .res 1
zp_tmp2:        .res 1
zp_voice:       .res 1          ; current hardware voice 0-13
zp_voice_idx:   .res 1          ; display voice index
zp_first_voice: .res 1          ; first hardware voice for the active source
zp_voice_count: .res 1          ; active source voice count
zp_voice_mode:  .res 1          ; MODE_SID or MODE_WTS
zp_note:        .res 1          ; current MIDI note
zp_octave:      .res 1          ; octave 0-7
zp_pitch:       .res 1          ; pitch class 0-11
zp_row:         .res 1          ; keyboard row 0=upper 1=lower
zp_key_x_lo:    .res 1          ; key X lo
zp_key_x_hi:    .res 1          ; key X hi
zp_key_y:       .res 1          ; key Y
zp_is_black:    .res 1          ; 1=black key
zp_paint_col:   .res 1          ; color for flood fill
zp_str_ptr:     .res 2          ; string pointer
zp_prev_bar:    .res 2          ; previous bar fill width
zp_last_frame:  .res 1          ; last frame counter for vsync
zp_was_playing: .res 1          ; nonzero once music has been detected
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

prev_notes:     .res MAX_VOICES
save_mode:      .res 1
save_bgcol:     .res 1
save_fgcol:     .res 1
save_cursor:    .res 1
save_font:      .res 1
zp_save_buf:    .res KBD_ZP_SAVE_LEN
input_quiet_frames: .res 1
input_armed:    .res 1
music_stop_frames: .res 1

.ifdef KEYBOARD_TEST
test_current_note: .res 1
test_direction:    .res 1      ; 0=ascending, nonzero=descending
test_frame_count:  .res 1
test_color_index:  .res 1
.endif

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
    php
    cld
    jsr save_app_zp

    ; Save VGC state
    lda RegMode
    sta save_mode
    lda RegBgCol
    sta save_bgcol
    lda RegFgCol
    sta save_fgcol
    lda RegCursorEnable
    sta save_cursor
    lda RegFont
    sta save_font

    ; Clear previous notes
    lda #0
    ldx #MAX_VOICES - 1
@clr_prev:
    sta prev_notes,x
    dex
    bpl @clr_prev

    stz zp_prev_bar
    stz zp_prev_bar+1
    stz zp_was_playing
    stz zp_voice_mode
    stz zp_first_voice
    stz zp_voice_count
    stz input_quiet_frames
    stz input_armed
    stz music_stop_frames

    ; Set mode 1 (gfx over text), default font
    lda #1
    sta RegMode
    stz RegFont
    lda #COL_BLACK
    sta RegBgCol
    lda #COL_WHITE
    sta RegFgCol
    stz RegCursorEnable

    ; Clear graphics
    lda #CmdGcls
    sta RegCmd
    jsr vgc_wait_cmd

    ; Clear text
    jsr clear_text

    ; Draw keyboards
    jsr draw_keyboards

    ; Draw octave labels
    jsr draw_labels

    ; Draw chrome
.ifndef KEYBOARD_TEST
    jsr draw_song_header
    jsr draw_source_header
    jsr draw_soundfont_header
    jsr draw_chrome
.endif

.ifdef KEYBOARD_TEST
    jsr init_test_mode
.endif

    ; Snapshot frame counter
    lda RegStatus
    sta zp_last_frame

    ; ---- Main loop ----
@main_loop:
    jsr wait_vsync
.ifdef KEYBOARD_TEST
    jsr update_test_sweep
.else
    jsr update_voice_mode
    jsr update_voices
    jsr update_progress
    jsr check_exit_key
    bcs @user_exit
.endif

.ifdef KEYBOARD_TEST
    bra @main_loop
.else
    ; Check music playing (bit 1)
    lda MusicStatus
    and #AUDIO_STATUS_MUSIC
    beq @chk_stop
    ; Music is playing — remember that
    sta zp_was_playing
    stz music_stop_frames
    bra @main_loop
@chk_stop:
    ; Music not playing — only exit if it WAS playing before
    lda zp_was_playing
    beq @main_loop            ; never started, keep waiting
    ; Was playing, now stopped. Debounce this so a transient host status gap
    ; doesn't close the visualizer while music is still audible.
    lda music_stop_frames
    cmp #STOP_EXIT_FRAMES
    bcs @exit
    inc music_stop_frames
    bra @main_loop
.endif

@user_exit:
.ifndef KEYBOARD_TEST
    jsr stop_hosted_music
.endif

@exit:
    ; Clear graphics
    lda #CmdGcls
    sta RegCmd
    jsr vgc_wait_cmd

    ; Restore state
    lda save_mode
    sta RegMode
    lda save_font
    sta RegFont
    lda save_bgcol
    sta RegBgCol
    lda save_fgcol
    sta RegFgCol
    lda save_cursor
    sta RegCursorEnable

    jsr clear_text
    jsr restore_app_zp
    plp
    rts

; =====================================================================
; Preserve BASIC zero page used by the standalone visualizer.
; BASIC resumes after SYS through zero-page interpreter state, so any app
; that borrows this range must restore it before returning.
; =====================================================================
save_app_zp:
    ldx #KBD_ZP_SAVE_LEN - 1
@save_loop:
    lda KBD_ZP_BASE,x
    sta zp_save_buf,x
    dex
    bpl @save_loop
    rts

restore_app_zp:
    ldx #KBD_ZP_SAVE_LEN - 1
@restore_loop:
    lda zp_save_buf,x
    sta KBD_ZP_BASE,x
    dex
    bpl @restore_loop
    rts

; =====================================================================
; Read one input byte and decide whether it should exit the visualizer.
; Carry set means exit. Before input is armed, consumed bytes are treated as
; launch residue and reset the quiet counter. Once armed, only explicit exit
; keys close the visualizer; ordinary buffered input is ignored.
; =====================================================================
check_exit_key:
    jsr read_input
    bcc @no_key

    ; Key in A — use X to check armed so A is preserved.
    ldx input_armed
    bne @check_key

    stz input_quiet_frames
    clc
    rts

@check_key:
    cmp #$1B                    ; ESC
    beq @exit
    cmp #'Q'
    beq @exit
    cmp #'q'
    beq @exit
    clc
    rts

@no_key:
    lda input_armed
    bne @stay

    lda input_quiet_frames
    cmp #INPUT_ARM_QUIET_FRAMES
    bcs @arm
    inc input_quiet_frames
    clc
    rts

@arm:
    lda #1
    sta input_armed
@stay:
    clc
    rts

@exit:
    sec
    rts

; =====================================================================
; Stop hosted playback when the user explicitly exits the visualizer.
; This is intentionally bounded: if the host is unhealthy, return to BASIC
; instead of getting trapped forever waiting for FIO_STATUS.
; =====================================================================
.ifndef KEYBOARD_TEST
stop_hosted_music:
    lda MusicStatus
    sta zp_tmp
    and #AUDIO_STATUS_WTS
    beq @check_sid
    lda #FioCmdMidStop
    jsr issue_music_stop_cmd

@check_sid:
    lda zp_tmp
    and #AUDIO_STATUS_SID
    beq @done
    lda #FioCmdSidStop
    jsr issue_music_stop_cmd

@done:
    rts

issue_music_stop_cmd:
    sta zp_tmp2
    stz FioStatus
    stz FioErrCode
    lda zp_tmp2
    sta FioCmd

    ldx #$FF
@outer:
    ldy #$FF
@inner:
    lda FioStatus
    bne @done
    dey
    bne @inner
    dex
    bne @outer
@done:
    rts
.endif

; =====================================================================
; Read one raw keyboard byte from the VGC input latch.
; Carry set means a key byte was returned in A.
; =====================================================================
read_input:
    lda RegCharIn
    beq @none
    sec
    rts
@none:
    clc
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
    ; A manual 80x50 RegCharOut clear writes the bottom-right cell and arms
    ; VGC text scroll. Use FF so the VGC clears text and resets scroll_offset.
    lda #$0C
    sta RegCharOut
    jsr vgc_wait_cmd
    stz RegCursorX
    stz RegCursorY
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
    jsr vgc_wait_cmd

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
    jsr vgc_wait_cmd

    ; Step 2: white-key separators. Use 1-pixel filled rectangles instead of
    ; line commands so the boundaries are hard walls for PAINT on hardware.
    lda #COL_DGRAY
    sta RegP0
    lda #CmdGcolor
    sta RegCmd
    jsr vgc_wait_cmd

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
    lda #CmdFill
    sta RegCmd
    jsr vgc_wait_cmd

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
    lda #COL_DGRAY
    sta RegP0
    lda #CmdGcolor
    sta RegCmd
    jsr vgc_wait_cmd

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
    jsr vgc_wait_cmd

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
    jsr vgc_wait_cmd

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
    jsr vgc_wait_cmd

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
    jsr vgc_wait_cmd

    ; Step 4: black keys
    jsr draw_black_keys
    rts

; =====================================================================
; Draw black keys for one keyboard row.
; Input: zp_key_y
; =====================================================================
draw_black_keys:
    lda #COL_DGRAY
    sta RegP0
    lda #CmdGcolor
    sta RegCmd
    jsr vgc_wait_cmd

    stz zp_oct_in_row           ; 0-3
@bk_oct:
    lda zp_oct_in_row
    cmp #4
    bcc @bk_oct_go
    jmp @bk_done
@bk_oct_go:

    ldx #0                      ; black key index 0-4
@bk_key:
    cpx #5
    bcc @bk_key_go
    jmp @bk_next_oct
@bk_key_go:
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

    ; Draw a dark border first. The border isolates the black-key interior from
    ; the black background so PAINT can safely recolor black keys.
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
    jsr vgc_wait_cmd

    ; Fill the interior black.
    lda #COL_KEY_BLACK
    sta RegP0
    lda #CmdGcolor
    sta RegCmd
    jsr vgc_wait_cmd

    lda zp_key_x_lo
    clc
    adc #1
    sta RegP0
    lda zp_key_x_hi
    adc #0
    sta RegP1
    lda zp_key_y
    inc
    sta RegP2
    stz RegP3
    lda zp_key_x_lo
    clc
    adc #(BW - 2)
    sta RegP4
    lda zp_key_x_hi
    adc #0
    sta RegP5
    lda zp_key_y
    clc
    adc #(BH - 2)
    sta RegP6
    stz RegP7
    lda #CmdFill
    sta RegCmd
    jsr vgc_wait_cmd

    lda #COL_DGRAY
    sta RegP0
    lda #CmdGcolor
    sta RegCmd
    jsr vgc_wait_cmd

    plx
    inx
    jmp @bk_key

@bk_next_oct:
    inc zp_oct_in_row
    jmp @bk_oct
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
    ; val*11 = val*8 + val*2 + val (16-bit result)
    asl                         ; *2
    sta zp_tmp2
    asl                         ; *4
    asl                         ; *8
    clc
    adc zp_tmp2                 ; val*10 (may overflow)
    sta zp_mul_r_lo
    lda #0
    adc #0                      ; capture carry from val*10
    sta zp_mul_r_hi
    lda zp_mul_r_lo
    clc
    adc zp_tmp                  ; + val = val*11
    sta zp_mul_r_lo
    lda zp_mul_r_hi
    adc #0                      ; + carry
    sta zp_mul_r_hi
    rts

; =====================================================================
; Draw octave labels C0-C7
; =====================================================================
draw_labels:
    ; Upper row: C0-C3, text row below keys = (Y1+KH)/4 ≈ row 20
    lda #20
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
    ; Lower row: C4-C7, text row = (Y2+KH)/4 ≈ row 33
    lda #33
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
; Draw player chrome (progress bar, time, instructions)
; =====================================================================
draw_chrome:
    lda #TIME_ROW
    jsr clear_text_row

    lda MusicTotalL
    ora MusicTotalH
    beq @skip_progress_bar

    ; Progress bar outline
    lda #COL_DGRAY
    sta RegP0
    lda #CmdGcolor
    sta RegCmd
    jsr vgc_wait_cmd

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
    jsr vgc_wait_cmd

@skip_progress_bar:
    ; Exit hint near the bottom, centered.
    lda #EXIT_ROW
    sta RegCursorY
    lda #EXIT_X
    sta RegCursorX
    lda #COL_LGRAY
    sta RegFgCol
    lda #<str_exit
    sta zp_str_ptr
    lda #>str_exit
    sta zp_str_ptr+1
    jsr print_str

    lda MusicTotalL
    ora MusicTotalH
    beq @skip_time_label

    ; Time label above the progress bar
    lda #COL_WHITE
    sta RegFgCol
    lda #TIME_ROW
    sta RegCursorY
    lda #2
    sta RegCursorX
    lda #<str_time
    sta zp_str_ptr
    lda #>str_time
    sta zp_str_ptr+1
    jsr print_str

@skip_time_label:
    rts

; =====================================================================
; Draw current song/file name centered above the keyboard.
; Prefer the unified hosted-music metadata block; fall back to the FIO filename
; buffer for older hosts or manual launches.
; =====================================================================
.ifndef KEYBOARD_TEST
draw_song_header:
    lda #TITLE_ROW
    sta zp_tmp2

    lda AUDIO_META_TITLE
    beq @try_filename
    lda #<AUDIO_META_TITLE
    sta zp_str_ptr
    lda #>AUDIO_META_TITLE
    sta zp_str_ptr+1
    lda #AUDIO_META_TITLE_LEN
    jsr string_len_limited
    bne @print

@try_filename:
    lda FioNameLen
    beq @fallback
    cmp #64
    bcc @len_ok
    lda #63
@len_ok:
    sta zp_tmp
    lda #<FioName
    sta zp_str_ptr
    lda #>FioName
    sta zp_str_ptr+1
    bra @print

@fallback:
    lda #STR_PLAYER_LEN
    sta zp_tmp
    lda #<str_player
    sta zp_str_ptr
    lda #>str_player
    sta zp_str_ptr+1

@print:
    lda #COL_CYAN
    sta RegFgCol
    jsr print_centered_buffer
    ; Author on row 2, released/copyright on row 4 (blank fields are skipped).
    lda #2
    ldx #<AUDIO_META_AUTHOR
    ldy #>AUDIO_META_AUTHOR
    jsr draw_meta_centered
    lda #4
    ldx #<AUDIO_META_COPYRIGHT
    ldy #>AUDIO_META_COPYRIGHT
    jsr draw_meta_centered
    rts

; Center a metadata string (X/Y = address) on row A in light gray; skip if empty.
draw_meta_centered:
    sta zp_tmp2
    stx zp_str_ptr
    sty zp_str_ptr+1
    lda #32
    jsr string_len_limited
    beq @mc_done
    lda #COL_LGRAY
    sta RegFgCol
    jsr print_centered_buffer
@mc_done:
    rts

draw_source_header:
    lda #SOURCE_ROW
    sta zp_tmp2
    lda AUDIO_META_TYPE
    cmp #AUDIO_META_TYPE_SID
    beq @sid
    cmp #AUDIO_META_TYPE_MIDI
    beq @midi
    lda MusicStatus
    and #AUDIO_STATUS_WTS
    bne @midi
    lda MusicStatus
    and #AUDIO_STATUS_SID
    beq @done

@sid:
    lda MusicMetaFlags
    and #AUDIO_META_FLAG_SID_8580
    bne @sid_8580
    lda MusicMetaFlags
    and #AUDIO_META_FLAG_SID_6581
    beq @sid_unknown
    lda MusicMetaFlags
    and #AUDIO_META_FLAG_STEREO
    bne @sid_6581_stereo
    lda #<str_sid_6581
    sta zp_str_ptr
    lda #>str_sid_6581
    sta zp_str_ptr+1
    bra @print_cstr
@sid_6581_stereo:
    lda #<str_sid_6581_stereo
    sta zp_str_ptr
    lda #>str_sid_6581_stereo
    sta zp_str_ptr+1
    bra @print_cstr
@sid_8580:
    lda MusicMetaFlags
    and #AUDIO_META_FLAG_STEREO
    bne @sid_8580_stereo
    lda #<str_sid_8580
    sta zp_str_ptr
    lda #>str_sid_8580
    sta zp_str_ptr+1
    bra @print_cstr
@sid_8580_stereo:
    lda #<str_sid_8580_stereo
    sta zp_str_ptr
    lda #>str_sid_8580_stereo
    sta zp_str_ptr+1
    bra @print_cstr
@sid_unknown:
    lda #<str_sid_source
    sta zp_str_ptr
    lda #>str_sid_source
    sta zp_str_ptr+1
    bra @print_cstr

@midi:
    lda #<str_midi_source
    sta zp_str_ptr
    lda #>str_midi_source
    sta zp_str_ptr+1

@print_cstr:
    lda #COL_LGRAY
    sta RegFgCol
    jmp print_centered_cstr

@done:
    rts

draw_soundfont_header:
    lda AUDIO_META_TYPE
    cmp #AUDIO_META_TYPE_MIDI
    beq @maybe_print
    lda MusicStatus
    and #AUDIO_STATUS_WTS
    beq @done

@maybe_print:
    lda AUDIO_META_SOUNDFONT
    beq @done
    lda #<AUDIO_META_SOUNDFONT
    sta zp_str_ptr
    lda #>AUDIO_META_SOUNDFONT
    sta zp_str_ptr+1
    lda #AUDIO_META_SOUNDFONT_LEN
    jsr string_len_limited
    beq @done

    lda zp_tmp
    pha
    clc
    adc #STR_FONT_PREFIX_LEN
    sta zp_tmp2

    lda #FONT_ROW
    sta RegCursorY
    lda #80
    sec
    sbc zp_tmp2
    lsr
    sta RegCursorX
    lda #COL_LGRAY
    sta RegFgCol
    lda #<str_font_prefix
    sta zp_str_ptr
    lda #>str_font_prefix
    sta zp_str_ptr+1
    jsr print_str

    lda #<AUDIO_META_SOUNDFONT
    sta zp_str_ptr
    lda #>AUDIO_META_SOUNDFONT
    sta zp_str_ptr+1
    pla
    sta zp_tmp
    ldy #0
@name_loop:
    cpy zp_tmp
    bcs @done
    lda (zp_str_ptr),y
    beq @done
    sta RegCharOut
    iny
    bra @name_loop

@done:
    rts
.endif

; =====================================================================
; Select the visible voice bank from MUSIC_STATUS source bits.
; WTS wins if a MIDI/WTS source and SID source are both active.
; =====================================================================
update_voice_mode:
    lda MusicStatus
    and #AUDIO_STATUS_WTS
    beq @check_sid
    lda #MODE_WTS
    cmp zp_voice_mode
    beq @done
    sta zp_voice_mode
    lda #WTS_FIRST_VOICE
    sta zp_first_voice
    lda #WTS_VOICE_COUNT
    sta zp_voice_count
    jmp draw_voice_legend

@check_sid:
    lda MusicStatus
    and #AUDIO_STATUS_SID
    beq @done
    lda #MODE_SID
    cmp zp_voice_mode
    beq @done
    sta zp_voice_mode
    lda #SID_FIRST_VOICE
    sta zp_first_voice
    lda #SID_VOICE_COUNT
    sta zp_voice_count
    jmp draw_voice_legend

@done:
    rts

.ifdef KEYBOARD_TEST
; =====================================================================
; Synthetic-note harness for validating the keyboard renderer without music.
; =====================================================================
init_test_mode:
    lda #COL_CYAN
    sta RegFgCol
    lda #<str_test
    sta zp_str_ptr
    lda #>str_test
    sta zp_str_ptr+1
    lda #TITLE_ROW
    sta zp_tmp2
    jsr print_centered_cstr
    jsr draw_test_voice_legend

    lda #TEST_FIRST_NOTE
    sta test_current_note
    stz test_direction
    stz test_frame_count
    stz test_color_index
    jmp paint_test_current

update_test_sweep:
    inc test_frame_count
    lda test_frame_count
    cmp #TEST_FRAME_DELAY
    bcs @advance
    rts

@advance:
    stz test_frame_count

    ; Restore the currently highlighted key before moving to the next one.
    lda test_current_note
    sta zp_note
    jsr calc_key_coords
    bcs @skip_restore
    lda zp_is_black
    bne @restore_black
    lda #COL_WHITE
    bra @restore_key
@restore_black:
    lda #COL_KEY_BLACK
@restore_key:
    jsr paint_at_key
@skip_restore:
    jsr advance_test_note
    jmp paint_test_current

paint_test_current:
    lda test_current_note
    sta zp_note
    jsr calc_key_coords
    bcs @done
    ldx test_color_index
    lda voice_colors,x
    jsr paint_at_key

    inc test_color_index
    lda test_color_index
    cmp #MAX_VOICES
    bcc @done
    stz test_color_index
@done:
    rts

advance_test_note:
    lda test_direction
    bne @descending

    lda test_current_note
    cmp #TEST_LAST_NOTE
    bcs @turn_down
    inc test_current_note
    rts

@turn_down:
    lda #1
    sta test_direction
    lda #(TEST_LAST_NOTE - 1)
    sta test_current_note
    rts

@descending:
    lda test_current_note
    cmp #TEST_FIRST_NOTE
    beq @turn_up
    dec test_current_note
    rts

@turn_up:
    stz test_direction
    lda #(TEST_FIRST_NOTE + 1)
    sta test_current_note
    rts
.endif

; =====================================================================
; Draw one voice legend row for the active source.
; =====================================================================
draw_voice_legend:
    jsr clear_legend_area
    lda #COL_WHITE
    sta RegFgCol
    lda zp_voice_mode
    cmp #MODE_WTS
    beq draw_midi_legend_one_row
    jmp draw_sid_legend_one_row

clear_legend_area:
    lda #LEGEND_SID_ROW
    jsr clear_text_row
    lda #LEGEND_MIDI_ROW
    jmp clear_text_row

clear_text_row:
    sta RegCursorY
    stz RegCursorX
    lda #COL_WHITE
    sta RegFgCol
    ldx #0
@clr_loop:
    lda #' '
    sta RegCharOut
    inx
    cpx #80
    bcc @clr_loop
    rts

draw_sid_legend_one_row:
    lda #LEGEND_ONE_ROW
    sta RegCursorY
    lda #LEGEND_SID_X
    sta RegCursorX
    jsr print_sid_prefix
    stz zp_first_voice
    lda #SID_VOICE_COUNT
    sta zp_voice_count
    jmp draw_voice_legend_entries

draw_midi_legend_one_row:
    lda #LEGEND_ONE_ROW
    sta RegCursorY
    lda #LEGEND_MIDI_X
    sta RegCursorX
    jsr print_midi_prefix
    lda #WTS_FIRST_VOICE
    sta zp_first_voice
    lda #WTS_VOICE_COUNT
    sta zp_voice_count
    jmp draw_voice_legend_entries

.ifdef KEYBOARD_TEST
draw_test_voice_legend:
    jsr clear_legend_area
    lda #LEGEND_SID_ROW
    sta RegCursorY
    lda #LEGEND_SID_X
    sta RegCursorX
    jsr print_sid_prefix
    stz zp_first_voice
    lda #SID_VOICE_COUNT
    sta zp_voice_count
    jsr draw_voice_legend_entries

    lda #LEGEND_MIDI_ROW
    sta RegCursorY
    lda #LEGEND_MIDI_X
    sta RegCursorX
    jsr print_midi_prefix
    lda #WTS_FIRST_VOICE
    sta zp_first_voice
    lda #WTS_VOICE_COUNT
    sta zp_voice_count
    jmp draw_voice_legend_entries
.endif

print_sid_prefix:
    lda #COL_WHITE
    sta RegFgCol
    lda #'S'
    sta RegCharOut
    lda #'I'
    sta RegCharOut
    lda #'D'
    sta RegCharOut
    lda #' '
    sta RegCharOut
    lda #' '
    sta RegCharOut
    rts

print_midi_prefix:
    lda #COL_WHITE
    sta RegFgCol
    lda #'M'
    sta RegCharOut
    lda #'I'
    sta RegCharOut
    lda #'D'
    sta RegCharOut
    lda #'I'
    sta RegCharOut
    lda #' '
    sta RegCharOut
    rts

draw_voice_legend_entries:
    ldx #0
@legend_loop:
    cpx zp_voice_count
    bcs @done
    txa
    clc
    adc zp_first_voice
    tay
    lda voice_colors,y
    sta RegFgCol
    lda #$DB
    sta RegCharOut
    sta RegCharOut
    lda #' '
    sta RegCharOut

    lda #COL_WHITE
    sta RegFgCol
    txa
    clc
    adc #'1'
    sta RegCharOut
    lda #' '
    sta RegCharOut

    inx
    bra @legend_loop
@done:
    rts

; =====================================================================
; Update voices: flood-fill keys with voice colors.
; =====================================================================
.ifndef KEYBOARD_TEST
update_voices:
    ldx #0
@uv_loop:
    cpx zp_voice_count
    bcs @uv_done
    stx zp_voice_idx
    txa
    clc
    adc zp_first_voice
    sta zp_voice
    tax

.ifdef KEYBOARD_TEST
    lda test_notes,x
.else
    lda MusicNote1,x
.endif
    sta zp_note
    cmp prev_notes,x
    beq @uv_next

    ; Note changed — restore old key first
    lda prev_notes,x
    beq @uv_no_restore
    sta zp_note
    jsr calc_key_coords
    bcs @uv_no_restore
    lda zp_is_black
    bne @uv_rest_blk
    lda #COL_WHITE
    bra @uv_do_rest
@uv_rest_blk:
    lda #COL_KEY_BLACK
@uv_do_rest:
    jsr paint_at_key

@uv_no_restore:
    ; Store new note
    ldx zp_voice
.ifdef KEYBOARD_TEST
    lda test_notes,x
.else
    lda MusicNote1,x
.endif
    sta prev_notes,x
    sta zp_note
    beq @uv_next

    ; Paint new key with voice color
    jsr calc_key_coords
    bcs @uv_next
    ldx zp_voice
    lda voice_colors,x
    jsr paint_at_key

@uv_next:
    ldx zp_voice_idx
    inx
    bra @uv_loop
@uv_done:
    rts
.endif

; =====================================================================
; Calculate bounded paint rectangle origin for a key from MIDI note.
; Input:  zp_note
; Output: zp_key_x_lo/hi, zp_key_y, zp_is_black
; Carry:  clear=valid, set=out of range
; =====================================================================
calc_key_coords:
    lda zp_note
    sec
    sbc #MIDI_BASE
    bcs @ckc_ok1
    sec
    rts
@ckc_ok1:
    cmp #96
    bcc @ckc_ok2
    sec
    rts
@ckc_ok2:
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
    sta zp_tmp2             ; white key index (zp_tmp clobbered by mul_by_7)

    ; Row and octave-in-row
    lda zp_octave
    cmp #4
    bcs @ckc_lower
    stz zp_row
    sta zp_oct_in_row
    bra @ckc_calc_x
@ckc_lower:
    lda #1
    sta zp_row
    lda zp_octave
    sec
    sbc #4
    sta zp_oct_in_row

@ckc_calc_x:
    lda zp_oct_in_row
    jsr mul_by_7
    clc
    adc zp_tmp2             ; + key index in octave
    jsr mul_by_kw           ; zp_mul_r_lo/hi

    lda zp_is_black
    bne @ckc_black_x

    ; White key lower-body paint origin: x = global_wk*11 + KX + 1
    lda zp_mul_r_lo
    clc
    adc #(KX + 1)
    sta zp_key_x_lo
    lda zp_mul_r_hi
    adc #0
    sta zp_key_x_hi
    bra @ckc_y

@ckc_black_x:
    ; Black key top-left: x = global_wk*11 + KX + KW - BW/2
    lda zp_mul_r_lo
    clc
    adc #(KX + KW - BW/2)
    sta zp_key_x_lo
    lda zp_mul_r_hi
    adc #0
    sta zp_key_x_hi

@ckc_y:
    lda zp_row
    bne @ckc_y_lower
    lda #Y1
    bra @ckc_y_set
@ckc_y_lower:
    lda #Y2
@ckc_y_set:
    ; A = row base Y.
    sta zp_key_y
    clc
    rts

; =====================================================================
; Area-fill a key with given color.
; Input:  A = color, zp_key_x_lo/hi, zp_key_y, zp_is_black
; The seed is inside the key; existing black outlines/black keys bound the fill.
; =====================================================================
paint_at_key:
    ; Set draw color
    sta RegP0
    lda #CmdGcolor
    sta RegCmd
    jsr vgc_wait_cmd

    ldx zp_is_black
    bne @paint_black

    lda zp_key_x_lo
    clc
    adc #5
    sta RegP0
    lda zp_key_x_hi
    adc #0
    sta RegP1
    lda zp_key_y
    clc
    adc #32
    bra @paint_go

@paint_black:
    lda zp_key_x_lo
    clc
    adc #3
    sta RegP0
    lda zp_key_x_hi
    adc #0
    sta RegP1
    lda zp_key_y
    clc
    adc #12
@paint_go:
    sta RegP2
    stz RegP3
    lda #CmdPaint
    sta RegCmd
    jsr vgc_wait_cmd
    rts

; =====================================================================
; Update progress bar and time display
; =====================================================================
.ifndef KEYBOARD_TEST
update_progress:
    lda MusicTotalL
    ora MusicTotalH
    bne @up_has_total

    ; SID files do not carry duration, so leave the progress bar empty but
    ; still show the elapsed timer from MusicElapsedL/H.
    lda zp_prev_bar
    ora zp_prev_bar+1
    bne @up_clear_bar
    jmp @up_time
@up_clear_bar:
    stz zp_prev_bar
    stz zp_prev_bar+1

    lda #COL_BLACK
    sta RegP0
    lda #CmdGcolor
    sta RegCmd
    jsr vgc_wait_cmd

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
    lda #CmdFill
    sta RegCmd
    jsr vgc_wait_cmd

    jmp @up_time
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
    jsr vgc_wait_cmd

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
    jsr vgc_wait_cmd

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

    lda MusicTotalL
    ora MusicTotalH
    bne @up_time_with_total

    ; SID has no total duration; center the elapsed TIME field.
    lda #TIME_ROW
    sta RegCursorY
    lda #SID_TIME_X
    sta RegCursorX
    lda #COL_WHITE
    sta RegFgCol

    lda #<str_time
    sta zp_str_ptr
    lda #>str_time
    sta zp_str_ptr+1
    jsr print_str

    lda zp_quot_lo
    jsr print_dec2
    lda #':'
    sta RegCharOut
    lda zp_remain
    jsr print_dec2

    bra @up_done

@up_time_with_total:
    ; Display next to the TIME label.
    lda #TIME_ROW
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
.endif

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
.ifndef KEYBOARD_TEST
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
.endif

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
; String helpers
; =====================================================================

; Input: zp_str_ptr points at a null-terminated or null-padded string,
;        A = max length.
; Output: zp_tmp = length, A = length.
string_len_limited:
    sta zp_tmp
    ldy #0
@sll_loop:
    cpy zp_tmp
    bcs @sll_done
    lda (zp_str_ptr),y
    beq @sll_done
    iny
    bra @sll_loop
@sll_done:
    sty zp_tmp
    tya
    rts

; Input: zp_str_ptr points at a null-terminated string, zp_tmp2 = row.
print_centered_cstr:
    lda #63
    jsr string_len_limited
    ; fall through

; Input: zp_str_ptr points at bytes, zp_tmp = length, zp_tmp2 = row.
print_centered_buffer:
    lda zp_tmp2
    sta RegCursorY
    lda #80
    sec
    sbc zp_tmp
    lsr
    sta RegCursorX
    ldy #0
@pcb_loop:
    cpy zp_tmp
    bcs @pcb_done
    lda (zp_str_ptr),y
    beq @pcb_done
    sta RegCharOut
    iny
    bra @pcb_loop
@pcb_done:
    rts

; =====================================================================
; Data tables
; =====================================================================
.segment "RODATA"

; SID voices 0-5, then WTS voices 6-13
voice_colors:
    .byte COL_RED, COL_ORANGE, COL_YELLOW, COL_GREEN, COL_CYAN, COL_BLUE
    .byte 9, 10, 8, 12, 13, 14, 15, 4

; Pitch class tables (indexed 0-11)
;              C  C# D  D# E  F  F# G  G# A  A# B
key_type: .byte 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0
key_index:.byte 0, 0, 1, 1, 2, 3, 3, 4, 4, 5, 5, 6

.ifdef KEYBOARD_TEST
TEST_FIRST_NOTE = MIDI_BASE
TEST_LAST_NOTE  = MIDI_BASE + 95
TEST_FRAME_DELAY = 5
.endif

str_exit: .byte "PRESS ESC/Q TO EXIT", 0
str_time: .byte "TIME: ", 0
str_sid:  .byte "SID    ", 0
str_wts:  .byte "MIDI/WTS", 0
.ifndef KEYBOARD_TEST
STR_PLAYER_LEN = 12
STR_FONT_PREFIX_LEN = 6
str_player: .byte "MUSIC PLAYER", 0
str_font_prefix: .byte "FONT: ", 0
str_sid_source: .byte "SID", 0
str_sid_6581: .byte "SID 6581", 0
str_sid_8580: .byte "SID 8580", 0
str_sid_6581_stereo: .byte "SID 6581 STEREO", 0
str_sid_8580_stereo: .byte "SID 8580 STEREO", 0
str_midi_source: .byte "MIDI/WTS", 0
.endif
.ifdef KEYBOARD_TEST
str_test: .byte "KEYBOARD TEST - SYNTHETIC NOTES", 0
.endif

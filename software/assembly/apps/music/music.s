; music.s -- Nova Music Player (clean rebuild, no demo.s).
;
; Browse a curated SID/MIDI library by category; RETURN starts playback. Eight
; eighth-note sprites drift behind the text as ambient background. Built on the
; board-proven paced-output pattern: one vsync per text row (the VGC char path
; garbles on an unpaced full-screen blast) and a command-FIFO drain after every
; sprite op. Autoboots from an NDI at $7200.

.include "novavm.inc"
.import kbdviz_run              ; linked keyboard visualizer library
.import vgc_gtext                             ; gfx text (2x SUPERNOVA wordmark)
.import copper_list, copper_clear, copper_add, copper_on, copper_off

NUM_CAT       = 5
VISIBLE_ROWS  = 6
TITLE_ROW     = 2
TAB_ROW       = 9
LIST_TOP      = 11
NAV_ROW       = 21
PLAY_ROW      = 23
LIST_COL      = 4
BADGE_COL     = 40
ENTRY_SIZE    = 55
NUM_COL       = 9
T_COL         = 13
BADGE2_COL    = 54
MODEL_COL     = 60

COL_BLACK  = 0
COL_WHITE  = 1
COL_CYAN   = 3
COL_BLUE   = 6
COL_YELLOW = 7
COL_LGREEN = 13
COL_LBLUE  = 14
COL_LGRAY  = 15

.segment "ZEROPAGE"
zp_cat:   .res 1
zp_sel:   .res 1
zp_top:   .res 1
zp_count: .res 1
zp_last:  .res 1
zp_rowc:  .res 1
zp_ptr:   .res 2
zp_src:   .res 2
zp_tmp:   .res 1
zp_i:     .res 1
zp_old_sel: .res 1

.segment "BSS"
cat_ptr_lo: .res NUM_CAT
cat_ptr_hi: .res NUM_CAT
cat_cnt:    .res NUM_CAT
copper_tick: .res 1
copper_frame: .res 1

.segment "HEADER"
    .byte $00, $72

.segment "CODE"
start:
    sei
    cld
    ldx #$FF
    txs

    lda #2                      ; MODE_TEXT_GFX: text list + gfx wordmark + sprites
    sta RegMode
    stz RegBgCol
    lda #COL_WHITE
    sta RegFgCol
    lda #1
    sta RegFont
    stz RegCursorEnable
    stz RegBorder
    lda #$0C
    sta RegCharOut
    lda RegStatus
    sta zp_last

    jsr load_dirs
    stz zp_cat
    stz zp_sel
    stz zp_top
    jsr set_count

    jsr draw_header
    jsr draw_tabs
    jsr draw_nav
    jsr draw_list
    jsr init_notes

@drain:
    lda RegCharIn
    bne @drain

main_loop:
    jsr mus_vsync
    jsr update_notes
    jsr advance_copper
    lda RegCharIn
    beq main_loop
    cmp #$1E
    beq key_up
    cmp #$1F
    beq key_down
    cmp #$1C
    beq key_left
    cmp #$1D
    beq key_right
    cmp #$0D
    beq key_play
    bra main_loop

key_up:
    lda zp_sel
    beq @done                   ; at top -> no change
    sta zp_old_sel
    dec zp_sel
    lda zp_sel
    cmp zp_top
    bcs @tworow                 ; still in window -> repaint just the 2 rows
    dec zp_top
    jsr draw_list               ; scrolled -> full redraw
    jmp main_loop
@tworow:
    jsr redraw_sel
@done:
    jmp main_loop

key_down:
    lda zp_sel
    sta zp_old_sel
    clc
    adc #1
    cmp zp_count
    bcs @done                   ; at bottom -> no change
    sta zp_sel
    sec
    sbc zp_top
    cmp #VISIBLE_ROWS
    bcc @tworow                 ; still in window -> repaint just the 2 rows
    inc zp_top
    jsr draw_list               ; scrolled -> full redraw
    jmp main_loop
@tworow:
    jsr redraw_sel
@done:
    jmp main_loop

key_left:
    lda zp_cat
    beq @done
    dec zp_cat
    jsr new_category
@done:
    jmp main_loop

key_right:
    lda zp_cat
    cmp #NUM_CAT-1
    bcs @done
    inc zp_cat
    jsr new_category
@done:
    jmp main_loop

key_play:
    jsr copper_off              ; hide the browse chrome behind the visualizer
    jsr notes_off
    jsr start_play              ; build dir/name into FioName + start the song playing
    lda #0
    jsr kbdviz_run              ; A=0: visualize the running song; back on song end/ESC
    jsr redraw_all              ; rebuild Browse (draw_header re-enables the copper)
    jsr notes_on
    jmp main_loop

; redraw_all -- rebuild the whole browser screen (after returning from Perform)
redraw_all:
    lda #$0C
    sta RegCharOut
    lda #CmdGcls
    sta RegCmd
    jsr mus_cmdwait
    jsr draw_header
    jsr draw_tabs
    jsr draw_nav
    jsr draw_list
    rts

new_category:
    stz zp_sel
    stz zp_top
    jsr set_count
    jsr draw_tabs
    jsr draw_list
    rts

; ---- data plumbing --------------------------------------------------------
load_dirs:
    ldx #NUM_CAT-1
@l:
    lda track_ptrs_lo,x
    sta cat_ptr_lo,x
    lda track_ptrs_hi,x
    sta cat_ptr_hi,x
    lda track_counts,x
    sta cat_cnt,x
    dex
    bpl @l
    rts

set_count:
    ldx zp_cat
    lda cat_cnt,x
    sta zp_count
    rts

cat_base_ptr:
    ldx zp_cat
    lda cat_ptr_lo,x
    sta zp_src
    lda cat_ptr_hi,x
    sta zp_src+1
    rts

; zp_src -> selected entry (advance from category base by zp_sel * ENTRY_SIZE)
sel_ptr:
    jsr cat_base_ptr
    lda zp_sel
    beq @done
    tax
@adv:
    clc
    lda zp_src
    adc #ENTRY_SIZE
    sta zp_src
    bcc @adv2
    inc zp_src+1
@adv2:
    dex
    bne @adv
@done:
    rts

; ---- drawing (paced) ------------------------------------------------------
; draw_header -- the SUPERNOVA hero: cycling copper bars behind a 2x gfx wordmark.
draw_header:
    lda #CmdGcls                ; clear the gfx layer
    sta RegCmd
    jsr mus_cmdwait
    jsr build_copper_bars       ; RegBgCol gradient behind the wordmark
    jsr copper_on
    ; wordmark colour, then gfx text at 2x, centered near the top
    lda #COL_WHITE
    sta RegP0
    lda #CmdGcolor
    sta RegCmd
    jsr mus_cmdwait
    ldx #0
@wn:
    lda word_super,x
    sta FioName,x
    beq @wd
    inx
    bra @wn
@wd:
    stx FioNameLen
    lda #88
    sta RegP0                   ; x = (320-9*16)/2
    stz RegP1
    lda #14
    sta RegP2                   ; y
    stz RegP3
    stz RegP4                   ; font slot 0
    lda #2
    sta RegP5                   ; scale 2x
    jsr vgc_gtext
    jsr mus_cmdwait
    rts

; build_copper_bars -- copper list confined just wider than the wordmark: for each
; scanline behind it, set RegBgCol to a cycling colour at x=72 and back to black at
; x=248 (gfx space). 15 scanlines * 2 events = 30 (under the 32-event copper cap).
build_copper_bars:
    stz RegP0
    jsr copper_list             ; list 0
    jsr copper_clear
    ldx #0                      ; scanline index 0..14
@b:
    txa                         ; y = 10 + index (extends above + below the wordmark)
    clc
    adc #10
    sta zp_tmp
    lda #72                     ; colour on at x=72
    sta RegP0
    stz RegP1
    lda zp_tmp
    sta RegP2
    lda #1                      ; register 1 = RegBgCol
    sta RegP3
    stz RegP4
    txa                         ; colour = bar_palette[(index + copper_frame) & 15]
    clc
    adc copper_frame
    and #$0F
    tay
    lda bar_palette,y
    sta RegP5
    jsr copper_add
    jsr mus_cmdwait
    lda #248                    ; black off at x=248
    sta RegP0
    stz RegP1
    lda zp_tmp
    sta RegP2
    lda #1
    sta RegP3
    stz RegP4
    stz RegP5
    jsr copper_add
    jsr mus_cmdwait
    inx
    cpx #24
    bne @b
    rts

; advance_copper -- shift the band palette every 4th frame so the bars cycle.
advance_copper:
    inc copper_tick
    lda copper_tick
    cmp #4
    bcc @done
    stz copper_tick
    inc copper_frame
    jsr build_copper_bars
@done:
    rts

draw_tabs:
    lda #TAB_ROW
    sta RegCursorY
    lda #24                     ; centered: 32-char tab row in 80 cols
    sta RegCursorX
    ldx #0
@t:
    stx zp_i
    cpx zp_cat
    bne @dim
    lda #COL_YELLOW
    bra @setc
@dim:
    lda #COL_LGRAY
@setc:
    sta RegFgCol
    ldx zp_i
    lda catname_lo,x
    sta zp_ptr
    lda catname_hi,x
    sta zp_ptr+1
    jsr print_str
    lda #' '
    sta RegCharOut
    lda #' '
    sta RegCharOut
    ldx zp_i
    inx
    cpx #NUM_CAT
    bne @t
    rts

draw_nav:
    lda #NAV_ROW
    sta RegCursorY
    lda #16
    sta RegCursorX
    lda #<str_nav
    sta zp_ptr
    lda #>str_nav
    sta zp_ptr+1
    lda #COL_LGRAY
    jsr print_str_col
    rts

; draw_list -- 1a two-line rows: number, title, artist, badge, SID model; an accent
; gfx bar highlights the selected track. 6 visible tracks, 3 text rows each.
; draw_list -- full redraw of all visible tracks (scroll / category change). Keeps
; the notes + copper animating through the paced redraw so the display never freezes.
draw_list:
    stz zp_rowc
@l:
    jsr draw_track
    jsr mus_vsync
    jsr update_notes
    jsr advance_copper
    inc zp_rowc
    lda zp_rowc
    cmp #VISIBLE_ROWS
    bcc @l
    rts

; draw_track -- render one visible track (index in zp_rowc). Self-contained: computes
; its own entry pointer, clears its own 2 text rows + gfx highlight. Unpaced (a few
; cells, well under the FIFO limit) so a single-row update is instant.
draw_track:
    lda zp_rowc
    asl
    clc
    adc zp_rowc
    clc
    adc #LIST_TOP
    sta zp_tmp
    jsr ptr_for_row
    jsr clear_two_rows
    lda zp_top
    clc
    adc zp_rowc
    cmp zp_count
    bcc @draw
    jsr clear_track_gfx
    rts
@draw:
    cmp zp_sel
    bne @notsel
    lda #1
    sta zp_i
    jsr draw_highlight
    bra @num
@notsel:
    stz zp_i
    jsr clear_track_gfx
@num:
    lda zp_tmp
    sta RegCursorY
    lda #NUM_COL
    sta RegCursorX
    jsr sel_col_dim
    sta RegFgCol
    jsr print_num2
    lda zp_tmp
    sta RegCursorY
    lda #T_COL
    sta RegCursorX
    jsr sel_col_white
    sta RegFgCol
    lda zp_src
    sta zp_ptr
    lda zp_src+1
    sta zp_ptr+1
    jsr print_name
    lda zp_tmp
    clc
    adc #1
    sta RegCursorY
    lda #T_COL
    sta RegCursorX
    jsr sel_col_dim
    sta RegFgCol
    lda zp_src
    clc
    adc #33
    sta zp_ptr
    lda zp_src+1
    adc #0
    sta zp_ptr+1
    jsr print_str
    lda zp_tmp
    sta RegCursorY
    lda #BADGE2_COL
    sta RegCursorX
    ldy #32
    lda (zp_src),y
    cmp #$01
    bne @mid
    lda zp_i
    bne @sb
    lda #COL_LGREEN
    bra @sc
@sb:
    lda #COL_BLACK
@sc:
    sta RegFgCol
    lda #<str_sid
    sta zp_ptr
    lda #>str_sid
    sta zp_ptr+1
    jsr print_str
    bra @model
@mid:
    lda zp_i
    bne @mb
    lda #COL_LBLUE
    bra @mc
@mb:
    lda #COL_BLACK
@mc:
    sta RegFgCol
    lda #<str_mid
    sta zp_ptr
    lda #>str_mid
    sta zp_ptr+1
    jsr print_str
@model:
    lda zp_tmp
    sta RegCursorY
    lda #MODEL_COL
    sta RegCursorX
    jsr sel_col_dim
    sta RegFgCol
    lda zp_src
    clc
    adc #49
    sta zp_ptr
    lda zp_src+1
    adc #0
    sta zp_ptr+1
    jsr print_str
    rts

; ptr_for_row -- zp_src -> entry for visible row zp_rowc (cat base + (top+rowc)*ENTRY_SIZE)
ptr_for_row:
    jsr cat_base_ptr
    lda zp_top
    clc
    adc zp_rowc
    beq @done
    tax
@adv:
    clc
    lda zp_src
    adc #ENTRY_SIZE
    sta zp_src
    bcc @a2
    inc zp_src+1
@a2:
    dex
    bne @adv
@done:
    rts

; clear_track_gfx -- black-fill the 2-row gfx band of the track whose title row is zp_tmp
clear_track_gfx:
    stz RegP0
    lda #CmdGcolor
    sta RegCmd
    jsr mus_cmdwait
    lda #36
    sta RegP0
    stz RegP1
    lda zp_tmp
    asl
    asl
    sta RegP2
    stz RegP3
    lda #<284
    sta RegP4
    lda #>284
    sta RegP5
    lda zp_tmp
    asl
    asl
    clc
    adc #8
    sta RegP6
    stz RegP7
    lda #CmdFill
    sta RegCmd
    jsr mus_cmdwait
    rts

; redraw_sel -- fast 2-row update after an in-window up/down: repaint the previously
; selected row and the now-selected row. Unpaced -> effectively instant.
redraw_sel:
    lda zp_old_sel
    sec
    sbc zp_top
    sta zp_rowc
    jsr draw_track
    lda zp_sel
    sec
    sbc zp_top
    sta zp_rowc
    jsr draw_track
    rts

; sel_col_white -- A = black if selected (zp_i) else white
sel_col_white:
    lda zp_i
    bne @b
    lda #COL_WHITE
    rts
@b:
    lda #COL_BLACK
    rts

; sel_col_dim -- A = black if selected else mgray
sel_col_dim:
    lda zp_i
    bne @b
    lda #12
    rts
@b:
    lda #COL_BLACK
    rts

; print_num2 -- print (zp_top+zp_rowc+1) as two decimal digits
print_num2:
    lda zp_top
    clc
    adc zp_rowc
    clc
    adc #1
    ldx #'0'-1
@t:
    inx
    sec
    sbc #10
    bcs @t
    adc #10
    pha
    txa
    sta RegCharOut
    pla
    clc
    adc #'0'
    sta RegCharOut
    rts

; print_name -- (zp_ptr) folded a-z->A-Z, stop at '.' or null (drop extension)
print_name:
    ldy #0
@l:
    lda (zp_ptr),y
    beq @d
    cmp #'.'
    beq @d
    cmp #'a'
    bcc @e
    cmp #'z'+1
    bcs @e
    and #$DF
@e:
    sta RegCharOut
    iny
    cpy #32
    bcc @l
@d:
    rts

; clear_two_rows -- blank the title row (zp_tmp) and the artist row below it
clear_two_rows:
    lda zp_tmp
    jsr clear_one_row
    lda zp_tmp
    clc
    adc #1
clear_one_row:
    sta RegCursorY
    lda #NUM_COL
    sta RegCursorX
    lda #COL_BLACK
    sta RegFgCol
    ldx #58
@c:
    lda #' '
    sta RegCharOut
    dex
    bne @c
    rts

; clear_list_gfx -- black-fill the list gfx area (clears old highlight bars)
clear_list_gfx:
    stz RegP0
    lda #CmdGcolor
    sta RegCmd
    jsr mus_cmdwait
    lda #36
    sta RegP0
    stz RegP1
    lda #(LIST_TOP*4)
    sta RegP2
    stz RegP3
    lda #<284
    sta RegP4
    lda #>284
    sta RegP5
    lda #(LIST_TOP*4 + VISIBLE_ROWS*3*4)
    sta RegP6
    stz RegP7
    lda #CmdFill
    sta RegCmd
    jsr mus_cmdwait
    rts

; draw_highlight -- accent gfx bar behind the track whose title row is zp_tmp
draw_highlight:
    ldx zp_cat
    lda cat_accent,x
    sta RegP0
    lda #CmdGcolor
    sta RegCmd
    jsr mus_cmdwait
    lda #36
    sta RegP0
    stz RegP1
    lda zp_tmp
    asl
    asl
    sta RegP2
    stz RegP3
    lda #<284
    sta RegP4
    lda #>284
    sta RegP5
    lda zp_tmp
    asl
    asl
    clc
    adc #8
    sta RegP6
    stz RegP7
    lda #CmdFill
    sta RegCmd
    jsr mus_cmdwait
    rts

start_play:
    ; copy category dir into FioName
    ldx zp_cat
    lda catdir_lo,x
    sta zp_ptr
    lda catdir_hi,x
    sta zp_ptr+1
    ldy #0
@dir:
    lda (zp_ptr),y
    beq @slash
    sta FioName,y
    iny
    bra @dir
@slash:
    lda #'/'
    sta FioName,y
    iny
    sty zp_tmp                  ; FioName write offset
    jsr sel_ptr                 ; zp_src -> selected entry
    ; append the null-terminated name
    ldy #0
@nm:
    lda (zp_src),y
    beq @nmdone
    sty zp_i                    ; save src index
    ldy zp_tmp
    sta FioName,y
    inc zp_tmp
    ldy zp_i                    ; restore src index
    iny
    cpy #32
    bcc @nm
@nmdone:
    ldy zp_tmp
    lda #0
    sta FioName,y
    sty FioNameLen
    ldy #32                     ; type byte -> trigger SID/MID play (before kbdviz_run's own FIO use)
    lda (zp_src),y
    cmp #$01
    bne @mid
    lda #FioCmdSidPlay          ; the host now handles $08 (fio_sidplay -> SID VM); no NDK CLI
    sta FioCmd
    rts
@mid:
    lda #FioCmdMidPlay          ; MID: FIO command (the board handles $13)
    sta FioCmd
    rts

; ---- helpers --------------------------------------------------------------
print_str_col:
    sta RegFgCol
print_str:
    ldy #0
@l:
    lda (zp_ptr),y
    beq @d
    cmp #'a'
    bcc @e
    cmp #'z'+1
    bcs @e
    and #$DF
@e:
    sta RegCharOut
    iny
    bra @l
@d:
    rts

mus_vsync:
@w: lda RegStatus
    cmp zp_last
    beq @w
    sta zp_last
    rts

mus_cmdwait:
@c: lda RegCmd
    and #1
    bne @c
    rts

.segment "RODATA"
word_super:  .byte "SUPERNOVA", 0
bar_palette: .byte 6,6,14,14,3,3,3,3,14,14,6,6,4,4,6,6
str_nav:     .byte "UP/DOWN SELECT    LEFT/RIGHT SET    RETURN PLAY", 0
str_sid:     .byte "SID", 0
str_mid:     .byte "MID", 0

catname_0:   .byte "FEATURED", 0
catname_1:   .byte "2SID", 0
catname_2:   .byte "SID", 0
catname_3:   .byte "WTS", 0
catname_4:   .byte "ARCADE", 0
cat_accent:  .byte 3,4,13,14,7
catname_lo:  .byte <catname_0, <catname_1, <catname_2, <catname_3, <catname_4
catname_hi:  .byte >catname_0, >catname_1, >catname_2, >catname_3, >catname_4

catdir_0:    .byte "featured", 0
catdir_1:    .byte "2sid", 0
catdir_2:    .byte "sid", 0
catdir_3:    .byte "wts", 0
catdir_4:    .byte "arcade", 0
catdir_lo:   .byte <catdir_0, <catdir_1, <catdir_2, <catdir_3, <catdir_4
catdir_hi:   .byte >catdir_0, >catdir_1, >catdir_2, >catdir_3, >catdir_4

.include "music_tracks.inc"
.include "music_notes.inc"

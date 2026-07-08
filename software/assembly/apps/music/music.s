; music.s -- Nova Music Player (clean rebuild, no demo.s).
;
; Browse a curated SID/MIDI library by category; RETURN starts playback. Eight
; eighth-note sprites drift behind the text as ambient background. Built on the
; board-proven paced-output pattern: one vsync per text row (the VGC char path
; garbles on an unpaced full-screen blast) and a command-FIFO drain after every
; sprite op. Autoboots from an NDI at $7200.

.include "novavm.inc"

NUM_CAT       = 5
VISIBLE_ROWS  = 11
TITLE_ROW     = 2
TAB_ROW       = 5
LIST_TOP      = 8
NAV_ROW       = 21
PLAY_ROW      = 23
LIST_COL      = 4
BADGE_COL     = 40
ENTRY_SIZE    = 33

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

.segment "BSS"
cat_ptr_lo: .res NUM_CAT
cat_ptr_hi: .res NUM_CAT
cat_cnt:    .res NUM_CAT

.segment "HEADER"
    .byte $00, $72

.segment "CODE"
start:
    sei
    cld
    ldx #$FF
    txs

    lda #1
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
    beq @done
    dec zp_sel
    lda zp_sel
    cmp zp_top
    bcs @redraw
    dec zp_top
@redraw:
    jsr draw_list
@done:
    jmp main_loop

key_down:
    lda zp_sel
    clc
    adc #1
    cmp zp_count
    bcs @done
    sta zp_sel
    sec
    sbc zp_top
    cmp #VISIBLE_ROWS
    bcc @redraw
    inc zp_top
@redraw:
    jsr draw_list
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
    jsr start_play
    jmp main_loop

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
draw_header:
    lda #TITLE_ROW
    sta RegCursorY
    lda #34
    sta RegCursorX
    lda #<str_title
    sta zp_ptr
    lda #>str_title
    sta zp_ptr+1
    lda #COL_CYAN
    jsr print_str_col
    lda #(TITLE_ROW+1)
    sta RegCursorY
    lda #22
    sta RegCursorX
    lda #<str_sub
    sta zp_ptr
    lda #>str_sub
    sta zp_ptr+1
    lda #COL_LGRAY
    jsr print_str_col
    rts

draw_tabs:
    lda #TAB_ROW
    sta RegCursorY
    lda #6
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

draw_list:
    jsr cat_base_ptr
    ldx zp_top
    beq @noadv
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
@noadv:
    stz zp_rowc
@row:
    lda #LIST_TOP
    clc
    adc zp_rowc
    sta RegCursorY
    lda #LIST_COL
    sta RegCursorX
    lda zp_top
    clc
    adc zp_rowc
    cmp zp_count
    bcc @entry
    jmp @blank
@entry:
    lda zp_top
    clc
    adc zp_rowc
    cmp zp_sel
    bne @white
    lda #COL_CYAN
    bra @colset
@white:
    lda #COL_WHITE
@colset:
    sta RegFgCol
    ldy #0
@name:
    lda (zp_src),y
    beq @namedone
    cmp #'a'
    bcc @emit
    cmp #'z'+1
    bcs @emit
    and #$DF
@emit:
    sta RegCharOut
    iny
    cpy #32
    bcc @name
@namedone:
    tya
    clc
    adc #LIST_COL
@pad:
    cmp #BADGE_COL
    bcs @badge
    pha
    lda #' '
    sta RegCharOut
    pla
    inc a
    bra @pad
@badge:
    ldy #32
    lda (zp_src),y
    cmp #$01
    bne @mid
    lda #COL_LGREEN
    sta RegFgCol
    lda #<str_sid
    sta zp_ptr
    lda #>str_sid
    sta zp_ptr+1
    jsr print_str
    bra @adv_entry
@mid:
    cmp #$03
    bne @adv_entry
    lda #COL_LBLUE
    sta RegFgCol
    lda #<str_mid
    sta zp_ptr
    lda #>str_mid
    sta zp_ptr+1
    jsr print_str
@adv_entry:
    clc
    lda zp_src
    adc #ENTRY_SIZE
    sta zp_src
    bcc @next
    inc zp_src+1
    bra @next
@blank:
    lda #COL_WHITE
    sta RegFgCol
    lda #LIST_COL
@bl:
    cmp #(BADGE_COL+4)
    bcs @next
    pha
    lda #' '
    sta RegCharOut
    pla
    inc a
    bra @bl
@next:
    jsr mus_vsync
    inc zp_rowc
    lda zp_rowc
    cmp #VISIBLE_ROWS
    bcs @done
    jmp @row
@done:
    rts

; ---- playback -------------------------------------------------------------
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
    ldy #32                     ; type byte -> SID/MID play
    lda (zp_src),y
    cmp #$01
    bne @mid
    lda #FioCmdSidPlay
    sta FioCmd
    bra @show
@mid:
    lda #FioCmdMidPlay
    sta FioCmd
@show:
    lda #PLAY_ROW
    sta RegCursorY
    lda #4
    sta RegCursorX
    lda #COL_YELLOW
    sta RegFgCol
    ; "> " then the selected name (uppercased)
    lda #'>'
    sta RegCharOut
    lda #' '
    sta RegCharOut
    jsr sel_ptr
    ldy #0
@pn:
    lda (zp_src),y
    beq @pndone
    cmp #'a'
    bcc @pe
    cmp #'z'+1
    bcs @pe
    and #$DF
@pe:
    sta RegCharOut
    iny
    cpy #32
    bcc @pn
@pndone:
    ; pad a few trailing spaces to clear a previous longer title
    ldx #8
@pp:
    lda #' '
    sta RegCharOut
    dex
    bne @pp
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
str_title:   .byte "NOVA MUSIC", 0
str_sub:     .byte "CURATED SID + MIDI FROM THE ARCHIVE", 0
str_nav:     .byte "UP/DOWN SELECT    LEFT/RIGHT SET    RETURN PLAY", 0
str_sid:     .byte "SID", 0
str_mid:     .byte "MID", 0

catname_0:   .byte "FEATURED", 0
catname_1:   .byte "2SID", 0
catname_2:   .byte "SID", 0
catname_3:   .byte "WTS", 0
catname_4:   .byte "ARCADE", 0
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

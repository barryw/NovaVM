; demo.s -- Demo Disk Player for NovaVM
; A demo scene-style music browser and player that autoboots from NDI floppy.
; Coexists with the keyboard visualizer at $9000.
;
; Load address: $4000   Invoke: SYS $4000
; Assembler:    ca65 --cpu 65c02
; Linker:       ld65 -C demo.cfg

.include "novavm.inc"

; ---------------------------------------------------------------------------
; Layout constants
; ---------------------------------------------------------------------------
NUM_CATEGORIES  = 5                 ; number of music categories
MAX_FILES       = 40                ; max files per category
ENTRY_SIZE      = 33                ; 32 chars + null terminator per entry
VISIBLE_ROWS    = 11                ; visible file rows in browser list

; Screen row assignments (80x25 text grid)
ROW_TITLE       = 0                 ; title bar
ROW_SUBTITLE    = 1                 ; subtitle / tagline
ROW_TABS        = 3                 ; category tabs
ROW_DIVIDER     = 4                 ; divider line under tabs
ROW_LIST_TOP    = 5                 ; first visible file row
ROW_LIST_BOT    = 15                ; last visible file row (5+11-1)
ROW_SCROLLBAR   = 5                 ; scrollbar top (same as list top)
ROW_INFO        = 17                ; song info area
ROW_NAV         = 23                ; navigation help
ROW_SCROLL      = 24                ; scrolling credits ticker

; PETSCII character codes (mapped to ASCII positions via PETSCII Upper font)
; Screen codes 64-95 (shifted graphics) -> ASCII $80-$9F
; Screen codes 96-127 (C= key graphics) -> ASCII $A0-$BF
CH_HLINE        = $80               ; SC 64: horizontal bar (mid-height)
CH_VLINE        = $82               ; SC 66: vertical line
CH_BLOCK        = $8B               ; SC 75: solid block (all pixels)
CH_RNDTL        = $91               ; SC 81: rounded top-left corner
CH_RNDTR        = $92               ; SC 82: rounded top-right corner
CH_RNDBR        = $93               ; SC 83: rounded bottom-right corner
CH_RNDBL        = $94               ; SC 84: rounded bottom-left corner
CH_LHALF        = $A1               ; SC 97: left half block
CH_CROSS        = $B8               ; SC 120: cross/plus junction
CH_T_DOWN       = $AE               ; SC 110: T pointing down (horiz + vert down)
CH_T_UP         = $AD               ; SC 109: T pointing up (horiz + vert up)
CH_T_RIGHT      = $A8               ; SC 104: T pointing right (vert + horiz right)
CH_T_LEFT       = $AC               ; SC 108: T pointing left (vert + horiz left)
CH_CORNER_TR    = $A9               ; SC 105: corner top-right (horiz left + vert down)
CH_CORNER_BL    = $AA               ; SC 106: corner bottom-left (vert up + horiz right)
CH_CORNER_BR    = $AF               ; SC 111: corner bottom-right (vert up + horiz left)
CH_CORNER_TL    = $B5               ; SC 117: corner top-left (horiz left + vert down)
CH_TRIANGLE_R   = $81               ; SC 65: right-pointing triangle / arrow

; Color indices (NovaVM palette)
COL_BLACK       = 0
COL_WHITE       = 1
COL_RED         = 2
COL_CYAN        = 3
COL_PURPLE      = 4
COL_GREEN       = 5
COL_BLUE        = 6
COL_YELLOW      = 7
COL_ORANGE      = 8
COL_BROWN       = 9
COL_LRED        = 10
COL_DGRAY       = 11
COL_MGRAY       = 12
COL_LGREEN      = 13
COL_LBLUE       = 14
COL_LGRAY       = 15

; ---------------------------------------------------------------------------
; Zero page variables ($40-$7F -- safe user range)
; ---------------------------------------------------------------------------
.segment "ZEROPAGE"

zp_category:    .res 1              ; current category index (0-4)
zp_sel_index:   .res 1              ; selected file index within category
zp_scroll_top:  .res 1              ; first visible file index (scroll offset)
zp_file_count:  .res 1              ; number of files in current category
zp_copper_frame:.res 1              ; animation frame counter for copper effect
zp_scroll_pos:  .res 1              ; scroll text pixel/char offset
zp_scroll_idx:  .res 2              ; 16-bit index into scroll text
zp_kbd_loaded:  .res 1              ; nonzero if keyboard.bin has been loaded
zp_tmp:         .res 1              ; general temp
zp_tmp2:        .res 1              ; general temp 2
zp_tmp3:        .res 1              ; general temp 3
zp_tmp4:        .res 1              ; general temp 4
zp_ptr:         .res 2              ; general pointer (lo/hi)
zp_src:         .res 2              ; source pointer (lo/hi)
zp_dst:         .res 2              ; destination pointer (lo/hi)
zp_count:       .res 1              ; loop counter
zp_last_frame:  .res 1              ; last frame counter for vsync
zp_col:         .res 1              ; current column for printing
zp_row:         .res 1              ; current row for printing

; ---------------------------------------------------------------------------
; BSS section (uninitialized data)
; ---------------------------------------------------------------------------
.segment "BSS"

; Per-category file counts
cat_counts:     .res NUM_CATEGORIES

; Per-category pointers into dir_buffer (lo/hi byte tables)
cat_ptrs_lo:    .res NUM_CATEGORIES
cat_ptrs_hi:    .res NUM_CATEGORIES

; Directory buffer: 5 categories x 40 files x 33 bytes = 6600 bytes
dir_buffer:     .res NUM_CATEGORIES * MAX_FILES * ENTRY_SIZE

; Saved VGC state for clean restore on exit
save_mode:      .res 1
save_bgcol:     .res 1
save_fgcol:     .res 1
save_cursor:    .res 1
save_font:      .res 1
save_border:    .res 1

; Current playback file type (for stop_playback)
play_type:      .res 1

; ---------------------------------------------------------------------------
; Header -- 2-byte load address prefix
; ---------------------------------------------------------------------------
.segment "HEADER"
    .byte $00, $40

; ---------------------------------------------------------------------------
; RODATA -- constant data
; ---------------------------------------------------------------------------
.segment "RODATA"

; Category names (null-terminated) -- must match demo.ndi directory names
cat_name_0:     .byte "CLASSICAL", 0
cat_name_1:     .byte "MOVIES", 0
cat_name_2:     .byte "TV", 0
cat_name_3:     .byte "GAMES", 0
cat_name_4:     .byte "SID", 0

; Category name pointer tables
cat_names_lo:
    .byte <cat_name_0, <cat_name_1, <cat_name_2, <cat_name_3, <cat_name_4
cat_names_hi:
    .byte >cat_name_0, >cat_name_1, >cat_name_2, >cat_name_3, >cat_name_4

; Directory path strings (null-terminated) -- subdirectories on the NDI disk
cat_dir_0:      .byte "classical", 0
cat_dir_1:      .byte "movies", 0
cat_dir_2:      .byte "tv", 0
cat_dir_3:      .byte "games", 0
cat_dir_4:      .byte "sid", 0

; Directory path pointer tables
cat_dirs_lo:
    .byte <cat_dir_0, <cat_dir_1, <cat_dir_2, <cat_dir_3, <cat_dir_4
cat_dirs_hi:
    .byte >cat_dir_0, >cat_dir_1, >cat_dir_2, >cat_dir_3, >cat_dir_4

; Tab display widths (characters per tab label including padding)
tab_widths:     .byte 11, 8, 4, 7, 5

; Per-category accent colors
cat_colors:     .byte COL_CYAN, COL_LRED, COL_YELLOW, COL_LGREEN, COL_LBLUE

; Scrolling credits text (~300 chars, loops back to start)
scroll_text:
    .byte "     NOVAVM DEMO DISK  ///  "
    .byte "MUSIC COLLECTION BY THE NOVAVM CREW  ///  "
    .byte "GREETS TO ALL C64 SCENERS PAST AND PRESENT  ///  "
    .byte "CODE: BARRY  ART: NOVAVM  MUSIC: VARIOUS ARTISTS  ///  "
    .byte "USE LEFT/RIGHT TO BROWSE CATEGORIES  ///  "
    .byte "UP/DOWN TO SELECT ... PRESS RETURN TO PLAY  ///  "
    .byte "ACTIVE KEYBOARD VISUALIZER WITH PROGRESS BAR  ///  "
    .byte "THANKS FOR LISTENING!  ///  "
    .byte "    ", 0
SCROLL_TEXT_LEN = * - scroll_text - 1  ; length excluding null

; Navigation help text
nav_text:       .byte " LEFT/RIGHT:CATEGORY  UP/DOWN:SELECT  RETURN:PLAY  Q:QUIT", 0

; Keyboard visualizer filename (no extension — FileIO resolves it)
kbd_filename:   .byte "KEYBOARD", 0
KBD_NAME_LEN    = 8

; Root path for CD
root_path:      .byte "/", 0

; Raster palette -- 16 colors for sine-wave copper bar effect
raster_palette:
    .byte COL_BLACK, COL_DGRAY, COL_BLUE, COL_LBLUE
    .byte COL_CYAN, COL_LGREEN, COL_WHITE, COL_LGREEN
    .byte COL_CYAN, COL_LBLUE, COL_BLUE, COL_DGRAY
    .byte COL_BLACK, COL_BLACK, COL_BLACK, COL_BLACK

; Rainbow colors for title bar effect
rainbow_colors:
    .byte COL_RED, COL_ORANGE, COL_YELLOW, COL_GREEN
    .byte COL_CYAN, COL_BLUE, COL_PURPLE

RAINBOW_LEN = * - rainbow_colors

; Title and subtitle strings
title_str:      .byte "NOVAVM DEMO DISK", 0
subtitle_str:   .byte "MUSIC BROWSER & PLAYER V1.0", 0

; ---------------------------------------------------------------------------
; CODE
; ---------------------------------------------------------------------------
.segment "CODE"

; =====================================================================
; Entry point -- full browser and player
; =====================================================================
main:
    ; --- Save VGC state ---
    lda RegMode
    sta save_mode
    lda RegBgCol
    sta save_bgcol
    lda RegFgCol
    sta save_fgcol
    lda RegFont
    sta save_font
    lda RegCursorEnable
    sta save_cursor
    lda RegBorder
    sta save_border

    ; --- Set VGC: mode 1, black bg, white fg, PETSCII Upper (slot 1), cursor off ---
    lda #1
    sta RegMode
    lda #COL_BLACK
    sta RegBgCol
    lda #COL_WHITE
    sta RegFgCol
    lda #1
    sta RegFont
    stz RegCursorEnable

    ; --- Zero all state vars ---
    stz zp_category
    stz zp_sel_index
    stz zp_scroll_top
    stz zp_kbd_loaded
    stz zp_copper_frame

    ; --- Init screen and data ---
    jsr clear_text
    jsr draw_header
    jsr build_copper
    jsr load_all_dirs
    jsr load_keyboard

    ; Set file count for initial category
    ldx zp_category
    lda cat_counts,x
    sta zp_file_count

    jsr draw_tabs
    jsr draw_file_list
    jsr draw_nav
    jsr init_scroll

    ; --- Drain keyboard buffer ---
@drain:
    jsr V_INPT
    bcs @drain

    ; --- Snapshot frame counter ---
    lda RegStatus
    sta zp_last_frame

; =====================================================================
; Browser loop -- one iteration per frame
; =====================================================================
@browser_loop:
    jsr wait_vsync
    jsr advance_copper
    jsr advance_scroll

    ; Check for keypress
    jsr V_INPT
    bcc @browser_loop           ; no key -> next frame

    ; --- Handle keys ---
    cmp #$1D                    ; cursor RIGHT
    beq @key_right
    cmp #$9D                    ; cursor LEFT
    beq @key_left
    cmp #$11                    ; cursor DOWN
    bne :+
    jmp @key_down
:   cmp #$91                    ; cursor UP
    bne :+
    jmp @key_up
:
    cmp #$0D                    ; RETURN
    bne :+
    jmp @key_return
:   cmp #'q'
    bne :+
    jmp @key_quit
:   cmp #'Q'
    bne :+
    jmp @key_quit
:   jmp @browser_loop

; --- Cursor RIGHT: next category ---
@key_right:
    lda zp_category
    cmp #4
    bcs @browser_jmp            ; already at max
    inc zp_category
    jsr @switch_category
@browser_jmp:
    jmp @browser_loop

; --- Cursor LEFT: previous category ---
@key_left:
    lda zp_category
    beq @browser_jmp2           ; already at 0
    dec zp_category
    jsr @switch_category
@browser_jmp2:
    jmp @browser_loop

; --- Cursor DOWN: next file ---
@key_down:
    lda zp_sel_index
    inc a
    cmp zp_file_count
    bcs @browser_jmp3           ; at end of list
    sta zp_sel_index
    ; Check if we need to scroll
    sec
    sbc zp_scroll_top
    cmp #VISIBLE_ROWS
    bcc @down_no_scroll
    inc zp_scroll_top
@down_no_scroll:
    jsr draw_file_list
@browser_jmp3:
    jmp @browser_loop

; --- Cursor UP: previous file ---
@key_up:
    lda zp_sel_index
    beq @browser_jmp4           ; already at 0
    dec zp_sel_index
    lda zp_sel_index
    cmp zp_scroll_top
    bcs @up_no_scroll
    dec zp_scroll_top
@up_no_scroll:
    jsr draw_file_list
@browser_jmp4:
    jmp @browser_loop

; --- RETURN: play selected file ---
@key_return:
    lda zp_file_count
    beq @browser_jmp5           ; no files
    ; Disable copper
    lda #CmdCopperDisable
    sta RegCmd
    jsr start_playback
    ; If keyboard visualizer loaded, call it
    lda zp_kbd_loaded
    beq @no_kbd
    jsr $9000
@no_kbd:
    jsr stop_playback
    ; Restore VGC for browser
    lda #1
    sta RegMode
    lda #COL_BLACK
    sta RegBgCol
    lda #1
    sta RegFont
    stz RegCursorEnable
    ; Redraw everything
    jsr clear_text
    jsr draw_header
    jsr build_copper
    jsr draw_tabs
    jsr draw_file_list
    jsr draw_nav
    jsr init_scroll
    ; Drain keyboard buffer
@drain2:
    jsr V_INPT
    bcs @drain2
    ; Snapshot frame counter
    lda RegStatus
    sta zp_last_frame
@browser_jmp5:
    jmp @browser_loop

; --- Quit ---
@key_quit:
    ; Disable copper
    lda #CmdCopperDisable
    sta RegCmd
    ; Clear graphics
    lda #CmdGcls
    sta RegCmd
    ; Restore saved VGC state
    lda save_mode
    sta RegMode
    lda save_bgcol
    sta RegBgCol
    lda save_fgcol
    sta RegFgCol
    lda save_font
    sta RegFont
    lda save_cursor
    sta RegCursorEnable
    lda save_border
    sta RegBorder
    jsr clear_text
    rts

; --- Helper: switch category (reset selection, redraw) ---
@switch_category:
    stz zp_sel_index
    stz zp_scroll_top
    ldx zp_category
    lda cat_counts,x
    sta zp_file_count
    jsr draw_tabs
    jsr draw_file_list
    rts

; =====================================================================
; load_keyboard -- load keyboard visualizer binary to $9000
; Skips if already loaded (zp_kbd_loaded != 0).
; Clobbers: A, X, Y
; =====================================================================
load_keyboard:
    lda zp_kbd_loaded
    bne @lk_done                    ; already loaded

    jsr cd_root

    ; Copy "KEYBOARD" to FioName
    lda #<kbd_filename
    sta zp_src
    lda #>kbd_filename
    sta zp_src+1
    jsr copy_filename

    ; Set load address $9000
    stz FioSrcL
    lda #$90
    sta FioSrcH

    ; Issue load command
    lda #FioCmdLoad
    sta FioCmd

    ; Check status
    lda FioStatus
    cmp #FioStatusOk
    bne @lk_done

    lda #$FF
    sta zp_kbd_loaded
@lk_done:
    rts

; =====================================================================
; start_playback -- build path and start SID/MID playback
; Builds "dirname/filename" in FioName from current category and
; selected file entry, then issues the appropriate play command.
; Clobbers: A, X, Y, zp_src, zp_dst, zp_tmp, zp_tmp2
; =====================================================================
start_playback:
    ; --- Build directory name into FioName ---
    ldx zp_category
    lda cat_dirs_lo,x
    sta zp_src
    lda cat_dirs_hi,x
    sta zp_src+1

    ldy #0
@sp_copy_dir:
    lda (zp_src),y
    beq @sp_dir_done
    sta FioName,y
    iny
    bra @sp_copy_dir
@sp_dir_done:
    ; Append '/' separator
    lda #'/'
    sta FioName,y
    iny
    sty zp_tmp                      ; save current offset

    ; --- Find selected entry in dir_buffer ---
    ; Entry address = cat_ptrs[category] + sel_index * ENTRY_SIZE
    ldx zp_category
    lda cat_ptrs_lo,x
    sta zp_src
    lda cat_ptrs_hi,x
    sta zp_src+1

    lda zp_sel_index
    beq @sp_no_advance
    tax
@sp_advance:
    clc
    lda zp_src
    adc #ENTRY_SIZE
    sta zp_src
    lda zp_src+1
    adc #0
    sta zp_src+1
    dex
    bne @sp_advance
@sp_no_advance:

    ; --- Copy filename (first 32 bytes, null-terminated) ---
    ; zp_tmp = offset into FioName, zp_tmp2 = offset into entry
    stz zp_tmp2
@sp_copy_name:
    ldy zp_tmp2
    lda (zp_src),y
    beq @sp_name_done
    ldy zp_tmp
    sta FioName,y
    inc zp_tmp
    inc zp_tmp2
    lda zp_tmp2
    cmp #32
    bcc @sp_copy_name
@sp_name_done:
    ldy zp_tmp                      ; total length
    ; Null terminate and set length
    lda #0
    sta FioName,y
    sty FioNameLen

    ; --- Read file type from entry offset 32 ---
    ldy #32
    lda (zp_src),y
    sta play_type

    ; --- Issue play command based on type ---
    cmp #$01                        ; SID
    bne @sp_check_mid
    lda #FioCmdSidPlay
    sta FioCmd
    bra @sp_wait
@sp_check_mid:
    cmp #$03                        ; MID
    bne @sp_done
    lda #FioCmdMidPlay
    sta FioCmd

@sp_wait:
    ; Wait ~10 frames for playback to start
    ldx #10
@sp_wait_loop:
    jsr wait_vsync
    dex
    bne @sp_wait_loop
@sp_done:
    rts

; =====================================================================
; stop_playback -- stop current SID/MID playback
; Clobbers: A
; =====================================================================
stop_playback:
    lda play_type
    cmp #$01
    bne @stp_check_mid
    lda #FioCmdSidStop
    sta FioCmd
    rts
@stp_check_mid:
    cmp #$03
    bne @stp_done
    lda #FioCmdMidStop
    sta FioCmd
@stp_done:
    rts

; =====================================================================
; load_all_dirs -- scan all 5 category subdirectories into dir_buffer
; Populates cat_ptrs_lo/hi, cat_counts, and dir_buffer with up to
; MAX_FILES entries per category.  Each entry is 32 bytes of filename
; plus 1 byte of FioDirType.
; Clobbers: A, X, Y, zp_src, zp_dst, zp_count, zp_tmp
; =====================================================================
load_all_dirs:
    ; Initialize destination pointer to start of dir_buffer
    lda #<dir_buffer
    sta zp_dst
    lda #>dir_buffer
    sta zp_dst+1

    ldx #0                              ; category index
@lad_cat_loop:
    stx zp_tmp                         ; save category index

    ; Record this category's buffer start address
    lda zp_dst
    sta cat_ptrs_lo,x
    lda zp_dst+1
    sta cat_ptrs_hi,x

    ; CD into category subdirectory
    lda cat_dirs_lo,x
    sta zp_src
    lda cat_dirs_hi,x
    sta zp_src+1
    jsr do_cd

    ; Open directory listing (all files: FioNameLen=0)
    stz FioNameLen
    lda #FioCmdDirOpen
    sta FioCmd

    ; Read directory entries
    stz zp_count                        ; file count for this category
@lad_read_loop:
    lda #FioCmdDirRead
    sta FioCmd

    ; Check for end-of-directory
    lda FioStatus
    cmp #FioStatusOk
    bne @lad_dir_done                   ; not OK -> end of dir or error

    lda FioErrCode
    cmp #FioErrEndOfDir
    beq @lad_dir_done

    ; Skip subdirectories
    lda FioDirType
    cmp #FioDirTypeDir
    beq @lad_read_loop

    ; Copy 32 bytes of filename from FioName to (zp_dst)
    ldy #0
@lad_copy:
    lda FioName,y
    sta (zp_dst),y
    iny
    cpy #32
    bcc @lad_copy

    ; Store file type byte at offset 32
    lda FioDirType
    ldy #32
    sta (zp_dst),y

    ; Advance destination pointer by ENTRY_SIZE (33)
    clc
    lda zp_dst
    adc #ENTRY_SIZE
    sta zp_dst
    lda zp_dst+1
    adc #0
    sta zp_dst+1

    ; Increment file count, check limit
    inc zp_count
    lda zp_count
    cmp #MAX_FILES
    bcc @lad_read_loop

@lad_dir_done:
    ; Store file count for this category
    ldx zp_tmp
    lda zp_count
    sta cat_counts,x

    ; CD back to root
    jsr cd_root

    ; Next category
    inx
    cpx #NUM_CATEGORIES
    bcc @lad_cat_loop

    rts

; =====================================================================
; draw_header -- draw rainbow title header on the graphics layer
; Clears graphics, draws rainbow bars above/below the title text,
; and prints the centered title and subtitle on the text layer.
; Clobbers: A, X, Y, zp_ptr, zp_tmp, zp_tmp2
; =====================================================================
draw_header:
    ; Clear graphics layer
    lda #CmdGcls
    sta RegCmd

    ; --- Draw 7 rainbow bars above title (y=4 to y=18, each 2px tall) ---
    ldx #0                              ; color index
    lda #4                              ; starting y
    sta zp_tmp                          ; current y position
@dh_top_loop:
    ; Set draw color
    lda rainbow_colors,x
    sta RegP0
    lda #CmdGcolor
    sta RegCmd

    ; Fill rectangle: x1=20, y1=zp_tmp, x2=299, y2=zp_tmp+1
    lda #20                             ; x1 lo
    sta RegP0
    stz RegP1                           ; x1 hi
    lda zp_tmp                          ; y1 lo
    sta RegP2
    stz RegP3                           ; y1 hi
    lda #<299                           ; x2 lo
    sta RegP4
    lda #>299                           ; x2 hi
    sta RegP5
    lda zp_tmp                          ; y2 = y1 + 1
    inc a
    sta RegP6
    stz RegP7                           ; y2 hi
    lda #CmdFill
    sta RegCmd

    ; Advance y by 2
    lda zp_tmp
    clc
    adc #2
    sta zp_tmp

    inx
    cpx #RAINBOW_LEN
    bcc @dh_top_loop

    ; --- Print title centered on text row 3, col ~31 ---
    lda #31
    sta RegCursorX
    lda #3
    sta RegCursorY
    lda #<title_str
    sta zp_ptr
    lda #>title_str
    sta zp_ptr+1
    lda #COL_CYAN
    jsr print_str_color

    ; --- Print subtitle centered on text row 5, col ~26 ---
    lda #26
    sta RegCursorX
    lda #5
    sta RegCursorY
    lda #<subtitle_str
    sta zp_ptr
    lda #>subtitle_str
    sta zp_ptr+1
    lda #COL_LGRAY
    jsr print_str_color

    ; --- Draw 7 rainbow bars below title (y=46 to y=60, same pattern) ---
    ldx #0
    lda #46
    sta zp_tmp
@dh_bot_loop:
    lda rainbow_colors,x
    sta RegP0
    lda #CmdGcolor
    sta RegCmd

    lda #20
    sta RegP0
    stz RegP1
    lda zp_tmp
    sta RegP2
    stz RegP3
    lda #<299
    sta RegP4
    lda #>299
    sta RegP5
    lda zp_tmp
    inc a
    sta RegP6
    stz RegP7
    lda #CmdFill
    sta RegCmd

    lda zp_tmp
    clc
    adc #2
    sta zp_tmp

    inx
    cpx #RAINBOW_LEN
    bcc @dh_bot_loop

    rts

; =====================================================================
; build_copper -- pre-build 32 copper animation lists for raster bars
; Each list writes a cycling color palette to RegBgCol across scanlines
; 0-59 (step 4), then resets to black at scanline 60.
; Clobbers: A, X, Y, zp_tmp, zp_tmp2, zp_tmp3
; =====================================================================
build_copper:
    stz zp_tmp3                         ; frame_offset (list index 0-31)

@bc_list_loop:
    ; Select target copper list
    lda zp_tmp3
    sta RegP0
    lda #CmdCopperList
    sta RegCmd

    ; Clear it
    lda #CmdCopperClear
    sta RegCmd

    ; Add entries for scanlines 0, 4, 8, ... 56 (15 entries)
    lda #0
    sta zp_tmp                          ; current scanline
@bc_scan_loop:
    ; CopperAdd: P0/P1=x(0), P2=y, P3=register(1=RegBgCol), P4=0, P5=color
    stz RegP0                           ; x lo = 0
    stz RegP1                           ; x hi = 0
    lda zp_tmp
    sta RegP2                           ; y = scanline
    lda #1                              ; register index 1 = RegBgCol offset
    sta RegP3
    stz RegP4                           ; reserved = 0

    ; Color = raster_palette[(scanline/4 + frame_offset) % 16]
    lda zp_tmp
    lsr a
    lsr a                               ; scanline / 4
    clc
    adc zp_tmp3                         ; + frame_offset
    and #$0F                            ; % 16
    tax
    lda raster_palette,x
    sta RegP5                           ; color value

    lda #CmdCopperAdd
    sta RegCmd

    ; Next scanline (step 4)
    lda zp_tmp
    clc
    adc #4
    sta zp_tmp
    cmp #60
    bcc @bc_scan_loop

    ; Final entry at scanline 60: reset RegBgCol to black
    stz RegP0
    stz RegP1
    lda #60
    sta RegP2
    lda #1
    sta RegP3
    stz RegP4
    lda #COL_BLACK
    sta RegP5
    lda #CmdCopperAdd
    sta RegCmd

    ; Next list
    inc zp_tmp3
    lda zp_tmp3
    cmp #32
    bcc @bc_list_loop

    ; Activate list 0 and enable copper
    stz RegP0
    lda #CmdCopperUse
    sta RegCmd
    lda #CmdCopperEnable
    sta RegCmd

    rts

; =====================================================================
; advance_copper -- advance to next copper animation frame
; Called once per vsync.  Increments zp_copper_frame (0-31, wraps)
; and switches the active copper list.
; Clobbers: A
; =====================================================================
advance_copper:
    lda zp_copper_frame
    inc a
    and #$1F                            ; wrap 0-31
    sta zp_copper_frame
    sta RegP0
    lda #CmdCopperUse
    sta RegCmd
    rts

; =====================================================================
; wait_vsync -- wait for VGC frame counter to change
; Polls RegStatus ($A008) until the frame counter increments.
; =====================================================================
wait_vsync:
@wv:
    lda RegStatus
    cmp zp_last_frame
    beq @wv
    sta zp_last_frame
    rts

; =====================================================================
; clear_text -- fill all 80x25 text cells with spaces
; Uses RegCursorX/Y and RegCharOut to write each cell.
; Clobbers: A, X, Y
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
; print_str -- print null-terminated string at (zp_ptr)
; Outputs each byte via RegCharOut until a zero byte is encountered.
; Input:  zp_ptr = pointer to string
; Clobbers: A, Y
; =====================================================================
print_str:
    ldy #0
@ps_loop:
    lda (zp_ptr),y
    beq @ps_done
    sta RegCharOut
    iny
    bra @ps_loop
@ps_done:
    rts

; =====================================================================
; print_str_color -- set foreground color then print string
; Input:  A = color index, zp_ptr = pointer to string
; Clobbers: A, Y
; =====================================================================
print_str_color:
    sta RegFgCol
    jsr print_str
    rts

; =====================================================================
; copy_filename -- copy null-terminated string from (zp_src) to FioName
; Also sets FioNameLen to the string length.
; Input:  zp_src = pointer to null-terminated filename
; Clobbers: A, X, Y
; =====================================================================
copy_filename:
    ldy #0
@cf_loop:
    lda (zp_src),y
    sta FioName,y
    beq @cf_done                    ; hit null terminator
    iny
    cpy #(FioNameEnd - FioName)     ; guard against overflow
    bcc @cf_loop
    lda #0                          ; force-terminate if too long
    sta FioName,y
@cf_done:
    sty FioNameLen
    rts

; =====================================================================
; do_cd -- change directory to string at (zp_src)
; Copies filename then issues FioCmdCd command.
; Input:  zp_src = pointer to null-terminated directory name
; Clobbers: A, X, Y
; =====================================================================
do_cd:
    jsr copy_filename
    lda #FioCmdCd
    sta FioCmd
    rts

; =====================================================================
; cd_root -- change to root directory "/"
; Clobbers: A, X, Y
; =====================================================================
cd_root:
    lda #<root_path
    sta zp_src
    lda #>root_path
    sta zp_src+1
    jmp do_cd

; =====================================================================
; fill_row -- fill a text row with a character in a given color
; Input:  A = character, X = color, Y = row number
; Clobbers: A, X, Y, zp_tmp, zp_tmp2
; =====================================================================
fill_row:
    sta zp_tmp                      ; save character
    stx RegFgCol                    ; set color
    sty RegCursorY                  ; set row
    ldx #0
@fr_loop:
    stx RegCursorX
    lda zp_tmp
    sta RegCharOut
    inx
    cpx #80
    bcc @fr_loop
    rts

; =====================================================================
; draw_tabs -- draw the PETSCII category tab bar
; Draws a divider line on ROW_DIVIDER, then tabs on ROW_TABS.
; Active tab gets rounded-corner borders and an open bottom gap.
; Input:  zp_category = active tab (0-4)
; Clobbers: A, X, Y, zp_tmp, zp_tmp2, zp_tmp3, zp_tmp4, zp_col
; =====================================================================
draw_tabs:
    ; --- Fill ROW_DIVIDER with CH_HLINE in COL_DGRAY ---
    lda #CH_HLINE
    ldx #COL_DGRAY
    ldy #ROW_DIVIDER
    jsr fill_row

    ; --- Draw each tab on ROW_TABS ---
    lda #2
    sta zp_col                      ; running column position, start at col 2

    ldx #0                          ; tab index
@dt_tab_loop:
    stx zp_tmp4                     ; save tab index

    ; Get this tab's width
    lda tab_widths,x
    sta zp_tmp2                     ; tab label width

    ; Check if this is the active tab
    cpx zp_category
    beq @dt_active_tab

    ; --- INACTIVE tab: print name in COL_DGRAY ---
    lda zp_col
    inc a                           ; col + 1 (skip left border column)
    sta RegCursorX
    lda #ROW_TABS
    sta RegCursorY
    lda #COL_DGRAY
    sta RegFgCol

    ; Set pointer to category name
    ldx zp_tmp4
    lda cat_names_lo,x
    sta zp_ptr
    lda cat_names_hi,x
    sta zp_ptr+1
    jsr print_str

    jmp @dt_next_tab

@dt_active_tab:
    ; --- ACTIVE tab: draw bordered tab with open bottom ---

    ; Get accent color for this category
    ldx zp_tmp4
    lda cat_colors,x
    sta zp_tmp3                     ; save accent color

    ; -- ROW_TABS: draw top border --
    ; CH_RNDTL at zp_col
    lda #ROW_TABS
    sta RegCursorY
    lda zp_col
    sta RegCursorX
    lda #COL_WHITE
    sta RegFgCol
    lda #CH_RNDTL
    sta RegCharOut

    ; CH_HLINE across the top (tab_width characters)
    lda zp_col
    inc a
    sta RegCursorX
    ldy #0
@dt_top_line:
    lda #CH_HLINE
    sta RegCharOut
    ; Advance cursor manually
    lda RegCursorX
    inc a
    sta RegCursorX
    iny
    cpy zp_tmp2
    bcc @dt_top_line

    ; CH_RNDTR at end
    lda #CH_RNDTR
    sta RegCharOut

    ; -- ROW_TABS: overwrite hlines with category name in COL_WHITE --
    lda zp_col
    inc a                           ; inside left border
    sta RegCursorX
    lda #ROW_TABS
    sta RegCursorY
    lda #COL_WHITE
    sta RegFgCol
    ldx zp_tmp4
    lda cat_names_lo,x
    sta zp_ptr
    lda cat_names_hi,x
    sta zp_ptr+1
    jsr print_str

    ; -- ROW_DIVIDER: draw open bottom (VLINE + spaces + VLINE) --
    lda #ROW_DIVIDER
    sta RegCursorY

    ; CH_VLINE at left edge (zp_col)
    lda zp_col
    sta RegCursorX
    lda #COL_WHITE
    sta RegFgCol
    lda #CH_VLINE
    sta RegCharOut

    ; Spaces inside (clear the hline)
    lda zp_col
    inc a
    sta RegCursorX
    ldy #0
@dt_clear_div:
    lda #' '
    sta RegCharOut
    lda RegCursorX
    inc a
    sta RegCursorX
    iny
    cpy zp_tmp2
    bcc @dt_clear_div

    ; CH_VLINE at right edge
    ; Right edge = zp_col + tab_width + 1
    lda zp_col
    clc
    adc zp_tmp2
    inc a
    sta RegCursorX
    lda #CH_VLINE
    sta RegCharOut

@dt_next_tab:
    ; Advance column: zp_col += tab_width + 3
    lda zp_col
    clc
    adc zp_tmp2
    adc #3
    sta zp_col

    ldx zp_tmp4
    inx
    cpx #NUM_CATEGORIES
    bcs :+
    jmp @dt_tab_loop
:
    rts

; =====================================================================
; draw_file_list -- display the scrollable file list
; Shows VISIBLE_ROWS files starting from zp_scroll_top, highlighting
; the selected entry (zp_sel_index) with the category accent color.
; Input:  zp_category, zp_sel_index, zp_scroll_top
; Clobbers: A, X, Y, zp_tmp, zp_tmp2, zp_tmp3, zp_ptr, zp_src
; =====================================================================
draw_file_list:
    ; Get file count for current category
    ldx zp_category
    lda cat_counts,x
    sta zp_tmp3                     ; total file count

    ; Get base pointer for current category's entries
    lda cat_ptrs_lo,x
    sta zp_src
    lda cat_ptrs_hi,x
    sta zp_src+1

    ; Advance pointer to scroll_top entry: src += scroll_top * ENTRY_SIZE
    lda zp_scroll_top
    beq @dfl_no_advance
    tax                             ; loop counter
@dfl_advance:
    clc
    lda zp_src
    adc #ENTRY_SIZE
    sta zp_src
    lda zp_src+1
    adc #0
    sta zp_src+1
    dex
    bne @dfl_advance
@dfl_no_advance:

    ; zp_src now points to the first visible entry
    lda #0
    sta zp_tmp                      ; visible row counter (0 to VISIBLE_ROWS-1)

@dfl_row_loop:
    ; Set cursor to ROW_LIST_TOP + row, col 3
    lda #ROW_LIST_TOP
    clc
    adc zp_tmp
    sta RegCursorY
    lda #3
    sta RegCursorX

    ; Calculate absolute file index = scroll_top + row
    lda zp_scroll_top
    clc
    adc zp_tmp
    sta zp_tmp2                     ; absolute index

    ; Past end of file list?
    cmp zp_tmp3
    bcc @dfl_has_entry
    jmp @dfl_clear_row
@dfl_has_entry:

    ; Determine color: selected = accent, else white
    cmp zp_sel_index
    bne @dfl_not_selected
    ldx zp_category
    lda cat_colors,x
    bra @dfl_set_color
@dfl_not_selected:
    lda #COL_WHITE
@dfl_set_color:
    sta RegFgCol

    ; Print filename from (zp_src), up to 32 chars, stop at null
    ldy #0
@dfl_name_loop:
    lda (zp_src),y
    beq @dfl_pad_name               ; null terminator
    sta RegCharOut
    iny
    cpy #32
    bcc @dfl_name_loop

@dfl_pad_name:
    ; Pad with spaces to column 44
    ; Current column = 3 + Y (characters printed)
    tya
    clc
    adc #3                          ; actual column
@dfl_pad_loop:
    cmp #44
    bcs @dfl_print_badge
    pha
    lda #' '
    sta RegCharOut
    pla
    inc a
    sta RegCursorX
    bra @dfl_pad_loop

@dfl_print_badge:
    ; Check file type at offset 32 of entry
    ldy #32
    lda (zp_src),y

    cmp #$01                        ; SID type
    bne @dfl_check_mid
    ; Print "SID" in COL_LGREEN
    lda #COL_LGREEN
    sta RegFgCol
    lda #'S'
    sta RegCharOut
    lda #'I'
    sta RegCharOut
    lda #'D'
    sta RegCharOut
    bra @dfl_pad_end

@dfl_check_mid:
    cmp #$03                        ; MID type
    bne @dfl_pad_end
    ; Print "MID" in COL_LBLUE
    lda #COL_LBLUE
    sta RegFgCol
    lda #'M'
    sta RegCharOut
    lda #'I'
    sta RegCharOut
    lda #'D'
    sta RegCharOut

@dfl_pad_end:
    ; Pad rest of line with spaces to column 78
    ; Current column after badge = 44 + 3 = 47 (or 44 if no badge)
    ; Just set cursor and fill
    lda RegCursorX
@dfl_trail_pad:
    cmp #78
    bcs @dfl_next_entry
    sta RegCursorX
    pha
    lda #' '
    sta RegCharOut
    pla
    inc a
    bra @dfl_trail_pad

@dfl_next_entry:
    ; Advance source pointer by ENTRY_SIZE
    clc
    lda zp_src
    adc #ENTRY_SIZE
    sta zp_src
    lda zp_src+1
    adc #0
    sta zp_src+1

    ; Next row
    inc zp_tmp
    lda zp_tmp
    cmp #VISIBLE_ROWS
    bcs @dfl_done
    jmp @dfl_row_loop
@dfl_done:
    rts

@dfl_clear_row:
    ; Clear this row with spaces to col 78
    lda #COL_WHITE
    sta RegFgCol
    lda #3
@dfl_clear_loop:
    cmp #78
    bcs @dfl_clear_next
    sta RegCursorX
    pha
    lda #' '
    sta RegCharOut
    pla
    inc a
    bra @dfl_clear_loop
@dfl_clear_next:
    ; No entry here — don't advance src pointer
    inc zp_tmp
    lda zp_tmp
    cmp #VISIBLE_ROWS
    bcs @dfl_clear_done
    jmp @dfl_row_loop
@dfl_clear_done:
    rts

; =====================================================================
; init_scroll -- initialize the scroll text engine
; Zeros the scroll index and fills ROW_SCROLL with spaces in COL_LGREEN.
; Clobbers: A, X, Y
; =====================================================================
init_scroll:
    ; Zero the 16-bit scroll index
    stz zp_scroll_idx
    stz zp_scroll_idx+1

    ; Fill ROW_SCROLL with spaces in COL_LGREEN
    lda #' '
    ldx #COL_LGREEN
    ldy #ROW_SCROLL
    jsr fill_row
    rts

; =====================================================================
; advance_scroll -- shift ROW_SCROLL left by one char, append next char
; Called once per frame.  Directly accesses CharRam for speed.
; Clobbers: A, X, Y, zp_ptr
; =====================================================================
advance_scroll:
    ; Row 24 starts at CharRamBase + 24*80 = CharRamBase + 1920
    SCROLL_ROW_ADDR = CharRamBase + 1920

    ; Shift bytes left: copy [offset+1] to [offset] for 79 iterations
    ldx #0
@as_shift:
    lda SCROLL_ROW_ADDR+1,x
    sta SCROLL_ROW_ADDR,x
    inx
    cpx #79
    bcc @as_shift

    ; Get next character from scroll_text using 16-bit index
    lda zp_scroll_idx
    sta zp_ptr
    lda zp_scroll_idx+1
    sta zp_ptr+1
    ; Add base address of scroll_text
    clc
    lda zp_ptr
    adc #<scroll_text
    sta zp_ptr
    lda zp_ptr+1
    adc #>scroll_text
    sta zp_ptr+1

    ; Read character
    ldy #0
    lda (zp_ptr),y
    bne @as_got_char

    ; Null terminator — wrap index to 0
    stz zp_scroll_idx
    stz zp_scroll_idx+1
    ; Get first character
    lda scroll_text
@as_got_char:
    ; Write to column 79
    sta SCROLL_ROW_ADDR+79

    ; Increment 16-bit scroll index
    inc zp_scroll_idx
    bne @as_no_carry
    inc zp_scroll_idx+1
@as_no_carry:

    ; Check if index reached SCROLL_TEXT_LEN — wrap to 0
    lda zp_scroll_idx+1
    cmp #>SCROLL_TEXT_LEN
    bcc @as_done                    ; hi byte less, no wrap
    bne @as_wrap                    ; hi byte greater, wrap
    lda zp_scroll_idx
    cmp #<SCROLL_TEXT_LEN
    bcc @as_done                    ; lo byte less, no wrap
@as_wrap:
    stz zp_scroll_idx
    stz zp_scroll_idx+1
@as_done:
    rts

; =====================================================================
; draw_nav -- draw the navigation help bar on ROW_NAV
; Prints nav_text centered in COL_LGRAY.
; Clobbers: A, Y, zp_ptr
; =====================================================================
draw_nav:
    lda #11
    sta RegCursorX
    lda #ROW_NAV
    sta RegCursorY
    lda #<nav_text
    sta zp_ptr
    lda #>nav_text
    sta zp_ptr+1
    lda #COL_LGRAY
    jsr print_str_color
    rts

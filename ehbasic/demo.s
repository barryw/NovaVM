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
; Entry point -- stub for now, full main loop comes in a later task
; =====================================================================
main:
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

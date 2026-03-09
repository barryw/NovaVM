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

; Category names (null-terminated)
cat_name_0:     .byte "SID TUNES", 0
cat_name_1:     .byte "MIDI MUSIC", 0
cat_name_2:     .byte "CHIPTUNES", 0
cat_name_3:     .byte "CLASSICAL", 0
cat_name_4:     .byte "DEMOS", 0

; Category name pointer tables
cat_names_lo:
    .byte <cat_name_0, <cat_name_1, <cat_name_2, <cat_name_3, <cat_name_4
cat_names_hi:
    .byte >cat_name_0, >cat_name_1, >cat_name_2, >cat_name_3, >cat_name_4

; Directory path strings (null-terminated) -- subdirectories on the NDI disk
cat_dir_0:      .byte "sid", 0
cat_dir_1:      .byte "midi", 0
cat_dir_2:      .byte "chip", 0
cat_dir_3:      .byte "classical", 0
cat_dir_4:      .byte "demos", 0

; Directory path pointer tables
cat_dirs_lo:
    .byte <cat_dir_0, <cat_dir_1, <cat_dir_2, <cat_dir_3, <cat_dir_4
cat_dirs_hi:
    .byte >cat_dir_0, >cat_dir_1, >cat_dir_2, >cat_dir_3, >cat_dir_4

; Tab display widths (characters per tab label including padding)
tab_widths:     .byte 12, 13, 12, 12, 9

; Per-category accent colors
cat_colors:     .byte COL_CYAN, COL_GREEN, COL_YELLOW, COL_PURPLE, COL_ORANGE

; Scrolling credits text (~300 chars, loops back to start)
scroll_text:
    .byte "     NOVAVM DEMO DISK  ///  "
    .byte "MUSIC COLLECTION BY THE NOVAVM CREW  ///  "
    .byte "GREETS TO ALL C64 SCENERS PAST AND PRESENT  ///  "
    .byte "CODE: BARRY  ART: NOVAVM  MUSIC: VARIOUS ARTISTS  ///  "
    .byte "PRESS F1-F5 TO SWITCH CATEGORIES  ///  "
    .byte "THANKS FOR LISTENING!  ///  "
    .byte "    ", 0
SCROLL_TEXT_LEN = * - scroll_text - 1  ; length excluding null

; Navigation help text
nav_text:       .byte "CRSR:BROWSE  RETURN:PLAY  ESC:STOP  Q:QUIT", 0

; Keyboard visualizer filename
kbd_filename:   .byte "keyboard.bin", 0

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

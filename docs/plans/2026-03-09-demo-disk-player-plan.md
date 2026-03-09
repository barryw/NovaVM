# Demo Disk Player Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Build AUTOBOOT.bin — a demo scene-style music browser and player for the NovaVM demo disk, written in 65C02 assembly.

**Architecture:** Single assembly file (demo.s) at $4000, coexisting with keyboard.bin at $9000. Two screens: a copper-animated browser with PETSCII tabbed categories and scroll text, and the existing keyboard visualizer for playback. All file I/O through the FileIoController registers at $B9A0-$B9EF.

**Tech Stack:** ca65/ld65 (65C02 mode), novavm.inc hardware constants, VGC mode 1, copper coprocessor, PETSCII Upper font (slot 1)

---

### Task 1: Linker Config and Build Infrastructure

**Files:**
- Create: `ehbasic/demo.cfg`
- Modify: `ehbasic/Makefile`

**Step 1: Create linker config**

Create `ehbasic/demo.cfg`:

```
# demo.cfg -- ld65 config for demo disk autoboot binary
# Emits 2-byte load address prefix ($00 $40) followed by code at $4000

MEMORY {
    ZP:   start = $0040, size = $40, type = rw;
    HDR:  start = $0000, size = 2, fill = yes;
    CODE: start = $4000, size = $5000;
}
SEGMENTS {
    ZEROPAGE: load = ZP,   type = zp;
    HEADER:   load = HDR,  type = ro;
    CODE:     load = CODE, type = ro;
    RODATA:   load = CODE, type = ro;
    BSS:      load = CODE, type = bss, define = yes;
}
```

**Step 2: Add demo target to Makefile**

Add these rules to `ehbasic/Makefile` before the `clean:` target:

```makefile
# Demo disk autoboot program
demo: demo.bin

demo.bin: demo.o demo.cfg
	ld65 -C demo.cfg -o demo.bin demo.o

demo.o: demo.s novavm.inc
	ca65 --cpu 65c02 demo.s -o demo.o
```

Update the `all:` target to include `demo.bin`:
```makefile
all: basic.bin keyboard.bin demo.bin tokens.json
```

Update the `clean:` target — it already deletes `*.bin` so no change needed.

**Step 3: Create minimal demo.s stub**

Create `ehbasic/demo.s` with a minimal stub that assembles and links:

```asm
; demo.s -- NovaVM Demo Disk autoboot program
; Load address: $4000   Autoboot: name file AUTOBOOT.bin on disk
; Assembler:    ca65 --cpu 65c02
; Linker:       ld65 -C demo.cfg

.include "novavm.inc"

; ---------------------------------------------------------------------------
; Header -- 2-byte load address
; ---------------------------------------------------------------------------
.segment "HEADER"
    .byte $00, $40

; ---------------------------------------------------------------------------
; CODE
; ---------------------------------------------------------------------------
.segment "CODE"

main:
    rts
```

**Step 4: Build and verify**

Run:
```bash
cd ehbasic && make demo
```
Expected: `demo.bin` created, 3 bytes (2-byte header + 1 byte RTS).

**Step 5: Commit**

```bash
git add ehbasic/demo.cfg ehbasic/demo.s ehbasic/Makefile
git commit -m "feat: demo disk player build infrastructure"
```

---

### Task 2: Zero Page, BSS, and Constants

**Files:**
- Modify: `ehbasic/demo.s`

**Step 1: Add layout constants and zero page variables**

Replace the contents of `ehbasic/demo.s` with the full skeleton including all constants, zero page, and BSS declarations. The program does nothing yet but defines the complete data layout.

```asm
; demo.s -- NovaVM Demo Disk autoboot program
; Load address: $4000   Autoboot: name file AUTOBOOT.bin on disk
; Assembler:    ca65 --cpu 65c02
; Linker:       ld65 -C demo.cfg

.include "novavm.inc"

; ---------------------------------------------------------------------------
; Layout constants
; ---------------------------------------------------------------------------
NUM_CATEGORIES  = 5
MAX_FILES       = 40            ; max files per category
ENTRY_SIZE      = 33            ; 32-byte filename + 1-byte type
VISIBLE_ROWS    = 11            ; file list rows visible (rows 10-20)
TAB_ROW         = 8             ; text row for tab bar
LIST_TOP_ROW    = 10            ; first file list text row
LIST_BOT_ROW    = 20            ; last file list text row
NAV_ROW         = 22            ; navigation hints row
SCROLL_ROW      = 24            ; scroll text row
HDR_HEIGHT      = 60            ; graphics header height in pixels
COPPER_FRAMES   = 32            ; number of copper animation frames

; File types from FioDirType
FTYPE_SID       = $01
FTYPE_MID       = $03

; PETSCII box-drawing characters (font slot 1)
CH_HLINE        = $C0           ; horizontal line (PETSCII $C0)
CH_VLINE        = $DD           ; vertical line (PETSCII $DD)
CH_TL_ROUND     = $B0           ; top-left rounded corner
CH_TR_ROUND     = $AE           ; top-right rounded corner
CH_BL_ROUND     = $AD           ; bottom-left rounded corner
CH_BR_ROUND     = $BD           ; bottom-right rounded corner
CH_T_DOWN       = $B2           ; T-junction down
CH_T_UP         = $B1           ; T-junction up
CH_BLOCK        = $A0           ; solid block
CH_LHALF        = $A1           ; left half block
CH_RHALF        = $A2           ; right half block
CH_ARROW_L      = $9F           ; left arrow
CH_ARROW_R      = $9E           ; right arrow
CH_ARROW_U      = $9C           ; up arrow
CH_ARROW_D      = $9D           ; down arrow

; Color indices
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
; Zero page ($40-$7F)
; ---------------------------------------------------------------------------
.segment "ZEROPAGE"

zp_category:    .res 1          ; current category index (0-4)
zp_sel_index:   .res 1          ; selected file index within category
zp_scroll_top:  .res 1          ; top visible index in file list
zp_file_count:  .res 1          ; number of files in current category
zp_copper_frame:.res 1          ; current copper animation frame (0-31)
zp_scroll_lo:   .res 1          ; scroll text character index (low)
zp_scroll_hi:   .res 1          ; scroll text character index (high)
zp_last_frame:  .res 1          ; last vsync frame counter
zp_kbd_loaded:  .res 1          ; nonzero once keyboard.bin is loaded
zp_tmp:         .res 1          ; scratch
zp_tmp2:        .res 1          ; scratch
zp_ptr:         .res 2          ; general 16-bit pointer
zp_src:         .res 2          ; source pointer
zp_dst:         .res 2          ; dest pointer
zp_count:       .res 1          ; loop counter
zp_file_type:   .res 1          ; type of currently selected file

; ---------------------------------------------------------------------------
; BSS — directory buffer and saved state
; ---------------------------------------------------------------------------
.segment "BSS"

; Per-category file counts
cat_counts:     .res NUM_CATEGORIES

; Per-category buffer start pointers (lo/hi)
cat_ptrs_lo:    .res NUM_CATEGORIES
cat_ptrs_hi:    .res NUM_CATEGORIES

; Directory entry buffer: 5 categories * 40 entries * 33 bytes = 6600 bytes
dir_buffer:     .res NUM_CATEGORIES * MAX_FILES * ENTRY_SIZE

; Saved VGC state
save_mode:      .res 1
save_bgcol:     .res 1
save_fgcol:     .res 1
save_font:      .res 1
save_cursor:    .res 1

; ---------------------------------------------------------------------------
; Header -- 2-byte load address
; ---------------------------------------------------------------------------
.segment "HEADER"
    .byte $00, $40

; ---------------------------------------------------------------------------
; CODE
; ---------------------------------------------------------------------------
.segment "CODE"

main:
    rts

; ---------------------------------------------------------------------------
; RODATA
; ---------------------------------------------------------------------------
.segment "RODATA"

; Category display names (null-terminated)
cat_name_0: .byte "CLASSICAL", 0
cat_name_1: .byte "MOVIES", 0
cat_name_2: .byte "TV", 0
cat_name_3: .byte "GAMES", 0
cat_name_4: .byte "SID", 0

cat_names_lo: .byte <cat_name_0, <cat_name_1, <cat_name_2, <cat_name_3, <cat_name_4
cat_names_hi: .byte >cat_name_0, >cat_name_1, >cat_name_2, >cat_name_3, >cat_name_4

; Category directory paths (null-terminated)
cat_dir_0: .byte "classical", 0
cat_dir_1: .byte "movies", 0
cat_dir_2: .byte "tv", 0
cat_dir_3: .byte "games", 0
cat_dir_4: .byte "sid", 0

cat_dirs_lo: .byte <cat_dir_0, <cat_dir_1, <cat_dir_2, <cat_dir_3, <cat_dir_4
cat_dirs_hi: .byte >cat_dir_0, >cat_dir_1, >cat_dir_2, >cat_dir_3, >cat_dir_4

; Tab widths in characters (name length + 2 for padding)
cat_tab_widths: .byte 11, 8, 4, 7, 5

; Category highlight colors
cat_colors: .byte COL_CYAN, COL_LRED, COL_YELLOW, COL_LGREEN, COL_LBLUE

; Scroll text (null-terminated, loops)
scroll_text:
    .byte "     WELCOME TO THE NOVAVM DEMO DISK"
    .byte " ... FEATURING 83 TRACKS ACROSS 5 CATEGORIES"
    .byte " ... SID CHIP EMULATION POWERED BY MOS 6581"
    .byte " ... 14-VOICE MUSIC ENGINE WITH WAVETABLE SYNTHESIS"
    .byte " ... ACTIVE KEYBOARD VISUALIZER WITH PROGRESS BAR"
    .byte " ... USE LEFT/RIGHT TO BROWSE CATEGORIES"
    .byte " ... UP/DOWN TO SELECT A TRACK"
    .byte " ... PRESS RETURN TO PLAY"
    .byte " ...     ", 0
scroll_text_end:
SCROLL_LEN = scroll_text_end - scroll_text - 1

; Navigation help text
nav_text: .byte " ", CH_ARROW_L, CH_ARROW_R, " CATEGORY   ", CH_ARROW_U, CH_ARROW_D, " SELECT   RETURN PLAY", 0

; Keyboard binary filename
kbd_filename: .byte "KEYBOARD"
KBD_NAME_LEN = 8

; Root path for CD
root_path: .byte "/", 0

; Raster bar color palette (16 entries, sine-wave intensity)
raster_palette:
    .byte COL_BLACK, COL_DGRAY, COL_BLUE, COL_LBLUE
    .byte COL_CYAN, COL_WHITE, COL_CYAN, COL_LBLUE
    .byte COL_BLUE, COL_DGRAY, COL_BLACK, COL_DGRAY
    .byte COL_PURPLE, COL_LRED, COL_RED, COL_BROWN
RASTER_PAL_LEN = 16
```

**Step 2: Build and verify**

Run:
```bash
cd ehbasic && make demo
```
Expected: Assembles and links without errors. Binary should be ~700+ bytes (header + rodata + BSS reservations).

**Step 3: Commit**

```bash
git add ehbasic/demo.s
git commit -m "feat: demo player data layout, constants, and string tables"
```

---

### Task 3: Utility Subroutines

**Files:**
- Modify: `ehbasic/demo.s`

**Step 1: Add core utility subroutines**

Add these subroutines to the CODE segment (after `main: rts`, before RODATA):

```asm
; =====================================================================
; Wait for next vsync frame
; =====================================================================
wait_vsync:
    lda RegStatus
    cmp zp_last_frame
    beq wait_vsync
    sta zp_last_frame
    rts

; =====================================================================
; Clear the full text layer (spaces + white on black)
; =====================================================================
clear_text:
    ldy #0
@ct_row:
    sty RegCursorY
    ldx #0
@ct_col:
    stx RegCursorX
    lda #COL_WHITE
    sta RegFgCol
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
; Print null-terminated string at (zp_ptr)
; Prints at current cursor position, advances cursor
; =====================================================================
print_str:
    ldy #0
@ps_loop:
    lda (zp_ptr),y
    beq @ps_done
    sta RegCharOut
    iny
    bne @ps_loop
@ps_done:
    rts

; =====================================================================
; Print null-terminated string at (zp_ptr) with color in A
; =====================================================================
print_str_color:
    sta RegFgCol
    jsr print_str
    rts

; =====================================================================
; Copy filename from (zp_src) to FioName, set FioNameLen
; String is null-terminated, max 63 bytes
; =====================================================================
copy_filename:
    ldy #0
@cf_loop:
    lda (zp_src),y
    beq @cf_done
    sta FioName,y
    iny
    cpy #63
    bcc @cf_loop
@cf_done:
    sty FioNameLen
    rts

; =====================================================================
; CD to directory path at (zp_src)
; =====================================================================
do_cd:
    jsr copy_filename
    lda #FioCmdCd
    sta FioCmd
    rts

; =====================================================================
; CD to root directory
; =====================================================================
cd_root:
    lda #<root_path
    sta zp_src
    lda #>root_path
    sta zp_src+1
    jmp do_cd

; =====================================================================
; Set color for a range of columns on current cursor row
; Input: X=start col, Y=end col (exclusive), A=color
; Uses CharRam math: addr = CharRamBase + row*80 + col (but we use
; cursor positioning + RegFgCol + RegCharOut re-read approach)
; Actually: just set color RAM directly for speed
; =====================================================================
; (We'll use cursor-based approach for simplicity in the text routines)

; =====================================================================
; Fill a text row with a character
; Input: Y=row, A=character, X=color
; =====================================================================
fill_row:
    sty RegCursorY
    stx RegFgCol
    sta zp_tmp
    ldx #0
@fr_loop:
    stx RegCursorX
    lda zp_tmp
    sta RegCharOut
    inx
    cpx #80
    bcc @fr_loop
    rts
```

**Step 2: Build and verify**

Run:
```bash
cd ehbasic && make demo
```
Expected: Assembles and links clean.

**Step 3: Commit**

```bash
git add ehbasic/demo.s
git commit -m "feat: demo player utility subroutines"
```

---

### Task 4: Directory Loading

**Files:**
- Modify: `ehbasic/demo.s`

**Step 1: Add directory loading subroutines**

Add to CODE segment:

```asm
; =====================================================================
; Load all category directories into the dir_buffer
; Populates cat_counts[] and cat_ptrs_lo/hi[]
; =====================================================================
load_all_dirs:
    ; Initialize buffer pointer
    lda #<dir_buffer
    sta zp_dst
    lda #>dir_buffer
    sta zp_dst+1

    ldx #0
@lad_cat:
    cpx #NUM_CATEGORIES
    bcs @lad_done
    stx zp_count               ; save category index

    ; Store buffer pointer for this category
    lda zp_dst
    sta cat_ptrs_lo,x
    lda zp_dst+1
    sta cat_ptrs_hi,x

    ; CD to category directory
    txa
    tax
    lda cat_dirs_lo,x
    sta zp_src
    lda cat_dirs_hi,x
    sta zp_src+1
    jsr do_cd

    ; Open directory (no filter — enumerate all files)
    stz FioNameLen
    lda #FioCmdDirOpen
    sta FioCmd

    ; Read entries
    lda #0
    sta zp_tmp2                 ; file count for this category

@lad_read:
    lda #FioCmdDirRead
    sta FioCmd
    lda FioErrCode
    cmp #FioErrEndOfDir
    beq @lad_cat_done

    lda FioStatus
    cmp #FioStatusOk
    bne @lad_cat_done

    ; Skip directory entries (type 5)
    lda FioDirType
    cmp #FioDirTypeDir
    beq @lad_read

    ; Check max files
    lda zp_tmp2
    cmp #MAX_FILES
    bcs @lad_cat_done

    ; Copy filename (32 bytes) from FioName to buffer
    ldy #0
@lad_copy_name:
    lda FioName,y
    sta (zp_dst),y
    iny
    cpy #32
    bcc @lad_copy_name

    ; Store file type at offset 32
    lda FioDirType
    sta (zp_dst),y

    ; Advance buffer pointer by ENTRY_SIZE (33)
    clc
    lda zp_dst
    adc #ENTRY_SIZE
    sta zp_dst
    lda zp_dst+1
    adc #0
    sta zp_dst+1

    inc zp_tmp2
    bra @lad_read

@lad_cat_done:
    ; Store file count
    ldx zp_count
    lda zp_tmp2
    sta cat_counts,x

    ; CD back to root
    jsr cd_root

    ; Next category
    ldx zp_count
    inx
    bra @lad_cat

@lad_done:
    rts
```

**Step 2: Build and verify**

Run:
```bash
cd ehbasic && make demo
```
Expected: Assembles and links clean.

**Step 3: Commit**

```bash
git add ehbasic/demo.s
git commit -m "feat: demo player directory loading"
```

---

### Task 5: Graphics Header Drawing

**Files:**
- Modify: `ehbasic/demo.s`

**Step 1: Add graphics header drawing routine**

This draws the static graphics elements: title text as large filled rectangles and rainbow color bars.

```asm
; =====================================================================
; Draw the graphics header (top 60 pixels)
; Uses VGC graphics commands on the graphics layer
; =====================================================================
draw_header:
    ; Clear graphics layer
    lda #CmdGcls
    sta RegCmd

    ; --- Rainbow color bars above title (y=4 to y=18) ---
    ldx #0                      ; bar index
@dh_top_bars:
    cpx #7
    bcs @dh_title

    ; Set color
    lda rainbow_colors,x
    sta RegP0
    lda #CmdGcolor
    sta RegCmd

    ; Fill bar: x1=20, y1=4+x*2, x2=299, y2=4+x*2+1
    phx
    lda #20
    sta RegP0
    stz RegP1
    txa
    asl
    clc
    adc #4
    sta RegP2                   ; y1
    stz RegP3
    lda #<299
    sta RegP4
    lda #>299
    sta RegP5
    lda RegP2
    inc
    sta RegP6                   ; y2 = y1+1
    stz RegP7
    lda #CmdFill
    sta RegCmd
    plx
    inx
    bra @dh_top_bars

@dh_title:
    ; --- Title "NOVAVM DEMO DISK" as large text ---
    ; We'll use the text layer for the title (row 3-4), rendered in
    ; bright cyan with the PETSCII font. The graphics bars above and
    ; below frame it nicely.
    ;
    ; Title centered: 16 chars, starting at col 32
    lda #3
    sta RegCursorY
    lda #27
    sta RegCursorX
    lda #COL_CYAN
    sta RegFgCol
    lda #<title_text
    sta zp_ptr
    lda #>title_text
    sta zp_ptr+1
    jsr print_str

    ; Subtitle on row 5
    lda #5
    sta RegCursorY
    lda #23
    sta RegCursorX
    lda #COL_LGRAY
    sta RegFgCol
    lda #<subtitle_text
    sta zp_ptr
    lda #>subtitle_text
    sta zp_ptr+1
    jsr print_str

    ; --- Rainbow color bars below title (y=46 to y=60) ---
    ldx #0
@dh_bot_bars:
    cpx #7
    bcs @dh_done

    lda rainbow_colors,x
    sta RegP0
    lda #CmdGcolor
    sta RegCmd

    phx
    lda #20
    sta RegP0
    stz RegP1
    txa
    asl
    clc
    adc #46
    sta RegP2
    stz RegP3
    lda #<299
    sta RegP4
    lda #>299
    sta RegP5
    lda RegP2
    inc
    sta RegP6
    stz RegP7
    lda #CmdFill
    sta RegCmd
    plx
    inx
    bra @dh_bot_bars

@dh_done:
    rts
```

Add to RODATA:

```asm
; Title strings
title_text:    .byte "NOVAVM  DEMO  DISK", 0
subtitle_text: .byte "65C02 MUSIC PLAYER & VISUALIZER", 0

; Rainbow bar colors (7 bars)
rainbow_colors: .byte COL_RED, COL_ORANGE, COL_YELLOW, COL_GREEN, COL_CYAN, COL_BLUE, COL_PURPLE
```

**Step 2: Build and verify**

Run:
```bash
cd ehbasic && make demo
```
Expected: Clean build.

**Step 3: Commit**

```bash
git add ehbasic/demo.s
git commit -m "feat: demo player graphics header with rainbow bars and title"
```

---

### Task 6: Copper Raster Bar Animation

**Files:**
- Modify: `ehbasic/demo.s`

**Step 1: Add copper list builder and animation**

```asm
; =====================================================================
; Build copper animation lists (32 frames)
; Each list changes RegBgCol across scanlines 0-59 to create
; flowing raster bar effect, then resets to black at scanline 60.
; =====================================================================
build_copper:
    ldx #0                      ; frame index
@bc_frame:
    cpx #COPPER_FRAMES
    bcs @bc_enable
    phx

    ; Select target copper list
    stx RegP0
    lda #CmdCopperList
    sta RegCmd

    ; Clear it
    lda #CmdCopperClear
    sta RegCmd

    ; Add entries for scanlines 0-59, every 4 scanlines
    plx
    phx
    stx zp_tmp2                 ; frame offset for palette rotation

    ldy #0                      ; scanline
@bc_scanline:
    cpy #HDR_HEIGHT
    bcs @bc_reset_bg

    ; Color = raster_palette[(scanline/4 + frame) % RASTER_PAL_LEN]
    tya
    lsr
    lsr                         ; scanline / 4
    clc
    adc zp_tmp2                 ; + frame offset
@bc_mod:
    cmp #RASTER_PAL_LEN
    bcc @bc_got_idx
    sec
    sbc #RASTER_PAL_LEN
    bra @bc_mod
@bc_got_idx:
    tax
    lda raster_palette,x

    ; CopperAdd: P0/P1=x(0), P2=y, P3/P4=register(1=RegBgCol), P5=value
    pha
    stz RegP0                   ; x lo = 0
    stz RegP1                   ; x hi = 0
    sty RegP2                   ; y = scanline
    lda #1                      ; register index 1 = RegBgCol
    sta RegP3
    stz RegP4
    pla
    sta RegP5                   ; color value
    lda #CmdCopperAdd
    sta RegCmd

    iny
    iny
    iny
    iny                         ; step by 4 scanlines
    bra @bc_scanline

@bc_reset_bg:
    ; Reset background to black at scanline HDR_HEIGHT
    stz RegP0
    stz RegP1
    lda #HDR_HEIGHT
    sta RegP2
    lda #1
    sta RegP3
    stz RegP4
    lda #COL_BLACK
    sta RegP5
    lda #CmdCopperAdd
    sta RegCmd

    plx
    inx
    bra @bc_frame

@bc_enable:
    ; Start with list 0
    stz RegP0
    lda #CmdCopperUse
    sta RegCmd

    ; Enable copper
    lda #CmdCopperEnable
    sta RegCmd

    stz zp_copper_frame
    rts

; =====================================================================
; Advance copper animation to next frame
; Called once per vsync from the main loop
; =====================================================================
advance_copper:
    inc zp_copper_frame
    lda zp_copper_frame
    cmp #COPPER_FRAMES
    bcc @ac_set
    stz zp_copper_frame
    lda #0
@ac_set:
    sta RegP0
    lda #CmdCopperUse
    sta RegCmd
    rts
```

**Step 2: Build and verify**

Run:
```bash
cd ehbasic && make demo
```
Expected: Clean build.

**Step 3: Commit**

```bash
git add ehbasic/demo.s
git commit -m "feat: demo player copper raster bar animation (32 frames)"
```

---

### Task 7: PETSCII Tab Bar

**Files:**
- Modify: `ehbasic/demo.s`

**Step 1: Add tab bar drawing routine**

```asm
; =====================================================================
; Draw the category tab bar at TAB_ROW
; Input: zp_category = active tab (0-4)
; =====================================================================
draw_tabs:
    ; Row TAB_ROW: draw tab tops
    ; Row TAB_ROW+1: draw tab body / connector line

    ; First, clear tab rows
    lda #TAB_ROW
    sta RegCursorY
    ldx #0
@dt_clr1:
    stx RegCursorX
    lda #' '
    sta RegCharOut
    inx
    cpx #80
    bcc @dt_clr1

    lda #TAB_ROW + 1
    sta RegCursorY
    ldx #0
@dt_clr2:
    stx RegCursorX
    lda #CH_HLINE
    lda #COL_DGRAY
    sta RegFgCol
    lda #CH_HLINE
    sta RegCharOut
    inx
    cpx #80
    bcc @dt_clr2

    ; Now draw each tab
    lda #2                      ; starting column (left margin)
    sta zp_tmp                  ; current column position

    ldx #0                      ; tab index
@dt_tab:
    cpx #NUM_CATEGORIES
    bcs @dt_done
    stx zp_count

    ; Is this the active tab?
    cpx zp_category
    beq @dt_active

    ; --- Inactive tab: just the name in dim color ---
    lda #TAB_ROW
    sta RegCursorY
    lda zp_tmp
    inc                         ; +1 for left padding
    sta RegCursorX
    lda #COL_DGRAY
    sta RegFgCol

    ldx zp_count
    lda cat_names_lo,x
    sta zp_ptr
    lda cat_names_hi,x
    sta zp_ptr+1
    jsr print_str

    bra @dt_next

@dt_active:
    ; --- Active tab: box-drawing border, bright name ---
    ; Top-left corner
    lda #TAB_ROW
    sta RegCursorY
    ldx zp_count
    lda cat_colors,x
    sta RegFgCol
    lda zp_tmp
    sta RegCursorX
    lda #CH_TL_ROUND
    sta RegCharOut

    ; Top horizontal line
    ldx zp_count
    lda cat_tab_widths,x
    sta zp_tmp2                 ; tab width
    ldy #0
@dt_top_line:
    cpy zp_tmp2
    bcs @dt_top_corner
    lda #CH_HLINE
    sta RegCharOut
    iny
    bra @dt_top_line

@dt_top_corner:
    ; Top-right corner
    lda #CH_TR_ROUND
    sta RegCharOut

    ; Tab name on same row, inside the borders
    lda #TAB_ROW
    sta RegCursorY
    lda zp_tmp
    inc                         ; skip left border
    sta RegCursorX
    lda #COL_WHITE
    sta RegFgCol

    ldx zp_count
    lda cat_names_lo,x
    sta zp_ptr
    lda cat_names_hi,x
    sta zp_ptr+1
    jsr print_str

    ; Bottom row: vertical bars on edges, spaces inside (open bottom)
    lda #TAB_ROW + 1
    sta RegCursorY

    ldx zp_count
    lda cat_colors,x
    sta RegFgCol

    ; Left vertical
    lda zp_tmp
    sta RegCursorX
    lda #CH_VLINE
    sta RegCharOut

    ; Spaces inside (overwrite the hline from the base row)
    lda #COL_BLACK
    sta RegFgCol
    ldy #0
@dt_inner:
    cpy zp_tmp2
    bcs @dt_right_vert
    lda #' '
    sta RegCharOut
    iny
    bra @dt_inner

@dt_right_vert:
    ldx zp_count
    lda cat_colors,x
    sta RegFgCol
    lda #CH_VLINE
    sta RegCharOut

@dt_next:
    ; Advance column: tab width + 3 (borders + gap)
    ldx zp_count
    lda cat_tab_widths,x
    clc
    adc zp_tmp
    adc #3                      ; left border + right border + gap
    sta zp_tmp

    ldx zp_count
    inx
    bra @dt_tab

@dt_done:
    rts
```

**Step 2: Build and verify**

Run:
```bash
cd ehbasic && make demo
```
Expected: Clean build.

**Step 3: Commit**

```bash
git add ehbasic/demo.s
git commit -m "feat: demo player PETSCII tab bar with active tab highlight"
```

---

### Task 8: File List Display

**Files:**
- Modify: `ehbasic/demo.s`

**Step 1: Add file list drawing routine**

```asm
; =====================================================================
; Draw the file list for the current category
; Uses: zp_category, zp_scroll_top, zp_sel_index
; =====================================================================
draw_file_list:
    ; Get current category's file count and buffer pointer
    ldx zp_category
    lda cat_counts,x
    sta zp_file_count
    lda cat_ptrs_lo,x
    sta zp_src
    lda cat_ptrs_hi,x
    sta zp_src+1

    ; Advance source pointer to scroll_top entry
    lda zp_scroll_top
    beq @dfl_no_offset
    sta zp_count
    ; Add scroll_top * ENTRY_SIZE to zp_src
@dfl_offset:
    clc
    lda zp_src
    adc #ENTRY_SIZE
    sta zp_src
    lda zp_src+1
    adc #0
    sta zp_src+1
    dec zp_count
    bne @dfl_offset
@dfl_no_offset:

    ; Draw VISIBLE_ROWS entries (or fewer if at end of list)
    ldy #0                      ; visible row index
@dfl_row:
    cpy #VISIBLE_ROWS
    bcs @dfl_clear_rest

    ; Calculate file index
    tya
    clc
    adc zp_scroll_top
    cmp zp_file_count
    bcs @dfl_clear_rest         ; past end of file list

    phy
    sta zp_tmp                  ; absolute file index

    ; Set cursor to this row
    tya
    clc
    adc #LIST_TOP_ROW
    sta RegCursorY
    lda #2
    sta RegCursorX

    ; Is this the selected entry?
    lda zp_tmp
    cmp zp_sel_index
    bne @dfl_normal

    ; --- Highlighted entry ---
    ldx zp_category
    lda cat_colors,x
    sta RegFgCol
    bra @dfl_print_name

@dfl_normal:
    lda #COL_WHITE
    sta RegFgCol

@dfl_print_name:
    ; Print filename from (zp_src), max 40 chars
    phy
    ldy #0
@dfl_char:
    cpy #40
    bcs @dfl_name_done
    lda (zp_src),y
    beq @dfl_name_done
    sta RegCharOut
    iny
    bra @dfl_char
@dfl_name_done:
    ; Pad with spaces to column 44
@dfl_pad:
    cpy #42
    bcs @dfl_type
    lda #' '
    sta RegCharOut
    iny
    bra @dfl_pad
@dfl_type:
    ply

    ; Print file type badge
    ; Type byte is at offset 32 in the entry
    ldy #32
    lda (zp_src),y
    cmp #FTYPE_SID
    beq @dfl_sid
    cmp #FTYPE_MID
    beq @dfl_mid
    bra @dfl_type_done

@dfl_sid:
    lda #COL_LGREEN
    sta RegFgCol
    lda #'S'
    sta RegCharOut
    lda #'I'
    sta RegCharOut
    lda #'D'
    sta RegCharOut
    bra @dfl_type_done

@dfl_mid:
    lda #COL_LBLUE
    sta RegFgCol
    lda #'M'
    sta RegCharOut
    lda #'I'
    sta RegCharOut
    lda #'D'
    sta RegCharOut

@dfl_type_done:
    ; Pad rest of line with spaces
    lda #' '
    lda #COL_BLACK
    sta RegFgCol
    lda #' '
@dfl_line_pad:
    lda RegCursorX
    cmp #78
    bcs @dfl_advance
    lda #' '
    sta RegCharOut
    bra @dfl_line_pad

@dfl_advance:
    ; Advance source pointer to next entry
    clc
    lda zp_src
    adc #ENTRY_SIZE
    sta zp_src
    lda zp_src+1
    adc #0
    sta zp_src+1

    ply
    iny
    jmp @dfl_row

@dfl_clear_rest:
    ; Clear remaining rows
    cpy #VISIBLE_ROWS
    bcs @dfl_done
    phy
    tya
    clc
    adc #LIST_TOP_ROW
    tay
    lda #' '
    ldx #COL_BLACK
    jsr fill_row
    ply
    iny
    bra @dfl_clear_rest

@dfl_done:
    rts
```

**Step 2: Build and verify**

Run:
```bash
cd ehbasic && make demo
```
Expected: Clean build.

**Step 3: Commit**

```bash
git add ehbasic/demo.s
git commit -m "feat: demo player scrollable file list with type badges"
```

---

### Task 9: Scroll Text Engine

**Files:**
- Modify: `ehbasic/demo.s`

**Step 1: Add scroll text routines**

```asm
; =====================================================================
; Initialize scroll text
; =====================================================================
init_scroll:
    stz zp_scroll_lo
    stz zp_scroll_hi
    ; Set color for scroll row
    lda #SCROLL_ROW
    sta RegCursorY
    ldx #0
@is_col:
    stx RegCursorX
    lda #COL_LGREEN
    sta RegFgCol
    lda #' '
    sta RegCharOut
    inx
    cpx #80
    bcc @is_col
    rts

; =====================================================================
; Advance scroll text by one character (called once per frame)
; Shifts row 24 left by one char, appends next character at col 79
; =====================================================================
advance_scroll:
    ; Shift columns 1-79 left to 0-78
    lda #SCROLL_ROW
    sta RegCursorY

    ; Read char at each column and write it one left
    ; We use screen RAM direct access for speed
    ; Row 24: CharRam offset = 24 * 80 = 1920 = $0780
    ; Address = CharRamBase + $0780 = $AA00 + $0780 = $B180

    ; Copy bytes: B181..B1CF -> B180..B1CE (79 bytes)
    ldx #0
@as_shift:
    lda CharRamBase + 1920 + 1,x
    sta CharRamBase + 1920,x
    inx
    cpx #79
    bcc @as_shift

    ; Get next scroll character
    lda zp_scroll_lo
    sta zp_ptr
    lda zp_scroll_hi
    sta zp_ptr+1
    clc
    lda zp_ptr
    adc #<scroll_text
    sta zp_ptr
    lda zp_ptr+1
    adc #>scroll_text
    sta zp_ptr+1

    lda (zp_ptr)
    bne @as_got_char
    ; Hit null terminator — reset to start
    stz zp_scroll_lo
    stz zp_scroll_hi
    lda scroll_text             ; first char
@as_got_char:
    ; Write to column 79 of row 24
    sta CharRamBase + 1920 + 79

    ; Advance scroll index
    inc zp_scroll_lo
    bne @as_no_hi
    inc zp_scroll_hi
@as_no_hi:
    ; Check for wrap
    lda zp_scroll_lo
    cmp #<SCROLL_LEN
    bne @as_done
    lda zp_scroll_hi
    cmp #>SCROLL_LEN
    bne @as_done
    stz zp_scroll_lo
    stz zp_scroll_hi
@as_done:
    rts
```

**Step 2: Build and verify**

Run:
```bash
cd ehbasic && make demo
```
Expected: Clean build.

**Step 3: Commit**

```bash
git add ehbasic/demo.s
git commit -m "feat: demo player scroll text engine"
```

---

### Task 10: Navigation Help Bar

**Files:**
- Modify: `ehbasic/demo.s`

**Step 1: Add navigation bar drawing**

```asm
; =====================================================================
; Draw navigation hints at NAV_ROW
; =====================================================================
draw_nav:
    lda #NAV_ROW
    sta RegCursorY
    lda #18
    sta RegCursorX
    lda #COL_LGRAY
    sta RegFgCol
    lda #<nav_text
    sta zp_ptr
    lda #>nav_text
    sta zp_ptr+1
    jsr print_str
    rts
```

**Step 2: Build and verify**

Run:
```bash
cd ehbasic && make demo
```
Expected: Clean build.

**Step 3: Commit**

```bash
git add ehbasic/demo.s
git commit -m "feat: demo player navigation help bar"
```

---

### Task 11: Keyboard Visualizer Loading

**Files:**
- Modify: `ehbasic/demo.s`

**Step 1: Add keyboard.bin loader**

```asm
; =====================================================================
; Load keyboard.bin from disk (if not already loaded)
; Sets zp_kbd_loaded on success
; =====================================================================
load_keyboard:
    lda zp_kbd_loaded
    bne @lk_done                ; already loaded

    ; CD to root first
    jsr cd_root

    ; Copy "KEYBOARD" to FioName
    ldx #0
@lk_copy:
    lda kbd_filename,x
    sta FioName,x
    inx
    cpx #KBD_NAME_LEN
    bcc @lk_copy
    lda #KBD_NAME_LEN
    sta FioNameLen

    ; Set load address (keyboard.bin has 2-byte header, FileIO handles it)
    lda #$00
    sta FioSrcL
    lda #$90
    sta FioSrcH

    ; Issue LOAD
    lda #FioCmdLoad
    sta FioCmd

    ; Check result
    lda FioStatus
    cmp #FioStatusOk
    bne @lk_done

    lda #$FF
    sta zp_kbd_loaded

@lk_done:
    rts
```

**Step 2: Build and verify**

Run:
```bash
cd ehbasic && make demo
```
Expected: Clean build.

**Step 3: Commit**

```bash
git add ehbasic/demo.s
git commit -m "feat: demo player keyboard.bin loader"
```

---

### Task 12: Playback Routine

**Files:**
- Modify: `ehbasic/demo.s`

**Step 1: Add playback start/stop routines**

```asm
; =====================================================================
; Start playback of the currently selected file
; Builds the full path "category/filename" and issues SidPlay or MidPlay
; =====================================================================
start_playback:
    ; Build path: "dirname/filename"
    ; First, copy directory name
    ldx zp_category
    lda cat_dirs_lo,x
    sta zp_src
    lda cat_dirs_hi,x
    sta zp_src+1

    ldy #0
@sp_dir:
    lda (zp_src),y
    beq @sp_slash
    sta FioName,y
    iny
    bra @sp_dir
@sp_slash:
    lda #'/'
    sta FioName,y
    iny

    ; Now copy the filename from the dir buffer entry
    ; Get pointer to selected entry
    ldx zp_category
    lda cat_ptrs_lo,x
    sta zp_src
    lda cat_ptrs_hi,x
    sta zp_src+1

    ; Advance to selected entry: sel_index * ENTRY_SIZE
    ldx zp_sel_index
    beq @sp_copy_name
@sp_seek:
    clc
    lda zp_src
    adc #ENTRY_SIZE
    sta zp_src
    lda zp_src+1
    adc #0
    sta zp_src+1
    dex
    bne @sp_seek

@sp_copy_name:
    ; Copy filename bytes (null-terminated, max 32)
    ldx #0
@sp_name:
    lda (zp_src,x)              ; can't index both src and FioName easily
    ; Use different approach: index into (zp_src)
    phx
    txa
    tax                         ; X = source index
    ; Actually, we need to index into (zp_src) with X offset
    ; and write to FioName+Y. Let's use a simpler approach.
    plx

    ; Simpler: use zp_ptr as second index
    phx
    stx zp_tmp
    ldx zp_tmp
    lda (zp_src),x
    plx
    beq @sp_name_done
    sta FioName,y
    iny
    inx
    cpx #32
    bcc @sp_name
@sp_name_done:
    sty FioNameLen

    ; Determine file type
    ; Type byte is at offset 32 of the entry
    phy
    ldy #32
    lda (zp_src),y
    sta zp_file_type
    ply

    ; Issue appropriate play command
    lda zp_file_type
    cmp #FTYPE_SID
    beq @sp_sid
    cmp #FTYPE_MID
    beq @sp_mid
    rts                         ; unknown type, do nothing

@sp_sid:
    lda #FioCmdSidPlay
    sta FioCmd
    bra @sp_wait

@sp_mid:
    lda #FioCmdMidPlay
    sta FioCmd

@sp_wait:
    ; Wait a few frames for playback to start
    ldx #10
@sp_delay:
    jsr wait_vsync
    dex
    bne @sp_delay
    rts

; =====================================================================
; Stop any active playback
; =====================================================================
stop_playback:
    lda zp_file_type
    cmp #FTYPE_SID
    beq @stop_sid
    lda #FioCmdMidStop
    sta FioCmd
    rts
@stop_sid:
    lda #FioCmdSidStop
    sta FioCmd
    rts
```

**Step 2: Build and verify**

Run:
```bash
cd ehbasic && make demo
```
Expected: Clean build.

**Step 3: Commit**

```bash
git add ehbasic/demo.s
git commit -m "feat: demo player SID/MIDI playback start/stop"
```

---

### Task 13: Main Entry Point and Browser Loop

**Files:**
- Modify: `ehbasic/demo.s`

**Step 1: Replace the `main:` stub with the full entry point and main loop**

```asm
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

    ; --- Initialize ---
    lda #1                      ; mode 1: graphics over text
    sta RegMode
    lda #COL_BLACK
    sta RegBgCol
    lda #COL_WHITE
    sta RegFgCol
    lda #1                      ; PETSCII Upper font
    sta RegFont
    stz RegCursorEnable         ; hide cursor

    stz zp_category
    stz zp_sel_index
    stz zp_scroll_top
    stz zp_kbd_loaded
    stz zp_copper_frame

    ; Clear screen
    jsr clear_text

    ; Draw graphics header
    jsr draw_header

    ; Build and enable copper raster bars
    jsr build_copper

    ; Load all directories
    jsr load_all_dirs

    ; Load keyboard.bin for later use
    jsr load_keyboard

    ; Draw initial UI
    jsr draw_tabs
    jsr draw_file_list
    jsr draw_nav
    jsr init_scroll

    ; Drain keyboard buffer
@drain:
    jsr V_INPT
    bcs @drain

    ; Snapshot frame counter
    lda RegStatus
    sta zp_last_frame

    ; --- Main browser loop ---
@browser_loop:
    jsr wait_vsync
    jsr advance_copper
    jsr advance_scroll

    ; Check keyboard input
    jsr V_INPT
    bcc @browser_loop           ; no key

    ; --- Handle key ---
    cmp #$1D                    ; cursor right
    beq @key_right
    cmp #$9D                    ; cursor left (shifted right)
    beq @key_left
    cmp #$11                    ; cursor down
    beq @key_down
    cmp #$91                    ; cursor up (shifted down)
    beq @key_up
    cmp #$0D                    ; RETURN
    beq @key_enter
    cmp #'q'                    ; quit
    beq @exit
    cmp #'Q'
    beq @exit
    bra @browser_loop

@key_right:
    lda zp_category
    cmp #NUM_CATEGORIES - 1
    bcs @browser_loop           ; already at last tab
    inc zp_category
    stz zp_sel_index
    stz zp_scroll_top
    jsr draw_tabs
    jsr draw_file_list
    bra @browser_loop

@key_left:
    lda zp_category
    beq @browser_loop           ; already at first tab
    dec zp_category
    stz zp_sel_index
    stz zp_scroll_top
    jsr draw_tabs
    jsr draw_file_list
    bra @browser_loop

@key_down:
    lda zp_sel_index
    inc
    cmp zp_file_count
    bcs @browser_loop           ; at end of list
    sta zp_sel_index
    ; Scroll down if needed
    sec
    sbc zp_scroll_top
    cmp #VISIBLE_ROWS
    bcc @key_down_redraw
    inc zp_scroll_top
@key_down_redraw:
    jsr draw_file_list
    bra @browser_loop

@key_up:
    lda zp_sel_index
    beq @browser_loop           ; already at top
    dec zp_sel_index
    ; Scroll up if needed
    lda zp_sel_index
    cmp zp_scroll_top
    bcs @key_up_redraw
    dec zp_scroll_top
@key_up_redraw:
    jsr draw_file_list
    bra @browser_loop

@key_enter:
    ; Check we have files
    lda zp_file_count
    beq @browser_loop

    ; Disable copper during playback
    lda #CmdCopperDisable
    sta RegCmd

    ; Start playback
    jsr start_playback

    ; Launch keyboard visualizer
    lda zp_kbd_loaded
    beq @skip_viz
    jsr $9000
@skip_viz:

    ; Stop playback
    jsr stop_playback

    ; Restore browser screen
    lda #1
    sta RegMode
    lda #COL_BLACK
    sta RegBgCol
    lda #1
    sta RegFont
    stz RegCursorEnable

    jsr clear_text
    jsr draw_header
    jsr build_copper
    jsr draw_tabs
    jsr draw_file_list
    jsr draw_nav
    jsr init_scroll

    ; Drain keyboard
@drain2:
    jsr V_INPT
    bcs @drain2

    lda RegStatus
    sta zp_last_frame
    jmp @browser_loop

@exit:
    ; --- Cleanup ---
    ; Disable copper
    lda #CmdCopperDisable
    sta RegCmd

    ; Clear graphics
    lda #CmdGcls
    sta RegCmd

    ; Restore VGC state
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

    jsr clear_text
    rts
```

**Step 2: Build and verify**

Run:
```bash
cd ehbasic && make demo
```
Expected: Clean build. This is the complete program.

**Step 3: Commit**

```bash
git add ehbasic/demo.s
git commit -m "feat: demo player main loop with input handling and playback"
```

---

### Task 14: PETSCII Character Code Verification

**Files:**
- Modify: `ehbasic/demo.s` (if corrections needed)

**Step 1: Verify PETSCII character codes**

The box-drawing character codes used in the tab bar need to be verified against the actual PETSCII Upper font. Check by examining the font generation script:

Run:
```bash
python3 tools/make_petscii_font.py --dump-codes 2>/dev/null || echo "Check tools/make_petscii_font.py manually for the screen-to-ASCII mapping table"
```

Cross-reference the character codes in `demo.s` against the actual font bitmap data. The key characters:
- `CH_HLINE` ($C0): Should be PETSCII horizontal line
- `CH_VLINE` ($DD): Should be PETSCII vertical line
- `CH_TL_ROUND` ($B0): Top-left rounded corner
- `CH_TR_ROUND` ($AE): Top-right rounded corner

If any codes are wrong, update the constants in demo.s.

**Step 2: Test visually**

Build the demo and import to the demo disk:
```bash
cd ehbasic && make demo
dotnet run --project e6502.NDI -- delete docs/programs/demo.ndi /AUTOBOOT.bin 2>/dev/null; true
dotnet run --project e6502.NDI -- import docs/programs/demo.ndi ehbasic/demo.bin / --name AUTOBOOT.bin
dotnet run --project e6502.NDI -- delete docs/programs/demo.ndi /KEYBOARD.bin 2>/dev/null; true
dotnet run --project e6502.NDI -- import docs/programs/demo.ndi ehbasic/keyboard.bin / --name KEYBOARD.bin
```

Verify the file list shows both files:
```bash
dotnet run --project e6502.NDI -- dir docs/programs/demo.ndi
```

**Step 3: Commit any fixes**

```bash
git add ehbasic/demo.s
git commit -m "fix: verify and correct PETSCII character codes"
```

---

### Task 15: Key Code Verification and Input Handling

**Files:**
- Modify: `ehbasic/demo.s` (if corrections needed)

**Step 1: Verify key codes**

The key codes used in the main loop need to match what the NovaVM actually sends for cursor keys. Check the ScreenEditor / input handling code:

Look at how `V_INPT` returns key codes. The codes used are:
- `$1D` = cursor right
- `$9D` = cursor left
- `$11` = cursor down
- `$91` = cursor up
- `$0D` = RETURN

These are standard PETSCII codes. However, with font slot 1 (PETSCII Upper), the keymap may map differently. Verify by checking `FontKeymap.cs` for any remapping of cursor keys.

If the emulator uses different codes for arrow keys (e.g., ANSI escape sequences), update the key handling. Common alternative:
- Left arrow = $08 or $1C
- Right arrow = $1D or $1C
- Up arrow = $91 or $1E
- Down arrow = $11 or $1F

Check `ScreenEditor.cs` for `KeyDown` handling to see what byte values get queued for cursor keys.

**Step 2: Update if needed and rebuild**

```bash
cd ehbasic && make demo
```

**Step 3: Commit any fixes**

```bash
git add ehbasic/demo.s
git commit -m "fix: verify and correct key codes for cursor navigation"
```

---

### Task 16: Integration Test on Demo Disk

**Files:**
- Modify: `docs/programs/demo.ndi`

**Step 1: Build everything**

```bash
cd ehbasic && make clean && make all
```

Expected: `basic.bin`, `keyboard.bin`, `demo.bin`, `tokens.json` all built successfully.

**Step 2: Import files to demo disk**

```bash
# Remove old versions if they exist
dotnet run --project e6502.NDI -- delete docs/programs/demo.ndi /AUTOBOOT.bin 2>/dev/null; true
dotnet run --project e6502.NDI -- delete docs/programs/demo.ndi /KEYBOARD.bin 2>/dev/null; true

# Import fresh copies
dotnet run --project e6502.NDI -- import docs/programs/demo.ndi ehbasic/demo.bin / --name AUTOBOOT.bin
dotnet run --project e6502.NDI -- import docs/programs/demo.ndi ehbasic/keyboard.bin / --name KEYBOARD.bin
```

**Step 3: Verify disk contents**

```bash
dotnet run --project e6502.NDI -- dir docs/programs/demo.ndi
```

Expected output should show AUTOBOOT.bin and KEYBOARD.bin in the root alongside the 5 category directories.

**Step 4: Run the emulator**

```bash
dotnet run --project e6502.Avalonia
```

The demo should autoboot. Verify:
1. Rainbow color bars and title appear in the header
2. Copper raster bars animate smoothly
3. Tab bar shows 5 categories with CLASSICAL highlighted
4. File list shows classical music files
5. Left/right switches tabs
6. Up/down scrolls the file list
7. Scroll text flows across the bottom
8. RETURN plays the selected file and shows the keyboard visualizer
9. Pressing a key during playback returns to the browser
10. Q quits cleanly

**Step 5: Commit**

```bash
git add docs/programs/demo.ndi ehbasic/demo.s
git commit -m "feat: demo disk player complete with autoboot, browser, and playback"
```

---

### Task 17: Final Polish

**Files:**
- Modify: `ehbasic/demo.s` (as needed)

**Step 1: Visual refinements based on testing**

After running the demo, common adjustments might include:
- Adjusting tab column positions for better spacing
- Tuning raster bar colors or animation speed
- Fixing scroll text speed (could advance every 2nd frame if too fast)
- Adjusting file list highlight color contrast
- Fixing any rendering artifacts from copper/text layer overlap

**Step 2: Test again**

Rebuild, reimport, and run the emulator to verify fixes.

**Step 3: Final commit**

```bash
git add ehbasic/demo.s docs/programs/demo.ndi
git commit -m "polish: demo disk player visual refinements"
```

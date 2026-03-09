# Demo Disk Player — Design

## Overview

An autoboot assembly program (AUTOBOOT.bin) for the NovaVM demo disk that provides a visually striking file browser and music player. Showcases the NovaVM's hardware: copper raster bars, PETSCII graphics, 14-voice music engine, SID chip emulation, and keyboard visualizer.

## Architecture

**AUTOBOOT.bin** — 65C02 assembly, loads at $4000, up to 20KB. The ROM autoboot mechanism finds it on the demo disk and jumps to $4000 at boot.

Two screens, both using VGC mode 1 (graphics over text):

1. **Browser screen** — Graphics header with copper raster bars, PETSCII tabbed category bar, scrollable file list, smooth scroll text at bottom.
2. **Playback screen** — Delegates entirely to keyboard.bin at $9000 (loaded from disk on first use, stays resident).

### Memory Layout

```
$0040-$007F  Zero page variables (user safe range)
$4000-$8FFF  Demo program code + rodata + bss (20KB max)
$9000-$9FFF  Keyboard visualizer (loaded from disk, stays resident)
$AA00-$B1CF  Character RAM (80x25 text, used by both screens)
$B1D0-$B99F  Color RAM (80x25, used by both screens)
```

### Program Flow

```
Boot -> ROM autoboot loads AUTOBOOT.bin -> JMP $4000
  -> Set mode 1, PETSCII font (slot 1), draw graphics header
  -> Build copper raster bar lists, enable copper
  -> Load keyboard.bin into $9000 (stays resident)
  -> Load all 5 category directories into RAM buffers
  -> Enter browser loop:
      |-- Left/Right: switch category tab, redraw file list
      |-- Up/Down: scroll file list highlight
      |-- Return: start playback
      |    |-- Issue SidPlay ($08) or MidPlay ($13) via FileIO
      |    |-- Wait for MusicStatus bit 1 to confirm playback started
      |    |-- JSR $9000 (keyboard visualizer takes over)
      |    |-- On return: issue SidStop ($09) or MidStop ($14)
      |    |-- Redraw browser screen, resume loop
      |-- Copper animates raster bars + scroll text advances each frame
```

## Screen Design: Browser

### Graphics Header (top ~60 pixels)

Drawn once at startup using VGC graphics primitives:

- **Title**: "NOVAVM DEMO DISK" rendered as large blocky text using filled rectangles. Centered, ~16px tall, bright cyan.
- **Color bars**: Thin horizontal filled rectangles (2-3px each) above and below the title in rainbow palette colors (red, orange, yellow, green, cyan, blue, purple).

**Copper raster effects** (zero CPU cost, runs every frame automatically):

- Raster bars cycle the background color through a smooth gradient on scanlines within the header area. Classic demo scene sine-wave pattern: dark -> bright -> dark.
- Multiple copper lists (up to 32 animation frames) pre-built at startup. Main loop advances to the next list each frame via CopperUse, creating flowing color animation.
- CopperAdd parameters: x=0, y=scanline, register=RegBgCol ($01), value=color index.

### PETSCII Tab Bar (text row 8)

Five category tabs rendered with PETSCII Upper (font slot 1) box-drawing characters:

```
+----------+ -----------  -----------  -----------  -----------
| CLASSICAL |------------------------------------------------
|           |------------------------------------------------|
```

- **Selected tab**: Bright color (cyan text on black), open bottom connecting to list area. Uses corner chars ($81/$82) for rounded top, vertical ($82) for sides, horizontal ($80) for unselected tab borders.
- **Unselected tabs**: Dimmed (dark gray), closed bottom border.
- **Categories**: CLASSICAL, MOVIES, TV, GAMES, SID
- **Navigation hints**: Small arrow symbols at edges.

### File List (text rows 10-20)

- 11 visible entries per page.
- Each entry: filename (left-aligned) + type badge (MID or SID in contrasting color, right-aligned).
- **Highlighted entry**: Inverse video or bright color bar (e.g., cyan background).
- **Scrolling**: Up/down moves highlight. Window scrolls when highlight reaches top/bottom edge.
- **Scroll indicator**: Right edge column uses PETSCII block characters to show position within full list.

### Navigation Bar (text row 22)

```
  <-> CATEGORY   ^v SELECT   RETURN PLAY
```

Rendered in light gray PETSCII with arrow symbols.

### Scroll Text (text row 24)

Software character scroll, 1 character per frame (right to left):

- Each frame: shift all 80 chars on row 24 left by one, place next character at column 79.
- Content: looping credits string — "WELCOME TO THE NOVAVM DEMO DISK ... FEATURING 83 TRACKS ACROSS 5 CATEGORIES ... SID CHIP EMULATION BY MOS 6581 ... 14-VOICE MUSIC ENGINE WITH WAVETABLE SYNTHESIS ... USE LEFT/RIGHT TO BROWSE CATEGORIES ... UP/DOWN TO SELECT ... RETURN TO PLAY ..."
- Color: light green on black, set once in color RAM.

## Screen Design: Playback

Entirely handled by keyboard.bin at $9000 (existing, 1328 lines of tested assembly):

- 8-octave piano keyboard in mode 1
- 14 voice indicators with per-voice colors (flood fill on active keys)
- Progress bar with elapsed/total time
- Exits on keypress or song completion (polls MusicStatus bit 1)
- Saves/restores VGC state on entry/exit

## File I/O Strategy

### Directory Loading

At startup, load all 5 category directories into RAM buffers:

1. For each category (classical, movies, tv, games, sid):
   - Write category name to FioName
   - Issue FioCmdCd ($20) to enter the directory
   - Issue FioCmdDirOpen ($03) with no filter
   - Loop FioCmdDirRead ($04), storing each entry (filename + type)
   - FioCmdCd back to root ("/")
2. Store in a directory buffer area (e.g., $7000-$8EFF)
3. Each entry: 33 bytes (32 filename + 1 type). Max ~180 entries total.

### Playback Commands

- **SID files**: Write path "sid/filename.sid" to FioName, issue FioCmdSidPlay ($08)
- **MIDI files**: Write path "classical/filename.mid" to FioName, issue FioCmdMidPlay ($13)
- **Stop**: FioCmdSidStop ($09) or FioCmdMidStop ($14) on return from visualizer

### Keyboard Visualizer Loading

- On first playback: write "KEYBOARD" to FioName, issue FioCmdLoad ($02)
- keyboard.bin has 2-byte load address header ($00 $90), loads at $9000
- Stays resident — only loaded once

## Data Structures

### Zero Page ($40-$7F)

```
$40     zp_category      ; current category index (0-4)
$41     zp_sel_index     ; selected file index within category
$42     zp_scroll_top    ; top visible index in file list
$43     zp_file_count    ; number of files in current category
$44     zp_copper_frame  ; current copper animation frame
$45     zp_scroll_pos_lo ; scroll text character index (low)
$46     zp_scroll_pos_hi ; scroll text character index (high)
$47     zp_last_frame    ; last vsync frame counter
$48     zp_kbd_loaded    ; nonzero once keyboard.bin is loaded
$49     zp_tmp           ; scratch
$4A     zp_tmp2          ; scratch
$4B-$4C zp_ptr           ; general 16-bit pointer
$4D-$4E zp_src           ; source pointer for memcpy
$4F-$50 zp_dst           ; dest pointer for memcpy
```

### Directory Buffer ($7000-$8EFF)

Five category tables, contiguous:

```
$7000   cat0_count (1 byte)
$7001   cat0_entries (count * 33 bytes: 32 name + 1 type)
...next category follows immediately...
```

Category start offsets stored in a lookup table in rodata.

### Category Metadata (rodata)

```
cat_names:    5 null-terminated strings ("CLASSICAL", "MOVIES", etc.)
cat_dirs:     5 null-terminated path strings ("classical", "movies", etc.)
cat_offsets:  5 x 2-byte pointers into directory buffer
```

## Copper Animation

Pre-build 32 copper lists at startup (lists 0-31):

- Each list: ~100 CopperAdd entries covering scanlines 0-59
- Each entry sets RegBgCol to a color from a sine-wave palette
- List N offsets the sine pattern by N positions
- Main loop: each frame, CopperUse the next list index (wrapping at 32)
- Result: smoothly flowing raster bars in the header area

Below scanline 60: all lists set RegBgCol back to black so the text area renders cleanly.

## Build & Deploy

### Assembly

```bash
ca65 --cpu 65c02 demo.s -o demo.o
ld65 -C demo.cfg -o AUTOBOOT.bin demo.o
```

### Linker Config (demo.cfg)

```
MEMORY {
    ZP:     start = $0040, size = $0040, type = rw;
    HEADER: start = $0000, size = $0002, file = %O;
    CODE:   start = $4000, size = $5000, file = %O;
}
SEGMENTS {
    ZEROPAGE: load = ZP, type = zp;
    HEADER:   load = HEADER, type = ro;
    CODE:     load = CODE, type = ro;
    RODATA:   load = CODE, type = ro;
    BSS:      load = CODE, type = bss;
}
```

### Demo Disk

```bash
# Add autoboot program
dotnet run --project e6502.NDI -- import docs/programs/demo.ndi AUTOBOOT.bin /

# Add keyboard visualizer
dotnet run --project e6502.NDI -- import docs/programs/demo.ndi ehbasic/keyboard.bin /
```

## File Inventory

| File | Purpose |
|------|---------|
| `ehbasic/demo.s` | Main assembly source |
| `ehbasic/demo.cfg` | Linker configuration |
| `ehbasic/novavm.inc` | Hardware register definitions (existing) |
| `docs/programs/demo.ndi` | Demo disk image (existing, add AUTOBOOT.bin + keyboard.bin) |
| `ehbasic/Makefile` | Add demo build target |

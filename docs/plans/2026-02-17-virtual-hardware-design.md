# Virtual Hardware Design — e6502 Graphics & Sound System

## Overview

Extend the e6502 emulator with virtual graphics and sound chips, add custom BASIC
commands to EhBASIC, and build a Terminal.Gui-based TUI with a C64-style unified
screen editor. All character and graphics output flows through the Virtual Graphics
Controller (VGC). Sound chip (VSC) address space is reserved but stubbed.

## Memory Map

```
$0000-$01FF  Zero page + Stack (6502 system)
$0200-$9FFF  BASIC program space (~40KB)
$A000-$A0FF  VGC registers (Virtual Graphics Controller)
$A100-$A1FF  VSC registers (Virtual Sound Controller — stubbed)
$A200-$A3FF  Sprite shape data (16 sprites × 32 bytes)
$A400-$ABCF  Character RAM (80 × 25 = 2000 bytes)
$ABD0-$B39F  Color RAM (80 × 25 = 2000 bytes)
$B3A0-$BFFF  Free (future use / banking window)
$C000-$FFFF  EhBASIC ROM
```

Row N characters start at `$A400 + (N × 80)`.
Row N colors start at `$ABD0 + (N × 80)`.

## VGC — Virtual Graphics Controller

### Registers ($A000-$A0FF)

```
$A000  MODE       Display mode (0=text, 1=block graphics, 2=mixed)
$A001  BGCOL      Background color (0-15)
$A002  FGCOL      Current foreground color (0-15)
$A003  CURSX      Cursor X (0-79)
$A004  CURSY      Cursor Y (0-24)
$A005  SCROLLX    Horizontal scroll offset (block mode)
$A006  SCROLLY    Vertical scroll offset (block mode)
$A007  BANK       Reserved for future memory banking
$A008  STATUS     Read-only (bit 0: vsync, bits 1-2: collision flags)
$A009  SPREN      Sprite enable bits (low byte, sprites 0-7)
$A00A  SPRENH     Sprite enable bits (high byte, sprites 8-15)
$A00B  COLLST     Sprite-sprite collision register (read-only, read clears)
$A00C  COLLBG     Sprite-background collision register (read-only, read clears)
$A00D  BORDER     Border color (0-15)
$A00E  CHAROUT    Write: put char at cursor, advance cursor (replaces $F001)
$A00F  CHARIN     Read: last keypress, read clears (replaces $F004)
```

### Sprite Registers ($A010-$A06F)

16 sprites × 6 bytes each:

```
+0  X position low byte
+1  X position high bit (bit 0) + flags (bit 7: x-flip, bit 6: y-flip)
+2  Y position
+3  Color (0-15)
+4  Priority (bit 0: behind background)
+5  Shape index (0-15, points into $A200-$A3FF)
```

### Graphics Command Registers ($A070-$A078)

Parameters are written first, then the command byte triggers execution:

```
$A070  GFXCMD    Command byte (write last — triggers execution)
$A071  GFXP0     Parameter 0 low byte
$A072  GFXP0H    Parameter 0 high byte
$A073  GFXP1     Parameter 1 low byte
$A074  GFXP1H    Parameter 1 high byte
$A075  GFXP2     Parameter 2
$A076  GFXP3     Parameter 3
$A077  GFXP4     Parameter 4
$A078  GFXP5     Parameter 5
```

Command bytes:

```
$01  PLOT      P0=x, P1=y
$02  UNPLOT    P0=x, P1=y
$03  LINE      P0=x1, P1=y1, P2=x2, P3=y2
$04  CIRCLE    P0=x, P1=y, P2=r
$05  RECT      P0=x1, P1=y1, P2=x2, P3=y2
$06  FILL      P0=x1, P1=y1, P2=x2, P3=y2
$07  GCLS      Clear graphics layer
$08  GCOLOR    P0=draw color (0-15)
```

### Sprite Shape Data ($A200-$A3FF)

16 shapes, 32 bytes each. Each sprite is 16×16 block-pixels.
2 bytes per row × 16 rows = 32 bytes. Bit set = pixel on, bit clear = transparent.

## Unified Character I/O

All screen output routes through the VGC. There is no separate console path.

### CHAROUT ($A00E) — Output

When a byte is written:

- `$08` (BS) — cursor left, erase character
- `$0A` (LF) — cursor down one line
- `$0C` (FF) — clear screen, cursor to 0,0
- `$0D` (CR) — cursor to column 0, advance to next line
- `$13` (HOME) — cursor to 0,0
- Printable characters: write byte + current FGCOL into screen/color RAM at
  cursor position, advance cursor right

Cursor wrapping:
- Past column 79 → column 0, next line
- Past row 24 → scroll screen up, cursor stays on row 24

### CHARIN ($A00F) — Input

Returns 0 if no key waiting. Returns keypress byte and clears on read.

### Scrolling

When cursor moves below row 24:
1. Copy character RAM rows 1-24 up to rows 0-23 (shift 80 bytes)
2. Copy color RAM rows 1-24 up to rows 0-23
3. Clear row 24 to spaces with current background color
4. Cursor remains on row 24

### C64-Style Screen Editor

Cursor keys move freely across screen RAM. Typing overwrites character at cursor
position. On RETURN:

1. Find start of the logical line the cursor is on
2. Read characters from screen RAM across the line, trim trailing spaces
3. Feed those characters into the input buffer at CHARIN followed by `$0D`
4. EhBASIC reads them as freshly typed input

This enables editing previously listed program lines by cursoring up, changing
text, and pressing RETURN — exactly like a Commodore 64.

## EhBASIC Modifications

### New BASIC Commands

Graphics:

```
CLS                        Clear screen (all layers)
COLOR fg [,bg]             Set foreground/background color
LOCATE x, y               Move text cursor
PLOT x, y                 Set block pixel
UNPLOT x, y               Clear block pixel
LINE x1, y1, x2, y2      Draw line
CIRCLE x, y, r            Draw circle
RECT x1, y1, x2, y2      Draw rectangle
FILL x1, y1, x2, y2      Filled rectangle
MODE n                    Set display mode (0=text, 1=graphics, 2=mixed)
```

Sprites:

```
SPRITE n, ON/OFF               Enable/disable sprite
SPRITE n, x, y                Set sprite position
SPRITECOLOR n, c               Set sprite color
SPRITESHAPE n, shape           Set shape index (0-15)
SPRITEDATA shape, row, b1, b2 Define one row of shape data
```

Sound (stubbed):

```
SOUND channel, freq, duration
VOLUME channel, level
ENVELOPE channel, a, d, s, r
WAVE channel, type
```

### New BASIC Functions

```
SPRITEX(n)      Sprite X position
SPRITEY(n)      Sprite Y position
COLLISION(n)    Sprite-sprite collision status
BUMPED(n)       Sprite-background collision status
```

### Assembly Changes

**Token allocation:** New primary command tokens start after `TK_NMI = $A8`.
Secondary tokens (TAB, ELSE, TO, etc.) are renumbered above the new commands.
The boundary check in `LAB_1609` is updated.

**Keyword tables:** Add entries to the appropriate first-character tables
(TAB_ASCC, TAB_ASCL, etc.). Longer keywords before shorter ones.

**Dispatch table:** Add `.word LAB_xxx-1` entries to `LAB_CTBL` for each
new command.

**6502 handlers:** Each handler is ~20-40 bytes. Pattern:
1. Evaluate arguments using EhBASIC routines (LAB_EVNM, LAB_F2FX, LAB_1C01)
2. Write parameter values to VGC registers ($A071-$A078)
3. Write command byte to $A070 to trigger execution
4. RTS

**Function handlers:** Added to LAB_FTPL/LAB_FTBL. Read VGC registers, convert
to FAC1 integer, return.

**min_mon.asm:** Point ACIAout to write $A00E, ACIAin to read $A00F.

## .NET Architecture

### Component Diagram

```
                    ┌─────────────┐
                    │     Cpu     │
                    └──────┬──────┘
                           │ Read/Write
                    ┌──────┴──────┐
                    │CompositeBus │ Routes by address range
                    ├─────────────┤
          ┌─────────┼─────────────┼──────────┐
          │         │             │           │
   ┌──────┴───┐ ┌───┴────┐ ┌─────┴────┐ ┌───┴──────┐
   │   RAM    │ │  VGC   │ │   VSC    │ │ EhBASIC  │
   │$0000-    │ │$A000-  │ │$A100-    │ │  $C000-  │
   │  $9FFF   │ │ $B39F  │ │  $A1FF   │ │  $FFFF   │
   └──────────┘ └───┬────┘ │ (stub)   │ └──────────┘
                    │      └──────────┘
              ┌─────┴──────┐
              │  Renderer  │ Composites VGC state → terminal
              │  (30fps)   │
              └────────────┘
```

### Classes

**`CompositeBusDevice : IBusDevice`** — Routes Read/Write to the correct
component by address range. Replaces BasicBusDevice/TuiBasicBusDevice.

**`VirtualGraphicsController`** — Owns register bank, screen RAM, color RAM,
sprite shapes, sprite registers, and block graphics bitmap (in .NET memory,
not 6502 address space). Handles CHAROUT logic (cursor movement, scrolling,
control characters). Runs collision detection on sprite position changes.

**`VirtualSoundController`** — Stub. Accepts writes, does nothing.

**`TerminalRenderer`** — Runs on a 30fps timer. Reads VGC state. Composites
layers bottom to top: background → characters → block graphics → sprites.
Renders blinking cursor. Pushes result to DisplayView.

**`BlockGraphics`** — Bresenham line, midpoint circle, rectangle, fill
algorithms. Operates on the VGC's block graphics bitmap.

**`DisplayView : View`** — Terminal.Gui widget. Receives rendered frames from
TerminalRenderer. Captures keyboard input, feeds to VGC CHARIN register.

**`ScreenEditor`** — Implements C64-style screen editing. Tracks cursor
movement, handles RETURN key read-back from screen RAM.

### Rendering Pipeline

```
1. Read character RAM + color RAM → base text layer
2. If mode 1 or 2: composite block graphics bitmap
   - Pair vertical pixels into Unicode half-blocks (▀ ▄ █)
   - Map 4-bit colors to closest ANSI/Terminal.Gui colors
3. For each enabled sprite (priority order):
   a. Read 32-byte shape, apply x/y-flip
   b. Composite at sprite x,y (transparent = bit clear)
   c. Detect collisions, update COLLST/COLLBG
4. Draw cursor (blinking block at CURSX, CURSY)
5. Push frame to DisplayView
```

### Color Palette

16 colors, C64-inspired, mapped to closest ANSI equivalents:

```
 0 Black         4 Purple        8 Orange       12 Grey Med
 1 White         5 Green         9 Brown        13 Light Green
 2 Red           6 Blue         10 Light Red    14 Light Blue
 3 Cyan          7 Yellow       11 Grey Dark    15 Grey Light
```

## Project Structure

```
e6502/
  ehbasic/
    basic.asm            EhBASIC source (modified)
    min_mon.asm          Monitor source (modified)
    Makefile             Builds ehbasic.bin via ca65/ld65
  e6502/                 Core emulator library (unchanged)
  e6502.TUI/
    Program.cs           Entry point
    Hardware/
      CompositeBusDevice.cs
      VirtualGraphicsController.cs
      VirtualSoundController.cs
    Rendering/
      TerminalRenderer.cs
      BlockGraphics.cs
      DisplayView.cs
      ScreenEditor.cs
    Resources/
      ehbasic.bin        Built from ehbasic/ by pre-build step
  e6502UnitTests/        Existing tests (unchanged)
  e6502.CLI/             Existing CLI (unchanged for now)
```

## Build

```bash
# Assemble EhBASIC
cd ehbasic && make

# Build .NET solution (TUI pre-build step runs make automatically)
dotnet build

# Run
dotnet run --project e6502.TUI
```

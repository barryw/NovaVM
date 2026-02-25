# NCC — Nova C Compiler

## Overview

NCC is a C-like compiled language for the Nova virtual machine, designed for game development. It produces native 6502 machine code, includes a full-screen editor with integrated debugger, and ships as a 16K ROM that can be swapped with the BASIC ROM on demand.

## Design Goals

- **Game-first**: Fixed-point math, sprite/sound/graphics library, frame-accurate timing
- **Hardware-aware but structured**: Built-in library for all Nova hardware, with escape hatches to bare metal
- **Turbo Pascal experience**: Full-screen editor, one-key compile, integrated debugger
- **Fits in 16K ROM**: Editor, debugger UI, and runtime library in ROM; compiler runs on the C# host as virtual hardware
- **No line numbers**: Structured control flow, named functions, proper scoping

## Language Specification

### Primitive Types

| Type | Size | Range | Use |
|------|------|-------|-----|
| `byte` | 8-bit | 0-255 | Most game variables, hardware registers |
| `int` | 16-bit | -32768 to 32767 | Coordinates, counters |
| `uint` | 16-bit | 0-65535 | Addresses, unsigned math |
| `bool` | 8-bit | true/false | Flags |
| `fixed` | 16-bit | -128.0 to 127.996 (8.8) | Smooth movement, velocity |
| `ufixed` | 16-bit | 0.0 to 255.996 (8.8) | Unsigned fixed-point |

No `float`, no `long`, no 32-bit types. This is an 8-bit machine.

### Fixed-Point Math

`fixed` and `ufixed` are first-class types, not library hacks:

```c
fixed x = 100.0;
fixed vel = 0.75;
x += vel;              // 16-bit add
int screen_x = (int)x; // truncates fractional part
fixed half = x >> 1;   // shift on full 16 bits
```

### Structs and Enums

```c
enum State { IDLE, WALK, JUMP, FALL, DEAD };

struct Player {
  fixed x, y;
  fixed vx, vy;
  byte shape;
  byte health;
  State state;
  bool facing_left;
};

struct Player player;
player.x = 160.0;
player.state = IDLE;
```

### Arrays

Fixed-size only. No heap allocation.

```c
byte bullets_active[16];
int bullets_x[16];
struct Enemy enemies[8];
enemies[3].health = 10;
```

### Pointers

Limited but functional:

```c
byte *ptr = 0xA040;        // point at hardware registers
*ptr = 100;
struct Player *p = &player;
p->health -= 1;
```

### Constants

```c
#define MAX_ENEMIES 8
#define SCREEN_W 320
const byte colors[] = { RED, GREEN, BLUE, YELLOW };
```

### Strings

No string type. Character arrays only:

```c
byte name[16] = "PLAYER 1";   // null-terminated byte array
```

### What's Deliberately Excluded

- No heap allocation (`malloc`/`free`) — all memory is static or stack
- No floating point — `fixed` covers game math
- No multi-file linking — single compilation unit with `#include`
- No variadic functions, no function pointers (maybe later), no recursive structs
- No `long` / 32-bit integers
- No preprocessor macros beyond `#include` and `#define` constants

## Control Flow

Standard C control flow:

```c
// if/else
if (player.health <= 0) {
  player.state = DEAD;
} else if (player.y > 200) {
  player.state = FALL;
}

// while, do/while
while (enemy_count > 0) { update_wave(); }
do { key = get_key(); } while (key == 0);

// for
for (byte i = 0; i < MAX_ENEMIES; i++) {
  update_enemy(&enemies[i]);
}

// switch
switch (player.state) {
  case IDLE: animate_idle(); break;
  case WALK: animate_walk(); break;
  default: break;
}
```

## Functions

Typed, no forward declarations needed (two-pass compiler):

```c
void update_player(struct Player *p, byte input) {
  if (input & JOY_LEFT)  p->vx = -1.5;
  if (input & JOY_RIGHT) p->vx = 1.5;
  p->x += p->vx;
  p->y += p->vy;
}

byte find_free_bullet() {
  for (byte i = 0; i < 16; i++) {
    if (!bullets_active[i]) return i;
  }
  return 0xFF;
}
```

### Interrupt Handlers

```c
__interrupt void on_vsync() {
  frame_count++;
  // compiler generates RTI, saves/restores registers
}

void main() {
  set_irq(on_vsync);
}
```

### Inline Assembly

```c
void wait_scanline(byte line) {
  asm {
    LDA %line        // % prefix = local C variable
    CMP $A009
    BNE *-3
  };
}
```

`%varname` in `asm` blocks references C variables — compiler resolves their zero-page or stack locations.

## Memory Architecture & Overlay System

### Two-Tier Model

Program RAM holds active code. XRAM (512K) holds assets and overlay segments.

```
$0280 ┌─────────────────────┐
      │ Base code + globals │  Always resident
      │ (main loop, core    │
      │  functions, vars)   │
      ├─────────────────────┤ ← boundary set by #pragma
      │ Overlay window      │  Managed automatically
$9FFF └─────────────────────┘
```

### Overlay Window Configuration

```c
#pragma overlay_size 8192    // 8K window (default)
                             // also: 4096, 16384, or any multiple of 256
```

### Automatic Overlay Management

The programmer does not manually manage overlays. The compiler and runtime handle placement:

1. Compiler builds a call graph from `main()`
2. Functions marked `__resident` stay in base RAM
3. Remaining functions are grouped into overlay segments that fit the window
4. Each overlay function gets a trampoline in base RAM (~6 bytes) that checks if its segment is loaded, DMAs it from XRAM if not, then jumps
5. LRU eviction when the window is full

```c
__resident void game_loop() {     // always in RAM
  update_player();                // may live in an overlay
  update_enemies();               // may live in a different overlay
  draw_hud();                     // compiler figures it out
  vgc_vsync();
}
```

### Asset Pipeline

Assets are declared with `#asset`, bundled into a `.pak` file at compile time, loaded to XRAM at runtime:

```c
#asset sprite "hero.gfx"
#asset sprite "baddies.gfx"
#asset sid    "theme.sid"
#asset gfx    "titlescreen.gfx"
#asset binary "levels.dat"

void main() {
  ovl_init("mygame");           // load .pak from disk -> XRAM
  spr_load(hero);               // DMA: XRAM -> sprite shape RAM
  gfx_load(titlescreen);        // DMA: XRAM -> graphics bitmap
  sid_play(theme);              // DMA: XRAM -> CPU RAM, start playback
  ovl_read(levels, &map, sizeof(map));  // XRAM -> CPU RAM variable
}
```

Asset loading is cached — `spr_load(hero)` checks if hero sprites are already in sprite RAM before DMA'ing again. LRU eviction when sprite shape RAM is full.

### Compile-Time Safety

The compiler warns if:
- Base code exceeds available RAM (program RAM minus overlay window)
- Any single overlay segment exceeds the window size
- Asset pack exceeds XRAM capacity

## Hardware Library (nova.h)

All Nova hardware accessed through library functions. No magic numbers — all constants named.

### Named Constants

```c
// Colors
#define BLACK    0
#define WHITE    1
#define RED      2
#define CYAN     3
#define PURPLE   4
#define GREEN    5
#define BLUE     6
#define YELLOW   7
#define ORANGE   8
#define BROWN    9
#define LTRED    10
#define DKGRAY   11
#define GRAY     12
#define LTGREEN  13
#define LTBLUE   14
#define LTGRAY   15

// Graphics modes
#define MODE_TEXT      0
#define MODE_GFX_OVER  1
#define MODE_TEXT_OVER  2
#define MODE_GFX_ONLY  3

// Joystick / input bits
#define JOY_UP     0x01
#define JOY_DOWN   0x02
#define JOY_LEFT   0x04
#define JOY_RIGHT  0x08
#define JOY_FIRE   0x10
#define JOY_FIRE2  0x20

// Sprite priority
#define SPR_BEHIND    0
#define SPR_BETWEEN   1
#define SPR_INFRONT   2

// Memory spaces (for DMA/blit)
#define MEM_CPU    0
#define MEM_CHAR   1
#define MEM_COLOR  2
#define MEM_GFX    3
#define MEM_SPRITE 4
#define MEM_XRAM   5

// Network status
#define NET_DISCONNECTED 0
#define NET_CONNECTED    1
#define NET_DATA_READY   2
#define NET_SEND_READY   4
#define NET_ERROR        8
#define NET_REMOTE_CLOSED 16

// SID waveforms
#define WAVE_TRIANGLE 0
#define WAVE_SAW      1
#define WAVE_PULSE    2
#define WAVE_NOISE    3

// Key codes
#define KEY_NONE    0
#define KEY_RETURN  13
#define KEY_ESC     27
#define KEY_LEFT    128
#define KEY_RIGHT   129
#define KEY_UP      130
#define KEY_DOWN    131
#define KEY_F1      132
// ... etc

// VGC registers (for direct access when needed)
#define REG_MODE       0xA000
#define REG_BG_COLOR   0xA001
#define REG_FG_COLOR   0xA002
#define REG_BORDER     0xA003
#define REG_CURSOR_X   0xA004
#define REG_CURSOR_Y   0xA005
#define REG_SCROLL_X   0xA006
#define REG_SCROLL_Y   0xA007
#define REG_FRAME      0xA009
#define REG_SPR_COLL   0xA00B
#define REG_SPR_BUMP   0xA00C
#define REG_CHAR_OUT   0xA00E
#define REG_CHAR_IN    0xA00F

// SID registers (for direct access)
#define SID1_BASE      0xD400
#define SID2_BASE      0xD420

// Sprite registers (base + id * 8)
#define SPR_REG_BASE   0xA040
// Per sprite: +0 X low, +1 X high, +2 Y low, +3 Y high,
//             +4 shape, +5 flags, +6 priority, +7 transparent
```

### Graphics

```c
void gfx_mode(byte mode);
void gfx_cls();
void gfx_color(byte fg);
void gfx_plot(int x, int y);
void gfx_line(int x1, int y1, int x2, int y2);
void gfx_rect(int x1, int y1, int x2, int y2);
void gfx_fill(int x1, int y1, int x2, int y2);
void gfx_circle(int x, int y, byte r);
void gfx_paint(int x, int y);
```

### Sprites

```c
void spr_load(asset a);
void spr_enable(byte id);
void spr_disable(byte id);
void spr_move(byte id, int x, int y);
void spr_shape(byte id, byte shape);
void spr_flip(byte id, bool h, bool v);
void spr_priority(byte id, byte pri);
byte spr_collision();
byte spr_bumped();
```

### Text

```c
void cls();
void color(byte fg, byte bg);
void locate(byte col, byte row);
void print(byte *str);
void print_int(int val);
void print_hex(uint val);
byte get_key();
byte wait_key();
```

### Sound & Music

```c
void snd_play(byte note, byte dur, byte inst);
void snd_volume(byte vol);
void mus_instrument(byte id, byte wave, byte a, byte d, byte s, byte r);
void mus_seq(byte voice, byte *mml);
void mus_play();
void mus_stop();
void mus_tempo(uint bpm);
void mus_loop(bool on);
void sid_play(asset a);
void sid_stop();
```

### System

```c
void vgc_vsync();
byte frame_count();
void set_irq(void *handler);
void set_nmi(void *handler);
```

### Copper

```c
void copper_add(byte list, int x, int y, byte reg, byte val);
void copper_clear(byte list);
void copper_on(byte list);
void copper_off(byte list);
```

### DMA & Blitter

```c
void dma_copy(byte src_space, uint src, byte dst_space, uint dst, uint len);
void dma_fill(byte space, uint addr, uint len, byte val);
void blit_copy(byte src_space, uint src_addr, uint src_stride,
               byte dst_space, uint dst_addr, uint dst_stride,
               uint width, uint height);
void blit_fill(byte dst_space, uint dst_addr, uint dst_stride,
               uint width, uint height, byte val);
void blit_keyed(byte src_space, uint src_addr, uint src_stride,
                byte dst_space, uint dst_addr, uint dst_stride,
                uint width, uint height, byte transparent);
byte blit_busy();
```

### Memory

```c
void poke(byte addr, byte val);
byte peek(uint addr);
void memcpy(byte *dst, byte *src, uint len);
void memset(byte *dst, byte val, uint len);
```

### Networking

```c
byte net_open(byte *host, uint port);
void net_close(byte slot);
void net_listen(uint port);
void net_send(byte slot, byte *data, byte len);
byte net_recv(byte slot, byte *buf);
byte net_status(byte slot);
```

### Overlay

```c
void ovl_init(byte *pakname);
void ovl_read(asset a, byte *dst, uint len);
```

## Editor Environment

### Layout

Full-screen editor with syntax highlighting, launched on boot:

```
┌─ NCC ─── game.c ──────────────────── Ln 14 Col 8 ─┐
│ (source code with syntax highlighting)              │
│ ...                                                 │
├─────────────────────────────────────────────────────┤
│ F1:Help F3:Files F5:Run F7:Debug F9:Build  ESC:Menu │
└─────────────────────────────────────────────────────┘
```

### Editor Features

- Syntax highlighting (keywords, types, strings, numbers, comments)
- Auto-indent on `{` and `}`
- Brace matching
- Goto line (Ctrl+G)
- Find / Find+Replace (Ctrl+F / Ctrl+H)
- Undo / Redo (Ctrl+Z / Ctrl+Y)
- Source stored in XRAM, paged into view — files can exceed RAM
- File browser (F3) lists `.c` files in `~/e6502-programs`

### Build (F9)

Compiles current project. Errors shown inline with file and line number. Cursor jumps to first error. Enter cycles through errors.

```
├── Build ────────────────────────────────────────────┤
│ Compiling game.c... 3847 bytes code, 12K assets     │
│ enemies.c:42: error: undeclared variable 'halth'    │
│ 1 error, 1 warning                                  │
```

### Run (F5)

Builds if needed, then executes. Full screen handed to the program. Ctrl+Break stops execution and returns to editor.

### Debug (F7)

```
┌─ DEBUG ── game.c ──────────┬─ Watch ────────────────┐
│                             │ player.x    = 142.50   │
│ (source with current line   │ player.state= JUMP     │
│  highlighted by ►)          │ enemy_count = 5        │
│                             ├─ Stack ────────────────┤
│                             │ main()           :14   │
├────────────────────────────┴────────────────────────┤
│ F5:Continue F8:StepOver F7:StepIn F9:StepOut Esc:Stop│
│ F2:Breakpoint  F4:Watch  Tab:Switch panel            │
└─────────────────────────────────────────────────────┘
```

**Debugger features:**
- Breakpoints (F2) — toggle on current line
- Step over (F8), step into (F7), step out (F9)
- Watch panel (F4) — type-aware formatting (fixed shows decimal, enums show names)
- Call stack with function name and line number
- Continue (F5) — run to next breakpoint
- Break on collision — special breakpoint for hit detection debugging

**Implementation:** Compiler embeds symbol table in XRAM. Debug builds insert `BRK` instructions at line boundaries. BRK handler in ROM checks breakpoint table, updates watch panel, waits for input. Program runs on the real 6502 — not simulated.

## ROM Layout

```
$C000-$DFFF  Runtime library (8K)
             - nova.h implementations (native 6502)
             - overlay manager + LRU cache
             - fixed-point math routines
             - debug BRK handler + watch/step engine
             - string/memory utilities

$E000-$FB7F  Editor (~7K)
             - screen editor with syntax highlighting
             - file browser
             - find/replace engine
             - build output panel
             - debugger UI (watch, stack, breakpoint panels)

$FB80-$FDFF  Constants & tables (~640 bytes)
             - keyword table (for highlighting)
             - error message strings (compressed)
             - default color scheme

$FE00-$FEF7  Boot + menu (~248 bytes)

$FEF8-$FFF9  Reserved

$FFFA-$FFFF  CPU Vectors (NMI, RESET, IRQ)
```

## Compiler Controller (Virtual Hardware)

The compiler runs on the C# host, exposed as virtual hardware registers:

```
$A031  CmpCmd        - command (compile, status, read error)
$A032  CmpStatus     - result (idle, compiling, success, error)
$A033  CmpErrCount   - number of errors
$A034  CmpWarnCount  - number of warnings
$A035  CmpErrLineL   - error line number (low byte)
$A036  CmpErrLineH   - error line number (high byte)
$A037  CmpCodeSizeL  - compiled code size (low byte)
$A038  CmpCodeSizeH  - compiled code size (high byte)
$A039  CmpErrMsg     - read error message byte-by-byte
$A03A  CmpErrIdx     - current error index (write to select)
$A03B  CmpSrcAddrL   - source XRAM address low byte
$A03C  CmpSrcAddrM   - source XRAM address mid byte
$A03D  CmpSrcAddrH   - source XRAM address high byte (24-bit, full 512K)
```

## ROM Switching

**BASIC to NCC:** `NCC` command in BASIC, prompts "Switch to NCC? (Y/N)".

**NCC to BASIC:** Ctrl+B in the editor, prompts "Switch to BASIC? (Y/N)".

**Implementation:** Both ROM binaries embedded as resources in the C# assembly. ROM swap register at `$A03F`:
- Write `$01` = load NCC ROM
- Write `$02` = load BASIC ROM

`CompositeBusDevice` intercepts the write, swaps the 16K block at $C000-$FFFF, triggers CPU reset. Confirmation prompt happens in each ROM before the register write.

## Example: Complete Game Skeleton

```c
#include <nova.h>
#include <overlay.h>
#include "player.c"
#include "enemies.c"

#pragma overlay_size 8192

#asset sprite "hero.gfx"
#asset sprite "baddies.gfx"
#asset gfx    "titlescreen.gfx"
#asset sid    "theme.sid"
#asset binary "level1.dat"

struct Player player;
struct Enemy enemies[MAX_ENEMIES];
byte level_map[1024];

void init() {
  ovl_init("mygame");
  gfx_mode(MODE_GFX_OVER);
  gfx_load(titlescreen);
  sid_play(theme);
  wait_key();

  sid_stop();
  gfx_cls();
  spr_load(hero);
  spr_load(baddies);
  ovl_read(level1, level_map, sizeof(level_map));

  player.x = 160.0;
  player.y = 100.0;
  player.health = 3;
  player.state = IDLE;
  spr_enable(0);
}

__resident void game_loop() {
  byte input = get_key();
  update_player(&player, input);
  update_enemies(enemies, MAX_ENEMIES);

  byte coll = spr_collision();
  if (coll) handle_collisions(coll);

  spr_move(0, (int)player.x, (int)player.y);
  draw_hud();
  vgc_vsync();
}

void main() {
  init();
  while (player.health > 0) {
    game_loop();
  }
  game_over();
}
```

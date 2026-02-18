# Avalonia Renderer & Multicolor Sprites Design

## Summary

Replace the Terminal.Gui TUI with an Avalonia-based renderer that provides real pixel graphics. The 6502 emulator core is unchanged. The VGC gets multicolor sprite support and a unified command interface — no direct POKE access to sprite memory. Sprite shape data lives host-side (VDC-style private VRAM). The display renders at native 640x200 and scales with nearest-neighbor filtering to fill the window.

## Projects

| Action | Project | Notes |
|--------|---------|-------|
| **Create** | `e6502.Avalonia` | New Avalonia app, replaces TUI |
| **Delete** | `e6502.TUI` | Remove entirely |
| **Keep** | `e6502` | Core library, unchanged |
| **Keep** | `e6502.CLI` | Console BASIC, unchanged |
| **Keep** | `e6502.MCP` | MCP server, unchanged (TCP client) |
| **Keep** | `e6502UnitTests` | Core tests, unchanged |

## e6502.Avalonia Project Structure

```
e6502.Avalonia/
  Hardware/
    CompositeBusDevice.cs       Updated memory map, moved from TUI
    VirtualGraphicsController.cs  Multicolor sprites, new commands
    VirtualSoundController.cs   Carried from TUI
    FileIoController.cs         Carried from TUI
  Rendering/
    EmulatorCanvas.cs           Avalonia custom control, main render surface
    TextRenderer.cs             80x25 character grid using CP437 bitmap font
    GraphicsRenderer.cs         160x50 bitmap to pixels
    SpriteRenderer.cs           Multicolor 16x16 sprites with priority layers
    ColorPalette.cs             16-color palette mapped to Avalonia colors
  Input/
    ScreenEditor.cs             Keyboard input queue (carried from TUI)
  Ipc/
    EmulatorTcpServer.cs        TCP server for MCP (carried from TUI)
  Resources/
    cp437.bin                   8x8 bitmap font (2KB)
  App.axaml
  MainWindow.axaml              Single window containing EmulatorCanvas
  Program.cs                    Entry point, CPU thread, frame timer
```

## Display

- **Native resolution:** 640x200 pixels (160x50 VGC pixels at 4x scale)
- **Text cells:** 8x8 pixels each (80 columns x 25 rows)
- **Scaling:** Nearest-neighbor to fill window, locked 16:5 aspect ratio
- **Font:** CP437 8x8 bitmap font, embedded as resource (256 chars x 8 bytes = 2KB)
- **Frame rate:** 30fps render timer

## Render Pipeline

Renders to a `WriteableBitmap` at 640x200, then scaled to window.

Six passes, bottom to top:

1. Fill background color
2. Draw priority-0 sprites (behind text and graphics)
3. Draw text glyphs from screen RAM + color RAM
4. Draw priority-1 sprites (between text and graphics)
5. Draw graphics bitmap
6. Draw priority-2 sprites (in front of everything, default)

Sprite pixels with color 0 are transparent (skipped).

## Sprites

- **Count:** 16 sprites (index 0-15)
- **Size:** 16x16 pixels each
- **Color:** 4-bit per pixel (16 colors from palette, color 0 = transparent)
- **Shape storage:** Host-side only, not 6502-addressable (VDC-style private VRAM)
- **Shape size:** 128 bytes per sprite (16 rows x 8 bytes, 2 pixels per byte)
- **Pixel packing:** High nibble = left pixel, low nibble = right pixel
- **Flip:** Per-sprite X and Y flip flags
- **Priority:** 0 (behind all), 1 (between text/gfx), 2 (in front of all)
- **Collision detection:** Sprite-sprite and sprite-background, same as current

## VGC Memory Map

```
$A000       RegMode         0=text, 1=gfx, 2=mixed
$A001       RegBgCol        Background color (0-15)
$A002       RegFgCol        Foreground color (0-15)
$A003       RegCursorX      0-79
$A004       RegCursorY      0-24
$A005-$A007 (reserved)
$A008       RegStatus       Frame counter (read-only, incremented at 30fps)
$A009       RegSpriteCount  Number of enabled sprites (read-only)
$A00A       (reserved)
$A00B       RegColSt        Sprite-sprite collision bits (read-clears)
$A00C       RegColBg        Sprite-background collision bits (read-clears)
$A00D       (reserved)
$A00E       RegCharOut      Write character to screen at cursor
$A00F       RegCharIn       Read queued keyboard input

$A010       RegCmd          Command byte (write triggers execution)
$A011-$A01E P0-P13          14 parameter bytes

$A020-$A0FF (reserved)
$A100-$A1FF VSC registers (sound, stub)

$AA00-$B1CF Character RAM   80x25 = 2000 bytes
$B1D0-$B99F Color RAM       80x25 = 2000 bytes

$B9A0-$B9EF File I/O controller
$BA00-$BFFF (free, ~1.5KB)
$C000-$FFFF ROM (EhBASIC)
```

Note: Sprite shape data (16 x 128 bytes = 2KB) is stored host-side. The 6502 never addresses it directly. All sprite manipulation goes through the command register at $A010.

## Command Table

Write the command byte to `$A010` to trigger execution. Parameters are pre-loaded into `$A011-$A01E`.

### Graphics Commands

| Cmd | Byte | Params | Description |
|-----|------|--------|-------------|
| PLOT | $01 | P0:x(16) P2:y(16) | Set pixel at (x,y) |
| UNPLOT | $02 | P0:x(16) P2:y(16) | Clear pixel at (x,y) |
| LINE | $03 | P0:x0(16) P2:y0(16) P4:x1(16) P6:y1(16) | Bresenham line |
| CIRCLE | $04 | P0:x(16) P2:y(16) P4:radius(16) | Midpoint circle |
| RECT | $05 | P0:x0(16) P2:y0(16) P4:x1(16) P6:y1(16) | Hollow rectangle |
| FILL | $06 | P0:x0(16) P2:y0(16) P4:x1(16) P6:y1(16) | Filled rectangle |
| GCLS | $07 | — | Clear graphics bitmap |
| GCOLOR | $08 | P0:color(8) | Set graphics draw color (0-15) |

### Sprite Commands

| Cmd | Byte | Params | Description |
|-----|------|--------|-------------|
| SDEF | $10 | P0:sprite(8) P1:x(8) P2:y(8) P3:color(8) | Set one sprite pixel |
| SROW | $11 | P0:sprite(8) P1:row(8) P2-P9:data(8x8) | Define full 16-pixel row |
| SCLR | $12 | P0:sprite(8) | Clear sprite shape to transparent |
| SCOPY | $13 | P0:src(8) P1:dest(8) | Copy sprite shape between slots |
| SPOS | $14 | P0:sprite(8) P1:x(16) P3:y(8) | Set sprite position |
| SENA | $15 | P0:sprite(8) | Enable (show) sprite |
| SDIS | $16 | P0:sprite(8) | Disable (hide) sprite |
| SFLIP | $17 | P0:sprite(8) P1:flags(8) | Set flip (bit0=X, bit1=Y) |
| SPRI | $18 | P0:sprite(8) P1:priority(8) | Set layer: 0, 1, or 2 |

Parameter notation: `(8)` = 8-bit single byte, `(16)` = 16-bit little-endian pair, `(8x8)` = 8 consecutive bytes.

### SROW Pixel Packing

Each of the 8 data bytes encodes 2 pixels:

```
Byte: [high nibble][low nibble] = [left pixel color][right pixel color]

Example row: red, red, blue, blue, white, white, black, black, ...
Colors:       2    2    6    6    1    1    0    0   ...
Bytes:       $22  $66  $11  $00  ...
```

## EhBASIC Integration

The 6502 assembly handlers parse BASIC arguments and write to command registers. From BASIC, sprite usage looks like:

```basic
SYS SCLR, 0              : REM clear sprite 0
SYS SROW, 0, 0, data...  : REM define row 0
SYS SPOS, 0, 100, 25     : REM position at (100, 25)
SYS SPRI, 0, 2           : REM priority 2 (front)
SYS SENA, 0              : REM show it
```

Exact BASIC syntax depends on the existing handler conventions.

## Vector & Jump Table ($0200-$02FF)

Modeled after the C64/C128 Kernal. All EhBASIC handlers and assembly code reference hardware through this indirection layer. If hardware addresses change, only the boot-time initialization needs updating.

### Address Vector Table ($0200-$020F)

```
$0200  VGC_BASE     .word $A000   ; VGC register base
$0202  CMD_REG      .word $A010   ; command register
$0204  SCREEN_RAM   .word $AA00   ; character RAM base
$0206  COLOR_RAM    .word $B1D0   ; color RAM base
$0208  VSC_BASE     .word $A100   ; sound controller base
$020A  FIO_BASE     .word $B9A0   ; file I/O controller base
$020C  (reserved)
$020E  (reserved)
```

### Jump Table ($0220-$027F)

Fixed entry points — each is a 3-byte `JMP addr` instruction.

```
; Graphics commands
$0220  JMP CmdPlot       ; PLOT pixel
$0223  JMP CmdUnplot     ; UNPLOT pixel
$0226  JMP CmdLine       ; LINE
$0229  JMP CmdCircle     ; CIRCLE
$022C  JMP CmdRect       ; RECT
$022F  JMP CmdFill       ; FILL
$0232  JMP CmdGcls       ; GCLS (clear graphics)
$0235  JMP CmdGcolor     ; GCOLOR (set draw color)

; Sprite commands
$0240  JMP CmdSprDef     ; SDEF (set sprite pixel)
$0243  JMP CmdSprRow     ; SROW (define sprite row)
$0246  JMP CmdSprClr     ; SCLR (clear sprite)
$0249  JMP CmdSprCopy    ; SCOPY (copy sprite shape)
$024C  JMP CmdSprPos     ; SPOS (set position)
$024F  JMP CmdSprEna     ; SENA (enable)
$0252  JMP CmdSprDis     ; SDIS (disable)
$0255  JMP CmdSprFlip    ; SFLIP (set flip flags)
$0258  JMP CmdSprPri     ; SPRI (set priority)

; Utility
$0260  JMP ReadCollSpr   ; Read sprite-sprite collision
$0263  JMP ReadCollBg    ; Read sprite-background collision
$0266  JMP ReadFrame     ; Read frame counter
$0269  JMP WaitFrame     ; Wait for next frame (busy-wait on counter)
```

Boot code initializes the vector table and jump table. The actual command routines load parameters into registers, read the command register address from the vector table, and trigger execution. Client code never hardcodes hardware addresses.

## MCP Integration

The TCP server (`EmulatorTcpServer`) moves to the Avalonia project. Existing MCP commands continue to work. New commands to add:

- `sprite_define_row` — wraps SROW command
- `sprite_set_pixel` — wraps SDEF command
- `sprite_position` — wraps SPOS command
- `sprite_enable` / `sprite_disable` — wraps SENA/SDIS
- `sprite_clear` — wraps SCLR
- `sprite_priority` — wraps SPRI
- `read_sprites` — updated to return multicolor shape data

## Migration from TUI

1. Create `e6502.Avalonia` project with Avalonia dependencies
2. Move Hardware/ classes from TUI, update VGC for new memory map and commands
3. Implement Rendering/ classes (EmulatorCanvas, TextRenderer, GraphicsRenderer, SpriteRenderer)
4. Implement Input/ScreenEditor (adapt from TUI's Terminal.Gui key handling to Avalonia key events)
5. Move TCP server from TUI
6. Wire up MainWindow, CPU thread, frame timer
7. Update MCP tools for new sprite commands
8. Update EhBASIC handlers for new command register layout
9. Delete e6502.TUI project
10. Update solution file and CI

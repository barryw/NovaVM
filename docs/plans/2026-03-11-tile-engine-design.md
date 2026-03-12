# Tile Map Engine Design

## Overview

A hardware tile map engine for the VGC, exposed as **Mode 4** (Tiles + Sprites). Provides NES-style scrolling with four nametables, configurable 8×8 or 16×16 tiles, 16 sub-palettes, per-tile attributes, pixel-precise hardware scrolling, and sprite-tile collision detection. Tile data lives in host memory (not in the 6502 address space) and is populated via VGC commands or DMA from XRAM.

The design philosophy: use the horsepower at our disposal to make scrolling tile maps pleasant to program from both BASIC and assembly. No scanline counting, no mid-frame register tricks, no hoops.

## Display Mode

**Mode 4** — written to `RegMode` ($A000).

Compositing order (back to front):
1. Background fill (BgColor)
2. Priority-0 sprites (behind)
3. Tile layer (tiles with priority bit = 0)
4. Priority-1 sprites (between)
5. Tile layer (tiles with priority bit = 1, drawn over between-sprites)
6. Priority-2 sprites (front, always on top)

Color index matching the global tile transparent color is not drawn, allowing lower layers to show through.

No text or bitmap graphics layers are active in Mode 4. Sprites work identically to all other modes (16 hardware sprites, 3 priority levels, per-sprite transparent color, collision detection).

## Tile Dimensions

Configurable via `TileConfig` register:
- **8×8 mode:** 40×25 visible tiles (320÷8 × 200÷8). 32 bytes per tile (8×8 pixels, 4-bit color = 4 bytes/row × 8 rows).
- **16×16 mode:** 20×13 visible tiles (320÷16 × 200÷16, bottom 8 pixels show partial 13th row). 128 bytes per tile (16×16 pixels, 4-bit color = 8 bytes/row × 16 rows).

Tile pixel data is stored as packed nibbles, high nibble first (same as sprite pixel packing). Each nibble is a 4-bit index into the tile's assigned sub-palette.

## Tile Definitions

256 tile slots (indices 0–255), stored in host memory:
- 8×8 mode: 256 × 32 bytes = 8 KB
- 16×16 mode: 256 × 128 bytes = 32 KB

Tiles are defined by copying pixel data from CPU RAM into host tile memory via the `TDEF` command.

## Nametables

Four nametables arranged in a 2×2 grid:

```
┌───────────────┬───────────────┐
│  Nametable 0  │  Nametable 1  │
│   (40×25)     │   (40×25)     │
├───────────────┼───────────────┤
│  Nametable 2  │  Nametable 3  │
│   (40×25)     │   (40×25)     │
└───────────────┴───────────────┘
```

Each nametable: 40×25 = 1000 entries.

Total logical map:
- 8×8 mode: 80×50 tiles = 640×400 pixels
- 16×16 mode: 80×50 tiles = 1280×800 pixels

Scroll wraps at the edges of the logical map.

### Mirroring Modes

The mirror register selects which nametables are independent vs. mirrored:

| Value | Layout | Use Case |
|-------|--------|----------|
| 0 | Horizontal mirror (NT0=NT1, NT2=NT3) | Vertical scrolling games |
| 1 | Vertical mirror (NT0=NT2, NT1=NT3) | Horizontal scrolling games |
| 2 | Four-screen (all independent) | Free 4-directional scrolling |
| 3 | Single-screen (NT0 only, all mirrored) | Static screens, boss rooms |

In mirrored modes, writes to a mirrored nametable update the source. Reads return the source data.

## Attribute Table

Each nametable entry has a corresponding attribute byte (1000 bytes per nametable, 4000 bytes total):

```
Bit 7   : Vertical flip
Bit 6   : Horizontal flip
Bit 5   : Priority (0 = behind between-sprites, 1 = in front of between-sprites)
Bit 4   : (reserved)
Bit 3-0 : Sub-palette index (0–15)
```

Attributes are stored in a parallel array — attribute[i] corresponds to nametable[i].

## Sub-Palette System

16 sub-palettes × 16 colors = 256 color entries, stored in host palette RAM (256 × 3 bytes RGB = 768 bytes).

Each tile pixel's 4-bit value is combined with the tile's sub-palette index to look up the final color:

```
final_color = palette_ram[(sub_palette_index × 16) + pixel_value]
```

Sub-palette 0 defaults to the standard VGC 16-color palette (C64 colors) so existing color indices work unchanged out of the box.

### Palette Commands

Palettes are defined by writing RGB triplets from CPU RAM:

```
TPAL sub_palette_index, source_address
```

Source data: 16 × 3 bytes (R, G, B for each of the 16 color slots). Or individual color writes for fine control:

```
TPALC sub_palette_index, color_index, R, G, B
```

## Transparent Color

A single global register (`TileTransColor`) defines which color index is transparent across all tiles. Default: 0.

When a tile pixel's 4-bit value equals `TileTransColor`, that pixel is not drawn, allowing lower layers (background, behind-sprites) to show through.

This is the raw pixel value before palette lookup — i.e., the index within the tile data, not the sub-palette color.

## Scrolling

Two 16-bit registers provide pixel-precise scroll position:

| Register | Name | Range |
|----------|------|-------|
| $A0C2–$A0C3 | TileScrollX | 0 to (map_width_pixels − 1) |
| $A0C4–$A0C5 | TileScrollY | 0 to (map_height_pixels − 1) |

Little-endian (low byte first). Values wrap modulo the total map size.

At render time, the VGC reads the scroll registers and calculates which tiles are visible. The renderer handles sub-tile pixel offsets, nametable boundary crossings, and wrapping — the 6502 program just writes a position.

### Scroll ranges by mode

| Tile Size | Mirror | Map Size (px) | Scroll Range |
|-----------|--------|---------------|--------------|
| 8×8 | Four-screen | 640×400 | 0–639, 0–399 |
| 8×8 | H-mirror | 320×400 | 0–319, 0–399 |
| 8×8 | V-mirror | 640×200 | 0–639, 0–199 |
| 8×8 | Single | 320×200 | 0–319, 0–199 |
| 16×16 | Four-screen | 1280×800 | 0–1279, 0–799 |

### Smooth Scrolling from BASIC

```basic
MODE 4
TSCROLL 0, 0          : REM top-left corner
TSCROLL 160, 100      : REM center of map
TSCROLL TX+1, TY      : REM smooth scroll right by 1 pixel
```

### Smooth Scrolling from Assembly

```asm
; scroll right by 1 pixel
    CLC
    LDA $A0C2       ; TileScrollX low
    ADC #1
    STA $A0C2
    LDA $A0C3       ; TileScrollX high
    ADC #0
    STA $A0C3       ; takes effect next frame
```

No special timing needed. Write the registers whenever you want; the renderer uses the current value at the start of each frame.

## Sprite-Tile Collision Detection

During rendering, when a visible sprite pixel (non-transparent) overlaps a visible tile pixel (non-transparent), a collision is flagged.

### Collision Registers

| Register | Name | R/W | Description |
|----------|------|-----|-------------|
| $A0D0 | TileColL | R | Sprite-tile collision bitmask low byte (sprites 0–7) |
| $A0D1 | TileColH | R | Sprite-tile collision bitmask high byte (sprites 8–15) |

Reading either register clears both (same behavior as existing sprite-sprite collision register `RegColSt`).

Each bit corresponds to a sprite index. Bit set = that sprite is overlapping a non-transparent tile pixel.

### Collision Tile Lookup

For game logic, the program needs to know *which tile* was hit. Since the sprite position and scroll offset are known, this is a calculation the 6502 can do:

```
tile_x = (sprite_x + scroll_x) / tile_width
tile_y = (sprite_y + scroll_y) / tile_height
tile_index = nametable[tile_y * 40 + tile_x]
```

This keeps the hardware simple (just a bitmask) while still giving the game everything it needs. BASIC helper:

```basic
TX = (SPRX(S) + TSCROLLX) / 8 : REM tile X coordinate
TY = (SPRY(S) + TSCROLLY) / 8 : REM tile Y coordinate
T = TPEEK(NT, TX, TY)          : REM read tile index at position
```

## Register Map ($A0C0–$A0DF)

Located after sprite registers ($A040–$A0BF), within the existing VGC range ($A000–$A0FF).

### Configuration Registers

| Address | Name | R/W | Description |
|---------|------|-----|-------------|
| $A0C0 | TileConfig | R/W | bit 0: tile size (0=8×8, 1=16×16); bits 2–1: mirror mode (0–3) |
| $A0C1 | TileTransColor | R/W | Global transparent color index (0–15). Default 0. |
| $A0C2 | TileScrollXL | R/W | Scroll X low byte |
| $A0C3 | TileScrollXH | R/W | Scroll X high byte |
| $A0C4 | TileScrollYL | R/W | Scroll Y low byte |
| $A0C5 | TileScrollYH | R/W | Scroll Y high byte |
| $A0C6 | TileStatus | R | Last command status (0=ok, nonzero=error) |
| $A0C7 | TileCmd | W | Write triggers tile command execution |

### Command Parameters

| Address | Name | R/W | Description |
|---------|------|-----|-------------|
| $A0C8 | TileP0 | R/W | Parameter 0 |
| $A0C9 | TileP1 | R/W | Parameter 1 |
| $A0CA | TileP2 | R/W | Parameter 2 |
| $A0CB | TileP3 | R/W | Parameter 3 |
| $A0CC | TileAddrL | R/W | Source/dest address low byte (CPU RAM) |
| $A0CD | TileAddrH | R/W | Source/dest address high byte (CPU RAM) |
| $A0CE | TilePalP0 | R/W | Palette parameter 0 |
| $A0CF | TilePalP1 | R/W | Palette parameter 1 |

### Collision Registers

| Address | Name | R/W | Description |
|---------|------|-----|-------------|
| $A0D0 | TileColL | R | Sprite-tile collision bits 0–7 (read clears both) |
| $A0D1 | TileColH | R | Sprite-tile collision bits 8–15 (read clears both) |

### Nametable Peek/Poke

| Address | Name | R/W | Description |
|---------|------|-----|-------------|
| $A0D2 | TilePeekNT | W | Write NT# + coords via P0/P1/P2 to populate TilePeekVal |
| $A0D3 | TilePeekVal | R | Tile index at last peeked position |
| $A0D4 | TilePeekAttr | R | Attribute byte at last peeked position |

## Tile Commands

Written to `TileCmd` ($A0C7). Parameters set in TileP0–P3 and TileAddr before writing the command byte.

| Cmd | Name | Parameters | Description |
|-----|------|------------|-------------|
| $01 | TDEF | P0=tile#, TileAddr=src | Define one tile from CPU RAM pixel data |
| $02 | TDEFBULK | P0=start#, P1=count, TileAddr=src | Define multiple consecutive tiles |
| $03 | TPUT | P0=NT#, P1=X, P2=Y, P3=tile# | Set single nametable entry |
| $04 | TATTR | P0=NT#, P1=X, P2=Y, P3=attr | Set single attribute entry |
| $05 | TFILL | P0=NT#, P1=tile# | Fill entire nametable with one tile |
| $06 | TROW | P0=NT#, P1=Y, TileAddr=src | Write 40 tile indices to a row |
| $07 | TCOL | P0=NT#, P1=X, TileAddr=src | Write 25 tile indices to a column |
| $08 | TLOAD | P0=NT#, TileAddr=src | Load entire nametable (1000 bytes) from CPU RAM |
| $09 | TALOAD | P0=NT#, TileAddr=src | Load entire attribute table (1000 bytes) from CPU RAM |
| $0A | TPAL | PalP0=sub_palette#, TileAddr=src | Load 16×3 RGB bytes into sub-palette |
| $0B | TPALC | PalP0=sub_palette#, PalP1=color#, P0=R, P1=G, P2=B | Set one color in a sub-palette |
| $0C | TPEEK | P0=NT#, P1=X, P2=Y | Read tile index + attr into peek registers |
| $0D | TROWATTR | P0=NT#, P1=Y, TileAddr=src | Write 40 attribute bytes to a row |
| $0E | TCOLATTR | P0=NT#, P1=X, TileAddr=src | Write 25 attribute bytes to a column |
| $0F | TCLS | (none) | Clear all nametables and attributes to 0 |

## DMA Integration

Tile definitions and nametable data can be staged in the 512KB XRAM and transferred via the existing XMC:

1. Load tileset from disk into XRAM (`XLOAD`)
2. Fetch tile data from XRAM into CPU RAM (`XFETCH`)
3. Define tiles from CPU RAM (`TDEF` / `TDEFBULK`)

Or load nametable maps the same way. This enables large worlds stored in XRAM with only the visible data transferred to tile RAM as needed.

Future optimization: direct XRAM-to-tile DMA command that bypasses CPU RAM entirely.

## BASIC Commands

All tile commands are implemented in the extension ROM and available as BASIC keywords.

### Mode and Configuration

```basic
MODE 4                      : REM enter tile mode (tiles + sprites)
TILESIZE 8                  : REM set 8×8 tiles (default)
TILESIZE 16                 : REM set 16×16 tiles
MIRROR 0                    : REM horizontal mirror (vertical scrolling)
MIRROR 1                    : REM vertical mirror (horizontal scrolling)
MIRROR 2                    : REM four-screen (full 4-way scrolling)
MIRROR 3                    : REM single-screen
TTRANS 0                    : REM set transparent color index (default 0)
```

### Tile Definitions

```basic
TDEF 0, $4000               : REM define tile 0 from data at $4000
TDEF 0, 32, $4000           : REM define 32 tiles starting at tile 0
```

### Nametable Operations

```basic
TPUT 0, 5, 3, 42            : REM NT 0, x=5, y=3, set tile #42
TATTR 0, 5, 3, %11000001    : REM NT 0, x=5, y=3: flip H+V, palette 1
TFILL 0, 0                  : REM fill NT 0 with tile 0
TROW 0, 5, $4000            : REM load row 5 of NT 0 from $4000
TCOL 0, 10, $4000           : REM load column 10 of NT 0 from $4000
TLOAD 0, $4000              : REM load entire NT 0 from $4000
TCLS                        : REM clear all nametables
```

### Scrolling

```basic
TSCROLL 160, 100             : REM set scroll position (pixels)
TSCROLL TX+1, TY             : REM expressions allowed
TX = TSCROLLX                : REM read current X scroll
TY = TSCROLLY                : REM read current Y scroll
```

### Palette

```basic
TPAL 0, $5000                : REM load sub-palette 0 from 48 bytes at $5000
TPALC 0, 1, 255, 0, 0       : REM sub-palette 0, color 1 = bright red
```

### Tile Peek

```basic
T = TPEEK(0, 5, 3)           : REM read tile index at NT 0, x=5, y=3
A = TPEEKATTR(0, 5, 3)       : REM read attribute at NT 0, x=5, y=3
```

### Collision

```basic
C = TILECOL                   : REM read 16-bit sprite-tile collision bitmask
IF C AND 1 THEN ...           : REM sprite 0 hit a tile
IF C AND 4 THEN ...           : REM sprite 2 hit a tile
```

### File I/O

```basic
TSAVE "world1"                : REM save tileset + all nametables + palettes
TLOAD "world1"                : REM load tileset + all nametables + palettes
```

These use the FileIoController with a new `.tile` file format that bundles tile definitions, nametable data, attribute data, palette data, and configuration (tile size, mirror mode) in a single file.

## TCP/MCP Interface

New TCP commands for external tooling (tile editors, AI agents):

| Command | Parameters | Description |
|---------|------------|-------------|
| `tile_def` | `tile`, `data` (hex) | Define a tile |
| `tile_def_bulk` | `start`, `count`, `data` (hex) | Define multiple tiles |
| `tile_put` | `nt`, `x`, `y`, `tile` | Set nametable entry |
| `tile_attr` | `nt`, `x`, `y`, `attr` | Set attribute entry |
| `tile_fill` | `nt`, `tile` | Fill nametable |
| `tile_row` | `nt`, `y`, `data` (hex) | Write nametable row |
| `tile_col` | `nt`, `x`, `data` (hex) | Write nametable column |
| `tile_scroll` | `x`, `y` | Set scroll position |
| `tile_config` | `size`, `mirror`, `trans` | Set configuration |
| `tile_palette` | `index`, `data` (hex) | Define sub-palette |
| `tile_cls` | (none) | Clear all |
| `tile_peek` | `nt`, `x`, `y` | Read tile + attr |
| `tile_collision` | (none) | Read collision bitmask |
| `tile_save` | `filename` | Save to disk |
| `tile_load` | `filename` | Load from disk |

## Rendering Implementation

### Per-Frame Tile Sampling

In `EmulatorCanvas.RenderFramebuffer()`, Mode 4 adds a tile sampling path:

```
For each pixel (px, py) on screen:
    // Apply scroll offset
    worldX = (px + scrollX) mod mapWidthPixels
    worldY = (py + scrollY) mod mapHeightPixels

    // Determine which nametable (accounting for mirroring)
    ntCol = worldX / screenWidthPixels    // 0 or 1
    ntRow = worldY / screenHeightPixels   // 0 or 1
    nt = resolveNametable(ntRow, ntCol, mirrorMode)

    // Local position within nametable
    localX = worldX mod screenWidthPixels
    localY = worldY mod screenHeightPixels

    // Which tile and pixel within tile
    tileCol = localX / tileWidth
    tileRow = localY / tileHeight
    pixelX = localX mod tileWidth
    pixelY = localY mod tileHeight

    // Look up tile and attribute
    ntIndex = tileRow * 40 + tileCol
    tileNum = nametable[nt][ntIndex]
    attr = attributes[nt][ntIndex]

    // Apply flips
    if (attr & 0x80) pixelY = (tileHeight - 1) - pixelY   // V flip
    if (attr & 0x40) pixelX = (tileWidth - 1) - pixelX     // H flip

    // Read pixel from tile data
    subPalette = attr & 0x0F
    colorIndex = tilePixels[tileNum][pixelY][pixelX]

    if (colorIndex == transColor) → transparent, skip
    else → finalColor = paletteRam[subPalette * 16 + colorIndex]
```

### Sprite-Tile Collision

During the same per-pixel loop, when a sprite pixel is being composited and the tile pixel at that position is non-transparent, set the collision bit for that sprite. Collision is accumulated across the entire frame and latched into the collision registers, which are cleared on read.

### Performance Consideration

The tile lookup per pixel is simple integer math (shifts and masks for power-of-2 tile sizes). At 320×200 = 64,000 pixels per frame at 60Hz, this is well within the capabilities of the host machine. The nametable and tile data are small and cache-friendly.

## Host Memory Summary

| Data | Size | Notes |
|------|------|-------|
| Tile definitions | 8 KB (8×8) or 32 KB (16×16) | 256 tiles |
| Nametables | 4 × 1000 = 4 KB | Tile indices |
| Attribute tables | 4 × 1000 = 4 KB | Per-tile attributes |
| Palette RAM | 256 × 3 = 768 bytes | 16 sub-palettes × 16 colors |
| **Total** | **~17 KB (8×8) or ~41 KB (16×16)** | All in host memory |

## .tile File Format

Binary format for `TSAVE`/`TLOAD`:

| Offset | Size | Description |
|--------|------|-------------|
| 0 | 4 | Magic: "TILE" |
| 4 | 1 | Version (1) |
| 5 | 1 | Tile size (8 or 16) |
| 6 | 1 | Mirror mode (0–3) |
| 7 | 1 | Transparent color index |
| 8 | 768 | Palette RAM (16 sub-palettes × 16 × 3 RGB) |
| 776 | 8192 or 32768 | Tile definitions (256 tiles) |
| varies | 4000 | Nametable data (4 × 1000) |
| varies | 4000 | Attribute data (4 × 1000) |

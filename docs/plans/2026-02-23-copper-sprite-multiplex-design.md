# Copper Expansion & Sprite Multiplexing Design

## Goal

Expand the copper to access sprite registers, enabling vertical sprite multiplexing — reusing 16 hardware sprites across multiple Y bands to display far more than 16 on screen. Also expand sprite shape storage to 256 slots with DMA support for bulk shape loading from XRAM.

## Shape Slot Expansion

Current: 16 shape slots (2KB), each hardwired to its sprite index.

New: **256 shape slots** (32KB). Each sprite has a 1-byte shape index register pointing to any slot. The copper can repoint a sprite's shape in a single register write.

DMA between XRAM (512KB) and sprite shape RAM already works via `DmaSpaceVgcSprite`. The expanded buffer scales the existing addressing — no DMA changes needed beyond updating the size validation.

## Memory-Mapped Sprite Registers

**Address range:** `$A040`–`$A0BF` (128 bytes, 8 per sprite)

`$A020`–`$A03F` is reserved for future VGC registers.

### Per-Sprite Layout

Base address: `$A040 + (sprite_index * 8)`

| Offset | Name     | R/W | Description                              |
|--------|----------|-----|------------------------------------------|
| +0     | XLo      | R/W | X position low byte                      |
| +1     | XHi      | R/W | X position high byte (signed)            |
| +2     | YLo      | R/W | Y position low byte                      |
| +3     | YHi      | R/W | Y position high byte (signed)            |
| +4     | Shape    | R/W | Shape slot index (0–255)                 |
| +5     | Flags    | R/W | bit0=xFlip, bit1=yFlip, bit7=enable      |
| +6     | Priority | R/W | 0=behind all, 1=between, 2=in front      |
| +7     | Reserved | R/W | Future use                               |

### Integration with Existing Commands

- `CmdSprPos` writes to XLo/XHi/YLo/YHi registers
- `CmdSprEna` sets bit7 of Flags register
- `CmdSprDis` clears bit7 of Flags register
- `CmdSprFlip` writes bit0/bit1 of Flags register
- `CmdSprPri` writes to Priority register
- Direct register reads give the 6502 access to sprite state without commands
- Shape index defaults to sprite index (sprite 0 → slot 0, etc.) for backward compatibility

## Copper Changes

### Expanded Register Whitelist

The copper can now write:

| Register(s)         | Granularity | Notes                       |
|---------------------|-------------|-----------------------------|
| Mode ($A000)        | pixel       | Display mode switching       |
| BgColor ($A001)     | pixel       | Background color             |
| ScrollX ($A005)     | pixel       | Horizontal scroll            |
| ScrollY ($A006)     | pixel       | Vertical scroll              |
| Sprite regs ($A040–$A0BF) | scanline | All 128 sprite register bytes |

### Copper Event Resolution

Copper events targeting sprite registers fire at **scanline granularity**: the event's Y coordinate determines when it fires, X is ignored. Sprite state changes take effect for the remainder of that scanline and all subsequent scanlines (until another copper event changes it).

Non-sprite copper events retain **pixel granularity** as today.

### Copper Register Addressing

The existing `CmdCopperAdd` accepts an absolute address ($A000–$A0BF) or a register index. The index range expands from 0–15 to 0–191 to cover sprite registers. `TryResolveCopperRegister` and `IsCopperWritableRegister` are updated accordingly.

## Rendering Pipeline Restructure

### Current Pipeline (full-frame)

1. `BuildSpritePriorityMaps()` — rasterize all 16 sprites into 3 × 320×200 maps
2. Snapshot render state
3. Pixel loop — copper fires, composite layers using pre-baked sprite maps
4. Collision detection on baked maps

### New Pipeline (per-scanline)

```
snapshot render state from VGC registers (including sprite registers)

for each scanline y (0..199):
    fire copper events at this Y (scanline-level: sprite registers)
    evaluate which sprites overlap this scanline
    rasterize overlapping sprites into 3 × 320-byte line buffers (behind/between/front)
    for each pixel x (0..319):
        fire copper events at exact (x,y) for non-sprite registers
        composite: bg → behind sprites → gfx/text → between sprites → front sprites
    accumulate collision data from this scanline
```

### Key Changes

- `BuildSpritePriorityMaps()` is removed entirely
- Sprite evaluation moves into the scanline loop: for each scanline, check which of the 16 sprites overlap (comparing sprite Y ≤ scanline < sprite Y + 16), then rasterize only the intersecting row of each overlapping sprite
- Three 320-byte line buffers replace the three 64000-byte priority maps (memory reduction: 192KB → 960 bytes)
- Collision detection accumulates per-scanline: sprite-sprite overlap checked per-row, sprite-background checked against gfx pixels as they're sampled
- The copper snapshot of sprite state is mutable per-scanline — copper events update it, and sprite evaluation reads the current state each scanline

### Rendering Loop Performance

The rendering loop is **exempt from VM clock constraints**. It runs on the UI thread at whatever speed is needed to maintain 60Hz output. The VM clock governs CPU, DMA, and other hardware — not the renderer.

Per-scanline sprite evaluation: 16 sprites × 200 scanlines = 3200 overlap checks per frame, but each check is a simple Y range comparison. Only overlapping sprites (typically 2–4 per scanline) have their pixel row rasterized. Total pixel work is similar to the current pre-pass — just distributed across scanlines.

## Backward Compatibility

- Default shape index for sprite N is N — existing programs that never touch the shape index register see identical behavior
- Existing sprite commands (`CmdSprPos`, `CmdSprEna`, etc.) now write through to the memory-mapped registers, keeping both interfaces in sync
- Copper programs that only use the original 4 registers continue to work unchanged
- Sprite shape RAM expands from 2KB to 32KB but the first 2KB is laid out identically

## Multiplexing Example

Display 32 sprites using 16 hardware sprites with copper multiplexing:

```
; Define 32 shapes in slots 0–31
; Set up copper list:
;   At Y=0:   sprite 0 → shape 0, pos (x0,0),  enable
;             sprite 1 → shape 1, pos (x1,0),  enable
;             ...
;             sprite 15 → shape 15, pos (x15,0), enable
;   At Y=100: sprite 0 → shape 16, pos (x16,100), enable
;             sprite 1 → shape 17, pos (x17,100), enable
;             ...
;             sprite 15 → shape 31, pos (x31,100), enable
```

Each sprite is reused once at the Y=100 boundary, doubling the visible sprite count.

# Tile Map Engine Implementation Plan

Based on [tile-engine-design.md](2026-03-11-tile-engine-design.md).

## Phase 1: Constants and Host Memory

**Files:** `VgcConstants.cs`

Add all tile engine constants:
- Tile register block: `$A0C0–$A0DF` (config, scroll, command, params, collision, peek)
- Tile VGC commands: `0x30–0x3F` (TDEF, TDEFBULK, TPUT, TATTR, TFILL, TROW, TCOL, TLOAD, TALOAD, TPAL, TPALC, TPEEK, TROWATTR, TCOLATTR, TCLS)
- Tile memory space ID: `MemSpaceTile = 0x04`
- File I/O commands: `FioCmdTSave`, `FioCmdTLoad`
- Mode 4 constant

## Phase 2: Tile State in VGC

**Files:** `VirtualGraphicsController.cs`

Add host-side tile state:
- `byte[]` tile definitions (256 × 32 or 128 bytes, sized for 16×16 max)
- `byte[][]` nametables (4 × 1000)
- `byte[][]` attribute tables (4 × 1000)
- `byte[]` palette RAM (256 × 3 = 768 bytes)
- Config state: tile size, mirror mode, transparent color
- 16-bit scroll X/Y
- Collision bitmask (ushort)
- Peek result registers
- Thread lock for tile data

Initialize palette RAM sub-palette 0 to the existing 16-color C64 palette.

Add `Reset()` cleanup for all tile state.

## Phase 3: Register Read/Write

**Files:** `VirtualGraphicsController.cs`

Extend `OwnsAddress()` to include `$A0C0–$A0DF`.

Implement `WriteTileRegister(int address, byte data)`:
- Config register → update tile size + mirror mode
- TransColor register → update transparent color
- ScrollX/Y low/high → update 16-bit scroll values
- TileCmd → dispatch to `ExecuteTileCommand(data)`
- Parameter registers → store in tile command param array

Implement `ReadTileRegister(int address)`:
- Scroll registers → return current values
- Config/TransColor → return current values
- TileColL/H → return collision bitmask, clear on read
- TilePeekVal/Attr → return last peek result
- TileStatus → return last command status

## Phase 4: Tile Command Dispatch

**Files:** `VirtualGraphicsController.cs`

Implement `ExecuteTileCommand(byte cmd)` with all 15 commands from the design:

- **TDEF ($01):** Copy tile pixel data from CPU RAM (via `_busMemory`) into host tile definitions array. P0=tile index, TileAddr=source in CPU RAM. Read 32 bytes (8×8) or 128 bytes (16×16) depending on current tile size config.
- **TDEFBULK ($02):** Same but P0=start, P1=count, reads count×tileSize bytes.
- **TPUT ($03):** P0=NT, P1=X, P2=Y, P3=tile index. Write single nametable entry. Resolve mirroring.
- **TATTR ($04):** P0=NT, P1=X, P2=Y, P3=attribute byte. Write single attribute entry. Resolve mirroring.
- **TFILL ($05):** P0=NT, P1=tile index. Fill entire nametable with one tile index. Clear corresponding attributes to 0.
- **TROW ($06):** P0=NT, P1=Y row. Read 40 bytes from TileAddr into nametable row.
- **TCOL ($07):** P0=NT, P1=X column. Read 25 bytes from TileAddr into nametable column.
- **TLOAD ($08):** P0=NT. Read 1000 bytes from TileAddr into entire nametable.
- **TALOAD ($09):** P0=NT. Read 1000 bytes from TileAddr into entire attribute table.
- **TPAL ($0A):** PalP0=sub-palette index. Read 48 bytes (16×RGB) from TileAddr into palette RAM.
- **TPALC ($0B):** PalP0=sub-palette, PalP1=color index, P0=R, P1=G, P2=B. Set one color.
- **TPEEK ($0C):** P0=NT, P1=X, P2=Y. Read tile index and attribute into peek result registers.
- **TROWATTR ($0D):** P0=NT, P1=Y. Read 40 bytes from TileAddr into attribute row.
- **TCOLATTR ($0E):** P0=NT, P1=X. Read 25 bytes from TileAddr into attribute column.
- **TCLS ($0F):** Clear all 4 nametables and attributes to 0.

All nametable writes must resolve mirroring (writes to mirrored NT redirect to source NT).

## Phase 5: Bus Routing

**Files:** `CompositeBusDevice.cs`

Extend the VGC write/read interception to cover `$A0C0–$A0DF`:
- These addresses already fall within `VgcBase–VgcEnd` ($A000–$A0FF), so `CompositeBusDevice` should already route them to VGC
- Verify the VGC's `OwnsAddress` and `Read`/`Write` methods handle the new range
- No changes to CompositeBusDevice needed if VGC already handles the full $A000–$A0FF range

## Phase 6: Render State Snapshot

**Files:** New file `Rendering/TileRenderState.cs`

Create `TileRenderState` class (following `SpriteRenderState` pattern):
- Snapshot from VGC: tile size, mirror mode, transparent color, scroll X/Y
- Deep-copy nametable and attribute arrays (4 × 1000 bytes each)
- Deep-copy tile definitions (under lock)
- Pre-compute palette lookup table: `uint[256]` mapping palette RAM RGB to BGRA pixel values
- `FromVgc(VirtualGraphicsController vgc)` static factory

This snapshot is taken once per frame, ensuring the renderer sees consistent state even if the 6502 is modifying tile data mid-frame.

## Phase 7: Tile Renderer

**Files:** New file `Rendering/TileRenderer.cs`

Static class with per-scanline tile sampling:

```
RasterizeScanline(
    int scanlineY,
    TileRenderState tiles,
    Span<byte> tileLineBehind,    // priority-0 tile pixels
    Span<byte> tileLineFront,     // priority-1 tile pixels
    Span<uint> tilePalBehind,     // resolved BGRA for behind tiles
    Span<uint> tilePalFront)      // resolved BGRA for front tiles
```

For each X pixel (0–319):
1. Compute world position: `worldX = (x + scrollX) % mapWidth`, `worldY = (scanlineY + scrollY) % mapHeight`
2. Determine nametable from world position and mirror mode
3. Compute tile column/row within nametable
4. Look up tile index and attribute from nametable/attribute arrays
5. Compute pixel position within tile, applying H/V flip from attribute
6. Read 4-bit pixel value from tile definition data
7. If pixel == transparent color → skip (leave 0 in output)
8. Otherwise: compute palette index = `(subPalette × 16) + pixelValue`, look up BGRA from palette table
9. Write to behind or front line based on tile priority bit (attribute bit 5)

## Phase 8: Render Loop Integration

**Files:** `EmulatorCanvas.cs`

In `RenderFramebuffer()`:

1. After `SpriteRenderState.FromVgc()`, add `TileRenderState.FromVgc()` (only when Mode == 4)
2. Allocate scanline buffers for tile pixels (behind and front layers)
3. Inside the Y scanline loop, call `TileRenderer.RasterizeScanline()` (only Mode 4)
4. Add Mode 4 compositing branch:

```
Background fill
→ sprite behind (priority 0)
→ tile behind (priority bit 0)
→ sprite between (priority 1)
→ tile front (priority bit 1)
→ sprite front (priority 2)
```

For sprite-tile collision: during compositing, when a sprite pixel is non-zero and a tile pixel at the same position is non-zero, set the corresponding bit in the collision accumulator. After the frame, latch to VGC collision registers.

## Phase 9: Sprite-Tile Collision

**Files:** `TileRenderer.cs`, `EmulatorCanvas.cs`, `VirtualGraphicsController.cs`

During the per-pixel compositing loop in Mode 4:
- Combine `_spriteMask[x]` (existing per-sprite bit) with tile opacity
- If `spriteMask[x] != 0` AND tile pixel at x is non-transparent → OR sprite bits into tile collision accumulator
- After full frame: write accumulated `ushort` to VGC via `SetTileCollision(ushort mask)`
- VGC latches it; `ReadTileRegister` for `TileColL`/`TileColH` returns and clears

## Phase 10: Memory Space Extension

**Files:** `VirtualGraphicsController.cs`

Extend `TryReadMemorySpace()` and `TryWriteMemorySpace()` with `MemSpaceTile`:
- Maps into tile definition data (under lock)
- Enables existing `CmdMemRead`/`CmdMemWrite` VGC commands to access tile data
- Enables `GSAVE`/`GLOAD` to work with tile data via space parameter

## Phase 11: File I/O

**Files:** `FileIoController.cs`, `VgcConstants.cs`

Add `FioCmdTSave` and `FioCmdTLoad` commands:

**TSAVE:** Writes `.tile` binary file containing:
- Header: magic "TILE", version, tile size, mirror mode, transparent color
- Palette RAM (768 bytes)
- Tile definitions (8KB or 32KB)
- Nametable data (4 × 1000 bytes)
- Attribute data (4 × 1000 bytes)

**TLOAD:** Reads `.tile` file, populates all tile state. Sets mode to 4 automatically.

Add VGC accessor methods for FileIoController to read/write tile data:
- `GetTileData()` / `SetTileData()` for bulk tile definition access
- `GetNametable(int nt)` / `SetNametable(int nt, byte[] data)`
- `GetAttributeTable(int nt)` / `SetAttributeTable(int nt, byte[] data)`
- `GetPaletteRam()` / `SetPaletteRam(byte[] data)`

## Phase 12: TCP Server

**Files:** `EmulatorTcpServer.cs`

Add TCP command handlers matching the design doc's TCP/MCP interface table. Each handler:
1. Extracts parameters from JSON request
2. Writes to VGC tile registers / executes tile commands
3. Returns JSON response with status

## Phase 13: MCP Tools

**Files:** `e6502.MCP/EmulatorTools.cs`

Add `[McpServerTool]` methods for tile operations, wrapping the TCP commands. Higher-level helpers where useful (e.g., `DefineTileFromArray` that formats pixel data).

## Phase 14: Extension ROM (BASIC Commands)

**Files:** Extension ROM assembly source

Add BASIC keywords to the extension ROM tokenizer and executor:
- `MODE 4` — already handled by existing MODE command, just needs VGC to accept value 4
- `TILESIZE n` — write to TileConfig register
- `MIRROR n` — write to TileConfig register
- `TTRANS n` — write to TileTransColor register
- `TDEF tile#, addr` / `TDEF start, count, addr` — set params, execute TDEF/TDEFBULK command
- `TPUT nt, x, y, tile#` — set params, execute TPUT command
- `TATTR nt, x, y, attr` — set params, execute TATTR command
- `TFILL nt, tile#` — set params, execute TFILL command
- `TROW nt, y, addr` — set params, execute TROW command
- `TCOL nt, x, addr` — set params, execute TCOL command
- `TLOAD nt, addr` — set params, execute TLOAD command (nametable from RAM)
- `TCLS` — execute TCLS command
- `TSCROLL x, y` — write to scroll registers
- `TPAL sub, addr` — set params, execute TPAL command
- `TPALC sub, color, r, g, b` — set params, execute TPALC command
- `TSAVE "file"` / `TLOAD "file"` — file I/O variants (overloaded with string arg)
- `TPEEK(nt, x, y)` — function returning tile index at position
- `TPEEKATTR(nt, x, y)` — function returning attribute at position
- `TILECOL` — function returning 16-bit collision bitmask
- `TSCROLLX` / `TSCROLLY` — functions returning current scroll position

## Phase 15: Unit Tests

**Files:** `e6502UnitTests/TileEngineTests.cs` (new)

Test groups:
1. **Register read/write** — config, scroll, transparent color round-trip
2. **TDEF** — define tile, verify pixel data stored correctly
3. **TDEFBULK** — define multiple tiles
4. **Nametable operations** — TPUT, TFILL, TROW, TCOL, TLOAD, verify contents
5. **Attribute operations** — TATTR, TROWATTR, TCOLATTR, TALOAD, verify contents
6. **Mirroring** — write to NT0, verify NT1 reflects in vertical mirror mode, etc.
7. **TPEEK** — peek returns correct tile index and attribute
8. **TCLS** — all nametables and attributes zeroed
9. **Palette** — TPAL loads 48 bytes correctly, TPALC sets individual color
10. **Scroll** — 16-bit scroll X/Y read/write, verify wrap calculation
11. **Tile rendering** — set up simple tileset + nametable, verify correct pixels sampled at known positions
12. **Tile rendering with flip** — H/V flip attributes produce expected pixel layout
13. **Tile rendering with scroll** — verify pixel sampling shifts with scroll offset
14. **Collision** — set up sprite overlapping non-transparent tile, verify collision bit set and read-clears
15. **Transparent color** — verify pixels matching trans color are skipped
16. **File I/O** — TSAVE then TLOAD round-trips all tile state

## Execution Order

Phases 1–5 first (foundation: constants, state, registers, commands, bus). Then Phase 6–9 (rendering + collision). Then 10–11 (memory space + file I/O). Then 12–13 (TCP/MCP). Then 14 (BASIC). Phase 15 tests written alongside each phase.

Phases 1–4 can be done in one pass (they're all in VgcConstants.cs and VirtualGraphicsController.cs). Phase 6–8 can be done in one pass (render state + renderer + integration). This gives us roughly 6 implementation waves:

1. **Wave 1:** Constants + VGC state + registers + commands (Phases 1–5)
2. **Wave 2:** Render state + renderer + render loop (Phases 6–8)
3. **Wave 3:** Collision detection (Phase 9)
4. **Wave 4:** Memory space + file I/O (Phases 10–11)
5. **Wave 5:** TCP + MCP (Phases 12–13)
6. **Wave 6:** Extension ROM BASIC commands (Phase 14)

Tests built during each wave.

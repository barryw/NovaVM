# Sprite Collision Hardware Plan

## Status

Planning only. Do not implement this until the current video/border/sprite work is stable.

The current Avalonia renderer has useful collision behavior, but the FPGA implementation is incomplete. Before exposing this as stable NovaVM behavior, the VGC collision interface should be made 16-bit and pixel-accurate for all 16 sprites.

## Goals

- Support all 16 hardware sprites in collision registers.
- Detect sprite-to-sprite collisions by overlapping non-transparent sprite pixels, not bounding boxes.
- Detect sprite-to-character collisions by overlapping a non-transparent sprite pixel with an "on" font pixel.
- Detect sprite-to-bitmap-background collisions by overlapping a non-transparent sprite pixel with a nonzero graphics pixel.
- Detect sprite-to-tile collisions by overlapping a non-transparent sprite pixel with a non-transparent tile pixel.
- Keep collision handling invisible to BASIC programmers and straightforward for assembly programmers.
- Keep Avalonia and FPGA behavior aligned.

## Non-Goals

- Pair-specific sprite collision matrices in the first pass.
- Border collision reporting.
- Bounding-box collision acceleration.
- Per-tile collision class/material reporting.

## Current Audit

### Avalonia

Avalonia already has the right basic model:

- `SpriteRenderer.RasterizeScanline()` builds a per-pixel `spriteMask`.
- `SpriteRenderer.AccumulateCollisions()` sets sprite-to-sprite bits when more than one sprite contributes to the same pixel.
- `SpriteRenderer.AccumulateCollisions()` sets sprite-to-background bits when a sprite pixel overlaps a nonzero graphics pixel.
- `EmulatorCanvas` already accumulates sprite-to-tile collisions by checking `spriteMask[x]` against `tileOpaque[x]`.

Limitations:

- Collision values are truncated to 8 bits when stored in the current VGC sprite collision registers.
- Sprite-to-character collision is not currently part of the collision register path.
- Tile collision read-clear behavior is risky for a 16-bit value because low-byte reads can clear before the high byte is read.

### FPGA

The FPGA side has mostly placeholder behavior:

- `vgc.sv` has `collision_ss` and `collision_bg`, but both are only 8-bit.
- `collision_ss` is never set.
- `collision_bg` is set to `8'hFF` when any sprite pixel overlaps any nonzero graphics pixel, so sprite identity is lost.
- `vgc_sprites.sv` scanline buffer stores only `{priority, color, valid}`. It does not retain sprite ID or sprite mask.
- `tile_engine.sv` has 16-bit `collision_bits`, but nothing currently sets it.
- Sprite collision docs/Avalonia say read-clears, while FPGA sprite collision currently clears on write.

## Collision Semantics

Use 16-bit participant masks. Bit `n` means sprite `n` participated in that collision type during the accumulation window.

For phase 1, sprite-to-sprite reports participants, not pairs. Example: if sprite 0 overlaps sprite 1, bits 0 and 1 are set. If sprite 2 separately overlaps sprite 3 in the same frame, bits 2 and 3 are also set. This does not identify which sprite hit which other sprite.

Collision detection should be independent of visual priority. If two non-transparent sprite pixels occupy the same coordinate, both sprites collide even if one sprite visually wins. If a sprite is behind a tile/text/gfx pixel but has a non-transparent pixel there, the collision bit still sets.

Character collision should use rendered glyph "on" pixels only. Character cell background color does not count. Cursor overlay should not count as character collision unless we intentionally add a separate cursor collision feature later.

Sprite-to-tile collision should use tile opacity, not tile color value alone. Any non-transparent tile pixel counts, regardless of tile priority.

Border masking should not create collision bits. The border hides sprites visually, but it is not a collision surface in this phase.

## Register Interface

The current `$A00B/$A00C` registers cannot grow naturally because `$A00D-$A00F` are already assigned. We need a clean 16-bit register interface instead of bolting high bytes into random core-register holes.

Proposed new collision register block in currently reserved VGC space:

| Address | Name | Description |
|---|---|---|
| `$A0E8` | `CollSpriteL` | Sprite-to-sprite collision bits 0-7 |
| `$A0E9` | `CollSpriteH` | Sprite-to-sprite collision bits 8-15 |
| `$A0EA` | `CollBgL` | Sprite-to-character/bitmap-background collision bits 0-7 |
| `$A0EB` | `CollBgH` | Sprite-to-character/bitmap-background collision bits 8-15 |
| `$A0EC` | `CollTileL` | Sprite-to-tile collision bits 0-7 |
| `$A0ED` | `CollTileH` | Sprite-to-tile collision bits 8-15 |
| `$A0EE` | `CollClear` | Write bits to clear selected collision groups |
| `$A0EF` | Reserved | Future collision control/status |

Suggested `CollClear` bits:

| Bit | Meaning |
|---|---|
| `0` | Clear sprite-to-sprite collision |
| `1` | Clear sprite-to-background/character collision |
| `2` | Clear sprite-to-tile collision |
| `7` | Clear all collision registers |

Recommended read behavior:

- Reads should be stable and side-effect free.
- Software explicitly clears collision registers by writing `CollClear`.

Rationale: read-clear is awkward for 16-bit values on a 6502. If reading the low byte clears the register, the high byte can be lost. If we really want read-clear, implement a latched two-byte read where low-byte read snapshots the full 16-bit value and high-byte read clears after returning the latched high byte. Explicit clear is simpler and less surprising.

Compatibility option:

- `$A00B` can remain a low-byte alias for sprite-to-sprite collision during transition.
- `$A00C` can remain a low-byte alias for sprite-to-background/character collision during transition.
- Before declaring the VM stable, decide whether these aliases stay documented or become internal/deprecated.

## Hardware Design

### Sprite Mask Scanline Buffer

Add a parallel per-pixel sprite mask buffer to `vgc_sprites.sv`.

Current scanline buffer:

```text
2 banks * SPR_PLANE_W * 7 bits
{ priority[1:0], color[3:0], valid }
```

Add:

```text
2 banks * SPR_PLANE_W * 16 bits
sprite_mask[15:0]
```

In 720x480 mode, `SPR_PLANE_W` is 360, so this costs:

```text
2 * 360 * 16 = 11,520 flip-flops
```

That is not free, but it keeps the behavior deterministic and avoids the LUTRAM/BRAM scanline-buffer problems we already hit. If timing or utilization becomes a problem, revisit memory inference later.

### Sprite-to-Sprite Collision

During sprite scanline preparation:

1. Decode each non-transparent sprite pixel.
2. Compute `current_mask = 16'h0001 << sprite_index`.
3. Read the existing mask at the destination scanline pixel.
4. If `existing_mask != 0`, accumulate `collision_ss_next |= existing_mask | current_mask`.
5. Write back `existing_mask | current_mask`.

The final color/priority buffer can continue to decide which sprite is visible. Collision uses the mask buffer, not the winning visible pixel.

### Sprite-to-Background/Character Collision

Expose the current display pixel's `spr_pixel_mask[15:0]` from `vgc_sprites.sv` to `vgc.sv`, aligned with `spr_pixel_hit`.

During compositing:

```text
if spr_pixel_mask != 0 and graphics pixel is nonzero:
    collision_bg |= spr_pixel_mask

if spr_pixel_mask != 0 and text glyph pixel is on:
    collision_bg |= spr_pixel_mask
```

This makes `BUMPED(n)` mean "sprite n touched a non-transparent background layer pixel", where background includes bitmap graphics and rendered character glyph pixels.

### Sprite-to-Tile Collision

In tile mode:

```text
if spr_pixel_mask != 0 and tile_opaque_d2 != 0:
    collision_tile |= spr_pixel_mask
```

This should live in the VGC compositor path because that is where the sprite mask and tile opacity are aligned.

The existing `tile_engine.sv` collision register can either be removed from the tile engine and replaced by the unified collision block, or left as a read alias of the unified tile collision mask. Prefer one authoritative storage location to avoid drift.

## BASIC/API Behavior

Current `COLLISION(n)` and `BUMPED(n)` ignore `n` and return an 8-bit register value. That should change.

Recommended BASIC behavior:

- `COLLISION(n)` returns nonzero if sprite `n` participated in a sprite-to-sprite collision.
- `BUMPED(n)` returns nonzero if sprite `n` touched bitmap graphics or a character glyph pixel.
- `TILECOL` returns the full 16-bit sprite-to-tile participant mask.
- Add or document a collision clear command/function so BASIC programs do not need to poke registers.

Optional additional BASIC functions:

- `COLLMASK` returns the full 16-bit sprite-to-sprite mask.
- `BUMPMASK` returns the full 16-bit sprite-to-background/character mask.
- `TILECOL` already fills this role for tiles.

Do not promise pair-specific results from `COLLISION(n)` unless we add a per-sprite partner matrix later.

## Implementation Steps

1. Finalize register allocation and clear semantics.
2. Update `VgcConstants`, `novavm.inc`, memory-map docs, and help docs.
3. Update Avalonia collision storage to preserve 16-bit sprite/sprite and sprite/background values.
4. Add sprite-to-character collision to Avalonia.
5. Fix tile collision read semantics in Avalonia.
6. Add the `spr_pixel_mask` scanline buffer to `vgc_sprites.sv`.
7. Accumulate sprite-to-sprite collision during sprite scanline preparation.
8. Accumulate sprite-to-background/character and sprite-to-tile collision in `vgc.sv`.
9. Remove or alias old/stub tile collision storage so only one source of truth remains.
10. Update NovaBASIC `COLLISION(n)`, `BUMPED(n)`, and any new mask/clear routines.
11. Update debug bridge/MCP reads to expose the new 16-bit masks.
12. Run targeted Verilator tests before attempting full synth.

## Test Plan

Add FPGA tests for:

- Two sprites with overlapping bounding boxes but no overlapping non-transparent pixels: no collision.
- Two sprites with one overlapping non-transparent pixel: both participant bits set.
- Sprite 0 colliding with sprite 9: low and high bytes both work.
- Transparent sprite pixels do not collide.
- Sprite priority does not affect collision.
- Sprite over nonzero graphics pixel sets background collision.
- Sprite over zero graphics pixel does not set background collision.
- Sprite over character glyph "on" pixel sets background collision.
- Sprite over character cell background or glyph "off" pixel does not set background collision.
- Sprite over opaque tile pixel sets tile collision.
- Sprite over transparent tile pixel does not set tile collision.
- Reads are stable and explicit clear clears only the selected collision group.

Add Avalonia tests for the same cases where practical, especially high-byte sprite IDs and sprite-to-character collision.

## Open Questions

- Should `$A00B/$A00C` remain documented low-byte aliases, or should we remove them before stable VM docs?
- Should `BUMPED(n)` include both graphics and character pixels, or should character collision get a separate BASIC function?
- Do we want pair-specific sprite collision later? If yes, reserve an extension path now rather than overloading the phase-1 mask registers.
- Should tile collision continue to be exposed as `TILECOL`, or should we add a more general collision mask function family?

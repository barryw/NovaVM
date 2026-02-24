namespace e6502.Avalonia.Rendering;

using e6502.Avalonia.Hardware;

public struct SpritePixel
{
    public int ScreenX;
    public int ScreenY;
    public byte Color;
}

public static class SpriteRenderer
{
    public static List<SpritePixel> GetSpritePixels(VirtualGraphicsController vgc, int index)
    {
        var state = vgc.GetSpriteState(index);
        if (!state.enabled) return [];

        ReadOnlySpan<byte> shape = vgc.GetSpriteShape(index);
        byte transColor = vgc.GetSpriteTransColor(index);
        bool xFlip = (state.flags & 0x01) != 0;
        bool yFlip = (state.flags & 0x02) != 0;

        var pixels = new List<SpritePixel>(256);

        for (int row = 0; row < 16; row++)
        {
            int srcRow = yFlip ? 15 - row : row;
            int rowBase = srcRow * VgcConstants.SpriteBytesPerRow;

            for (int col = 0; col < 16; col++)
            {
                int srcCol = xFlip ? 15 - col : col;
                int byteIdx = rowBase + srcCol / 2;
                byte color = (srcCol % 2 == 0)
                    ? (byte)((shape[byteIdx] >> 4) & 0x0F)
                    : (byte)(shape[byteIdx] & 0x0F);

                if (color == transColor) continue; // transparent

                pixels.Add(new SpritePixel
                {
                    ScreenX = state.x + col,
                    ScreenY = state.y + row,
                    Color = color,
                });
            }
        }

        return pixels;
    }

    /// <summary>
    /// Rasterizes all sprites that overlap a given scanline into three line buffers (behind/between/front).
    /// Also fills spriteMask with bitmask of which sprites contributed to each pixel (for collision detection).
    /// </summary>
    public static void RasterizeScanline(
        int scanlineY,
        SpriteRenderState sprites,
        ReadOnlySpan<byte> shapeRam,
        Span<byte> lineBehind,
        Span<byte> lineBetween,
        Span<byte> lineFront,
        Span<ushort> spriteMask)
    {
        lineBehind.Clear();
        lineBetween.Clear();
        lineFront.Clear();
        spriteMask.Clear();

        for (int i = 0; i < VgcConstants.MaxSprites; i++)
        {
            if (!sprites.IsEnabled(i)) continue;

            int sy = sprites.GetY(i);
            if (scanlineY < sy || scanlineY >= sy + VgcConstants.SpritePixelSize) continue;

            int row = scanlineY - sy;
            bool xFlip = sprites.IsXFlip(i);
            bool yFlip = sprites.IsYFlip(i);
            int srcRow = yFlip ? 15 - row : row;

            Span<byte> target = sprites.GetPriority(i) switch
            {
                VgcConstants.SpritePriBehindAll => lineBehind,
                VgcConstants.SpritePriBetween => lineBetween,
                _ => lineFront
            };

            int slotOffset = sprites.GetShapeIndex(i) * VgcConstants.SpriteShapeSize;
            int rowBase = slotOffset + srcRow * VgcConstants.SpriteBytesPerRow;

            for (int col = 0; col < VgcConstants.SpritePixelSize; col++)
            {
                int srcCol = xFlip ? 15 - col : col;
                int byteIdx = rowBase + srcCol / 2;
                byte color = (srcCol % 2 == 0)
                    ? (byte)((shapeRam[byteIdx] >> 4) & 0x0F)
                    : (byte)(shapeRam[byteIdx] & 0x0F);

                if (color == sprites.GetTransColor(i)) continue;

                int screenX = sprites.GetX(i) + col;
                if ((uint)screenX >= VgcConstants.GfxWidth) continue;

                target[screenX] = color;
                spriteMask[screenX] |= (ushort)(1 << i);
            }
        }
    }

    /// <summary>
    /// Accumulates collision bits for one scanline using the sprite mask from RasterizeScanline.
    /// </summary>
    public static void AccumulateCollisions(
        ReadOnlySpan<ushort> spriteMask,
        VirtualGraphicsController vgc,
        int scrollX,
        int scrollY,
        int scanlineY,
        ref ushort spriteToSprite,
        ref ushort spriteToBg)
    {
        for (int x = 0; x < VgcConstants.GfxWidth; x++)
        {
            ushort mask = spriteMask[x];
            if (mask == 0) continue;

            // Sprite-sprite: if more than one bit set, those sprites collide
            if ((mask & (mask - 1)) != 0)
                spriteToSprite |= mask;

            // Sprite-background: check gfx pixel at scrolled position
            int gfxX = (x + scrollX) % VgcConstants.GfxWidth;
            if (gfxX < 0) gfxX += VgcConstants.GfxWidth;
            int gfxY = (scanlineY + scrollY) % VgcConstants.GfxHeight;
            if (gfxY < 0) gfxY += VgcConstants.GfxHeight;

            if (vgc.GetGfxPixelColor(gfxX, gfxY) != 0)
                spriteToBg |= mask;
        }
    }

    public static (ushort spriteToSprite, ushort spriteToBg) DetectCollisions(VirtualGraphicsController vgc)
    {
        // Build pixel sets for all enabled sprites
        var pixelSets = new HashSet<(int x, int y)>?[VgcConstants.MaxSprites];
        for (int i = 0; i < VgcConstants.MaxSprites; i++)
        {
            var state = vgc.GetSpriteState(i);
            if (!state.enabled) continue;

            var pixels = GetSpritePixels(vgc, i);
            var set = new HashSet<(int x, int y)>(pixels.Count);
            foreach (var p in pixels)
                set.Add((p.ScreenX, p.ScreenY));
            pixelSets[i] = set;
        }

        ushort spriteToSprite = 0;
        ushort spriteToBg = 0;

        for (int i = 0; i < VgcConstants.MaxSprites; i++)
        {
            if (pixelSets[i] is null) continue;

            // Sprite-background collision
            foreach (var (px, py) in pixelSets[i]!)
            {
                if (px >= 0 && px < VgcConstants.GfxWidth &&
                    py >= 0 && py < VgcConstants.GfxHeight &&
                    vgc.GetGfxPixelColor(px, py) != 0)
                {
                    spriteToBg |= (ushort)(1 << i);
                    break;
                }
            }

            // Sprite-sprite collision
            for (int j = i + 1; j < VgcConstants.MaxSprites; j++)
            {
                if (pixelSets[j] is null) continue;

                if (pixelSets[i]!.Overlaps(pixelSets[j]!))
                {
                    spriteToSprite |= (ushort)(1 << i);
                    spriteToSprite |= (ushort)(1 << j);
                }
            }
        }

        return (spriteToSprite, spriteToBg);
    }
}

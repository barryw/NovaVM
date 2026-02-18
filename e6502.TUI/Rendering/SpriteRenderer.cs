using e6502.TUI.Hardware;

namespace e6502.TUI.Rendering;

public struct SpritePixel
{
    public int ScreenX;
    public int ScreenY;
    public byte Color;
}

public static class SpriteRenderer
{
    // Returns all visible pixels for a single sprite in block-pixel coordinates.
    public static List<SpritePixel> GetSpritePixels(VirtualGraphicsController vgc, int index)
    {
        var (x, y, color, enabled, shapeIdx, flags, _) = vgc.GetSpriteState(index);

        if (!enabled)
            return [];

        ReadOnlySpan<byte> shape = vgc.GetSpriteShape(shapeIdx);
        bool xFlip = (flags & 0x80) != 0;
        bool yFlip = (flags & 0x40) != 0;

        var pixels = new List<SpritePixel>(256);

        for (int row = 0; row < 16; row++)
        {
            int srcRow = yFlip ? 15 - row : row;
            byte hi = shape[srcRow * 2];
            byte lo = shape[srcRow * 2 + 1];
            int bits = (hi << 8) | lo;

            for (int col = 0; col < 16; col++)
            {
                int srcCol = xFlip ? 15 - col : col;
                if ((bits & (0x8000 >> srcCol)) != 0)
                {
                    pixels.Add(new SpritePixel
                    {
                        ScreenX = x + col,
                        ScreenY = y + row,
                        Color   = color,
                    });
                }
            }
        }

        return pixels;
    }

    // Returns sprite-sprite and sprite-background collision bitmasks.
    public static (ushort spriteToSprite, ushort spriteToBg) DetectCollisions(VirtualGraphicsController vgc)
    {
        // Build pixel sets for all enabled sprites.
        var pixelSets = new HashSet<(int x, int y)>?[VgcConstants.MaxSprites];
        for (int i = 0; i < VgcConstants.MaxSprites; i++)
        {
            var (_, _, _, enabled, _, _, _) = vgc.GetSpriteState(i);
            if (!enabled)
                continue;

            var pixels = GetSpritePixels(vgc, i);
            var set = new HashSet<(int x, int y)>(pixels.Count);
            foreach (var p in pixels)
                set.Add((p.ScreenX, p.ScreenY));
            pixelSets[i] = set;
        }

        ushort spriteToSprite = 0;
        ushort spriteToBg     = 0;

        for (int i = 0; i < VgcConstants.MaxSprites; i++)
        {
            if (pixelSets[i] is null)
                continue;

            // Sprite-background collision: any pixel over a non-zero gfx pixel.
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

            // Sprite-sprite collision: check against all higher-indexed sprites.
            for (int j = i + 1; j < VgcConstants.MaxSprites; j++)
            {
                if (pixelSets[j] is null)
                    continue;

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

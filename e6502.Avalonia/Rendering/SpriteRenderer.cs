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

                if (color == 0) continue; // transparent

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

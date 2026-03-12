using e6502.Avalonia.Hardware;

namespace e6502.Avalonia.Rendering;

/// <summary>
/// Rasterizes tile map scanlines for Mode 4 rendering.
/// </summary>
public static class TileRenderer
{
    /// <summary>
    /// Samples tile pixels for one scanline into two output buffers (behind/front priority).
    /// Output buffers contain raw 4-bit color indices (0 = transparent/not drawn).
    /// Resolved BGRA values are written to the corresponding palette output buffers.
    /// </summary>
    public static void RasterizeScanline(
        int scanlineY,
        TileRenderState tiles,
        Span<uint> tileLine,
        Span<byte> tileOpaque)
    {
        tileLine.Clear();
        tileOpaque.Clear();

        int scrollX = tiles.ScrollX;
        int scrollY = tiles.ScrollY;
        int tileW = tiles.TileWidth;
        int tileH = tiles.TileHeight;
        int mapW = tiles.MapWidthPixels;
        int mapH = tiles.MapHeightPixels;
        int screenW = VgcConstants.NametableCols * tileW;  // one NT width in pixels
        int screenH = VgcConstants.NametableRows * tileH;  // one NT height in pixels
        byte transColor = tiles.TransColor;

        int worldY = (scanlineY + scrollY) % mapH;
        if (worldY < 0) worldY += mapH;

        int ntRow = worldY / screenH;
        int localY = worldY % screenH;
        int tileRow = localY / tileH;
        int pixelY = localY % tileH;

        for (int x = 0; x < VgcConstants.GfxWidth; x++)
        {
            int worldX = (x + scrollX) % mapW;
            if (worldX < 0) worldX += mapW;

            int ntCol = worldX / screenW;
            int localX = worldX % screenW;
            int tileCol = localX / tileW;
            int pixelX = localX % tileW;

            int nt = tiles.ResolveNametable(ntCol, ntRow);
            int ntIndex = tileRow * VgcConstants.NametableCols + tileCol;
            byte tileNum = tiles.Nametables[nt][ntIndex];
            byte attr = tiles.AttrTables[nt][ntIndex];

            // Apply flips
            int sampleX = (attr & VgcConstants.TileAttrHFlip) != 0 ? (tileW - 1 - pixelX) : pixelX;
            int sampleY = (attr & VgcConstants.TileAttrVFlip) != 0 ? (tileH - 1 - pixelY) : pixelY;

            // Read pixel from tile data (packed nibbles, high nibble first)
            int tileOffset = tileNum * tiles.TileByteSize + sampleY * tiles.TileBytesPerRow + sampleX / 2;
            byte packed = tiles.TileData[tileOffset];
            byte colorIndex = (sampleX % 2 == 0)
                ? (byte)((packed >> 4) & 0x0F)
                : (byte)(packed & 0x0F);

            if (colorIndex == transColor) continue;

            // Resolve through sub-palette
            int subPalette = attr & VgcConstants.TileAttrPalMask;
            int palIndex = subPalette * VgcConstants.TilePaletteColors + colorIndex;

            tileLine[x] = tiles.Palette[palIndex];
            // Encode priority in opaque byte: 1=behind between-sprites, 2=in front of between-sprites
            tileOpaque[x] = (byte)(((attr & VgcConstants.TileAttrPriority) != 0) ? 2 : 1);
        }
    }
}

using e6502.Avalonia.Hardware;

namespace e6502.Avalonia.Rendering;

/// <summary>
/// Immutable snapshot of tile engine state for the renderer.
/// Taken once per frame to ensure consistent rendering even if the 6502 modifies tile data mid-frame.
/// </summary>
public class TileRenderState
{
    public readonly bool IsSize16;
    public readonly int MirrorMode;
    public readonly byte TransColor;
    public readonly int ScrollX;
    public readonly int ScrollY;

    // Tile definitions (max 32KB for 16x16 mode)
    public readonly byte[] TileData;

    // 4 nametables, each 1000 bytes
    public readonly byte[][] Nametables;

    // 4 attribute tables, each 1000 bytes
    public readonly byte[][] AttrTables;

    // Pre-computed BGRA palette: 256 entries (16 sub-palettes × 16 colors)
    public readonly uint[] Palette;

    // Derived dimensions
    public readonly int TileWidth;
    public readonly int TileHeight;
    public readonly int TileBytesPerRow;   // bytes per pixel row within a tile
    public readonly int TileByteSize;      // bytes per tile definition
    public readonly int MapWidthPixels;    // total logical map width in pixels
    public readonly int MapHeightPixels;   // total logical map height in pixels

    private TileRenderState(
        bool isSize16, int mirrorMode, byte transColor,
        int scrollX, int scrollY,
        byte[] tileData,
        byte[][] nametables, byte[][] attrTables,
        uint[] palette)
    {
        IsSize16 = isSize16;
        MirrorMode = mirrorMode;
        TransColor = transColor;
        ScrollX = scrollX;
        ScrollY = scrollY;
        TileData = tileData;
        Nametables = nametables;
        AttrTables = attrTables;
        Palette = palette;

        TileWidth = isSize16 ? 16 : 8;
        TileHeight = isSize16 ? 16 : 8;
        TileBytesPerRow = TileWidth / 2;  // 4-bit packed nibbles
        TileByteSize = isSize16 ? VgcConstants.TileSize16 : VgcConstants.TileSize8;

        // Map dimensions depend on mirror mode
        int ntCols = mirrorMode switch
        {
            VgcConstants.TileMirrorH => 1,      // horizontal mirror: 1 unique column
            VgcConstants.TileMirrorSingle => 1,  // single screen
            _ => 2                                // vertical mirror or four-screen: 2 columns
        };
        int ntRows = mirrorMode switch
        {
            VgcConstants.TileMirrorV => 1,       // vertical mirror: 1 unique row
            VgcConstants.TileMirrorSingle => 1,   // single screen
            _ => 2                                 // horizontal mirror or four-screen: 2 rows
        };
        MapWidthPixels = ntCols * VgcConstants.NametableCols * TileWidth;
        MapHeightPixels = ntRows * VgcConstants.NametableRows * TileHeight;
    }

    public static TileRenderState FromVgc(VirtualGraphicsController vgc)
    {
        bool isSize16 = vgc.IsTileSize16;
        int mirrorMode = vgc.GetTileMirrorMode();
        byte transColor = vgc.GetTileTransColor();
        int scrollX = vgc.GetTileScrollX();
        int scrollY = vgc.GetTileScrollY();

        // Snapshot tile definitions
        int tileDataSize = isSize16 ? VgcConstants.TileRamSize16 : VgcConstants.TileRamSize16; // always copy full buffer
        var tileData = new byte[tileDataSize];
        vgc.SnapshotTileData(tileData);

        // Snapshot nametables and attribute tables
        var nametables = new byte[VgcConstants.NametableCount][];
        var attrTables = new byte[VgcConstants.NametableCount][];
        for (int i = 0; i < VgcConstants.NametableCount; i++)
        {
            nametables[i] = new byte[VgcConstants.NametableSize];
            attrTables[i] = new byte[VgcConstants.NametableSize];
            vgc.CopyNametable(i, nametables[i]);
            vgc.CopyAttrTable(i, attrTables[i]);
        }

        // Snapshot palette RAM and convert to BGRA
        var palRam = new byte[VgcConstants.TilePaletteRamSize];
        vgc.CopyTilePaletteRam(palRam);
        var palette = new uint[VgcConstants.TilePaletteCount * VgcConstants.TilePaletteColors];
        for (int i = 0; i < palette.Length; i++)
        {
            int offset = i * 3;
            byte r = palRam[offset];
            byte g = palRam[offset + 1];
            byte b = palRam[offset + 2];
            palette[i] = 0xFF000000u | ((uint)r << 16) | ((uint)g << 8) | b;
        }

        return new TileRenderState(
            isSize16, mirrorMode, transColor,
            scrollX, scrollY,
            tileData, nametables, attrTables, palette);
    }

    /// <summary>
    /// Resolves which nametable index to use for a given grid position in the 2x2 layout.
    /// </summary>
    public int ResolveNametable(int ntCol, int ntRow)
    {
        int raw = ntRow * 2 + ntCol;
        return MirrorMode switch
        {
            VgcConstants.TileMirrorH => raw switch     // NT0=NT1, NT2=NT3
            {
                1 => 0,
                3 => 2,
                _ => raw
            },
            VgcConstants.TileMirrorV => raw switch     // NT0=NT2, NT1=NT3
            {
                2 => 0,
                3 => 1,
                _ => raw
            },
            VgcConstants.TileMirrorSingle => 0,
            _ => raw  // four-screen
        };
    }
}

namespace e6502.Avalonia.Rendering;

public class BitmapFont
{
    private readonly byte[] _data; // 2048 bytes (256 chars x 8 rows)
    public const int GlyphWidth = 8;
    public const int GlyphHeight = 8;
    public const int GlyphCount = 256;

    public BitmapFont(byte[] fontData)
    {
        if (fontData.Length != GlyphCount * GlyphHeight)
            throw new ArgumentException($"Font data must be {GlyphCount * GlyphHeight} bytes");
        _data = fontData;
    }

    public static BitmapFont LoadFromFile(string path) =>
        new(File.ReadAllBytes(path));

    /// <summary>
    /// Returns true if the pixel at (x, y) in the given glyph is set.
    /// x: 0-7 (0=leftmost), y: 0-7 (0=topmost).
    /// </summary>
    public bool GetPixel(byte charCode, int x, int y)
    {
        byte row = _data[charCode * GlyphHeight + y];
        return (row & (0x80 >> x)) != 0;
    }

    /// <summary>Returns the raw byte for one row of a glyph.</summary>
    public byte GetRow(byte charCode, int y) =>
        _data[charCode * GlyphHeight + y];
}

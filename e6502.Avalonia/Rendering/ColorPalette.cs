using Avalonia.Media;

namespace e6502.Avalonia.Rendering;

public static class ColorPalette
{
    private static readonly Color[] _c64Palette =
    [
        Color.FromRgb(0,   0,   0),     // 0  Black
        Color.FromRgb(255, 255, 255),    // 1  White
        Color.FromRgb(136, 0,   0),      // 2  Red
        Color.FromRgb(170, 255, 238),    // 3  Cyan
        Color.FromRgb(204, 68,  204),    // 4  Purple
        Color.FromRgb(0,   204, 85),     // 5  Green
        Color.FromRgb(0,   0,   170),    // 6  Blue
        Color.FromRgb(238, 238, 119),    // 7  Yellow
        Color.FromRgb(221, 136, 85),     // 8  Orange
        Color.FromRgb(102, 68,  0),      // 9  Brown
        Color.FromRgb(255, 119, 119),    // 10 Light Red
        Color.FromRgb(51,  51,  51),     // 11 Grey Dark
        Color.FromRgb(119, 119, 119),    // 12 Grey Med
        Color.FromRgb(170, 255, 102),    // 13 Light Green
        Color.FromRgb(0,   136, 255),    // 14 Light Blue
        Color.FromRgb(187, 187, 187),    // 15 Grey Light
    ];

    private static readonly Color[] _egaPalette =
    [
        Color.FromRgb(0,   0,   0),      // 0  Black
        Color.FromRgb(0,   0,   170),    // 1  Blue
        Color.FromRgb(0,   170, 0),      // 2  Green
        Color.FromRgb(0,   170, 170),    // 3  Cyan
        Color.FromRgb(170, 0,   0),      // 4  Red
        Color.FromRgb(170, 0,   170),    // 5  Magenta
        Color.FromRgb(170, 85,  0),      // 6  Brown
        Color.FromRgb(170, 170, 170),    // 7  Light Grey
        Color.FromRgb(85,  85,  85),     // 8  Dark Grey
        Color.FromRgb(85,  85,  255),    // 9  Bright Blue
        Color.FromRgb(85,  255, 85),     // 10 Bright Green
        Color.FromRgb(85,  255, 255),    // 11 Bright Cyan
        Color.FromRgb(255, 85,  85),     // 12 Bright Red
        Color.FromRgb(255, 85,  255),    // 13 Bright Magenta
        Color.FromRgb(255, 255, 85),     // 14 Yellow
        Color.FromRgb(255, 255, 255),    // 15 White
    ];

    private static readonly uint[] _c64BgraPalette = BuildBgraPalette(_c64Palette);
    private static readonly uint[] _egaBgraPalette = BuildBgraPalette(_egaPalette);

    /// <summary>Maps a 4-bit color index to an RGBA Color.</summary>
    public static Color Get(int index) => Get(index, 0);

    /// <summary>Maps a 4-bit color index to an RGBA Color in the selected palette.</summary>
    public static Color Get(int index, byte paletteMode) => GetPalette(paletteMode)[index & 0x0F];

    /// <summary>Returns a 32-bit BGRA pixel value for direct bitmap writing.</summary>
    public static uint GetBgra(int index) => GetBgra(index, 0);

    /// <summary>Returns a 32-bit BGRA pixel value for direct bitmap writing.</summary>
    public static uint GetBgra(int index, byte paletteMode) =>
        GetBgraPalette(paletteMode)[index & 0x0F];

    public static ReadOnlySpan<uint> GetBgraPalette(byte paletteMode) =>
        (paletteMode & 0x01) != 0 ? _egaBgraPalette : _c64BgraPalette;

    private static Color[] GetPalette(byte paletteMode) =>
        (paletteMode & 0x01) != 0 ? _egaPalette : _c64Palette;

    private static uint[] BuildBgraPalette(Color[] palette)
    {
        var bgra = new uint[palette.Length];
        for (int i = 0; i < palette.Length; i++)
        {
            var c = palette[i];
            bgra[i] = (uint)((c.A << 24) | (c.R << 16) | (c.G << 8) | c.B);
        }

        return bgra;
    }
}

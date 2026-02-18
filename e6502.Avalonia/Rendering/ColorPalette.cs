using Avalonia.Media;

namespace e6502.Avalonia.Rendering;

public static class ColorPalette
{
    private static readonly Color[] _palette =
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

    /// <summary>Maps a 4-bit color index to an RGBA Color.</summary>
    public static Color Get(int index) => _palette[index & 0x0F];

    /// <summary>Returns a 32-bit BGRA pixel value for direct bitmap writing.</summary>
    public static uint GetBgra(int index)
    {
        var c = _palette[index & 0x0F];
        return (uint)((c.A << 24) | (c.R << 16) | (c.G << 8) | c.B);
    }
}

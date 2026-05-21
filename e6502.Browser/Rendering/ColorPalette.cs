namespace e6502.Avalonia.Rendering;

public readonly struct RgbColor
{
    public RgbColor(byte r, byte g, byte b)
    {
        R = r;
        G = g;
        B = b;
    }

    public byte R { get; }
    public byte G { get; }
    public byte B { get; }
}

public static class ColorPalette
{
    private static readonly RgbColor[] _c64Palette =
    [
        new(0,   0,   0),
        new(255, 255, 255),
        new(136, 0,   0),
        new(170, 255, 238),
        new(204, 68,  204),
        new(0,   204, 85),
        new(0,   0,   170),
        new(238, 238, 119),
        new(221, 136, 85),
        new(102, 68,  0),
        new(255, 119, 119),
        new(51,  51,  51),
        new(119, 119, 119),
        new(170, 255, 102),
        new(0,   136, 255),
        new(187, 187, 187),
    ];

    private static readonly RgbColor[] _egaPalette =
    [
        new(0,   0,   0),
        new(0,   0,   170),
        new(0,   170, 0),
        new(0,   170, 170),
        new(170, 0,   0),
        new(170, 0,   170),
        new(170, 85,  0),
        new(170, 170, 170),
        new(85,  85,  85),
        new(85,  85,  255),
        new(85,  255, 85),
        new(85,  255, 255),
        new(255, 85,  85),
        new(255, 85,  255),
        new(255, 255, 85),
        new(255, 255, 255),
    ];

    public static RgbColor Get(int index) => Get(index, 0);

    public static RgbColor Get(int index, byte paletteMode) =>
        ((paletteMode & 0x01) != 0 ? _egaPalette : _c64Palette)[index & 0x0F];
}

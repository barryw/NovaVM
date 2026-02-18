using Terminal.Gui;

namespace e6502.TUI.Rendering;

public static class ColorPalette
{
    private static readonly Color[] _palette =
    [
        new Color(0,   0,   0,   255), // 0  Black
        new Color(255, 255, 255, 255), // 1  White
        new Color(136, 0,   0,   255), // 2  Red
        new Color(170, 255, 238, 255), // 3  Cyan
        new Color(204, 68,  204, 255), // 4  Purple
        new Color(0,   204, 85,  255), // 5  Green
        new Color(0,   0,   170, 255), // 6  Blue
        new Color(238, 238, 119, 255), // 7  Yellow
        new Color(221, 136, 85,  255), // 8  Orange
        new Color(102, 68,  0,   255), // 9  Brown
        new Color(255, 119, 119, 255), // 10 Light Red
        new Color(51,  51,  51,  255), // 11 Grey Dark
        new Color(119, 119, 119, 255), // 12 Grey Med
        new Color(170, 255, 102, 255), // 13 Light Green
        new Color(0,   136, 255, 255), // 14 Light Blue
        new Color(187, 187, 187, 255), // 15 Grey Light
    ];

    public static Color Get(int index) => _palette[index & 0x0F];
}

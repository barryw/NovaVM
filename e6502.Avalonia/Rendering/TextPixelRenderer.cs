using e6502.Avalonia.Hardware;

namespace e6502.Avalonia.Rendering;

internal static class TextPixelRenderer
{
    public static bool TrySample(
        VirtualGraphicsController vgc,
        BitmapFont font,
        int px,
        int py,
        byte mode,
        int scrollX,
        int scrollY,
        byte bgColor,
        int fontIndex,
        bool flashVisible,
        int cursorX,
        int cursorY,
        bool cursorEnabled,
        out byte colorIndex)
    {
        int srcPx = Wrap640(px + (scrollX << 1));
        int srcPy = Wrap400(py + (scrollY << 1));

        int col = srcPx / BitmapFont.GlyphWidth;
        int row = srcPy / (BitmapFont.GlyphHeight * 2);

        byte ch = vgc.GetScreenChar(col, row);
        byte colorAttr = vgc.GetScreenColor(col, row);
        byte textAttr = vgc.GetScreenTextAttr(col, row);

        byte fgColor = (byte)(colorAttr & 0x0F);
        byte cellBgColor = (byte)((colorAttr >> 4) & 0x0F);

        bool isCursor = cursorEnabled && col == cursorX && row == cursorY;
        if (isCursor)
            (fgColor, cellBgColor) = (cellBgColor, fgColor);

        int glyphX = srcPx % BitmapFont.GlyphWidth;
        int glyphY = (srcPy % (BitmapFont.GlyphHeight * 2)) / 2;
        byte rowBits = font.GetRow(fontIndex, ch, glyphY);
        bool set = (rowBits & (0x80 >> glyphX)) != 0;
        if ((textAttr & VgcConstants.TextAttrFlash) != 0 && !flashVisible)
            set = false;

        if (mode == 2 && !set && !isCursor && cellBgColor == (bgColor & 0x0F))
        {
            colorIndex = 0;
            return false;
        }

        colorIndex = set ? fgColor : cellBgColor;
        return true;
    }

    private static int Wrap640(int value)
    {
        if (value >= VgcConstants.CanvasWidth) value -= VgcConstants.CanvasWidth;
        return value;
    }

    private static int Wrap400(int value)
    {
        if (value >= VgcConstants.CanvasHeight) value -= VgcConstants.CanvasHeight;
        if (value >= VgcConstants.CanvasHeight) value -= VgcConstants.CanvasHeight;
        return value;
    }
}

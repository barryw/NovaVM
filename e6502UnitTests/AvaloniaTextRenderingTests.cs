using e6502.Avalonia.Hardware;
using e6502.Avalonia.Rendering;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class AvaloniaTextRenderingTests
{
    [TestMethod]
    public void TextPixelRenderer_UsesPackedReverseColors()
    {
        var vgc = new VirtualGraphicsController();
        var font = SinglePixelAFont();
        WriteTextCell(vgc, 0, (byte)'A', colorAttr: 0x24, textAttr: 0);

        Assert.IsTrue(TrySample(vgc, font, x: 0, y: 0, flashVisible: true, out byte fg));
        Assert.AreEqual(4, fg, "Set glyph pixels should use the low-nibble foreground color.");

        Assert.IsTrue(TrySample(vgc, font, x: 1, y: 0, flashVisible: true, out byte bg));
        Assert.AreEqual(2, bg, "Unset glyph pixels should use the high-nibble background color.");
    }

    [TestMethod]
    public void TextPixelRenderer_FlashHiddenDrawsCellBackground()
    {
        var vgc = new VirtualGraphicsController();
        var font = SinglePixelAFont();
        WriteTextCell(vgc, 0, (byte)'A', colorAttr: 0x42, textAttr: VgcConstants.TextAttrFlash);

        Assert.IsTrue(TrySample(vgc, font, x: 0, y: 0, flashVisible: true, out byte visible));
        Assert.AreEqual(2, visible);

        Assert.IsTrue(TrySample(vgc, font, x: 0, y: 0, flashVisible: false, out byte hidden));
        Assert.AreEqual(4, hidden);
    }

    [TestMethod]
    public void TextPixelRenderer_Mode2TreatsMatchingCellBackgroundAsTransparent()
    {
        var vgc = new VirtualGraphicsController();
        var font = SinglePixelAFont();
        WriteTextCell(vgc, 0, (byte)'A', colorAttr: 0x42, textAttr: 0);

        bool opaque = TextPixelRenderer.TrySample(
            vgc,
            font,
            px: 1,
            py: 0,
            mode: 2,
            scrollX: 0,
            scrollY: 0,
            bgColor: 4,
            fontIndex: 0,
            flashVisible: true,
            cursorX: 10,
            cursorY: 10,
            cursorEnabled: false,
            out _);

        Assert.IsFalse(opaque);
    }

    [TestMethod]
    public void TextPixelRenderer_SamplesAllFiftyRows()
    {
        var vgc = new VirtualGraphicsController();
        var font = SinglePixelAFont();
        WriteTextCell(vgc, 24 * VgcConstants.ScreenCols, (byte)'A', colorAttr: 0x12, textAttr: 0);
        WriteTextCell(vgc, 49 * VgcConstants.ScreenCols, (byte)'A', colorAttr: 0x34, textAttr: 0);

        Assert.IsTrue(TrySample(vgc, font, x: 0, y: 49 * BitmapFont.GlyphHeight, flashVisible: true, out byte row49));
        Assert.AreEqual(4, row49);
    }

    private static BitmapFont SinglePixelAFont()
    {
        var fontData = new byte[BitmapFont.FontDataSize];
        fontData[(byte)'A' * BitmapFont.GlyphHeight] = 0x80;
        return new BitmapFont(fontData);
    }

    private static void WriteTextCell(VirtualGraphicsController vgc, int offset, byte ch, byte colorAttr, byte textAttr)
    {
        Assert.IsTrue(vgc.TryWriteMemorySpace(VgcConstants.MemSpaceScreen, offset, ch));
        Assert.IsTrue(vgc.TryWriteMemorySpace(VgcConstants.MemSpaceColor, offset, colorAttr));
        Assert.IsTrue(vgc.TryWriteMemorySpace(VgcConstants.MemSpaceTextAttr, offset, textAttr));
    }

    private static bool TrySample(
        VirtualGraphicsController vgc,
        BitmapFont font,
        int x,
        int y,
        bool flashVisible,
        out byte colorIndex) =>
        TextPixelRenderer.TrySample(
            vgc,
            font,
            x,
            y,
            mode: 0,
            scrollX: 0,
            scrollY: 0,
            bgColor: 0,
            fontIndex: 0,
            flashVisible: flashVisible,
            cursorX: 10,
            cursorY: 10,
            cursorEnabled: false,
            out colorIndex);
}

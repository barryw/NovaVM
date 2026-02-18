using e6502.TUI.Rendering;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class BlockGraphicsTests
{
    private byte[] _bmp = null!;

    [TestInitialize]
    public void Setup() =>
        _bmp = new byte[BlockGraphics.Width * BlockGraphics.Height];

    private byte At(int x, int y) => _bmp[y * BlockGraphics.Width + x];

    // -------------------------------------------------------------------------
    // Plot
    // -------------------------------------------------------------------------

    [TestMethod]
    public void Plot_SetsPixel()
    {
        BlockGraphics.Plot(_bmp, 10, 5, 3);
        Assert.AreEqual(3, At(10, 5));
    }

    [TestMethod]
    public void Plot_OutOfBounds_Left_DoesNotThrow()
    {
        BlockGraphics.Plot(_bmp, -1, 0, 1);
        // no exception — all zeros
        Assert.AreEqual(0, At(0, 0));
    }

    [TestMethod]
    public void Plot_OutOfBounds_Right_DoesNotThrow()
    {
        BlockGraphics.Plot(_bmp, BlockGraphics.Width, 0, 1);
        Assert.AreEqual(0, At(BlockGraphics.Width - 1, 0));
    }

    [TestMethod]
    public void Plot_OutOfBounds_Top_DoesNotThrow()
    {
        BlockGraphics.Plot(_bmp, 0, -1, 1);
        Assert.AreEqual(0, At(0, 0));
    }

    [TestMethod]
    public void Plot_OutOfBounds_Bottom_DoesNotThrow()
    {
        BlockGraphics.Plot(_bmp, 0, BlockGraphics.Height, 1);
        Assert.AreEqual(0, At(0, BlockGraphics.Height - 1));
    }

    // -------------------------------------------------------------------------
    // Clear
    // -------------------------------------------------------------------------

    [TestMethod]
    public void Clear_ZerosBitmap()
    {
        BlockGraphics.Plot(_bmp, 0,  0,  7);
        BlockGraphics.Plot(_bmp, 10, 10, 3);
        BlockGraphics.Clear(_bmp);

        for (int i = 0; i < _bmp.Length; i++)
            Assert.AreEqual(0, _bmp[i], $"byte at index {i} not zero after Clear");
    }

    // -------------------------------------------------------------------------
    // Line — horizontal
    // -------------------------------------------------------------------------

    [TestMethod]
    public void Line_Horizontal_DrawsAllPixels()
    {
        BlockGraphics.Line(_bmp, 5, 10, 20, 10, 2);

        for (int x = 5; x <= 20; x++)
            Assert.AreEqual(2, At(x, 10), $"pixel ({x},10) not set");
    }

    // -------------------------------------------------------------------------
    // Line — vertical
    // -------------------------------------------------------------------------

    [TestMethod]
    public void Line_Vertical_DrawsAllPixels()
    {
        BlockGraphics.Line(_bmp, 30, 3, 30, 18, 4);

        for (int y = 3; y <= 18; y++)
            Assert.AreEqual(4, At(30, y), $"pixel (30,{y}) not set");
    }

    // -------------------------------------------------------------------------
    // Line — diagonal
    // -------------------------------------------------------------------------

    [TestMethod]
    public void Line_Diagonal_DrawsPixels()
    {
        BlockGraphics.Line(_bmp, 0, 0, 9, 9, 5);

        // Bresenham 45-degree line sets the diagonal
        for (int i = 0; i <= 9; i++)
            Assert.AreEqual(5, At(i, i), $"diagonal pixel ({i},{i}) not set");
    }

    [TestMethod]
    public void Line_SinglePoint_DrawsOnePixel()
    {
        BlockGraphics.Line(_bmp, 7, 7, 7, 7, 6);
        Assert.AreEqual(6, At(7, 7));
    }

    // -------------------------------------------------------------------------
    // Circle
    // -------------------------------------------------------------------------

    [TestMethod]
    public void Circle_DrawsSomePixels()
    {
        BlockGraphics.Circle(_bmp, 40, 20, 10, 9);

        // The circle should have set at least a few pixels
        int count = 0;
        foreach (byte b in _bmp)
            if (b == 9) count++;
        Assert.IsTrue(count > 0, "Circle drew no pixels");
    }

    [TestMethod]
    public void Circle_CardinalPoints_AreSet()
    {
        // For radius 10 centered at (40,20), the cardinal pixels must be set
        BlockGraphics.Circle(_bmp, 40, 20, 10, 1);

        Assert.AreEqual(1, At(40, 10), "top");
        Assert.AreEqual(1, At(40, 30), "bottom");
        Assert.AreEqual(1, At(50, 20), "right");
        Assert.AreEqual(1, At(30, 20), "left");
    }

    [TestMethod]
    public void Circle_ZeroRadius_DrawsOnePixel()
    {
        BlockGraphics.Circle(_bmp, 20, 15, 0, 3);
        Assert.AreEqual(3, At(20, 15));
    }

    // -------------------------------------------------------------------------
    // Rect
    // -------------------------------------------------------------------------

    [TestMethod]
    public void Rect_DrawsFourSides()
    {
        BlockGraphics.Rect(_bmp, 5, 5, 15, 15, 7);

        // Top and bottom edges
        for (int x = 5; x <= 15; x++)
        {
            Assert.AreEqual(7, At(x, 5),  $"top edge ({x},5)");
            Assert.AreEqual(7, At(x, 15), $"bottom edge ({x},15)");
        }

        // Left and right edges
        for (int y = 5; y <= 15; y++)
        {
            Assert.AreEqual(7, At(5,  y), $"left edge (5,{y})");
            Assert.AreEqual(7, At(15, y), $"right edge (15,{y})");
        }
    }

    [TestMethod]
    public void Rect_InteriorIsEmpty()
    {
        BlockGraphics.Rect(_bmp, 5, 5, 15, 15, 7);

        // Interior pixels must remain 0
        for (int y = 6; y < 15; y++)
        for (int x = 6; x < 15; x++)
            Assert.AreEqual(0, At(x, y), $"interior pixel ({x},{y}) should be empty");
    }

    // -------------------------------------------------------------------------
    // Fill
    // -------------------------------------------------------------------------

    [TestMethod]
    public void Fill_FillsAllPixelsInRect()
    {
        BlockGraphics.Fill(_bmp, 10, 10, 20, 20, 5);

        for (int y = 10; y <= 20; y++)
        for (int x = 10; x <= 20; x++)
            Assert.AreEqual(5, At(x, y), $"pixel ({x},{y}) not filled");
    }

    [TestMethod]
    public void Fill_PixelsOutsideRectAreZero()
    {
        BlockGraphics.Fill(_bmp, 10, 10, 20, 20, 5);

        Assert.AreEqual(0, At(9,  10));
        Assert.AreEqual(0, At(21, 10));
        Assert.AreEqual(0, At(10, 9));
        Assert.AreEqual(0, At(10, 21));
    }

    [TestMethod]
    public void Fill_ReversedCoordinates_StillFills()
    {
        // x1 < x0 and y1 < y0 — should normalise and fill correctly
        BlockGraphics.Fill(_bmp, 20, 20, 10, 10, 3);

        for (int y = 10; y <= 20; y++)
        for (int x = 10; x <= 20; x++)
            Assert.AreEqual(3, At(x, y), $"pixel ({x},{y}) not filled");
    }
}

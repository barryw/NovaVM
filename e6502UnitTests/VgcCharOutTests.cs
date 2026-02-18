using e6502.TUI.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class VgcCharOutTests
{
    private VirtualGraphicsController _vgc = null!;

    [TestInitialize]
    public void Setup() => _vgc = new VirtualGraphicsController();

    // Helper: write CHAROUT
    private void CharOut(byte ch) => _vgc.Write(VgcConstants.RegCharOut, ch);

    // -------------------------------------------------------------------------
    // Printable character
    // -------------------------------------------------------------------------

    [TestMethod]
    public void PrintableChar_WritesToScreenRamAtCursorPosition()
    {
        _vgc.Write(VgcConstants.RegCursorX, 5);
        _vgc.Write(VgcConstants.RegCursorY, 3);
        CharOut(0x41); // 'A'
        Assert.AreEqual(0x41, _vgc.GetScreenChar(5, 3));
    }

    [TestMethod]
    public void PrintableChar_AdvancesCursorRight()
    {
        _vgc.Write(VgcConstants.RegCursorX, 10);
        _vgc.Write(VgcConstants.RegCursorY, 2);
        CharOut(0x42); // 'B'
        Assert.AreEqual(11, _vgc.GetCursorX());
        Assert.AreEqual(2, _vgc.GetCursorY());
    }

    [TestMethod]
    public void PrintableChar_SetsColorRamToFgCol()
    {
        _vgc.Write(VgcConstants.RegFgCol, 7);
        _vgc.Write(VgcConstants.RegCursorX, 0);
        _vgc.Write(VgcConstants.RegCursorY, 0);
        CharOut(0x43); // 'C'
        Assert.AreEqual(7, _vgc.GetScreenColor(0, 0));
    }

    [TestMethod]
    public void PrintableChar_WrapsAtEndOfLine_ColBecomesZero_RowAdvances()
    {
        _vgc.Write(VgcConstants.RegCursorX, 79);
        _vgc.Write(VgcConstants.RegCursorY, 5);
        CharOut(0x58); // 'X'
        Assert.AreEqual(0, _vgc.GetCursorX());
        Assert.AreEqual(6, _vgc.GetCursorY());
    }

    [TestMethod]
    public void PrintableChar_WrapAtLastRow_TriggersScroll_CursorStaysOnRow24()
    {
        _vgc.Write(VgcConstants.RegCursorX, 79);
        _vgc.Write(VgcConstants.RegCursorY, 24);
        CharOut(0x59); // 'Y'
        Assert.AreEqual(0, _vgc.GetCursorX());
        Assert.AreEqual(24, _vgc.GetCursorY());
    }

    // -------------------------------------------------------------------------
    // Carriage Return ($0D)
    // -------------------------------------------------------------------------

    [TestMethod]
    public void CR_MovesCursorToColZero_NextRow()
    {
        _vgc.Write(VgcConstants.RegCursorX, 40);
        _vgc.Write(VgcConstants.RegCursorY, 10);
        CharOut(0x0D);
        Assert.AreEqual(0, _vgc.GetCursorX());
        Assert.AreEqual(11, _vgc.GetCursorY());
    }

    [TestMethod]
    public void CR_AtLastRow_TriggersScroll_CursorStaysOnRow24()
    {
        _vgc.Write(VgcConstants.RegCursorX, 20);
        _vgc.Write(VgcConstants.RegCursorY, 24);
        CharOut(0x0D);
        Assert.AreEqual(0, _vgc.GetCursorX());
        Assert.AreEqual(24, _vgc.GetCursorY());
    }

    // -------------------------------------------------------------------------
    // Line Feed ($0A)
    // -------------------------------------------------------------------------

    [TestMethod]
    public void LF_AdvancesRowByOne()
    {
        _vgc.Write(VgcConstants.RegCursorX, 5);
        _vgc.Write(VgcConstants.RegCursorY, 7);
        CharOut(0x0A);
        Assert.AreEqual(5, _vgc.GetCursorX());
        Assert.AreEqual(8, _vgc.GetCursorY());
    }

    [TestMethod]
    public void LF_AtLastRow_TriggersScroll_CursorStaysOnRow24()
    {
        _vgc.Write(VgcConstants.RegCursorX, 0);
        _vgc.Write(VgcConstants.RegCursorY, 24);
        CharOut(0x0A);
        Assert.AreEqual(24, _vgc.GetCursorY());
    }

    // -------------------------------------------------------------------------
    // Backspace ($08)
    // -------------------------------------------------------------------------

    [TestMethod]
    public void BS_MovesLeft_ErasesCharAtNewPosition()
    {
        _vgc.Write(VgcConstants.RegCursorX, 5);
        _vgc.Write(VgcConstants.RegCursorY, 2);
        // Put something at col 4
        _vgc.Write(VgcConstants.CharRamBase + 2 * VgcConstants.ScreenCols + 4, 0x41);
        CharOut(0x08);
        Assert.AreEqual(4, _vgc.GetCursorX());
        Assert.AreEqual(0x20, _vgc.GetScreenChar(4, 2)); // erased
    }

    [TestMethod]
    public void BS_AtColZero_DoesNotMoveLeft()
    {
        _vgc.Write(VgcConstants.RegCursorX, 0);
        _vgc.Write(VgcConstants.RegCursorY, 0);
        CharOut(0x08);
        Assert.AreEqual(0, _vgc.GetCursorX());
    }

    // -------------------------------------------------------------------------
    // Form Feed ($0C)
    // -------------------------------------------------------------------------

    [TestMethod]
    public void FF_ClearsScreenToSpaces()
    {
        // Write something to screen
        _vgc.Write(VgcConstants.CharRamBase + 100, 0x41);
        CharOut(0x0C);
        for (int row = 0; row < VgcConstants.ScreenRows; row++)
            for (int col = 0; col < VgcConstants.ScreenCols; col++)
                Assert.AreEqual(0x20, _vgc.GetScreenChar(col, row),
                    $"Expected space at ({col},{row}) after FF");
    }

    [TestMethod]
    public void FF_HomesCursor()
    {
        _vgc.Write(VgcConstants.RegCursorX, 30);
        _vgc.Write(VgcConstants.RegCursorY, 15);
        CharOut(0x0C);
        Assert.AreEqual(0, _vgc.GetCursorX());
        Assert.AreEqual(0, _vgc.GetCursorY());
    }

    [TestMethod]
    public void FF_SetsColorRamToCurrentFgCol()
    {
        _vgc.Write(VgcConstants.RegFgCol, 5);
        CharOut(0x0C);
        for (int row = 0; row < VgcConstants.ScreenRows; row++)
            for (int col = 0; col < VgcConstants.ScreenCols; col++)
                Assert.AreEqual(5, _vgc.GetScreenColor(col, row),
                    $"Expected fg color at ({col},{row}) after FF");
    }

    // -------------------------------------------------------------------------
    // HOME ($13)
    // -------------------------------------------------------------------------

    [TestMethod]
    public void HOME_MovesCursorToZeroZero()
    {
        _vgc.Write(VgcConstants.RegCursorX, 50);
        _vgc.Write(VgcConstants.RegCursorY, 20);
        CharOut(0x13);
        Assert.AreEqual(0, _vgc.GetCursorX());
        Assert.AreEqual(0, _vgc.GetCursorY());
    }

    // -------------------------------------------------------------------------
    // ScrollUp
    // -------------------------------------------------------------------------

    [TestMethod]
    public void ScrollUp_FirstRowContentsWhatWasSecondRow()
    {
        // Write 'A' across row 1
        for (int col = 0; col < VgcConstants.ScreenCols; col++)
            _vgc.Write((ushort)(VgcConstants.CharRamBase + 1 * VgcConstants.ScreenCols + col), 0x41);

        // Trigger scroll via CR on last row
        _vgc.Write(VgcConstants.RegCursorX, 0);
        _vgc.Write(VgcConstants.RegCursorY, 24);
        CharOut(0x0D);

        // Row 0 should now have 'A'
        for (int col = 0; col < VgcConstants.ScreenCols; col++)
            Assert.AreEqual(0x41, _vgc.GetScreenChar(col, 0),
                $"Expected 'A' at ({col},0) after scroll");
    }

    [TestMethod]
    public void ScrollUp_LastRowClearedToSpaces()
    {
        // Fill row 24 with 'Z'
        for (int col = 0; col < VgcConstants.ScreenCols; col++)
            _vgc.Write((ushort)(VgcConstants.CharRamBase + 24 * VgcConstants.ScreenCols + col), 0x5A);

        // Trigger scroll
        _vgc.Write(VgcConstants.RegCursorX, 0);
        _vgc.Write(VgcConstants.RegCursorY, 24);
        CharOut(0x0A); // LF

        for (int col = 0; col < VgcConstants.ScreenCols; col++)
            Assert.AreEqual(0x20, _vgc.GetScreenChar(col, 24),
                $"Expected space at ({col},24) after scroll");
    }

    [TestMethod]
    public void ScrollUp_LastRowColorSetToBgCol()
    {
        _vgc.Write(VgcConstants.RegBgCol, 3);
        _vgc.Write(VgcConstants.RegCursorX, 0);
        _vgc.Write(VgcConstants.RegCursorY, 24);
        CharOut(0x0A);

        for (int col = 0; col < VgcConstants.ScreenCols; col++)
            Assert.AreEqual(3, _vgc.GetScreenColor(col, 24),
                $"Expected bg color at ({col},24) after scroll");
    }
}

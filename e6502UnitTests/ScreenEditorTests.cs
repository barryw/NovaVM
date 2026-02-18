using e6502.TUI.Hardware;
using e6502.TUI.Rendering;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class ScreenEditorTests
{
    private VirtualGraphicsController _vgc = null!;
    private ScreenEditor _editor = null!;

    [TestInitialize]
    public void Setup()
    {
        _vgc = new VirtualGraphicsController();
        _editor = new ScreenEditor(_vgc);
    }

    // -------------------------------------------------------------------------
    // Cursor movement
    // -------------------------------------------------------------------------

    [TestMethod]
    public void CursorRight_IncrementsX()
    {
        _editor.CursorRight();
        Assert.AreEqual(1, _vgc.GetCursorX());
    }

    [TestMethod]
    public void CursorLeft_DecrementsX()
    {
        _vgc.Write(VgcConstants.RegCursorX, 5);
        _editor.CursorLeft();
        Assert.AreEqual(4, _vgc.GetCursorX());
    }

    [TestMethod]
    public void CursorLeft_StopsAtZero()
    {
        // Already at 0
        _editor.CursorLeft();
        Assert.AreEqual(0, _vgc.GetCursorX());
    }

    [TestMethod]
    public void CursorDown_IncrementsY()
    {
        _editor.CursorDown();
        Assert.AreEqual(1, _vgc.GetCursorY());
    }

    [TestMethod]
    public void CursorDown_StopsAt24()
    {
        _vgc.Write(VgcConstants.RegCursorY, 24);
        _editor.CursorDown();
        Assert.AreEqual(24, _vgc.GetCursorY());
    }

    [TestMethod]
    public void CursorUp_DecrementsY()
    {
        _vgc.Write(VgcConstants.RegCursorY, 5);
        _editor.CursorUp();
        Assert.AreEqual(4, _vgc.GetCursorY());
    }

    [TestMethod]
    public void CursorUp_StopsAtZero()
    {
        // Already at 0
        _editor.CursorUp();
        Assert.AreEqual(0, _vgc.GetCursorY());
    }

    // -------------------------------------------------------------------------
    // ReadLineFromScreen
    // -------------------------------------------------------------------------

    [TestMethod]
    public void ReadLineFromScreen_ReadsCorrectText()
    {
        // Write "HELLO" at row 0
        _vgc.Write(VgcConstants.RegCursorY, 0);
        byte[] text = "HELLO"u8.ToArray();
        for (int i = 0; i < text.Length; i++)
            _vgc.Write((ushort)(VgcConstants.CharRamBase + i), text[i]);

        string result = _editor.ReadLineFromScreen();
        Assert.AreEqual("HELLO", result);
    }

    [TestMethod]
    public void ReadLineFromScreen_TrimsTrailingSpaces()
    {
        // Write "OK" at row 0 — rest of row is spaces (default)
        _vgc.Write(VgcConstants.RegCursorY, 0);
        _vgc.Write(VgcConstants.CharRamBase + 0, (byte)'O');
        _vgc.Write(VgcConstants.CharRamBase + 1, (byte)'K');

        string result = _editor.ReadLineFromScreen();
        Assert.AreEqual("OK", result);
    }

    // -------------------------------------------------------------------------
    // HandleReturn
    // -------------------------------------------------------------------------

    [TestMethod]
    public void HandleReturn_QueuesLinePlusCr()
    {
        // Write "HI" at row 0
        _vgc.Write(VgcConstants.RegCursorY, 0);
        _vgc.Write(VgcConstants.CharRamBase + 0, (byte)'H');
        _vgc.Write(VgcConstants.CharRamBase + 1, (byte)'I');

        _editor.HandleReturn();

        Assert.IsTrue(_editor.HasQueuedInput);
        Assert.AreEqual((byte)'H', _editor.DequeueInput());
        Assert.AreEqual((byte)'I', _editor.DequeueInput());
        Assert.AreEqual(0x0D, _editor.DequeueInput());
        Assert.IsFalse(_editor.HasQueuedInput);
    }

    // -------------------------------------------------------------------------
    // DequeueInput
    // -------------------------------------------------------------------------

    [TestMethod]
    public void DequeueInput_ReturnsBytesInOrder()
    {
        _vgc.Write(VgcConstants.RegCursorY, 0);
        _vgc.Write(VgcConstants.CharRamBase + 0, (byte)'A');
        _vgc.Write(VgcConstants.CharRamBase + 1, (byte)'B');
        _vgc.Write(VgcConstants.CharRamBase + 2, (byte)'C');

        _editor.HandleReturn();

        Assert.AreEqual((byte)'A', _editor.DequeueInput());
        Assert.AreEqual((byte)'B', _editor.DequeueInput());
        Assert.AreEqual((byte)'C', _editor.DequeueInput());
        Assert.AreEqual(0x0D, _editor.DequeueInput());
    }

    [TestMethod]
    public void DequeueInput_ReturnsZeroWhenEmpty()
    {
        Assert.IsFalse(_editor.HasQueuedInput);
        Assert.AreEqual(0, _editor.DequeueInput());
    }

    // -------------------------------------------------------------------------
    // HandleTypedChar
    // -------------------------------------------------------------------------

    [TestMethod]
    public void HandleTypedChar_WritesToScreenRam()
    {
        // Cursor at 0,0
        _editor.HandleTypedChar((byte)'X');

        byte written = _vgc.GetScreenChar(0, 0);
        Assert.AreEqual((byte)'X', written);
    }

    [TestMethod]
    public void HandleTypedChar_AdvancesCursor()
    {
        _editor.HandleTypedChar((byte)'A');
        Assert.AreEqual(1, _vgc.GetCursorX());
    }
}

using e6502.TUI.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class VgcTests
{
    private VirtualGraphicsController _vgc = null!;

    [TestInitialize]
    public void Setup() => _vgc = new VirtualGraphicsController();

    // -------------------------------------------------------------------------
    // Initial state
    // -------------------------------------------------------------------------

    [TestMethod]
    public void InitialMode_IsZero()
    {
        Assert.AreEqual(0, _vgc.GetMode());
    }

    [TestMethod]
    public void InitialForegroundColor_IsOne()
    {
        byte fg = _vgc.Read(VgcConstants.RegFgCol);
        Assert.AreEqual(1, fg);
    }

    [TestMethod]
    public void InitialScreenRam_IsSpaces()
    {
        for (int row = 0; row < VgcConstants.ScreenRows; row++)
            for (int col = 0; col < VgcConstants.ScreenCols; col++)
                Assert.AreEqual(0x20, _vgc.GetScreenChar(col, row),
                    $"Expected space at ({col},{row})");
    }

    [TestMethod]
    public void InitialColorRam_IsOne()
    {
        for (int row = 0; row < VgcConstants.ScreenRows; row++)
            for (int col = 0; col < VgcConstants.ScreenCols; col++)
                Assert.AreEqual(1, _vgc.GetScreenColor(col, row),
                    $"Expected color 1 at ({col},{row})");
    }

    // -------------------------------------------------------------------------
    // Register read/write
    // -------------------------------------------------------------------------

    [TestMethod]
    public void Register_WriteRead_RoundTrips()
    {
        _vgc.Write(VgcConstants.RegBgCol, 0x05);
        Assert.AreEqual(0x05, _vgc.Read(VgcConstants.RegBgCol));
        Assert.AreEqual(0x05, _vgc.GetBgColor());
    }

    [TestMethod]
    public void Register_CursorXY_WriteRead()
    {
        _vgc.Write(VgcConstants.RegCursorX, 42);
        _vgc.Write(VgcConstants.RegCursorY, 12);
        Assert.AreEqual(42, _vgc.GetCursorX());
        Assert.AreEqual(12, _vgc.GetCursorY());
    }

    // -------------------------------------------------------------------------
    // Screen RAM
    // -------------------------------------------------------------------------

    [TestMethod]
    public void ScreenRam_WriteRead_RoundTrips()
    {
        ushort addr = VgcConstants.CharRamBase + 100;
        _vgc.Write(addr, 0x41); // 'A'
        Assert.AreEqual(0x41, _vgc.Read(addr));
    }

    [TestMethod]
    public void ScreenRam_GetScreenChar_ReflectsWrite()
    {
        _vgc.Write(VgcConstants.CharRamBase + 5, 0x42); // col=5, row=0
        Assert.AreEqual(0x42, _vgc.GetScreenChar(5, 0));
    }

    // -------------------------------------------------------------------------
    // Color RAM
    // -------------------------------------------------------------------------

    [TestMethod]
    public void ColorRam_WriteRead_RoundTrips()
    {
        ushort addr = VgcConstants.ColorRamBase + 10;
        _vgc.Write(addr, 0x07);
        Assert.AreEqual(0x07, _vgc.Read(addr));
    }

    [TestMethod]
    public void ColorRam_GetScreenColor_ReflectsWrite()
    {
        _vgc.Write(VgcConstants.ColorRamBase + 3, 0x0E); // col=3, row=0
        Assert.AreEqual(0x0E, _vgc.GetScreenColor(3, 0));
    }

    // -------------------------------------------------------------------------
    // Sprite shape data
    // -------------------------------------------------------------------------

    [TestMethod]
    public void SpriteShape_WriteRead_RoundTrips()
    {
        ushort addr = VgcConstants.SpriteShapeBase + 4;
        _vgc.Write(addr, 0xAB);
        Assert.AreEqual(0xAB, _vgc.Read(addr));
    }

    [TestMethod]
    public void GetSpriteShape_ReflectsWrite()
    {
        // Write 4 bytes into sprite 1 shape
        int shapeOffset = 1 * VgcConstants.SpriteShapeSize;
        for (int i = 0; i < 4; i++)
            _vgc.Write((ushort)(VgcConstants.SpriteShapeBase + shapeOffset + i), (byte)(0x10 + i));

        var shape = _vgc.GetSpriteShape(1);
        for (int i = 0; i < 4; i++)
            Assert.AreEqual((byte)(0x10 + i), shape[i]);
    }

    // -------------------------------------------------------------------------
    // Collision registers clear on read
    // -------------------------------------------------------------------------

    [TestMethod]
    public void CollisionSprite_ClearsOnRead()
    {
        // Force a value via internal write path (write to RegColSt through bus)
        // We can't write normally since writes go to _regs directly
        // Write then read — writes should store, first read returns & clears
        _vgc.Write(VgcConstants.RegColSt, 0xFF);
        byte first = _vgc.Read(VgcConstants.RegColSt);
        byte second = _vgc.Read(VgcConstants.RegColSt);
        Assert.AreEqual(0xFF, first);
        Assert.AreEqual(0x00, second);
    }

    [TestMethod]
    public void CollisionBackground_ClearsOnRead()
    {
        _vgc.Write(VgcConstants.RegColBg, 0x55);
        byte first = _vgc.Read(VgcConstants.RegColBg);
        byte second = _vgc.Read(VgcConstants.RegColBg);
        Assert.AreEqual(0x55, first);
        Assert.AreEqual(0x00, second);
    }

    // -------------------------------------------------------------------------
    // CHARIN clears on read
    // -------------------------------------------------------------------------

    [TestMethod]
    public void CharIn_ClearsOnRead()
    {
        _vgc.FeedInput(0x41); // 'A'
        byte first = _vgc.Read(VgcConstants.RegCharIn);
        byte second = _vgc.Read(VgcConstants.RegCharIn);
        Assert.AreEqual(0x41, first);
        Assert.AreEqual(0x00, second);
    }

    [TestMethod]
    public void CharIn_WriteAlsoSetsValue()
    {
        _vgc.Write(VgcConstants.RegCharIn, 0x42);
        Assert.AreEqual(0x42, _vgc.Read(VgcConstants.RegCharIn));
    }

    // -------------------------------------------------------------------------
    // STATUS is read-only
    // -------------------------------------------------------------------------

    [TestMethod]
    public void Status_WritesAreIgnored()
    {
        _vgc.SetStatus(0x03);
        _vgc.Write(VgcConstants.RegStatus, 0xFF); // must be ignored
        Assert.AreEqual(0x03, _vgc.Read(VgcConstants.RegStatus));
    }

    // -------------------------------------------------------------------------
    // OwnsAddress
    // -------------------------------------------------------------------------

    [TestMethod]
    public void OwnsAddress_VgcBase_ReturnsTrue()
    {
        Assert.IsTrue(_vgc.OwnsAddress(VgcConstants.VgcBase));
    }

    [TestMethod]
    public void OwnsAddress_ColorRamEnd_ReturnsTrue()
    {
        Assert.IsTrue(_vgc.OwnsAddress(VgcConstants.ColorRamEnd));
    }

    [TestMethod]
    public void OwnsAddress_ScreenRamMid_ReturnsTrue()
    {
        Assert.IsTrue(_vgc.OwnsAddress(0xA800));
    }

    [TestMethod]
    public void OwnsAddress_BelowRange_ReturnsFalse()
    {
        Assert.IsFalse(_vgc.OwnsAddress(0x9FFF));
    }

    [TestMethod]
    public void OwnsAddress_AboveRange_ReturnsFalse()
    {
        Assert.IsFalse(_vgc.OwnsAddress(0xB3A0));
    }

    [TestMethod]
    public void OwnsAddress_Zero_ReturnsFalse()
    {
        Assert.IsFalse(_vgc.OwnsAddress(0x0000));
    }

    // -------------------------------------------------------------------------
    // Sprite state
    // -------------------------------------------------------------------------

    [TestMethod]
    public void GetSpriteState_ReflectsSpriteRegWrites()
    {
        // Sprite 2: X=300 (0x12C), Y=100, color=7, priority=1, shape=3
        int sprBase = VgcConstants.SpriteReg(2, 0);
        _vgc.Write((ushort)(sprBase + VgcConstants.SprOffXLow),      0x2C); // low byte of 300
        _vgc.Write((ushort)(sprBase + VgcConstants.SprOffXHighFlags), 0x01); // high bit = 1
        _vgc.Write((ushort)(sprBase + VgcConstants.SprOffY),         100);
        _vgc.Write((ushort)(sprBase + VgcConstants.SprOffColor),      7);
        _vgc.Write((ushort)(sprBase + VgcConstants.SprOffPriority),   1);
        _vgc.Write((ushort)(sprBase + VgcConstants.SprOffShape),      3);

        // Enable sprite 2
        _vgc.Write(VgcConstants.RegSpriteEn, 0x04); // bit 2

        var (x, y, color, enabled, shapeIdx, flags, priority) = _vgc.GetSpriteState(2);
        Assert.AreEqual(300, x);
        Assert.AreEqual(100, y);
        Assert.AreEqual(7,   color);
        Assert.IsTrue(enabled);
        Assert.AreEqual(3,   shapeIdx);
        Assert.AreEqual(1,   priority);
    }
}

using e6502.Avalonia.Hardware;
using e6502.Avalonia.Rendering;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class AvaloniaVgcTests
{
    private VirtualGraphicsController _vgc = null!;

    [TestInitialize]
    public void Setup() => _vgc = new VirtualGraphicsController();

    // -- Initial state --------------------------------------------------------

    [TestMethod]
    public void InitialMode_IsZero() =>
        Assert.AreEqual(0, _vgc.GetMode());

    [TestMethod]
    public void InitialFgColor_IsOne() =>
        Assert.AreEqual(1, _vgc.Read(VgcConstants.RegFgCol));

    [TestMethod]
    public void InitialScreenRam_IsSpaces()
    {
        for (int row = 0; row < VgcConstants.ScreenRows; row++)
            for (int col = 0; col < VgcConstants.ScreenCols; col++)
                Assert.AreEqual(0x20, _vgc.GetScreenChar(col, row));
    }

    // -- Command register: Graphics -------------------------------------------

    [TestMethod]
    public void GfxPlot_ViaCommandRegister_SetsPixel()
    {
        // GCOLOR 5
        _vgc.Write(VgcConstants.RegP0, 5);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdGcolor);

        // PLOT at (10, 5)
        _vgc.Write(VgcConstants.RegP0, 10);  // x low
        _vgc.Write(VgcConstants.RegP1, 0);   // x high
        _vgc.Write(VgcConstants.RegP2, 5);   // y low
        _vgc.Write(VgcConstants.RegP3, 0);   // y high
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdPlot);

        Assert.IsTrue(_vgc.GetGfxPixel(10, 5));
        Assert.AreEqual(5, _vgc.GetGfxPixelColor(10, 5));
    }

    // -- Sprite commands ------------------------------------------------------

    [TestMethod]
    public void SprDef_SetsSinglePixel()
    {
        // SDEF: sprite 0, x=8, y=0, color=5
        _vgc.Write(VgcConstants.RegP0, 0);   // sprite index
        _vgc.Write(VgcConstants.RegP1, 8);   // x
        _vgc.Write(VgcConstants.RegP2, 0);   // y
        _vgc.Write(VgcConstants.RegP3, 5);   // color
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprDef);

        var shape = _vgc.GetSpriteShape(0);
        // Row 0, pixel 8 -> byte 4 (8/2), high nibble (8 is even)
        Assert.AreEqual(0x50, shape[4]); // color 5 in high nibble
    }

    [TestMethod]
    public void SprRow_DefinesFullRow()
    {
        // SROW: sprite 0, row 0, 8 bytes of data
        _vgc.Write(VgcConstants.RegP0, 0);    // sprite index
        _vgc.Write(VgcConstants.RegP1, 0);    // row number
        _vgc.Write(VgcConstants.RegP2, 0x12); // pixels 0-1: colors 1,2
        _vgc.Write(VgcConstants.RegP3, 0x34); // pixels 2-3: colors 3,4
        _vgc.Write(VgcConstants.RegP4, 0x56); // pixels 4-5: colors 5,6
        _vgc.Write(VgcConstants.RegP5, 0x78); // pixels 6-7: colors 7,8
        _vgc.Write(VgcConstants.RegP6, 0x9A); // pixels 8-9: colors 9,10
        _vgc.Write(VgcConstants.RegP7, 0xBC); // pixels 10-11: colors 11,12
        _vgc.Write(VgcConstants.RegP8, 0xDE); // pixels 12-13: colors 13,14
        _vgc.Write(VgcConstants.RegP9, 0xF0); // pixels 14-15: colors 15,0
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprRow);

        var shape = _vgc.GetSpriteShape(0);
        Assert.AreEqual(0x12, shape[0]);
        Assert.AreEqual(0x34, shape[1]);
        Assert.AreEqual(0xF0, shape[7]);
    }

    [TestMethod]
    public void SprClr_ClearsShapeToTransparent()
    {
        // Define a pixel first
        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegP1, 0);
        _vgc.Write(VgcConstants.RegP2, 0);
        _vgc.Write(VgcConstants.RegP3, 5);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprDef);

        // Clear
        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprClr);

        var shape = _vgc.GetSpriteShape(0);
        for (int i = 0; i < VgcConstants.SpriteShapeSize; i++)
            Assert.AreEqual(0, shape[i]);
    }

    [TestMethod]
    public void SprCopy_CopiesShapeData()
    {
        // Define row in sprite 0
        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegP1, 0);
        _vgc.Write(VgcConstants.RegP2, 0xAB);
        _vgc.Write(VgcConstants.RegP3, 0xCD);
        _vgc.Write(VgcConstants.RegP4, 0);
        _vgc.Write(VgcConstants.RegP5, 0);
        _vgc.Write(VgcConstants.RegP6, 0);
        _vgc.Write(VgcConstants.RegP7, 0);
        _vgc.Write(VgcConstants.RegP8, 0);
        _vgc.Write(VgcConstants.RegP9, 0);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprRow);

        // Copy sprite 0 -> sprite 3
        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegP1, 3);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprCopy);

        var shape3 = _vgc.GetSpriteShape(3);
        Assert.AreEqual(0xAB, shape3[0]);
        Assert.AreEqual(0xCD, shape3[1]);
    }

    [TestMethod]
    public void SprPos_SetsPosition()
    {
        // Enable sprite first
        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprEna);

        // SPOS: sprite 0, x=300 (0x012C), y=25
        _vgc.Write(VgcConstants.RegP0, 0);    // sprite
        _vgc.Write(VgcConstants.RegP1, 0x2C); // x low
        _vgc.Write(VgcConstants.RegP2, 0x01); // x high
        _vgc.Write(VgcConstants.RegP3, 25);   // y
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprPos);

        var state = _vgc.GetSpriteState(0);
        Assert.AreEqual(300, state.x);
        Assert.AreEqual(25, state.y);
    }

    [TestMethod]
    public void SprEna_EnablesSprite()
    {
        _vgc.Write(VgcConstants.RegP0, 5);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprEna);

        Assert.IsTrue(_vgc.GetSpriteState(5).enabled);
    }

    [TestMethod]
    public void SprDis_DisablesSprite()
    {
        _vgc.Write(VgcConstants.RegP0, 5);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprEna);

        _vgc.Write(VgcConstants.RegP0, 5);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprDis);

        Assert.IsFalse(_vgc.GetSpriteState(5).enabled);
    }

    [TestMethod]
    public void SprFlip_SetsFlags()
    {
        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprEna);

        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegP1, 0x03); // both X and Y flip
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprFlip);

        var state = _vgc.GetSpriteState(0);
        Assert.AreEqual(0x03, state.flags);
    }

    [TestMethod]
    public void SprPri_SetsPriority()
    {
        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprEna);

        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegP1, VgcConstants.SpritePriBehindAll);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprPri);

        Assert.AreEqual(VgcConstants.SpritePriBehindAll, _vgc.GetSpriteState(0).priority);
    }

    [TestMethod]
    public void SpriteCount_ReflectsEnabledSprites()
    {
        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprEna);
        _vgc.Write(VgcConstants.RegP0, 3);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprEna);

        Assert.AreEqual(2, _vgc.Read(VgcConstants.RegSpriteCount));
    }

    // -- CharOut still works --------------------------------------------------

    [TestMethod]
    public void CharOut_WritesCharToScreen()
    {
        _vgc.Write(VgcConstants.RegCharOut, (byte)'A');
        Assert.AreEqual((byte)'A', _vgc.GetScreenChar(0, 0));
        Assert.AreEqual(1, _vgc.GetCursorX());
    }

    // -- Collision registers still clear on read ------------------------------

    [TestMethod]
    public void CollisionRegisters_ClearOnRead()
    {
        _vgc.SetCollisionRegisters(0xAB, 0xCD);
        byte ss = _vgc.Read(VgcConstants.RegColSt);
        byte sb = _vgc.Read(VgcConstants.RegColBg);
        Assert.AreEqual(0xAB, ss);
        Assert.AreEqual(0xCD, sb);
        Assert.AreEqual(0, _vgc.Read(VgcConstants.RegColSt));
        Assert.AreEqual(0, _vgc.Read(VgcConstants.RegColBg));
    }

    // -- OwnsAddress with new ranges ------------------------------------------

    [TestMethod]
    public void OwnsAddress_VgcBase_True() =>
        Assert.IsTrue(_vgc.OwnsAddress(VgcConstants.VgcBase));

    [TestMethod]
    public void OwnsAddress_ColorRamEnd_True() =>
        Assert.IsTrue(_vgc.OwnsAddress(VgcConstants.ColorRamEnd));

    [TestMethod]
    public void OwnsAddress_BelowRange_False() =>
        Assert.IsFalse(_vgc.OwnsAddress(0x9FFF));

    [TestMethod]
    public void OwnsAddress_FioBase_False() =>
        Assert.IsFalse(_vgc.OwnsAddress(VgcConstants.FioBase));

    [TestMethod]
    public void OwnsAddress_GapStart_False() =>
        Assert.IsFalse(_vgc.OwnsAddress(0xA020));

    [TestMethod]
    public void OwnsAddress_GapMiddle_False() =>
        Assert.IsFalse(_vgc.OwnsAddress(0xA500));

    // -- Default sprite priority ----------------------------------------------

    [TestMethod]
    public void DefaultSpritePriority_IsInFront()
    {
        // Enable sprite without setting priority -- should default to InFront (2)
        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprEna);

        Assert.AreEqual(VgcConstants.SpritePriInFront, _vgc.GetSpriteState(0).priority);
    }

    // -- HandleCharOut special characters -------------------------------------

    [TestMethod]
    public void CharOut_Backspace_MovesLeftAndClears()
    {
        // Write 'A' then backspace
        _vgc.Write(VgcConstants.RegCharOut, (byte)'A');
        Assert.AreEqual(1, _vgc.GetCursorX());

        _vgc.Write(VgcConstants.RegCharOut, 0x08); // BS
        Assert.AreEqual(0, _vgc.GetCursorX());
        Assert.AreEqual(0x20, _vgc.GetScreenChar(0, 0)); // cleared to space
    }

    [TestMethod]
    public void CharOut_LineFeed_AdvancesRow()
    {
        _vgc.Write(VgcConstants.RegCharOut, 0x0A); // LF
        Assert.AreEqual(0, _vgc.GetCursorX());
        Assert.AreEqual(1, _vgc.GetCursorY());
    }

    [TestMethod]
    public void CharOut_FormFeed_ClearsScreenAndHomesCursor()
    {
        // Put cursor somewhere and write a char
        _vgc.Write(VgcConstants.RegCharOut, (byte)'X');
        _vgc.Write(VgcConstants.RegCharOut, 0x0C); // FF

        Assert.AreEqual(0, _vgc.GetCursorX());
        Assert.AreEqual(0, _vgc.GetCursorY());
        Assert.AreEqual(0x20, _vgc.GetScreenChar(0, 0)); // screen cleared
    }

    [TestMethod]
    public void CharOut_CarriageReturn_MovesToCol0()
    {
        _vgc.Write(VgcConstants.RegCharOut, (byte)'A');
        _vgc.Write(VgcConstants.RegCharOut, (byte)'B');
        Assert.AreEqual(2, _vgc.GetCursorX());

        _vgc.Write(VgcConstants.RegCharOut, 0x0D); // CR
        Assert.AreEqual(0, _vgc.GetCursorX());
    }

    [TestMethod]
    public void CharOut_Home_CursorTo00()
    {
        // Move cursor down and right
        _vgc.Write(VgcConstants.RegCharOut, (byte)'A');
        _vgc.Write(VgcConstants.RegCharOut, 0x0A); // LF

        _vgc.Write(VgcConstants.RegCharOut, 0x13); // HOME
        Assert.AreEqual(0, _vgc.GetCursorX());
        Assert.AreEqual(0, _vgc.GetCursorY());
    }

    // -- Read-only registers --------------------------------------------------

    [TestMethod]
    public void WriteToRegStatus_IsIgnored()
    {
        byte before = _vgc.Read(VgcConstants.RegStatus);
        _vgc.Write(VgcConstants.RegStatus, 0xFF);
        Assert.AreEqual(before, _vgc.Read(VgcConstants.RegStatus));
    }
}

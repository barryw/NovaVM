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
    public void SprPos_DecodesSignedCoordinates()
    {
        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprEna);

        // SPOS: sprite 0, x=-16 (0xFFF0), y=-8 (0xFFF8)
        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegP1, 0xF0); // x low
        _vgc.Write(VgcConstants.RegP2, 0xFF); // x high
        _vgc.Write(VgcConstants.RegP3, 0xF8); // y low
        _vgc.Write(VgcConstants.RegP4, 0xFF); // y high
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprPos);

        var state = _vgc.GetSpriteState(0);
        Assert.AreEqual(-16, state.x);
        Assert.AreEqual(-8, state.y);
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

    // -- VGC memory I/O ports -------------------------------------------------

    [TestMethod]
    public void MemIo_CanWriteAndReadScreenRam()
    {
        _vgc.Write(VgcConstants.RegP0, VgcConstants.MemSpaceScreen); // space
        _vgc.Write(VgcConstants.RegP1, 0x34); // addr low
        _vgc.Write(VgcConstants.RegP2, 0x00); // addr high
        _vgc.Write(VgcConstants.RegP3, 0x41); // 'A'
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdMemWrite);

        Assert.AreEqual(0x41, _vgc.Read(VgcConstants.CharRamBase + 0x34));

        _vgc.Write(VgcConstants.RegP3, 0x00);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdMemRead);
        Assert.AreEqual(0x41, _vgc.Read(VgcConstants.RegP3));
    }

    [TestMethod]
    public void MemIo_AutoIncrement_WritesSequentialBytes()
    {
        _vgc.Write(VgcConstants.RegP0, VgcConstants.MemSpaceScreen); // space
        _vgc.Write(VgcConstants.RegP1, 0x00); // addr low
        _vgc.Write(VgcConstants.RegP2, 0x00); // addr high
        _vgc.Write(VgcConstants.RegP4, 0x01); // flags: auto-inc

        _vgc.Write(VgcConstants.RegP3, 0x31);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdMemWrite);
        _vgc.Write(VgcConstants.RegP3, 0x32);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdMemWrite);

        Assert.AreEqual(0x31, _vgc.Read(VgcConstants.CharRamBase + 0));
        Assert.AreEqual(0x32, _vgc.Read(VgcConstants.CharRamBase + 1));
        Assert.AreEqual(0x02, _vgc.Read(VgcConstants.RegP1)); // addr low advanced twice
        Assert.AreEqual(0x00, _vgc.Read(VgcConstants.RegP2)); // addr high unchanged
    }

    [TestMethod]
    public void MemIo_CanWriteGraphicsBitmap()
    {
        int x = 10;
        int y = 5;
        int linear = y * VgcConstants.GfxWidth + x;

        _vgc.Write(VgcConstants.RegP0, VgcConstants.MemSpaceGfx); // space
        _vgc.Write(VgcConstants.RegP1, (byte)(linear & 0xFF));
        _vgc.Write(VgcConstants.RegP2, (byte)((linear >> 8) & 0xFF));
        _vgc.Write(VgcConstants.RegP3, 0x07);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdMemWrite);

        Assert.AreEqual(0x07, _vgc.GetGfxPixelColor(x, y));
    }

    [TestMethod]
    public void MemIo_CanWriteSpriteShapeRam()
    {
        _vgc.Write(VgcConstants.RegP0, VgcConstants.MemSpaceSprite); // space
        _vgc.Write(VgcConstants.RegP1, 0x00); // addr low
        _vgc.Write(VgcConstants.RegP2, 0x00); // addr high
        _vgc.Write(VgcConstants.RegP3, 0xA5);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdMemWrite);

        var shape = _vgc.GetSpriteShape(0);
        Assert.AreEqual(0xA5, shape[0]);
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
    public void OwnsAddress_RegIrqCtrl_True() =>
        Assert.IsTrue(_vgc.OwnsAddress(VgcConstants.RegIrqCtrl));

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

    // -- Raster IRQ enable ----------------------------------------------------

    [TestMethod]
    public void Vgc_RasterIrqEnable_DefaultOff()
    {
        var vgc = new VirtualGraphicsController();
        Assert.IsFalse(vgc.IsRasterIrqEnabled);
    }

    [TestMethod]
    public void Vgc_RasterIrqEnable_WriteToRegister()
    {
        var vgc = new VirtualGraphicsController();
        vgc.Write(VgcConstants.RegIrqCtrl, 0x01);
        Assert.IsTrue(vgc.IsRasterIrqEnabled);
    }

    [TestMethod]
    public void Vgc_RasterIrqEnable_DisableClears()
    {
        var vgc = new VirtualGraphicsController();
        vgc.Write(VgcConstants.RegIrqCtrl, 0x01);
        vgc.Write(VgcConstants.RegIrqCtrl, 0x00);
        Assert.IsFalse(vgc.IsRasterIrqEnabled);
    }

    [TestMethod]
    public void ScrollRegisters_WriteAndRead()
    {
        _vgc.Write(VgcConstants.RegScrollX, 12);
        _vgc.Write(VgcConstants.RegScrollY, 34);

        Assert.AreEqual(12, _vgc.GetScrollX());
        Assert.AreEqual(34, _vgc.GetScrollY());
    }

    [TestMethod]
    public void Copper_AddEnableClear_Disable()
    {
        _vgc.Write(VgcConstants.RegP0, 40); // x low
        _vgc.Write(VgcConstants.RegP1, 0);  // x high
        _vgc.Write(VgcConstants.RegP2, 10); // y
        _vgc.Write(VgcConstants.RegP3, (byte)(VgcConstants.RegMode & 0xFF)); // reg low
        _vgc.Write(VgcConstants.RegP4, (byte)((VgcConstants.RegMode >> 8) & 0xFF)); // reg high
        _vgc.Write(VgcConstants.RegP5, 2); // value
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdCopperAdd);

        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdCopperEnable);
        Assert.IsTrue(_vgc.IsCopperEnabled);

        _vgc.IncrementFrameCounter(); // commit copper program
        var program = _vgc.GetCopperProgram();
        Assert.AreEqual(1, program.Length);
        Assert.AreEqual((ushort)(10 * VgcConstants.GfxWidth + 40), program[0].Position);
        Assert.AreEqual((byte)(VgcConstants.RegMode - VgcConstants.VgcBase), program[0].RegisterIndex);
        Assert.AreEqual(2, program[0].Value);

        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdCopperClear);
        _vgc.IncrementFrameCounter(); // commit clear
        Assert.AreEqual(0, _vgc.GetCopperProgram().Length);

        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdCopperDisable);
        Assert.IsFalse(_vgc.IsCopperEnabled);
    }

    [TestMethod]
    public void Copper_ListSwap_ActiveChangesAtVblank()
    {
        // Build events on list 1
        _vgc.Write(VgcConstants.RegP0, 1); // list index
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdCopperList);

        _vgc.Write(VgcConstants.RegP0, 10);
        _vgc.Write(VgcConstants.RegP1, 0);
        _vgc.Write(VgcConstants.RegP2, 5);
        _vgc.Write(VgcConstants.RegP3, (byte)(VgcConstants.RegBgCol - VgcConstants.VgcBase));
        _vgc.Write(VgcConstants.RegP4, 0);
        _vgc.Write(VgcConstants.RegP5, 7);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdCopperAdd);

        // USE list 1
        _vgc.Write(VgcConstants.RegP0, 1);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdCopperUse);

        // Before vblank, active is still list 0 (empty)
        Assert.AreEqual(0, _vgc.GetCopperProgram().Length);

        // Vblank: active swaps to list 1
        _vgc.IncrementFrameCounter();
        var program = _vgc.GetCopperProgram();
        Assert.AreEqual(1, program.Length);
        Assert.AreEqual(7, program[0].Value);
    }

    [TestMethod]
    public void Copper_AddToMultipleLists_EventsInCorrectLists()
    {
        // Add event to list 0 (default target)
        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegP1, 0);
        _vgc.Write(VgcConstants.RegP2, 10);
        _vgc.Write(VgcConstants.RegP3, (byte)(VgcConstants.RegMode - VgcConstants.VgcBase));
        _vgc.Write(VgcConstants.RegP4, 0);
        _vgc.Write(VgcConstants.RegP5, 1);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdCopperAdd);

        // Switch target to list 1
        _vgc.Write(VgcConstants.RegP0, 1);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdCopperList);

        // Add two events to list 1
        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegP1, 0);
        _vgc.Write(VgcConstants.RegP2, 20);
        _vgc.Write(VgcConstants.RegP3, (byte)(VgcConstants.RegBgCol - VgcConstants.VgcBase));
        _vgc.Write(VgcConstants.RegP4, 0);
        _vgc.Write(VgcConstants.RegP5, 3);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdCopperAdd);

        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegP1, 0);
        _vgc.Write(VgcConstants.RegP2, 40);
        _vgc.Write(VgcConstants.RegP3, (byte)(VgcConstants.RegBgCol - VgcConstants.VgcBase));
        _vgc.Write(VgcConstants.RegP4, 0);
        _vgc.Write(VgcConstants.RegP5, 5);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdCopperAdd);

        _vgc.IncrementFrameCounter();

        // Active is still list 0 — should have 1 event
        var prog0 = _vgc.GetCopperProgram();
        Assert.AreEqual(1, prog0.Length);
        Assert.AreEqual(1, prog0[0].Value);

        // Switch to list 1
        _vgc.Write(VgcConstants.RegP0, 1);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdCopperUse);
        _vgc.IncrementFrameCounter();

        var prog1 = _vgc.GetCopperProgram();
        Assert.AreEqual(2, prog1.Length);
    }

    [TestMethod]
    public void Copper_DefaultBehavior_BackwardCompatibleOnList0()
    {
        // No LIST command — default target is list 0, active is list 0
        _vgc.Write(VgcConstants.RegP0, 20);
        _vgc.Write(VgcConstants.RegP1, 0);
        _vgc.Write(VgcConstants.RegP2, 50);
        _vgc.Write(VgcConstants.RegP3, (byte)(VgcConstants.RegBgCol - VgcConstants.VgcBase));
        _vgc.Write(VgcConstants.RegP4, 0);
        _vgc.Write(VgcConstants.RegP5, 4);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdCopperAdd);

        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdCopperEnable);
        _vgc.IncrementFrameCounter();

        Assert.IsTrue(_vgc.IsCopperEnabled);
        var program = _vgc.GetCopperProgram();
        Assert.AreEqual(1, program.Length);
        Assert.AreEqual(4, program[0].Value);

        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdCopperClear);
        _vgc.IncrementFrameCounter();
        Assert.AreEqual(0, _vgc.GetCopperProgram().Length);
    }

    [TestMethod]
    public void Copper_ListEnd_ResetsTargetToActiveList()
    {
        // Set target to list 5
        _vgc.Write(VgcConstants.RegP0, 5);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdCopperList);

        // LIST END resets target back to active (0)
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdCopperListEnd);

        // ADD should go to list 0 now
        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegP1, 0);
        _vgc.Write(VgcConstants.RegP2, 10);
        _vgc.Write(VgcConstants.RegP3, (byte)(VgcConstants.RegBgCol - VgcConstants.VgcBase));
        _vgc.Write(VgcConstants.RegP4, 0);
        _vgc.Write(VgcConstants.RegP5, 9);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdCopperAdd);

        _vgc.IncrementFrameCounter();
        var program = _vgc.GetCopperProgram();
        Assert.AreEqual(1, program.Length);
        Assert.AreEqual(9, program[0].Value);
    }

    // -- Shape slot expansion ---------------------------------------------------

    [TestMethod]
    public void ShapeSlots_DefaultShapeIndexMatchesSpriteIndex()
    {
        for (int i = 0; i < VgcConstants.MaxSprites; i++)
            Assert.AreEqual(i, _vgc.GetSpriteShapeIndex(i),
                $"Sprite {i} should default to shape slot {i}");
    }

    [TestMethod]
    public void ShapeSlots_SetShapeIndex_ChangesShapeUsed()
    {
        // Write a pixel to shape slot 5
        _vgc.SetSpritePixelInSlot(5, 0, 0, 0x0A);

        // Point sprite 0 at slot 5
        _vgc.SetSpriteShapeIndex(0, 5);

        // GetSpriteShape(0) should now return slot 5's data
        var shape = _vgc.GetSpriteShape(0);
        Assert.AreEqual(0xA0, shape[0], "High nibble should be 0xA for color 10");
    }

    [TestMethod]
    public void ShapeSlots_CanAccessSlot255()
    {
        _vgc.SetSpritePixelInSlot(255, 0, 0, 0x07);
        _vgc.SetSpriteShapeIndex(0, 255);
        var shape = _vgc.GetSpriteShape(0);
        Assert.AreEqual(0x70, shape[0]);
    }

    [TestMethod]
    public void ShapeSlots_GetShapeBySlot_ReturnsCorrectData()
    {
        _vgc.SetSpritePixelInSlot(42, 3, 0, 0x0C);
        var shape = _vgc.GetSpriteShapeBySlot(42);
        // Pixel at x=3 (odd) -> low nibble of byte 1
        Assert.AreEqual(0x0C, shape[1] & 0x0F);
    }

    // -- Sprite register writes $A040-$A0BF -----------------------------------

    [TestMethod]
    public void SpriteReg_WritePosition_UpdatesSpriteState()
    {
        // Write X=300 (0x012C), Y=-10 (0xFFF6) to sprite 0 via registers
        _vgc.Write(VgcConstants.SpriteRegBase + 0, 0x2C);  // XLo
        _vgc.Write(VgcConstants.SpriteRegBase + 1, 0x01);  // XHi
        _vgc.Write(VgcConstants.SpriteRegBase + 2, 0xF6);  // YLo
        _vgc.Write(VgcConstants.SpriteRegBase + 3, 0xFF);  // YHi

        var state = _vgc.GetSpriteState(0);
        Assert.AreEqual(300, state.x);
        Assert.AreEqual(-10, state.y);
    }

    [TestMethod]
    public void SpriteReg_WriteShapeIndex_UpdatesShapePointer()
    {
        _vgc.Write(VgcConstants.SpriteRegBase + VgcConstants.SprRegShape, 42);
        Assert.AreEqual(42, _vgc.GetSpriteShapeIndex(0));
    }

    [TestMethod]
    public void SpriteReg_WriteFlags_UpdatesEnableAndFlip()
    {
        // Enable + xFlip + yFlip
        _vgc.Write(VgcConstants.SpriteRegBase + VgcConstants.SprRegFlags,
                   VgcConstants.SprFlagEnable | VgcConstants.SprFlagXFlip | VgcConstants.SprFlagYFlip);

        var state = _vgc.GetSpriteState(0);
        Assert.IsTrue(state.enabled);
        Assert.AreEqual(0x03, state.flags);  // both flip bits
    }

    [TestMethod]
    public void SpriteReg_WriteFlagsEnable_UpdatesSpriteCount()
    {
        _vgc.Write(VgcConstants.SpriteRegBase + VgcConstants.SprRegFlags, VgcConstants.SprFlagEnable);
        Assert.AreEqual(1, _vgc.Read(VgcConstants.RegSpriteCount));

        // Disable via flags
        _vgc.Write(VgcConstants.SpriteRegBase + VgcConstants.SprRegFlags, 0x00);
        Assert.AreEqual(0, _vgc.Read(VgcConstants.RegSpriteCount));
    }

    [TestMethod]
    public void SpriteReg_WritePriority_ClampedTo2()
    {
        // Default priority is 2 (SpritePriInFront), set to 0 first
        _vgc.SetSpritePriority(0, 0);
        Assert.AreEqual((byte)0, _vgc.GetSpriteState(0).priority, "Precondition: priority should be 0");

        // Write 5 via register — should clamp to 2
        _vgc.Write(VgcConstants.SpriteRegBase + VgcConstants.SprRegPriority, 5);
        Assert.AreEqual((byte)2, _vgc.GetSpriteState(0).priority, "Priority 5 should clamp to 2");
    }

    [TestMethod]
    public void SpriteReg_Sprite7_CorrectOffset()
    {
        int base7 = VgcConstants.SpriteRegBase + 7 * VgcConstants.SpriteRegStride;
        _vgc.Write(base7 + VgcConstants.SprRegXLo, 0x40);
        _vgc.Write(base7 + VgcConstants.SprRegFlags, VgcConstants.SprFlagEnable);

        var state = _vgc.GetSpriteState(7);
        Assert.AreEqual(0x40, state.x);
        Assert.IsTrue(state.enabled);
    }

    // -- Copper ---------------------------------------------------------------

    [TestMethod]
    public void Copper_Program_IsSortedByRasterPosition()
    {
        _vgc.Write(VgcConstants.RegP0, 30); // x low
        _vgc.Write(VgcConstants.RegP1, 0);  // x high
        _vgc.Write(VgcConstants.RegP2, 8);  // y
        _vgc.Write(VgcConstants.RegP3, (byte)(VgcConstants.RegBgCol - VgcConstants.VgcBase)); // register index
        _vgc.Write(VgcConstants.RegP4, 0);
        _vgc.Write(VgcConstants.RegP5, 6);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdCopperAdd);

        _vgc.Write(VgcConstants.RegP0, 5); // x low
        _vgc.Write(VgcConstants.RegP1, 0); // x high
        _vgc.Write(VgcConstants.RegP2, 2); // y
        _vgc.Write(VgcConstants.RegP3, (byte)(VgcConstants.RegMode - VgcConstants.VgcBase)); // register index
        _vgc.Write(VgcConstants.RegP4, 0);
        _vgc.Write(VgcConstants.RegP5, 1);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdCopperAdd);

        _vgc.IncrementFrameCounter(); // commit copper program
        var program = _vgc.GetCopperProgram();
        Assert.AreEqual(2, program.Length);
        Assert.IsTrue(program[0].Position < program[1].Position);
    }
}

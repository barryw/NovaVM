using e6502.TUI.Hardware;
using e6502.TUI.Rendering;
using KDS.e6502;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class IntegrationTests
{
    // -------------------------------------------------------------------------
    // Helpers
    // -------------------------------------------------------------------------

    private static CompositeBusDevice MakeBus() => new();

    /// <summary>
    /// Issues a GFX GCOLOR command through the bus to set the draw color,
    /// then issues a PLOT command at (x, y).
    /// </summary>
    private static void BusPlotPixel(CompositeBusDevice bus, int x, int y, byte color)
    {
        // GCOLOR: p0 = color (16-bit low byte = color, high byte = 0)
        bus.Write(VgcConstants.RegGfxP0, color);
        bus.Write(VgcConstants.RegGfxP1, 0);
        bus.Write(VgcConstants.RegGfxCmd, VgcConstants.GfxCmdGcolor);

        // PLOT: p0 = x (16-bit), p1 = y (16-bit)
        bus.Write(VgcConstants.RegGfxP0, (byte)(x & 0xFF));
        bus.Write(VgcConstants.RegGfxP1, (byte)((x >> 8) & 0xFF));
        bus.Write(VgcConstants.RegGfxP2, (byte)(y & 0xFF));
        bus.Write(VgcConstants.RegGfxP3, (byte)((y >> 8) & 0xFF));
        bus.Write(VgcConstants.RegGfxCmd, VgcConstants.GfxCmdPlot);
    }

    // -------------------------------------------------------------------------
    // 1. CPU boots with CompositeBus — ROM is loaded and CPU can execute
    // -------------------------------------------------------------------------

    [TestMethod]
    public void CpuBoot_WithCompositeBus_RomIsLoaded()
    {
        var bus = MakeBus();

        // ROM at $C000 must be non-zero (EhBASIC is loaded there)
        byte firstByte = bus.Read(VgcConstants.RomBase);
        Assert.AreNotEqual(0x00, firstByte,
            "EhBASIC ROM must be non-zero at $C000 after CompositeBusDevice construction.");
    }

    [TestMethod]
    public void CpuBoot_WithCompositeBus_CanExecuteFromRom()
    {
        var bus = MakeBus();
        var cpu = new Cpu(bus);

        // Boot directly into ROM and execute one instruction without crashing
        cpu.Boot(VgcConstants.RomBase);
        Assert.AreEqual(VgcConstants.RomBase, cpu.Pc,
            "CPU PC must be set to ROM base after Boot(ushort).");

        // ExecuteNext must not throw — ROM is readable
        cpu.ExecuteNext();
        Assert.AreNotEqual(VgcConstants.RomBase, cpu.Pc,
            "CPU PC must advance after executing the first ROM instruction.");
    }

    // -------------------------------------------------------------------------
    // 2. CharOut via bus produces screen content
    // -------------------------------------------------------------------------

    [TestMethod]
    public void CharOut_ViaBus_ProducesScreenContent()
    {
        var bus = MakeBus();

        // Cursor starts at (0, 0); write two characters
        bus.Write(VgcConstants.RegCharOut, (byte)'H');
        bus.Write(VgcConstants.RegCharOut, (byte)'i');

        Assert.AreEqual((byte)'H', bus.Vgc.GetScreenChar(0, 0),
            "First char 'H' must appear at col 0, row 0.");
        Assert.AreEqual((byte)'i', bus.Vgc.GetScreenChar(1, 0),
            "Second char 'i' must appear at col 1, row 0.");
    }

    // -------------------------------------------------------------------------
    // 3. CharIn round-trip: write, read, confirm read-clears
    // -------------------------------------------------------------------------

    [TestMethod]
    public void CharIn_ViaBus_RoundTrip_AndReadClears()
    {
        var bus = MakeBus();

        // Feed a key directly into the VGC
        bus.Vgc.FeedInput((byte)'Q');

        byte first  = bus.Read(VgcConstants.RegCharIn);
        byte second = bus.Read(VgcConstants.RegCharIn);

        Assert.AreEqual((byte)'Q', first,  "First read must return the fed key.");
        Assert.AreEqual(0x00,      second, "Second read must return 0 (cleared).");
    }

    // -------------------------------------------------------------------------
    // 4. GFX command via bus: set color then plot, verify via VGC accessor
    // -------------------------------------------------------------------------

    [TestMethod]
    public void GfxCommand_ViaBus_PlotPixel_IsVisible()
    {
        var bus = MakeBus();

        BusPlotPixel(bus, x: 10, y: 5, color: 7);

        Assert.IsTrue(bus.Vgc.GetGfxPixel(10, 5),
            "Plotted pixel at (10,5) must be set.");
        Assert.AreEqual(7, bus.Vgc.GetGfxPixelColor(10, 5),
            "Pixel color at (10,5) must be 7.");
    }

    [TestMethod]
    public void GfxCommand_ViaBus_UnplottedPixel_IsNotSet()
    {
        var bus = MakeBus();

        // Plot then unplot
        BusPlotPixel(bus, x: 3, y: 3, color: 5);
        Assert.IsTrue(bus.Vgc.GetGfxPixel(3, 3), "Pixel must be set before unplot.");

        bus.Write(VgcConstants.RegGfxP0, 3);
        bus.Write(VgcConstants.RegGfxP1, 0);
        bus.Write(VgcConstants.RegGfxP2, 3);
        bus.Write(VgcConstants.RegGfxP3, 0);
        bus.Write(VgcConstants.RegGfxCmd, VgcConstants.GfxCmdUnplot);

        Assert.IsFalse(bus.Vgc.GetGfxPixel(3, 3), "Pixel must be clear after unplot.");
    }

    // -------------------------------------------------------------------------
    // 5. Sprite enable via bus: define shape, enable sprite, check state
    // -------------------------------------------------------------------------

    [TestMethod]
    public void SpriteEnable_ViaBus_SpriteStateReflectsRegisters()
    {
        var bus = MakeBus();

        // Write sprite 0 registers: X=20, Y=30, color=5, priority=0, shapeIdx=0
        int sprBase = VgcConstants.SpriteRegsBase;
        bus.Write((ushort)(sprBase + VgcConstants.SprOffXLow),       20);
        bus.Write((ushort)(sprBase + VgcConstants.SprOffXHighFlags),  0);
        bus.Write((ushort)(sprBase + VgcConstants.SprOffY),          30);
        bus.Write((ushort)(sprBase + VgcConstants.SprOffColor),       5);
        bus.Write((ushort)(sprBase + VgcConstants.SprOffPriority),    0);
        bus.Write((ushort)(sprBase + VgcConstants.SprOffShape),       0);

        // Write a non-zero shape byte so shape data is present
        bus.Write(VgcConstants.SpriteShapeBase, 0x80);

        // Enable sprite 0 (bit 0 of RegSpriteEn)
        bus.Write(VgcConstants.RegSpriteEn, 0x01);

        var (x, y, color, enabled, shapeIdx, _, _) = bus.Vgc.GetSpriteState(0);

        Assert.AreEqual(20,   x,         "Sprite X must match written value.");
        Assert.AreEqual(30,   y,         "Sprite Y must match written value.");
        Assert.AreEqual(5,    color,     "Sprite color must match written value.");
        Assert.IsTrue(enabled,           "Sprite must be enabled after writing enable bit.");
        Assert.AreEqual(0,    shapeIdx,  "Sprite shape index must match written value.");
    }

    [TestMethod]
    public void SpriteEnableH_ViaBus_EnablesHighSprite()
    {
        var bus = MakeBus();

        // Enable sprite 8 via RegSpriteEnH bit 0
        bus.Write(VgcConstants.RegSpriteEnH, 0x01);

        var (_, _, _, enabled, _, _, _) = bus.Vgc.GetSpriteState(8);
        Assert.IsTrue(enabled, "Sprite 8 must be enabled via RegSpriteEnH bit 0.");
    }

    // -------------------------------------------------------------------------
    // 6. VGC mode change via bus
    // -------------------------------------------------------------------------

    [TestMethod]
    public void ModeChange_ViaBus_ReflectedInVgc()
    {
        var bus = MakeBus();

        bus.Write(VgcConstants.RegMode, 2);

        Assert.AreEqual(2, bus.Vgc.GetMode(),
            "VGC mode must reflect the value written via bus.");
    }

    [TestMethod]
    public void ModeChange_TextMode_ViaBus()
    {
        var bus = MakeBus();

        bus.Write(VgcConstants.RegMode, 0);
        Assert.AreEqual(0, bus.Vgc.GetMode(), "Mode 0 (text) must be stored correctly.");

        bus.Write(VgcConstants.RegMode, 1);
        Assert.AreEqual(1, bus.Vgc.GetMode(), "Mode 1 (block) must be stored correctly.");
    }

    // -------------------------------------------------------------------------
    // 7. ScreenEditor integration: write chars, read back via ScreenEditor
    // -------------------------------------------------------------------------

    [TestMethod]
    public void ScreenEditor_ReadLineFromScreen_ReflectsCharOut()
    {
        var bus = MakeBus();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        // Cursor starts at (0,0); write a word via CharOut
        foreach (byte b in "HELLO"u8)
            bus.Write(VgcConstants.RegCharOut, b);

        // Move cursor back to row 0 to read the line
        bus.Write(VgcConstants.RegCursorX, 0);
        bus.Write(VgcConstants.RegCursorY, 0);

        string line = editor.ReadLineFromScreen();
        Assert.IsTrue(line.StartsWith("HELLO"),
            $"ReadLineFromScreen must start with 'HELLO', got: '{line}'");
    }

    [TestMethod]
    public void ScreenEditor_QueueInput_FeedsCharsToCpu()
    {
        var bus = MakeBus();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        // Queue "RUN\r" — serial terminal model
        editor.QueueInput((byte)'R');
        editor.QueueInput((byte)'U');
        editor.QueueInput((byte)'N');
        editor.QueueInput(0x0D);

        // CPU reads via CHARIN register
        Assert.AreEqual((byte)'R', bus.Read(VgcConstants.RegCharIn));
        Assert.AreEqual((byte)'U', bus.Read(VgcConstants.RegCharIn));
        Assert.AreEqual((byte)'N', bus.Read(VgcConstants.RegCharIn));
        Assert.AreEqual(0x0D, bus.Read(VgcConstants.RegCharIn));
        Assert.AreEqual(0, bus.Read(VgcConstants.RegCharIn),
            "CHARIN must return 0 when queue is empty.");
    }

    // -------------------------------------------------------------------------
    // 8. Collision register read-clears via bus
    // -------------------------------------------------------------------------

    [TestMethod]
    public void CollisionSpriteSprite_ViaBus_ReadClears()
    {
        var bus = MakeBus();

        // Set collision registers via the VGC helper
        bus.Vgc.SetCollisionRegisters(spriteToSprite: 0xAB, spriteToBg: 0x00);

        byte first  = bus.Read(VgcConstants.RegColSt);
        byte second = bus.Read(VgcConstants.RegColSt);

        Assert.AreEqual(0xAB, first,  "RegColSt must return the set value on first read.");
        Assert.AreEqual(0x00, second, "RegColSt must clear to 0 after first read.");
    }

    [TestMethod]
    public void CollisionSpriteBg_ViaBus_ReadClears()
    {
        var bus = MakeBus();

        bus.Vgc.SetCollisionRegisters(spriteToSprite: 0x00, spriteToBg: 0x55);

        byte first  = bus.Read(VgcConstants.RegColBg);
        byte second = bus.Read(VgcConstants.RegColBg);

        Assert.AreEqual(0x55, first,  "RegColBg must return the set value on first read.");
        Assert.AreEqual(0x00, second, "RegColBg must clear to 0 after first read.");
    }

    // -------------------------------------------------------------------------
    // 9. ROM protection — writes above $C000 are ignored
    // -------------------------------------------------------------------------

    [TestMethod]
    public void RomProtection_WriteAtRomBase_IsIgnored()
    {
        var bus = MakeBus();

        byte original = bus.Read(VgcConstants.RomBase);
        bus.Write(VgcConstants.RomBase, (byte)(original ^ 0xFF));

        Assert.AreEqual(original, bus.Read(VgcConstants.RomBase),
            "Write to $C000 must be ignored (ROM protection).");
    }

    [TestMethod]
    public void RomProtection_WriteAtResetVector_IsIgnored()
    {
        var bus = MakeBus();

        byte originalLow  = bus.Read(0xFFFC);
        byte originalHigh = bus.Read(0xFFFD);

        bus.Write(0xFFFC, 0x00);
        bus.Write(0xFFFD, 0x00);

        Assert.AreEqual(originalLow,  bus.Read(0xFFFC),
            "Write to reset vector low byte must be ignored.");
        Assert.AreEqual(originalHigh, bus.Read(0xFFFD),
            "Write to reset vector high byte must be ignored.");
    }

    [TestMethod]
    public void RomProtection_WriteAtRomEnd_IsIgnored()
    {
        var bus = MakeBus();

        byte original = bus.Read(VgcConstants.RomEnd);
        bus.Write(VgcConstants.RomEnd, (byte)(original ^ 0x55));

        Assert.AreEqual(original, bus.Read(VgcConstants.RomEnd),
            "Write to $FFFF must be ignored (ROM protection).");
    }

    // -------------------------------------------------------------------------
    // 10. VSC isolation — writes to $A100 don't affect VGC registers or RAM
    // -------------------------------------------------------------------------

    // -------------------------------------------------------------------------
    // 11. GFX draw color defaults to foreground color when not explicitly set
    // -------------------------------------------------------------------------

    [TestMethod]
    public void GfxLine_WithoutGcolor_UsesFgColor()
    {
        var bus = MakeBus();

        // Set foreground color to 5 (text COLOR command writes here)
        bus.Write(VgcConstants.RegFgCol, 5);

        // Set MODE 1 (block graphics)
        bus.Write(VgcConstants.RegMode, 1);

        // Draw a LINE without ever issuing GCOLOR
        bus.Write(VgcConstants.RegGfxP0, 10);  // x1 low
        bus.Write(VgcConstants.RegGfxP1, 0);   // x1 high
        bus.Write(VgcConstants.RegGfxP2, 10);  // y1 low
        bus.Write(VgcConstants.RegGfxP3, 0);   // y1 high
        bus.Write(VgcConstants.RegGfxP4, 20);  // x2
        bus.Write(VgcConstants.RegGfxP5, 10);  // y2
        bus.Write(VgcConstants.RegGfxCmd, VgcConstants.GfxCmdLine);

        // Pixel at (10, 10) should be color 5 (the foreground color)
        Assert.IsTrue(bus.Vgc.GetGfxPixel(10, 10),
            "Line pixel must be set.");
        Assert.AreEqual(5, bus.Vgc.GetGfxPixelColor(10, 10),
            "Line must use text foreground color when no GCOLOR set.");
    }

    [TestMethod]
    public void GfxPlot_WithoutGcolor_UsesFgColor()
    {
        var bus = MakeBus();

        bus.Write(VgcConstants.RegFgCol, 3);

        // PLOT at (50, 25) without GCOLOR
        bus.Write(VgcConstants.RegGfxP0, 50);  // x low
        bus.Write(VgcConstants.RegGfxP1, 0);   // x high
        bus.Write(VgcConstants.RegGfxP2, 25);  // y low
        bus.Write(VgcConstants.RegGfxP3, 0);   // y high
        bus.Write(VgcConstants.RegGfxCmd, VgcConstants.GfxCmdPlot);

        Assert.AreEqual(3, bus.Vgc.GetGfxPixelColor(50, 25),
            "Plot must use text foreground color when no GCOLOR set.");
    }

    [TestMethod]
    public void VscIsolation_WriteToVscBase_DoesNotAffectVgc()
    {
        var bus = MakeBus();

        // $A100 is inside the VGC ownership range ($A000-$B39F),
        // but VSC is checked first — it must intercept and not touch VGC.
        bus.Write(VgcConstants.VscBase, 0x42);

        // Reading $A100 via the bus returns VSC value, not VGC
        Assert.AreEqual(0x42, bus.Read(VgcConstants.VscBase),
            "VSC must own $A100 and return the written value.");

        // VGC screen RAM starts at $A400; $A100 is not in screen RAM.
        // Confirm VGC screen char at col 0 row 0 is still a space.
        Assert.AreEqual(0x20, bus.Vgc.GetScreenChar(0, 0),
            "VGC screen RAM must be unaffected by a write to the VSC range.");
    }

    [TestMethod]
    public void VscIsolation_WriteToVscEnd_DoesNotAffectVgc()
    {
        var bus = MakeBus();

        bus.Write(VgcConstants.VscEnd, 0x7F);

        Assert.AreEqual(0x7F, bus.Read(VgcConstants.VscEnd),
            "VSC must own $A1FF and return the written value.");
    }
}

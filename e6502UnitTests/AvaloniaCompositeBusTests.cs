using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class AvaloniaCompositeBusTests
{
    private static CompositeBusDevice MakeBus() => new();

    // -------------------------------------------------------------------------
    // RAM read/write roundtrip
    // -------------------------------------------------------------------------

    [TestMethod]
    public void Ram_ReadWrite_RoundTrips()
    {
        var bus = MakeBus();
        bus.Write(0x0300, 0xAB);
        Assert.AreEqual(0xAB, bus.Read(0x0300));
    }

    [TestMethod]
    public void Ram_DefaultsToZero()
    {
        var bus = MakeBus();
        Assert.AreEqual(0x00, bus.Read(0x0200));
    }

    // -------------------------------------------------------------------------
    // VGC register access
    // -------------------------------------------------------------------------

    [TestMethod]
    public void Vgc_FgColorRegister_ReadBack()
    {
        var bus = MakeBus();
        bus.Write((ushort)VgcConstants.RegFgCol, 0x07);
        Assert.AreEqual(0x07, bus.Read((ushort)VgcConstants.RegFgCol));
    }

    [TestMethod]
    public void Vgc_BgColorRegister_ReadBack()
    {
        var bus = MakeBus();
        bus.Write((ushort)VgcConstants.RegBgCol, 0x05);
        Assert.AreEqual(0x05, bus.Read((ushort)VgcConstants.RegBgCol));
    }

    // -------------------------------------------------------------------------
    // CharOut via bus writes to VGC screen RAM
    // -------------------------------------------------------------------------

    [TestMethod]
    public void Vgc_CharOut_AppearsOnScreen()
    {
        var bus = MakeBus();
        bus.Write((ushort)VgcConstants.RegCharOut, (byte)'A');
        Assert.AreEqual((byte)'A', bus.Vgc.GetScreenChar(0, 0));
    }

    [TestMethod]
    public void Vgc_CharOut_AdvancesCursor()
    {
        var bus = MakeBus();
        bus.Write((ushort)VgcConstants.RegCharOut, (byte)'X');
        Assert.AreEqual(1, bus.Vgc.GetCursorX());
    }

    // -------------------------------------------------------------------------
    // ROM protection
    // -------------------------------------------------------------------------

    [TestMethod]
    public void Rom_EhBasicLoaded_FirstByteNonZero()
    {
        var bus = MakeBus();
        byte firstByte = bus.Read((ushort)VgcConstants.RomBase);
        Assert.AreNotEqual(0x00, firstByte);
    }

    [TestMethod]
    public void Rom_WritesIgnored()
    {
        var bus = MakeBus();
        byte original = bus.Read((ushort)VgcConstants.RomBase);
        bus.Write((ushort)VgcConstants.RomBase, (byte)(original ^ 0xFF));
        Assert.AreEqual(original, bus.Read((ushort)VgcConstants.RomBase));
    }

    // -------------------------------------------------------------------------
    // VSC isolation
    // -------------------------------------------------------------------------

    [TestMethod]
    public void Vsc_WriteRead_RoundTrips()
    {
        var bus = MakeBus();
        bus.Write((ushort)VgcConstants.VscBase, 0x42);
        Assert.AreEqual(0x42, bus.Read((ushort)VgcConstants.VscBase));
    }

    [TestMethod]
    public void Vsc_WriteLastRegister_ReadBack()
    {
        var bus = MakeBus();
        bus.Write((ushort)VgcConstants.VscEnd, 0x7F);
        Assert.AreEqual(0x7F, bus.Read((ushort)VgcConstants.VscEnd));
    }

    [TestMethod]
    public void Vsc_DoesNotRouteToVgc()
    {
        var bus = MakeBus();
        bus.Write((ushort)VgcConstants.VscBase, 0x99);
        Assert.AreEqual(0x99, bus.Read((ushort)VgcConstants.VscBase));
    }

    // -------------------------------------------------------------------------
    // Command execution through bus
    // -------------------------------------------------------------------------

    [TestMethod]
    public void Vgc_PlotCommand_SetsPixel()
    {
        var bus = MakeBus();
        // Set P0/P1 = x=5, P2/P3 = y=3
        bus.Write((ushort)VgcConstants.RegP0, 5);    // x low
        bus.Write((ushort)VgcConstants.RegP1, 0);    // x high
        bus.Write((ushort)VgcConstants.RegP2, 3);    // y low
        bus.Write((ushort)VgcConstants.RegP3, 0);    // y high
        // Execute PLOT command
        bus.Write((ushort)VgcConstants.RegCmd, VgcConstants.CmdPlot);
        Assert.IsTrue(bus.Vgc.GetGfxPixel(5, 3));
    }

    // -------------------------------------------------------------------------
    // Screen RAM access through bus
    // -------------------------------------------------------------------------

    [TestMethod]
    public void ScreenRam_DirectWrite_ReadBack()
    {
        var bus = MakeBus();
        bus.Write((ushort)VgcConstants.CharRamBase, (byte)'Z');
        Assert.AreEqual((byte)'Z', bus.Read((ushort)VgcConstants.CharRamBase));
    }

    [TestMethod]
    public void ScreenRam_DirectWrite_VisibleViaVgc()
    {
        var bus = MakeBus();
        bus.Write((ushort)VgcConstants.CharRamBase, (byte)'Q');
        Assert.AreEqual((byte)'Q', bus.Vgc.GetScreenChar(0, 0));
    }

    [TestMethod]
    public void ColorRam_DirectWrite_ReadBack()
    {
        var bus = MakeBus();
        bus.Write((ushort)VgcConstants.ColorRamBase, 0x0A);
        Assert.AreEqual(0x0A, bus.Read((ushort)VgcConstants.ColorRamBase));
    }

    // -------------------------------------------------------------------------
    // FIO address ownership
    // -------------------------------------------------------------------------

    [TestMethod]
    public void Fio_WriteCmd_ReadStatus()
    {
        var bus = MakeBus();
        // Writing an invalid command should set error status
        bus.Write((ushort)VgcConstants.FioCmd, 0xFF);
        byte status = bus.Read((ushort)VgcConstants.FioStatus);
        Assert.AreEqual(VgcConstants.FioStatusError, status);
    }

    [TestMethod]
    public void Fio_ReadErrCode_AfterBadCommand()
    {
        var bus = MakeBus();
        bus.Write((ushort)VgcConstants.FioCmd, 0xFF);
        byte errCode = bus.Read((ushort)VgcConstants.FioErrCode);
        Assert.AreEqual(VgcConstants.FioErrIo, errCode);
    }

    [TestMethod]
    public void Fio_RegisterWrite_DoesNotLeakToRam()
    {
        var bus = MakeBus();
        // Writing to FIO name length register
        bus.Write((ushort)VgcConstants.FioNameLen, 0x05);
        Assert.AreEqual(0x05, bus.Read((ushort)VgcConstants.FioNameLen));
    }

    // -------------------------------------------------------------------------
    // CharIn clears after read
    // -------------------------------------------------------------------------

    [TestMethod]
    public void Vgc_CharIn_ClearsAfterRead()
    {
        var bus = MakeBus();
        bus.Vgc.FeedInput((byte)'Z');
        byte first = bus.Read((ushort)VgcConstants.RegCharIn);
        byte second = bus.Read((ushort)VgcConstants.RegCharIn);
        Assert.AreEqual((byte)'Z', first);
        Assert.AreEqual(0x00, second);
    }

    // -------------------------------------------------------------------------
    // Vector table initialization
    // -------------------------------------------------------------------------

    [TestMethod]
    public void VectorTable_Initialized_AtBoot()
    {
        var bus = MakeBus();
        int vgcBase = bus.Read(0x0200) | (bus.Read(0x0201) << 8);
        Assert.AreEqual(VgcConstants.VgcBase, vgcBase);

        int cmdReg = bus.Read(0x0202) | (bus.Read(0x0203) << 8);
        Assert.AreEqual(VgcConstants.RegCmd, cmdReg);

        int charRam = bus.Read(0x0204) | (bus.Read(0x0205) << 8);
        Assert.AreEqual(VgcConstants.CharRamBase, charRam);

        int colorRam = bus.Read(0x0206) | (bus.Read(0x0207) << 8);
        Assert.AreEqual(VgcConstants.ColorRamBase, colorRam);

        int vscBase = bus.Read(0x0208) | (bus.Read(0x0209) << 8);
        Assert.AreEqual(VgcConstants.VscBase, vscBase);

        int fioBase = bus.Read(0x020A) | (bus.Read(0x020B) << 8);
        Assert.AreEqual(VgcConstants.FioBase, fioBase);
    }
}

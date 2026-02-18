using e6502.TUI.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class CompositeBusTests
{
    private static CompositeBusDevice MakeBus() => new();

    // -------------------------------------------------------------------------
    // RAM
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
    public void Vgc_RegisterWrite_ReadBack()
    {
        var bus = MakeBus();
        // Write to RegMode ($A000)
        bus.Write(VgcConstants.RegMode, 0x01);
        Assert.AreEqual(0x01, bus.Read(VgcConstants.RegMode));
    }

    [TestMethod]
    public void Vgc_BgColorRegister_ReadBack()
    {
        var bus = MakeBus();
        bus.Write(VgcConstants.RegBgCol, 0x05);
        Assert.AreEqual(0x05, bus.Read(VgcConstants.RegBgCol));
    }

    // -------------------------------------------------------------------------
    // CharOut via bus writes to VGC screen RAM
    // -------------------------------------------------------------------------

    [TestMethod]
    public void Vgc_CharOut_AppearsOnScreen()
    {
        var bus = MakeBus();
        // Write 'A' to CharOut
        bus.Write(VgcConstants.RegCharOut, (byte)'A');
        // Cursor was at (0,0) so 'A' lands at col=0, row=0
        Assert.AreEqual((byte)'A', bus.Vgc.GetScreenChar(0, 0));
    }

    [TestMethod]
    public void Vgc_CharOut_AdvancesCursor()
    {
        var bus = MakeBus();
        bus.Write(VgcConstants.RegCharOut, (byte)'X');
        // After one char, cursor X should be 1
        Assert.AreEqual(1, bus.Vgc.GetCursorX());
    }

    // -------------------------------------------------------------------------
    // ROM: EhBASIC loaded
    // -------------------------------------------------------------------------

    [TestMethod]
    public void Rom_EhBasicLoaded_FirstByteNonZero()
    {
        var bus = MakeBus();
        byte firstByte = bus.Read(VgcConstants.RomBase);
        Assert.AreNotEqual(0x00, firstByte);
    }

    [TestMethod]
    public void Rom_WritesIgnored()
    {
        var bus = MakeBus();
        byte original = bus.Read(VgcConstants.RomBase);
        bus.Write(VgcConstants.RomBase, (byte)(original ^ 0xFF));
        Assert.AreEqual(original, bus.Read(VgcConstants.RomBase));
    }

    // -------------------------------------------------------------------------
    // VSC stub
    // -------------------------------------------------------------------------

    [TestMethod]
    public void Vsc_WriteRead_RoundTrips()
    {
        var bus = MakeBus();
        bus.Write(VgcConstants.VscBase, 0x42);
        Assert.AreEqual(0x42, bus.Read(VgcConstants.VscBase));
    }

    [TestMethod]
    public void Vsc_WriteLastRegister_ReadBack()
    {
        var bus = MakeBus();
        bus.Write(VgcConstants.VscEnd, 0x7F);
        Assert.AreEqual(0x7F, bus.Read(VgcConstants.VscEnd));
    }

    [TestMethod]
    public void Vsc_DoesNotRouteToVgc()
    {
        // VSC range $A100-$A1FF is inside VGC range $A000-$B39F.
        // A write to $A100 must land in VSC, not VGC screen/color RAM.
        var bus = MakeBus();
        bus.Write(VgcConstants.VscBase, 0x99);
        // VGC should not have been touched — reading same address via bus returns VSC value
        Assert.AreEqual(0x99, bus.Read(VgcConstants.VscBase));
    }

    // -------------------------------------------------------------------------
    // CharIn clears after read
    // -------------------------------------------------------------------------

    [TestMethod]
    public void Vgc_CharIn_ClearsAfterRead()
    {
        var bus = MakeBus();
        bus.Vgc.FeedInput((byte)'Z');
        byte first = bus.Read(VgcConstants.RegCharIn);
        byte second = bus.Read(VgcConstants.RegCharIn);
        Assert.AreEqual((byte)'Z', first);
        Assert.AreEqual(0x00, second);
    }
}

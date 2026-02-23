using System;
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
    // SID routing
    // -------------------------------------------------------------------------

    [TestMethod]
    public void Sid_Write_DoesNotCorruptRom()
    {
        var bus = MakeBus();
        byte romByte = bus.Read(0xD400);
        bus.Write(0xD400, 0xFF);
        Assert.AreEqual(romByte, bus.Read(0xD400));  // reads still return ROM
    }

    [TestMethod]
    public void Sid_WriteToSidRange_DoesNotThrow()
    {
        var bus = MakeBus();
        bus.Write(0xD418, 0x0F);  // SID master volume
        // No exception = pass
    }

    [TestMethod]
    public void Sid_OutsideRange_RomProtected()
    {
        var bus = MakeBus();
        byte romBefore = bus.Read(0xD41D);
        bus.Write(0xD41D, 0xAA);
        Assert.AreEqual(romBefore, bus.Read(0xD41D));
    }

    // -------------------------------------------------------------------------
    // XMC expansion memory controller
    // -------------------------------------------------------------------------

    [TestMethod]
    public void Xmc_RegisterReadWrite_RoundTrips()
    {
        var bus = MakeBus();
        bus.Write((ushort)VgcConstants.XmcData, 0x5A);
        Assert.AreEqual(0x5A, bus.Read((ushort)VgcConstants.XmcData));
    }

    [TestMethod]
    public void Xmc_PutGetByte_RoundTrips()
    {
        var bus = MakeBus();

        bus.Write((ushort)VgcConstants.XmcAddrL, 0x34);
        bus.Write((ushort)VgcConstants.XmcAddrM, 0x12);
        bus.Write((ushort)VgcConstants.XmcAddrH, 0x00);
        bus.Write((ushort)VgcConstants.XmcData, 0xA7);
        bus.Write((ushort)VgcConstants.XmcCmd, VgcConstants.XmcCmdPutByte);

        Assert.AreEqual(VgcConstants.XmcStatusOk, bus.Read((ushort)VgcConstants.XmcStatus));

        bus.Write((ushort)VgcConstants.XmcCmd, VgcConstants.XmcCmdGetByte);
        Assert.AreEqual(0xA7, bus.Read((ushort)VgcConstants.XmcData));
    }

    [TestMethod]
    public void Xmc_StashFetch_CopiesData()
    {
        var bus = MakeBus();
        bus.Write(0x0400, 0x11);
        bus.Write(0x0401, 0x22);
        bus.Write(0x0402, 0x33);

        bus.Write((ushort)VgcConstants.XmcRamL, 0x00);
        bus.Write((ushort)VgcConstants.XmcRamH, 0x04);
        bus.Write((ushort)VgcConstants.XmcAddrL, 0x00);
        bus.Write((ushort)VgcConstants.XmcAddrM, 0x20);
        bus.Write((ushort)VgcConstants.XmcAddrH, 0x00);
        bus.Write((ushort)VgcConstants.XmcLenL, 0x03);
        bus.Write((ushort)VgcConstants.XmcLenH, 0x00);
        bus.Write((ushort)VgcConstants.XmcCmd, VgcConstants.XmcCmdStash);
        Assert.AreEqual(VgcConstants.XmcStatusOk, bus.Read((ushort)VgcConstants.XmcStatus));

        bus.Write(0x0400, 0x00);
        bus.Write(0x0401, 0x00);
        bus.Write(0x0402, 0x00);

        bus.Write((ushort)VgcConstants.XmcCmd, VgcConstants.XmcCmdFetch);
        Assert.AreEqual(VgcConstants.XmcStatusOk, bus.Read((ushort)VgcConstants.XmcStatus));
        Assert.AreEqual(0x11, bus.Read(0x0400));
        Assert.AreEqual(0x22, bus.Read(0x0401));
        Assert.AreEqual(0x33, bus.Read(0x0402));
    }

    [TestMethod]
    public void Xmc_UsageStats_TrackWrittenPages()
    {
        var bus = MakeBus();

        bus.Write((ushort)VgcConstants.XmcCmd, VgcConstants.XmcCmdResetUsage);
        Assert.AreEqual(VgcConstants.XmcStatusOk, bus.Read((ushort)VgcConstants.XmcStatus));

        bus.Write((ushort)VgcConstants.XmcAddrL, 0x00);
        bus.Write((ushort)VgcConstants.XmcAddrM, 0x20);
        bus.Write((ushort)VgcConstants.XmcAddrH, 0x00);
        bus.Write((ushort)VgcConstants.XmcData, 0x7A);
        bus.Write((ushort)VgcConstants.XmcCmd, VgcConstants.XmcCmdPutByte);
        Assert.AreEqual(VgcConstants.XmcStatusOk, bus.Read((ushort)VgcConstants.XmcStatus));

        int usedPages = bus.Read((ushort)VgcConstants.XmcPagesUsedL)
            | (bus.Read((ushort)VgcConstants.XmcPagesUsedH) << 8);
        int freePages = bus.Read((ushort)VgcConstants.XmcPagesFreeL)
            | (bus.Read((ushort)VgcConstants.XmcPagesFreeH) << 8);
        int totalPages = bus.Read((ushort)VgcConstants.XmcBanks) * 256;

        Assert.AreEqual(1, usedPages);
        Assert.AreEqual(totalPages - 1, freePages);
    }

    [TestMethod]
    public void Xmc_ReleaseCommand_UnmarksUsedPages()
    {
        var bus = MakeBus();

        bus.Write((ushort)VgcConstants.XmcCmd, VgcConstants.XmcCmdResetUsage);

        bus.Write((ushort)VgcConstants.XmcAddrL, 0xF0);
        bus.Write((ushort)VgcConstants.XmcAddrM, 0x20);
        bus.Write((ushort)VgcConstants.XmcAddrH, 0x00);
        bus.Write((ushort)VgcConstants.XmcLenL, 0x20);
        bus.Write((ushort)VgcConstants.XmcLenH, 0x00);
        bus.Write((ushort)VgcConstants.XmcData, 0xCC);
        bus.Write((ushort)VgcConstants.XmcCmd, VgcConstants.XmcCmdFill);
        Assert.AreEqual(VgcConstants.XmcStatusOk, bus.Read((ushort)VgcConstants.XmcStatus));

        int usedBeforeRelease = bus.Read((ushort)VgcConstants.XmcPagesUsedL)
            | (bus.Read((ushort)VgcConstants.XmcPagesUsedH) << 8);
        Assert.AreEqual(2, usedBeforeRelease);

        bus.Write((ushort)VgcConstants.XmcCmd, VgcConstants.XmcCmdRelease);
        Assert.AreEqual(VgcConstants.XmcStatusOk, bus.Read((ushort)VgcConstants.XmcStatus));

        int usedAfterRelease = bus.Read((ushort)VgcConstants.XmcPagesUsedL)
            | (bus.Read((ushort)VgcConstants.XmcPagesUsedH) << 8);
        int freeAfterRelease = bus.Read((ushort)VgcConstants.XmcPagesFreeL)
            | (bus.Read((ushort)VgcConstants.XmcPagesFreeH) << 8);
        int totalPages = bus.Read((ushort)VgcConstants.XmcBanks) * 256;

        Assert.AreEqual(0, usedAfterRelease);
        Assert.AreEqual(totalPages, freeAfterRelease);
    }

    [TestMethod]
    public void Xmc_AllocCommand_ReturnsHandleAndMarksPages()
    {
        var bus = MakeBus();

        bus.Write((ushort)VgcConstants.XmcCmd, VgcConstants.XmcCmdResetUsage);
        bus.Write((ushort)VgcConstants.XmcLenL, 0x2C); // 300 bytes
        bus.Write((ushort)VgcConstants.XmcLenH, 0x01);
        bus.Write((ushort)VgcConstants.XmcCmd, VgcConstants.XmcCmdAlloc);

        Assert.AreEqual(VgcConstants.XmcStatusOk, bus.Read((ushort)VgcConstants.XmcStatus));
        Assert.AreNotEqual(0, bus.Read((ushort)VgcConstants.XmcHandle));

        int usedPages = bus.Read((ushort)VgcConstants.XmcPagesUsedL)
            | (bus.Read((ushort)VgcConstants.XmcPagesUsedH) << 8);
        Assert.AreEqual(2, usedPages);
    }

    [TestMethod]
    public void Xmc_NamedStashFetch_RoundTrips()
    {
        var bus = MakeBus();

        bus.Write(0x0500, 0xDE);
        bus.Write(0x0501, 0xAD);
        bus.Write(0x0502, 0xBE);
        bus.Write(0x0503, 0xEF);

        WriteXmcName(bus, "MELODY");
        bus.Write((ushort)VgcConstants.XmcRamL, 0x00);
        bus.Write((ushort)VgcConstants.XmcRamH, 0x05);
        bus.Write((ushort)VgcConstants.XmcLenL, 0x04);
        bus.Write((ushort)VgcConstants.XmcLenH, 0x00);
        bus.Write((ushort)VgcConstants.XmcCmd, VgcConstants.XmcCmdNStash);
        Assert.AreEqual(VgcConstants.XmcStatusOk, bus.Read((ushort)VgcConstants.XmcStatus));

        bus.Write(0x0500, 0x00);
        bus.Write(0x0501, 0x00);
        bus.Write(0x0502, 0x00);
        bus.Write(0x0503, 0x00);

        WriteXmcName(bus, "MELODY");
        bus.Write((ushort)VgcConstants.XmcRamL, 0x00);
        bus.Write((ushort)VgcConstants.XmcRamH, 0x05);
        bus.Write((ushort)VgcConstants.XmcLenL, 0x00); // 0 = whole named block
        bus.Write((ushort)VgcConstants.XmcLenH, 0x00);
        bus.Write((ushort)VgcConstants.XmcCmd, VgcConstants.XmcCmdNFetch);
        Assert.AreEqual(VgcConstants.XmcStatusOk, bus.Read((ushort)VgcConstants.XmcStatus));

        Assert.AreEqual(0xDE, bus.Read(0x0500));
        Assert.AreEqual(0xAD, bus.Read(0x0501));
        Assert.AreEqual(0xBE, bus.Read(0x0502));
        Assert.AreEqual(0xEF, bus.Read(0x0503));
    }

    [TestMethod]
    public void Xmc_NamedDir_EnumeratesBlocks()
    {
        var bus = MakeBus();

        bus.Write(0x0600, 0x11);
        bus.Write(0x0601, 0x22);

        WriteXmcName(bus, "BETA");
        bus.Write((ushort)VgcConstants.XmcRamL, 0x00);
        bus.Write((ushort)VgcConstants.XmcRamH, 0x06);
        bus.Write((ushort)VgcConstants.XmcLenL, 0x02);
        bus.Write((ushort)VgcConstants.XmcLenH, 0x00);
        bus.Write((ushort)VgcConstants.XmcCmd, VgcConstants.XmcCmdNStash);

        WriteXmcName(bus, "ALPHA");
        bus.Write((ushort)VgcConstants.XmcRamL, 0x00);
        bus.Write((ushort)VgcConstants.XmcRamH, 0x06);
        bus.Write((ushort)VgcConstants.XmcLenL, 0x02);
        bus.Write((ushort)VgcConstants.XmcLenH, 0x00);
        bus.Write((ushort)VgcConstants.XmcCmd, VgcConstants.XmcCmdNStash);

        bus.Write((ushort)VgcConstants.XmcCmd, VgcConstants.XmcCmdNDirOpen);
        Assert.AreEqual(VgcConstants.XmcStatusOk, bus.Read((ushort)VgcConstants.XmcStatus));
        Assert.AreEqual("ALPHA", ReadXmcName(bus));

        bus.Write((ushort)VgcConstants.XmcCmd, VgcConstants.XmcCmdNDirRead);
        Assert.AreEqual(VgcConstants.XmcStatusOk, bus.Read((ushort)VgcConstants.XmcStatus));
        Assert.AreEqual("BETA", ReadXmcName(bus));
    }

    [TestMethod]
    public void Xmc_MappedWindow_ReadWrite_Works()
    {
        var bus = MakeBus();

        bus.Write((ushort)VgcConstants.XmcWin0AL, 0x00);
        bus.Write((ushort)VgcConstants.XmcWin0AM, 0x20);
        bus.Write((ushort)VgcConstants.XmcWin0AH, 0x00);
        bus.Write((ushort)VgcConstants.XmcWinCtl, 0x01); // enable window 0

        bus.Write(0xBC42, 0x5C);
        Assert.AreEqual(0x5C, bus.Read(0xBC42));

        bus.Write((ushort)VgcConstants.XmcAddrL, 0x42);
        bus.Write((ushort)VgcConstants.XmcAddrM, 0x20);
        bus.Write((ushort)VgcConstants.XmcAddrH, 0x00);
        bus.Write((ushort)VgcConstants.XmcCmd, VgcConstants.XmcCmdGetByte);
        Assert.AreEqual(0x5C, bus.Read((ushort)VgcConstants.XmcData));
    }

    private static void WriteXmcName(CompositeBusDevice bus, string name)
    {
        int maxLen = VgcConstants.XmcNameEnd - VgcConstants.XmcName + 1;
        int len = Math.Min(name.Length, maxLen);

        bus.Write((ushort)VgcConstants.XmcNameLen, (byte)len);
        for (int i = 0; i < maxLen; i++)
            bus.Write((ushort)(VgcConstants.XmcName + i), 0x00);
        for (int i = 0; i < len; i++)
            bus.Write((ushort)(VgcConstants.XmcName + i), (byte)name[i]);
    }

    private static string ReadXmcName(CompositeBusDevice bus)
    {
        int len = bus.Read((ushort)VgcConstants.XmcNameLen);
        Span<char> chars = stackalloc char[len];
        for (int i = 0; i < len; i++)
            chars[i] = (char)bus.Read((ushort)(VgcConstants.XmcName + i));
        return new string(chars);
    }

    // -------------------------------------------------------------------------
    // NIC register routing
    // -------------------------------------------------------------------------

    [TestMethod]
    public void Bus_NicRegisters_Routed()
    {
        var bus = new CompositeBusDevice(enableSound: false);
        bus.Write((ushort)VgcConstants.NicSlot, 2);
        Assert.AreEqual(2, bus.Read((ushort)VgcConstants.NicSlot));
        bus.Dispose();
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

        int sidBase = bus.Read(0x0208) | (bus.Read(0x0209) << 8);
        Assert.AreEqual(VgcConstants.SidBase, sidBase);

        int fioBase = bus.Read(0x020A) | (bus.Read(0x020B) << 8);
        Assert.AreEqual(VgcConstants.FioBase, fioBase);

        int xmcBase = bus.Read(0x020C) | (bus.Read(0x020D) << 8);
        Assert.AreEqual(VgcConstants.XmcBase, xmcBase);

        int timerBase = bus.Read(0x020E) | (bus.Read(0x020F) << 8);
        Assert.AreEqual(VgcConstants.TimerBase, timerBase);
    }
}

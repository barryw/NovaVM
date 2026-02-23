using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class AvaloniaDmaTests
{
    private static CompositeBusDevice MakeBus() => new(enableSound: false);

    [TestMethod]
    public void Dma_RamToVgcChar_CopiesBytes()
    {
        var bus = MakeBus();
        bus.Write(0x0400, (byte)'N');
        bus.Write(0x0401, (byte)'O');
        bus.Write(0x0402, (byte)'V');

        StartDma(
            bus,
            srcSpace: VgcConstants.DmaSpaceCpuRam,
            dstSpace: VgcConstants.DmaSpaceVgcChar,
            srcAddr: 0x000400,
            dstAddr: 100,
            length: 3);

        AssertDmaOk(bus, expectedCount: 3);
        Assert.AreEqual((byte)'N', bus.Read((ushort)(VgcConstants.CharRamBase + 100)));
        Assert.AreEqual((byte)'O', bus.Read((ushort)(VgcConstants.CharRamBase + 101)));
        Assert.AreEqual((byte)'V', bus.Read((ushort)(VgcConstants.CharRamBase + 102)));
    }

    [TestMethod]
    public void Dma_VgcGfxToRam_CopiesBytes()
    {
        var bus = MakeBus();
        Assert.IsTrue(bus.Vgc.TryWriteMemorySpace(VgcConstants.MemSpaceGfx, 500, 0x0E));
        Assert.IsTrue(bus.Vgc.TryWriteMemorySpace(VgcConstants.MemSpaceGfx, 501, 0x0F));

        StartDma(
            bus,
            srcSpace: VgcConstants.DmaSpaceVgcGfx,
            dstSpace: VgcConstants.DmaSpaceCpuRam,
            srcAddr: 500,
            dstAddr: 0x002200,
            length: 2);

        AssertDmaOk(bus, expectedCount: 2);
        Assert.AreEqual(0x0E, bus.Read(0x2200));
        Assert.AreEqual(0x0F, bus.Read(0x2201));
    }

    [TestMethod]
    public void Dma_RamToXram_AndBack_RoundTrips()
    {
        var bus = MakeBus();
        bus.Write(0x0600, 0x11);
        bus.Write(0x0601, 0x22);
        bus.Write(0x0602, 0x33);
        bus.Write(0x0603, 0x44);

        StartDma(
            bus,
            srcSpace: VgcConstants.DmaSpaceCpuRam,
            dstSpace: VgcConstants.DmaSpaceXram,
            srcAddr: 0x000600,
            dstAddr: 0x001234,
            length: 4);
        AssertDmaOk(bus, expectedCount: 4);

        bus.Write(0x0700, 0x00);
        bus.Write(0x0701, 0x00);
        bus.Write(0x0702, 0x00);
        bus.Write(0x0703, 0x00);

        StartDma(
            bus,
            srcSpace: VgcConstants.DmaSpaceXram,
            dstSpace: VgcConstants.DmaSpaceCpuRam,
            srcAddr: 0x001234,
            dstAddr: 0x000700,
            length: 4);
        AssertDmaOk(bus, expectedCount: 4);

        Assert.AreEqual(0x11, bus.Read(0x0700));
        Assert.AreEqual(0x22, bus.Read(0x0701));
        Assert.AreEqual(0x33, bus.Read(0x0702));
        Assert.AreEqual(0x44, bus.Read(0x0703));

        int usedPages = bus.Read((ushort)VgcConstants.XmcPagesUsedL)
            | (bus.Read((ushort)VgcConstants.XmcPagesUsedH) << 8);
        Assert.AreEqual(1, usedPages);
    }

    [TestMethod]
    public void Dma_FillMode_WritesDestinationRange()
    {
        var bus = MakeBus();

        StartDma(
            bus,
            srcSpace: VgcConstants.DmaSpaceCpuRam,
            dstSpace: VgcConstants.DmaSpaceVgcColor,
            srcAddr: 0,
            dstAddr: 32,
            length: 5,
            mode: VgcConstants.DmaModeFill,
            fillValue: 0x0C);

        AssertDmaOk(bus, expectedCount: 5);
        for (int i = 0; i < 5; i++)
            Assert.AreEqual(0x0C, bus.Read((ushort)(VgcConstants.ColorRamBase + 32 + i)));
    }

    [TestMethod]
    public void Dma_CpuRomDestination_IsWriteProtected()
    {
        var bus = MakeBus();
        Assert.IsTrue(bus.Vgc.TryWriteMemorySpace(VgcConstants.MemSpaceScreen, 0, 0x41));
        byte romBefore = bus.Read((ushort)VgcConstants.RomBase);

        StartDma(
            bus,
            srcSpace: VgcConstants.DmaSpaceVgcChar,
            dstSpace: VgcConstants.DmaSpaceCpuRam,
            srcAddr: 0,
            dstAddr: VgcConstants.RomBase,
            length: 1);

        Assert.AreEqual(VgcConstants.DmaStatusError, bus.Read((ushort)VgcConstants.DmaStatus));
        Assert.AreEqual(VgcConstants.DmaErrWriteProt, bus.Read((ushort)VgcConstants.DmaErrCode));
        Assert.AreEqual(0, GetDmaCount(bus));
        Assert.AreEqual(romBefore, bus.Read((ushort)VgcConstants.RomBase));
    }

    private static void StartDma(
        CompositeBusDevice bus,
        byte srcSpace,
        byte dstSpace,
        int srcAddr,
        int dstAddr,
        int length,
        byte mode = 0,
        byte fillValue = 0)
    {
        bus.Write((ushort)VgcConstants.DmaSrcSpace, srcSpace);
        bus.Write((ushort)VgcConstants.DmaDstSpace, dstSpace);
        Write24(bus, VgcConstants.DmaSrcL, srcAddr);
        Write24(bus, VgcConstants.DmaDstL, dstAddr);
        Write24(bus, VgcConstants.DmaLenL, length);
        bus.Write((ushort)VgcConstants.DmaMode, mode);
        bus.Write((ushort)VgcConstants.DmaFillValue, fillValue);
        bus.Write((ushort)VgcConstants.DmaCmd, VgcConstants.DmaCmdStart);
    }

    private static void AssertDmaOk(CompositeBusDevice bus, int expectedCount)
    {
        Assert.AreEqual(VgcConstants.DmaStatusOk, bus.Read((ushort)VgcConstants.DmaStatus));
        Assert.AreEqual(VgcConstants.DmaErrNone, bus.Read((ushort)VgcConstants.DmaErrCode));
        Assert.AreEqual(expectedCount, GetDmaCount(bus));
    }

    private static int GetDmaCount(CompositeBusDevice bus) =>
        bus.Read((ushort)VgcConstants.DmaCountL)
        | (bus.Read((ushort)VgcConstants.DmaCountM) << 8)
        | (bus.Read((ushort)VgcConstants.DmaCountH) << 16);

    private static void Write24(CompositeBusDevice bus, int baseAddress, int value)
    {
        bus.Write((ushort)baseAddress, (byte)(value & 0xFF));
        bus.Write((ushort)(baseAddress + 1), (byte)((value >> 8) & 0xFF));
        bus.Write((ushort)(baseAddress + 2), (byte)((value >> 16) & 0xFF));
    }
}

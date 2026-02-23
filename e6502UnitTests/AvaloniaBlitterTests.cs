using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class AvaloniaBlitterTests
{
    private static CompositeBusDevice MakeBus() => new(enableSound: false);

    [TestMethod]
    public void Blitter_RamToVgcGfx_RectCopy_WritesRowsWithStride()
    {
        var bus = MakeBus();

        int srcBase = 0x2400;
        int srcStride = 8;
        int width = 4;
        int height = 3;
        for (int row = 0; row < height; row++)
        {
            for (int col = 0; col < width; col++)
                bus.Write((ushort)(srcBase + row * srcStride + col), (byte)(0x30 + row * 8 + col));
        }

        int dstBase = 512;
        int dstStride = VgcConstants.GfxWidth;
        StartBlit(
            bus,
            srcSpace: VgcConstants.DmaSpaceCpuRam,
            dstSpace: VgcConstants.DmaSpaceVgcGfx,
            srcAddr: srcBase,
            dstAddr: dstBase,
            width: width,
            height: height,
            srcStride: srcStride,
            dstStride: dstStride);

        AssertBlitOk(bus, expectedCount: width * height);
        for (int row = 0; row < height; row++)
        {
            for (int col = 0; col < width; col++)
            {
                int offset = dstBase + row * dstStride + col;
                Assert.IsTrue(bus.Vgc.TryReadMemorySpace(VgcConstants.MemSpaceGfx, offset, out byte value));
                Assert.AreEqual((byte)(0x30 + row * 8 + col), value);
            }
        }
    }

    [TestMethod]
    public void Blitter_FillMode_WritesSolidRect()
    {
        var bus = MakeBus();

        int width = 5;
        int height = 4;
        StartBlit(
            bus,
            srcSpace: VgcConstants.DmaSpaceCpuRam,
            dstSpace: VgcConstants.DmaSpaceVgcColor,
            srcAddr: 0,
            dstAddr: 24,
            width: width,
            height: height,
            srcStride: width,
            dstStride: VgcConstants.ScreenCols,
            mode: VgcConstants.BltModeFill,
            fillValue: 0x0E);

        AssertBlitOk(bus, expectedCount: width * height);
        for (int row = 0; row < height; row++)
        {
            for (int col = 0; col < width; col++)
            {
                int offset = 24 + row * VgcConstants.ScreenCols + col;
                Assert.AreEqual(0x0E, bus.Read((ushort)(VgcConstants.ColorRamBase + offset)));
            }
        }
    }

    [TestMethod]
    public void Blitter_ColorKeyMode_SkipsMatchingSourceBytes()
    {
        var bus = MakeBus();

        int src = 0x3100;
        int dst = 0x3200;
        byte[] srcData = [1, 0, 2, 0, 3];
        for (int i = 0; i < srcData.Length; i++)
            bus.Write((ushort)(src + i), srcData[i]);
        for (int i = 0; i < srcData.Length; i++)
            bus.Write((ushort)(dst + i), 9);

        StartBlit(
            bus,
            srcSpace: VgcConstants.DmaSpaceCpuRam,
            dstSpace: VgcConstants.DmaSpaceCpuRam,
            srcAddr: src,
            dstAddr: dst,
            width: srcData.Length,
            height: 1,
            srcStride: srcData.Length,
            dstStride: srcData.Length,
            mode: VgcConstants.BltModeColorKey,
            colorKey: 0);

        AssertBlitOk(bus, expectedCount: 3);
        CollectionAssert.AreEqual(
            new byte[] { 1, 9, 2, 9, 3 },
            new byte[]
            {
                bus.Read((ushort)dst),
                bus.Read((ushort)(dst + 1)),
                bus.Read((ushort)(dst + 2)),
                bus.Read((ushort)(dst + 3)),
                bus.Read((ushort)(dst + 4))
            });
    }

    [TestMethod]
    public void Blitter_SameSpaceOverlap_CopiesUsingStableSource()
    {
        var bus = MakeBus();
        int baseAddr = 0x3400;
        for (int i = 0; i < 10; i++)
            bus.Write((ushort)(baseAddr + i), (byte)i);

        StartBlit(
            bus,
            srcSpace: VgcConstants.DmaSpaceCpuRam,
            dstSpace: VgcConstants.DmaSpaceCpuRam,
            srcAddr: baseAddr,
            dstAddr: baseAddr + 2,
            width: 8,
            height: 1,
            srcStride: 8,
            dstStride: 8);

        AssertBlitOk(bus, expectedCount: 8);
        CollectionAssert.AreEqual(
            new byte[] { 0, 1, 0, 1, 2, 3, 4, 5, 6, 7 },
            new byte[]
            {
                bus.Read((ushort)baseAddr),
                bus.Read((ushort)(baseAddr + 1)),
                bus.Read((ushort)(baseAddr + 2)),
                bus.Read((ushort)(baseAddr + 3)),
                bus.Read((ushort)(baseAddr + 4)),
                bus.Read((ushort)(baseAddr + 5)),
                bus.Read((ushort)(baseAddr + 6)),
                bus.Read((ushort)(baseAddr + 7)),
                bus.Read((ushort)(baseAddr + 8)),
                bus.Read((ushort)(baseAddr + 9))
            });
    }

    [TestMethod]
    public void Blitter_CpuRomDestination_IsWriteProtected()
    {
        var bus = MakeBus();
        bus.Write(0x2400, 0x41);
        byte romBefore = bus.Read((ushort)VgcConstants.RomBase);

        StartBlit(
            bus,
            srcSpace: VgcConstants.DmaSpaceCpuRam,
            dstSpace: VgcConstants.DmaSpaceCpuRam,
            srcAddr: 0x2400,
            dstAddr: VgcConstants.RomBase,
            width: 1,
            height: 1,
            srcStride: 1,
            dstStride: 1);

        Assert.AreEqual(VgcConstants.BltStatusError, bus.Read((ushort)VgcConstants.BltStatus));
        Assert.AreEqual(VgcConstants.BltErrWriteProt, bus.Read((ushort)VgcConstants.BltErrCode));
        Assert.AreEqual(0, GetBlitCount(bus));
        Assert.AreEqual(romBefore, bus.Read((ushort)VgcConstants.RomBase));
    }

    private static void StartBlit(
        CompositeBusDevice bus,
        byte srcSpace,
        byte dstSpace,
        int srcAddr,
        int dstAddr,
        int width,
        int height,
        int srcStride,
        int dstStride,
        byte mode = 0,
        byte fillValue = 0,
        byte colorKey = 0)
    {
        bus.Write((ushort)VgcConstants.BltSrcSpace, srcSpace);
        bus.Write((ushort)VgcConstants.BltDstSpace, dstSpace);
        Write24(bus, VgcConstants.BltSrcL, srcAddr);
        Write24(bus, VgcConstants.BltDstL, dstAddr);
        Write16(bus, VgcConstants.BltWidthL, width);
        Write16(bus, VgcConstants.BltHeightL, height);
        Write16(bus, VgcConstants.BltSrcStrideL, srcStride);
        Write16(bus, VgcConstants.BltDstStrideL, dstStride);
        bus.Write((ushort)VgcConstants.BltMode, mode);
        bus.Write((ushort)VgcConstants.BltFillValue, fillValue);
        bus.Write((ushort)VgcConstants.BltColorKey, colorKey);
        bus.Write((ushort)VgcConstants.BltCmd, VgcConstants.BltCmdStart);
    }

    private static void AssertBlitOk(CompositeBusDevice bus, int expectedCount)
    {
        Assert.AreEqual(VgcConstants.BltStatusOk, bus.Read((ushort)VgcConstants.BltStatus));
        Assert.AreEqual(VgcConstants.BltErrNone, bus.Read((ushort)VgcConstants.BltErrCode));
        Assert.AreEqual(expectedCount, GetBlitCount(bus));
    }

    private static int GetBlitCount(CompositeBusDevice bus) =>
        bus.Read((ushort)VgcConstants.BltCountL)
        | (bus.Read((ushort)VgcConstants.BltCountM) << 8)
        | (bus.Read((ushort)VgcConstants.BltCountH) << 16);

    private static void Write16(CompositeBusDevice bus, int baseAddress, int value)
    {
        bus.Write((ushort)baseAddress, (byte)(value & 0xFF));
        bus.Write((ushort)(baseAddress + 1), (byte)((value >> 8) & 0xFF));
    }

    private static void Write24(CompositeBusDevice bus, int baseAddress, int value)
    {
        bus.Write((ushort)baseAddress, (byte)(value & 0xFF));
        bus.Write((ushort)(baseAddress + 1), (byte)((value >> 8) & 0xFF));
        bus.Write((ushort)(baseAddress + 2), (byte)((value >> 16) & 0xFF));
    }
}

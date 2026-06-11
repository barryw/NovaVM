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
                Assert.AreEqual(0x0E, bus.ReadVramByte(VgcConstants.VramPlaneColor, offset));
            }
        }
    }

    [TestMethod]
    public void Blitter_FillMode_WritesTextAttributeRamRect()
    {
        var bus = MakeBus();

        StartBlit(
            bus,
            srcSpace: VgcConstants.DmaSpaceCpuRam,
            dstSpace: VgcConstants.DmaSpaceVgcTextAttr,
            srcAddr: 0,
            dstAddr: VgcConstants.ScreenCols + 3,
            width: 3,
            height: 2,
            srcStride: 3,
            dstStride: VgcConstants.ScreenCols,
            mode: VgcConstants.BltModeFill,
            fillValue: VgcConstants.TextAttrFlash);

        AssertBlitOk(bus, expectedCount: 6);
        for (int row = 1; row <= 2; row++)
            for (int col = 3; col <= 5; col++)
                Assert.AreEqual(VgcConstants.TextAttrFlash, bus.Vgc.GetScreenTextAttr(col, row));

        Assert.AreEqual(0, bus.Vgc.GetScreenTextAttr(2, 1));
        Assert.AreEqual(0, bus.Vgc.GetScreenTextAttr(6, 1));
        Assert.AreEqual(0, bus.Vgc.GetScreenTextAttr(3, 3));
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
    public void Blitter_StartTransitionsToBusyUntilCyclesAdvance()
    {
        var bus = MakeBus();
        bus.Write(0x3000, 0x2A);

        StartBlit(
            bus,
            srcSpace: VgcConstants.DmaSpaceCpuRam,
            dstSpace: VgcConstants.DmaSpaceCpuRam,
            srcAddr: 0x3000,
            dstAddr: 0x3010,
            width: 1,
            height: 1,
            srcStride: 1,
            dstStride: 1);

        // Blitter completes synchronously (emulates RDY stall)
        Assert.AreEqual(VgcConstants.BltStatusOk, bus.Read((ushort)VgcConstants.BltStatus));
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
    public void Blitter_RotateMode_RotatesSquareIntoDestinationBuffer()
    {
        var bus = MakeBus();
        int src = 0x3600;
        int dst = 0x3700;
        for (int row = 0; row < 3; row++)
            for (int col = 0; col < 3; col++)
                bus.Write((ushort)(src + row * 3 + col), (byte)(row * 3 + col + 1));

        StartBlit(
            bus,
            srcSpace: VgcConstants.DmaSpaceCpuRam,
            dstSpace: VgcConstants.DmaSpaceCpuRam,
            srcAddr: src,
            dstAddr: dst,
            width: 3,
            height: 3,
            srcStride: 3,
            dstStride: 3,
            mode: VgcConstants.BltModeRotate,
            rotateAngle: 64);

        AssertBlitOk(bus, expectedCount: 9);
        CollectionAssert.AreEqual(
            new byte[] { 7, 4, 1, 8, 5, 2, 9, 6, 3 },
            new byte[]
            {
                bus.Read((ushort)dst),
                bus.Read((ushort)(dst + 1)),
                bus.Read((ushort)(dst + 2)),
                bus.Read((ushort)(dst + 3)),
                bus.Read((ushort)(dst + 4)),
                bus.Read((ushort)(dst + 5)),
                bus.Read((ushort)(dst + 6)),
                bus.Read((ushort)(dst + 7)),
                bus.Read((ushort)(dst + 8))
            });
    }

    [TestMethod]
    public void Blitter_RotateMode_WritesColorKeyForOutOfBoundsSamples()
    {
        var bus = MakeBus();
        int src = 0x3800;
        int dst = 0x3900;
        for (int row = 0; row < 5; row++)
            for (int col = 0; col < 5; col++)
                bus.Write((ushort)(src + row * 5 + col), (byte)(row * 5 + col + 1));

        StartBlit(
            bus,
            srcSpace: VgcConstants.DmaSpaceCpuRam,
            dstSpace: VgcConstants.DmaSpaceCpuRam,
            srcAddr: src,
            dstAddr: dst,
            width: 5,
            height: 5,
            srcStride: 5,
            dstStride: 5,
            mode: VgcConstants.BltModeRotate,
            colorKey: 0xFE,
            rotateAngle: 32);

        AssertBlitOk(bus, expectedCount: 25);
        Assert.AreEqual(0xFE, bus.Read((ushort)dst));
        Assert.AreEqual(0xFE, bus.Read((ushort)(dst + 4)));
        Assert.AreEqual(0xFE, bus.Read((ushort)(dst + 20)));
        Assert.AreEqual(0xFE, bus.Read((ushort)(dst + 24)));
        Assert.AreEqual(13, bus.Read((ushort)(dst + 12)));
    }

    [TestMethod]
    public void Blitter_RotateMode_RejectsNonSquareAndOverlappingRanges()
    {
        var bus = MakeBus();

        StartBlit(
            bus,
            srcSpace: VgcConstants.DmaSpaceCpuRam,
            dstSpace: VgcConstants.DmaSpaceCpuRam,
            srcAddr: 0x3A00,
            dstAddr: 0x3B00,
            width: 3,
            height: 2,
            srcStride: 3,
            dstStride: 3,
            mode: VgcConstants.BltModeRotate,
            rotateAngle: 64);

        Assert.AreEqual(VgcConstants.BltStatusError, bus.Read((ushort)VgcConstants.BltStatus));
        Assert.AreEqual(VgcConstants.BltErrBadArgs, bus.Read((ushort)VgcConstants.BltErrCode));

        StartBlit(
            bus,
            srcSpace: VgcConstants.DmaSpaceCpuRam,
            dstSpace: VgcConstants.DmaSpaceCpuRam,
            srcAddr: 0x3A00,
            dstAddr: 0x3B00,
            width: 257,
            height: 257,
            srcStride: 257,
            dstStride: 257,
            mode: VgcConstants.BltModeRotate,
            rotateAngle: 64);

        Assert.AreEqual(VgcConstants.BltStatusError, bus.Read((ushort)VgcConstants.BltStatus));
        Assert.AreEqual(VgcConstants.BltErrBadArgs, bus.Read((ushort)VgcConstants.BltErrCode));

        StartBlit(
            bus,
            srcSpace: VgcConstants.DmaSpaceCpuRam,
            dstSpace: VgcConstants.DmaSpaceCpuRam,
            srcAddr: 0x3A00,
            dstAddr: 0x3A01,
            width: 3,
            height: 3,
            srcStride: 3,
            dstStride: 3,
            mode: VgcConstants.BltModeRotate,
            rotateAngle: 64);

        Assert.AreEqual(VgcConstants.BltStatusError, bus.Read((ushort)VgcConstants.BltStatus));
        Assert.AreEqual(VgcConstants.BltErrBadArgs, bus.Read((ushort)VgcConstants.BltErrCode));
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

    // Nibble-granular color key (BltModeColorKey4): the gfx plane packs two
    // 4bpp pixels per byte, so byte-granular keying fringes every odd
    // opaque/transparent boundary. In this mode each nibble compares against
    // the key's low nibble independently; mixed bytes read-modify-write the
    // destination so the transparent pixel underneath survives.

    [TestMethod]
    public void Blitter_ColorKey4_SkipsTransparentNibbles()
    {
        var bus = MakeBus();

        int srcBase = 0x2400;
        bus.Write((ushort)(srcBase + 0), 0x51); // opaque 5, transparent 1
        bus.Write((ushort)(srcBase + 1), 0x15); // transparent 1, opaque 5
        bus.Write((ushort)(srcBase + 2), 0x11); // fully transparent
        bus.Write((ushort)(srcBase + 3), 0x55); // fully opaque

        int dstBase = 1024;
        for (int i = 0; i < 4; i++)
            bus.Vgc.TryWriteMemorySpace(VgcConstants.MemSpaceGfx, dstBase + i, 0x33);

        StartBlit(
            bus,
            srcSpace: VgcConstants.DmaSpaceCpuRam,
            dstSpace: VgcConstants.DmaSpaceVgcGfx,
            srcAddr: srcBase,
            dstAddr: dstBase,
            width: 4,
            height: 1,
            srcStride: 4,
            dstStride: VgcConstants.GfxWidth,
            mode: VgcConstants.BltModeColorKey4,
            colorKey: 0x01);

        AssertBlitOk(bus, expectedCount: 3); // 3 bytes carried opaque nibbles
        Assert.IsTrue(bus.Vgc.TryReadMemorySpace(VgcConstants.MemSpaceGfx, dstBase + 0, out byte b0));
        Assert.IsTrue(bus.Vgc.TryReadMemorySpace(VgcConstants.MemSpaceGfx, dstBase + 1, out byte b1));
        Assert.IsTrue(bus.Vgc.TryReadMemorySpace(VgcConstants.MemSpaceGfx, dstBase + 2, out byte b2));
        Assert.IsTrue(bus.Vgc.TryReadMemorySpace(VgcConstants.MemSpaceGfx, dstBase + 3, out byte b3));
        Assert.AreEqual(0x53, b0, "hi nibble from src, lo preserved");
        Assert.AreEqual(0x35, b1, "lo nibble from src, hi preserved");
        Assert.AreEqual(0x33, b2, "fully transparent byte untouched");
        Assert.AreEqual(0x55, b3, "fully opaque byte copied");
    }

    [TestMethod]
    public void Blitter_ColorKey4_CombinedWithRotate_IsBadArgs()
    {
        var bus = MakeBus();
        StartBlit(
            bus,
            srcSpace: VgcConstants.DmaSpaceCpuRam,
            dstSpace: VgcConstants.DmaSpaceVgcGfx,
            srcAddr: 0x2400,
            dstAddr: 0,
            width: 4,
            height: 4,
            srcStride: 4,
            dstStride: VgcConstants.GfxWidth,
            mode: (byte)(VgcConstants.BltModeColorKey4 | VgcConstants.BltModeRotate),
            colorKey: 0x01);

        Assert.AreEqual(VgcConstants.BltStatusError, bus.Read((ushort)VgcConstants.BltStatus));
        Assert.AreEqual(VgcConstants.BltErrBadArgs, bus.Read((ushort)VgcConstants.BltErrCode));
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
        byte colorKey = 0,
        byte rotateAngle = 0)
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
        bus.Write((ushort)VgcConstants.BltRotateAngle, rotateAngle);
        bus.Write((ushort)VgcConstants.BltCmd, VgcConstants.BltCmdStart);
    }

    private static void AssertBlitOk(CompositeBusDevice bus, int expectedCount)
    {
        RunUntilBlitNotBusy(bus);
        Assert.AreEqual(VgcConstants.BltStatusOk, bus.Read((ushort)VgcConstants.BltStatus));
        Assert.AreEqual(VgcConstants.BltErrNone, bus.Read((ushort)VgcConstants.BltErrCode));
        Assert.AreEqual(expectedCount, GetBlitCount(bus));
    }

    private static void RunUntilBlitNotBusy(CompositeBusDevice bus, int maxIterations = 100_000)
    {
        for (int i = 0; i < maxIterations; i++)
        {
            byte status = bus.Read((ushort)VgcConstants.BltStatus);
            if (status != VgcConstants.BltStatusBusy)
                return;
            bus.AdvanceCycles(16);
        }

        Assert.Fail("Blitter did not complete within allotted cycles.");
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

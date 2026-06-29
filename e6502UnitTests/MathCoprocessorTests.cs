using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class MathCoprocessorTests
{
    private readonly MathCoprocessor _math = new();

    [TestInitialize]
    public void Reset() => _math.Reset();

    [TestMethod]
    public void AbiRegistersExposeVersionCapabilitiesAndStatus()
    {
        byte expectedCaps = VgcConstants.MathCapMul16 |
                            VgcConstants.MathCapMulFx |
                            VgcConstants.MathCapSinCos |
                            VgcConstants.MathCapDistApprox |
                            VgcConstants.MathCapRng |
                            VgcConstants.MathCapDivS32_16 |
                            VgcConstants.MathCapAtan2 |
                            VgcConstants.MathCapSqrt;
        byte expectedCaps1 = VgcConstants.MathCap1VecDotS16 |
                             VgcConstants.MathCap1VecDotFx |
                             VgcConstants.MathCap1VecCrossS16 |
                             VgcConstants.MathCap1VecLen2 |
                             VgcConstants.MathCap1VecScaleFx;

        Assert.AreEqual(VgcConstants.MathVersionCurrent, _math.Read((ushort)VgcConstants.MathVersion));
        Assert.AreEqual(expectedCaps, _math.Read((ushort)VgcConstants.MathCaps0));
        Assert.AreEqual(expectedCaps1, _math.Read((ushort)VgcConstants.MathCaps1));
        Assert.AreEqual(VgcConstants.MathStatusOk, _math.Read((ushort)VgcConstants.MathStatus));

        _math.Write((ushort)VgcConstants.MathVersion, 0x99);
        _math.Write((ushort)VgcConstants.MathCaps0, 0);
        _math.Write((ushort)VgcConstants.MathStatus, 0xFF);

        Assert.AreEqual(VgcConstants.MathVersionCurrent, _math.Read((ushort)VgcConstants.MathVersion));
        Assert.AreEqual(expectedCaps, _math.Read((ushort)VgcConstants.MathCaps0));
        Assert.AreEqual(expectedCaps1, _math.Read((ushort)VgcConstants.MathCaps1));
        Assert.AreEqual(VgcConstants.MathStatusOk, _math.Read((ushort)VgcConstants.MathStatus));
    }

    [TestMethod]
    public void Mul16ProducesSigned32BitResult()
    {
        WriteWord(VgcConstants.MathMul16ALo, 300);
        WriteWord(VgcConstants.MathMul16BLo, unchecked((ushort)-7));

        Assert.AreEqual(0xFFFFF7CCu, ReadResult32());
        Assert.AreEqual(VgcConstants.MathStatusOk, _math.Read((ushort)VgcConstants.MathStatus));
    }

    [TestMethod]
    public void Mul16HandlesSignedMinimumEdge()
    {
        WriteWord(VgcConstants.MathMul16ALo, 0x8000);
        WriteWord(VgcConstants.MathMul16BLo, 0x8000);

        Assert.AreEqual(0x40000000u, ReadResult32());
    }

    [TestMethod]
    public void MulFxUsesSignedQ8Point8WithSaturation()
    {
        WriteWord(VgcConstants.MathMulFxALo, 0x0180);
        WriteWord(VgcConstants.MathMulFxBLo, 0x0180);
        Assert.AreEqual(0x0240, ReadResult16());
        Assert.AreEqual(VgcConstants.MathStatusOk, _math.Read((ushort)VgcConstants.MathStatus));

        WriteWord(VgcConstants.MathMulFxALo, 0x6400);
        WriteWord(VgcConstants.MathMulFxBLo, 0x0200);
        Assert.AreEqual(0x7FFF, ReadResult16());
        Assert.AreEqual(VgcConstants.MathStatusOverflow, _math.Read((ushort)VgcConstants.MathStatus));

        WriteWord(VgcConstants.MathMulFxALo, 0x9C00);
        WriteWord(VgcConstants.MathMulFxBLo, 0x0200);
        Assert.AreEqual(0x8000, ReadResult16());
        Assert.AreEqual(VgcConstants.MathStatusOverflow, _math.Read((ushort)VgcConstants.MathStatus));
    }

    [TestMethod]
    public void DistApproxHandlesSignedDeltas()
    {
        WriteWord(VgcConstants.MathDistDxLo, 3);
        WriteWord(VgcConstants.MathDistDyLo, 4);
        Assert.AreEqual(5, ReadResult16());
        Assert.AreEqual(VgcConstants.MathStatusOk, _math.Read((ushort)VgcConstants.MathStatus));

        WriteWord(VgcConstants.MathDistDxLo, unchecked((ushort)-10));
        WriteWord(VgcConstants.MathDistDyLo, 4);
        Assert.AreEqual(12, ReadResult16());
    }

    [TestMethod]
    public void SinCosReturnsSignedOnePointSevenAtExpectedPhases()
    {
        AssertSinCos(0, 0x00, 0x7F);
        AssertSinCos(32, 0x5B, 0x5B);
        AssertSinCos(64, 0x7F, 0x00);
        AssertSinCos(128, 0x00, 0x80);
        AssertSinCos(192, 0x80, 0x00);
    }

    [TestMethod]
    public void RngUsesSeededXorshiftSequence()
    {
        _math.Write((ushort)VgcConstants.MathRng, 0x78);
        _math.Write((ushort)VgcConstants.MathRng, 0x56);
        _math.Write((ushort)VgcConstants.MathRng, 0x34);
        _math.Write((ushort)VgcConstants.MathRng, 0x12);

        Assert.AreEqual(0xA5, _math.Read((ushort)VgcConstants.MathRng));
        Assert.AreEqual(VgcConstants.MathStatusOk, _math.Read((ushort)VgcConstants.MathStatus));
    }

    [TestMethod]
    public void DivS32ByS16ReturnsQuotientAndRemainder()
    {
        WriteDword(VgcConstants.MathDivNLo, unchecked((uint)-100));
        WriteWord(VgcConstants.MathDivDLo, 7);
        Assert.AreEqual(unchecked((ushort)-14), ReadResult16());
        Assert.AreEqual(unchecked((ushort)-2), ReadResultHi16());
        Assert.AreEqual(VgcConstants.MathStatusOk, _math.Read((ushort)VgcConstants.MathStatus));

        WriteDword(VgcConstants.MathDivNLo, 42);
        WriteWord(VgcConstants.MathDivDLo, 0);
        Assert.AreEqual(0xFFFFFFFFu, ReadResult32());
        Assert.AreEqual(VgcConstants.MathStatusDivZero, _math.Read((ushort)VgcConstants.MathStatus));

        WriteDword(VgcConstants.MathDivNLo, 1_000_000);
        WriteWord(VgcConstants.MathDivDLo, 1);
        Assert.AreEqual(0x7FFF, ReadResult16());
        Assert.AreEqual(VgcConstants.MathStatusOverflow, _math.Read((ushort)VgcConstants.MathStatus));
    }

    [TestMethod]
    public void Atan2ReturnsUnsignedAngleAndApproximateHypot()
    {
        AssertAtan2(0, 100, 0, 100);
        AssertAtan2(100, 0, 64, 100);
        AssertAtan2(0, -100, 128, 100);
        AssertAtan2(-100, 0, 192, 100);
        AssertAtan2(100, 100, 32, 138);
        AssertAtan2(-100, 100, 224, 138);
    }

    [TestMethod]
    public void VectorOpsHandleDotCrossLen2AndScaleFx()
    {
        WriteVector(0x0180, 0x0100, 0x0200, 0xFF00, 0x0080);
        _math.Write((ushort)VgcConstants.MathVecOp, VgcConstants.MathVecOpDotFx);
        Assert.AreEqual(0x0200, ReadResult16());
        Assert.AreEqual(VgcConstants.MathStatusOk, _math.Read((ushort)VgcConstants.MathStatus));

        _math.Write((ushort)VgcConstants.MathVecOp, VgcConstants.MathVecOpDotS16);
        Assert.AreEqual(0x00020000u, ReadResult32());

        _math.Write((ushort)VgcConstants.MathVecOp, VgcConstants.MathVecOpCrossS16);
        Assert.AreEqual(0xFFFC8000u, ReadResult32());

        _math.Write((ushort)VgcConstants.MathVecOp, VgcConstants.MathVecOpLen2);
        Assert.AreEqual(0x00034000u, ReadResult32());

        _math.Write((ushort)VgcConstants.MathVecOp, VgcConstants.MathVecOpScaleFx);
        Assert.AreEqual(0x00C0, ReadResult16());
        Assert.AreEqual(0x0080, ReadResultHi16());
    }

    [TestMethod]
    public void SqrtComputesFloorIntegerRoot()
    {
        (ushort input, ushort expected)[] cases =
        [
            (0, 0),
            (1, 1),
            (9, 3),
            (15, 3),
            (16, 4),
            (144, 12),
            (65535, 255),
        ];

        foreach ((ushort input, ushort expected) in cases)
        {
            WriteWord(VgcConstants.MathSqrtLo, input);
            Assert.AreEqual(expected, ReadResult16(), $"SQRT {input} should be {expected}");
            Assert.AreEqual(VgcConstants.MathStatusOk, _math.Read((ushort)VgcConstants.MathStatus),
                $"SQRT {input} should leave STATUS ok");
        }
    }

    [TestMethod]
    public void Caps0AdvertisesSqrt()
    {
        byte caps0 = _math.Read((ushort)VgcConstants.MathCaps0);
        Assert.AreEqual(VgcConstants.MathCapSqrt,
            (byte)(caps0 & VgcConstants.MathCapSqrt), "CAPS0 must advertise the SQRT capability bit.");
        Assert.AreEqual(0xFF, caps0, "With SQRT added, all eight CAPS0 scalar-op bits are now set.");
    }

    [TestMethod]
    public void UnknownVectorOpReturnsUnimplementedStatus()
    {
        _math.Write((ushort)VgcConstants.MathVecOp, 0xFF);
        Assert.AreEqual(0xFFFFFFFFu, ReadResult32());
        Assert.AreEqual(VgcConstants.MathStatusUnimpl, _math.Read((ushort)VgcConstants.MathStatus));
    }


    [TestMethod]
    public void CompositeBusRoutesMathWithoutRamShadowing()
    {
        using var bus = new CompositeBusDevice(enableSound: false);

        bus.Write((ushort)VgcConstants.MathMul16ALo, 0x2C);
        bus.Write((ushort)VgcConstants.MathMul16AHi, 0x01);
        bus.Write((ushort)VgcConstants.MathMul16BLo, 0xF9);
        bus.Write((ushort)VgcConstants.MathMul16BHi, 0xFF);

        Assert.AreEqual(0xCC, bus.Read((ushort)VgcConstants.MathRes0));
        Assert.AreEqual(0xF7, bus.Read((ushort)VgcConstants.MathRes1));
        Assert.AreEqual(0xFF, bus.Read((ushort)VgcConstants.MathRes2));
        Assert.AreEqual(0xFF, bus.Read((ushort)VgcConstants.MathRes3));
    }

    private void AssertSinCos(byte angle, byte expectedSin, byte expectedCos)
    {
        _math.Write((ushort)VgcConstants.MathSinCosAngle, angle);
        Assert.AreEqual(expectedSin, _math.Read((ushort)VgcConstants.MathRes0));
        Assert.AreEqual(expectedCos, _math.Read((ushort)VgcConstants.MathRes1));
        Assert.AreEqual(0, _math.Read((ushort)VgcConstants.MathRes2));
        Assert.AreEqual(0, _math.Read((ushort)VgcConstants.MathRes3));
    }

    private void WriteWord(int address, ushort value)
    {
        _math.Write((ushort)address, (byte)value);
        _math.Write((ushort)(address + 1), (byte)(value >> 8));
    }

    private void WriteDword(int address, uint value)
    {
        _math.Write((ushort)address, (byte)value);
        _math.Write((ushort)(address + 1), (byte)(value >> 8));
        _math.Write((ushort)(address + 2), (byte)(value >> 16));
        _math.Write((ushort)(address + 3), (byte)(value >> 24));
    }

    private void WriteVector(ushort ax, ushort ay, ushort bx, ushort by, ushort scalar)
    {
        WriteWord(VgcConstants.MathVecAxLo, ax);
        WriteWord(VgcConstants.MathVecAyLo, ay);
        WriteWord(VgcConstants.MathVecBxLo, bx);
        WriteWord(VgcConstants.MathVecByLo, by);
        WriteWord(VgcConstants.MathVecScalarLo, scalar);
    }

    private void AssertAtan2(short dy, short dx, byte expectedAngle, ushort expectedHypot)
    {
        WriteWord(VgcConstants.MathAtanDyLo, unchecked((ushort)dy));
        WriteWord(VgcConstants.MathAtanDxLo, unchecked((ushort)dx));
        Assert.AreEqual(expectedAngle, _math.Read((ushort)VgcConstants.MathRes0));
        Assert.AreEqual(expectedHypot, ReadResultHi24());
        Assert.AreEqual(VgcConstants.MathStatusOk, _math.Read((ushort)VgcConstants.MathStatus));
    }

    private ushort ReadResult16() =>
        (ushort)(_math.Read((ushort)VgcConstants.MathRes0) |
                 (_math.Read((ushort)VgcConstants.MathRes1) << 8));

    private ushort ReadResultHi16() =>
        (ushort)(_math.Read((ushort)VgcConstants.MathRes2) |
                 (_math.Read((ushort)VgcConstants.MathRes3) << 8));

    private ushort ReadResultHi24() =>
        (ushort)(_math.Read((ushort)VgcConstants.MathRes1) |
                 (_math.Read((ushort)VgcConstants.MathRes2) << 8));

    private uint ReadResult32() =>
        (uint)(_math.Read((ushort)VgcConstants.MathRes0) |
              (_math.Read((ushort)VgcConstants.MathRes1) << 8) |
              (_math.Read((ushort)VgcConstants.MathRes2) << 16) |
              (_math.Read((ushort)VgcConstants.MathRes3) << 24));
}

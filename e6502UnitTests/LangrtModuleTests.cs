using System;
using System.IO;
using e6502.Avalonia.Hardware;
using KDS.e6502;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

/// <summary>
/// Verifies the shared LANGRT module through the same standard $C000 dispatch
/// contract used by every other paged NDK module. These tests intentionally call
/// the module directly; loader paging is covered independently by ShelfMissTests.
/// </summary>
[TestClass]
public class LangrtModuleTests
{
    private const ushort FnId = 0x0301;
    private const ushort Status = 0x0302;
    private const ushort Arg0 = 0x0303;
    private const ushort Arg1 = 0x0307;
    private const ushort Arg2 = 0x030B;
    private const ushort Arg3 = 0x030F;
    private const ushort Result = 0x0313;
    private const ushort Output = 0x1000;
    private const ushort Right = 0x1137;
    private const ushort Sentinel = 0xFFF9;

    private const byte RealFromByte = 0x00;
    private const byte RealAdd = 0x03;
    private const byte RealCompare = 0x05;
    private const byte RealMultiplyWordLeft = 0x07;
    private const byte RealSqrt = 0x14;
    private const byte RealCos = 0x17;
    private const byte StrRealPrecision = 0x12;
    private const byte RealTrunc = 0x1B;
    private const byte RealVal = 0x1C;
    private const byte RealRound = 0x1D;
    private const byte LongFromUWord = 0x1F;
    private const byte LongFromInteger = 0x20;
    private const byte LongAdd = 0x22;
    private const byte LongCompare = 0x24;
    private const byte LongMultiply = 0x25;
    private const byte LongDivide = 0x26;
    private const byte LongModulo = 0x27;

    [TestMethod]
    public void LangrtModule_Header_IsWellFormed()
    {
        byte[] image = File.ReadAllBytes(RepoPath("software", "modules", "langrt", "langrt.bin"));

        Assert.AreEqual(16384, image.Length, "module image must be exactly 16 KB");
        Assert.AreEqual(0x4C, image[0], "module entry must be JMP dispatch");
        Assert.AreEqual((byte)'N', image[3]);
        Assert.AreEqual((byte)'L', image[4]);
        Assert.AreEqual(0x09, image[5], "MODULE_ID_LANGRT");
        Assert.AreEqual(0x01, image[6], "LIB_ABI_VERSION");
        Assert.AreEqual(0x29, image[7], "LANGRT_FN_COUNT");
    }

    [TestMethod]
    public void RealFromByte_WritesCallerOwnedQ16Result()
    {
        using var bus = MakeBus();
        SetCell(bus, Arg0, 42);

        RunFn(bus, RealFromByte);

        CollectionAssert.AreEqual(
            new byte[] { 0x00, 0x00, 0x2A, 0x00 },
            ReadBytes(bus, Result, 4),
            "42 must be returned as signed 16.16 in the canonical result cell");
    }

    [TestMethod]
    public void RealAdd_UsesOnlyDocumentedMailboxOperands()
    {
        using var bus = MakeBus();
        WriteBytes(bus, Arg1, 0x00, 0x80, 0x01, 0x00); // 1.5
        WriteBytes(bus, Right, 0x00, 0x40, 0x02, 0x00); // 2.25
        SetCell(bus, Arg0, Right);

        RunFn(bus, RealAdd);

        CollectionAssert.AreEqual(
            new byte[] { 0x00, 0xC0, 0x03, 0x00 },
            ReadBytes(bus, Result, 4),
            "the shared runtime must add Q16.16 values without Pascal-owned storage");
    }

    [TestMethod]
    public void LongIntServices_ShareTheFourByteScalarAbi()
    {
        using var bus = MakeBus();

        SetCell(bus, Arg0, 0xFFFF);
        RunFn(bus, LongFromUWord);
        CollectionAssert.AreEqual(new byte[] { 0xFF, 0xFF, 0x00, 0x00 }, ReadBytes(bus, Result, 4));

        RunFn(bus, LongFromInteger);
        CollectionAssert.AreEqual(new byte[] { 0xFF, 0xFF, 0xFF, 0xFF }, ReadBytes(bus, Result, 4));

        WriteBytes(bus, Arg1, 0x70, 0x11, 0x01, 0x00); // 70000
        WriteBytes(bus, Right, 0x05, 0x00, 0x00, 0x00);
        SetCell(bus, Arg0, Right);
        RunFn(bus, LongAdd);
        CollectionAssert.AreEqual(new byte[] { 0x75, 0x11, 0x01, 0x00 }, ReadBytes(bus, Result, 4));

        WriteBytes(bus, Arg1, 0xFF, 0xFF, 0xFF, 0xFF);
        WriteBytes(bus, Right, 0x01, 0x00, 0x00, 0x00);
        RunFn(bus, LongCompare);
        byte flags = bus.ReadRam((ushort)(Result + 2));
        Assert.AreEqual(0, flags & 0x01, "signed -1 must compare below +1");
        Assert.AreEqual(0, flags & 0x02, "different LongInt values must clear zero");
    }

    [TestMethod]
    public void LongMultiply_KeepsLowThirtyTwoBitsForEitherSign()
    {
        using var bus = MakeBus();

        // 100000 * 20 = 2000000
        WriteBytes(bus, Arg1, 0xA0, 0x86, 0x01, 0x00);
        WriteBytes(bus, Right, 0x14, 0x00, 0x00, 0x00);
        SetCell(bus, Arg0, Right);
        RunFn(bus, LongMultiply);
        CollectionAssert.AreEqual(new byte[] { 0x80, 0x84, 0x1E, 0x00 }, ReadBytes(bus, Result, 4));

        // -3 * 5 = -15: the low 32 bits are sign-agnostic.
        WriteBytes(bus, Arg1, 0xFD, 0xFF, 0xFF, 0xFF);
        WriteBytes(bus, Right, 0x05, 0x00, 0x00, 0x00);
        SetCell(bus, Arg0, Right);
        RunFn(bus, LongMultiply);
        CollectionAssert.AreEqual(new byte[] { 0xF1, 0xFF, 0xFF, 0xFF }, ReadBytes(bus, Result, 4));

        // Both cross terms must contribute: 65536 * 65536 wraps to zero.
        WriteBytes(bus, Arg1, 0x00, 0x00, 0x01, 0x00);
        WriteBytes(bus, Right, 0x00, 0x00, 0x01, 0x00);
        SetCell(bus, Arg0, Right);
        RunFn(bus, LongMultiply);
        CollectionAssert.AreEqual(new byte[] { 0x00, 0x00, 0x00, 0x00 }, ReadBytes(bus, Result, 4));

        // 65537 * 3 exercises the low product and both cross terms together.
        WriteBytes(bus, Arg1, 0x01, 0x00, 0x01, 0x00);
        WriteBytes(bus, Right, 0x03, 0x00, 0x00, 0x00);
        SetCell(bus, Arg0, Right);
        RunFn(bus, LongMultiply);
        CollectionAssert.AreEqual(new byte[] { 0x03, 0x00, 0x03, 0x00 }, ReadBytes(bus, Result, 4));
    }

    [TestMethod]
    public void LongDivideAndModulo_TruncateTowardZeroWithDividendSignedRemainder()
    {
        using var bus = MakeBus();

        // 2000000 div 7 = 285714 remainder 2
        WriteBytes(bus, Arg1, 0x80, 0x84, 0x1E, 0x00);
        WriteBytes(bus, Right, 0x07, 0x00, 0x00, 0x00);
        SetCell(bus, Arg0, Right);
        RunFn(bus, LongDivide);
        CollectionAssert.AreEqual(new byte[] { 0x12, 0x5C, 0x04, 0x00 }, ReadBytes(bus, Result, 4));

        WriteBytes(bus, Arg1, 0x80, 0x84, 0x1E, 0x00);
        SetCell(bus, Arg0, Right);
        RunFn(bus, LongModulo);
        CollectionAssert.AreEqual(new byte[] { 0x02, 0x00, 0x00, 0x00 }, ReadBytes(bus, Result, 4));

        // -7 div 2 = -3 and -7 mod 2 = -1: truncation toward zero leaves the
        // remainder carrying the dividend's sign, as in Turbo.
        WriteBytes(bus, Arg1, 0xF9, 0xFF, 0xFF, 0xFF);
        WriteBytes(bus, Right, 0x02, 0x00, 0x00, 0x00);
        SetCell(bus, Arg0, Right);
        RunFn(bus, LongDivide);
        CollectionAssert.AreEqual(new byte[] { 0xFD, 0xFF, 0xFF, 0xFF }, ReadBytes(bus, Result, 4));

        WriteBytes(bus, Arg1, 0xF9, 0xFF, 0xFF, 0xFF);
        SetCell(bus, Arg0, Right);
        RunFn(bus, LongModulo);
        CollectionAssert.AreEqual(new byte[] { 0xFF, 0xFF, 0xFF, 0xFF }, ReadBytes(bus, Result, 4));

        // 7 div -2 = -3, remainder keeps the positive dividend's sign.
        WriteBytes(bus, Arg1, 0x07, 0x00, 0x00, 0x00);
        WriteBytes(bus, Right, 0xFE, 0xFF, 0xFF, 0xFF);
        SetCell(bus, Arg0, Right);
        RunFn(bus, LongDivide);
        CollectionAssert.AreEqual(new byte[] { 0xFD, 0xFF, 0xFF, 0xFF }, ReadBytes(bus, Result, 4));

        WriteBytes(bus, Arg1, 0x07, 0x00, 0x00, 0x00);
        SetCell(bus, Arg0, Right);
        RunFn(bus, LongModulo);
        CollectionAssert.AreEqual(new byte[] { 0x01, 0x00, 0x00, 0x00 }, ReadBytes(bus, Result, 4));

        // A divisor larger than the dividend yields a zero quotient and passes
        // the dividend through as the remainder.
        WriteBytes(bus, Arg1, 0x05, 0x00, 0x00, 0x00);
        WriteBytes(bus, Right, 0x00, 0x00, 0x01, 0x00);
        SetCell(bus, Arg0, Right);
        RunFn(bus, LongDivide);
        CollectionAssert.AreEqual(new byte[] { 0x00, 0x00, 0x00, 0x00 }, ReadBytes(bus, Result, 4));

        WriteBytes(bus, Arg1, 0x05, 0x00, 0x00, 0x00);
        SetCell(bus, Arg0, Right);
        RunFn(bus, LongModulo);
        CollectionAssert.AreEqual(new byte[] { 0x05, 0x00, 0x00, 0x00 }, ReadBytes(bus, Result, 4));

        // A remainder whose doubled value overflows 32 bits must still divide:
        // $FFFFFFFF div $80000000 is 1 remainder $7FFFFFFF when both are read
        // as magnitudes.
        WriteBytes(bus, Arg1, 0xFF, 0xFF, 0xFF, 0x7F);
        WriteBytes(bus, Right, 0x00, 0x00, 0x00, 0x40);
        SetCell(bus, Arg0, Right);
        RunFn(bus, LongDivide);
        CollectionAssert.AreEqual(new byte[] { 0x01, 0x00, 0x00, 0x00 }, ReadBytes(bus, Result, 4));
    }

    [TestMethod]
    public void RealCompare_PublishesOrderingFlagsBeforeLibCallRestoresItsStatus()
    {
        using var bus = MakeBus();
        WriteBytes(bus, Arg1, 0x00, 0x00, 0x01, 0x00); // 1.0
        WriteBytes(bus, Right, 0x00, 0x00, 0x02, 0x00); // 2.0
        SetCell(bus, Arg0, Right);

        RunFn(bus, RealCompare);

        byte flags = bus.ReadRam((ushort)(Result + 2));
        Assert.AreEqual(0, flags & 0x01, "1.0 < 2.0 must clear carry");
        Assert.AreEqual(0, flags & 0x02, "1.0 < 2.0 must clear zero");

        WriteBytes(bus, Arg1, 0x00, 0x00, 0x02, 0x00);
        RunFn(bus, RealCompare);
        flags = bus.ReadRam((ushort)(Result + 2));
        Assert.AreEqual(1, flags & 0x01, "equal values must set carry");
        Assert.AreEqual(0x02, flags & 0x02, "equal values must set zero");
    }

    [TestMethod]
    public void RealMultiplyWordLeft_UsesMailboxOrdinalAndRealOperands()
    {
        using var bus = MakeBus();
        WriteBytes(bus, Arg1, 0x02, 0x00, 0x00, 0x00); // 2
        WriteBytes(bus, Right, 0x00, 0x00, 0x03, 0x00); // 3.0
        SetCell(bus, Arg0, Right);
        WriteBytes(bus, (ushort)(Arg0 + 2), 0xA5, 0x5A); // outside ptr16 width

        RunFn(bus, RealMultiplyWordLeft);

        CollectionAssert.AreEqual(
            new byte[] { 0x00, 0x00, 0x06, 0x00 },
            ReadBytes(bus, Result, 4));
    }

    [TestMethod]
    public void RealSqrt_PreservesQ16FractionalInput()
    {
        using var bus = MakeBus();
        WriteBytes(bus, Right, 0x00, 0x40, 0x02, 0x00); // 2.25
        SetCell(bus, Arg0, Right);

        RunFn(bus, RealSqrt);

        CollectionAssert.AreEqual(
            new byte[] { 0x00, 0x80, 0x01, 0x00 },
            ReadBytes(bus, Result, 4),
            "hardware integer SQRT must be scaled as Q16.16 instead of discarding the fraction");
    }

    [TestMethod]
    public void RealCos_UsesTheFullMailboxPointer()
    {
        using var bus = MakeBus();
        const ushort other = 0x1249;
        WriteBytes(bus, Right, 0x00, 0x00, 0x00, 0x00);
        WriteBytes(bus, other, 0x00, 0x00, 0x00, 0x00);
        WriteBytes(bus, 0x1101, 0x00, 0x00, 0x48, 0x06);
        WriteBytes(bus, 0x1201, 0x00, 0x00, 0x00, 0x00);

        SetCell(bus, Arg0, Right);
        RunFn(bus, RealCos);
        byte[] first = ReadBytes(bus, Result, 4);

        SetCell(bus, Arg0, other);
        RunFn(bus, RealCos);
        CollectionAssert.AreEqual(first, ReadBytes(bus, Result, 4),
            "COS must not replace the low byte of its Q16.16 operand pointer with its mode selector.");
    }

    [TestMethod]
    public void RealTrunc_ReturnsTheWholeSignedInteger()
    {
        using var bus = MakeBus();
        WriteBytes(bus, Right, 0x00, 0xC0, 0x03, 0x00); // 3.75
        SetCell(bus, Arg0, Right);

        RunFn(bus, RealTrunc);

        CollectionAssert.AreEqual(new byte[] { 0x03, 0x00 }, ReadBytes(bus, Result, 2));
    }

    [TestMethod]
    public void RealRound_RoundsHalvesAwayFromZero()
    {
        using var bus = MakeBus();
        SetCell(bus, Arg0, Right);

        WriteBytes(bus, Right, 0x00, 0x80, 0x01, 0x00); // 1.5
        RunFn(bus, RealRound);
        CollectionAssert.AreEqual(new byte[] { 0x02, 0x00 }, ReadBytes(bus, Result, 2));

        WriteBytes(bus, Right, 0x00, 0x80, 0xFE, 0xFF); // -1.5
        RunFn(bus, RealRound);
        CollectionAssert.AreEqual(new byte[] { 0xFE, 0xFF }, ReadBytes(bus, Result, 2));
    }

    [TestMethod]
    public void StrRealPrecision_UsesOnlyTheDocumentedMailboxArguments()
    {
        using var bus = MakeBus();
        WriteBytes(bus, Right, 0x00, 0xC0, 0x03, 0x00); // 3.75
        SetCell(bus, Arg0, Output);
        SetCell(bus, Arg1, Right);
        SetCell(bus, Arg2, 0); // minimum width
        SetCell(bus, Arg3, 2); // precision

        RunFn(bus, StrRealPrecision);

        CollectionAssert.AreEqual(
            new byte[] { 4, (byte)'3', (byte)'.', (byte)'7', (byte)'5' },
            ReadBytes(bus, Output, 5));
    }

    [TestMethod]
    public void RealVal_UsesOnlyTheDocumentedMailboxArguments()
    {
        using var bus = MakeBus();
        ushort value = Output;
        ushort code = (ushort)(Output + 4);
        WriteBytes(bus, Right, 4, (byte)'4', (byte)'.', (byte)'5', (byte)'0');
        SetCell(bus, Arg0, Right);
        SetCell(bus, Arg1, value);
        SetCell(bus, Arg2, code);
        SetCell(bus, Arg3, 4);

        RunFn(bus, RealVal);

        byte[] parsed = ReadBytes(bus, value, 4);
        CollectionAssert.AreEqual(new byte[] { 0x00, 0x80, 0x04, 0x00 }, parsed,
            $"VAL produced {Convert.ToHexString(parsed)} instead of signed 16.16 4.50");
        CollectionAssert.AreEqual(new byte[] { 0x00, 0x00 }, ReadBytes(bus, code, 2));
    }

    [TestMethod]
    public void RealVal_AccumulatesMultipleDecimalDigitsInBaseTen()
    {
        using var bus = MakeBus();
        ushort value = Output;
        ushort code = (ushort)(Output + 4);
        WriteBytes(bus, Right, 2, (byte)'6', (byte)'9');
        SetCell(bus, Arg0, Right);
        SetCell(bus, Arg1, value);
        SetCell(bus, Arg2, code);
        SetCell(bus, Arg3, 4);

        RunFn(bus, RealVal);

        CollectionAssert.AreEqual(new byte[] { 0x00, 0x00, 0x45, 0x00 }, ReadBytes(bus, value, 4),
            "A new decimal digit must multiply the accumulated value by ten, not six.");
        CollectionAssert.AreEqual(new byte[] { 0x00, 0x00 }, ReadBytes(bus, code, 2));
    }

    private static CompositeBusDevice MakeBus()
    {
        var bus = new CompositeBusDevice(enableSound: false);
        bus.LoadExtensionRomBytesForTest(
            File.ReadAllBytes(RepoPath("software", "modules", "langrt", "langrt.bin")));
        bus.Write(VgcConstants.RegRomSwap, VgcConstants.RomSwapExtension);
        return bus;
    }

    private static void RunFn(CompositeBusDevice bus, byte fn)
    {
        bus.WriteRam(FnId, fn);
        bus.WriteRam(Status, 0xFF);
        bus.WriteRam(0x01FF, (byte)((Sentinel - 1) >> 8));
        bus.WriteRam(0x01FE, (byte)((Sentinel - 1) & 0xFF));

        var cpu = new Cpu(bus, E6502Type.Cmos);
        CpuState state = cpu.GetState();
        cpu.RestoreState(new CpuState(
            state.A, state.X, state.Y, 0xFD, 0xC000,
            state.Nf, state.Vf, state.Df, true, state.Zf, state.Cf));

        for (int guard = 0; guard < 4_000_000 && cpu.Pc != Sentinel; guard++)
            cpu.ExecuteNext();

        Assert.AreEqual(Sentinel, cpu.Pc, $"LANGRT fn ${fn:X2} did not return through the standard module frame");
        Assert.AreEqual(0, bus.ReadRam(Status), $"LANGRT fn ${fn:X2} must report LERR_OK");
    }

    private static void SetCell(CompositeBusDevice bus, ushort address, int value)
    {
        for (int i = 0; i < 4; i++)
            bus.WriteRam((ushort)(address + i), (byte)(value >> (8 * i)));
    }

    private static void WriteBytes(CompositeBusDevice bus, ushort address, params byte[] values)
    {
        for (int i = 0; i < values.Length; i++)
            bus.WriteRam((ushort)(address + i), values[i]);
    }

    private static byte[] ReadBytes(CompositeBusDevice bus, ushort address, int count)
    {
        var values = new byte[count];
        for (int i = 0; i < count; i++)
            values[i] = bus.ReadRam((ushort)(address + i));
        return values;
    }

    private static string RepoPath(params string[] parts)
    {
        string? dir = Directory.GetCurrentDirectory();
        while (dir != null)
        {
            if (File.Exists(Path.Combine(dir, "software", "runtime", "asm", "libabi.inc")))
                return Path.Combine([dir, .. parts]);
            dir = Directory.GetParent(dir)?.FullName;
        }
        throw new DirectoryNotFoundException("Could not locate repository root.");
    }
}

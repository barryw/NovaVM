namespace e6502.Avalonia.Hardware;

/// <summary>
/// Software mirror of the FPGA math coprocessor register bank.
/// </summary>
public sealed class MathCoprocessor
{
    private const uint RngDefault = 0xDEADBEEF;

    private static readonly byte[] SinQuarterMagnitudes =
    [
        0, 3, 6, 9, 13, 16, 19, 22, 25, 28, 31, 34, 37,
        40, 43, 46, 49, 52, 55, 58, 60, 63, 66, 68, 71, 74,
        76, 79, 81, 84, 86, 88, 91, 93, 95, 97, 99, 101, 103,
        105, 106, 108, 110, 111, 113, 114, 116, 117, 118, 119,
        121, 122, 122, 123, 124, 125, 126, 126, 127, 127, 127,
        128, 128, 128, 128
    ];

    private readonly byte[] _regs = new byte[VgcConstants.MathEnd - VgcConstants.MathBase + 1];
    private uint _rngState = RngDefault;
    private int _rngSeedIndex;

    public bool OwnsAddress(ushort address) =>
        address >= VgcConstants.MathBase && address <= VgcConstants.MathEnd;

    public byte Read(ushort address)
    {
        int offset = address - VgcConstants.MathBase;
        if (address == VgcConstants.MathRng)
        {
            _rngState = NextRng(_rngState);
            _regs[VgcConstants.MathStatus - VgcConstants.MathBase] = VgcConstants.MathStatusOk;
            return (byte)_rngState;
        }

        return address switch
        {
            VgcConstants.MathCaps0 => VgcConstants.MathCapMul16 |
                                      VgcConstants.MathCapMulFx |
                                      VgcConstants.MathCapSinCos |
                                      VgcConstants.MathCapDistApprox |
                                      VgcConstants.MathCapRng |
                                      VgcConstants.MathCapDivS32_16 |
                                      VgcConstants.MathCapAtan2 |
                                      VgcConstants.MathCapSqrt,
            VgcConstants.MathCaps1 => VgcConstants.MathCap1VecDotS16 |
                                      VgcConstants.MathCap1VecDotFx |
                                      VgcConstants.MathCap1VecCrossS16 |
                                      VgcConstants.MathCap1VecLen2 |
                                      VgcConstants.MathCap1VecScaleFx,
            VgcConstants.MathStatus => _regs[offset],
            VgcConstants.MathVersion => VgcConstants.MathVersionCurrent,
            _ => _regs[offset]
        };
    }

    public void Reset()
    {
        Array.Clear(_regs);
        _rngState = RngDefault;
        _rngSeedIndex = 0;
    }

    public void Write(ushort address, byte data)
    {
        int offset = address - VgcConstants.MathBase;
        if (address is VgcConstants.MathCaps0 or
            VgcConstants.MathCaps1 or
            VgcConstants.MathStatus or
            VgcConstants.MathVersion)
            return;

        _regs[offset] = data;

        if (address == VgcConstants.MathMul16BHi)
            Mul16(data);
        else if (address == VgcConstants.MathMulFxBHi)
            MulFx(data);
        else if (address == VgcConstants.MathDistDyHi)
            DistApprox(data);
        else if (address == VgcConstants.MathSinCosAngle)
            SinCos(data);
        else if (address == VgcConstants.MathRng)
            SeedRng(data);
        else if (address == VgcConstants.MathDivDHi)
            DivS32ByS16(data);
        else if (address == VgcConstants.MathAtanDxHi)
            Atan2(data);
        else if (address == VgcConstants.MathSqrtHi)
            Sqrt(data);
        else if (address == VgcConstants.MathVecOp)
            VectorOp(data);
    }

    private void Mul16(byte bHi)
    {
        short a = ReadInt16(VgcConstants.MathMul16ALo);
        short b = (short)((bHi << 8) | _regs[VgcConstants.MathMul16BLo - VgcConstants.MathBase]);
        SetResult(a * b);
    }

    private void MulFx(byte bHi)
    {
        short a = ReadInt16(VgcConstants.MathMulFxALo);
        short b = (short)((bHi << 8) | _regs[VgcConstants.MathMulFxBLo - VgcConstants.MathBase]);
        int shifted = (a * b) >> 8;
        short result = shifted switch
        {
            > short.MaxValue => short.MaxValue,
            < short.MinValue => short.MinValue,
            _ => (short)shifted
        };
        byte status = shifted is > short.MaxValue or < short.MinValue
            ? VgcConstants.MathStatusOverflow
            : VgcConstants.MathStatusOk;

        SetResult16((ushort)result, status);
    }

    private void DistApprox(byte dyHi)
    {
        short dx = ReadInt16(VgcConstants.MathDistDxLo);
        short dy = (short)((dyHi << 8) | _regs[VgcConstants.MathDistDyLo - VgcConstants.MathBase]);
        int ax = AbsS16(dx);
        int ay = AbsS16(dy);
        int max = Math.Max(ax, ay);
        int min = Math.Min(ax, ay);
        int approx = max + (min >> 1) - (min >> 3);
        SetResult16((ushort)Math.Min(approx, ushort.MaxValue),
            approx > ushort.MaxValue ? VgcConstants.MathStatusOverflow : VgcConstants.MathStatusOk);
    }

    private void SinCos(byte angle)
    {
        _regs[VgcConstants.MathRes0 - VgcConstants.MathBase] = Sin1P7(angle);
        _regs[VgcConstants.MathRes1 - VgcConstants.MathBase] = Sin1P7((byte)(angle + 64));
        _regs[VgcConstants.MathRes2 - VgcConstants.MathBase] = 0;
        _regs[VgcConstants.MathRes3 - VgcConstants.MathBase] = 0;
        _regs[VgcConstants.MathStatus - VgcConstants.MathBase] = VgcConstants.MathStatusOk;
    }

    private void DivS32ByS16(byte dHi)
    {
        int numerator = ReadInt32(VgcConstants.MathDivNLo);
        short denominator = (short)((dHi << 8) | _regs[VgcConstants.MathDivDLo - VgcConstants.MathBase]);
        if (denominator == 0)
        {
            SetResult(unchecked((int)0xFFFFFFFF), VgcConstants.MathStatusDivZero);
            return;
        }

        long quotient = numerator / denominator;
        long remainder = numerator % denominator;
        byte status = VgcConstants.MathStatusOk;
        if (quotient > short.MaxValue)
        {
            quotient = short.MaxValue;
            remainder = 0;
            status = VgcConstants.MathStatusOverflow;
        }
        else if (quotient < short.MinValue)
        {
            quotient = short.MinValue;
            remainder = 0;
            status = VgcConstants.MathStatusOverflow;
        }

        SetResultWords((ushort)(short)quotient, (ushort)(short)remainder, status);
    }

    private void Atan2(byte dxHi)
    {
        short dy = ReadInt16(VgcConstants.MathAtanDyLo);
        short dx = (short)((dxHi << 8) | _regs[VgcConstants.MathAtanDxLo - VgcConstants.MathBase]);
        byte angle = Atan2CordicAngle(dy, dx);
        ushort hypot = DistApproxValue(dx, dy);

        _regs[VgcConstants.MathRes0 - VgcConstants.MathBase] = angle;
        _regs[VgcConstants.MathRes1 - VgcConstants.MathBase] = (byte)hypot;
        _regs[VgcConstants.MathRes2 - VgcConstants.MathBase] = (byte)(hypot >> 8);
        _regs[VgcConstants.MathRes3 - VgcConstants.MathBase] = 0;
        _regs[VgcConstants.MathStatus - VgcConstants.MathBase] = VgcConstants.MathStatusOk;
    }

    private void Sqrt(byte hi)
    {
        // Unsigned 16-bit floor integer sqrt (non-restoring, bit-by-bit).
        // MUST stay bit-identical with math_copro.sv's OP_SQRT FSM.
        int n = _regs[VgcConstants.MathSqrtLo - VgcConstants.MathBase] | (hi << 8);
        int res = 0;
        int bit = 1 << 14;          // 4^7, highest power of 4 <= 65535
        while (bit > n)
            bit >>= 2;
        while (bit != 0)
        {
            if (n >= res + bit)
            {
                n -= res + bit;
                res = (res >> 1) + bit;
            }
            else
            {
                res >>= 1;
            }
            bit >>= 2;
        }

        SetResult16((ushort)res, VgcConstants.MathStatusOk);
    }

    private void VectorOp(byte op)
    {
        short ax = ReadInt16(VgcConstants.MathVecAxLo);
        short ay = ReadInt16(VgcConstants.MathVecAyLo);
        short bx = ReadInt16(VgcConstants.MathVecBxLo);
        short by = ReadInt16(VgcConstants.MathVecByLo);
        short scalar = ReadInt16(VgcConstants.MathVecScalarLo);

        switch (op)
        {
            case VgcConstants.MathVecOpDotS16:
                SetResultSaturating32((long)ax * bx + (long)ay * by);
                break;
            case VgcConstants.MathVecOpDotFx:
                SetResult16(SaturateS16(((long)ax * bx + (long)ay * by) >> 8, out byte dotStatus), dotStatus);
                break;
            case VgcConstants.MathVecOpCrossS16:
                SetResultSaturating32((long)ax * by - (long)ay * bx);
                break;
            case VgcConstants.MathVecOpLen2:
                SetResultUnchecked32((uint)((long)ax * ax + (long)ay * ay), VgcConstants.MathStatusOk);
                break;
            case VgcConstants.MathVecOpScaleFx:
                ushort sx = SaturateS16((long)ax * scalar >> 8, out byte sxStatus);
                ushort sy = SaturateS16((long)ay * scalar >> 8, out byte syStatus);
                SetResultWords(sx, sy, (sxStatus | syStatus) == 0
                    ? VgcConstants.MathStatusOk
                    : VgcConstants.MathStatusOverflow);
                break;
            default:
                SetResult(unchecked((int)0xFFFFFFFF), VgcConstants.MathStatusUnimpl);
                break;
        }
    }

    private short ReadInt16(int address)
    {
        int offset = address - VgcConstants.MathBase;
        return (short)(_regs[offset] | (_regs[offset + 1] << 8));
    }

    private int ReadInt32(int address)
    {
        int offset = address - VgcConstants.MathBase;
        return _regs[offset] |
               (_regs[offset + 1] << 8) |
               (_regs[offset + 2] << 16) |
               (_regs[offset + 3] << 24);
    }

    private static int AbsS16(short value) =>
        value == short.MinValue ? 0x8000 : Math.Abs(value);

    private static ushort DistApproxValue(short dx, short dy)
    {
        int ax = AbsS16(dx);
        int ay = AbsS16(dy);
        int max = Math.Max(ax, ay);
        int min = Math.Min(ax, ay);
        int approx = max + (min >> 1) - (min >> 3);
        return (ushort)Math.Min(approx, ushort.MaxValue);
    }

    private static byte Atan2CordicAngle(short dy, short dx)
    {
        if (dx == 0 && dy == 0)
            return 0;
        if (dy == 0)
            return dx < 0 ? (byte)128 : (byte)0;
        if (dx == 0)
            return dy < 0 ? (byte)192 : (byte)64;

        ReadOnlySpan<int> atanSteps = [32, 19, 10, 5, 3, 1, 1, 0];
        int x = dx;
        int y = dy;
        int z = 0;

        if (x < 0)
        {
            x = -x;
            y = -y;
            z = 128;
        }

        for (int i = 0; i < atanSteps.Length; i++)
        {
            if (y > 0)
            {
                int nextX = x + (y >> i);
                int nextY = y - (x >> i);
                x = nextX;
                y = nextY;
                z += atanSteps[i];
            }
            else if (y < 0)
            {
                int nextX = x - (y >> i);
                int nextY = y + (x >> i);
                x = nextX;
                y = nextY;
                z -= atanSteps[i];
            }
        }

        return unchecked((byte)z);
    }

    internal static byte Sin1P7(byte angle)
    {
        int index;
        bool negate;
        int fraction = angle & 0x3F;

        switch (angle >> 6)
        {
            case 0:
                index = fraction;
                negate = false;
                break;
            case 1:
                index = 64 - fraction;
                negate = false;
                break;
            case 2:
                index = fraction;
                negate = true;
                break;
            default:
                index = 64 - fraction;
                negate = true;
                break;
        }

        int magnitude = SinQuarterMagnitudes[index];
        int value = negate ? -magnitude : Math.Min(magnitude, sbyte.MaxValue);
        return unchecked((byte)(sbyte)value);
    }

    private void SetResult(int value, byte status = VgcConstants.MathStatusOk)
    {
        SetResultUnchecked32(unchecked((uint)value), status);
    }

    private void SetResultUnchecked32(uint value, byte status = VgcConstants.MathStatusOk)
    {
        _regs[VgcConstants.MathRes0 - VgcConstants.MathBase] = (byte)value;
        _regs[VgcConstants.MathRes1 - VgcConstants.MathBase] = (byte)(value >> 8);
        _regs[VgcConstants.MathRes2 - VgcConstants.MathBase] = (byte)(value >> 16);
        _regs[VgcConstants.MathRes3 - VgcConstants.MathBase] = (byte)(value >> 24);
        _regs[VgcConstants.MathStatus - VgcConstants.MathBase] = status;
    }

    private void SetResultSaturating32(long value)
    {
        byte status = VgcConstants.MathStatusOk;
        if (value > int.MaxValue)
        {
            value = int.MaxValue;
            status = VgcConstants.MathStatusOverflow;
        }
        else if (value < int.MinValue)
        {
            value = int.MinValue;
            status = VgcConstants.MathStatusOverflow;
        }

        SetResultUnchecked32(unchecked((uint)(int)value), status);
    }

    private void SetResult16(ushort value, byte status = VgcConstants.MathStatusOk)
    {
        _regs[VgcConstants.MathRes0 - VgcConstants.MathBase] = (byte)value;
        _regs[VgcConstants.MathRes1 - VgcConstants.MathBase] = (byte)(value >> 8);
        _regs[VgcConstants.MathRes2 - VgcConstants.MathBase] = 0;
        _regs[VgcConstants.MathRes3 - VgcConstants.MathBase] = 0;
        _regs[VgcConstants.MathStatus - VgcConstants.MathBase] = status;
    }

    private void SetResultWords(ushort loWord, ushort hiWord, byte status = VgcConstants.MathStatusOk)
    {
        _regs[VgcConstants.MathRes0 - VgcConstants.MathBase] = (byte)loWord;
        _regs[VgcConstants.MathRes1 - VgcConstants.MathBase] = (byte)(loWord >> 8);
        _regs[VgcConstants.MathRes2 - VgcConstants.MathBase] = (byte)hiWord;
        _regs[VgcConstants.MathRes3 - VgcConstants.MathBase] = (byte)(hiWord >> 8);
        _regs[VgcConstants.MathStatus - VgcConstants.MathBase] = status;
    }

    private static ushort SaturateS16(long value, out byte status)
    {
        status = VgcConstants.MathStatusOk;
        if (value > short.MaxValue)
        {
            status = VgcConstants.MathStatusOverflow;
            return 0x7FFF;
        }

        if (value < short.MinValue)
        {
            status = VgcConstants.MathStatusOverflow;
            return 0x8000;
        }

        return (ushort)(short)value;
    }

    private void SeedRng(byte data)
    {
        int shift = _rngSeedIndex * 8;
        _rngState = (_rngState & ~(0xFFu << shift)) | ((uint)data << shift);
        if (_rngState == 0)
            _rngState = RngDefault;
        _rngSeedIndex = (_rngSeedIndex + 1) & 0x03;
        _regs[VgcConstants.MathStatus - VgcConstants.MathBase] = VgcConstants.MathStatusOk;
    }

    private static uint NextRng(uint state)
    {
        uint y = state == 0 ? RngDefault : state;
        y ^= y << 13;
        y ^= y >> 17;
        y ^= y << 5;
        return y == 0 ? RngDefault : y;
    }
}

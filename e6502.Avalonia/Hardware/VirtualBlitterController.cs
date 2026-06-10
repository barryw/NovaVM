namespace e6502.Avalonia.Hardware;

/// <summary>
/// 2D blitter for rectangular copies, fills, and rotations across unified memory spaces.
/// Registers are memory-mapped at $BA83-$BA9B plus $BAA2.
/// </summary>
public sealed class VirtualBlitterController
{
    private const int BltRotateAngleIndex = VgcConstants.BltEnd - VgcConstants.BltBase + 1;

    private readonly byte[] _regs = new byte[BltRotateAngleIndex + 1];
    private readonly Func<byte, int> _getSpaceLength;
    private readonly Func<byte, int, (bool ok, byte value)> _tryReadByte;
    private readonly Func<byte, int, byte, bool> _tryWriteByte;
    private readonly Func<byte, int, int, bool>? _canWriteRange;
    private readonly Action<byte>? _postTransferWrite;
    private bool _busy;
    private bool _fillMode;
    private bool _colorKeyMode;
    private bool _colorKey4Mode;
    private bool _rotateMode;
    private bool _useRowBuffer;
    private byte _srcSpace;
    private byte _dstSpace;
    private byte _fillValue;
    private byte _colorKey;
    private int _srcBase;
    private int _dstBase;
    private int _srcStride;
    private int _dstStride;
    private int _width;
    private int _height;
    private int _row;
    private int _col;
    private int _rowBufferReadCol;
    private bool _rowBufferedReady;
    private byte[]? _rowBuffer;
    private int _wroteCount;
    private long _opCredit;

    public VirtualBlitterController(
        Func<byte, int> getSpaceLength,
        Func<byte, int, (bool ok, byte value)> tryReadByte,
        Func<byte, int, byte, bool> tryWriteByte,
        Func<byte, int, int, bool>? canWriteRange = null,
        Action<byte>? postTransferWrite = null)
    {
        _getSpaceLength = getSpaceLength;
        _tryReadByte = tryReadByte;
        _tryWriteByte = tryWriteByte;
        _canWriteRange = canWriteRange;
        _postTransferWrite = postTransferWrite;

        Reset();
    }

    public bool OwnsAddress(ushort address) =>
        (address >= VgcConstants.BltBase && address <= VgcConstants.BltEnd) ||
        address == VgcConstants.BltRotateAngle;

    public byte Read(ushort address) => _regs[RegIndex(address)];

    public void Reset()
    {
        Array.Clear(_regs);
        _busy = false;
        _fillMode = false;
        _colorKeyMode = false;
        _colorKey4Mode = false;
        _rotateMode = false;
        _useRowBuffer = false;
        _srcSpace = 0;
        _dstSpace = 0;
        _fillValue = 0;
        _colorKey = 0;
        _srcBase = 0;
        _dstBase = 0;
        _srcStride = 0;
        _dstStride = 0;
        _width = 0;
        _height = 0;
        _row = 0;
        _col = 0;
        _rowBufferReadCol = 0;
        _rowBufferedReady = false;
        _rowBuffer = null;
        _wroteCount = 0;
        _opCredit = 0;
        SetCount(0);
        SetStatus(VgcConstants.BltStatusIdle, VgcConstants.BltErrNone);
    }

    public void Write(ushort address, byte data)
    {
        int idx = RegIndex(address);
        _regs[idx] = data;

        if (address == VgcConstants.BltCmd)
            ExecuteCommand(data);
    }

    public void AdvanceCycles(int cycles)
    {
        if (!_busy || cycles <= 0)
            return;

        _opCredit += (long)cycles * VgcConstants.BltOpsPerCycle;
        if (_opCredit <= 0)
            return;

        int opsBudget = (int)Math.Min(_opCredit, int.MaxValue);
        int opsUsed = 0;

        while (opsBudget > 0 && _busy)
        {
            if (_row >= _height)
            {
                CompleteTransfer();
                break;
            }

            if (_fillMode)
            {
                int dstAddr = _dstBase + _row * _dstStride + _col;
                if (!_tryWriteByte(_dstSpace, dstAddr, _fillValue))
                {
                    FailTransfer(VgcConstants.BltErrRange);
                    break;
                }

                _wroteCount++;
                AdvanceCursor();
                opsBudget--;
                opsUsed++;
                if (_row >= _height)
                {
                    CompleteTransfer();
                    break;
                }
                continue;
            }

            if (_rotateMode)
            {
                byte value = _colorKey;
                if (TryGetRotatedSourceAddress(out int srcAddr))
                {
                    var read = _tryReadByte(_srcSpace, srcAddr);
                    if (!read.ok)
                    {
                        FailTransfer(VgcConstants.BltErrRange);
                        break;
                    }

                    value = read.value;
                }

                int dstAddr = _dstBase + _row * _dstStride + _col;
                if (!_tryWriteByte(_dstSpace, dstAddr, value))
                {
                    FailTransfer(VgcConstants.BltErrRange);
                    break;
                }

                _wroteCount++;
                AdvanceCursor();
                opsBudget--;
                opsUsed++;
                if (_row >= _height)
                {
                    CompleteTransfer();
                    break;
                }
                continue;
            }

            if (_useRowBuffer)
            {
                if (!_rowBufferedReady)
                {
                    int srcAddr = _srcBase + _row * _srcStride + _rowBufferReadCol;
                    var read = _tryReadByte(_srcSpace, srcAddr);
                    if (!read.ok)
                    {
                        FailTransfer(VgcConstants.BltErrRange);
                        break;
                    }

                    _rowBuffer![_rowBufferReadCol++] = read.value;
                    if (_rowBufferReadCol >= _width)
                    {
                        _rowBufferedReady = true;
                        _col = 0;
                    }

                    opsBudget--;
                    opsUsed++;
                    continue;
                }

                byte value = _rowBuffer![_col];
                if (!(_colorKeyMode && value == _colorKey))
                {
                    int dstAddr = _dstBase + _row * _dstStride + _col;
                    if (!TryWriteKeyed(dstAddr, value, out bool wrote))
                    {
                        FailTransfer(VgcConstants.BltErrRange);
                        break;
                    }

                    if (wrote)
                        _wroteCount++;
                }

                _col++;
                opsBudget--;
                opsUsed++;
                if (_col >= _width)
                {
                    _row++;
                    _col = 0;
                    _rowBufferReadCol = 0;
                    _rowBufferedReady = false;
                    if (_row >= _height)
                    {
                        CompleteTransfer();
                        break;
                    }
                }
                continue;
            }

            int src = _srcBase + _row * _srcStride + _col;
            var directRead = _tryReadByte(_srcSpace, src);
            if (!directRead.ok)
            {
                FailTransfer(VgcConstants.BltErrRange);
                break;
            }

            byte directValue = directRead.value;
            if (!(_colorKeyMode && directValue == _colorKey))
            {
                int dst = _dstBase + _row * _dstStride + _col;
                if (!TryWriteKeyed(dst, directValue, out bool directWrote))
                {
                    FailTransfer(VgcConstants.BltErrRange);
                    break;
                }

                if (directWrote)
                    _wroteCount++;
            }

            AdvanceCursor();
            opsBudget--;
            opsUsed++;
            if (_row >= _height)
            {
                CompleteTransfer();
                break;
            }
        }

        _opCredit -= opsUsed;
        SetCount(_wroteCount);
    }

    private void ExecuteCommand(byte cmd)
    {
        if (cmd != VgcConstants.BltCmdStart)
        {
            SetStatus(VgcConstants.BltStatusError, VgcConstants.BltErrBadCmd);
            return;
        }

        if (_busy)
        {
            FailTransfer(VgcConstants.BltErrBadCmd);
            return;
        }

        int width = Get16(VgcConstants.BltWidthL);
        int height = Get16(VgcConstants.BltHeightL);
        if (width <= 0 || height <= 0)
        {
            SetCount(0);
            SetStatus(VgcConstants.BltStatusError, VgcConstants.BltErrBadArgs);
            return;
        }

        byte mode = _regs[RegIndex(VgcConstants.BltMode)];
        bool fillMode = (mode & VgcConstants.BltModeFill) != 0;
        bool colorKeyMode = (mode & VgcConstants.BltModeColorKey) != 0;
        bool rotateMode = (mode & VgcConstants.BltModeRotate) != 0;
        bool colorKey4Mode = (mode & VgcConstants.BltModeColorKey4) != 0;
        byte colorKey = _regs[RegIndex(VgcConstants.BltColorKey)];

        if (rotateMode && (fillMode || width != height || width > 256))
        {
            SetCount(0);
            SetStatus(VgcConstants.BltStatusError, VgcConstants.BltErrBadArgs);
            return;
        }

        // Nibble-granular keying only makes sense for a plain copy: fill has
        // no source and rotate keys per byte (its key doubles as background).
        if (colorKey4Mode && (fillMode || rotateMode || colorKeyMode))
        {
            SetCount(0);
            SetStatus(VgcConstants.BltStatusError, VgcConstants.BltErrBadArgs);
            return;
        }

        byte srcSpace = _regs[RegIndex(VgcConstants.BltSrcSpace)];
        byte dstSpace = _regs[RegIndex(VgcConstants.BltDstSpace)];
        int dstSpaceLen = _getSpaceLength(dstSpace);
        if (dstSpaceLen <= 0)
        {
            SetCount(0);
            SetStatus(VgcConstants.BltStatusError, VgcConstants.BltErrBadSpace);
            return;
        }

        int srcSpaceLen = 0;
        if (!fillMode)
        {
            srcSpaceLen = _getSpaceLength(srcSpace);
            if (srcSpaceLen <= 0)
            {
                SetCount(0);
                SetStatus(VgcConstants.BltStatusError, VgcConstants.BltErrBadSpace);
                return;
            }
        }

        int srcBase = Get24(VgcConstants.BltSrcL);
        int dstBase = Get24(VgcConstants.BltDstL);
        int srcStride = Get16(VgcConstants.BltSrcStrideL);
        int dstStride = Get16(VgcConstants.BltDstStrideL);

        if (srcStride < 0 || dstStride < 0)
        {
            SetCount(0);
            SetStatus(VgcConstants.BltStatusError, VgcConstants.BltErrBadArgs);
            return;
        }

        if (!fillMode && !RectFits(srcBase, width, height, srcStride, srcSpaceLen))
        {
            SetCount(0);
            SetStatus(VgcConstants.BltStatusError, VgcConstants.BltErrRange);
            return;
        }

        if (!RectFits(dstBase, width, height, dstStride, dstSpaceLen))
        {
            SetCount(0);
            SetStatus(VgcConstants.BltStatusError, VgcConstants.BltErrRange);
            return;
        }

        if (!CanWriteRect(dstSpace, dstBase, width, height, dstStride))
        {
            SetCount(0);
            SetStatus(VgcConstants.BltStatusError, VgcConstants.BltErrWriteProt);
            return;
        }

        if (rotateMode && srcSpace == dstSpace &&
            RectLinearRangesOverlap(srcBase, width, height, srcStride, dstBase, width, height, dstStride))
        {
            SetCount(0);
            SetStatus(VgcConstants.BltStatusError, VgcConstants.BltErrBadArgs);
            return;
        }

        SetCount(0);
        SetStatus(VgcConstants.BltStatusBusy, VgcConstants.BltErrNone);
        _busy = true;
        _fillMode = fillMode;
        _colorKeyMode = colorKeyMode;
        _colorKey4Mode = colorKey4Mode;
        _rotateMode = rotateMode;
        _useRowBuffer = !fillMode && !rotateMode && srcSpace == dstSpace;
        _srcSpace = srcSpace;
        _dstSpace = dstSpace;
        _srcBase = srcBase;
        _dstBase = dstBase;
        _srcStride = srcStride;
        _dstStride = dstStride;
        _width = width;
        _height = height;
        _row = 0;
        _col = 0;
        _fillValue = _regs[RegIndex(VgcConstants.BltFillValue)];
        _colorKey = colorKey;
        _wroteCount = 0;
        _opCredit = 0;
        _rowBufferReadCol = 0;
        _rowBufferedReady = !_useRowBuffer;
        _rowBuffer = _useRowBuffer ? new byte[width] : null;

        // Complete transfer synchronously — emulates RDY stalling on real hardware.
        // The extension ROM reads status immediately after starting the blitter.
        AdvanceCycles(int.MaxValue / VgcConstants.BltOpsPerCycle);
    }

    /// <summary>
    /// Write one source byte through the active key mode. Plain modes write
    /// the byte as-is. ColorKey4 compares each 4bpp nibble against the key's
    /// low nibble: fully transparent bytes are skipped, fully opaque bytes
    /// written, and mixed bytes read-modify-write the destination so the
    /// transparent pixel underneath survives. (FPGA note for M6: the RTL
    /// blitter needs the same mode bit — a dst read port already exists for
    /// the overlap row buffer, so mixed bytes can RMW in silicon too.)
    /// </summary>
    private bool TryWriteKeyed(int dstAddr, byte value, out bool wrote)
    {
        wrote = false;
        if (_colorKey4Mode)
        {
            byte key = (byte)(_colorKey & 0x0F);
            bool hiTransparent = (value >> 4) == key;
            bool loTransparent = (value & 0x0F) == key;
            if (hiTransparent && loTransparent)
                return true;

            if (hiTransparent || loTransparent)
            {
                var dstRead = _tryReadByte(_dstSpace, dstAddr);
                if (!dstRead.ok)
                    return false;

                value = hiTransparent
                    ? (byte)((dstRead.value & 0xF0) | (value & 0x0F))
                    : (byte)((value & 0xF0) | (dstRead.value & 0x0F));
            }
        }

        if (!_tryWriteByte(_dstSpace, dstAddr, value))
            return false;

        wrote = true;
        return true;
    }

    private void AdvanceCursor()
    {
        _col++;
        if (_col >= _width)
        {
            _col = 0;
            _row++;
        }
    }

    private void CompleteTransfer()
    {
        _busy = false;
        _opCredit = 0;
        SetCount(_wroteCount);
        _postTransferWrite?.Invoke(_dstSpace);
        SetStatus(VgcConstants.BltStatusOk, VgcConstants.BltErrNone);
    }

    private void FailTransfer(byte errCode)
    {
        _busy = false;
        _opCredit = 0;
        SetCount(_wroteCount);
        SetStatus(VgcConstants.BltStatusError, errCode);
    }

    private bool CanWriteRect(byte space, int baseAddr, int width, int height, int stride)
    {
        if (_canWriteRange is null)
            return true;

        for (int row = 0; row < height; row++)
        {
            int rowStart = baseAddr + row * stride;
            if (!_canWriteRange(space, rowStart, width))
                return false;
        }

        return true;
    }

    private bool TryGetRotatedSourceAddress(out int srcAddr)
    {
        int sin = unchecked((sbyte)MathCoprocessor.Sin1P7(_regs[RegIndex(VgcConstants.BltRotateAngle)]));
        int cos = unchecked((sbyte)MathCoprocessor.Sin1P7((byte)(_regs[RegIndex(VgcConstants.BltRotateAngle)] + 64)));
        long centerFp = ((long)_width - 1) << 7;
        long dx = ((long)_col << 8) - centerFp;
        long dy = ((long)_row << 8) - centerFp;

        long srcXFp = centerFp + (((dx * cos) + (dy * sin)) >> 7);
        long srcYFp = centerFp + (((dy * cos) - (dx * sin)) >> 7);
        long srcX = (srcXFp + 128) >> 8;
        long srcY = (srcYFp + 128) >> 8;

        if (srcX < 0 || srcY < 0 || srcX >= _width || srcY >= _height)
        {
            srcAddr = 0;
            return false;
        }

        srcAddr = _srcBase + (int)srcY * _srcStride + (int)srcX;
        return true;
    }

    private int Get16(int baseAddress)
    {
        int l = _regs[RegIndex(baseAddress)];
        int h = _regs[RegIndex(baseAddress + 1)];
        return l | (h << 8);
    }

    private int Get24(int baseAddress)
    {
        int l = _regs[RegIndex(baseAddress)];
        int m = _regs[RegIndex(baseAddress + 1)];
        int h = _regs[RegIndex(baseAddress + 2)];
        return l | (m << 8) | (h << 16);
    }

    private void SetCount(int count)
    {
        _regs[RegIndex(VgcConstants.BltCountL)] = (byte)(count & 0xFF);
        _regs[RegIndex(VgcConstants.BltCountM)] = (byte)((count >> 8) & 0xFF);
        _regs[RegIndex(VgcConstants.BltCountH)] = (byte)((count >> 16) & 0xFF);
    }

    private void SetStatus(byte status, byte errCode)
    {
        _regs[RegIndex(VgcConstants.BltStatus)] = status;
        _regs[RegIndex(VgcConstants.BltErrCode)] = errCode;
    }

    private static int RegIndex(int address) =>
        address == VgcConstants.BltRotateAngle
            ? BltRotateAngleIndex
            : address - VgcConstants.BltBase;

    private static bool RectLinearRangesOverlap(
        int aBase, int aWidth, int aHeight, int aStride,
        int bBase, int bWidth, int bHeight, int bStride)
    {
        long aEnd = RectLinearEnd(aBase, aWidth, aHeight, aStride);
        long bEnd = RectLinearEnd(bBase, bWidth, bHeight, bStride);
        return aBase < bEnd && bBase < aEnd;
    }

    private static long RectLinearEnd(int baseAddr, int width, int height, int stride) =>
        (long)baseAddr + ((long)(height - 1) * stride) + width;

    private static bool RectFits(int baseAddr, int width, int height, int stride, int spaceLength)
    {
        if (baseAddr < 0 || width <= 0 || height <= 0 || stride < 0 || spaceLength <= 0)
            return false;

        for (int row = 0; row < height; row++)
        {
            long rowStart = (long)baseAddr + ((long)row * stride);
            long rowEnd = rowStart + width;
            if (rowStart < 0 || rowStart >= spaceLength || rowEnd > spaceLength)
                return false;
        }

        return true;
    }
}

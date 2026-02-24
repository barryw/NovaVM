namespace e6502.Avalonia.Hardware;

/// <summary>
/// 2D blitter for rectangular copies and fills across unified memory spaces.
/// Registers are memory-mapped at $BA80-$BA9F.
/// </summary>
public sealed class VirtualBlitterController
{
    private readonly byte[] _regs = new byte[VgcConstants.BltEnd - VgcConstants.BltBase + 1];
    private readonly Func<byte, int> _getSpaceLength;
    private readonly Func<byte, int, (bool ok, byte value)> _tryReadByte;
    private readonly Func<byte, int, byte, bool> _tryWriteByte;
    private readonly Func<byte, int, int, bool>? _canWriteRange;
    private readonly Action<byte>? _postTransferWrite;
    private bool _busy;
    private bool _fillMode;
    private bool _colorKeyMode;
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

        SetStatus(VgcConstants.BltStatusIdle, VgcConstants.BltErrNone);
        SetCount(0);
    }

    public bool OwnsAddress(ushort address) =>
        address >= VgcConstants.BltBase && address <= VgcConstants.BltEnd;

    public byte Read(ushort address) => _regs[RegIndex(address)];

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
                    if (!_tryWriteByte(_dstSpace, dstAddr, value))
                    {
                        FailTransfer(VgcConstants.BltErrRange);
                        break;
                    }

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
                if (!_tryWriteByte(_dstSpace, dst, directValue))
                {
                    FailTransfer(VgcConstants.BltErrRange);
                    break;
                }

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
        byte colorKey = _regs[RegIndex(VgcConstants.BltColorKey)];

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

        SetCount(0);
        SetStatus(VgcConstants.BltStatusBusy, VgcConstants.BltErrNone);
        _busy = true;
        _fillMode = fillMode;
        _colorKeyMode = colorKeyMode;
        _useRowBuffer = !fillMode && srcSpace == dstSpace;
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

    private static int RegIndex(int address) => address - VgcConstants.BltBase;

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

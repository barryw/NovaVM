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

    private void ExecuteCommand(byte cmd)
    {
        if (cmd != VgcConstants.BltCmdStart)
        {
            SetStatus(VgcConstants.BltStatusError, VgcConstants.BltErrBadCmd);
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

        byte fillValue = _regs[RegIndex(VgcConstants.BltFillValue)];
        int wroteCount = 0;
        bool useRowBuffer = !fillMode && srcSpace == dstSpace;
        byte[]? rowBuffer = useRowBuffer ? new byte[width] : null;

        for (int row = 0; row < height; row++)
        {
            int srcRowBase = srcBase + row * srcStride;
            int dstRowBase = dstBase + row * dstStride;

            if (fillMode)
            {
                for (int col = 0; col < width; col++)
                {
                    if (!_tryWriteByte(dstSpace, dstRowBase + col, fillValue))
                    {
                        SetCount(wroteCount);
                        SetStatus(VgcConstants.BltStatusError, VgcConstants.BltErrRange);
                        return;
                    }

                    wroteCount++;
                }

                continue;
            }

            if (rowBuffer is not null)
            {
                for (int col = 0; col < width; col++)
                {
                    var read = _tryReadByte(srcSpace, srcRowBase + col);
                    if (!read.ok)
                    {
                        SetCount(wroteCount);
                        SetStatus(VgcConstants.BltStatusError, VgcConstants.BltErrRange);
                        return;
                    }

                    rowBuffer[col] = read.value;
                }

                for (int col = 0; col < width; col++)
                {
                    byte value = rowBuffer[col];
                    if (colorKeyMode && value == colorKey)
                        continue;

                    if (!_tryWriteByte(dstSpace, dstRowBase + col, value))
                    {
                        SetCount(wroteCount);
                        SetStatus(VgcConstants.BltStatusError, VgcConstants.BltErrRange);
                        return;
                    }

                    wroteCount++;
                }

                continue;
            }

            for (int col = 0; col < width; col++)
            {
                var read = _tryReadByte(srcSpace, srcRowBase + col);
                if (!read.ok)
                {
                    SetCount(wroteCount);
                    SetStatus(VgcConstants.BltStatusError, VgcConstants.BltErrRange);
                    return;
                }

                byte value = read.value;
                if (colorKeyMode && value == colorKey)
                    continue;

                if (!_tryWriteByte(dstSpace, dstRowBase + col, value))
                {
                    SetCount(wroteCount);
                    SetStatus(VgcConstants.BltStatusError, VgcConstants.BltErrRange);
                    return;
                }

                wroteCount++;
            }
        }

        SetCount(wroteCount);
        _postTransferWrite?.Invoke(dstSpace);
        SetStatus(VgcConstants.BltStatusOk, VgcConstants.BltErrNone);
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

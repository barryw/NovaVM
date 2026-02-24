namespace e6502.Avalonia.Hardware;

/// <summary>
/// Unified DMA controller for bulk copies between CPU RAM and virtual devices.
/// Registers are memory-mapped at $BA60-$BA7F.
/// </summary>
public sealed class VirtualDmaController
{
    private readonly byte[] _regs = new byte[VgcConstants.DmaEnd - VgcConstants.DmaBase + 1];
    private readonly Func<byte, int> _getSpaceLength;
    private readonly Func<byte, int, (bool ok, byte value)> _tryReadByte;
    private readonly Func<byte, int, byte, bool> _tryWriteByte;
    private readonly Func<byte, int, int, bool>? _canWriteRange;
    private readonly Action<byte>? _postTransferWrite;
    private bool _busy;
    private bool _fillMode;
    private byte _srcSpace;
    private byte _dstSpace;
    private byte _fillValue;
    private int _srcAddr;
    private int _dstAddr;
    private int _length;
    private int _index;
    private int _moved;
    private long _byteCredit;

    public VirtualDmaController(
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

        SetStatus(VgcConstants.DmaStatusIdle, VgcConstants.DmaErrNone);
        SetCount(0);
    }

    public bool OwnsAddress(ushort address) =>
        address >= VgcConstants.DmaBase && address <= VgcConstants.DmaEnd;

    public byte Read(ushort address) => _regs[RegIndex(address)];

    public void Write(ushort address, byte data)
    {
        _regs[RegIndex(address)] = data;

        if (address == VgcConstants.DmaCmd)
            ExecuteCommand(data);
    }

    public void AdvanceCycles(int cycles)
    {
        if (!_busy || cycles <= 0)
            return;

        _byteCredit += (long)cycles * VgcConstants.DmaBytesPerCycle;
        int remaining = _length - _index;
        if (remaining <= 0)
        {
            CompleteTransfer();
            return;
        }

        int toProcess = (int)Math.Min(_byteCredit, remaining);
        if (toProcess <= 0)
            return;
        _byteCredit -= toProcess;

        for (int i = 0; i < toProcess; i++)
        {
            byte value;
            if (_fillMode)
            {
                value = _fillValue;
            }
            else
            {
                var read = _tryReadByte(_srcSpace, _srcAddr + _index);
                if (!read.ok)
                {
                    FailTransfer(VgcConstants.DmaErrRange);
                    return;
                }
                value = read.value;
            }

            if (!_tryWriteByte(_dstSpace, _dstAddr + _index, value))
            {
                FailTransfer(VgcConstants.DmaErrRange);
                return;
            }

            _index++;
            _moved++;
        }

        SetCount(_moved);
        if (_index >= _length)
            CompleteTransfer();
    }

    private void ExecuteCommand(byte cmd)
    {
        if (cmd != VgcConstants.DmaCmdStart)
        {
            SetStatus(VgcConstants.DmaStatusError, VgcConstants.DmaErrBadCmd);
            return;
        }

        if (_busy)
        {
            FailTransfer(VgcConstants.DmaErrBadCmd);
            return;
        }

        int len = Get24(VgcConstants.DmaLenL);
        if (len <= 0)
        {
            SetCount(0);
            SetStatus(VgcConstants.DmaStatusError, VgcConstants.DmaErrBadArgs);
            return;
        }

        byte srcSpace = _regs[RegIndex(VgcConstants.DmaSrcSpace)];
        byte dstSpace = _regs[RegIndex(VgcConstants.DmaDstSpace)];
        int srcSpaceLen = _getSpaceLength(srcSpace);
        int dstSpaceLen = _getSpaceLength(dstSpace);
        if (srcSpaceLen <= 0 || dstSpaceLen <= 0)
        {
            SetCount(0);
            SetStatus(VgcConstants.DmaStatusError, VgcConstants.DmaErrBadSpace);
            return;
        }

        int srcAddr = Get24(VgcConstants.DmaSrcL);
        int dstAddr = Get24(VgcConstants.DmaDstL);
        bool fillMode = (_regs[RegIndex(VgcConstants.DmaMode)] & VgcConstants.DmaModeFill) != 0;

        if (!fillMode && !RangeFits(srcAddr, len, srcSpaceLen))
        {
            SetCount(0);
            SetStatus(VgcConstants.DmaStatusError, VgcConstants.DmaErrRange);
            return;
        }

        if (!RangeFits(dstAddr, len, dstSpaceLen))
        {
            SetCount(0);
            SetStatus(VgcConstants.DmaStatusError, VgcConstants.DmaErrRange);
            return;
        }

        if (_canWriteRange is not null && !_canWriteRange(dstSpace, dstAddr, len))
        {
            SetCount(0);
            SetStatus(VgcConstants.DmaStatusError, VgcConstants.DmaErrWriteProt);
            return;
        }

        SetCount(0);
        SetStatus(VgcConstants.DmaStatusBusy, VgcConstants.DmaErrNone);
        _busy = true;
        _fillMode = fillMode;
        _srcSpace = srcSpace;
        _dstSpace = dstSpace;
        _srcAddr = srcAddr;
        _dstAddr = dstAddr;
        _length = len;
        _index = 0;
        _moved = 0;
        _fillValue = _regs[RegIndex(VgcConstants.DmaFillValue)];
        _byteCredit = 0;
    }

    private void CompleteTransfer()
    {
        _busy = false;
        _byteCredit = 0;
        SetCount(_moved);
        _postTransferWrite?.Invoke(_dstSpace);
        SetStatus(VgcConstants.DmaStatusOk, VgcConstants.DmaErrNone);
    }

    private void FailTransfer(byte errCode)
    {
        _busy = false;
        _byteCredit = 0;
        SetCount(_moved);
        SetStatus(VgcConstants.DmaStatusError, errCode);
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
        _regs[RegIndex(VgcConstants.DmaCountL)] = (byte)(count & 0xFF);
        _regs[RegIndex(VgcConstants.DmaCountM)] = (byte)((count >> 8) & 0xFF);
        _regs[RegIndex(VgcConstants.DmaCountH)] = (byte)((count >> 16) & 0xFF);
    }

    private void SetStatus(byte status, byte errCode)
    {
        _regs[RegIndex(VgcConstants.DmaStatus)] = status;
        _regs[RegIndex(VgcConstants.DmaErrCode)] = errCode;
    }

    private static int RegIndex(int address) => address - VgcConstants.DmaBase;

    private static bool RangeFits(int start, int len, int spaceLength)
    {
        if (start < 0 || len < 0 || spaceLength <= 0)
            return false;

        long end = (long)start + len;
        return start < spaceLength && end <= spaceLength;
    }
}

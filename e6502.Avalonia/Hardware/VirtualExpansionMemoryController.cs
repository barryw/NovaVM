namespace e6502.Avalonia.Hardware;

using System.Text;

public sealed class VirtualExpansionMemoryController
{
    private const int PageSize = 256;
    private const int BankSize = 65536;
    private const ushort WindowBase = 0xBC00;
    private const ushort WindowEnd = 0xBFFF;

    private readonly byte[] _regs = new byte[VgcConstants.XmcEnd - VgcConstants.XmcBase + 1];
    private readonly byte[] _xram;
    private readonly bool[] _usedPages;
    private int _usedPageCount;

    private readonly Func<ushort, byte> _busRead;
    private readonly Action<ushort, byte> _busWrite;

    private readonly Dictionary<byte, BlockInfo> _blocksByHandle = [];
    private readonly Dictionary<string, byte> _handleByName = new(StringComparer.OrdinalIgnoreCase);
    private readonly List<byte> _dirHandles = [];
    private int _dirCursor;
    private byte _nextHandle = 1;

    private sealed class BlockInfo
    {
        public required byte Handle { get; init; }
        public string Name { get; set; } = string.Empty;
        public int Addr { get; set; }
        public int Length { get; set; }
        public int Pages { get; set; }

        public bool IsNamed => !string.IsNullOrEmpty(Name);
    }

    public VirtualExpansionMemoryController(
        Func<ushort, byte> busRead,
        Action<ushort, byte> busWrite,
        int sizeBytes = 512 * 1024)
    {
        if (sizeBytes < BankSize || sizeBytes % PageSize != 0)
            throw new ArgumentOutOfRangeException(nameof(sizeBytes), "XRAM size must be >=64KB and page aligned.");

        _busRead = busRead;
        _busWrite = busWrite;
        _xram = new byte[sizeBytes];
        _usedPages = new bool[sizeBytes / PageSize];

        int banks = Math.Clamp(sizeBytes / BankSize, 1, 255);
        _regs[RegIndex(VgcConstants.XmcBanks)] = (byte)banks;
        _regs[RegIndex(VgcConstants.XmcBank)] = 0;

        RefreshStats();
        SetStatus(VgcConstants.XmcStatusIdle, VgcConstants.XmcErrNone);
    }

    public bool OwnsAddress(ushort address)
    {
        if (address >= VgcConstants.XmcBase && address <= VgcConstants.XmcEnd)
            return true;

        if (address >= WindowBase && address <= WindowEnd)
            return IsWindowMapped((address - WindowBase) >> 8);

        return false;
    }

    public byte Read(ushort address)
    {
        if (address >= VgcConstants.XmcBase && address <= VgcConstants.XmcEnd)
            return _regs[RegIndex(address)];

        if (address >= WindowBase && address <= WindowEnd)
            return ReadWindowByte(address);

        return 0;
    }

    public void Write(ushort address, byte data)
    {
        if (address >= WindowBase && address <= WindowEnd)
        {
            WriteWindowByte(address, data);
            return;
        }

        int idx = RegIndex(address);

        if (address == VgcConstants.XmcBanks
            || address == VgcConstants.XmcPagesUsedL
            || address == VgcConstants.XmcPagesUsedH
            || address == VgcConstants.XmcPagesFreeL
            || address == VgcConstants.XmcPagesFreeH
            || address == VgcConstants.XmcDirCountL
            || address == VgcConstants.XmcDirCountH)
            return; // read-only

        _regs[idx] = data;

        if (address == VgcConstants.XmcCmd)
            ExecuteCommand(data);
    }

    /// <summary>Total XRAM capacity in bytes.</summary>
    public int CapacityBytes => _xram.Length;

    /// <summary>Read one byte from linear XRAM address space (0..CapacityBytes-1).</summary>
    public bool TryReadLinear(int address, out byte value)
    {
        value = 0;
        if (!RangeOk(address, 1))
            return false;

        value = _xram[address];
        return true;
    }

    /// <summary>
    /// Write one byte to linear XRAM address space (0..CapacityBytes-1).
    /// Marks pages as used but does not refresh stats registers; call RefreshStatsRegisters()
    /// after bulk writes.
    /// </summary>
    public bool TryWriteLinear(int address, byte value)
    {
        if (!RangeOk(address, 1))
            return false;

        _xram[address] = value;
        MarkRangeUsed(address, 1);
        return true;
    }

    /// <summary>Refresh exposed XMC stats registers after external/bulk XRAM writes.</summary>
    public void RefreshStatsRegisters() => RefreshStats();

    private void ExecuteCommand(byte cmd)
    {
        switch (cmd)
        {
            case VgcConstants.XmcCmdGetByte:
                DoGetByte();
                break;
            case VgcConstants.XmcCmdPutByte:
                DoPutByte();
                break;
            case VgcConstants.XmcCmdStash:
                DoStash();
                break;
            case VgcConstants.XmcCmdFetch:
                DoFetch();
                break;
            case VgcConstants.XmcCmdFill:
                DoFill();
                break;
            case VgcConstants.XmcCmdStats:
                RefreshStats();
                SetStatus(VgcConstants.XmcStatusOk, VgcConstants.XmcErrNone);
                break;
            case VgcConstants.XmcCmdResetUsage:
                ResetUsage();
                RefreshStats();
                SetStatus(VgcConstants.XmcStatusOk, VgcConstants.XmcErrNone);
                break;
            case VgcConstants.XmcCmdRelease:
                DoReleaseUsage();
                break;
            case VgcConstants.XmcCmdAlloc:
                DoAlloc();
                break;
            case VgcConstants.XmcCmdNStash:
                DoNamedStash();
                break;
            case VgcConstants.XmcCmdNFetch:
                DoNamedFetch();
                break;
            case VgcConstants.XmcCmdNDelete:
                DoNamedDelete();
                break;
            case VgcConstants.XmcCmdNDirOpen:
                DoNamedDirOpen();
                break;
            case VgcConstants.XmcCmdNDirRead:
                DoNamedDirRead();
                break;
            default:
                SetStatus(VgcConstants.XmcStatusError, VgcConstants.XmcErrBadArgs);
                break;
        }
    }

    private void DoGetByte()
    {
        int xaddr = GetXAddr();
        if (!RangeOk(xaddr, 1))
        {
            SetStatus(VgcConstants.XmcStatusError, VgcConstants.XmcErrRange);
            return;
        }

        _regs[RegIndex(VgcConstants.XmcData)] = _xram[xaddr];
        SetStatus(VgcConstants.XmcStatusOk, VgcConstants.XmcErrNone);
    }

    private void DoPutByte()
    {
        int xaddr = GetXAddr();
        if (!RangeOk(xaddr, 1))
        {
            SetStatus(VgcConstants.XmcStatusError, VgcConstants.XmcErrRange);
            return;
        }

        _xram[xaddr] = _regs[RegIndex(VgcConstants.XmcData)];
        MarkRangeUsed(xaddr, 1);
        RefreshStats();
        SetStatus(VgcConstants.XmcStatusOk, VgcConstants.XmcErrNone);
    }

    private void DoStash()
    {
        int xaddr = GetXAddr();
        int ramAddr = GetRamAddr();
        int len = GetLength();

        if (len <= 0)
        {
            SetStatus(VgcConstants.XmcStatusOk, VgcConstants.XmcErrNone);
            return;
        }

        if (!RangeOk(xaddr, len) || !RamRangeOk(ramAddr, len, forWrite: false))
        {
            SetStatus(VgcConstants.XmcStatusError, VgcConstants.XmcErrRange);
            return;
        }

        for (int i = 0; i < len; i++)
            _xram[xaddr + i] = _busRead((ushort)(ramAddr + i));

        MarkRangeUsed(xaddr, len);
        RefreshStats();
        SetStatus(VgcConstants.XmcStatusOk, VgcConstants.XmcErrNone);
    }

    private void DoFetch()
    {
        int xaddr = GetXAddr();
        int ramAddr = GetRamAddr();
        int len = GetLength();

        if (len <= 0)
        {
            SetStatus(VgcConstants.XmcStatusOk, VgcConstants.XmcErrNone);
            return;
        }

        if (!RangeOk(xaddr, len) || !RamRangeOk(ramAddr, len, forWrite: true))
        {
            SetStatus(VgcConstants.XmcStatusError, VgcConstants.XmcErrRange);
            return;
        }

        for (int i = 0; i < len; i++)
            _busWrite((ushort)(ramAddr + i), _xram[xaddr + i]);

        SetStatus(VgcConstants.XmcStatusOk, VgcConstants.XmcErrNone);
    }

    private void DoFill()
    {
        int xaddr = GetXAddr();
        int len = GetLength();
        if (len <= 0)
        {
            SetStatus(VgcConstants.XmcStatusOk, VgcConstants.XmcErrNone);
            return;
        }

        if (!RangeOk(xaddr, len))
        {
            SetStatus(VgcConstants.XmcStatusError, VgcConstants.XmcErrRange);
            return;
        }

        byte value = _regs[RegIndex(VgcConstants.XmcData)];
        Array.Fill(_xram, value, xaddr, len);
        MarkRangeUsed(xaddr, len);
        RefreshStats();
        SetStatus(VgcConstants.XmcStatusOk, VgcConstants.XmcErrNone);
    }

    private void DoReleaseUsage()
    {
        int xaddr = GetXAddr();
        int len = GetLength();
        if (len <= 0)
        {
            SetStatus(VgcConstants.XmcStatusOk, VgcConstants.XmcErrNone);
            return;
        }

        if (!RangeOk(xaddr, len))
        {
            SetStatus(VgcConstants.XmcStatusError, VgcConstants.XmcErrRange);
            return;
        }

        RemoveOverlappingBlocks(xaddr, len);
        MarkRangeFree(xaddr, len);
        RefreshStats();
        SetStatus(VgcConstants.XmcStatusOk, VgcConstants.XmcErrNone);
    }

    private void DoAlloc()
    {
        int len = GetLength();
        if (len <= 0)
        {
            SetStatus(VgcConstants.XmcStatusError, VgcConstants.XmcErrBadArgs);
            return;
        }

        int pages = PagesForLength(len);
        int firstPage = FindFreeRun(pages);
        if (firstPage < 0)
        {
            SetStatus(VgcConstants.XmcStatusError, VgcConstants.XmcErrNoSpace);
            return;
        }

        byte handle = AllocateHandle();
        if (handle == 0)
        {
            SetStatus(VgcConstants.XmcStatusError, VgcConstants.XmcErrNoSpace);
            return;
        }

        int addr = firstPage * PageSize;
        MarkPagesUsed(firstPage, pages);

        _blocksByHandle[handle] = new BlockInfo
        {
            Handle = handle,
            Addr = addr,
            Length = len,
            Pages = pages
        };

        _regs[RegIndex(VgcConstants.XmcHandle)] = handle;
        SetXAddr(addr);
        _regs[RegIndex(VgcConstants.XmcBank)] = (byte)((addr >> 16) & 0xFF);
        RefreshStats();
        SetStatus(VgcConstants.XmcStatusOk, VgcConstants.XmcErrNone);
    }

    private void DoNamedStash()
    {
        string? name = ReadNameFromRegs();
        if (name is null)
        {
            SetStatus(VgcConstants.XmcStatusError, VgcConstants.XmcErrName);
            return;
        }

        int ramAddr = GetRamAddr();
        int len = GetLength();
        if (len <= 0 || !RamRangeOk(ramAddr, len, forWrite: false))
        {
            SetStatus(VgcConstants.XmcStatusError, VgcConstants.XmcErrBadArgs);
            return;
        }

        int pages = PagesForLength(len);
        BlockInfo block;

        if (_handleByName.TryGetValue(name, out byte existingHandle)
            && _blocksByHandle.TryGetValue(existingHandle, out BlockInfo? existing))
        {
            block = existing;
            if (block.Pages < pages)
            {
                RemoveBlock(block.Handle);
                if (!TryAllocateNamedBlock(name, len, pages, out block))
                {
                    SetStatus(VgcConstants.XmcStatusError, VgcConstants.XmcErrNoSpace);
                    return;
                }
            }
            else
            {
                block.Length = len;
            }
        }
        else
        {
            if (!TryAllocateNamedBlock(name, len, pages, out block))
            {
                SetStatus(VgcConstants.XmcStatusError, VgcConstants.XmcErrNoSpace);
                return;
            }
        }

        for (int i = 0; i < len; i++)
            _xram[block.Addr + i] = _busRead((ushort)(ramAddr + i));

        _regs[RegIndex(VgcConstants.XmcHandle)] = block.Handle;
        SetXAddr(block.Addr);
        _regs[RegIndex(VgcConstants.XmcBank)] = (byte)((block.Addr >> 16) & 0xFF);
        RefreshStats();
        SetStatus(VgcConstants.XmcStatusOk, VgcConstants.XmcErrNone);
    }

    private void DoNamedFetch()
    {
        string? name = ReadNameFromRegs();
        if (name is null)
        {
            SetStatus(VgcConstants.XmcStatusError, VgcConstants.XmcErrName);
            return;
        }

        if (!_handleByName.TryGetValue(name, out byte handle)
            || !_blocksByHandle.TryGetValue(handle, out BlockInfo? block))
        {
            SetStatus(VgcConstants.XmcStatusError, VgcConstants.XmcErrNotFound);
            return;
        }

        int ramAddr = GetRamAddr();
        int requested = GetLength();
        int len = requested <= 0 ? block.Length : Math.Min(requested, block.Length);

        if (!RamRangeOk(ramAddr, len, forWrite: true) || !RangeOk(block.Addr, len))
        {
            SetStatus(VgcConstants.XmcStatusError, VgcConstants.XmcErrRange);
            return;
        }

        for (int i = 0; i < len; i++)
            _busWrite((ushort)(ramAddr + i), _xram[block.Addr + i]);

        _regs[RegIndex(VgcConstants.XmcHandle)] = block.Handle;
        SetXAddr(block.Addr);
        SetLength(len);
        _regs[RegIndex(VgcConstants.XmcBank)] = (byte)((block.Addr >> 16) & 0xFF);
        SetStatus(VgcConstants.XmcStatusOk, VgcConstants.XmcErrNone);
    }

    private void DoNamedDelete()
    {
        string? name = ReadNameFromRegs();
        if (name is null)
        {
            SetStatus(VgcConstants.XmcStatusError, VgcConstants.XmcErrName);
            return;
        }

        if (!_handleByName.TryGetValue(name, out byte handle) || !_blocksByHandle.ContainsKey(handle))
        {
            SetStatus(VgcConstants.XmcStatusError, VgcConstants.XmcErrNotFound);
            return;
        }

        RemoveBlock(handle);
        RefreshStats();
        SetStatus(VgcConstants.XmcStatusOk, VgcConstants.XmcErrNone);
    }

    private void DoNamedDirOpen()
    {
        _dirHandles.Clear();
        foreach (BlockInfo block in _blocksByHandle.Values.Where(b => b.IsNamed).OrderBy(b => b.Name, StringComparer.OrdinalIgnoreCase))
            _dirHandles.Add(block.Handle);

        _dirCursor = 0;
        if (_dirHandles.Count == 0)
        {
            SetStatus(VgcConstants.XmcStatusError, VgcConstants.XmcErrEndOfDir);
            return;
        }

        EmitDirEntry(_dirHandles[0]);
        SetStatus(VgcConstants.XmcStatusOk, VgcConstants.XmcErrNone);
    }

    private void DoNamedDirRead()
    {
        if (_dirHandles.Count == 0)
        {
            SetStatus(VgcConstants.XmcStatusError, VgcConstants.XmcErrEndOfDir);
            return;
        }

        _dirCursor++;
        if (_dirCursor >= _dirHandles.Count)
        {
            SetStatus(VgcConstants.XmcStatusError, VgcConstants.XmcErrEndOfDir);
            return;
        }

        EmitDirEntry(_dirHandles[_dirCursor]);
        SetStatus(VgcConstants.XmcStatusOk, VgcConstants.XmcErrNone);
    }

    private void EmitDirEntry(byte handle)
    {
        if (!_blocksByHandle.TryGetValue(handle, out BlockInfo? block))
            return;

        _regs[RegIndex(VgcConstants.XmcHandle)] = handle;
        SetXAddr(block.Addr);
        SetLength(block.Length);
        WriteNameToRegs(block.Name);
    }

    private bool TryAllocateNamedBlock(string name, int len, int pages, out BlockInfo block)
    {
        block = null!;

        int firstPage = FindFreeRun(pages);
        if (firstPage < 0)
            return false;

        byte handle = AllocateHandle();
        if (handle == 0)
            return false;

        int addr = firstPage * PageSize;
        MarkPagesUsed(firstPage, pages);

        block = new BlockInfo
        {
            Handle = handle,
            Name = name,
            Addr = addr,
            Length = len,
            Pages = pages
        };

        _blocksByHandle[handle] = block;
        _handleByName[name] = handle;
        return true;
    }

    private void RemoveOverlappingBlocks(int addr, int len)
    {
        int end = addr + len;
        List<byte> remove = [];

        foreach (BlockInfo block in _blocksByHandle.Values)
        {
            int blockEnd = block.Addr + block.Pages * PageSize;
            if (addr < blockEnd && block.Addr < end)
                remove.Add(block.Handle);
        }

        foreach (byte handle in remove)
            RemoveBlock(handle);
    }

    private void RemoveBlock(byte handle)
    {
        if (!_blocksByHandle.TryGetValue(handle, out BlockInfo? block))
            return;

        MarkRangeFree(block.Addr, block.Pages * PageSize);
        _blocksByHandle.Remove(handle);

        if (block.IsNamed)
            _handleByName.Remove(block.Name);
    }

    private static int RegIndex(int address) => address - VgcConstants.XmcBase;

    private int GetXAddr() =>
        _regs[RegIndex(VgcConstants.XmcAddrL)]
        | (_regs[RegIndex(VgcConstants.XmcAddrM)] << 8)
        | (_regs[RegIndex(VgcConstants.XmcAddrH)] << 16);

    private void SetXAddr(int addr)
    {
        _regs[RegIndex(VgcConstants.XmcAddrL)] = (byte)(addr & 0xFF);
        _regs[RegIndex(VgcConstants.XmcAddrM)] = (byte)((addr >> 8) & 0xFF);
        _regs[RegIndex(VgcConstants.XmcAddrH)] = (byte)((addr >> 16) & 0xFF);
    }

    private int GetRamAddr() =>
        _regs[RegIndex(VgcConstants.XmcRamL)]
        | (_regs[RegIndex(VgcConstants.XmcRamH)] << 8);

    private int GetLength() =>
        _regs[RegIndex(VgcConstants.XmcLenL)]
        | (_regs[RegIndex(VgcConstants.XmcLenH)] << 8);

    private void SetLength(int len)
    {
        _regs[RegIndex(VgcConstants.XmcLenL)] = (byte)(len & 0xFF);
        _regs[RegIndex(VgcConstants.XmcLenH)] = (byte)((len >> 8) & 0xFF);
    }

    private string? ReadNameFromRegs()
    {
        int maxNameBytes = VgcConstants.XmcNameEnd - VgcConstants.XmcName + 1;
        int len = _regs[RegIndex(VgcConstants.XmcNameLen)];
        if (len <= 0 || len > maxNameBytes)
            return null;

        Span<byte> tmp = stackalloc byte[len];
        for (int i = 0; i < len; i++)
            tmp[i] = _regs[RegIndex(VgcConstants.XmcName + i)];

        string name = Encoding.ASCII.GetString(tmp).Trim();
        if (string.IsNullOrWhiteSpace(name))
            return null;

        return name;
    }

    private void WriteNameToRegs(string name)
    {
        int maxNameBytes = VgcConstants.XmcNameEnd - VgcConstants.XmcName + 1;
        int len = Math.Min(name.Length, maxNameBytes);

        _regs[RegIndex(VgcConstants.XmcNameLen)] = (byte)len;
        for (int i = 0; i < maxNameBytes; i++)
            _regs[RegIndex(VgcConstants.XmcName + i)] = 0;

        for (int i = 0; i < len; i++)
            _regs[RegIndex(VgcConstants.XmcName + i)] = (byte)name[i];
    }

    private bool RangeOk(int addr, int len)
    {
        if (addr < 0 || len < 0)
            return false;

        long end = (long)addr + len;
        return addr < _xram.Length && end <= _xram.Length;
    }

    private static bool RamRangeOk(int addr, int len, bool forWrite)
    {
        if (addr < 0 || len < 0)
            return false;

        int end = addr + len;
        if (end > 0x10000)
            return false;

        // Prevent writes into ROM space. Reads may include ROM (for stash/code capture).
        if (forWrite && end > VgcConstants.RomBase)
            return false;

        return true;
    }

    private int FindFreeRun(int pages)
    {
        int runStart = -1;
        int runLength = 0;

        for (int page = 0; page < _usedPages.Length; page++)
        {
            if (_usedPages[page])
            {
                runStart = -1;
                runLength = 0;
                continue;
            }

            if (runStart < 0)
                runStart = page;

            runLength++;
            if (runLength >= pages)
                return runStart;
        }

        return -1;
    }

    private byte AllocateHandle()
    {
        for (int i = 0; i < 255; i++)
        {
            byte h = _nextHandle;
            _nextHandle = (byte)(_nextHandle == 255 ? 1 : _nextHandle + 1);
            if (!_blocksByHandle.ContainsKey(h))
                return h;
        }

        return 0;
    }

    private static int PagesForLength(int len) => (len + PageSize - 1) / PageSize;

    private void MarkPagesUsed(int firstPage, int pages)
    {
        for (int i = 0; i < pages; i++)
        {
            int page = firstPage + i;
            if (_usedPages[page])
                continue;

            _usedPages[page] = true;
            _usedPageCount++;
        }
    }

    private void MarkRangeUsed(int addr, int len)
    {
        int firstPage = addr / PageSize;
        int lastPage = (addr + len - 1) / PageSize;
        MarkPagesUsed(firstPage, lastPage - firstPage + 1);
    }

    private void MarkRangeFree(int addr, int len)
    {
        int firstPage = addr / PageSize;
        int lastPage = (addr + len - 1) / PageSize;

        for (int page = firstPage; page <= lastPage; page++)
        {
            if (!_usedPages[page])
                continue;

            _usedPages[page] = false;
            _usedPageCount--;
        }
    }

    private void ResetUsage()
    {
        Array.Fill(_usedPages, false);
        _usedPageCount = 0;
        _blocksByHandle.Clear();
        _handleByName.Clear();
        _dirHandles.Clear();
        _dirCursor = 0;
        _regs[RegIndex(VgcConstants.XmcHandle)] = 0;
    }

    private void RefreshStats()
    {
        int totalPages = _xram.Length / PageSize;
        int usedPages = _usedPageCount;
        int freePages = totalPages - usedPages;
        int namedCount = _handleByName.Count;

        _regs[RegIndex(VgcConstants.XmcPagesUsedL)] = (byte)(usedPages & 0xFF);
        _regs[RegIndex(VgcConstants.XmcPagesUsedH)] = (byte)((usedPages >> 8) & 0xFF);
        _regs[RegIndex(VgcConstants.XmcPagesFreeL)] = (byte)(freePages & 0xFF);
        _regs[RegIndex(VgcConstants.XmcPagesFreeH)] = (byte)((freePages >> 8) & 0xFF);
        _regs[RegIndex(VgcConstants.XmcDirCountL)] = (byte)(namedCount & 0xFF);
        _regs[RegIndex(VgcConstants.XmcDirCountH)] = (byte)((namedCount >> 8) & 0xFF);
    }

    private void SetStatus(byte status, byte err)
    {
        _regs[RegIndex(VgcConstants.XmcStatus)] = status;
        _regs[RegIndex(VgcConstants.XmcErrCode)] = err;
    }

    private bool IsWindowMapped(int window)
    {
        if ((uint)window > 3)
            return false;

        byte ctl = _regs[RegIndex(VgcConstants.XmcWinCtl)];
        return (ctl & (1 << window)) != 0;
    }

    private int GetWindowBase(int window)
    {
        int regBase = window switch
        {
            0 => VgcConstants.XmcWin0AL,
            1 => VgcConstants.XmcWin1AL,
            2 => VgcConstants.XmcWin2AL,
            3 => VgcConstants.XmcWin3AL,
            _ => VgcConstants.XmcWin0AL
        };

        int low = _regs[RegIndex(regBase)];
        int mid = _regs[RegIndex(regBase + 1)];
        int high = _regs[RegIndex(regBase + 2)];
        return (high << 16) | (mid << 8) | low;
    }

    private byte ReadWindowByte(ushort address)
    {
        int window = (address - WindowBase) >> 8;
        int offset = address & 0xFF;
        int xaddr = (GetWindowBase(window) & ~0xFF) + offset;

        if (!RangeOk(xaddr, 1))
            return 0;

        return _xram[xaddr];
    }

    private void WriteWindowByte(ushort address, byte data)
    {
        int window = (address - WindowBase) >> 8;
        int offset = address & 0xFF;
        int xaddr = (GetWindowBase(window) & ~0xFF) + offset;

        if (!RangeOk(xaddr, 1))
            return;

        _xram[xaddr] = data;
        MarkRangeUsed(xaddr, 1);
        RefreshStats();
    }
}

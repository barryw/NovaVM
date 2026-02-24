namespace e6502.Avalonia.Hardware;

using KDS.e6502;

public class CompositeBusDevice : IBusDevice, IDisposable
{
    private readonly byte[] _ram = new byte[65536];
    private readonly VirtualGraphicsController _vgc = new();
    private readonly SidChip _sid;
    private readonly SidChip _sid2;
    private readonly FileIoController _fio;
    private readonly VirtualExpansionMemoryController _xmc;
    private readonly VirtualTimerController _timer = new();
    private readonly VirtualNetworkController _nic;
    private readonly VirtualDmaController _dma;
    private readonly VirtualBlitterController _blitter;
    private readonly SidPlayer _sidPlayer;
    private readonly MusicEngine _musicEngine;
    private readonly int _cpuHz;
    private readonly int _frameRateHz;
    private long _frameNumeratorAccumulator;
    private long _totalFrames;
    private bool _rasterIrqPending;

    public VirtualGraphicsController Vgc => _vgc;
    public SidChip Sid => _sid;
    public SidChip Sid2 => _sid2;
    public FileIoController Fio => _fio;
    public VirtualTimerController Timer => _timer;
    public VirtualNetworkController Nic => _nic;
    public VirtualDmaController Dma => _dma;
    public VirtualBlitterController Blitter => _blitter;
    public SidPlayer SidPlayer => _sidPlayer;
    public MusicEngine Music => _musicEngine;
    public int CpuHz => _cpuHz;
    public int FrameRateHz => _frameRateHz;
    public long TotalFrames => _totalFrames;


    public CompositeBusDevice(
        bool enableSound = false,
        int cpuHz = VgcConstants.DefaultCpuHz,
        int frameRateHz = VgcConstants.FrameRateHz)
    {
        _cpuHz = cpuHz > 0 ? cpuHz : VgcConstants.DefaultCpuHz;
        _frameRateHz = frameRateHz > 0 ? frameRateHz : VgcConstants.FrameRateHz;

        _sid = new SidChip(enableSound);
        _sid2 = new SidChip(enableSound, baseAddress: 0xD420);
        _sidPlayer = new SidPlayer(this);
        _musicEngine = new MusicEngine(this);

        _fio = new FileIoController(
            addr => _ram[addr],
            (addr, data) => _ram[addr] = data,
            saveDir: null,
            vgcRead: (space, offset) => _vgc.TryReadMemorySpace(space, offset, out byte value) ? value : (byte)0,
            vgcWrite: (space, offset, value) => _vgc.TryWriteMemorySpace(space, offset, value),
            vgcSpaceLength: space => _vgc.GetMemorySpaceLength(space),
            sidPlayer: _sidPlayer,
            musicEngine: _musicEngine);
        _xmc = new VirtualExpansionMemoryController(
            addr => _ram[addr],
            (addr, data) => _ram[addr] = data);
        _nic = new VirtualNetworkController(
            addr => _ram[addr],
            (addr, data) => _ram[addr] = data);
        _dma = new VirtualDmaController(
            getSpaceLength: GetDmaSpaceLength,
            tryReadByte: TryDmaReadByte,
            tryWriteByte: TryDmaWriteByte,
            canWriteRange: CanDmaWriteRange,
            postTransferWrite: PostDmaWrite);
        _blitter = new VirtualBlitterController(
            getSpaceLength: GetDmaSpaceLength,
            tryReadByte: TryDmaReadByte,
            tryWriteByte: TryDmaWriteByte,
            canWriteRange: CanDmaWriteRange,
            postTransferWrite: PostDmaWrite);

        string romPath = Path.Combine(AppContext.BaseDirectory, "Resources", "ehbasic.bin");
        byte[] rom = File.ReadAllBytes(romPath);
        rom.CopyTo(_ram, VgcConstants.RomBase);

        InitVectorTable();
    }

    /// <summary>Write directly to backing RAM, bypassing ROM protection and hardware routing.</summary>
    public void WriteRam(ushort address, byte data) => _ram[address] = data;

    public void Dispose()
    {
        _nic.Dispose();
        _sid.Dispose();
        _sid2.Dispose();
    }

    private void InitVectorTable()
    {
        void WriteWord(int addr, int value)
        {
            _ram[addr]     = (byte)(value & 0xFF);
            _ram[addr + 1] = (byte)((value >> 8) & 0xFF);
        }

        WriteWord(VgcConstants.VectorTableBase + 0x00, VgcConstants.VgcBase);
        WriteWord(VgcConstants.VectorTableBase + 0x02, VgcConstants.RegCmd);
        WriteWord(VgcConstants.VectorTableBase + 0x04, VgcConstants.CharRamBase);
        WriteWord(VgcConstants.VectorTableBase + 0x06, VgcConstants.ColorRamBase);
        WriteWord(VgcConstants.VectorTableBase + 0x08, VgcConstants.SidBase);
        WriteWord(VgcConstants.VectorTableBase + 0x0A, VgcConstants.FioBase);
        WriteWord(VgcConstants.VectorTableBase + 0x0C, VgcConstants.XmcBase);
        WriteWord(VgcConstants.VectorTableBase + 0x0E, VgcConstants.TimerBase);
        WriteWord(VgcConstants.VectorTableBase + 0x10, VgcConstants.Sid2Base);
        WriteWord(VgcConstants.VectorTableBase + 0x12, VgcConstants.NicBase);
        WriteWord(VgcConstants.VectorTableBase + 0x14, VgcConstants.DmaBase);
        WriteWord(VgcConstants.VectorTableBase + 0x16, VgcConstants.BltBase);
    }

    public byte Read(ushort address)
    {
        if (address == VgcConstants.MusicStatus)
            return (byte)((_musicEngine.IsPlaying ? 1 : 0) | (_musicEngine.IsMusicPlaying ? 2 : 0));
        if (address >= VgcConstants.MusicNote1 && address <= VgcConstants.MusicNote6)
            return _musicEngine.GetVoiceNote(address - VgcConstants.MusicNote1);
        if (_timer.OwnsAddress(address)) return _timer.Read(address);
        if (_nic.OwnsAddress(address)) return _nic.Read(address);
        if (_dma.OwnsAddress(address)) return _dma.Read(address);
        if (_blitter.OwnsAddress(address)) return _blitter.Read(address);
        if (_xmc.OwnsAddress(address)) return _xmc.Read(address);
        if (_fio.OwnsAddress(address)) return _fio.Read(address);
        if (_vgc.OwnsAddress(address)) return _vgc.Read(address);
        return _ram[address];
    }

    public void Write(ushort address, byte data)
    {
        if (_timer.OwnsAddress(address)) { _timer.Write(address, data); return; }
        if (_nic.OwnsAddress(address)) { _nic.Write(address, data); return; }
        if (_dma.OwnsAddress(address)) { _dma.Write(address, data); return; }
        if (_blitter.OwnsAddress(address)) { _blitter.Write(address, data); return; }
        if (_xmc.OwnsAddress(address)) { _xmc.Write(address, data); return; }
        if (_fio.OwnsAddress(address)) { _fio.Write(address, data); return; }
        if (_vgc.OwnsAddress(address))
        {
            _vgc.Write(address, data);
            if (_vgc.SysResetRequested)
            {
                _vgc.SysResetRequested = false;
                _musicEngine.MusicStop();
                _sid.Write(0xD404, 0x00); // gate off voice 1
                _sid.Write(0xD40B, 0x00); // gate off voice 2
                _sid.Write(0xD412, 0x00); // gate off voice 3
                _sid.Write(0xD418, 0x00); // silence master volume
                _sid2.Write(0xD424, 0x00); // gate off voice 4
                _sid2.Write(0xD42B, 0x00); // gate off voice 5
                _sid2.Write(0xD432, 0x00); // gate off voice 6
                _sid2.Write(0xD438, 0x00); // silence SID2 volume
                _nic.ResetAll();
            }
            return;
        }
        if (_sid.OwnsAddress(address)) { _sid.Write(address, data); return; }
        if (_sid2.OwnsAddress(address)) { _sid2.Write(address, data); return; }
        // Transparent mirror: $D500-$D51C → remap to SID2 at $D420
        if (address >= VgcConstants.Sid2MirrorBase && address <= VgcConstants.Sid2MirrorEnd)
        {
            _sid2.Write((ushort)(VgcConstants.Sid2Base + (address - VgcConstants.Sid2MirrorBase)), data);
            return;
        }
        // ROM write protection: drop writes to $C000-$FFF9.
        // Hardware vectors $FFFA-$FFFF pass through (RSID players install own IRQ).
        if (address >= VgcConstants.RomBase && address < 0xFFFA) return;
        _ram[address] = data;
    }

    public void AdvanceCycles(int cycles)
    {
        if (cycles <= 0) return;

        _timer.AdvanceCycles(cycles);
        _dma.AdvanceCycles(cycles);
        _blitter.AdvanceCycles(cycles);

        _frameNumeratorAccumulator += (long)cycles * _frameRateHz;
        while (_frameNumeratorAccumulator >= _cpuHz)
        {
            _frameNumeratorAccumulator -= _cpuHz;
            _totalFrames++;
            _vgc.IncrementFrameCounter();
            _musicEngine.Tick();
            if (_vgc.IsRasterIrqEnabled)
                _rasterIrqPending = true;
        }
    }

    public bool ConsumeRasterIrqPending()
    {
        if (!_rasterIrqPending) return false;
        _rasterIrqPending = false;
        return true;
    }

    private int GetDmaSpaceLength(byte space) =>
        space switch
        {
            VgcConstants.DmaSpaceCpuRam => 0x10000,
            VgcConstants.DmaSpaceVgcChar => _vgc.GetMemorySpaceLength(VgcConstants.MemSpaceScreen),
            VgcConstants.DmaSpaceVgcColor => _vgc.GetMemorySpaceLength(VgcConstants.MemSpaceColor),
            VgcConstants.DmaSpaceVgcGfx => _vgc.GetMemorySpaceLength(VgcConstants.MemSpaceGfx),
            VgcConstants.DmaSpaceVgcSprite => _vgc.GetMemorySpaceLength(VgcConstants.MemSpaceSprite),
            VgcConstants.DmaSpaceXram => _xmc.CapacityBytes,
            _ => 0
        };

    private (bool ok, byte value) TryDmaReadByte(byte space, int address)
    {
        switch (space)
        {
            case VgcConstants.DmaSpaceCpuRam:
                if ((uint)address >= 0x10000)
                    return (false, 0);
                return (true, _ram[address]);

            case VgcConstants.DmaSpaceVgcChar:
                return _vgc.TryReadMemorySpace(VgcConstants.MemSpaceScreen, address, out byte screen)
                    ? (true, screen)
                    : (false, (byte)0);

            case VgcConstants.DmaSpaceVgcColor:
                return _vgc.TryReadMemorySpace(VgcConstants.MemSpaceColor, address, out byte color)
                    ? (true, color)
                    : (false, (byte)0);

            case VgcConstants.DmaSpaceVgcGfx:
                return _vgc.TryReadMemorySpace(VgcConstants.MemSpaceGfx, address, out byte gfx)
                    ? (true, gfx)
                    : (false, (byte)0);

            case VgcConstants.DmaSpaceVgcSprite:
                return _vgc.TryReadMemorySpace(VgcConstants.MemSpaceSprite, address, out byte sprite)
                    ? (true, sprite)
                    : (false, (byte)0);

            case VgcConstants.DmaSpaceXram:
                return _xmc.TryReadLinear(address, out byte xram)
                    ? (true, xram)
                    : (false, (byte)0);

            default:
                return (false, 0);
        }
    }

    private bool TryDmaWriteByte(byte space, int address, byte value)
    {
        switch (space)
        {
            case VgcConstants.DmaSpaceCpuRam:
                return TryWriteCpuRamByte(address, value);

            case VgcConstants.DmaSpaceVgcChar:
                return _vgc.TryWriteMemorySpace(VgcConstants.MemSpaceScreen, address, value);

            case VgcConstants.DmaSpaceVgcColor:
                return _vgc.TryWriteMemorySpace(VgcConstants.MemSpaceColor, address, value);

            case VgcConstants.DmaSpaceVgcGfx:
                return _vgc.TryWriteMemorySpace(VgcConstants.MemSpaceGfx, address, value);

            case VgcConstants.DmaSpaceVgcSprite:
                return _vgc.TryWriteMemorySpace(VgcConstants.MemSpaceSprite, address, value);

            case VgcConstants.DmaSpaceXram:
                return _xmc.TryWriteLinear(address, value);

            default:
                return false;
        }
    }

    private static bool CanDmaWriteRange(byte space, int address, int length)
    {
        if (address < 0 || length < 0)
            return false;

        long end = (long)address + length;
        if (space != VgcConstants.DmaSpaceCpuRam)
            return true;

        // CPU RAM writes are allowed either below ROM or in hardware vectors
        // ($FFFA-$FFFF). Writes into ROM body are write-protected.
        if (end <= VgcConstants.RomBase)
            return true;
        if (address >= 0xFFFA && end <= 0x10000)
            return true;
        return false;
    }

    private void PostDmaWrite(byte dstSpace)
    {
        if (dstSpace == VgcConstants.DmaSpaceXram)
            _xmc.RefreshStatsRegisters();
    }

    private bool TryWriteCpuRamByte(int address, byte data)
    {
        if ((uint)address >= 0x10000)
            return false;

        if (address >= VgcConstants.RomBase && address < 0xFFFA)
            return false;

        _ram[address] = data;
        return true;
    }
}

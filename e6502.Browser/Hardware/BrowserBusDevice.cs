namespace e6502.Avalonia.Hardware;

using System.Diagnostics;
using System.Reflection;
using e6502.Storage;
using KDS.e6502;

/// <summary>
/// Browser-adapted CompositeBusDevice. Loads ROMs and demo disk from embedded
/// resources instead of filesystem. No NCC compiler, no help system, no TCP.
/// Named CompositeBusDevice so linked SidPlayer/MusicEngine resolve without changes.
/// </summary>
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
    private readonly MidiPlayback _midiPlayback;
    private readonly WavetableSynth _wts;
    private readonly DeviceManager _deviceManager;
    private readonly int _cpuHz;
    private readonly int _frameRateHz;
    private long _frameNumeratorAccumulator;
    private long _totalFrames;
    private bool _rasterIrqPending;
    private long _lastMusicWallTick;
    private double _musicFrameAccum;
    private readonly byte[] _basicRom;
    private readonly byte[]? _extRom;

    public VirtualGraphicsController Vgc => _vgc;
    public SidChip Sid => _sid;
    public SidChip Sid2 => _sid2;
    public FileIoController Fio => _fio;
    public VirtualTimerController Timer => _timer;
    public VirtualNetworkController Nic => _nic;
    public SidPlayer SidPlayer => _sidPlayer;
    public MusicEngine Music => _musicEngine;
    public MidiPlayback MidiPlayback => _midiPlayback;
    public WavetableSynth Wts => _wts;
    public int CpuHz => _cpuHz;
    public int FrameRateHz => _frameRateHz;
    public long TotalFrames => _totalFrames;

    public CompositeBusDevice(
        int cpuHz = VgcConstants.DefaultCpuHz,
        int frameRateHz = VgcConstants.FrameRateHz)
    {
        _cpuHz = cpuHz > 0 ? cpuHz : VgcConstants.DefaultCpuHz;
        _frameRateHz = frameRateHz > 0 ? frameRateHz : VgcConstants.FrameRateHz;

        _sid = new SidChip(enableAudio: true);
        _sid2 = new SidChip(enableAudio: true, baseAddress: 0xD420);
        _wts = new WavetableSynth(enableAudio: true);
        _sidPlayer = new SidPlayer(this);
        _musicEngine = new MusicEngine(this);
        _midiPlayback = new MidiPlayback(_musicEngine, _frameRateHz);

        // Browser: use a temporary in-memory directory structure
        string tempDir = Path.GetTempPath();
        string hd0 = Path.Combine(tempDir, "e6502-programs");
        string hd1 = Path.Combine(tempDir, "e6502-data");
        string disks = Path.Combine(tempDir, "e6502-disks");
        _deviceManager = new DeviceManager(hd0, hd1, disks);

        // Mount demo.ndi as FD0 from embedded resource
        MountDemoDisk();

        // Set FD0 as default device so autoboot finds it
        _deviceManager.DefaultDevice = "FD0";

        _fio = new FileIoController(
            addr => _ram[addr],
            (addr, data) => _ram[addr] = data,
            saveDir: hd0,
            vgcRead: (space, offset) => _vgc.TryReadMemorySpace(space, offset, out byte value) ? value : (byte)0,
            vgcWrite: (space, offset, value) => _vgc.TryWriteMemorySpace(space, offset, value),
            vgcSpaceLength: space => _vgc.GetMemorySpaceLength(space),
            sidPlayer: _sidPlayer,
            musicEngine: _musicEngine,
            midiPlayback: _midiPlayback,
            deviceManager: _deviceManager,
            wts: _wts,
            vgc: _vgc);
        _vgc.SetBusMemory(_ram);

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

        // Load ROM from embedded resource
        byte[] rom = LoadEmbeddedResource("ehbasic.bin");
        rom.CopyTo(_ram, VgcConstants.RomBase);

        _basicRom = new byte[16384];
        Array.Copy(_ram, VgcConstants.RomBase, _basicRom, 0, 16384);

        // Extension ROM
        byte[]? extData = TryLoadEmbeddedResource("extension.bin");
        if (extData != null)
        {
            _extRom = new byte[16384];
            Array.Copy(extData, _extRom, Math.Min(extData.Length, 16384));
        }

        InitVectorTable();
    }

    private void MountDemoDisk()
    {
        byte[]? demoData = TryLoadEmbeddedResource("demo.ndi");
        if (demoData != null)
        {
            var fd0 = _deviceManager.GetDevice("FD0") as NdiFloppyDevice;
            fd0?.MountFromBytes(demoData);
        }
    }

    private static byte[] LoadEmbeddedResource(string name)
    {
        var asm = typeof(CompositeBusDevice).Assembly;
        using var stream = asm.GetManifestResourceStream(name)
            ?? throw new FileNotFoundException($"Embedded resource '{name}' not found.");
        using var ms = new MemoryStream();
        stream.CopyTo(ms);
        return ms.ToArray();
    }

    private static byte[]? TryLoadEmbeddedResource(string name)
    {
        var asm = typeof(CompositeBusDevice).Assembly;
        using var stream = asm.GetManifestResourceStream(name);
        if (stream == null) return null;
        using var ms = new MemoryStream();
        stream.CopyTo(ms);
        return ms.ToArray();
    }

    public byte ReadRam(ushort address) => _ram[address];
    public void WriteRam(ushort address, byte data) => _ram[address] = data;

    public void Dispose()
    {
        _nic.Dispose();
        _sid.Dispose();
        _sid2.Dispose();
        _wts.Dispose();
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
        WriteWord(VgcConstants.VectorTableBase + 0x04, VgcConstants.VramRegBase);
        WriteWord(VgcConstants.VectorTableBase + 0x06, VgcConstants.VramData);
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
            return (byte)((_musicEngine.IsPlaying ? 1 : 0)
                | (_musicEngine.IsMusicPlaying || _midiPlayback.IsPlaying || _sidPlayer.IsPlaying ? 2 : 0));
        if (address >= VgcConstants.MusicNote1 && address <= VgcConstants.MusicNote14)
            return _musicEngine.GetVoiceNote(address - VgcConstants.MusicNote1);
        if (address >= VgcConstants.MusicElapsedL && address <= VgcConstants.MusicTotalH)
        {
            int elapsed = _sidPlayer.IsPlaying ? _sidPlayer.ElapsedFrames : _musicEngine.ElapsedFrames;
            int total = _musicEngine.TotalFrames;
            return address switch
            {
                VgcConstants.MusicElapsedL => (byte)(elapsed & 0xFF),
                VgcConstants.MusicElapsedH => (byte)((elapsed >> 8) & 0xFF),
                VgcConstants.MusicTotalL   => (byte)(total & 0xFF),
                VgcConstants.MusicTotalH   => (byte)((total >> 8) & 0xFF),
                _ => 0
            };
        }
        if (_timer.OwnsAddress(address)) return _timer.Read(address);
        if (_nic.OwnsAddress(address)) return _nic.Read(address);
        if (_dma.OwnsAddress(address)) return _dma.Read(address);
        if (_blitter.OwnsAddress(address)) return _blitter.Read(address);
        if (_xmc.OwnsAddress(address)) return _xmc.Read(address);
        if (_fio.OwnsAddress(address)) return _fio.Read(address);
        if (address >= VgcConstants.WtsBase && address <= VgcConstants.WtsEnd)
            return _wts.ReadRegister(address);
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
        // Help register — ignore in browser (no help panel)
        if (address >= VgcConstants.HelpSearchBuffer &&
            address < VgcConstants.HelpSearchBuffer + VgcConstants.HelpSearchBufferLen)
        {
            _ram[address] = data;
            return;
        }
        if (address == VgcConstants.RegHelp) return;
        // ROM swap — support basic ↔ extension only (no NCC in browser)
        if (address == VgcConstants.RegRomSwap)
        {
            if (data == VgcConstants.RomSwapBasic)
            {
                Array.Copy(_basicRom, 0, _ram, VgcConstants.RomBase, 16384);
            }
            else if (data == VgcConstants.RomSwapExtension && _extRom != null)
            {
                Array.Copy(_extRom, 0, _ram, VgcConstants.RomBase, 16384);
            }
            return;
        }
        if (address >= VgcConstants.WtsBase && address <= VgcConstants.WtsEnd)
        {
            _wts.WriteRegister(address, data);
            return;
        }
        if (_vgc.OwnsAddress(address))
        {
            if (address == VgcConstants.RegCmd && data == VgcConstants.CmdGtext)
            {
                int len = _fio.Read(VgcConstants.FioNameLen);
                _ram[VgcConstants.FioNameLen] = (byte)len;
                for (int i = 0; i < len; i++)
                    _ram[VgcConstants.FioName + i] = _fio.Read((ushort)(VgcConstants.FioName + i));
            }
            _vgc.Write(address, data);
            if (_vgc.SysResetRequested)
            {
                _vgc.SysResetRequested = false;
                _midiPlayback.Stop();
                _musicEngine.MusicReset();
                _wts.AllNotesOff();
                _sid.Write(0xD404, 0x00);
                _sid.Write(0xD40B, 0x00);
                _sid.Write(0xD412, 0x00);
                _sid.Write(0xD418, 0x00);
                _sid2.Write(0xD424, 0x00);
                _sid2.Write(0xD42B, 0x00);
                _sid2.Write(0xD432, 0x00);
                _sid2.Write(0xD438, 0x00);
                _nic.ResetAll();
            }
            return;
        }
        if (_sid.OwnsAddress(address)) { _sid.Write(address, data); return; }
        if (_sid2.OwnsAddress(address)) { _sid2.Write(address, data); return; }
        if (address >= VgcConstants.Sid2MirrorBase && address <= VgcConstants.Sid2MirrorEnd)
        {
            _sid2.Write((ushort)(VgcConstants.Sid2Base + (address - VgcConstants.Sid2MirrorBase)), data);
            return;
        }
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

            long now = Stopwatch.GetTimestamp();
            if (_lastMusicWallTick == 0) _lastMusicWallTick = now;
            double wallElapsed = (double)(now - _lastMusicWallTick) / Stopwatch.Frequency;
            _lastMusicWallTick = now;
            _musicFrameAccum += wallElapsed * _frameRateHz;
            if (_musicFrameAccum >= 1.0)
            {
                int musicTicks = Math.Min((int)_musicFrameAccum, 2);
                _musicFrameAccum -= musicTicks;
                for (int mt = 0; mt < musicTicks; mt++)
                {
                    _musicEngine.Tick();
                    _midiPlayback.Tick();
                    if (_sidPlayer.IsPlaying)
                    {
                        _sidPlayer.SetPlayPending();
                        SniffSidNotes();
                        _sidPlayer.IncrementElapsed();
                    }
                }
            }

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

    private void SniffSidNotes()
    {
        for (int v = 0; v < 3; v++)
        {
            ushort regBase = (ushort)(VgcConstants.SidBase + v * 7);
            byte ctrl = Sid.Read((ushort)(regBase + 4));
            if ((ctrl & 0x01) != 0)
            {
                int freqLo = Sid.Read(regBase);
                int freqHi = Sid.Read((ushort)(regBase + 1));
                int sidFreq = freqLo | (freqHi << 8);
                int midi = MusicEngine.SidFreqToMidi(sidFreq);
                _musicEngine.SetVoiceNoteExternal(v, midi);
            }
            else
            {
                _musicEngine.SetVoiceNoteExternal(v, -1);
            }
        }
        for (int v = 0; v < 3; v++)
        {
            ushort regBase = (ushort)(VgcConstants.Sid2Base + v * 7);
            byte ctrl = Sid2.Read((ushort)(regBase + 4));
            if ((ctrl & 0x01) != 0)
            {
                int freqLo = Sid2.Read(regBase);
                int freqHi = Sid2.Read((ushort)(regBase + 1));
                int sidFreq = freqLo | (freqHi << 8);
                int midi = MusicEngine.SidFreqToMidi(sidFreq);
                _musicEngine.SetVoiceNoteExternal(v + 3, midi);
            }
            else
            {
                _musicEngine.SetVoiceNoteExternal(v + 3, -1);
            }
        }
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
                return (uint)address < 0x10000 ? (true, _ram[address]) : (false, (byte)0);
            case VgcConstants.DmaSpaceVgcChar:
                return _vgc.TryReadMemorySpace(VgcConstants.MemSpaceScreen, address, out byte screen) ? (true, screen) : (false, (byte)0);
            case VgcConstants.DmaSpaceVgcColor:
                return _vgc.TryReadMemorySpace(VgcConstants.MemSpaceColor, address, out byte color) ? (true, color) : (false, (byte)0);
            case VgcConstants.DmaSpaceVgcGfx:
                return _vgc.TryReadMemorySpace(VgcConstants.MemSpaceGfx, address, out byte gfx) ? (true, gfx) : (false, (byte)0);
            case VgcConstants.DmaSpaceVgcSprite:
                return _vgc.TryReadMemorySpace(VgcConstants.MemSpaceSprite, address, out byte sprite) ? (true, sprite) : (false, (byte)0);
            case VgcConstants.DmaSpaceXram:
                return _xmc.TryReadLinear(address, out byte xram) ? (true, xram) : (false, (byte)0);
            default:
                return (false, 0);
        }
    }

    private bool TryDmaWriteByte(byte space, int address, byte value)
    {
        switch (space)
        {
            case VgcConstants.DmaSpaceCpuRam:
                if ((uint)address >= 0x10000) return false;
                if (address >= VgcConstants.RomBase && address < 0xFFFA) return false;
                _ram[address] = value;
                return true;
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
        if (address < 0 || length < 0) return false;
        long end = (long)address + length;
        if (space != VgcConstants.DmaSpaceCpuRam) return true;
        if (end <= VgcConstants.RomBase) return true;
        if (address >= 0xFFFA && end <= 0x10000) return true;
        return false;
    }

    private void PostDmaWrite(byte dstSpace)
    {
        if (dstSpace == VgcConstants.DmaSpaceXram)
            _xmc.RefreshStatsRegisters();
    }
}

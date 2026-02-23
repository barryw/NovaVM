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
    private readonly SidPlayer _sidPlayer;
    private readonly MusicEngine _musicEngine;

    public VirtualGraphicsController Vgc => _vgc;
    public SidChip Sid => _sid;
    public SidChip Sid2 => _sid2;
    public FileIoController Fio => _fio;
    public VirtualTimerController Timer => _timer;
    public VirtualNetworkController Nic => _nic;
    public SidPlayer SidPlayer => _sidPlayer;
    public MusicEngine Music => _musicEngine;


    public CompositeBusDevice(bool enableSound = false)
    {
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
    }

    public byte Read(ushort address)
    {
        if (address == VgcConstants.MusicStatus)
            return (byte)((_musicEngine.IsPlaying ? 1 : 0) | (_musicEngine.IsMusicPlaying ? 2 : 0));
        if (address >= VgcConstants.MusicNote1 && address <= VgcConstants.MusicNote6)
            return _musicEngine.GetVoiceNote(address - VgcConstants.MusicNote1);
        if (_timer.OwnsAddress(address)) return _timer.Read(address);
        if (_nic.OwnsAddress(address)) return _nic.Read(address);
        if (_xmc.OwnsAddress(address)) return _xmc.Read(address);
        if (_fio.OwnsAddress(address)) return _fio.Read(address);
        if (_vgc.OwnsAddress(address)) return _vgc.Read(address);
        return _ram[address];
    }

    public void Write(ushort address, byte data)
    {
        if (_timer.OwnsAddress(address)) { _timer.Write(address, data); return; }
        if (_nic.OwnsAddress(address)) { _nic.Write(address, data); return; }
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
}

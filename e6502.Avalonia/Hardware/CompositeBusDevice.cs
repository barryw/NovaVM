namespace e6502.Avalonia.Hardware;

using KDS.e6502;

public class CompositeBusDevice : IBusDevice, IDisposable
{
    private readonly byte[] _ram = new byte[65536];
    private readonly VirtualGraphicsController _vgc = new();
    private readonly SidChip _sid;
    private readonly FileIoController _fio;
    private readonly VirtualExpansionMemoryController _xmc;
    private readonly VirtualTimerController _timer = new();

    public VirtualGraphicsController Vgc => _vgc;
    public SidChip Sid => _sid;
    public FileIoController Fio => _fio;
    public VirtualTimerController Timer => _timer;

    public CompositeBusDevice(bool enableSound = false)
    {
        _sid = new SidChip(enableSound);

        _fio = new FileIoController(
            addr => _ram[addr],
            (addr, data) => _ram[addr] = data,
            saveDir: null,
            vgcRead: (space, offset) => _vgc.TryReadMemorySpace(space, offset, out byte value) ? value : (byte)0,
            vgcWrite: (space, offset, value) => _vgc.TryWriteMemorySpace(space, offset, value),
            vgcSpaceLength: space => _vgc.GetMemorySpaceLength(space));
        _xmc = new VirtualExpansionMemoryController(
            addr => _ram[addr],
            (addr, data) => _ram[addr] = data);

        string romPath = Path.Combine(AppContext.BaseDirectory, "Resources", "ehbasic.bin");
        byte[] rom = File.ReadAllBytes(romPath);
        rom.CopyTo(_ram, VgcConstants.RomBase);

        InitVectorTable();
    }

    /// <summary>Write directly to backing RAM, bypassing ROM protection and hardware routing.</summary>
    public void WriteRam(ushort address, byte data) => _ram[address] = data;

    public void Dispose() =>
        _sid.Dispose();

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
    }

    public byte Read(ushort address)
    {
        if (_timer.OwnsAddress(address)) return _timer.Read(address);
        if (_xmc.OwnsAddress(address)) return _xmc.Read(address);
        if (_fio.OwnsAddress(address)) return _fio.Read(address);
        if (_vgc.OwnsAddress(address)) return _vgc.Read(address);
        return _ram[address];
    }

    public void Write(ushort address, byte data)
    {
        if (_timer.OwnsAddress(address)) { _timer.Write(address, data); return; }
        if (_xmc.OwnsAddress(address)) { _xmc.Write(address, data); return; }
        if (_fio.OwnsAddress(address)) { _fio.Write(address, data); return; }
        if (_vgc.OwnsAddress(address)) { _vgc.Write(address, data); return; }
        if (_sid.OwnsAddress(address)) { _sid.Write(address, data); return; }
        if (address >= VgcConstants.RomBase) return;
        _ram[address] = data;
    }
}

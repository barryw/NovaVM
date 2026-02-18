namespace e6502.Avalonia.Hardware;

using KDS.e6502;

public class CompositeBusDevice : IBusDevice
{
    private readonly byte[] _ram = new byte[65536];
    private readonly VirtualGraphicsController _vgc = new();
    private readonly VirtualSoundController _vsc = new();
    private readonly FileIoController _fio;

    public VirtualGraphicsController Vgc => _vgc;
    public FileIoController Fio => _fio;

    public CompositeBusDevice()
    {
        _fio = new FileIoController(
            addr => _ram[addr],
            (addr, data) => _ram[addr] = data);

        string romPath = Path.Combine(AppContext.BaseDirectory, "Resources", "ehbasic.bin");
        byte[] rom = File.ReadAllBytes(romPath);
        rom.CopyTo(_ram, VgcConstants.RomBase);
    }

    public byte Read(ushort address)
    {
        if (_fio.OwnsAddress(address)) return _fio.Read(address);
        if (_vsc.OwnsAddress(address)) return _vsc.Read(address);
        if (_vgc.OwnsAddress(address)) return _vgc.Read(address);
        return _ram[address];
    }

    public void Write(ushort address, byte data)
    {
        if (_fio.OwnsAddress(address)) { _fio.Write(address, data); return; }
        if (_vsc.OwnsAddress(address)) { _vsc.Write(address, data); return; }
        if (_vgc.OwnsAddress(address)) { _vgc.Write(address, data); return; }
        if (address >= VgcConstants.RomBase) return;
        _ram[address] = data;
    }
}

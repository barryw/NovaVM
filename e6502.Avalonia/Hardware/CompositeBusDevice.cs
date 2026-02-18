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

        InitVectorTable();
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
        WriteWord(VgcConstants.VectorTableBase + 0x08, VgcConstants.VscBase);
        WriteWord(VgcConstants.VectorTableBase + 0x0A, VgcConstants.FioBase);
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

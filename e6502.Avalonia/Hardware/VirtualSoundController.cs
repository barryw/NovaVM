namespace e6502.Avalonia.Hardware;

public class VirtualSoundController
{
    private readonly byte[] _registers = new byte[VgcConstants.VscEnd - VgcConstants.VscBase + 1];

    public bool OwnsAddress(ushort address) =>
        address >= VgcConstants.VscBase && address <= VgcConstants.VscEnd;

    public byte Read(ushort address) =>
        _registers[address - VgcConstants.VscBase];

    public void Write(ushort address, byte data) =>
        _registers[address - VgcConstants.VscBase] = data;
}

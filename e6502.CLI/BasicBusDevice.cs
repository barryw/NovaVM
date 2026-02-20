using KDS.e6502;

namespace e6502.CLI;

public class BasicBusDevice : IBusDevice
{
    private readonly byte[] _ram = new byte[0x10000]; // 64k of RAM
    private const string ResourcePath = @"Resources/";
    private const int ReadPort = 0xf004;
    private const int WritePort = 0xf001;
    private const int BasicStart = 0xc000;
    
    public BasicBusDevice()
    {
        // Load NovaBASIC ROM into memory @ $c000
        var basic = File.ReadAllBytes($"{ResourcePath}ehbasic.bin");
        basic.CopyTo(_ram, BasicStart);
    }
    
    public byte Read(ushort address)
    {
        var returnByte = _ram[address];
        if (address != ReadPort || _ram[ReadPort] == 0) return returnByte;
        
        returnByte = _ram[address];
        _ram[address] = 0;

        return returnByte;
    }

    public void Write(ushort address, byte data)
    {
        switch (address)
        {
            // Allow a write to $f001. This is the IO output port for NovaBASIC to write to the screen
            case WritePort:
                Console.Write(Convert.ToChar(data));
                break;
            case ReadPort:
                _ram[address] = data;
                break;
            default:
            {
                // Writes above $c000 are ignored
                if(address >= BasicStart)
                    return;

                _ram[address] = data;
                break;
            }
        }
    }
}

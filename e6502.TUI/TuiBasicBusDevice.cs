using KDS.e6502;
using Terminal.Gui;

namespace e6502.TUI;

public class TuiBasicBusDevice : IBusDevice
{
    private readonly byte[] _ram = new byte[0x10000]; // 64k of RAM
    private const string ResourcePath = @"Resources/";
    private readonly ConsoleView _display;
    
    public TuiBasicBusDevice(ConsoleView display)
    {
        _display = display;
        
        var basic = File.ReadAllBytes($"{ResourcePath}ehbasic.bin");
        basic.CopyTo(_ram, 0xc000);
    }
    
    public byte Read(ushort address)
    {
        var returnByte = _ram[address];
        if (address != 0xf004 || _ram[0xf004] == 0) return returnByte;
        
        returnByte = _ram[address];
        _ram[address] = 0;

        return returnByte;
    }

    public void Write(ushort address, byte data)
    {
        switch (address)
        {
            // Allow a write to $f001. This is the IO output port for Enhanced BASIC to write to the screen
            case 0xf001:
                Application.Invoke(() =>
                {
                    _display.AppendCharacter((char)data);
                });
                break;
            case 0xf004:
                _ram[address] = data;
                break;
            default:
            {
                // Writes above $c000 are ignored
                if(address >= 0xc000)
                    return;

                _ram[address] = data;
                break;
            }
        }
    }
}
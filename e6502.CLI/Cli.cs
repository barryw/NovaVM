using KDS.e6502;

namespace e6502.CLI;

internal class Cli
{
    private Cpu? _cpu;
    private readonly object _lock = new();
    
    public static int Main()
    {
        var cli = new Cli
        {
            _cpu = new Cpu(new BasicBusDevice())
        };
        cli._cpu.Boot();
        cli.Run();
        return 0;
    }

    private void Run()
    {
        var inputThread = new Thread(HandleInput);
        inputThread.Start();

        do
        {
            lock (_lock)
            {
                _cpu?.ExecuteNext();    
            }
        } while(inputThread.IsAlive);
        
        Console.WriteLine(@"Exiting NovaBASIC v1.0");
    }

    private void HandleInput()
    {
        bool quoteMode = true;
        
        while (true)
        {
            if(!Console.KeyAvailable) continue;
            var key = Console.ReadKey(true);
            if (key.Key == ConsoleKey.Escape)
                break;
            var pressed = key.Key switch
            {
                ConsoleKey.Backspace => (char)8, // BS
                ConsoleKey.F1 => (char)3, // CTRL-C
                _ => key.KeyChar
            };
            
            if(pressed == '"')
                quoteMode = !quoteMode;

            if (pressed == (char)13)
            {
                quoteMode = false;
            }
            
            if(!quoteMode && pressed is >= (char)97 and <= (char)122)
                pressed = (char)(pressed - 32);
            
            lock (_lock)
            {
                _cpu?.SystemBus.Write(0xf004, (byte)pressed);    
            }
        }
    }
}

using System.Diagnostics;
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

        int targetCpuHz = RuntimeOptions.GetIntFromEnvironment("NOVA_CPU_HZ", 12_000_000);
        bool turboMode = RuntimeOptions.GetFlagFromEnvironment("NOVA_TURBO");
        bool timingLog = RuntimeOptions.GetFlagFromEnvironment("NOVA_TIMING_LOG");
        var scheduler = new RealTimeCycleScheduler(
            targetCpuHz,
            maxBacklogCycles: Math.Max(targetCpuHz / 5, 100_000));
        const int turboChunkCycles = 200_000;
        long telemetryStartTicks = Stopwatch.GetTimestamp();
        long telemetryCycles = 0;
        double peakPendingCycles = 0;

        do
        {
            int cycleBudget = turboMode ? turboChunkCycles : scheduler.TakeCycleBudget();
            if (cycleBudget <= 0)
            {
                Thread.Yield();
                continue;
            }

            int chunkCyclesExecuted = 0;
            while (inputThread.IsAlive && cycleBudget > 0)
            {
                int cycles;
                lock (_lock)
                {
                    if (_cpu is null) break;
                    cycles = _cpu.ClocksForNext();
                    _cpu.ExecuteNext();
                }

                chunkCyclesExecuted += cycles;
                cycleBudget -= cycles;
            }

            if (!timingLog)
                continue;

            telemetryCycles += chunkCyclesExecuted;
            if (!turboMode && scheduler.PendingCycles > peakPendingCycles)
                peakPendingCycles = scheduler.PendingCycles;

            long nowTicks = Stopwatch.GetTimestamp();
            long elapsedTicks = nowTicks - telemetryStartTicks;
            if (elapsedTicks < Stopwatch.Frequency)
                continue;

            double elapsedSec = (double)elapsedTicks / Stopwatch.Frequency;
            double effectiveMhz = telemetryCycles / elapsedSec / 1_000_000.0;
            double backlogPct = turboMode || scheduler.MaxBacklogCycles == 0
                ? 0
                : peakPendingCycles * 100.0 / scheduler.MaxBacklogCycles;
            Console.WriteLine($"[VM CLOCK] mode={(turboMode ? "turbo" : "realtime")} targetHz={targetCpuHz} effMHz={effectiveMhz:F2} backlog={backlogPct:F1}%");

            telemetryStartTicks = nowTicks;
            telemetryCycles = 0;
            peakPendingCycles = 0;
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

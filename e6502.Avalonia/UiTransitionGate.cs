using System.Threading;

namespace e6502.Avalonia;

internal static class UiTransitionGate
{
    private static long _pauseUntilTick;

    public static bool IsPaused => Environment.TickCount64 < Volatile.Read(ref _pauseUntilTick);

    public static void PauseFor(int milliseconds)
    {
        long until = Environment.TickCount64 + milliseconds;
        while (true)
        {
            long current = Volatile.Read(ref _pauseUntilTick);
            if (current >= until)
                return;
            if (Interlocked.CompareExchange(ref _pauseUntilTick, until, current) == current)
                return;
        }
    }
}


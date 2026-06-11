using System.Runtime.InteropServices.JavaScript;

namespace e6502.Browser.Audio;

public static partial class BrowserAudio
{
    public const int EventStride = 5;

    public const int EventSid1Write = 1;
    public const int EventSid2Write = 2;
    public const int EventWtsNoteOn = 16;
    public const int EventWtsNoteOff = 17;
    public const int EventWtsVolume = 18;
    public const int EventWtsPanning = 19;
    public const int EventWtsPitchBend = 20;
    public const int EventWtsAllNotesOff = 21;
    public const int EventWtsMasterVolume = 22;
    public const int EventWtsResetEffects = 23;
    public const int EventReset = 31;

    private const int MaxQueuedEvents = 16384;
    private static readonly object _lock = new();
    private static readonly List<int> _events = new(MaxQueuedEvents * EventStride);
    private static int _cpuHz = 1_000_000;
    private static int _droppedEvents;
    private static volatile bool _enabled;

    public static void Configure(int cpuHz)
    {
        lock (_lock)
        {
            _cpuHz = Math.Clamp(cpuHz, 100_000, 12_000_000);
            _events.Clear();
            _droppedEvents = 0;
            _enabled = false;
        }
    }

    [JSExport]
    public static void SetEnabled(bool enabled)
    {
        lock (_lock)
        {
            _enabled = enabled;
            _events.Clear();
        }
    }

    [JSExport]
    public static int GetCpuHz()
    {
        lock (_lock)
            return _cpuHz;
    }

    [JSExport]
    public static int GetDroppedEventCount()
    {
        lock (_lock)
            return _droppedEvents;
    }

    [JSExport]
    public static int[] DrainEvents()
    {
        lock (_lock)
        {
            if (_events.Count == 0)
                return Array.Empty<int>();

            int[] result = _events.ToArray();
            _events.Clear();
            return result;
        }
    }

    public static void QueueReset(long cycle) =>
        Enqueue(cycle, EventReset, 0, 0);

    public static void QueueSidWrite(long cycle, int chipIndex, ushort baseAddress, ushort address, byte value)
    {
        int register = address - baseAddress;
        if ((uint)register >= 32)
            return;

        Enqueue(
            cycle,
            chipIndex == 0 ? EventSid1Write : EventSid2Write,
            register,
            value);
    }

    public static void QueueWtsEvent(long cycle, byte eventKind, byte voice, ushort value0, ushort value1)
    {
        int kind = eventKind switch
        {
            1 => EventWtsNoteOn,
            2 => EventWtsNoteOff,
            3 => EventWtsVolume,
            4 => EventWtsPanning,
            5 => EventWtsPitchBend,
            6 => EventWtsAllNotesOff,
            7 => EventWtsMasterVolume,
            8 => EventWtsResetEffects,
            _ => 0
        };
        if (kind == 0)
            return;

        int arg0 = voice;
        int arg1 = value0 | (value1 << 16);
        Enqueue(cycle, kind, arg0, arg1);
    }

    private static void Enqueue(long cycle, int kind, int arg0, int arg1)
    {
        if (!_enabled)
            return;

        lock (_lock)
        {
            if (!_enabled)
                return;

            if (_events.Count >= MaxQueuedEvents * EventStride)
            {
                _events.Clear();
                _droppedEvents++;
            }

            _events.Add((int)(cycle & 0xFFFFFFFF));
            _events.Add((int)((cycle >> 32) & 0xFFFFFFFF));
            _events.Add(kind);
            _events.Add(arg0);
            _events.Add(arg1);
        }
    }
}

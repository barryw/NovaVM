using System.Collections.Concurrent;
using KDS.e6502;

namespace e6502.Avalonia.Debugging;

public record BreakpointCondition(string Register, string Op, byte Value);

public sealed class DebuggerService
{
    private readonly Cpu _cpu;
    private readonly IBusDevice _bus;

    private readonly ConcurrentDictionary<ushort, List<BreakpointCondition?>> _breakpoints = new();
    private volatile bool _hasBreakpoints;

    private readonly ManualResetEventSlim _pauseGate = new(true); // initially running
    private volatile bool _paused;
    private volatile bool _stepping;
    private readonly ManualResetEventSlim _stepComplete = new(false);

    public bool IsPaused => _paused;

    public DebuggerService(Cpu cpu, IBusDevice bus)
    {
        _cpu = cpu;
        _bus = bus;
    }

    // Called once per CPU loop iteration — must be extremely fast when inactive
    public void CheckBreakpointAndWait()
    {
        if (!_paused && !_hasBreakpoints) return;

        if (!_paused && _hasBreakpoints)
        {
            var pc = _cpu.Pc;
            if (_breakpoints.TryGetValue(pc, out var conditions))
            {
                if (EvaluateConditions(conditions))
                {
                    _paused = true;
                    _pauseGate.Reset();
                }
            }
        }

        if (!_paused) return;

        _pauseGate.Wait();

        if (_stepping)
        {
            _paused = true;
            _pauseGate.Reset();
            _stepping = false;
            _stepComplete.Set();
        }
    }

    private bool EvaluateConditions(List<BreakpointCondition?> conditions)
    {
        if (conditions.Count == 0) return true;

        var state = _cpu.GetState();
        foreach (var cond in conditions)
        {
            if (cond is null) return true; // unconditional

            byte regVal = cond.Register.ToUpperInvariant() switch
            {
                "A" => state.A,
                "X" => state.X,
                "Y" => state.Y,
                "SP" => state.Sp,
                "PCL" => (byte)(state.Pc & 0xFF),
                "PCH" => (byte)(state.Pc >> 8),
                _ => 0
            };

            bool match = cond.Op switch
            {
                "==" or "=" => regVal == cond.Value,
                "!=" => regVal != cond.Value,
                "<" => regVal < cond.Value,
                ">" => regVal > cond.Value,
                "<=" => regVal <= cond.Value,
                ">=" => regVal >= cond.Value,
                _ => false
            };

            if (match) return true;
        }

        return false;
    }

    public void Pause()
    {
        _paused = true;
        _pauseGate.Reset();
    }

    public void Resume()
    {
        _paused = false;
        _stepping = false;
        _pauseGate.Set();
    }

    public CpuState Step()
    {
        if (!_paused) Pause();
        Thread.Sleep(1); // let CPU thread reach the gate

        _stepComplete.Reset();
        _stepping = true;
        _pauseGate.Set(); // let one instruction through

        _stepComplete.Wait(TimeSpan.FromSeconds(5));
        return _cpu.GetState();
    }

    public void AddBreakpoint(ushort address, BreakpointCondition? condition)
    {
        _breakpoints.AddOrUpdate(address,
            _ => new List<BreakpointCondition?> { condition },
            (_, list) => { list.Add(condition); return list; });
        _hasBreakpoints = true;
    }

    public bool RemoveBreakpoint(ushort address)
    {
        bool removed = _breakpoints.TryRemove(address, out _);
        _hasBreakpoints = !_breakpoints.IsEmpty;
        return removed;
    }

    public void ClearAllBreakpoints()
    {
        _breakpoints.Clear();
        _hasBreakpoints = false;
    }

    public Dictionary<ushort, List<BreakpointCondition?>> ListBreakpoints()
    {
        return new Dictionary<ushort, List<BreakpointCondition?>>(_breakpoints);
    }

    public byte[] ReadMemory(ushort address, int length)
    {
        if (length > 4096) length = 4096;
        var data = new byte[length];
        for (int i = 0; i < length; i++)
            data[i] = _bus.Read((ushort)(address + i));
        return data;
    }

    public byte[] ReadStack()
    {
        var state = _cpu.GetState();
        int sp = state.Sp;
        int count = 0xFF - sp;
        var data = new byte[count];
        for (int i = 0; i < count; i++)
            data[i] = _bus.Read((ushort)(0x0100 + sp + 1 + i));
        return data;
    }

    public List<(ushort Address, string Text, int Bytes)> DisassembleRange(ushort address, int count)
    {
        var result = new List<(ushort, string, int)>();
        ushort pc = address;
        for (int i = 0; i < count && pc <= 0xFFFF; i++)
        {
            var (text, bytes) = _cpu.Disassemble(pc);
            result.Add((pc, text, bytes));
            pc += (ushort)bytes;
        }
        return result;
    }
}

namespace e6502.Avalonia.Hardware;

public sealed class VirtualTimerController
{
    private readonly byte[] _regs = new byte[VgcConstants.TimerEnd - VgcConstants.TimerBase + 1];
    private int _counter;
    private int _cycleAccumulator;
    public bool IrqPending { get; private set; }
    public bool IsEnabled => (_regs[0] & 0x01) != 0;

    public bool OwnsAddress(ushort address) =>
        address >= VgcConstants.TimerBase && address <= VgcConstants.TimerEnd;

    public byte Read(ushort address)
    {
        if (address == VgcConstants.TimerStatus)
        {
            byte val = (byte)(IrqPending ? 0x01 : 0x00);
            IrqPending = false;
            return val;
        }
        return _regs[address - VgcConstants.TimerBase];
    }

    public void Write(ushort address, byte data)
    {
        _regs[address - VgcConstants.TimerBase] = data;
        if (address == VgcConstants.TimerCtrl && (data & 0x01) == 0)
        {
            _counter = 0;
            _cycleAccumulator = 0;
            IrqPending = false;
        }
    }

    public void AdvanceCycles(int cycles)
    {
        if (cycles <= 0) return;
        if ((_regs[0] & 0x01) == 0) return;

        int divisor = _regs[VgcConstants.TimerDivL - VgcConstants.TimerBase]
                    | (_regs[VgcConstants.TimerDivH - VgcConstants.TimerBase] << 8);
        if (divisor == 0) return;

        _cycleAccumulator += cycles;
        int ticks = _cycleAccumulator / VgcConstants.TimerTickQuantumCycles;
        if (ticks <= 0) return;
        _cycleAccumulator -= ticks * VgcConstants.TimerTickQuantumCycles;

        _counter += ticks;
        if (_counter >= divisor)
        {
            _counter %= divisor;
            IrqPending = true;
        }
    }

    public void Tick()
    {
        AdvanceCycles(VgcConstants.TimerTickQuantumCycles);
    }
}

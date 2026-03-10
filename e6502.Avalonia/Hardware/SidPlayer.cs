using KDS.e6502;

namespace e6502.Avalonia.Hardware;

public sealed class SidPlayer
{
    // Trap: when the play/init routine returns (RTS), PC lands here.
    // We write JMP $03D0 as a safety net, but detection is via PC check.
    private const ushort TrapAddress = 0x03D0;

    // SID files that overlap reserved regions are relocated to this RAM address.
    // $0800 is past the system area; demo code starts at $7200, giving ~27KB.
    private const ushort RelocBase = 0x0800;

    // Demo player and keyboard code live at $7200+; SIDs must not overlap.
    private const ushort DemoCodeBase = 0x7200;

    // Maximum cycles per init/play call before we declare it stuck.
    private const int MaxInitCycles  = 1_000_000;
    private const int MaxPlayCycles  = 50_000;

    // Auto-stop after this many consecutive play hangs (SID is incompatible).
    private const int MaxConsecutiveHangs = 5;

    // Protected RAM regions saved/restored around every virtual JSR call
    // so rogue SID routines can't corrupt the host program.
    private const ushort StackBase = 0x0100;
    private const int StackSize = 256;           // $0100-$01FF
    private const int ProtectedSize = 0xA000 - DemoCodeBase;  // $7200-$9FFF

    private readonly CompositeBusDevice _bus;
    private ushort _playAddress;
    private ushort _initAddress;
    private int _song;
    private bool _playing;
    private bool _needsInit;
    private bool _playPending;
    private int _elapsedFrames;
    private int _consecutiveHangs;
    private readonly byte[] _sidZeroPage = new byte[256];
    private readonly byte[] _savedStack = new byte[StackSize];
    private readonly byte[] _savedProtected = new byte[ProtectedSize];

    public bool IsPlaying => _playing;
    public int ElapsedFrames => _elapsedFrames;
    public void IncrementElapsed() => _elapsedFrames++;
    public bool HasPendingCall => _playing && (_needsInit || _playPending);

    /// <summary>Mark that a frame tick occurred; queues the next play call.</summary>
    public void SetPlayPending()
    {
        if (_playing && !_needsInit) _playPending = true;
    }

    public SidPlayer(CompositeBusDevice bus) => _bus = bus;

    public void Play(SidFileInfo info, int song = 1)
    {
        if (!info.IsValid) { Console.Error.WriteLine("[SID] Play: invalid SidFileInfo"); return; }
        _elapsedFrames = 0;

        // Relocate if the SID overlaps demo player ($7200+) or hardware ($A000+) space.
        bool overlapsDemo = info.LoadAddress >= DemoCodeBase ||
                            info.LoadAddress + info.Payload.Length > DemoCodeBase;
        bool overlapsHardware = info.LoadAddress >= VgcConstants.VgcBase;

        if (overlapsDemo || overlapsHardware)
        {
            Console.Error.WriteLine($"[SID] Relocating ${info.LoadAddress:X4} -> ${RelocBase:X4} ({info.Payload.Length} bytes)");
            info = SidRelocator.Relocate(info, RelocBase);
            if (!info.IsValid) { Console.Error.WriteLine("[SID] Relocation failed"); return; }
            if (info.LoadAddress + info.Payload.Length > DemoCodeBase)
            {
                Console.Error.WriteLine($"[SID] Payload too large after relocation: ${info.LoadAddress:X4}+{info.Payload.Length} > ${DemoCodeBase:X4}");
                return;
            }
        }

        // Write payload directly to RAM (bypasses hardware register interception)
        for (int i = 0; i < info.Payload.Length; i++)
            _bus.WriteRam((ushort)(info.LoadAddress + i), info.Payload[i]);

        // Write trap at $03D0: JMP $03D0 (infinite loop as safety net)
        _bus.WriteRam(TrapAddress, 0x4C);
        _bus.WriteRam((ushort)(TrapAddress + 1), (byte)(TrapAddress & 0xFF));
        _bus.WriteRam((ushort)(TrapAddress + 2), (byte)(TrapAddress >> 8));

        _playAddress = info.PlayAddress;
        _initAddress = info.InitAddress;
        _song = song;
        _needsInit = true;
        _playPending = false;
        _consecutiveHangs = 0;
        Array.Clear(_sidZeroPage);
        _playing = true;

        Console.Error.WriteLine($"[SID] Playing: load=${info.LoadAddress:X4} init=${info.InitAddress:X4} play=${info.PlayAddress:X4} song={song}");
    }

    public void Stop()
    {
        _playing = false;
        _needsInit = false;
        _playPending = false;

        // Gate off all SID voices and silence volume (both chips)
        _bus.Write(0xD404, 0x00);
        _bus.Write(0xD40B, 0x00);
        _bus.Write(0xD412, 0x00);
        _bus.Write(0xD418, 0x00);
        _bus.Write(0xD424, 0x00);
        _bus.Write(0xD42B, 0x00);
        _bus.Write(0xD432, 0x00);
        _bus.Write(0xD438, 0x00);

        // Flush any buffered audio so silence is immediate
        _bus.Sid.Flush();
        _bus.Sid2.Flush();
    }

    /// <summary>
    /// Execute pending init/play calls via virtual JSR.
    /// Saves and restores CPU state and zero page so the SID routines
    /// cannot corrupt the host program's state.
    /// </summary>
    public void ExecutePendingCalls(Cpu cpu)
    {
        if (_needsInit)
        {
            VirtualJsr(cpu, _initAddress, MaxInitCycles, argA: (byte)(_song - 1));
            _needsInit = false;
            if (_playAddress == 0)
                Console.Error.WriteLine("[SID] PlayAddress=0: init-only SID (no virtual play calls)");
        }
        if (_playPending)
        {
            _playPending = false;
            if (_playAddress != 0)
            {
                bool hung = VirtualJsr(cpu, _playAddress, MaxPlayCycles);
                if (hung)
                {
                    _consecutiveHangs++;
                    if (_consecutiveHangs >= MaxConsecutiveHangs)
                    {
                        Console.Error.WriteLine($"[SID] Auto-stopping: play routine hung {MaxConsecutiveHangs} times in a row");
                        Stop();
                    }
                }
                else
                {
                    _consecutiveHangs = 0;
                }
            }
        }
    }

    /// <returns>true if the routine exceeded the cycle limit (hung)</returns>
    private bool VirtualJsr(Cpu cpu, ushort address, int maxCycles, byte? argA = null)
    {
        // Save host state: CPU, zero page, stack, and demo player region
        var savedState = cpu.GetState();
        var savedZp = new byte[256];
        for (int i = 0; i < 256; i++)
            savedZp[i] = _bus.ReadRam((ushort)i);
        for (int i = 0; i < StackSize; i++)
            _savedStack[i] = _bus.ReadRam((ushort)(StackBase + i));
        for (int i = 0; i < ProtectedSize; i++)
            _savedProtected[i] = _bus.ReadRam((ushort)(DemoCodeBase + i));

        // Restore SID's zero page
        for (int i = 0; i < 256; i++)
            _bus.WriteRam((ushort)i, _sidZeroPage[i]);

        // Set up CPU: SP at top of stack, push trap return address, set PC
        // RTS pops (lo, hi) and adds 1, so push (TrapAddress - 1)
        ushort retAddr = (ushort)(TrapAddress - 1);
        _bus.WriteRam(0x01FF, (byte)(retAddr >> 8));
        _bus.WriteRam(0x01FE, (byte)(retAddr & 0xFF));
        cpu.RestoreState(new CpuState(
            A: argA ?? 0, X: 0, Y: 0, Sp: 0xFD, Pc: address,
            Nf: false, Vf: false, Df: false, If: true, Zf: false, Cf: false));

        // Execute until PC hits trap or cycle limit
        int cycles = 0;
        while (cycles < maxCycles)
        {
            int c = cpu.ClocksForNext();
            cpu.ExecuteNext();
            cycles += c;
            if (cpu.Pc == TrapAddress) break;
        }

        bool hung = cycles >= maxCycles;
        if (hung)
            Console.Error.WriteLine($"[SID] Virtual JSR to ${address:X4} exceeded {maxCycles} cycles — possible hang");

        // Save SID's zero page state for next call
        for (int i = 0; i < 256; i++)
            _sidZeroPage[i] = _bus.ReadRam((ushort)i);

        // Restore host state: zero page, stack, demo player region, CPU
        for (int i = 0; i < 256; i++)
            _bus.WriteRam((ushort)i, savedZp[i]);
        for (int i = 0; i < StackSize; i++)
            _bus.WriteRam((ushort)(StackBase + i), _savedStack[i]);
        for (int i = 0; i < ProtectedSize; i++)
            _bus.WriteRam((ushort)(DemoCodeBase + i), _savedProtected[i]);
        cpu.RestoreState(savedState);
        return hung;
    }
}

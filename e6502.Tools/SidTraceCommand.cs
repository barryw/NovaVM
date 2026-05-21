using e6502.Avalonia.Hardware;
using KDS.e6502;

namespace e6502.Tools;

public static class SidTraceCommand
{
    private const ushort TrapAddress = 0x03D0;
    private const ushort DemoCodeBase = 0x7200;
    private const int ProtectedSize = 0xA000 - DemoCodeBase;

    private const ushort SidWorkspaceMin = 0x0400;
    private const ushort SidWorkspaceTop = 0xA000;
    private const ushort SidWorkspaceBytes = 0x0300;
    private const ushort SidIrqEntryOffset = 0x0010;
    private const ushort SidHostZpOffset = 0x0100;
    private const ushort SidPlayerZpOffset = 0x0200;

    private const ushort VgcIrqEnable = 0xA0F0;
    private const ushort VgcIrqStatus = 0xA0F1;
    private const byte VgcIrqTimer = 0x80;
    private const byte VgcIrqNonSidMask = 0x7F;
    private const byte IrqBasicIrqBaseZp = 0xDF;

    public static int Run(string[] args)
    {
        if (args.Length < 1)
        {
            PrintUsage();
            return 1;
        }

        string path = args[0];
        int frames = 8;
        int song = 1;
        string? dumpCsvPath = null;
        string? dumpRtlPath = null;

        for (int i = 1; i < args.Length; i++)
        {
            switch (args[i])
            {
                case "--frames" when i + 1 < args.Length:
                    frames = int.Parse(args[++i]);
                    break;
                case "--song" when i + 1 < args.Length:
                    song = int.Parse(args[++i]);
                    break;
                case "--dump-csv" when i + 1 < args.Length:
                    dumpCsvPath = args[++i];
                    break;
                case "--dump-rtl" when i + 1 < args.Length:
                    dumpRtlPath = args[++i];
                    break;
                case "-h" or "--help":
                    PrintUsage();
                    return 0;
                default:
                    Console.Error.WriteLine($"Unknown or incomplete option: {args[i]}");
                    PrintUsage();
                    return 1;
            }
        }

        if (!File.Exists(path))
        {
            Console.Error.WriteLine($"SID file not found: {path}");
            return 1;
        }

        var raw = File.ReadAllBytes(path);
        var info = SidFileParser.Parse(raw);
        if (!info.IsValid)
        {
            Console.Error.WriteLine($"Invalid SID file: {path}");
            return 1;
        }

        if (song < 1)
            song = info.StartSong == 0 ? 1 : info.StartSong;

        Console.WriteLine(
            $"SID trace: {Path.GetFileName(path)} title=\"{info.Title}\" load=${info.LoadAddress:X4} " +
            $"init=${info.InitAddress:X4} play=${info.PlayAddress:X4} song={song} frames={frames}");

        var avalonia = TraceAvalonia(info, frames, song);
        var hardware = TraceHardwareIrq(info, frames, song);

        Console.WriteLine($"Avalonia writes: {avalonia.Writes.Count}");
        Console.WriteLine($"IRQ-player writes: {hardware.Writes.Count}");
        if (dumpCsvPath is not null)
            WriteCsvDump(dumpCsvPath, avalonia, hardware);
        if (dumpRtlPath is not null)
            WriteRtlDump(dumpRtlPath, hardware);

        int mismatch = FirstMismatch(avalonia.Writes, hardware.Writes);
        if (mismatch < 0 && avalonia.Writes.Count == hardware.Writes.Count)
        {
            Console.WriteLine("SID register streams match.");
            PrintVoiceSummary(avalonia, hardware);
            return 0;
        }

        Console.WriteLine($"SID register streams diverge at write #{mismatch + 1}.");
        PrintMismatchWindow(avalonia.Writes, hardware.Writes, Math.Max(0, mismatch - 5), 12);
        PrintVoiceSummary(avalonia, hardware);
        return 2;
    }

    private static void PrintUsage() =>
        Console.Error.WriteLine("Usage: sidtrace <file.sid> [--frames N] [--song N] [--dump-csv PATH] [--dump-rtl PATH]");

    private static void WriteCsvDump(string path, TraceResult avalonia, TraceResult hardware)
    {
        EnsureParentDirectory(path);
        using var writer = new StreamWriter(path);
        writer.WriteLine("source,ordinal,frame,phase,address,normalized_address,data");
        WriteCsvRows(writer, avalonia.Name, avalonia.Writes);
        WriteCsvRows(writer, hardware.Name, hardware.Writes);
        Console.WriteLine($"Wrote CSV SID trace: {path}");
    }

    private static void WriteCsvRows(StreamWriter writer, string source, IReadOnlyList<SidWrite> writes)
    {
        foreach (var write in writes)
        {
            writer.Write(source);
            writer.Write(',');
            writer.Write(write.Ordinal);
            writer.Write(',');
            writer.Write(write.Frame);
            writer.Write(',');
            writer.Write(write.Phase);
            writer.Write(',');
            writer.Write($"0x{write.Address:X4}");
            writer.Write(',');
            writer.Write($"0x{write.NormalizedAddress:X4}");
            writer.Write(',');
            writer.WriteLine($"0x{write.Data:X2}");
        }
    }

    private static void WriteRtlDump(string path, TraceResult hardware)
    {
        EnsureParentDirectory(path);
        using var writer = new StreamWriter(path);
        foreach (var write in hardware.Writes)
        {
            if (!TryMapSidRegister(write.NormalizedAddress, out int chip, out int register))
                continue;

            writer.Write(write.Frame);
            writer.Write(' ');
            writer.Write(chip);
            writer.Write(' ');
            writer.Write($"{register:X2}");
            writer.Write(' ');
            writer.WriteLine($"{write.Data:X2}");
        }
        Console.WriteLine($"Wrote RTL SID replay trace: {path}");
    }

    private static bool TryMapSidRegister(ushort address, out int chip, out int register)
    {
        if (address >= 0xD400 && address <= 0xD41F)
        {
            chip = 1;
            register = address - 0xD400;
            return true;
        }
        if (address >= 0xD420 && address <= 0xD43F)
        {
            chip = 2;
            register = address - 0xD420;
            return true;
        }

        chip = 0;
        register = 0;
        return false;
    }

    private static void EnsureParentDirectory(string path)
    {
        string? directory = Path.GetDirectoryName(Path.GetFullPath(path));
        if (!string.IsNullOrEmpty(directory))
            Directory.CreateDirectory(directory);
    }

    private static TraceResult TraceAvalonia(SidFileInfo info, int frames, int song)
    {
        var bus = new TraceBus("avalonia");
        LoadSidPayload(bus, info);
        InstallTrap(bus);

        var cpu = new Cpu(bus);
        byte[] sidZeroPage = new byte[256];
        RunVirtualJsr(cpu, bus, info.InitAddress, 1_000_000, (byte)(song - 1), -1, "init", sidZeroPage);

        for (int frame = 0; frame < frames; frame++)
            RunVirtualJsr(cpu, bus, info.PlayAddress, 50_000, null, frame, "play", sidZeroPage);

        return bus.ToResult();
    }

    private static TraceResult TraceHardwareIrq(SidFileInfo info, int frames, int song)
    {
        var bus = new TraceBus("irq");
        LoadSidPayload(bus, info);

        ushort workspace = ChooseSidWorkspace(info.LoadAddress, info.Payload.Length);
        if (workspace == 0)
            throw new InvalidOperationException("No free SID IRQ-player workspace.");

        byte[] page = BuildSidIrqCode(workspace, info.InitAddress, info.PlayAddress, (byte)(song - 1));
        for (int i = 0; i < page.Length; i++)
            bus.DirectWrite((ushort)(workspace + i), page[i]);

        for (int i = 0; i < 256; i++)
        {
            bus.DirectWrite((ushort)(workspace + SidHostZpOffset + i), 0);
            bus.DirectWrite((ushort)(workspace + SidPlayerZpOffset + i), 0);
        }

        var cpu = new Cpu(bus);
        ushort entry = (ushort)(workspace + SidIrqEntryOffset);
        for (int frame = 0; frame < frames; frame++)
        {
            bus.BeginFrame(frame, "irq");
            bus.IrqEnable |= VgcIrqTimer;
            bus.IrqStatus |= VgcIrqTimer;

            // State after a 6502 IRQ with host SP=$FD: PC/P are already stacked.
            bus.DirectWrite(0x01FB, 0x24);
            bus.DirectWrite(0x01FC, 0x34);
            bus.DirectWrite(0x01FD, 0x12);
            cpu.RestoreState(new CpuState(
                A: 0x55, X: 0x66, Y: 0x77, Sp: 0xFA, Pc: entry,
                Nf: false, Vf: false, Df: false, If: true, Zf: false, Cf: false));

            int cycles = 0;
            while (cycles < 2_000_000 && cpu.Pc != 0x1234)
            {
                int c = cpu.ClocksForNext();
                cpu.ExecuteNext();
                cycles += c;
            }

            if (cpu.Pc != 0x1234)
                throw new InvalidOperationException($"IRQ player did not RTI on frame {frame}; PC=${cpu.Pc:X4}");
        }

        return bus.ToResult();
    }

    private static void RunVirtualJsr(Cpu cpu, TraceBus bus, ushort address, int maxCycles,
                                      byte? argA, int frame, string phase,
                                      byte[] sidZeroPage)
    {
        bus.BeginFrame(frame, phase);

        var savedState = cpu.GetState();
        byte[] savedZp = new byte[256];
        byte[] savedStack = new byte[256];
        byte[] savedProtected = new byte[ProtectedSize];

        for (int i = 0; i < 256; i++)
        {
            savedZp[i] = bus.DirectRead((ushort)i);
            savedStack[i] = bus.DirectRead((ushort)(0x0100 + i));
            bus.DirectWrite((ushort)i, sidZeroPage[i]);
        }
        for (int i = 0; i < ProtectedSize; i++)
            savedProtected[i] = bus.DirectRead((ushort)(DemoCodeBase + i));

        ushort retAddr = TrapAddress - 1;
        bus.DirectWrite(0x01FF, (byte)(retAddr >> 8));
        bus.DirectWrite(0x01FE, (byte)(retAddr & 0xFF));
        cpu.RestoreState(new CpuState(
            A: argA ?? 0, X: 0, Y: 0, Sp: 0xFD, Pc: address,
            Nf: false, Vf: false, Df: false, If: true, Zf: false, Cf: false));

        int cycles = 0;
        while (cycles < maxCycles)
        {
            int c = cpu.ClocksForNext();
            cpu.ExecuteNext();
            cycles += c;
            if (cpu.Pc == TrapAddress)
                break;
        }

        if (cpu.Pc != TrapAddress)
            throw new InvalidOperationException($"Virtual JSR to ${address:X4} exceeded {maxCycles} cycles.");

        for (int i = 0; i < 256; i++)
            sidZeroPage[i] = bus.DirectRead((ushort)i);

        for (int i = 0; i < 256; i++)
        {
            bus.DirectWrite((ushort)i, savedZp[i]);
            bus.DirectWrite((ushort)(0x0100 + i), savedStack[i]);
        }
        for (int i = 0; i < ProtectedSize; i++)
            bus.DirectWrite((ushort)(DemoCodeBase + i), savedProtected[i]);

        cpu.RestoreState(savedState);
    }

    private static void LoadSidPayload(TraceBus bus, SidFileInfo info)
    {
        uint loadEnd = (uint)info.LoadAddress + (uint)info.Payload.Length;
        if (info.Payload.Length == 0 ||
            info.LoadAddress < SidWorkspaceMin ||
            loadEnd > SidWorkspaceTop ||
            info.InitAddress < info.LoadAddress ||
            info.InitAddress >= loadEnd ||
            info.PlayAddress == 0 ||
            info.PlayAddress < info.LoadAddress ||
            info.PlayAddress >= loadEnd)
        {
            throw new InvalidOperationException(
                $"Unsupported SID layout: load=${info.LoadAddress:X4} size={info.Payload.Length} " +
                $"init=${info.InitAddress:X4} play=${info.PlayAddress:X4}");
        }

        for (int i = 0; i < info.Payload.Length; i++)
            bus.DirectWrite((ushort)(info.LoadAddress + i), info.Payload[i]);
    }

    private static void InstallTrap(TraceBus bus)
    {
        bus.DirectWrite(TrapAddress, 0x4C);
        bus.DirectWrite((ushort)(TrapAddress + 1), (byte)(TrapAddress & 0xFF));
        bus.DirectWrite((ushort)(TrapAddress + 2), (byte)(TrapAddress >> 8));
    }

    private static byte[] BuildSidIrqCode(ushort workspace, ushort initAddr,
                                          ushort playAddr, byte songArg)
    {
        byte[] page = new byte[256];
        ushort hostZp = (ushort)(workspace + SidHostZpOffset);
        ushort sidZp = (ushort)(workspace + SidPlayerZpOffset);
        ushort activeFlag = workspace;
        ushort initFlag = (ushort)(workspace + 1);
        ushort songValue = (ushort)(workspace + 2);
        ushort irqStatusSave = (ushort)(workspace + 3);
        ushort nonSidStatus = (ushort)(workspace + 4);
        ushort frameCounter = (ushort)(workspace + 5);
        ushort playCounter = (ushort)(workspace + 7);
        ushort initCounter = (ushort)(workspace + 9);
        ushort irqEnableSave = (ushort)(workspace + 11);

        page[0] = 1;
        page[1] = 1;
        page[2] = songArg;

        int n = SidIrqEntryOffset;
        void Emit(byte value) => page[n++] = value;
        void Emit16(ushort value)
        {
            Emit((byte)(value & 0xFF));
            Emit((byte)(value >> 8));
        }
        void LdaAbs(ushort addr) { Emit(0xAD); Emit16(addr); }
        void StaAbs(ushort addr) { Emit(0x8D); Emit16(addr); }
        void LdaAbsX(ushort addr) { Emit(0xBD); Emit16(addr); }
        void StaAbsX(ushort addr) { Emit(0x9D); Emit16(addr); }
        void IncAbs(ushort addr) { Emit(0xEE); Emit16(addr); }
        int BeqPlaceholder() { Emit(0xF0); int op = n; Emit(0); return op; }
        int BnePlaceholder() { Emit(0xD0); int op = n; Emit(0); return op; }
        int JmpAbsPlaceholder() { Emit(0x4C); int op = n; Emit(0); Emit(0); return op; }
        void PatchBranch(int operand, int target)
        {
            int rel = target - (operand + 1);
            if (rel < -128 || rel > 127)
                throw new InvalidOperationException("SID IRQ branch target out of range.");
            page[operand] = unchecked((byte)(sbyte)rel);
        }
        void PatchJmp(int operand, int target)
        {
            ushort addr = (ushort)(workspace + target);
            page[operand] = (byte)(addr & 0xFF);
            page[operand + 1] = (byte)(addr >> 8);
        }
        void IncAbs16(ushort addr)
        {
            IncAbs(addr);
            int noCarryBne = BnePlaceholder();
            IncAbs((ushort)(addr + 1));
            PatchBranch(noCarryBne, n);
        }

        Emit(0x48);
        Emit(0x8A); Emit(0x48);
        Emit(0x98); Emit(0x48);
        Emit(0xD8);

        LdaAbs(VgcIrqEnable);
        StaAbs(irqEnableSave);
        LdaAbs(VgcIrqStatus);
        StaAbs(irqStatusSave);
        Emit(0x29); Emit(VgcIrqTimer);
        int hasTimerBne = BnePlaceholder();
        int noTimerJmp = JmpAbsPlaceholder();
        PatchBranch(hasTimerBne, n);

        LdaAbs(irqEnableSave);
        Emit(0x29); Emit(unchecked((byte)~VgcIrqTimer));
        StaAbs(VgcIrqEnable);
        Emit(0xA9); Emit(VgcIrqTimer);
        StaAbs(VgcIrqStatus);
        IncAbs16(frameCounter);
        LdaAbs(activeFlag);
        int activeBne = BnePlaceholder();
        int inactiveJmp = JmpAbsPlaceholder();
        PatchBranch(activeBne, n);

        LdaAbs(initFlag);
        int initPendingBne = BnePlaceholder();
        PatchBranch(initPendingBne, n);

        Emit(0xA2); Emit(0x00);
        int saveHostLoop = n;
        Emit(0xB5); Emit(0x00);
        StaAbsX(hostZp);
        Emit(0xE8);
        int saveHostBne = BnePlaceholder();

        Emit(0xA2); Emit(0x00);
        int restoreSidLoop = n;
        LdaAbsX(sidZp);
        Emit(0x95); Emit(0x00);
        Emit(0xE8);
        int restoreSidBne = BnePlaceholder();

        LdaAbs(initFlag);
        int skipInitBeq = BeqPlaceholder();
        IncAbs(initCounter);
        Emit(0xA2); Emit(0x00);
        Emit(0xA0); Emit(0x00);
        LdaAbs(songValue);
        Emit(0x20); Emit16(initAddr);
        Emit(0xA9); Emit(0x00);
        StaAbs(initFlag);

        int callPlay = n;
        IncAbs16(playCounter);
        Emit(0xA2); Emit(0x00);
        Emit(0xA0); Emit(0x00);
        Emit(0x20); Emit16(playAddr);

        Emit(0xA2); Emit(0x00);
        int saveSidLoop = n;
        Emit(0xB5); Emit(0x00);
        StaAbsX(sidZp);
        Emit(0xE8);
        int saveSidBne = BnePlaceholder();

        Emit(0xA2); Emit(0x00);
        int restoreHostLoop = n;
        LdaAbsX(hostZp);
        Emit(0x95); Emit(0x00);
        Emit(0xE8);
        int restoreHostBne = BnePlaceholder();

        int maybeBasicIrq = n;
        LdaAbs(irqStatusSave);
        Emit(0x29); Emit(VgcIrqNonSidMask);
        int noBasicIrqBeq = BeqPlaceholder();
        StaAbs(nonSidStatus);
        Emit(0xA5); Emit(IrqBasicIrqBaseZp);
        Emit(0x4A);
        Emit(0x05); Emit(IrqBasicIrqBaseZp);
        Emit(0x85); Emit(IrqBasicIrqBaseZp);
        LdaAbs(nonSidStatus);
        StaAbs(VgcIrqStatus);

        int done = n;
        LdaAbs(irqEnableSave);
        StaAbs(VgcIrqEnable);
        Emit(0x68); Emit(0xA8);
        Emit(0x68); Emit(0xAA);
        Emit(0x68);
        Emit(0x40);

        PatchJmp(noTimerJmp, maybeBasicIrq);
        PatchJmp(inactiveJmp, maybeBasicIrq);
        PatchBranch(saveHostBne, saveHostLoop);
        PatchBranch(restoreSidBne, restoreSidLoop);
        PatchBranch(skipInitBeq, callPlay);
        PatchBranch(saveSidBne, saveSidLoop);
        PatchBranch(restoreHostBne, restoreHostLoop);
        PatchBranch(noBasicIrqBeq, done);

        if (n > 256)
            throw new InvalidOperationException("SID IRQ handler overflowed its 256-byte page.");

        return page;
    }

    private static ushort ChooseSidWorkspace(ushort load, int payloadLength)
    {
        for (int baseAddr = SidWorkspaceTop - SidWorkspaceBytes;
             baseAddr >= SidWorkspaceMin;
             baseAddr -= 0x0100)
        {
            if (!RangesOverlap((uint)baseAddr, SidWorkspaceBytes, load, (uint)payloadLength))
                return (ushort)baseAddr;
        }
        return 0;
    }

    private static bool RangesOverlap(uint a, uint aLen, uint b, uint bLen) =>
        aLen != 0 && bLen != 0 && a < b + bLen && b < a + aLen;

    private static int FirstMismatch(IReadOnlyList<SidWrite> left,
                                     IReadOnlyList<SidWrite> right)
    {
        int count = Math.Min(left.Count, right.Count);
        for (int i = 0; i < count; i++)
        {
            if (left[i].NormalizedAddress != right[i].NormalizedAddress ||
                left[i].Data != right[i].Data)
            {
                return i;
            }
        }
        return left.Count == right.Count ? -1 : count;
    }

    private static void PrintMismatchWindow(IReadOnlyList<SidWrite> avalonia,
                                            IReadOnlyList<SidWrite> hardware,
                                            int start, int count)
    {
        int end = Math.Min(Math.Max(avalonia.Count, hardware.Count), start + count);
        for (int i = start; i < end; i++)
        {
            string a = i < avalonia.Count ? avalonia[i].ToString() : "<none>";
            string h = i < hardware.Count ? hardware[i].ToString() : "<none>";
            Console.WriteLine($"#{i + 1:0000}  avalonia {a,-34}  irq {h}");
        }
    }

    private static void PrintVoiceSummary(TraceResult avalonia, TraceResult hardware)
    {
        Console.WriteLine("Final SID1 voice summary:");
        for (int voice = 0; voice < 3; voice++)
        {
            ushort baseAddr = (ushort)(0xD400 + voice * 7);
            Console.WriteLine(
                $"  v{voice + 1}: avalonia f=${Read16(avalonia, baseAddr):X4} c=${Read(avalonia, (ushort)(baseAddr + 4)):X2} " +
                $"ad=${Read(avalonia, (ushort)(baseAddr + 5)):X2} sr=${Read(avalonia, (ushort)(baseAddr + 6)):X2} | " +
                $"irq f=${Read16(hardware, baseAddr):X4} c=${Read(hardware, (ushort)(baseAddr + 4)):X2} " +
                $"ad=${Read(hardware, (ushort)(baseAddr + 5)):X2} sr=${Read(hardware, (ushort)(baseAddr + 6)):X2}");
        }
        Console.WriteLine(
            $"  filter/vol: avalonia fc=${Read(avalonia, 0xD415):X2}${Read(avalonia, 0xD416):X2} " +
            $"res=${Read(avalonia, 0xD417):X2} vol=${Read(avalonia, 0xD418):X2} | " +
            $"irq fc=${Read(hardware, 0xD415):X2}${Read(hardware, 0xD416):X2} " +
            $"res=${Read(hardware, 0xD417):X2} vol=${Read(hardware, 0xD418):X2}");
    }

    private static ushort Read16(TraceResult trace, ushort address) =>
        (ushort)(Read(trace, address) | (Read(trace, (ushort)(address + 1)) << 8));

    private static byte Read(TraceResult trace, ushort address) =>
        trace.SidShadow.TryGetValue(address, out byte value) ? value : (byte)0;

    private sealed class TraceBus(string name) : IBusDevice
    {
        private readonly byte[] _ram = new byte[65536];
        private readonly Dictionary<ushort, byte> _sidShadow = [];
        private int _ordinal;
        private int _frame;
        private string _phase = "";

        public byte IrqEnable { get; set; }
        public byte IrqStatus { get; set; }
        public List<SidWrite> Writes { get; } = [];

        public void BeginFrame(int frame, string phase)
        {
            _frame = frame;
            _phase = phase;
        }

        public byte Read(ushort address)
        {
            if (address == VgcIrqEnable)
                return IrqEnable;
            if (address == VgcIrqStatus)
                return IrqStatus;
            return _ram[address];
        }

        public void Write(ushort address, byte data)
        {
            if (address == VgcIrqEnable)
            {
                IrqEnable = data;
                return;
            }
            if (address == VgcIrqStatus)
            {
                IrqStatus = (byte)(IrqStatus & ~data);
                return;
            }
            if (TryNormalizeSidAddress(address, out ushort normalized))
            {
                _sidShadow[normalized] = data;
                Writes.Add(new SidWrite(_ordinal++, _frame, _phase, address, normalized, data));
                return;
            }
            _ram[address] = data;
        }

        public byte DirectRead(ushort address) => _ram[address];
        public void DirectWrite(ushort address, byte data) => _ram[address] = data;

        public TraceResult ToResult() => new(name, Writes, new Dictionary<ushort, byte>(_sidShadow));

        private static bool TryNormalizeSidAddress(ushort address, out ushort normalized)
        {
            if (address >= 0xD400 && address <= 0xD43F)
            {
                normalized = address;
                return true;
            }
            if (address >= 0xD500 && address <= 0xD51F)
            {
                normalized = (ushort)(0xD420 + (address - 0xD500));
                return true;
            }
            if (address == 0xD440)
            {
                normalized = address;
                return true;
            }
            normalized = 0;
            return false;
        }
    }

    private sealed record TraceResult(
        string Name,
        IReadOnlyList<SidWrite> Writes,
        IReadOnlyDictionary<ushort, byte> SidShadow);

    private sealed record SidWrite(
        int Ordinal,
        int Frame,
        string Phase,
        ushort Address,
        ushort NormalizedAddress,
        byte Data)
    {
        public override string ToString() =>
            $"[{Frame,3}:{Phase,-4}] ${Address:X4}->${NormalizedAddress:X4}=${Data:X2}";
    }
}

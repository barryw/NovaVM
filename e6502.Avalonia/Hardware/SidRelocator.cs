using KDS.e6502.OpCodes;

namespace e6502.Avalonia.Hardware;

public static class SidRelocator
{
    /// <summary>
    /// Relocate a SID file payload to a new base address, patching all absolute
    /// addresses in discovered code paths. Returns a new SidFileInfo with updated
    /// loadAddress, initAddress, playAddress and patched payload.
    /// </summary>
    public static SidFileInfo Relocate(SidFileInfo info, ushort targetAddr)
    {
        if (!info.IsValid) return SidFileInfo.Invalid;

        int delta = targetAddr - info.LoadAddress;
        if (delta == 0) return info;

        ushort origBase = info.LoadAddress;
        ushort origEnd = (ushort)(origBase + info.Payload.Length);

        // Discover code bytes by control-flow following
        var codeOffsets = new HashSet<int>();
        var opTable = new OpCodeTable();
        DiscoverCode(info.Payload, origBase, info.InitAddress, opTable, codeOffsets);
        if (info.PlayAddress != 0 && info.PlayAddress != info.InitAddress)
            DiscoverCode(info.Payload, origBase, info.PlayAddress, opTable, codeOffsets);

        // Patch absolute addresses in discovered code
        byte[] patched = (byte[])info.Payload.Clone();
        foreach (int offset in codeOffsets)
        {
            if (offset >= patched.Length) continue;
            var op = opTable.OpCodes[patched[offset]];
            if (!op.IsValid || op.Bytes < 3) continue;

            if (op.AddressMode is not (AddressModes.Absolute or AddressModes.AbsoluteX or AddressModes.AbsoluteY))
                continue;

            int operandOffset = offset + 1;
            if (operandOffset + 1 >= patched.Length) continue;

            ushort addr = (ushort)(patched[operandOffset] | (patched[operandOffset + 1] << 8));

            // Only patch addresses within the player's code range
            if (addr >= origBase && addr < origEnd)
            {
                ushort newAddr = (ushort)(addr + delta);
                patched[operandOffset] = (byte)(newAddr & 0xFF);
                patched[operandOffset + 1] = (byte)(newAddr >> 8);
            }
        }

        return new SidFileInfo
        {
            IsValid = true,
            Magic = info.Magic,
            Version = info.Version,
            LoadAddress = targetAddr,
            InitAddress = (ushort)(info.InitAddress + delta),
            PlayAddress = info.PlayAddress == 0 ? (ushort)0 : (ushort)(info.PlayAddress + delta),
            Songs = info.Songs,
            StartSong = info.StartSong,
            Speed = info.Speed,
            UsesCiaTiming = info.UsesCiaTiming,
            Title = info.Title,
            Author = info.Author,
            Copyright = info.Copyright,
            Payload = patched
        };
    }

    private static void DiscoverCode(byte[] payload, ushort loadBase, ushort entryPoint,
                                      OpCodeTable opTable, HashSet<int> codeOffsets)
    {
        var queue = new Queue<ushort>();
        queue.Enqueue(entryPoint);

        while (queue.Count > 0)
        {
            ushort pc = queue.Dequeue();

            while (true)
            {
                int offset = pc - loadBase;
                if (offset < 0 || offset >= payload.Length) break;
                if (!codeOffsets.Add(offset)) break; // already visited

                var op = opTable.OpCodes[payload[offset]];
                if (!op.IsValid) break;

                string mnemonic = op.Instruction;

                // Follow branch targets (relative addressing)
                if (op.AddressMode == AddressModes.Relative && op.Bytes == 2 && offset + 1 < payload.Length)
                {
                    sbyte rel = (sbyte)payload[offset + 1];
                    ushort branchTarget = (ushort)(pc + 2 + rel);
                    queue.Enqueue(branchTarget);
                }

                // JSR absolute - follow target, then continue (fall through after return)
                if (mnemonic == "JSR" && op.AddressMode == AddressModes.Absolute && op.Bytes == 3 && offset + 2 < payload.Length)
                {
                    ushort target = (ushort)(payload[offset + 1] | (payload[offset + 2] << 8));
                    queue.Enqueue(target);
                }

                // JMP absolute - follow target, no fall-through
                if (mnemonic == "JMP" && op.AddressMode == AddressModes.Absolute && op.Bytes == 3 && offset + 2 < payload.Length)
                {
                    ushort target = (ushort)(payload[offset + 1] | (payload[offset + 2] << 8));
                    queue.Enqueue(target);
                    break;
                }

                // JMP indirect - can't follow statically
                if (mnemonic == "JMP" && op.AddressMode == AddressModes.Indirect)
                    break;

                // RTS/RTI/BRK — end of this path
                if (mnemonic is "RTS" or "RTI" or "BRK")
                    break;

                // Advance to next instruction
                pc += op.Bytes;
            }
        }
    }
}

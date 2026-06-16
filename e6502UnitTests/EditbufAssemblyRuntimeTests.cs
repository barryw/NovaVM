using System;
using System.IO;
using KDS.e6502;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class EditbufAssemblyRuntimeTests
{
    private const ushort GotoEntry = 0x4000;
    private const ushort CutEntry = 0x4003;
    private const ushort Harness = 0x0200;
    private const ushort Done = 0x0206;
    private const ushort GotoResult = 0x2200;
    private const ushort CutResult = 0x2240;

    [TestMethod]
    public void GotoLineParsesLineNumbersAbove255()
    {
        BusDevice bus = Run(GotoEntry);

        Assert.AreEqual(0xAA, bus.Read(GotoResult + 0x1F), "goto fixture ran to completion");

        // 1-based line input -> 0-based buffer line index.
        AssertGoto(bus, slot: 0, line: 0, "goto 1");
        AssertGoto(bus, slot: 3, line: 4, "goto 5");
        AssertGoto(bus, slot: 6, line: 255, "goto 256");
        AssertGoto(bus, slot: 9, line: 999, "goto 1000");
    }

    [TestMethod]
    public void CutNeverDeletesMoreThanItCopies()
    {
        BusDevice bus = Run(CutEntry);

        Assert.AreEqual(0xAA, bus.Read(CutResult + 4), "cut fixture ran to completion");

        int clip = bus.Read(CutResult + 2) | (bus.Read(CutResult + 3) << 8);
        int len = bus.Read(CutResult) | (bus.Read(CutResult + 1) << 8);

        // 300-byte selection, 256-byte clipboard cap -> copy clamps to 255.
        Assert.AreEqual(255, clip, "clipboard holds the clamped copy length");
        // Cut must delete exactly what it copied. The bug deletes the whole
        // selection (len -> 0) while only 255 bytes reach the clipboard: data loss.
        Assert.AreEqual(300 - 255, len, "buffer keeps bytes that could not be clipped");
    }

    private static void AssertGoto(BusDevice bus, int slot, int line, string label)
    {
        Assert.AreEqual(1, bus.Read((ushort)(GotoResult + slot)),
            $"{label}: parse should report a valid line");
        int got = bus.Read((ushort)(GotoResult + slot + 1))
                | (bus.Read((ushort)(GotoResult + slot + 2)) << 8);
        Assert.AreEqual(line, got, $"{label}: 0-based line number");
    }

    private static BusDevice Run(ushort entry)
    {
        byte[] fixture = File.ReadAllBytes(
            RepoPath("tests", "integration", "fixtures", "editbuf_runtime.bin"));
        var bus = new BusDevice(fixture, 0x4000);

        // LDX #$FF; TXS; JSR entry; JMP Done
        bus.Write(Harness, 0xA2);
        bus.Write(Harness + 1, 0xFF);
        bus.Write(Harness + 2, 0x9A);
        bus.Write(Harness + 3, 0x20);
        bus.Write(Harness + 4, (byte)(entry & 0xFF));
        bus.Write(Harness + 5, (byte)(entry >> 8));
        bus.Write(Harness + 6, 0x4C);
        bus.Write(Harness + 7, (byte)(Done & 0xFF));
        bus.Write(Harness + 8, (byte)(Done >> 8));

        var cpu = new Cpu(bus, E6502Type.Cmos);
        cpu.Boot(Harness);

        for (int i = 0; i < 2_000_000; i++)
        {
            ushort prevPc = cpu.Pc;
            cpu.ExecuteNext();
            if (prevPc == Done && cpu.Pc == Done)
                return bus;
        }

        Assert.Fail($"editbuf fixture did not return; PC=${cpu.Pc:X4}.");
        return bus;
    }

    private static string RepoPath(params string[] parts)
    {
        string root = Path.GetFullPath(Path.Combine(
            AppContext.BaseDirectory, "..", "..", "..", ".."));
        return Path.Combine([root, .. parts]);
    }
}

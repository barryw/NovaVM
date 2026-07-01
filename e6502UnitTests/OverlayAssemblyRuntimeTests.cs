using System;
using System.IO;
using KDS.e6502;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class OverlayAssemblyRuntimeTests
{
    private const ushort Entry = 0x7000;
    private const ushort Harness = 0x0200;
    private const ushort Done = 0x0206;
    private const ushort ResultBase = 0x2290;

    [TestMethod]
    public void OverlayAssemblyRuntimeCallsFixedAddressEntries()
    {
        byte[] fixture = File.ReadAllBytes(RepoPath("software", "tests", "integration", "fixtures", "overlay_runtime.bin"));
        var bus = new BusDevice(fixture, Entry);

        // LDX #$FF; TXS; JSR $7000; JMP $0206
        bus.Write(Harness, 0xA2);
        bus.Write(Harness + 1, 0xFF);
        bus.Write(Harness + 2, 0x9A);
        bus.Write(Harness + 3, 0x20);
        bus.Write(Harness + 4, (byte)(Entry & 0xFF));
        bus.Write(Harness + 5, (byte)(Entry >> 8));
        bus.Write(Harness + 6, 0x4C);
        bus.Write(Harness + 7, (byte)(Done & 0xFF));
        bus.Write(Harness + 8, (byte)(Done >> 8));

        var cpu = new Cpu(bus, E6502Type.Cmos);
        cpu.Boot(Harness);
        RunUntilDone(cpu);

        AssertByte(bus, 0x00, 0x00, "init entry returned OK");
        AssertByte(bus, 0x01, 0x00, "init entry left OVL_RESULT OK");
        AssertByte(bus, 0x02, 0x01, "init entry was called once");

        AssertByte(bus, 0x03, 0x00, "main entry returned OK");
        AssertByte(bus, 0x04, 0x00, "main entry left OVL_RESULT OK");
        AssertByte(bus, 0x05, 0x01, "main entry was called once");

        AssertByte(bus, 0x06, 0x01, "missing main returns generic error");
        AssertByte(bus, 0x07, 0x08, "missing main records OVL_ERR_NO_ENTRY");

        AssertByte(bus, 0x08, 0x01, "nonzero tick return maps to generic error");
        AssertByte(bus, 0x09, 0x09, "nonzero tick return records OVL_ERR_ENTRY");

        AssertByte(bus, 0x0A, 0x01, "calling unloaded overlay returns generic error");
        AssertByte(bus, 0x0B, 0x07, "calling unloaded overlay records OVL_ERR_NOT_LOADED");

        AssertByte(bus, 0x3F, 0xAA, "assembly fixture reached completion");
    }

    private static void RunUntilDone(Cpu cpu)
    {
        for (int i = 0; i < 50_000; i++)
        {
            ushort prevPc = cpu.Pc;
            cpu.ExecuteNext();
            if (prevPc == Done && cpu.Pc == Done)
                return;
        }

        Assert.Fail($"Assembly fixture did not return; PC=${cpu.Pc:X4}.");
    }

    private static void AssertByte(BusDevice bus, int offset, byte expected, string message)
    {
        Assert.AreEqual(expected, bus.Read((ushort)(ResultBase + offset)), message);
    }

    private static string RepoPath(params string[] parts)
    {
        string root = Path.GetFullPath(Path.Combine(
            AppContext.BaseDirectory,
            "..",
            "..",
            "..",
            ".."));

        return Path.Combine([root, .. parts]);
    }
}

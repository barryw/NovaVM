using System;
using System.IO;
using KDS.e6502;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class GameServerAssemblyRuntimeTests
{
    private const ushort Entry = 0x7000;
    private const ushort Harness = 0x0200;
    private const ushort Done = 0x0206;
    private const ushort ResultBase = 0x2290;

    [TestMethod]
    public void GameServerAssemblyRuntimeBuildsAndParsesFrames()
    {
        byte[] fixture = File.ReadAllBytes(RepoPath("tests", "integration", "fixtures", "gameserver_runtime.bin"));
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

        AssertByte(bus, 0x00, 0x00, "HELLO builder returned OK");
        AssertByte(bus, 0x01, 0x1A, "HELLO frame length");
        AssertByte(bus, 0x02, 0x01, "HELLO server protocol major");
        AssertByte(bus, 0x03, 0x01, "HELLO kind");
        AssertByte(bus, 0x04, 0x01, "HELLO sequence");
        AssertByte(bus, 0x05, 0x00, "HELLO server protocol minor payload");
        AssertByte(bus, 0x06, 0x06, "HELLO handle length");

        AssertByte(bus, 0x07, 0x00, "GAME_LIST_REQUEST builder returned OK");
        AssertByte(bus, 0x08, 0x04, "GAME_LIST_REQUEST frame length");
        AssertByte(bus, 0x09, 0x20, "GAME_LIST_REQUEST kind");
        AssertByte(bus, 0x0A, 0x02, "GAME_LIST_REQUEST sequence");

        AssertByte(bus, 0x0B, 0x00, "TABLE_CREATE builder returned OK");
        AssertByte(bus, 0x0C, 0x0D, "TABLE_CREATE frame length");
        AssertByte(bus, 0x0D, 0x32, "TABLE_CREATE kind");
        AssertByte(bus, 0x0E, 0x01, "TABLE_CREATE game id low");
        AssertByte(bus, 0x0F, 0x00, "TABLE_CREATE game id high");

        AssertByte(bus, 0x10, 0x00, "PLAYER_ACTION builder returned OK");
        AssertByte(bus, 0x11, 0x0B, "PLAYER_ACTION frame length");
        AssertByte(bus, 0x12, 0x41, "PLAYER_ACTION kind");
        AssertByte(bus, 0x13, 0x34, "PLAYER_ACTION table id low");
        AssertByte(bus, 0x14, 0x12, "PLAYER_ACTION table id high");
        AssertByte(bus, 0x15, 0x07, "PLAYER_ACTION action type");
        AssertByte(bus, 0x16, 0xDE, "PLAYER_ACTION first payload byte");

        AssertByte(bus, 0x17, 0x00, "WELCOME parse returned OK");
        AssertByte(bus, 0x18, 0x02, "WELCOME kind");
        AssertByte(bus, 0x19, 0x09, "WELCOME sequence");
        AssertByte(bus, 0x1A, 0x01, "WELCOME flags");
        AssertByte(bus, 0x1B, 0x10, "WELCOME payload remaining");
        AssertByte(bus, 0x1C, 0x34, "WELCOME session id low");
        AssertByte(bus, 0x1D, 0x12, "WELCOME session id high");
        AssertByte(bus, 0x1E, 0x06, "WELCOME handle length");
        AssertByte(bus, 0x1F, (byte)'n', "WELCOME handle first byte");

        AssertByte(bus, 0x20, 0x08, "ERROR parse returned protocol-error result");
        AssertByte(bus, 0x21, 0x0E, "ERROR parse code");
        AssertByte(bus, 0x22, 0x07, "ERROR parse text length");
        AssertByte(bus, 0x23, (byte)'t', "ERROR parse text first byte");

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

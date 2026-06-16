using System;
using System.IO;
using KDS.e6502;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class NicDmaAssemblyRuntimeTests
{
    private const ushort Entry = 0x7000;
    private const ushort Harness = 0x0200;
    private const ushort Done = 0x0206;
    private const ushort Result = 0x2200;

    private const ushort NicCmd = 0xA100;
    private const ushort NicDmaErr = 0xA115;
    private const byte NicCmdSend = 0x03;
    private const byte NicDmaErrRange = 0x01;

    // Bus double: a flat RAM image whose NIC faults every payload DMA. Writing a
    // SEND command latches NIC_DMAERR=RANGE and clears NIC_CMD (command consumed),
    // exactly as a controller that rejected an out-of-range DMA would present.
    private sealed class FaultNicBus : IBusDevice
    {
        private readonly byte[] _ram = new byte[65536];

        public FaultNicBus(byte[] image, ushort loadAddr)
        {
            Array.Copy(image, 0, _ram, loadAddr, image.Length);
        }

        public byte Read(ushort address) => _ram[address];

        public void Write(ushort address, byte value)
        {
            if (address == NicCmd && value == NicCmdSend)
            {
                _ram[NicDmaErr] = NicDmaErrRange;
                _ram[NicCmd] = 0;       // command consumed; wait-for-clear succeeds
                return;
            }
            _ram[address] = value;
        }
    }

    [TestMethod]
    public void SendSurfacesDmaError()
    {
        byte[] fixture = File.ReadAllBytes(
            RepoPath("tests", "integration", "fixtures", "nicdma_runtime.bin"));
        var bus = new FaultNicBus(fixture, Entry);

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
        for (int i = 0; i < 500_000; i++)
        {
            ushort prevPc = cpu.Pc;
            cpu.ExecuteNext();
            if (prevPc == Done && cpu.Pc == Done)
                break;
        }

        Assert.AreEqual(0xAA, bus.Read(Result + 2), "nicdma fixture ran to completion");
        // A faulted DMA transferred no bytes -> ngs_send_current must report a
        // nonzero NGS_RESULT, not silent success.
        Assert.AreNotEqual(0, bus.Read(Result + 1), "send surfaces the NIC DMA error");
    }

    private static string RepoPath(params string[] parts)
    {
        string root = Path.GetFullPath(Path.Combine(
            AppContext.BaseDirectory, "..", "..", "..", ".."));
        return Path.Combine([root, .. parts]);
    }
}

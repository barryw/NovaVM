using System;
using System.IO;
using KDS.e6502;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class VspriteAssemblyRuntimeTests
{
    private const ushort Entry = 0x7000;
    private const ushort Harness = 0x0200;
    private const ushort Done = 0x0206;
    private const ushort Result = 0x2200;

    [TestMethod]
    public void SceneWorkAddrClampsSpriteAboveDirtyRect()
    {
        BusDevice bus = Run();

        Assert.AreEqual(0xAA, bus.Read(Result + 6), "vsprite fixture ran to completion");

        // Work buffer base = $010000, row stride = 64.
        // Case A: VSPRITE_Y (10) < VSPRITE_SCENE_Y (20). The row delta must clamp
        // to 0 (stay at the buffer base) instead of underflowing to 246 rows and
        // walking the destination ~15.7KB past the work buffer.
        int caseA = bus.Read(Result + 0) | (bus.Read(Result + 1) << 8) | (bus.Read(Result + 2) << 16);
        Assert.AreEqual(0x010000, caseA, "sprite above dirty rect stays at work-buffer base");

        // Case B: VSPRITE_Y (13) >= VSPRITE_SCENE_Y (10), 3 rows -> base + 3*64.
        int caseB = bus.Read(Result + 3) | (bus.Read(Result + 4) << 8) | (bus.Read(Result + 5) << 16);
        Assert.AreEqual(0x010000 + 3 * 64, caseB, "normal row delta unaffected");
    }

    private static BusDevice Run()
    {
        byte[] fixture = File.ReadAllBytes(
            RepoPath("tests", "integration", "fixtures", "vsprite_runtime.bin"));
        var bus = new BusDevice(fixture, Entry);

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

        for (int i = 0; i < 200_000; i++)
        {
            ushort prevPc = cpu.Pc;
            cpu.ExecuteNext();
            if (prevPc == Done && cpu.Pc == Done)
                return bus;
        }

        Assert.Fail($"vsprite fixture did not return; PC=${cpu.Pc:X4}.");
        return bus;
    }

    private static string RepoPath(params string[] parts)
    {
        string root = Path.GetFullPath(Path.Combine(
            AppContext.BaseDirectory, "..", "..", "..", ".."));
        return Path.Combine([root, .. parts]);
    }
}

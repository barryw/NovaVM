using System;
using System.Collections.Generic;
using System.IO;
using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class ShelfMissTests
{
    private static string RepoPath(params string[] p) =>
        Path.Combine(Path.GetFullPath(Path.Combine(AppContext.BaseDirectory, "..", "..", "..", "..")), Path.Combine(p));

    [TestMethod]
    public void Fio_LoadModule_StreamsStoreImageIntoSlot()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        byte[] gfx = File.ReadAllBytes(RepoPath("modules", "graphics", "graphics.bin"));
        bus.SetShelfModuleStore(new Dictionary<byte, byte[]> { [1] = gfx });

        // request LOAD_MODULE(id=1, slot=2). FIO owns its register bank, so params and the
        // trigger must go through the routed bus write (Write), not the raw RAM hook (WriteRam).
        bus.Write(0xB9A4, 1);     // OFF_SRC_LO = id   (FioSrcL)
        bus.Write(0xB9A6, 2);     // OFF_END_LO = slot (FioEndL)
        bus.Write(0xB9A0, 0x2C);  // CMD_LOAD_MODULE -> triggers synchronously

        // host (emulator) executes synchronously on the cmd write; verify XRAM slot 2 == image
        for (int i = 0; i < gfx.Length; i++)
            Assert.AreEqual(gfx[i], bus.ReadXram(0x068000 + i), $"slot-2 XRAM mismatch at {i}");
        Assert.AreEqual(2, bus.Read(0xB9A1), "FIO STATUS must be OK(2)");
    }

    [TestMethod]
    public void Fio_LoadModule_UnknownId_SetsError()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        bus.SetShelfModuleStore(new Dictionary<byte, byte[]>());   // empty store
        bus.Write(0xB9A4, 0x42);
        bus.Write(0xB9A6, 0);
        bus.Write(0xB9A0, 0x2C);
        Assert.AreEqual(3, bus.Read(0xB9A1), "unknown id must set FIO STATUS ERR(3)");
    }
}

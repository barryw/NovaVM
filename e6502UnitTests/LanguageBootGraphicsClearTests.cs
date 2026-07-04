using System;
using e6502.Avalonia.Hardware;
using KDS.e6502;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class LanguageBootGraphicsClearTests
{
    private const int TextColumns = 80;
    private const int TextRows = 50;

    [DataTestMethod]
    [DataRow(CompositeBusDevice.ActiveRom.Basic, "Ready")]
    [DataRow(CompositeBusDevice.ActiveRom.Logo, "?")]
    [DataRow(CompositeBusDevice.ActiveRom.Forth, "OK>")]
    public void LanguageColdStartClearsGraphicsBitmap(CompositeBusDevice.ActiveRom rom, string readyText)
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: rom);
        int[] probes = { 0, 1, 320, 1234, 31999, VgcConstants.GfxWidth * VgcConstants.GfxHeight - 1 };
        foreach (int probe in probes)
        {
            Assert.IsTrue(bus.Vgc.TryWriteMemorySpace(VgcConstants.MemSpaceGfx, probe, 0x5A),
                $"failed to seed stale gfx byte {probe}");
        }

        var cpu = new Cpu(bus);
        cpu.Boot();
        RunUntilScreenContains(cpu, bus, readyText, 60_000_000);

        foreach (int probe in probes)
        {
            Assert.IsTrue(bus.Vgc.TryReadMemorySpace(VgcConstants.MemSpaceGfx, probe, out byte value),
                $"failed to read gfx byte {probe}");
            Assert.AreEqual(0x00, value,
                $"Booting {rom} must clear stale bitmap byte {probe}, not leave prior-language graphics visible.");
        }
    }

    private static void RunUntilScreenContains(Cpu cpu, CompositeBusDevice bus, string text, int maxSteps)
    {
        for (int i = 0; i < maxSteps; i++)
        {
            cpu.ExecuteNext();
            bus.AdvanceCycles(16);
            if ((i & 0x0FFF) == 0 && SnapshotScreen(bus).Contains(text, StringComparison.Ordinal))
                return;
        }

        Assert.Fail($"Timed out waiting for '{text}'.\n{SnapshotScreen(bus)}");
    }

    private static string SnapshotScreen(CompositeBusDevice bus)
    {
        char[] chars = new char[TextRows * (TextColumns + 1)];
        int idx = 0;
        for (int row = 0; row < TextRows; row++)
        {
            for (int col = 0; col < TextColumns; col++)
            {
                byte ch = bus.Vgc.GetScreenChar(col, row);
                chars[idx++] = ch is >= 32 and < 127 ? (char)ch : ' ';
            }
            chars[idx++] = '\n';
        }

        return new string(chars);
    }
}

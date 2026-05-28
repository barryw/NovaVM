using System;
using System.Text;
using e6502.Avalonia.Hardware;
using e6502.Avalonia.Input;
using KDS.e6502;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class NovaLogoTests
{
    [TestMethod]
    public void BootShowsBannerAndPrompt()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();

        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screen.Contains("NOVALOGO", StringComparison.Ordinal),
            $"Expected NOVALOGO banner on screen.\n{screen}");
        Assert.IsTrue(screen.Contains("?", StringComparison.Ordinal),
            $"Expected ? prompt on screen.\n{screen}");
    }

    [TestMethod]
    public void TypedCharactersAreEchoed()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        // Type "HELLO" — should appear on screen after the prompt
        foreach (char ch in "HELLO")
            editor.QueueInput((byte)ch);

        // Run enough cycles for all characters to be read and echoed
        RunSteps(cpu, bus, 500_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screen.Contains("HELLO", StringComparison.Ordinal),
            $"Expected typed text HELLO on screen.\n{screen}");
    }

    private static void RunUntilScreenContains(Cpu cpu, CompositeBusDevice bus, string marker, int maxSteps)
    {
        RunUntil(cpu, bus, maxSteps,
            () => SnapshotScreen(bus.Vgc).Contains(marker, StringComparison.Ordinal),
            $"screen to contain '{marker}'");
    }

    private static void RunUntil(Cpu cpu, CompositeBusDevice bus, int maxSteps, Func<bool> predicate, string description)
    {
        for (int i = 0; i < maxSteps; i++)
        {
            int cycles = cpu.ClocksForNext();
            cpu.ExecuteNext();
            bus.AdvanceCycles(cycles);
            if ((i & 0x3FF) == 0 && predicate())
                return;
        }
        Assert.Fail($"Timed out waiting for {description}.\n{SnapshotScreen(bus.Vgc)}");
    }

    private static void RunSteps(Cpu cpu, CompositeBusDevice bus, int steps)
    {
        for (int i = 0; i < steps; i++)
        {
            int cycles = cpu.ClocksForNext();
            cpu.ExecuteNext();
            bus.AdvanceCycles(cycles);
        }
    }

    private static string SnapshotScreen(VirtualGraphicsController vgc)
    {
        var sb = new StringBuilder();
        for (int row = 0; row < VgcConstants.ScreenRows; row++)
        {
            for (int col = 0; col < VgcConstants.ScreenCols; col++)
            {
                byte ch = vgc.GetScreenChar(col, row);
                sb.Append(ch >= 0x20 && ch <= 0x7E ? (char)ch : ' ');
            }
            sb.Append('\n');
        }
        return sb.ToString();
    }
}

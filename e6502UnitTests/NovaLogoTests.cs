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

    [TestMethod]
    public void BackspaceDeletesCharacter()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        // Type "HELXO" then backspace twice and retype "LO"
        foreach (byte b in new byte[] {
            (byte)'H', (byte)'E', (byte)'L', (byte)'X', (byte)'O',
            0x08, 0x08,  // two backspaces
            (byte)'L', (byte)'O',
            0x0D  // Enter
        })
            editor.QueueInput(b);

        RunSteps(cpu, bus, 1_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screen.Contains("HELLO", StringComparison.Ordinal),
            $"Expected HELLO after backspace correction.\n{screen}");
    }

    [TestMethod]
    public void PrintNumber()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "PRINT 42");
        RunSteps(cpu, bus, 2_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsFalse(screen.Contains("NOT ENOUGH", StringComparison.Ordinal),
            $"Got spurious NOT ENOUGH INPUTS.\n{screen}");
        Assert.IsTrue(screen.Contains("42", StringComparison.Ordinal),
            $"Expected '42' from PRINT 42.\n{screen}");
    }

    [TestMethod]
    public void PrintQuotedWord()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        foreach (char ch in "PRINT \"HELLO")
            editor.QueueInput((byte)ch);
        editor.QueueInput(0x0D);

        RunSteps(cpu, bus, 2_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screen.Contains("HELLO", StringComparison.Ordinal),
            $"Expected 'HELLO' from PRINT \"HELLO.\n{screen}");
    }

    [TestMethod]
    public void ArithmeticOperations()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        // Test addition
        QueueLine(editor, "PRINT 3 + 4");
        RunSteps(cpu, bus, 2_000_000);
        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screen.Contains("7", StringComparison.Ordinal),
            $"Expected '7' from PRINT 3 + 4.\n{screen}");

        // Test subtraction
        QueueLine(editor, "PRINT 10 - 3");
        RunSteps(cpu, bus, 2_000_000);
        screen = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screen.Contains("7", StringComparison.Ordinal),
            $"Expected '7' from PRINT 10 - 3.\n{screen}");

        // Test multiplication
        QueueLine(editor, "PRINT 10 * 5");
        RunSteps(cpu, bus, 2_000_000);
        screen = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screen.Contains("50", StringComparison.Ordinal),
            $"Expected '50' from PRINT 10 * 5.\n{screen}");

        // Test division
        QueueLine(editor, "PRINT 20 / 4");
        RunSteps(cpu, bus, 2_000_000);
        screen = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screen.Contains("5", StringComparison.Ordinal),
            $"Expected '5' from PRINT 20 / 4.\n{screen}");
    }

    [TestMethod]
    public void MakeAndPrintVariable()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "MAKE \"X 42");
        RunSteps(cpu, bus, 2_000_000);

        QueueLine(editor, "PRINT :X");
        RunSteps(cpu, bus, 2_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screen.Contains("42", StringComparison.Ordinal),
            $"Expected '42' from PRINT :X after MAKE \"X 42.\n{screen}");
    }

    [TestMethod]
    public void VariableInExpression()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "MAKE \"X 10");
        RunSteps(cpu, bus, 2_000_000);
        QueueLine(editor, "PRINT :X + 5");
        RunSteps(cpu, bus, 2_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screen.Contains("15", StringComparison.Ordinal),
            $"Expected '15' from PRINT :X + 5.\n{screen}");
    }

    private static void QueueLine(ScreenEditor editor, string line)
    {
        foreach (char ch in line)
            editor.QueueInput((byte)ch);
        editor.QueueInput(0x0D);
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

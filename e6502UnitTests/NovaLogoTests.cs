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
    public void RepeatPrintsMultipleTimes()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "REPEAT 3 [PRINT 42]");
        RunSteps(cpu, bus, 5_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        // Count occurrences of "42" on separate lines
        int count = 0;
        foreach (string line in screen.Split('\n'))
            if (line.TrimEnd().EndsWith("42")) count++;
        Assert.IsTrue(count >= 3,
            $"Expected at least 3 lines ending with '42' from REPEAT 3 [PRINT 42].\n{screen}");
    }

    [TestMethod]
    public void RepeatZeroTimesDoesNothing()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "REPEAT 0 [PRINT 99]");
        RunSteps(cpu, bus, 5_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        // Count lines where "99" appears alone (not part of the input echo)
        int count = 0;
        foreach (string line in screen.Split('\n'))
        {
            string trimmed = line.TrimEnd();
            if (trimmed.EndsWith("99") && !trimmed.Contains("REPEAT"))
                count++;
        }
        Assert.AreEqual(0, count,
            $"Expected no output lines ending with '99' from REPEAT 0 [PRINT 99].\n{screen}");
    }

    [TestMethod]
    public void RepeatWithMultipleCommands()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        // Two commands in one body
        QueueLine(editor, "REPEAT 2 [TYPE 7 PRINT 8]");
        RunSteps(cpu, bus, 5_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        // Should see "78" printed as "7" then "8\n" twice
        int count = 0;
        foreach (string line in screen.Split('\n'))
            if (line.TrimEnd().EndsWith("78")) count++;
        Assert.IsTrue(count >= 2,
            $"Expected at least 2 lines ending with '78' from REPEAT 2 [TYPE 7 PRINT 8].\n{screen}");
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

    [TestMethod]
    public void IfTrueExecutesBody()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "IF 10 > 5 [PRINT \"YES]");
        RunSteps(cpu, bus, 3_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screen.Contains("YES", StringComparison.Ordinal),
            $"Expected 'YES' from IF 10 > 5.\n{screen}");
    }

    [TestMethod]
    public void IfFalseSkipsBody()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "IF 3 > 5 [PRINT 777]");
        RunSteps(cpu, bus, 3_000_000);
        QueueLine(editor, "PRINT \"DONE");
        RunSteps(cpu, bus, 3_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        // 777 should NOT appear on its own line (only as part of the echoed input)
        int count777 = 0;
        foreach (string line in screen.Split('\n'))
        {
            string t = line.TrimEnd();
            if (t.EndsWith("777") && !t.Contains("IF"))
                count777++;
        }
        Assert.AreEqual(0, count777,
            $"Expected no output line '777' from IF 3 > 5.\n{screen}");
        Assert.IsTrue(screen.Contains("DONE", StringComparison.Ordinal),
            $"Expected DONE after skipped IF.\n{screen}");
    }

    [TestMethod]
    public void IfElseSelectsBranch()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "IFELSE 10 > 5 [PRINT 111] [PRINT 222]");
        RunSteps(cpu, bus, 3_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        // 111 should appear as output on its own line, 222 should not
        int count111 = 0;
        int count222 = 0;
        foreach (string line in screen.Split('\n'))
        {
            string t = line.TrimEnd();
            if (t.EndsWith("111") && !t.Contains("IFELSE"))
                count111++;
            if (t.EndsWith("222") && !t.Contains("IFELSE"))
                count222++;
        }
        Assert.IsTrue(count111 >= 1,
            $"Expected output '111' from IFELSE 10 > 5 true branch.\n{screen}");
        Assert.AreEqual(0, count222,
            $"Expected no output '222' from IFELSE 10 > 5 false branch.\n{screen}");
    }

    [TestMethod]
    public void DefineAndCallNoParamProcedure()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        // Define a no-param procedure
        QueueLine(editor, "TO HI");
        RunSteps(cpu, bus, 2_000_000);
        QueueLine(editor, "PRINT 99");
        RunSteps(cpu, bus, 2_000_000);
        QueueLine(editor, "END");
        RunSteps(cpu, bus, 2_000_000);

        string screenAfterDef = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screenAfterDef.Contains("DEFINED", StringComparison.Ordinal),
            $"Expected 'DEFINED' confirmation.\n{screenAfterDef}");

        // Call it
        QueueLine(editor, "HI");
        RunSteps(cpu, bus, 3_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        int count = 0;
        foreach (string line in screen.Split('\n'))
        {
            string t = line.Trim();
            if (t == "99") count++;
        }
        Assert.IsTrue(count >= 1,
            $"Expected at least one line of just '99' from HI.\n{screen}");
    }

    [TestMethod]
    public void DefineAndCallProcedure()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        // Define procedure
        QueueLine(editor, "TO SQUARE :SIZE");
        RunSteps(cpu, bus, 2_000_000);
        QueueLine(editor, "PRINT :SIZE");
        RunSteps(cpu, bus, 2_000_000);
        QueueLine(editor, "END");
        RunSteps(cpu, bus, 2_000_000);

        string screenAfterDef = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screenAfterDef.Contains("DEFINED", StringComparison.Ordinal),
            $"Expected 'DEFINED' confirmation.\n{screenAfterDef}");

        // Call it
        QueueLine(editor, "SQUARE 42");
        RunSteps(cpu, bus, 3_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        int count = 0;
        foreach (string line in screen.Split('\n'))
        {
            string t = line.Trim();
            if (t == "42") count++;
        }
        Assert.IsTrue(count >= 1,
            $"Expected at least one line of just '42' from SQUARE 42.\n{screen}");
    }

    [TestMethod]
    public void ProcedureWithTwoParams()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "TO ADD2 :A :B");
        RunSteps(cpu, bus, 2_000_000);
        QueueLine(editor, "PRINT :A + :B");
        RunSteps(cpu, bus, 2_000_000);
        QueueLine(editor, "END");
        RunSteps(cpu, bus, 2_000_000);

        QueueLine(editor, "ADD2 10 20");
        RunSteps(cpu, bus, 3_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        int count = 0;
        foreach (string line in screen.Split('\n'))
        {
            string t = line.Trim();
            if (t == "30") count++;
        }
        Assert.IsTrue(count >= 1,
            $"Expected '30' from ADD2 10 20.\n{screen}");
    }

    [TestMethod]
    public void StopExitsProcedureEarly()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "TO HALF :N");
        RunSteps(cpu, bus, 1_000_000);
        QueueLine(editor, "IF :N < 2 [STOP]");
        RunSteps(cpu, bus, 1_000_000);
        QueueLine(editor, "PRINT :N");
        RunSteps(cpu, bus, 1_000_000);
        QueueLine(editor, "END");
        RunSteps(cpu, bus, 2_000_000);

        QueueLine(editor, "HALF 1");
        RunSteps(cpu, bus, 3_000_000);
        QueueLine(editor, "HALF 5");
        RunSteps(cpu, bus, 3_000_000);
        QueueLine(editor, "PRINT 99");
        RunSteps(cpu, bus, 3_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        // HALF 1 should not print (STOP fires), HALF 5 should print 5
        int count5 = 0, count99 = 0;
        foreach (string line in screen.Split('\n'))
        {
            string t = line.Trim();
            if (t == "5") count5++;
            if (t == "99") count99++;
        }
        Assert.IsTrue(count5 >= 1, $"Expected '5' from HALF 5.\n{screen}");
        Assert.IsTrue(count99 >= 1, $"Expected '99' after both HALF calls.\n{screen}");
    }

    [TestMethod]
    public void OutputReturnValue()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "TO DOUBLE :N");
        RunSteps(cpu, bus, 1_000_000);
        QueueLine(editor, "OUTPUT :N * 2");
        RunSteps(cpu, bus, 1_000_000);
        QueueLine(editor, "END");
        RunSteps(cpu, bus, 2_000_000);

        QueueLine(editor, "PRINT DOUBLE 5");
        RunSteps(cpu, bus, 4_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        int count = 0;
        foreach (string line in screen.Split('\n'))
        {
            string t = line.Trim();
            if (t == "10") count++;
        }
        Assert.IsTrue(count >= 1, $"Expected '10' from PRINT DOUBLE 5.\n{screen}");
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

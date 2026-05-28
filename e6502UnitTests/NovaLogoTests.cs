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

    [TestMethod]
    public void RecursiveProcedure()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        // Define recursive countdown
        QueueLine(editor, "TO COUNTDOWN :N");
        RunSteps(cpu, bus, 1_000_000);
        QueueLine(editor, "IF :N = 0 [STOP]");
        RunSteps(cpu, bus, 1_000_000);
        QueueLine(editor, "PRINT :N");
        RunSteps(cpu, bus, 1_000_000);
        QueueLine(editor, "COUNTDOWN :N - 1");
        RunSteps(cpu, bus, 1_000_000);
        QueueLine(editor, "END");
        RunSteps(cpu, bus, 2_000_000);

        // Call it
        QueueLine(editor, "COUNTDOWN 5");
        RunSteps(cpu, bus, 10_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        // Should see 5, 4, 3, 2, 1 on separate lines
        bool has5 = false, has4 = false, has3 = false, has2 = false, has1 = false;
        foreach (string line in screen.Split('\n'))
        {
            string t = line.Trim();
            if (t == "5") has5 = true;
            if (t == "4") has4 = true;
            if (t == "3") has3 = true;
            if (t == "2") has2 = true;
            if (t == "1") has1 = true;
        }
        Assert.IsTrue(has5 && has4 && has3 && has2 && has1,
            $"Expected countdown 5,4,3,2,1 from recursive COUNTDOWN 5.\n{screen}");
    }

    [TestMethod]
    public void RecursiveOutputFunction()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        // Define recursive factorial (simplified: just multiply down)
        // FACT 1 = 1, FACT N = N * FACT N-1
        QueueLine(editor, "TO FACT :N");
        RunSteps(cpu, bus, 1_000_000);
        QueueLine(editor, "IF :N = 1 [OUTPUT 1]");
        RunSteps(cpu, bus, 1_000_000);
        QueueLine(editor, "OUTPUT :N * FACT :N - 1");
        RunSteps(cpu, bus, 1_000_000);
        QueueLine(editor, "END");
        RunSteps(cpu, bus, 2_000_000);

        QueueLine(editor, "PRINT FACT 5");
        RunSteps(cpu, bus, 15_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        // 5! = 120
        int count = 0;
        foreach (string line in screen.Split('\n'))
        {
            string t = line.Trim();
            if (t == "120") count++;
        }
        Assert.IsTrue(count >= 1,
            $"Expected '120' from PRINT FACT 5.\n{screen}");
    }

    [TestMethod]
    public void FirstOfList()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "PRINT FIRST [10 20 30]");
        RunSteps(cpu, bus, 3_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        bool found = false;
        foreach (string line in screen.Split('\n'))
            if (line.Trim() == "10") found = true;
        Assert.IsTrue(found, $"Expected '10' from PRINT FIRST [10 20 30].\n{screen}");
    }

    [TestMethod]
    public void CountOfList()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "PRINT COUNT [1 2 3]");
        RunSteps(cpu, bus, 3_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        bool found = false;
        foreach (string line in screen.Split('\n'))
            if (line.Trim() == "3") found = true;
        Assert.IsTrue(found, $"Expected '3' from PRINT COUNT [1 2 3].\n{screen}");
    }

    [TestMethod]
    public void EmptyListTest()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "PRINT EMPTY? []");
        RunSteps(cpu, bus, 3_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        bool found = false;
        foreach (string line in screen.Split('\n'))
            if (line.Trim() == "1") found = true;
        Assert.IsTrue(found, $"Expected '1' (TRUE) from PRINT EMPTY? [].\n{screen}");
    }

    [TestMethod]
    public void LastOfList()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "PRINT LAST [10 20 30]");
        RunSteps(cpu, bus, 3_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        bool found = false;
        foreach (string line in screen.Split('\n'))
            if (line.Trim() == "30") found = true;
        Assert.IsTrue(found, $"Expected '30' from PRINT LAST [10 20 30].\n{screen}");
    }

    [TestMethod]
    public void ButFirstOfList()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "PRINT BUTFIRST [10 20 30]");
        RunSteps(cpu, bus, 3_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        bool found = false;
        foreach (string line in screen.Split('\n'))
            if (line.Trim() == "[20 30]") found = true;
        Assert.IsTrue(found, $"Expected '[20 30]' from PRINT BUTFIRST [10 20 30].\n{screen}");
    }

    [TestMethod]
    public void ItemOfList()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "PRINT ITEM 2 [10 20 30]");
        RunSteps(cpu, bus, 3_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        bool found = false;
        foreach (string line in screen.Split('\n'))
            if (line.Trim() == "20") found = true;
        Assert.IsTrue(found, $"Expected '20' from PRINT ITEM 2 [10 20 30].\n{screen}");
    }

    [TestMethod]
    public void ShowList()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "SHOW [1 2 3]");
        RunSteps(cpu, bus, 3_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        bool found = false;
        foreach (string line in screen.Split('\n'))
            if (line.Trim() == "[1 2 3]") found = true;
        Assert.IsTrue(found, $"Expected '[1 2 3]' from SHOW [1 2 3].\n{screen}");
    }

    [TestMethod]
    public void FputConstructsList()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "SHOW FPUT 1 [2 3]");
        RunSteps(cpu, bus, 3_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screen.Contains("[1 2 3]", StringComparison.Ordinal),
            $"Expected '[1 2 3]' from SHOW FPUT 1 [2 3].\n{screen}");
    }

    [TestMethod]
    public void RunEvaluatesListAsCode()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "MAKE \"CMD [PRINT 777]");
        RunSteps(cpu, bus, 2_000_000);
        QueueLine(editor, "RUN :CMD");
        RunSteps(cpu, bus, 3_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        bool found = false;
        foreach (string line in screen.Split('\n'))
            if (line.Trim() == "777") found = true;
        Assert.IsTrue(found, $"Expected '777' from RUN :CMD.\n{screen}");
    }

    [TestMethod]
    public void WordConcatenates()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "PRINT WORD \"HEL \"LO");
        RunSteps(cpu, bus, 3_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screen.Contains("HELLO", StringComparison.Ordinal),
            $"Expected 'HELLO' from PRINT WORD \"HEL \"LO.\n{screen}");
    }

    [TestMethod]
    public void GarbageCollectionFreesMemory()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        // This would exhaust ~39KB heap without GC.
        // Each iteration allocates 2 cons pairs (18 bytes with headers).
        // 2500 * 18 = 45000 bytes > 39KB heap → OOM without GC.
        // With GC, the previous iteration's list cells are reclaimed.
        QueueLine(editor, "REPEAT 2500 [MAKE \"X LIST 1 2]");
        RunSteps(cpu, bus, 200_000_000);
        QueueLine(editor, "PRINT \"DONE");
        RunSteps(cpu, bus, 3_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        bool found = false;
        foreach (string line in screen.Split('\n'))
            if (line.Trim() == "DONE") found = true;
        Assert.IsTrue(found,
            $"Expected 'DONE' — program should not OOM with GC.\n{screen}");
        Assert.IsFalse(screen.Contains("OUT OF MEMORY", StringComparison.OrdinalIgnoreCase),
            $"Got OOM error — GC didn't reclaim enough.\n{screen}");
    }

    [TestMethod]
    public void ForLoopCountsUp()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "FOR [I 1 5] [TYPE :I]");
        RunSteps(cpu, bus, 5_000_000);
        QueueLine(editor, "PRINT \"X");
        RunSteps(cpu, bus, 3_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screen.Contains("12345", StringComparison.Ordinal),
            $"Expected '12345' from FOR loop.\n{screen}");
    }

    [TestMethod]
    public void RepcountInRepeat()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "REPEAT 3 [TYPE REPCOUNT]");
        RunSteps(cpu, bus, 5_000_000);
        QueueLine(editor, "PRINT \"X");
        RunSteps(cpu, bus, 3_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screen.Contains("123", StringComparison.Ordinal),
            $"Expected '123' from REPCOUNT in REPEAT.\n{screen}");
    }

    [TestMethod]
    public void WhileLoop()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "MAKE \"X 1");
        RunSteps(cpu, bus, 2_000_000);
        QueueLine(editor, "WHILE [:X < 4] [TYPE :X MAKE \"X :X + 1]");
        RunSteps(cpu, bus, 5_000_000);
        QueueLine(editor, "PRINT \"X");
        RunSteps(cpu, bus, 3_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screen.Contains("123", StringComparison.Ordinal),
            $"Expected '123' from WHILE loop.\n{screen}");
    }

    [TestMethod]
    public void CatchPreventsErrorAbort()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "CATCH \"ERROR [PRINT THING \"NOPE]");
        RunSteps(cpu, bus, 3_000_000);
        QueueLine(editor, "PRINT 999");
        RunSteps(cpu, bus, 3_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        bool found999 = false;
        foreach (string line in screen.Split('\n'))
            if (line.Trim() == "999") found999 = true;
        Assert.IsTrue(found999,
            $"Expected '999' after CATCH — error should not abort.\n{screen}");
    }

    [TestMethod]
    public void ThrowUnwindsToCatch()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "TO BOOM");
        RunSteps(cpu, bus, 1_000_000);
        QueueLine(editor, "PRINT 111");
        RunSteps(cpu, bus, 1_000_000);
        QueueLine(editor, "THROW \"DONE");
        RunSteps(cpu, bus, 1_000_000);
        QueueLine(editor, "PRINT 222");
        RunSteps(cpu, bus, 1_000_000);
        QueueLine(editor, "END");
        RunSteps(cpu, bus, 2_000_000);

        QueueLine(editor, "CATCH \"DONE [BOOM]");
        RunSteps(cpu, bus, 5_000_000);
        QueueLine(editor, "PRINT 333");
        RunSteps(cpu, bus, 3_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        bool has111 = false, has222 = false, has333 = false;
        foreach (string line in screen.Split('\n'))
        {
            string t = line.Trim();
            if (t == "111") has111 = true;
            if (t == "222") has222 = true;
            if (t == "333") has333 = true;
        }
        Assert.IsTrue(has111, $"Expected '111' before THROW.\n{screen}");
        Assert.IsFalse(has222, $"Should NOT see '222' after THROW.\n{screen}");
        Assert.IsTrue(has333, $"Expected '333' after CATCH completes.\n{screen}");
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

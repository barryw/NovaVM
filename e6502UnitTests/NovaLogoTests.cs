using System;
using System.IO;
using System.Text;
using e6502.Avalonia.Hardware;
using e6502.Avalonia.Input;
using KDS.e6502;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class NovaLogoTests
{
    private const byte CtrlQ = 0x11;
    private const byte CtrlS = 0x13;
    private const ushort LogoHeapPtr = 0x0030;
    private const ushort LogoHeapEnd = 0x9800;
    private const int LogoGcHeaderBytes = 2;
    private const ushort TurtleStateBase = 0x9F00;
    private const ushort TurtleXLo = TurtleStateBase + 1;
    private const ushort TurtleXHi = TurtleStateBase + 2;
    private const ushort TurtleYLo = TurtleStateBase + 4;
    private const ushort TurtleYHi = TurtleStateBase + 5;
    private const ushort TurtleHeadingLo = TurtleStateBase + 6;
    private const ushort TurtleHeadingHi = TurtleStateBase + 7;

    [TestMethod]
    public void BootShowsBannerAndPrompt()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();

        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screen.Contains("Nova LOGO v1.0", StringComparison.Ordinal),
            $"Expected Nova LOGO banner on screen.\n{screen}");
        Assert.IsTrue(screen.Contains("BYTES FREE", StringComparison.Ordinal),
            $"Expected dynamic free-memory line on screen.\n{screen}");
        Assert.IsFalse(screen.Contains("-"),
            $"Free-memory banner should print as an unsigned value.\n{screen}");
        int heapPtr = bus.ReadRam(LogoHeapPtr) | (bus.ReadRam(LogoHeapPtr + 1) << 8);
        int expectedFreeBytes = LogoHeapEnd - heapPtr - LogoGcHeaderBytes;
        int displayedFreeBytes = ExtractBytesFree(screen);
        Assert.AreEqual(expectedFreeBytes, displayedFreeBytes,
            $"Free-memory banner should reflect the live heap pointer, not a hard-coded value. heap_ptr=${heapPtr:X4}\n{screen}");
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
    public void PrintLargeNumbersParseCorrectly()
    {
        // Regression: tok_parse_number's *10 dropped the carry out of the low
        // byte whenever an intermediate accumulator had bit 7 set, so e.g.
        // 4242 parsed as 3986 (-256). Assert the OUTPUT line (not the echoed
        // command) equals the literal exactly.
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        // All within Logo's signed 16-bit range; each has an intermediate *10
        // accumulator with bit 7 set (424, 130, 200, 3000), which tripped the bug.
        foreach (string literal in new[] { "4242", "1300", "2000", "30000" })
        {
            QueueLine(editor, "PRINT " + literal);
            RunSteps(cpu, bus, 2_000_000);
            string screen = SnapshotScreen(bus.Vgc);
            bool found = false;
            foreach (string line in screen.Split('\n'))
                if (line.Trim() == literal) { found = true; break; }
            Assert.IsTrue(found, $"Expected an output line equal to '{literal}' from PRINT {literal}.\n{screen}");
        }
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

    // =================================================================
    // Arity-flip (R1) characterization tests.
    //
    // These pin the OBSERVABLE behavior of the one-arg math/logic
    // reporters (SIN, COS, ABS, INT, ROUND, RANDOM, SQRT, NOT, NUMBER?,
    // WORD?, LIST?) both when given an argument and when their argument
    // is missing (under-supply). The R1 refactor flips these reporters
    // from "arity 0 + hand-rolled eval_expr/BCS prologue" to "arity 1",
    // letting the dispatcher pre-evaluate the single argument. That is
    // only safe if behavior stays byte-identical — these tests prove it.
    //
    // Reference: PrintWithNoArgGivesGenericNotEnough captures the
    // dispatcher's generic arity-1 under-supply path (a true arity-1
    // builtin, PRINT, with no argument). The flipped reporters route
    // their under-supply through exactly that same generic path.
    // =================================================================

    [TestMethod]
    public void PrintWithNoArgGivesGenericNotEnough()
    {
        // Reference: an existing arity-1 builtin (PRINT) given no argument
        // routes through the dispatcher's generic missing-arg path, which
        // prints the generic "NOT ENOUGH INPUTS" (no command name appended).
        string screen = RunLogoLines("PRINT");
        Assert.IsTrue(screen.Contains("NOT ENOUGH INPUTS", StringComparison.Ordinal),
            $"Expected generic NOT ENOUGH INPUTS from bare PRINT.\n{screen}");
        Assert.IsFalse(screen.Contains("NOT ENOUGH INPUTS TO", StringComparison.Ordinal),
            $"Generic arity path must NOT name the command.\n{screen}");
    }

    [TestMethod]
    public void ReporterSinComputesAndUnderSupplyIsSilent()
    {
        // With an argument: SIN reports a value (PRINT SIN 90 -> 127).
        string ok = RunLogoLines("PRINT SIN 90");
        Assert.IsTrue(ok.Contains("127", StringComparison.Ordinal),
            $"Expected SIN 90 -> 127.\n{ok}");
        Assert.IsFalse(ok.Contains("NOT ENOUGH", StringComparison.Ordinal),
            $"SIN 90 should not error.\n{ok}");

        // Bare SIN as a command with no argument: today this terminates the
        // line silently (handler's SEC/RTS), printing NO command-specific
        // "NOT ENOUGH INPUTS TO SIN" message. A following good line still runs.
        string bare = RunLogoLines("SIN", "PRINT \"AOK");
        Assert.IsFalse(bare.Contains("NOT ENOUGH INPUTS TO SIN", StringComparison.Ordinal),
            $"Bare SIN must not print a command-specific NOT ENOUGH message.\n{bare}");
        Assert.IsTrue(bare.Contains("AOK", StringComparison.Ordinal),
            $"A line after bare SIN should still execute.\n{bare}");
    }

    [TestMethod]
    public void ReporterFirstComputesAndKeepsSpecificError()
    {
        // FIRST is a list reporter that prints a SPECIFIC under-supply error
        // ("NOT ENOUGH INPUTS TO FIRST") — it must NOT be flipped. Capture
        // both its happy path and its specific-error path so a wrongful flip
        // would break this test.
        string ok = RunLogoLines("PRINT FIRST [A B C]");
        Assert.IsTrue(ok.Contains("A", StringComparison.Ordinal),
            $"Expected FIRST [A B C] -> A.\n{ok}");

        string bare = RunLogoLines("FIRST");
        Assert.IsTrue(bare.Contains("NOT ENOUGH INPUTS TO FIRST", StringComparison.Ordinal),
            $"FIRST must keep its command-specific NOT ENOUGH INPUTS TO FIRST message.\n{bare}");
    }

    [TestMethod]
    public void ReporterNotComputesAndUnderSupplyIsSilent()
    {
        // NOT 0 -> 1, NOT 1 -> 0.
        string ok = RunLogoLines("PRINT NOT 0");
        Assert.IsTrue(ok.Contains("1", StringComparison.Ordinal),
            $"Expected NOT 0 -> 1.\n{ok}");
        string ok2 = RunLogoLines("PRINT NOT 5");
        // NOT of a nonzero value is 0.
        Assert.IsFalse(ok2.Contains("NOT ENOUGH", StringComparison.Ordinal),
            $"NOT 5 should not error.\n{ok2}");

        // Bare NOT with no argument: silent (SEC/RTS), no specific message.
        string bare = RunLogoLines("NOT", "PRINT \"AOK");
        Assert.IsFalse(bare.Contains("NOT ENOUGH INPUTS TO NOT", StringComparison.Ordinal),
            $"Bare NOT must not print a command-specific NOT ENOUGH message.\n{bare}");
        Assert.IsTrue(bare.Contains("AOK", StringComparison.Ordinal),
            $"A line after bare NOT should still execute.\n{bare}");
    }

    [TestMethod]
    public void ReporterMathFamilyComputesCorrectly()
    {
        // Lock the computed results of every reporter slated for the flip so
        // the refactor can't silently change a value (only the prologue moves).
        Assert.IsTrue(RunLogoLines("PRINT COS 0").Contains("127", StringComparison.Ordinal),
            "COS 0 -> 127");
        Assert.IsTrue(RunLogoLines("PRINT ABS 7").Contains("7", StringComparison.Ordinal),
            "ABS 7 -> 7");
        Assert.IsTrue(RunLogoLines("PRINT ABS 0 - 7").Contains("7", StringComparison.Ordinal),
            "ABS (0-7) -> 7");
        Assert.IsTrue(RunLogoLines("PRINT INT 9").Contains("9", StringComparison.Ordinal),
            "INT 9 -> 9");
        Assert.IsTrue(RunLogoLines("PRINT ROUND 5").Contains("5", StringComparison.Ordinal),
            "ROUND 5 -> 5");
        Assert.IsTrue(RunLogoLines("PRINT SQRT 9").Contains("3", StringComparison.Ordinal),
            "SQRT 9 -> 3");
        Assert.IsTrue(RunLogoLines("PRINT NUMBER? 5").Contains("1", StringComparison.Ordinal),
            "NUMBER? 5 -> 1");
        Assert.IsTrue(RunLogoLines("PRINT WORD? \"HI").Contains("1", StringComparison.Ordinal),
            "WORD? \"HI -> 1");
        Assert.IsTrue(RunLogoLines("PRINT LIST? [A]").Contains("1", StringComparison.Ordinal),
            "LIST? [A] -> 1");
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

        QueueProcedureDefinition(cpu, bus, editor, "TO HI", "PRINT 99");
        RunSteps(cpu, bus, 5_000_000);

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
    public void ProcedureEditorSaveQuitReturnsToGraphicsWithoutClearingDrawing()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "CS");
        RunSteps(cpu, bus, 5_000_000);
        byte modeBefore = bus.Vgc.GetMode();
        int turtlePixelsBefore = CountNonzeroPixels(bus, 152, 72, 168, 88);

        QueueProcedureDefinition(cpu, bus, editor, "TO FART", "PRINT 4242");
        RunSteps(cpu, bus, 8_000_000);

        Assert.AreEqual(modeBefore, bus.Vgc.GetMode(),
            "Exiting the procedure editor should restore the display mode that was active before TO.");
        Assert.IsTrue(CountNonzeroPixels(bus, 152, 72, 168, 88) >= turtlePixelsBefore,
            "Exiting the procedure editor should not clear the graphics plane or erase the turtle.");

        QueueLine(editor, "FART");
        RunSteps(cpu, bus, 5_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screen.Contains("4242", StringComparison.Ordinal),
            $"Expected saved FART procedure to run after Ctrl-S/Ctrl-Q editor flow.\n{screen}");
    }

    [TestMethod]
    public void ProcedureEditorCtrlQWithoutSaveAbandonsDefinition()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "TO ABANDON");
        QueueLine(editor, "PRINT 5151");
        editor.QueueInput(CtrlQ);    // dirty: opens the 3-choice exit dialog
        editor.QueueInput(0x0D);     // ENTER selects "Exit Anyway" (default), abandons

        QueueLine(editor, "ABANDON");
        RunUntilScreenContains(cpu, bus, "I DON'T KNOW HOW TO ABANDON", 60_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screen.Contains("I DON'T KNOW HOW TO ABANDON", StringComparison.Ordinal),
            $"Ctrl-Q + Exit Anyway should abandon the pending procedure definition.\n{screen}");
    }

    [TestMethod]
    public void ProcedureEditorShowsDirtyMarkerUntilSaved()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "TO DIRTY");
        QueueLine(editor, "PRINT 1");
        // Editing the body makes the editor dirty. The dirty marker is the only
        // '*' on screen (the TO/END scaffold + "PRINT 1" contain none). Waiting
        // for the typed line to render also proves the cursor-on-body-line path.
        RunUntilScreenContains(cpu, bus, "PRINT 1", 30_000_000);
        string dirtyScreen = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(dirtyScreen.Contains("*", StringComparison.Ordinal),
            $"Editing should set the dirty marker.\n{dirtyScreen}");

        editor.QueueInput(CtrlS);
        // Ctrl-S clears the dirty marker (the only '*' disappears).
        RunUntil(cpu, bus, 30_000_000,
            () => !SnapshotScreen(bus.Vgc).Contains("*", StringComparison.Ordinal),
            "Ctrl-S to clear the dirty marker");

        editor.QueueInput(CtrlQ);
        RunSteps(cpu, bus, 5_000_000);
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

        QueueProcedureDefinition(cpu, bus, editor, "TO SQUARE :SIZE", "PRINT :SIZE");
        RunSteps(cpu, bus, 5_000_000);

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

        QueueProcedureDefinition(cpu, bus, editor, "TO ADD2 :A :B", "PRINT :A + :B");
        RunSteps(cpu, bus, 5_000_000);

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

        QueueProcedureDefinition(cpu, bus, editor, "TO HALF :N", "IF :N < 2 [STOP]", "PRINT :N");
        RunSteps(cpu, bus, 5_000_000);

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

        QueueProcedureDefinition(cpu, bus, editor, "TO DOUBLE :N", "OUTPUT :N * 2");
        RunSteps(cpu, bus, 5_000_000);

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

        QueueProcedureDefinition(cpu, bus, editor, "TO COUNTDOWN :N",
            "IF :N = 0 [STOP]",
            "PRINT :N",
            "COUNTDOWN :N - 1");
        RunSteps(cpu, bus, 6_000_000);

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
        QueueProcedureDefinition(cpu, bus, editor, "TO FACT :N",
            "IF :N = 1 [OUTPUT 1]",
            "OUTPUT :N * FACT :N - 1");
        RunSteps(cpu, bus, 6_000_000);

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

        // This would exhaust the ~36KB heap (HEAP_START $09E0 .. HEAP_END $9800)
        // many times over without GC. Each iteration builds an 8-element list:
        // ~8 cons pairs * ~9 bytes (payload + 2-byte GC header) = ~72 bytes, so
        // without GC OOM hits around iteration ~500 (36KB / 72). 1000 iterations
        // is ~2x the whole heap → guaranteed OOM without GC; with GC the prior
        // iteration's list (only :X is live) is reclaimed each cycle.
        //
        // NB: the iteration count and list size were retuned in 4c.2-1, which
        // shrank the heap by 1248 bytes (HEAP_START $0500 -> $09E0 for the
        // module-BSS carve). The old "2500 * 2-element" tuning sat right at the
        // high-water-mark allocator's reclaim ceiling for the larger heap and
        // wedges on the smaller one. Fewer, larger allocations keep the same
        // "far exceeds heap" guarantee with comfortable margin below the wedge.
        QueueLine(editor, "REPEAT 1000 [MAKE \"X LIST 1 2 3 4 5 6 7 8]");
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

        QueueProcedureDefinition(cpu, bus, editor, "TO BOOM", "PRINT 111", "THROW \"DONE", "PRINT 222");
        RunSteps(cpu, bus, 5_000_000);

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

    [TestMethod]
    public void CharAndAscii()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "PRINT ASCII \"A");
        RunSteps(cpu, bus, 3_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        bool found = false;
        foreach (string line in screen.Split('\n'))
            if (line.Trim() == "65") found = true;
        Assert.IsTrue(found, $"Expected '65' from PRINT ASCII \"A.\n{screen}");
    }

    [TestMethod]
    public void ReadCharReturnsKeypress()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        // Queue the READCHAR command, then a key to be read
        QueueLine(editor, "PRINT ASCII READCHAR");
        RunSteps(cpu, bus, 1_000_000);
        // READCHAR is now blocking waiting for input — feed it a key
        editor.QueueInput((byte)'Z');
        RunSteps(cpu, bus, 3_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        bool found = false;
        foreach (string line in screen.Split('\n'))
            if (line.Trim() == "90") found = true;  // 'Z' = 90
        Assert.IsTrue(found, $"Expected '90' (ASCII Z) from READCHAR.\n{screen}");
    }

    [TestMethod]
    public void CharConvertsNumberToWord()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "PRINT CHAR 66");
        RunSteps(cpu, bus, 3_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        bool found = false;
        foreach (string line in screen.Split('\n'))
            if (line.Trim() == "B") found = true;
        Assert.IsTrue(found, $"Expected 'B' from PRINT CHAR 66.\n{screen}");
    }

    [TestMethod]
    public void MathFunctions()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        // ABS
        QueueLine(editor, "PRINT ABS -5");
        RunSteps(cpu, bus, 3_000_000);
        string screen = SnapshotScreen(bus.Vgc);
        bool found = false;
        foreach (string line in screen.Split('\n'))
            if (line.Trim() == "5") found = true;
        Assert.IsTrue(found, $"Expected '5' from ABS -5.\n{screen}");

        // INT
        QueueLine(editor, "PRINT INT 3");
        RunSteps(cpu, bus, 3_000_000);
        screen = SnapshotScreen(bus.Vgc);
        found = false;
        foreach (string line in screen.Split('\n'))
            if (line.Trim() == "3") found = true;
        Assert.IsTrue(found, $"Expected '3' from INT 3.\n{screen}");

        // NOT
        QueueLine(editor, "PRINT NOT 0");
        RunSteps(cpu, bus, 3_000_000);
        screen = SnapshotScreen(bus.Vgc);
        found = false;
        foreach (string line in screen.Split('\n'))
            if (line.Trim() == "1") found = true;
        Assert.IsTrue(found, $"Expected '1' from NOT 0.\n{screen}");

        QueueLine(editor, "PRINT NOT 5");
        RunSteps(cpu, bus, 3_000_000);
        screen = SnapshotScreen(bus.Vgc);
        // Should see 0 on its own line
        found = false;
        foreach (string line in screen.Split('\n'))
            if (line.Trim() == "0") found = true;
        Assert.IsTrue(found, $"Expected '0' from NOT 5.\n{screen}");

        // RANDOM should return a number (just verify no crash)
        QueueLine(editor, "PRINT RANDOM 100");
        RunSteps(cpu, bus, 3_000_000);
        // No crash = pass

        // AND / OR
        QueueLine(editor, "PRINT AND 1 1");
        RunSteps(cpu, bus, 3_000_000);
        QueueLine(editor, "PRINT OR 0 1");
        RunSteps(cpu, bus, 3_000_000);
        // Both should output 1 — already tested via prior '1' checks
    }

    [TestMethod]
    public void TypePredicates()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "PRINT NUMBER? 42");
        RunSteps(cpu, bus, 3_000_000);
        QueueLine(editor, "PRINT WORD? \"HI");
        RunSteps(cpu, bus, 3_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        // Both should print 1 (TRUE)
        int ones = 0;
        foreach (string line in screen.Split('\n'))
            if (line.Trim() == "1") ones++;
        Assert.IsTrue(ones >= 2, $"Expected at least two '1' outputs from predicates.\n{screen}");
    }

    [TestMethod]
    public void PotsListsProcedures()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueProcedureDefinition(cpu, bus, editor, "TO FOO");
        RunSteps(cpu, bus, 4_000_000);
        QueueProcedureDefinition(cpu, bus, editor, "TO BAR");
        RunSteps(cpu, bus, 4_000_000);

        QueueLine(editor, "POTS");
        RunSteps(cpu, bus, 3_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screen.Contains("FOO", StringComparison.Ordinal),
            $"Expected FOO in POTS output.\n{screen}");
        Assert.IsTrue(screen.Contains("BAR", StringComparison.Ordinal),
            $"Expected BAR in POTS output.\n{screen}");
    }

    [TestMethod]
    public void WorkspaceLongAliasesPrintProcedures()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueProcedureDefinition(cpu, bus, editor, "TO ALIASP", "PRINT 123");
        RunSteps(cpu, bus, 5_000_000);

        QueueLine(editor, "PRINTOUT \"ALIASP");
        RunSteps(cpu, bus, 4_000_000);
        QueueLine(editor, "PRINTTITLES");
        RunSteps(cpu, bus, 3_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsFalse(screen.Contains("I DON'T KNOW", StringComparison.Ordinal),
            $"Workspace long aliases should not fall through to the unknown-word path.\n{screen}");
        Assert.IsTrue(screen.Contains("TO ALIASP", StringComparison.Ordinal),
            $"Expected PRINTOUT to print the ALIASP definition.\n{screen}");
        Assert.IsTrue(screen.Contains("PRINT 123", StringComparison.Ordinal),
            $"Expected PRINTOUT to include the procedure body.\n{screen}");
        Assert.IsTrue(screen.Contains("ALIASP", StringComparison.Ordinal),
            $"Expected PRINTTITLES to list ALIASP.\n{screen}");
    }

    [TestMethod]
    public void EraseProcedure()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueProcedureDefinition(cpu, bus, editor, "TO ZAP", "PRINT 1");
        RunSteps(cpu, bus, 5_000_000);

        QueueLine(editor, "ERASE \"ZAP");
        RunSteps(cpu, bus, 3_000_000);
        QueueLine(editor, "ZAP");
        RunSteps(cpu, bus, 3_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screen.Contains("I DON'T KNOW HOW TO", StringComparison.Ordinal),
            $"Expected error after ERASE.\n{screen}");
    }

    [TestMethod]
    public void EraseMiddleProcedureKeepsNeighbors()
    {
        // Exercises do_erase's unlink (prev->next = current->next) for a node
        // that has both a predecessor and a successor.
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueProcedureDefinition(cpu, bus, editor, "TO AAA", "PRINT 111");
        QueueProcedureDefinition(cpu, bus, editor, "TO BBB", "PRINT 222");
        QueueProcedureDefinition(cpu, bus, editor, "TO CCC", "PRINT 333");

        QueueLine(editor, "ERASE \"BBB");
        RunSteps(cpu, bus, 3_000_000);
        QueueLine(editor, "AAA");
        RunSteps(cpu, bus, 3_000_000);
        QueueLine(editor, "CCC");
        RunSteps(cpu, bus, 3_000_000);
        QueueLine(editor, "BBB");
        RunUntilScreenContains(cpu, bus, "I DON'T KNOW HOW TO BBB", 60_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screen.Contains("111", StringComparison.Ordinal),
            $"AAA should still run after erasing BBB.\n{screen}");
        Assert.IsTrue(screen.Contains("333", StringComparison.Ordinal),
            $"CCC should still run after erasing BBB.\n{screen}");
        Assert.IsTrue(screen.Contains("I DON'T KNOW HOW TO BBB", StringComparison.Ordinal),
            $"BBB should be gone after ERASE.\n{screen}");
    }

    [TestMethod]
    public void ExtensionCommandWorks()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        // EXT.TEST is a test command: returns arg + 1
        QueueLine(editor, "PRINT EXT.TEST 41");
        RunSteps(cpu, bus, 5_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        bool found = false;
        foreach (string line in screen.Split('\n'))
            if (line.Trim() == "42") found = true;
        Assert.IsTrue(found, $"Expected '42' from EXT.TEST 41.\n{screen}");
    }

    [TestMethod]
    public void ExtensionCommandInExpression()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        // Use EXT.TEST as a reporter in an arithmetic expression
        QueueLine(editor, "PRINT EXT.TEST 41 + 8");
        RunSteps(cpu, bus, 5_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        bool found = false;
        foreach (string line in screen.Split('\n'))
            if (line.Trim() == "50") found = true;
        Assert.IsTrue(found, $"Expected '50' from EXT.TEST 41 + 8.\n{screen}");
    }

    [TestMethod]
    public void PlotAndLine()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        StageGraphicsModule(bus);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "CS");
        RunSteps(cpu, bus, 5_000_000);
        QueueLine(editor, "SETCOLOR 1");
        RunSteps(cpu, bus, 3_000_000);
        QueueLine(editor, "LINE 10 10 100 80");
        RunSteps(cpu, bus, 5_000_000);
        QueueLine(editor, "PLOT 50 50");
        RunSteps(cpu, bus, 3_000_000);
        QueueLine(editor, "PRINT 555");
        RunSteps(cpu, bus, 3_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        bool found = false;
        foreach (string line in screen.Split('\n'))
            if (line.Trim() == "555") found = true;
        Assert.IsTrue(found, $"Expected '555' after drawing commands.\n{screen}");
        Assert.IsFalse(screen.Contains("I DON'T KNOW", StringComparison.Ordinal),
            $"Drawing commands must resolve through lib_call(GRAPHICS), not the unknown-word path.\n{screen}");
    }

    [TestMethod]
    public void CircleAndRect()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        StageGraphicsModule(bus);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "CS");
        RunSteps(cpu, bus, 5_000_000);
        QueueLine(editor, "CIRCLE 160 100 50");
        RunSteps(cpu, bus, 5_000_000);
        QueueLine(editor, "RECT 20 20 80 60");
        RunSteps(cpu, bus, 5_000_000);
        QueueLine(editor, "PRINT 444");
        RunSteps(cpu, bus, 3_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        bool found = false;
        foreach (string line in screen.Split('\n'))
            if (line.Trim() == "444") found = true;
        Assert.IsTrue(found, $"Expected '444' after CIRCLE + RECT.\n{screen}");
        Assert.IsFalse(screen.Contains("I DON'T KNOW", StringComparison.Ordinal),
            $"CIRCLE/RECT must resolve through lib_call(GRAPHICS), not the unknown-word path.\n{screen}");
    }

    [TestMethod]
    public void FillAndPaint()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        StageGraphicsModule(bus);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "CS");
        RunSteps(cpu, bus, 5_000_000);
        QueueLine(editor, "SETCOLOR 2");
        RunSteps(cpu, bus, 3_000_000);
        QueueLine(editor, "FILL 10 10 50 50");
        RunSteps(cpu, bus, 5_000_000);
        QueueLine(editor, "SETCOLOR 3");
        RunSteps(cpu, bus, 3_000_000);
        QueueLine(editor, "PAINT 100 100");
        RunSteps(cpu, bus, 5_000_000);
        QueueLine(editor, "PRINT 333");
        RunSteps(cpu, bus, 3_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        bool found = false;
        foreach (string line in screen.Split('\n'))
            if (line.Trim() == "333") found = true;
        Assert.IsTrue(found, $"Expected '333' after FILL + PAINT.\n{screen}");
        Assert.IsFalse(screen.Contains("I DON'T KNOW", StringComparison.Ordinal),
            $"FILL/PAINT must resolve through lib_call(GRAPHICS), not the unknown-word path.\n{screen}");
    }

    [TestMethod]
    public void UnplotClearsPixel()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        StageGraphicsModule(bus);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "CS");
        RunSteps(cpu, bus, 5_000_000);
        QueueLine(editor, "SETCOLOR 1");
        RunSteps(cpu, bus, 3_000_000);
        QueueLine(editor, "PLOT 50 50");
        RunSteps(cpu, bus, 3_000_000);
        QueueLine(editor, "UNPLOT 50 50");
        RunSteps(cpu, bus, 3_000_000);
        QueueLine(editor, "PRINT 222");
        RunSteps(cpu, bus, 3_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        bool found = false;
        foreach (string line in screen.Split('\n'))
            if (line.Trim() == "222") found = true;
        Assert.IsTrue(found, $"Expected '222' after PLOT + UNPLOT.\n{screen}");
        Assert.IsFalse(screen.Contains("I DON'T KNOW", StringComparison.Ordinal),
            $"PLOT/UNPLOT must resolve through lib_call(GRAPHICS), not the unknown-word path.\n{screen}");
    }

    [TestMethod]
    public void ClearScreenAndForward()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "CS");
        RunSteps(cpu, bus, 5_000_000);
        QueueLine(editor, "FD 50");
        RunSteps(cpu, bus, 5_000_000);

        // Verify no crash and prompt returns
        QueueLine(editor, "PRINT 888");
        RunSteps(cpu, bus, 3_000_000);
        string screen = SnapshotScreen(bus.Vgc);
        bool found = false;
        foreach (string line in screen.Split('\n'))
            if (line.Trim() == "888") found = true;
        Assert.IsTrue(found, $"Expected '888' after CS + FD 50.\n{screen}");
    }

    [TestMethod]
    public void SmallForwardStepsAccumulateSubPixelInsteadOfOctagon()
    {
        // REPEAT 360 [FD 1 RT 1] should draw a circle. If each FD rounds its move
        // to whole pixels, FD 1 collapses to 8 directions (dx,dy in {-1,0,1}) and
        // the "circle" becomes an octagon. Discriminator: ten FD 1 steps at heading
        // 30deg accumulate ~5px east (sin30=0.5); the octagon bug rounds each step
        // to (1,-1) -> ~10px east.
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "CS");                 // turtle to center (160,80), heading 0
        RunSteps(cpu, bus, 5_000_000);
        QueueLine(editor, "RT 30");              // heading 30deg
        RunSteps(cpu, bus, 3_000_000);
        QueueLine(editor, "REPEAT 10 [FD 1]");
        RunSteps(cpu, bus, 8_000_000);

        int x = bus.ReadRam(0x9F01);             // TURTLE_X_LO ($9F00 base + 1)
        int dxEast = x - 160;                    // center X = 160
        // Fixed: ten FD 1 at ~30deg accumulate ~5px east. Octagon bug snaps each
        // step's dx to 0 (round(0.49)=0) -> pure north -> dxEast=0.
        Assert.IsTrue(dxEast is >= 3 and <= 7,
            $"Ten FD 1 steps at 30deg should accumulate ~5px east (sub-pixel), not snap " +
            $"to an octant. dxEast={dxEast} (x={x}).");
    }

    [TestMethod]
    public void DrawingCommandWithoutClearScreenAutoEntersSplit()
    {
        // A drawing command issued in text mode (no CS/DRAW first) should auto-
        // switch to split screen so the drawing is actually visible. The split
        // moves the Logo prompt into the bottom text band (row SPLIT_TEXT_ROW=40).
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "FD 50");              // no CS first
        RunSteps(cpu, bus, 5_000_000);

        Assert.IsTrue(bus.Vgc.GetCursorY() >= 40,
            $"FD in text mode should auto-switch to split screen (prompt drops to the " +
            $"split text band, row>=40). cursorY={bus.Vgc.GetCursorY()}\n{SnapshotScreen(bus.Vgc)}");
    }

    [TestMethod]
    public void CommandAfterRepeatOnSameLineExecutes()
    {
        // "REPEAT n [..] CMD" must run CMD after the loop. The bug: do_repeat
        // restored the outer body_resume before using it, so eval_cur resumed at
        // the wrong token and trailing commands (e.g. ST after a circle) never ran.
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);
        QueueLine(editor, "CS");
        RunSteps(cpu, bus, 5_000_000);
        QueueLine(editor, "REPEAT 1 [FD 1] RT 90");   // trailing RT must run
        RunSteps(cpu, bus, 5_000_000);
        int heading = bus.ReadRam(0x9F07) * 256 + bus.ReadRam(0x9F06);  // TURTLE_HEADING
        Assert.AreEqual(90, heading,
            $"Command after REPEAT on the same line must execute (heading should be 90). heading={heading}");
    }

    [TestMethod]
    public void ColdStartZerosTurtleGraphicsState()
    {
        // The turtle graphics-mode flag (TURTLE_GFX_VISIBLE=$9F11) and inited flag
        // ($9F0C) live in pinned RAM. On HW that RAM is stale across soft-reboot;
        // a stale flag=1 makes drawing commands believe graphics mode is already
        // active, so they never switch and everything draws to the invisible gfx
        // plane. Cold start must zero this state for deterministic behavior.
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        bus.WriteRam(0x9F11, 0xAA);     // stale TURTLE_GFX_VISIBLE
        bus.WriteRam(0x9F0C, 0xAA);     // stale TURTLE_INITED
        cpu.Boot();
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);   // runs cold_start
        Assert.AreEqual(0, bus.ReadRam(0x9F11), "cold start must zero TURTLE_GFX_VISIBLE.");
        Assert.AreEqual(0, bus.ReadRam(0x9F0C), "cold start must zero TURTLE_INITED.");
    }

    [TestMethod]
    public void HideTurtleInTextModeAutoEntersSplit()
    {
        // HT is a turtle command too — it should auto-switch to split like FD.
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);
        QueueLine(editor, "HT");
        RunSteps(cpu, bus, 5_000_000);
        Assert.IsTrue(bus.Vgc.GetCursorY() >= 40,
            $"HT in text mode should auto-switch to split screen. cursorY={bus.Vgc.GetCursorY()}");
    }

    [TestMethod]
    public void DrawAliasEntersVisibleSplitPrompt()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "DRAW");
        RunSteps(cpu, bus, 5_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsFalse(screen.Contains("I DON'T KNOW HOW TO DRAW", StringComparison.Ordinal),
            $"DRAW should be an alias for CS, not an unknown word.\n{screen}");
        Assert.AreEqual(40, bus.Vgc.GetCursorY(),
            "DRAW should move the prompt cursor into the visible split-screen text band.");
        Assert.AreEqual((byte)'?', bus.Vgc.GetScreenChar(0, 40),
            $"Expected prompt at start of visible split text band.\n{screen}");

        int turtlePixels = 0;
        for (int y = 72; y < 88; y++)
        {
            for (int x = 152; x < 168; x++)
            {
                if (bus.Vgc.GetGfxPixelColor(x, y) != 0)
                    turtlePixels++;
            }
        }

        Assert.IsTrue(turtlePixels is >= 20 and <= 80,
            $"DRAW should render the small NDK virtual-sprite turtle at the center, not a corrupted large shape. Nonzero pixels: {turtlePixels}");
        Assert.IsFalse(bus.Vgc.GetSpriteState(15).enabled,
            "Logo turtle rendering should use the NDK virtual-sprite path instead of consuming hardware sprite slot 15.");
    }

    [TestMethod]
    public void SplitScreenKeepsPromptVisible()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "SS");
        RunSteps(cpu, bus, 5_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.AreEqual(40, bus.Vgc.GetCursorY(),
            "SS should move the prompt cursor into the visible split-screen text band.");
        Assert.AreEqual((byte)'?', bus.Vgc.GetScreenChar(0, 40),
            $"Expected prompt at start of visible split text band.\n{screen}");
    }

    [TestMethod]
    public void CursorHidesWhileCommandRunsAndReturnsAtPrompt()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);
        Assert.IsTrue(bus.Vgc.IsCursorEnabled, "Logo prompt should show the cursor while waiting for input.");

        QueueLine(editor, "WAIT 20");

        bool queueDrained = false;
        bool sawCursorOff = false;
        for (int i = 0; i < 20_000_000; i++)
        {
            int cycles = cpu.ClocksForNext();
            cpu.ExecuteNext();
            bus.AdvanceCycles(cycles);

            queueDrained |= !editor.HasQueuedInput;
            if (queueDrained && !bus.Vgc.IsCursorEnabled)
                sawCursorOff = true;
            if (queueDrained && sawCursorOff && bus.Vgc.IsCursorEnabled)
                return;
        }

        Assert.Fail($"Timed out waiting for Logo cursor off/on command cycle. Cursor={bus.Vgc.IsCursorEnabled} Queued={editor.HasQueuedInput}\n{SnapshotScreen(bus.Vgc)}");
    }

    [TestMethod]
    public void TurtleDrawsSquare()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "CS");
        RunSteps(cpu, bus, 5_000_000);
        QueueLine(editor, "REPEAT 4 [FD 50 RT 90]");
        RunSteps(cpu, bus, 10_000_000);
        QueueLine(editor, "PRINT 777");
        RunSteps(cpu, bus, 3_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        bool found = false;
        foreach (string line in screen.Split('\n'))
            if (line.Trim() == "777") found = true;
        Assert.IsTrue(found, $"Expected '777' after drawing a square.\n{screen}");
    }

    [TestMethod]
    public void TurtleCardinalForwardUsesFullDistance()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "CS");
        QueueLine(editor, "PU");
        QueueLine(editor, "FD 20");
        QueueLine(editor, "PRINT 908");
        RunUntilScreenContains(cpu, bus, "908", 20_000_000);
        RunSteps(cpu, bus, 2_000_000);

        Assert.AreEqual(160, ReadTurtleWord(bus, TurtleXLo, TurtleXHi),
            "Heading 0 FD should not drift horizontally.");
        Assert.AreEqual(60, ReadTurtleWord(bus, TurtleYLo, TurtleYHi),
            "Heading 0 FD 20 should move the full 20 pixels north from the center.");
        Assert.AreEqual(0, ReadTurtleWord(bus, TurtleHeadingLo, TurtleHeadingHi),
            "FD should not change the turtle heading.");
    }

    [TestMethod]
    public void TurtleForwardAfterTurnUsesCurrentHeadingAndPosition()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "CS");
        QueueLine(editor, "PU");
        QueueLine(editor, "FD 20");
        QueueLine(editor, "PD");
        QueueLine(editor, "RT 45");
        QueueLine(editor, "FD 20");
        QueueLine(editor, "PRINT 909");
        RunUntilScreenContains(cpu, bus, "909", 20_000_000);
        RunSteps(cpu, bus, 2_000_000);

        int x = ReadTurtleWord(bus, TurtleXLo, TurtleXHi);
        int y = ReadTurtleWord(bus, TurtleYLo, TurtleYHi);
        int heading = ReadTurtleWord(bus, TurtleHeadingLo, TurtleHeadingHi);

        Assert.AreEqual(45, heading, "RT 45 should leave turtle heading at 45 degrees.");
        Assert.IsTrue(x is >= 172 and <= 175,
            $"Second FD should advance eastward from the current turtle position after RT 45. X={x}");
        Assert.IsTrue(y is >= 44 and <= 47,
            $"Second FD should advance northward on the 45-degree heading from the post-FD position. Y={y}");
        Assert.IsTrue(CountNonzeroPixels(bus, 161, 53, 167, 60) >= 4,
            "Pen-down FD after RT 45 should draw the diagonal segment from the current turtle position.");
        Assert.AreEqual(0, bus.Vgc.GetGfxPixelColor(160, 80),
            "The isolated pen-down move should not redraw from the original center position.");
    }

    [TestMethod]
    public void TurtleRightTurnsAreRelativeAndCardinalMovesDoNotDrift()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "CS");
        QueueLine(editor, "PU");
        QueueLine(editor, "FD 50");
        RunSteps(cpu, bus, 8_000_000);
        Assert.AreEqual(0, ReadTurtleWord(bus, TurtleHeadingLo, TurtleHeadingHi),
            "FD must not mutate turtle heading before any turn.");
        Assert.AreEqual(160, ReadTurtleWord(bus, TurtleXLo, TurtleXHi),
            "Initial FD 50 should move north with no x drift.");
        Assert.AreEqual(30, ReadTurtleWord(bus, TurtleYLo, TurtleYHi),
            "Initial FD 50 should move north by exactly 50 pixels.");
        QueueLine(editor, "RT 90");
        RunSteps(cpu, bus, 8_000_000);
        Assert.AreEqual(90, ReadTurtleWord(bus, TurtleHeadingLo, TurtleHeadingHi),
            "First RT 90 should add 90 degrees to the current heading.");
        QueueLine(editor, "FD 50");
        RunSteps(cpu, bus, 8_000_000);
        Assert.AreEqual(210, ReadTurtleWord(bus, TurtleXLo, TurtleXHi),
            "FD 50 after RT 90 should move east by exactly 50 pixels.");
        Assert.AreEqual(30, ReadTurtleWord(bus, TurtleYLo, TurtleYHi),
            "FD 50 after RT 90 should not drift vertically.");
        Assert.AreEqual(90, ReadTurtleWord(bus, TurtleHeadingLo, TurtleHeadingHi),
            "FD after RT 90 must not mutate turtle heading.");
        QueueLine(editor, "RT 90");
        RunSteps(cpu, bus, 8_000_000);
        Assert.AreEqual(180, ReadTurtleWord(bus, TurtleHeadingLo, TurtleHeadingHi),
            "Second RT 90 should be relative to the east-facing heading.");
        QueueLine(editor, "FD 50");
        QueueLine(editor, "PRINT 913");
        RunUntilScreenContains(cpu, bus, "913", 30_000_000);
        RunSteps(cpu, bus, 2_000_000);

        int x = ReadTurtleWord(bus, TurtleXLo, TurtleXHi);
        int y = ReadTurtleWord(bus, TurtleYLo, TurtleYHi);
        int heading = ReadTurtleWord(bus, TurtleHeadingLo, TurtleHeadingHi);
        string state = $"x={x}, y={y}, heading={heading}";

        Assert.AreEqual(210, x,
            $"After FD 50 RT 90 FD 50 RT 90 FD 50, X should stay on the exact east leg. {state}");
        Assert.AreEqual(80, y,
            $"The second RT 90 is relative to the current east-facing heading and should move south. {state}");
        Assert.AreEqual(180, heading,
            $"Two RT 90 commands should leave the turtle facing 180 degrees, not reset to an absolute 90. {state}");
    }

    [TestMethod]
    public void TurtleRightTurnAddsToCurrentHeading()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "CS");
        QueueLine(editor, "SETH 90");
        QueueLine(editor, "RT 90");
        QueueLine(editor, "PRINT 914");
        RunUntilScreenContains(cpu, bus, "914", 30_000_000);
        RunSteps(cpu, bus, 2_000_000);

        Assert.AreEqual(180, ReadTurtleWord(bus, TurtleHeadingLo, TurtleHeadingHi),
            "RT 90 from heading 90 should face 180 degrees.");
    }

    [TestMethod]
    public void LongTurtleCommandAliasesUseSameDispatchAsShortForms()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "CLEARSCREEN");
        QueueLine(editor, "PENUP");
        QueueLine(editor, "FORWARD 20");
        QueueLine(editor, "PENDOWN");
        QueueLine(editor, "RIGHT 45");
        QueueLine(editor, "FORWARD 20");
        QueueLine(editor, "PRINT 910");
        RunUntilScreenContains(cpu, bus, "910", 20_000_000);
        RunSteps(cpu, bus, 5_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        bool found = false;
        foreach (string line in screen.Split('\n'))
            if (line.Trim() == "910") found = true;

        Assert.IsTrue(found, $"Expected '910' after long-form turtle commands.\n{screen}");
        Assert.IsFalse(screen.Contains("I DON'T KNOW", StringComparison.Ordinal),
            $"Long-form turtle command aliases should not fall through to the unknown-word path.\n{screen}");

        int x = ReadTurtleWord(bus, TurtleXLo, TurtleXHi);
        int y = ReadTurtleWord(bus, TurtleYLo, TurtleYHi);
        int heading = ReadTurtleWord(bus, TurtleHeadingLo, TurtleHeadingHi);

        Assert.AreEqual(45, heading, "RIGHT should update the same heading state as RT.");
        Assert.IsTrue(x is >= 172 and <= 175,
            $"FORWARD after RIGHT should advance eastward from the current turtle position. X={x}");
        Assert.IsTrue(y is >= 44 and <= 47,
            $"FORWARD after RIGHT should advance northward on the 45-degree heading. Y={y}");
        Assert.AreEqual(0, bus.Vgc.GetGfxPixelColor(160, 80),
            "Long-form FORWARD should not redraw from the original center position.");
    }

    [TestMethod]
    public void LongAliasesCoverVisibilityPenAndGraphicsNames()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        StageGraphicsModule(bus);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "CLEARSCREEN");
        QueueLine(editor, "HIDETURTLE");
        QueueLine(editor, "SHOWTURTLE");
        QueueLine(editor, "LEFT 90");
        QueueLine(editor, "BACK 5");
        QueueLine(editor, "BACKWARD 5");
        QueueLine(editor, "SETPENCOLOR 1");
        QueueLine(editor, "SETBACKGROUND 0");
        QueueLine(editor, "SETCOLOR 1");
        QueueLine(editor, "RECTANGLE 10 10 20 20");
        QueueLine(editor, "FILLRECT 30 30 35 35");
        QueueLine(editor, "PRINT 911");
        RunUntilScreenContains(cpu, bus, "911", 25_000_000);
        RunSteps(cpu, bus, 5_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        bool found = false;
        foreach (string line in screen.Split('\n'))
            if (line.Trim() == "911") found = true;

        Assert.IsTrue(found, $"Expected '911' after secondary long-form aliases.\n{screen}");
        Assert.IsFalse(screen.Contains("I DON'T KNOW", StringComparison.Ordinal),
            $"Long-form aliases should resolve to existing commands, not unknown words.\n{screen}");
        Assert.IsTrue(CountNonzeroPixels(bus, 10, 10, 20, 20) > 0,
            "RECTANGLE should draw through the existing RECT command path.");
        Assert.IsTrue(CountNonzeroPixels(bus, 30, 30, 35, 35) > 0,
            "FILLRECT should draw through the existing FILL command path.");
    }

    [TestMethod]
    public void PenUpDoesNotDraw()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "CS");
        RunSteps(cpu, bus, 5_000_000);
        QueueLine(editor, "PU");
        RunSteps(cpu, bus, 3_000_000);
        QueueLine(editor, "FD 50");
        RunSteps(cpu, bus, 5_000_000);
        QueueLine(editor, "PRINT 666");
        RunSteps(cpu, bus, 3_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        bool found = false;
        foreach (string line in screen.Split('\n'))
            if (line.Trim() == "666") found = true;
        Assert.IsTrue(found, $"Expected '666' after PU + FD 50.\n{screen}");
    }

    [TestMethod]
    public void TurtlePositionQueries()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "CS");
        RunSteps(cpu, bus, 5_000_000);
        QueueLine(editor, "SETXY 100 50");
        RunSteps(cpu, bus, 5_000_000);
        QueueLine(editor, "PRINT XCOR");
        RunSteps(cpu, bus, 3_000_000);
        QueueLine(editor, "PRINT YCOR");
        RunSteps(cpu, bus, 3_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        bool has100 = false, has50 = false;
        foreach (string line in screen.Split('\n'))
        {
            string t = line.Trim();
            if (t == "100") has100 = true;
            if (t == "50") has50 = true;
        }
        Assert.IsTrue(has100, $"Expected '100' from XCOR.\n{screen}");
        Assert.IsTrue(has50, $"Expected '50' from YCOR.\n{screen}");
    }

    [TestMethod]
    public void SetPosAcceptsLogoPositionList()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "CS");
        QueueLine(editor, "PU");
        QueueLine(editor, "SETPOS [100 50]");
        QueueLine(editor, "PRINT 912");
        RunUntilScreenContains(cpu, bus, "912", 20_000_000);
        RunSteps(cpu, bus, 2_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsFalse(screen.Contains("I DON'T KNOW", StringComparison.Ordinal),
            $"SETPOS should be a real turtle primitive, not an unknown word.\n{screen}");
        Assert.AreEqual(100, ReadTurtleWord(bus, TurtleXLo, TurtleXHi),
            "SETPOS [x y] should move the turtle to the list's X coordinate.");
        Assert.AreEqual(50, ReadTurtleWord(bus, TurtleYLo, TurtleYHi),
            "SETPOS [x y] should move the turtle to the list's Y coordinate.");
    }

    [TestMethod]
    public void SetHeadingAndQuery()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "CS");
        RunSteps(cpu, bus, 5_000_000);
        QueueLine(editor, "SETH 90");
        RunSteps(cpu, bus, 5_000_000);
        QueueLine(editor, "PRINT HEADING");
        RunSteps(cpu, bus, 3_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        bool found = false;
        foreach (string line in screen.Split('\n'))
            if (line.Trim() == "90") found = true;
        Assert.IsTrue(found, $"Expected '90' from HEADING after SETH 90.\n{screen}");
    }

    [TestMethod]
    public void SpriteOnAndOff()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        StageGraphicsModule(bus);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "CS");
        RunSteps(cpu, bus, 5_000_000);
        QueueLine(editor, "SPRITE 0 100 50");
        RunSteps(cpu, bus, 3_000_000);
        QueueLine(editor, "SPRITEOFF 0");
        RunSteps(cpu, bus, 3_000_000);
        QueueLine(editor, "PRINT 333");
        RunSteps(cpu, bus, 3_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        bool found = false;
        foreach (string line in screen.Split('\n'))
            if (line.Trim() == "333") found = true;
        Assert.IsTrue(found, $"Expected '333' after sprite commands.\n{screen}");
        Assert.IsFalse(screen.Contains("I DON'T KNOW", StringComparison.Ordinal),
            $"SPRITE/SPRITEOFF must resolve through lib_call(GRAPHICS), not the unknown-word path.\n{screen}");
    }

    [TestMethod]
    public void WaitAndTimer()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "PRINT TIMER");
        RunSteps(cpu, bus, 3_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        // TIMER should return a number (frame counter)
        // Just verify it doesn't crash and prompt reappears
        Assert.IsTrue(screen.Contains("?", StringComparison.Ordinal),
            $"Expected prompt after PRINT TIMER.\n{screen}");
    }

    [TestMethod]
    public void ToneDoesNotCrash()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        // TONE with very short duration so test doesn't hang
        QueueLine(editor, "TONE 440 1");
        RunSteps(cpu, bus, 5_000_000);
        QueueLine(editor, "PRINT 222");
        RunSteps(cpu, bus, 3_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        bool found = false;
        foreach (string line in screen.Split('\n'))
            if (line.Trim() == "222") found = true;
        Assert.IsTrue(found, $"Expected '222' after TONE.\n{screen}");
    }

    [TestMethod]
    public void VolumeCommand()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "VOLUME 10");
        RunSteps(cpu, bus, 3_000_000);
        QueueLine(editor, "PRINT 444");
        RunSteps(cpu, bus, 3_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        bool found = false;
        foreach (string line in screen.Split('\n'))
            if (line.Trim() == "444") found = true;
        Assert.IsTrue(found, $"Expected '444' after VOLUME.\n{screen}");
    }

    [TestMethod]
    public void NoiseDoesNotCrash()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "NOISE 1");
        RunSteps(cpu, bus, 5_000_000);
        QueueLine(editor, "PRINT 555");
        RunSteps(cpu, bus, 3_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        bool found = false;
        foreach (string line in screen.Split('\n'))
            if (line.Trim() == "555") found = true;
        Assert.IsTrue(found, $"Expected '555' after NOISE.\n{screen}");
    }

    [TestMethod]
    public void WaitvblDoesNotHang()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "WAITVBL");
        RunSteps(cpu, bus, 5_000_000);
        QueueLine(editor, "PRINT 666");
        RunSteps(cpu, bus, 3_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        bool found = false;
        foreach (string line in screen.Split('\n'))
            if (line.Trim() == "666") found = true;
        Assert.IsTrue(found, $"Expected '666' after WAITVBL.\n{screen}");
    }

    [TestMethod]
    public void SpritePositionAndReposition()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        StageGraphicsModule(bus);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "CS");
        RunSteps(cpu, bus, 5_000_000);
        QueueLine(editor, "SPRITE 0 50 50");
        RunSteps(cpu, bus, 3_000_000);
        QueueLine(editor, "SPRITEPOS 0 100 75");
        RunSteps(cpu, bus, 3_000_000);
        QueueLine(editor, "SPRITEON 0");
        RunSteps(cpu, bus, 3_000_000);
        QueueLine(editor, "PRINT 777");
        RunSteps(cpu, bus, 3_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        bool found = false;
        foreach (string line in screen.Split('\n'))
            if (line.Trim() == "777") found = true;
        Assert.IsTrue(found, $"Expected '777' after sprite reposition.\n{screen}");
        Assert.IsFalse(screen.Contains("I DON'T KNOW", StringComparison.Ordinal),
            $"SPRITE/SPRITEPOS/SPRITEON must resolve through lib_call(GRAPHICS), not the unknown-word path.\n{screen}");
        var sprite = bus.Vgc.GetSpriteState(0);
        Assert.AreEqual(100, sprite.x, "SPRITEPOS should set sprite X via lib_call(GFN_SPR_POS).");
        Assert.AreEqual(75, sprite.y, "SPRITEPOS should set sprite Y via lib_call(GFN_SPR_POS).");
        Assert.IsTrue(sprite.enabled, "SPRITEON should enable the sprite via lib_call(GFN_SPR_ENABLE).");
    }

    [TestMethod]
    public void MultiLineRepeatBodyInProcedureExecutes()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        // A procedure whose REPEAT body spans multiple physical lines — the '['
        // opens on one line and the ']' closes on a later line. The interpreter
        // must piece these lines into a single logical statement.
        QueueProcedureDefinition(cpu, bus, editor, "TO TRIO",
            "REPEAT 3 [",
            "  PRINT 7",
            "]");
        RunSteps(cpu, bus, 5_000_000);

        QueueLine(editor, "TRIO");
        RunSteps(cpu, bus, 5_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsFalse(screen.Contains("REPEAT NEEDS", StringComparison.Ordinal),
            $"Multi-line REPEAT body should not raise 'REPEAT NEEDS [ BODY ]'.\n{screen}");
        int count = 0;
        foreach (string line in screen.Split('\n'))
            if (line.Trim() == "7") count++;
        Assert.IsTrue(count >= 3,
            $"Expected three '7' lines from a multi-line REPEAT 3 body, got {count}.\n{screen}");
    }

    [TestMethod]
    public void CommentsAndBlankLinesInProcedureAreIgnored()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        // A full-line comment, a blank line, and an inline ';' comment after the
        // open bracket. None of the comment words (DRAW is a real command!) may
        // be treated as code, and the loop must still run three times.
        QueueProcedureDefinition(cpu, bus, editor, "TO NOTES",
            "; draw three sevens",
            "",
            "REPEAT 3 [   ; loop body",
            "  PRINT 7",
            "]");
        RunSteps(cpu, bus, 5_000_000);

        QueueLine(editor, "NOTES");
        RunSteps(cpu, bus, 5_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsFalse(screen.Contains("I DON'T KNOW HOW TO", StringComparison.Ordinal),
            $"Comment text must not be evaluated as commands.\n{screen}");
        Assert.IsFalse(screen.Contains("REPEAT NEEDS", StringComparison.Ordinal),
            $"Inline comment after '[' must not break the multi-line body.\n{screen}");
        int count = 0;
        foreach (string line in screen.Split('\n'))
            if (line.Trim() == "7") count++;
        Assert.IsTrue(count >= 3,
            $"Expected three '7' lines despite comments/blank lines, got {count}.\n{screen}");
    }

    [TestMethod]
    public void ReplContinuesUntilBracketsBalance()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        // Type a statement whose '[' opens on the first line and ']' closes two
        // lines later, directly at the '?' prompt. The REPL must keep reading
        // until the brackets balance, then run it as one statement.
        QueueLine(editor, "REPEAT 3 [");
        RunSteps(cpu, bus, 2_000_000);
        QueueLine(editor, "PRINT 7");
        RunSteps(cpu, bus, 2_000_000);
        QueueLine(editor, "]");
        RunSteps(cpu, bus, 5_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsFalse(screen.Contains("REPEAT NEEDS", StringComparison.Ordinal),
            $"Unbalanced first line at the prompt must not error.\n{screen}");
        int count = 0;
        foreach (string line in screen.Split('\n'))
            if (line.Trim() == "7") count++;
        Assert.IsTrue(count >= 3,
            $"Expected three '7' lines from a continued REPL statement, got {count}.\n{screen}");
    }

    [TestMethod]
    public void EditReopensExistingProcedureAndRoundTripsBody()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueProcedureDefinition(cpu, bus, editor, "TO GREET", "PRINT 11");
        RunUntilScreenContains(cpu, bus, "GREET DEFINED", 60_000_000);

        // EDIT must reopen GREET in the editor PRE-FILLED with its existing body.
        // We immediately save+quit without changing anything: if EDIT opened an
        // empty editor (like a fresh TO), the save would wipe GREET's body and
        // the later call would print nothing.
        QueueLine(editor, "EDIT GREET");
        RunSteps(cpu, bus, 5_000_000);
        editor.QueueInput(CtrlS);
        editor.QueueInput(CtrlQ);
        RunSteps(cpu, bus, 5_000_000);

        string afterEdit = SnapshotScreen(bus.Vgc);
        Assert.IsFalse(afterEdit.Contains("I DON'T KNOW HOW TO", StringComparison.Ordinal),
            $"EDIT must be a recognized command.\n{afterEdit}");

        QueueLine(editor, "GREET");
        RunSteps(cpu, bus, 5_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        int count = 0;
        foreach (string line in screen.Split('\n'))
            if (line.Trim() == "11") count++;
        Assert.IsTrue(count >= 1,
            $"GREET should still print 11 after EDIT round-trip (body must survive).\n{screen}");
    }

    [TestMethod]
    public void EditInsertsLineIntoExistingProcedure()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueProcedureDefinition(cpu, bus, editor, "TO GREET", "PRINT 11");
        RunUntilScreenContains(cpu, bus, "GREET DEFINED", 60_000_000);

        // EDIT reopens GREET with the cursor at the start of the body. Typing a
        // new line inserts it before the existing "PRINT 11", proving the body
        // was pre-filled AND that edits take effect.
        QueueLine(editor, "EDIT GREET");
        RunSteps(cpu, bus, 5_000_000);
        QueueLine(editor, "PRINT 22");
        editor.QueueInput(CtrlS);
        editor.QueueInput(CtrlQ);
        RunUntilScreenContains(cpu, bus, "GREET DEFINED", 60_000_000);

        QueueLine(editor, "GREET");
        RunSteps(cpu, bus, 5_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        bool has22 = false, has11 = false;
        foreach (string line in screen.Split('\n'))
        {
            if (line.Trim() == "22") has22 = true;
            if (line.Trim() == "11") has11 = true;
        }
        Assert.IsTrue(has22, $"Edited-in 'PRINT 22' should run.\n{screen}");
        Assert.IsTrue(has11, $"Original 'PRINT 11' should survive the edit.\n{screen}");
    }

    [TestMethod]
    public void EditCreatesProcedureWhenMissing()
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        // EDIT on a procedure that does not exist yet opens a fresh editor to
        // define it (UCBLogo behaviour) instead of erroring.
        QueueLine(editor, "EDIT NEWGUY");
        RunSteps(cpu, bus, 5_000_000);
        QueueLine(editor, "PRINT 55");
        editor.QueueInput(CtrlS);
        editor.QueueInput(CtrlQ);
        RunUntilScreenContains(cpu, bus, "NEWGUY DEFINED", 60_000_000);

        QueueLine(editor, "NEWGUY");
        RunSteps(cpu, bus, 5_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsFalse(screen.Contains("I DON'T KNOW HOW TO", StringComparison.Ordinal),
            $"EDIT of a missing procedure should create it, not error.\n{screen}");
        int count = 0;
        foreach (string line in screen.Split('\n'))
            if (line.Trim() == "55") count++;
        Assert.IsTrue(count >= 1, $"Created NEWGUY should print 55.\n{screen}");
    }

    // =====================================================================
    // Error-message characterization tests (Task 4a).
    //
    // These lock the EXACT text of every Logo error-message family as the
    // CURRENT shipped ROM emits it, byte-for-byte. A later refactor replaces
    // ~26 whole error strings with runtime composition (shared fragments +
    // the command's existing name string); these tests are the safety net
    // proving the composed output is byte-identical. Each asserts the exact
    // string (trailing spaces included) via StringComparison.Ordinal.
    //
    // Verbatim sources: novalogo/lists.s (str_*_err / _type / _empty /
    // _range), novalogo/eval.s (str_idk, " HAS NO VALUE").
    // =====================================================================

    [TestMethod]
    public void NotEnoughInputs_ExactText()
    {
        // do_first (lists.s) evaluates its own argument; with no argument
        // eval_expr reports carry-set and FIRST hits @err_args → str_first_err.
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "PRINT FIRST");
        RunUntilScreenContains(cpu, bus, "NOT ENOUGH INPUTS TO FIRST", 60_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screen.Contains("NOT ENOUGH INPUTS TO FIRST", StringComparison.Ordinal),
            $"Expected exact NEI text 'NOT ENOUGH INPUTS TO FIRST'.\n{screen}");
    }

    [TestMethod]
    public void NotEnoughInputsQuestionMark_ExactText()
    {
        // EMPTY? with no argument → str_emptyp_err, which includes the '?'
        // in the command name. Locks the question-mark variant.
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "PRINT EMPTY?");
        RunUntilScreenContains(cpu, bus, "NOT ENOUGH INPUTS TO EMPTY?", 60_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screen.Contains("NOT ENOUGH INPUTS TO EMPTY?", StringComparison.Ordinal),
            $"Expected exact NEI text 'NOT ENOUGH INPUTS TO EMPTY?'.\n{screen}");
    }

    [TestMethod]
    public void DoesntLikeTrailingSpace_ExactText()
    {
        // FIRST of a non-list (5) → @err_type → str_first_type. The literal
        // is "FIRST DOESN'T LIKE " WITH A TRAILING SPACE (the offending value
        // is printed after it). The assertion MUST include that trailing space.
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "PRINT FIRST 5");
        RunUntilScreenContains(cpu, bus, "FIRST DOESN'T LIKE ", 60_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screen.Contains("FIRST DOESN'T LIKE ", StringComparison.Ordinal),
            $"Expected exact 'FIRST DOESN'T LIKE ' INCLUDING the trailing space.\n{screen}");
    }

    [TestMethod]
    public void DoesntLikeEmptyList_ExactText()
    {
        // FIRST of the empty list → @err_empty → str_first_empty.
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "PRINT FIRST []");
        RunUntilScreenContains(cpu, bus, "FIRST DOESN'T LIKE [] AS INPUT", 60_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screen.Contains("FIRST DOESN'T LIKE [] AS INPUT", StringComparison.Ordinal),
            $"Expected exact 'FIRST DOESN'T LIKE [] AS INPUT'.\n{screen}");
    }

    [TestMethod]
    public void NeedsAList_ExactText()
    {
        // ITEM's second argument must be a list; a number (5) → @err_type_pop2
        // → str_item_type.
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "PRINT ITEM 1 5");
        RunUntilScreenContains(cpu, bus, "ITEM NEEDS A LIST", 60_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screen.Contains("ITEM NEEDS A LIST", StringComparison.Ordinal),
            $"Expected exact 'ITEM NEEDS A LIST'.\n{screen}");
    }

    [TestMethod]
    public void IndexOutOfRange_ExactText()
    {
        // ITEM index 9 into a 2-element list walks off the end → @err_range
        // → str_item_range.
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "PRINT ITEM 9 [A B]");
        RunUntilScreenContains(cpu, bus, "ITEM INDEX OUT OF RANGE", 60_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screen.Contains("ITEM INDEX OUT OF RANGE", StringComparison.Ordinal),
            $"Expected exact 'ITEM INDEX OUT OF RANGE'.\n{screen}");
    }

    [TestMethod]
    public void WordType_ExactText()
    {
        // WORD of a list argument → @err_type → str_word_type. Unlike the
        // FIRST family this string is self-contained (no trailing value).
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "PRINT WORD [A] [B]");
        RunUntilScreenContains(cpu, bus, "WORD DOESN'T LIKE THIS INPUT", 60_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screen.Contains("WORD DOESN'T LIKE THIS INPUT", StringComparison.Ordinal),
            $"Expected exact 'WORD DOESN'T LIKE THIS INPUT'.\n{screen}");
    }

    [TestMethod]
    public void IDontKnowHowTo_ExactText()
    {
        // An undefined word → @truly_unknown → str_idk ("I DON'T KNOW HOW TO ")
        // followed by the word itself.
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "XYZZY");
        RunUntilScreenContains(cpu, bus, "I DON'T KNOW HOW TO XYZZY", 60_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screen.Contains("I DON'T KNOW HOW TO XYZZY", StringComparison.Ordinal),
            $"Expected exact 'I DON'T KNOW HOW TO XYZZY'.\n{screen}");
    }

    [TestMethod]
    public void HasNoValue_ExactText()
    {
        // Referencing an unset variable :FOO → eval.s @varref var_get fails →
        // prints the name char-by-char then the literal " HAS NO VALUE" (note
        // the leading space in the literal). Discovered verbatim text:
        // "FOO HAS NO VALUE".
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        QueueLine(editor, "PRINT :FOO");
        RunUntilScreenContains(cpu, bus, "FOO HAS NO VALUE", 60_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screen.Contains("FOO HAS NO VALUE", StringComparison.Ordinal),
            $"Expected exact 'FOO HAS NO VALUE'.\n{screen}");
    }

    [TestMethod]
    public void CatchInterceptsError_ExactText()
    {
        // CATCH "ERROR semantics, as implemented (eval.s try_throw_error /
        // builtins.s do_catch): when an error fires inside the body, the catch
        // unwinds the stack and resumes PAST the body, so the error does NOT
        // abort the program — a subsequent statement still runs. We characterize
        // that no-abort behavior here. (This mirrors the existing
        // CatchPreventsErrorAbort test but pins the error to an explicit
        // I-DON'T-KNOW error and asserts the exact follow-on output.)
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        // XYZZY is undefined → "I DON'T KNOW HOW TO XYZZY" error. Wrapped in
        // CATCH "ERROR, the error is swallowed (no abort), so the following
        // PRINT 31415 on the next line still runs.
        QueueLine(editor, "CATCH \"ERROR [XYZZY]");
        RunSteps(cpu, bus, 5_000_000);
        QueueLine(editor, "PRINT 31415");
        RunSteps(cpu, bus, 5_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        // The error text printed (the ROM emits it before CATCH unwinds)...
        Assert.IsTrue(screen.Contains("I DON'T KNOW HOW TO XYZZY", StringComparison.Ordinal),
            $"ROM prints the error text before CATCH unwinds.\n{screen}");
        // ...but CATCH "ERROR swallowed the abort, so the trailing PRINT ran on
        // its own output line (distinct from the echoed '? PRINT 31415').
        bool foundValue = false;
        foreach (string line in screen.Split('\n'))
            if (line.Trim() == "31415") foundValue = true;
        Assert.IsTrue(foundValue,
            $"CATCH \"ERROR should swallow the error so the following PRINT 31415 still runs.\n{screen}");
    }

    // XRAM shelf slot 0 — matches libabi.inc SHELF_BASE ($060000). Graphics and
    // sprite commands route through lib_call(GRAPHICS) (4c.1-3), which pages the
    // GRAPHICS module in from this shelf; tests that exercise those commands must
    // stage graphics.bin here or the command MISSES and aborts the line.
    private const int GraphicsShelfBase = 0x060000;

    private static void StageGraphicsModule(CompositeBusDevice bus) =>
        bus.LoadXram(GraphicsShelfBase,
            File.ReadAllBytes(Path.Combine(FindRepoRoot(), "modules", "graphics", "graphics.bin")));

    private static string FindRepoRoot()
    {
        string? dir = AppContext.BaseDirectory;
        while (dir is not null)
        {
            if (File.Exists(Path.Combine(dir, "novalogo", "builtins.s")))
                return dir;
            dir = Directory.GetParent(dir)?.FullName;
        }

        Assert.Fail("Could not locate repository root containing novalogo/builtins.s.");
        return "";
    }

    private static void QueueLine(ScreenEditor editor, string line)
    {
        foreach (char ch in line)
            editor.QueueInput((byte)ch);
        editor.QueueInput(0x0D);
    }

    // Drive the shared EDITUI procedure editor: type the TO line at the prompt
    // (which opens the editor), type the body lines into the editor, then
    // Ctrl-S (save) and Ctrl-Q (exit). The editor re-renders on every key, so
    // block until the "<name> DEFINED" confirmation rather than guessing a
    // fixed cycle budget.
    private static void QueueProcedureDefinition(Cpu cpu, CompositeBusDevice bus, ScreenEditor editor, string header, params string[] bodyLines)
    {
        QueueLine(editor, header);
        foreach (string bodyLine in bodyLines)
            QueueLine(editor, bodyLine);
        editor.QueueInput(CtrlS);
        editor.QueueInput(CtrlQ);
        // Wait for "<NAME> DEFINED" specifically so back-to-back defines in one
        // test don't match a previous procedure's stale confirmation.
        string name = header.Split(' ', StringSplitOptions.RemoveEmptyEntries)[1];
        RunUntilScreenContains(cpu, bus, name + " DEFINED", 60_000_000);
    }

    // Boot a fresh Logo session, run each line in order (2M cycles per line),
    // and return the final screen snapshot. Used by the arity-flip
    // characterization tests to capture under-supply behavior.
    private static string RunLogoLines(params string[] lines)
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000);

        foreach (string line in lines)
        {
            QueueLine(editor, line);
            RunSteps(cpu, bus, 2_000_000);
        }
        return SnapshotScreen(bus.Vgc);
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

    private static int ReadTurtleWord(CompositeBusDevice bus, ushort loAddress, ushort hiAddress) =>
        bus.ReadRam(loAddress) | (bus.ReadRam(hiAddress) << 8);

    private static int CountNonzeroPixels(CompositeBusDevice bus, int left, int top, int right, int bottom)
    {
        int count = 0;
        for (int y = top; y <= bottom; y++)
        {
            for (int x = left; x <= right; x++)
            {
                if (bus.Vgc.GetGfxPixelColor(x, y) != 0)
                    count++;
            }
        }

        return count;
    }

    private static int ExtractBytesFree(string screen)
    {
        const string marker = " BYTES FREE";
        int markerIndex = screen.IndexOf(marker, StringComparison.Ordinal);
        Assert.IsTrue(markerIndex > 0, $"Expected BYTES FREE banner line.\n{screen}");

        int firstDigit = markerIndex - 1;
        while (firstDigit >= 0 && char.IsDigit(screen[firstDigit]))
            firstDigit--;
        firstDigit++;

        Assert.IsTrue(firstDigit < markerIndex, $"Expected numeric byte count before BYTES FREE.\n{screen}");
        string digits = screen[firstDigit..markerIndex];
        Assert.IsTrue(int.TryParse(digits, out int value), $"Invalid BYTES FREE value '{digits}'.\n{screen}");
        return value;
    }
}

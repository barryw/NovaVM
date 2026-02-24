using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using e6502.Avalonia.Hardware;
using e6502.Avalonia.Input;
using KDS.e6502;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class EhBasicTokenizationTests
{
    private const ushort InputBuffer = 0x0221;

    [TestMethod]
    public void AvaloniaRomAcceptsPrintCommand()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);

        foreach (char ch in "PRINT 1")
            editor.QueueInput((byte)ch);
        editor.QueueInput(0x0D);
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        byte[] ibuf = Enumerable.Range(0, 16)
            .Select(i => bus.Read((ushort)(InputBuffer + i)))
            .ToArray();
        string ibufHex = string.Join(" ", ibuf.Select(b => b.ToString("X2")));
        string regs = $"Oquote={bus.Read(0x0060):X2} Bpntrl={bus.Read(0x00C3):X2} Bpntrh={bus.Read(0x00C4):X2}";
        Assert.IsFalse(screen.Contains("Syntax Error", StringComparison.Ordinal), $"Unexpected syntax error.\n{regs}\nIbuffs: {ibufHex}\n{screen}");
        Assert.IsTrue(screen.Contains(" 1", StringComparison.Ordinal), $"PRINT result missing.\n{regs}\nIbuffs: {ibufHex}\n{screen}");
    }

    [TestMethod]
    public void AvaloniaRomListsLongMusicLinesWithoutCorruption()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);

        EnterLine(editor, "10 REM ================================");
        EnterLine(editor, "20 REM  BACH - INVENTION No. 8 IN F");
        EnterLine(editor, "120 MUSIC 1,\"T90I1O4L8FAF>C<F>FL16EDCDC<B-AB-AGL8FA>C<A>FCL16A>C<B->C<A>C<B->C<A>C<B->C<FAGAFAGAFAGADFEFDFEFDFEF<L8BG>D<B>FDL16GAGFEFEDCDC<B-L8A>L16DC<B>C<BAGAGFEFEDL8C>L16C<B>L8C<EF>C<E>C<DB>L4CL8CECGC>C<L16BAGAG\"");
        EnterLine(editor, "130 MUSIC 1,\"O5L16FEFEDC<B->CACA<B->ACA<A>A<L8B-GB-G>D<G>GL16FE-DE-DC<B->C<B-AL8GB->D<B->GDB-D-B-D-B-D-D<A>FDAFL16GFGB-CB-DB-EB-CB-FEFA<B>AD-ADA<B>AEDEG<A>G<B>GD-G<A>GL8FD<B->D<G>FEC<A>C<F>E-L16DFE-FDFE-FDFE-\"");
        RunUntilEditorIdle(cpu, bus, editor, 20_000_000);

        EnterLine(editor, "LIST");
        RunUntilEditorIdle(cpu, bus, editor, 20_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsFalse(screen.Contains("Syntax Error", StringComparison.Ordinal), $"Unexpected syntax error during LIST.\n{screen}");
        Assert.IsTrue(screen.Contains("120", StringComparison.Ordinal), $"Expected line number 120 not visible.\n{screen}");
        Assert.IsTrue(screen.Contains("130", StringComparison.Ordinal), $"Expected line number 130 not visible.\n{screen}");
    }

    [TestMethod]
    public void AvaloniaRomPreservesRemAndDataTextOnList()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);

        EnterLine(editor, "10 REM MODE COLOR PRINT GOTO LIST");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "20 DATA MODE,COLOR,PRINT,GOTO,LIST");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "LIST");
        RunUntilEditorIdle(cpu, bus, editor, 20_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        string progDump = DumpProgram(bus);
        Assert.IsFalse(screen.Contains("Syntax Error", StringComparison.Ordinal), $"Unexpected syntax error during LIST.\n{screen}");
        Assert.IsTrue(screen.Contains("10 REM MODE COLOR PRINT GOTO LIST", StringComparison.Ordinal), $"REM line corrupted.\n{progDump}\n{screen}");
        Assert.IsTrue(screen.Contains("20 DATA MODE,COLOR,PRINT,GOTO,LIST", StringComparison.Ordinal), $"DATA line corrupted.\n{progDump}\n{screen}");
    }

    [TestMethod]
    public void AvaloniaRomInvention8ProgramChainAndListAreStable()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);

        string path = Path.GetFullPath(
            Path.Combine(AppContext.BaseDirectory, "..", "..", "..", "..", "docs", "programs", "invention8.bas"));
        string[] lines = File.ReadAllLines(path)
            .Where(l => !string.IsNullOrWhiteSpace(l))
            .ToArray();

        int entered = 0;
        foreach (string line in lines)
        {
            EnterLine(editor, line.TrimEnd());
            RunUntilEditorIdle(cpu, bus, editor, 12_000_000);
            entered++;
        }

        // Validate the in-memory BASIC line chain.
        ushort smem = (ushort)(bus.Read(0x0079) | (bus.Read(0x007A) << 8));
        ushort p = smem;
        int count = 0;
        int prevLine = -1;
        var chainLineNos = new List<int>();
        while (true)
        {
            ushort next = (ushort)(bus.Read(p) | (bus.Read((ushort)(p + 1)) << 8));
            if (next == 0)
                break;
            int lineNo = bus.Read((ushort)(p + 2)) | (bus.Read((ushort)(p + 3)) << 8);
            if (lineNo <= prevLine)
                Assert.Fail($"Line numbers not increasing at ${p:X4}: prev={prevLine}, now={lineNo}");
            prevLine = lineNo;
            chainLineNos.Add(lineNo);
            count++;
            if (next <= p || next >= 0xA000)
                Assert.Fail($"Invalid next-line pointer ${next:X4} at ${p:X4}");
            p = next;
            if (count > 2000)
                Assert.Fail("Line chain did not terminate.");
        }

        Assert.AreEqual(entered, count, $"Entered {entered} lines but chain contains {count}. Lines: {string.Join(",", chainLineNos)}");

        EnterLine(editor, "LIST");
        RunUntilEditorIdle(cpu, bus, editor, 80_000_000);
        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsFalse(screen.Contains("Syntax Error", StringComparison.Ordinal), $"LIST produced syntax error.\n{screen}");
    }

    [TestMethod]
    public void AvaloniaRomInvention8BurstPasteChainAndListAreStable()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);

        string path = Path.GetFullPath(
            Path.Combine(AppContext.BaseDirectory, "..", "..", "..", "..", "docs", "programs", "invention8.bas"));
        string[] lines = File.ReadAllLines(path)
            .Where(l => !string.IsNullOrWhiteSpace(l))
            .ToArray();

        foreach (string line in lines)
        {
            foreach (char ch in line.TrimEnd())
                editor.QueueInput((byte)ch);
            editor.QueueInput(0x0D);
        }

        RunUntilEditorIdle(cpu, bus, editor, 220_000_000);

        ValidateProgramLineChain(bus, expectedLineCount: lines.Length);

        EnterLine(editor, "LIST");
        RunUntilEditorIdle(cpu, bus, editor, 100_000_000);
        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsFalse(screen.Contains("Syntax Error", StringComparison.Ordinal), $"LIST produced syntax error.\n{screen}");
    }

    [TestMethod]
    public void AvaloniaRomInvention8InterleavedPasteStreamChainAndListAreStable()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);

        string path = Path.GetFullPath(
            Path.Combine(AppContext.BaseDirectory, "..", "..", "..", "..", "docs", "programs", "invention8.bas"));
        string[] lines = File.ReadAllLines(path)
            .Where(l => !string.IsNullOrWhiteSpace(l))
            .ToArray();
        string text = string.Join("\n", lines) + "\n";

        QueuePastedTextInterleaved(cpu, editor, text);
        RunUntilEditorIdle(cpu, bus, editor, 260_000_000);

        ValidateProgramLineChain(bus, expectedLineCount: lines.Length);

        EnterLine(editor, "LIST");
        RunUntilEditorIdle(cpu, bus, editor, 120_000_000);
        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsFalse(screen.Contains("Syntax Error", StringComparison.Ordinal), $"LIST produced syntax error.\n{screen}");
    }

    [TestMethod]
    public void AvaloniaRomListDoesNotMisdecodeCoreTokensAsBumped()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);

        EnterLine(editor, "10 REM A MODE COLOR MUSIC COPPER XFREE PLAYING MNOTE( XPEEK(");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "20 REM NOPEN NCLOSE NLISTEN NACCEPT NSEND NRECV$( NSTATUS( NREADY( NLEN");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "LIST");
        RunUntilEditorIdle(cpu, bus, editor, 20_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screen.Contains("10 REM A MODE COLOR MUSIC COPPER XFREE PLAYING MNOTE( XPEEK(", StringComparison.Ordinal), $"Expected REM line 10 missing or truncated.\n{screen}");
        Assert.IsTrue(screen.Contains("20 REM NOPEN NCLOSE NLISTEN NACCEPT NSEND NRECV$( NSTATUS( NREADY( NLEN", StringComparison.Ordinal), $"Expected REM line 20 missing or truncated.\n{screen}");
        Assert.IsFalse(screen.Contains("BUMPED(", StringComparison.Ordinal), $"LIST misdecoded token as BUMPED.\n{screen}");
    }

    [TestMethod]
    public void AvaloniaRomListsExtendedTokensWithoutCorruption()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);

        EnterLine(editor, "10 MUSIC 1,\"CDEFG\"");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "20 IF PLAYING THEN 30");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "30 A=MNOTE(1)");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "40 COPPER CLEAR");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "50 X=XPEEK(49152)");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "60 XFREE");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "LIST");
        RunUntilEditorIdle(cpu, bus, editor, 40_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsFalse(screen.Contains("Syntax Error", StringComparison.Ordinal), $"LIST produced syntax error.\n{screen}");
        Assert.IsTrue(screen.Contains("10 MUSIC 1,\"CDEFG\"", StringComparison.Ordinal), $"MUSIC line corrupted.\n{screen}");
        Assert.IsTrue(screen.Contains("20 IF PLAYING THEN 30", StringComparison.Ordinal), $"PLAYING line corrupted.\n{screen}");
        Assert.IsTrue(screen.Contains("30 A=MNOTE(1)", StringComparison.Ordinal), $"MNOTE line corrupted.\n{screen}");
        Assert.IsTrue(screen.Contains("40 COPPER CLEAR", StringComparison.Ordinal), $"COPPER line corrupted.\n{screen}");
        Assert.IsTrue(screen.Contains("50 X=XPEEK(49152)", StringComparison.Ordinal), $"XPEEK line corrupted.\n{screen}");
        Assert.IsTrue(screen.Contains("60 XFREE", StringComparison.Ordinal), $"XFREE line corrupted.\n{screen}");
    }

    [TestMethod]
    public void AvaloniaRomCopperSubcommandsTokenizeCorrectly()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);

        EnterLine(editor, "10 COPPER ADD 0,50,BGCOL,3");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "20 COPPER ADD 0,100,MODE,1");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "30 COPPER ON");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "40 COPPER OFF");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "50 COPPER CLEAR");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "LIST");
        RunUntilEditorIdle(cpu, bus, editor, 40_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsFalse(screen.Contains("Syntax Error", StringComparison.Ordinal), $"LIST produced syntax error.\n{screen}");
        Assert.IsTrue(screen.Contains("10 COPPER ADD 0,50,BGCOL,3", StringComparison.Ordinal), $"COPPER ADD BGCOL line corrupted.\n{screen}");
        Assert.IsTrue(screen.Contains("20 COPPER ADD 0,100,MODE,1", StringComparison.Ordinal), $"COPPER ADD MODE line corrupted.\n{screen}");
        Assert.IsTrue(screen.Contains("30 COPPER ON", StringComparison.Ordinal), $"COPPER ON line corrupted.\n{screen}");
        Assert.IsTrue(screen.Contains("40 COPPER OFF", StringComparison.Ordinal), $"COPPER OFF line corrupted.\n{screen}");
        Assert.IsTrue(screen.Contains("50 COPPER CLEAR", StringComparison.Ordinal), $"COPPER CLEAR line corrupted.\n{screen}");
    }

    [TestMethod]
    public void AvaloniaRomCopperListAndUseTokenizeCorrectly()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);

        EnterLine(editor, "10 COPPER LIST 1");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "20 COPPER ADD 0,50,BGCOL,3");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "30 COPPER LIST END");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "40 COPPER USE 0");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "LIST");
        RunUntilEditorIdle(cpu, bus, editor, 40_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsFalse(screen.Contains("Syntax Error", StringComparison.Ordinal), $"LIST produced syntax error.\n{screen}");
        Assert.IsTrue(screen.Contains("10 COPPER LIST 1", StringComparison.Ordinal), $"COPPER LIST line corrupted.\n{screen}");
        Assert.IsTrue(screen.Contains("20 COPPER ADD 0,50,BGCOL,3", StringComparison.Ordinal), $"COPPER ADD line corrupted.\n{screen}");
        Assert.IsTrue(screen.Contains("30 COPPER LIST END", StringComparison.Ordinal), $"COPPER LIST END line corrupted.\n{screen}");
        Assert.IsTrue(screen.Contains("40 COPPER USE 0", StringComparison.Ordinal), $"COPPER USE line corrupted.\n{screen}");
    }

    [TestMethod]
    public void AvaloniaRomMnoteAfterColonTokenizesCorrectly()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);

        EnterLine(editor, "590 N1=MNOTE(1):N2=MNOTE(2)");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "LIST");
        RunUntilEditorIdle(cpu, bus, editor, 40_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsFalse(screen.Contains("Syntax Error", StringComparison.Ordinal),
            $"LIST produced syntax error.\n{screen}");
        Assert.IsTrue(screen.Contains("590 N1=MNOTE(1):N2=MNOTE(2)", StringComparison.Ordinal),
            $"MNOTE line corrupted.\n{screen}");
    }

    [TestMethod]
    public void AvaloniaRomInvention8RunsPast590()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);

        string path = Path.GetFullPath(
            Path.Combine(AppContext.BaseDirectory, "..", "..", "..", "..", "docs", "programs", "invention8.bas"));
        string[] allLines = File.ReadAllLines(path)
            .Where(l => !string.IsNullOrWhiteSpace(l))
            .ToArray();
        foreach (string line in allLines)
        {
            EnterLine(editor, line.TrimEnd());
            RunUntilEditorIdle(cpu, bus, editor, 12_000_000);
        }

        // Use debugger to break at LAB_SNER (syntax error handler)
        var debugger = new e6502.Avalonia.Debugging.DebuggerService(cpu, bus);
        const ushort LAB_SNER = 0xCE0E;
        debugger.AddBreakpoint(LAB_SNER, null);

        // RUN with cycle-driven device ticking to match runtime scheduling.
        EnterLine(editor, "RUN");
        bool hitBreakpoint = false;
        for (int i = 0; i < 200_000_000; i++)
        {
            int cycles = cpu.ClocksForNext();
            cpu.ExecuteNext();
            bus.AdvanceCycles(cycles);
            var state = cpu.GetState();
            if (state.Pc == LAB_SNER)
            {
                hitBreakpoint = true;
                // Dump full diagnostic state
                ushort bp = (ushort)(bus.Read(0xC3) | (bus.Read(0xC4) << 8));
                string nearBp = string.Join(" ", Enumerable.Range(-5, 16)
                    .Select(off => bus.Read((ushort)(bp + off)).ToString("X2")));

                // Read stack to find return addresses
                var stackBytes = new List<string>();
                for (int s = state.Sp + 1; s <= 0xFF; s++)
                    stackBytes.Add(bus.Read((ushort)(0x100 + s)).ToString("X2"));

                // Disassemble around caller (from stack return address)
                string disasm = "";
                if (state.Sp < 0xFD)
                {
                    ushort retLo = bus.Read((ushort)(0x101 + state.Sp));
                    ushort retHi = bus.Read((ushort)(0x102 + state.Sp));
                    ushort retAddr = (ushort)((retHi << 8) | retLo);
                    disasm = $"Return addr: ${retAddr:X4} (caller ~${retAddr - 2:X4})";
                    // Disassemble a few instructions around the return address
                    for (int d = -6; d <= 6; d += 3)
                    {
                        ushort da = (ushort)(retAddr + d);
                        var (text, _) = cpu.Disassemble(da);
                        disasm += $"\n  ${da:X4}: {text}";
                    }
                }

                string hex590 = DumpLineHex(bus, 590);
                Assert.Fail($"Syntax error breakpoint hit at cycle {i}.\n" +
                    $"PC=${state.Pc:X4} A=${state.A:X2} X=${state.X:X2} Y=${state.Y:X2} SP=${state.Sp:X2}\n" +
                    $"Bpntr=${bp:X4} near=[{nearBp}]\n" +
                    $"Stack: {string.Join(" ", stackBytes.Take(20))}\n" +
                    $"{disasm}\n" +
                    $"Line 590: {hex590}\n" +
                    SnapshotScreen(bus.Vgc));
            }
        }

        if (!hitBreakpoint)
        {
            string screen = SnapshotScreen(bus.Vgc);
            Assert.IsFalse(screen.Contains("Syntax Error", StringComparison.Ordinal),
                $"Syntax error but breakpoint not hit.\n{screen}");
        }
    }

    [TestMethod]
    public void AvaloniaRomListsPrintStringLineWithoutTruncation()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);

        EnterLine(editor, "10 PRINT \"HELLO WORLD\"");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "LIST");
        RunUntilEditorIdle(cpu, bus, editor, 20_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        string dump = DumpProgram(bus);
        Assert.IsTrue(screen.Contains("10 PRINT \"HELLO WORLD\"", StringComparison.Ordinal), $"PRINT line truncated/corrupted.\n{dump}\n{screen}");
    }

    [TestMethod]
    public void AvaloniaRomRandomTokenizationStressKeepsProgramChainStable()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);

        var rng = new Random(6502);
        var lines = new List<string>(300);
        int lineNo = 10;
        for (int i = 0; i < 300; i++)
        {
            string body = BuildRandomStressLine(rng, i);
            lines.Add($"{lineNo} {body}");
            lineNo += 10;
        }

        foreach (string line in lines)
        {
            EnterLine(editor, line);
            RunUntilEditorIdle(cpu, bus, editor, 15_000_000);
        }

        ValidateProgramLineChain(bus, expectedLineCount: lines.Count);

        EnterLine(editor, "LIST");
        RunUntilEditorIdle(cpu, bus, editor, 160_000_000);
        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsFalse(screen.Contains("Syntax Error", StringComparison.Ordinal), $"LIST produced syntax error under stress.\n{screen}");
    }

    private static void RunUntilScreenContains(Cpu cpu, CompositeBusDevice bus, string marker, int maxSteps)
    {
        for (int i = 0; i < maxSteps; i++)
        {
            int cycles = cpu.ClocksForNext();
            cpu.ExecuteNext();
            bus.AdvanceCycles(cycles);
            if ((i & 0x3FF) == 0)
            {
                string screen = SnapshotScreen(bus.Vgc);
                if (screen.Contains(marker, StringComparison.Ordinal))
                {
                    return;
                }
            }
        }

        Assert.Fail($"Timed out waiting for screen marker '{marker}'.\n{SnapshotScreen(bus.Vgc)}");
    }

    private static void RunCpuSteps(Cpu cpu, int steps)
    {
        for (int i = 0; i < steps; i++)
        {
            cpu.ExecuteNext();
        }
    }

    private static void RunUntilEditorIdle(Cpu cpu, CompositeBusDevice bus, ScreenEditor editor, int maxSteps)
    {
        // Wait for: (1) queue fully drained, then (2) cursor OFF (BASIC
        // processing CR), then (3) cursor back ON (processing complete).
        // Sticky flags ensure the OFF→ON transition is detected only AFTER
        // the queue is empty, preventing false early exits.
        bool queueDrained = false;
        bool sawCursorOff = false;

        for (int i = 0; i < maxSteps; i++)
        {
            int cycles = cpu.ClocksForNext();
            cpu.ExecuteNext();
            bus.AdvanceCycles(cycles);
            if (!editor.HasQueuedInput)
                queueDrained = true;
            if (queueDrained && !bus.Vgc.IsCursorEnabled)
                sawCursorOff = true;
            if (queueDrained && sawCursorOff && bus.Vgc.IsCursorEnabled)
                return;
        }

        string screen = SnapshotScreen(bus.Vgc);
        Assert.Fail($"Timed out waiting for BASIC input idle state. PC=${cpu.Pc:X4} Cursor={bus.Vgc.IsCursorEnabled} Queued={editor.HasQueuedInput}\n{screen}");
    }

    private static void EnterLine(ScreenEditor editor, string line)
    {
        foreach (char ch in line)
            editor.QueueInput((byte)ch);
        editor.QueueInput(0x0D);
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

    private static string DumpProgram(CompositeBusDevice bus)
    {
        ushort smem = (ushort)(bus.Read(0x0079) | (bus.Read(0x007A) << 8));
        var sb = new StringBuilder();
        sb.AppendLine($"SMEM=${smem:X4}");
        ushort p = smem;
        for (int line = 0; line < 6; line++)
        {
            byte nextLo = bus.Read(p);
            byte nextHi = bus.Read((ushort)(p + 1));
            ushort next = (ushort)(nextLo | (nextHi << 8));
            byte numLo = bus.Read((ushort)(p + 2));
            byte numHi = bus.Read((ushort)(p + 3));
            ushort num = (ushort)(numLo | (numHi << 8));
            sb.Append($"@{p:X4} next={next:X4} line={num} bytes:");
            for (int i = 0; i < 24; i++)
            {
                sb.Append(' ');
                sb.Append(bus.Read((ushort)(p + i)).ToString("X2"));
            }

            sb.AppendLine();
            if (next == 0 || next <= p || next >= 0xA000)
                break;
            p = next;
        }

        return sb.ToString();
    }

    private static void ValidateProgramLineChain(CompositeBusDevice bus, int expectedLineCount)
    {
        ushort smem = (ushort)(bus.Read(0x0079) | (bus.Read(0x007A) << 8));
        ushort p = smem;
        int count = 0;
        int prevLine = -1;
        var lineNumbers = new List<int>();

        while (true)
        {
            ushort next = (ushort)(bus.Read(p) | (bus.Read((ushort)(p + 1)) << 8));
            if (next == 0)
                break;
            int lineNo = bus.Read((ushort)(p + 2)) | (bus.Read((ushort)(p + 3)) << 8);
            if (lineNo <= prevLine)
                Assert.Fail($"Line numbers not increasing at ${p:X4}: prev={prevLine}, now={lineNo}");
            prevLine = lineNo;
            lineNumbers.Add(lineNo);
            count++;
            if (next <= p || next >= 0xA000)
                Assert.Fail($"Invalid next-line pointer ${next:X4} at ${p:X4}");
            p = next;
            if (count > 2000)
                Assert.Fail("Line chain did not terminate.");
        }

        Assert.AreEqual(expectedLineCount, count, $"Expected {expectedLineCount} lines but chain contains {count}. Line numbers: {string.Join(",", lineNumbers)}");
    }

    private static void QueuePastedTextInterleaved(Cpu cpu, ScreenEditor editor, string text)
    {
        bool quoteMode = false;
        for (int i = 0; i < text.Length; i++)
        {
            char ch = text[i];
            if (ch == '\r' || ch == '\n')
            {
                if (ch == '\n' && i > 0 && text[i - 1] == '\r')
                    continue;
                quoteMode = false;
                editor.QueueInput(0x0D);
                RunCpuSteps(cpu, 1024);
                continue;
            }

            if (ch == '\t')
                ch = ' ';

            if (ch < 0x20 || ch > 0x7E)
                continue;

            byte b = (byte)ch;
            if (b == (byte)'"')
                quoteMode = !quoteMode;
            if (!quoteMode && b >= (byte)'a' && b <= (byte)'z')
                b = (byte)(b - 0x20);

            editor.QueueInput(b);
            RunCpuSteps(cpu, 64);
        }
    }

    /// <summary>
    /// Validates the LIST fix for JSR LAB_152E_RESTORE stack corruption.
    /// The old bug: LAB_152E_RESTORE was a subroutine (PLA;STA;PLA;STA;RTS) called via JSR.
    /// JSR pushes a return address, so PLAs would pop the return address instead of saved data,
    /// and RTS would jump to the saved data as a code address — crashing the CPU.
    /// This test enters core keywords (END, PRINT, FOR, NEXT, IF, GOTO, GOSUB, RETURN)
    /// and verifies LIST produces correct output. Pre-fix, LIST would crash on the first token.
    /// </summary>
    [TestMethod]
    public void AvaloniaRomListCoreTokensRoundTripCorrectly()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);

        // Each line uses a different core token as a statement keyword.
        EnterLine(editor, "10 PRINT \"HELLO\"");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "20 FOR I=1 TO 5");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "30 NEXT I");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "40 IF A=1 THEN 10");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "50 GOTO 10");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "60 GOSUB 10");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "70 RETURN");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "80 END");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);

        EnterLine(editor, "LIST");
        RunUntilEditorIdle(cpu, bus, editor, 40_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        string dump = DumpProgram(bus);

        Assert.IsFalse(screen.Contains("Syntax Error", StringComparison.Ordinal),
            $"LIST produced syntax error.\n{dump}\n{screen}");
        Assert.IsTrue(screen.Contains("10 PRINT \"HELLO\"", StringComparison.Ordinal),
            $"PRINT not round-tripped.\n{screen}");
        Assert.IsTrue(screen.Contains("20 FOR I=1 TO 5", StringComparison.Ordinal),
            $"FOR not round-tripped.\n{screen}");
        Assert.IsTrue(screen.Contains("30 NEXT I", StringComparison.Ordinal),
            $"NEXT not round-tripped.\n{screen}");
        Assert.IsTrue(screen.Contains("40 IF A=1 THEN 10", StringComparison.Ordinal),
            $"IF/THEN not round-tripped.\n{screen}");
        Assert.IsTrue(screen.Contains("50 GOTO 10", StringComparison.Ordinal),
            $"GOTO not round-tripped.\n{screen}");
        Assert.IsTrue(screen.Contains("60 GOSUB 10", StringComparison.Ordinal),
            $"GOSUB not round-tripped.\n{screen}");
        Assert.IsTrue(screen.Contains("70 RETURN", StringComparison.Ordinal),
            $"RETURN not round-tripped.\n{screen}");
        Assert.IsTrue(screen.Contains("80 END", StringComparison.Ordinal),
            $"END not round-tripped.\n{screen}");
    }

    /// <summary>
    /// Validates that LIST correctly detokenizes a line with multiple tokens on one line.
    /// The stack corruption bug would crash after the first token, so multi-token lines
    /// are especially good at catching regressions.
    /// </summary>
    [TestMethod]
    public void AvaloniaRomListMultiTokenLineRoundTrips()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);

        EnterLine(editor, "10 FOR I=1 TO 10:PRINT I:NEXT I");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "LIST");
        RunUntilEditorIdle(cpu, bus, editor, 20_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screen.Contains("10 FOR I=1 TO 10:PRINT I:NEXT I", StringComparison.Ordinal),
            $"Multi-token line not round-tripped.\n{screen}");
    }

    /// <summary>
    /// Validates LIST output for function tokens (CHR$, LEFT$, RIGHT$, MID$, ASC, LEN, VAL, STR$).
    /// These exercise the function dispatch path through LAB_TOK_PUSHFUN and LAB_XTK_FINDDEF.
    /// </summary>
    [TestMethod]
    public void AvaloniaRomListFunctionTokensRoundTrip()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);

        EnterLine(editor, "10 A$=CHR$(65)");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "20 B$=LEFT$(A$,1)");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "30 C=ASC(A$)");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "40 D=LEN(A$)");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "50 E=VAL(\"123\")");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "60 F$=STR$(42)");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);

        EnterLine(editor, "LIST");
        RunUntilEditorIdle(cpu, bus, editor, 40_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsFalse(screen.Contains("Syntax Error", StringComparison.Ordinal),
            $"LIST produced syntax error.\n{screen}");
        Assert.IsTrue(screen.Contains("10 A$=CHR$(65)", StringComparison.Ordinal),
            $"CHR$ not round-tripped.\n{screen}");
        Assert.IsTrue(screen.Contains("20 B$=LEFT$(A$,1)", StringComparison.Ordinal),
            $"LEFT$ not round-tripped.\n{screen}");
        Assert.IsTrue(screen.Contains("30 C=ASC(A$)", StringComparison.Ordinal),
            $"ASC not round-tripped.\n{screen}");
        Assert.IsTrue(screen.Contains("40 D=LEN(A$)", StringComparison.Ordinal),
            $"LEN not round-tripped.\n{screen}");
        Assert.IsTrue(screen.Contains("50 E=VAL(\"123\")", StringComparison.Ordinal),
            $"VAL not round-tripped.\n{screen}");
        Assert.IsTrue(screen.Contains("60 F$=STR$(42)", StringComparison.Ordinal),
            $"STR$ not round-tripped.\n{screen}");
    }

    /// <summary>
    /// Targeted regression test: LIST a single END line. The old stack corruption
    /// bug would cause END (token $80, the very first entry in TAB_TOKDEF) to render
    /// as a foreign character instead of the keyword.
    /// </summary>
    [TestMethod]
    public void AvaloniaRomListEndTokenDisplaysCorrectly()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);

        EnterLine(editor, "10 END");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "LIST");
        RunUntilEditorIdle(cpu, bus, editor, 20_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screen.Contains("10 END", StringComparison.Ordinal),
            $"END token not displayed correctly.\n{screen}");
    }

    /// <summary>
    /// Diagnostic: compare line 120 hex with and without later lines.
    /// The test uses a flush line (999 REM) to force processing before checking.
    /// </summary>
    [TestMethod]
    public void AvaloniaRomInvention8RunsWithoutSyntaxError()
    {
        // Test A: Enter lines 10-120 only + flush, check line 120 hex
        string hexAlone;
        {
            using var bus = new CompositeBusDevice(enableSound: false);
            var cpu = new Cpu(bus);
            cpu.Boot();
            var editor = new ScreenEditor(bus.Vgc);
            bus.Vgc.SetScreenEditor(editor);
            RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);

            string path = Path.GetFullPath(
                Path.Combine(AppContext.BaseDirectory, "..", "..", "..", "..", "docs", "programs", "invention8.bas"));
            string[] allLines = File.ReadAllLines(path)
                .Where(l => !string.IsNullOrWhiteSpace(l))
                .ToArray();
            // Enter lines 10-120 (first 12 lines)
            foreach (string line in allLines.Take(12))
            {
                EnterLine(editor, line.TrimEnd());
                RunUntilEditorIdle(cpu, bus, editor, 12_000_000);
            }
            // Flush: enter a dummy line to force line 120 processing
            EnterLine(editor, "999 REM FLUSH");
            RunUntilEditorIdle(cpu, bus, editor, 12_000_000);
            hexAlone = GetLine120Full(bus);
        }

        // Test B: Enter ALL lines + flush, check line 120 hex
        string hexFull;
        {
            using var bus = new CompositeBusDevice(enableSound: false);
            var cpu = new Cpu(bus);
            cpu.Boot();
            var editor = new ScreenEditor(bus.Vgc);
            bus.Vgc.SetScreenEditor(editor);
            RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);

            string path = Path.GetFullPath(
                Path.Combine(AppContext.BaseDirectory, "..", "..", "..", "..", "docs", "programs", "invention8.bas"));
            string[] allLines = File.ReadAllLines(path)
                .Where(l => !string.IsNullOrWhiteSpace(l))
                .ToArray();
            foreach (string line in allLines)
            {
                EnterLine(editor, line.TrimEnd());
                RunUntilEditorIdle(cpu, bus, editor, 12_000_000);
            }
            // Flush
            EnterLine(editor, "999 REM FLUSH");
            RunUntilEditorIdle(cpu, bus, editor, 12_000_000);
            hexFull = GetLine120Full(bus);

            // Now RUN
            EnterLine(editor, "RUN");
            for (int i = 0; i < 30_000_000; i++) cpu.ExecuteNext();
            string screen = SnapshotScreen(bus.Vgc);
            bool same = hexAlone == hexFull;
            Assert.IsFalse(screen.Contains("Syntax Error", StringComparison.Ordinal),
                $"Syntax error on RUN.\nLine120 alone: {hexAlone}\nLine120 full:  {hexFull}\nSame: {same}\n{screen}");
        }
    }

    private static string? GetLine120Tail(CompositeBusDevice bus, int tailBytes)
    {
        ushort smem = (ushort)(bus.Read(0x0079) | (bus.Read(0x007A) << 8));
        ushort p = smem;
        for (int iter = 0; iter < 2000; iter++)
        {
            ushort next = (ushort)(bus.Read(p) | (bus.Read((ushort)(p + 1)) << 8));
            if (next == 0) return null;
            int lineNum = bus.Read((ushort)(p + 2)) | (bus.Read((ushort)(p + 3)) << 8);
            if (lineNum == 120)
            {
                int len = next - p - 4;
                int start = Math.Max(0, len - tailBytes);
                var sb = new StringBuilder();
                for (int i = start; i < len; i++)
                    sb.Append($" {bus.Read((ushort)(p + 4 + i)):X2}");
                return sb.ToString().TrimStart();
            }
            p = next;
        }
        return null;
    }

    private static string GetLine120Full(CompositeBusDevice bus)
    {
        ushort smem = (ushort)(bus.Read(0x0079) | (bus.Read(0x007A) << 8));
        ushort p = smem;
        for (int iter = 0; iter < 2000; iter++)
        {
            ushort next = (ushort)(bus.Read(p) | (bus.Read((ushort)(p + 1)) << 8));
            if (next == 0) return "Line 120 not found";
            int lineNum = bus.Read((ushort)(p + 2)) | (bus.Read((ushort)(p + 3)) << 8);
            if (lineNum == 120)
            {
                var sb = new StringBuilder();
                sb.Append($"@{p:X4} next={next:X4} len={next - p}: ");
                for (ushort a = (ushort)(p + 4); a < next; a++)
                    sb.Append($"{bus.Read(a):X2} ");
                return sb.ToString();
            }
            p = next;
        }
        return "Line 120 not found";
    }

    /// <summary>
    /// Reproduction test: user reports PRINT CHR$(12) gives syntax error.
    /// </summary>
    [TestMethod]
    public void AvaloniaRomPrintChrDollarDoesNotSyntaxError()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);

        EnterLine(editor, "10 PRINT CHR$(12)");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);

        // Check it stored without error
        string screen1 = SnapshotScreen(bus.Vgc);
        Assert.IsFalse(screen1.Contains("Syntax Error", StringComparison.Ordinal),
            $"Syntax error on entry.\n{screen1}");

        // LIST it to verify round-trip
        EnterLine(editor, "LIST");
        RunUntilEditorIdle(cpu, bus, editor, 20_000_000);
        string screen2 = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screen2.Contains("10 PRINT CHR$(12)", StringComparison.Ordinal),
            $"PRINT CHR$ line not listed correctly.\n{screen2}");

        // RUN it to verify no syntax error at execution
        EnterLine(editor, "RUN");
        RunUntilEditorIdle(cpu, bus, editor, 20_000_000);
        string screen3 = SnapshotScreen(bus.Vgc);
        Assert.IsFalse(screen3.Contains("Syntax Error", StringComparison.Ordinal),
            $"Syntax error on RUN.\n{screen3}");
    }

    /// <summary>
    /// Validates that TOKDEF function dispatch works for multiple function tokens.
    /// The PUSHFUN fix (PLA PLA to discard JSR return) must work for all functions.
    /// </summary>
    [TestMethod]
    public void AvaloniaRomTokdefFunctionsRunWithoutSyntaxError()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);

        // Test several function tokens that use the TOKDEF preprocess/function chain
        EnterLine(editor, "10 A=ASC(\"X\")");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "20 B=LEN(\"HELLO\")");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "30 C=VAL(\"42\")");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "40 D$=CHR$(65)");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "50 E$=STR$(99)");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "60 F=ABS(-7)");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "70 PRINT A;B;C;D$;E$;F");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);

        EnterLine(editor, "RUN");
        RunUntilEditorIdle(cpu, bus, editor, 20_000_000);
        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsFalse(screen.Contains("Syntax Error", StringComparison.Ordinal),
            $"Syntax error running TOKDEF functions.\n{screen}");
        Assert.IsFalse(screen.Contains("Error", StringComparison.Ordinal),
            $"Runtime error running TOKDEF functions.\n{screen}");
    }

    /// <summary>
    /// Reproduction test: user reports line 770 END shows as foreign character
    /// after entering the full invention8 program. Tests END after many lines.
    /// </summary>
    [TestMethod]
    public void AvaloniaRomEndTokenAfterManyLinesListsCorrectly()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);

        // Enter many lines to build up program, then END at the end
        for (int i = 1; i <= 76; i++)
            EnterLine(editor, $"{i * 10} REM LINE {i}");
        EnterLine(editor, "770 END");
        RunUntilEditorIdle(cpu, bus, editor, 200_000_000);

        // LIST just line 770
        EnterLine(editor, "LIST 770");
        RunUntilEditorIdle(cpu, bus, editor, 20_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        string dump = DumpProgram(bus);
        Assert.IsTrue(screen.Contains("770 END", StringComparison.Ordinal),
            $"END on line 770 not displayed correctly.\n{dump}\n{screen}");
    }

    /// <summary>
    /// Reproduction: enter the full invention8.bas program, verify line storage
    /// and LIST output for the lines the user reported as broken.
    /// </summary>
    [TestMethod]
    public void AvaloniaRomInvention8Line60And770ListCorrectly()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);

        string path = Path.GetFullPath(
            Path.Combine(AppContext.BaseDirectory, "..", "..", "..", "..", "docs", "programs", "invention8.bas"));
        string[] lines = File.ReadAllLines(path)
            .Where(l => !string.IsNullOrWhiteSpace(l))
            .ToArray();

        foreach (string line in lines)
        {
            EnterLine(editor, line.TrimEnd());
            RunUntilEditorIdle(cpu, bus, editor, 12_000_000);
        }

        // Validate chain and check that lines 60 and 770 are stored
        var storedLines = GetStoredLineNumbers(bus);
        string chainInfo = $"Lines in chain ({storedLines.Count}): {string.Join(",", storedLines.Take(20))}...";
        Assert.IsTrue(storedLines.Contains(60), $"Line 60 missing!\n{chainInfo}");
        Assert.IsTrue(storedLines.Contains(770), $"Line 770 missing!\n{chainInfo}");

        // Dump hex of line 770 to check what's actually stored
        string line770Hex = DumpLineHex(bus, 770);

        // Dump hex of line 60 to verify tokenization
        string line60Hex = DumpLineHex(bus, 60);

        // Verify line 770 shows "END" by examining hex dump.
        // Token byte $80 at offset 4 of the line = END token (core token, bit 7 set).
        Assert.IsTrue(line770Hex.Contains("80 00"),
            $"Line 770 stored data does not contain END token ($80).\n{line770Hex}");

        // LIST via trace: enter "LIST 770" and run until output appears on screen.
        // After entering all 88 lines, stale "770 END" may still be on screen from
        // entry, so run extra cycles to let the LIST command execute fully.
        EnterLine(editor, "LIST 770");
        for (int i = 0; i < 10_000_000; i++)
            cpu.ExecuteNext();
        string screen770 = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screen770.Contains("770 END", StringComparison.Ordinal),
            $"Line 770 END not listed correctly.\n{line770Hex}\n{screen770}");

        // LIST line 60 — run enough cycles for the command to complete
        EnterLine(editor, "LIST 60");
        for (int i = 0; i < 10_000_000; i++)
            cpu.ExecuteNext();
        string screen60 = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screen60.Contains("60 PRINT CHR$(12)", StringComparison.Ordinal),
            $"Line 60 not listed correctly.\nHex: {line60Hex}\n{screen60}");
    }

    /// <summary>
    /// Enter invention8 using paste-like interleaved method (closer to GUI behavior)
    /// and verify storage and LIST output.
    /// </summary>
    [TestMethod]
    public void AvaloniaRomInvention8PastedLine60And770ListCorrectly()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);

        string path = Path.GetFullPath(
            Path.Combine(AppContext.BaseDirectory, "..", "..", "..", "..", "docs", "programs", "invention8.bas"));
        string[] lines = File.ReadAllLines(path)
            .Where(l => !string.IsNullOrWhiteSpace(l))
            .ToArray();
        string text = string.Join("\n", lines) + "\n";

        QueuePastedTextInterleaved(cpu, editor, text);
        RunUntilEditorIdle(cpu, bus, editor, 260_000_000);

        var storedLines = GetStoredLineNumbers(bus);
        Assert.IsTrue(storedLines.Contains(60),
            $"Line 60 missing after paste! Stored: {string.Join(",", storedLines)}");
        Assert.IsTrue(storedLines.Contains(770),
            $"Line 770 missing after paste! Stored: {string.Join(",", storedLines)}");
        Assert.AreEqual(lines.Length, storedLines.Count,
            $"Expected {lines.Length} lines, got {storedLines.Count}");

        // LIST specific line to avoid scrolling off screen
        EnterLine(editor, "LIST 770");
        RunUntilEditorIdle(cpu, bus, editor, 20_000_000);
        string screenFull = SnapshotScreen(bus.Vgc);
        Assert.IsTrue(screenFull.Contains("770 END", StringComparison.Ordinal),
            $"Line 770 END not listed correctly after paste.\n{screenFull}");
    }

    private static List<int> GetStoredLineNumbers(CompositeBusDevice bus)
    {
        var result = new List<int>();
        ushort smem = (ushort)(bus.Read(0x0079) | (bus.Read(0x007A) << 8));
        ushort p = smem;
        for (int iter = 0; iter < 2000; iter++)
        {
            ushort next = (ushort)(bus.Read(p) | (bus.Read((ushort)(p + 1)) << 8));
            if (next == 0) break;
            int lineNo = bus.Read((ushort)(p + 2)) | (bus.Read((ushort)(p + 3)) << 8);
            result.Add(lineNo);
            if (next <= p || next >= 0xA000) break;
            p = next;
        }
        return result;
    }


    private static string DumpLineHex(CompositeBusDevice bus, int targetLineNo)
    {
        ushort smem = (ushort)(bus.Read(0x0079) | (bus.Read(0x007A) << 8));
        ushort p = smem;
        for (int iter = 0; iter < 2000; iter++)
        {
            ushort next = (ushort)(bus.Read(p) | (bus.Read((ushort)(p + 1)) << 8));
            if (next == 0) break;
            int lineNo = bus.Read((ushort)(p + 2)) | (bus.Read((ushort)(p + 3)) << 8);
            if (lineNo == targetLineNo)
            {
                int len = Math.Min(next - p, 64);
                var sb = new StringBuilder();
                sb.Append($"Line {targetLineNo} @{p:X4}: ");
                for (int i = 0; i < len; i++)
                    sb.Append(bus.Read((ushort)(p + i)).ToString("X2") + " ");
                return sb.ToString();
            }
            if (next <= p || next >= 0xA000) break;
            p = next;
        }
        return $"Line {targetLineNo} NOT FOUND";
    }

    /// <summary>
    /// Full LIST of invention8 — verify last visible lines contain "770 END"
    /// when scrolling through the complete program listing.
    /// </summary>
    [TestMethod]
    public void AvaloniaRomInvention8FullListShows770End()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);

        string path = Path.GetFullPath(
            Path.Combine(AppContext.BaseDirectory, "..", "..", "..", "..", "docs", "programs", "invention8.bas"));
        string[] lines = File.ReadAllLines(path)
            .Where(l => !string.IsNullOrWhiteSpace(l))
            .ToArray();

        foreach (string line in lines)
        {
            EnterLine(editor, line.TrimEnd());
            RunUntilEditorIdle(cpu, bus, editor, 12_000_000);
        }

        // Full LIST — scrolls through entire program
        EnterLine(editor, "LIST");
        RunUntilEditorIdle(cpu, bus, editor, 60_000_000);
        string screen = SnapshotScreen(bus.Vgc);

        // After full LIST, "770 END" should be visible (last line before Ready)
        Console.WriteLine("=== SCREEN AFTER FULL LIST ===");
        Console.WriteLine(screen);

        Assert.IsTrue(screen.Contains("770 END", StringComparison.Ordinal),
            $"770 END not visible after full LIST.\n{screen}");
    }

    [TestMethod]
    public void AvaloniaRomEchoServerRunsWithoutSyntaxError()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);

        // Enter the full echo_server.bas program
        string path = Path.GetFullPath(
            Path.Combine(AppContext.BaseDirectory, "..", "..", "..", "..", "docs", "programs", "echo_server.bas"));
        string[] lines = File.ReadAllLines(path)
            .Where(l => !string.IsNullOrWhiteSpace(l))
            .ToArray();
        foreach (string line in lines)
        {
            EnterLine(editor, line.TrimEnd());
            RunUntilEditorIdle(cpu, bus, editor, 12_000_000);
        }

        // Dump hex of critical lines
        string hex70 = DumpLineHex(bus, 70);
        string hex100 = DumpLineHex(bus, 100);
        string hex150 = DumpLineHex(bus, 150);
        string hex160 = DumpLineHex(bus, 160);

        // LIST to verify
        EnterLine(editor, "LIST");
        RunUntilEditorIdle(cpu, bus, editor, 40_000_000);
        string listScreen = SnapshotScreen(bus.Vgc);

        Console.WriteLine($"Line 70: {hex70}");
        Console.WriteLine($"Line 100: {hex100}");
        Console.WriteLine($"Line 150: {hex150}");
        Console.WriteLine($"Line 160: {hex160}");
        Console.WriteLine("LIST output:");
        Console.WriteLine(listScreen);

        // RUN with cycle-driven device ticking to match runtime scheduling.
        EnterLine(editor, "RUN");
        for (int i = 0; i < 20_000_000; i++)
        {
            int cycles = cpu.ClocksForNext();
            cpu.ExecuteNext();
            bus.AdvanceCycles(cycles);
        }

        string screen = SnapshotScreen(bus.Vgc);
        Console.WriteLine("Screen after RUN:");
        Console.WriteLine(screen);
        Assert.IsFalse(screen.Contains("Syntax Error", StringComparison.Ordinal),
            $"Syntax error running echo_server.\nLine 70: {hex70}\nLine 100: {hex100}\nLine 150: {hex150}\nLine 160: {hex160}\nLIST:\n{listScreen}\nRUN:\n{screen}");
    }

    [TestMethod]
    public void AvaloniaRomListsNicKeywordsWithoutCorruption()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);

        EnterLine(editor, "10 NOPEN 0,\"localhost\",6502");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "20 NCLOSE 0");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "30 NLISTEN 1,8080");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "40 NACCEPT 1");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "50 NSEND 0,\"HELLO\"");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "60 A$=NRECV$(0)");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "70 S=NSTATUS(0)");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "80 R=NREADY(0)");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "90 L=NLEN");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);

        EnterLine(editor, "LIST");
        RunUntilEditorIdle(cpu, bus, editor, 40_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsFalse(screen.Contains("Syntax Error", StringComparison.Ordinal),
            $"LIST produced syntax error.\n{screen}");
        Assert.IsTrue(screen.Contains("10 NOPEN 0,\"localhost\",6502", StringComparison.Ordinal),
            $"NOPEN line corrupted.\n{screen}");
        Assert.IsTrue(screen.Contains("20 NCLOSE 0", StringComparison.Ordinal),
            $"NCLOSE line corrupted.\n{screen}");
        Assert.IsTrue(screen.Contains("30 NLISTEN 1,8080", StringComparison.Ordinal),
            $"NLISTEN line corrupted.\n{screen}");
        Assert.IsTrue(screen.Contains("40 NACCEPT 1", StringComparison.Ordinal),
            $"NACCEPT line corrupted.\n{screen}");
        Assert.IsTrue(screen.Contains("50 NSEND 0,\"HELLO\"", StringComparison.Ordinal),
            $"NSEND line corrupted.\n{screen}");
        Assert.IsTrue(screen.Contains("60 A$=NRECV$(0)", StringComparison.Ordinal),
            $"NRECV$ line corrupted.\n{screen}");
        Assert.IsTrue(screen.Contains("70 S=NSTATUS(0)", StringComparison.Ordinal),
            $"NSTATUS line corrupted.\n{screen}");
        Assert.IsTrue(screen.Contains("80 R=NREADY(0)", StringComparison.Ordinal),
            $"NREADY line corrupted.\n{screen}");
        Assert.IsTrue(screen.Contains("90 L=NLEN", StringComparison.Ordinal),
            $"NLEN line corrupted.\n{screen}");
    }

    [TestMethod]
    public void AvaloniaRomListsDmaKeywordsWithoutCorruption()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);

        EnterLine(editor, "10 DMACOPY 0,1024,1,0,64");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "20 DMAFILL 2,320,16,7");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "30 S=DMASTATUS:E=DMAERR:C=DMACOUNT");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);

        EnterLine(editor, "LIST");
        RunUntilEditorIdle(cpu, bus, editor, 40_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        string line30Hex = DumpLineHex(bus, 30);
        Assert.IsFalse(screen.Contains("Syntax Error", StringComparison.Ordinal),
            $"LIST produced syntax error.\n{screen}");
        Assert.IsTrue(screen.Contains("10 DMACOPY 0,1024,1,0,64", StringComparison.Ordinal),
            $"DMACOPY line corrupted.\n{screen}");
        Assert.IsTrue(screen.Contains("20 DMAFILL 2,320,16,7", StringComparison.Ordinal),
            $"DMAFILL line corrupted.\n{screen}");
        Assert.IsTrue(screen.Contains("30 S=DMASTATUS:E=DMAERR:C=DMACOUNT", StringComparison.Ordinal),
            $"DMA status line corrupted.\n{screen}");
        Assert.IsTrue(line30Hex.Contains("01 25", StringComparison.Ordinal), $"DMASTATUS token missing.\n{line30Hex}");
        Assert.IsTrue(line30Hex.Contains("01 27", StringComparison.Ordinal), $"DMAERR token missing.\n{line30Hex}");
        Assert.IsTrue(line30Hex.Contains("01 28", StringComparison.Ordinal), $"DMACOUNT token missing.\n{line30Hex}");
    }

    [TestMethod]
    public void AvaloniaRomDmaCommands_MoveDataInDirectMode()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);

        bus.Write(0x5000, 65);
        EnterLine(editor, "DMACOPY 0,20480,4,0,1");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        RunCpuSteps(cpu, 500_000);

        byte dmaStatus = bus.Read((ushort)VgcConstants.DmaStatus);
        byte dmaErr = bus.Read((ushort)VgcConstants.DmaErrCode);
        string screenAfterCopy = SnapshotScreen(bus.Vgc);
        Assert.AreEqual(VgcConstants.DmaStatusOk, dmaStatus,
            $"DMACOPY did not complete successfully. status={dmaStatus:X2} err={dmaErr:X2}\n{screenAfterCopy}");

        Assert.AreEqual(65, bus.Vgc.GetSpriteShape(0)[0],
            $"DMACOPY did not move byte from CPU RAM to sprite memory. status={dmaStatus:X2} err={dmaErr:X2}\n{screenAfterCopy}");

        EnterLine(editor, "DMAFILL 4,1,3,66");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        RunCpuSteps(cpu, 500_000);

        Assert.AreEqual(66, bus.Vgc.GetSpriteShape(0)[1]);
        Assert.AreEqual(66, bus.Vgc.GetSpriteShape(0)[2]);
        Assert.AreEqual(66, bus.Vgc.GetSpriteShape(0)[3]);

        // Seed DMA status/readback registers with known values and validate
        // BASIC functions return those values through expression evaluation.
        bus.Write((ushort)VgcConstants.DmaStatus, 0x5A);
        bus.Write((ushort)VgcConstants.DmaErrCode, 0xA5);
        bus.Write((ushort)VgcConstants.DmaCountL, 0x33);
        bus.Write((ushort)VgcConstants.DmaCountM, 0x00);
        bus.Write((ushort)VgcConstants.DmaCountH, 0x00);
        Assert.AreEqual(0xA5, bus.Read((ushort)VgcConstants.DmaErrCode), "Seeded DMA errcode did not stick.");

        EnterLine(editor, "POKE 5000,DMASTATUS");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "POKE 5001,DMAERR");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "POKE 5002,DMACOUNT");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);

        Assert.AreEqual(0x5A, bus.Read(5000), "DMASTATUS function did not return expected value.");
        Assert.AreEqual(0xA5, bus.Read(5001), "DMAERR function did not return expected value.");
        Assert.AreEqual(0x33, bus.Read(5002), "DMACOUNT function did not return expected value.");
    }

    [TestMethod]
    public void AvaloniaRomListsBlitterKeywordsWithoutCorruption()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);

        EnterLine(editor, "10 BLITCOPY 0,24576,4,2,0,80,3,2");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "20 BLITFILL 2,160,80,2,2,9");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        EnterLine(editor, "30 S=BLITSTATUS:E=BLITERR:C=BLITCOUNT");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);

        EnterLine(editor, "LIST");
        RunUntilEditorIdle(cpu, bus, editor, 40_000_000);

        string screen = SnapshotScreen(bus.Vgc);
        string line10Hex = DumpLineHex(bus, 10);
        string line20Hex = DumpLineHex(bus, 20);
        string line30Hex = DumpLineHex(bus, 30);
        Assert.IsFalse(screen.Contains("Syntax Error", StringComparison.Ordinal),
            $"LIST produced syntax error.\n{screen}");
        Assert.IsTrue(screen.Contains("10 BLITCOPY 0,24576,4,2,0,80,3,2", StringComparison.Ordinal),
            $"BLITCOPY line corrupted.\n{screen}");
        Assert.IsTrue(screen.Contains("20 BLITFILL 2,160,80,2,2,9", StringComparison.Ordinal),
            $"BLITFILL line corrupted.\n{screen}");
        Assert.IsTrue(screen.Contains("30 S=BLITSTATUS:E=BLITERR:C=BLITCOUNT", StringComparison.Ordinal),
            $"BLIT status line corrupted.\n{screen}");
        Assert.IsTrue(line10Hex.Contains("01 40", StringComparison.Ordinal), $"BLITCOPY token missing.\n{line10Hex}");
        Assert.IsTrue(line20Hex.Contains("01 41", StringComparison.Ordinal), $"BLITFILL token missing.\n{line20Hex}");
        Assert.IsTrue(line30Hex.Contains("01 42", StringComparison.Ordinal), $"BLITSTATUS token missing.\n{line30Hex}");
        Assert.IsTrue(line30Hex.Contains("01 43", StringComparison.Ordinal), $"BLITERR token missing.\n{line30Hex}");
        Assert.IsTrue(line30Hex.Contains("01 44", StringComparison.Ordinal), $"BLITCOUNT token missing.\n{line30Hex}");
    }

    [TestMethod]
    public void AvaloniaRomBlitterCommands_MoveDataInDirectMode()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);

        bus.Write(0x6000, 1);
        bus.Write(0x6001, 2);
        bus.Write(0x6002, 3);
        bus.Write(0x6004, 4);
        bus.Write(0x6005, 5);
        bus.Write(0x6006, 6);

        EnterLine(editor, "BLITCOPY 0,24576,4,2,0,80,3,2");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        RunCpuSteps(cpu, 500_000);

        byte bltStatus = bus.Read((ushort)VgcConstants.BltStatus);
        byte bltErr = bus.Read((ushort)VgcConstants.BltErrCode);
        string screenAfterCopy = SnapshotScreen(bus.Vgc);
        Assert.AreEqual(VgcConstants.BltStatusOk, bltStatus,
            $"BLITCOPY did not complete successfully. status={bltStatus:X2} err={bltErr:X2}\n{screenAfterCopy}");

        Assert.AreEqual(1, bus.Read((ushort)(VgcConstants.ColorRamBase + 0)));
        Assert.AreEqual(2, bus.Read((ushort)(VgcConstants.ColorRamBase + 1)));
        Assert.AreEqual(3, bus.Read((ushort)(VgcConstants.ColorRamBase + 2)));
        Assert.AreEqual(4, bus.Read((ushort)(VgcConstants.ColorRamBase + 80)));
        Assert.AreEqual(5, bus.Read((ushort)(VgcConstants.ColorRamBase + 81)));
        Assert.AreEqual(6, bus.Read((ushort)(VgcConstants.ColorRamBase + 82)));

        EnterLine(editor, "BLITFILL 2,160,80,2,2,9");
        RunUntilEditorIdle(cpu, bus, editor, 10_000_000);
        RunCpuSteps(cpu, 500_000);

        Assert.AreEqual(9, bus.Read((ushort)(VgcConstants.ColorRamBase + 160)));
        Assert.AreEqual(9, bus.Read((ushort)(VgcConstants.ColorRamBase + 161)));
        Assert.AreEqual(9, bus.Read((ushort)(VgcConstants.ColorRamBase + 240)));
        Assert.AreEqual(9, bus.Read((ushort)(VgcConstants.ColorRamBase + 241)));

    }

    private static string BuildRandomStressLine(Random rng, int i)
    {
        string[] kw = ["MODE", "COLOR", "GCLS", "GCOLOR", "LINE", "MUSIC", "PLAYING", "MNOTE(", "COPPER", "XFREE", "XPEEK(", "SIDPLAY", "SIDSTOP", "IF", "THEN", "GOTO", "FOR", "NEXT", "REM", "DATA", "NOPEN", "NCLOSE", "NLISTEN", "NACCEPT", "NSEND", "NRECV$(", "NSTATUS(", "NREADY(", "NLEN", "DMACOPY", "DMAFILL", "DMASTATUS", "DMAERR", "DMACOUNT", "BLITCOPY", "BLITFILL", "BLITSTATUS", "BLITERR", "BLITCOUNT"];
        string[] ident = ["A", "B", "C", "I", "N1", "N2", "T", "X", "Y", "P"];

        int kind = i % 6;
        return kind switch
        {
            0 => $"REM {kw[rng.Next(kw.Length)]} {kw[rng.Next(kw.Length)]} {kw[rng.Next(kw.Length)]} {rng.Next(0, 999)}",
            1 => $"DATA {kw[rng.Next(kw.Length)]},{kw[rng.Next(kw.Length)]},{rng.Next(0, 65535)}",
            2 => $"{ident[rng.Next(ident.Length)]}={rng.Next(0, 999)}:{kw[rng.Next(0, 5)]} {rng.Next(0, 15)},{rng.Next(0, 15)}",
            3 => $"IF PLAYING THEN {rng.Next(10, 3000)}",
            4 => $"{ident[rng.Next(ident.Length)]}=MNOTE({rng.Next(1, 3)})",
            _ => $"MUSIC {rng.Next(1, 3)},\"T{rng.Next(60, 140)}O{rng.Next(2, 6)}L8CDEFGAB\""
        };
    }
}

using System;
using System.Text;
using e6502.Avalonia.Hardware;
using e6502.Avalonia.Input;
using e6502.Avalonia.Rendering;
using KDS.e6502;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

/// <summary>
/// Integration-level regressions exercising EhBASIC ROM + Avalonia bus.
/// Each test entered verbatim lines the same way the integration harness
/// does, then asserts the printed screen contains expected output.
/// </summary>
[TestClass]
public class BasicRegressionTests
{
    [TestMethod]
    public void ArrayIndexedAssignAndPrintReadsBack()
    {
        string screen = RunProgram(new[]
        {
            "10 DIM A(10)",
            "20 FOR I=0 TO 10:A(I)=I*I:NEXT",
            "30 PRINT A(5)",
            "RUN"
        });

        Assert.IsTrue(screen.Contains(" 25"),
            $"A(5) should be 25 but screen shows:\n{screen}");
    }

    [TestMethod]
    public void Sid1PokeLandsInSidChip()
    {
        // Known structural limitation: BASIC PEEK($D400-$D43F) cannot return
        // SID register values because the EhBASIC ROM has live instructions
        // at those addresses (e.g., LAB_1F45 at $D3FF, LAB_1F48 at $D402).
        // Any mux that routes SID reads to the SID chip clobbers opcode
        // fetches and silently breaks array operations (see
        // ArrayIndexedAssignAndPrintReadsBack).
        //
        // So this test asserts POKE delivers the value to the SID chip
        // shadow directly, without going through BASIC PEEK.
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);

        foreach (char ch in "POKE $D400,$37")
            editor.QueueInput((byte)ch);
        editor.QueueInput(0x0D);
        RunUntilEditorIdle(cpu, bus, editor, 40_000_000);

        Assert.AreEqual(0x37, bus.Sid.Read(0xD400),
            "POKE $D400,$37 should set SID1 voice-1 freq-lo register to $37");
    }

    [TestMethod]
    public void XramStashFetchRoundtripRestoresRam()
    {
        string screen = RunProgram(new[]
        {
            "10 XALLOC 1",
            "20 POKE $2000,$42",
            "30 STASH $2000,0,1",
            "40 POKE $2000,$FF",
            "50 FETCH $2000,0,1",
            "60 PRINT PEEK($2000)",
            "RUN"
        });

        Assert.IsTrue(screen.Contains(" 66"),
            $"After STASH/FETCH roundtrip, $2000 should hold $42=66 but got:\n{screen}");
    }

    [TestMethod]
    public void XramStash256ByteRoundtripRestoresAllBytes()
    {
        // Mirrors tests/integration/xram.6502 stash-fetch-roundtrip. STASH and
        // FETCH are now routed through the shared DMA-backed XRAM runtime, so
        // this catches regressions in the BASIC / extension ROM / DMA path.
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);

        foreach (string line in new[]
        {
            "10 XALLOC 1",
            "20 FOR I=0 TO 255:POKE $2000+I,I:NEXT",
            "30 STASH $2000,0,256",
            "40 FOR I=0 TO 255:POKE $2000+I,0:NEXT",
            "50 FETCH $2000,0,256",
            "RUN",
        })
        {
            foreach (char ch in line)
                editor.QueueInput((byte)ch);
            editor.QueueInput(0x0D);
            RunUntilEditorIdle(cpu, bus, editor, 80_000_000);
        }

        for (int i = 0; i < 256; i++)
        {
            byte got = bus.Read((ushort)(0x2000 + i));
            Assert.AreEqual((byte)i, got,
                $"After STASH/FETCH 256-byte roundtrip, $2000+{i} should be {i:X2} but is {got:X2}");
        }
    }

    [TestMethod]
    public void ReverseFlashCommands_PrintExpectedAvaloniaTextAttributes()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);

        foreach (string line in new[]
        {
            "10 CLS",
            "20 COLOR 2,4",
            "30 LOCATE 0,0",
            "40 REVERSE:PRINT \"R\";",
            "50 REVERSEOFF:PRINT \"N\";",
            "60 REVERSE 1,6:PRINT \"E\";",
            "70 REVERSEOFF:FLASH:PRINT \"F\";",
            "80 FLASHOFF:PRINT \"S\";",
            "90 END",
            "RUN",
        })
        {
            foreach (char ch in line)
                editor.QueueInput((byte)ch);
            editor.QueueInput(0x0D);
            RunUntilEditorIdle(cpu, bus, editor, 80_000_000);
        }

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsFalse(screen.Contains("Syntax Error", StringComparison.Ordinal),
            $"Reverse/flash program should run without syntax errors.\n{screen}");
        Assert.IsTrue(screen.StartsWith("RNEFS", StringComparison.Ordinal),
            $"Expected styled output at the top-left of the Avalonia text layer.\n{screen}");

        AssertTextCell(bus.Vgc, 0, 'R', color: 0x24, attr: 0x00); // default reverse swaps fg/bg
        AssertTextCell(bus.Vgc, 1, 'N', color: 0x42, attr: 0x00); // normal fg/bg
        AssertTextCell(bus.Vgc, 2, 'E', color: 0x61, attr: 0x00); // explicit reverse fg/bg
        AssertTextCell(bus.Vgc, 3, 'F', color: 0x42, attr: VgcConstants.TextAttrFlash);
        AssertTextCell(bus.Vgc, 4, 'S', color: 0x42, attr: 0x00);

        Assert.AreEqual(0x00, bus.Read((ushort)VgcConstants.RegTextFlags),
            "REVERSEOFF/FLASHOFF should leave subsequent BASIC output normal.");
    }

    [TestMethod]
    public void TextReverseCommand_RendersMockInfocomStatusLineInAvaloniaEmulator()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);
        EnterProgramLines(cpu, bus, editor,
        [
            "10 CLS",
            "20 COLOR 15,0",
            "30 LOCATE 0,0",
            "40 REVERSE",
            "50 FOR I=1 TO 80:PRINT \" \";:NEXT",
            "60 LOCATE 0,0",
            "70 PRINT \"WEST OF HOUSE\";",
            "80 LOCATE 55,0",
            "90 PRINT \"SCORE: 0 MOVES: 1\";",
            "100 REVERSEOFF",
            "110 END",
            "RUN",
        ]);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsFalse(screen.Contains("Syntax Error", StringComparison.Ordinal),
            $"Mock Infocom status line program should run without syntax errors.\n{screen}");
        Assert.IsTrue(screen.StartsWith("WEST OF HOUSE", StringComparison.Ordinal),
            $"Expected status location at the top-left of the Avalonia text layer.\n{screen}");
        Assert.IsTrue(screen.Split('\n')[0].Contains("SCORE: 0 MOVES: 1", StringComparison.Ordinal),
            $"Expected score/moves field on status row 0.\n{screen}");

        AssertTextCell(bus.Vgc, 0, 'W', color: 0xF0, attr: 0x00);
        AssertTextCell(bus.Vgc, 12, 'E', color: 0xF0, attr: 0x00);
        AssertTextCell(bus.Vgc, 13, ' ', color: 0xF0, attr: 0x00);
        AssertTextCell(bus.Vgc, 55, 'S', color: 0xF0, attr: 0x00);
        AssertTextCell(bus.Vgc, 79, ' ', color: 0xF0, attr: 0x00);
        Assert.AreEqual(0x00, bus.Read((ushort)VgcConstants.RegTextFlags),
            "REVERSEOFF should leave later output normal.");
    }

    [TestMethod]
    public void TextFlashCommand_ExecutesInAvaloniaEmulator()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);
        EnterProgramLines(cpu, bus, editor,
        [
            "10 CLS",
            "20 COLOR 2,4",
            "30 LOCATE 0,0",
            "40 FLASH:PRINT \"F\";",
            "50 FLASHOFF:PRINT \"S\";",
            "60 END",
            "RUN",
        ]);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsFalse(screen.Contains("Syntax Error", StringComparison.Ordinal),
            $"Text flash program should run without syntax errors.\n{screen}");
        Assert.IsTrue(screen.StartsWith("FS", StringComparison.Ordinal),
            $"Expected text flash output at the top-left of the Avalonia text layer.\n{screen}");

        AssertTextCell(bus.Vgc, 0, 'F', color: 0x42, attr: VgcConstants.TextAttrFlash);
        AssertTextCell(bus.Vgc, 1, 'S', color: 0x42, attr: 0x00);
    }

    [TestMethod]
    public void GtextReverseCommand_ExecutesInAvaloniaEmulator()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        bus.Vgc.SetFont(SinglePixelAFont());
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);
        EnterProgramLines(cpu, bus, editor,
        [
            "10 GCLS",
            "20 GCOLOR 5",
            "30 REVERSE 2,6",
            "40 GTEXT 0,0,0,1,\"A\"",
            "50 REVERSEOFF",
            "60 END",
            "RUN",
        ]);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsFalse(screen.Contains("Syntax Error", StringComparison.Ordinal),
            $"GTEXT reverse program should run without syntax errors.\n{screen}");

        Assert.AreEqual(2, bus.Vgc.GetGfxPixelColor(0, 0),
            "GTEXT reverse should draw set glyph pixels with the explicit reverse foreground.");
        Assert.AreEqual(6, bus.Vgc.GetGfxPixelColor(1, 0),
            "GTEXT reverse should fill unset glyph pixels with the explicit reverse background.");
        Assert.AreEqual(6, bus.Vgc.GetGfxPixelColor(7, 7),
            "GTEXT reverse should fill the whole glyph cell background.");
        Assert.AreEqual(0, bus.Vgc.GetGfxPixelColor(8, 0),
            "GTEXT reverse should not draw outside the glyph cell.");
        Assert.AreEqual(0x00, bus.Read((ushort)VgcConstants.RegTextFlags),
            "REVERSEOFF should leave later text/graphics output normal.");
    }

    private static string RunProgram(string[] lines)
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);

        foreach (string line in lines)
        {
            foreach (char ch in line)
                editor.QueueInput((byte)ch);
            editor.QueueInput(0x0D);
            RunUntilEditorIdle(cpu, bus, editor, 40_000_000);
        }

        return SnapshotScreen(bus.Vgc);
    }

    private static void EnterProgramLines(Cpu cpu, CompositeBusDevice bus, ScreenEditor editor, string[] lines)
    {
        foreach (string line in lines)
        {
            foreach (char ch in line)
                editor.QueueInput((byte)ch);
            editor.QueueInput(0x0D);
            RunUntilEditorIdle(cpu, bus, editor, 80_000_000);
        }
    }

    private static void RunUntilScreenContains(Cpu cpu, CompositeBusDevice bus, string marker, int maxSteps)
    {
        for (int i = 0; i < maxSteps; i++)
        {
            int cycles = cpu.ClocksForNext();
            cpu.ExecuteNext();
            bus.AdvanceCycles(cycles);
            if ((i & 0x3FF) == 0 && SnapshotScreen(bus.Vgc).Contains(marker, StringComparison.Ordinal))
                return;
        }
        Assert.Fail($"Timed out waiting for '{marker}'.\n{SnapshotScreen(bus.Vgc)}");
    }

    private static void RunUntilEditorIdle(Cpu cpu, CompositeBusDevice bus, ScreenEditor editor, int maxSteps)
    {
        bool queueDrained = false;
        bool sawCursorOff = false;
        for (int i = 0; i < maxSteps; i++)
        {
            int cycles = cpu.ClocksForNext();
            cpu.ExecuteNext();
            bus.AdvanceCycles(cycles);
            if (!editor.HasQueuedInput) queueDrained = true;
            if (queueDrained && !bus.Vgc.IsCursorEnabled) sawCursorOff = true;
            if (queueDrained && sawCursorOff && bus.Vgc.IsCursorEnabled) return;
        }
        Assert.Fail($"Timed out waiting for BASIC idle. Cursor={bus.Vgc.IsCursorEnabled} Queued={editor.HasQueuedInput}\n{SnapshotScreen(bus.Vgc)}");
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

    private static void AssertTextCell(VirtualGraphicsController vgc, int col, char ch, byte color, byte attr)
    {
        Assert.AreEqual((byte)ch, vgc.GetScreenChar(col, 0), $"Unexpected char at text cell {col},0.");
        Assert.AreEqual(color, vgc.GetScreenColor(col, 0), $"Unexpected color attr at text cell {col},0.");
        Assert.AreEqual(attr, vgc.GetScreenTextAttr(col, 0), $"Unexpected text attr at text cell {col},0.");
    }

    private static BitmapFont SinglePixelAFont()
    {
        var fontData = new byte[BitmapFont.FontDataSize];
        fontData[(byte)'A' * BitmapFont.GlyphHeight] = 0x80;
        return new BitmapFont(fontData);
    }
}

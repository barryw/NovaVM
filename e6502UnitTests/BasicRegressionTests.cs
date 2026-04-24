using System;
using System.Text;
using e6502.Avalonia.Hardware;
using e6502.Avalonia.Input;
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
        // Mirrors tests/integration/xram.6502 stash-fetch-roundtrip — fails
        // on FPGA HW (blitter → XRAM back-pressure drops writes when
        // xram_busy=1). This test verifies the Avalonia software path:
        // if it passes, the BASIC / extension ROM / blitter-simulation
        // chain is correct and the HW failure is purely RTL.
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
}

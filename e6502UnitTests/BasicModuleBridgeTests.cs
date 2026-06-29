using System;
using System.Text;
using e6502.Avalonia.Hardware;
using e6502.Avalonia.Input;
using e6502.Avalonia.Rendering;
using KDS.e6502;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

/// <summary>
/// Coexistence bridge regressions for the BASIC-on-Modules refactor.
///
/// BASIC's graphics/text/timing keywords now route through the paged GRAPHICS and
/// SYSTEM modules via lib_call. A lib_call page-in clobbers bank 1 (the overlay
/// that also holds BASIC's static extension ROM). <c>ensure_ext_resident</c>
/// re-pages the extension ROM from XRAM $07C000 before any EXT_CODE keyword runs.
///
/// These tests run BOTH a converted GRAPHICS keyword (which forces a module
/// page-in) AND an extension-ROM keyword (a math-coprocessor reporter routed via
/// EXT_vec) on the SAME persistent machine, proving the two coexist. Without
/// ensure_ext_resident the second keyword would execute the leftover GRAPHICS
/// module image and produce garbage / a syntax error.
/// </summary>
[TestClass]
public class BasicModuleBridgeTests
{
    private const ushort LibResident = 0x0318;        // libabi.inc LIB_RESIDENT
    private const byte LibResidentHostExt = 0xFF;     // libabi.inc LIB_RESIDENT_HOSTEXT
    private const byte LibModuleSystem = 0x03;        // libsystem.inc MODULE_ID_SYSTEM

    [TestMethod]
    public void GraphicsLibCallThenExtensionKeywordBothWorkOnOneMachine()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);

        // Cold start arms LIB_RESIDENT = HOSTEXT, but reaching the "Ready" prompt
        // immediately enters the line reader (LAB_1357 -> lib_call(SYSTEM,
        // SYS_SCREEN_READLINE)), which pages the SYSTEM module into bank 1 and sets
        // LIB_RESIDENT = MODULE_ID_SYSTEM. So by the time the prompt is observable the
        // ext ROM is already displaced — exactly like any other lib_call. The
        // functional contract (ext-ROM keywords still work) is restored on demand by
        // ensure_ext_resident, which parts (a)/(b) below verify.
        Assert.AreEqual(LibResidentHostExt, bus.ReadRam(LibResident),
            "At the first Ready prompt the cold-start extension ROM is still resident " +
            "(the line reader's SYSTEM lib_call fires on the first input, not before); " +
            "the steady-state SYSTEM-resident prompt is asserted at the end of this test.");

        // (a) A converted GRAPHICS keyword forces a real lib_call page-in that
        //     displaces the extension ROM from bank 1 and sets LIB_RESIDENT to a
        //     module id (no longer $FF).
        int pageInsBefore = bus.PageInCount;
        EnterProgramLines(cpu, bus, editor,
        [
            "GCOLOR 5",
            "PLOT 10,20",
        ]);

        Assert.IsTrue(bus.PageInCount > pageInsBefore,
            "A converted GRAPHICS keyword must page a module into bank 1 via lib_call.");
        Assert.AreNotEqual(LibResidentHostExt, bus.ReadRam(LibResident),
            "After a GRAPHICS lib_call the extension ROM should no longer be resident.");
        Assert.AreEqual(5, bus.Vgc.GetGfxPixelColor(10, 20),
            "PLOT routed through the GRAPHICS module must set the real VGC gfx pixel to GCOLOR 5.");

        // (b) An extension-ROM keyword (MMUL16L, a math-coprocessor reporter routed
        //     via EXT_vec) must still produce the correct deterministic result.
        //     This succeeds ONLY if ensure_ext_resident re-paged the extension ROM
        //     back into bank 1 before EXT_CODE swapped to it.
        EnterProgramLines(cpu, bus, editor,
        [
            "POKE 5100,0",
            "A=MMUL16L(300,4)",
            "IF A=1200 THEN POKE 5100,1",
            "PRINT \"M=\";A",
        ]);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsFalse(screen.Contains("Syntax Error", StringComparison.Ordinal),
            $"Extension-ROM math keyword must parse/run after a graphics lib_call.\n{screen}");
        Assert.AreEqual(1, bus.ReadRam(5100),
            $"MMUL16L(300,4) must return 1200 after a graphics lib_call displaced the ext ROM " +
            $"(ensure_ext_resident re-pages it). Screen:\n{screen}");
        Assert.IsTrue(screen.Contains("M= 1200", StringComparison.Ordinal),
            $"MMUL16L should print 1200 through the ext-ROM bridge.\n{screen}");

        // The EXT_CODE re-page guard (ensure_ext_resident) DID restore the ext ROM
        // for MMUL16L — proven by part (b) returning 1200 above (it could not run
        // otherwise). After that, control returned to the Ready prompt, whose line
        // reader fires lib_call(SYSTEM, SYS_SCREEN_READLINE) and pages the SYSTEM
        // module back in. So the steady prompt state is SYSTEM-resident; the ext ROM
        // self-heals on the next ext keyword, exactly as it did here.
        Assert.AreEqual(LibModuleSystem, bus.ReadRam(LibResident),
            "At the Ready prompt the line-reader lib_call leaves the SYSTEM module resident " +
            "(the ext-ROM re-page on demand is proven by MMUL16L returning 1200 above).");
    }

    [TestMethod]
    public void AlternatingGraphicsAndExtensionKeywordsStayConsistent()
    {
        using var bus = new CompositeBusDevice(enableSound: false);
        var cpu = new Cpu(bus);
        cpu.Boot();
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "Ready", 50_000_000);

        // Interleave module lib_calls (LINE) and ext-ROM keywords (MDIST) repeatedly.
        EnterProgramLines(cpu, bus, editor,
        [
            "10 GCOLOR 7",
            "20 FOR I=1 TO 3",
            "30 LINE 0,0,30,30",
            "40 D=MDIST(3,4)",
            "50 IF D<>5 THEN PRINT \"BAD\";I:END",
            "60 NEXT",
            "70 PRINT \"OK\"",
            "RUN",
        ]);

        string screen = SnapshotScreen(bus.Vgc);
        // Look only at output after the RUN line (the source listing contains the
        // literal "BAD" inside line 50, which is not program OUTPUT).
        int runIndex = screen.LastIndexOf("RUN", StringComparison.Ordinal);
        string output = runIndex >= 0 ? screen[runIndex..] : screen;
        Assert.IsFalse(output.Contains("BAD", StringComparison.Ordinal),
            $"MDIST(3,4) must stay 5 across alternating GRAPHICS lib_calls.\n{screen}");
        Assert.IsTrue(output.Contains("OK", StringComparison.Ordinal),
            $"Alternating GRAPHICS + ext-ROM program did not finish cleanly.\n{screen}");
        Assert.AreEqual(7, bus.Vgc.GetGfxPixelColor(0, 0),
            "LINE through the GRAPHICS module must draw with GCOLOR 7.");
    }

    // --- harness (mirrors BasicRegressionTests, persistent single-machine) ---

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

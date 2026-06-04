using System;
using System.IO;
using System.Text;
using e6502.Avalonia.Hardware;
using e6502.Avalonia.Input;
using KDS.e6502;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

/// <summary>
/// Stage 4c.1-3 — NovaLogo's direct graphics/sprite commands now route through
/// <c>lib_call(GRAPHICS)</c> instead of the legacy in-ROM extension trampoline.
///
/// Each test runs a real Logo program on the real <see cref="CompositeBusDevice"/>
/// (Logo bootRom) WITH the GRAPHICS module staged in the XRAM shelf so the resident
/// loader can page it in. Drawing is asserted against the real VGC framebuffer /
/// sprite state, proving the command executed THROUGH the paged library — not the
/// old trampoline path.
/// </summary>
[TestClass]
public class NovaLogoLibCallGraphicsTests
{
    // XRAM shelf slot 0 — matches libabi.inc SHELF_BASE ($060000).
    private const int GraphicsShelfBase = 0x060000;

    // Legacy turtle state ZP (extension.s TURTLE_STATE_BASE = $9F00).
    private const ushort TurtleXLo = 0x9F00 + 1;
    private const ushort TurtleXHi = 0x9F00 + 2;

    [TestMethod]
    public void DrawCommandsRenderThroughLibCallGraphics()
    {
        (string screen, CompositeBusDevice bus) = RunLogo(
            "GFX_DONE",
            [
                "CS",
                "SETCOLOR 1",
                "PLOT 10 10",
                "LINE 20 20 50 20",
                "CIRCLE 80 40 10",
                "RECT 100 20 120 40",
                "FILL 150 20 160 30",
                "SETCOLOR 3",
                "PAINT 200 40",
                "PRINT \"GFX_DONE",
            ]);

        AssertNoError(screen);

        Assert.AreEqual(1, bus.Vgc.GetGfxPixelColor(10, 10),
            "PLOT should set the pixel (color 1) through lib_call(GRAPHICS).");
        Assert.IsTrue(CountNonzeroPixels(bus, 20, 20, 50, 20) >= 20,
            "LINE should leave visible pixels across its span via lib_call.");
        Assert.IsTrue(CountNonzeroPixels(bus, 70, 30, 90, 50) > 10,
            "CIRCLE (radius arg only -> zero-unused-cells leaves ry=rx) should draw an arc.");
        Assert.IsTrue(CountNonzeroPixels(bus, 100, 20, 120, 40) >= 40,
            "RECT should outline a rectangle via lib_call.");
        Assert.IsTrue(CountNonzeroPixels(bus, 150, 20, 160, 30) > 50,
            "FILL should fill a solid rectangle via lib_call(GFN_FILL).");
    }

    [TestMethod]
    public void CircleWithRadiusOnlyDrawsRoundShape()
    {
        // The zero-unused-cells fix: CIRCLE has arity 3 (cx, cy, r). ARG3 (ry) is
        // unwritten -> must read 0 -> the module degenerates ry=rx into a circle.
        // A bad (stale) ARG3 would distort or blank the shape.
        (string screen, CompositeBusDevice bus) = RunLogo(
            "CIRC_DONE",
            [
                "CS",
                "SETCOLOR 1",
                "CIRCLE 100 100 20",
                "PRINT \"CIRC_DONE",
            ]);

        AssertNoError(screen);

        // A radius-20 circle centred at (100,100) should touch all four cardinal
        // extremes roughly equidistant from the centre — i.e. it is round, not a
        // degenerate flat line, which is what a stale ry would have produced.
        Assert.IsTrue(CountNonzeroPixels(bus, 78, 98, 82, 102) > 0, "left arc present");
        Assert.IsTrue(CountNonzeroPixels(bus, 118, 98, 122, 102) > 0, "right arc present");
        Assert.IsTrue(CountNonzeroPixels(bus, 98, 78, 102, 82) > 0, "top arc present");
        Assert.IsTrue(CountNonzeroPixels(bus, 98, 118, 102, 122) > 0, "bottom arc present");
    }

    [TestMethod]
    public void LibCallGraphicsAndLegacyTurtleCoexist()
    {
        // The 4c.0c payoff: a lib_call graphics command pages the GRAPHICS module
        // into bank 1, clobbering the runtime's own extension. A subsequent LEGACY
        // turtle command (SETXY/XCOR) must still work — ensure_ext_resident re-pages
        // the host extension on demand.
        (string screen, CompositeBusDevice bus) = RunLogo(
            "COEXIST_DONE",
            [
                "CS",
                "PLOT 10 10",        // lib_call(GRAPHICS) — pages module into bank 1
                "SETXY 100 50",      // legacy turtle command — re-pages host ext
                "PRINT XCOR",        // legacy reporter — confirms ext still callable
                "PRINT \"COEXIST_DONE",
            ]);

        AssertNoError(screen);

        Assert.AreEqual(1, bus.Vgc.GetGfxPixelColor(10, 10),
            "PLOT should have drawn through lib_call before the turtle command.");
        Assert.AreEqual(100, bus.ReadRam(TurtleXLo) | (bus.ReadRam(TurtleXHi) << 8),
            "Legacy SETXY must still set turtle X after the graphics module paged in.");
        AssertScreenContains(screen, "100"); // XCOR printed 100
    }

    [TestMethod]
    public void SpriteCommandsRouteThroughLibCallGraphics()
    {
        // SPRITE is the POS+ENABLE adapter; SPRITEPOS/ON/OFF are 1:1 GFNs.
        (string screen, CompositeBusDevice bus) = RunLogo(
            "SPR_DONE",
            [
                "CS",
                "SPRITE 0 50 50",         // adapter: GFN_SPR_POS then GFN_SPR_ENABLE
                "SPRITEPOS 0 60 60",      // GFN_SPR_POS
                "SPRITEON 0",             // GFN_SPR_ENABLE
                "PRINT SPRITECOLLISION? 0", // adapter: GFN_SPR_COLL_MASK + bit test
                "SPRITEOFF 0",            // GFN_SPR_DISABLE
                "PRINT \"SPR_DONE",
            ]);

        AssertNoError(screen);

        var sprite = bus.Vgc.GetSpriteState(0);
        Assert.AreEqual(60, sprite.x, "SPRITEPOS should set sprite X via lib_call(GFN_SPR_POS).");
        Assert.AreEqual(60, sprite.y, "SPRITEPOS should set sprite Y via lib_call(GFN_SPR_POS).");
        Assert.IsFalse(sprite.enabled, "SPRITEOFF should disable the sprite via lib_call(GFN_SPR_DISABLE).");
    }

    // ----- harness -----

    private static (string screen, CompositeBusDevice bus) RunLogo(string marker, string[] lines)
    {
        var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        bus.LoadXram(GraphicsShelfBase,
            File.ReadAllBytes(Path.Combine(FindRepoRoot(), "modules", "graphics", "graphics.bin")));

        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntilScreenContains(cpu, bus, "?", 10_000_000, marker);

        foreach (string line in lines)
            foreach (char ch in line + "\r")
                editor.QueueInput((byte)ch);

        RunUntilScreenContains(cpu, bus, marker, 80_000_000, marker);
        RunSteps(cpu, bus, 1_000_000);

        return (SnapshotScreen(bus.Vgc), bus);
    }

    private static void AssertNoError(string screen)
    {
        Assert.IsFalse(screen.Contains("I DON'T KNOW HOW TO", StringComparison.Ordinal),
            $"No graphics command should fall through to the unknown-word path.\n{screen}");
        Assert.IsFalse(screen.Contains("NOT ENOUGH INPUTS", StringComparison.Ordinal),
            $"Programs should supply the required arguments.\n{screen}");
    }

    private static void RunUntilScreenContains(Cpu cpu, CompositeBusDevice bus, string marker, int maxSteps, string name)
    {
        for (int i = 0; i < maxSteps; i++)
        {
            int cycles = cpu.ClocksForNext();
            cpu.ExecuteNext();
            bus.AdvanceCycles(cycles);
            if ((i & 0x3FF) == 0 && SnapshotScreen(bus.Vgc).Contains(marker, StringComparison.Ordinal))
                return;
        }

        Assert.Fail($"{name}: timed out waiting for screen marker '{marker}'.\n{SnapshotScreen(bus.Vgc)}");
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

    private static void AssertScreenContains(string screen, string expected) =>
        Assert.IsTrue(screen.Contains(expected, StringComparison.Ordinal),
            $"Expected screen to contain '{expected}'.\n{screen}");

    private static int CountNonzeroPixels(CompositeBusDevice bus, int left, int top, int right, int bottom)
    {
        int count = 0;
        for (int y = top; y <= bottom; y++)
            for (int x = left; x <= right; x++)
                if (bus.Vgc.GetGfxPixelColor(x, y) != 0)
                    count++;
        return count;
    }

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
}

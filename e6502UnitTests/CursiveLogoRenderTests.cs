using System;
using System.IO;
using System.Text;
using e6502.Avalonia.Hardware;
using e6502.Avalonia.Input;
using KDS.e6502;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

// SCRATCH render rig for designing the cursive-LOGO startup splash. Reads a Logo
// program from a scratch file, runs it headless on the Logo runtime, and dumps the
// text screen + a downsampled ASCII of the 320x200 gfx layer to a file. Not a real
// test (no asserts) -- delete before final commit.
[TestClass]
public class CursiveLogoRenderTests
{
    private const string SrcPath = "/home/barry/.claude/jobs/b1fd5d91/tmp/cursive.logo";
    private const string OutPath = "/home/barry/.claude/jobs/b1fd5d91/tmp/render.txt";

    [TestMethod]
    public void RenderCursive()
    {
        string[] lines = File.ReadAllText(SrcPath).Replace("\r", "").Split('\n');

        var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        bus.StageShelfModule(0,
            File.ReadAllBytes(Path.Combine(FindRepoRoot(), "software", "modules", "graphics", "graphics.bin")), 0x01);
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);

        RunUntil(cpu, bus, "?", 10_000_000);
        foreach (string line in lines)
            foreach (char ch in line + "\r")
                editor.QueueInput((byte)ch);

        // Program ends with `PRINT 12345 * 2` -> outputs 24690, which can't appear
        // in the echoed source. Stop as soon as it shows (cap = safety).
        RunUntil(cpu, bus, "24690", 90_000_000);
        RunSteps(cpu, bus, 2_000_000);

        var sb = new StringBuilder();
        sb.Append("=== text screen (errors/markers) ===\n");
        sb.Append(Snapshot(bus.Vgc));
        sb.Append("\n=== gfx layer 320x200 -> 80x50 (4x4 blocks; '#'=ink) ===\n");
        for (int y = 0; y < 200; y += 4)
        {
            for (int x = 0; x < 320; x += 4)
            {
                bool on = false;
                for (int dy = 0; dy < 4 && !on; dy++)
                    for (int dx = 0; dx < 4 && !on; dx++)
                        if (bus.Vgc.GetGfxPixelColor(x + dx, y + dy) != 0) on = true;
                sb.Append(on ? '#' : '.');
            }
            sb.Append('\n');
        }
        File.WriteAllText(OutPath, sb.ToString());

        // Also emit a 3x-scaled PPM of the gfx layer (white ink on black) for a real look.
        const int sc = 3, W = 320, H = 200;
        using var fs = new FileStream("/home/barry/.claude/jobs/b1fd5d91/tmp/render.ppm", FileMode.Create);
        byte[] hdr = System.Text.Encoding.ASCII.GetBytes($"P6\n{W * sc} {H * sc}\n255\n");
        fs.Write(hdr, 0, hdr.Length);
        byte[] row = new byte[W * sc * 3];
        for (int y = 0; y < H; y++)
        {
            for (int x = 0; x < W; x++)
            {
                byte v = (byte)(bus.Vgc.GetGfxPixelColor(x, y) != 0 ? 255 : 0);
                for (int s = 0; s < sc; s++)
                {
                    int o = (x * sc + s) * 3;
                    row[o] = v; row[o + 1] = v; row[o + 2] = v;
                }
            }
            for (int s = 0; s < sc; s++) fs.Write(row, 0, row.Length);
        }
    }

    private static void RunUntil(Cpu cpu, CompositeBusDevice bus, string marker, int maxSteps)
    {
        for (int i = 0; i < maxSteps; i++)
        {
            int cycles = cpu.ClocksForNext();
            cpu.ExecuteNext();
            bus.AdvanceCycles(cycles);
            if ((i & 0x3FF) == 0 && Snapshot(bus.Vgc).Contains(marker, StringComparison.Ordinal))
                return;
        }
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

    private static string Snapshot(VirtualGraphicsController vgc)
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

    private static string FindRepoRoot()
    {
        string? dir = AppContext.BaseDirectory;
        while (dir is not null)
        {
            if (File.Exists(Path.Combine(dir, "software", "languages", "novalogo", "builtins.s"))) return dir;
            dir = Directory.GetParent(dir)?.FullName;
        }
        throw new Exception("repo root not found");
    }
}

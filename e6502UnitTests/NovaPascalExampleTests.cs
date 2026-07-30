using System;
using System.IO;
using System.Text;
using e6502.Avalonia.Hardware;
using e6502.Avalonia.Input;
using e6502.Storage;
using KDS.e6502;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
[DoNotParallelize]
public class NovaPascalExampleTests
{
    // Every project shipped on novapascal.ndi. The demos and games run until a
    // key arrives (Esc leaves); Life stops on Enter; Hello and FizzBuzz return
    // on their own. MicroCalc has its own end-to-end test.
    private static readonly (string Name, byte QuitKey)[] Projects =
    [
        ("HELLO", 0x00),
        ("FIZZBUZZ", 0x00),
        ("LIFE", 0x0D),
        ("STARBURST", 0x1B),
        ("CUBE", 0x1B),
        ("JUKEBOX", 0x1B),
        ("SNAKE", 0x1B),
        ("BREAKOUT", 0x1B),
    ];

    [TestMethod]
    public void EveryShippedProjectBuildsRunsAndReturns()
    {
        string root = Path.Combine(Path.GetTempPath(), $"novapascal-examples-{Guid.NewGuid():N}");
        string disks = Path.Combine(root, "disks");
        Directory.CreateDirectory(disks);
        File.Copy(
            RepoPath("software", "languages", "novapascal", "novapascal.ndi"),
            Path.Combine(disks, "fd0.ndi"));

        try
        {
            using var storage = new EnvScope("NOVA_STORAGE_ROOT", root);
            using var automount = new EnvScope("NOVA_NO_AUTOMOUNT", null);
            using var autoboot = new EnvScope("NOAUTO", null);
            using var bus = new CompositeBusDevice(enableSound: false);
            var cpu = new Cpu(bus);
            var editor = new ScreenEditor(bus.Vgc);
            bus.Vgc.SetScreenEditor(editor);
            cpu.Boot();
            RunUntil(cpu, bus, s => s.Contains("NovaPascal Shell v1.0", StringComparison.Ordinal), "shell banner");

            var disk = bus.DeviceManager.GetDevice("FD0");
            foreach ((string name, byte quitKey) in Projects)
            {
                bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);
                QueueLine(editor, $"BUILD {name}");
                string build = RunUntil(cpu, bus,
                    s => s.Contains($"Build complete: {name}.BIN", StringComparison.Ordinal)
                         || s.Contains(": error:", StringComparison.Ordinal)
                         || s.Contains("syntax error", StringComparison.OrdinalIgnoreCase)
                         || s.Contains("Assembler error", StringComparison.Ordinal)
                         || s.Contains("Linker error", StringComparison.Ordinal),
                    $"{name} build", instructionLimit: 1_500_000_000);

                disk.CurrentDirectory = name;
                bool built = disk.FileExists(name, ".BIN");
                disk.CurrentDirectory = "/";
                Assert.IsTrue(built,
                    $"{name} failed to build: line={bus.Read(0x02F3) | bus.Read(0x02F4) << 8}, " +
                    $"column={bus.Read(0x02F5) | bus.Read(0x02F6) << 8}\n{build}");

                bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);
                QueueLine(editor, $"RUN {name}");
                RunUntil(cpu, bus, s => s.Contains("Running at $", StringComparison.Ordinal),
                    $"{name} start");
                RunSteps(cpu, bus, 6_000_000);

                if (quitKey == 0)
                {
                    // Hello and FizzBuzz finish on their own, well inside the
                    // settle above, so the prompt is already back.
                    string done = Snapshot(bus);
                    Assert.IsTrue(Prompts(done) > 0,
                        $"{name} must return to the shell on its own.\n{done}");
                    continue;
                }

                // An interactive program that clears the screen takes the RUN
                // line with it, so the shell is back when a prompt appears that
                // was not there while the program was running.
                int prompts = Prompts(Snapshot(bus));
                editor.QueueInput(quitKey);
                RunUntil(cpu, bus, s => Prompts(s) > prompts,
                    $"{name} exit to shell", instructionLimit: 400_000_000);
            }
        }
        finally
        {
            Directory.Delete(root, recursive: true);
        }
    }

    private static int Prompts(string screen)
    {
        int count = 0;
        // Match the bare prompt: the cursor glyph may occupy the cell after it.
        for (int index = screen.IndexOf("NP>", StringComparison.Ordinal); index >= 0;
             index = screen.IndexOf("NP>", index + 1, StringComparison.Ordinal))
            count++;
        return count;
    }

    private static void QueueLine(ScreenEditor editor, string line)
    {
        foreach (char ch in line)
            editor.QueueInput((byte)ch);
        editor.QueueInput(0x0D);
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

    private static string RunUntil(Cpu cpu, CompositeBusDevice bus, Func<string, bool> predicate, string expected,
        int instructionLimit = 200_000_000)
    {
        for (int i = 0; i < instructionLimit; i++)
        {
            int cycles = cpu.ClocksForNext();
            cpu.ExecuteNext();
            bus.AdvanceCycles(cycles);
            if ((i & 0x3FFF) == 0)
            {
                string screen = Snapshot(bus);
                if (predicate(screen))
                    return screen;
            }
        }

        Assert.Fail($"Timed out waiting for {expected}.\n{Snapshot(bus)}");
        return string.Empty;
    }

    private static string Snapshot(CompositeBusDevice bus)
    {
        var text = new StringBuilder();
        for (int row = 0; row < VgcConstants.ScreenRows; row++)
        {
            for (int col = 0; col < VgcConstants.ScreenCols; col++)
            {
                byte ch = bus.Vgc.GetScreenChar(col, row);
                text.Append(ch is >= 0x20 and <= 0x7E ? (char)ch : ' ');
            }
            text.Append('\n');
        }

        return text.ToString();
    }

    private static string RepoPath(params string[] parts)
    {
        string? directory = AppContext.BaseDirectory;
        while (directory is not null)
        {
            string path = Path.Combine([directory, .. parts]);
            if (File.Exists(path))
                return path;
            directory = Directory.GetParent(directory)?.FullName;
        }

        throw new FileNotFoundException($"Could not find {Path.Combine(parts)} from the test output directory.");
    }
}

using System;
using System.IO;
using System.Text;
using e6502.Avalonia.Hardware;
using e6502.Avalonia.Input;
using e6502.Storage;
using KDS.e6502;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

/// <summary>
/// The shell surface a developer touches on the shipped disk: EDIT must open
/// every kind of source the toolchain consumes and refuse only real binaries.
/// Turbo-era sources (MicroCalc) carry CP/M $1A end-of-file padding, which once
/// made the editor call them binary; the text rule lives in two places (the
/// shared nptool validator and the Editor module's paged scan) and both are
/// exercised here.
/// </summary>
[TestClass]
[DoNotParallelize]
public class NovaPascalShellCoverageTests
{
    private static readonly string[] EditableFiles =
    [
        "MICROCALC",                  // project: opens its manifest-selected main source
        "MICROCALC/MC.PAS",           // Pascal source with CP/M padding
        "MICROCALC/MC-MOD03.INC",     // include with CP/M padding
        "MICROCALC/MICROCALC.NPP",    // project manifest
        "/SYSTEM/CRT.PAS",            // unit source
        "/SYSTEM/CRT.NPI",            // precompiled interface
        "/SYSTEM/GRAPH.S",            // assembly
        "NPHELP.TXT",                 // plain text
        "STARBURST",                  // a sample project
    ];

    private static readonly string[] RefusedFiles =
    [
        "NAS.BIN",                    // executable
        "MICROCALC/MCDEMO.MCS",       // binary worksheet
    ];

    [TestMethod]
    public void EditorOpensEverySourceKindAndRefusesBinaries()
    {
        foreach (string file in EditableFiles)
        {
            string screen = RunShellCommand($"EDIT {file}");
            StringAssert.Contains(screen, "File   Edit   Search",
                $"EDIT {file} must open the editor.\n{screen}");
            Assert.IsFalse(screen.Contains("Not a text file", StringComparison.Ordinal),
                $"EDIT {file} must not be rejected as binary.\n{screen}");
        }

        foreach (string file in RefusedFiles)
        {
            string screen = RunShellCommand($"EDIT {file}");
            StringAssert.Contains(screen, "Not a text file",
                $"EDIT {file} must refuse a binary.\n{screen}");
        }
    }

    private static string RunShellCommand(string command)
    {
        string root = Path.Combine(Path.GetTempPath(), $"novapascal-shell-{Guid.NewGuid():N}");
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

            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);
            foreach (char ch in command)
                editor.QueueInput((byte)ch);
            editor.QueueInput(0x0D);
            RunUntil(cpu, bus,
                s => s.Contains("File   Edit   Search", StringComparison.Ordinal)
                     || s.Contains("Not a text file", StringComparison.Ordinal)
                     || s.Contains("File not found", StringComparison.Ordinal)
                     || s.Contains("Unknown command", StringComparison.Ordinal),
                $"result of {command}");
            return Snapshot(bus);
        }
        finally
        {
            Directory.Delete(root, recursive: true);
        }
    }

    private static string RunUntil(Cpu cpu, CompositeBusDevice bus, Func<string, bool> predicate, string expected)
    {
        for (int i = 0; i < 400_000_000; i++)
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

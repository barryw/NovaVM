using System;
using System.IO;
using System.Text;
using e6502.Avalonia.Hardware;
using e6502.Avalonia.Input;
using KDS.e6502;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
[DoNotParallelize]
public class NovaPascalTests
{
    [TestMethod]
    public void ShellBuildsRunsAndReturnsFromEditor()
    {
        string root = Path.Combine(Path.GetTempPath(), $"novapascal-{Guid.NewGuid():N}");
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
            Assert.IsTrue(Snapshot(bus).Contains("Type HELP to get started", StringComparison.Ordinal));

            var disk = bus.DeviceManager.GetDevice("FD0");
            Assert.IsFalse(disk.FileExists("NPC", ".BIN"), "NPC must remain resident at $C000, not be a disk tool.");
            Assert.IsTrue(disk.FileExists("NPEDIT", ".BIN"), "The editor must be a standard disk-loaded binary.");
            Assert.IsTrue(disk.FileExists("NAS", ".BIN"), "NAS must be a standard, language-neutral binary.");
            Assert.IsTrue(disk.FileExists("NL", ".BIN"), "NL must be a standard, language-neutral binary.");
            Assert.IsTrue(disk.FileExists("PASCAL", ".NLIB"), "Pascal runtime APIs belong in an ordinary linker library.");
            CollectionAssert.AreEqual(new byte[] { 0x00, 0x40 }, disk.Load("NAS", ".BIN")[..2]);
            CollectionAssert.AreEqual(new byte[] { 0x00, 0x40 }, disk.Load("NL", ".BIN")[..2]);

            QueueLine(editor, "DIR");
            RunUntil(cpu, bus, s => s.Contains("PASCAL SOURCE", StringComparison.Ordinal)
                                    && s.Contains("PASCAL PROJECT", StringComparison.Ordinal),
                "distinct Pascal source and project descriptors in directory listing");
            Assert.IsTrue(Snapshot(bus).Contains("HELLO.PAS", StringComparison.Ordinal));
            Assert.IsTrue(Snapshot(bus).Contains("HELLO.NPP", StringComparison.Ordinal));
            RunSteps(cpu, bus, 100_000);

            QueueLine(editor, "HELP");
            RunUntil(cpu, bus, s => s.Contains("RUN file.bin", StringComparison.Ordinal), "shell help");
            RunSteps(cpu, bus, 100_000);

            QueueLine(editor, "BUILD HELLO.NPP");
            RunUntil(cpu, bus, s => s.Contains("Build complete: HELLO.BIN", StringComparison.Ordinal)
                                    || s.Contains("Invalid NPP", StringComparison.Ordinal)
                                    || s.Contains("File error", StringComparison.Ordinal)
                                    || s.Contains("Pascal syntax error", StringComparison.Ordinal)
                                    || s.Contains("Assembler error", StringComparison.Ordinal)
                                    || s.Contains("Linker error", StringComparison.Ordinal), "project build result");
            string buildScreen = Snapshot(bus);
            Assert.IsTrue(buildScreen.Contains("Build complete: HELLO.BIN", StringComparison.Ordinal),
                buildScreen.Contains("Invalid NPP", StringComparison.Ordinal) ? "NovaPascal reported: Invalid NPP"
                : buildScreen.Contains("File error", StringComparison.Ordinal) ? "NovaPascal reported: File error"
                : buildScreen.Contains("Pascal syntax error", StringComparison.Ordinal) ? "NovaPascal reported: Pascal syntax error"
                : buildScreen.Contains("Assembler error", StringComparison.Ordinal) ? "NovaPascal reported: Assembler error"
                : buildScreen.Contains("Linker error", StringComparison.Ordinal) ? "NovaPascal reported: Linker error"
                : buildScreen);
            StringAssert.Contains(buildScreen, "Nova Pascal Compiler v1.0");
            StringAssert.Contains(buildScreen, "Compiling HELLO.PAS");
            StringAssert.Contains(buildScreen, "Writing HELLO.S");
            StringAssert.Contains(buildScreen, "Nova Assembler v1.0");
            StringAssert.Contains(buildScreen, "Assembling HELLO.S");
            StringAssert.Contains(buildScreen, "Writing HELLO.OBJ");
            StringAssert.Contains(buildScreen, "Nova Linker v1.0");
            StringAssert.Contains(buildScreen, "Linking HELLO.OBJ");
            StringAssert.Contains(buildScreen, "Library PASCAL.NLIB");
            StringAssert.Contains(buildScreen, "Writing HELLO.BIN");

            QueueLine(editor, "RUN HELLO.BIN");
            RunUntil(cpu, bus, s => s.Contains("Hello from NovaPascal", StringComparison.Ordinal), "linked program output");

            QueueLine(editor, "TYPE HELLO.S");
            RunUntil(cpu, bus, s => s.Contains("JSR P_WRITE_CHAR", StringComparison.Ordinal),
                "TYPE displays generated assembly text");

            QueueLine(editor, "EDIT HELLO.PAS");
            RunUntil(cpu, bus, s => s.Contains("HELLO.PAS", StringComparison.Ordinal)
                                    && s.Contains("program Hello;", StringComparison.Ordinal), "Pascal editor");
            editor.QueueInput(0x1B);
            editor.QueueInput((byte)'x');
            RunUntil(cpu, bus, s => s.Contains("NP> ", StringComparison.Ordinal)
                                    && !s.Contains("program Hello;", StringComparison.Ordinal), "shell after Alt-X");

            QueueLine(editor, "EDIT HELLO.S");
            RunUntil(cpu, bus, s => s.Contains("JSR P_WRITE_CHAR", StringComparison.Ordinal)
                                    && s.Contains("T:Assembly Source", StringComparison.Ordinal),
                "assembly-aware editor");
            editor.QueueInput(0x11);
            RunUntil(cpu, bus, s => s.Contains("NP> ", StringComparison.Ordinal), "shell after Ctrl-Q");

            QueueLine(editor, "EDIT HELLO.NPP");
            RunUntil(cpu, bus, s => s.Contains("MAIN HELLO.PAS", StringComparison.Ordinal)
                                    && s.Contains("T:Pascal Project", StringComparison.Ordinal),
                "project-aware editor");
            editor.QueueInput(0x11);
            RunUntil(cpu, bus, s => s.Contains("NP> ", StringComparison.Ordinal), "shell after project editor");

            disk.Save("HELLO", Encoding.ASCII.GetBytes("OUTPUT HELLO.BIN\n"), ".CFG");
            QueueLine(editor, "EDIT HELLO.CFG");
            RunUntil(cpu, bus, s => s.Contains("OUTPUT HELLO.BIN", StringComparison.Ordinal)
                                    && s.Contains("T:Linker Config", StringComparison.Ordinal),
                "linker-config-aware editor");
            editor.QueueInput(0x11);
            RunUntil(cpu, bus, s => s.Contains("NP> ", StringComparison.Ordinal), "shell after config editor");

            QueueLine(editor, "TYPE HELLO.BIN");
            RunUntil(cpu, bus, s => s.Contains("Not a text file.", StringComparison.Ordinal),
                "TYPE rejects binary files");

            QueueLine(editor, "EDIT HELLO.ASM");
            RunUntil(cpu, bus, s => s.Contains("File not found: HELLO.ASM", StringComparison.Ordinal),
                "human-readable missing-file error");

            Assert.IsTrue(disk.FileExists("HELLO", ".S"), "BUILD must retain generated assembly.");
            Assert.IsTrue(disk.FileExists("HELLO", ".OBJ"), "BUILD must retain the NOBJ object.");
            Assert.IsTrue(disk.FileExists("HELLO", ".BIN"), "BUILD must retain the linked binary.");

            string assembly = Encoding.ASCII.GetString(disk.Load("HELLO", ".S"));
            StringAssert.Contains(assembly, "JSR P_WRITE_CHAR");
            Assert.IsFalse(assembly.Contains("$A00E", StringComparison.Ordinal),
                "NPC must emit a symbol, not a hard-coded hardware address.");

            byte[] objectFile = disk.Load("HELLO", ".OBJ");
            int codeLength = objectFile[6] | objectFile[7] << 8;
            Assert.IsTrue((objectFile[10] | objectFile[11] << 8) > 0,
                "NAS must preserve external calls as NOBJ relocations.");
            byte[] library = disk.Load("PASCAL", ".NLIB");
            byte[] usedMember = ReadNlibMember(library, "P_WRITE_CHAR");
            byte[] unusedMember = ReadNlibMember(library, "P_UNUSED");
            byte[] executable = disk.Load("HELLO", ".BIN");
            Assert.AreEqual(2 + codeLength + usedMember.Length, executable.Length,
                "NL must include one referenced archive member and no archive padding or duplicates.");
            Assert.IsTrue(ContainsSequence(executable, usedMember));
            Assert.IsFalse(ContainsSequence(executable, unusedMember),
                "NL must strip unreferenced library members.");
        }
        finally
        {
            try { Directory.Delete(root, recursive: true); } catch { }
        }
    }

    private static void QueueLine(ScreenEditor editor, string line)
    {
        foreach (char ch in line)
            editor.QueueInput((byte)ch);
        editor.QueueInput(0x0D);
    }

    private static void RunUntil(Cpu cpu, CompositeBusDevice bus, Func<string, bool> predicate, string expected)
    {
        for (int i = 0; i < 200_000_000; i++)
        {
            int cycles = cpu.ClocksForNext();
            cpu.ExecuteNext();
            bus.AdvanceCycles(cycles);
            if ((i & 0x3FFF) == 0 && predicate(Snapshot(bus)))
                return;
        }

        Assert.Fail($"Timed out waiting for {expected}. PC=${cpu.Pc:X4}\n{Snapshot(bus)}");
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

    private static byte[] ReadNlibMember(byte[] library, string wanted)
    {
        int offset = 6;
        for (int member = 0; member < library[5]; member++)
        {
            int nameLength = library[offset++];
            string name = Encoding.ASCII.GetString(library, offset, nameLength);
            offset += nameLength;
            int codeLength = library[offset] | library[offset + 1] << 8;
            offset += 2;
            if (name == wanted)
                return library[offset..(offset + codeLength)];
            offset += codeLength;
        }
        Assert.Fail($"NLIB member {wanted} was not found.");
        return [];
    }

    private static bool ContainsSequence(byte[] haystack, byte[] needle)
    {
        return haystack.AsSpan().IndexOf(needle) >= 0;
    }
}

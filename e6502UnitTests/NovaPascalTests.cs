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
            Assert.IsTrue(disk.FileExists("NASPP", ".OVL"), "NAS preprocessing must live in its disk-loaded overlay.");
            Assert.IsTrue(disk.FileExists("NASBE", ".OVL"), "NAS assembly core and opcode tables must live in its disk-loaded backend overlay.");
            Assert.IsTrue(disk.FileExists("NL", ".BIN"), "NL must be a standard, language-neutral binary.");
            Assert.IsTrue(disk.FileExists("NLWORK", ".OVL"), "NL map/GC work must live in its disk-loaded overlay.");
            Assert.IsTrue(disk.FileExists("PASCAL", ".NLIB"), "Pascal runtime APIs belong in an ordinary linker library.");
            CollectionAssert.AreEqual(new byte[] { 0x00, 0x20 }, disk.Load("NAS", ".BIN")[..2]);
            CollectionAssert.AreEqual(new byte[] { 0x00, 0x20 }, disk.Load("NL", ".BIN")[..2]);

            QueueLine(editor, "DIR");
            RunUntil(cpu, bus, s => s.Contains("PASCAL SOURCE", StringComparison.Ordinal)
                                    && s.Contains("PASCAL PROJECT", StringComparison.Ordinal),
                "distinct Pascal source and project descriptors in directory listing");
            Assert.IsTrue(Snapshot(bus).Contains("HELLO.PAS", StringComparison.Ordinal));
            Assert.IsTrue(Snapshot(bus).Contains("HELLO.NPP", StringComparison.Ordinal));
            RunSteps(cpu, bus, 100_000);

            QueueLine(editor, "HELP");
            RunUntil(cpu, bus, s => s.Contains("RUN file.bin", StringComparison.Ordinal), "shell help");
            StringAssert.Contains(Snapshot(bus), "NEW name",
                "The project generator must be discoverable from shell HELP.");
            RunSteps(cpu, bus, 100_000);
            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);

            QueueLine(editor, "NEW DEMO");
            RunUntil(cpu, bus, s => s.Contains("Created DEMO.NPP", StringComparison.Ordinal),
                "new Pascal project");
            byte[] demoSource = Encoding.ASCII.GetBytes(
                "program DEMO;\nbegin\n  writeln('Hello, world!');\nend.\n");
            CollectionAssert.AreEqual(demoSource, disk.Load("DEMO", ".PAS"),
                "NEW must create a minimal LF-only Pascal program.");
            CollectionAssert.AreEqual(Encoding.ASCII.GetBytes(
                    "NPP 1\nMAIN DEMO.PAS\nOUTPUT DEMO.BIN\nOPTIMIZE O2\n" +
                    "DEFINE NOVA=1\nCONFIG INLINE\nMAP DEMO.MAP\nLABEL DEMO.LBL\n" +
                    "MEMORY {\n    RAM: start = $8000, size = $0100, file = %O;\n}\n\n" +
                    "SEGMENTS {\n    CODE: load = RAM, type = ro;\n}\n"),
                disk.Load("DEMO", ".NPP"),
                "The generated project must contain NPC, NAS, and NL configuration.");
            Assert.IsFalse(disk.FileExists("DEMO", ".CFG"),
                "Inline projects must not create a redundant linker-config file.");

            QueueLine(editor, "NEW DEMO");
            RunUntil(cpu, bus, s => s.Contains("Project already exists.", StringComparison.Ordinal),
                "new project overwrite protection");
            CollectionAssert.AreEqual(demoSource, disk.Load("DEMO", ".PAS"),
                "NEW must never overwrite an existing project file.");

            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);
            QueueLine(editor, "BUILD DEMO.NPP");
            RunUntil(cpu, bus, s => s.Contains("Build complete: DEMO.BIN", StringComparison.Ordinal)
                                    || s.Contains("Invalid NPP", StringComparison.Ordinal)
                                    || s.Contains("error", StringComparison.OrdinalIgnoreCase),
                "generated project build");
            string generatedBuildScreen = Snapshot(bus);
            Assert.IsTrue(generatedBuildScreen.Contains("Build complete: DEMO.BIN", StringComparison.Ordinal),
                generatedBuildScreen);
            Assert.IsTrue(
                generatedBuildScreen.IndexOf("Configuration valid", StringComparison.Ordinal) <
                generatedBuildScreen.IndexOf("Compiling DEMO.PAS", StringComparison.Ordinal),
                "BUILD must validate linker configuration before NPC creates intermediate files.");
            StringAssert.Contains(generatedBuildScreen, "Library PASCAL.NLIB",
                "Generated Pascal projects must link the standard runtime/NDK library.");

            QueueLine(editor, "RUN DEMO.BIN");
            RunUntil(cpu, bus, s => s.Contains("Hello, world!", StringComparison.Ordinal),
                "generated project executable");
            RunSteps(cpu, bus, 100_000);
            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);

            QueueLine(editor, "BUILD HELLO.NPP");
            RunUntil(cpu, bus, s => s.Contains("Compiling HELLO.PAS", StringComparison.Ordinal),
                "project compiler launch");
            RunUntil(cpu, bus, s => s.Contains("Assembling HELLO.S", StringComparison.Ordinal),
                "project assembler launch");
            RunSteps(cpu, bus, 2_000_000);
            Assert.IsTrue(disk.FileExists("HELLO", ".OBJ"),
                $"NAS did not write HELLO.OBJ (detail ${bus.Read(0x0276):X2}):\n{Snapshot(bus)}");
            RunUntil(cpu, bus, s => s.Contains("Build complete: HELLO.BIN", StringComparison.Ordinal)
                                    || s.Contains("Linker configuration error", StringComparison.Ordinal),
                "project build result");
            string buildScreen = Snapshot(bus);
            Assert.IsTrue(buildScreen.Contains("Build complete: HELLO.BIN", StringComparison.Ordinal),
                buildScreen.Contains("Invalid NPP", StringComparison.Ordinal) ? "NovaPascal reported: Invalid NPP"
                : buildScreen.Contains("File error", StringComparison.Ordinal) ? "NovaPascal reported: File error"
                : buildScreen.Contains("Pascal syntax error", StringComparison.Ordinal) ? "NovaPascal reported: Pascal syntax error"
                : buildScreen.Contains(": error:", StringComparison.Ordinal) ? $"NovaPascal reported an assembler error:\n{buildScreen}"
                : buildScreen.Contains("Linker configuration error", StringComparison.Ordinal)
                    ? "NovaPascal reported a linker configuration error"
                : buildScreen.Contains("Linker error", StringComparison.Ordinal) ? "NovaPascal reported: Linker error"
                : buildScreen);
            StringAssert.Contains(buildScreen, "Nova Pascal Compiler v1.0");
            StringAssert.Contains(buildScreen, "Compiling HELLO.PAS");
            StringAssert.Contains(buildScreen, "Writing HELLO.S");
            StringAssert.Contains(buildScreen, "Nova Assembler v1.0");
            StringAssert.Contains(buildScreen, "Preprocessing HELLO.S");
            StringAssert.Contains(buildScreen, "Assembling HELLO.S");
            StringAssert.Contains(buildScreen, "Writing HELLO.OBJ");
            StringAssert.Contains(buildScreen, "Nova Linker v1.0");
            StringAssert.Contains(buildScreen, "Config HELLO.NPP");
            StringAssert.Contains(buildScreen, "Linking HELLO.OBJ");
            StringAssert.Contains(buildScreen, "Library PASCAL.NLIB");
            StringAssert.Contains(buildScreen, "Writing HELLO.BIN");
            StringAssert.Contains(buildScreen, "Map HELLO.MAP");
            StringAssert.Contains(buildScreen, "Labels HELLO.LBL");
            CollectionAssert.AreEqual(new byte[] { 0x00, 0x80 }, disk.Load("HELLO", ".BIN")[..2],
                "NPP CONFIG must control NL placement through the shared tool mailbox.");
            string helloMap = Encoding.ASCII.GetString(disk.Load("HELLO", ".MAP"));
            StringAssert.Contains(helloMap, "Nova Linker Map v1\nLoad $8000\n");
            StringAssert.Contains(helloMap, "Sections\n");
            StringAssert.Contains(helloMap, "Exports\n");
            StringAssert.Contains(helloMap, "P_WRITE_CHAR");
            Assert.IsFalse(helloMap.Contains('\r'), "NL maps must use Nova's LF-only text convention.");
            string helloLabels = Encoding.ASCII.GetString(disk.Load("HELLO", ".LBL"));
            StringAssert.Contains(helloLabels, "al 00");
            StringAssert.Contains(helloLabels, " .P_WRITE_CHAR\n");
            Assert.IsFalse(helloLabels.Contains('\r'), "NL labels must use Nova's LF-only text convention.");

            QueueLine(editor, "RUN HELLO.BIN");
            RunUntil(cpu, bus, s => s.Contains("Hello from NovaPascal", StringComparison.Ordinal), "linked program output");
            StringAssert.Contains(Snapshot(bus), "Running at $8000:");

            RunSteps(cpu, bus, 100_000);
            disk.Save("TINY", Encoding.ASCII.GetBytes(
                "mEmOrY { ram: StArT = $8000, SiZe = $0001, FiLe = %o; }\n"), ".CFG");
            disk.Save("TINY", Encoding.ASCII.GetBytes(
                "NPP 1\nMAIN HELLO.PAS\nOUTPUT TINY.BIN\nCONFIG TINY.CFG\n"), ".NPP");
            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);
            QueueLine(editor, "BUILD TINY.NPP");
            RunUntil(cpu, bus, s => s.Contains("Linker error.", StringComparison.Ordinal),
                "configured linker-region overflow");
            Assert.IsFalse(disk.FileExists("TINY", ".BIN"),
                "NL must reject output that does not fit the configured MEMORY region.");
            RunSteps(cpu, bus, 100_000);

            disk.Save("BADRUN", new byte[] { 0x00, 0xA0, 0x60 }, ".BIN");
            QueueLine(editor, "RUN BADRUN.BIN");
            RunUntil(cpu, bus, s => s.Contains("Not a runnable Nova binary.", StringComparison.Ordinal),
                "MMIO-overlapping binary rejection");

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

            disk.Save("LABELS", Encoding.ASCII.GetBytes(
                "Start:\nbra ENTRY\n.byte $DE,$AD\nEntry:\njsr sub\nRTS\n" +
                "Sub:\nLdA #$7E\nSta $A00E\nrts\n"), ".S");
            QueueLine(editor, "ASSEMBLE LABELS.S");
            RunUntil(cpu, bus, s => s.Contains("Writing LABELS.OBJ", StringComparison.Ordinal)
                                    || s.Contains(": error:", StringComparison.Ordinal),
                "generic assembly result");
            RunSteps(cpu, bus, 100_000);
            Assert.IsTrue(Snapshot(bus).Contains("Assembly successful", StringComparison.Ordinal),
                $"NAS failed generic assembly with detail ${bus.Read(0x0276):X2}:\n{Snapshot(bus)}");

            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);
            QueueLine(editor, "LINK LABELS.OBJ");
            RunUntil(cpu, bus, s => s.Contains("Link successful", StringComparison.Ordinal)
                                    || s.Contains("Linker error", StringComparison.Ordinal),
                "generic link result");
            StringAssert.Contains(Snapshot(bus), "Link successful");

            byte[] labelsObject = disk.Load("LABELS", ".OBJ");
            CollectionAssert.AreEqual(Encoding.ASCII.GetBytes("NOBJ"), labelsObject[..4]);
            Assert.AreEqual(2, labelsObject[4], "NAS must emit section-aware NOBJ v2 files.");
            Assert.AreEqual(3, labelsObject[10], "All three local labels must be represented as symbols.");
            Assert.AreEqual(1, labelsObject[14], "JSR SUB must produce one indexed ABS16 relocation.");
            byte[] labelsCode = labelsObject[32..46];
            CollectionAssert.AreEqual(new byte[]
            {
                0x80, 0x02, 0xDE, 0xAD, 0x20, 0x00, 0x00, 0x60,
                0xA9, 0x7E, 0x8D, 0x0E, 0xA0, 0x60
            }, labelsCode);

            byte[] labelsExecutable = disk.Load("LABELS", ".BIN");
            CollectionAssert.AreEqual(new byte[]
            {
                0x00, 0x70, 0x80, 0x02, 0xDE, 0xAD, 0x20, 0x08,
                0x70, 0x60, 0xA9, 0x7E, 0x8D, 0x0E, 0xA0, 0x60
            }, labelsExecutable, "NL must resolve local symbols against the configured load address.");

            QueueLine(editor, "RUN LABELS.BIN");
            RunUntil(cpu, bus, s => s.Contains('~'), "generic linked program output");

            disk.Save("CHEAPLOC", Encoding.ASCII.GetBytes(
                ".export Start\nStart:\nldx #$02\n@loop:\ndex\nbne @LoOp\njsr Next\nrts\n" +
                "Next:\nldy #$02\n@loop:\ndey\nbne @LOOP\nlda #'@'\nsta $A00E\nrts\n"), ".S");
            QueueLine(editor, "ASSEMBLE CHEAPLOC.S");
            RunUntil(cpu, bus, s => s.Contains("Writing CHEAPLOC.OBJ", StringComparison.Ordinal)
                                    || s.Contains(": error:", StringComparison.Ordinal),
                "cheap-local-label assembly");
            RunSteps(cpu, bus, 500_000);
            byte[] cheapLocalObject = disk.Load("CHEAPLOC", ".OBJ");
            Assert.AreEqual(4, cheapLocalObject[10],
                "Repeated @loop labels must become distinct symbols under Start and Next.");
            CollectionAssert.AreEqual(new byte[]
            {
                0xA2, 0x02, 0xCA, 0xD0, 0xFD, 0x20, 0x00, 0x00, 0x60,
                0xA0, 0x02, 0x88, 0xD0, 0xFD, 0xA9, 0x40, 0x8D, 0x0E, 0xA0, 0x60
            }, ReadNobjSectionData(cheapLocalObject, "CODE"),
                "NAS cheap-local references must remain case-insensitive within their global scope.");
            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);
            QueueLine(editor, "LINK CHEAPLOC.OBJ");
            RunUntil(cpu, bus, s => s.Contains("Link successful", StringComparison.Ordinal)
                                    || s.Contains("Linker error", StringComparison.Ordinal),
                "cheap-local-label link");
            CollectionAssert.AreEqual(new byte[]
            {
                0x00, 0x70,
                0xA2, 0x02, 0xCA, 0xD0, 0xFD, 0x20, 0x09, 0x70, 0x60,
                0xA0, 0x02, 0x88, 0xD0, 0xFD, 0xA9, 0x40, 0x8D, 0x0E, 0xA0, 0x60
            }, disk.Load("CHEAPLOC", ".BIN"));
            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);
            QueueLine(editor, "RUN CHEAPLOC.BIN");
            RunUntil(cpu, bus, s => s.Contains('@'), "cheap-local-label executable output");

            disk.Save("BYTESTR", Encoding.ASCII.GetBytes(
                ".export Start\nStart:\nbra @after\n" +
                ".byte \"Nova\\n\",0,\"\\\"\",\"\\\\\",$FF\n" +
                "@after:\nlda #'^'\nsta $A00E\nrts\n"), ".S");
            QueueLine(editor, "ASSEMBLE BYTESTR.S");
            RunUntil(cpu, bus, s => s.Contains("Writing BYTESTR.OBJ", StringComparison.Ordinal)
                                    || s.Contains(": error:", StringComparison.Ordinal),
                ".BYTE string-literal assembly");
            RunSteps(cpu, bus, 500_000);
            byte[] byteStringObject = disk.Load("BYTESTR", ".OBJ");
            CollectionAssert.AreEqual(new byte[]
            {
                0x80, 0x09,
                0x4E, 0x6F, 0x76, 0x61, 0x0A, 0x00, 0x22, 0x5C, 0xFF,
                0xA9, 0x5E, 0x8D, 0x0E, 0xA0, 0x60
            }, ReadNobjSectionData(byteStringObject, "CODE"),
                ".BYTE strings must preserve case, decode escapes, and mix with expressions.");
            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);
            QueueLine(editor, "LINK BYTESTR.OBJ");
            RunUntil(cpu, bus, s => s.Contains("Link successful", StringComparison.Ordinal)
                                    || s.Contains("Linker error", StringComparison.Ordinal),
                ".BYTE string-literal link");
            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);
            QueueLine(editor, "RUN BYTESTR.BIN");
            RunUntil(cpu, bus, s => s.Contains('^'), ".BYTE string-literal executable output");

            disk.Save("ANON", Encoding.ASCII.GetBytes(
                ".export Start\nStart:\nldx #$02\n:\ndex\nbne :-\nbra :++\n" +
                ":\n.byte $DE,$AD\n:\nldy #$03\n:\ndey\nbne :-\nbra :+\n" +
                ":\nlda #'!'\nsta $A00E\nrts\n"), ".S");
            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);
            QueueLine(editor, "ASSEMBLE ANON.S");
            RunUntil(cpu, bus, s => s.Contains("Writing ANON.OBJ", StringComparison.Ordinal)
                                    || s.Contains(": error:", StringComparison.Ordinal),
                "anonymous-label assembly");
            RunSteps(cpu, bus, 500_000);
            Assert.IsTrue(disk.FileExists("ANON", ".OBJ"), Snapshot(bus));
            byte[] anonymousObject = disk.Load("ANON", ".OBJ");
            Assert.AreEqual(6, anonymousObject[10],
                "Each anonymous definition must become a distinct local NOBJ symbol.");
            CollectionAssert.AreEqual(new byte[]
            {
                0xA2, 0x02, 0xCA, 0xD0, 0xFD, 0x80, 0x02, 0xDE, 0xAD,
                0xA0, 0x03, 0x88, 0xD0, 0xFD, 0x80, 0x00,
                0xA9, 0x21, 0x8D, 0x0E, 0xA0, 0x60
            }, ReadNobjSectionData(anonymousObject, "CODE"),
                "NAS must resolve :-, :+, and :++ through ordinary checked branches.");
            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);
            QueueLine(editor, "LINK ANON.OBJ");
            RunUntil(cpu, bus, s => s.Contains("Link successful", StringComparison.Ordinal)
                                    || s.Contains("Linker error", StringComparison.Ordinal),
                "anonymous-label link");
            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);
            QueueLine(editor, "RUN ANON.BIN");
            RunUntil(cpu, bus, s => s.Contains('!'), "anonymous-label executable output");

            disk.Save("ASSERTOK", Encoding.ASCII.GetBytes(
                ".export Start\nEnabled = 1\n.aSsErT eNaBlEd\n" +
                "Start:\nlda #'^'\nsta $A00E\nrts\n"), ".S");
            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);
            QueueLine(editor, "ASSEMBLE ASSERTOK.S");
            RunUntil(cpu, bus, s => s.Contains("Assembly successful", StringComparison.Ordinal)
                                    || s.Contains(": error:", StringComparison.Ordinal),
                "passing assembly assertion");
            Assert.IsTrue(disk.FileExists("ASSERTOK", ".OBJ"), Snapshot(bus));
            CollectionAssert.AreEqual(new byte[]
            {
                0xA9, 0x5E, 0x8D, 0x0E, 0xA0, 0x60
            }, ReadNobjSectionData(disk.Load("ASSERTOK", ".OBJ"), "CODE"),
                ".ASSERT must be case-insensitive and emit no bytes when true.");
            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);
            QueueLine(editor, "LINK ASSERTOK.OBJ");
            RunUntil(cpu, bus, s => s.Contains("Link successful", StringComparison.Ordinal)
                                    || s.Contains("Linker error", StringComparison.Ordinal),
                "assertion-bearing object link");
            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);
            QueueLine(editor, "RUN ASSERTOK.BIN");
            RunUntil(cpu, bus, s => s.Contains('^'), "assertion-bearing executable output");

            disk.Save("ASSERTBAD", Encoding.ASCII.GetBytes(".ASSERT 0\nrts\n"), ".S");
            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);
            QueueLine(editor, "ASSEMBLE ASSERTBAD.S");
            RunUntil(cpu, bus, s => s.Contains("assertion failed.", StringComparison.Ordinal),
                "failing assembly assertion diagnostic");
            StringAssert.Contains(Snapshot(bus), "ASSERTBAD.S:1:");
            Assert.IsFalse(disk.FileExists("ASSERTBAD", ".OBJ"),
                "A failed assembly assertion must not leave an object file.");
            RunUntil(cpu, bus, s => s.Contains("NP> ", StringComparison.Ordinal),
                "shell after failed assembly assertion");
            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);

            disk.Save("MULTIMAIN", Encoding.ASCII.GetBytes(
                ".import Helper\nStart:\njsr hElPeR\nrts\n"), ".S");
            QueueLine(editor, "ASSEMBLE MULTIMAIN.S");
            RunUntil(cpu, bus, s => s.Contains("Writing MULTIMAIN.OBJ", StringComparison.Ordinal),
                "multi-object main assembly");
            RunSteps(cpu, bus, 100_000);

            disk.Save("MULTIUTIL", Encoding.ASCII.GetBytes(
                ".export helper\nHELPER:\nlda #'M'\nsta $A00E\nrts\n"), ".S");
            QueueLine(editor, "ASSEMBLE MULTIUTIL.S");
            RunUntil(cpu, bus, s => s.Contains("Writing MULTIUTIL.OBJ", StringComparison.Ordinal),
                "multi-object utility assembly");
            RunSteps(cpu, bus, 100_000);

            disk.Save("MULTIPRJ", Encoding.ASCII.GetBytes(
                "NPP 1\nMAIN HELLO.PAS\nOUTPUT MULTIPRJ.BIN\nOBJECT MULTIUTIL.OBJ\n"), ".NPP");
            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);
            QueueLine(editor, "BUILD MULTIPRJ.NPP");
            RunUntil(cpu, bus, s => s.Contains("Build complete: MULTIPRJ.BIN", StringComparison.Ordinal)
                                    || s.Contains("Linker error", StringComparison.Ordinal),
                "project multi-object build result");
            string multiProjectScreen = Snapshot(bus);
            StringAssert.Contains(multiProjectScreen, "Linking MULTIUTIL.OBJ");
            Assert.IsTrue(disk.FileExists("MULTIPRJ", ".BIN"), multiProjectScreen);
            Assert.IsTrue(ContainsSequence(disk.Load("MULTIPRJ", ".BIN"),
                    new byte[] { 0xA9, 0x4D, 0x8D, 0x0E, 0xA0, 0x60 }),
                "NPP OBJECT must pass an ordinary assembly object through NL's shared object path.");
            RunSteps(cpu, bus, 100_000);

            disk.Save("PRJUTIL", Encoding.ASCII.GetBytes(
                ".export ProjectHelper\nProjectHelper:\nlda #ProjectChar\nsta $A00E\nrts\n"), ".S");
            disk.Save("ASMPROJ", Encoding.ASCII.GetBytes(
                "NPP 1\nMAIN HELLO.PAS\nOUTPUT ASMPROJ.BIN\n" +
                "DEFINE projectchar=$50\nASM PRJUTIL.S\n"), ".NPP");
            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);
            QueueLine(editor, "BUILD ASMPROJ.NPP");
            RunUntil(cpu, bus, s => s.Contains("Build complete: ASMPROJ.BIN", StringComparison.Ordinal)
                                    || s.Contains(": error:", StringComparison.Ordinal)
                                    || s.Contains("Linker error", StringComparison.Ordinal),
                "project assembly-source build result");
            string asmProjectScreen = Snapshot(bus);
            StringAssert.Contains(asmProjectScreen, "Assembling PRJUTIL.S");
            StringAssert.Contains(asmProjectScreen, "Linking PRJUTIL.OBJ");
            Assert.IsTrue(disk.FileExists("PRJUTIL", ".OBJ"), asmProjectScreen);
            Assert.IsTrue(ContainsSequence(disk.Load("ASMPROJ", ".BIN"),
                    new byte[] { 0xA9, 0x50, 0x8D, 0x0E, 0xA0, 0x60 }),
                "NPP ASM must invoke NAS and pass its output through NL's shared object path.");
            RunSteps(cpu, bus, 100_000);

            QueueLine(editor, "LINK MULTIMAIN.OBJ MULTIUTIL.OBJ");
            RunUntil(cpu, bus, s => s.Contains("Writing MULTIMAIN.BIN", StringComparison.Ordinal)
                                    || s.Contains("Linker error", StringComparison.Ordinal),
                "multi-object link result");
            RunSteps(cpu, bus, 100_000);
            byte[] multiExecutable = disk.Load("MULTIMAIN", ".BIN");
            CollectionAssert.AreEqual(new byte[]
            {
                0x00, 0x70, 0x20, 0x04, 0x70, 0x60,
                0xA9, 0x4D, 0x8D, 0x0E, 0xA0, 0x60
            }, multiExecutable,
                "NL must place ordered objects and resolve case-insensitive globals across them.");
            QueueLine(editor, "RUN MULTIMAIN.BIN");
            RunUntil(cpu, bus, s => s.Contains('M'), "multi-object executable output");

            disk.Save("RELOCMAIN", Encoding.ASCII.GetBytes(
                ".import target\n.byte target-$7000,<target,>target\n" +
                ".word target\nlda #<target\nldx #>target\nlda <target\nbra target\n"), ".S");
            QueueLine(editor, "ASSEMBLE RELOCMAIN.S");
            RunUntil(cpu, bus, s => s.Contains("Writing RELOCMAIN.OBJ", StringComparison.Ordinal)
                                    || s.Contains(": error:", StringComparison.Ordinal),
                "all relocation types assembly result");
            RunSteps(cpu, bus, 100_000);

            disk.Save("RELOCTGT", Encoding.ASCII.GetBytes(
                ".export target\nTarget:\nrts\n"), ".S");
            QueueLine(editor, "ASSEMBLE RELOCTGT.S");
            RunUntil(cpu, bus, s => s.Contains("Writing RELOCTGT.OBJ", StringComparison.Ordinal)
                                    || s.Contains(": error:", StringComparison.Ordinal),
                "relocation target assembly result");
            RunSteps(cpu, bus, 100_000);

            byte[] relocationObject = disk.Load("RELOCMAIN", ".OBJ");
            int relocationOffset = relocationObject[16] | relocationObject[17] << 8;
            Assert.AreEqual(8, relocationObject[14] | relocationObject[15] << 8);
            CollectionAssert.AreEqual(new byte[] { 2, 3, 4, 1, 3, 4, 3, 5 }, new byte[]
            {
                relocationObject[relocationOffset + 1], relocationObject[relocationOffset + 9],
                relocationObject[relocationOffset + 17], relocationObject[relocationOffset + 25],
                relocationObject[relocationOffset + 33], relocationObject[relocationOffset + 41],
                relocationObject[relocationOffset + 49], relocationObject[relocationOffset + 57]
            }, "NAS must emit each NOBJ relocation type from ordinary source syntax.");

            QueueLine(editor, "LINK RELOCMAIN.OBJ RELOCTGT.OBJ");
            RunUntil(cpu, bus, s => s.Contains("Writing RELOCMAIN.BIN", StringComparison.Ordinal)
                                    || s.Contains("Linker error", StringComparison.Ordinal),
                "all relocation types link result");
            RunSteps(cpu, bus, 100_000);
            CollectionAssert.AreEqual(new byte[]
            {
                0x00,0x70, 0x0D,0x0D,0x70,0x0D,0x70,
                0xA9,0x0D,0xA2,0x70,0xA5,0x0D,0x80,0x00,0x60
            }, disk.Load("RELOCMAIN", ".BIN"),
                "NL must apply ABS8, LO8, HI8, ABS16, and REL8 relocations.");

            disk.Save("BADREL", Encoding.ASCII.GetBytes(
                ".import target\n.byte target-$7000,<target,>target\n" +
                ".word target\nlda #<target\nldx #>target\nlda <target\n" +
                "bra target\n.res 187\n"), ".S");
            QueueLine(editor, "ASSEMBLE BADREL.S");
            RunUntil(cpu, bus, s => s.Contains("Writing BADREL.OBJ", StringComparison.Ordinal)
                                    || s.Contains(": error:", StringComparison.Ordinal),
                "distant branch assembly result");
            RunSteps(cpu, bus, 100_000);
            QueueLine(editor, "LINK BADREL.OBJ RELOCTGT.OBJ");
            RunUntil(cpu, bus, s => s.Contains("Linker error.", StringComparison.Ordinal),
                "out-of-range REL8 rejection");
            RunSteps(cpu, bus, 100_000);
            Assert.IsFalse(disk.FileExists("BADREL", ".BIN"),
                "NL must reject rather than truncate an out-of-range relative relocation.");

            disk.Save("MACRO", Encoding.ASCII.GetBytes(
                ".mAcRo Emit value, address\n" +
                "lda #value\nsta address\n.eNdMaCrO\n" +
                ".If 0\n.byte $FF\n.eLsE\n.iFdEf eMiT\n.IfNdEf Missing\n" +
                "eMiT $4D, $A00E\n.eNdIf\n.EnDiF\n.eNdIf\nrts\n"), ".S");
            QueueLine(editor, "ASSEMBLE MACRO.S");
            RunUntil(cpu, bus, s => s.Contains("Writing MACRO.OBJ", StringComparison.Ordinal)
                                    || s.Contains(": error:", StringComparison.Ordinal),
                "macro and conditional assembly result");
            RunSteps(cpu, bus, 500_000);
            byte[] macroObject = disk.Load("MACRO", ".OBJ");
            CollectionAssert.AreEqual(new byte[] { 0xA9, 0x4D, 0x8D, 0x0E, 0xA0, 0x60 },
                macroObject[32..38],
                "NAS preprocessing must expand case-insensitive parameterized macros and conditionals.");

            disk.Save("IFEXPR", Encoding.ASCII.GetBytes(
                ".define Feature 2+3\n" +
                ".define AliasA (.NoT 0) .aNd (1 .xOr 0) .and (($F0 .bItOr $0F) = $FF)\n" +
                ".define AliasB (($FF .BitAnd $0F) = $0F) .AND (($AA .BitXor $FF) = $55)\n" +
                ".define AliasC ((1 .ShL 4) = 16) .AND ((16 .sHr 2) = 4) .AND ((10 .MoD 3) = 1)\n" +
                ".define AliasD ((.BiTnOt $FF00 .BITAND $FFFF) = $00FF) .AND " +
                "(.LoByTe $1234 = $34) .AND (.HiByTe $1234 = $12)\n" +
                ".If (fEaTuRe * 3 = 15) && (($12 << 1) = $24) && !0 && " +
                "(100 / 9 = 11) && (100 % 9 = 1) && ('A' < 'B')\n" +
                ".if (($F0 | $0F) = $FF) && (($FF & $0F) = $0F) && " +
                "(($AA ^ $FF) = $55) && ((~$FF00 & $FFFF) = $00FF) && " +
                "(<$1234 = $34) && (>$1234 = $12) && (8 >> 2 = 2)\n" +
                ".if AliasA\n.if AliasB\n.if AliasC\n.if AliasD\n" +
                "lda #'E'\n.endif\n.endif\n.endif\n.endif\n" +
                ".else\nlda #'Y'\n.endif\n" +
                ".else\nlda #'X'\n.endif\nsta $A00E\nrts\n"), ".S");
            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);
            QueueLine(editor, "ASSEMBLE IFEXPR.S");
            RunUntil(cpu, bus, s => s.Contains("Writing IFEXPR.OBJ", StringComparison.Ordinal)
                                    || s.Contains(": error:", StringComparison.Ordinal),
                "conditional-expression assembly result");
            RunSteps(cpu, bus, 500_000);
            Assert.IsTrue(disk.FileExists("IFEXPR", ".OBJ"), Snapshot(bus));
            CollectionAssert.AreEqual(new byte[] { 0xA9, 0x45, 0x8D, 0x0E, 0xA0, 0x60 },
                ReadNobjSectionData(disk.Load("IFEXPR", ".OBJ"), "CODE"),
                "NAS .IF expressions must honor definition expansion and operator precedence.");
            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);
            QueueLine(editor, "LINK IFEXPR.OBJ");
            RunUntil(cpu, bus, s => s.Contains("Link successful", StringComparison.Ordinal)
                                    || s.Contains("Linker error", StringComparison.Ordinal),
                "conditional-expression link");
            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);
            QueueLine(editor, "RUN IFEXPR.BIN");
            RunUntil(cpu, bus, s => s.Contains('E'), "conditional-expression executable output");

            disk.Save("ELSEIF", Encoding.ASCII.GetBytes(
                ".define Mode 2\n" +
                ".if mode = 1\nlda #'1'\n" +
                ".eLsEiF mOdE = 2\nlda #'2'\n" +
                ".elseif 1\nlda #'3'\n" +
                ".else\nlda #'X'\n.endif\nsta $A00E\nrts\n"), ".S");
            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);
            QueueLine(editor, "ASSEMBLE ELSEIF.S");
            RunUntil(cpu, bus, s => s.Contains("Writing ELSEIF.OBJ", StringComparison.Ordinal)
                                    || s.Contains(": error:", StringComparison.Ordinal),
                "else-if assembly result");
            RunSteps(cpu, bus, 500_000);
            Assert.IsTrue(disk.FileExists("ELSEIF", ".OBJ"), Snapshot(bus));
            CollectionAssert.AreEqual(new byte[] { 0xA9, 0x32, 0x8D, 0x0E, 0xA0, 0x60 },
                ReadNobjSectionData(disk.Load("ELSEIF", ".OBJ"), "CODE"),
                "NAS .ELSEIF must select only the first true branch, case-insensitively.");
            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);
            QueueLine(editor, "LINK ELSEIF.OBJ");
            RunUntil(cpu, bus, s => s.Contains("Link successful", StringComparison.Ordinal)
                                    || s.Contains("Linker error", StringComparison.Ordinal),
                "else-if link");
            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);
            QueueLine(editor, "RUN ELSEIF.BIN");
            RunUntil(cpu, bus, s => s.Contains('2'), "else-if executable output");

            disk.Save("BADMAC", Encoding.ASCII.GetBytes(
                ".macro Emit value\nlda #value\n"), ".S");
            QueueLine(editor, "ASSEMBLE BADMAC.S");
            RunUntil(cpu, bus, s => s.Contains(
                    "BADMAC.S:2:1: error: syntax error.", StringComparison.Ordinal),
                "source-located preprocessing diagnostic");
            Assert.IsFalse(disk.FileExists("BADMAC", ".OBJ"));

            disk.Save("DEFINE", Encoding.ASCII.GetBytes(
                ".dEfInE Value $2A\n.iFdEf vAlUe\nlda #VALUE\n.eNdIf\n" +
                ".uNdEfInE VALUE\n.IfNdEf value\nldx #$11\n.eNdIf\nrts\n"), ".S");
            QueueLine(editor, "ASSEMBLE DEFINE.S");
            RunUntil(cpu, bus, s => s.Contains("Writing DEFINE.OBJ", StringComparison.Ordinal),
                "preprocessor definition assembly result");
            RunSteps(cpu, bus, 100_000);
            byte[] defineObject = disk.Load("DEFINE", ".OBJ");
            CollectionAssert.AreEqual(new byte[] { 0xA9, 0x2A, 0xA2, 0x11, 0x60 },
                defineObject[32..37],
                "NAS .DEFINE/.UNDEFINE substitution and condition lookup must be case-insensitive.");

            disk.Save("CMDDEF", Encoding.ASCII.GetBytes(
                "lda #BuildValue\nsta $A00E\nrts\n"), ".S");
            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);
            QueueLine(editor, "ASSEMBLE CMDDEF.S -o CUSTOM.OBJ -DbUiLdVaLuE=$2B");
            RunUntil(cpu, bus, s => s.Contains("Writing CUSTOM.OBJ", StringComparison.Ordinal)
                                    || s.Contains(": error:", StringComparison.Ordinal),
                "command-line output and definition assembly result");
            RunSteps(cpu, bus, 100_000);
            CollectionAssert.AreEqual(new byte[] { 0xA9, 0x2B, 0x8D, 0x0E, 0xA0, 0x60 },
                disk.Load("CUSTOM", ".OBJ")[32..38],
                "NAS -o and -D must compose while preserving case-insensitive definitions.");

            disk.Save("TOOBIG", Encoding.ASCII.GetBytes(new string(' ', 3_073)), ".S");
            QueueLine(editor, "ASSEMBLE TOOBIG.S");
            RunUntil(cpu, bus, s => s.Contains("File is too large for this command.", StringComparison.Ordinal),
                "oversized assembly source rejection");

            disk.Save("EXPRESS", Encoding.ASCII.GetBytes(
                "Base:\n" +
                ".byte 1+2*3,(1+2)*3,$F0|%00001111,'A',<$1234,>$1234,100/9,100%9\n" +
                ".word 10+6*5,$1000+($20<<1),-1,~$00FF,Base+2\n" +
                ".byte .NoT 0,1 .xOr 0,$F0 .BiToR $0F,$FF .bItAnD $0F,$AA .BiTxOr $FF\n" +
                ".byte 1 .sHl 4,16 .ShR 2,10 .mOd 3,.LoByTe $1234,.HiByTe $1234\n" +
                ".word .BiTnOt $FF00\n" +
                ".byte 5=5,5<>4,4<5,4<=4,5>4,5>=5,1 .AnD 2,0 .oR 3\n" +
                "lda #(1<<4)|3\nsta $1000+2\nrts\n"), ".S");
            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);
            QueueLine(editor, "ASSEMBLE EXPRESS.S");
            RunUntil(cpu, bus, s => s.Contains("Writing EXPRESS.OBJ", StringComparison.Ordinal)
                                    || s.Contains(": error:", StringComparison.Ordinal),
                "expression assembly result");
            RunSteps(cpu, bus, 500_000);
            Assert.IsTrue(Snapshot(bus).Contains("Assembly successful", StringComparison.Ordinal),
                $"NAS expression assembly failed with detail ${bus.Read(0x0276):X2}:\n{Snapshot(bus)}");
            byte[] expressionObject = disk.Load("EXPRESS", ".OBJ");
            CollectionAssert.AreEqual(new byte[]
            {
                0x07, 0x09, 0xFF, 0x41, 0x34, 0x12, 0x0B, 0x01,
                0x28, 0x00, 0x40, 0x10, 0xFF, 0xFF, 0x00, 0xFF,
                0x00, 0x00,
                0x01, 0x01, 0xFF, 0x0F, 0x55,
                0x10, 0x04, 0x01, 0x34, 0x12, 0xFF, 0x00,
                0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
                0xA9, 0x13, 0x8D, 0x02, 0x10, 0x60
            }, expressionObject[32..76],
                "NAS must evaluate symbolic and spelled operators with ca65-style precedence.");
            int expressionReloc = expressionObject[16] | (expressionObject[17] << 8);
            CollectionAssert.AreEqual(new byte[] { 0, 1, 16, 0, 0, 0, 2, 0 },
                expressionObject[expressionReloc..(expressionReloc + 8)],
                "A symbolic expression must preserve its constant relocation addend.");

            QueueLine(editor, "LINK EXPRESS.OBJ");
            RunUntil(cpu, bus, s => s.Contains("Writing EXPRESS.BIN", StringComparison.Ordinal)
                                    || s.Contains("Linker error", StringComparison.Ordinal),
                "expression link result");
            RunSteps(cpu, bus, 500_000);
            byte[] expressionExecutable = disk.Load("EXPRESS", ".BIN");
            Assert.AreEqual(0x02, expressionExecutable[18]);
            Assert.AreEqual(0x70, expressionExecutable[19],
                "NL must apply the symbolic expression addend at the configured load address.");

            disk.Save("CONSTDEF", Encoding.ASCII.GetBytes(
                ".export Screen\nsCrEeN = $A000 + $0E\nrts\n"), ".S");
            QueueLine(editor, "ASSEMBLE CONSTDEF.S");
            RunUntil(cpu, bus, s => s.Contains("Writing CONSTDEF.OBJ", StringComparison.Ordinal),
                "absolute-symbol definition assembly");
            RunSteps(cpu, bus, 100_000);
            byte[] constantDefinition = disk.Load("CONSTDEF", ".OBJ");
            int constantSymbol = constantDefinition[12] | constantDefinition[13] << 8;
            CollectionAssert.AreEqual(new byte[] { 0x0E, 0xA0, 0xFE, 0x01 },
                constantDefinition[constantSymbol..(constantSymbol + 4)],
                "Exported assignments must emit global absolute NOBJ symbols.");

            disk.Save("CONSTUSE", Encoding.ASCII.GetBytes(
                ".import screen\nCount = Next+1\nNext = Base+1\nBase = 5\n" +
                "lda #cOuNt\nlda #'^'\nsta SCREEN\nrts\n"), ".S");
            QueueLine(editor, "ASSEMBLE CONSTUSE.S");
            RunUntil(cpu, bus, s => s.Contains("Writing CONSTUSE.OBJ", StringComparison.Ordinal),
                "case-insensitive named-constant assembly");
            RunSteps(cpu, bus, 100_000);
            CollectionAssert.AreEqual(new byte[]
            {
                0xA9, 0x00, 0xA9, 0x5E, 0x8D, 0x00, 0x00, 0x60
            }, disk.Load("CONSTUSE", ".OBJ")[32..40],
                "NAS must keep forward constants relocatable so instruction sizing stays stable.");
            Assert.AreEqual(2, disk.Load("CONSTUSE", ".OBJ")[14],
                "Forward absolute and imported absolute operands must both reach NL as relocations.");

            disk.Save("CONSTCYC", Encoding.ASCII.GetBytes(
                "First = Second+1\nSecond = First+1\nlda #First\nrts\n"), ".S");
            QueueLine(editor, "ASSEMBLE CONSTCYC.S");
            RunUntil(cpu, bus, s => s.Contains("CONSTCYC.S:", StringComparison.Ordinal),
                "cyclic constant rejection");
            Assert.AreEqual(3, bus.Read(0x0276),
                "NAS must fail unresolved assignment cycles as symbol errors.");
            Assert.IsFalse(disk.FileExists("CONSTCYC", ".OBJ"));

            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);
            QueueLine(editor, "LINK CONSTUSE.OBJ CONSTDEF.OBJ");
            RunUntil(cpu, bus, s => s.Contains("Writing CONSTUSE.BIN", StringComparison.Ordinal)
                                    || s.Contains("Linker error", StringComparison.Ordinal),
                "absolute-symbol cross-object link result");
            Assert.IsTrue(disk.FileExists("CONSTUSE", ".BIN"), Snapshot(bus));
            CollectionAssert.AreEqual(new byte[]
            {
                0x00, 0x70, 0xA9, 0x07, 0xA9, 0x5E, 0x8D, 0x0E, 0xA0, 0x60, 0x60
            }, disk.Load("CONSTUSE", ".BIN"),
                "NL must resolve exported absolute symbols without rebasing their values.");
            QueueLine(editor, "RUN CONSTUSE.BIN");
            RunUntil(cpu, bus, s => s.Contains('^'), "absolute-symbol executable output");
            RunSteps(cpu, bus, 100_000);

            disk.Save("PUBLIC", Encoding.ASCII.GetBytes(
                ".iMpOrT p_write_char\n.eXpOrT sTaRt\nSTART:\nlda #'@'\njsr P_WRITE_CHAR\nrts\n"), ".S");
            QueueLine(editor, "ASSEMBLE PUBLIC.S");
            RunUntil(cpu, bus, s => s.Contains("Writing PUBLIC.OBJ", StringComparison.Ordinal)
                                    || s.Contains(": error:", StringComparison.Ordinal),
                "public symbol assembly result");
            RunSteps(cpu, bus, 500_000);
            byte[] publicObject = disk.Load("PUBLIC", ".OBJ");
            int publicSymbols = publicObject[12] | (publicObject[13] << 8);
            Assert.AreEqual(0xFF, publicObject[publicSymbols + 2]);
            Assert.AreEqual(1, publicObject[publicSymbols + 3],
                ".IMPORT must emit an undefined global symbol.");
            int startSymbol = publicSymbols + 5 + 12;
            Assert.AreEqual(0, publicObject[startSymbol + 2]);
            Assert.AreEqual(1, publicObject[startSymbol + 3],
                ".EXPORT must emit a section-defined global symbol.");

            QueueLine(editor, "LINK PUBLIC.OBJ");
            RunUntil(cpu, bus, s => s.Contains("Writing PUBLIC.BIN", StringComparison.Ordinal)
                                    || s.Contains("Linker error", StringComparison.Ordinal),
                "public symbol link result");
            RunSteps(cpu, bus, 500_000);
            QueueLine(editor, "RUN PUBLIC.BIN");
            RunUntil(cpu, bus, s => s.Contains('@'), "explicit import executable output");

            disk.Save("UNDECL", Encoding.ASCII.GetBytes("jsr P_WRITE_CHAR\nrts\n"), ".S");
            QueueLine(editor, "ASSEMBLE UNDECL.S");
            RunUntil(cpu, bus, s => s.Contains("UNDECL.S:", StringComparison.Ordinal),
                "undeclared external rejection");
            Assert.AreEqual(3, bus.Read(0x0276),
                "Undefined symbols must require an explicit .IMPORT declaration.");
            Assert.IsFalse(disk.FileExists("UNDECL", ".OBJ"));

            disk.Save("BROKEN", Encoding.ASCII.GetBytes(
                "lda #1\nsta $A00E\nNOPE\n"), ".S");
            QueueLine(editor, "ASSEMBLE BROKEN.S");
            RunUntil(cpu, bus, s => s.Contains(
                    "BROKEN.S:3:5: error: syntax error.", StringComparison.Ordinal),
                "source-located assembler diagnostic");
            Assert.IsFalse(disk.FileExists("BROKEN", ".OBJ"));

            disk.Save("LEVEL2", Encoding.ASCII.GetBytes(
                "Included:\nlda #'I'\nsta $A00E\nrts\n"), ".INC");
            disk.Save("LEVEL1", Encoding.ASCII.GetBytes(
                ".include \"LEVEL2.INC\"\n"), ".INC");
            disk.Save("INCLUDE", Encoding.ASCII.GetBytes(
                ".export Start\nStart:\njsr Included\nrts\n" +
                ".include \"LEVEL1.INC\"\n"), ".S");
            QueueLine(editor, "ASSEMBLE INCLUDE.S");
            RunUntil(cpu, bus, s => s.Contains("Writing INCLUDE.OBJ", StringComparison.Ordinal)
                                    || s.Contains(": error:", StringComparison.Ordinal),
                "nested include assembly result");
            RunSteps(cpu, bus, 500_000);
            byte[] includeObject = disk.Load("INCLUDE", ".OBJ");
            CollectionAssert.AreEqual(new byte[]
            {
                0x20, 0x00, 0x00, 0x60, 0xA9, 0x49, 0x8D, 0x0E, 0xA0, 0x60
            }, includeObject[32..42],
                ".INCLUDE must assemble nested XRAM sources into the current section.");
            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);
            QueueLine(editor, "LINK INCLUDE.OBJ");
            RunUntil(cpu, bus, s => s.Contains("Link successful", StringComparison.Ordinal)
                                    || s.Contains("Linker error", StringComparison.Ordinal),
                "nested include link result");
            byte[] includeExecutable = disk.Load("INCLUDE", ".BIN");
            CollectionAssert.AreEqual(new byte[]
            {
                0x00, 0x70, 0x20, 0x04, 0x70, 0x60,
                0xA9, 0x49, 0x8D, 0x0E, 0xA0, 0x60
            }, includeExecutable);
            QueueLine(editor, "RUN INCLUDE.BIN");
            RunUntil(cpu, bus, s => s.Contains('I'), "nested include executable output");

            disk.Save("BADINC", Encoding.ASCII.GetBytes("NOPE\n"), ".INC");
            disk.Save("BADMAIN", Encoding.ASCII.GetBytes(
                ".include \"BADINC.INC\"\n"), ".S");
            QueueLine(editor, "ASSEMBLE BADMAIN.S");
            RunUntil(cpu, bus, s => s.Contains(
                    "BADINC.INC:1:5: error: syntax error.", StringComparison.Ordinal),
                "included-source diagnostic");
            Assert.IsFalse(disk.FileExists("BADMAIN", ".OBJ"));

            byte[] rawAsset = { 0x00, 0xFF, 0x0D, 0x0A, 0x41 };
            disk.Save("ASSET", rawAsset, ".DAT");
            disk.Save("INCBIN", Encoding.ASCII.GetBytes(
                ".segment \"CODE\"\n.export Start\nStart:\nlda #'&'\nsta $A00E\nrts\n" +
                ".segment \"RODATA\"\n.iNcBiN \"ASSET.DAT\"\n"), ".S");
            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);
            QueueLine(editor, "ASSEMBLE INCBIN.S");
            RunUntil(cpu, bus, s => s.Contains("Writing INCBIN.OBJ", StringComparison.Ordinal)
                                    || s.Contains(": error:", StringComparison.Ordinal),
                "raw binary inclusion assembly result");
            RunSteps(cpu, bus, 100_000);
            Assert.IsTrue(disk.FileExists("INCBIN", ".OBJ"), Snapshot(bus));
            CollectionAssert.AreEqual(rawAsset,
                ReadNobjSectionData(disk.Load("INCBIN", ".OBJ"), "RODATA"),
                "NAS .INCBIN must preserve arbitrary binary bytes through its XRAM file callback.");
            QueueLine(editor, "LINK INCBIN.OBJ");
            RunUntil(cpu, bus, s => s.Contains("Writing INCBIN.BIN", StringComparison.Ordinal),
                "raw binary inclusion link result");
            QueueLine(editor, "RUN INCBIN.BIN");
            RunUntil(cpu, bus, s => s.Contains('&'), "raw binary inclusion executable output");
            RunSteps(cpu, bus, 100_000);

            disk.Save("STORAGE", Encoding.ASCII.GetBytes(
                ".byte $AA\n.align 4,$EE\n.byte $BB\n.res 3,$CC\n" +
                ".word $1234\n.align 16\n.byte $DD\n"), ".S");
            QueueLine(editor, "ASSEMBLE STORAGE.S");
            RunUntil(cpu, bus, s => s.Contains("Writing STORAGE.OBJ", StringComparison.Ordinal)
                                    || s.Contains(": error:", StringComparison.Ordinal),
                "storage directive assembly result");
            RunSteps(cpu, bus, 500_000);
            byte[] storageObject = disk.Load("STORAGE", ".OBJ");
            Assert.AreEqual(4, storageObject[21],
                "NAS must publish the strongest requested section alignment as log2.");
            CollectionAssert.AreEqual(new byte[]
            {
                0xAA, 0xEE, 0xEE, 0xEE, 0xBB, 0xCC, 0xCC, 0xCC,
                0x34, 0x12, 0, 0, 0, 0, 0, 0, 0xDD
            }, storageObject[32..49],
                ".RES and .ALIGN must emit deterministic fill bytes in both passes.");

            disk.Save("SECTIONS", Encoding.ASCII.GetBytes(
                ".SeGmEnT \"cOdE\"\n.eXpOrT Start\nSTART:\n" +
                "lda #$5A\nsta $A00E\nrts\n.word DataValue\n.word Buffer\n" +
                ".segment \"rodata\"\n.align 4\nDataValue:\n.byte $AA\n" +
                ".SEGMENT \"bss\"\nBuffer:\n.res 3\n.segment \"CODE\"\n"), ".S");
            QueueLine(editor, "ASSEMBLE SECTIONS.S");
            RunUntil(cpu, bus, s => s.Contains("Writing SECTIONS.OBJ", StringComparison.Ordinal)
                                    || s.Contains(": error:", StringComparison.Ordinal),
                "named-section assembly result");
            RunSteps(cpu, bus, 500_000);
            byte[] sectionsObject = disk.Load("SECTIONS", ".OBJ");
            Assert.AreEqual(3, sectionsObject[6],
                "NAS must merge case-insensitive .SEGMENT selections by name.");
            CollectionAssert.AreEqual(new byte[] { 3, 0, 4, 0, 10, 0, 10, 0 },
                sectionsObject[20..28], "CODE must be executable initialized storage.");
            CollectionAssert.AreEqual(new byte[] { 1, 2, 6, 0, 1, 0, 1, 0 },
                sectionsObject[42..50], "RODATA must retain its alignment and initialized byte.");
            CollectionAssert.AreEqual(new byte[] { 13, 0, 3, 0, 3, 0, 0, 0 },
                sectionsObject[57..65], "BSS must carry memory size without serialized data.");
            Assert.AreEqual(0xAA, sectionsObject[56]);

            QueueLine(editor, "LINK SECTIONS.OBJ");
            RunUntil(cpu, bus, s => s.Contains("Writing SECTIONS.BIN", StringComparison.Ordinal)
                                    || s.Contains("Linker error", StringComparison.Ordinal),
                "named-section link result");
            RunSteps(cpu, bus, 500_000);
            byte[] sectionsExecutable = disk.Load("SECTIONS", ".BIN");
            CollectionAssert.AreEqual(new byte[]
            {
                0x00, 0x70,
                0xA9, 0x5A, 0x8D, 0x0E, 0xA0, 0x60, 0x0C, 0x70, 0x0D, 0x70,
                0x00, 0x00, 0xAA, 0x00, 0x00, 0x00
            }, sectionsExecutable,
                "NL must align sections, relocate through their placement offsets, and materialize BSS.");
            QueueLine(editor, "RUN SECTIONS.BIN");
            RunUntil(cpu, bus, s => s.Contains('Z'), "named-section executable output");

            disk.Save("SECTIONS", Encoding.ASCII.GetBytes(
                "MEMORY {\n" +
                "  MAIN: start=$7000,size=$0010,file=%O;\n" +
                "  DATA: start=$7080,size=$0010,file=%O;\n" +
                "}\nSEGMENTS {\n" +
                "  CODE: load=MAIN,type=ro;\n" +
                "  RODATA: load=DATA,type=ro;\n" +
                "  BSS: load=DATA,type=bss;\n}\n"), ".CFG");
            QueueLine(editor, "link sections.obj -c sections.cfg -m sections.map -ln sections.lbl");
            RunUntil(cpu, bus, s => s.Contains("Writing sections.BIN", StringComparison.Ordinal)
                                    || s.Contains("Linker configuration error", StringComparison.Ordinal),
                "configured multi-region link result");
            RunSteps(cpu, bus, 500_000);
            byte[] configuredSections = disk.Load("SECTIONS", ".BIN");
            Assert.AreEqual(134, configuredSections.Length,
                "NL output must span through the last occupied configured region byte.");
            CollectionAssert.AreEqual(new byte[]
            {
                0x00, 0x70, 0xA9, 0x5A, 0x8D, 0x0E, 0xA0, 0x60, 0x80, 0x70, 0x81, 0x70
            }, configuredSections[..12],
                "NL must relocate CODE references to sections placed in another MEMORY region.");
            Assert.IsTrue(Array.TrueForAll(configuredSections[12..130], value => value == 0),
                "NL must zero deterministic gaps between configured MEMORY regions.");
            CollectionAssert.AreEqual(new byte[] { 0xAA, 0, 0, 0 }, configuredSections[130..],
                "NL must place initialized and zerofill sections in their configured region.");
            string sectionsMap = Encoding.ASCII.GetString(disk.Load("SECTIONS", ".MAP"));
            StringAssert.Contains(sectionsMap, "$7000 $000A CODE\n");
            StringAssert.Contains(sectionsMap, "$7080 $0001 RODATA\n");
            StringAssert.Contains(sectionsMap, "$7081 $0003 BSS\n");
            StringAssert.Contains(sectionsMap, "$7000 START\n");
            CollectionAssert.AreEqual(Encoding.ASCII.GetBytes("al 007000 .START\n"),
                disk.Load("SECTIONS", ".LBL"),
                "NL -Ln must emit VICE labels from the same live placed exports as its map.");

            disk.Save("OVERLAP", sectionsObject, ".OBJ");
            disk.Save("OVERLAP", Encoding.ASCII.GetBytes(
                "MEMORY { A:start=$7000,size=$0010; B:start=$7008,size=$0010; }\n" +
                "SEGMENTS { CODE:load=A; RODATA:load=B; BSS:load=B; }\n"), ".CFG");
            QueueLine(editor, "LINK OVERLAP.OBJ -C OVERLAP.CFG");
            RunUntil(cpu, bus, s => s.Contains("Linker configuration error.", StringComparison.Ordinal),
                "overlapping linker-region rejection");
            Assert.IsFalse(disk.FileExists("OVERLAP", ".BIN"),
                "NL must reject overlapping MEMORY regions before placing sections.");

            disk.Save("DEADSTRIP", Encoding.ASCII.GetBytes(
                ".import NeverThere\n.segment \"CODE\"\n.export Start\nStart:\njsr Used\nrts\n" +
                ".segment \"USED\"\nUsed:\nlda #'G'\nsta $A00E\nrts\n" +
                ".segment \"DEAD\"\njsr NeverThere\n.byte $DE,$AD,$BE,$EF\n" +
                ".segment \"KEPT\"\n.export PublicData\nPublicData:\n.byte $C3\n"), ".S");
            QueueLine(editor, "ASSEMBLE DEADSTRIP.S");
            RunUntil(cpu, bus, s => s.Contains("Writing DEADSTRIP.OBJ", StringComparison.Ordinal)
                                    || s.Contains(": error:", StringComparison.Ordinal),
                "dead-strip source assembly result");
            RunSteps(cpu, bus, 500_000);
            Assert.AreEqual(4, disk.Load("DEADSTRIP", ".OBJ")[6]);

            disk.Save("DEADSTRIP", Encoding.ASCII.GetBytes(
                "MEMORY { RAM:start=$7000,size=$0010; }\n" +
                "SEGMENTS { CODE:load=RAM; USED:load=RAM; KEPT:load=RAM; }\n"), ".CFG");
            QueueLine(editor, "LINK DEADSTRIP.OBJ -C DEADSTRIP.CFG -M DEADSTRIP.MAP");
            RunUntil(cpu, bus, s => s.Contains("Writing DEADSTRIP.BIN", StringComparison.Ordinal)
                                    || s.Contains("Linker error", StringComparison.Ordinal),
                "dead-strip link result");
            RunSteps(cpu, bus, 500_000);
            CollectionAssert.AreEqual(new byte[]
            {
                0x00, 0x70,
                0x20, 0x04, 0x70, 0x60,
                0xA9, 0x47, 0x8D, 0x0E, 0xA0, 0x60,
                0xC3
            }, disk.Load("DEADSTRIP", ".BIN"),
                "NL must compact reachable sections while retaining explicitly exported sections; " +
                "a stripped section must need neither a configuration rule nor import resolution.");
            string deadStripMap = Encoding.ASCII.GetString(disk.Load("DEADSTRIP", ".MAP"));
            StringAssert.Contains(deadStripMap, "$7000 $0004 CODE\n");
            StringAssert.Contains(deadStripMap, "$7004 $0006 USED\n");
            StringAssert.Contains(deadStripMap, "$700A $0001 KEPT\n");
            Assert.IsFalse(deadStripMap.Contains(" DEAD\n", StringComparison.Ordinal),
                "NL maps must omit sections removed by reachability GC.");
            StringAssert.Contains(deadStripMap, "$700A PUBLICDATA\n");
            QueueLine(editor, "RUN DEADSTRIP.BIN");
            RunUntil(cpu, bus, s => s.Contains('G'), "dead-stripped executable output");
            RunSteps(cpu, bus, 100_000);

            disk.Save("DUPA", Encoding.ASCII.GetBytes(
                ".export Collision\nCollision:\nrts\n"), ".S");
            QueueLine(editor, "ASSEMBLE DUPA.S");
            RunUntil(cpu, bus, s => s.Contains("Writing DUPA.OBJ", StringComparison.Ordinal),
                "first duplicate-global object assembly");
            RunSteps(cpu, bus, 100_000);

            disk.Save("DUPB", Encoding.ASCII.GetBytes(
                ".export cOlLiSiOn\nCOLLISION:\nrts\n"), ".S");
            QueueLine(editor, "ASSEMBLE DUPB.S");
            RunUntil(cpu, bus, s => s.Contains("Writing DUPB.OBJ", StringComparison.Ordinal),
                "second duplicate-global object assembly");
            RunSteps(cpu, bus, 100_000);

            QueueLine(editor, "LINK DUPA.OBJ DUPB.OBJ");
            RunUntil(cpu, bus, s => s.Contains("Linker error.", StringComparison.Ordinal),
                "case-insensitive duplicate-global rejection");
            Assert.IsFalse(disk.FileExists("DUPA", ".BIN"),
                "NL must reject duplicate case-insensitive exports even when no relocation references them.");

            disk.Save("CFGSYM", Encoding.ASCII.GetBytes(
                ".import device\n.export Start\nStart:\nlda #'^'\nsta DeViCe\nrts\n"), ".S");
            QueueLine(editor, "ASSEMBLE CFGSYM.S");
            RunUntil(cpu, bus, s => s.Contains("Writing CFGSYM.OBJ", StringComparison.Ordinal),
                "configured-symbol source assembly");
            RunSteps(cpu, bus, 100_000);
            disk.Save("CFGSYM", Encoding.ASCII.GetBytes(
                "MEMORY { RAM:start=$7000,size=$0010; }\n" +
                "SEGMENTS { CODE:load=RAM; }\n" +
                "SYMBOLS { dEvIcE:value=$A00E; }\n"), ".CFG");
            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);
            QueueLine(editor, "LINK CFGSYM.OBJ -C CFGSYM.CFG");
            RunUntil(cpu, bus, s => s.Contains("Writing CFGSYM.BIN", StringComparison.Ordinal)
                                    || s.Contains("Linker error", StringComparison.Ordinal),
                "configured absolute-symbol link");
            CollectionAssert.AreEqual(new byte[]
            {
                0x00, 0x70, 0xA9, 0x5E, 0x8D, 0x0E, 0xA0, 0x60
            }, disk.Load("CFGSYM", ".BIN"),
                "NL config symbols must resolve case-insensitively as absolute addresses without rebasing.");
            QueueLine(editor, "RUN CFGSYM.BIN");
            RunUntil(cpu, bus, s => s.Contains('^'), "configured-symbol executable output");

            disk.Save("CFGDUPE", Encoding.ASCII.GetBytes(
                ".export DEVICE\nDEVICE:\nrts\n"), ".S");
            QueueLine(editor, "ASSEMBLE CFGDUPE.S");
            RunUntil(cpu, bus, s => s.Contains("Writing CFGDUPE.OBJ", StringComparison.Ordinal),
                "config-collision object assembly");
            RunSteps(cpu, bus, 100_000);
            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);
            QueueLine(editor, "LINK CFGDUPE.OBJ -C CFGSYM.CFG");
            RunUntil(cpu, bus, s => s.Contains("Linker error.", StringComparison.Ordinal),
                "object/config duplicate-global rejection");
            Assert.IsFalse(disk.FileExists("CFGDUPE", ".BIN"),
                "NL must reject a symbol defined by both an object and linker configuration.");

            disk.Save("IMPLIED", Encoding.ASCII.GetBytes(
                "BRK\nCLC\nCLD\nCLI\nCLV\nDEA\nDEX\nDEY\nINA\nINX\nINY\nNOP\n" +
                "PHA\nPHP\nPHX\nPHY\nPLA\nPLP\nPLX\nPLY\nRTI\nRTS\nSEC\nSED\n" +
                "SEI\nSTP\nTAX\nTAY\nTSX\nTXA\nTXS\nTYA\nWAI\n"), ".S");
            QueueLine(editor, "ASSEMBLE IMPLIED.S");
            RunUntil(cpu, bus, s => s.Contains("Writing IMPLIED.OBJ", StringComparison.Ordinal),
                "complete implied-opcode assembly result");
            RunSteps(cpu, bus, 100_000);
            byte[] impliedObject = disk.Load("IMPLIED", ".OBJ");
            CollectionAssert.AreEqual(new byte[]
            {
                0x00, 0x18, 0xD8, 0x58, 0xB8, 0x3A, 0xCA, 0x88,
                0x1A, 0xE8, 0xC8, 0xEA, 0x48, 0x08, 0xDA, 0x5A,
                0x68, 0x28, 0xFA, 0x7A, 0x40, 0x60, 0x38, 0xF8,
                0x78, 0xDB, 0xAA, 0xA8, 0xBA, 0x8A, 0x9A, 0x98,
                0xCB
            }, impliedObject[32..65], "NAS must cover every implied W65C02 mnemonic.");

            disk.Save("IMMEDIATE", Encoding.ASCII.GetBytes(
                "adc #$01\nAnD #$02\nBIT #$03\ncmp #$04\nCPx #$05\ncpy #$06\n" +
                "EOR #$07\nlda #$08\nLdX #$09\nLDY #$0A\nora #$0B\nSbC #$0C\n"), ".S");
            QueueLine(editor, "ASSEMBLE IMMEDIATE.S");
            RunUntil(cpu, bus, s => s.Contains("Writing IMMEDIATE.OBJ", StringComparison.Ordinal),
                "complete immediate-opcode assembly result");
            RunSteps(cpu, bus, 100_000);
            byte[] immediateObject = disk.Load("IMMEDIATE", ".OBJ");
            CollectionAssert.AreEqual(new byte[]
            {
                0x69, 0x01, 0x29, 0x02, 0x89, 0x03, 0xC9, 0x04,
                0xE0, 0x05, 0xC0, 0x06, 0x49, 0x07, 0xA9, 0x08,
                0xA2, 0x09, 0xA0, 0x0A, 0x09, 0x0B, 0xE9, 0x0C
            }, immediateObject[32..56],
                "NAS must encode every immediate W65C02 mnemonic case-insensitively.");

            disk.Save("ACCUM", Encoding.ASCII.GetBytes(
                "asl a\nDec A\nINC a\nLsR A\nrol a\nRoR A\n"), ".S");
            QueueLine(editor, "ASSEMBLE ACCUM.S");
            RunUntil(cpu, bus, s => s.Contains("Writing ACCUM.OBJ", StringComparison.Ordinal),
                "complete accumulator-opcode assembly result");
            RunSteps(cpu, bus, 500_000);
            byte[] accumulatorObject = disk.Load("ACCUM", ".OBJ");
            CollectionAssert.AreEqual(new byte[] { 0x0A, 0x3A, 0x1A, 0x4A, 0x2A, 0x6A },
                accumulatorObject[32..38],
                "NAS must encode every accumulator W65C02 mnemonic case-insensitively.");

            disk.Save("DIRECT", Encoding.ASCII.GetBytes(
                "ADC $12\nAND $12\nASL $12\nBIT $12\nCMP $12\nCPX $12\nCPY $12\nDEC $12\n" +
                "EOR $12\nINC $12\nLDA $12\nLDX $12\nLDY $12\nLSR $12\nORA $12\nROL $12\n" +
                "ROR $12\nSBC $12\nSTA $12\nSTX $12\nSTY $12\nSTZ $12\nTRB $12\nTSB $12\n" +
                "ADC $12,X\nAND $12,X\nASL $12,X\nBIT $12,X\nCMP $12,X\nDEC $12,X\n" +
                "EOR $12,X\nINC $12,X\nLDA $12,X\nLDY $12,X\nLSR $12,X\nORA $12,X\n" +
                "ROL $12,X\nROR $12,X\nSBC $12,X\nSTA $12,X\nSTY $12,X\nSTZ $12,X\n" +
                "LDX $12,Y\nSTX $12,Y\n" +
                "ADC $1234\nAND $1234\nASL $1234\nBIT $1234\nCMP $1234\nCPX $1234\n" +
                "CPY $1234\nDEC $1234\nEOR $1234\nINC $1234\nJMP $1234\nJSR $1234\n" +
                "LDA $1234\nLDX $1234\nLDY $1234\nLSR $1234\nORA $1234\nROL $1234\n" +
                "ROR $1234\nSBC $1234\nSTA $1234\nSTX $1234\nSTY $1234\nSTZ $1234\n" +
                "TRB $1234\nTSB $1234\n" +
                "ADC $1234,X\nAND $1234,X\nASL $1234,X\nBIT $1234,X\nCMP $1234,X\n" +
                "DEC $1234,X\nEOR $1234,X\nINC $1234,X\nLDA $1234,X\nLDY $1234,X\n" +
                "LSR $1234,X\nORA $1234,X\nROL $1234,X\nROR $1234,X\nSBC $1234,X\n" +
                "STA $1234,X\nSTZ $1234,X\n" +
                "ADC $1234,Y\nAND $1234,Y\nCMP $1234,Y\nEOR $1234,Y\nLDA $1234,Y\n" +
                "LDX $1234,Y\nORA $1234,Y\nSBC $1234,Y\nSTA $1234,Y\n"), ".S");
            QueueLine(editor, "ASSEMBLE DIRECT.S");
            RunUntil(cpu, bus, s => s.Contains("Writing DIRECT.OBJ", StringComparison.Ordinal),
                "complete direct-addressing assembly result");
            RunSteps(cpu, bus, 500_000);
            byte[] directObject = disk.Load("DIRECT", ".OBJ");
            CollectionAssert.AreEqual(new byte[]
            {
                0x65,0x12, 0x25,0x12, 0x06,0x12, 0x24,0x12, 0xC5,0x12, 0xE4,0x12,
                0xC4,0x12, 0xC6,0x12, 0x45,0x12, 0xE6,0x12, 0xA5,0x12, 0xA6,0x12,
                0xA4,0x12, 0x46,0x12, 0x05,0x12, 0x26,0x12, 0x66,0x12, 0xE5,0x12,
                0x85,0x12, 0x86,0x12, 0x84,0x12, 0x64,0x12, 0x14,0x12, 0x04,0x12,
                0x75,0x12, 0x35,0x12, 0x16,0x12, 0x34,0x12, 0xD5,0x12, 0xD6,0x12,
                0x55,0x12, 0xF6,0x12, 0xB5,0x12, 0xB4,0x12, 0x56,0x12, 0x15,0x12,
                0x36,0x12, 0x76,0x12, 0xF5,0x12, 0x95,0x12, 0x94,0x12, 0x74,0x12,
                0xB6,0x12, 0x96,0x12,
                0x6D,0x34,0x12, 0x2D,0x34,0x12, 0x0E,0x34,0x12, 0x2C,0x34,0x12,
                0xCD,0x34,0x12, 0xEC,0x34,0x12, 0xCC,0x34,0x12, 0xCE,0x34,0x12,
                0x4D,0x34,0x12, 0xEE,0x34,0x12, 0x4C,0x34,0x12, 0x20,0x34,0x12,
                0xAD,0x34,0x12, 0xAE,0x34,0x12, 0xAC,0x34,0x12, 0x4E,0x34,0x12,
                0x0D,0x34,0x12, 0x2E,0x34,0x12, 0x6E,0x34,0x12, 0xED,0x34,0x12,
                0x8D,0x34,0x12, 0x8E,0x34,0x12, 0x8C,0x34,0x12, 0x9C,0x34,0x12,
                0x1C,0x34,0x12, 0x0C,0x34,0x12,
                0x7D,0x34,0x12, 0x3D,0x34,0x12, 0x1E,0x34,0x12, 0x3C,0x34,0x12,
                0xDD,0x34,0x12, 0xDE,0x34,0x12, 0x5D,0x34,0x12, 0xFE,0x34,0x12,
                0xBD,0x34,0x12, 0xBC,0x34,0x12, 0x5E,0x34,0x12, 0x1D,0x34,0x12,
                0x3E,0x34,0x12, 0x7E,0x34,0x12, 0xFD,0x34,0x12, 0x9D,0x34,0x12,
                0x9E,0x34,0x12,
                0x79,0x34,0x12, 0x39,0x34,0x12, 0xD9,0x34,0x12, 0x59,0x34,0x12,
                0xB9,0x34,0x12, 0xBE,0x34,0x12, 0x19,0x34,0x12, 0xF9,0x34,0x12,
                0x99,0x34,0x12
            }, directObject[32..276],
                "NAS must cover direct zero-page, absolute, X-indexed, and Y-indexed modes.");

            disk.Save("INDIRECT", Encoding.ASCII.GetBytes(
                "ADC ($12,X)\nAND ($12,X)\nCMP ($12,X)\nEOR ($12,X)\n" +
                "LDA ($12,X)\nORA ($12,X)\nSBC ($12,X)\nSTA ($12,X)\n" +
                "ADC ($12),Y\nAND ($12),Y\nCMP ($12),Y\nEOR ($12),Y\n" +
                "LDA ($12),Y\nORA ($12),Y\nSBC ($12),Y\nSTA ($12),Y\n" +
                "ADC ($12)\nAND ($12)\nCMP ($12)\nEOR ($12)\n" +
                "LDA ($12)\nORA ($12)\nSBC ($12)\nSTA ($12)\n" +
                "JMP ($1234)\nJMP ($1234,X)\nTarget:\nRTS\n" +
                "JMP (target)\nJMP (TARGET,X)\n"), ".S");
            QueueLine(editor, "ASSEMBLE INDIRECT.S");
            RunUntil(cpu, bus, s => s.Contains("Writing INDIRECT.OBJ", StringComparison.Ordinal),
                "complete indirect-addressing assembly result");
            RunSteps(cpu, bus, 500_000);
            byte[] indirectObject = disk.Load("INDIRECT", ".OBJ");
            CollectionAssert.AreEqual(new byte[]
            {
                0x61,0x12, 0x21,0x12, 0xC1,0x12, 0x41,0x12,
                0xA1,0x12, 0x01,0x12, 0xE1,0x12, 0x81,0x12,
                0x71,0x12, 0x31,0x12, 0xD1,0x12, 0x51,0x12,
                0xB1,0x12, 0x11,0x12, 0xF1,0x12, 0x91,0x12,
                0x72,0x12, 0x32,0x12, 0xD2,0x12, 0x52,0x12,
                0xB2,0x12, 0x12,0x12, 0xF2,0x12, 0x92,0x12,
                0x6C,0x34,0x12, 0x7C,0x34,0x12
            }, indirectObject[32..86],
                "NAS must cover every W65C02 parenthesized addressing mode.");
            CollectionAssert.AreEqual(new byte[] { 0x60, 0x6C, 0x00, 0x00, 0x7C, 0x00, 0x00 },
                indirectObject[86..93], "Symbolic indirect JMP operands must remain relocatable.");
            Assert.AreEqual(2, indirectObject[14]);

            QueueLine(editor, "LINK INDIRECT.OBJ");
            RunUntil(cpu, bus, s => s.Contains("Writing INDIRECT.BIN", StringComparison.Ordinal),
                "indirect-symbol link result");
            RunSteps(cpu, bus, 500_000);
            byte[] indirectExecutable = disk.Load("INDIRECT", ".BIN");
            CollectionAssert.AreEqual(new byte[] { 0x60, 0x6C, 0x36, 0x70, 0x7C, 0x36, 0x70 },
                indirectExecutable[56..63],
                "NL must relocate absolute and absolute-indexed indirect JMP operands.");

            disk.Save("BITOPS", Encoding.ASCII.GetBytes(
                "rmb0 $12\nRMB1 $12\nRMB2 $12\nRMB3 $12\nRMB4 $12\nRMB5 $12\nRMB6 $12\nRMB7 $12\n" +
                "smb0 $12\nSMB1 $12\nSMB2 $12\nSMB3 $12\nSMB4 $12\nSMB5 $12\nSMB6 $12\nSMB7 $12\n" +
                "bbr0 $12,target\nBBR1 $12,target\nBBR2 $12,target\nBBR3 $12,target\n" +
                "BBR4 $12,target\nBBR5 $12,target\nBBR6 $12,target\nBBR7 $12,target\n" +
                "bbs0 $12,target\nBBS1 $12,target\nBBS2 $12,target\nBBS3 $12,target\n" +
                "BBS4 $12,target\nBBS5 $12,target\nBBS6 $12,target\nBBS7 $12,target\n" +
                "Target:\nRTS\n"), ".S");
            QueueLine(editor, "ASSEMBLE BITOPS.S");
            RunUntil(cpu, bus, s => s.Contains("Writing BITOPS.OBJ", StringComparison.Ordinal),
                "complete bit-opcode assembly result");
            RunSteps(cpu, bus, 500_000);
            byte[] bitObject = disk.Load("BITOPS", ".OBJ");
            CollectionAssert.AreEqual(new byte[]
            {
                0x07,0x12, 0x17,0x12, 0x27,0x12, 0x37,0x12,
                0x47,0x12, 0x57,0x12, 0x67,0x12, 0x77,0x12,
                0x87,0x12, 0x97,0x12, 0xA7,0x12, 0xB7,0x12,
                0xC7,0x12, 0xD7,0x12, 0xE7,0x12, 0xF7,0x12,
                0x0F,0x12,0x2D, 0x1F,0x12,0x2A, 0x2F,0x12,0x27, 0x3F,0x12,0x24,
                0x4F,0x12,0x21, 0x5F,0x12,0x1E, 0x6F,0x12,0x1B, 0x7F,0x12,0x18,
                0x8F,0x12,0x15, 0x9F,0x12,0x12, 0xAF,0x12,0x0F, 0xBF,0x12,0x0C,
                0xCF,0x12,0x09, 0xDF,0x12,0x06, 0xEF,0x12,0x03, 0xFF,0x12,0x00,
                0x60
            }, bitObject[32..113],
                "NAS must encode all RMB/SMB and checked BBR/BBS forms.");

            disk.Save("BRANCHES", Encoding.ASCII.GetBytes(
                "BCC target\nBCS target\nBEQ target\nBMI target\nBNE target\n" +
                "BPL target\nBRA target\nBVC target\nBVS target\nTarget:\nRTS\n"), ".S");
            QueueLine(editor, "ASSEMBLE BRANCHES.S");
            RunUntil(cpu, bus, s => s.Contains("Writing BRANCHES.OBJ", StringComparison.Ordinal),
                "complete relative-branch assembly result");
            RunSteps(cpu, bus, 500_000);
            byte[] branchObject = disk.Load("BRANCHES", ".OBJ");
            CollectionAssert.AreEqual(new byte[]
            {
                0x90,0x10, 0xB0,0x0E, 0xF0,0x0C, 0x30,0x0A, 0xD0,0x08,
                0x10,0x06, 0x80,0x04, 0x50,0x02, 0x70,0x00, 0x60
            }, branchObject[32..51],
                "NAS must encode every W65C02 relative branch with checked displacement.");

            Assert.IsTrue(disk.FileExists("HELLO", ".S"), "BUILD must retain generated assembly.");
            Assert.IsTrue(disk.FileExists("HELLO", ".OBJ"), "BUILD must retain the NOBJ object.");
            Assert.IsTrue(disk.FileExists("HELLO", ".BIN"), "BUILD must retain the linked binary.");

            string assembly = Encoding.ASCII.GetString(disk.Load("HELLO", ".S"));
            StringAssert.Contains(assembly, ".IMPORT P_WRITE_CHAR");
            StringAssert.Contains(assembly, "; HELLO.PAS:3 WRITELN");
            StringAssert.Contains(assembly, "JSR P_WRITE_CHAR");
            Assert.IsFalse(assembly.Contains('\r'), "NPC output must use Nova's LF-only text convention.");
            Assert.IsFalse(assembly.Contains("LDA #$0D", StringComparison.Ordinal),
                "WRITELN must emit one Nova LF, not a redundant CR/LF pair.");
            Assert.IsFalse(assembly.Contains("$A00E", StringComparison.Ordinal),
                "NPC must emit a symbol, not a hard-coded hardware address.");

            byte[] objectFile = disk.Load("HELLO", ".OBJ");
            int codeLength = objectFile[26] | objectFile[27] << 8;
            Assert.AreEqual(111, codeLength, "Literal WRITELN should compile to 22 writes and one RTS.");
            Assert.IsTrue((objectFile[14] | objectFile[15] << 8) > 0,
                "NAS must preserve external calls as NOBJ relocations.");
            byte[] library = disk.Load("PASCAL", ".NLIB");
            Assert.AreEqual(2, library[4], "Pascal libraries must use complete NOBJ members.");
            byte[] writeMember = ReadNlibMember(library, "P_WRITE_CHAR");
            byte[] deviceMember = ReadNlibMember(library, "P_CHAR_DEVICE");
            byte[] unusedMember = ReadNlibMember(library, "P_UNUSED");
            byte[] writeCode = ReadNobjSectionData(writeMember, "CODE");
            byte[] deviceCode = ReadNobjSectionData(deviceMember, "CODE");
            byte[] unusedCode = ReadNobjSectionData(unusedMember, "CODE");
            byte[] executable = disk.Load("HELLO", ".BIN");
            Assert.AreEqual(2 + codeLength + writeCode.Length + deviceCode.Length, executable.Length,
                "NL must extract the complete transitive member set without NOBJ metadata or padding.");
            CollectionAssert.AreEqual(new byte[]
            {
                0x20, 0x73, 0x80, 0x60,       // P_WRITE_CHAR calls P_CHAR_DEVICE
                0x8D, 0x0E, 0xA0, 0x60        // P_CHAR_DEVICE writes through the NDK address
            }, executable[^8..],
                "NL must relocate imports inside extracted archive members after fixed-point selection.");
            Assert.IsFalse(ContainsSequence(executable, unusedCode),
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
            int objectLength = library[offset] | library[offset + 1] << 8;
            offset += 2;
            byte[] objectFile = library[offset..(offset + objectLength)];
            if (NobjDefines(objectFile, wanted))
                return objectFile;
            offset += objectLength;
        }
        Assert.Fail($"NLIB member {wanted} was not found.");
        return [];
    }

    private static bool NobjDefines(byte[] objectFile, string wanted)
    {
        int symbolCount = objectFile[10] | objectFile[11] << 8;
        int offset = objectFile[12] | objectFile[13] << 8;
        for (int symbol = 0; symbol < symbolCount; symbol++)
        {
            byte section = objectFile[offset + 2];
            byte flags = objectFile[offset + 3];
            int nameLength = objectFile[offset + 4];
            string name = Encoding.ASCII.GetString(objectFile, offset + 5, nameLength);
            if (section != 0xFF && (flags & 1) != 0 &&
                name.Equals(wanted, StringComparison.OrdinalIgnoreCase))
                return true;
            offset += 5 + nameLength;
        }
        return false;
    }

    private static byte[] ReadNobjSectionData(byte[] objectFile, string wanted)
    {
        int offset = 20;
        for (int section = 0; section < objectFile[6]; section++)
        {
            int nameLength = objectFile[offset + 2];
            int dataLength = objectFile[offset + 6] | objectFile[offset + 7] << 8;
            string name = Encoding.ASCII.GetString(objectFile, offset + 8, nameLength);
            int dataOffset = offset + 8 + nameLength;
            if (name.Equals(wanted, StringComparison.OrdinalIgnoreCase))
                return objectFile[dataOffset..(dataOffset + dataLength)];
            offset = dataOffset + dataLength;
        }
        Assert.Fail($"NOBJ section {wanted} was not found.");
        return [];
    }

    private static bool ContainsSequence(byte[] haystack, byte[] needle)
    {
        return haystack.AsSpan().IndexOf(needle) >= 0;
    }
}

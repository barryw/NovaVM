using System;
using System.IO;
using System.Linq;
using System.Text;
using e6502.Avalonia.Hardware;
using e6502.Avalonia.Input;
using e6502.Storage;
using KDS.e6502;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
[DoNotParallelize]
public class NovaPascalTests
{
    [TestMethod]
    public void ToolchainUsesNdkManagedXramAndOverlayApis()
    {
        string npc = File.ReadAllText(RepoPath("software", "languages", "novapascal", "tools", "npc.s"));
        string optimizer = File.ReadAllText(RepoPath("software", "languages", "novapascal", "tools", "npo2.s"));
        string editor = File.ReadAllText(RepoPath("software", "languages", "novapascal", "tools", "npedit.s"));
        string assembler = File.ReadAllText(RepoPath("software", "toolchain", "novaasm_tool.s"));
        string linker = File.ReadAllText(RepoPath("software", "toolchain", "novalink_tool.s"));
        string linkerAbi = File.ReadAllText(RepoPath("software", "toolchain", "novalink.inc"));

        StringAssert.Contains(npc, "LDA   #MEM_ALLOC");
        StringAssert.Contains(npc, "LDA   #MEM_RELEASE");
        Assert.IsFalse(npc.Contains("DOCBUF_XRAM_BASE", StringComparison.Ordinal),
            "NPC must allocate transient source storage instead of claiming the editor's XRAM slot.");
        StringAssert.Contains(optimizer, "LDA   #MEM_ALLOC");
        StringAssert.Contains(optimizer, "LDA   #MEM_RELEASE");
        StringAssert.Contains(optimizer, ".include \"xramstream.inc\"");
        StringAssert.Contains(editor, "LDA   #MEM_ALLOC");
        StringAssert.Contains(editor, "LDA   #MEM_RELEASE");
        StringAssert.Contains(assembler, "LDA   #MEM_ALLOC");
        StringAssert.Contains(assembler, "LDA   #MEM_RELEASE");
        StringAssert.Contains(assembler, "LDA   #SYS_OVL_LOAD");
        StringAssert.Contains(assembler, "LDA   #SYS_OVL_MAIN");
        StringAssert.Contains(assembler, "LDA   #SYS_OVL_UNLOAD");
        Assert.IsFalse(assembler.Contains("overlay_load_fixed", StringComparison.Ordinal),
            "NAS must not bypass the NDK System overlay lifecycle.");
        StringAssert.Contains(linker, "LDA   #SYS_OVL_LOAD");
        StringAssert.Contains(linker, "LDA   #SYS_OVL_MAIN");
        StringAssert.Contains(linker, "LDA   #SYS_OVL_UNLOAD");
        Assert.IsFalse(linker.Contains("overlay_load_fixed", StringComparison.Ordinal),
            "NL must not bypass the NDK System overlay lifecycle.");
        StringAssert.Contains(linkerAbi, "NLINK_OBJECT_CAP     = 8");
    }

    [TestMethod]
    public void ShellBuildsRunsAndReturnsFromEditor()
    {
        string root = Path.Combine(Path.GetTempPath(), $"novapascal-{Guid.NewGuid():N}");
        string disks = Path.Combine(root, "disks");
        Directory.CreateDirectory(disks);
        string sourceImage = RepoPath("software", "languages", "novapascal", "novapascal.ndi");
        using (NdiImage image = NdiImage.Open(sourceImage))
        {
            Assert.AreEqual(96u, image.Header.DirectorySectorCount,
                "A development disk needs room for generated units and user build artifacts.");
            NdiDirEntry[] entries = image.ListDirectory(0xFFFF);
            Assert.AreEqual(NdiFileType.Assembly,
                entries.Single(entry => entry.Filename.Equals("AUDIO.NPI", StringComparison.OrdinalIgnoreCase)).FileType,
                "Compiled unit interfaces are editable assembly text, not opaque binaries.");
        }
        File.Copy(
            sourceImage,
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
            Assert.IsTrue(disk.FileExists("NPO2", ".BIN"), "O2 passes must run from lower RAM without displacing resident NPC.");
            Assert.IsTrue(disk.FileExists("NPPROJ", ".BIN"), "Project operations must run from a standard disk-loaded binary.");
            Assert.IsTrue(disk.FileExists("NAS", ".BIN"), "NAS must be a standard, language-neutral binary.");
            Assert.IsTrue(disk.FileExists("NASPP", ".OVL"), "NAS preprocessing must live in its disk-loaded overlay.");
            Assert.IsTrue(disk.FileExists("NASBE", ".OVL"), "NAS assembly core and opcode tables must live in its disk-loaded backend overlay.");
            Assert.IsTrue(disk.FileExists("NL", ".BIN"), "NL must be a standard, language-neutral binary.");
            Assert.IsTrue(disk.FileExists("NLWORK", ".OVL"), "NL map/GC work must live in its disk-loaded overlay.");
            Assert.IsTrue(disk.FileExists("PASCAL", ".NLIB"), "Pascal runtime APIs belong in an ordinary linker library.");
            Assert.IsTrue(disk.FileExists("PASCAL", ".INC"), "Generated code must use the declared Pascal compiler ABI.");
            Assert.IsTrue(disk.FileExists("NVR", ".INC"), "Compiler scratch registers must come from the canonical NDK mailbox include.");
            Assert.IsTrue(disk.FileExists("NOVA", ".INC"), "Pascal NDK units must use the canonical hardware declarations.");
            Assert.IsTrue(disk.FileExists("NOVA", ".NPI"), "Pascal bindings must be generated from canonical NDK metadata.");
            string[] ndkUnitStems =
            {
                "NOVA", "FIO", "AUDIO", "VGC", "SPRITE", "MSPRITE", "VSPRITE", "VTEXT", "NUI",
                "COPPER", "DMA", "BLITTER", "XRAM", "XMC", "PAGER", "RNG", "NVG", "ANIM", "TWEEN",
                "NIC", "GAMESERVER", "OVERLAY", "VGCWAIT", "COPPERSPLIT", "DOCBUF", "FIOCLEARERROR",
                "MOUSE", "MOUSEEVENTS", "NUIDIALOG", "NUIFILE", "NUIINPUT", "NUILIST", "NUITEXT",
                "NUIUISAVE", "NUIWAIT", "SPRITEBANK", "VGCPALETTE", "VGCVSYNC", "VTEXTMIXED", "WTS",
            };
            foreach (string stem in ndkUnitStems)
            {
                Assert.IsTrue(disk.FileExists(stem, ".NPI"), $"{stem} must have a generated Pascal ABI.");
                Assert.IsTrue(disk.FileExists(stem, ".INC"), $"{stem} must have a generated declaration facade.");
                Assert.IsTrue(disk.FileExists(stem, ".S"), $"{stem} must have a generated implementation facade.");
                Assert.IsTrue(disk.FileExists("NOVA" + stem, ".PAS"), $"{stem} must have a Pascal unit contract.");
            }
            Assert.IsTrue(disk.FileExists("RNG", ".NPI"), "Each Pascal NDK unit must carry generated ABI signatures.");
            Assert.IsTrue(disk.FileExists("RNG", ".S"), "The Pascal disk must carry canonical NDK implementation sources.");
            Assert.IsTrue(disk.FileExists("FIO", ".S"), "Dependent canonical NDK implementations must remain available to NAS.");
            Assert.IsTrue(disk.FileExists("NOVAGFX", ".PAS"), "Graphics must be exposed as a native Pascal unit.");
            Assert.IsTrue(disk.FileExists("GRAPHICS", ".NPI"), "The graphics unit must carry its compiled Pascal interface.");
            Assert.IsTrue(disk.FileExists("GRAPHICS", ".S"), "The graphics unit adapter must be available to NAS.");
            Assert.IsTrue(disk.FileExists("NOVAINPUT", ".PAS"), "Input must be exposed as a native Pascal unit.");
            Assert.IsTrue(disk.FileExists("INPUT", ".NPI"), "The input unit must carry its compiled Pascal interface.");
            Assert.IsTrue(disk.FileExists("INPUT", ".S"), "The input unit must adapt Nova's canonical input register.");
            Assert.IsTrue(disk.FileExists("VGC", ".S"), "The graphics unit must reuse the canonical VGC NDK implementation.");
            Assert.IsTrue(disk.FileExists("NOVARAND", ".PAS"), "Randomness must be exposed as a native Pascal unit.");
            Assert.IsTrue(disk.FileExists("RANDOM", ".NPI"), "The random unit must carry its compiled Pascal interface.");
            Assert.IsTrue(disk.FileExists("RANDOM", ".S"), "The random unit must adapt the canonical RNG implementation.");
            CollectionAssert.AreEqual(new byte[] { 0x00, 0x20 }, disk.Load("NAS", ".BIN")[..2]);
            CollectionAssert.AreEqual(new byte[] { 0x00, 0x20 }, disk.Load("NL", ".BIN")[..2]);

            Assert.IsTrue(disk.ListDirectory(null).Any(entry => entry.IsDirectory &&
                entry.Filename.Equals("HELLO", StringComparison.OrdinalIgnoreCase)),
                "Packaged Pascal projects must live in directories.");
            disk.CurrentDirectory = "HELLO";
            var helloEntries = disk.ListDirectory(null).ToArray();
            Assert.AreEqual(NdiFileType.Pascal,
                helloEntries.Single(entry => entry.Filename.Equals("MAIN", StringComparison.OrdinalIgnoreCase)
                                             && entry.Extension.Equals(".PAS", StringComparison.OrdinalIgnoreCase)).FileType);
            Assert.AreEqual(NdiFileType.PascalProject,
                helloEntries.Single(entry => entry.Filename.Equals("HELLO", StringComparison.OrdinalIgnoreCase)
                                             && entry.Extension.Equals(".NPP", StringComparison.OrdinalIgnoreCase)).FileType);
            disk.CurrentDirectory = "/";

            QueueLine(editor, "HELP");
            RunUntil(cpu, bus, s => s.Contains("RUN file.bin", StringComparison.Ordinal), "shell help");
            StringAssert.Contains(Snapshot(bus), "NEW name",
                "The project generator must be discoverable from shell HELP.");
            RunSteps(cpu, bus, 100_000);
            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);

            QueueLine(editor, "NEW DEMO");
            RunUntil(cpu, bus, s => s.Contains("Created project DEMO", StringComparison.Ordinal),
                "new Pascal project");
            Assert.IsTrue(disk.ListDirectory(null).Any(entry => entry.IsDirectory &&
                entry.Filename.Equals("DEMO", StringComparison.OrdinalIgnoreCase)));
            disk.CurrentDirectory = "DEMO";
            byte[] demoSource = Encoding.ASCII.GetBytes(
                "program DEMO;\nbegin\n  writeln('Hello, world!');\nend.\n");
            byte[] createdDemoSource = disk.Load("MAIN", ".PAS");
            CollectionAssert.AreEqual(demoSource, createdDemoSource,
                $"NEW must create a minimal LF-only Pascal program; got {Convert.ToHexString(createdDemoSource)}.");
            CollectionAssert.AreEqual(Encoding.ASCII.GetBytes(
                    "NPP 2\nMAIN MAIN.PAS\nOUTPUT DEMO.BIN\nOPTIMIZE O2\n" +
                    "DEFINE NOVA=1\nCONFIG INLINE\nMAP DEMO.MAP\nLABEL DEMO.LBL\n" +
                    "MEMORY {\n    RAM: start = $8000, size = $1000, file = %O;\n}\n\n" +
                    "SEGMENTS {\n    CODE: load = RAM, type = ro;\n" +
                    "    BSS: load = RAM, type = bss;\n}\n"),
                disk.Load("DEMO", ".NPP"),
                "The generated project must contain NPC, NAS, and NL configuration.");
            Assert.IsFalse(disk.FileExists("DEMO", ".CFG"),
                "Inline projects must not create a redundant linker-config file.");
            disk.CurrentDirectory = "/";

            QueueLine(editor, "NEW DEMO");
            RunUntil(cpu, bus, s => s.Contains("Project or unit already exists.", StringComparison.Ordinal),
                "new project overwrite protection");
            disk.CurrentDirectory = "DEMO";
            CollectionAssert.AreEqual(demoSource, disk.Load("MAIN", ".PAS"),
                "NEW must never overwrite an existing project file.");
            disk.CurrentDirectory = "/";

            QueueLine(editor, "ADDUNIT DEMO GREETER");
            RunUntil(cpu, bus, s => s.Contains("Added unit GREETER", StringComparison.Ordinal),
                "add Pascal unit");
            QueueLine(editor, "ADDUNIT DEMO SECOND");
            RunUntil(cpu, bus, s => s.Contains("Added unit SECOND", StringComparison.Ordinal),
                "add second Pascal unit");
            disk.CurrentDirectory = "DEMO";
            string demoManifest = Encoding.ASCII.GetString(disk.Load("DEMO", ".NPP"));
            StringAssert.Contains(demoManifest, "UNIT GREETER.PAS\n");
            StringAssert.Contains(demoManifest, "UNIT SECOND.PAS\n");
            Assert.IsTrue(disk.FileExists("GREETER", ".PAS"));
            Assert.IsTrue(disk.FileExists("SECOND", ".PAS"));
            disk.Save("MAIN", Encoding.ASCII.GetBytes(
                "program DEMO;\nuses Greeter;\nbegin\n  Greet;\nend.\n"), ".PAS");
            disk.Save("GREETER", Encoding.ASCII.GetBytes(
                "unit Greeter;\n\ninterface\nprocedure Greet;\n\nimplementation\n" +
                "procedure Greet;\nbegin\n  writeln('Hello, world!');\nend;\n\nend.\n"), ".PAS");
            disk.Save("SECOND", Encoding.ASCII.GetBytes(
                "unit Second;\n\ninterface\nimplementation\nend.\n"), ".PAS");
            disk.CurrentDirectory = "/";

            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);
            QueueLine(editor, "BUILD DEMO");
            RunUntil(cpu, bus, s => s.Contains("Compiling MAIN.PAS", StringComparison.Ordinal),
                "generated project main compilation");
            string generatedMainScreen = Snapshot(bus);
            RunUntil(cpu, bus, s => s.Contains("Compiling GREETER.PAS", StringComparison.Ordinal),
                "generated project unit compilation");
            string generatedFirstUnitScreen = Snapshot(bus);
            RunUntil(cpu, bus, s => s.Contains("Compiling SECOND.PAS", StringComparison.Ordinal),
                "generated project second unit compilation");
            string generatedUnitScreen = Snapshot(bus);
            QueueLine(editor, "ZZZ");
            RunUntil(cpu, bus, s => s.Contains("Unknown command. Type HELP.", StringComparison.Ordinal),
                "generated project build completion marker");
            string generatedBuildScreen = Snapshot(bus);
            disk.CurrentDirectory = "DEMO";
            string generatedAssembly = disk.FileExists("DEMO", ".S")
                ? Encoding.ASCII.GetString(disk.Load("DEMO", ".S"))
                : string.Empty;
            bool generatedObjectExists = disk.FileExists("DEMO", ".OBJ");
            bool generatedSecondAssemblyExists = disk.FileExists("SECOND", ".ASM");
            disk.CurrentDirectory = "/";
            Assert.IsTrue(generatedBuildScreen.Contains("Build complete: DEMO.BIN", StringComparison.Ordinal),
                $"Tool status ${bus.Read(0x0275):X2}, detail ${bus.Read(0x0276):X2}, " +
                $"diagnostic {bus.Read(0x02F3) | bus.Read(0x02F4) << 8}:" +
                $"{bus.Read(0x02F5) | bus.Read(0x02F6) << 8}, source " +
                Encoding.ASCII.GetString(Enumerable.Range(0, bus.Read(0x02FF))
                    .Select(i => bus.Read((ushort)(0x0800 + i))).ToArray()) + ", " +
                $"object {generatedObjectExists}, assembly {generatedAssembly.Length} bytes\n" +
                generatedAssembly.Replace("\n", "\\n", StringComparison.Ordinal) + "\n" +
                generatedBuildScreen);
            Assert.IsTrue(
                generatedMainScreen.IndexOf("Configuration valid", StringComparison.Ordinal) <
                generatedMainScreen.IndexOf("Compiling MAIN.PAS", StringComparison.Ordinal),
                "BUILD must validate linker configuration before NPC creates intermediate files.");
            StringAssert.Contains(generatedMainScreen, "Compiling MAIN.PAS");
            StringAssert.Contains(generatedFirstUnitScreen, "Compiling GREETER.PAS");
            StringAssert.Contains(generatedUnitScreen, "Compiling SECOND.PAS");
            Assert.IsTrue(generatedSecondAssemblyExists,
                "BUILD must compile every declared unit, including manifest entries after the first.");
            StringAssert.Contains(generatedBuildScreen, "Library /PASCAL.NLIB",
                "Generated Pascal projects must link the standard runtime/NDK library.");

            QueueLine(editor, "RUN DEMO");
            RunUntil(cpu, bus, s => s.Contains("Hello, world!", StringComparison.Ordinal),
                "generated project executable");
            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);
            QueueLine(editor, "DELUNIT DEMO GREETER");
            RunUntil(cpu, bus, s => s.Contains("Deleted unit GREETER", StringComparison.Ordinal)
                                    || s.Contains("Invalid Pascal project.", StringComparison.Ordinal)
                                    || s.Contains("Unit is not in the project.", StringComparison.Ordinal)
                                    || s.Contains("File I/O error.", StringComparison.Ordinal),
                "delete Pascal unit result");
            disk.CurrentDirectory = "DEMO";
            string deletedUnitManifest = Encoding.ASCII.GetString(disk.Load("DEMO", ".NPP"));
            bool deletedUnitSourceExists = disk.FileExists("GREETER", ".PAS");
            bool deletedUnitOutputExists = disk.FileExists("DEMO", ".BIN");
            disk.CurrentDirectory = "/";
            string deleteUnitScreen = Snapshot(bus);
            Assert.IsTrue(deleteUnitScreen.Contains("Deleted unit GREETER", StringComparison.Ordinal),
                $"status=${bus.Read(0x0275):X2}, detail=${bus.Read(0x0276):X2}, " +
                $"source={deletedUnitSourceExists}, output={deletedUnitOutputExists}, " +
                $"manifest={deletedUnitManifest.Replace("\n", "\\n", StringComparison.Ordinal)}\n{deleteUnitScreen}");
            disk.CurrentDirectory = "DEMO";
            Assert.IsFalse(disk.FileExists("GREETER", ".PAS"));
            Assert.IsFalse(disk.FileExists("DEMO", ".BIN"),
                "Changing unit membership must invalidate stale project output.");
            Assert.IsFalse(deletedUnitManifest.Contains("UNIT GREETER.PAS", StringComparison.Ordinal));
            disk.CurrentDirectory = "/";
            QueueLine(editor, "DELPROJECT DEMO");
            RunUntil(cpu, bus, s => s.Contains("Deleted project DEMO", StringComparison.Ordinal),
                "delete Pascal project");
            Assert.IsFalse(disk.ListDirectory(null).Any(entry => entry.IsDirectory &&
                entry.Filename.Equals("DEMO", StringComparison.OrdinalIgnoreCase)));
            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);

            QueueLine(editor, "NEW MAIN");
            RunUntil(cpu, bus, s => s.Contains("Created project MAIN", StringComparison.Ordinal),
                "project whose name matches the conventional main source");
            QueueLine(editor, "BUILD MAIN");
            RunUntil(cpu, bus, s => s.Contains("Build complete: MAIN.BIN", StringComparison.Ordinal)
                                    || s.Contains("Assembler error.", StringComparison.Ordinal)
                                    || s.Contains("Linker error.", StringComparison.Ordinal),
                "MAIN project build without assembly filename collision");
            disk.CurrentDirectory = "MAIN";
            Assert.IsTrue(disk.FileExists("MAIN", ".ASM"),
                "Per-source assembly must use a distinct filename from the whole-project stream.");
            Assert.IsTrue(disk.FileExists("MAIN", ".S"),
                "The whole-project assembly must retain the conventional project .S filename.");
            Assert.IsTrue(disk.FileExists("MAIN", ".BIN"),
                "A project named MAIN must build end to end.");
            disk.CurrentDirectory = "/";
            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);
            QueueLine(editor, "RUN MAIN");
            RunUntil(cpu, bus, s => s.Contains("Hello, world!", StringComparison.Ordinal),
                "MAIN project executable");
            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);
            QueueLine(editor, "DELPROJECT MAIN");
            RunUntil(cpu, bus, s => s.Contains("Deleted project MAIN", StringComparison.Ordinal),
                "delete MAIN project");
            Assert.IsFalse(disk.ListDirectory(null).Any(entry => entry.IsDirectory &&
                entry.Filename.Equals("MAIN", StringComparison.OrdinalIgnoreCase)));
            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);

            int xramPagesBeforeBuild = bus.Read(0xBA0E) | bus.Read(0xBA0F) << 8;
            QueueLine(editor, "BUILD HELLO");
            RunUntil(cpu, bus, s => s.Contains("Compiling MAIN.PAS", StringComparison.Ordinal),
                "project compiler launch");
            RunUntil(cpu, bus, s => s.Contains("Assembling HELLO.S", StringComparison.Ordinal),
                "project assembler launch");
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
            Assert.AreEqual(xramPagesBeforeBuild, bus.Read(0xBA0E) | bus.Read(0xBA0F) << 8,
                "NPC, NPO2, and NAS must release every transient NDK XRAM allocation after BUILD.");
            StringAssert.Contains(buildScreen, "Nova Pascal Compiler v1.0");
            StringAssert.Contains(buildScreen, "Compiling MAIN.PAS");
            StringAssert.Contains(buildScreen, "Writing MAIN.ASM");
            StringAssert.Contains(buildScreen, "Nova Pascal Optimizer v1.0");
            StringAssert.Contains(buildScreen, "Pass 1: typed IR optimization");
            StringAssert.Contains(buildScreen, "Pass 2: leaf routine inlining");
            StringAssert.Contains(buildScreen, "Pass 3: caller routine inlining");
            StringAssert.Contains(buildScreen, "Pass 4: dead routine cleanup");
            StringAssert.Contains(buildScreen, "Pass 5: instruction selection");
            StringAssert.Contains(buildScreen, "Pass 6: machine peepholes");
            StringAssert.Contains(buildScreen, "Nova Assembler v1.0");
            StringAssert.Contains(buildScreen, "Preprocessing HELLO.S");
            StringAssert.Contains(buildScreen, "Assembling HELLO.S");
            StringAssert.Contains(buildScreen, "Writing HELLO.OBJ");
            StringAssert.Contains(buildScreen, "Nova Linker v1.0");
            StringAssert.Contains(buildScreen, "Config HELLO.NPP");
            StringAssert.Contains(buildScreen, "Linking HELLO.OBJ");
            StringAssert.Contains(buildScreen, "Library /PASCAL.NLIB");
            StringAssert.Contains(buildScreen, "Writing HELLO.BIN");
            StringAssert.Contains(buildScreen, "Map HELLO.MAP");
            StringAssert.Contains(buildScreen, "Labels HELLO.LBL");
            disk.CurrentDirectory = "HELLO";
            Assert.IsTrue(disk.FileExists("HELLO", ".OBJ"),
                $"NAS did not write HELLO.OBJ (detail ${bus.Read(0x0276):X2}):\n{buildScreen}");
            CollectionAssert.AreEqual(new byte[] { 0x00, 0x80 }, disk.Load("HELLO", ".BIN")[..2],
                "NPP CONFIG must control NL placement through the shared tool mailbox.");
            string helloMap = Encoding.ASCII.GetString(disk.Load("HELLO", ".MAP"));
            StringAssert.Contains(helloMap, "Nova Linker Map v1\nLoad $8000\n");
            StringAssert.Contains(helloMap, "Sections\n");
            StringAssert.Contains(helloMap, "Exports\n");
            StringAssert.Contains(helloMap, "I_P_WRITE_LINE");
            Assert.IsFalse(helloMap.Contains("P_WRITE_CHAR", StringComparison.Ordinal),
                "NL must strip the single-character writer when a program only uses inline line data.");
            Assert.IsFalse(helloMap.Contains('\r'), "NL maps must use Nova's LF-only text convention.");
            string helloLabels = Encoding.ASCII.GetString(disk.Load("HELLO", ".LBL"));
            StringAssert.Contains(helloLabels, "al 00");
            StringAssert.Contains(helloLabels, " .I_P_WRITE_LINE\n");
            Assert.IsFalse(helloLabels.Contains('\r'), "NL labels must use Nova's LF-only text convention.");
            disk.CurrentDirectory = "/";

            QueueLine(editor, "RUN HELLO");
            RunUntil(cpu, bus, s => s.Contains("Hello from NovaPascal", StringComparison.Ordinal), "linked program output");
            StringAssert.Contains(Snapshot(bus), "Running at $8000:");

            disk.Save("LITERALS", Encoding.ASCII.GetBytes(
                "program Literals;\nbegin\n  writeln('');\n  writeln('A');\n" +
                "  writeln('AB');\n  writeln('ABC')\nend.\n"), ".PAS");
            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);
            QueueLine(editor, "COMPILE LITERALS.PAS");
            RunUntil(cpu, bus, s => s.Contains("Writing LITERALS.S", StringComparison.Ordinal),
                "literal lowering compile");
            RunUntil(cpu, bus, s => s.Contains("Compile successful", StringComparison.Ordinal),
                "literal lowering completion");
            string literalAssembly = Encoding.ASCII.GetString(disk.Load("LITERALS", ".S"));
            Assert.AreEqual(6, literalAssembly.Split("JSR P_WRITE_CHAR", StringSplitOptions.None).Length - 1,
                "Zero-, one-, and two-character lines must stay inline, including their LF.");
            Assert.AreEqual(1, literalAssembly.Split("JSR I_P_WRITE_LINE", StringSplitOptions.None).Length - 1,
                "A three-character line must cross to the inline-parameter line writer.");
            StringAssert.Contains(literalAssembly, ".BYTE $41,$42,$43,$00");

            disk.Save("INLINEASM", Encoding.ASCII.GetBytes(
                "program InlineAsm;\nvar\n  Cells: array[0..2] of Byte;\n" +
                "  Index: Word;\n  First, Second, WindowA, WindowB, Result: Byte;\n\n" +
                "procedure Mark;\nbegin\n  writeln('M')\nend;\n\n" +
                "procedure ReadWindow;\nbegin\n  WindowA := Cells[Index + 1];\n  asm\n" +
                "    stz NVR4L\n    stz NVR4H\n  end;\n" +
                "  WindowB := Cells[Index + 2]\nend;\n\n" +
                "function SafeResult(): Byte;\nbegin\n  SafeResult := 7;\n  asm\n" +
                "    stz NVR3L\n  end\nend;\n\nbegin\n  Index := 0;\n" +
                "  Cells[0] := 11;\n  Cells[1] := 22;\n  Cells[2] := 33;\n  First := Cells[Index];\n  asm\n" +
                "    stz NVR2L\n    lda #'@'\n    jsr P_WRITE_CHAR\n" +
                "    lda #$0A\n    jsr P_WRITE_CHAR\n    jsr mark\n  end;\n" +
                "  Second := Cells[Index];\n  Mark;\n  ReadWindow;\n" +
                "  Result := SafeResult();\n  writeln(First);\n  writeln(Second);\n" +
                "  writeln(WindowA);\n  writeln(WindowB);\n  writeln(Result)\nend.\n"), ".PAS");
            disk.Save("INLINEASM", Encoding.ASCII.GetBytes(
                "NPP 1\nMAIN INLINEASM.PAS\nOUTPUT INLINEASM.BIN\n" +
                "OPTIMIZE O2\nMAP INLINEASM.MAP\n"), ".NPP");
            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);
            QueueLine(editor, "BUILD INLINEASM.NPP");
            RunUntil(cpu, bus, s => s.Contains("Build complete: INLINEASM.BIN", StringComparison.Ordinal)
                                    || s.Contains("Pascal syntax error.", StringComparison.Ordinal)
                                    || s.Contains("Assembler error.", StringComparison.Ordinal)
                                    || s.Contains("Linker error.", StringComparison.Ordinal),
                "inline assembly build");
            string inlineBuildScreen = Snapshot(bus);
            Assert.IsTrue(disk.FileExists("INLINEASM", ".BIN"), inlineBuildScreen);
            string inlineAssembly = Encoding.ASCII.GetString(disk.Load("INLINEASM", ".S"));
            StringAssert.Contains(inlineAssembly, "\n     stz NVR2L\n",
                "NPC must preserve inline NAS source while marking it opaque to O2.");
            StringAssert.Contains(inlineAssembly, "\n     lda #'@'\n");
            StringAssert.Contains(inlineAssembly, "\n     jsr mark\n");
            StringAssert.Contains(inlineAssembly, "MARK:",
                "An inline reference must keep the Pascal routine callable.");
            StringAssert.Contains(inlineAssembly, "JSR MARK");
            StringAssert.Contains(inlineAssembly, "READWINDOW:",
                "A routine containing opaque assembly must not be inlined.");
            StringAssert.Contains(inlineAssembly, "SAFERESULT:");
            Assert.IsFalse(inlineAssembly.Contains("LDA (NVR4L),Y", StringComparison.Ordinal),
                "Opaque assembly must disable a whole-routine array window it could clobber.");
            StringAssert.Contains(inlineAssembly, "TSX\nSTA $0101,X",
                "A function containing opaque assembly must retain its stack-backed result.");
            Assert.IsFalse(inlineAssembly.Contains("STA NVR3L", StringComparison.Ordinal),
                "O2 must not keep a function result in scratch across opaque assembly.");
            Assert.IsFalse(inlineAssembly.Contains(".O2", StringComparison.Ordinal),
                "Optimizer IR must never leak into an assembly block or the generated source.");
            StringAssert.Contains(Encoding.ASCII.GetString(disk.Load("INLINEASM", ".MAP")),
                "P_WRITE_CHAR", "Inline assembly references must participate in normal NL library resolution.");

            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);
            QueueLine(editor, "RUN INLINEASM.BIN");
            RunUntil(cpu, bus, s => s.Split('\n').Any(line => line.Trim() == "22"),
                "inline assembly executable output");
            RunSteps(cpu, bus, 100_000);
            string inlineRunScreen = Snapshot(bus);
            Assert.AreEqual(1, inlineRunScreen.Split('\n').Count(line => line.Trim() == "@"),
                $"NAS must execute the copied assembly block exactly once.\n{inlineRunScreen}");
            Assert.AreEqual(2, inlineRunScreen.Split('\n').Count(line => line.Trim() == "M"),
                "Both the inline and Pascal calls must reach the same retained procedure.");
            Assert.AreEqual(2, inlineRunScreen.Split('\n').Count(line => line.Trim() == "11"),
                "Inline assembly must invalidate O2's repeated-array-load cache before subsequent Pascal code.");
            Assert.AreEqual(1, inlineRunScreen.Split('\n').Count(line => line.Trim() == "22"),
                "Array accesses after an opaque block must rebuild their address.");
            Assert.AreEqual(1, inlineRunScreen.Split('\n').Count(line => line.Trim() == "33"),
                "Array-window state must not survive opaque assembly.");
            Assert.AreEqual(1, inlineRunScreen.Split('\n').Count(line => line.Trim() == "7"),
                "A function result must survive opaque assembly.");

            Assert.IsTrue(disk.ListDirectory(null).Any(entry => entry.IsDirectory &&
                entry.Filename.Equals("FIZZBUZZ", StringComparison.OrdinalIgnoreCase)),
                "The development disk must carry the compiler's executable language slice as a project.");
            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);
            QueueLine(editor, "BUILD FIZZBUZZ");
            RunUntil(cpu, bus, s => s.Contains("Build complete: FIZZBUZZ.BIN", StringComparison.Ordinal)
                                    || s.Contains("Assembler error.", StringComparison.Ordinal)
                                    || s.Contains("Linker error.", StringComparison.Ordinal)
                                    || s.Contains("Pascal syntax error.", StringComparison.Ordinal)
                                    || s.Contains("Invalid Pascal project.", StringComparison.Ordinal),
                "FizzBuzz compile, assemble, and link");
            disk.CurrentDirectory = "FIZZBUZZ";
            Assert.IsTrue(disk.FileExists("FIZZBUZZ", ".BIN"),
                $"detail=${bus.Read(0x0276):X2}, object=" +
                $"{(disk.FileExists("FIZZBUZZ", ".OBJ") ? disk.Load("FIZZBUZZ", ".OBJ").Length : 0)} bytes, " +
                $"worker=${bus.Read(0x0903):X2}, objects={bus.Read(0x0901)}, roots={bus.Read(0x0902)}\n{Snapshot(bus)}");
            string fizzBuzzAssembly = Encoding.ASCII.GetString(disk.Load("FIZZBUZZ", ".S"));
            StringAssert.Contains(fizzBuzzAssembly, ".INCLUDE \"PASCAL.INC\"");
            StringAssert.Contains(fizzBuzzAssembly, "STA NVR0L");
            StringAssert.Contains(fizzBuzzAssembly, "CMP #$64");
            Assert.IsFalse(fizzBuzzAssembly.Contains("JSR P_LE", StringComparison.Ordinal),
                "O2 must branch directly from comparison flags instead of materializing a Boolean helper result.");
            Assert.IsFalse(fizzBuzzAssembly.Contains(".O2", StringComparison.Ordinal),
                "Internal optimizer IR must never reach NAS or the user-visible generated assembly.");
            StringAssert.Contains(fizzBuzzAssembly, "JSR P_WRITE_BYTE_LN");
            StringAssert.Contains(fizzBuzzAssembly, "JSR I_P_WRITE_LINE");
            StringAssert.Contains(fizzBuzzAssembly, ".BYTE $46,$69,$7A,$7A,$42,$75,$7A,$7A,$00");
            string fizzBuzzMap = Encoding.ASCII.GetString(disk.Load("FIZZBUZZ", ".MAP"));
            StringAssert.Contains(fizzBuzzMap, "P_WRITE_BYTE_LN");
            StringAssert.Contains(fizzBuzzMap, "I_P_WRITE_LINE");
            disk.CurrentDirectory = "/";

            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);
            QueueLine(editor, "RUN FIZZBUZZ");
            RunUntil(cpu, bus, s => s.Contains("97", StringComparison.Ordinal), "FizzBuzz executable output");
            RunSteps(cpu, bus, 1_000_000);
            string fizzBuzzOutput = string.Join('\n', Snapshot(bus).Split('\n').Select(line => line.Trim()));
            StringAssert.Contains(fizzBuzzOutput,
                "91\n92\nFizz\n94\nBuzz\nFizz\n97\n98\nFizz\nBuzz",
                "FizzBuzz must execute all numeric, MOD, nested IF/ELSE, and WHILE paths through 100.");

            Assert.IsTrue(disk.ListDirectory(null).Any(entry => entry.IsDirectory &&
                entry.Filename.Equals("LIFE", StringComparison.OrdinalIgnoreCase)),
                "The development disk must carry the graphics language slice as a project.");
            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);
            QueueLine(editor, "BUILD LIFE");
            RunUntil(cpu, bus, s => s.Contains("Build complete: LIFE.BIN", StringComparison.Ordinal)
                                    || s.Contains("Assembler error.", StringComparison.Ordinal)
                                    || s.Contains("Linker error.", StringComparison.Ordinal)
                                    || s.Contains("Pascal syntax error.", StringComparison.Ordinal)
                                    || s.Contains("File is too large", StringComparison.Ordinal),
                "Life compile, assemble, and link");
            disk.CurrentDirectory = "LIFE";
            if (!disk.FileExists("LIFE", ".BIN"))
                RunSteps(cpu, bus, 200_000);
            string lifeAssembly = disk.FileExists("LIFE", ".S")
                ? Encoding.ASCII.GetString(disk.Load("LIFE", ".S"))
                : string.Empty;
            byte[] lifeObject = disk.FileExists("LIFE", ".OBJ")
                ? disk.Load("LIFE", ".OBJ")
                : [];
            Assert.IsTrue(disk.FileExists("LIFE", ".BIN"),
                $"detail=${bus.Read(0x0276):X2}, object={lifeObject.Length} bytes, " +
                $"sections={(lifeObject.Length > 6 ? lifeObject[6] : 0)}, " +
                $"symbols={(lifeObject.Length > 11 ? lifeObject[10] | lifeObject[11] << 8 : 0)}, " +
                $"relocations={(lifeObject.Length > 15 ? lifeObject[14] | lifeObject[15] << 8 : 0)}\n{Snapshot(bus)}");
            byte[] lifeExecutable = disk.Load("LIFE", ".BIN");
            CollectionAssert.AreEqual(new byte[] { 0x00, 0x80 }, lifeExecutable[..2]);
            Assert.IsTrue(lifeExecutable.Length <= 5200,
                $"Life O2 output regressed beyond 5,200 bytes; got {lifeExecutable.Length} bytes.");
            string lifeMap = Encoding.ASCII.GetString(disk.Load("LIFE", ".MAP"));
            StringAssert.Contains(lifeMap, "GRAPHICSTILE4X8");
            StringAssert.Contains(lifeMap, "VGC_FILL");
            StringAssert.Contains(lifeMap, "VGC_ISSUE");
            StringAssert.Contains(lifeMap, "RANDOMBYTE");
            StringAssert.Contains(lifeMap, "RNG_GET8");
            StringAssert.Contains(lifeMap, "POLLKEY");
            StringAssert.Contains(lifeMap, "FIO_EXEC");
            Assert.IsFalse(lifeMap.Contains("VGC_PLOT", StringComparison.Ordinal),
                "Selective canonical VGC emission must omit unused primitives.");
            StringAssert.Contains(lifeAssembly, ".INCLUDE \"GRAPHICS.INC\"");
            StringAssert.Contains(lifeAssembly, ".INCLUDE \"GRAPHICS.NPI\"");
            StringAssert.Contains(lifeAssembly, ".INCLUDE \"GRAPHICS.S\"");
            StringAssert.Contains(lifeAssembly, ".INCLUDE \"INPUT.INC\"");
            StringAssert.Contains(lifeAssembly, ".INCLUDE \"INPUT.NPI\"");
            StringAssert.Contains(lifeAssembly, ".INCLUDE \"INPUT.S\"");
            StringAssert.Contains(lifeAssembly, ".INCLUDE \"RANDOM.INC\"");
            StringAssert.Contains(lifeAssembly, ".INCLUDE \"RANDOM.NPI\"");
            StringAssert.Contains(lifeAssembly, ".INCLUDE \"RANDOM.S\"");
            StringAssert.Contains(lifeAssembly, "CELLS: .RES $07D0");
            StringAssert.Contains(lifeAssembly, "NEXT: .RES $07D0");
            StringAssert.Contains(lifeAssembly, "ADC #<CELLS");
            StringAssert.Contains(lifeAssembly, "ADC #>CELLS");
            StringAssert.Contains(lifeAssembly, "STA (NVR1L)");
            StringAssert.Contains(lifeAssembly, "CMP #$19");
            StringAssert.Contains(lifeAssembly, "INC I");
            StringAssert.Contains(lifeAssembly, "INC COL");
            StringAssert.Contains(lifeAssembly, "INC ROW");
            StringAssert.Contains(lifeAssembly, "ADC #<(CELLS-$51)",
                "O2 must form the guarded neighbor window once from its lowest cell offset.");
            StringAssert.Contains(lifeAssembly, "LDA (NVR4L),Y",
                "O2 must reuse a formed array window through indexed-indirect loads.");
            StringAssert.Contains(lifeAssembly, "LDY #$A2",
                "The -81 through +81 neighbor window must span all eight B3/S23 cells.");
            StringAssert.Contains(lifeAssembly,
                "LDA (NVR4L),Y\nCLC\nADC NEIGHBORS\nSTA NEIGHBORS",
                "A pure array read may commute with the accumulator load and eliminate its stack temporary.");
            Assert.IsFalse(lifeAssembly.Contains("LDA NEIGHBORS\nPHA", StringComparison.Ordinal),
                "The hot neighbor sum must not spill its accumulator to the hardware stack.");
            Assert.IsFalse(lifeAssembly.Contains("LDA ROW\nCMP #$00", StringComparison.Ordinal),
                "Unsigned x > 0 must branch from the zero flag without a redundant compare.");
            StringAssert.Contains(lifeAssembly, "STA NVR2L",
                "O2 must retain repeated effect-free array loads within a basic block.");
            StringAssert.Contains(lifeAssembly, "LDA NVR2L");
            foreach (string helper in new[]
                     {
                         "JSR I_P_AGET", "JSR I_P_ASETB", "JSR I_P_ASETW",
                         "JSR P_ADDW", "JSR P_SUBW", "JSR P_CMPW",
                         "JSR P_EQ", "JSR P_NE", "JSR P_LT", "JSR P_LE", "JSR P_GT", "JSR P_GE"
                     })
                Assert.IsFalse(lifeAssembly.Contains(helper, StringComparison.Ordinal),
                    $"O2 must remove generic hot-path helper call: {helper}");
            Assert.IsFalse(lifeAssembly.Contains(".O2", StringComparison.Ordinal),
                "Internal optimizer IR must be fully lowered before NAS.");
            StringAssert.Contains(lifeAssembly, "JMP __NP_MAIN");
            StringAssert.Contains(lifeAssembly, "DRAW:");
            StringAssert.Contains(lifeAssembly, "COMMIT:");
            StringAssert.Contains(lifeAssembly, "\n     lda Next+$0100,x\n",
                "Life must demonstrate inline NAS by copying Pascal arrays directly.");
            StringAssert.Contains(lifeAssembly, "\n     cpx #$D0\n",
                "The inline copy must cover the final 208 cells of the 2,000-cell board.");
            StringAssert.Contains(lifeAssembly, "JSR RANDOMBYTE");
            Assert.IsFalse(lifeAssembly.Contains("JSR COUNTNEIGHBORS", StringComparison.Ordinal));
            Assert.IsFalse(lifeAssembly.Contains("JSR NEXTCELL", StringComparison.Ordinal));
            Assert.IsFalse(lifeAssembly.Contains("COUNTNEIGHBORS:", StringComparison.Ordinal),
                "A single-call leaf routine must be inlined and its dead body removed.");
            Assert.IsFalse(lifeAssembly.Contains("EVOLVE:", StringComparison.Ordinal),
                "Inlining must iterate so a single-call caller becomes eligible after its leaves.");
            Assert.IsFalse(lifeAssembly.Contains("STA $0101,X", StringComparison.Ordinal),
                "A call-free function tail must keep its result in compiler scratch, not a stack slot.");
            StringAssert.Contains(lifeAssembly, "STA NVR3L");
            StringAssert.Contains(lifeAssembly, ".ASSERT __PA02FE8 = $02");
            StringAssert.Contains(lifeAssembly, "JSR GRAPHICSTILE4X8");
            StringAssert.Contains(lifeAssembly, "JSR POLLKEY");

            string lifeSource = Encoding.ASCII.GetString(disk.Load("MAIN", ".PAS"));
            StringAssert.Contains(lifeSource, "RandomByte() < 128",
                "Life must begin from a random, roughly half-full soup rather than one canned organism.");
            StringAssert.Contains(lifeSource, "if Neighbors < 2");
            StringAssert.Contains(lifeSource, "else if Neighbors > 3");
            StringAssert.Contains(lifeSource, "else if Neighbors = 3",
                "Life must encode survival on two or three neighbors and birth on exactly three.");
            StringAssert.Contains(lifeSource, "array[0..1999] of Boolean");
            StringAssert.Contains(lifeSource, "I: Word");
            StringAssert.Contains(lifeSource, "while Row < 25");
            StringAssert.Contains(lifeSource, "while Col < 80");
            StringAssert.Contains(lifeSource, "GraphicsWait(1)");
            StringAssert.Contains(lifeSource, "if Cells[I] <> Next[I]",
                "Life must update changed cells instead of blanking the visible plane.");
            StringAssert.Contains(lifeSource, "lda Next+$0100,x",
                "Life must retain its inline-assembly example on the development disk.");
            Assert.IsFalse(lifeSource.Contains("GraphicsClear", StringComparison.Ordinal),
                "Life must not flicker by clearing between generations.");
            StringAssert.Contains(lifeSource, "while Key <> 13");
            disk.CurrentDirectory = "/";

            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);
            QueueLine(editor, "RUN LIFE");
            int initialPopulation = WaitForStableLifePopulation(
                cpu, bus, minimum: 400, "a random Life soup across the graphics plane");
            int evolvedPopulation = 0;
            RunUntil(cpu, bus, _ =>
            {
                evolvedPopulation = CountLifeCells(bus);
                return evolvedPopulation > 0 && evolvedPopulation != initialPopulation;
            }, "an evolved Life generation without a blank frame");
            Assert.AreNotEqual(initialPopulation, evolvedPopulation,
                "Applying B3/S23 to a random soup must change its population.");
            editor.QueueInput(0x0D);
            RunUntil(cpu, bus, s =>
                    bus.Read((ushort)VgcConstants.RegMode) == VgcConstants.ModeTextOnly
                    && s.Contains("NP> ", StringComparison.Ordinal),
                "Life return to the Pascal shell");

            disk.Save("RETSAFE", Encoding.ASCII.GetBytes(
                "program RetSafe;\nuses NovaRandom;\nvar Sample: Byte;\n" +
                "function Keep(): Byte;\nbegin\n  Keep := 7;\n  Sample := RandomByte()\nend;\n" +
                "begin\n  writeln(Keep())\nend.\n"), ".PAS");
            disk.Save("RETSAFE", Encoding.ASCII.GetBytes(
                "NPP 1\nMAIN RETSAFE.PAS\nOUTPUT RETSAFE.BIN\nOPTIMIZE O2\n" +
                "CONFIG INLINE\nMAP RETSAFE.MAP\nLABEL RETSAFE.LBL\n" +
                "MEMORY {\n    RAM: start = $8000, size = $1000, file = %O;\n}\n\n" +
                "SEGMENTS {\n    CODE: load = RAM, type = ro;\n    BSS: load = RAM, type = bss;\n}\n"), ".NPP");
            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);
            QueueLine(editor, "BUILD RETSAFE.NPP");
            RunUntil(cpu, bus, s => s.Contains("Build complete: RETSAFE.BIN", StringComparison.Ordinal)
                                    || s.Contains("error", StringComparison.OrdinalIgnoreCase),
                "effectful function-result fallback build");
            Assert.IsTrue(disk.FileExists("RETSAFE", ".S"), Snapshot(bus));
            string returnSafeAssembly = Encoding.ASCII.GetString(disk.Load("RETSAFE", ".S"));
            StringAssert.Contains(returnSafeAssembly, "TSX\nSTA $0101,X",
                "A function that calls after assigning its result must retain the stack-backed result ABI.");
            Assert.AreEqual(1,
                returnSafeAssembly.Split("JSR RANDOMBYTE", StringSplitOptions.None).Length - 1,
                "Effect analysis must preserve the original call exactly once while reconstructing typed IR.");
            Assert.IsFalse(returnSafeAssembly.Contains("STA NVR3L", StringComparison.Ordinal),
                "Effect analysis must not place a result across an unknown call in volatile scratch.");
            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);
            QueueLine(editor, "RUN RETSAFE.BIN");
            RunUntil(cpu, bus, s => s.Split('\n').Any(line => line.Trim() == "7"),
                "stack-preserved function result");
            RunUntil(cpu, bus, s => s.Contains("NP> ", StringComparison.Ordinal),
                "effectful function return to the Pascal shell");
            foreach (string extension in new[] { ".PAS", ".NPP", ".S", ".OBJ", ".BIN", ".MAP", ".LBL" })
                disk.Delete("RETSAFE", extension);

            disk.Save("BADPAS", Encoding.ASCII.GetBytes(
                "program BadPas;\nbegin\n  writeln(\nend.\n"), ".PAS");
            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);
            QueueLine(editor, "COMPILE BADPAS.PAS");
            RunUntil(cpu, bus, s => s.Contains("BADPAS.PAS:", StringComparison.Ordinal)
                                    && s.Contains(": error: Pascal syntax error.", StringComparison.Ordinal),
                "source-located Pascal diagnostic");
            Assert.IsFalse(disk.FileExists("BADPAS", ".S"),
                "NPC must not leave an assembly artifact after a syntax failure.");

            disk.Save("NDKPAS", Encoding.ASCII.GetBytes(
                "program NdkPas;\nuses NovaRng, NovaFio;\n" +
                "var Status, Sample: Byte;\nbegin\n" +
                "  Status := 0;\n" +
                "  Status := $00;\n" +
                "  Status := 'A';\n" +
                "  Status := rng_get8();\n" +
                "  Status := fio_exec(Byte(FIO_CMD_RNG));\n" +
                "  fio_issue(Status);\n" +
                "  Sample := RNG_VALUE0;\n" +
                "  VGC_BORDER := Sample;\n" +
                "  writeln('Pascal NDK');\nend.\n"), ".PAS");
            disk.Save("NDKPAS", Encoding.ASCII.GetBytes(
                "NPP 1\nMAIN NDKPAS.PAS\nOUTPUT NDKPAS.BIN\nCONFIG INLINE\n" +
                "MAP NDKPAS.MAP\nLABEL NDKPAS.LBL\n" +
                "MEMORY {\n    RAM: start = $8000, size = $0200, file = %O;\n}\n\n" +
                "SEGMENTS {\n    CODE: load = RAM, type = ro;\n" +
                "    BSS: load = RAM, type = bss;\n}\n"), ".NPP");
            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);
            QueueLine(editor, "BUILD NDKPAS.NPP");
            RunUntil(cpu, bus, s => s.Contains("Build complete: NDKPAS.BIN", StringComparison.Ordinal)
                                    || s.Contains("error", StringComparison.OrdinalIgnoreCase),
                "Pascal NDK unit build");
            Assert.IsTrue(disk.FileExists("NDKPAS", ".BIN"), Snapshot(bus));
            string ndkAssembly = Encoding.ASCII.GetString(disk.Load("NDKPAS", ".S"));
            StringAssert.Contains(ndkAssembly, ".INCLUDE \"RNG.INC\"");
            StringAssert.Contains(ndkAssembly, ".INCLUDE \"RNG.NPI\"");
            StringAssert.Contains(ndkAssembly, "JSR RNG_GET8");
            StringAssert.Contains(ndkAssembly, ".ASSERT (__S8578E3) = $02");
            StringAssert.Contains(ndkAssembly, ".ASSERT __CBAB93C = 1\nLDA #FIO_CMD_RNG");
            StringAssert.Contains(ndkAssembly, ".ASSERT (__S34C9C7) = $03\nJSR FIO_EXEC");
            StringAssert.Contains(ndkAssembly, ".ASSERT (__S13E4EB & $01) = $01");
            StringAssert.Contains(ndkAssembly, "LDA #$41\nSTA STATUS");
            StringAssert.Contains(ndkAssembly, "STA STATUS");
            StringAssert.Contains(ndkAssembly,
                "LDA STATUS\n.ASSERT (__S13E4EB & $01) = $01\nJSR FIO_ISSUE");
            StringAssert.Contains(ndkAssembly, "LDA RNG_VALUE0\nSTA SAMPLE");
            StringAssert.Contains(ndkAssembly, "LDA SAMPLE\nSTA VGC_BORDER");
            StringAssert.Contains(ndkAssembly,
                ".SEGMENT \"BSS\"\nSTATUS: .RES 1\nSAMPLE: .RES 1");
            StringAssert.Contains(ndkAssembly, ".INCLUDE \"RNG.S\"");
            StringAssert.Contains(ndkAssembly, ".INCLUDE \"FIO.S\"");
            Assert.IsFalse(ndkAssembly.Contains("$A", StringComparison.Ordinal),
                "NPC must emit NDK symbols and canonical includes, not hardware addresses.");
            QueueLine(editor, "RUN NDKPAS.BIN");
            RunUntil(cpu, bus, s => s.Contains("Pascal NDK", StringComparison.Ordinal),
                "Pascal executable using canonical NDK units");
            Assert.AreEqual(bus.Read((ushort)VgcConstants.FioRng0),
                bus.Read((ushort)VgcConstants.RegBorder),
                "The executable must copy the hardware RNG byte through Pascal's NDK byte ABI.");

            disk.Save("NDKCAT", Encoding.ASCII.GetBytes(
                "program NdkCatalog;\nuses NovaAudio, NovaVgc, NovaXram, NovaDma, NovaVgcVsync;\n" +
                "var Status: Byte;\nbegin\n" +
                "  Status := audio_status();\n" +
                "  vgc_display_on;\n" +
                "  xram_copy_from_ram;\n" +
                "  Status := dma_wait();\n" +
                "  xram_set_ok;\n" +
                "  Status := vgc_vsync();\n" +
                "  writeln('NDK catalog')\nend.\n"), ".PAS");
            disk.Save("NDKCAT", Encoding.ASCII.GetBytes(
                "NPP 1\nMAIN NDKCAT.PAS\nOUTPUT NDKCAT.BIN\nCONFIG INLINE\n" +
                "MAP NDKCAT.MAP\nLABEL NDKCAT.LBL\n" +
                "MEMORY {\n    RAM: start = $8000, size = $1000, file = %O;\n}\n\n" +
                "SEGMENTS {\n    CODE: load = RAM, type = ro;\n    BSS: load = RAM, type = bss;\n}\n"), ".NPP");
            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);
            QueueLine(editor, "BUILD NDKCAT.NPP");
            RunUntil(cpu, bus, _ => disk.FileExists("NDKCAT", ".BIN") || bus.Read(0x0275) == 5,
                "generated NDK unit catalog build");
            byte[] catalogObject = disk.FileExists("NDKCAT", ".OBJ") ? disk.Load("NDKCAT", ".OBJ") : [];
            Assert.IsTrue(disk.FileExists("NDKCAT", ".BIN"),
                $"detail=${bus.Read(0x0276):X2}, object={catalogObject.Length} bytes, " +
                $"sections={(catalogObject.Length > 6 ? catalogObject[6] : 0)}, " +
                $"symbols={(catalogObject.Length > 11 ? catalogObject[10] | catalogObject[11] << 8 : 0)}, " +
                $"relocations={(catalogObject.Length > 15 ? catalogObject[14] | catalogObject[15] << 8 : 0)}, " +
                $"header={Convert.ToHexString(catalogObject[..Math.Min(catalogObject.Length, 80)])}\n{Snapshot(bus)}");
            string catalogAssembly = Encoding.ASCII.GetString(disk.Load("NDKCAT", ".S"));
            StringAssert.Contains(catalogAssembly, ".INCLUDE \"AUDIO.S\"");
            StringAssert.Contains(catalogAssembly, ".INCLUDE \"VGCVSYNC.S\"");
            StringAssert.Contains(catalogAssembly, "JSR AUDIO_STATUS");
            Assert.IsTrue(disk.Load("NDKCAT", ".BIN").Length <= 768,
                "Cross-unit dependency closure must retain only selected XRAM and DMA routines.");
            foreach (string extension in new[] { ".PAS", ".NPP", ".S", ".OBJ", ".BIN", ".MAP", ".LBL" })
            {
                if (disk.FileExists("NDKCAT", extension))
                    disk.Delete("NDKCAT", extension);
            }

            disk.Save("BADABI", Encoding.ASCII.GetBytes(
                "program BadAbi;\nuses NovaRng, NovaFio;\nbegin\n  rng_get8(0);\nend.\n"), ".PAS");
            disk.Save("BADABI", Encoding.ASCII.GetBytes(
                "NPP 1\nMAIN BADABI.PAS\nOUTPUT BADABI.BIN\nCONFIG INLINE\n" +
                "MAP BADABI.MAP\nLABEL BADABI.LBL\n" +
                "MEMORY {\n    RAM: start = $8000, size = $0200, file = %O;\n}\n\n" +
                "SEGMENTS {\n    CODE: load = RAM, type = ro;\n}\n"), ".NPP");
            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);
            QueueLine(editor, "BUILD BADABI.NPP");
            RunUntil(cpu, bus, s => s.Contains("assertion failed", StringComparison.OrdinalIgnoreCase),
                "generated Pascal NDK signature rejection");
            Assert.IsFalse(disk.FileExists("BADABI", ".BIN"),
                "A Pascal call with the wrong NDK arity must fail before linking.");

            RunSteps(cpu, bus, 100_000);
            disk.Save("TINY", Encoding.ASCII.GetBytes(
                "mEmOrY { ram: StArT = $8000, SiZe = $0001, FiLe = %o; }\n"), ".CFG");
            disk.Save("TINY", Encoding.ASCII.GetBytes(
                "NPP 1\nMAIN LITERALS.PAS\nOUTPUT TINY.BIN\nCONFIG TINY.CFG\n"), ".NPP");
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

            QueueLine(editor, "TYPE HELLO/HELLO.S");
            RunUntil(cpu, bus, s => s.Contains("JSR I_P_WRITE_LINE", StringComparison.Ordinal),
                "TYPE displays generated assembly text");

            QueueLine(editor, "EDIT HELLO/MAIN.PAS");
            RunUntil(cpu, bus, s => s.Contains("MAIN.PAS", StringComparison.Ordinal)
                                    && s.Contains("program Hello;", StringComparison.Ordinal), "Pascal editor");
            RunSteps(cpu, bus, 100_000);
            Assert.AreEqual((byte)0x63, bus.Vgc.GetScreenColor(0, 1),
                "NPEDIT must apply Pascal lexical color to source words.");
            Assert.AreEqual((byte)0x65, bus.Vgc.GetScreenColor(10, 3),
                "NPEDIT must apply Pascal string color to quoted literals.");
            editor.QueueInput(0x1B);
            editor.QueueInput((byte)'x');
            RunUntil(cpu, bus, s => s.Contains("NP> ", StringComparison.Ordinal)
                                    && !s.Contains("program Hello;", StringComparison.Ordinal), "shell after Alt-X");

            QueueLine(editor, "EDIT HELLO/HELLO.S");
            RunUntil(cpu, bus, s => s.Contains("JSR I_P_WRITE_LINE", StringComparison.Ordinal)
                                    && s.Contains("T:Assembly Source", StringComparison.Ordinal),
                "assembly-aware editor");
            editor.QueueInput(0x11);
            RunUntil(cpu, bus, s => s.Contains("NP> ", StringComparison.Ordinal), "shell after Ctrl-Q");

            QueueLine(editor, "EDIT HELLO/HELLO.NPP");
            RunUntil(cpu, bus, s => s.Contains("MAIN MAIN.PAS", StringComparison.Ordinal)
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

            QueueLine(editor, "TYPE HELLO/HELLO.BIN");
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
                "NPP 1\nMAIN LITERALS.PAS\nOUTPUT MULTIPRJ.BIN\nOBJECT MULTIUTIL.OBJ\n"), ".NPP");
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
                "NPP 1\nMAIN LITERALS.PAS\nOUTPUT ASMPROJ.BIN\n" +
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

            disk.Save("TOOBIG", Encoding.ASCII.GetBytes(new string(' ', 32_769)), ".S");
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
            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);
            QueueLine(editor, "ASSEMBLE INCLUDE.S");
            RunUntil(cpu, bus, s => s.Contains("Writing INCLUDE.OBJ", StringComparison.Ordinal)
                                    || s.Contains(": error:", StringComparison.Ordinal),
                "nested include assembly result");
            RunSteps(cpu, bus, 500_000);
            Assert.IsTrue(disk.FileExists("INCLUDE", ".OBJ"),
                $"NAS nested include failed with detail ${bus.Read(0x0276):X2}:\n{Snapshot(bus)}");
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

            disk.Save("ZEROPAGE", Encoding.ASCII.GetBytes(
                ".globalzp Scratch\n.export Start\nStart:\nlda Scratch\nsta Scratch,x\nrts\n"), ".S");
            QueueLine(editor, "ASSEMBLE ZEROPAGE.S");
            RunUntil(cpu, bus, s => s.Contains("Writing ZEROPAGE.OBJ", StringComparison.Ordinal)
                                    || s.Contains(": error:", StringComparison.Ordinal),
                "global zero-page assembly");
            byte[] zeroPageObject = disk.Load("ZEROPAGE", ".OBJ");
            CollectionAssert.AreEqual(new byte[] { 0xA5, 0x00, 0x95, 0x00, 0x60 },
                ReadNobjSectionData(zeroPageObject, "CODE"),
                ".GLOBALZP must select zero-page opcodes for unresolved NDK symbols.");
            int zeroPageReloc = zeroPageObject[16] | zeroPageObject[17] << 8;
            Assert.AreEqual(2, zeroPageObject[zeroPageReloc + 1]);
            Assert.AreEqual(2, zeroPageObject[zeroPageReloc + 9],
                ".GLOBALZP operands must use NL's range-checked ABS8 relocation.");

            disk.Save("NOVA", File.ReadAllBytes(RepoPath(
                "software", "runtime", "asm", "nova.inc")), ".INC");
            disk.Save("DMA", File.ReadAllBytes(RepoPath(
                "software", "runtime", "asm", "dma.inc")), ".INC");
            disk.Save("DMA", File.ReadAllBytes(RepoPath(
                "software", "runtime", "asm", "dma.s")), ".S");
            disk.Save("NDKDMA", Encoding.ASCII.GetBytes(
                ".setcpu \"65c02\"\n.include \"dma.inc\"\n.segment \"CODE\"\n.export Start\n" +
                "Start:\nlda #DMA_SPACE_CPU\nsta DMA_DSTSPACE\nstz DMA_DSTL\n" +
                "lda #$60\nsta DMA_DSTM\nstz DMA_DSTH\nlda #'N'\nsta DMA_FILLVALUE\n" +
                "lda #1\nsta DMA_LENL\nstz DMA_LENM\nstz DMA_LENH\njsr dma_fill\n" +
                "lda $6000\nsta VGC_CHAROUT\nrts\n.include \"dma.s\"\n"), ".S");
            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);
            QueueLine(editor, "ASSEMBLE NDKDMA.S");
            RunUntil(cpu, bus, s => s.Contains("Writing NDKDMA.OBJ", StringComparison.Ordinal)
                                    || s.Contains(": error:", StringComparison.Ordinal),
                "canonical NDK source assembly");
            Assert.IsTrue(disk.FileExists("NDKDMA", ".OBJ"), Snapshot(bus));
            QueueLine(editor, "LINK NDKDMA.OBJ");
            RunUntil(cpu, bus, s => s.Contains("Writing NDKDMA.BIN", StringComparison.Ordinal)
                                    || s.Contains("Linker error", StringComparison.Ordinal),
                "canonical NDK source link");
            QueueLine(editor, "RUN NDKDMA.BIN");
            RunUntil(cpu, bus, s => s.Contains('N'),
                "executable assembled directly from canonical NDK sources");

            disk.Save("FIO", File.ReadAllBytes(RepoPath(
                "software", "runtime", "asm", "fio.inc")), ".INC");
            disk.Save("FIO", File.ReadAllBytes(RepoPath(
                "software", "runtime", "asm", "fio.s")), ".S");
            disk.Save("RNG", File.ReadAllBytes(RepoPath(
                "software", "runtime", "asm", "rng.inc")), ".INC");
            disk.Save("RNG", File.ReadAllBytes(RepoPath(
                "software", "runtime", "asm", "rng.s")), ".S");
            disk.Save("NDKRNG", Encoding.ASCII.GetBytes(
                ".setcpu \"65c02\"\n.include \"rng.inc\"\n.segment \"CODE\"\n.export Start\n" +
                "Start:\njsr rng_get8\nlda #'R'\nsta VGC_CHAROUT\nrts\n" +
                ".include \"rng.s\"\n.include \"fio.s\"\n"), ".S");
            bus.Write((ushort)VgcConstants.RegCharOut, 0x0C);
            QueueLine(editor, "ASSEMBLE NDKRNG.S");
            RunUntil(cpu, bus, s => s.Contains("Writing NDKRNG.OBJ", StringComparison.Ordinal)
                                    || s.Contains(": error:", StringComparison.Ordinal),
                "selective canonical NDK source assembly");
            Assert.IsTrue(disk.FileExists("NDKRNG", ".OBJ"), Snapshot(bus));
            byte[] rngObject = disk.Load("NDKRNG", ".OBJ");
            Assert.IsTrue(NobjDefines(rngObject, "fio_exec"));
            Assert.IsTrue(NobjDefines(rngObject, "fio_check"));
            Assert.IsFalse(NobjDefines(rngObject, "fio_save"),
                ".REFERENCED/.REFTO must omit unused canonical NDK routines before NOBJ emission.");
            QueueLine(editor, "LINK NDKRNG.OBJ");
            RunUntil(cpu, bus, s => s.Contains("Writing NDKRNG.BIN", StringComparison.Ordinal)
                                    || s.Contains("Linker error", StringComparison.Ordinal),
                "selective canonical NDK source link");
            QueueLine(editor, "RUN NDKRNG.BIN");
            RunUntil(cpu, bus, s => s.Contains('R'),
                "executable assembled from selectively emitted canonical NDK sources");

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
            disk.Delete("IMPLIED", ".S");
            disk.Delete("IMPLIED", ".OBJ");

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
            Assert.IsTrue(disk.FileExists("BITOPS", ".OBJ"),
                $"NAS did not write BITOPS.OBJ (status ${bus.Read(0x0275):X2}, " +
                $"detail ${bus.Read(0x0276):X2}):\n{Snapshot(bus)}");
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
            disk.Delete("BITOPS", ".S");
            disk.Delete("BITOPS", ".OBJ");

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

            disk.CurrentDirectory = "HELLO";
            Assert.IsTrue(disk.FileExists("HELLO", ".S"), "BUILD must retain generated assembly.");
            Assert.IsTrue(disk.FileExists("HELLO", ".OBJ"), "BUILD must retain the NOBJ object.");
            Assert.IsTrue(disk.FileExists("HELLO", ".BIN"), "BUILD must retain the linked binary.");

            string assembly = Encoding.ASCII.GetString(disk.Load("HELLO", ".S"));
            StringAssert.Contains(assembly, ".IMPORT P_WRITE_CHAR");
            StringAssert.Contains(assembly, ".IMPORT I_P_WRITE_LINE");
            StringAssert.Contains(assembly, "; MAIN.PAS:3 WRITELN");
            StringAssert.Contains(assembly, "JSR I_P_WRITE_LINE");
            StringAssert.Contains(assembly,
                ".BYTE $48,$65,$6C,$6C,$6F,$20,$66,$72,$6F,$6D,$20,$4E,$6F,$76,$61,$50,$61,$73,$63,$61,$6C,$00");
            Assert.IsFalse(assembly.Contains("LDA #$48", StringComparison.Ordinal),
                "Long string literals must not be lowered character by character.");
            Assert.IsFalse(assembly.Contains('\r'), "NPC output must use Nova's LF-only text convention.");
            Assert.IsFalse(assembly.Contains("LDA #$0D", StringComparison.Ordinal),
                "WRITELN must emit one Nova LF, not a redundant CR/LF pair.");
            Assert.IsFalse(assembly.Contains("$A00E", StringComparison.Ordinal),
                "NPC must emit a symbol, not a hard-coded hardware address.");

            byte[] objectFile = disk.Load("HELLO", ".OBJ");
            int codeLength = objectFile[26] | objectFile[27] << 8;
            Assert.AreEqual(26, codeLength,
                "A 21-character WRITELN should compile to JSR, 22 inline bytes, and RTS.");
            Assert.IsTrue((objectFile[14] | objectFile[15] << 8) > 0,
                "NAS must preserve external calls as NOBJ relocations.");
            disk.CurrentDirectory = "/";
            byte[] library = disk.Load("PASCAL", ".NLIB");
            disk.CurrentDirectory = "HELLO";
            Assert.AreEqual(2, library[4], "Pascal libraries must use complete NOBJ members.");
            byte[] lineMember = ReadNlibMember(library, "I_P_WRITE_LINE");
            byte[] unusedMember = ReadNlibMember(library, "P_UNUSED");
            byte[] lineCode = ReadNobjSectionData(lineMember, "CODE");
            byte[] unusedCode = ReadNobjSectionData(unusedMember, "CODE");
            byte[] executable = disk.Load("HELLO", ".BIN");
            Assert.AreEqual(2 + codeLength + lineCode.Length, executable.Length,
                "NL must extract the complete transitive member set without NOBJ metadata or padding.");
            CollectionAssert.AreEqual(new byte[] { 0xA9, 0x0A, 0x8D, 0x0E, 0xA0, 0x60 }, executable[^6..],
                "The inline line writer must end by emitting Nova LF through the NDK address.");
            Assert.IsFalse(ContainsSequence(executable, unusedCode),
                "NL must strip unreferenced library members.");
            disk.CurrentDirectory = "/";
        }
        finally
        {
            try { Directory.Delete(root, recursive: true); } catch { }
        }
    }

    [TestMethod]
    public void GenericEditorPagesAndSavesLargeXramDocuments()
    {
        string root = Path.Combine(Path.GetTempPath(), $"novapascal-editor-{Guid.NewGuid():N}");
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
            RunUntil(cpu, bus, s => s.Contains("NovaPascal Shell v1.0", StringComparison.Ordinal),
                "shell banner");

            var disk = bus.DeviceManager.GetDevice("FD0");
            string bigText = "FIRST-XRAM-PAGE\n" + string.Concat(
                Enumerable.Range(0, 300).Select(i => $"line {i:D3}: paging belongs to the generic editor\n"))
                + "END-OF-XRAM-DOCUMENT";
            Assert.IsTrue(bigText.Length > 4096, "The fixture must exceed NPEDIT's RAM window.");
            disk.Save("BIGTEXT", Encoding.ASCII.GetBytes(bigText), ".TXT");

            QueueLine(editor, "EDIT BIGTEXT.TXT");
            RunUntil(cpu, bus, s => s.Contains("FIRST-XRAM-PAGE", StringComparison.Ordinal)
                                    && s.Contains("T:Text", StringComparison.Ordinal),
                "first XRAM-backed editor page");
            editor.QueueInput(0x81); // Ctrl-End
            RunUntil(cpu, bus, s => s.Contains("END-OF-XRAM-DOCUMENT", StringComparison.Ordinal),
                "last XRAM-backed editor page");
            editor.QueueInput((byte)'!');
            editor.QueueInput(0x13); // Ctrl-S
            editor.QueueInput(0x11); // Ctrl-Q
            RunUntil(cpu, bus, s => s.Contains("NP> ", StringComparison.Ordinal),
                "shell after saving a paged document");

            CollectionAssert.AreEqual(Encoding.ASCII.GetBytes(bigText + "!"), disk.Load("BIGTEXT", ".TXT"),
                "The generic editor must commit and save an edit made in a nonresident XRAM window.");
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

        Assert.Fail($"Timed out waiting for {expected}. PC=${cpu.Pc:X4}, " +
                    $"tool status=${bus.Read(0x0275):X2}, detail=${bus.Read(0x0276):X2}, " +
                    $"line={bus.Read(0x02F3) | bus.Read(0x02F4) << 8}, " +
                    $"column={bus.Read(0x02F5) | bus.Read(0x02F6) << 8}, " +
                    $"source={ReadMailboxText(bus, 0x0800, 64)}\n{Snapshot(bus)}");
    }

    private static string ReadMailboxText(CompositeBusDevice bus, ushort address, int capacity)
    {
        var bytes = new byte[capacity];
        int length = 0;
        while (length < bytes.Length && (bytes[length] = bus.Read((ushort)(address + length))) != 0)
            length++;
        return Encoding.ASCII.GetString(bytes, 0, length);
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

    private static int WaitForStableLifePopulation(
        Cpu cpu, CompositeBusDevice bus, int minimum, string expected)
    {
        int previous = -1;
        int stableChecks = 0;
        int population = 0;
        RunUntil(cpu, bus, _ =>
        {
            if (bus.Read((ushort)VgcConstants.RegMode) != VgcConstants.ModeGfxOnly)
                return false;

            population = CountLifeCells(bus);
            if (population < minimum)
            {
                previous = population;
                stableChecks = 0;
                return false;
            }

            stableChecks = population == previous ? stableChecks + 1 : 0;
            previous = population;
            return stableChecks >= 3;
        }, expected);
        return population;
    }

    private static int CountLifeCells(CompositeBusDevice bus)
    {
        int population = 0;
        for (int row = 0; row < 25; row++)
        {
            for (int col = 0; col < 80; col++)
            {
                if (bus.Vgc.GetGfxPixelColor(2 + col * 4, 4 + row * 8) == 10)
                    population++;
            }
        }
        return population;
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

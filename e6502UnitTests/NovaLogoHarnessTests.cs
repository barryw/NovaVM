using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using e6502.Avalonia.Hardware;
using e6502.Avalonia.Input;
using KDS.e6502;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class NovaLogoHarnessTests
{
    private const ushort TurtleStateBase = 0x9F00;
    private const ushort TurtleXLo = TurtleStateBase + 1;
    private const ushort TurtleXHi = TurtleStateBase + 2;
    private const ushort TurtleYLo = TurtleStateBase + 4;
    private const ushort TurtleYHi = TurtleStateBase + 5;
    private const ushort TurtleHeadingLo = TurtleStateBase + 6;
    private const ushort TurtleHeadingHi = TurtleStateBase + 7;

    [TestMethod]
    public void EveryRegisteredCommandHasHarnessCoverage()
    {
        var registered = LoadRegisteredLogoCommands();
        var covered = HarnessCases()
            .SelectMany(c => c.Covers)
            .ToHashSet(StringComparer.Ordinal);

        var missing = registered.Except(covered, StringComparer.Ordinal).Order(StringComparer.Ordinal).ToArray();
        var stale = covered.Except(registered, StringComparer.Ordinal).Order(StringComparer.Ordinal).ToArray();

        Assert.AreEqual(0, missing.Length,
            "Every command in novalogo's builtin and extension command tables must have a harness case. Missing: "
            + string.Join(", ", missing));
        Assert.AreEqual(0, stale.Length,
            "Harness coverage names must match real command table entries. Stale: "
            + string.Join(", ", stale));
    }

    [TestMethod]
    public void LogoFeatureHarnessRunsRepresentativePrograms()
    {
        foreach (LogoHarnessCase testCase in HarnessCases())
            RunHarnessCase(testCase);
    }

    private static IEnumerable<LogoHarnessCase> HarnessCases()
    {
        yield return new LogoHarnessCase(
            "core expressions, math, and predicates",
            [
                Line("PRINT 3 + 4"),
                Line("TYPE \"A"),
                Line("PRINT \"B"),
                Line("MAKE \"N 21"),
                Line("PRINT :N * 2"),
                Line("PRINT ABS -5"),
                Line("PRINT INT 3"),
                Line("PRINT ROUND 3"),
                Line("PRINT SQRT 9"),
                Line("PRINT REMAINDER 10 3"),
                Line("PRINT SIN 0"),
                Line("PRINT COS 0"),
                Line("PRINT NOT 0"),
                Line("PRINT AND 1 1"),
                Line("PRINT OR 0 1"),
                Line("PRINT NUMBER? 42"),
                Line("PRINT WORD? \"HI"),
                Line("PRINT LIST? [1 2]"),
                Line("PRINT CHAR 66"),
                Line("PRINT ASCII \"A"),
                Line("PRINT RANDOM 4"),
                Line("PRINT \"CORE_DONE")
            ],
            "CORE_DONE",
            [
                "PRINT", "TYPE", "MAKE", "ABS", "INT", "ROUND", "SQRT", "REMAINDER",
                "SIN", "COS", "NOT", "AND", "OR", "NUMBER?", "WORD?", "LIST?", "CHAR", "ASCII", "RANDOM"
            ],
            screen =>
            {
                AssertScreenContains(screen, "7");
                AssertScreenContains(screen, "AB");
                AssertScreenContains(screen, "42");
                AssertScreenContains(screen, "65");
            });

        yield return new LogoHarnessCase(
            "control flow loops and conditionals",
            [
                Line("MAKE \"X 1"),
                Line("IF :X = 1 [PRINT \"IFOK]"),
                Line("IFELSE :X = 0 [PRINT \"BAD] [PRINT \"ELSEOK]"),
                Line("REPEAT 3 [TYPE REPCOUNT]"),
                Line("PRINT \"R"),
                Line("FOR [I 1 3] [TYPE :I]"),
                Line("PRINT \"F"),
                Line("WHILE [:X < 3] [TYPE :X MAKE \"X :X + 1]"),
                Line("PRINT \"W"),
                Line("UNTIL [:X = 5] [TYPE :X MAKE \"X :X + 1]"),
                Line("PRINT \"U"),
                Line("PRINT \"FLOW_DONE")
            ],
            "FLOW_DONE",
            ["IF", "IFELSE", "REPEAT", "REPCOUNT", "FOR", "WHILE", "UNTIL"],
            screen =>
            {
                AssertScreenContains(screen, "IFOK");
                AssertScreenContains(screen, "ELSEOK");
                AssertScreenContains(screen, "123? PRINT \"R");
                AssertScreenContains(screen, "123? PRINT \"F");
            });

        yield return new LogoHarnessCase(
            "procedure output reporter",
            [
                Line("TO DOUBLE :N"),
                Line("OUTPUT :N * 2"),
                SaveAndQuit(),
                Line("PRINT DOUBLE 9"),
                Line("PRINT \"OUTPUT_DONE")
            ],
            "OUTPUT_DONE",
            ["TO", "OUTPUT"],
            screen =>
            {
                AssertScreenContains(screen, "18");
            });

        yield return new LogoHarnessCase(
            "procedures, stop, catch/throw",
            [
                Line("TO STOPPER"),
                Line("PRINT 61"),
                Line("STOP"),
                Line("PRINT 62"),
                SaveAndQuit(),
                Line("STOPPER"),
                Line("CATCH \"DONE [THROW \"DONE PRINT 63]"),
                Line("PRINT \"CONTROL_DONE")
            ],
            "CONTROL_DONE",
            ["STOP", "CATCH", "THROW"],
            screen =>
            {
                AssertScreenContains(screen, "61");
            });

        yield return new LogoHarnessCase(
            "list processing, word construction, run, and keyboard readers",
            [
                Line("SHOW [1 2 3]"),
                Line("PRINT FIRST [10 20 30]"),
                Line("PRINT BUTFIRST [10 20 30]"),
                Line("PRINT BF [10 20 30]"),
                Line("PRINT COUNT [1 2 3]"),
                Line("PRINT EMPTY? []"),
                Line("PRINT LAST [10 20 30]"),
                Line("PRINT BUTLAST [10 20 30]"),
                Line("PRINT BL [10 20 30]"),
                Line("PRINT ITEM 2 [10 20 30]"),
                Line("PRINT MEMBER? 20 [10 20 30]"),
                Line("SHOW FPUT 1 [2 3]"),
                Line("SHOW LPUT 3 [1 2]"),
                Line("SHOW LIST 1 2"),
                Line("SHOW SENTENCE [1 2] [3 4]"),
                Line("SHOW SE [A B] [C]"),
                Line("PRINT WORD \"HE \"LLO"),
                Line("MAKE \"CMD [PRINT 77]"),
                Line("RUN :CMD"),
                Line("PRINT READCHAR"),
                Raw("Z"),
                Line("PRINT READWORD"),
                Raw("WORDIN\r"),
                Line("SHOW READLIST"),
                Raw("1 2 THREE\r"),
                Line("PRINT \"LIST_DONE")
            ],
            "LIST_DONE",
            [
                "FIRST", "BUTFIRST", "BF", "COUNT", "EMPTY?", "LAST", "BUTLAST", "BL", "ITEM", "MEMBER?",
                "SHOW", "FPUT", "LPUT", "LIST", "SENTENCE", "SE", "WORD", "RUN", "READCHAR", "READWORD", "READLIST"
            ],
            screen =>
            {
                AssertScreenContains(screen, "[1 2 3]");
                AssertScreenContains(screen, "[20 30]");
                AssertScreenContains(screen, "[10 20]");
                AssertScreenContains(screen, "HELLO");
                AssertScreenContains(screen, "77");
                AssertScreenContains(screen, "Z");
                AssertScreenContains(screen, "WORDIN");
                AssertScreenContains(screen, "[1 2 THREE]");
            });

        yield return new LogoHarnessCase(
            "workspace commands and APPLY",
            [
                Line("TO H1 :N"),
                Line("PRINT :N"),
                SaveAndQuit(),
                Line("PO \"H1"),
                Line("POTS"),
                Line("PRINTOUT \"H1"),
                Line("PRINTTITLES"),
                Line("APPLY \"H1 [99]"),
                Line("ERASE \"H1"),
                Line("PRINT \"WORK_DONE")
            ],
            "WORK_DONE",
            ["PO", "PRINTOUT", "POTS", "PRINTTITLES", "APPLY", "ERASE"],
            screen =>
            {
                AssertScreenContains(screen, "TO H1");
                AssertScreenContains(screen, "99");
            });

        yield return new LogoHarnessCase(
            "turtle movement, aliases, and reporters",
            [
                Line("DRAW"),
                Line("PENUP"),
                Line("FORWARD 10"),
                Line("PENDOWN"),
                Line("RIGHT 90"),
                Line("FORWARD 10"),
                Line("LEFT 45"),
                Line("BACK 5"),
                Line("BACKWARD 5"),
                Line("CLEARSCREEN"),
                Line("PU"),
                Line("FD 20"),
                Line("PD"),
                Line("RT 90"),
                Line("FD 20"),
                Line("LT 90"),
                Line("BK 10"),
                Line("HOME"),
                Line("SETXY 100 50"),
                Line("SETPOS [120 60]"),
                Line("SETX 130"),
                Line("SETY 70"),
                Line("SETHEADING 270"),
                Line("SETH 180"),
                Line("PRINT XCOR"),
                Line("PRINT YCOR"),
                Line("PRINT HEADING"),
                Line("PRINT PENDOWN?"),
                Line("PRINT SHOWN?"),
                Line("PRINT TOWARDS 160 80"),
                Line("PRINT \"TURTLE_DONE")
            ],
            "TURTLE_DONE",
            [
                "DRAW", "PENUP", "FORWARD", "PENDOWN", "RIGHT", "LEFT", "BACK", "BACKWARD", "CLEARSCREEN",
                "PU", "FD", "PD", "RT", "LT", "BK", "HOME", "SETXY", "SETPOS", "SETX", "SETY",
                "SETHEADING", "SETH", "XCOR", "YCOR", "HEADING", "PENDOWN?", "SHOWN?", "TOWARDS"
            ],
            (screen, bus) =>
            {
                AssertScreenContains(screen, "130");
                AssertScreenContains(screen, "70");
                AssertScreenContains(screen, "180");
                // Internal state is screen-pixel: SETX 130 -> 130+160=290; SETY 70 ->
                // 100-70=30. The public XCOR/YCOR (130/70) are checked on-screen above.
                Assert.AreEqual(290, ReadTurtleWord(bus, TurtleXLo, TurtleXHi),
                    "SETX 130 should leave the turtle at screen X 130+160.");
                Assert.AreEqual(30, ReadTurtleWord(bus, TurtleYLo, TurtleYHi),
                    "SETY 70 should leave the turtle at screen Y 100-70.");
                Assert.AreEqual(180, ReadTurtleWord(bus, TurtleHeadingLo, TurtleHeadingHi),
                    "SETH should use the same heading state as SETHEADING.");
            });

        yield return new LogoHarnessCase(
            "screen mode, visibility, and color aliases",
            [
                Line("CS"),
                Line("HT"),
                Line("ST"),
                Line("HIDETURTLE"),
                Line("SHOWTURTLE"),
                Line("SETPC 1"),
                Line("SETPENCOLOR 2"),
                Line("SETBG 0"),
                Line("SETBACKGROUND 0"),
                Line("TEXTSCREEN"),
                Line("TS"),
                Line("SPLITSCREEN"),
                Line("SS"),
                Line("FULLSCREEN"),
                Line("FS"),
                Line("SS"),
                Line("PRINT \"MODE_DONE")
            ],
            "MODE_DONE",
            [
                "CS", "HT", "ST", "HIDETURTLE", "SHOWTURTLE", "SETPC", "SETPENCOLOR", "SETBG",
                "SETBACKGROUND", "TEXTSCREEN", "TS", "SPLITSCREEN", "SS", "FULLSCREEN", "FS"
            ],
            (_, bus) =>
            {
                int cursorY = bus.Vgc.GetCursorY();
                Assert.IsTrue(cursorY is >= 40 and < VgcConstants.ScreenRows,
                    $"Returning to split-screen mode should leave the prompt in the visible text band. CursorY={cursorY}");
            });

        yield return new LogoHarnessCase(
            "graphics primitives",
            [
                Line("CS"),
                Line("SETCOLOR 1"),
                Line("PLOT 10 10"),
                Line("UNPLOT 10 10"),
                Line("LINE 20 20 50 20"),
                Line("CIRCLE 80 40 10"),
                Line("RECT 100 20 120 40"),
                Line("RECTANGLE 125 20 145 40"),
                Line("FILL 150 20 160 30"),
                Line("FILLRECT 165 20 175 30"),
                Line("SETCOLOR 2"),
                Line("RECT 190 30 210 50"),
                Line("SETCOLOR 3"),
                Line("PAINT 200 40"),
                Line("PRINT \"GFX_DONE")
            ],
            "GFX_DONE",
            ["SETCOLOR", "PLOT", "UNPLOT", "LINE", "CIRCLE", "RECT", "RECTANGLE", "FILL", "FILLRECT", "PAINT"],
            (_, bus) =>
            {
                Assert.AreEqual(0, bus.Vgc.GetGfxPixelColor(10, 10),
                    "UNPLOT should clear the pixel that PLOT set.");
                Assert.IsTrue(CountNonzeroPixels(bus, 20, 20, 50, 20) >= 20,
                    "LINE should leave visible pixels along the expected span.");
                Assert.IsTrue(CountNonzeroPixels(bus, 150, 20, 160, 30) > 50,
                    "FILL should draw a filled rectangle through the VGC path.");
                Assert.IsTrue(CountNonzeroPixels(bus, 165, 20, 175, 30) > 50,
                    "FILLRECT should draw through the same filled-rectangle command path.");
            });

        yield return new LogoHarnessCase(
            "sprites, sound commands, and timers",
            [
                Line("CS"),
                Line("SPRITE 0 50 50"),
                Line("SPRITEPOS 0 60 60"),
                Line("SPRITEON 0"),
                Line("PRINT SPRITECOLLISION? 0"),
                Line("SPRITEOFF 0"),
                Line("VOLUME 5"),
                Line("TONE 440 1"),
                Line("NOISE 1"),
                Line("WAIT 1"),
                Line("WAITVBL"),
                Line("PRINT TIMER"),
                Line("PRINT \"DEVICE_DONE")
            ],
            "DEVICE_DONE",
            ["SPRITE", "SPRITEPOS", "SPRITEON", "SPRITEOFF", "SPRITECOLLISION?", "VOLUME", "TONE", "NOISE", "WAIT", "WAITVBL", "TIMER"],
            (_, bus) =>
            {
                var sprite = bus.Vgc.GetSpriteState(0);
                Assert.AreEqual(60, sprite.x, "SPRITEPOS should update sprite X through the extension command.");
                Assert.AreEqual(60, sprite.y, "SPRITEPOS should update sprite Y through the extension command.");
                Assert.IsFalse(sprite.enabled, "SPRITEOFF should disable the sprite after SPRITEON.");
            });
    }

    private static void RunHarnessCase(LogoHarnessCase testCase)
    {
        using var bus = new CompositeBusDevice(enableSound: false, bootRom: CompositeBusDevice.ActiveRom.Logo);
        var cpu = new Cpu(bus);
        cpu.Boot();
        bus.StageShelfModule(0,
            File.ReadAllBytes(Path.Combine(FindRepoRoot(), "modules", "graphics", "graphics.bin")),
            0x01);   // MODULE_ID_GRAPHICS — seed the shelf directory so lib_call scan hits
        var editor = new ScreenEditor(bus.Vgc);
        bus.Vgc.SetScreenEditor(editor);
        RunUntilScreenContains(cpu, bus, "?", 10_000_000, testCase.Name);

        foreach (LogoInput input in testCase.Inputs)
            QueueInput(editor, input);

        RunUntilScreenContains(cpu, bus, testCase.Marker, testCase.MaxSteps, testCase.Name);
        RunSteps(cpu, bus, testCase.SettleSteps);

        string screen = SnapshotScreen(bus.Vgc);
        Assert.IsFalse(screen.Contains("I DON'T KNOW HOW TO", StringComparison.Ordinal),
            $"{testCase.Name}: no covered command should fall through to the unknown-word path.\n{screen}");
        Assert.IsFalse(screen.Contains("NOT ENOUGH INPUTS", StringComparison.Ordinal),
            $"{testCase.Name}: harness commands should supply the required arguments.\n{screen}");
        Assert.IsFalse(screen.Contains("OUT OF MEMORY", StringComparison.OrdinalIgnoreCase),
            $"{testCase.Name}: harness programs should not exhaust Logo heap.\n{screen}");

        testCase.Verify(screen, bus);
    }

    private static IReadOnlySet<string> LoadRegisteredLogoCommands()
    {
        string root = FindRepoRoot();
        string logoDir = Path.Combine(root, "novalogo");
        var labels = LoadLogoLabelStrings(logoDir);
        var commands = new HashSet<string>(StringComparer.Ordinal);

        foreach (string name in ReadCommandTableNames(Path.Combine(logoDir, "builtins.s"), "builtin_table", labels))
            commands.Add(name);
        foreach (string name in ReadCommandTableNames(Path.Combine(logoDir, "eval.s"), "ext_cmd_table", labels))
            commands.Add(name);
        commands.Add("TO");

        return commands;
    }

    private static Dictionary<string, string> LoadLogoLabelStrings(string logoDir)
    {
        var labels = new Dictionary<string, string>(StringComparer.Ordinal);
        var labelRegex = new Regex(
            @"(?m)^([A-Za-z0-9_]+):\s*\r?\n\s*\.byte\s+\d+,\s*""([^""]+)""",
            RegexOptions.CultureInvariant);

        foreach (string file in Directory.EnumerateFiles(logoDir, "*.s"))
        {
            string source = File.ReadAllText(file);
            foreach (Match match in labelRegex.Matches(source))
                labels[match.Groups[1].Value] = match.Groups[2].Value;
        }

        return labels;
    }

    private static IEnumerable<string> ReadCommandTableNames(
        string tablePath,
        string tableName,
        IReadOnlyDictionary<string, string> labels)
    {
        string source = File.ReadAllText(tablePath);
        string startMarker = tableName + ":";
        int start = source.IndexOf(startMarker, StringComparison.Ordinal);
        Assert.IsTrue(start >= 0, $"Could not find {tableName} in {tablePath}.");

        int end = source.IndexOf(".word $0000", start, StringComparison.Ordinal);
        Assert.IsTrue(end > start, $"Could not find {tableName} sentinel in {tablePath}.");

        string table = source[start..end];
        foreach (Match match in Regex.Matches(table, @"\.word\s+([A-Za-z0-9_]+)", RegexOptions.CultureInvariant))
        {
            string label = match.Groups[1].Value;
            if (labels.TryGetValue(label, out string? name))
            {
                yield return name;
                continue;
            }

            Assert.IsFalse(label.StartsWith("str_", StringComparison.Ordinal),
                $"Command table {tableName} references {label}, but no length-prefixed name string was found.");
        }
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

    private static LogoInput Line(string text) => new(text + "\r");

    private static LogoInput Raw(string text) => new(text);

    private static LogoInput SaveAndQuit() => Raw("\x0B" + "s" + "\x1B" + "x");

    private static void QueueInput(ScreenEditor editor, LogoInput input)
    {
        foreach (char ch in input.Text)
            editor.QueueInput((byte)ch);
    }

    private static void RunUntilScreenContains(Cpu cpu, CompositeBusDevice bus, string marker, int maxSteps, string testName)
    {
        for (int i = 0; i < maxSteps; i++)
        {
            int cycles = cpu.ClocksForNext();
            cpu.ExecuteNext();
            bus.AdvanceCycles(cycles);
            if ((i & 0x3FF) == 0 && SnapshotScreen(bus.Vgc).Contains(marker, StringComparison.Ordinal))
                return;
        }

        Assert.Fail($"{testName}: timed out waiting for screen marker '{marker}'.\n{SnapshotScreen(bus.Vgc)}");
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

    private static int ReadTurtleWord(CompositeBusDevice bus, ushort loAddress, ushort hiAddress) =>
        bus.ReadRam(loAddress) | (bus.ReadRam(hiAddress) << 8);

    private static int CountNonzeroPixels(CompositeBusDevice bus, int left, int top, int right, int bottom)
    {
        int count = 0;
        for (int y = top; y <= bottom; y++)
        {
            for (int x = left; x <= right; x++)
            {
                if (bus.Vgc.GetGfxPixelColor(x, y) != 0)
                    count++;
            }
        }

        return count;
    }

    private sealed class LogoHarnessCase
    {
        public LogoHarnessCase(
            string name,
            IReadOnlyList<LogoInput> inputs,
            string marker,
            IReadOnlyCollection<string> covers,
            Action<string> verify)
            : this(name, inputs, marker, covers, (screen, _) => verify(screen))
        {
        }

        public LogoHarnessCase(
            string name,
            IReadOnlyList<LogoInput> inputs,
            string marker,
            IReadOnlyCollection<string> covers,
            Action<string, CompositeBusDevice> verify)
        {
            Name = name;
            Inputs = inputs;
            Marker = marker;
            Covers = covers;
            Verify = verify;
        }

        public string Name { get; }
        public IReadOnlyList<LogoInput> Inputs { get; }
        public string Marker { get; }
        public IReadOnlyCollection<string> Covers { get; }
        public Action<string, CompositeBusDevice> Verify { get; }
        public int MaxSteps { get; init; } = 80_000_000;
        public int SettleSteps { get; init; } = 1_000_000;
    }

    private readonly record struct LogoInput(string Text);
}

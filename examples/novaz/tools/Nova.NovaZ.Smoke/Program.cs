using System.Globalization;
using System.Text;
using System.Text.Json.Nodes;
using e6502.Avalonia.Hardware;
using e6502.Avalonia.Input;
using e6502.Storage;
using KDS.e6502;

if (args.Length < 1)
{
    Console.Error.WriteLine("usage: Nova.NovaZ.Smoke <fd0.ndi> [command[=>expected] ...]");
    Console.Error.WriteLine("       Nova.NovaZ.Smoke <fd0.ndi> --script <file>");
    Console.Error.WriteLine("       Nova.NovaZ.Smoke <fd0.ndi> --generic-boot [--boot-only] [--screen-only] [--screen-input <text>] [--expect-more] [--expect-time-status] [--expect-screen <text>] [--expect-at <col>,<row>=><text>] [--expect-text-color <text=>hex>] [--expect-gfx-color <x,y=>hex>] [--expect-stop <byte>] [--skip-manifest-check]");
    return 1;
}

string imagePath = Path.GetFullPath(args[0]);
if (!File.Exists(imagePath))
{
    Console.Error.WriteLine($"image not found: {imagePath}");
    return 1;
}

bool genericBoot = args.Skip(1).Contains("--generic-boot", StringComparer.Ordinal);
bool bootOnly = args.Skip(1).Contains("--boot-only", StringComparer.Ordinal);
bool screenOnly = args.Skip(1).Contains("--screen-only", StringComparer.Ordinal);
bool expectMore = args.Skip(1).Contains("--expect-more", StringComparer.Ordinal);
bool expectTimeStatus = args.Skip(1).Contains("--expect-time-status", StringComparer.Ordinal);
bool skipManifestCheck = args.Skip(1).Contains("--skip-manifest-check", StringComparer.Ordinal);
bool noStatusLine = args.Skip(1).Contains("--no-status-line", StringComparer.Ordinal);
bool expectSoundfont = args.Skip(1).Contains("--expect-soundfont", StringComparer.Ordinal);
List<string> expectedScreens = LoadExpectedScreens(args);
int? expectStop = LoadExpectStop(args);
List<ExpectedAt> expectedAts = LoadExpectedAts(args);
List<string> screenInputs = LoadScreenInputs(args);
List<ExpectedTextColor> expectedTextColors = LoadExpectedTextColors(args);
List<ExpectedGfxColor> expectedGfxColors = LoadExpectedGfxColors(args);
List<SmokeCommand> commands = LoadCommands(args, bootOnly, screenOnly);
JsonNode? manifest = TryReadManifest(imagePath);
int? rawInputModeAddress = TryReadRuntimeSymbol("nz_raw_input_mode");
int? readKeyLoopAddress = TryReadRuntimeSymbol("zvm_read_key_loop");
int? readTimedLoopAddress = TryReadRuntimeSymbol("zvm_read_timed_poll");
string storageRoot = Path.Combine(Path.GetTempPath(), $"nova-novaz-smoke-{Guid.NewGuid():N}");
CompositeBusDevice? bus = null;
Cpu? cpu = null;
ScreenEditor? editor = null;

try
{
    string disks = Path.Combine(storageRoot, "disks");
    Directory.CreateDirectory(disks);
    File.Copy(imagePath, Path.Combine(disks, "fd0.ndi"));

    Environment.SetEnvironmentVariable("NOVA_STORAGE_ROOT", storageRoot);
    Environment.SetEnvironmentVariable("NOVA_NO_AUTOMOUNT", null);
    Environment.SetEnvironmentVariable("NOAUTO", null);

    (bus, cpu, editor) = StartMachine();

    int maxSteps = ReadEnvInt("NOVAZ_SMOKE_MAX_STEPS", 80_000_000);
    int morePrompts = 0;

    // --expect-stop: run until the runtime parks in its post-game halt loop,
    // then pin zvm_stop_reason. This is the only mode that works for stories
    // which halt without ever reaching a read prompt (e.g. a V6 main routine
    // that returns): the prompt path would time out and the screen-only path
    // cannot tell a clean ZVM_STOP_QUIT from a bad-target wedge. The CPU PC
    // is the readiness signal — NOT zvm_stop_reason itself, which holds boot
    // residue (the disk loader stages through BSS) until zvm_run_until_read
    // clears it.
    if (expectStop is { } expectedStopReason)
    {
        int stopAddress = TryReadRuntimeSymbol("zvm_stop_reason")
            ?? throw new InvalidOperationException("--expect-stop requires zvm_stop_reason in build/runtime.sym.");
        int haltAddress = TryReadRuntimeSymbol("halt")
            ?? throw new InvalidOperationException("--expect-stop requires halt in build/runtime.sym.");
        RunUntilCpuHalt(cpu, bus, maxSteps, haltAddress);
        byte actualStop = bus.Read((ushort)stopAddress);
        string stopScreen = SnapshotScreen(bus.Vgc);
        if (actualStop != expectedStopReason)
            throw new InvalidOperationException($"Expected zvm_stop_reason={expectedStopReason}, saw {actualStop}.\n{stopScreen}");
        foreach (string expected in expectedScreens)
            RequireContains(stopScreen, expected);
        Console.WriteLine($"NovaZ stop smoke passed. stop={actualStop}");
        Console.WriteLine(stopScreen);
        return 0;
    }

    if (screenOnly)
    {
        if (expectedScreens.Count == 0 && expectedAts.Count == 0 && expectedTextColors.Count == 0 && expectedGfxColors.Count == 0)
            throw new InvalidOperationException("--screen-only requires at least one --expect-screen, --expect-at, --expect-text-color, or --expect-gfx-color check.");

        foreach (string input in screenInputs)
            SendRaw(cpu, bus, editor, input);

        string screenOnlySnapshot = RunUntilScreenMatches(cpu, bus, editor, maxSteps, expectedScreens, expectedAts, expectedTextColors, expectedGfxColors, ref morePrompts);
        foreach (string expected in expectedScreens)
            RequireContains(screenOnlySnapshot, expected);
        foreach (var expected in expectedAts)
            RequireAt(screenOnlySnapshot, expected);
        foreach (var expected in expectedTextColors)
            RequireTextColor(bus.Vgc, screenOnlySnapshot, expected);
        foreach (var expected in expectedGfxColors)
            RequireGfxColor(bus.Vgc, screenOnlySnapshot, expected);
        if (expectMore && morePrompts == 0)
            throw new InvalidOperationException($"Expected at least one [ MORE ] prompt.\n{screenOnlySnapshot}");
        if (expectSoundfont)
            RequireSoundfontLoaded(bus, screenOnlySnapshot);

        Console.WriteLine($"NovaZ screen smoke passed. morePrompts={morePrompts}");
        Console.WriteLine(screenOnlySnapshot);
        return 0;
    }

    string screen = RunUntilReadyPrompt(cpu, bus, editor, maxSteps, ref morePrompts, rawInputModeAddress, readKeyLoopAddress, readTimedLoopAddress);
    RunForSteps(cpu, bus, 200_000);
    screen = SnapshotScreen(bus.Vgc);

    if (screen.Contains("UNSUPPORTED Z-OPCODE", StringComparison.Ordinal))
        throw new InvalidOperationException($"NovaZ hit an unsupported opcode.\n{screen}");

    if (!genericBoot)
    {
        RequireContains(screen, "ZORK I: The Great Underground Empire");
        RequireContains(screen, "West of House");
        RequireContains(screen, "boarded front door");
        RequireContains(screen, "There is a small mailbox here.");
    }
    RequireContains(screen, ">");
    if (!noStatusLine)
        RequireStatusLine(screen, cpu, bus);
    if (expectTimeStatus)
        RequireTimeStatusLine(screen);
    foreach (string expected in expectedScreens)
        RequireContains(screen, expected);
    foreach (var expected in expectedAts)
        RequireAt(screen, expected);
    RequireReadyPrompt(cpu, bus, screen, rawInputModeAddress, readKeyLoopAddress, readTimedLoopAddress);
    if (expectSoundfont)
        RequireSoundfontLoaded(bus, screen);

    string bootScreen = screen;

    foreach (var command in commands)
    {
        if (command.Text.Equals(".reboot", StringComparison.OrdinalIgnoreCase))
        {
            Console.WriteLine("> .reboot");
            cpu.Boot();
            RunForSteps(cpu, bus, 500_000);
            screen = RunUntilReadyPrompt(cpu, bus, editor, maxSteps, ref morePrompts, rawInputModeAddress, readKeyLoopAddress, readTimedLoopAddress);
            RunForSteps(cpu, bus, 200_000);
            screen = SnapshotScreen(bus.Vgc);
            if (screen.Contains("UNSUPPORTED Z-OPCODE", StringComparison.Ordinal))
                throw new InvalidOperationException($"NovaZ hit an unsupported opcode after reboot.\n{screen}");
            RequireContains(screen, ">");
            if (!noStatusLine)
                RequireStatusLine(screen, cpu, bus);
            if (expectTimeStatus)
                RequireTimeStatusLine(screen);
            RequireReadyPrompt(cpu, bus, screen, rawInputModeAddress, readKeyLoopAddress, readTimedLoopAddress);
            continue;
        }
        if (command.Mode == SmokeInputMode.Wait)
        {
            if (!double.TryParse(command.Text, NumberStyles.Float, CultureInfo.InvariantCulture, out double seconds) ||
                seconds < 0)
                throw new InvalidOperationException($".wait requires non-negative seconds, got '{command.Text}'.");
            Console.WriteLine($".wait {command.Text}");
            int steps = Math.Max(1, (int)Math.Round(seconds * 1_000_000));
            RunForSteps(cpu, bus, steps);
            continue;
        }
        if (command.Mode == SmokeInputMode.ExpectStop)
        {
            if (!byte.TryParse(command.Text, NumberStyles.Integer, CultureInfo.InvariantCulture, out byte expectedStop))
                throw new InvalidOperationException($".expect-stop requires a byte value, got '{command.Text}'.");
            int stopAddress = TryReadRuntimeSymbol("zvm_stop_reason") ?? 0;
            byte actualStop = stopAddress > 0 ? bus.Read((ushort)stopAddress) : (byte)0;
            Console.WriteLine($".expect-stop {expectedStop}");
            if (actualStop != expectedStop)
                throw new InvalidOperationException($"Expected zvm_stop_reason={expectedStop}, saw {actualStop}.\n{SnapshotScreen(bus.Vgc)}");
            continue;
        }

        string marker = command.Mode == SmokeInputMode.Raw ? ".raw " : "> ";
        Console.WriteLine($"{marker}{command.Text}");
        if (command.Mode == SmokeInputMode.Raw)
            SendRaw(cpu, bus, editor, command.Text);
        else
            SendLine(cpu, bus, editor, command.Text);

        if (command.WaitForPrompt)
        {
            screen = RunUntilReadyPrompt(cpu, bus, editor, maxSteps, ref morePrompts, rawInputModeAddress, readKeyLoopAddress, readTimedLoopAddress);
        }
        else
        {
            if (command.Expected.Count == 0)
                throw new InvalidOperationException($"No-prompt command '{command.Text}' requires expected screen text.");
            screen = RunUntilScreenMatches(cpu, bus, editor, maxSteps, command.Expected, [], [], [], ref morePrompts);
        }
        RunForSteps(cpu, bus, 200_000);
        screen = SnapshotScreen(bus.Vgc);
        if (screen.Contains("UNSUPPORTED Z-OPCODE", StringComparison.Ordinal))
            throw new InvalidOperationException($"NovaZ hit an unsupported opcode after input '{command.Text}'.\n{screen}");
        if (command.WaitForPrompt)
        {
            RequireContains(screen, ">");
            if (!noStatusLine)
                RequireStatusLine(screen, cpu, bus);
            if (expectTimeStatus)
                RequireTimeStatusLine(screen);
            RequireReadyPrompt(cpu, bus, screen, rawInputModeAddress, readKeyLoopAddress, readTimedLoopAddress);
        }
        if (Environment.GetEnvironmentVariable("NOVAZ_SMOKE_DUMP_PARSE") == "1")
            Console.Error.WriteLine($"{FormatZvmState(cpu, bus)} {FormatParserState(bus)}");
        string commandTranscript = command.WaitForPrompt && command.Mode == SmokeInputMode.Line
            ? ExtractCommandTranscript(screen, command.Text)
            : screen;
        foreach (string expected in command.Expected)
            RequireContains(commandTranscript, expected, command.Text);
    }

    if (manifest is not null && !skipManifestCheck)
    {
        int? version = manifest["Version"]?.GetValue<int>();
        int? release = manifest["Release"]?.GetValue<int>();
        string? serial = manifest["Serial"]?.GetValue<string>();

        if (!genericBoot && version is not null)
            RequireContains(bootScreen, "ZORK I:");
        if (release is not null)
            RequireContainsAny(bootScreen, $"Revision {release}", $"Release {release}", $"Version {release}");
        if (!string.IsNullOrWhiteSpace(serial))
            RequireContains(bootScreen, $"Serial number {serial}");
    }

    if (expectMore && morePrompts == 0)
        throw new InvalidOperationException($"Expected at least one [ MORE ] prompt.\n{SnapshotScreen(bus.Vgc)}");

    Console.WriteLine($"NovaZ smoke passed. morePrompts={morePrompts}");
    Console.WriteLine(SnapshotScreen(bus.Vgc));
    return 0;
}
finally
{
    bus?.Dispose();

    Environment.SetEnvironmentVariable("NOAUTO", null);
    Environment.SetEnvironmentVariable("NOVA_NO_AUTOMOUNT", null);
    Environment.SetEnvironmentVariable("NOVA_STORAGE_ROOT", null);

    if (Directory.Exists(storageRoot))
        Directory.Delete(storageRoot, recursive: true);
}

static (CompositeBusDevice Bus, Cpu Cpu, ScreenEditor Editor) StartMachine()
{
    var bus = new CompositeBusDevice(enableSound: false);
    var cpu = new Cpu(bus);
    var editor = new ScreenEditor(bus.Vgc);
    bus.Vgc.SetScreenEditor(editor);
    cpu.Boot();
    return (bus, cpu, editor);
}

static JsonNode? TryReadManifest(string imagePath)
{
    try
    {
        using var image = NdiImage.Open(imagePath);
        byte[] data = image.ReadFile("STORY.MANIFEST", 0xFFFF);
        return JsonNode.Parse(Encoding.ASCII.GetString(data));
    }
    catch
    {
        return null;
    }
}

static int? TryReadRuntimeSymbol(string symbolName)
{
    string[] candidates =
    [
        Path.Combine(Environment.CurrentDirectory, "build", "runtime.sym"),
        Path.Combine(Environment.CurrentDirectory, "examples", "novaz", "build", "runtime.sym")
    ];

    foreach (string path in candidates)
    {
        if (!File.Exists(path))
            continue;

        foreach (string line in File.ReadLines(path))
        {
            string[] parts = line.Split(' ', StringSplitOptions.RemoveEmptyEntries);
            if (parts.Length < 3 || !parts[2].TrimStart('.').Equals(symbolName, StringComparison.Ordinal))
                continue;
            if (int.TryParse(parts[1], NumberStyles.HexNumber, CultureInfo.InvariantCulture, out int address))
                return address;
        }
    }

    return null;
}

static int ReadEnvInt(string name, int fallback)
{
    string? value = Environment.GetEnvironmentVariable(name);
    return int.TryParse(value, out int parsed) && parsed > 0 ? parsed : fallback;
}

static int ReadEnvHex(string name, int fallback)
{
    string? value = Environment.GetEnvironmentVariable(name);
    if (string.IsNullOrWhiteSpace(value))
        return fallback;
    value = value.Trim();
    if (value.StartsWith("0x", StringComparison.OrdinalIgnoreCase))
        value = value[2..];
    return int.TryParse(value, NumberStyles.HexNumber, CultureInfo.InvariantCulture, out int parsed)
        ? parsed
        : fallback;
}

static List<SmokeCommand> LoadCommands(string[] args, bool bootOnly, bool screenOnly)
{
    if (bootOnly || screenOnly)
        return [];

    var commands = new List<SmokeCommand>();
    for (int i = 1; i < args.Length; i++)
    {
        switch (args[i])
        {
            case "--generic-boot":
            case "--boot-only":
            case "--screen-only":
            case "--expect-more":
            case "--expect-time-status":
            case "--skip-manifest-check":
            case "--no-status-line":
            case "--expect-soundfont":
                break;
            case "--expect-screen":
                if (i + 1 >= args.Length)
                    throw new ArgumentException("--expect-screen requires text.");
                i++;
                break;
            case "--expect-at":
                if (i + 1 >= args.Length)
                    throw new ArgumentException("--expect-at requires <col>,<row>=><text>.");
                i++;
                break;
            case "--screen-input":
                if (i + 1 >= args.Length)
                    throw new ArgumentException("--screen-input requires text.");
                i++;
                break;
            case "--expect-text-color":
                if (i + 1 >= args.Length)
                    throw new ArgumentException("--expect-text-color requires text=>hex.");
                i++;
                break;
            case "--expect-stop":
                if (i + 1 >= args.Length)
                    throw new ArgumentException("--expect-stop requires a byte value.");
                i++;
                break;
            case "--script":
                if (i + 1 >= args.Length)
                    throw new ArgumentException("--script requires a file path.");
                string path = Path.GetFullPath(args[++i]);
                commands.AddRange(
                    File.ReadLines(path)
                        .Select(line => line.Trim())
                        .Where(line => line.Length > 0 && !line.StartsWith('#'))
                        .Select(ParseCommandSpec));
                break;
            default:
                commands.Add(ParseCommandSpec(args[i]));
                break;
        }
    }

    if (commands.Count == 0)
    {
        return
        [
            new SmokeCommand("look", ["There is a small mailbox here."])
        ];
    }

    return commands;
}

static SmokeCommand ParseCommandSpec(string spec)
{
    string[] parts = spec.Split("=>", 2, StringSplitOptions.TrimEntries);
    string command = parts[0];
    if (string.IsNullOrWhiteSpace(command))
        throw new ArgumentException($"Empty NovaZ smoke command in '{spec}'.");

    bool waitForPrompt = true;
    if (command.StartsWith('!'))
    {
        waitForPrompt = false;
        command = command[1..].TrimStart();
    }

    SmokeInputMode mode = SmokeInputMode.Line;
    const string rawPrefix = ".raw ";
    if (command.StartsWith(rawPrefix, StringComparison.OrdinalIgnoreCase))
    {
        mode = SmokeInputMode.Raw;
        command = command[rawPrefix.Length..];
        if (command.Length == 0)
            throw new ArgumentException($"Empty raw NovaZ smoke input in '{spec}'.");
    }
    const string waitPrefix = ".wait ";
    if (command.StartsWith(waitPrefix, StringComparison.OrdinalIgnoreCase))
    {
        mode = SmokeInputMode.Wait;
        waitForPrompt = false;
        command = command[waitPrefix.Length..].Trim();
        if (command.Length == 0)
            throw new ArgumentException($".wait requires seconds in '{spec}'.");
    }
    const string expectStopPrefix = ".expect-stop ";
    if (command.StartsWith(expectStopPrefix, StringComparison.OrdinalIgnoreCase))
    {
        mode = SmokeInputMode.ExpectStop;
        waitForPrompt = false;
        command = command[expectStopPrefix.Length..].Trim();
        if (command.Length == 0)
            throw new ArgumentException($".expect-stop requires a byte value in '{spec}'.");
    }

    string[] expected = parts.Length == 2 && !string.IsNullOrWhiteSpace(parts[1])
        ? parts[1].Split("&&", StringSplitOptions.TrimEntries | StringSplitOptions.RemoveEmptyEntries)
        : [];
    return new SmokeCommand(command, expected, mode, waitForPrompt);
}

static int? LoadExpectStop(string[] args)
{
    for (int i = 1; i < args.Length; i++)
    {
        if (!args[i].Equals("--expect-stop", StringComparison.Ordinal))
            continue;
        if (i + 1 >= args.Length || !byte.TryParse(args[i + 1], NumberStyles.Integer, CultureInfo.InvariantCulture, out byte value))
            throw new ArgumentException("--expect-stop requires a byte value.");
        return value;
    }

    return null;
}

static List<string> LoadExpectedScreens(string[] args)
{
    var expected = new List<string>();
    for (int i = 1; i < args.Length; i++)
    {
        if (!args[i].Equals("--expect-screen", StringComparison.Ordinal))
            continue;
        if (i + 1 >= args.Length)
            throw new ArgumentException("--expect-screen requires text.");
        expected.Add(args[++i]);
    }

    return expected;
}

// --expect-at <col>,<row>=><text>: the screen snapshot at 0-based cell
// (col,row) must START with <text>, whitespace-exact — no normalization.
static List<ExpectedAt> LoadExpectedAts(string[] args)
{
    var expected = new List<ExpectedAt>();
    for (int i = 1; i < args.Length; i++)
    {
        if (!args[i].Equals("--expect-at", StringComparison.Ordinal))
            continue;
        if (i + 1 >= args.Length)
            throw new ArgumentException("--expect-at requires <col>,<row>=><text>.");

        string spec = args[++i];
        string[] parts = spec.Split("=>", 2, StringSplitOptions.None);
        if (parts.Length != 2 || parts[1].Length == 0)
            throw new ArgumentException($"Expected position check must be '<col>,<row>=><text>': {spec}");

        string[] xy = parts[0].Split(',', 2, StringSplitOptions.TrimEntries);
        if (xy.Length != 2 ||
            !int.TryParse(xy[0], NumberStyles.Integer, CultureInfo.InvariantCulture, out int col) ||
            !int.TryParse(xy[1], NumberStyles.Integer, CultureInfo.InvariantCulture, out int row))
            throw new ArgumentException($"Expected position must be '<col>,<row>': {spec}");
        if ((uint)col >= VgcConstants.ScreenCols || (uint)row >= VgcConstants.ScreenRows)
            throw new ArgumentOutOfRangeException(nameof(args), $"--expect-at cell out of range in '{spec}'.");
        if (col + parts[1].Length > VgcConstants.ScreenCols)
            throw new ArgumentOutOfRangeException(nameof(args), $"--expect-at text runs past column {VgcConstants.ScreenCols} in '{spec}'.");

        expected.Add(new ExpectedAt(col, row, parts[1]));
    }

    return expected;
}

static bool MatchesAt(string screen, ExpectedAt expected)
{
    string[] lines = screen.Split('\n');
    if (expected.Row >= lines.Length || expected.Col + expected.Text.Length > lines[expected.Row].Length)
        return false;
    return lines[expected.Row].Substring(expected.Col, expected.Text.Length)
        .Equals(expected.Text, StringComparison.Ordinal);
}

static void RequireAt(string screen, ExpectedAt expected)
{
    if (MatchesAt(screen, expected))
        return;

    string[] lines = screen.Split('\n');
    string actual = expected.Row < lines.Length
        ? lines[expected.Row].Substring(
            Math.Min(expected.Col, lines[expected.Row].Length),
            Math.Min(expected.Text.Length, Math.Max(0, lines[expected.Row].Length - expected.Col)))
        : "";
    throw new InvalidOperationException(
        $"Expected screen cell {expected.Col},{expected.Row} to start with '{expected.Text}'; saw '{actual}'.\n{screen}");
}

static List<string> LoadScreenInputs(string[] args)
{
    var inputs = new List<string>();
    for (int i = 1; i < args.Length; i++)
    {
        if (!args[i].Equals("--screen-input", StringComparison.Ordinal))
            continue;
        if (i + 1 >= args.Length)
            throw new ArgumentException("--screen-input requires text.");
        inputs.Add(DecodeScreenInput(args[++i]));
    }

    return inputs;
}

static string DecodeScreenInput(string input)
{
    var decoded = new StringBuilder(input.Length);
    for (int i = 0; i < input.Length; i++)
    {
        char ch = input[i];
        if (ch != '\\')
        {
            decoded.Append(ch);
            continue;
        }

        if (++i >= input.Length)
            throw new ArgumentException("--screen-input cannot end with a trailing escape.");

        char escape = input[i];
        switch (escape)
        {
            case '0':
                decoded.Append('\0');
                break;
            case 'n':
                decoded.Append('\n');
                break;
            case 'r':
                decoded.Append('\r');
                break;
            case 't':
                decoded.Append('\t');
                break;
            case '\\':
                decoded.Append('\\');
                break;
            case 'x':
                if (i + 2 >= input.Length ||
                    !byte.TryParse(input.Substring(i + 1, 2), NumberStyles.HexNumber, CultureInfo.InvariantCulture, out byte value))
                    throw new ArgumentException("--screen-input has an invalid \\xNN escape.");
                decoded.Append((char)value);
                i += 2;
                break;
            default:
                throw new ArgumentException($"--screen-input has an unknown escape '\\{escape}'.");
        }
    }

    return decoded.ToString();
}

static List<ExpectedTextColor> LoadExpectedTextColors(string[] args)
{
    var expected = new List<ExpectedTextColor>();
    for (int i = 1; i < args.Length; i++)
    {
        if (!args[i].Equals("--expect-text-color", StringComparison.Ordinal))
            continue;
        if (i + 1 >= args.Length)
            throw new ArgumentException("--expect-text-color requires text=>hex.");

        string spec = args[++i];
        string[] parts = spec.Split("=>", 2, StringSplitOptions.TrimEntries);
        if (parts.Length != 2 || string.IsNullOrWhiteSpace(parts[0]) || string.IsNullOrWhiteSpace(parts[1]))
            throw new ArgumentException($"Expected text color must be '<text=>hex>': {spec}");

        string hex = parts[1].StartsWith("0x", StringComparison.OrdinalIgnoreCase)
            ? parts[1][2..]
            : parts[1];
        if (!byte.TryParse(hex, NumberStyles.HexNumber, CultureInfo.InvariantCulture, out byte color))
            throw new ArgumentException($"Expected text color must use a one-byte hex value: {spec}");

        expected.Add(new ExpectedTextColor(parts[0], color));
    }

    return expected;
}

static List<ExpectedGfxColor> LoadExpectedGfxColors(string[] args)
{
    var expected = new List<ExpectedGfxColor>();
    for (int i = 1; i < args.Length; i++)
    {
        if (!args[i].Equals("--expect-gfx-color", StringComparison.Ordinal))
            continue;
        if (i + 1 >= args.Length)
            throw new ArgumentException("--expect-gfx-color requires x,y=>hex.");

        string spec = args[++i];
        string[] parts = spec.Split("=>", 2, StringSplitOptions.TrimEntries);
        if (parts.Length != 2 || string.IsNullOrWhiteSpace(parts[0]) || string.IsNullOrWhiteSpace(parts[1]))
            throw new ArgumentException($"Expected graphics color must be '<x,y=>hex>': {spec}");

        string[] xy = parts[0].Split(',', 2, StringSplitOptions.TrimEntries);
        if (xy.Length != 2 ||
            !int.TryParse(xy[0], NumberStyles.Integer, CultureInfo.InvariantCulture, out int x) ||
            !int.TryParse(xy[1], NumberStyles.Integer, CultureInfo.InvariantCulture, out int y))
            throw new ArgumentException($"Expected graphics color coordinate must be 'x,y': {spec}");
        if ((uint)x >= VgcConstants.GfxWidth || (uint)y >= VgcConstants.GfxHeight)
            throw new ArgumentOutOfRangeException(nameof(args), $"Graphics coordinate out of range in '{spec}'.");

        string hex = parts[1].StartsWith("0x", StringComparison.OrdinalIgnoreCase)
            ? parts[1][2..]
            : parts[1];
        if (!byte.TryParse(hex, NumberStyles.HexNumber, CultureInfo.InvariantCulture, out byte color))
            throw new ArgumentException($"Expected graphics color must use a one-byte hex value: {spec}");

        expected.Add(new ExpectedGfxColor(x, y, color));
    }

    return expected;
}

static string RunUntilScreenMatches(
    Cpu cpu,
    CompositeBusDevice bus,
    ScreenEditor editor,
    int maxSteps,
    IReadOnlyList<string> expectedScreens,
    IReadOnlyList<ExpectedAt> expectedAts,
    IReadOnlyList<ExpectedTextColor> expectedTextColors,
    IReadOnlyList<ExpectedGfxColor> expectedGfxColors,
    ref int morePrompts)
{
    const int snapshotMask = 0x3FFF;
    string? lastMatchSnapshot = null;
    for (int i = 0; i < maxSteps; i++)
    {
        YieldHostHardware(i);

        int cycles = cpu.ClocksForNext();
        cpu.ExecuteNext();
        bus.AdvanceCycles(cycles);

        if ((i & snapshotMask) != 0)
            continue;

        string screen = SnapshotScreen(bus.Vgc);
        if (screen.Contains("UNSUPPORTED Z-OPCODE", StringComparison.Ordinal))
            throw new InvalidOperationException($"NovaZ hit an unsupported opcode. {FormatZvmState(cpu, bus)}\n{screen}");
        if (screen.Contains("[ MORE ]", StringComparison.Ordinal))
        {
            morePrompts++;
            editor.QueueInput(0x0D);
            RunForSteps(cpu, bus, 8_000);
            lastMatchSnapshot = null;
            continue;
        }
        bool screenStable = screen == lastMatchSnapshot;
        lastMatchSnapshot = screen;
        if (HandleStartupPrompt(screen, cpu, bus, editor, screenStable))
            continue;

        bool hasExpectedScreens = expectedScreens.All(expected => ContainsNormalized(screen, expected));
        bool hasExpectedAts = expectedAts.All(expected => MatchesAt(screen, expected));
        bool hasExpectedColorText = expectedTextColors.All(expected => FindText(screen, expected.Text) is not null);
        bool hasExpectedGfxColors = expectedGfxColors.All(expected => bus.Vgc.GetGfxPixelColor(expected.X, expected.Y) == expected.Color);
        if (hasExpectedScreens && hasExpectedAts && hasExpectedColorText && hasExpectedGfxColors)
            return screen;
    }

    string finalScreen = SnapshotScreen(bus.Vgc);
    string gfxMismatches = FormatGfxMismatches(bus, expectedGfxColors);
    throw new TimeoutException($"Timed out waiting for expected screen content. {FormatZvmState(cpu, bus)}{gfxMismatches}\n{finalScreen}");
}

static string FormatGfxMismatches(CompositeBusDevice bus, IReadOnlyList<ExpectedGfxColor> expectedGfxColors)
{
    if (expectedGfxColors.Count == 0)
        return string.Empty;

    var mismatches = expectedGfxColors
        .Select(expected => new
        {
            Expected = expected,
            Actual = bus.Vgc.GetGfxPixelColor(expected.X, expected.Y)
        })
        .Where(sample => sample.Actual != sample.Expected.Color)
        .Select(sample => $"{sample.Expected.X},{sample.Expected.Y}: expected ${sample.Expected.Color:X2}, got ${sample.Actual:X2}");

    string summary = string.Join("; ", mismatches);
    return summary.Length == 0 ? string.Empty : $" gfx mismatches=[{summary}]";
}

static string RunUntilReadyPrompt(
    Cpu cpu,
    CompositeBusDevice bus,
    ScreenEditor editor,
    int maxSteps,
    ref int morePrompts,
    int? rawInputModeAddress,
    int? readKeyLoopAddress,
    int? readTimedLoopAddress)
{
    const int snapshotMask = 0x3FFF;
    var trace = new Queue<string>();
    int lastOpcodePc = -1;
    bool traceTopRow = Environment.GetEnvironmentVariable("NOVAZ_SMOKE_TRACE_TOP_ROW") == "1";
    string? lastTopRow = null;
    int traceXram = ReadEnvHex("NOVAZ_SMOKE_TRACE_XRAM", -1);
    string? lastXram = null;
    bool traceCursor = Environment.GetEnvironmentVariable("NOVAZ_SMOKE_TRACE_CURSOR") == "1";
    var cursorTrace = new Queue<string>();
    int lastCx = -1, lastCy = -1;
    string? lastReadySnapshot = null;
    var probePcs = new HashSet<int>(
        (Environment.GetEnvironmentVariable("NOVAZ_SMOKE_PROBE_PC") ?? "")
            .Split(',', StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries)
            .Select(s => Convert.ToInt32(s, 16)));
    int opcodePcLoAddress = TryReadRuntimeSymbol("zvm_opcode_pc_l") ?? 0x0091;
    int opcodePcHiAddress = TryReadRuntimeSymbol("zvm_opcode_pc_h") ?? 0x0092;
    int opcodePcBankAddress = TryReadRuntimeSymbol("zvm_opcode_pc_b") ?? 0x00B3;
    int opcodeAddress = TryReadRuntimeSymbol("zvm_opcode") ?? 0x0093;

    for (int i = 0; i < maxSteps; i++)
    {
        int cycles = cpu.ClocksForNext();
        cpu.ExecuteNext();
        bus.AdvanceCycles(cycles);

        if (traceCursor)
        {
            int cx = bus.Vgc.GetCursorX();
            int cy = bus.Vgc.GetCursorY();
            if (cx != lastCx || cy != lastCy)
            {
                lastCx = cx;
                lastCy = cy;
                int zpc = ReadWord(bus, opcodePcHiAddress, opcodePcLoAddress);
                cursorTrace.Enqueue($"({cx},{cy})@pc${cpu.Pc:X4}|zpc${zpc:X4}|op${bus.Read((ushort)opcodeAddress):X2}");
                while (cursorTrace.Count > 60)
                    cursorTrace.Dequeue();
            }
        }

        int opcodePc = ReadWord(bus, opcodePcHiAddress, opcodePcLoAddress);
        if (opcodePc != lastOpcodePc)
        {
            lastOpcodePc = opcodePc;
            if (probePcs.Contains(opcodePc))
            {
                int lo = TryReadRuntimeSymbol("zvm_locals_lo") ?? 0x0314;
                int hi = TryReadRuntimeSymbol("zvm_locals_hi") ?? 0x0324;
                int spAddr = TryReadRuntimeSymbol("zvm_sp") ?? 0;
                int stkLo = TryReadRuntimeSymbol("zvm_stack_lo") ?? 0;
                int stkHi = TryReadRuntimeSymbol("zvm_stack_hi") ?? 0;
                int sp = spAddr > 0 ? bus.Read((ushort)spAddr) : -1;
                string top = sp > 0 && stkLo > 0
                    ? $"{bus.Read((ushort)(stkHi + sp - 1)):X2}{bus.Read((ushort)(stkLo + sp - 1)):X2}"
                    : "----";
                var locals = new List<string>();
                for (int li = 0; li < 6; li++)
                    locals.Add($"{ReadWord(bus, hi + li, lo + li):X4}");
                Console.Error.WriteLine($"probe pc=${opcodePc:X4} sp={sp:X2} top=${top} locals={string.Join(",", locals)}");
            }
            trace.Enqueue($"${bus.Read((ushort)opcodePcBankAddress):X2}{opcodePc:X4}:${bus.Read((ushort)opcodeAddress):X2}");
            while (trace.Count > 400)
                trace.Dequeue();
            if (traceTopRow)
            {
                string topRow = SnapshotTopRow(bus.Vgc).TrimEnd();
                if (topRow.Length > 0 && topRow != lastTopRow)
                {
                    lastTopRow = topRow;
                    Console.Error.WriteLine($"top-row: {FormatZvmState(cpu, bus)} recent={string.Join(" ", trace)} row='{topRow}'");
                }
            }
            if (traceXram >= 0)
            {
                string xram = ReadXramBytes(bus, traceXram, 16);
                if (xram != lastXram)
                {
                    lastXram = xram;
                    Console.Error.WriteLine($"xram[{traceXram:X4}]: {FormatZvmState(cpu, bus)} recent={string.Join(" ", trace)} bytes=[{xram}]");
                }
            }
        }

        if ((i & snapshotMask) != 0)
            continue;

        string screen = SnapshotScreen(bus.Vgc);
        if (screen.Contains("UNSUPPORTED Z-OPCODE", StringComparison.Ordinal))
            throw new InvalidOperationException($"NovaZ hit an unsupported opcode. {FormatZvmState(cpu, bus)} recent={string.Join(" ", trace)}\n{screen}");
        if (screen.Contains("[ MORE ]", StringComparison.Ordinal))
        {
            morePrompts++;
            editor.QueueInput(0x0D);
            RunForSteps(cpu, bus, 8_000);
            lastReadySnapshot = null;
            continue;
        }
        bool screenStable = screen == lastReadySnapshot;
        lastReadySnapshot = screen;
        if (HandleStartupPrompt(screen, cpu, bus, editor, screenStable))
            continue;
        if (IsReadyPrompt(cpu, bus, screen, rawInputModeAddress, readKeyLoopAddress, readTimedLoopAddress))
            return screen;
        // The PC is only sampled every 0x4000 steps. The untimed read spin is
        // two instructions (a divisor of the stride) so it is always sampled at
        // its head, but the interrupt-poll loop (timed input and/or sampled
        // sound) is many instructions long and may be sampled mid-body -- which
        // is fatal if its length happens to be a power of two. When the screen
        // already shows a settled input prompt, micro-step a bounded amount to
        // see if the CPU is simply spinning in a read loop, independent of where
        // in that loop the stride landed.
        if (SettledAtReadLoop(cpu, bus, screen, rawInputModeAddress, readKeyLoopAddress, readTimedLoopAddress))
            return screen;
    }

    string finalScreen = SnapshotScreen(bus.Vgc);
    if (IsReadyPrompt(cpu, bus, finalScreen, rawInputModeAddress, readKeyLoopAddress, readTimedLoopAddress))
        return finalScreen;
    if (SettledAtReadLoop(cpu, bus, finalScreen, rawInputModeAddress, readKeyLoopAddress, readTimedLoopAddress))
        return finalScreen;

    string recent = string.Join(" ", trace);
    if (traceCursor)
        Console.Error.WriteLine($"cursor-trace: {string.Join(" ", cursorTrace)}");
    throw new TimeoutException(
        $"Timed out waiting for input prompt. {FormatZvmState(cpu, bus)} recent={recent} " +
        $"{FormatPromptDiagnostics(bus.Vgc, finalScreen)}\n{finalScreen}");
}

static string FormatZvmState(Cpu cpu, CompositeBusDevice bus)
{
    int globals = ReadWord(bus, 0x0051, 0x0052);
    int opcodePcLo = TryReadRuntimeSymbol("zvm_opcode_pc_l") ?? 0x0091;
    int opcodePcHi = TryReadRuntimeSymbol("zvm_opcode_pc_h") ?? 0x0092;
    int opcodePcBank = TryReadRuntimeSymbol("zvm_opcode_pc_b") ?? 0x00B3;
    int opcodeAddr = TryReadRuntimeSymbol("zvm_opcode") ?? 0x0093;
    int pcLo = TryReadRuntimeSymbol("zvm_pc_l") ?? 0x008F;
    int pcHi = TryReadRuntimeSymbol("zvm_pc_h") ?? 0x0090;
    int pcBank = TryReadRuntimeSymbol("zvm_pc_b") ?? 0x00B2;
    int opcodePc = ReadWord(bus, opcodePcHi, opcodePcLo);
    int objectTable = ReadWord(bus, 0x004F, 0x0050);
    int operandLoAddress = TryReadRuntimeSymbol("zvm_operand_lo") ?? 0x0304;
    int operandHiAddress = TryReadRuntimeSymbol("zvm_operand_hi") ?? 0x030C;
    int localsLoAddress = TryReadRuntimeSymbol("zvm_locals_lo") ?? 0x0314;
    int localsHiAddress = TryReadRuntimeSymbol("zvm_locals_hi") ?? 0x0324;
    int operand0 = ReadWord(bus, operandHiAddress, operandLoAddress);
    int objectEntry = objectTable > 0 && operand0 > 0
        ? objectTable + 126 + ((operand0 - 1) * 14)
        : 0;
    int objectParent = objectEntry > 0 ? ReadXramWord(bus, objectEntry + 6) : 0;
    int objectSibling = objectEntry > 0 ? ReadXramWord(bus, objectEntry + 8) : 0;
    int objectChild = objectEntry > 0 ? ReadXramWord(bus, objectEntry + 10) : 0;
    int global10 = globals > 0 ? ReadXramWord(bus, globals) : 0;
    int global11 = globals > 0 ? ReadXramWord(bus, globals + 2) : 0;
    int global12 = globals > 0 ? ReadXramWord(bus, globals + 4) : 0;
    int global13 = globals > 0 ? ReadXramWord(bus, globals + 6) : 0;
    int stopAddress = TryReadRuntimeSymbol("zvm_stop_reason") ?? 0x0300;
    int spAddress = TryReadRuntimeSymbol("zvm_sp") ?? 0x0301;
    int frameCountAddress = TryReadRuntimeSymbol("zvm_frame_count") ?? 0x0302;
    int? verifySumLoAddress = TryReadRuntimeSymbol("zvm_verify_sum_lo");
    int? verifySumHiAddress = TryReadRuntimeSymbol("zvm_verify_sum_hi");
    int? verifyEndLoAddress = TryReadRuntimeSymbol("zvm_verify_end_l");
    int? verifyEndMidAddress = TryReadRuntimeSymbol("zvm_verify_end_m");
    int? verifyEndHiAddress = TryReadRuntimeSymbol("zvm_verify_end_h");
    int? fileBytesLAddress = TryReadRuntimeSymbol("zstory_filebytes_l");
    int? fileBytesMAddress = TryReadRuntimeSymbol("zstory_filebytes_m");
    int? fileBytesHAddress = TryReadRuntimeSymbol("zstory_filebytes_h");
    int? checksumLoAddress = TryReadRuntimeSymbol("zstory_checksum_lo");
    int? checksumHiAddress = TryReadRuntimeSymbol("zstory_checksum_hi");
    int? vtextCurXAddress = TryReadRuntimeSymbol("VTEXT_CURX");
    int? vtextCurYAddress = TryReadRuntimeSymbol("VTEXT_CURY");
    int? vtextTopAddress = TryReadRuntimeSymbol("VTEXT_TOP");
    int? vtextLeftAddress = TryReadRuntimeSymbol("VTEXT_LEFT");
    string verify = verifySumLoAddress is int sumLo && verifySumHiAddress is int sumHi
        ? $" verify=${bus.Read((ushort)sumHi):X2}{bus.Read((ushort)sumLo):X2}"
        : "";
    string verifyEnd =
        verifyEndLoAddress is int endLo &&
        verifyEndMidAddress is int endMid &&
        verifyEndHiAddress is int endHi
            ? $" verifyEnd=${bus.Read((ushort)endHi):X2}{bus.Read((ushort)endMid):X2}{bus.Read((ushort)endLo):X2}"
            : "";
    string storyMeta =
        fileBytesLAddress is int fileL &&
        fileBytesMAddress is int fileM &&
        fileBytesHAddress is int fileH &&
        checksumLoAddress is int checkLo &&
        checksumHiAddress is int checkHi
            ? $" storyBytes=${bus.Read((ushort)fileH):X2}{bus.Read((ushort)fileM):X2}{bus.Read((ushort)fileL):X2} checksum=${bus.Read((ushort)checkHi):X2}{bus.Read((ushort)checkLo):X2}"
            : "";
    string vtext =
        vtextCurXAddress is int vx &&
        vtextCurYAddress is int vy &&
        vtextLeftAddress is int vl &&
        vtextTopAddress is int vt
            ? $" vtext={bus.Read((ushort)vl)},{bus.Read((ushort)vt)}+{bus.Read((ushort)vx)},{bus.Read((ushort)vy)}"
            : "";
    var state = cpu.GetState();
    return
        $"cpu=PC${cpu.Pc:X4}/A${state.A:X2}/X${state.X:X2}/Y${state.Y:X2} " +
        $"zvm_pc=${bus.Read((ushort)pcBank):X2}{bus.Read((ushort)pcHi):X2}{bus.Read((ushort)pcLo):X2} " +
        $"op_pc=${bus.Read((ushort)opcodePcBank):X2}{opcodePc:X4} op_bytes=${ReadXramWord(bus, (bus.Read((ushort)opcodePcBank) << 16) | opcodePc):X4} " +
        $"globals=${globals:X4} g10=${global10:X4} g11=${global11:X4} g12=${global12:X4} g13=${global13:X4} " +
        $"obj=${operand0:X4} objEntry=${objectEntry:X4} objParent=${objectParent:X4} objSibling=${objectSibling:X4} objChild=${objectChild:X4} " +
        $"zaddr=${bus.Read(0x0042):X2}{bus.Read(0x0041):X2}{bus.Read(0x0040):X2} " +
        $"xaddr=${bus.Read(0x0022):X2}{bus.Read(0x0021):X2}{bus.Read(0x0020):X2} xdata=${bus.Read(0x0023):X2} " +
        $"fio=${bus.Read(VgcConstants.FioCmd):X2}/${bus.Read(VgcConstants.FioStatus):X2}/${bus.Read(VgcConstants.FioErrCode):X2} " +
        $"xmc=${bus.Read(VgcConstants.XmcStatus):X2}/${bus.Read(VgcConstants.XmcErrCode):X2} " +
        $"cursor={bus.Vgc.GetCursorX()},{bus.Vgc.GetCursorY()}{vtext} " +
        $"zvm_opcode=${bus.Read((ushort)opcodeAddr):X2} stop=${bus.Read((ushort)stopAddress):X2} sp=${bus.Read((ushort)spAddress):X2} frames=${bus.Read((ushort)frameCountAddress):X2}{verify}{verifyEnd}{storyMeta} " +
        $"ops={ReadWord(bus, operandHiAddress, operandLoAddress):X4},{ReadWord(bus, operandHiAddress + 1, operandLoAddress + 1):X4},{ReadWord(bus, operandHiAddress + 2, operandLoAddress + 2):X4},{ReadWord(bus, operandHiAddress + 3, operandLoAddress + 3):X4} " +
        $"locals={ReadWord(bus, localsHiAddress, localsLoAddress):X4},{ReadWord(bus, localsHiAddress + 1, localsLoAddress + 1):X4},{ReadWord(bus, localsHiAddress + 2, localsLoAddress + 2):X4},{ReadWord(bus, localsHiAddress + 3, localsLoAddress + 3):X4}";
}

static string FormatParserState(CompositeBusDevice bus)
{
    int text = ReadWord(bus, 0x1749, 0x1748);
    int parse = ReadWord(bus, 0x174B, 0x174A);
    int inputLen = bus.Read(0x174D);
    int dict = ReadWord(bus, 0x1755, 0x1754);
    int dictFound = ReadWord(bus, 0x175F, 0x175E);
    int parseCount = parse > 0 ? ReadXramByte(bus, parse + 1) : 0;
    string textBytes = text > 0 ? ReadXramBytes(bus, text, Math.Min(inputLen + 2, 12)) : "";
    string parseBytes = parse > 0 ? ReadXramBytes(bus, parse, Math.Min(2 + (parseCount * 4), 18)) : "";
    string dictBytes = dict > 0 ? ReadXramBytes(bus, dict, 12) : "";
    string encoded = ReadRamBytes(bus, 0x176C, 6);
    return $"parser=text${text:X4} parse${parse:X4} dict${dict:X4} found${dictFound:X4} inputLen={inputLen} parseCount={parseCount} text=[{textBytes}] parse=[{parseBytes}] dictBytes=[{dictBytes}] enc=[{encoded}]";
}

static string ReadRamBytes(CompositeBusDevice bus, int address, int count)
{
    var sb = new StringBuilder();
    for (int i = 0; i < count; i++)
    {
        if (i > 0)
            sb.Append(' ');
        sb.Append(bus.Read((ushort)(address + i)).ToString("X2"));
    }
    return sb.ToString();
}

// Step until the CPU parks in the runtime's terminal halt loop (halt: WAI /
// BRA halt — a 3-byte window the PC can never leave), then return. Sampling
// between chunks is safe: once parked, every later sample sees the window.
static void RunUntilCpuHalt(Cpu cpu, CompositeBusDevice bus, int maxSteps, int haltAddress)
{
    const int chunk = 10_000;
    for (int executed = 0; executed < maxSteps; executed += chunk)
    {
        RunForSteps(cpu, bus, chunk);
        // halt: is WAI + BRA (3 bytes); halt+3 is already the next routine.
        if (cpu.Pc >= haltAddress && cpu.Pc < haltAddress + 3)
            return;
    }

    throw new TimeoutException($"Timed out waiting for the runtime halt loop at ${haltAddress:X4}; cpu=PC${cpu.Pc:X4}.\n{SnapshotScreen(bus.Vgc)}");
}

static void RunForSteps(Cpu cpu, CompositeBusDevice bus, int steps)
{
    for (int i = 0; i < steps; i++)
    {
        YieldHostHardware(i);

        int cycles = cpu.ClocksForNext();
        cpu.ExecuteNext();
        bus.AdvanceCycles(cycles);
    }
}

static void YieldHostHardware(int step)
{
    if ((step & 0x3FF) == 0)
        Thread.Sleep(0);
}

static void SendLine(Cpu cpu, CompositeBusDevice bus, ScreenEditor editor, string text)
{
    bool traceInput = Environment.GetEnvironmentVariable("NOVAZ_SMOKE_TRACE_INPUT") == "1";
    foreach (char ch in text)
    {
        editor.QueueInput((byte)ch);
        RunForSteps(cpu, bus, 50_000);
        if (traceInput)
            Console.Error.WriteLine($"input '{ch}': {FormatZvmState(cpu, bus)} {FormatParserState(bus)}");
    }
    editor.QueueInput(0x0D);
    RunForSteps(cpu, bus, 50_000);
    if (traceInput)
        Console.Error.WriteLine($"input CR: {FormatZvmState(cpu, bus)} {FormatParserState(bus)}");
}

static void SendRaw(Cpu cpu, CompositeBusDevice bus, ScreenEditor editor, string text)
{
    bool traceInput = Environment.GetEnvironmentVariable("NOVAZ_SMOKE_TRACE_INPUT") == "1";
    foreach (char ch in text)
    {
        editor.QueueInput((byte)ch);
        RunForSteps(cpu, bus, 50_000);
        if (traceInput)
            Console.Error.WriteLine($"raw '{ch}': {FormatZvmState(cpu, bus)} {FormatParserState(bus)}");
    }
}

static bool HandleStartupPrompt(string screen, Cpu cpu, CompositeBusDevice bus, ScreenEditor editor, bool screenStable)
{
    if (screen.Contains("Hit any key", StringComparison.OrdinalIgnoreCase) ||
        screen.Contains("Press any key", StringComparison.OrdinalIgnoreCase))
    {
        editor.QueueInput(0x20);
        RunForSteps(cpu, bus, 500_000);
        return true;
    }

    if (screen.Contains("Is this a VT220", StringComparison.OrdinalIgnoreCase) ||
        screen.Contains("Please type YES or NO", StringComparison.OrdinalIgnoreCase))
    {
        SendLine(cpu, bus, editor, "NO");
        return true;
    }

    if (screen.Contains("Type RESTORE", StringComparison.OrdinalIgnoreCase) &&
        screen.Contains("start the game", StringComparison.OrdinalIgnoreCase))
    {
        if (screen.Contains(">n", StringComparison.OrdinalIgnoreCase))
            return false;
        SendLine(cpu, bus, editor, "N");
        return true;
    }

    if (screen.Contains("Type BEGIN, RESTORE or QUIT", StringComparison.OrdinalIgnoreCase))
    {
        if (screen.Contains(">begin", StringComparison.OrdinalIgnoreCase))
            return false;
        SendLine(cpu, bus, editor, "BEGIN");
        return true;
    }

    if (screen.Contains("Which chapter would you like", StringComparison.OrdinalIgnoreCase))
    {
        if (screen.Contains(">1", StringComparison.OrdinalIgnoreCase))
            return false;
        SendLine(cpu, bus, editor, "1");
        return true;
    }

    if (screen.Contains("beyond Science", StringComparison.OrdinalIgnoreCase) ||
        screen.Contains("ZORK is a registered trademark", StringComparison.OrdinalIgnoreCase))
    {
        // These banner/credits strings appear both on press-a-key title screens
        // (e.g. Beyond Zork, which parks waiting for a keystroke) AND as permanent
        // copyright text on the main play screen (e.g. Zork I/II, where the game
        // is already at its live ">" command prompt). Injecting a space into a
        // live READ feeds stray characters into the command and walks the cursor.
        // Screen text and the raw-input flag alone cannot tell a transient
        // mid-boot pause apart from a real title gate, so confirm by waiting: a
        // title gate does not advance on its own, whereas boot/compute does. Only
        // inject if the game is not in line input (raw input mode) and the screen
        // stays identical after running forward.
        int? rawModeAddr = TryReadRuntimeSymbol("nz_raw_input_mode");
        bool inLineRead = rawModeAddr is int addr && bus.Read((ushort)addr) != 0;
        if (inLineRead || !screenStable)
            return false;
        RunForSteps(cpu, bus, 1_000_000);
        if (SnapshotScreen(bus.Vgc) != screen)
            return true;   // advanced on its own — not a keypress gate
        editor.QueueInput(0x20);
        RunForSteps(cpu, bus, 500_000);
        return true;
    }

    if (screen.Contains("Type [RETURN] to continue", StringComparison.OrdinalIgnoreCase))
    {
        editor.QueueInput(0x0D);
        RunForSteps(cpu, bus, 500_000);
        return true;
    }

    if ((screen.Contains("RETURN", StringComparison.OrdinalIgnoreCase) ||
         screen.Contains("ENTER", StringComparison.OrdinalIgnoreCase)) &&
        (screen.Contains("to begin", StringComparison.OrdinalIgnoreCase) ||
         screen.Contains("to start", StringComparison.OrdinalIgnoreCase) ||
         screen.Contains("to continue", StringComparison.OrdinalIgnoreCase)))
    {
        // Many titles park on an intro screen before the first ">" prompt, e.g.
        // "Hit the RETURN/ENTER key to begin!" (Leather Goddesses) or
        // "[Press RETURN or ENTER to begin.]" (Plundered Hearts). These are
        // single-key reads (raw input mode), so press unconditionally like the
        // "Press any key" gate above; the "to begin/start/continue" phrasing
        // cannot appear on a live command prompt, so there is no live read to
        // corrupt.
        editor.QueueInput(0x0D);
        RunForSteps(cpu, bus, 500_000);
        return true;
    }

    return false;
}

static int ReadWord(CompositeBusDevice bus, int hi, int lo) =>
    (bus.Read((ushort)hi) << 8) | bus.Read((ushort)lo);

static int ReadXramWord(CompositeBusDevice bus, int address)
    => (ReadXramByte(bus, address) << 8) | ReadXramByte(bus, address + 1);

static int ReadXramByte(CompositeBusDevice bus, int address)
{
    bus.Write(VgcConstants.XmcWin3AL, 0);
    bus.Write(VgcConstants.XmcWin3AM, (byte)((address >> 8) & 0xFF));
    bus.Write(VgcConstants.XmcWin3AH, (byte)((address >> 16) & 0xFF));
    return bus.Read((ushort)(0xBF00 + (address & 0xFF)));
}

static string ReadXramBytes(CompositeBusDevice bus, int address, int count)
{
    var sb = new StringBuilder();
    for (int i = 0; i < count; i++)
    {
        if (i > 0)
            sb.Append(' ');
        sb.Append(ReadXramByte(bus, address + i).ToString("X2"));
    }
    return sb.ToString();
}

static string ExtractCommandTranscript(string screen, string command)
{
    string[] lines = screen.Split('\n');
    string echo = ">" + command;
    for (int i = lines.Length - 1; i >= 0; i--)
    {
        string line = lines[i].TrimEnd();
        if (!line.Equals(echo, StringComparison.OrdinalIgnoreCase))
            continue;

        return string.Join('\n', lines.Skip(i));
    }

    return screen;
}

static void RequireContains(string screen, string expected, string? command = null)
{
    if (!screen.Contains(expected, StringComparison.Ordinal) &&
        !NormalizeWhitespace(screen).Contains(NormalizeWhitespace(expected), StringComparison.Ordinal))
    {
        string context = command is null ? "screen" : $"output after '{command}'";
        throw new InvalidOperationException($"Expected {context} to contain '{expected}'.\n{screen}");
    }
}

static void RequireSoundfontLoaded(CompositeBusDevice bus, string screen)
{
    if (!bus.ZSound.PackLoaded)
        throw new InvalidOperationException(
            "Expected the story's soundfont (SOUND.PAK) to be loaded after boot, " +
            "but ZSound.PackLoaded is false. The image may not bundle sounds, or the " +
            $"runtime did not issue the boot preload.\n{screen}");
    Console.WriteLine("soundfont loaded (SOUND.PAK resident at boot)");
}

static void RequireTextColor(VirtualGraphicsController vgc, string screen, ExpectedTextColor expected)
{
    (int row, int col)? position = FindText(screen, expected.Text);
    if (position is null)
        throw new InvalidOperationException($"Expected screen to contain text for color check '{expected.Text}'.\n{screen}");

    (int row, int col) = position.Value;
    for (int i = 0; i < expected.Text.Length; i++)
    {
        if (expected.Text[i] == ' ')
            continue;
        byte actual = vgc.GetScreenColor(col + i, row);
        if (actual == expected.Color)
            continue;

        throw new InvalidOperationException(
            $"Expected '{expected.Text}' at {col},{row} to use text color ${expected.Color:X2}; " +
            $"cell {col + i},{row} was ${actual:X2}.\n{screen}");
    }
}

static void RequireGfxColor(VirtualGraphicsController vgc, string screen, ExpectedGfxColor expected)
{
    byte actual = vgc.GetGfxPixelColor(expected.X, expected.Y);
    if (actual == expected.Color)
        return;

    throw new InvalidOperationException(
        $"Expected graphics pixel {expected.X},{expected.Y} to use color ${expected.Color:X2}; " +
        $"was ${actual:X2}.\n{screen}");
}

static (int Row, int Col)? FindText(string screen, string text)
{
    string[] lines = screen.Split('\n');
    for (int row = 0; row < lines.Length; row++)
    {
        int col = lines[row].IndexOf(text, StringComparison.Ordinal);
        if (col >= 0)
            return (row, col);
    }

    return null;
}

static void RequireContainsAny(string screen, params string[] expectedValues)
{
    foreach (string expected in expectedValues)
    {
        if (ContainsNormalized(screen, expected))
            return;
    }

    string expectedList = string.Join("' or '", expectedValues);
    throw new InvalidOperationException(
        $"Expected screen to contain '{expectedList}'.\n{screen}");
}

static bool ContainsNormalized(string screen, string expected)
{
    return screen.Contains(expected, StringComparison.Ordinal) ||
        NormalizeWhitespace(screen).Contains(NormalizeWhitespace(expected), StringComparison.Ordinal);
}

static string NormalizeWhitespace(string text)
{
    var sb = new StringBuilder(text.Length);
    bool inWhitespace = false;
    foreach (char ch in text)
    {
        if (char.IsWhiteSpace(ch))
        {
            if (!inWhitespace)
                sb.Append(' ');
            inWhitespace = true;
            continue;
        }

        sb.Append(ch);
        inWhitespace = false;
    }

    return sb.ToString();
}

static void RequireStatusLine(string screen, Cpu cpu, CompositeBusDevice bus)
{
    string firstLine = screen.Split('\n')[0];
    int scoreAt = firstLine.IndexOf("Score:", StringComparison.Ordinal);
    int movesAt = firstLine.IndexOf("Moves:", StringComparison.Ordinal);
    int timeAt = firstLine.IndexOf("Time:", StringComparison.Ordinal);
    bool hasScoreStatus = scoreAt > 0 &&
        movesAt > scoreAt &&
        !string.IsNullOrWhiteSpace(firstLine[..scoreAt]);
    bool hasTimeStatus = timeAt > 0 &&
        !string.IsNullOrWhiteSpace(firstLine[..timeAt]);
    if (!hasScoreStatus && !hasTimeStatus)
    {
        int globals = ReadWord(bus, 0x0050, 0x0051);
        int objectTable = ReadWord(bus, 0x004E, 0x004F);
        int room = ReadXramWord(bus, globals) & 0xFF;
        int entry = objectTable + 62 + ((room - 1) * 9);
        int propTable = room > 0 ? ReadXramWord(bus, entry + 7) : 0;
        int nameWords = propTable > 0 ? ReadXramByte(bus, propTable) : 0;
        string nameBytes = propTable > 0 ? ReadXramBytes(bus, propTable, 12) : "";
        throw new InvalidOperationException(
            $"Expected status line to show location plus score/moves or time. {FormatZvmState(cpu, bus)} " +
            $"room={room} objEntry=${entry:X4} prop=${propTable:X4} nameWords={nameWords} nameBytes={nameBytes}\n{screen}");
    }
}

static void RequireTimeStatusLine(string screen)
{
    string firstLine = screen.Split('\n')[0];
    int timeAt = firstLine.IndexOf("Time:", StringComparison.Ordinal);
    if (timeAt <= 0 || string.IsNullOrWhiteSpace(firstLine[..timeAt]))
        throw new InvalidOperationException($"Expected time-game status line.\n{screen}");
}

static void RequireReadyPrompt(
    Cpu cpu,
    CompositeBusDevice bus,
    string screen,
    int? rawInputModeAddress,
    int? readKeyLoopAddress,
    int? readTimedLoopAddress)
{
    if (!IsReadyPrompt(cpu, bus, screen, rawInputModeAddress, readKeyLoopAddress, readTimedLoopAddress))
    {
        throw new InvalidOperationException(
            $"Expected visible cursor or active read loop on a bare Zork prompt; got {bus.Vgc.GetCursorX()},{bus.Vgc.GetCursorY()} pc=${cpu.Pc:X4} opcode=${bus.Read((ushort)(TryReadRuntimeSymbol("zvm_opcode") ?? 0x0093)):X2}. " +
            $"{FormatPromptDiagnostics(bus.Vgc, screen)}\n{screen}");
    }
}

static bool IsReadyPrompt(
    Cpu cpu,
    CompositeBusDevice bus,
    string screen,
    int? rawInputModeAddress,
    int? readKeyLoopAddress,
    int? readTimedLoopAddress)
{
    string[] lines = screen.Split('\n');
    VirtualGraphicsController vgc = bus.Vgc;
    if (rawInputModeAddress is int address && readKeyLoopAddress is int keyLoopAddress)
    {
        // The interpreter waits for line input in one of two loops: the untimed
        // tight spin (zvm_read_key_loop) or, for V4+ timed input / sampled
        // sound, the interrupt poll (zvm_read_timed_poll). Both are valid
        // "ready for input" states.
        return IsAtReadLoop(cpu, keyLoopAddress, readTimedLoopAddress) &&
            bus.Read((ushort)address) != 0 &&
            HasCursorBarePrompt(vgc, lines);
    }

    return HasCursorBarePrompt(vgc, lines);
}

static bool IsAtReadLoop(Cpu cpu, int keyLoopAddress, int? readTimedLoopAddress)
{
    // Either of the read loop's first two instructions (the input-port load or
    // the branch back) is "at the loop".
    if (cpu.Pc == keyLoopAddress || cpu.Pc == keyLoopAddress + 3)
        return true;
    return readTimedLoopAddress is int timedLoop &&
        (cpu.Pc == timedLoop || cpu.Pc == timedLoop + 3);
}

// Fallback for the interrupt-poll loop, whose length may defeat the fixed PC
// sampling stride. If the screen is already a settled bare input prompt with
// raw input armed, micro-step a bounded amount: if the CPU passes through the
// read-loop head it is genuinely waiting for input. Leaves the CPU parked at
// the loop head when it returns true so later re-validation also succeeds.
static bool SettledAtReadLoop(
    Cpu cpu,
    CompositeBusDevice bus,
    string screen,
    int? rawInputModeAddress,
    int? readKeyLoopAddress,
    int? readTimedLoopAddress)
{
    if (rawInputModeAddress is not int address || readKeyLoopAddress is not int keyLoopAddress)
        return false;
    if (bus.Read((ushort)address) == 0)
        return false;
    if (!HasCursorBarePrompt(bus.Vgc, screen.Split('\n')))
        return false;

    // One full interrupt-poll iteration (including the timer/sound service
    // subroutines) is well under 256 instructions.
    for (int s = 0; s < 256; s++)
    {
        if (IsAtReadLoop(cpu, keyLoopAddress, readTimedLoopAddress))
            return true;
        int cycles = cpu.ClocksForNext();
        cpu.ExecuteNext();
        bus.AdvanceCycles(cycles);
    }
    return IsAtReadLoop(cpu, keyLoopAddress, readTimedLoopAddress);
}

static bool HasCursorBarePrompt(VirtualGraphicsController vgc, string[] lines)
{
    int actualX = vgc.GetCursorX();
    int actualY = vgc.GetCursorY();
    if (actualY >= 0 && actualY < Math.Min(lines.Length, VgcConstants.ScreenRows) && actualX == 1)
    {
        string line = lines[actualY];
        if (line.Length > 0 && line[0] == '>')
            return line[1..].All(ch => ch == ' ');
    }

    return false;
}

static string FormatPromptDiagnostics(VirtualGraphicsController vgc, string screen)
{
    string[] lines = screen.Split('\n');
    int cursorY = vgc.GetCursorY();
    int cursorX = vgc.GetCursorX();
    int firstRow = Math.Max(0, cursorY - 2);
    int lastRow = Math.Min(Math.Min(lines.Length, VgcConstants.ScreenRows) - 1, cursorY + 2);
    var sb = new StringBuilder($"cursor={cursorX},{cursorY}");
    for (int row = firstRow; row <= lastRow; row++)
        sb.Append($" row{row}='{lines[row].TrimEnd()}'");
    return sb.ToString();
}

static string SnapshotScreen(VirtualGraphicsController vgc)
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

static string SnapshotTopRow(VirtualGraphicsController vgc)
{
    var sb = new StringBuilder(VgcConstants.ScreenCols);
    for (int col = 0; col < VgcConstants.ScreenCols; col++)
    {
        byte ch = vgc.GetScreenChar(col, 0);
        sb.Append(ch >= 0x20 && ch <= 0x7E ? (char)ch : ' ');
    }
    return sb.ToString();
}

enum SmokeInputMode
{
    Line,
    Raw,
    Wait,
    ExpectStop
}

sealed record SmokeCommand(
    string Text,
    IReadOnlyList<string> Expected,
    SmokeInputMode Mode = SmokeInputMode.Line,
    bool WaitForPrompt = true);
sealed record ExpectedTextColor(string Text, byte Color);
sealed record ExpectedAt(int Col, int Row, string Text);
sealed record ExpectedGfxColor(int X, int Y, byte Color);

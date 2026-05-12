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
    Console.Error.WriteLine("       Nova.NovaZ.Smoke <fd0.ndi> --generic-boot [--boot-only] [--screen-only] [--screen-input <text>] [--expect-more] [--expect-time-status] [--expect-screen <text>] [--expect-text-color <text=>hex>] [--expect-gfx-color <x,y=>hex>] [--skip-manifest-check]");
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
List<string> expectedScreens = LoadExpectedScreens(args);
List<string> screenInputs = LoadScreenInputs(args);
List<ExpectedTextColor> expectedTextColors = LoadExpectedTextColors(args);
List<ExpectedGfxColor> expectedGfxColors = LoadExpectedGfxColors(args);
List<SmokeCommand> commands = LoadCommands(args, bootOnly, screenOnly);
JsonNode? manifest = TryReadManifest(imagePath);
int? rawInputModeAddress = TryReadRuntimeSymbol("nz_raw_input_mode");
int? readKeyLoopAddress = TryReadRuntimeSymbol("zvm_read_key_loop");
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

    if (screenOnly)
    {
        if (expectedScreens.Count == 0 && expectedTextColors.Count == 0 && expectedGfxColors.Count == 0)
            throw new InvalidOperationException("--screen-only requires at least one --expect-screen, --expect-text-color, or --expect-gfx-color check.");

        foreach (string input in screenInputs)
            SendRaw(cpu, bus, editor, input);

        string screenOnlySnapshot = RunUntilScreenMatches(cpu, bus, editor, maxSteps, expectedScreens, expectedTextColors, expectedGfxColors, ref morePrompts);
        foreach (string expected in expectedScreens)
            RequireContains(screenOnlySnapshot, expected);
        foreach (var expected in expectedTextColors)
            RequireTextColor(bus.Vgc, screenOnlySnapshot, expected);
        foreach (var expected in expectedGfxColors)
            RequireGfxColor(bus.Vgc, screenOnlySnapshot, expected);
        if (expectMore && morePrompts == 0)
            throw new InvalidOperationException($"Expected at least one [ MORE ] prompt.\n{screenOnlySnapshot}");

        Console.WriteLine($"NovaZ screen smoke passed. morePrompts={morePrompts}");
        Console.WriteLine(screenOnlySnapshot);
        return 0;
    }

    string screen = RunUntilReadyPrompt(cpu, bus, editor, maxSteps, ref morePrompts, rawInputModeAddress, readKeyLoopAddress);
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
    RequireReadyPrompt(cpu, bus, screen, rawInputModeAddress, readKeyLoopAddress);

    string bootScreen = screen;

    foreach (var command in commands)
    {
        if (command.Text.Equals(".reboot", StringComparison.OrdinalIgnoreCase))
        {
            Console.WriteLine("> .reboot");
            cpu.Boot();
            RunForSteps(cpu, bus, 500_000);
            screen = RunUntilReadyPrompt(cpu, bus, editor, maxSteps, ref morePrompts, rawInputModeAddress, readKeyLoopAddress);
            RunForSteps(cpu, bus, 200_000);
            screen = SnapshotScreen(bus.Vgc);
            if (screen.Contains("UNSUPPORTED Z-OPCODE", StringComparison.Ordinal))
                throw new InvalidOperationException($"NovaZ hit an unsupported opcode after reboot.\n{screen}");
            RequireContains(screen, ">");
            if (!noStatusLine)
                RequireStatusLine(screen, cpu, bus);
            if (expectTimeStatus)
                RequireTimeStatusLine(screen);
            RequireReadyPrompt(cpu, bus, screen, rawInputModeAddress, readKeyLoopAddress);
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
            screen = RunUntilReadyPrompt(cpu, bus, editor, maxSteps, ref morePrompts, rawInputModeAddress, readKeyLoopAddress);
        }
        else
        {
            if (command.Expected.Count == 0)
                throw new InvalidOperationException($"No-prompt command '{command.Text}' requires expected screen text.");
            screen = RunUntilScreenMatches(cpu, bus, editor, maxSteps, command.Expected, [], [], ref morePrompts);
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
            RequireReadyPrompt(cpu, bus, screen, rawInputModeAddress, readKeyLoopAddress);
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
                break;
            case "--expect-screen":
                if (i + 1 >= args.Length)
                    throw new ArgumentException("--expect-screen requires text.");
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
    IReadOnlyList<ExpectedTextColor> expectedTextColors,
    IReadOnlyList<ExpectedGfxColor> expectedGfxColors,
    ref int morePrompts)
{
    const int snapshotMask = 0x3FFF;
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
            continue;
        }
        if (HandleStartupPrompt(screen, cpu, bus, editor))
            continue;

        bool hasExpectedScreens = expectedScreens.All(expected => ContainsNormalized(screen, expected));
        bool hasExpectedColorText = expectedTextColors.All(expected => FindText(screen, expected.Text) is not null);
        bool hasExpectedGfxColors = expectedGfxColors.All(expected => bus.Vgc.GetGfxPixelColor(expected.X, expected.Y) == expected.Color);
        if (hasExpectedScreens && hasExpectedColorText && hasExpectedGfxColors)
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
    int? readKeyLoopAddress)
{
    const int snapshotMask = 0x3FFF;
    var trace = new Queue<string>();
    int lastOpcodePc = -1;
    bool traceTopRow = Environment.GetEnvironmentVariable("NOVAZ_SMOKE_TRACE_TOP_ROW") == "1";
    string? lastTopRow = null;
    int traceXram = ReadEnvHex("NOVAZ_SMOKE_TRACE_XRAM", -1);
    string? lastXram = null;

    for (int i = 0; i < maxSteps; i++)
    {
        int cycles = cpu.ClocksForNext();
        cpu.ExecuteNext();
        bus.AdvanceCycles(cycles);

        int opcodePc = ReadWord(bus, 0x008E, 0x008D);
        if (opcodePc != lastOpcodePc)
        {
            lastOpcodePc = opcodePc;
            trace.Enqueue($"${bus.Read(0x00AF):X2}{opcodePc:X4}:${bus.Read(0x008F):X2}");
            while (trace.Count > 24)
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
            throw new InvalidOperationException($"NovaZ hit an unsupported opcode. {FormatZvmState(cpu, bus)}\n{screen}");
        if (screen.Contains("[ MORE ]", StringComparison.Ordinal))
        {
            morePrompts++;
            editor.QueueInput(0x0D);
            RunForSteps(cpu, bus, 8_000);
            continue;
        }
        if (HandleStartupPrompt(screen, cpu, bus, editor))
            continue;
        if (IsReadyPrompt(cpu, bus, screen, rawInputModeAddress, readKeyLoopAddress))
            return screen;
    }

    string finalScreen = SnapshotScreen(bus.Vgc);
    if (IsReadyPrompt(cpu, bus, finalScreen, rawInputModeAddress, readKeyLoopAddress))
        return finalScreen;

    string recent = string.Join(" ", trace);
    throw new TimeoutException(
        $"Timed out waiting for input prompt. {FormatZvmState(cpu, bus)} recent={recent} " +
        $"{FormatPromptDiagnostics(bus.Vgc, finalScreen)}\n{finalScreen}");
}

static string FormatZvmState(Cpu cpu, CompositeBusDevice bus)
{
    int globals = ReadWord(bus, 0x0051, 0x0052);
    int opcodePc = ReadWord(bus, 0x008E, 0x008D);
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
        $"zvm_pc=${bus.Read(0x00AE):X2}{bus.Read(0x008C):X2}{bus.Read(0x008B):X2} " +
        $"op_pc=${bus.Read(0x00AF):X2}{opcodePc:X4} op_bytes=${ReadXramWord(bus, (bus.Read(0x00AF) << 16) | opcodePc):X4} " +
        $"globals=${globals:X4} g10=${global10:X4} g11=${global11:X4} g12=${global12:X4} g13=${global13:X4} " +
        $"obj=${operand0:X4} objEntry=${objectEntry:X4} objParent=${objectParent:X4} objSibling=${objectSibling:X4} objChild=${objectChild:X4} " +
        $"zaddr=${bus.Read(0x0042):X2}{bus.Read(0x0041):X2}{bus.Read(0x0040):X2} " +
        $"xaddr=${bus.Read(0x0022):X2}{bus.Read(0x0021):X2}{bus.Read(0x0020):X2} xdata=${bus.Read(0x0023):X2} " +
        $"fio=${bus.Read(VgcConstants.FioCmd):X2}/${bus.Read(VgcConstants.FioStatus):X2}/${bus.Read(VgcConstants.FioErrCode):X2} " +
        $"xmc=${bus.Read(VgcConstants.XmcStatus):X2}/${bus.Read(VgcConstants.XmcErrCode):X2} " +
        $"cursor={bus.Vgc.GetCursorX()},{bus.Vgc.GetCursorY()}{vtext} " +
        $"zvm_opcode=${bus.Read(0x008F):X2} stop=${bus.Read((ushort)stopAddress):X2} sp=${bus.Read((ushort)spAddress):X2} frames=${bus.Read((ushort)frameCountAddress):X2}{verify}{verifyEnd}{storyMeta} " +
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

static bool HandleStartupPrompt(string screen, Cpu cpu, CompositeBusDevice bus, ScreenEditor editor)
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
    int? readKeyLoopAddress)
{
    if (!IsReadyPrompt(cpu, bus, screen, rawInputModeAddress, readKeyLoopAddress))
    {
        throw new InvalidOperationException(
            $"Expected visible cursor or active read loop on a bare Zork prompt; got {bus.Vgc.GetCursorX()},{bus.Vgc.GetCursorY()} pc=${cpu.Pc:X4} opcode=${bus.Read(0x008F):X2}. " +
            $"{FormatPromptDiagnostics(bus.Vgc, screen)}\n{screen}");
    }
}

static bool IsReadyPrompt(
    Cpu cpu,
    CompositeBusDevice bus,
    string screen,
    int? rawInputModeAddress,
    int? readKeyLoopAddress)
{
    string[] lines = screen.Split('\n');
    VirtualGraphicsController vgc = bus.Vgc;
    if (rawInputModeAddress is int address && readKeyLoopAddress is int keyLoopAddress)
    {
        bool inReadLoop = cpu.Pc == keyLoopAddress || cpu.Pc == keyLoopAddress + 3;
        return inReadLoop &&
            bus.Read((ushort)address) != 0 &&
            HasCursorBarePrompt(vgc, lines);
    }

    return HasCursorBarePrompt(vgc, lines);
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
sealed record ExpectedGfxColor(int X, int Y, byte Color);

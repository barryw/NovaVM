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
    Console.Error.WriteLine("       Nova.NovaZ.Smoke <fd0.ndi> --generic-boot [--boot-only] [--expect-more] [--expect-time-status] [--expect-screen <text>] [--skip-manifest-check]");
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
bool expectMore = args.Skip(1).Contains("--expect-more", StringComparer.Ordinal);
bool expectTimeStatus = args.Skip(1).Contains("--expect-time-status", StringComparer.Ordinal);
bool skipManifestCheck = args.Skip(1).Contains("--skip-manifest-check", StringComparer.Ordinal);
List<string> expectedScreens = LoadExpectedScreens(args);
List<SmokeCommand> commands = LoadCommands(args, bootOnly);
JsonNode? manifest = TryReadManifest(imagePath);
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
    string screen = RunUntilReadyPrompt(cpu, bus, editor, maxSteps, ref morePrompts);
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
    RequireStatusLine(screen, cpu, bus);
    if (expectTimeStatus)
        RequireTimeStatusLine(screen);
    foreach (string expected in expectedScreens)
        RequireContains(screen, expected);
    RequireReadyPrompt(bus.Vgc, screen);

    editor.QueueInput(0xF0);
    RunForSteps(cpu, bus, 8_000);
    screen = SnapshotScreen(bus.Vgc);
    RequireReadyPrompt(bus.Vgc, screen);

    string bootScreen = screen;

    foreach (var command in commands)
    {
        if (command.Text.Equals(".reboot", StringComparison.OrdinalIgnoreCase))
        {
            Console.WriteLine("> .reboot");
            cpu.Boot();
            RunForSteps(cpu, bus, 500_000);
            screen = RunUntilReadyPrompt(cpu, bus, editor, maxSteps, ref morePrompts);
            RunForSteps(cpu, bus, 200_000);
            screen = SnapshotScreen(bus.Vgc);
            if (screen.Contains("UNSUPPORTED Z-OPCODE", StringComparison.Ordinal))
                throw new InvalidOperationException($"NovaZ hit an unsupported opcode after reboot.\n{screen}");
            RequireContains(screen, ">");
            RequireStatusLine(screen, cpu, bus);
            if (expectTimeStatus)
                RequireTimeStatusLine(screen);
            RequireReadyPrompt(bus.Vgc, screen);
            continue;
        }

        Console.WriteLine($"> {command.Text}");
        SendLine(cpu, bus, editor, command.Text);
        screen = RunUntilReadyPrompt(cpu, bus, editor, maxSteps, ref morePrompts);
        RunForSteps(cpu, bus, 200_000);
        screen = SnapshotScreen(bus.Vgc);
        if (screen.Contains("UNSUPPORTED Z-OPCODE", StringComparison.Ordinal))
            throw new InvalidOperationException($"NovaZ hit an unsupported opcode after input '{command.Text}'.\n{screen}");
        RequireContains(screen, ">");
        RequireStatusLine(screen, cpu, bus);
        if (expectTimeStatus)
            RequireTimeStatusLine(screen);
        RequireReadyPrompt(bus.Vgc, screen);
        string commandTranscript = ExtractCommandTranscript(screen, command.Text);
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
            RequireContainsEither(bootScreen, $"Revision {release}", $"Release {release}");
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

static int ReadEnvInt(string name, int fallback)
{
    string? value = Environment.GetEnvironmentVariable(name);
    return int.TryParse(value, out int parsed) && parsed > 0 ? parsed : fallback;
}

static List<SmokeCommand> LoadCommands(string[] args, bool bootOnly)
{
    if (bootOnly)
        return [];

    var commands = new List<SmokeCommand>();
    for (int i = 1; i < args.Length; i++)
    {
        switch (args[i])
        {
            case "--generic-boot":
            case "--boot-only":
            case "--expect-more":
            case "--expect-time-status":
            case "--skip-manifest-check":
                break;
            case "--expect-screen":
                if (i + 1 >= args.Length)
                    throw new ArgumentException("--expect-screen requires text.");
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

    string[] expected = parts.Length == 2 && !string.IsNullOrWhiteSpace(parts[1])
        ? parts[1].Split("&&", StringSplitOptions.TrimEntries | StringSplitOptions.RemoveEmptyEntries)
        : [];
    return new SmokeCommand(command, expected);
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

static string RunUntilReadyPrompt(Cpu cpu, CompositeBusDevice bus, ScreenEditor editor, int maxSteps, ref int morePrompts)
{
    const int snapshotMask = 0x3FFF;
    var trace = new Queue<string>();
    int lastOpcodePc = -1;

    for (int i = 0; i < maxSteps; i++)
    {
        int cycles = cpu.ClocksForNext();
        cpu.ExecuteNext();
        bus.AdvanceCycles(cycles);

        int opcodePc = ReadWord(bus, 0x0081, 0x0080);
        if (opcodePc != lastOpcodePc)
        {
            lastOpcodePc = opcodePc;
            trace.Enqueue($"${opcodePc:X4}:${bus.Read(0x0082):X2}");
            while (trace.Count > 24)
                trace.Dequeue();
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
        if (IsReadyPrompt(bus.Vgc, screen))
            return screen;
    }

    string finalScreen = SnapshotScreen(bus.Vgc);
    string recent = string.Join(" ", trace);
    throw new TimeoutException($"Timed out waiting for input prompt. {FormatZvmState(cpu, bus)} recent={recent}\n{finalScreen}");
}

static string FormatZvmState(Cpu cpu, CompositeBusDevice bus)
{
    int globals = ReadWord(bus, 0x0050, 0x0051);
    int opcodePc = ReadWord(bus, 0x0081, 0x0080);
    int global10 = globals > 0 ? ReadXramWord(bus, globals) : 0;
    int global11 = globals > 0 ? ReadXramWord(bus, globals + 2) : 0;
    int global12 = globals > 0 ? ReadXramWord(bus, globals + 4) : 0;
    int global13 = globals > 0 ? ReadXramWord(bus, globals + 6) : 0;
    var state = cpu.GetState();
    return
        $"cpu=PC${cpu.Pc:X4}/A${state.A:X2}/X${state.X:X2}/Y${state.Y:X2} " +
        $"zvm_pc=${bus.Read(0x007F):X2}{bus.Read(0x007E):X2} " +
        $"op_pc=${opcodePc:X4} op_bytes=${ReadXramWord(bus, opcodePc):X4} " +
        $"globals=${globals:X4} g10=${global10:X4} g11=${global11:X4} g12=${global12:X4} g13=${global13:X4} " +
        $"zaddr=${bus.Read(0x0042):X2}{bus.Read(0x0041):X2}{bus.Read(0x0040):X2} " +
        $"xaddr=${bus.Read(0x0022):X2}{bus.Read(0x0021):X2}{bus.Read(0x0020):X2} xdata=${bus.Read(0x0023):X2} " +
        $"zvm_opcode=${bus.Read(0x0082):X2} stop=${bus.Read(0x0300):X2} sp=${bus.Read(0x0301):X2} frames=${bus.Read(0x0302):X2} " +
        $"ops={ReadWord(bus, 0x0307, 0x0303):X4},{ReadWord(bus, 0x0308, 0x0304):X4},{ReadWord(bus, 0x0309, 0x0305):X4},{ReadWord(bus, 0x030A, 0x0306):X4} " +
        $"locals={ReadWord(bus, 0x031B, 0x030B):X4},{ReadWord(bus, 0x031C, 0x030C):X4},{ReadWord(bus, 0x031D, 0x030D):X4},{ReadWord(bus, 0x031E, 0x030E):X4}";
}

static void RunForSteps(Cpu cpu, CompositeBusDevice bus, int steps)
{
    for (int i = 0; i < steps; i++)
    {
        int cycles = cpu.ClocksForNext();
        cpu.ExecuteNext();
        bus.AdvanceCycles(cycles);
    }
}

static void SendLine(Cpu cpu, CompositeBusDevice bus, ScreenEditor editor, string text)
{
    foreach (char ch in text)
    {
        editor.QueueInput((byte)ch);
        RunForSteps(cpu, bus, 8_000);
    }
    editor.QueueInput(0x0D);
    RunForSteps(cpu, bus, 8_000);
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

static void RequireContainsEither(string screen, string expectedA, string expectedB)
{
    if (ContainsNormalized(screen, expectedA) || ContainsNormalized(screen, expectedB))
        return;
    throw new InvalidOperationException(
        $"Expected screen to contain either '{expectedA}' or '{expectedB}'.\n{screen}");
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

static void RequireReadyPrompt(VirtualGraphicsController vgc, string screen)
{
    if (!IsReadyPrompt(vgc, screen))
    {
        throw new InvalidOperationException(
            $"Expected visible cursor on a bare Zork prompt; got {vgc.GetCursorX()},{vgc.GetCursorY()}.\n{screen}");
    }
}

static bool IsReadyPrompt(VirtualGraphicsController vgc, string screen)
{
    string[] lines = screen.Split('\n');
    int actualX = vgc.GetCursorX();
    int actualY = vgc.GetCursorY();
    if (actualY < 0 || actualY >= Math.Min(lines.Length, VgcConstants.ScreenRows))
        return false;
    string line = lines[actualY];
    if (actualX != 1 || line.Length == 0 || line[0] != '>')
        return false;
    return line[1..].All(ch => ch == ' ');
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

sealed record SmokeCommand(string Text, IReadOnlyList<string> Expected);

using System.Text;
using System.Text.Json.Nodes;
using e6502.Avalonia.Hardware;
using e6502.Avalonia.Input;
using e6502.Storage;
using KDS.e6502;

if (args.Length != 1)
{
    Console.Error.WriteLine("usage: Nova.Ozmoo.Smoke <fd0.ndi>");
    return 1;
}

string imagePath = Path.GetFullPath(args[0]);
if (!File.Exists(imagePath))
{
    Console.Error.WriteLine($"image not found: {imagePath}");
    return 1;
}

JsonNode? manifest = TryReadManifest(imagePath);
string storageRoot = Path.Combine(Path.GetTempPath(), $"nova-ozmoo-smoke-{Guid.NewGuid():N}");

try
{
    string disks = Path.Combine(storageRoot, "disks");
    Directory.CreateDirectory(disks);
    File.Copy(imagePath, Path.Combine(disks, "fd0.ndi"));

    Environment.SetEnvironmentVariable("NOVA_STORAGE_ROOT", storageRoot);
    Environment.SetEnvironmentVariable("NOVA_NO_AUTOMOUNT", null);
    Environment.SetEnvironmentVariable("NOAUTO", null);

    using var bus = new CompositeBusDevice(enableSound: false);
    var cpu = new Cpu(bus);
    var editor = new ScreenEditor(bus.Vgc);
    bus.Vgc.SetScreenEditor(editor);
    cpu.Boot();

    int maxSteps = ReadEnvInt("OZMOO_SMOKE_MAX_STEPS", 80_000_000);
    string screen = RunUntilScreenContains(
        cpu,
        bus,
        ">",
        maxSteps);
    RunForSteps(cpu, bus, 200_000);
    screen = SnapshotScreen(bus.Vgc);

    if (screen.Contains("UNSUPPORTED Z-OPCODE", StringComparison.Ordinal))
        throw new InvalidOperationException($"Ozmoo hit an unsupported opcode.\n{screen}");

    RequireContains(screen, "ZORK I: The Great Underground Empire");
    RequireContains(screen, "West of House");
    RequireContains(screen, "There is a small mailbox here.");
    RequireContains(screen, ">");
    RequireStatusLine(screen, cpu, bus);
    RequireCursorAfterLastTextLine(bus.Vgc, screen);

    SendLine(cpu, bus, editor, "look");
    screen = RunUntilPromptCount(cpu, bus, 2, maxSteps);
    RunForSteps(cpu, bus, 200_000);
    screen = SnapshotScreen(bus.Vgc);
    if (screen.Contains("UNSUPPORTED Z-OPCODE", StringComparison.Ordinal))
        throw new InvalidOperationException($"Ozmoo hit an unsupported opcode after input.\n{screen}");
    RequireContains(screen, "There is a small mailbox here.");
    RequireContains(screen, ">");
    RequireStatusLine(screen, cpu, bus);

    if (manifest is not null)
    {
        int? version = manifest["Version"]?.GetValue<int>();
        int? release = manifest["Release"]?.GetValue<int>();
        string? serial = manifest["Serial"]?.GetValue<string>();

        if (version is not null)
            RequireContains(screen, "ZORK I:");
        if (release is not null)
            RequireContains(screen, $"Revision {release}");
        if (!string.IsNullOrWhiteSpace(serial))
            RequireContains(screen, $"Serial number {serial}");
    }

    Console.WriteLine("Nova Ozmoo smoke passed.");
    Console.WriteLine(SnapshotScreen(bus.Vgc));
    return 0;
}
finally
{
    Environment.SetEnvironmentVariable("NOAUTO", null);
    Environment.SetEnvironmentVariable("NOVA_NO_AUTOMOUNT", null);
    Environment.SetEnvironmentVariable("NOVA_STORAGE_ROOT", null);

    if (Directory.Exists(storageRoot))
        Directory.Delete(storageRoot, recursive: true);
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

static string RunUntilScreenContains(Cpu cpu, CompositeBusDevice bus, string marker, int maxSteps)
{
    const int snapshotMask = 0x3FFF;

    for (int i = 0; i < maxSteps; i++)
    {
        int cycles = cpu.ClocksForNext();
        cpu.ExecuteNext();
        bus.AdvanceCycles(cycles);

        if ((i & snapshotMask) != 0)
            continue;

        string screen = SnapshotScreen(bus.Vgc);
        if (screen.Contains(marker, StringComparison.Ordinal))
            return screen;
    }

    string finalScreen = SnapshotScreen(bus.Vgc);
    int globals = ReadWord(bus, 0x0050, 0x0051);
    int opcodePc = ReadWord(bus, 0x0081, 0x0080);
    var finalState = cpu.GetState();
    string zvmState =
        $"cpu=A${finalState.A:X2}/X${finalState.X:X2}/Y${finalState.Y:X2} " +
        $"zvm_pc=${bus.Read(0x007F):X2}{bus.Read(0x007E):X2} " +
        $"op_pc=${opcodePc:X4} op_bytes=${ReadXramWord(bus, opcodePc):X4} " +
        $"globals=${globals:X4} g92=${ReadXramWord(bus, globals + ((0x92 - 0x10) * 2)):X4} g94=${ReadXramWord(bus, globals + ((0x94 - 0x10) * 2)):X4} " +
        $"zaddr=${bus.Read(0x0042):X2}{bus.Read(0x0041):X2}{bus.Read(0x0040):X2} " +
        $"xaddr=${bus.Read(0x0022):X2}{bus.Read(0x0021):X2}{bus.Read(0x0020):X2} xdata=${bus.Read(0x0023):X2} " +
        $"zvm_opcode=${bus.Read(0x0082):X2} " +
        $"stop=${bus.Read(0x0300):X2} sp=${bus.Read(0x0301):X2} frames=${bus.Read(0x0302):X2} " +
        $"ops={ReadWord(bus, 0x0307, 0x0303):X4},{ReadWord(bus, 0x0308, 0x0304):X4},{ReadWord(bus, 0x0309, 0x0305):X4} " +
        $"locals={ReadWord(bus, 0x031B, 0x030B):X4},{ReadWord(bus, 0x031C, 0x030C):X4},{ReadWord(bus, 0x031D, 0x030D):X4},{ReadWord(bus, 0x031E, 0x030E):X4},{ReadWord(bus, 0x031F, 0x030F):X4}";
    throw new TimeoutException($"Timed out waiting for '{marker}' at PC=${cpu.Pc:X4} {zvmState}.\n{finalScreen}");
}

static string RunUntilPromptCount(Cpu cpu, CompositeBusDevice bus, int minPromptCount, int maxSteps)
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
        if (CountPromptLines(screen) >= minPromptCount)
            return screen;
    }

    string finalScreen = SnapshotScreen(bus.Vgc);
    string recent = string.Join(" ", trace);
    throw new TimeoutException($"Timed out waiting for {minPromptCount} prompts. {FormatZvmState(cpu, bus)} recent={recent}\n{finalScreen}");
}

static int CountPromptLines(string text)
{
    int count = 0;
    foreach (string line in text.Split('\n'))
    {
        if (line.TrimStart().StartsWith('>'))
            count++;
    }
    return count;
}

static string FormatZvmState(Cpu cpu, CompositeBusDevice bus)
{
    int globals = ReadWord(bus, 0x0050, 0x0051);
    int opcodePc = ReadWord(bus, 0x0081, 0x0080);
    var state = cpu.GetState();
    return
        $"cpu=PC${cpu.Pc:X4}/A${state.A:X2}/X${state.X:X2}/Y${state.Y:X2} " +
        $"zvm_pc=${bus.Read(0x007F):X2}{bus.Read(0x007E):X2} " +
        $"op_pc=${opcodePc:X4} op_bytes=${ReadXramWord(bus, opcodePc):X4} " +
        $"globals=${globals:X4} " +
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

static void RequireContains(string screen, string expected)
{
    if (!screen.Contains(expected, StringComparison.Ordinal))
        throw new InvalidOperationException($"Expected screen to contain '{expected}'.\n{screen}");
}

static void RequireStatusLine(string screen, Cpu cpu, CompositeBusDevice bus)
{
    string firstLine = screen.Split('\n')[0];
    if (!firstLine.Contains("West of House", StringComparison.Ordinal) ||
        !firstLine.Contains("Score:", StringComparison.Ordinal) ||
        !firstLine.Contains("Moves:", StringComparison.Ordinal))
    {
        int globals = ReadWord(bus, 0x0050, 0x0051);
        int objectTable = ReadWord(bus, 0x004E, 0x004F);
        int room = ReadXramWord(bus, globals) & 0xFF;
        int entry = objectTable + 62 + ((room - 1) * 9);
        int propTable = room > 0 ? ReadXramWord(bus, entry + 7) : 0;
        int nameWords = propTable > 0 ? ReadXramByte(bus, propTable) : 0;
        string nameBytes = propTable > 0 ? ReadXramBytes(bus, propTable, 12) : "";
        throw new InvalidOperationException(
            $"Expected status line to show location, score, and moves. {FormatZvmState(cpu, bus)} " +
            $"room={room} objEntry=${entry:X4} prop=${propTable:X4} nameWords={nameWords} nameBytes={nameBytes}\n{screen}");
    }
}

static void RequireCursorAfterLastTextLine(VirtualGraphicsController vgc, string screen)
{
    string[] lines = screen.Split('\n');
    int rows = Math.Min(lines.Length, VgcConstants.ScreenRows);
    for (int row = 0; row < rows; row++)
    {
        string trimmed = lines[row].TrimStart();
        int leadingSpaces = lines[row].Length - trimmed.Length;
        int promptColumn = trimmed.StartsWith('>') ? leadingSpaces : -1;
        if (promptColumn >= 0)
        {
            int promptActualX = vgc.GetCursorX();
            int promptActualY = vgc.GetCursorY();
            int expectedX = Math.Min(promptColumn + 1, VgcConstants.ScreenCols - 1);
            if (promptActualX != expectedX || promptActualY != row)
            {
                throw new InvalidOperationException(
                    $"Expected visible cursor after prompt at {expectedX},{row}; got {promptActualX},{promptActualY}.\n{screen}");
            }
            return;
        }
    }

    int lastTextRow = -1;
    for (int row = 0; row < rows; row++)
    {
        if (!string.IsNullOrWhiteSpace(lines[row]))
            lastTextRow = row;
    }

    if (lastTextRow < 0)
        throw new InvalidOperationException("Expected Ozmoo smoke screen to contain text.");

    int expectedY = Math.Min(lastTextRow + 1, VgcConstants.ScreenRows - 1);
    int actualX = vgc.GetCursorX();
    int actualY = vgc.GetCursorY();
    if (actualX != 0 || actualY != expectedY)
    {
        throw new InvalidOperationException(
            $"Expected visible cursor at 0,{expectedY} after Ozmoo text output; got {actualX},{actualY}.\n{screen}");
    }
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

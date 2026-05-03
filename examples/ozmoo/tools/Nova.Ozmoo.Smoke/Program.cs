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
        "NOVA Z-MEMORY PLATFORM READY.",
        maxSteps);

    RequireContains(screen, "NOVA OZMOO RUNTIME");
    RequireContains(screen, "STORY LOADED.");
    RequireContains(screen, "NOVA Z-MEMORY PLATFORM READY.");

    if (manifest is not null)
    {
        int? version = manifest["Version"]?.GetValue<int>();
        int? release = manifest["Release"]?.GetValue<int>();
        string? serial = manifest["Serial"]?.GetValue<string>();

        if (version is not null)
            RequireContains(screen, $"Z-MACHINE VERSION: {version}");
        if (release is not null)
            RequireContains(screen, $"RELEASE: {release}");
        if (!string.IsNullOrWhiteSpace(serial))
            RequireContains(screen, $"SERIAL: {serial}");
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
    bool trace = Environment.GetEnvironmentVariable("OZMOO_SMOKE_TRACE") == "1";
    const int snapshotMask = 0x3FFF;
    const int traceEvery = 5_000_000;

    for (int i = 0; i < maxSteps; i++)
    {
        int cycles = cpu.ClocksForNext();
        cpu.ExecuteNext();
        bus.AdvanceCycles(cycles);

        if (trace && i > 0 && i % traceEvery == 0)
            Console.Error.WriteLine($"steps={i} pc=${cpu.Pc:X4}");

        if ((i & snapshotMask) != 0)
            continue;

        string screen = SnapshotScreen(bus.Vgc);
        if (screen.Contains(marker, StringComparison.Ordinal))
            return screen;
    }

    string finalScreen = SnapshotScreen(bus.Vgc);
    throw new TimeoutException($"Timed out waiting for '{marker}' at PC=${cpu.Pc:X4}.\n{finalScreen}");
}

static void RequireContains(string screen, string expected)
{
    if (!screen.Contains(expected, StringComparison.Ordinal))
        throw new InvalidOperationException($"Expected screen to contain '{expected}'.\n{screen}");
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

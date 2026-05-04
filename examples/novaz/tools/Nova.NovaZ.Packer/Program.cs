using System.Text;
using System.Text.Json;
using e6502.Storage;

var options = Options.Parse(args);
if (options is null)
{
    Options.PrintUsage();
    return 1;
}

if (!File.Exists(options.AutobootPath))
{
    Console.Error.WriteLine($"AUTOBOOT binary not found: {options.AutobootPath}");
    return 1;
}

if (!File.Exists(options.RuntimePath))
{
    Console.Error.WriteLine($"Runtime ROM not found: {options.RuntimePath}");
    return 1;
}

if (options.StoryPath is not null && !File.Exists(options.StoryPath))
{
    Console.Error.WriteLine($"Story file not found: {options.StoryPath}");
    return 1;
}

var outputDir = Path.GetDirectoryName(Path.GetFullPath(options.OutputPath));
if (!string.IsNullOrEmpty(outputDir))
    Directory.CreateDirectory(outputDir);

NdiImage.CreateFormatted(options.OutputPath, options.Label, options.SizeKb);
using var image = NdiImage.Open(options.OutputPath);

var autoboot = File.ReadAllBytes(options.AutobootPath);
image.WriteFile("AUTOBOOT.bin", NdiFileType.Bin, 0xFFFF, autoboot);
Console.WriteLine($"Imported AUTOBOOT.bin ({autoboot.Length} bytes)");

var runtime = File.ReadAllBytes(options.RuntimePath);
if (runtime.Length != 16 * 1024)
{
    Console.Error.WriteLine($"Runtime ROM must be exactly 16384 bytes: {options.RuntimePath} is {runtime.Length}");
    return 1;
}
image.WriteFile(options.RuntimeName, NdiFileType.Bin, 0xFFFF, runtime);
Console.WriteLine($"Imported {options.RuntimeName} ({runtime.Length} bytes)");

if (options.StoryPath is not null)
{
    var story = File.ReadAllBytes(options.StoryPath);
    var storyName = options.StoryName;
    image.WriteFile(storyName, NdiFileType.Bin, 0xFFFF, story);
    Console.WriteLine($"Imported {storyName} ({story.Length} bytes)");

    var manifest = StoryManifest.FromStory(story, storyName);
    var json = JsonSerializer.Serialize(manifest, new JsonSerializerOptions
    {
        WriteIndented = true
    });
    image.WriteFile("STORY.MANIFEST", NdiFileType.Bin, 0xFFFF, Encoding.ASCII.GetBytes(json + "\n"));
    Console.WriteLine("Imported STORY.MANIFEST");
}

Console.WriteLine($"Wrote {options.OutputPath}");
return 0;

internal sealed record Options(
    string OutputPath,
    string AutobootPath,
    string RuntimePath,
    string RuntimeName,
    string? StoryPath,
    string StoryName,
    string Label,
    int SizeKb)
{
    public static Options? Parse(string[] args)
    {
        string output = "dist/fd0.ndi";
        string autoboot = "build/AUTOBOOT.bin";
        string runtime = "build/novaz.bin";
        string runtimeName = "novaz.bin";
        string? story = null;
        string storyName = "story.bin";
        string label = "NOVAZ";
        int sizeKb = 1440;

        for (int i = 0; i < args.Length; i++)
        {
            string arg = args[i];
            string? value = i + 1 < args.Length ? args[i + 1] : null;
            switch (arg)
            {
                case "--output" when value is not null:
                    output = value;
                    i++;
                    break;
                case "--autoboot" when value is not null:
                    autoboot = value;
                    i++;
                    break;
                case "--runtime" when value is not null:
                    runtime = value;
                    i++;
                    break;
                case "--runtime-name" when value is not null:
                    runtimeName = value;
                    i++;
                    break;
                case "--story" when value is not null:
                    story = value;
                    i++;
                    break;
                case "--story-name" when value is not null:
                    storyName = value;
                    i++;
                    break;
                case "--label" when value is not null:
                    label = value;
                    i++;
                    break;
                case "--size-kb" when value is not null && int.TryParse(value, out var parsed):
                    sizeKb = parsed;
                    i++;
                    break;
                case "-h" or "--help":
                    return null;
                default:
                    Console.Error.WriteLine($"Unknown or incomplete argument: {arg}");
                    return null;
            }
        }

        if (string.IsNullOrWhiteSpace(output) ||
            string.IsNullOrWhiteSpace(autoboot) ||
            string.IsNullOrWhiteSpace(runtime))
            return null;
        if (string.IsNullOrWhiteSpace(runtimeName) || runtimeName.Length > 32)
            return null;
        if (string.IsNullOrWhiteSpace(storyName) || storyName.Length > 32)
            return null;
        if (sizeKb <= 0)
            return null;

        return new Options(output, autoboot, runtime, runtimeName, story, storyName, label, sizeKb);
    }

    public static void PrintUsage()
    {
        Console.Error.WriteLine("Nova.NovaZ.Packer");
        Console.Error.WriteLine("  --output <fd0.ndi> --autoboot <AUTOBOOT.bin> --runtime <novaz.bin> [--runtime-name novaz.bin] [--story <story-file>] [--story-name story.bin] [--label NOVAZ] [--size-kb 1440]");
    }
}

internal sealed record StoryManifest(
    string FileName,
    int ActualSize,
    int Version,
    int Release,
    string Serial,
    int Checksum,
    int HeaderFileLength,
    int HighMemory,
    int InitialPc,
    int Dictionary,
    int ObjectTable,
    int Globals,
    int StaticMemory,
    int Abbreviations)
{
    public static StoryManifest FromStory(byte[] story, string fileName)
    {
        if (story.Length < 64)
            throw new InvalidDataException("Story file is too short for a Z-machine header.");

        int version = story[0];
        int packedLength = ReadBE16(story, 26);
        int multiplier = version switch
        {
            <= 3 => 2,
            <= 5 => 4,
            _ => 8
        };

        return new StoryManifest(
            fileName,
            story.Length,
            version,
            ReadBE16(story, 2),
            Encoding.ASCII.GetString(story, 18, 6),
            ReadBE16(story, 28),
            packedLength * multiplier,
            ReadBE16(story, 4),
            ReadBE16(story, 6),
            ReadBE16(story, 8),
            ReadBE16(story, 10),
            ReadBE16(story, 12),
            ReadBE16(story, 14),
            ReadBE16(story, 24));
    }

    private static int ReadBE16(byte[] data, int offset) => (data[offset] << 8) | data[offset + 1];
}

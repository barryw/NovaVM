// =====================================================================
//  NovaZ — Program.cs (Nova.NovaZ.Packer)
//
//  Command-line entry point for the NovaZ packer: assembles the AUTOBOOT
//  binary, runtime ROM, story file, and packed picture/sound resources
//  into a mountable Nova disk (.ndi) image.
//
//  Copyright (C) 2026 Barry Walker
//  SPDX-License-Identifier: MIT
// =====================================================================

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

foreach (var asset in options.Assets)
{
    if (!File.Exists(asset.Path))
    {
        Console.Error.WriteLine($"Asset file not found: {asset.Path}");
        return 1;
    }
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

// Assets keep their relative path as REAL .ndi subdirectories — e.g.
// forth/lib/core.4th becomes forth/ -> lib/ -> core.4th, navigable with CD/DIR.
// .4th files are stored as the FORTH type. Nothing is hidden or locked: every
// entry (RUNTIME.BIN, the forth tree, ...) shows in DIR and can be deleted.
var dirCache = new Dictionary<string, ushort> { [""] = 0xFFFF };
ushort EnsureDir(string dirPath)
{
    if (dirCache.TryGetValue(dirPath, out var cached)) return cached;
    int s = dirPath.LastIndexOf('/');
    ushort parent = EnsureDir(s < 0 ? "" : dirPath[..s]);
    var idx = (ushort)image.MakeDirectory(s < 0 ? dirPath : dirPath[(s + 1)..], parent);
    dirCache[dirPath] = idx;
    Console.WriteLine($"  mkdir {dirPath}/");
    return idx;
}

foreach (var asset in options.Assets)
{
    var data = File.ReadAllBytes(asset.Path);
    var name = asset.Name.Replace('\\', '/');
    int slash = name.LastIndexOf('/');
    ushort parent = slash < 0 ? (ushort)0xFFFF : EnsureDir(name[..slash]);
    string fname = slash < 0 ? name : name[(slash + 1)..];
    var type = Path.GetExtension(fname).ToLowerInvariant() switch
    {
        ".4th" or ".fth" or ".fs" => NdiFileType.Forth,
        ".pas" => NdiFileType.Pascal,
        ".npp" => NdiFileType.PascalProject,
        ".s" or ".asm" or ".inc" => NdiFileType.Assembly,
        _ => NdiFileType.Bin
    };
    image.WriteFile(fname, type, parent, data);
    Console.WriteLine($"Imported {name} ({data.Length} bytes, {type})");
}

if (options.SoundsPath is not null)
{
    var pack = BlorbSounds.BuildPack(options.SoundsPath);
    image.WriteFile("SOUND.PAK", NdiFileType.Bin, 0xFFFF, pack);
    Console.WriteLine($"Imported SOUND.PAK ({pack.Length} bytes, {BlorbSounds.Summary(pack)})");

    // Hardware path: the same samples as a WTS soundfont NovaHost plays via the
    // FPGA WTS chip (Avalonia uses SOUND.PAK; the FPGA has no PCM mixer of its own).
    var sf = BlorbSounds.BuildSoundfont(options.SoundsPath);
    image.WriteFile("ZSOUND.NSF", NdiFileType.Bin, 0xFFFF, sf);
    Console.WriteLine($"Imported ZSOUND.NSF ({sf.Length} bytes, {BlorbSounds.SoundfontSummary(sf)})");
    if (options.SoundfontOutPath is not null)
    {
        File.WriteAllBytes(options.SoundfontOutPath, sf);
        Console.WriteLine($"Wrote {options.SoundfontOutPath} ({sf.Length} bytes)");
    }
}

if (options.PicturesPath is not null)
{
    var pics = BlorbPictures.BuildPack(options.PicturesPath);
    image.WriteFile("PICS.PAK", NdiFileType.Bin, 0xFFFF, pics);
    Console.WriteLine($"Imported PICS.PAK ({pics.Length} bytes, {BlorbPictures.Summary(pics)})");
}

Console.WriteLine($"Wrote {options.OutputPath}");
return 0;

internal sealed record AssetOption(string Path, string Name);

internal sealed record Options(
    string OutputPath,
    string AutobootPath,
    string RuntimePath,
    string RuntimeName,
    string? StoryPath,
    string StoryName,
    string Label,
    int SizeKb,
    IReadOnlyList<AssetOption> Assets,
    string? SoundsPath,
    string? SoundfontOutPath,
    string? PicturesPath)
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
        var assets = new List<AssetOption>();
        string? sounds = null;
        string? soundfontOut = null;
        string? pictures = null;

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
                case "--asset" when value is not null:
                    if (!TryParseAsset(value, out var asset))
                    {
                        Console.Error.WriteLine($"Invalid asset argument: {value}");
                        return null;
                    }
                    assets.Add(asset);
                    i++;
                    break;
                case "--sounds" when value is not null:
                    sounds = value;
                    i++;
                    break;
                case "--soundfont-out" when value is not null:
                    soundfontOut = value;
                    i++;
                    break;
                case "--pictures" when value is not null:
                    pictures = value;
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
        if (assets.Any(a => string.IsNullOrWhiteSpace(a.Name) || a.Name.Length > 32))
            return null;
        if (sizeKb <= 0)
            return null;

        if (sounds is not null && !File.Exists(sounds))
        {
            Console.Error.WriteLine($"Sounds file not found: {sounds}");
            return null;
        }

        if (pictures is not null && !File.Exists(pictures))
        {
            Console.Error.WriteLine($"Pictures file not found: {pictures}");
            return null;
        }

        return new Options(output, autoboot, runtime, runtimeName, story, storyName, label, sizeKb, assets, sounds, soundfontOut, pictures);
    }

    private static bool TryParseAsset(string value, out AssetOption asset)
    {
        asset = new AssetOption(string.Empty, string.Empty);
        int separator = value.LastIndexOf(':');
        if (separator <= 0 || separator == value.Length - 1)
            return false;

        string path = value[..separator];
        string name = value[(separator + 1)..];
        if (string.IsNullOrWhiteSpace(path) || string.IsNullOrWhiteSpace(name))
            return false;

        asset = new AssetOption(path, name);
        return true;
    }

    public static void PrintUsage()
    {
        Console.Error.WriteLine("Nova.NovaZ.Packer");
        Console.Error.WriteLine("  --output <fd0.ndi> --autoboot <AUTOBOOT.bin> --runtime <novaz.bin> [--runtime-name novaz.bin] [--story <story-file>] [--story-name story.bin] [--asset <path:image-name>] [--sounds <file.blb>] [--pictures <file.blb>] [--label NOVAZ] [--size-kb 1440]");
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

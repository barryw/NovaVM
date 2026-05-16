using e6502.Avalonia.Hardware;
using e6502.Tools;
using Melanchall.DryWetMidi.Core;

if (args.Length < 1)
{
    PrintUsage();
    return 1;
}

// Verb-based routing
if (args[0] == "mid2bas")
    return RunMidToBas(args.Skip(1).ToArray());
if (args[0] == "pack-overlay")
    return RunPackOverlay(args.Skip(1).ToArray());
if (args[0] == "sidtrace")
    return SidTraceCommand.Run(args.Skip(1).ToArray());

// Legacy: sidreloc (no verb prefix)
return RunSidReloc(args);

static int RunPackOverlay(string[] args)
{
    if (args.Length < 1)
    {
        PrintPackOverlayUsage();
        return 1;
    }

    string? inputPath = null;
    string? outputPath = null;
    string? symbolPath = null;
    ushort? loadAddress = null;
    ushort? maxSize = null;
    ushort? initEntry = null;
    ushort? mainEntry = null;
    ushort? tickEntry = null;
    ushort? unloadEntry = null;
    string? initSymbol = null;
    string? mainSymbol = null;
    string? tickSymbol = null;
    string? unloadSymbol = null;
    ushort bssSize = 0;
    ushort moduleId = 0;
    ushort moduleVersion = 0;
    byte flags = 0;

    try
    {
        for (int i = 0; i < args.Length; i++)
        {
            switch (args[i])
            {
                case "-i" or "--input" when i + 1 < args.Length:
                    inputPath = args[++i];
                    break;
                case "-o" or "--output" when i + 1 < args.Length:
                    outputPath = args[++i];
                    break;
                case "--sym" when i + 1 < args.Length:
                    symbolPath = args[++i];
                    break;
                case "--load" when i + 1 < args.Length:
                    loadAddress = OverlayPackager.ParseU16(args[++i], "--load");
                    break;
                case "--max-size" when i + 1 < args.Length:
                    maxSize = OverlayPackager.ParseU16(args[++i], "--max-size");
                    break;
                case "--bss-size" when i + 1 < args.Length:
                    bssSize = OverlayPackager.ParseU16(args[++i], "--bss-size");
                    break;
                case "--module-id" when i + 1 < args.Length:
                    moduleId = OverlayPackager.ParseU16(args[++i], "--module-id");
                    break;
                case "--module-version" when i + 1 < args.Length:
                    moduleVersion = OverlayPackager.ParseU16(args[++i], "--module-version");
                    break;
                case "--flags" when i + 1 < args.Length:
                    flags = checked((byte)OverlayPackager.ParseU16(args[++i], "--flags"));
                    break;
                case "--init" when i + 1 < args.Length:
                    (initEntry, initSymbol) = ParseEntryArg(args[++i], "--init");
                    break;
                case "--main" when i + 1 < args.Length:
                    (mainEntry, mainSymbol) = ParseEntryArg(args[++i], "--main");
                    break;
                case "--tick" when i + 1 < args.Length:
                    (tickEntry, tickSymbol) = ParseEntryArg(args[++i], "--tick");
                    break;
                case "--unload" when i + 1 < args.Length:
                    (unloadEntry, unloadSymbol) = ParseEntryArg(args[++i], "--unload");
                    break;
                case "-h" or "--help":
                    PrintPackOverlayUsage();
                    return 0;
                default:
                    Console.Error.WriteLine($"Unknown or incomplete option: {args[i]}");
                    PrintPackOverlayUsage();
                    return 1;
            }
        }

        if (inputPath is null || outputPath is null || loadAddress is null)
        {
            PrintPackOverlayUsage();
            return 1;
        }

        var result = OverlayPackager.Pack(new OverlayPackageOptions
        {
            InputPath = inputPath,
            OutputPath = outputPath,
            SymbolPath = symbolPath,
            LoadAddress = loadAddress.Value,
            MaxSize = maxSize,
            InitEntry = initEntry,
            MainEntry = mainEntry,
            TickEntry = tickEntry,
            UnloadEntry = unloadEntry,
            InitSymbol = initSymbol,
            MainSymbol = mainSymbol,
            TickSymbol = tickSymbol,
            UnloadSymbol = unloadSymbol,
            BssSize = bssSize,
            ModuleId = moduleId,
            ModuleVersion = moduleVersion,
            Flags = flags,
        });

        Console.WriteLine(
            $"Packed {outputPath}: load=${result.LoadAddress:X4}, payload={result.PayloadSize}, " +
            $"init=${result.InitEntry:X4}, main=${result.MainEntry:X4}, " +
            $"tick=${result.TickEntry:X4}, unload=${result.UnloadEntry:X4}, checksum=${result.Checksum:X4}");
        return 0;
    }
    catch (Exception ex) when (ex is IOException or UnauthorizedAccessException or ArgumentException or InvalidOperationException or OverflowException)
    {
        Console.Error.WriteLine($"pack-overlay failed: {ex.Message}");
        return 1;
    }
}

static (ushort? Address, string? Symbol) ParseEntryArg(string value, string optionName)
{
    string trimmed = value.Trim();
    if (trimmed.StartsWith('$') ||
        trimmed.StartsWith("0x", StringComparison.OrdinalIgnoreCase) ||
        trimmed.All(char.IsDigit))
    {
        return (OverlayPackager.ParseU16(trimmed, optionName), null);
    }

    return (null, trimmed);
}

static int RunMidToBas(string[] args)
{
    if (args.Length < 1)
    {
        Console.Error.WriteLine("Usage: mid2bas <input.mid> [-o output.bas] [--mml-only] [--wts] [--title TITLE] [--subtitle SUB] [--voices 1=3,2=5] [--max-line-len 200] [--max-voices N]");
        return 1;
    }

    string inputPath = args[0];
    if (!File.Exists(inputPath))
    {
        Console.Error.WriteLine($"File not found: {inputPath}");
        return 1;
    }

    string? outputPath = null;
    string title = "";
    string subtitle = "";
    bool mmlOnly = false;
    bool useWts = false;
    bool compact = false;
    int maxLineLen = 200;
    int maxVoices = 6;
    Dictionary<int, int>? mapping = null;

    for (int i = 1; i < args.Length; i++)
    {
        switch (args[i])
        {
            case "-o" or "--output" when i + 1 < args.Length:
                outputPath = args[++i];
                break;
            case "--title" when i + 1 < args.Length:
                title = args[++i];
                break;
            case "--subtitle" when i + 1 < args.Length:
                subtitle = args[++i];
                break;
            case "--mml-only":
                mmlOnly = true;
                break;
            case "--wts":
                useWts = true;
                break;
            case "--compact":
                compact = true;
                break;
            case "--max-line-len" when i + 1 < args.Length:
                maxLineLen = int.Parse(args[++i]);
                break;
            case "--max-voices" when i + 1 < args.Length:
                maxVoices = int.Parse(args[++i]);
                break;
            case "--voices" when i + 1 < args.Length:
                mapping = ParseVoiceMapping(args[++i]);
                break;
        }
    }

    MidiFile midi;
    try { midi = MidiFile.Read(inputPath); }
    catch (Exception ex)
    {
        Console.Error.WriteLine($"Failed to read MIDI file: {ex.Message}");
        return 1;
    }

    if (mmlOnly)
    {
        int ppqn = ((TicksPerQuarterNoteTimeDivision)midi.TimeDivision).TicksPerQuarterNote;
        var effectiveMaxVoices = useWts ? Math.Max(maxVoices, 14) : maxVoices;
        var channels = MidiEngine.SelectChannels(midi, effectiveMaxVoices, mapping);
        for (int v = 0; v < channels.Length; v++)
        {
            string mml = MidiEngine.GenerateMml(midi, channels[v], ppqn);
            Console.WriteLine($"Voice {v + 1} (ch {channels[v]}): {mml}");
        }
        return 0;
    }

    string bas = MidiEngine.GenerateBasProgram(midi, title, subtitle, maxLineLen,
        maxVoices, mapping, useWts, compact);

    if (outputPath is null)
        outputPath = Path.ChangeExtension(inputPath, ".bas");

    File.WriteAllText(outputPath, bas);
    Console.WriteLine($"Wrote {outputPath}");
    return 0;
}

static Dictionary<int, int> ParseVoiceMapping(string s)
{
    var map = new Dictionary<int, int>();
    foreach (var pair in s.Split(','))
    {
        var parts = pair.Split('=');
        if (parts.Length == 2 && int.TryParse(parts[0], out int voice) && int.TryParse(parts[1], out int channel))
            map[voice] = channel;
    }
    return map;
}

static void WriteSidFile(string path, SidFileInfo info)
{
    byte[] header = new byte[124];
    header[0] = (byte)'P'; header[1] = (byte)'S'; header[2] = (byte)'I'; header[3] = (byte)'D';
    WriteBE16(header, 4, info.Version);
    WriteBE16(header, 6, 124);
    WriteBE16(header, 8, info.LoadAddress);
    WriteBE16(header, 10, info.InitAddress);
    WriteBE16(header, 12, info.PlayAddress);
    WriteBE16(header, 14, info.Songs);
    WriteBE16(header, 16, info.StartSong);
    header[18] = (byte)(info.Speed >> 24);
    header[19] = (byte)(info.Speed >> 16);
    header[20] = (byte)(info.Speed >> 8);
    header[21] = (byte)info.Speed;
    WriteString(header, 22, info.Title, 32);
    WriteString(header, 54, info.Author, 32);
    WriteString(header, 86, info.Copyright, 32);

    using var fs = File.Create(path);
    fs.Write(header);
    fs.Write(info.Payload);
}

static void WriteBE16(byte[] buf, int offset, int value)
{
    buf[offset] = (byte)(value >> 8);
    buf[offset + 1] = (byte)(value & 0xFF);
}

static void WriteString(byte[] buf, int offset, string s, int maxLen)
{
    int len = Math.Min(s.Length, maxLen);
    for (int i = 0; i < len; i++)
        buf[offset + i] = (byte)s[i];
}

static void PrintUsage()
{
    Console.Error.WriteLine("e6502 Tools");
    Console.Error.WriteLine("  mid2bas <input.mid> [-o output.bas] [--mml-only] [--wts] [--title T] [--subtitle S] [--voices 1=3,2=5] [--max-voices N]");
    Console.Error.WriteLine("  pack-overlay --input payload.bin --output module.ovl --load $7000 [--sym payload.sym] [--main symbol]");
    Console.Error.WriteLine("  <input.sid> [output.sid] --target 0x1000   (SID relocator)");
    Console.Error.WriteLine("  <input.sid> --info                         (SID info)");
}

static void PrintPackOverlayUsage()
{
    Console.Error.WriteLine("Usage: pack-overlay --input payload.bin --output module.ovl --load $7000 [options]");
    Console.Error.WriteLine("Options:");
    Console.Error.WriteLine("  --sym file.sym             ld65 -Ln symbol file for entry names");
    Console.Error.WriteLine("  --max-size $2000           maximum overlay slot size");
    Console.Error.WriteLine("  --init symbol|$addr        optional init entry");
    Console.Error.WriteLine("  --main symbol|$addr        optional main entry");
    Console.Error.WriteLine("  --tick symbol|$addr        optional tick entry");
    Console.Error.WriteLine("  --unload symbol|$addr      optional unload entry");
    Console.Error.WriteLine("  --bss-size n               informational BSS size");
    Console.Error.WriteLine("  --module-id n              product-defined module id");
    Console.Error.WriteLine("  --module-version n         product-defined module version");
    Console.Error.WriteLine("  --flags n                  reserved overlay flags byte");
}

static int RunSidReloc(string[] args)
{
    string inputPath = args[0];
    if (!File.Exists(inputPath))
    {
        Console.Error.WriteLine($"File not found: {inputPath}");
        return 1;
    }

    byte[] data = File.ReadAllBytes(inputPath);
    var info = SidFileParser.Parse(data);
    if (!info.IsValid)
    {
        Console.Error.WriteLine("Invalid SID file");
        return 1;
    }

    if (args.Contains("--info"))
    {
        Console.WriteLine($"Title:     {info.Title}");
        Console.WriteLine($"Author:    {info.Author}");
        Console.WriteLine($"Copyright: {info.Copyright}");
        Console.WriteLine($"Load:      ${info.LoadAddress:X4}");
        Console.WriteLine($"Init:      ${info.InitAddress:X4}");
        Console.WriteLine($"Play:      ${info.PlayAddress:X4}");
        Console.WriteLine($"Songs:     {info.Songs}");
        Console.WriteLine($"Speed:     {(info.UsesCiaTiming ? "CIA" : "VBI")}");
        Console.WriteLine($"Size:      {info.Payload.Length} bytes");
        return 0;
    }

    int targetIdx = Array.IndexOf(args, "--target");
    if (targetIdx < 0 || targetIdx + 1 >= args.Length)
    {
        Console.Error.WriteLine("Missing --target address");
        return 1;
    }

    if (!ushort.TryParse(args[targetIdx + 1].Replace("0x", "").Replace("$", ""),
        System.Globalization.NumberStyles.HexNumber, null, out ushort target))
    {
        Console.Error.WriteLine($"Invalid target address: {args[targetIdx + 1]}");
        return 1;
    }

    string outputPath = args.Length > 1 && !args[1].StartsWith("--") ? args[1] : inputPath;

    var relocated = e6502.Tools.SidRelocator.Relocate(info, target);
    if (!relocated.IsValid)
    {
        Console.Error.WriteLine("Relocation failed");
        return 1;
    }

    WriteSidFile(outputPath, relocated);
    Console.WriteLine($"Relocated ${info.LoadAddress:X4} -> ${target:X4}, wrote {outputPath}");
    return 0;
}

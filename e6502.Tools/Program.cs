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

// Legacy: sidreloc (no verb prefix)
return RunSidReloc(args);

static int RunMidToBas(string[] args)
{
    if (args.Length < 1)
    {
        Console.Error.WriteLine("Usage: mid2bas <input.mid> [-o output.bas] [--mml-only] [--title TITLE] [--subtitle SUB] [--voices 1=3,2=5] [--max-line-len 200]");
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
    int maxLineLen = 200;
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
            case "--max-line-len" when i + 1 < args.Length:
                maxLineLen = int.Parse(args[++i]);
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
        var channels = MidiEngine.SelectChannels(midi, explicitMapping: mapping);
        for (int v = 0; v < channels.Length; v++)
        {
            string mml = MidiEngine.GenerateMml(midi, channels[v], ppqn);
            Console.WriteLine($"Voice {v + 1} (ch {channels[v]}): {mml}");
        }
        return 0;
    }

    string bas = MidiEngine.GenerateBasProgram(midi, title, subtitle, maxLineLen,
        explicitMapping: mapping);

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
    Console.Error.WriteLine("  mid2bas <input.mid> [-o output.bas] [--mml-only] [--title T] [--subtitle S] [--voices 1=3,2=5]");
    Console.Error.WriteLine("  <input.sid> [output.sid] --target 0x1000   (SID relocator)");
    Console.Error.WriteLine("  <input.sid> --info                         (SID info)");
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

using e6502.Avalonia.Hardware;
using e6502.Tools;

if (args.Length < 1)
{
    Console.Error.WriteLine("Usage: sidreloc <input.sid> [output.sid] --target 0x1000");
    Console.Error.WriteLine("       sidreloc <input.sid> --info");
    return 1;
}

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

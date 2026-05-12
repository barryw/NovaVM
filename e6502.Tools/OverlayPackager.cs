using System.Globalization;
using System.Text.RegularExpressions;

namespace e6502.Tools;

public sealed record OverlayPackageOptions
{
    public required string InputPath { get; init; }
    public required string OutputPath { get; init; }
    public required ushort LoadAddress { get; init; }
    public ushort? MaxSize { get; init; }
    public string? SymbolPath { get; init; }
    public ushort? InitEntry { get; init; }
    public ushort? MainEntry { get; init; }
    public ushort? TickEntry { get; init; }
    public ushort? UnloadEntry { get; init; }
    public string? InitSymbol { get; init; }
    public string? MainSymbol { get; init; }
    public string? TickSymbol { get; init; }
    public string? UnloadSymbol { get; init; }
    public ushort BssSize { get; init; }
    public ushort ModuleId { get; init; }
    public ushort ModuleVersion { get; init; }
    public byte FormatVersion { get; init; } = OverlayPackager.FormatVersion;
    public byte AbiMajor { get; init; } = OverlayPackager.AbiMajor;
    public byte AbiMinor { get; init; } = OverlayPackager.AbiMinor;
    public byte Flags { get; init; }
}

public sealed record OverlayPackageResult(
    int PayloadSize,
    ushort LoadAddress,
    ushort InitEntry,
    ushort MainEntry,
    ushort TickEntry,
    ushort UnloadEntry,
    ushort Checksum);

public static class OverlayPackager
{
    public const int HeaderSize = 0x20;
    public const byte FormatVersion = 0x01;
    public const byte AbiMajor = 0x01;
    public const byte AbiMinor = 0x00;

    private static readonly Regex SymbolRegex = new(
        @"^\s*al\s+([0-9A-Fa-f]{1,6})\s+\.?([A-Za-z_][A-Za-z0-9_.$@]*)\s*$",
        RegexOptions.Compiled);

    public static OverlayPackageResult Pack(OverlayPackageOptions options)
    {
        byte[] payload = File.ReadAllBytes(options.InputPath);
        if (payload.Length == 0)
            throw new InvalidOperationException("Overlay payload is empty.");
        if (payload.Length > ushort.MaxValue)
            throw new InvalidOperationException($"Overlay payload is too large: {payload.Length} bytes.");

        ushort maxSize = options.MaxSize ?? ushort.MaxValue;
        int totalSlotSize = payload.Length + options.BssSize;
        if (totalSlotSize > maxSize)
            throw new InvalidOperationException($"Overlay payload+BSS {totalSlotSize} bytes exceeds slot size {maxSize} bytes.");
        if (options.LoadAddress + totalSlotSize > 0x10000)
            throw new InvalidOperationException($"Overlay payload+BSS wraps 64K RAM at load address ${options.LoadAddress:X4}.");

        IReadOnlyDictionary<string, ushort> symbols = LoadSymbols(options.SymbolPath);
        ushort init = ResolveEntry(options.InitEntry, options.InitSymbol, symbols, nameof(options.InitEntry));
        ushort main = ResolveEntry(options.MainEntry, options.MainSymbol, symbols, nameof(options.MainEntry));
        ushort tick = ResolveEntry(options.TickEntry, options.TickSymbol, symbols, nameof(options.TickEntry));
        ushort unload = ResolveEntry(options.UnloadEntry, options.UnloadSymbol, symbols, nameof(options.UnloadEntry));

        ValidateEntry(init, options.LoadAddress, payload.Length, "init");
        ValidateEntry(main, options.LoadAddress, payload.Length, "main");
        ValidateEntry(tick, options.LoadAddress, payload.Length, "tick");
        ValidateEntry(unload, options.LoadAddress, payload.Length, "unload");

        ushort checksum = Sum16(payload);
        byte[] header = BuildHeader(options, checked((ushort)payload.Length), init, main, tick, unload, checksum);

        string? dir = Path.GetDirectoryName(Path.GetFullPath(options.OutputPath));
        if (!string.IsNullOrEmpty(dir))
            Directory.CreateDirectory(dir);

        using FileStream output = File.Create(options.OutputPath);
        output.Write(header);
        output.Write(payload);

        return new OverlayPackageResult(payload.Length, options.LoadAddress, init, main, tick, unload, checksum);
    }

    public static IReadOnlyDictionary<string, ushort> LoadSymbols(string? path)
    {
        var symbols = new Dictionary<string, ushort>(StringComparer.Ordinal);
        if (string.IsNullOrWhiteSpace(path))
            return symbols;

        foreach (string line in File.ReadLines(path))
        {
            Match match = SymbolRegex.Match(line);
            if (!match.Success)
                continue;

            int value = int.Parse(match.Groups[1].Value, NumberStyles.HexNumber, CultureInfo.InvariantCulture);
            if (value is < 0 or > ushort.MaxValue)
                continue;

            string name = match.Groups[2].Value;
            symbols[name] = (ushort)value;
        }

        return symbols;
    }

    public static ushort ParseU16(string value, string optionName)
    {
        string text = value.Trim();
        NumberStyles style = NumberStyles.Integer;
        if (text.StartsWith('$'))
        {
            text = text[1..];
            style = NumberStyles.HexNumber;
        }
        else if (text.StartsWith("0x", StringComparison.OrdinalIgnoreCase))
        {
            text = text[2..];
            style = NumberStyles.HexNumber;
        }

        if (!ushort.TryParse(text, style, CultureInfo.InvariantCulture, out ushort result))
            throw new ArgumentException($"Invalid {optionName}: {value}");
        return result;
    }

    private static ushort ResolveEntry(
        ushort? explicitAddress,
        string? symbol,
        IReadOnlyDictionary<string, ushort> symbols,
        string optionName)
    {
        if (explicitAddress.HasValue && !string.IsNullOrWhiteSpace(symbol))
            throw new InvalidOperationException($"{optionName} cannot specify both an address and a symbol.");
        if (explicitAddress.HasValue)
            return explicitAddress.Value;
        if (string.IsNullOrWhiteSpace(symbol))
            return 0;
        if (symbols.TryGetValue(symbol, out ushort value))
            return value;
        if (symbol.StartsWith('.') && symbols.TryGetValue(symbol[1..], out value))
            return value;
        throw new InvalidOperationException($"Symbol '{symbol}' was not found.");
    }

    private static void ValidateEntry(ushort entry, ushort loadAddress, int payloadSize, string label)
    {
        if (entry == 0)
            return;

        int start = loadAddress;
        int end = loadAddress + payloadSize;
        if (entry < start || entry >= end)
        {
            throw new InvalidOperationException(
                $"{label} entry ${entry:X4} is outside overlay payload range ${start:X4}-${end - 1:X4}.");
        }
    }

    private static byte[] BuildHeader(
        OverlayPackageOptions options,
        ushort payloadSize,
        ushort init,
        ushort main,
        ushort tick,
        ushort unload,
        ushort checksum)
    {
        byte[] header = new byte[HeaderSize];
        header[0x00] = (byte)'N';
        header[0x01] = (byte)'O';
        header[0x02] = (byte)'V';
        header[0x03] = (byte)'O';
        header[0x04] = options.FormatVersion;
        header[0x05] = options.AbiMajor;
        header[0x06] = options.AbiMinor;
        header[0x07] = options.Flags;
        WriteU16(header, 0x08, options.LoadAddress);
        WriteU16(header, 0x0A, payloadSize);
        WriteU16(header, 0x0C, options.BssSize);
        WriteU16(header, 0x0E, init);
        WriteU16(header, 0x10, main);
        WriteU16(header, 0x12, tick);
        WriteU16(header, 0x14, unload);
        WriteU16(header, 0x16, options.ModuleId);
        WriteU16(header, 0x18, options.ModuleVersion);
        WriteU16(header, 0x1E, checksum);
        return header;
    }

    private static void WriteU16(byte[] data, int offset, ushort value)
    {
        data[offset] = (byte)(value & 0xFF);
        data[offset + 1] = (byte)(value >> 8);
    }

    private static ushort Sum16(ReadOnlySpan<byte> data)
    {
        int sum = 0;
        foreach (byte value in data)
            sum = (sum + value) & 0xFFFF;
        return (ushort)sum;
    }
}

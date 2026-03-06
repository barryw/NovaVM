using System.Text;
using System.Text.Json;

namespace e6502.Storage;

/// <summary>
/// Tokenizes and detokenizes EhBASIC programs between ASCII text and the binary
/// .bas format used by the 6502 emulator.
///
/// Binary format per line (no 2-byte load-address prefix):
///   [next-line-ptr: 2B LE] [line-num: 2B LE] [tokenized body] [$00]
/// Program ends with [$00][$00].
/// </summary>
public sealed class BasicTokenizer
{
    // -------------------------------------------------------------------------
    // Token tables
    // -------------------------------------------------------------------------

    /// <summary>Byte value of the prefix that introduces an extended token.</summary>
    private readonly byte _prefixByte;

    /// <summary>Primary token lookup: keyword (upper) -> byte value 128..255.</summary>
    private readonly (string Keyword, byte Value)[] _primarySorted;

    /// <summary>Extended token lookup: keyword (upper) -> byte id.</summary>
    private readonly (string Keyword, byte Id)[] _extendedSorted;

    /// <summary>Reverse map: primary byte -> keyword string.</summary>
    private readonly Dictionary<byte, string> _primaryReverse;

    /// <summary>Reverse map: extended id byte -> keyword string.</summary>
    private readonly Dictionary<byte, string> _extendedReverse;

    // -------------------------------------------------------------------------
    // Construction
    // -------------------------------------------------------------------------

    private BasicTokenizer(
        byte prefixByte,
        Dictionary<byte, string> primary,
        Dictionary<byte, string> extended)
    {
        _prefixByte = prefixByte;
        _primaryReverse = primary;
        _extendedReverse = extended;

        // Build forward lookup sorted longest-first for greedy matching
        _primarySorted = primary
            .Select(kv => (kv.Value.ToUpperInvariant(), kv.Key))
            .OrderByDescending(t => t.Item1.Length)
            .ToArray();

        _extendedSorted = extended
            .Select(kv => (kv.Value.ToUpperInvariant(), kv.Key))
            .OrderByDescending(t => t.Item1.Length)
            .ToArray();
    }

    /// <summary>Loads token tables from the JSON file produced by gen_tokens.py.</summary>
    public static BasicTokenizer FromJsonFile(string path)
    {
        using var stream = File.OpenRead(path);
        using var doc = JsonDocument.Parse(stream);
        var root = doc.RootElement;

        byte prefixByte = root.GetProperty("prefix_byte").GetByte();

        var primary = new Dictionary<byte, string>();
        foreach (var prop in root.GetProperty("primary").EnumerateObject())
            primary[byte.Parse(prop.Name)] = prop.Value.GetString()!;

        var extended = new Dictionary<byte, string>();
        foreach (var prop in root.GetProperty("extended").EnumerateObject())
            extended[byte.Parse(prop.Name)] = prop.Value.GetString()!;

        return new BasicTokenizer(prefixByte, primary, extended);
    }

    // -------------------------------------------------------------------------
    // Tokenize
    // -------------------------------------------------------------------------

    /// <summary>
    /// Converts an array of ASCII BASIC lines into binary .bas format.
    /// Each element of <paramref name="lines"/> must start with a decimal line number.
    /// </summary>
    /// <param name="lines">ASCII source lines, e.g. "10 PRINT \"HELLO\"".</param>
    /// <param name="baseAddr">Load address of the first byte of the output.</param>
    /// <returns>Binary .bas data WITHOUT the 2-byte load-address prefix.</returns>
    public byte[] Tokenize(string[] lines, ushort baseAddr)
    {
        if (lines.Length == 0)
            return [0x00, 0x00];

        // First pass: build tokenized body for each line (without next-ptr)
        var bodies = new List<(ushort LineNum, byte[] Body)>(lines.Length);
        foreach (string line in lines)
        {
            var (lineNum, body) = TokenizeLine(line);
            bodies.Add((lineNum, body));
        }

        // Second pass: compute next-ptr and emit
        // Each line record is: [2B next-ptr][2B line-num][body bytes][$00]
        int totalSize = bodies.Sum(b => 4 + b.Body.Length + 1) + 2; // +2 for terminator
        var result = new byte[totalSize];
        int offset = 0;

        for (int i = 0; i < bodies.Count; i++)
        {
            var (lineNum, body) = bodies[i];
            int recordLen = 4 + body.Length + 1; // next-ptr(2) + linenum(2) + body + $00
            // next-ptr always points to start of next record (or the $00$00 terminator).
            // It is never zero for a real line — zero is reserved for the terminator block.
            ushort nextPtr = (ushort)(baseAddr + offset + recordLen);

            // next-ptr (but we'll fill it after: calculate from baseAddr + current offset + recordLen)
            // Actually: next-ptr points to the start of the NEXT line record in emulator memory.
            // baseAddr is where result[0] lives in emulator RAM.
            // next line starts at baseAddr + offset + recordLen
            WriteU16(result, offset, nextPtr);
            WriteU16(result, offset + 2, lineNum);
            Array.Copy(body, 0, result, offset + 4, body.Length);
            result[offset + 4 + body.Length] = 0x00;
            offset += recordLen;
        }

        // Terminator $00 $00
        result[offset] = 0x00;
        result[offset + 1] = 0x00;

        return result;
    }

    private (ushort LineNum, byte[] Body) TokenizeLine(string line)
    {
        int pos = 0;

        // Parse line number
        int numStart = pos;
        while (pos < line.Length && char.IsDigit(line[pos]))
            pos++;
        ushort lineNum = ushort.Parse(line[numStart..pos]);

        // Skip exactly one space after line number (if present)
        if (pos < line.Length && line[pos] == ' ')
            pos++;

        // Tokenize body
        var body = new List<byte>(line.Length);
        bool afterRem = false;

        while (pos < line.Length)
        {
            if (afterRem)
            {
                // Everything after REM is literal
                body.Add((byte)line[pos]);
                pos++;
                continue;
            }

            if (line[pos] == '"')
            {
                // Pass quoted string through literally (including delimiters)
                body.Add((byte)'"');
                pos++;
                while (pos < line.Length && line[pos] != '"')
                {
                    body.Add((byte)line[pos]);
                    pos++;
                }
                if (pos < line.Length)
                {
                    body.Add((byte)'"');
                    pos++;
                }
                continue;
            }

            // Try extended tokens first (longer keywords may appear in extended)
            bool matched = false;
            string upper = line[pos..].ToUpperInvariant();

            foreach (var (keyword, id) in _extendedSorted)
            {
                if (upper.StartsWith(keyword, StringComparison.Ordinal))
                {
                    body.Add(_prefixByte);
                    body.Add(id);
                    pos += keyword.Length;
                    matched = true;
                    break;
                }
            }

            if (!matched)
            {
                // Try primary tokens
                foreach (var (keyword, value) in _primarySorted)
                {
                    if (upper.StartsWith(keyword, StringComparison.Ordinal))
                    {
                        body.Add(value);
                        pos += keyword.Length;
                        matched = true;

                        // If REM, everything that follows is literal
                        if (value == 0x91) // REM = 145 = 0x91
                            afterRem = true;

                        break;
                    }
                }
            }

            if (!matched)
            {
                // Literal byte
                body.Add((byte)line[pos]);
                pos++;
            }
        }

        return (lineNum, body.ToArray());
    }

    // -------------------------------------------------------------------------
    // Detokenize
    // -------------------------------------------------------------------------

    /// <summary>
    /// Converts binary .bas data (WITHOUT the 2-byte load-address prefix) back
    /// to ASCII BASIC lines.
    /// </summary>
    public string[] Detokenize(byte[] data)
    {
        var lines = new List<string>();
        int pos = 0;

        while (pos + 1 < data.Length)
        {
            ushort nextPtr = ReadU16(data, pos);
            if (nextPtr == 0)
                break;

            ushort lineNum = ReadU16(data, pos + 2);
            pos += 4;

            var sb = new StringBuilder();
            sb.Append(lineNum);
            sb.Append(' ');

            bool afterRem = false;

            while (pos < data.Length && data[pos] != 0x00)
            {
                byte b = data[pos];

                if (afterRem)
                {
                    sb.Append((char)b);
                    pos++;
                    continue;
                }

                if (b == _prefixByte)
                {
                    // Extended token: next byte is the id
                    pos++;
                    if (pos < data.Length)
                    {
                        byte id = data[pos];
                        if (_extendedReverse.TryGetValue(id, out string? keyword))
                            sb.Append(keyword);
                        else
                            sb.Append((char)id); // unknown — emit raw
                        pos++;
                    }
                    continue;
                }

                if (b == (byte)'"')
                {
                    // Quoted string — copy literally
                    sb.Append('"');
                    pos++;
                    while (pos < data.Length && data[pos] != 0x00 && data[pos] != (byte)'"')
                    {
                        sb.Append((char)data[pos]);
                        pos++;
                    }
                    if (pos < data.Length && data[pos] == (byte)'"')
                    {
                        sb.Append('"');
                        pos++;
                    }
                    continue;
                }

                if (b >= 128 && _primaryReverse.TryGetValue(b, out string? primary))
                {
                    sb.Append(primary);
                    pos++;
                    if (b == 0x91) // REM
                        afterRem = true;
                    continue;
                }

                // Literal character
                sb.Append((char)b);
                pos++;
            }

            // Skip $00 line terminator
            if (pos < data.Length && data[pos] == 0x00)
                pos++;

            lines.Add(sb.ToString());
        }

        return lines.ToArray();
    }

    // -------------------------------------------------------------------------
    // Helpers
    // -------------------------------------------------------------------------

    private static void WriteU16(byte[] buf, int offset, ushort value)
    {
        buf[offset] = (byte)(value & 0xFF);
        buf[offset + 1] = (byte)(value >> 8);
    }

    private static ushort ReadU16(byte[] data, int offset) =>
        (ushort)(data[offset] | (data[offset + 1] << 8));
}

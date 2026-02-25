namespace e6502.Avalonia.Editor;

/// <summary>
/// Lightweight per-line syntax highlighter for NCC C source code.
/// Tracks block comment state across lines. Returns color indices per character.
/// </summary>
public sealed class SyntaxHighlighter
{
    // VGC color indices
    public const byte ColorDefault     = 1;   // white
    public const byte ColorKeyword     = 3;   // cyan
    public const byte ColorPreproc     = 4;   // purple
    public const byte ColorString      = 5;   // green
    public const byte ColorType        = 7;   // yellow
    public const byte ColorNumber      = 8;   // orange
    public const byte ColorComment     = 12;  // grey

    private bool _inBlockComment;

    private static readonly HashSet<string> Keywords = new()
    {
        "if", "else", "while", "do", "for", "switch", "case", "default",
        "break", "continue", "return", "asm", "sizeof",
        "true", "false", "__resident", "__interrupt",
        "struct", "enum", "const"
    };

    private static readonly HashSet<string> Types = new()
    {
        "byte", "int", "uint", "bool", "fixed", "ufixed", "void"
    };

    public bool InBlockComment => _inBlockComment;

    /// <summary>
    /// Resets the block comment state (call when loading a new file).
    /// </summary>
    public void Reset() => _inBlockComment = false;

    /// <summary>
    /// Highlights a single line and returns a color byte per character.
    /// Call lines in order from top to bottom so block comment state tracks correctly.
    /// </summary>
    public byte[] HighlightLine(string line)
    {
        var colors = new byte[line.Length];
        int i = 0;

        while (i < line.Length)
        {
            if (_inBlockComment)
            {
                int end = line.IndexOf("*/", i, StringComparison.Ordinal);
                if (end < 0)
                {
                    Fill(colors, i, line.Length, ColorComment);
                    i = line.Length;
                }
                else
                {
                    Fill(colors, i, end + 2, ColorComment);
                    i = end + 2;
                    _inBlockComment = false;
                }
                continue;
            }

            char c = line[i];

            // Line comment
            if (c == '/' && i + 1 < line.Length && line[i + 1] == '/')
            {
                Fill(colors, i, line.Length, ColorComment);
                break;
            }

            // Block comment start
            if (c == '/' && i + 1 < line.Length && line[i + 1] == '*')
            {
                _inBlockComment = true;
                int end = line.IndexOf("*/", i + 2, StringComparison.Ordinal);
                if (end < 0)
                {
                    Fill(colors, i, line.Length, ColorComment);
                    i = line.Length;
                }
                else
                {
                    Fill(colors, i, end + 2, ColorComment);
                    i = end + 2;
                    _inBlockComment = false;
                }
                continue;
            }

            // Preprocessor directive
            if (c == '#')
            {
                Fill(colors, i, line.Length, ColorPreproc);
                break;
            }

            // String literal
            if (c == '"')
            {
                int start = i;
                i++;
                while (i < line.Length && line[i] != '"')
                {
                    if (line[i] == '\\' && i + 1 < line.Length)
                        i++;
                    i++;
                }
                if (i < line.Length) i++; // closing quote
                Fill(colors, start, i, ColorString);
                continue;
            }

            // Char literal
            if (c == '\'')
            {
                int start = i;
                i++;
                while (i < line.Length && line[i] != '\'')
                {
                    if (line[i] == '\\' && i + 1 < line.Length)
                        i++;
                    i++;
                }
                if (i < line.Length) i++;
                Fill(colors, start, i, ColorString);
                continue;
            }

            // Number (decimal, hex, binary)
            if (char.IsDigit(c))
            {
                int start = i;
                if (c == '0' && i + 1 < line.Length && (line[i + 1] == 'x' || line[i + 1] == 'X'))
                {
                    i += 2;
                    while (i < line.Length && IsHexDigit(line[i])) i++;
                }
                else if (c == '0' && i + 1 < line.Length && (line[i + 1] == 'b' || line[i + 1] == 'B'))
                {
                    i += 2;
                    while (i < line.Length && (line[i] == '0' || line[i] == '1')) i++;
                }
                else
                {
                    while (i < line.Length && (char.IsDigit(line[i]) || line[i] == '.')) i++;
                }
                Fill(colors, start, i, ColorNumber);
                continue;
            }

            // Identifier / keyword
            if (c == '_' || char.IsLetter(c))
            {
                int start = i;
                while (i < line.Length && (char.IsLetterOrDigit(line[i]) || line[i] == '_'))
                    i++;
                string word = line[start..i];
                byte color = Types.Contains(word) ? ColorType
                           : Keywords.Contains(word) ? ColorKeyword
                           : ColorDefault;
                Fill(colors, start, i, color);
                continue;
            }

            // Default: operators, whitespace, etc.
            colors[i] = ColorDefault;
            i++;
        }

        return colors;
    }

    private static void Fill(byte[] arr, int from, int to, byte value)
    {
        for (int i = from; i < to && i < arr.Length; i++)
            arr[i] = value;
    }

    private static bool IsHexDigit(char c) =>
        char.IsDigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

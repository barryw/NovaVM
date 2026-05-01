namespace e6502.Avalonia.Editor;

/// <summary>
/// Per-line syntax highlighter for BASIC source code.
/// Input is the code portion of a line (without the line number).
/// Returns a color byte per character for VGC color RAM.
/// </summary>
public sealed class BasicSyntaxHighlighter : ISyntaxHighlighter
{
    public const byte ColorDefault = 14;  // light blue — variables, operators
    public const byte ColorKeyword = 1;   // white
    public const byte ColorString  = 5;   // green
    public const byte ColorNumber  = 8;   // orange
    public const byte ColorComment = 12;  // grey

    // EhBASIC keywords sorted longest-first for greedy matching.
    private static readonly string[] Keywords =
    [
        "DIROPEN", "DIRNAM$", "DIRNEXT", "DIRTYP", "DIRSIZ",
        "MIDPLAY", "MIDSTOP", "SIDPLAY", "SIDSTOP",
        "RETURN", "RESTORE", "RENUM",
        "PRINT", "GOSUB", "INPUT", "WHILE", "WEND",
        "MUSIC", "MOUNT", "MKDIR", "RMDIR",
        "XLOAD", "XSAVE",
        "COLOR", "SLEEP",
        "THEN", "ELSE", "STEP", "NEXT", "GOTO", "READ",
        "LOAD", "SAVE", "POKE", "PEEK", "CALL",
        "META$", "DIR",
        "FOR", "TO", "IF", "ON", "OR", "LET", "DIM", "DEF",
        "END", "RUN", "NEW", "CLR", "REM", "NOT", "AND",
        "CHR$", "STR$", "LEFT$", "RIGHT$", "MID$", "LEN", "VAL",
        "ASC", "SPC", "TAB", "FN", "ABS", "SGN", "INT", "SQR",
        "RND", "LOG", "EXP", "COS", "SIN", "TAN", "ATN",
        "FRE", "POS", "DATA", "CONT", "LIST", "STOP",
        "GET", "PUT", "WAIT", "USR", "DEL", "PWD", "CD",
    ];

    public void Reset() { }

    public byte[] HighlightLine(string line)
    {
        if (line.Length == 0)
            return [];

        var colors = new byte[line.Length];
        int i = 0;

        while (i < line.Length)
        {
            char c = line[i];

            // Quoted string
            if (c == '"')
            {
                int start = i++;
                while (i < line.Length && line[i] != '"')
                    i++;
                if (i < line.Length) i++; // closing quote
                Fill(colors, start, i, ColorString);
                continue;
            }

            // Number: only when not preceded by a letter (i.e. not part of variable name)
            if (char.IsDigit(c) && (i == 0 || !char.IsLetterOrDigit(line[i - 1])))
            {
                int start = i;
                while (i < line.Length && (char.IsDigit(line[i]) || line[i] == '.'))
                    i++;
                Fill(colors, start, i, ColorNumber);
                continue;
            }

            // Identifier: try keyword match first, greedy longest-first
            if (char.IsLetter(c))
            {
                string upper = line[i..].ToUpperInvariant();
                bool matched = false;

                foreach (string kw in Keywords)
                {
                    if (!upper.StartsWith(kw, StringComparison.Ordinal))
                        continue;

                    int kwLen = kw.Length;

                    // Keywords ending in $ are naturally bounded by the $
                    bool dollarSuffix = kw[^1] == '$';

                    // For non-$ keywords: must not be followed by a letter or digit
                    if (!dollarSuffix)
                    {
                        int next = i + kwLen;
                        if (next < line.Length && char.IsLetterOrDigit(line[next]))
                            continue;
                    }

                    // REM: everything from here to end of line is a comment
                    if (kw == "REM")
                    {
                        Fill(colors, i, line.Length, ColorComment);
                        i = line.Length;
                        matched = true;
                        break;
                    }

                    // DATA: keyword colored, rest of line is comment
                    if (kw == "DATA")
                    {
                        Fill(colors, i, i + kwLen, ColorKeyword);
                        Fill(colors, i + kwLen, line.Length, ColorComment);
                        i = line.Length;
                        matched = true;
                        break;
                    }

                    Fill(colors, i, i + kwLen, ColorKeyword);
                    i += kwLen;
                    matched = true;
                    break;
                }

                if (!matched)
                {
                    // Regular identifier — consume letters/digits
                    int start = i;
                    while (i < line.Length && (char.IsLetterOrDigit(line[i]) || line[i] == '_'))
                        i++;
                    Fill(colors, start, i, ColorDefault);
                }
                continue;
            }

            // Default: operators, whitespace, punctuation
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
}

using Avalonia.Input;
using e6502.Avalonia.Hardware;
using e6502.Avalonia.Input;
using e6502.Storage;
using KDS.e6502;

namespace e6502.Avalonia.Editor;

/// <summary>
/// Full-screen BASIC source editor. Inherits all common editor mechanics from
/// ScreenTextEditor and adds BASIC-specific features: line numbers, memory
/// round-trip via BasicTokenizer, Run/ReturnFromRun, and gutter rendering.
/// </summary>
public sealed class BasicEditor : ScreenTextEditor
{
    // ── BASIC line number constants ───────────────────────────────────────────
    private const int MaxLineNumber = 63999;
    private const int DefaultStep   = 10;

    // ── Parallel line-number list (synced with base _lines) ──────────────────
    private readonly List<int> _lineNumbers = new();

    // ── Lazy-init tokenizer ───────────────────────────────────────────────────
    private BasicTokenizer? _tokenizer;

    // ── External dependencies (set after construction) ────────────────────────
    private ScreenEditor? _screenEditor;

    // ── Syntax highlighter ────────────────────────────────────────────────────
    private readonly BasicSyntaxHighlighter _highlighter = new();

    // ── Constructor ───────────────────────────────────────────────────────────

    public BasicEditor(IBusDevice bus) : base(bus)
    {
        // Start with one empty line numbered 10
        _lineNumbers.Add(10);
        // base class already has one empty string in _lines
    }

    /// <summary>Set by MainWindow after construction.</summary>
    public void SetRunDependencies(Cpu cpu, ScreenEditor screenEditor)
    {
        _screenEditor = screenEditor;
    }

    // ── ScreenTextEditor abstract overrides ───────────────────────────────────

    protected override string GetTitle()    => Modified ? "NOVA EDIT [*]" : "NOVA EDIT";
    protected override string GetHelpText() => "F5:Run  Esc:Exit  ^F:Find  ^G:Goto  ^S:Save  F3:Open";
    protected override ISyntaxHighlighter GetHighlighter() => _highlighter;
    protected override int GetGutterWidth() => 6;  // 5 digits + 1 space
    protected override string GetFileExtension() => ".bas";

    protected override string GetStatusExtra()
    {
        try
        {
            ushort progEnd = (ushort)(Bus.Read(VgcConstants.ZpSvarl) | (Bus.Read(VgcConstants.ZpSvarh) << 8));
            int free = VgcConstants.BasicEnd - progEnd;
            if (free < 0) free = 0;
            return $"Free: {free}";
        }
        catch
        {
            return "";
        }
    }

    protected override void RenderGutterCell(int lineIndex, int row)
    {
        if (lineIndex < _lineNumbers.Count)
        {
            string numStr = _lineNumbers[lineIndex].ToString().PadLeft(5);
            for (int g = 0; g < 5; g++)
                WriteChar(1 + g, row, (byte)numStr[g], GutterFg);
            WriteChar(6, row, (byte)' ', GutterFg);
        }
    }

    protected override byte[] SerializeForSave(string filename)
    {
        var tok = GetTokenizer() ?? throw new InvalidOperationException("Tokenizer not available");
        ushort baseAddr = (ushort)(Bus.Read(VgcConstants.ZpSmeml) | (Bus.Read(VgcConstants.ZpSmemh) << 8));
        return tok.Tokenize(ToTextLines(), baseAddr);
    }

    protected override void DeserializeOnLoad(byte[] data, string filename)
    {
        var tok = GetTokenizer() ?? throw new InvalidOperationException("Tokenizer not available");
        string[] textLines = tok.Detokenize(data);
        LoadText(textLines);
    }

    protected override void OnActivate()
    {
        // Disable hardware cursor (base has already set bg color and cleared screen)
        Bus.Write(VgcConstants.RegCursorEnable, 0);

        var tok = GetTokenizer();
        if (tok != null)
            ReadFromMemory(tok);
    }

    protected override void OnDeactivate()
    {
        var tok = GetTokenizer();
        if (tok != null)
            WriteToMemory(tok);
        RestoreScreen();
    }

    protected override bool HandleEditorKey(Key key, KeyModifiers modifiers)
    {
        if (key == Key.Escape)
        {
            Deactivate();
            return true;
        }

        if (key == Key.F5)
        {
            Run();
            return true;
        }

        return false;
    }

    // ── Line number hooks (called by base class after structural changes) ─────

    protected override void OnLineInserted(int lineIndex)
    {
        int newNum = CalculateNewLineNumber(lineIndex);
        _lineNumbers.Insert(lineIndex, newNum);
    }

    protected override void OnLineRemoved(int lineIndex)
    {
        if (lineIndex < _lineNumbers.Count)
            _lineNumbers.RemoveAt(lineIndex);
    }

    protected override void OnLinesReset()
    {
        // Called when LoadLines replaces the buffer. If _lineNumbers is already
        // in sync (set by LoadText before calling LoadLines), do nothing.
        if (_lineNumbers.Count == LineCount) return;

        // Otherwise (e.g., called from NewFile), generate default numbers.
        _lineNumbers.Clear();
        for (int i = 0; i < LineCount; i++)
            _lineNumbers.Add((i + 1) * DefaultStep);
    }

    // ── Public accessors for testing ─────────────────────────────────────────

    internal int GetLineNumber(int index) => _lineNumbers[index];

    // ── Text buffer operations (BASIC-specific) ───────────────────────────────

    /// <summary>Load text lines of the form "10 PRINT ..." into the buffer.</summary>
    internal void LoadText(string[] lines)
    {
        var texts = new List<string>();
        var numbers = new List<int>();

        foreach (string raw in lines)
        {
            if (ParseLine(raw, out int lineNum, out string text))
            {
                texts.Add(text);
                numbers.Add(lineNum);
            }
        }

        if (texts.Count == 0)
        {
            texts.Add("");
            numbers.Add(10);
        }

        _lineNumbers.Clear();
        _lineNumbers.AddRange(numbers);
        // LoadLines syncs base _lines without triggering hooks
        LoadLines(texts.ToArray());
    }

    /// <summary>Reassemble "10 PRINT ..." strings from the buffer.</summary>
    internal string[] ToTextLines()
    {
        var result = new string[LineCount];
        for (int i = 0; i < LineCount; i++)
            result[i] = $"{_lineNumbers[i]} {GetLineText(i)}";
        return result;
    }

    private static bool ParseLine(string raw, out int lineNum, out string text)
    {
        int i = 0;
        while (i < raw.Length && char.IsWhiteSpace(raw[i])) i++;
        int start = i;
        while (i < raw.Length && char.IsDigit(raw[i])) i++;
        if (i == start || !int.TryParse(raw[start..i], out lineNum))
        {
            lineNum = 0;
            text = "";
            return false;
        }
        // Skip one optional space
        if (i < raw.Length && raw[i] == ' ') i++;
        text = raw[i..];
        return true;
    }

    // ── Line number calculation ───────────────────────────────────────────────

    private int CalculateNewLineNumber(int insertedAt)
    {
        // The line at insertedAt was just inserted; we need to find a number
        // between the previous and next line numbers.
        int prevNum = insertedAt > 0 && insertedAt - 1 < _lineNumbers.Count
            ? _lineNumbers[insertedAt - 1]
            : 0;

        // Next: look at the entry after insertedAt (if any)
        int nextNum = insertedAt < _lineNumbers.Count
            ? _lineNumbers[insertedAt]
            : Math.Min(prevNum + DefaultStep * 2, MaxLineNumber);

        if (nextNum <= prevNum)
            nextNum = Math.Min(prevNum + DefaultStep, MaxLineNumber);

        int midpoint = (prevNum + nextNum) / 2;
        if (midpoint <= prevNum)
        {
            // No integer gap — renumber entire buffer after inserting placeholder
            // Insert 0 temporarily, renumber, then the caller will use the result
            _lineNumbers.Insert(insertedAt, 0);
            RenumberAll();
            int result = _lineNumbers[insertedAt];
            _lineNumbers.RemoveAt(insertedAt);
            return result;
        }
        return Math.Min(midpoint, MaxLineNumber);
    }

    private void RenumberAll()
    {
        for (int i = 0; i < _lineNumbers.Count; i++)
            _lineNumbers[i] = (i + 1) * DefaultStep;
    }

    // ── BASIC memory round-trip ───────────────────────────────────────────────

    /// <summary>
    /// Read the current BASIC program from CPU RAM using the start/end pointers
    /// at Smeml ($79/$7A) and Svarl ($7B/$7C).
    /// </summary>
    internal void ReadFromMemory(BasicTokenizer tokenizer)
    {
        ushort progStart = (ushort)(Bus.Read(VgcConstants.ZpSmeml) | (Bus.Read(VgcConstants.ZpSmemh) << 8));
        ushort progEnd   = (ushort)(Bus.Read(VgcConstants.ZpSvarl) | (Bus.Read(VgcConstants.ZpSvarh) << 8));

        if (progEnd <= progStart)
        {
            LoadText([]);
            return;
        }

        int length = progEnd - progStart;
        var data = new byte[length];
        for (int i = 0; i < length; i++)
            data[i] = Bus.Read((ushort)(progStart + i));

        string[] textLines = tokenizer.Detokenize(data);
        LoadText(textLines);
    }

    /// <summary>
    /// Write the current buffer to CPU RAM as a tokenized BASIC program.
    /// Updates the end-of-program pointer (Svarl at $7B/$7C).
    /// </summary>
    internal void WriteToMemory(BasicTokenizer tokenizer)
    {
        ushort baseAddr = (ushort)(Bus.Read(VgcConstants.ZpSmeml) | (Bus.Read(VgcConstants.ZpSmemh) << 8));
        string[] textLines = ToTextLines();
        byte[] data = tokenizer.Tokenize(textLines, baseAddr);

        for (int i = 0; i < data.Length; i++)
            Bus.Write((ushort)(baseAddr + i), data[i]);

        // Update end-of-program pointer (Svarl = start of variables)
        ushort endAddr = (ushort)(baseAddr + data.Length);
        Bus.Write(VgcConstants.ZpSvarl, (byte)(endAddr & 0xFF));
        Bus.Write(VgcConstants.ZpSvarh, (byte)(endAddr >> 8));
        ClearModified();
    }

    /// <summary>Find tokens.json by walking up from current directory.</summary>
    internal static string FindTokensJson()
    {
        string? dir = Directory.GetCurrentDirectory();
        while (dir != null)
        {
            string candidate = Path.Combine(dir, "ehbasic", "tokens.json");
            if (File.Exists(candidate)) return candidate;
            string? parent = Path.GetDirectoryName(dir);
            if (parent == dir) break;
            dir = parent;
        }
        throw new FileNotFoundException("tokens.json not found");
    }

    private BasicTokenizer? GetTokenizer()
    {
        if (_tokenizer != null) return _tokenizer;
        try
        {
            string path = FindTokensJson();
            _tokenizer = BasicTokenizer.FromJsonFile(path);
            return _tokenizer;
        }
        catch
        {
            return null;
        }
    }

    // ── Run / ReturnFromRun ───────────────────────────────────────────────────

    /// <summary>
    /// Tokenizes the buffer to memory, clears the screen, re-enables VGC cursor,
    /// and injects "RUN\r" into the ScreenEditor input queue.
    /// </summary>
    public void Run()
    {
        var tok = GetTokenizer();
        if (tok != null)
            WriteToMemory(tok);

        Bus.Write(VgcConstants.RegBgCol, 0);
        ClearScreen();
        Bus.Write(VgcConstants.RegCursorEnable, 1);

        Mode = EditorMode.Running;

        if (_screenEditor != null)
        {
            foreach (byte b in "RUN\r"u8)
                _screenEditor.QueueInput(b);
        }
    }

    /// <summary>
    /// Called when "Ready" is detected on screen after a program run.
    /// Re-activates the editor and re-reads the program from memory.
    /// </summary>
    public void ReturnFromRun()
    {
        Mode = EditorMode.Edit;
        Bus.Write(VgcConstants.RegCursorEnable, 0);
        Bus.Write(VgcConstants.RegBgCol, EditorBg);
        ClearScreen();

        var tok = GetTokenizer();
        if (tok != null)
            ReadFromMemory(tok);

        Redraw();
    }

    /// <summary>Toggle editor on/off — called by F5 from BASIC.</summary>
    public void ToggleActivation()
    {
        if (IsActive)
            Deactivate();
        else
            Activate();
    }
}

using Avalonia.Input;
using e6502.Avalonia.Hardware;
using e6502.Avalonia.Input;
using e6502.Storage;
using KDS.e6502;

namespace e6502.Avalonia.Editor;

/// <summary>
/// Full-screen BASIC source editor rendered to VGC character/color RAM.
/// Layout: row 0 = title bar, rows 1-23 = code area (23 visible lines),
/// row 24 = status bar.  Gutter is 6 chars wide (5-digit line number + space).
/// Code area is 74 chars wide.
/// </summary>
public sealed class BasicEditor
{
    // ── Screen layout ────────────────────────────────────────────────────────
    private const int ScreenCols   = VgcConstants.ScreenCols;  // 80
    private const int ScreenRows   = VgcConstants.ScreenRows;  // 25
    private const int TitleRow     = 0;
    private const int CodeStartRow = 1;
    private const int CodeEndRow   = 23;   // rows 1-23 = 23 visible lines
    private const int VisibleLines = CodeEndRow - CodeStartRow + 1;  // 23
    private const int StatusRow    = 24;

    private const int GutterWidth  = 6;    // 5 digits + 1 space
    private const int CodeWidth    = 74;   // 80 - 6

    // ── Colors ───────────────────────────────────────────────────────────────
    private const byte EditorBg   = 6;   // dark blue
    private const byte TitleFg    = 1;   // white
    private const byte StatusFg   = 14;  // light blue
    private const byte GutterFg   = 12;  // grey
    private const byte DefaultFg  = 1;   // white (fallback)

    // ── Data model ───────────────────────────────────────────────────────────
    private readonly List<(int LineNumber, string Text)> _lines = new();
    private int _cursorLine;    // 0-based index into _lines
    private int _cursorCol;     // 0-based column into line text
    private int _scrollY;       // first visible line index
    private bool _modified;
    private bool _insertMode = true;

    // Lazy-init tokenizer
    private BasicTokenizer? _tokenizer;

    // ── External references ──────────────────────────────────────────────────
    private readonly IBusDevice _bus;
    private readonly BasicSyntaxHighlighter _highlighter = new();
    private ScreenEditor? _screenEditor;
    private Cpu? _cpu;

    // ── Clipboard ────────────────────────────────────────────────────────────
    private readonly List<string> _clipboard = new();

    // ── Prompt state ─────────────────────────────────────────────────────────
    private bool _prompting;
    private string _promptText = "";
    private string _promptInput = "";
    private Action<string>? _promptCallback;
    private string _message = "";
    private string _lastSearch = "";

    // ── Editor state ─────────────────────────────────────────────────────────
    public EditorMode Mode { get; private set; } = EditorMode.Edit;
    public bool IsActive { get; private set; }

    // ── Constructor ──────────────────────────────────────────────────────────

    public BasicEditor(IBusDevice bus)
    {
        _bus = bus;
        // Start with one empty line
        _lines.Add((10, ""));
    }

    /// <summary>Set by MainWindow after construction.</summary>
    public void SetRunDependencies(Cpu cpu, ScreenEditor screenEditor)
    {
        _cpu = cpu;
        _screenEditor = screenEditor;
    }

    // ── Public accessors for testing ─────────────────────────────────────────

    internal int LineCount => _lines.Count;

    internal int GetLineNumber(int index) => _lines[index].LineNumber;

    internal string GetLineText(int index) => _lines[index].Text;

    internal void SetCursor(int line, int col)
    {
        _cursorLine = Math.Clamp(line, 0, Math.Max(0, _lines.Count - 1));
        _cursorCol = Math.Max(0, col);
    }

    internal int CursorLine => _cursorLine;
    internal int CursorCol  => _cursorCol;
    internal bool Modified   => _modified;
    internal bool InsertMode => _insertMode;

    // ── Activation ───────────────────────────────────────────────────────────

    public void Activate()
    {
        if (IsActive) return;
        IsActive = true;
        _bus.Write(VgcConstants.RegCursorEnable, 0);
        _bus.Write(VgcConstants.RegBgCol, EditorBg);
        ClearScreen();

        // Load current BASIC program from memory
        var tok = GetTokenizer();
        if (tok != null)
            ReadFromMemory(tok);

        Redraw();
    }

    public void Deactivate()
    {
        if (!IsActive) return;
        IsActive = false;

        // Write program back to memory
        var tok = GetTokenizer();
        if (tok != null)
            WriteToMemory(tok);

        _bus.Write(VgcConstants.RegBgCol, 0);
        ClearScreen();
        _bus.Write(VgcConstants.RegCursorEnable, 1);
    }

    /// <summary>Toggle editor on/off — called by F5 from BASIC.</summary>
    public void ToggleActivation()
    {
        if (IsActive)
            Deactivate();
        else
            Activate();
    }

    /// <summary>
    /// Tokenizes the buffer to memory, clears the screen, re-enables VGC cursor,
    /// and injects "RUN\r" into the ScreenEditor input queue.
    /// Sets Mode = Running so that keyboard input passes through to BASIC.
    /// </summary>
    public void Run()
    {
        var tok = GetTokenizer();
        if (tok != null)
            WriteToMemory(tok);

        // Restore normal BASIC display
        _bus.Write(VgcConstants.RegBgCol, 0);
        ClearScreen();
        _bus.Write(VgcConstants.RegCursorEnable, 1);

        Mode = EditorMode.Running;

        // Inject RUN command into BASIC input queue
        if (_screenEditor != null)
        {
            foreach (byte b in "RUN\r"u8)
                _screenEditor.QueueInput(b);
        }
    }

    /// <summary>
    /// Called when "Ready" is detected on screen after a program run.
    /// Re-activates the editor, re-reads the program from memory.
    /// </summary>
    public void ReturnFromRun()
    {
        Mode = EditorMode.Edit;
        _bus.Write(VgcConstants.RegCursorEnable, 0);
        _bus.Write(VgcConstants.RegBgCol, EditorBg);
        ClearScreen();

        var tok = GetTokenizer();
        if (tok != null)
            ReadFromMemory(tok);

        Redraw();
    }

    // ── Keyboard handling ────────────────────────────────────────────────────

    public bool HandleKeyDown(Key key, KeyModifiers modifiers)
    {
        if (!IsActive) return false;

        if (_prompting)
            return HandlePromptKey(key);

        bool ctrl = modifiers.HasFlag(KeyModifiers.Control) || modifiers.HasFlag(KeyModifiers.Meta);

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

        if (Mode != EditorMode.Edit) return false;

        if (ctrl)
        {
            switch (key)
            {
                case Key.F: StartFind(); return true;
                case Key.G: StartGoToLine(); return true;
                case Key.C: CopyLine(); return true;
                case Key.X: CutLine(); return true;
                case Key.V: PasteLines(); return true;
                case Key.Y: DeleteCurrentLine(); return true;
                case Key.D: DuplicateLine(); return true;
                case Key.Home:
                    _cursorLine = 0;
                    _cursorCol = 0;
                    EnsureVisible();
                    Redraw();
                    return true;
                case Key.End:
                    _cursorLine = Math.Max(0, _lines.Count - 1);
                    _cursorCol = _lines[_cursorLine].Text.Length;
                    EnsureVisible();
                    Redraw();
                    return true;
            }
        }

        switch (key)
        {
            case Key.Up:         MoveCursor(0, -1); return true;
            case Key.Down:       MoveCursor(0, 1); return true;
            case Key.Left:       MoveCursor(-1, 0); return true;
            case Key.Right:      MoveCursor(1, 0); return true;
            case Key.Home:       Home(); return true;
            case Key.End:        End(); return true;
            case Key.PageUp:     PageUp(); return true;
            case Key.PageDown:   PageDown(); return true;
            case Key.Enter:      InsertNewline(); return true;
            case Key.Back:       Backspace(); return true;
            case Key.Delete:     Delete(); return true;
            case Key.Tab:        InsertChar(' '); InsertChar(' '); return true;
            case Key.Insert:     _insertMode = !_insertMode; RedrawStatusBar(); return true;
        }

        return false;
    }

    public void HandleTextInput(string text)
    {
        if (!IsActive || Mode != EditorMode.Edit) return;
        if (_prompting)
        {
            foreach (char ch in text)
            {
                if (ch >= 0x20 && ch <= 0x7E)
                    _promptInput += ch;
            }
            RedrawStatusBar();
            return;
        }
        foreach (char ch in text)
        {
            if (ch >= 0x20 && ch <= 0x7E)
                InsertChar(ch);
        }
    }

    // ── Text buffer operations ────────────────────────────────────────────────

    /// <summary>Load text lines of the form "10 PRINT ..." into the buffer.</summary>
    internal void LoadText(string[] lines)
    {
        _lines.Clear();
        foreach (string raw in lines)
        {
            if (ParseLine(raw, out int lineNum, out string text))
                _lines.Add((lineNum, text));
        }
        if (_lines.Count == 0)
            _lines.Add((10, ""));

        _cursorLine = 0;
        _cursorCol = 0;
        _scrollY = 0;
        _modified = false;
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

    /// <summary>Reassemble "10 PRINT ..." strings from the buffer.</summary>
    internal string[] ToTextLines()
    {
        var result = new string[_lines.Count];
        for (int i = 0; i < _lines.Count; i++)
            result[i] = $"{_lines[i].LineNumber} {_lines[i].Text}";
        return result;
    }

    internal void InsertChar(char ch)
    {
        EnsureLine();
        var (lineNum, text) = _lines[_cursorLine];
        if (_cursorCol > text.Length) _cursorCol = text.Length;

        string newText;
        if (_insertMode || _cursorCol >= text.Length)
        {
            newText = text.Insert(_cursorCol, ch.ToString());
            _cursorCol++;
        }
        else
        {
            // Overwrite
            newText = text[.._cursorCol] + ch + text[(_cursorCol + 1)..];
            _cursorCol++;
        }

        _lines[_cursorLine] = (lineNum, newText);
        _modified = true;
        RedrawCode();
    }

    internal void InsertNewline()
    {
        EnsureLine();
        var (lineNum, text) = _lines[_cursorLine];
        if (_cursorCol > text.Length) _cursorCol = text.Length;

        string before = text[.._cursorCol];
        string after  = text[_cursorCol..];
        _lines[_cursorLine] = (lineNum, before);

        int newLineNum = CalculateNewLineNumber(_cursorLine);
        _lines.Insert(_cursorLine + 1, (newLineNum, after));

        _cursorLine++;
        _cursorCol = 0;
        _modified = true;
        EnsureVisible();
        RedrawCode();
    }

    private const int MaxLineNumber = 63999;

    private int CalculateNewLineNumber(int insertAfter)
    {
        int prev = _lines[insertAfter].LineNumber;
        int next = (insertAfter + 1 < _lines.Count) ? _lines[insertAfter + 1].LineNumber : Math.Min(prev + 20, MaxLineNumber);

        int midpoint = (prev + next) / 2;
        if (midpoint <= prev)
        {
            // No integer gap — renumber entire buffer
            RenumberAll();
            // After renumber, recalculate
            prev = _lines[insertAfter].LineNumber;
            next = (insertAfter + 1 < _lines.Count) ? _lines[insertAfter + 1].LineNumber : Math.Min(prev + 20, MaxLineNumber);
            midpoint = (prev + next) / 2;
        }
        return Math.Min(midpoint, MaxLineNumber);
    }

    private void RenumberAll()
    {
        for (int i = 0; i < _lines.Count; i++)
            _lines[i] = ((i + 1) * 10, _lines[i].Text);
    }

    internal void Backspace()
    {
        EnsureLine();
        var (lineNum, text) = _lines[_cursorLine];
        if (_cursorCol > 0)
        {
            if (_cursorCol <= text.Length)
            {
                _lines[_cursorLine] = (lineNum, text.Remove(_cursorCol - 1, 1));
                _cursorCol--;
                _modified = true;
                RedrawCode();
            }
        }
        else if (_cursorLine > 0)
        {
            // Join with previous line (keep previous line's number)
            var (prevNum, prevText) = _lines[_cursorLine - 1];
            _lines[_cursorLine - 1] = (prevNum, prevText + text);
            _lines.RemoveAt(_cursorLine);
            _cursorLine--;
            _cursorCol = prevText.Length;
            _modified = true;
            EnsureVisible();
            RedrawCode();
        }
    }

    internal void Delete()
    {
        EnsureLine();
        var (lineNum, text) = _lines[_cursorLine];
        if (_cursorCol < text.Length)
        {
            _lines[_cursorLine] = (lineNum, text.Remove(_cursorCol, 1));
            _modified = true;
            RedrawCode();
        }
        else if (_cursorLine < _lines.Count - 1)
        {
            // Join with next line
            var (_, nextText) = _lines[_cursorLine + 1];
            _lines[_cursorLine] = (lineNum, text + nextText);
            _lines.RemoveAt(_cursorLine + 1);
            _modified = true;
            RedrawCode();
        }
    }

    internal void DeleteCurrentLine()
    {
        if (_lines.Count <= 1)
        {
            _lines[0] = (_lines[0].LineNumber, "");
            _cursorCol = 0;
            _modified = true;
            RedrawCode();
            return;
        }

        _lines.RemoveAt(_cursorLine);
        if (_cursorLine >= _lines.Count)
            _cursorLine = _lines.Count - 1;
        _cursorCol = Math.Min(_cursorCol, _lines[_cursorLine].Text.Length);
        _modified = true;
        EnsureVisible();
        RedrawCode();
    }

    private void DuplicateLine()
    {
        EnsureLine();
        var (lineNum, text) = _lines[_cursorLine];
        int newNum = CalculateNewLineNumber(_cursorLine);
        _lines.Insert(_cursorLine + 1, (newNum, text));
        _cursorLine++;
        _modified = true;
        EnsureVisible();
        RedrawCode();
    }

    // ── Clipboard operations ──────────────────────────────────────────────────

    private void CopyLine()
    {
        EnsureLine();
        _clipboard.Clear();
        _clipboard.Add(_lines[_cursorLine].Text);
        ShowMessage("Copied");
    }

    private void CutLine()
    {
        EnsureLine();
        _clipboard.Clear();
        _clipboard.Add(_lines[_cursorLine].Text);
        DeleteCurrentLine();
        ShowMessage("Cut");
    }

    private void PasteLines()
    {
        if (_clipboard.Count == 0)
        {
            ShowMessage("Clipboard empty");
            return;
        }

        foreach (string text in _clipboard)
        {
            int newNum = CalculateNewLineNumber(_cursorLine);
            _lines.Insert(_cursorLine + 1, (newNum, text));
            _cursorLine++;
        }

        _modified = true;
        EnsureVisible();
        RedrawCode();
        ShowMessage($"Pasted {_clipboard.Count} line{(_clipboard.Count > 1 ? "s" : "")}");
    }

    // ── Prompt system ────────────────────────────────────────────────────────

    private void StartPrompt(string prompt, Action<string> callback)
    {
        _prompting = true;
        _promptText = prompt;
        _promptInput = "";
        _promptCallback = callback;
        _message = "";
        RedrawStatusBar();
    }

    private bool HandlePromptKey(Key key)
    {
        switch (key)
        {
            case Key.Enter:
                _prompting = false;
                string input = _promptInput;
                var cb = _promptCallback;
                _promptCallback = null;
                cb?.Invoke(input);
                return true;
            case Key.Escape:
                _prompting = false;
                _promptCallback = null;
                RedrawStatusBar();
                return true;
            case Key.Back:
                if (_promptInput.Length > 0)
                    _promptInput = _promptInput[..^1];
                RedrawStatusBar();
                return true;
        }
        return true; // consume all keys while prompting
    }

    private void ShowMessage(string msg)
    {
        _message = msg;
        RedrawStatusBar();
    }

    // ── Search and Go To ──────────────────────────────────────────────────────

    private void StartFind()
    {
        StartPrompt("Find: ", input =>
        {
            if (string.IsNullOrEmpty(input))
                input = _lastSearch;
            if (string.IsNullOrEmpty(input))
            {
                ShowMessage("No search term");
                return;
            }
            _lastSearch = input;
            FindNext(input);
        });
    }

    private void FindNext(string term)
    {
        // Search forward from current position
        for (int i = _cursorLine; i < _lines.Count; i++)
        {
            int startCol = (i == _cursorLine) ? _cursorCol + 1 : 0;
            int idx = _lines[i].Text.IndexOf(term, startCol, StringComparison.OrdinalIgnoreCase);
            if (idx >= 0)
            {
                _cursorLine = i;
                _cursorCol = idx;
                EnsureVisible();
                RedrawCode();
                ShowMessage($"Found at line {_lines[i].LineNumber}");
                return;
            }
        }
        // Wrap around from beginning
        for (int i = 0; i <= _cursorLine; i++)
        {
            int endCol = (i == _cursorLine) ? _cursorCol : _lines[i].Text.Length;
            int idx = _lines[i].Text.IndexOf(term, 0, Math.Min(endCol + 1, _lines[i].Text.Length), StringComparison.OrdinalIgnoreCase);
            if (idx >= 0)
            {
                _cursorLine = i;
                _cursorCol = idx;
                EnsureVisible();
                RedrawCode();
                ShowMessage($"Found at line {_lines[i].LineNumber} (wrapped)");
                return;
            }
        }
        ShowMessage("Not found");
    }

    private void StartGoToLine()
    {
        StartPrompt("Go to line: ", input =>
        {
            if (!int.TryParse(input, out int lineNum))
            {
                ShowMessage("Invalid line number");
                return;
            }
            for (int i = 0; i < _lines.Count; i++)
            {
                if (_lines[i].LineNumber == lineNum)
                {
                    _cursorLine = i;
                    _cursorCol = 0;
                    EnsureVisible();
                    RedrawCode();
                    ShowMessage("");
                    return;
                }
            }
            ShowMessage("Line not found");
        });
    }

    // ── Cursor movement ───────────────────────────────────────────────────────

    internal void MoveCursor(int dx, int dy)
    {
        if (dy != 0)
        {
            _cursorLine = Math.Clamp(_cursorLine + dy, 0, _lines.Count - 1);
            _cursorCol = Math.Min(_cursorCol, _lines[_cursorLine].Text.Length);
        }

        if (dx != 0)
        {
            _cursorCol += dx;
            if (_cursorCol < 0)
            {
                if (_cursorLine > 0)
                {
                    _cursorLine--;
                    _cursorCol = _lines[_cursorLine].Text.Length;
                }
                else
                {
                    _cursorCol = 0;
                }
            }
            else if (_cursorCol > _lines[_cursorLine].Text.Length)
            {
                if (_cursorLine < _lines.Count - 1)
                {
                    _cursorLine++;
                    _cursorCol = 0;
                }
                else
                {
                    _cursorCol = _lines[_cursorLine].Text.Length;
                }
            }
        }

        EnsureVisible();
        UpdateCursor();
    }

    internal void Home()
    {
        _cursorCol = 0;
        EnsureVisible();
        UpdateCursor();
    }

    internal void End()
    {
        _cursorCol = _lines[_cursorLine].Text.Length;
        EnsureVisible();
        UpdateCursor();
    }

    internal void PageUp()
    {
        _cursorLine = Math.Max(0, _cursorLine - VisibleLines);
        EnsureVisible();
        RedrawCode();
    }

    internal void PageDown()
    {
        _cursorLine = Math.Min(_lines.Count - 1, _cursorLine + VisibleLines);
        EnsureVisible();
        RedrawCode();
    }

    private void EnsureVisible()
    {
        if (_cursorLine < _scrollY)
            _scrollY = _cursorLine;
        else if (_cursorLine >= _scrollY + VisibleLines)
            _scrollY = _cursorLine - VisibleLines + 1;
    }

    private void EnsureLine()
    {
        if (_lines.Count == 0)
            _lines.Add((10, ""));
    }

    // ── BASIC memory round-trip ───────────────────────────────────────────────

    /// <summary>
    /// Read the current BASIC program from CPU RAM using the start/end pointers
    /// stored at $002B/$002C (program start) and $002D/$002E (program end).
    /// </summary>
    internal void ReadFromMemory(BasicTokenizer tokenizer)
    {
        // EhBASIC stores program start pointer at $002B/$002C (LSTROM in ehbasic)
        // and program end at $002D/$002E (ENDPROG).
        // The actual RAM start for EhBASIC programs is $0280 (BasicBase).
        ushort progStart = (ushort)(VgcConstants.BasicBase);

        // Read end pointer from zero-page ($002D/$002E)
        byte endL = _bus.Read(0x002D);
        byte endH = _bus.Read(0x002E);
        ushort progEnd = (ushort)(endL | (endH << 8));

        if (progEnd <= progStart)
        {
            // No program in memory — start fresh
            _lines.Clear();
            _lines.Add((10, ""));
            _cursorLine = 0;
            _cursorCol = 0;
            _scrollY = 0;
            _modified = false;
            return;
        }

        int length = progEnd - progStart;
        var data = new byte[length];
        for (int i = 0; i < length; i++)
            data[i] = _bus.Read((ushort)(progStart + i));

        string[] textLines = tokenizer.Detokenize(data);
        LoadText(textLines);
    }

    /// <summary>
    /// Write the current buffer to CPU RAM as a tokenized BASIC program.
    /// Updates the program end pointer at $002D/$002E.
    /// </summary>
    internal void WriteToMemory(BasicTokenizer tokenizer)
    {
        string[] textLines = ToTextLines();
        ushort baseAddr = (ushort)VgcConstants.BasicBase;
        byte[] data = tokenizer.Tokenize(textLines, baseAddr);

        for (int i = 0; i < data.Length; i++)
            _bus.Write((ushort)(baseAddr + i), data[i]);

        // Update program end pointer ($002D/$002E)
        ushort endAddr = (ushort)(baseAddr + data.Length);
        _bus.Write(0x002D, (byte)(endAddr & 0xFF));
        _bus.Write(0x002E, (byte)(endAddr >> 8));
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

    // ── Rendering ─────────────────────────────────────────────────────────────

    internal void Redraw()
    {
        RedrawTitleBar();
        RedrawCode();
        RedrawStatusBar();
    }

    internal void RedrawTitleBar()
    {
        string left  = "NOVA EDIT";
        string right = "F5:Run  Esc:Exit";
        // Pad to fill 80 cols
        int innerWidth = ScreenCols - left.Length - right.Length;
        if (innerWidth < 1) innerWidth = 1;
        string title = left + new string(' ', innerWidth) + right;

        int col = 0;
        for (int i = 0; i < ScreenCols; i++)
        {
            byte ch = i < title.Length ? (byte)title[i] : (byte)' ';
            WriteChar(col++, TitleRow, ch, TitleFg);
        }
    }

    internal void RedrawCode()
    {
        for (int row = CodeStartRow; row <= CodeEndRow; row++)
        {
            int lineIdx = _scrollY + (row - CodeStartRow);

            // Clear entire row first
            for (int c = 0; c < ScreenCols; c++)
                WriteChar(c, row, (byte)' ', DefaultFg);

            if (lineIdx < _lines.Count)
            {
                var (lineNum, text) = _lines[lineIdx];

                // Draw gutter: right-justified line number + space
                string lineNumStr = lineNum.ToString().PadLeft(5);
                for (int g = 0; g < 5; g++)
                    WriteChar(g, row, (byte)lineNumStr[g], GutterFg);
                WriteChar(5, row, (byte)' ', GutterFg);

                // Draw code with syntax highlighting (clipped to CodeWidth)
                byte[] colors = _highlighter.HighlightLine(text);
                for (int c = 0; c < CodeWidth; c++)
                {
                    byte ch = c < text.Length ? (byte)text[c] : (byte)' ';
                    byte fg = c < colors.Length ? colors[c] : DefaultFg;
                    WriteChar(GutterWidth + c, row, ch, fg);
                }
            }
        }

        UpdateCursor();
    }

    internal void RedrawStatusBar()
    {
        string status;
        if (_prompting)
        {
            status = " " + _promptText + _promptInput + "_";
        }
        else
        {
            int line = _cursorLine + 1;
            int col  = _cursorCol + 1;
            string mode = _insertMode ? "INS" : "OVR";
            string mod = _modified ? " [*]" : "";

            // Compute free memory: space from end of program to $9FFF
            int freeBytes = 0;
            try
            {
                byte endL = _bus.Read(0x002D);
                byte endH = _bus.Read(0x002E);
                ushort progEnd = (ushort)(endL | (endH << 8));
                int available = VgcConstants.BasicEnd - progEnd;
                if (available < 0) available = 0;
                freeBytes = available;
            }
            catch { }

            status = $"Ln {line} Col {col}  {mode}  Free: {freeBytes}{mod}";
            if (!string.IsNullOrEmpty(_message))
                status += "  " + _message;
        }

        // Pad to screen width
        status = status.PadRight(ScreenCols);
        if (status.Length > ScreenCols) status = status[..ScreenCols];

        for (int c = 0; c < ScreenCols; c++)
            WriteChar(c, StatusRow, (byte)status[c], StatusFg);
    }

    private void UpdateCursor()
    {
        int screenCol = GutterWidth + _cursorCol;
        int screenRow = CodeStartRow + (_cursorLine - _scrollY);

        if (screenRow >= CodeStartRow && screenRow <= CodeEndRow &&
            screenCol >= GutterWidth && screenCol < ScreenCols)
        {
            _bus.Write(VgcConstants.RegCursorX, (byte)screenCol);
            _bus.Write(VgcConstants.RegCursorY, (byte)screenRow);
            _bus.Write(VgcConstants.RegCursorEnable, 1);
        }
        else
        {
            _bus.Write(VgcConstants.RegCursorEnable, 0);
        }
    }

    // ── Low-level VGC screen writing ──────────────────────────────────────────

    internal void WriteChar(int col, int row, byte ch, byte fg)
    {
        if (col < 0 || col >= ScreenCols || row < 0 || row >= ScreenRows) return;
        int offset = row * ScreenCols + col;
        _bus.Write((ushort)(VgcConstants.CharRamBase + offset), ch);
        _bus.Write((ushort)(VgcConstants.ColorRamBase + offset), fg);
    }

    internal void ClearScreen()
    {
        for (int i = 0; i < VgcConstants.ScreenSize; i++)
        {
            _bus.Write((ushort)(VgcConstants.CharRamBase + i), (byte)' ');
            _bus.Write((ushort)(VgcConstants.ColorRamBase + i), DefaultFg);
        }
    }
}

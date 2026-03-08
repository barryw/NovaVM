using Avalonia.Input;
using e6502.Avalonia.Hardware;
using KDS.e6502;

namespace e6502.Avalonia.Editor;

/// <summary>
/// Abstract base class for full-screen text editors rendered to VGC character/color RAM.
/// Owns the text buffer, cursor state, movement, rendering, screen save/restore,
/// prompt system, and search.
/// </summary>
public abstract class ScreenTextEditor
{
    // ── Screen layout constants ──────────────────────────────────────────────
    protected const int ScreenCols = 80;
    protected const int ScreenRows = 25;
    protected const int TitleRow = 0;
    protected const int CodeStartRow = 1;
    protected const int CodeEndRow = 22;
    protected const int VisibleLines = CodeEndRow - CodeStartRow + 1; // 22
    protected const int StatusRow = 23;
    protected const int MessageRow = 24;
    protected const int CodeStartCol = 1;
    protected const int BorderCol = 79;
    protected const int CodeWidth = 78;

    // ── Color constants ──────────────────────────────────────────────────────
    protected const byte EditorBg = 6;
    protected const byte BorderFg = 14;
    protected const byte TitleFg = 1;
    protected const byte StatusFg = 14;
    protected const byte MessageFg = 7;
    protected const byte ErrorFg = 2;
    protected const byte CursorFg = 2;
    protected const byte GutterFg = 12;
    protected const byte DefaultFg = 1;
    protected const byte SelectionFg = 3; // cyan for selection highlight

    // ── CP437 box-drawing characters ─────────────────────────────────────────
    protected const byte BoxTL = 0xC9;
    protected const byte BoxTR = 0xBB;
    protected const byte BoxBL = 0xC8;
    protected const byte BoxBR = 0xBC;
    protected const byte BoxH = 0xCD;
    protected const byte BoxV = 0xBA;
    protected const byte BoxML = 0xCC;
    protected const byte BoxMR = 0xB9;

    // ── State ────────────────────────────────────────────────────────────────
    private readonly List<string> _lines = new() { "" };
    private int _cursorLine;
    private int _cursorCol;
    private int _scrollY;
    private int _scrollX;
    private bool _modified;
    private bool _insertMode = true;

    // ── Selection state ──────────────────────────────────────────────────────
    private int _selAnchorLine;
    private int _selAnchorCol;
    private bool _selActive;
    private string _clipboard = "";
    private bool _clipboardIsLine;

    // ── Screen save/restore ──────────────────────────────────────────────────
    private readonly byte[] _savedCharRam = new byte[VgcConstants.ScreenSize];
    private readonly byte[] _savedColorRam = new byte[VgcConstants.ScreenSize];
    private byte _savedBgColor;
    private byte _savedCursorEnable;
    private byte _savedCursorX;
    private byte _savedCursorY;

    // ── Prompt state ─────────────────────────────────────────────────────────
    private bool _promptActive;
    private string _promptLabel = "";
    private string _promptInput = "";
    private Action<string>? _promptCallback;
    private string? _message;
    private bool _messageIsError;

    // ── Search state ─────────────────────────────────────────────────────────
    private string _lastSearchTerm = "";

    protected readonly IBusDevice Bus;

    public EditorMode Mode { get; protected set; } = EditorMode.Edit;
    public bool IsActive { get; protected set; }

    // ── Constructor ──────────────────────────────────────────────────────────

    protected ScreenTextEditor(IBusDevice bus)
    {
        Bus = bus;
    }

    // ── Abstract methods ─────────────────────────────────────────────────────

    protected abstract string GetTitle();
    protected abstract string GetStatusExtra();
    protected abstract string GetHelpText();
    protected abstract ISyntaxHighlighter GetHighlighter();
    protected abstract int GetGutterWidth();
    protected abstract void RenderGutterCell(int lineIndex, int row);
    protected abstract string GetFileExtension();
    protected abstract byte[] SerializeForSave(string filename);
    protected abstract void DeserializeOnLoad(byte[] data, string filename);

    // ── Virtual methods ──────────────────────────────────────────────────────

    protected virtual bool HandleEditorKey(Key key, KeyModifiers modifiers) => false;
    protected virtual void OnActivate() { }
    protected virtual void OnDeactivate() { }
    protected virtual void OnAfterRedrawCode() { }
    protected virtual int GetVisibleLineCount() => VisibleLines;

    // ── Public accessors (internal for testing) ──────────────────────────────

    internal int LineCount => _lines.Count;
    internal string GetLineText(int index) => _lines[index];
    internal IReadOnlyList<string> Lines => _lines;
    internal int CursorLine => _cursorLine;
    internal int CursorCol => _cursorCol;
    internal bool Modified => _modified;
    internal bool InsertMode => _insertMode;
    internal int ScrollX => _scrollX;
    internal int ScrollY => _scrollY;

    internal bool HasSelection => _selActive;

    internal void ClearSelection() => _selActive = false;

    internal void SetCursor(int line, int col)
    {
        _cursorLine = Math.Clamp(line, 0, _lines.Count - 1);
        _cursorCol = Math.Clamp(col, 0, _lines[_cursorLine].Length);
    }

    internal void ClearModified() => _modified = false;
    internal void ToggleInsertMode() => _insertMode = !_insertMode;

    internal void LoadLines(string[] lines)
    {
        _lines.Clear();
        if (lines.Length == 0)
        {
            _lines.Add("");
        }
        else
        {
            _lines.AddRange(lines);
        }
        _cursorLine = 0;
        _cursorCol = 0;
        _scrollY = 0;
        _scrollX = 0;
        _modified = false;
    }

    // ── Activation ───────────────────────────────────────────────────────────

    public void Activate()
    {
        IsActive = true;
        SaveScreen();
        Bus.Write(VgcConstants.RegBgCol, EditorBg);
        ClearScreen();
        OnActivate();
        Redraw();
    }

    public void Deactivate()
    {
        OnDeactivate();
        IsActive = false;
        RestoreScreen();
    }

    // ── Screen save/restore ───────────────────────────────────────────────────

    protected void SaveScreen()
    {
        for (int i = 0; i < VgcConstants.ScreenSize; i++)
        {
            _savedCharRam[i] = Bus.Read((ushort)(VgcConstants.CharRamBase + i));
            _savedColorRam[i] = Bus.Read((ushort)(VgcConstants.ColorRamBase + i));
        }
        _savedBgColor = Bus.Read(VgcConstants.RegBgCol);
        _savedCursorEnable = Bus.Read(VgcConstants.RegCursorEnable);
        _savedCursorX = Bus.Read(VgcConstants.RegCursorX);
        _savedCursorY = Bus.Read(VgcConstants.RegCursorY);
    }

    protected void RestoreScreen()
    {
        for (int i = 0; i < VgcConstants.ScreenSize; i++)
        {
            Bus.Write((ushort)(VgcConstants.CharRamBase + i), _savedCharRam[i]);
            Bus.Write((ushort)(VgcConstants.ColorRamBase + i), _savedColorRam[i]);
        }
        Bus.Write(VgcConstants.RegBgCol, _savedBgColor);
        Bus.Write(VgcConstants.RegCursorEnable, _savedCursorEnable);
        Bus.Write(VgcConstants.RegCursorX, _savedCursorX);
        Bus.Write(VgcConstants.RegCursorY, _savedCursorY);
    }

    // ── Low-level VGC helpers ─────────────────────────────────────────────────

    protected void WriteChar(int col, int row, byte ch, byte fg)
    {
        if (col < 0 || col >= ScreenCols || row < 0 || row >= ScreenRows) return;
        int offset = row * ScreenCols + col;
        Bus.Write((ushort)(VgcConstants.CharRamBase + offset), ch);
        Bus.Write((ushort)(VgcConstants.ColorRamBase + offset), fg);
    }

    protected void ClearScreen()
    {
        for (int i = 0; i < VgcConstants.ScreenSize; i++)
        {
            Bus.Write((ushort)(VgcConstants.CharRamBase + i), (byte)' ');
            Bus.Write((ushort)(VgcConstants.ColorRamBase + i), DefaultFg);
        }
    }

    // ── Box border rendering ──────────────────────────────────────────────────

    protected void DrawBoxTop(int row, string title)
    {
        WriteChar(0, row, BoxTL, BorderFg);
        WriteChar(1, row, BoxH, BorderFg);
        int titleStart = 2;
        for (int i = 0; i < title.Length && titleStart + i < ScreenCols - 2; i++)
            WriteChar(titleStart + i, row, (byte)title[i], TitleFg);
        for (int i = titleStart + title.Length; i < ScreenCols - 1; i++)
            WriteChar(i, row, BoxH, BorderFg);
        WriteChar(ScreenCols - 1, row, BoxTR, BorderFg);
    }

    protected void DrawBoxBottom(int row, string text, byte fg)
    {
        WriteChar(0, row, BoxBL, BorderFg);
        WriteChar(1, row, BoxH, BorderFg);
        int textStart = 2;
        for (int i = 0; i < text.Length && textStart + i < ScreenCols - 2; i++)
            WriteChar(textStart + i, row, (byte)text[i], fg);
        for (int i = textStart + text.Length; i < ScreenCols - 1; i++)
            WriteChar(i, row, BoxH, BorderFg);
        WriteChar(ScreenCols - 1, row, BoxBR, BorderFg);
    }

    protected void DrawBoxMiddle(int row, string text)
    {
        WriteChar(0, row, BoxML, BorderFg);
        WriteChar(1, row, BoxH, BorderFg);
        int textStart = 2;
        for (int i = 0; i < text.Length && textStart + i < ScreenCols - 2; i++)
            WriteChar(textStart + i, row, (byte)text[i], StatusFg);
        for (int i = textStart + text.Length; i < ScreenCols - 1; i++)
            WriteChar(i, row, BoxH, BorderFg);
        WriteChar(ScreenCols - 1, row, BoxMR, BorderFg);
    }

    // ── Main rendering methods ────────────────────────────────────────────────

    public void Redraw()
    {
        RedrawTitleBar();
        RedrawCode();
        RedrawStatusBar();
        RedrawMessageRow();
    }

    protected void RedrawTitleBar()
    {
        DrawBoxTop(TitleRow, GetTitle());
    }

    protected void RedrawCode()
    {
        var highlighter = GetHighlighter();
        int gutterW = GetGutterWidth();

        // Pre-scan lines above scroll position to track block comment state
        highlighter.Reset();
        for (int i = 0; i < _scrollY && i < _lines.Count; i++)
            highlighter.HighlightLine(_lines[i]);

        // Compute selection range (normalized)
        bool hasSel = _selActive;
        int selStartLine = 0, selStartCol = 0, selEndLine = 0, selEndCol = 0;
        if (hasSel)
            GetSelectionRange(out selStartLine, out selStartCol, out selEndLine, out selEndCol);

        for (int row = CodeStartRow; row <= CodeEndRow; row++)
        {
            // Left border
            WriteChar(0, row, BoxV, BorderFg);

            int lineIdx = _scrollY + (row - CodeStartRow);

            if (lineIdx < _lines.Count)
            {
                // Gutter
                RenderGutterCell(lineIdx, row);

                string line = _lines[lineIdx];
                byte[] colors = highlighter.HighlightLine(line);

                // Code columns
                for (int c = 0; c < CodeWidth - gutterW; c++)
                {
                    int srcCol = c + _scrollX;
                    byte ch = srcCol < line.Length ? (byte)line[srcCol] : (byte)' ';
                    byte fg = srcCol < colors.Length ? colors[srcCol] : DefaultFg;

                    // Selection override
                    if (hasSel && IsInSelection(lineIdx, srcCol, selStartLine, selStartCol, selEndLine, selEndCol))
                        fg = SelectionFg;

                    WriteChar(CodeStartCol + gutterW + c, row, ch, fg);
                }
            }
            else
            {
                // Empty row below content — include left border
                WriteChar(0, row, BoxV, BorderFg);
                for (int c = CodeStartCol; c < BorderCol; c++)
                    WriteChar(c, row, (byte)' ', DefaultFg);
            }

            // Right border
            WriteChar(BorderCol, row, BoxV, BorderFg);
        }

        UpdateCursor();
        OnAfterRedrawCode();
    }

    protected void RedrawStatusBar()
    {
        string ins = _insertMode ? "INS" : "OVR";
        string extra = GetStatusExtra();
        string status = $" Ln {_cursorLine + 1} Col {_cursorCol + 1}  {ins}";
        if (!string.IsNullOrEmpty(extra))
            status += $"  {extra}";
        DrawBoxMiddle(StatusRow, status);
    }

    protected void RedrawMessageRow()
    {
        string text;
        byte fg;

        if (_promptActive)
        {
            text = " " + _promptLabel + _promptInput + "_";
            fg = MessageFg;
        }
        else if (_message != null)
        {
            text = " " + _message;
            fg = _messageIsError ? ErrorFg : MessageFg;
        }
        else
        {
            text = " " + GetHelpText();
            fg = MessageFg;
        }

        DrawBoxBottom(MessageRow, text, fg);
    }

    // ── Cursor rendering ──────────────────────────────────────────────────────

    protected void UpdateCursor()
    {
        int gutterW = GetGutterWidth();
        int screenCol = CodeStartCol + gutterW + (_cursorCol - _scrollX);
        int screenRow = CodeStartRow + (_cursorLine - _scrollY);

        if (screenRow >= CodeStartRow && screenRow <= CodeEndRow &&
            screenCol >= CodeStartCol + gutterW && screenCol < BorderCol)
        {
            Bus.Write(VgcConstants.RegCursorX, (byte)screenCol);
            Bus.Write(VgcConstants.RegCursorY, (byte)screenRow);
            Bus.Write(VgcConstants.RegCursorEnable, 1);

            // Color the cursor cell
            int offset = screenRow * ScreenCols + screenCol;
            Bus.Write((ushort)(VgcConstants.ColorRamBase + offset), CursorFg);
        }
        else
        {
            Bus.Write(VgcConstants.RegCursorEnable, 0);
        }
    }

    // ── Prompt system ─────────────────────────────────────────────────────────

    protected void StartPrompt(string label, Action<string> callback)
    {
        _promptActive = true;
        _promptLabel = label;
        _promptInput = "";
        _promptCallback = callback;
        RedrawMessageRow();
    }

    protected bool HandlePromptKey(Key key, string? text)
    {
        if (!_promptActive) return false;

        if (key == Key.Escape)
        {
            _promptActive = false;
            RedrawMessageRow();
            return true;
        }
        if (key == Key.Enter)
        {
            _promptActive = false;
            var cb = _promptCallback;
            var input = _promptInput;
            _promptCallback = null;
            RedrawMessageRow();
            cb?.Invoke(input);
            return true;
        }
        if (key == Key.Back && _promptInput.Length > 0)
        {
            _promptInput = _promptInput[..^1];
            RedrawMessageRow();
            return true;
        }
        if (text != null && text.Length > 0 && !char.IsControl(text[0]))
        {
            _promptInput += text;
            RedrawMessageRow();
            return true;
        }
        return true;
    }

    protected void ShowMessage(string msg, bool isError = false)
    {
        _message = msg;
        _messageIsError = isError;
        RedrawMessageRow();
    }

    protected void ClearMessage()
    {
        _message = null;
        RedrawMessageRow();
    }

    // ── Search ────────────────────────────────────────────────────────────────

    internal void StartFind()
    {
        StartPrompt("Find: ", term =>
        {
            if (!string.IsNullOrEmpty(term))
            {
                _lastSearchTerm = term;
                FindNext(term);
            }
        });
    }

    internal void FindNext(string term)
    {
        int startLine = _cursorLine;
        int startCol = _cursorCol + 1;

        for (int i = 0; i < _lines.Count; i++)
        {
            int lineIdx = (startLine + i) % _lines.Count;
            string line = _lines[lineIdx];
            int searchFrom = (i == 0) ? Math.Min(startCol, line.Length) : 0;
            int found = line.IndexOf(term, searchFrom, StringComparison.OrdinalIgnoreCase);
            if (found >= 0)
            {
                _selAnchorLine = lineIdx;
                _selAnchorCol = found;
                _selActive = true;
                _cursorLine = lineIdx;
                _cursorCol = found + term.Length;
                EnsureVisible();
                RedrawCode();
                RedrawStatusBar();
                return;
            }
        }
        ShowMessage("Not found: " + term, true);
    }

    internal void StartGoToLine()
    {
        StartPrompt("Go to line: ", input =>
        {
            if (int.TryParse(input, out int lineNum))
            {
                lineNum = Math.Clamp(lineNum, 1, _lines.Count);
                SetCursor(lineNum - 1, 0);
                EnsureVisible();
                RedrawCode();
                RedrawStatusBar();
            }
        });
    }

    // ── Text buffer operations ───────────────────────────────────────────────

    internal void InsertChar(char ch)
    {
        if (_selActive) DeleteSelection();
        EnsureLine();
        string line = _lines[_cursorLine];
        if (_cursorCol > line.Length) _cursorCol = line.Length;

        if (_insertMode || _cursorCol >= line.Length)
        {
            _lines[_cursorLine] = line.Insert(_cursorCol, ch.ToString());
            _cursorCol++;
        }
        else
        {
            // Overwrite mode
            _lines[_cursorLine] = line[.._cursorCol] + ch + line[(_cursorCol + 1)..];
            _cursorCol++;
        }

        _modified = true;
        RedrawCode();
        RedrawStatusBar();
    }

    internal void InsertNewline()
    {
        EnsureLine();
        string line = _lines[_cursorLine];
        if (_cursorCol > line.Length) _cursorCol = line.Length;

        string before = line[.._cursorCol];
        string after = line[_cursorCol..];
        _lines[_cursorLine] = before;
        _lines.Insert(_cursorLine + 1, after);

        _cursorLine++;
        _cursorCol = 0;
        _modified = true;
        EnsureVisible();
        RedrawCode();
        RedrawStatusBar();
    }

    internal void Backspace()
    {
        if (_selActive)
        {
            DeleteSelection();
            RedrawCode();
            RedrawStatusBar();
            return;
        }
        if (_cursorCol > 0)
        {
            string line = _lines[_cursorLine];
            if (_cursorCol <= line.Length)
            {
                _lines[_cursorLine] = line.Remove(_cursorCol - 1, 1);
                _cursorCol--;
                _modified = true;
                RedrawCode();
            }
        }
        else if (_cursorLine > 0)
        {
            // Join with previous line
            string current = _lines[_cursorLine];
            _lines.RemoveAt(_cursorLine);
            _cursorLine--;
            _cursorCol = _lines[_cursorLine].Length;
            _lines[_cursorLine] += current;
            _modified = true;
            EnsureVisible();
            RedrawCode();
        }
    }

    internal void Delete()
    {
        if (_selActive)
        {
            DeleteSelection();
            RedrawCode();
            RedrawStatusBar();
            return;
        }
        string line = _lines[_cursorLine];
        if (_cursorCol < line.Length)
        {
            _lines[_cursorLine] = line.Remove(_cursorCol, 1);
            _modified = true;
            RedrawCode();
        }
        else if (_cursorLine < _lines.Count - 1)
        {
            // Join with next line
            _lines[_cursorLine] += _lines[_cursorLine + 1];
            _lines.RemoveAt(_cursorLine + 1);
            _modified = true;
            RedrawCode();
        }
    }

    internal void DeleteCurrentLine()
    {
        if (_lines.Count <= 1)
        {
            _lines[0] = "";
            _cursorCol = 0;
            _modified = true;
            RedrawCode();
            RedrawStatusBar();
            return;
        }

        _lines.RemoveAt(_cursorLine);
        if (_cursorLine >= _lines.Count)
            _cursorLine = _lines.Count - 1;
        _cursorCol = Math.Min(_cursorCol, _lines[_cursorLine].Length);
        _modified = true;
        EnsureVisible();
        RedrawCode();
        RedrawStatusBar();
    }

    internal void DuplicateLine()
    {
        EnsureLine();
        string text = _lines[_cursorLine];
        _lines.Insert(_cursorLine + 1, text);
        _cursorLine++;
        _modified = true;
        EnsureVisible();
        RedrawCode();
        RedrawStatusBar();
    }

    // ── Cursor movement ──────────────────────────────────────────────────────

    internal void MoveCursorWithSelection(int dx, int dy)
    {
        if (!_selActive)
        {
            _selAnchorLine = _cursorLine;
            _selAnchorCol = _cursorCol;
            _selActive = true;
        }
        MoveCursor(dx, dy);
        // Re-activate because MoveCursor clears it
        _selActive = true;
    }

    internal void SelectToHome()
    {
        if (!_selActive)
        {
            _selAnchorLine = _cursorLine;
            _selAnchorCol = _cursorCol;
        }
        Home();
        _selActive = true;
    }

    internal void SelectToEnd()
    {
        if (!_selActive)
        {
            _selAnchorLine = _cursorLine;
            _selAnchorCol = _cursorCol;
        }
        End();
        _selActive = true;
    }

    internal string GetSelectedText()
    {
        if (!_selActive) return "";
        GetSelectionRange(out int startLine, out int startCol, out int endLine, out int endCol);
        if (startLine == endLine)
            return _lines[startLine][startCol..endCol];

        var sb = new System.Text.StringBuilder();
        sb.Append(_lines[startLine][startCol..]);
        for (int i = startLine + 1; i < endLine; i++)
        {
            sb.Append('\n');
            sb.Append(_lines[i]);
        }
        sb.Append('\n');
        sb.Append(_lines[endLine][..endCol]);
        return sb.ToString();
    }

    private void GetSelectionRange(out int startLine, out int startCol, out int endLine, out int endCol)
    {
        if (_selAnchorLine < _cursorLine || (_selAnchorLine == _cursorLine && _selAnchorCol <= _cursorCol))
        {
            startLine = _selAnchorLine;
            startCol = _selAnchorCol;
            endLine = _cursorLine;
            endCol = _cursorCol;
        }
        else
        {
            startLine = _cursorLine;
            startCol = _cursorCol;
            endLine = _selAnchorLine;
            endCol = _selAnchorCol;
        }
    }

    private static bool IsInSelection(int lineIdx, int col, int selStartLine, int selStartCol, int selEndLine, int selEndCol)
    {
        if (lineIdx < selStartLine || lineIdx > selEndLine) return false;
        if (lineIdx == selStartLine && lineIdx == selEndLine)
            return col >= selStartCol && col < selEndCol;
        if (lineIdx == selStartLine)
            return col >= selStartCol;
        if (lineIdx == selEndLine)
            return col < selEndCol;
        return true;
    }

    internal void DeleteSelection()
    {
        if (!_selActive) return;
        GetSelectionRange(out int startLine, out int startCol, out int endLine, out int endCol);

        if (startLine == endLine)
        {
            _lines[startLine] = _lines[startLine][..startCol] + _lines[startLine][endCol..];
        }
        else
        {
            string before = _lines[startLine][..startCol];
            string after = _lines[endLine][endCol..];
            _lines[startLine] = before + after;
            _lines.RemoveRange(startLine + 1, endLine - startLine);
        }

        _cursorLine = startLine;
        _cursorCol = startCol;
        _selActive = false;
        _modified = true;
    }

    internal void CopySelection()
    {
        if (_selActive)
        {
            _clipboard = GetSelectedText();
            _clipboardIsLine = false;
        }
        else
        {
            _clipboard = _lines[_cursorLine];
            _clipboardIsLine = true;
        }
    }

    internal void CutSelection()
    {
        if (_selActive)
        {
            _clipboard = GetSelectedText();
            _clipboardIsLine = false;
            DeleteSelection();
            RedrawCode();
            RedrawStatusBar();
        }
        else
        {
            _clipboard = _lines[_cursorLine];
            _clipboardIsLine = true;
            DeleteCurrentLine();
        }
    }

    internal void PasteClipboard()
    {
        if (string.IsNullOrEmpty(_clipboard)) return;

        if (_selActive) DeleteSelection();

        if (_clipboardIsLine)
        {
            _lines.Insert(_cursorLine + 1, _clipboard);
            _cursorLine++;
            _cursorCol = 0;
            _modified = true;
            EnsureVisible();
            RedrawCode();
            RedrawStatusBar();
            return;
        }

        string[] pasteLines = _clipboard.Split('\n');
        if (pasteLines.Length == 1)
        {
            string line = _lines[_cursorLine];
            _lines[_cursorLine] = line[.._cursorCol] + pasteLines[0] + line[_cursorCol..];
            _cursorCol += pasteLines[0].Length;
        }
        else
        {
            string currentLine = _lines[_cursorLine];
            string before = currentLine[.._cursorCol];
            string after = currentLine[_cursorCol..];

            _lines[_cursorLine] = before + pasteLines[0];
            for (int i = 1; i < pasteLines.Length - 1; i++)
                _lines.Insert(_cursorLine + i, pasteLines[i]);
            _lines.Insert(_cursorLine + pasteLines.Length - 1, pasteLines[^1] + after);

            _cursorLine += pasteLines.Length - 1;
            _cursorCol = pasteLines[^1].Length;
        }

        _modified = true;
        EnsureVisible();
        RedrawCode();
        RedrawStatusBar();
    }

    internal void MoveCursor(int dx, int dy)
    {
        _selActive = false;
        if (dy != 0)
        {
            _cursorLine = Math.Clamp(_cursorLine + dy, 0, _lines.Count - 1);
            string line = _lines[_cursorLine];
            if (_cursorCol > line.Length)
                _cursorCol = line.Length;
        }

        if (dx != 0)
        {
            _cursorCol += dx;
            if (_cursorCol < 0)
            {
                if (_cursorLine > 0)
                {
                    _cursorLine--;
                    _cursorCol = _lines[_cursorLine].Length;
                }
                else
                {
                    _cursorCol = 0;
                }
            }
            else if (_cursorCol > _lines[_cursorLine].Length)
            {
                if (_cursorLine < _lines.Count - 1)
                {
                    _cursorLine++;
                    _cursorCol = 0;
                }
                else
                {
                    _cursorCol = _lines[_cursorLine].Length;
                }
            }
        }

        EnsureVisible();
    }

    internal void MoveWordRight()
    {
        string line = _lines[_cursorLine];
        if (_cursorCol >= line.Length)
        {
            if (_cursorLine < _lines.Count - 1)
            {
                _cursorLine++;
                _cursorCol = 0;
            }
            EnsureVisible();
            return;
        }
        // Skip current word chars
        while (_cursorCol < line.Length && IsWordChar(line[_cursorCol]))
            _cursorCol++;
        // Skip non-word chars
        while (_cursorCol < line.Length && !IsWordChar(line[_cursorCol]))
            _cursorCol++;
        EnsureVisible();
    }

    internal void MoveWordLeft()
    {
        if (_cursorCol <= 0)
        {
            if (_cursorLine > 0)
            {
                _cursorLine--;
                _cursorCol = _lines[_cursorLine].Length;
            }
            EnsureVisible();
            return;
        }
        string line = _lines[_cursorLine];
        _cursorCol--;
        // Skip non-word chars
        while (_cursorCol > 0 && !IsWordChar(line[_cursorCol]))
            _cursorCol--;
        // Skip word chars
        while (_cursorCol > 0 && IsWordChar(line[_cursorCol - 1]))
            _cursorCol--;
        EnsureVisible();
    }

    internal void Home()
    {
        _cursorCol = 0;
        EnsureVisible();
    }

    internal void End()
    {
        _cursorCol = _lines[_cursorLine].Length;
        EnsureVisible();
    }

    internal void PageUp()
    {
        int vis = GetVisibleLineCount();
        _cursorLine = Math.Max(0, _cursorLine - vis);
        EnsureVisible();
        RedrawCode();
    }

    internal void PageDown()
    {
        int vis = GetVisibleLineCount();
        _cursorLine = Math.Min(_lines.Count - 1, _cursorLine + vis);
        EnsureVisible();
        RedrawCode();
    }

    internal void EnsureVisible()
    {
        int vis = GetVisibleLineCount();
        if (_cursorLine < _scrollY)
            _scrollY = _cursorLine;
        else if (_cursorLine >= _scrollY + vis)
            _scrollY = _cursorLine - vis + 1;

        int visibleWidth = CodeWidth - GetGutterWidth();
        if (_cursorCol < _scrollX)
            _scrollX = _cursorCol;
        else if (_cursorCol >= _scrollX + visibleWidth)
            _scrollX = _cursorCol - visibleWidth + 1;
    }

    // ── Helpers ──────────────────────────────────────────────────────────────

    private void EnsureLine()
    {
        if (_lines.Count == 0) _lines.Add("");
    }

    protected static bool IsWordChar(char c) => char.IsLetterOrDigit(c) || c == '_';
}

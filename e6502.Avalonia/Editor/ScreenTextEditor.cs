using Avalonia.Input;
using KDS.e6502;

namespace e6502.Avalonia.Editor;

/// <summary>
/// Abstract base class for full-screen text editors rendered to VGC character/color RAM.
/// Owns the text buffer, cursor state, and movement logic. Rendering and keyboard
/// dispatch are added in later tasks.
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

    /// <summary>Placeholder — real rendering added in Task 4.</summary>
    protected virtual void RedrawCode() { }

    /// <summary>Placeholder — real rendering added in Task 4.</summary>
    protected virtual void RedrawStatusBar() { }

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

    // ── Text buffer operations ───────────────────────────────────────────────

    internal void InsertChar(char ch)
    {
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

    internal void MoveCursor(int dx, int dy)
    {
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

        if (_cursorCol < _scrollX)
            _scrollX = _cursorCol;
        else if (_cursorCol >= _scrollX + CodeWidth)
            _scrollX = _cursorCol - CodeWidth + 1;
    }

    // ── Helpers ──────────────────────────────────────────────────────────────

    private void EnsureLine()
    {
        if (_lines.Count == 0) _lines.Add("");
    }

    protected static bool IsWordChar(char c) => char.IsLetterOrDigit(c) || c == '_';
}

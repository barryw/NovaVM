using Avalonia.Input;
using e6502.Avalonia.Compiler;
using e6502.Avalonia.Debugging;
using e6502.Avalonia.Hardware;
using KDS.e6502;

namespace e6502.Avalonia.Editor;

public enum EditorMode { Edit, FileBrowser, Running, Debug }

/// <summary>
/// Full-screen NCC source code editor rendered to VGC character/color RAM.
/// Pauses the CPU when active. Compiles via NccCompiler (C# call).
/// </summary>
public sealed class NccEditor
{
    // ── Screen layout ────────────────────────────────────────────────────────
    private const int ScreenCols = VgcConstants.ScreenCols;  // 80
    private const int ScreenRows = VgcConstants.ScreenRows;  // 25
    private const int TitleRow = 0;
    private const int CodeStartRow = 1;
    private const int CodeEndRow = 22;        // rows 1-22 = 22 visible lines
    private const int VisibleLines = CodeEndRow - CodeStartRow + 1;  // 22
    private const int StatusRow = 23;
    private const int MessageRow = 24;

    // Border layout — CP437 double-line box
    private const int CodeStartCol = 1;       // left border at col 0
    private const int CodeWidth = 78;         // cols 1-78 for code
    private const int BorderCol = 79;         // right border at col 79

    // CP437 box-drawing characters
    private const byte BoxTL = 0xC9;   // ╔
    private const byte BoxTR = 0xBB;   // ╗
    private const byte BoxBL = 0xC8;   // ╚
    private const byte BoxBR = 0xBC;   // ╝
    private const byte BoxH  = 0xCD;   // ═
    private const byte BoxV  = 0xBA;   // ║
    private const byte BoxML = 0xCC;   // ╠
    private const byte BoxMR = 0xB9;   // ╣

    // Debug layout
    private const int DebugCodeEndRow = 18;   // rows 1-18 source with gutter
    private const int DebugVisibleLines = DebugCodeEndRow - CodeStartRow + 1; // 18
    private const int DebugSepRow = 19;
    private const int DebugWatchStart = 20;   // rows 20-22
    private const int DebugWatchEnd = 22;
    private const int DebugStatusRow = 23;
    private const int DebugKeyRow = 24;
    private const int GutterWidth = 5;        // "123 > " or "123 * "

    // ── Colors ───────────────────────────────────────────────────────────────
    private const byte EditorBg = 6;    // blue background
    private const byte BorderFg = 14;   // light blue borders
    private const byte TitleFg = 1;     // white
    private const byte StatusFg = 14;   // light blue
    private const byte MessageFg = 7;   // yellow
    private const byte ErrorFg = 2;     // red
    private const byte GutterFg = 12;   // grey
    private const byte BreakpointFg = 2; // red
    private const byte CurrentLineFg = 5; // green
    private const byte WatchFg = 3;     // cyan
    private const byte DefaultFg = 1;   // white

    // ── State ────────────────────────────────────────────────────────────────
    private readonly List<string> _lines = new() { "" };
    private int _cursorLine;    // 0-based line in buffer
    private int _cursorCol;     // 0-based column
    private int _scrollY;       // first visible line index
    private bool _modified;
    private string _filename = "untitled";
    private string _message = "";
    private string _statusExtra = "";

    // External references
    private readonly IBusDevice _bus;
    private readonly DebuggerService _debugger;
    private readonly Cpu _cpu;
    private readonly SyntaxHighlighter _highlighter = new();

    // Editor state
    public EditorMode Mode { get; private set; } = EditorMode.Edit;
    public bool IsActive { get; private set; }

    // Compile state
    private CompileResult? _lastCompileResult;
    private Dictionary<int, ushort>? _lineMap;

    // Debug state
    private readonly HashSet<int> _breakpointLines = new();
    private bool _isRunning;

    // File browser state
    private List<string> _fileList = new();
    private int _fileListIndex;

    // Input prompt state
    private bool _prompting;
    private string _promptText = "";
    private string _promptInput = "";
    private Action<string>? _promptCallback;

    // Exit confirmation state
    private bool _exitPromptShowing;

    // ── Constructor ──────────────────────────────────────────────────────────

    public NccEditor(IBusDevice bus, DebuggerService debugger, Cpu cpu)
    {
        _bus = bus;
        _debugger = debugger;
        _cpu = cpu;
    }

    // ── Properties for external polling ──────────────────────────────────────

    public bool IsRunning => _isRunning;

    // ── Activation ───────────────────────────────────────────────────────────

    public void Activate()
    {
        if (IsActive) return;
        IsActive = true;
        _exitPromptShowing = false;
        _debugger.Pause();
        // Disable VGC cursor (we draw our own via color inversion)
        _bus.Write(VgcConstants.RegCursorEnable, 0);
        // Set blue background (TurboC style)
        _bus.Write(VgcConstants.RegBgCol, EditorBg);
        ClearScreen();
        Redraw();
    }

    public void Deactivate()
    {
        if (!IsActive) return;
        IsActive = false;
        // Restore black background for BASIC
        _bus.Write(VgcConstants.RegBgCol, 0);
        // Clear screen so BASIC starts fresh
        ClearScreen();
        // Re-enable VGC cursor
        _bus.Write(VgcConstants.RegCursorEnable, 1);
        // Reboot the CPU — cold-starts BASIC with header and Ready prompt
        _cpu.Boot();
        _debugger.Resume();
    }

    // ── Input handling ──────────────────────────────────────────────────────

    public bool HandleKeyDown(Key key, KeyModifiers modifiers)
    {
        if (!IsActive) return false;

        if (_prompting)
            return HandlePromptKey(key, modifiers);

        if (Mode == EditorMode.FileBrowser)
            return HandleFileBrowserKey(key);

        bool ctrl = modifiers.HasFlag(KeyModifiers.Control) || modifiers.HasFlag(KeyModifiers.Meta);

        // Handle Y/N response to exit prompt
        if (_exitPromptShowing)
        {
            if (key == Key.Y)
            {
                _exitPromptShowing = false;
                SetMessage("");
                Deactivate();
                return true;
            }
            // Any other key cancels
            _exitPromptShowing = false;
            SetMessage("");
            RedrawMessageRow();
            return true;
        }

        // Escape: cancel sub-modes, but NOT exit to BASIC
        if (key == Key.Escape)
        {
            if (Mode == EditorMode.Running)
            {
                StopRunning();
                return true;
            }
            if (Mode == EditorMode.Debug)
            {
                ExitDebugMode();
                return true;
            }
            // In edit mode, Escape clears any message
            SetMessage("");
            RedrawMessageRow();
            return true;
        }

        // Ctrl+Q: deliberate exit to BASIC (with confirmation if modified)
        if (ctrl && key == Key.Q)
        {
            if (_modified)
            {
                _exitPromptShowing = true;
                SetMessage("UNSAVED CHANGES! EXIT TO BASIC? (Y/N) ");
                RedrawMessageRow();
            }
            else
            {
                _exitPromptShowing = true;
                SetMessage("EXIT TO BASIC? (Y/N) ");
                RedrawMessageRow();
            }
            return true;
        }

        // Edit mode keys
        if (Mode == EditorMode.Edit)
        {
            if (ctrl)
            {
                switch (key)
                {
                    case Key.S: SaveFile(); return true;
                    case Key.O: PromptForFile(); return true;
                    case Key.N: NewFile(); return true;
                }
            }

            switch (key)
            {
                case Key.F3: OpenFileBrowser(); return true;
                case Key.F5: CompileAndRun(); return true;
                case Key.F6: EnterDebugMode(); return true;
                case Key.F9: CompileOnly(); return true;
                case Key.Up: MoveCursor(0, -1, ctrl); return true;
                case Key.Down: MoveCursor(0, 1, ctrl); return true;
                case Key.Left: MoveCursor(-1, 0, ctrl); return true;
                case Key.Right: MoveCursor(1, 0, ctrl); return true;
                case Key.Home: Home(ctrl); return true;
                case Key.End: End(ctrl); return true;
                case Key.PageUp: PageUp(); return true;
                case Key.PageDown: PageDown(); return true;
                case Key.Enter: InsertNewline(); return true;
                case Key.Back: Backspace(); return true;
                case Key.Delete: Delete(); return true;
                case Key.Tab: InsertText("  "); return true;
            }
            // Character keys — let them fall through to TextInput
            return false;
        }

        // Debug mode keys
        if (Mode == EditorMode.Debug)
        {
            switch (key)
            {
                case Key.F2: ToggleBreakpoint(); return true;
                case Key.F5: DebugContinue(); return true;
                case Key.F7: DebugStepInto(); return true;
                case Key.F8: DebugStepLine(); return true;
                case Key.Up: MoveCursor(0, -1, ctrl); return true;
                case Key.Down: MoveCursor(0, 1, ctrl); return true;
                case Key.PageUp: PageUp(); return true;
                case Key.PageDown: PageDown(); return true;
            }
        }

        return false;
    }

    public void HandleTextInput(string text)
    {
        if (!IsActive) return;

        if (_prompting)
        {
            foreach (char ch in text)
            {
                if (ch >= 0x20 && ch <= 0x7E)
                    _promptInput += ch;
            }
            RedrawMessageRow();
            return;
        }

        if (Mode != EditorMode.Edit) return;

        foreach (char ch in text)
        {
            if (ch >= 0x20 && ch <= 0x7E)
                InsertChar(ch);
        }
    }

    // ── Text buffer operations ──────────────────────────────────────────────

    internal void InsertChar(char ch)
    {
        EnsureLine();
        string line = _lines[_cursorLine];
        if (_cursorCol > line.Length) _cursorCol = line.Length;
        _lines[_cursorLine] = line.Insert(_cursorCol, ch.ToString());
        _cursorCol++;
        _modified = true;
        RedrawCode();
    }

    internal void InsertText(string text)
    {
        foreach (char ch in text)
            InsertChar(ch);
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

    // ── Cursor movement ─────────────────────────────────────────────────────

    internal void MoveCursor(int dx, int dy, bool wordJump = false)
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
            if (wordJump)
            {
                if (dx > 0) MoveWordRight();
                else MoveWordLeft();
            }
            else
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
        }

        EnsureVisible();
        UpdateCursor();
    }

    private void MoveWordRight()
    {
        string line = _lines[_cursorLine];
        if (_cursorCol >= line.Length)
        {
            if (_cursorLine < _lines.Count - 1)
            {
                _cursorLine++;
                _cursorCol = 0;
            }
            return;
        }
        // Skip current word chars
        while (_cursorCol < line.Length && IsWordChar(line[_cursorCol]))
            _cursorCol++;
        // Skip whitespace
        while (_cursorCol < line.Length && !IsWordChar(line[_cursorCol]))
            _cursorCol++;
    }

    private void MoveWordLeft()
    {
        if (_cursorCol <= 0)
        {
            if (_cursorLine > 0)
            {
                _cursorLine--;
                _cursorCol = _lines[_cursorLine].Length;
            }
            return;
        }
        string line = _lines[_cursorLine];
        _cursorCol--;
        // Skip whitespace
        while (_cursorCol > 0 && !IsWordChar(line[_cursorCol]))
            _cursorCol--;
        // Skip word chars
        while (_cursorCol > 0 && IsWordChar(line[_cursorCol - 1]))
            _cursorCol--;
    }

    private static bool IsWordChar(char c) => char.IsLetterOrDigit(c) || c == '_';

    internal void Home(bool ctrlHeld)
    {
        if (ctrlHeld) { _cursorLine = 0; _cursorCol = 0; }
        else { _cursorCol = 0; }
        EnsureVisible();
        UpdateCursor();
    }

    internal void End(bool ctrlHeld)
    {
        if (ctrlHeld) { _cursorLine = _lines.Count - 1; }
        _cursorCol = _lines[_cursorLine].Length;
        EnsureVisible();
        UpdateCursor();
    }

    internal void PageUp()
    {
        int vis = Mode == EditorMode.Debug ? DebugVisibleLines : VisibleLines;
        _cursorLine = Math.Max(0, _cursorLine - vis);
        EnsureVisible();
        RedrawCode();
    }

    internal void PageDown()
    {
        int vis = Mode == EditorMode.Debug ? DebugVisibleLines : VisibleLines;
        _cursorLine = Math.Min(_lines.Count - 1, _cursorLine + vis);
        EnsureVisible();
        RedrawCode();
    }

    private void EnsureVisible()
    {
        int vis = Mode == EditorMode.Debug ? DebugVisibleLines : VisibleLines;
        if (_cursorLine < _scrollY)
            _scrollY = _cursorLine;
        else if (_cursorLine >= _scrollY + vis)
            _scrollY = _cursorLine - vis + 1;
    }

    private void EnsureLine()
    {
        if (_lines.Count == 0) _lines.Add("");
    }

    // ── File operations ─────────────────────────────────────────────────────

    private static string ProgramDir =>
        Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.UserProfile), "e6502-programs");

    private void SaveFile()
    {
        Directory.CreateDirectory(ProgramDir);
        string path = Path.Combine(ProgramDir, _filename.EndsWith(".c") ? _filename : _filename + ".c");
        File.WriteAllText(path, string.Join('\n', _lines));
        _modified = false;
        SetMessage($"Saved: {Path.GetFileName(path)}");
        RedrawTitleBar();
    }

    private void PromptForFile()
    {
        StartPrompt("Filename: ", name =>
        {
            if (string.IsNullOrWhiteSpace(name)) return;
            LoadFile(name);
        });
    }

    private void LoadFile(string name)
    {
        if (!name.EndsWith(".c")) name += ".c";
        string path = Path.Combine(ProgramDir, name);
        if (!File.Exists(path))
        {
            SetMessage($"Not found: {name}");
            return;
        }
        string content = File.ReadAllText(path);
        _lines.Clear();
        _lines.AddRange(content.Split('\n'));
        if (_lines.Count == 0) _lines.Add("");
        _filename = name;
        _cursorLine = 0;
        _cursorCol = 0;
        _scrollY = 0;
        _modified = false;
        _highlighter.Reset();
        SetMessage($"Loaded: {name}");
        Redraw();
    }

    private void NewFile()
    {
        _lines.Clear();
        _lines.Add("");
        _filename = "untitled";
        _cursorLine = 0;
        _cursorCol = 0;
        _scrollY = 0;
        _modified = false;
        _highlighter.Reset();
        _lastCompileResult = null;
        _lineMap = null;
        SetMessage("New file");
        Redraw();
    }

    private void OpenFileBrowser()
    {
        Directory.CreateDirectory(ProgramDir);
        _fileList = Directory.GetFiles(ProgramDir, "*.c")
            .Select(Path.GetFileName)
            .Where(f => f != null)
            .Select(f => f!)
            .OrderBy(f => f)
            .ToList();
        if (_fileList.Count == 0)
        {
            SetMessage("No .c files found");
            return;
        }
        _fileListIndex = 0;
        Mode = EditorMode.FileBrowser;
        RedrawFileBrowser();
    }

    private bool HandleFileBrowserKey(Key key)
    {
        switch (key)
        {
            case Key.Escape:
                Mode = EditorMode.Edit;
                Redraw();
                return true;
            case Key.Up:
                _fileListIndex = Math.Max(0, _fileListIndex - 1);
                RedrawFileBrowser();
                return true;
            case Key.Down:
                _fileListIndex = Math.Min(_fileList.Count - 1, _fileListIndex + 1);
                RedrawFileBrowser();
                return true;
            case Key.Enter:
                if (_fileListIndex < _fileList.Count)
                {
                    string name = _fileList[_fileListIndex];
                    Mode = EditorMode.Edit;
                    LoadFile(name);
                }
                return true;
        }
        return true; // Consume all keys in file browser
    }

    // ── Prompt handling ─────────────────────────────────────────────────────

    private void StartPrompt(string prompt, Action<string> callback)
    {
        _prompting = true;
        _promptText = prompt;
        _promptInput = "";
        _promptCallback = callback;
        RedrawMessageRow();
    }

    private bool HandlePromptKey(Key key, KeyModifiers modifiers)
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
                RedrawMessageRow();
                return true;
            case Key.Back:
                if (_promptInput.Length > 0)
                    _promptInput = _promptInput[..^1];
                RedrawMessageRow();
                return true;
        }
        // Let character keys fall through to TextInput for prompt input
        return false;
    }

    // ── Compile & Run ───────────────────────────────────────────────────────

    internal void CompileOnly()
    {
        string source = string.Join('\n', _lines);
        var compiler = new NccCompiler { BaseAddress = VgcConstants.BasicBase };
        _lastCompileResult = compiler.Compile(source);

        if (_lastCompileResult.Success)
        {
            _lineMap = _lastCompileResult.LineMap;  // null until Task 9.1 adds line mapping
            SetMessage($"Compile OK — {_lastCompileResult.Code.Length} bytes");
        }
        else
        {
            _lineMap = null;
            string firstErr = _lastCompileResult.Errors.FirstOrDefault() ?? "Unknown error";
            SetMessage($"Error: {firstErr}");
            // Try to jump to error line
            JumpToErrorLine(firstErr);
        }
        _statusExtra = "";
        RedrawStatusBar();
        RedrawMessageRow();
    }

    internal void CompileAndRun()
    {
        CompileOnly();
        if (_lastCompileResult == null || !_lastCompileResult.Success) return;

        LoadAndRun(_lastCompileResult);
    }

    private void LoadAndRun(CompileResult result)
    {
        // Load code at $0280
        byte[] code = result.Code;
        for (int i = 0; i < code.Length; i++)
            _bus.Write((ushort)(VgcConstants.BasicBase + i), code[i]);

        // Set up halt at $FF00
        _bus.Write(0xFF00, 0x00); // BRK
        _debugger.AddBreakpoint(0xFF00, null);

        // Set CPU state: use Boot to set PC, write SP to stack pointer address
        _cpu.Boot((ushort)VgcConstants.BasicBase);

        // Clear screen for program output
        ClearScreen();
        _bus.Write(VgcConstants.RegCursorEnable, 1);

        _isRunning = true;
        Mode = EditorMode.Running;
        _debugger.Resume();
    }

    /// <summary>Called by MainWindow 60Hz timer when debugger is paused and editor was running.</summary>
    public void StopRunning()
    {
        _isRunning = false;
        _debugger.RemoveBreakpoint(0xFF00);
        _debugger.Pause();
        _bus.Write(VgcConstants.RegCursorEnable, 0);
        Mode = EditorMode.Edit;
        SetMessage("Program ended");
        Redraw();
    }

    // ── Debug mode ──────────────────────────────────────────────────────────

    internal void EnterDebugMode()
    {
        CompileOnly();
        if (_lastCompileResult == null || !_lastCompileResult.Success) return;

        // Load code but don't resume
        byte[] code = _lastCompileResult.Code;
        for (int i = 0; i < code.Length; i++)
            _bus.Write((ushort)(VgcConstants.BasicBase + i), code[i]);

        _cpu.Boot((ushort)VgcConstants.BasicBase);

        // Set breakpoints from editor markers
        _debugger.ClearAllBreakpoints();
        if (_lineMap != null)
        {
            foreach (int line in _breakpointLines)
            {
                if (_lineMap.TryGetValue(line, out ushort addr))
                    _debugger.AddBreakpoint(addr, null);
            }
        }
        // Add halt breakpoint
        _bus.Write(0xFF00, 0x00);
        _debugger.AddBreakpoint(0xFF00, null);

        Mode = EditorMode.Debug;
        Redraw();
    }

    private void ExitDebugMode()
    {
        _debugger.ClearAllBreakpoints();
        _debugger.Pause();
        Mode = EditorMode.Edit;
        Redraw();
    }

    internal void ToggleBreakpoint()
    {
        if (_breakpointLines.Contains(_cursorLine))
        {
            _breakpointLines.Remove(_cursorLine);
            if (_lineMap != null && _lineMap.TryGetValue(_cursorLine, out ushort addr))
                _debugger.RemoveBreakpoint(addr);
        }
        else
        {
            _breakpointLines.Add(_cursorLine);
            if (_lineMap != null && _lineMap.TryGetValue(_cursorLine, out ushort addr))
                _debugger.AddBreakpoint(addr, null);
        }
        RedrawCode();
    }

    internal void DebugContinue()
    {
        _debugger.Resume();
        // 60Hz timer will detect when we pause again at next breakpoint
        SetMessage("Running...");
        RedrawMessageRow();
    }

    internal void DebugStepLine()
    {
        if (_lineMap == null) return;

        int startLine = GetSourceLineFromPC(_cpu.Pc);
        // Step until PC maps to a different source line
        for (int i = 0; i < 1000; i++)
        {
            _debugger.Step();
            int currentLine = GetSourceLineFromPC(_cpu.Pc);
            if (currentLine != startLine || _cpu.Pc == 0xFF00) break;
        }

        // Move cursor to current line
        int newLine = GetSourceLineFromPC(_cpu.Pc);
        if (newLine >= 0)
        {
            _cursorLine = newLine;
            EnsureVisible();
        }
        Redraw();
    }

    internal void DebugStepInto()
    {
        // Same as step line for now
        DebugStepLine();
    }

    private int GetSourceLineFromPC(ushort pc)
    {
        if (_lineMap == null) return -1;
        int bestLine = -1;
        ushort bestAddr = 0;
        foreach (var (line, addr) in _lineMap)
        {
            if (addr <= pc && addr >= bestAddr)
            {
                bestAddr = addr;
                bestLine = line;
            }
        }
        return bestLine;
    }

    /// <summary>Called by the 60Hz timer to check if debug execution hit a breakpoint.</summary>
    public void CheckDebugBreak()
    {
        if (Mode != EditorMode.Debug || !_debugger.IsPaused) return;

        int newLine = GetSourceLineFromPC(_cpu.Pc);
        if (newLine >= 0)
        {
            _cursorLine = newLine;
            EnsureVisible();
        }
        Redraw();
    }

    // ── Error navigation ────────────────────────────────────────────────────

    private void JumpToErrorLine(string errorMsg)
    {
        // Try to extract line number from "line N:" or "(N," patterns
        var match = System.Text.RegularExpressions.Regex.Match(errorMsg, @"(?:line\s+|[\(])(\d+)");
        if (match.Success && int.TryParse(match.Groups[1].Value, out int lineNum))
        {
            _cursorLine = Math.Max(0, lineNum - 1); // 1-based to 0-based
            EnsureVisible();
        }
    }

    // ── Rendering ───────────────────────────────────────────────────────────

    private void SetMessage(string msg) => _message = msg;

    internal void Redraw()
    {
        RedrawTitleBar();
        RedrawCode();
        RedrawStatusBar();
        RedrawMessageRow();
    }

    private void RedrawTitleBar()
    {
        string mod = _modified ? " [modified]" : "";
        string left = $" Nova C Compiler v1.0 - {_filename}{mod} ";
        int freeBytes = 40320;
        string right = $" {freeBytes} bytes free ";
        int fillLen = ScreenCols - 2 - left.Length - right.Length; // -2 for corners
        if (fillLen < 0) fillLen = 0;
        string fill = new string((char)BoxH, fillLen);

        WriteChar(0, TitleRow, BoxTL, BorderFg);
        int col = 1;
        for (int i = 0; i < left.Length && col < BorderCol; i++, col++)
            WriteChar(col, TitleRow, (byte)left[i], TitleFg);
        for (int i = 0; i < fill.Length && col < BorderCol; i++, col++)
            WriteChar(col, TitleRow, BoxH, BorderFg);
        for (int i = 0; i < right.Length && col < BorderCol; i++, col++)
            WriteChar(col, TitleRow, (byte)right[i], TitleFg);
        while (col < BorderCol)
        {
            WriteChar(col, TitleRow, BoxH, BorderFg);
            col++;
        }
        WriteChar(BorderCol, TitleRow, BoxTR, BorderFg);
    }

    internal void RedrawCode()
    {
        _highlighter.Reset();

        bool debugMode = Mode == EditorMode.Debug;
        int endRow = debugMode ? DebugCodeEndRow : CodeEndRow;
        int codeW = debugMode ? CodeWidth - GutterWidth : CodeWidth;

        // Scan from line 0 to _scrollY to track block comment state
        for (int i = 0; i < _scrollY && i < _lines.Count; i++)
            _highlighter.HighlightLine(_lines[i]);

        for (int row = CodeStartRow; row <= endRow; row++)
        {
            // Left border
            WriteChar(0, row, BoxV, BorderFg);

            int lineIdx = _scrollY + (row - CodeStartRow);
            if (lineIdx < _lines.Count)
            {
                string line = _lines[lineIdx];
                byte[] colors = _highlighter.HighlightLine(line);

                if (debugMode)
                {
                    // Draw gutter
                    string lineNum = (lineIdx + 1).ToString().PadLeft(3);
                    char marker = _breakpointLines.Contains(lineIdx) ? '*' : ' ';
                    bool isCurrent = lineIdx == GetSourceLineFromPC(_cpu.Pc);
                    char indicator = isCurrent ? '>' : ' ';
                    string gutter = $"{lineNum}{marker}{indicator}";

                    for (int c = 0; c < GutterWidth && c < gutter.Length; c++)
                    {
                        byte fg = GutterFg;
                        if (c == 3 && marker == '*') fg = BreakpointFg;
                        if (c == 4 && indicator == '>') fg = CurrentLineFg;
                        WriteChar(CodeStartCol + c, row, (byte)gutter[c], fg);
                    }

                    // Draw code with syntax highlighting
                    for (int c = 0; c < codeW; c++)
                    {
                        int srcCol = c;
                        byte ch = srcCol < line.Length ? (byte)line[srcCol] : (byte)' ';
                        byte fg = srcCol < colors.Length ? colors[srcCol] : DefaultFg;
                        WriteChar(CodeStartCol + GutterWidth + c, row, ch, fg);
                    }
                }
                else
                {
                    for (int c = 0; c < CodeWidth; c++)
                    {
                        byte ch = c < line.Length ? (byte)line[c] : (byte)' ';
                        byte fg = c < colors.Length ? colors[c] : DefaultFg;
                        WriteChar(CodeStartCol + c, row, ch, fg);
                    }
                }
            }
            else
            {
                // Empty line below content — blank spaces, no ~ markers
                for (int c = CodeStartCol; c < BorderCol; c++)
                    WriteChar(c, row, (byte)' ', DefaultFg);
            }

            // Right border
            WriteChar(BorderCol, row, BoxV, BorderFg);
        }

        if (debugMode)
        {
            RedrawDebugSeparator();
            RedrawWatchPanel();
        }

        UpdateCursor();
    }

    private void RedrawStatusBar()
    {
        int line = _cursorLine + 1;
        int col = _cursorCol + 1;
        int total = _lines.Count;
        string modeStr = Mode switch
        {
            EditorMode.Debug => "DEBUG",
            EditorMode.Running => "RUNNING",
            _ => "EDIT"
        };
        string status = $" Ln {line}, Col {col} | {total} lines | {modeStr}";
        if (!string.IsNullOrEmpty(_statusExtra))
            status += $" | {_statusExtra}";

        int statusRow = Mode == EditorMode.Debug ? DebugStatusRow : StatusRow;

        WriteChar(0, statusRow, BoxML, BorderFg);
        int c = 1;
        for (int i = 0; i < status.Length && c < BorderCol; i++, c++)
            WriteChar(c, statusRow, (byte)status[i], StatusFg);
        while (c < BorderCol)
        {
            WriteChar(c, statusRow, BoxH, BorderFg);
            c++;
        }
        WriteChar(BorderCol, statusRow, BoxMR, BorderFg);
    }

    private void RedrawMessageRow()
    {
        int msgRow = Mode == EditorMode.Debug ? DebugKeyRow : MessageRow;
        string text;
        if (_prompting)
        {
            text = " " + _promptText + _promptInput + "_";
        }
        else if (_exitPromptShowing)
        {
            text = " " + _message;
        }
        else if (Mode == EditorMode.Debug)
        {
            text = !string.IsNullOrEmpty(_message) ? " " + _message : " F2:Brk F5:Run F7:Into F8:Step Esc:Edit ^Q:BASIC";
        }
        else
        {
            text = !string.IsNullOrEmpty(_message) ? " " + _message : " F3:Open F5:Run F6:Debug F9:Build ^S:Save ^N:New ^Q:BASIC";
        }

        byte fg = _message.StartsWith("Error") ? ErrorFg : MessageFg;

        WriteChar(0, msgRow, BoxBL, BorderFg);
        int c = 1;
        for (int i = 0; i < text.Length && c < BorderCol; i++, c++)
            WriteChar(c, msgRow, (byte)text[i], fg);
        while (c < BorderCol)
        {
            WriteChar(c, msgRow, BoxH, BorderFg);
            c++;
        }
        WriteChar(BorderCol, msgRow, BoxBR, BorderFg);
    }

    private void RedrawFileBrowser()
    {
        ClearScreen();
        // Title bar
        WriteChar(0, TitleRow, BoxTL, BorderFg);
        string title = " Open File ";
        int c = 1;
        for (int i = 0; i < title.Length && c < BorderCol; i++, c++)
            WriteChar(c, TitleRow, (byte)title[i], TitleFg);
        while (c < BorderCol) { WriteChar(c, TitleRow, BoxH, BorderFg); c++; }
        WriteChar(BorderCol, TitleRow, BoxTR, BorderFg);

        for (int i = 0; i < _fileList.Count && i < VisibleLines; i++)
        {
            string prefix = i == _fileListIndex ? "> " : "  ";
            string text = prefix + _fileList[i];
            byte fg = i == _fileListIndex ? (byte)3 : DefaultFg;
            WriteBorderedRow(CodeStartRow + i, text, fg);
        }
        // Blank remaining rows
        for (int r = CodeStartRow + Math.Min(_fileList.Count, VisibleLines); r <= CodeEndRow; r++)
            WriteBorderedRow(r, "", DefaultFg);

        // Status bar
        WriteChar(0, StatusRow, BoxML, BorderFg);
        string status = " Arrows: navigate  Enter: open  Esc: cancel";
        c = 1;
        for (int i = 0; i < status.Length && c < BorderCol; i++, c++)
            WriteChar(c, StatusRow, (byte)status[i], StatusFg);
        while (c < BorderCol) { WriteChar(c, StatusRow, BoxH, BorderFg); c++; }
        WriteChar(BorderCol, StatusRow, BoxMR, BorderFg);

        // Bottom border
        WriteChar(0, MessageRow, BoxBL, BorderFg);
        for (c = 1; c < BorderCol; c++) WriteChar(c, MessageRow, BoxH, BorderFg);
        WriteChar(BorderCol, MessageRow, BoxBR, BorderFg);
    }

    private void RedrawDebugSeparator()
    {
        WriteChar(0, DebugSepRow, BoxML, BorderFg);
        for (int c = 1; c < BorderCol; c++)
            WriteChar(c, DebugSepRow, BoxH, BorderFg);
        WriteChar(BorderCol, DebugSepRow, BoxMR, BorderFg);
    }

    private void RedrawWatchPanel()
    {
        var state = _cpu.GetState();
        string regs = $" A=${state.A:X2} X=${state.X:X2} Y=${state.Y:X2} SP=${state.Sp:X2} PC=${state.Pc:X4}";
        byte p = (byte)(
            (state.Nf ? 0x80 : 0) | (state.Vf ? 0x40 : 0) | 0x20 |
            (state.Df ? 0x08 : 0) | (state.If ? 0x04 : 0) |
            (state.Zf ? 0x02 : 0) | (state.Cf ? 0x01 : 0));
        string flags = $" NV-BDIZC={Convert.ToString(p, 2).PadLeft(8, '0')}";
        regs = regs + "  " + flags;
        WriteBorderedRow(DebugWatchStart, regs, WatchFg);

        // Show variables from symbol table
        string vars = " Vars: ";
        if (_lastCompileResult?.Symbols != null)
        {
            foreach (var (name, sym) in _lastCompileResult.Symbols.Take(8))
            {
                byte val = _bus.Read(sym.Address);
                vars += $"{name}=${val:X2} ";
            }
        }
        WriteBorderedRow(DebugWatchStart + 1, vars, WatchFg);

        string pcLine = $" Source line: {GetSourceLineFromPC(state.Pc) + 1}  PC: ${state.Pc:X4}  {(state.Pc == 0xFF00 ? "HALTED" : "PAUSED")}";
        WriteBorderedRow(DebugWatchStart + 2, pcLine, WatchFg);
    }

    private void UpdateCursor()
    {
        // We use the VGC cursor registers for blinking
        bool debugMode = Mode == EditorMode.Debug;
        int screenCol = debugMode
            ? _cursorCol + GutterWidth + CodeStartCol
            : _cursorCol + CodeStartCol;
        int screenRow = CodeStartRow + (_cursorLine - _scrollY);

        int endRow = debugMode ? DebugCodeEndRow : CodeEndRow;
        if (screenRow >= CodeStartRow && screenRow <= endRow &&
            screenCol >= CodeStartCol && screenCol < BorderCol)
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

    // ── Low-level VGC screen writing ────────────────────────────────────────

    private void WriteChar(int col, int row, byte ch, byte fg, bool invert = false)
    {
        if (col < 0 || col >= ScreenCols || row < 0 || row >= ScreenRows) return;
        int offset = row * ScreenCols + col;
        _bus.Write((ushort)(VgcConstants.CharRamBase + offset), ch);
        _bus.Write((ushort)(VgcConstants.ColorRamBase + offset), invert ? (byte)0 : fg);
    }

    private void WriteRow(int row, string text, byte fg)
    {
        for (int c = 0; c < ScreenCols; c++)
        {
            byte ch = c < text.Length ? (byte)text[c] : (byte)' ';
            WriteChar(c, row, ch, fg);
        }
    }

    /// <summary>Write a content row with left/right box borders.</summary>
    private void WriteBorderedRow(int row, string text, byte fg)
    {
        WriteChar(0, row, BoxV, BorderFg);
        for (int c = CodeStartCol; c < BorderCol; c++)
        {
            int i = c - CodeStartCol;
            byte ch = i < text.Length ? (byte)text[i] : (byte)' ';
            WriteChar(c, row, ch, fg);
        }
        WriteChar(BorderCol, row, BoxV, BorderFg);
    }

    private void ClearScreen()
    {
        for (int i = 0; i < VgcConstants.ScreenSize; i++)
        {
            _bus.Write((ushort)(VgcConstants.CharRamBase + i), (byte)' ');
            _bus.Write((ushort)(VgcConstants.ColorRamBase + i), DefaultFg);
        }
    }

    // ── Public accessors for testing ────────────────────────────────────────

    internal IReadOnlyList<string> Lines => _lines;
    internal int CursorLine => _cursorLine;
    internal int CursorCol => _cursorCol;
    internal int ScrollY => _scrollY;
    internal string Filename => _filename;
    internal bool Modified => _modified;
    internal bool ExitPromptShowing => _exitPromptShowing;
}

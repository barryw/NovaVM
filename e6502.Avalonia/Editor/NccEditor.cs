using Avalonia.Input;
using e6502.Avalonia.Compiler;
using e6502.Avalonia.Debugging;
using e6502.Avalonia.Hardware;
using KDS.e6502;

namespace e6502.Avalonia.Editor;

public enum EditorMode { Edit, FileBrowser, Running, Debug }

/// <summary>
/// Full-screen NCC source code editor rendered to VGC character/color RAM.
/// Inherits common editor mechanics from ScreenTextEditor and adds NCC-specific
/// features: compile/debug/run state, debug gutter, watch panel, and F-key bindings.
/// </summary>
public sealed class NccEditor : ScreenTextEditor
{
    // ── Debug layout constants ────────────────────────────────────────────────
    private const int DebugCodeEndRow = 18;
    private const int DebugVisibleLines = DebugCodeEndRow - CodeStartRow + 1; // 18
    private const int DebugSepRow = 19;
    private const int DebugWatchStart = 20;
    private const int GutterWidth = 5;  // "NNN*>" format

    // ── Debug colors ─────────────────────────────────────────────────────────
    private const byte BreakpointFg = 2;   // red
    private const byte CurrentLineFg = 5;  // green
    private const byte WatchFg = 3;        // cyan

    // ── External references ───────────────────────────────────────────────────
    private readonly DebuggerService _debugger;
    private readonly Cpu _cpu;
    private readonly SyntaxHighlighter _highlighter = new();

    // ── Compile state ─────────────────────────────────────────────────────────
    private CompileResult? _lastCompileResult;
    private Dictionary<int, ushort>? _lineMap;

    // ── Debug state ───────────────────────────────────────────────────────────
    private readonly HashSet<int> _breakpointLines = new();
    private bool _isRunning;

    // ── Exit prompt state (NCC uses Y/N via HandleKeyDown, not the base prompt) ──
    private bool _exitPromptShowing;

    // ── Constructor ───────────────────────────────────────────────────────────

    public NccEditor(IBusDevice bus, DebuggerService debugger, Cpu cpu) : base(bus)
    {
        _debugger = debugger;
        _cpu = cpu;
    }

    // ── Properties for external polling ──────────────────────────────────────

    public bool IsRunning => _isRunning;

    // ── ScreenTextEditor abstract overrides ───────────────────────────────────

    protected override string GetTitle()
    {
        string name = string.IsNullOrEmpty(CurrentFilename) ? "untitled" : CurrentFilename;
        string mod = Modified ? " [modified]" : "";
        return $" Nova C Compiler v1.0 - {name}{mod} ";
    }

    protected override string GetStatusExtra() => Mode switch
    {
        EditorMode.Debug   => "DEBUG",
        EditorMode.Running => "RUNNING",
        _                  => "EDIT"
    };

    protected override string GetHelpText() => Mode == EditorMode.Debug
        ? "F2:Brk F5:Run F7:Into F8:Step Esc:Edit ^Q:BASIC"
        : "F3:Open F5:Run F6:Debug F9:Build ^S:Save ^N:New ^Q:BASIC";

    protected override ISyntaxHighlighter GetHighlighter() => _highlighter;

    protected override int GetGutterWidth() => Mode == EditorMode.Debug ? GutterWidth : 0;

    protected override int GetVisibleLineCount()
        => Mode == EditorMode.Debug ? DebugVisibleLines : base.GetVisibleLineCount();

    protected override string GetFileExtension() => ".c";

    protected override byte[] SerializeForSave(string filename)
        => System.Text.Encoding.UTF8.GetBytes(string.Join('\n', Lines));

    protected override void DeserializeOnLoad(byte[] data, string filename)
    {
        string content = System.Text.Encoding.UTF8.GetString(data);
        LoadLines(content.Split('\n'));
        _highlighter.Reset();
    }

    protected override void OnActivate()
    {
        // Disable VGC cursor (we position it ourselves via cursor registers)
        Bus.Write(VgcConstants.RegCursorEnable, 0);
        _debugger.Pause();
    }

    protected override void OnDeactivate()
    {
        // Restore BASIC environment — RestoreScreen() runs after this hook
        Bus.Write(VgcConstants.RegBgCol, 0);
        Bus.Write(VgcConstants.RegCursorEnable, 1);
        // Cold-start BASIC; BASIC will overwrite whatever RestoreScreen restores
        _cpu.Boot();
        _debugger.Resume();
    }

    protected override void RenderGutterCell(int lineIndex, int row)
    {
        if (Mode != EditorMode.Debug) return;

        string lineNum = (lineIndex + 1).ToString().PadLeft(3);
        char marker    = _breakpointLines.Contains(lineIndex) ? '*' : ' ';
        bool isCurrent = lineIndex == GetSourceLineFromPC(_cpu.Pc);
        char indicator = isCurrent ? '>' : ' ';
        string gutter  = $"{lineNum}{marker}{indicator}";

        for (int c = 0; c < GutterWidth && c < gutter.Length; c++)
        {
            byte fg = GutterFg;
            if (c == 3 && marker    == '*') fg = BreakpointFg;
            if (c == 4 && indicator == '>') fg = CurrentLineFg;
            WriteChar(CodeStartCol + c, row, (byte)gutter[c], fg);
        }
    }

    protected override void OnAfterRedrawCode()
    {
        if (Mode == EditorMode.Debug)
        {
            RedrawDebugSeparator();
            RedrawWatchPanel();
        }
    }

    // ── HandleEditorKey: NCC-specific key handling ────────────────────────────
    //
    // Called by base.HandleKeyDown() before the base's own key processing.
    // Handles the exit-prompt Y/N flow and all compile/debug F-keys.

    protected override bool HandleEditorKey(Key key, KeyModifiers modifiers)
    {
        bool ctrl = modifiers.HasFlag(KeyModifiers.Control) || modifiers.HasFlag(KeyModifiers.Meta);

        // Exit confirmation Y/N — intercept before anything else
        if (_exitPromptShowing)
        {
            _exitPromptShowing = false;
            if (key == Key.Y)
            {
                ClearMessage();
                Deactivate();
            }
            else
            {
                ClearMessage();
            }
            return true;
        }

        // Ctrl+Q: NCC-managed exit prompt (direct Y/N, not the base prompt system)
        if (ctrl && key == Key.Q)
        {
            _exitPromptShowing = true;
            string msg = Modified ? "UNSAVED CHANGES! EXIT TO BASIC? (Y/N) " : "EXIT TO BASIC? (Y/N) ";
            ShowMessage(msg);
            return true;
        }

        if (Mode == EditorMode.Edit)
        {
            switch (key)
            {
                case Key.F5: CompileAndRun();  return true;
                case Key.F6: EnterDebugMode(); return true;
                case Key.F9: CompileOnly();    return true;
            }
        }

        if (Mode == EditorMode.Debug)
        {
            switch (key)
            {
                case Key.Escape: ExitDebugMode();    return true;
                case Key.F2:     ToggleBreakpoint(); return true;
                case Key.F5:     DebugContinue();    return true;
                case Key.F7:     DebugStepInto();    return true;
                case Key.F8:     DebugStepLine();    return true;
            }
        }

        if (Mode == EditorMode.Running && key == Key.Escape)
        {
            StopRunning();
            return true;
        }

        return false;
    }

    // ── Compile & Run ─────────────────────────────────────────────────────────

    internal void CompileOnly()
    {
        string source = string.Join('\n', Lines);
        var compiler = new NccCompiler { BaseAddress = VgcConstants.BasicBase };
        _lastCompileResult = compiler.Compile(source);

        if (_lastCompileResult.Success)
        {
            _lineMap = _lastCompileResult.LineMap;
            ShowMessage($"Compile OK — {_lastCompileResult.Code.Length} bytes");
        }
        else
        {
            _lineMap = null;
            string firstErr = _lastCompileResult.Errors.FirstOrDefault() ?? "Unknown error";
            ShowMessage($"Error: {firstErr}", isError: true);
            JumpToErrorLine(firstErr);
        }
        RedrawStatusBar();
    }

    internal void CompileAndRun()
    {
        CompileOnly();
        if (_lastCompileResult == null || !_lastCompileResult.Success) return;
        LoadAndRun(_lastCompileResult);
    }

    private void LoadAndRun(CompileResult result)
    {
        byte[] code = result.Code;
        for (int i = 0; i < code.Length; i++)
            Bus.Write((ushort)(VgcConstants.BasicBase + i), code[i]);

        Bus.Write(0xFF00, 0x00); // BRK halt sentinel
        _debugger.AddBreakpoint(0xFF00, null);

        _cpu.Boot((ushort)VgcConstants.BasicBase);

        ClearScreen();
        Bus.Write(VgcConstants.RegCursorEnable, 1);

        _isRunning = true;
        Mode = EditorMode.Running;
        _debugger.Resume();
    }

    /// <summary>Called by MainWindow 60Hz timer when debugger pauses while editor is in Running mode.</summary>
    public void StopRunning()
    {
        _isRunning = false;
        _debugger.RemoveBreakpoint(0xFF00);
        _debugger.Pause();
        Bus.Write(VgcConstants.RegCursorEnable, 0);
        Mode = EditorMode.Edit;
        ShowMessage("Program ended");
        Redraw();
    }

    // ── Debug mode ────────────────────────────────────────────────────────────

    internal void EnterDebugMode()
    {
        CompileOnly();
        if (_lastCompileResult == null || !_lastCompileResult.Success) return;

        byte[] code = _lastCompileResult.Code;
        for (int i = 0; i < code.Length; i++)
            Bus.Write((ushort)(VgcConstants.BasicBase + i), code[i]);

        _cpu.Boot((ushort)VgcConstants.BasicBase);

        _debugger.ClearAllBreakpoints();
        if (_lineMap != null)
        {
            foreach (int line in _breakpointLines)
            {
                if (_lineMap.TryGetValue(line, out ushort addr))
                    _debugger.AddBreakpoint(addr, null);
            }
        }
        Bus.Write(0xFF00, 0x00);
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
        int line = CursorLine;
        if (_breakpointLines.Contains(line))
        {
            _breakpointLines.Remove(line);
            if (_lineMap != null && _lineMap.TryGetValue(line, out ushort addr))
                _debugger.RemoveBreakpoint(addr);
        }
        else
        {
            _breakpointLines.Add(line);
            if (_lineMap != null && _lineMap.TryGetValue(line, out ushort addr))
                _debugger.AddBreakpoint(addr, null);
        }
        RedrawCode();
    }

    internal void DebugContinue()
    {
        _debugger.Resume();
        ShowMessage("Running...");
    }

    internal void DebugStepLine()
    {
        if (_lineMap == null) return;

        int startLine = GetSourceLineFromPC(_cpu.Pc);
        for (int i = 0; i < 1000; i++)
        {
            _debugger.Step();
            int currentLine = GetSourceLineFromPC(_cpu.Pc);
            if (currentLine != startLine || _cpu.Pc == 0xFF00) break;
        }

        int newLine = GetSourceLineFromPC(_cpu.Pc);
        if (newLine >= 0)
        {
            SetCursor(newLine, 0);
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
            SetCursor(newLine, 0);
            EnsureVisible();
        }
        Redraw();
    }

    // ── Error navigation ──────────────────────────────────────────────────────

    private void JumpToErrorLine(string errorMsg)
    {
        var match = System.Text.RegularExpressions.Regex.Match(errorMsg, @"(?:line\s+|[\(])(\d+)");
        if (match.Success && int.TryParse(match.Groups[1].Value, out int lineNum))
        {
            SetCursor(Math.Max(0, lineNum - 1), 0); // 1-based to 0-based
            EnsureVisible();
        }
    }

    // ── Debug panel rendering ─────────────────────────────────────────────────

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
        regs += "  " + flags;
        WriteBorderedRow(DebugWatchStart, regs, WatchFg);

        string vars = " Vars: ";
        if (_lastCompileResult?.Symbols != null)
        {
            foreach (var (name, sym) in _lastCompileResult.Symbols.Take(8))
            {
                byte val = Bus.Read(sym.Address);
                vars += $"{name}=${val:X2} ";
            }
        }
        WriteBorderedRow(DebugWatchStart + 1, vars, WatchFg);

        string pcLine = $" Source line: {GetSourceLineFromPC(state.Pc) + 1}  PC: ${state.Pc:X4}  {(state.Pc == 0xFF00 ? "HALTED" : "PAUSED")}";
        WriteBorderedRow(DebugWatchStart + 2, pcLine, WatchFg);
    }

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

    // ── Test accessors ────────────────────────────────────────────────────────
    // Lines, CursorLine, CursorCol, ScrollY, Modified are all inherited from base.

    internal bool ExitPromptShowing => _exitPromptShowing;
}

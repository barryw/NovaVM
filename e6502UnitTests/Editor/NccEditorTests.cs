using e6502.Avalonia.Editor;
using e6502.Avalonia.Debugging;
using e6502.Avalonia.Hardware;
using KDS.e6502;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests.Editor;

[TestClass]
public class NccEditorTests
{
    private BusDevice _bus = null!;
    private Cpu _cpu = null!;
    private DebuggerService _debugger = null!;
    private NccEditor _editor = null!;

    [TestInitialize]
    public void Setup()
    {
        _bus = new BusDevice(new byte[65536]);
        _cpu = new Cpu(_bus, E6502Type.Nmos);
        _cpu.Boot();
        _debugger = new DebuggerService(_cpu, _bus);
        _editor = new NccEditor(_bus, _debugger, _cpu);
    }

    // ── Buffer operations ───────────────────────────────────────────────────

    [TestMethod]
    public void InsertChar_AppendsToLine()
    {
        _editor.InsertChar('H');
        _editor.InsertChar('i');
        Assert.AreEqual("Hi", _editor.Lines[0]);
        Assert.AreEqual(2, _editor.CursorCol);
    }

    [TestMethod]
    public void Backspace_RemovesCharBeforeCursor()
    {
        _editor.InsertChar('A');
        _editor.InsertChar('B');
        _editor.InsertChar('C');
        _editor.Backspace();
        Assert.AreEqual("AB", _editor.Lines[0]);
        Assert.AreEqual(2, _editor.CursorCol);
    }

    [TestMethod]
    public void Delete_RemovesCharAtCursor()
    {
        _editor.InsertChar('A');
        _editor.InsertChar('B');
        _editor.InsertChar('C');
        _editor.MoveCursor(-2, 0);  // cursor at col 1
        _editor.Delete();
        Assert.AreEqual("AC", _editor.Lines[0]);
    }

    [TestMethod]
    public void InsertNewline_SplitsLine()
    {
        _editor.InsertChar('A');
        _editor.InsertChar('B');
        _editor.InsertChar('C');
        _editor.MoveCursor(-1, 0);  // cursor after 'B'
        _editor.InsertNewline();
        Assert.AreEqual(2, _editor.Lines.Count);
        Assert.AreEqual("AB", _editor.Lines[0]);
        Assert.AreEqual("C", _editor.Lines[1]);
        Assert.AreEqual(1, _editor.CursorLine);
        Assert.AreEqual(0, _editor.CursorCol);
    }

    [TestMethod]
    public void Backspace_AtStartOfLine_JoinsWithPrevious()
    {
        _editor.InsertChar('A');
        _editor.InsertNewline();
        _editor.InsertChar('B');
        _editor.Home(false);
        _editor.Backspace();
        Assert.AreEqual(1, _editor.Lines.Count);
        Assert.AreEqual("AB", _editor.Lines[0]);
        Assert.AreEqual(0, _editor.CursorLine);
        Assert.AreEqual(1, _editor.CursorCol);
    }

    [TestMethod]
    public void Delete_AtEndOfLine_JoinsWithNext()
    {
        _editor.InsertChar('A');
        _editor.InsertNewline();
        _editor.InsertChar('B');
        // Go to end of first line
        _editor.MoveCursor(0, -1);
        _editor.End(false);
        _editor.Delete();
        Assert.AreEqual(1, _editor.Lines.Count);
        Assert.AreEqual("AB", _editor.Lines[0]);
    }

    // ── Cursor movement ─────────────────────────────────────────────────────

    [TestMethod]
    public void CursorMovement_ArrowKeys()
    {
        _editor.InsertChar('A');
        _editor.InsertChar('B');
        _editor.InsertNewline();
        _editor.InsertChar('C');

        // At line 1, col 1
        Assert.AreEqual(1, _editor.CursorLine);
        Assert.AreEqual(1, _editor.CursorCol);

        _editor.MoveCursor(0, -1);  // up
        Assert.AreEqual(0, _editor.CursorLine);

        _editor.MoveCursor(0, 1);  // down
        Assert.AreEqual(1, _editor.CursorLine);
    }

    [TestMethod]
    public void Home_MovesToStartOfLine()
    {
        _editor.InsertChar('A');
        _editor.InsertChar('B');
        _editor.Home(false);
        Assert.AreEqual(0, _editor.CursorCol);
    }

    [TestMethod]
    public void End_MovesToEndOfLine()
    {
        _editor.InsertChar('A');
        _editor.InsertChar('B');
        _editor.Home(false);
        _editor.End(false);
        Assert.AreEqual(2, _editor.CursorCol);
    }

    [TestMethod]
    public void CtrlHome_GoesToTopOfFile()
    {
        for (int i = 0; i < 5; i++)
        {
            _editor.InsertChar((char)('A' + i));
            _editor.InsertNewline();
        }
        _editor.Home(true);
        Assert.AreEqual(0, _editor.CursorLine);
        Assert.AreEqual(0, _editor.CursorCol);
    }

    [TestMethod]
    public void CtrlEnd_GoesToBottomOfFile()
    {
        _editor.InsertChar('A');
        _editor.InsertNewline();
        _editor.InsertChar('B');
        _editor.InsertNewline();
        _editor.InsertChar('C');
        _editor.Home(true);
        _editor.End(true);
        Assert.AreEqual(2, _editor.CursorLine);
    }

    // ── Viewport scrolling ──────────────────────────────────────────────────

    [TestMethod]
    public void ScrollY_AdjustsWhenCursorMovesBelowViewport()
    {
        // Add 30 lines (more than 22 visible)
        for (int i = 0; i < 30; i++)
        {
            _editor.InsertText($"line {i}");
            _editor.InsertNewline();
        }
        // Cursor should be at line 30, scrollY should have adjusted
        Assert.IsTrue(_editor.ScrollY > 0);
    }

    [TestMethod]
    public void PageDown_MovesVisibleLines()
    {
        for (int i = 0; i < 50; i++)
        {
            _editor.InsertText($"line {i}");
            _editor.InsertNewline();
        }
        _editor.Home(true);
        Assert.AreEqual(0, _editor.CursorLine);
        _editor.PageDown();
        Assert.AreEqual(22, _editor.CursorLine);
    }

    [TestMethod]
    public void PageUp_MovesBackVisibleLines()
    {
        for (int i = 0; i < 50; i++)
        {
            _editor.InsertText($"line {i}");
            _editor.InsertNewline();
        }
        _editor.Home(true);
        _editor.PageDown();
        _editor.PageUp();
        Assert.AreEqual(0, _editor.CursorLine);
    }
}

[TestClass]
public class SyntaxHighlighterTests
{
    private SyntaxHighlighter _hl = null!;

    [TestInitialize]
    public void Setup()
    {
        _hl = new SyntaxHighlighter();
    }

    [TestMethod]
    public void Keywords_HighlightedAsCyan()
    {
        var colors = _hl.HighlightLine("if else while return");
        // "if" at positions 0-1
        Assert.AreEqual(SyntaxHighlighter.ColorKeyword, colors[0]);
        Assert.AreEqual(SyntaxHighlighter.ColorKeyword, colors[1]);
    }

    [TestMethod]
    public void Types_HighlightedAsYellow()
    {
        var colors = _hl.HighlightLine("byte x;");
        Assert.AreEqual(SyntaxHighlighter.ColorType, colors[0]);  // 'b'
        Assert.AreEqual(SyntaxHighlighter.ColorType, colors[3]);  // 'e'
    }

    [TestMethod]
    public void LineComment_HighlightedAsGrey()
    {
        var colors = _hl.HighlightLine("x = 1; // comment");
        // Everything after // should be grey
        int commentStart = "x = 1; ".Length;
        Assert.AreEqual(SyntaxHighlighter.ColorComment, colors[commentStart]);
        Assert.AreEqual(SyntaxHighlighter.ColorComment, colors[colors.Length - 1]);
    }

    [TestMethod]
    public void BlockComment_SpansLines()
    {
        // First line starts block comment
        var colors1 = _hl.HighlightLine("x = 1; /* start");
        Assert.IsTrue(_hl.InBlockComment);

        // Second line should be all comment
        var colors2 = _hl.HighlightLine("still in comment");
        Assert.AreEqual(SyntaxHighlighter.ColorComment, colors2[0]);
        Assert.AreEqual(SyntaxHighlighter.ColorComment, colors2[colors2.Length - 1]);
        Assert.IsTrue(_hl.InBlockComment);

        // Third line ends block comment
        var colors3 = _hl.HighlightLine("end */ byte x;");
        Assert.IsFalse(_hl.InBlockComment);
        // "byte" after */ should be type color
        int byteStart = "end */ ".Length;
        Assert.AreEqual(SyntaxHighlighter.ColorType, colors3[byteStart]);
    }

    [TestMethod]
    public void Strings_HighlightedAsGreen()
    {
        var colors = _hl.HighlightLine("x = \"hello\";");
        int strStart = 4; // position of first "
        Assert.AreEqual(SyntaxHighlighter.ColorString, colors[strStart]);
    }

    [TestMethod]
    public void Numbers_HighlightedAsOrange()
    {
        var colors = _hl.HighlightLine("x = 42;");
        Assert.AreEqual(SyntaxHighlighter.ColorNumber, colors[4]); // '4'
        Assert.AreEqual(SyntaxHighlighter.ColorNumber, colors[5]); // '2'
    }

    [TestMethod]
    public void HexNumbers_HighlightedAsOrange()
    {
        var colors = _hl.HighlightLine("x = 0xFF;");
        Assert.AreEqual(SyntaxHighlighter.ColorNumber, colors[4]); // '0'
        Assert.AreEqual(SyntaxHighlighter.ColorNumber, colors[5]); // 'x'
        Assert.AreEqual(SyntaxHighlighter.ColorNumber, colors[7]); // 'F'
    }

    [TestMethod]
    public void PreprocessorDirective_HighlightedAsPurple()
    {
        var colors = _hl.HighlightLine("#include <stdio.h>");
        Assert.AreEqual(SyntaxHighlighter.ColorPreproc, colors[0]);
        Assert.AreEqual(SyntaxHighlighter.ColorPreproc, colors[colors.Length - 1]);
    }

    [TestMethod]
    public void DefaultText_IsWhite()
    {
        var colors = _hl.HighlightLine("x = y;");
        Assert.AreEqual(SyntaxHighlighter.ColorDefault, colors[0]); // 'x'
    }

    [TestMethod]
    public void EscapedStringQuote_DoesNotEndString()
    {
        var colors = _hl.HighlightLine("x = \"he\\\"llo\";");
        // The escaped quote should still be string color
        int escPos = "x = \"he".Length;  // position of backslash
        Assert.AreEqual(SyntaxHighlighter.ColorString, colors[escPos]);
    }

    [TestMethod]
    public void BlockComment_OnSingleLine()
    {
        var colors = _hl.HighlightLine("x /* comment */ y");
        Assert.AreEqual(SyntaxHighlighter.ColorDefault, colors[0]); // 'x'
        Assert.AreEqual(SyntaxHighlighter.ColorComment, colors[2]); // '/'
        // 'y' at position 16 should be default again
        Assert.AreEqual(SyntaxHighlighter.ColorDefault, colors[16]);
        Assert.IsFalse(_hl.InBlockComment);
    }

    [TestMethod]
    public void Reset_ClearsBlockCommentState()
    {
        _hl.HighlightLine("/* start");
        Assert.IsTrue(_hl.InBlockComment);
        _hl.Reset();
        Assert.IsFalse(_hl.InBlockComment);
    }
}

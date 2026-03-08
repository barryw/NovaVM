using System;
using e6502.Avalonia.Editor;
using KDS.e6502;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests.Editor;

internal sealed class TestEditor : ScreenTextEditor
{
    private readonly BasicSyntaxHighlighter _hl = new();
    public TestEditor(IBusDevice bus) : base(bus) { }
    protected override string GetTitle() => "TEST";
    protected override string GetStatusExtra() => "";
    protected override string GetHelpText() => "";
    protected override ISyntaxHighlighter GetHighlighter() => _hl;
    protected override int GetGutterWidth() => 0;
    protected override void RenderGutterCell(int lineIndex, int row) { }
    protected override string GetFileExtension() => ".txt";
    protected override byte[] SerializeForSave(string filename)
        => System.Text.Encoding.ASCII.GetBytes(string.Join('\n', Lines));
    protected override void DeserializeOnLoad(byte[] data, string filename)
        => LoadLines(System.Text.Encoding.ASCII.GetString(data).Split('\n'));
}

[TestClass]
public class ScreenTextEditorTests
{
    private BusDevice _bus = null!;
    private TestEditor _ed = null!;

    [TestInitialize]
    public void Setup()
    {
        _bus = new BusDevice(new byte[65536]);
        _ed = new TestEditor(_bus);
    }

    [TestMethod]
    public void NewEditor_HasOneEmptyLine()
    {
        Assert.AreEqual(1, _ed.LineCount);
        Assert.AreEqual("", _ed.GetLineText(0));
    }

    [TestMethod]
    public void InsertChar_AppendsToLine()
    {
        _ed.InsertChar('A');
        _ed.InsertChar('B');
        Assert.AreEqual("AB", _ed.GetLineText(0));
        Assert.AreEqual(2, _ed.CursorCol);
    }

    [TestMethod]
    public void InsertNewline_SplitsLine()
    {
        _ed.InsertChar('A');
        _ed.InsertChar('B');
        _ed.SetCursor(0, 1);
        _ed.InsertNewline();
        Assert.AreEqual(2, _ed.LineCount);
        Assert.AreEqual("A", _ed.GetLineText(0));
        Assert.AreEqual("B", _ed.GetLineText(1));
        Assert.AreEqual(1, _ed.CursorLine);
        Assert.AreEqual(0, _ed.CursorCol);
    }

    [TestMethod]
    public void Backspace_DeletesCharBefore()
    {
        _ed.InsertChar('A');
        _ed.InsertChar('B');
        _ed.Backspace();
        Assert.AreEqual("A", _ed.GetLineText(0));
        Assert.AreEqual(1, _ed.CursorCol);
    }

    [TestMethod]
    public void Backspace_AtStartOfLine_JoinsWithPrevious()
    {
        _ed.InsertChar('A');
        _ed.InsertNewline();
        _ed.InsertChar('B');
        _ed.SetCursor(1, 0);
        _ed.Backspace();
        Assert.AreEqual(1, _ed.LineCount);
        Assert.AreEqual("AB", _ed.GetLineText(0));
        Assert.AreEqual(0, _ed.CursorLine);
        Assert.AreEqual(1, _ed.CursorCol);
    }

    [TestMethod]
    public void Delete_RemovesCharAtCursor()
    {
        _ed.InsertChar('A');
        _ed.InsertChar('B');
        _ed.SetCursor(0, 0);
        _ed.Delete();
        Assert.AreEqual("B", _ed.GetLineText(0));
        Assert.AreEqual(0, _ed.CursorCol);
    }

    [TestMethod]
    public void Delete_AtEndOfLine_JoinsWithNext()
    {
        _ed.InsertChar('A');
        _ed.InsertNewline();
        _ed.InsertChar('B');
        _ed.SetCursor(0, 1);
        _ed.Delete();
        Assert.AreEqual(1, _ed.LineCount);
        Assert.AreEqual("AB", _ed.GetLineText(0));
    }

    [TestMethod]
    public void DeleteCurrentLine_RemovesLine()
    {
        _ed.LoadLines(new[] { "AAA", "BBB", "CCC" });
        _ed.SetCursor(1, 0);
        _ed.DeleteCurrentLine();
        Assert.AreEqual(2, _ed.LineCount);
        Assert.AreEqual("AAA", _ed.GetLineText(0));
        Assert.AreEqual("CCC", _ed.GetLineText(1));
    }

    [TestMethod]
    public void DeleteCurrentLine_LastLine_LeavesEmptyLine()
    {
        _ed.DeleteCurrentLine();
        Assert.AreEqual(1, _ed.LineCount);
        Assert.AreEqual("", _ed.GetLineText(0));
    }

    [TestMethod]
    public void DuplicateLine_CreatesNewLine()
    {
        _ed.LoadLines(new[] { "HELLO" });
        _ed.SetCursor(0, 0);
        _ed.DuplicateLine();
        Assert.AreEqual(2, _ed.LineCount);
        Assert.AreEqual("HELLO", _ed.GetLineText(0));
        Assert.AreEqual("HELLO", _ed.GetLineText(1));
        Assert.AreEqual(1, _ed.CursorLine);
    }

    [TestMethod]
    public void MoveCursor_DownUp()
    {
        _ed.LoadLines(new[] { "AAA", "BBB", "CCC" });
        _ed.MoveCursor(0, 1);
        Assert.AreEqual(1, _ed.CursorLine);
        _ed.MoveCursor(0, -1);
        Assert.AreEqual(0, _ed.CursorLine);
    }

    [TestMethod]
    public void MoveCursor_UpAtTop_StaysAtTop()
    {
        _ed.MoveCursor(0, -1);
        Assert.AreEqual(0, _ed.CursorLine);
    }

    [TestMethod]
    public void MoveCursor_LeftAtStartOfLine_WrapsUp()
    {
        _ed.LoadLines(new[] { "AAA", "BBB" });
        _ed.SetCursor(1, 0);
        _ed.MoveCursor(-1, 0);
        Assert.AreEqual(0, _ed.CursorLine);
        Assert.AreEqual(3, _ed.CursorCol); // end of "AAA"
    }

    [TestMethod]
    public void MoveCursor_RightAtEndOfLine_WrapsDown()
    {
        _ed.LoadLines(new[] { "AAA", "BBB" });
        _ed.SetCursor(0, 3);
        _ed.MoveCursor(1, 0);
        Assert.AreEqual(1, _ed.CursorLine);
        Assert.AreEqual(0, _ed.CursorCol);
    }

    [TestMethod]
    public void Home_MovesCursorToStart()
    {
        _ed.InsertChar('A');
        _ed.InsertChar('B');
        _ed.Home();
        Assert.AreEqual(0, _ed.CursorCol);
    }

    [TestMethod]
    public void End_MovesCursorToEnd()
    {
        _ed.InsertChar('A');
        _ed.InsertChar('B');
        _ed.Home();
        _ed.End();
        Assert.AreEqual(2, _ed.CursorCol);
    }

    [TestMethod]
    public void PageDown_MovesVisibleLines()
    {
        // Create 50 lines
        var lines = new string[50];
        for (int i = 0; i < 50; i++) lines[i] = $"LINE {i}";
        _ed.LoadLines(lines);
        _ed.SetCursor(0, 0);
        _ed.PageDown();
        Assert.AreEqual(22, _ed.CursorLine); // VisibleLines = 22
    }

    [TestMethod]
    public void CursorCol_ClampedToLineLength()
    {
        _ed.LoadLines(new[] { "LONG LINE", "HI" });
        _ed.SetCursor(0, 9);
        _ed.MoveCursor(0, 1); // move down to shorter line
        Assert.AreEqual(2, _ed.CursorCol); // clamped to "HI".Length
    }

    [TestMethod]
    public void InsertOverwriteMode_Toggle()
    {
        Assert.IsTrue(_ed.InsertMode);
        _ed.ToggleInsertMode();
        Assert.IsFalse(_ed.InsertMode);

        // Overwrite mode replaces char
        _ed.LoadLines(new[] { "ABC" });
        _ed.SetCursor(0, 0);
        _ed.InsertChar('X');
        Assert.AreEqual("XBC", _ed.GetLineText(0));
    }

    [TestMethod]
    public void Modified_SetOnTextChange()
    {
        Assert.IsFalse(_ed.Modified);
        _ed.InsertChar('A');
        Assert.IsTrue(_ed.Modified);
    }

    [TestMethod]
    public void Modified_SetOnInsertNewline()
    {
        _ed.ClearModified();
        _ed.InsertNewline();
        Assert.IsTrue(_ed.Modified);
    }

    [TestMethod]
    public void Modified_SetOnDelete()
    {
        _ed.LoadLines(new[] { "AB" });
        _ed.SetCursor(0, 0);
        _ed.ClearModified();
        _ed.Delete();
        Assert.IsTrue(_ed.Modified);
    }

    [TestMethod]
    public void LoadLines_SetsBuffer()
    {
        _ed.LoadLines(new[] { "ONE", "TWO", "THREE" });
        Assert.AreEqual(3, _ed.LineCount);
        Assert.AreEqual("TWO", _ed.GetLineText(1));
        Assert.AreEqual(0, _ed.CursorLine);
        Assert.AreEqual(0, _ed.CursorCol);
        Assert.IsFalse(_ed.Modified);
    }

    [TestMethod]
    public void LoadLines_Empty_HasOneEmptyLine()
    {
        _ed.LoadLines(Array.Empty<string>());
        Assert.AreEqual(1, _ed.LineCount);
        Assert.AreEqual("", _ed.GetLineText(0));
    }

    [TestMethod]
    public void WordJumpRight_SkipsToNextWord()
    {
        _ed.LoadLines(new[] { "HELLO WORLD" });
        _ed.SetCursor(0, 0);
        _ed.MoveWordRight();
        Assert.AreEqual(6, _ed.CursorCol); // start of "WORLD"
    }

    [TestMethod]
    public void WordJumpLeft_SkipsToPreviousWord()
    {
        _ed.LoadLines(new[] { "HELLO WORLD" });
        _ed.SetCursor(0, 11);
        _ed.MoveWordLeft();
        Assert.AreEqual(6, _ed.CursorCol); // start of "WORLD"
    }

    [TestMethod]
    public void HorizontalScroll_UpdatesWhenCursorExceedsWidth()
    {
        // Create a line longer than CodeWidth (78)
        _ed.LoadLines(new[] { new string('X', 100) });
        _ed.SetCursor(0, 90);
        _ed.EnsureVisible();
        Assert.IsTrue(_ed.ScrollX > 0, "ScrollX should be > 0 when cursor is past CodeWidth");
        Assert.IsTrue(_ed.ScrollX <= 90, "ScrollX should be <= cursor position");
    }
}

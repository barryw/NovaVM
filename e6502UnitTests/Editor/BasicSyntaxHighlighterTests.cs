using e6502.Avalonia.Editor;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests.Editor;

[TestClass]
public class BasicSyntaxHighlighterTests
{
    private readonly BasicSyntaxHighlighter _hl = new();

    [TestMethod]
    public void Keywords_AreHighlighted()
    {
        var colors = _hl.HighlightLine("PRINT \"HELLO\"");
        // "PRINT" = indices 0..4
        for (int i = 0; i < 5; i++)
            Assert.AreEqual(BasicSyntaxHighlighter.ColorKeyword, colors[i], $"char {i}");
    }

    [TestMethod]
    public void Strings_AreHighlighted()
    {
        var colors = _hl.HighlightLine("PRINT \"HELLO\"");
        // " = index 6, HELLO = 7..11, " = 12
        for (int i = 6; i <= 12; i++)
            Assert.AreEqual(BasicSyntaxHighlighter.ColorString, colors[i], $"char {i}");
    }

    [TestMethod]
    public void Numbers_AreHighlighted()
    {
        // "X=42" — X=0, ==1, 4=2, 2=3
        var colors = _hl.HighlightLine("X=42");
        Assert.AreEqual(BasicSyntaxHighlighter.ColorNumber, colors[2], "digit 4");
        Assert.AreEqual(BasicSyntaxHighlighter.ColorNumber, colors[3], "digit 2");
    }

    [TestMethod]
    public void Rem_GreysOutRest()
    {
        var colors = _hl.HighlightLine("REM THIS IS A COMMENT");
        for (int i = 0; i < colors.Length; i++)
            Assert.AreEqual(BasicSyntaxHighlighter.ColorComment, colors[i], $"char {i}");
    }

    [TestMethod]
    public void Variables_AreDefaultColor()
    {
        // "X=Y+Z" — X, Y, Z are variables
        var colors = _hl.HighlightLine("X=Y+Z");
        Assert.AreEqual(BasicSyntaxHighlighter.ColorDefault, colors[0], "X");
        Assert.AreEqual(BasicSyntaxHighlighter.ColorDefault, colors[2], "Y");
        Assert.AreEqual(BasicSyntaxHighlighter.ColorDefault, colors[4], "Z");
    }

    [TestMethod]
    public void KeywordsAreCaseInsensitive()
    {
        var colors = _hl.HighlightLine("print \"hi\"");
        // "print" = indices 0..4
        for (int i = 0; i < 5; i++)
            Assert.AreEqual(BasicSyntaxHighlighter.ColorKeyword, colors[i], $"char {i}");
    }

    [TestMethod]
    public void EmptyLine_ReturnsEmptyArray()
    {
        var colors = _hl.HighlightLine("");
        Assert.AreEqual(0, colors.Length);
    }

    [TestMethod]
    public void DataKeyword_GreysOutRest()
    {
        // "DATA 1,2,3,HELLO" — DATA=ColorKeyword, rest=ColorComment
        var colors = _hl.HighlightLine("DATA 1,2,3,HELLO");
        for (int i = 0; i < 4; i++)
            Assert.AreEqual(BasicSyntaxHighlighter.ColorKeyword, colors[i], $"DATA char {i}");
        for (int i = 4; i < colors.Length; i++)
            Assert.AreEqual(BasicSyntaxHighlighter.ColorComment, colors[i], $"after DATA char {i}");
    }

    [TestMethod]
    public void KeywordNotMatchedInsideWord()
    {
        // "FORGET" should not color FOR as a keyword
        var colors = _hl.HighlightLine("FORGET");
        for (int i = 0; i < 6; i++)
            Assert.AreEqual(BasicSyntaxHighlighter.ColorDefault, colors[i], $"char {i}");
    }

    [TestMethod]
    public void NumberAfterLetter_NotColored()
    {
        // "A1" — the 1 is part of a variable name, not a standalone number
        var colors = _hl.HighlightLine("A1");
        Assert.AreEqual(BasicSyntaxHighlighter.ColorDefault, colors[1], "digit after letter");
    }
}

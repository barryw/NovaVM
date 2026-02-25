using System.Collections.Generic;
using e6502.Avalonia.Compiler;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests.Compiler;

[TestClass]
public class LexerTests
{
    private static List<Token> Lex(string source) => new Lexer(source).Tokenize();

    [TestMethod]
    public void Empty_ReturnsEof()
    {
        var tokens = Lex("");
        Assert.AreEqual(1, tokens.Count);
        Assert.AreEqual(TokenType.Eof, tokens[0].Type);
    }

    [TestMethod]
    public void IntegerLiteral()
    {
        var tokens = Lex("42");
        Assert.AreEqual(2, tokens.Count);
        Assert.AreEqual(TokenType.IntLiteral, tokens[0].Type);
        Assert.AreEqual("42", tokens[0].Value);
    }

    [TestMethod]
    public void HexLiteral()
    {
        var tokens = Lex("0xA040 0xFF");
        Assert.AreEqual(3, tokens.Count);
        Assert.AreEqual(TokenType.HexLiteral, tokens[0].Type);
        Assert.AreEqual("A040", tokens[0].Value);
        Assert.AreEqual(TokenType.HexLiteral, tokens[1].Type);
        Assert.AreEqual("FF", tokens[1].Value);
    }

    [TestMethod]
    public void BinaryLiteral()
    {
        var tokens = Lex("0b10110");
        Assert.AreEqual(2, tokens.Count);
        Assert.AreEqual(TokenType.BinLiteral, tokens[0].Type);
        Assert.AreEqual("10110", tokens[0].Value);
    }

    [TestMethod]
    public void FixedLiteral()
    {
        var tokens = Lex("3.14 0.5");
        Assert.AreEqual(3, tokens.Count);
        Assert.AreEqual(TokenType.FixedLiteral, tokens[0].Type);
        Assert.AreEqual("3.14", tokens[0].Value);
        Assert.AreEqual(TokenType.FixedLiteral, tokens[1].Type);
        Assert.AreEqual("0.5", tokens[1].Value);
    }

    [TestMethod]
    public void StringLiteral()
    {
        var tokens = Lex("\"hello world\"");
        Assert.AreEqual(2, tokens.Count);
        Assert.AreEqual(TokenType.StringLiteral, tokens[0].Type);
        Assert.AreEqual("hello world", tokens[0].Value);
    }

    [TestMethod]
    public void StringLiteral_WithEscapes()
    {
        var tokens = Lex("\"a\\nb\\tc\\\\d\\\"e\\0f\"");
        Assert.AreEqual(2, tokens.Count);
        Assert.AreEqual(TokenType.StringLiteral, tokens[0].Type);
        Assert.AreEqual("a\nb\tc\\d\"e\0f", tokens[0].Value);
    }

    [TestMethod]
    public void CharLiteral()
    {
        var tokens = Lex("'A' '\\n'");
        Assert.AreEqual(3, tokens.Count);
        Assert.AreEqual(TokenType.CharLiteral, tokens[0].Type);
        Assert.AreEqual("A", tokens[0].Value);
        Assert.AreEqual(TokenType.CharLiteral, tokens[1].Type);
        Assert.AreEqual("\n", tokens[1].Value);
    }

    [TestMethod]
    public void TypeKeywords()
    {
        var tokens = Lex("byte int uint bool fixed ufixed void struct enum const");
        // 10 keywords + Eof
        Assert.AreEqual(11, tokens.Count);
        Assert.AreEqual(TokenType.Byte, tokens[0].Type);
        Assert.AreEqual(TokenType.Int, tokens[1].Type);
        Assert.AreEqual(TokenType.Uint, tokens[2].Type);
        Assert.AreEqual(TokenType.Bool, tokens[3].Type);
        Assert.AreEqual(TokenType.Fixed, tokens[4].Type);
        Assert.AreEqual(TokenType.Ufixed, tokens[5].Type);
        Assert.AreEqual(TokenType.Void, tokens[6].Type);
        Assert.AreEqual(TokenType.Struct, tokens[7].Type);
        Assert.AreEqual(TokenType.Enum, tokens[8].Type);
        Assert.AreEqual(TokenType.Const, tokens[9].Type);
    }

    [TestMethod]
    public void ControlFlowKeywords()
    {
        var tokens = Lex("if else while do for switch case default break continue return");
        Assert.AreEqual(12, tokens.Count);
        Assert.AreEqual(TokenType.If, tokens[0].Type);
        Assert.AreEqual(TokenType.Else, tokens[1].Type);
        Assert.AreEqual(TokenType.While, tokens[2].Type);
        Assert.AreEqual(TokenType.Do, tokens[3].Type);
        Assert.AreEqual(TokenType.For, tokens[4].Type);
        Assert.AreEqual(TokenType.Switch, tokens[5].Type);
        Assert.AreEqual(TokenType.Case, tokens[6].Type);
        Assert.AreEqual(TokenType.Default, tokens[7].Type);
        Assert.AreEqual(TokenType.Break, tokens[8].Type);
        Assert.AreEqual(TokenType.Continue, tokens[9].Type);
        Assert.AreEqual(TokenType.Return, tokens[10].Type);
    }

    [TestMethod]
    public void BoolLiterals()
    {
        var tokens = Lex("true false");
        Assert.AreEqual(3, tokens.Count);
        Assert.AreEqual(TokenType.BoolLiteral, tokens[0].Type);
        Assert.AreEqual("true", tokens[0].Value);
        Assert.AreEqual(TokenType.BoolLiteral, tokens[1].Type);
        Assert.AreEqual("false", tokens[1].Value);
    }

    [TestMethod]
    public void SingleCharOperators()
    {
        var tokens = Lex("+ - * / % & | ^ ~ ! = < > .");
        Assert.AreEqual(15, tokens.Count);
        Assert.AreEqual(TokenType.Plus, tokens[0].Type);
        Assert.AreEqual(TokenType.Minus, tokens[1].Type);
        Assert.AreEqual(TokenType.Star, tokens[2].Type);
        Assert.AreEqual(TokenType.Slash, tokens[3].Type);
        Assert.AreEqual(TokenType.Percent, tokens[4].Type);
        Assert.AreEqual(TokenType.Amp, tokens[5].Type);
        Assert.AreEqual(TokenType.Pipe, tokens[6].Type);
        Assert.AreEqual(TokenType.Caret, tokens[7].Type);
        Assert.AreEqual(TokenType.Tilde, tokens[8].Type);
        Assert.AreEqual(TokenType.Bang, tokens[9].Type);
        Assert.AreEqual(TokenType.Assign, tokens[10].Type);
        Assert.AreEqual(TokenType.Lt, tokens[11].Type);
        Assert.AreEqual(TokenType.Gt, tokens[12].Type);
        Assert.AreEqual(TokenType.Dot, tokens[13].Type);
        Assert.AreEqual(TokenType.Eof, tokens[14].Type);
    }

    [TestMethod]
    public void MultiCharOperators()
    {
        var tokens = Lex("== != <= >= && || << >>");
        Assert.AreEqual(9, tokens.Count);
        Assert.AreEqual(TokenType.Eq, tokens[0].Type);
        Assert.AreEqual("==", tokens[0].Value);
        Assert.AreEqual(TokenType.NotEq, tokens[1].Type);
        Assert.AreEqual(TokenType.LtEq, tokens[2].Type);
        Assert.AreEqual(TokenType.GtEq, tokens[3].Type);
        Assert.AreEqual(TokenType.AmpAmp, tokens[4].Type);
        Assert.AreEqual(TokenType.PipePipe, tokens[5].Type);
        Assert.AreEqual(TokenType.LShift, tokens[6].Type);
        Assert.AreEqual(TokenType.RShift, tokens[7].Type);
    }

    [TestMethod]
    public void AssignmentOperators()
    {
        var tokens = Lex("+= -= *= /= %= &= |= ^=");
        Assert.AreEqual(9, tokens.Count);
        Assert.AreEqual(TokenType.PlusAssign, tokens[0].Type);
        Assert.AreEqual(TokenType.MinusAssign, tokens[1].Type);
        Assert.AreEqual(TokenType.StarAssign, tokens[2].Type);
        Assert.AreEqual(TokenType.SlashAssign, tokens[3].Type);
        Assert.AreEqual(TokenType.PercentAssign, tokens[4].Type);
        Assert.AreEqual(TokenType.AmpAssign, tokens[5].Type);
        Assert.AreEqual(TokenType.PipeAssign, tokens[6].Type);
        Assert.AreEqual(TokenType.CaretAssign, tokens[7].Type);
    }

    [TestMethod]
    public void IncrementDecrement()
    {
        var tokens = Lex("++ --");
        Assert.AreEqual(3, tokens.Count);
        Assert.AreEqual(TokenType.PlusPlus, tokens[0].Type);
        Assert.AreEqual("++", tokens[0].Value);
        Assert.AreEqual(TokenType.MinusMinus, tokens[1].Type);
        Assert.AreEqual("--", tokens[1].Value);
    }

    [TestMethod]
    public void ArrowAndDot()
    {
        var tokens = Lex("-> .");
        Assert.AreEqual(3, tokens.Count);
        Assert.AreEqual(TokenType.Arrow, tokens[0].Type);
        Assert.AreEqual("->", tokens[0].Value);
        Assert.AreEqual(TokenType.Dot, tokens[1].Type);
        Assert.AreEqual(".", tokens[1].Value);
    }

    [TestMethod]
    public void Delimiters()
    {
        var tokens = Lex("( ) { } [ ] ; , :");
        Assert.AreEqual(10, tokens.Count);
        Assert.AreEqual(TokenType.LParen, tokens[0].Type);
        Assert.AreEqual(TokenType.RParen, tokens[1].Type);
        Assert.AreEqual(TokenType.LBrace, tokens[2].Type);
        Assert.AreEqual(TokenType.RBrace, tokens[3].Type);
        Assert.AreEqual(TokenType.LBracket, tokens[4].Type);
        Assert.AreEqual(TokenType.RBracket, tokens[5].Type);
        Assert.AreEqual(TokenType.Semicolon, tokens[6].Type);
        Assert.AreEqual(TokenType.Comma, tokens[7].Type);
        Assert.AreEqual(TokenType.Colon, tokens[8].Type);
    }

    [TestMethod]
    public void Identifier()
    {
        var tokens = Lex("myVar x foo123");
        Assert.AreEqual(4, tokens.Count);
        Assert.AreEqual(TokenType.Identifier, tokens[0].Type);
        Assert.AreEqual("myVar", tokens[0].Value);
        Assert.AreEqual(TokenType.Identifier, tokens[1].Type);
        Assert.AreEqual("x", tokens[1].Value);
        Assert.AreEqual(TokenType.Identifier, tokens[2].Type);
        Assert.AreEqual("foo123", tokens[2].Value);
    }

    [TestMethod]
    public void Identifier_WithUnderscore()
    {
        var tokens = Lex("_start my_var _0");
        Assert.AreEqual(4, tokens.Count);
        Assert.AreEqual(TokenType.Identifier, tokens[0].Type);
        Assert.AreEqual("_start", tokens[0].Value);
        Assert.AreEqual(TokenType.Identifier, tokens[1].Type);
        Assert.AreEqual("my_var", tokens[1].Value);
        Assert.AreEqual(TokenType.Identifier, tokens[2].Type);
        Assert.AreEqual("_0", tokens[2].Value);
    }

    [TestMethod]
    public void LineComment()
    {
        var tokens = Lex("x // comment\ny");
        Assert.AreEqual(3, tokens.Count);
        Assert.AreEqual(TokenType.Identifier, tokens[0].Type);
        Assert.AreEqual("x", tokens[0].Value);
        Assert.AreEqual(TokenType.Identifier, tokens[1].Type);
        Assert.AreEqual("y", tokens[1].Value);
    }

    [TestMethod]
    public void BlockComment()
    {
        var tokens = Lex("a /* block\ncomment */ b");
        Assert.AreEqual(3, tokens.Count);
        Assert.AreEqual(TokenType.Identifier, tokens[0].Type);
        Assert.AreEqual("a", tokens[0].Value);
        Assert.AreEqual(TokenType.Identifier, tokens[1].Type);
        Assert.AreEqual("b", tokens[1].Value);
    }

    [TestMethod]
    public void TracksLineNumbers()
    {
        var tokens = Lex("a\nb\nc");
        Assert.AreEqual(4, tokens.Count);
        Assert.AreEqual(1, tokens[0].Line);
        Assert.AreEqual(2, tokens[1].Line);
        Assert.AreEqual(3, tokens[2].Line);
    }

    [TestMethod]
    public void TracksColumns()
    {
        var tokens = Lex("ab cd ef");
        Assert.AreEqual(4, tokens.Count);
        Assert.AreEqual(1, tokens[0].Column);  // "ab" starts at col 1
        Assert.AreEqual(4, tokens[1].Column);  // "cd" starts at col 4
        Assert.AreEqual(7, tokens[2].Column);  // "ef" starts at col 7
    }

    [TestMethod]
    public void CompleteStatement()
    {
        var tokens = Lex("int x = 42;");
        Assert.AreEqual(6, tokens.Count);
        Assert.AreEqual(TokenType.Int, tokens[0].Type);
        Assert.AreEqual(TokenType.Identifier, tokens[1].Type);
        Assert.AreEqual("x", tokens[1].Value);
        Assert.AreEqual(TokenType.Assign, tokens[2].Type);
        Assert.AreEqual(TokenType.IntLiteral, tokens[3].Type);
        Assert.AreEqual("42", tokens[3].Value);
        Assert.AreEqual(TokenType.Semicolon, tokens[4].Type);
        Assert.AreEqual(TokenType.Eof, tokens[5].Type);
    }

    [TestMethod]
    public void PreprocessorDirectives()
    {
        var tokens = Lex("#include #define #asset #pragma #");
        Assert.AreEqual(6, tokens.Count);
        Assert.AreEqual(TokenType.Include, tokens[0].Type);
        Assert.AreEqual("#include", tokens[0].Value);
        Assert.AreEqual(TokenType.Define, tokens[1].Type);
        Assert.AreEqual("#define", tokens[1].Value);
        Assert.AreEqual(TokenType.Asset, tokens[2].Type);
        Assert.AreEqual("#asset", tokens[2].Value);
        Assert.AreEqual(TokenType.Pragma, tokens[3].Type);
        Assert.AreEqual("#pragma", tokens[3].Value);
        Assert.AreEqual(TokenType.Hash, tokens[4].Type);
    }

    [TestMethod]
    public void DoubleUnderscoreKeywords()
    {
        var tokens = Lex("__resident __interrupt");
        Assert.AreEqual(3, tokens.Count);
        Assert.AreEqual(TokenType.Resident, tokens[0].Type);
        Assert.AreEqual("__resident", tokens[0].Value);
        Assert.AreEqual(TokenType.Interrupt, tokens[1].Type);
        Assert.AreEqual("__interrupt", tokens[1].Value);
    }

    [TestMethod]
    public void ShiftAssignOperators()
    {
        var tokens = Lex("<<= >>=");
        Assert.AreEqual(3, tokens.Count);
        Assert.AreEqual(TokenType.LShiftAssign, tokens[0].Type);
        Assert.AreEqual("<<=", tokens[0].Value);
        Assert.AreEqual(TokenType.RShiftAssign, tokens[1].Type);
        Assert.AreEqual(">>=", tokens[1].Value);
    }

    [TestMethod]
    public void UnknownChar_ProducesError()
    {
        var tokens = Lex("@");
        Assert.AreEqual(2, tokens.Count);
        Assert.AreEqual(TokenType.Error, tokens[0].Type);
        Assert.AreEqual("@", tokens[0].Value);
    }

    [TestMethod]
    public void ComplexExpression()
    {
        var tokens = Lex("a = (b + c) * d >> 2;");
        Assert.AreEqual(13, tokens.Count);
        Assert.AreEqual(TokenType.Identifier, tokens[0].Type);   // a
        Assert.AreEqual(TokenType.Assign, tokens[1].Type);        // =
        Assert.AreEqual(TokenType.LParen, tokens[2].Type);        // (
        Assert.AreEqual(TokenType.Identifier, tokens[3].Type);    // b
        Assert.AreEqual(TokenType.Plus, tokens[4].Type);           // +
        Assert.AreEqual(TokenType.Identifier, tokens[5].Type);    // c
        Assert.AreEqual(TokenType.RParen, tokens[6].Type);         // )
        Assert.AreEqual(TokenType.Star, tokens[7].Type);           // *
        Assert.AreEqual(TokenType.Identifier, tokens[8].Type);    // d
        Assert.AreEqual(TokenType.RShift, tokens[9].Type);         // >>
        Assert.AreEqual(TokenType.IntLiteral, tokens[10].Type);   // 2
        Assert.AreEqual(TokenType.Semicolon, tokens[11].Type);    // ;
        Assert.AreEqual(TokenType.Eof, tokens[12].Type);
    }

    [TestMethod]
    public void FunctionSignature()
    {
        var tokens = Lex("void main(int argc, byte* argv)");
        Assert.AreEqual(11, tokens.Count);
        Assert.AreEqual(TokenType.Void, tokens[0].Type);
        Assert.AreEqual(TokenType.Identifier, tokens[1].Type);
        Assert.AreEqual("main", tokens[1].Value);
        Assert.AreEqual(TokenType.LParen, tokens[2].Type);
        Assert.AreEqual(TokenType.Int, tokens[3].Type);
        Assert.AreEqual(TokenType.Identifier, tokens[4].Type);
        Assert.AreEqual("argc", tokens[4].Value);
        Assert.AreEqual(TokenType.Comma, tokens[5].Type);
        Assert.AreEqual(TokenType.Byte, tokens[6].Type);
        Assert.AreEqual(TokenType.Star, tokens[7].Type);
        Assert.AreEqual(TokenType.Identifier, tokens[8].Type);
        Assert.AreEqual("argv", tokens[8].Value);
        Assert.AreEqual(TokenType.RParen, tokens[9].Type);
        Assert.AreEqual(TokenType.Eof, tokens[10].Type);
    }
}

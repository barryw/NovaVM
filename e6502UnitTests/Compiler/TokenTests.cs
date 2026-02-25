using e6502.Avalonia.Compiler;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests.Compiler;

[TestClass]
public class TokenTests
{
    [TestMethod]
    public void Token_StoresTypeAndValue()
    {
        var tok = new Token(TokenType.IntLiteral, "42", 1, 5);
        Assert.AreEqual(TokenType.IntLiteral, tok.Type);
        Assert.AreEqual("42", tok.Value);
        Assert.AreEqual(1, tok.Line);
        Assert.AreEqual(5, tok.Column);
    }

    [TestMethod]
    public void Token_ValueEquality()
    {
        var a = new Token(TokenType.Identifier, "x", 1, 1);
        var b = new Token(TokenType.Identifier, "x", 1, 1);
        Assert.AreEqual(a, b);
    }
}

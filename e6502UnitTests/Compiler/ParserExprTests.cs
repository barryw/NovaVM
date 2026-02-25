using e6502.Avalonia.Compiler;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests.Compiler;

[TestClass]
public class ParserExprTests
{
    private static Expr Parse(string source)
    {
        var parser = Parser.FromSource(source);
        var expr = parser.ParseExpression();
        Assert.AreEqual(0, parser.Errors.Count,
            string.Join("\n", parser.Errors));
        return expr;
    }

    // ── Literals ─────────────────────────────────────────────────────────

    [TestMethod]
    public void IntLiteral()
    {
        var expr = Parse("42");
        Assert.IsInstanceOfType<IntLiteralExpr>(expr);
        Assert.AreEqual(42L, ((IntLiteralExpr)expr).Value);
    }

    [TestMethod]
    public void HexLiteral()
    {
        var expr = Parse("0xA040");
        Assert.IsInstanceOfType<HexLiteralExpr>(expr);
        Assert.AreEqual("A040", ((HexLiteralExpr)expr).Value);
    }

    [TestMethod]
    public void BinLiteral()
    {
        var expr = Parse("0b11001010");
        Assert.IsInstanceOfType<BinLiteralExpr>(expr);
        Assert.AreEqual("11001010", ((BinLiteralExpr)expr).Value);
    }

    [TestMethod]
    public void FixedLiteral()
    {
        var expr = Parse("3.14");
        Assert.IsInstanceOfType<FixedLiteralExpr>(expr);
        Assert.AreEqual("3.14", ((FixedLiteralExpr)expr).Value);
    }

    [TestMethod]
    public void BoolLiteral()
    {
        var expr = Parse("true");
        Assert.IsInstanceOfType<BoolLiteralExpr>(expr);
        Assert.IsTrue(((BoolLiteralExpr)expr).Value);
    }

    [TestMethod]
    public void BoolLiteralFalse()
    {
        var expr = Parse("false");
        Assert.IsInstanceOfType<BoolLiteralExpr>(expr);
        Assert.IsFalse(((BoolLiteralExpr)expr).Value);
    }

    [TestMethod]
    public void StringLiteral()
    {
        var expr = Parse("\"hello\"");
        Assert.IsInstanceOfType<StringLiteralExpr>(expr);
        Assert.AreEqual("hello", ((StringLiteralExpr)expr).Value);
    }

    [TestMethod]
    public void CharLiteral()
    {
        var expr = Parse("'A'");
        Assert.IsInstanceOfType<CharLiteralExpr>(expr);
        Assert.AreEqual('A', ((CharLiteralExpr)expr).Value);
    }

    [TestMethod]
    public void Identifier()
    {
        var expr = Parse("foo");
        Assert.IsInstanceOfType<IdentifierExpr>(expr);
        Assert.AreEqual("foo", ((IdentifierExpr)expr).Name);
    }

    // ── Binary operators ─────────────────────────────────────────────────

    [TestMethod]
    public void BinaryAdd()
    {
        var expr = Parse("1 + 2");
        var bin = (BinaryExpr)expr;
        Assert.AreEqual(TokenType.Plus, bin.Op);
        Assert.IsInstanceOfType<IntLiteralExpr>(bin.Left);
        Assert.IsInstanceOfType<IntLiteralExpr>(bin.Right);
    }

    [TestMethod]
    public void BinarySub()
    {
        var expr = Parse("5 - 3");
        var bin = (BinaryExpr)expr;
        Assert.AreEqual(TokenType.Minus, bin.Op);
    }

    [TestMethod]
    public void BinaryMul()
    {
        var expr = Parse("2 * 4");
        var bin = (BinaryExpr)expr;
        Assert.AreEqual(TokenType.Star, bin.Op);
    }

    [TestMethod]
    public void BinaryDiv()
    {
        var expr = Parse("10 / 2");
        var bin = (BinaryExpr)expr;
        Assert.AreEqual(TokenType.Slash, bin.Op);
    }

    [TestMethod]
    public void BinaryMod()
    {
        var expr = Parse("10 % 3");
        var bin = (BinaryExpr)expr;
        Assert.AreEqual(TokenType.Percent, bin.Op);
    }

    // ── Precedence ───────────────────────────────────────────────────────

    [TestMethod]
    public void Precedence_MulOverAdd()
    {
        var expr = Parse("1 + 2 * 3");
        var bin = (BinaryExpr)expr;
        Assert.AreEqual(TokenType.Plus, bin.Op);
        Assert.IsInstanceOfType<BinaryExpr>(bin.Right);
        var right = (BinaryExpr)bin.Right;
        Assert.AreEqual(TokenType.Star, right.Op);
    }

    [TestMethod]
    public void Precedence_AddLeftAssociative()
    {
        // 1 + 2 + 3 => (1+2)+3
        var expr = Parse("1 + 2 + 3");
        var bin = (BinaryExpr)expr;
        Assert.AreEqual(TokenType.Plus, bin.Op);
        Assert.IsInstanceOfType<BinaryExpr>(bin.Left);
        Assert.IsInstanceOfType<IntLiteralExpr>(bin.Right);
    }

    [TestMethod]
    public void Parenthesized()
    {
        var expr = Parse("(1 + 2) * 3");
        var bin = (BinaryExpr)expr;
        Assert.AreEqual(TokenType.Star, bin.Op);
        Assert.IsInstanceOfType<BinaryExpr>(bin.Left);
    }

    // ── Unary ────────────────────────────────────────────────────────────

    [TestMethod]
    public void UnaryNegate()
    {
        var expr = Parse("-x");
        var un = (UnaryExpr)expr;
        Assert.AreEqual(TokenType.Minus, un.Op);
        Assert.IsTrue(un.Prefix);
    }

    [TestMethod]
    public void UnaryNot()
    {
        var expr = Parse("!flag");
        Assert.IsInstanceOfType<UnaryExpr>(expr);
        var un = (UnaryExpr)expr;
        Assert.AreEqual(TokenType.Bang, un.Op);
        Assert.IsTrue(un.Prefix);
    }

    [TestMethod]
    public void UnaryBitwiseNot()
    {
        var expr = Parse("~mask");
        Assert.IsInstanceOfType<UnaryExpr>(expr);
        var un = (UnaryExpr)expr;
        Assert.AreEqual(TokenType.Tilde, un.Op);
        Assert.IsTrue(un.Prefix);
    }

    [TestMethod]
    public void PrefixIncrement()
    {
        var expr = Parse("++x");
        var un = (UnaryExpr)expr;
        Assert.AreEqual(TokenType.PlusPlus, un.Op);
        Assert.IsTrue(un.Prefix);
    }

    [TestMethod]
    public void PrefixDecrement()
    {
        var expr = Parse("--x");
        var un = (UnaryExpr)expr;
        Assert.AreEqual(TokenType.MinusMinus, un.Op);
        Assert.IsTrue(un.Prefix);
    }

    [TestMethod]
    public void PostfixIncrement()
    {
        var expr = Parse("x++");
        var un = (UnaryExpr)expr;
        Assert.AreEqual(TokenType.PlusPlus, un.Op);
        Assert.IsFalse(un.Prefix);
    }

    [TestMethod]
    public void PostfixDecrement()
    {
        var expr = Parse("x--");
        var un = (UnaryExpr)expr;
        Assert.AreEqual(TokenType.MinusMinus, un.Op);
        Assert.IsFalse(un.Prefix);
    }

    [TestMethod]
    public void AddressOf()
    {
        var expr = Parse("&player");
        var un = (UnaryExpr)expr;
        Assert.AreEqual(TokenType.Amp, un.Op);
        Assert.IsTrue(un.Prefix);
    }

    [TestMethod]
    public void Dereference()
    {
        var expr = Parse("*ptr");
        var un = (UnaryExpr)expr;
        Assert.AreEqual(TokenType.Star, un.Op);
        Assert.IsTrue(un.Prefix);
    }

    // ── Function calls ───────────────────────────────────────────────────

    [TestMethod]
    public void FunctionCall()
    {
        var expr = Parse("spr_move(0, x, y)");
        var call = (CallExpr)expr;
        Assert.AreEqual(3, call.Args.Count);
        Assert.IsInstanceOfType<IdentifierExpr>(call.Callee);
        Assert.AreEqual("spr_move", ((IdentifierExpr)call.Callee).Name);
    }

    [TestMethod]
    public void FunctionCallNoArgs()
    {
        var expr = Parse("vgc_vsync()");
        var call = (CallExpr)expr;
        Assert.AreEqual(0, call.Args.Count);
    }

    [TestMethod]
    public void FunctionCallOneArg()
    {
        var expr = Parse("abs(x)");
        var call = (CallExpr)expr;
        Assert.AreEqual(1, call.Args.Count);
    }

    // ── Array index ──────────────────────────────────────────────────────

    [TestMethod]
    public void ArrayIndex()
    {
        var expr = Parse("arr[i]");
        var idx = (IndexExpr)expr;
        Assert.IsInstanceOfType<IdentifierExpr>(idx.Array);
        Assert.IsInstanceOfType<IdentifierExpr>(idx.Index);
    }

    [TestMethod]
    public void ArrayIndexWithExpression()
    {
        var expr = Parse("arr[i + 1]");
        var idx = (IndexExpr)expr;
        Assert.IsInstanceOfType<BinaryExpr>(idx.Index);
    }

    // ── Member access ────────────────────────────────────────────────────

    [TestMethod]
    public void MemberDot()
    {
        var expr = Parse("player.x");
        var mem = (MemberExpr)expr;
        Assert.AreEqual("x", mem.Member);
        Assert.IsFalse(mem.IsArrow);
    }

    [TestMethod]
    public void MemberArrow()
    {
        var expr = Parse("p->health");
        var mem = (MemberExpr)expr;
        Assert.AreEqual("health", mem.Member);
        Assert.IsTrue(mem.IsArrow);
    }

    [TestMethod]
    public void ChainedMemberAccess()
    {
        var expr = Parse("a.b.c");
        var outer = (MemberExpr)expr;
        Assert.AreEqual("c", outer.Member);
        var inner = (MemberExpr)outer.Object;
        Assert.AreEqual("b", inner.Member);
    }

    [TestMethod]
    public void CompoundExpression()
    {
        // a->b[i].c => MemberExpr(IndexExpr(MemberExpr(a, b, arrow), i), c, dot)
        var expr = Parse("a->b[i].c");
        Assert.IsInstanceOfType<MemberExpr>(expr);
        var dot = (MemberExpr)expr;
        Assert.AreEqual("c", dot.Member);
        Assert.IsFalse(dot.IsArrow);
        Assert.IsInstanceOfType<IndexExpr>(dot.Object);
        var idx = (IndexExpr)dot.Object;
        Assert.IsInstanceOfType<MemberExpr>(idx.Array);
        var arrow = (MemberExpr)idx.Array;
        Assert.AreEqual("b", arrow.Member);
        Assert.IsTrue(arrow.IsArrow);
    }

    // ── Assignment ───────────────────────────────────────────────────────

    [TestMethod]
    public void Assignment()
    {
        var expr = Parse("x = 42");
        var assign = (AssignExpr)expr;
        Assert.AreEqual(TokenType.Assign, assign.Op);
    }

    [TestMethod]
    public void CompoundAssignment()
    {
        var expr = Parse("x += 1");
        var assign = (AssignExpr)expr;
        Assert.AreEqual(TokenType.PlusAssign, assign.Op);
    }

    [TestMethod]
    public void AssignmentRightAssociative()
    {
        // x = y = 1 => x = (y = 1)
        var expr = Parse("x = y = 1");
        var outer = (AssignExpr)expr;
        Assert.AreEqual(TokenType.Assign, outer.Op);
        Assert.IsInstanceOfType<AssignExpr>(outer.Value);
    }

    // ── Logical operators ────────────────────────────────────────────────

    [TestMethod]
    public void LogicalOperators()
    {
        var expr = Parse("a && b || c");
        // || has lower precedence than &&
        var bin = (BinaryExpr)expr;
        Assert.AreEqual(TokenType.PipePipe, bin.Op);
        Assert.IsInstanceOfType<BinaryExpr>(bin.Left);
        Assert.AreEqual(TokenType.AmpAmp, ((BinaryExpr)bin.Left).Op);
    }

    [TestMethod]
    public void LogicalAnd()
    {
        var expr = Parse("a && b");
        var bin = (BinaryExpr)expr;
        Assert.AreEqual(TokenType.AmpAmp, bin.Op);
    }

    [TestMethod]
    public void LogicalOr()
    {
        var expr = Parse("a || b");
        var bin = (BinaryExpr)expr;
        Assert.AreEqual(TokenType.PipePipe, bin.Op);
    }

    // ── Bitwise operators ────────────────────────────────────────────────

    [TestMethod]
    public void BitwiseOperators()
    {
        // a & b | c ^ d
        // Precedence: & (5) > ^ (4) > | (3)
        // Parse: (a & b) | (c ^ d)
        var expr = Parse("a & b | c ^ d");
        var bin = (BinaryExpr)expr;
        Assert.AreEqual(TokenType.Pipe, bin.Op);
        Assert.IsInstanceOfType<BinaryExpr>(bin.Left);
        Assert.AreEqual(TokenType.Amp, ((BinaryExpr)bin.Left).Op);
        Assert.IsInstanceOfType<BinaryExpr>(bin.Right);
        Assert.AreEqual(TokenType.Caret, ((BinaryExpr)bin.Right).Op);
    }

    [TestMethod]
    public void BitwiseAnd()
    {
        var expr = Parse("a & b");
        var bin = (BinaryExpr)expr;
        Assert.AreEqual(TokenType.Amp, bin.Op);
    }

    [TestMethod]
    public void BitwiseOr()
    {
        var expr = Parse("a | b");
        var bin = (BinaryExpr)expr;
        Assert.AreEqual(TokenType.Pipe, bin.Op);
    }

    [TestMethod]
    public void BitwiseXor()
    {
        var expr = Parse("a ^ b");
        var bin = (BinaryExpr)expr;
        Assert.AreEqual(TokenType.Caret, bin.Op);
    }

    // ── Shift operators ──────────────────────────────────────────────────

    [TestMethod]
    public void ShiftOperators()
    {
        var expr = Parse("x << 2");
        var bin = (BinaryExpr)expr;
        Assert.AreEqual(TokenType.LShift, bin.Op);
    }

    [TestMethod]
    public void RightShift()
    {
        var expr = Parse("x >> 4");
        var bin = (BinaryExpr)expr;
        Assert.AreEqual(TokenType.RShift, bin.Op);
    }

    // ── Comparison operators ─────────────────────────────────────────────

    [TestMethod]
    public void ComparisonOperators()
    {
        var expr = Parse("x >= 10");
        var bin = (BinaryExpr)expr;
        Assert.AreEqual(TokenType.GtEq, bin.Op);
    }

    [TestMethod]
    public void LessThan()
    {
        var expr = Parse("a < b");
        var bin = (BinaryExpr)expr;
        Assert.AreEqual(TokenType.Lt, bin.Op);
    }

    [TestMethod]
    public void LessThanOrEqual()
    {
        var expr = Parse("a <= b");
        var bin = (BinaryExpr)expr;
        Assert.AreEqual(TokenType.LtEq, bin.Op);
    }

    [TestMethod]
    public void GreaterThan()
    {
        var expr = Parse("a > b");
        var bin = (BinaryExpr)expr;
        Assert.AreEqual(TokenType.Gt, bin.Op);
    }

    // ── Equality operators ───────────────────────────────────────────────

    [TestMethod]
    public void EqualityOperators()
    {
        var expr = Parse("x == 0");
        var bin = (BinaryExpr)expr;
        Assert.AreEqual(TokenType.Eq, bin.Op);
    }

    [TestMethod]
    public void NotEqual()
    {
        var expr = Parse("x != 0");
        var bin = (BinaryExpr)expr;
        Assert.AreEqual(TokenType.NotEq, bin.Op);
    }

    // ── sizeof / cast ────────────────────────────────────────────────────

    [TestMethod]
    public void Sizeof()
    {
        var expr = Parse("sizeof(int)");
        Assert.IsInstanceOfType<SizeofExpr>(expr);
        Assert.AreEqual("int", ((SizeofExpr)expr).TargetType.Name);
    }

    [TestMethod]
    public void SizeofPointer()
    {
        var expr = Parse("sizeof(byte*)");
        var sz = (SizeofExpr)expr;
        Assert.AreEqual("byte", sz.TargetType.Name);
        Assert.IsTrue(sz.TargetType.IsPointer);
    }

    [TestMethod]
    public void CastExpression()
    {
        var expr = Parse("(int)x");
        var cast = (CastExpr)expr;
        Assert.AreEqual("int", cast.TargetType.Name);
        Assert.IsInstanceOfType<IdentifierExpr>(cast.Operand);
    }

    [TestMethod]
    public void CastPointerType()
    {
        var expr = Parse("(byte*)addr");
        var cast = (CastExpr)expr;
        Assert.AreEqual("byte", cast.TargetType.Name);
        Assert.IsTrue(cast.TargetType.IsPointer);
    }

    // ── Complex expressions ──────────────────────────────────────────────

    [TestMethod]
    public void ComplexArithmetic()
    {
        // (a + b) * (c - d)
        var expr = Parse("(a + b) * (c - d)");
        var bin = (BinaryExpr)expr;
        Assert.AreEqual(TokenType.Star, bin.Op);
        Assert.IsInstanceOfType<BinaryExpr>(bin.Left);
        Assert.IsInstanceOfType<BinaryExpr>(bin.Right);
    }

    [TestMethod]
    public void NestedCallsAndIndex()
    {
        // foo(bar[0])
        var expr = Parse("foo(bar[0])");
        var call = (CallExpr)expr;
        Assert.AreEqual(1, call.Args.Count);
        Assert.IsInstanceOfType<IndexExpr>(call.Args[0]);
    }

    [TestMethod]
    public void AssignWithBinaryRhs()
    {
        // x = a + b * c
        var expr = Parse("x = a + b * c");
        var assign = (AssignExpr)expr;
        Assert.AreEqual(TokenType.Assign, assign.Op);
        Assert.IsInstanceOfType<BinaryExpr>(assign.Value);
    }

    [TestMethod]
    public void UnaryInBinaryExpr()
    {
        // -a + b
        var expr = Parse("-a + b");
        var bin = (BinaryExpr)expr;
        Assert.AreEqual(TokenType.Plus, bin.Op);
        Assert.IsInstanceOfType<UnaryExpr>(bin.Left);
    }

    [TestMethod]
    public void ComparisonWithArithmetic()
    {
        // a + 1 < b * 2
        var expr = Parse("a + 1 < b * 2");
        var bin = (BinaryExpr)expr;
        Assert.AreEqual(TokenType.Lt, bin.Op);
        Assert.IsInstanceOfType<BinaryExpr>(bin.Left);
        Assert.AreEqual(TokenType.Plus, ((BinaryExpr)bin.Left).Op);
        Assert.IsInstanceOfType<BinaryExpr>(bin.Right);
        Assert.AreEqual(TokenType.Star, ((BinaryExpr)bin.Right).Op);
    }
}

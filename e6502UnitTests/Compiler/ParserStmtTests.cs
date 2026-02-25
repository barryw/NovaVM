using e6502.Avalonia.Compiler;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests.Compiler;

[TestClass]
public class ParserStmtTests
{
    private static Stmt Parse(string source)
    {
        var parser = Parser.FromSource(source);
        var stmt = parser.ParseStatement();
        Assert.AreEqual(0, parser.Errors.Count,
            string.Join("\n", parser.Errors));
        return stmt;
    }

    // ── Expression statement ──────────────────────────────────────────────

    [TestMethod]
    public void ExpressionStatement()
    {
        var stmt = Parse("spr_move(0, x, y);");
        var exprStmt = (ExprStmt)stmt;
        var call = (CallExpr)exprStmt.Expression;
        Assert.AreEqual(3, call.Args.Count);
        Assert.AreEqual("spr_move", ((IdentifierExpr)call.Callee).Name);
    }

    // ── Variable declarations ─────────────────────────────────────────────

    [TestMethod]
    public void LocalVarDecl_NoInit()
    {
        var stmt = Parse("byte x;");
        var decl = (VarDeclStmt)stmt;
        Assert.AreEqual("byte", decl.Type.Name);
        Assert.AreEqual("x", decl.Name);
        Assert.IsNull(decl.Initializer);
    }

    [TestMethod]
    public void LocalVarDecl_WithInit()
    {
        var stmt = Parse("byte x = 42;");
        var decl = (VarDeclStmt)stmt;
        Assert.AreEqual("byte", decl.Type.Name);
        Assert.AreEqual("x", decl.Name);
        Assert.IsNotNull(decl.Initializer);
        Assert.IsInstanceOfType<IntLiteralExpr>(decl.Initializer);
        Assert.AreEqual(42L, ((IntLiteralExpr)decl.Initializer).Value);
    }

    [TestMethod]
    public void LocalVarDecl_Int()
    {
        var stmt = Parse("int score = -1;");
        var decl = (VarDeclStmt)stmt;
        Assert.AreEqual("int", decl.Type.Name);
        Assert.AreEqual("score", decl.Name);
        Assert.IsNotNull(decl.Initializer);
        // -1 is parsed as unary minus on 1
        var unary = (UnaryExpr)decl.Initializer;
        Assert.AreEqual(TokenType.Minus, unary.Op);
    }

    [TestMethod]
    public void LocalArrayDecl()
    {
        var stmt = Parse("byte arr[16];");
        var decl = (ArrayDeclStmt)stmt;
        Assert.AreEqual("byte", decl.ElementType.Name);
        Assert.AreEqual("arr", decl.Name);
        Assert.IsInstanceOfType<IntLiteralExpr>(decl.Size);
        Assert.AreEqual(16L, ((IntLiteralExpr)decl.Size).Value);
        Assert.IsNull(decl.Initializer);
    }

    // ── Block ─────────────────────────────────────────────────────────────

    [TestMethod]
    public void Block()
    {
        var stmt = Parse("{ byte x = 1; x = 2; }");
        var block = (BlockStmt)stmt;
        Assert.AreEqual(2, block.Statements.Count);
        Assert.IsInstanceOfType<VarDeclStmt>(block.Statements[0]);
        Assert.IsInstanceOfType<ExprStmt>(block.Statements[1]);
    }

    [TestMethod]
    public void EmptyBlock()
    {
        var stmt = Parse("{}");
        var block = (BlockStmt)stmt;
        Assert.AreEqual(0, block.Statements.Count);
    }

    // ── If ────────────────────────────────────────────────────────────────

    [TestMethod]
    public void IfStatement()
    {
        var stmt = Parse("if (x > 0) { x = 0; }");
        var ifStmt = (IfStmt)stmt;
        Assert.IsInstanceOfType<BinaryExpr>(ifStmt.Condition);
        Assert.IsInstanceOfType<BlockStmt>(ifStmt.Then);
        Assert.IsNull(ifStmt.Else);
    }

    [TestMethod]
    public void IfElseStatement()
    {
        var stmt = Parse("if (x > 0) { x = 1; } else { x = 0; }");
        var ifStmt = (IfStmt)stmt;
        Assert.IsNotNull(ifStmt.Then);
        Assert.IsNotNull(ifStmt.Else);
        Assert.IsInstanceOfType<BlockStmt>(ifStmt.Then);
        Assert.IsInstanceOfType<BlockStmt>(ifStmt.Else);
    }

    // ── While ─────────────────────────────────────────────────────────────

    [TestMethod]
    public void WhileStatement()
    {
        var stmt = Parse("while (running) { update(); }");
        var whileStmt = (WhileStmt)stmt;
        Assert.IsInstanceOfType<IdentifierExpr>(whileStmt.Condition);
        Assert.AreEqual("running", ((IdentifierExpr)whileStmt.Condition).Name);
        Assert.IsInstanceOfType<BlockStmt>(whileStmt.Body);
    }

    // ── Do-While ──────────────────────────────────────────────────────────

    [TestMethod]
    public void DoWhileStatement()
    {
        var stmt = Parse("do { x++; } while (x < 10);");
        var doWhile = (DoWhileStmt)stmt;
        Assert.IsInstanceOfType<BlockStmt>(doWhile.Body);
        Assert.IsInstanceOfType<BinaryExpr>(doWhile.Condition);
        Assert.AreEqual(TokenType.Lt, ((BinaryExpr)doWhile.Condition).Op);
    }

    // ── For ───────────────────────────────────────────────────────────────

    [TestMethod]
    public void ForStatement()
    {
        var stmt = Parse("for (byte i = 0; i < 10; i++) { update(); }");
        var forStmt = (ForStmt)stmt;
        Assert.IsNotNull(forStmt.Init);
        Assert.IsInstanceOfType<VarDeclStmt>(forStmt.Init);
        Assert.AreEqual("i", ((VarDeclStmt)forStmt.Init).Name);
        Assert.IsNotNull(forStmt.Condition);
        Assert.IsInstanceOfType<BinaryExpr>(forStmt.Condition);
        Assert.IsNotNull(forStmt.Increment);
        Assert.IsInstanceOfType<UnaryExpr>(forStmt.Increment);
        Assert.IsInstanceOfType<BlockStmt>(forStmt.Body);
    }

    [TestMethod]
    public void ForStatement_InfiniteLoop()
    {
        var stmt = Parse("for (;;) { update(); }");
        var forStmt = (ForStmt)stmt;
        Assert.IsNull(forStmt.Init);
        Assert.IsNull(forStmt.Condition);
        Assert.IsNull(forStmt.Increment);
        Assert.IsInstanceOfType<BlockStmt>(forStmt.Body);
    }

    // ── Switch ────────────────────────────────────────────────────────────

    [TestMethod]
    public void SwitchStatement()
    {
        var stmt = Parse("switch (state) { case 0: x = 1; break; case 1: x = 2; break; default: x = 0; break; }");
        var switchStmt = (SwitchStmt)stmt;
        Assert.IsInstanceOfType<IdentifierExpr>(switchStmt.Value);
        Assert.AreEqual(3, switchStmt.Cases.Count);

        // case 0
        Assert.IsNotNull(switchStmt.Cases[0].Value);
        Assert.AreEqual(0L, ((IntLiteralExpr)switchStmt.Cases[0].Value!).Value);
        Assert.AreEqual(2, switchStmt.Cases[0].Body.Count); // x = 1; break;

        // case 1
        Assert.IsNotNull(switchStmt.Cases[1].Value);
        Assert.AreEqual(1L, ((IntLiteralExpr)switchStmt.Cases[1].Value!).Value);
        Assert.AreEqual(2, switchStmt.Cases[1].Body.Count);

        // default
        Assert.IsNull(switchStmt.Cases[2].Value);
        Assert.AreEqual(2, switchStmt.Cases[2].Body.Count);
    }

    // ── Return ────────────────────────────────────────────────────────────

    [TestMethod]
    public void ReturnVoid()
    {
        var stmt = Parse("return;");
        var ret = (ReturnStmt)stmt;
        Assert.IsNull(ret.Value);
    }

    [TestMethod]
    public void ReturnValue()
    {
        var stmt = Parse("return 42;");
        var ret = (ReturnStmt)stmt;
        Assert.IsNotNull(ret.Value);
        Assert.IsInstanceOfType<IntLiteralExpr>(ret.Value);
        Assert.AreEqual(42L, ((IntLiteralExpr)ret.Value).Value);
    }

    // ── Break / Continue ──────────────────────────────────────────────────

    [TestMethod]
    public void BreakStatement()
    {
        var stmt = Parse("break;");
        Assert.IsInstanceOfType<BreakStmt>(stmt);
    }

    [TestMethod]
    public void ContinueStatement()
    {
        var stmt = Parse("continue;");
        Assert.IsInstanceOfType<ContinueStmt>(stmt);
    }

    // ── Asm ───────────────────────────────────────────────────────────────

    [TestMethod]
    public void AsmBlock()
    {
        var stmt = Parse("asm { LDA #$00 };");
        var asm = (AsmStmt)stmt;
        Assert.IsFalse(string.IsNullOrEmpty(asm.Assembly));
        // The lexer tokenizes the content; we get tokens joined by spaces
        // LDA is an identifier, # is a hash, $00 could be error+IntLiteral or just tokens
        // Key assertion: the assembly text is captured
        Assert.IsTrue(asm.Assembly.Contains("LDA"));
    }

    // ── Nested / Complex ──────────────────────────────────────────────────

    [TestMethod]
    public void NestedIf()
    {
        var stmt = Parse("if (a) { if (b) { x = 1; } }");
        var outer = (IfStmt)stmt;
        Assert.IsInstanceOfType<BlockStmt>(outer.Then);
        var block = (BlockStmt)outer.Then;
        Assert.AreEqual(1, block.Statements.Count);
        var inner = (IfStmt)block.Statements[0];
        Assert.IsInstanceOfType<BlockStmt>(inner.Then);
    }

    [TestMethod]
    public void Assignment_AsStatement()
    {
        var stmt = Parse("x = 42;");
        var exprStmt = (ExprStmt)stmt;
        var assign = (AssignExpr)exprStmt.Expression;
        Assert.AreEqual(TokenType.Assign, assign.Op);
        Assert.AreEqual(42L, ((IntLiteralExpr)assign.Value).Value);
    }

    [TestMethod]
    public void CompoundAssignment_AsStatement()
    {
        var stmt = Parse("x += 1;");
        var exprStmt = (ExprStmt)stmt;
        var assign = (AssignExpr)exprStmt.Expression;
        Assert.AreEqual(TokenType.PlusAssign, assign.Op);
    }
}

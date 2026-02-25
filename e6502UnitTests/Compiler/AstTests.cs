using System.Collections.Generic;
using e6502.Avalonia.Compiler;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests.Compiler;

[TestClass]
public class AstTests
{
    [TestMethod]
    public void IntLiteralExpr_StoresValue()
    {
        var expr = new IntLiteralExpr(42, 1, 5);
        Assert.AreEqual(42L, expr.Value);
        Assert.AreEqual(1, expr.Line);
        Assert.AreEqual(5, expr.Col);
    }

    [TestMethod]
    public void BinaryExpr_StoresOperands()
    {
        var left = new IntLiteralExpr(1, 1, 1);
        var right = new IntLiteralExpr(2, 1, 5);
        var bin = new BinaryExpr(left, TokenType.Plus, right, 1, 1);

        Assert.AreSame(left, bin.Left);
        Assert.AreSame(right, bin.Right);
        Assert.AreEqual(TokenType.Plus, bin.Op);
    }

    [TestMethod]
    public void FuncDecl_StoresAllFields()
    {
        var retType = new TypeRef("int", false, 1, 1);
        var param = new ParamDecl(new TypeRef("byte", false, 1, 10), "x", 1, 10);
        var body = new BlockStmt(new List<Stmt>
        {
            new ReturnStmt(new IdentifierExpr("x", 2, 12), 2, 5)
        }, 1, 15);

        var func = new FuncDecl(retType, "add", new List<ParamDecl> { param }, body,
            false, true, 1, 1);

        Assert.AreEqual("add", func.Name);
        Assert.AreEqual("int", func.ReturnType.Name);
        Assert.AreEqual(1, func.Params.Count);
        Assert.AreEqual("x", func.Params[0].Name);
        Assert.IsFalse(func.IsResident);
        Assert.IsTrue(func.IsInterrupt);
    }

    [TestMethod]
    public void Program_HoldsDeclarations()
    {
        var global = new GlobalVarDecl(
            new TypeRef("byte", false, 1, 1), "count", null, false, 1, 1);
        var prog = new NccProgram(new List<Decl> { global }, 1, 1);

        Assert.AreEqual(1, prog.Declarations.Count);
        Assert.IsInstanceOfType(prog.Declarations[0], typeof(GlobalVarDecl));
    }
}

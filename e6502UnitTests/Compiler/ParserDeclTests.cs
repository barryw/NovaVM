using e6502.Avalonia.Compiler;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests.Compiler;

[TestClass]
public class ParserDeclTests
{
    private static NccProgram ParseProgram(string source)
    {
        var parser = Parser.FromSource(source);
        var program = parser.ParseProgram();
        Assert.AreEqual(0, parser.Errors.Count,
            string.Join("\n", parser.Errors));
        return program;
    }

    // ── Empty program ──────────────────────────────────────────────────────

    [TestMethod]
    public void EmptyProgram()
    {
        var prog = ParseProgram("");
        Assert.AreEqual(0, prog.Declarations.Count);
    }

    // ── Function declarations ──────────────────────────────────────────────

    [TestMethod]
    public void SimpleFunction()
    {
        var prog = ParseProgram("void main() { }");
        Assert.AreEqual(1, prog.Declarations.Count);
        var func = (FuncDecl)prog.Declarations[0];
        Assert.AreEqual("void", func.ReturnType.Name);
        Assert.AreEqual("main", func.Name);
        Assert.AreEqual(0, func.Params.Count);
        Assert.IsFalse(func.IsResident);
        Assert.IsFalse(func.IsInterrupt);
    }

    [TestMethod]
    public void FunctionWithParams()
    {
        var prog = ParseProgram("byte add(byte a, byte b) { return a + b; }");
        Assert.AreEqual(1, prog.Declarations.Count);
        var func = (FuncDecl)prog.Declarations[0];
        Assert.AreEqual("byte", func.ReturnType.Name);
        Assert.AreEqual("add", func.Name);
        Assert.AreEqual(2, func.Params.Count);
        Assert.AreEqual("byte", func.Params[0].Type.Name);
        Assert.AreEqual("a", func.Params[0].Name);
        Assert.AreEqual("byte", func.Params[1].Type.Name);
        Assert.AreEqual("b", func.Params[1].Name);
    }

    [TestMethod]
    public void FunctionResident()
    {
        var prog = ParseProgram("__resident void game_loop() { }");
        var func = (FuncDecl)prog.Declarations[0];
        Assert.AreEqual("game_loop", func.Name);
        Assert.IsTrue(func.IsResident);
        Assert.IsFalse(func.IsInterrupt);
    }

    [TestMethod]
    public void FunctionInterrupt()
    {
        var prog = ParseProgram("__interrupt void on_vsync() { }");
        var func = (FuncDecl)prog.Declarations[0];
        Assert.AreEqual("on_vsync", func.Name);
        Assert.IsFalse(func.IsResident);
        Assert.IsTrue(func.IsInterrupt);
    }

    [TestMethod]
    public void FunctionReturnTypePointer()
    {
        var prog = ParseProgram("byte* get_ptr() { return 0; }");
        var func = (FuncDecl)prog.Declarations[0];
        Assert.AreEqual("byte", func.ReturnType.Name);
        Assert.IsTrue(func.ReturnType.IsPointer);
        Assert.AreEqual("get_ptr", func.Name);
    }

    // ── Global variables ───────────────────────────────────────────────────

    [TestMethod]
    public void GlobalVariable()
    {
        var prog = ParseProgram("byte score;");
        Assert.AreEqual(1, prog.Declarations.Count);
        var v = (GlobalVarDecl)prog.Declarations[0];
        Assert.AreEqual("byte", v.Type.Name);
        Assert.AreEqual("score", v.Name);
        Assert.IsNull(v.Initializer);
        Assert.IsFalse(v.IsConst);
    }

    [TestMethod]
    public void GlobalVariableWithInit()
    {
        var prog = ParseProgram("byte score = 0;");
        var v = (GlobalVarDecl)prog.Declarations[0];
        Assert.AreEqual("score", v.Name);
        Assert.IsNotNull(v.Initializer);
        Assert.AreEqual(0L, ((IntLiteralExpr)v.Initializer).Value);
    }

    [TestMethod]
    public void GlobalConstant()
    {
        var prog = ParseProgram("const byte MAX = 10;");
        var v = (GlobalVarDecl)prog.Declarations[0];
        Assert.AreEqual("MAX", v.Name);
        Assert.IsTrue(v.IsConst);
        Assert.AreEqual(10L, ((IntLiteralExpr)v.Initializer!).Value);
    }

    // ── Global arrays ──────────────────────────────────────────────────────

    [TestMethod]
    public void GlobalArray()
    {
        var prog = ParseProgram("byte bullets[16];");
        Assert.AreEqual(1, prog.Declarations.Count);
        var a = (GlobalArrayDecl)prog.Declarations[0];
        Assert.AreEqual("byte", a.ElementType.Name);
        Assert.AreEqual("bullets", a.Name);
        Assert.AreEqual(16L, ((IntLiteralExpr)a.Size).Value);
        Assert.IsNull(a.Initializer);
    }

    [TestMethod]
    public void GlobalArrayWithInit()
    {
        var prog = ParseProgram("byte colors[] = { 1, 2, 3 };");
        var a = (GlobalArrayDecl)prog.Declarations[0];
        Assert.AreEqual("colors", a.Name);
        Assert.AreEqual(0L, ((IntLiteralExpr)a.Size).Value); // empty brackets
        Assert.IsNotNull(a.Initializer);
        Assert.AreEqual(3, a.Initializer!.Count);
        Assert.AreEqual(1L, ((IntLiteralExpr)a.Initializer[0]).Value);
        Assert.AreEqual(2L, ((IntLiteralExpr)a.Initializer[1]).Value);
        Assert.AreEqual(3L, ((IntLiteralExpr)a.Initializer[2]).Value);
    }

    // ── Struct ─────────────────────────────────────────────────────────────

    [TestMethod]
    public void StructDeclaration()
    {
        var prog = ParseProgram("struct Player { fixed x; fixed y; byte health; };");
        Assert.AreEqual(1, prog.Declarations.Count);
        var s = (StructDecl)prog.Declarations[0];
        Assert.AreEqual("Player", s.Name);
        Assert.AreEqual(3, s.Fields.Count);
        Assert.AreEqual("x", s.Fields[0].Name);
        Assert.AreEqual("fixed", s.Fields[0].Type.Name);
        Assert.AreEqual("y", s.Fields[1].Name);
        Assert.AreEqual("health", s.Fields[2].Name);
        Assert.AreEqual("byte", s.Fields[2].Type.Name);
    }

    [TestMethod]
    public void StructWithArrayField()
    {
        var prog = ParseProgram("struct Entity { byte name[16]; int x; };");
        var s = (StructDecl)prog.Declarations[0];
        Assert.AreEqual(2, s.Fields.Count);
        Assert.AreEqual("name", s.Fields[0].Name);
        Assert.IsNotNull(s.Fields[0].ArraySize);
        Assert.AreEqual(16L, ((IntLiteralExpr)s.Fields[0].ArraySize!).Value);
        Assert.AreEqual("x", s.Fields[1].Name);
        Assert.IsNull(s.Fields[1].ArraySize);
    }

    [TestMethod]
    public void StructMultipleFieldsSameType()
    {
        var prog = ParseProgram("struct Point { fixed x, y; };");
        var s = (StructDecl)prog.Declarations[0];
        Assert.AreEqual(2, s.Fields.Count);
        Assert.AreEqual("x", s.Fields[0].Name);
        Assert.AreEqual("fixed", s.Fields[0].Type.Name);
        Assert.AreEqual("y", s.Fields[1].Name);
        Assert.AreEqual("fixed", s.Fields[1].Type.Name);
    }

    // ── Enum ───────────────────────────────────────────────────────────────

    [TestMethod]
    public void EnumDeclaration()
    {
        var prog = ParseProgram("enum State { IDLE, WALK, JUMP, DEAD };");
        Assert.AreEqual(1, prog.Declarations.Count);
        var e = (EnumDecl)prog.Declarations[0];
        Assert.AreEqual("State", e.Name);
        Assert.AreEqual(4, e.Values.Count);
        Assert.AreEqual("IDLE", e.Values[0].Name);
        Assert.IsNull(e.Values[0].Value);
        Assert.AreEqual("DEAD", e.Values[3].Name);
    }

    [TestMethod]
    public void EnumWithExplicitValues()
    {
        var prog = ParseProgram("enum Color { RED = 2, GREEN = 5, BLUE = 6 };");
        var e = (EnumDecl)prog.Declarations[0];
        Assert.AreEqual(3, e.Values.Count);
        Assert.AreEqual("RED", e.Values[0].Name);
        Assert.AreEqual(2L, ((IntLiteralExpr)e.Values[0].Value!).Value);
        Assert.AreEqual("GREEN", e.Values[1].Name);
        Assert.AreEqual(5L, ((IntLiteralExpr)e.Values[1].Value!).Value);
        Assert.AreEqual("BLUE", e.Values[2].Name);
        Assert.AreEqual(6L, ((IntLiteralExpr)e.Values[2].Value!).Value);
    }

    // ── Preprocessor directives ────────────────────────────────────────────

    [TestMethod]
    public void IncludeDirective()
    {
        var prog = ParseProgram("#include \"player.c\"");
        Assert.AreEqual(1, prog.Declarations.Count);
        var inc = (IncludeDirective)prog.Declarations[0];
        Assert.AreEqual("player.c", inc.Path);
    }

    [TestMethod]
    public void IncludeAngleBracket()
    {
        var prog = ParseProgram("#include <nova.h>");
        Assert.AreEqual(1, prog.Declarations.Count);
        var inc = (IncludeDirective)prog.Declarations[0];
        Assert.AreEqual("nova.h", inc.Path);
    }

    [TestMethod]
    public void DefineDirective()
    {
        var prog = ParseProgram("#define MAX_ENEMIES 8");
        Assert.AreEqual(1, prog.Declarations.Count);
        var def = (DefineDirective)prog.Declarations[0];
        Assert.AreEqual("MAX_ENEMIES", def.Name);
        Assert.AreEqual("8", def.Value);
    }

    [TestMethod]
    public void AssetDirective()
    {
        var prog = ParseProgram("#asset sprite \"hero.gfx\"");
        Assert.AreEqual(1, prog.Declarations.Count);
        var asset = (AssetDirective)prog.Declarations[0];
        Assert.AreEqual("sprite", asset.AssetType);
        Assert.AreEqual("hero.gfx", asset.FilePath);
    }

    [TestMethod]
    public void PragmaDirective()
    {
        var prog = ParseProgram("#pragma overlay_size 8192");
        Assert.AreEqual(1, prog.Declarations.Count);
        var pragma = (PragmaDirective)prog.Declarations[0];
        Assert.AreEqual("overlay_size", pragma.Name);
        Assert.AreEqual("8192", pragma.Value);
    }

    // ── Multiple declarations ──────────────────────────────────────────────

    [TestMethod]
    public void MultipleDeclarations()
    {
        var prog = ParseProgram("""
            byte score;
            byte health = 100;
            void main() { }
            """);
        Assert.AreEqual(3, prog.Declarations.Count);
        Assert.IsInstanceOfType(prog.Declarations[0], typeof(GlobalVarDecl));
        Assert.IsInstanceOfType(prog.Declarations[1], typeof(GlobalVarDecl));
        Assert.IsInstanceOfType(prog.Declarations[2], typeof(FuncDecl));
    }

    [TestMethod]
    public void CompleteProgram()
    {
        var prog = ParseProgram("""
            #include "nova.h"
            #define MAX 10
            #asset sprite "hero.gfx"
            byte score;
            struct Player { fixed x; fixed y; };
            void main() { score = 0; }
            """);
        Assert.AreEqual(6, prog.Declarations.Count);
        Assert.IsInstanceOfType(prog.Declarations[0], typeof(IncludeDirective));
        Assert.IsInstanceOfType(prog.Declarations[1], typeof(DefineDirective));
        Assert.IsInstanceOfType(prog.Declarations[2], typeof(AssetDirective));
        Assert.IsInstanceOfType(prog.Declarations[3], typeof(GlobalVarDecl));
        Assert.IsInstanceOfType(prog.Declarations[4], typeof(StructDecl));
        Assert.IsInstanceOfType(prog.Declarations[5], typeof(FuncDecl));

        var inc = (IncludeDirective)prog.Declarations[0];
        Assert.AreEqual("nova.h", inc.Path);

        var def = (DefineDirective)prog.Declarations[1];
        Assert.AreEqual("MAX", def.Name);
        Assert.AreEqual("10", def.Value);

        var func = (FuncDecl)prog.Declarations[5];
        Assert.AreEqual("main", func.Name);
        Assert.AreEqual(1, func.Body.Statements.Count);
    }
}

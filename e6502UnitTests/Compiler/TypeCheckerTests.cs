using System;
using System.Linq;
using e6502.Avalonia.Compiler;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests.Compiler;

[TestClass]
public class TypeCheckerTests
{
    // ── Helper ────────────────────────────────────────────────────────────────

    private static TypeChecker CheckSource(string source)
    {
        var parser  = Parser.FromSource(source);
        var program = parser.ParseProgram();
        Assert.AreEqual(0, parser.Errors.Count, string.Join("\n", parser.Errors));
        var checker = new TypeChecker();
        checker.Check(program);
        return checker;
    }

    private static void AssertNoErrors(TypeChecker tc)
        => Assert.AreEqual(0, tc.Errors.Count, string.Join("\n", tc.Errors));

    private static void AssertHasError(TypeChecker tc, string fragment)
    {
        Assert.IsTrue(
            tc.Errors.Any(e => e.Contains(fragment, StringComparison.OrdinalIgnoreCase)),
            $"Expected error containing '{fragment}', got:\n{string.Join("\n", tc.Errors)}");
    }

    // ── Pass cases ────────────────────────────────────────────────────────────

    [TestMethod]
    public void SimpleGlobalVariableDeclaration()
    {
        var tc = CheckSource("byte x;");
        AssertNoErrors(tc);
    }

    [TestMethod]
    public void GlobalWithInitializer()
    {
        var tc = CheckSource("byte x = 42;");
        AssertNoErrors(tc);
    }

    [TestMethod]
    public void GlobalIntWithInitializer()
    {
        var tc = CheckSource("int x = 300;");
        AssertNoErrors(tc);
    }

    [TestMethod]
    public void FunctionDeclarationAndCall()
    {
        var tc = CheckSource("void foo() {} void main() { foo(); }");
        AssertNoErrors(tc);
    }

    [TestMethod]
    public void ForwardReferenceCall()
    {
        // main calls foo which is declared after main.
        var tc = CheckSource("void main() { foo(); } void foo() {}");
        AssertNoErrors(tc);
    }

    [TestMethod]
    public void StructDeclarationAndMemberAccess()
    {
        var tc = CheckSource(
            "struct Point { int x; int y; };" +
            "void main() { Point p; int v; v = p.x; }");
        AssertNoErrors(tc);
    }

    [TestMethod]
    public void EnumDeclarationAndUse()
    {
        var tc = CheckSource(
            "enum Dir { North, South, East, West };" +
            "void main() { Dir d; d = North; }");
        AssertNoErrors(tc);
    }

    [TestMethod]
    public void ArrayDeclarationAndIndexing()
    {
        var tc = CheckSource(
            "void main() { byte arr[10]; byte v; v = arr[3]; }");
        AssertNoErrors(tc);
    }

    [TestMethod]
    public void ImplicitByteToIntConversion()
    {
        var tc = CheckSource("void main() { byte b; int i; i = b; }");
        AssertNoErrors(tc);
    }

    [TestMethod]
    public void ArithmeticExpressionsCompatibleTypes()
    {
        var tc = CheckSource("void main() { byte a; byte b; byte c; c = a + b; }");
        AssertNoErrors(tc);
    }

    [TestMethod]
    public void BytePlusIntEqualsInt()
    {
        // byte + int → int; assign to int is fine.
        var tc = CheckSource("void main() { byte a; int b; int c; c = a + b; }");
        AssertNoErrors(tc);
    }

    [TestMethod]
    public void ComparisonOperatorsReturnBool()
    {
        var tc = CheckSource("void main() { byte a; byte b; bool c; c = a == b; }");
        AssertNoErrors(tc);
    }

    [TestMethod]
    public void CastExpression()
    {
        var tc = CheckSource("void main() { int x; byte b; b = (byte)x; }");
        AssertNoErrors(tc);
    }

    [TestMethod]
    public void SizeofReturnsUint()
    {
        var tc = CheckSource("void main() { uint s; s = sizeof(int); }");
        AssertNoErrors(tc);
    }

    [TestMethod]
    public void PointerDeclarationAndDereference()
    {
        var tc = CheckSource("void main() { byte* p; byte v; v = *p; }");
        AssertNoErrors(tc);
    }

    [TestMethod]
    public void StringLiteralAsBytePointer()
    {
        var tc = CheckSource("void main() { byte* s; s = \"hello\"; }");
        AssertNoErrors(tc);
    }

    [TestMethod]
    public void NestedScopesWithShadowing()
    {
        var tc = CheckSource(
            "void main() { byte x; { int x; x = 5; } x = 2; }");
        AssertNoErrors(tc);
    }

    [TestMethod]
    public void FunctionWithParams()
    {
        var tc = CheckSource("void add(byte a, int b) { int c; c = a + b; }");
        AssertNoErrors(tc);
    }

    [TestMethod]
    public void ReturnVoidFromVoidFunction()
    {
        var tc = CheckSource("void foo() { return; }");
        AssertNoErrors(tc);
    }

    [TestMethod]
    public void ReturnValueFromTypedFunction()
    {
        var tc = CheckSource("byte foo() { byte x; return x; }");
        AssertNoErrors(tc);
    }

    [TestMethod]
    public void WhileLoopWithNumericCondition()
    {
        var tc = CheckSource("void main() { byte i; while (i) { i = i - 1; } }");
        AssertNoErrors(tc);
    }

    [TestMethod]
    public void ForLoop()
    {
        var tc = CheckSource("void main() { for (byte i = 0; i < 10; i++) {} }");
        AssertNoErrors(tc);
    }

    [TestMethod]
    public void IfElse()
    {
        var tc = CheckSource("void main() { bool b; if (b) { } else { } }");
        AssertNoErrors(tc);
    }

    [TestMethod]
    public void SwitchOnByte()
    {
        var tc = CheckSource("void main() { byte x; switch (x) { case 0: break; case 1: break; } }");
        AssertNoErrors(tc);
    }

    [TestMethod]
    public void ArrowMemberAccessOnPointerToStruct()
    {
        var tc = CheckSource(
            "struct Node { int val; };" +
            "void foo(Node* n) { int v; v = n->val; }");
        AssertNoErrors(tc);
    }

    [TestMethod]
    public void GlobalArrayDeclaration()
    {
        var tc = CheckSource("byte buf[64];");
        AssertNoErrors(tc);
    }

    [TestMethod]
    public void NarrowingIntToByteIsWarning()
    {
        var tc = CheckSource("void main() { int x; byte b; b = x; }");
        AssertNoErrors(tc);
        Assert.IsTrue(tc.Warnings.Any(w => w.Contains("narrowing")),
            $"Expected narrowing warning, got: {string.Join("\n", tc.Warnings)}");
    }

    [TestMethod]
    public void LogicalAndOnBooleans()
    {
        var tc = CheckSource("void main() { bool a; bool b; bool c; c = a && b; }");
        AssertNoErrors(tc);
    }

    [TestMethod]
    public void BitwiseOpsOnIntegrals()
    {
        var tc = CheckSource("void main() { byte a; byte b; byte c; c = a & b; c = a | b; c = a ^ b; }");
        AssertNoErrors(tc);
    }

    [TestMethod]
    public void UnaryMinusOnNumeric()
    {
        var tc = CheckSource("void main() { int a; int b; b = -a; }");
        AssertNoErrors(tc);
    }

    [TestMethod]
    public void HexLiteralFitsInByte()
    {
        var tc = CheckSource("byte x = 0xFF;");
        AssertNoErrors(tc);
    }

    [TestMethod]
    public void HexLiteralBeyondByteIsUint()
    {
        // 0x100 does not fit in byte; assigning to uint should be fine.
        var tc = CheckSource("uint x = 0x100;");
        AssertNoErrors(tc);
    }

    [TestMethod]
    public void BinLiteralFitsInByte()
    {
        var tc = CheckSource("byte x = 0b11001100;");
        AssertNoErrors(tc);
    }

    [TestMethod]
    public void CharLiteralIsByte()
    {
        var tc = CheckSource("byte c = 'A';");
        AssertNoErrors(tc);
    }

    [TestMethod]
    public void DoWhileLoop()
    {
        var tc = CheckSource("void main() { byte i; do { i = i + 1; } while (i < 10); }");
        AssertNoErrors(tc);
    }

    // ── Error cases ───────────────────────────────────────────────────────────

    [TestMethod]
    public void UndeclaredVariable()
    {
        var tc = CheckSource("void main() { x = 1; }");
        AssertHasError(tc, "undeclared identifier 'x'");
    }

    [TestMethod]
    public void DuplicateVariableInSameScope()
    {
        var tc = CheckSource("void main() { byte x; byte x; }");
        AssertHasError(tc, "already declared");
    }

    [TestMethod]
    public void DuplicateGlobalVariable()
    {
        var tc = CheckSource("byte x; byte x;");
        AssertHasError(tc, "already declared");
    }

    [TestMethod]
    public void TypeMismatchBoolToInt()
    {
        var tc = CheckSource("void main() { int x; x = true; }");
        AssertHasError(tc, "cannot assign 'bool' to 'int'");
    }

    [TestMethod]
    public void TypeMismatchIntToBool()
    {
        var tc = CheckSource("void main() { bool b; b = 42; }");
        AssertHasError(tc, "cannot assign");
    }

    [TestMethod]
    public void WrongNumberOfArguments_TooFew()
    {
        var tc = CheckSource("void foo(byte a, byte b) {} void main() { foo(1); }");
        AssertHasError(tc, "expects 2 argument(s), got 1");
    }

    [TestMethod]
    public void WrongNumberOfArguments_TooMany()
    {
        var tc = CheckSource("void foo(byte a) {} void main() { foo(1, 2); }");
        AssertHasError(tc, "expects 1 argument(s), got 2");
    }

    [TestMethod]
    public void WrongArgumentType()
    {
        var tc = CheckSource("void foo(bool b) {} void main() { foo(42); }");
        AssertHasError(tc, "argument 1");
    }

    [TestMethod]
    public void MemberAccessOnNonStruct()
    {
        var tc = CheckSource("void main() { byte x; byte v; v = x.y; }");
        AssertHasError(tc, "requires struct type");
    }

    [TestMethod]
    public void IndexOnNonArray()
    {
        var tc = CheckSource("void main() { byte x; byte v; v = x[0]; }");
        AssertHasError(tc, "cannot index non-array");
    }

    [TestMethod]
    public void ReturnTypeMismatch()
    {
        var tc = CheckSource("byte foo() { bool b; return b; }");
        AssertHasError(tc, "cannot assign 'bool' to 'byte'");
    }

    [TestMethod]
    public void ReturnValueFromVoidFunction()
    {
        var tc = CheckSource("void foo() { return 1; }");
        AssertHasError(tc, "void function cannot return a value");
    }

    [TestMethod]
    public void ArithmeticOnBool()
    {
        var tc = CheckSource("void main() { bool a; bool b; bool c; c = a + b; }");
        AssertHasError(tc, "arithmetic operator on non-numeric type 'bool'");
    }

    [TestMethod]
    public void LogicalOperatorOnNonBool()
    {
        var tc = CheckSource("void main() { byte a; byte b; bool c; c = a && b; }");
        AssertHasError(tc, "logical operator requires bool operands");
    }

    [TestMethod]
    public void UndeclaredStructMember()
    {
        var tc = CheckSource(
            "struct Foo { int x; };" +
            "void main() { Foo f; int v; v = f.z; }");
        AssertHasError(tc, "has no member 'z'");
    }

    [TestMethod]
    public void ArrowOnNonPointer()
    {
        var tc = CheckSource(
            "struct Foo { int x; };" +
            "void main() { Foo f; int v; v = f->x; }");
        AssertHasError(tc, "'->' requires pointer to struct");
    }

    [TestMethod]
    public void CallNonFunction()
    {
        var tc = CheckSource("void main() { byte x; x(); }");
        AssertHasError(tc, "callee is not a function");
    }

    [TestMethod]
    public void BitwiseOnFixed()
    {
        var tc = CheckSource("void main() { fixed a; fixed b; fixed c; c = a & b; }");
        AssertHasError(tc, "bitwise operator requires integral operands");
    }

    [TestMethod]
    public void VoidFunctionCallInAssignment()
    {
        var tc = CheckSource("void foo() {} void main() { int x; x = foo(); }");
        AssertHasError(tc, "void");
    }

    [TestMethod]
    public void CrossEnumComparison_ShouldError()
    {
        var tc = CheckSource(
            "enum Color { Red, Green };" +
            "enum Dir { North, South };" +
            "void main() { Color c; Dir d; bool b; c = Red; d = North; b = c == d; }");
        AssertHasError(tc, "incompatible");
    }

    [TestMethod]
    public void EnumValues_AutoIncrement()
    {
        // Verify enum values parse and check without errors
        var tc = CheckSource(
            "enum Dir { North, South = 5, East, West };" +
            "void main() { Dir d; d = North; }");
        AssertNoErrors(tc);
    }
}

using System.Collections.Generic;
using System.Linq;
using e6502.Avalonia.Compiler;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests.Compiler;

[TestClass]
public class TypeTests
{
    // ── Primitive sizes ────────────────────────────────────────────────────

    [TestMethod] public void Byte_Is_1_Byte()   => Assert.AreEqual(1, NccType.Byte.SizeInBytes);
    [TestMethod] public void Int_Is_2_Bytes()   => Assert.AreEqual(2, NccType.Int.SizeInBytes);
    [TestMethod] public void Uint_Is_2_Bytes()  => Assert.AreEqual(2, NccType.Uint.SizeInBytes);
    [TestMethod] public void Bool_Is_1_Byte()   => Assert.AreEqual(1, NccType.Bool.SizeInBytes);
    [TestMethod] public void Fixed_Is_2_Bytes() => Assert.AreEqual(2, NccType.Fixed.SizeInBytes);
    [TestMethod] public void Ufixed_Is_2_Bytes()=> Assert.AreEqual(2, NccType.Ufixed.SizeInBytes);
    [TestMethod] public void Void_Is_0_Bytes()  => Assert.AreEqual(0, NccType.Void.SizeInBytes);

    // ── Primitive names ────────────────────────────────────────────────────

    [TestMethod] public void Byte_Name()   => Assert.AreEqual("byte",   NccType.Byte.Name);
    [TestMethod] public void Int_Name()    => Assert.AreEqual("int",    NccType.Int.Name);
    [TestMethod] public void Uint_Name()   => Assert.AreEqual("uint",   NccType.Uint.Name);
    [TestMethod] public void Bool_Name()   => Assert.AreEqual("bool",   NccType.Bool.Name);
    [TestMethod] public void Fixed_Name()  => Assert.AreEqual("fixed",  NccType.Fixed.Name);
    [TestMethod] public void Ufixed_Name() => Assert.AreEqual("ufixed", NccType.Ufixed.Name);
    [TestMethod] public void Void_Name()   => Assert.AreEqual("void",   NccType.Void.Name);

    // ── PointerType ────────────────────────────────────────────────────────

    [TestMethod]
    public void Pointer_Is_Always_2_Bytes()
    {
        Assert.AreEqual(2, new PointerType(NccType.Byte).SizeInBytes);
        Assert.AreEqual(2, new PointerType(NccType.Int).SizeInBytes);
        Assert.AreEqual(2, new PointerType(NccType.Void).SizeInBytes);
    }

    [TestMethod]
    public void Pointer_Name_Includes_Base()
    {
        Assert.AreEqual("byte*", new PointerType(NccType.Byte).Name);
        Assert.AreEqual("int*",  new PointerType(NccType.Int).Name);
    }

    // ── ArrayType ──────────────────────────────────────────────────────────

    [TestMethod]
    public void Array_Size_Is_Element_Times_Count()
    {
        Assert.AreEqual(10,  new ArrayType(NccType.Byte, 10).SizeInBytes);   // 1 * 10
        Assert.AreEqual(20,  new ArrayType(NccType.Int,  10).SizeInBytes);   // 2 * 10
        Assert.AreEqual(0,   new ArrayType(NccType.Byte, 0).SizeInBytes);    // 0-length
        Assert.AreEqual(200, new ArrayType(NccType.Uint, 100).SizeInBytes);  // 2 * 100
    }

    [TestMethod]
    public void Array_Name()
    {
        Assert.AreEqual("byte[10]", new ArrayType(NccType.Byte, 10).Name);
        Assert.AreEqual("int[5]",   new ArrayType(NccType.Int,  5).Name);
    }

    // ── StructType ─────────────────────────────────────────────────────────

    [TestMethod]
    public void Struct_Size_Is_Sum_Of_Fields()
    {
        var fields = new List<StructField>
        {
            new("x", NccType.Int,  0),   // 2
            new("y", NccType.Int,  2),   // 2
            new("flags", NccType.Byte, 4), // 1
        };
        var s = new StructType("Point", fields);
        Assert.AreEqual(5, s.SizeInBytes);
    }

    [TestMethod]
    public void Struct_Empty_Is_0_Bytes()
    {
        var s = new StructType("Empty", []);
        Assert.AreEqual(0, s.SizeInBytes);
    }

    [TestMethod]
    public void Struct_Name()
    {
        var s = new StructType("Sprite", []);
        Assert.AreEqual("Sprite", s.Name);
    }

    // ── EnumType ───────────────────────────────────────────────────────────

    [TestMethod]
    public void Enum_Is_1_Byte()
    {
        Assert.AreEqual(1, new EnumType("Direction", new List<EnumMember>()).SizeInBytes);
    }

    // ── FuncType ───────────────────────────────────────────────────────────

    [TestMethod]
    public void Func_Is_0_Bytes()
    {
        var f = new FuncType(NccType.Void, []);
        Assert.AreEqual(0, f.SizeInBytes);
    }

    // ── IsNumeric ──────────────────────────────────────────────────────────

    [TestMethod]
    public void IsNumeric_True_For_Numeric_Types()
    {
        Assert.IsTrue(NccType.Byte.IsNumeric);
        Assert.IsTrue(NccType.Int.IsNumeric);
        Assert.IsTrue(NccType.Uint.IsNumeric);
        Assert.IsTrue(NccType.Fixed.IsNumeric);
        Assert.IsTrue(NccType.Ufixed.IsNumeric);
    }

    [TestMethod]
    public void IsNumeric_False_For_NonNumeric()
    {
        Assert.IsFalse(NccType.Bool.IsNumeric);
        Assert.IsFalse(NccType.Void.IsNumeric);
        Assert.IsFalse(new PointerType(NccType.Byte).IsNumeric);
    }

    // ── IsIntegral ─────────────────────────────────────────────────────────

    [TestMethod]
    public void IsIntegral_True_For_Integral_Types()
    {
        Assert.IsTrue(NccType.Byte.IsIntegral);
        Assert.IsTrue(NccType.Int.IsIntegral);
        Assert.IsTrue(NccType.Uint.IsIntegral);
    }

    [TestMethod]
    public void IsIntegral_False_For_NonIntegral()
    {
        Assert.IsFalse(NccType.Fixed.IsIntegral);
        Assert.IsFalse(NccType.Ufixed.IsIntegral);
        Assert.IsFalse(NccType.Bool.IsIntegral);
        Assert.IsFalse(NccType.Void.IsIntegral);
    }

    // ── CanImplicitlyConvert ───────────────────────────────────────────────

    [TestMethod]
    public void Same_Type_Is_Always_Convertible()
    {
        Assert.IsTrue(NccType.CanImplicitlyConvert(NccType.Byte,   NccType.Byte));
        Assert.IsTrue(NccType.CanImplicitlyConvert(NccType.Int,    NccType.Int));
        Assert.IsTrue(NccType.CanImplicitlyConvert(NccType.Void,   NccType.Void));
        Assert.IsTrue(NccType.CanImplicitlyConvert(NccType.Fixed,  NccType.Fixed));
        Assert.IsTrue(NccType.CanImplicitlyConvert(NccType.Ufixed, NccType.Ufixed));
    }

    [TestMethod]
    public void Byte_Widens_To_Int()
        => Assert.IsTrue(NccType.CanImplicitlyConvert(NccType.Byte, NccType.Int));

    [TestMethod]
    public void Byte_Widens_To_Uint()
        => Assert.IsTrue(NccType.CanImplicitlyConvert(NccType.Byte, NccType.Uint));

    [TestMethod]
    public void Byte_Converts_To_Fixed()
        => Assert.IsTrue(NccType.CanImplicitlyConvert(NccType.Byte, NccType.Fixed));

    [TestMethod]
    public void Byte_Converts_To_Ufixed()
        => Assert.IsTrue(NccType.CanImplicitlyConvert(NccType.Byte, NccType.Ufixed));

    [TestMethod]
    public void Int_Converts_To_Fixed()
        => Assert.IsTrue(NccType.CanImplicitlyConvert(NccType.Int, NccType.Fixed));

    [TestMethod]
    public void Uint_Converts_To_Ufixed()
        => Assert.IsTrue(NccType.CanImplicitlyConvert(NccType.Uint, NccType.Ufixed));

    [TestMethod]
    public void Int_Does_Not_Convert_To_Byte()
        => Assert.IsFalse(NccType.CanImplicitlyConvert(NccType.Int, NccType.Byte));

    [TestMethod]
    public void Uint_Does_Not_Convert_To_Int()
        => Assert.IsFalse(NccType.CanImplicitlyConvert(NccType.Uint, NccType.Int));

    [TestMethod]
    public void Fixed_Does_Not_Convert_To_Byte()
        => Assert.IsFalse(NccType.CanImplicitlyConvert(NccType.Fixed, NccType.Byte));

    [TestMethod]
    public void Bool_Does_Not_Convert_To_Int()
        => Assert.IsFalse(NccType.CanImplicitlyConvert(NccType.Bool, NccType.Int));

    [TestMethod]
    public void Int_Does_Not_Convert_To_Ufixed()
        => Assert.IsFalse(NccType.CanImplicitlyConvert(NccType.Int, NccType.Ufixed));

    // ── ResolveType ────────────────────────────────────────────────────────

    [TestMethod]
    public void ResolveType_Returns_Correct_Singletons()
    {
        Assert.AreSame(NccType.Byte,   NccType.ResolveType("byte"));
        Assert.AreSame(NccType.Int,    NccType.ResolveType("int"));
        Assert.AreSame(NccType.Uint,   NccType.ResolveType("uint"));
        Assert.AreSame(NccType.Bool,   NccType.ResolveType("bool"));
        Assert.AreSame(NccType.Fixed,  NccType.ResolveType("fixed"));
        Assert.AreSame(NccType.Ufixed, NccType.ResolveType("ufixed"));
        Assert.AreSame(NccType.Void,   NccType.ResolveType("void"));
    }

    [TestMethod]
    public void ResolveType_Returns_Null_For_Unknown()
    {
        Assert.IsNull(NccType.ResolveType("string"));
        Assert.IsNull(NccType.ResolveType("MyStruct"));
        Assert.IsNull(NccType.ResolveType(""));
        Assert.IsNull(NccType.ResolveType("INT"));   // case-sensitive
    }
}

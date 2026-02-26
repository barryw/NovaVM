using System.Linq;
using e6502.Avalonia.Input;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class FontKeymapTests
{
    [TestMethod]
    public void PetsciiUpper_ShiftA_ReturnsGraphicChar()
    {
        Assert.IsTrue(FontKeymap.PetsciiUpper.TryMap(KeyMod.Shift, 'A', out byte code));
        Assert.AreEqual(0x80, code);
    }

    [TestMethod]
    public void PetsciiUpper_ShiftZ_ReturnsGraphicChar()
    {
        Assert.IsTrue(FontKeymap.PetsciiUpper.TryMap(KeyMod.Shift, 'Z', out byte code));
        Assert.AreEqual(0x99, code);
    }

    [TestMethod]
    public void PetsciiUpper_CtrlA_ReturnsGraphicChar()
    {
        Assert.IsTrue(FontKeymap.PetsciiUpper.TryMap(KeyMod.Ctrl, 'A', out byte code));
        Assert.AreEqual(0xA0, code);
    }

    [TestMethod]
    public void PetsciiUpper_CtrlZ_ReturnsGraphicChar()
    {
        Assert.IsTrue(FontKeymap.PetsciiUpper.TryMap(KeyMod.Ctrl, 'Z', out byte code));
        Assert.AreEqual(0xB9, code);
    }

    [TestMethod]
    public void None_ShiftA_ReturnsFalse()
    {
        Assert.IsFalse(FontKeymap.None.TryMap(KeyMod.Shift, 'A', out _));
    }

    [TestMethod]
    public void None_HasMappings_IsFalse()
    {
        Assert.IsFalse(FontKeymap.None.HasMappings);
    }

    [TestMethod]
    public void PetsciiUpper_HasMappings_IsTrue()
    {
        Assert.IsTrue(FontKeymap.PetsciiUpper.HasMappings);
    }

    [TestMethod]
    public void PetsciiUpper_LowercaseKey_StillMaps()
    {
        // TryMap should uppercase the key internally
        Assert.IsTrue(FontKeymap.PetsciiUpper.TryMap(KeyMod.Shift, 'a', out byte code));
        Assert.AreEqual(0x80, code);
    }

    [TestMethod]
    public void GetMappings_ReturnsAllShiftMappings()
    {
        var mappings = FontKeymap.PetsciiUpper.GetMappings(KeyMod.Shift).ToList();
        Assert.AreEqual(26, mappings.Count);
    }
}

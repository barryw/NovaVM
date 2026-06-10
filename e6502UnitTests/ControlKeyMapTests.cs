using Avalonia.Input;
using e6502.Avalonia.Input;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

/// <summary>
/// Ctrl + A-Z must map to ASCII control codes (0x01-0x1A) so the emulator's
/// key handler feeds editor commands (Ctrl-C=Copy=0x03, Ctrl-Q prefix=0x11, etc.)
/// to the running program instead of inserting the literal letter.
/// </summary>
[TestClass]
public class ControlKeyMapTests
{
    [TestMethod]
    public void CtrlS_MapsToControlCode()
    {
        Assert.IsTrue(ControlKeyMap.TryMap(Key.S, KeyModifiers.Control, out byte code));
        Assert.AreEqual(0x13, code);
    }

    [TestMethod]
    public void CtrlA_MapsToOne()
    {
        Assert.IsTrue(ControlKeyMap.TryMap(Key.A, KeyModifiers.Control, out byte code));
        Assert.AreEqual(0x01, code);
    }

    [TestMethod]
    public void CtrlZ_MapsToTwentySix()
    {
        Assert.IsTrue(ControlKeyMap.TryMap(Key.Z, KeyModifiers.Control, out byte code));
        Assert.AreEqual(0x1A, code);
    }

    [TestMethod]
    public void CtrlQ_MapsToCommandPrefixControlCode()
    {
        Assert.IsTrue(ControlKeyMap.TryMap(Key.Q, KeyModifiers.Control, out byte code));
        Assert.AreEqual(0x11, code);
    }

    [TestMethod]
    public void MetaAlone_DoesNotMap()
    {
        // macOS Cmd (Meta) is reserved for host app shortcuts, not editor control codes.
        Assert.IsFalse(ControlKeyMap.TryMap(Key.Q, KeyModifiers.Meta, out byte code));
        Assert.AreEqual(0, code);
    }

    [TestMethod]
    public void LetterWithoutModifier_DoesNotMap()
    {
        Assert.IsFalse(ControlKeyMap.TryMap(Key.S, KeyModifiers.None, out byte code));
        Assert.AreEqual(0, code);
    }

    [TestMethod]
    public void CtrlNonLetter_DoesNotMap()
    {
        Assert.IsFalse(ControlKeyMap.TryMap(Key.D1, KeyModifiers.Control, out _));
        Assert.IsFalse(ControlKeyMap.TryMap(Key.Enter, KeyModifiers.Control, out _));
    }
}

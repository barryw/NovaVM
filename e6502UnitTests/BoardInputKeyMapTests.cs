using Avalonia.Input;
using e6502.Avalonia.Hardware;
using e6502.Avalonia.Input;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

/// <summary>
/// Maps host keyboard keys to board joystick/fire button bits ($BA9C layout)
/// so the Avalonia GUI can drive JOY(). Arrow keys = directions, Z/X = fires.
/// </summary>
[TestClass]
public class BoardInputKeyMapTests
{
    [TestMethod]
    public void ArrowKeys_MapToJoystickDirectionBits()
    {
        Assert.IsTrue(BoardInputKeyMap.TryMap(Key.Up, out byte up));
        Assert.AreEqual(VgcConstants.BoardButtonUp, up);
        Assert.IsTrue(BoardInputKeyMap.TryMap(Key.Down, out byte down));
        Assert.AreEqual(VgcConstants.BoardButtonDown, down);
        Assert.IsTrue(BoardInputKeyMap.TryMap(Key.Left, out byte left));
        Assert.AreEqual(VgcConstants.BoardButtonLeft, left);
        Assert.IsTrue(BoardInputKeyMap.TryMap(Key.Right, out byte right));
        Assert.AreEqual(VgcConstants.BoardButtonRight, right);
    }

    [TestMethod]
    public void ZX_MapToFireButtons()
    {
        Assert.IsTrue(BoardInputKeyMap.TryMap(Key.Z, out byte z));
        Assert.AreEqual(VgcConstants.BoardButtonFire1, z);
        Assert.IsTrue(BoardInputKeyMap.TryMap(Key.X, out byte x));
        Assert.AreEqual(VgcConstants.BoardButtonFire2, x);
    }

    [TestMethod]
    public void UnmappedKey_ReturnsFalseAndZero()
    {
        Assert.IsFalse(BoardInputKeyMap.TryMap(Key.A, out byte bit));
        Assert.AreEqual(0, bit);
    }
}

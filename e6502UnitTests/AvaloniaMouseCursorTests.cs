using e6502.Avalonia.Hardware;
using e6502.Avalonia.Rendering;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class AvaloniaMouseCursorTests
{
    [TestMethod]
    public void MouseRegisters_CommitVisibleStateOnlyOnVBlank()
    {
        var vgc = new VirtualGraphicsController();

        vgc.Write(VgcConstants.RegMouseCtrl, VgcConstants.MouseCtrlEnable);
        vgc.Write(VgcConstants.RegMouseXLo, 42);
        vgc.Write(VgcConstants.RegMouseXHi, 1);
        vgc.Write(VgcConstants.RegMouseY, 99);
        vgc.Write(VgcConstants.RegMouseButtons, VgcConstants.MouseButtonLeft);

        var before = vgc.GetMouseCursorState();
        Assert.IsFalse(before.Enabled, "Visible mouse state must not update until vblank commit.");
        Assert.AreEqual(0, before.X);
        Assert.AreEqual(0, before.Y);

        vgc.IncrementFrameCounter();

        var after = vgc.GetMouseCursorState();
        Assert.IsTrue(after.Enabled);
        Assert.AreEqual(298, after.X);
        Assert.AreEqual(99, after.Y);
        Assert.AreEqual(VgcConstants.MouseButtonLeft, after.Buttons & VgcConstants.MouseButtonMask);
    }

    [TestMethod]
    public void MouseCursorRenderer_UsesSelectedShapeSlotAndTransparency()
    {
        var vgc = new VirtualGraphicsController();
        vgc.SetSpritePixelInSlot(255, 0, 0, 7);
        vgc.SetSpritePixelInSlot(255, 1, 0, 0);
        vgc.Write(VgcConstants.RegMouseCtrl, VgcConstants.MouseCtrlEnable);
        vgc.Write(VgcConstants.RegMouseShape, 255);
        vgc.Write(VgcConstants.RegMouseXLo, 10);
        vgc.Write(VgcConstants.RegMouseY, 20);
        vgc.IncrementFrameCounter();

        byte[] shapeRam = vgc.GetSpriteShapeRam().ToArray();
        var cursor = vgc.GetMouseCursorState();

        Assert.IsTrue(MouseCursorRenderer.TrySample(cursor, shapeRam, 10, 20, 3, out byte color));
        Assert.AreEqual(7, color);
        Assert.IsFalse(MouseCursorRenderer.TrySample(cursor, shapeRam, 11, 20, 3, out _),
            "Color 0 in cursor shape RAM is transparent.");
    }
}

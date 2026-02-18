using System;
using e6502.TUI.Hardware;
using e6502.TUI.Rendering;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class SpriteRendererTests
{
    // -------------------------------------------------------------------------
    // Helpers
    // -------------------------------------------------------------------------

    /// <summary>
    /// Writes 6-byte sprite register block for one sprite.
    /// </summary>
    private static void SetSprite(VirtualGraphicsController vgc, int index,
        int x, int y, byte color, byte flags = 0, byte shapeIdx = 0, byte priority = 0)
    {
        int baseAddr = VgcConstants.SpriteRegsBase + index * VgcConstants.SpriteBytesEach;
        vgc.Write((ushort)(baseAddr + VgcConstants.SprOffXLow),      (byte)(x & 0xFF));
        vgc.Write((ushort)(baseAddr + VgcConstants.SprOffXHighFlags), (byte)(((x >> 8) & 0x01) | (flags & 0xFE)));
        vgc.Write((ushort)(baseAddr + VgcConstants.SprOffY),          (byte)(y & 0xFF));
        vgc.Write((ushort)(baseAddr + VgcConstants.SprOffColor),      color);
        vgc.Write((ushort)(baseAddr + VgcConstants.SprOffPriority),   priority);
        vgc.Write((ushort)(baseAddr + VgcConstants.SprOffShape),      shapeIdx);
    }

    /// <summary>
    /// Enables or disables a sprite via the enable registers.
    /// </summary>
    private static void EnableSprite(VirtualGraphicsController vgc, int index, bool enable)
    {
        ushort regAddr = (ushort)(index < 8 ? VgcConstants.RegSpriteEn : VgcConstants.RegSpriteEnH);
        int bit = index < 8 ? index : index - 8;
        byte current = vgc.Read(regAddr);
        byte updated = enable
            ? (byte)(current | (1 << bit))
            : (byte)(current & ~(1 << bit));
        vgc.Write(regAddr, updated);
    }

    /// <summary>
    /// Writes 32 bytes of shape data for the given shape index.
    /// </summary>
    private static void SetShape(VirtualGraphicsController vgc, int shapeIdx, byte[] data)
    {
        int baseAddr = VgcConstants.SpriteShapeBase + shapeIdx * VgcConstants.SpriteShapeSize;
        for (int i = 0; i < Math.Min(data.Length, VgcConstants.SpriteShapeSize); i++)
            vgc.Write((ushort)(baseAddr + i), data[i]);
    }

    /// <summary>
    /// Plots a single gfx pixel by issuing a PLOT command through the VGC.
    /// </summary>
    private static void PlotGfxPixel(VirtualGraphicsController vgc, int x, int y, byte color)
    {
        // Set draw color
        vgc.Write(VgcConstants.RegGfxP0, color);
        vgc.Write(VgcConstants.RegGfxP1, 0);
        vgc.Write(VgcConstants.RegGfxCmd, VgcConstants.GfxCmdGcolor);

        // Plot pixel
        vgc.Write(VgcConstants.RegGfxP0, (byte)(x & 0xFF));
        vgc.Write(VgcConstants.RegGfxP1, (byte)((x >> 8) & 0xFF));
        vgc.Write(VgcConstants.RegGfxP2, (byte)(y & 0xFF));
        vgc.Write(VgcConstants.RegGfxP3, (byte)((y >> 8) & 0xFF));
        vgc.Write(VgcConstants.RegGfxCmd, VgcConstants.GfxCmdPlot);
    }

    // -------------------------------------------------------------------------
    // Tests: GetSpritePixels
    // -------------------------------------------------------------------------

    [TestMethod]
    public void DisabledSprite_ReturnsNoPixels()
    {
        var vgc = new VirtualGraphicsController();

        // Set up a sprite with a solid top-left pixel but do NOT enable it.
        var shapeData = new byte[32];
        shapeData[0] = 0x80; // row 0 hi: bit 15 set → pixel at col 0
        shapeData[1] = 0x00;
        SetShape(vgc, 0, shapeData);
        SetSprite(vgc, 0, x: 0, y: 0, color: 1, shapeIdx: 0);
        // sprite remains disabled (enable register default = 0)

        var pixels = SpriteRenderer.GetSpritePixels(vgc, 0);

        Assert.AreEqual(0, pixels.Count, "Disabled sprite must produce no pixels.");
    }

    [TestMethod]
    public void EnabledSpriteWithShape_ProducesExpectedPixels()
    {
        var vgc = new VirtualGraphicsController();

        // Shape: only the top-left pixel (row 0, col 0) is set.
        var shapeData = new byte[32];
        shapeData[0] = 0x80; // hi byte of row 0: bit 15 = col 0
        shapeData[1] = 0x00;
        SetShape(vgc, 0, shapeData);
        SetSprite(vgc, 0, x: 10, y: 20, color: 3, shapeIdx: 0);
        EnableSprite(vgc, 0, true);

        var pixels = SpriteRenderer.GetSpritePixels(vgc, 0);

        Assert.AreEqual(1, pixels.Count, "Exactly one pixel expected.");
        Assert.AreEqual(10, pixels[0].ScreenX);
        Assert.AreEqual(20, pixels[0].ScreenY);
        Assert.AreEqual(3, pixels[0].Color);
    }

    [TestMethod]
    public void XFlip_ReversesHorizontalPixelPositions()
    {
        var vgc = new VirtualGraphicsController();

        // Shape: top-right pixel only (row 0, col 15).
        // Without flip: col 15 → ScreenX = x + 15
        // With X-flip:  srcCol becomes 15-col; when col=0, srcCol=15 → pixel at ScreenX = x + 0
        var shapeData = new byte[32];
        shapeData[0] = 0x00; // hi byte of row 0: col 15 is bit 0 of lo byte
        shapeData[1] = 0x01; // lo byte: bit 0 = col 15
        SetShape(vgc, 0, shapeData);

        // Without flip first.
        SetSprite(vgc, 0, x: 0, y: 0, color: 1, flags: 0x00, shapeIdx: 0);
        EnableSprite(vgc, 0, true);
        var pixelsNoFlip = SpriteRenderer.GetSpritePixels(vgc, 0);
        Assert.AreEqual(1, pixelsNoFlip.Count);
        Assert.AreEqual(15, pixelsNoFlip[0].ScreenX, "Without X-flip, pixel should be at col 15.");

        // Now with X-flip (flag bit 7 = 0x80, stored in XHighFlags & 0xFE).
        SetSprite(vgc, 0, x: 0, y: 0, color: 1, flags: 0x80, shapeIdx: 0);
        var pixelsXFlip = SpriteRenderer.GetSpritePixels(vgc, 0);
        Assert.AreEqual(1, pixelsXFlip.Count);
        Assert.AreEqual(0, pixelsXFlip[0].ScreenX, "With X-flip, pixel should mirror to col 0.");
    }

    [TestMethod]
    public void YFlip_ReversesVerticalPixelPositions()
    {
        var vgc = new VirtualGraphicsController();

        // Shape: only the last row (row 15), col 0.
        var shapeData = new byte[32];
        shapeData[30] = 0x80; // row 15 hi byte
        shapeData[31] = 0x00;
        SetShape(vgc, 0, shapeData);

        // Without Y-flip: pixel appears at row 15.
        SetSprite(vgc, 0, x: 0, y: 0, color: 1, flags: 0x00, shapeIdx: 0);
        EnableSprite(vgc, 0, true);
        var pixelsNoFlip = SpriteRenderer.GetSpritePixels(vgc, 0);
        Assert.AreEqual(1, pixelsNoFlip.Count);
        Assert.AreEqual(15, pixelsNoFlip[0].ScreenY, "Without Y-flip, pixel should be at row 15.");

        // With Y-flip (flag bit 6 = 0x40): row 15 source maps to display row 0.
        SetSprite(vgc, 0, x: 0, y: 0, color: 1, flags: 0x40, shapeIdx: 0);
        var pixelsYFlip = SpriteRenderer.GetSpritePixels(vgc, 0);
        Assert.AreEqual(1, pixelsYFlip.Count);
        Assert.AreEqual(0, pixelsYFlip[0].ScreenY, "With Y-flip, pixel should mirror to row 0.");
    }

    // -------------------------------------------------------------------------
    // Tests: DetectCollisions — sprite-sprite
    // -------------------------------------------------------------------------

    [TestMethod]
    public void CollisionDetected_WhenSpritesOverlap()
    {
        var vgc = new VirtualGraphicsController();

        // Both sprites have a single pixel at the same location.
        var shapeData = new byte[32];
        shapeData[0] = 0x80; // row 0, col 0
        SetShape(vgc, 0, shapeData);
        SetShape(vgc, 1, shapeData);

        SetSprite(vgc, 0, x: 5, y: 5, color: 1, shapeIdx: 0);
        SetSprite(vgc, 1, x: 5, y: 5, color: 2, shapeIdx: 1);
        EnableSprite(vgc, 0, true);
        EnableSprite(vgc, 1, true);

        var (ss, _) = SpriteRenderer.DetectCollisions(vgc);

        Assert.IsTrue((ss & 0x01) != 0, "Sprite 0 collision bit must be set.");
        Assert.IsTrue((ss & 0x02) != 0, "Sprite 1 collision bit must be set.");
    }

    [TestMethod]
    public void NoCollision_WhenSpritesDoNotOverlap()
    {
        var vgc = new VirtualGraphicsController();

        // Sprite 0 at (0,0), sprite 1 at (20,20) — no overlap for 16×16 shapes.
        var shapeData = new byte[32];
        shapeData[0] = 0x80; // single pixel at row 0, col 0
        SetShape(vgc, 0, shapeData);
        SetShape(vgc, 1, shapeData);

        SetSprite(vgc, 0, x: 0,  y: 0,  color: 1, shapeIdx: 0);
        SetSprite(vgc, 1, x: 20, y: 20, color: 2, shapeIdx: 1);
        EnableSprite(vgc, 0, true);
        EnableSprite(vgc, 1, true);

        var (ss, _) = SpriteRenderer.DetectCollisions(vgc);

        Assert.AreEqual(0, ss, "No sprite-sprite collision expected.");
    }

    // -------------------------------------------------------------------------
    // Tests: DetectCollisions — sprite-background
    // -------------------------------------------------------------------------

    [TestMethod]
    public void SpriteBgCollision_WhenSpriteOverlapsGfxPixel()
    {
        var vgc = new VirtualGraphicsController();

        // Put a gfx pixel at (5, 5).
        PlotGfxPixel(vgc, 5, 5, color: 1);

        // Sprite 0 has a single pixel at (5, 5).
        var shapeData = new byte[32];
        shapeData[0] = 0x80; // row 0, col 0
        SetShape(vgc, 0, shapeData);
        SetSprite(vgc, 0, x: 5, y: 5, color: 3, shapeIdx: 0);
        EnableSprite(vgc, 0, true);

        var (_, sb) = SpriteRenderer.DetectCollisions(vgc);

        Assert.IsTrue((sb & 0x01) != 0, "Sprite 0 background collision bit must be set.");
    }

    [TestMethod]
    public void NoSpriteBgCollision_WhenSpriteDoesNotOverlapGfxPixel()
    {
        var vgc = new VirtualGraphicsController();

        // Gfx pixel at (50, 50), sprite at (0, 0).
        PlotGfxPixel(vgc, 50, 50, color: 1);

        var shapeData = new byte[32];
        shapeData[0] = 0x80; // row 0, col 0 → pixel at (0, 0)
        SetShape(vgc, 0, shapeData);
        SetSprite(vgc, 0, x: 0, y: 0, color: 3, shapeIdx: 0);
        EnableSprite(vgc, 0, true);

        var (_, sb) = SpriteRenderer.DetectCollisions(vgc);

        Assert.AreEqual(0, sb, "No sprite-background collision expected.");
    }
}

using e6502.Avalonia.Hardware;
using e6502.Avalonia.Rendering;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class AvaloniaSpriteRendererTests
{
    private VirtualGraphicsController _vgc = null!;

    [TestInitialize]
    public void Setup() => _vgc = new VirtualGraphicsController();

    [TestMethod]
    public void GetSpritePixels_DisabledSprite_ReturnsEmpty()
    {
        var pixels = SpriteRenderer.GetSpritePixels(_vgc, 0);
        Assert.AreEqual(0, pixels.Count);
    }

    [TestMethod]
    public void GetSpritePixels_MulticolorSprite_ReturnsCorrectColors()
    {
        // Enable sprite 0, position at (10, 5)
        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprEna);
        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegP1, 10);
        _vgc.Write(VgcConstants.RegP2, 0);
        _vgc.Write(VgcConstants.RegP3, 5);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprPos);

        // Define row 0: first two pixels are color 1 and 2 (byte 0x12), rest transparent
        _vgc.Write(VgcConstants.RegP0, 0);    // sprite
        _vgc.Write(VgcConstants.RegP1, 0);    // row
        _vgc.Write(VgcConstants.RegP2, 0x12); // pixels 0,1 = colors 1,2
        _vgc.Write(VgcConstants.RegP3, 0);
        _vgc.Write(VgcConstants.RegP4, 0);
        _vgc.Write(VgcConstants.RegP5, 0);
        _vgc.Write(VgcConstants.RegP6, 0);
        _vgc.Write(VgcConstants.RegP7, 0);
        _vgc.Write(VgcConstants.RegP8, 0);
        _vgc.Write(VgcConstants.RegP9, 0);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprRow);

        var pixels = SpriteRenderer.GetSpritePixels(_vgc, 0);

        Assert.AreEqual(2, pixels.Count);
        Assert.AreEqual(1, pixels[0].Color);
        Assert.AreEqual(2, pixels[1].Color);
        Assert.AreEqual(10, pixels[0].ScreenX);
        Assert.AreEqual(5, pixels[0].ScreenY);
        Assert.AreEqual(11, pixels[1].ScreenX);
        Assert.AreEqual(5, pixels[1].ScreenY);
    }

    [TestMethod]
    public void GetSpritePixels_TransparentPixels_AreSkipped()
    {
        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprEna);
        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegP1, 0);
        _vgc.Write(VgcConstants.RegP2, 0);
        _vgc.Write(VgcConstants.RegP3, 0);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprPos);

        // Row with all transparent (0x00) bytes — no visible pixels
        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegP1, 0);
        for (int i = 2; i <= 9; i++)
            _vgc.Write((ushort)(VgcConstants.RegP0 + i), 0);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprRow);

        var pixels = SpriteRenderer.GetSpritePixels(_vgc, 0);
        Assert.AreEqual(0, pixels.Count);
    }

    [TestMethod]
    public void GetSpritePixels_XFlip_ReversesColumns()
    {
        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprEna);
        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegP1, 0);
        _vgc.Write(VgcConstants.RegP2, 0);
        _vgc.Write(VgcConstants.RegP3, 0);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprPos);

        // Set X flip (bit 0)
        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegP1, 0x01);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprFlip);

        // Row 0: only pixel 0 has color 5 (byte 0x50 = high nibble 5, low nibble 0)
        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegP1, 0);
        _vgc.Write(VgcConstants.RegP2, 0x50);
        for (int i = 3; i <= 9; i++)
            _vgc.Write((ushort)(VgcConstants.RegP0 + i), 0);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprRow);

        var pixels = SpriteRenderer.GetSpritePixels(_vgc, 0);

        // With X flip, pixel 0 in shape maps to screen column 15
        Assert.AreEqual(1, pixels.Count);
        Assert.AreEqual(15, pixels[0].ScreenX);  // flipped: 0 → 15
        Assert.AreEqual(5, pixels[0].Color);
    }

    [TestMethod]
    public void GetSpritePixels_YFlip_ReversesRows()
    {
        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprEna);
        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegP1, 0);
        _vgc.Write(VgcConstants.RegP2, 0);
        _vgc.Write(VgcConstants.RegP3, 0);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprPos);

        // Set Y flip (bit 1)
        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegP1, 0x02);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprFlip);

        // Row 15 (last row): pixel 0 has color 7
        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegP1, 15);
        _vgc.Write(VgcConstants.RegP2, 0x70);
        for (int i = 3; i <= 9; i++)
            _vgc.Write((ushort)(VgcConstants.RegP0 + i), 0);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprRow);

        var pixels = SpriteRenderer.GetSpritePixels(_vgc, 0);

        // With Y flip, row 15 maps to screen row 0
        Assert.AreEqual(1, pixels.Count);
        Assert.AreEqual(0, pixels[0].ScreenY);
        Assert.AreEqual(7, pixels[0].Color);
    }

    [TestMethod]
    public void GetSpritePixels_BothFlips_ReversesRowsAndColumns()
    {
        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprEna);
        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegP1, 0);
        _vgc.Write(VgcConstants.RegP2, 0);
        _vgc.Write(VgcConstants.RegP3, 0);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprPos);

        // Set both flips (bits 0 and 1)
        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegP1, 0x03);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprFlip);

        // Row 15, pixel 0 has color 9
        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegP1, 15);
        _vgc.Write(VgcConstants.RegP2, 0x90);
        for (int i = 3; i <= 9; i++)
            _vgc.Write((ushort)(VgcConstants.RegP0 + i), 0);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprRow);

        var pixels = SpriteRenderer.GetSpritePixels(_vgc, 0);

        // X flip: col 0 → screen col 15, Y flip: row 15 → screen row 0
        Assert.AreEqual(1, pixels.Count);
        Assert.AreEqual(15, pixels[0].ScreenX);
        Assert.AreEqual(0, pixels[0].ScreenY);
        Assert.AreEqual(9, pixels[0].Color);
    }

    [TestMethod]
    public void DetectCollisions_SpriteSpriteOverlap_SetsBits()
    {
        // Enable two sprites at same position
        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprEna);
        _vgc.Write(VgcConstants.RegP0, 1);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprEna);

        // Position both at (0,0)
        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegP1, 0);
        _vgc.Write(VgcConstants.RegP2, 0);
        _vgc.Write(VgcConstants.RegP3, 0);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprPos);
        _vgc.Write(VgcConstants.RegP0, 1);
        _vgc.Write(VgcConstants.RegP1, 0);
        _vgc.Write(VgcConstants.RegP2, 0);
        _vgc.Write(VgcConstants.RegP3, 0);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprPos);

        // Give both sprites a non-transparent pixel at (0,0)
        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegP1, 0);
        _vgc.Write(VgcConstants.RegP2, 0x50);
        for (int i = 3; i <= 9; i++) _vgc.Write((ushort)(VgcConstants.RegP0 + i), 0);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprRow);

        _vgc.Write(VgcConstants.RegP0, 1);
        _vgc.Write(VgcConstants.RegP1, 0);
        _vgc.Write(VgcConstants.RegP2, 0x30);
        for (int i = 3; i <= 9; i++) _vgc.Write((ushort)(VgcConstants.RegP0 + i), 0);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprRow);

        var (ss, sb) = SpriteRenderer.DetectCollisions(_vgc);

        Assert.IsTrue((ss & 0x01) != 0, "Sprite 0 should be in collision");
        Assert.IsTrue((ss & 0x02) != 0, "Sprite 1 should be in collision");
    }

    [TestMethod]
    public void DetectCollisions_NoOverlap_NoBits()
    {
        // Enable two sprites at distant positions
        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprEna);
        _vgc.Write(VgcConstants.RegP0, 1);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprEna);

        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegP1, 0);
        _vgc.Write(VgcConstants.RegP2, 0);
        _vgc.Write(VgcConstants.RegP3, 0);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprPos);
        _vgc.Write(VgcConstants.RegP0, 1);
        _vgc.Write(VgcConstants.RegP1, 100);
        _vgc.Write(VgcConstants.RegP2, 0);
        _vgc.Write(VgcConstants.RegP3, 40);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprPos);

        // Give both a pixel at row 0, col 0
        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegP1, 0);
        _vgc.Write(VgcConstants.RegP2, 0x50);
        for (int i = 3; i <= 9; i++) _vgc.Write((ushort)(VgcConstants.RegP0 + i), 0);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprRow);

        _vgc.Write(VgcConstants.RegP0, 1);
        _vgc.Write(VgcConstants.RegP1, 0);
        _vgc.Write(VgcConstants.RegP2, 0x30);
        for (int i = 3; i <= 9; i++) _vgc.Write((ushort)(VgcConstants.RegP0 + i), 0);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprRow);

        var (ss, _) = SpriteRenderer.DetectCollisions(_vgc);

        Assert.AreEqual(0, ss, "No collision expected for distant sprites");
    }

    [TestMethod]
    public void DetectCollisions_SpriteBgOverlap_SetsBits()
    {
        // Enable sprite 0 at (5,5)
        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprEna);
        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegP1, 5);
        _vgc.Write(VgcConstants.RegP2, 0);
        _vgc.Write(VgcConstants.RegP3, 5);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprPos);

        // Give sprite a pixel at row 0, col 0 (screen position 5,5)
        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegP1, 0);
        _vgc.Write(VgcConstants.RegP2, 0x20);
        for (int i = 3; i <= 9; i++) _vgc.Write((ushort)(VgcConstants.RegP0 + i), 0);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprRow);

        // Plot a gfx pixel at (5,5)
        _vgc.Write(VgcConstants.RegP0, 3); // color
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdGcolor);
        _vgc.Write(VgcConstants.RegP0, 5);
        _vgc.Write(VgcConstants.RegP1, 0);
        _vgc.Write(VgcConstants.RegP2, 5);
        _vgc.Write(VgcConstants.RegP3, 0);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdPlot);

        var (ss, sb) = SpriteRenderer.DetectCollisions(_vgc);

        Assert.IsTrue((sb & 0x01) != 0, "Sprite 0 should collide with background");
    }

    [TestMethod]
    public void DetectCollisions_NoBgOverlap_NoBits()
    {
        // Enable sprite 0 at (0,0)
        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprEna);
        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegP1, 0);
        _vgc.Write(VgcConstants.RegP2, 0);
        _vgc.Write(VgcConstants.RegP3, 0);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprPos);

        // Give sprite a pixel at (0,0)
        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegP1, 0);
        _vgc.Write(VgcConstants.RegP2, 0x20);
        for (int i = 3; i <= 9; i++) _vgc.Write((ushort)(VgcConstants.RegP0 + i), 0);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprRow);

        // Plot a gfx pixel at (50,40) — far from sprite
        _vgc.Write(VgcConstants.RegP0, 3);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdGcolor);
        _vgc.Write(VgcConstants.RegP0, 50);
        _vgc.Write(VgcConstants.RegP1, 0);
        _vgc.Write(VgcConstants.RegP2, 40);
        _vgc.Write(VgcConstants.RegP3, 0);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdPlot);

        var (_, sb) = SpriteRenderer.DetectCollisions(_vgc);

        Assert.AreEqual(0, sb, "No background collision expected");
    }

    [TestMethod]
    public void GetSpritePixels_FullRow_Returns16Pixels()
    {
        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprEna);
        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegP1, 0);
        _vgc.Write(VgcConstants.RegP2, 0);
        _vgc.Write(VgcConstants.RegP3, 0);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprPos);

        // Fill row 0 with all non-transparent pixels (all nibbles = 0xF)
        _vgc.Write(VgcConstants.RegP0, 0);
        _vgc.Write(VgcConstants.RegP1, 0);
        _vgc.Write(VgcConstants.RegP2, 0xFF);
        _vgc.Write(VgcConstants.RegP3, 0xFF);
        _vgc.Write(VgcConstants.RegP4, 0xFF);
        _vgc.Write(VgcConstants.RegP5, 0xFF);
        _vgc.Write(VgcConstants.RegP6, 0xFF);
        _vgc.Write(VgcConstants.RegP7, 0xFF);
        _vgc.Write(VgcConstants.RegP8, 0xFF);
        _vgc.Write(VgcConstants.RegP9, 0xFF);
        _vgc.Write(VgcConstants.RegCmd, VgcConstants.CmdSprRow);

        var pixels = SpriteRenderer.GetSpritePixels(_vgc, 0);

        Assert.AreEqual(16, pixels.Count);
        for (int i = 0; i < 16; i++)
        {
            Assert.AreEqual(i, pixels[i].ScreenX);
            Assert.AreEqual(0, pixels[i].ScreenY);
            Assert.AreEqual(15, pixels[i].Color);
        }
    }
}

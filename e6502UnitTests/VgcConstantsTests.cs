using Microsoft.VisualStudio.TestTools.UnitTesting;
using e6502.TUI.Hardware;

namespace e6502UnitTests;

[TestClass]
public class VgcConstantsTests
{
    // -------------------------------------------------------------------------
    // Screen size
    // -------------------------------------------------------------------------

    [TestMethod]
    public void ScreenSize_Equals_ColsTimesRows()
    {
        Assert.AreEqual(VgcConstants.ScreenCols * VgcConstants.ScreenRows, VgcConstants.ScreenSize);
    }

    [TestMethod]
    public void ScreenSize_Is2000()
    {
        Assert.AreEqual(2000, VgcConstants.ScreenSize);
    }

    // -------------------------------------------------------------------------
    // Character RAM fits exactly ScreenSize bytes
    // -------------------------------------------------------------------------

    [TestMethod]
    public void CharRam_SpansExactlyScreenSize()
    {
        int length = VgcConstants.CharRamEnd - VgcConstants.CharRamBase + 1;
        Assert.AreEqual(VgcConstants.ScreenSize, length);
    }

    // -------------------------------------------------------------------------
    // Color RAM starts immediately after Character RAM (no gap, no overlap)
    // -------------------------------------------------------------------------

    [TestMethod]
    public void ColorRam_StartsImmediatelyAfterCharRam()
    {
        Assert.AreEqual(VgcConstants.CharRamEnd + 1, VgcConstants.ColorRamBase);
    }

    [TestMethod]
    public void ColorRam_SpansExactlyScreenSize()
    {
        int length = VgcConstants.ColorRamEnd - VgcConstants.ColorRamBase + 1;
        Assert.AreEqual(VgcConstants.ScreenSize, length);
    }

    // -------------------------------------------------------------------------
    // Color RAM does not overflow into free area
    // -------------------------------------------------------------------------

    [TestMethod]
    public void ColorRam_DoesNotOverflowIntoFreeArea()
    {
        Assert.IsTrue(VgcConstants.ColorRamEnd < VgcConstants.FreeBase);
    }

    [TestMethod]
    public void FreeArea_StartsImmediatelyAfterColorRam()
    {
        Assert.AreEqual(VgcConstants.ColorRamEnd + 1, VgcConstants.FreeBase);
    }

    // -------------------------------------------------------------------------
    // Sprite registers do not overlap graphics command registers
    // -------------------------------------------------------------------------

    [TestMethod]
    public void SpriteRegs_DoNotOverlapGfxCommandRegs()
    {
        Assert.IsTrue(VgcConstants.SpriteRegsEnd < VgcConstants.RegGfxCmd);
    }

    // -------------------------------------------------------------------------
    // Sprite shape data fits all 16 sprites
    // -------------------------------------------------------------------------

    [TestMethod]
    public void SpriteShapeArea_FitsAllSprites()
    {
        int required = VgcConstants.MaxSprites * VgcConstants.SpriteShapeSize;
        int available = VgcConstants.SpriteShapeEnd - VgcConstants.SpriteShapeBase + 1;
        Assert.IsTrue(available >= required,
            $"Sprite shape area ({available} bytes) is too small for {VgcConstants.MaxSprites} sprites × {VgcConstants.SpriteShapeSize} bytes = {required} bytes");
    }

    [TestMethod]
    public void SpriteShapeSize_Is32Bytes()
    {
        Assert.AreEqual(32, VgcConstants.SpriteShapeSize);
    }

    // -------------------------------------------------------------------------
    // SpriteReg helper addresses
    // -------------------------------------------------------------------------

    [TestMethod]
    public void SpriteReg_Sprite0_Offset0_IsBase()
    {
        Assert.AreEqual(VgcConstants.SpriteRegsBase, VgcConstants.SpriteReg(0, 0));
    }

    [TestMethod]
    public void SpriteReg_Sprite15_LastByte_IsWithinSpriteRegsEnd()
    {
        int last = VgcConstants.SpriteReg(VgcConstants.MaxSprites - 1, VgcConstants.SpriteBytesEach - 1);
        Assert.IsTrue(last <= VgcConstants.SpriteRegsEnd,
            $"Last sprite register address 0x{last:X4} exceeds SpriteRegsEnd 0x{VgcConstants.SpriteRegsEnd:X4}");
    }

    // -------------------------------------------------------------------------
    // VGC register block is within VGC address range
    // -------------------------------------------------------------------------

    [TestMethod]
    public void AllCoreRegisters_AreWithinVgcBlock()
    {
        int[] coreRegs =
        [
            VgcConstants.RegMode, VgcConstants.RegBgCol, VgcConstants.RegFgCol,
            VgcConstants.RegCursorX, VgcConstants.RegCursorY,
            VgcConstants.RegScrollX, VgcConstants.RegScrollY,
            VgcConstants.RegBank, VgcConstants.RegStatus,
            VgcConstants.RegSpriteEn, VgcConstants.RegSpriteEnH,
            VgcConstants.RegColSt, VgcConstants.RegColBg,
            VgcConstants.RegBorder, VgcConstants.RegCharOut, VgcConstants.RegCharIn,
        ];

        foreach (int reg in coreRegs)
        {
            Assert.IsTrue(reg >= VgcConstants.VgcBase && reg <= VgcConstants.VgcEnd,
                $"Register 0x{reg:X4} is outside VGC block [0x{VgcConstants.VgcBase:X4}-0x{VgcConstants.VgcEnd:X4}]");
        }
    }

    [TestMethod]
    public void AllGfxCommandRegisters_AreWithinVgcBlock()
    {
        int[] gfxRegs =
        [
            VgcConstants.RegGfxCmd, VgcConstants.RegGfxP0, VgcConstants.RegGfxP1,
            VgcConstants.RegGfxP2, VgcConstants.RegGfxP3, VgcConstants.RegGfxP4,
            VgcConstants.RegGfxP5, VgcConstants.RegGfxP6, VgcConstants.RegGfxP7,
        ];

        foreach (int reg in gfxRegs)
        {
            Assert.IsTrue(reg >= VgcConstants.VgcBase && reg <= VgcConstants.VgcEnd,
                $"GFX register 0x{reg:X4} is outside VGC block [0x{VgcConstants.VgcBase:X4}-0x{VgcConstants.VgcEnd:X4}]");
        }
    }

    // -------------------------------------------------------------------------
    // Palette and sprite counts
    // -------------------------------------------------------------------------

    [TestMethod]
    public void PaletteSize_Is16()
    {
        Assert.AreEqual(16, VgcConstants.PaletteSize);
    }

    [TestMethod]
    public void MaxSprites_Is16()
    {
        Assert.AreEqual(16, VgcConstants.MaxSprites);
    }

    // -------------------------------------------------------------------------
    // ROM starts at $C000
    // -------------------------------------------------------------------------

    [TestMethod]
    public void RomBase_Is_C000()
    {
        Assert.AreEqual(0xC000, VgcConstants.RomBase);
    }
}

using System;
using System.IO;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class VgcMouseCursorSourceTests
{
    private static string RepoPath(params string[] parts)
    {
        string root = Path.GetFullPath(Path.Combine(
            AppContext.BaseDirectory, "..", "..", "..", ".."));
        return Path.Combine([root, .. parts]);
    }

    [TestMethod]
    public void Rtl_DefinesVblankCommittedMouseCursorBlock()
    {
        string vgc = File.ReadAllText(RepoPath("e6502.FPGA", "rtl", "vgc.sv"));
        string top = File.ReadAllText(RepoPath("e6502.FPGA", "rtl", "top.sv"));

        StringAssert.Contains(vgc, "localparam MOUSE_REG_BASE = 16'hA0D0");
        StringAssert.Contains(vgc, "mouse_pending_x");
        StringAssert.Contains(vgc, "mouse_active_x");
        StringAssert.Contains(vgc, "mouse_active_x <= mouse_pending_x");
        StringAssert.Contains(vgc, "mouse_cursor_hit");
        StringAssert.Contains(vgc, "pixel_color_idx = mouse_cursor_color");
        StringAssert.Contains(top, "16'hA0D0");
    }

    [TestMethod]
    public void Rtl_SpriteShapeRamSupports256Slots()
    {
        string sprites = File.ReadAllText(RepoPath("e6502.FPGA", "rtl", "vgc_sprites.sv"));
        string vgc = File.ReadAllText(RepoPath("e6502.FPGA", "rtl", "vgc.sv"));

        StringAssert.Contains(sprites, "localparam SHAPE_RAM_SIZE = 32768");
        StringAssert.Contains(sprites, "input  logic [16*8-1:0]  spr_shape_flat");
        StringAssert.Contains(sprites, ".DEPTH(SHAPE_RAM_SIZE)");
        StringAssert.Contains(vgc, "logic [7:0]  spr_shape [0:15]");
        StringAssert.Contains(vgc, "spr_shape_flat[i*8 +: 8] = spr_shape[i]");
        StringAssert.Contains(vgc, "SPACE_SPRITE && vram_port_read_addr < SPR_SIZE");
        Assert.IsFalse(sprites.Contains("shape_copy_dirty"),
            "Do not track 32K sprite-shape dirty bits in LUT fabric; sync-time writes must mirror both banks instead.");
    }

    [TestMethod]
    public void Ndk_ExposesMousePointerHelpers()
    {
        string nova = File.ReadAllText(RepoPath("software", "runtime", "asm", "nova.inc"));
        string include = File.ReadAllText(RepoPath("software", "runtime", "asm", "mouse.inc"));
        string impl = File.ReadAllText(RepoPath("software", "runtime", "asm", "mouse.s"));
        string runtimeMake = File.ReadAllText(RepoPath("software", "runtime", "asm", "Makefile"));
        string basicMake = File.ReadAllText(RepoPath("software", "languages", "ehbasic", "Makefile"));

        StringAssert.Contains(nova, "VGC_MOUSE_XL      = $A0D0");
        StringAssert.Contains(nova, "VGC_MOUSE_CTRL_ENABLE = $01");
        StringAssert.Contains(include, ".global mouse_show");
        StringAssert.Contains(include, ".global mouse_set_shape");
        StringAssert.Contains(include, ".global mouse_get_buttons");
        StringAssert.Contains(impl, "STA   VGC_MOUSE_CTRL");
        StringAssert.Contains(impl, "STA   VGC_MOUSE_SHAPE");
        StringAssert.Contains(impl, "LDA   VGC_MOUSE_BUTTONS");
        StringAssert.Contains(runtimeMake, "mouse.s");
        StringAssert.Contains(basicMake, "$(NOVA_ASM)/mouse.inc $(NOVA_ASM)/mouse.s");
    }
}

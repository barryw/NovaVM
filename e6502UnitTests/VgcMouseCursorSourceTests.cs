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
    public void Rtl_Mode2UsesPerCellTransparentBackgroundAttr()
    {
        string vgc = File.ReadAllText(RepoPath("e6502.FPGA", "rtl", "vgc.sv"));
        string vgcSim = File.ReadAllText(RepoPath("e6502.FPGA", "boards", "arty_z7", "sim", "vgc_sim.sv"));
        string novaCli = File.ReadAllText(RepoPath("e6502.Nova", "Program.cs"));

        StringAssert.Contains(vgc, "TATTR_BGTRANS = 8'h08",
            "Hardware mode 2 uses a per-cell attr bit for transparent text backgrounds.");
        StringAssert.Contains(vgc, "text_bgtrans_d2 = attr_b_dout[3]",
            "The HDMI path must fetch the transparent-background bit from text-attr RAM.");
        StringAssert.Contains(vgc, "&& text_bgtrans_d2 && !text_reverse_d2",
            "Mode 2 should reveal graphics only for non-reverse cells with the transparent-background attr set.");
        StringAssert.Contains(vgc, "!text_reverse_d2",
            "Reverse text backgrounds stay opaque in mode 2.");
        Assert.IsFalse(vgc.Contains("cur_bg_d2 == bg_color", StringComparison.Ordinal),
            "Do not add a dynamic background-colour comparator in the HDMI pixel path; set TATTR_BGTRANS in software instead.");
        StringAssert.Contains(vgcSim, "TATTR_BGTRANS = 8'h08",
            "The simulator copy must use the same transparent-background attr bit.");
        StringAssert.Contains(vgcSim, "text_bgtrans_d2 = attr_b_dout[3]",
            "The simulator copy must fetch the same transparent-background bit.");
        Assert.IsFalse(vgcSim.Contains("cur_bg_d2 == bg_color", StringComparison.Ordinal),
            "The simulator copy must not preserve the old background-colour matching rule.");
        StringAssert.Contains(novaCli, "textAttr & 0x08",
            "CLI screenshots must composite mode 2 using the same transparent-background attr bit as HDMI.");
        Assert.IsFalse(novaCli.Contains("cellBg == bgColor", StringComparison.Ordinal),
            "CLI screenshots must not preserve the old background-colour matching rule.");
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

using System;
using System.IO;
using System.Text.RegularExpressions;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public sealed class NovaZRuntimeLayoutTests
{
    [TestMethod]
    public void RuntimeBss_ReservesLibCallAndModuleBssBands()
    {
        string mapPath = RepoPath("examples", "novaz", "build", "runtime.map");
        if (!File.Exists(mapPath))
            Assert.Inconclusive("examples/novaz/build/runtime.map has not been built");

        string map = File.ReadAllText(mapPath);
        var match = Regex.Match(map, @"^BSS\s+([0-9A-F]{6})\s+([0-9A-F]{6})", RegexOptions.Multiline);
        Assert.IsTrue(match.Success, "runtime.map must contain the BSS segment row");

        int start = Convert.ToInt32(match.Groups[1].Value, 16);
        int end = Convert.ToInt32(match.Groups[2].Value, 16);

        Assert.AreEqual(0x0900, start,
            "NovaZ must reserve $0300-$041F for lib_call and $0420-$08FF for paged module BSS");
        Assert.IsTrue(end < 0x2000,
            $"NovaZ BSS must stay below the $2000 NOVAZ6 segment, got end ${end:X4}");
    }

    [TestMethod]
    public void Autoboot_ArmsSharedLibraryAbiForPrimaryRuntime()
    {
        string autobootPath = RepoPath("examples", "novaz", "build", "AUTOBOOT.bin");
        if (!File.Exists(autobootPath))
            Assert.Inconclusive("examples/novaz/build/AUTOBOOT.bin has not been built");

        byte[] bin = File.ReadAllBytes(autobootPath);

        Assert.IsTrue(Contains(bin,
                0xA9, 0x02,       // LDA #ROMSWAP_PRIMARY
                0x8D, 0x3F, 0xA0, // STA REG_ROMSWAP
                0x8D, 0x17, 0x03, // STA LIB_HOME_BANK
                0x9C, 0x18, 0x03),// STZ LIB_RESIDENT
            "NovaZ autoboot must arm lib_call for the primary runtime before jumping to the reset vector");
    }

    [TestMethod]
    public void SaveLoadOverlay_LoadsAboveV6MemoryAndHasMainEntry()
    {
        string overlayPath = RepoPath("examples", "novaz", "build", "SAVLOAD.OVL");
        if (!File.Exists(overlayPath))
            Assert.Inconclusive("examples/novaz/build/SAVLOAD.OVL has not been built");

        byte[] ovl = File.ReadAllBytes(overlayPath);

        Assert.AreEqual((byte)'N', ovl[0]);
        Assert.AreEqual((byte)'O', ovl[1]);
        Assert.AreEqual((byte)'V', ovl[2]);
        Assert.AreEqual((byte)'O', ovl[3]);
        Assert.AreEqual(0x4800, ReadU16(ovl, 0x08),
            "SAVLOAD.OVL must load above NOVAZ6 image/BSS and below high runtime RAM");
        Assert.AreEqual(0x4800, ReadU16(ovl, 0x10),
            "SAVLOAD.OVL main entry must be the exported overlay entry");
        Assert.IsTrue(0x20 + ReadU16(ovl, 0x0A) + ReadU16(ovl, 0x0C) <= 0x2800,
            "SAVLOAD.OVL payload+BSS must fit the declared $4800-$6FFF slot");
    }

    [TestMethod]
    public void Runtime_RequestsXramThroughMemoryService()
    {
        string zstory = File.ReadAllText(RepoPath("examples", "novaz", "src", "zstory.s"));
        string zstoryInc = File.ReadAllText(RepoPath("examples", "novaz", "src", "zstory.inc"));
        string saveLoad = File.ReadAllText(RepoPath("examples", "novaz", "src", "save_load_overlay.s"));
        string zvm6 = File.ReadAllText(RepoPath("examples", "novaz", "src", "zvm6.s"));

        StringAssert.Contains(zstory, "MEM_RESET_USAGE");
        StringAssert.Contains(zstory, "MEM_ALLOC");
        StringAssert.Contains(zstory, "JSR LIB_LOADER_BAND");
        StringAssert.Contains(zstory, "ZSTORY_AUX_ALLOC_HI     = $FF");
        StringAssert.Contains(zstory, "zstory_dynamic_base_l");
        StringAssert.Contains(zstory, "zstory_cache_base_l");
        StringAssert.Contains(zstoryInc, "ZSTORY_AUX_SAVE_CHAR_OFF_M");
        StringAssert.Contains(zstoryInc, "ZSTORY_AUX_SAVE_COLOR_OFF_M");
        StringAssert.Contains(zstoryInc, "ZSTORY_AUX_SAVE_ATTR_OFF_M");
        StringAssert.Contains(zstoryInc, "ZSTORY_AUX_SAVE_GFX_OFF_M");
        StringAssert.Contains(zstoryInc, "ZSTORY_AUX_PICS_INDEX_OFF_M   = $64");
        StringAssert.Contains(zstoryInc, "ZSTORY_AUX_PICS_BOUNCE_OFF_M  = $84");
        StringAssert.Contains(saveLoad, "zstory_dynamic_base_l");
        StringAssert.Contains(saveLoad, "zstory_cache_base_l");
        StringAssert.Contains(saveLoad, "ZSTORY_AUX_SAVE_CHAR_OFF_M");
        StringAssert.Contains(saveLoad, "ZSTORY_AUX_SAVE_COLOR_OFF_M");
        StringAssert.Contains(saveLoad, "ZSTORY_AUX_SAVE_ATTR_OFF_M");
        StringAssert.Contains(saveLoad, "ZSTORY_AUX_SAVE_GFX_OFF_M");
        StringAssert.Contains(saveLoad, "SYS_NUI_SAVE_UNDER_FULL");
        StringAssert.Contains(saveLoad, "SYS_NUI_RESTORE_UNDER_FULL");
        StringAssert.Contains(saveLoad, "save_load_title_save");
        StringAssert.Contains(saveLoad, "save_load_title_restore");
        StringAssert.Contains(saveLoad, "save_load_copy_header_desc_to_row");
        StringAssert.Contains(saveLoad, "save_load_empty_desc");
        Assert.IsFalse(saveLoad.Contains("SAVE / RESTORE", StringComparison.Ordinal),
            "Save/load UI must use operation-specific titles.");
        Assert.IsFalse(saveLoad.Contains("SAVE00.NZS       ", StringComparison.Ordinal),
            "Save/load picker rows must be generated from save descriptions, not filenames.");
        StringAssert.Contains(zvm6, "zstory_cache_base_l");
        StringAssert.Contains(zvm6, "nz6_pics_base_l");
        StringAssert.Contains(zvm6, "ZSTORY_AUX_PICS_INDEX_OFF_M");
        StringAssert.Contains(zvm6, "ZSTORY_AUX_PICS_BOUNCE_OFF_M - ZSTORY_AUX_PICS_INDEX_OFF_M");

        Assert.IsFalse(zvm6.Contains("NZ6_PICS_ALLOC", StringComparison.Ordinal),
            "V6 graphics must use the allocator-owned story aux arena, not a private fixed allocation");
        Assert.IsFalse(zvm6.Contains("MEM_ALLOC", StringComparison.Ordinal),
            "V6 graphics must not call MEMORY directly from the paged V6 segment");
        Assert.IsFalse(zvm6.Contains("LIB_LOADER_BAND", StringComparison.Ordinal),
            "V6 graphics must not use the resident loader directly from the paged V6 segment");

        AssertNoNovaZFixedXram(zstory);
        AssertNoNovaZFixedXram(zstoryInc);
        AssertNoNovaZFixedXram(saveLoad);
        AssertNoNovaZFixedXram(zvm6);
    }

    [TestMethod]
    public void V6FlowPicturesClearReservedGfxCellsBeforeTransparentBlit()
    {
        string zvm6 = File.ReadAllText(RepoPath("examples", "novaz", "src", "zvm6.s"));
        string draw = Slice(zvm6, "nz6_pic_draw_current_abs:", "; erase_picture N [y x]");
        string ownership = Slice(draw, "@mark_picture_cells:", "; Compute the 4x4 text-cell rectangle");
        string prefill = Slice(zvm6, "nz6_pic_prefill_flow_cell_rect:", "; erase_picture N [y x]");
        string record = Slice(zvm6, "nz6_gfx_record_draw_current:", "nz6_gfx_record_erase_current:");

        // Flow icons reserve whole 4x4 text cells, while the transparent
        // bitmap may only cover part of the first/last cell. The gfx plane
        // must be cleared before the keyed blit or text-transparent padding
        // exposes stale pixels from older inline pictures.
        int prefillCall = IndexOfOrFail(draw, "JSR nz6_pic_prefill_flow_cell_rect", "flow prefill call");
        int unpackCall = IndexOfOrFail(draw, "JSR blitter_start_gfx4_unpack", "picture unpack");
        Assert.IsTrue(prefillCall < unpackCall, "Flow cell prefill must run before the transparent picture blit.");

        StringAssert.Contains(prefill, "AND #(NZ6_PIC_FLAG_FLOW_ICON | NZ6_PIC_FLAG_FLOW_CELLTOP)");
        StringAssert.Contains(prefill, "JSR nz6_pic_compute_cell_rect");
        StringAssert.Contains(prefill, "JMP nz6_fill_gfx_cell_rect");
        StringAssert.Contains(prefill, "JMP vtext_fill_gfx_region");

        // After the keyed blit, flow pictures own their whole reserved text
        // cell rectangle. They must clear the text cells to gfx-transparent;
        // treating them as ordinary overlays leaves old glyph/picture edges
        // behind when the transcript scrolls.
        int flowOwnerTest = IndexOfOrFail(ownership, "AND #(NZ6_PIC_FLAG_FLOW_ICON | NZ6_PIC_FLAG_FLOW_CELLTOP)", "flow owner test");
        int transparentOverlayTest = IndexOfOrFail(ownership, "AND #NZ6_PIC_FLAG_TRANSPARENT", "transparent overlay test");
        int clearCall = IndexOfOrFail(ownership, "JSR vtext_clear_region", "flow cell clear");
        int exposeSpacesCall = IndexOfOrFail(ownership, "JSR vtext_expose_gfx_spaces_region", "overlay space exposure");
        Assert.IsTrue(flowOwnerTest < transparentOverlayTest, "Flow ownership must be decided before transparent-overlay handling.");
        Assert.IsTrue(clearCall < exposeSpacesCall, "Flow pictures must clear owned cells instead of exposing only spaces.");

        StringAssert.Contains(record, "AND #(NZ6_PIC_FLAG_FLOW_ICON | NZ6_PIC_FLAG_FLOW_CELLTOP)");
        StringAssert.Contains(record, "LDA nz6_gfx_replay_fill");
        StringAssert.Contains(record, "STA nz6_gfx_list_fill,X");
    }

    [TestMethod]
    public void V6MarginReleaseRestylesOnlyReleasedBlankCells()
    {
        string zvm6 = File.ReadAllText(RepoPath("examples", "novaz", "src", "zvm6.s"));
        string setMargins = Slice(zvm6, "nz6_ext_set_margins:", "; Shared tail for move_window/window_size");
        string helper = Slice(zvm6, "nz6_restore_released_margin_spaces:", "; Shared tail for move_window/window_size");

        // Z6 flow pictures reserve temporary left/right margins. When a story
        // releases that margin, the newly writable blank cells become normal
        // background again; otherwise text-transparent picture cells scroll
        // forward and stale icon edges show as vertical stripes.
        StringAssert.Contains(setMargins, "STA nz6_rect_left               ; old writable left edge");
        StringAssert.Contains(setMargins, "STA nz6_rect_w                  ; old writable right edge");
        StringAssert.Contains(setMargins, "ADC VTEXT_CURY");
        StringAssert.Contains(setMargins, "ADC #1");
        StringAssert.Contains(setMargins, "JSR nz6_restore_released_margin_spaces");

        StringAssert.Contains(helper, "JSR nz6_build_region_tmp_win    ; new writable rect");
        StringAssert.Contains(helper, "Left margin shrank: [new_left, old_left).");
        StringAssert.Contains(helper, "Right margin shrank: [old_right, new_right).");
        StringAssert.Contains(helper, "LDA nz6_colour                  ; opaque current background");
        StringAssert.Contains(helper, "JSR vtext_fill_gfx_region");
        StringAssert.Contains(helper, "JSR vtext_expose_gfx_spaces_region");
        Assert.IsFalse(helper.Contains("JSR vtext_clear_region", StringComparison.Ordinal),
            "Margin release must restyle blank cells only; clearing would erase already printed text.");
    }

    [TestMethod]
    public void V6CompositeScrollUsesExactGfxWindowAndSnappedTextWindow()
    {
        string zvm6 = File.ReadAllText(RepoPath("examples", "novaz", "src", "zvm6.s"));
        string gfxBuilder = Slice(zvm6, "nz6_build_gfx_region_tmp_win:", "; Freshen the CURRENT window");
        string fullBuilder = Slice(zvm6, "nz6_build_full_region_tmp_win:", "; Pixel-precise graphics rectangle");
        string hook = Slice(zvm6, "nz6_scroll_live_composite:", "; ROM newline hook");
        string clearTop = Slice(zvm6, "nz6_clear_scroll_excluded_top_rows:", "nz6_scroll_live_rows_composite:");
        string rows = Slice(zvm6, "nz6_scroll_live_rows_composite:", "; ROM newline hook");

        // Z6 windows are pixel rectangles, but Nova's text plane can only own
        // whole 4x4 cells. The scroll hook must therefore scroll the exact gfx
        // pixel window and the snapped text-cell window separately; otherwise
        // transparent text padding exposes stale picture pixels at non-cell
        // aligned edges.
        StringAssert.Contains(gfxBuilder, "STA VTEXT_GFX_LEFTL");
        StringAssert.Contains(gfxBuilder, "STA VTEXT_GFX_LEFTH");
        StringAssert.Contains(gfxBuilder, "STA VTEXT_GFX_TOP");
        StringAssert.Contains(gfxBuilder, "STA VTEXT_GFX_WIDTHL");
        StringAssert.Contains(gfxBuilder, "STA VTEXT_GFX_WIDTHH");
        StringAssert.Contains(gfxBuilder, "STA VTEXT_GFX_HEIGHT");

        StringAssert.Contains(fullBuilder, "JSR nz6_prop_origin_zero");
        StringAssert.Contains(fullBuilder, "JSR nz6_unit_edge_start_cell");

        StringAssert.Contains(hook, "JSR nz6_build_full_region_tmp_win");
        StringAssert.Contains(hook, "JSR nz6_clear_scroll_excluded_top_rows");
        StringAssert.Contains(hook, "JSR nz6_scroll_live_rows_composite");

        StringAssert.Contains(clearTop, "JSR nz6_prop_top_cell");
        StringAssert.Contains(clearTop, "CMP VTEXT_TOP");
        StringAssert.Contains(clearTop, "LDA #NZ6_COLOR_DEFAULT");
        StringAssert.Contains(clearTop, "JSR vtext_clear_region");

        // The composite scroll now builds the exact gfx pixel window into the
        // VTEXT_GFX_* registers, then issues one vblank-aligned hardware
        // transaction (vtext_scroll_composite_up -> VCMD_SCROLLMIXED) that scrolls
        // the gfx, character, colour, and attribute planes together. This replaces
        // the old two-pass software scroll and removes the tear between planes.
        StringAssert.Contains(rows, "JSR nz6_build_gfx_region_tmp_win");
        StringAssert.Contains(rows, "JMP vtext_scroll_composite_up");
        Assert.IsFalse(rows.Contains("vtext_scroll_mixed_up", StringComparison.Ordinal),
            "Z6 composite scroll must not derive the gfx rectangle from the snapped VTEXT cell rectangle.");
    }

    private static bool Contains(byte[] haystack, params byte[] needle)
    {
        for (int i = 0; i + needle.Length <= haystack.Length; i++)
        {
            bool matches = true;
            for (int j = 0; j < needle.Length; j++)
            {
                if (haystack[i + j] == needle[j])
                    continue;
                matches = false;
                break;
            }

            if (matches)
                return true;
        }

        return false;
    }

    private static ushort ReadU16(byte[] data, int offset)
        => (ushort)(data[offset] | (data[offset + 1] << 8));

    private static void AssertNoNovaZFixedXram(string source)
    {
        Assert.IsFalse(source.Contains("XRAM_NOVAZ_", StringComparison.Ordinal));
        Assert.IsFalse(source.Contains("ZSTORY_XRAM_", StringComparison.Ordinal));
        Assert.IsFalse(source.Contains("$060000", StringComparison.Ordinal));
        Assert.IsFalse(source.Contains("$064000", StringComparison.Ordinal));
        Assert.IsFalse(source.Contains("$068000", StringComparison.Ordinal));
        Assert.IsFalse(source.Contains("$06C000", StringComparison.Ordinal));
    }

    private static string Slice(string source, string startMarker, string endMarker)
    {
        int start = source.IndexOf(startMarker, StringComparison.Ordinal);
        Assert.IsTrue(start >= 0, $"Missing start marker {startMarker}.");
        int end = source.IndexOf(endMarker, start, StringComparison.Ordinal);
        Assert.IsTrue(end > start, $"Missing end marker {endMarker} after {startMarker}.");
        return source[start..end];
    }

    private static int IndexOfOrFail(string source, string value, string name)
    {
        int index = source.IndexOf(value, StringComparison.Ordinal);
        Assert.IsTrue(index >= 0, $"{name} missing {value}.");
        return index;
    }

    private static string RepoPath(params string[] parts)
    {
        string root = Path.GetFullPath(Path.Combine(
            AppContext.BaseDirectory, "..", "..", "..", ".."));
        return Path.Combine([root, .. parts]);
    }
}

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
        string mapPath = RepoPath("software", "examples", "novaz", "build", "runtime.map");
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
        string autobootPath = RepoPath("software", "examples", "novaz", "build", "AUTOBOOT.bin");
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
        string overlayPath = RepoPath("software", "examples", "novaz", "build", "SAVLOAD.OVL");
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
        string zstory = File.ReadAllText(RepoPath("software", "examples", "novaz", "src", "zstory.s"));
        string zstoryInc = File.ReadAllText(RepoPath("software", "examples", "novaz", "src", "zstory.inc"));
        string saveLoad = File.ReadAllText(RepoPath("software", "examples", "novaz", "src", "save_load_overlay.s"));
        string zvm6 = File.ReadAllText(RepoPath("software", "examples", "novaz", "src", "zvm6.s"));

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
        string zvm6 = File.ReadAllText(RepoPath("software", "examples", "novaz", "src", "zvm6.s"));
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
        int transparentAttrSet = IndexOfOrFail(ownership, "LDA #VTXT_ATTR_BGTRANS", "transparent text attr setup");
        int clearCall = IndexOfOrFail(ownership, "JSR vtext_clear_region", "flow cell clear");
        int exposeSpacesCall = IndexOfOrFail(ownership, "JSR vtext_expose_gfx_spaces_region", "overlay space exposure");
        Assert.IsTrue(flowOwnerTest < transparentOverlayTest, "Flow ownership must be decided before transparent-overlay handling.");
        Assert.IsTrue(transparentAttrSet < clearCall,
            "Picture-owned cells must clear with transparent text attrs; otherwise the text plane hides the freshly drawn picture.");
        Assert.IsTrue(transparentAttrSet < exposeSpacesCall,
            "Transparent picture overlays must expose spaces with transparent text attrs, not the caller's current style.");
        Assert.IsTrue(clearCall < exposeSpacesCall, "Flow pictures must clear owned cells instead of exposing only spaces.");

        StringAssert.Contains(record, "AND #(NZ6_PIC_FLAG_FLOW_ICON | NZ6_PIC_FLAG_FLOW_CELLTOP)");
        StringAssert.Contains(record, "LDA nz6_gfx_replay_fill");
        StringAssert.Contains(record, "STA nz6_gfx_list_fill,X");
    }

    [TestMethod]
    public void V6NormalTextBackgroundsAreOpaqueUnlessPictureOwned()
    {
        string zvm6 = File.ReadAllText(RepoPath("software", "examples", "novaz", "src", "zvm6.s"));
        string applyStyle = Slice(zvm6, "nz6_apply_colour_style:", "nz6_attr_for_vtext_bg:");
        string attrForBg = Slice(zvm6, "nz6_attr_for_vtext_bg:", "; Keep the physical display border");

        Assert.IsFalse(applyStyle.Contains("ORA #VTXT_ATTR_BGTRANS", StringComparison.Ordinal),
            "Normal V6 text must cover stale graphics with its background; picture-owned cells opt into transparency separately.");
        StringAssert.Contains(applyStyle, "AND #VTXT_ATTR_FLASH",
            "Applying normal V6 text style must clear stale reverse/bold/background-transparency attrs while preserving flash.");
        StringAssert.Contains(attrForBg, "LDA #0",
            "Normal region clears must use opaque text attrs so erased windows cover stale picture pixels.");
        Assert.IsFalse(attrForBg.Contains("VTXT_ATTR_BGTRANS", StringComparison.Ordinal),
            "Only picture-owned cells should use transparent text attrs.");
    }

    [TestMethod]
    public void V6WholeScreenClearPreservesCurrentBackgroundNibble()
    {
        string zvm = File.ReadAllText(RepoPath("software", "examples", "novaz", "src", "zvm.s"));
        string runtime = File.ReadAllText(RepoPath("software", "examples", "novaz", "src", "runtime.s"));
        string clear = Slice(zvm, "zvm_clear_whole_screen:", "zvm_clear_selected_window:");
        string initScreen = Slice(runtime, "init_screen:", "init_video_colors:");
        string initGame = Slice(runtime, "init_game_screen:", "; A/Y = pointer to null-terminated string.");

        StringAssert.Contains(clear, "CMP #$06");
        StringAssert.Contains(clear, "JSR zvm_reset_text_ring");
        StringAssert.Contains(clear, "STA VGC_CHAROUT",
            "Whole-screen clears must issue VGC form-feed before VTEXT clear so hardware ring-scroll state cannot survive into mixed V6 scrolling.");
        Assert.IsTrue(
            IndexOfOrFail(clear, "JSR zvm_reset_text_ring", "hardware text-ring reset") <
            IndexOfOrFail(clear, "JSR vtext_clear_region", "VTEXT full clear"),
            "The hardware text-ring reset must run before VTEXT repaints the full-screen logical region.");
        Assert.IsFalse(clear.Contains("AND #$0F", StringComparison.Ordinal),
            "V6 whole-screen clears must keep the packed current background colour; forcing bg 0 exposes the old Zork Zero title art colour.");
        StringAssert.Contains(clear, "STZ VTEXT_ATTR",
            "Whole-screen clears stay opaque; picture-owned cells opt into BGTRANS separately.");

        StringAssert.Contains(clear, "STZ VGC_TEXT_TOPROW",
            "The hardware display top-row register must be reset with the hidden FF scroll offset.");
        StringAssert.Contains(clear, "STZ VTEXT_TOPROW",
            "VTEXT's software row mapper must be reset before NovaZ takes over the screen.");
        StringAssert.Contains(initScreen, "JSR zvm_reset_text_ring",
            "NovaZ boot clear must reset inherited hardware text-ring state before showing loader/game text.");
        StringAssert.Contains(initGame, "JSR zvm_reset_text_ring",
            "NovaZ game-entry clear must reset inherited hardware text-ring state before the story starts drawing V6 windows.");
    }

    [TestMethod]
    public void V6MarginChangesMaintainThePixelSurfaceContract()
    {
        string zvm6 = File.ReadAllText(RepoPath("software", "examples", "novaz", "src", "zvm6.s"));
        string setMargins = Slice(zvm6, "nz6_ext_set_margins:", "; Shared tail for move_window/window_size");
        string reconcile = Slice(zvm6, "nz6_reconcile_margin_surface:", "; Shared tail for move_window/window_size");
        string exclude = Slice(zvm6, "nz6_exclude_margin_rect:", "nz6_restore_space_rect:");
        string restore = Slice(zvm6, "nz6_restore_space_rect:", "; Shared tail for move_window/window_size");

        // Z6 flow pictures reserve temporary left/right margins on a single
        // pixel surface. Nova has split text/gfx planes, so margin growth must
        // turn newly hidden text cells into transparent spaces before later
        // full-window scrolls can copy stale glyph bytes into view. Margin
        // release then restores only the newly writable blank cells to normal
        // background below the current output row.
        StringAssert.Contains(setMargins, "STA nz6_rect_left               ; old writable left edge");
        StringAssert.Contains(setMargins, "STA nz6_rect_w                  ; old writable right edge");
        StringAssert.Contains(setMargins, "ADC VTEXT_CURY");
        StringAssert.Contains(setMargins, "STA nz6_margin_row              ; growth starts on current row");
        StringAssert.Contains(setMargins, "JSR nz6_reconcile_margin_surface");

        StringAssert.Contains(reconcile, "JSR nz6_build_region_tmp_win    ; new writable rect");
        StringAssert.Contains(reconcile, "Left margin grew: [old_left, new_left).");
        StringAssert.Contains(reconcile, "Right margin grew: [new_right, old_right).");
        StringAssert.Contains(reconcile, "JSR nz6_exclude_margin_rect");
        StringAssert.Contains(reconcile, "Margin release starts below the current output row.");
        StringAssert.Contains(reconcile, "Left margin shrank: [new_left, old_left).");
        StringAssert.Contains(reconcile, "Right margin shrank: [old_right, new_right).");
        StringAssert.Contains(reconcile, "JSR nz6_restore_space_rect");

        StringAssert.Contains(exclude, "LDA #VTXT_ATTR_BGTRANS");
        StringAssert.Contains(exclude, "JSR vtext_clear_region");
        AssertVtextBorrowedRectangleIsRestored(exclude, "JSR vtext_clear_region");
        Assert.IsFalse(exclude.Contains("JSR vtext_fill_gfx_region", StringComparison.Ordinal),
            "Growing a margin must not erase the gfx pixels that the text plane should reveal.");

        StringAssert.Contains(restore, "LDA nz6_colour                  ; current paper; text owns background");
        StringAssert.Contains(restore, "JSR vtext_fill_gfx_region");
        StringAssert.Contains(restore, "JSR vtext_expose_gfx_spaces_region");
        AssertVtextBorrowedRectangleIsRestored(restore, "JSR vtext_expose_gfx_spaces_region");
        Assert.IsFalse(restore.Contains("JSR vtext_clear_region", StringComparison.Ordinal),
            "Margin release must restyle blank cells only; clearing would erase already printed text.");
    }

    [TestMethod]
    public void V6CompositeScrollUsesExactGfxWindowAndSnappedTextWindow()
    {
        string zvm6 = File.ReadAllText(RepoPath("software", "examples", "novaz", "src", "zvm6.s"));
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

        // The composite scroll builds the exact gfx pixel window into the
        // VTEXT_GFX_* registers, scrolls the snapped text-cell window, then
        // forces the left partial-cell column (VTEXT_LEFT-1) transparent so the
        // gfx behind it (pillar edge or plain background) shows through instead
        // of an opaque text-bg cell — the Zork Zero left-gutter "maroon bars".
        // It must not derive the gfx rectangle from the snapped VTEXT cell rect.
        StringAssert.Contains(rows, "JSR nz6_build_gfx_region_tmp_win");
        StringAssert.Contains(rows, "JSR vtext_scroll_composite_up");
        StringAssert.Contains(rows, "JSR nz6_scroll_left_bgtrans_column");
        Assert.IsFalse(rows.Contains("vtext_scroll_mixed_up", StringComparison.Ordinal),
            "Z6 composite scroll must not derive the gfx rectangle from the snapped VTEXT cell rectangle.");

        // The left-gutter column must be forced transparent unconditionally for
        // every window row; the old flag-gated copy/fill left opaque bg-index-0
        // (maroon) cells on scrolled rows.
        string leftCol = Slice(zvm6, "nz6_scroll_left_bgtrans_column:", "nz6_calc_left_attr_addr:");
        StringAssert.Contains(leftCol, "LDA #VTXT_ATTR_BGTRANS");
        Assert.IsFalse(leftCol.Contains("LDA nz6_scroll_left_flag", StringComparison.Ordinal),
            "Left-gutter column must be normalized unconditionally, not gated on a prior-BGTRANS flag.");
        Assert.IsFalse(leftCol.Contains("LDA VTEXT_ATTR", StringComparison.Ordinal),
            "Left-gutter fill must use BGTRANS, not the live VTEXT_ATTR (opaque during normal text).");
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

    private static void AssertVtextBorrowedRectangleIsRestored(string routine, string workCall)
    {
        int saveLeft = IndexOfOrFail(routine, "LDA VTEXT_LEFT\n        PHA", "VTEXT_LEFT save");
        int saveTop = IndexOfOrFail(routine, "LDA VTEXT_TOP\n        PHA", "VTEXT_TOP save");
        int saveWidth = IndexOfOrFail(routine, "LDA VTEXT_WIDTH\n        PHA", "VTEXT_WIDTH save");
        int saveHeight = IndexOfOrFail(routine, "LDA VTEXT_HEIGHT\n        PHA", "VTEXT_HEIGHT save");
        int borrow = IndexOfOrFail(routine, "LDA nz6_clr_tmp\n        STA VTEXT_LEFT", "borrowed VTEXT_LEFT");
        int call = IndexOfOrFail(routine, workCall, "borrowed rectangle work");
        int restoreHeight = IndexOfOrFail(routine, "PLA\n        STA VTEXT_HEIGHT", "VTEXT_HEIGHT restore");
        int restoreWidth = IndexOfOrFail(routine, "PLA\n        STA VTEXT_WIDTH", "VTEXT_WIDTH restore");
        int restoreTop = IndexOfOrFail(routine, "PLA\n        STA VTEXT_TOP", "VTEXT_TOP restore");
        int restoreLeft = IndexOfOrFail(routine, "PLA\n        STA VTEXT_LEFT", "VTEXT_LEFT restore");

        Assert.IsTrue(saveLeft < saveTop && saveTop < saveWidth && saveWidth < saveHeight,
            "Borrowed VTEXT rectangle must save current LEFT/TOP/WIDTH/HEIGHT before mutation.");
        Assert.IsTrue(saveHeight < borrow && borrow < call,
            "Borrowed VTEXT rectangle must be installed only after the current rectangle is saved.");
        Assert.IsTrue(call < restoreHeight && restoreHeight < restoreWidth &&
                      restoreWidth < restoreTop && restoreTop < restoreLeft,
            "Borrowed VTEXT rectangle must be restored after the helper work finishes.");
    }

    private static string RepoPath(params string[] parts)
    {
        string root = Path.GetFullPath(Path.Combine(
            AppContext.BaseDirectory, "..", "..", "..", ".."));
        return Path.Combine([root, .. parts]);
    }
}

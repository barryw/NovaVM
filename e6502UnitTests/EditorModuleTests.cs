using System;
using System.IO;
using System.Text.RegularExpressions;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

/// <summary>
/// Verifies the EDITOR module image (id=$08, built from modules/editor/editor.s)
/// carries the canonical paged-library header at $C000. Forth and Logo EDIT call
/// this module directly, so the loader must see the exact id/count contract.
/// </summary>
[TestClass]
public class EditorModuleTests
{
    [TestMethod]
    public void EditorModule_Header_IsWellFormed()
    {
        byte[] img = File.ReadAllBytes(RepoPath("software", "modules", "editor", "editor.bin"));

        Assert.AreEqual(16384, img.Length, "module image must be exactly 16 KB");
        Assert.AreEqual(0x4C, img[0]);   // JMP opcode at $C000
        Assert.AreEqual(0x4E, img[3]);   // 'N'
        Assert.AreEqual(0x4C, img[4]);   // 'L'
        Assert.AreEqual(0x08, img[5]);   // MODULE_ID_EDITOR
        Assert.AreEqual(0x01, img[6]);   // LIB_ABI_VERSION
        Assert.AreEqual(0x02, img[7]);   // EDITOR_FN_COUNT (RAM and XRAM documents)
    }

    [TestMethod]
    public void EditorModule_DoesNotLinkVspriteRuntime()
    {
        string map = File.ReadAllText(RepoPath("software", "modules", "editor", "editor.map"));

        Assert.IsFalse(
            map.Contains("nova.lib(vsprite.o)", StringComparison.Ordinal),
            "The editor module must not pull the full vsprite runtime for dialog rectangle fills.");
    }

    [TestMethod]
    public void EditorModule_DispatchTablePublishesRamAndXramEditors()
    {
        string src = File.ReadAllText(RepoPath("software", "modules", "editor", "editor.s"));
        string table = Slice(src, "sys_jtable:", "; ===========================================================================");
        string editEx = "EDITOR_FN_EDIT" + "_EX";
        string profilePrefix = "EDITOR_EDIT" + "_PROFILE";
        string oldDispatch = "sys_edit" + "_ex";
        string forthHilite = "sys_edit_hilite" + "_forth";
        string logoHilite = "sys_edit_hilite" + "_logo";
        string forthType = "sys_edit_type" + "_forth";
        string logoType = "sys_edit_type" + "_logo";

        StringAssert.Contains(table, ".word   sys_edit-1");
        StringAssert.Contains(table, ".word   sys_edit_xram-1");
        Assert.IsFalse(table.Contains(oldDispatch, StringComparison.Ordinal), "The editor dispatch table must not restore the legacy extended ABI.");
        Assert.IsFalse(src.Contains(editEx, StringComparison.Ordinal), "The editor module must not keep the legacy extended edit ABI.");
        Assert.IsFalse(src.Contains(profilePrefix, StringComparison.Ordinal), "The editor module must not dispatch language profile bytes.");
        Assert.IsFalse(src.Contains(forthHilite, StringComparison.Ordinal), "Forth highlighting belongs in Forth-owned hooks.");
        Assert.IsFalse(src.Contains(logoHilite, StringComparison.Ordinal), "Logo highlighting belongs in Logo-owned hooks.");
        Assert.IsFalse(src.Contains(forthType, StringComparison.Ordinal), "Forth type text belongs in a caller-owned hook table.");
        Assert.IsFalse(src.Contains(logoType, StringComparison.Ordinal), "Logo type text belongs in a caller-owned hook table.");
        StringAssert.Contains(src, "sys_edit_apply_hook_table:");
        StringAssert.Contains(src, "sys_edit_hook_save_hook:");
    }

    [TestMethod]
    public void ForthAndLogoUseSingleEditorAbiWithoutProfiles()
    {
        string forth = File.ReadAllText(RepoPath("software", "languages", "novaforth", "novaforth.s"));
        string logo = File.ReadAllText(RepoPath("software", "languages", "novalogo", "procedures.s"))
            + "\n"
            + File.ReadAllText(RepoPath("software", "languages", "novalogo", "novalogo_hooks.s"));

        AssertUsesSingleEditorAbi("NovaForth", forth);
        AssertUsesSingleEditorAbi("NovaLogo", logo);
        AssertInstallsHookTable("NovaForth", forth);
        AssertInstallsHookTable("NovaLogo", logo);

        string logoMakefile = File.ReadAllText(RepoPath("software", "languages", "novalogo", "Makefile"));
        StringAssert.Contains(logoMakefile, "LOGOHK.BIN");
        StringAssert.Contains(logoMakefile, "NDI_ASSETS := --asset LOGOHK.BIN:LOGOHK.BIN");

        string logoHooks = File.ReadAllText(RepoPath("software", "languages", "novalogo", "novalogo_hooks.s"));
        StringAssert.Contains(logoHooks, "editor_hooks_status:\n      .word 0",
            "NovaLogo must not install its file type as a persistent status override; status is transient editor-owned UI.");
        Assert.IsFalse(Regex.IsMatch(forth, @"STA\s+editor_hooks\+EDITOR_HOOKS_STATUS[HL]"),
            "NovaForth must not install a persistent status override; status is transient editor-owned UI.");

        static void AssertUsesSingleEditorAbi(string name, string source)
        {
            string profilePrefix = "EDITOR_EDIT" + "_PROFILE";
            string editEx = "EDITOR_FN_EDIT" + "_EX";
            Assert.IsFalse(source.Contains(profilePrefix, StringComparison.Ordinal), $"{name} must not pass a legacy editor profile byte.");
            Assert.IsFalse(source.Contains(editEx, StringComparison.Ordinal), $"{name} must use the single EDITOR_FN_EDIT hook-table ABI.");
            Assert.IsTrue(source.Contains("EDITOR_FN_EDIT", StringComparison.Ordinal), $"{name} must call the single editor function.");
        }

        static void AssertInstallsHookTable(string name, string source)
        {
            Assert.IsTrue(source.Contains("editor_install_hooks:", StringComparison.Ordinal), $"{name} must install its editor hooks before calling the editor.");
            Assert.IsTrue(source.Contains("editor_hook_code_start:", StringComparison.Ordinal), $"{name} must own low-RAM hook code.");
            Assert.IsTrue(source.Contains("editor_hook_code_end:", StringComparison.Ordinal), $"{name} must size the hook-code copy from labels.");
            Assert.IsTrue(source.Contains("EDITOR_HOOKS_SIZE", StringComparison.Ordinal), $"{name} must build an EDITOR_HOOKS_* table before calling the editor.");
            Assert.IsTrue(source.Contains("EDITOR_HOOKS_TYPEL", StringComparison.Ordinal), $"{name} must provide its document type through the hook table.");
            Assert.IsTrue(source.Contains("EDITOR_HOOKS_SAVE_VECL", StringComparison.Ordinal), $"{name} must install a save hook.");
            Assert.IsTrue(source.Contains("EDITOR_HOOKS_INDENT_VECL", StringComparison.Ordinal), $"{name} must install an indent hook.");
            Assert.IsTrue(source.Contains("EDITOR_HOOKS_HILITE_VECL", StringComparison.Ordinal), $"{name} must install a syntax highlight hook.");
            Assert.IsTrue(source.Contains("EDITOR_HOOKS_MENU_VECL", StringComparison.Ordinal), $"{name} must install a menu hook, even if it is a no-op.");
            Assert.IsTrue(source.Contains("EDITOR_HOOKS_COMMAND_VECL", StringComparison.Ordinal), $"{name} must install a command hook, even if it is a no-op.");
            Assert.IsTrue(source.Contains("EDITOR_HOOKS_CHANGED_VECL", StringComparison.Ordinal), $"{name} must install a changed hook, even if it is a no-op.");
            Assert.IsTrue(source.Contains("editor_hook_indent", StringComparison.Ordinal), $"{name} must own its indentation routine.");
            Assert.IsTrue(source.Contains("editor_hook_hilite", StringComparison.Ordinal), $"{name} must own its syntax highlighter.");
            Assert.IsTrue(source.Contains("LIB_ARG3+2", StringComparison.Ordinal), $"{name} must pass the hook-table pointer in ARG3 high word.");
        }
    }

    [TestMethod]
    public void EditorModule_HasRomHeadroom()
    {
        string map = File.ReadAllText(RepoPath("software", "modules", "editor", "editor.map"));
        int rodataEnd = ParseSegmentEnd(map, "RODATA");
        const int vectorsStart = 0xFFFA;
        int freeBytes = vectorsStart - (rodataEnd + 1);

        // Lowered from $0400 as word-wise movement, shift-selection, block
        // indent/unindent, auto-indent and the toolchain command bindings
        // landed. The module is now close to full: the next editor feature of
        // any size needs space reclaimed here first rather than another step
        // down. Move this floor only for deliberate work, never to absorb drift.
        Assert.IsTrue(
            freeBytes >= 0x00C0,
            $"Editor module is nearly full; keep at least $00C0 bytes free. Found ${freeBytes:X4}.");
    }

    private static string RepoPath(params string[] parts)
    {
        string? dir = Directory.GetCurrentDirectory();
        while (dir != null)
        {
            string candidate = Path.Combine(dir, "software", "runtime", "asm", "libeditor.inc");
            if (File.Exists(candidate))
            {
                string[] allParts = new string[parts.Length + 1];
                allParts[0] = dir;
                Array.Copy(parts, 0, allParts, 1, parts.Length);
                return Path.Combine(allParts);
            }
            dir = Directory.GetParent(dir)?.FullName;
        }
        throw new DirectoryNotFoundException("Could not locate repository root.");
    }

    private static int ParseSegmentEnd(string map, string segment)
    {
        var match = Regex.Match(map, $@"^\s*{segment}\s+[0-9A-Fa-f]{{6}}\s+([0-9A-Fa-f]{{6}})\s+", RegexOptions.Multiline);
        Assert.IsTrue(match.Success, $"Could not find {segment} segment in editor.map.");
        return Convert.ToInt32(match.Groups[1].Value, 16);
    }

    private static string Slice(string source, string startMarker, string endMarker)
    {
        int start = source.IndexOf(startMarker, StringComparison.Ordinal);
        Assert.IsTrue(start >= 0, $"Missing start marker {startMarker}.");
        int end = source.IndexOf(endMarker, start, StringComparison.Ordinal);
        Assert.IsTrue(end > start, $"Missing end marker {endMarker} after {startMarker}.");
        return source[start..end];
    }
}

using System;
using System.IO;
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
        Assert.AreEqual(0x01, img[7]);   // EDITOR_FN_COUNT (EDITOR_FN_EDIT)
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
}

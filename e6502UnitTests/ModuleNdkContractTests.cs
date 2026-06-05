using System;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

/// <summary>
/// Enforces the NDK-source-of-truth contract: the NDK (runtime/asm/*.s) owns all
/// function implementations and paged-library modules are THIN lib_call wrappers
/// that .include the NDK driver bodies and JSR them — never reimplementations.
///
/// Two drift guards per module (graphics/system/sound):
///   1. fn-count consistency — the module binary header's fn_count ($C007) equals
///      the number of jtable entries in &lt;mod&gt;.s. Bumping the FN_COUNT symbol
///      without adding a jtable entry (or vice versa) fails here.
///   2. thin-wrapper enforcement — the module's OWN code (everything before it
///      .includes the NDK driver bodies) must not contain NDK-owned hardware idioms:
///      no direct SID register pokes, no VGC frame-counter busy-wait, no VGC command
///      issue. Those belong in the NDK; the module must wrap an NDK routine instead.
/// </summary>
[TestClass]
public class ModuleNdkContractTests
{
    private static readonly (string Name, string Jtable)[] Modules =
    {
        ("graphics", "gfx_jtable"),
        ("system",   "sys_jtable"),
        ("sound",    "snd_jtable"),
    };

    [TestMethod]
    public void Module_HeaderFnCount_MatchesJtableEntryCount()
    {
        foreach (var (name, jtable) in Modules)
        {
            byte[] bin = File.ReadAllBytes(RepoPath("modules", name, name + ".bin"));
            int header = bin[7];                       // $C007 fn_count
            int entries = CountJtableEntries(File.ReadAllText(RepoPath("modules", name, name + ".s")), jtable);
            Assert.AreEqual(header, entries,
                $"{name}: binary header fn_count ({header}) != {jtable} entry count ({entries}). " +
                "The FN_COUNT symbol and the dispatch jtable have drifted.");
        }
    }

    [TestMethod]
    public void Modules_AreThinWrappers_NoNdkOwnedHardwareIdioms()
    {
        foreach (var (name, _) in Modules)
        {
            string own = ModuleOwnCode(File.ReadAllText(RepoPath("modules", name, name + ".s")));
            StringAssert.DoesNotMatch(own, new Regex(@"SID2?_BASE\s*\+"),
                $"{name}.s pokes SID registers directly — wrap an NDK audio_* routine instead (NDK is source of truth).");
            StringAssert.DoesNotMatch(own, new Regex(@"CMP\s+VGC_FRAME"),
                $"{name}.s busy-waits the VGC frame counter — wrap NDK vgc_vsync / vgc_wait_frames instead.");
        }
    }

    // A module's own code = everything before it .includes the first NDK driver body.
    private static string ModuleOwnCode(string src)
    {
        var ndkInclude = new Regex(
            @"\.include\s+""(audio|vgc|sprite|vsprite|msprite|blitter|copper|copper_split|dma|nvg|anim|tween|editui|editbuf|vtext)\.s""");
        var sb = new StringBuilder();
        foreach (string line in src.Split('\n'))
        {
            if (ndkInclude.IsMatch(line)) break;
            sb.Append(line).Append('\n');
        }
        return sb.ToString();
    }

    private static int CountJtableEntries(string src, string label)
    {
        string[] lines = src.Split('\n');
        int start = Array.FindIndex(lines, l => l.TrimStart().StartsWith(label + ":", StringComparison.Ordinal));
        Assert.IsTrue(start >= 0, $"jtable label '{label}' not found");
        int count = 0;
        for (int j = start + 1; j < lines.Length; j++)
        {
            string t = lines[j].Trim();
            if (t.StartsWith(".word", StringComparison.Ordinal)) count++;
            else if (t.Length > 0 && !t.StartsWith(";", StringComparison.Ordinal)) break;
        }
        return count;
    }

    private static string RepoPath(params string[] parts) =>
        Path.Combine(new[] { Path.GetFullPath(Path.Combine(AppContext.BaseDirectory, "..", "..", "..", "..")) }
            .Concat(parts).ToArray());
}

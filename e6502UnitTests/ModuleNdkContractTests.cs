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
        ("files",    "file_jtable"),
        ("memory",   "mem_jtable"),
        ("net",      "net_jtable"),
    };

    [TestMethod]
    public void Module_HeaderFnCount_MatchesJtableEntryCount()
    {
        foreach (var (name, jtable) in Modules)
        {
            byte[] bin = File.ReadAllBytes(RepoPath("software", "modules", name, name + ".bin"));
            int header = bin[7];                       // $C007 fn_count
            int entries = CountJtableEntries(File.ReadAllText(RepoPath("software", "modules", name, name + ".s")), jtable);
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
            string own = ModuleOwnCode(File.ReadAllText(RepoPath("software", "modules", name, name + ".s")));
            StringAssert.DoesNotMatch(own, new Regex(@"SID2?_BASE\s*\+"),
                $"{name}.s pokes SID registers directly — wrap an NDK audio_* routine instead (NDK is source of truth).");
            StringAssert.DoesNotMatch(own, new Regex(@"CMP\s+VGC_FRAME"),
                $"{name}.s busy-waits the VGC frame counter — wrap NDK vgc_vsync / vgc_wait_frames instead.");
        }
    }

    /// <summary>
    /// docs-from-NDK drift guard (CI mirror of nmod_pack's build-time check): every
    /// module fn that carries a <c>;@ndk &lt;symbol&gt;</c> mapping (the single source
    /// of its doc summary) must (1) map to a routine that actually exists in the NDK
    /// sources, and (2) have a wrapper body that really JSRs that routine. A stale or
    /// lying mapping fails here even if the committed .nmod was not rebuilt.
    /// </summary>
    [TestMethod]
    public void Module_NdkMappings_Exist_AndWrappersCallThem()
    {
        var ndkSymbols = NdkRoutineLabels();
        Assert.IsTrue(ndkSymbols.Contains("vgc_vsync"),
            "sanity: NDK routine label scan found nothing — check runtime/asm path.");

        foreach (var (name, jtable) in Modules)
        {
            string src = File.ReadAllText(RepoPath("software", "modules", name, name + ".s"));
            var ids = ParseIncIds(File.ReadAllText(RepoPath("software", "runtime", "asm", "lib" + name + ".inc")));
            string[] labels = JtableLabels(src, jtable);

            foreach (var (fn, ndk) in NdkMappings(src))
            {
                Assert.IsTrue(ids.ContainsKey(fn),
                    $"{name}.s: ;@fn {fn} has no id in lib{name}.inc.");
                int id = ids[fn];
                Assert.IsTrue(id < labels.Length,
                    $"{name}.s: {fn} id ${id:X2} beyond {jtable} ({labels.Length} entries).");
                Assert.IsTrue(ndkSymbols.Contains(ndk),
                    $"{name}.s: {fn} maps to NDK routine '{ndk}' which is not defined in runtime/asm/*.s.");
                string body = WrapperBody(src, labels[id]);
                StringAssert.Matches(body, new Regex(@"\b[jJ][sS][rR]\s+" + Regex.Escape(ndk) + @"\b"),
                    $"{name}.s: wrapper '{labels[id]}' ({fn}) does not JSR its mapped NDK routine '{ndk}'.");
            }
        }
    }

    // (;@fn NAME, ;@ndk symbol) pairs — a fn's mapping is the ;@ndk line before the
    // next ;@fn (fns without a ;@ndk are skipped: composite/non-routine wrappers).
    private static System.Collections.Generic.IEnumerable<(string Fn, string Ndk)> NdkMappings(string src)
    {
        string? fn = null;
        foreach (string raw in src.Split('\n'))
        {
            string line = raw.Trim();
            var mf = Regex.Match(line, @"^;@fn\s+(\S+)");
            if (mf.Success) { fn = mf.Groups[1].Value; continue; }
            var mn = Regex.Match(line, @"^;@ndk\s+(\S+)");
            if (mn.Success && fn != null) yield return (fn, mn.Groups[1].Value);
        }
    }

    // NAME = $HH constant ids from a lib*.inc.
    private static System.Collections.Generic.Dictionary<string, int> ParseIncIds(string inc)
    {
        var ids = new System.Collections.Generic.Dictionary<string, int>();
        foreach (Match m in Regex.Matches(inc, @"(?m)^\s*([A-Za-z_]\w*)\s*=\s*\$([0-9A-Fa-f]+)\b"))
            ids[m.Groups[1].Value] = Convert.ToInt32(m.Groups[2].Value, 16);
        return ids;
    }

    // Dispatch jtable wrapper labels in fn-id order (.word label-1 entries).
    private static string[] JtableLabels(string src, string label)
    {
        string[] lines = src.Split('\n');
        int start = Array.FindIndex(lines, l => l.TrimStart().StartsWith(label + ":", StringComparison.Ordinal));
        Assert.IsTrue(start >= 0, $"jtable label '{label}' not found");
        var labels = new System.Collections.Generic.List<string>();
        for (int j = start + 1; j < lines.Length; j++)
        {
            string t = lines[j].Trim();
            var m = Regex.Match(t, @"^\.word\s+([A-Za-z_]\w*)\s*-\s*1\b");
            if (m.Success) labels.Add(m.Groups[1].Value);
            else if (t.Length > 0 && !t.StartsWith(";", StringComparison.Ordinal)) break;
        }
        return labels.ToArray();
    }

    // A wrapper's body = from its `label:` definition to the next top-level label.
    private static string WrapperBody(string src, string label)
    {
        string[] lines = src.Split('\n');
        int start = Array.FindIndex(lines, l => Regex.IsMatch(l, @"^" + Regex.Escape(label) + @":"));
        Assert.IsTrue(start >= 0, $"wrapper label '{label}' not found");
        var sb = new StringBuilder();
        for (int j = start; j < lines.Length; j++)
        {
            if (j > start && Regex.IsMatch(lines[j], @"^[A-Za-z_]\w*:")) break;
            sb.Append(lines[j]).Append('\n');
        }
        return sb.ToString();
    }

    // All NDK routine labels (`symbol:` at column 0) across runtime/asm/*.s.
    private static System.Collections.Generic.HashSet<string> NdkRoutineLabels()
    {
        var set = new System.Collections.Generic.HashSet<string>(StringComparer.Ordinal);
        foreach (string path in Directory.GetFiles(RepoPath("software", "runtime", "asm"), "*.s"))
            foreach (Match m in Regex.Matches(File.ReadAllText(path), @"(?m)^([A-Za-z_]\w*):"))
                set.Add(m.Groups[1].Value);
        return set;
    }

    // A module's own code = everything before it .includes the first NDK driver body.
    private static string ModuleOwnCode(string src)
    {
        var ndkInclude = new Regex(
            @"\.include\s+""(audio|vgc|sprite|vsprite|msprite|blitter|copper|copper_split|dma|nvg|anim|tween|editui|editbuf|vtext|fio|pager|xram|xmc|nic|gameserver|nui|rng|overlay)\.s""");
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

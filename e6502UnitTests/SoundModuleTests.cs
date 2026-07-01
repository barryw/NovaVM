using System;
using System.IO;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

/// <summary>
/// Verifies the SOUND module image (id=$02, built from modules/sound/sound.s) carries
/// the canonical paged-library header at $C000:
///   $C000  JMP dispatch  ($4C ...)
///   $C003  "NL" magic    ($4E $4C)
///   $C005  module id     ($02  MODULE_ID_SOUND)
///   $C006  ABI version   ($01  LIB_ABI_VERSION)
///   $C007  fn count      ($1B  the full audio NDK: SID effects + music engine)
/// The simple SID commands (TONE/NOISE/VOLUME) moved out of the deleted NovaLogo
/// extension ROM in Phase B; the module was later expanded to wrap the whole
/// runtime/asm/audio.s NDK (27 fns). The exact header fn_count vs the dispatch
/// jtable is cross-checked dynamically in ModuleNdkContractTests.
/// </summary>
[TestClass]
public class SoundModuleTests
{
    [TestMethod]
    public void SoundModule_Header_IsWellFormed()
    {
        byte[] img = File.ReadAllBytes(RepoPath("software", "modules", "sound", "sound.bin"));

        Assert.AreEqual(16384, img.Length, "module image must be exactly 16 KB");
        Assert.AreEqual(0x4C, img[0]);   // JMP opcode at $C000
        Assert.AreEqual(0x4E, img[3]);   // 'N'
        Assert.AreEqual(0x4C, img[4]);   // 'L'
        Assert.AreEqual(0x02, img[5]);   // MODULE_ID_SOUND
        Assert.AreEqual(0x01, img[6]);   // LIB_ABI_VERSION
        Assert.AreEqual(0x1B, img[7]);   // SND_FN_COUNT (full audio NDK, 27 fns)
    }

    private static string RepoPath(params string[] parts)
    {
        string root = Path.GetFullPath(Path.Combine(
            AppContext.BaseDirectory, "..", "..", "..", ".."));
        return Path.Combine([root, .. parts]);
    }
}

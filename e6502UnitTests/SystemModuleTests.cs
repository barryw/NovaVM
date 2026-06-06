using System;
using System.IO;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

/// <summary>
/// Verifies the SYSTEM module image (id=$03, built from modules/system/system.s) carries
/// the canonical paged-library header at $C000:
///   $C000  JMP dispatch  ($4C ...)
///   $C003  "NL" magic    ($4E $4C)
///   $C005  module id     ($03  MODULE_ID_SYSTEM)
///   $C006  ABI version   ($01  LIB_ABI_VERSION)
///   $C007  fn count      ($11  EDIT + WAIT/WAITVBL/TIMER + RNG8/16/32 +
///                              DIALOG_DEFAULTS/DIALOG/DIALOG_WAIT/DIALOG_ERROR/WAIT_KEY +
///                              OVL_LOAD/UNLOAD/INIT/MAIN/TICK)
/// The header bytes are defined by runtime/asm/libmod.inc + libabi.inc + libsystem.inc;
/// this is the byte-exact guard the loader (lib_call) depends on when paging the editor.
/// </summary>
[TestClass]
public class SystemModuleTests
{
    [TestMethod]
    public void SystemModule_Header_IsWellFormed()
    {
        byte[] img = File.ReadAllBytes(RepoPath("modules", "system", "system.bin"));

        Assert.AreEqual(16384, img.Length, "module image must be exactly 16 KB");
        Assert.AreEqual(0x4C, img[0]);   // JMP opcode at $C000
        Assert.AreEqual(0x4E, img[3]);   // 'N'
        Assert.AreEqual(0x4C, img[4]);   // 'L'
        Assert.AreEqual(0x03, img[5]);   // MODULE_ID_SYSTEM
        Assert.AreEqual(0x01, img[6]);   // LIB_ABI_VERSION
        Assert.AreEqual(0x11, img[7]);   // SYS_FN_COUNT (EDIT + WAIT/WAITVBL/TIMER + RNG + DIALOG + OVL)
    }

    private static string RepoPath(params string[] parts)
    {
        string root = Path.GetFullPath(Path.Combine(
            AppContext.BaseDirectory, "..", "..", "..", ".."));
        return Path.Combine([root, .. parts]);
    }
}

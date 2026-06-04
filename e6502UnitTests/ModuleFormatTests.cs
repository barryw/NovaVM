using System;
using System.IO;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

/// <summary>
/// Verifies the paged-library "test module" image (id=$7F, ECHO/ADD/SUM) built from
/// tests/asm/testmod.s carries the canonical module header at $C000:
///   $C000  JMP dispatch  ($4C ...)
///   $C003  "NL" magic    ($4E $4C)
///   $C005  module id     ($7F  MODULE_ID_TEST)
///   $C006  ABI version   ($01  LIB_ABI_VERSION)
///   $C007  fn count      ($03)
/// The header bytes are defined by runtime/asm/libmod.inc + libabi.inc; this test
/// is the byte-exact guard the loader (lib_call) and FPGA page-in proofs depend on.
/// The 16 KB image is a deterministic, committed build artifact (tests/asm/testmod.bin),
/// resolved by walking up to the repo root from the test output directory — the same
/// pattern RuntimeLibraryAbiTests uses for runtime/asm sources.
/// </summary>
[TestClass]
public class ModuleFormatTests
{
    [TestMethod]
    public void TestModule_Header_IsWellFormed()
    {
        byte[] img = File.ReadAllBytes(RepoPath("tests", "asm", "testmod.bin"));

        Assert.AreEqual(16384, img.Length);
        Assert.AreEqual(0x4C, img[0]);   // JMP opcode at $C000
        Assert.AreEqual(0x4E, img[3]);   // 'N'
        Assert.AreEqual(0x4C, img[4]);   // 'L'
        Assert.AreEqual(0x7F, img[5]);   // MODULE_ID_TEST
        Assert.AreEqual(0x01, img[6]);   // LIB_ABI_VERSION
        Assert.AreEqual(0x03, img[7]);   // fn count
    }

    private static string RepoPath(params string[] parts)
    {
        string root = Path.GetFullPath(Path.Combine(
            AppContext.BaseDirectory,
            "..",
            "..",
            "..",
            ".."));

        return Path.Combine([root, .. parts]);
    }
}

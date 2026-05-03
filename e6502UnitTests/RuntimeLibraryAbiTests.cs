using System;
using System.IO;
using System.Text.RegularExpressions;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class RuntimeLibraryAbiTests
{
    [TestMethod]
    public void VTextStateUsesLinkerStorageNotNvrMailbox()
    {
        string inc = File.ReadAllText(RepoPath("ehbasic", "lib", "vtext.inc"));
        string impl = File.ReadAllText(RepoPath("ehbasic", "lib", "vtext.s"));

        string[] stateSymbols =
        [
            "VTEXT_LEFT",
            "VTEXT_TOP",
            "VTEXT_WIDTH",
            "VTEXT_HEIGHT",
            "VTEXT_CURX",
            "VTEXT_CURY",
            "VTEXT_COLOR",
            "VTEXT_ATTR",
            "VTEXT_CHAR",
            "VTEXT_FLAGS",
            "VTEXT_TABLEL",
            "VTEXT_TABLEH",
            "VTEXT_REGION_ID"
        ];

        foreach (string symbol in stateSymbols)
        {
            AssertNoNvrAlias(inc, symbol);
            StringAssert.Contains(impl, $"{symbol}:");
        }

        StringAssert.Contains(impl, ".segment \"BSS\"");
    }

    [TestMethod]
    public void XmcPrivateStateUsesLinkerStorageNotNvrMailbox()
    {
        string source = File.ReadAllText(RepoPath("ehbasic", "lib", "xmc.s"));

        string[] privateStateSymbols =
        [
            "xmc_eidx",
            "xmc_dircur",
            "xmc_npgL",
            "xmc_npgH",
            "xmc_tmp",
            "xmc_tmp2"
        ];

        foreach (string symbol in privateStateSymbols)
        {
            AssertNoNvrAlias(source, symbol);
            Assert.IsTrue(
                Regex.IsMatch(source, $@"(?m)^{Regex.Escape(symbol)}:\s+\.res\s+1\b"),
                $"{symbol} should be allocated as private linker storage.");
        }

        StringAssert.Contains(source, ".segment \"BSS\"");
    }

    [TestMethod]
    public void ExtensionRomCodeSegmentUsesLinkerPlacement()
    {
        string source = File.ReadAllText(RepoPath("ehbasic", "extension.s"));
        string config = File.ReadAllText(RepoPath("ehbasic", "extension.cfg"));

        Assert.IsFalse(
            Regex.IsMatch(source, @"(?im)^\s*\.org\s+\$C000\b"),
            "Extension CODE must not pin .org $C000; it breaks linker-owned BSS relocation.");
        StringAssert.Contains(config, "BSS:");
    }

    private static void AssertNoNvrAlias(string source, string symbol)
    {
        Assert.IsFalse(
            Regex.IsMatch(source, $@"(?m)^\s*{Regex.Escape(symbol)}\s*=\s*NVR\d[HL]?\b"),
            $"{symbol} must not alias an NVR pseudo-register.");
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

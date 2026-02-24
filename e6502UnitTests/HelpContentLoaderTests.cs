using System;
using System.IO;
using System.Linq;
using e6502.Avalonia.Help;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class HelpContentLoaderTests
{
    [TestMethod]
    public void LoadFromDirectory_FindsMarkdownFiles()
    {
        var helpDir = FindHelpDirectory();
        if (helpDir == null)
        {
            Assert.Inconclusive("docs/help/ directory not found — run from repo root");
            return;
        }

        var topics = HelpContentLoader.LoadFromDirectory(helpDir);
        Assert.IsTrue(topics.Count > 0, "Should find at least one .md file");
    }

    [TestMethod]
    public void LoadFromDirectory_ParsesTopicsCorrectly()
    {
        var helpDir = FindHelpDirectory();
        if (helpDir == null)
        {
            Assert.Inconclusive("docs/help/ directory not found");
            return;
        }

        var topics = HelpContentLoader.LoadFromDirectory(helpDir);
        Assert.IsTrue(topics.All(t => !string.IsNullOrEmpty(t.Title)));
        Assert.IsTrue(topics.All(t => Enum.IsDefined(t.Type)));
    }

    [TestMethod]
    public void LoadFromDirectory_SetsCorrectSourcePaths()
    {
        var helpDir = FindHelpDirectory();
        if (helpDir == null)
        {
            Assert.Inconclusive("docs/help/ directory not found");
            return;
        }

        var topics = HelpContentLoader.LoadFromDirectory(helpDir);
        Assert.IsTrue(topics.All(t => !Path.IsPathRooted(t.SourcePath)));
        Assert.IsTrue(topics.All(t => t.SourcePath.EndsWith(".md")));
    }

    private static string? FindHelpDirectory()
    {
        var dir = Directory.GetCurrentDirectory();
        while (dir != null)
        {
            var candidate = Path.Combine(dir, "docs", "help");
            if (Directory.Exists(candidate))
                return candidate;
            dir = Directory.GetParent(dir)?.FullName;
        }
        return null;
    }
}

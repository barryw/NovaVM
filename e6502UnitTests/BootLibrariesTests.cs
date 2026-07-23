using System;
using System.IO;
using System.Text.Json.Nodes;
using e6502.Storage;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class BootLibrariesTests
{
    [TestMethod]
    public void SetEntry_NewModule_WritesFullSchemaNoBase()
    {
        var cfg = new JsonObject();
        BootLibraries.SetEntry(cfg, "graphics.nmod", id: 1, version: 1, size: 16384);

        JsonObject e = cfg["libraries"]!.AsArray()[0]!.AsObject();
        Assert.AreEqual("graphics", (string?)e["name"]);
        Assert.AreEqual(1, (int)e["id"]!);
        Assert.AreEqual(1, (int)e["version"]!);
        Assert.AreEqual("/lib/graphics.nmod", (string?)e["path"]);
        Assert.AreEqual(16384, (int)e["size"]!);
        Assert.IsNull(e["base"], "base must not be written (firmware assigns the slot)");
    }

    [TestMethod]
    public void SetEntry_ExistingModule_UpdatesInPlace()
    {
        var cfg = new JsonObject();
        BootLibraries.SetEntry(cfg, "graphics.nmod", 1, 1, 16384);
        BootLibraries.SetEntry(cfg, "graphics.nmod", 1, 2, 16384);   // version bump
        Assert.AreEqual(1, cfg["libraries"]!.AsArray().Count, "no duplicate entry");
        Assert.AreEqual(2, (int)cfg["libraries"]!.AsArray()[0]!["version"]!);
    }

    [TestMethod]
    public void SetEntry_LegacyEntryWithBase_StripsBase()
    {
        var cfg = new JsonObject
        {
            ["libraries"] = new JsonArray
            {
                new JsonObject { ["name"]="graphics", ["id"]=1, ["path"]="/lib/graphics.nmod", ["base"]=393216, ["size"]=16384 }
            }
        };
        BootLibraries.SetEntry(cfg, "graphics.nmod", 1, 1, 16384);
        Assert.IsNull(cfg["libraries"]!.AsArray()[0]!["base"], "updating a legacy entry must drop base");
    }

    [TestMethod]
    public void RemoveEntry_ByBasename_RemovesAndReportsTrue()
    {
        var cfg = new JsonObject();
        BootLibraries.SetEntry(cfg, "graphics.nmod", 1, 1, 16384);
        Assert.IsTrue(BootLibraries.RemoveEntry(cfg, "graphics.nmod"));
        Assert.AreEqual(0, cfg["libraries"]!.AsArray().Count);
        Assert.IsFalse(BootLibraries.RemoveEntry(cfg, "graphics.nmod"), "second remove is a no-op");
    }

    [TestMethod]
    public void StagedBootConfig_IncludesResidentToolLibrariesWithoutFixedBases()
    {
        string json = File.ReadAllText(RepoPath("e6502.ESP32", "novahost", "assets", "config", "boot.json"));
        JsonObject cfg = JsonNode.Parse(json)!.AsObject();
        JsonArray libs = cfg["libraries"]!.AsArray();

        JsonObject? editor = null;
        JsonObject? langrt = null;
        foreach (JsonNode? node in libs)
        {
            JsonObject lib = node!.AsObject();
            Assert.IsNull(lib["base"], "firmware assigns shelf slots; boot library entries must not pin XRAM bases");
            if ((string?)lib["name"] == "editor")
                editor = lib;
            if ((string?)lib["name"] == "langrt")
                langrt = lib;
        }

        Assert.IsNotNull(editor, "hardware boot config must catalog editor.nmod for EDITOR module calls");
        Assert.AreEqual(8, (int)editor!["id"]!);
        Assert.AreEqual("/lib/editor.nmod", (string?)editor["path"]);
        Assert.AreEqual(16384, (int)editor["size"]!);
        Assert.IsNotNull(langrt, "hardware boot config must catalog langrt.nmod for compiled-language calls");
        Assert.AreEqual(9, (int)langrt!["id"]!);
        Assert.AreEqual("/lib/langrt.nmod", (string?)langrt["path"]);
        Assert.AreEqual(16384, (int)langrt["size"]!);
    }

    private static string RepoPath(params string[] parts)
    {
        string? dir = Directory.GetCurrentDirectory();
        while (dir != null)
        {
            if (File.Exists(Path.Combine(dir, "e6502.sln")))
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

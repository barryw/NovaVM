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
}

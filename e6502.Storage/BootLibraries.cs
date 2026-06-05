using System.IO;
using System.Text.Json.Nodes;

namespace e6502.Storage;

/// <summary>
/// Boot-config libraries[] membership helpers, shared by the nova CLI and web.
/// A libraries[] entry names a paged-library module for the firmware to stage at
/// boot. Phase A: no `base` — the firmware assigns slot = array index and computes
/// the XRAM address. See docs/plans/2026-06-05-dynamic-module-shelf-design.md.
/// </summary>
public static class BootLibraries
{
    /// <summary>Add or update (by filename) a libraries[] entry. Writes
    /// {name,id,version,path,size}; never writes base.</summary>
    public static void SetEntry(JsonObject config, string fileName, int id, int version, int size)
    {
        if (config["libraries"] is not JsonArray libs)
        {
            libs = new JsonArray();
            config["libraries"] = libs;
        }
        string name = Path.GetFileNameWithoutExtension(fileName);
        string path = $"/lib/{fileName}";
        foreach (JsonNode? lib in libs)
        {
            if (lib is JsonObject o && Matches(o, fileName))
            {
                o["name"] = name; o["id"] = id; o["version"] = version;
                o["path"] = path; o["size"] = size;
                o.Remove("base");
                return;
            }
        }
        libs.Add(new JsonObject
        {
            ["name"] = name, ["id"] = id, ["version"] = version,
            ["path"] = path, ["size"] = size,
        });
    }

    /// <summary>Remove all libraries[] entries whose path basename matches
    /// <paramref name="fileName"/>. Returns true if anything was removed.</summary>
    public static bool RemoveEntry(JsonObject config, string fileName)
    {
        if (config["libraries"] is not JsonArray libs) return false;
        bool removed = false;
        for (int i = libs.Count - 1; i >= 0; i--)
        {
            if (libs[i] is JsonObject o && Matches(o, fileName)) { libs.RemoveAt(i); removed = true; }
        }
        return removed;
    }

    private static bool Matches(JsonObject entry, string fileName) =>
        string.Equals(Path.GetFileName(entry["path"]?.GetValue<string>() ?? ""),
                      fileName, System.StringComparison.OrdinalIgnoreCase);
}

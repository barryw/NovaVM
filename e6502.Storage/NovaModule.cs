using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.Json.Nodes;

namespace e6502.Storage;

/// <summary>One documented argument of a module function.</summary>
public sealed record NovaArg(int Index, string Name, string Type, string Desc);

/// <summary>A module function's return value.</summary>
public sealed record NovaRet(string Type, string Desc);

/// <summary>One documented module function (from the NDOC trailer).</summary>
public sealed record NovaFn(
    int Id, string IdHex, string Name, string? Brief,
    IReadOnlyList<NovaArg> Args, NovaRet Ret, string? Effect, IReadOnlyList<string> Status)
{
    /// <summary>e.g. "GFN_LINE(x0:s16, y0:s16) -> void".</summary>
    public string Signature() =>
        $"{Name}({string.Join(", ", Args.Select(a => $"{a.Name}:{a.Type}"))}) -> {Ret.Type}";
}

/// <summary>Module-level metadata from the NDOC trailer.</summary>
public sealed record NovaModuleMeta(
    string? Name, int Id, int AbiVersion, string? Version, string? Brief, string? AbiNote);

/// <summary>The parsed NDOC documentation trailer.</summary>
public sealed record NovaModuleDoc(
    int NdocVersion, NovaModuleMeta Module, IReadOnlyList<NovaFn> Functions);

/// <summary>
/// Parser for the .nmod / .mod paged-library module format: the 16384-byte image
/// with its "NL" header, plus an optional "NDOC" JSON documentation trailer.
/// Shared by the nova CLI and the web UI. See
/// docs/plans/2026-06-05-nmod-self-documenting-modules-design.md.
/// </summary>
public sealed class NovaModule
{
    public bool Valid { get; private init; }
    public string? Error { get; private init; }
    public int Id { get; private init; }
    public int AbiVersion { get; private init; }
    public int FnCount { get; private init; }
    public bool HasDoc { get; private init; }
    public NovaModuleDoc? Doc { get; private init; }

    public const int ImageSize = 16384;

    public static NovaModule Parse(byte[] bytes)
    {
        if (bytes.Length < ImageSize)
            return Invalid($"too short: {bytes.Length} bytes (need >= {ImageSize})");
        if (bytes[3] != 0x4E || bytes[4] != 0x4C)
            return Invalid("bad module magic at +3 (expected 'NL')");

        int id = bytes[5];
        int abi = bytes[6];
        int fn = bytes[7];

        NovaModuleDoc? doc = null;
        bool hasDoc = false;
        if (bytes.Length > ImageSize)
        {
            int off = ImageSize;
            if (bytes.Length < off + 8)
                return Invalid("truncated NDOC trailer (missing magic/length)");
            if (bytes[off] != (byte)'N' || bytes[off + 1] != (byte)'D'
                || bytes[off + 2] != (byte)'O' || bytes[off + 3] != (byte)'C')
                return Invalid("bad NDOC trailer magic (expected 'NDOC')");
            int len = bytes[off + 4] | (bytes[off + 5] << 8)
                    | (bytes[off + 6] << 16) | (bytes[off + 7] << 24);
            if (len < 0 || (long)off + 8 + len > bytes.Length)
                return Invalid("truncated NDOC trailer (length exceeds file)");
            try
            {
                string json = Encoding.UTF8.GetString(bytes, off + 8, len);
                doc = ParseDoc(json);
                hasDoc = true;
            }
            catch (Exception e)
            {
                return Invalid($"NDOC parse error: {e.Message}");
            }
        }

        return new NovaModule
        {
            Valid = true,
            Id = id,
            AbiVersion = abi,
            FnCount = fn,
            HasDoc = hasDoc,
            Doc = doc,
        };
    }

    /// <summary>
    /// Full pre-staging gate: the module must parse, and (if it carries an NDOC
    /// doc) every documented id must fall within the dispatch-table span
    /// [0, FnCount). A documented subset is fine — sparse-dispatch modules such as
    /// graphics only annotate their implemented ids. Returns (true, null) when the
    /// module is safe to stage, else (false, reason). Shared by the nova CLI
    /// (`module validate`/`put`) and the web (`PUT /api/modules/{name}`) so the two
    /// surfaces agree on what "valid" means.
    /// </summary>
    public (bool Ok, string? Reason) ValidateForStaging()
    {
        if (!Valid)
            return (false, Error);
        if (HasDoc && Doc is not null)
        {
            NovaFn? bad = Doc.Functions.FirstOrDefault(f => f.Id < 0 || f.Id >= FnCount);
            if (bad is not null)
                return (false, $"doc id ${bad.Id:X2} ({bad.Name}) " +
                               $"out of dispatch range [0, {FnCount})");
        }
        return (true, null);
    }

    private static NovaModule Invalid(string error) =>
        new() { Valid = false, Error = error };

    private static NovaModuleDoc ParseDoc(string json)
    {
        JsonObject root = JsonNode.Parse(json)!.AsObject();
        JsonObject mod = root["module"]!.AsObject();
        var meta = new NovaModuleMeta(
            (string?)mod["name"], (int)mod["id"]!, (int)mod["abiVersion"]!,
            (string?)mod["version"], (string?)mod["brief"], (string?)mod["abiNote"]);

        var fns = new List<NovaFn>();
        foreach (JsonNode? fnode in root["functions"]!.AsArray())
        {
            JsonObject f = fnode!.AsObject();

            var args = new List<NovaArg>();
            if (f["args"] is JsonArray aarr)
            {
                foreach (JsonNode? an in aarr)
                {
                    JsonObject a = an!.AsObject();
                    args.Add(new NovaArg(
                        (int)a["i"]!, (string)a["name"]!,
                        (string)a["type"]!, (string)a["desc"]!));
                }
            }

            NovaRet ret = f["ret"] is JsonObject rnode
                ? new NovaRet((string)rnode["type"]!, (string?)rnode["desc"] ?? "")
                : new NovaRet("void", "");

            var status = new List<string>();
            if (f["status"] is JsonArray sarr)
            {
                foreach (JsonNode? s in sarr)
                    status.Add((string)s!);
            }

            fns.Add(new NovaFn(
                (int)f["id"]!, (string)f["idHex"]!, (string)f["name"]!,
                (string?)f["brief"], args, ret, (string?)f["effect"], status));
        }

        return new NovaModuleDoc((int)root["ndocVersion"]!, meta, fns);
    }
}

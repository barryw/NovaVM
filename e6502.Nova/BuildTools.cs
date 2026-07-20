using System.Globalization;
using System.Collections;
using System.Text;
using System.Text.Encodings.Web;
using System.Text.Json;
using System.Text.RegularExpressions;
using System.Security.Cryptography;

internal static class NovaBuildTools
{
    public static void GenerateTokens(string asmPath, string outputPath)
    {
        string[] lines = File.ReadAllLines(asmPath);
        Dictionary<string, int> equates = ResolveTokenEquates(lines);
        Dictionary<int, string> primary = ParseKeywordTables(lines, equates);
        Dictionary<int, string> extended = ParseExtendedTokens(lines, equates);

        var result = new Dictionary<string, object?>
        {
            ["version"] = 1,
            ["prefix_byte"] = equates.TryGetValue("TKX_PREFIX", out int prefix) ? prefix : 1,
            ["primary"] = primary.OrderBy(kv => kv.Key).ToDictionary(kv => kv.Key.ToString(CultureInfo.InvariantCulture), kv => kv.Value),
            ["extended"] = extended.OrderBy(kv => kv.Key).ToDictionary(kv => kv.Key.ToString(CultureInfo.InvariantCulture), kv => kv.Value),
        };

        WriteJson(outputPath, result);
        Console.WriteLine($"Generated {outputPath}: {primary.Count} primary + {extended.Count} extended tokens");
    }

    private static Dictionary<string, int> ResolveTokenEquates(string[] lines)
    {
        var raw = new Dictionary<string, string>();
        Regex eq = new(@"^(TK_\w+|XTK_\w+|TKX_PREFIX)\s*=\s*(.+?)(?:\s*;.*)?$");
        foreach (string line in lines)
        {
            Match m = eq.Match(line.Trim());
            if (m.Success)
                raw[m.Groups[1].Value] = m.Groups[2].Value.Trim();
        }

        var resolved = new Dictionary<string, int>();
        int Resolve(string name)
        {
            if (resolved.TryGetValue(name, out int cached))
                return cached;
            if (!raw.TryGetValue(name, out string? expr))
                throw new InvalidOperationException($"unknown token equate: {name}");

            int value;
            if (expr.StartsWith("$", StringComparison.Ordinal))
                value = Convert.ToInt32(expr[1..], 16);
            else if (expr.StartsWith("0x", StringComparison.OrdinalIgnoreCase))
                value = Convert.ToInt32(expr, 16);
            else if (int.TryParse(expr, NumberStyles.Integer, CultureInfo.InvariantCulture, out int dec))
                value = dec;
            else
            {
                Match add = Regex.Match(expr, @"^(\w+)\s*\+\s*(\d+)$");
                Match sub = Regex.Match(expr, @"^(\w+)\s*-\s*(\w+)$");
                if (add.Success)
                    value = Resolve(add.Groups[1].Value) + int.Parse(add.Groups[2].Value, CultureInfo.InvariantCulture);
                else if (sub.Success)
                    value = Resolve(sub.Groups[1].Value) - Resolve(sub.Groups[2].Value);
                else
                    value = Resolve(expr);
            }

            resolved[name] = value;
            return value;
        }

        foreach (string name in raw.Keys)
            _ = Resolve(name);
        return resolved;
    }

    private static Dictionary<int, string> ParseKeywordTables(string[] lines, Dictionary<string, int> equates)
    {
        var tokens = new Dictionary<int, string>();
        Regex tabAsc = new(@"^TAB_ASC([A-Z])\s*$");
        Regex lbb = new(@"^LBB_\w+\s*$");
        var opTables = new Dictionary<string, string>
        {
            ["TAB_STAR"] = "*",
            ["TAB_PLUS"] = "+",
            ["TAB_MNUS"] = "-",
            ["TAB_SLAS"] = "/",
            ["TAB_LESS"] = "<",
            ["TAB_EQUL"] = "=",
            ["TAB_MORE"] = ">",
            ["TAB_QEST"] = "?",
            ["TAB_POWR"] = "^",
        };
        string prefix = "";
        bool inTable = false;

        foreach (string line in lines)
        {
            string stripped = line.Trim();
            Match asc = tabAsc.Match(stripped);
            if (asc.Success)
            {
                prefix = asc.Groups[1].Value;
                inTable = true;
                continue;
            }

            foreach ((string table, string value) in opTables)
            {
                if (stripped.StartsWith(table, StringComparison.Ordinal)
                    && (stripped.Length == table.Length || char.IsWhiteSpace(stripped[table.Length])))
                {
                    prefix = value;
                    inTable = true;
                    break;
                }
            }

            if (lbb.IsMatch(stripped))
                continue;

            if (!inTable || !stripped.StartsWith(".byte", StringComparison.Ordinal))
                continue;

            string content = StripAsmComment(stripped[5..].Trim());
            if (content is "$00" or "0")
            {
                inTable = false;
                continue;
            }

            (string Keyword, int Token)? entry = ParseByteEntry(content, prefix, equates);
            if (entry is not null)
                tokens[entry.Value.Token] = entry.Value.Keyword;
        }

        return tokens;
    }

    private static (string Keyword, int Token)? ParseByteEntry(string byteArgs, string prefix, Dictionary<string, int> equates)
    {
        int? token = null;
        var pieces = new List<string>();
        foreach (string raw in SplitAsmArgs(byteArgs))
        {
            string part = raw.Trim();
            if (equates.TryGetValue(part, out int value) && part.StartsWith("TK_", StringComparison.Ordinal))
                token = value;
            else if (part is "$00" or "0")
                continue;
            else if (part.StartsWith("\"", StringComparison.Ordinal) && part.EndsWith("\"", StringComparison.Ordinal))
                pieces.Add(part[1..^1]);
        }

        return token is null ? null : (prefix + string.Concat(pieces), token.Value);
    }

    private static Dictionary<int, string> ParseExtendedTokens(string[] lines, Dictionary<string, int> equates)
    {
        var result = new Dictionary<int, string>();
        HashSet<int> xtkIds = equates.Where(kv => kv.Key.StartsWith("XTK_", StringComparison.Ordinal))
                                     .Select(kv => kv.Value)
                                     .ToHashSet();
        var labelStrings = new Dictionary<string, string?>();
        Regex label = new(@"^@s_\w+:\s*\.byte\s+""([^""]*)"",0\s*(?:;.*)?$");
        Regex placeholder = new(@"^@s_\w+:\s*\.byte\s+\$FF,0");
        foreach (string line in lines)
        {
            string stripped = line.Trim();
            Match m = label.Match(stripped);
            if (m.Success)
                labelStrings[stripped.Split(':', 2)[0].Trim()] = m.Groups[1].Value;
            else if (placeholder.IsMatch(stripped))
                labelStrings[stripped.Split(':', 2)[0].Trim()] = null;
        }

        bool inTable = false;
        int repeatCount = 0;
        string? repeatLabel = null;
        var wordLabels = new List<string>();
        foreach (string line in lines)
        {
            string stripped = line.Trim();
            if (stripped.StartsWith("TAB_XTKSTR", StringComparison.Ordinal))
            {
                inTable = true;
                continue;
            }
            if (!inTable)
                continue;

            if (stripped.StartsWith(".repeat", StringComparison.Ordinal))
            {
                Match m = Regex.Match(stripped, @"\.repeat\s+\$?([0-9A-Fa-f]+)");
                if (m.Success)
                    repeatCount = stripped.Contains('$', StringComparison.Ordinal)
                        ? Convert.ToInt32(m.Groups[1].Value, 16)
                        : int.Parse(m.Groups[1].Value, CultureInfo.InvariantCulture);
            }
            else if (stripped.StartsWith(".endrepeat", StringComparison.Ordinal))
            {
                if (repeatLabel is not null)
                    for (int i = 0; i < repeatCount; i++)
                        wordLabels.Add(repeatLabel);
                repeatCount = 0;
                repeatLabel = null;
            }
            else if (stripped.StartsWith(".word", StringComparison.Ordinal))
            {
                string words = StripAsmComment(stripped[5..]);
                var labels = words.Split(',').Select(w => w.Trim()).Where(w => w.StartsWith("@", StringComparison.Ordinal)).ToList();
                if (repeatCount > 0)
                    repeatLabel = labels.FirstOrDefault();
                else
                    wordLabels.AddRange(labels);
            }
            else if (stripped.StartsWith("@s_", StringComparison.Ordinal) || stripped.Length == 0)
            {
                inTable = false;
            }
        }

        for (int i = 0; i < wordLabels.Count; i++)
        {
            int id = i + 1;
            if (xtkIds.Contains(id) && labelStrings.TryGetValue(wordLabels[i], out string? value) && !string.IsNullOrEmpty(value))
                result[id] = value;
        }

        return result;
    }

    public static void GenerateNovaVmInc(string vgcConstantsPath, string? symPath, string outputPath)
    {
        List<(string Section, string Name, string Value)> hw = ParseVgcConstants(vgcConstantsPath);
        List<(string Name, string Value)> rom = symPath is null ? [] : ParseBasicSymbols(symPath);
        int maxName = hw.Select(v => v.Name.Length).Concat(rom.Select(v => v.Name.Length)).DefaultIfEmpty(20).Max();

        var output = new List<string>
        {
            "; novavm.inc -- auto-generated, DO NOT EDIT",
            "; Sources: " + Path.GetFileName(vgcConstantsPath) + (symPath is null ? "" : ", " + Path.GetFileName(symPath)),
            "; Regenerate: nova codegen novavm-inc",
            "",
        };

        string? currentSection = null;
        foreach ((string section, string name, string value) in hw)
        {
            if (section != currentSection)
            {
                currentSection = section;
                output.Add("");
                output.Add($"; --- {section} ---");
            }
            output.Add($"{name.PadRight(maxName)} = {value}");
        }

        if (rom.Count > 0)
        {
            output.Add("");
            output.Add("; --- ROM Entry Points (from basic.sym) ---");
            foreach ((string name, string value) in rom.OrderBy(v => v.Value, StringComparer.Ordinal))
                output.Add($"{name.PadRight(maxName)} = {value}");
        }

        output.Add("");
        WriteText(outputPath, string.Join("\n", output));
        Console.WriteLine($"Generated {outputPath}: {hw.Count} hardware + {rom.Count} ROM symbols");
    }

    private static List<(string Section, string Name, string Value)> ParseVgcConstants(string path)
    {
        string[] lines = File.ReadAllLines(path);
        Regex constant = new(@"public\s+const\s+(?:int|byte|ushort)\s+(\w+)\s*=\s*(0x[0-9A-Fa-f]+|\d+)\s*;");
        Regex section = new(@"//\s*[-=]{5,}");
        string current = "General";
        var results = new List<(string, string, string)>();

        for (int i = 0; i < lines.Length; i++)
        {
            string line = lines[i];
            if (section.IsMatch(line.Trim()) && i + 1 < lines.Length)
            {
                string comment = lines[i + 1].Trim();
                if (comment.StartsWith("//", StringComparison.Ordinal))
                {
                    current = comment.TrimStart('/').Trim();
                    i++;
                }
            }

            Match m = constant.Match(line);
            if (!m.Success)
                continue;
            int value = ParseInt(m.Groups[2].Value);
            results.Add((current, m.Groups[1].Value, value > 0xFF ? $"${value:X4}" : $"${value:X2}"));
        }

        return results;
    }

    private static readonly HashSet<string> RomExports =
    [
        "V_INPT", "V_OUTP", "V_LOAD", "V_SAVE",
        "LAB_COLD", "LAB_WARM",
        "Smeml", "Smemh",
        "Svarl", "Svarh",
        "Earryl", "Earryh",
        "Sstorl", "Sstorh",
    ];

    private static List<(string Name, string Value)> ParseBasicSymbols(string path)
    {
        Regex sym = new(@"^al\s+([0-9A-Fa-f]+)\s+\.(\S+)");
        var result = new List<(string, string)>();
        foreach (string line in File.ReadLines(path))
        {
            Match m = sym.Match(line);
            if (!m.Success || !RomExports.Contains(m.Groups[2].Value))
                continue;
            int addr = Convert.ToInt32(m.Groups[1].Value, 16);
            result.Add((m.Groups[2].Value, addr > 0xFF ? $"${addr:X4}" : $"${addr:X2}"));
        }
        return result;
    }

    public static void GenerateRuntimeAbiDocs(IEnumerable<string> sources, string? symPath, string jsonPath, string mdPath, string? asmPath)
    {
        var sourcePaths = sources.ToList();
        List<DocEntry> entries = ParseTaggedSources(sourcePaths);
        Dictionary<string, string> equates = ParseEquates(sourcePaths);
        Dictionary<string, int> symbols = ParseViceSymbols(symPath);
        foreach (DocEntry entry in entries)
        {
            string symbol = entry.String("symbol");
            int? address = null;
            if (symbols.TryGetValue(symbol, out int symAddress))
                address = symAddress;
            else if (equates.ContainsKey(symbol))
                address = ResolveExpr(symbol, equates);
            if (address is not null)
                entry.Values["address"] = address > 0xFF ? $"${address:X4}" : $"${address:X2}";
        }
        entries = entries.OrderBy(e => e.String("label"), StringComparer.Ordinal).ToList();

        WriteJson(jsonPath, new Dictionary<string, object?> { ["version"] = 1, ["labels"] = entries.Select(e => e.Values).ToList() });
        WriteRuntimeMarkdown(entries, mdPath);
        if (asmPath is not null)
            WriteRuntimeAsm(entries, asmPath);

        var outputs = new List<string> { jsonPath, mdPath };
        if (asmPath is not null)
            outputs.Add(asmPath);
        Console.WriteLine($"Generated {string.Join(", ", outputs)}: {entries.Count} labels");
    }

    public static void GenerateNdkReference(string runtimeDir, string texPath, string jsonPath, string? pascalDir = null)
    {
        List<string> sources = Directory.GetFiles(runtimeDir, "*.inc")
                                        .Concat(Directory.GetFiles(runtimeDir, "*.s"))
                                        .Order(StringComparer.Ordinal)
                                        .ToList();
        List<DocEntry> entries = ParseTaggedSources(sources);
        ValidateNdkEntries(entries);
        EnrichNdkValues(entries, ParseEquates(sources));
        List<Dictionary<string, object?>> libraries = MakeNdkLibraries(entries);

        WriteJson(jsonPath, new Dictionary<string, object?> { ["version"] = 1, ["libraries"] = libraries });
        WriteNdkTex(libraries, texPath);
        if (pascalDir is not null)
            WriteNdkPascalBindings(libraries, pascalDir);
        int total = libraries.Sum(l => ((List<Dictionary<string, object?>>)l["entries"]!).Count);
        Console.WriteLine($"Generated {texPath}, {jsonPath}: {libraries.Count} libraries, {total} entries");
    }

    private static void WriteNdkPascalBindings(IEnumerable<Dictionary<string, object?>> libraries, string outputDirectory)
    {
        var markerSymbols = new Dictionary<string, string>(StringComparer.Ordinal);
        foreach (Dictionary<string, object?> library in libraries)
        {
            string name = Convert.ToString(library["name"], CultureInfo.InvariantCulture)!.ToUpperInvariant();
            string guard = "NPI_" + name;
            var lines = new List<string>
            {
                "; Generated by nova codegen ndk-reference. Do not edit.",
                $".ifndef {guard}",
                $"{guard} = 1",
            };
            if (name != "NOVA")
                lines.Add(".include \"NOVA.NPI\"");

            var entries = (List<Dictionary<string, object?>>)library["entries"]!;
            foreach (var group in entries
                         .Where(entry => Value(entry, "symbol").Length > 0)
                         .GroupBy(entry => (
                             Symbol: Value(entry, "symbol").ToUpperInvariant(),
                             Kind: Value(entry, "kind").ToLowerInvariant()))
                         .OrderBy(group => group.Key.Symbol, StringComparer.Ordinal))
            {
                Dictionary<string, object?> entry = group.First();
                string symbol = group.Key.Symbol;
                string kind = group.Key.Kind;
                int? routineSignature = null;
                if (kind == "routine")
                {
                    int?[] signatures = group.Select(NdkPascalRoutineSignature).Distinct().ToArray();
                    if (signatures.Length != 1)
                        throw new InvalidOperationException($"{symbol}: conflicting Pascal ABI signatures");
                    routineSignature = signatures[0];
                }
                string? declaration = kind switch
                {
                    "routine" => routineSignature is int signature
                        ? $"{NdkPascalMarker('S', symbol)} = ${signature:X2}"
                        : null,
                    "u8" or "zp" => $"{NdkPascalMarker('B', symbol)} = 1",
                    "const" when AddressToInt(entry.GetValueOrDefault("resolved")) is int value && value <= byte.MaxValue
                        => $"{NdkPascalMarker('C', symbol)} = 1",
                    _ => null,
                };
                if (declaration is null)
                    continue;
                string marker = declaration[..declaration.IndexOf(' ')];
                if (markerSymbols.TryGetValue(marker, out string? existing) && existing != symbol)
                    throw new InvalidOperationException($"Pascal binding hash collision: {existing} and {symbol}");
                markerSymbols[marker] = symbol;
                lines.Add($"; {symbol}");
                lines.Add(declaration);
            }
            lines.Add(".endif");
            WriteText(Path.Combine(outputDirectory, name + ".NPI"), string.Join("\n", lines) + "\n");
        }
    }

    private static string NdkPascalMarker(char kind, string symbol)
    {
        int hash = 5381;
        foreach (char ch in symbol.ToUpperInvariant())
            hash = ((hash << 5) + hash + ch) & 0xFFFFFF;
        return $"__{kind}{hash:X6}";
    }

    // bit 0: one Byte argument in A; bit 1: Byte result in A.
    private static int? NdkPascalRoutineSignature(Dictionary<string, object?> entry)
    {
        if (Value(entry, "abi").Equals("inline", StringComparison.OrdinalIgnoreCase))
            return null;

        bool argument = false;
        if (entry.GetValueOrDefault("inputs") is List<object?> inputs)
        {
            foreach (object? input in inputs)
            {
                string name = input is Dictionary<string, string> named
                    ? named.GetValueOrDefault("name", "").Trim().ToUpperInvariant()
                    : "";
                string[] registers = name.Split('/', StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries);
                bool cpuRegisters = registers.Length > 0 && registers.All(register => register is "A" or "X" or "Y");
                if (!cpuRegisters)
                    continue; // Named NDK pseudo-registers are prepared separately.
                if (registers.Length != 1 || registers[0] != "A" || argument)
                    return null;
                argument = true;
            }
        }

        bool result = entry.GetValueOrDefault("outputs") is List<object?> outputs
                      && outputs.OfType<Dictionary<string, string>>()
                          .Any(output => output.GetValueOrDefault("name", "").Trim().Equals("A", StringComparison.OrdinalIgnoreCase));
        return (argument ? 1 : 0) | (result ? 2 : 0);
    }

    public static void AssembleHelpBook(string manifestPath, string helpDir, string outputPath)
    {
        BookManifest book = ParseBookManifest(manifestPath);
        var parts = new List<string>
        {
            $"---\ntitle: \"{book.Title}\"\nsubtitle: \"{book.Subtitle}\"\n---\n",
        };

        foreach (BookSection chapter in book.Chapters)
        {
            parts.Add($"\n# {chapter.Title}\n");
            foreach (string source in chapter.Sources)
                parts.Add(ReadMarkdownBodyOrPending(Path.Combine(helpDir, source), source));
        }

        foreach (BookSection appendix in book.Appendices)
        {
            parts.Add($"\n# {appendix.Title}\n");
            if (appendix.AutoGenerate is not null)
            {
                string referenceDir = Path.Combine(helpDir, "reference", appendix.AutoGenerate);
                if (Directory.Exists(referenceDir))
                {
                    foreach (string path in Directory.GetFiles(referenceDir, "*.md").Order(StringComparer.Ordinal))
                    {
                        parts.Add($"\n## {ExtractMarkdownTitle(path)}\n");
                        parts.Add(ReadMarkdownBody(path));
                        parts.Add("\n");
                    }
                }
                continue;
            }

            foreach (string source in appendix.Sources)
                parts.Add(ReadMarkdownBodyOrPending(Path.Combine(helpDir, source), source));
        }

        WriteText(outputPath, string.Join("\n", parts));
    }

    public static int CheckFpgaTiming(string reportPath, string? logPath, double marginMhz)
    {
        using JsonDocument doc = JsonDocument.Parse(File.ReadAllText(reportPath));
        if (!doc.RootElement.TryGetProperty("fmax", out JsonElement fmax) || fmax.ValueKind != JsonValueKind.Object)
        {
            Console.Error.WriteLine($"error: {reportPath} has no fmax object");
            return 2;
        }

        var clocks = new Dictionary<string, (double Achieved, double Constraint)>(StringComparer.Ordinal);
        foreach (JsonProperty prop in fmax.EnumerateObject())
        {
            JsonElement value = prop.Value;
            if (value.ValueKind != JsonValueKind.Object)
                continue;
            clocks[prop.Name] = (
                value.GetProperty("achieved").GetDouble(),
                value.GetProperty("constraint").GetDouble());
        }

        if (clocks.Count == 0 && logPath is not null)
            clocks = LoadFmaxFromLog(logPath);
        if (clocks.Count == 0)
        {
            Console.Error.WriteLine($"error: {reportPath} has an empty fmax object" + (logPath is null ? "" : $" and {logPath} had no Max frequency lines"));
            return 2;
        }

        var failed = new List<(string Clock, double Achieved, double Required)>();
        foreach ((string clock, (double achieved, double constraint)) in clocks.OrderBy(kv => kv.Key, StringComparer.Ordinal))
        {
            double required = constraint + marginMhz;
            string status = achieved >= required ? "PASS" : "FAIL";
            Console.WriteLine($"{status}: {clock}: achieved {achieved:F2} MHz, required {required:F2} MHz");
            if (achieved < required)
                failed.Add((clock, achieved, required));
        }

        if (failed.Count == 0)
            return 0;
        Console.Error.WriteLine();
        Console.Error.WriteLine("Timing failed:");
        foreach ((string clock, double achieved, double required) in failed)
            Console.Error.WriteLine($"  {clock}: achieved {achieved:F2} MHz, required {required:F2} MHz");
        return 1;
    }

    private static Dictionary<string, (double Achieved, double Constraint)> LoadFmaxFromLog(string path)
    {
        Regex maxFrequency = new(@"Max frequency for clock\s+(.+?):\s+([0-9.]+)\s+MHz\s+\((PASS|FAIL) at\s+([0-9.]+)\s+MHz\)");
        var result = new Dictionary<string, (double, double)>(StringComparer.Ordinal);
        foreach (string line in File.ReadLines(path))
        {
            Match match = maxFrequency.Match(line);
            if (!match.Success)
                continue;
            string clock = match.Groups[1].Value.Trim().Trim('\'');
            result[clock] = (
                double.Parse(match.Groups[2].Value, CultureInfo.InvariantCulture),
                double.Parse(match.Groups[4].Value, CultureInfo.InvariantCulture));
        }
        return result;
    }

    public static int CheckBitstreamFreshness(string bitstreamPath, string repoRoot, bool includeRoms)
    {
        string metadataPath = Path.ChangeExtension(bitstreamPath, ".metadata.json");
        if (!File.Exists(metadataPath))
        {
            Console.Error.WriteLine($"error: bitstream metadata not found: {metadataPath}");
            return 2;
        }

        using JsonDocument doc = JsonDocument.Parse(File.ReadAllText(metadataPath));
        if (!doc.RootElement.TryGetProperty("source", out JsonElement source) || source.ValueKind != JsonValueKind.Object)
        {
            Console.Error.WriteLine($"error: no source object in metadata: {metadataPath}");
            return 2;
        }

        var inputs = new List<JsonElement>();
        if (source.TryGetProperty("rtl_inputs", out JsonElement rtlInputs) && rtlInputs.ValueKind == JsonValueKind.Array)
            inputs.AddRange(rtlInputs.EnumerateArray().Where(item => item.ValueKind == JsonValueKind.Object));
        if (includeRoms && source.TryGetProperty("rom_inputs", out JsonElement romInputs) && romInputs.ValueKind == JsonValueKind.Array)
            inputs.AddRange(romInputs.EnumerateArray().Where(item => item.ValueKind == JsonValueKind.Object));

        var mismatches = new List<string>();
        int checkedCount = 0;
        foreach (JsonElement input in inputs)
        {
            if (!input.TryGetProperty("path", out JsonElement pathElement)
                || !input.TryGetProperty("sha256", out JsonElement shaElement))
                continue;
            string? relative = pathElement.GetString();
            string? expected = shaElement.GetString();
            if (string.IsNullOrEmpty(relative) || string.IsNullOrEmpty(expected))
                continue;
            string path = Path.Combine(Path.GetFullPath(repoRoot), relative);
            if (!File.Exists(path))
            {
                mismatches.Add($"{relative}: missing");
                continue;
            }

            checkedCount++;
            string actual = Sha256(path);
            if (!string.Equals(actual, expected, StringComparison.OrdinalIgnoreCase))
                mismatches.Add($"{relative}: current {actual}, bitstream {expected}");
        }

        if (checkedCount == 0)
        {
            Console.Error.WriteLine($"error: no comparable RTL inputs in metadata: {metadataPath}");
            return 2;
        }

        if (mismatches.Count > 0)
        {
            Console.Error.WriteLine($"error: stale bitstream: {bitstreamPath}");
            Console.Error.WriteLine($"metadata: {metadataPath}");
            foreach (string mismatch in mismatches.Take(20))
                Console.Error.WriteLine($"  {mismatch}");
            if (mismatches.Count > 20)
                Console.Error.WriteLine($"  ... {mismatches.Count - 20} more mismatches");
            return 1;
        }

        Console.WriteLine($"PASS: bitstream metadata matches {checkedCount} current RTL input files");
        return 0;
    }

    private static string Sha256(string path)
    {
        using var stream = File.OpenRead(path);
        return Convert.ToHexString(SHA256.HashData(stream)).ToLowerInvariant();
    }

    private sealed record BookManifest(string Title, string Subtitle, List<BookSection> Chapters, List<BookSection> Appendices);
    private sealed record BookSection(string Title)
    {
        public List<string> Sources { get; } = [];
        public string? AutoGenerate { get; set; }
    }

    private static BookManifest ParseBookManifest(string path)
    {
        string title = "NovaBASIC User Guide";
        string subtitle = "";
        var chapters = new List<BookSection>();
        var appendices = new List<BookSection>();
        List<BookSection>? currentList = null;
        BookSection? current = null;

        foreach (string raw in File.ReadLines(path))
        {
            string line = raw.Trim();
            if (line.StartsWith("title:", StringComparison.Ordinal))
                title = UnquoteYaml(line["title:".Length..].Trim());
            else if (line.StartsWith("subtitle:", StringComparison.Ordinal))
                subtitle = UnquoteYaml(line["subtitle:".Length..].Trim());
            else if (line == "chapters:")
                currentList = chapters;
            else if (line == "appendices:")
                currentList = appendices;
            else if (line.StartsWith("- title:", StringComparison.Ordinal) && currentList is not null)
            {
                current = new BookSection(UnquoteYaml(line["- title:".Length..].Trim()));
                currentList.Add(current);
            }
            else if (line.StartsWith("sources:", StringComparison.Ordinal) && current is not null)
            {
                foreach (string source in ParseYamlInlineList(line["sources:".Length..].Trim()))
                    current.Sources.Add(source);
            }
            else if (line.StartsWith("auto_generate:", StringComparison.Ordinal) && current is not null)
            {
                current.AutoGenerate = UnquoteYaml(line["auto_generate:".Length..].Trim());
            }
        }

        return new BookManifest(title, subtitle, chapters, appendices);
    }

    private static string UnquoteYaml(string value)
    {
        value = value.Trim();
        if (value.Length >= 2 && value[0] == '"' && value[^1] == '"')
            return value[1..^1];
        return value;
    }

    private static List<string> ParseYamlInlineList(string value)
    {
        value = value.Trim();
        if (value.StartsWith("[", StringComparison.Ordinal) && value.EndsWith("]", StringComparison.Ordinal))
            value = value[1..^1];
        return value.Split(',', StringSplitOptions.RemoveEmptyEntries)
            .Select(item => UnquoteYaml(item.Trim()))
            .Where(item => item.Length > 0)
            .ToList();
    }

    private static string ReadMarkdownBodyOrPending(string path, string source) =>
        File.Exists(path) ? ReadMarkdownBody(path) : $"\n*Content pending: {source}*\n";

    private static string ReadMarkdownBody(string path)
    {
        string content = File.ReadAllText(path);
        if (!content.StartsWith("---", StringComparison.Ordinal))
            return content.Trim();
        int end = content.IndexOf("---", 3, StringComparison.Ordinal);
        return end >= 0 ? content[(end + 3)..].Trim() : content;
    }

    private static string ExtractMarkdownTitle(string path)
    {
        string content = File.ReadAllText(path);
        if (content.StartsWith("---", StringComparison.Ordinal))
        {
            int end = content.IndexOf("---", 3, StringComparison.Ordinal);
            if (end >= 0)
            {
                foreach (string line in content[3..end].Split('\n'))
                {
                    string trimmed = line.Trim();
                    if (trimmed.StartsWith("title:", StringComparison.Ordinal))
                        return UnquoteYaml(trimmed["title:".Length..].Trim());
                }
            }
        }
        return Path.GetFileNameWithoutExtension(path);
    }

    private sealed class DocEntry
    {
        public Dictionary<string, object?> Values { get; } = new();
        public string String(string key) => Values.TryGetValue(key, out object? value) ? Convert.ToString(value, CultureInfo.InvariantCulture) ?? "" : "";
        public List<object?> List(string key)
        {
            if (!Values.TryGetValue(key, out object? value) || value is not List<object?> list)
            {
                list = [];
                Values[key] = list;
            }
            return list;
        }
    }

    private static List<DocEntry> ParseTaggedSources(IEnumerable<string> paths)
    {
        Regex comment = new(@"^\s*;\s*@(\w+)(?:\s+(.*))?$");
        var entries = new List<DocEntry>();
        DocEntry? pending = null;

        foreach (string path in paths)
        {
            int lineNo = 0;
            foreach (string line in File.ReadLines(path))
            {
                lineNo++;
                Match match = comment.Match(line);
                if (match.Success)
                {
                    string key = match.Groups[1].Value.ToLowerInvariant();
                    string value = match.Groups[2].Value.Trim();
                    if (key == "label")
                    {
                        if (pending is not null)
                            entries.Add(pending);
                        pending = new DocEntry();
                        pending.Values["label"] = value;
                        pending.Values["source"] = path;
                        pending.Values["line"] = lineNo;
                        pending.Values["inputs"] = new List<object?>();
                        pending.Values["outputs"] = new List<object?>();
                        pending.Values["requires"] = new List<object?>();
                    }
                    else if (pending is not null)
                    {
                        if (key == "in")
                            pending.List("inputs").Add(ParseNamedText(value));
                        else if (key == "out")
                            pending.List("outputs").Add(ParseNamedText(value));
                        else if (key == "requires")
                            pending.List("requires").AddRange(SplitWords(value).Cast<object?>());
                        else
                            pending.Values[key] = value;
                    }
                    continue;
                }

                if (pending is not null && line.Trim().Length > 0 && !line.TrimStart().StartsWith(";", StringComparison.Ordinal))
                {
                    entries.Add(pending);
                    pending = null;
                }
            }
        }

        if (pending is not null)
            entries.Add(pending);
        return entries;
    }

    private static Dictionary<string, string> ParseNamedText(string value)
    {
        if (value.Contains(':', StringComparison.Ordinal))
        {
            string[] parts = value.Split(':', 2);
            return new Dictionary<string, string> { ["name"] = parts[0].Trim(), ["description"] = parts[1].Trim() };
        }
        string[] words = value.Split((char[]?)null, 2, StringSplitOptions.RemoveEmptyEntries);
        return words.Length == 2
            ? new Dictionary<string, string> { ["name"] = words[0], ["description"] = words[1] }
            : new Dictionary<string, string> { ["name"] = value, ["description"] = "" };
    }

    private static List<string> SplitWords(string value) =>
        Regex.Split(value, @"[,\s]+").Select(v => v.Trim()).Where(v => v.Length > 0).ToList();

    private static Dictionary<string, string> ParseEquates(IEnumerable<string> paths)
    {
        Regex equate = new(@"^\s*([A-Za-z_][\w]*)\s*=\s*([^;]+)");
        var result = new Dictionary<string, string>();
        foreach (string path in paths)
        {
            foreach (string line in File.ReadLines(path))
            {
                Match match = equate.Match(line);
                if (match.Success)
                    result[match.Groups[1].Value] = match.Groups[2].Value.Trim();
            }
        }
        return result;
    }

    private static Dictionary<string, int> ParseViceSymbols(string? path)
    {
        if (path is null || !File.Exists(path))
            return [];
        Regex sym = new(@"^al\s+([0-9A-Fa-f]+)\s+\.(\S+)");
        var result = new Dictionary<string, int>();
        foreach (string line in File.ReadLines(path))
        {
            Match match = sym.Match(line);
            if (match.Success)
                result[match.Groups[2].Value] = Convert.ToInt32(match.Groups[1].Value, 16);
        }
        return result;
    }

    private static int? ResolveExpr(string expr, Dictionary<string, string> equates, HashSet<string>? seen = null)
    {
        expr = expr.Trim();
        if (expr.Length == 0)
            return null;
        if (expr.StartsWith("$", StringComparison.Ordinal))
            return Convert.ToInt32(expr[1..], 16);
        if (expr.StartsWith("0x", StringComparison.OrdinalIgnoreCase))
            return Convert.ToInt32(expr, 16);
        if (int.TryParse(expr, NumberStyles.Integer, CultureInfo.InvariantCulture, out int value))
            return value;
        seen ??= [];
        if (!seen.Add(expr) || !equates.TryGetValue(expr, out string? nested))
            return null;
        return ResolveExpr(nested, equates, seen);
    }

    private static void WriteRuntimeMarkdown(List<DocEntry> entries, string outputPath)
    {
        var lines = new List<string>
        {
            "# Runtime Labels",
            "",
            "Generated from `@label` comments in the assembly runtime sources.",
            "",
        };

        foreach (DocEntry entry in entries)
        {
            lines.Add($"## {entry.String("label")}");
            string summary = entry.String("summary").Trim();
            if (summary.Length > 0)
            {
                lines.Add("");
                lines.Add(summary);
            }
            lines.Add("");
            lines.Add($"- Kind: `{entry.String("kind").DefaultIfEmpty("unknown")}`");
            if (entry.String("symbol").Length > 0) lines.Add($"- Symbol: `{entry.String("symbol")}`");
            if (entry.String("address").Length > 0) lines.Add($"- Address: `{entry.String("address")}`");
            if (entry.String("abi").Length > 0) lines.Add($"- ABI: `{entry.String("abi")}`");
            AddNamedMarkdown(lines, "Inputs", entry.Values.GetValueOrDefault("inputs"));
            AddNamedMarkdown(lines, "Outputs", entry.Values.GetValueOrDefault("outputs"));
            if (entry.Values.GetValueOrDefault("requires") is List<object?> requires && requires.Count > 0)
            {
                lines.Add("");
                lines.Add("Requires:");
                foreach (object? value in requires)
                    lines.Add($"- `{value}`");
            }
            lines.Add("");
        }

        WriteText(outputPath, string.Join("\n", lines).TrimEnd() + "\n");
    }

    private static void AddNamedMarkdown(List<string> lines, string title, object? value)
    {
        if (value is not List<object?> values || values.Count == 0)
            return;
        lines.Add("");
        lines.Add(title + ":");
        foreach (object? item in values)
        {
            if (item is Dictionary<string, string> dict)
                lines.Add($"- `{dict.GetValueOrDefault("name", "")}`: {dict.GetValueOrDefault("description", "")}");
        }
    }

    private static void WriteRuntimeAsm(List<DocEntry> entries, string outputPath)
    {
        const int BucketCount = 64;
        var labels = new List<(string Label, int Address)>();
        foreach (DocEntry entry in entries)
        {
            string label = entry.String("label").Trim().ToUpperInvariant();
            int? address = AddressToInt(entry.Values.GetValueOrDefault("address"));
            if (label.Length == 0 || address is null)
                continue;
            _ = Encoding.ASCII.GetBytes(label);
            if (label.Length > 255)
                throw new InvalidOperationException($"{label}: runtime label is too long");
            labels.Add((label, address.Value));
        }

        var buckets = Enumerable.Range(0, BucketCount).Select(_ => new List<(string Label, int Address)>()).ToList();
        foreach ((string label, int address) in labels)
            buckets[LabelHash(label) & (BucketCount - 1)].Add((label, address));

        var lines = new List<string>
        {
            "; Generated by nova codegen runtime-abi. Do not edit.",
            "; Runtime label hash table for ADDR(\"LABEL\") in the SYSTEM module.",
            "",
            $"RUNTIME_LABEL_BUCKET_COUNT = {BucketCount}",
            $"RUNTIME_LABEL_COUNT = {labels.Count}",
            "",
            "RuntimeLabelBuckets:",
        };
        for (int i = 0; i < BucketCount; i++)
            lines.Add($"      .word @bucket_{i:X2}");
        for (int i = 0; i < BucketCount; i++)
        {
            lines.Add("");
            lines.Add($"@bucket_{i:X2}:");
            foreach ((string label, int address) in buckets[i].OrderBy(v => v.Label, StringComparer.Ordinal))
            {
                lines.Add($"      .byte {label.Length}");
                lines.Add($"      .word ${address:X4}");
                lines.Add($"      .byte {JsonQuote(label)}");
            }
            lines.Add("      .byte 0");
        }
        WriteText(outputPath, string.Join("\n", lines).TrimEnd() + "\n");
    }

    private static int? AddressToInt(object? value)
    {
        string text = Convert.ToString(value, CultureInfo.InvariantCulture)?.Trim() ?? "";
        if (text.StartsWith("$", StringComparison.Ordinal)) return Convert.ToInt32(text[1..], 16);
        if (text.StartsWith("0x", StringComparison.OrdinalIgnoreCase)) return Convert.ToInt32(text, 16);
        return int.TryParse(text, NumberStyles.Integer, CultureInfo.InvariantCulture, out int parsed) ? parsed : null;
    }

    private static int LabelHash(string label)
    {
        int hash = 0;
        foreach (byte ch in Encoding.ASCII.GetBytes(label.ToUpperInvariant()))
            hash = ((hash << 5) + hash + ch) & 0xFF;
        return hash;
    }

    private static readonly string[] LibraryOrder =
    [
        "nova", "fio", "audio", "vgc", "sprite", "msprite", "vsprite", "vtext", "nui",
        "copper", "dma", "blitter", "xram", "xmc", "pager", "rng", "nvg", "anim",
        "tween", "nic", "gameserver", "overlay", "math_copro", "math_fixed8", "strip", "vgc_wait",
    ];

    private static readonly Dictionary<string, string> LibraryTitles = new()
    {
        ["anim"] = "Animation",
        ["audio"] = "Audio and Music",
        ["blitter"] = "Blitter",
        ["copper"] = "Copper",
        ["dma"] = "DMA",
        ["fio"] = "File I/O",
        ["gameserver"] = "Game Server",
        ["math_copro"] = "Math Coprocessor",
        ["math_fixed8"] = "Fixed-Point Math",
        ["msprite"] = "Meta-Sprites",
        ["nic"] = "Network Interface",
        ["nova"] = "Nova Core",
        ["nui"] = "Nova UI",
        ["nvg"] = "Nova Graphics Images",
        ["overlay"] = "Overlays",
        ["pager"] = "File Pager",
        ["rng"] = "Random Numbers",
        ["sprite"] = "Sprites",
        ["strip"] = "Runtime Stripping",
        ["tween"] = "Tweening",
        ["vgc"] = "Video and Graphics Controller",
        ["vgc_wait"] = "VGC Wait Helpers",
        ["vsprite"] = "Vector Sprites",
        ["vtext"] = "Vector Text",
        ["xmc"] = "Expansion Memory Controller",
        ["xram"] = "Expansion RAM",
    };

    private static void ValidateNdkEntries(IEnumerable<DocEntry> entries)
    {
        var errors = new List<string>();
        foreach (DocEntry entry in entries)
        {
            string[] missing = ["label", "kind", "symbol", "summary"];
            var bad = missing.Where(k => entry.String(k).Trim().Length == 0).ToList();
            if (bad.Count > 0)
                errors.Add($"{entry.String("source")}:{entry.String("line")}: {entry.String("label")}: missing {string.Join(", ", bad)}");
        }
        if (errors.Count > 0)
            throw new InvalidOperationException("Invalid NDK API docs:\n" + string.Join("\n", errors));
    }

    private static void EnrichNdkValues(IEnumerable<DocEntry> entries, Dictionary<string, string> equates)
    {
        foreach (DocEntry entry in entries)
        {
            string symbol = entry.String("symbol").Trim();
            string kind = entry.String("kind").Trim();
            if (kind == "routine" || symbol.Length == 0 || !equates.TryGetValue(symbol, out string? value))
                continue;
            entry.Values["value"] = value;
            int? resolved = ResolveExpr(symbol, equates);
            if (resolved is not null)
                entry.Values["resolved"] = resolved > 0xFF ? $"${resolved:X4}" : $"${resolved:X2}";
        }
    }

    private static List<Dictionary<string, object?>> MakeNdkLibraries(List<DocEntry> entries)
    {
        return entries.GroupBy(LibraryName)
            .OrderBy(group => LibrarySortKey(group.Key))
            .Select(group =>
            {
                List<Dictionary<string, object?>> groupEntries = group.OrderBy(e => e.String("label"), StringComparer.Ordinal)
                                                                      .Select(e => e.Values)
                                                                      .ToList();
                return new Dictionary<string, object?>
                {
                    ["name"] = group.Key,
                    ["title"] = LibraryTitles.GetValueOrDefault(group.Key, CultureInfo.InvariantCulture.TextInfo.ToTitleCase(group.Key.Replace("_", " "))),
                    ["sources"] = group.Select(e => Path.GetFileName(e.String("source"))).Distinct(StringComparer.Ordinal).Order(StringComparer.Ordinal).ToList(),
                    ["entries"] = groupEntries,
                };
            })
            .ToList();
    }

    private static string LibraryName(DocEntry entry)
    {
        string explicitName = entry.String("library").Trim().ToLowerInvariant();
        return explicitName.Length > 0 ? explicitName : Path.GetFileNameWithoutExtension(entry.String("source"));
    }

    private static (int, string) LibrarySortKey(string name)
    {
        int index = Array.IndexOf(LibraryOrder, name);
        return (index >= 0 ? index : LibraryOrder.Length, name);
    }

    private static void WriteNdkTex(List<Dictionary<string, object?>> libraries, string outputPath)
    {
        var lines = new List<string>
        {
            "% Generated by nova codegen ndk-reference. Do not edit.",
            @"\chapter{Runtime Library Reference}",
            "",
            "This chapter is generated from structured comments in the Nova runtime",
            "assembly sources. It lists the callable routines, constants,",
            "pseudo-registers, and shared data blocks exposed to assembly programs.",
            "",
        };

        foreach (Dictionary<string, object?> library in libraries)
        {
            var entries = (List<Dictionary<string, object?>>)library["entries"]!;
            if (entries.Count == 0)
                continue;
            var sources = (List<string>)library["sources"]!;
            string sourceText = string.Join(", ", sources.Select(TexCode));
            lines.Add($@"\section{{{TexEscape(library["title"]!)}}}");
            lines.Add($@"\noindent\textit{{Generated from {sourceText}.}}");
            lines.Add("");

            foreach (Dictionary<string, object?> entry in entries)
            {
                lines.Add($@"\begin{{ndkapibox}}{{{TexEscape(Value(entry, "label"))}}}{{{TexEscape(Value(entry, "kind"))}}}");
                lines.Add($@"\textbf{{Symbol:}} {TexCode(Value(entry, "symbol"))}\\");
                lines.Add($@"\textbf{{Summary:}} {TexEscape(Value(entry, "summary"))}");
                if (Value(entry, "abi").Length > 0) lines.Add($@"\par\textbf{{ABI:}} {TexEscape(Value(entry, "abi"))}");
                if (Value(entry, "value").Length > 0)
                {
                    string resolved = Value(entry, "resolved");
                    lines.Add(resolved.Length > 0
                        ? $@"\par\textbf{{Value:}} {TexCode(Value(entry, "value"))} ({TexCode(resolved)})"
                        : $@"\par\textbf{{Value:}} {TexCode(Value(entry, "value"))}");
                }
                AddTexNamedList(lines, "Inputs", entry.GetValueOrDefault("inputs"));
                AddTexNamedList(lines, "Outputs", entry.GetValueOrDefault("outputs"));
                if (entry.GetValueOrDefault("requires") is List<object?> requires && requires.Count > 0)
                {
                    lines.Add(@"\par\textbf{Requires:}");
                    lines.Add(string.Join(", ", requires.Select(v => TexCode(Convert.ToString(v, CultureInfo.InvariantCulture) ?? ""))));
                }
                lines.Add($@"\par\textbf{{Source:}} {TexCode(Path.GetFileName(Value(entry, "source")))} line {TexEscape(Value(entry, "line"))}");
                lines.Add(@"\end{ndkapibox}");
                lines.Add("");
            }
        }

        WriteText(outputPath, string.Join("\n", lines).TrimEnd() + "\n");
    }

    private static void AddTexNamedList(List<string> lines, string title, object? value)
    {
        if (value is not List<object?> values || values.Count == 0)
            return;
        lines.Add($@"\par\textbf{{{title}:}}");
        lines.Add(@"\begin{itemize}");
        foreach (object? item in values)
        {
            if (item is Dictionary<string, string> dict)
                lines.Add($@"\item {TexCode(dict.GetValueOrDefault("name", ""))}: {TexEscape(dict.GetValueOrDefault("description", ""))}");
        }
        lines.Add(@"\end{itemize}");
    }

    public static void PackNmod(string srcPath, string binPath, string outPath, IEnumerable<string> symPaths, IEnumerable<string> ndkDirs)
    {
        string source = File.ReadAllText(srcPath);
        byte[] image = File.ReadAllBytes(binPath);
        Dictionary<string, int> symbols = [];
        foreach (string symPath in symPaths)
            foreach ((string key, int value) in LoadNmodSymbols(File.ReadAllText(symPath)))
                symbols[key] = value;

        Dictionary<string, DocEntry>? ndkIndex = null;
        var ndkDirList = ndkDirs.ToList();
        if (ndkDirList.Count > 0)
        {
            List<string> ndkSources = [];
            foreach (string dir in ndkDirList)
            {
                ndkSources.AddRange(Directory.GetFiles(dir, "*.inc").Order(StringComparer.Ordinal));
                ndkSources.AddRange(Directory.GetFiles(dir, "*.s").Order(StringComparer.Ordinal));
            }
            ndkIndex = new Dictionary<string, DocEntry>(StringComparer.Ordinal);
            foreach (DocEntry entry in ParseTaggedSources(ndkSources).Where(e => e.String("symbol").Length > 0))
                ndkIndex[entry.String("symbol")] = entry;
        }

        byte[] packed = BuildNmod(source, image, symbols, ndkIndex);
        WriteBytes(outPath, packed);
    }

    private static Dictionary<string, object?> ParseNmodAnnotations(string text)
    {
        var module = new Dictionary<string, object?> { ["name"] = null, ["version"] = null, ["brief"] = null, ["id"] = null };
        var functions = new List<Dictionary<string, object?>>();
        Dictionary<string, object?> current = module;
        foreach (string raw in text.Split('\n'))
        {
            string line = raw.Trim();
            if (!line.StartsWith(";@", StringComparison.Ordinal))
                continue;
            string body = line[2..].Trim();
            if (body.Length == 0)
                continue;
            string[] parts = body.Split((char[]?)null, 2, StringSplitOptions.RemoveEmptyEntries);
            string tag = parts[0];
            string rest = parts.Length > 1 ? parts[1].Trim() : "";
            switch (tag)
            {
                case "module":
                    module["name"] = rest;
                    current = module;
                    break;
                case "version":
                    module["version"] = rest;
                    break;
                case "brief":
                    current["brief"] = rest;
                    break;
                case "fn":
                    string[] fp = rest.Split((char[]?)null, 2, StringSplitOptions.RemoveEmptyEntries);
                    current = new Dictionary<string, object?>
                    {
                        ["name"] = fp[0],
                        ["idspec"] = fp.Length > 1 ? fp[1].Trim() : fp[0],
                        ["brief"] = null,
                        ["ndk"] = null,
                        ["args"] = new List<Dictionary<string, string>>(),
                        ["ret"] = null,
                        ["effect"] = null,
                        ["status"] = new List<string>(),
                    };
                    functions.Add(current);
                    break;
                case "ndk":
                    current["ndk"] = rest.Length > 0 ? rest.Split((char[]?)null, 2, StringSplitOptions.RemoveEmptyEntries)[0] : null;
                    break;
                case "arg":
                    string[] ap = rest.Split((char[]?)null, 3, StringSplitOptions.RemoveEmptyEntries);
                    ((List<Dictionary<string, string>>)current["args"]!).Add(new Dictionary<string, string>
                    {
                        ["name"] = ap.Length > 0 ? ap[0] : "",
                        ["type"] = ap.Length > 1 ? ap[1] : "",
                        ["desc"] = ap.Length > 2 ? ap[2] : "",
                    });
                    break;
                case "ret":
                    string[] rp = rest.Split((char[]?)null, 2, StringSplitOptions.RemoveEmptyEntries);
                    current["ret"] = new Dictionary<string, string> { ["type"] = rp.Length > 0 ? rp[0] : "void", ["desc"] = rp.Length > 1 ? rp[1] : "" };
                    break;
                case "effect":
                    current["effect"] = rest;
                    break;
                case "status":
                    current["status"] = rest.Split(',').Select(s => s.Trim()).Where(s => s.Length > 0).ToList();
                    break;
            }
        }
        return new Dictionary<string, object?> { ["module"] = module, ["functions"] = functions };
    }

    private static Dictionary<string, int> LoadNmodSymbols(string text)
    {
        Regex sym = new(@"^\s*([A-Za-z_][A-Za-z0-9_]*)\s*=\s*(\$[0-9A-Fa-f]+|0[xX][0-9A-Fa-f]+|\d+)\b");
        var result = new Dictionary<string, int>();
        foreach (string line in text.Split('\n'))
        {
            Match match = sym.Match(line);
            if (!match.Success)
                continue;
            result[match.Groups[1].Value] = ParseInt(match.Groups[2].Value);
        }
        return result;
    }

    private static byte[] BuildNmod(string source, byte[] image, Dictionary<string, int> symbols, Dictionary<string, DocEntry>? ndkIndex)
    {
        const int ModuleSize = 16384;
        if (image.Length != ModuleSize)
            throw new InvalidOperationException($"module image must be {ModuleSize} bytes, got {image.Length}");
        if (image[3] != 0x4E || image[4] != 0x4C)
            throw new InvalidOperationException("bad module magic at +3");

        var parsed = ParseNmodAnnotations(source);
        var header = new Dictionary<string, int> { ["id"] = image[5], ["abiVersion"] = image[6], ["fnCount"] = image[7] };
        if (ndkIndex is not null)
            VerifyWrapperCalls(source, parsed, symbols, ParseJtable(source));
        Dictionary<string, object?> doc = BuildNmodDoc(parsed, header, symbols, ndkIndex);
        byte[] payload = WriteJsonToBytes(doc, indented: false);
        byte[] outBytes = new byte[ModuleSize + 8 + payload.Length];
        Buffer.BlockCopy(image, 0, outBytes, 0, ModuleSize);
        Encoding.ASCII.GetBytes("NDOC").CopyTo(outBytes, ModuleSize);
        BitConverter.GetBytes(payload.Length).CopyTo(outBytes, ModuleSize + 4);
        Buffer.BlockCopy(payload, 0, outBytes, ModuleSize + 8, payload.Length);
        return outBytes;
    }

    private static Dictionary<string, object?> BuildNmodDoc(Dictionary<string, object?> parsed, Dictionary<string, int> header, Dictionary<string, int> symbols, Dictionary<string, DocEntry>? ndkIndex)
    {
        var parsedModule = (Dictionary<string, object?>)parsed["module"]!;
        var module = new Dictionary<string, object?>
        {
            ["name"] = parsedModule["name"],
            ["id"] = header["id"],
            ["abiVersion"] = header["abiVersion"],
            ["version"] = parsedModule["version"],
            ["brief"] = parsedModule["brief"],
            ["abiNote"] = "All calls clobber A/X/Y; communicate via the mailbox (LIB_ARG*/LIB_RESULT/LIB_STATUS).",
        };

        var functions = new List<Dictionary<string, object?>>();
        var seen = new HashSet<int>();
        int span = header["fnCount"];
        foreach (Dictionary<string, object?> f in (List<Dictionary<string, object?>>)parsed["functions"]!)
        {
            string name = Convert.ToString(f["name"], CultureInfo.InvariantCulture)!;
            int id = ResolveNmodId(Convert.ToString(f["idspec"], CultureInfo.InvariantCulture)!, symbols);
            if (id < 0 || id >= span)
                throw new InvalidOperationException($"function {name} id {id} (${id:X2}) out of dispatch range [0, {span})");
            if (!seen.Add(id))
                throw new InvalidOperationException($"duplicate function id {id} (${id:X2}) at {name}");

            string? ndk = Convert.ToString(f["ndk"], CultureInfo.InvariantCulture);
            object? brief = f["brief"];
            if (!string.IsNullOrEmpty(ndk) && ndkIndex is not null)
            {
                if (!ndkIndex.TryGetValue(ndk, out DocEntry? ndkEntry))
                    throw new InvalidOperationException($"function {name} maps to unknown NDK symbol {ndk}");
                if (brief is null || Convert.ToString(brief, CultureInfo.InvariantCulture)?.Length == 0)
                    brief = ndkEntry.Values.GetValueOrDefault("summary");
            }

            var entry = new Dictionary<string, object?>
            {
                ["id"] = id,
                ["idHex"] = $"${id:X2}",
                ["name"] = name,
                ["brief"] = brief,
                ["args"] = ((List<Dictionary<string, string>>)f["args"]!)
                    .Select((a, i) => new Dictionary<string, object?>
                    {
                        ["i"] = i,
                        ["name"] = a["name"],
                        ["type"] = a["type"],
                        ["desc"] = a["desc"],
                    })
                    .ToList(),
                ["ret"] = f["ret"] ?? new Dictionary<string, string> { ["type"] = "void", ["desc"] = "" },
            };
            if (!string.IsNullOrEmpty(ndk)) entry["ndk"] = ndk;
            if (f["effect"] is not null) entry["effect"] = f["effect"];
            if (f["status"] is List<string> status && status.Count > 0) entry["status"] = status;
            functions.Add(entry);
        }

        return new Dictionary<string, object?> { ["ndocVersion"] = 1, ["module"] = module, ["functions"] = functions };
    }

    private static int ResolveNmodId(string idspec, Dictionary<string, int> symbols)
    {
        string value = idspec.Trim();
        if (value.StartsWith("$", StringComparison.Ordinal)) return Convert.ToInt32(value[1..], 16);
        if (value.StartsWith("0x", StringComparison.OrdinalIgnoreCase)) return Convert.ToInt32(value, 16);
        if (int.TryParse(value, NumberStyles.Integer, CultureInfo.InvariantCulture, out int dec)) return dec;
        if (symbols.TryGetValue(value, out int symbol)) return symbol;
        throw new InvalidOperationException($"unknown id spec / symbol: {idspec}");
    }

    private static List<string> ParseJtable(string source)
    {
        Regex jtable = new(@"^\s*\.word\s+([A-Za-z_]\w*)\s*-\s*1\b");
        var runs = new List<List<string>>();
        var current = new List<string>();
        foreach (string line in source.Split('\n'))
        {
            Match match = jtable.Match(line);
            if (match.Success)
            {
                current.Add(match.Groups[1].Value);
                continue;
            }
            string stripped = line.Trim();
            if (stripped.Length == 0 || stripped.StartsWith(";", StringComparison.Ordinal))
                continue;
            if (current.Count > 0)
            {
                runs.Add(current);
                current = [];
            }
        }
        if (current.Count > 0)
            runs.Add(current);
        return runs.OrderByDescending(r => r.Count).FirstOrDefault() ?? [];
    }

    private static void VerifyWrapperCalls(string source, Dictionary<string, object?> parsed, Dictionary<string, int> symbols, List<string> jtableLabels)
    {
        string[] lines = source.Split('\n');
        Regex label = new(@"^([A-Za-z_]\w*):");
        var labelLine = new Dictionary<string, int>();
        for (int i = 0; i < lines.Length; i++)
        {
            Match match = label.Match(lines[i]);
            if (match.Success)
                labelLine.TryAdd(match.Groups[1].Value, i);
        }

        foreach (Dictionary<string, object?> f in (List<Dictionary<string, object?>>)parsed["functions"]!)
        {
            string sym = Convert.ToString(f["ndk"], CultureInfo.InvariantCulture) ?? "";
            if (sym.Length == 0)
                continue;
            string name = Convert.ToString(f["name"], CultureInfo.InvariantCulture) ?? "";
            int id = ResolveNmodId(Convert.ToString(f["idspec"], CultureInfo.InvariantCulture)!, symbols);
            if (id >= jtableLabels.Count)
                throw new InvalidOperationException($"function {name} id {id} has no jtable entry");
            string wrapper = jtableLabels[id];
            if (!labelLine.TryGetValue(wrapper, out int start))
                throw new InvalidOperationException($"wrapper label {wrapper} for function {name} not found");
            int end = lines.Length;
            for (int i = start + 1; i < lines.Length; i++)
            {
                if (label.IsMatch(lines[i]))
                {
                    end = i;
                    break;
                }
            }
            string body = string.Join("\n", lines[start..end]);
            if (!Regex.IsMatch(body, @"\bjsr\s+" + Regex.Escape(sym) + @"\b", RegexOptions.IgnoreCase))
                throw new InvalidOperationException($"wrapper {wrapper} (function {name}) does not JSR its mapped NDK routine {sym}");
        }
    }

    private static string StripAsmComment(string value)
    {
        bool inQuote = false;
        for (int i = 0; i < value.Length; i++)
        {
            if (value[i] == '"')
                inQuote = !inQuote;
            else if (value[i] == ';' && !inQuote)
                return value[..i].Trim();
        }
        return value.Trim();
    }

    private static List<string> SplitAsmArgs(string value)
    {
        var result = new List<string>();
        var current = new StringBuilder();
        bool inQuote = false;
        foreach (char ch in value)
        {
            if (ch == '"')
            {
                inQuote = !inQuote;
                current.Append(ch);
            }
            else if (ch == ',' && !inQuote)
            {
                result.Add(current.ToString().Trim());
                current.Clear();
            }
            else
            {
                current.Append(ch);
            }
        }
        if (current.Length > 0)
            result.Add(current.ToString().Trim());
        return result;
    }

    private static int ParseInt(string value)
    {
        value = value.Trim();
        if (value.StartsWith("$", StringComparison.Ordinal)) return Convert.ToInt32(value[1..], 16);
        if (value.StartsWith("0x", StringComparison.OrdinalIgnoreCase)) return Convert.ToInt32(value, 16);
        return int.Parse(value, CultureInfo.InvariantCulture);
    }

    private static string Value(Dictionary<string, object?> values, string key) =>
        values.TryGetValue(key, out object? value) ? Convert.ToString(value, CultureInfo.InvariantCulture) ?? "" : "";

    private static string TexEscape(object value)
    {
        string text = Convert.ToString(value, CultureInfo.InvariantCulture) ?? "";
        var sb = new StringBuilder();
        foreach (char ch in text)
        {
            sb.Append(ch switch
            {
                '\\' => @"\textbackslash{}",
                '&' => @"\&",
                '%' => @"\%",
                '$' => @"\$",
                '#' => @"\#",
                '_' => @"\_",
                '{' => @"\{",
                '}' => @"\}",
                '~' => @"\textasciitilde{}",
                '^' => @"\textasciicircum{}",
                _ => ch.ToString(),
            });
        }
        return sb.ToString();
    }

    private static string TexCode(object value) => $@"\texttt{{{TexEscape(value)}}}";

    private static void WriteJson(string path, object value) =>
        WriteBytes(path, WriteJsonToBytes(value, indented: true).Concat([(byte)'\n']).ToArray());

    private static byte[] WriteJsonToBytes(object? value, bool indented)
    {
        using var stream = new MemoryStream();
        using (var writer = new Utf8JsonWriter(stream, new JsonWriterOptions
        {
            Indented = indented,
            Encoder = JavaScriptEncoder.UnsafeRelaxedJsonEscaping,
        }))
            WriteJsonValue(writer, value);
        return Encoding.UTF8.GetBytes(EscapeNonAscii(Encoding.UTF8.GetString(stream.ToArray())));
    }

    private static string JsonQuote(string value) =>
        Encoding.UTF8.GetString(WriteJsonToBytes(value, indented: false));

    private static string EscapeNonAscii(string value)
    {
        var sb = new StringBuilder(value.Length);
        foreach (char ch in value)
        {
            if (ch <= 0x7F)
                sb.Append(ch);
            else
                sb.Append("\\u").Append(((int)ch).ToString("x4", CultureInfo.InvariantCulture));
        }
        return sb.ToString();
    }

    private static void WriteJsonValue(Utf8JsonWriter writer, object? value)
    {
        switch (value)
        {
            case null:
                writer.WriteNullValue();
                break;
            case string text:
                writer.WriteStringValue(text);
                break;
            case int number:
                writer.WriteNumberValue(number);
                break;
            case long number:
                writer.WriteNumberValue(number);
                break;
            case byte number:
                writer.WriteNumberValue(number);
                break;
            case bool boolean:
                writer.WriteBooleanValue(boolean);
                break;
            case IDictionary dictionary:
                writer.WriteStartObject();
                foreach (DictionaryEntry item in dictionary)
                {
                    writer.WritePropertyName(Convert.ToString(item.Key, CultureInfo.InvariantCulture) ?? "");
                    WriteJsonValue(writer, item.Value);
                }
                writer.WriteEndObject();
                break;
            case IEnumerable enumerable:
                writer.WriteStartArray();
                foreach (object? item in enumerable)
                    WriteJsonValue(writer, item);
                writer.WriteEndArray();
                break;
            default:
                writer.WriteStringValue(Convert.ToString(value, CultureInfo.InvariantCulture));
                break;
        }
    }

    private static void WriteText(string path, string text)
    {
        string? directory = Path.GetDirectoryName(Path.GetFullPath(path));
        if (!string.IsNullOrEmpty(directory))
            Directory.CreateDirectory(directory);
        File.WriteAllText(path, text);
    }

    private static void WriteBytes(string path, byte[] bytes)
    {
        string? directory = Path.GetDirectoryName(Path.GetFullPath(path));
        if (!string.IsNullOrEmpty(directory))
            Directory.CreateDirectory(directory);
        File.WriteAllBytes(path, bytes);
    }
}

internal static class StringExtensions
{
    public static string DefaultIfEmpty(this string value, string fallback) =>
        string.IsNullOrEmpty(value) ? fallback : value;
}

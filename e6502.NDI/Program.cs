using System.Text;
using e6502.Storage;

if (args.Length < 1)
{
    PrintUsage();
    return 1;
}

string verb = args[0].ToLowerInvariant();
return verb switch
{
    "create"     => DoCreate(args[1..]),
    "dir"        => DoDir(args[1..]),
    "info"       => DoInfo(args[1..]),
    "validate"   => DoValidate(args[1..]),
    "label"      => DoLabel(args[1..]),
    "import"     => DoImport(args[1..]),
    "export"     => DoExport(args[1..]),
    "delete"     => DoDelete(args[1..]),
    "mkdir"      => DoMkdir(args[1..]),
    "rmdir"      => DoRmdir(args[1..]),
    "tokenize"   => DoTokenize(args[1..]),
    "detokenize" => DoDetokenize(args[1..]),
    _            => UnknownVerb(verb),
};

// ---------------------------------------------------------------------------
// create <file.ndi> [--size <KB>] [--label <name>]
// ---------------------------------------------------------------------------
static int DoCreate(string[] args)
{
    if (args.Length < 1)
    {
        Console.Error.WriteLine("Usage: ndi create <file.ndi> [--size <KB>] [--label <name>]");
        return 1;
    }

    string path = args[0];
    int sizeKB = 800;
    string label = "DISK";

    for (int i = 1; i < args.Length; i++)
    {
        switch (args[i])
        {
            case "--size" when i + 1 < args.Length:
                if (!int.TryParse(args[++i], out sizeKB) || sizeKB < 1)
                {
                    Console.Error.WriteLine("--size must be a positive integer (KB)");
                    return 1;
                }
                break;
            case "--label" when i + 1 < args.Length:
                label = args[++i];
                break;
        }
    }

    try
    {
        NdiImage.CreateFormatted(path, label, sizeKB);
    }
    catch (Exception ex)
    {
        Console.Error.WriteLine($"Error: {ex.Message}");
        return 1;
    }

    using var img = NdiImage.Open(path);
    var h = img.Header;
    Console.WriteLine($"Created {path}");
    Console.WriteLine($"  Label:        {h.VolumeLabel}");
    Console.WriteLine($"  Size:         {sizeKB} KB  ({h.TotalSectors} sectors)");
    Console.WriteLine($"  Free sectors: {img.FreeSectors}");
    return 0;
}

// ---------------------------------------------------------------------------
// dir <file.ndi> [/path]
// ---------------------------------------------------------------------------
static int DoDir(string[] args)
{
    if (args.Length < 1)
    {
        Console.Error.WriteLine("Usage: ndi dir <file.ndi> [/path]");
        return 1;
    }

    string imagePath = args[0];
    string ndiPath = args.Length > 1 ? args[1] : "/";

    NdiImage img;
    try { img = NdiImage.Open(imagePath); }
    catch (Exception ex) { Console.Error.WriteLine($"Error: {ex.Message}"); return 1; }

    using (img)
    {
        ushort parentIndex;
        try { parentIndex = ResolvePath(img, ndiPath); }
        catch (Exception ex) { Console.Error.WriteLine($"Error: {ex.Message}"); return 1; }

        var entries = img.ListDirectory(parentIndex);

        Console.WriteLine($"Directory of {imagePath}:{ndiPath}");
        Console.WriteLine();

        if (entries.Length == 0)
        {
            Console.WriteLine("  (empty)");
        }
        else
        {
            Console.WriteLine($"  {"Name",-32}  {"Type",-4}  {"Size",8}");
            Console.WriteLine($"  {new string('-', 32)}  {new string('-', 4)}  {new string('-', 8)}");
            foreach (var e in entries)
            {
                string typeStr = e.IsDirectory ? "DIR" : e.FileType.ToString().ToUpperInvariant();
                string sizeStr = e.IsDirectory ? "<DIR>" : e.SizeBytes.ToString();
                Console.WriteLine($"  {e.Filename,-32}  {typeStr,-4}  {sizeStr,8}");
            }
        }

        Console.WriteLine();
        Console.WriteLine($"  {img.FreeSectors} sectors free  ({img.FreeSectors * 256 / 1024} KB)");
    }
    return 0;
}

// ---------------------------------------------------------------------------
// info <file.ndi>
// ---------------------------------------------------------------------------
static int DoInfo(string[] args)
{
    if (args.Length < 1)
    {
        Console.Error.WriteLine("Usage: ndi info <file.ndi>");
        return 1;
    }

    NdiImage img;
    try { img = NdiImage.Open(args[0]); }
    catch (Exception ex) { Console.Error.WriteLine($"Error: {ex.Message}"); return 1; }

    using (img)
    {
        var h = img.Header;
        int totalKB = h.TotalSectors * 256 / 1024;
        int freeKB = img.FreeSectors * 256 / 1024;
        int dirCapacity = h.DirectorySectorCount * NdiDirectory.EntriesPerSector;

        Console.WriteLine($"Image:            {args[0]}");
        Console.WriteLine($"Format version:   {h.FormatVersion}");
        Console.WriteLine($"Volume label:     {h.VolumeLabel}");
        Console.WriteLine($"Sector size:      {h.SectorSize} bytes");
        Console.WriteLine($"Total sectors:    {h.TotalSectors}  ({totalKB} KB)");
        Console.WriteLine($"Free sectors:     {img.FreeSectors}  ({freeKB} KB)");
        Console.WriteLine($"Dir start:        sector {h.DirectoryStartSector}");
        Console.WriteLine($"Dir sectors:      {h.DirectorySectorCount}");
        Console.WriteLine($"Data start:       sector {h.DataStartSector}");
        Console.WriteLine($"Dir capacity:     {dirCapacity} entries");
    }
    return 0;
}

// ---------------------------------------------------------------------------
// validate <file.ndi>
// ---------------------------------------------------------------------------
static int DoValidate(string[] args)
{
    if (args.Length < 1)
    {
        Console.Error.WriteLine("Usage: ndi validate <file.ndi>");
        return 1;
    }

    NdiImage img;
    try { img = NdiImage.Open(args[0]); }
    catch (Exception ex) { Console.Error.WriteLine($"Error: {ex.Message}"); return 1; }

    using (img)
    {
        var h = img.Header;
        int dataSectorCount = h.TotalSectors - h.DataStartSector;

        var entries = CollectAllEntries(img);
        var claimed = new bool[dataSectorCount];
        var errors = new List<string>();

        foreach (var e in entries)
        {
            if (e.IsDirectory) continue;

            int start = e.StartSector;
            int count = e.SectorCount;

            if (start + count > dataSectorCount)
            {
                errors.Add($"  [{e.Index}] '{e.Filename}': sectors {start}..{start + count - 1} extend beyond image bounds");
                continue;
            }

            for (int s = start; s < start + count; s++)
            {
                if (claimed[s])
                    errors.Add($"  [{e.Index}] '{e.Filename}': sector {s} is double-allocated");
                claimed[s] = true;
            }
        }

        int dirAllocated = claimed.Count(c => c);
        int bamFree = img.FreeSectors;
        int bamAllocated = dataSectorCount - bamFree;

        if (bamAllocated != dirAllocated)
            errors.Add($"  BAM says {bamAllocated} sectors used, but directory accounts for {dirAllocated}");

        if (errors.Count == 0)
        {
            Console.WriteLine("OK — no errors found.");
            Console.WriteLine($"  {entries.Length} directory entries, {dirAllocated} sectors used, {bamFree} sectors free.");
        }
        else
        {
            Console.WriteLine($"Found {errors.Count} error(s):");
            foreach (string err in errors)
                Console.WriteLine(err);
            return 1;
        }
    }
    return 0;
}

// ---------------------------------------------------------------------------
// label <file.ndi> <name>
// ---------------------------------------------------------------------------
static int DoLabel(string[] args)
{
    if (args.Length < 2)
    {
        Console.Error.WriteLine("Usage: ndi label <file.ndi> <name>");
        return 1;
    }

    string imagePath = args[0];
    string newLabel = args[1];
    if (newLabel.Length > 32)
    {
        Console.Error.WriteLine("Label must be 32 characters or fewer.");
        return 1;
    }

    try
    {
        using var fs = new FileStream(imagePath, FileMode.Open, FileAccess.ReadWrite, FileShare.None);

        var headerBuf = new byte[256];
        fs.Seek(0, SeekOrigin.Begin);
        fs.ReadExactly(headerBuf);

        if (headerBuf[0] != (byte)'N' || headerBuf[1] != (byte)'D' ||
            headerBuf[2] != (byte)'I' || headerBuf[3] != 0x1A)
        {
            Console.Error.WriteLine("Not a valid NDI image.");
            return 1;
        }

        Array.Clear(headerBuf, 9, 32);
        var labelBytes = Encoding.ASCII.GetBytes(newLabel);
        Array.Copy(labelBytes, 0, headerBuf, 9, Math.Min(labelBytes.Length, 32));

        fs.Seek(0, SeekOrigin.Begin);
        fs.Write(headerBuf);
    }
    catch (Exception ex)
    {
        Console.Error.WriteLine($"Error: {ex.Message}");
        return 1;
    }

    Console.WriteLine($"Label set to: {newLabel}");
    return 0;
}

// ---------------------------------------------------------------------------
// import <file.ndi> <hostfile> [/dest] [--tokenize] [--tokens <path>]
// ---------------------------------------------------------------------------
static int DoImport(string[] args)
{
    if (args.Length < 2)
    {
        Console.Error.WriteLine("Usage: ndi import <file.ndi> <hostfile> [/dest] [--tokenize] [--tokens <path>]");
        return 1;
    }

    string imagePath = args[0];
    string hostFile = args[1];
    string destPath = "/";
    bool doTokenize = false;
    string? tokensPath = null;

    for (int i = 2; i < args.Length; i++)
    {
        switch (args[i])
        {
            case "--tokenize":
                doTokenize = true;
                break;
            case "--tokens" when i + 1 < args.Length:
                tokensPath = args[++i];
                break;
            default:
                if (args[i].StartsWith('/'))
                    destPath = args[i];
                break;
        }
    }

    if (!File.Exists(hostFile))
    {
        Console.Error.WriteLine($"Host file not found: {hostFile}");
        return 1;
    }

    string filename = Path.GetFileName(hostFile);
    NdiFileType fileType = ExtensionToFileType(Path.GetExtension(hostFile));

    byte[] data;
    try { data = File.ReadAllBytes(hostFile); }
    catch (Exception ex) { Console.Error.WriteLine($"Error reading host file: {ex.Message}"); return 1; }

    if (doTokenize)
    {
        tokensPath ??= FindTokensJson();
        if (tokensPath is null)
        {
            Console.Error.WriteLine("tokens.json not found. Use --tokens <path>.");
            return 1;
        }
        try
        {
            var tokenizer = BasicTokenizer.FromJsonFile(tokensPath);
            string[] lines = Encoding.ASCII.GetString(data).ReplaceLineEndings("\n")
                .Split('\n', StringSplitOptions.RemoveEmptyEntries);
            data = tokenizer.Tokenize(lines, 0x0401);
            fileType = NdiFileType.Bas;
        }
        catch (Exception ex)
        {
            Console.Error.WriteLine($"Tokenize error: {ex.Message}");
            return 1;
        }
    }

    NdiImage img;
    try { img = NdiImage.Open(imagePath); }
    catch (Exception ex) { Console.Error.WriteLine($"Error: {ex.Message}"); return 1; }

    using (img)
    {
        ushort parentIndex;
        try { parentIndex = ResolvePath(img, destPath); }
        catch (Exception ex) { Console.Error.WriteLine($"Error: {ex.Message}"); return 1; }

        try { img.WriteFile(filename, fileType, parentIndex, data); }
        catch (Exception ex) { Console.Error.WriteLine($"Error writing file: {ex.Message}"); return 1; }
    }

    Console.WriteLine($"Imported {hostFile} -> {destPath.TrimEnd('/')}/{filename}  ({data.Length} bytes, type={fileType})");
    return 0;
}

// ---------------------------------------------------------------------------
// export <file.ndi> <path> [hostdir] [--detokenize] [--tokens <path>]
// ---------------------------------------------------------------------------
static int DoExport(string[] args)
{
    if (args.Length < 2)
    {
        Console.Error.WriteLine("Usage: ndi export <file.ndi> <path> [hostdir] [--detokenize] [--tokens <path>]");
        return 1;
    }

    string imagePath = args[0];
    string ndiFilePath = args[1];
    string? hostDest = null;
    bool doDetokenize = false;
    string? tokensPath = null;

    for (int i = 2; i < args.Length; i++)
    {
        switch (args[i])
        {
            case "--detokenize":
                doDetokenize = true;
                break;
            case "--tokens" when i + 1 < args.Length:
                tokensPath = args[++i];
                break;
            default:
                if (!args[i].StartsWith("--"))
                    hostDest = args[i];
                break;
        }
    }

    NdiImage img;
    try { img = NdiImage.Open(imagePath); }
    catch (Exception ex) { Console.Error.WriteLine($"Error: {ex.Message}"); return 1; }

    byte[] data;
    string filename;
    using (img)
    {
        string[] parts = ndiFilePath.TrimStart('/').Split('/', StringSplitOptions.RemoveEmptyEntries);
        if (parts.Length == 0)
        {
            Console.Error.WriteLine("Invalid path — must specify a file, not a directory.");
            return 1;
        }

        filename = parts[^1];
        string dirPath = "/" + string.Join("/", parts[..^1]);

        ushort parentIndex;
        try { parentIndex = ResolvePath(img, dirPath); }
        catch (Exception ex) { Console.Error.WriteLine($"Error: {ex.Message}"); return 1; }

        try { data = img.ReadFile(filename, parentIndex); }
        catch (Exception ex) { Console.Error.WriteLine($"Error: {ex.Message}"); return 1; }
    }

    if (doDetokenize)
    {
        tokensPath ??= FindTokensJson();
        if (tokensPath is null)
        {
            Console.Error.WriteLine("tokens.json not found. Use --tokens <path>.");
            return 1;
        }
        try
        {
            var tokenizer = BasicTokenizer.FromJsonFile(tokensPath);
            string[] lines = tokenizer.Detokenize(data);
            data = Encoding.ASCII.GetBytes(string.Join("\n", lines) + "\n");
            if (Path.GetExtension(filename).Equals(".bas", StringComparison.OrdinalIgnoreCase))
                filename = Path.ChangeExtension(filename, ".txt");
        }
        catch (Exception ex)
        {
            Console.Error.WriteLine($"Detokenize error: {ex.Message}");
            return 1;
        }
    }

    string outputPath;
    if (hostDest is not null && Directory.Exists(hostDest))
        outputPath = Path.Combine(hostDest, filename);
    else if (hostDest is not null)
        outputPath = hostDest;
    else
        outputPath = filename;

    try { File.WriteAllBytes(outputPath, data); }
    catch (Exception ex) { Console.Error.WriteLine($"Error writing output: {ex.Message}"); return 1; }

    Console.WriteLine($"Exported {ndiFilePath} -> {outputPath}  ({data.Length} bytes)");
    return 0;
}

// ---------------------------------------------------------------------------
// delete <file.ndi> <path>
// ---------------------------------------------------------------------------
static int DoDelete(string[] args)
{
    if (args.Length < 2)
    {
        Console.Error.WriteLine("Usage: ndi delete <file.ndi> <path>");
        return 1;
    }

    string imagePath = args[0];
    string ndiFilePath = args[1];

    NdiImage img;
    try { img = NdiImage.Open(imagePath); }
    catch (Exception ex) { Console.Error.WriteLine($"Error: {ex.Message}"); return 1; }

    using (img)
    {
        string[] parts = ndiFilePath.TrimStart('/').Split('/', StringSplitOptions.RemoveEmptyEntries);
        if (parts.Length == 0)
        {
            Console.Error.WriteLine("Invalid path.");
            return 1;
        }

        string filename = parts[^1];
        string dirPath = "/" + string.Join("/", parts[..^1]);

        ushort parentIndex;
        try { parentIndex = ResolvePath(img, dirPath); }
        catch (Exception ex) { Console.Error.WriteLine($"Error: {ex.Message}"); return 1; }

        try { img.DeleteFile(filename, parentIndex); }
        catch (Exception ex) { Console.Error.WriteLine($"Error: {ex.Message}"); return 1; }
    }

    Console.WriteLine($"Deleted {ndiFilePath}");
    return 0;
}

// ---------------------------------------------------------------------------
// mkdir <file.ndi> <path>
// ---------------------------------------------------------------------------
static int DoMkdir(string[] args)
{
    if (args.Length < 2)
    {
        Console.Error.WriteLine("Usage: ndi mkdir <file.ndi> <path>");
        return 1;
    }

    string imagePath = args[0];
    string ndiPath = args[1];

    NdiImage img;
    try { img = NdiImage.Open(imagePath); }
    catch (Exception ex) { Console.Error.WriteLine($"Error: {ex.Message}"); return 1; }

    using (img)
    {
        string[] parts = ndiPath.TrimStart('/').Split('/', StringSplitOptions.RemoveEmptyEntries);
        if (parts.Length == 0)
        {
            Console.Error.WriteLine("Invalid path.");
            return 1;
        }

        string dirName = parts[^1];
        string parentPath = "/" + string.Join("/", parts[..^1]);

        ushort parentIndex;
        try { parentIndex = ResolvePath(img, parentPath); }
        catch (Exception ex) { Console.Error.WriteLine($"Error: {ex.Message}"); return 1; }

        try { img.MakeDirectory(dirName, parentIndex); }
        catch (Exception ex) { Console.Error.WriteLine($"Error: {ex.Message}"); return 1; }
    }

    Console.WriteLine($"Created directory {ndiPath}");
    return 0;
}

// ---------------------------------------------------------------------------
// rmdir <file.ndi> <path>
// ---------------------------------------------------------------------------
static int DoRmdir(string[] args)
{
    if (args.Length < 2)
    {
        Console.Error.WriteLine("Usage: ndi rmdir <file.ndi> <path>");
        return 1;
    }

    string imagePath = args[0];
    string ndiPath = args[1];

    NdiImage img;
    try { img = NdiImage.Open(imagePath); }
    catch (Exception ex) { Console.Error.WriteLine($"Error: {ex.Message}"); return 1; }

    using (img)
    {
        string[] parts = ndiPath.TrimStart('/').Split('/', StringSplitOptions.RemoveEmptyEntries);
        if (parts.Length == 0)
        {
            Console.Error.WriteLine("Invalid path.");
            return 1;
        }

        string dirName = parts[^1];
        string parentPath = "/" + string.Join("/", parts[..^1]);

        ushort parentIndex;
        try { parentIndex = ResolvePath(img, parentPath); }
        catch (Exception ex) { Console.Error.WriteLine($"Error: {ex.Message}"); return 1; }

        try { img.RemoveDirectory(dirName, parentIndex); }
        catch (Exception ex) { Console.Error.WriteLine($"Error: {ex.Message}"); return 1; }
    }

    Console.WriteLine($"Removed directory {ndiPath}");
    return 0;
}

// ---------------------------------------------------------------------------
// tokenize <input.txt> <output.bas> [--base <addr>] [--tokens <path>]
// ---------------------------------------------------------------------------
static int DoTokenize(string[] args)
{
    if (args.Length < 2)
    {
        Console.Error.WriteLine("Usage: ndi tokenize <input.txt> <output.bas> [--base <addr>] [--tokens <path>]");
        return 1;
    }

    string inputPath = args[0];
    string outputPath = args[1];
    ushort baseAddr = 0x0401;
    string? tokensPath = null;

    for (int i = 2; i < args.Length; i++)
    {
        switch (args[i])
        {
            case "--base" when i + 1 < args.Length:
            {
                string baseStr = args[++i].Replace("0x", "").Replace("$", "");
                if (!ushort.TryParse(baseStr, System.Globalization.NumberStyles.HexNumber, null, out baseAddr))
                {
                    Console.Error.WriteLine($"Invalid base address: {args[i]}");
                    return 1;
                }
                break;
            }
            case "--tokens" when i + 1 < args.Length:
                tokensPath = args[++i];
                break;
        }
    }

    if (!File.Exists(inputPath))
    {
        Console.Error.WriteLine($"File not found: {inputPath}");
        return 1;
    }

    tokensPath ??= FindTokensJson();
    if (tokensPath is null)
    {
        Console.Error.WriteLine("tokens.json not found. Use --tokens <path>.");
        return 1;
    }

    try
    {
        var tokenizer = BasicTokenizer.FromJsonFile(tokensPath);
        string text = File.ReadAllText(inputPath);
        string[] lines = text.ReplaceLineEndings("\n").Split('\n', StringSplitOptions.RemoveEmptyEntries);

        byte[] body = tokenizer.Tokenize(lines, baseAddr);

        // Prepend 2-byte load address (little-endian)
        var withHeader = new byte[2 + body.Length];
        withHeader[0] = (byte)(baseAddr & 0xFF);
        withHeader[1] = (byte)(baseAddr >> 8);
        Array.Copy(body, 0, withHeader, 2, body.Length);

        File.WriteAllBytes(outputPath, withHeader);
        Console.WriteLine($"Tokenized {inputPath} -> {outputPath}  ({withHeader.Length} bytes, base=${baseAddr:X4})");
    }
    catch (Exception ex)
    {
        Console.Error.WriteLine($"Error: {ex.Message}");
        return 1;
    }

    return 0;
}

// ---------------------------------------------------------------------------
// detokenize <input.bas> [output.txt] [--tokens <path>]
// ---------------------------------------------------------------------------
static int DoDetokenize(string[] args)
{
    if (args.Length < 1)
    {
        Console.Error.WriteLine("Usage: ndi detokenize <input.bas> [output.txt] [--tokens <path>]");
        return 1;
    }

    string inputPath = args[0];
    string? outputPath = null;
    string? tokensPath = null;

    for (int i = 1; i < args.Length; i++)
    {
        switch (args[i])
        {
            case "--tokens" when i + 1 < args.Length:
                tokensPath = args[++i];
                break;
            default:
                if (!args[i].StartsWith("--"))
                    outputPath = args[i];
                break;
        }
    }

    if (!File.Exists(inputPath))
    {
        Console.Error.WriteLine($"File not found: {inputPath}");
        return 1;
    }

    tokensPath ??= FindTokensJson();
    if (tokensPath is null)
    {
        Console.Error.WriteLine("tokens.json not found. Use --tokens <path>.");
        return 1;
    }

    try
    {
        var tokenizer = BasicTokenizer.FromJsonFile(tokensPath);
        byte[] raw = File.ReadAllBytes(inputPath);

        // Skip 2-byte load-address prefix if present
        byte[] body = raw.Length >= 2 ? raw[2..] : raw;

        string[] lines = tokenizer.Detokenize(body);
        string text = string.Join("\n", lines) + "\n";

        if (outputPath is null)
        {
            Console.Write(text);
        }
        else
        {
            File.WriteAllText(outputPath, text, Encoding.ASCII);
            Console.WriteLine($"Detokenized {inputPath} -> {outputPath}  ({lines.Length} lines)");
        }
    }
    catch (Exception ex)
    {
        Console.Error.WriteLine($"Error: {ex.Message}");
        return 1;
    }

    return 0;
}

// ---------------------------------------------------------------------------
// Shared helpers
// ---------------------------------------------------------------------------

/// <summary>
/// Resolves an NDI path like "/" or "/games/rpg" to the parentIndex value
/// used by NdiImage (0xFFFF = root).
/// </summary>
static ushort ResolvePath(NdiImage img, string path)
{
    const ushort Root = 0xFFFF;

    string[] parts = path.TrimStart('/').Split('/', StringSplitOptions.RemoveEmptyEntries);
    if (parts.Length == 0)
        return Root;

    ushort current = Root;
    foreach (string part in parts)
    {
        var entries = img.ListDirectory(current);
        var match = entries.FirstOrDefault(e =>
            e.IsDirectory &&
            string.Equals(e.Filename, part, StringComparison.OrdinalIgnoreCase));

        if (match is null)
            throw new DirectoryNotFoundException($"Directory '{part}' not found.");

        current = (ushort)match.Index;
    }

    return current;
}

/// <summary>Collects all active directory entries across all directories (BFS).</summary>
static NdiDirEntry[] CollectAllEntries(NdiImage img)
{
    var result = new List<NdiDirEntry>();
    var queue = new Queue<ushort>();
    queue.Enqueue(0xFFFF);

    while (queue.Count > 0)
    {
        ushort parent = queue.Dequeue();
        foreach (var e in img.ListDirectory(parent))
        {
            result.Add(e);
            if (e.IsDirectory)
                queue.Enqueue((ushort)e.Index);
        }
    }

    return result.ToArray();
}

/// <summary>Infers NdiFileType from file extension.</summary>
static NdiFileType ExtensionToFileType(string ext) =>
    ext.ToLowerInvariant() switch
    {
        ".bas"            => NdiFileType.Bas,
        ".sid"            => NdiFileType.Sid,
        ".bin"            => NdiFileType.Bin,
        ".mid" or ".midi" => NdiFileType.Mid,
        ".gfx"            => NdiFileType.Gfx,
        _                 => NdiFileType.Bin,
    };

/// <summary>
/// Searches upward from the executable directory, then from cwd,
/// for ehbasic/tokens.json.
/// </summary>
static string? FindTokensJson()
{
    string? dir = Path.GetDirectoryName(Environment.ProcessPath);
    while (dir is not null)
    {
        string candidate = Path.Combine(dir, "ehbasic", "tokens.json");
        if (File.Exists(candidate))
            return candidate;
        dir = Path.GetDirectoryName(dir);
    }

    dir = Directory.GetCurrentDirectory();
    while (dir is not null)
    {
        string candidate = Path.Combine(dir, "ehbasic", "tokens.json");
        if (File.Exists(candidate))
            return candidate;
        string? parent = Path.GetDirectoryName(dir);
        if (parent == dir) break;
        dir = parent;
    }

    return null;
}

static int UnknownVerb(string verb)
{
    Console.Error.WriteLine($"Unknown command: {verb}");
    PrintUsage();
    return 1;
}

static void PrintUsage()
{
    Console.Error.WriteLine("e6502 NDI -- Nova Disk Image tool");
    Console.Error.WriteLine();
    Console.Error.WriteLine("Commands:");
    Console.Error.WriteLine("  create     <file.ndi> [--size <KB>] [--label <name>]");
    Console.Error.WriteLine("  dir        <file.ndi> [/path]");
    Console.Error.WriteLine("  info       <file.ndi>");
    Console.Error.WriteLine("  validate   <file.ndi>");
    Console.Error.WriteLine("  label      <file.ndi> <name>");
    Console.Error.WriteLine("  import     <file.ndi> <hostfile> [/dest] [--tokenize] [--tokens <path>]");
    Console.Error.WriteLine("  export     <file.ndi> <path> [hostdir] [--detokenize] [--tokens <path>]");
    Console.Error.WriteLine("  delete     <file.ndi> <path>");
    Console.Error.WriteLine("  mkdir      <file.ndi> <path>");
    Console.Error.WriteLine("  rmdir      <file.ndi> <path>");
    Console.Error.WriteLine("  tokenize   <input.txt> <output.bas> [--base <addr>] [--tokens <path>]");
    Console.Error.WriteLine("  detokenize <input.bas> [output.txt] [--tokens <path>]");
}

# File Enumeration & Metadata System Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Add wildcard-filtered directory enumeration with automatic file metadata extraction, accessible from both BASIC and assembly via memory-mapped FIO registers.

**Architecture:** Extend existing FIO_CMD_DIROPEN/DIRREAD in FileIoController.cs to support wildcard patterns and auto-populate a 112-byte metadata buffer at $BAB0-$BB1F. Add 6 new BASIC tokens as thin ROM wrappers. Assembly programmers use the same registers directly.

**Tech Stack:** C# (.NET 10, MSTest), 6502 assembly (ca65 assembler), DryWetMidi library

**Design doc:** `docs/plans/2026-03-06-file-enumeration-metadata-design.md`

---

## Task 1: Add metadata buffer constants to VgcConstants.cs

**Context:** The metadata buffer lives at $BAB0-$BB1F in the free I/O space. We need C# constants for all field offsets so FileIoController can write metadata there. The free space starts at $BAA0 (VgcConstants.FreeBase). The buffer is in the flat `byte[]` array managed by CompositeBusDevice — no special interception needed.

**Files:**
- Modify: `e6502.Avalonia/Hardware/VgcConstants.cs:312-313` (after `FreeBase`/`FreeEnd`)

**Step 1: Add the metadata buffer constants**

Add a new section after the `FreeBase`/`FreeEnd` lines (line 312-313) in `VgcConstants.cs`:

```csharp
// -------------------------------------------------------------------------
// File metadata buffer ($BAB0-$BB1F) — populated during filtered DIRREAD
// -------------------------------------------------------------------------

public const int MetaBase       = 0xBAB0;
public const int MetaEnd        = 0xBB1F;
public const int MetaSize       = MetaEnd - MetaBase + 1;  // 112 bytes

public const int MetaType       = 0xBAB0;   // 1 byte: file type (NdiFileType enum value)
public const int MetaSizeL      = 0xBAB1;   // file size low byte
public const int MetaSizeH      = 0xBAB2;   // file size high byte
public const int MetaTitle      = 0xBAB3;   // 32 bytes, null-padded ASCII
public const int MetaAuthor     = 0xBAD3;   // 32 bytes, null-padded ASCII
public const int MetaCopyright  = 0xBAF3;   // 32 bytes, null-padded ASCII
public const int MetaLoadL      = 0xBB13;   // load address low (BIN/SID)
public const int MetaLoadH      = 0xBB14;   // load address high
public const int MetaInitL      = 0xBB15;   // init address low (SID)
public const int MetaInitH      = 0xBB16;   // init address high
public const int MetaPlayL      = 0xBB17;   // play address low (SID)
public const int MetaPlayH      = 0xBB18;   // play address high
public const int MetaSongs      = 0xBB19;   // song/track count (SID subtunes / MID tracks)
public const int MetaDurL       = 0xBB1A;   // duration seconds low (MID only)
public const int MetaDurH       = 0xBB1B;   // duration seconds high
public const int MetaGfxSpace   = 0xBB1C;   // GFX space type

public const int MetaTitleLen   = 32;
public const int MetaAuthorLen  = 32;
public const int MetaCopyrightLen = 32;
```

**Step 2: Build to verify no errors**

Run: `dotnet build`
Expected: Build succeeds

**Step 3: Commit**

```bash
git add e6502.Avalonia/Hardware/VgcConstants.cs
git commit -m "feat: add metadata buffer constants to VgcConstants"
```

---

## Task 2: Add wildcard glob matching utility

**Context:** We need a simple glob matcher that supports `*` (zero or more chars) and `?` (one char), case-insensitive. This will be used by DoDirOpen to filter directory entries. It should be a static helper method in FileIoController.

**Files:**
- Modify: `e6502.Avalonia/Hardware/FileIoController.cs` (add static method near bottom, before the closing brace)
- Create: test in `e6502UnitTests/FileIoControllerTests.cs`

**Step 1: Write the failing tests**

Add to `e6502UnitTests/FileIoControllerTests.cs`. The method is internal static, so the test project needs access. Since the method is in a `partial class`, add a small public test-surface wrapper, OR make the method `internal` and add `[assembly: InternalsVisibleTo("e6502UnitTests")]`.

Actually, simpler: test glob matching indirectly through the DIROPEN command behavior (Task 4 tests will cover this). For now, add the method as `internal static` and add `InternalsVisibleTo` if not already present. Then add direct unit tests.

Check if InternalsVisibleTo exists:
```bash
grep -r "InternalsVisibleTo" e6502.Avalonia/
```

If not found, add `[assembly: InternalsVisibleTo("e6502UnitTests")]` to a file or the `.csproj`. Alternatively, make the method `public` since it's a pure utility with no side effects.

Add these tests to `e6502UnitTests/FileIoControllerTests.cs`:

```csharp
[TestMethod]
public void GlobMatch_StarMatchesAnything()
{
    Assert.IsTrue(FileIoController.GlobMatch("*", "anything"));
    Assert.IsTrue(FileIoController.GlobMatch("*", ""));
    Assert.IsTrue(FileIoController.GlobMatch("*", "BACH-FUGUE"));
}

[TestMethod]
public void GlobMatch_StarPrefix()
{
    Assert.IsTrue(FileIoController.GlobMatch("BACH*", "BACH-FUGUE"));
    Assert.IsTrue(FileIoController.GlobMatch("BACH*", "BACH"));
    Assert.IsFalse(FileIoController.GlobMatch("BACH*", "MOZART"));
}

[TestMethod]
public void GlobMatch_StarSuffix()
{
    Assert.IsTrue(FileIoController.GlobMatch("*FUGUE", "BACH-FUGUE"));
    Assert.IsFalse(FileIoController.GlobMatch("*FUGUE", "BACH-SONATA"));
}

[TestMethod]
public void GlobMatch_QuestionMark()
{
    Assert.IsTrue(FileIoController.GlobMatch("A?C", "ABC"));
    Assert.IsFalse(FileIoController.GlobMatch("A?C", "ABBC"));
    Assert.IsFalse(FileIoController.GlobMatch("A?C", "AC"));
}

[TestMethod]
public void GlobMatch_CaseInsensitive()
{
    Assert.IsTrue(FileIoController.GlobMatch("bach*", "BACH-FUGUE"));
    Assert.IsTrue(FileIoController.GlobMatch("BACH*", "bach-fugue"));
}

[TestMethod]
public void GlobMatch_MiddleStar()
{
    Assert.IsTrue(FileIoController.GlobMatch("BACH*FUGUE", "BACH-CHROMATIC-FUGUE"));
    Assert.IsFalse(FileIoController.GlobMatch("BACH*FUGUE", "BACH-SONATA"));
}
```

**Step 2: Run tests to verify they fail**

Run: `dotnet test --filter "GlobMatch" --verbosity normal`
Expected: FAIL (method doesn't exist yet)

**Step 3: Implement GlobMatch**

Add this `internal static` method to `FileIoController.cs`, near the bottom (before the closing `}`):

```csharp
/// <summary>
/// Simple glob matching: * matches zero or more chars, ? matches exactly one char.
/// Case-insensitive.
/// </summary>
internal static bool GlobMatch(string pattern, string text)
{
    return GlobMatchCore(pattern.AsSpan(), text.AsSpan());
}

private static bool GlobMatchCore(ReadOnlySpan<char> pattern, ReadOnlySpan<char> text)
{
    int p = 0, t = 0;
    int starP = -1, starT = -1;

    while (t < text.Length)
    {
        if (p < pattern.Length &&
            (char.ToUpperInvariant(pattern[p]) == char.ToUpperInvariant(text[t]) || pattern[p] == '?'))
        {
            p++;
            t++;
        }
        else if (p < pattern.Length && pattern[p] == '*')
        {
            starP = p;
            starT = t;
            p++;
        }
        else if (starP >= 0)
        {
            p = starP + 1;
            starT++;
            t = starT;
        }
        else
        {
            return false;
        }
    }

    while (p < pattern.Length && pattern[p] == '*')
        p++;

    return p == pattern.Length;
}
```

If `InternalsVisibleTo` is needed, add to `e6502.Avalonia/Properties/AssemblyInfo.cs` or create it:
```csharp
using System.Runtime.CompilerServices;
[assembly: InternalsVisibleTo("e6502UnitTests")]
```

**Step 4: Run tests to verify they pass**

Run: `dotnet test --filter "GlobMatch" --verbosity normal`
Expected: All 6 tests PASS

**Step 5: Commit**

```bash
git add e6502.Avalonia/Hardware/FileIoController.cs e6502UnitTests/FileIoControllerTests.cs
# Also add AssemblyInfo.cs if created
git commit -m "feat: add GlobMatch wildcard utility for directory filtering"
```

---

## Task 3: Add filter pattern parser

**Context:** DIROPEN patterns can be `*.mid`, `FD0:*.mid`, or `FD0:songs/*.sid`. We need a parser that splits these into: device prefix (optional), directory path (optional), name glob pattern, and extension filter (optional). This is a static utility method used by DoDirOpen.

**Files:**
- Modify: `e6502.Avalonia/Hardware/FileIoController.cs` (add static method and result type)
- Modify: `e6502UnitTests/FileIoControllerTests.cs` (add tests)

**Step 1: Write the failing tests**

Add to `FileIoControllerTests.cs`:

```csharp
[TestMethod]
public void ParseFilterPattern_SimpleWildcard()
{
    var result = FileIoController.ParseFilterPattern("*.mid");
    Assert.IsNull(result.DevicePrefix);
    Assert.IsNull(result.DirectoryPath);
    Assert.AreEqual("*", result.NamePattern);
    Assert.AreEqual(".mid", result.ExtFilter);
}

[TestMethod]
public void ParseFilterPattern_WithDevice()
{
    var result = FileIoController.ParseFilterPattern("FD0:*.sid");
    Assert.AreEqual("FD0", result.DevicePrefix);
    Assert.IsNull(result.DirectoryPath);
    Assert.AreEqual("*", result.NamePattern);
    Assert.AreEqual(".sid", result.ExtFilter);
}

[TestMethod]
public void ParseFilterPattern_WithDeviceAndPath()
{
    var result = FileIoController.ParseFilterPattern("FD0:songs/*.sid");
    Assert.AreEqual("FD0", result.DevicePrefix);
    Assert.AreEqual("songs", result.DirectoryPath);
    Assert.AreEqual("*", result.NamePattern);
    Assert.AreEqual(".sid", result.ExtFilter);
}

[TestMethod]
public void ParseFilterPattern_NoExtension()
{
    var result = FileIoController.ParseFilterPattern("BACH*");
    Assert.IsNull(result.DevicePrefix);
    Assert.IsNull(result.DirectoryPath);
    Assert.AreEqual("BACH*", result.NamePattern);
    Assert.IsNull(result.ExtFilter);
}

[TestMethod]
public void ParseFilterPattern_NameAndExt()
{
    var result = FileIoController.ParseFilterPattern("BACH*.mid");
    Assert.IsNull(result.DevicePrefix);
    Assert.IsNull(result.DirectoryPath);
    Assert.AreEqual("BACH*", result.NamePattern);
    Assert.AreEqual(".mid", result.ExtFilter);
}

[TestMethod]
public void ParseFilterPattern_DevicePathAndPattern()
{
    var result = FileIoController.ParseFilterPattern("HD:music/bach/*.mid");
    Assert.AreEqual("HD", result.DevicePrefix);
    Assert.AreEqual("music/bach", result.DirectoryPath);
    Assert.AreEqual("*", result.NamePattern);
    Assert.AreEqual(".mid", result.ExtFilter);
}
```

**Step 2: Run tests to verify they fail**

Run: `dotnet test --filter "ParseFilterPattern" --verbosity normal`
Expected: FAIL (type/method doesn't exist)

**Step 3: Implement the parser**

Add to `FileIoController.cs`:

```csharp
internal record struct FilterPattern(string? DevicePrefix, string? DirectoryPath, string NamePattern, string? ExtFilter);

internal static FilterPattern ParseFilterPattern(string pattern)
{
    string? devicePrefix = null;
    string remainder = pattern;

    // Split device prefix on first ':'
    int colon = pattern.IndexOf(':');
    if (colon > 0)
    {
        devicePrefix = pattern[..colon];
        remainder = pattern[(colon + 1)..];
    }

    // Split directory path on last '/'
    string? dirPath = null;
    int lastSlash = remainder.LastIndexOf('/');
    if (lastSlash >= 0)
    {
        dirPath = lastSlash > 0 ? remainder[..lastSlash] : null;
        remainder = remainder[(lastSlash + 1)..];
    }

    // Split extension from the glob pattern
    // Look for known extensions: .bas, .sid, .bin, .mid, .gfx
    string? extFilter = null;
    string namePattern = remainder;
    int dot = remainder.LastIndexOf('.');
    if (dot >= 0)
    {
        string ext = remainder[dot..];
        if (ext.Equals(".bas", StringComparison.OrdinalIgnoreCase) ||
            ext.Equals(".sid", StringComparison.OrdinalIgnoreCase) ||
            ext.Equals(".bin", StringComparison.OrdinalIgnoreCase) ||
            ext.Equals(".mid", StringComparison.OrdinalIgnoreCase) ||
            ext.Equals(".gfx", StringComparison.OrdinalIgnoreCase))
        {
            extFilter = ext.ToLowerInvariant();
            namePattern = remainder[..dot];
        }
    }

    return new FilterPattern(devicePrefix, dirPath, namePattern, extFilter);
}
```

**Step 4: Run tests to verify they pass**

Run: `dotnet test --filter "ParseFilterPattern" --verbosity normal`
Expected: All 6 tests PASS

**Step 5: Commit**

```bash
git add e6502.Avalonia/Hardware/FileIoController.cs e6502UnitTests/FileIoControllerTests.cs
git commit -m "feat: add filter pattern parser for wildcard directory enumeration"
```

---

## Task 4: Extend DoDirOpen for filtered enumeration

**Context:** Currently `DoDirOpen()` (line 427 of `FileIoController.cs`) always lists all files. We need to check `FIO_NAMELEN`: if > 0, read the pattern from `FIO_NAME`, parse it, resolve device/path, list directory, and filter entries by the parsed glob+extension. Store the filtered list in `_dirEntries`. Also add a `_dirFiltered` bool field to track whether metadata extraction should happen during DIRREAD.

The existing fields to use/modify:
- `_dirEntries` (List<StorageDirEntry>?) — store filtered results here
- `_dirDevice` (IStorageDevice?) — the device being enumerated
- `_dirIndex` (int) — current position
- `_dirFiles` (List<FileInfo>?) — legacy filesystem fallback

Add new field: `_dirFiltered` (bool) — true when enumeration was started with a filter pattern.

**Files:**
- Modify: `e6502.Avalonia/Hardware/FileIoController.cs:19-23` (add field), `427-485` (rewrite DoDirOpen)
- Modify: `e6502UnitTests/FileIoControllerTests.cs` (add tests)

**Step 1: Write the failing tests**

These tests use a `DeviceManager` with a real `HostDirectoryDevice` pointed at a temp directory containing test files. We need test `.mid` and `.sid` files.

Add to `FileIoControllerTests.cs`:

```csharp
private static (FileIoController Fio, byte[] Memory, string TempDir) MakeControllerWithDevice()
{
    var tempDir = Path.Combine(Path.GetTempPath(), "fio_test_" + Guid.NewGuid().ToString("N")[..8]);
    Directory.CreateDirectory(tempDir);

    var memory = new byte[65536];
    var dm = new e6502.Storage.DeviceManager();
    dm.RegisterDevice(new e6502.Storage.HostDirectoryDevice(tempDir, "HD"));
    dm.DefaultDevice = "HD";

    var fio = new FileIoController(
        address => memory[address],
        (address, data) => memory[address] = data,
        tempDir,
        deviceManager: dm);

    return (fio, memory, tempDir);
}

[TestMethod]
public void DirOpen_FilteredByExtension_OnlyReturnsMid()
{
    var (fio, memory, tempDir) = MakeControllerWithDevice();
    try
    {
        // Create test files
        File.WriteAllBytes(Path.Combine(tempDir, "song1.mid"), new byte[] { 0x4D, 0x54, 0x68, 0x64 }); // MThd
        File.WriteAllBytes(Path.Combine(tempDir, "song2.mid"), new byte[] { 0x4D, 0x54, 0x68, 0x64 });
        File.WriteAllBytes(Path.Combine(tempDir, "prog.bas"), new byte[] { 0x00, 0x00 });
        File.WriteAllBytes(Path.Combine(tempDir, "tune.sid"), new byte[124]); // min SID size

        // DIROPEN "*.mid"
        SetFilename(fio, "*.mid");
        fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdDirOpen);

        Assert.AreEqual(VgcConstants.FioStatusOk, fio.Read((ushort)VgcConstants.FioStatus));
        var firstName = ReadFilename(fio);
        Assert.AreEqual("song1", firstName);

        // DIRREAD — second entry
        fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdDirRead);
        Assert.AreEqual(VgcConstants.FioStatusOk, fio.Read((ushort)VgcConstants.FioStatus));
        Assert.AreEqual("song2", ReadFilename(fio));

        // DIRREAD — should be end of dir
        fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdDirRead);
        Assert.AreEqual(VgcConstants.FioStatusError, fio.Read((ushort)VgcConstants.FioStatus));
        Assert.AreEqual(VgcConstants.FioErrEndOfDir, fio.Read((ushort)VgcConstants.FioErrCode));
    }
    finally
    {
        Directory.Delete(tempDir, true);
    }
}

[TestMethod]
public void DirOpen_FilteredByNameGlob()
{
    var (fio, memory, tempDir) = MakeControllerWithDevice();
    try
    {
        File.WriteAllBytes(Path.Combine(tempDir, "bach-fugue.mid"), new byte[4]);
        File.WriteAllBytes(Path.Combine(tempDir, "bach-sonata.mid"), new byte[4]);
        File.WriteAllBytes(Path.Combine(tempDir, "mozart-sonata.mid"), new byte[4]);

        SetFilename(fio, "bach*.mid");
        fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdDirOpen);

        Assert.AreEqual(VgcConstants.FioStatusOk, fio.Read((ushort)VgcConstants.FioStatus));
        Assert.AreEqual("bach-fugue", ReadFilename(fio));

        fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdDirRead);
        Assert.AreEqual(VgcConstants.FioStatusOk, fio.Read((ushort)VgcConstants.FioStatus));
        Assert.AreEqual("bach-sonata", ReadFilename(fio));

        fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdDirRead);
        Assert.AreEqual(VgcConstants.FioStatusError, fio.Read((ushort)VgcConstants.FioStatus));
    }
    finally
    {
        Directory.Delete(tempDir, true);
    }
}

[TestMethod]
public void DirOpen_UnfilteredBackwardCompatible()
{
    var (fio, memory, tempDir) = MakeControllerWithDevice();
    try
    {
        File.WriteAllBytes(Path.Combine(tempDir, "prog.bas"), new byte[] { 0x00, 0x00 });
        File.WriteAllBytes(Path.Combine(tempDir, "tune.sid"), new byte[124]);

        // DIROPEN with NAMELEN=0 — should list all files
        fio.Write((ushort)VgcConstants.FioNameLen, 0);
        fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdDirOpen);

        Assert.AreEqual(VgcConstants.FioStatusOk, fio.Read((ushort)VgcConstants.FioStatus));
        // Should have at least one entry
        var name = ReadFilename(fio);
        Assert.IsTrue(name.Length > 0);
    }
    finally
    {
        Directory.Delete(tempDir, true);
    }
}

[TestMethod]
public void DirOpen_NoMatches_ReturnsEndOfDir()
{
    var (fio, memory, tempDir) = MakeControllerWithDevice();
    try
    {
        File.WriteAllBytes(Path.Combine(tempDir, "prog.bas"), new byte[] { 0x00, 0x00 });

        SetFilename(fio, "*.mid");
        fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdDirOpen);

        Assert.AreEqual(VgcConstants.FioStatusError, fio.Read((ushort)VgcConstants.FioStatus));
        Assert.AreEqual(VgcConstants.FioErrEndOfDir, fio.Read((ushort)VgcConstants.FioErrCode));
    }
    finally
    {
        Directory.Delete(tempDir, true);
    }
}
```

**Step 2: Run tests to verify they fail**

Run: `dotnet test --filter "DirOpen_Filtered|DirOpen_Unfiltered|DirOpen_NoMatches" --verbosity normal`
Expected: FAIL

**Step 3: Implement filtered DoDirOpen**

Add `_dirFiltered` field at line 23 of `FileIoController.cs`:

```csharp
private bool _dirFiltered;
```

Rewrite `DoDirOpen()` (starting at line 427). The key change: when `FIO_NAMELEN > 0`, read the pattern, parse it with `ParseFilterPattern`, resolve the device, list the directory, filter entries by extension and name glob, store in `_dirEntries`, set `_dirFiltered = true`.

```csharp
private void DoDirOpen()
{
    try
    {
        int nameLen = _regs[VgcConstants.FioNameLen - VgcConstants.FioBase];

        if (nameLen > 0)
        {
            // Filtered enumeration
            var sb = new StringBuilder(nameLen);
            int nameOffset = VgcConstants.FioName - VgcConstants.FioBase;
            for (int i = 0; i < nameLen; i++)
                sb.Append((char)_regs[nameOffset + i]);

            var filter = ParseFilterPattern(sb.ToString());
            IStorageDevice? device = null;
            string? savedDir = null;

            if (_deviceManager is not null)
            {
                device = filter.DevicePrefix is not null
                    ? _deviceManager.GetDevice(filter.DevicePrefix)
                    : _deviceManager.GetDevice(_deviceManager.DefaultDevice);

                if (!device.IsMounted)
                {
                    SetError(VgcConstants.FioErrNotMounted);
                    return;
                }

                // Temporarily cd into filter path if specified
                if (filter.DirectoryPath is not null)
                {
                    savedDir = device.CurrentDirectory;
                    device.CurrentDirectory = filter.DirectoryPath;
                }
            }

            try
            {
                StorageDirEntry[] allEntries;
                if (device is not null)
                {
                    allEntries = device.ListDirectory(null);
                }
                else
                {
                    // Direct filesystem fallback
                    var dir = new DirectoryInfo(_saveDir);
                    if (!dir.Exists)
                    {
                        allEntries = [];
                    }
                    else
                    {
                        var supported = new[] { ".bas", ".sid", ".bin", ".mid", ".gfx" };
                        allEntries = supported
                            .SelectMany(ext => dir.GetFiles("*" + ext))
                            .Select(f => new StorageDirEntry(
                                Path.GetFileNameWithoutExtension(f.Name),
                                false,
                                ExtToNdiType(Path.GetExtension(f.Name)),
                                (int)f.Length))
                            .OrderBy(e => e.Filename)
                            .ToArray();
                    }
                }

                // Apply filters
                var filtered = allEntries.Where(e =>
                {
                    if (e.IsDirectory) return false; // skip directories in filtered enum

                    // Extension filter
                    if (filter.ExtFilter is not null)
                    {
                        string entryExt = NdiTypeToExt(e.FileType);
                        if (!entryExt.Equals(filter.ExtFilter, StringComparison.OrdinalIgnoreCase))
                            return false;
                    }

                    // Name glob filter
                    return GlobMatch(filter.NamePattern, e.Filename);
                }).ToList();

                _dirEntries = filtered;
                _dirFiles = null;
                _dirDevice = device;
                _dirIndex = 0;
                _dirFiltered = true;

                if (_dirEntries.Count > 0)
                {
                    PopulateDirEntryFromStorage(_dirEntries[0]);
                    PopulateMetadata(_dirEntries[0]);
                    SetOk();
                }
                else
                {
                    SetEndOfDir();
                }
            }
            finally
            {
                if (device is not null && savedDir is not null)
                    RestoreDir(device, savedDir);
            }
            return;
        }

        // Unfiltered enumeration (FIO_NAMELEN == 0) — existing behavior
        _dirFiltered = false;

        if (_deviceManager is not null)
        {
            var device = _deviceManager.GetDevice(_deviceManager.DefaultDevice);
            if (!device.IsMounted)
            {
                SetError(VgcConstants.FioErrNotMounted);
                return;
            }
            var entries = device.ListDirectory(null);
            _dirEntries = [.. entries];
            _dirFiles = null;
            _dirDevice = device;
            _dirIndex = 0;

            if (_dirIndex < _dirEntries.Count)
            {
                PopulateDirEntryFromStorage(_dirEntries[_dirIndex]);
                SetOk();
            }
            else
            {
                SetEndOfDir();
            }
            return;
        }

        // Direct filesystem fallback
        _dirEntries = null;
        _dirDevice = null;
        var dirInfo = new DirectoryInfo(_saveDir);
        _dirFiles = dirInfo.Exists
            ? dirInfo.GetFiles("*.bas")
                  .Concat(dirInfo.GetFiles("*.sid"))
                  .Concat(dirInfo.GetFiles("*.bin"))
                  .Concat(dirInfo.GetFiles("*.mid"))
                  .OrderBy(f => f.Name).ToList()
            : [];
        _dirIndex = 0;

        if (_dirIndex < _dirFiles.Count)
        {
            PopulateDirEntry(_dirFiles[_dirIndex]);
            SetOk();
        }
        else
        {
            SetEndOfDir();
        }
    }
    catch
    {
        SetError(VgcConstants.FioErrIo);
    }
}
```

Also add these two helper methods (near the other helpers):

```csharp
private static NdiFileType ExtToNdiType(string ext) => ext.ToLowerInvariant() switch
{
    ".sid" => NdiFileType.Sid,
    ".bin" => NdiFileType.Bin,
    ".mid" => NdiFileType.Mid,
    ".gfx" => NdiFileType.Gfx,
    _ => NdiFileType.Bas
};

private static string NdiTypeToExt(NdiFileType type) => type switch
{
    NdiFileType.Sid => ".sid",
    NdiFileType.Bin => ".bin",
    NdiFileType.Mid => ".mid",
    NdiFileType.Gfx => ".gfx",
    _ => ".bas"
};
```

Add a stub for `PopulateMetadata` so it compiles (will be implemented in Task 5):

```csharp
private void PopulateMetadata(StorageDirEntry entry)
{
    // Stub — will be implemented in Task 5
}
```

**Step 4: Run tests to verify they pass**

Run: `dotnet test --filter "DirOpen_Filtered|DirOpen_Unfiltered|DirOpen_NoMatches" --verbosity normal`
Expected: All 4 tests PASS

**Step 5: Commit**

```bash
git add e6502.Avalonia/Hardware/FileIoController.cs e6502UnitTests/FileIoControllerTests.cs
git commit -m "feat: extend DoDirOpen to support wildcard filter patterns"
```

---

## Task 5: Implement metadata extraction and DIRREAD population

**Context:** When `_dirFiltered` is true, DIRREAD should populate the 112-byte metadata buffer at $BAB0-$BB1F. The buffer is in the CompositeBusDevice's flat `byte[]` — we write to it via the `_busWrite` callback. We need type-specific extractors for SID (header fields), MID (DryWetMidi), BIN (load address), and BAS/GFX (just type+size).

For SID: reuse the parsing logic from `SidFileParser.cs` (offsets 22/54/86 for title/author/copyright, 8/10/12 for addresses, 14 for song count).

For MID: use `DryWetMidi` to parse the MIDI file and extract track name, copyright, track count, and duration.

The file's raw bytes are obtained by loading from `_dirDevice` (or from disk in the filesystem fallback). For filtered enumeration, we load the current entry's file.

**Files:**
- Modify: `e6502.Avalonia/Hardware/FileIoController.cs` (replace stub, add extractors)
- Modify: `e6502UnitTests/FileIoControllerTests.cs` (add tests)

**Step 1: Write the failing tests**

Add to `FileIoControllerTests.cs`:

```csharp
[TestMethod]
public void DirOpen_SidMetadata_PopulatesBuffer()
{
    var (fio, memory, tempDir) = MakeControllerWithDevice();
    try
    {
        // Create a minimal valid SID file with known metadata
        var sid = new byte[124];
        // Magic: PSID
        sid[0] = 0x50; sid[1] = 0x53; sid[2] = 0x49; sid[3] = 0x44;
        // Version: 2
        sid[4] = 0x00; sid[5] = 0x02;
        // Data offset: 0x7C (124)
        sid[6] = 0x00; sid[7] = 0x7C;
        // Load address: $1000 (big-endian)
        sid[8] = 0x10; sid[9] = 0x00;
        // Init address: $1000
        sid[10] = 0x10; sid[11] = 0x00;
        // Play address: $1003
        sid[12] = 0x10; sid[13] = 0x03;
        // Songs: 3
        sid[14] = 0x00; sid[15] = 0x03;
        // Start song
        sid[16] = 0x00; sid[17] = 0x01;
        // Speed
        sid[18] = 0; sid[19] = 0; sid[20] = 0; sid[21] = 0;
        // Title at 22: "Test Song"
        var title = System.Text.Encoding.ASCII.GetBytes("Test Song");
        Array.Copy(title, 0, sid, 22, title.Length);
        // Author at 54: "Test Author"
        var author = System.Text.Encoding.ASCII.GetBytes("Test Author");
        Array.Copy(author, 0, sid, 54, author.Length);
        // Copyright at 86: "2026 Test"
        var copyright = System.Text.Encoding.ASCII.GetBytes("2026 Test");
        Array.Copy(copyright, 0, sid, 86, copyright.Length);

        File.WriteAllBytes(Path.Combine(tempDir, "mysong.sid"), sid);

        SetFilename(fio, "*.sid");
        fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdDirOpen);

        Assert.AreEqual(VgcConstants.FioStatusOk, fio.Read((ushort)VgcConstants.FioStatus));

        // Check metadata buffer
        Assert.AreEqual(1, memory[VgcConstants.MetaType]); // SID type

        // Title at MetaTitle
        var titleBytes = new byte[9];
        Array.Copy(memory, VgcConstants.MetaTitle, titleBytes, 0, 9);
        Assert.AreEqual("Test Song", Encoding.ASCII.GetString(titleBytes));

        // Author
        var authorBytes = new byte[11];
        Array.Copy(memory, VgcConstants.MetaAuthor, authorBytes, 0, 11);
        Assert.AreEqual("Test Author", Encoding.ASCII.GetString(authorBytes));

        // Load address (LE) — SID stores big-endian $10,$00 = $1000
        Assert.AreEqual(0x00, memory[VgcConstants.MetaLoadL]);
        Assert.AreEqual(0x10, memory[VgcConstants.MetaLoadH]);

        // Song count
        Assert.AreEqual(3, memory[VgcConstants.MetaSongs]);
    }
    finally
    {
        Directory.Delete(tempDir, true);
    }
}

[TestMethod]
public void DirOpen_BinMetadata_PopulatesLoadAddress()
{
    var (fio, memory, tempDir) = MakeControllerWithDevice();
    try
    {
        // BIN file: first 2 bytes are load address (LE)
        var bin = new byte[] { 0x00, 0x90, 0xEA, 0xEA }; // load at $9000, NOP NOP
        File.WriteAllBytes(Path.Combine(tempDir, "prog.bin"), bin);

        SetFilename(fio, "*.bin");
        fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdDirOpen);

        Assert.AreEqual(VgcConstants.FioStatusOk, fio.Read((ushort)VgcConstants.FioStatus));

        Assert.AreEqual(2, memory[VgcConstants.MetaType]); // BIN type
        Assert.AreEqual(0x00, memory[VgcConstants.MetaLoadL]);
        Assert.AreEqual(0x90, memory[VgcConstants.MetaLoadH]);
    }
    finally
    {
        Directory.Delete(tempDir, true);
    }
}

[TestMethod]
public void DirRead_Filtered_PopulatesMetadataEachEntry()
{
    var (fio, memory, tempDir) = MakeControllerWithDevice();
    try
    {
        // Two SID files with different titles
        var sid1 = new byte[124];
        sid1[0] = 0x50; sid1[1] = 0x53; sid1[2] = 0x49; sid1[3] = 0x44;
        sid1[4] = 0x00; sid1[5] = 0x02;
        sid1[6] = 0x00; sid1[7] = 0x7C;
        var t1 = Encoding.ASCII.GetBytes("First Song");
        Array.Copy(t1, 0, sid1, 22, t1.Length);

        var sid2 = new byte[124];
        Array.Copy(sid1, sid2, 124);
        var t2 = Encoding.ASCII.GetBytes("Second Song");
        Array.Clear(sid2, 22, 32); // clear title area
        Array.Copy(t2, 0, sid2, 22, t2.Length);

        File.WriteAllBytes(Path.Combine(tempDir, "aaa.sid"), sid1);
        File.WriteAllBytes(Path.Combine(tempDir, "bbb.sid"), sid2);

        SetFilename(fio, "*.sid");
        fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdDirOpen);
        Assert.AreEqual(VgcConstants.FioStatusOk, fio.Read((ushort)VgcConstants.FioStatus));

        // First entry metadata
        var title1 = Encoding.ASCII.GetString(memory, VgcConstants.MetaTitle, 10);
        Assert.AreEqual("First Song", title1);

        // Advance
        fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdDirRead);
        Assert.AreEqual(VgcConstants.FioStatusOk, fio.Read((ushort)VgcConstants.FioStatus));

        // Second entry metadata should replace first
        var title2 = Encoding.ASCII.GetString(memory, VgcConstants.MetaTitle, 11);
        Assert.AreEqual("Second Song", title2);
    }
    finally
    {
        Directory.Delete(tempDir, true);
    }
}

[TestMethod]
public void DirRead_Unfiltered_DoesNotPopulateMetadata()
{
    var (fio, memory, tempDir) = MakeControllerWithDevice();
    try
    {
        var sid = new byte[124];
        sid[0] = 0x50; sid[1] = 0x53; sid[2] = 0x49; sid[3] = 0x44;
        sid[4] = 0x00; sid[5] = 0x02;
        sid[6] = 0x00; sid[7] = 0x7C;
        var t = Encoding.ASCII.GetBytes("Should Not Appear");
        Array.Copy(t, 0, sid, 22, t.Length);
        File.WriteAllBytes(Path.Combine(tempDir, "test.sid"), sid);

        // Unfiltered: NAMELEN=0
        fio.Write((ushort)VgcConstants.FioNameLen, 0);
        fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdDirOpen);

        Assert.AreEqual(VgcConstants.FioStatusOk, fio.Read((ushort)VgcConstants.FioStatus));

        // Metadata buffer should remain zeroed
        Assert.AreEqual(0, memory[VgcConstants.MetaType]);
        Assert.AreEqual(0, memory[VgcConstants.MetaTitle]);
    }
    finally
    {
        Directory.Delete(tempDir, true);
    }
}
```

**Step 2: Run tests to verify they fail**

Run: `dotnet test --filter "SidMetadata|BinMetadata|Filtered_Populates|Unfiltered_DoesNot" --verbosity normal`
Expected: FAIL (PopulateMetadata is a stub)

**Step 3: Implement PopulateMetadata and extractors**

Replace the `PopulateMetadata` stub in `FileIoController.cs`:

```csharp
private void PopulateMetadata(StorageDirEntry entry)
{
    // Zero the entire metadata buffer
    for (int i = VgcConstants.MetaBase; i <= VgcConstants.MetaEnd; i++)
        _busWrite((ushort)i, 0);

    // Type
    int type = entry.FileType switch
    {
        NdiFileType.Sid => 1,
        NdiFileType.Bin => 2,
        NdiFileType.Mid => 3,
        NdiFileType.Gfx => 4,
        NdiFileType.Dir => 5,
        _ => 0
    };
    _busWrite((ushort)VgcConstants.MetaType, (byte)type);

    // Size
    _busWrite((ushort)VgcConstants.MetaSizeL, (byte)(entry.SizeBytes & 0xFF));
    _busWrite((ushort)VgcConstants.MetaSizeH, (byte)((entry.SizeBytes >> 8) & 0xFF));

    // Load file data for type-specific extraction
    byte[]? data = null;
    try
    {
        string ext = NdiTypeToExt(entry.FileType);
        if (_dirDevice is not null)
        {
            data = _dirDevice.Load(entry.Filename, ext);
        }
        else
        {
            string path = Path.Combine(_saveDir, entry.Filename + ext);
            if (File.Exists(path))
                data = File.ReadAllBytes(path);
        }
    }
    catch
    {
        // If we can't load the file, just leave metadata at defaults
        return;
    }

    if (data is null || data.Length == 0) return;

    switch (entry.FileType)
    {
        case NdiFileType.Sid:
            ExtractSidMetadata(data);
            break;
        case NdiFileType.Mid:
            ExtractMidiMetadata(data);
            break;
        case NdiFileType.Bin:
            ExtractBinMetadata(data);
            break;
        // BAS, GFX: just type+size, no additional metadata
    }
}

private void ExtractSidMetadata(byte[] data)
{
    if (data.Length < 124) return;

    // Title (32 bytes at offset 22)
    WriteMetaString(VgcConstants.MetaTitle, data, 22, VgcConstants.MetaTitleLen);
    // Author (32 bytes at offset 54)
    WriteMetaString(VgcConstants.MetaAuthor, data, 54, VgcConstants.MetaAuthorLen);
    // Copyright (32 bytes at offset 86)
    WriteMetaString(VgcConstants.MetaCopyright, data, 86, VgcConstants.MetaCopyrightLen);

    // Load address (big-endian at offset 8 → little-endian in buffer)
    _busWrite((ushort)VgcConstants.MetaLoadL, data[9]);  // BE low byte
    _busWrite((ushort)VgcConstants.MetaLoadH, data[8]);  // BE high byte

    // Init address (big-endian at offset 10)
    _busWrite((ushort)VgcConstants.MetaInitL, data[11]);
    _busWrite((ushort)VgcConstants.MetaInitH, data[10]);

    // Play address (big-endian at offset 12)
    _busWrite((ushort)VgcConstants.MetaPlayL, data[13]);
    _busWrite((ushort)VgcConstants.MetaPlayH, data[12]);

    // Song count (big-endian 16-bit at offset 14, take low byte)
    int songs = (data[14] << 8) | data[15];
    _busWrite((ushort)VgcConstants.MetaSongs, (byte)Math.Min(songs, 255));
}

private void ExtractMidiMetadata(byte[] data)
{
    try
    {
        using var stream = new MemoryStream(data);
        var midi = MidiFile.Read(stream);

        // Track count
        int trackCount = midi.GetTrackChunks().Count();
        _busWrite((ushort)VgcConstants.MetaSongs, (byte)Math.Min(trackCount, 255));

        // Title: first TrackName event
        string? title = null;
        string? copyright = null;
        foreach (var track in midi.GetTrackChunks())
        {
            foreach (var ev in track.Events)
            {
                if (title is null && ev is SequenceTrackNameEvent trackName)
                    title = trackName.Text;
                if (copyright is null && ev is CopyrightNoticeEvent crEvent)
                    copyright = crEvent.Text;
                if (title is not null && copyright is not null)
                    break;
            }
            if (title is not null && copyright is not null)
                break;
        }

        if (title is not null)
            WriteMetaStringFromManaged(VgcConstants.MetaTitle, title, VgcConstants.MetaTitleLen);
        if (copyright is not null)
        {
            WriteMetaStringFromManaged(VgcConstants.MetaAuthor, copyright, VgcConstants.MetaAuthorLen);
            WriteMetaStringFromManaged(VgcConstants.MetaCopyright, copyright, VgcConstants.MetaCopyrightLen);
        }

        // Duration
        var tempoMap = midi.GetTempoMap();
        var duration = midi.GetDuration<MetricTimeSpan>();
        int durationSecs = (int)duration.TotalSeconds;
        _busWrite((ushort)VgcConstants.MetaDurL, (byte)(durationSecs & 0xFF));
        _busWrite((ushort)VgcConstants.MetaDurH, (byte)((durationSecs >> 8) & 0xFF));
    }
    catch
    {
        // MIDI parsing failed — leave metadata at defaults
    }
}

private void ExtractBinMetadata(byte[] data)
{
    if (data.Length < 2) return;
    // First 2 bytes are load address (little-endian)
    _busWrite((ushort)VgcConstants.MetaLoadL, data[0]);
    _busWrite((ushort)VgcConstants.MetaLoadH, data[1]);
}

private void WriteMetaString(int destAddr, byte[] source, int srcOffset, int maxLen)
{
    int len = 0;
    while (len < maxLen && srcOffset + len < source.Length && source[srcOffset + len] != 0)
        len++;
    for (int i = 0; i < len; i++)
        _busWrite((ushort)(destAddr + i), source[srcOffset + i]);
    // Remaining bytes already zeroed by the buffer clear
}

private void WriteMetaStringFromManaged(int destAddr, string text, int maxLen)
{
    int len = Math.Min(text.Length, maxLen);
    for (int i = 0; i < len; i++)
        _busWrite((ushort)(destAddr + i), (byte)text[i]);
}
```

Also update `DoDirRead` to call `PopulateMetadata` when filtered:

In the existing `DoDirRead()` method (line 487), after `PopulateDirEntryFromStorage(_dirEntries[_dirIndex]);` add:
```csharp
if (_dirFiltered)
    PopulateMetadata(_dirEntries[_dirIndex]);
```

Add the necessary using statement at the top of `FileIoController.cs` if not present:
```csharp
using Melanchall.DryWetMidi.Core;
using Melanchall.DryWetMidi.Interaction;
```

**Step 4: Run tests to verify they pass**

Run: `dotnet test --filter "SidMetadata|BinMetadata|Filtered_Populates|Unfiltered_DoesNot" --verbosity normal`
Expected: All 4 tests PASS

**Step 5: Run all existing tests to check for regressions**

Run: `dotnet test --verbosity normal`
Expected: All tests PASS

**Step 6: Commit**

```bash
git add e6502.Avalonia/Hardware/FileIoController.cs e6502UnitTests/FileIoControllerTests.cs
git commit -m "feat: implement metadata extraction during filtered directory enumeration"
```

---

## Task 6: Add MIDI metadata extraction test

**Context:** Task 5 added MIDI metadata extraction but we only tested SID and BIN. We need a test that creates a minimal valid MIDI file with a track name and copyright event, then verifies the metadata buffer is populated correctly. Creating valid MIDI bytes by hand is fragile — use DryWetMidi to build the test file programmatically.

**Files:**
- Modify: `e6502UnitTests/FileIoControllerTests.cs`

**Step 1: Write the test**

```csharp
[TestMethod]
public void DirOpen_MidiMetadata_PopulatesBuffer()
{
    var (fio, memory, tempDir) = MakeControllerWithDevice();
    try
    {
        // Build a minimal MIDI file with DryWetMidi
        var trackChunk = new Melanchall.DryWetMidi.Core.TrackChunk(
            new Melanchall.DryWetMidi.Core.SequenceTrackNameEvent("Test MIDI Song"),
            new Melanchall.DryWetMidi.Core.CopyrightNoticeEvent("Test Composer"),
            new Melanchall.DryWetMidi.Core.NoteOnEvent((Melanchall.DryWetMidi.Common.SevenBitNumber)60, (Melanchall.DryWetMidi.Common.SevenBitNumber)100),
            new Melanchall.DryWetMidi.Core.NoteOffEvent((Melanchall.DryWetMidi.Common.SevenBitNumber)60, (Melanchall.DryWetMidi.Common.SevenBitNumber)0) { DeltaTime = 480 }
        );

        var midi = new Melanchall.DryWetMidi.Core.MidiFile(trackChunk);
        var midiPath = Path.Combine(tempDir, "test.mid");
        midi.Write(midiPath);

        SetFilename(fio, "*.mid");
        fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdDirOpen);

        Assert.AreEqual(VgcConstants.FioStatusOk, fio.Read((ushort)VgcConstants.FioStatus));

        // Type = MID (3)
        Assert.AreEqual(3, memory[VgcConstants.MetaType]);

        // Title
        var titleBytes = new byte[14];
        Array.Copy(memory, VgcConstants.MetaTitle, titleBytes, 0, 14);
        Assert.AreEqual("Test MIDI Song", Encoding.ASCII.GetString(titleBytes));

        // Author/Copyright
        var authorBytes = new byte[13];
        Array.Copy(memory, VgcConstants.MetaAuthor, authorBytes, 0, 13);
        Assert.AreEqual("Test Composer", Encoding.ASCII.GetString(authorBytes));

        // Track count
        Assert.AreEqual(1, memory[VgcConstants.MetaSongs]);

        // Duration should be > 0 (480 ticks at default 120 BPM = 1 second)
        int dur = memory[VgcConstants.MetaDurL] | (memory[VgcConstants.MetaDurH] << 8);
        Assert.IsTrue(dur > 0, $"Duration should be > 0, got {dur}");
    }
    finally
    {
        Directory.Delete(tempDir, true);
    }
}
```

**Step 2: Run test**

Run: `dotnet test --filter "MidiMetadata" --verbosity normal`
Expected: PASS

**Step 3: Commit**

```bash
git add e6502UnitTests/FileIoControllerTests.cs
git commit -m "test: add MIDI metadata extraction test"
```

---

## Task 7: Add new BASIC tokens to EhBASIC ROM

**Context:** We need 6 new extended tokens ($50-$55) in the ROM: DIROPEN (command), DIRNEXT (numeric function), DIRNAM$ (string function), DIRSIZ (numeric function), DIRTYP (numeric function), META$ (string function). This task adds the token definitions, keyword strings, and dispatch table entries. Handler code will be stubs initially (syntax error jumps), filled in by subsequent tasks.

The extended token system works as follows:
- `XTK_COUNT` (line 8656) defines the highest token ID — set to 85 ($55)
- `TAB_XTKSTR` (line 8658) is a word-pointer table indexed by (token_id - 1) mapping to keyword strings
- `TAB_XTKCMD` (line 1874) is a word-pointer table for command dispatch (indexed by token_id - 1)
- Function dispatch is via a CMP chain at line 3614-3648 in the expression evaluator

**Files:**
- Modify: `ehbasic/basic.asm`

**Step 1: Add token ID definitions**

After line 526 (`XTK_PWD = $4F`), add:

```asm
XTK_DIROPEN        = $50              ; DIROPEN "pattern" — filtered dir enumeration
XTK_DIRNEXT        = $51              ; DIRNEXT — numeric function (returns -1/0)
XTK_DIRNAM         = $52              ; DIRNAM$ — string function (current filename)
XTK_DIRSIZ         = $53              ; DIRSIZ — numeric function (current file size)
XTK_DIRTYP         = $54              ; DIRTYP — numeric function (current file type)
XTK_META           = $55              ; META$(n) — string function (metadata field)
```

**Step 2: Update XTK_COUNT**

Change line 8656 from:
```asm
XTK_COUNT = 79
```
to:
```asm
XTK_COUNT = 85
```

**Step 3: Add keyword strings**

After line 8683 (`@s_pwd: .byte "PWD",0`), add:

```asm
@s_diropen: .byte "DIROPEN",0
@s_dirnext: .byte "DIRNEXT",0
@s_dirnam:  .byte "DIRNAM$",0
@s_dirsiz:  .byte "DIRSIZ",0
@s_dirtyp:  .byte "DIRTYP",0
@s_meta:    .byte "META$(",0
```

Note: `META$(` includes the opening paren because it's a function that takes an argument (same pattern as `XPEEK(`, `MNOTE(`, `NRECV$(`, `NSTATUS(`).

**Step 4: Add keyword string pointers to TAB_XTKSTR**

After line 8683 (`.word @s_pwd`), add:

```asm
      .word @s_diropen, @s_dirnext, @s_dirnam, @s_dirsiz, @s_dirtyp, @s_meta
```

**Step 5: Add command dispatch table entries**

After line 1936 (`.word LAB_PWD-1`), add:

```asm
      .word LAB_DIROPEN-1      ; XTK_DIROPEN    ($50)
      .word LAB_15D9-1          ; XTK_DIRNEXT    ($51) — function only
      .word LAB_15D9-1          ; XTK_DIRNAM     ($52) — function only
      .word LAB_15D9-1          ; XTK_DIRSIZ     ($53) — function only
      .word LAB_15D9-1          ; XTK_DIRTYP     ($54) — function only
      .word LAB_15D9-1          ; XTK_META       ($55) — function only
```

(`LAB_15D9` is the syntax error handler, used for function-only tokens in the command table.)

**Step 6: Add stub command handler**

Add a stub `LAB_DIROPEN` near the other FIO handlers (e.g., near `LAB_DIR` around line 10701):

```asm
LAB_DIROPEN
      JMP   LAB_SNER          ; stub — will be implemented
```

**Step 7: Build the ROM**

Run: `cd ehbasic && make` (or whatever the build command is — check the Makefile)
Expected: Build succeeds

**Step 8: Commit**

```bash
git add ehbasic/basic.asm
git commit -m "feat: add 6 new BASIC tokens for directory enumeration and metadata"
```

---

## Task 8: Implement DIROPEN command handler in ROM

**Context:** DIROPEN parses a string expression, writes it to FIO_NAME/FIO_NAMELEN, and issues FIO_CMD_DIROPEN. This is similar to how MIDPLAY, SIDPLAY, CD, etc. work — they all parse a string argument and write it to FIO registers.

The pattern to follow is from `LAB_CD` or `LAB_MIDPLAY` — find one of these to see the exact calling convention for parsing a string and writing to FIO_NAME.

Key EhBASIC routines used:
- `LAB_EVEX` — evaluate expression, expect string
- `LAB_EVST` — get string pointer and length (A=length, ut1_pl/ut1_ph=pointer)
- FIO registers: `FIO_NAMELEN = $B9A3`, `FIO_NAME = $B9B0`, `FIO_CMD = $B9A0`

**Files:**
- Modify: `ehbasic/basic.asm` (replace LAB_DIROPEN stub)

**Step 1: Find the pattern**

Look at how `LAB_CD` works (around the CD token handler). It parses a string expression and copies it to FIO_NAME/FIO_NAMELEN, then issues a command. Use the same pattern.

**Step 2: Implement LAB_DIROPEN**

Replace the stub with:

```asm
LAB_DIROPEN
      JSR   LAB_EVEX          ; evaluate expression, must be string
      JSR   LAB_EVST          ; get string: A = length, (ut1_pl) = pointer
      TAX
      BEQ   @dir_empty        ; empty string = error
      CPX   #$40              ; max 63 chars
      BCS   @dir_err
      STX   FIO_NAMELEN
      LDY   #$00
@dir_cp
      LDA   (ut1_pl),Y
      STA   FIO_NAME,Y
      INY
      DEX
      BNE   @dir_cp
      LDA   #FIO_CMD_DIROPEN
      STA   FIO_CMD
      RTS

@dir_empty
@dir_err
      JMP   LAB_FCER          ; function call error
```

This is approximately 30 bytes.

**Step 3: Build the ROM**

Run: Build command for ROM
Expected: Build succeeds, ROM fits in 16KB

**Step 4: Commit**

```bash
git add ehbasic/basic.asm
git commit -m "feat: implement DIROPEN BASIC command handler"
```

---

## Task 9: Add function dispatch for DIRNEXT, DIRSIZ, DIRTYP

**Context:** Numeric functions need to be dispatched from the expression evaluator. The current dispatch is a CMP chain at lines 3614-3648 of `basic.asm`. We need to add entries for DIRNEXT ($51), DIRSIZ ($53), DIRTYP ($54), plus their handler code.

DIRNEXT: issues FIO_CMD_DIRREAD, checks FIO_STATUS, returns -1 (true, $FFFF) or 0 (false). Use BASIC boolean convention: `LDA #$FF; TAX` sets FAC1 to -1 (true).

DIRSIZ: reads FIO_SIZEL and FIO_SIZEH, loads as 16-bit unsigned into FAC1. Use the pattern from existing code that calls `LAB_STFA` to normalize a 16-bit integer into the floating-point accumulator.

DIRTYP: reads FIO_DIRTYPE as a single byte into FAC1. Use the `LAB_1FD0` (byte to FAC1) pattern if available, or the same LAB_STFA pattern with an 8-bit value.

**Files:**
- Modify: `ehbasic/basic.asm`

**Step 1: Add CMP entries to the function dispatch**

In the CMP chain at line 3614-3648, add before the `JMP LAB_SNER` at line 3648:

```asm
      CMP   #XTK_DIRNEXT
      BEQ   @j_dirnext
      CMP   #XTK_DIRSIZ
      BEQ   @j_dirsiz
      CMP   #XTK_DIRTYP
      BEQ   @j_dirtyp
```

And add the JMP trampolines near the existing ones (lines 3659-3672):

```asm
@j_dirnext
      JMP   @xtk_dirnext
@j_dirsiz
      JMP   @xtk_dirsiz
@j_dirtyp
      JMP   @xtk_dirtyp
```

**Step 2: Implement DIRNEXT handler**

Add near the other extended function handlers:

```asm
; DIRNEXT — issue DIRREAD, return -1 (true) if entry found, 0 (false) if done
@xtk_dirnext
      JSR   LAB_IGBY          ; consume token
      LDA   #FIO_CMD_DIRREAD
      STA   FIO_CMD
      LDA   FIO_STATUS
      CMP   #FIO_OK
      BNE   @dn_false
      ; return true (-1 = $FFFF)
      LDY   #$FF
      LDA   #$FF
      JMP   LAB_27DB          ; set FAC1 = signed integer from AY (A=high, Y=low)
@dn_false
      LDA   #$00
      TAY
      JMP   LAB_27DB          ; set FAC1 = 0
```

Note: `LAB_27DB` (or equivalent) converts a signed 16-bit integer in A(high)/Y(low) to FAC1. Check the existing code for the exact label — it's used by PLAYING, NSTATUS, etc. Look at how `@xtk_playing` returns its value.

**Step 3: Implement DIRSIZ handler**

```asm
; DIRSIZ — return file size as 16-bit unsigned integer
@xtk_dirsiz
      JSR   LAB_IGBY          ; consume token
      LDA   FIO_SIZEH         ; high byte
      LDX   FIO_SIZEL         ; low byte
      STA   FAC1_1
      STX   FAC1_2
      LDX   #$90              ; exponent for 16-bit integer
      SEC                     ; positive
      JMP   LAB_STFA          ; normalize into FAC1
```

This is the same pattern used by `LAB_DIR` to display file sizes (see lines 10713-10719).

**Step 4: Implement DIRTYP handler**

```asm
; DIRTYP — return file type as byte
@xtk_dirtyp
      JSR   LAB_IGBY          ; consume token
      LDA   FIO_DIRTYPE
      TAY
      LDA   #$00
      JMP   LAB_27DB          ; A=high=0, Y=low=type
```

**Step 5: Build the ROM**

Expected: Build succeeds

**Step 6: Commit**

```bash
git add ehbasic/basic.asm
git commit -m "feat: implement DIRNEXT, DIRSIZ, DIRTYP numeric functions"
```

---

## Task 10: Add function dispatch for DIRNAM$ and META$ (string functions)

**Context:** String functions need special handling — they allocate string space, copy data in, and return via `LAB_RTST`. The pattern is established by `NRECV$` (lines 3728-3746).

DIRNAM$: No arguments. Reads FIO_NAME (FIO_NAMELEN bytes), allocates a BASIC string, copies data in.

META$(n): Takes one numeric argument. Evaluates argument as byte offset, reads null-terminated string from $BAB0+offset. Allocates string, copies data in.

**Files:**
- Modify: `ehbasic/basic.asm`

**Step 1: Add CMP entries for string functions**

In the CMP chain (same location as Task 9), add:

```asm
      CMP   #XTK_DIRNAM
      BEQ   @j_dirnam
      CMP   #XTK_META
      BEQ   @j_meta
```

And trampolines:

```asm
@j_dirnam
      JMP   @xtk_dirnam
@j_meta
      JMP   @xtk_meta
```

**Step 2: Implement DIRNAM$**

```asm
; DIRNAM$ — return current directory entry filename as string
@xtk_dirnam
      JSR   LAB_IGBY          ; consume token
      LDA   FIO_NAMELEN
      BEQ   @dnam_empty
      JSR   LAB_MSSP          ; allocate string space (A = length), sets str_pl/ph
      LDY   #$00
@dnam_cp
      CPY   FIO_NAMELEN
      BCS   @dnam_done
      LDA   FIO_NAME,Y
      STA   (str_pl),Y
      INY
      BNE   @dnam_cp
@dnam_done
      STY   str_ln
      JMP   LAB_RTST          ; push descriptor, return
@dnam_empty
      LDA   #$00
      JSR   LAB_MSSP
      STA   str_ln
      JMP   LAB_RTST
```

Note: `LAB_MSSP` expects A = maximum length to allocate. `str_ln` is set to the actual length used. The pattern follows `NRECV$` closely.

**Step 3: Implement META$**

```asm
; META$(offset) — read null-terminated string from metadata buffer at $BAB0+offset
@xtk_meta
      JSR   LAB_IGBY          ; consume token (the extension id), advance to argument
      JSR   LAB_EVNM          ; evaluate numeric expression
      JSR   LAB_1BFB          ; scan for ')' and advance
      JSR   LAB_EVBY          ; convert FAC1 to byte in X
      ; X = offset into metadata buffer
      ; First, scan for null terminator to find length
      LDY   #$00
      STX   Ession_l          ; save offset in a temp (use a ZP temp like ut2_pl)
@meta_scan
      LDA   META_BASE,X
      BEQ   @meta_len
      INX
      INY
      CPY   #32               ; max 32 chars
      BCC   @meta_scan
@meta_len
      ; Y = string length
      TYA
      BEQ   @meta_empty
      JSR   LAB_MSSP          ; allocate Y bytes of string space
      LDX   Ession_l          ; restore offset
      LDY   #$00
@meta_cp
      LDA   META_BASE,X
      BEQ   @meta_done
      STA   (str_pl),Y
      INX
      INY
      BNE   @meta_cp
@meta_done
      STY   str_ln
      JMP   LAB_RTST
@meta_empty
      LDA   #$00
      JSR   LAB_MSSP
      STA   str_ln
      JMP   LAB_RTST
```

**Important:** Check that `META_BASE` ($BAB0) can be used as an absolute address for LDA. Since it's in the $BA range (below $C000 ROM), it's readable from the bus. Define `META_BASE = $BAB0` in the constants section of the assembly file.

Also, `Ession_l` or similar temp — find an available zero-page temporary. EhBASIC has `ut1_pl`/`ut1_ph` and `ut2_pl`/`ut2_ph` for temporaries. Use one that's not in use at this point. Check what the string function code path clobbers.

**Step 4: Build the ROM**

Expected: Build succeeds

**Step 5: Commit**

```bash
git add ehbasic/basic.asm
git commit -m "feat: implement DIRNAM$ and META$ string functions"
```

---

## Task 11: Extend DIR command with optional pattern argument

**Context:** The existing `LAB_DIR` (line 10701) immediately issues DIROPEN without checking for arguments. We need to add: if there's a string expression following DIR, parse it into FIO_NAME/FIO_NAMELEN first, otherwise set FIO_NAMELEN=0 (existing behavior).

EhBASIC's parser position is tracked by `Bpntrl`/`Bpntrh`. After the DIR token is consumed, the next byte indicates what follows. If it's a string delimiter (`"`) or a variable name, there's an argument. If it's `$00` (end of line) or `:` (statement separator), there's no argument.

The simplest check: use `LAB_GBYT` to peek at the current byte. If it's `$00` or `:`, skip to the no-argument path. Otherwise, try to parse a string expression.

**Files:**
- Modify: `ehbasic/basic.asm` (modify LAB_DIR)

**Step 1: Modify LAB_DIR**

Replace the start of `LAB_DIR` (line 10701) with:

```asm
LAB_DIR
      ; Check if there's a pattern argument
      JSR   LAB_GBYT          ; peek at current char
      BEQ   @dir_noarg        ; $00 = end of line, no argument
      CMP   #':'
      BEQ   @dir_noarg        ; : = statement separator, no argument
      ; Parse string expression into FIO_NAME/FIO_NAMELEN
      JSR   LAB_EVEX          ; evaluate expression, must be string
      JSR   LAB_EVST          ; get string: A = length, (ut1_pl) = pointer
      TAX
      BEQ   @dir_noarg        ; empty string = treat as no argument
      CPX   #$40
      BCS   @dir_noarg        ; too long = ignore
      STX   FIO_NAMELEN
      LDY   #$00
@dir_cp_pat
      LDA   (ut1_pl),Y
      STA   FIO_NAME,Y
      INY
      DEX
      BNE   @dir_cp_pat
      JMP   @dir_start
@dir_noarg
      LDA   #$00
      STA   FIO_NAMELEN
@dir_start
      ; trigger DirOpen (existing code continues from here)
      LDA   #FIO_CMD_DIROPEN
      STA   FIO_CMD
      ; ... rest of existing LAB_DIR code unchanged ...
```

The existing code from `LDA #FIO_CMD_DIROPEN` onwards remains the same.

**Step 2: Build the ROM**

Expected: Build succeeds

**Step 3: Commit**

```bash
git add ehbasic/basic.asm
git commit -m "feat: extend DIR command with optional wildcard pattern argument"
```

---

## Task 12: Add GFX file type to directory listing display

**Context:** The existing `TAB_DTYPE` table (line 10770) only has entries for BAS, SID, BIN, MID (indices 0-3). With `NdiFileType.Gfx = 4` and `NdiFileType.Dir = 5`, we need to add entries for GFX and DIR display strings so `DIR` doesn't crash when encountering these types.

**Files:**
- Modify: `ehbasic/basic.asm` (extend TAB_DTYPE)

**Step 1: Add missing type strings**

After line 10774 (`STR_MID .byte "  MID  ",$00`), add:

```asm
STR_GFX     .byte "  GFX  ",$00
STR_DIR     .byte "  DIR  ",$00
```

And update `TAB_DTYPE` (line 10770) to include the new entries:

```asm
TAB_DTYPE   .word STR_BAS, STR_SID, STR_BIN, STR_MID, STR_GFX, STR_DIR
```

**Step 2: Build the ROM**

Expected: Build succeeds

**Step 3: Commit**

```bash
git add ehbasic/basic.asm
git commit -m "feat: add GFX and DIR type strings to directory listing display"
```

---

## Task 13: Build ROM and verify size

**Context:** After all ROM changes, we need to verify the ROM still fits in 16KB (16384 bytes) and check how much space remains.

**Files:**
- None (verification only)

**Step 1: Build the ROM**

Run the ROM build command (check `ehbasic/Makefile` or equivalent).

**Step 2: Check ROM size**

Run: `wc -c ehbasic/basic.bin`
Expected: ≤ 16384 bytes

**Step 3: Check free space**

Run: `grep 'AA_end_basic' ehbasic/basic.sym`
Expected: Address should be ≤ $FFFA (leaving room for vectors at $FFFA-$FFFF)

Calculate remaining: $FFFA - end_address = free bytes.

**Step 4: If ROM is too large**

Optimization options:
- Share the string-to-FIO-NAME copy loop between DIROPEN and DIR (save ~15 bytes)
- Use a table-driven function dispatch instead of CMP chain (saves if many entries)
- Combine DIRSIZ and DIRTYP into one handler with different entry points

**Step 5: Commit the built ROM binary**

```bash
git add ehbasic/basic.bin ehbasic/basic.sym ehbasic/basic.map ehbasic/basic.o
git commit -m "chore: rebuild ROM with directory enumeration tokens"
```

---

## Task 14: End-to-end integration test

**Context:** We need to verify the complete flow works: DIROPEN with filter → DIRREAD advancing → metadata populated → end-of-dir detected. This is best tested at the C# level since we can't easily run the 6502 ROM in unit tests.

**Files:**
- Modify: `e6502UnitTests/FileIoControllerTests.cs`

**Step 1: Write comprehensive integration test**

```csharp
[TestMethod]
public void FilteredEnumeration_EndToEnd_MixedFileTypes()
{
    var (fio, memory, tempDir) = MakeControllerWithDevice();
    try
    {
        // Create a mix of files
        var sid = new byte[124];
        sid[0] = 0x50; sid[1] = 0x53; sid[2] = 0x49; sid[3] = 0x44;
        sid[4] = 0x00; sid[5] = 0x02;
        sid[6] = 0x00; sid[7] = 0x7C;
        var title = Encoding.ASCII.GetBytes("Cool SID Tune");
        Array.Copy(title, 0, sid, 22, title.Length);
        File.WriteAllBytes(Path.Combine(tempDir, "cool.sid"), sid);

        File.WriteAllBytes(Path.Combine(tempDir, "prog.bas"), new byte[] { 0x00, 0x00 });
        File.WriteAllBytes(Path.Combine(tempDir, "app.bin"), new byte[] { 0x00, 0x80, 0xEA });

        // Filter *.sid — should only get the SID file
        SetFilename(fio, "*.sid");
        fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdDirOpen);
        Assert.AreEqual(VgcConstants.FioStatusOk, fio.Read((ushort)VgcConstants.FioStatus));
        Assert.AreEqual("cool", ReadFilename(fio));
        Assert.AreEqual(1, memory[VgcConstants.MetaType]); // SID

        // Advance — should be end
        fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdDirRead);
        Assert.AreEqual(VgcConstants.FioErrEndOfDir, fio.Read((ushort)VgcConstants.FioErrCode));

        // Filter *.bin — should only get the BIN file
        SetFilename(fio, "*.bin");
        fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdDirOpen);
        Assert.AreEqual(VgcConstants.FioStatusOk, fio.Read((ushort)VgcConstants.FioStatus));
        Assert.AreEqual("app", ReadFilename(fio));
        Assert.AreEqual(2, memory[VgcConstants.MetaType]); // BIN
        Assert.AreEqual(0x00, memory[VgcConstants.MetaLoadL]);
        Assert.AreEqual(0x80, memory[VgcConstants.MetaLoadH]);

        // Filter * — should get all 3 files (no extension filter)
        SetFilename(fio, "*");
        fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdDirOpen);
        Assert.AreEqual(VgcConstants.FioStatusOk, fio.Read((ushort)VgcConstants.FioStatus));

        int count = 1; // first entry already loaded
        while (true)
        {
            fio.Write((ushort)VgcConstants.FioCmd, VgcConstants.FioCmdDirRead);
            if (fio.Read((ushort)VgcConstants.FioStatus) != VgcConstants.FioStatusOk)
                break;
            count++;
        }
        Assert.AreEqual(3, count);
    }
    finally
    {
        Directory.Delete(tempDir, true);
    }
}
```

**Step 2: Run**

Run: `dotnet test --filter "FilteredEnumeration_EndToEnd" --verbosity normal`
Expected: PASS

**Step 3: Commit**

```bash
git add e6502UnitTests/FileIoControllerTests.cs
git commit -m "test: add end-to-end integration test for filtered enumeration"
```

---

## Task 15: Run full test suite and verify

**Context:** Final verification that all existing tests still pass and no regressions were introduced.

**Step 1: Run all tests**

Run: `dotnet test --verbosity normal`
Expected: All tests PASS (should be ~870+ tests)

**Step 2: If any failures, fix them before proceeding**

Common issues:
- Existing DirOpen tests may need updating if they relied on FIO_NAMELEN being ignored
- The `PopulateDirEntry` for the filesystem fallback may need the `_dirFiltered` check added

**Step 3: Final commit if any fixes needed**

```bash
git add -u
git commit -m "fix: resolve test regressions from filtered enumeration changes"
```

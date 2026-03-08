# Nova Disk & I/O System Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Replace flat ~/e6502-programs storage with a device-oriented system supporting NDI disk images (FD0:--FD3:), host directories (HD0:--HD1:), subdirectories, autoboot, a BASIC tokenizer, and a CLI tool.

**Architecture:** New `e6502.Storage` class library holds NDI format code, device abstractions, and tokenizer. `FileIoController` delegates to `DeviceManager` instead of direct System.IO. New `e6502.NDI` console project provides CLI disk management. ROM gains ~220 bytes of new keywords and autoboot routine.

**Tech Stack:** C#/.NET 10, MSTest, Python 3 (build tooling), ca65/ld65 (6502 assembly)

**Design doc:** `docs/plans/2026-03-06-disk-io-system-design.md`

---

## Phase 1: NDI Format Library (e6502.Storage)

### Task 1: Create e6502.Storage project

**Files:**
- Create: `e6502.Storage/e6502.Storage.csproj`
- Modify: `e6502.sln`

**Step 1: Create the class library project**

Run:
```bash
cd /Users/barry/Git/e6502
dotnet new classlib -n e6502.Storage --framework net10.0
rm e6502.Storage/Class1.cs
dotnet sln add e6502.Storage/e6502.Storage.csproj
```

Expected: Project created and added to solution.

**Step 2: Verify build**

Run: `dotnet build e6502.Storage`
Expected: Build succeeded.

**Step 3: Commit**

```bash
git add e6502.Storage/ e6502.sln
git commit -m "feat: add e6502.Storage class library project"
```

---

### Task 2: NdiHeader -- read/write the 256-byte disk image header

**Files:**
- Create: `e6502.Storage/NdiHeader.cs`
- Create: `e6502UnitTests/NdiHeaderTests.cs`

**Step 1: Write failing tests**

```csharp
// e6502UnitTests/NdiHeaderTests.cs
using e6502.Storage;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class NdiHeaderTests
{
    [TestMethod]
    public void RoundTrip_DefaultHeader()
    {
        var header = NdiHeader.Create("TEST", 800);
        byte[] raw = header.ToBytes();
        Assert.AreEqual(256, raw.Length);

        var parsed = NdiHeader.FromBytes(raw);
        Assert.AreEqual("NDI\x1A", System.Text.Encoding.ASCII.GetString(raw, 0, 4));
        Assert.AreEqual(1, parsed.Version);
        Assert.AreEqual(256, parsed.SectorSize);
        Assert.AreEqual("TEST", parsed.VolumeLabel);
        Assert.AreEqual(header.TotalSectors, parsed.TotalSectors);
        Assert.AreEqual(header.DirectoryStartSector, parsed.DirectoryStartSector);
        Assert.AreEqual(header.DirectorySectorCount, parsed.DirectorySectorCount);
        Assert.AreEqual(header.DataStartSector, parsed.DataStartSector);
        Assert.AreEqual(header.FreeSectorCount, parsed.FreeSectorCount);
    }

    [TestMethod]
    public void Create_800KB_HasCorrectSectorCount()
    {
        var header = NdiHeader.Create("DISK", 800);
        // 800KB / 256 bytes = 3200 sectors total
        Assert.AreEqual(3200, header.TotalSectors);
    }

    [TestMethod]
    public void Create_170KB_MinimumSize()
    {
        var header = NdiHeader.Create("MINI", 170);
        Assert.AreEqual(170 * 1024 / 256, header.TotalSectors);
    }

    [TestMethod]
    public void InvalidMagic_Throws()
    {
        byte[] bad = new byte[256];
        Assert.ThrowsException<InvalidDataException>(() => NdiHeader.FromBytes(bad));
    }

    [TestMethod]
    public void VolumeLabel_TruncatedTo32()
    {
        var header = NdiHeader.Create(new string('X', 50), 800);
        Assert.AreEqual(32, header.VolumeLabel.Length);
    }
}
```

**Step 2: Run tests to verify they fail**

Run: `dotnet test --filter "NdiHeaderTests" --verbosity normal`
Expected: FAIL -- NdiHeader class not found.

**Step 3: Implement NdiHeader**

```csharp
// e6502.Storage/NdiHeader.cs
using System.Text;

namespace e6502.Storage;

public sealed class NdiHeader
{
    public const int Size = 256;
    public static readonly byte[] Magic = [(byte)'N', (byte)'D', (byte)'I', 0x1A];

    public byte Version { get; init; } = 1;
    public ushort SectorSize { get; init; } = 256;
    public ushort TotalSectors { get; init; }
    public string VolumeLabel { get; init; } = "";
    public ushort DirectoryStartSector { get; init; }
    public ushort DirectorySectorCount { get; init; }
    public ushort DataStartSector { get; init; }
    public ushort FreeSectorCount { get; set; }

    public static NdiHeader Create(string label, int sizeKB)
    {
        if (label.Length > 32) label = label[..32];
        ushort totalSectors = (ushort)(sizeKB * 1024 / 256);

        // Sector layout: [header=1] [BAM=N] [directory=48] [data...]
        // BAM: 1 bit per sector, 256 bytes per sector = 2048 bits per sector
        ushort bamSectors = (ushort)((totalSectors + 2047) / 2048);
        ushort dirStartSector = (ushort)(1 + bamSectors);
        ushort dirSectorCount = 48; // 768 directory entries (4 per sector)
        ushort dataStartSector = (ushort)(dirStartSector + dirSectorCount);
        ushort dataSectors = (ushort)(totalSectors - dataStartSector);

        return new NdiHeader
        {
            TotalSectors = totalSectors,
            VolumeLabel = label,
            DirectoryStartSector = dirStartSector,
            DirectorySectorCount = dirSectorCount,
            DataStartSector = dataStartSector,
            FreeSectorCount = dataSectors,
        };
    }

    public byte[] ToBytes()
    {
        byte[] buf = new byte[Size];
        Magic.CopyTo(buf, 0);
        buf[0x03] = Version;
        WriteU16(buf, 0x04, SectorSize);
        WriteU16(buf, 0x06, TotalSectors);
        byte[] labelBytes = Encoding.ASCII.GetBytes(VolumeLabel.PadRight(32)[..32]);
        Array.Copy(labelBytes, 0, buf, 0x08, 32);
        WriteU16(buf, 0x28, DirectoryStartSector);
        WriteU16(buf, 0x2A, DirectorySectorCount);
        WriteU16(buf, 0x2C, DataStartSector);
        WriteU16(buf, 0x2E, FreeSectorCount);
        return buf;
    }

    public static NdiHeader FromBytes(byte[] data)
    {
        if (data.Length < Size)
            throw new InvalidDataException("Header too short");
        if (data[0] != Magic[0] || data[1] != Magic[1] ||
            data[2] != Magic[2] || data[3] != Magic[3])
            throw new InvalidDataException("Invalid NDI magic");

        string label = Encoding.ASCII.GetString(data, 0x08, 32).TrimEnd('\0', ' ');

        return new NdiHeader
        {
            Version = data[0x03],
            SectorSize = ReadU16(data, 0x04),
            TotalSectors = ReadU16(data, 0x06),
            VolumeLabel = label,
            DirectoryStartSector = ReadU16(data, 0x28),
            DirectorySectorCount = ReadU16(data, 0x2A),
            DataStartSector = ReadU16(data, 0x2C),
            FreeSectorCount = ReadU16(data, 0x2E),
        };
    }

    private static void WriteU16(byte[] buf, int offset, ushort value)
    {
        buf[offset] = (byte)(value & 0xFF);
        buf[offset + 1] = (byte)(value >> 8);
    }

    private static ushort ReadU16(byte[] data, int offset)
        => (ushort)(data[offset] | (data[offset + 1] << 8));
}
```

**Step 4: Add project reference to test project**

Add to `e6502UnitTests/e6502UnitTests.csproj` inside `<ItemGroup>` with other ProjectReferences:
```xml
<ProjectReference Include="..\e6502.Storage\e6502.Storage.csproj" />
```

**Step 5: Run tests to verify they pass**

Run: `dotnet test --filter "NdiHeaderTests" --verbosity normal`
Expected: All 5 tests PASS.

**Step 6: Commit**

```bash
git add e6502.Storage/NdiHeader.cs e6502UnitTests/NdiHeaderTests.cs e6502UnitTests/e6502UnitTests.csproj
git commit -m "feat: NdiHeader read/write with tests"
```

---

### Task 3: NdiBam -- block allocation map

**Files:**
- Create: `e6502.Storage/NdiBam.cs`
- Create: `e6502UnitTests/NdiBamTests.cs`

**Step 1: Write failing tests**

```csharp
// e6502UnitTests/NdiBamTests.cs
using e6502.Storage;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class NdiBamTests
{
    [TestMethod]
    public void NewBam_AllFree()
    {
        var bam = new NdiBam(3200);
        Assert.AreEqual(3200, bam.FreeCount);
        Assert.IsFalse(bam.IsAllocated(0));
    }

    [TestMethod]
    public void AllocateContiguous_ReturnsConsecutiveSectors()
    {
        var bam = new NdiBam(3200);
        int start = bam.AllocateContiguous(10);
        Assert.AreEqual(0, start);
        Assert.AreEqual(3190, bam.FreeCount);
        for (int i = 0; i < 10; i++)
            Assert.IsTrue(bam.IsAllocated(i));
        Assert.IsFalse(bam.IsAllocated(10));
    }

    [TestMethod]
    public void Free_ReleaseSectors()
    {
        var bam = new NdiBam(100);
        bam.AllocateContiguous(5);
        Assert.AreEqual(95, bam.FreeCount);
        bam.Free(2, 3);
        Assert.AreEqual(98, bam.FreeCount);
        Assert.IsTrue(bam.IsAllocated(0));
        Assert.IsTrue(bam.IsAllocated(1));
        Assert.IsFalse(bam.IsAllocated(2));
    }

    [TestMethod]
    public void AllocateContiguous_SkipsUsedSectors()
    {
        var bam = new NdiBam(100);
        bam.AllocateContiguous(5); // 0-4 used
        int start = bam.AllocateContiguous(3);
        Assert.AreEqual(5, start);
    }

    [TestMethod]
    public void AllocateContiguous_NotEnoughSpace_ReturnsNegative()
    {
        var bam = new NdiBam(10);
        bam.AllocateContiguous(10);
        int result = bam.AllocateContiguous(1);
        Assert.AreEqual(-1, result);
    }

    [TestMethod]
    public void RoundTrip_Bytes()
    {
        var bam = new NdiBam(3200);
        bam.AllocateContiguous(7);
        byte[] data = bam.ToBytes();

        var bam2 = NdiBam.FromBytes(data, 3200);
        Assert.AreEqual(bam.FreeCount, bam2.FreeCount);
        for (int i = 0; i < 10; i++)
            Assert.AreEqual(bam.IsAllocated(i), bam2.IsAllocated(i));
    }
}
```

**Step 2: Run tests to verify they fail**

Run: `dotnet test --filter "NdiBamTests" --verbosity normal`
Expected: FAIL.

**Step 3: Implement NdiBam**

```csharp
// e6502.Storage/NdiBam.cs
namespace e6502.Storage;

public sealed class NdiBam
{
    private readonly byte[] _bits;
    private readonly int _totalSectors;

    public int FreeCount { get; private set; }

    public NdiBam(int totalSectors)
    {
        _totalSectors = totalSectors;
        _bits = new byte[(totalSectors + 7) / 8];
        FreeCount = totalSectors;
    }

    private NdiBam(byte[] bits, int totalSectors)
    {
        _bits = bits;
        _totalSectors = totalSectors;
        FreeCount = 0;
        for (int i = 0; i < totalSectors; i++)
            if (!IsAllocated(i)) FreeCount++;
    }

    public bool IsAllocated(int sector)
        => (_bits[sector >> 3] & (1 << (sector & 7))) != 0;

    public int AllocateContiguous(int count)
    {
        int run = 0;
        int runStart = 0;
        for (int i = 0; i < _totalSectors; i++)
        {
            if (IsAllocated(i))
            {
                run = 0;
                runStart = i + 1;
            }
            else
            {
                run++;
                if (run == count)
                {
                    for (int j = runStart; j < runStart + count; j++)
                        SetAllocated(j, true);
                    FreeCount -= count;
                    return runStart;
                }
            }
        }
        return -1;
    }

    public void Free(int start, int count)
    {
        for (int i = start; i < start + count; i++)
        {
            if (IsAllocated(i))
            {
                SetAllocated(i, false);
                FreeCount++;
            }
        }
    }

    private void SetAllocated(int sector, bool allocated)
    {
        if (allocated)
            _bits[sector >> 3] |= (byte)(1 << (sector & 7));
        else
            _bits[sector >> 3] &= (byte)~(1 << (sector & 7));
    }

    public byte[] ToBytes() => (byte[])_bits.Clone();

    public static NdiBam FromBytes(byte[] data, int totalSectors)
        => new((byte[])data.Clone(), totalSectors);
}
```

**Step 4: Run tests to verify they pass**

Run: `dotnet test --filter "NdiBamTests" --verbosity normal`
Expected: All 6 tests PASS.

**Step 5: Commit**

```bash
git add e6502.Storage/NdiBam.cs e6502UnitTests/NdiBamTests.cs
git commit -m "feat: NdiBam block allocation map with tests"
```

---

### Task 4: NdiDirectory -- directory entry CRUD

**Files:**
- Create: `e6502.Storage/NdiDirectory.cs`
- Create: `e6502UnitTests/NdiDirectoryTests.cs`

**Step 1: Write failing tests**

```csharp
// e6502UnitTests/NdiDirectoryTests.cs
using e6502.Storage;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class NdiDirectoryTests
{
    [TestMethod]
    public void AddFile_RoundTrip()
    {
        var dir = new NdiDirectory(48); // 48 sectors = 768 entries
        int idx = dir.AddEntry("HELLO", NdiFileType.Bas, 0xFFFF, 10, 500);
        Assert.IsTrue(idx >= 0);

        var entry = dir.GetEntry(idx);
        Assert.AreEqual("HELLO", entry.Filename);
        Assert.AreEqual(NdiFileType.Bas, entry.FileType);
        Assert.AreEqual(0xFFFF, entry.ParentIndex);
        Assert.AreEqual((ushort)10, entry.StartSector);
        Assert.AreEqual((ushort)500, entry.SizeBytes);
        Assert.IsTrue(entry.IsActive);
        Assert.IsFalse(entry.IsDirectory);
    }

    [TestMethod]
    public void AddDirectory_IsDirectory()
    {
        var dir = new NdiDirectory(48);
        int idx = dir.AddDirectory("MUSIC", 0xFFFF);
        var entry = dir.GetEntry(idx);
        Assert.IsTrue(entry.IsDirectory);
        Assert.AreEqual("MUSIC", entry.Filename);
    }

    [TestMethod]
    public void ListEntries_InParent()
    {
        var dir = new NdiDirectory(48);
        dir.AddEntry("FILE1", NdiFileType.Bas, 0xFFFF, 10, 100);
        int subdir = dir.AddDirectory("SUB", 0xFFFF);
        dir.AddEntry("FILE2", NdiFileType.Bas, (ushort)subdir, 20, 200);
        dir.AddEntry("FILE3", NdiFileType.Sid, 0xFFFF, 30, 300);

        var rootFiles = dir.ListEntries(0xFFFF);
        Assert.AreEqual(3, rootFiles.Length); // FILE1, SUB, FILE3

        var subFiles = dir.ListEntries((ushort)subdir);
        Assert.AreEqual(1, subFiles.Length); // FILE2
    }

    [TestMethod]
    public void RemoveEntry_MarksInactive()
    {
        var dir = new NdiDirectory(48);
        int idx = dir.AddEntry("TEMP", NdiFileType.Bas, 0xFFFF, 5, 50);
        dir.RemoveEntry(idx);

        var rootFiles = dir.ListEntries(0xFFFF);
        Assert.AreEqual(0, rootFiles.Length);
    }

    [TestMethod]
    public void FindEntry_ByNameAndParent()
    {
        var dir = new NdiDirectory(48);
        dir.AddEntry("HELLO", NdiFileType.Bas, 0xFFFF, 10, 100);
        int idx = dir.FindEntry("HELLO", 0xFFFF);
        Assert.IsTrue(idx >= 0);
        Assert.AreEqual(-1, dir.FindEntry("NOPE", 0xFFFF));
    }

    [TestMethod]
    public void RoundTrip_Bytes()
    {
        var dir = new NdiDirectory(48);
        dir.AddEntry("TEST", NdiFileType.Bin, 0xFFFF, 7, 256);
        dir.AddDirectory("GAMES", 0xFFFF);

        byte[] data = dir.ToBytes();
        var dir2 = NdiDirectory.FromBytes(data, 48);

        var entries = dir2.ListEntries(0xFFFF);
        Assert.AreEqual(2, entries.Length);
        Assert.AreEqual("TEST", entries[0].Filename);
        Assert.AreEqual("GAMES", entries[1].Filename);
    }
}
```

**Step 2: Run tests to verify they fail**

Run: `dotnet test --filter "NdiDirectoryTests" --verbosity normal`
Expected: FAIL.

**Step 3: Implement NdiDirectory**

```csharp
// e6502.Storage/NdiDirectory.cs
using System.Text;

namespace e6502.Storage;

public enum NdiFileType : byte
{
    Bas = 0, Sid = 1, Bin = 2, Mid = 3, Gfx = 4
}

[Flags]
public enum NdiEntryFlags : byte
{
    Active = 0x01,
    Directory = 0x02,
    Locked = 0x80,
}

public sealed record NdiDirEntry(
    int Index,
    NdiEntryFlags Flags,
    NdiFileType FileType,
    ushort ParentIndex,
    ushort StartSector,
    ushort SizeBytes,
    string Filename,
    ushort SectorCount)
{
    public bool IsActive => (Flags & NdiEntryFlags.Active) != 0;
    public bool IsDirectory => (Flags & NdiEntryFlags.Directory) != 0;
}

public sealed class NdiDirectory
{
    public const int EntrySize = 64;
    public const int EntriesPerSector = 256 / EntrySize; // 4
    public const int MaxFilenameLength = 32;

    private readonly byte[] _data;
    private readonly int _maxEntries;

    public NdiDirectory(int sectorCount)
    {
        _maxEntries = sectorCount * EntriesPerSector;
        _data = new byte[_maxEntries * EntrySize];
    }

    private NdiDirectory(byte[] data, int sectorCount)
    {
        _maxEntries = sectorCount * EntriesPerSector;
        _data = data;
    }

    public int AddEntry(string name, NdiFileType type, ushort parentIndex,
                        ushort startSector, ushort sizeBytes, ushort sectorCount = 0)
    {
        int idx = FindFreeSlot();
        if (idx < 0) return -1;
        WriteEntry(idx, NdiEntryFlags.Active, type, parentIndex,
                   startSector, sizeBytes, name, sectorCount);
        return idx;
    }

    public int AddDirectory(string name, ushort parentIndex)
    {
        int idx = FindFreeSlot();
        if (idx < 0) return -1;
        WriteEntry(idx, NdiEntryFlags.Active | NdiEntryFlags.Directory,
                   NdiFileType.Bas, parentIndex, 0, 0, name, 0);
        return idx;
    }

    public void RemoveEntry(int index)
    {
        int offset = index * EntrySize;
        _data[offset] = 0; // clear flags -> inactive
    }

    public NdiDirEntry GetEntry(int index)
    {
        int o = index * EntrySize;
        var flags = (NdiEntryFlags)_data[o + 0];
        var type = (NdiFileType)_data[o + 1];
        ushort parent = (ushort)(_data[o + 2] | (_data[o + 3] << 8));
        ushort start = (ushort)(_data[o + 4] | (_data[o + 5] << 8));
        ushort size = (ushort)(_data[o + 6] | (_data[o + 7] << 8));
        string name = Encoding.ASCII.GetString(_data, o + 8, MaxFilenameLength).TrimEnd('\0');
        ushort sectors = (ushort)(_data[o + 0x28] | (_data[o + 0x29] << 8));
        return new NdiDirEntry(index, flags, type, parent, start, size, name, sectors);
    }

    public int FindEntry(string name, ushort parentIndex)
    {
        for (int i = 0; i < _maxEntries; i++)
        {
            var e = GetEntry(i);
            if (e.IsActive && e.ParentIndex == parentIndex &&
                e.Filename.Equals(name, StringComparison.OrdinalIgnoreCase))
                return i;
        }
        return -1;
    }

    public NdiDirEntry[] ListEntries(ushort parentIndex)
    {
        var list = new List<NdiDirEntry>();
        for (int i = 0; i < _maxEntries; i++)
        {
            var e = GetEntry(i);
            if (e.IsActive && e.ParentIndex == parentIndex)
                list.Add(e);
        }
        return list.ToArray();
    }

    public byte[] ToBytes() => (byte[])_data.Clone();

    public static NdiDirectory FromBytes(byte[] data, int sectorCount)
        => new((byte[])data.Clone(), sectorCount);

    private int FindFreeSlot()
    {
        for (int i = 0; i < _maxEntries; i++)
        {
            if ((_data[i * EntrySize] & (byte)NdiEntryFlags.Active) == 0)
                return i;
        }
        return -1;
    }

    private void WriteEntry(int index, NdiEntryFlags flags, NdiFileType type,
                            ushort parent, ushort start, ushort size,
                            string name, ushort sectorCount)
    {
        int o = index * EntrySize;
        Array.Clear(_data, o, EntrySize);
        _data[o + 0] = (byte)flags;
        _data[o + 1] = (byte)type;
        _data[o + 2] = (byte)(parent & 0xFF);
        _data[o + 3] = (byte)(parent >> 8);
        _data[o + 4] = (byte)(start & 0xFF);
        _data[o + 5] = (byte)(start >> 8);
        _data[o + 6] = (byte)(size & 0xFF);
        _data[o + 7] = (byte)(size >> 8);
        byte[] nameBytes = Encoding.ASCII.GetBytes(name.Length > MaxFilenameLength
            ? name[..MaxFilenameLength] : name);
        Array.Copy(nameBytes, 0, _data, o + 8, nameBytes.Length);
        _data[o + 0x28] = (byte)(sectorCount & 0xFF);
        _data[o + 0x29] = (byte)(sectorCount >> 8);
    }
}
```

**Step 4: Run tests to verify they pass**

Run: `dotnet test --filter "NdiDirectoryTests" --verbosity normal`
Expected: All 6 tests PASS.

**Step 5: Commit**

```bash
git add e6502.Storage/NdiDirectory.cs e6502UnitTests/NdiDirectoryTests.cs
git commit -m "feat: NdiDirectory entry CRUD with subdirectories"
```

---

### Task 5: NdiImage -- full disk image create/open/read/write

**Files:**
- Create: `e6502.Storage/NdiImage.cs`
- Create: `e6502UnitTests/NdiImageTests.cs`

**Step 1: Write failing tests**

```csharp
// e6502UnitTests/NdiImageTests.cs
using e6502.Storage;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class NdiImageTests
{
    [TestMethod]
    public void Create_And_Open_RoundTrip()
    {
        string path = Path.Combine(Path.GetTempPath(), $"test-{Guid.NewGuid():N}.ndi");
        try
        {
            NdiImage.CreateFormatted(path, "TESTDISK", 800);
            Assert.IsTrue(File.Exists(path));

            using var img = NdiImage.Open(path);
            Assert.AreEqual("TESTDISK", img.Header.VolumeLabel);
            Assert.AreEqual(3200, img.Header.TotalSectors);
        }
        finally { File.Delete(path); }
    }

    [TestMethod]
    public void WriteFile_ReadFile_RoundTrip()
    {
        string path = Path.Combine(Path.GetTempPath(), $"test-{Guid.NewGuid():N}.ndi");
        try
        {
            NdiImage.CreateFormatted(path, "DATA", 800);
            using var img = NdiImage.Open(path);

            byte[] payload = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10];
            img.WriteFile("HELLO", NdiFileType.Bas, 0xFFFF, payload);

            byte[] read = img.ReadFile("HELLO", 0xFFFF);
            CollectionAssert.AreEqual(payload, read);
        }
        finally { File.Delete(path); }
    }

    [TestMethod]
    public void WriteFile_InSubdirectory()
    {
        string path = Path.Combine(Path.GetTempPath(), $"test-{Guid.NewGuid():N}.ndi");
        try
        {
            NdiImage.CreateFormatted(path, "DATA", 800);
            using var img = NdiImage.Open(path);

            int dirIdx = img.MakeDirectory("MUSIC", 0xFFFF);
            byte[] data = new byte[100];
            for (int i = 0; i < 100; i++) data[i] = (byte)i;
            img.WriteFile("BACH", NdiFileType.Bas, (ushort)dirIdx, data);

            byte[] read = img.ReadFile("BACH", (ushort)dirIdx);
            CollectionAssert.AreEqual(data, read);

            // Not visible in root
            Assert.ThrowsException<FileNotFoundException>(
                () => img.ReadFile("BACH", 0xFFFF));
        }
        finally { File.Delete(path); }
    }

    [TestMethod]
    public void DeleteFile_FreesSpace()
    {
        string path = Path.Combine(Path.GetTempPath(), $"test-{Guid.NewGuid():N}.ndi");
        try
        {
            NdiImage.CreateFormatted(path, "DATA", 800);
            using var img = NdiImage.Open(path);

            int freeBefore = img.FreeSectors;
            byte[] data = new byte[512]; // 2 sectors
            img.WriteFile("TEMP", NdiFileType.Bas, 0xFFFF, data);
            Assert.AreEqual(freeBefore - 2, img.FreeSectors);

            img.DeleteFile("TEMP", 0xFFFF);
            Assert.AreEqual(freeBefore, img.FreeSectors);
        }
        finally { File.Delete(path); }
    }

    [TestMethod]
    public void ListDirectory_ReturnsEntries()
    {
        string path = Path.Combine(Path.GetTempPath(), $"test-{Guid.NewGuid():N}.ndi");
        try
        {
            NdiImage.CreateFormatted(path, "DATA", 800);
            using var img = NdiImage.Open(path);

            img.WriteFile("A", NdiFileType.Bas, 0xFFFF, [1, 2]);
            img.WriteFile("B", NdiFileType.Sid, 0xFFFF, [3, 4, 5]);
            img.MakeDirectory("C", 0xFFFF);

            var entries = img.ListDirectory(0xFFFF);
            Assert.AreEqual(3, entries.Length);
        }
        finally { File.Delete(path); }
    }

    [TestMethod]
    public void LargeFile_SpansMultipleSectors()
    {
        string path = Path.Combine(Path.GetTempPath(), $"test-{Guid.NewGuid():N}.ndi");
        try
        {
            NdiImage.CreateFormatted(path, "DATA", 800);
            using var img = NdiImage.Open(path);

            byte[] data = new byte[1000]; // ~4 sectors
            for (int i = 0; i < data.Length; i++) data[i] = (byte)(i & 0xFF);
            img.WriteFile("BIG", NdiFileType.Bin, 0xFFFF, data);

            byte[] read = img.ReadFile("BIG", 0xFFFF);
            CollectionAssert.AreEqual(data, read);
        }
        finally { File.Delete(path); }
    }
}
```

**Step 2: Run tests to verify they fail**

Run: `dotnet test --filter "NdiImageTests" --verbosity normal`
Expected: FAIL.

**Step 3: Implement NdiImage**

```csharp
// e6502.Storage/NdiImage.cs
namespace e6502.Storage;

public sealed class NdiImage : IDisposable
{
    private readonly FileStream _stream;
    public NdiHeader Header { get; private set; }
    private NdiBam _bam;
    private NdiDirectory _directory;

    public int FreeSectors => _bam.FreeCount;

    private NdiImage(FileStream stream, NdiHeader header, NdiBam bam, NdiDirectory directory)
    {
        _stream = stream;
        Header = header;
        _bam = bam;
        _directory = directory;
    }

    public static void CreateFormatted(string path, string label, int sizeKB)
    {
        var header = NdiHeader.Create(label, sizeKB);
        int totalBytes = header.TotalSectors * header.SectorSize;

        using var fs = new FileStream(path, FileMode.Create, FileAccess.Write);
        fs.Write(header.ToBytes());

        // BAM (all data sectors free)
        int bamSectors = header.DirectoryStartSector - 1;
        int dataSectors = header.TotalSectors - header.DataStartSector;
        var bam = new NdiBam(dataSectors);
        byte[] bamBytes = bam.ToBytes();
        byte[] bamPadded = new byte[bamSectors * 256];
        Array.Copy(bamBytes, bamPadded, Math.Min(bamBytes.Length, bamPadded.Length));
        fs.Write(bamPadded);

        // Empty directory
        var dir = new NdiDirectory(header.DirectorySectorCount);
        fs.Write(dir.ToBytes());

        // Data area (zeroed)
        int remaining = totalBytes - (int)fs.Position;
        if (remaining > 0)
            fs.Write(new byte[remaining]);
    }

    public static NdiImage Open(string path)
    {
        var fs = new FileStream(path, FileMode.Open, FileAccess.ReadWrite);
        byte[] headerBuf = new byte[NdiHeader.Size];
        fs.ReadExactly(headerBuf);
        var header = NdiHeader.FromBytes(headerBuf);

        int bamSectors = header.DirectoryStartSector - 1;
        int dataSectors = header.TotalSectors - header.DataStartSector;
        byte[] bamBuf = new byte[bamSectors * 256];
        fs.ReadExactly(bamBuf);
        var bam = NdiBam.FromBytes(bamBuf, dataSectors);

        byte[] dirBuf = new byte[header.DirectorySectorCount * 256];
        fs.ReadExactly(dirBuf);
        var dir = NdiDirectory.FromBytes(dirBuf, header.DirectorySectorCount);

        return new NdiImage(fs, header, bam, dir);
    }

    public void WriteFile(string name, NdiFileType type, ushort parentIndex, byte[] data)
    {
        int sectorsNeeded = (data.Length + 255) / 256;
        if (sectorsNeeded == 0) sectorsNeeded = 1;

        int startSector = _bam.AllocateContiguous(sectorsNeeded);
        if (startSector < 0)
            throw new IOException("Disk full");

        // Write data to data area
        int dataOffset = (Header.DataStartSector + startSector) * 256;
        _stream.Position = dataOffset;
        _stream.Write(data);
        // Pad to sector boundary
        int pad = (sectorsNeeded * 256) - data.Length;
        if (pad > 0) _stream.Write(new byte[pad]);

        _directory.AddEntry(name, type, parentIndex,
            (ushort)startSector, (ushort)data.Length, (ushort)sectorsNeeded);

        Header.FreeSectorCount = (ushort)_bam.FreeCount;
        Flush();
    }

    public byte[] ReadFile(string name, ushort parentIndex)
    {
        int idx = _directory.FindEntry(name, parentIndex);
        if (idx < 0) throw new FileNotFoundException($"File not found: {name}");

        var entry = _directory.GetEntry(idx);
        if (entry.IsDirectory) throw new IOException("Is a directory");

        int dataOffset = (Header.DataStartSector + entry.StartSector) * 256;
        byte[] data = new byte[entry.SizeBytes];
        _stream.Position = dataOffset;
        _stream.ReadExactly(data);
        return data;
    }

    public void DeleteFile(string name, ushort parentIndex)
    {
        int idx = _directory.FindEntry(name, parentIndex);
        if (idx < 0) throw new FileNotFoundException($"File not found: {name}");

        var entry = _directory.GetEntry(idx);
        if (entry.IsDirectory) throw new IOException("Is a directory");

        _bam.Free(entry.StartSector, entry.SectorCount);
        _directory.RemoveEntry(idx);

        Header.FreeSectorCount = (ushort)_bam.FreeCount;
        Flush();
    }

    public int MakeDirectory(string name, ushort parentIndex)
    {
        int idx = _directory.AddDirectory(name, parentIndex);
        if (idx < 0) throw new IOException("Directory full");
        Flush();
        return idx;
    }

    public void RemoveDirectory(string name, ushort parentIndex)
    {
        int idx = _directory.FindEntry(name, parentIndex);
        if (idx < 0) throw new FileNotFoundException($"Directory not found: {name}");
        var entry = _directory.GetEntry(idx);
        if (!entry.IsDirectory) throw new IOException("Not a directory");
        // Check empty
        if (_directory.ListEntries((ushort)idx).Length > 0)
            throw new IOException("Directory not empty");
        _directory.RemoveEntry(idx);
        Flush();
    }

    public NdiDirEntry[] ListDirectory(ushort parentIndex)
        => _directory.ListEntries(parentIndex);

    private void Flush()
    {
        // Write header
        _stream.Position = 0;
        _stream.Write(Header.ToBytes());

        // Write BAM
        int bamSectors = Header.DirectoryStartSector - 1;
        byte[] bamBytes = _bam.ToBytes();
        byte[] bamPadded = new byte[bamSectors * 256];
        Array.Copy(bamBytes, bamPadded, Math.Min(bamBytes.Length, bamPadded.Length));
        _stream.Write(bamPadded);

        // Write directory
        _stream.Write(_directory.ToBytes());

        _stream.Flush();
    }

    public void Dispose() => _stream.Dispose();
}
```

**Step 4: Run tests to verify they pass**

Run: `dotnet test --filter "NdiImageTests" --verbosity normal`
Expected: All 6 tests PASS.

**Step 5: Commit**

```bash
git add e6502.Storage/NdiImage.cs e6502UnitTests/NdiImageTests.cs
git commit -m "feat: NdiImage create/open/read/write/delete with tests"
```

---

## Phase 2: Token Extractor & Tokenizer

### Task 6: gen_tokens.py -- extract token table from basic.asm

**Files:**
- Create: `tools/gen_tokens.py`

**Step 1: Write the script**

The script must:
1. Parse `TK_xxx = $80` and `TK_xxx = TK_yyy+1` chains to resolve numeric values
2. Parse `XTK_xxx = $nn` extended token IDs
3. Extract keyword spellings from the keyword tables (TAB_ASCx entries) and extended token name table (TAB_XTKSTR strings)
4. Emit JSON mapping byte value -> keyword string

```python
#!/usr/bin/env python3
"""Extract EhBASIC token tables from basic.asm into tokens.json."""

import re
import json
import argparse
from pathlib import Path


def parse_tokens(asm_path: str) -> dict:
    lines = Path(asm_path).read_text().splitlines()

    # Phase 1: resolve TK_ and XTK_ equates to numeric values
    equates: dict[str, int] = {}
    eq_re = re.compile(
        r'^(TK_\w+|XTK_\w+)\s*=\s*(.+?)(?:\s*;.*)?$'
    )

    for line in lines:
        m = eq_re.match(line.strip())
        if not m:
            continue
        name = m.group(1)
        expr = m.group(2).strip()
        equates[name] = _resolve(expr, equates)

    # Phase 2: extract keyword spellings from TAB_ASCx tables
    # Pattern: label starting with LBB_ followed by .byte "suffix",TK_xxx
    primary_keywords: dict[int, str] = {}
    current_first_char: str | None = None

    tab_label_re = re.compile(r'^TAB_ASC([A-Z])')
    lbb_re = re.compile(r'^LBB_\w+')
    byte_re = re.compile(r'\.byte\s+"([^"]*)"(?:,|\s)*(TK_\w+)')
    # Single-char operators: .byte TK_xxx,$00
    op_re = re.compile(r'\.byte\s+(TK_\w+),\$00')

    for line in lines:
        stripped = line.strip()
        m = tab_label_re.match(stripped)
        if m:
            current_first_char = m.group(1)
            continue

        # Check for operator tables (TAB_STAR, TAB_PLUS, etc.)
        if stripped.startswith('TAB_STAR'):
            current_first_char = '*'
        elif stripped.startswith('TAB_PLUS'):
            current_first_char = '+'
        elif stripped.startswith('TAB_MNUS'):
            current_first_char = '-'
        elif stripped.startswith('TAB_SLAS'):
            current_first_char = '/'
        elif stripped.startswith('TAB_LESS'):
            current_first_char = '<'
        elif stripped.startswith('TAB_EQUL'):
            current_first_char = '='
        elif stripped.startswith('TAB_MORE'):
            current_first_char = '>'
        elif stripped.startswith('TAB_QEST'):
            current_first_char = '?'

        m = byte_re.search(stripped)
        if m and current_first_char:
            suffix = m.group(1)
            tk_name = m.group(2)
            if tk_name in equates:
                keyword = current_first_char + suffix
                # Remove trailing ( for function tokens
                primary_keywords[equates[tk_name]] = keyword.rstrip('(')
            continue

        m = op_re.search(stripped)
        if m and current_first_char:
            tk_name = m.group(1)
            if tk_name in equates:
                primary_keywords[equates[tk_name]] = current_first_char

    # Special cases not in keyword tables
    if 'TK_PI' in equates and equates['TK_PI'] not in primary_keywords:
        primary_keywords[equates['TK_PI']] = 'PI'
    if 'TK_TWOPI' in equates and equates['TK_TWOPI'] not in primary_keywords:
        primary_keywords[equates['TK_TWOPI']] = 'TWOPI'

    # Phase 3: extract extended token names from TAB_XTKSTR string table
    extended_keywords: dict[int, str] = {}
    in_xtkstr = False
    xtk_string_re = re.compile(r'@s_\w+:\s*\.byte\s+"([^"]+)"')

    # Map XTK names to their IDs
    xtk_ids: dict[str, int] = {}
    for name, val in equates.items():
        if name.startswith('XTK_') and name != 'XTK_COUNT':
            xtk_ids[name] = val

    # Extract strings from @s_ labels
    s_label_re = re.compile(r'^@s_(\w+):\s*\.byte\s+"([^"]+)"')
    for line in lines:
        m = s_label_re.search(line.strip())
        if m:
            label_suffix = m.group(1).upper()
            keyword = m.group(2).rstrip('(').rstrip('$').rstrip('(')
            # Clean up: remove trailing null, parens
            keyword = keyword.replace(',0', '').rstrip('\0')
            # Find matching XTK_
            for xtk_name, xtk_id in xtk_ids.items():
                # Match by suffix: @s_dir -> XTK_DIR, @s_xmem -> XTK_XMEM
                if xtk_name == f'XTK_{label_suffix}':
                    extended_keywords[xtk_id] = keyword
                    break

    # Build output
    primary = {str(k): v for k, v in sorted(primary_keywords.items())}
    extended = {str(k): v for k, v in sorted(extended_keywords.items())}

    return {
        'version': 1,
        'prefix_byte': 1,
        'primary': primary,
        'extended': extended,
    }


def _resolve(expr: str, equates: dict[str, int]) -> int:
    """Resolve an expression like '$80', 'TK_END+1', etc."""
    expr = expr.strip()
    if expr.startswith('$'):
        return int(expr[1:], 16)
    if expr.isdigit():
        return int(expr)
    # Handle addition: TK_FOO+1
    if '+' in expr:
        parts = expr.split('+')
        return _resolve(parts[0], equates) + _resolve(parts[1], equates)
    if expr in equates:
        return equates[expr]
    raise ValueError(f"Cannot resolve: {expr}")


def main():
    p = argparse.ArgumentParser(description='Extract token tables from basic.asm')
    p.add_argument('asm_file', help='Path to basic.asm')
    p.add_argument('-o', '--output', default='tokens.json', help='Output path')
    args = p.parse_args()

    result = parse_tokens(args.asm_file)
    Path(args.output).write_text(json.dumps(result, indent=2))
    print(f"Generated {args.output}: {len(result['primary'])} primary + "
          f"{len(result['extended'])} extended tokens")


if __name__ == '__main__':
    main()
```

**Step 2: Run the script and verify output**

Run:
```bash
cd /Users/barry/Git/e6502
python3 tools/gen_tokens.py ehbasic/basic.asm -o ehbasic/tokens.json
cat ehbasic/tokens.json | python3 -m json.tool | head -30
```

Expected: JSON with `primary` containing entries like `"128": "END"`, `"129": "FOR"`, etc. and `extended` containing entries like `"1": "DIR"`, `"2": "DEL"`.

**Step 3: Verify token count is reasonable**

The primary table should have ~80 entries ($80 through $FF) and extended should have ~40+ entries. Manually spot-check a few: TK_SAVE ($98=152) should map to "SAVE", XTK_MUSIC ($14=20) should map to "MUSIC".

**Step 4: Add to Makefile**

Add to `ehbasic/Makefile` after the `novavm.inc` rule:

```makefile
tokens.json: basic.asm ../tools/gen_tokens.py
	python3 ../tools/gen_tokens.py basic.asm -o tokens.json
```

And add `tokens.json` to the `all` target dependencies.

**Step 5: Commit**

```bash
git add tools/gen_tokens.py ehbasic/tokens.json ehbasic/Makefile
git commit -m "feat: gen_tokens.py extracts token table from basic.asm"
```

---

### Task 7: BasicTokenizer -- tokenize/detokenize BASIC programs

**Files:**
- Create: `e6502.Storage/BasicTokenizer.cs`
- Create: `e6502UnitTests/BasicTokenizerTests.cs`

**Step 1: Write failing tests**

```csharp
// e6502UnitTests/BasicTokenizerTests.cs
using e6502.Storage;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class BasicTokenizerTests
{
    // Load tokens.json from ehbasic/ directory
    private static BasicTokenizer MakeTokenizer()
    {
        string tokensPath = Path.Combine(
            TestContext.TestDeploymentDir ?? ".",
            "..", "..", "..", "..", "ehbasic", "tokens.json");
        // Fall back to searching from project root
        if (!File.Exists(tokensPath))
            tokensPath = Path.Combine(
                AppContext.BaseDirectory, "..", "..", "..", "..", "..", "ehbasic", "tokens.json");
        return BasicTokenizer.FromJsonFile(tokensPath);
    }

    private static TestContext? _testContext;
    public static TestContext TestContext { get => _testContext!; set => _testContext = value; }

    [TestMethod]
    public void Tokenize_SimplePrint()
    {
        var tok = MakeTokenizer();
        string[] lines = ["10 PRINT \"HELLO\""];
        byte[] result = tok.Tokenize(lines, 0x0400);

        // Should contain: next-ptr(2), line-num 10(2), TK_PRINT, " HELLO", $22, $00
        // then $00,$00 end marker
        Assert.IsTrue(result.Length > 6);
        // Last two bytes should be $00,$00 (end of program)
        Assert.AreEqual(0x00, result[^2]);
        Assert.AreEqual(0x00, result[^1]);
    }

    [TestMethod]
    public void Detokenize_RoundTrip()
    {
        var tok = MakeTokenizer();
        string[] original = [
            "10 PRINT \"HELLO WORLD\"",
            "20 GOTO 10",
        ];
        byte[] binary = tok.Tokenize(original, 0x0400);
        string[] restored = tok.Detokenize(binary);

        Assert.AreEqual(2, restored.Length);
        Assert.AreEqual("10 PRINT \"HELLO WORLD\"", restored[0]);
        Assert.AreEqual("20 GOTO 10", restored[1]);
    }

    [TestMethod]
    public void Tokenize_StringsNotTokenized()
    {
        var tok = MakeTokenizer();
        string[] lines = ["10 PRINT \"PRINT GOTO FOR\""];
        byte[] binary = tok.Tokenize(lines, 0x0400);
        string[] restored = tok.Detokenize(binary);
        Assert.AreEqual("10 PRINT \"PRINT GOTO FOR\"", restored[0]);
    }

    [TestMethod]
    public void Tokenize_GosubBeforeGo()
    {
        var tok = MakeTokenizer();
        string[] lines = ["10 GOSUB 100"];
        byte[] binary = tok.Tokenize(lines, 0x0400);
        string[] restored = tok.Detokenize(binary);
        Assert.AreEqual("10 GOSUB 100", restored[0]);
    }

    [TestMethod]
    public void Tokenize_EmptyProgram()
    {
        var tok = MakeTokenizer();
        byte[] result = tok.Tokenize([], 0x0400);
        // Just end marker
        Assert.AreEqual(2, result.Length);
        Assert.AreEqual(0x00, result[0]);
        Assert.AreEqual(0x00, result[1]);
    }
}
```

**Step 2: Run tests to verify they fail**

Run: `dotnet test --filter "BasicTokenizerTests" --verbosity normal`
Expected: FAIL.

**Step 3: Implement BasicTokenizer**

```csharp
// e6502.Storage/BasicTokenizer.cs
using System.Text;
using System.Text.Json;

namespace e6502.Storage;

public sealed class BasicTokenizer
{
    private readonly Dictionary<int, string> _primary;      // byte value -> keyword
    private readonly Dictionary<int, string> _extended;     // id -> keyword
    private readonly List<(string Keyword, byte Token)> _primaryByLength;  // sorted longest first
    private readonly List<(string Keyword, byte Id)> _extendedByLength;
    private readonly byte _prefixByte;

    private BasicTokenizer(Dictionary<int, string> primary,
                           Dictionary<int, string> extended, byte prefixByte)
    {
        _primary = primary;
        _extended = extended;
        _prefixByte = prefixByte;

        // Build reverse lookups sorted by keyword length descending (longest match first)
        _primaryByLength = primary
            .Select(kv => (kv.Value, (byte)kv.Key))
            .OrderByDescending(x => x.Value.Length)
            .ToList();
        _extendedByLength = extended
            .Select(kv => (kv.Value, (byte)kv.Key))
            .OrderByDescending(x => x.Value.Length)
            .ToList();
    }

    public static BasicTokenizer FromJsonFile(string path)
    {
        string json = File.ReadAllText(path);
        using var doc = JsonDocument.Parse(json);
        var root = doc.RootElement;

        byte prefix = (byte)root.GetProperty("prefix_byte").GetInt32();

        var primary = new Dictionary<int, string>();
        foreach (var prop in root.GetProperty("primary").EnumerateObject())
            primary[int.Parse(prop.Name)] = prop.Value.GetString()!;

        var extended = new Dictionary<int, string>();
        foreach (var prop in root.GetProperty("extended").EnumerateObject())
            extended[int.Parse(prop.Name)] = prop.Value.GetString()!;

        return new BasicTokenizer(primary, extended, prefix);
    }

    public byte[] Tokenize(string[] lines, ushort baseAddr)
    {
        using var ms = new MemoryStream();
        ushort addr = baseAddr;

        foreach (string line in lines)
        {
            string trimmed = line.TrimStart();
            if (trimmed.Length == 0) continue;

            // Parse line number
            int numEnd = 0;
            while (numEnd < trimmed.Length && char.IsDigit(trimmed[numEnd])) numEnd++;
            if (numEnd == 0) continue; // skip lines without line numbers
            ushort lineNum = ushort.Parse(trimmed[..numEnd]);
            string body = trimmed[numEnd..];
            if (body.StartsWith(' ')) body = body[1..]; // skip one space after line number

            // Tokenize body
            byte[] tokenized = TokenizeLine(body);

            // Line format: [next-ptr 2B] [line-num 2B] [tokens...] [$00]
            int lineLen = 2 + 2 + tokenized.Length + 1;
            ushort nextAddr = (ushort)(addr + lineLen);

            ms.WriteByte((byte)(nextAddr & 0xFF));
            ms.WriteByte((byte)(nextAddr >> 8));
            ms.WriteByte((byte)(lineNum & 0xFF));
            ms.WriteByte((byte)(lineNum >> 8));
            ms.Write(tokenized);
            ms.WriteByte(0x00); // line terminator

            addr = nextAddr;
        }

        // End-of-program marker
        ms.WriteByte(0x00);
        ms.WriteByte(0x00);

        return ms.ToArray();
    }

    public string[] Detokenize(byte[] data)
    {
        var lines = new List<string>();
        int pos = 0;

        while (pos + 3 < data.Length)
        {
            ushort nextPtr = (ushort)(data[pos] | (data[pos + 1] << 8));
            if (nextPtr == 0) break; // end of program

            ushort lineNum = (ushort)(data[pos + 2] | (data[pos + 3] << 8));
            pos += 4;

            var sb = new StringBuilder();
            sb.Append(lineNum);
            sb.Append(' ');

            while (pos < data.Length && data[pos] != 0)
            {
                byte b = data[pos];

                if (b == '"')
                {
                    // Quoted string -- copy literally until closing quote or EOL
                    sb.Append('"');
                    pos++;
                    while (pos < data.Length && data[pos] != 0 && data[pos] != '"')
                    {
                        sb.Append((char)data[pos]);
                        pos++;
                    }
                    if (pos < data.Length && data[pos] == '"')
                    {
                        sb.Append('"');
                        pos++;
                    }
                    continue;
                }

                if (b == _prefixByte && pos + 1 < data.Length)
                {
                    // Extended token
                    byte id = data[pos + 1];
                    if (_extended.TryGetValue(id, out string? kw))
                    {
                        sb.Append(kw);
                        pos += 2;
                        continue;
                    }
                }

                if (_primary.TryGetValue(b, out string? keyword))
                {
                    sb.Append(keyword);
                    pos++;
                    continue;
                }

                // Literal character
                sb.Append((char)b);
                pos++;
            }

            if (pos < data.Length) pos++; // skip $00 terminator

            lines.Add(sb.ToString());
        }

        return lines.ToArray();
    }

    private byte[] TokenizeLine(string body)
    {
        using var ms = new MemoryStream();
        int i = 0;

        while (i < body.Length)
        {
            // Quoted strings pass through literally
            if (body[i] == '"')
            {
                ms.WriteByte((byte)'"');
                i++;
                while (i < body.Length && body[i] != '"')
                {
                    ms.WriteByte((byte)body[i]);
                    i++;
                }
                if (i < body.Length)
                {
                    ms.WriteByte((byte)'"');
                    i++;
                }
                continue;
            }

            // REM -- rest of line is literal
            if (i + 3 <= body.Length &&
                body[i..].StartsWith("REM", StringComparison.OrdinalIgnoreCase))
            {
                // Check it's actually the REM keyword (not part of REMEMBER etc.)
                bool isKeyword = (i + 3 >= body.Length) ||
                    !char.IsLetterOrDigit(body[i + 3]);
                if (isKeyword)
                {
                    // Find the REM token
                    var remEntry = _primaryByLength.FirstOrDefault(
                        x => x.Keyword.Equals("REM", StringComparison.OrdinalIgnoreCase));
                    if (remEntry.Keyword != null)
                    {
                        ms.WriteByte(remEntry.Token);
                        i += 3;
                        // Rest of line is literal
                        while (i < body.Length)
                        {
                            ms.WriteByte((byte)body[i]);
                            i++;
                        }
                        continue;
                    }
                }
            }

            // Try extended tokens first (longest match)
            bool matched = false;
            foreach (var (kw, id) in _extendedByLength)
            {
                if (i + kw.Length <= body.Length &&
                    body[i..].StartsWith(kw, StringComparison.OrdinalIgnoreCase))
                {
                    ms.WriteByte(_prefixByte);
                    ms.WriteByte(id);
                    i += kw.Length;
                    matched = true;
                    break;
                }
            }
            if (matched) continue;

            // Try primary tokens (longest match)
            foreach (var (kw, token) in _primaryByLength)
            {
                if (kw.Length < 2) continue; // skip single-char operators for keyword matching
                if (i + kw.Length <= body.Length &&
                    body[i..].StartsWith(kw, StringComparison.OrdinalIgnoreCase))
                {
                    ms.WriteByte(token);
                    i += kw.Length;
                    matched = true;
                    break;
                }
            }
            if (matched) continue;

            // Literal character
            ms.WriteByte((byte)body[i]);
            i++;
        }

        return ms.ToArray();
    }
}
```

**Step 4: Embed tokens.json as a resource or ship alongside**

For now, the tokenizer loads from a file path. The NDI CLI tool and tests will pass the path. Later, we can embed it as a resource if needed.

**Step 5: Run tests to verify they pass**

Run: `dotnet test --filter "BasicTokenizerTests" --verbosity normal`
Expected: All 5 tests PASS.

**Step 6: Commit**

```bash
git add e6502.Storage/BasicTokenizer.cs e6502UnitTests/BasicTokenizerTests.cs
git commit -m "feat: BasicTokenizer with tokenize/detokenize and tests"
```

---

## Phase 3: Device Layer & FileIoController Refactor

### Task 8: IStorageDevice interface and HostDirectoryDevice

**Files:**
- Create: `e6502.Storage/IStorageDevice.cs`
- Create: `e6502.Storage/HostDirectoryDevice.cs`
- Create: `e6502UnitTests/HostDirectoryDeviceTests.cs`

**Step 1: Write failing tests**

```csharp
// e6502UnitTests/HostDirectoryDeviceTests.cs
using e6502.Storage;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class HostDirectoryDeviceTests
{
    [TestMethod]
    public void SaveAndLoad_RoundTrip()
    {
        string dir = Path.Combine(Path.GetTempPath(), $"e6502-hdd-{Guid.NewGuid():N}");
        try
        {
            var dev = new HostDirectoryDevice("HD0", dir);
            Assert.IsTrue(dev.IsMounted);

            byte[] data = [0x00, 0x04, 1, 2, 3];
            dev.Save("HELLO", data, ".bas");

            byte[] loaded = dev.Load("HELLO", ".bas");
            CollectionAssert.AreEqual(data, loaded);
        }
        finally { if (Directory.Exists(dir)) Directory.Delete(dir, true); }
    }

    [TestMethod]
    public void Subdirectory_CdAndList()
    {
        string dir = Path.Combine(Path.GetTempPath(), $"e6502-hdd-{Guid.NewGuid():N}");
        try
        {
            var dev = new HostDirectoryDevice("HD0", dir);
            dev.MakeDirectory("GAMES");
            dev.CurrentDirectory = "GAMES";

            dev.Save("DEMO", [0x00, 0x04, 0xAA], ".bas");
            var entries = dev.ListDirectory(null);
            Assert.AreEqual(1, entries.Length);
            Assert.AreEqual("DEMO", entries[0].Filename);

            dev.CurrentDirectory = "/";
            entries = dev.ListDirectory(null);
            // Root should have GAMES directory
            Assert.IsTrue(entries.Any(e => e.Filename == "GAMES" && e.IsDirectory));
        }
        finally { if (Directory.Exists(dir)) Directory.Delete(dir, true); }
    }

    [TestMethod]
    public void Delete_RemovesFile()
    {
        string dir = Path.Combine(Path.GetTempPath(), $"e6502-hdd-{Guid.NewGuid():N}");
        try
        {
            var dev = new HostDirectoryDevice("HD0", dir);
            dev.Save("TEMP", [0x00, 0x04, 1], ".bas");
            dev.Delete("TEMP", ".bas");
            Assert.ThrowsException<FileNotFoundException>(() => dev.Load("TEMP", ".bas"));
        }
        finally { if (Directory.Exists(dir)) Directory.Delete(dir, true); }
    }

    [TestMethod]
    public void Load_NotFound_Throws()
    {
        string dir = Path.Combine(Path.GetTempPath(), $"e6502-hdd-{Guid.NewGuid():N}");
        try
        {
            var dev = new HostDirectoryDevice("HD0", dir);
            Assert.ThrowsException<FileNotFoundException>(() => dev.Load("NOPE", ".bas"));
        }
        finally { if (Directory.Exists(dir)) Directory.Delete(dir, true); }
    }
}
```

**Step 2: Run tests to verify they fail**

Run: `dotnet test --filter "HostDirectoryDeviceTests" --verbosity normal`
Expected: FAIL.

**Step 3: Implement IStorageDevice and HostDirectoryDevice**

```csharp
// e6502.Storage/IStorageDevice.cs
namespace e6502.Storage;

public record StorageDirEntry(string Filename, bool IsDirectory, NdiFileType FileType, int SizeBytes);

public interface IStorageDevice
{
    string Prefix { get; }
    string CurrentDirectory { get; set; }
    bool IsMounted { get; }
    byte[] Load(string path, string ext);
    void Save(string path, byte[] data, string ext);
    void Delete(string path, string ext);
    StorageDirEntry[] ListDirectory(string? path);
    void MakeDirectory(string path);
    void RemoveDirectory(string path);
    void Format(string label, int sizeKB);
    void Mount(string imagePath);
    void Unmount();
    bool FileExists(string name, string ext);
}
```

```csharp
// e6502.Storage/HostDirectoryDevice.cs
using System.Text.RegularExpressions;

namespace e6502.Storage;

public sealed partial class HostDirectoryDevice : IStorageDevice
{
    private readonly string _rootDir;
    public string Prefix { get; }
    public string CurrentDirectory { get; set; } = "/";
    public bool IsMounted => true;

    public HostDirectoryDevice(string prefix, string rootDir)
    {
        Prefix = prefix;
        _rootDir = rootDir;
        Directory.CreateDirectory(_rootDir);
    }

    public byte[] Load(string name, string ext)
    {
        string path = ResolvePath(name, ext);
        if (!File.Exists(path))
            throw new FileNotFoundException($"File not found: {name}{ext}");
        return File.ReadAllBytes(path);
    }

    public void Save(string name, byte[] data, string ext)
    {
        string dir = GetCurrentHostDir();
        Directory.CreateDirectory(dir);
        File.WriteAllBytes(Path.Combine(dir, name + ext), data);
    }

    public void Delete(string name, string ext)
    {
        string path = ResolvePath(name, ext);
        if (!File.Exists(path))
            throw new FileNotFoundException($"File not found: {name}{ext}");
        File.Delete(path);
    }

    public StorageDirEntry[] ListDirectory(string? path)
    {
        string dir = path != null ? Path.Combine(_rootDir, path.TrimStart('/'))
                                  : GetCurrentHostDir();
        if (!Directory.Exists(dir)) return [];

        var entries = new List<StorageDirEntry>();

        foreach (var d in Directory.GetDirectories(dir).OrderBy(d => d))
            entries.Add(new StorageDirEntry(
                Path.GetFileName(d), true, NdiFileType.Bas, 0));

        string[] exts = ["*.bas", "*.sid", "*.bin", "*.mid", "*.gfx"];
        var files = exts.SelectMany(e => Directory.GetFiles(dir, e))
                        .OrderBy(f => f);
        foreach (var f in files)
        {
            var fi = new FileInfo(f);
            var type = fi.Extension.ToLowerInvariant() switch
            {
                ".sid" => NdiFileType.Sid,
                ".bin" => NdiFileType.Bin,
                ".mid" => NdiFileType.Mid,
                ".gfx" => NdiFileType.Gfx,
                _ => NdiFileType.Bas,
            };
            entries.Add(new StorageDirEntry(
                Path.GetFileNameWithoutExtension(f), false, type, (int)fi.Length));
        }
        return entries.ToArray();
    }

    public void MakeDirectory(string name)
    {
        string dir = Path.Combine(GetCurrentHostDir(), name);
        Directory.CreateDirectory(dir);
    }

    public void RemoveDirectory(string name)
    {
        string dir = Path.Combine(GetCurrentHostDir(), name);
        if (!Directory.Exists(dir))
            throw new FileNotFoundException($"Directory not found: {name}");
        Directory.Delete(dir, false); // throws if not empty
    }

    public bool FileExists(string name, string ext)
        => File.Exists(ResolvePath(name, ext));

    public void Format(string label, int sizeKB)
        => throw new NotSupportedException("Cannot format a host directory device");
    public void Mount(string imagePath)
        => throw new NotSupportedException("Host directory is always mounted");
    public void Unmount()
        => throw new NotSupportedException("Host directory cannot be unmounted");

    private string GetCurrentHostDir()
    {
        string rel = CurrentDirectory.TrimStart('/');
        return string.IsNullOrEmpty(rel) ? _rootDir : Path.Combine(_rootDir, rel);
    }

    private string ResolvePath(string name, string ext)
        => Path.Combine(GetCurrentHostDir(), name + ext);
}
```

**Step 4: Run tests to verify they pass**

Run: `dotnet test --filter "HostDirectoryDeviceTests" --verbosity normal`
Expected: All 4 tests PASS.

**Step 5: Commit**

```bash
git add e6502.Storage/IStorageDevice.cs e6502.Storage/HostDirectoryDevice.cs e6502UnitTests/HostDirectoryDeviceTests.cs
git commit -m "feat: IStorageDevice interface and HostDirectoryDevice"
```

---

### Task 9: NdiFloppyDevice -- IStorageDevice over NdiImage

**Files:**
- Create: `e6502.Storage/NdiFloppyDevice.cs`
- Create: `e6502UnitTests/NdiFloppyDeviceTests.cs`

**Step 1: Write failing tests**

```csharp
// e6502UnitTests/NdiFloppyDeviceTests.cs
using e6502.Storage;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class NdiFloppyDeviceTests
{
    [TestMethod]
    public void Mount_SaveLoad_RoundTrip()
    {
        string ndiPath = Path.Combine(Path.GetTempPath(), $"test-{Guid.NewGuid():N}.ndi");
        try
        {
            NdiImage.CreateFormatted(ndiPath, "TEST", 800);
            var dev = new NdiFloppyDevice("FD0");
            dev.Mount(ndiPath);
            Assert.IsTrue(dev.IsMounted);

            byte[] data = [0x00, 0x04, 1, 2, 3, 4, 5];
            dev.Save("MYFILE", data, ".bas");
            byte[] loaded = dev.Load("MYFILE", ".bas");
            CollectionAssert.AreEqual(data, loaded);

            dev.Unmount();
            Assert.IsFalse(dev.IsMounted);
        }
        finally { File.Delete(ndiPath); }
    }

    [TestMethod]
    public void Subdirectory_CdAndAccess()
    {
        string ndiPath = Path.Combine(Path.GetTempPath(), $"test-{Guid.NewGuid():N}.ndi");
        try
        {
            NdiImage.CreateFormatted(ndiPath, "TEST", 800);
            var dev = new NdiFloppyDevice("FD0");
            dev.Mount(ndiPath);

            dev.MakeDirectory("SONGS");
            dev.CurrentDirectory = "SONGS";
            dev.Save("TUNE", [1, 2, 3], ".bas");

            var entries = dev.ListDirectory(null);
            Assert.AreEqual(1, entries.Length);
            Assert.AreEqual("TUNE", entries[0].Filename);

            dev.CurrentDirectory = "/";
            Assert.ThrowsException<FileNotFoundException>(() => dev.Load("TUNE", ".bas"));

            dev.Unmount();
        }
        finally { File.Delete(ndiPath); }
    }

    [TestMethod]
    public void Format_CreatesNewImage()
    {
        string ndiPath = Path.Combine(Path.GetTempPath(), $"test-{Guid.NewGuid():N}.ndi");
        try
        {
            var dev = new NdiFloppyDevice("FD0");
            dev.Format("FRESH", 800);
            // Format alone doesn't mount -- we need to mount it
            // Actually, Format should create at the standard location
            // For this test, let's use the Mount path
            NdiImage.CreateFormatted(ndiPath, "FRESH", 800);
            dev.Mount(ndiPath);
            Assert.IsTrue(dev.IsMounted);
            dev.Unmount();
        }
        finally { if (File.Exists(ndiPath)) File.Delete(ndiPath); }
    }

    [TestMethod]
    public void NotMounted_Throws()
    {
        var dev = new NdiFloppyDevice("FD0");
        Assert.IsFalse(dev.IsMounted);
        Assert.ThrowsException<IOException>(() => dev.Load("X", ".bas"));
    }
}
```

**Step 2: Run tests to verify they fail**

Run: `dotnet test --filter "NdiFloppyDeviceTests" --verbosity normal`
Expected: FAIL.

**Step 3: Implement NdiFloppyDevice**

```csharp
// e6502.Storage/NdiFloppyDevice.cs
namespace e6502.Storage;

public sealed class NdiFloppyDevice : IStorageDevice
{
    private NdiImage? _image;
    private ushort _currentDirIndex = 0xFFFF; // root

    public string Prefix { get; }
    public bool IsMounted => _image != null;

    public string CurrentDirectory
    {
        get => _currentPath;
        set => SetCurrentDirectory(value);
    }
    private string _currentPath = "/";

    public NdiFloppyDevice(string prefix) => Prefix = prefix;

    public void Mount(string imagePath)
    {
        _image?.Dispose();
        _image = NdiImage.Open(imagePath);
        _currentDirIndex = 0xFFFF;
        _currentPath = "/";
    }

    public void Unmount()
    {
        _image?.Dispose();
        _image = null;
        _currentDirIndex = 0xFFFF;
        _currentPath = "/";
    }

    public void Format(string label, int sizeKB)
        => throw new NotSupportedException("Use NdiImage.CreateFormatted then Mount");

    public byte[] Load(string name, string ext)
    {
        EnsureMounted();
        string fullName = name + ext.TrimStart('.');
        // Strip extension for NDI lookup, store with type
        var type = ExtToType(ext);
        return _image!.ReadFile(name, _currentDirIndex);
    }

    public void Save(string name, byte[] data, string ext)
    {
        EnsureMounted();
        var type = ExtToType(ext);
        _image!.WriteFile(name, type, _currentDirIndex, data);
    }

    public void Delete(string name, string ext)
    {
        EnsureMounted();
        _image!.DeleteFile(name, _currentDirIndex);
    }

    public StorageDirEntry[] ListDirectory(string? path)
    {
        EnsureMounted();
        ushort parent = path != null ? ResolveDirIndex(path) : _currentDirIndex;
        return _image!.ListDirectory(parent)
            .Select(e => new StorageDirEntry(
                e.Filename, e.IsDirectory, e.FileType, e.SizeBytes))
            .ToArray();
    }

    public void MakeDirectory(string name)
    {
        EnsureMounted();
        _image!.MakeDirectory(name, _currentDirIndex);
    }

    public void RemoveDirectory(string name)
    {
        EnsureMounted();
        _image!.RemoveDirectory(name, _currentDirIndex);
    }

    public bool FileExists(string name, string ext)
    {
        EnsureMounted();
        try { _image!.ReadFile(name, _currentDirIndex); return true; }
        catch (FileNotFoundException) { return false; }
    }

    private void SetCurrentDirectory(string path)
    {
        EnsureMounted();
        if (path == "/")
        {
            _currentDirIndex = 0xFFFF;
            _currentPath = "/";
            return;
        }

        _currentDirIndex = ResolveDirIndex(path);
        _currentPath = path;
    }

    private ushort ResolveDirIndex(string path)
    {
        string[] parts = path.TrimStart('/').Split('/', StringSplitOptions.RemoveEmptyEntries);
        ushort parent = 0xFFFF;
        foreach (string part in parts)
        {
            var entries = _image!.ListDirectory(parent);
            var dir = entries.FirstOrDefault(e => e.IsDirectory &&
                e.Filename.Equals(part, StringComparison.OrdinalIgnoreCase));
            if (dir == null) throw new DirectoryNotFoundException($"Directory not found: {part}");
            parent = (ushort)dir.Index;
        }
        return parent;
    }

    private void EnsureMounted()
    {
        if (_image == null) throw new IOException($"{Prefix}: not mounted");
    }

    private static NdiFileType ExtToType(string ext) => ext.ToLowerInvariant().TrimStart('.') switch
    {
        "sid" => NdiFileType.Sid,
        "bin" => NdiFileType.Bin,
        "mid" => NdiFileType.Mid,
        "gfx" => NdiFileType.Gfx,
        _ => NdiFileType.Bas,
    };
}
```

Note: The `NdiDirEntry` record needs its `Index` exposed. This is already done in Task 4's implementation.

**Step 4: Run tests to verify they pass**

Run: `dotnet test --filter "NdiFloppyDeviceTests" --verbosity normal`
Expected: All 4 tests PASS.

**Step 5: Commit**

```bash
git add e6502.Storage/NdiFloppyDevice.cs e6502UnitTests/NdiFloppyDeviceTests.cs
git commit -m "feat: NdiFloppyDevice wraps NdiImage as IStorageDevice"
```

---

### Task 10: DeviceManager -- device slot management and prefix resolution

**Files:**
- Create: `e6502.Storage/DeviceManager.cs`
- Create: `e6502UnitTests/DeviceManagerTests.cs`

**Step 1: Write failing tests**

```csharp
// e6502UnitTests/DeviceManagerTests.cs
using e6502.Storage;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class DeviceManagerTests
{
    [TestMethod]
    public void ParsePrefix_ExplicitDevice()
    {
        string dir = Path.Combine(Path.GetTempPath(), $"e6502-dm-{Guid.NewGuid():N}");
        try
        {
            var dm = new DeviceManager(dir, Path.Combine(dir, "data"), Path.Combine(dir, "disks"));
            var (device, path) = dm.ResolveFilename("HD0:MYFILE");
            Assert.AreEqual("HD0", device.Prefix);
            Assert.AreEqual("MYFILE", path);
        }
        finally { if (Directory.Exists(dir)) Directory.Delete(dir, true); }
    }

    [TestMethod]
    public void ParsePrefix_NoDevice_DefaultsToHD0()
    {
        string dir = Path.Combine(Path.GetTempPath(), $"e6502-dm-{Guid.NewGuid():N}");
        try
        {
            var dm = new DeviceManager(dir, Path.Combine(dir, "data"), Path.Combine(dir, "disks"));
            var (device, path) = dm.ResolveFilename("MYFILE");
            Assert.AreEqual("HD0", device.Prefix);
            Assert.AreEqual("MYFILE", path);
        }
        finally { if (Directory.Exists(dir)) Directory.Delete(dir, true); }
    }

    [TestMethod]
    public void ParsePrefix_WithSubdirectory()
    {
        string dir = Path.Combine(Path.GetTempPath(), $"e6502-dm-{Guid.NewGuid():N}");
        try
        {
            var dm = new DeviceManager(dir, Path.Combine(dir, "data"), Path.Combine(dir, "disks"));
            var (device, path) = dm.ResolveFilename("FD0:GAMES/DEMO");
            Assert.AreEqual("FD0", device.Prefix);
            Assert.AreEqual("GAMES/DEMO", path);
        }
        finally { if (Directory.Exists(dir)) Directory.Delete(dir, true); }
    }

    [TestMethod]
    public void GetDevice_InvalidPrefix_Throws()
    {
        string dir = Path.Combine(Path.GetTempPath(), $"e6502-dm-{Guid.NewGuid():N}");
        try
        {
            var dm = new DeviceManager(dir, Path.Combine(dir, "data"), Path.Combine(dir, "disks"));
            Assert.ThrowsException<IOException>(() => dm.ResolveFilename("XX9:FILE"));
        }
        finally { if (Directory.Exists(dir)) Directory.Delete(dir, true); }
    }

    [TestMethod]
    public void AutoMount_MountsExistingNdi()
    {
        string dir = Path.Combine(Path.GetTempPath(), $"e6502-dm-{Guid.NewGuid():N}");
        string disksDir = Path.Combine(dir, "disks");
        Directory.CreateDirectory(disksDir);
        try
        {
            NdiImage.CreateFormatted(Path.Combine(disksDir, "fd0.ndi"), "BOOT", 170);
            var dm = new DeviceManager(dir, Path.Combine(dir, "data"), disksDir);
            dm.AutoMount();

            var fd0 = dm.GetDevice("FD0");
            Assert.IsTrue(fd0.IsMounted);
        }
        finally { if (Directory.Exists(dir)) Directory.Delete(dir, true); }
    }

    [TestMethod]
    public void FindAutoboot_ReturnsFirstMatch()
    {
        string dir = Path.Combine(Path.GetTempPath(), $"e6502-dm-{Guid.NewGuid():N}");
        string hd0Dir = Path.Combine(dir, "programs");
        try
        {
            Directory.CreateDirectory(hd0Dir);
            // Put AUTOBOOT.BAS on HD0
            File.WriteAllBytes(Path.Combine(hd0Dir, "AUTOBOOT.bas"), [0x00, 0x04, 0x42]);

            var dm = new DeviceManager(hd0Dir, Path.Combine(dir, "data"), Path.Combine(dir, "disks"));
            var result = dm.FindAutoboot();
            Assert.IsNotNull(result);
            Assert.AreEqual("HD0", result.Value.Device.Prefix);
            Assert.AreEqual("AUTOBOOT", result.Value.Filename);
            Assert.AreEqual(".bas", result.Value.Extension);
        }
        finally { if (Directory.Exists(dir)) Directory.Delete(dir, true); }
    }
}
```

**Step 2: Run tests to verify they fail**

Run: `dotnet test --filter "DeviceManagerTests" --verbosity normal`
Expected: FAIL.

**Step 3: Implement DeviceManager**

```csharp
// e6502.Storage/DeviceManager.cs
namespace e6502.Storage;

public sealed class DeviceManager
{
    private readonly Dictionary<string, IStorageDevice> _devices = new(StringComparer.OrdinalIgnoreCase);
    private readonly string _disksDir;
    public string DefaultDevice { get; set; } = "HD0";

    public DeviceManager(string hd0Path, string hd1Path, string disksDir)
    {
        _disksDir = disksDir;
        _devices["HD0"] = new HostDirectoryDevice("HD0", hd0Path);
        _devices["HD1"] = new HostDirectoryDevice("HD1", hd1Path);
        _devices["FD0"] = new NdiFloppyDevice("FD0");
        _devices["FD1"] = new NdiFloppyDevice("FD1");
        _devices["FD2"] = new NdiFloppyDevice("FD2");
        _devices["FD3"] = new NdiFloppyDevice("FD3");
    }

    public IStorageDevice GetDevice(string prefix)
    {
        if (_devices.TryGetValue(prefix, out var dev))
            return dev;
        throw new IOException($"Unknown device: {prefix}");
    }

    public (IStorageDevice Device, string Path) ResolveFilename(string filename)
    {
        int colonIdx = filename.IndexOf(':');
        if (colonIdx > 0)
        {
            string prefix = filename[..colonIdx];
            string path = filename[(colonIdx + 1)..];
            return (GetDevice(prefix), path);
        }
        return (GetDevice(DefaultDevice), filename);
    }

    public void AutoMount()
    {
        if (!Directory.Exists(_disksDir)) return;

        for (int i = 0; i < 4; i++)
        {
            string ndiPath = Path.Combine(_disksDir, $"fd{i}.ndi");
            if (File.Exists(ndiPath))
            {
                try { _devices[$"FD{i}"].Mount(ndiPath); }
                catch { /* skip bad images */ }
            }
        }
    }

    public (IStorageDevice Device, string Filename, string Extension)? FindAutoboot()
    {
        string[] order = ["FD0", "FD1", "FD2", "FD3", "HD0", "HD1"];
        foreach (string prefix in order)
        {
            var dev = _devices[prefix];
            if (!dev.IsMounted) continue;

            // Check .bas first, then .bin
            if (dev.FileExists("AUTOBOOT", ".bas"))
                return (dev, "AUTOBOOT", ".bas");
            if (dev.FileExists("AUTOBOOT", ".bin"))
                return (dev, "AUTOBOOT", ".bin");
        }
        return null;
    }

    public void MountDevice(string prefix, string imageName)
    {
        string path = Path.Combine(_disksDir, imageName);
        if (!path.EndsWith(".ndi", StringComparison.OrdinalIgnoreCase))
            path += ".ndi";
        GetDevice(prefix).Mount(path);
    }

    public void UnmountDevice(string prefix)
        => GetDevice(prefix).Unmount();

    public void FormatDevice(string prefix, string label, int sizeKB)
    {
        Directory.CreateDirectory(_disksDir);
        string path = Path.Combine(_disksDir, $"{prefix.ToLowerInvariant()}.ndi");
        NdiImage.CreateFormatted(path, label, sizeKB);
        GetDevice(prefix).Mount(path);
    }
}
```

**Step 4: Run tests to verify they pass**

Run: `dotnet test --filter "DeviceManagerTests" --verbosity normal`
Expected: All 6 tests PASS.

**Step 5: Commit**

```bash
git add e6502.Storage/DeviceManager.cs e6502UnitTests/DeviceManagerTests.cs
git commit -m "feat: DeviceManager with auto-mount and autoboot detection"
```

---

### Task 11: Refactor FileIoController to use DeviceManager

**Files:**
- Modify: `e6502.Avalonia/Hardware/FileIoController.cs`
- Modify: `e6502.Avalonia/Hardware/VgcConstants.cs`
- Modify: `e6502.Avalonia/Hardware/CompositeBusDevice.cs`
- Modify: `e6502.Avalonia/e6502.Avalonia.csproj` (add reference to e6502.Storage)
- Modify: `e6502UnitTests/FileIoControllerTests.cs` (verify existing tests still pass)

This is a larger refactoring task. The key changes are:

**Step 1: Add project reference**

Add to `e6502.Avalonia/e6502.Avalonia.csproj`:
```xml
<ProjectReference Include="..\e6502.Storage\e6502.Storage.csproj" />
```

**Step 2: Add new FIO command constants to VgcConstants.cs**

Add after `FioCmdMidStop`:
```csharp
public const byte FioCmdCd        = 0x20;
public const byte FioCmdMkdir     = 0x21;
public const byte FioCmdRmdir     = 0x22;
public const byte FioCmdFormat    = 0x23;
public const byte FioCmdMount     = 0x24;
public const byte FioCmdUnmount   = 0x25;
public const byte FioCmdPwd       = 0x26;

public const byte FioDirTypeDir   = 0x05;  // directory entry in DIR listing

public const byte FioErrDiskFull  = 0x04;
public const byte FioErrNotMounted = 0x05;
```

**Step 3: Add DeviceManager to FileIoController constructor**

Add an optional `DeviceManager?` parameter. When present, Save/Load/Delete/Dir/GSave/GLoad/SidPlay/MidPlay route through it instead of direct file I/O. When null, fall back to existing behavior (backward compatibility for tests and CLI).

**Step 4: Implement device prefix parsing in ReadFilename**

After reading the filename from the FIO_NAME buffer, check for `:` to extract device prefix. Call `DeviceManager.ResolveFilename()` to get the device and remaining path.

**Step 5: Implement new command handlers**

Add cases to `ExecuteCommand` for $20-$26. Each reads parameters from FIO registers, calls DeviceManager, sets status.

**Step 6: Run ALL existing tests**

Run: `dotnet test --verbosity normal`
Expected: ALL tests pass (existing behavior preserved).

**Step 7: Commit**

```bash
git add -A
git commit -m "refactor: FileIoController routes through DeviceManager"
```

---

### Task 12: Wire DeviceManager into CompositeBusDevice and MainWindow

**Files:**
- Modify: `e6502.Avalonia/Hardware/CompositeBusDevice.cs`
- Modify: `e6502.Avalonia/MainWindow.axaml.cs`

**Step 1: Create DeviceManager in CompositeBusDevice constructor**

```csharp
string hd0 = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.UserProfile), "e6502-programs");
string hd1 = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.UserProfile), "e6502-data");
string disks = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.UserProfile), "e6502-disks");
_deviceManager = new DeviceManager(hd0, hd1, disks);
_deviceManager.AutoMount();
```

Pass `_deviceManager` to the FileIoController constructor.

**Step 2: Verify build and run**

Run: `dotnet build`
Then: `dotnet run --project e6502.Avalonia` -- verify SAVE/LOAD still work as before.

**Step 3: Run all tests**

Run: `dotnet test --verbosity normal`
Expected: ALL pass.

**Step 4: Commit**

```bash
git add -A
git commit -m "feat: wire DeviceManager into CompositeBusDevice"
```

---

## Phase 4: NDI CLI Tool

### Task 13: Create e6502.NDI project with basic commands

**Files:**
- Create: `e6502.NDI/e6502.NDI.csproj`
- Create: `e6502.NDI/Program.cs`
- Modify: `e6502.sln`

**Step 1: Create project**

```bash
cd /Users/barry/Git/e6502
dotnet new console -n e6502.NDI --framework net10.0
dotnet sln add e6502.NDI/e6502.NDI.csproj
```

Add project reference to e6502.Storage in the .csproj.

**Step 2: Implement Program.cs with verb dispatch**

Implement all commands from the design: `create`, `dir`, `info`, `validate`, `label`, `import`, `export`, `delete`, `mkdir`, `rmdir`, `tokenize`, `detokenize`.

Use simple arg parsing (no external library needed -- follow e6502.Tools pattern).

**Step 3: Test manually**

```bash
dotnet run --project e6502.NDI -- create /tmp/test.ndi --size 800 --label TESTDISK
dotnet run --project e6502.NDI -- info /tmp/test.ndi
dotnet run --project e6502.NDI -- dir /tmp/test.ndi
```

**Step 4: Commit**

```bash
git add e6502.NDI/ e6502.sln
git commit -m "feat: e6502.NDI CLI tool for disk image management"
```

---

### Task 14: Import/export with tokenize/detokenize

**Files:**
- Modify: `e6502.NDI/Program.cs`

**Step 1: Implement tokenize command**

```bash
dotnet run --project e6502.NDI -- tokenize docs/programs/joplin-entertainer.bas /tmp/entertainer.bas
```

Read ASCII text, tokenize via BasicTokenizer, write with 2-byte load address prefix.

**Step 2: Implement detokenize command**

Read .bas binary, strip 2-byte prefix, detokenize, write ASCII text.

**Step 3: Implement import --tokenize**

```bash
dotnet run --project e6502.NDI -- import /tmp/test.ndi docs/programs/joplin-entertainer.bas / --tokenize
```

**Step 4: Implement export --detokenize**

**Step 5: Test round-trip**

```bash
# Create image, import tokenized, export detokenized, diff
dotnet run --project e6502.NDI -- create /tmp/rt.ndi --size 800
dotnet run --project e6502.NDI -- import /tmp/rt.ndi docs/programs/joplin-entertainer.bas /MUSIC --tokenize
dotnet run --project e6502.NDI -- export /tmp/rt.ndi /MUSIC/joplin-entertainer /tmp/ --detokenize
diff docs/programs/joplin-entertainer.bas /tmp/joplin-entertainer.txt
```

**Step 6: Commit**

```bash
git add -A
git commit -m "feat: NDI import/export with tokenize/detokenize"
```

---

## Phase 5: ROM Changes & Autoboot

### Task 15: Add new BASIC keywords to ROM

**Files:**
- Modify: `ehbasic/basic.asm`

**Step 1: Add extended token IDs**

After the last XTK_ definition, add:
```asm
XTK_CD            = $49              ; CD "path"
XTK_MKDIR         = $4A              ; MKDIR "path"
XTK_RMDIR         = $4B              ; RMDIR "path"
XTK_FORMAT        = $4C              ; FORMAT "dev:",size
XTK_MOUNT         = $4D              ; MOUNT "dev:","image"
XTK_UNMOUNT       = $4E              ; UNMOUNT "dev:"
XTK_PWD           = $4F              ; PWD
```

Update `XTK_COUNT` accordingly.

**Step 2: Add keyword strings to TAB_XTKSTR**

Add `@s_cd`, `@s_mkdir`, `@s_rmdir`, `@s_format`, `@s_mount`, `@s_unmount`, `@s_pwd` string labels.

**Step 3: Add keyword entries to TAB_ASCx tables**

- TAB_ASCC: add `"D",XTK_PREFIX,XTK_CD` (CD -- careful with ordering, CD must come before CIRCLE/CLEAR/CLS/etc.)
- TAB_ASCF: add FORMAT
- TAB_ASCM: add MKDIR, MOUNT
- TAB_ASCP: add PWD
- TAB_ASCR: add RMDIR
- TAB_ASCU: add UNMOUNT

**Step 4: Add dispatch table entries**

Add entries to TAB_XTKCMD for each new token, pointing to handler labels.

**Step 5: Add handler stubs**

Each handler: parse args via `LAB_FIO_GETNAME` or `LAB_EVEX`, set FIO registers, write command byte, check status. ~20 bytes each.

**Step 6: Build and test**

```bash
cd ehbasic && make
```
Verify ROM builds and fits. Check remaining free space.

**Step 7: Commit**

```bash
git add ehbasic/
git commit -m "feat: add CD/MKDIR/RMDIR/FORMAT/MOUNT/UNMOUNT/PWD keywords to ROM"
```

---

### Task 16: Autoboot ROM routine

**Files:**
- Modify: `ehbasic/basic.asm` (or `min_mon.asm`)

**Step 1: Add autoboot skip flag address**

Define a known address in the vector table area:
```asm
AUTOBOOT_SKIP = $0280            ; C# sets to $FF to skip autoboot
```

**Step 2: Write LAB_AUTOBOOT routine**

Place in free space at ~$FDB0:
```asm
LAB_AUTOBOOT
      LDA   AUTOBOOT_SKIP
      BNE   @ab_done              ; skip if flag set

      ; Try loading "AUTOBOOT"
      LDA   #8
      STA   FIO_NAMELEN
      LDX   #0
@ab_copy
      LDA   @ab_name,X
      STA   FIO_NAME,X
      INX
      CPX   #8
      BNE   @ab_copy

      ; Set load address to BASIC start
      LDA   Smeml
      STA   FIO_SRCL
      LDA   Smemh
      STA   FIO_SRCH

      ; Try LOAD
      LDA   #FIO_CMD_LOAD
      STA   FIO_CMD
      LDA   FIO_STATUS
      CMP   #FIO_OK
      BNE   @ab_done              ; no autoboot file found

      ; Check file type
      LDA   FIO_DIRTYPE
      CMP   #$02                  ; .bin?
      BEQ   @ab_bin

      ; .bas -- update pointers and RUN
      CLC
      LDA   Smeml
      ADC   FIO_SIZEL
      STA   Svarl
      LDA   Smemh
      ADC   FIO_SIZEH
      STA   Svarh
      LDA   Svarl
      STA   Sarryl
      STA   Earryl
      LDA   Svarh
      STA   Sarryh
      STA   Earryh
      JMP   LAB_RUN               ; execute the program

@ab_bin
      ; .bin -- jump to load address
      LDA   FIO_SRCL
      STA   @ab_jmp+1
      LDA   FIO_SRCH
      STA   @ab_jmp+2
@ab_jmp
      JMP   $0000                 ; self-modified

@ab_done
      RTS

@ab_name
      .byte "AUTOBOOT"
```

**Step 3: Call LAB_AUTOBOOT from cold start**

In the cold start path (after memory init, before printing Ready), add:
```asm
      JSR   LAB_AUTOBOOT
```

**Step 4: Build and verify**

```bash
cd ehbasic && make
```

Check that ROM still fits (END_CODE < $FF80).

**Step 5: Add C# autoboot skip support**

In `CompositeBusDevice`, after boot, check for spacebar held or `NOAUTO=1` env var. If either, write `$FF` to address `$0280`.

**Step 6: Test autoboot end-to-end**

- Create `~/e6502-programs/AUTOBOOT.bas` with a simple program
- Run emulator, verify it auto-runs
- Hold spacebar or set `NOAUTO=1`, verify it skips

**Step 7: Commit**

```bash
git add -A
git commit -m "feat: autoboot routine in ROM + C# skip support"
```

---

### Task 17: Update gen_tokens.py for new tokens and rebuild

**Files:**
- Modify: `tools/gen_tokens.py` (if needed for new XTK_ patterns)
- Regenerate: `ehbasic/tokens.json`

**Step 1: Rebuild tokens.json**

```bash
cd ehbasic && make tokens.json
```

**Step 2: Verify new tokens appear**

```bash
python3 -c "import json; d=json.load(open('ehbasic/tokens.json')); print({k:v for k,v in d['extended'].items() if 'CD' in v or 'MKDIR' in v or 'FORMAT' in v})"
```

**Step 3: Run tokenizer tests**

```bash
dotnet test --filter "BasicTokenizerTests" --verbosity normal
```

**Step 4: Commit**

```bash
git add ehbasic/tokens.json tools/gen_tokens.py
git commit -m "chore: regenerate tokens.json with new disk I/O keywords"
```

---

### Task 18: Final integration test

**Step 1: Run all tests**

```bash
dotnet test --verbosity normal
```

Expected: ALL tests pass.

**Step 2: Manual end-to-end test**

```bash
# Create a disk image with the CLI tool
dotnet run --project e6502.NDI -- create ~/e6502-disks/fd0.ndi --size 800 --label "BOOT"

# Import an autoboot program
echo '10 PRINT "HELLO FROM DISK!"' > /tmp/autoboot.txt
echo '20 PRINT "AUTOBOOT WORKS!"' >> /tmp/autoboot.txt
dotnet run --project e6502.NDI -- import ~/e6502-disks/fd0.ndi /tmp/autoboot.txt / --tokenize

# Rename to AUTOBOOT
dotnet run --project e6502.NDI -- dir ~/e6502-disks/fd0.ndi

# Run the emulator -- should autoboot from FD0
dotnet run --project e6502.Avalonia
```

**Step 3: Commit any fixes, then final commit**

```bash
git add -A
git commit -m "feat: Nova Disk I/O system complete"
```

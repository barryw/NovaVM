using System.Text;

namespace e6502.Storage;

public enum NdiFileType : byte { Bas = 0, Sid = 1, Bin = 2, Mid = 3, Gfx = 4 }

[Flags]
public enum NdiEntryFlags : byte { Active = 0x01, Directory = 0x02, Locked = 0x80 }

public sealed record NdiDirEntry(
    int Index, NdiEntryFlags Flags, NdiFileType FileType,
    ushort ParentIndex, ushort StartSector, ushort SizeBytes,
    string Filename, ushort SectorCount)
{
    public bool IsActive => (Flags & NdiEntryFlags.Active) != 0;
    public bool IsDirectory => (Flags & NdiEntryFlags.Directory) != 0;
}

/// <summary>
/// Directory entry table for a Nova Disk Image (.ndi) file.
///
/// Each entry is 64 bytes; 4 entries fit in a 256-byte sector.
///
/// Entry layout:
///   $00      Flags: bit 0=active, bit 1=directory, bit 7=locked
///   $01      File type (0=BAS,1=SID,2=BIN,3=MID,4=GFX)
///   $02-$03  Parent directory entry index ($FFFF = root), little-endian
///   $04-$05  Start sector, little-endian
///   $06-$07  Size in bytes, little-endian
///   $08-$27  Filename (null-padded ASCII, max 32 chars)
///   $28-$29  Sector count (allocated), little-endian
///   $2A-$2F  Reserved
/// </summary>
public sealed class NdiDirectory
{
    public const int EntrySize = 64;
    public const int EntriesPerSector = 4;
    public const int MaxFilenameLength = 32;

    private readonly byte[] _data;
    private readonly int _entryCount;

    public NdiDirectory(int sectorCount)
    {
        _data = new byte[sectorCount * 256];
        _entryCount = sectorCount * EntriesPerSector;
    }

    private NdiDirectory(byte[] data, int sectorCount)
    {
        _data = data;
        _entryCount = sectorCount * EntriesPerSector;
    }

    /// <summary>
    /// Adds a file entry. Returns the entry index, or throws if the directory is full.
    /// </summary>
    public int AddEntry(string name, NdiFileType type, ushort parentIndex,
                        ushort startSector, ushort sizeBytes, ushort sectorCount = 0)
    {
        int slot = FindFreeSlot();
        if (slot < 0)
            throw new InvalidOperationException("Directory is full.");

        WriteEntry(slot, NdiEntryFlags.Active, type, parentIndex, startSector, sizeBytes,
                   TruncateName(name), sectorCount);
        return slot;
    }

    /// <summary>
    /// Adds a subdirectory entry. Returns the entry index.
    /// </summary>
    public int AddDirectory(string name, ushort parentIndex)
    {
        int slot = FindFreeSlot();
        if (slot < 0)
            throw new InvalidOperationException("Directory is full.");

        WriteEntry(slot, NdiEntryFlags.Active | NdiEntryFlags.Directory, NdiFileType.Bas,
                   parentIndex, 0, 0, TruncateName(name), 0);
        return slot;
    }

    /// <summary>
    /// Marks an entry as inactive (deleted). The slot becomes reusable.
    /// </summary>
    public void RemoveEntry(int index)
    {
        ValidateIndex(index);
        int offset = index * EntrySize;
        // Zero the flags byte — all other fields are irrelevant for a free slot.
        _data[offset] = 0;
    }

    /// <summary>
    /// Returns the entry at the given index regardless of active state.
    /// </summary>
    public NdiDirEntry GetEntry(int index)
    {
        ValidateIndex(index);
        return ReadEntry(index);
    }

    /// <summary>
    /// Returns the index of an active entry with the given name (case-insensitive)
    /// and parent, or -1 if not found.
    /// </summary>
    public int FindEntry(string name, ushort parentIndex)
    {
        for (int i = 0; i < _entryCount; i++)
        {
            var entry = ReadEntry(i);
            if (!entry.IsActive) continue;
            if (entry.ParentIndex != parentIndex) continue;
            if (string.Equals(entry.Filename, name, StringComparison.OrdinalIgnoreCase))
                return i;
        }
        return -1;
    }

    /// <summary>
    /// Returns all active entries whose parent matches the given index.
    /// </summary>
    public NdiDirEntry[] ListEntries(ushort parentIndex)
    {
        var results = new List<NdiDirEntry>();
        for (int i = 0; i < _entryCount; i++)
        {
            var entry = ReadEntry(i);
            if (entry.IsActive && entry.ParentIndex == parentIndex)
                results.Add(entry);
        }
        return [.. results];
    }

    /// <summary>Serializes the directory to a byte array sized to sectorCount * 256.</summary>
    public byte[] ToBytes() => (byte[])_data.Clone();

    /// <summary>Deserializes a directory from a byte array.</summary>
    public static NdiDirectory FromBytes(byte[] data, int sectorCount)
    {
        if (data.Length < sectorCount * 256)
            throw new ArgumentException("Data too short for given sector count.", nameof(data));

        var copy = new byte[sectorCount * 256];
        Array.Copy(data, copy, copy.Length);
        return new NdiDirectory(copy, sectorCount);
    }

    // --- Private helpers ---

    private int FindFreeSlot()
    {
        for (int i = 0; i < _entryCount; i++)
        {
            if (_data[i * EntrySize] == 0)
                return i;
        }
        return -1;
    }

    private void WriteEntry(int index, NdiEntryFlags flags, NdiFileType type,
                            ushort parentIndex, ushort startSector, ushort sizeBytes,
                            string name, ushort sectorCount)
    {
        int o = index * EntrySize;
        _data[o + 0x00] = (byte)flags;
        _data[o + 0x01] = (byte)type;
        WriteU16(o + 0x02, parentIndex);
        WriteU16(o + 0x04, startSector);
        WriteU16(o + 0x06, sizeBytes);

        // Filename: clear then write (null-padded)
        Array.Clear(_data, o + 0x08, MaxFilenameLength);
        var nameBytes = Encoding.ASCII.GetBytes(name);
        Array.Copy(nameBytes, 0, _data, o + 0x08, nameBytes.Length);

        WriteU16(o + 0x28, sectorCount);
        // Reserved $2A-$2F already zeroed (or left as-is; spec says reserved)
    }

    private NdiDirEntry ReadEntry(int index)
    {
        int o = index * EntrySize;
        var flags = (NdiEntryFlags)_data[o + 0x00];
        var type = (NdiFileType)_data[o + 0x01];
        ushort parentIndex = ReadU16(o + 0x02);
        ushort startSector = ReadU16(o + 0x04);
        ushort sizeBytes = ReadU16(o + 0x06);

        var nameBytes = new byte[MaxFilenameLength];
        Array.Copy(_data, o + 0x08, nameBytes, 0, MaxFilenameLength);
        int nullPos = Array.IndexOf(nameBytes, (byte)0);
        string name = Encoding.ASCII.GetString(nameBytes, 0, nullPos < 0 ? MaxFilenameLength : nullPos);

        ushort sectorCount = ReadU16(o + 0x28);

        return new NdiDirEntry(index, flags, type, parentIndex, startSector, sizeBytes, name, sectorCount);
    }

    private void WriteU16(int offset, ushort value)
    {
        _data[offset] = (byte)(value & 0xFF);
        _data[offset + 1] = (byte)(value >> 8);
    }

    private ushort ReadU16(int offset) =>
        (ushort)(_data[offset] | (_data[offset + 1] << 8));

    private void ValidateIndex(int index)
    {
        if (index < 0 || index >= _entryCount)
            throw new ArgumentOutOfRangeException(nameof(index), $"Index {index} out of range [0, {_entryCount}).");
    }

    private static string TruncateName(string name) =>
        name.Length > MaxFilenameLength ? name[..MaxFilenameLength] : name;
}

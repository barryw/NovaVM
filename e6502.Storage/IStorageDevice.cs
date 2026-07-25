namespace e6502.Storage;

public static class StorageAttributes
{
    public const byte ReadOnly = 0x01;
    public const byte Hidden = 0x02;
    public const byte System = 0x04;
    public const byte VolumeId = 0x08;
    public const byte Directory = 0x10;
    public const byte Archive = 0x20;
    public const byte AnyFile = 0x3F;

    public static byte FromFileAttributes(FileAttributes value)
    {
        byte result = 0;
        if ((value & FileAttributes.ReadOnly) != 0) result |= ReadOnly;
        if ((value & FileAttributes.Hidden) != 0) result |= Hidden;
        if ((value & FileAttributes.System) != 0) result |= System;
        if ((value & FileAttributes.Directory) != 0) result |= Directory;
        if ((value & FileAttributes.Archive) != 0) result |= Archive;
        return result;
    }

    public static FileAttributes ToFileAttributes(byte value)
    {
        FileAttributes result = 0;
        if ((value & ReadOnly) != 0) result |= FileAttributes.ReadOnly;
        if ((value & Hidden) != 0) result |= FileAttributes.Hidden;
        if ((value & System) != 0) result |= FileAttributes.System;
        if ((value & Archive) != 0) result |= FileAttributes.Archive;
        return result;
    }
}

public static class StorageTimestamp
{
    public static uint Pack(DateTime value)
    {
        int year = Math.Clamp(value.Year, 1980, 2107) - 1980;
        ushort date = (ushort)((year << 9) | (value.Month << 5) | value.Day);
        ushort time = (ushort)((value.Hour << 11) | (value.Minute << 5) | (value.Second / 2));
        return (uint)(date << 16) | time;
    }

    public static DateTime Unpack(uint value)
    {
        ushort date = (ushort)(value >> 16);
        ushort time = (ushort)value;
        int year = 1980 + ((date >> 9) & 0x7F);
        int month = Math.Clamp((date >> 5) & 0x0F, 1, 12);
        int day = Math.Clamp(date & 0x1F, 1, DateTime.DaysInMonth(year, month));
        int hour = Math.Min((time >> 11) & 0x1F, 23);
        int minute = Math.Min((time >> 5) & 0x3F, 59);
        int second = Math.Min((time & 0x1F) * 2, 59);
        return new DateTime(year, month, day, hour, minute, second, DateTimeKind.Local);
    }
}

public record StorageFileInfo(byte Attributes, uint PackedTimestamp, int SizeBytes);

public record StorageDirEntry(
    string Filename,
    bool IsDirectory,
    NdiFileType FileType,
    int SizeBytes,
    string Extension = "",
    byte Attributes = 0,
    uint PackedTimestamp = 0);

public interface IStorageDevice
{
    string Prefix { get; }
    string CurrentDirectory { get; set; }
    bool IsMounted { get; }
    byte[] Load(string name, string ext);
    void Save(string name, byte[] data, string ext);
    void Delete(string name, string ext);
    StorageDirEntry[] ListDirectory(string? path);
    void MakeDirectory(string name);
    void RemoveDirectory(string name);
    void Format(string label, int sizeKB);
    void Mount(string imagePath);
    void Unmount();
    bool FileExists(string name, string ext);
    uint FreeBytes { get; }
    uint CapacityBytes { get; }
    StorageFileInfo GetFileInfo(string name, string ext);
    void SetFileInfo(string name, string ext, byte attributes, uint packedTimestamp);
}

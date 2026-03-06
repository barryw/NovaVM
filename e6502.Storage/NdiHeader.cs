using System.Text;

namespace e6502.Storage;

/// <summary>
/// 256-byte header for Nova Disk Image (.ndi) files.
///
/// Layout (spec offset $03 overlaps the magic sentinel byte; we resolve this by
/// placing FormatVersion at $04 and shifting all subsequent fields by one byte):
///   $00-$03  Magic "NDI\x1A"
///   $04      Format version (1)
///   $05-$06  Sector size, little-endian (256)
///   $07-$08  Total sectors, little-endian
///   $09-$28  Volume label, 32 bytes, null-padded ASCII
///   $29-$2A  Directory start sector
///   $2B-$2C  Directory sector count
///   $2D-$2E  Data start sector
///   $2F-$30  Free sector count
///   $31-$FF  Reserved (zeroed)
/// </summary>
public sealed class NdiHeader
{
    public byte FormatVersion { get; init; } = 1;
    public ushort SectorSize { get; init; } = 256;
    public ushort TotalSectors { get; init; }
    public string VolumeLabel { get; init; } = string.Empty;
    public ushort DirectoryStartSector { get; init; }
    public ushort DirectorySectorCount { get; init; } = 48;
    public ushort DataStartSector { get; init; }
    public ushort FreeSectorCount { get; set; }

    /// <summary>
    /// Creates a new header for a disk of the given size in kilobytes.
    /// </summary>
    public static NdiHeader Create(string label, int sizeKB)
    {
        if (label.Length > 32)
            label = label[..32];

        int totalSectors = sizeKB * 1024 / 256;
        int bamSectors = (int)Math.Ceiling(totalSectors / 2048.0);
        ushort dirStart = (ushort)(1 + bamSectors);
        const ushort dirCount = 48;
        ushort dataStart = (ushort)(dirStart + dirCount);
        ushort free = (ushort)(totalSectors - dataStart);

        return new NdiHeader
        {
            TotalSectors = (ushort)totalSectors,
            VolumeLabel = label,
            DirectoryStartSector = dirStart,
            DirectorySectorCount = dirCount,
            DataStartSector = dataStart,
            FreeSectorCount = free,
        };
    }

    /// <summary>Serializes the header to exactly 256 bytes.</summary>
    public byte[] ToBytes()
    {
        var buf = new byte[256];

        buf[0] = (byte)'N';
        buf[1] = (byte)'D';
        buf[2] = (byte)'I';
        buf[3] = 0x1A;
        buf[4] = FormatVersion;
        WriteU16(buf, 5, SectorSize);
        WriteU16(buf, 7, TotalSectors);

        var labelBytes = Encoding.ASCII.GetBytes(VolumeLabel);
        Array.Copy(labelBytes, 0, buf, 9, Math.Min(labelBytes.Length, 32));

        WriteU16(buf, 0x29, DirectoryStartSector);
        WriteU16(buf, 0x2B, DirectorySectorCount);
        WriteU16(buf, 0x2D, DataStartSector);
        WriteU16(buf, 0x2F, FreeSectorCount);

        return buf;
    }

    /// <summary>
    /// Deserializes a header from 256 bytes. Throws <see cref="InvalidDataException"/>
    /// if the magic bytes are incorrect.
    /// </summary>
    public static NdiHeader FromBytes(byte[] data)
    {
        if (data.Length < 256)
            throw new InvalidDataException("NDI header too short.");

        if (data[0] != (byte)'N' || data[1] != (byte)'D' || data[2] != (byte)'I' || data[3] != 0x1A)
            throw new InvalidDataException("Invalid NDI magic bytes.");

        var labelBytes = new byte[32];
        Array.Copy(data, 9, labelBytes, 0, 32);
        int nullPos = Array.IndexOf(labelBytes, (byte)0);
        string label = Encoding.ASCII.GetString(labelBytes, 0, nullPos < 0 ? 32 : nullPos);

        return new NdiHeader
        {
            FormatVersion = data[4],
            SectorSize = ReadU16(data, 5),
            TotalSectors = ReadU16(data, 7),
            VolumeLabel = label,
            DirectoryStartSector = ReadU16(data, 0x29),
            DirectorySectorCount = ReadU16(data, 0x2B),
            DataStartSector = ReadU16(data, 0x2D),
            FreeSectorCount = ReadU16(data, 0x2F),
        };
    }

    private static void WriteU16(byte[] buf, int offset, ushort value)
    {
        buf[offset] = (byte)(value & 0xFF);
        buf[offset + 1] = (byte)(value >> 8);
    }

    private static ushort ReadU16(byte[] data, int offset) =>
        (ushort)(data[offset] | (data[offset + 1] << 8));
}

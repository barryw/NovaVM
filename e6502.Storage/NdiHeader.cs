using System.Text;

namespace e6502.Storage;

/// <summary>
/// 256-byte header for Nova Disk Image (.ndi) files.
///
/// Layout (spec offset $03 overlaps the magic sentinel byte; we resolve this by
/// placing FormatVersion at $04 and shifting all subsequent fields by one byte):
///   $00-$03  Magic "NDI\x1A"
///   $04      Format version (2)
///   $05-$06  Sector size, little-endian (256)
///   $07      Reserved (zero)
///   $08-$0B  Total sectors, little-endian
///   $0C-$2B  Volume label, 32 bytes, null-padded ASCII
///   $2C-$2F  Directory start sector
///   $30-$33  Directory sector count
///   $34-$37  Data start sector
///   $38-$3B  Free sector count
///   $3C-$FF  Reserved (zeroed)
/// </summary>
public sealed class NdiHeader
{
    public const int Size = 256;
    public const byte CurrentFormatVersion = 2;
    public const int LabelOffset = 0x0C;
    public const int LabelLength = 32;

    public byte FormatVersion { get; init; } = CurrentFormatVersion;
    public ushort SectorSize { get; init; } = 256;
    public uint TotalSectors { get; init; }
    public string VolumeLabel { get; init; } = string.Empty;
    public uint DirectoryStartSector { get; init; }
    public uint DirectorySectorCount { get; init; } = 48;
    public uint DataStartSector { get; init; }
    public uint FreeSectorCount { get; set; }

    /// <summary>
    /// Creates a new header for a disk of the given size in kilobytes.
    /// </summary>
    public static NdiHeader Create(string label, int sizeKB)
    {
        if (sizeKB < 1)
            throw new ArgumentOutOfRangeException(nameof(sizeKB), "Image size must be positive.");

        if (label.Length > 32)
            label = label[..32];

        const ushort sectorSize = 256;
        const uint dirCount = 48;
        const uint sectorsPerBamSector = sectorSize * 8u;

        long totalBytes = (long)sizeKB * 1024L;
        long totalSectorsLong = totalBytes / sectorSize;
        if (totalSectorsLong > uint.MaxValue)
            throw new ArgumentOutOfRangeException(nameof(sizeKB), "Image size exceeds NDI sector address range.");

        uint totalSectors = (uint)totalSectorsLong;
        uint bamSectors = (totalSectors + sectorsPerBamSector - 1) / sectorsPerBamSector;
        uint dirStart = 1 + bamSectors;
        uint dataStart = dirStart + dirCount;
        if (totalSectors <= dataStart)
            throw new ArgumentOutOfRangeException(nameof(sizeKB), "Image is too small for NDI metadata.");

        uint free = totalSectors - dataStart;

        return new NdiHeader
        {
            FormatVersion = CurrentFormatVersion,
            SectorSize = sectorSize,
            TotalSectors = totalSectors,
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
        var buf = new byte[Size];

        buf[0] = (byte)'N';
        buf[1] = (byte)'D';
        buf[2] = (byte)'I';
        buf[3] = 0x1A;
        buf[4] = FormatVersion;
        WriteU16(buf, 5, SectorSize);
        // $07 is reserved to keep 32-bit fields aligned.
        WriteU32(buf, 0x08, TotalSectors);

        var labelBytes = Encoding.ASCII.GetBytes(VolumeLabel);
        Array.Copy(labelBytes, 0, buf, LabelOffset, Math.Min(labelBytes.Length, LabelLength));

        WriteU32(buf, 0x2C, DirectoryStartSector);
        WriteU32(buf, 0x30, DirectorySectorCount);
        WriteU32(buf, 0x34, DataStartSector);
        WriteU32(buf, 0x38, FreeSectorCount);

        return buf;
    }

    /// <summary>
    /// Deserializes a header from 256 bytes. Throws <see cref="InvalidDataException"/>
    /// if the magic bytes are incorrect.
    /// </summary>
    public static NdiHeader FromBytes(byte[] data)
    {
        if (data.Length < Size)
            throw new InvalidDataException("NDI header too short.");

        if (data[0] != (byte)'N' || data[1] != (byte)'D' || data[2] != (byte)'I' || data[3] != 0x1A)
            throw new InvalidDataException("Invalid NDI magic bytes.");

        byte version = data[4];
        if (version != CurrentFormatVersion)
            throw new InvalidDataException($"Unsupported NDI format version {version}; expected {CurrentFormatVersion}.");

        var labelBytes = new byte[LabelLength];
        Array.Copy(data, LabelOffset, labelBytes, 0, LabelLength);
        int nullPos = Array.IndexOf(labelBytes, (byte)0);
        string label = Encoding.ASCII.GetString(labelBytes, 0, nullPos < 0 ? LabelLength : nullPos);

        ushort sectorSize = ReadU16(data, 5);
        if (sectorSize != 256)
            throw new InvalidDataException($"Unsupported NDI sector size {sectorSize}; expected 256.");

        uint totalSectors = ReadU32(data, 0x08);
        uint directoryStartSector = ReadU32(data, 0x2C);
        uint directorySectorCount = ReadU32(data, 0x30);
        uint dataStartSector = ReadU32(data, 0x34);
        uint freeSectorCount = ReadU32(data, 0x38);

        if (directoryStartSector < 1 ||
            dataStartSector <= directoryStartSector ||
            totalSectors < dataStartSector)
        {
            throw new InvalidDataException("Invalid NDI sector layout.");
        }

        return new NdiHeader
        {
            FormatVersion = version,
            SectorSize = sectorSize,
            TotalSectors = totalSectors,
            VolumeLabel = label,
            DirectoryStartSector = directoryStartSector,
            DirectorySectorCount = directorySectorCount,
            DataStartSector = dataStartSector,
            FreeSectorCount = freeSectorCount,
        };
    }

    private static void WriteU16(byte[] buf, int offset, ushort value)
    {
        buf[offset] = (byte)(value & 0xFF);
        buf[offset + 1] = (byte)(value >> 8);
    }

    private static ushort ReadU16(byte[] data, int offset) =>
        (ushort)(data[offset] | (data[offset + 1] << 8));

    private static void WriteU32(byte[] buf, int offset, uint value)
    {
        buf[offset] = (byte)(value & 0xFF);
        buf[offset + 1] = (byte)((value >> 8) & 0xFF);
        buf[offset + 2] = (byte)((value >> 16) & 0xFF);
        buf[offset + 3] = (byte)(value >> 24);
    }

    private static uint ReadU32(byte[] data, int offset) =>
        (uint)(data[offset] |
               (data[offset + 1] << 8) |
               (data[offset + 2] << 16) |
               (data[offset + 3] << 24));
}

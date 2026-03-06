namespace e6502.Storage;

/// <summary>
/// Composes NdiHeader, NdiBam, and NdiDirectory into a file-backed disk image.
///
/// File layout:
///   [Header: 1 sector] [BAM: N sectors] [Directory: 48 sectors] [Data: remaining sectors]
///
/// BAM tracks data sectors only (indexed from 0, physical offset = DataStartSector * 256).
/// </summary>
public sealed class NdiImage : IDisposable
{
    private NdiHeader _header;
    private readonly NdiBam _bam;
    private readonly NdiDirectory _directory;
    private readonly FileStream _stream;
    private bool _disposed;

    public NdiHeader Header => _header;
    public int FreeSectors => _bam.FreeCount;

    private NdiImage(NdiHeader header, NdiBam bam, NdiDirectory directory, FileStream stream)
    {
        _header = header;
        _bam = bam;
        _directory = directory;
        _stream = stream;
    }

    // -------------------------------------------------------------------------
    // Factory methods
    // -------------------------------------------------------------------------

    /// <summary>
    /// Creates a new, formatted NDI disk image at <paramref name="path"/>.
    /// Overwrites any existing file.
    /// </summary>
    public static void CreateFormatted(string path, string label, int sizeKB)
    {
        var header = NdiHeader.Create(label, sizeKB);
        int totalBytes = header.TotalSectors * 256;

        // Write a zeroed file of the correct total size.
        using var fs = new FileStream(path, FileMode.Create, FileAccess.Write);
        fs.SetLength(totalBytes);

        // Header (sector 0).
        fs.Seek(0, SeekOrigin.Begin);
        fs.Write(header.ToBytes());

        // BAM: sectors 1..(DirectoryStartSector-1), all zeroed (all free).
        int bamSectorCount = header.DirectoryStartSector - 1;
        int dataSectorCount = header.TotalSectors - header.DataStartSector;
        var bam = new NdiBam(dataSectorCount);
        byte[] bamBytes = bam.ToBytes();
        // Pad to full BAM sector coverage.
        byte[] bamPadded = new byte[bamSectorCount * 256];
        Array.Copy(bamBytes, bamPadded, Math.Min(bamBytes.Length, bamPadded.Length));
        fs.Seek(256, SeekOrigin.Begin);
        fs.Write(bamPadded);

        // Directory: NdiDirectory writes itself (already zeroed, but write explicitly).
        var directory = new NdiDirectory(header.DirectorySectorCount);
        fs.Seek(header.DirectoryStartSector * 256L, SeekOrigin.Begin);
        fs.Write(directory.ToBytes());
    }

    /// <summary>
    /// Opens an existing NDI disk image. Keeps the file open for data access.
    /// Caller must dispose the returned instance.
    /// </summary>
    public static NdiImage Open(string path)
    {
        var fs = new FileStream(path, FileMode.Open, FileAccess.ReadWrite, FileShare.None);

        // Read header.
        var headerBuf = new byte[256];
        fs.Seek(0, SeekOrigin.Begin);
        fs.ReadExactly(headerBuf);
        var header = NdiHeader.FromBytes(headerBuf);

        // Read BAM.
        int bamSectorCount = header.DirectoryStartSector - 1;
        int dataSectorCount = header.TotalSectors - header.DataStartSector;
        byte[] bamBuf = new byte[bamSectorCount * 256];
        fs.Seek(256, SeekOrigin.Begin);
        fs.ReadExactly(bamBuf);
        var bam = NdiBam.FromBytes(bamBuf, dataSectorCount);

        // Read directory.
        byte[] dirBuf = new byte[header.DirectorySectorCount * 256];
        fs.Seek(header.DirectoryStartSector * 256L, SeekOrigin.Begin);
        fs.ReadExactly(dirBuf);
        var directory = NdiDirectory.FromBytes(dirBuf, header.DirectorySectorCount);

        return new NdiImage(header, bam, directory, fs);
    }

    // -------------------------------------------------------------------------
    // File operations
    // -------------------------------------------------------------------------

    /// <summary>
    /// Writes <paramref name="data"/> as a new file. Allocates contiguous sectors,
    /// pads the last sector with zeros, and adds a directory entry.
    /// </summary>
    public void WriteFile(string name, NdiFileType type, ushort parentIndex, byte[] data)
    {
        ThrowIfDisposed();
        int sectorCount = (data.Length + 255) / 256;
        if (sectorCount == 0) sectorCount = 1;

        int startSector = _bam.AllocateContiguous(sectorCount);
        if (startSector < 0)
            throw new InvalidOperationException("Not enough contiguous free sectors.");

        // Write data sectors (padded to full sectors).
        long physicalOffset = (long)(_header.DataStartSector + startSector) * 256;
        _stream.Seek(physicalOffset, SeekOrigin.Begin);

        byte[] padded = new byte[sectorCount * 256];
        Array.Copy(data, padded, data.Length);
        _stream.Write(padded);

        _directory.AddEntry(name, type, parentIndex,
            (ushort)startSector, (ushort)data.Length, (ushort)sectorCount);

        Flush();
    }

    /// <summary>
    /// Reads a file by name and parent directory index.
    /// Returns exactly SizeBytes bytes.
    /// </summary>
    public byte[] ReadFile(string name, ushort parentIndex)
    {
        ThrowIfDisposed();
        int idx = _directory.FindEntry(name, parentIndex);
        if (idx < 0)
            throw new FileNotFoundException($"File '{name}' not found.");

        var entry = _directory.GetEntry(idx);
        if (entry.IsDirectory)
            throw new InvalidOperationException($"'{name}' is a directory, not a file.");

        long physicalOffset = (long)(_header.DataStartSector + entry.StartSector) * 256;
        _stream.Seek(physicalOffset, SeekOrigin.Begin);

        byte[] result = new byte[entry.SizeBytes];
        _stream.ReadExactly(result);
        return result;
    }

    /// <summary>
    /// Deletes a file: frees its BAM sectors and removes the directory entry.
    /// </summary>
    public void DeleteFile(string name, ushort parentIndex)
    {
        ThrowIfDisposed();
        int idx = _directory.FindEntry(name, parentIndex);
        if (idx < 0)
            throw new FileNotFoundException($"File '{name}' not found.");

        var entry = _directory.GetEntry(idx);
        if (entry.IsDirectory)
            throw new InvalidOperationException($"'{name}' is a directory. Use RemoveDirectory.");

        _bam.Free(entry.StartSector, entry.SectorCount);
        _directory.RemoveEntry(idx);
        Flush();
    }

    /// <summary>
    /// Creates a subdirectory entry. Returns the new entry's index (used as parentIndex for children).
    /// </summary>
    public int MakeDirectory(string name, ushort parentIndex)
    {
        ThrowIfDisposed();
        int idx = _directory.AddDirectory(name, parentIndex);
        Flush();
        return idx;
    }

    /// <summary>
    /// Removes an empty directory. Throws if the directory contains any active entries.
    /// </summary>
    public void RemoveDirectory(string name, ushort parentIndex)
    {
        ThrowIfDisposed();
        int idx = _directory.FindEntry(name, parentIndex);
        if (idx < 0)
            throw new FileNotFoundException($"Directory '{name}' not found.");

        var entry = _directory.GetEntry(idx);
        if (!entry.IsDirectory)
            throw new InvalidOperationException($"'{name}' is not a directory.");

        // Check for children.
        var children = _directory.ListEntries((ushort)idx);
        if (children.Length > 0)
            throw new InvalidOperationException($"Directory '{name}' is not empty.");

        _directory.RemoveEntry(idx);
        Flush();
    }

    /// <summary>
    /// Lists all active entries whose parent is <paramref name="parentIndex"/>.
    /// Use 0xFFFF for the root directory.
    /// </summary>
    public NdiDirEntry[] ListDirectory(ushort parentIndex)
    {
        ThrowIfDisposed();
        return _directory.ListEntries(parentIndex);
    }

    // -------------------------------------------------------------------------
    // Internal helpers
    // -------------------------------------------------------------------------

    /// <summary>
    /// Writes header, BAM, and directory back to disk after every mutation.
    /// </summary>
    private void Flush()
    {
        // Update free sector count in header before serializing.
        _header.FreeSectorCount = (ushort)_bam.FreeCount;

        int bamSectorCount = _header.DirectoryStartSector - 1;

        // Header.
        _stream.Seek(0, SeekOrigin.Begin);
        _stream.Write(_header.ToBytes());

        // BAM (padded to full sector coverage).
        byte[] bamBytes = _bam.ToBytes();
        byte[] bamPadded = new byte[bamSectorCount * 256];
        Array.Copy(bamBytes, bamPadded, Math.Min(bamBytes.Length, bamPadded.Length));
        _stream.Seek(256, SeekOrigin.Begin);
        _stream.Write(bamPadded);

        // Directory.
        _stream.Seek(_header.DirectoryStartSector * 256L, SeekOrigin.Begin);
        _stream.Write(_directory.ToBytes());

        _stream.Flush();
    }

    private void ThrowIfDisposed()
    {
        if (_disposed)
            throw new ObjectDisposedException(nameof(NdiImage));
    }

    public void Dispose()
    {
        if (!_disposed)
        {
            _stream.Dispose();
            _disposed = true;
        }
    }
}

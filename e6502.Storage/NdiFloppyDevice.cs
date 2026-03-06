namespace e6502.Storage;

/// <summary>
/// IStorageDevice backed by a Nova Disk Image (.ndi) file.
/// Mount() opens the image; Unmount() disposes it.
/// CurrentDirectory "/" maps to the root (parentIndex 0xFFFF).
/// Setting CurrentDirectory to a name resolves it as a subdirectory entry.
/// </summary>
public sealed class NdiFloppyDevice : IStorageDevice
{
    private static readonly Dictionary<string, NdiFileType> ExtToType =
        new(StringComparer.OrdinalIgnoreCase)
        {
            [".bas"] = NdiFileType.Bas,
            [".sid"] = NdiFileType.Sid,
            [".bin"] = NdiFileType.Bin,
            [".mid"] = NdiFileType.Mid,
            [".gfx"] = NdiFileType.Gfx,
        };

    private NdiImage? _image;

    // Tracks the current directory as a path string and its resolved parentIndex.
    private string _currentDir = "/";
    private ushort _parentIndex = 0xFFFF;

    public string Prefix { get; }
    public bool IsMounted => _image is not null;

    public string CurrentDirectory
    {
        get => _currentDir;
        set
        {
            ThrowIfNotMounted();
            if (string.IsNullOrEmpty(value) || value == "/")
            {
                _currentDir = "/";
                _parentIndex = 0xFFFF;
                return;
            }

            string path = value.Trim('/');
            string[] parts = path.Split('/');
            ushort parent = 0xFFFF;

            foreach (string part in parts)
            {
                var entries = _image!.ListDirectory(parent);
                var dir = Array.Find(entries, e => e.IsDirectory &&
                    string.Equals(e.Filename, part, StringComparison.OrdinalIgnoreCase));
                if (dir is null)
                    throw new DirectoryNotFoundException($"Directory '{part}' not found.");
                parent = (ushort)dir.Index;
            }

            _currentDir = path;
            _parentIndex = parent;
        }
    }

    public NdiFloppyDevice(string prefix)
    {
        Prefix = prefix;
    }

    // -------------------------------------------------------------------------
    // Mount / Unmount / Format
    // -------------------------------------------------------------------------

    public void Mount(string imagePath)
    {
        _image?.Dispose();
        _image = NdiImage.Open(imagePath);
        _currentDir = "/";
        _parentIndex = 0xFFFF;
    }

    public void Unmount()
    {
        _image?.Dispose();
        _image = null;
        _currentDir = "/";
        _parentIndex = 0xFFFF;
    }

    public void Format(string label, int sizeKB)
        => throw new NotSupportedException(
            "NdiFloppyDevice does not support Format. Use NdiImage.CreateFormatted then Mount.");

    // -------------------------------------------------------------------------
    // File operations
    // -------------------------------------------------------------------------

    public byte[] Load(string name, string ext)
    {
        ThrowIfNotMounted();
        return _image!.ReadFile(name + ext, _parentIndex);
    }

    public void Save(string name, byte[] data, string ext)
    {
        ThrowIfNotMounted();
        var type = ExtToType.TryGetValue(ext, out var t) ? t : NdiFileType.Bin;
        _image!.WriteFile(name + ext, type, _parentIndex, data);
    }

    public void Delete(string name, string ext)
    {
        ThrowIfNotMounted();
        _image!.DeleteFile(name + ext, _parentIndex);
    }

    public bool FileExists(string name, string ext)
    {
        ThrowIfNotMounted();
        var entries = _image!.ListDirectory(_parentIndex);
        return Array.Exists(entries, e => !e.IsDirectory &&
            string.Equals(e.Filename, name + ext, StringComparison.OrdinalIgnoreCase));
    }

    // -------------------------------------------------------------------------
    // Directory operations
    // -------------------------------------------------------------------------

    public StorageDirEntry[] ListDirectory(string? path)
    {
        ThrowIfNotMounted();

        ushort parent = _parentIndex;
        if (path is not null)
        {
            // Resolve an explicit multi-level path from root.
            string trimmed = path.Trim('/');
            if (trimmed.Length > 0)
            {
                parent = 0xFFFF;
                foreach (string part in trimmed.Split('/'))
                {
                    var entries = _image!.ListDirectory(parent);
                    var dir = Array.Find(entries, e => e.IsDirectory &&
                        string.Equals(e.Filename, part, StringComparison.OrdinalIgnoreCase));
                    if (dir is null)
                        throw new DirectoryNotFoundException($"Directory '{part}' not found.");
                    parent = (ushort)dir.Index;
                }
            }
        }

        var raw = _image!.ListDirectory(parent);
        return raw.Select(e =>
        {
            // Files are stored as "NAME.EXT" — strip the extension for the display name.
            string displayName = e.IsDirectory
                ? e.Filename
                : Path.GetFileNameWithoutExtension(e.Filename);
            return new StorageDirEntry(displayName, e.IsDirectory, e.FileType, e.IsDirectory ? 0 : e.SizeBytes);
        }).ToArray();
    }

    public void MakeDirectory(string name)
    {
        ThrowIfNotMounted();
        _image!.MakeDirectory(name, _parentIndex);
    }

    public void RemoveDirectory(string name)
    {
        ThrowIfNotMounted();
        _image!.RemoveDirectory(name, _parentIndex);
    }

    // -------------------------------------------------------------------------
    // Private helpers
    // -------------------------------------------------------------------------

    private void ThrowIfNotMounted()
    {
        if (_image is null)
            throw new IOException("No disk image is mounted.");
    }
}

namespace e6502.Storage;

/// <summary>
/// IStorageDevice implementation backed by a host filesystem directory.
/// Always mounted. CurrentDirectory is relative to the host root dir.
/// </summary>
public sealed class HostDirectoryDevice : IStorageDevice
{
    private static readonly string[] SupportedExtensions = [".bas", ".sid", ".bin", ".mid", ".gfx", ".xram"];

    private static readonly Dictionary<string, NdiFileType> ExtToType = new(StringComparer.OrdinalIgnoreCase)
    {
        [".bas"] = NdiFileType.Bas,
        [".sid"] = NdiFileType.Sid,
        [".bin"] = NdiFileType.Bin,
        [".mid"] = NdiFileType.Mid,
        [".gfx"] = NdiFileType.Gfx,
        [".xram"] = NdiFileType.Bin,
    };

    private readonly string _rootDir;
    private string _currentDir = "/";

    public string Prefix { get; }
    public bool IsMounted => true;

    public string CurrentDirectory
    {
        get => _currentDir;
        set => _currentDir = string.IsNullOrEmpty(value) || value == "/" ? "/" : value.Trim('/');
    }

    public HostDirectoryDevice(string rootDir, string prefix)
    {
        _rootDir = rootDir;
        Prefix = prefix;
    }

    private string ResolvePath(string name, string ext)
    {
        string dir = ResolveDir();
        string resolved = Path.GetFullPath(Path.Combine(dir, name + ext));
        string rootFull = Path.GetFullPath(_rootDir);
        if (!resolved.StartsWith(rootFull + Path.DirectorySeparatorChar, StringComparison.OrdinalIgnoreCase)
            && !resolved.Equals(rootFull, StringComparison.OrdinalIgnoreCase))
            throw new UnauthorizedAccessException($"Path traversal rejected: '{name}{ext}'");
        return resolved;
    }

    private string ResolveDir()
    {
        if (_currentDir == "/")
            return _rootDir;
        string dir = Path.GetFullPath(Path.Combine(_rootDir, _currentDir));
        string rootFull = Path.GetFullPath(_rootDir);
        if (!dir.StartsWith(rootFull + Path.DirectorySeparatorChar, StringComparison.OrdinalIgnoreCase)
            && !dir.Equals(rootFull, StringComparison.OrdinalIgnoreCase))
            throw new UnauthorizedAccessException("Path traversal rejected.");
        return dir;
    }

    public byte[] Load(string name, string ext)
    {
        string path = ResolvePath(name, ext);
        if (!File.Exists(path))
            throw new FileNotFoundException($"File not found: {name}{ext}", path);
        return File.ReadAllBytes(path);
    }

    public void Save(string name, byte[] data, string ext)
    {
        string dir = ResolveDir();
        Directory.CreateDirectory(dir);
        File.WriteAllBytes(Path.Combine(dir, name + ext), data);
    }

    public void Delete(string name, string ext)
    {
        string path = ResolvePath(name, ext);
        if (!File.Exists(path))
            throw new FileNotFoundException($"File not found: {name}{ext}", path);
        File.Delete(path);
    }

    public StorageDirEntry[] ListDirectory(string? path)
    {
        string targetDir;
        if (path is null)
        {
            targetDir = ResolveDir();
        }
        else
        {
            targetDir = Path.GetFullPath(Path.Combine(_rootDir, path.Trim('/')));
            string rootFull = Path.GetFullPath(_rootDir);
            if (!targetDir.StartsWith(rootFull + Path.DirectorySeparatorChar, StringComparison.OrdinalIgnoreCase)
                && !targetDir.Equals(rootFull, StringComparison.OrdinalIgnoreCase))
                throw new UnauthorizedAccessException($"Path traversal rejected: '{path}'");
        }

        if (!Directory.Exists(targetDir))
            return [];

        var dirs = Directory.GetDirectories(targetDir)
            .Select(d => new StorageDirEntry(Path.GetFileName(d), true, NdiFileType.Dir, 0))
            .OrderBy(e => e.Filename);

        var files = SupportedExtensions
            .SelectMany(ext => Directory.GetFiles(targetDir, "*" + ext))
            .Select(f =>
            {
                string ext = Path.GetExtension(f);
                string nameNoExt = Path.GetFileNameWithoutExtension(f);
                var type = ExtToType.TryGetValue(ext, out var t) ? t : NdiFileType.Bin;
                int size = (int)new FileInfo(f).Length;
                return new StorageDirEntry(nameNoExt, false, type, size);
            })
            .OrderBy(e => e.Filename);

        return [.. dirs, .. files];
    }

    public void MakeDirectory(string name)
    {
        string path = Path.GetFullPath(Path.Combine(ResolveDir(), name));
        string rootFull = Path.GetFullPath(_rootDir);
        if (!path.StartsWith(rootFull + Path.DirectorySeparatorChar, StringComparison.OrdinalIgnoreCase))
            throw new UnauthorizedAccessException($"Path traversal rejected: '{name}'");
        Directory.CreateDirectory(path);
    }

    public void RemoveDirectory(string name)
    {
        string path = Path.GetFullPath(Path.Combine(ResolveDir(), name));
        string rootFull = Path.GetFullPath(_rootDir);
        if (!path.StartsWith(rootFull + Path.DirectorySeparatorChar, StringComparison.OrdinalIgnoreCase))
            throw new UnauthorizedAccessException($"Path traversal rejected: '{name}'");
        Directory.Delete(path, recursive: false);
    }

    public bool FileExists(string name, string ext)
        => File.Exists(ResolvePath(name, ext));

    public void Format(string label, int sizeKB)
        => throw new NotSupportedException("HostDirectoryDevice does not support Format.");

    public void Mount(string imagePath)
        => throw new NotSupportedException("HostDirectoryDevice does not support Mount.");

    public void Unmount()
        => throw new NotSupportedException("HostDirectoryDevice does not support Unmount.");
}

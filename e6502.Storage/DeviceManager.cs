namespace e6502.Storage;

/// <summary>
/// Manages the 6 named device slots (HD0, HD1, FD0-FD3) and provides
/// filename prefix resolution and autoboot detection.
/// </summary>
public sealed class DeviceManager
{
    // Ordered slot names used for autoboot scan.
    private static readonly string[] SlotOrder = ["FD0", "FD1", "FD2", "FD3", "HD0", "HD1"];
    private static readonly string[] AutobootFiles = ["AUTOBOOT.bas", "AUTOBOOT.bin"];

    private readonly Dictionary<string, IStorageDevice> _devices =
        new(StringComparer.OrdinalIgnoreCase);

    private readonly string _disksDir;

    public string DefaultDevice { get; set; } = "HD0";

    /// <summary>
    /// Creates all 6 device slots.
    ///   HD0 = HostDirectoryDevice(hd0Path)
    ///   HD1 = HostDirectoryDevice(hd1Path)
    ///   FD0-FD3 = NdiFloppyDevice (unmounted)
    /// </summary>
    public DeviceManager(string hd0Path, string hd1Path, string disksDir)
    {
        _disksDir = disksDir;

        _devices["HD0"] = new HostDirectoryDevice(hd0Path, "HD0");
        _devices["HD1"] = new HostDirectoryDevice(hd1Path, "HD1");
        _devices["FD0"] = new NdiFloppyDevice("FD0");
        _devices["FD1"] = new NdiFloppyDevice("FD1");
        _devices["FD2"] = new NdiFloppyDevice("FD2");
        _devices["FD3"] = new NdiFloppyDevice("FD3");
    }

    /// <summary>
    /// Returns the device for a given prefix (e.g. "HD0", "FD1").
    /// Throws IOException for unknown prefixes.
    /// </summary>
    public IStorageDevice GetDevice(string prefix)
    {
        if (_devices.TryGetValue(prefix, out var dev))
            return dev;
        throw new IOException($"Unknown device prefix: '{prefix}'");
    }

    /// <summary>
    /// Parses a filename of the form "PREFIX:path" or "path".
    /// Returns the resolved device and the path portion.
    /// </summary>
    public (IStorageDevice Device, string Path) ResolveFilename(string filename)
    {
        int colon = filename.IndexOf(':');
        if (colon > 0)
        {
            string prefix = filename[..colon];
            string path = filename[(colon + 1)..];
            return (GetDevice(prefix), path);
        }
        return (GetDevice(DefaultDevice), filename);
    }

    /// <summary>
    /// Scans disksDir for fd0.ndi through fd3.ndi and mounts any found.
    /// Silently skips failures.
    /// </summary>
    public void AutoMount()
    {
        for (int i = 0; i <= 3; i++)
        {
            string imageName = $"fd{i}.ndi";
            string imagePath = Path.Combine(_disksDir, imageName);
            if (!File.Exists(imagePath))
                continue;

            try
            {
                _devices[$"FD{i}"].Mount(imagePath);
            }
            catch (Exception)
            {
                // Silently skip failures
            }
        }
    }

    /// <summary>
    /// Scans FD0, FD1, FD2, FD3, HD0, HD1 for AUTOBOOT.bas then AUTOBOOT.bin.
    /// Returns the first match found, or null.
    /// </summary>
    public (IStorageDevice Device, string Filename, string Extension)? FindAutoboot()
    {
        foreach (string slot in SlotOrder)
        {
            var dev = _devices[slot];
            if (!dev.IsMounted)
                continue;

            foreach (string file in AutobootFiles)
            {
                string name = Path.GetFileNameWithoutExtension(file);
                string ext = Path.GetExtension(file);
                try
                {
                    if (dev.FileExists(name, ext))
                        return (dev, name, ext);
                }
                catch (Exception)
                {
                    // Device error — skip
                }
            }
        }
        return null;
    }

    /// <summary>
    /// Mounts a named image: looks in disksDir for imageName.ndi and mounts it on the given prefix.
    /// </summary>
    public void MountDevice(string prefix, string imageName)
    {
        string imagePath = Path.Combine(_disksDir, imageName + ".ndi");
        GetDevice(prefix).Mount(imagePath);
    }

    /// <summary>
    /// Unmounts a floppy device.
    /// </summary>
    public void UnmountDevice(string prefix)
    {
        GetDevice(prefix).Unmount();
    }

    /// <summary>
    /// Creates a new image in disksDir named prefix.ndi (lowercase), then mounts it.
    /// </summary>
    public void FormatDevice(string prefix, string label, int sizeKB)
    {
        string imagePath = Path.Combine(_disksDir, prefix.ToLowerInvariant() + ".ndi");
        NdiImage.CreateFormatted(imagePath, label, sizeKB);
        GetDevice(prefix).Mount(imagePath);
    }
}

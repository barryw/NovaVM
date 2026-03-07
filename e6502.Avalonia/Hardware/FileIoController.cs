using System.Text;
using System.Text.RegularExpressions;
using e6502.Storage;

namespace e6502.Avalonia.Hardware;

public sealed partial class FileIoController
{
    private readonly byte[] _regs = new byte[VgcConstants.FioEnd - VgcConstants.FioBase + 1];
    private readonly Func<ushort, byte> _busRead;
    private readonly Action<ushort, byte> _busWrite;
    private readonly Func<byte, int, byte>? _vgcRead;
    private readonly Action<byte, int, byte>? _vgcWrite;
    private readonly Func<byte, int>? _vgcSpaceLength;
    private readonly SidPlayer? _sidPlayer;
    private readonly MusicEngine? _musicEngine;
    private readonly MidiPlayback? _midiPlayback;
    private readonly string _saveDir;
    private readonly DeviceManager? _deviceManager;
    private List<FileInfo>? _dirFiles;
    private List<StorageDirEntry>? _dirEntries;
    private IStorageDevice? _dirDevice;
    private int _dirIndex;

    public FileIoController(
        Func<ushort, byte> busRead,
        Action<ushort, byte> busWrite,
        string? saveDir = null,
        Func<byte, int, byte>? vgcRead = null,
        Action<byte, int, byte>? vgcWrite = null,
        Func<byte, int>? vgcSpaceLength = null,
        SidPlayer? sidPlayer = null,
        MusicEngine? musicEngine = null,
        MidiPlayback? midiPlayback = null,
        DeviceManager? deviceManager = null)
    {
        _busRead = busRead;
        _busWrite = busWrite;
        _vgcRead = vgcRead;
        _vgcWrite = vgcWrite;
        _vgcSpaceLength = vgcSpaceLength;
        _sidPlayer = sidPlayer;
        _musicEngine = musicEngine;
        _midiPlayback = midiPlayback;
        _deviceManager = deviceManager;
        _saveDir = saveDir ?? Path.Combine(
            Environment.GetFolderPath(Environment.SpecialFolder.UserProfile),
            "e6502-programs");
    }

    public string SaveDirectory => _saveDir;

    /// <summary>Fires after DoLoad succeeds. Argument is the filename (without extension).</summary>
    public Action<string>? ProgramLoaded;

    /// <summary>Fires after DoSave succeeds. Arguments: filename (without extension), whether .md was newly created.</summary>
    public Action<string, bool>? ProgramSaved;

    public bool OwnsAddress(ushort address) =>
        address >= VgcConstants.FioBase && address <= VgcConstants.FioEnd;

    public byte Read(ushort address) =>
        _regs[address - VgcConstants.FioBase];

    public void Write(ushort address, byte data)
    {
        _regs[address - VgcConstants.FioBase] = data;

        if (address == VgcConstants.FioCmd)
            ExecuteCommand(data);
    }

    private void ExecuteCommand(byte cmd)
    {
        switch (cmd)
        {
            case VgcConstants.FioCmdSave:
                DoSave();
                break;
            case VgcConstants.FioCmdLoad:
                DoLoad();
                break;
            case VgcConstants.FioCmdDirOpen:
                DoDirOpen();
                break;
            case VgcConstants.FioCmdDirRead:
                DoDirRead();
                break;
            case VgcConstants.FioCmdDelete:
                DoDelete();
                break;
            case VgcConstants.FioCmdGSave:
                DoGSave();
                break;
            case VgcConstants.FioCmdGLoad:
                DoGLoad();
                break;
            case VgcConstants.FioCmdSidPlay:
                DoSidPlay();
                break;
            case VgcConstants.FioCmdSidStop:
                DoSidStop();
                break;
            case VgcConstants.FioCmdInstrument:
                DoInstrument();
                break;
            case VgcConstants.FioCmdSound:
                DoSound();
                break;
            case VgcConstants.FioCmdVolume:
                DoVolume();
                break;
            case VgcConstants.FioCmdMusicSeq:
                DoMusicSeq();
                break;
            case VgcConstants.FioCmdMusicPlay:
                DoMusicPlay();
                break;
            case VgcConstants.FioCmdMusicStop:
                DoMusicStop();
                break;
            case VgcConstants.FioCmdMusicTempo:
                DoMusicTempo();
                break;
            case VgcConstants.FioCmdMusicLoop:
                DoMusicLoop();
                break;
            case VgcConstants.FioCmdMusicPri:
                DoMusicPri();
                break;
            case VgcConstants.FioCmdMidPlay:
                DoMidPlay();
                break;
            case VgcConstants.FioCmdMidStop:
                DoMidStop();
                break;
            case VgcConstants.FioCmdCd:
                DoCd();
                break;
            case VgcConstants.FioCmdMkdir:
                DoMkdir();
                break;
            case VgcConstants.FioCmdRmdir:
                DoRmdir();
                break;
            case VgcConstants.FioCmdFormat:
                DoFormat();
                break;
            case VgcConstants.FioCmdMount:
                DoMount();
                break;
            case VgcConstants.FioCmdUnmount:
                DoUnmount();
                break;
            case VgcConstants.FioCmdPwd:
                DoPwd();
                break;
            default:
                SetError(VgcConstants.FioErrIo);
                break;
        }
    }

    // -------------------------------------------------------------------------
    // Device resolution helper
    // -------------------------------------------------------------------------

    /// <summary>
    /// Resolves a raw filename string to a device and local name (no extension).
    /// Returns null if _deviceManager is null (fall through to direct filesystem).
    /// Throws IOException / FileNotFoundException on bad prefix or unmounted device.
    /// </summary>
    private (IStorageDevice Device, string Name, string? SavedDir)? ResolveDevice(string filename)
    {
        if (_deviceManager is null)
            return null;

        var (device, path) = _deviceManager.ResolveFilename(filename);

        if (!device.IsMounted)
            throw new InvalidOperationException("Device not mounted");

        // If path contains '/' the leading segments are a subdirectory to cd into temporarily.
        // We support simple single-directory navigation: "subdir/name".
        string name = path;
        string? savedDir = null;
        if (path.Contains('/'))
        {
            int slash = path.LastIndexOf('/');
            string subdir = path[..slash];
            name = path[(slash + 1)..];
            savedDir = device.CurrentDirectory;
            device.CurrentDirectory = subdir;
        }

        return (device, name, savedDir);
    }

    /// <summary>Restores device CurrentDirectory if it was temporarily changed.</summary>
    private static void RestoreDir(IStorageDevice device, string? savedDir)
    {
        if (savedDir is not null)
            device.CurrentDirectory = savedDir;
    }

    // -------------------------------------------------------------------------
    // File I/O commands
    // -------------------------------------------------------------------------

    private void DoSave()
    {
        try
        {
            string? filename = ReadFilename();
            if (filename is null)
            {
                SetError(VgcConstants.FioErrIo);
                return;
            }

            int srcAddr = _regs[VgcConstants.FioSrcL - VgcConstants.FioBase]
                        | (_regs[VgcConstants.FioSrcH - VgcConstants.FioBase] << 8);
            int endAddr = _regs[VgcConstants.FioEndL - VgcConstants.FioBase]
                        | (_regs[VgcConstants.FioEndH - VgcConstants.FioBase] << 8);

            if (endAddr <= srcAddr)
            {
                SetError(VgcConstants.FioErrIo);
                return;
            }

            int length = endAddr - srcAddr;
            byte[] data = new byte[2 + length]; // 2-byte load-address prefix
            data[0] = (byte)(srcAddr & 0xFF);
            data[1] = (byte)((srcAddr >> 8) & 0xFF);
            for (int i = 0; i < length; i++)
                data[2 + i] = _busRead((ushort)(srcAddr + i));

            var resolved = ResolveDevice(filename);
            if (resolved is not null)
            {
                var (device, name, savedDir) = resolved.Value;
                try
                {
                    device.Save(name, data, ".bas");

                    // Companion .md — only on HostDirectoryDevice (skip for NDI floppy)
                    bool isNewDoc = false;
                    if (device is HostDirectoryDevice hdd)
                    {
                        string mdPath = Path.Combine(
                            hdd.CurrentDirectory == "/" || string.IsNullOrEmpty(hdd.CurrentDirectory)
                                ? SaveDirectory
                                : Path.Combine(SaveDirectory, hdd.CurrentDirectory.Trim('/')),
                            name + ".md");
                        // Use the actual filesystem path via FileExists
                        isNewDoc = !device.FileExists(name, ".md");
                        if (isNewDoc)
                            File.WriteAllText(mdPath, $"---\ntitle: {name}\ntype: program\ncategory: Programs\nkeywords: [{name}]\n---\n\n");
                    }

                    SetOk();
                    ProgramSaved?.Invoke(name, isNewDoc);
                }
                finally
                {
                    RestoreDir(device, savedDir);
                }
                return;
            }

            // Direct filesystem fallback (null DeviceManager)
            Directory.CreateDirectory(_saveDir);
            string path = GetFullPath(filename, ".bas");
            File.WriteAllBytes(path, data);

            // Create companion .md if it doesn't exist
            string mdPathFallback = Path.ChangeExtension(path, ".md");
            bool isNewDocFallback = !File.Exists(mdPathFallback);
            if (isNewDocFallback)
                File.WriteAllText(mdPathFallback, $"---\ntitle: {filename}\ntype: program\ncategory: Programs\nkeywords: [{filename}]\n---\n\n");

            SetOk();
            ProgramSaved?.Invoke(filename, isNewDocFallback);
        }
        catch
        {
            SetError(VgcConstants.FioErrIo);
        }
    }

    private void DoLoad()
    {
        try
        {
            string? filename = ReadFilename();
            if (filename is null)
            {
                SetError(VgcConstants.FioErrIo);
                return;
            }

            var resolved = ResolveDevice(filename);
            if (resolved is not null)
            {
                var (device, name, savedDir) = resolved.Value;
                try
                {
                    byte[] data;
                    bool isBin = false;

                    if (device.FileExists(name, ".bas"))
                    {
                        data = device.Load(name, ".bas");
                    }
                    else if (device.FileExists(name, ".bin"))
                    {
                        data = device.Load(name, ".bin");
                        isBin = true;
                    }
                    else
                    {
                        SetError(VgcConstants.FioErrNotFound);
                        return;
                    }

                    if (data.Length < 2)
                    {
                        SetError(VgcConstants.FioErrIo);
                        return;
                    }

                    int dataLength = data.Length - 2;
                    int destAddr;

                    if (isBin)
                    {
                        destAddr = data[0] | (data[1] << 8);
                        _regs[VgcConstants.FioSrcL - VgcConstants.FioBase] = (byte)(destAddr & 0xFF);
                        _regs[VgcConstants.FioSrcH - VgcConstants.FioBase] = (byte)((destAddr >> 8) & 0xFF);
                    }
                    else
                    {
                        destAddr = _regs[VgcConstants.FioSrcL - VgcConstants.FioBase]
                                 | (_regs[VgcConstants.FioSrcH - VgcConstants.FioBase] << 8);
                    }

                    for (int i = 0; i < dataLength; i++)
                        _busWrite((ushort)(destAddr + i), data[2 + i]);

                    _regs[VgcConstants.FioSizeL - VgcConstants.FioBase] = (byte)(dataLength & 0xFF);
                    _regs[VgcConstants.FioSizeH - VgcConstants.FioBase] = (byte)((dataLength >> 8) & 0xFF);
                    _regs[VgcConstants.FioDirType - VgcConstants.FioBase] = (byte)(isBin ? VgcConstants.FioDirTypeBin : 0);

                    SetOk();
                    ProgramLoaded?.Invoke(name);
                }
                finally
                {
                    RestoreDir(device, savedDir);
                }
                return;
            }

            // Direct filesystem fallback
            string path = GetFullPath(filename, ".bas");
            bool isBinFallback = false;
            if (!File.Exists(path))
            {
                string binPath = GetFullPath(filename, ".bin");
                if (File.Exists(binPath))
                {
                    path = binPath;
                    isBinFallback = true;
                }
            }

            if (!File.Exists(path))
            {
                SetError(VgcConstants.FioErrNotFound);
                return;
            }

            byte[] dataFallback = File.ReadAllBytes(path);

            if (dataFallback.Length < 2)
            {
                SetError(VgcConstants.FioErrIo);
                return;
            }

            int dataLengthFallback = dataFallback.Length - 2;
            int destAddrFallback;

            if (isBinFallback)
            {
                destAddrFallback = dataFallback[0] | (dataFallback[1] << 8);
                _regs[VgcConstants.FioSrcL - VgcConstants.FioBase] = (byte)(destAddrFallback & 0xFF);
                _regs[VgcConstants.FioSrcH - VgcConstants.FioBase] = (byte)((destAddrFallback >> 8) & 0xFF);
            }
            else
            {
                destAddrFallback = _regs[VgcConstants.FioSrcL - VgcConstants.FioBase]
                                 | (_regs[VgcConstants.FioSrcH - VgcConstants.FioBase] << 8);
            }

            for (int i = 0; i < dataLengthFallback; i++)
                _busWrite((ushort)(destAddrFallback + i), dataFallback[2 + i]);

            _regs[VgcConstants.FioSizeL - VgcConstants.FioBase] = (byte)(dataLengthFallback & 0xFF);
            _regs[VgcConstants.FioSizeH - VgcConstants.FioBase] = (byte)((dataLengthFallback >> 8) & 0xFF);
            _regs[VgcConstants.FioDirType - VgcConstants.FioBase] = (byte)(isBinFallback ? VgcConstants.FioDirTypeBin : 0);

            SetOk();
            ProgramLoaded?.Invoke(filename);
        }
        catch (FileNotFoundException)
        {
            SetError(VgcConstants.FioErrNotFound);
        }
        catch
        {
            SetError(VgcConstants.FioErrIo);
        }
    }

    private void DoDirOpen()
    {
        try
        {
            if (_deviceManager is not null)
            {
                // Use current default device's listing
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
            var dir = new DirectoryInfo(_saveDir);
            _dirFiles = dir.Exists
                ? dir.GetFiles("*.bas")
                      .Concat(dir.GetFiles("*.sid"))
                      .Concat(dir.GetFiles("*.bin"))
                      .Concat(dir.GetFiles("*.mid"))
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

    private void DoDirRead()
    {
        _dirIndex++;

        if (_dirEntries is not null)
        {
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

        if (_dirFiles is null)
        {
            SetEndOfDir();
            return;
        }

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

    private void DoDelete()
    {
        try
        {
            string? filename = ReadFilename();
            if (filename is null)
            {
                SetError(VgcConstants.FioErrIo);
                return;
            }

            var resolved = ResolveDevice(filename);
            if (resolved is not null)
            {
                var (device, name, savedDir) = resolved.Value;
                try
                {
                    if (!device.FileExists(name, ".bas"))
                    {
                        SetError(VgcConstants.FioErrNotFound);
                        return;
                    }
                    device.Delete(name, ".bas");

                    // Companion .md cleanup for host devices
                    if (device.FileExists(name, ".md"))
                        device.Delete(name, ".md");

                    SetOk();
                }
                finally
                {
                    RestoreDir(device, savedDir);
                }
                return;
            }

            // Direct filesystem fallback
            string path = GetFullPath(filename, ".bas");
            if (!File.Exists(path))
            {
                SetError(VgcConstants.FioErrNotFound);
                return;
            }

            File.Delete(path);

            string mdPath = Path.ChangeExtension(path, ".md");
            if (File.Exists(mdPath))
                File.Delete(mdPath);

            SetOk();
        }
        catch
        {
            SetError(VgcConstants.FioErrIo);
        }
    }

    private void DoGSave()
    {
        try
        {
            if (_vgcRead is null || _vgcSpaceLength is null)
            {
                SetError(VgcConstants.FioErrIo);
                return;
            }

            string? filename = ReadFilename();
            if (filename is null)
            {
                SetError(VgcConstants.FioErrIo);
                return;
            }

            byte space = _regs[VgcConstants.FioGSpace - VgcConstants.FioBase];
            int addr = _regs[VgcConstants.FioGAddrL - VgcConstants.FioBase]
                     | (_regs[VgcConstants.FioGAddrH - VgcConstants.FioBase] << 8);
            int len = _regs[VgcConstants.FioGLenL - VgcConstants.FioBase]
                    | (_regs[VgcConstants.FioGLenH - VgcConstants.FioBase] << 8);

            int spaceLen = _vgcSpaceLength(space);
            if (len <= 0 || spaceLen <= 0 || addr < 0 || addr + len > spaceLen)
            {
                SetError(VgcConstants.FioErrIo);
                return;
            }

            byte[] data = new byte[len];
            for (int i = 0; i < len; i++)
                data[i] = _vgcRead(space, addr + i);

            var resolved = ResolveDevice(filename);
            if (resolved is not null)
            {
                var (device, name, savedDir) = resolved.Value;
                try
                {
                    device.Save(name, data, ".gfx");
                }
                finally
                {
                    RestoreDir(device, savedDir);
                }
            }
            else
            {
                Directory.CreateDirectory(_saveDir);
                string path = GetFullPath(filename, ".gfx");
                File.WriteAllBytes(path, data);
            }

            _regs[VgcConstants.FioSizeL - VgcConstants.FioBase] = (byte)(len & 0xFF);
            _regs[VgcConstants.FioSizeH - VgcConstants.FioBase] = (byte)((len >> 8) & 0xFF);
            SetOk();
        }
        catch
        {
            SetError(VgcConstants.FioErrIo);
        }
    }

    private void DoGLoad()
    {
        try
        {
            if (_vgcWrite is null || _vgcSpaceLength is null)
            {
                SetError(VgcConstants.FioErrIo);
                return;
            }

            string? filename = ReadFilename();
            if (filename is null)
            {
                SetError(VgcConstants.FioErrIo);
                return;
            }

            byte space = _regs[VgcConstants.FioGSpace - VgcConstants.FioBase];
            int addr = _regs[VgcConstants.FioGAddrL - VgcConstants.FioBase]
                     | (_regs[VgcConstants.FioGAddrH - VgcConstants.FioBase] << 8);
            int reqLen = _regs[VgcConstants.FioGLenL - VgcConstants.FioBase]
                       | (_regs[VgcConstants.FioGLenH - VgcConstants.FioBase] << 8);

            byte[] data;
            var resolved = ResolveDevice(filename);
            if (resolved is not null)
            {
                var (device, name, savedDir) = resolved.Value;
                try
                {
                    if (!device.FileExists(name, ".gfx"))
                    {
                        SetError(VgcConstants.FioErrNotFound);
                        return;
                    }
                    data = device.Load(name, ".gfx");
                }
                finally
                {
                    RestoreDir(device, savedDir);
                }
            }
            else
            {
                string path = GetFullPath(filename, ".gfx");
                if (!File.Exists(path))
                {
                    SetError(VgcConstants.FioErrNotFound);
                    return;
                }
                data = File.ReadAllBytes(path);
            }

            int len = reqLen > 0 ? Math.Min(reqLen, data.Length) : data.Length;
            int spaceLen = _vgcSpaceLength(space);

            if (len < 0 || spaceLen <= 0 || addr < 0 || addr + len > spaceLen)
            {
                SetError(VgcConstants.FioErrIo);
                return;
            }

            for (int i = 0; i < len; i++)
                _vgcWrite(space, addr + i, data[i]);

            _regs[VgcConstants.FioSizeL - VgcConstants.FioBase] = (byte)(len & 0xFF);
            _regs[VgcConstants.FioSizeH - VgcConstants.FioBase] = (byte)((len >> 8) & 0xFF);
            SetOk();
        }
        catch (FileNotFoundException)
        {
            SetError(VgcConstants.FioErrNotFound);
        }
        catch
        {
            SetError(VgcConstants.FioErrIo);
        }
    }

    private void DoSidPlay()
    {
        if (_sidPlayer is null) { SetError(VgcConstants.FioErrIo); return; }

        try
        {
            string? filename = ReadFilename();
            if (filename is null) { SetError(VgcConstants.FioErrIo); return; }

            byte[] data;
            var resolved = ResolveDevice(filename);
            if (resolved is not null)
            {
                var (device, name, savedDir) = resolved.Value;
                try
                {
                    if (!device.FileExists(name, ".sid"))
                    {
                        SetError(VgcConstants.FioErrNotFound);
                        return;
                    }
                    data = device.Load(name, ".sid");
                }
                finally
                {
                    RestoreDir(device, savedDir);
                }
            }
            else
            {
                string path = GetFullPath(filename, ".sid");
                if (!File.Exists(path))
                {
                    SetError(VgcConstants.FioErrNotFound);
                    return;
                }
                data = File.ReadAllBytes(path);
            }

            var info = SidFileParser.Parse(data);
            if (!info.IsValid) { SetError(VgcConstants.FioErrIo); return; }

            int song = _regs[VgcConstants.FioSrcL - VgcConstants.FioBase];
            if (song < 1) song = info.StartSong;

            _sidPlayer.Play(info, song);
            SetOk();
        }
        catch (FileNotFoundException) { SetError(VgcConstants.FioErrNotFound); }
        catch { SetError(VgcConstants.FioErrIo); }
    }

    private void DoSidStop()
    {
        if (_sidPlayer is null) { SetError(VgcConstants.FioErrIo); return; }
        _sidPlayer.Stop();
        SetOk();
    }

    private void DoInstrument()
    {
        if (_musicEngine is null) { SetError(VgcConstants.FioErrIo); return; }
        int id        = _regs[VgcConstants.FioSrcL   - VgcConstants.FioBase];
        byte waveform = _regs[VgcConstants.FioSrcH   - VgcConstants.FioBase];
        byte a        = _regs[VgcConstants.FioEndL   - VgcConstants.FioBase];
        byte d        = _regs[VgcConstants.FioEndH   - VgcConstants.FioBase];
        byte s        = _regs[VgcConstants.FioSizeL  - VgcConstants.FioBase];
        byte r        = _regs[VgcConstants.FioSizeH  - VgcConstants.FioBase];
        _musicEngine.DefineInstrument(id, waveform, a, d, s, r);
        SetOk();
    }

    private void DoSound()
    {
        if (_musicEngine is null) { SetError(VgcConstants.FioErrIo); return; }
        int note     = _regs[VgcConstants.FioSrcL - VgcConstants.FioBase];
        int duration = _regs[VgcConstants.FioSrcH - VgcConstants.FioBase];
        int instId   = _regs[VgcConstants.FioEndL - VgcConstants.FioBase];
        _musicEngine.PlaySound(note, duration, instId);
        SetOk();
    }

    private void DoVolume()
    {
        if (_musicEngine is null) { SetError(VgcConstants.FioErrIo); return; }
        int level = _regs[VgcConstants.FioSrcL - VgcConstants.FioBase];
        int voice = _regs[VgcConstants.FioSrcH - VgcConstants.FioBase];
        if (voice == 0)
            _musicEngine.SetVolume(level);
        else
            _musicEngine.SetVoiceVolume(voice, level);
        SetOk();
    }

    private void DoMusicSeq()
    {
        if (_musicEngine is null) { SetError(VgcConstants.FioErrIo); return; }
        int voice = _regs[VgcConstants.FioSrcL - VgcConstants.FioBase];

        int len = _regs[VgcConstants.FioNameLen - VgcConstants.FioBase];
        int ptr = _regs[VgcConstants.FioEndL - VgcConstants.FioBase]
                | (_regs[VgcConstants.FioEndH - VgcConstants.FioBase] << 8);
        if (len < 1) { SetError(VgcConstants.FioErrIo); return; }

        var sb = new StringBuilder(len);
        for (int i = 0; i < len; i++)
            sb.Append((char)_busRead((ushort)(ptr + i)));

        _musicEngine.SetSequence(voice - 1, sb.ToString());
        SetOk();
    }

    private void DoMusicPlay()
    {
        if (_musicEngine is null) { SetError(VgcConstants.FioErrIo); return; }
        _musicEngine.MusicPlay();
        SetOk();
    }

    private void DoMusicStop()
    {
        if (_musicEngine is null) { SetError(VgcConstants.FioErrIo); return; }
        _musicEngine.MusicStop();
        SetOk();
    }

    private void DoMusicTempo()
    {
        if (_musicEngine is null) { SetError(VgcConstants.FioErrIo); return; }
        int bpm = _regs[VgcConstants.FioSrcL - VgcConstants.FioBase]
                | (_regs[VgcConstants.FioSrcH - VgcConstants.FioBase] << 8);
        _musicEngine.SetTempo(bpm);
        SetOk();
    }

    private void DoMusicLoop()
    {
        if (_musicEngine is null) { SetError(VgcConstants.FioErrIo); return; }
        bool on = _regs[VgcConstants.FioSrcL - VgcConstants.FioBase] != 0;
        _musicEngine.SetLoop(on);
        SetOk();
    }

    private void DoMusicPri()
    {
        if (_musicEngine is null) { SetError(VgcConstants.FioErrIo); return; }
        var pri = new List<int>();
        byte v1 = _regs[VgcConstants.FioSrcL - VgcConstants.FioBase];
        byte v2 = _regs[VgcConstants.FioSrcH - VgcConstants.FioBase];
        byte v3 = _regs[VgcConstants.FioEndL - VgcConstants.FioBase];
        byte v4 = _regs[VgcConstants.FioEndH - VgcConstants.FioBase];
        byte v5 = _regs[VgcConstants.FioSizeL - VgcConstants.FioBase];
        byte v6 = _regs[VgcConstants.FioSizeH - VgcConstants.FioBase];
        if (v1 > 0) pri.Add(v1 - 1);
        if (v2 > 0) pri.Add(v2 - 1);
        if (v3 > 0) pri.Add(v3 - 1);
        if (v4 > 0) pri.Add(v4 - 1);
        if (v5 > 0) pri.Add(v5 - 1);
        if (v6 > 0) pri.Add(v6 - 1);
        _musicEngine.SetPriority(pri.ToArray());
        SetOk();
    }

    private void DoMidPlay()
    {
        if (_midiPlayback is null || _musicEngine is null)
        {
            SetError(VgcConstants.FioErrIo);
            return;
        }

        try
        {
            string? filename = ReadFilename();
            if (filename is null) { SetError(VgcConstants.FioErrIo); return; }

            string path = string.Empty;
            var resolved = ResolveDevice(filename);
            if (resolved is not null)
            {
                var (device, name, savedDir) = resolved.Value;
                try
                {
                    // MIDI playback requires a file path — extract it for DryWetMidi
                    if (device is HostDirectoryDevice hdd)
                    {
                        string dir = hdd.CurrentDirectory == "/"
                            ? _saveDir
                            : Path.Combine(_saveDir, hdd.CurrentDirectory.Trim('/'));
                        path = Path.Combine(dir, name + ".mid");
                        if (!File.Exists(path))
                        {
                            SetError(VgcConstants.FioErrNotFound);
                            return;
                        }
                    }
                    else
                    {
                        // NDI floppy: load bytes then write to temp file
                        if (!device.FileExists(name, ".mid"))
                        {
                            SetError(VgcConstants.FioErrNotFound);
                            return;
                        }
                        byte[] midiBytes = device.Load(name, ".mid");
                        path = Path.Combine(Path.GetTempPath(), $"e6502-{name}.mid");
                        File.WriteAllBytes(path, midiBytes);
                    }
                }
                finally
                {
                    RestoreDir(device, savedDir);
                }
            }
            else
            {
                path = GetFullPath(filename, ".mid");
                if (!File.Exists(path))
                {
                    SetError(VgcConstants.FioErrNotFound);
                    return;
                }
            }

            var midi = Melanchall.DryWetMidi.Core.MidiFile.Read(path);
            var analysis = MidiEngine.AnalyzeChannels(midi);
            var selectedChannels = MidiEngine.SelectChannels(midi);

            var instrumentSlots = new int[selectedChannels.Length];
            var registeredBuckets = new Dictionary<int, int>();
            int nextSlot = 1;

            for (int v = 0; v < selectedChannels.Length; v++)
            {
                int ch = selectedChannels[v];
                var bucket = MidiEngine.GetInstrumentBucket(analysis[ch].GmProgram, analysis[ch].IsDrums);
                int bucketIdx = Array.IndexOf(MidiEngine.GetAllBuckets(), bucket);

                if (!registeredBuckets.TryGetValue(bucketIdx, out int slot))
                {
                    slot = nextSlot++;
                    registeredBuckets[bucketIdx] = slot;
                    _musicEngine.DefineInstrument(slot, bucket.Waveform,
                        bucket.Attack, bucket.Decay, bucket.Sustain, bucket.Release);
                }
                instrumentSlots[v] = slot;
            }

            _musicEngine.SetVolume(15);
            _midiPlayback.Play(midi, selectedChannels, instrumentSlots);
            SetOk();
        }
        catch (FileNotFoundException)
        {
            SetError(VgcConstants.FioErrNotFound);
        }
        catch
        {
            SetError(VgcConstants.FioErrIo);
        }
    }

    private void DoMidStop()
    {
        _midiPlayback?.Stop();
        SetOk();
    }

    // -------------------------------------------------------------------------
    // New device management commands ($20-$26)
    // -------------------------------------------------------------------------

    private void DoCd()
    {
        if (_deviceManager is null) { SetError(VgcConstants.FioErrIo); return; }
        try
        {
            string? raw = ReadFilenameRaw();
            if (raw is null) { SetError(VgcConstants.FioErrIo); return; }

            var (device, path) = _deviceManager.ResolveFilename(raw);
            if (!device.IsMounted) { SetError(VgcConstants.FioErrNotMounted); return; }
            device.CurrentDirectory = path;
            SetOk();
        }
        catch { SetError(VgcConstants.FioErrIo); }
    }

    private void DoMkdir()
    {
        if (_deviceManager is null) { SetError(VgcConstants.FioErrIo); return; }
        try
        {
            string? raw = ReadFilenameRaw();
            if (raw is null) { SetError(VgcConstants.FioErrIo); return; }

            var (device, name) = _deviceManager.ResolveFilename(raw);
            if (!device.IsMounted) { SetError(VgcConstants.FioErrNotMounted); return; }
            device.MakeDirectory(name);
            SetOk();
        }
        catch { SetError(VgcConstants.FioErrIo); }
    }

    private void DoRmdir()
    {
        if (_deviceManager is null) { SetError(VgcConstants.FioErrIo); return; }
        try
        {
            string? raw = ReadFilenameRaw();
            if (raw is null) { SetError(VgcConstants.FioErrIo); return; }

            var (device, name) = _deviceManager.ResolveFilename(raw);
            if (!device.IsMounted) { SetError(VgcConstants.FioErrNotMounted); return; }
            device.RemoveDirectory(name);
            SetOk();
        }
        catch { SetError(VgcConstants.FioErrIo); }
    }

    private void DoFormat()
    {
        if (_deviceManager is null) { SetError(VgcConstants.FioErrIo); return; }
        try
        {
            // FIO_NAME holds the device prefix (e.g. "FD0")
            string? prefix = ReadFilenameRaw();
            if (prefix is null) { SetError(VgcConstants.FioErrIo); return; }

            int sizeKB = _regs[VgcConstants.FioSrcL - VgcConstants.FioBase]
                       | (_regs[VgcConstants.FioSrcH - VgcConstants.FioBase] << 8);
            if (sizeKB == 0) sizeKB = 1440; // default: 1.44MB

            _deviceManager.FormatDevice(prefix, prefix, sizeKB);
            SetOk();
        }
        catch { SetError(VgcConstants.FioErrIo); }
    }

    private void DoMount()
    {
        if (_deviceManager is null) { SetError(VgcConstants.FioErrIo); return; }
        try
        {
            // FIO_NAME: "PREFIX\0imagename" — two null-separated strings in the buffer
            int nameLen = _regs[VgcConstants.FioNameLen - VgcConstants.FioBase];
            if (nameLen < 2) { SetError(VgcConstants.FioErrIo); return; }

            int nameOffset = VgcConstants.FioName - VgcConstants.FioBase;
            // Find null separator
            int sep = -1;
            for (int i = 0; i < nameLen; i++)
            {
                if (_regs[nameOffset + i] == 0)
                {
                    sep = i;
                    break;
                }
            }
            if (sep < 1) { SetError(VgcConstants.FioErrIo); return; }

            string prefix = Encoding.ASCII.GetString(_regs, nameOffset, sep);
            string imageName = Encoding.ASCII.GetString(_regs, nameOffset + sep + 1, nameLen - sep - 1).TrimEnd('\0');

            _deviceManager.MountDevice(prefix, imageName);
            SetOk();
        }
        catch { SetError(VgcConstants.FioErrIo); }
    }

    private void DoUnmount()
    {
        if (_deviceManager is null) { SetError(VgcConstants.FioErrIo); return; }
        try
        {
            string? prefix = ReadFilenameRaw();
            if (prefix is null) { SetError(VgcConstants.FioErrIo); return; }
            _deviceManager.UnmountDevice(prefix);
            SetOk();
        }
        catch { SetError(VgcConstants.FioErrIo); }
    }

    private void DoPwd()
    {
        if (_deviceManager is null) { SetError(VgcConstants.FioErrIo); return; }
        try
        {
            var device = _deviceManager.GetDevice(_deviceManager.DefaultDevice);
            string pwd = $"{device.Prefix}:{device.CurrentDirectory}";
            int len = Math.Min(pwd.Length, 63);
            _regs[VgcConstants.FioNameLen - VgcConstants.FioBase] = (byte)len;
            int nameOffset = VgcConstants.FioName - VgcConstants.FioBase;
            for (int i = 0; i < len; i++)
                _regs[nameOffset + i] = (byte)pwd[i];
            SetOk();
        }
        catch { SetError(VgcConstants.FioErrIo); }
    }

    // -------------------------------------------------------------------------
    // Directory entry population
    // -------------------------------------------------------------------------

    private void PopulateDirEntryFromStorage(StorageDirEntry entry)
    {
        int type;
        if (entry.IsDirectory)
        {
            type = VgcConstants.FioDirTypeDir;
        }
        else
        {
            type = entry.FileType switch
            {
                NdiFileType.Sid => 1,
                NdiFileType.Bin => 2,
                NdiFileType.Mid => 3,
                _ => 0
            };
        }
        _regs[VgcConstants.FioDirType - VgcConstants.FioBase] = (byte)type;

        string displayName = entry.Filename;
        int nameLen = Math.Min(displayName.Length, 63);
        _regs[VgcConstants.FioNameLen - VgcConstants.FioBase] = (byte)nameLen;
        int nameOffset = VgcConstants.FioName - VgcConstants.FioBase;
        for (int i = 0; i < nameLen; i++)
            _regs[nameOffset + i] = (byte)displayName[i];

        long dataSize = (type == 1 || type == 3) ? entry.SizeBytes : Math.Max(0, entry.SizeBytes - 2);
        _regs[VgcConstants.FioSizeL - VgcConstants.FioBase] = (byte)(dataSize & 0xFF);
        _regs[VgcConstants.FioSizeH - VgcConstants.FioBase] = (byte)((dataSize >> 8) & 0xFF);
    }

    private void PopulateDirEntry(FileInfo fi)
    {
        string baseName = Path.GetFileNameWithoutExtension(fi.Name);
        string ext = fi.Extension.ToLowerInvariant();

        // File type: 0=BAS, 1=SID, 2=BIN, 3=MID
        int type = ext switch { ".sid" => 1, ".bin" => 2, ".mid" => 3, _ => 0 };
        _regs[VgcConstants.FioDirType - VgcConstants.FioBase] = (byte)type;

        // Build display name with addresses appended
        string displayName = baseName;
        try
        {
            if (type == 2 && fi.Length >= 2) // BIN: show load address
            {
                byte[] hdr = new byte[2];
                using var fs = fi.OpenRead();
                if (fs.Read(hdr, 0, 2) == 2)
                    displayName = $"{baseName} ${hdr[0] | (hdr[1] << 8):X4}";
            }
            else if (type == 1 && fi.Length >= 0x16) // SID: show load/init/play
            {
                byte[] hdr = new byte[0x16];
                using var fs = fi.OpenRead();
                if (fs.Read(hdr, 0, hdr.Length) == hdr.Length)
                {
                    int load = (hdr[0x08] << 8) | hdr[0x09]; // big-endian
                    int init = (hdr[0x0A] << 8) | hdr[0x0B];
                    int play = (hdr[0x0C] << 8) | hdr[0x0D];
                    displayName = $"{baseName} ${load:X4} ${init:X4} ${play:X4}";
                }
            }
        }
        catch { /* ignore — just use bare name */ }

        int nameLen = Math.Min(displayName.Length, 63);
        _regs[VgcConstants.FioNameLen - VgcConstants.FioBase] = (byte)nameLen;
        int nameOffset = VgcConstants.FioName - VgcConstants.FioBase;
        for (int i = 0; i < nameLen; i++)
            _regs[nameOffset + i] = (byte)displayName[i];

        // File size (SID/MID = full file size, BAS/BIN = minus 2-byte header)
        long dataSize = (type == 1 || type == 3) ? fi.Length : Math.Max(0, fi.Length - 2);
        _regs[VgcConstants.FioSizeL - VgcConstants.FioBase] = (byte)(dataSize & 0xFF);
        _regs[VgcConstants.FioSizeH - VgcConstants.FioBase] = (byte)((dataSize >> 8) & 0xFF);
    }

    // -------------------------------------------------------------------------
    // Helpers
    // -------------------------------------------------------------------------

    private void SetEndOfDir()
    {
        _regs[VgcConstants.FioStatus - VgcConstants.FioBase] = VgcConstants.FioStatusError;
        _regs[VgcConstants.FioErrCode - VgcConstants.FioBase] = VgcConstants.FioErrEndOfDir;
    }

    /// <summary>
    /// Reads the FIO_NAME buffer as a sanitized safe filename (alphanumeric/dash/underscore/dot only).
    /// Used for Save/Load/Delete where filenames must be clean.
    /// </summary>
    private string? ReadFilename()
    {
        int len = _regs[VgcConstants.FioNameLen - VgcConstants.FioBase];
        if (len < 1 || len > 63)
            return null;

        var sb = new StringBuilder(len);
        int nameOffset = VgcConstants.FioName - VgcConstants.FioBase;
        for (int i = 0; i < len; i++)
            sb.Append((char)_regs[nameOffset + i]);

        string raw = sb.ToString().Trim();
        if (raw.Length == 0)
            return null;

        // Sanitize: only allow alphanumeric, dash, underscore, dot
        if (!SafeFilename().IsMatch(raw))
            return null;

        return raw;
    }

    /// <summary>
    /// Reads the FIO_NAME buffer with relaxed validation — allows ':', '/' for device prefixes and paths.
    /// Used for CD/Mkdir/Rmdir/Format/Mount/Unmount/Pwd.
    /// </summary>
    private string? ReadFilenameRaw()
    {
        int len = _regs[VgcConstants.FioNameLen - VgcConstants.FioBase];
        if (len < 1 || len > 63)
            return null;

        var sb = new StringBuilder(len);
        int nameOffset = VgcConstants.FioName - VgcConstants.FioBase;
        for (int i = 0; i < len; i++)
            sb.Append((char)_regs[nameOffset + i]);

        string raw = sb.ToString().Trim();
        return raw.Length == 0 ? null : raw;
    }

    private string GetFullPath(string filename, string defaultExtension)
    {
        if (!Path.HasExtension(filename))
            filename += defaultExtension;
        return Path.Combine(_saveDir, filename);
    }

    private void SetOk()
    {
        _regs[VgcConstants.FioStatus - VgcConstants.FioBase] = VgcConstants.FioStatusOk;
        _regs[VgcConstants.FioErrCode - VgcConstants.FioBase] = VgcConstants.FioErrNone;
    }

    private void SetError(byte errCode)
    {
        _regs[VgcConstants.FioStatus - VgcConstants.FioBase] = VgcConstants.FioStatusError;
        _regs[VgcConstants.FioErrCode - VgcConstants.FioBase] = errCode;
    }

    [GeneratedRegex(@"^[A-Za-z0-9_.\-]+$")]
    private static partial Regex SafeFilename();

    /// <summary>
    /// Simple glob matching: * matches zero or more chars, ? matches exactly one char.
    /// Case-insensitive.
    /// </summary>
    internal static bool GlobMatch(string pattern, string text)
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

    internal record struct FilterPattern(string? DevicePrefix, string? DirectoryPath, string NamePattern, string? ExtFilter);

    internal static FilterPattern ParseFilterPattern(string pattern)
    {
        string? devicePrefix = null;
        string remainder = pattern;

        int colon = pattern.IndexOf(':');
        if (colon > 0)
        {
            devicePrefix = pattern[..colon];
            remainder = pattern[(colon + 1)..];
        }

        string? dirPath = null;
        int lastSlash = remainder.LastIndexOf('/');
        if (lastSlash >= 0)
        {
            dirPath = lastSlash > 0 ? remainder[..lastSlash] : null;
            remainder = remainder[(lastSlash + 1)..];
        }

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
}

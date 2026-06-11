using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Text.RegularExpressions;
using e6502.Storage;
using Melanchall.DryWetMidi.Core;
using Melanchall.DryWetMidi.Interaction;

namespace e6502.Avalonia.Hardware;

public sealed partial class FileIoController
{
    private readonly byte[] _regs = new byte[VgcConstants.FioEnd - VgcConstants.FioBase + 1];
    private readonly Func<ushort, byte> _busRead;
    private readonly Action<ushort, byte> _busWrite;
    private readonly Func<byte, int, byte>? _vgcRead;
    private readonly Action<byte, int, byte>? _vgcWrite;
    private readonly Func<byte, int>? _vgcSpaceLength;
    private readonly Func<int, byte>? _xramRead;
    private readonly Func<int, byte, bool>? _xramWrite;
    private readonly Func<int>? _xramCapacity;
    private readonly Action? _xramRefreshStats;
    private readonly Func<uint> _rngProvider;
    private readonly SidPlayer? _sidPlayer;
    private readonly MusicEngine? _musicEngine;
    private readonly MidiPlayback? _midiPlayback;
    private readonly WavetableSynth? _wts;
    private readonly VirtualGraphicsController? _vgc;
    private readonly Action<byte[]>? _loadRuntimeRom;
    private readonly string _saveDir;
    private readonly DeviceManager? _deviceManager;
    private readonly ZSoundController? _zsound;
    // Demand-load module store (Phase B): id -> 16K module image. Set by the host
    // (CompositeBusDevice.SetShelfModuleStore) so CMD_LOAD_MODULE can stream an image
    // into an XRAM shelf slot, mirroring the real FIO LOAD_MODULE the ESP32 services.
    private IDictionary<byte, byte[]>? _moduleStore;
    private List<FileInfo>? _dirFiles;
    private List<StorageDirEntry>? _dirEntries;
    private IStorageDevice? _dirDevice;
    private int _dirIndex;
    private bool _dirFiltered;
    private string _wtsBankName = string.Empty;
    private const int MaxOpenFiles = 8;
    private readonly OpenFileHandle?[] _openFiles = new OpenFileHandle?[MaxOpenFiles];

    private sealed class OpenFileHandle
    {
        public required string Filename { get; init; }
        public byte[] Data { get; set; } = [];
        public int Position { get; set; }
        public bool CanRead { get; init; }
        public bool CanWrite { get; init; }
        public bool Dirty { get; set; }
    }

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
        DeviceManager? deviceManager = null,
        WavetableSynth? wts = null,
        VirtualGraphicsController? vgc = null,
        Func<int, byte>? xramRead = null,
        Func<int, byte, bool>? xramWrite = null,
        Func<int>? xramCapacity = null,
        Action? xramRefreshStats = null,
        Action<byte[]>? loadRuntimeRom = null,
        Func<uint>? rngProvider = null,
        ZSoundController? zsound = null)
    {
        _busRead = busRead;
        _busWrite = busWrite;
        _vgcRead = vgcRead;
        _vgcWrite = vgcWrite;
        _vgcSpaceLength = vgcSpaceLength;
        _xramRead = xramRead;
        _xramWrite = xramWrite;
        _xramCapacity = xramCapacity;
        _xramRefreshStats = xramRefreshStats;
        _rngProvider = rngProvider ?? DefaultRandomUInt32;
        _sidPlayer = sidPlayer;
        _musicEngine = musicEngine;
        _midiPlayback = midiPlayback;
        _wts = wts;
        _vgc = vgc;
        _loadRuntimeRom = loadRuntimeRom;
        _zsound = zsound;
        _deviceManager = deviceManager;
        _saveDir = saveDir ?? Path.Combine(
            Environment.GetFolderPath(Environment.SpecialFolder.UserProfile),
            "e6502-programs");

        // Wire up auto-soundfont loading for MML playback with WTS voices
        if (_musicEngine is not null && _wts is not null)
            _musicEngine.OnWtsSoundfontNeeded = () =>
            {
                if (MidiAutoSoundfont.TryLoad(this, _wts))
                    SetCurrentSoundfontName(MidiAutoSoundfont.DefaultSoundfont);
            };
    }

    private static uint DefaultRandomUInt32()
    {
        Span<byte> bytes = stackalloc byte[4];
        RandomNumberGenerator.Fill(bytes);
        return (uint)bytes[0]
            | ((uint)bytes[1] << 8)
            | ((uint)bytes[2] << 16)
            | ((uint)bytes[3] << 24);
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

    public void Reset()
    {
        Array.Clear(_regs);
        _dirFiles = null;
        _dirEntries = null;
        _dirDevice = null;
        _dirIndex = 0;
        _dirFiltered = false;
        Array.Clear(_openFiles);
    }

    /// <summary>
    /// Provides the demand-load module store consulted by CMD_LOAD_MODULE: maps a module
    /// id to its 16K image. Mirrors the SD-backed module library the ESP32 streams from on
    /// real hardware. Persists across Reset() (the library is part of the firmware, not state).
    /// </summary>
    internal void SetModuleStore(IDictionary<byte, byte[]>? store) => _moduleStore = store;

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
            case VgcConstants.FioCmdXLoad:
                DoXLoad();
                break;
            case VgcConstants.FioCmdXSave:
                DoXSave();
                break;
            case VgcConstants.FioCmdXPage:
                DoXPage();
                break;
            case VgcConstants.FioCmdRng:
                DoRng();
                break;
            case VgcConstants.FioCmdNvgLoad:
                DoNvgLoad();
                break;
            case VgcConstants.FioCmdLoadModule:
                DoLoadModule();
                break;
            case VgcConstants.FioCmdFOpen:
                DoFOpen(create: false);
                break;
            case VgcConstants.FioCmdFCreate:
                DoFOpen(create: true);
                break;
            case VgcConstants.FioCmdFClose:
                DoFClose();
                break;
            case VgcConstants.FioCmdFRead:
                DoFRead();
                break;
            case VgcConstants.FioCmdFWrite:
                DoFWrite();
                break;
            case VgcConstants.FioCmdFSeek:
                DoFSeek();
                break;
            case VgcConstants.FioCmdFTell:
                DoFTell();
                break;
            case VgcConstants.FioCmdFSize:
                DoFSize();
                break;
            case VgcConstants.FioCmdFResize:
                DoFResize();
                break;
            case VgcConstants.FioCmdFFlush:
                DoFFlush();
                break;
            case VgcConstants.FioCmdFStatus:
                DoFStatus();
                break;
            case VgcConstants.FioCmdFDelete:
                DoFDelete();
                break;
            case VgcConstants.FioCmdFRename:
                DoFRename();
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
            case VgcConstants.FioCmdZSound:
                DoZSound();
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
            case VgcConstants.FioCmdClearErr:
                SetOk();
                break;
            case VgcConstants.FioCmdLoadRuntime:
                DoLoadRuntime();
                break;
            case VgcConstants.FioCmdSfLoad:
                DoSfLoad();
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

    private void DoRng()
    {
        uint value = _rngProvider();
        _regs[VgcConstants.FioRng0 - VgcConstants.FioBase] = (byte)(value & 0xFF);
        _regs[VgcConstants.FioRng1 - VgcConstants.FioBase] = (byte)((value >> 8) & 0xFF);
        _regs[VgcConstants.FioRng2 - VgcConstants.FioBase] = (byte)((value >> 16) & 0xFF);
        _regs[VgcConstants.FioRng3 - VgcConstants.FioBase] = (byte)((value >> 24) & 0xFF);
        SetOk();
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

            byte fioType = ResolveLoadSaveType(filename)
                ?? _regs[VgcConstants.FioDirType - VgcConstants.FioBase];
            if (fioType == VgcConstants.FioDirTypeForth)
            {
                SaveRawRamFile(filename, ".4th", VgcConstants.FioDirTypeForth);
                return;
            }
            if (fioType != VgcConstants.FioDirTypeBas && fioType != VgcConstants.FioDirTypeBin)
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

            string defaultExtension = fioType == VgcConstants.FioDirTypeBin ? ".bin" : ".bas";
            var resolved = ResolveDevice(filename);
            if (resolved is not null)
            {
                var (device, name, savedDir) = resolved.Value;
                try
                {
                    device.Save(name, data, defaultExtension);

                    // Companion .md — only on HostDirectoryDevice (skip for NDI floppy)
                    bool isNewDoc = false;
                    if (fioType == VgcConstants.FioDirTypeBas && device is HostDirectoryDevice hdd)
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
            string path = GetFullPath(filename, defaultExtension);
            File.WriteAllBytes(path, data);

            // Create companion .md if it doesn't exist
            string mdPathFallback = Path.ChangeExtension(path, ".md");
            bool isNewDocFallback = fioType == VgcConstants.FioDirTypeBas && !File.Exists(mdPathFallback);
            if (fioType == VgcConstants.FioDirTypeBas && isNewDocFallback)
                File.WriteAllText(mdPathFallback, $"---\ntitle: {filename}\ntype: program\ncategory: Programs\nkeywords: [{filename}]\n---\n\n");

            _regs[VgcConstants.FioDirType - VgcConstants.FioBase] = fioType;
            SetTransferSize(length);
            SetOk();
            ProgramSaved?.Invoke(filename, isNewDocFallback);
        }
        catch
        {
            SetError(VgcConstants.FioErrIo);
        }
    }

    private void SaveRawRamFile(string filename, string defaultExtension, byte fioType)
    {
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
        byte[] data = new byte[length];
        for (int i = 0; i < length; i++)
            data[i] = _busRead((ushort)(srcAddr + i));

        SaveDataFile(filename, defaultExtension, data);
        _regs[VgcConstants.FioDirType - VgcConstants.FioBase] = fioType;
        SetTransferSize(length);
        SetOk();
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

            byte? requestedType = ResolveLoadSaveType(filename);
            requestedType ??= _regs[VgcConstants.FioDirType - VgcConstants.FioBase] switch
            {
                VgcConstants.FioDirTypeForth => VgcConstants.FioDirTypeForth,
                VgcConstants.FioDirTypeBin => VgcConstants.FioDirTypeBin,
                _ => null
            };

            if (requestedType == VgcConstants.FioDirTypeForth)
            {
                LoadRawRamFile(filename, ".4th", VgcConstants.FioDirTypeForth);
                return;
            }
            if (requestedType == VgcConstants.FioDirTypeBin)
            {
                LoadPrefixedRamFile(filename, ".bin", VgcConstants.FioDirTypeBin, useFileLoadAddress: true);
                return;
            }
            if (requestedType == VgcConstants.FioDirTypeBas)
            {
                LoadPrefixedRamFile(filename, ".bas", VgcConstants.FioDirTypeBas, useFileLoadAddress: false);
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

    private void LoadRawRamFile(string filename, string defaultExtension, byte fioType)
    {
        byte[]? data = LoadDataFile(filename, defaultExtension, out bool notFound);
        if (data is null)
        {
            SetError(notFound ? VgcConstants.FioErrNotFound : VgcConstants.FioErrIo);
            return;
        }

        int destAddr = _regs[VgcConstants.FioSrcL - VgcConstants.FioBase]
                     | (_regs[VgcConstants.FioSrcH - VgcConstants.FioBase] << 8);
        int maxLen = GetFioTransferLength();
        if ((maxLen > 0 && data.Length > maxLen) || destAddr < 0 || destAddr + data.Length > 0x10000)
        {
            SetError(VgcConstants.FioErrIo);
            return;
        }

        for (int i = 0; i < data.Length; i++)
            _busWrite((ushort)(destAddr + i), data[i]);

        _regs[VgcConstants.FioDirType - VgcConstants.FioBase] = fioType;
        SetTransferSize(data.Length);
        SetOk();
        ProgramLoaded?.Invoke(Path.GetFileNameWithoutExtension(filename));
    }

    private void LoadPrefixedRamFile(string filename, string defaultExtension, byte fioType, bool useFileLoadAddress)
    {
        byte[]? data = LoadDataFile(filename, defaultExtension, out bool notFound);
        if (data is null)
        {
            SetError(notFound ? VgcConstants.FioErrNotFound : VgcConstants.FioErrIo);
            return;
        }

        if (data.Length < 2)
        {
            SetError(VgcConstants.FioErrIo);
            return;
        }

        int dataLength = data.Length - 2;
        int destAddr = useFileLoadAddress
            ? data[0] | (data[1] << 8)
            : _regs[VgcConstants.FioSrcL - VgcConstants.FioBase]
              | (_regs[VgcConstants.FioSrcH - VgcConstants.FioBase] << 8);
        if (destAddr < 0 || destAddr + dataLength > 0x10000)
        {
            SetError(VgcConstants.FioErrIo);
            return;
        }

        if (useFileLoadAddress)
        {
            _regs[VgcConstants.FioSrcL - VgcConstants.FioBase] = (byte)(destAddr & 0xFF);
            _regs[VgcConstants.FioSrcH - VgcConstants.FioBase] = (byte)((destAddr >> 8) & 0xFF);
        }

        for (int i = 0; i < dataLength; i++)
            _busWrite((ushort)(destAddr + i), data[2 + i]);

        _regs[VgcConstants.FioDirType - VgcConstants.FioBase] = fioType;
        SetTransferSize(dataLength);
        SetOk();
        ProgramLoaded?.Invoke(Path.GetFileNameWithoutExtension(filename));
    }

    private void DoLoadRuntime()
    {
        if (_loadRuntimeRom is null)
        {
            SetError(VgcConstants.FioErrIo);
            return;
        }

        try
        {
            string? filename = ReadFilename();
            if (filename is null)
            {
                SetError(VgcConstants.FioErrIo);
                return;
            }

            // NovaHost resolves LOADRUNTIME through the DeviceManager, so runtimes
            // shipped on the mounted floppy (the AUTOBOOT contract) win. The host
            // <saveDir>/roms/ library (mirrors NovaHost's SD /roms/) is the fallback
            // for runtimes not on the inserted disk.
            byte[]? data = LoadDataFile(filename, ".bin", out bool notFound);
            if (data is null && notFound)
            {
                var (baseName, extension) = SplitDataFilename(filename, ".bin");
                string romPath = Path.Combine(_saveDir, "roms", baseName + extension);
                if (File.Exists(romPath))
                    data = File.ReadAllBytes(romPath);
            }
            if (data is null)
            {
                SetError(notFound ? VgcConstants.FioErrNotFound : VgcConstants.FioErrIo);
                return;
            }
            if (data.Length != VgcConstants.RomSize)
            {
                SetError(VgcConstants.FioErrIo);
                return;
            }

            _loadRuntimeRom(data);
            SetTransferSize(data.Length);
            SetOk();
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
            int filterLen = _regs[VgcConstants.FioNameLen - VgcConstants.FioBase];

            if (filterLen > 0)
            {
                // Filtered enumeration
                string? rawPattern = ReadFilenameRaw();
                if (rawPattern is null)
                {
                    SetError(VgcConstants.FioErrIo);
                    return;
                }

                var filter = ParseFilterPattern(rawPattern);

                // Resolve device
                IStorageDevice? device = null;
                if (_deviceManager is not null)
                {
                    string deviceName = filter.DevicePrefix ?? _deviceManager.DefaultDevice;
                    device = _deviceManager.GetDevice(deviceName);
                    if (!device.IsMounted)
                    {
                        SetError(VgcConstants.FioErrNotMounted);
                        return;
                    }
                }

                if (device is null)
                {
                    // No device manager — filtered enumeration not supported on filesystem fallback
                    SetError(VgcConstants.FioErrIo);
                    return;
                }

                // CD into subdirectory if specified
                string? savedDir = null;
                if (filter.DirectoryPath is not null)
                {
                    savedDir = device.CurrentDirectory;
                    device.CurrentDirectory = filter.DirectoryPath;
                }

                List<StorageDirEntry> filtered;
                try
                {
                    var entries = device.ListDirectory(null);
                    filtered = [];
                    foreach (var entry in entries)
                    {
                        if (entry.IsDirectory)
                        {
                            // Include directories that match the name pattern
                            if (!GlobMatch(filter.NamePattern, entry.Filename))
                                continue;
                            filtered.Add(entry);
                            continue;
                        }

                        // Extension filter
                        if (filter.ExtFilter is not null)
                        {
                            string entryExt = NdiTypeToExt(entry.FileType);
                            string filterExt = NormalizeFilterExtension(filter.ExtFilter) ?? filter.ExtFilter;
                            if (!string.Equals(entryExt, filterExt, StringComparison.OrdinalIgnoreCase))
                                continue;
                        }

                        // Name glob filter
                        if (!GlobMatch(filter.NamePattern, entry.Filename))
                            continue;

                        filtered.Add(entry);
                    }
                }
                finally
                {
                    if (savedDir is not null)
                        device.CurrentDirectory = savedDir;
                }

                _dirEntries = filtered;
                _dirFiles = null;
                _dirDevice = device;
                _dirIndex = 0;
                _dirFiltered = true;

                if (_dirEntries.Count > 0)
                {
                    PopulateDirEntryFromStorage(_dirEntries[0]);
                    PopulateMetadata(_dirEntries[0]);
                    SetOk();
                }
                else
                {
                    SetEndOfDir();
                }
                return;
            }

            // Unfiltered enumeration — existing behavior unchanged
            _dirFiltered = false;

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
                      .Concat(dir.GetFiles("*.nms"))
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
                if (_dirFiltered) PopulateMetadata(_dirEntries[_dirIndex]);
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

    private void DoFOpen(bool create)
    {
        try
        {
            string? filename = ReadFilename();
            if (filename is null)
            {
                SetError(VgcConstants.FioErrIo);
                return;
            }

            byte fam = _regs[VgcConstants.FioDirType - VgcConstants.FioBase];
            if (!DecodeForthFileAccess(fam, out bool canRead, out bool canWrite))
            {
                SetError(VgcConstants.FioErrIo);
                return;
            }

            byte[] data;
            bool dirty = create;
            if (create)
            {
                data = [];
            }
            else
            {
                data = LoadExactDataFile(filename, out bool notFound) ?? [];
                if (notFound)
                {
                    SetError(VgcConstants.FioErrNotFound);
                    return;
                }
            }

            int fileId = AllocateFileHandle(new OpenFileHandle
            {
                Filename = filename,
                Data = data,
                CanRead = canRead,
                CanWrite = canWrite,
                Dirty = dirty
            });

            if (fileId == 0)
            {
                SetError(VgcConstants.FioErrIo);
                return;
            }

            _regs[VgcConstants.FioSrcL - VgcConstants.FioBase] = (byte)fileId;
            _regs[VgcConstants.FioSrcH - VgcConstants.FioBase] = 0;
            SetTransferSize24(data.Length);
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

    private void DoFClose()
    {
        int fileId = _regs[VgcConstants.FioSrcL - VgcConstants.FioBase];
        var handle = GetFileHandle(fileId);
        if (handle is null)
        {
            SetError(VgcConstants.FioErrIo);
            return;
        }

        try
        {
            if (handle.Dirty)
                SaveExactDataFile(handle.Filename, handle.Data);
            _openFiles[fileId - 1] = null;
            SetOk();
        }
        catch
        {
            SetError(VgcConstants.FioErrIo);
        }
    }

    private void DoFRead()
    {
        int fileId = _regs[VgcConstants.FioSrcL - VgcConstants.FioBase];
        var handle = GetFileHandle(fileId);
        if (handle is null || !handle.CanRead)
        {
            SetError(VgcConstants.FioErrIo);
            return;
        }

        int dest = _regs[VgcConstants.FioEndL - VgcConstants.FioBase]
                 | (_regs[VgcConstants.FioEndH - VgcConstants.FioBase] << 8);
        int requested = GetFioTransferLength();
        int available = Math.Max(0, handle.Data.Length - handle.Position);
        int count = Math.Min(requested, available);
        if (!CpuRangeOk(dest, count))
        {
            SetError(VgcConstants.FioErrIo);
            return;
        }

        for (int i = 0; i < count; i++)
            _busWrite((ushort)(dest + i), handle.Data[handle.Position + i]);

        handle.Position += count;
        SetTransferSize24(count);
        SetOk();
    }

    private void DoFWrite()
    {
        int fileId = _regs[VgcConstants.FioSrcL - VgcConstants.FioBase];
        var handle = GetFileHandle(fileId);
        if (handle is null || !handle.CanWrite)
        {
            SetError(VgcConstants.FioErrIo);
            return;
        }

        int src = _regs[VgcConstants.FioEndL - VgcConstants.FioBase]
                | (_regs[VgcConstants.FioEndH - VgcConstants.FioBase] << 8);
        int count = GetFioTransferLength();
        if (!CpuRangeOk(src, count))
        {
            SetError(VgcConstants.FioErrIo);
            return;
        }

        int end = handle.Position + count;
        if (end < handle.Position)
        {
            SetError(VgcConstants.FioErrIo);
            return;
        }
        if (end > handle.Data.Length)
        {
            byte[] resized = handle.Data;
            Array.Resize(ref resized, end);
            handle.Data = resized;
        }

        for (int i = 0; i < count; i++)
            handle.Data[handle.Position + i] = _busRead((ushort)(src + i));

        handle.Position = end;
        handle.Dirty = true;
        SetTransferSize24(count);
        SetOk();
    }

    private void DoFSeek()
    {
        int fileId = _regs[VgcConstants.FioSrcL - VgcConstants.FioBase];
        var handle = GetFileHandle(fileId);
        if (handle is null)
        {
            SetError(VgcConstants.FioErrIo);
            return;
        }

        handle.Position = GetFioSize24();
        SetOk();
    }

    private void DoFTell()
    {
        int fileId = _regs[VgcConstants.FioSrcL - VgcConstants.FioBase];
        var handle = GetFileHandle(fileId);
        if (handle is null)
        {
            SetError(VgcConstants.FioErrIo);
            return;
        }

        SetTransferSize24(handle.Position);
        SetOk();
    }

    private void DoFSize()
    {
        int fileId = _regs[VgcConstants.FioSrcL - VgcConstants.FioBase];
        var handle = GetFileHandle(fileId);
        if (handle is null)
        {
            SetError(VgcConstants.FioErrIo);
            return;
        }

        SetTransferSize24(handle.Data.Length);
        SetOk();
    }

    private void DoFResize()
    {
        int fileId = _regs[VgcConstants.FioSrcL - VgcConstants.FioBase];
        var handle = GetFileHandle(fileId);
        if (handle is null || !handle.CanWrite)
        {
            SetError(VgcConstants.FioErrIo);
            return;
        }

        int size = GetFioSize24();
        byte[] resized = handle.Data;
        Array.Resize(ref resized, size);
        handle.Data = resized;
        if (handle.Position > size)
            handle.Position = size;
        handle.Dirty = true;
        SetOk();
    }

    private void DoFFlush()
    {
        int fileId = _regs[VgcConstants.FioSrcL - VgcConstants.FioBase];
        var handle = GetFileHandle(fileId);
        if (handle is null)
        {
            SetError(VgcConstants.FioErrIo);
            return;
        }

        try
        {
            if (handle.Dirty)
            {
                SaveExactDataFile(handle.Filename, handle.Data);
                handle.Dirty = false;
            }
            SetOk();
        }
        catch
        {
            SetError(VgcConstants.FioErrIo);
        }
    }

    private void DoFStatus()
    {
        try
        {
            string? filename = ReadFilename();
            if (filename is null)
            {
                SetError(VgcConstants.FioErrIo);
                return;
            }

            if (!ExactDataFileExists(filename))
            {
                SetError(VgcConstants.FioErrNotFound);
                return;
            }

            _regs[VgcConstants.FioSrcL - VgcConstants.FioBase] = 3; // implementation-defined fam: readable + writable
            _regs[VgcConstants.FioSrcH - VgcConstants.FioBase] = 0;
            SetOk();
        }
        catch
        {
            SetError(VgcConstants.FioErrIo);
        }
    }

    private void DoFDelete()
    {
        try
        {
            string? filename = ReadFilename();
            if (filename is null)
            {
                SetError(VgcConstants.FioErrIo);
                return;
            }

            if (!DeleteExactDataFile(filename))
            {
                SetError(VgcConstants.FioErrNotFound);
                return;
            }

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

    private void DoFRename()
    {
        try
        {
            string? oldName = ReadFilename();
            if (oldName is null)
            {
                SetError(VgcConstants.FioErrIo);
                return;
            }

            int newNameAddr = _regs[VgcConstants.FioEndL - VgcConstants.FioBase]
                            | (_regs[VgcConstants.FioEndH - VgcConstants.FioBase] << 8);
            int newNameLen = GetFioTransferLength();
            string? newName = ReadCpuFilename(newNameAddr, newNameLen);
            if (newName is null)
            {
                SetError(VgcConstants.FioErrIo);
                return;
            }

            if (!RenameExactDataFile(oldName, newName))
            {
                SetError(VgcConstants.FioErrNotFound);
                return;
            }

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
                    var (baseName, extension) = SplitDataFilename(name, ".gfx");
                    device.Save(baseName, data, extension);
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
                    var (baseName, extension) = SplitDataFilename(name, ".gfx");
                    if (!device.FileExists(baseName, extension))
                    {
                        SetError(VgcConstants.FioErrNotFound);
                        return;
                    }
                    data = device.Load(baseName, extension);
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

    private void DoNvgLoad()
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
            if (space != VgcConstants.VramPlaneGfx)
            {
                SetError(VgcConstants.FioErrIo);
                return;
            }

            int dest = GetFioGraphicsAddress();
            byte[]? data = LoadDataFile(filename, ".nvg", out bool notFound);
            if (data is null)
            {
                SetError(notFound ? VgcConstants.FioErrNotFound : VgcConstants.FioErrIo);
                return;
            }

            int spaceLen = _vgcSpaceLength(space);
            if (spaceLen <= 0)
            {
                SetError(VgcConstants.FioErrIo);
                return;
            }
            for (int i = 0; i < spaceLen; i++)
                _vgcWrite(space, i, 0);

            if (!DecodeNvgToGraphics(data, space, dest, out int pixelsWritten))
            {
                SetError(VgcConstants.FioErrIo);
                return;
            }

            SetTransferSize(pixelsWritten);
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

    private bool DecodeNvgToGraphics(byte[] data, byte space, int dest, out int pixelsWritten)
    {
        pixelsWritten = 0;
        if (data.Length < 12 ||
            data[0] != (byte)'N' ||
            data[1] != (byte)'V' ||
            data[2] != (byte)'G' ||
            data[3] != (byte)'1')
            return false;

        int width = data[4] | (data[5] << 8);
        int height = data[6] | (data[7] << 8);
        int spanCount = data[8] | (data[9] << 8) | (data[10] << 16) | (data[11] << 24);
        if (width <= 0 || height <= 0 || width > 320 || height > 200 || spanCount < 0)
            return false;

        int baseX = dest % 320;
        int baseY = dest / 320;
        if (baseX < 0 || baseY < 0 || baseX + width > 320 || baseY + height > 200)
            return false;

        int spaceLen = _vgcSpaceLength?.Invoke(space) ?? 0;
        if (spaceLen <= 0)
            return false;

        int imageLen = width * height;
        int offset = 12;
        for (int span = 0; span < spanCount; span++)
        {
            if (offset + 3 > data.Length)
                return false;

            int addr = data[offset] | (data[offset + 1] << 8);
            int len = data[offset + 2];
            offset += 3;

            if (len == 0 || addr < 0 || addr + len > imageLen || offset + len > data.Length)
                return false;

            int imagePos = addr;
            int pos = 0;
            while (pos < len)
            {
                int srcX = imagePos % width;
                int srcY = imagePos / width;
                int chunk = Math.Min(len - pos, width - srcX);
                int vgcAddr = dest + (srcY * 320) + srcX;

                if (vgcAddr < 0 || vgcAddr + chunk > spaceLen)
                    return false;

                for (int i = 0; i < chunk; i++)
                    _vgcWrite!(space, vgcAddr + i, data[offset + pos + i]);

                imagePos += chunk;
                pos += chunk;
                pixelsWritten += chunk;
            }

            offset += len;
        }

        return true;
    }

    // -------------------------------------------------------------------------
    // Demand-load module (Phase B): stream a module image from the host store into
    // an XRAM shelf slot. The CPU's miss-handler writes id (OFF_SRC_LO) + dest slot
    // (OFF_END_LO) then triggers CMD_LOAD_MODULE; the host streams 16K into the slot
    // and sets STATUS. Emulator analogue of the FIO LOAD_MODULE the ESP32 services.
    // -------------------------------------------------------------------------

    private void DoLoadModule()
    {
        if (_xramWrite is null || _moduleStore is null)
        {
            SetError(VgcConstants.FioErrIo);
            return;
        }

        byte id = _regs[VgcConstants.FioSrcL - VgcConstants.FioBase];
        int slot = _regs[VgcConstants.FioEndL - VgcConstants.FioBase];

        if (slot < 0 || slot >= VgcConstants.ShelfN)
        {
            SetError(VgcConstants.FioErrIo);
            return;
        }

        if (!_moduleStore.TryGetValue(id, out byte[]? image) ||
            image is null ||
            image.Length != VgcConstants.ShelfSlotBytes)
        {
            SetError(VgcConstants.FioErrIo);
            return;
        }

        int dest = VgcConstants.ShelfBaseAddr + slot * VgcConstants.ShelfSlotBytes;
        for (int i = 0; i < image.Length; i++)
        {
            if (!_xramWrite(dest + i, image[i]))
            {
                SetError(VgcConstants.FioErrIo);
                return;
            }
        }

        _xramRefreshStats?.Invoke();
        SetTransferSize(image.Length);
        SetOk();
    }

    // -------------------------------------------------------------------------
    // XRAM file I/O (.xram by default, explicit filename extensions honored)
    // -------------------------------------------------------------------------

    private void DoXLoad()
    {
        try
        {
            if (_xramWrite is null || _xramCapacity is null)
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

            int xaddr = GetFioXramAddress();
            int reqLen = GetFioTransferLength();

            byte[]? data = LoadDataFile(filename, ".xram", out bool notFound);
            if (data is null)
            {
                SetError(notFound ? VgcConstants.FioErrNotFound : VgcConstants.FioErrIo);
                return;
            }

            int len = reqLen > 0 ? Math.Min(reqLen, data.Length) : data.Length;
            if (!XramRangeOk(xaddr, len))
            {
                SetError(VgcConstants.FioErrIo);
                return;
            }

            for (int i = 0; i < len; i++)
            {
                if (!_xramWrite(xaddr + i, data[i]))
                {
                    SetError(VgcConstants.FioErrIo);
                    return;
                }
            }

            _xramRefreshStats?.Invoke();
            SetTransferSize(len);
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

    private void DoXSave()
    {
        try
        {
            if (_xramRead is null || _xramCapacity is null)
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

            int xaddr = GetFioXramAddress();
            int len = GetFioTransferLength();
            if (len <= 0 || !XramRangeOk(xaddr, len))
            {
                SetError(VgcConstants.FioErrIo);
                return;
            }

            byte[] data = new byte[len];
            for (int i = 0; i < len; i++)
                data[i] = _xramRead(xaddr + i);

            SaveDataFile(filename, ".xram", data);

            SetTransferSize(len);
            SetOk();
        }
        catch
        {
            SetError(VgcConstants.FioErrIo);
        }
    }

    private void DoXPage()
    {
        try
        {
            string? filename = ReadFilename();
            if (filename is null)
            {
                SetError(VgcConstants.FioErrIo);
                return;
            }

            byte target = _regs[VgcConstants.FioDirType - VgcConstants.FioBase];
            int reqLen = GetFioTransferLength();
            int fileOffset = GetFioFileOffset();
            if (reqLen <= 0 || fileOffset < 0)
            {
                SetError(VgcConstants.FioErrIo);
                return;
            }

            byte[]? data = LoadDataFile(filename, ".bin", out bool notFound);
            if (data is null)
            {
                SetError(notFound ? VgcConstants.FioErrNotFound : VgcConstants.FioErrIo);
                return;
            }
            if (fileOffset >= data.Length)
            {
                SetError(VgcConstants.FioErrIo);
                return;
            }

            int len = Math.Min(reqLen, data.Length - fileOffset);
            switch (target)
            {
                case VgcConstants.FioPageTargetXram:
                {
                    if (_xramWrite is null || _xramCapacity is null)
                    {
                        SetError(VgcConstants.FioErrIo);
                        return;
                    }

                    int xaddr = GetFioXramAddress();
                    if (!XramRangeOk(xaddr, len))
                    {
                        SetError(VgcConstants.FioErrIo);
                        return;
                    }

                    for (int i = 0; i < len; i++)
                    {
                        if (!_xramWrite(xaddr + i, data[fileOffset + i]))
                        {
                            SetError(VgcConstants.FioErrIo);
                            return;
                        }
                    }

                    _xramRefreshStats?.Invoke();
                    break;
                }
                case VgcConstants.FioPageTargetRam:
                {
                    int addr = GetFioGraphicsAddress();
                    if (addr < 0 || len < 0 || addr + len > 0x10000)
                    {
                        SetError(VgcConstants.FioErrIo);
                        return;
                    }

                    for (int i = 0; i < len; i++)
                        _busWrite((ushort)(addr + i), data[fileOffset + i]);
                    break;
                }
                case VgcConstants.FioPageTargetVgc:
                {
                    if (_vgcWrite is null || _vgcSpaceLength is null)
                    {
                        SetError(VgcConstants.FioErrIo);
                        return;
                    }

                    byte space = _regs[VgcConstants.FioGSpace - VgcConstants.FioBase];
                    int addr = GetFioGraphicsAddress();
                    int spaceLen = _vgcSpaceLength(space);
                    if (spaceLen <= 0 || addr < 0 || len < 0 || addr + len > spaceLen)
                    {
                        SetError(VgcConstants.FioErrIo);
                        return;
                    }

                    for (int i = 0; i < len; i++)
                        _vgcWrite(space, addr + i, data[fileOffset + i]);
                    break;
                }
                case VgcConstants.FioPageTargetGfx4:
                {
                    // The NovaZ picture path (PICS.PAK): unpack a 4bpp
                    // row-packed slice (two pixels per byte, high nibble
                    // left) into the 1-byte-per-pixel gfx plane.
                    //   FioGAddr  = start PIXEL address (y*320 + x)
                    //   FioGSpace = source bytes per row (1..160)
                    //   FioEndH   = flags: bit 0 + high nibble = transparent
                    //               index (skip those pixels), bit 1 = odd
                    //               width (the last pixel per row is pad)
                    // Rows clip at the 320px right edge; rows past the 200px
                    // bottom edge are dropped. (Hardware note for M6: the
                    // FPGA gfx plane is 4bpp PACKED, so this unpack is
                    // Avalonia-only — NovaHost streams pak bytes as-is and
                    // the RTL blitter's nibble color key handles the skip.)
                    if (_vgcWrite is null || _vgcSpaceLength is null)
                    {
                        SetError(VgcConstants.FioErrIo);
                        return;
                    }

                    int rowBytes = _regs[VgcConstants.FioGSpace - VgcConstants.FioBase];
                    byte flags = _regs[VgcConstants.FioEndH - VgcConstants.FioBase];
                    int startPixel = GetFioGraphicsAddress();
                    int gfxLen = _vgcSpaceLength(VgcConstants.MemSpaceGfx);
                    if (rowBytes <= 0 || rowBytes > VgcConstants.GfxWidth / 2 ||
                        startPixel < 0 || startPixel >= gfxLen)
                    {
                        SetError(VgcConstants.FioErrIo);
                        return;
                    }

                    bool transparent = (flags & 0x01) != 0;
                    byte transparentIndex = (byte)(flags >> 4);
                    // Transparent source pixels normally skip, preserving art
                    // drawn earlier; when the destination is still EMPTY
                    // (colour 0 = the display-transparent index) the
                    // underpaint colour (FioSizeL, read before the size
                    // registers are rewritten) fills it instead — the MCGA
                    // framebuffer would have held the parchment field there.
                    byte underpaint = _regs[VgcConstants.FioSizeL - VgcConstants.FioBase];
                    int widthPx = rowBytes * 2 - ((flags & 0x02) != 0 ? 1 : 0);
                    int x0 = startPixel % VgcConstants.GfxWidth;
                    int y0 = startPixel / VgcConstants.GfxWidth;
                    int rows = len / rowBytes;
                    for (int row = 0; row < rows; row++)
                    {
                        int y = y0 + row;
                        if (y >= VgcConstants.GfxHeight)
                            break;
                        int src = fileOffset + row * rowBytes;
                        for (int p = 0; p < widthPx; p++)
                        {
                            int x = x0 + p;
                            if (x >= VgcConstants.GfxWidth)
                                break;
                            byte pair = data[src + p / 2];
                            byte color = (p & 1) == 0 ? (byte)(pair >> 4) : (byte)(pair & 0x0F);
                            int dst = y * VgcConstants.GfxWidth + x;
                            if (transparent && color == transparentIndex)
                            {
                                if (underpaint != 0 && _vgcRead is not null &&
                                    _vgcRead(VgcConstants.MemSpaceGfx, dst) == 0)
                                    _vgcWrite(VgcConstants.MemSpaceGfx, dst, underpaint);
                                continue;
                            }
                            _vgcWrite(VgcConstants.MemSpaceGfx, dst, color);
                        }
                    }
                    break;
                }
                default:
                    SetError(VgcConstants.FioErrIo);
                    return;
            }

            SetTransferSize(len);
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

    /// <summary>
    /// Test observability: the 1-based SID song number the last SIDPLAY request
    /// carried in FioSrcL (what the BASIC handler + SOUND module marshalled into
    /// AUDIO.NOTE = FioSrcL). Captured at the top of <see cref="DoSidPlay"/> so it
    /// reflects the requested song even when playback is skipped (no .sid file).
    /// </summary>
    internal int LastSidPlaySongRequested { get; private set; } = -1;

    private void DoSidPlay()
    {
        if (_sidPlayer is null) { SetError(VgcConstants.FioErrIo); return; }

        LastSidPlaySongRequested = _regs[VgcConstants.FioSrcL - VgcConstants.FioBase];

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

            PublishSidPlaybackMetadata(data, filename);
            _sidPlayer.Play(info, song);
            SetOk();
        }
        catch (FileNotFoundException ex) { Console.Error.WriteLine($"[SID] File not found: {ex.Message}"); SetError(VgcConstants.FioErrNotFound); }
        catch (Exception ex) { Console.Error.WriteLine($"[SID] Error: {ex.Message}"); SetError(VgcConstants.FioErrIo); }
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

    // Z-machine sampled sound. number in FioSrcL, effect (2=start,3=stop) in
    // FioSrcH, volume level in FioEndL, repeats (255=loop) in FioEndH.
    private void DoZSound()
    {
        if (_zsound is null) { SetError(VgcConstants.FioErrIo); return; }
        int number  = _regs[VgcConstants.FioSrcL - VgcConstants.FioBase];
        int effect  = _regs[VgcConstants.FioSrcH - VgcConstants.FioBase];
        int level   = _regs[VgcConstants.FioEndL - VgcConstants.FioBase];
        int repeats = _regs[VgcConstants.FioEndH - VgcConstants.FioBase];

        if (effect == 3) { _zsound.Stop(); SetOk(); return; }

        if (!_zsound.PackLoaded)
            TryLoadSoundPack();

        // effect 1 = prepare: load the bank now, but do not play. Matches the
        // NovaHost path so the soundfont is resident before the first start.
        if (effect == 1) { SetOk(); return; }

        float gain = (level <= 0 || level >= 255) ? 1f : Math.Clamp(level / 8f, 0f, 1f);
        bool loop = repeats == 255;
        _zsound.Play(number, gain, loop);
        SetOk();
    }

    private void TryLoadSoundPack()
    {
        if (_deviceManager is null) return;
        try
        {
            var (device, _) = _deviceManager.ResolveFilename("SOUND.PAK");
            if (device.IsMounted && device.FileExists("SOUND", ".PAK"))
                _zsound!.LoadPack(device.Load("SOUND", ".PAK"));
        }
        catch { /* no sound pack on this disk; sampled sounds stay silent */ }
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

            // Parse optional voice mapping from filename string
            var parsed = MidPlayParser.Parse(filename);
            filename = parsed.Filename;
            var mapping = parsed.Mapping;

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
            var mode = _midiPlayback.RoutingMode;

            // Auto-load soundfont if WTS is empty and not in SID-only mode
            if (mode != MidiRoutingMode.SidOnly && _wts is not null && _wts.InstrumentCount == 0)
            {
                if (MidiAutoSoundfont.TryLoad(this, _wts))
                    SetCurrentSoundfontName(MidiAutoSoundfont.DefaultSoundfont);
            }

            bool sidOnly = mode == MidiRoutingMode.SidOnly || (_wts is null || _wts.InstrumentCount == 0);
            int maxVoices = sidOnly ? 6 : 14;
            var rawChannels = MidiEngine.SelectChannels(midi, maxVoices);

            var routing = MidiEngine.RouteVoices(analysis, rawChannels, sidOnly,
                (slot, bucket) => _musicEngine.DefineInstrument(slot, bucket.Waveform,
                    bucket.Attack, bucket.Decay, bucket.Sustain, bucket.Release),
                mapping);

            _musicEngine.SetVolume(15);
            PublishMidiPlaybackMetadata(midi, new FileInfo(path).Length,
                Path.GetFileNameWithoutExtension(filename),
                sidOnly ? null : _wtsBankName);
            _midiPlayback.Play(midi, routing.VoiceToChannel, routing.InstrumentSlots);
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

    private void DoSfLoad()
    {
        if (_wts is null) { SetError(VgcConstants.FioErrIo); return; }

        string? filename = ReadFilename();
        if (filename is null) { SetError(VgcConstants.FioErrIo); return; }

        try
        {
            string path = string.Empty;
            var resolved = ResolveDevice(filename);
            if (resolved is not null)
            {
                var (device, name, savedDir) = resolved.Value;
                try
                {
                    if (device is e6502.Storage.HostDirectoryDevice hdd)
                    {
                        string dir = hdd.CurrentDirectory == "/"
                            ? _saveDir
                            : Path.Combine(_saveDir, hdd.CurrentDirectory.Trim('/'));
                        dir = Path.Combine(dir, "soundfonts");
                        path = Path.Combine(dir, name + ".sf2");
                    }
                    else
                    {
                        // NDI floppy: load from soundfonts/ subdir
                        string savedDevDir = device.CurrentDirectory;
                        device.CurrentDirectory = "soundfonts";
                        try
                        {
                            if (!device.FileExists(name, ".sf2"))
                            {
                                SetError(VgcConstants.FioErrNotFound);
                                return;
                            }
                            byte[] sf2Bytes = device.Load(name, ".sf2");
                            path = Path.Combine(Path.GetTempPath(), $"e6502-{name}.sf2");
                            File.WriteAllBytes(path, sf2Bytes);
                        }
                        finally
                        {
                            device.CurrentDirectory = savedDevDir;
                        }
                    }
                }
                finally
                {
                    RestoreDir(device, savedDir);
                }
            }
            else
            {
                path = Path.Combine(_saveDir, "soundfonts", filename + ".sf2");
            }

            if (!File.Exists(path))
            {
                SetError(VgcConstants.FioErrNotFound);
                return;
            }

            using var stream = File.OpenRead(path);
            var bank = Sf2Loader.Load(stream);
            _wts.LoadBank(bank);
            SetCurrentSoundfontName(filename);
            SetOk();
        }
        catch
        {
            SetError(VgcConstants.FioErrIo);
        }
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
            _deviceManager.DefaultDevice = device.Prefix;
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
    // Metadata buffer population
    // -------------------------------------------------------------------------

    private void ClearMetadata()
    {
        for (int i = VgcConstants.MetaBase; i <= VgcConstants.MetaEnd; i++)
            _busWrite((ushort)i, 0);
    }

    private void WriteMetadataHeader(int metaType, long size)
    {
        ClearMetadata();
        _busWrite((ushort)VgcConstants.MetaType, (byte)metaType);
        int clamped = (int)Math.Min(size, 0xFFFF);
        _busWrite((ushort)VgcConstants.MetaSizeL, (byte)(clamped & 0xFF));
        _busWrite((ushort)VgcConstants.MetaSizeH, (byte)((clamped >> 8) & 0xFF));
    }

    private static string NormalizeSoundfontName(string name)
    {
        string normalized = name.Replace('\\', '/');
        int slash = normalized.LastIndexOf('/');
        if (slash >= 0)
            normalized = normalized[(slash + 1)..];
        int colon = normalized.LastIndexOf(':');
        if (colon >= 0)
            normalized = normalized[(colon + 1)..];
        string ext = Path.GetExtension(normalized);
        if (!string.IsNullOrEmpty(ext))
            normalized = normalized[..^ext.Length];
        return normalized;
    }

    private void SetCurrentSoundfontName(string name)
    {
        _wtsBankName = NormalizeSoundfontName(name);
        PublishSoundfontMetadata(_wtsBankName);
    }

    private void PublishSoundfontMetadata(string? name)
    {
        for (int i = 0; i < VgcConstants.MusicMetaSoundfontLen; i++)
            _busWrite((ushort)(VgcConstants.MusicMetaSoundfont + i), 0);

        if (string.IsNullOrEmpty(name))
            return;

        WriteMetaStringFromManaged(VgcConstants.MusicMetaSoundfont,
            NormalizeSoundfontName(name), VgcConstants.MusicMetaSoundfontLen);
    }

    private void PublishSidPlaybackMetadata(byte[] data, string fallbackTitle)
    {
        WriteMetadataHeader(1, data.Length);
        ExtractSidMetadata(data);
        if (_busRead((ushort)VgcConstants.MetaTitle) == 0)
            WriteMetaStringFromManaged(VgcConstants.MetaTitle, fallbackTitle,
                VgcConstants.MetaTitleLen);
        PublishSoundfontMetadata(null);
    }

    private void PublishMidiPlaybackMetadata(MidiFile midi, long size,
        string fallbackTitle, string? soundfontName)
    {
        WriteMetadataHeader(3, size);
        try { ExtractMidiMetadata(midi); }
        catch { /* playback metadata is best-effort in the UI host */ }
        if (_busRead((ushort)VgcConstants.MetaTitle) == 0)
            WriteMetaStringFromManaged(VgcConstants.MetaTitle, fallbackTitle,
                VgcConstants.MetaTitleLen);
        PublishSoundfontMetadata(soundfontName);
    }

    private static NdiFileType ExtToNdiType(string ext) => ext.ToLowerInvariant() switch
    {
        ".sid" => NdiFileType.Sid,
        ".bin" or ".xram" => NdiFileType.Bin,
        ".mid" or ".midi" or ".nms" => NdiFileType.Mid,
        ".gfx" or ".nvg" => NdiFileType.Gfx,
        ".4th" or ".fth" or ".fs" or ".fr" => NdiFileType.Forth,
        _ => NdiFileType.Bas
    };

    private static string NdiTypeToExt(NdiFileType type) => type switch
    {
        NdiFileType.Sid => ".sid",
        NdiFileType.Bin => ".bin",
        NdiFileType.Mid => ".mid",
        NdiFileType.Gfx => ".gfx",
        NdiFileType.Forth => ".4th",
        _ => ".bas"
    };

    private static byte NdiTypeToFioDirType(NdiFileType type) => type switch
    {
        NdiFileType.Sid => VgcConstants.FioDirTypeSid,
        NdiFileType.Bin => VgcConstants.FioDirTypeBin,
        NdiFileType.Mid => VgcConstants.FioDirTypeMid,
        NdiFileType.Gfx => VgcConstants.FioDirTypeGfx,
        NdiFileType.Dir => VgcConstants.FioDirTypeDir,
        NdiFileType.Forth => VgcConstants.FioDirTypeForth,
        _ => VgcConstants.FioDirTypeBas
    };

    private static byte? ResolveLoadSaveType(string filename)
    {
        string ext = Path.GetExtension(filename).ToLowerInvariant();
        return ext switch
        {
            ".bas" => VgcConstants.FioDirTypeBas,
            ".bin" => VgcConstants.FioDirTypeBin,
            ".4th" or ".fth" or ".fs" or ".fr" => VgcConstants.FioDirTypeForth,
            "" => null,
            _ => null
        };
    }

    private static bool HasTwoByteLoadPrefix(int fioDirType) =>
        fioDirType is VgcConstants.FioDirTypeBas or VgcConstants.FioDirTypeBin;

    private void PopulateMetadata(StorageDirEntry entry)
    {
        int metaType = entry.IsDirectory ? VgcConstants.FioDirTypeDir : NdiTypeToFioDirType(entry.FileType);
        WriteMetadataHeader(metaType, entry.SizeBytes);

        // Load file bytes and extract type-specific metadata
        if (_dirDevice is null || entry.IsDirectory) return;
        try
        {
            switch (entry.FileType)
            {
                case NdiFileType.Sid:
                    ExtractSidMetadata(_dirDevice.Load(entry.Filename, NdiTypeToExt(entry.FileType)));
                    break;
                case NdiFileType.Mid:
                    ExtractMidiMetadata(_dirDevice.Load(entry.Filename, NdiTypeToExt(entry.FileType)));
                    break;
                case NdiFileType.Bin:
                    ExtractBinMetadata(_dirDevice.Load(entry.Filename, NdiTypeToExt(entry.FileType)));
                    break;
            }
        }
        catch { /* leave at defaults */ }
    }

    private void ExtractSidMetadata(byte[] data)
    {
        if (data.Length < 124) return;
        WriteMetaString(VgcConstants.MetaTitle, data, 22, VgcConstants.MetaTitleLen);
        WriteMetaString(VgcConstants.MetaAuthor, data, 54, VgcConstants.MetaAuthorLen);
        WriteMetaString(VgcConstants.MetaCopyright, data, 86, VgcConstants.MetaCopyrightLen);
        // SID stores addresses big-endian, convert to LE
        _busWrite((ushort)VgcConstants.MetaLoadL, data[9]);
        _busWrite((ushort)VgcConstants.MetaLoadH, data[8]);
        _busWrite((ushort)VgcConstants.MetaInitL, data[11]);
        _busWrite((ushort)VgcConstants.MetaInitH, data[10]);
        _busWrite((ushort)VgcConstants.MetaPlayL, data[13]);
        _busWrite((ushort)VgcConstants.MetaPlayH, data[12]);
        int songs = (data[14] << 8) | data[15];
        _busWrite((ushort)VgcConstants.MetaSongs, (byte)Math.Min(songs, 255));

        int flags = data.Length > 0x77 ? (data[0x76] << 8) | data[0x77] : 0;
        int clock = (flags >> 2) & 0x03;
        int model = (flags >> 4) & 0x03;
        byte metaFlags = model == 2
            ? VgcConstants.MusicMetaFlagSid8580
            : VgcConstants.MusicMetaFlagSid6581;
        if (clock == 2)
            metaFlags |= VgcConstants.MusicMetaFlagNtsc;
        if (data.Length > 0x7B && (data[0x7A] != 0 || data[0x7B] != 0))
            metaFlags |= VgcConstants.MusicMetaFlagStereo;
        _busWrite((ushort)VgcConstants.MusicMetaFlags, metaFlags);
    }

    private void ExtractMidiMetadata(byte[] data)
    {
        try
        {
            using var stream = new MemoryStream(data);
            var midi = Melanchall.DryWetMidi.Core.MidiFile.Read(stream);
            ExtractMidiMetadata(midi);
        }
        catch { /* MIDI parsing failed — leave at defaults */ }
    }

    private void ExtractMidiMetadata(MidiFile midi)
    {
        int trackCount = midi.GetTrackChunks().Count();
        _busWrite((ushort)VgcConstants.MetaSongs, (byte)Math.Min(trackCount, 255));

        string? title = null;
        string? copyright = null;
        foreach (var track in midi.GetTrackChunks())
        {
            foreach (var ev in track.Events)
            {
                if (title is null && ev is SequenceTrackNameEvent tn)
                    title = tn.Text;
                if (copyright is null && ev is CopyrightNoticeEvent cr)
                    copyright = cr.Text;
                if (title is not null && copyright is not null) break;
            }
            if (title is not null && copyright is not null) break;
        }

        if (title is not null)
            WriteMetaStringFromManaged(VgcConstants.MetaTitle, title, VgcConstants.MetaTitleLen);
        if (copyright is not null)
        {
            WriteMetaStringFromManaged(VgcConstants.MetaAuthor, copyright, VgcConstants.MetaAuthorLen);
            WriteMetaStringFromManaged(VgcConstants.MetaCopyright, copyright, VgcConstants.MetaCopyrightLen);
        }

        var duration = midi.GetDuration<MetricTimeSpan>();
        int durationSecs = Math.Min((int)duration.TotalSeconds, 0xFFFF);
        _busWrite((ushort)VgcConstants.MetaDurL, (byte)(durationSecs & 0xFF));
        _busWrite((ushort)VgcConstants.MetaDurH, (byte)((durationSecs >> 8) & 0xFF));
    }

    private void ExtractBinMetadata(byte[] data)
    {
        if (data.Length < 2) return;
        _busWrite((ushort)VgcConstants.MetaLoadL, data[0]);
        _busWrite((ushort)VgcConstants.MetaLoadH, data[1]);
    }

    private void WriteMetaString(int destAddr, byte[] source, int srcOffset, int maxLen)
    {
        int len = 0;
        while (len < maxLen && srcOffset + len < source.Length && source[srcOffset + len] != 0)
            len++;
        for (int i = 0; i < len; i++)
            _busWrite((ushort)(destAddr + i), source[srcOffset + i]);
    }

    private void WriteMetaStringFromManaged(int destAddr, string text, int maxLen)
    {
        int len = Math.Min(text.Length, maxLen);
        for (int i = 0; i < len; i++)
            _busWrite((ushort)(destAddr + i), (byte)text[i]);
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
            type = NdiTypeToFioDirType(entry.FileType);
        }
        _regs[VgcConstants.FioDirType - VgcConstants.FioBase] = (byte)type;

        string displayName = entry.Filename;
        int nameLen = Math.Min(displayName.Length, 63);
        _regs[VgcConstants.FioNameLen - VgcConstants.FioBase] = (byte)nameLen;
        int nameOffset = VgcConstants.FioName - VgcConstants.FioBase;
        for (int i = 0; i < nameLen; i++)
            _regs[nameOffset + i] = (byte)displayName[i];

        long dataSize = HasTwoByteLoadPrefix(type) ? Math.Max(0, entry.SizeBytes - 2) : entry.SizeBytes;
        _regs[VgcConstants.FioSizeL - VgcConstants.FioBase] = (byte)(dataSize & 0xFF);
        _regs[VgcConstants.FioSizeH - VgcConstants.FioBase] = (byte)((dataSize >> 8) & 0xFF);
        _regs[VgcConstants.FioSize2 - VgcConstants.FioBase] = (byte)((dataSize >> 16) & 0xFF);
    }

    private void PopulateDirEntry(FileInfo fi)
    {
        string baseName = Path.GetFileNameWithoutExtension(fi.Name);
        string ext = fi.Extension.ToLowerInvariant();

        int type = NdiTypeToFioDirType(ExtToNdiType(ext));
        _regs[VgcConstants.FioDirType - VgcConstants.FioBase] = (byte)type;

        // Build display name with addresses appended
        string displayName = baseName;
        try
        {
            if (type == VgcConstants.FioDirTypeBin && fi.Length >= 2) // BIN: show load address
            {
                byte[] hdr = new byte[2];
                using var fs = fi.OpenRead();
                if (fs.Read(hdr, 0, 2) == 2)
                    displayName = $"{baseName} ${hdr[0] | (hdr[1] << 8):X4}";
            }
            else if (type == VgcConstants.FioDirTypeSid && fi.Length >= 0x16) // SID: show load/init/play
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

        long dataSize = HasTwoByteLoadPrefix(type) ? Math.Max(0, fi.Length - 2) : fi.Length;
        _regs[VgcConstants.FioSizeL - VgcConstants.FioBase] = (byte)(dataSize & 0xFF);
        _regs[VgcConstants.FioSizeH - VgcConstants.FioBase] = (byte)((dataSize >> 8) & 0xFF);
        _regs[VgcConstants.FioSize2 - VgcConstants.FioBase] = (byte)((dataSize >> 16) & 0xFF);
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

    private int GetFioXramAddress()
    {
        int lo = _regs[VgcConstants.FioGAddrL - VgcConstants.FioBase];
        int mid = _regs[VgcConstants.FioGAddrH - VgcConstants.FioBase];
        int hi = _regs[VgcConstants.FioGSpace - VgcConstants.FioBase];
        return lo | (mid << 8) | (hi << 16);
    }

    private int GetFioGraphicsAddress() =>
        _regs[VgcConstants.FioGAddrL - VgcConstants.FioBase]
        | (_regs[VgcConstants.FioGAddrH - VgcConstants.FioBase] << 8);

    private int GetFioTransferLength() =>
        _regs[VgcConstants.FioGLenL - VgcConstants.FioBase]
        | (_regs[VgcConstants.FioGLenH - VgcConstants.FioBase] << 8);

    private int GetFioSize24() =>
        _regs[VgcConstants.FioSizeL - VgcConstants.FioBase]
        | (_regs[VgcConstants.FioSizeH - VgcConstants.FioBase] << 8)
        | (_regs[VgcConstants.FioSize2 - VgcConstants.FioBase] << 16);

    private int GetFioFileOffset() =>
        _regs[VgcConstants.FioSrcL - VgcConstants.FioBase]
        | (_regs[VgcConstants.FioSrcH - VgcConstants.FioBase] << 8)
        | (_regs[VgcConstants.FioEndL - VgcConstants.FioBase] << 16);

    private static bool CpuRangeOk(int address, int length)
    {
        if (address < 0 || length < 0)
            return false;
        long end = (long)address + length;
        return end <= 0x10000 && (length == 0 || address < 0x10000);
    }

    private bool XramRangeOk(int address, int length)
    {
        int capacity = _xramCapacity?.Invoke() ?? 0;
        if (capacity <= 0 || address < 0 || length < 0)
            return false;

        long end = (long)address + length;
        return end <= capacity && (length == 0 || address < capacity);
    }

    private void SetTransferSize(int length)
    {
        _regs[VgcConstants.FioSizeL - VgcConstants.FioBase] = (byte)(length & 0xFF);
        _regs[VgcConstants.FioSizeH - VgcConstants.FioBase] = (byte)((length >> 8) & 0xFF);
    }

    private void SetTransferSize24(int length)
    {
        _regs[VgcConstants.FioSizeL - VgcConstants.FioBase] = (byte)(length & 0xFF);
        _regs[VgcConstants.FioSizeH - VgcConstants.FioBase] = (byte)((length >> 8) & 0xFF);
        _regs[VgcConstants.FioSize2 - VgcConstants.FioBase] = (byte)((length >> 16) & 0xFF);
    }

    private static bool DecodeForthFileAccess(byte fam, out bool canRead, out bool canWrite)
    {
        switch (fam & 0x03)
        {
            case 1:
                canRead = true;
                canWrite = false;
                return true;
            case 2:
                canRead = false;
                canWrite = true;
                return true;
            case 3:
                canRead = true;
                canWrite = true;
                return true;
            default:
                canRead = false;
                canWrite = false;
                return false;
        }
    }

    private int AllocateFileHandle(OpenFileHandle handle)
    {
        for (int i = 0; i < _openFiles.Length; i++)
        {
            if (_openFiles[i] is null)
            {
                _openFiles[i] = handle;
                return i + 1;
            }
        }

        return 0;
    }

    private OpenFileHandle? GetFileHandle(int fileId)
    {
        if (fileId < 1 || fileId > _openFiles.Length)
            return null;
        return _openFiles[fileId - 1];
    }

    private string? ReadCpuFilename(int address, int length)
    {
        if (length < 1 || length > 63 || !CpuRangeOk(address, length))
            return null;

        var sb = new StringBuilder(length);
        for (int i = 0; i < length; i++)
            sb.Append((char)_busRead((ushort)(address + i)));

        string raw = sb.ToString().Trim();
        if (raw.Length == 0 || !SafeFilename().IsMatch(raw))
            return null;
        return raw;
    }

    private byte[]? LoadDataFile(string filename, string defaultExtension, out bool notFound)
    {
        notFound = false;
        var resolved = ResolveDevice(filename);
        if (resolved is not null)
        {
            var (device, name, savedDir) = resolved.Value;
            try
            {
                var (baseName, extension) = SplitDataFilename(name, defaultExtension);
                if (!device.FileExists(baseName, extension))
                {
                    notFound = true;
                    return null;
                }
                return device.Load(baseName, extension);
            }
            finally
            {
                RestoreDir(device, savedDir);
            }
        }

        string path = GetFullPath(filename, defaultExtension);
        if (!File.Exists(path))
        {
            notFound = true;
            return null;
        }
        return File.ReadAllBytes(path);
    }

    private void SaveDataFile(string filename, string defaultExtension, byte[] data)
    {
        var resolved = ResolveDevice(filename);
        if (resolved is not null)
        {
            var (device, name, savedDir) = resolved.Value;
            try
            {
                var (baseName, extension) = SplitDataFilename(name, defaultExtension);
                device.Save(baseName, data, extension);
            }
            finally
            {
                RestoreDir(device, savedDir);
            }
            return;
        }

        Directory.CreateDirectory(_saveDir);
        string path = GetFullPath(filename, defaultExtension);
        File.WriteAllBytes(path, data);
    }

    private byte[]? LoadExactDataFile(string filename, out bool notFound)
    {
        notFound = false;
        var resolved = ResolveDevice(filename);
        if (resolved is not null)
        {
            var (device, name, savedDir) = resolved.Value;
            try
            {
                var (baseName, extension) = SplitDataFilename(name, "");
                if (!device.FileExists(baseName, extension))
                {
                    notFound = true;
                    return null;
                }
                return device.Load(baseName, extension);
            }
            finally
            {
                RestoreDir(device, savedDir);
            }
        }

        string path = GetExactFallbackPath(filename);
        if (!File.Exists(path))
        {
            notFound = true;
            return null;
        }
        return File.ReadAllBytes(path);
    }

    private void SaveExactDataFile(string filename, byte[] data)
    {
        var resolved = ResolveDevice(filename);
        if (resolved is not null)
        {
            var (device, name, savedDir) = resolved.Value;
            try
            {
                var (baseName, extension) = SplitDataFilename(name, "");
                device.Save(baseName, data, extension);
            }
            finally
            {
                RestoreDir(device, savedDir);
            }
            return;
        }

        string path = GetExactFallbackPath(filename);
        Directory.CreateDirectory(Path.GetDirectoryName(path)!);
        File.WriteAllBytes(path, data);
    }

    private bool ExactDataFileExists(string filename)
    {
        var resolved = ResolveDevice(filename);
        if (resolved is not null)
        {
            var (device, name, savedDir) = resolved.Value;
            try
            {
                var (baseName, extension) = SplitDataFilename(name, "");
                return device.FileExists(baseName, extension);
            }
            finally
            {
                RestoreDir(device, savedDir);
            }
        }

        return File.Exists(GetExactFallbackPath(filename));
    }

    private bool DeleteExactDataFile(string filename)
    {
        var resolved = ResolveDevice(filename);
        if (resolved is not null)
        {
            var (device, name, savedDir) = resolved.Value;
            try
            {
                var (baseName, extension) = SplitDataFilename(name, "");
                if (!device.FileExists(baseName, extension))
                    return false;
                device.Delete(baseName, extension);
                return true;
            }
            finally
            {
                RestoreDir(device, savedDir);
            }
        }

        string path = GetExactFallbackPath(filename);
        if (!File.Exists(path))
            return false;
        File.Delete(path);
        return true;
    }

    private bool RenameExactDataFile(string oldName, string newName)
    {
        byte[]? data = LoadExactDataFile(oldName, out bool notFound);
        if (data is null)
        {
            if (notFound)
                return false;
            throw new IOException("Could not read file for rename.");
        }

        SaveExactDataFile(newName, data);
        DeleteExactDataFile(oldName);
        return true;
    }

    private string GetExactFallbackPath(string filename)
    {
        string path = Path.GetFullPath(Path.Combine(_saveDir, filename));
        string root = Path.GetFullPath(_saveDir);
        if (!path.StartsWith(root + Path.DirectorySeparatorChar, StringComparison.OrdinalIgnoreCase)
            && !path.Equals(root, StringComparison.OrdinalIgnoreCase))
            throw new UnauthorizedAccessException($"Path traversal rejected: '{filename}'");
        return path;
    }

    private static (string BaseName, string Extension) SplitDataFilename(string name, string defaultExtension)
    {
        string extension = Path.GetExtension(name);
        if (string.IsNullOrEmpty(extension))
            return (name, defaultExtension);

        return (name[..^extension.Length], extension);
    }

    private void SetOk()
    {
        _regs[VgcConstants.FioStatus - VgcConstants.FioBase] = VgcConstants.FioStatusOk;
        _regs[VgcConstants.FioErrCode - VgcConstants.FioBase] = VgcConstants.FioErrNone;
        _regs[VgcConstants.FioCmd - VgcConstants.FioBase] = 0;
    }

    private void SetError(byte errCode)
    {
        _regs[VgcConstants.FioStatus - VgcConstants.FioBase] = VgcConstants.FioStatusError;
        _regs[VgcConstants.FioErrCode - VgcConstants.FioBase] = errCode;
        _regs[VgcConstants.FioCmd - VgcConstants.FioBase] = 0;
    }

    [GeneratedRegex(@"^[A-Za-z0-9_.\-/:]+$")]
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
        string namePattern = remainder.Length > 0 ? remainder : "*";
        int dot = remainder.LastIndexOf('.');
        if (dot >= 0)
        {
            string ext = remainder[dot..];
            if (NormalizeFilterExtension(ext) is not null)
            {
                extFilter = ext.ToLowerInvariant();
                namePattern = remainder[..dot];
            }
        }

        return new FilterPattern(devicePrefix, dirPath, namePattern, extFilter);
    }

    private static string? NormalizeFilterExtension(string ext) => ext.ToLowerInvariant() switch
    {
        ".bas" => ".bas",
        ".sid" => ".sid",
        ".bin" or ".xram" => ".bin",
        ".mid" or ".midi" or ".nms" => ".mid",
        ".gfx" or ".nvg" => ".gfx",
        ".4th" or ".fth" or ".fs" or ".fr" => ".4th",
        _ => null
    };
}

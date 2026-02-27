using System.Text;
using System.Text.RegularExpressions;

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
    private readonly string _saveDir;
    private List<FileInfo>? _dirFiles;
    private int _dirIndex;

    public FileIoController(
        Func<ushort, byte> busRead,
        Action<ushort, byte> busWrite,
        string? saveDir = null,
        Func<byte, int, byte>? vgcRead = null,
        Action<byte, int, byte>? vgcWrite = null,
        Func<byte, int>? vgcSpaceLength = null,
        SidPlayer? sidPlayer = null,
        MusicEngine? musicEngine = null)
    {
        _busRead = busRead;
        _busWrite = busWrite;
        _vgcRead = vgcRead;
        _vgcWrite = vgcWrite;
        _vgcSpaceLength = vgcSpaceLength;
        _sidPlayer = sidPlayer;
        _musicEngine = musicEngine;
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
            default:
                SetError(VgcConstants.FioErrIo);
                break;
        }
    }

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

            Directory.CreateDirectory(_saveDir);
            string path = GetFullPath(filename, ".bas");
            File.WriteAllBytes(path, data);

            // Create companion .md if it doesn't exist
            string mdPath = Path.ChangeExtension(path, ".md");
            bool isNewDoc = !File.Exists(mdPath);
            if (isNewDoc)
                File.WriteAllText(mdPath, $"---\ntitle: {filename}\ntype: program\ncategory: Programs\nkeywords: [{filename}]\n---\n\n");

            SetOk();
            ProgramSaved?.Invoke(filename, isNewDoc);
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

            string path = GetFullPath(filename, ".bas");
            if (!File.Exists(path))
            {
                SetError(VgcConstants.FioErrNotFound);
                return;
            }

            byte[] data = File.ReadAllBytes(path);

            if (data.Length < 2)
            {
                SetError(VgcConstants.FioErrIo);
                return;
            }

            // Skip 2-byte load-address prefix
            int dataLength = data.Length - 2;

            int destAddr = _regs[VgcConstants.FioSrcL - VgcConstants.FioBase]
                         | (_regs[VgcConstants.FioSrcH - VgcConstants.FioBase] << 8);

            for (int i = 0; i < dataLength; i++)
                _busWrite((ushort)(destAddr + i), data[2 + i]);

            // Set loaded size (excluding prefix)
            _regs[VgcConstants.FioSizeL - VgcConstants.FioBase] = (byte)(dataLength & 0xFF);
            _regs[VgcConstants.FioSizeH - VgcConstants.FioBase] = (byte)((dataLength >> 8) & 0xFF);

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
            var dir = new DirectoryInfo(_saveDir);
            _dirFiles = dir.Exists
                ? dir.GetFiles("*.bas")
                      .Concat(dir.GetFiles("*.sid"))
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
        if (_dirFiles is null)
        {
            SetEndOfDir();
            return;
        }

        _dirIndex++;
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

            string path = GetFullPath(filename, ".bas");
            if (!File.Exists(path))
            {
                SetError(VgcConstants.FioErrNotFound);
                return;
            }

            File.Delete(path);

            // Also delete companion .md if it exists
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

            Directory.CreateDirectory(_saveDir);
            string path = GetFullPath(filename, ".gfx");
            File.WriteAllBytes(path, data);

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

            string path = GetFullPath(filename, ".gfx");
            if (!File.Exists(path))
            {
                SetError(VgcConstants.FioErrNotFound);
                return;
            }

            byte space = _regs[VgcConstants.FioGSpace - VgcConstants.FioBase];
            int addr = _regs[VgcConstants.FioGAddrL - VgcConstants.FioBase]
                     | (_regs[VgcConstants.FioGAddrH - VgcConstants.FioBase] << 8);
            int reqLen = _regs[VgcConstants.FioGLenL - VgcConstants.FioBase]
                       | (_regs[VgcConstants.FioGLenH - VgcConstants.FioBase] << 8);

            byte[] data = File.ReadAllBytes(path);
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

            string path = GetFullPath(filename, ".sid");
            if (!File.Exists(path))
            {
                SetError(VgcConstants.FioErrNotFound);
                return;
            }

            byte[] data = File.ReadAllBytes(path);
            var info = SidFileParser.Parse(data);
            if (!info.IsValid) { SetError(VgcConstants.FioErrIo); return; }

            // Song number from FioSrcL register (1-based, default to StartSong)
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
        _musicEngine.SetVolume(level);
        SetOk();
    }

    private void DoMusicSeq()
    {
        if (_musicEngine is null) { SetError(VgcConstants.FioErrIo); return; }
        int voice = _regs[VgcConstants.FioSrcL - VgcConstants.FioBase];

        // MML string is passed via pointer in 6502 memory (not the 64-byte FIO_NAME buffer)
        // so strings up to 255 bytes are supported.
        int len = _regs[VgcConstants.FioNameLen - VgcConstants.FioBase];
        int ptr = _regs[VgcConstants.FioEndL - VgcConstants.FioBase]
                | (_regs[VgcConstants.FioEndH - VgcConstants.FioBase] << 8);
        if (len < 1) { SetError(VgcConstants.FioErrIo); return; }

        var sb = new StringBuilder(len);
        for (int i = 0; i < len; i++)
            sb.Append((char)_busRead((ushort)(ptr + i)));

        // voice from BASIC is 1-6, MusicEngine expects 0-5
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
        if (v1 > 0) pri.Add(v1 - 1); // convert 1-based to 0-based
        if (v2 > 0) pri.Add(v2 - 1);
        if (v3 > 0) pri.Add(v3 - 1);
        if (v4 > 0) pri.Add(v4 - 1);
        if (v5 > 0) pri.Add(v5 - 1);
        if (v6 > 0) pri.Add(v6 - 1);
        _musicEngine.SetPriority(pri.ToArray());
        SetOk();
    }

    private void PopulateDirEntry(FileInfo fi)
    {
        // Name without extension
        string name = Path.GetFileNameWithoutExtension(fi.Name);
        int nameLen = Math.Min(name.Length, 63);
        _regs[VgcConstants.FioNameLen - VgcConstants.FioBase] = (byte)nameLen;
        int nameOffset = VgcConstants.FioName - VgcConstants.FioBase;
        for (int i = 0; i < nameLen; i++)
            _regs[nameOffset + i] = (byte)name[i];

        // File type: 0=PRG (.bas), 1=SID (.sid)
        bool isSid = fi.Extension.Equals(".sid", StringComparison.OrdinalIgnoreCase);
        _regs[VgcConstants.FioDirType - VgcConstants.FioBase] = (byte)(isSid ? 1 : 0);

        // File size excluding 2-byte load-address prefix (PRG only)
        long dataSize = isSid ? fi.Length : Math.Max(0, fi.Length - 2);
        _regs[VgcConstants.FioSizeL - VgcConstants.FioBase] = (byte)(dataSize & 0xFF);
        _regs[VgcConstants.FioSizeH - VgcConstants.FioBase] = (byte)((dataSize >> 8) & 0xFF);
    }

    private void SetEndOfDir()
    {
        _regs[VgcConstants.FioStatus - VgcConstants.FioBase] = VgcConstants.FioStatusError;
        _regs[VgcConstants.FioErrCode - VgcConstants.FioBase] = VgcConstants.FioErrEndOfDir;
    }

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
}

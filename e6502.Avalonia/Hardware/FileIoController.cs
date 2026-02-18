using System.Text;
using System.Text.RegularExpressions;

namespace e6502.Avalonia.Hardware;

public sealed partial class FileIoController
{
    private readonly byte[] _regs = new byte[VgcConstants.FioEnd - VgcConstants.FioBase + 1];
    private readonly Func<ushort, byte> _busRead;
    private readonly Action<ushort, byte> _busWrite;
    private readonly string _saveDir;

    public FileIoController(Func<ushort, byte> busRead, Action<ushort, byte> busWrite, string? saveDir = null)
    {
        _busRead = busRead;
        _busWrite = busWrite;
        _saveDir = saveDir ?? Path.Combine(
            Environment.GetFolderPath(Environment.SpecialFolder.UserProfile),
            "e6502-programs");
    }

    public string SaveDirectory => _saveDir;

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
            byte[] data = new byte[length];
            for (int i = 0; i < length; i++)
                data[i] = _busRead((ushort)(srcAddr + i));

            Directory.CreateDirectory(_saveDir);
            string path = GetFullPath(filename);
            File.WriteAllBytes(path, data);

            SetOk();
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

            string path = GetFullPath(filename);
            if (!File.Exists(path))
            {
                SetError(VgcConstants.FioErrNotFound);
                return;
            }

            byte[] data = File.ReadAllBytes(path);

            int destAddr = _regs[VgcConstants.FioSrcL - VgcConstants.FioBase]
                         | (_regs[VgcConstants.FioSrcH - VgcConstants.FioBase] << 8);

            for (int i = 0; i < data.Length; i++)
                _busWrite((ushort)(destAddr + i), data[i]);

            // Set loaded size
            _regs[VgcConstants.FioSizeL - VgcConstants.FioBase] = (byte)(data.Length & 0xFF);
            _regs[VgcConstants.FioSizeH - VgcConstants.FioBase] = (byte)((data.Length >> 8) & 0xFF);

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

    private string GetFullPath(string filename)
    {
        if (!filename.EndsWith(".bas", StringComparison.OrdinalIgnoreCase))
            filename += ".bas";
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

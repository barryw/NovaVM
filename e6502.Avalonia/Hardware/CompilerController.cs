namespace e6502.Avalonia.Hardware;

public sealed class CompilerController
{
    private readonly byte[] _regs;
    private readonly Func<int, byte> _readXram;
    private readonly Action<ushort, byte> _writeCpuRam;
    private readonly List<CompileError> _errors = new();
    private readonly List<CompileError> _warnings = new();
    private List<CompileError> _activeList;
    private int _errMsgPos;

    public CompilerController(Func<int, byte> readXram, Action<ushort, byte> writeCpuRam)
    {
        _regs = new byte[VgcConstants.CmpEnd - VgcConstants.CmpBase + 1];
        _readXram = readXram;
        _writeCpuRam = writeCpuRam;
        _activeList = _errors;
    }

    public bool OwnsAddress(ushort address) =>
        address >= VgcConstants.CmpBase && address <= VgcConstants.CmpEnd;

    public byte Read(ushort address)
    {
        if (address == VgcConstants.CmpErrMsg)
            return ReadNextErrorMsgByte();
        return _regs[address - VgcConstants.CmpBase];
    }

    public void Write(ushort address, byte data)
    {
        _regs[address - VgcConstants.CmpBase] = data;
        if (address == VgcConstants.CmpCmd)
            ExecuteCommand(data);
    }

    private void ExecuteCommand(byte cmd)
    {
        switch (cmd)
        {
            case VgcConstants.CmpCmdCompile:
                DoCompile();
                break;
            case VgcConstants.CmpCmdGetError:
                PopulateErrorFields(_errors);
                break;
            case VgcConstants.CmpCmdGetWarn:
                PopulateErrorFields(_warnings);
                break;
        }
    }

    private void DoCompile()
    {
        SetStatus(VgcConstants.CmpStatusCompiling);
        _errors.Clear();
        _warnings.Clear();

        int srcAddr = _regs[VgcConstants.CmpSrcAddrL - VgcConstants.CmpBase]
                    | (_regs[VgcConstants.CmpSrcAddrM - VgcConstants.CmpBase] << 8);

        var source = ReadSourceFromXram(srcAddr);

        if (string.IsNullOrWhiteSpace(source))
        {
            _errors.Add(new CompileError(1, "empty source"));
            SetResult(VgcConstants.CmpStatusError, _errors.Count, _warnings.Count, 0);
            return;
        }

        // TODO: invoke actual compiler pipeline here (Phase 3-6)
        _errors.Add(new CompileError(0, "compiler not yet implemented"));
        _warnings.Add(new CompileError(0, "compilation support is experimental"));
        SetResult(VgcConstants.CmpStatusError, _errors.Count, _warnings.Count, 0);
    }

    private string ReadSourceFromXram(int addr)
    {
        var sb = new System.Text.StringBuilder();
        for (int i = 0; i < 64 * 1024; i++) // max 64K source
        {
            byte b = _readXram(addr + i);
            if (b == 0) break;
            sb.Append((char)b);
        }
        return sb.ToString();
    }

    private void PopulateErrorFields(List<CompileError> list)
    {
        int idx = _regs[VgcConstants.CmpErrIdx - VgcConstants.CmpBase];
        if (idx < list.Count)
        {
            var err = list[idx];
            _regs[VgcConstants.CmpErrLineL - VgcConstants.CmpBase] = (byte)(err.Line & 0xFF);
            _regs[VgcConstants.CmpErrLineH - VgcConstants.CmpBase] = (byte)((err.Line >> 8) & 0xFF);
            _errMsgPos = 0;
            _activeList = list;
        }
    }

    private byte ReadNextErrorMsgByte()
    {
        int idx = _regs[VgcConstants.CmpErrIdx - VgcConstants.CmpBase];
        if (idx >= _activeList.Count) return 0;
        var msg = _activeList[idx].Message;
        if (_errMsgPos >= msg.Length) return 0;
        return (byte)msg[_errMsgPos++];
    }

    private void SetStatus(byte status) =>
        _regs[VgcConstants.CmpStatus - VgcConstants.CmpBase] = status;

    private void SetResult(byte status, int errors, int warnings, int codeSize)
    {
        _regs[VgcConstants.CmpStatus - VgcConstants.CmpBase] = status;
        _regs[VgcConstants.CmpErrCount - VgcConstants.CmpBase] = (byte)errors;
        _regs[VgcConstants.CmpWarnCount - VgcConstants.CmpBase] = (byte)warnings;
        _regs[VgcConstants.CmpCodeSizeL - VgcConstants.CmpBase] = (byte)(codeSize & 0xFF);
        _regs[VgcConstants.CmpCodeSizeH - VgcConstants.CmpBase] = (byte)((codeSize >> 8) & 0xFF);
    }

    internal record CompileError(int Line, string Message);
}

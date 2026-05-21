namespace e6502.Avalonia.Hardware;

/// <summary>
/// Browser placeholder for the memory-mapped NIC. The desktop implementation uses
/// TCP sockets, which are not available in browser WASM.
/// </summary>
public sealed class VirtualNetworkController : IDisposable
{
    private readonly byte[] _regs = new byte[VgcConstants.NicEnd - VgcConstants.NicBase + 1];

    public VirtualNetworkController(Func<ushort, byte> busRead, Action<ushort, byte> busWrite)
    {
        ResetAll();
    }

    public bool IrqPending => false;

    public bool OwnsAddress(ushort address) =>
        address >= VgcConstants.NicBase && address <= VgcConstants.NicEnd;

    public byte Read(ushort address)
    {
        if (!OwnsAddress(address))
            return 0;

        if (address == VgcConstants.NicStatus)
            return VgcConstants.NicStatusReady;

        if (address >= VgcConstants.NicSlotStatus0 && address <= VgcConstants.NicSlotStatus3)
            return VgcConstants.NicSlotSendReady;

        return _regs[address - VgcConstants.NicBase];
    }

    public void Write(ushort address, byte data)
    {
        if (!OwnsAddress(address))
            return;

        _regs[address - VgcConstants.NicBase] = data;
        if (address == VgcConstants.NicCmd)
        {
            _regs[VgcConstants.NicDmaStatus - VgcConstants.NicBase] = VgcConstants.NicDmaStatusTxReady;
            _regs[VgcConstants.NicDmaErr - VgcConstants.NicBase] = VgcConstants.NicDmaErrNone;
        }
    }

    public void ResetAll()
    {
        Array.Clear(_regs);
        _regs[VgcConstants.NicDmaStatus - VgcConstants.NicBase] = VgcConstants.NicDmaStatusTxReady;
    }

    public void Dispose()
    {
    }
}

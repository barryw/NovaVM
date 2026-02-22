using e6502.Avalonia.Hardware;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class NicControllerTests
{
    private byte[] _ram = null!;
    private VirtualNetworkController _nic = null!;

    [TestInitialize]
    public void Setup()
    {
        _ram = new byte[65536];
        _nic = new VirtualNetworkController(
            addr => _ram[addr],
            (addr, data) => _ram[addr] = data);
    }

    [TestCleanup]
    public void Cleanup()
    {
        _nic.Dispose();
    }

    [TestMethod]
    public void Nic_OwnsAddress_InRange()
    {
        Assert.IsTrue(_nic.OwnsAddress((ushort)VgcConstants.NicBase));
        Assert.IsTrue(_nic.OwnsAddress((ushort)VgcConstants.NicEnd));
        Assert.IsFalse(_nic.OwnsAddress((ushort)(VgcConstants.NicBase - 1)));
        Assert.IsFalse(_nic.OwnsAddress((ushort)(VgcConstants.NicEnd + 1)));
    }

    [TestMethod]
    public void Nic_WriteRegister_ReadBack()
    {
        _nic.Write((ushort)VgcConstants.NicSlot, 2);
        Assert.AreEqual(2, _nic.Read((ushort)VgcConstants.NicSlot));
    }

    [TestMethod]
    public void Nic_Status_ReportsReady()
    {
        byte status = _nic.Read((ushort)VgcConstants.NicStatus);
        Assert.IsTrue((status & VgcConstants.NicStatusReady) != 0);
    }

    [TestMethod]
    public void Nic_IrqStatus_ReadClears()
    {
        // Enable IRQ for slot 0
        _nic.Write((ushort)VgcConstants.NicIrqCtrl, 0x01);
        // Inject a test message into slot 0
        _nic.InjectTestMessage(0, new byte[] { 0x41, 0x42 });
        // First read should be non-zero
        byte first = _nic.Read((ushort)VgcConstants.NicIrqStatus);
        Assert.AreNotEqual(0, first);
        // Second read should be zero (read-clears)
        byte second = _nic.Read((ushort)VgcConstants.NicIrqStatus);
        Assert.AreEqual(0, second);
    }

    [TestMethod]
    public void Nic_SlotStatus_NoConnection_ReportsZero()
    {
        byte status = _nic.Read((ushort)VgcConstants.NicSlotStatus0);
        // SendReady should be set even when not connected
        Assert.IsTrue((status & VgcConstants.NicSlotSendReady) != 0);
        // Connected should not be set
        Assert.IsFalse((status & VgcConstants.NicSlotConnected) != 0);
    }
}

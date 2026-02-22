using System.Net.Sockets;
using System.Threading;
using System.Threading.Tasks;
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

    [TestMethod]
    public void Nic_Recv_DmaWritesToRam()
    {
        _nic.InjectTestMessage(0, new byte[] { 0x48, 0x49, 0x21 });
        _nic.Write((ushort)VgcConstants.NicSlot, 0);
        _nic.Write((ushort)VgcConstants.NicDmaAddrL, 0x80);
        _nic.Write((ushort)VgcConstants.NicDmaAddrH, 0x20);
        _nic.Write((ushort)VgcConstants.NicCmd, VgcConstants.NicCmdRecv);

        Assert.AreEqual(0x48, _ram[0x2080]);
        Assert.AreEqual(0x49, _ram[0x2081]);
        Assert.AreEqual(0x21, _ram[0x2082]);
        Assert.AreEqual(3, _nic.Read((ushort)VgcConstants.NicMsgLen));
    }

    [TestMethod]
    public void Nic_Recv_EmptyQueue_SetsLenZero()
    {
        _nic.Write((ushort)VgcConstants.NicSlot, 0);
        _nic.Write((ushort)VgcConstants.NicDmaAddrL, 0x00);
        _nic.Write((ushort)VgcConstants.NicDmaAddrH, 0x20);
        _nic.Write((ushort)VgcConstants.NicCmd, VgcConstants.NicCmdRecv);
        Assert.AreEqual(0, _nic.Read((ushort)VgcConstants.NicMsgLen));
    }

    [TestMethod]
    public void Nic_Send_UnconnectedSlot_SetsError()
    {
        _ram[0x2000] = 0x48;
        _ram[0x2001] = 0x49;
        _nic.Write((ushort)VgcConstants.NicSlot, 0);
        _nic.Write((ushort)VgcConstants.NicDmaAddrL, 0x00);
        _nic.Write((ushort)VgcConstants.NicDmaAddrH, 0x20);
        _nic.Write((ushort)VgcConstants.NicDmaLen, 2);
        _nic.Write((ushort)VgcConstants.NicCmd, VgcConstants.NicCmdSend);
        byte slotStatus = _nic.Read((ushort)VgcConstants.NicSlotStatus0);
        Assert.AreNotEqual(0, (byte)(slotStatus & VgcConstants.NicSlotError));
    }

    [TestMethod]
    public void Nic_Recv_ClearsDataReady_WhenQueueEmpty()
    {
        _nic.InjectTestMessage(0, new byte[] { 0x41 });
        Assert.AreNotEqual(0, (byte)(_nic.Read((ushort)VgcConstants.NicSlotStatus0) & VgcConstants.NicSlotDataReady));

        _nic.Write((ushort)VgcConstants.NicSlot, 0);
        _nic.Write((ushort)VgcConstants.NicDmaAddrL, 0x00);
        _nic.Write((ushort)VgcConstants.NicDmaAddrH, 0x20);
        _nic.Write((ushort)VgcConstants.NicCmd, VgcConstants.NicCmdRecv);

        Assert.AreEqual(0, (byte)(_nic.Read((ushort)VgcConstants.NicSlotStatus0) & VgcConstants.NicSlotDataReady));
    }

    [TestMethod]
    public async Task Nic_Connect_ToLoopbackServer()
    {
        using var server = new TcpListener(System.Net.IPAddress.Loopback, 0);
        server.Start();
        int port = ((System.Net.IPEndPoint)server.LocalEndpoint).Port;

        string host = "127.0.0.1";
        for (int i = 0; i < host.Length; i++)
            _nic.Write((ushort)(VgcConstants.NicNameBuf + i), (byte)host[i]);
        _nic.Write((ushort)(VgcConstants.NicNameBuf + host.Length), 0);

        _nic.Write((ushort)VgcConstants.NicSlot, 0);
        _nic.Write((ushort)VgcConstants.NicRemotePortL, (byte)(port & 0xFF));
        _nic.Write((ushort)VgcConstants.NicRemotePortH, (byte)(port >> 8));
        _nic.Write((ushort)VgcConstants.NicCmd, VgcConstants.NicCmdConnect);

        var acceptTask = server.AcceptTcpClientAsync();

        var sw = System.Diagnostics.Stopwatch.StartNew();
        while (sw.ElapsedMilliseconds < 2000)
        {
            byte status = _nic.Read((ushort)VgcConstants.NicSlotStatus0);
            if ((status & VgcConstants.NicSlotConnected) != 0) break;
            await Task.Delay(10);
        }

        Assert.AreNotEqual(0, (byte)(_nic.Read((ushort)VgcConstants.NicSlotStatus0) & VgcConstants.NicSlotConnected));

        using var serverClient = await acceptTask;
        _nic.Write((ushort)VgcConstants.NicSlot, 0);
        _nic.Write((ushort)VgcConstants.NicCmd, VgcConstants.NicCmdDisconnect);

        await Task.Delay(50);
        Assert.AreEqual(0, (byte)(_nic.Read((ushort)VgcConstants.NicSlotStatus0) & VgcConstants.NicSlotConnected));

        server.Stop();
    }

    [TestMethod]
    public void Nic_Connect_BadHost_SetsError()
    {
        string host = "invalid.host.that.does.not.exist.example";
        for (int i = 0; i < host.Length; i++)
            _nic.Write((ushort)(VgcConstants.NicNameBuf + i), (byte)host[i]);
        _nic.Write((ushort)(VgcConstants.NicNameBuf + host.Length), 0);

        _nic.Write((ushort)VgcConstants.NicSlot, 0);
        _nic.Write((ushort)VgcConstants.NicRemotePortL, 0x00);
        _nic.Write((ushort)VgcConstants.NicRemotePortH, 0x10);
        _nic.Write((ushort)VgcConstants.NicCmd, VgcConstants.NicCmdConnect);

        Thread.Sleep(3000);
        byte status = _nic.Read((ushort)VgcConstants.NicSlotStatus0);
        Assert.AreNotEqual(0, (byte)(status & VgcConstants.NicSlotError));
    }
}

using System;
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
    public async Task Nic_SendRecv_RoundTrip()
    {
        using var server = new TcpListener(System.Net.IPAddress.Loopback, 0);
        server.Start();
        int port = ((System.Net.IPEndPoint)server.LocalEndpoint).Port;

        // Connect
        string host = "127.0.0.1";
        for (int i = 0; i < host.Length; i++)
            _nic.Write((ushort)(VgcConstants.NicNameBuf + i), (byte)host[i]);
        _nic.Write((ushort)(VgcConstants.NicNameBuf + host.Length), 0);
        _nic.Write((ushort)VgcConstants.NicSlot, 0);
        _nic.Write((ushort)VgcConstants.NicRemotePortL, (byte)(port & 0xFF));
        _nic.Write((ushort)VgcConstants.NicRemotePortH, (byte)(port >> 8));
        _nic.Write((ushort)VgcConstants.NicCmd, VgcConstants.NicCmdConnect);

        using var serverClient = await server.AcceptTcpClientAsync();
        var serverStream = serverClient.GetStream();

        // Wait for connected
        var sw = System.Diagnostics.Stopwatch.StartNew();
        while (sw.ElapsedMilliseconds < 2000 &&
               (_nic.Read((ushort)VgcConstants.NicSlotStatus0) & VgcConstants.NicSlotConnected) == 0)
            await Task.Delay(10);

        // Send "HI!" from 6502
        _ram[0x2000] = 0x48;
        _ram[0x2001] = 0x49;
        _ram[0x2002] = 0x21;
        _nic.Write((ushort)VgcConstants.NicDmaAddrL, 0x00);
        _nic.Write((ushort)VgcConstants.NicDmaAddrH, 0x20);
        _nic.Write((ushort)VgcConstants.NicDmaLen, 3);
        _nic.Write((ushort)VgcConstants.NicCmd, VgcConstants.NicCmdSend);

        // Server reads: 1-byte length + 3-byte payload
        var recvBuf = new byte[4];
        int totalRead = 0;
        while (totalRead < 4)
            totalRead += await serverStream.ReadAsync(recvBuf.AsMemory(totalRead));
        Assert.AreEqual(3, recvBuf[0]);
        Assert.AreEqual(0x48, recvBuf[1]);
        Assert.AreEqual(0x49, recvBuf[2]);
        Assert.AreEqual(0x21, recvBuf[3]);

        // Server sends reply: "OK"
        serverStream.WriteByte(2);
        serverStream.Write(new byte[] { 0x4F, 0x4B });
        serverStream.Flush();

        // Wait for data available
        sw.Restart();
        while (sw.ElapsedMilliseconds < 2000 &&
               (_nic.Read((ushort)VgcConstants.NicSlotStatus0) & VgcConstants.NicSlotDataReady) == 0)
            await Task.Delay(10);

        // Receive into $2080
        _nic.Write((ushort)VgcConstants.NicDmaAddrL, 0x80);
        _nic.Write((ushort)VgcConstants.NicDmaAddrH, 0x20);
        _nic.Write((ushort)VgcConstants.NicCmd, VgcConstants.NicCmdRecv);

        Assert.AreEqual(2, _nic.Read((ushort)VgcConstants.NicMsgLen));
        Assert.AreEqual(0x4F, _ram[0x2080]);
        Assert.AreEqual(0x4B, _ram[0x2081]);

        _nic.Write((ushort)VgcConstants.NicCmd, VgcConstants.NicCmdDisconnect);
        server.Stop();
    }

    [TestMethod]
    public async Task Nic_Listen_Accept_ReceivesConnection()
    {
        _nic.Write((ushort)VgcConstants.NicSlot, 0);
        _nic.Write((ushort)VgcConstants.NicLocalPortL, 0x00);
        _nic.Write((ushort)VgcConstants.NicLocalPortH, 0x00); // port 0 = OS assigns
        _nic.Write((ushort)VgcConstants.NicCmd, VgcConstants.NicCmdListen);

        int assignedPort = _nic.GetListeningPort(0) ?? 0;
        Assert.AreNotEqual(0, assignedPort);

        using var client = new TcpClient();
        await client.ConnectAsync("127.0.0.1", assignedPort);

        var sw = System.Diagnostics.Stopwatch.StartNew();
        while (sw.ElapsedMilliseconds < 2000 &&
               (_nic.Read((ushort)VgcConstants.NicSlotStatus0) & VgcConstants.NicSlotDataReady) == 0)
            await Task.Delay(10);

        _nic.Write((ushort)VgcConstants.NicSlot, 0);
        _nic.Write((ushort)VgcConstants.NicCmd, VgcConstants.NicCmdAccept);

        Assert.AreNotEqual(0, (byte)(_nic.Read((ushort)VgcConstants.NicSlotStatus0) & VgcConstants.NicSlotConnected));

        _nic.Write((ushort)VgcConstants.NicCmd, VgcConstants.NicCmdDisconnect);
    }

    [TestMethod]
    public async Task Nic_Irq_FiredOnMessageArrival()
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

        _nic.Write((ushort)VgcConstants.NicIrqCtrl, 0x01); // enable IRQ for slot 0

        _nic.Write((ushort)VgcConstants.NicCmd, VgcConstants.NicCmdConnect);
        using var serverClient = await server.AcceptTcpClientAsync();

        var sw = System.Diagnostics.Stopwatch.StartNew();
        while (sw.ElapsedMilliseconds < 2000 &&
               (_nic.Read((ushort)VgcConstants.NicSlotStatus0) & VgcConstants.NicSlotConnected) == 0)
            await Task.Delay(10);

        Assert.IsFalse(_nic.IrqPending);

        // Server sends a message
        var stream = serverClient.GetStream();
        stream.WriteByte(2);
        stream.Write(new byte[] { 0x41, 0x42 });
        stream.Flush();

        // Wait for IRQ
        sw.Restart();
        while (sw.ElapsedMilliseconds < 2000 && !_nic.IrqPending)
            await Task.Delay(10);

        Assert.IsTrue(_nic.IrqPending);

        byte irqStatus = _nic.Read((ushort)VgcConstants.NicIrqStatus);
        Assert.AreEqual(0x01, irqStatus);
        Assert.IsFalse(_nic.IrqPending);

        _nic.Write((ushort)VgcConstants.NicSlot, 0);
        _nic.Write((ushort)VgcConstants.NicCmd, VgcConstants.NicCmdDisconnect);
        server.Stop();
    }

    [TestMethod]
    public void Nic_SendDirect_UnconnectedSlot_SetsError()
    {
        _nic.SendDirect(0, new byte[] { 0x48, 0x49 });
        byte status = _nic.Read((ushort)VgcConstants.NicSlotStatus0);
        Assert.AreNotEqual(0, (byte)(status & VgcConstants.NicSlotError));
    }

    [TestMethod]
    public void Nic_RecvDirect_EmptyQueue_ReturnsNull()
    {
        Assert.IsNull(_nic.RecvDirect(0));
    }

    [TestMethod]
    public void Nic_RecvDirect_WithMessage_ReturnsData()
    {
        _nic.InjectTestMessage(0, new byte[] { 0x41, 0x42, 0x43 });
        byte[]? data = _nic.RecvDirect(0);
        Assert.IsNotNull(data);
        Assert.AreEqual(3, data!.Length);
        Assert.AreEqual(0x41, data[0]);
        Assert.AreEqual(0x42, data[1]);
        Assert.AreEqual(0x43, data[2]);
    }

    [TestMethod]
    public void Nic_SendDirect_ClampsSlotTo03()
    {
        // Slot 5 should map to slot 1 (5 & 0x03 = 1), not crash
        _nic.SendDirect(5, new byte[] { 0x48 });
        byte status = _nic.Read((ushort)VgcConstants.NicSlotStatus1);
        Assert.AreNotEqual(0, (byte)(status & VgcConstants.NicSlotError));
    }

    [TestMethod]
    public void Nic_ResetAll_ClearsIrqAndState()
    {
        _nic.Write((ushort)VgcConstants.NicIrqCtrl, 0x01);
        _nic.InjectTestMessage(0, new byte[] { 0x41 });
        Assert.IsTrue(_nic.IrqPending);

        _nic.ResetAll();

        Assert.IsFalse(_nic.IrqPending);
        Assert.AreEqual(0, _nic.Read((ushort)VgcConstants.NicIrqStatus));
        Assert.IsNull(_nic.RecvDirect(0));
    }

    [TestMethod]
    public async Task Nic_RemoteClose_SetsRemoteClosedFlag()
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

        using var serverClient = await server.AcceptTcpClientAsync();

        var sw = System.Diagnostics.Stopwatch.StartNew();
        while (sw.ElapsedMilliseconds < 2000 &&
               (_nic.Read((ushort)VgcConstants.NicSlotStatus0) & VgcConstants.NicSlotConnected) == 0)
            await Task.Delay(10);

        // Server closes connection
        serverClient.Close();

        // Wait for remote-closed flag
        sw.Restart();
        while (sw.ElapsedMilliseconds < 2000 &&
               (_nic.Read((ushort)VgcConstants.NicSlotStatus0) & VgcConstants.NicSlotRemoteClosed) == 0)
            await Task.Delay(10);

        byte status = _nic.Read((ushort)VgcConstants.NicSlotStatus0);
        Assert.AreNotEqual(0, (byte)(status & VgcConstants.NicSlotRemoteClosed));
        Assert.AreEqual(0, (byte)(status & VgcConstants.NicSlotConnected));

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

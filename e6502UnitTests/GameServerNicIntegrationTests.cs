using System;
using System.Diagnostics;
using System.Net;
using System.Threading;
using System.Threading.Tasks;
using e6502.Avalonia.Hardware;
using e6502.GameServer.Protocol;
using e6502.GameServer.Server;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class GameServerNicIntegrationTests
{
    private const ushort SendBuffer = 0x2100;
    private const ushort RecvBuffer = 0x2200;

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
    public async Task AvaloniaNic_CanHelloAndListGamesFromGameServer()
    {
        var state = new GameServerState();
        var server = new TcpGameServer(state, IPAddress.Loopback, 0);
        using var cts = new CancellationTokenSource();
        Task serverTask = server.RunAsync(cts.Token);

        try
        {
            int port = await WaitForServerPortAsync(server);
            await ConnectNicAsync(port);

            SendProtocolMessage(Hello(sequence: 1, handle: "nova01"));
            ProtocolMessage welcome = await ReceiveProtocolMessageAsync();

            Assert.AreEqual(MessageKind.Welcome, welcome.Kind);
            Assert.AreEqual(1, welcome.Sequence);
            Assert.IsTrue((welcome.Flags & MessageFlags.Response) != 0);

            var welcomeReader = new ProtocolReader(welcome.Payload);
            Assert.IsTrue(welcomeReader.TryReadUInt16(out ushort sessionId));
            Assert.IsTrue(welcomeReader.TryReadString(out string acceptedHandle));
            Assert.IsTrue(welcomeReader.TryReadString(out string serverName));
            Assert.AreNotEqual((ushort)0, sessionId);
            Assert.AreEqual("nova01", acceptedHandle);
            Assert.AreEqual("Nova Game Server", serverName);

            SendProtocolMessage(ProtocolMessage.Create(MessageKind.GameListRequest, sequence: 2));
            ProtocolMessage game = await ReceiveProtocolMessageAsync();

            Assert.AreEqual(MessageKind.GameListItem, game.Kind);
            Assert.AreEqual(2, game.Sequence);
            Assert.IsTrue((game.Flags & MessageFlags.Response) != 0);

            var gameReader = new ProtocolReader(game.Payload);
            Assert.IsTrue(gameReader.TryReadUInt16(out ushort gameId));
            Assert.IsTrue(gameReader.TryReadByte(out byte gameMajor));
            Assert.IsTrue(gameReader.TryReadByte(out byte gameMinor));
            Assert.IsTrue(gameReader.TryReadUInt16(out _));
            Assert.IsTrue(gameReader.TryReadByte(out byte minPlayers));
            Assert.IsTrue(gameReader.TryReadByte(out byte maxPlayers));
            Assert.IsTrue(gameReader.TryReadString(out string gameCode));
            Assert.IsTrue(gameReader.TryReadString(out string displayName));

            Assert.AreEqual((ushort)1, gameId);
            Assert.AreEqual((byte)1, gameMajor);
            Assert.AreEqual((byte)0, gameMinor);
            Assert.AreEqual((byte)1, minPlayers);
            Assert.AreEqual((byte)8, maxPlayers);
            Assert.AreEqual("echo", gameCode);
            Assert.AreEqual("Echo Turn", displayName);
        }
        finally
        {
            _nic.Write((ushort)VgcConstants.NicSlot, 0);
            _nic.Write((ushort)VgcConstants.NicCmd, VgcConstants.NicCmdDisconnect);
            cts.Cancel();
            await serverTask;
        }
    }

    private static async Task<int> WaitForServerPortAsync(TcpGameServer server)
    {
        var sw = Stopwatch.StartNew();
        while (sw.ElapsedMilliseconds < 2000)
        {
            if (server.BoundPort != 0)
                return server.BoundPort;

            await Task.Delay(10);
        }

        Assert.Fail("Game server did not start listening.");
        return 0;
    }

    private async Task ConnectNicAsync(int port)
    {
        const string host = "127.0.0.1";
        for (int i = 0; i < host.Length; i++)
            _nic.Write((ushort)(VgcConstants.NicNameBuf + i), (byte)host[i]);
        _nic.Write((ushort)(VgcConstants.NicNameBuf + host.Length), 0);

        _nic.Write((ushort)VgcConstants.NicSlot, 0);
        _nic.Write((ushort)VgcConstants.NicRemotePortL, (byte)(port & 0xFF));
        _nic.Write((ushort)VgcConstants.NicRemotePortH, (byte)(port >> 8));
        _nic.Write((ushort)VgcConstants.NicCmd, VgcConstants.NicCmdConnect);

        await WaitForSlotStatusAsync(VgcConstants.NicSlotConnected);
    }

    private async Task WaitForSlotStatusAsync(byte mask)
    {
        var sw = Stopwatch.StartNew();
        while (sw.ElapsedMilliseconds < 2000)
        {
            byte status = _nic.Read((ushort)VgcConstants.NicSlotStatus0);
            if ((status & mask) != 0)
                return;

            await Task.Delay(10);
        }

        Assert.Fail($"Timed out waiting for NIC status mask ${mask:X2}.");
    }

    private void SendProtocolMessage(ProtocolMessage message)
    {
        byte[] frame = ProtocolCodec.Encode(message);
        Assert.IsTrue(frame.Length <= 256);

        Array.Copy(frame, 0, _ram, SendBuffer, frame.Length);
        _nic.Write((ushort)VgcConstants.NicSlot, 0);
        _nic.Write((ushort)VgcConstants.NicDmaAddrL, (byte)(SendBuffer & 0xFF));
        _nic.Write((ushort)VgcConstants.NicDmaAddrH, (byte)(SendBuffer >> 8));
        _nic.Write((ushort)VgcConstants.NicDmaLen, (byte)(frame.Length == 256 ? 0 : frame.Length));
        _nic.Write((ushort)VgcConstants.NicCmd, VgcConstants.NicCmdSend);
    }

    private async Task<ProtocolMessage> ReceiveProtocolMessageAsync()
    {
        await WaitForSlotStatusAsync(VgcConstants.NicSlotDataReady);

        _nic.Write((ushort)VgcConstants.NicSlot, 0);
        _nic.Write((ushort)VgcConstants.NicDmaAddrL, (byte)(RecvBuffer & 0xFF));
        _nic.Write((ushort)VgcConstants.NicDmaAddrH, (byte)(RecvBuffer >> 8));
        _nic.Write((ushort)VgcConstants.NicCmd, VgcConstants.NicCmdRecv);

        int length = _nic.Read((ushort)VgcConstants.NicMsgLen);
        if (length == 0)
            length = 256;

        var frame = new byte[length];
        Array.Copy(_ram, RecvBuffer, frame, 0, length);
        return ProtocolCodec.Decode(frame);
    }

    private static ProtocolMessage Hello(byte sequence, string handle)
    {
        var writer = new ProtocolWriter();
        writer.WriteByte(NovaGameProtocol.ServerProtocolMinor);
        writer.WriteUInt16(0);
        writer.WriteString(handle);
        writer.WriteString("avalonia-nic-test");
        writer.WriteByte(1);
        writer.WriteByte(0);
        return ProtocolMessage.Create(MessageKind.Hello, sequence, payload: writer.ToArray());
    }
}

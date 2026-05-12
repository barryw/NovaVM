using System;
using System.Diagnostics;
using System.IO;
using System.Net;
using System.Threading;
using System.Threading.Tasks;
using e6502.Avalonia.Hardware;
using e6502.GameServer.Protocol;
using e6502.GameServer.Server;
using KDS.e6502;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

[TestClass]
public class GameServer6502RuntimeIntegrationTests
{
    private const ushort Entry = 0x7000;
    private const ushort Harness = 0x0200;
    private const ushort Done = 0x0206;
    private const ushort ResultBase = 0x22D0;

    [TestMethod]
    public async Task GameServerAssemblyRuntime_TalksToTcpGameServerThroughNic()
    {
        byte[] fixture = File.ReadAllBytes(RepoPath("tests", "integration", "fixtures", "gameserver_live_runtime.bin"));
        using var bus = new RuntimeBus(fixture, Entry);
        InstallHarness(bus);

        var state = new GameServerState();
        var server = new TcpGameServer(state, IPAddress.Loopback, 0);
        using var cts = new CancellationTokenSource();
        Task serverTask = server.RunAsync(cts.Token);

        string? previousHost = Environment.GetEnvironmentVariable("NOVA_GAME_SERVER_HOST");
        string? previousPort = Environment.GetEnvironmentVariable("NOVA_GAME_SERVER_PORT");

        try
        {
            int port = await WaitForServerPortAsync(server);
            Environment.SetEnvironmentVariable("NOVA_GAME_SERVER_HOST", "127.0.0.1");
            Environment.SetEnvironmentVariable("NOVA_GAME_SERVER_PORT", port.ToString());

            var cpu = new Cpu(bus, E6502Type.Cmos);
            cpu.Boot(Harness);
            await RunUntilDoneAsync(cpu, bus);

            AssertByte(bus, 0x00, 0x00, "NGS.SET_BUFFER returned OK");
            AssertByte(bus, 0x01, 0x00, "NGS.CONNECT_DEFAULT returned OK");
            AssertByte(bus, 0x02, 0x00, "NGS.WAIT_CONNECTED returned OK");
            AssertByte(bus, 0x03, 0x00, "HELLO builder returned OK");
            AssertByte(bus, 0x04, 0x00, "HELLO send returned OK");
            AssertByte(bus, 0x05, 0x00, "WELCOME wait returned OK");
            AssertByte(bus, 0x06, (byte)MessageKind.Welcome, "WELCOME kind");
            AssertByte(bus, 0x07, 0x01, "WELCOME sequence echoes HELLO");
            AssertFlag(bus, 0x08, (byte)MessageFlags.Response, "WELCOME response flag");
            Assert.AreNotEqual(0, ReadWord(bus, 0x09), "WELCOME session id");
            AssertByte(bus, 0x0B, 0x08, "WELCOME handle length");
            AssertByte(bus, 0x0C, (byte)'n', "WELCOME handle first byte");

            AssertByte(bus, 0x0D, 0x00, "GAME_LIST_REQUEST builder returned OK");
            AssertByte(bus, 0x0E, 0x00, "GAME_LIST_REQUEST send returned OK");
            AssertByte(bus, 0x0F, 0x00, "GAME_LIST_ITEM wait returned OK");
            AssertByte(bus, 0x10, (byte)MessageKind.GameListItem, "GAME_LIST_ITEM kind");
            AssertByte(bus, 0x11, 0x02, "GAME_LIST_ITEM sequence echoes request");
            AssertFlag(bus, 0x12, (byte)MessageFlags.Response, "GAME_LIST_ITEM response flag");
            Assert.AreEqual(1, ReadWord(bus, 0x13), "first advertised game id");

            AssertByte(bus, 0x15, 0x00, "TABLE_CREATE builder returned OK");
            AssertByte(bus, 0x16, 0x00, "TABLE_CREATE send returned OK");
            AssertByte(bus, 0x17, 0x00, "TABLE_CREATED wait returned OK");
            AssertByte(bus, 0x18, (byte)MessageKind.TableCreated, "TABLE_CREATED kind");
            AssertByte(bus, 0x19, 0x03, "TABLE_CREATED sequence echoes request");
            AssertFlag(bus, 0x1A, (byte)MessageFlags.Response, "TABLE_CREATED response flag");
            Assert.AreNotEqual(0, ReadWord(bus, 0x1B), "created table id");

            AssertByte(bus, 0x1D, 0x00, "PING roundtrip returned OK");
            AssertByte(bus, 0x1E, (byte)MessageKind.Pong, "PING roundtrip received PONG");
            AssertByte(bus, 0x1F, 0x04, "PONG sequence echoes direct PING");
            AssertFlag(bus, 0x20, (byte)MessageFlags.Response, "PONG response flag");

            AssertByte(bus, 0x21, 0x00, "first keepalive tick returned OK");
            AssertByte(bus, 0x22, 0x00, "second keepalive tick returned OK");
            AssertByte(bus, 0x23, 0x01, "keepalive ping is pending after tick sends");
            AssertByte(bus, 0x24, 0x00, "keepalive PONG wait returned OK");
            AssertByte(bus, 0x25, (byte)MessageKind.Pong, "keepalive received PONG");
            AssertByte(bus, 0x26, 0x00, "keepalive pending cleared after PONG");
            AssertByte(bus, 0x3F, 0xAA, "6502 live fixture reached completion");
        }
        finally
        {
            Environment.SetEnvironmentVariable("NOVA_GAME_SERVER_HOST", previousHost);
            Environment.SetEnvironmentVariable("NOVA_GAME_SERVER_PORT", previousPort);
            bus.DisconnectNic();
            cts.Cancel();
            await serverTask;
        }
    }

    private static void InstallHarness(RuntimeBus bus)
    {
        // LDX #$FF; TXS; JSR $7000; JMP $0206
        bus.WriteRam(Harness, 0xA2);
        bus.WriteRam(Harness + 1, 0xFF);
        bus.WriteRam(Harness + 2, 0x9A);
        bus.WriteRam(Harness + 3, 0x20);
        bus.WriteRam(Harness + 4, (byte)(Entry & 0xFF));
        bus.WriteRam(Harness + 5, (byte)(Entry >> 8));
        bus.WriteRam(Harness + 6, 0x4C);
        bus.WriteRam(Harness + 7, (byte)(Done & 0xFF));
        bus.WriteRam(Harness + 8, (byte)(Done >> 8));
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

    private static async Task RunUntilDoneAsync(Cpu cpu, RuntimeBus bus)
    {
        for (int i = 0; i < 2_000_000; i++)
        {
            ushort previousPc = cpu.Pc;
            cpu.ExecuteNext();

            if ((i & 0x3F) == 0)
                bus.AdvanceFrame();

            if ((i & 0x3FF) == 0)
                await Task.Delay(1);

            if (previousPc == Done && cpu.Pc == Done)
                return;
        }

        Assert.Fail($"6502 live game-server fixture did not return; PC=${cpu.Pc:X4}.");
    }

    private static void AssertByte(RuntimeBus bus, int offset, byte expected, string message)
    {
        Assert.AreEqual(expected, bus.ReadRam((ushort)(ResultBase + offset)), message);
    }

    private static void AssertFlag(RuntimeBus bus, int offset, byte mask, string message)
    {
        Assert.AreNotEqual(0, (byte)(bus.ReadRam((ushort)(ResultBase + offset)) & mask), message);
    }

    private static ushort ReadWord(RuntimeBus bus, int offset)
    {
        byte low = bus.ReadRam((ushort)(ResultBase + offset));
        byte high = bus.ReadRam((ushort)(ResultBase + offset + 1));
        return (ushort)(low | (high << 8));
    }

    private static string RepoPath(params string[] parts)
    {
        string root = Path.GetFullPath(Path.Combine(
            AppContext.BaseDirectory,
            "..",
            "..",
            "..",
            ".."));

        return Path.Combine([root, .. parts]);
    }

    private sealed class RuntimeBus : IBusDevice, IDisposable
    {
        private readonly byte[] _ram = new byte[0x10000];
        private readonly VirtualNetworkController _nic;
        private byte _frame;

        public RuntimeBus(byte[] program, ushort loadAddress)
        {
            Array.Copy(program, 0, _ram, loadAddress, program.Length);
            _nic = new VirtualNetworkController(
                addr => _ram[addr],
                (addr, data) => _ram[addr] = data);
        }

        public byte Read(ushort address)
        {
            if (address == VgcConstants.RegStatus)
                return _frame;

            return _nic.OwnsAddress(address) ? _nic.Read(address) : _ram[address];
        }

        public void Write(ushort address, byte data)
        {
            if (_nic.OwnsAddress(address))
            {
                _nic.Write(address, data);
                return;
            }

            if (address != VgcConstants.RegStatus)
                _ram[address] = data;
        }

        public byte ReadRam(ushort address) => _ram[address];

        public void WriteRam(int address, byte data) => _ram[address & 0xFFFF] = data;

        public void AdvanceFrame() => _frame++;

        public void DisconnectNic()
        {
            Write((ushort)VgcConstants.NicSlot, 0);
            Write((ushort)VgcConstants.NicCmd, VgcConstants.NicCmdDisconnect);
        }

        public void Dispose()
        {
            _nic.Dispose();
        }
    }
}

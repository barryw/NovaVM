# Network Controller (NIC) Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Add a message-oriented TCP network controller at $A100-$A13F with 4 connection slots, 256-byte messages, DMA transfer, and optional IRQ notification.

**Architecture:** Standard hardware controller pattern (`OwnsAddress`/`Read`/`Write` with command dispatch). Each connection slot manages a `TcpClient` with a background read task that enqueues framed messages. DMA delegates (`busRead`/`busWrite`) handle payload transfer to/from 6502 RAM, matching the FileIoController pattern.

**Tech Stack:** C#/.NET 10, MSTest, `System.Net.Sockets`

**Design doc:** `docs/plans/2026-02-22-network-controller-design.md`

---

### Task 1: Add NIC Constants to VgcConstants

**Files:**
- Modify: `e6502.Avalonia/Hardware/VgcConstants.cs:191` (after `FreeBase`/`FreeEnd`)

**Step 1: Add NIC register and command constants**

Insert after line 192 (`FreeEnd`), before the `RomBase` section:

```csharp
    // -------------------------------------------------------------------------
    // Network Interface Controller (NIC) registers ($A100-$A13F)
    // -------------------------------------------------------------------------

    public const int NicBase           = 0xA100;
    public const int NicEnd            = 0xA13F;

    public const int NicCmd            = 0xA100;   // write triggers command
    public const int NicStatus         = 0xA101;   // global status (read)
    public const int NicSlot           = 0xA102;   // active slot ID (0-3)
    public const int NicIrqCtrl        = 0xA103;   // IRQ enable mask (bit per slot)
    public const int NicIrqStatus      = 0xA104;   // IRQ pending flags (read-clears)

    public const int NicRemotePortL    = 0xA108;   // remote port low byte
    public const int NicRemotePortH    = 0xA109;   // remote port high byte
    public const int NicLocalPortL     = 0xA10A;   // local port low byte (listen)
    public const int NicLocalPortH     = 0xA10B;   // local port high byte

    public const int NicDmaAddrL       = 0xA110;   // DMA RAM address low
    public const int NicDmaAddrH       = 0xA111;   // DMA RAM address high
    public const int NicDmaLen         = 0xA112;   // DMA length (1-255, 0=256)
    public const int NicMsgLen         = 0xA113;   // received message length (read-only)

    public const int NicSlotStatus0    = 0xA118;   // slot 0 status
    public const int NicSlotStatus1    = 0xA119;   // slot 1 status
    public const int NicSlotStatus2    = 0xA11A;   // slot 2 status
    public const int NicSlotStatus3    = 0xA11B;   // slot 3 status

    public const int NicNameBuf        = 0xA120;   // hostname buffer (32 bytes)
    public const int NicNameEnd        = 0xA13F;   // end of hostname buffer

    // NIC commands
    public const byte NicCmdConnect    = 0x01;
    public const byte NicCmdDisconnect = 0x02;
    public const byte NicCmdSend       = 0x03;
    public const byte NicCmdRecv       = 0x04;
    public const byte NicCmdListen     = 0x05;
    public const byte NicCmdAccept     = 0x06;

    // NIC slot status bits
    public const byte NicSlotConnected    = 0x01;  // bit 0
    public const byte NicSlotDataReady    = 0x02;  // bit 1
    public const byte NicSlotSendReady    = 0x04;  // bit 2
    public const byte NicSlotError        = 0x08;  // bit 3
    public const byte NicSlotRemoteClosed = 0x10;  // bit 4

    // NIC global status bits
    public const byte NicStatusReady      = 0x01;  // bit 0
    public const byte NicStatusAnyData    = 0x02;  // bit 1
    public const byte NicStatusAnyError   = 0x80;  // bit 7

    public const int NicMaxSlots          = 4;
    public const int NicMaxMessageSize    = 256;
    public const int NicMaxQueueDepth     = 16;
```

**Step 2: Add NIC base to vector table**

In `CompositeBusDevice.cs:78`, add after the `Sid2Base` entry:

```csharp
        WriteWord(VgcConstants.VectorTableBase + 0x12, VgcConstants.NicBase);
```

**Step 3: Commit**

```bash
git add e6502.Avalonia/Hardware/VgcConstants.cs e6502.Avalonia/Hardware/CompositeBusDevice.cs
git commit -m "feat(nic): add NIC register and command constants"
```

---

### Task 2: NIC Controller Skeleton with Register Read/Write (TDD)

**Files:**
- Create: `e6502UnitTests/NicControllerTests.cs`
- Create: `e6502.Avalonia/Hardware/VirtualNetworkController.cs`

**Step 1: Write failing tests for register read/write and address ownership**

Create `e6502UnitTests/NicControllerTests.cs`:

```csharp
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
        Assert.AreEqual(VgcConstants.NicStatusReady, (byte)(status & VgcConstants.NicStatusReady));
    }

    [TestMethod]
    public void Nic_IrqStatus_ReadClears()
    {
        // Internally set an IRQ pending flag, then read should clear it
        _nic.Write((ushort)VgcConstants.NicIrqCtrl, 0x01); // enable IRQ for slot 0
        _nic.InjectTestMessage(0, new byte[] { 0x41 });     // inject data to trigger IRQ
        byte val = _nic.Read((ushort)VgcConstants.NicIrqStatus);
        Assert.AreNotEqual(0, val);
        byte val2 = _nic.Read((ushort)VgcConstants.NicIrqStatus);
        Assert.AreEqual(0, val2);
    }

    [TestMethod]
    public void Nic_SlotStatus_NoConnection_ReportsZero()
    {
        byte status = _nic.Read((ushort)VgcConstants.NicSlotStatus0);
        // Not connected, no data, send ready by default
        Assert.AreEqual(VgcConstants.NicSlotSendReady, (byte)(status & VgcConstants.NicSlotSendReady));
        Assert.AreEqual(0, (byte)(status & VgcConstants.NicSlotConnected));
    }
}
```

**Step 2: Run tests to verify they fail**

Run: `dotnet test --filter "FullyQualifiedName~NicController" --verbosity normal`
Expected: Build failure — `VirtualNetworkController` does not exist.

**Step 3: Implement the controller skeleton**

Create `e6502.Avalonia/Hardware/VirtualNetworkController.cs`:

```csharp
using System.Collections.Concurrent;
using System.Net.Sockets;

namespace e6502.Avalonia.Hardware;

public sealed class VirtualNetworkController : IDisposable
{
    private readonly byte[] _regs = new byte[VgcConstants.NicEnd - VgcConstants.NicBase + 1];
    private readonly Func<ushort, byte> _busRead;
    private readonly Action<ushort, byte> _busWrite;
    private readonly ConnectionSlot[] _slots = new ConnectionSlot[VgcConstants.NicMaxSlots];

    public bool IrqPending { get; private set; }

    public VirtualNetworkController(Func<ushort, byte> busRead, Action<ushort, byte> busWrite)
    {
        _busRead = busRead;
        _busWrite = busWrite;
        for (int i = 0; i < VgcConstants.NicMaxSlots; i++)
            _slots[i] = new ConnectionSlot();
    }

    public bool OwnsAddress(ushort address) =>
        address >= VgcConstants.NicBase && address <= VgcConstants.NicEnd;

    public byte Read(ushort address)
    {
        if (address == VgcConstants.NicStatus)
            return BuildGlobalStatus();

        if (address == VgcConstants.NicIrqStatus)
        {
            byte val = _regs[VgcConstants.NicIrqStatus - VgcConstants.NicBase];
            _regs[VgcConstants.NicIrqStatus - VgcConstants.NicBase] = 0;
            IrqPending = false;
            return val;
        }

        if (address >= VgcConstants.NicSlotStatus0 &&
            address <= VgcConstants.NicSlotStatus3)
        {
            int slot = address - VgcConstants.NicSlotStatus0;
            return _slots[slot].GetStatusByte();
        }

        if (address == VgcConstants.NicMsgLen)
            return _regs[VgcConstants.NicMsgLen - VgcConstants.NicBase];

        return _regs[address - VgcConstants.NicBase];
    }

    public void Write(ushort address, byte data)
    {
        _regs[address - VgcConstants.NicBase] = data;
        if (address == VgcConstants.NicCmd)
            ExecuteCommand(data);
    }

    private void ExecuteCommand(byte cmd)
    {
        int slotIndex = _regs[VgcConstants.NicSlot - VgcConstants.NicBase] & 0x03;
        switch (cmd)
        {
            case VgcConstants.NicCmdConnect:    DoConnect(slotIndex); break;
            case VgcConstants.NicCmdDisconnect: DoDisconnect(slotIndex); break;
            case VgcConstants.NicCmdSend:       DoSend(slotIndex); break;
            case VgcConstants.NicCmdRecv:       DoRecv(slotIndex); break;
            case VgcConstants.NicCmdListen:     DoListen(slotIndex); break;
            case VgcConstants.NicCmdAccept:     DoAccept(slotIndex); break;
        }
    }

    private byte BuildGlobalStatus()
    {
        byte status = VgcConstants.NicStatusReady;
        for (int i = 0; i < VgcConstants.NicMaxSlots; i++)
        {
            var s = _slots[i].GetStatusByte();
            if ((s & VgcConstants.NicSlotDataReady) != 0)
                status |= VgcConstants.NicStatusAnyData;
            if ((s & VgcConstants.NicSlotError) != 0)
                status |= VgcConstants.NicStatusAnyError;
        }
        return status;
    }

    // ── Test helper ──────────────────────────────────────────────────────

    /// <summary>Inject a message into a slot's receive queue (for testing without TCP).</summary>
    public void InjectTestMessage(int slotIndex, byte[] data)
    {
        _slots[slotIndex].Enqueue(data);
        byte irqCtrl = _regs[VgcConstants.NicIrqCtrl - VgcConstants.NicBase];
        if ((irqCtrl & (1 << slotIndex)) != 0)
        {
            _regs[VgcConstants.NicIrqStatus - VgcConstants.NicBase] |= (byte)(1 << slotIndex);
            IrqPending = true;
        }
    }

    // ── Command stubs (implemented in later tasks) ───────────────────────

    private void DoConnect(int slot) { }
    private void DoDisconnect(int slot) { }
    private void DoSend(int slot) { }
    private void DoRecv(int slot) { }
    private void DoListen(int slot) { }
    private void DoAccept(int slot) { }

    public void Dispose()
    {
        for (int i = 0; i < VgcConstants.NicMaxSlots; i++)
            _slots[i].Dispose();
    }

    // ── ConnectionSlot ───────────────────────────────────────────────────

    internal sealed class ConnectionSlot : IDisposable
    {
        private TcpClient? _client;
        private NetworkStream? _stream;
        private TcpListener? _listener;
        private CancellationTokenSource? _readCts;
        private Task? _readTask;
        private readonly ConcurrentQueue<byte[]> _receiveQueue = new();
        private volatile bool _connected;
        private volatile bool _error;
        private volatile bool _remoteClosed;
        private volatile bool _pendingAccept; // for listen mode

        public byte GetStatusByte()
        {
            byte status = 0;
            if (_connected) status |= VgcConstants.NicSlotConnected;
            if (!_receiveQueue.IsEmpty || _pendingAccept)
                status |= VgcConstants.NicSlotDataReady;
            if (_connected || !_connected) // always ready when idle or connected
                status |= VgcConstants.NicSlotSendReady;
            if (_error) status |= VgcConstants.NicSlotError;
            if (_remoteClosed) status |= VgcConstants.NicSlotRemoteClosed;
            return status;
        }

        public bool IsConnected => _connected;
        public bool HasData => !_receiveQueue.IsEmpty;
        public bool HasPendingAccept => _pendingAccept;

        public void Enqueue(byte[] data)
        {
            if (_receiveQueue.Count >= VgcConstants.NicMaxQueueDepth)
            {
                _receiveQueue.TryDequeue(out _); // drop oldest
                _error = true;
            }
            _receiveQueue.Enqueue(data);
        }

        public bool TryDequeue(out byte[] data) =>
            _receiveQueue.TryDequeue(out data!);

        public void Connect(TcpClient client)
        {
            _client = client;
            _stream = client.GetStream();
            _connected = true;
            _error = false;
            _remoteClosed = false;
        }

        public void StartReading(Action<int> onMessageReceived)
        {
            _readCts = new CancellationTokenSource();
            var ct = _readCts.Token;
            _readTask = Task.Run(async () =>
            {
                try
                {
                    while (!ct.IsCancellationRequested && _stream is not null)
                    {
                        int lenByte = await _stream.ReadByteAsync(ct);
                        if (lenByte < 0) { _remoteClosed = true; _connected = false; break; }

                        int payloadLen = lenByte == 0 ? 256 : lenByte;
                        var buf = new byte[payloadLen];
                        int read = 0;
                        while (read < payloadLen)
                        {
                            int n = await _stream.ReadAsync(buf.AsMemory(read, payloadLen - read), ct);
                            if (n == 0) { _remoteClosed = true; _connected = false; return; }
                            read += n;
                        }
                        Enqueue(buf);
                        onMessageReceived(0); // slot index filled by caller
                    }
                }
                catch (OperationCanceledException) { }
                catch (IOException) { _remoteClosed = true; _connected = false; }
            }, ct);
        }

        public void Send(byte[] data)
        {
            if (_stream is null || !_connected) { _error = true; return; }
            try
            {
                byte lenByte = (byte)(data.Length == 256 ? 0 : data.Length);
                _stream.WriteByte(lenByte);
                _stream.Write(data, 0, data.Length);
                _stream.Flush();
            }
            catch (IOException) { _error = true; _connected = false; }
        }

        public void StartListening(int port)
        {
            _listener = new TcpListener(System.Net.IPAddress.Any, port);
            _listener.Start();
            _ = AcceptOneAsync();
        }

        private async Task AcceptOneAsync()
        {
            try
            {
                var client = await _listener!.AcceptTcpClientAsync();
                _client = client;
                _stream = client.GetStream();
                _pendingAccept = true;
            }
            catch { _error = true; }
        }

        public bool AcceptPending()
        {
            if (!_pendingAccept) return false;
            _pendingAccept = false;
            _connected = true;
            _error = false;
            _remoteClosed = false;
            return true;
        }

        public void Reset()
        {
            _readCts?.Cancel();
            _readCts?.Dispose();
            _readCts = null;
            _readTask = null;
            _stream?.Dispose();
            _stream = null;
            _client?.Dispose();
            _client = null;
            _listener?.Stop();
            _listener = null;
            _connected = false;
            _error = false;
            _remoteClosed = false;
            _pendingAccept = false;
            while (_receiveQueue.TryDequeue(out _)) { }
        }

        public void Dispose() => Reset();
    }
}

// Extension for reading a single byte async from NetworkStream
file static class NetworkStreamExtensions
{
    public static async Task<int> ReadByteAsync(this NetworkStream stream, CancellationToken ct)
    {
        var buf = new byte[1];
        int n = await stream.ReadAsync(buf, 0, 1, ct);
        return n == 0 ? -1 : buf[0];
    }
}
```

**Step 4: Run tests to verify they pass**

Run: `dotnet test --filter "FullyQualifiedName~NicController" --verbosity normal`
Expected: All 5 tests PASS.

**Step 5: Commit**

```bash
git add e6502.Avalonia/Hardware/VirtualNetworkController.cs e6502UnitTests/NicControllerTests.cs
git commit -m "feat(nic): add VirtualNetworkController skeleton with register R/W and ConnectionSlot"
```

---

### Task 3: DMA Send and Receive (TDD)

**Files:**
- Modify: `e6502UnitTests/NicControllerTests.cs`
- Modify: `e6502.Avalonia/Hardware/VirtualNetworkController.cs`

**Step 1: Write failing tests for DMA send and receive**

Add to `NicControllerTests.cs`:

```csharp
    [TestMethod]
    public void Nic_Send_DmaReadsFromRam()
    {
        // Put data at $2000 in RAM
        _ram[0x2000] = 0x48; // H
        _ram[0x2001] = 0x49; // I
        _ram[0x2002] = 0x21; // !

        // Inject a message then verify DMA read works by using InjectTestMessage
        // For this test, we test the DMA plumbing via DoRecv
        _nic.InjectTestMessage(0, new byte[] { 0x48, 0x49, 0x21 });

        // Set DMA addr and trigger receive
        _nic.Write((ushort)VgcConstants.NicSlot, 0);
        _nic.Write((ushort)VgcConstants.NicDmaAddrL, 0x80);
        _nic.Write((ushort)VgcConstants.NicDmaAddrH, 0x20);
        _nic.Write((ushort)VgcConstants.NicCmd, VgcConstants.NicCmdRecv);

        // Verify DMA wrote to $2080
        Assert.AreEqual(0x48, _ram[0x2080]);
        Assert.AreEqual(0x49, _ram[0x2081]);
        Assert.AreEqual(0x21, _ram[0x2082]);

        // Verify MsgLen was set
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
    public void Nic_Send_DmaReadsRamIntoSlot()
    {
        // Put "HI!" at $2000
        _ram[0x2000] = 0x48;
        _ram[0x2001] = 0x49;
        _ram[0x2002] = 0x21;

        _nic.Write((ushort)VgcConstants.NicSlot, 0);
        _nic.Write((ushort)VgcConstants.NicDmaAddrL, 0x00);
        _nic.Write((ushort)VgcConstants.NicDmaAddrH, 0x20);
        _nic.Write((ushort)VgcConstants.NicDmaLen, 3);

        // Send on unconnected slot — should set error
        _nic.Write((ushort)VgcConstants.NicCmd, VgcConstants.NicCmdSend);
        byte slotStatus = _nic.Read((ushort)VgcConstants.NicSlotStatus0);
        Assert.AreNotEqual(0, (byte)(slotStatus & VgcConstants.NicSlotError));
    }

    [TestMethod]
    public void Nic_Recv_SlotStatusClearsDataReady_WhenQueueEmpty()
    {
        _nic.InjectTestMessage(0, new byte[] { 0x41 });
        Assert.AreNotEqual(0, (byte)(_nic.Read((ushort)VgcConstants.NicSlotStatus0) & VgcConstants.NicSlotDataReady));

        _nic.Write((ushort)VgcConstants.NicSlot, 0);
        _nic.Write((ushort)VgcConstants.NicDmaAddrL, 0x00);
        _nic.Write((ushort)VgcConstants.NicDmaAddrH, 0x20);
        _nic.Write((ushort)VgcConstants.NicCmd, VgcConstants.NicCmdRecv);

        Assert.AreEqual(0, (byte)(_nic.Read((ushort)VgcConstants.NicSlotStatus0) & VgcConstants.NicSlotDataReady));
    }
```

**Step 2: Run tests to verify they fail**

Run: `dotnet test --filter "FullyQualifiedName~NicController" --verbosity normal`
Expected: DMA tests fail — `DoSend` and `DoRecv` are empty stubs.

**Step 3: Implement DoSend and DoRecv**

Replace the stubs in `VirtualNetworkController.cs`:

```csharp
    private void DoSend(int slot)
    {
        var s = _slots[slot];
        ushort addr = (ushort)(_regs[VgcConstants.NicDmaAddrL - VgcConstants.NicBase]
                             | (_regs[VgcConstants.NicDmaAddrH - VgcConstants.NicBase] << 8));
        int len = _regs[VgcConstants.NicDmaLen - VgcConstants.NicBase];
        if (len == 0) len = 256;

        var data = new byte[len];
        for (int i = 0; i < len; i++)
            data[i] = _busRead((ushort)(addr + i));

        s.Send(data);
    }

    private void DoRecv(int slot)
    {
        var s = _slots[slot];
        if (!s.TryDequeue(out var data))
        {
            _regs[VgcConstants.NicMsgLen - VgcConstants.NicBase] = 0;
            return;
        }

        ushort addr = (ushort)(_regs[VgcConstants.NicDmaAddrL - VgcConstants.NicBase]
                             | (_regs[VgcConstants.NicDmaAddrH - VgcConstants.NicBase] << 8));
        for (int i = 0; i < data.Length; i++)
            _busWrite((ushort)(addr + i), data[i]);

        _regs[VgcConstants.NicMsgLen - VgcConstants.NicBase] = (byte)(data.Length == 256 ? 0 : data.Length);
    }
```

**Step 4: Run tests to verify they pass**

Run: `dotnet test --filter "FullyQualifiedName~NicController" --verbosity normal`
Expected: All tests PASS.

**Step 5: Commit**

```bash
git add e6502.Avalonia/Hardware/VirtualNetworkController.cs e6502UnitTests/NicControllerTests.cs
git commit -m "feat(nic): implement DMA send and receive commands"
```

---

### Task 4: TCP Connect and Disconnect (TDD)

**Files:**
- Modify: `e6502UnitTests/NicControllerTests.cs`
- Modify: `e6502.Avalonia/Hardware/VirtualNetworkController.cs`

**Step 1: Write failing tests using loopback TCP**

Add to `NicControllerTests.cs`:

```csharp
    [TestMethod]
    public async Task Nic_Connect_ToLoopbackServer()
    {
        using var server = new TcpListener(System.Net.IPAddress.Loopback, 0);
        server.Start();
        int port = ((System.Net.IPEndPoint)server.LocalEndpoint).Port;

        // Write hostname "127.0.0.1" to NicNameBuf
        string host = "127.0.0.1";
        for (int i = 0; i < host.Length; i++)
            _nic.Write((ushort)(VgcConstants.NicNameBuf + i), (byte)host[i]);
        _nic.Write((ushort)(VgcConstants.NicNameBuf + host.Length), 0); // null term

        _nic.Write((ushort)VgcConstants.NicSlot, 0);
        _nic.Write((ushort)VgcConstants.NicRemotePortL, (byte)(port & 0xFF));
        _nic.Write((ushort)VgcConstants.NicRemotePortH, (byte)(port >> 8));
        _nic.Write((ushort)VgcConstants.NicCmd, VgcConstants.NicCmdConnect);

        // Accept on server side
        var acceptTask = server.AcceptTcpClientAsync();

        // Poll until connected (with timeout)
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

        // Give async connect time to fail
        Thread.Sleep(3000);
        byte status = _nic.Read((ushort)VgcConstants.NicSlotStatus0);
        Assert.AreNotEqual(0, (byte)(status & VgcConstants.NicSlotError));
    }
```

**Step 2: Run tests to verify they fail**

Run: `dotnet test --filter "FullyQualifiedName~NicController" --verbosity normal`
Expected: Connect tests fail — `DoConnect` is empty.

**Step 3: Implement DoConnect and DoDisconnect**

Replace stubs in `VirtualNetworkController.cs`:

```csharp
    private void DoConnect(int slot)
    {
        var s = _slots[slot];
        s.Reset();

        // Read hostname from NicNameBuf
        int nameStart = VgcConstants.NicNameBuf - VgcConstants.NicBase;
        int nameLen = 0;
        while (nameLen < 32 && _regs[nameStart + nameLen] != 0)
            nameLen++;
        string hostname = System.Text.Encoding.ASCII.GetString(_regs, nameStart, nameLen);

        int port = _regs[VgcConstants.NicRemotePortL - VgcConstants.NicBase]
                 | (_regs[VgcConstants.NicRemotePortH - VgcConstants.NicBase] << 8);

        _ = ConnectAsync(slot, hostname, port);
    }

    private async Task ConnectAsync(int slot, string hostname, int port)
    {
        var s = _slots[slot];
        try
        {
            var client = new TcpClient();
            await client.ConnectAsync(hostname, port);
            s.Connect(client);
            s.StartReading(slotIdx => OnMessageReceived(slot));
        }
        catch
        {
            s.SetError();
        }
    }

    private void OnMessageReceived(int slot)
    {
        byte irqCtrl = _regs[VgcConstants.NicIrqCtrl - VgcConstants.NicBase];
        if ((irqCtrl & (1 << slot)) != 0)
        {
            _regs[VgcConstants.NicIrqStatus - VgcConstants.NicBase] |= (byte)(1 << slot);
            IrqPending = true;
        }
    }

    private void DoDisconnect(int slot)
    {
        _slots[slot].Reset();
    }
```

Also add `SetError()` method to `ConnectionSlot`:

```csharp
        public void SetError() { _error = true; }
```

**Step 4: Run tests to verify they pass**

Run: `dotnet test --filter "FullyQualifiedName~NicController" --verbosity normal`
Expected: All tests PASS.

**Step 5: Commit**

```bash
git add e6502.Avalonia/Hardware/VirtualNetworkController.cs e6502UnitTests/NicControllerTests.cs
git commit -m "feat(nic): implement TCP connect and disconnect commands"
```

---

### Task 5: End-to-End Send/Receive Over TCP (TDD)

**Files:**
- Modify: `e6502UnitTests/NicControllerTests.cs`
- Modify: `e6502.Avalonia/Hardware/VirtualNetworkController.cs` (if needed)

**Step 1: Write failing test for TCP round-trip**

Add to `NicControllerTests.cs`:

```csharp
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
        Assert.AreEqual(3, recvBuf[0]);       // length prefix
        Assert.AreEqual(0x48, recvBuf[1]);    // H
        Assert.AreEqual(0x49, recvBuf[2]);    // I
        Assert.AreEqual(0x21, recvBuf[3]);    // !

        // Server sends reply: "OK"
        serverStream.WriteByte(2);            // length = 2
        serverStream.Write(new byte[] { 0x4F, 0x4B }); // OK
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
        Assert.AreEqual(0x4F, _ram[0x2080]); // O
        Assert.AreEqual(0x4B, _ram[0x2081]); // K

        _nic.Write((ushort)VgcConstants.NicCmd, VgcConstants.NicCmdDisconnect);
        server.Stop();
    }
```

**Step 2: Run tests to verify it fails or passes**

Run: `dotnet test --filter "Nic_SendRecv_RoundTrip" --verbosity normal`
Expected: If Task 3 and 4 are implemented correctly, this should PASS. If it fails, debug the wire protocol framing.

**Step 3: Commit**

```bash
git add e6502UnitTests/NicControllerTests.cs
git commit -m "test(nic): add end-to-end TCP send/receive round-trip test"
```

---

### Task 6: Listen and Accept (TDD)

**Files:**
- Modify: `e6502UnitTests/NicControllerTests.cs`
- Modify: `e6502.Avalonia/Hardware/VirtualNetworkController.cs`

**Step 1: Write failing test for listen/accept**

Add to `NicControllerTests.cs`:

```csharp
    [TestMethod]
    public async Task Nic_Listen_Accept_ReceivesConnection()
    {
        // Listen on slot 0
        _nic.Write((ushort)VgcConstants.NicSlot, 0);
        _nic.Write((ushort)VgcConstants.NicLocalPortL, 0x00);
        _nic.Write((ushort)VgcConstants.NicLocalPortH, 0x00); // port 0 = OS assigns
        _nic.Write((ushort)VgcConstants.NicCmd, VgcConstants.NicCmdListen);

        // Get the assigned port
        int assignedPort = _nic.GetListeningPort(0);
        Assert.AreNotEqual(0, assignedPort);

        // Client connects
        using var client = new TcpClient();
        await client.ConnectAsync("127.0.0.1", assignedPort);

        // Wait for pending accept
        var sw = System.Diagnostics.Stopwatch.StartNew();
        while (sw.ElapsedMilliseconds < 2000 &&
               (_nic.Read((ushort)VgcConstants.NicSlotStatus0) & VgcConstants.NicSlotDataReady) == 0)
            await Task.Delay(10);

        // Accept
        _nic.Write((ushort)VgcConstants.NicSlot, 0);
        _nic.Write((ushort)VgcConstants.NicCmd, VgcConstants.NicCmdAccept);

        Assert.AreNotEqual(0, (byte)(_nic.Read((ushort)VgcConstants.NicSlotStatus0) & VgcConstants.NicSlotConnected));

        _nic.Write((ushort)VgcConstants.NicCmd, VgcConstants.NicCmdDisconnect);
    }
```

**Step 2: Run test to verify it fails**

Run: `dotnet test --filter "Nic_Listen_Accept" --verbosity normal`
Expected: Fails — `DoListen`/`DoAccept` are stubs, `GetListeningPort` doesn't exist.

**Step 3: Implement DoListen, DoAccept, and GetListeningPort**

Replace stubs in `VirtualNetworkController.cs`:

```csharp
    private void DoListen(int slot)
    {
        var s = _slots[slot];
        s.Reset();
        int port = _regs[VgcConstants.NicLocalPortL - VgcConstants.NicBase]
                 | (_regs[VgcConstants.NicLocalPortH - VgcConstants.NicBase] << 8);
        s.StartListening(port);
    }

    private void DoAccept(int slot)
    {
        var s = _slots[slot];
        if (s.AcceptPending())
            s.StartReading(_ => OnMessageReceived(slot));
    }

    /// <summary>Get the actual listening port for a slot (for testing with port 0).</summary>
    public int GetListeningPort(int slot) => _slots[slot].ListeningPort;
```

Add to `ConnectionSlot`:

```csharp
        public int ListeningPort =>
            _listener?.LocalEndpoint is System.Net.IPEndPoint ep ? ep.Port : 0;
```

**Step 4: Run tests to verify they pass**

Run: `dotnet test --filter "FullyQualifiedName~NicController" --verbosity normal`
Expected: All tests PASS.

**Step 5: Commit**

```bash
git add e6502.Avalonia/Hardware/VirtualNetworkController.cs e6502UnitTests/NicControllerTests.cs
git commit -m "feat(nic): implement listen and accept commands"
```

---

### Task 7: IRQ Notification (TDD)

**Files:**
- Modify: `e6502UnitTests/NicControllerTests.cs`

**Step 1: Write test for IRQ on message arrival over TCP**

Add to `NicControllerTests.cs`:

```csharp
    [TestMethod]
    public async Task Nic_Irq_FiredOnMessageArrival()
    {
        using var server = new TcpListener(System.Net.IPAddress.Loopback, 0);
        server.Start();
        int port = ((System.Net.IPEndPoint)server.LocalEndpoint).Port;

        // Connect slot 0
        string host = "127.0.0.1";
        for (int i = 0; i < host.Length; i++)
            _nic.Write((ushort)(VgcConstants.NicNameBuf + i), (byte)host[i]);
        _nic.Write((ushort)(VgcConstants.NicNameBuf + host.Length), 0);
        _nic.Write((ushort)VgcConstants.NicSlot, 0);
        _nic.Write((ushort)VgcConstants.NicRemotePortL, (byte)(port & 0xFF));
        _nic.Write((ushort)VgcConstants.NicRemotePortH, (byte)(port >> 8));

        // Enable IRQ for slot 0 BEFORE connecting
        _nic.Write((ushort)VgcConstants.NicIrqCtrl, 0x01);

        _nic.Write((ushort)VgcConstants.NicCmd, VgcConstants.NicCmdConnect);
        using var serverClient = await server.AcceptTcpClientAsync();

        // Wait connected
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

        // Reading IrqStatus clears it
        byte irqStatus = _nic.Read((ushort)VgcConstants.NicIrqStatus);
        Assert.AreEqual(0x01, irqStatus); // slot 0 bit
        Assert.IsFalse(_nic.IrqPending);

        _nic.Write((ushort)VgcConstants.NicSlot, 0);
        _nic.Write((ushort)VgcConstants.NicCmd, VgcConstants.NicCmdDisconnect);
        server.Stop();
    }
```

**Step 2: Run test**

Run: `dotnet test --filter "Nic_Irq_FiredOnMessageArrival" --verbosity normal`
Expected: PASS (IRQ logic was implemented in Task 2's skeleton + Task 4's `OnMessageReceived`).

**Step 3: Commit**

```bash
git add e6502UnitTests/NicControllerTests.cs
git commit -m "test(nic): add IRQ notification test over TCP"
```

---

### Task 8: Wire NIC into CompositeBusDevice and MainWindow

**Files:**
- Modify: `e6502.Avalonia/Hardware/CompositeBusDevice.cs`
- Modify: `e6502.Avalonia/MainWindow.axaml.cs`
- Modify: `e6502UnitTests/AvaloniaCompositeBusTests.cs`

**Step 1: Write test for NIC routing through CompositeBusDevice**

Check existing test file first, then add:

```csharp
    [TestMethod]
    public void Bus_NicRegisters_Routed()
    {
        var bus = new CompositeBusDevice(enableSound: false);
        bus.Write((ushort)VgcConstants.NicSlot, 2);
        Assert.AreEqual(2, bus.Read((ushort)VgcConstants.NicSlot));
    }
```

**Step 2: Add NIC to CompositeBusDevice**

In `CompositeBusDevice.cs`:

Add field (after line 13, `_timer`):
```csharp
    private readonly VirtualNetworkController _nic;
```

Add property (after line 21, `Timer`):
```csharp
    public VirtualNetworkController Nic => _nic;
```

In constructor (after line 44, XMC creation):
```csharp
        _nic = new VirtualNetworkController(
            addr => _ram[addr],
            (addr, data) => _ram[addr] = data);
```

In `Read()` method (after line 87, timer check):
```csharp
        if (_nic.OwnsAddress(address)) return _nic.Read(address);
```

In `Write()` method (after line 96, timer check):
```csharp
        if (_nic.OwnsAddress(address)) { _nic.Write(address, data); return; }
```

In `Dispose()` (after line 59):
```csharp
        _nic.Dispose();
```

In `InitVectorTable()` (after line 78):
```csharp
        WriteWord(VgcConstants.VectorTableBase + 0x12, VgcConstants.NicBase);
```

**Step 3: Add NIC IRQ check to MainWindow CPU loop**

In `MainWindow.axaml.cs`, after line 70 (`_cpu.IrqWaiting = true;` for timer):
```csharp
                    if (_bus.Nic.IrqPending)
                        _cpu.IrqWaiting = true;
```

**Step 4: Run all tests**

Run: `dotnet test --verbosity normal`
Expected: All tests PASS (existing + new).

**Step 5: Commit**

```bash
git add e6502.Avalonia/Hardware/CompositeBusDevice.cs e6502.Avalonia/MainWindow.axaml.cs e6502UnitTests/AvaloniaCompositeBusTests.cs
git commit -m "feat(nic): wire NIC into CompositeBusDevice and CPU loop"
```

---

### Task 9: TCP Server Commands

**Files:**
- Modify: `e6502.Avalonia/Ipc/EmulatorTcpServer.cs`

**Step 1: Add NIC commands to the switch expression**

In `EmulatorTcpServer.cs`, in the `cmd switch` (around line 160, before the `_ =>` default):

```csharp
                // Network commands
                "nic_connect" => CmdNicConnect(req),
                "nic_disconnect" => CmdNicDisconnect(req),
                "nic_send" => CmdNicSend(req),
                "nic_recv" => CmdNicRecv(req),
                "nic_listen" => CmdNicListen(req),
                "nic_accept" => CmdNicAccept(req),
                "nic_status" => CmdNicStatus(req),
```

**Step 2: Implement command handlers**

Add to `EmulatorTcpServer.cs` before the Helpers section:

```csharp
    // ── Network commands ──────────────────────────────────────────────────

    private string CmdNicConnect(JsonNode req)
    {
        int? slot = req["slot"]?.GetValue<int>();
        string? host = req["host"]?.GetValue<string>();
        int? port = req["port"]?.GetValue<int>();
        if (slot is null || host is null || port is null)
            return Error("Need slot, host, port");

        _bus.Write((ushort)VgcConstants.NicSlot, (byte)slot.Value);
        // Write hostname
        for (int i = 0; i < host.Length && i < 31; i++)
            _bus.Write((ushort)(VgcConstants.NicNameBuf + i), (byte)host[i]);
        _bus.Write((ushort)(VgcConstants.NicNameBuf + Math.Min(host.Length, 31)), 0);
        _bus.Write((ushort)VgcConstants.NicRemotePortL, (byte)(port.Value & 0xFF));
        _bus.Write((ushort)VgcConstants.NicRemotePortH, (byte)(port.Value >> 8));
        _bus.Write((ushort)VgcConstants.NicCmd, VgcConstants.NicCmdConnect);
        return Ok();
    }

    private string CmdNicDisconnect(JsonNode req)
    {
        int? slot = req["slot"]?.GetValue<int>();
        if (slot is null) return Error("Need slot");
        _bus.Write((ushort)VgcConstants.NicSlot, (byte)slot.Value);
        _bus.Write((ushort)VgcConstants.NicCmd, VgcConstants.NicCmdDisconnect);
        return Ok();
    }

    private string CmdNicSend(JsonNode req)
    {
        int? slot = req["slot"]?.GetValue<int>();
        string? data = req["data"]?.GetValue<string>();
        if (slot is null || data is null) return Error("Need slot, data (base64)");

        byte[] payload = Convert.FromBase64String(data);
        if (payload.Length == 0 || payload.Length > 256) return Error("Data must be 1-256 bytes");

        // Write payload to scratch area in RAM
        ushort scratchAddr = 0x0280; // use start of BASIC area temporarily
        for (int i = 0; i < payload.Length; i++)
            _bus.WriteRam((ushort)(scratchAddr + i), payload[i]);

        _bus.Write((ushort)VgcConstants.NicSlot, (byte)slot.Value);
        _bus.Write((ushort)VgcConstants.NicDmaAddrL, (byte)(scratchAddr & 0xFF));
        _bus.Write((ushort)VgcConstants.NicDmaAddrH, (byte)(scratchAddr >> 8));
        _bus.Write((ushort)VgcConstants.NicDmaLen, (byte)(payload.Length == 256 ? 0 : payload.Length));
        _bus.Write((ushort)VgcConstants.NicCmd, VgcConstants.NicCmdSend);
        return Ok();
    }

    private string CmdNicRecv(JsonNode req)
    {
        int? slot = req["slot"]?.GetValue<int>();
        if (slot is null) return Error("Need slot");

        ushort scratchAddr = 0x0280;
        _bus.Write((ushort)VgcConstants.NicSlot, (byte)slot.Value);
        _bus.Write((ushort)VgcConstants.NicDmaAddrL, (byte)(scratchAddr & 0xFF));
        _bus.Write((ushort)VgcConstants.NicDmaAddrH, (byte)(scratchAddr >> 8));
        _bus.Write((ushort)VgcConstants.NicCmd, VgcConstants.NicCmdRecv);

        int len = _bus.Read((ushort)VgcConstants.NicMsgLen);
        if (len == 0)
        {
            return new JsonObject { ["ok"] = true, ["data"] = "", ["length"] = 0 }.ToJsonString();
        }

        var payload = new byte[len];
        for (int i = 0; i < len; i++)
            payload[i] = _bus.Read((ushort)(scratchAddr + i));

        return new JsonObject
        {
            ["ok"] = true,
            ["data"] = Convert.ToBase64String(payload),
            ["length"] = len
        }.ToJsonString();
    }

    private string CmdNicListen(JsonNode req)
    {
        int? slot = req["slot"]?.GetValue<int>();
        int? port = req["port"]?.GetValue<int>();
        if (slot is null || port is null) return Error("Need slot, port");

        _bus.Write((ushort)VgcConstants.NicSlot, (byte)slot.Value);
        _bus.Write((ushort)VgcConstants.NicLocalPortL, (byte)(port.Value & 0xFF));
        _bus.Write((ushort)VgcConstants.NicLocalPortH, (byte)(port.Value >> 8));
        _bus.Write((ushort)VgcConstants.NicCmd, VgcConstants.NicCmdListen);
        return Ok();
    }

    private string CmdNicAccept(JsonNode req)
    {
        int? slot = req["slot"]?.GetValue<int>();
        if (slot is null) return Error("Need slot");
        _bus.Write((ushort)VgcConstants.NicSlot, (byte)slot.Value);
        _bus.Write((ushort)VgcConstants.NicCmd, VgcConstants.NicCmdAccept);
        return Ok();
    }

    private string CmdNicStatus(JsonNode req)
    {
        int? slot = req["slot"]?.GetValue<int>();
        if (slot is null) return Error("Need slot");

        int slotAddr = VgcConstants.NicSlotStatus0 + slot.Value;
        byte slotStatus = _bus.Read((ushort)slotAddr);
        byte globalStatus = _bus.Read((ushort)VgcConstants.NicStatus);

        return new JsonObject
        {
            ["ok"] = true,
            ["slot"] = slot.Value,
            ["connected"] = (slotStatus & VgcConstants.NicSlotConnected) != 0,
            ["data_ready"] = (slotStatus & VgcConstants.NicSlotDataReady) != 0,
            ["send_ready"] = (slotStatus & VgcConstants.NicSlotSendReady) != 0,
            ["error"] = (slotStatus & VgcConstants.NicSlotError) != 0,
            ["remote_closed"] = (slotStatus & VgcConstants.NicSlotRemoteClosed) != 0,
            ["global_status"] = globalStatus
        }.ToJsonString();
    }
```

**Step 3: Build to verify**

Run: `dotnet build`
Expected: Build succeeds.

**Step 4: Commit**

```bash
git add e6502.Avalonia/Ipc/EmulatorTcpServer.cs
git commit -m "feat(nic): add TCP server commands for NIC"
```

---

### Task 10: MCP Tools

**Files:**
- Modify: `e6502.MCP/EmulatorTools.cs`

**Step 1: Add MCP tool methods**

Add to `EmulatorTools.cs`:

```csharp
    // ── Network (NIC) ─────────────────────────────────────────────────────

    [McpServerTool, Description("Connect a NIC slot to a remote host. Poll nic_status to check connection.")]
    public static async Task<string> NicConnect(
        EmulatorClient client,
        [Description("Slot ID (0-3)")] int slot,
        [Description("Remote hostname or IP")] string host,
        [Description("Remote port")] int port)
    {
        var result = await client.SendAsync(new JsonObject
        {
            ["command"] = "nic_connect",
            ["slot"] = slot,
            ["host"] = host,
            ["port"] = port
        });
        return result.ToJsonString();
    }

    [McpServerTool, Description("Disconnect a NIC slot.")]
    public static async Task<string> NicDisconnect(
        EmulatorClient client,
        [Description("Slot ID (0-3)")] int slot)
    {
        var result = await client.SendAsync(new JsonObject
        {
            ["command"] = "nic_disconnect",
            ["slot"] = slot
        });
        return result.ToJsonString();
    }

    [McpServerTool, Description("Send data on a NIC slot. Data is base64-encoded, max 256 bytes.")]
    public static async Task<string> NicSend(
        EmulatorClient client,
        [Description("Slot ID (0-3)")] int slot,
        [Description("Base64-encoded data to send (max 256 bytes)")] string data)
    {
        var result = await client.SendAsync(new JsonObject
        {
            ["command"] = "nic_send",
            ["slot"] = slot,
            ["data"] = data
        });
        return result.ToJsonString();
    }

    [McpServerTool, Description("Receive a message from a NIC slot. Returns base64-encoded data.")]
    public static async Task<string> NicRecv(
        EmulatorClient client,
        [Description("Slot ID (0-3)")] int slot)
    {
        var result = await client.SendAsync(new JsonObject
        {
            ["command"] = "nic_recv",
            ["slot"] = slot
        });
        return result.ToJsonString();
    }

    [McpServerTool, Description("Listen for incoming connections on a NIC slot.")]
    public static async Task<string> NicListen(
        EmulatorClient client,
        [Description("Slot ID (0-3)")] int slot,
        [Description("Local port to listen on")] int port)
    {
        var result = await client.SendAsync(new JsonObject
        {
            ["command"] = "nic_listen",
            ["slot"] = slot,
            ["port"] = port
        });
        return result.ToJsonString();
    }

    [McpServerTool, Description("Accept a pending connection on a listening NIC slot.")]
    public static async Task<string> NicAccept(
        EmulatorClient client,
        [Description("Slot ID (0-3)")] int slot)
    {
        var result = await client.SendAsync(new JsonObject
        {
            ["command"] = "nic_accept",
            ["slot"] = slot
        });
        return result.ToJsonString();
    }

    [McpServerTool, Description("Get the status of a NIC slot (connected, data ready, errors).")]
    public static async Task<string> NicStatus(
        EmulatorClient client,
        [Description("Slot ID (0-3)")] int slot)
    {
        var result = await client.SendAsync(new JsonObject
        {
            ["command"] = "nic_status",
            ["slot"] = slot
        });
        return result.ToJsonString();
    }
```

**Step 2: Build to verify**

Run: `dotnet build`
Expected: Build succeeds.

**Step 3: Commit**

```bash
git add e6502.MCP/EmulatorTools.cs
git commit -m "feat(nic): add MCP tools for NIC commands"
```

---

### Task 11: Final Verification

**Step 1: Run full test suite**

Run: `dotnet test --verbosity normal`
Expected: All existing tests + all new NIC tests PASS. Zero regressions.

**Step 2: Build all projects**

Run: `dotnet build`
Expected: Clean build, no warnings from NIC code.

**Step 3: Commit any cleanup**

If any issues found, fix and commit.

```bash
git commit -m "chore(nic): final cleanup and verification"
```

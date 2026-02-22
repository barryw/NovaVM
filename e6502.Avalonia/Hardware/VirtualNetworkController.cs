using System.Collections.Concurrent;
using System.Net;
using System.Net.Sockets;

namespace e6502.Avalonia.Hardware;

public sealed class VirtualNetworkController : IDisposable
{
    private readonly byte[] _regs = new byte[VgcConstants.NicEnd - VgcConstants.NicBase + 1];
    private readonly Func<ushort, byte> _busRead;
    private readonly Action<ushort, byte> _busWrite;
    private readonly ConnectionSlot[] _slots = new ConnectionSlot[VgcConstants.NicMaxSlots];
    private readonly object _irqLock = new();

    public bool IrqPending { get; private set; }

    public VirtualNetworkController(Func<ushort, byte> busRead, Action<ushort, byte> busWrite)
    {
        _busRead = busRead;
        _busWrite = busWrite;
        for (int i = 0; i < _slots.Length; i++)
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
            lock (_irqLock)
            {
                byte val = _regs[address - VgcConstants.NicBase];
                _regs[address - VgcConstants.NicBase] = 0;
                IrqPending = false;
                return val;
            }
        }

        if (address >= VgcConstants.NicSlotStatus0 && address <= VgcConstants.NicSlotStatus3)
        {
            int slot = address - VgcConstants.NicSlotStatus0;
            return _slots[slot].GetStatusByte();
        }

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

    private void DoConnect(int slot) { }
    private void DoDisconnect(int slot) { }
    private void DoSend(int slot) { }
    private void DoRecv(int slot) { }
    private void DoListen(int slot) { }
    private void DoAccept(int slot) { }

    private byte BuildGlobalStatus()
    {
        byte status = VgcConstants.NicStatusReady;

        for (int i = 0; i < _slots.Length; i++)
        {
            if (_slots[i].HasData)
                status |= VgcConstants.NicStatusAnyData;
            if (_slots[i].HasError)
                status |= VgcConstants.NicStatusAnyError;
        }

        return status;
    }

    public void InjectTestMessage(int slotIndex, byte[] data)
    {
        _slots[slotIndex].Enqueue(data);
        OnMessageReceived(slotIndex);
    }

    private void OnMessageReceived(int slot)
    {
        byte irqCtrl = _regs[VgcConstants.NicIrqCtrl - VgcConstants.NicBase];
        if ((irqCtrl & (1 << slot)) != 0)
        {
            lock (_irqLock)
            {
                _regs[VgcConstants.NicIrqStatus - VgcConstants.NicBase] |= (byte)(1 << slot);
                IrqPending = true;
            }
        }
    }

    public void Dispose()
    {
        for (int i = 0; i < _slots.Length; i++)
            _slots[i].Dispose();
    }

    // -------------------------------------------------------------------------
    // ConnectionSlot — manages one TCP connection
    // -------------------------------------------------------------------------

    internal sealed class ConnectionSlot : IDisposable
    {
        private TcpClient? _client;
        private NetworkStream? _stream;
        private TcpListener? _listener;
        private CancellationTokenSource? _cts;
        private Task? _readTask;
        private readonly ConcurrentQueue<byte[]> _receiveQueue = new();

        private volatile bool _connected;
        private volatile bool _error;
        private volatile bool _remoteClosed;
        private volatile bool _pendingAccept;

        public bool IsConnected => _connected;
        public bool HasData => !_receiveQueue.IsEmpty;
        public bool HasError => _error;
        public bool HasPendingAccept => _pendingAccept;
        public int? ListeningPort => _listener?.LocalEndpoint is IPEndPoint ep ? ep.Port : null;

        public byte GetStatusByte()
        {
            byte status = VgcConstants.NicSlotSendReady; // always set
            if (_connected) status |= VgcConstants.NicSlotConnected;
            if (!_receiveQueue.IsEmpty || _pendingAccept) status |= VgcConstants.NicSlotDataReady;
            if (_error) status |= VgcConstants.NicSlotError;
            if (_remoteClosed) status |= VgcConstants.NicSlotRemoteClosed;
            return status;
        }

        public void Enqueue(byte[] data)
        {
            _receiveQueue.Enqueue(data);

            // Drop oldest if over max depth
            while (_receiveQueue.Count > VgcConstants.NicMaxQueueDepth)
            {
                _receiveQueue.TryDequeue(out _);
                _error = true;
            }
        }

        public bool TryDequeue(out byte[]? data) => _receiveQueue.TryDequeue(out data);

        public void Connect(TcpClient client)
        {
            _client = client;
            _stream = client.GetStream();
            _connected = true;
            _error = false;
            _remoteClosed = false;
        }

        public void SetError() => _error = true;

        public void StartReading(Action<int> onMessageReceived, int slotIndex)
        {
            _cts = new CancellationTokenSource();
            var token = _cts.Token;
            _readTask = Task.Run(async () =>
            {
                try
                {
                    while (!token.IsCancellationRequested && _stream != null)
                    {
                        int lenByte = await _stream.ReadByteAsync(token);
                        if (lenByte < 0) break; // stream closed

                        int payloadLen = lenByte == 0 ? 256 : lenByte;
                        var buf = new byte[payloadLen];
                        int read = 0;
                        while (read < payloadLen)
                        {
                            int n = await _stream.ReadAsync(buf, read, payloadLen - read, token);
                            if (n == 0) break;
                            read += n;
                        }
                        if (read < payloadLen) break;

                        Enqueue(buf);
                        onMessageReceived(slotIndex);
                    }
                }
                catch (IOException) { }
                catch (OperationCanceledException) { }

                _remoteClosed = true;
                _connected = false;
            }, token);
        }

        public void Send(byte[] data)
        {
            if (!_connected || _stream == null)
            {
                _error = true;
                return;
            }

            try
            {
                byte lenByte = (byte)(data.Length == 256 ? 0 : data.Length);
                _stream.WriteByte(lenByte);
                _stream.Write(data, 0, data.Length);
                _stream.Flush();
            }
            catch (IOException)
            {
                _error = true;
                _remoteClosed = true;
                _connected = false;
            }
        }

        public void StartListening(int port)
        {
            _listener = new TcpListener(IPAddress.Loopback, port);
            _listener.Start();

            _cts = new CancellationTokenSource();
            _ = AcceptClientAsync(_cts.Token);
        }

        private async Task AcceptClientAsync(CancellationToken token)
        {
            try
            {
                var client = await _listener!.AcceptTcpClientAsync(token);
                _client = client;
                _stream = client.GetStream();
                _pendingAccept = true;
            }
            catch (OperationCanceledException) { }
            catch (ObjectDisposedException) { }
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
            _cts?.Cancel();

            try { _readTask?.Wait(TimeSpan.FromMilliseconds(500)); }
            catch { /* ignore */ }

            _stream?.Dispose();
            _client?.Dispose();
            _listener?.Stop();
            _listener = null;
            _cts?.Dispose();
            _cts = null;
            _readTask = null;
            _stream = null;
            _client = null;

            // Drain queue
            while (_receiveQueue.TryDequeue(out _)) { }

            _connected = false;
            _error = false;
            _remoteClosed = false;
            _pendingAccept = false;
        }

        public void Dispose() => Reset();
    }
}

// -------------------------------------------------------------------------
// NetworkStream extension
// -------------------------------------------------------------------------

internal static class NetworkStreamExtensions
{
    public static async Task<int> ReadByteAsync(this NetworkStream stream, CancellationToken token)
    {
        var buf = new byte[1];
        int n = await stream.ReadAsync(buf, 0, 1, token);
        return n == 0 ? -1 : buf[0];
    }
}

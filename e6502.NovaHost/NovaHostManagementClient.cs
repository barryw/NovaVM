using System.Buffers.Binary;
using System.Net.Sockets;
using System.Text;
using System.Text.Json;
using System.Text.Json.Nodes;

namespace e6502.NovaHost;

public sealed class NovaHostManagementClient : IDisposable
{
    private const ushort Version = 1;
    private const int HeaderBytes = 24;
    private const int DefaultPort = 6504;
    private const int ChunkBytes = 512;
    private static readonly TimeSpan DefaultCommandTimeout = TimeSpan.FromSeconds(60);

    private enum Command : ushort
    {
        Hello = 1,
        GetStatus = 2,
        ListDirectory = 3,
        ReadFile = 4,
        WriteFileBegin = 5,
        WriteFileChunk = 6,
        WriteFileCommit = 7,
        WriteFileAbort = 8,
        DeletePath = 9,
        MountDrive = 10,
        UnmountDrive = 11,
        SetRuntimeConfig = 12,
        AudioStop = 13,
        VmReset = 14,
        HostReboot = 15,
        WifiScan = 16,
        WifiConfig = 17,
        WifiAction = 18,
        ReadFileChunk = 19
    }

    private readonly string _host;
    private readonly int _port;
    private readonly SemaphoreSlim _gate = new(1, 1);
    private TcpClient? _client;
    private NetworkStream? _stream;
    private uint _nextRequestId = 1;
    private JsonObject? _hello;
    private string? _lastError;

    public NovaHostManagementClient(string boardHost, int port = DefaultPort)
    {
        Uri uri = boardHost.StartsWith("http://", StringComparison.OrdinalIgnoreCase) ||
            boardHost.StartsWith("https://", StringComparison.OrdinalIgnoreCase)
            ? new Uri(boardHost)
            : new Uri($"tcp://{boardHost}");
        _host = uri.Host;
        _port = port;
    }

    public JsonObject ConnectionState
    {
        get
        {
            return new JsonObject
            {
                ["protocol"] = "tcp+cbor",
                ["host"] = _host,
                ["port"] = _port,
                ["connected"] = _client?.Connected == true,
                ["lastError"] = _lastError ?? "",
                ["hello"] = _hello?.DeepClone()
            };
        }
    }

    public async Task<JsonObject> GetStatusAsync(CancellationToken token)
    {
        JsonObject data = await SendAsync(Command.GetStatus, null, null, token);
        data["management"] = ConnectionState;
        if (data["audioJson"] is JsonValue audioJson &&
            audioJson.TryGetValue(out string? audioText))
        {
            data["audio"] = ParseJsonOrError(audioText);
            data.Remove("audioJson");
        }
        if (data["bootConfigJson"] is JsonValue bootJson &&
            bootJson.TryGetValue(out string? bootText))
        {
            data["bootConfig"] = string.IsNullOrWhiteSpace(bootText)
                ? DefaultBootConfig()
                : ParseBootConfig(bootText);
            data.Remove("bootConfigJson");
        }
        return data;
    }

    public Task<JsonArray> ListDirectoryAsync(string path, CancellationToken token) =>
        SendArrayAsync(Command.ListDirectory, new Dictionary<string, object?>
        {
            ["path"] = NormalizeSdPath(path)
        }, token);

    public async Task<byte[]> ReadFileAsync(string path, CancellationToken token)
    {
        using var output = new MemoryStream();
        await DownloadFileAsync(path, output, null, token);
        return output.ToArray();
    }

    public async Task<string> ReadTextFileAsync(string path, CancellationToken token) =>
        Encoding.UTF8.GetString(await ReadFileAsync(path, token));

    public async Task<long> DownloadFileAsync(string path, Stream output,
                                              IProgress<NovaTransferProgress>? progress,
                                              CancellationToken token)
    {
        string normalized = NormalizeSdPath(path);
        uint offset = 0;
        long total = 0;

        while (true)
        {
            ManagementResponse response = await SendRawAsync(Command.ReadFileChunk,
                new Dictionary<string, object?>
                {
                    ["path"] = normalized,
                    ["offset"] = offset,
                    ["length"] = (uint)ChunkBytes
                }, null, token);
            if (response.Raw.Length > 0)
                await output.WriteAsync(response.Raw, token);
            offset += (uint)response.Raw.Length;
            total = response.Data["size"]?.GetValue<long>() ?? total;
            progress?.Report(new NovaTransferProgress(offset, total));

            bool eof = response.Data["eof"]?.GetValue<bool>() == true;
            if (eof)
                return total;
            if (response.Raw.Length == 0)
                throw new EndOfStreamException("management download made no progress");
        }
    }

    public async Task<NovaFileChunk> ReadFileChunkAsync(string path, uint offset,
                                                        uint length,
                                                        CancellationToken token)
    {
        ManagementResponse response = await SendRawAsync(Command.ReadFileChunk,
            new Dictionary<string, object?>
            {
                ["path"] = NormalizeSdPath(path),
                ["offset"] = offset,
                ["length"] = length
            }, null, token);

        long size = response.Data["size"]?.GetValue<long>() ?? 0;
        bool eof = response.Data["eof"]?.GetValue<bool>() == true;
        return new NovaFileChunk(response.Raw, size, eof);
    }

    public async Task WriteFileAsync(string path, Stream input, long length,
                                     IProgress<long>? progress,
                                     CancellationToken token)
    {
        if (length < 0 || length > uint.MaxValue)
            throw new InvalidOperationException("upload size is unknown or too large");

        string normalized = NormalizeSdPath(path);
        await SendAsync(Command.WriteFileBegin, new Dictionary<string, object?>
        {
            ["path"] = normalized,
            ["size"] = (uint)length
        }, null, token);

        byte[] buffer = new byte[ChunkBytes];
        long offset = 0;
        try
        {
            while (offset < length)
            {
                int want = (int)Math.Min(buffer.Length, length - offset);
                int got = await input.ReadAsync(buffer.AsMemory(0, want), token);
                if (got <= 0)
                    throw new EndOfStreamException("upload stream ended early");
                byte[] raw = got == buffer.Length ? buffer : buffer[..got];
                await SendAsync(Command.WriteFileChunk, new Dictionary<string, object?>
                {
                    ["offset"] = (uint)offset
                }, raw, token);
                offset += got;
                progress?.Report(offset);
            }

            await SendAsync(Command.WriteFileCommit, null, null, token);
        }
        catch
        {
            try { await SendAsync(Command.WriteFileAbort, null, null, CancellationToken.None); }
            catch { }
            throw;
        }
    }

    public Task<JsonObject> DeletePathAsync(string path, CancellationToken token) =>
        SendAsync(Command.DeletePath, new Dictionary<string, object?>
        {
            ["path"] = NormalizeSdPath(path)
        }, null, token);

    public Task<JsonObject> MountDriveAsync(string slot, string? path,
                                            CancellationToken token)
    {
        var payload = new Dictionary<string, object?> { ["slot"] = slot };
        if (!string.IsNullOrWhiteSpace(path))
            payload["path"] = NormalizeSdPath(path);
        return SendAsync(Command.MountDrive, payload, null, token);
    }

    public Task<JsonObject> UnmountDriveAsync(string slot, CancellationToken token) =>
        SendAsync(Command.UnmountDrive, new Dictionary<string, object?>
        {
            ["slot"] = slot
        }, null, token);

    public Task<JsonObject> RuntimeSetAsync(string name, CancellationToken token) =>
        SendAsync(Command.SetRuntimeConfig, new Dictionary<string, object?>
        {
            ["action"] = "set",
            ["name"] = name
        }, null, token);

    public Task<JsonObject> RuntimeAddAsync(string name, string rom,
                                            string? extensionRom,
                                            bool makeActive,
                                            CancellationToken token) =>
        SendAsync(Command.SetRuntimeConfig, new Dictionary<string, object?>
        {
            ["action"] = "add",
            ["name"] = name,
            ["rom"] = NormalizeSdPath(rom),
            ["extensionRom"] = string.IsNullOrWhiteSpace(extensionRom)
                ? null
                : NormalizeSdPath(extensionRom),
            ["makeActive"] = makeActive
        }, null, token);

    public Task<JsonObject> RuntimeRemoveAsync(string name, CancellationToken token) =>
        SendAsync(Command.SetRuntimeConfig, new Dictionary<string, object?>
        {
            ["action"] = "remove",
            ["name"] = name
        }, null, token);

    public Task<JsonObject> AudioStopAsync(CancellationToken token) =>
        SendAsync(Command.AudioStop, null, null, token);

    public Task<JsonObject> VmResetAsync(CancellationToken token) =>
        SendAsync(Command.VmReset, null, null, token);

    public Task<JsonObject> HostRebootAsync(CancellationToken token) =>
        SendAsync(Command.HostReboot, null, null, token);

    public async Task<JsonObject> WifiStatusAsync(CancellationToken token)
    {
        JsonObject status = await GetStatusAsync(token);
        return status["wifi"] is JsonObject wifi
            ? wifi.DeepClone().AsObject()
            : new JsonObject();
    }

    public Task<JsonArray> WifiScanAsync(CancellationToken token) =>
        SendArrayAsync(Command.WifiScan, new Dictionary<string, object?>(), token);

    public Task<JsonObject> WifiConfigAsync(Dictionary<string, object?> settings,
                                            CancellationToken token) =>
        SendAsync(Command.WifiConfig, settings, null, token);

    public Task<JsonObject> WifiActionAsync(string action, CancellationToken token) =>
        SendAsync(Command.WifiAction, new Dictionary<string, object?>
        {
            ["action"] = action
        }, null, token);

    private async Task<JsonArray> SendArrayAsync(Command command,
                                                 Dictionary<string, object?> payload,
                                                 CancellationToken token)
    {
        JsonObject data = await SendAsync(command, payload, null, token);
        return data["value"] as JsonArray ?? new JsonArray();
    }

    private async Task<JsonObject> SendAsync(Command command,
                                             Dictionary<string, object?>? payload,
                                             byte[]? raw,
                                             CancellationToken token)
    {
        ManagementResponse response = await SendRawAsync(command, payload, raw, token);
        return response.Data;
    }

    private async Task<ManagementResponse> SendRawAsync(Command command,
                                                        Dictionary<string, object?>? payload,
                                                        byte[]? raw,
                                                        CancellationToken token)
    {
        using var timeout = CancellationTokenSource.CreateLinkedTokenSource(token);
        timeout.CancelAfter(DefaultCommandTimeout);
        CancellationToken commandToken = timeout.Token;
        bool gateEntered = false;
        try
        {
            await _gate.WaitAsync(commandToken);
            gateEntered = true;

            await EnsureConnectedAsync(commandToken);
            if (_hello is null && command != Command.Hello)
                _hello = (await SendNoLockAsync(Command.Hello, null, null, commandToken)).Data;
            return await SendNoLockAsync(command, payload, raw, commandToken);
        }
        catch (OperationCanceledException ex) when (!token.IsCancellationRequested)
        {
            _lastError = $"management command timed out after {DefaultCommandTimeout.TotalSeconds:0} seconds";
            CloseSocket();
            throw new TimeoutException(_lastError, ex);
        }
        catch (Exception ex) when (ex is IOException or SocketException or InvalidOperationException)
        {
            _lastError = ex.Message;
            CloseSocket();
            throw;
        }
        finally
        {
            if (gateEntered)
                _gate.Release();
        }
    }

    private async Task EnsureConnectedAsync(CancellationToken token)
    {
        if (_client?.Connected == true && _stream is not null)
            return;

        CloseSocket();
        _client = new TcpClient { NoDelay = true };
        await _client.ConnectAsync(_host, _port, token);
        _stream = _client.GetStream();
        _lastError = null;
    }

    private async Task<ManagementResponse> SendNoLockAsync(Command command,
                                                           Dictionary<string, object?>? payload,
                                                           byte[]? raw,
                                                           CancellationToken token)
    {
        if (_stream is null)
            throw new InvalidOperationException("management socket is not connected");

        uint requestId = _nextRequestId++;
        byte[] cbor = payload is null ? Array.Empty<byte>() : CborLite.EncodeMap(payload);
        byte[] header = new byte[HeaderBytes];
        Encoding.ASCII.GetBytes("NVH1").CopyTo(header, 0);
        BinaryPrimitives.WriteUInt16BigEndian(header.AsSpan(4), Version);
        BinaryPrimitives.WriteUInt16BigEndian(header.AsSpan(6), (ushort)command);
        BinaryPrimitives.WriteUInt16BigEndian(header.AsSpan(8), 0);
        BinaryPrimitives.WriteUInt32BigEndian(header.AsSpan(10), requestId);
        BinaryPrimitives.WriteUInt32BigEndian(header.AsSpan(14), (uint)cbor.Length);
        BinaryPrimitives.WriteUInt32BigEndian(header.AsSpan(18), (uint)(raw?.Length ?? 0));

        await _stream.WriteAsync(header, token);
        if (cbor.Length > 0)
            await _stream.WriteAsync(cbor, token);
        if (raw is { Length: > 0 })
            await _stream.WriteAsync(raw, token);

        ManagementResponse response = await ReadResponseAsync(token);
        if (response.RequestId != requestId)
            throw new InvalidOperationException("management response request id mismatch");
        if (!response.Ok)
            throw new NovaHostCommandException(response.Error ?? response.Code ?? "management command failed");
        return response;
    }

    private async Task<ManagementResponse> ReadResponseAsync(CancellationToken token)
    {
        if (_stream is null)
            throw new InvalidOperationException("management socket is not connected");

        byte[] header = new byte[HeaderBytes];
        await ReadExactAsync(_stream, header, token);
        if (Encoding.ASCII.GetString(header, 0, 4) != "NVH1")
            throw new InvalidOperationException("bad management frame magic");
        if (BinaryPrimitives.ReadUInt16BigEndian(header.AsSpan(4)) != Version)
            throw new InvalidOperationException("bad management protocol version");

        ushort command = BinaryPrimitives.ReadUInt16BigEndian(header.AsSpan(6));
        uint requestId = BinaryPrimitives.ReadUInt32BigEndian(header.AsSpan(10));
        uint payloadLength = BinaryPrimitives.ReadUInt32BigEndian(header.AsSpan(14));
        uint rawLength = BinaryPrimitives.ReadUInt32BigEndian(header.AsSpan(18));
        if (payloadLength > 1_048_576 || rawLength > 16_777_216)
            throw new InvalidOperationException("management frame too large");

        byte[] payload = new byte[payloadLength];
        byte[] raw = new byte[rawLength];
        if (payload.Length > 0)
            await ReadExactAsync(_stream, payload, token);
        if (raw.Length > 0)
            await ReadExactAsync(_stream, raw, token);

        JsonObject envelope = payload.Length == 0
            ? new JsonObject()
            : CborLite.Decode(payload).AsObject();
        bool ok = envelope["ok"]?.GetValue<bool>() == true;
        JsonObject data = envelope["data"] is JsonObject obj
            ? obj.DeepClone().AsObject()
            : new JsonObject();
        if (envelope["data"] is JsonArray array)
            data = new JsonObject { ["value"] = array.DeepClone() };
        return new ManagementResponse(command, requestId, ok,
            envelope["code"]?.GetValue<string>(),
            envelope["error"]?.GetValue<string>(), data, raw);
    }

    private static async Task ReadExactAsync(Stream stream, byte[] buffer,
                                             CancellationToken token)
    {
        int offset = 0;
        while (offset < buffer.Length)
        {
            int got = await stream.ReadAsync(buffer.AsMemory(offset), token);
            if (got <= 0)
                throw new EndOfStreamException("management socket closed");
            offset += got;
        }
    }

    private static string NormalizeSdPath(string path)
    {
        string clean = path.Trim().Replace('\\', '/');
        while (clean.StartsWith('/'))
            clean = clean[1..];
        if (clean.StartsWith("sd/", StringComparison.OrdinalIgnoreCase))
            clean = clean[3..];
        return "/" + clean;
    }

    private static JsonNode ParseJsonOrError(string text)
    {
        try { return JsonNode.Parse(text) ?? new JsonObject(); }
        catch (Exception ex) { return new JsonObject { ["ok"] = false, ["error"] = ex.Message }; }
    }

    private static JsonObject ParseBootConfig(string text)
    {
        try
        {
            return MergeBootConfigDefaults(JsonNode.Parse(text) as JsonObject);
        }
        catch
        {
            return DefaultBootConfig();
        }
    }

    private static JsonObject DefaultBootConfig() => JsonNode.Parse("""
    {
      "vm": { "defaultRuntime": "novabasic" },
      "languages": {
        "novabasic": {
          "rom": "/roms/novabasic.bin",
          "extensionRom": "/roms/extension.bin",
          "autoboot": true
        }
      },
      "mounts": {},
      "network": {}
    }
    """)!.AsObject();

    private static JsonObject MergeBootConfigDefaults(JsonObject? config)
    {
        JsonObject merged = DefaultBootConfig();
        if (config is null)
            return merged;

        if (config["mounts"] is JsonObject mounts)
            merged["mounts"] = mounts.DeepClone();
        if (config["network"] is JsonObject network)
            merged["network"] = network.DeepClone();

        JsonObject mergedLanguages = merged["languages"]!.AsObject();
        if (config["languages"] is JsonObject languages)
        {
            foreach ((string name, JsonNode? entry) in languages)
            {
                if (entry is not null)
                    mergedLanguages[name] = entry.DeepClone();
            }
        }

        JsonObject mergedVm = merged["vm"]!.AsObject();
        if (config["vm"] is JsonObject vm)
        {
            foreach ((string key, JsonNode? value) in vm)
            {
                if (value is not null)
                    mergedVm[key] = value.DeepClone();
            }
        }

        string active = mergedVm["defaultRuntime"]?.GetValue<string>() ?? "";
        if (active.Length == 0 || !mergedLanguages.ContainsKey(active))
            mergedVm["defaultRuntime"] = mergedLanguages.ContainsKey("novabasic")
                ? "novabasic"
                : mergedLanguages.First().Key;
        return merged;
    }

    private void CloseSocket()
    {
        try { _stream?.Dispose(); } catch { }
        try { _client?.Dispose(); } catch { }
        _stream = null;
        _client = null;
        _hello = null;
    }

    public void Dispose()
    {
        CloseSocket();
        _gate.Dispose();
    }

    private sealed record ManagementResponse(ushort Command, uint RequestId,
        bool Ok, string? Code, string? Error, JsonObject Data, byte[] Raw);
}

public sealed class NovaHostCommandException : Exception
{
    public NovaHostCommandException(string message) : base(message) {}
}

public sealed record NovaTransferProgress(long Done, long Total);

public sealed record NovaFileChunk(byte[] Data, long Size, bool Eof);

static class CborLite
{
    public static byte[] EncodeMap(Dictionary<string, object?> map)
    {
        using var ms = new MemoryStream();
        WriteMapHeader(ms, map.Count(kv => kv.Value is not null));
        foreach ((string key, object? value) in map)
        {
            if (value is null)
                continue;
            WriteText(ms, key);
            WriteValue(ms, value);
        }
        return ms.ToArray();
    }

    public static JsonNode Decode(byte[] data)
    {
        var reader = new Reader(data);
        return reader.ReadValue();
    }

    private static void WriteValue(Stream stream, object value)
    {
        switch (value)
        {
            case string text:
                WriteText(stream, text);
                break;
            case bool flag:
                stream.WriteByte((byte)(flag ? 0xF5 : 0xF4));
                break;
            case byte[] bytes:
                WriteType(stream, 2, (ulong)bytes.Length);
                stream.Write(bytes);
                break;
            case int i:
                WriteInt(stream, i);
                break;
            case uint u:
                WriteType(stream, 0, u);
                break;
            case long l:
                WriteInt(stream, l);
                break;
            default:
                throw new InvalidOperationException($"unsupported CBOR value: {value.GetType().Name}");
        }
    }

    private static void WriteInt(Stream stream, long value)
    {
        if (value >= 0)
            WriteType(stream, 0, (ulong)value);
        else
            WriteType(stream, 1, (ulong)(-1 - value));
    }

    private static void WriteMapHeader(Stream stream, int count) =>
        WriteType(stream, 5, (ulong)count);

    private static void WriteText(Stream stream, string text)
    {
        byte[] bytes = Encoding.UTF8.GetBytes(text);
        WriteType(stream, 3, (ulong)bytes.Length);
        stream.Write(bytes);
    }

    private static void WriteType(Stream stream, int major, ulong value)
    {
        int prefix = major << 5;
        if (value < 24)
        {
            stream.WriteByte((byte)(prefix | (int)value));
        }
        else if (value <= byte.MaxValue)
        {
            stream.WriteByte((byte)(prefix | 24));
            stream.WriteByte((byte)value);
        }
        else if (value <= ushort.MaxValue)
        {
            Span<byte> b = stackalloc byte[2];
            stream.WriteByte((byte)(prefix | 25));
            BinaryPrimitives.WriteUInt16BigEndian(b, (ushort)value);
            stream.Write(b);
        }
        else if (value <= uint.MaxValue)
        {
            Span<byte> b = stackalloc byte[4];
            stream.WriteByte((byte)(prefix | 26));
            BinaryPrimitives.WriteUInt32BigEndian(b, (uint)value);
            stream.Write(b);
        }
        else
        {
            Span<byte> b = stackalloc byte[8];
            stream.WriteByte((byte)(prefix | 27));
            BinaryPrimitives.WriteUInt64BigEndian(b, value);
            stream.Write(b);
        }
    }

    private sealed class Reader
    {
        private readonly byte[] _data;
        private int _pos;

        public Reader(byte[] data) => _data = data;

        public JsonNode ReadValue()
        {
            byte initial = ReadByte();
            int major = initial >> 5;
            int add = initial & 0x1F;
            ulong value = ReadArgument(add);
            return major switch
            {
                0 => JsonValue.Create((long)value)!,
                1 => JsonValue.Create(-1L - (long)value)!,
                2 => JsonValue.Create(Convert.ToBase64String(ReadBytes((int)value)))!,
                3 => JsonValue.Create(Encoding.UTF8.GetString(ReadBytes((int)value)))!,
                4 => ReadArray((int)value),
                5 => ReadMap((int)value),
                7 => ReadSimple(add),
                _ => throw new InvalidOperationException("unsupported CBOR major type")
            };
        }

        private JsonArray ReadArray(int count)
        {
            var array = new JsonArray();
            for (int i = 0; i < count; i++)
                array.Add(ReadValue());
            return array;
        }

        private JsonObject ReadMap(int count)
        {
            var map = new JsonObject();
            for (int i = 0; i < count; i++)
            {
                string key = ReadValue().GetValue<string>();
                map[key] = ReadValue();
            }
            return map;
        }

        private static JsonNode ReadSimple(int add) => add switch
        {
            20 => JsonValue.Create(false)!,
            21 => JsonValue.Create(true)!,
            22 => null!,
            _ => throw new InvalidOperationException("unsupported CBOR simple value")
        };

        private ulong ReadArgument(int add)
        {
            return add switch
            {
                < 24 => (ulong)add,
                24 => ReadByte(),
                25 => BinaryPrimitives.ReadUInt16BigEndian(ReadBytes(2)),
                26 => BinaryPrimitives.ReadUInt32BigEndian(ReadBytes(4)),
                27 => BinaryPrimitives.ReadUInt64BigEndian(ReadBytes(8)),
                _ => throw new InvalidOperationException("unsupported CBOR length")
            };
        }

        private byte ReadByte()
        {
            if (_pos >= _data.Length)
                throw new EndOfStreamException("CBOR payload ended early");
            return _data[_pos++];
        }

        private ReadOnlySpan<byte> ReadBytes(int count)
        {
            if (_pos + count > _data.Length)
                throw new EndOfStreamException("CBOR payload ended early");
            ReadOnlySpan<byte> span = _data.AsSpan(_pos, count);
            _pos += count;
            return span;
        }
    }
}

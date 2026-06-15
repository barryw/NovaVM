using System.Net.Sockets;
using System.Text;
using System.Text.Json.Nodes;
using System.Text.Json;

namespace e6502.NovaHost;

/// Newline-delimited JSON client for the NovaHost debug port (6503 remote, 6502 local).
/// Used for keyboard input: type_text and send_key.
public sealed class NovaDebugClient : IDisposable
{
    private static readonly UTF8Encoding Utf8NoBom = new(false);
    private static readonly JsonSerializerOptions JsonOpts = new();

    private readonly string _host;
    private readonly int _port;
    private readonly SemaphoreSlim _sem = new(1, 1);
    private TcpClient? _tcp;
    private StreamReader? _reader;
    private StreamWriter? _writer;

    public NovaDebugClient(string host, int port = 6503)
    {
        _host = host;
        _port = port;
    }

    public bool IsConnected => _tcp is { Connected: true };

    private async Task EnsureConnectedAsync(CancellationToken ct)
    {
        if (_tcp is { Connected: true }) return;
        _tcp?.Dispose();
        _tcp = new TcpClient();
        await _tcp.ConnectAsync(_host, _port, ct);
        NetworkStream stream = _tcp.GetStream();
        _reader = new StreamReader(stream, Utf8NoBom, false);
        _writer = new StreamWriter(stream, Utf8NoBom) { AutoFlush = true, NewLine = "\n" };
    }

    public Task SendKeyAsync(string key, CancellationToken ct = default) =>
        SendAsync(new JsonObject { ["command"] = "send_key", ["key"] = key }, ct);

    public Task TypeTextAsync(string text, CancellationToken ct = default) =>
        SendAsync(new JsonObject { ["command"] = "type_text", ["text"] = text }, ct);

    private async Task SendAsync(JsonObject request, CancellationToken ct)
    {
        await _sem.WaitAsync(ct);
        try
        {
            await EnsureConnectedAsync(ct);
            await _writer!.WriteLineAsync(request.ToJsonString(JsonOpts));
            await _reader!.ReadLineAsync(ct); // drain the {"ok":true} reply
        }
        finally
        {
            _sem.Release();
        }
    }

    public void Dispose()
    {
        _tcp?.Dispose();
        _sem.Dispose();
    }
}

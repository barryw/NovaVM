using System.Net.Sockets;
using System.Text;
using System.Text.Json.Nodes;

namespace e6502.MCP;

public sealed class EmulatorClient : IDisposable
{
    private readonly string _host;
    private readonly int _port;
    private readonly SemaphoreSlim _sem = new(1, 1);
    private TcpClient? _tcp;
    private StreamReader? _reader;
    private StreamWriter? _writer;

    public EmulatorClient(string host = "127.0.0.1", int port = 6502)
    {
        _host = host;
        _port = port;
    }

    private async Task EnsureConnectedAsync()
    {
        if (_tcp is { Connected: true }) return;

        _tcp?.Dispose();
        _tcp = new TcpClient();
        await _tcp.ConnectAsync(_host, _port);
        var stream = _tcp.GetStream();
        _reader = new StreamReader(stream, Encoding.UTF8);
        _writer = new StreamWriter(stream, Encoding.UTF8) { AutoFlush = true };
    }

    public async Task<JsonNode> SendAsync(JsonObject request)
    {
        await _sem.WaitAsync();
        try
        {
            await EnsureConnectedAsync();
            await _writer!.WriteLineAsync(request.ToJsonString());
            var line = await _reader!.ReadLineAsync();
            return JsonNode.Parse(line ?? "{}") ?? new JsonObject();
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

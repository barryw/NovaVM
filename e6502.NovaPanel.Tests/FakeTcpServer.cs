using System.Net;
using System.Net.Sockets;
using System.Text;

namespace e6502.NovaPanel.Tests;

/// Minimal loopback server: accepts one client, records each received line,
/// and replies with a fixed line per request.
public sealed class FakeTcpServer : IAsyncDisposable
{
    private readonly TcpListener _listener;
    private readonly string _reply;
    public List<string> Received { get; } = new();
    public int Port => ((IPEndPoint)_listener.LocalEndpoint).Port;

    public FakeTcpServer(string reply = "{\"ok\":true}")
    {
        _reply = reply;
        _listener = new TcpListener(IPAddress.Loopback, 0);
        _listener.Start();
        _ = AcceptLoop();
    }

    private async Task AcceptLoop()
    {
        try
        {
            using TcpClient client = await _listener.AcceptTcpClientAsync();
            using NetworkStream s = client.GetStream();
            using var reader = new StreamReader(s, new UTF8Encoding(false));
            using var writer = new StreamWriter(s, new UTF8Encoding(false)) { AutoFlush = true, NewLine = "\n" };
            string? line;
            while ((line = await reader.ReadLineAsync()) is not null)
            {
                Received.Add(line);
                await writer.WriteLineAsync(_reply);
            }
        }
        catch { /* server torn down */ }
    }

    public ValueTask DisposeAsync()
    {
        _listener.Stop();
        return ValueTask.CompletedTask;
    }
}

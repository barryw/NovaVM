using e6502.NovaHost;

namespace e6502.NovaPanel.Services;

/// Bridges the on-screen keyboard's tokens to the NovaHost debug socket.
public sealed class DebugKeySink : IKeySink, IDisposable
{
    private readonly NovaDebugClient _client;
    public DebugKeySink(string host, int port = 6503) => _client = new NovaDebugClient(host, port);
    public Task SendKeyAsync(string token, CancellationToken ct) => _client.SendKeyAsync(token, ct);
    public void Dispose() => _client.Dispose();
}

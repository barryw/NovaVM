using e6502.NovaHost;
using Xunit;

namespace e6502.NovaPanel.Tests;

public class NovaDebugClientTests
{
    [Fact]
    public async Task SendKey_writes_exact_send_key_frame()
    {
        await using var server = new FakeTcpServer();
        using var client = new NovaDebugClient("127.0.0.1", server.Port);

        await client.SendKeyAsync("ENTER");

        Assert.Single(server.Received);
        Assert.Equal("{\"command\":\"send_key\",\"key\":\"ENTER\"}", server.Received[0]);
    }

    [Fact]
    public async Task TypeText_writes_exact_type_text_frame()
    {
        await using var server = new FakeTcpServer();
        using var client = new NovaDebugClient("127.0.0.1", server.Port);

        await client.TypeTextAsync("hi");

        Assert.Equal("{\"command\":\"type_text\",\"text\":\"hi\"}", server.Received[0]);
    }
}

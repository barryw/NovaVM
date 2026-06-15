using System.Text.Json.Nodes;
using e6502.NovaPanel.Services;
using Xunit;

namespace e6502.NovaPanel.Tests;

public class NovaConnectionTests
{
    private static JsonObject SampleStatus() => new()
    {
        ["drives"] = new JsonArray(),
        ["bootConfig"] = new JsonObject { ["vm"] = new JsonObject { ["defaultRuntime"] = "novabasic" }, ["languages"] = new JsonObject() }
    };

    [Fact]
    public async Task Successful_poll_transitions_connecting_then_connected()
    {
        var states = new List<ConnectionState>();
        var conn = new NovaConnection(_ => Task.FromResult(SampleStatus()));
        conn.StateChanged += states.Add;

        NovaStatus? pushed = null;
        conn.StatusUpdated += s => pushed = s;

        await conn.PollOnceAsync();

        Assert.Equal(new[] { ConnectionState.Connecting, ConnectionState.Connected }, states);
        Assert.Equal(ConnectionState.Connected, conn.State);
        Assert.NotNull(pushed);
        Assert.Equal("novabasic", conn.LastStatus.ActiveRuntime);
    }

    [Fact]
    public async Task Failing_poll_transitions_to_error()
    {
        var states = new List<ConnectionState>();
        var conn = new NovaConnection(_ => throw new InvalidOperationException("boom"));
        conn.StateChanged += states.Add;

        await conn.PollOnceAsync();

        Assert.Equal(new[] { ConnectionState.Connecting, ConnectionState.Error }, states);
        Assert.Equal(ConnectionState.Error, conn.State);
        Assert.Equal("boom", conn.LastError);
    }

    [Fact]
    public async Task Action_without_management_client_returns_not_connected()
    {
        var conn = new NovaConnection(_ => Task.FromResult(SampleStatus()));
        (bool ok, string? error) = await conn.VmResetAsync();
        Assert.False(ok);
        Assert.Equal("not connected", error);
    }
}

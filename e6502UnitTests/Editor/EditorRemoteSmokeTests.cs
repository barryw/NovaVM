using System;
using System.Linq;
using System.Net;
using System.Text.Json.Nodes;
using System.Threading.Tasks;
using e6502.MCP;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests.Editor;

/// <summary>
/// Opt-in live smoke for the standalone editor demo over the shared VM debug
/// protocol. The same test targets Avalonia (6502) or hardware (6503).
/// </summary>
[TestClass]
public class EditorRemoteSmokeTests
{
    private EmulatorClient _client = null!;

    [TestInitialize]
    public void Init()
    {
        string? host = Environment.GetEnvironmentVariable("NOVA_EDITOR_SMOKE_HOST");
        if (string.IsNullOrWhiteSpace(host))
            Assert.Inconclusive("Set NOVA_EDITOR_SMOKE_HOST to 127.0.0.1 for Avalonia or the board IP for hardware.");

        int port = int.TryParse(Environment.GetEnvironmentVariable("NOVA_EDITOR_SMOKE_PORT"), out int p)
            ? p
            : DefaultDebugPort(host!);

        _client = new EmulatorClient(host!, port);
    }

    [TestCleanup]
    public void Cleanup() => _client?.Dispose();

    [TestMethod]
    [TestCategory("LiveEditor")]
    public async Task EditorDemo_FindAndOpenDialogsWorkOverDebugProtocol()
    {
        string screen = await WaitForScreenAsync(
            s => s.Contains("demo/NOVAPASCAL.PAS", StringComparison.Ordinal),
            "editor demo title");
        Assert.IsTrue(screen.Contains("program Demo;", StringComparison.Ordinal), screen);

        await SendKeyAsync("CTRL-F");
        screen = await WaitForScreenAsync(
            s => s.Contains("Text:", StringComparison.Ordinal),
            "Find dialog Text field");
        Assert.IsTrue(screen.Contains("Find", StringComparison.Ordinal), screen);

        await SendKeyAsync("ESCAPE");
        screen = await WaitForScreenAsync(
            s => s.Contains("demo/NOVAPASCAL.PAS", StringComparison.Ordinal)
                 && !s.Contains("Text:", StringComparison.Ordinal),
            "editor after Find cancel");

        await SendKeyAsync("CTRL-O");
        screen = await WaitForScreenAsync(
            s => s.Contains("Filename", StringComparison.Ordinal),
            "Open dialog filename field");
        Assert.IsTrue(screen.Contains("Open", StringComparison.Ordinal), screen);

        await SendKeyAsync("TAB");
        await SendKeyAsync("ESCAPE");
        screen = await WaitForScreenAsync(
            s => s.Contains("demo/NOVAPASCAL.PAS", StringComparison.Ordinal)
                 && !s.Contains("Filename", StringComparison.Ordinal),
            "editor after Open cancel");
        Assert.IsTrue(screen.Contains("program Demo;", StringComparison.Ordinal), screen);
    }

    private static int DefaultDebugPort(string host)
    {
        if (host.Equals("localhost", StringComparison.OrdinalIgnoreCase))
            return 6502;
        return IPAddress.TryParse(host, out IPAddress? ip) && IPAddress.IsLoopback(ip) ? 6502 : 6503;
    }

    private async Task SendKeyAsync(string key)
    {
        JsonNode res = await SendAsync(new JsonObject
        {
            ["command"] = "send_key",
            ["key"] = key,
        });
        AssertOk(res, $"send_key {key}");
        await Task.Delay(100);
    }

    private async Task<string> WaitForScreenAsync(Func<string, bool> predicate, string expected)
    {
        string screen = "";
        for (int i = 0; i < 50; i++)
        {
            screen = await ReadScreenAsync();
            if (predicate(screen))
                return screen;
            await Task.Delay(100);
        }

        Assert.Fail($"Timed out waiting for {expected}.\n{screen}");
        return screen;
    }

    private async Task<string> ReadScreenAsync()
    {
        JsonNode res = await SendAsync(new JsonObject { ["command"] = "read_screen" });
        AssertOk(res, "read_screen");
        var lines = (JsonArray)res["lines"]!;
        return string.Join('\n', lines.Select(l => l?.GetValue<string>() ?? ""));
    }

    private async Task<JsonNode> SendAsync(JsonObject req) => await _client.SendAsync(req);

    private static void AssertOk(JsonNode res, string command) =>
        Assert.IsTrue(res["ok"]?.GetValue<bool>() == true, $"{command} failed: {res}");
}

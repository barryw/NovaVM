using System;
using System.Text.Json.Nodes;
using System.Threading.Tasks;
using e6502.Avalonia.Hardware;
using e6502.MCP;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace e6502UnitTests;

/// <summary>
/// Integration tests that exercise the live NovaHost ESP32 firmware talking
/// to a running FPGA. Skipped unless NOVAHOST env var is set to the hostname
/// (e.g. "novahost.local").
/// Port defaults to 6503 or can be overridden via NOVAHOST_PORT.
/// </summary>
[TestClass]
public class NovaHostHardwareTests
{
    private EmulatorClient _client = null!;

    [TestInitialize]
    public void Init()
    {
        var host = Environment.GetEnvironmentVariable("NOVAHOST");
        if (string.IsNullOrWhiteSpace(host))
        {
            Assert.Inconclusive("NOVAHOST env var not set — skipping hardware tests.");
        }
        var port = int.TryParse(Environment.GetEnvironmentVariable("NOVAHOST_PORT"), out var p) ? p : 6503;
        _client = new EmulatorClient(host!, port);
    }

    [TestCleanup]
    public void Cleanup() => _client?.Dispose();

    private async Task<JsonNode> SendAsync(string command, params (string key, JsonNode value)[] args)
    {
        var req = new JsonObject { ["command"] = command };
        foreach (var (k, v) in args) req[k] = v;
        return await _client.SendAsync(req);
    }

    private static bool Ok(JsonNode n) => n["ok"]?.GetValue<bool>() == true;

    // ── Connectivity / basic reads ──

    [TestMethod]
    public async Task DbgState_ReturnsValidCpuState()
    {
        var res = await SendAsync("dbg_state");
        Assert.IsTrue(Ok(res), $"dbg_state failed: {res}");
        Assert.IsNotNull(res["pc"]);
        Assert.IsNotNull(res["a"]);
        Assert.IsNotNull(res["x"]);
        Assert.IsNotNull(res["y"]);
        Assert.IsNotNull(res["sp"]);
    }

    [TestMethod]
    public async Task GetCursor_ReturnsCoordinatesInRange()
    {
        var res = await SendAsync("get_cursor");
        Assert.IsTrue(Ok(res));
        var x = res["x"]!.GetValue<int>();
        var y = res["y"]!.GetValue<int>();
        Assert.IsTrue(x is >= 0 and < 80, $"cursor x out of range: {x}");
        Assert.IsTrue(y is >= 0 and < VgcConstants.ScreenRows, $"cursor y out of range: {y}");
    }

    [TestMethod]
    public async Task Peek_RomAddress_ReturnsByte()
    {
        // $FFFC is the reset vector low byte — must be a valid byte
        var res = await SendAsync("peek", ("address", 0xFFFC));
        Assert.IsTrue(Ok(res), $"peek failed: {res}");
        Assert.AreEqual(0xFFFC, res["address"]!.GetValue<int>());
        var val = res["value"]!.GetValue<int>();
        Assert.IsTrue(val is >= 0 and <= 255);
    }

    [TestMethod]
    public async Task Peek_MissingAddress_ReturnsError()
    {
        var res = await SendAsync("peek");
        Assert.IsFalse(Ok(res));
        Assert.IsTrue(res["error"]?.ToString().Contains("address", StringComparison.OrdinalIgnoreCase) ?? false);
    }

    // ── Writes ──

    [TestMethod]
    public async Task PokeAndPeek_Roundtrip()
    {
        // Use zero page address that's not used by BASIC vectors
        const int addr = 0x0050;
        var pause = await SendAsync("dbg_pause");
        Assert.IsTrue(Ok(pause), $"dbg_pause failed: {pause}");

        var orig = (await SendAsync("peek", ("address", addr)))["value"]!.GetValue<int>();
        try
        {
            var pokeRes = await SendAsync("poke", ("address", addr), ("value", 0xAB));
            Assert.IsTrue(Ok(pokeRes));
            var readBack = (await SendAsync("peek", ("address", addr)))["value"]!.GetValue<int>();
            Assert.AreEqual(0xAB, readBack);
        }
        finally
        {
            await SendAsync("poke", ("address", addr), ("value", orig));
            await SendAsync("dbg_resume");
        }
    }

    [TestMethod]
    public async Task Poke_MissingValue_ReturnsError()
    {
        var res = await SendAsync("poke", ("address", 0x50));
        Assert.IsFalse(Ok(res));
    }

    // ── Screen ──

    [TestMethod]
    public async Task ReadScreen_Returns50Lines()
    {
        var res = await SendAsync("read_screen");
        Assert.IsTrue(Ok(res));
        var lines = (JsonArray)res["lines"]!;
        Assert.AreEqual(VgcConstants.ScreenRows, lines.Count);
    }

    [TestMethod]
    public async Task ReadLine_DefaultRowUsesCursor()
    {
        var cursor = await SendAsync("get_cursor");
        var cy = cursor["y"]!.GetValue<int>();

        var res = await SendAsync("read_line");
        Assert.IsTrue(Ok(res));
        Assert.AreEqual(cy, res["row"]!.GetValue<int>());
    }

    [TestMethod]
    public async Task ReadLine_SpecificRow()
    {
        var res = await SendAsync("read_line", ("row", 0));
        Assert.IsTrue(Ok(res));
        Assert.AreEqual(0, res["row"]!.GetValue<int>());
        Assert.IsNotNull(res["text"]);
    }

    // ── Pause / Resume ──

    [TestMethod]
    public async Task DbgPause_ReturnsCpuState()
    {
        var res = await SendAsync("dbg_pause");
        Assert.IsTrue(Ok(res));
        Assert.IsNotNull(res["pc"]);
        // Always resume after pause so subsequent tests see live CPU
        await SendAsync("dbg_resume");
    }

    [TestMethod]
    public async Task DbgResume_ReturnsOk()
    {
        await SendAsync("dbg_pause");
        var res = await SendAsync("dbg_resume");
        Assert.IsTrue(Ok(res));
    }

    // ── Unknown command ──

    [TestMethod]
    public async Task UnknownCommand_ReturnsError()
    {
        var res = await SendAsync("this_command_does_not_exist");
        Assert.IsFalse(Ok(res));
        Assert.IsNotNull(res["error"]);
    }
}

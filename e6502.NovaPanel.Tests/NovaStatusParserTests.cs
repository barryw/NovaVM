using System.Text.Json.Nodes;
using e6502.NovaPanel.Services;
using Xunit;

namespace e6502.NovaPanel.Tests;

public class NovaStatusParserTests
{
    private static JsonObject LoadFixture()
    {
        string path = Path.Combine(AppContext.BaseDirectory, "Fixtures", "status-sample.json");
        return (JsonObject)JsonNode.Parse(File.ReadAllText(path))!;
    }

    [Fact]
    public void Parses_drives()
    {
        NovaStatus s = NovaStatusParser.Parse(LoadFixture());
        Assert.Equal(6, s.Drives.Count);
        Assert.Equal("fd0", s.Drives[0].Slot);
        Assert.True(s.Drives[0].Mounted);
        Assert.Equal("/disks/floppy/zork.ndi", s.Drives[0].CurrentPath);
        Assert.False(s.Drives[1].Mounted);
    }

    [Fact]
    public void Parses_runtimes_and_active()
    {
        NovaStatus s = NovaStatusParser.Parse(LoadFixture());
        Assert.Equal("novabasic", s.ActiveRuntime);
        Assert.Contains(s.Runtimes, r => r.Name == "novabasic" && r.IsActive);
        Assert.Contains(s.Runtimes, r => r.Name == "novaforth" && !r.IsActive);
        Assert.Equal("/roms/novabasic.bin", s.Runtimes.First(r => r.Name == "novabasic").Rom);
    }

    [Fact]
    public void Parses_health_wifi_sd_audio()
    {
        NovaStatus s = NovaStatusParser.Parse(LoadFixture());
        Assert.Equal("ready", s.BootPhase);
        Assert.True(s.FpgaBridgeAvailable);
        Assert.Equal("novanet", s.Wifi.Ssid);
        Assert.Equal(-55, s.Wifi.Rssi);
        Assert.Equal("SDHC", s.Sd.CardType);
        Assert.Equal("idle", s.Audio.Status);
    }

    [Fact]
    public void Null_status_returns_empty()
    {
        NovaStatus s = NovaStatusParser.Parse(null);
        Assert.Empty(s.Drives);
        Assert.Empty(s.Runtimes);
        Assert.Equal("", s.ActiveRuntime);
    }
}

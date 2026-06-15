using Avalonia.Controls;
using Avalonia.Headless.XUnit;
using e6502.NovaPanel.Services;
using e6502.NovaPanel.Views.Panels;
using Xunit;

namespace e6502.NovaPanel.Tests;

public class PanelTests
{
    private static NovaStatus Sample() => NovaStatusParser.Parse(new System.Text.Json.Nodes.JsonObject
    {
        ["health"] = new System.Text.Json.Nodes.JsonObject { ["bootPhase"] = "ready", ["fpgaBridgeAvailable"] = true },
        ["wifi"] = new System.Text.Json.Nodes.JsonObject { ["ssid"] = "novanet", ["connected"] = true, ["rssi"] = -55 },
        ["sdStatus"] = new System.Text.Json.Nodes.JsonObject { ["cardType"] = "SDHC", ["mounted"] = true },
        ["audio"] = new System.Text.Json.Nodes.JsonObject { ["status"] = "idle", ["ok"] = true },
        ["drives"] = new System.Text.Json.Nodes.JsonArray(),
        ["bootConfig"] = new System.Text.Json.Nodes.JsonObject { ["vm"] = new System.Text.Json.Nodes.JsonObject { ["defaultRuntime"] = "novabasic" }, ["languages"] = new System.Text.Json.Nodes.JsonObject() }
    });

    [AvaloniaFact]
    public void ConnBar_constructs_and_updates()
    {
        var bar = new ConnBar();
        var w = new Window { Content = bar };
        w.Show();
        bar.Update(Sample());
        bar.SetState(ConnectionState.Connected);
        // smoke: no throw; host box default present
        Assert.False(string.IsNullOrEmpty(bar.HostText));
    }

    [AvaloniaFact]
    public void ConnBar_connect_raises_event_with_host()
    {
        var bar = new ConnBar();
        var w = new Window { Content = bar };
        w.Show();
        string? host = null;
        bar.ConnectRequested += h => host = h;
        bar.RaiseConnectForTest();
        Assert.False(string.IsNullOrWhiteSpace(host));
    }

    [AvaloniaFact]
    public void StatusPanel_constructs_and_updates()
    {
        var p = new StatusPanel();
        var w = new Window { Content = p };
        w.Show();
        p.Update(Sample());
        Assert.True(true); // smoke: constructed + updated without throwing
    }
}

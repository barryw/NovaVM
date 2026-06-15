using Avalonia.Controls;
using Avalonia.Controls.Shapes;
using Avalonia.Media;
using e6502.NovaPanel.Services;

namespace e6502.NovaPanel.Views.Panels;

public partial class StatusPanel : UserControl
{
    // ── Panel convention ──────────────────────────────────────────────────

    /// <summary>Connection for action-bearing panels. Null-guarded; StatusPanel is read-only.</summary>
    public NovaConnection? Connection { get; set; }

    // ── Construction ──────────────────────────────────────────────────────

    public StatusPanel()
    {
        InitializeComponent();
    }

    // ── Public update method ──────────────────────────────────────────────

    /// <summary>Refreshes all lamp rows from status. Safe with NovaStatus.Empty.</summary>
    public void Update(NovaStatus status)
    {
        // Boot phase — green when "ready", yellow when in-progress, grey otherwise
        bool bootReady = status.BootPhase == "ready";
        bool bootActive = !string.IsNullOrEmpty(status.BootPhase) && !bootReady;
        SetLamp(BootLamp, bootReady ? LampState.Green : bootActive ? LampState.Yellow : LampState.Grey);
        BootValue.Text = string.IsNullOrEmpty(status.BootPhase) ? "—" : status.BootPhase;

        // SD card
        SetLamp(SdLamp, status.Sd.Mounted ? LampState.Green : LampState.Red);
        SdValue.Text = status.Sd.CardType.Length > 0
            ? $"{status.Sd.CardType} — {(status.Sd.Mounted ? "mounted" : "not mounted")}"
            : "—";

        // FPGA bridge
        SetLamp(FpgaLamp, status.FpgaBridgeAvailable ? LampState.Green : LampState.Red);
        FpgaValue.Text = status.FpgaBridgeAvailable ? "available" : "unavailable";

        // WiFi
        SetLamp(WifiLamp, status.Wifi.Connected ? LampState.Green : LampState.Red);
        WifiValue.Text = status.Wifi.Connected
            ? $"{status.Wifi.Ssid}  {status.Wifi.Rssi} dBm  {status.Wifi.LocalIp}"
            : status.Wifi.Configured ? "configured / not connected" : "—";

        // Audio
        SetLamp(AudioLamp, status.Audio.Ok ? LampState.Green : LampState.Red);
        AudioValue.Text = status.Audio.Status.Length > 0 ? status.Audio.Status : "—";
    }

    // ── Lamp helpers ──────────────────────────────────────────────────────

    private enum LampState { Green, Yellow, Red, Grey }

    private static void SetLamp(Ellipse lamp, LampState state)
    {
        lamp.Fill = state switch
        {
            LampState.Green  => Brush.Parse("#a6e3a1"),
            LampState.Yellow => Brush.Parse("#f9e2af"),
            LampState.Red    => Brush.Parse("#f38ba8"),
            _                => Brush.Parse("#6c7086"),
        };
    }
}

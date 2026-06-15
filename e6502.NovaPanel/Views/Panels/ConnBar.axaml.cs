using Avalonia;
using Avalonia.Controls;
using Avalonia.Media;
using e6502.NovaPanel.Services;

namespace e6502.NovaPanel.Views.Panels;

public partial class ConnBar : UserControl
{
    // ── Public surface ────────────────────────────────────────────────────

    /// <summary>Raised when the user clicks Connect; payload is the current host text.</summary>
    public event Action<string>? ConnectRequested;

    /// <summary>Raised when the user clicks Disconnect.</summary>
    public event Action? DisconnectRequested;

    /// <summary>Connection used by VM Reset / Host Reboot actions. Guard null.</summary>
    public NovaConnection? Connection { get; set; }

    /// <summary>Current text of the host input box. Never null — returns empty string.</summary>
    public string HostText => HostBox.Text ?? "";

    // ── Construction ──────────────────────────────────────────────────────

    public ConnBar()
    {
        InitializeComponent();
        ConnectBtn.Click += (_, _) => ConnectRequested?.Invoke(HostText);
        DisconnectBtn.Click += (_, _) => DisconnectRequested?.Invoke();
        VmResetBtn.Click += OnVmReset;
        HostRebootBtn.Click += OnHostReboot;
    }

    // ── Public update methods ─────────────────────────────────────────────

    /// <summary>Refresh status chips from a NovaStatus. Safe with NovaStatus.Empty.</summary>
    public void Update(NovaStatus status)
    {
        BootPhaseChip.Text = string.IsNullOrEmpty(status.BootPhase) ? "phase: —" : $"phase: {status.BootPhase}";

        string sdText = status.Sd.CardType.Length > 0
            ? $"SD: {status.Sd.CardType}/{(status.Sd.Mounted ? "mnt" : "umnt")}"
            : "SD: —";
        SdChip.Text = sdText;

        FpgaChip.Text = status.FpgaBridgeAvailable ? "FPGA: ok" : "FPGA: —";

        string wifiText = status.Wifi.Connected
            ? $"WiFi: {status.Wifi.Ssid} ({status.Wifi.Rssi} dBm)"
            : "WiFi: —";
        WifiChip.Text = wifiText;

        string audioText = status.Audio.Ok
            ? $"Audio: {status.Audio.Status}"
            : "Audio: —";
        AudioChip.Text = audioText;
    }

    /// <summary>Refresh the connection state chip color and label.</summary>
    public void SetState(ConnectionState state)
    {
        (string label, IBrush bg, IBrush fg) = state switch
        {
            ConnectionState.Connected    => ("Connected",    Brush.Parse("#1e4d2b"), Brush.Parse("#a6e3a1")),
            ConnectionState.Connecting   => ("Connecting…", Brush.Parse("#3b3210"), Brush.Parse("#f9e2af")),
            ConnectionState.Error        => ("Error",        Brush.Parse("#3b1a1a"), Brush.Parse("#f38ba8")),
            ConnectionState.Disconnected => ("Disconnected", Brush.Parse("#1e1e2e"), Brush.Parse("#6c7086")),
            _                            => ("Unknown",      Brush.Parse("#1e1e2e"), Brush.Parse("#6c7086")),
        };
        StateChip.Text = label;
        StateChipBorder.Background = bg;
        StateChip.Foreground = fg;
    }

    // ── Test seams ────────────────────────────────────────────────────────

    /// <summary>Exercises the same code path as the Connect button click. For tests.</summary>
    public void RaiseConnectForTest() => ConnectRequested?.Invoke(HostText);

    // ── Button handlers ───────────────────────────────────────────────────

    private async void OnVmReset(object? sender, Avalonia.Interactivity.RoutedEventArgs e)
    {
        if (Connection is null) return;
        await Connection.VmResetAsync();
    }

    private async void OnHostReboot(object? sender, Avalonia.Interactivity.RoutedEventArgs e)
    {
        if (Connection is null) return;
        bool confirmed = await ConfirmAsync("Reboot the host? This will interrupt all active sessions.");
        if (!confirmed) return;
        await Connection.HostRebootAsync();
    }

    /// <summary>
    /// Shows a yes/no confirmation dialog if a hosting Window is available.
    /// Returns true when no TopLevel is present (e.g. in headless tests).
    /// </summary>
    private async Task<bool> ConfirmAsync(string message)
    {
        if (TopLevel.GetTopLevel(this) is not Window parentWindow)
            return true;

        var tcs = new TaskCompletionSource<bool>();

        var okBtn = new Button { Content = "Reboot", HorizontalAlignment = Avalonia.Layout.HorizontalAlignment.Right };
        var cancelBtn = new Button { Content = "Cancel", HorizontalAlignment = Avalonia.Layout.HorizontalAlignment.Right };

        var dialog = new Window
        {
            Title = "Confirm",
            Width = 380,
            Height = 140,
            WindowStartupLocation = WindowStartupLocation.CenterOwner,
            CanResize = false,
            Content = new StackPanel
            {
                Margin = new Thickness(16),
                Spacing = 12,
                Children =
                {
                    new TextBlock { Text = message, TextWrapping = Avalonia.Media.TextWrapping.Wrap },
                    new StackPanel
                    {
                        Orientation = Avalonia.Layout.Orientation.Horizontal,
                        HorizontalAlignment = Avalonia.Layout.HorizontalAlignment.Right,
                        Spacing = 8,
                        Children = { cancelBtn, okBtn }
                    }
                }
            }
        };

        okBtn.Click += (_, _) => { tcs.TrySetResult(true); dialog.Close(); };
        cancelBtn.Click += (_, _) => { tcs.TrySetResult(false); dialog.Close(); };
        dialog.Closed += (_, _) => tcs.TrySetResult(false);

        await dialog.ShowDialog(parentWindow);
        return await tcs.Task;
    }
}

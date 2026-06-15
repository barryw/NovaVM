using Avalonia.Controls;
using Avalonia.Layout;
using Avalonia.Media;
using e6502.NovaPanel.Services;

namespace e6502.NovaPanel.Views.Panels;

public partial class RuntimePanel : UserControl
{
    // ── Public surface ────────────────────────────────────────────────────

    /// <summary>Connection for runtime actions. Null-guarded.</summary>
    public NovaConnection? Connection { get; set; }

    /// <summary>Test seam: number of runtime rows currently shown.</summary>
    public int RuntimeRowCount => RuntimeStack.Children.Count;

    // ── Construction ──────────────────────────────────────────────────────

    public RuntimePanel()
    {
        InitializeComponent();
        AddBtn.Click += async (_, _) => await OnAddClickedAsync();
    }

    // ── Public update method ──────────────────────────────────────────────

    /// <summary>Rebuilds runtime rows from status. Safe with NovaStatus.Empty.</summary>
    public void Update(NovaStatus status)
    {
        RuntimeStack.Children.Clear();
        foreach (var rt in status.Runtimes)
            RuntimeStack.Children.Add(BuildRow(rt));
    }

    // ── Private helpers ───────────────────────────────────────────────────

    private StackPanel BuildRow(RuntimeInfo rt)
    {
        var nameText = new TextBlock
        {
            Classes = { "rtname" },
            Text = rt.Name,
            FontWeight = rt.IsActive ? FontWeight.Bold : FontWeight.Normal,
            Foreground = rt.IsActive
                ? Brush.Parse("#a6e3a1")
                : Brush.Parse("#cdd6f4")
        };

        var romText = new TextBlock { Classes = { "rtpath" }, Text = rt.Rom };
        var extText = new TextBlock
        {
            Classes = { "rtmeta" },
            Text = string.IsNullOrEmpty(rt.ExtensionRom) ? "—" : rt.ExtensionRom
        };
        var autobootText = new TextBlock
        {
            Classes = { "rtmeta" },
            Text = rt.Autoboot ? "autoboot" : ""
        };

        var setActiveBtn = new Button { Classes = { "action" }, Content = "Set Active" };
        var removeBtn = new Button { Classes = { "danger" }, Content = "Remove", IsEnabled = !rt.IsActive };

        string capturedName = rt.Name;
        setActiveBtn.Click += async (_, _) =>
        {
            if (Connection is null) return;
            await Connection.RuntimeSetAsync(capturedName);
        };
        removeBtn.Click += async (_, _) =>
        {
            if (Connection is null) return;
            await Connection.RuntimeRemoveAsync(capturedName);
        };

        return new StackPanel
        {
            Orientation = Orientation.Horizontal,
            Spacing = 4,
            Children = { nameText, romText, extText, autobootText, setActiveBtn, removeBtn }
        };
    }

    private async Task OnAddClickedAsync()
    {
        if (Connection is null) return;
        string name = AddNameBox.Text?.Trim() ?? "";
        string rom = AddRomBox.Text?.Trim() ?? "";
        string? ext = string.IsNullOrWhiteSpace(AddExtRomBox.Text) ? null : AddExtRomBox.Text.Trim();
        bool makeActive = MakeActiveCheck.IsChecked ?? false;
        if (string.IsNullOrEmpty(name) || string.IsNullOrEmpty(rom)) return;
        await Connection.RuntimeAddAsync(name, rom, ext, makeActive);
    }
}

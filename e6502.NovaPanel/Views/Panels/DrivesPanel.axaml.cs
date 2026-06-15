using Avalonia.Controls;
using Avalonia.Layout;
using Avalonia.Media;
using e6502.NovaPanel.Services;

namespace e6502.NovaPanel.Views.Panels;

public partial class DrivesPanel : UserControl
{
    // Fixed slot order: fd0..fd3, hd0..hd1
    private static readonly string[] SlotOrder = ["fd0", "fd1", "fd2", "fd3", "hd0", "hd1"];

    // Per-row controls
    private sealed record DriveRow(
        Border BadgeBorder,
        TextBlock BadgeText,
        TextBlock PathText,
        TextBlock CfgPathText,
        Button MountBtn,
        Button RecallBtn,
        Button UnmountBtn,
        string Slot);

    private readonly Dictionary<string, DriveRow> _rows = new(StringComparer.OrdinalIgnoreCase);

    // ── Public surface ────────────────────────────────────────────────────

    /// <summary>Connection for drive actions. Null-guarded.</summary>
    public NovaConnection? Connection { get; set; }

    /// <summary>Test seam: number of slot rows currently shown (always 6).</summary>
    public int DriveRowCount => _rows.Count;

    // ── Construction ──────────────────────────────────────────────────────

    public DrivesPanel()
    {
        InitializeComponent();
        BuildRows();
    }

    // ── Public update method ──────────────────────────────────────────────

    /// <summary>Refreshes drive rows from status. Safe with NovaStatus.Empty.</summary>
    public void Update(NovaStatus status)
    {
        // Index incoming drives by slot
        var bySlot = status.Drives.ToDictionary(d => d.Slot, d => d, StringComparer.OrdinalIgnoreCase);

        foreach (var (slot, row) in _rows)
        {
            if (bySlot.TryGetValue(slot, out var info))
                ApplyDriveInfo(row, info);
            else
                ApplyEmpty(row);
        }
    }

    // ── Layout helpers ────────────────────────────────────────────────────

    private void BuildRows()
    {
        foreach (var slot in SlotOrder)
        {
            var badgeText = new TextBlock { Classes = { "badge" } };
            var badgeBorder = new Border
            {
                Classes = { "badge" },
                Child = badgeText
            };

            var pathText = new TextBlock { Classes = { "path" }, Text = "—" };
            var cfgText = new TextBlock { Classes = { "cfgpath" }, Text = "" };

            var mountBtn = new Button { Classes = { "action" }, Content = "Mount" };
            var recallBtn = new Button { Classes = { "action" }, Content = "Recall" };
            var unmountBtn = new Button { Classes = { "danger" }, Content = "Unmount" };

            var row = new DriveRow(badgeBorder, badgeText, pathText, cfgText, mountBtn, recallBtn, unmountBtn, slot);
            _rows[slot] = row;

            // Wire buttons (capture slot)
            string capturedSlot = slot;
            mountBtn.Click += async (_, _) =>
            {
                if (Connection is null) return;
                await Connection.MountAsync(capturedSlot, null);
            };
            recallBtn.Click += async (_, _) =>
            {
                if (Connection is null) return;
                await Connection.MountAsync(capturedSlot, null);
            };
            unmountBtn.Click += async (_, _) =>
            {
                if (Connection is null) return;
                await Connection.UnmountAsync(capturedSlot);
            };

            var slotLabel = new TextBlock
            {
                Classes = { "slot" },
                Text = slot.ToUpperInvariant()
            };

            var rowPanel = new StackPanel
            {
                Orientation = Orientation.Horizontal,
                Spacing = 4,
                Children =
                {
                    slotLabel,
                    badgeBorder,
                    pathText,
                    cfgText,
                    mountBtn,
                    recallBtn,
                    unmountBtn
                }
            };

            DriveStack.Children.Add(rowPanel);

            // Seed with empty state
            ApplyEmpty(row);
        }
    }

    private static void ApplyDriveInfo(DriveRow row, Services.DriveInfo info)
    {
        if (info.Mounted)
        {
            row.BadgeBorder.Background = Brush.Parse("#1e4d2b");
            row.BadgeText.Text = "mounted";
            row.BadgeText.Foreground = Brush.Parse("#a6e3a1");
        }
        else
        {
            row.BadgeBorder.Background = Brush.Parse("#2a2a3a");
            row.BadgeText.Text = "empty";
            row.BadgeText.Foreground = Brush.Parse("#6c7086");
        }

        row.PathText.Text = string.IsNullOrEmpty(info.CurrentPath) ? "—" : info.CurrentPath;
        row.CfgPathText.Text = string.IsNullOrEmpty(info.ConfiguredPath) ? "" : info.ConfiguredPath;
    }

    private static void ApplyEmpty(DriveRow row)
    {
        row.BadgeBorder.Background = Brush.Parse("#2a2a3a");
        row.BadgeText.Text = "empty";
        row.BadgeText.Foreground = Brush.Parse("#6c7086");
        row.PathText.Text = "—";
        row.CfgPathText.Text = "";
    }
}

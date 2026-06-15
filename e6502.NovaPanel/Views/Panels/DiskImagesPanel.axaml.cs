using System.Text.Json.Nodes;
using Avalonia.Controls;
using Avalonia.Layout;
using e6502.NovaPanel.Services;

namespace e6502.NovaPanel.Views.Panels;

public partial class DiskImagesPanel : UserControl
{
    // ── Public surface ────────────────────────────────────────────────────

    /// <summary>Connection for disk actions. Null-guarded.</summary>
    public NovaConnection? Connection { get; set; }

    /// <summary>Raised with the disk SD path when the user clicks Mount on a row.</summary>
    public event Action<string>? MountRequested;

    /// <summary>Test seam: number of disk image rows currently shown.</summary>
    public int DiskRowCount => DiskStack.Children.Count;

    // ── Construction ──────────────────────────────────────────────────────

    public DiskImagesPanel()
    {
        InitializeComponent();
        RefreshBtn.Click += async (_, _) => await RefreshAsync();
        UploadBtn.Click += async (_, _) => await OnUploadClickedAsync();
    }

    // ── Public methods ────────────────────────────────────────────────────

    /// <summary>
    /// Fetches disk image listings from disks/floppy and disks/hard, merges them,
    /// skips directories, and rebuilds the row list.
    /// </summary>
    public async Task RefreshAsync()
    {
        if (Connection is null)
        {
            DiskStack.Children.Clear();
            return;
        }

        var floppyTask = Connection.ListDirectoryAsync("disks/floppy");
        var hardTask = Connection.ListDirectoryAsync("disks/hard");

        JsonArray floppyEntries = await floppyTask;
        JsonArray hardEntries = await hardTask;

        DiskStack.Children.Clear();

        AddEntries(floppyEntries, "floppy", "disks/floppy");
        AddEntries(hardEntries, "hard", "disks/hard");
    }

    /// <summary>
    /// Uploads a local .ndi file to destDir on the SD card.
    /// </summary>
    public async Task UploadAsync(string localPath, string destDir, IProgress<long>? progress = null, CancellationToken ct = default)
    {
        if (Connection is null) return;
        string fileName = System.IO.Path.GetFileName(localPath);
        string destPath = $"{destDir.TrimEnd('/')}/{fileName}";
        await Connection.UploadFileAsync(localPath, destPath, progress, ct);
        await RefreshAsync();
    }

    // ── Private helpers ───────────────────────────────────────────────────

    private void AddEntries(JsonArray entries, string group, string dirPath)
    {
        foreach (var node in entries)
        {
            if (node is not JsonObject obj) continue;
            bool isDir = obj["isDir"]?.GetValue<bool>() ?? false;
            if (isDir) continue;

            string name = obj["name"]?.GetValue<string>() ?? "";
            long size = obj["size"]?.GetValue<long>() ?? 0;
            string sdPath = $"{dirPath}/{name}";

            DiskStack.Children.Add(BuildRow(name, size, group, sdPath));
        }
    }

    private StackPanel BuildRow(string name, long size, string group, string sdPath)
    {
        var nameLabel = new TextBlock { Classes = { "name" }, Text = name };
        var sizeLabel = new TextBlock { Classes = { "meta" }, Text = FormatSize(size) };
        var groupLabel = new TextBlock { Classes = { "meta" }, Text = group };

        var mountBtn = new Button { Classes = { "action" }, Content = "Mount" };
        var deleteBtn = new Button { Classes = { "danger" }, Content = "Delete" };

        mountBtn.Click += async (_, _) =>
        {
            MountRequested?.Invoke(sdPath);
            if (Connection is not null)
                await Connection.MountAsync("fd0", sdPath);
        };

        deleteBtn.Click += async (_, _) =>
        {
            if (Connection is null) return;
            await Connection.DeleteAsync(sdPath);
            await RefreshAsync();
        };

        return new StackPanel
        {
            Orientation = Orientation.Horizontal,
            Spacing = 4,
            Children = { nameLabel, sizeLabel, groupLabel, mountBtn, deleteBtn }
        };
    }

    private static string FormatSize(long bytes)
    {
        if (bytes < 1024) return $"{bytes} B";
        if (bytes < 1024 * 1024) return $"{bytes / 1024} KB";
        return $"{bytes / (1024 * 1024)} MB";
    }

    private async Task OnUploadClickedAsync()
    {
        // Guard: no TopLevel means headless/test context — do nothing.
        if (TopLevel.GetTopLevel(this) is not Window parentWindow) return;

        var dialog = new Avalonia.Platform.Storage.FilePickerOpenOptions
        {
            Title = "Select .ndi disk image",
            AllowMultiple = false,
            FileTypeFilter = [new Avalonia.Platform.Storage.FilePickerFileType("Nova Disk Image") { Patterns = ["*.ndi"] }]
        };

        var files = await parentWindow.StorageProvider.OpenFilePickerAsync(dialog);
        if (files is not { Count: > 0 }) return;

        string localPath = files[0].Path.LocalPath;
        await UploadAsync(localPath, "disks/floppy");
    }
}

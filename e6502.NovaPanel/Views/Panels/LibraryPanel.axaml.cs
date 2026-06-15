using System.Text.Json.Nodes;
using Avalonia.Controls;
using Avalonia.Layout;
using e6502.NovaPanel.Services;

namespace e6502.NovaPanel.Views.Panels;

public partial class LibraryPanel : UserControl
{
    private static readonly string[] KindItems = ["roms", "soundfonts", "music", "boot", "sid"];

    // Kind → SD directory mapping (1:1 for these)
    private static string KindToDir(string kind) => kind;

    // ── Public surface ────────────────────────────────────────────────────

    /// <summary>Connection for library actions. Null-guarded.</summary>
    public NovaConnection? Connection { get; set; }

    /// <summary>Test seam: number of file rows currently shown.</summary>
    public int LibraryRowCount => LibStack.Children.Count;

    // ── Construction ──────────────────────────────────────────────────────

    public LibraryPanel()
    {
        InitializeComponent();

        // Populate kind combo
        foreach (var k in KindItems)
            KindBox.Items.Add(k);
        KindBox.SelectedIndex = 0;

        RefreshBtn.Click += async (_, _) => await RefreshAsync();
        UploadBtn.Click += async (_, _) => await OnUploadClickedAsync();
    }

    // ── Public methods ────────────────────────────────────────────────────

    /// <summary>Fetches file listings for the selected kind. Guards null connection.</summary>
    public async Task RefreshAsync()
    {
        if (Connection is null)
        {
            LibStack.Children.Clear();
            StatusText.Text = "Not connected.";
            return;
        }

        string kind = KindBox.SelectedItem as string ?? KindItems[0];
        string dir = KindToDir(kind);

        StatusText.Text = $"Loading {dir}…";
        JsonArray entries = await Connection.ListDirectoryAsync(dir);

        LibStack.Children.Clear();
        int count = 0;
        foreach (var node in entries)
        {
            if (node is not JsonObject obj) continue;
            bool isDir = obj["isDir"]?.GetValue<bool>() ?? false;
            if (isDir) continue;

            string name = obj["name"]?.GetValue<string>() ?? "";
            long size = obj["size"]?.GetValue<long>() ?? 0;
            string sdPath = $"{dir}/{name}";

            LibStack.Children.Add(BuildRow(name, size, sdPath));
            count++;
        }

        StatusText.Text = count == 0 ? "No files." : $"{count} file(s)";
    }

    // ── Private helpers ───────────────────────────────────────────────────

    private StackPanel BuildRow(string name, long size, string sdPath)
    {
        var nameLabel = new TextBlock { Classes = { "name" }, Text = name };
        var sizeLabel = new TextBlock { Classes = { "meta" }, Text = FormatSize(size) };
        var deleteBtn = new Button { Classes = { "danger" }, Content = "Delete" };

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
            Children = { nameLabel, sizeLabel, deleteBtn }
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

        string kind = KindBox.SelectedItem as string ?? KindItems[0];
        string dir = KindToDir(kind);

        var dialog = new Avalonia.Platform.Storage.FilePickerOpenOptions
        {
            Title = $"Upload to {dir}",
            AllowMultiple = false
        };

        var files = await parentWindow.StorageProvider.OpenFilePickerAsync(dialog);
        if (files is not { Count: > 0 }) return;

        string localPath = files[0].Path.LocalPath;
        string fileName = System.IO.Path.GetFileName(localPath);
        string destPath = $"{dir}/{fileName}";

        if (Connection is null) return;
        StatusText.Text = "Uploading…";
        await Connection.UploadFileAsync(localPath, destPath);
        await RefreshAsync();
    }
}

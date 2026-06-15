using System.Text.Json.Nodes;
using Avalonia.Controls;
using Avalonia.Layout;
using e6502.Storage;
using e6502.NovaPanel.Services;

namespace e6502.NovaPanel.Views.Panels;

public partial class ModulesPanel : UserControl
{
    // ── Public surface ────────────────────────────────────────────────────

    /// <summary>Connection for module actions. Null-guarded.</summary>
    public NovaConnection? Connection { get; set; }

    /// <summary>Test seam: number of module rows currently shown.</summary>
    public int ModuleRowCount => ModuleStack.Children.Count;

    // ── Construction ──────────────────────────────────────────────────────

    public ModulesPanel()
    {
        InitializeComponent();
        RefreshBtn.Click += async (_, _) => await RefreshAsync();
    }

    // ── Public methods ────────────────────────────────────────────────────

    /// <summary>
    /// Lists lib/ directory, filters *.mod/*.nmod, parses each with NovaModule.Parse.
    /// Rows show name, id (hex), version, function count.
    /// </summary>
    public async Task RefreshAsync()
    {
        if (Connection is null)
        {
            ModuleStack.Children.Clear();
            StatusText.Text = "Not connected.";
            return;
        }

        StatusText.Text = "Loading modules…";
        JsonArray entries = await Connection.ListDirectoryAsync("lib");

        ModuleStack.Children.Clear();
        int count = 0;

        foreach (var node in entries)
        {
            if (node is not JsonObject obj) continue;
            bool isDir = obj["isDir"]?.GetValue<bool>() ?? false;
            if (isDir) continue;

            string name = obj["name"]?.GetValue<string>() ?? "";
            bool isModule = name.EndsWith(".mod", StringComparison.OrdinalIgnoreCase)
                         || name.EndsWith(".nmod", StringComparison.OrdinalIgnoreCase);
            if (!isModule) continue;

            string sdPath = $"lib/{name}";
            NovaModule? parsed = null;
            string? parseError = null;

            try
            {
                byte[]? bytes = await Connection.ReadFileAsync(sdPath);
                if (bytes is not null)
                    parsed = NovaModule.Parse(bytes);
                else
                    parseError = "read failed";
            }
            catch (Exception ex)
            {
                parseError = ex.Message;
            }

            ModuleStack.Children.Add(BuildRow(name, sdPath, parsed, parseError));
            count++;
        }

        StatusText.Text = count == 0 ? "No modules." : $"{count} module(s)";
    }

    // ── Private helpers ───────────────────────────────────────────────────

    private StackPanel BuildRow(string name, string sdPath, NovaModule? module, string? parseError)
    {
        bool valid = module?.Valid ?? false;
        string idHex = valid ? $"${module!.Id:X2}" : "—";
        string version = (valid && module!.HasDoc && module.Doc?.Module.Version is string v) ? v : "—";
        string fnCount = valid ? module!.FnCount.ToString() : "—";

        var nameLabel = new TextBlock
        {
            Classes = { "modname" },
            Text = parseError is not null ? $"{name} (invalid)" : name,
            Foreground = parseError is not null
                ? Avalonia.Media.Brush.Parse("#f38ba8")
                : Avalonia.Media.Brush.Parse("#cdd6f4")
        };
        var idLabel = new TextBlock { Classes = { "modmeta" }, Text = idHex };
        var verLabel = new TextBlock { Classes = { "modmeta" }, Text = version };
        var fnLabel = new TextBlock { Classes = { "modmeta" }, Text = $"{fnCount} fn" };

        var funcsBtn = new Button { Classes = { "action" }, Content = "Functions", IsEnabled = valid && module!.HasDoc };
        var deleteBtn = new Button { Classes = { "danger" }, Content = "Delete" };

        if (valid && module!.HasDoc && module.Doc is { } doc)
        {
            var capturedDoc = doc;
            string capturedName = name;
            funcsBtn.Click += (_, _) => ShowFunctionsModal(capturedName, capturedDoc);
        }

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
            Children = { nameLabel, idLabel, verLabel, fnLabel, funcsBtn, deleteBtn }
        };
    }

    private void ShowFunctionsModal(string moduleName, NovaModuleDoc doc)
    {
        // Guard: no TopLevel means headless/test — do nothing.
        if (TopLevel.GetTopLevel(this) is null) return;

        var stack = new StackPanel { Spacing = 4, Margin = new Avalonia.Thickness(12) };
        foreach (var fn in doc.Functions)
        {
            stack.Children.Add(new TextBlock
            {
                Text = fn.Signature(),
                FontSize = 11,
                FontFamily = new Avalonia.Media.FontFamily("Monospace"),
                Foreground = Avalonia.Media.Brush.Parse("#cdd6f4")
            });
        }

        var win = new Window
        {
            Title = $"Functions — {moduleName}",
            Width = 500,
            Height = 400,
            Background = Avalonia.Media.Brush.Parse("#11111b"),
            Content = new ScrollViewer { Content = stack }
        };
        win.Show();
    }
}

using System.Text.Json;

namespace e6502.NovaPanel.Services;

public sealed class PanelSettings
{
    public string Host { get; set; } = "novahost.local";
    public double? X { get; set; }
    public double? Y { get; set; }
    public double Width { get; set; }
    public double Height { get; set; }

    private static readonly JsonSerializerOptions Opts = new() { WriteIndented = true };

    /// <summary>Default path: ~/.e6502/novapanel.json</summary>
    public static string DefaultPath
    {
        get
        {
            string home = Environment.GetFolderPath(Environment.SpecialFolder.UserProfile);
            return Path.Combine(home, ".e6502", "novapanel.json");
        }
    }

    public static PanelSettings Load(string? path = null)
    {
        path ??= DefaultPath;
        try
        {
            if (!File.Exists(path)) return new PanelSettings();
            string json = File.ReadAllText(path);
            return JsonSerializer.Deserialize<PanelSettings>(json, Opts) ?? new PanelSettings();
        }
        catch
        {
            return new PanelSettings(); // corrupt/unreadable → defaults
        }
    }

    public void Save(string? path = null)
    {
        path ??= DefaultPath;
        try
        {
            string dir = Path.GetDirectoryName(path)!;
            Directory.CreateDirectory(dir);
            File.WriteAllText(path, JsonSerializer.Serialize(this, Opts));
        }
        catch
        {
            // best-effort; never throw from save
        }
    }
}

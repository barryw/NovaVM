using e6502.NovaPanel.Services;
using Xunit;

namespace e6502.NovaPanel.Tests;

public class PanelSettingsTests
{
    [Fact]
    public void Roundtrips_via_file()
    {
        string path = Path.Combine(Path.GetTempPath(), "novapanel-test-" + Guid.NewGuid().ToString("N") + ".json");
        try
        {
            var s = new PanelSettings { Host = "192.168.1.65", X = 10, Y = 20, Width = 1200, Height = 700 };
            s.Save(path);

            PanelSettings loaded = PanelSettings.Load(path);
            Assert.Equal("192.168.1.65", loaded.Host);
            Assert.Equal(10, loaded.X);
            Assert.Equal(20, loaded.Y);
            Assert.Equal(1200, loaded.Width);
            Assert.Equal(700, loaded.Height);
        }
        finally
        {
            if (File.Exists(path)) File.Delete(path);
        }
    }

    [Fact]
    public void Missing_file_returns_defaults()
    {
        string path = Path.Combine(Path.GetTempPath(), "novapanel-missing-" + Guid.NewGuid().ToString("N") + ".json");
        PanelSettings s = PanelSettings.Load(path);
        Assert.Equal("novahost.local", s.Host);
    }
}

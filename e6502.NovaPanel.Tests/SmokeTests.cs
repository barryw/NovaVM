using Avalonia.Headless.XUnit;
using e6502.NovaPanel;
using Xunit;

namespace e6502.NovaPanel.Tests;

public class SmokeTests
{
    [AvaloniaFact]
    public void Window_constructs()
    {
        var w = new MainWindow();
        w.Show();
        Assert.Equal("NovaPanel", w.Title);
    }
}

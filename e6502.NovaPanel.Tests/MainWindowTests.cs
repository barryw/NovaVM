using Avalonia.Headless.XUnit;
using e6502.NovaPanel;
using e6502.NovaPanel.Layout;
using Xunit;

namespace e6502.NovaPanel.Tests;

public class MainWindowTests
{
    [AvaloniaFact]
    public void Compact_hides_rail_and_bottom()
    {
        var w = new MainWindow();
        w.Show();
        w.ApplyLayoutForTest(900, 400);
        Assert.Equal(SizeClass.Compact, w.CurrentSizeClass);
        Assert.False(w.RailVisible);
        Assert.False(w.BottomVisible);
    }

    [AvaloniaFact]
    public void Wide_shows_rail_only()
    {
        var w = new MainWindow();
        w.Show();
        w.ApplyLayoutForTest(1300, 400);
        Assert.Equal(SizeClass.Wide, w.CurrentSizeClass);
        Assert.True(w.RailVisible);
        Assert.False(w.BottomVisible);
    }

    [AvaloniaFact]
    public void Full_shows_rail_and_bottom()
    {
        var w = new MainWindow();
        w.Show();
        w.ApplyLayoutForTest(1300, 800);
        Assert.Equal(SizeClass.Full, w.CurrentSizeClass);
        Assert.True(w.RailVisible);
        Assert.True(w.BottomVisible);
    }

    [AvaloniaFact]
    public void Window_has_keyboard_minimum_size()
    {
        var w = new MainWindow();
        w.Show();
        Assert.True(w.MinWidth > 0);
        Assert.True(w.MinHeight > 0);
    }
}

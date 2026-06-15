using Avalonia.Controls;
using Avalonia.Headless.XUnit;
using Avalonia.Input;
using e6502.NovaPanel.Views;
using Xunit;

namespace e6502.NovaPanel.Tests;

public class KeyboardViewTests
{
    [AvaloniaFact]
    public void Clicking_enter_raises_ENTER()
    {
        var window = new Window { Content = new KeyboardView() };
        window.Show();
        var view = (KeyboardView)window.Content!;

        string? got = null;
        view.KeyActivated += t => got = t;
        view.ActivateForTest(Key.Enter);

        Assert.Equal("ENTER", got);
    }

    [AvaloniaFact]
    public void Latched_shift_then_letter_raises_uppercase()
    {
        var window = new Window { Content = new KeyboardView() };
        window.Show();
        var view = (KeyboardView)window.Content!;

        string? got = null;
        view.KeyActivated += t => got = t;
        view.ActivateForTest(Key.LeftShift); // latch shift
        view.ActivateForTest(Key.A);

        Assert.Equal("A", got);
    }
}

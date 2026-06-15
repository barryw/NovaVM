using Avalonia.Controls;
using Avalonia.Headless;
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

    [AvaloniaFact]
    public void Physical_shift_A_raises_uppercase_and_highlights()
    {
        var window = new Window();
        var view = new KeyboardView();
        window.Content = view;
        window.Show();
        view.Focus();

        string? got = null;
        view.KeyActivated += t => got = t;

        // KeyPressQwerty is press-only (separate from KeyReleaseQwerty).
        // Assert IsKeyActive after press, before release, so the highlight is still set.
        window.KeyPressQwerty(PhysicalKey.A, RawInputModifiers.Shift);

        Assert.Equal("A", got);
        Assert.True(view.IsKeyActive(Key.A));

        // Release and confirm highlight clears.
        window.KeyReleaseQwerty(PhysicalKey.A, RawInputModifiers.Shift);
        Assert.False(view.IsKeyActive(Key.A));
    }
}

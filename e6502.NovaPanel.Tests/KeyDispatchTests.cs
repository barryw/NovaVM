using Avalonia.Input;
using e6502.NovaPanel.Input;
using Xunit;

namespace e6502.NovaPanel.Tests;

public class KeyDispatchTests
{
    [Theory]
    [InlineData(Key.Enter, KeyModifiers.None, "ENTER")]
    [InlineData(Key.Back, KeyModifiers.None, "BACKSPACE")]
    [InlineData(Key.Tab, KeyModifiers.None, "TAB")]
    [InlineData(Key.Escape, KeyModifiers.None, "ESC")]
    [InlineData(Key.Space, KeyModifiers.None, "SPACE")]
    [InlineData(Key.Left, KeyModifiers.None, "LEFT")]
    [InlineData(Key.Right, KeyModifiers.None, "RIGHT")]
    [InlineData(Key.Up, KeyModifiers.None, "UP")]
    [InlineData(Key.Down, KeyModifiers.None, "DOWN")]
    [InlineData(Key.PageUp, KeyModifiers.None, "PGUP")]
    [InlineData(Key.PageDown, KeyModifiers.None, "PGDN")]
    [InlineData(Key.Delete, KeyModifiers.None, "DELETE")]
    [InlineData(Key.Home, KeyModifiers.None, "HOME")]
    [InlineData(Key.End, KeyModifiers.None, "END")]
    [InlineData(Key.Home, KeyModifiers.Control, "CTRL-HOME")]
    [InlineData(Key.End, KeyModifiers.Control, "CTRL-END")]
    [InlineData(Key.A, KeyModifiers.None, "a")]
    [InlineData(Key.A, KeyModifiers.Shift, "A")]
    [InlineData(Key.Z, KeyModifiers.Shift, "Z")]
    [InlineData(Key.D1, KeyModifiers.None, "1")]
    [InlineData(Key.D1, KeyModifiers.Shift, "!")]
    [InlineData(Key.D0, KeyModifiers.Shift, ")")]
    [InlineData(Key.OemMinus, KeyModifiers.None, "-")]
    [InlineData(Key.OemMinus, KeyModifiers.Shift, "_")]
    [InlineData(Key.A, KeyModifiers.Control, "CTRL-A")]
    [InlineData(Key.A, KeyModifiers.Alt, "ALT-A")]
    [InlineData(Key.C, KeyModifiers.Control, "CTRL-C")]
    public void Resolves_expected_token(Key key, KeyModifiers mods, string expected)
        => Assert.Equal(expected, KeyDispatch.Resolve(key, mods));

    [Theory]
    [InlineData(Key.F1)]
    [InlineData(Key.F12)]
    [InlineData(Key.LeftShift)]
    [InlineData(Key.LeftCtrl)]
    [InlineData(Key.Insert)]
    public void Dead_keys_return_null(Key key)
        => Assert.Null(KeyDispatch.Resolve(key, KeyModifiers.None));
}

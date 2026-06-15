using Avalonia.Input;
using e6502.NovaPanel.Input;
using Xunit;

namespace e6502.NovaPanel.Tests;

public class KeyboardLayoutTests
{
    [Fact]
    public void No_normal_cap_is_a_dead_key()
    {
        foreach (KeyCap cap in KeyboardLayout.AllCaps.Where(c => c.Kind == KeyKind.Normal))
        {
            string? token = KeyDispatch.Resolve(cap.Key, KeyModifiers.None);
            Assert.True(token is not null,
                $"Normal cap '{cap.Label}' (Key.{cap.Key}) resolves to no firmware token");
        }
    }

    [Fact]
    public void Modifier_caps_are_marked_modifier()
    {
        Assert.Contains(KeyboardLayout.AllCaps, c => c.Kind == KeyKind.Modifier);
    }

    [Fact]
    public void Contains_essential_keys()
    {
        var keys = KeyboardLayout.AllCaps.Select(c => c.Key).ToHashSet();
        Assert.Contains(Key.Enter, keys);
        Assert.Contains(Key.Space, keys);
        Assert.Contains(Key.Up, keys);
        Assert.Contains(Key.Down, keys);
        Assert.Contains(Key.Left, keys);
        Assert.Contains(Key.Right, keys);
    }

    [Fact]
    public void Contains_no_function_keys()
    {
        foreach (KeyCap cap in KeyboardLayout.AllCaps)
            Assert.False(cap.Key is >= Key.F1 and <= Key.F24,
                $"Layout must not contain function key {cap.Key}");
    }
}

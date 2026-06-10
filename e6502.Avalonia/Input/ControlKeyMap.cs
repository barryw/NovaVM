using Avalonia.Input;

namespace e6502.Avalonia.Input;

/// <summary>
/// Maps Ctrl + A-Z to ASCII control codes (Ctrl-A=0x01 .. Ctrl-Z=0x1A).
/// </summary>
public static class ControlKeyMap
{
    public static bool TryMap(Key key, KeyModifiers modifiers, out byte code)
    {
        code = 0;
        // Control only: on macOS, Meta (Cmd) is reserved for host app shortcuts
        // (Cmd-F find, etc.) handled in MainWindow. The hardware keyboard has Ctrl.
        if (!modifiers.HasFlag(KeyModifiers.Control)) return false;
        if (key < Key.A || key > Key.Z) return false;
        code = (byte)(1 + (key - Key.A));   // Ctrl-A=0x01 .. Ctrl-Z=0x1A
        return true;
    }
}

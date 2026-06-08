using Avalonia.Input;
using e6502.Avalonia.Hardware;

namespace e6502.Avalonia.Input;

/// <summary>
/// Maps host keyboard keys to board-input button bits (the $BA9C layout in
/// VgcConstants / runtime/asm/input.inc) so the GUI can drive JOY().
/// Arrow keys = joystick directions, Z/X = FIRE1/FIRE2. The mapping is additive
/// at the call site (it never consumes the key event), so arrows still move the
/// editor cursor and Z/X still type their letters.
/// </summary>
public static class BoardInputKeyMap
{
    public static bool TryMap(Key key, out byte bit)
    {
        bit = key switch
        {
            Key.Up    => VgcConstants.BoardButtonUp,
            Key.Down  => VgcConstants.BoardButtonDown,
            Key.Left  => VgcConstants.BoardButtonLeft,
            Key.Right => VgcConstants.BoardButtonRight,
            Key.Z     => VgcConstants.BoardButtonFire1,
            Key.X     => VgcConstants.BoardButtonFire2,
            _         => (byte)0,
        };
        return bit != 0;
    }
}

namespace e6502.Avalonia.Input;

/// <summary>
/// Maps modifier+key combinations to character codes for a specific font.
/// When a font has a keymap, Shift+A-Z and Ctrl+A-Z produce graphic characters.
/// </summary>
public class FontKeymap
{
    public static readonly FontKeymap None = new(null);

    private readonly Dictionary<(KeyMod mod, char key), byte>? _map;

    private FontKeymap(Dictionary<(KeyMod, char), byte>? map) { _map = map; }

    public bool TryMap(KeyMod mod, char key, out byte charCode)
    {
        charCode = 0;
        if (_map is null) return false;
        return _map.TryGetValue((mod, char.ToUpper(key)), out charCode);
    }

    public bool HasMappings => _map is not null;

    /// <summary>Get all mappings for a given modifier, for display in help panel.</summary>
    public IEnumerable<(char key, byte charCode)> GetMappings(KeyMod mod)
    {
        if (_map is null) yield break;
        foreach (var ((m, k), code) in _map)
            if (m == mod) yield return (k, code);
    }

    /// <summary>PETSCII Upper/Graphics keymap.</summary>
    public static readonly FontKeymap PetsciiUpper = BuildPetsciiKeymap();

    /// <summary>PETSCII Lower/Upper keymap (same graphic chars).</summary>
    public static readonly FontKeymap PetsciiLower = BuildPetsciiKeymap();

    private static FontKeymap BuildPetsciiKeymap()
    {
        var map = new Dictionary<(KeyMod, char), byte>();
        // Shift+A through Shift+Z → graphic chars at $80-$99
        for (int i = 0; i < 26; i++)
            map[(KeyMod.Shift, (char)('A' + i))] = (byte)(0x80 + i);
        // Ctrl+A through Ctrl+Z → graphic chars at $A0-$B9
        for (int i = 0; i < 26; i++)
            map[(KeyMod.Ctrl, (char)('A' + i))] = (byte)(0xA0 + i);
        return new FontKeymap(map);
    }
}

public enum KeyMod { None, Shift, Ctrl }

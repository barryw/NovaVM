using System.Collections.Generic;
using Avalonia.Input;

namespace e6502.NovaPanel.Input;

/// <summary>
/// Maps an Avalonia key press to the NovaHost debug-protocol token.
/// Returns null for keys with no firmware representation (dead keys).
/// </summary>
public static class KeyDispatch
{
    // US-ANSI unshifted characters for digit/punctuation keys
    private static readonly Dictionary<Key, char> Unshifted = new()
    {
        { Key.D0, '0' }, { Key.D1, '1' }, { Key.D2, '2' }, { Key.D3, '3' }, { Key.D4, '4' },
        { Key.D5, '5' }, { Key.D6, '6' }, { Key.D7, '7' }, { Key.D8, '8' }, { Key.D9, '9' },
        { Key.OemMinus,        '-' },
        { Key.OemPlus,         '=' },
        { Key.OemOpenBrackets, '[' },
        { Key.OemCloseBrackets,']' },
        { Key.OemPipe,         '\\'},
        { Key.OemSemicolon,    ';' },
        { Key.OemQuotes,       '\'' },
        { Key.OemComma,        ',' },
        { Key.OemPeriod,       '.' },
        { Key.OemQuestion,     '/' },
        { Key.OemTilde,        '`' },
    };

    // US-ANSI shifted characters for digit/punctuation keys
    private static readonly Dictionary<Key, char> Shifted = new()
    {
        { Key.D0, ')' }, { Key.D1, '!' }, { Key.D2, '@' }, { Key.D3, '#' }, { Key.D4, '$' },
        { Key.D5, '%' }, { Key.D6, '^' }, { Key.D7, '&' }, { Key.D8, '*' }, { Key.D9, '(' },
        { Key.OemMinus,        '_' },
        { Key.OemPlus,         '+' },
        { Key.OemOpenBrackets, '{' },
        { Key.OemCloseBrackets,'}' },
        { Key.OemPipe,         '|' },
        { Key.OemSemicolon,    ':' },
        { Key.OemQuotes,       '"' },
        { Key.OemComma,        '<' },
        { Key.OemPeriod,       '>' },
        { Key.OemQuestion,     '?' },
        { Key.OemTilde,        '~' },
    };

    public static string? Resolve(Key key, KeyModifiers mods)
    {
        bool ctrl  = mods.HasFlag(KeyModifiers.Control);
        bool alt   = mods.HasFlag(KeyModifiers.Alt);
        bool shift = mods.HasFlag(KeyModifiers.Shift);

        // Rule 1: named non-letter keys
        switch (key)
        {
            case Key.Enter:    return "ENTER";
            case Key.Back:     return "BACKSPACE";
            case Key.Tab:      return "TAB";
            case Key.Escape:   return "ESC";
            case Key.Space:    return "SPACE";
            case Key.Left:     return "LEFT";
            case Key.Right:    return "RIGHT";
            case Key.Up:       return "UP";
            case Key.Down:     return "DOWN";
            case Key.PageUp:   return "PGUP";
            case Key.PageDown: return "PGDN";
            case Key.Delete:   return "DELETE";
            case Key.Home:     return ctrl ? "CTRL-HOME" : "HOME";
            case Key.End:      return ctrl ? "CTRL-END"  : "END";
        }

        // Rule 2: letters A–Z
        if (key >= Key.A && key <= Key.Z)
        {
            char upper = (char)('A' + (key - Key.A));
            if (ctrl)  return "CTRL-" + upper;
            if (alt)   return "ALT-"  + upper;
            return shift ? upper.ToString() : char.ToLower(upper).ToString();
        }

        // Rule 3: digits and punctuation — no token for modified digit/punct
        if (ctrl || alt)
            return null;

        if (shift && Shifted.TryGetValue(key, out char shiftedCh))
            return shiftedCh.ToString();

        if (!shift && Unshifted.TryGetValue(key, out char unshiftedCh))
            return unshiftedCh.ToString();

        // Rule 4: everything else (F-keys, modifiers, Insert, …)
        return null;
    }
}

using System.Collections.Generic;
using System.Linq;
using Avalonia.Input;

namespace e6502.NovaPanel.Input;

public enum KeyKind { Normal, Modifier }

public sealed record KeyCap(
    string Label,
    string? ShiftLabel,
    Key Key,
    double WidthUnits = 1.0,
    KeyKind Kind = KeyKind.Normal);

public static class KeyboardLayout
{
    public static IReadOnlyList<IReadOnlyList<KeyCap>> Rows { get; } = Build();

    public static IEnumerable<KeyCap> AllCaps => Rows.SelectMany(r => r);

    private static IReadOnlyList<IReadOnlyList<KeyCap>> Build()
    {
        // Row 0: Esc, ` 1 2 3 4 5 6 7 8 9 0 - = Backspace
        var row0 = new List<KeyCap>
        {
            new("Esc",       null,  Key.Escape),
            new("`",         "~",   Key.OemTilde),
            new("1",         "!",   Key.D1),
            new("2",         "@",   Key.D2),
            new("3",         "#",   Key.D3),
            new("4",         "$",   Key.D4),
            new("5",         "%",   Key.D5),
            new("6",         "^",   Key.D6),
            new("7",         "&",   Key.D7),
            new("8",         "*",   Key.D8),
            new("9",         "(",   Key.D9),
            new("0",         ")",   Key.D0),
            new("-",         "_",   Key.OemMinus),
            new("=",         "+",   Key.OemPlus),
            new("Backspace", null,  Key.Back, 2.0),
        };

        // Row 1: Tab Q W E R T Y U I O P [ ] \
        var row1 = new List<KeyCap>
        {
            new("Tab",  null, Key.Tab, 1.5),
            new("Q",    null, Key.Q),
            new("W",    null, Key.W),
            new("E",    null, Key.E),
            new("R",    null, Key.R),
            new("T",    null, Key.T),
            new("Y",    null, Key.Y),
            new("U",    null, Key.U),
            new("I",    null, Key.I),
            new("O",    null, Key.O),
            new("P",    null, Key.P),
            new("[",    "{",  Key.OemOpenBrackets),
            new("]",    "}",  Key.OemCloseBrackets),
            new("\\",   "|",  Key.OemPipe),
        };

        // Row 2: A S D F G H J K L ; ' Enter
        var row2 = new List<KeyCap>
        {
            new("A",     null, Key.A),
            new("S",     null, Key.S),
            new("D",     null, Key.D),
            new("F",     null, Key.F),
            new("G",     null, Key.G),
            new("H",     null, Key.H),
            new("J",     null, Key.J),
            new("K",     null, Key.K),
            new("L",     null, Key.L),
            new(";",     ":",  Key.OemSemicolon),
            new("'",     "\"", Key.OemQuotes),
            new("Enter", null, Key.Enter, 2.2),
        };

        // Row 3: Shift Z X C V B N M , . / Shift
        var row3 = new List<KeyCap>
        {
            new("Shift", null, Key.LeftShift,  2.4, KeyKind.Modifier),
            new("Z",     null, Key.Z),
            new("X",     null, Key.X),
            new("C",     null, Key.C),
            new("V",     null, Key.V),
            new("B",     null, Key.B),
            new("N",     null, Key.N),
            new("M",     null, Key.M),
            new(",",     "<",  Key.OemComma),
            new(".",     ">",  Key.OemPeriod),
            new("/",     "?",  Key.OemQuestion),
            new("Shift", null, Key.RightShift, 2.4, KeyKind.Modifier),
        };

        // Row 4: Ctrl Alt Space Alt Ctrl
        var row4 = new List<KeyCap>
        {
            new("Ctrl",  null, Key.LeftCtrl,  1.0, KeyKind.Modifier),
            new("Alt",   null, Key.LeftAlt,   1.0, KeyKind.Modifier),
            new("Space", null, Key.Space,      6.0),
            new("Alt",   null, Key.RightAlt,  1.0, KeyKind.Modifier),
            new("Ctrl",  null, Key.RightCtrl, 1.0, KeyKind.Modifier),
        };

        // Nav cluster
        var rowNav = new List<KeyCap>
        {
            new("Home",  null, Key.Home),
            new("End",   null, Key.End),
            new("PgUp",  null, Key.PageUp),
            new("PgDn",  null, Key.PageDown),
            new("Del",   null, Key.Delete),
            new("Up",    null, Key.Up),
            new("Down",  null, Key.Down),
            new("Left",  null, Key.Left),
            new("Right", null, Key.Right),
        };

        return new List<IReadOnlyList<KeyCap>>
        {
            row0, row1, row2, row3, row4, rowNav,
        };
    }
}

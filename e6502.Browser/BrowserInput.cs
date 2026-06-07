using System.Runtime.InteropServices.JavaScript;
using e6502.Avalonia.Input;

namespace e6502.Browser;

public static partial class BrowserInput
{
    private static ScreenEditor? _editor;
    private static bool _quoteMode;

    public static void Initialize(ScreenEditor editor)
    {
        _editor = editor;
        _quoteMode = false;
    }

    [JSExport]
    public static bool QueueKey(string key, bool shift, bool control, bool meta)
    {
        if (_editor is null)
            return false;

        if (control || meta)
            return false;

        switch (key)
        {
            case "ArrowLeft":
                _editor.QueueInput(28);
                return true;
            case "ArrowRight":
                _editor.QueueInput(29);
                return true;
            case "ArrowUp":
                _editor.QueueInput(30);
                return true;
            case "ArrowDown":
                _editor.QueueInput(31);
                return true;
            case "Enter":
                _quoteMode = false;
                _editor.QueueInput(0x0D);
                return true;
            case "Backspace":
                _editor.QueueInput(0x08);
                return true;
            case "Escape":
                _editor.QueueInput(0x03);
                return true;
        }

        if (key.Length == 1)
        {
            char ch = key[0];
            if (ch >= 0x20 && ch <= 0x7E)
            {
                QueuePrintableChar((byte)ch);
                return true;
            }
        }

        return false;
    }

    [JSExport]
    public static void QueueText(string text)
    {
        if (_editor is null || string.IsNullOrEmpty(text))
            return;

        for (int i = 0; i < text.Length; i++)
        {
            char ch = text[i];
            if (ch == '\r' || ch == '\n')
            {
                if (ch == '\n' && i > 0 && text[i - 1] == '\r')
                    continue;
                _quoteMode = false;
                _editor.QueueInput(0x0D);
                continue;
            }

            if (ch == '\t')
                ch = ' ';

            if (ch >= 0x20 && ch <= 0x7E)
                QueuePrintableChar((byte)ch);
        }
    }

    private static void QueuePrintableChar(byte ch)
    {
        if (_editor is null)
            return;

        if (ch == '"')
            _quoteMode = !_quoteMode;

        if (!_quoteMode && ch >= 0x61 && ch <= 0x7A)
            ch -= 0x20;

        _editor.QueueInput(ch);
    }
}

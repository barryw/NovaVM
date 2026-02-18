using System.Text;
using e6502.TUI.Hardware;
using Terminal.Gui;

namespace e6502.TUI.Rendering;

public class DisplayView : View
{
    private readonly VirtualGraphicsController _vgc;
    private readonly ScreenEditor _editor;
    private readonly object _renderLock = new();
    private bool _cursorVisible = true;

    public DisplayView(VirtualGraphicsController vgc)
    {
        _vgc = vgc;
        _editor = new ScreenEditor(vgc);
        _vgc.SetScreenEditor(_editor);

        CanFocus = true;
        Width = VgcConstants.ScreenCols;
        Height = VgcConstants.ScreenRows;
    }

    public void ToggleCursor()
    {
        lock (_renderLock)
        {
            _cursorVisible = !_cursorVisible;
        }
        SetNeedsDraw();
    }

    protected override bool OnKeyDown(Key keyEvent)
    {
        var key = keyEvent.KeyCode;

        switch (key)
        {
            case KeyCode.CursorLeft:
                _editor.CursorLeft();
                return true;

            case KeyCode.CursorRight:
                _editor.CursorRight();
                return true;

            case KeyCode.CursorUp:
                _editor.CursorUp();
                return true;

            case KeyCode.CursorDown:
                _editor.CursorDown();
                return true;

            case KeyCode.Enter:
                _editor.HandleReturn();
                return true;

            case KeyCode.Backspace:
                _editor.CursorLeft();
                // Write space to screen RAM at new cursor position
                {
                    int cx = _vgc.GetCursorX();
                    int cy = _vgc.GetCursorY();
                    ushort addr = (ushort)(VgcConstants.CharRamBase + cy * VgcConstants.ScreenCols + cx);
                    _vgc.Write(addr, 0x20);
                }
                return true;

            case KeyCode.Esc:
                // CTRL-C interrupt — feed directly to VGC
                _vgc.FeedInput(0x03);
                return true;

            default:
                // Printable characters
                if (keyEvent.AsRune.Value >= 0x20 && keyEvent.AsRune.Value <= 0x7E)
                {
                    _editor.HandleTypedChar((byte)keyEvent.AsRune.Value);
                    return true;
                }
                break;
        }

        return base.OnKeyDown(keyEvent);
    }

    protected override bool OnDrawingContent()
    {
        lock (_renderLock)
        {
            var driver = Application.Driver;
            if (driver is null)
                return false;

            byte bgColorIdx = _vgc.GetBgColor();
            Color bgColor = ColorPalette.Get(bgColorIdx);

            int cursorX = _vgc.GetCursorX();
            int cursorY = _vgc.GetCursorY();

            for (int row = 0; row < VgcConstants.ScreenRows; row++)
            {
                for (int col = 0; col < VgcConstants.ScreenCols; col++)
                {
                    byte screenChar  = _vgc.GetScreenChar(col, row);
                    byte colorIdx    = _vgc.GetScreenColor(col, row);

                    Color fg = ColorPalette.Get(colorIdx);
                    Color bg = bgColor;

                    bool isCursor = _cursorVisible && col == cursorX && row == cursorY;
                    if (isCursor)
                    {
                        // Invert fg/bg for cursor
                        (fg, bg) = (bg, fg);
                    }

                    var attr = new Terminal.Gui.Attribute(fg, bg);
                    driver.SetAttribute(attr);
                    Move(col, row);

                    char ch = screenChar >= 0x20 ? (char)screenChar : ' ';
                    driver.AddRune(new Rune(ch));
                }
            }
        }

        return true;
    }
}

using System.Text;
using e6502.TUI.Hardware;
using Terminal.Gui;

namespace e6502.TUI.Rendering;

public class DisplayView : View
{
    private readonly VirtualGraphicsController _vgc;
    private readonly object _renderLock = new();
    private bool _cursorVisible = true;

    public DisplayView(VirtualGraphicsController vgc)
    {
        _vgc = vgc;
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

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
    private bool _quoteMode;

    public DisplayView(VirtualGraphicsController vgc, ScreenEditor editor)
    {
        _vgc = vgc;
        _editor = editor;
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
                _quoteMode = false;
                _editor.QueueInput(0x0D);
                return true;

            case KeyCode.Backspace:
                _editor.QueueInput(0x08);
                return true;

            case KeyCode.Esc:
                // CTRL-C interrupt
                _editor.QueueInput(0x03);
                return true;

            default:
                // Printable characters — queue for CPU, EhBASIC echoes via CHAROUT
                if (keyEvent.AsRune.Value >= 0x20 && keyEvent.AsRune.Value <= 0x7E)
                {
                    byte ch = (byte)keyEvent.AsRune.Value;
                    if (ch == '"')
                        _quoteMode = !_quoteMode;
                    // Uppercase outside quotes — EhBASIC keywords are uppercase only
                    if (!_quoteMode && ch >= 0x61 && ch <= 0x7A)
                        ch -= 0x20;
                    _editor.QueueInput(ch);
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

            // Block graphics compositing layer (mode 1 = graphics, 2 = mixed)
            if (_vgc.GetMode() >= 1)
            {
                for (int row = 0; row < VgcConstants.ScreenRows; row++)
                {
                    for (int col = 0; col < VgcConstants.ScreenCols; col++)
                    {
                        // Each terminal cell maps to two vertical gfx pixels (2-wide each)
                        // gfx coords: x = col*2, top y = row*2, bottom y = row*2+1
                        int gx = col * 2;
                        int gyTop    = row * 2;
                        int gyBottom = row * 2 + 1;

                        byte topColor    = _vgc.GetGfxPixelColor(gx, gyTop);
                        byte bottomColor = _vgc.GetGfxPixelColor(gx, gyBottom);

                        bool topSet    = topColor    != 0;
                        bool bottomSet = bottomColor != 0;

                        if (!topSet && !bottomSet)
                            continue; // transparent — text layer shows through

                        Rune glyph;
                        Color cellFg;
                        Color cellBg;

                        if (topSet && bottomSet)
                        {
                            // Both halves set — use upper-half block, fg=top, bg=bottom
                            glyph   = new Rune('▀');
                            cellFg  = ColorPalette.Get(topColor);
                            cellBg  = ColorPalette.Get(bottomColor);
                        }
                        else if (topSet)
                        {
                            // Top only — upper-half block, fg=top, bg=screen bg
                            glyph   = new Rune('▀');
                            cellFg  = ColorPalette.Get(topColor);
                            cellBg  = bgColor;
                        }
                        else
                        {
                            // Bottom only — lower-half block, fg=bottom, bg=screen bg
                            glyph   = new Rune('▄');
                            cellFg  = ColorPalette.Get(bottomColor);
                            cellBg  = bgColor;
                        }

                        var gfxAttr = new Terminal.Gui.Attribute(cellFg, cellBg);
                        driver.SetAttribute(gfxAttr);
                        Move(col, row);
                        driver.AddRune(glyph);
                    }
                }
            }

            // Sprite compositing layer — overlay sprites onto whatever is already rendered.
            // Sprites live in the same block-pixel coordinate space as the gfx bitmap (160×50).
            // Each terminal cell is 2 pixels wide × 2 pixels tall (upper/lower half-block).
            // We build a per-cell overlay: top-pixel color and bottom-pixel color from sprites.
            {
                // sparse overlay: cell key = row * ScreenCols + col
                var spriteTopColor    = new Dictionary<int, byte>();
                var spriteBottomColor = new Dictionary<int, byte>();

                for (int si = 0; si < VgcConstants.MaxSprites; si++)
                {
                    var pixels = SpriteRenderer.GetSpritePixels(_vgc, si);
                    foreach (var sp in pixels)
                    {
                        // sprite pixel (sp.ScreenX, sp.ScreenY) is in block-pixel space.
                        // terminal col = sp.ScreenX / 2, terminal row = sp.ScreenY / 2.
                        // top half = even pixel rows, bottom half = odd pixel rows.
                        int termCol = sp.ScreenX / 2;
                        int termRow = sp.ScreenY / 2;
                        bool isBottom = (sp.ScreenY & 1) == 1;

                        if (termCol < 0 || termCol >= VgcConstants.ScreenCols ||
                            termRow < 0 || termRow >= VgcConstants.ScreenRows)
                            continue;

                        int key = termRow * VgcConstants.ScreenCols + termCol;
                        if (isBottom)
                            spriteBottomColor[key] = sp.Color;
                        else
                            spriteTopColor[key] = sp.Color;
                    }
                }

                // Render all cells that have at least one sprite pixel.
                var allCells = new HashSet<int>(spriteTopColor.Keys);
                allCells.UnionWith(spriteBottomColor.Keys);

                foreach (int key in allCells)
                {
                    int termRow = key / VgcConstants.ScreenCols;
                    int termCol = key % VgcConstants.ScreenCols;

                    bool hasTop    = spriteTopColor.TryGetValue(key, out byte sTop);
                    bool hasBottom = spriteBottomColor.TryGetValue(key, out byte sBottom);

                    Rune glyph;
                    Color cellFg;
                    Color cellBg;

                    if (hasTop && hasBottom)
                    {
                        glyph   = new Rune('▀');
                        cellFg  = ColorPalette.Get(sTop);
                        cellBg  = ColorPalette.Get(sBottom);
                    }
                    else if (hasTop)
                    {
                        glyph   = new Rune('▀');
                        cellFg  = ColorPalette.Get(sTop);
                        cellBg  = bgColor;
                    }
                    else
                    {
                        glyph   = new Rune('▄');
                        cellFg  = ColorPalette.Get(sBottom);
                        cellBg  = bgColor;
                    }

                    var sprAttr = new Terminal.Gui.Attribute(cellFg, cellBg);
                    driver.SetAttribute(sprAttr);
                    Move(termCol, termRow);
                    driver.AddRune(glyph);
                }

                // Collision detection — write results back to VGC registers.
                var (ss, sb) = SpriteRenderer.DetectCollisions(_vgc);
                _vgc.SetCollisionRegisters(ss, sb);
            }
        }

        return true;
    }
}

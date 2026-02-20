using Avalonia;
using Avalonia.Controls;
using Avalonia.Input;
using Avalonia.Media;
using Avalonia.Media.Imaging;
using e6502.Avalonia.Hardware;
using e6502.Avalonia.Input;

namespace e6502.Avalonia.Rendering;

public class EmulatorCanvas : Control
{
    public const int NativeWidth = 640;
    public const int NativeHeight = 400;

    private readonly WriteableBitmap _framebuffer;
    private readonly VirtualGraphicsController _vgc;
    private readonly BitmapFont _font;
    private readonly ScreenEditor _editor;
    private readonly object _renderLock = new();
    private bool _cursorVisible = true;
    private bool _quoteMode;

    public EmulatorCanvas(VirtualGraphicsController vgc, BitmapFont font, ScreenEditor editor)
    {
        _vgc = vgc;
        _font = font;
        _editor = editor;
        Focusable = true;
        _framebuffer = new WriteableBitmap(
            new PixelSize(NativeWidth, NativeHeight),
            new Vector(96, 96),
            global::Avalonia.Platform.PixelFormat.Bgra8888,
            global::Avalonia.Platform.AlphaFormat.Opaque);
    }

    public void ToggleCursor()
    {
        lock (_renderLock) { _cursorVisible = !_cursorVisible; }
        InvalidateVisual();
    }

    public void RequestRedraw() => InvalidateVisual();

    protected override void OnKeyDown(KeyEventArgs e)
    {
        if ((e.KeyModifiers.HasFlag(KeyModifiers.Control) || e.KeyModifiers.HasFlag(KeyModifiers.Meta)) && e.Key == Key.V
            || (e.KeyModifiers.HasFlag(KeyModifiers.Shift) && e.Key == Key.Insert))
        {
            _ = PasteClipboardAsync();
            e.Handled = true;
            base.OnKeyDown(e);
            return;
        }

        switch (e.Key)
        {
            case Key.Left:
                _editor.CursorLeft();
                e.Handled = true;
                break;
            case Key.Right:
                _editor.CursorRight();
                e.Handled = true;
                break;
            case Key.Up:
                _editor.CursorUp();
                e.Handled = true;
                break;
            case Key.Down:
                _editor.CursorDown();
                e.Handled = true;
                break;
            case Key.Enter:
                _quoteMode = false;
                _editor.QueueInput(0x0D);
                e.Handled = true;
                break;
            case Key.Back:
                _editor.QueueInput(0x08);
                e.Handled = true;
                break;
            case Key.Escape:
                _editor.QueueInput(0x03);
                e.Handled = true;
                break;
            default:
                if (e.KeySymbol is { Length: 1 } s && s[0] >= 0x20 && s[0] <= 0x7E)
                {
                    QueuePrintableChar((byte)s[0]);
                    e.Handled = true;
                }
                break;
        }
        base.OnKeyDown(e);
    }

    private async Task PasteClipboardAsync()
    {
        var clipboard = TopLevel.GetTopLevel(this)?.Clipboard;
        if (clipboard is null)
            return;

        string? text = await global::Avalonia.Input.Platform.ClipboardExtensions.TryGetTextAsync(clipboard);
        if (string.IsNullOrEmpty(text))
            return;

        for (int i = 0; i < text.Length; i++)
        {
            char ch = text[i];

            // Normalize CR/LF input into a single BASIC Enter key event.
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

    private void QueuePrintableChar(byte ch)
    {
        if (ch == '"')
            _quoteMode = !_quoteMode;
        if (!_quoteMode && ch >= 0x61 && ch <= 0x7A)
            ch -= 0x20;
        _editor.QueueInput(ch);
    }

    public override void Render(DrawingContext context)
    {
        lock (_renderLock)
        {
            RenderFramebuffer();
        }

        var bounds = Bounds;
        const double pad = 4;
        var destRect = new Rect(pad, pad, bounds.Width - pad * 2, bounds.Height - pad * 2);
        var srcRect = new Rect(0, 0, NativeWidth, NativeHeight);

        using (context.PushRenderOptions(new RenderOptions
        {
            BitmapInterpolationMode = BitmapInterpolationMode.None
        }))
        {
            context.DrawImage(_framebuffer, srcRect, destRect);
        }
    }

    private unsafe void RenderFramebuffer()
    {
        using var fb = _framebuffer.Lock();
        var ptr = (uint*)fb.Address;
        int stride = fb.RowBytes / 4;

        uint bgColor = ColorPalette.GetBgra(_vgc.GetBgColor());

        // Pass 1: Fill background
        for (int i = 0; i < NativeWidth * NativeHeight; i++)
            ptr[i] = bgColor;

        byte mode = _vgc.GetMode();

        // Pass 2: Priority-0 sprites (behind everything)
        RenderSpritesAtPriority(ptr, stride, VgcConstants.SpritePriBehindAll);

        if (mode == 2)
        {
            // Mode 2: Text on top of graphics (text bg is transparent)
            RenderGraphics(ptr, stride);
            RenderSpritesAtPriority(ptr, stride, VgcConstants.SpritePriBetween);
            RenderText(ptr, stride, bgColor, transparentBg: true);
        }
        else
        {
            // Mode 0/1: Graphics on top of text (text only when mode 0)
            RenderText(ptr, stride, bgColor);
            RenderSpritesAtPriority(ptr, stride, VgcConstants.SpritePriBetween);
            if (mode >= 1)
                RenderGraphics(ptr, stride);
        }

        // Priority-2 sprites (in front of everything)
        RenderSpritesAtPriority(ptr, stride, VgcConstants.SpritePriInFront);

        // Collision detection
        var (ss, sb) = SpriteRenderer.DetectCollisions(_vgc);
        _vgc.SetCollisionRegisters(ss, sb);
    }

    private unsafe void RenderText(uint* ptr, int stride, uint bgColor, bool transparentBg = false)
    {
        int cx = _vgc.GetCursorX();
        int cy = _vgc.GetCursorY();

        for (int row = 0; row < VgcConstants.ScreenRows; row++)
        {
            for (int col = 0; col < VgcConstants.ScreenCols; col++)
            {
                byte ch = _vgc.GetScreenChar(col, row);
                byte colorIdx = _vgc.GetScreenColor(col, row);

                uint fg = ColorPalette.GetBgra(colorIdx);
                uint bg = bgColor;

                bool isCursor = _cursorVisible && _vgc.IsCursorEnabled && col == cx && row == cy;
                if (isCursor) (fg, bg) = (bg, fg);

                int cellX = col * BitmapFont.GlyphWidth;
                int cellY = row * BitmapFont.GlyphHeight * 2;

                for (int gy = 0; gy < BitmapFont.GlyphHeight; gy++)
                {
                    byte rowBits = _font.GetRow(ch, gy);
                    int py = cellY + gy * 2;
                    for (int gx = 0; gx < BitmapFont.GlyphWidth; gx++)
                    {
                        int px = cellX + gx;
                        bool set = (rowBits & (0x80 >> gx)) != 0;
                        if (transparentBg && !set && !isCursor) continue;
                        uint c = set ? fg : bg;
                        ptr[py * stride + px] = c;
                        ptr[(py + 1) * stride + px] = c;
                    }
                }
            }
        }
    }

    private unsafe void RenderGraphics(uint* ptr, int stride)
    {
        for (int gy = 0; gy < VgcConstants.GfxHeight; gy++)
        {
            for (int gx = 0; gx < VgcConstants.GfxWidth; gx++)
            {
                byte color = _vgc.GetGfxPixelColor(gx, gy);
                if (color == 0) continue;

                uint pixel = ColorPalette.GetBgra(color);
                int baseX = gx * 2;
                int baseY = gy * 2;

                for (int dy = 0; dy < 2; dy++)
                    for (int dx = 0; dx < 2; dx++)
                        ptr[(baseY + dy) * stride + (baseX + dx)] = pixel;
            }
        }
    }

    private unsafe void RenderSpritesAtPriority(uint* ptr, int stride, int priority)
    {
        for (int i = 0; i < VgcConstants.MaxSprites; i++)
        {
            var state = _vgc.GetSpriteState(i);
            if (!state.enabled || state.priority != priority) continue;

            var pixels = SpriteRenderer.GetSpritePixels(_vgc, i);
            foreach (var sp in pixels)
            {
                int baseX = sp.ScreenX * 2;
                int baseY = sp.ScreenY * 2;
                uint color = ColorPalette.GetBgra(sp.Color);

                for (int dy = 0; dy < 2; dy++)
                {
                    int py = baseY + dy;
                    if (py < 0 || py >= NativeHeight) continue;
                    for (int dx = 0; dx < 2; dx++)
                    {
                        int px = baseX + dx;
                        if (px < 0 || px >= NativeWidth) continue;
                        ptr[py * stride + px] = color;
                    }
                }
            }
        }
    }
}

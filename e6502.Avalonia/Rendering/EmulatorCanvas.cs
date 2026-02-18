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
    public const int NativeHeight = 200;

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
                    byte ch = (byte)s[0];
                    if (ch == '"')
                        _quoteMode = !_quoteMode;
                    if (!_quoteMode && ch >= 0x61 && ch <= 0x7A)
                        ch -= 0x20;
                    _editor.QueueInput(ch);
                    e.Handled = true;
                }
                break;
        }
        base.OnKeyDown(e);
    }

    public override void Render(DrawingContext context)
    {
        lock (_renderLock)
        {
            RenderFramebuffer();
        }

        var bounds = Bounds;
        double scale = Math.Min(bounds.Width / NativeWidth, bounds.Height / NativeHeight);
        double w = NativeWidth * scale;
        double h = NativeHeight * scale;
        double x = (bounds.Width - w) / 2;
        double y = (bounds.Height - h) / 2;

        var destRect = new Rect(x, y, w, h);
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

        // Pass 2: Priority-0 sprites (behind everything)
        RenderSpritesAtPriority(ptr, stride, VgcConstants.SpritePriBehindAll);

        // Pass 3: Text layer
        RenderText(ptr, stride, bgColor);

        // Pass 4: Priority-1 sprites (between text and graphics)
        RenderSpritesAtPriority(ptr, stride, VgcConstants.SpritePriBetween);

        // Pass 5: Graphics bitmap
        if (_vgc.GetMode() >= 1)
            RenderGraphics(ptr, stride);

        // Pass 6: Priority-2 sprites (in front of everything)
        RenderSpritesAtPriority(ptr, stride, VgcConstants.SpritePriInFront);

        // Collision detection
        var (ss, sb) = SpriteRenderer.DetectCollisions(_vgc);
        _vgc.SetCollisionRegisters(ss, sb);
    }

    private unsafe void RenderText(uint* ptr, int stride, uint bgColor)
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

                bool isCursor = _cursorVisible && col == cx && row == cy;
                if (isCursor) (fg, bg) = (bg, fg);

                int cellX = col * BitmapFont.GlyphWidth;
                int cellY = row * BitmapFont.GlyphHeight;

                for (int gy = 0; gy < BitmapFont.GlyphHeight; gy++)
                {
                    byte rowBits = _font.GetRow(ch, gy);
                    int py = cellY + gy;
                    for (int gx = 0; gx < BitmapFont.GlyphWidth; gx++)
                    {
                        int px = cellX + gx;
                        bool set = (rowBits & (0x80 >> gx)) != 0;
                        ptr[py * stride + px] = set ? fg : bg;
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
                int baseX = gx * 4;
                int baseY = gy * 4;

                for (int dy = 0; dy < 4; dy++)
                    for (int dx = 0; dx < 4; dx++)
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
                int baseX = sp.ScreenX * 4;
                int baseY = sp.ScreenY * 4;
                uint color = ColorPalette.GetBgra(sp.Color);

                for (int dy = 0; dy < 4; dy++)
                {
                    int py = baseY + dy;
                    if (py < 0 || py >= NativeHeight) continue;
                    for (int dx = 0; dx < 4; dx++)
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

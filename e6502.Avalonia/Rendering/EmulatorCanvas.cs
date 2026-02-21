using Avalonia;
using Avalonia.Controls;
using Avalonia.Input;
using Avalonia.Media;
using Avalonia.Media.Imaging;
using Avalonia.Threading;
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
    private static readonly uint[] Palette = BuildPalette();
    private readonly byte[] _spriteMapBehind = new byte[VgcConstants.GfxWidth * VgcConstants.GfxHeight];
    private readonly byte[] _spriteMapBetween = new byte[VgcConstants.GfxWidth * VgcConstants.GfxHeight];
    private readonly byte[] _spriteMapFront = new byte[VgcConstants.GfxWidth * VgcConstants.GfxHeight];

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
                _ = QueueEnterAsync();
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
        }
        base.OnKeyDown(e);
    }

    protected override void OnTextInput(TextInputEventArgs e)
    {
        if (string.IsNullOrEmpty(e.Text))
        {
            base.OnTextInput(e);
            return;
        }

        bool handled = false;
        foreach (char ch in e.Text)
        {
            if (ch < 0x20 || ch > 0x7E)
                continue;
            QueuePrintableChar((byte)ch);
            handled = true;
        }

        if (handled)
            e.Handled = true;

        base.OnTextInput(e);
    }

    private async Task QueueEnterAsync()
    {
        // Ensure pending text-input events are delivered before CR, then
        // wait briefly for queued bytes to drain so Enter cannot overtake text.
        await Dispatcher.UIThread.InvokeAsync(() => { }, DispatcherPriority.Background);
        await Task.Delay(4);
        await Dispatcher.UIThread.InvokeAsync(() => { }, DispatcherPriority.Background);
        await WaitForInputIdleAsync(maxWaitMs: 250);
        _quoteMode = false;
        _editor.QueueInput(0x0D);
    }

    private async Task PasteClipboardAsync()
    {
        var clipboard = TopLevel.GetTopLevel(this)?.Clipboard;
        if (clipboard is null)
            return;

        string? text = await global::Avalonia.Input.Platform.ClipboardExtensions.TryGetTextAsync(clipboard);
        if (string.IsNullOrEmpty(text))
            return;

        // Start only when BASIC is accepting line input.
        await WaitForInputIdleAsync(maxWaitMs: 3000);

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
                await WaitForInputIdleAsync(maxWaitMs: 8000);
                continue;
            }

            if (ch == '\t')
                ch = ' ';

            if (ch >= 0x20 && ch <= 0x7E)
            {
                QueuePrintableChar((byte)ch);
            }
        }
    }

    private async Task WaitForInputIdleAsync(int maxWaitMs)
    {
        // Fast-safe gate: next line only after BASIC consumed queued bytes
        // and is back in prompt/input mode.
        int waited = 0;
        while ((_editor.HasQueuedInput || !_vgc.IsCursorEnabled) && waited < maxWaitMs)
        {
            await Task.Delay(1);
            waited++;
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

        BuildSpritePriorityMaps();

        ReadOnlySpan<VirtualGraphicsController.CopperEvent> copperProgram = _vgc.GetCopperProgram();
        bool copperEnabled = _vgc.IsCopperEnabled && !copperProgram.IsEmpty;
        int copperIndex = 0;

        var state = RenderVideoState.FromVgc(_vgc);
        int cursorX = _vgc.GetCursorX();
        int cursorY = _vgc.GetCursorY();
        bool cursorEnabled = _cursorVisible && _vgc.IsCursorEnabled;

        for (int y = 0; y < VgcConstants.GfxHeight; y++)
        {
            for (int x = 0; x < VgcConstants.GfxWidth; x++)
            {
                if (copperEnabled)
                {
                    int position = y * VgcConstants.GfxWidth + x;
                    while (copperIndex < copperProgram.Length && copperProgram[copperIndex].Position == position)
                    {
                        state.Apply(copperProgram[copperIndex].RegisterIndex, copperProgram[copperIndex].Value);
                        copperIndex++;
                    }
                }

                int logicalIndex = y * VgcConstants.GfxWidth + x;
                byte spriteBehind = _spriteMapBehind[logicalIndex];
                byte spriteBetween = _spriteMapBetween[logicalIndex];
                byte spriteFront = _spriteMapFront[logicalIndex];

                int sampleGfxX = Wrap320(x + state.ScrollX);
                int sampleGfxY = Wrap200(y + state.ScrollY);
                byte gfxColorIndex = _vgc.GetGfxPixelColor(sampleGfxX, sampleGfxY);
                uint gfxPixel = gfxColorIndex == 0 ? 0u : Palette[gfxColorIndex & 0x0F];

                for (int dy = 0; dy < 2; dy++)
                {
                    int py = y * 2 + dy;
                    int rowBase = py * stride;
                    for (int dx = 0; dx < 2; dx++)
                    {
                        int px = x * 2 + dx;
                        uint pixel = Palette[state.BgColor & 0x0F];

                        if (spriteBehind != 0)
                            pixel = Palette[spriteBehind & 0x0F];

                        bool textOpaque = TrySampleTextPixel(px, py, state, cursorX, cursorY, cursorEnabled, out uint textPixel);
                        if (state.Mode == 2)
                        {
                            if (gfxPixel != 0)
                                pixel = gfxPixel;
                            if (spriteBetween != 0)
                                pixel = Palette[spriteBetween & 0x0F];
                            if (textOpaque)
                                pixel = textPixel;
                        }
                        else
                        {
                            if (textOpaque)
                                pixel = textPixel;
                            if (spriteBetween != 0)
                                pixel = Palette[spriteBetween & 0x0F];
                            if (state.Mode >= 1 && gfxPixel != 0)
                                pixel = gfxPixel;
                        }

                        if (spriteFront != 0)
                            pixel = Palette[spriteFront & 0x0F];

                        ptr[rowBase + px] = pixel;
                    }
                }
            }
        }

        // Collision detection
        var (ss, sb) = SpriteRenderer.DetectCollisions(_vgc);
        _vgc.SetCollisionRegisters(ss, sb);
    }

    private void BuildSpritePriorityMaps()
    {
        Array.Clear(_spriteMapBehind);
        Array.Clear(_spriteMapBetween);
        Array.Clear(_spriteMapFront);

        for (int i = 0; i < VgcConstants.MaxSprites; i++)
        {
            var spriteState = _vgc.GetSpriteState(i);
            if (!spriteState.enabled)
                continue;

            byte[] target = spriteState.priority switch
            {
                VgcConstants.SpritePriBehindAll => _spriteMapBehind,
                VgcConstants.SpritePriBetween => _spriteMapBetween,
                _ => _spriteMapFront
            };

            foreach (var pixel in SpriteRenderer.GetSpritePixels(_vgc, i))
            {
                if ((uint)pixel.ScreenX >= VgcConstants.GfxWidth || (uint)pixel.ScreenY >= VgcConstants.GfxHeight)
                    continue;

                target[pixel.ScreenY * VgcConstants.GfxWidth + pixel.ScreenX] = (byte)(pixel.Color & 0x0F);
            }
        }
    }

    private bool TrySampleTextPixel(
        int px,
        int py,
        RenderVideoState state,
        int cursorX,
        int cursorY,
        bool cursorEnabled,
        out uint pixel)
    {
        int srcPx = Wrap640(px + (state.ScrollX << 1));
        int srcPy = Wrap400(py + (state.ScrollY << 1));

        int col = srcPx / BitmapFont.GlyphWidth;
        int row = srcPy / (BitmapFont.GlyphHeight * 2);

        byte ch = _vgc.GetScreenChar(col, row);
        byte fgColor = _vgc.GetScreenColor(col, row);

        uint fg = Palette[fgColor & 0x0F];
        uint bg = Palette[state.BgColor & 0x0F];

        bool isCursor = cursorEnabled && col == cursorX && row == cursorY;
        if (isCursor)
            (fg, bg) = (bg, fg);

        int glyphX = srcPx % BitmapFont.GlyphWidth;
        int glyphY = (srcPy % (BitmapFont.GlyphHeight * 2)) / 2;
        byte rowBits = _font.GetRow(ch, glyphY);
        bool set = (rowBits & (0x80 >> glyphX)) != 0;

        if (state.Mode == 2 && !set && !isCursor)
        {
            pixel = 0;
            return false;
        }

        pixel = set ? fg : bg;
        return true;
    }

    private static int Wrap320(int value)
    {
        if (value >= VgcConstants.GfxWidth) value -= VgcConstants.GfxWidth;
        if (value >= VgcConstants.GfxWidth) value -= VgcConstants.GfxWidth;
        return value;
    }

    private static int Wrap200(int value)
    {
        if (value >= VgcConstants.GfxHeight) value -= VgcConstants.GfxHeight;
        if (value >= VgcConstants.GfxHeight) value -= VgcConstants.GfxHeight;
        return value;
    }

    private static int Wrap640(int value)
    {
        if (value >= NativeWidth) value -= NativeWidth;
        return value;
    }

    private static int Wrap400(int value)
    {
        if (value >= NativeHeight) value -= NativeHeight;
        if (value >= NativeHeight) value -= NativeHeight;
        return value;
    }

    private static uint[] BuildPalette()
    {
        var palette = new uint[16];
        for (byte i = 0; i < 16; i++)
            palette[i] = ColorPalette.GetBgra(i);
        return palette;
    }

    private struct RenderVideoState
    {
        public byte Mode;
        public int ScrollX;
        public int ScrollY;
        public byte BgColor;

        public static RenderVideoState FromVgc(VirtualGraphicsController vgc) =>
            new()
            {
                Mode = vgc.GetMode(),
                ScrollX = vgc.GetScrollX(),
                ScrollY = vgc.GetScrollY(),
                BgColor = vgc.GetBgColor()
            };

        public void Apply(byte registerIndex, byte value)
        {
            switch (registerIndex)
            {
                case VgcConstants.RegMode - VgcConstants.VgcBase:
                    Mode = value;
                    break;
                case VgcConstants.RegBgCol - VgcConstants.VgcBase:
                    BgColor = (byte)(value & 0x0F);
                    break;
                case VgcConstants.RegScrollX - VgcConstants.VgcBase:
                    ScrollX = value;
                    break;
                case VgcConstants.RegScrollY - VgcConstants.VgcBase:
                    ScrollY = value;
                    break;
            }
        }
    }
}

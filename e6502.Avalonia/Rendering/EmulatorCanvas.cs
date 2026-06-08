using Avalonia;
using Avalonia.Controls;
using Avalonia.Input;
using Avalonia.Media;
using Avalonia.Media.Imaging;
using Avalonia.Threading;
using e6502.Avalonia;
using e6502.Avalonia.Editor;
using e6502.Avalonia.Hardware;
using e6502.Avalonia.Input;

namespace e6502.Avalonia.Rendering;

public class EmulatorCanvas : Control
{
    public const int NativeWidth = VgcConstants.VideoWidth;
    public const int NativeHeight = VgcConstants.VideoHeight;

    private readonly WriteableBitmap _framebuffer;
    private readonly VirtualGraphicsController _vgc;
    private readonly BitmapFont _font;
    private readonly ScreenEditor _editor;
    private readonly CompositeBusDevice? _boardInput;   // null in headless/test contexts
    private readonly object _renderLock = new();
    private bool _cursorVisible = true;
    private readonly byte[] _lineBehind = new byte[VgcConstants.SpritePlaneWidth];
    private readonly byte[] _lineBetween = new byte[VgcConstants.SpritePlaneWidth];
    private readonly byte[] _lineFront = new byte[VgcConstants.SpritePlaneWidth];
    private readonly ushort[] _spriteMask = new ushort[VgcConstants.SpritePlaneWidth];
    private readonly byte[] _shapeRamSnapshot = new byte[VgcConstants.ShapeRamSize];
    private bool _shapeRamInitialized;

    private readonly List<ScreenTextEditor> _editors = new();

    /// <summary>Register an editor for keyboard routing. The first active, non-Running editor receives input.</summary>
    public void AddEditor(ScreenTextEditor editor) => _editors.Add(editor);


    public EmulatorCanvas(VirtualGraphicsController vgc, BitmapFont font, ScreenEditor editor,
        CompositeBusDevice? boardInput = null)
    {
        _vgc = vgc;
        _font = font;
        _editor = editor;
        _boardInput = boardInput;
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

    public unsafe void SaveScreenshot(string path)
    {
        var dir = Path.GetDirectoryName(path);
        if (!string.IsNullOrEmpty(dir))
            Directory.CreateDirectory(dir);

        using var snapshot = new WriteableBitmap(
            new PixelSize(NativeWidth, NativeHeight),
            new Vector(96, 96),
            global::Avalonia.Platform.PixelFormat.Bgra8888,
            global::Avalonia.Platform.AlphaFormat.Opaque);

        lock (_renderLock)
        {
            RenderFramebuffer();
            using var src = _framebuffer.Lock();
            using var dst = snapshot.Lock();
            Buffer.MemoryCopy(
                (void*)src.Address,
                (void*)dst.Address,
                (long)dst.RowBytes * NativeHeight,
                (long)src.RowBytes * NativeHeight);
        }

        using var fs = File.Create(path);
        snapshot.Save(fs);
    }

    protected override void OnKeyDown(KeyEventArgs e)
    {
        // Mirror joystick/fire keys into the board-input register ($BA9C) for
        // JOY(). Additive — never consumes the event, so arrows still move the
        // editor cursor and Z/X still type. KeyUp clears the bit.
        if (_boardInput != null && BoardInputKeyMap.TryMap(e.Key, out byte downBit))
            _boardInput.BoardButtonState |= downBit;

        // Route to the first active editor that is not in Running mode
        var activeEditor = _editors.FirstOrDefault(ed => ed.IsActive && ed.Mode != EditorMode.Running);
        if (activeEditor != null)
        {
            if (activeEditor.HandleKeyDown(e.Key, e.KeyModifiers))
                e.Handled = true;
            base.OnKeyDown(e);
            return;
        }

        // F5: activate BASIC editor (only when NCC editor is not active)
        var nccEditor = _editors.OfType<NccEditor>().FirstOrDefault();
        var basicEditor = _editors.OfType<BasicEditor>().FirstOrDefault();
        if (e.Key == Key.F5 && nccEditor is not { IsActive: true })
        {
            basicEditor?.ToggleActivation();
            e.Handled = true;
            base.OnKeyDown(e);
            return;
        }

        if ((e.KeyModifiers.HasFlag(KeyModifiers.Control) || e.KeyModifiers.HasFlag(KeyModifiers.Meta)) && e.Key == Key.V
            || (e.KeyModifiers.HasFlag(KeyModifiers.Shift) && e.Key == Key.Insert))
        {
            _ = PasteClipboardAsync();
            e.Handled = true;
            base.OnKeyDown(e);
            return;
        }

        if (e.KeyModifiers.HasFlag(KeyModifiers.Alt) && TryMapAltMenuKey(e.Key, out byte altKey))
        {
            _editor.QueueInput(0x1B);
            _editor.QueueInput(altKey);
            e.Handled = true;
            base.OnKeyDown(e);
            return;
        }

        // Check for font keymap graphic character input
        if (e.Key >= Key.A && e.Key <= Key.Z)
        {
            int fontIndex = _vgc.GetFontIndex();
            var keymap = _font.GetKeymap(fontIndex);
            KeyMod mod = KeyMod.None;
            if (e.KeyModifiers.HasFlag(KeyModifiers.Shift))
                mod = KeyMod.Shift;
            else if (e.KeyModifiers.HasFlag(KeyModifiers.Control) || e.KeyModifiers.HasFlag(KeyModifiers.Meta))
                mod = KeyMod.Ctrl;

            if (mod != KeyMod.None && keymap.TryMap(mod, (char)('A' + (e.Key - Key.A)), out byte ch))
            {
                _editor.QueueInput(ch);
                e.Handled = true;
                base.OnKeyDown(e);
                return;
            }
        }

        // Ctrl/Meta + letter the font keymap didn't claim -> ASCII control code
        // (Ctrl-S=Save 0x13, Ctrl-Q=Quit 0x11, ...). Without this it falls through
        // to OnTextInput and inserts the literal letter.
        if (ControlKeyMap.TryMap(e.Key, e.KeyModifiers, out byte ctrlCode))
        {
            _editor.QueueInput(ctrlCode);
            e.Handled = true;
            base.OnKeyDown(e);
            return;
        }

        switch (e.Key)
        {
            case Key.Left:
                _editor.QueueInput(28);
                e.Handled = true;
                break;
            case Key.Right:
                _editor.QueueInput(29);
                e.Handled = true;
                break;
            case Key.Up:
                _editor.QueueInput(30);
                e.Handled = true;
                break;
            case Key.Down:
                _editor.QueueInput(31);
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

    protected override void OnKeyUp(KeyEventArgs e)
    {
        // Release the mirrored joystick/fire bit (see OnKeyDown). Additive.
        if (_boardInput != null && BoardInputKeyMap.TryMap(e.Key, out byte upBit))
            _boardInput.BoardButtonState &= (byte)~upBit;
        base.OnKeyUp(e);
    }

    protected override void OnTextInput(TextInputEventArgs e)
    {
        // Route to the first active editor that is not in Running mode
        var activeEditor = _editors.FirstOrDefault(ed => ed.IsActive && ed.Mode != EditorMode.Running);
        if (activeEditor != null)
        {
            if (!string.IsNullOrEmpty(e.Text))
            {
                activeEditor.HandleTextInput(e.Text);
                e.Handled = true;
            }
            base.OnTextInput(e);
            return;
        }

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
        _editor.QueueInput(ch);
    }

    private static bool TryMapAltMenuKey(Key key, out byte ch)
    {
        ch = 0;
        if (key is < Key.A or > Key.Z)
            return false;

        ch = (byte)('a' + (key - Key.A));
        return true;
    }

    public override void Render(DrawingContext context)
    {
        if (!UiTransitionGate.IsPaused)
        {
            lock (_renderLock)
            {
                RenderFramebuffer();
            }
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

        ReadOnlySpan<VirtualGraphicsController.CopperEvent> copperProgram = _vgc.GetCopperProgram();
        bool copperEnabled = _vgc.IsCopperEnabled && !copperProgram.IsEmpty;
        int copperIndex = 0;

        var state = RenderVideoState.FromVgc(_vgc);
        ReadOnlySpan<uint> palette = ColorPalette.GetBgraPalette(state.PaletteMode);
        uint borderPixel = palette[state.BorderColor & 0x0F];
        if (state.DisplayDim != 15)
            borderPixel = DimColor(borderPixel, state.DisplayDim);

        for (int py = 0; py < NativeHeight; py++)
        {
            int rowBase = py * stride;
            for (int px = 0; px < NativeWidth; px++)
                ptr[rowBase + px] = borderPixel;
        }

        var sprites = SpriteRenderState.FromVgc(_vgc);
        if (_vgc.SnapshotSpriteShapes(_shapeRamSnapshot) || !_shapeRamInitialized)
            _shapeRamInitialized = true;
        ReadOnlySpan<byte> shapeRam = _shapeRamSnapshot;
        int cursorX = _vgc.GetCursorX();
        int cursorY = _vgc.GetCursorY();
        bool cursorEnabled = _cursorVisible && _vgc.IsCursorVisibleInCurrentMode;
        byte gfxTransparentColor = _vgc.GetGfxTransparentColor();

        ushort colSS = 0, colSB = 0;

        for (int y = 0; y < VgcConstants.GfxHeight; y++)
        {
            // Pre-fire copper events targeting sprite registers for this scanline
            if (copperEnabled)
            {
                int scanlineEnd = (y + 1) * VgcConstants.GfxWidth;
                int peekIndex = copperIndex;
                while (peekIndex < copperProgram.Length && copperProgram[peekIndex].Position < scanlineEnd)
                {
                    if (SpriteRenderState.IsSpriteRegister(copperProgram[peekIndex].RegisterIndex))
                        sprites.Apply(copperProgram[peekIndex].RegisterIndex, copperProgram[peekIndex].Value);
                    peekIndex++;
                }
            }

            // Rasterize sprites for this scanline. Sprite coordinates match
            // the 320x200 Nova graphics canvas; hardware border pixels are
            // outside the sprite-visible area.
            int spritePlaneY = y + VgcConstants.SpriteCanvasY;
            SpriteRenderer.RasterizeScanline(spritePlaneY, sprites, shapeRam,
                _lineBehind, _lineBetween, _lineFront, _spriteMask);

            // Accumulate collision data
            SpriteRenderer.AccumulateCollisions(
                _spriteMask.AsSpan(VgcConstants.SpriteCanvasX, VgcConstants.GfxWidth),
                _vgc,
                state.GfxScrollX, state.GfxScrollY, y, ref colSS, ref colSB);

            for (int x = 0; x < VgcConstants.GfxWidth; x++)
            {
                // Fire non-sprite copper events at exact pixel position
                if (copperEnabled)
                {
                    int position = y * VgcConstants.GfxWidth + x;
                    while (copperIndex < copperProgram.Length && copperProgram[copperIndex].Position == position)
                    {
                        if (copperProgram[copperIndex].RegisterIndex == VgcConstants.CopperRegIrq)
                            _vgc.RaiseCopperIrq(copperProgram[copperIndex].Value);
                        else if (!SpriteRenderState.IsSpriteRegister(copperProgram[copperIndex].RegisterIndex))
                            state.Apply(copperProgram[copperIndex].RegisterIndex, copperProgram[copperIndex].Value);
                        copperIndex++;
                    }
                }

                int spritePlaneX = x + VgcConstants.SpriteCanvasX;
                byte spriteBehind = _lineBehind[spritePlaneX];
                byte spriteBetween = _lineBetween[spritePlaneX];
                byte spriteFront = _lineFront[spritePlaneX];

                int sampleGfxX = Wrap320(x + state.GfxScrollX);
                int sampleGfxY = Wrap200(y + state.GfxScrollY);
                byte gfxColorIndex = _vgc.GetGfxPixelColor(sampleGfxX, sampleGfxY);
                bool gfxOpaque = gfxColorIndex != gfxTransparentColor;
                uint gfxPixel = gfxOpaque ? palette[gfxColorIndex & 0x0F] : 0u;

                for (int dy = 0; dy < 2; dy++)
                {
                    int canvasPy = y * 2 + dy;
                    int py = VgcConstants.CanvasOffsetY + canvasPy;
                    int rowBase = py * stride;
                    for (int dx = 0; dx < 2; dx++)
                    {
                        int canvasPx = x * 2 + dx;
                        int px = VgcConstants.CanvasOffsetX + canvasPx;
                        uint pixel = palette[state.BgColor & 0x0F];

                        if (spriteBehind != 0)
                            pixel = palette[spriteBehind & 0x0F];

                        bool textOpaque = false;
                        uint textPixel = 0;
                        if (VirtualGraphicsController.IsTextLayerVisible(state.Mode))
                            textOpaque = TrySampleTextPixel(canvasPx, canvasPy, state, cursorX, cursorY, cursorEnabled, palette, out textPixel);

                        if (state.Mode == 3 || state.Mode == 4)
                        {
                            if (gfxOpaque)
                                pixel = gfxPixel;
                            if (spriteBetween != 0)
                                pixel = palette[spriteBetween & 0x0F];
                        }
                        else if (state.Mode == 2)
                        {
                            if (gfxOpaque)
                                pixel = gfxPixel;
                            if (spriteBetween != 0)
                                pixel = palette[spriteBetween & 0x0F];
                            if (textOpaque)
                                pixel = textPixel;
                        }
                        else
                        {
                            if (textOpaque)
                                pixel = textPixel;
                            if (spriteBetween != 0)
                                pixel = palette[spriteBetween & 0x0F];
                            if (state.Mode >= 1 && gfxOpaque)
                                pixel = gfxPixel;
                        }

                        if (spriteFront != 0)
                            pixel = palette[spriteFront & 0x0F];

                        if (state.DisplayDim != 15)
                            pixel = DimColor(pixel, state.DisplayDim);

                        ptr[rowBase + px] = pixel;
                    }
                }
            }
        }

        _vgc.SetCollisionRegisters(colSS, colSB);
    }

    private bool TrySampleTextPixel(
        int px,
        int py,
        RenderVideoState state,
        int cursorX,
        int cursorY,
        bool cursorEnabled,
        ReadOnlySpan<uint> palette,
        out uint pixel)
    {
        if (!TextPixelRenderer.TrySample(
            _vgc,
            _font,
            px,
            py,
            state.Mode,
            state.TextScrollX,
            state.TextScrollY,
            state.BgColor,
            state.FontIndex,
            state.FlashVisible,
            cursorX,
            cursorY,
            cursorEnabled,
            out byte colorIndex))
        {
            pixel = 0;
            return false;
        }

        pixel = palette[colorIndex & 0x0F];
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
        if (value >= VgcConstants.CanvasWidth) value -= VgcConstants.CanvasWidth;
        return value;
    }

    private static int Wrap400(int value)
    {
        if (value >= VgcConstants.CanvasHeight) value -= VgcConstants.CanvasHeight;
        if (value >= VgcConstants.CanvasHeight) value -= VgcConstants.CanvasHeight;
        return value;
    }

    private static uint DimColor(uint bgra, byte dim)
    {
        if (dim == 0)
            return 0xFF000000;
        uint r = ((bgra >> 16) & 0xFF) * dim >> 4;
        uint g = ((bgra >> 8) & 0xFF) * dim >> 4;
        uint b = (bgra & 0xFF) * dim >> 4;
        return 0xFF000000 | (r << 16) | (g << 8) | b;
    }

    private struct RenderVideoState
    {
        public byte Mode;
        public int ScrollX;
        public int ScrollY;
        public byte ScrollCtl;
        public byte BgColor;
        public byte BorderColor;
        public int FontIndex;
        public byte DisplayDim;
        public byte PaletteMode;
        public bool FlashVisible;

        public static RenderVideoState FromVgc(VirtualGraphicsController vgc) =>
            new()
            {
                Mode = vgc.GetMode(),
                ScrollX = vgc.GetScrollX(),
                ScrollY = vgc.GetScrollY(),
                ScrollCtl = vgc.GetScrollCtl(),
                BgColor = vgc.GetBgColor(),
                BorderColor = vgc.GetBorderColor(),
                FontIndex = vgc.GetFontIndex(),
                DisplayDim = vgc.GetDisplayDim(),
                PaletteMode = vgc.GetPaletteMode(),
                FlashVisible = ((Environment.TickCount64 / 500) & 1) == 0
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
                case VgcConstants.RegBorder - VgcConstants.VgcBase:
                    BorderColor = (byte)(value & 0x0F);
                    break;
                case VgcConstants.RegScrollX - VgcConstants.VgcBase:
                    ScrollX = value;
                    break;
                case VgcConstants.RegScrollY - VgcConstants.VgcBase:
                    ScrollY = value;
                    break;
                case VgcConstants.RegScrollCtl - VgcConstants.VgcBase:
                    ScrollCtl = (byte)(value & 0x07);
                    break;
                case VgcConstants.RegFont - VgcConstants.VgcBase:
                    FontIndex = value & 0x07;
                    break;
            }
        }

        public int GfxScrollX => (ScrollCtl & VgcConstants.ScrollCtlGfx) != 0 ? ScrollXFull : 0;
        public int GfxScrollY => (ScrollCtl & VgcConstants.ScrollCtlGfx) != 0 ? ScrollYMod : 0;
        public int TextScrollX => (ScrollCtl & VgcConstants.ScrollCtlText) != 0 ? ScrollXFull : 0;
        public int TextScrollY => (ScrollCtl & VgcConstants.ScrollCtlText) != 0 ? ScrollYMod : 0;

        private int ScrollXFull => NormalizeScrollX(ScrollX | ((ScrollCtl & VgcConstants.ScrollCtlXHigh) != 0 ? 0x100 : 0));
        private int ScrollYMod => NormalizeScrollY(ScrollY);

        private static int NormalizeScrollX(int value) =>
            value >= VgcConstants.GfxWidth ? value - VgcConstants.GfxWidth : value;

        private static int NormalizeScrollY(int value) =>
            value >= VgcConstants.GfxHeight ? value - VgcConstants.GfxHeight : value;
    }
}

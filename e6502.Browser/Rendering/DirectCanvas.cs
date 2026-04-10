// Direct framebuffer renderer for the browser.
// Replicates EmulatorCanvas.RenderFramebuffer() pixel compositing logic
// but writes RGBA bytes to a byte[] exposed to JS via [JSExport].
// JS reads this buffer and pushes it to an HTML5 Canvas with putImageData().

using System.Runtime.InteropServices.JavaScript;
using e6502.Avalonia;
using e6502.Avalonia.Hardware;
using e6502.Avalonia.Input;
using e6502.Avalonia.Rendering;

namespace e6502.Browser.Rendering;

public static partial class DirectCanvas
{
    public const int NativeWidth = 640;
    public const int NativeHeight = 400;
    public const int FramebufferSize = NativeWidth * NativeHeight * 4; // RGBA

    // RGBA framebuffer: 640x400x4 = 1,024,000 bytes
    private static readonly byte[] _framebuffer = new byte[FramebufferSize];

    // Palette pre-computed as RGBA byte quads (R, G, B, A)
    private static readonly uint[] _palette = BuildRgbaPalette();

    // Scratch buffers for sprite rasterization (one scanline at native 320 resolution)
    private static readonly byte[] _lineBehind = new byte[VgcConstants.GfxWidth];
    private static readonly byte[] _lineBetween = new byte[VgcConstants.GfxWidth];
    private static readonly byte[] _lineFront = new byte[VgcConstants.GfxWidth];
    private static readonly ushort[] _spriteMask = new ushort[VgcConstants.GfxWidth];
    private static readonly byte[] _shapeRamSnapshot = new byte[VgcConstants.ShapeRamSize];
    private static bool _shapeRamInitialized;

    // Tile scratch buffers
    private static readonly uint[] _tileLine = new uint[VgcConstants.GfxWidth];
    private static readonly byte[] _tileOpaque = new byte[VgcConstants.GfxWidth];

    // Cursor state
    private static bool _cursorVisible = true;

    // References set at init
    private static VirtualGraphicsController? _vgc;
    private static BitmapFont? _font;

    // CPU references — set at init, used by Tick()
    private static KDS.e6502.Cpu? _cpu;
    private static CompositeBusDevice? _bus;
    private static int _cyclesPerTick;
    private static int _cursorCounter;

    public static void Initialize(VirtualGraphicsController vgc, BitmapFont font,
        KDS.e6502.Cpu cpu, CompositeBusDevice bus, int cpuHz)
    {
        _vgc = vgc;
        _font = font;
        _cpu = cpu;
        _bus = bus;
        _cyclesPerTick = cpuHz / 60;
    }

    public static void ToggleCursor()
    {
        _cursorVisible = !_cursorVisible;
    }

    /// <summary>
    /// Runs one frame of CPU + renders framebuffer. Called from JS requestAnimationFrame.
    /// Single entry point — no timers, no dispatchers.
    /// </summary>
    [JSExport]
    public static byte[] Tick()
    {
        // Run CPU
        if (_cpu != null && _bus != null)
        {
            int remaining = _cyclesPerTick;
            while (remaining > 0)
            {
                int cycles = _cpu.ClocksForNext();
                _cpu.ExecuteNext();
                _bus.AdvanceCycles(cycles);

                if (_bus.SidPlayer.HasPendingCall)
                    _bus.SidPlayer.ExecutePendingCalls(_cpu);
                if (_bus.Timer.IrqPending || _bus.Nic.IrqPending || _bus.ConsumeRasterIrqPending())
                    _cpu.IrqWaiting = true;

                remaining -= cycles;
            }
        }

        // Cursor blink (~2Hz at 60fps)
        _cursorCounter++;
        if (_cursorCounter >= 30)
        {
            _cursorCounter = 0;
            _cursorVisible = !_cursorVisible;
        }

        // Render
        if (_vgc != null && _font != null)
            RenderFramebuffer();

        return _framebuffer;
    }

    private static void RenderFramebuffer()
    {
        var vgc = _vgc!;
        var font = _font!;

        ReadOnlySpan<VirtualGraphicsController.CopperEvent> copperProgram = vgc.GetCopperProgram();
        bool copperEnabled = vgc.IsCopperEnabled && !copperProgram.IsEmpty;
        int copperIndex = 0;

        var state = RenderVideoState.FromVgc(vgc);
        var sprites = SpriteRenderState.FromVgc(vgc);
        if (vgc.SnapshotSpriteShapes(_shapeRamSnapshot) || !_shapeRamInitialized)
            _shapeRamInitialized = true;
        ReadOnlySpan<byte> shapeRam = _shapeRamSnapshot;
        int cursorX = vgc.GetCursorX();
        int cursorY = vgc.GetCursorY();
        bool cursorEnabled = _cursorVisible && vgc.IsCursorEnabled;

        bool tileMode = state.Mode == 4;
        TileRenderState? tiles = tileMode ? TileRenderState.FromVgc(vgc) : null;

        ushort colSS = 0, colSB = 0;
        ushort colTile = 0;

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

            // Rasterize sprites for this scanline
            SpriteRenderer.RasterizeScanline(y, sprites, shapeRam,
                _lineBehind, _lineBetween, _lineFront, _spriteMask);

            // Rasterize tiles for this scanline (Mode 4 only)
            if (tileMode)
                TileRenderer.RasterizeScanline(y, tiles!, _tileLine, _tileOpaque);

            // Accumulate collision data
            SpriteRenderer.AccumulateCollisions(_spriteMask, vgc,
                state.ScrollX, state.ScrollY, y, ref colSS, ref colSB);

            // Sprite-tile collision: any sprite pixel overlapping a non-transparent tile pixel
            if (tileMode)
            {
                for (int cx = 0; cx < VgcConstants.GfxWidth; cx++)
                {
                    if (_spriteMask[cx] != 0 && _tileOpaque[cx] != 0)
                        colTile |= _spriteMask[cx];
                }
            }

            for (int x = 0; x < VgcConstants.GfxWidth; x++)
            {
                // Fire non-sprite copper events at exact pixel position
                if (copperEnabled)
                {
                    int position = y * VgcConstants.GfxWidth + x;
                    while (copperIndex < copperProgram.Length && copperProgram[copperIndex].Position == position)
                    {
                        if (!SpriteRenderState.IsSpriteRegister(copperProgram[copperIndex].RegisterIndex))
                            state.Apply(copperProgram[copperIndex].RegisterIndex, copperProgram[copperIndex].Value);
                        copperIndex++;
                    }
                }

                byte spriteBehind = _lineBehind[x];
                byte spriteBetween = _lineBetween[x];
                byte spriteFront = _lineFront[x];

                int sampleGfxX = Wrap320(x + state.ScrollX);
                int sampleGfxY = Wrap200(y + state.ScrollY);
                byte gfxColorIndex = vgc.GetGfxPixelColor(sampleGfxX, sampleGfxY);
                uint gfxPixel = gfxColorIndex == 0 ? 0u : _palette[gfxColorIndex & 0x0F];

                for (int dy = 0; dy < 2; dy++)
                {
                    int py = y * 2 + dy;
                    int rowBase = py * NativeWidth;
                    for (int dx = 0; dx < 2; dx++)
                    {
                        int px = x * 2 + dx;
                        uint pixel = _palette[state.BgColor & 0x0F];

                        if (spriteBehind != 0)
                            pixel = _palette[spriteBehind & 0x0F];

                        bool textOpaque = false;
                        uint textPixel = 0;
                        if (state.Mode != 3)
                            textOpaque = TrySampleTextPixel(px, py, state, cursorX, cursorY, cursorEnabled, font, vgc, out textPixel);

                        if (tileMode)
                        {
                            // Mode 4: tiles + sprites
                            byte tilePri = _tileOpaque[x];
                            // _tileLine stores ARGB (0xAARRGGBB) from TileRenderState — convert to our RGBA
                            uint tilePixel = ArgbToRgba(_tileLine[x]);

                            // Behind tiles (priority 0)
                            if (tilePri == 1)
                                pixel = tilePixel;
                            if (spriteBetween != 0)
                                pixel = _palette[spriteBetween & 0x0F];
                            // Front tiles (priority 1) go over between-sprites
                            if (tilePri == 2)
                                pixel = tilePixel;
                        }
                        else if (state.Mode == 3)
                        {
                            if (gfxPixel != 0)
                                pixel = gfxPixel;
                            if (spriteBetween != 0)
                                pixel = _palette[spriteBetween & 0x0F];
                        }
                        else if (state.Mode == 2)
                        {
                            if (gfxPixel != 0)
                                pixel = gfxPixel;
                            if (spriteBetween != 0)
                                pixel = _palette[spriteBetween & 0x0F];
                            if (textOpaque)
                                pixel = textPixel;
                        }
                        else
                        {
                            if (textOpaque)
                                pixel = textPixel;
                            if (spriteBetween != 0)
                                pixel = _palette[spriteBetween & 0x0F];
                            if (state.Mode >= 1 && gfxPixel != 0)
                                pixel = gfxPixel;
                        }

                        if (spriteFront != 0)
                            pixel = _palette[spriteFront & 0x0F];

                        // Write RGBA bytes
                        int offset = (rowBase + px) * 4;
                        _framebuffer[offset] = (byte)(pixel >> 24);         // R
                        _framebuffer[offset + 1] = (byte)(pixel >> 16);     // G
                        _framebuffer[offset + 2] = (byte)(pixel >> 8);      // B
                        _framebuffer[offset + 3] = (byte)(pixel);           // A
                    }
                }
            }
        }

        vgc.SetCollisionRegisters(colSS, colSB);
        if (tileMode)
            vgc.SetTileCollision(colTile);
    }

    private static bool TrySampleTextPixel(
        int px,
        int py,
        RenderVideoState state,
        int cursorX,
        int cursorY,
        bool cursorEnabled,
        BitmapFont font,
        VirtualGraphicsController vgc,
        out uint pixel)
    {
        int srcPx = Wrap640(px + (state.ScrollX << 1));
        int srcPy = Wrap400(py + (state.ScrollY << 1));

        int col = srcPx / BitmapFont.GlyphWidth;
        int row = srcPy / (BitmapFont.GlyphHeight * 2);

        byte ch = vgc.GetScreenChar(col, row);
        byte fgColor = vgc.GetScreenColor(col, row);

        uint fg = _palette[fgColor & 0x0F];
        uint bg = _palette[state.BgColor & 0x0F];

        bool isCursor = cursorEnabled && col == cursorX && row == cursorY;
        if (isCursor)
            (fg, bg) = (bg, fg);

        int glyphX = srcPx % BitmapFont.GlyphWidth;
        int glyphY = (srcPy % (BitmapFont.GlyphHeight * 2)) / 2;
        byte rowBits = font.GetRow(state.FontIndex, ch, glyphY);
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

    /// <summary>
    /// Builds palette as RGBA packed uint values.
    /// Layout: R in bits 31-24, G in bits 23-16, B in bits 15-8, A in bits 7-0.
    /// </summary>
    private static uint[] BuildRgbaPalette()
    {
        var palette = new uint[16];
        for (byte i = 0; i < 16; i++)
        {
            var c = ColorPalette.Get(i);
            palette[i] = ((uint)c.R << 24) | ((uint)c.G << 16) | ((uint)c.B << 8) | 0xFF;
        }
        return palette;
    }

    /// <summary>
    /// Converts ARGB uint (0xAARRGGBB) from TileRenderState to our RGBA format (R<<24|G<<16|B<<8|A).
    /// </summary>
    private static uint ArgbToRgba(uint argb)
    {
        byte a = (byte)(argb >> 24);
        byte r = (byte)(argb >> 16);
        byte g = (byte)(argb >> 8);
        byte b = (byte)(argb);
        return ((uint)r << 24) | ((uint)g << 16) | ((uint)b << 8) | a;
    }

    private struct RenderVideoState
    {
        public byte Mode;
        public int ScrollX;
        public int ScrollY;
        public byte BgColor;
        public int FontIndex;

        public static RenderVideoState FromVgc(VirtualGraphicsController vgc) =>
            new()
            {
                Mode = vgc.GetMode(),
                ScrollX = vgc.GetScrollX(),
                ScrollY = vgc.GetScrollY(),
                BgColor = vgc.GetBgColor(),
                FontIndex = vgc.GetFontIndex()
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
                case VgcConstants.RegFont - VgcConstants.VgcBase:
                    FontIndex = value & 0x07;
                    break;
            }
        }
    }
}

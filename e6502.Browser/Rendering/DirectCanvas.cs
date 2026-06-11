// Direct framebuffer renderer for the browser.
// Replicates EmulatorCanvas.RenderFramebuffer() pixel compositing logic
// but writes RGBA bytes to a byte[] exposed to JS via [JSExport].

using System.Runtime.InteropServices.JavaScript;
using System.Runtime.InteropServices;
using System.Diagnostics;
using e6502.Avalonia.Hardware;
using e6502.Avalonia.Rendering;

namespace e6502.Browser.Rendering;

public static partial class DirectCanvas
{
    public const int NativeWidth = VgcConstants.VideoWidth;
    public const int NativeHeight = VgcConstants.VideoHeight;
    public const int FramebufferSize = NativeWidth * NativeHeight * 4;
    private const int TextRowPixelHeight = BitmapFont.GlyphHeight;
    private const int TextRowBytes = VgcConstants.CanvasWidth * TextRowPixelHeight * 4;
    private const int ActiveCanvasBytes = VgcConstants.CanvasWidth * VgcConstants.CanvasHeight * 4;
    private const int DirtyRowFullFrameThreshold = 24;

    private static readonly byte[] _framebuffer = new byte[FramebufferSize];
    private static readonly byte[] _packetBuffer = new byte[FramebufferSize + 1];
    private static readonly byte[] _emptyPacket = { 2 };
    private static readonly uint[] _c64Palette = BuildRgbaPalette(0);
    private static readonly uint[] _egaPalette = BuildRgbaPalette(1);

    private static readonly byte[] _lineBehind = new byte[VgcConstants.SpritePlaneWidth];
    private static readonly byte[] _lineBetween = new byte[VgcConstants.SpritePlaneWidth];
    private static readonly byte[] _lineFront = new byte[VgcConstants.SpritePlaneWidth];
    private static readonly ushort[] _spriteMask = new ushort[VgcConstants.SpritePlaneWidth];
    private static readonly byte[] _shapeRamSnapshot = new byte[VgcConstants.ShapeRamSize];
    private static bool _shapeRamInitialized;
    private static readonly byte[] _lastScreenChars = new byte[VgcConstants.ScreenSize];
    private static readonly byte[] _lastScreenColors = new byte[VgcConstants.ScreenSize];
    private static readonly byte[] _lastScreenAttrs = new byte[VgcConstants.ScreenSize];
    private static readonly bool[] _dirtyRows = new bool[VgcConstants.ScreenRows];
    private static bool _textSnapshotInitialized;
    private static byte _lastTextFontIndex;
    private static byte _lastTextDisplayDim;
    private static byte _lastTextPaletteMode;
    private static byte _lastTextBorderColor;
    private static bool _lastFlashVisible;
    private static int _lastCursorX;
    private static int _lastCursorY;
    private static bool _lastCursorEnabled;

    private static bool _cursorVisible = true;
    private static VirtualGraphicsController? _vgc;
    private static BitmapFont? _font;
    private static KDS.e6502.Cpu? _cpu;
    private static CompositeBusDevice? _bus;
    private static int _promptCpuHz;
    private static int _programCpuHz;
    private static int _bootCpuHz;
    private static int _cursorCounter;
    private static bool _basicReady;
    private static long _executedCyclesSinceLastRead;

    public static void Initialize(
        VirtualGraphicsController vgc,
        BitmapFont font,
        KDS.e6502.Cpu cpu,
        CompositeBusDevice bus,
        int cpuHz)
    {
        _vgc = vgc;
        _font = font;
        _cpu = cpu;
        _bus = bus;
        _promptCpuHz = cpuHz;
        _programCpuHz = Math.Max(cpuHz, 4_000_000);
        _bootCpuHz = VgcConstants.DefaultCpuHz;
    }

    public static void ToggleCursor()
    {
        _cursorVisible = !_cursorVisible;
    }

    [JSExport]
    public static void ConfigureCpuHz(int promptCpuHz, int programCpuHz, int bootCpuHz)
    {
        _promptCpuHz = Math.Clamp(promptCpuHz, 100_000, VgcConstants.DefaultCpuHz);
        _programCpuHz = Math.Clamp(programCpuHz, 100_000, VgcConstants.DefaultCpuHz);
        _bootCpuHz = Math.Clamp(bootCpuHz, 100_000, VgcConstants.DefaultCpuHz);
    }

    [JSExport]
    public static byte[] Tick()
    {
        RunCpuSlice(promptMilliseconds: 2, programMilliseconds: 6, bootMilliseconds: 33);
        return Render();
    }

    [JSExport]
    public static int RunCpuSlice(int promptMilliseconds, int programMilliseconds, int bootMilliseconds)
    {
        if (_cpu != null && _bus != null)
        {
            bool promptInputMode = _basicReady && _bus.Vgc.IsCursorEnabled;
            int budgetMs = !_basicReady
                ? bootMilliseconds
                : promptInputMode ? promptMilliseconds : programMilliseconds;
            int targetHz = !_basicReady
                ? _bootCpuHz
                : promptInputMode ? _promptCpuHz : _programCpuHz;
            int targetCycles = Math.Max(1, (int)Math.Min(int.MaxValue, (long)targetHz * Math.Max(1, budgetMs) / 1000));
            int executedCycles = RunCpu(targetCycles, promptInputMode ? budgetMs : 0);

            if (_vgc != null && !_basicReady)
                _basicReady = DetectReadyPrompt(_vgc);

            return executedCycles;
        }

        return 0;
    }

    [JSExport]
    public static double DrainExecutedCycles()
    {
        long cycles = _executedCyclesSinceLastRead;
        _executedCyclesSinceLastRead = 0;
        return cycles;
    }

    [JSExport]
    public static byte[] Render()
    {
        AdvanceCursorBlink();

        if (_vgc != null && _font != null)
        {
            if (!_basicReady)
                _basicReady = DetectReadyPrompt(_vgc);
            RenderFramebuffer();
        }

        return _framebuffer;
    }

    [JSExport]
    public static byte[] RenderPacket()
    {
        AdvanceCursorBlink();

        if (_vgc == null || _font == null)
            return BuildFullPacket();

        var vgc = _vgc;
        if (!_basicReady)
            _basicReady = DetectReadyPrompt(vgc);

        ReadOnlySpan<VirtualGraphicsController.CopperEvent> copperProgram = vgc.GetCopperProgram();
        bool copperEnabled = vgc.IsCopperEnabled && !copperProgram.IsEmpty;
        var state = RenderVideoState.FromVgc(vgc);
        if (CanUseTextFastPath(vgc, state, copperEnabled))
            return RenderTextPacket(vgc, _font, state);

        RenderFramebuffer();
        _textSnapshotInitialized = false;
        return BuildFullPacket();
    }

    private static void AdvanceCursorBlink()
    {
        _cursorCounter++;
        if (_cursorCounter >= 30)
        {
            _cursorCounter = 0;
            _cursorVisible = !_cursorVisible;
        }
    }

    private static int RunCpu(int targetCycles, int maxMilliseconds)
    {
        var cpu = _cpu!;
        var bus = _bus!;
        int remaining = targetCycles;
        long sliceStarted = Stopwatch.GetTimestamp();
        bool limitByTime = maxMilliseconds > 0;
        long maxSliceTicks = limitByTime
            ? Stopwatch.Frequency * maxMilliseconds / 1000
            : long.MaxValue;
        int executedCycles = 0;
        int instructions = 0;

        while (remaining > 0)
        {
            int cycles = cpu.ClocksForNext();
            cpu.ExecuteNext();
            bus.AdvanceCycles(cycles);

            if (bus.SidPlayer.HasPendingCall)
                bus.SidPlayer.ExecutePendingCalls(cpu);
            if (bus.Timer.IrqPending ||
                bus.Nic.IrqPending ||
                bus.VgcIrqPending ||
                bus.BoardInputIrqPending)
                cpu.IrqWaiting = true;

            remaining -= cycles;
            executedCycles += cycles;
            instructions++;
            if (limitByTime &&
                (instructions & 0x1FF) == 0 &&
                Stopwatch.GetTimestamp() - sliceStarted >= maxSliceTicks)
            {
                break;
            }
        }

        _executedCyclesSinceLastRead += executedCycles;
        return executedCycles;
    }

    private static void RenderFramebuffer()
    {
        var vgc = _vgc!;
        var font = _font!;

        ReadOnlySpan<VirtualGraphicsController.CopperEvent> copperProgram = vgc.GetCopperProgram();
        bool copperEnabled = vgc.IsCopperEnabled && !copperProgram.IsEmpty;
        int copperIndex = 0;

        var state = RenderVideoState.FromVgc(vgc);
        Span<uint> framebuffer = MemoryMarshal.Cast<byte, uint>(_framebuffer.AsSpan());
        ReadOnlySpan<uint> palette = GetRgbaPalette(state.PaletteMode);
        uint borderPixel = palette[state.BorderColor & 0x0F];
        if (state.DisplayDim != 15)
            borderPixel = DimColor(borderPixel, state.DisplayDim);
        framebuffer.Fill(borderPixel);

        if (CanUseTextFastPath(vgc, state, copperEnabled))
        {
            RenderTextOnlyFramebuffer(vgc, font, state, framebuffer, palette);
            vgc.SetCollisionRegisters(0, 0);
            return;
        }

        var sprites = SpriteRenderState.FromVgc(vgc);
        if (vgc.SnapshotSpriteShapes(_shapeRamSnapshot) || !_shapeRamInitialized)
            _shapeRamInitialized = true;
        ReadOnlySpan<byte> shapeRam = _shapeRamSnapshot;
        int cursorX = vgc.GetCursorX();
        int cursorY = vgc.GetCursorY();
        bool cursorEnabled = _cursorVisible && vgc.IsCursorVisibleInCurrentMode;
        byte gfxTransparentColor = vgc.GetGfxTransparentColor();

        ushort colSS = 0, colSB = 0;

        for (int y = 0; y < VgcConstants.GfxHeight; y++)
        {
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

            int spritePlaneY = y + VgcConstants.SpriteCanvasY;
            SpriteRenderer.RasterizeScanline(spritePlaneY, sprites, shapeRam,
                _lineBehind, _lineBetween, _lineFront, _spriteMask);

            SpriteRenderer.AccumulateCollisions(
                _spriteMask.AsSpan(VgcConstants.SpriteCanvasX, VgcConstants.GfxWidth),
                vgc,
                state.GfxScrollX,
                state.GfxScrollY,
                y,
                ref colSS,
                ref colSB);

            for (int x = 0; x < VgcConstants.GfxWidth; x++)
            {
                if (copperEnabled)
                {
                    int position = y * VgcConstants.GfxWidth + x;
                    while (copperIndex < copperProgram.Length && copperProgram[copperIndex].Position == position)
                    {
                        if (copperProgram[copperIndex].RegisterIndex == VgcConstants.CopperRegIrq)
                            vgc.RaiseCopperIrq(copperProgram[copperIndex].Value);
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
                byte gfxColorIndex = vgc.GetGfxPixelColor(sampleGfxX, sampleGfxY);
                bool gfxOpaque = gfxColorIndex != gfxTransparentColor;
                uint gfxPixel = gfxOpaque ? palette[gfxColorIndex & 0x0F] : 0u;

                for (int dy = 0; dy < 2; dy++)
                {
                    int canvasPy = y * 2 + dy;
                    int py = VgcConstants.CanvasOffsetY + canvasPy;
                    int rowBase = py * NativeWidth;
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
                        {
                            textOpaque = TrySampleTextPixel(
                                canvasPx,
                                canvasPy,
                                state,
                                cursorX,
                                cursorY,
                                cursorEnabled,
                                font,
                                vgc,
                                palette,
                                out textPixel);
                        }

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

                        framebuffer[rowBase + px] = pixel;
                    }
                }
            }
        }

        vgc.SetCollisionRegisters(colSS, colSB);
    }

    private static bool CanUseTextFastPath(
        VirtualGraphicsController vgc,
        RenderVideoState state,
        bool copperEnabled)
    {
        if (state.Mode != 0 || copperEnabled || state.TextScrollX != 0 || state.TextScrollY != 0)
            return false;

        for (int i = 0; i < VgcConstants.MaxSprites; i++)
        {
            if (vgc.GetSpriteState(i).enabled)
                return false;
        }

        return true;
    }

    private static void RenderTextOnlyFramebuffer(
        VirtualGraphicsController vgc,
        BitmapFont font,
        RenderVideoState state,
        Span<uint> framebuffer,
        ReadOnlySpan<uint> palette)
    {
        for (int row = 0; row < VgcConstants.ScreenRows; row++)
            RenderTextRow(vgc, font, state, framebuffer, palette, row);
    }

    private static void RenderTextRow(
        VirtualGraphicsController vgc,
        BitmapFont font,
        RenderVideoState state,
        Span<uint> framebuffer,
        ReadOnlySpan<uint> palette,
        int row)
    {
        int cursorX = vgc.GetCursorX();
        int cursorY = vgc.GetCursorY();
        bool cursorEnabled = _cursorVisible && vgc.IsCursorVisibleInCurrentMode;

        for (int col = 0; col < VgcConstants.ScreenCols; col++)
        {
            byte ch = vgc.GetScreenChar(col, row);
            byte colorAttr = vgc.GetScreenColor(col, row);
            byte textAttr = vgc.GetScreenTextAttr(col, row);

            byte fgColor = (byte)(colorAttr & 0x0F);
            byte bgColor = (byte)((colorAttr >> 4) & 0x0F);

            if (cursorEnabled && col == cursorX && row == cursorY)
                (fgColor, bgColor) = (bgColor, fgColor);

            uint fgPixel = palette[fgColor];
            uint bgPixel = palette[bgColor];
            if (state.DisplayDim != 15)
            {
                fgPixel = DimColor(fgPixel, state.DisplayDim);
                bgPixel = DimColor(bgPixel, state.DisplayDim);
            }

            bool flashHidden = (textAttr & VgcConstants.TextAttrFlash) != 0 && !state.FlashVisible;
            int baseX = VgcConstants.CanvasOffsetX + col * BitmapFont.GlyphWidth;
            int baseY = VgcConstants.CanvasOffsetY + row * BitmapFont.GlyphHeight;

            for (int glyphY = 0; glyphY < BitmapFont.GlyphHeight; glyphY++)
            {
                byte rowBits = flashHidden ? (byte)0 : font.GetRow(state.FontIndex, ch, glyphY);
                int dest = (baseY + glyphY) * NativeWidth + baseX;
                for (int glyphX = 0; glyphX < BitmapFont.GlyphWidth; glyphX++)
                {
                    framebuffer[dest + glyphX] =
                        (rowBits & (0x80 >> glyphX)) != 0 ? fgPixel : bgPixel;
                }
            }
        }
    }

    private static byte[] RenderTextPacket(
        VirtualGraphicsController vgc,
        BitmapFont font,
        RenderVideoState state)
    {
        Span<uint> framebuffer = MemoryMarshal.Cast<byte, uint>(_framebuffer.AsSpan());
        ReadOnlySpan<uint> palette = GetRgbaPalette(state.PaletteMode);
        bool fullRedraw = !_textSnapshotInitialized
            || _lastTextFontIndex != state.FontIndex
            || _lastTextDisplayDim != state.DisplayDim
            || _lastTextPaletteMode != state.PaletteMode
            || _lastTextBorderColor != state.BorderColor;

        if (fullRedraw)
        {
            uint borderPixel = palette[state.BorderColor & 0x0F];
            if (state.DisplayDim != 15)
                borderPixel = DimColor(borderPixel, state.DisplayDim);
            framebuffer.Fill(borderPixel);
            RenderTextOnlyFramebuffer(vgc, font, state, framebuffer, palette);
            CaptureTextSnapshot(vgc);
            SaveTextState(vgc, state);
            return BuildFullPacket();
        }

        Array.Clear(_dirtyRows);
        int dirtyCount = MarkDirtyTextRows(vgc, state);
        if (dirtyCount == 0)
        {
            SaveTextState(vgc, state);
            return _emptyPacket;
        }

        if (dirtyCount > DirtyRowFullFrameThreshold)
        {
            RenderTextOnlyFramebuffer(vgc, font, state, framebuffer, palette);
            SaveTextState(vgc, state);
            return BuildActiveCanvasPacket();
        }

        _packetBuffer[0] = 1;
        _packetBuffer[1] = (byte)dirtyCount;

        int rowHeader = 2;
        int dataOffset = rowHeader + dirtyCount;
        for (int row = 0; row < VgcConstants.ScreenRows; row++)
        {
            if (!_dirtyRows[row])
                continue;

            _packetBuffer[rowHeader++] = (byte)row;
            RenderTextRow(vgc, font, state, framebuffer, palette, row);
            CopyTextRowToPacket(row, dataOffset);
            dataOffset += TextRowBytes;
        }

        SaveTextState(vgc, state);
        return _packetBuffer;
    }

    private static int MarkDirtyTextRows(VirtualGraphicsController vgc, RenderVideoState state)
    {
        int dirtyCount = 0;
        for (int row = 0; row < VgcConstants.ScreenRows; row++)
        {
            bool rowDirty = false;
            int rowOffset = row * VgcConstants.ScreenCols;
            for (int col = 0; col < VgcConstants.ScreenCols; col++)
            {
                int index = rowOffset + col;
                byte ch = vgc.GetScreenChar(col, row);
                byte color = vgc.GetScreenColor(col, row);
                byte attr = vgc.GetScreenTextAttr(col, row);
                if (_lastScreenChars[index] != ch ||
                    _lastScreenColors[index] != color ||
                    _lastScreenAttrs[index] != attr)
                {
                    _lastScreenChars[index] = ch;
                    _lastScreenColors[index] = color;
                    _lastScreenAttrs[index] = attr;
                    rowDirty = true;
                }
            }

            if (state.FlashVisible != _lastFlashVisible && RowHasFlashAttribute(row))
                rowDirty = true;

            if (rowDirty)
                dirtyCount += MarkDirtyRow(row);
        }

        int cursorX = vgc.GetCursorX();
        int cursorY = vgc.GetCursorY();
        bool cursorEnabled = _cursorVisible && vgc.IsCursorVisibleInCurrentMode;
        if (cursorEnabled != _lastCursorEnabled ||
            cursorX != _lastCursorX ||
            cursorY != _lastCursorY)
        {
            if (_lastCursorEnabled)
                dirtyCount += MarkDirtyRow(_lastCursorY);
            if (cursorEnabled)
                dirtyCount += MarkDirtyRow(cursorY);
        }

        return dirtyCount;
    }

    private static int MarkDirtyRow(int row)
    {
        if ((uint)row >= VgcConstants.ScreenRows || _dirtyRows[row])
            return 0;

        _dirtyRows[row] = true;
        return 1;
    }

    private static bool RowHasFlashAttribute(int row)
    {
        int rowOffset = row * VgcConstants.ScreenCols;
        for (int col = 0; col < VgcConstants.ScreenCols; col++)
        {
            if ((_lastScreenAttrs[rowOffset + col] & VgcConstants.TextAttrFlash) != 0)
                return true;
        }

        return false;
    }

    private static void CaptureTextSnapshot(VirtualGraphicsController vgc)
    {
        for (int row = 0; row < VgcConstants.ScreenRows; row++)
        {
            int rowOffset = row * VgcConstants.ScreenCols;
            for (int col = 0; col < VgcConstants.ScreenCols; col++)
            {
                int index = rowOffset + col;
                _lastScreenChars[index] = vgc.GetScreenChar(col, row);
                _lastScreenColors[index] = vgc.GetScreenColor(col, row);
                _lastScreenAttrs[index] = vgc.GetScreenTextAttr(col, row);
            }
        }

        _textSnapshotInitialized = true;
    }

    private static void SaveTextState(VirtualGraphicsController vgc, RenderVideoState state)
    {
        _lastTextFontIndex = (byte)state.FontIndex;
        _lastTextDisplayDim = state.DisplayDim;
        _lastTextPaletteMode = state.PaletteMode;
        _lastTextBorderColor = state.BorderColor;
        _lastFlashVisible = state.FlashVisible;
        _lastCursorX = vgc.GetCursorX();
        _lastCursorY = vgc.GetCursorY();
        _lastCursorEnabled = _cursorVisible && vgc.IsCursorVisibleInCurrentMode;
    }

    private static byte[] BuildFullPacket()
    {
        _packetBuffer[0] = 0;
        Buffer.BlockCopy(_framebuffer, 0, _packetBuffer, 1, FramebufferSize);
        return _packetBuffer;
    }

    private static byte[] BuildActiveCanvasPacket()
    {
        _packetBuffer[0] = 3;
        int dataOffset = 1;
        for (int y = 0; y < VgcConstants.CanvasHeight; y++)
        {
            int sourceOffset = ((VgcConstants.CanvasOffsetY + y) * NativeWidth + VgcConstants.CanvasOffsetX) * 4;
            Buffer.BlockCopy(_framebuffer, sourceOffset, _packetBuffer, dataOffset, VgcConstants.CanvasWidth * 4);
            dataOffset += VgcConstants.CanvasWidth * 4;
        }

        return _packetBuffer;
    }

    private static void CopyTextRowToPacket(int row, int dataOffset)
    {
        for (int glyphY = 0; glyphY < TextRowPixelHeight; glyphY++)
        {
            int sourceOffset = ((VgcConstants.CanvasOffsetY + row * TextRowPixelHeight + glyphY)
                * NativeWidth + VgcConstants.CanvasOffsetX) * 4;
            Buffer.BlockCopy(_framebuffer, sourceOffset, _packetBuffer, dataOffset, VgcConstants.CanvasWidth * 4);
            dataOffset += VgcConstants.CanvasWidth * 4;
        }
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
        ReadOnlySpan<uint> palette,
        out uint pixel)
    {
        if (!TextPixelRenderer.TrySample(
            vgc,
            font,
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

    private static bool DetectReadyPrompt(VirtualGraphicsController vgc)
    {
        const string ready = "Ready";
        for (int row = 0; row < VgcConstants.ScreenRows; row++)
        {
            for (int col = 0; col <= VgcConstants.ScreenCols - ready.Length; col++)
            {
                int i = 0;
                for (; i < ready.Length; i++)
                {
                    byte ch = vgc.GetScreenChar(col + i, row);
                    if (ch != ready[i])
                        break;
                }

                if (i == ready.Length)
                    return true;
            }
        }

        return false;
    }

    private static ReadOnlySpan<uint> GetRgbaPalette(byte paletteMode) =>
        (paletteMode & 0x01) != 0 ? _egaPalette : _c64Palette;

    private static uint DimColor(uint rgba, byte dim)
    {
        if (dim == 0)
            return 0xFF000000;

        uint r = (rgba & 0xFF) * dim >> 4;
        uint g = ((rgba >> 8) & 0xFF) * dim >> 4;
        uint b = ((rgba >> 16) & 0xFF) * dim >> 4;
        return r | (g << 8) | (b << 16) | 0xFF000000;
    }

    private static uint[] BuildRgbaPalette(byte paletteMode)
    {
        var palette = new uint[16];
        for (byte i = 0; i < 16; i++)
        {
            var c = ColorPalette.Get(i, paletteMode);
            palette[i] = (uint)c.R | ((uint)c.G << 8) | ((uint)c.B << 16) | 0xFF000000;
        }

        return palette;
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

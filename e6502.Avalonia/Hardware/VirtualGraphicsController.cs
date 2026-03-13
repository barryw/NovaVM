using e6502.Avalonia.Rendering;

namespace e6502.Avalonia.Hardware;

/// <summary>
/// Interface for screen input polling (keyboard queue from ScreenEditor).
/// </summary>
public interface IScreenInput
{
    bool HasQueuedInput { get; }
    byte DequeueInput();
}

/// <summary>
/// Virtual Graphics Controller for the Avalonia renderer.
/// Manages text display (80x25), block graphics (320x200), and multicolor sprites
/// (16 sprites, 16x16 pixels, 4-bit color per pixel, 256 shape slots).
///
/// Address ownership:
///   $A000-$A01F  VGC registers + command interface
///   $A040-$A0BF  Sprite registers (8 bytes × 16 sprites)
///   $AA00-$B1CF  Character RAM (2000 bytes)
///   $B1D0-$B99F  Color RAM (2000 bytes)
///
/// Sprite shape data stored in 256 × 128-byte slots (32KB), accessible via
/// memory space I/O and DMA. Each sprite has a shape index register pointing
/// to its active shape slot. The copper can write sprite registers at scanline
/// granularity for vertical sprite multiplexing.
/// </summary>
public class VirtualGraphicsController
{
    public readonly struct CopperEvent
    {
        public CopperEvent(ushort position, byte registerIndex, byte value)
        {
            Position = position;
            RegisterIndex = registerIndex;
            Value = value;
        }

        public ushort Position { get; }
        public byte RegisterIndex { get; }
        public byte Value { get; }
    }

    // Core registers $A000-$A00F (16 bytes)
    private readonly byte[] _regs = new byte[16];

    // Command registers $A010-$A01E (15 bytes: [0]=cmd, [1..14]=P0..P13)
    private readonly byte[] _cmdRegs = new byte[15];

    // Sprite shape data: 16 sprites x 128 bytes each (4-bit color, 16x16 pixels)
    // Accessed from CPU thread (bus writes/commands) and UI thread (renderer, sprite editor).
    // All mutations must hold _spriteShapeLock.
    private readonly byte[] _spriteShapes = new byte[VgcConstants.ShapeRamSize];
    private readonly object _spriteShapeLock = new();
    private volatile bool _spriteShapesDirty;

    // Sprite state arrays (host-side, not bus-mapped)
    private readonly bool[] _spriteEnabled = new bool[VgcConstants.MaxSprites];
    private readonly int[] _spriteX = new int[VgcConstants.MaxSprites];
    private readonly int[] _spriteY = new int[VgcConstants.MaxSprites];
    private readonly byte[] _spriteFlags = new byte[VgcConstants.MaxSprites];
    private readonly byte[] _spritePriority = new byte[VgcConstants.MaxSprites];
    private readonly byte[] _spriteShapeIndex = new byte[VgcConstants.MaxSprites];
    private readonly byte[] _spriteTransColor = new byte[VgcConstants.MaxSprites];

    // Screen RAM (2000 bytes, 80x25)
    private readonly byte[] _screenRam = new byte[VgcConstants.ScreenSize];

    // Color RAM (2000 bytes, 80x25)
    private readonly byte[] _colorRam = new byte[VgcConstants.ScreenSize];

    // Block graphics bitmap (320x200, NOT 6502-addressable)
    private readonly byte[] _gfxBitmap = new byte[VgcConstants.GfxWidth * VgcConstants.GfxHeight];

    // Current graphics draw color (0 = use text foreground color)
    private byte _gfxDrawColor;

    // -------------------------------------------------------------------------
    // Tile engine state (host-side, not in 6502 address space)
    // -------------------------------------------------------------------------

    // Tile definitions: 256 tiles, max 128 bytes each (16x16 mode)
    private readonly byte[] _tileData = new byte[VgcConstants.TileRamSize16];
    private readonly object _tileLock = new();
    private volatile bool _tileDirty;
    private readonly byte[] _columnBuffer = new byte[VgcConstants.NametableRows]; // 25-entry column buffer

    // 4 nametables, each 1000 bytes (40x25 tile indices)
    private readonly byte[][] _nametables = new byte[VgcConstants.NametableCount][];

    // 4 attribute tables, each 1000 bytes (per-tile attribute)
    private readonly byte[][] _attrTables = new byte[VgcConstants.NametableCount][];

    // Palette RAM: 16 sub-palettes × 16 colors × 3 bytes (RGB)
    private readonly byte[] _tilePaletteRam = new byte[VgcConstants.TilePaletteRamSize];

    // Tile config state
    private byte _tileConfig;       // bit0=size, bits2-1=mirror
    private byte _tileTransColor;   // global transparent color index
    private int _tileScrollX;       // 16-bit pixel scroll X
    private int _tileScrollY;       // 16-bit pixel scroll Y
    private byte _tileStatus;       // last command status
    private byte _tilePeekVal;      // last peeked tile index
    private byte _tilePeekAttr;     // last peeked attribute
    private ushort _tileCollision;  // sprite-tile collision bitmask

    // Tile command parameter registers (local copy)
    private readonly byte[] _tileRegs = new byte[VgcConstants.TileRegEnd - VgcConstants.TileRegBase + 1];

    // IRQ control register ($A01F)
    private byte _irqCtrl;

    // Copper program (host-side, command driven) — 128 lists, vblank-synchronized
    private readonly List<CopperEvent>[] _copperEvents = new List<CopperEvent>[VgcConstants.CopperListCount];
    private readonly CopperEvent[][] _copperPrograms = new CopperEvent[VgcConstants.CopperListCount][];
    private readonly bool[] _copperListDirty = new bool[VgcConstants.CopperListCount];
    private int _copperTargetList;      // which list ADD/CLEAR edit (default 0)
    private int _copperActiveList;      // which compiled program the renderer reads
    private int _copperPendingList;     // becomes active at next vblank
    private bool _copperEnabled;


    // Font for GTEXT command
    private BitmapFont? _bitmapFont;

    // Bus memory reference for reading FIO_NAME buffer
    private byte[]? _busMemory;

    // Screen input source
    private IScreenInput? _screenEditor;

    public VirtualGraphicsController()
    {
        for (int i = 0; i < VgcConstants.NametableCount; i++)
        {
            _nametables[i] = new byte[VgcConstants.NametableSize];
            _attrTables[i] = new byte[VgcConstants.NametableSize];
        }
        Reset();
    }

    public void SetFont(BitmapFont font) => _bitmapFont = font;
    public void SetBusMemory(byte[] mem) => _busMemory = mem;

    public void Reset()
    {
        Array.Clear(_regs);
        Array.Clear(_cmdRegs);
        Array.Clear(_spriteShapes);
        _spriteShapesDirty = true;
        Array.Clear(_spriteEnabled);
        Array.Clear(_spriteX);
        Array.Clear(_spriteY);
        Array.Clear(_spriteFlags);
        Array.Clear(_spriteTransColor);
        Array.Clear(_gfxBitmap);
        _gfxDrawColor = 1;
        _irqCtrl = 0;
        for (int i = 0; i < VgcConstants.CopperListCount; i++)
        {
            if (_copperEvents[i] != null)
                _copperEvents[i].Clear();
            else
                _copperEvents[i] = [];
            _copperPrograms[i] = [];
            _copperListDirty[i] = false;
        }
        _copperTargetList = 0;
        _copperActiveList = 0;
        _copperPendingList = 0;
        _copperEnabled = false;

        // Default sprite priority: in front of all
        Array.Fill(_spritePriority, (byte)VgcConstants.SpritePriInFront);

        // Default shape index: sprite N uses slot N
        for (int i = 0; i < VgcConstants.MaxSprites; i++)
            _spriteShapeIndex[i] = (byte)i;

        // Screen RAM initialized to spaces
        Array.Fill(_screenRam, (byte)0x20);

        // Color RAM initialized to 1 (white)
        Array.Fill(_colorRam, (byte)1);

        // Default foreground color = 1
        _regs[VgcConstants.RegFgCol - VgcConstants.VgcBase] = 1;

        // Tile engine reset
        ResetTileEngine();
    }

    private void ResetTileEngine()
    {
        lock (_tileLock)
        {
            Array.Clear(_tileData);
            _tileDirty = true;
        }
        for (int i = 0; i < VgcConstants.NametableCount; i++)
        {
            Array.Clear(_nametables[i]);
            Array.Clear(_attrTables[i]);
        }
        Array.Clear(_tilePaletteRam);
        Array.Clear(_tileRegs);
        _tileConfig = 0;
        _tileTransColor = 0;
        _tileScrollX = 0;
        _tileScrollY = 0;
        _tileStatus = 0;
        _tilePeekVal = 0;
        _tilePeekAttr = 0;
        _tileCollision = 0;

        // Initialize sub-palette 0 to the standard VGC palette
        InitDefaultTilePalette();
    }

    private void InitDefaultTilePalette()
    {
        for (int i = 0; i < VgcConstants.TilePaletteColors; i++)
        {
            var c = Rendering.ColorPalette.Get(i);
            int offset = i * 3;
            _tilePaletteRam[offset] = c.R;
            _tilePaletteRam[offset + 1] = c.G;
            _tilePaletteRam[offset + 2] = c.B;
        }
    }

    // -------------------------------------------------------------------------
    // Address ownership
    // -------------------------------------------------------------------------

    public bool OwnsAddress(ushort address)
    {
        // VGC registers + command: $A000-$A01E
        if (address >= VgcConstants.VgcBase && address <= VgcConstants.VgcRegsEnd)
            return true;

        // Sprite registers: $A040-$A0BF
        if (address >= VgcConstants.SpriteRegBase && address <= VgcConstants.SpriteRegEnd)
            return true;

        // Tile registers: $A0C0-$A0DF
        if (address >= VgcConstants.TileRegBase && address <= VgcConstants.TileRegEnd)
            return true;

        // Character RAM: $AA00-$B1CF
        if (address >= VgcConstants.CharRamBase && address <= VgcConstants.CharRamEnd)
            return true;

        // Color RAM: $B1D0-$B99F
        if (address >= VgcConstants.ColorRamBase && address <= VgcConstants.ColorRamEnd)
            return true;

        return false;
    }

    // -------------------------------------------------------------------------
    // Bus read
    // -------------------------------------------------------------------------

    public byte Read(ushort address)
    {
        // Color RAM
        if (address >= VgcConstants.ColorRamBase && address <= VgcConstants.ColorRamEnd)
            return _colorRam[address - VgcConstants.ColorRamBase];

        // Character RAM
        if (address >= VgcConstants.CharRamBase && address <= VgcConstants.CharRamEnd)
            return _screenRam[address - VgcConstants.CharRamBase];

        // Sprite registers $A040-$A0BF
        if (address >= VgcConstants.SpriteRegBase && address <= VgcConstants.SpriteRegEnd)
            return ReadSpriteRegister(address);

        // Tile registers $A0C0-$A0DF
        if (address >= VgcConstants.TileRegBase && address <= VgcConstants.TileRegEnd)
            return ReadTileRegister(address);

        // Command parameter registers $A011-$A01E
        if (address >= VgcConstants.RegP0 && address <= VgcConstants.RegP13)
            return _cmdRegs[address - VgcConstants.RegCmd];

        // Command register $A010
        if (address == VgcConstants.RegCmd)
            return _cmdRegs[0];

        // IRQ control register $A01F
        if (address == VgcConstants.RegIrqCtrl)
            return _irqCtrl;

        // Core registers $A000-$A00F
        if (address >= VgcConstants.VgcBase && address < VgcConstants.VgcBase + 16)
        {
            int idx = address - VgcConstants.VgcBase;
            switch (address)
            {
                case VgcConstants.RegColSt:
                {
                    byte val = _regs[idx];
                    _regs[idx] = 0;
                    return val;
                }
                case VgcConstants.RegColBg:
                {
                    byte val = _regs[idx];
                    _regs[idx] = 0;
                    return val;
                }
                case VgcConstants.RegCharIn:
                {
                    if (_screenEditor is not null && _screenEditor.HasQueuedInput)
                        return _screenEditor.DequeueInput();
                    byte val = _regs[idx];
                    _regs[idx] = 0;
                    return val;
                }
                default:
                    return _regs[idx];
            }
        }

        return 0;
    }

    // -------------------------------------------------------------------------
    // Bus write
    // -------------------------------------------------------------------------

    public void Write(ushort address, byte data)
    {
        // Color RAM
        if (address >= VgcConstants.ColorRamBase && address <= VgcConstants.ColorRamEnd)
        {
            _colorRam[address - VgcConstants.ColorRamBase] = data;
            return;
        }

        // Character RAM
        if (address >= VgcConstants.CharRamBase && address <= VgcConstants.CharRamEnd)
        {
            _screenRam[address - VgcConstants.CharRamBase] = data;
            return;
        }

        // Sprite registers $A040-$A0BF
        if (address >= VgcConstants.SpriteRegBase && address <= VgcConstants.SpriteRegEnd)
        {
            WriteSpriteRegister(address, data);
            return;
        }

        // Tile registers $A0C0-$A0DF
        if (address >= VgcConstants.TileRegBase && address <= VgcConstants.TileRegEnd)
        {
            WriteTileRegister(address, data);
            return;
        }

        // IRQ control register $A01F
        if (address == VgcConstants.RegIrqCtrl)
        {
            _irqCtrl = data;
            return;
        }

        // Command register $A010 — store AND execute
        if (address == VgcConstants.RegCmd)
        {
            _cmdRegs[0] = data;
            ExecuteCommand(data);
            return;
        }

        // Command parameter registers $A011-$A01E
        if (address >= VgcConstants.RegP0 && address <= VgcConstants.RegP13)
        {
            _cmdRegs[address - VgcConstants.RegCmd] = data;
            return;
        }

        // Core registers $A000-$A00F
        if (address >= VgcConstants.VgcBase && address < VgcConstants.VgcBase + 16)
        {
            switch (address)
            {
                case VgcConstants.RegStatus:
                case VgcConstants.RegSpriteCount:
                    // read-only, ignore writes
                    return;
                case VgcConstants.RegCharOut:
                    _regs[address - VgcConstants.VgcBase] = data;
                    HandleCharOut(data);
                    return;
                default:
                    _regs[address - VgcConstants.VgcBase] = data;
                    return;
            }
        }
    }

    // -------------------------------------------------------------------------
    // Overloads for int address (convenience for tests using VgcConstants)
    // -------------------------------------------------------------------------

    public byte Read(int address) => Read((ushort)address);
    public void Write(int address, byte data) => Write((ushort)address, data);

    // -------------------------------------------------------------------------
    // Screen editor integration
    // -------------------------------------------------------------------------

    public void SetScreenEditor(IScreenInput editor) =>
        _screenEditor = editor;

    // -------------------------------------------------------------------------
    // Input
    // -------------------------------------------------------------------------

    public void FeedInput(byte key) =>
        _regs[VgcConstants.RegCharIn - VgcConstants.VgcBase] = key;

    // -------------------------------------------------------------------------
    // Renderer accessors
    // -------------------------------------------------------------------------

    public byte GetScreenChar(int col, int row) =>
        _screenRam[row * VgcConstants.ScreenCols + col];

    public byte GetScreenColor(int col, int row) =>
        _colorRam[row * VgcConstants.ScreenCols + col];

    public byte GetMode() =>
        _regs[VgcConstants.RegMode - VgcConstants.VgcBase];

    public byte GetBgColor() =>
        _regs[VgcConstants.RegBgCol - VgcConstants.VgcBase];

    public byte GetScrollX() =>
        _regs[VgcConstants.RegScrollX - VgcConstants.VgcBase];

    public byte GetScrollY() =>
        _regs[VgcConstants.RegScrollY - VgcConstants.VgcBase];

    public byte GetFontIndex() =>
        (byte)(_regs[VgcConstants.RegFont - VgcConstants.VgcBase] & 0x07);

    public byte GetBorderColor() =>
        _regs[VgcConstants.RegBorder - VgcConstants.VgcBase];

    public byte GetCursorX() =>
        _regs[VgcConstants.RegCursorX - VgcConstants.VgcBase];

    public byte GetCursorY() =>
        _regs[VgcConstants.RegCursorY - VgcConstants.VgcBase];

    public bool IsCursorEnabled =>
        _regs[VgcConstants.RegCursorEnable - VgcConstants.VgcBase] != 0;

    public bool IsRasterIrqEnabled => (_irqCtrl & 0x01) != 0;
    public bool IsCopperEnabled => _copperEnabled;

    public ReadOnlySpan<CopperEvent> GetCopperProgram() => _copperPrograms[_copperActiveList];

    public bool GetGfxPixel(int x, int y) =>
        _gfxBitmap[y * VgcConstants.GfxWidth + x] != 0;

    public byte GetGfxPixelColor(int x, int y) =>
        _gfxBitmap[y * VgcConstants.GfxWidth + x];

    public ReadOnlySpan<byte> GetSpriteShape(int index) =>
        _spriteShapes.AsSpan(_spriteShapeIndex[index] * VgcConstants.SpriteShapeSize, VgcConstants.SpriteShapeSize);

    /// <summary>
    /// Copies sprite shape data under lock. Safe for cross-thread reads.
    /// </summary>
    public void CopySpriteShape(int index, Span<byte> destination)
    {
        lock (_spriteShapeLock)
            _spriteShapes.AsSpan(_spriteShapeIndex[index] * VgcConstants.SpriteShapeSize, VgcConstants.SpriteShapeSize)
                .CopyTo(destination);
    }

    /// <summary>
    /// Atomically sets a single sprite pixel (nibble read-modify-write under lock).
    /// Safe to call from any thread.
    /// </summary>
    public void SetSpritePixel(int spriteIndex, int x, int y, byte color)
    {
        if ((uint)spriteIndex >= VgcConstants.MaxSprites || (uint)x >= 16 || (uint)y >= 16) return;
        int byteIdx = _spriteShapeIndex[spriteIndex] * VgcConstants.SpriteShapeSize + y * VgcConstants.SpriteBytesPerRow + x / 2;
        lock (_spriteShapeLock)
        {
            if (x % 2 == 0)
                _spriteShapes[byteIdx] = (byte)((_spriteShapes[byteIdx] & 0x0F) | ((color & 0x0F) << 4));
            else
                _spriteShapes[byteIdx] = (byte)((_spriteShapes[byteIdx] & 0xF0) | (color & 0x0F));
            _spriteShapesDirty = true;
        }
    }

    public (int x, int y, bool enabled, byte flags, byte priority) GetSpriteState(int index) =>
        (_spriteX[index], _spriteY[index], _spriteEnabled[index],
         _spriteFlags[index], _spritePriority[index]);

    public void SetSpritePosition(int index, int x, int y)
    {
        if ((uint)index >= VgcConstants.MaxSprites) return;
        _spriteX[index] = x;
        _spriteY[index] = y;
    }

    public void SetSpriteEnabled(int index, bool enabled)
    {
        if ((uint)index >= VgcConstants.MaxSprites) return;
        _spriteEnabled[index] = enabled;
        UpdateSpriteCount();
    }

    public void SetSpriteFlags(int index, byte flags)
    {
        if ((uint)index >= VgcConstants.MaxSprites) return;
        _spriteFlags[index] = (byte)(flags & 0x03);
    }

    public void SetSpritePriority(int index, byte priority)
    {
        if ((uint)index >= VgcConstants.MaxSprites) return;
        _spritePriority[index] = (byte)Math.Min((int)priority, 2);
    }

    public byte GetSpriteShapeIndex(int index) => _spriteShapeIndex[index];
    public byte GetSpriteTransColor(int index) => _spriteTransColor[index];

    public void SetSpriteTransColor(int index, byte color)
    {
        if ((uint)index >= VgcConstants.MaxSprites) return;
        _spriteTransColor[index] = color;
    }

    public void SetSpriteShapeIndex(int index, byte slotIndex)
    {
        if ((uint)index >= VgcConstants.MaxSprites) return;
        _spriteShapeIndex[index] = slotIndex;
    }

    public ReadOnlySpan<byte> GetSpriteShapeBySlot(int slotIndex) =>
        _spriteShapes.AsSpan(slotIndex * VgcConstants.SpriteShapeSize, VgcConstants.SpriteShapeSize);

    public ReadOnlySpan<byte> GetSpriteShapeRam() => _spriteShapes;

    /// <summary>
    /// Copies sprite shape RAM into the caller's buffer under lock if shapes have changed.
    /// Returns true when a copy was performed; false means the buffer is still current.
    /// </summary>
    public bool SnapshotSpriteShapes(byte[] buffer)
    {
        if (!_spriteShapesDirty) return false;
        lock (_spriteShapeLock)
        {
            Array.Copy(_spriteShapes, buffer, _spriteShapes.Length);
            _spriteShapesDirty = false;
        }
        return true;
    }

    public void SetSpritePixelInSlot(int slotIndex, int x, int y, byte color)
    {
        if ((uint)slotIndex >= VgcConstants.ShapeSlotCount || (uint)x >= 16 || (uint)y >= 16) return;
        int byteIdx = slotIndex * VgcConstants.SpriteShapeSize + y * VgcConstants.SpriteBytesPerRow + x / 2;
        lock (_spriteShapeLock)
        {
            if (x % 2 == 0)
                _spriteShapes[byteIdx] = (byte)((_spriteShapes[byteIdx] & 0x0F) | ((color & 0x0F) << 4));
            else
                _spriteShapes[byteIdx] = (byte)((_spriteShapes[byteIdx] & 0xF0) | (color & 0x0F));
            _spriteShapesDirty = true;
        }
    }

    private void WriteSpriteRegister(int address, byte data)
    {
        int offset = address - VgcConstants.SpriteRegBase;
        int sprite = offset / VgcConstants.SpriteRegStride;
        int field = offset % VgcConstants.SpriteRegStride;

        if ((uint)sprite >= VgcConstants.MaxSprites) return;

        switch (field)
        {
            case VgcConstants.SprRegXLo:
                _spriteX[sprite] = (short)((_spriteX[sprite] & ~0xFF) | data);
                break;
            case VgcConstants.SprRegXHi:
                _spriteX[sprite] = (short)((data << 8) | (_spriteX[sprite] & 0xFF));
                break;
            case VgcConstants.SprRegYLo:
                _spriteY[sprite] = (short)((_spriteY[sprite] & ~0xFF) | data);
                break;
            case VgcConstants.SprRegYHi:
                _spriteY[sprite] = (short)((data << 8) | (_spriteY[sprite] & 0xFF));
                break;
            case VgcConstants.SprRegShape:
                _spriteShapeIndex[sprite] = data;
                break;
            case VgcConstants.SprRegFlags:
                _spriteEnabled[sprite] = (data & VgcConstants.SprFlagEnable) != 0;
                _spriteFlags[sprite] = (byte)(data & VgcConstants.SprFlagFlipMask);
                UpdateSpriteCount();
                break;
            case VgcConstants.SprRegPriority:
                _spritePriority[sprite] = (byte)Math.Min((int)data, 2);
                break;
            case VgcConstants.SprRegTransColor:
                _spriteTransColor[sprite] = data;
                break;
        }
    }

    private byte ReadSpriteRegister(int address)
    {
        int offset = address - VgcConstants.SpriteRegBase;
        int sprite = offset / VgcConstants.SpriteRegStride;
        int field = offset % VgcConstants.SpriteRegStride;

        if ((uint)sprite >= VgcConstants.MaxSprites) return 0;

        return field switch
        {
            VgcConstants.SprRegXLo => (byte)(_spriteX[sprite] & 0xFF),
            VgcConstants.SprRegXHi => (byte)((_spriteX[sprite] >> 8) & 0xFF),
            VgcConstants.SprRegYLo => (byte)(_spriteY[sprite] & 0xFF),
            VgcConstants.SprRegYHi => (byte)((_spriteY[sprite] >> 8) & 0xFF),
            VgcConstants.SprRegShape => _spriteShapeIndex[sprite],
            VgcConstants.SprRegFlags => (byte)((_spriteEnabled[sprite] ? VgcConstants.SprFlagEnable : 0)
                                             | (_spriteFlags[sprite] & VgcConstants.SprFlagFlipMask)),
            VgcConstants.SprRegPriority => _spritePriority[sprite],
            VgcConstants.SprRegTransColor => _spriteTransColor[sprite],
            _ => 0
        };
    }

    public void CopySpriteSlot(int src, int dest)
    {
        if ((uint)src >= VgcConstants.MaxSprites || (uint)dest >= VgcConstants.MaxSprites) return;
        lock (_spriteShapeLock)
        {
            Array.Copy(_spriteShapes, _spriteShapeIndex[src] * VgcConstants.SpriteShapeSize,
                       _spriteShapes, _spriteShapeIndex[dest] * VgcConstants.SpriteShapeSize,
                       VgcConstants.SpriteShapeSize);
            _spriteShapesDirty = true;
        }
    }

    public void ClearSpriteShape(int index)
    {
        if ((uint)index >= VgcConstants.MaxSprites) return;
        lock (_spriteShapeLock)
        {
            Array.Clear(_spriteShapes, _spriteShapeIndex[index] * VgcConstants.SpriteShapeSize, VgcConstants.SpriteShapeSize);
            _spriteShapesDirty = true;
        }
    }

    // -------------------------------------------------------------------------
    // Collision registers (written by renderer after collision detection)
    // -------------------------------------------------------------------------

    public void SetCollisionRegisters(ushort spriteToSprite, ushort spriteToBg)
    {
        _regs[VgcConstants.RegColSt - VgcConstants.VgcBase] = (byte)(spriteToSprite & 0xFF);
        _regs[VgcConstants.RegColBg - VgcConstants.VgcBase] = (byte)(spriteToBg & 0xFF);
    }

    // -------------------------------------------------------------------------
    // Frame counter
    // -------------------------------------------------------------------------

    public void SetStatus(byte value) =>
        _regs[VgcConstants.RegStatus - VgcConstants.VgcBase] = value;

    public void IncrementFrameCounter()
    {
        _regs[VgcConstants.RegStatus - VgcConstants.VgcBase]++;

        // Vblank: swap active list and rebuild any dirty copper programs
        _copperActiveList = _copperPendingList;
        for (int i = 0; i < VgcConstants.CopperListCount; i++)
        {
            if (!_copperListDirty[i]) continue;
            _copperPrograms[i] = _copperEvents[i].ToArray();
            Array.Sort(_copperPrograms[i], static (a, b) =>
            {
                int byPos = a.Position.CompareTo(b.Position);
                return byPos != 0 ? byPos : a.RegisterIndex.CompareTo(b.RegisterIndex);
            });
            _copperListDirty[i] = false;
        }
    }

    // -------------------------------------------------------------------------
    // Command dispatch
    // -------------------------------------------------------------------------

    /// <summary>
    /// Set by CmdSysReset; polled and cleared by CompositeBusDevice after Write.
    /// </summary>
    public bool SysResetRequested { get; set; }

    private void ExecuteCommand(byte cmd)
    {
        if (cmd >= VgcConstants.CmdSprDef && cmd <= VgcConstants.CmdSprPri)
            ExecuteSpriteCommand(cmd);
        else if (cmd == VgcConstants.CmdMemRead || cmd == VgcConstants.CmdMemWrite)
            ExecuteMemoryCommand(cmd);
        else if (cmd >= VgcConstants.CmdCopperAdd && cmd <= VgcConstants.CmdCopperDisable)
            ExecuteCopperCommand(cmd);
        else if (cmd >= VgcConstants.CmdCopperList && cmd <= VgcConstants.CmdCopperListEnd)
            ExecuteCopperCommand(cmd);
        else if (cmd >= VgcConstants.CmdPlot && cmd <= VgcConstants.CmdGtext)
            ExecuteGfxCommand(cmd);
        else if (cmd == VgcConstants.CmdSysReset)
        {
            Reset();
            SysResetRequested = true;
        }
    }

    // -------------------------------------------------------------------------
    // Graphics commands
    // -------------------------------------------------------------------------

    private byte EffectiveDrawColor => _gfxDrawColor;

    private void ExecuteGfxCommand(byte cmd)
    {
        // 16-bit parameters from command registers
        int p0 = _cmdRegs[1] | (_cmdRegs[2] << 8);   // x or x0
        int p1 = _cmdRegs[3] | (_cmdRegs[4] << 8);   // y or y0
        int p2 = _cmdRegs[5] | (_cmdRegs[6] << 8);   // x1 (LINE/RECT/FILL)
        int p3 = _cmdRegs[7] | (_cmdRegs[8] << 8);   // y1 (LINE/RECT/FILL)

        byte color = EffectiveDrawColor;

        switch (cmd)
        {
            case VgcConstants.CmdPlot:
                BlockGraphics.Plot(_gfxBitmap, p0, p1, color);
                break;
            case VgcConstants.CmdUnplot:
                BlockGraphics.Plot(_gfxBitmap, p0, p1, 0);
                break;
            case VgcConstants.CmdLine:
                BlockGraphics.Line(_gfxBitmap, p0, p1, p2, p3, color);
                break;
            case VgcConstants.CmdCircle:
                int ry = _cmdRegs[7] | (_cmdRegs[8] << 8);
                BlockGraphics.Ellipse(_gfxBitmap, p0, p1, p2, ry == 0 ? p2 : ry, color);
                break;
            case VgcConstants.CmdRect:
                BlockGraphics.Rect(_gfxBitmap, p0, p1, p2, p3, color);
                break;
            case VgcConstants.CmdFill:
                BlockGraphics.Fill(_gfxBitmap, p0, p1, p2, p3, color);
                break;
            case VgcConstants.CmdGcls:
                BlockGraphics.Clear(_gfxBitmap);
                break;
            case VgcConstants.CmdGcolor:
                _gfxDrawColor = (byte)(_cmdRegs[1] & 0x0F);
                break;
            case VgcConstants.CmdPaint:
                BlockGraphics.FloodFill(_gfxBitmap, p0, p1, color);
                break;
            case VgcConstants.CmdGtext:
                if (_bitmapFont != null && _busMemory != null)
                {
                    int fontSlot = _cmdRegs[5] & 0x07;
                    int scale = _cmdRegs[6];
                    if (scale < 1) scale = 1;
                    int nameLen = _busMemory[VgcConstants.FioNameLen];
                    var chars = new byte[nameLen];
                    Array.Copy(_busMemory, VgcConstants.FioName, chars, 0, nameLen);
                    BlockGraphics.Text(_gfxBitmap, p0, p1, scale, _bitmapFont, fontSlot, chars, nameLen, color);
                }
                break;
        }
    }

    // -------------------------------------------------------------------------
    // Sprite commands
    // -------------------------------------------------------------------------

    private void ExecuteSpriteCommand(byte cmd)
    {
        byte p0 = _cmdRegs[1]; // sprite index for most commands

        switch (cmd)
        {
            case VgcConstants.CmdSprDef:
            {
                int sprite = p0;
                int x = _cmdRegs[2];
                int y = _cmdRegs[3];
                byte color = _cmdRegs[4];
                if (sprite >= VgcConstants.MaxSprites || x >= 16 || y >= 16) return;
                int byteIdx = _spriteShapeIndex[sprite] * VgcConstants.SpriteShapeSize + y * VgcConstants.SpriteBytesPerRow + x / 2;
                lock (_spriteShapeLock)
                {
                    if (x % 2 == 0)
                        _spriteShapes[byteIdx] = (byte)((_spriteShapes[byteIdx] & 0x0F) | ((color & 0x0F) << 4));
                    else
                        _spriteShapes[byteIdx] = (byte)((_spriteShapes[byteIdx] & 0xF0) | (color & 0x0F));
                    _spriteShapesDirty = true;
                }
                break;
            }
            case VgcConstants.CmdSprRow:
            {
                int sprite = p0;
                int row = _cmdRegs[2];
                if (sprite >= VgcConstants.MaxSprites || row >= 16) return;
                int baseIdx = _spriteShapeIndex[sprite] * VgcConstants.SpriteShapeSize + row * VgcConstants.SpriteBytesPerRow;
                lock (_spriteShapeLock)
                {
                    for (int i = 0; i < VgcConstants.SpriteBytesPerRow; i++)
                        _spriteShapes[baseIdx + i] = _cmdRegs[3 + i];
                    _spriteShapesDirty = true;
                }
                break;
            }
            case VgcConstants.CmdSprClr:
            {
                if (p0 >= VgcConstants.MaxSprites) return;
                lock (_spriteShapeLock)
                {
                    Array.Clear(_spriteShapes, _spriteShapeIndex[p0] * VgcConstants.SpriteShapeSize, VgcConstants.SpriteShapeSize);
                    _spriteShapesDirty = true;
                }
                break;
            }
            case VgcConstants.CmdSprCopy:
            {
                byte dest = _cmdRegs[2];
                if (p0 >= VgcConstants.MaxSprites || dest >= VgcConstants.MaxSprites) return;
                lock (_spriteShapeLock)
                {
                    Array.Copy(_spriteShapes, _spriteShapeIndex[p0] * VgcConstants.SpriteShapeSize,
                               _spriteShapes, _spriteShapeIndex[dest] * VgcConstants.SpriteShapeSize,
                               VgcConstants.SpriteShapeSize);
                    _spriteShapesDirty = true;
                }
                break;
            }
            case VgcConstants.CmdSprPos:
            {
                if (p0 >= VgcConstants.MaxSprites) return;
                _spriteX[p0] = (short)(_cmdRegs[2] | (_cmdRegs[3] << 8));
                _spriteY[p0] = (short)(_cmdRegs[4] | (_cmdRegs[5] << 8));
                break;
            }
            case VgcConstants.CmdSprEna:
            {
                if (p0 >= VgcConstants.MaxSprites) return;
                _spriteEnabled[p0] = true;
                UpdateSpriteCount();
                break;
            }
            case VgcConstants.CmdSprDis:
            {
                if (p0 >= VgcConstants.MaxSprites) return;
                _spriteEnabled[p0] = false;
                UpdateSpriteCount();
                break;
            }
            case VgcConstants.CmdSprFlip:
            {
                if (p0 >= VgcConstants.MaxSprites) return;
                _spriteFlags[p0] = (byte)(_cmdRegs[2] & 0x03);
                break;
            }
            case VgcConstants.CmdSprPri:
            {
                if (p0 >= VgcConstants.MaxSprites) return;
                _spritePriority[p0] = (byte)Math.Min((int)_cmdRegs[2], 2);
                break;
            }
        }
    }

    private void UpdateSpriteCount()
    {
        int count = 0;
        for (int i = 0; i < VgcConstants.MaxSprites; i++)
            if (_spriteEnabled[i]) count++;
        _regs[VgcConstants.RegSpriteCount - VgcConstants.VgcBase] = (byte)count;
    }

    // -------------------------------------------------------------------------
    // Memory I/O commands
    // -------------------------------------------------------------------------

    private void ExecuteMemoryCommand(byte cmd)
    {
        byte space = _cmdRegs[1]; // P0
        int addr = _cmdRegs[2] | (_cmdRegs[3] << 8); // P1/P2
        bool autoInc = (_cmdRegs[5] & 0x01) != 0; // P4 bit0

        if (cmd == VgcConstants.CmdMemRead)
        {
            _cmdRegs[4] = TryReadMemorySpace(space, addr, out byte value) ? value : (byte)0; // P3
        }
        else
        {
            TryWriteMemorySpace(space, addr, _cmdRegs[4]); // P3
        }

        if (!autoInc) return;

        addr = (addr + 1) & 0xFFFF;
        _cmdRegs[2] = (byte)(addr & 0xFF);
        _cmdRegs[3] = (byte)((addr >> 8) & 0xFF);
    }

    public int GetMemorySpaceLength(byte space) =>
        space switch
        {
            VgcConstants.MemSpaceScreen => _screenRam.Length,
            VgcConstants.MemSpaceColor => _colorRam.Length,
            VgcConstants.MemSpaceGfx => _gfxBitmap.Length,
            VgcConstants.MemSpaceSprite => _spriteShapes.Length,
            VgcConstants.MemSpaceTile => _tileData.Length,
            _ => 0
        };

    public bool TryReadMemorySpace(byte space, int addr, out byte value)
    {
        value = 0;
        switch (space)
        {
            case VgcConstants.MemSpaceScreen when (uint)addr < _screenRam.Length:
                value = _screenRam[addr];
                return true;
            case VgcConstants.MemSpaceColor when (uint)addr < _colorRam.Length:
                value = _colorRam[addr];
                return true;
            case VgcConstants.MemSpaceGfx when (uint)addr < _gfxBitmap.Length:
                value = _gfxBitmap[addr];
                return true;
            case VgcConstants.MemSpaceSprite when (uint)addr < _spriteShapes.Length:
                lock (_spriteShapeLock) value = _spriteShapes[addr];
                return true;
            case VgcConstants.MemSpaceTile when (uint)addr < _tileData.Length:
                lock (_tileLock) value = _tileData[addr];
                return true;
            default:
                return false;
        }
    }

    public bool TryWriteMemorySpace(byte space, int addr, byte value)
    {
        switch (space)
        {
            case VgcConstants.MemSpaceScreen when (uint)addr < _screenRam.Length:
                _screenRam[addr] = value;
                return true;
            case VgcConstants.MemSpaceColor when (uint)addr < _colorRam.Length:
                _colorRam[addr] = value;
                return true;
            case VgcConstants.MemSpaceGfx when (uint)addr < _gfxBitmap.Length:
                _gfxBitmap[addr] = value;
                return true;
            case VgcConstants.MemSpaceSprite when (uint)addr < _spriteShapes.Length:
                lock (_spriteShapeLock) { _spriteShapes[addr] = value; _spriteShapesDirty = true; }
                return true;
            case VgcConstants.MemSpaceTile when (uint)addr < _tileData.Length:
                lock (_tileLock) { _tileData[addr] = value; _tileDirty = true; }
                return true;
            default:
                return false;
        }
    }

    // -------------------------------------------------------------------------
    // Copper (raster) commands
    // -------------------------------------------------------------------------

    private void ExecuteCopperCommand(byte cmd)
    {
        switch (cmd)
        {
            case VgcConstants.CmdCopperAdd:
                AddCopperEventFromRegisters();
                break;
            case VgcConstants.CmdCopperClear:
                _copperEvents[_copperTargetList].Clear();
                _copperListDirty[_copperTargetList] = true;
                break;
            case VgcConstants.CmdCopperEnable:
                _copperEnabled = true;
                break;
            case VgcConstants.CmdCopperDisable:
                _copperEnabled = false;
                break;
            case VgcConstants.CmdCopperList:
            {
                int idx = _cmdRegs[1] & 0x7F; // P0, clamped to 0-127
                _copperTargetList = idx;
                break;
            }
            case VgcConstants.CmdCopperUse:
            {
                int idx = _cmdRegs[1] & 0x7F; // P0, clamped to 0-127
                _copperPendingList = idx;
                break;
            }
            case VgcConstants.CmdCopperListEnd:
                _copperTargetList = _copperActiveList;
                break;
        }
    }

    private void AddCopperEventFromRegisters()
    {
        int x = _cmdRegs[1] | (_cmdRegs[2] << 8);   // P0/P1
        int y = _cmdRegs[3];                        // P2
        int regSpecifier = _cmdRegs[4] | (_cmdRegs[5] << 8); // P3/P4
        byte value = _cmdRegs[6];                   // P5

        if ((uint)x >= VgcConstants.GfxWidth || (uint)y >= VgcConstants.GfxHeight)
            return;
        if (!TryResolveCopperRegister(regSpecifier, out byte registerIndex))
            return;

        ushort position = (ushort)(y * VgcConstants.GfxWidth + x);
        var events = _copperEvents[_copperTargetList];

        for (int i = 0; i < events.Count; i++)
        {
            var existing = events[i];
            if (existing.Position != position || existing.RegisterIndex != registerIndex)
                continue;
            events[i] = new CopperEvent(position, registerIndex, value);
            _copperListDirty[_copperTargetList] = true;
            return;
        }

        if (events.Count >= VgcConstants.MaxCopperEntriesPerList)
            return;

        events.Add(new CopperEvent(position, registerIndex, value));
        _copperListDirty[_copperTargetList] = true;
    }

    private static bool TryResolveCopperRegister(int registerSpecifier, out byte registerIndex)
    {
        registerIndex = 0;

        // Direct index: 0-15 for VGC core registers
        if ((uint)registerSpecifier < 16)
        {
            registerIndex = (byte)registerSpecifier;
            return IsCopperWritableRegister(registerIndex);
        }

        // Absolute address: VGC core registers $A000-$A00F
        if (registerSpecifier >= VgcConstants.VgcBase && registerSpecifier < VgcConstants.VgcBase + 16)
        {
            registerIndex = (byte)(registerSpecifier - VgcConstants.VgcBase);
            return IsCopperWritableRegister(registerIndex);
        }

        // Absolute address: Sprite registers $A040-$A0BF
        if (registerSpecifier >= VgcConstants.SpriteRegBase && registerSpecifier <= VgcConstants.SpriteRegEnd)
        {
            registerIndex = (byte)(registerSpecifier - VgcConstants.VgcBase);
            return IsCopperWritableRegister(registerIndex);
        }

        return false;
    }

    private static bool IsCopperWritableRegister(byte registerIndex) =>
        registerIndex == VgcConstants.RegMode - VgcConstants.VgcBase ||
        registerIndex == VgcConstants.RegBgCol - VgcConstants.VgcBase ||
        registerIndex == VgcConstants.RegScrollX - VgcConstants.VgcBase ||
        registerIndex == VgcConstants.RegScrollY - VgcConstants.VgcBase ||
        IsCopperSpriteRegister(registerIndex);

    private static bool IsCopperSpriteRegister(byte registerIndex)
    {
        int offset = registerIndex - (VgcConstants.SpriteRegBase - VgcConstants.VgcBase);
        return offset >= 0 && offset < VgcConstants.MaxSprites * VgcConstants.SpriteRegStride;
    }


    // -------------------------------------------------------------------------
    // Tile engine registers
    // -------------------------------------------------------------------------

    private byte ReadTileRegister(int address)
    {
        return address switch
        {
            VgcConstants.TileConfig => _tileConfig,
            VgcConstants.TileTransColor => _tileTransColor,
            VgcConstants.TileScrollXL => (byte)(_tileScrollX & 0xFF),
            VgcConstants.TileScrollXH => (byte)((_tileScrollX >> 8) & 0xFF),
            VgcConstants.TileScrollYL => (byte)(_tileScrollY & 0xFF),
            VgcConstants.TileScrollYH => (byte)((_tileScrollY >> 8) & 0xFF),
            VgcConstants.TileStatus => _tileStatus,
            VgcConstants.TileColL =>
                ReadAndClearTileCollisionLow(),
            VgcConstants.TileColH =>
                ReadAndClearTileCollisionHigh(),
            VgcConstants.TilePeekVal => _tilePeekVal,
            VgcConstants.TilePeekAttr => _tilePeekAttr,
            _ => _tileRegs[address - VgcConstants.TileRegBase]
        };
    }

    private byte ReadAndClearTileCollisionLow()
    {
        byte val = (byte)(_tileCollision & 0xFF);
        _tileCollision = 0;
        return val;
    }

    private byte ReadAndClearTileCollisionHigh()
    {
        byte val = (byte)((_tileCollision >> 8) & 0xFF);
        _tileCollision = 0;
        return val;
    }

    private void WriteTileRegister(int address, byte data)
    {
        _tileRegs[address - VgcConstants.TileRegBase] = data;

        switch (address)
        {
            case VgcConstants.TileConfig:
                _tileConfig = data;
                break;
            case VgcConstants.TileTransColor:
                _tileTransColor = (byte)(data & 0x0F);
                break;
            case VgcConstants.TileScrollXL:
                _tileScrollX = (_tileScrollX & 0xFF00) | data;
                break;
            case VgcConstants.TileScrollXH:
                _tileScrollX = (data << 8) | (_tileScrollX & 0xFF);
                break;
            case VgcConstants.TileScrollYL:
                _tileScrollY = (_tileScrollY & 0xFF00) | data;
                break;
            case VgcConstants.TileScrollYH:
                _tileScrollY = (data << 8) | (_tileScrollY & 0xFF);
                break;
            case VgcConstants.TileCmd:
                ExecuteTileCommand(data);
                break;
            // All other registers (params, palette params, addr) are just stored
        }
    }

    // -------------------------------------------------------------------------
    // Tile engine commands
    // -------------------------------------------------------------------------

    private void ExecuteTileCommand(byte cmd)
    {
        _tileStatus = 0;
        byte p0 = _tileRegs[VgcConstants.TileP0 - VgcConstants.TileRegBase];
        byte p1 = _tileRegs[VgcConstants.TileP1 - VgcConstants.TileRegBase];
        byte p2 = _tileRegs[VgcConstants.TileP2 - VgcConstants.TileRegBase];
        byte p3 = _tileRegs[VgcConstants.TileP3 - VgcConstants.TileRegBase];
        int addr = _tileRegs[VgcConstants.TileAddrL - VgcConstants.TileRegBase]
                 | (_tileRegs[VgcConstants.TileAddrH - VgcConstants.TileRegBase] << 8);
        byte palP0 = _tileRegs[VgcConstants.TilePalP0 - VgcConstants.TileRegBase];
        byte palP1 = _tileRegs[VgcConstants.TilePalP1 - VgcConstants.TileRegBase];

        switch (cmd)
        {
            case VgcConstants.TileCmdDef:
                TileDefine(p0, addr);
                break;
            case VgcConstants.TileCmdDefBulk:
                TileDefineBulk(p0, p1, addr);
                break;
            case VgcConstants.TileCmdPut:
                TilePut(p0, p1, p2, p3);
                break;
            case VgcConstants.TileCmdAttr:
                TileSetAttr(p0, p1, p2, p3);
                break;
            case VgcConstants.TileCmdFill:
                TileFillNametable(p0, p1);
                break;
            case VgcConstants.TileCmdRow:
                TileWriteRow(p0, p1, addr);
                break;
            case VgcConstants.TileCmdCol:
                TileWriteCol(p0, p1, addr);
                break;
            case VgcConstants.TileCmdLoad:
                TileLoadNametable(p0, addr);
                break;
            case VgcConstants.TileCmdALoad:
                TileLoadAttrTable(p0, addr);
                break;
            case VgcConstants.TileCmdPal:
                TileLoadPalette(palP0, addr);
                break;
            case VgcConstants.TileCmdPalC:
                TileSetPaletteColor(palP0, palP1, p0, p1, p2);
                break;
            case VgcConstants.TileCmdPeek:
                TilePeek(p0, p1, p2);
                break;
            case VgcConstants.TileCmdRowAttr:
                TileWriteRowAttr(p0, p1, addr);
                break;
            case VgcConstants.TileCmdColAttr:
                TileWriteColAttr(p0, p1, addr);
                break;
            case VgcConstants.TileCmdCls:
                TileClearAll();
                break;
            case VgcConstants.TileCmdBufFill:
                TileBufferFill(p0);
                break;
            case VgcConstants.TileCmdBufSet:
                TileBufferSet(p0, p1);
                break;
            case VgcConstants.TileCmdBufRange:
                TileBufferRange(p0, p1, p2);
                break;
            case VgcConstants.TileCmdBufPut:
                TileBufferPut(p0, p1);
                break;
            default:
                _tileStatus = 1; // unknown command
                break;
        }
    }

    private int CurrentTileSize =>
        (_tileConfig & VgcConstants.TileCfgSize16) != 0
            ? VgcConstants.TileSize16
            : VgcConstants.TileSize8;

    private int ResolveMirroredNT(int nt)
    {
        int mirror = (_tileConfig & VgcConstants.TileCfgMirrorMask) >> VgcConstants.TileCfgMirrorShift;
        return mirror switch
        {
            VgcConstants.TileMirrorH => nt switch      // NT0=NT1, NT2=NT3
            {
                1 => 0,
                3 => 2,
                _ => nt
            },
            VgcConstants.TileMirrorV => nt switch      // NT0=NT2, NT1=NT3
            {
                2 => 0,
                3 => 1,
                _ => nt
            },
            VgcConstants.TileMirrorSingle => 0,         // all = NT0
            _ => nt                                      // four-screen: no mirroring
        };
    }

    private void TileDefine(int tileIndex, int cpuAddr)
    {
        if ((uint)tileIndex >= VgcConstants.TileCount || _busMemory == null) { _tileStatus = 1; return; }
        int size = CurrentTileSize;
        int offset = tileIndex * size;
        lock (_tileLock)
        {
            for (int i = 0; i < size && (cpuAddr + i) < _busMemory.Length; i++)
                _tileData[offset + i] = _busMemory[cpuAddr + i];
            _tileDirty = true;
        }
    }

    private void TileDefineBulk(int startTile, int count, int cpuAddr)
    {
        if (_busMemory == null) { _tileStatus = 1; return; }
        int size = CurrentTileSize;
        lock (_tileLock)
        {
            for (int t = 0; t < count && (startTile + t) < VgcConstants.TileCount; t++)
            {
                int offset = (startTile + t) * size;
                int src = cpuAddr + t * size;
                for (int i = 0; i < size && (src + i) < _busMemory.Length; i++)
                    _tileData[offset + i] = _busMemory[src + i];
            }
            _tileDirty = true;
        }
    }

    private void TilePut(int nt, int x, int y, byte tileIndex)
    {
        nt = ResolveMirroredNT(nt & 0x03);
        if ((uint)x >= VgcConstants.NametableCols || (uint)y >= VgcConstants.NametableRows) return;
        _nametables[nt][y * VgcConstants.NametableCols + x] = tileIndex;
    }

    private void TileSetAttr(int nt, int x, int y, byte attr)
    {
        nt = ResolveMirroredNT(nt & 0x03);
        if ((uint)x >= VgcConstants.NametableCols || (uint)y >= VgcConstants.NametableRows) return;
        _attrTables[nt][y * VgcConstants.NametableCols + x] = attr;
    }

    private void TileFillNametable(int nt, byte tileIndex)
    {
        nt = ResolveMirroredNT(nt & 0x03);
        Array.Fill(_nametables[nt], tileIndex);
        Array.Clear(_attrTables[nt]);
    }

    private void TileWriteRow(int nt, int row, int cpuAddr)
    {
        nt = ResolveMirroredNT(nt & 0x03);
        if ((uint)row >= VgcConstants.NametableRows || _busMemory == null) return;
        int offset = row * VgcConstants.NametableCols;
        for (int i = 0; i < VgcConstants.NametableCols && (cpuAddr + i) < _busMemory.Length; i++)
            _nametables[nt][offset + i] = _busMemory[cpuAddr + i];
    }

    private void TileWriteCol(int nt, int col, int cpuAddr)
    {
        nt = ResolveMirroredNT(nt & 0x03);
        if ((uint)col >= VgcConstants.NametableCols || _busMemory == null) return;
        for (int r = 0; r < VgcConstants.NametableRows && (cpuAddr + r) < _busMemory.Length; r++)
            _nametables[nt][r * VgcConstants.NametableCols + col] = _busMemory[cpuAddr + r];
    }

    private void TileBufferFill(byte tile)
    {
        Array.Fill(_columnBuffer, tile);
    }

    private void TileBufferSet(int row, byte tile)
    {
        if ((uint)row < VgcConstants.NametableRows)
            _columnBuffer[row] = tile;
    }

    private void TileBufferRange(int y1, int y2, byte tile)
    {
        if (y1 < 0) y1 = 0;
        if (y2 >= VgcConstants.NametableRows) y2 = VgcConstants.NametableRows - 1;
        for (int r = y1; r <= y2; r++)
            _columnBuffer[r] = tile;
    }

    private void TileBufferPut(int nt, int col)
    {
        nt = ResolveMirroredNT(nt & 0x03);
        if ((uint)col >= VgcConstants.NametableCols) return;
        for (int r = 0; r < VgcConstants.NametableRows; r++)
            _nametables[nt][r * VgcConstants.NametableCols + col] = _columnBuffer[r];
    }

    private void TileLoadNametable(int nt, int cpuAddr)
    {
        nt = ResolveMirroredNT(nt & 0x03);
        if (_busMemory == null) return;
        int len = Math.Min(VgcConstants.NametableSize, _busMemory.Length - cpuAddr);
        if (len <= 0) return;
        Array.Copy(_busMemory, cpuAddr, _nametables[nt], 0, len);
    }

    private void TileLoadAttrTable(int nt, int cpuAddr)
    {
        nt = ResolveMirroredNT(nt & 0x03);
        if (_busMemory == null) return;
        int len = Math.Min(VgcConstants.NametableSize, _busMemory.Length - cpuAddr);
        if (len <= 0) return;
        Array.Copy(_busMemory, cpuAddr, _attrTables[nt], 0, len);
    }

    private void TileLoadPalette(int subPalette, int cpuAddr)
    {
        if ((uint)subPalette >= VgcConstants.TilePaletteCount || _busMemory == null) { _tileStatus = 1; return; }
        int offset = subPalette * VgcConstants.TilePaletteColors * 3;
        int bytes = VgcConstants.TilePaletteColors * 3; // 48
        for (int i = 0; i < bytes && (cpuAddr + i) < _busMemory.Length; i++)
            _tilePaletteRam[offset + i] = _busMemory[cpuAddr + i];
    }

    private void TileSetPaletteColor(int subPalette, int colorIndex, byte r, byte g, byte b)
    {
        if ((uint)subPalette >= VgcConstants.TilePaletteCount ||
            (uint)colorIndex >= VgcConstants.TilePaletteColors) { _tileStatus = 1; return; }
        int offset = (subPalette * VgcConstants.TilePaletteColors + colorIndex) * 3;
        _tilePaletteRam[offset] = r;
        _tilePaletteRam[offset + 1] = g;
        _tilePaletteRam[offset + 2] = b;
    }

    private void TilePeek(int nt, int x, int y)
    {
        nt = ResolveMirroredNT(nt & 0x03);
        if ((uint)x >= VgcConstants.NametableCols || (uint)y >= VgcConstants.NametableRows)
        {
            _tilePeekVal = 0;
            _tilePeekAttr = 0;
            return;
        }
        int idx = y * VgcConstants.NametableCols + x;
        _tilePeekVal = _nametables[nt][idx];
        _tilePeekAttr = _attrTables[nt][idx];
    }

    private void TileWriteRowAttr(int nt, int row, int cpuAddr)
    {
        nt = ResolveMirroredNT(nt & 0x03);
        if ((uint)row >= VgcConstants.NametableRows || _busMemory == null) return;
        int offset = row * VgcConstants.NametableCols;
        for (int i = 0; i < VgcConstants.NametableCols && (cpuAddr + i) < _busMemory.Length; i++)
            _attrTables[nt][offset + i] = _busMemory[cpuAddr + i];
    }

    private void TileWriteColAttr(int nt, int col, int cpuAddr)
    {
        nt = ResolveMirroredNT(nt & 0x03);
        if ((uint)col >= VgcConstants.NametableCols || _busMemory == null) return;
        for (int r = 0; r < VgcConstants.NametableRows && (cpuAddr + r) < _busMemory.Length; r++)
            _attrTables[nt][r * VgcConstants.NametableCols + col] = _busMemory[cpuAddr + r];
    }

    private void TileClearAll()
    {
        for (int i = 0; i < VgcConstants.NametableCount; i++)
        {
            Array.Clear(_nametables[i]);
            Array.Clear(_attrTables[i]);
        }
    }

    // -------------------------------------------------------------------------
    // Tile engine accessors (for renderer and file I/O)
    // -------------------------------------------------------------------------

    public byte GetTileConfig() => _tileConfig;
    public byte GetTileTransColor() => _tileTransColor;
    public int GetTileScrollX() => _tileScrollX;
    public int GetTileScrollY() => _tileScrollY;

    public bool IsTileSize16 => (_tileConfig & VgcConstants.TileCfgSize16) != 0;

    public int GetTileMirrorMode() =>
        (_tileConfig & VgcConstants.TileCfgMirrorMask) >> VgcConstants.TileCfgMirrorShift;

    public void SetTileCollision(ushort mask) => _tileCollision |= mask;

    /// <summary>Copies tile definition data under lock. Returns true if data was dirty.</summary>
    public bool SnapshotTileData(byte[] buffer)
    {
        bool wasDirty = _tileDirty;
        lock (_tileLock)
        {
            Array.Copy(_tileData, buffer, Math.Min(_tileData.Length, buffer.Length));
            _tileDirty = false;
        }
        return wasDirty;
    }

    /// <summary>Copies a nametable for rendering.</summary>
    public void CopyNametable(int nt, byte[] dest)
    {
        if ((uint)nt < VgcConstants.NametableCount)
            Array.Copy(_nametables[nt], dest, VgcConstants.NametableSize);
    }

    /// <summary>Copies an attribute table for rendering.</summary>
    public void CopyAttrTable(int nt, byte[] dest)
    {
        if ((uint)nt < VgcConstants.NametableCount)
            Array.Copy(_attrTables[nt], dest, VgcConstants.NametableSize);
    }

    /// <summary>Copies palette RAM for rendering.</summary>
    public void CopyTilePaletteRam(byte[] dest) =>
        Array.Copy(_tilePaletteRam, dest, VgcConstants.TilePaletteRamSize);

    /// <summary>Gets raw nametable reference (for tests).</summary>
    public byte[] GetNametable(int nt) => _nametables[ResolveMirroredNT(nt & 0x03)];

    /// <summary>Gets raw attribute table reference (for tests).</summary>
    public byte[] GetAttrTable(int nt) => _attrTables[ResolveMirroredNT(nt & 0x03)];

    /// <summary>Gets raw tile data reference (for tests).</summary>
    public ReadOnlySpan<byte> GetTileData() => _tileData;

    /// <summary>Gets raw palette RAM reference (for tests).</summary>
    public ReadOnlySpan<byte> GetTilePaletteRam() => _tilePaletteRam;

    /// <summary>Bulk-set nametable data (for file I/O load).</summary>
    public void SetNametable(int nt, byte[] data)
    {
        if ((uint)nt < VgcConstants.NametableCount)
            Array.Copy(data, _nametables[nt], Math.Min(data.Length, VgcConstants.NametableSize));
    }

    /// <summary>Bulk-set attribute table data (for file I/O load).</summary>
    public void SetAttrTable(int nt, byte[] data)
    {
        if ((uint)nt < VgcConstants.NametableCount)
            Array.Copy(data, _attrTables[nt], Math.Min(data.Length, VgcConstants.NametableSize));
    }

    /// <summary>Bulk-set tile definitions (for file I/O load).</summary>
    public void SetTileData(byte[] data)
    {
        lock (_tileLock)
        {
            Array.Copy(data, _tileData, Math.Min(data.Length, _tileData.Length));
            _tileDirty = true;
        }
    }

    /// <summary>Bulk-set palette RAM (for file I/O load).</summary>
    public void SetTilePaletteRam(byte[] data) =>
        Array.Copy(data, _tilePaletteRam, Math.Min(data.Length, _tilePaletteRam.Length));

    /// <summary>Set tile config register directly (for file I/O load).</summary>
    public void SetTileConfig(byte config) => _tileConfig = config;

    /// <summary>Set tile transparent color directly (for file I/O load).</summary>
    public void SetTileTransColor(byte color) => _tileTransColor = (byte)(color & 0x0F);

    // -------------------------------------------------------------------------
    // Character output
    // -------------------------------------------------------------------------

    protected virtual void HandleCharOut(byte data)
    {
        int cx = _regs[VgcConstants.RegCursorX - VgcConstants.VgcBase];
        int cy = _regs[VgcConstants.RegCursorY - VgcConstants.VgcBase];

        switch (data)
        {
            case 0x08: // BS -- backspace: move left, erase
                if (cx > 0)
                    cx--;
                _screenRam[cy * VgcConstants.ScreenCols + cx] = 0x20;
                break;

            case 0x0A: // LF -- line feed: advance row
                cy++;
                if (cy >= VgcConstants.ScreenRows)
                {
                    ScrollUp();
                    cy = VgcConstants.ScreenRows - 1;
                }
                break;

            case 0x0C: // FF -- form feed: clear screen, home cursor
                Array.Fill(_screenRam, (byte)0x20);
                byte fgCol = _regs[VgcConstants.RegFgCol - VgcConstants.VgcBase];
                Array.Fill(_colorRam, fgCol);
                cx = 0;
                cy = 0;
                break;

            case 0x0D: // CR -- carriage return: col 0
                cx = 0;
                break;

            case 0x13: // HOME -- cursor to 0,0
                cx = 0;
                cy = 0;
                break;

            default:
                if (data >= 0x20) // printable
                {
                    // When a PETSCII font is active (slot 1 = upper, slot 2 = lower),
                    // map lowercase ASCII letters to uppercase so EhBASIC output like
                    // "Ready" renders correctly (PETSCII uppercase set has no lowercase).
                    byte ch = data;
                    int fontSlot = _regs[VgcConstants.RegFont - VgcConstants.VgcBase];
                    if (fontSlot == 1 && ch >= 0x61 && ch <= 0x7A)
                        ch -= 0x20; // a-z -> A-Z

                    int idx = cy * VgcConstants.ScreenCols + cx;
                    _screenRam[idx] = ch;
                    _colorRam[idx] = _regs[VgcConstants.RegFgCol - VgcConstants.VgcBase];
                    cx++;
                    if (cx >= VgcConstants.ScreenCols)
                    {
                        cx = 0;
                        cy++;
                        if (cy >= VgcConstants.ScreenRows)
                        {
                            ScrollUp();
                            cy = VgcConstants.ScreenRows - 1;
                        }
                    }
                }
                break;
        }

        _regs[VgcConstants.RegCursorX - VgcConstants.VgcBase] = (byte)cx;
        _regs[VgcConstants.RegCursorY - VgcConstants.VgcBase] = (byte)cy;
    }

    private void ScrollUp()
    {
        // Shift screen RAM rows 1-24 up to rows 0-23
        Array.Copy(_screenRam, VgcConstants.ScreenCols, _screenRam, 0,
            VgcConstants.ScreenCols * (VgcConstants.ScreenRows - 1));

        // Shift color RAM rows 1-24 up to rows 0-23
        Array.Copy(_colorRam, VgcConstants.ScreenCols, _colorRam, 0,
            VgcConstants.ScreenCols * (VgcConstants.ScreenRows - 1));

        // Clear last row to spaces; color RAM to current fg color
        int lastRowStart = (VgcConstants.ScreenRows - 1) * VgcConstants.ScreenCols;
        Array.Fill(_screenRam, (byte)0x20, lastRowStart, VgcConstants.ScreenCols);
        byte fgCol = _regs[VgcConstants.RegFgCol - VgcConstants.VgcBase];
        Array.Fill(_colorRam, fgCol, lastRowStart, VgcConstants.ScreenCols);
    }
}

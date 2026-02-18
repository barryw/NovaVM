namespace e6502.TUI.Hardware;

public class VirtualGraphicsController
{
    // Core registers $A000-$A00F (16 bytes)
    private readonly byte[] _regs = new byte[16];

    // Sprite registers $A010-$A06F (96 bytes: 16 sprites x 6 bytes)
    private readonly byte[] _spriteRegs = new byte[VgcConstants.MaxSprites * VgcConstants.SpriteBytesEach];

    // Gfx command registers $A070-$A078 (9 bytes)
    private readonly byte[] _gfxRegs = new byte[9];

    // Sprite shape data $A200-$A3FF (512 bytes: 16 sprites x 32 bytes)
    private readonly byte[] _spriteShapes = new byte[VgcConstants.MaxSprites * VgcConstants.SpriteShapeSize];

    // Screen RAM $A400-$ABCF (2000 bytes, 80x25)
    private readonly byte[] _screenRam = new byte[VgcConstants.ScreenSize];

    // Color RAM $ABD0-$B39F (2000 bytes, 80x25)
    private readonly byte[] _colorRam = new byte[VgcConstants.ScreenSize];

    // Block graphics bitmap (160x50, NOT 6502-addressable)
    private readonly byte[] _gfxBitmap = new byte[VgcConstants.GfxWidth * VgcConstants.GfxHeight];

    public VirtualGraphicsController()
    {
        // Screen RAM initialized to spaces
        Array.Fill(_screenRam, (byte)0x20);

        // Color RAM initialized to 1 (white)
        Array.Fill(_colorRam, (byte)1);

        // Default foreground color = 1
        _regs[VgcConstants.RegFgCol - VgcConstants.VgcBase] = 1;
    }

    // -------------------------------------------------------------------------
    // Address ownership
    // -------------------------------------------------------------------------

    public bool OwnsAddress(ushort address) =>
        address >= VgcConstants.VgcBase && address <= VgcConstants.ColorRamEnd;

    // -------------------------------------------------------------------------
    // Bus read
    // -------------------------------------------------------------------------

    public byte Read(ushort address)
    {
        if (address >= VgcConstants.ColorRamBase && address <= VgcConstants.ColorRamEnd)
            return _colorRam[address - VgcConstants.ColorRamBase];

        if (address >= VgcConstants.CharRamBase && address <= VgcConstants.CharRamEnd)
            return _screenRam[address - VgcConstants.CharRamBase];

        if (address >= VgcConstants.SpriteShapeBase && address <= VgcConstants.SpriteShapeEnd)
            return _spriteShapes[address - VgcConstants.SpriteShapeBase];

        if (address >= VgcConstants.RegGfxCmd && address <= VgcConstants.RegGfxP7)
            return _gfxRegs[address - VgcConstants.RegGfxCmd];

        if (address >= VgcConstants.SpriteRegsBase && address <= VgcConstants.SpriteRegsEnd)
            return _spriteRegs[address - VgcConstants.SpriteRegsBase];

        if (address >= VgcConstants.VgcBase && address <= VgcConstants.VgcEnd)
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
        if (address >= VgcConstants.ColorRamBase && address <= VgcConstants.ColorRamEnd)
        {
            _colorRam[address - VgcConstants.ColorRamBase] = data;
            return;
        }

        if (address >= VgcConstants.CharRamBase && address <= VgcConstants.CharRamEnd)
        {
            _screenRam[address - VgcConstants.CharRamBase] = data;
            return;
        }

        if (address >= VgcConstants.SpriteShapeBase && address <= VgcConstants.SpriteShapeEnd)
        {
            _spriteShapes[address - VgcConstants.SpriteShapeBase] = data;
            return;
        }

        if (address >= VgcConstants.RegGfxCmd && address <= VgcConstants.RegGfxP7)
        {
            _gfxRegs[address - VgcConstants.RegGfxCmd] = data;
            if (address == VgcConstants.RegGfxCmd)
                ExecuteGfxCommand(data);
            return;
        }

        if (address >= VgcConstants.SpriteRegsBase && address <= VgcConstants.SpriteRegsEnd)
        {
            _spriteRegs[address - VgcConstants.SpriteRegsBase] = data;
            return;
        }

        if (address >= VgcConstants.VgcBase && address <= VgcConstants.VgcEnd)
        {
            switch (address)
            {
                case VgcConstants.RegStatus:
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

    public byte GetBorderColor() =>
        _regs[VgcConstants.RegBorder - VgcConstants.VgcBase];

    public byte GetCursorX() =>
        _regs[VgcConstants.RegCursorX - VgcConstants.VgcBase];

    public byte GetCursorY() =>
        _regs[VgcConstants.RegCursorY - VgcConstants.VgcBase];

    public bool GetGfxPixel(int x, int y) =>
        _gfxBitmap[y * VgcConstants.GfxWidth + x] != 0;

    public ReadOnlySpan<byte> GetSpriteShape(int index) =>
        _spriteShapes.AsSpan(index * VgcConstants.SpriteShapeSize, VgcConstants.SpriteShapeSize);

    public (int x, int y, byte color, bool enabled, int shapeIdx, byte flags, byte priority) GetSpriteState(int index)
    {
        int baseIdx = index * VgcConstants.SpriteBytesEach;
        int xLow    = _spriteRegs[baseIdx + VgcConstants.SprOffXLow];
        byte xHighFlags = _spriteRegs[baseIdx + VgcConstants.SprOffXHighFlags];
        int x       = xLow | ((xHighFlags & 0x01) << 8);
        byte flags  = (byte)(xHighFlags & 0xFE);
        int y       = _spriteRegs[baseIdx + VgcConstants.SprOffY];
        byte color  = _spriteRegs[baseIdx + VgcConstants.SprOffColor];
        byte priority = _spriteRegs[baseIdx + VgcConstants.SprOffPriority];
        int shapeIdx = _spriteRegs[baseIdx + VgcConstants.SprOffShape];

        // enabled bit: SpriteEn register, bit = sprite index
        bool enabled = index < 8
            ? (_regs[VgcConstants.RegSpriteEn   - VgcConstants.VgcBase] & (1 << index)) != 0
            : (_regs[VgcConstants.RegSpriteEnH  - VgcConstants.VgcBase] & (1 << (index - 8))) != 0;

        return (x, y, color, enabled, shapeIdx, flags, priority);
    }

    // -------------------------------------------------------------------------
    // Stubs (filled in later tasks)
    // -------------------------------------------------------------------------

    protected virtual void HandleCharOut(byte data) { }

    protected virtual void ExecuteGfxCommand(byte cmd) { }

    // -------------------------------------------------------------------------
    // Internal status write (for the emulator core to set STATUS)
    // -------------------------------------------------------------------------

    public void SetStatus(byte value) =>
        _regs[VgcConstants.RegStatus - VgcConstants.VgcBase] = value;
}

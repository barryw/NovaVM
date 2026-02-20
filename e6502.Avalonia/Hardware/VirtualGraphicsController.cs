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
/// (16 sprites, 16x16 pixels, 4-bit color per pixel).
///
/// Address ownership:
///   $A000-$A01E  VGC registers + command interface
///   $AA00-$B1CF  Character RAM (2000 bytes)
///   $B1D0-$B99F  Color RAM (2000 bytes)
///
/// Sprite shape data is host-side only (not 6502-addressable).
/// All sprite manipulation goes through the command register at $A010.
/// </summary>
public class VirtualGraphicsController
{
    // Core registers $A000-$A00F (16 bytes)
    private readonly byte[] _regs = new byte[16];

    // Command registers $A010-$A01E (15 bytes: [0]=cmd, [1..14]=P0..P13)
    private readonly byte[] _cmdRegs = new byte[15];

    // Sprite shape data: 16 sprites x 128 bytes each (4-bit color, 16x16 pixels)
    private readonly byte[] _spriteShapes = new byte[VgcConstants.MaxSprites * VgcConstants.SpriteShapeSize];

    // Sprite state arrays (host-side, not bus-mapped)
    private readonly bool[] _spriteEnabled = new bool[VgcConstants.MaxSprites];
    private readonly int[] _spriteX = new int[VgcConstants.MaxSprites];
    private readonly int[] _spriteY = new int[VgcConstants.MaxSprites];
    private readonly byte[] _spriteFlags = new byte[VgcConstants.MaxSprites];
    private readonly byte[] _spritePriority = new byte[VgcConstants.MaxSprites];

    // Screen RAM (2000 bytes, 80x25)
    private readonly byte[] _screenRam = new byte[VgcConstants.ScreenSize];

    // Color RAM (2000 bytes, 80x25)
    private readonly byte[] _colorRam = new byte[VgcConstants.ScreenSize];

    // Block graphics bitmap (320x200, NOT 6502-addressable)
    private readonly byte[] _gfxBitmap = new byte[VgcConstants.GfxWidth * VgcConstants.GfxHeight];

    // Current graphics draw color (0 = use text foreground color)
    private byte _gfxDrawColor;

    // IRQ control register ($A01F)
    private byte _irqCtrl;


    // Screen input source
    private IScreenInput? _screenEditor;

    public VirtualGraphicsController()
    {
        Reset();
    }

    public void Reset()
    {
        Array.Clear(_regs);
        Array.Clear(_cmdRegs);
        Array.Clear(_spriteShapes);
        Array.Clear(_spriteEnabled);
        Array.Clear(_spriteX);
        Array.Clear(_spriteY);
        Array.Clear(_spriteFlags);
        Array.Clear(_gfxBitmap);
        _gfxDrawColor = 0;
        _irqCtrl = 0;

        // Default sprite priority: in front of all
        Array.Fill(_spritePriority, (byte)VgcConstants.SpritePriInFront);

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

    public bool OwnsAddress(ushort address)
    {
        // VGC registers + command: $A000-$A01E
        if (address >= VgcConstants.VgcBase && address <= VgcConstants.VgcRegsEnd)
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

    public byte GetBorderColor() =>
        _regs[VgcConstants.RegBorder - VgcConstants.VgcBase];

    public byte GetCursorX() =>
        _regs[VgcConstants.RegCursorX - VgcConstants.VgcBase];

    public byte GetCursorY() =>
        _regs[VgcConstants.RegCursorY - VgcConstants.VgcBase];

    public bool IsCursorEnabled =>
        _regs[VgcConstants.RegCursorEnable - VgcConstants.VgcBase] != 0;

    public bool IsRasterIrqEnabled => (_irqCtrl & 0x01) != 0;

    public bool GetGfxPixel(int x, int y) =>
        _gfxBitmap[y * VgcConstants.GfxWidth + x] != 0;

    public byte GetGfxPixelColor(int x, int y) =>
        _gfxBitmap[y * VgcConstants.GfxWidth + x];

    public ReadOnlySpan<byte> GetSpriteShape(int index) =>
        _spriteShapes.AsSpan(index * VgcConstants.SpriteShapeSize, VgcConstants.SpriteShapeSize);

    public (int x, int y, bool enabled, byte flags, byte priority) GetSpriteState(int index) =>
        (_spriteX[index], _spriteY[index], _spriteEnabled[index],
         _spriteFlags[index], _spritePriority[index]);

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

    public void IncrementFrameCounter() =>
        _regs[VgcConstants.RegStatus - VgcConstants.VgcBase]++;

    // -------------------------------------------------------------------------
    // Command dispatch
    // -------------------------------------------------------------------------

    private void ExecuteCommand(byte cmd)
    {
        if (cmd >= VgcConstants.CmdSprDef && cmd <= VgcConstants.CmdSprPri)
            ExecuteSpriteCommand(cmd);
        else if (cmd == VgcConstants.CmdMemRead || cmd == VgcConstants.CmdMemWrite)
            ExecuteMemoryCommand(cmd);
        else if (cmd >= VgcConstants.CmdPlot && cmd <= VgcConstants.CmdPaint)
            ExecuteGfxCommand(cmd);
    }

    // -------------------------------------------------------------------------
    // Graphics commands
    // -------------------------------------------------------------------------

    private byte EffectiveDrawColor =>
        _gfxDrawColor != 0
            ? _gfxDrawColor
            : Math.Max((byte)1, _regs[VgcConstants.RegFgCol - VgcConstants.VgcBase]);

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
                BlockGraphics.Circle(_gfxBitmap, p0, p1, p2, color);
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
                int byteIdx = sprite * VgcConstants.SpriteShapeSize + y * VgcConstants.SpriteBytesPerRow + x / 2;
                if (x % 2 == 0) // high nibble
                    _spriteShapes[byteIdx] = (byte)((_spriteShapes[byteIdx] & 0x0F) | ((color & 0x0F) << 4));
                else // low nibble
                    _spriteShapes[byteIdx] = (byte)((_spriteShapes[byteIdx] & 0xF0) | (color & 0x0F));
                break;
            }
            case VgcConstants.CmdSprRow:
            {
                int sprite = p0;
                int row = _cmdRegs[2];
                if (sprite >= VgcConstants.MaxSprites || row >= 16) return;
                int baseIdx = sprite * VgcConstants.SpriteShapeSize + row * VgcConstants.SpriteBytesPerRow;
                for (int i = 0; i < VgcConstants.SpriteBytesPerRow; i++)
                    _spriteShapes[baseIdx + i] = _cmdRegs[3 + i]; // P2-P9
                break;
            }
            case VgcConstants.CmdSprClr:
            {
                if (p0 >= VgcConstants.MaxSprites) return;
                Array.Clear(_spriteShapes, p0 * VgcConstants.SpriteShapeSize, VgcConstants.SpriteShapeSize);
                break;
            }
            case VgcConstants.CmdSprCopy:
            {
                byte dest = _cmdRegs[2];
                if (p0 >= VgcConstants.MaxSprites || dest >= VgcConstants.MaxSprites) return;
                Array.Copy(_spriteShapes, p0 * VgcConstants.SpriteShapeSize,
                           _spriteShapes, dest * VgcConstants.SpriteShapeSize,
                           VgcConstants.SpriteShapeSize);
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
                value = _spriteShapes[addr];
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
                _spriteShapes[addr] = value;
                return true;
            default:
                return false;
        }
    }

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
                    int idx = cy * VgcConstants.ScreenCols + cx;
                    _screenRam[idx] = data;
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

namespace e6502.TUI.Hardware;

public static class VgcConstants
{
    // -------------------------------------------------------------------------
    // Memory map
    // -------------------------------------------------------------------------

    public const int ZeroPageBase    = 0x0000;
    public const int ZeroPageEnd     = 0x00FF;

    public const int StackBase       = 0x0100;
    public const int StackEnd        = 0x01FF;

    public const int BasicBase       = 0x0200;
    public const int BasicEnd        = 0x9FFF;

    public const int VgcBase         = 0xA000;
    public const int VgcEnd          = 0xA0FF;

    public const int VscBase         = 0xA100;
    public const int VscEnd          = 0xA1FF;

    public const int SpriteShapeBase = 0xA200;
    public const int SpriteShapeEnd  = 0xA3FF;

    public const int CharRamBase     = 0xA400;
    public const int CharRamEnd      = 0xABCF;   // A400 + 2000 - 1

    public const int ColorRamBase    = 0xABD0;
    public const int ColorRamEnd     = 0xB39F;   // ABD0 + 2000 - 1

    public const int FreeBase        = 0xB3A0;
    public const int FreeEnd         = 0xBFFF;

    public const int RomBase         = 0xC000;
    public const int RomEnd          = 0xFFFF;

    // -------------------------------------------------------------------------
    // Screen / graphics dimensions
    // -------------------------------------------------------------------------

    public const int ScreenCols      = 80;
    public const int ScreenRows      = 25;
    public const int ScreenSize      = ScreenCols * ScreenRows;  // 2000

    public const int GfxWidth        = 160;
    public const int GfxHeight       = 50;

    public const int MaxSprites      = 16;
    public const int SpriteShapeSize = 32;   // 16 px × 16 px, 2 bytes/row
    public const int PaletteSize     = 16;

    // -------------------------------------------------------------------------
    // VGC core registers ($A000-$A00F)
    // -------------------------------------------------------------------------

    public const int RegMode         = 0xA000;   // 0=text, 1=block, 2=mixed
    public const int RegBgCol        = 0xA001;
    public const int RegFgCol        = 0xA002;
    public const int RegCursorX      = 0xA003;   // 0-79
    public const int RegCursorY      = 0xA004;   // 0-24
    public const int RegScrollX      = 0xA005;
    public const int RegScrollY      = 0xA006;
    public const int RegBank         = 0xA007;   // reserved
    public const int RegStatus       = 0xA008;   // read-only
    public const int RegSpriteEn     = 0xA009;   // sprites 0-7
    public const int RegSpriteEnH    = 0xA00A;   // sprites 8-15
    public const int RegColSt        = 0xA00B;   // sprite-sprite collision (read clears)
    public const int RegColBg        = 0xA00C;   // sprite-background collision (read clears)
    public const int RegBorder       = 0xA00D;
    public const int RegCharOut      = 0xA00E;   // character output (replaces $F001)
    public const int RegCharIn       = 0xA00F;   // character input  (replaces $F004)

    // -------------------------------------------------------------------------
    // Sprite registers ($A010-$A06F): 16 sprites × 6 bytes each
    //   +0 X low, +1 X high/flags, +2 Y, +3 Color, +4 Priority, +5 Shape index
    // -------------------------------------------------------------------------

    public const int SpriteRegsBase  = 0xA010;
    public const int SpriteRegsEnd   = 0xA06F;
    public const int SpriteBytesEach = 6;

    public static int SpriteReg(int sprite, int offset) =>
        SpriteRegsBase + sprite * SpriteBytesEach + offset;

    // Sprite register offsets
    public const int SprOffXLow      = 0;
    public const int SprOffXHighFlags = 1;
    public const int SprOffY         = 2;
    public const int SprOffColor     = 3;
    public const int SprOffPriority  = 4;
    public const int SprOffShape     = 5;

    // -------------------------------------------------------------------------
    // Graphics command registers ($A070-$A078)
    // -------------------------------------------------------------------------

    public const int RegGfxCmd       = 0xA070;   // write to trigger
    public const int RegGfxP0        = 0xA071;
    public const int RegGfxP1        = 0xA072;
    public const int RegGfxP2        = 0xA073;
    public const int RegGfxP3        = 0xA074;
    public const int RegGfxP4        = 0xA075;
    public const int RegGfxP5        = 0xA076;
    public const int RegGfxP6        = 0xA077;
    public const int RegGfxP7        = 0xA078;

    // -------------------------------------------------------------------------
    // Graphics commands
    // -------------------------------------------------------------------------

    public const byte GfxCmdPlot     = 0x01;
    public const byte GfxCmdUnplot   = 0x02;
    public const byte GfxCmdLine     = 0x03;
    public const byte GfxCmdCircle   = 0x04;
    public const byte GfxCmdRect     = 0x05;
    public const byte GfxCmdFill     = 0x06;
    public const byte GfxCmdGcls     = 0x07;
    public const byte GfxCmdGcolor   = 0x08;
}

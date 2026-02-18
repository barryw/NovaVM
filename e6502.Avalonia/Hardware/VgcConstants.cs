namespace e6502.Avalonia.Hardware;

public static class VgcConstants
{
    // -------------------------------------------------------------------------
    // Memory map
    // -------------------------------------------------------------------------

    public const int ZeroPageBase      = 0x0000;
    public const int ZeroPageEnd       = 0x00FF;

    public const int StackBase         = 0x0100;
    public const int StackEnd          = 0x01FF;

    public const int VectorTableBase   = 0x0200;
    public const int VectorTableEnd    = 0x027F;

    public const int BasicBase         = 0x0280;
    public const int BasicEnd          = 0x9FFF;

    public const int VgcBase           = 0xA000;
    public const int VgcRegsEnd        = 0xA01E;
    public const int VgcEnd            = 0xA0FF;

    public const int VscBase           = 0xA100;
    public const int VscEnd            = 0xA1FF;

    public const int CharRamBase       = 0xAA00;
    public const int CharRamEnd        = 0xB1CF;   // AA00 + 2000 - 1

    public const int ColorRamBase      = 0xB1D0;
    public const int ColorRamEnd       = 0xB99F;   // B1D0 + 2000 - 1

    // -------------------------------------------------------------------------
    // File I/O coprocessor registers ($B9A0-$B9EF)
    // -------------------------------------------------------------------------

    public const int FioBase           = 0xB9A0;
    public const int FioEnd            = 0xB9EF;

    public const int FioCmd            = 0xB9A0;   // write triggers operation
    public const int FioStatus         = 0xB9A1;   // 0=idle, 2=ok, 3=error
    public const int FioErrCode        = 0xB9A2;   // 0=none, 1=not found, 2=io error
    public const int FioNameLen        = 0xB9A3;   // filename length (1-63)
    public const int FioSrcL           = 0xB9A4;   // source/dest addr low
    public const int FioSrcH           = 0xB9A5;   // source/dest addr high
    public const int FioEndL           = 0xB9A6;   // end addr low (SAVE only)
    public const int FioEndH           = 0xB9A7;   // end addr high (SAVE only)
    public const int FioSizeL          = 0xB9A8;   // loaded size low (set by host after LOAD)
    public const int FioSizeH          = 0xB9A9;   // loaded size high
    public const int FioName           = 0xB9B0;   // filename buffer (64 bytes ASCII)
    public const int FioNameEnd        = 0xB9EF;

    public const byte FioCmdSave       = 0x01;
    public const byte FioCmdLoad       = 0x02;

    public const byte FioStatusIdle    = 0x00;
    public const byte FioStatusOk      = 0x02;
    public const byte FioStatusError   = 0x03;

    public const byte FioErrNone       = 0x00;
    public const byte FioErrNotFound   = 0x01;
    public const byte FioErrIo         = 0x02;

    public const int FreeBase          = 0xBA00;
    public const int FreeEnd           = 0xBFFF;

    public const int RomBase           = 0xC000;
    public const int RomEnd            = 0xFFFF;

    // -------------------------------------------------------------------------
    // Screen / graphics dimensions
    // -------------------------------------------------------------------------

    public const int ScreenCols        = 80;
    public const int ScreenRows        = 25;
    public const int ScreenSize        = ScreenCols * ScreenRows;  // 2000

    public const int GfxWidth          = 160;
    public const int GfxHeight         = 50;

    public const int MaxSprites        = 16;
    public const int SpritePixelSize   = 16;       // 16x16 pixels
    public const int SpriteBytesPerRow = 8;        // 16 pixels x 4 bits / 8
    public const int SpriteShapeSize   = 128;      // 4-bit color per pixel
    public const int PaletteSize       = 16;

    // Sprite priority values
    public const int SpritePriBehindAll = 0;
    public const int SpritePriBetween   = 1;
    public const int SpritePriInFront   = 2;

    // -------------------------------------------------------------------------
    // VGC core registers ($A000-$A00F)
    // -------------------------------------------------------------------------

    public const int RegMode           = 0xA000;   // 0=text, 1=block, 2=mixed
    public const int RegBgCol          = 0xA001;
    public const int RegFgCol          = 0xA002;
    public const int RegCursorX        = 0xA003;   // 0-79
    public const int RegCursorY        = 0xA004;   // 0-24
    public const int RegScrollX        = 0xA005;
    public const int RegScrollY        = 0xA006;
    public const int RegBank           = 0xA007;   // reserved
    public const int RegStatus         = 0xA008;   // read-only
    public const int RegSpriteCount    = 0xA009;   // read-only sprite count
    public const int RegReservedA      = 0xA00A;   // reserved
    public const int RegColSt          = 0xA00B;   // sprite-sprite collision (read clears)
    public const int RegColBg          = 0xA00C;   // sprite-background collision (read clears)
    public const int RegBorder         = 0xA00D;
    public const int RegCharOut        = 0xA00E;   // character output
    public const int RegCharIn         = 0xA00F;   // character input

    // -------------------------------------------------------------------------
    // Command register and parameters ($A010-$A01E)
    // -------------------------------------------------------------------------

    public const int RegCmd            = 0xA010;   // write triggers execution
    public const int RegP0             = 0xA011;
    public const int RegP1             = 0xA012;
    public const int RegP2             = 0xA013;
    public const int RegP3             = 0xA014;
    public const int RegP4             = 0xA015;
    public const int RegP5             = 0xA016;
    public const int RegP6             = 0xA017;
    public const int RegP7             = 0xA018;
    public const int RegP8             = 0xA019;
    public const int RegP9             = 0xA01A;
    public const int RegP10            = 0xA01B;
    public const int RegP11            = 0xA01C;
    public const int RegP12            = 0xA01D;
    public const int RegP13            = 0xA01E;

    // -------------------------------------------------------------------------
    // Graphics commands
    // -------------------------------------------------------------------------

    public const byte CmdPlot          = 0x01;
    public const byte CmdUnplot        = 0x02;
    public const byte CmdLine          = 0x03;
    public const byte CmdCircle        = 0x04;
    public const byte CmdRect          = 0x05;
    public const byte CmdFill          = 0x06;
    public const byte CmdGcls          = 0x07;
    public const byte CmdGcolor        = 0x08;

    // -------------------------------------------------------------------------
    // Sprite commands
    // -------------------------------------------------------------------------

    public const byte CmdSprDef        = 0x10;
    public const byte CmdSprRow        = 0x11;
    public const byte CmdSprClr        = 0x12;
    public const byte CmdSprCopy       = 0x13;
    public const byte CmdSprPos        = 0x14;
    public const byte CmdSprEna        = 0x15;
    public const byte CmdSprDis        = 0x16;
    public const byte CmdSprFlip       = 0x17;
    public const byte CmdSprPri        = 0x18;
}

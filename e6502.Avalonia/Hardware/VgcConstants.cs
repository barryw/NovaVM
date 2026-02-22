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
    public const int VgcRegsEnd        = 0xA01F;
    public const int VgcEnd            = 0xA0FF;

    // -------------------------------------------------------------------------
    // SID chip registers ($D400-$D41C, inside ROM space — write-only via bus)
    // -------------------------------------------------------------------------

    public const int SidBase           = 0xD400;
    public const int SidEnd            = 0xD41C;

    public const int Sid2Base          = 0xD420;
    public const int Sid2End           = 0xD43C;

    // Legacy mirror — SID files targeting $D500 are transparently routed to SID2
    public const int Sid2MirrorBase    = 0xD500;
    public const int Sid2MirrorEnd     = 0xD51C;

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
    public const int FioGSpace         = 0xB9AA;   // graphics space selector
    public const int FioGAddrL         = 0xB9AB;   // graphics offset low
    public const int FioGAddrH         = 0xB9AC;   // graphics offset high
    public const int FioGLenL          = 0xB9AD;   // graphics length low
    public const int FioGLenH          = 0xB9AE;   // graphics length high
    public const int FioDirType        = 0xB9AF;   // dir entry type: 0=PRG, 1=SID
    public const int FioName           = 0xB9B0;   // filename buffer (64 bytes ASCII)
    public const int FioNameEnd        = 0xB9EF;

    public const byte FioCmdSave       = 0x01;
    public const byte FioCmdLoad       = 0x02;
    public const byte FioCmdDirOpen    = 0x03;
    public const byte FioCmdDirRead    = 0x04;
    public const byte FioCmdDelete     = 0x05;
    public const byte FioCmdGSave      = 0x06;     // VGC memory -> disk
    public const byte FioCmdGLoad      = 0x07;     // disk -> VGC memory
    public const byte FioCmdSidPlay    = 0x08;     // .sid file → SID playback
    public const byte FioCmdSidStop    = 0x09;     // stop SID playback
    public const byte FioCmdInstrument = 0x0A;
    public const byte FioCmdSound      = 0x0B;
    public const byte FioCmdVolume     = 0x0C;
    public const byte FioCmdMusicSeq   = 0x0D;
    public const byte FioCmdMusicPlay  = 0x0E;
    public const byte FioCmdMusicStop  = 0x0F;
    public const byte FioCmdMusicTempo = 0x10;
    public const byte FioCmdMusicLoop  = 0x11;
    public const byte FioCmdMusicPri   = 0x12;

    // Music/SFX status register
    public const int  MusicStatus      = 0xBA50; // bit 0 = SFX playing, bit 1 = music playing
    public const int  MusicNote1       = 0xBA51; // voice 1 current MIDI note (0=silent)
    public const int  MusicNote2       = 0xBA52; // voice 2 current MIDI note
    public const int  MusicNote3       = 0xBA53; // voice 3 current MIDI note
    public const int  MusicNote4       = 0xBA54; // voice 4 current MIDI note
    public const int  MusicNote5       = 0xBA55; // voice 5 current MIDI note
    public const int  MusicNote6       = 0xBA56; // voice 6 current MIDI note

    public const byte FioStatusIdle    = 0x00;
    public const byte FioStatusOk      = 0x02;
    public const byte FioStatusError   = 0x03;

    public const byte FioErrNone       = 0x00;
    public const byte FioErrNotFound   = 0x01;
    public const byte FioErrIo         = 0x02;
    public const byte FioErrEndOfDir   = 0x03;

    // -------------------------------------------------------------------------
    // Expansion Memory Controller (XMC) registers ($BA00-$BA3F)
    // -------------------------------------------------------------------------

    public const int XmcBase           = 0xBA00;
    public const int XmcEnd            = 0xBA3F;

    public const int XmcCmd            = 0xBA00;   // write triggers operation
    public const int XmcStatus         = 0xBA01;   // 0=idle, 2=ok, 3=error
    public const int XmcErrCode        = 0xBA02;   // 0=none, 1=range, 2=bad args
    public const int XmcCfg            = 0xBA03;   // reserved
    public const int XmcAddrL          = 0xBA04;   // expansion addr low
    public const int XmcAddrM          = 0xBA05;   // expansion addr mid
    public const int XmcAddrH          = 0xBA06;   // expansion addr high
    public const int XmcRamL           = 0xBA07;   // CPU RAM addr low
    public const int XmcRamH           = 0xBA08;   // CPU RAM addr high
    public const int XmcLenL           = 0xBA09;   // transfer length low
    public const int XmcLenH           = 0xBA0A;   // transfer length high
    public const int XmcData           = 0xBA0B;   // byte data port
    public const int XmcBank           = 0xBA0C;   // default 64KB bank selector
    public const int XmcBanks          = 0xBA0D;   // total bank count (read-only)
    public const int XmcPagesUsedL     = 0xBA0E;   // used 256-byte pages low
    public const int XmcPagesUsedH     = 0xBA0F;   // used pages high
    public const int XmcPagesFreeL     = 0xBA10;   // free pages low
    public const int XmcPagesFreeH     = 0xBA11;   // free pages high
    public const int XmcNameLen        = 0xBA12;   // name length (0-27)
    public const int XmcHandle         = 0xBA13;   // block handle (output/selector)
    public const int XmcDirCountL      = 0xBA14;   // number of named blocks low
    public const int XmcDirCountH      = 0xBA15;   // number of named blocks high
    public const int XmcWinCtl         = 0xBA16;   // window enable bits (0-3)
    public const int XmcWin0AL         = 0xBA18;   // window 0 mapped XRAM base low
    public const int XmcWin0AM         = 0xBA19;   // window 0 mapped XRAM base mid
    public const int XmcWin0AH         = 0xBA1A;   // window 0 mapped XRAM base high
    public const int XmcWin1AL         = 0xBA1B;   // window 1 mapped XRAM base low
    public const int XmcWin1AM         = 0xBA1C;   // window 1 mapped XRAM base mid
    public const int XmcWin1AH         = 0xBA1D;   // window 1 mapped XRAM base high
    public const int XmcWin2AL         = 0xBA1E;   // window 2 mapped XRAM base low
    public const int XmcWin2AM         = 0xBA1F;   // window 2 mapped XRAM base mid
    public const int XmcWin2AH         = 0xBA20;   // window 2 mapped XRAM base high
    public const int XmcWin3AL         = 0xBA21;   // window 3 mapped XRAM base low
    public const int XmcWin3AM         = 0xBA22;   // window 3 mapped XRAM base mid
    public const int XmcWin3AH         = 0xBA23;   // window 3 mapped XRAM base high
    public const int XmcName           = 0xBA24;   // ASCII name buffer start (28 bytes)
    public const int XmcNameEnd        = 0xBA3F;   // ASCII name buffer end

    public const byte XmcCmdGetByte    = 0x01;     // read byte at XADDR into XDATA
    public const byte XmcCmdPutByte    = 0x02;     // write XDATA to XADDR
    public const byte XmcCmdStash      = 0x03;     // RAM -> XRAM
    public const byte XmcCmdFetch      = 0x04;     // XRAM -> RAM
    public const byte XmcCmdFill       = 0x05;     // fill XRAM range with XDATA
    public const byte XmcCmdStats      = 0x07;     // refresh stats registers
    public const byte XmcCmdResetUsage = 0x08;     // clear page usage tracking
    public const byte XmcCmdRelease    = 0x09;     // mark a range as free in usage tracking
    public const byte XmcCmdAlloc      = 0x0A;     // allocate len bytes, returns XADDR+handle
    public const byte XmcCmdNStash     = 0x0B;     // named stash: RAM->named block
    public const byte XmcCmdNFetch     = 0x0C;     // named fetch: named block->RAM
    public const byte XmcCmdNDelete    = 0x0D;     // delete named block
    public const byte XmcCmdNDirOpen   = 0x0E;     // open named block directory
    public const byte XmcCmdNDirRead   = 0x0F;     // read next named block directory entry

    public const byte XmcStatusIdle    = 0x00;
    public const byte XmcStatusOk      = 0x02;
    public const byte XmcStatusError   = 0x03;

    public const byte XmcErrNone       = 0x00;
    public const byte XmcErrRange      = 0x01;
    public const byte XmcErrBadArgs    = 0x02;
    public const byte XmcErrNotFound   = 0x03;
    public const byte XmcErrNoSpace    = 0x04;
    public const byte XmcErrName       = 0x05;
    public const byte XmcErrEndOfDir   = 0x06;

    // -------------------------------------------------------------------------
    // Timer controller registers ($BA40-$BA4F)
    // -------------------------------------------------------------------------

    public const int TimerBase         = 0xBA40;
    public const int TimerEnd          = 0xBA4F;
    public const int TimerCtrl         = 0xBA40;  // bit 0 = enable
    public const int TimerStatus       = 0xBA41;  // bit 0 = IRQ pending (read clears)
    public const int TimerDivL         = 0xBA42;  // divisor low byte
    public const int TimerDivH         = 0xBA43;  // divisor high byte

    public const int FreeBase          = 0xBA57;
    public const int FreeEnd           = 0xBFFF;

    // -------------------------------------------------------------------------
    // Network Interface Controller (NIC) registers ($A100-$A13F)
    // -------------------------------------------------------------------------

    public const int NicBase           = 0xA100;
    public const int NicEnd            = 0xA13F;

    public const int NicCmd            = 0xA100;   // write triggers command
    public const int NicStatus         = 0xA101;   // global status (read)
    public const int NicSlot           = 0xA102;   // active slot ID (0-3)
    public const int NicIrqCtrl        = 0xA103;   // IRQ enable mask (bit per slot)
    public const int NicIrqStatus      = 0xA104;   // IRQ pending flags (read-clears)

    public const int NicRemotePortL    = 0xA108;   // remote port low byte
    public const int NicRemotePortH    = 0xA109;   // remote port high byte
    public const int NicLocalPortL     = 0xA10A;   // local port low byte (listen)
    public const int NicLocalPortH     = 0xA10B;   // local port high byte

    public const int NicDmaAddrL       = 0xA110;   // DMA RAM address low
    public const int NicDmaAddrH       = 0xA111;   // DMA RAM address high
    public const int NicDmaLen         = 0xA112;   // DMA length (1-255, 0=256)
    public const int NicMsgLen         = 0xA113;   // received message length (read-only)

    public const int NicSlotStatus0    = 0xA118;   // slot 0 status
    public const int NicSlotStatus1    = 0xA119;   // slot 1 status
    public const int NicSlotStatus2    = 0xA11A;   // slot 2 status
    public const int NicSlotStatus3    = 0xA11B;   // slot 3 status

    public const int NicNameBuf        = 0xA120;   // hostname buffer (32 bytes)
    public const int NicNameEnd        = 0xA13F;   // end of hostname buffer

    // NIC commands
    public const byte NicCmdConnect    = 0x01;
    public const byte NicCmdDisconnect = 0x02;
    public const byte NicCmdSend       = 0x03;
    public const byte NicCmdRecv       = 0x04;
    public const byte NicCmdListen     = 0x05;
    public const byte NicCmdAccept     = 0x06;

    // NIC slot status bits
    public const byte NicSlotConnected    = 0x01;  // bit 0
    public const byte NicSlotDataReady    = 0x02;  // bit 1
    public const byte NicSlotSendReady    = 0x04;  // bit 2
    public const byte NicSlotError        = 0x08;  // bit 3
    public const byte NicSlotRemoteClosed = 0x10;  // bit 4

    // NIC global status bits
    public const byte NicStatusReady      = 0x01;  // bit 0
    public const byte NicStatusAnyData    = 0x02;  // bit 1
    public const byte NicStatusAnyError   = 0x80;  // bit 7

    public const int NicMaxSlots          = 4;
    public const int NicMaxMessageSize    = 256;
    public const int NicMaxQueueDepth     = 16;

    public const int RomBase           = 0xC000;
    public const int RomEnd            = 0xFFFF;

    // -------------------------------------------------------------------------
    // Screen / graphics dimensions
    // -------------------------------------------------------------------------

    public const int ScreenCols        = 80;
    public const int ScreenRows        = 25;
    public const int ScreenSize        = ScreenCols * ScreenRows;  // 2000

    public const int GfxWidth          = 320;
    public const int GfxHeight         = 200;

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

    public const int RegMode           = 0xA000;   // 0=text only, 1=gfx over text, 2=text over gfx, 3=gfx+sprites no text
    public const int RegBgCol          = 0xA001;
    public const int RegFgCol          = 0xA002;
    public const int RegCursorX        = 0xA003;   // 0-79
    public const int RegCursorY        = 0xA004;   // 0-24
    public const int RegScrollX        = 0xA005;
    public const int RegScrollY        = 0xA006;
    public const int RegBank           = 0xA007;   // reserved
    public const int RegStatus         = 0xA008;   // read-only
    public const int RegSpriteCount    = 0xA009;   // read-only sprite count
    public const int RegCursorEnable   = 0xA00A;   // cursor visible (non-zero = show)
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
    // IRQ control register ($A01F)
    // -------------------------------------------------------------------------

    public const int RegIrqCtrl        = 0xA01F;   // bit 0 = raster IRQ enable

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
    public const byte CmdPaint         = 0x09;

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

    // -------------------------------------------------------------------------
    // VGC memory I/O commands (via RegP0..RegP4)
    // -------------------------------------------------------------------------

    // P0=space, P1=addr low, P2=addr high, P3=data, P4=flags (bit0=auto-inc)
    public const byte CmdMemRead       = 0x19;     // read selected memory into P3
    public const byte CmdMemWrite      = 0x1A;     // write P3 into selected memory

    // -------------------------------------------------------------------------
    // Copper (raster) commands
    // -------------------------------------------------------------------------

    // CmdCopperAdd: P0=x low, P1=x high, P2=y, P3=reg low/index, P4=reg high, P5=value
    public const byte CmdCopperAdd     = 0x1B;
    public const byte CmdCopperClear   = 0x1C;
    public const byte CmdCopperEnable  = 0x1D;
    public const byte CmdCopperDisable = 0x1E;
    public const byte CmdSysReset      = 0x1F;

    public const byte CmdCopperList    = 0x20;     // P0 = list index (0-127)
    public const byte CmdCopperUse     = 0x21;     // P0 = list index (0-127)
    public const byte CmdCopperListEnd = 0x22;     // reset target to active list

    public const int CopperListCount        = 128;
    public const int MaxCopperEntriesPerList = 256;

    // Memory spaces for CmdMemRead/CmdMemWrite
    public const byte MemSpaceScreen   = 0x00;     // 2000 bytes (text character RAM)
    public const byte MemSpaceColor    = 0x01;     // 2000 bytes (color RAM)
    public const byte MemSpaceGfx      = 0x02;     // 320*200 bytes (graphics bitmap)
    public const byte MemSpaceSprite   = 0x03;     // 16*128 bytes (sprite shape RAM)
}

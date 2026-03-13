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

    // EhBASIC zero-page pointers
    public const int ZpSmeml           = 0x79;   // start of program memory (low byte)
    public const int ZpSmemh           = 0x7A;   // start of program memory (high byte)
    public const int ZpSvarl           = 0x7B;   // start of variables / end of program (low byte)
    public const int ZpSvarh           = 0x7C;   // start of variables / end of program (high byte)

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
    public const int FioDirType        = 0xB9AF;   // dir entry type: 0=BAS, 1=SID, 2=BIN, 3=MID
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
    public const byte FioCmdMidPlay    = 0x13;
    public const byte FioCmdMidStop    = 0x14;

    public const byte FioCmdCd         = 0x20;
    public const byte FioCmdMkdir      = 0x21;
    public const byte FioCmdRmdir      = 0x22;
    public const byte FioCmdFormat     = 0x23;
    public const byte FioCmdMount      = 0x24;
    public const byte FioCmdUnmount    = 0x25;
    public const byte FioCmdPwd        = 0x26;

    public const byte FioDirTypeDir    = 0x05;
    public const byte FioErrDiskFull   = 0x04;
    public const byte FioErrNotMounted = 0x05;

    // Music/SFX status register
    public const int  MusicStatus      = 0xBA50; // bit 0 = SFX playing, bit 1 = music playing
    public const int  MusicNote1       = 0xBA51; // voice 1 current MIDI note (0=silent)
    public const int  MusicNote2       = 0xBA52; // voice 2 current MIDI note
    public const int  MusicNote3       = 0xBA53; // voice 3 current MIDI note
    public const int  MusicNote4       = 0xBA54; // voice 4 current MIDI note
    public const int  MusicNote5       = 0xBA55; // voice 5 current MIDI note
    public const int  MusicNote6       = 0xBA56; // voice 6 current MIDI note
    public const int  MusicNote7       = 0xBA57; // voice 7 current MIDI note (WTS 0)
    public const int  MusicNote8       = 0xBA58; // voice 8 current MIDI note (WTS 1)
    public const int  MusicNote9       = 0xBA59; // voice 9 current MIDI note (WTS 2)
    public const int  MusicNote10      = 0xBA5A; // voice 10 current MIDI note (WTS 3)
    public const int  MusicNote11      = 0xBA5B; // voice 11 current MIDI note (WTS 4)
    public const int  MusicNote12      = 0xBA5C; // voice 12 current MIDI note (WTS 5)
    public const int  MusicNote13      = 0xBA5D; // voice 13 current MIDI note (WTS 6)
    public const int  MusicNote14      = 0xBA5E; // voice 14 current MIDI note (WTS 7)
    public const int  MusicElapsedL    = 0xBA5F; // elapsed frames low byte (60Hz)
    public const int  MusicElapsedH    = 0xBA60; // elapsed frames high byte
    public const int  MusicTotalL      = 0xBA61; // total frames low byte
    public const int  MusicTotalH      = 0xBA62; // total frames high byte

    // -------------------------------------------------------------------------
    // Wavetable Synthesizer registers ($A140-$A1DF)
    // -------------------------------------------------------------------------

    public const int WtsBase             = 0xA140;

    // Per-voice registers: 8 voices × 8 bytes ($A140-$A17F)
    public const int WtsVoiceBase        = 0xA140;
    public const int WtsVoiceStride      = 8;
    public const int WtsVoiceEnd         = 0xA17F;

    // Voice register offsets (add to WtsVoiceBase + voice * WtsVoiceStride)
    public const int WtsVoiceNote        = 0;  // W: MIDI note (1-127), write 0 = note off
    public const int WtsVoiceVelocity    = 1;  // W: velocity (0-127)
    public const int WtsVoiceInstrument  = 2;  // W: instrument index (0-255)
    public const int WtsVoiceVolume      = 3;  // R/W: volume (0-255, default 255)
    public const int WtsVoicePanning     = 4;  // R/W: pan (0=left, 128=center, 255=right)
    public const int WtsVoicePitchBendLo = 5;  // W: pitch bend low byte
    public const int WtsVoicePitchBendHi = 6;  // W: pitch bend high byte ($80=center)
    public const int WtsVoiceStatus      = 7;  // R: bit 0=active, bit 1=releasing

    // Global registers ($A180-$A19F)
    public const int WtsGlobalBase       = 0xA180;
    public const int WtsReverbLevel      = 0xA180; // R/W: 0-255 (default 80)
    public const int WtsChorusLevel      = 0xA181; // R/W: 0-255 (default 40)
    public const int WtsMasterVolume     = 0xA182; // R/W: 0-255 (default 255)
    public const int WtsSoundfontStatus  = 0xA183; // R: 0=none, 1=ready, 2=loading, $FF=error
    public const int WtsInstrumentCount  = 0xA184; // R: instrument count in loaded soundfont
    public const int WtsCommand          = 0xA185; // W: command byte
    public const int WtsCommandArg       = 0xA186; // W: command argument
    public const int WtsActiveVoices     = 0xA187; // R: bitmask of active voices

    // Instrument enumeration ($A1A0-$A1DF)
    public const int WtsEnumBase         = 0xA1A0;
    public const int WtsEnumIndex        = 0xA1A0; // W: write index to populate buffer
    public const int WtsEnumBank         = 0xA1A1; // R: MIDI bank
    public const int WtsEnumProgram      = 0xA1A2; // R: MIDI program
    public const int WtsEnumName         = 0xA1A3; // R: null-terminated name (28 chars)
    public const int WtsEnumEnd          = 0xA1DF;

    public const int WtsEnd              = 0xA1DF;

    // WTS commands (write to WtsCommand)
    public const byte WtsCmdAllNotesOff  = 0x01;
    public const byte WtsCmdResetEffects = 0x02;

    // FileIO command for soundfont loading
    public const byte FioCmdSfLoad       = 0x15;

    public const byte FioStatusIdle    = 0x00;
    public const byte FioStatusOk      = 0x02;
    public const byte FioStatusError   = 0x03;

    public const byte FioErrNone       = 0x00;
    public const byte FioErrNotFound   = 0x01;
    public const byte FioErrIo         = 0x02;
    public const byte FioErrEndOfDir   = 0x03;

    public const byte FioDirTypeBin     = 0x02;   // dir entry type: .bin (binary executable)

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

    // -------------------------------------------------------------------------
    // DMA controller registers ($BA63-$BA82)
    // -------------------------------------------------------------------------

    public const int DmaBase           = 0xBA63;
    public const int DmaEnd            = 0xBA82;

    public const int DmaCmd            = 0xBA63;  // write triggers command
    public const int DmaStatus         = 0xBA64;  // 0=idle, 1=busy, 2=ok, 3=error
    public const int DmaErrCode        = 0xBA65;  // error code (DmaErr*)
    public const int DmaSrcSpace       = 0xBA66;  // source space id (DmaSpace*)
    public const int DmaDstSpace       = 0xBA67;  // destination space id (DmaSpace*)
    public const int DmaSrcL           = 0xBA68;  // source address low
    public const int DmaSrcM           = 0xBA69;  // source address mid
    public const int DmaSrcH           = 0xBA6A;  // source address high
    public const int DmaDstL           = 0xBA6B;  // destination address low
    public const int DmaDstM           = 0xBA6C;  // destination address mid
    public const int DmaDstH           = 0xBA6D;  // destination address high
    public const int DmaLenL           = 0xBA6E;  // length low
    public const int DmaLenM           = 0xBA6F;  // length mid
    public const int DmaLenH           = 0xBA70;  // length high
    public const int DmaMode           = 0xBA71;  // bit0=fill mode
    public const int DmaFillValue      = 0xBA72;  // fill byte when mode bit0 set
    public const int DmaCountL         = 0xBA73;  // bytes moved low
    public const int DmaCountM         = 0xBA74;  // bytes moved mid
    public const int DmaCountH         = 0xBA75;  // bytes moved high

    public const byte DmaCmdStart      = 0x01;

    public const byte DmaStatusIdle    = 0x00;
    public const byte DmaStatusBusy    = 0x01;
    public const byte DmaStatusOk      = 0x02;
    public const byte DmaStatusError   = 0x03;

    public const byte DmaErrNone       = 0x00;
    public const byte DmaErrBadCmd     = 0x01;
    public const byte DmaErrBadSpace   = 0x02;
    public const byte DmaErrRange      = 0x03;
    public const byte DmaErrBadArgs    = 0x04;
    public const byte DmaErrWriteProt  = 0x05;

    public const byte DmaModeFill      = 0x01;

    // DMA space ids
    public const byte DmaSpaceCpuRam   = 0x00;
    public const byte DmaSpaceVgcChar  = 0x01;
    public const byte DmaSpaceVgcColor = 0x02;
    public const byte DmaSpaceVgcGfx   = 0x03;
    public const byte DmaSpaceVgcSprite = 0x04;
    public const byte DmaSpaceXram     = 0x05;

    // -------------------------------------------------------------------------
    // Blitter controller registers ($BA83-$BAA2)
    // -------------------------------------------------------------------------

    public const int BltBase           = 0xBA83;
    public const int BltEnd            = 0xBAA2;

    public const int BltCmd            = 0xBA83;  // write triggers command
    public const int BltStatus         = 0xBA84;  // 0=idle, 1=busy, 2=ok, 3=error
    public const int BltErrCode        = 0xBA85;  // error code (BltErr*)
    public const int BltSrcSpace       = 0xBA86;  // source space id (DmaSpace*)
    public const int BltDstSpace       = 0xBA87;  // destination space id (DmaSpace*)
    public const int BltSrcL           = 0xBA88;  // source base address low
    public const int BltSrcM           = 0xBA89;  // source base address mid
    public const int BltSrcH           = 0xBA8A;  // source base address high
    public const int BltDstL           = 0xBA8B;  // destination base address low
    public const int BltDstM           = 0xBA8C;  // destination base address mid
    public const int BltDstH           = 0xBA8D;  // destination base address high
    public const int BltWidthL         = 0xBA8E;  // width low
    public const int BltWidthH         = 0xBA8F;  // width high
    public const int BltHeightL        = 0xBA90;  // height low
    public const int BltHeightH        = 0xBA91;  // height high
    public const int BltSrcStrideL     = 0xBA92;  // source stride low
    public const int BltSrcStrideH     = 0xBA93;  // source stride high
    public const int BltDstStrideL     = 0xBA94;  // destination stride low
    public const int BltDstStrideH     = 0xBA95;  // destination stride high
    public const int BltMode           = 0xBA96;  // bit0=fill, bit1=color-key
    public const int BltFillValue      = 0xBA97;  // fill byte when mode bit0 set
    public const int BltColorKey       = 0xBA98;  // transparent source color when bit1 set
    public const int BltCountL         = 0xBA99;  // bytes written low
    public const int BltCountM         = 0xBA9A;  // bytes written mid
    public const int BltCountH         = 0xBA9B;  // bytes written high

    public const byte BltCmdStart      = 0x01;

    public const byte BltStatusIdle    = 0x00;
    public const byte BltStatusBusy    = 0x01;
    public const byte BltStatusOk      = 0x02;
    public const byte BltStatusError   = 0x03;

    public const byte BltErrNone       = 0x00;
    public const byte BltErrBadCmd     = 0x01;
    public const byte BltErrBadSpace   = 0x02;
    public const byte BltErrRange      = 0x03;
    public const byte BltErrBadArgs    = 0x04;
    public const byte BltErrWriteProt  = 0x05;

    public const byte BltModeFill      = 0x01;
    public const byte BltModeColorKey  = 0x02;

    public const int FreeBase          = 0xBAA3;
    public const int FreeEnd           = 0xBFFF;

    // -------------------------------------------------------------------------
    // File metadata buffer ($BAB0-$BB1F) — populated during filtered DIRREAD
    // -------------------------------------------------------------------------

    public const int MetaBase       = 0xBAB0;
    public const int MetaEnd        = 0xBB1F;
    public const int MetaSize       = MetaEnd - MetaBase + 1;  // 112 bytes

    public const int MetaType       = 0xBAB0;   // 1 byte: file type (NdiFileType enum value)
    public const int MetaSizeL      = 0xBAB1;   // file size low byte
    public const int MetaSizeH      = 0xBAB2;   // file size high byte
    public const int MetaTitle      = 0xBAB3;   // 32 bytes, null-padded ASCII
    public const int MetaAuthor     = 0xBAD3;   // 32 bytes, null-padded ASCII
    public const int MetaCopyright  = 0xBAF3;   // 32 bytes, null-padded ASCII
    public const int MetaLoadL      = 0xBB13;   // load address low (BIN/SID)
    public const int MetaLoadH      = 0xBB14;   // load address high
    public const int MetaInitL      = 0xBB15;   // init address low (SID)
    public const int MetaInitH      = 0xBB16;   // init address high
    public const int MetaPlayL      = 0xBB17;   // play address low (SID)
    public const int MetaPlayH      = 0xBB18;   // play address high
    public const int MetaSongs      = 0xBB19;   // song/track count (SID subtunes / MID tracks)
    public const int MetaDurL       = 0xBB1A;   // duration seconds low (MID only)
    public const int MetaDurH       = 0xBB1B;   // duration seconds high
    public const int MetaGfxSpace   = 0xBB1C;   // GFX space type

    public const int MetaTitleLen   = 32;
    public const int MetaAuthorLen  = 32;
    public const int MetaCopyrightLen = 32;

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

    public const int AutobootSkip      = 0xB9F0;
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

    // VM timing defaults
    public const int DefaultCpuHz = 12_000_000;
    public const int FrameRateHz = 60;
    public const int TimerTickQuantumCycles = 100;
    public const int DmaBytesPerCycle = 4;
    public const int BltOpsPerCycle = 2;

    // Sprite priority values
    public const int SpritePriBehindAll = 0;
    public const int SpritePriBetween   = 1;
    public const int SpritePriInFront   = 2;

    // Sprite register block: $A040-$A0BF (8 bytes per sprite, 16 sprites)
    public const int SpriteRegBase    = 0xA040;
    public const int SpriteRegEnd     = 0xA0BF;
    public const int SpriteRegStride  = 8;

    // Per-sprite register offsets within each 8-byte block
    public const int SprRegXLo        = 0;
    public const int SprRegXHi        = 1;
    public const int SprRegYLo        = 2;
    public const int SprRegYHi        = 3;
    public const int SprRegShape      = 4;
    public const int SprRegFlags      = 5;  // bit0=xFlip, bit1=yFlip, bit7=enable
    public const int SprRegPriority   = 6;
    public const int SprRegTransColor = 7;

    // Sprite flags bitmasks
    public const byte SprFlagXFlip    = 0x01;
    public const byte SprFlagYFlip    = 0x02;
    public const byte SprFlagFlipMask = 0x03;
    public const byte SprFlagEnable   = 0x80;

    // Expanded shape slot storage
    public const int ShapeSlotCount   = 256;
    public const int ShapeRamSize     = ShapeSlotCount * SpriteShapeSize;  // 32768

    // -------------------------------------------------------------------------
    // Tile engine registers ($A0C0-$A0DF)
    // -------------------------------------------------------------------------

    public const int TileRegBase       = 0xA0C0;
    public const int TileRegEnd        = 0xA0DF;

    // Configuration registers
    public const int TileConfig        = 0xA0C0;   // bit0: size (0=8x8, 1=16x16), bits 2-1: mirror mode (0-3)
    public const int TileTransColor    = 0xA0C1;   // global transparent color index (0-15), default 0
    public const int TileScrollXL      = 0xA0C2;   // scroll X low byte
    public const int TileScrollXH      = 0xA0C3;   // scroll X high byte
    public const int TileScrollYL      = 0xA0C4;   // scroll Y low byte
    public const int TileScrollYH      = 0xA0C5;   // scroll Y high byte
    public const int TileStatus        = 0xA0C6;   // R: last command status (0=ok)
    public const int TileCmd           = 0xA0C7;   // W: write triggers tile command

    // Command parameters
    public const int TileP0            = 0xA0C8;
    public const int TileP1            = 0xA0C9;
    public const int TileP2            = 0xA0CA;
    public const int TileP3            = 0xA0CB;
    public const int TileAddrL         = 0xA0CC;   // source/dest CPU RAM address low
    public const int TileAddrH         = 0xA0CD;   // source/dest CPU RAM address high
    public const int TilePalP0         = 0xA0CE;   // palette parameter 0 (sub-palette index)
    public const int TilePalP1         = 0xA0CF;   // palette parameter 1 (color index)

    // Collision registers
    public const int TileColL          = 0xA0D0;   // R: sprite-tile collision bits 0-7 (read clears)
    public const int TileColH          = 0xA0D1;   // R: sprite-tile collision bits 8-15 (read clears)

    // Peek registers
    public const int TilePeekVal       = 0xA0D2;   // R: tile index at last peeked position
    public const int TilePeekAttr      = 0xA0D3;   // R: attribute at last peeked position

    // Tile commands (written to TileCmd)
    public const byte TileCmdDef       = 0x01;     // define one tile: P0=tile#, TileAddr=src
    public const byte TileCmdDefBulk   = 0x02;     // define multiple: P0=start, P1=count, TileAddr=src
    public const byte TileCmdPut       = 0x03;     // set NT entry: P0=NT, P1=X, P2=Y, P3=tile#
    public const byte TileCmdAttr      = 0x04;     // set attribute: P0=NT, P1=X, P2=Y, P3=attr
    public const byte TileCmdFill      = 0x05;     // fill NT: P0=NT, P1=tile#
    public const byte TileCmdRow       = 0x06;     // write row: P0=NT, P1=Y, TileAddr=src
    public const byte TileCmdCol       = 0x07;     // write column: P0=NT, P1=X, TileAddr=src
    public const byte TileCmdLoad      = 0x08;     // load NT: P0=NT, TileAddr=src (1000 bytes)
    public const byte TileCmdALoad     = 0x09;     // load attr table: P0=NT, TileAddr=src (1000 bytes)
    public const byte TileCmdPal       = 0x0A;     // load sub-palette: PalP0=index, TileAddr=src (48 bytes)
    public const byte TileCmdPalC      = 0x0B;     // set one color: PalP0=sub, PalP1=color, P0=R, P1=G, P2=B
    public const byte TileCmdPeek      = 0x0C;     // peek: P0=NT, P1=X, P2=Y → TilePeekVal/Attr
    public const byte TileCmdRowAttr   = 0x0D;     // write attr row: P0=NT, P1=Y, TileAddr=src
    public const byte TileCmdColAttr   = 0x0E;     // write attr col: P0=NT, P1=X, TileAddr=src
    public const byte TileCmdCls       = 0x0F;     // clear all NTs and attrs to 0
    public const byte TileCmdBufFill   = 0x10;     // fill column buffer: P0=tile#
    public const byte TileCmdBufSet    = 0x11;     // set buffer entry: P0=row, P1=tile#
    public const byte TileCmdBufRange  = 0x12;     // fill buffer range: P0=y1, P1=y2, P2=tile#
    public const byte TileCmdBufPut    = 0x13;     // write buffer to NT: P0=NT, P1=col

    // Tile config bit masks
    public const byte TileCfgSize16    = 0x01;     // bit 0: 0=8x8, 1=16x16
    public const byte TileCfgMirrorMask = 0x06;    // bits 2-1: mirror mode
    public const int  TileCfgMirrorShift = 1;

    // Mirror modes
    public const byte TileMirrorH      = 0;        // horizontal mirror (NT0=NT1, NT2=NT3)
    public const byte TileMirrorV      = 1;        // vertical mirror (NT0=NT2, NT1=NT3)
    public const byte TileMirrorFour   = 2;        // four-screen (all independent)
    public const byte TileMirrorSingle = 3;        // single-screen (all = NT0)

    // Tile attribute bit masks
    public const byte TileAttrVFlip    = 0x80;
    public const byte TileAttrHFlip    = 0x40;
    public const byte TileAttrPriority = 0x20;     // 0=behind between-sprites, 1=in front
    public const byte TileAttrPalMask  = 0x0F;     // sub-palette index (0-15)

    // Tile dimensions and counts
    public const int TileCount         = 256;
    public const int NametableCount    = 4;
    public const int NametableCols     = 40;        // tiles per row (320/8)
    public const int NametableRows     = 25;        // tiles per column (200/8)
    public const int NametableSize     = NametableCols * NametableRows; // 1000
    public const int TileSize8         = 32;        // 8x8 × 4-bit = 32 bytes
    public const int TileSize16        = 128;       // 16x16 × 4-bit = 128 bytes
    public const int TileRamSize8      = TileCount * TileSize8;   // 8192
    public const int TileRamSize16     = TileCount * TileSize16;  // 32768
    public const int TilePaletteCount  = 16;        // sub-palettes
    public const int TilePaletteColors = 16;        // colors per sub-palette
    public const int TilePaletteRamSize = TilePaletteCount * TilePaletteColors * 3; // 768 bytes RGB

    // File I/O commands for tiles
    public const byte FioCmdTSave      = 0x16;
    public const byte FioCmdTLoad      = 0x17;

    // DMA space for tile data
    public const byte DmaSpaceVgcTile  = 0x06;

    // -------------------------------------------------------------------------
    // VGC core registers ($A000-$A00F)
    // -------------------------------------------------------------------------

    public const int RegMode           = 0xA000;   // 0=text only, 1=gfx over text, 2=text over gfx, 3=gfx+sprites no text, 4=tiles+sprites
    public const int RegBgCol          = 0xA001;
    public const int RegFgCol          = 0xA002;
    public const int RegCursorX        = 0xA003;   // 0-79
    public const int RegCursorY        = 0xA004;   // 0-24
    public const int RegScrollX        = 0xA005;
    public const int RegScrollY        = 0xA006;
    public const int RegFont           = 0xA007;   // active font slot (0-7)
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
    public const byte CmdGtext         = 0x0A;

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
    public const byte MemSpaceTile     = 0x04;     // tile definition data

    // -------------------------------------------------------------------------
    // Help system registers ($A020-$A030)
    // -------------------------------------------------------------------------

    public const ushort RegHelp           = 0xA020;
    public const ushort HelpSearchBuffer  = 0xA021;
    public const int    HelpSearchBufferLen = 16;

    // -------------------------------------------------------------------------
    // ROM swap register ($A03F)
    // -------------------------------------------------------------------------

    public const int RegRomSwap       = 0xA03F;
    public const byte RomSwapNcc      = 0x01;   // switch to NCC ROM
    public const byte RomSwapBasic    = 0x02;   // switch to BASIC ROM
    public const byte RomSwapNccEdit  = 0x03;   // activate NCC editor (from BASIC NCC command)
    public const byte RomSwapExtension = 0x04;  // switch to extension ROM

    // -------------------------------------------------------------------------
    // Compiler Controller registers ($A031-$A03E)
    // -------------------------------------------------------------------------

    public const int CmpBase          = 0xA031;
    public const int CmpEnd           = 0xA03E;

    public const int CmpCmd           = 0xA031;  // write triggers command
    public const int CmpStatus        = 0xA032;  // 0=idle, 1=compiling, 2=ok, 3=error
    public const int CmpErrCount      = 0xA033;  // number of errors
    public const int CmpWarnCount     = 0xA034;  // number of warnings
    public const int CmpErrLineL      = 0xA035;  // current error line low
    public const int CmpErrLineH      = 0xA036;  // current error line high
    public const int CmpCodeSizeL     = 0xA037;  // compiled code size low
    public const int CmpCodeSizeH     = 0xA038;  // compiled code size high
    public const int CmpErrMsg        = 0xA039;  // read error message byte-by-byte
    public const int CmpErrIdx        = 0xA03A;  // current error index (write to select)
    public const int CmpSrcAddrL      = 0xA03B;  // source XRAM address low
    public const int CmpSrcAddrM      = 0xA03C;  // source XRAM address mid
    public const int CmpSrcAddrH      = 0xA03D;  // source XRAM address high

    public const byte CmpCmdCompile   = 0x01;   // compile source from XRAM
    public const byte CmpCmdGetError  = 0x02;   // populate error fields for CmpErrIdx
    public const byte CmpCmdGetWarn   = 0x03;   // populate warning fields for CmpErrIdx

    public const byte CmpStatusIdle      = 0x00;
    public const byte CmpStatusCompiling = 0x01;
    public const byte CmpStatusOk        = 0x02;
    public const byte CmpStatusError     = 0x03;
}

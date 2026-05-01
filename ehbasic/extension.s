; Extension ROM for NovaVM (bank 4)
; Contains overflow command handlers called from BASIC ROM via RAM trampoline.
;
; Entry point at $C000: reads ExtCmdId from zero page and dispatches.
; Handlers access hardware registers directly — never call BASIC ROM routines.

; --- Zero page communication ---
ExtCmdId        = $E4           ; command ID (set by RAM trampoline)

; --- Nova zero-page pseudo-registers ($20-$2F) ---
NVR0L           = $20
NVR0H           = $21
NVR1L           = $22
NVR1H           = $23
NVR2L           = $24
NVR2H           = $25
NVR3L           = $26
NVR3H           = $27
NVR4L           = $28
NVR4H           = $29
NVR5L           = $2A
NVR5H           = $2B
NVR6L           = $2C
NVR6H           = $2D
NVR7L           = $2E
NVR7H           = $2F

; Short-lived extension helper scratch. Keep NVR4-NVR6 for XMC metadata state.
ext_ptrL        = NVR0L
ext_ptrH        = NVR0H
ext_u16h        = NVR0L
ext_u16l        = NVR0H
ext_dig0        = NVR1L
ext_dig1        = NVR1H
ext_dig2        = NVR2L
ext_dig3        = NVR2H
ext_dig4        = NVR3L
ext_firstdig    = NVR3H

; --- Hardware registers ---
VGC_CHAROUT     = $A00E         ; write character to screen
VGC_CHARIN      = $A00F         ; read character (0 = none)
REG_ROMSWAP     = $A03F         ; ROM bank switch register
FIO_CMD         = $B9A0         ; FileIO command register
FIO_STATUS      = $B9A1         ; FileIO status (0=idle, 2=ok, 3=error)
FIO_ERRCODE     = $B9A2         ; FileIO error code

; --- FileIO registers ---
FIO_NAMELEN     = $B9A3         ; filename length
FIO_SIZEL       = $B9A8         ; dir entry size low
FIO_SIZEH       = $B9A9         ; dir entry size high
FIO_DIRTYPE     = $B9AF         ; dir entry type (0=BAS,1=SID,2=BIN,3=MID,4=GFX,5=DIR)
FIO_NAME        = $B9B0         ; filename buffer (64 bytes)

; --- XMC registers ---
XMC_CMD         = $BA00         ; command register
XMC_STATUS      = $BA01         ; status
XMC_ERRCODE     = $BA02         ; error code
XMC_BANK        = $BA0C         ; current bank
XMC_BANKS       = $BA0D         ; total banks
XMC_USEDL       = $BA0E         ; used pages low
XMC_USEDH       = $BA0F         ; used pages high
XMC_FREEL       = $BA10         ; free pages low
XMC_FREEH       = $BA11         ; free pages high
XMC_NAMELEN     = $BA12         ; name length
XMC_DIRCOUNTL   = $BA14         ; named block count low
XMC_DIRCOUNTH   = $BA15         ; named block count high
XMC_LENL        = $BA09         ; block length low
XMC_LENH        = $BA0A         ; block length high
XMC_NAME        = $BA24         ; name buffer
XMC_CMD_STATS   = $07           ; refresh stats
XMC_CMD_NDIRO   = $0E           ; named dir open
XMC_CMD_NDIRR   = $0F           ; named dir read
XMC_OK          = $02           ; success status

; --- Constants ---
FIO_CMD_SFLOAD  = $15           ; soundfont load command
FIO_CMD_DIROPEN = $03           ; open directory listing
FIO_CMD_DIRREAD = $04           ; read next directory entry
FIO_CMD_PWD     = $26           ; print working directory
FIO_CMD_TSAVE   = $16           ; save tile state
FIO_CMD_TLOAD   = $17           ; load tile state
FIO_OK          = $02           ; status: success
ROMSWAP_NCCEDIT = $03           ; NCC editor event

; --- Extension command IDs ---
EXT_CMD_NCC     = $00
EXT_CMD_SFLOAD  = $01
EXT_CMD_DIR     = $02
EXT_CMD_PWD     = $03
EXT_CMD_XMEM    = $04
EXT_CMD_XDIR    = $05
EXT_CMD_TSAVE   = $06
EXT_CMD_TLOAD   = $07
EXT_CMD_HELP    = $08
EXT_CMD_DMAFILL = $09
EXT_CMD_BLTFILL = $0A
EXT_CMD_XMCCMD  = $0B
EXT_CMD_COPPER  = $0C           ; COPPER subcommand parser
EXT_CMD_XLOAD   = $0D
EXT_CMD_XSAVE   = $0E

; --- XMC command codes (written to $BA00 before calling processor) ---
XMC_CMD_GET     = $01           ; GetByte: read XRAM[XA] → DATA
XMC_CMD_PUT     = $02           ; PutByte: write DATA → XRAM[XA]
XMC_CMD_STASH   = $03           ; Stash: RAM → XRAM via DMA
XMC_CMD_FETCH   = $04           ; Fetch: XRAM → RAM via DMA
XMC_CMD_FILL    = $05           ; Fill: fill XRAM with DATA
XMC_CMD_RSTUS   = $08           ; ResetUsage: clear page tracking
XMC_CMD_REL     = $09           ; Release: mark pages free
XMC_CMD_ALLOC   = $0A           ; Alloc: find free pages
XMC_CMD_NSTSH   = $0B           ; Named stash
XMC_CMD_NFETC   = $0C           ; Named fetch
XMC_CMD_NDEL    = $0D           ; Named delete
; XMC_CMD_NDIRO ($0E) and XMC_CMD_NDIRR ($0F) already defined above

; --- Additional XMC register addresses ---
XMC_XAL         = $BA04         ; XRAM address low
XMC_XAM         = $BA05         ; XRAM address mid
XMC_XAH         = $BA06         ; XRAM address high
XMC_RAML        = $BA07         ; CPU RAM address low
XMC_RAMH        = $BA08         ; CPU RAM address high
XMC_DATA        = $BA0B         ; data port byte
XMC_HANDLE      = $BA13         ; block handle

; --- Blitter register addresses ---
BLT_DSTSPACE    = $BA87
BLT_DSTL        = $BA8B
BLT_DSTM        = $BA8C
BLT_DSTH        = $BA8D
BLT_WIDTHL      = $BA8E
BLT_WIDTHH      = $BA8F
BLT_HEIGHTL     = $BA90
BLT_HEIGHTH     = $BA91
BLT_DSTSTRL     = $BA94
BLT_DSTRH       = $BA95
BLT_FILLVAL     = $BA97

; --- Window 3 registers (used for single-byte XRAM access) ---
WIN3_LO         = $BA21         ; window 3 base address low (ignored)
WIN3_MI         = $BA22         ; window 3 base address mid
WIN3_HI         = $BA23         ; window 3 base address high
WIN3_BASE       = $BF00         ; window 3 data area ($BF00-$BFFF)

; --- RAM addresses ---
EXT_RESET_VEC   = $0233         ; reset recovery routine in RAM
EXT_GTBY_VEC    = $0236         ; bridge: extension → BASIC LAB_GTBY → extension
EXT_GTWRD_VEC   = $0244         ; bridge: extension → BASIC LAB_GTWRD → extension
EXT_SNERR_VEC   = $0252         ; bridge: extension → BASIC LAB_15D9 syntax err

; --- BASIC ZP routines (RAM-resident, callable from extension ROM) ---
LAB_IGBY        = $BC           ; advance + skip-spaces + return byte (A)
LAB_GBYT        = $C2           ; peek current + skip-spaces + return byte (A)

; --- BASIC ZP scratch / FAC1 ---
Bpntrl          = $C3           ; BASIC execute pointer low (alias of Bpntrl_ext)
Bpntrh          = $C4
FAC1_2          = $AE           ; FAC1 mantissa2 (16-bit hi after LAB_GTWRD)
FAC1_3          = $AF           ; FAC1 mantissa3 (16-bit lo after LAB_GTWRD)

; --- BASIC token bytes (sequential from $80, see basic.asm:321) ---
TK_END_VAL      = $80
TK_ON_VAL       = $93
TK_LIST_VAL     = $A1
TK_CLEAR_VAL    = $A2
TK_GMODE_VAL    = $B5
TK_OFF_VAL      = $CB

; --- VGC command + parameter registers ---
VGC_CMD         = $A010
VGC_P0          = $A011
VGC_P1          = $A012
VGC_P2          = $A013
VGC_P3          = $A014
VGC_P4          = $A015
VGC_P5          = $A016

; --- VGC copper command codes (from basic.asm:8856-8863) ---
VCMD_COPPERADD  = $1B
VCMD_COPPERCLR  = $1C
VCMD_COPPERENA  = $1D
VCMD_COPPERDIS  = $1E
VCMD_COPPERLIST = $20
VCMD_COPPERUSE  = $21
VCMD_COPPEREND  = $22

      .segment "CODE"
      .org    $C000

; =====================================================================
; Extension entry point — called via JSR $C000 from RAM trampoline
; =====================================================================
ExtEntry:
      LDA   ExtCmdId          ; read command ID
      ASL                     ; * 2 for word-sized table
      TAX
      LDA   ExtTable+1,X      ; high byte of handler-1
      PHA
      LDA   ExtTable,X        ; low byte of handler-1
      PHA
      RTS                     ; dispatch via RTS trick

; --- Dispatch table (entries are handler_address - 1) ---
ExtTable:
      .word EXT_NCC-1         ; cmd 0: NCC confirmation dialog
      .word EXT_SFLOAD-1      ; cmd 1: SFLOAD soundfont
      .word EXT_DIR-1         ; cmd 2: DIR listing loop
      .word EXT_PWD-1         ; cmd 3: PWD print working directory
      .word EXT_XMEM-1        ; cmd 4: XMEM status display
      .word EXT_XDIR-1        ; cmd 5: XDIR listing loop
      .word EXT_TSAVE-1       ; cmd 6: TSAVE tile file
      .word EXT_TLOAD-1       ; cmd 7: TLOAD tile file
      .word EXT_HELP-1        ; cmd 8: HELP command
      .word EXT_DMAFILL-1     ; cmd 9: DMAFILL epilogue
      .word EXT_BLTFILL-1     ; cmd A: BLITFILL epilogue
      .word EXT_XMCCMD-1      ; cmd B: XMC command processor
      .word EXT_COPPER-1      ; cmd C: COPPER subcommand parser
      .word EXT_XLOAD-1       ; cmd D: XLOAD file -> XRAM
      .word EXT_XSAVE-1       ; cmd E: XSAVE XRAM -> file

; =====================================================================
; SFLOAD handler — issue FIO_CMD_SFLOAD and return status
; Filename already in FIO_NAME/FIO_NAMELEN (set by BASIC ROM stub).
; Returns: A=0 success, A=errcode on error.
; =====================================================================
EXT_SFLOAD:
      LDA   #FIO_CMD_SFLOAD
      STA   FIO_CMD           ; trigger soundfont load
      LDA   FIO_STATUS
      CMP   #FIO_OK
      BEQ   @ok
      LDA   FIO_ERRCODE       ; nonzero error code
      RTS
@ok:
      LDA   #$00
      RTS

; =====================================================================
; NCC handler — confirmation dialog then trigger NCC editor
; =====================================================================
EXT_NCC:
      LDY   #$00
@msg1:
      LDA   @ncc_msg1,Y
      BEQ   @msg1_done
      STA   VGC_CHAROUT
      INY
      BNE   @msg1
@msg1_done:
      LDA   #$0D
      STA   VGC_CHAROUT
      LDA   #$0A
      STA   VGC_CHAROUT

      LDY   #$00
@msg2:
      LDA   @ncc_msg2,Y
      BEQ   @msg2_done
      STA   VGC_CHAROUT
      INY
      BNE   @msg2
@msg2_done:

@wait_key:
      LDA   VGC_CHARIN
      BEQ   @wait_key
      CMP   #'Y'
      BEQ   @do_ncc
      CMP   #'y'
      BEQ   @do_ncc

      ; Not Y — echo char + CRLF, return
      STA   VGC_CHAROUT
      LDA   #$0D
      STA   VGC_CHAROUT
      LDA   #$0A
      STA   VGC_CHAROUT
      LDA   #$00
      RTS

@do_ncc:
      STA   VGC_CHAROUT       ; echo Y
      LDA   #$0D
      STA   VGC_CHAROUT
      LDA   #$0A
      STA   VGC_CHAROUT
      LDA   #ROMSWAP_NCCEDIT
      STA   REG_ROMSWAP
      LDA   #$00
      RTS

@ncc_msg1: .byte "THIS WILL CLEAR THE BASIC PROGRAM",0
@ncc_msg2: .byte "ARE YOU SURE? (Y/N) ",0

; =====================================================================
; DIR handler — list directory entries
; FIO_NAMELEN already set (0 = no filter, >0 = pattern in FIO_NAME).
; =====================================================================
EXT_DIR:
      LDA   #FIO_CMD_DIROPEN
      STA   FIO_CMD           ; trigger DirOpen
      LDA   FIO_STATUS
      CMP   #FIO_OK
      BNE   @dir_done         ; no files or error
      JSR   ext_crlf          ; blank line
@dir_loop:
      ; --- right-justified file size (5 columns) ---
      LDA   FIO_SIZEH
      LDX   FIO_SIZEL
      JSR   ext_print_u16     ; print AX as decimal, right-justified in 5 cols
      ; --- file type ---
      LDA   FIO_DIRTYPE
      ASL
      TAX
      LDA   @tab_dtype,X
      STA   ext_ptrL
      LDA   @tab_dtype+1,X
      STA   ext_ptrH
      LDY   #$00
@dir_tloop:
      LDA   (ext_ptrL),Y
      BEQ   @dir_tname
      STA   VGC_CHAROUT
      INY
      BNE   @dir_tloop
@dir_tname:
      ; --- filename ---
      LDY   #$00
@dir_pname:
      CPY   FIO_NAMELEN
      BCS   @dir_nl
      LDA   FIO_NAME,Y
      STA   VGC_CHAROUT
      INY
      BNE   @dir_pname
@dir_nl:
      JSR   ext_crlf
      ; advance to next entry
      LDA   #FIO_CMD_DIRREAD
      STA   FIO_CMD
      LDA   FIO_STATUS
      CMP   #FIO_OK
      BEQ   @dir_loop
@dir_done:
      LDA   #$00
      RTS

@tab_dtype:
      .word @s_bas, @s_sid, @s_bin, @s_mid, @s_gfx, @s_dir
@s_bas: .byte "  BAS  ",0
@s_sid: .byte "  SID  ",0
@s_bin: .byte "  BIN  ",0
@s_mid: .byte "  MID  ",0
@s_gfx: .byte "  GFX  ",0
@s_dir: .byte "  DIR  ",0

; =====================================================================
; ext_print_u16 — print 16-bit unsigned in A(high) X(low),
;                 right-justified in a 5-character field.
; =====================================================================
ext_print_u16:
      STA   ext_u16h
      STX   ext_u16l
      ; Extract 5 decimal digits into ext_dig0(10000s)..ext_dig4(1s)
      LDX   #$00
@d10k:
      SEC
      LDA   ext_u16l
      SBC   #<10000
      PHA
      LDA   ext_u16h
      SBC   #>10000
      BCC   @d10kd
      STA   ext_u16h
      PLA
      STA   ext_u16l
      INX
      BCS   @d10k             ; always branches (just subtracted successfully)
@d10kd:
      PLA
      STX   ext_dig0          ; 10000s digit
      LDX   #$00
@d1k:
      SEC
      LDA   ext_u16l
      SBC   #<1000
      PHA
      LDA   ext_u16h
      SBC   #>1000
      BCC   @d1kd
      STA   ext_u16h
      PLA
      STA   ext_u16l
      INX
      BCS   @d1k
@d1kd:
      PLA
      STX   ext_dig1          ; 1000s digit
      LDX   #$00
@d100:
      SEC
      LDA   ext_u16l
      SBC   #100
      BCC   @d100d
      STA   ext_u16l
      INX
      BNE   @d100             ; always branches (100s digit max 9)
@d100d:
      STX   ext_dig2          ; 100s digit
      LDX   #$00
@d10:
      LDA   ext_u16l
      CMP   #10
      BCC   @d10d
      SBC   #10
      STA   ext_u16l
      INX
      BNE   @d10              ; always branches
@d10d:
      STX   ext_dig3          ; 10s digit
      LDA   ext_u16l
      STA   ext_dig4          ; 1s digit
      ; Find first non-zero digit (index in X), always print at least ones
      LDX   #$00
@findnz:
      CPX   #$04
      BCS   @print            ; always print last digit
      LDA   ext_dig0,X
      BNE   @print
      INX
      BNE   @findnz           ; always branches
@print:
      ; Print X leading spaces
      STX   ext_firstdig      ; first significant digit index
      LDY   #$00
      CPX   #$00
      BEQ   @digits
@spc:
      LDA   #' '
      STA   VGC_CHAROUT
      INY
      CPY   ext_firstdig
      BCC   @spc
@digits:
      LDX   ext_firstdig
@dloop:
      LDA   ext_dig0,X
      ORA   #'0'
      STA   VGC_CHAROUT
      INX
      CPX   #$05
      BCC   @dloop
      RTS

; =====================================================================
; PWD handler — print working directory
; =====================================================================
EXT_PWD:
      LDA   #FIO_CMD_PWD
      STA   FIO_CMD
      LDA   FIO_STATUS
      CMP   #FIO_OK
      BNE   @pwd_done
      LDY   #$00
@pwd_lp:
      CPY   FIO_NAMELEN
      BCS   @pwd_nl
      LDA   FIO_NAME,Y
      STA   VGC_CHAROUT
      INY
      BNE   @pwd_lp
@pwd_nl:
      JSR   ext_crlf
@pwd_done:
      LDA   #$00
      RTS

; =====================================================================
; XMEM handler — print expansion memory status
; =====================================================================
EXT_XMEM:
      LDA   #XMC_CMD_STATS
      STA   XMC_CMD
      JSR   xmc_process         ; process locally (FPGA has no hardware XMC)
      ; print "<banks>"
      LDA   #$00
      LDX   XMC_BANKS
      JSR   ext_print_u16
      JSR   ext_print_str
      .byte " BANKS, ",0
      ; print "<banks*64>" as KB
      LDA   XMC_BANKS
      STA   ext_u16l
      LDA   #$00
      STA   ext_u16h
      LDY   #$06
@xm_kb:
      ASL   ext_u16l
      ROL   ext_u16h
      DEY
      BNE   @xm_kb
      LDA   ext_u16h
      LDX   ext_u16l
      JSR   ext_print_u16
      JSR   ext_print_str
      .byte " KB XRAM, BANK ",0
      ; print selected bank
      LDA   #$00
      LDX   XMC_BANK
      JSR   ext_print_u16
      JSR   ext_print_str
      .byte ", USED ",0
      LDA   XMC_USEDH
      LDX   XMC_USEDL
      JSR   ext_print_u16
      JSR   ext_print_str
      .byte ", FREE ",0
      LDA   XMC_FREEH
      LDX   XMC_FREEL
      JSR   ext_print_u16
      JSR   ext_print_str
      .byte " PAGES, NAMED ",0
      LDA   XMC_DIRCOUNTH
      LDX   XMC_DIRCOUNTL
      JSR   ext_print_u16
      JSR   ext_crlf
      LDA   #$00
      RTS

; =====================================================================
; XDIR handler — list named XRAM blocks
; =====================================================================
EXT_XDIR:
      LDA   #XMC_CMD_NDIRO
      STA   XMC_CMD
      JSR   xmc_process
      LDA   XMC_STATUS
      CMP   #XMC_OK
      BNE   @xdir_done
@xdir_loop:
      LDA   XMC_LENH
      LDX   XMC_LENL
      JSR   ext_print_u16
      JSR   ext_print_str
      .byte " BYTES  ",0
      LDY   #$00
@xdir_name:
      CPY   XMC_NAMELEN
      BCS   @xdir_nl
      LDA   XMC_NAME,Y
      STA   VGC_CHAROUT
      INY
      BNE   @xdir_name
@xdir_nl:
      JSR   ext_crlf
      LDA   #XMC_CMD_NDIRR
      STA   XMC_CMD
      JSR   xmc_process
      LDA   XMC_STATUS
      CMP   #XMC_OK
      BEQ   @xdir_loop
@xdir_done:
      LDA   #$00
      RTS

; =====================================================================
; TSAVE handler — save tile state to .tile file
; Filename already in FIO_NAME/FIO_NAMELEN.
; Returns: A=0 success, A=errcode on error (Z flag set accordingly).
; =====================================================================
EXT_TSAVE:
      LDA   #FIO_CMD_TSAVE
      STA   FIO_CMD
      LDA   FIO_STATUS
      CMP   #FIO_OK
      BEQ   @tsave_ok
      LDA   FIO_ERRCODE
      RTS
@tsave_ok:
      LDA   #$00
      RTS

; =====================================================================
; TLOAD handler — load tile state from .tile file
; Filename already in FIO_NAME/FIO_NAMELEN.
; Returns: A=0 success, A=errcode on error (Z flag set accordingly).
; =====================================================================
EXT_TLOAD:
      LDA   #FIO_CMD_TLOAD
      STA   FIO_CMD
      LDA   FIO_STATUS
      CMP   #FIO_OK
      BEQ   @tload_ok
      LDA   FIO_ERRCODE
      RTS
@tload_ok:
      LDA   #$00
      RTS

; =====================================================================
; HELP handler — moved from BASIC ROM to save space.
; Reads BASIC text at (Bpntrl),Y and copies raw bytes to help buffer.
; The C# help system handles detokenization.
; =====================================================================
Bpntrl_ext      = $C3           ; BASIC execute pointer low
Bpntrh_ext      = $C4           ; BASIC execute pointer high
help_len_ext    = $E2           ; help keyword length scratch
REG_HELP        = $A020         ; help command register
HELP_BUF        = $A021         ; help buffer (16 bytes)
TKX_PREFIX_EXT  = $01           ; extended token prefix

EXT_HELP:
      LDY   #$00
      LDA   (Bpntrl_ext),Y
      BEQ   @help_noarg
      CMP   #':'
      BEQ   @help_noarg
      CMP   #'"'
      BEQ   @help_quoted
      LDX   #$00
@help_raw:
      LDA   (Bpntrl_ext),Y
      BEQ   @help_rawdone
      CMP   #':'
      BEQ   @help_rawdone
      STA   HELP_BUF,X
      INY
      INX
      CPX   #$10
      BCC   @help_raw
@help_rawdone:
      TYA
      CLC
      ADC   Bpntrl_ext
      STA   Bpntrl_ext
      BCC   @help_noinc
      INC   Bpntrh_ext
@help_noinc:
      STX   help_len_ext
      JMP   @help_trigger
@help_quoted:
      INY
      LDX   #$00
@help_qloop:
      LDA   (Bpntrl_ext),Y
      BEQ   @help_qdone
      CMP   #'"'
      BEQ   @help_qclose
      STA   HELP_BUF,X
      INY
      INX
      CPX   #$10
      BCC   @help_qloop
@help_qclose:
      INY
@help_qdone:
      TYA
      CLC
      ADC   Bpntrl_ext
      STA   Bpntrl_ext
      BCC   @help_noinc2
      INC   Bpntrh_ext
@help_noinc2:
      STX   help_len_ext
@help_trigger:
      LDY   help_len_ext
@help_zero:
      CPY   #$10
      BCS   @help_search
      LDA   #$00
      STA   HELP_BUF,Y
      INY
      BNE   @help_zero
@help_search:
      LDA   #$02
      STA   REG_HELP
      LDA   #$00
      RTS
@help_noarg:
      LDA   #$01
      STA   REG_HELP
      LDA   #$00
      RTS

; =====================================================================
; DMAFILL epilogue — zero source fields, set fill mode, start, check
; Returns: A=0 success, A=1 error
; =====================================================================
DMA_CMD_REG   = $BA63
DMA_STATUS_REG = $BA64
DMA_SRCSPACE  = $BA66
DMA_SRCL      = $BA68
DMA_SRCM      = $BA69
DMA_SRCH      = $BA6A
DMA_MODE_REG  = $BA71
DMA_CMD_START = $01
DMA_MODE_FILL = $01
HW_BUSY       = $01
HW_OK         = $02

EXT_DMAFILL:
      LDA   #DMA_MODE_FILL
      STA   DMA_MODE_REG
      LDA   #$00
      STA   DMA_SRCSPACE
      STA   DMA_SRCL
      STA   DMA_SRCM
      STA   DMA_SRCH
      LDA   #DMA_CMD_START
      STA   DMA_CMD_REG
      LDA   DMA_STATUS_REG
      JMP   ext_hw_chk

; =====================================================================
; BLITFILL epilogue — zero source fields, set fill mode, start, check
; Returns: A=0 success, A=1 error
; =====================================================================
BLT_CMD_REG   = $BA83
BLT_STATUS_REG = $BA84
BLT_SRCSPACE  = $BA86
BLT_SRCL      = $BA88
BLT_SRCM      = $BA89
BLT_SRCH      = $BA8A
BLT_SRCSTRL   = $BA92
BLT_SRCSTRH   = $BA93
BLT_MODE_REG  = $BA96
BLT_CKEY      = $BA98
BLT_MODE_FILL = $01
BLT_CMD_START = $01

EXT_BLTFILL:
      LDA   #$00
      STA   BLT_SRCSPACE
      STA   BLT_SRCL
      STA   BLT_SRCM
      STA   BLT_SRCH
      STA   BLT_SRCSTRL
      STA   BLT_SRCSTRH
      STA   BLT_CKEY
      LDA   #BLT_MODE_FILL
      STA   BLT_MODE_REG
      LDA   #BLT_CMD_START
      STA   BLT_CMD_REG
      LDA   BLT_STATUS_REG
      ; fall through to ext_hw_chk

; shared: check hardware status (A=status). Returns A=0 ok, A=1 error.
ext_hw_chk:
      CMP   #HW_BUSY
      BEQ   @ok
      CMP   #HW_OK
      BEQ   @ok
      LDA   #$01
      RTS
@ok:  LDA   #$00
      RTS

      .include "lib/xram.s"
      .include "lib/xmc.s"

; =====================================================================
; XLOAD/XSAVE handlers — bridge BASIC's FIO-style arguments into the
; shared XRAM pseudo-register ABI, then call the assembly-facing runtime.
; Filename is already in FIO_NAME/FIO_NAMELEN.
; Returns: A=0 success, A=FIO_ERRCODE on error.
; =====================================================================
EXT_XLOAD:
      JSR   ext_xfio_load_args
      JSR   xram_xload
      BEQ   @ok
      LDA   FIO_ERRCODE
      BNE   @ret
      LDA   #FIO_ERR_IO
@ret: RTS
@ok:  LDA   #$00
      RTS

EXT_XSAVE:
      JSR   ext_xfio_load_args
      JSR   xram_xsave
      BEQ   @ok
      LDA   FIO_ERRCODE
      BNE   @ret
      LDA   #FIO_ERR_IO
@ret: RTS
@ok:  LDA   #$00
      RTS

ext_xfio_load_args:
      LDA   FIO_GADDRL
      STA   XRAM_ADDRL
      LDA   FIO_GADDRH
      STA   XRAM_ADDRM
      LDA   FIO_GSPACE
      STA   XRAM_ADDRH
      LDA   FIO_GLENL
      STA   XRAM_LENL
      LDA   FIO_GLENH
      STA   XRAM_LENH
      LDA   FIO_NAMELEN
      STA   XRAM_NAMELEN
      LDA   #<FIO_NAME
      STA   XRAM_NAMEPTR_L
      LDA   #>FIO_NAME
      STA   XRAM_NAMEPTR_H
      RTS

; =====================================================================
; ext_print_str — print inline null-terminated string
; String data follows the JSR instruction. Returns to byte after null.
; =====================================================================
ext_print_str:
      PLA                     ; low byte of return addr (points to string-1)
      STA   ext_ptrL
      PLA                     ; high byte
      STA   ext_ptrH
      LDY   #$01              ; skip past low byte of return addr
@ps_loop:
      LDA   (ext_ptrL),Y
      BEQ   @ps_done
      STA   VGC_CHAROUT
      INY
      BNE   @ps_loop
@ps_done:
      ; fix return address: ext_ptr + Y = address of null byte
      ; RTS needs addr-1 on stack, and Y points to null, so push ext_ptr+Y.
      ; Actually: ext_ptr = retaddr (from PLA)
      ; The string starts at retaddr+1. Null is at retaddr+Y.
      ; We need to return to retaddr+Y+1, so push retaddr+Y (RTS adds 1)
      TYA
      CLC
      ADC   ext_ptrL
      TAX                     ; low byte
      LDA   #$00
      ADC   ext_ptrH          ; high byte with carry
      PHA                     ; push high
      TXA
      PHA                     ; push low
      RTS                     ; return past the string

; =====================================================================
; ext_crlf — print CR LF
; =====================================================================
ext_crlf:
      LDA   #$0D
      STA   VGC_CHAROUT
      LDA   #$0A
      STA   VGC_CHAROUT
      RTS

; =====================================================================
; EXT_XMCCMD — entry point for XMC command processing from BASIC ROM
; Called via EXT_vec. Returns A=0 ok, A=1 error (for BNE in CHKOK).
; =====================================================================
EXT_XMCCMD:
      JSR   xmc_process
      LDA   XMC_STATUS
      CMP   #XMC_OK
      BEQ   @xmc_ret_ok
      LDA   #$01
      RTS
@xmc_ret_ok:
      LDA   #$00
      RTS

; =====================================================================
; EXT_COPPER — COPPER subcommand parser.
; Relocated from BASIC ROM (LAB_COPPER) to free ~390 bytes there.
; Original handler lived in basic.asm and used LAB_GBYT / LAB_IGBY ZP
; helpers (still callable from extension ROM since they are RAM-
; resident) plus LAB_GTBY / LAB_GTWRD / LAB_1C01 / LAB_15D9 ROM helpers
; (NOT callable here — replaced with the EXT_GTBY / EXT_GTWRD /
; EXT_1C01 / EXT_SNERR_VEC bridges).
;
; Subcommand grammar:
;   COPPER CLEAR | ON | OFF
;   COPPER LIST n | LIST END
;   COPPER USE n
;   COPPER ADD x, y, reg, value
;     reg = MODE | BGCOL | SCROLLX | SCROLLY |
;           SPRX(n) | SPRXH(n) | SPRY(n) |
;           SPRSHAPE(n) | SPRFLAGS(n) | SPRPRI(n) |
;           <8-bit numeric expression>
; =====================================================================
EXT_COPPER:
      JSR   LAB_GBYT          ; peek next byte (ZP routine, skips spaces)
      CMP   #TK_CLEAR_VAL
      BEQ   @c_clear
      CMP   #TK_ON_VAL
      BEQ   @c_on
      CMP   #TK_OFF_VAL
      BEQ   @c_off
      CMP   #'A'
      BEQ   @c_add
      CMP   #TK_LIST_VAL
      BEQ   @c_list
      CMP   #'U'
      BEQ   @c_use
      JMP   EXT_SNERR_VEC     ; syntax error

; --- COPPER CLEAR ---
@c_clear:
      JSR   LAB_IGBY          ; consume CLEAR token
      LDA   #VCMD_COPPERCLR
      STA   VGC_CMD
      LDA   #$00
      RTS

; --- COPPER ON ---
@c_on:
      JSR   LAB_IGBY          ; consume ON token
      LDA   #VCMD_COPPERENA
      STA   VGC_CMD
      LDA   #$00
      RTS

; --- COPPER OFF ---
@c_off:
      JSR   LAB_IGBY          ; consume OFF token
      LDA   #VCMD_COPPERDIS
      STA   VGC_CMD
      LDA   #$00
      RTS

; --- COPPER LIST n / COPPER LIST END ---
@c_list:
      JSR   LAB_IGBY          ; consume LIST token
      JSR   LAB_GBYT          ; peek next
      CMP   #TK_END_VAL
      BEQ   @c_list_end
      JSR   EXT_GTBY_VEC      ; bridge: parse 8-bit expr → X
      STX   VGC_P0
      LDA   #VCMD_COPPERLIST
      STA   VGC_CMD
      LDA   #$00
      RTS
@c_list_end:
      JSR   LAB_IGBY          ; consume END token
      LDA   #VCMD_COPPEREND
      STA   VGC_CMD
      LDA   #$00
      RTS

; --- COPPER USE n ---
@c_use:
      LDX   #3
      JSR   ext_skipx         ; USE
      JSR   EXT_GTBY_VEC      ; list index → X
      STX   VGC_P0
      LDA   #VCMD_COPPERUSE
      STA   VGC_CMD
      LDA   #$00
      RTS

; --- COPPER ADD x, y, reg, value ---
@c_add:
      LDX   #3
      JSR   ext_skipx         ; ADD
      JSR   EXT_GTWRD_VEC     ; x (16-bit) → FAC1_3 (lo) / FAC1_2 (hi)
      LDA   FAC1_3
      STA   VGC_P0
      LDA   FAC1_2
      STA   VGC_P1
      JSR   ext_comma         ; comma
      JSR   EXT_GTBY_VEC      ; y (8-bit) → X
      STX   VGC_P2
      JSR   ext_comma         ; comma
      ; parse register name
      JSR   LAB_GBYT          ; peek
      CMP   #'B'
      BEQ   @c_bgcol
      CMP   #TK_GMODE_VAL
      BEQ   @c_mode
      CMP   #'S'
      BEQ   @c_s_dispatch
      ; numeric register index fallback
      JSR   EXT_GTBY_VEC      ; 8-bit expression → X
      TXA
      JMP   @c_store_idx

@c_bgcol:
      LDX   #5
      JSR   ext_skipx         ; BGCOL
      LDA   #$01
      JMP   @c_store_idx

@c_mode:
      JSR   LAB_IGBY          ; consume MODE token
      LDA   #$00
      JMP   @c_store_idx

@c_s_dispatch:
      JSR   LAB_IGBY          ; consume S
      JSR   LAB_GBYT          ; peek next: C=scroll, P=sprite
      CMP   #'C'
      BEQ   @c_scroll
      CMP   #'P'
      BEQ   @c_spr
      JMP   EXT_SNERR_VEC

@c_scroll:
      LDX   #5
      JSR   ext_skipx         ; CROLL
      JSR   LAB_GBYT          ; peek X or Y
      CMP   #'X'
      BEQ   @c_scrollx
      CMP   #'Y'
      BEQ   @c_scrolly
      JMP   EXT_SNERR_VEC

@c_scrollx:
      JSR   LAB_IGBY          ; consume X
      LDA   #$05
      JMP   @c_store_idx

@c_scrolly:
      JSR   LAB_IGBY          ; consume Y
      LDA   #$06
      JMP   @c_store_idx

@c_store_idx:
      STA   VGC_P3
      LDA   #$00
      STA   VGC_P4
      JMP   @c_store_val

      ; --- Sprite register names: SPR + field + (n) ---
@c_spr:
      JSR   LAB_IGBY          ; consume P
      JSR   LAB_IGBY          ; consume R
      JSR   LAB_GBYT          ; peek field start: X, Y, S, F, P
      CMP   #'X'
      BEQ   @cs_x
      CMP   #'Y'
      BEQ   @cs_y
      CMP   #'S'
      BEQ   @cs_shape
      CMP   #'F'
      BEQ   @cs_flags
      CMP   #'P'
      BEQ   @cs_pri
      JMP   EXT_SNERR_VEC

@cs_x:
      JSR   LAB_IGBY          ; consume X
      JSR   LAB_GBYT          ; peek H or (
      CMP   #'H'
      BEQ   @cs_xh
      LDA   #$00
      JMP   @c_spr_idx
@cs_xh:
      JSR   LAB_IGBY          ; consume H
      LDA   #$01
      JMP   @c_spr_idx

@cs_y:
      JSR   LAB_IGBY          ; consume Y
      JSR   LAB_GBYT          ; peek H or (
      CMP   #'H'
      BEQ   @cs_yh
      LDA   #$02
      JMP   @c_spr_idx
@cs_yh:
      JSR   LAB_IGBY          ; consume H
      LDA   #$03
      JMP   @c_spr_idx

@cs_shape:
      LDX   #5
      JSR   ext_skipx         ; SHAPE
      LDA   #$04
      JMP   @c_spr_idx

@cs_flags:
      LDX   #5
      JSR   ext_skipx         ; FLAGS
      LDA   #$05
      JMP   @c_spr_idx

@cs_pri:
      LDX   #3
      JSR   ext_skipx         ; PRI
      LDA   #$06
      ; fall through

      ; A = field offset, parse (n) and compute $A040 + n*8 + A
@c_spr_idx:
      PHA                     ; save field offset
      JSR   LAB_IGBY          ; consume '('
      JSR   EXT_GTBY_VEC      ; sprite index → X
      CPX   #$10
      BCC   @cs_idx_ok
      JMP   EXT_SNERR_VEC
@cs_idx_ok:
      JSR   LAB_IGBY          ; consume ')'
      TXA                     ; A = sprite index
      ASL                     ; *2
      ASL                     ; *4
      ASL                     ; *8
      STA   VGC_P3            ; temp = n*8
      PLA                     ; A = field offset
      CLC
      ADC   VGC_P3
      ADC   #$40              ; +$40 (low byte of $A040)
      STA   VGC_P3
      LDA   #$A0              ; high byte of $A040
      ADC   #$00              ; add carry
      STA   VGC_P4

@c_store_val:
      JSR   ext_comma         ; comma
      JSR   EXT_GTBY_VEC      ; value → X
      STX   VGC_P5
      LDA   #VCMD_COPPERADD
      STA   VGC_CMD
      LDA   #$00
      RTS

; --- Local helpers used only by EXT_COPPER ---

; ext_skipx — skip X bytes of BASIC text (LAB_SKIPX equivalent)
ext_skipx:
      JSR   LAB_IGBY          ; advance + skip spaces
      DEX
      BNE   ext_skipx
      RTS

; ext_comma — consume comma (LAB_1C01 equivalent). Triggers syntax
; error via EXT_SNERR_VEC if next non-space char isn't ','.
ext_comma:
      JSR   LAB_GBYT          ; peek (skips spaces)
      CMP   #','
      BNE   @no_comma
      JSR   LAB_IGBY          ; consume comma
      RTS
@no_comma:
      JMP   EXT_SNERR_VEC

; =====================================================================
; Reset handler — if RESET fires while extension ROM is active,
; jump to RAM recovery code that swaps back to BASIC ROM.
; =====================================================================
ExtReset:
      CLD
      LDX   #$FF
      TXS
      JMP   EXT_RESET_VEC     ; RAM routine swaps to BASIC + JMP $FFBD

; =====================================================================
; CPU vectors
; =====================================================================
      .segment "VECTORS"
      .org    $FFFA

      .word $0217             ; NMI  -> RAM NMI handler
      .word ExtReset          ; RESET -> extension reset handler
      .word $020D             ; IRQ  -> RAM IRQ handler

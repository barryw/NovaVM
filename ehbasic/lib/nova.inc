; Shared Nova hardware constants for BASIC, assembly, and runtime libraries.
;
; This include is deliberately guarded per symbol so it can coexist with older
; source files that still define a subset of these names locally.

.ifndef NOVA_INC_INCLUDED
NOVA_INC_INCLUDED = 1

.ifndef NVR0L
NVR0L             = $20
.endif
.ifndef NVR0H
NVR0H             = $21
.endif
.ifndef NVR1L
NVR1L             = $22
.endif
.ifndef NVR1H
NVR1H             = $23
.endif
.ifndef NVR2L
NVR2L             = $24
.endif
.ifndef NVR2H
NVR2H             = $25
.endif
.ifndef NVR3L
NVR3L             = $26
.endif
.ifndef NVR3H
NVR3H             = $27
.endif
.ifndef NVR4L
NVR4L             = $28
.endif
.ifndef NVR4H
NVR4H             = $29
.endif
.ifndef NVR5L
NVR5L             = $2A
.endif
.ifndef NVR5H
NVR5H             = $2B
.endif
.ifndef NVR6L
NVR6L             = $2C
.endif
.ifndef NVR6H
NVR6H             = $2D
.endif
.ifndef NVR7L
NVR7L             = $2E
.endif
.ifndef NVR7H
NVR7H             = $2F
.endif

; BASIC SYS register bridge. SYS addr[,A[,X[,Y]]] loads A/X/Y from these
; bytes before jumping and stores A/X/Y back here when the routine returns.
; @label SYS.REGA
; @kind u8
; @symbol SYS_REGA
; @summary Returned A register from the most recent SYS call.
.ifndef SYS_REGA
SYS_REGA          = $E7
.endif
; @label SYS.REGX
; @kind u8
; @symbol SYS_REGX
; @summary Returned X register from the most recent SYS call.
.ifndef SYS_REGX
SYS_REGX          = $E8
.endif
; @label SYS.REGY
; @kind u8
; @symbol SYS_REGY
; @summary Returned Y register from the most recent SYS call.
.ifndef SYS_REGY
SYS_REGY          = $E9
.endif

; Extension ROM argument mailbox used by small BASIC stubs that parse
; BASIC-owned data before calling an extension handler.
.ifndef EXT_ARG_LEN
EXT_ARG_LEN       = $EA
.endif
.ifndef EXT_ARG_PTRL
EXT_ARG_PTRL      = $EB
.endif
.ifndef EXT_ARG_PTRH
EXT_ARG_PTRH      = $EC
.endif

.ifndef NOVA_SCREEN_COLS
NOVA_SCREEN_COLS  = 80
.endif
.ifndef NOVA_SCREEN_ROWS
NOVA_SCREEN_ROWS  = 50
.endif
.ifndef NOVA_SCREEN_SIZE
NOVA_SCREEN_SIZE  = NOVA_SCREEN_COLS * NOVA_SCREEN_ROWS
.endif

; ---------------------------------------------------------------------
; VGC core and text/VRAM port registers.
; ---------------------------------------------------------------------
.ifndef VGC_MODE
VGC_MODE          = $A000
.endif
.ifndef VGC_BGCOL
VGC_BGCOL         = $A001
.endif
.ifndef VGC_FGCOL
VGC_FGCOL         = $A002
.endif
.ifndef VGC_CURSX
VGC_CURSX         = $A003
.endif
.ifndef VGC_CURSY
VGC_CURSY         = $A004
.endif
.ifndef VGC_SCROLLX
VGC_SCROLLX       = $A005
.endif
.ifndef VGC_SCROLLY
VGC_SCROLLY       = $A006
.endif
.ifndef VGC_FONT
VGC_FONT          = $A007
.endif
.ifndef VGC_FRAME
VGC_FRAME         = $A008
.endif
.ifndef VGC_STATUS
VGC_STATUS        = $A008
.endif
.ifndef VGC_SPRCOUNT
VGC_SPRCOUNT      = $A009
.endif
.ifndef VGC_CURSEN
VGC_CURSEN        = $A00A
.endif
.ifndef VGC_COLLST
VGC_COLLST        = $A00B
.endif
.ifndef VGC_COLLBG
VGC_COLLBG        = $A00C
.endif
.ifndef VGC_BORDER
VGC_BORDER        = $A00D
.endif
.ifndef VGC_CHAROUT
VGC_CHAROUT       = $A00E
.endif
.ifndef VGC_CHARIN
VGC_CHARIN        = $A00F
.endif

.ifndef VGC_CMD
VGC_CMD           = $A010
.endif
.ifndef VGC_P0
VGC_P0            = $A011
.endif
.ifndef VGC_P1
VGC_P1            = $A012
.endif
.ifndef VGC_P2
VGC_P2            = $A013
.endif
.ifndef VGC_P3
VGC_P3            = $A014
.endif
.ifndef VGC_P4
VGC_P4            = $A015
.endif
.ifndef VGC_P5
VGC_P5            = $A016
.endif
.ifndef VGC_P6
VGC_P6            = $A017
.endif
.ifndef VGC_P7
VGC_P7            = $A018
.endif
.ifndef VGC_P8
VGC_P8            = $A019
.endif
.ifndef VGC_P9
VGC_P9            = $A01A
.endif
.ifndef VGC_P10
VGC_P10           = $A01B
.endif
.ifndef VGC_P11
VGC_P11           = $A01C
.endif
.ifndef VGC_P12
VGC_P12           = $A01D
.endif
.ifndef VGC_P13
VGC_P13           = $A01E
.endif
.ifndef VGC_P14
VGC_P14           = $A01F
.endif

.ifndef REG_HELP
REG_HELP          = $A020
.endif
.ifndef HELP_BUF
HELP_BUF          = $A021
.endif
.ifndef REG_ROMSWAP
REG_ROMSWAP       = $A03F
.endif
.ifndef ROMSWAP_NCC
ROMSWAP_NCC       = $01
.endif
.ifndef ROMSWAP_BASIC
ROMSWAP_BASIC     = $02
.endif
.ifndef ROMSWAP_NCCEDIT
ROMSWAP_NCCEDIT   = $03
.endif
.ifndef ROMSWAP_EXTENSION
ROMSWAP_EXTENSION = $04
.endif

.ifndef VGC_SPR_BASE
VGC_SPR_BASE      = $A040
.endif
.ifndef VGC_SPR_STRIDE
VGC_SPR_STRIDE    = $08
.endif
.ifndef VGC_MAX_SPRITES
VGC_MAX_SPRITES   = $10
.endif
.ifndef VGC_SPR_XL_OFF
VGC_SPR_XL_OFF    = $00
.endif
.ifndef VGC_SPR_XH_OFF
VGC_SPR_XH_OFF    = $01
.endif
.ifndef VGC_SPR_YL_OFF
VGC_SPR_YL_OFF    = $02
.endif
.ifndef VGC_SPR_YH_OFF
VGC_SPR_YH_OFF    = $03
.endif
.ifndef VGC_SPR_SHAPE_OFF
VGC_SPR_SHAPE_OFF = $04
.endif
.ifndef VGC_SPR_FLAGS_OFF
VGC_SPR_FLAGS_OFF = $05
.endif
.ifndef VGC_SPR_PRI_OFF
VGC_SPR_PRI_OFF   = $06
.endif
.ifndef VGC_SPR_TRANS_OFF
VGC_SPR_TRANS_OFF = $07
.endif
.ifndef VGC_SPR_XL
VGC_SPR_XL        = VGC_SPR_BASE + VGC_SPR_XL_OFF
.endif
.ifndef VGC_SPR_XH
VGC_SPR_XH        = VGC_SPR_BASE + VGC_SPR_XH_OFF
.endif
.ifndef VGC_SPR_YL
VGC_SPR_YL        = VGC_SPR_BASE + VGC_SPR_YL_OFF
.endif
.ifndef VGC_SPR_YH
VGC_SPR_YH        = VGC_SPR_BASE + VGC_SPR_YH_OFF
.endif
.ifndef VGC_SPR_SHAPE
VGC_SPR_SHAPE     = VGC_SPR_BASE + VGC_SPR_SHAPE_OFF
.endif
.ifndef VGC_SPR_FLAGS
VGC_SPR_FLAGS     = VGC_SPR_BASE + VGC_SPR_FLAGS_OFF
.endif
.ifndef VGC_SPR_PRI
VGC_SPR_PRI       = VGC_SPR_BASE + VGC_SPR_PRI_OFF
.endif
.ifndef VGC_SPR_TRANS
VGC_SPR_TRANS     = VGC_SPR_BASE + VGC_SPR_TRANS_OFF
.endif
.ifndef VGC_SPR_FLAG_XFLIP
VGC_SPR_FLAG_XFLIP = $01
.endif
.ifndef VGC_SPR_FLAG_YFLIP
VGC_SPR_FLAG_YFLIP = $02
.endif
.ifndef VGC_SPR_FLAG_FLIPMASK
VGC_SPR_FLAG_FLIPMASK = $03
.endif
.ifndef VGC_SPR_FLAG_ENABLE
VGC_SPR_FLAG_ENABLE = $80
.endif
.ifndef VGC_SPR_PRI_BEHIND
VGC_SPR_PRI_BEHIND = $00
.endif
.ifndef VGC_SPR_PRI_BETWEEN
VGC_SPR_PRI_BETWEEN = $01
.endif
.ifndef VGC_SPR_PRI_FRONT
VGC_SPR_PRI_FRONT = $02
.endif

.ifndef VGC_VRAM_PLANE
VGC_VRAM_PLANE    = $A0E0
.endif
.ifndef VGC_VRAM_ADDRL
VGC_VRAM_ADDRL    = $A0E1
.endif
.ifndef VGC_VRAM_ADDRH
VGC_VRAM_ADDRH    = $A0E2
.endif
.ifndef VGC_VRAM_DATA
VGC_VRAM_DATA     = $A0E3
.endif
.ifndef VGC_VRAM_CTRL
VGC_VRAM_CTRL     = $A0E4
.endif
.ifndef VGC_DIMMER
VGC_DIMMER        = $A0E5
.endif
.ifndef VGC_TXTFLAGS
VGC_TXTFLAGS      = $A0E6
.endif
.ifndef VGC_TXTREVATTR
VGC_TXTREVATTR    = $A0E7
.endif

.ifndef VGC_VPLANE
VGC_VPLANE        = VGC_VRAM_PLANE
.endif
.ifndef VGC_VADDRL
VGC_VADDRL        = VGC_VRAM_ADDRL
.endif
.ifndef VGC_VADDRH
VGC_VADDRH        = VGC_VRAM_ADDRH
.endif
.ifndef VGC_VDATA
VGC_VDATA         = VGC_VRAM_DATA
.endif
.ifndef VGC_VCTRL
VGC_VCTRL         = VGC_VRAM_CTRL
.endif

.ifndef VGC_PLANE_CHAR
VGC_PLANE_CHAR    = $01
.endif
.ifndef VGC_PLANE_COLOR
VGC_PLANE_COLOR   = $02
.endif
.ifndef VGC_PLANE_GFX
VGC_PLANE_GFX     = $03
.endif
.ifndef VGC_PLANE_SPRITE
VGC_PLANE_SPRITE  = $04
.endif
.ifndef VGC_PLANE_TILE
VGC_PLANE_TILE    = $06
.endif
.ifndef VGC_PLANE_TEXTATTR
VGC_PLANE_TEXTATTR = $07
.endif
.ifndef VGC_VRAM_AUTOINC
VGC_VRAM_AUTOINC  = $01
.endif

.ifndef VTXT_REV
VTXT_REV          = $01
.endif
.ifndef VTXT_REVEX
VTXT_REVEX        = $02
.endif
.ifndef VTXT_FLASH
VTXT_FLASH        = $04
.endif
.ifndef VTXT_ATTR_FLASH
VTXT_ATTR_FLASH   = $01
.endif

; VGC IRQ and command constants.
.ifndef VGC_IRQ_ENABLE
VGC_IRQ_ENABLE    = $A0F0
.endif
.ifndef VGC_IRQ_STATUS
VGC_IRQ_STATUS    = $A0F1
.endif
.ifndef VGC_IRQ_FORCE
VGC_IRQ_FORCE     = $A0F2
.endif
.ifndef VGC_IRQ_VALID
VGC_IRQ_VALID     = $A0F3
.endif

.ifndef VCMD_PLOT
VCMD_PLOT         = $01
.endif
.ifndef VCMD_UNPLOT
VCMD_UNPLOT       = $02
.endif
.ifndef VCMD_LINE
VCMD_LINE         = $03
.endif
.ifndef VCMD_CIRCLE
VCMD_CIRCLE       = $04
.endif
.ifndef VCMD_RECT
VCMD_RECT         = $05
.endif
.ifndef VCMD_FILL
VCMD_FILL         = $06
.endif
.ifndef VCMD_GCLS
VCMD_GCLS         = $07
.endif
.ifndef VCMD_GCOLOR
VCMD_GCOLOR       = $08
.endif
.ifndef VCMD_PAINT
VCMD_PAINT        = $09
.endif
.ifndef VCMD_GTEXT
VCMD_GTEXT        = $0A
.endif
.ifndef VCMD_SPRDEF
VCMD_SPRDEF       = $10
.endif
.ifndef VCMD_SPRROW
VCMD_SPRROW       = $11
.endif
.ifndef VCMD_SPRCLR
VCMD_SPRCLR       = $12
.endif
.ifndef VCMD_SPRCOPY
VCMD_SPRCOPY      = $13
.endif
.ifndef VCMD_SPRPOS
VCMD_SPRPOS       = $14
.endif
.ifndef VCMD_SPRENA
VCMD_SPRENA       = $15
.endif
.ifndef VCMD_SPRDIS
VCMD_SPRDIS       = $16
.endif
.ifndef VCMD_SPRFLIP
VCMD_SPRFLIP      = $17
.endif
.ifndef VCMD_SPRPRI
VCMD_SPRPRI       = $18
.endif
.ifndef VCMD_MEMREAD
VCMD_MEMREAD      = $19
.endif
.ifndef VCMD_MEMWRITE
VCMD_MEMWRITE     = $1A
.endif
.ifndef VCMD_COPPERADD
VCMD_COPPERADD    = $1B
.endif
.ifndef VCMD_COPPERCLR
VCMD_COPPERCLR    = $1C
.endif
.ifndef VCMD_COPPERENA
VCMD_COPPERENA    = $1D
.endif
.ifndef VCMD_COPPERDIS
VCMD_COPPERDIS    = $1E
.endif
.ifndef VCMD_SYSRESET
VCMD_SYSRESET     = $1F
.endif
.ifndef VCMD_COPPERLIST
VCMD_COPPERLIST   = $20
.endif
.ifndef VCMD_COPPERUSE
VCMD_COPPERUSE    = $21
.endif
.ifndef VCMD_COPPEREND
VCMD_COPPEREND    = $22
.endif

; ---------------------------------------------------------------------
; NIC network interface controller.
; ---------------------------------------------------------------------
.ifndef NIC_CMD
NIC_CMD           = $A100
.endif
.ifndef NIC_STATUS
NIC_STATUS        = $A101
.endif
.ifndef NIC_SLOT
NIC_SLOT          = $A102
.endif
.ifndef NIC_IRQCTRL
NIC_IRQCTRL       = $A103
.endif
.ifndef NIC_IRQSTATUS
NIC_IRQSTATUS     = $A104
.endif
.ifndef NIC_RPORTL
NIC_RPORTL        = $A108
.endif
.ifndef NIC_RPORTH
NIC_RPORTH        = $A109
.endif
.ifndef NIC_LPORTL
NIC_LPORTL        = $A10A
.endif
.ifndef NIC_LPORTH
NIC_LPORTH        = $A10B
.endif
.ifndef NIC_DMAL
NIC_DMAL          = $A110
.endif
.ifndef NIC_DMAH
NIC_DMAH          = $A111
.endif
.ifndef NIC_DMALEN
NIC_DMALEN        = $A112
.endif
.ifndef NIC_MSGLEN
NIC_MSGLEN        = $A113
.endif
.ifndef NIC_SLOTST0
NIC_SLOTST0       = $A118
.endif
.ifndef NIC_SLOTST1
NIC_SLOTST1       = $A119
.endif
.ifndef NIC_SLOTST2
NIC_SLOTST2       = $A11A
.endif
.ifndef NIC_SLOTST3
NIC_SLOTST3       = $A11B
.endif
.ifndef NIC_NAMEBUF
NIC_NAMEBUF       = $A120
.endif
.ifndef NIC_NAMEEND
NIC_NAMEEND       = $A13F
.endif

.ifndef NIC_CMD_CONNECT
NIC_CMD_CONNECT   = $01
.endif
.ifndef NIC_CMD_DISCONNECT
NIC_CMD_DISCONNECT = $02
.endif
.ifndef NIC_CMD_SEND
NIC_CMD_SEND      = $03
.endif
.ifndef NIC_CMD_RECV
NIC_CMD_RECV      = $04
.endif
.ifndef NIC_CMD_LISTEN
NIC_CMD_LISTEN    = $05
.endif
.ifndef NIC_CMD_ACCEPT
NIC_CMD_ACCEPT    = $06
.endif
.ifndef NIC_ST_CONNECTED
NIC_ST_CONNECTED  = $01
.endif
.ifndef NIC_ST_DATAREADY
NIC_ST_DATAREADY  = $02
.endif
.ifndef NIC_ST_SENDREADY
NIC_ST_SENDREADY  = $04
.endif
.ifndef NIC_ST_ERROR
NIC_ST_ERROR      = $08
.endif
.ifndef NIC_ST_REMOTE_CLOSED
NIC_ST_REMOTE_CLOSED = $10
.endif
.ifndef NIC_STATUS_READY
NIC_STATUS_READY  = $01
.endif
.ifndef NIC_STATUS_ANYDATA
NIC_STATUS_ANYDATA = $02
.endif
.ifndef NIC_STATUS_ANYERROR
NIC_STATUS_ANYERROR = $80
.endif
.ifndef NIC_MAX_SLOTS
NIC_MAX_SLOTS     = $04
.endif
.ifndef NIC_MAX_MSGSIZE
NIC_MAX_MSGSIZE   = $0100
.endif
.ifndef NIC_MAX_QUEUE
NIC_MAX_QUEUE     = $10
.endif

; ---------------------------------------------------------------------
; Tile engine registers and command constants.
; ---------------------------------------------------------------------
.ifndef TileConfig
TileConfig        = $A0C0
.endif
.ifndef TileTransColor
TileTransColor    = $A0C1
.endif
.ifndef TileScrollXL
TileScrollXL      = $A0C2
.endif
.ifndef TileScrollXH
TileScrollXH      = $A0C3
.endif
.ifndef TileScrollYL
TileScrollYL      = $A0C4
.endif
.ifndef TileScrollYH
TileScrollYH      = $A0C5
.endif
.ifndef TileStatus
TileStatus        = $A0C6
.endif
.ifndef TileCmd
TileCmd           = $A0C7
.endif
.ifndef TileP0
TileP0            = $A0C8
.endif
.ifndef TileP1
TileP1            = $A0C9
.endif
.ifndef TileP2
TileP2            = $A0CA
.endif
.ifndef TileP3
TileP3            = $A0CB
.endif
.ifndef TileAddrL
TileAddrL         = $A0CC
.endif
.ifndef TileAddrH
TileAddrH         = $A0CD
.endif
.ifndef TilePalP0
TilePalP0         = $A0CE
.endif
.ifndef TilePalP1
TilePalP1         = $A0CF
.endif
.ifndef TileColL
TileColL          = $A0D0
.endif
.ifndef TileColH
TileColH          = $A0D1
.endif
.ifndef TilePeekVal
TilePeekVal       = $A0D2
.endif
.ifndef TilePeekAttr
TilePeekAttr      = $A0D3
.endif

.ifndef TileCfgSize16
TileCfgSize16     = $01
.endif
.ifndef TileCfgMirrorMask
TileCfgMirrorMask = $06
.endif
.ifndef TileCfgMirrorShift
TileCfgMirrorShift = 1
.endif
.ifndef TileCmdDef
TileCmdDef        = $01
.endif
.ifndef TileCmdDefBulk
TileCmdDefBulk    = $02
.endif
.ifndef TileCmdPut
TileCmdPut        = $03
.endif
.ifndef TileCmdAttr
TileCmdAttr       = $04
.endif
.ifndef TileCmdFill
TileCmdFill       = $05
.endif
.ifndef TileCmdRow
TileCmdRow        = $06
.endif
.ifndef TileCmdCol
TileCmdCol        = $07
.endif
.ifndef TileCmdLoad
TileCmdLoad       = $08
.endif
.ifndef TileCmdALoad
TileCmdALoad      = $09
.endif
.ifndef TileCmdPal
TileCmdPal        = $0A
.endif
.ifndef TileCmdPalC
TileCmdPalC       = $0B
.endif
.ifndef TileCmdPeek
TileCmdPeek       = $0C
.endif
.ifndef TileCmdRowAttr
TileCmdRowAttr    = $0D
.endif
.ifndef TileCmdColAttr
TileCmdColAttr    = $0E
.endif
.ifndef TileCmdCls
TileCmdCls        = $0F
.endif
.ifndef TileCmdBufFill
TileCmdBufFill    = $10
.endif
.ifndef TileCmdBufSet
TileCmdBufSet     = $11
.endif
.ifndef TileCmdBufRange
TileCmdBufRange   = $12
.endif
.ifndef TileCmdBufPut
TileCmdBufPut     = $13
.endif

; ---------------------------------------------------------------------
; File I/O coprocessor registers and command/status constants.
; ---------------------------------------------------------------------
.ifndef FIO_CMD
FIO_CMD           = $B9A0
.endif
.ifndef FIO_STATUS
FIO_STATUS        = $B9A1
.endif
.ifndef FIO_ERRCODE
FIO_ERRCODE       = $B9A2
.endif
; @label FIO.NAMELEN
; @kind u8
; @symbol FIO_NAMELEN
; @summary Length of the filename in FIO.NAME, 1..63 bytes.
.ifndef FIO_NAMELEN
FIO_NAMELEN       = $B9A3
.endif
.ifndef FIO_SRCL
FIO_SRCL          = $B9A4
.endif
.ifndef FIO_SRCH
FIO_SRCH          = $B9A5
.endif
.ifndef FIO_ENDL
FIO_ENDL          = $B9A6
.endif
.ifndef FIO_ENDH
FIO_ENDH          = $B9A7
.endif
.ifndef FIO_SIZEL
FIO_SIZEL         = $B9A8
.endif
.ifndef FIO_SIZEH
FIO_SIZEH         = $B9A9
.endif
.ifndef FIO_GSPACE
FIO_GSPACE        = $B9AA
.endif
.ifndef FIO_GADDRL
FIO_GADDRL        = $B9AB
.endif
.ifndef FIO_GADDRH
FIO_GADDRH        = $B9AC
.endif
.ifndef FIO_GLENL
FIO_GLENL         = $B9AD
.endif
.ifndef FIO_GLENH
FIO_GLENH         = $B9AE
.endif
.ifndef FIO_DIRTYPE
FIO_DIRTYPE       = $B9AF
.endif
; @label FIO.NAME
; @kind buffer
; @symbol FIO_NAME
; @summary Filename buffer used by file-backed runtime routines.
.ifndef FIO_NAME
FIO_NAME          = $B9B0
.endif
.ifndef FIO_NAMEEND
FIO_NAMEEND       = $B9EF
.endif
.ifndef META_BASE
META_BASE         = $BAB0
.endif
.ifndef AUTOBOOT_SKIP
AUTOBOOT_SKIP     = $B9F0
.endif
.ifndef AUTOBOOT_ACTIVE
AUTOBOOT_ACTIVE   = $B9F1
.endif

.ifndef FIO_CMD_SAVE
FIO_CMD_SAVE      = $01
.endif
.ifndef FIO_CMD_LOAD
FIO_CMD_LOAD      = $02
.endif
.ifndef FIO_CMD_DIROPEN
FIO_CMD_DIROPEN   = $03
.endif
.ifndef FIO_CMD_DIRREAD
FIO_CMD_DIRREAD   = $04
.endif
.ifndef FIO_CMD_DELETE
FIO_CMD_DELETE    = $05
.endif
.ifndef FIO_CMD_GSAVE
FIO_CMD_GSAVE     = $06
.endif
.ifndef FIO_CMD_GLOAD
FIO_CMD_GLOAD     = $07
.endif
.ifndef FIO_CMD_SIDPLAY
FIO_CMD_SIDPLAY   = $08
.endif
.ifndef FIO_CMD_SIDSTOP
FIO_CMD_SIDSTOP   = $09
.endif
.ifndef FIO_CMD_INSTRUMENT
FIO_CMD_INSTRUMENT = $0A
.endif
.ifndef FIO_CMD_SOUND
FIO_CMD_SOUND     = $0B
.endif
.ifndef FIO_CMD_VOLUME
FIO_CMD_VOLUME    = $0C
.endif
.ifndef FIO_CMD_MSEQ
FIO_CMD_MSEQ      = $0D
.endif
.ifndef FIO_CMD_MPLAY
FIO_CMD_MPLAY     = $0E
.endif
.ifndef FIO_CMD_MSTOP
FIO_CMD_MSTOP     = $0F
.endif
.ifndef FIO_CMD_MTEMPO
FIO_CMD_MTEMPO    = $10
.endif
.ifndef FIO_CMD_MLOOP
FIO_CMD_MLOOP     = $11
.endif
.ifndef FIO_CMD_MPRI
FIO_CMD_MPRI      = $12
.endif
.ifndef FIO_CMD_MIDPLAY
FIO_CMD_MIDPLAY   = $13
.endif
.ifndef FIO_CMD_MIDSTOP
FIO_CMD_MIDSTOP   = $14
.endif
.ifndef FIO_CMD_SFLOAD
FIO_CMD_SFLOAD    = $15
.endif
.ifndef FIO_CMD_TSAVE
FIO_CMD_TSAVE     = $16
.endif
.ifndef FIO_CMD_TLOAD
FIO_CMD_TLOAD     = $17
.endif
.ifndef FioCmdTSave
FioCmdTSave       = FIO_CMD_TSAVE
.endif
.ifndef FioCmdTLoad
FioCmdTLoad       = FIO_CMD_TLOAD
.endif
.ifndef FIO_CMD_XLOAD
FIO_CMD_XLOAD     = $18
.endif
.ifndef FIO_CMD_XSAVE
FIO_CMD_XSAVE     = $19
.endif
.ifndef FIO_CMD_CD
FIO_CMD_CD        = $20
.endif
.ifndef FIO_CMD_MKDIR
FIO_CMD_MKDIR     = $21
.endif
.ifndef FIO_CMD_RMDIR
FIO_CMD_RMDIR     = $22
.endif
.ifndef FIO_CMD_FORMAT
FIO_CMD_FORMAT    = $23
.endif
.ifndef FIO_CMD_MOUNT
FIO_CMD_MOUNT     = $24
.endif
.ifndef FIO_CMD_UNMOUNT
FIO_CMD_UNMOUNT   = $25
.endif
.ifndef FIO_CMD_PWD
FIO_CMD_PWD       = $26
.endif
.ifndef FIO_CMD_CLEARERR
FIO_CMD_CLEARERR  = $27
.endif

.ifndef FIO_OK
FIO_OK            = $02
.endif
.ifndef FIO_STATUS_IDLE
FIO_STATUS_IDLE   = $00
.endif
.ifndef FIO_STATUS_OK
FIO_STATUS_OK     = FIO_OK
.endif
.ifndef FIO_ERR
FIO_ERR           = $03
.endif
.ifndef FIO_STATUS_ERROR
FIO_STATUS_ERROR  = $03
.endif
.ifndef FIO_ERR_NONE
FIO_ERR_NONE      = $00
.endif
.ifndef FIO_ERR_NOTFOUND
FIO_ERR_NOTFOUND  = $01
.endif
.ifndef FIO_ERR_IO
FIO_ERR_IO        = $02
.endif
.ifndef FIO_ERR_EOD
FIO_ERR_EOD       = $03
.endif
.ifndef FIO_ERR_DISKFULL
FIO_ERR_DISKFULL  = $04
.endif
.ifndef FIO_ERR_NOTMOUNTED
FIO_ERR_NOTMOUNTED = $05
.endif
.ifndef FIO_TYPE_SID
FIO_TYPE_SID      = $01
.endif
.ifndef FIO_TYPE_BIN
FIO_TYPE_BIN      = $02
.endif
.ifndef FIO_TYPE_MID
FIO_TYPE_MID      = $03
.endif
.ifndef FIO_TYPE_DIR
FIO_TYPE_DIR      = $05
.endif

; Extension ROM command IDs used by the BASIC-to-extension trampoline.
.ifndef EXT_CMD_NCC
EXT_CMD_NCC       = $00
.endif
.ifndef EXT_CMD_SFLOAD
EXT_CMD_SFLOAD    = $01
.endif
.ifndef EXT_CMD_DIR
EXT_CMD_DIR       = $02
.endif
.ifndef EXT_CMD_PWD
EXT_CMD_PWD       = $03
.endif
.ifndef EXT_CMD_XMEM
EXT_CMD_XMEM      = $04
.endif
.ifndef EXT_CMD_XDIR
EXT_CMD_XDIR      = $05
.endif
.ifndef EXT_CMD_TSAVE
EXT_CMD_TSAVE     = $06
.endif
.ifndef EXT_CMD_TLOAD
EXT_CMD_TLOAD     = $07
.endif
.ifndef EXT_CMD_HELP
EXT_CMD_HELP      = $08
.endif
.ifndef EXT_CMD_DMAFILL
EXT_CMD_DMAFILL   = $09
.endif
.ifndef EXT_CMD_BLTFILL
EXT_CMD_BLTFILL   = $0A
.endif
.ifndef EXT_CMD_XMCCMD
EXT_CMD_XMCCMD    = $0B
.endif
.ifndef EXT_CMD_COPPER
EXT_CMD_COPPER    = $0C
.endif
.ifndef EXT_CMD_XLOAD
EXT_CMD_XLOAD     = $0D
.endif
.ifndef EXT_CMD_XSAVE
EXT_CMD_XSAVE     = $0E
.endif
.ifndef EXT_CMD_DMACOPY
EXT_CMD_DMACOPY   = $0F
.endif
.ifndef EXT_CMD_BLITCOPY
EXT_CMD_BLITCOPY  = $10
.endif
.ifndef EXT_CMD_TILESIZE
EXT_CMD_TILESIZE  = $11
.endif
.ifndef EXT_CMD_TMIRROR
EXT_CMD_TMIRROR   = $12
.endif
.ifndef EXT_CMD_TTRANS
EXT_CMD_TTRANS    = $13
.endif
.ifndef EXT_CMD_TDEF
EXT_CMD_TDEF      = $14
.endif
.ifndef EXT_CMD_TPUT
EXT_CMD_TPUT      = $15
.endif
.ifndef EXT_CMD_TATTR
EXT_CMD_TATTR     = $16
.endif
.ifndef EXT_CMD_TFILL
EXT_CMD_TFILL     = $17
.endif
.ifndef EXT_CMD_TROW
EXT_CMD_TROW      = $18
.endif
.ifndef EXT_CMD_TCOL
EXT_CMD_TCOL      = $19
.endif
.ifndef EXT_CMD_TNTLOAD
EXT_CMD_TNTLOAD   = $1A
.endif
.ifndef EXT_CMD_TCLS
EXT_CMD_TCLS      = $1B
.endif
.ifndef EXT_CMD_TBUF
EXT_CMD_TBUF      = $1C
.endif
.ifndef EXT_CMD_TBSET
EXT_CMD_TBSET     = $1D
.endif
.ifndef EXT_CMD_TBPUT
EXT_CMD_TBPUT     = $1E
.endif
.ifndef EXT_CMD_TBFILL
EXT_CMD_TBFILL    = $1F
.endif
.ifndef EXT_CMD_TSCROLL
EXT_CMD_TSCROLL   = $20
.endif
.ifndef EXT_CMD_TPAL
EXT_CMD_TPAL      = $21
.endif
.ifndef EXT_CMD_TPALC
EXT_CMD_TPALC     = $22
.endif
.ifndef EXT_CMD_ADDR
EXT_CMD_ADDR      = $23
.endif
.ifndef EXT_CMD_XBANK
EXT_CMD_XBANK     = $24
.endif
.ifndef EXT_CMD_XPOKE
EXT_CMD_XPOKE     = $25
.endif
.ifndef EXT_CMD_XFREE
EXT_CMD_XFREE     = $26
.endif
.ifndef EXT_CMD_XRESET
EXT_CMD_XRESET    = $27
.endif
.ifndef EXT_CMD_XALLOC
EXT_CMD_XALLOC    = $28
.endif
.ifndef EXT_CMD_XMAP
EXT_CMD_XMAP      = $29
.endif
.ifndef EXT_CMD_XUNMAP
EXT_CMD_XUNMAP    = $2A
.endif

; Music/SFX status registers.
.ifndef MUSIC_STATUS
MUSIC_STATUS      = $BA50
.endif
.ifndef MUSIC_NOTE1
MUSIC_NOTE1       = $BA51
.endif
.ifndef MUSIC_NOTE2
MUSIC_NOTE2       = $BA52
.endif
.ifndef MUSIC_NOTE3
MUSIC_NOTE3       = $BA53
.endif
.ifndef MUSIC_NOTE4
MUSIC_NOTE4       = $BA54
.endif
.ifndef MUSIC_NOTE5
MUSIC_NOTE5       = $BA55
.endif
.ifndef MUSIC_NOTE6
MUSIC_NOTE6       = $BA56
.endif
.ifndef MUSIC_NOTE7
MUSIC_NOTE7       = $BA57
.endif
.ifndef MUSIC_NOTE8
MUSIC_NOTE8       = $BA58
.endif
.ifndef MUSIC_NOTE9
MUSIC_NOTE9       = $BA59
.endif
.ifndef MUSIC_NOTE10
MUSIC_NOTE10      = $BA5A
.endif
.ifndef MUSIC_NOTE11
MUSIC_NOTE11      = $BA5B
.endif
.ifndef MUSIC_NOTE12
MUSIC_NOTE12      = $BA5C
.endif
.ifndef MUSIC_NOTE13
MUSIC_NOTE13      = $BA5D
.endif
.ifndef MUSIC_NOTE14
MUSIC_NOTE14      = $BA5E
.endif
.ifndef MUSIC_ELAPSEDL
MUSIC_ELAPSEDL    = $BA5F
.endif
.ifndef MUSIC_ELAPSEDH
MUSIC_ELAPSEDH    = $BA60
.endif
.ifndef MUSIC_TOTALL
MUSIC_TOTALL      = $BA61
.endif
.ifndef MUSIC_TOTALH
MUSIC_TOTALH      = $BA62
.endif

; SID register windows. CPU reads in this range still fetch ROM bytes on Nova;
; these names are for writes and emulator/host-side register shadow APIs.
.ifndef SID_BASE
SID_BASE          = $D400
.endif
.ifndef SID_END
SID_END           = $D41C
.endif
.ifndef SID2_BASE
SID2_BASE         = $D420
.endif
.ifndef SID2_END
SID2_END          = $D43C
.endif
.ifndef SID2_MIRROR_BASE
SID2_MIRROR_BASE  = $D500
.endif
.ifndef SID2_MIRROR_END
SID2_MIRROR_END   = $D51C
.endif

; Wavetable synthesizer registers.
.ifndef WTS_BASE
WTS_BASE          = $A140
.endif
.ifndef WTS_VOICE_BASE
WTS_VOICE_BASE    = $A140
.endif
.ifndef WTS_VOICE_STRIDE
WTS_VOICE_STRIDE  = $08
.endif
.ifndef WTS_VOICE_END
WTS_VOICE_END     = $A17F
.endif
.ifndef WTS_VOICE_NOTE
WTS_VOICE_NOTE    = $00
.endif
.ifndef WTS_VOICE_VELOCITY
WTS_VOICE_VELOCITY = $01
.endif
.ifndef WTS_VOICE_INSTRUMENT
WTS_VOICE_INSTRUMENT = $02
.endif
.ifndef WTS_VOICE_VOLUME
WTS_VOICE_VOLUME  = $03
.endif
.ifndef WTS_VOICE_PANNING
WTS_VOICE_PANNING = $04
.endif
.ifndef WTS_VOICE_PITCH_BEND_LO
WTS_VOICE_PITCH_BEND_LO = $05
.endif
.ifndef WTS_VOICE_PITCH_BEND_HI
WTS_VOICE_PITCH_BEND_HI = $06
.endif
.ifndef WTS_VOICE_STATUS
WTS_VOICE_STATUS  = $07
.endif
.ifndef WTS_GLOBAL_BASE
WTS_GLOBAL_BASE   = $A180
.endif
.ifndef WTS_REVERB_LEVEL
WTS_REVERB_LEVEL  = $A180
.endif
.ifndef WTS_CHORUS_LEVEL
WTS_CHORUS_LEVEL  = $A181
.endif
.ifndef WTS_MASTER_VOLUME
WTS_MASTER_VOLUME = $A182
.endif
.ifndef WTS_SOUNDFONT_STATUS
WTS_SOUNDFONT_STATUS = $A183
.endif
.ifndef WTS_INSTRUMENT_COUNT
WTS_INSTRUMENT_COUNT = $A184
.endif
.ifndef WTS_COMMAND
WTS_COMMAND       = $A185
.endif
.ifndef WTS_COMMAND_ARG
WTS_COMMAND_ARG   = $A186
.endif
.ifndef WTS_ACTIVE_VOICES
WTS_ACTIVE_VOICES = $A187
.endif
.ifndef WTS_ENUM_BASE
WTS_ENUM_BASE     = $A1A0
.endif
.ifndef WTS_ENUM_INDEX
WTS_ENUM_INDEX    = $A1A0
.endif
.ifndef WTS_ENUM_BANK
WTS_ENUM_BANK     = $A1A1
.endif
.ifndef WTS_ENUM_PROGRAM
WTS_ENUM_PROGRAM  = $A1A2
.endif
.ifndef WTS_ENUM_NAME
WTS_ENUM_NAME     = $A1A3
.endif
.ifndef WTS_ENUM_END
WTS_ENUM_END      = $A1DF
.endif
.ifndef WTS_END
WTS_END           = $A1DF
.endif
.ifndef WTS_CMD_ALL_NOTES_OFF
WTS_CMD_ALL_NOTES_OFF = $01
.endif
.ifndef WTS_CMD_RESET_EFFECTS
WTS_CMD_RESET_EFFECTS = $02
.endif

; ---------------------------------------------------------------------
; Expansion Memory Controller.
; ---------------------------------------------------------------------
.ifndef XMC_CMD
XMC_CMD           = $BA00
.endif
.ifndef XMC_STATUS
XMC_STATUS        = $BA01
.endif
.ifndef XMC_ERRCODE
XMC_ERRCODE       = $BA02
.endif
.ifndef XMC_CFG
XMC_CFG           = $BA03
.endif
.ifndef XMC_XAL
XMC_XAL           = $BA04
.endif
.ifndef XMC_XAM
XMC_XAM           = $BA05
.endif
.ifndef XMC_XAH
XMC_XAH           = $BA06
.endif
.ifndef XMC_RAML
XMC_RAML          = $BA07
.endif
.ifndef XMC_RAMH
XMC_RAMH          = $BA08
.endif
.ifndef XMC_LENL
XMC_LENL          = $BA09
.endif
.ifndef XMC_LENH
XMC_LENH          = $BA0A
.endif
.ifndef XMC_DATA
XMC_DATA          = $BA0B
.endif
.ifndef XMC_BANK
XMC_BANK          = $BA0C
.endif
.ifndef XMC_BANKS
XMC_BANKS         = $BA0D
.endif
.ifndef XMC_USEDL
XMC_USEDL         = $BA0E
.endif
.ifndef XMC_USEDH
XMC_USEDH         = $BA0F
.endif
.ifndef XMC_FREEL
XMC_FREEL         = $BA10
.endif
.ifndef XMC_FREEH
XMC_FREEH         = $BA11
.endif
.ifndef XMC_NAMELEN
XMC_NAMELEN       = $BA12
.endif
.ifndef XMC_HANDLE
XMC_HANDLE        = $BA13
.endif
.ifndef XMC_DIRCOUNTL
XMC_DIRCOUNTL     = $BA14
.endif
.ifndef XMC_DIRCOUNTH
XMC_DIRCOUNTH     = $BA15
.endif
.ifndef XMC_WINCTL
XMC_WINCTL        = $BA16
.endif
.ifndef XMC_W0AL
XMC_W0AL          = $BA18
.endif
.ifndef XMC_W0AM
XMC_W0AM          = $BA19
.endif
.ifndef XMC_W0AH
XMC_W0AH          = $BA1A
.endif
.ifndef XMC_W1AL
XMC_W1AL          = $BA1B
.endif
.ifndef XMC_W1AM
XMC_W1AM          = $BA1C
.endif
.ifndef XMC_W1AH
XMC_W1AH          = $BA1D
.endif
.ifndef XMC_W2AL
XMC_W2AL          = $BA1E
.endif
.ifndef XMC_W2AM
XMC_W2AM          = $BA1F
.endif
.ifndef XMC_W2AH
XMC_W2AH          = $BA20
.endif
.ifndef XMC_W3AL
XMC_W3AL          = $BA21
.endif
.ifndef XMC_W3AM
XMC_W3AM          = $BA22
.endif
.ifndef XMC_W3AH
XMC_W3AH          = $BA23
.endif
.ifndef XMC_NAME
XMC_NAME          = $BA24
.endif
.ifndef XMC_NAME_MAX
XMC_NAME_MAX      = 28
.endif

.ifndef WIN3_LO
WIN3_LO           = XMC_W3AL
.endif
.ifndef WIN3_MI
WIN3_MI           = XMC_W3AM
.endif
.ifndef WIN3_HI
WIN3_HI           = XMC_W3AH
.endif
.ifndef WIN3_BASE
WIN3_BASE         = $BF00
.endif

.ifndef XMC_CMD_GET
XMC_CMD_GET       = $01
.endif
.ifndef XMC_CMD_PUT
XMC_CMD_PUT       = $02
.endif
.ifndef XMC_CMD_STASH
XMC_CMD_STASH     = $03
.endif
.ifndef XMC_CMD_FETCH
XMC_CMD_FETCH     = $04
.endif
.ifndef XMC_CMD_FILL
XMC_CMD_FILL      = $05
.endif
.ifndef XMC_CMD_STATS
XMC_CMD_STATS     = $07
.endif
.ifndef XMC_CMD_RSTUS
XMC_CMD_RSTUS     = $08
.endif
.ifndef XMC_CMD_REL
XMC_CMD_REL       = $09
.endif
.ifndef XMC_CMD_ALLOC
XMC_CMD_ALLOC     = $0A
.endif
.ifndef XMC_CMD_NSTSH
XMC_CMD_NSTSH     = $0B
.endif
.ifndef XMC_CMD_NFETC
XMC_CMD_NFETC     = $0C
.endif
.ifndef XMC_CMD_NDEL
XMC_CMD_NDEL      = $0D
.endif
.ifndef XMC_CMD_NDIRO
XMC_CMD_NDIRO     = $0E
.endif
.ifndef XMC_CMD_NDIRR
XMC_CMD_NDIRR     = $0F
.endif

.ifndef XMC_OK
XMC_OK            = $02
.endif
.ifndef XMC_STATUS_ERROR
XMC_STATUS_ERROR  = $03
.endif
.ifndef XMC_ERR_RANGE
XMC_ERR_RANGE     = $01
.endif
.ifndef XMC_ERR_BADARGS
XMC_ERR_BADARGS   = $02
.endif
.ifndef XMC_ERR_NF
XMC_ERR_NF        = $03
.endif
.ifndef XMC_ERR_NOTFOUND
XMC_ERR_NOTFOUND  = $03
.endif
.ifndef XMC_ERR_NOSPACE
XMC_ERR_NOSPACE   = $04
.endif
.ifndef XMC_ERR_NAME
XMC_ERR_NAME      = $05
.endif
.ifndef XMC_ERR_EOD
XMC_ERR_EOD       = $06
.endif

; ---------------------------------------------------------------------
; Unified DMA controller.
; ---------------------------------------------------------------------
.ifndef DMA_CMD_REG
DMA_CMD_REG       = $BA63
.endif
.ifndef DMA_STATUS_REG
DMA_STATUS_REG    = $BA64
.endif
.ifndef DMA_ERRCODE_REG
DMA_ERRCODE_REG   = $BA65
.endif
.ifndef DMA_SRCSPACE
DMA_SRCSPACE      = $BA66
.endif
.ifndef DMA_DSTSPACE
DMA_DSTSPACE      = $BA67
.endif
.ifndef DMA_SRCL
DMA_SRCL          = $BA68
.endif
.ifndef DMA_SRCM
DMA_SRCM          = $BA69
.endif
.ifndef DMA_SRCH
DMA_SRCH          = $BA6A
.endif
.ifndef DMA_DSTL
DMA_DSTL          = $BA6B
.endif
.ifndef DMA_DSTM
DMA_DSTM          = $BA6C
.endif
.ifndef DMA_DSTH
DMA_DSTH          = $BA6D
.endif
.ifndef DMA_LENL
DMA_LENL          = $BA6E
.endif
.ifndef DMA_LENM
DMA_LENM          = $BA6F
.endif
.ifndef DMA_LENH
DMA_LENH          = $BA70
.endif
.ifndef DMA_MODE_REG
DMA_MODE_REG      = $BA71
.endif
.ifndef DMA_FILLVALUE
DMA_FILLVALUE     = $BA72
.endif
.ifndef DMA_CNTL
DMA_CNTL          = $BA73
.endif
.ifndef DMA_CNTM
DMA_CNTM          = $BA74
.endif
.ifndef DMA_CNTH
DMA_CNTH          = $BA75
.endif

.ifndef DMA_CMD
DMA_CMD           = DMA_CMD_REG
.endif
.ifndef DMA_STATUS
DMA_STATUS        = DMA_STATUS_REG
.endif
.ifndef DMA_ERRCODE
DMA_ERRCODE       = DMA_ERRCODE_REG
.endif
.ifndef DMA_MODE
DMA_MODE          = DMA_MODE_REG
.endif
.ifndef DMA_FILL
DMA_FILL          = DMA_FILLVALUE
.endif

.ifndef DMA_CMD_START
DMA_CMD_START     = $01
.endif
.ifndef DMA_STATUS_IDLE
DMA_STATUS_IDLE   = $00
.endif
.ifndef DMA_STATUS_BUSY
DMA_STATUS_BUSY   = $01
.endif
.ifndef DMA_STATUS_OK
DMA_STATUS_OK     = $02
.endif
.ifndef DMA_STATUS_ERROR
DMA_STATUS_ERROR  = $03
.endif
.ifndef DMA_BUSY
DMA_BUSY          = DMA_STATUS_BUSY
.endif
.ifndef DMA_OK
DMA_OK            = DMA_STATUS_OK
.endif
.ifndef DMA_ERR_BADCMD
DMA_ERR_BADCMD    = $01
.endif
.ifndef DMA_ERR_BADSPACE
DMA_ERR_BADSPACE  = $02
.endif
.ifndef DMA_ERR_RANGE
DMA_ERR_RANGE     = $03
.endif
.ifndef DMA_ERR_BADARGS
DMA_ERR_BADARGS   = $04
.endif
.ifndef DMA_ERR_WRITEPROT
DMA_ERR_WRITEPROT = $05
.endif
.ifndef DMA_MODE_FILL
DMA_MODE_FILL     = $01
.endif
.ifndef DMA_SPACE_CPU
DMA_SPACE_CPU     = $00
.endif
.ifndef DMA_SPACE_VGC_CHAR
DMA_SPACE_VGC_CHAR = $01
.endif
.ifndef DMA_SPACE_VGC_COLOR
DMA_SPACE_VGC_COLOR = $02
.endif
.ifndef DMA_SPACE_VGC_GFX
DMA_SPACE_VGC_GFX = $03
.endif
.ifndef DMA_SPACE_VGC_SPRITE
DMA_SPACE_VGC_SPRITE = $04
.endif
.ifndef DMA_SPACE_XRAM
DMA_SPACE_XRAM    = $05
.endif
.ifndef DMA_SPACE_VGC_TILE
DMA_SPACE_VGC_TILE = $06
.endif
.ifndef DMA_SPACE_VGC_TEXTATTR
DMA_SPACE_VGC_TEXTATTR = $07
.endif

; ---------------------------------------------------------------------
; Blitter controller.
; ---------------------------------------------------------------------
.ifndef BLT_CMD_REG
BLT_CMD_REG       = $BA83
.endif
.ifndef BLT_STATUS_REG
BLT_STATUS_REG    = $BA84
.endif
.ifndef BLT_ERRCODE_REG
BLT_ERRCODE_REG   = $BA85
.endif
.ifndef BLT_SRCSPACE
BLT_SRCSPACE      = $BA86
.endif
.ifndef BLT_DSTSPACE
BLT_DSTSPACE      = $BA87
.endif
.ifndef BLT_SRCL
BLT_SRCL          = $BA88
.endif
.ifndef BLT_SRCM
BLT_SRCM          = $BA89
.endif
.ifndef BLT_SRCH
BLT_SRCH          = $BA8A
.endif
.ifndef BLT_DSTL
BLT_DSTL          = $BA8B
.endif
.ifndef BLT_DSTM
BLT_DSTM          = $BA8C
.endif
.ifndef BLT_DSTH
BLT_DSTH          = $BA8D
.endif
.ifndef BLT_WIDTHL
BLT_WIDTHL        = $BA8E
.endif
.ifndef BLT_WIDTHH
BLT_WIDTHH        = $BA8F
.endif
.ifndef BLT_HEIGHTL
BLT_HEIGHTL       = $BA90
.endif
.ifndef BLT_HEIGHTH
BLT_HEIGHTH       = $BA91
.endif
.ifndef BLT_SRCSTRL
BLT_SRCSTRL       = $BA92
.endif
.ifndef BLT_SRCSTRH
BLT_SRCSTRH       = $BA93
.endif
.ifndef BLT_DSTSTRL
BLT_DSTSTRL       = $BA94
.endif
.ifndef BLT_DSTSTRH
BLT_DSTSTRH       = $BA95
.endif
.ifndef BLT_DSTRH
BLT_DSTRH         = BLT_DSTSTRH
.endif
.ifndef BLT_MODE_REG
BLT_MODE_REG      = $BA96
.endif
.ifndef BLT_FILLVALUE
BLT_FILLVALUE     = $BA97
.endif
.ifndef BLT_FILLVAL
BLT_FILLVAL       = BLT_FILLVALUE
.endif
.ifndef BLT_CKEY
BLT_CKEY          = $BA98
.endif
.ifndef BLT_CNTL
BLT_CNTL          = $BA99
.endif
.ifndef BLT_CNTM
BLT_CNTM          = $BA9A
.endif
.ifndef BLT_CNTH
BLT_CNTH          = $BA9B
.endif

.ifndef BLT_CMD
BLT_CMD           = BLT_CMD_REG
.endif
.ifndef BLT_STATUS
BLT_STATUS        = BLT_STATUS_REG
.endif
.ifndef BLT_ERRCODE
BLT_ERRCODE       = BLT_ERRCODE_REG
.endif
.ifndef BLT_MODE
BLT_MODE          = BLT_MODE_REG
.endif
.ifndef BLT_FILL
BLT_FILL          = BLT_FILLVALUE
.endif

.ifndef BLT_CMD_START
BLT_CMD_START     = $01
.endif
.ifndef BLT_STATUS_IDLE
BLT_STATUS_IDLE   = $00
.endif
.ifndef BLT_STATUS_BUSY
BLT_STATUS_BUSY   = $01
.endif
.ifndef BLT_STATUS_OK
BLT_STATUS_OK     = $02
.endif
.ifndef BLT_STATUS_ERROR
BLT_STATUS_ERROR  = $03
.endif
.ifndef BLT_BUSY
BLT_BUSY          = BLT_STATUS_BUSY
.endif
.ifndef BLT_OK
BLT_OK            = BLT_STATUS_OK
.endif
.ifndef BLT_ERR_BADCMD
BLT_ERR_BADCMD    = $01
.endif
.ifndef BLT_ERR_BADSPACE
BLT_ERR_BADSPACE  = $02
.endif
.ifndef BLT_ERR_RANGE
BLT_ERR_RANGE     = $03
.endif
.ifndef BLT_ERR_BADARGS
BLT_ERR_BADARGS   = $04
.endif
.ifndef BLT_ERR_WRITEPROT
BLT_ERR_WRITEPROT = $05
.endif
.ifndef BLT_MODE_FILL
BLT_MODE_FILL     = $01
.endif
.ifndef BLT_MODE_COLORKEY
BLT_MODE_COLORKEY = $02
.endif
.ifndef BLT_SPACE_CPU
BLT_SPACE_CPU     = DMA_SPACE_CPU
.endif
.ifndef BLT_SPACE_VGC_CHAR
BLT_SPACE_VGC_CHAR = DMA_SPACE_VGC_CHAR
.endif
.ifndef BLT_SPACE_VGC_COLOR
BLT_SPACE_VGC_COLOR = DMA_SPACE_VGC_COLOR
.endif
.ifndef BLT_SPACE_VGC_GFX
BLT_SPACE_VGC_GFX = DMA_SPACE_VGC_GFX
.endif
.ifndef BLT_SPACE_VGC_SPRITE
BLT_SPACE_VGC_SPRITE = DMA_SPACE_VGC_SPRITE
.endif
.ifndef BLT_SPACE_XRAM
BLT_SPACE_XRAM    = DMA_SPACE_XRAM
.endif
.ifndef BLT_SPACE_VGC_TILE
BLT_SPACE_VGC_TILE = DMA_SPACE_VGC_TILE
.endif
.ifndef BLT_SPACE_VGC_TEXTATTR
BLT_SPACE_VGC_TEXTATTR = DMA_SPACE_VGC_TEXTATTR
.endif

.endif

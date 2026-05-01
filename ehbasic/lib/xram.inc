; NovaVM XRAM runtime interface.
;
; Calling convention:
;   XRAM address: XRAM_ADDRL/M/H (NVR0L/NVR0H/NVR1L, flat 24-bit)
;   CPU address:  XRAM_RAML/H (NVR2)
;   Length:       XRAM_LENL/H (NVR3, 16-bit transfer length)
;   Byte value:   XRAM_DATA (NVR1H)
;   Filename:     XRAM_NAMELEN (NVR4L), XRAM_NAMEPTR_L/H (NVR4H/NVR5L)
;
; Routines return A=0 on success and A=1 on error. They also update the
; XMC-compatible status registers so BASIC and direct assembly callers can
; share the same error path.

.ifndef XRAM_INC_INCLUDED
XRAM_INC_INCLUDED = 1

.ifndef NVR0L
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
.endif

.ifndef XMC_STATUS
XMC_STATUS      = $BA01
.endif
.ifndef XMC_ERRCODE
XMC_ERRCODE     = $BA02
.endif
.ifndef XMC_CMD
XMC_CMD         = $BA00
.endif
.ifndef XMC_XAL
XMC_XAL         = $BA04
.endif
.ifndef XMC_XAM
XMC_XAM         = $BA05
.endif
.ifndef XMC_XAH
XMC_XAH         = $BA06
.endif
.ifndef XMC_RAML
XMC_RAML        = $BA07
.endif
.ifndef XMC_RAMH
XMC_RAMH        = $BA08
.endif
.ifndef XMC_LENL
XMC_LENL        = $BA09
.endif
.ifndef XMC_LENH
XMC_LENH        = $BA0A
.endif
.ifndef XMC_DATA
XMC_DATA        = $BA0B
.endif
.ifndef XMC_BANK
XMC_BANK        = $BA0C
.endif
.ifndef XMC_BANKS
XMC_BANKS       = $BA0D
.endif
.ifndef XMC_USEDL
XMC_USEDL       = $BA0E
.endif
.ifndef XMC_USEDH
XMC_USEDH       = $BA0F
.endif
.ifndef XMC_FREEL
XMC_FREEL       = $BA10
.endif
.ifndef XMC_FREEH
XMC_FREEH       = $BA11
.endif
.ifndef XMC_NAMELEN
XMC_NAMELEN     = $BA12
.endif
.ifndef XMC_HANDLE
XMC_HANDLE      = $BA13
.endif
.ifndef XMC_DIRCOUNTL
XMC_DIRCOUNTL   = $BA14
.endif
.ifndef XMC_DIRCOUNTH
XMC_DIRCOUNTH   = $BA15
.endif
.ifndef XMC_WINCTL
XMC_WINCTL      = $BA16
.endif
.ifndef XMC_NAME
XMC_NAME        = $BA24
.endif
.ifndef WIN3_LO
WIN3_LO         = $BA21
.endif
.ifndef WIN3_MI
WIN3_MI         = $BA22
.endif
.ifndef WIN3_HI
WIN3_HI         = $BA23
.endif
.ifndef WIN3_BASE
WIN3_BASE       = $BF00
.endif

.ifndef DMA_CMD_REG
DMA_CMD_REG     = $BA63
.endif
.ifndef DMA_STATUS_REG
DMA_STATUS_REG  = $BA64
.endif
.ifndef DMA_ERRCODE_REG
DMA_ERRCODE_REG = $BA65
.endif
.ifndef DMA_SRCSPACE
DMA_SRCSPACE    = $BA66
.endif
.ifndef DMA_DSTSPACE
DMA_DSTSPACE    = $BA67
.endif
.ifndef DMA_SRCL
DMA_SRCL        = $BA68
.endif
.ifndef DMA_SRCM
DMA_SRCM        = $BA69
.endif
.ifndef DMA_SRCH
DMA_SRCH        = $BA6A
.endif
.ifndef DMA_DSTL
DMA_DSTL        = $BA6B
.endif
.ifndef DMA_DSTM
DMA_DSTM        = $BA6C
.endif
.ifndef DMA_DSTH
DMA_DSTH        = $BA6D
.endif
.ifndef DMA_LENL
DMA_LENL        = $BA6E
.endif
.ifndef DMA_LENM
DMA_LENM        = $BA6F
.endif
.ifndef DMA_LENH
DMA_LENH        = $BA70
.endif
.ifndef DMA_MODE_REG
DMA_MODE_REG    = $BA71
.endif
.ifndef DMA_FILLVALUE
DMA_FILLVALUE   = $BA72
.endif

.ifndef FIO_CMD
FIO_CMD         = $B9A0
.endif
.ifndef FIO_STATUS
FIO_STATUS      = $B9A1
.endif
.ifndef FIO_ERRCODE
FIO_ERRCODE     = $B9A2
.endif
.ifndef FIO_NAMELEN
FIO_NAMELEN     = $B9A3
.endif
.ifndef FIO_SIZEL
FIO_SIZEL       = $B9A8
.endif
.ifndef FIO_SIZEH
FIO_SIZEH       = $B9A9
.endif
.ifndef FIO_GSPACE
FIO_GSPACE      = $B9AA
.endif
.ifndef FIO_GADDRL
FIO_GADDRL      = $B9AB
.endif
.ifndef FIO_GADDRH
FIO_GADDRH      = $B9AC
.endif
.ifndef FIO_GLENL
FIO_GLENL       = $B9AD
.endif
.ifndef FIO_GLENH
FIO_GLENH       = $B9AE
.endif
.ifndef FIO_NAME
FIO_NAME        = $B9B0
.endif

.ifndef XMC_OK
XMC_OK          = $02
.endif
.ifndef XMC_STATUS_ERROR
XMC_STATUS_ERROR = $03
.endif
.ifndef XMC_CMD_GET
XMC_CMD_GET     = $01
.endif
.ifndef XMC_CMD_PUT
XMC_CMD_PUT     = $02
.endif
.ifndef XMC_CMD_STASH
XMC_CMD_STASH   = $03
.endif
.ifndef XMC_CMD_FETCH
XMC_CMD_FETCH   = $04
.endif
.ifndef XMC_CMD_FILL
XMC_CMD_FILL    = $05
.endif
.ifndef XMC_CMD_STATS
XMC_CMD_STATS   = $07
.endif
.ifndef XMC_CMD_RSTUS
XMC_CMD_RSTUS   = $08
.endif
.ifndef XMC_CMD_REL
XMC_CMD_REL     = $09
.endif
.ifndef XMC_CMD_ALLOC
XMC_CMD_ALLOC   = $0A
.endif
.ifndef XMC_CMD_NSTSH
XMC_CMD_NSTSH   = $0B
.endif
.ifndef XMC_CMD_NFETC
XMC_CMD_NFETC   = $0C
.endif
.ifndef XMC_CMD_NDEL
XMC_CMD_NDEL    = $0D
.endif
.ifndef XMC_CMD_NDIRO
XMC_CMD_NDIRO   = $0E
.endif
.ifndef XMC_CMD_NDIRR
XMC_CMD_NDIRR   = $0F
.endif
.ifndef DMA_CMD_START
DMA_CMD_START   = $01
.endif
.ifndef DMA_STATUS_BUSY
DMA_STATUS_BUSY = $01
.endif
.ifndef DMA_STATUS_OK
DMA_STATUS_OK   = $02
.endif
.ifndef DMA_STATUS_ERROR
DMA_STATUS_ERROR = $03
.endif
.ifndef DMA_MODE_FILL
DMA_MODE_FILL   = $01
.endif
.ifndef DMA_SPACE_CPU
DMA_SPACE_CPU   = $00
.endif
.ifndef DMA_SPACE_XRAM
DMA_SPACE_XRAM  = $05
.endif
.ifndef DMA_ERR_RANGE
DMA_ERR_RANGE   = $03
.endif
.ifndef FIO_OK
FIO_OK          = $02
.endif
.ifndef FIO_STATUS_ERROR
FIO_STATUS_ERROR = $03
.endif
.ifndef FIO_ERR_NOTFOUND
FIO_ERR_NOTFOUND = $01
.endif
.ifndef FIO_ERR_IO
FIO_ERR_IO      = $02
.endif
.ifndef FIO_CMD_XLOAD
FIO_CMD_XLOAD   = $18
.endif
.ifndef FIO_CMD_XSAVE
FIO_CMD_XSAVE   = $19
.endif

; Assembly-facing pseudo-register ABI. BASIC/XMC callers enter through the
; xram_xmc_* wrappers, which copy XMC MMIO arguments into this layout.
XRAM_ADDRL      = NVR0L
XRAM_ADDRM      = NVR0H
XRAM_ADDRH      = NVR1L
XRAM_DATA       = NVR1H
XRAM_RAML       = NVR2L
XRAM_RAMH       = NVR2H
XRAM_LENL       = NVR3L
XRAM_LENH       = NVR3H
XRAM_NAMELEN    = NVR4L
XRAM_NAMEPTR_L  = NVR4H
XRAM_NAMEPTR_H  = NVR5L
XRAM_WIN3_BASE  = WIN3_BASE

XRAM_CAPACITY_HIGH = $08        ; 512KB == flat address $080000
XRAM_WIN3_ENABLE = $08
XRAM_XMC_ERR_RANGE = $01
XRAM_XMC_ERR_BADARGS = $02
XRAM_XMC_ERR_NOTFOUND = $03

.endif

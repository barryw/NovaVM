; XMC helper macros for ca65-style assembly

XMC_CMD      = $BA00
XMC_STATUS   = $BA01
XMC_ERRCODE  = $BA02
XMC_XAL      = $BA04
XMC_XAM      = $BA05
XMC_XAH      = $BA06
XMC_RAML     = $BA07
XMC_RAMH     = $BA08
XMC_LENL     = $BA09
XMC_LENH     = $BA0A
XMC_DATA     = $BA0B

XMC_CMD_GET   = $01
XMC_CMD_PUT   = $02
XMC_CMD_STASH = $03
XMC_CMD_FETCH = $04
XMC_CMD_FILL  = $05
XMC_CMD_ALLOC = $0A

.macro XMC_SET_ADDR24 low, mid, high
  lda #low
  sta XMC_XAL
  lda #mid
  sta XMC_XAM
  lda #high
  sta XMC_XAH
.endmacro

.macro XMC_SET_RAM16 low, high
  lda #low
  sta XMC_RAML
  lda #high
  sta XMC_RAMH
.endmacro

.macro XMC_SET_LEN16 low, high
  lda #low
  sta XMC_LENL
  lda #high
  sta XMC_LENH
.endmacro

.macro XMC_RUN cmd
  lda #cmd
  sta XMC_CMD
.endmacro

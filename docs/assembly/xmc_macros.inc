; XMC helper macros for ca65-style assembly

.include "nova.inc"

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

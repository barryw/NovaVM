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

.include "nova.inc"

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

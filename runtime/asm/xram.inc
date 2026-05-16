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
; @label XRAM.ADDRL
; @kind zp
; @symbol XRAM_ADDRL
; @summary Low byte of flat 24-bit XRAM address.
XRAM_ADDRL      = NVR0L
; @label XRAM.ADDRM
; @kind zp
; @symbol XRAM_ADDRM
; @summary Middle byte of flat 24-bit XRAM address.
XRAM_ADDRM      = NVR0H
; @label XRAM.ADDRH
; @kind zp
; @symbol XRAM_ADDRH
; @summary High byte of flat 24-bit XRAM address.
XRAM_ADDRH      = NVR1L
; @label XRAM.DATA
; @kind zp
; @symbol XRAM_DATA
; @summary Byte value used by XRAM read, write, and fill routines.
XRAM_DATA       = NVR1H
; @label XRAM.RAML
; @kind zp
; @symbol XRAM_RAML
; @summary Low byte of CPU RAM address for XRAM copy routines.
XRAM_RAML       = NVR2L
; @label XRAM.RAMH
; @kind zp
; @symbol XRAM_RAMH
; @summary High byte of CPU RAM address for XRAM copy routines.
XRAM_RAMH       = NVR2H
; @label XRAM.LENL
; @kind zp
; @symbol XRAM_LENL
; @summary Low byte of XRAM transfer length.
XRAM_LENL       = NVR3L
; @label XRAM.LENH
; @kind zp
; @symbol XRAM_LENH
; @summary High byte of XRAM transfer length.
XRAM_LENH       = NVR3H
; @label XRAM.NAMELEN
; @kind zp
; @symbol XRAM_NAMELEN
; @summary Filename length for XRAM file streaming routines.
XRAM_NAMELEN    = NVR4L
; @label XRAM.NAMEPTR_L
; @kind zp
; @symbol XRAM_NAMEPTR_L
; @summary Low byte of filename pointer for XRAM file streaming routines.
XRAM_NAMEPTR_L  = NVR4H
; @label XRAM.NAMEPTR_H
; @kind zp
; @symbol XRAM_NAMEPTR_H
; @summary High byte of filename pointer for XRAM file streaming routines.
XRAM_NAMEPTR_H  = NVR5L
XRAM_WIN3_BASE  = WIN3_BASE

XRAM_CAPACITY_HIGH = $08        ; 512KB == flat address $080000
XRAM_WIN3_ENABLE = $08
XRAM_XMC_ERR_RANGE = $01
XRAM_XMC_ERR_BADARGS = $02
XRAM_XMC_ERR_NOTFOUND = $03

; Fixed XRAM layout. Nova is single-tasking, so shared runtimes use
; role-sized workspaces instead of a hardware malloc/free table.
XRAM_USER_HEAP_PAGES = 1024     ; $000000-$03FFFF for BASIC/XMC blocks.

XRAM_NOVAZ_DYNAMIC_L = $00      ; $040000-$04FFFF, max 64KB dynamic story.
XRAM_NOVAZ_DYNAMIC_M = $00
XRAM_NOVAZ_DYNAMIC_H = $04
XRAM_NOVAZ_CACHE_L   = $00      ; $050000-$053FFF, 16KB static page cache.
XRAM_NOVAZ_CACHE_M   = $00
XRAM_NOVAZ_CACHE_H   = $05
XRAM_NOVAZ_SAVE_L    = $00      ; $054000+, transient save/restore staging.
XRAM_NOVAZ_SAVE_M    = $40
XRAM_NOVAZ_SAVE_H    = $05

XRAM_NVG_STAGE_L     = $00      ; $070000-$07F9FF, 64,000-byte NVG stage.
XRAM_NVG_STAGE_M     = $00
XRAM_NVG_STAGE_H     = $07

.endif

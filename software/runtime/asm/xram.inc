; =====================================================================
;  Nova NDK — xram.inc
;
;  NovaVM XRAM runtime interface: read/write/copy/fill and file streaming
;  over the flat 24-bit XRAM address space, with XMC-compatible status
;  registers for a shared BASIC/assembly error path.
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

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
XRAM_WIN0_BASE  = $BC00
XRAM_WIN1_BASE  = $BD00
XRAM_WIN2_BASE  = $BE00
XRAM_WIN3_BASE  = WIN3_BASE

XRAM_CAPACITY_HIGH = $08        ; 512KB == flat address $080000
XRAM_WIN0_ENABLE = $01
XRAM_WIN1_ENABLE = $02
XRAM_WIN2_ENABLE = $04
XRAM_WIN3_ENABLE = $08
XRAM_XMC_ERR_RANGE = $01
XRAM_XMC_ERR_BADARGS = $02
XRAM_XMC_ERR_NOTFOUND = $03

; XMC-managed application heap. Standalone runtimes and tools that need
; transient XRAM should request blocks through xmc_alloc_block / MEM_ALLOC
; instead of claiming fixed addresses.
XRAM_USER_HEAP_PAGES = 1024     ; $000000-$03FFFF, 256-byte pages.

XRAM_NVG_STAGE_L     = $00      ; $070000-$07F9FF, 64,000-byte NVG stage.
XRAM_NVG_STAGE_M     = $00
XRAM_NVG_STAGE_H     = $07

.global xram_set_ok
.global xram_set_error
.global xram_addr_is_in_range
.global xram_map_window3
.global xram_read8
.global xram_write8
.global xram_copy_from_ram
.global xram_copy_to_ram
.global xram_fill
.global xram_xload
.global xram_xsave
.global xram_wait_dma
.global xram_xmc_read8
.global xram_xmc_write8
.global xram_xmc_copy_from_ram
.global xram_xmc_copy_to_ram
.global xram_xmc_fill

.endif

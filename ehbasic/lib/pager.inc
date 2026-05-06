; Shared Nova file pager declarations.
;
; The pager layer is intentionally above the backing memory target: callers
; choose a file offset, destination target/address, and byte count. The host
; streams that slice through the existing FIO transport.

.ifndef PAGER_INC_INCLUDED
PAGER_INC_INCLUDED = 1

.include "fio.inc"

; Destination address and length reuse the XRAM/FIO streaming pseudo-register ABI.
; @label PAGER.ADDRL
; @kind zp
; @symbol PAGER_ADDRL
; @summary Low byte of destination address.
PAGER_ADDRL      = FIO_ARG_ADDRL
; @label PAGER.ADDRM
; @kind zp
; @symbol PAGER_ADDRM
; @summary Middle byte of destination address or VGC space selector.
PAGER_ADDRM      = FIO_ARG_ADDRM
; @label PAGER.ADDRH
; @kind zp
; @symbol PAGER_ADDRH
; @summary High byte of destination address for 24-bit targets.
PAGER_ADDRH      = FIO_ARG_ADDRH
; @label PAGER.LENL
; @kind zp
; @symbol PAGER_LENL
; @summary Low byte of transfer length.
PAGER_LENL       = FIO_ARG_LENL
; @label PAGER.LENH
; @kind zp
; @symbol PAGER_LENH
; @summary High byte of transfer length.
PAGER_LENH       = FIO_ARG_LENH
; @label PAGER.NAMELEN
; @kind zp
; @symbol PAGER_NAMELEN
; @summary Filename length for the paged file.
PAGER_NAMELEN    = FIO_ARG_NAMELEN
; @label PAGER.NAMEPTR_L
; @kind zp
; @symbol PAGER_NAMEPTR_L
; @summary Low byte of paged filename pointer.
PAGER_NAMEPTR_L  = FIO_ARG_NAMEPTR_L
; @label PAGER.NAMEPTR_H
; @kind zp
; @symbol PAGER_NAMEPTR_H
; @summary High byte of paged filename pointer.
PAGER_NAMEPTR_H  = FIO_ARG_NAMEPTR_H

; 24-bit source offset inside the file.
; @label PAGER.FILEL
; @kind zp
; @symbol PAGER_FILEL
; @summary Low byte of the source file offset.
PAGER_FILEL      = NVR5H
; @label PAGER.FILEM
; @kind zp
; @symbol PAGER_FILEM
; @summary Middle byte of the source file offset.
PAGER_FILEM      = NVR6L
; @label PAGER.FILEH
; @kind zp
; @symbol PAGER_FILEH
; @summary High byte of the source file offset.
PAGER_FILEH      = NVR6H

; Destination target for PAGER.LOAD_FILE_PAGE.
; @label PAGER.TARGET
; @kind zp
; @symbol PAGER_TARGET
; @summary Destination target selector: 0=XRAM, 1=CPU RAM, 2=VGC memory.
PAGER_TARGET     = NVR7L

; @label PAGER.TARGET_XRAM
; @kind const
; @symbol PAGER_TARGET_XRAM
; @summary PAGER.TARGET value for flat XRAM destinations.
PAGER_TARGET_XRAM = $00
; @label PAGER.TARGET_RAM
; @kind const
; @symbol PAGER_TARGET_RAM
; @summary PAGER.TARGET value for CPU RAM destinations.
PAGER_TARGET_RAM  = $01
; @label PAGER.TARGET_VGC
; @kind const
; @symbol PAGER_TARGET_VGC
; @summary PAGER.TARGET value for VGC memory destinations.
PAGER_TARGET_VGC  = $02

.endif

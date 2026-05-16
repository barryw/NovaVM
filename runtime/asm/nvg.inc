; Shared Nova NVG image loader declarations.
;
; NVG files are decoded by NovaHost and streamed directly into the graphics
; bitmap plane. Callers provide the filename through the shared FIO name
; arguments.

.ifndef NVG_INC_INCLUDED
NVG_INC_INCLUDED = 1

.include "fio.inc"

; @label NVG.ADDRL
; @kind u8
; @symbol NVG_ADDRL
; @summary Low byte of the graphics bitmap destination offset for NVG.LOAD_AT.
NVG_ADDRL = FIO_GADDRL
; @label NVG.ADDRH
; @kind u8
; @symbol NVG_ADDRH
; @summary High byte of the graphics bitmap destination offset for NVG.LOAD_AT.
NVG_ADDRH = FIO_GADDRH
; @label NVG.NAMELEN
; @kind zp
; @symbol NVG_NAMELEN
; @summary Filename length argument for NVG.LOAD_NAMED.
NVG_NAMELEN = FIO_ARG_NAMELEN
; @label NVG.NAMEPTR_L
; @kind zp
; @symbol NVG_NAMEPTR_L
; @summary Low byte of filename pointer argument for NVG.LOAD_NAMED.
NVG_NAMEPTR_L = FIO_ARG_NAMEPTR_L
; @label NVG.NAMEPTR_H
; @kind zp
; @symbol NVG_NAMEPTR_H
; @summary High byte of filename pointer argument for NVG.LOAD_NAMED.
NVG_NAMEPTR_H = FIO_ARG_NAMEPTR_H
.endif

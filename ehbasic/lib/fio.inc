; Shared Nova File I/O helper declarations.
;
; Calling convention for pointer-based filename helpers:
;   FIO_ARG_NAMELEN      filename length, 1..63
;   FIO_ARG_NAMEPTR_L/H  CPU pointer to filename bytes
;
; Calling convention for XRAM streaming setup:
;   FIO_ARG_ADDRL/M/H    flat 24-bit XRAM address
;   FIO_ARG_LENL/H       transfer length; zero means full file for XLOAD
;
; Routines return A=0 on success and A=1 on error. Detailed status remains
; in the FIO_STATUS/FIO_ERRCODE hardware registers.

.ifndef FIO_INC_INCLUDED
FIO_INC_INCLUDED = 1

.include "nova.inc"

FIO_RESULT_OK    = $00
FIO_RESULT_ERROR = $01
FIO_NAME_LIMIT   = $3F

; Match the XRAM streaming ABI so xram_xload/xram_xsave can call directly.
; @label FIO.ARG_ADDRL
; @kind zp
; @symbol FIO_ARG_ADDRL
; @summary Low byte of XRAM address argument for FIO.PREPARE_XRAM_TRANSFER.
FIO_ARG_ADDRL     = NVR0L
; @label FIO.ARG_ADDRM
; @kind zp
; @symbol FIO_ARG_ADDRM
; @summary Middle byte of XRAM address argument for FIO.PREPARE_XRAM_TRANSFER.
FIO_ARG_ADDRM     = NVR0H
; @label FIO.ARG_ADDRH
; @kind zp
; @symbol FIO_ARG_ADDRH
; @summary High byte of XRAM address argument for FIO.PREPARE_XRAM_TRANSFER.
FIO_ARG_ADDRH     = NVR1L
; @label FIO.ARG_LENL
; @kind zp
; @symbol FIO_ARG_LENL
; @summary Low byte of transfer length for FIO.PREPARE_XRAM_TRANSFER.
FIO_ARG_LENL      = NVR3L
; @label FIO.ARG_LENH
; @kind zp
; @symbol FIO_ARG_LENH
; @summary High byte of transfer length for FIO.PREPARE_XRAM_TRANSFER.
FIO_ARG_LENH      = NVR3H
; @label FIO.ARG_NAMELEN
; @kind zp
; @symbol FIO_ARG_NAMELEN
; @summary Filename length for FIO.COPY_NAME.
FIO_ARG_NAMELEN   = NVR4L
; @label FIO.ARG_NAMEPTR_L
; @kind zp
; @symbol FIO_ARG_NAMEPTR_L
; @summary Low byte of filename pointer for FIO.COPY_NAME.
FIO_ARG_NAMEPTR_L = NVR4H
; @label FIO.ARG_NAMEPTR_H
; @kind zp
; @symbol FIO_ARG_NAMEPTR_H
; @summary High byte of filename pointer for FIO.COPY_NAME.
FIO_ARG_NAMEPTR_H = NVR5L

.endif

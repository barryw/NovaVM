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
FIO_ARG_ADDRL     = NVR0L
FIO_ARG_ADDRM     = NVR0H
FIO_ARG_ADDRH     = NVR1L
FIO_ARG_LENL      = NVR3L
FIO_ARG_LENH      = NVR3H
FIO_ARG_NAMELEN   = NVR4L
FIO_ARG_NAMEPTR_L = NVR4H
FIO_ARG_NAMEPTR_H = NVR5L

.endif

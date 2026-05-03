; Shared Nova NIC runtime interface.
;
; BASIC handles string allocation/parsing.  Assembly callers can use these
; routines by loading the NIC MMIO registers directly.
;
; Connect:
;   NIC_SLOT      slot number
;   NIC_NAMEBUF   zero-terminated host name
;   NIC_RPORTL/H  remote port
;
; Listen:
;   NIC_SLOT      slot number
;   NIC_LPORTL/H  local port
;
; Send:
;   NIC_SLOT      slot number
;   NIC_DMAL/H    CPU pointer to bytes
;   NIC_DMALEN    byte count
;
; Receive:
;   NIC_SLOT      slot number
;   NIC_DMAL/H    CPU destination pointer
;   NIC_MSGLEN    byte count returned by hardware

.ifndef NIC_INC_INCLUDED
NIC_INC_INCLUDED = 1

.include "nova.inc"

NIC_RESULT_OK    = $00
NIC_RESULT_ERROR = $01
NIC_NAME_LIMIT   = $1F

; @label NIC.ARG_NAMELEN
; @kind zp
; @symbol NIC_ARG_NAMELEN
; @summary Hostname length for NIC.COPY_NAME.
NIC_ARG_NAMELEN   = NVR0L
; @label NIC.ARG_NAMEPTR_L
; @kind zp
; @symbol NIC_ARG_NAMEPTR_L
; @summary Low byte of hostname pointer for NIC.COPY_NAME.
NIC_ARG_NAMEPTR_L = NVR0H
; @label NIC.ARG_NAMEPTR_H
; @kind zp
; @symbol NIC_ARG_NAMEPTR_H
; @summary High byte of hostname pointer for NIC.COPY_NAME.
NIC_ARG_NAMEPTR_H = NVR1L

.endif

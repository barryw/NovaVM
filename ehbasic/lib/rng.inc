; Shared Nova random-number service declarations.
;
; The host backs this through NovaHost/ESP hardware entropy on real hardware
; and the emulator's cryptographic RNG in Avalonia. Routines return A=0 on
; success and A=1 on error. Returned bytes remain in RNG_VALUE0..3.

.ifndef RNG_INC_INCLUDED
RNG_INC_INCLUDED = 1

.include "fio.inc"

; @label RNG.VALUE0
; @kind u8
; @symbol RNG_VALUE0
; @summary Low byte of the most recent RNG.GET32 result.
RNG_VALUE0 = FIO_RNG0
; @label RNG.VALUE1
; @kind u8
; @symbol RNG_VALUE1
; @summary Second byte of the most recent RNG.GET32 result.
RNG_VALUE1 = FIO_RNG1
; @label RNG.VALUE2
; @kind u8
; @symbol RNG_VALUE2
; @summary Third byte of the most recent RNG.GET32 result.
RNG_VALUE2 = FIO_RNG2
; @label RNG.VALUE3
; @kind u8
; @symbol RNG_VALUE3
; @summary High byte of the most recent RNG.GET32 result.
RNG_VALUE3 = FIO_RNG3

.endif

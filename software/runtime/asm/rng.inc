; =====================================================================
;  Nova NDK — rng.inc
;
;  Shared Nova random-number service declarations. Backed by hardware entropy
;  on real hardware and the emulator's cryptographic RNG in Avalonia; routines
;  return A=0 on success and A=1 on error, with bytes in RNG_VALUE0..3.
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

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

.global rng_get8
.global rng_get16
.global rng_get32

.endif

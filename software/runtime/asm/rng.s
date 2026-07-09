; =====================================================================
;  Nova NDK — rng.s
;
;  Random-number service: fetches host-backed random bits (8/16/32) into
;  RNG.VALUE0..3 via the FileIoController RNG command.
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

.include "rng.inc"

.ifndef RNG_IMPLEMENTATION_INCLUDED
RNG_IMPLEMENTATION_INCLUDED = 1

      .segment "CODE"

      .export rng_get8
      .export rng_get16
      .export rng_get32

; Fetch 32 host-backed random bits.
; @label RNG.GET32
; @kind routine
; @symbol rng_get32
; @summary Fill RNG.VALUE0..3 with 32 random bits from the Nova host.
; @out A: 0 on success, 1 on error.
; @out RNG_VALUE0 RNG_VALUE1 RNG_VALUE2 RNG_VALUE3
rng_get32:
      LDA   #FIO_CMD_RNG
      JMP   fio_exec

; Fetch at least 16 host-backed random bits.
; @label RNG.GET16
; @kind routine
; @symbol rng_get16
; @summary Fill RNG.VALUE0..1 with 16 random bits from the Nova host.
; @out A: 0 on success, 1 on error.
; @out RNG_VALUE0 RNG_VALUE1
rng_get16:
      BRA   rng_get32

; Fetch at least 8 host-backed random bits.
; @label RNG.GET8
; @kind routine
; @symbol rng_get8
; @summary Fill RNG.VALUE0 with an 8-bit random value from the Nova host.
; @out A: 0 on success, 1 on error.
; @out RNG_VALUE0
rng_get8:
      BRA   rng_get32

.endif

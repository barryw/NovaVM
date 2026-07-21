; Precompiled NovaRandom unit implementation declarations.

.ifndef NOVA_RANDOM_UNIT_INCLUDED
NOVA_RANDOM_UNIT_INCLUDED = 1

; Keep only this adapter's canonical dependencies in NAS's transient symbol
; table. The values remain aliases of nova.inc; RANDOM.S still includes the
; unmodified RNG and FIO implementations.
.include "nova.inc"
FIO_INC_INCLUDED = 1
RNG_INC_INCLUDED = 1

FIO_RESULT_OK = $00
FIO_RESULT_ERROR = $01
RNG_VALUE0 = FIO_RNG0

.global fio_check
.global fio_exec
.global rng_get8

.global randombyte

.endif

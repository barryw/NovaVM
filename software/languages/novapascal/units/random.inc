; Precompiled NovaRandom unit implementation declarations.

.ifndef NOVA_RANDOM_UNIT_INCLUDED
NOVA_RANDOM_UNIT_INCLUDED = 1

; Include this adapter's canonical dependencies rather than asserting their
; include guards: another unit in the same program (NovaAudio, NovaFio, ...)
; may have pulled them already, and a second assignment of a guard or of an
; alias such as RNG_VALUE0 is a symbol redefinition.
.include "nova.inc"
.include "fio.inc"
.include "rng.inc"

.global fio_check
.global fio_exec
.global rng_get8

.global randombyte

.endif

; NovaRandom Pascal unit adapter over the canonical RNG NDK.

.include "random.inc"

.ifndef NOVA_RANDOM_UNIT_IMPLEMENTATION_INCLUDED
NOVA_RANDOM_UNIT_IMPLEMENTATION_INCLUDED = 1

      .segment "CODE"

      .export randombyte

randombyte:
      JSR   rng_get8
      LDA   RNG_VALUE0
      RTS

.include "rng.s"
.include "fio.s"

.endif

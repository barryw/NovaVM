; NovaInput Pascal unit adapter over Nova's canonical VGC input register.

.include "input.inc"

.ifndef NOVA_INPUT_UNIT_IMPLEMENTATION_INCLUDED
NOVA_INPUT_UNIT_IMPLEMENTATION_INCLUDED = 1

      .segment "CODE"

      .export pollkey

pollkey:
      LDA   VGC_CHARIN
      RTS

.endif

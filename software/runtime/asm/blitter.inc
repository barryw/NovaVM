; =====================================================================
;  Nova NDK — blitter.inc
;
;  Blitter helper ABI: block copy, fill, and gfx4 unpack (blocking and
;  async start/wait variants).
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

.ifndef BLITTER_INC_INCLUDED
BLITTER_INC_INCLUDED = 1

.include "nova.inc"

BLITTER_RESULT_OK    = $00
BLITTER_RESULT_ERROR = $01

.global blitter_copy
.global blitter_fill
.global blitter_gfx4_unpack
.global blitter_start_copy
.global blitter_start_fill
.global blitter_start_gfx4_unpack
.global blitter_wait

.endif

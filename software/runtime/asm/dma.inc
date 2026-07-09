; =====================================================================
;  Nova NDK — dma.inc
;
;  DMA helper ABI: memory copy and fill (blocking and async start/wait
;  variants).
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

.ifndef DMA_INC_INCLUDED
DMA_INC_INCLUDED = 1

.include "nova.inc"

DMA_RESULT_OK    = $00
DMA_RESULT_ERROR = $01

.global dma_copy
.global dma_fill
.global dma_start_copy
.global dma_start_fill
.global dma_wait

.endif

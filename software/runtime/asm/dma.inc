; Shared Nova DMA helper declarations.

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

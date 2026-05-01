; Shared Nova DMA helper routines.
;
; Callers load the DMA hardware registers, then call dma_start_copy or
; dma_start_fill. Routines return A=0 on success and A=1 on error; detailed
; errors remain in DMA_ERRCODE_REG.

.include "dma.inc"

.ifndef DMA_IMPLEMENTATION_INCLUDED
DMA_IMPLEMENTATION_INCLUDED = 1

      .segment "CODE"

      .export dma_start_copy
      .export dma_start_fill
      .export dma_wait

dma_start_copy:
      STZ   DMA_MODE_REG
      LDA   #DMA_CMD_START
      STA   DMA_CMD_REG
      JMP   dma_wait

dma_start_fill:
      LDA   #DMA_MODE_FILL
      STA   DMA_MODE_REG
      LDA   #DMA_CMD_START
      STA   DMA_CMD_REG
      JMP   dma_wait

dma_wait:
@loop:
      LDA   DMA_STATUS_REG
      CMP   #DMA_STATUS_BUSY
      BEQ   @loop
      CMP   #DMA_STATUS_OK
      BEQ   @ok
      LDA   #DMA_RESULT_ERROR
      RTS
@ok:
      LDA   #DMA_RESULT_OK
      RTS

.endif

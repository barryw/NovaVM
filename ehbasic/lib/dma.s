; Shared Nova DMA helper routines.
;
; Callers load the DMA hardware registers, then call dma_start_copy or
; dma_start_fill. Routines return A=0 on success and A=1 on error; detailed
; errors remain in DMA_ERRCODE_REG.

.include "dma.inc"

.ifndef DMA_IMPLEMENTATION_INCLUDED
DMA_IMPLEMENTATION_INCLUDED = 1

      .segment "CODE"

      .export dma_copy
      .export dma_fill
      .export dma_start_copy
      .export dma_start_fill
      .export dma_wait

; @label DMA.COPY
; @kind routine
; @symbol dma_copy
; @summary Start a DMA copy using the DMA controller registers.
; @requires DMA_SRCSPACE DMA_DSTSPACE DMA_SRCL DMA_SRCM DMA_SRCH DMA_DSTL DMA_DSTM DMA_DSTH DMA_LENL DMA_LENM DMA_LENH
; @out A: 0 on success, 1 on error.
dma_copy:
      JMP   dma_start_copy

; @label DMA.FILL
; @kind routine
; @symbol dma_fill
; @summary Start a DMA fill using the destination and length registers.
; @requires DMA_DSTSPACE DMA_DSTL DMA_DSTM DMA_DSTH DMA_LENL DMA_LENM DMA_LENH DMA_FILLVALUE
; @out A: 0 on success, 1 on error.
dma_fill:
      STZ   DMA_SRCSPACE
      STZ   DMA_SRCL
      STZ   DMA_SRCM
      STZ   DMA_SRCH
      JMP   dma_start_fill

; @label DMA.START_COPY
; @kind routine
; @symbol dma_start_copy
; @summary Low-level DMA copy start. Call DMA.COPY for the stable public wrapper.
; @out A: 0 on success, 1 on error.
dma_start_copy:
      STZ   DMA_MODE_REG
      LDA   #DMA_CMD_START
      STA   DMA_CMD_REG
      JMP   dma_wait

; @label DMA.START_FILL
; @kind routine
; @symbol dma_start_fill
; @summary Low-level DMA fill start. Call DMA.FILL for the stable public wrapper.
; @out A: 0 on success, 1 on error.
dma_start_fill:
      LDA   #DMA_MODE_FILL
      STA   DMA_MODE_REG
      LDA   #DMA_CMD_START
      STA   DMA_CMD_REG
      JMP   dma_wait

; @label DMA.WAIT
; @kind routine
; @symbol dma_wait
; @summary Wait for the DMA controller to leave BUSY and return shared status.
; @out A: 0 on success, 1 on error.
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

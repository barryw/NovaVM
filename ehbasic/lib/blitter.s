; Shared Nova blitter helper routines.
;
; Callers load the BLT hardware registers, then call blitter_start_copy or
; blitter_start_fill. Routines return A=0 on success and A=1 on error; detailed
; errors remain in BLT_ERRCODE_REG.

.include "blitter.inc"

.ifndef BLITTER_IMPLEMENTATION_INCLUDED
BLITTER_IMPLEMENTATION_INCLUDED = 1

      .segment "CODE"

      .export blitter_copy
      .export blitter_fill
      .export blitter_start_copy
      .export blitter_start_fill
      .export blitter_wait

; @label BLITTER.COPY
; @kind routine
; @symbol blitter_copy
; @summary Start a rectangular blitter copy using the BLT controller registers.
; @requires BLT_SRCSPACE BLT_DSTSPACE BLT_SRCL BLT_SRCM BLT_SRCH BLT_DSTL BLT_DSTM BLT_DSTH BLT_WIDTHL BLT_WIDTHH BLT_HEIGHTL BLT_HEIGHTH BLT_SRCSTRL BLT_SRCSTRH BLT_DSTSTRL BLT_DSTSTRH
; @out A: 0 on success, 1 on error.
blitter_copy:
      JMP   blitter_start_copy

; @label BLITTER.FILL
; @kind routine
; @symbol blitter_fill
; @summary Start a rectangular blitter fill using destination, size, stride, and fill registers.
; @requires BLT_DSTSPACE BLT_DSTL BLT_DSTM BLT_DSTH BLT_WIDTHL BLT_WIDTHH BLT_HEIGHTL BLT_HEIGHTH BLT_DSTSTRL BLT_DSTSTRH BLT_FILLVALUE
; @out A: 0 on success, 1 on error.
blitter_fill:
      STZ   BLT_SRCSPACE
      STZ   BLT_SRCL
      STZ   BLT_SRCM
      STZ   BLT_SRCH
      STZ   BLT_SRCSTRL
      STZ   BLT_SRCSTRH
      STZ   BLT_CKEY
      JMP   blitter_start_fill

; @label BLITTER.START_COPY
; @kind routine
; @symbol blitter_start_copy
; @summary Low-level blitter copy start. Call BLITTER.COPY for the public wrapper.
; @out A: 0 on success, 1 on error.
blitter_start_copy:
      STZ   BLT_MODE_REG
      STZ   BLT_CKEY
      LDA   #BLT_CMD_START
      STA   BLT_CMD_REG
      JMP   blitter_wait

; @label BLITTER.START_FILL
; @kind routine
; @symbol blitter_start_fill
; @summary Low-level blitter fill start. Call BLITTER.FILL for the public wrapper.
; @out A: 0 on success, 1 on error.
blitter_start_fill:
      LDA   #BLT_MODE_FILL
      STA   BLT_MODE_REG
      LDA   #BLT_CMD_START
      STA   BLT_CMD_REG
      JMP   blitter_wait

; @label BLITTER.WAIT
; @kind routine
; @symbol blitter_wait
; @summary Wait for the blitter controller to leave BUSY and return shared status.
; @out A: 0 on success, 1 on error.
blitter_wait:
@loop:
      LDA   BLT_STATUS_REG
      CMP   #BLT_STATUS_BUSY
      BEQ   @loop
      CMP   #BLT_STATUS_OK
      BEQ   @ok
      LDA   #BLITTER_RESULT_ERROR
      RTS
@ok:
      LDA   #BLITTER_RESULT_OK
      RTS

.endif

; Shared Nova blitter helper routines.
;
; Callers load the BLT hardware registers, then call blitter_start_copy or
; blitter_start_fill. Routines return A=0 on success and A=1 on error; detailed
; errors remain in BLT_ERRCODE_REG.

.include "blitter.inc"

.ifndef BLITTER_IMPLEMENTATION_INCLUDED
BLITTER_IMPLEMENTATION_INCLUDED = 1

      .segment "CODE"

      .export blitter_start_copy
      .export blitter_start_fill
      .export blitter_wait

blitter_start_copy:
      STZ   BLT_MODE_REG
      STZ   BLT_CKEY
      LDA   #BLT_CMD_START
      STA   BLT_CMD_REG
      JMP   blitter_wait

blitter_start_fill:
      LDA   #BLT_MODE_FILL
      STA   BLT_MODE_REG
      LDA   #BLT_CMD_START
      STA   BLT_CMD_REG
      JMP   blitter_wait

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

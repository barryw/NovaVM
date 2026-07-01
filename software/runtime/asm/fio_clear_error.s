; Small standalone FIO status-clear helper.
;
; Kept in its own object so ROMs that only need FIOCLR do not pull the full
; file-I/O helper body from fio.s.

.include "fio.inc"

.ifndef FIO_CLEAR_ERROR_IMPLEMENTATION_INCLUDED
FIO_CLEAR_ERROR_IMPLEMENTATION_INCLUDED = 1

      .segment "CODE"

      .export fio_clear_error

; Clear the host-visible FIO error/status latch.
; @label FIO.CLEAR_ERROR
; @kind routine
; @symbol fio_clear_error
; @summary Clear the host-visible FIO status and error latch.
; @out A: 0 on success.
fio_clear_error:
      LDA   #FIO_ERR_NONE
      STA   FIO_ERRCODE
      LDA   #FIO_STATUS_OK
      STA   FIO_STATUS
      LDA   #FIO_CMD_CLEARERR
      STA   FIO_CMD
      LDA   #FIO_RESULT_OK
      RTS

.endif

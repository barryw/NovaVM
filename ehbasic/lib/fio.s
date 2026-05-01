; Shared Nova File I/O helper routines.
;
; This file is BASIC-agnostic. BASIC is still responsible for parsing language
; expressions; assembly callers can load the FIO_ARG_* pseudo-registers and use
; these helpers directly.

.include "fio.inc"

.ifndef FIO_IMPLEMENTATION_INCLUDED
FIO_IMPLEMENTATION_INCLUDED = 1

      .segment "CODE"

      .export fio_issue
      .export fio_check
      .export fio_exec
      .export fio_copy_name
      .export fio_set_io_error
.ifndef FIO_NO_STREAMING
      .export fio_run
      .export fio_prepare_xram_transfer
.endif

; Input: A = FIO command. Does not check completion/status.
fio_issue:
      STA   FIO_CMD
      RTS

; Check current FIO_STATUS.
fio_check:
      LDA   FIO_STATUS
      CMP   #FIO_OK
      BEQ   @ok
      LDA   #FIO_RESULT_ERROR
      RTS
@ok:
      LDA   #FIO_RESULT_OK
      RTS

; Input: A = FIO command. Issue and check current status.
fio_exec:
      STA   FIO_CMD
      JMP   fio_check

; Mark an argument/host I/O error in the hardware status registers.
fio_set_io_error:
      LDA   #FIO_STATUS_ERROR
      STA   FIO_STATUS
      LDA   #FIO_ERR_IO
      STA   FIO_ERRCODE
      LDA   #FIO_RESULT_ERROR
      RTS

; Copy filename from FIO_ARG_NAMEPTR_L/H into FIO_NAME.
fio_copy_name:
      LDA   FIO_ARG_NAMELEN
      BEQ   @bad
      CMP   #FIO_NAME_LIMIT + 1
      BCS   @bad
      STA   FIO_NAMELEN
      TAX
      LDY   #$00
@copy:
      LDA   (FIO_ARG_NAMEPTR_L),Y
      STA   FIO_NAME,Y
      INY
      DEX
      BNE   @copy
      LDA   #FIO_RESULT_OK
      RTS
@bad:
      JMP   fio_set_io_error

.ifndef FIO_NO_STREAMING
; Input: A = FIO command. Issue, wait for non-idle status, and check it.
fio_run:
      PHA
      STZ   FIO_STATUS
      STZ   FIO_ERRCODE
      PLA
      STA   FIO_CMD
@wait:
      LDA   FIO_STATUS
      BEQ   @wait
      JMP   fio_check

; Prepare the FIO hardware registers for XLOAD/XSAVE-style direct XRAM I/O.
fio_prepare_xram_transfer:
      JSR   fio_copy_name
      BNE   @done
      LDA   FIO_ARG_ADDRL
      STA   FIO_GADDRL
      LDA   FIO_ARG_ADDRM
      STA   FIO_GADDRH
      LDA   FIO_ARG_ADDRH
      STA   FIO_GSPACE
      LDA   FIO_ARG_LENL
      STA   FIO_GLENL
      LDA   FIO_ARG_LENH
      STA   FIO_GLENH
      LDA   #FIO_RESULT_OK
@done:
      RTS
.endif

.endif

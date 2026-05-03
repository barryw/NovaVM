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
      .export fio_clear_error
      .export fio_set_io_error
      .export fio_save
      .export fio_load
      .export fio_dir_open
      .export fio_dir_read
      .export fio_delete
      .export fio_gsave
      .export fio_gload
      .export fio_cd
      .export fio_mkdir
      .export fio_rmdir
      .export fio_pwd
      .export fio_load_runtime
.ifndef FIO_NO_STREAMING
      .export fio_run
      .export fio_prepare_xram_transfer
.endif

; Input: A = FIO command. Does not check completion/status.
; @label FIO.ISSUE
; @kind routine
; @symbol fio_issue
; @summary Issue a raw FIO command without waiting for completion.
; @in A: FIO command byte.
fio_issue:
      STA   FIO_CMD
      RTS

; Check current FIO_STATUS.
; @label FIO.CHECK
; @kind routine
; @symbol fio_check
; @summary Convert the current FIO status register to the shared A=0/1 result.
; @out A: 0 on success, 1 on error.
fio_check:
      LDA   FIO_STATUS
      CMP   #FIO_OK
      BEQ   @ok
      LDA   #FIO_RESULT_ERROR
      RTS
@ok:
      LDA   #FIO_RESULT_OK
      RTS

; Input: A = FIO command. Issue, wait for completion, and check status.
; @label FIO.EXEC
; @kind routine
; @symbol fio_exec
; @summary Issue an FIO command, wait for completion, and return A=0/1 status.
; @in A: FIO command byte.
; @out A: 0 on success, 1 on error.
fio_exec:
      PHA
      STZ   FIO_STATUS
      STZ   FIO_ERRCODE
      PLA
      STA   FIO_CMD
@wait:
      LDA   FIO_STATUS
      BEQ   @wait
      JMP   fio_check

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

; Mark an argument/host I/O error in the hardware status registers.
; @label FIO.SET_IO_ERROR
; @kind routine
; @symbol fio_set_io_error
; @summary Set FIO_STATUS/FIO_ERRCODE to the shared I/O error state.
; @out A: 1 on error.
fio_set_io_error:
      LDA   #FIO_STATUS_ERROR
      STA   FIO_STATUS
      LDA   #FIO_ERR_IO
      STA   FIO_ERRCODE
      LDA   #FIO_RESULT_ERROR
      RTS

; Copy filename from FIO_ARG_NAMEPTR_L/H into FIO_NAME.
; @label FIO.COPY_NAME
; @kind routine
; @symbol fio_copy_name
; @summary Copy a pointer-based filename into FIO.NAME/FIO.NAMELEN.
; @requires FIO_ARG_NAMELEN FIO_ARG_NAMEPTR_L FIO_ARG_NAMEPTR_H
; @out A: 0 on success, 1 on invalid name.
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

; @label FIO.SAVE
; @kind routine
; @symbol fio_save
; @summary Save RAM using FIO.NAME plus FIO_SRCL/H and FIO_ENDL/H.
; @requires FIO_NAME FIO_NAMELEN FIO_SRCL FIO_SRCH FIO_ENDL FIO_ENDH
fio_save:
      LDA   #FIO_CMD_SAVE
      JMP   fio_exec

; @label FIO.LOAD
; @kind routine
; @symbol fio_load
; @summary Load a BASIC or binary file using FIO.NAME and FIO_SRCL/H.
; @requires FIO_NAME FIO_NAMELEN FIO_SRCL FIO_SRCH
fio_load:
      LDA   #FIO_CMD_LOAD
      JMP   fio_exec

; @label FIO.DIR_OPEN
; @kind routine
; @symbol fio_dir_open
; @summary Open a directory listing using optional FIO.NAME/FIO.NAMELEN filter.
fio_dir_open:
      LDA   #FIO_CMD_DIROPEN
      JMP   fio_exec

; @label FIO.DIR_READ
; @kind routine
; @symbol fio_dir_read
; @summary Read the next directory entry into the FIO directory result registers.
fio_dir_read:
      LDA   #FIO_CMD_DIRREAD
      JMP   fio_exec

; @label FIO.DELETE
; @kind routine
; @symbol fio_delete
; @summary Delete the file named by FIO.NAME/FIO.NAMELEN.
; @requires FIO_NAME FIO_NAMELEN
fio_delete:
      LDA   #FIO_CMD_DELETE
      JMP   fio_exec

; @label FIO.GSAVE
; @kind routine
; @symbol fio_gsave
; @summary Save VGC/graphics memory using FIO graphics registers.
; @requires FIO_NAME FIO_NAMELEN FIO_GSPACE FIO_GADDRL FIO_GADDRH FIO_GLENL FIO_GLENH
fio_gsave:
      LDA   #FIO_CMD_GSAVE
      JMP   fio_exec

; @label FIO.GLOAD
; @kind routine
; @symbol fio_gload
; @summary Load VGC/graphics memory using FIO graphics registers.
; @requires FIO_NAME FIO_NAMELEN FIO_GSPACE FIO_GADDRL FIO_GADDRH FIO_GLENL FIO_GLENH
fio_gload:
      LDA   #FIO_CMD_GLOAD
      JMP   fio_exec

; @label FIO.CD
; @kind routine
; @symbol fio_cd
; @summary Change directory to FIO.NAME/FIO.NAMELEN.
; @requires FIO_NAME FIO_NAMELEN
fio_cd:
      LDA   #FIO_CMD_CD
      JMP   fio_exec

; @label FIO.MKDIR
; @kind routine
; @symbol fio_mkdir
; @summary Create the directory named by FIO.NAME/FIO.NAMELEN.
; @requires FIO_NAME FIO_NAMELEN
fio_mkdir:
      LDA   #FIO_CMD_MKDIR
      JMP   fio_exec

; @label FIO.RMDIR
; @kind routine
; @symbol fio_rmdir
; @summary Remove the directory named by FIO.NAME/FIO_NAMELEN.
; @requires FIO_NAME FIO_NAMELEN
fio_rmdir:
      LDA   #FIO_CMD_RMDIR
      JMP   fio_exec

; @label FIO.PWD
; @kind routine
; @symbol fio_pwd
; @summary Read the current directory into FIO.NAME/FIO.NAMELEN.
fio_pwd:
      LDA   #FIO_CMD_PWD
      JMP   fio_exec

; Load a 16K runtime ROM image into the primary $C000 bank.
; This routine is meant for RAM-resident launchers. The host overwrites the
; primary ROM bank while the command is in progress.
; @label FIO.LOAD_RUNTIME
; @kind routine
; @symbol fio_load_runtime
; @summary Load a 16K runtime ROM image named by FIO.NAME/FIO.NAMELEN into the primary runtime ROM bank.
; @requires FIO_NAME FIO_NAMELEN
fio_load_runtime:
      LDA   #FIO_CMD_LOADRUNTIME
      JMP   fio_exec

.ifndef FIO_NO_STREAMING
; Compatibility alias for assembly callers that used the streaming name.
; @label FIO.RUN
; @kind routine
; @symbol fio_run
; @summary Compatibility alias for FIO.EXEC.
fio_run:
      JMP   fio_exec

; Prepare the FIO hardware registers for XLOAD/XSAVE-style direct XRAM I/O.
; @label FIO.PREPARE_XRAM_TRANSFER
; @kind routine
; @symbol fio_prepare_xram_transfer
; @summary Copy pointer filename and XRAM transfer arguments into FIO registers.
; @requires FIO_ARG_NAMELEN FIO_ARG_NAMEPTR_L FIO_ARG_NAMEPTR_H FIO_ARG_ADDRL FIO_ARG_ADDRM FIO_ARG_ADDRH FIO_ARG_LENL FIO_ARG_LENH
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

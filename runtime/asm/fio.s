; Shared Nova File I/O helper routines.
;
; This file is BASIC-agnostic. BASIC is still responsible for parsing language
; expressions; assembly callers can load the FIO_ARG_* pseudo-registers and use
; these helpers directly.

.include "fio.inc"

.ifndef FIO_IMPLEMENTATION_INCLUDED
FIO_IMPLEMENTATION_INCLUDED = 1

; Default to emitting only routines that have been referenced earlier in the
; source file. Define NOVA_EMIT_ALL_RUNTIME or FIO_EMIT_ALL_RUNTIME before
; including this file to keep the full implementation for ROMs/debug builds.
.ifdef NOVA_STRIP_UNUSED
FIO_EMIT_ALL = 0
.else
.ifdef FIO_STRIP_UNUSED
FIO_EMIT_ALL = 0
.else
.ifdef NOVA_EMIT_ALL_RUNTIME
FIO_EMIT_ALL = 1
.else
.ifdef FIO_EMIT_ALL_RUNTIME
FIO_EMIT_ALL = 1
.else
FIO_EMIT_ALL = 0
.endif
.endif
.endif
.endif

.if FIO_EMIT_ALL = 0
.if .referenced(fio_save) .OR .referenced(fio_load) .OR .referenced(fio_dir_open) .OR .referenced(fio_dir_read) .OR .referenced(fio_delete) .OR .referenced(fio_gsave) .OR .referenced(fio_gload) .OR .referenced(fio_cd) .OR .referenced(fio_mkdir) .OR .referenced(fio_rmdir) .OR .referenced(fio_pwd) .OR .referenced(fio_load_runtime) .OR .referenced(fio_rng) .OR .referenced(fio_run)
      .refto fio_exec
.endif
.if .referenced(fio_exec)
      .refto fio_check
.endif
.if .referenced(fio_copy_name) .OR .referenced(fio_prepare_xram_transfer)
      .refto fio_set_io_error
.endif
.if .referenced(fio_prepare_xram_transfer)
      .refto fio_copy_name
.endif
.endif

      .segment "CODE"

.if FIO_EMIT_ALL .OR .referenced(fio_issue)
      .export fio_issue
.endif
.if FIO_EMIT_ALL .OR .referenced(fio_check)
      .export fio_check
.endif
.if FIO_EMIT_ALL .OR .referenced(fio_exec)
      .export fio_exec
.endif
.if FIO_EMIT_ALL .OR .referenced(fio_copy_name)
      .export fio_copy_name
.endif
.if FIO_EMIT_ALL .OR .referenced(fio_clear_error)
      .export fio_clear_error
.endif
.if FIO_EMIT_ALL .OR .referenced(fio_set_io_error)
      .export fio_set_io_error
.endif
.if FIO_EMIT_ALL .OR .referenced(fio_save)
      .export fio_save
.endif
.if FIO_EMIT_ALL .OR .referenced(fio_load)
      .export fio_load
.endif
.if FIO_EMIT_ALL .OR .referenced(fio_dir_open)
      .export fio_dir_open
.endif
.if FIO_EMIT_ALL .OR .referenced(fio_dir_read)
      .export fio_dir_read
.endif
.if FIO_EMIT_ALL .OR .referenced(fio_delete)
      .export fio_delete
.endif
.if FIO_EMIT_ALL .OR .referenced(fio_gsave)
      .export fio_gsave
.endif
.if FIO_EMIT_ALL .OR .referenced(fio_gload)
      .export fio_gload
.endif
.if FIO_EMIT_ALL .OR .referenced(fio_cd)
      .export fio_cd
.endif
.if FIO_EMIT_ALL .OR .referenced(fio_mkdir)
      .export fio_mkdir
.endif
.if FIO_EMIT_ALL .OR .referenced(fio_rmdir)
      .export fio_rmdir
.endif
.if FIO_EMIT_ALL .OR .referenced(fio_pwd)
      .export fio_pwd
.endif
.if FIO_EMIT_ALL .OR .referenced(fio_load_runtime)
      .export fio_load_runtime
.endif
.if FIO_EMIT_ALL .OR .referenced(fio_rng)
      .export fio_rng
.endif
.ifndef FIO_NO_STREAMING
.if FIO_EMIT_ALL .OR .referenced(fio_run)
      .export fio_run
.endif
.if FIO_EMIT_ALL .OR .referenced(fio_prepare_xram_transfer)
      .export fio_prepare_xram_transfer
.endif
.endif

; Input: A = FIO command. Does not check completion/status.
; @label FIO.ISSUE
; @kind routine
; @symbol fio_issue
; @summary Issue a raw FIO command without waiting for completion.
; @in A: FIO command byte.
.if FIO_EMIT_ALL .OR .referenced(fio_issue)
fio_issue:
      STA   FIO_CMD
      RTS
.endif

; Check current FIO_STATUS.
; @label FIO.CHECK
; @kind routine
; @symbol fio_check
; @summary Convert the current FIO status register to the shared A=0/1 result.
; @out A: 0 on success, 1 on error.
.if FIO_EMIT_ALL .OR .referenced(fio_check)
fio_check:
      LDA   FIO_STATUS
      CMP   #FIO_OK
      BEQ   @ok
      LDA   #FIO_RESULT_ERROR
      RTS
@ok:
      LDA   #FIO_RESULT_OK
      RTS
.endif

; Input: A = FIO command. Issue, wait for completion, and check status.
; @label FIO.EXEC
; @kind routine
; @symbol fio_exec
; @summary Issue an FIO command, wait for completion, and return A=0/1 status.
; @in A: FIO command byte.
; @out A: 0 on success, 1 on error.
.if FIO_EMIT_ALL .OR .referenced(fio_exec)
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
.endif

; Clear the host-visible FIO error/status latch.
; @label FIO.CLEAR_ERROR
; @kind routine
; @symbol fio_clear_error
; @summary Clear the host-visible FIO status and error latch.
; @out A: 0 on success.
.if FIO_EMIT_ALL .OR .referenced(fio_clear_error)
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

; Mark an argument/host I/O error in the hardware status registers.
; @label FIO.SET_IO_ERROR
; @kind routine
; @symbol fio_set_io_error
; @summary Set FIO_STATUS/FIO_ERRCODE to the shared I/O error state.
; @out A: 1 on error.
.if FIO_EMIT_ALL .OR .referenced(fio_set_io_error)
fio_set_io_error:
      LDA   #FIO_STATUS_ERROR
      STA   FIO_STATUS
      LDA   #FIO_ERR_IO
      STA   FIO_ERRCODE
      LDA   #FIO_RESULT_ERROR
      RTS
.endif

; Copy filename from FIO_ARG_NAMEPTR_L/H into FIO_NAME.
; @label FIO.COPY_NAME
; @kind routine
; @symbol fio_copy_name
; @summary Copy a pointer-based filename into FIO.NAME/FIO.NAMELEN.
; @requires FIO_ARG_NAMELEN FIO_ARG_NAMEPTR_L FIO_ARG_NAMEPTR_H
; @out A: 0 on success, 1 on invalid name.
.if FIO_EMIT_ALL .OR .referenced(fio_copy_name)
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
.endif

; @label FIO.SAVE
; @kind routine
; @symbol fio_save
; @summary Save RAM using FIO.NAME plus FIO_SRCL/H and FIO_ENDL/H.
; @requires FIO_NAME FIO_NAMELEN FIO_SRCL FIO_SRCH FIO_ENDL FIO_ENDH
.if FIO_EMIT_ALL .OR .referenced(fio_save)
fio_save:
      LDA   #FIO_CMD_SAVE
      JMP   fio_exec
.endif

; @label FIO.LOAD
; @kind routine
; @symbol fio_load
; @summary Load a BASIC or binary file using FIO.NAME and FIO_SRCL/H.
; @requires FIO_NAME FIO_NAMELEN FIO_SRCL FIO_SRCH
.if FIO_EMIT_ALL .OR .referenced(fio_load)
fio_load:
      LDA   #FIO_CMD_LOAD
      JMP   fio_exec
.endif

; @label FIO.DIR_OPEN
; @kind routine
; @symbol fio_dir_open
; @summary Open a directory listing using optional FIO.NAME/FIO.NAMELEN filter.
.if FIO_EMIT_ALL .OR .referenced(fio_dir_open)
fio_dir_open:
      LDA   #FIO_CMD_DIROPEN
      JMP   fio_exec
.endif

; @label FIO.DIR_READ
; @kind routine
; @symbol fio_dir_read
; @summary Read the next directory entry into the FIO directory result registers.
.if FIO_EMIT_ALL .OR .referenced(fio_dir_read)
fio_dir_read:
      LDA   #FIO_CMD_DIRREAD
      JMP   fio_exec
.endif

; @label FIO.DELETE
; @kind routine
; @symbol fio_delete
; @summary Delete the host file or BASIC program named by FIO.NAME/FIO.NAMELEN.
; @requires FIO_NAME FIO_NAMELEN
.if FIO_EMIT_ALL .OR .referenced(fio_delete)
fio_delete:
      LDA   #FIO_CMD_DELETE
      JMP   fio_exec
.endif

; @label FIO.GSAVE
; @kind routine
; @symbol fio_gsave
; @summary Save VGC/graphics memory using FIO graphics registers.
; @requires FIO_NAME FIO_NAMELEN FIO_GSPACE FIO_GADDRL FIO_GADDRH FIO_GLENL FIO_GLENH
.if FIO_EMIT_ALL .OR .referenced(fio_gsave)
fio_gsave:
      LDA   #FIO_CMD_GSAVE
      JMP   fio_exec
.endif

; @label FIO.GLOAD
; @kind routine
; @symbol fio_gload
; @summary Load VGC/graphics memory using FIO graphics registers.
; @requires FIO_NAME FIO_NAMELEN FIO_GSPACE FIO_GADDRL FIO_GADDRH FIO_GLENL FIO_GLENH
.if FIO_EMIT_ALL .OR .referenced(fio_gload)
fio_gload:
      LDA   #FIO_CMD_GLOAD
      JMP   fio_exec
.endif

; @label FIO.CD
; @kind routine
; @symbol fio_cd
; @summary Change directory to FIO.NAME/FIO.NAMELEN.
; @requires FIO_NAME FIO_NAMELEN
.if FIO_EMIT_ALL .OR .referenced(fio_cd)
fio_cd:
      LDA   #FIO_CMD_CD
      JMP   fio_exec
.endif

; @label FIO.MKDIR
; @kind routine
; @symbol fio_mkdir
; @summary Create the directory named by FIO.NAME/FIO.NAMELEN.
; @requires FIO_NAME FIO_NAMELEN
.if FIO_EMIT_ALL .OR .referenced(fio_mkdir)
fio_mkdir:
      LDA   #FIO_CMD_MKDIR
      JMP   fio_exec
.endif

; @label FIO.RMDIR
; @kind routine
; @symbol fio_rmdir
; @summary Remove the directory named by FIO.NAME/FIO_NAMELEN.
; @requires FIO_NAME FIO_NAMELEN
.if FIO_EMIT_ALL .OR .referenced(fio_rmdir)
fio_rmdir:
      LDA   #FIO_CMD_RMDIR
      JMP   fio_exec
.endif

; @label FIO.PWD
; @kind routine
; @symbol fio_pwd
; @summary Read the current directory into FIO.NAME/FIO.NAMELEN.
.if FIO_EMIT_ALL .OR .referenced(fio_pwd)
fio_pwd:
      LDA   #FIO_CMD_PWD
      JMP   fio_exec
.endif

; Load a 16K runtime ROM image into the primary $C000 bank.
; This routine is meant for RAM-resident launchers. The host overwrites the
; primary ROM bank while the command is in progress.
; @label FIO.LOAD_RUNTIME
; @kind routine
; @symbol fio_load_runtime
; @summary Load a 16K runtime ROM image named by FIO.NAME/FIO.NAMELEN into the primary runtime ROM bank.
; @requires FIO_NAME FIO_NAMELEN
.if FIO_EMIT_ALL .OR .referenced(fio_load_runtime)
fio_load_runtime:
      LDA   #FIO_CMD_LOADRUNTIME
      JMP   fio_exec
.endif

; Fetch 32 host-backed random bits into FIO_RNG0..3.
; @label FIO.RNG
; @kind routine
; @symbol fio_rng
; @summary Issue FIO.CMD_RNG and return random bytes in FIO_RNG0..3.
; @out A: 0 on success, 1 on error.
; @out FIO_RNG0 FIO_RNG1 FIO_RNG2 FIO_RNG3
.if FIO_EMIT_ALL .OR .referenced(fio_rng)
fio_rng:
      LDA   #FIO_CMD_RNG
      JMP   fio_exec
.endif

.ifndef FIO_NO_STREAMING
; Compatibility alias for assembly callers that used the streaming name.
; @label FIO.RUN
; @kind routine
; @symbol fio_run
; @summary Compatibility alias for FIO.EXEC.
.if FIO_EMIT_ALL .OR .referenced(fio_run)
fio_run:
      JMP   fio_exec
.endif

; Prepare the FIO hardware registers for XLOAD/XSAVE-style direct XRAM I/O.
; @label FIO.PREPARE_XRAM_TRANSFER
; @kind routine
; @symbol fio_prepare_xram_transfer
; @summary Copy pointer filename and XRAM transfer arguments into FIO registers.
; @requires FIO_ARG_NAMELEN FIO_ARG_NAMEPTR_L FIO_ARG_NAMEPTR_H FIO_ARG_ADDRL FIO_ARG_ADDRM FIO_ARG_ADDRH FIO_ARG_LENL FIO_ARG_LENH
.if FIO_EMIT_ALL .OR .referenced(fio_prepare_xram_transfer)
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

.endif

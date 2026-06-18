; Shared Nova file pager routines.

.include "pager.inc"

.ifndef PAGER_IMPLEMENTATION_INCLUDED
PAGER_IMPLEMENTATION_INCLUDED = 1

      .segment "CODE"

      .export pager_load_file_page
      .export pager_load_current_file_page

; Stream a file slice directly into the selected target.
; @label PAGER.LOAD_FILE_PAGE
; @kind routine
; @symbol pager_load_file_page
; @summary Load PAGER_LEN bytes from PAGER_FILE offset in PAGER_NAME into PAGER_TARGET/PAGER_ADDR.
; @requires PAGER_FILEL PAGER_FILEM PAGER_FILEH PAGER_TARGET PAGER_ADDRL PAGER_ADDRM PAGER_ADDRH PAGER_LENL PAGER_LENH PAGER_NAMELEN PAGER_NAMEPTR_L PAGER_NAMEPTR_H
; @out A: 0 on success, 1 on error.
pager_load_file_page:
      JSR   fio_copy_name
      BEQ   pager_load_current_file_page
      RTS

; Stream a file slice directly into the selected target using the current
; FIO.NAME/FIO.NAMELEN.
; @label PAGER.LOAD_CURRENT_FILE_PAGE
; @kind routine
; @symbol pager_load_current_file_page
; @summary Load PAGER_LEN bytes from PAGER_FILE offset in the current FIO.NAME into PAGER_TARGET/PAGER_ADDR.
; @requires PAGER_FILEL PAGER_FILEM PAGER_FILEH PAGER_TARGET PAGER_ADDRL PAGER_ADDRM PAGER_ADDRH PAGER_LENL PAGER_LENH FIO_NAME FIO_NAMELEN
; @out A: 0 on success, 1 on error.
pager_load_current_file_page:
      LDA   PAGER_FILEL
      STA   FIO_SRCL
      LDA   PAGER_FILEM
      STA   FIO_SRCH
      LDA   PAGER_FILEH
      STA   FIO_ENDL
      STZ   FIO_ENDH

      LDA   PAGER_ADDRL
      STA   FIO_GADDRL
      LDA   PAGER_ADDRM
      STA   FIO_GADDRH
      LDA   PAGER_ADDRH
      STA   FIO_GSPACE
      LDA   PAGER_LENL
      STA   FIO_GLENL
      LDA   PAGER_LENH
      STA   FIO_GLENH
      LDA   PAGER_TARGET
      STA   FIO_DIRTYPE

      LDA   #FIO_CMD_XPAGE
      JMP   fio_exec

.endif

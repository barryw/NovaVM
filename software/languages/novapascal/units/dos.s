; Turbo-compatible Dos unit over the canonical FILES module.

.include "dos.inc"

.ifndef NOVA_DOS_UNIT_IMPLEMENTATION_INCLUDED
NOVA_DOS_UNIT_IMPLEMENTATION_INCLUDED = 1

DOS_ERROR_FILE_NOT_FOUND = 2
DOS_ERROR_ACCESS_DENIED  = 5
DOS_ERROR_INVALID_DRIVE = 15
DOS_ERROR_NO_MORE_FILES = 18

DOS_SEARCH_ATTR = 21
DOS_SEARCH_TIME = 22
DOS_SEARCH_SIZE = 26
DOS_SEARCH_NAME = 30

.if .referenced(doserror) .OR .referenced(findfirst) .OR .referenced(findnext)
.ifndef DOS_UNIT_REFERENCED
DOS_UNIT_REFERENCED = 1
.endif
.endif
.if .referenced(diskfree) .OR .referenced(disksize)
.ifndef DOS_UNIT_REFERENCED
DOS_UNIT_REFERENCED = 1
.endif
.endif
.if .referenced(getfattr) .OR .referenced(setfattr) .OR .referenced(getftime) .OR .referenced(setftime)
.ifndef DOS_UNIT_REFERENCED
DOS_UNIT_REFERENCED = 1
.endif
.endif
.if .referenced(getdate) .OR .referenced(setdate) .OR .referenced(gettime) .OR .referenced(settime)
.ifndef DOS_UNIT_REFERENCED
DOS_UNIT_REFERENCED = 1
.endif
.endif
.if .referenced(packtime) .OR .referenced(unpacktime) .OR .referenced(fexpand) .OR .referenced(fsplit)
.ifndef DOS_UNIT_REFERENCED
DOS_UNIT_REFERENCED = 1
.endif
.endif

.ifdef DOS_UNIT_REFERENCED
      .segment "BSS"
      .export doserror
doserror:          .res 2
dos_find_attr:     .res 1
dos_search_ptr:    .res 2
dos_file_ptr:      .res 2
dos_value_ptr:     .res 2
dos_source_ptr:    .res 2
dos_dir_ptr:       .res 2
dos_name_ptr:      .res 2
dos_ext_ptr:       .res 2
dos_value:         .res 1
dos_path_kind:     .res 1
dos_path_len:      .res 1
dos_dir_end:       .res 1
dos_ext_start:     .res 1
dos_copy_start:    .res 1
dos_copy_count:    .res 1
dos_copy_limit:    .res 1
dos_result:        .res 80

      .segment "CODE"

dos_file_call:
      STA   LIB_FN_ID
      LDA   #MODULE_ID_FILES
      STA   LIB_MOD_ID
      JMP   LIB_LOADER_BAND

dos_ok:
      STZ   doserror
      STZ   doserror+1
      RTS

dos_set_error:
      STA   doserror
      STZ   doserror+1
      RTS

dos_map_fio_error:
      LDA   FIO_ERRCODE
      CMP   #FIO_ERR_EOD
      BEQ   @eod
      CMP   #FIO_ERR_NOTFOUND
      BEQ   @not_found
      CMP   #FIO_ERR_NOTMOUNTED
      BEQ   @drive
      LDA   #DOS_ERROR_ACCESS_DENIED
      BRA   dos_set_error
@eod:
      LDA   #DOS_ERROR_NO_MORE_FILES
      BRA   dos_set_error
@not_found:
      LDA   #DOS_ERROR_FILE_NOT_FOUND
      BRA   dos_set_error
@drive:
      LDA   #DOS_ERROR_INVALID_DRIVE
      BRA   dos_set_error

; A/X points to a Pascal string. Marshal its character bytes to FILES.
dos_name_from_string:
      STA   NVR0L
      STX   NVR0H
      LDY   #0
      LDA   (NVR0L),Y
      STA   LIB_ARG1
      STZ   LIB_ARG1+1
      STZ   LIB_ARG1+2
      STZ   LIB_ARG1+3
      CLC
      LDA   NVR0L
      ADC   #1
      STA   LIB_ARG0
      LDA   NVR0H
      ADC   #0
      STA   LIB_ARG0+1
      STZ   LIB_ARG0+2
      STZ   LIB_ARG0+3
      RTS

; A/X points to NovaPascal's 16-byte file descriptor.
dos_name_from_file:
      STA   NVR0L
      STX   NVR0H
      LDY   #2
      LDA   (NVR0L),Y
      STA   LIB_ARG1
      STZ   LIB_ARG1+1
      STZ   LIB_ARG1+2
      STZ   LIB_ARG1+3
      CLC
      LDA   NVR0L
      ADC   #3
      STA   LIB_ARG0
      LDA   NVR0H
      ADC   #0
      STA   LIB_ARG0+1
      STZ   LIB_ARG0+2
      STZ   LIB_ARG0+3
      RTS

      .export findfirst
findfirst:
      TSX
      LDA   $0103,X
      STA   dos_search_ptr
      LDA   $0104,X
      STA   dos_search_ptr+1
      LDA   $0105,X
      STA   dos_find_attr
      LDA   $0107,X
      PHA
      LDA   $0108,X
      TAX
      PLA
      JSR   dos_name_from_string
      LDA   #FIO_DIR_FLAG_FULLNAME
      STA   FIO_DIRTYPE
      LDA   #FILE_DIR_OPEN
      JSR   dos_file_call
      LDA   LIB_STATUS
      BNE   @failed
      BRA   dos_find_next_current
@failed:
      JMP   dos_map_fio_error

      .export findnext
findnext:
      STA   dos_search_ptr
      STX   dos_search_ptr+1
dos_find_next_current:
@read:
      LDA   #FILE_DIR_READ
      JSR   dos_file_call
      LDA   LIB_STATUS
      BNE   @failed
      LDA   FIO_FILE_ATTR
      AND   #$1E
      STA   NVR1L
      LDA   dos_find_attr
      EOR   #$FF
      AND   NVR1L
      BNE   @read

      LDA   dos_search_ptr
      STA   NVR0L
      LDA   dos_search_ptr+1
      STA   NVR0H
      LDA   #0
      LDY   #20
@clear_fill:
      STA   (NVR0L),Y
      DEY
      BPL   @clear_fill
      LDY   #DOS_SEARCH_ATTR
      LDA   FIO_FILE_ATTR
      STA   (NVR0L),Y
      LDX   #0
      LDY   #DOS_SEARCH_TIME
@time:
      LDA   FIO_FILE_TIME0,X
      STA   (NVR0L),Y
      INX
      INY
      CPX   #4
      BNE   @time
      LDX   #0
      LDY   #DOS_SEARCH_SIZE
@size:
      LDA   FIO_SIZEL,X
      STA   (NVR0L),Y
      INX
      INY
      CPX   #3
      BNE   @size
      LDA   #0
      STA   (NVR0L),Y
      LDA   FIO_NAMELEN
      CMP   #13
      BCC   @len_ok
      LDA   #12
@len_ok:
      LDY   #DOS_SEARCH_NAME
      STA   (NVR0L),Y
      TAX
      BEQ   @done
      LDY   #DOS_SEARCH_NAME+1
      LDX   #0
@name:
      LDA   FIO_NAME,X
      STA   (NVR0L),Y
      INX
      INY
      CPX   FIO_NAMELEN
      BEQ   @done
      CPX   #12
      BNE   @name
@done:
      JMP   dos_ok
@failed:
      JMP   dos_map_fio_error

dos_prepare_drive:
      CMP   #0
      BNE   @numbered
      LDA   #FILE_PWD
      JSR   dos_file_call
      LDA   LIB_STATUS
      BEQ   @pwd_ready
      JSR   dos_map_fio_error
      SEC
      RTS
@pwd_ready:
      LDX   #0
@colon:
      CPX   FIO_NAMELEN
      BCS   @invalid
      LDA   FIO_NAME,X
      CMP   #':'
      BEQ   @pwd_length
      INX
      BRA   @colon
@pwd_length:
      STX   LIB_ARG1
      LDA   #<FIO_NAME
      STA   LIB_ARG0
      LDA   #>FIO_NAME
      STA   LIB_ARG0+1
      BRA   @clear
@numbered:
      CMP   #7
      BCS   @invalid
      DEC
      TAX
      LDA   dos_drive_lo,X
      STA   LIB_ARG0
      LDA   dos_drive_hi,X
      STA   LIB_ARG0+1
      LDA   #3
      STA   LIB_ARG1
@clear:
      STZ   LIB_ARG0+2
      STZ   LIB_ARG0+3
      STZ   LIB_ARG1+1
      STZ   LIB_ARG1+2
      STZ   LIB_ARG1+3
      CLC
      RTS
@invalid:
      LDA   #DOS_ERROR_INVALID_DRIVE
      JSR   dos_set_error
      SEC
      RTS

      .export diskfree
diskfree:
      LDY   #FILE_DISK_FREE
      BRA   dos_disk_status
      .export disksize
disksize:
      LDY   #FILE_DISK_SIZE
dos_disk_status:
      PHY
      JSR   dos_prepare_drive
      PLY
      BCS   @minus_one
      TYA
      JSR   dos_file_call
      LDA   LIB_STATUS
      BEQ   @ok
      JSR   dos_map_fio_error
@minus_one:
      LDA   #$FF
      STA   dos_result
      STA   dos_result+1
      STA   dos_result+2
      STA   dos_result+3
      LDA   #<dos_result
      LDX   #>dos_result
      RTS
@ok:
      JSR   dos_ok
      LDA   #<LIB_RESULT
      LDX   #>LIB_RESULT
      RTS

      .export getfattr
getfattr:
      TSX
      LDA   $0103,X
      STA   dos_value_ptr
      LDA   $0104,X
      STA   dos_value_ptr+1
      LDA   $0105,X
      STA   dos_file_ptr
      LDA   $0106,X
      STA   dos_file_ptr+1
      JSR   dos_get_info
      BCS   dos_info_failed
      LDA   dos_value_ptr
      STA   NVR0L
      LDA   dos_value_ptr+1
      STA   NVR0H
      LDY   #0
      LDA   LIB_RESULT
      STA   (NVR0L),Y
      INY
      LDA   #0
      STA   (NVR0L),Y
      JMP   dos_ok

      .export getftime
getftime:
      TSX
      LDA   $0103,X
      STA   dos_value_ptr
      LDA   $0104,X
      STA   dos_value_ptr+1
      LDA   $0105,X
      STA   dos_file_ptr
      LDA   $0106,X
      STA   dos_file_ptr+1
      JSR   dos_get_info
      BCS   dos_info_failed
      LDA   dos_value_ptr
      STA   NVR0L
      LDA   dos_value_ptr+1
      STA   NVR0H
      LDX   #0
      LDY   #0
@copy:
      LDA   FIO_FILE_TIME0,X
      STA   (NVR0L),Y
      INX
      INY
      CPX   #4
      BNE   @copy
      JMP   dos_ok

dos_get_info:
      LDA   dos_file_ptr
      LDX   dos_file_ptr+1
      JSR   dos_name_from_file
      LDA   #FILE_INFO_GET
      JSR   dos_file_call
      LDA   LIB_STATUS
      BEQ   @ok
      SEC
      RTS
@ok:
      CLC
      RTS

dos_info_failed:
      JMP   dos_map_fio_error

      .export setfattr
setfattr:
      TSX
      LDA   $0103,X
      STA   dos_value
      LDA   $0105,X
      STA   dos_file_ptr
      LDA   $0106,X
      STA   dos_file_ptr+1
      JSR   dos_get_info
      BCS   dos_info_failed
      LDX   #0
@save_time:
      LDA   FIO_FILE_TIME0,X
      STA   LIB_ARG3,X
      INX
      CPX   #4
      BNE   @save_time
      LDA   dos_file_ptr
      LDX   dos_file_ptr+1
      JSR   dos_name_from_file
      LDA   dos_value
      STA   LIB_ARG2
      STZ   LIB_ARG2+1
      STZ   LIB_ARG2+2
      STZ   LIB_ARG2+3
      BRA   dos_set_info

      .export setftime
setftime:
      TSX
      LDA   $0103,X
      STA   dos_value_ptr
      LDA   $0104,X
      STA   dos_value_ptr+1
      LDA   $0105,X
      STA   dos_file_ptr
      LDA   $0106,X
      STA   dos_file_ptr+1
      JSR   dos_get_info
      BCS   dos_info_failed
      LDA   LIB_RESULT
      STA   dos_value
      LDA   dos_file_ptr
      LDX   dos_file_ptr+1
      JSR   dos_name_from_file
      LDA   dos_value
      STA   LIB_ARG2
      STZ   LIB_ARG2+1
      STZ   LIB_ARG2+2
      STZ   LIB_ARG2+3
      LDA   dos_value_ptr
      STA   NVR0L
      LDA   dos_value_ptr+1
      STA   NVR0H
      LDX   #0
      LDY   #0
@load_time:
      LDA   (NVR0L),Y
      STA   LIB_ARG3,X
      INX
      INY
      CPX   #4
      BNE   @load_time
dos_set_info:
      LDA   #FILE_INFO_SET
      JSR   dos_file_call
      LDA   LIB_STATUS
      BEQ   @ok
      JMP   dos_info_failed
@ok:
      JMP   dos_ok

      .export getdate
getdate:
      LDA   #FILE_CLOCK_GET
      JSR   dos_file_call
      LDA   LIB_STATUS
      BEQ   @ok
      JMP   dos_info_failed
@ok:
      TSX
      LDA   $0109,X
      STA   NVR0L
      LDA   $010A,X
      STA   NVR0H
      LDY   #0
      LDA   FIO_CLOCK_YEARL
      STA   (NVR0L),Y
      INY
      LDA   FIO_CLOCK_YEARH
      STA   (NVR0L),Y
      LDA   $0107,X
      STA   NVR0L
      LDA   $0108,X
      STA   NVR0H
      JSR   dos_store_month
      LDA   $0105,X
      STA   NVR0L
      LDA   $0106,X
      STA   NVR0H
      JSR   dos_store_day
      LDA   $0103,X
      STA   NVR0L
      LDA   $0104,X
      STA   NVR0H
      LDA   FIO_CLOCK_DOW
      JSR   dos_store_byte_word
      JMP   dos_ok
dos_store_month:
      LDA   FIO_CLOCK_MONTH
      BRA   dos_store_byte_word
dos_store_day:
      LDA   FIO_CLOCK_DAY
dos_store_byte_word:
      LDY   #0
      STA   (NVR0L),Y
      INY
      LDA   #0
      STA   (NVR0L),Y
      RTS

      .export gettime
gettime:
      LDA   #FILE_CLOCK_GET
      JSR   dos_file_call
      LDA   LIB_STATUS
      BEQ   @ok
      JMP   dos_info_failed
@ok:
      TSX
      LDA   $0109,X
      STA   NVR0L
      LDA   $010A,X
      STA   NVR0H
      LDA   FIO_CLOCK_HOUR
      JSR   dos_store_byte_word
      LDA   $0107,X
      STA   NVR0L
      LDA   $0108,X
      STA   NVR0H
      LDA   FIO_CLOCK_MIN
      JSR   dos_store_byte_word
      LDA   $0105,X
      STA   NVR0L
      LDA   $0106,X
      STA   NVR0H
      LDA   FIO_CLOCK_SEC
      JSR   dos_store_byte_word
      LDA   $0103,X
      STA   NVR0L
      LDA   $0104,X
      STA   NVR0H
      LDA   FIO_CLOCK_HSEC
      JSR   dos_store_byte_word
      JMP   dos_ok

      .export setdate
setdate:
      LDA   #FILE_CLOCK_GET
      JSR   dos_file_call
      LDA   LIB_STATUS
      BEQ   @ok
      JMP   dos_info_failed
@ok:
      LDA   FIO_CLOCK_HOUR
      STA   LIB_ARG1
      LDA   FIO_CLOCK_MIN
      STA   LIB_ARG1+1
      LDA   FIO_CLOCK_SEC
      STA   LIB_ARG1+2
      LDA   FIO_CLOCK_HSEC
      STA   LIB_ARG1+3
      TSX
      LDA   $0107,X
      STA   LIB_ARG0
      LDA   $0108,X
      STA   LIB_ARG0+1
      LDA   $0105,X
      STA   LIB_ARG0+2
      LDA   $0103,X
      STA   LIB_ARG0+3
      BRA   dos_clock_set

      .export settime
settime:
      LDA   #FILE_CLOCK_GET
      JSR   dos_file_call
      LDA   LIB_STATUS
      BEQ   @ok
      JMP   dos_info_failed
@ok:
      LDA   FIO_CLOCK_YEARL
      STA   LIB_ARG0
      LDA   FIO_CLOCK_YEARH
      STA   LIB_ARG0+1
      LDA   FIO_CLOCK_MONTH
      STA   LIB_ARG0+2
      LDA   FIO_CLOCK_DAY
      STA   LIB_ARG0+3
      TSX
      LDA   $0109,X
      STA   LIB_ARG1
      LDA   $0107,X
      STA   LIB_ARG1+1
      LDA   $0105,X
      STA   LIB_ARG1+2
      LDA   $0103,X
      STA   LIB_ARG1+3
dos_clock_set:
      LDA   #FILE_CLOCK_SET
      JSR   dos_file_call
      LDA   LIB_STATUS
      BEQ   @ok
      JMP   dos_info_failed
@ok:
      JMP   dos_ok

      .export packtime
packtime:
      TSX
      LDA   $0105,X
      STA   NVR0L
      LDA   $0106,X
      STA   NVR0H
      LDA   $0103,X
      STA   NVR1L
      LDA   $0104,X
      STA   NVR1H
      LDY   #10
      LDA   (NVR0L),Y
      LSR
      AND   #$1F
      STA   NVR2L
      LDY   #8
      LDA   (NVR0L),Y
      STA   NVR2H
      ASL
      ASL
      ASL
      ASL
      ASL
      ORA   NVR2L
      LDY   #0
      STA   (NVR1L),Y
      LDA   NVR2H
      LSR
      LSR
      LSR
      STA   NVR2L
      LDY   #6
      LDA   (NVR0L),Y
      ASL
      ASL
      ASL
      ORA   NVR2L
      LDY   #1
      STA   (NVR1L),Y
      LDY   #4
      LDA   (NVR0L),Y
      AND   #$1F
      STA   NVR2L
      LDY   #2
      LDA   (NVR0L),Y
      STA   NVR2H
      ASL
      ASL
      ASL
      ASL
      ASL
      ORA   NVR2L
      LDY   #2
      STA   (NVR1L),Y
      LDA   NVR2H
      LSR
      LSR
      LSR
      AND   #1
      STA   NVR2L
      LDY   #0
      LDA   (NVR0L),Y
      SEC
      SBC   #<$07BC
      ASL
      ORA   NVR2L
      LDY   #3
      STA   (NVR1L),Y
      JMP   dos_ok

      .export unpacktime
unpacktime:
      TSX
      LDA   $0105,X
      STA   NVR0L
      LDA   $0106,X
      STA   NVR0H
      LDA   $0103,X
      STA   NVR1L
      LDA   $0104,X
      STA   NVR1H
      LDY   #3
      LDA   (NVR0L),Y
      LSR
      CLC
      ADC   #<$07BC
      LDY   #0
      STA   (NVR1L),Y
      LDA   #>$07BC
      ADC   #0
      INY
      STA   (NVR1L),Y
      LDY   #3
      LDA   (NVR0L),Y
      AND   #1
      ASL
      ASL
      ASL
      STA   NVR2L
      DEY
      LDA   (NVR0L),Y
      LSR
      LSR
      LSR
      LSR
      LSR
      ORA   NVR2L
      LDY   #2
      JSR   dos_store_dt_word
      LDY   #2
      LDA   (NVR0L),Y
      AND   #$1F
      LDY   #4
      JSR   dos_store_dt_word
      LDY   #1
      LDA   (NVR0L),Y
      LSR
      LSR
      LSR
      LDY   #6
      JSR   dos_store_dt_word
      LDY   #1
      LDA   (NVR0L),Y
      AND   #7
      ASL
      ASL
      ASL
      STA   NVR2L
      LDY   #0
      LDA   (NVR0L),Y
      LSR
      LSR
      LSR
      LSR
      LSR
      ORA   NVR2L
      LDY   #8
      JSR   dos_store_dt_word
      LDY   #0
      LDA   (NVR0L),Y
      AND   #$1F
      ASL
      LDY   #10
      JSR   dos_store_dt_word
      JMP   dos_ok
dos_store_dt_word:
      STA   (NVR1L),Y
      INY
      LDA   #0
      STA   (NVR1L),Y
      RTS

      .export fexpand
fexpand:
      STA   dos_source_ptr
      STX   dos_source_ptr+1
      STZ   dos_result
      STZ   dos_path_kind
      STA   NVR0L
      STX   NVR0H
      LDY   #0
      LDA   (NVR0L),Y
      STA   dos_path_len
      BEQ   @need_pwd
      INY
      LDA   (NVR0L),Y
      CMP   #'/'
      BEQ   @rooted
      CMP   #$5C
      BEQ   @rooted
@scan_colon:
      CPY   dos_path_len
      BCC   @scan_char
      BEQ   @scan_char
      BRA   @need_pwd
@scan_char:
      LDA   (NVR0L),Y
      CMP   #':'
      BEQ   @absolute
      INY
      BRA   @scan_colon
@rooted:
      LDA   #1
      STA   dos_path_kind
      BRA   @need_pwd
@absolute:
      LDA   #2
      STA   dos_path_kind
      BRA   @copy_source
@need_pwd:
      LDA   #FILE_PWD
      JSR   dos_file_call
      LDA   LIB_STATUS
      BNE   @failed
      LDX   #0
      LDY   #0
@pwd:
      CPY   FIO_NAMELEN
      BCS   @pwd_done
      LDA   FIO_NAME,Y
      JSR   dos_append_char
      INY
      LDA   dos_path_kind
      CMP   #1
      BNE   @pwd
      LDA   FIO_NAME-1,Y
      CMP   #':'
      BNE   @pwd
@pwd_done:
      LDA   dos_path_kind
      BNE   @copy_source
      CPX   #0
      BEQ   @append_slash
      LDA   dos_result,X
      CMP   #'/'
      BEQ   @copy_source
@append_slash:
      LDA   #'/'
      JSR   dos_append_char
@copy_source:
      LDA   dos_source_ptr
      STA   NVR0L
      LDA   dos_source_ptr+1
      STA   NVR0H
      LDX   dos_result
      LDY   #1
@source:
      CPY   dos_path_len
      BCC   @source_char
      BEQ   @source_char
      BRA   @expanded
@source_char:
      LDA   (NVR0L),Y
      JSR   dos_append_char
      INY
      BRA   @source
@expanded:
      JSR   dos_ok
      LDA   #<dos_result
      LDX   #>dos_result
      RTS
@failed:
      STZ   dos_result
      JSR   dos_map_fio_error
      LDA   #<dos_result
      LDX   #>dos_result
      RTS

dos_append_char:
      CPX   #79
      BCS   @done
      CMP   #$5C
      BNE   @slash_ok
      LDA   #'/'
@slash_ok:
      CMP   #'a'
      BCC   @store
      CMP   #'z'+1
      BCS   @store
      AND   #$DF
@store:
      INX
      STA   dos_result,X
      STX   dos_result
@done:
      RTS

      .export fsplit
fsplit:
      TSX
      LDA   $0109,X
      STA   dos_source_ptr
      LDA   $010A,X
      STA   dos_source_ptr+1
      LDA   $0107,X
      STA   dos_dir_ptr
      LDA   $0108,X
      STA   dos_dir_ptr+1
      LDA   $0105,X
      STA   dos_name_ptr
      LDA   $0106,X
      STA   dos_name_ptr+1
      LDA   $0103,X
      STA   dos_ext_ptr
      LDA   $0104,X
      STA   dos_ext_ptr+1
      LDA   dos_source_ptr
      STA   NVR0L
      LDA   dos_source_ptr+1
      STA   NVR0H
      LDY   #0
      LDA   (NVR0L),Y
      STA   dos_path_len
      STZ   dos_dir_end
      STZ   dos_ext_start
      LDY   #1
@scan:
      CPY   dos_path_len
      BCC   @scan_char
      BEQ   @scan_char
      BRA   @split
@scan_char:
      LDA   (NVR0L),Y
      CMP   #'/'
      BEQ   @directory
      CMP   #$5C
      BEQ   @directory
      CMP   #':'
      BEQ   @directory
      CMP   #'.'
      BNE   @next
      STY   dos_ext_start
      BRA   @next
@directory:
      STY   dos_dir_end
      STZ   dos_ext_start
@next:
      INY
      BRA   @scan
@split:
      LDA   #1
      STA   dos_copy_start
      LDA   dos_dir_end
      STA   dos_copy_count
      LDA   #67
      STA   dos_copy_limit
      LDA   dos_dir_ptr
      STA   NVR1L
      LDA   dos_dir_ptr+1
      STA   NVR1H
      JSR   dos_copy_slice

      LDA   dos_dir_end
      INC
      STA   dos_copy_start
      LDA   dos_ext_start
      BEQ   @name_to_end
      SEC
      SBC   dos_copy_start
      BRA   @name_count
@name_to_end:
      LDA   dos_path_len
      SEC
      SBC   dos_dir_end
@name_count:
      STA   dos_copy_count
      LDA   #8
      STA   dos_copy_limit
      LDA   dos_name_ptr
      STA   NVR1L
      LDA   dos_name_ptr+1
      STA   NVR1H
      JSR   dos_copy_slice

      LDA   dos_ext_start
      STA   dos_copy_start
      BEQ   @no_ext
      LDA   dos_path_len
      SEC
      SBC   dos_ext_start
      INC
      BRA   @ext_count
@no_ext:
      LDA   #0
@ext_count:
      STA   dos_copy_count
      LDA   #4
      STA   dos_copy_limit
      LDA   dos_ext_ptr
      STA   NVR1L
      LDA   dos_ext_ptr+1
      STA   NVR1H
      JSR   dos_copy_slice
      JMP   dos_ok

dos_copy_slice:
      CLC
      LDA   dos_source_ptr
      ADC   dos_copy_start
      STA   NVR0L
      LDA   dos_source_ptr+1
      ADC   #0
      STA   NVR0H
      LDX   #0
@copy:
      CPX   dos_copy_count
      BCS   @done
      CPX   dos_copy_limit
      BCS   @done
      LDY   #0
      LDA   (NVR0L),Y
      INC   NVR0L
      BNE   @ptr_ok
      INC   NVR0H
@ptr_ok:
      STA   NVR2L
      TXA
      TAY
      INY
      LDA   NVR2L
      STA   (NVR1L),Y
      INX
      BRA   @copy
@done:
      LDY   #0
      TXA
      STA   (NVR1L),Y
      RTS

      .segment "RODATA"
dos_fd0: .byte "FD0"
dos_fd1: .byte "FD1"
dos_fd2: .byte "FD2"
dos_fd3: .byte "FD3"
dos_hd0: .byte "HD0"
dos_hd1: .byte "HD1"
dos_drive_lo: .byte <dos_fd0, <dos_fd1, <dos_fd2, <dos_fd3, <dos_hd0, <dos_hd1
dos_drive_hi: .byte >dos_fd0, >dos_fd1, >dos_fd2, >dos_fd3, >dos_hd0, >dos_hd1

.endif
.endif

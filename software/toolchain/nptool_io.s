; Raw file I/O shared by Nova disk-tool executables.

      .setcpu "w65c02"
      .include "nova.inc"
      .include "libfiles.inc"
      .include "nptool.inc"

NPTOOL_IO_SHORT      = $FD

.ifdef NPTOOL_BIN
      .segment "HEADER"
      .word NPTOOL_LOAD
.endif

      .segment "ZEROPAGE"
nptool_io_ptr:
io_name_ptr:    .res 2
io_name_len:    .res 1
nptool_io_aux:
io_file_id:     .res 2
io_saved_error: .res 1
nptool_io_left:
io_left:        .res 2

      .segment "CODE"
      .export nptool_load_arg0
      .export nptool_load_arg2
      .export nptool_load_arg3
      .export nptool_load_arg4
      .export nptool_save_arg0
      .export nptool_save_arg1
      .export nptool_save_named
      .export nptool_validate_text
      .export nptool_print_z
      .export nptool_newline
      .exportzp nptool_io_ptr
      .exportzp nptool_io_aux
      .exportzp nptool_io_left
.ifdef NPTOOL_BIN
      .export nptool_clear_bss
      .import __BSS_RUN__, __BSS_SIZE__

nptool_clear_bss:
      LDA   #<__BSS_RUN__
      STA   io_name_ptr
      LDA   #>__BSS_RUN__
      STA   io_name_ptr+1
      LDA   #<__BSS_SIZE__
      STA   io_left
      LDA   #>__BSS_SIZE__
      STA   io_left+1
@loop:
      LDA   io_left
      ORA   io_left+1
      BEQ   @done
      LDY   #0
      LDA   #0
      STA   (io_name_ptr),Y
      INC   io_name_ptr
      BNE   :+
      INC   io_name_ptr+1
:     LDA   io_left
      BNE   :+
      DEC   io_left+1
:     DEC   io_left
      BRA   @loop
@done:
      RTS
.endif

; A/X -> NUL-terminated string.
nptool_print_z:
      STA   io_name_ptr
      STX   io_name_ptr+1
      LDY   #0
@loop:
      LDA   (io_name_ptr),Y
      BEQ   @done
      STA   VGC_CHAROUT
      INC   io_name_ptr
      BNE   @loop
      INC   io_name_ptr+1
      BRA   @loop
@done:
      RTS

nptool_newline:
      LDA   #$0D
      STA   VGC_CHAROUT
      LDA   #$0A
      STA   VGC_CHAROUT
      RTS

; NPTOOL_IO_ADDR/LEN -> A=0 for editable ASCII text, A=1 for binary data.
nptool_validate_text:
      LDA   NPTOOL_IO_ADDR+0
      STA   io_name_ptr
      LDA   NPTOOL_IO_ADDR+1
      STA   io_name_ptr+1
      LDA   NPTOOL_IO_LEN+0
      STA   io_left+0
      LDA   NPTOOL_IO_LEN+1
      STA   io_left+1
@next:
      LDA   io_left+0
      ORA   io_left+1
      BEQ   @text
      LDY   #0
      LDA   (io_name_ptr),Y
      CMP   #$09
      BEQ   @accepted
      CMP   #$0A
      BEQ   @accepted
      CMP   #$0D
      BEQ   @accepted
      CMP   #$20
      BCC   @binary
      CMP   #$7F
      BCS   @binary
@accepted:
      INC   io_name_ptr
      BNE   :+
      INC   io_name_ptr+1
:     LDA   io_left+0
      BNE   :+
      DEC   io_left+1
:     DEC   io_left+0
      BRA   @next
@text:
      LDA   #0
      RTS
@binary:
      LDA   #1
      RTS

nptool_load_arg0:
      LDA   #<NPTOOL_ARG0
      STA   io_name_ptr
      LDA   #>NPTOOL_ARG0
      STA   io_name_ptr+1
      LDA   NPTOOL_ARG0_LEN
      STA   io_name_len
      BRA   nptool_load

nptool_load_arg2:
      LDA   #<NPTOOL_ARG2
      STA   io_name_ptr
      LDA   #>NPTOOL_ARG2
      STA   io_name_ptr+1
      LDA   NPTOOL_ARG2_LEN
      STA   io_name_len
      BRA   nptool_load

nptool_load_arg3:
      LDA   #<NPTOOL_ARG3
      STA   io_name_ptr
      LDA   #>NPTOOL_ARG3
      STA   io_name_ptr+1
      LDA   NPTOOL_ARG3_LEN
      STA   io_name_len
      BRA   nptool_load

nptool_load_arg4:
      LDA   #<NPTOOL_ARG4
      STA   io_name_ptr
      LDA   #>NPTOOL_ARG4
      STA   io_name_ptr+1
      LDA   NPTOOL_ARG4_LEN
      STA   io_name_len
nptool_load:
      JSR   io_open_read
      BEQ   :+
      JMP   io_fail
:
      JSR   io_file_size
      BEQ   :+
      JMP   io_fail_close
:
      LDA   LIB_RESULT+2
      ORA   LIB_RESULT+3
      BNE   @too_large
      LDA   LIB_RESULT+1
      CMP   NPTOOL_IO_CAP+1
      BCC   @fits
      BNE   @too_large
      LDA   LIB_RESULT+0
      CMP   NPTOOL_IO_CAP+0
      BCC   @fits
      BEQ   @fits
@too_large:
      LDA   #NPTOOL_IO_TOO_LARGE
      STA   io_saved_error
      JMP   io_fail_close_saved
@fits:
      LDA   LIB_RESULT+0
      STA   NPTOOL_IO_LEN+0
      LDA   LIB_RESULT+1
      STA   NPTOOL_IO_LEN+1
      ORA   NPTOOL_IO_LEN+0
      BEQ   @close
      JSR   io_clear_args
      LDA   io_file_id+0
      STA   LIB_ARG0+0
      LDA   io_file_id+1
      STA   LIB_ARG0+1
      LDA   NPTOOL_IO_ADDR+0
      STA   LIB_ARG1+0
      LDA   NPTOOL_IO_ADDR+1
      STA   LIB_ARG1+1
      LDA   NPTOOL_IO_LEN+0
      STA   LIB_ARG2+0
      LDA   NPTOOL_IO_LEN+1
      STA   LIB_ARG2+1
      LDA   #FILE_FREAD
      JSR   io_call
      BEQ   :+
      JMP   io_fail_close
:
      LDA   LIB_RESULT+0
      CMP   NPTOOL_IO_LEN+0
      BNE   @short
      LDA   LIB_RESULT+1
      CMP   NPTOOL_IO_LEN+1
      BNE   @short
@close:
      JSR   io_close
      BEQ   :+
      JMP   io_fail
:
      LDA   #0
      RTS
@short:
      LDA   #NPTOOL_IO_SHORT
      STA   io_saved_error
      JMP   io_fail_close_saved

nptool_save_arg0:
      LDA   #<NPTOOL_ARG0
      LDX   #>NPTOOL_ARG0
      LDY   NPTOOL_ARG0_LEN
      BRA   nptool_save_named

nptool_save_arg1:
      LDA   #<NPTOOL_ARG1
      LDX   #>NPTOOL_ARG1
      LDY   NPTOOL_ARG1_LEN

nptool_save_named:
      STA   io_name_ptr
      STX   io_name_ptr+1
      STY   io_name_len

io_save:
      JSR   io_open_write
      BEQ   :+
      JMP   io_fail
:
      LDA   NPTOOL_IO_LEN+0
      ORA   NPTOOL_IO_LEN+1
      BEQ   @close
      JSR   io_clear_args
      LDA   io_file_id+0
      STA   LIB_ARG0+0
      LDA   io_file_id+1
      STA   LIB_ARG0+1
      LDA   NPTOOL_IO_ADDR+0
      STA   LIB_ARG1+0
      LDA   NPTOOL_IO_ADDR+1
      STA   LIB_ARG1+1
      LDA   NPTOOL_IO_LEN+0
      STA   LIB_ARG2+0
      LDA   NPTOOL_IO_LEN+1
      STA   LIB_ARG2+1
      LDA   #FILE_FWRITE
      JSR   io_call
      BEQ   :+
      JMP   io_fail_close
:
      LDA   LIB_RESULT+0
      CMP   NPTOOL_IO_LEN+0
      BNE   @short
      LDA   LIB_RESULT+1
      CMP   NPTOOL_IO_LEN+1
      BNE   @short
@close:
      JSR   io_close
      BEQ   :+
      JMP   io_fail
:
      LDA   #0
      RTS
@short:
      LDA   #NPTOOL_IO_SHORT
      STA   io_saved_error
      JMP   io_fail_close_saved

io_open_read:
      LDA   #FIO_FILE_ACCESS_READ
      BRA   io_open

io_open_write:
      LDA   #FIO_FILE_ACCESS_WRITE

io_open:
      PHA
      JSR   io_clear_args
      LDA   io_name_ptr+0
      STA   LIB_ARG0+0
      LDA   io_name_ptr+1
      STA   LIB_ARG0+1
      LDA   io_name_len
      STA   LIB_ARG1+0
      PLA
      STA   LIB_ARG2+0
      LDA   LIB_ARG2+0
      CMP   #FIO_FILE_ACCESS_READ
      BNE   :+
      LDA   #FILE_FOPEN
      BRA   @call
:     LDA   #FILE_FCREATE
@call:
      JSR   io_call
      BNE   @done
      LDA   LIB_RESULT+0
      STA   io_file_id+0
      LDA   LIB_RESULT+1
      STA   io_file_id+1
      LDA   #0
@done:
      RTS

io_file_size:
      JSR   io_clear_args
      LDA   io_file_id+0
      STA   LIB_ARG0+0
      LDA   io_file_id+1
      STA   LIB_ARG0+1
      LDA   #FILE_FSIZE
      JMP   io_call

io_close:
      JSR   io_clear_args
      LDA   io_file_id+0
      STA   LIB_ARG0+0
      LDA   io_file_id+1
      STA   LIB_ARG0+1
      LDA   #FILE_FCLOSE
      JMP   io_call

io_clear_args:
      LDX   #15
@loop:
      STZ   LIB_ARG0,X
      DEX
      BPL   @loop
      RTS

io_call:
      STA   LIB_FN_ID
      LDA   #MODULE_ID_FILES
      STA   LIB_MOD_ID
      JSR   LIB_LOADER_BAND
      LDA   LIB_STATUS
      RTS

io_fail_close:
      JSR   io_capture_error
io_fail_close_saved:
      JSR   io_close
      BRA   io_set_saved_error

io_fail:
      JSR   io_capture_error
io_set_saved_error:
      LDA   io_saved_error
      STA   NPTOOL_DETAIL
      LDA   #NPTOOL_ERR_IO
      STA   NPTOOL_STATUS
      LDA   #1
      RTS

io_capture_error:
      LDA   FIO_ERRCODE
      BNE   :+
      LDA   LIB_STATUS
:     STA   io_saved_error
      RTS

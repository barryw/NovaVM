; NAS-owned project option reader. The frontend provides project_buf and
; project_left; output reuses the ordinary -D mailbox consumed by NASPP.

nas_project_options:
      LDA   NPTOOL_ARG4_LEN
      BNE   :+
      JMP   @ok
:
      LDA   #<project_buf
      STA   NPTOOL_IO_ADDR
      LDA   #>project_buf
      STA   NPTOOL_IO_ADDR+1
      LDA   #<PROJECT_CAP
      STA   NPTOOL_IO_CAP
      LDA   #>PROJECT_CAP
      STA   NPTOOL_IO_CAP+1
      JSR   nptool_load_arg4
      BEQ   :+
      JMP   @io_error
:
      JSR   nptool_validate_text
      BEQ   :+
      JMP   @bad
:
      STZ   NPTOOL_ARG6_LEN
      STZ   project_found
      LDA   #<project_buf
      STA   nptool_io_ptr
      LDA   #>project_buf
      STA   nptool_io_ptr+1
      LDA   NPTOOL_IO_LEN
      STA   project_left
      LDA   NPTOOL_IO_LEN+1
      STA   project_left+1
@line:
      JSR   project_skip_horizontal
      JSR   project_peek
      BCS   :+
      JMP   @ok
:
      JSR   project_upper
      CMP   #'D'
      BNE   @skip_line
      LDY   #0
@keyword:
      LDA   project_define_kw,Y
      BEQ   @keyword_done
      STA   project_expected
      PHY
      JSR   project_next
      PLY
      BCC   @bad
      JSR   project_upper
      CMP   project_expected
      BNE   @skip_line
      INY
      BRA   @keyword
@keyword_done:
      JSR   project_peek
      BCC   @bad
      CMP   #' '
      BEQ   :+
      CMP   #$09
      BNE   @skip_line
:
      LDA   project_found
      BNE   @bad
      INC   project_found
      JSR   project_skip_horizontal
      LDX   #0
@copy:
      JSR   project_peek
      BCC   @copied
      CMP   #$0D
      BEQ   @copied
      CMP   #$0A
      BEQ   @copied
      CPX   #NPTOOL_ARG_CAP-1
      BCS   @bad
      JSR   project_next
      STA   NPTOOL_ARG6,X
      INX
      BRA   @copy
@copied:
      CPX   #0
      BEQ   @bad
@trim:
      LDA   NPTOOL_ARG6-1,X
      CMP   #' '
      BEQ   @trim_one
      CMP   #$09
      BNE   @terminated
@trim_one:
      DEX
      BEQ   @bad
      BRA   @trim
@terminated:
      STX   NPTOOL_ARG6_LEN
      STZ   NPTOOL_ARG6,X
@skip_line:
      JSR   project_next
      BCC   @ok
      CMP   #$0A
      BNE   @skip_line
      JMP   @line
@io_error:
      RTS
@bad:
      LDA   #NPTOOL_ERR_ASSEMBLE
      STA   NPTOOL_STATUS
      LDA   #NASM_ERR_SYNTAX
      STA   NPTOOL_DETAIL
      LDA   #1
      RTS
@ok:
      LDA   #0
      RTS

project_skip_horizontal:
@again:
      JSR   project_peek
      BCC   @done
      CMP   #' '
      BEQ   @eat
      CMP   #$09
      BNE   @done
@eat:
      JSR   project_next
      BRA   @again
@done:
      RTS

project_peek:
      LDA   project_left
      ORA   project_left+1
      BEQ   @eof
      LDY   #0
      LDA   (nptool_io_ptr),Y
      SEC
      RTS
@eof:
      CLC
      RTS

project_next:
      JSR   project_peek
      BCC   @eof
      PHA
      INC   nptool_io_ptr
      BNE   :+
      INC   nptool_io_ptr+1
:
      LDA   project_left
      BNE   :+
      DEC   project_left+1
:
      DEC   project_left
      PLA
      SEC
@eof:
      RTS

project_upper:
      CMP   #'a'
      BCC   @done
      CMP   #'z'+1
      BCS   @done
      AND   #$DF
@done:
      RTS

project_define_kw: .byte "DEFINE", 0

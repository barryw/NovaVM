; NL executable frontend: one NOBJ + one NLIB -> $7000 Nova binary.

      .setcpu "w65c02"
      .include "novalink.inc"
      .include "nptool.inc"

OBJECT_CAP  = 1024
LIBRARY_CAP = 1024
OUTPUT_CAP  = 1024
OUTPUT_BASE = $7000

      .segment "BSS"
object_buf: .res OBJECT_CAP
library_buf:.res LIBRARY_CAP
output_buf: .res OUTPUT_CAP

      .segment "CODE"
      .export tool_main

tool_main:
      JSR   nptool_clear_bss
      STZ   NPTOOL_STATUS
      STZ   NPTOOL_DETAIL
      LDA   #<nl_banner
      LDX   #>nl_banner
      JSR   nptool_print_z
      LDA   NPTOOL_ARG0_LEN
      BNE   :+
      JMP   tool_bad_args
:
      LDA   NPTOOL_ARG1_LEN
      BNE   :+
      JMP   tool_bad_args
:
      LDA   NPTOOL_ARG2_LEN
      BNE   :+
      JMP   tool_bad_args
:
      LDA   #<nl_linking
      LDX   #>nl_linking
      JSR   nptool_print_z
      LDA   #<NPTOOL_ARG0
      LDX   #>NPTOOL_ARG0
      JSR   nptool_print_z
      JSR   nptool_newline
      LDA   #<nl_library
      LDX   #>nl_library
      JSR   nptool_print_z
      LDA   #<NPTOOL_ARG2
      LDX   #>NPTOOL_ARG2
      JSR   nptool_print_z
      JSR   nptool_newline
      LDA   #<object_buf
      STA   NPTOOL_IO_ADDR+0
      LDA   #>object_buf
      STA   NPTOOL_IO_ADDR+1
      LDA   #<OBJECT_CAP
      STA   NPTOOL_IO_CAP+0
      LDA   #>OBJECT_CAP
      STA   NPTOOL_IO_CAP+1
      JSR   nptool_load_arg0
      BEQ   :+
      JMP   @fail
:
      LDA   NPTOOL_IO_LEN+0
      STA   nlink_object_len+0
      LDA   NPTOOL_IO_LEN+1
      STA   nlink_object_len+1

      LDA   #<library_buf
      STA   NPTOOL_IO_ADDR+0
      LDA   #>library_buf
      STA   NPTOOL_IO_ADDR+1
      LDA   #<LIBRARY_CAP
      STA   NPTOOL_IO_CAP+0
      LDA   #>LIBRARY_CAP
      STA   NPTOOL_IO_CAP+1
      JSR   nptool_load_arg2
      BEQ   :+
      JMP   @fail
:
      LDA   NPTOOL_IO_LEN+0
      STA   nlink_library_len+0
      LDA   NPTOOL_IO_LEN+1
      STA   nlink_library_len+1

      LDA   #<object_buf
      STA   nlink_object_ptr+0
      LDA   #>object_buf
      STA   nlink_object_ptr+1
      LDA   #<library_buf
      STA   nlink_library_ptr+0
      LDA   #>library_buf
      STA   nlink_library_ptr+1
      LDA   #<output_buf
      STA   nlink_output_ptr+0
      LDA   #>output_buf
      STA   nlink_output_ptr+1
      LDA   #<OUTPUT_CAP
      STA   nlink_output_cap+0
      LDA   #>OUTPUT_CAP
      STA   nlink_output_cap+1
      LDA   #<OUTPUT_BASE
      STA   nlink_load_base+0
      LDA   #>OUTPUT_BASE
      STA   nlink_load_base+1
      JSR   nlink_link
      BNE   @link_error

      LDA   #<output_buf
      STA   NPTOOL_IO_ADDR+0
      LDA   #>output_buf
      STA   NPTOOL_IO_ADDR+1
      LDA   nlink_output_len+0
      STA   NPTOOL_IO_LEN+0
      LDA   nlink_output_len+1
      STA   NPTOOL_IO_LEN+1
      LDA   #<nl_writing
      LDX   #>nl_writing
      JSR   nptool_print_z
      LDA   #<NPTOOL_ARG1
      LDX   #>NPTOOL_ARG1
      JSR   nptool_print_z
      JSR   nptool_newline
      JSR   nptool_save_arg1
      BNE   @fail
      LDA   #<nl_ok
      LDX   #>nl_ok
      JSR   nptool_print_z
      LDA   #0
      RTS

@link_error:
      LDA   nlink_error
      STA   NPTOOL_DETAIL
      LDA   #NPTOOL_ERR_LINK
      STA   NPTOOL_STATUS
@fail:
      LDA   #1
      RTS

tool_bad_args:
      LDA   #NPTOOL_ERR_ARGS
      STA   NPTOOL_STATUS
      LDA   #1
      RTS

      .segment "RODATA"
nl_banner:  .byte "Nova Linker v1.0", $0D, $0A, 0
nl_linking: .byte "Linking ", 0
nl_library: .byte "Library ", 0
nl_writing: .byte "Writing ", 0
nl_ok:      .byte "Link successful", $0D, $0A, 0

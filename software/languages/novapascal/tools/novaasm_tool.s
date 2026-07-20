; NAS executable frontend: textual 65C02 assembly -> NOBJ.

      .setcpu "w65c02"
      .include "novaasm.inc"
      .include "nptool.inc"

ASM_CAP    = 3072
OBJECT_CAP = 1024

      .segment "BSS"
assembly_buf: .res ASM_CAP
object_buf:   .res OBJECT_CAP

      .segment "CODE"
      .export tool_main

tool_main:
      JSR   nptool_clear_bss
      STZ   NPTOOL_STATUS
      STZ   NPTOOL_DETAIL
      LDA   #<nas_banner
      LDX   #>nas_banner
      JSR   nptool_print_z
      LDA   NPTOOL_ARG0_LEN
      BNE   :+
      JMP   tool_bad_args
:
      LDA   NPTOOL_ARG1_LEN
      BNE   :+
      JMP   tool_bad_args
:
      LDA   #<nas_assembling
      LDX   #>nas_assembling
      JSR   nptool_print_z
      LDA   #<NPTOOL_ARG0
      LDX   #>NPTOOL_ARG0
      JSR   nptool_print_z
      JSR   nptool_newline
      LDA   #<assembly_buf
      STA   NPTOOL_IO_ADDR+0
      LDA   #>assembly_buf
      STA   NPTOOL_IO_ADDR+1
      LDA   #<ASM_CAP
      STA   NPTOOL_IO_CAP+0
      LDA   #>ASM_CAP
      STA   NPTOOL_IO_CAP+1
      JSR   nptool_load_arg0
      BNE   @fail

      LDA   #<assembly_buf
      STA   nasm_source_ptr+0
      LDA   #>assembly_buf
      STA   nasm_source_ptr+1
      LDA   NPTOOL_IO_LEN+0
      STA   nasm_source_len+0
      LDA   NPTOOL_IO_LEN+1
      STA   nasm_source_len+1
      LDA   #<object_buf
      STA   nasm_object_ptr+0
      LDA   #>object_buf
      STA   nasm_object_ptr+1
      LDA   #<OBJECT_CAP
      STA   nasm_object_cap+0
      LDA   #>OBJECT_CAP
      STA   nasm_object_cap+1
      JSR   nasm_assemble
      BNE   @assemble_error

      LDA   #<object_buf
      STA   NPTOOL_IO_ADDR+0
      LDA   #>object_buf
      STA   NPTOOL_IO_ADDR+1
      LDA   nasm_object_len+0
      STA   NPTOOL_IO_LEN+0
      LDA   nasm_object_len+1
      STA   NPTOOL_IO_LEN+1
      LDA   #<nas_writing
      LDX   #>nas_writing
      JSR   nptool_print_z
      LDA   #<NPTOOL_ARG1
      LDX   #>NPTOOL_ARG1
      JSR   nptool_print_z
      JSR   nptool_newline
      JSR   nptool_save_arg1
      BNE   @fail
      LDA   #<nas_ok
      LDX   #>nas_ok
      JSR   nptool_print_z
      LDA   #0
      RTS

@assemble_error:
      LDA   nasm_error
      STA   NPTOOL_DETAIL
      LDA   #NPTOOL_ERR_ASSEMBLE
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
nas_banner:     .byte "Nova Assembler v1.0", $0D, $0A, 0
nas_assembling: .byte "Assembling ", 0
nas_writing:    .byte "Writing ", 0
nas_ok:         .byte "Assembly successful", $0D, $0A, 0

; NovaPascal editor executable. Alt-X returns through tool_main to the shell.

      .setcpu "w65c02"
      .include "libeditor.inc"
      .include "libmemory.inc"
      .include "nptool.inc"

SOURCE_CAP = $1000
DOCUMENT_CAP = $FFFF

      .segment "BSS"
source_buf:         .res SOURCE_CAP
source_len:         .res 2
document_xaddr:     .res 3
document_allocated: .res 1
editor_type_ptr:    .res 2

      .segment "CODE"
      .export tool_main

tool_main:
      JSR   nptool_clear_bss
      STZ   NPTOOL_STATUS
      STZ   NPTOOL_DETAIL
      LDA   NPTOOL_ARG0_LEN
      BNE   :+
      JMP   tool_bad_args
:
      LDA   #<source_buf
      STA   NPTOOL_IO_ADDR+0
      LDA   #>source_buf
      STA   NPTOOL_IO_ADDR+1
      LDA   #<SOURCE_CAP
      STA   NPTOOL_IO_CAP+0
      LDA   #>SOURCE_CAP
      STA   NPTOOL_IO_CAP+1
      JSR   nptool_load_arg0
      BEQ   @loaded_ram
      LDA   NPTOOL_STATUS
      CMP   #NPTOOL_ERR_IO
      BEQ   :+
      JMP   @fail
:
      LDA   NPTOOL_DETAIL
      CMP   #NPTOOL_IO_TOO_LARGE
      BEQ   :+
      JMP   @fail
:
      LDA   NPTOOL_IO_LEN+0
      STA   source_len+0
      LDA   NPTOOL_IO_LEN+1
      STA   source_len+1
      STZ   NPTOOL_STATUS
      STZ   NPTOOL_DETAIL
      JSR   tool_alloc_document
      BEQ   :+
      JMP   @memory_error
:
      JSR   tool_load_document
      BEQ   :+
      JMP   @memory_error
:
      BRA   @edit

@loaded_ram:
      JSR   nptool_validate_text
      BEQ   :+
      LDA   #NPTOOL_ERR_NOT_TEXT
      STA   NPTOOL_STATUS
      JMP   @fail
:
      LDA   NPTOOL_IO_LEN+0
      STA   source_len+0
      LDA   NPTOOL_IO_LEN+1
      STA   source_len+1
      JSR   tool_alloc_document
      BEQ   :+
      JMP   @memory_error
:
      JSR   tool_copy_document
      BEQ   :+
      JMP   @memory_error
:

@edit:
      JSR   editor_select_type
      LDA   document_xaddr+0
      STA   LIB_ARG0+0
      LDA   document_xaddr+1
      STA   LIB_ARG0+1
      LDA   document_xaddr+2
      STA   LIB_ARG0+2
      STZ   LIB_ARG0+3
      LDA   source_len+0
      STA   LIB_ARG1+0
      LDA   source_len+1
      STA   LIB_ARG1+1
      LDA   #<source_buf
      STA   LIB_ARG1+2
      LDA   #>source_buf
      STA   LIB_ARG1+3
      LDA   #<DOCUMENT_CAP
      STA   LIB_ARG2+0
      LDA   #>DOCUMENT_CAP
      STA   LIB_ARG2+1
      LDA   #<SOURCE_CAP
      STA   LIB_ARG2+2
      LDA   #>SOURCE_CAP
      STA   LIB_ARG2+3
      LDA   #<NPTOOL_ARG0
      STA   LIB_ARG3+0
      LDA   #>NPTOOL_ARG0
      STA   LIB_ARG3+1
      LDA   editor_type_ptr+0
      STA   LIB_ARG3+2
      LDA   editor_type_ptr+1
      STA   LIB_ARG3+3
      LDA   #MODULE_ID_EDITOR
      STA   LIB_MOD_ID
      LDA   #EDITOR_FN_EDIT_XRAM
      STA   LIB_FN_ID
      JSR   LIB_LOADER_BAND
      LDA   LIB_STATUS
      BNE   @editor_error
      LDA   LIB_ARG1+0
      STA   source_len+0
      LDA   LIB_ARG1+1
      STA   source_len+1
      LDA   LIB_RESULT+1
      BEQ   @ok
      LDA   #1
      STA   NPTOOL_DETAIL
      LDA   source_len+0
      ORA   source_len+1
      BEQ   @save_empty
      JSR   tool_save_document
      BNE   @memory_error
      BRA   @ok
@save_empty:
      STZ   NPTOOL_IO_LEN+0
      STZ   NPTOOL_IO_LEN+1
      JSR   nptool_save_arg0
      BNE   @fail
@ok:
      JSR   tool_release_document
      LDA   #0
      RTS

@editor_error:
      CMP   #LERR_EDITOR_NOT_TEXT
      BNE   :+
      LDA   #NPTOOL_ERR_NOT_TEXT
      STA   NPTOOL_STATUS
      BRA   @fail
:
      STA   NPTOOL_DETAIL
      LDA   #NPTOOL_ERR_EDITOR
      STA   NPTOOL_STATUS
@fail:
      JSR   tool_release_document
      LDA   #1
      RTS

@memory_error:
      STA   NPTOOL_DETAIL
      LDA   #NPTOOL_ERR_MEMORY
      STA   NPTOOL_STATUS
      BRA   @fail

tool_bad_args:
      LDA   #NPTOOL_ERR_ARGS
      STA   NPTOOL_STATUS
      LDA   #1
      RTS

tool_clear_lib_args:
      LDX   #15
@loop:
      STZ   LIB_ARG0,X
      DEX
      BPL   @loop
      RTS

tool_mem_call:
      STA   LIB_FN_ID
      LDA   #MODULE_ID_MEMORY
      STA   LIB_MOD_ID
      JSR   LIB_LOADER_BAND
      LDA   LIB_STATUS
      RTS

tool_alloc_document:
      JSR   tool_clear_lib_args
      LDA   #<DOCUMENT_CAP
      STA   LIB_ARG2+0
      LDA   #>DOCUMENT_CAP
      STA   LIB_ARG2+1
      LDA   #MEM_ALLOC
      JSR   tool_mem_call
      BNE   @done
      LDX   #2
@copy:
      LDA   LIB_RESULT,X
      STA   document_xaddr,X
      DEX
      BPL   @copy
      INC   document_allocated
      LDA   #0
@done:
      RTS

tool_copy_document:
      LDA   source_len+0
      ORA   source_len+1
      BEQ   @done
      JSR   tool_clear_lib_args
      LDX   #2
@address:
      LDA   document_xaddr,X
      STA   LIB_ARG0,X
      DEX
      BPL   @address
      LDA   #<source_buf
      STA   LIB_ARG1+0
      LDA   #>source_buf
      STA   LIB_ARG1+1
      LDA   source_len+0
      STA   LIB_ARG2+0
      LDA   source_len+1
      STA   LIB_ARG2+1
      LDA   #MEM_COPY_FROM_RAM
      JMP   tool_mem_call
@done:
      LDA   #0
      RTS

tool_load_document:
      JSR   tool_clear_lib_args
      LDA   #<NPTOOL_ARG0
      STA   LIB_ARG0+0
      LDA   #>NPTOOL_ARG0
      STA   LIB_ARG0+1
      LDA   NPTOOL_ARG0_LEN
      STA   LIB_ARG1+0
      LDX   #2
@address:
      LDA   document_xaddr,X
      STA   LIB_ARG2,X
      DEX
      BPL   @address
      LDA   source_len+0
      STA   LIB_ARG3+0
      LDA   source_len+1
      STA   LIB_ARG3+1
      LDA   #MEM_XLOAD
      JMP   tool_mem_call

tool_save_document:
      JSR   tool_clear_lib_args
      LDA   #<NPTOOL_ARG0
      STA   LIB_ARG0+0
      LDA   #>NPTOOL_ARG0
      STA   LIB_ARG0+1
      LDA   NPTOOL_ARG0_LEN
      STA   LIB_ARG1+0
      LDX   #2
@address:
      LDA   document_xaddr,X
      STA   LIB_ARG2,X
      DEX
      BPL   @address
      LDA   source_len+0
      STA   LIB_ARG3+0
      LDA   source_len+1
      STA   LIB_ARG3+1
      LDA   #MEM_XSAVE
      JMP   tool_mem_call

tool_release_document:
      LDA   document_allocated
      BEQ   @done
      STZ   document_allocated
      JSR   tool_clear_lib_args
      LDX   #2
@address:
      LDA   document_xaddr,X
      STA   LIB_ARG0,X
      DEX
      BPL   @address
      LDA   #<DOCUMENT_CAP
      STA   LIB_ARG2+0
      LDA   #>DOCUMENT_CAP
      STA   LIB_ARG2+1
      LDA   #MEM_RELEASE
      JMP   tool_mem_call
@done:
      LDA   #0
      RTS

editor_set_type:
      STA   editor_type_ptr+0
      STX   editor_type_ptr+1
      RTS

editor_select_type:
      LDA   #<editor_type_text
      LDX   #>editor_type_text
      JSR   editor_set_type
      LDX   NPTOOL_ARG0_LEN
      CPX   #2
      BCC   @done
      DEX
      LDA   NPTOOL_ARG0,X
      AND   #$DF
      CMP   #'S'
      BNE   @three
      DEX
      LDA   NPTOOL_ARG0,X
      CMP   #'.'
      BNE   @three
      JMP   @assembly
@three:
      LDX   NPTOOL_ARG0_LEN
      CPX   #4
      BCC   @done
      DEX
      DEX
      DEX
      DEX
      LDA   NPTOOL_ARG0,X
      CMP   #'.'
      BNE   @done
      INX
      LDA   NPTOOL_ARG0,X
      AND   #$DF
      CMP   #'P'
      BEQ   @check_pascal
      CMP   #'N'
      BEQ   @check_project
      CMP   #'A'
      BEQ   @check_asm
      CMP   #'I'
      BEQ   @check_inc
      CMP   #'C'
      BEQ   @check_cfg
      CMP   #'L'
      BEQ   @check_lnk
@done:
      RTS
@check_lnk:
      INX
      LDA   NPTOOL_ARG0,X
      AND   #$DF
      CMP   #'N'
      BNE   @done
      INX
      LDA   NPTOOL_ARG0,X
      AND   #$DF
      CMP   #'K'
      BEQ   @linker
      BRA   @done
@check_cfg:
      INX
      LDA   NPTOOL_ARG0,X
      AND   #$DF
      CMP   #'F'
      BNE   @done
      INX
      LDA   NPTOOL_ARG0,X
      AND   #$DF
      CMP   #'G'
      BEQ   @linker
      BRA   @done
@check_pascal:
      INX
      LDA   NPTOOL_ARG0,X
      AND   #$DF
      CMP   #'A'
      BNE   @done
      INX
      LDA   NPTOOL_ARG0,X
      AND   #$DF
      CMP   #'S'
      BEQ   @pascal
      BRA   @done
@check_project:
      INX
      LDA   NPTOOL_ARG0,X
      AND   #$DF
      CMP   #'P'
      BNE   @done
      INX
      LDA   NPTOOL_ARG0,X
      AND   #$DF
      CMP   #'P'
      BEQ   @project
      BRA   @done
@check_asm:
      INX
      LDA   NPTOOL_ARG0,X
      AND   #$DF
      CMP   #'S'
      BNE   @done
      INX
      LDA   NPTOOL_ARG0,X
      AND   #$DF
      CMP   #'M'
      BEQ   @assembly
      BRA   @done
@check_inc:
      INX
      LDA   NPTOOL_ARG0,X
      AND   #$DF
      CMP   #'N'
      BNE   @done
      INX
      LDA   NPTOOL_ARG0,X
      AND   #$DF
      CMP   #'C'
      BEQ   @assembly
      JMP   @done
@linker:
      LDA   #<editor_type_linker
      LDX   #>editor_type_linker
      JMP   editor_set_type
@pascal:
      LDA   #<editor_type_pascal
      LDX   #>editor_type_pascal
      JMP   editor_set_type
@project:
      LDA   #<editor_type_project
      LDX   #>editor_type_project
      JMP   editor_set_type
@assembly:
      LDA   #<editor_type_assembly
      LDX   #>editor_type_assembly
      JMP   editor_set_type

      .segment "RODATA"
editor_type_text:     .byte "Text", 0
editor_type_pascal:   .byte "Pascal Source", 0
editor_type_project:  .byte "Pascal Project", 0
editor_type_assembly: .byte "Assembly Source", 0
editor_type_linker:   .byte "Linker Config", 0

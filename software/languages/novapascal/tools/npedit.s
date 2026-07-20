; NovaPascal editor executable. Alt-X returns through tool_main to the shell.

      .setcpu "w65c02"
      .include "libeditor.inc"
      .include "nptool.inc"

SOURCE_CAP = 2048

      .segment "BSS"
source_buf:   .res SOURCE_CAP
source_len:   .res 2
editor_hooks: .res EDITOR_HOOKS_SIZE

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
      BEQ   :+
      JMP   @fail
:
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

      JSR   editor_select_type
      LDA   #<source_buf
      STA   LIB_ARG0+0
      LDA   #>source_buf
      STA   LIB_ARG0+1
      STZ   LIB_ARG0+2
      STZ   LIB_ARG0+3
      LDA   source_len+0
      STA   LIB_ARG1+0
      LDA   source_len+1
      STA   LIB_ARG1+1
      STZ   LIB_ARG1+2
      STZ   LIB_ARG1+3
      LDA   #<SOURCE_CAP
      STA   LIB_ARG2+0
      LDA   #>SOURCE_CAP
      STA   LIB_ARG2+1
      STZ   LIB_ARG2+2
      STZ   LIB_ARG2+3
      LDA   #<NPTOOL_ARG0
      STA   LIB_ARG3+0
      LDA   #>NPTOOL_ARG0
      STA   LIB_ARG3+1
      LDA   #<editor_hooks
      STA   LIB_ARG3+2
      LDA   #>editor_hooks
      STA   LIB_ARG3+3
      LDA   #MODULE_ID_EDITOR
      STA   LIB_MOD_ID
      LDA   #EDITOR_FN_EDIT
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
      LDA   #<source_buf
      STA   NPTOOL_IO_ADDR+0
      LDA   #>source_buf
      STA   NPTOOL_IO_ADDR+1
      LDA   source_len+0
      STA   NPTOOL_IO_LEN+0
      LDA   source_len+1
      STA   NPTOOL_IO_LEN+1
      JSR   nptool_save_arg0
      BNE   @fail
@ok:
      LDA   #0
      RTS

@editor_error:
      STA   NPTOOL_DETAIL
      LDA   #NPTOOL_ERR_EDITOR
      STA   NPTOOL_STATUS
@fail:
      LDA   #1
      RTS

tool_bad_args:
      LDA   #NPTOOL_ERR_ARGS
      STA   NPTOOL_STATUS
      LDA   #1
      RTS

editor_set_type:
      STA   editor_hooks+EDITOR_HOOKS_TYPEL
      STX   editor_hooks+EDITOR_HOOKS_TYPEH
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

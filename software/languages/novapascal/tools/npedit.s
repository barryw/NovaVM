; NovaPascal editor executable. Alt-X returns through tool_main to the shell.

      .setcpu "w65c02"
      .include "nova.inc"
      .include "editbuf.inc"
      .include "editui.inc"
      .include "libeditor.inc"
      .include "libfiles.inc"
      .include "libmemory.inc"
      .include "libsystem.inc"
      .include "nui.inc"
      .include "nptool.inc"
      .include "longbranch.inc"

SOURCE_CAP = $1000
DOCUMENT_CAP = $FFFF
PICKER_NAME_CAP = 64
PICKER_ROWS_SIZE = 24 * 36

      .segment "BSS"
source_len:         .res 2
document_xaddr:     .res 3
document_allocated: .res 1
editor_type_ptr:    .res 2
editor_hooks:       .res EDITOR_HOOKS_SIZE
editor_hl_color:    .res 1
editor_exit:        .res 1
editor_command_action:.res 1
editor_saved:       .res 1
picker_name:        .res PICKER_NAME_CAP
picker_name_len:    .res 1
picker_cwd:         .res PICKER_NAME_CAP
picker_cwd_len:     .res 1
picker_rows:        .res PICKER_ROWS_SIZE

      .segment "NOINIT"
source_buf:         .res SOURCE_CAP

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
      STZ   editor_command_action
      JSR   editor_select_type
      LDA   editor_type_ptr+0
      STA   editor_hooks+EDITOR_HOOKS_TYPEL
      LDA   editor_type_ptr+1
      STA   editor_hooks+EDITOR_HOOKS_TYPEH
      LDA   #<editor_command_hook
      STA   editor_hooks+EDITOR_HOOKS_COMMAND_VECL
      LDA   #>editor_command_hook
      STA   editor_hooks+EDITOR_HOOKS_COMMAND_VECH
      LDA   document_xaddr+0
      STA   LIB_ARG0+0
      LDA   document_xaddr+1
      STA   LIB_ARG0+1
      LDA   document_xaddr+2
      STA   LIB_ARG0+2
      LDA   #EDITOR_XRAM_FLAG_HOOKS
      STA   LIB_ARG0+3
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
      LDA   #<editor_hooks
      STA   LIB_ARG3+2
      LDA   #>editor_hooks
      STA   LIB_ARG3+3
      LDA   #MODULE_ID_EDITOR
      STA   LIB_MOD_ID
      LDA   #EDITOR_FN_EDIT_XRAM
      STA   LIB_FN_ID
      JSR   LIB_LOADER_BAND
      LDA   LIB_STATUS
      BNE   @editor_error
      LDA   LIB_RESULT+0
      STA   editor_exit
      LDA   LIB_ARG1+0
      STA   source_len+0
      LDA   LIB_ARG1+1
      STA   source_len+1
      LDA   LIB_RESULT+1
      BEQ   @after_save
      LDA   #1
      STA   editor_saved
      LDA   source_len+0
      ORA   source_len+1
      BEQ   @save_empty
      JSR   tool_save_document
      BNE   @memory_error
      BRA   @after_save
@save_empty:
      STZ   NPTOOL_IO_LEN+0
      STZ   NPTOOL_IO_LEN+1
      JSR   nptool_save_arg0
      BNE   @fail
@after_save:
      LDA   editor_exit
      CMP   #EDITBUF_EXIT_COMMAND
      BNE   @ok
      LDA   editor_command_action
      BEQ   @ok
      JSR   tool_pick_file
      CMP   #2
      BEQ   @fail
      CMP   #1
      BEQ   @reload
      JSR   tool_use_picker_name
@reload:
      JSR   tool_load_current_document
      BNE   @fail
      JMP   @edit
@ok:
      LDA   editor_saved
      STA   NPTOOL_DETAIL
      JSR   tool_release_document
      BNE   @memory_error
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

; Replace the allocated XRAM document with the text file named by ARG0.
tool_load_current_document:
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
      BNE   @failed
      LDA   NPTOOL_DETAIL
      CMP   #NPTOOL_IO_TOO_LARGE
      BNE   @failed
      LDA   NPTOOL_IO_LEN+0
      STA   source_len+0
      LDA   NPTOOL_IO_LEN+1
      STA   source_len+1
      STZ   NPTOOL_STATUS
      STZ   NPTOOL_DETAIL
      JSR   tool_load_document
      BEQ   @ok
      BRA   @memory_error
@loaded_ram:
      JSR   nptool_validate_text
      BNE   @not_text
      LDA   NPTOOL_IO_LEN+0
      STA   source_len+0
      LDA   NPTOOL_IO_LEN+1
      STA   source_len+1
      JSR   tool_copy_document
      BEQ   @ok
@memory_error:
      STA   NPTOOL_DETAIL
      LDA   #NPTOOL_ERR_MEMORY
      STA   NPTOOL_STATUS
      LDA   #1
      RTS
@not_text:
      LDA   #NPTOOL_ERR_NOT_TEXT
      STA   NPTOOL_STATUS
@failed:
      LDA   #1
      RTS
@ok:
      LDA   #0
      RTS

tool_save_picker_cwd:
      JSR   tool_clear_lib_args
      LDA   #MODULE_ID_FILES
      STA   LIB_MOD_ID
      LDA   #FILE_PWD
      STA   LIB_FN_ID
      JSR   LIB_LOADER_BAND
      LDA   LIB_STATUS
      BNE   @done
      LDA   FIO_NAMELEN
      CMP   #PICKER_NAME_CAP
      BCS   @bad
      STA   picker_cwd_len
      TAX
      STZ   picker_cwd,X
@copy:
      DEX
      BMI   @ok
      LDA   FIO_NAME,X
      STA   picker_cwd,X
      BRA   @copy
@ok:
      LDA   #0
@done:
      RTS
@bad:
      LDA   #LERR_FIO_FAIL
      STA   LIB_STATUS
      RTS

tool_restore_picker_cwd:
      JSR   tool_clear_lib_args
      LDA   #<picker_cwd
      STA   LIB_ARG0+0
      LDA   #>picker_cwd
      STA   LIB_ARG0+1
      LDA   picker_cwd_len
      STA   LIB_ARG1+0
      LDA   #MODULE_ID_FILES
      STA   LIB_MOD_ID
      LDA   #FILE_CD
      STA   LIB_FN_ID
      JSR   LIB_LOADER_BAND
      LDA   LIB_STATUS
      RTS

; Return A=0 for a selected file, 1 for cancel, 2 for an NDK failure.
tool_pick_file:
      JSR   tool_save_picker_cwd
      BNE   @error
      JSR   tool_clear_lib_args
      LDA   #<picker_config
      STA   LIB_ARG0+0
      LDA   #>picker_config
      STA   LIB_ARG0+1
      LDA   #MODULE_ID_SYSTEM
      STA   LIB_MOD_ID
      LDA   #SYS_NUI_FILE_PICKER
      STA   LIB_FN_ID
      JSR   LIB_LOADER_BAND
      LDA   LIB_STATUS
      BNE   @restore_error
      LDA   LIB_RESULT+1
      CMP   #NUI_RESULT_CANCEL
      BEQ   @cancel
      LDA   LIB_RESULT+0
      BEQ   @restore_error
      STA   picker_name_len
      LDA   #0
      RTS
@cancel:
      JSR   tool_restore_picker_cwd
      BNE   @error
      LDA   #1
      RTS
@restore_error:
      LDA   LIB_STATUS
      BNE   :+
      LDA   #LERR_SYS_FAIL
:     STA   NPTOOL_DETAIL
      JSR   tool_restore_picker_cwd
      BRA   @publish_error
@error:
      LDA   LIB_STATUS
      STA   NPTOOL_DETAIL
@publish_error:
      LDA   #NPTOOL_ERR_EDITOR
      STA   NPTOOL_STATUS
      LDA   #2
      RTS

tool_use_picker_name:
      LDA   picker_name_len
      STA   NPTOOL_ARG0_LEN
      TAX
      STZ   NPTOOL_ARG0,X
@copy:
      DEX
      BMI   @done
      LDA   picker_name,X
      STA   NPTOOL_ARG0,X
      BRA   @copy
@done:
      RTS

editor_command_hook:
      CMP   #EDITUI_CMD_OPEN
      BEQ   @open
      CMP   #EDITUI_CMD_BUFFER_LIST
      BNE   @ignored
@open:
      STA   editor_command_action
      LDA   #EDITBUF_EXIT_COMMAND
      STA   EDITOR_HOOK_ABI_RESULT
      SEC
      RTS
@ignored:
      CLC
      RTS

editor_set_type:
      STA   editor_type_ptr+0
      STX   editor_type_ptr+1
      RTS

editor_select_type:
      STZ   editor_hooks+EDITOR_HOOKS_HILITE_VECL
      STZ   editor_hooks+EDITOR_HOOKS_HILITE_VECH
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
      LDA   #<editor_pascal_hilite
      STA   editor_hooks+EDITOR_HOOKS_HILITE_VECL
      LDA   #>editor_pascal_hilite
      STA   editor_hooks+EDITOR_HOOKS_HILITE_VECH
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

; Pascal lexer hook. The shared editor owns rendering; NPEDIT only assigns
; lexical colors for Pascal identifiers, literals, strings, and comments.
editor_pascal_hilite:
      LDY   #0
@next:
      CPY   EDITOR_HOOK_ABI_HL_LEN
      long_bcs @done
      LDA   (EDITOR_HOOK_ABI_HL_PTRL),Y
      CMP   #$7B
      long_beq @brace_comment
      CMP   #'('
      BEQ   @maybe_paren_comment
      CMP   #'/'
      BEQ   @maybe_line_comment
      CMP   #$27
      long_beq @string
      CMP   #'$'
      long_beq @number
      JSR   editor_pascal_is_digit
      long_bcs @number
      JSR   editor_pascal_is_alpha
      long_bcs @word
      INY
      BRA   @next

@maybe_line_comment:
      INY
      CPY   EDITOR_HOOK_ABI_HL_LEN
      long_bcs @done
      LDA   (EDITOR_HOOK_ABI_HL_PTRL),Y
      CMP   #'/'
      BNE   @single_char
      DEY
      LDA   #EDITOR_PASCAL_COLOR_COMMENT
      JMP   editor_pascal_color_to_eol

@maybe_paren_comment:
      INY
      CPY   EDITOR_HOOK_ABI_HL_LEN
      long_bcs @done
      LDA   (EDITOR_HOOK_ABI_HL_PTRL),Y
      CMP   #'*'
      BNE   @single_char
      DEY
      LDA   #EDITOR_PASCAL_COLOR_COMMENT
      STA   editor_hl_color
@paren_loop:
      CPY   EDITOR_HOOK_ABI_HL_LEN
      long_bcs @done
      LDA   editor_hl_color
      STA   EDITOR_HOOK_ABI_HL_COLORS,Y
      LDA   (EDITOR_HOOK_ABI_HL_PTRL),Y
      INY
      CMP   #'*'
      BNE   @paren_loop
      CPY   EDITOR_HOOK_ABI_HL_LEN
      long_bcs @done
      LDA   (EDITOR_HOOK_ABI_HL_PTRL),Y
      CMP   #'/'
      BNE   @paren_loop
      LDA   editor_hl_color
      STA   EDITOR_HOOK_ABI_HL_COLORS,Y
      INY
      JMP   @next

@single_char:
      DEY
      INY
      JMP   @next

@brace_comment:
      LDA   #EDITOR_PASCAL_COLOR_COMMENT
      STA   editor_hl_color
@brace_loop:
      LDA   editor_hl_color
      STA   EDITOR_HOOK_ABI_HL_COLORS,Y
      LDA   (EDITOR_HOOK_ABI_HL_PTRL),Y
      INY
      CMP   #'}'
      long_beq @next
      CPY   EDITOR_HOOK_ABI_HL_LEN
      BCC   @brace_loop
      RTS

@string:
      LDA   #EDITOR_PASCAL_COLOR_STRING
      STA   editor_hl_color
      STA   EDITOR_HOOK_ABI_HL_COLORS,Y
      INY
@string_loop:
      CPY   EDITOR_HOOK_ABI_HL_LEN
      BCS   @done
      LDA   editor_hl_color
      STA   EDITOR_HOOK_ABI_HL_COLORS,Y
      LDA   (EDITOR_HOOK_ABI_HL_PTRL),Y
      INY
      CMP   #$27
      BNE   @string_loop
      CPY   EDITOR_HOOK_ABI_HL_LEN
      BCS   @done
      LDA   (EDITOR_HOOK_ABI_HL_PTRL),Y
      CMP   #$27
      long_bne @next
      LDA   editor_hl_color
      STA   EDITOR_HOOK_ABI_HL_COLORS,Y
      INY
      BRA   @string_loop

@word:
      LDA   #EDITOR_PASCAL_COLOR_WORD
      STA   editor_hl_color
@word_loop:
      CPY   EDITOR_HOOK_ABI_HL_LEN
      BCS   @done
      LDA   (EDITOR_HOOK_ABI_HL_PTRL),Y
      JSR   editor_pascal_is_ident
      long_bcc @next
      LDA   editor_hl_color
      STA   EDITOR_HOOK_ABI_HL_COLORS,Y
      INY
      BRA   @word_loop

@number:
      LDA   #EDITOR_PASCAL_COLOR_NUMBER
      STA   editor_hl_color
@number_loop:
      CPY   EDITOR_HOOK_ABI_HL_LEN
      BCS   @done
      LDA   (EDITOR_HOOK_ABI_HL_PTRL),Y
      CMP   #'$'
      BEQ   @number_color
      JSR   editor_pascal_is_ident
      long_bcc @next
@number_color:
      LDA   editor_hl_color
      STA   EDITOR_HOOK_ABI_HL_COLORS,Y
      INY
      BRA   @number_loop
@done:
      RTS

editor_pascal_color_to_eol:
      STA   editor_hl_color
@loop:
      CPY   EDITOR_HOOK_ABI_HL_LEN
      BCS   @done
      LDA   editor_hl_color
      STA   EDITOR_HOOK_ABI_HL_COLORS,Y
      INY
      BRA   @loop
@done:
      RTS

editor_pascal_is_ident:
      CMP   #'_'
      BEQ   @yes
      JSR   editor_pascal_is_digit
      BCS   @yes
      JMP   editor_pascal_is_alpha
@yes:
      SEC
      RTS

editor_pascal_is_alpha:
      CMP   #'A'
      BCC   @lower
      CMP   #'Z' + 1
      BCC   @yes
@lower:
      CMP   #'a'
      BCC   @no
      CMP   #'z' + 1
      BCC   @yes
@yes:
      SEC
      RTS
@no:
      CLC
      RTS
editor_pascal_is_digit:
      CMP   #'0'
      BCC   @no
      CMP   #'9' + 1
      BCS   @no
      SEC
      RTS
@no:
      CLC
      RTS

      .segment "RODATA"
EDITOR_PASCAL_COLOR_WORD    = $63
EDITOR_PASCAL_COLOR_STRING  = $65
EDITOR_PASCAL_COLOR_NUMBER  = $67
EDITOR_PASCAL_COLOR_COMMENT = $6C
editor_type_text:     .byte "Text", 0
editor_type_pascal:   .byte "Pascal Source", 0
editor_type_project:  .byte "Pascal Project", 0
editor_type_assembly: .byte "Assembly Source", 0
editor_type_linker:   .byte "Linker Config", 0
picker_title:         .byte "Open Text File", 0
picker_footer:        .byte "Enter Open   Esc Cancel", 0
picker_config:
      .word picker_title
      .word picker_name
      .byte PICKER_NAME_CAP, NUI_FILE_MODE_OPEN, 0, 0, 0, 0, 0, 0
      .word picker_footer
      .word picker_rows

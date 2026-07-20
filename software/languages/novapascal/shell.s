; =====================================================================
; NovaPascal native command shell.
;
; This file is included by the resident $C000 shell/compiler ROM. Commands
; load the editor, NAS, and NL standard binaries into the $4000 tool slot.
; =====================================================================

SHELL_NAME_CAP = 64

      .segment "ZEROPAGE"
shell_scan:       .res 1
shell_name_len:   .res 1
shell_name2_len:  .res 1
shell_io_len:     .res 1
shell_file_id:    .res 2
shell_file_len:   .res 2
shell_fio_error:  .res 1
shell_load_error: .res 1
shell_token_len:  .res 1
shell_args_error: .res 1
shell_tool_result:.res 1
shell_tool_load_error_code:.res 1

      .segment "BSS"
shell_name:           .res SHELL_NAME_CAP
shell_name2:          .res SHELL_NAME_CAP
shell_project_output: .res SHELL_NAME_CAP
shell_project_out_len:.res 1

      .segment "CODE"
      .export repl_line_complete

.macro shell_try word, handler
      LDA   #<word
      STA   p_word
      LDA   #>word
      STA   p_word+1
      JSR   shell_match
      BCC   :+
      JMP   handler
:
.endmacro

shell_start:
      LDA   #$0C
      STA   VGC_CHAROUT
      JSR   repl_init
      LDA   #<shell_banner
      STA   p_word
      LDA   #>shell_banner
      STA   p_word+1
      JSR   print_z

shell_loop:
      LDA   #<shell_prompt
      STA   p_word
      LDA   #>shell_prompt
      STA   p_word+1
      JSR   print_z
      JSR   repl_read_line
      LDA   buf_idx
      BEQ   shell_loop

      shell_try shell_kw_help,     shell_cmd_help
      shell_try shell_kw_dir,      shell_cmd_dir
      shell_try shell_kw_pwd,      shell_cmd_pwd
      shell_try shell_kw_cd,       shell_cmd_cd
      shell_try shell_kw_type,     shell_cmd_type
      shell_try shell_kw_edit,     shell_cmd_edit
      shell_try shell_kw_del,      shell_cmd_del
      shell_try shell_kw_ren,      shell_cmd_ren
      shell_try shell_kw_mkdir,    shell_cmd_mkdir
      shell_try shell_kw_rmdir,    shell_cmd_rmdir
      shell_try shell_kw_compile,  shell_cmd_compile
      shell_try shell_kw_assemble, shell_cmd_assemble
      shell_try shell_kw_link,     shell_cmd_link
      shell_try shell_kw_build,    shell_cmd_build
      shell_try shell_kw_run,      shell_cmd_run

      LDA   #<shell_unknown
      STA   p_word
      LDA   #>shell_unknown
      STA   p_word+1
      JSR   print_z
      JMP   shell_loop

; Pascal and shell commands are always complete after one line.
repl_line_complete:
      LDA   #0
      RTS

; Compare input_buf's first token with the uppercase Z string at p_word.
; Carry set means matched; shell_scan points to the first argument.
shell_match:
      LDX   #0
@skip:
      LDA   input_buf,X
      CMP   #' '
      BEQ   @eat
      CMP   #$09
      BNE   @word
@eat:
      INX
      BRA   @skip
@word:
      LDY   #0
@compare:
      LDA   (p_word),Y
      BEQ   @boundary
      STA   p_expected
      LDA   input_buf,X
      JSR   shell_upper
      CMP   p_expected
      BNE   @no
      INX
      INY
      BRA   @compare
@boundary:
      LDA   input_buf,X
      BEQ   @yes
      CMP   #' '
      BEQ   @arg_skip
      CMP   #$09
      BNE   @no
@arg_skip:
      INX
      LDA   input_buf,X
      CMP   #' '
      BEQ   @arg_skip
      CMP   #$09
      BEQ   @arg_skip
@yes:
      STX   shell_scan
      SEC
      RTS
@no:
      CLC
      RTS

shell_upper:
      CMP   #'a'
      BCC   @done
      CMP   #'z'+1
      BCS   @done
      AND   #$DF
@done:
      RTS

; Parse up to two whitespace-delimited filename arguments.
shell_read_args:
      STZ   shell_name_len
      STZ   shell_name2_len
      STZ   shell_args_error
      LDX   shell_scan
      LDY   #0
@first:
      LDA   input_buf,X
      BEQ   @first_done
      CMP   #' '
      BEQ   @first_done
      CMP   #$09
      BEQ   @first_done
      CPY   #SHELL_NAME_CAP-1
      BCC   @first_store
      INC   shell_args_error
@first_discard:
      INX
      LDA   input_buf,X
      BEQ   @first_done
      CMP   #' '
      BEQ   @first_done
      CMP   #$09
      BNE   @first_discard
      BRA   @first_done
@first_store:
      STA   shell_name,Y
      INX
      INY
      BRA   @first
@first_done:
      STY   shell_name_len
      LDA   #0
      STA   shell_name,Y
@skip:
      LDA   input_buf,X
      CMP   #' '
      BEQ   @skip_one
      CMP   #$09
      BNE   @second_start
@skip_one:
      INX
      BRA   @skip
@second_start:
      LDY   #0
@second:
      LDA   input_buf,X
      BEQ   @done
      CMP   #' '
      BEQ   @done
      CMP   #$09
      BEQ   @done
      CPY   #SHELL_NAME_CAP-1
      BCC   @second_store
      INC   shell_args_error
@second_discard:
      INX
      LDA   input_buf,X
      BEQ   @done
      CMP   #' '
      BEQ   @done
      CMP   #$09
      BNE   @second_discard
      BRA   @done
@second_store:
      STA   shell_name2,Y
      INX
      INY
      BRA   @second
@done:
      STY   shell_name2_len
      LDA   #0
      STA   shell_name2,Y
      LDA   shell_args_error
      BEQ   @valid
      SEC
      RTS
@valid:
      CLC
      RTS

shell_require_name:
      JSR   shell_read_args
      BCS   @bad
      LDA   shell_name_len
      BNE   @ok
@bad:
      LDA   #<shell_usage
      STA   p_word
      LDA   #>shell_usage
      STA   p_word+1
      JSR   print_z
      SEC
      RTS
@ok:
      CLC
      RTS

shell_cmd_help:
      LDA   #<shell_help
      STA   p_word
      LDA   #>shell_help
      STA   p_word+1
      JSR   print_z
      JMP   shell_loop

shell_cmd_dir:
      JSR   shell_read_args
      BCS   @usage
      LDA   #FILE_DIR_LIST
      JSR   shell_call_file_name
      JSR   shell_check_lib
      JMP   shell_loop
@usage:
      LDA   #<shell_usage
      STA   p_word
      LDA   #>shell_usage
      STA   p_word+1
      JSR   print_z
      JMP   shell_loop

shell_cmd_pwd:
      STZ   shell_name_len
      LDA   #FILE_PWD
      JSR   shell_call_file_name
      LDA   LIB_STATUS
      BNE   @error
      LDY   #0
@print:
      CPY   FIO_NAMELEN
      BCS   @done
      LDA   FIO_NAME,Y
      STA   VGC_CHAROUT
      INY
      BRA   @print
@done:
      JSR   shell_newline
      JMP   shell_loop
@error:
      JSR   shell_print_file_error
      JMP   shell_loop

shell_cmd_cd:
      JSR   shell_require_name
      BCS   @done
      LDA   #FILE_CD
      JSR   shell_call_file_name
      JSR   shell_check_lib
@done:
      JMP   shell_loop

shell_cmd_del:
      JSR   shell_require_name
      BCS   @done
      LDA   #FILE_FDELETE
      JSR   shell_call_file_name
      JSR   shell_check_lib
@done:
      JMP   shell_loop

shell_cmd_mkdir:
      JSR   shell_require_name
      BCS   @done
      LDA   #FILE_MKDIR
      JSR   shell_call_file_name
      JSR   shell_check_lib
@done:
      JMP   shell_loop

shell_cmd_rmdir:
      JSR   shell_require_name
      BCS   @done
      LDA   #FILE_RMDIR
      JSR   shell_call_file_name
      JSR   shell_check_lib
@done:
      JMP   shell_loop

shell_cmd_ren:
      JSR   shell_read_args
      BCS   @usage
      LDA   shell_name_len
      BEQ   @usage
      LDA   shell_name2_len
      BEQ   @usage
      STZ   shell_load_error
      STZ   shell_fio_error
      LDA   #MODULE_ID_FILES
      STA   LIB_MOD_ID
      LDA   #FILE_FRENAME
      STA   LIB_FN_ID
      LDA   #<shell_name
      STA   LIB_ARG0
      LDA   #>shell_name
      STA   LIB_ARG0+1
      LDA   shell_name_len
      STA   LIB_ARG1
      LDA   #<shell_name2
      STA   LIB_ARG2
      LDA   #>shell_name2
      STA   LIB_ARG2+1
      LDA   shell_name2_len
      STA   LIB_ARG3
      JSR   LIB_LOADER_BAND
      JSR   shell_check_lib
      JMP   shell_loop
@usage:
      LDA   #<shell_usage
      STA   p_word
      LDA   #>shell_usage
      STA   p_word+1
      JSR   print_z
      JMP   shell_loop

shell_cmd_type:
      JSR   shell_require_name
      BCS   @done
      LDA   #<source_buf
      STA   p_copy_dst
      LDA   #>source_buf
      STA   p_copy_dst+1
      LDA   #<SHELL_BUFFER_CAP
      STA   p_out_left
      LDA   #>SHELL_BUFFER_CAP
      STA   p_out_left+1
      JSR   shell_load_shell_name
      BNE   @file_error
      LDA   #<source_buf
      STA   NPTOOL_IO_ADDR+0
      LDA   #>source_buf
      STA   NPTOOL_IO_ADDR+1
      LDA   shell_file_len+0
      STA   NPTOOL_IO_LEN+0
      LDA   shell_file_len+1
      STA   NPTOOL_IO_LEN+1
      JSR   nptool_validate_text
      BNE   @not_text
      LDA   #<source_buf
      STA   p_src
      LDA   #>source_buf
      STA   p_src+1
      LDA   shell_file_len
      STA   p_left
      LDA   shell_file_len+1
      STA   p_left+1
      JSR   shell_print_buffer
      JSR   shell_newline
      BRA   @done
@not_text:
      LDA   #<shell_not_text
      STA   p_word
      LDA   #>shell_not_text
      STA   p_word+1
      JSR   print_z
      BRA   @done
@file_error:
      JSR   shell_print_file_error
@done:
      JMP   shell_loop

shell_cmd_edit:
      JSR   shell_require_name
      BCS   @done
      STZ   shell_name2_len
      JSR   shell_prepare_tool_args
      LDA   #<shell_tool_editor
      STA   p_word
      LDA   #>shell_tool_editor
      STA   p_word+1
      LDA   #shell_tool_editor_end-shell_tool_editor
      JSR   shell_launch_tool
      BCS   @tool_error
      LDA   NPTOOL_DETAIL
      BEQ   @done
      LDA   #<shell_saved
      STA   p_word
      LDA   #>shell_saved
      STA   p_word+1
      JSR   print_z
      BRA   @done
@tool_error:
      JSR   shell_print_tool_error
@done:
      JMP   shell_loop

shell_cmd_compile:
      JSR   shell_require_name
      BCS   @done
      LDA   #<shell_ext_s
      STA   p_word
      LDA   #>shell_ext_s
      STA   p_word+1
      JSR   shell_derive_name
      JSR   shell_prepare_tool_args
      JSR   npc_compile_file
      BNE   @tool_error
      BRA   @done
@tool_error:
      JSR   shell_print_tool_error
@done:
      JMP   shell_loop

shell_cmd_assemble:
      JSR   shell_require_name
      BCS   @done
      LDA   #<shell_ext_obj
      STA   p_word
      LDA   #>shell_ext_obj
      STA   p_word+1
      JSR   shell_derive_name
      JSR   shell_prepare_tool_args
      LDA   #<shell_tool_assembler
      STA   p_word
      LDA   #>shell_tool_assembler
      STA   p_word+1
      LDA   #shell_tool_assembler_end-shell_tool_assembler
      JSR   shell_launch_tool
      BCS   @tool_error
      BRA   @done
@tool_error:
      JSR   shell_print_tool_error
@done:
      JMP   shell_loop

shell_cmd_link:
      JSR   shell_require_name
      BCS   @done
      LDA   #<shell_ext_bin
      STA   p_word
      LDA   #>shell_ext_bin
      STA   p_word+1
      JSR   shell_derive_name
      JSR   shell_prepare_tool_args
      LDA   #<shell_tool_linker
      STA   p_word
      LDA   #>shell_tool_linker
      STA   p_word+1
      LDA   #shell_tool_linker_end-shell_tool_linker
      JSR   shell_launch_tool
      BCS   @tool_error
      BRA   @done
@tool_error:
      JSR   shell_print_tool_error
@done:
      JMP   shell_loop

shell_cmd_build:
      JSR   shell_require_name
      BCC   :+
      JMP   @done
:
      STZ   shell_project_out_len
      JSR   shell_name_is_npp
      BCC   @source_ready
      JSR   shell_load_project
      BEQ   :+
      JMP   @file_error
:
      JSR   shell_parse_project
      BCC   @source_ready
      JMP   @project_error
@source_ready:
      LDA   #<shell_ext_s
      STA   p_word
      LDA   #>shell_ext_s
      STA   p_word+1
      JSR   shell_derive_name
      JSR   shell_prepare_tool_args
      JSR   npc_compile_file
      BNE   @tool_error

      JSR   shell_promote_name2
      LDA   #<shell_ext_obj
      STA   p_word
      LDA   #>shell_ext_obj
      STA   p_word+1
      JSR   shell_derive_name
      JSR   shell_prepare_tool_args
      LDA   #<shell_tool_assembler
      STA   p_word
      LDA   #>shell_tool_assembler
      STA   p_word+1
      LDA   #shell_tool_assembler_end-shell_tool_assembler
      JSR   shell_launch_tool
      BCS   @tool_error

      JSR   shell_promote_name2
      LDA   shell_project_out_len
      BEQ   @default_output
      JSR   shell_use_project_output
      BRA   @link
@default_output:
      LDA   #<shell_ext_bin
      STA   p_word
      LDA   #>shell_ext_bin
      STA   p_word+1
      JSR   shell_derive_name
@link:
      JSR   shell_prepare_tool_args
      LDA   #<shell_tool_linker
      STA   p_word
      LDA   #>shell_tool_linker
      STA   p_word+1
      LDA   #shell_tool_linker_end-shell_tool_linker
      JSR   shell_launch_tool
      BCS   @tool_error
      LDA   #<shell_build_ok
      STA   p_word
      LDA   #>shell_build_ok
      STA   p_word+1
      JSR   print_z
      JSR   shell_print_name2
      JSR   shell_newline
      BRA   @done
@file_error:
      JSR   shell_print_file_error
      BRA   @done
@tool_error:
      JSR   shell_print_tool_error
      BRA   @done
@project_error:
      LDA   #<shell_project_error
      STA   p_word
      LDA   #>shell_project_error
      STA   p_word+1
      JSR   print_z
@done:
      JMP   shell_loop

shell_cmd_run:
      JSR   shell_require_name
      BCS   @done
      LDA   #<source_buf
      STA   p_copy_dst
      LDA   #>source_buf
      STA   p_copy_dst+1
      LDA   #<SHELL_BUFFER_CAP
      STA   p_out_left
      LDA   #>SHELL_BUFFER_CAP
      STA   p_out_left+1
      JSR   shell_load_shell_name
      BNE   @file_error
      LDA   shell_file_len+1
      BNE   @size_ok
      LDA   shell_file_len
      CMP   #3
      BCC   @bad_binary
@size_ok:
      LDA   source_buf
      CMP   #<OUTPUT_BASE
      BNE   @bad_binary
      LDA   source_buf+1
      CMP   #>OUTPUT_BASE
      BNE   @bad_binary
      JSR   shell_copy_program
      LDA   #<shell_running
      STA   p_word
      LDA   #>shell_running
      STA   p_word+1
      JSR   print_z
      JSR   OUTPUT_BASE
      BRA   @done
@bad_binary:
      LDA   #<shell_binary_error
      STA   p_word
      LDA   #>shell_binary_error
      STA   p_word+1
      JSR   print_z
      BRA   @done
@file_error:
      JSR   shell_print_file_error
@done:
      JMP   shell_loop

; ---------------------------------------------------------------------
; External tool execution.
; ---------------------------------------------------------------------
shell_load_project:
      LDA   #<source_buf
      STA   p_copy_dst
      LDA   #>source_buf
      STA   p_copy_dst+1
      LDA   #<SHELL_BUFFER_CAP
      STA   p_out_left
      LDA   #>SHELL_BUFFER_CAP
      STA   p_out_left+1
      JMP   shell_load_shell_name

shell_prepare_tool_args:
      STZ   NPTOOL_STATUS
      STZ   NPTOOL_DETAIL
      LDA   shell_name_len
      STA   NPTOOL_ARG0_LEN
      LDA   shell_name2_len
      STA   NPTOOL_ARG1_LEN
      LDY   #0
@arg0:
      LDA   shell_name,Y
      STA   NPTOOL_ARG0,Y
      BEQ   @arg1_start
      INY
      BRA   @arg0
@arg1_start:
      LDY   #0
      LDA   shell_name2_len
      BEQ   @empty_arg1
@arg1:
      LDA   shell_name2,Y
      STA   NPTOOL_ARG1,Y
      BEQ   @done
      INY
      BRA   @arg1
@empty_arg1:
      STZ   NPTOOL_ARG1
@done:
      LDA   #shell_pascal_library_end-shell_pascal_library
      STA   NPTOOL_ARG2_LEN
      LDY   #0
@arg2:
      LDA   shell_pascal_library,Y
      STA   NPTOOL_ARG2,Y
      INY
      CPY   #shell_pascal_library_end-shell_pascal_library
      BCC   @arg2
      RTS

; p_word = executable filename, A = filename length.
shell_launch_tool:
      STA   shell_io_len
      JSR   shell_copy_fio_name
      BNE   @load_failed
      LDA   #<NPTOOL_LOAD
      STA   FIO_SRCL
      LDA   #>NPTOOL_LOAD
      STA   FIO_SRCH
      STZ   shell_tool_load_error_code
      JSR   fio_load
      BNE   @load_failed
      JSR   NPTOOL_LOAD
      STA   shell_tool_result
      BNE   @failed
      CLC
      RTS
@load_failed:
      LDA   FIO_ERRCODE
      STA   shell_tool_load_error_code
      SEC
      RTS
@failed:
      SEC
      RTS

shell_print_tool_error:
      LDA   NPTOOL_STATUS
      CMP   #NPTOOL_ERR_IO
      BEQ   @io
      CMP   #NPTOOL_ERR_COMPILE
      BEQ   @compile
      CMP   #NPTOOL_ERR_ASSEMBLE
      BEQ   @assemble
      CMP   #NPTOOL_ERR_LINK
      BEQ   @link
      CMP   #NPTOOL_ERR_EDITOR
      BEQ   @editor
      CMP   #NPTOOL_ERR_NOT_TEXT
      BEQ   @not_text
      CMP   #NPTOOL_ERR_ARGS
      BEQ   @args
      LDA   #<shell_tool_load_error
      STA   p_word
      LDA   #>shell_tool_load_error
      STA   p_word+1
      JSR   print_z
      LDA   shell_tool_load_error_code
      JSR   shell_print_hex
      JMP   shell_newline
@io:
      STZ   shell_load_error
      LDA   NPTOOL_DETAIL
      STA   shell_fio_error
      JMP   shell_print_file_error
@compile:
      LDA   #<shell_compile_error
      STA   p_word
      LDA   #>shell_compile_error
      STA   p_word+1
      JMP   print_z
@assemble:
      LDA   #<shell_assemble_error
      STA   p_word
      LDA   #>shell_assemble_error
      STA   p_word+1
      JMP   print_z
@link:
      LDA   #<shell_link_error
      STA   p_word
      LDA   #>shell_link_error
      STA   p_word+1
      JMP   print_z
@editor:
      LDA   #<shell_editor_error
      STA   p_word
      LDA   #>shell_editor_error
      STA   p_word+1
      JMP   print_z
@not_text:
      LDA   #<shell_not_text
      STA   p_word
      LDA   #>shell_not_text
      STA   p_word+1
      JMP   print_z
@args:
      LDA   #<shell_usage
      STA   p_word
      LDA   #>shell_usage
      STA   p_word+1
      JMP   print_z

shell_promote_name2:
      LDY   #0
@copy:
      LDA   shell_name2,Y
      STA   shell_name,Y
      BEQ   @done
      INY
      BRA   @copy
@done:
      STY   shell_name_len
      RTS

shell_copy_program:
      LDA   #<(source_buf+2)
      STA   p_src
      LDA   #>(source_buf+2)
      STA   p_src+1
      LDA   #<OUTPUT_BASE
      STA   p_copy_dst
      LDA   #>OUTPUT_BASE
      STA   p_copy_dst+1
      SEC
      LDA   shell_file_len+0
      SBC   #2
      STA   p_left
      LDA   shell_file_len+1
      SBC   #0
      STA   p_left+1
@copy:
      LDA   p_left
      ORA   p_left+1
      BEQ   @done
      LDY   #0
      LDA   (p_src),Y
      STA   (p_copy_dst),Y
      INC   p_src
      BNE   :+
      INC   p_src+1
:     INC   p_copy_dst
      BNE   :+
      INC   p_copy_dst+1
:     LDA   p_left
      BNE   :+
      DEC   p_left+1
:     DEC   p_left
      BRA   @copy
@done:
      RTS

; ---------------------------------------------------------------------
; NPP v1 parser.
;
; ponytail: one MAIN and one OUTPUT are enough for today's single-object
; linker. Add PASCAL/ASM source entries only with real symbols, relocations,
; and multi-object linking; silently concatenating files would be incorrect.
; ---------------------------------------------------------------------
p_expect_word:
      JSR   p_skip_ws
      LDY   #0
@loop:
      LDA   (p_word),Y
      BEQ   @boundary
      STA   p_expected
      JSR   p_read_upper
      BCC   @bad
      CMP   p_expected
      BNE   @bad
      INC   p_word
      BNE   @loop
      INC   p_word+1
      BRA   @loop
@boundary:
      JSR   p_peek_upper
      BCC   @ok
      JSR   p_is_ident
      BCS   @bad
@ok:
      CLC
      RTS
@bad:
      SEC
      RTS

p_expect_char:
      STA   p_expected
      JSR   p_skip_ws
      JSR   p_next
      BCC   @bad
      CMP   p_expected
      BNE   @bad
      CLC
      RTS
@bad:
      SEC
      RTS

p_skip_ws:
@loop:
      JSR   p_peek
      BCC   @done
      CMP   #' '
      BEQ   @eat
      CMP   #$09
      BEQ   @eat
      CMP   #$0D
      BEQ   @eat
      CMP   #$0A
      BNE   @done
@eat:
      JSR   p_next
      BRA   @loop
@done:
      RTS

p_peek_upper:
      JSR   p_peek
      BCC   @eof
      JSR   shell_upper
      SEC
      RTS
@eof:
      CLC
      RTS

p_read_upper:
      JSR   p_next
      BCC   @eof
      JSR   shell_upper
      SEC
      RTS
@eof:
      CLC
      RTS

p_is_ident:
      CMP   #'A'
      BCC   @underscore
      CMP   #'Z'+1
      BCC   @yes
@underscore:
      CMP   #'_'
      BEQ   @yes
      CMP   #'0'
      BCC   @no
      CMP   #'9'+1
      BCC   @yes
@no:
      CLC
      RTS
@yes:
      SEC
      RTS

p_peek:
      LDA   p_left
      ORA   p_left+1
      BEQ   @eof
      LDY   #0
      LDA   (p_src),Y
      SEC
      RTS
@eof:
      CLC
      RTS

p_next:
      JSR   p_peek
      BCC   @eof
      PHA
      INC   p_src
      BNE   :+
      INC   p_src+1
:     LDA   p_left
      BNE   :+
      DEC   p_left+1
:     DEC   p_left
      PLA
      SEC
@eof:
      RTS

shell_parse_project:
      LDA   #<source_buf
      STA   p_src
      LDA   #>source_buf
      STA   p_src+1
      LDA   shell_file_len
      STA   p_left
      LDA   shell_file_len+1
      STA   p_left+1

      LDA   #<shell_project_magic
      STA   p_word
      LDA   #>shell_project_magic
      STA   p_word+1
      JSR   p_expect_word
      BCS   @bad
      LDA   #'1'
      JSR   p_expect_char
      BCS   @bad
      LDA   #<shell_project_main_kw
      STA   p_word
      LDA   #>shell_project_main_kw
      STA   p_word+1
      JSR   p_expect_word
      BCS   @bad
      LDA   #<shell_name
      STA   p_copy_dst
      LDA   #>shell_name
      STA   p_copy_dst+1
      JSR   shell_project_token
      BCS   @bad
      LDA   shell_token_len
      STA   shell_name_len

      LDA   #<shell_project_output_kw
      STA   p_word
      LDA   #>shell_project_output_kw
      STA   p_word+1
      JSR   p_expect_word
      BCS   @bad
      LDA   #<shell_project_output
      STA   p_copy_dst
      LDA   #>shell_project_output
      STA   p_copy_dst+1
      JSR   shell_project_token
      BCS   @bad
      LDA   shell_token_len
      STA   shell_project_out_len
      JSR   p_skip_ws
      LDA   p_left
      ORA   p_left+1
      BNE   @bad
      CLC
      RTS
@bad:
      SEC
      RTS

shell_project_token:
      JSR   p_skip_ws
      STZ   shell_token_len
@loop:
      JSR   p_peek
      BCC   @done
      CMP   #' '
      BEQ   @done
      CMP   #$09
      BEQ   @done
      CMP   #$0D
      BEQ   @done
      CMP   #$0A
      BEQ   @done
      LDA   shell_token_len
      CMP   #SHELL_NAME_CAP-1
      BCS   @bad
      JSR   p_next
      LDY   shell_token_len
      STA   (p_copy_dst),Y
      INC   shell_token_len
      BRA   @loop
@done:
      LDA   shell_token_len
      BEQ   @bad
      LDY   shell_token_len
      LDA   #0
      STA   (p_copy_dst),Y
      CLC
      RTS
@bad:
      SEC
      RTS

shell_name_is_npp:
      LDA   shell_name_len
      CMP   #4
      BCC   @no
      SEC
      SBC   #4
      TAX
      LDA   shell_name,X
      CMP   #'.'
      BNE   @no
      INX
      LDA   shell_name,X
      JSR   shell_upper
      CMP   #'N'
      BNE   @no
      INX
      LDA   shell_name,X
      JSR   shell_upper
      CMP   #'P'
      BNE   @no
      INX
      LDA   shell_name,X
      JSR   shell_upper
      CMP   #'P'
      BNE   @no
      SEC
      RTS
@no:
      CLC
      RTS

shell_use_project_output:
      LDY   #0
@copy:
      LDA   shell_project_output,Y
      STA   shell_name2,Y
      BEQ   @done
      INY
      BRA   @copy
@done:
      STY   shell_name2_len
      RTS

; Replace shell_name's extension and write the result to shell_name2.
; p_word points at the new extension, including its dot.
shell_derive_name:
      LDY   #0
@base:
      CPY   shell_name_len
      BCS   @extension
      LDA   shell_name,Y
      CMP   #'.'
      BEQ   @extension
      STA   shell_name2,Y
      INY
      BRA   @base
@extension:
      STY   shell_scan
      LDY   #0
@ext:
      LDA   (p_word),Y
      BEQ   @done
      LDX   shell_scan
      CPX   #SHELL_NAME_CAP-1
      BCS   @done
      STA   shell_name2,X
      INC   shell_scan
      INY
      BRA   @ext
@done:
      LDY   shell_scan
      LDA   #0
      STA   shell_name2,Y
      STY   shell_name2_len
      RTS

; ---------------------------------------------------------------------
; Raw file loader for .NPP, TYPE, and $7000 binaries.
; ---------------------------------------------------------------------
shell_load_shell_name:
      LDA   #<shell_name
      STA   p_word
      LDA   #>shell_name
      STA   p_word+1
      LDA   shell_name_len
      STA   shell_io_len
      JMP   shell_load_raw

shell_copy_fio_name:
      LDA   shell_io_len
      STA   FIO_ARG_NAMELEN
      LDA   p_word
      STA   FIO_ARG_NAMEPTR_L
      LDA   p_word+1
      STA   FIO_ARG_NAMEPTR_H
      JMP   fio_copy_name

; Input: p_word/name length, p_copy_dst destination, p_out_left capacity.
; Output: shell_file_len and A=0 success / A=1 failure.
shell_load_raw:
      STZ   shell_load_error
      STZ   shell_fio_error
      JSR   shell_copy_fio_name
      BNE   @open_failed
      LDA   #FIO_FILE_ACCESS_READ
      STA   FIO_DIRTYPE
      JSR   fio_fopen
      BNE   @open_failed
      LDA   FIO_SRCL
      STA   shell_file_id
      LDA   FIO_SRCH
      STA   shell_file_id+1

      JSR   fio_fsize
      BNE   @io_failed
      LDA   FIO_SIZE2
      BNE   @too_large
      LDA   FIO_SIZEH
      CMP   p_out_left+1
      BCC   @fits
      BNE   @too_large
      LDA   FIO_SIZEL
      CMP   p_out_left
      BCC   @fits
      BEQ   @fits
      BRA   @too_large
@fits:
      LDA   FIO_SIZEL
      STA   shell_file_len
      LDA   FIO_SIZEH
      STA   shell_file_len+1
      ORA   shell_file_len
      BEQ   @close_ok
      LDA   shell_file_id
      STA   FIO_SRCL
      LDA   shell_file_id+1
      STA   FIO_SRCH
      LDA   p_copy_dst
      STA   FIO_ENDL
      LDA   p_copy_dst+1
      STA   FIO_ENDH
      LDA   shell_file_len
      STA   FIO_GLENL
      LDA   shell_file_len+1
      STA   FIO_GLENH
      LDA   #FIO_FILE_TARGET_RAM
      STA   FIO_DIRTYPE
      JSR   fio_fread
      BNE   @io_failed
@close_ok:
      JSR   shell_close_file
      RTS
@too_large:
      LDA   #1
      STA   shell_load_error
      BRA   @close_failed
@io_failed:
      LDA   FIO_ERRCODE
      STA   shell_fio_error
@close_failed:
      JSR   shell_close_file
      LDA   #1
      RTS
@open_failed:
      LDA   FIO_ERRCODE
      STA   shell_fio_error
      LDA   #1
      RTS

shell_close_file:
      LDA   shell_file_id
      STA   FIO_SRCL
      LDA   shell_file_id+1
      STA   FIO_SRCH
      JMP   fio_fclose

; ---------------------------------------------------------------------
; FILES-module and output helpers.
; ---------------------------------------------------------------------
shell_call_file_name:
      STA   LIB_FN_ID
      STZ   shell_load_error
      STZ   shell_fio_error
      LDA   #MODULE_ID_FILES
      STA   LIB_MOD_ID
      LDA   #<shell_name
      STA   LIB_ARG0
      LDA   #>shell_name
      STA   LIB_ARG0+1
      STZ   LIB_ARG0+2
      STZ   LIB_ARG0+3
      LDA   shell_name_len
      STA   LIB_ARG1
      STZ   LIB_ARG1+1
      STZ   LIB_ARG1+2
      STZ   LIB_ARG1+3
      JMP   LIB_LOADER_BAND

shell_check_lib:
      LDA   LIB_STATUS
      BEQ   @ok
      JMP   shell_print_file_error
@ok:
      RTS

shell_print_file_error:
      LDA   shell_load_error
      BEQ   @fio
      LDA   #<shell_too_large
      STA   p_word
      LDA   #>shell_too_large
      STA   p_word+1
      JMP   print_z
@fio:
      LDA   shell_fio_error
      BNE   :+
      LDA   FIO_ERRCODE
:     CMP   #FIO_ERR_NOTFOUND
      BEQ   @not_found
      CMP   #FIO_ERR_DISKFULL
      BEQ   @disk_full
      CMP   #FIO_ERR_NOTMOUNTED
      BEQ   @not_mounted
      LDA   #<shell_io_error
      STA   p_word
      LDA   #>shell_io_error
      STA   p_word+1
      JMP   print_z
@not_found:
      LDA   #<shell_not_found
      STA   p_word
      LDA   #>shell_not_found
      STA   p_word+1
      JSR   print_z
      LDA   #<shell_name
      STA   p_word
      LDA   #>shell_name
      STA   p_word+1
      JSR   print_z
      JMP   shell_newline
@disk_full:
      LDA   #<shell_disk_full
      STA   p_word
      LDA   #>shell_disk_full
      STA   p_word+1
      JMP   print_z
@not_mounted:
      LDA   #<shell_not_mounted
      STA   p_word
      LDA   #>shell_not_mounted
      STA   p_word+1
      JMP   print_z

shell_print_hex:
      PHA
      LSR
      LSR
      LSR
      LSR
      TAX
      LDA   hex_digits,X
      STA   VGC_CHAROUT
      PLA
      AND   #$0F
      TAX
      LDA   hex_digits,X
      STA   VGC_CHAROUT
      RTS

shell_print_buffer:
      LDA   p_left
      ORA   p_left+1
      BEQ   @done
      LDY   #0
      LDA   (p_src),Y
      STA   VGC_CHAROUT
      INC   p_src
      BNE   :+
      INC   p_src+1
:     LDA   p_left
      BNE   :+
      DEC   p_left+1
:     DEC   p_left
      BRA   shell_print_buffer
@done:
      RTS

shell_print_name2:
      LDY   #0
@loop:
      CPY   shell_name2_len
      BCS   @done
      LDA   shell_name2,Y
      STA   VGC_CHAROUT
      INY
      BRA   @loop
@done:
      RTS

shell_newline:
      LDA   #$0D
      STA   VGC_CHAROUT
      LDA   #$0A
      STA   VGC_CHAROUT
      RTS

      .segment "RODATA"
shell_banner:
      .byte "NovaPascal Shell v1.0", $0D, $0A
      .byte "Type HELP to get started", $0D, $0A, $0D, $0A, 0
shell_prompt: .byte "NP> ", 0
shell_unknown: .byte "Unknown command. Type HELP.", $0D, $0A, 0
shell_usage: .byte "Missing or invalid argument. Type HELP.", $0D, $0A, 0
shell_saved: .byte "Saved.", $0D, $0A, 0
shell_build_ok: .byte "Build complete: ", 0
shell_running: .byte "Running at $7000:", $0D, $0A, 0
shell_not_found: .byte "File not found: ", 0
shell_io_error: .byte "File I/O error.", $0D, $0A, 0
shell_disk_full: .byte "Disk full.", $0D, $0A, 0
shell_not_mounted: .byte "Drive not mounted.", $0D, $0A, 0
shell_too_large: .byte "File is too large for this command.", $0D, $0A, 0
shell_not_text: .byte "Not a text file.", $0D, $0A, 0
shell_editor_error: .byte "Editor error.", $0D, $0A, 0
shell_compile_error: .byte "Pascal syntax error.", $0D, $0A, 0
shell_assemble_error: .byte "Assembler error.", $0D, $0A, 0
shell_link_error: .byte "Linker error.", $0D, $0A, 0
shell_project_error: .byte "Invalid NPP 1 project file.", $0D, $0A, 0
shell_binary_error: .byte "Not a NovaPascal $7000 binary.", $0D, $0A, 0
shell_tool_load_error: .byte "Tool load error $", 0

shell_help:
      .byte "File commands:", $0D, $0A
      .byte "  DIR [mask]       List files", $0D, $0A
      .byte "  PWD              Show directory", $0D, $0A
      .byte "  CD dir           Change directory", $0D, $0A
      .byte "  TYPE file        Print ASCII text only", $0D, $0A
      .byte "  EDIT file        Edit source/project/config text", $0D, $0A
      .byte "  DEL file         Delete a file", $0D, $0A
      .byte "  REN old new      Rename a file", $0D, $0A
      .byte "  MKDIR dir        Create directory", $0D, $0A
      .byte "  RMDIR dir        Remove directory", $0D, $0A
      .byte "Build commands:", $0D, $0A
      .byte "  COMPILE file.pas Write file.S", $0D, $0A
      .byte "  ASSEMBLE file.s  Write file.OBJ", $0D, $0A
      .byte "  LINK file.obj    Write file.BIN", $0D, $0A
      .byte "  BUILD file.pas   Compile, assemble, link", $0D, $0A
      .byte "  BUILD file.npp   Build an NPP 1 project", $0D, $0A
      .byte "  RUN file.bin     Run a linked program", $0D, $0A, 0

shell_kw_help:     .byte "HELP", 0
shell_kw_dir:      .byte "DIR", 0
shell_kw_pwd:      .byte "PWD", 0
shell_kw_cd:       .byte "CD", 0
shell_kw_type:     .byte "TYPE", 0
shell_kw_edit:     .byte "EDIT", 0
shell_kw_del:      .byte "DEL", 0
shell_kw_ren:      .byte "REN", 0
shell_kw_mkdir:    .byte "MKDIR", 0
shell_kw_rmdir:    .byte "RMDIR", 0
shell_kw_compile:  .byte "COMPILE", 0
shell_kw_assemble: .byte "ASSEMBLE", 0
shell_kw_link:     .byte "LINK", 0
shell_kw_build:    .byte "BUILD", 0
shell_kw_run:      .byte "RUN", 0

shell_project_magic:     .byte "NPP", 0
shell_project_main_kw:   .byte "MAIN", 0
shell_project_output_kw: .byte "OUTPUT", 0
shell_ext_s:   .byte ".S", 0
shell_ext_obj: .byte ".OBJ", 0
shell_ext_bin: .byte ".BIN", 0

shell_pascal_library: .byte "PASCAL.NLIB"
shell_pascal_library_end:
shell_tool_editor: .byte "NPEDIT.BIN"
shell_tool_editor_end:
shell_tool_assembler: .byte "NAS.BIN"
shell_tool_assembler_end:
shell_tool_linker: .byte "NL.BIN"
shell_tool_linker_end:

      .segment "CODE"

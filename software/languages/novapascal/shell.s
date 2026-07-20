; =====================================================================
; NovaPascal native command shell.
;
; This file is included by the resident $C000 shell/compiler ROM. Commands
; load the editor, NAS, and NL standard binaries into the $2000 tool slot.
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
shell_project_object: .res SHELL_NAME_CAP
shell_project_obj_len:.res 1
shell_project_main_obj:.res SHELL_NAME_CAP
shell_project_main_len:.res 1
shell_project_asm:    .res 1
shell_nas_define:     .res SHELL_NAME_CAP
shell_nas_define_len: .res 1
shell_project_config: .res SHELL_NAME_CAP
shell_project_cfg_len:.res 1
shell_project_map:    .res SHELL_NAME_CAP
shell_project_map_len:.res 1
shell_project_label:  .res SHELL_NAME_CAP
shell_project_label_len:.res 1
shell_project_optimize:.res 1
shell_project_file:   .res SHELL_NAME_CAP
shell_project_file_len:.res 1
shell_project_inline: .res 1
shell_new_len:        .res 2
shell_program_entry:  .res 2
shell_number:          .res 2
shell_digit:           .res 1
shell_number_printed:  .res 1

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
      JSR   shell_reset_link_base
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
      shell_try shell_kw_new,      shell_cmd_new
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
      STX   shell_scan
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

shell_cmd_new:
      JSR   shell_require_name
      BCS   @done
      LDA   shell_name2_len
      BNE   @usage
      JSR   shell_new_project
      BCS   @failed
      LDA   #<shell_new_created
      STA   p_word
      LDA   #>shell_new_created
      STA   p_word+1
      JSR   print_z
      LDA   #<shell_ext_npp
      STA   p_word
      LDA   #>shell_ext_npp
      STA   p_word+1
      JSR   shell_derive_name
      JSR   shell_print_name2
      JSR   shell_newline
      BRA   @done
@failed:
      LDA   shell_load_error
      CMP   #2
      BEQ   @usage
      CMP   #0
      BEQ   @file_error
      LDA   #<shell_new_exists
      STA   p_word
      LDA   #>shell_new_exists
      STA   p_word+1
      JSR   print_z
      BRA   @done
@file_error:
      JSR   shell_print_file_error
      BRA   @done
@usage:
      LDA   #<shell_usage
      STA   p_word
      LDA   #>shell_usage
      STA   p_word+1
      JSR   print_z
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
      JSR   shell_parse_nas_options
      BCS   @usage
      LDA   shell_project_out_len
      BEQ   @default_output
      JSR   shell_use_project_output
      BRA   @output_ready
@default_output:
      LDA   #<shell_ext_obj
      STA   p_word
      LDA   #>shell_ext_obj
      STA   p_word+1
      JSR   shell_derive_name
@output_ready:
      JSR   shell_prepare_tool_args
      JSR   shell_prepare_nas_define
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
      BRA   @done
@usage:
      LDA   #<shell_usage
      STA   p_word
      LDA   #>shell_usage
      STA   p_word+1
      JSR   print_z
@done:
      JMP   shell_loop

shell_cmd_link:
      JSR   shell_require_name
      BCS   @done
      JSR   shell_parse_link_options
      BCS   @usage
      JSR   shell_reset_link_base
      JSR   shell_save_link_object2
      LDA   #<shell_ext_bin
      STA   p_word
      LDA   #>shell_ext_bin
      STA   p_word+1
      JSR   shell_derive_name
      JSR   shell_prepare_tool_args
      JSR   shell_prepare_link_object2
      JSR   shell_prepare_link_config
      JSR   shell_prepare_link_map
      JSR   shell_prepare_link_label
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
      BRA   @done
@usage:
      LDA   #<shell_usage
      STA   p_word
      LDA   #>shell_usage
      STA   p_word+1
      JSR   print_z
@done:
      JMP   shell_loop

shell_cmd_build:
      JSR   shell_require_name
      BCC   :+
      JMP   @done
:
      JSR   shell_reset_link_base
      STZ   shell_project_out_len
      STZ   shell_project_obj_len
      STZ   shell_project_asm
      STZ   shell_nas_define_len
      STZ   shell_project_cfg_len
      STZ   shell_project_map_len
      STZ   shell_project_label_len
      STZ   shell_project_optimize
      STZ   shell_project_inline
      STZ   shell_project_file_len
      JSR   shell_name_is_npp
      BCC   @source_ready
      JSR   shell_save_project_file
      JSR   shell_load_project
      BEQ   :+
      JMP   @file_error
:
      JSR   shell_parse_project
      BCC   :+
      JMP   @project_error
:
      LDA   shell_project_cfg_len
      BEQ   @source_ready
      JSR   shell_prepare_tool_args
      JSR   shell_prepare_link_config
      LDA   #NPTOOL_FLAG_VALIDATE
      STA   NPTOOL_FLAGS
      LDA   #<shell_tool_linker
      STA   p_word
      LDA   #>shell_tool_linker
      STA   p_word+1
      LDA   #shell_tool_linker_end-shell_tool_linker
      JSR   shell_launch_tool
      BCC   @source_ready
      JMP   @tool_error
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
      JSR   shell_prepare_nas_project
      LDA   #<shell_tool_assembler
      STA   p_word
      LDA   #>shell_tool_assembler
      STA   p_word+1
      LDA   #shell_tool_assembler_end-shell_tool_assembler
      JSR   shell_launch_tool
      BCS   @tool_error

      JSR   shell_promote_name2
      LDA   shell_project_asm
      BEQ   :+
      JSR   shell_assemble_project_source
      BCS   @tool_error
:
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
      JSR   shell_prepare_project_object
      JSR   shell_prepare_link_config
      JSR   shell_prepare_link_map
      JSR   shell_prepare_link_label
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
      JSR   shell_copy_program
      BCS   @bad_binary
      LDA   #<shell_running
      STA   p_word
      LDA   #>shell_running
      STA   p_word+1
      JSR   print_z
      LDA   shell_program_entry+1
      JSR   shell_print_hex
      LDA   shell_program_entry
      JSR   shell_print_hex
      LDA   #':'
      STA   VGC_CHAROUT
      JSR   shell_newline
      JSR   shell_call_program
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
      STZ   NPTOOL_FLAGS
      STZ   NPTOOL_ARG3_LEN
      STZ   NPTOOL_ARG3
      STZ   NPTOOL_ARG4_LEN
      STZ   NPTOOL_ARG4
      STZ   NPTOOL_ARG5_LEN
      STZ   NPTOOL_ARG5
      STZ   NPTOOL_ARG6_LEN
      STZ   NPTOOL_ARG6
      STZ   NPTOOL_DIAG_LINE
      STZ   NPTOOL_DIAG_LINE+1
      STZ   NPTOOL_DIAG_COL
      STZ   NPTOOL_DIAG_COL+1
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
      BNE   :+
      JMP   @link
:
      CMP   #NPTOOL_ERR_EDITOR
      BNE   :+
      JMP   @editor
:
      CMP   #NPTOOL_ERR_NOT_TEXT
      BNE   :+
      JMP   @not_text
:
      CMP   #NPTOOL_ERR_MEMORY
      BNE   :+
      JMP   @memory
:
      CMP   #NPTOOL_ERR_ARGS
      BNE   :+
      JMP   @args
:
      LDA   #<shell_tool_load_error
      STA   p_word
      LDA   #>shell_tool_load_error
      STA   p_word+1
      JSR   print_z
      LDA   shell_tool_load_error_code
      JSR   shell_print_hex
      JMP   shell_newline
@io:
      LDA   NPTOOL_DETAIL
      CMP   #NPTOOL_IO_TOO_LARGE
      BNE   :+
      LDA   #1
      STA   shell_load_error
      JMP   shell_print_file_error
:
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
      LDA   NPTOOL_ARG2_LEN
      BEQ   @assemble_primary
      LDA   #<NPTOOL_ARG2
      LDX   #>NPTOOL_ARG2
      BRA   @assemble_name
@assemble_primary:
      LDA   #<NPTOOL_ARG0
      LDX   #>NPTOOL_ARG0
@assemble_name:
      STA   p_word
      STX   p_word+1
      JSR   print_z
      LDA   #':'
      STA   VGC_CHAROUT
      LDA   NPTOOL_DIAG_LINE
      LDX   NPTOOL_DIAG_LINE+1
      JSR   shell_print_u16
      LDA   #':'
      STA   VGC_CHAROUT
      LDA   NPTOOL_DIAG_COL
      LDX   NPTOOL_DIAG_COL+1
      JSR   shell_print_u16
      LDA   #<shell_diag_separator
      STA   p_word
      LDA   #>shell_diag_separator
      STA   p_word+1
      JSR   print_z
      LDA   NPTOOL_DETAIL
      CMP   #1
      BEQ   @assemble_syntax
      CMP   #2
      BEQ   @assemble_output
      CMP   #3
      BEQ   @assemble_symbol
      CMP   #4
      BEQ   @assemble_range
      CMP   #5
      BEQ   @assemble_include
      CMP   #6
      BEQ   @assemble_assert
      LDA   #<shell_assemble_error
      LDX   #>shell_assemble_error
      BRA   @assemble_message
@assemble_syntax:
      LDA   #<shell_assemble_syntax
      LDX   #>shell_assemble_syntax
      BRA   @assemble_message
@assemble_output:
      LDA   #<shell_assemble_output
      LDX   #>shell_assemble_output
      BRA   @assemble_message
@assemble_symbol:
      LDA   #<shell_assemble_symbol
      LDX   #>shell_assemble_symbol
      BRA   @assemble_message
@assemble_range:
      LDA   #<shell_assemble_range
      LDX   #>shell_assemble_range
      BRA   @assemble_message
@assemble_include:
      LDA   #<shell_assemble_include
      LDX   #>shell_assemble_include
      BRA   @assemble_message
@assemble_assert:
      LDA   #<shell_assemble_assert
      LDX   #>shell_assemble_assert
@assemble_message:
      STA   p_word
      STX   p_word+1
      JMP   print_z
@link:
      LDA   NPTOOL_DETAIL
      CMP   #4
      BNE   @link_generic
      LDA   #<shell_link_config_error
      STA   p_word
      LDA   #>shell_link_config_error
      STA   p_word+1
      JMP   print_z
@link_generic:
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
@memory:
      LDA   #<shell_memory_error
      STA   p_word
      LDA   #>shell_memory_error
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
      LDA   source_buf
      STA   shell_program_entry
      STA   p_copy_dst
      LDA   source_buf+1
      STA   shell_program_entry+1
      STA   p_copy_dst+1
      CMP   #>OUTPUT_BASE
      BCC   @bad
      BNE   @base_ok
      LDA   shell_program_entry
      CMP   #<OUTPUT_BASE
      BCC   @bad
@base_ok:
      LDA   #<(source_buf+2)
      STA   p_src
      LDA   #>(source_buf+2)
      STA   p_src+1
      SEC
      LDA   shell_file_len+0
      SBC   #2
      STA   p_left
      LDA   shell_file_len+1
      SBC   #0
      STA   p_left+1
      CLC
      LDA   shell_program_entry
      ADC   p_left
      STA   shell_number
      LDA   shell_program_entry+1
      ADC   p_left+1
      STA   shell_number+1
      BCS   @bad
      CMP   #>VGC_MODE
      BCC   @copy
      BNE   @bad
      LDA   shell_number
      CMP   #<VGC_MODE
      BNE   @bad
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
      CLC
      RTS
@bad:
      SEC
      RTS

shell_call_program:
      LDA   #>(@returned-1)
      PHA
      LDA   #<(@returned-1)
      PHA
      JMP   (shell_program_entry)
@returned:
      RTS

shell_reset_link_base:
      LDA   #<OUTPUT_BASE
      STA   NPTOOL_LINK_BASE
      LDA   #>OUTPUT_BASE
      STA   NPTOOL_LINK_BASE+1
      RTS

; ---------------------------------------------------------------------
; NPP v1 parser.
;
; NPP 1 keeps one MAIN/OUTPUT pair and optional O2/DEFINE/OBJECT/ASM/link options.
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
      BCS   @bad_early
      LDA   #'1'
      JSR   p_expect_char
      BCS   @bad_early
      LDA   #<shell_project_main_kw
      STA   p_word
      LDA   #>shell_project_main_kw
      STA   p_word+1
      JSR   p_expect_word
      BCS   @bad_early
      LDA   #<shell_name
      STA   p_copy_dst
      LDA   #>shell_name
      STA   p_copy_dst+1
      JSR   shell_project_token
      BCS   @bad_early
      LDA   shell_token_len
      STA   shell_name_len
      BRA   @output
@bad_early:
      JMP   @bad

@output:
      LDA   #<shell_project_output_kw
      STA   p_word
      LDA   #>shell_project_output_kw
      STA   p_word+1
      JSR   p_expect_word
      BCC   :+
      JMP   @bad
:
      LDA   #<shell_project_output
      STA   p_copy_dst
      LDA   #>shell_project_output
      STA   p_copy_dst+1
      JSR   shell_project_token
      BCC   :+
      JMP   @bad
:
      LDA   shell_token_len
      STA   shell_project_out_len
      JSR   p_skip_ws
      LDA   p_left
      ORA   p_left+1
      BNE   :+
      JMP   @ok
:
      JSR   p_peek_upper
      CMP   #'A'
      BNE   :+
      JMP   @asm
:
      CMP   #'D'
      BNE   :+
      JMP   @define
:
      CMP   #'O'
      BNE   :+
      JMP   @object_or_optimize
:
      CMP   #'C'
      BNE   :+
      JMP   @config
:
      CMP   #'M'
      BEQ   :+
      JMP   @load_or_label
:
      JMP   @map
@define:
      LDA   shell_nas_define_len
      BEQ   :+
      JMP   @bad
:
      LDA   #<shell_project_define_kw
      STA   p_word
      LDA   #>shell_project_define_kw
      STA   p_word+1
      JSR   p_expect_word
      BCC   :+
      JMP   @bad
:
      LDA   #<shell_nas_define
      STA   p_copy_dst
      LDA   #>shell_nas_define
      STA   p_copy_dst+1
      JSR   shell_project_token
      BCC   :+
      JMP   @bad
:
      LDA   shell_token_len
      STA   shell_nas_define_len
      JSR   p_skip_ws
      LDA   p_left
      ORA   p_left+1
      BNE   :+
      JMP   @ok
:
      JSR   p_peek_upper
      CMP   #'A'
      BNE   :+
      JMP   @asm
:
      CMP   #'O'
      BNE   :+
      JMP   @object_or_optimize
:
      CMP   #'C'
      BNE   :+
      JMP   @config
:
      CMP   #'M'
      BEQ   :+
      JMP   @load_or_label
:
      JMP   @map
@object_or_optimize:
      JSR   p_skip_ws
      LDA   p_left+1
      BNE   :+
      LDA   p_left
      CMP   #2
      BCC   @optimize_bad
:
      LDY   #1
      LDA   (p_src),Y
      JSR   shell_upper
      CMP   #'P'
      BEQ   @optimize
      JMP   @object
@optimize:
      LDA   shell_project_optimize
      BNE   @optimize_bad
      LDA   #<shell_project_optimize_kw
      STA   p_word
      LDA   #>shell_project_optimize_kw
      STA   p_word+1
      JSR   p_expect_word
      BCS   @optimize_bad
      JSR   p_skip_ws
      JSR   p_read_upper
      BCC   @optimize_bad
      CMP   #'O'
      BNE   @optimize_bad
      JSR   p_next
      BCC   @optimize_bad
      CMP   #'2'
      BNE   @optimize_bad
      LDA   #2
      STA   shell_project_optimize
      JSR   p_skip_ws
      LDA   p_left
      ORA   p_left+1
      BNE   :+
      JMP   @ok
:
      JSR   p_peek_upper
      CMP   #'A'
      BNE   :+
      JMP   @asm
:
      CMP   #'D'
      BNE   :+
      JMP   @define
:
      CMP   #'O'
      BNE   :+
      JMP   @object_or_optimize
:
      CMP   #'C'
      BNE   :+
      JMP   @config
:
      CMP   #'M'
      BNE   @load_or_label
      JMP   @map
@optimize_bad:
      JMP   @bad
@asm:
      LDA   #<shell_project_asm_kw
      STA   p_word
      LDA   #>shell_project_asm_kw
      STA   p_word+1
      LDA   #1
      STA   shell_project_asm
      BRA   @aux_keyword
@object:
      LDA   #<shell_project_object_kw
      STA   p_word
      LDA   #>shell_project_object_kw
      STA   p_word+1
      STZ   shell_project_asm
@aux_keyword:
      JSR   p_expect_word
      BCC   :+
      JMP   @bad
:
      LDA   #<shell_project_object
      STA   p_copy_dst
      LDA   #>shell_project_object
      STA   p_copy_dst+1
      JSR   shell_project_token
      BCC   :+
      JMP   @bad
:
      LDA   shell_token_len
      STA   shell_project_obj_len
      JSR   p_skip_ws
      LDA   p_left
      ORA   p_left+1
      BNE   :+
      JMP   @ok
:
      JSR   p_peek_upper
      CMP   #'C'
      BNE   :+
      JMP   @config
:
      CMP   #'M'
      BNE   @load_or_label
      JMP   @map
@load_bad:
      JMP   @bad
@load_ok:
      JMP   @ok
@load_label:
      JMP   @label
@load_or_label:
      JSR   p_skip_ws
      LDA   p_left+1
      BNE   @second_keyword_byte
      LDA   p_left
      CMP   #2
      BCC   @load_bad
@second_keyword_byte:
      LDY   #1
      LDA   (p_src),Y
      JSR   shell_upper
      CMP   #'A'
      BEQ   @load_label
@load:
      LDA   #<shell_project_load_kw
      STA   p_word
      LDA   #>shell_project_load_kw
      STA   p_word+1
      JSR   p_expect_word
      BCC   :+
      JMP   @bad
:
      LDA   #'$'
      JSR   p_expect_char
      BCS   @load_bad
      JSR   p_parse_hex16
      BCS   @load_bad
      LDA   shell_number
      STA   NPTOOL_LINK_BASE
      LDA   shell_number+1
      STA   NPTOOL_LINK_BASE+1
      JSR   p_skip_ws
      LDA   p_left
      ORA   p_left+1
      BNE   :+
      JMP   @ok
:
      JSR   p_peek_upper
      CMP   #'C'
      BEQ   @config
      CMP   #'M'
      BEQ   @map
      CMP   #'L'
      BEQ   @load_or_label
      BRA   @load_bad
@config:
      LDA   #<shell_project_config_kw
      STA   p_word
      LDA   #>shell_project_config_kw
      STA   p_word+1
      JSR   p_expect_word
      BCS   @load_bad
      LDA   #<shell_project_config
      STA   p_copy_dst
      LDA   #>shell_project_config
      STA   p_copy_dst+1
      JSR   shell_project_token
      BCS   @load_bad
      LDA   shell_token_len
      STA   shell_project_cfg_len
      JSR   shell_use_inline_config
      JSR   p_skip_ws
      LDA   p_left
      ORA   p_left+1
      BNE   :+
      JMP   @ok
:
      JSR   p_peek_upper
      CMP   #'M'
      BEQ   @map
      CMP   #'L'
      BEQ   :+
      JMP   @bad
:
      JMP   @load_or_label
@map:
      LDA   shell_project_map_len
      BNE   @bad
      LDA   #<shell_project_map_kw
      STA   p_word
      LDA   #>shell_project_map_kw
      STA   p_word+1
      JSR   p_expect_word
      BCS   @bad
      LDA   #<shell_project_map
      STA   p_copy_dst
      LDA   #>shell_project_map
      STA   p_copy_dst+1
      JSR   shell_project_token
      BCS   @bad
      LDA   shell_token_len
      STA   shell_project_map_len
      JSR   p_skip_ws
      LDA   p_left
      ORA   p_left+1
      BNE   @label
      BRA   @ok
@label:
      LDA   shell_project_label_len
      BNE   @bad
      LDA   #<shell_project_label_kw
      STA   p_word
      LDA   #>shell_project_label_kw
      STA   p_word+1
      JSR   p_expect_word
      BCS   @bad
      LDA   #<shell_project_label
      STA   p_copy_dst
      LDA   #>shell_project_label
      STA   p_copy_dst+1
      JSR   shell_project_token
      BCS   @bad
      LDA   shell_token_len
      STA   shell_project_label_len
      JSR   p_skip_ws
      LDA   p_left
      ORA   p_left+1
      BEQ   @ok
      LDA   shell_project_inline
      BEQ   @bad
      LDA   #<shell_project_memory_kw
      STA   p_word
      LDA   #>shell_project_memory_kw
      STA   p_word+1
      JSR   p_expect_word
      BCS   @bad
@ok:
      CLC
      RTS
@bad:
      SEC
      RTS

p_parse_hex16:
      STZ   shell_number
      STZ   shell_number+1
      LDX   #4
@digit:
      JSR   p_next
      BCC   @bad
      JSR   shell_hex_nibble
      BCS   @bad
      STA   shell_digit
      ASL   shell_number
      ROL   shell_number+1
      ASL   shell_number
      ROL   shell_number+1
      ASL   shell_number
      ROL   shell_number+1
      ASL   shell_number
      ROL   shell_number+1
      LDA   shell_number
      ORA   shell_digit
      STA   shell_number
      DEX
      BNE   @digit
      CLC
      RTS
@bad:
      SEC
      RTS

shell_hex_nibble:
      JSR   shell_upper
      CMP   #'0'
      BCC   @bad
      CMP   #'9'+1
      BCS   @letter
      SEC
      SBC   #'0'
      CLC
      RTS
@letter:
      CMP   #'A'
      BCC   @bad
      CMP   #'F'+1
      BCS   @bad
      SEC
      SBC   #'A'-10
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

shell_save_project_file:
      LDA   shell_name_len
      STA   shell_project_file_len
      LDY   #0
@copy:
      LDA   shell_name,Y
      STA   shell_project_file,Y
      BEQ   @done
      INY
      BRA   @copy
@done:
      RTS

; CONFIG INLINE means NL reads its MEMORY/SEGMENTS/SYMBOLS blocks from NPP.
shell_use_inline_config:
      LDA   shell_project_cfg_len
      CMP   #6
      BNE   @done
      LDY   #0
@compare:
      LDA   shell_project_config,Y
      JSR   shell_upper
      CMP   shell_project_inline_kw,Y
      BNE   @done
      INY
      CPY   #6
      BCC   @compare
      LDA   #1
      STA   shell_project_inline
      LDA   shell_project_file_len
      STA   shell_project_cfg_len
      LDY   #0
@copy:
      LDA   shell_project_file,Y
      STA   shell_project_config,Y
      BEQ   @done
      INY
      BRA   @copy
@done:
      RTS

; Preserve the optional second LINK input while shell_name2 becomes output.
shell_save_link_object2:
      LDA   shell_name2_len
      STA   shell_project_out_len
      LDY   #0
@copy:
      LDA   shell_name2,Y
      STA   shell_project_output,Y
      BEQ   @done
      INY
      BRA   @copy
@done:
      RTS

shell_prepare_link_object2:
      LDA   shell_project_out_len
      STA   NPTOOL_ARG3_LEN
      BEQ   @empty
      LDY   #0
@copy:
      LDA   shell_project_output,Y
      STA   NPTOOL_ARG3,Y
      BEQ   @done
      INY
      BRA   @copy
@empty:
      STZ   NPTOOL_ARG3
@done:
      RTS

; Assemble one optional NPP ASM source while preserving the main Pascal object
; as NL's first input. The derived object replaces the project source token.
shell_assemble_project_source:
      LDY   #0
@save_main:
      LDA   shell_name,Y
      STA   shell_project_main_obj,Y
      BEQ   @use_source
      INY
      BRA   @save_main
@use_source:
      STY   shell_project_main_len
      LDY   #0
@copy_source:
      LDA   shell_project_object,Y
      STA   shell_name,Y
      BEQ   @derive
      INY
      BRA   @copy_source
@derive:
      STY   shell_name_len
      LDA   #<shell_ext_obj
      STA   p_word
      LDA   #>shell_ext_obj
      STA   p_word+1
      JSR   shell_derive_name
      JSR   shell_prepare_tool_args
      JSR   shell_prepare_nas_project
      LDA   #<shell_tool_assembler
      STA   p_word
      LDA   #>shell_tool_assembler
      STA   p_word+1
      LDA   #shell_tool_assembler_end-shell_tool_assembler
      JSR   shell_launch_tool
      BCS   @bad
      LDA   shell_name2_len
      STA   shell_project_obj_len
      LDY   #0
@save_object:
      LDA   shell_name2,Y
      STA   shell_project_object,Y
      BEQ   @restore_main
      INY
      BRA   @save_object
@restore_main:
      LDY   #0
@restore:
      LDA   shell_project_main_obj,Y
      STA   shell_name,Y
      BEQ   @ok
      INY
      BRA   @restore
@ok:
      LDA   shell_project_main_len
      STA   shell_name_len
      CLC
      RTS
@bad:
      SEC
      RTS

shell_prepare_project_object:
      LDA   shell_project_obj_len
      STA   NPTOOL_ARG3_LEN
      BEQ   @empty
      LDY   #0
@copy:
      LDA   shell_project_object,Y
      STA   NPTOOL_ARG3,Y
      BEQ   @done
      INY
      BRA   @copy
@empty:
      STZ   NPTOOL_ARG3
@done:
      RTS

; Accept ASSEMBLE source [-DNAME=VALUE] [-o object].
shell_parse_nas_options:
      STZ   shell_nas_define_len
      STZ   shell_nas_define
      STZ   shell_project_out_len
      STZ   shell_project_output
      LDA   shell_name2_len
      BEQ   @remaining_start
      LDA   shell_name2
      CMP   #'-'
      BNE   @first_bad
      LDA   shell_name2+1
      JSR   shell_upper
      CMP   #'O'
      BEQ   @first_output
      CMP   #'D'
      BNE   @first_bad
      LDA   shell_name2_len
      CMP   #4
      BCC   @first_bad
      STZ   shell_digit
      LDY   #2
      LDX   #0
@copy_first_define:
      CPY   shell_name2_len
      BCS   @first_define_copied
      LDA   shell_name2,Y
      CMP   #'='
      BNE   @store_first_define
      LDA   shell_digit
      BNE   @first_bad
      CPX   #0
      BEQ   @first_bad
      INC   shell_digit
      LDA   #'='
@store_first_define:
      STA   shell_nas_define,X
      INX
      INY
      BRA   @copy_first_define
@first_define_copied:
      LDA   shell_digit
      BEQ   @first_bad
      DEX
      LDA   shell_nas_define,X
      CMP   #'='
      BEQ   @first_bad
      INX
      STX   shell_nas_define_len
      STZ   shell_nas_define,X
      BRA   @remaining_start
@first_bad:
      JMP   @bad
@first_output:
      LDA   shell_name2_len
      CMP   #2
      BNE   @first_bad
      LDX   shell_scan
      BRA   @output_value

@remaining_start:
      LDX   shell_scan
@remaining:
@skip_option:
      LDA   input_buf,X
      BNE   :+
      JMP   @ok
:
      CMP   #' '
      BEQ   @skip_option_byte
      CMP   #$09
      BNE   @option
@skip_option_byte:
      INX
      BRA   @skip_option
@option_bad:
      JMP   @bad
@option:
      CMP   #'-'
      BNE   @option_bad
      INX
      LDA   input_buf,X
      JSR   shell_upper
      CMP   #'D'
      BEQ   @define
      CMP   #'O'
      BNE   @option_bad
      LDA   shell_project_out_len
      BNE   @option_bad
      INX
      LDA   input_buf,X
      BEQ   @option_bad
      CMP   #' '
      BEQ   @output_value
      CMP   #$09
      BNE   @option_bad
@output_value:
      LDA   input_buf,X
      CMP   #' '
      BEQ   @skip_output_byte
      CMP   #$09
      BNE   @copy_output_start
@skip_output_byte:
      INX
      BRA   @output_value
@copy_output_start:
      LDY   #0
@copy_output:
      LDA   input_buf,X
      BEQ   @output_copied
      CMP   #' '
      BEQ   @output_copied
      CMP   #$09
      BEQ   @output_copied
      CPY   #SHELL_NAME_CAP-1
      BCS   @bad
      STA   shell_project_output,Y
      INX
      INY
      BRA   @copy_output
@output_copied:
      CPY   #0
      BEQ   @bad
      STY   shell_project_out_len
      LDA   #0
      STA   shell_project_output,Y
      JMP   @remaining

@define:
      LDA   shell_nas_define_len
      BNE   @bad
      INX
      STZ   shell_digit
      LDY   #0
@copy_define:
      LDA   input_buf,X
      BEQ   @define_copied
      CMP   #' '
      BEQ   @define_copied
      CMP   #$09
      BEQ   @define_copied
      CMP   #'='
      BNE   @store_define
      LDA   shell_digit
      BNE   @bad
      CPY   #0
      BEQ   @bad
      INC   shell_digit
      LDA   #'='
@store_define:
      CPY   #SHELL_NAME_CAP-1
      BCS   @bad
      STA   shell_nas_define,Y
      INX
      INY
      BRA   @copy_define
@define_copied:
      LDA   shell_digit
      BEQ   @bad
      DEY
      LDA   shell_nas_define,Y
      CMP   #'='
      BEQ   @bad
      INY
      STY   shell_nas_define_len
      LDA   #0
      STA   shell_nas_define,Y
      JMP   @remaining
@ok:
      STZ   shell_name2_len
      STZ   shell_name2
      CLC
      RTS
@bad:
      SEC
      RTS

shell_prepare_nas_define:
      LDA   shell_nas_define_len
      STA   NPTOOL_ARG6_LEN
      BEQ   @empty
      LDY   #0
@copy:
      LDA   shell_nas_define,Y
      STA   NPTOOL_ARG6,Y
      BEQ   @done
      INY
      BRA   @copy
@empty:
      STZ   NPTOOL_ARG6
@done:
      RTS

shell_prepare_nas_project:
      LDA   shell_project_file_len
      STA   NPTOOL_ARG4_LEN
      BEQ   @empty
      LDY   #0
@copy:
      LDA   shell_project_file,Y
      STA   NPTOOL_ARG4,Y
      BEQ   @done
      INY
      BRA   @copy
@empty:
      STZ   NPTOOL_ARG4
@done:
      RTS

shell_prepare_link_config:
      LDA   shell_project_cfg_len
      STA   NPTOOL_ARG4_LEN
      BEQ   @empty
      LDY   #0
@copy:
      LDA   shell_project_config,Y
      STA   NPTOOL_ARG4,Y
      BEQ   @done
      INY
      BRA   @copy
@empty:
      STZ   NPTOOL_ARG4
@done:
      RTS

shell_prepare_link_map:
      LDA   shell_project_map_len
      STA   NPTOOL_ARG5_LEN
      BEQ   @empty
      LDY   #0
@copy:
      LDA   shell_project_map,Y
      STA   NPTOOL_ARG5,Y
      BEQ   @done
      INY
      BRA   @copy
@empty:
      STZ   NPTOOL_ARG5
@done:
      RTS

shell_prepare_link_label:
      LDA   shell_project_label_len
      STA   NPTOOL_ARG6_LEN
      BEQ   @empty
      LDY   #0
@copy:
      LDA   shell_project_label,Y
      STA   NPTOOL_ARG6,Y
      BEQ   @done
      INY
      BRA   @copy
@empty:
      STZ   NPTOOL_ARG6
@done:
      RTS

; Accept LINK main.obj [more.obj] [-C file.cfg] [-M file.map] [-Ln file.lbl].
shell_parse_link_options:
      STZ   shell_project_cfg_len
      STZ   shell_project_config
      STZ   shell_project_map_len
      STZ   shell_project_map
      STZ   shell_project_label_len
      STZ   shell_project_label
      LDA   shell_name2_len
      CMP   #3
      BNE   @first_short
      LDA   shell_name2
      CMP   #'-'
      BNE   @start_remaining
      LDA   shell_name2+1
      JSR   shell_upper
      CMP   #'L'
      BNE   @start_remaining
      LDA   shell_name2+2
      JSR   shell_upper
      CMP   #'N'
      BNE   @start_remaining
      LDA   #'L'
      BRA   @first_option
@first_short:
      CMP   #2
      BNE   @start_remaining
      LDA   shell_name2
      CMP   #'-'
      BNE   @start_remaining
      LDA   shell_name2+1
      JSR   shell_upper
      CMP   #'C'
      BEQ   @first_option
      CMP   #'M'
      BNE   @start_remaining
@first_option:
      STA   shell_digit
      STZ   shell_name2_len
      STZ   shell_name2
      BRA   @value

@start_remaining:
      LDX   shell_scan
@remaining:
@skip_option:
      LDA   input_buf,X
      BNE   :+
      JMP   @ok
:
      CMP   #' '
      BEQ   @skip_option_byte
      CMP   #$09
      BNE   @option
@skip_option_byte:
      INX
      BRA   @skip_option
@option_bad:
      JMP   @bad
@option:
      CMP   #'-'
      BEQ   :+
      JMP   @bad
:
      INX
      LDA   input_buf,X
      JSR   shell_upper
      CMP   #'C'
      BEQ   @option_found
      CMP   #'M'
      BEQ   @option_found
      CMP   #'L'
      BNE   @option_bad
      INX
      LDA   input_buf,X
      JSR   shell_upper
      CMP   #'N'
      BNE   @option_bad
      LDA   #'L'
@option_found:
      STA   shell_digit
      INX
      LDA   input_buf,X
      BEQ   @option_bad
      CMP   #' '
      BEQ   @value
      CMP   #$09
      BNE   @option_bad

@value:
      LDA   shell_digit
      CMP   #'C'
      BNE   @check_map
      LDA   shell_project_cfg_len
      BNE   @option_bad
      BRA   @skip_value
@check_map:
      CMP   #'M'
      BNE   @check_label
      LDA   shell_project_map_len
      BNE   @option_bad
      BRA   @skip_value
@check_label:
      LDA   shell_project_label_len
      BNE   @option_bad
@skip_value:
      LDA   input_buf,X
      CMP   #' '
      BEQ   @skip_value_byte
      CMP   #$09
      BNE   @copy_start
@skip_value_byte:
      INX
      BRA   @value
@copy_start:
      LDY   #0
@copy:
      LDA   input_buf,X
      BEQ   @copied
      CMP   #' '
      BEQ   @copied
      CMP   #$09
      BEQ   @copied
      CPY   #SHELL_NAME_CAP-1
      BCS   @bad
      PHA
      LDA   shell_digit
      CMP   #'C'
      BNE   @copy_nonconfig
      PLA
      STA   shell_project_config,Y
      BRA   @copied_byte
@copy_nonconfig:
      CMP   #'M'
      BNE   @copy_label
      PLA
      STA   shell_project_map,Y
      BRA   @copied_byte
@copy_label:
      PLA
      STA   shell_project_label,Y
@copied_byte:
      INX
      INY
      BRA   @copy
@copied:
      CPY   #0
      BEQ   @bad
      LDA   #0
      PHA
      LDA   shell_digit
      CMP   #'C'
      BNE   @finish_nonconfig
      STY   shell_project_cfg_len
      PLA
      STA   shell_project_config,Y
      JMP   @remaining
@finish_nonconfig:
      CMP   #'M'
      BNE   @finish_label
      STY   shell_project_map_len
      PLA
      STA   shell_project_map,Y
      JMP   @remaining
@finish_label:
      STY   shell_project_label_len
      PLA
      STA   shell_project_label,Y
      JMP   @remaining
@ok:
      CLC
      RTS
@bad:
      SEC
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

shell_new_project:
      STZ   shell_load_error
      STZ   shell_fio_error
      JSR   shell_new_validate_name
      BCC   :+
      JMP   @usage
:
      LDA   #<shell_ext_pas
      LDX   #>shell_ext_pas
      JSR   shell_new_require_absent
      BCC   :+
      JMP   @failed
:
      LDA   #<shell_ext_npp
      LDX   #>shell_ext_npp
      JSR   shell_new_require_absent
      BCC   :+
      JMP   @failed
:
      JSR   shell_new_reset
      LDA   #<shell_new_pas_prefix
      LDX   #>shell_new_pas_prefix
      JSR   shell_new_append_z
      JSR   shell_new_append_name
      LDA   #<shell_new_pas_suffix
      LDX   #>shell_new_pas_suffix
      JSR   shell_new_append_z
      LDA   #<shell_ext_pas
      LDX   #>shell_ext_pas
      JSR   shell_new_save_ext
      BCS   @failed

      JSR   shell_new_reset
      LDA   #<shell_new_npp_prefix
      LDX   #>shell_new_npp_prefix
      JSR   shell_new_append_z
      JSR   shell_new_append_name
      LDA   #<shell_ext_pas
      LDX   #>shell_ext_pas
      JSR   shell_new_append_z
      LDA   #<shell_new_npp_output
      LDX   #>shell_new_npp_output
      JSR   shell_new_append_z
      JSR   shell_new_append_name
      LDA   #<shell_ext_bin
      LDX   #>shell_ext_bin
      JSR   shell_new_append_z
      LDA   #<shell_new_npp_options
      LDX   #>shell_new_npp_options
      JSR   shell_new_append_z
      LDA   #<shell_new_npp_map
      LDX   #>shell_new_npp_map
      JSR   shell_new_append_z
      JSR   shell_new_append_name
      LDA   #<shell_ext_map
      LDX   #>shell_ext_map
      JSR   shell_new_append_z
      LDA   #<shell_new_npp_label
      LDX   #>shell_new_npp_label
      JSR   shell_new_append_z
      JSR   shell_new_append_name
      LDA   #<shell_ext_lbl
      LDX   #>shell_ext_lbl
      JSR   shell_new_append_z
      LDA   #<shell_new_lf
      LDX   #>shell_new_lf
      JSR   shell_new_append_z
      LDA   #<shell_ext_npp
      LDX   #>shell_ext_npp
      JSR   shell_new_save_ext
@failed:
      RTS
@usage:
      LDA   #2
      STA   shell_load_error
      SEC
      RTS

shell_new_validate_name:
      LDA   shell_name_len
      BEQ   @bad
      LDA   shell_name
      CMP   #'_'
      BEQ   @rest
      JSR   shell_upper
      CMP   #'A'
      BCC   @bad
      CMP   #'Z'+1
      BCS   @bad
@rest:
      LDX   #1
@byte:
      CPX   shell_name_len
      BCS   @ok
      LDA   shell_name,X
      JSR   shell_upper
      JSR   p_is_ident
      BCC   @bad
      INX
      BRA   @byte
@ok:
      CLC
      RTS
@bad:
      SEC
      RTS

shell_new_require_absent:
      STA   p_word
      STX   p_word+1
      JSR   shell_derive_name
      LDA   #<shell_name2
      STA   p_word
      LDA   #>shell_name2
      STA   p_word+1
      LDA   shell_name2_len
      STA   shell_io_len
      JSR   shell_copy_fio_name
      BNE   @open_error
      LDA   #FIO_FILE_ACCESS_READ
      STA   FIO_DIRTYPE
      JSR   fio_fopen
      BNE   @open_error
      LDA   FIO_SRCL
      STA   shell_file_id
      LDA   FIO_SRCH
      STA   shell_file_id+1
      JSR   shell_close_file
      LDA   #1
      STA   shell_load_error
      SEC
      RTS
@open_error:
      LDA   FIO_ERRCODE
      CMP   #FIO_ERR_NOTFOUND
      BEQ   @absent
      STA   shell_fio_error
      STZ   shell_load_error
      SEC
      RTS
@absent:
      CLC
      RTS

shell_new_reset:
      STZ   shell_new_len
      STZ   shell_new_len+1
      RTS

shell_new_append_name:
      LDA   #<shell_name
      LDX   #>shell_name
      LDY   shell_name_len
      BRA   shell_new_append

; A/X = source, Y = length.
shell_new_append:
      STA   p_word
      STX   p_word+1
      STY   shell_token_len
      LDY   #0
@loop:
      CPY   shell_token_len
      BCS   @ok
      LDA   (p_word),Y
      PHY
      JSR   shell_new_append_byte
      PLY
      BCS   @bad
      INY
      BRA   @loop
@ok:
      CLC
@bad:
      RTS

; A/X = zero-terminated source.
shell_new_append_z:
      STA   p_word
      STX   p_word+1
      LDY   #0
@loop:
      LDA   (p_word),Y
      BEQ   @ok
      PHY
      JSR   shell_new_append_byte
      PLY
      BCS   @bad
      INY
      BNE   @loop
@bad:
      SEC
      RTS
@ok:
      CLC
      RTS

shell_new_append_byte:
      PHA
      LDA   shell_new_len+1
      CMP   #>SHELL_BUFFER_CAP
      BCC   @room
      PLA
      SEC
      RTS
@room:
      CLC
      LDA   #<source_buf
      ADC   shell_new_len
      STA   p_copy_dst
      LDA   #>source_buf
      ADC   shell_new_len+1
      STA   p_copy_dst+1
      PLA
      LDY   #0
      STA   (p_copy_dst),Y
      INC   shell_new_len
      BNE   :+
      INC   shell_new_len+1
:
      CLC
      RTS

shell_new_save_ext:
      STA   p_word
      STX   p_word+1
      JSR   shell_derive_name
      LDA   #<shell_name2
      STA   p_word
      LDA   #>shell_name2
      STA   p_word+1
      LDA   shell_name2_len
      STA   shell_io_len
      JSR   shell_copy_fio_name
      BNE   @error
      LDA   #FIO_FILE_ACCESS_WRITE
      STA   FIO_DIRTYPE
      JSR   fio_fcreate
      BNE   @error
      LDA   FIO_SRCL
      STA   shell_file_id
      LDA   FIO_SRCH
      STA   shell_file_id+1
      LDA   #<source_buf
      STA   FIO_ENDL
      LDA   #>source_buf
      STA   FIO_ENDH
      LDA   shell_new_len
      STA   FIO_GLENL
      LDA   shell_new_len+1
      STA   FIO_GLENH
      LDA   #FIO_FILE_TARGET_RAM
      STA   FIO_DIRTYPE
      JSR   fio_fwrite
      BNE   @write_error
      JSR   shell_close_file
      CLC
      RTS
@write_error:
      LDA   FIO_ERRCODE
      STA   shell_fio_error
      JSR   shell_close_file
      SEC
      RTS
@error:
      LDA   FIO_ERRCODE
      STA   shell_fio_error
      STZ   shell_load_error
      SEC
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

; Print unsigned A/X (low/high) in decimal.
shell_print_u16:
      STA   shell_number
      STX   shell_number+1
      STZ   shell_number_printed
      LDX   #0
@divisor:
      STZ   shell_digit
@count:
      LDA   shell_number+1
      CMP   shell_decimal_divisors+1,X
      BCC   @emit
      BNE   @subtract
      LDA   shell_number
      CMP   shell_decimal_divisors,X
      BCC   @emit
@subtract:
      SEC
      LDA   shell_number
      SBC   shell_decimal_divisors,X
      STA   shell_number
      LDA   shell_number+1
      SBC   shell_decimal_divisors+1,X
      STA   shell_number+1
      INC   shell_digit
      BRA   @count
@emit:
      LDA   shell_digit
      BNE   @write
      LDA   shell_number_printed
      BNE   @write
      CPX   #8
      BNE   @next
@write:
      LDA   shell_digit
      ORA   #'0'
      STA   VGC_CHAROUT
      INC   shell_number_printed
@next:
      INX
      INX
      CPX   #10
      BCC   @divisor
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
shell_running: .byte "Running at $", 0
shell_not_found: .byte "File not found: ", 0
shell_io_error: .byte "File I/O error.", $0D, $0A, 0
shell_disk_full: .byte "Disk full.", $0D, $0A, 0
shell_not_mounted: .byte "Drive not mounted.", $0D, $0A, 0
shell_too_large: .byte "File is too large for this command.", $0D, $0A, 0
shell_not_text: .byte "Not a text file.", $0D, $0A, 0
shell_memory_error: .byte "Not enough XRAM.", $0D, $0A, 0
shell_editor_error: .byte "Editor error.", $0D, $0A, 0
shell_compile_error: .byte "Pascal syntax error.", $0D, $0A, 0
shell_diag_separator: .byte ": error: ", 0
shell_assemble_error: .byte "assembler error.", $0D, $0A, 0
shell_assemble_syntax:.byte "syntax error.", $0D, $0A, 0
shell_assemble_output:.byte "object output is too large.", $0D, $0A, 0
shell_assemble_symbol:.byte "invalid or unresolved symbol.", $0D, $0A, 0
shell_assemble_range: .byte "value is out of range.", $0D, $0A, 0
shell_assemble_include:.byte "cannot read include file.", $0D, $0A, 0
shell_assemble_assert:.byte "assertion failed.", $0D, $0A, 0
shell_link_error: .byte "Linker error.", $0D, $0A, 0
shell_link_config_error: .byte "Linker configuration error.", $0D, $0A, 0
shell_project_error: .byte "Invalid NPP 1 project file.", $0D, $0A, 0
shell_binary_error: .byte "Not a runnable Nova binary.", $0D, $0A, 0
shell_tool_load_error: .byte "Tool load error $", 0
shell_decimal_divisors:
      .word 10000, 1000, 100, 10, 1

shell_help:
      .byte "File commands:", $0D, $0A
      .byte "  DIR [mask]       List files", $0D, $0A
      .byte "  PWD              Show directory", $0D, $0A
      .byte "  CD dir           Change directory", $0D, $0A
      .byte "  TYPE file        Print ASCII text only", $0D, $0A
      .byte "  NEW name         Create an O2 Pascal project", $0D, $0A
      .byte "  EDIT file        Edit source/project/config text", $0D, $0A
      .byte "  DEL file         Delete a file", $0D, $0A
      .byte "  REN old new      Rename a file", $0D, $0A
      .byte "  MKDIR dir        Create directory", $0D, $0A
      .byte "  RMDIR dir        Remove directory", $0D, $0A
      .byte "Build commands:", $0D, $0A
      .byte "  COMPILE file.pas Write file.S", $0D, $0A
      .byte "  ASSEMBLE file.s [-Dname=value] [-o file.obj]", $0D, $0A
      .byte "  LINK main.obj [more.obj] [-C cfg] [-M map] [-Ln labels]", $0D, $0A
      .byte "  BUILD file.pas   Compile, assemble, link", $0D, $0A
      .byte "  BUILD file.npp   Build an NPP 1 project", $0D, $0A
      .byte "  RUN file.bin     Run a linked program", $0D, $0A, 0

shell_kw_help:     .byte "HELP", 0
shell_kw_dir:      .byte "DIR", 0
shell_kw_pwd:      .byte "PWD", 0
shell_kw_cd:       .byte "CD", 0
shell_kw_type:     .byte "TYPE", 0
shell_kw_new:      .byte "NEW", 0
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
shell_project_optimize_kw:.byte "OPTIMIZE", 0
shell_project_define_kw: .byte "DEFINE", 0
shell_project_asm_kw:    .byte "ASM", 0
shell_project_object_kw: .byte "OBJECT", 0
shell_project_load_kw:   .byte "LOAD", 0
shell_project_config_kw: .byte "CONFIG", 0
shell_project_memory_kw: .byte "MEMORY", 0
shell_project_map_kw:    .byte "MAP", 0
shell_project_label_kw:  .byte "LABEL", 0
shell_ext_s:   .byte ".S", 0
shell_ext_obj: .byte ".OBJ", 0
shell_ext_bin: .byte ".BIN", 0
shell_ext_pas: .byte ".PAS", 0
shell_ext_npp: .byte ".NPP", 0
shell_ext_cfg: .byte ".CFG", 0
shell_ext_map: .byte ".MAP", 0
shell_ext_lbl: .byte ".LBL", 0

shell_new_created: .byte "Created ", 0
shell_new_exists: .byte "Project already exists.", $0D, $0A, 0
shell_new_pas_prefix: .byte "program ", 0
shell_new_pas_suffix:
      .byte ";", $0A
      .byte "begin", $0A
      .byte "  writeln('Hello, world!');", $0A
      .byte "end.", $0A, 0
shell_new_npp_prefix: .byte "NPP 1", $0A, "MAIN ", 0
shell_new_npp_output: .byte $0A, "OUTPUT ", 0
shell_new_npp_options:
      .byte $0A, "OPTIMIZE O2", $0A
      .byte "DEFINE NOVA=1", $0A
      .byte "CONFIG INLINE", 0
shell_new_npp_map: .byte $0A, "MAP ", 0
shell_new_npp_label: .byte $0A, "LABEL ", 0
shell_new_lf:
      .byte $0A
      .byte "MEMORY {", $0A
      .byte "    RAM: start = $8000, size = $0100, file = %O;", $0A
      .byte "}", $0A, $0A
      .byte "SEGMENTS {", $0A
      .byte "    CODE: load = RAM, type = ro;", $0A
      .byte "}", $0A, 0
shell_project_inline_kw: .byte "INLINE"

shell_pascal_library: .byte "PASCAL.NLIB"
shell_pascal_library_end:
shell_tool_editor: .byte "NPEDIT.BIN"
shell_tool_editor_end:
shell_tool_assembler: .byte "NAS.BIN"
shell_tool_assembler_end:
shell_tool_linker: .byte "NL.BIN"
shell_tool_linker_end:

      .segment "CODE"

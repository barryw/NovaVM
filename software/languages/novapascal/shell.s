; =====================================================================
; NovaPascal native command shell.
;
; This file is included by the resident $C000 shell/compiler ROM. Commands
; load the editor, NAS, and NL standard binaries into the $1D00 tool slot.
; =====================================================================

SHELL_NAME_CAP = 64
SHELL_RUN_CWD_SIZE = SHELL_NAME_CAP + 1
SHELL_RUN_CWD_NAME_SIZE = 10

      .define shell_clear_lib_args nptool_clear_args

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
shell_project_plan:   .res NPP_PLAN_SIZE
.assert shell_project_plan = NPP_PLAN_BASE, error, "NPP project mailbox moved"

shell_project_out_len = NPP_PLAN_BASE + NPP_PLAN_OUTPUT_LEN
shell_project_output  = NPP_PLAN_BASE + NPP_PLAN_OUTPUT
shell_project_obj_len = NPP_PLAN_BASE + NPP_PLAN_OBJECT_LEN
shell_project_object  = NPP_PLAN_BASE + NPP_PLAN_OBJECT
shell_nas_define_len  = NPP_PLAN_BASE + NPP_PLAN_DEFINE_LEN
shell_nas_define      = NPP_PLAN_BASE + NPP_PLAN_DEFINE
shell_project_cfg_len = NPP_PLAN_BASE + NPP_PLAN_CONFIG_LEN
shell_project_config  = NPP_PLAN_BASE + NPP_PLAN_CONFIG
shell_project_map_len = NPP_PLAN_BASE + NPP_PLAN_MAP_LEN
shell_project_map     = NPP_PLAN_BASE + NPP_PLAN_MAP
shell_project_label_len = NPP_PLAN_BASE + NPP_PLAN_LABEL_LEN
shell_project_label   = NPP_PLAN_BASE + NPP_PLAN_LABEL
shell_project_optimize = NPP_PLAN_BASE + NPP_PLAN_OPTIMIZE
shell_project_asm      = NPP_PLAN_BASE + NPP_PLAN_ASM
shell_project_inline   = NPP_PLAN_BASE + NPP_PLAN_INLINE
shell_project_target   = NPP_PLAN_BASE + NPP_PLAN_TARGET
shell_local_unit_count = NPP_PLAN_BASE + NPP_PLAN_LOCAL_COUNT
shell_unit_path_count  = NPP_PLAN_BASE + NPP_PLAN_UNIT_PATH_COUNT
shell_unit_path1_len   = NPP_PLAN_BASE + NPP_PLAN_UNIT_PATHS
shell_unit_path1       = shell_unit_path1_len + 1
shell_unit_path2_len   = shell_unit_path1_len + NPP_UNIT_PATH_SIZE
shell_unit_path2       = shell_unit_path2_len + 1

shell_name:           .res SHELL_NAME_CAP
shell_name2:          .res SHELL_NAME_CAP
shell_project_main_obj:.res SHELL_NAME_CAP
shell_project_main_len:.res 1
shell_project_file:   .res SHELL_NAME_CAP
shell_project_file_len:.res 1
shell_project_dir:    .res SHELL_NAME_CAP
shell_project_dir_len:.res 1
shell_saved_cwd:      .res SHELL_NAME_CAP
shell_saved_cwd_len:  .res 1
shell_run_cwd_name_buf:.res SHELL_RUN_CWD_NAME_SIZE
shell_project_unit_index:.res 1
shell_project_scoped: .res 1
shell_program_entry:  .res 2
shell_program_xaddr:  .res 3
shell_program_allocated:.res 1
shell_number:          .res 2
shell_digit:           .res 1
shell_number_printed:  .res 1
shell_stage_cached:    .res 1

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
      STZ   LIB_EXEC_SP
      JSR   shell_reset_link_base
      JSR   shell_delete_run_cwd
      LDA   #$0C
      STA   VGC_CHAROUT
      JSR   repl_init
      LDA   #<shell_banner
      STA   p_word
      LDA   #>shell_banner
      STA   p_word+1
      JSR   print_z
      JMP   shell_loop

; A standalone application may replace every byte of the shell's BSS. Rebuild
; the small amount of resident state needed for the next prompt without
; clearing the program's final screen output or changing the restored CWD.
shell_warm_start:
      CLD
      LDX   #$FF
      TXS
      LDA   #ROMSWAP_PRIMARY
      STA   LIB_HOME_BANK
      STZ   LIB_RESIDENT
      STZ   LIB_EXEC_SP
      STZ   shell_project_scoped
      STZ   shell_program_allocated
      JSR   shell_reset_link_base
      JSR   repl_init
      JSR   shell_restore_run_cwd
      LDA   VGC_CURSX
      BEQ   shell_loop
      JSR   shell_newline

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
      shell_try shell_kw_newunit,  shell_cmd_newunit
      shell_try shell_kw_addunit,  shell_cmd_addunit
      shell_try shell_kw_delunit,  shell_cmd_delunit
      shell_try shell_kw_delproject, shell_cmd_delproject
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
      LDA   #shell_help_file_end-shell_help_file
      STA   shell_name_len
      LDX   #shell_help_file_end-shell_help_file
      LDA   #0
      STA   shell_name,X
@copy:
      DEX
      BPL   @copy_byte
      JMP   shell_type_name
@copy_byte:
      LDA   shell_help_file,X
      STA   shell_name,X
      BRA   @copy

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
      BCC   shell_type_name
      JMP   shell_loop
shell_type_name:
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
      JSR   shell_read_args
      BCS   @usage
      LDA   shell_name_len
      BEQ   @usage
      LDA   shell_name2_len
      BNE   @usage
      LDA   #NPP_OP_NEW
      JSR   shell_launch_project_op
      BCC   @done
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

shell_cmd_newunit:
      JSR   shell_read_args
      BCS   @usage
      LDA   shell_name_len
      BEQ   @usage
      LDA   shell_name2_len
      BNE   @usage
      STZ   shell_project_scoped
      JSR   shell_enter_user_root
      BCS   @file_error
      LDA   #NPP_OP_NEW_UNIT_PROJECT
      JSR   shell_launch_project_op
      BCC   @done
      JSR   shell_print_tool_error
      BRA   @done
@file_error:
      JSR   shell_print_file_error
      BRA   @done
@usage:
      JSR   shell_print_usage
@done:
      JSR   shell_leave_project
      JMP   shell_loop

shell_cmd_addunit:
      JSR   shell_read_args
      BCS   @usage
      LDA   shell_name_len
      BEQ   @usage
      LDA   shell_name2_len
      BEQ   @usage
      LDA   #NPP_OP_ADD_UNIT
      JSR   shell_launch_project_op_with_user_fallback
      BCC   @done
      JSR   shell_print_tool_error
      BRA   @done
@usage:
      JSR   shell_print_usage
@done:
      JMP   shell_loop

shell_cmd_delunit:
      JSR   shell_read_args
      BCS   @usage
      LDA   shell_name_len
      BEQ   @usage
      LDA   shell_name2_len
      BEQ   @usage
      LDA   #NPP_OP_DEL_UNIT
      JSR   shell_launch_project_op_with_user_fallback
      BCC   @done
      JSR   shell_print_tool_error
      BRA   @done
@usage:
      JSR   shell_print_usage
@done:
      JMP   shell_loop

shell_cmd_delproject:
      JSR   shell_read_args
      BCS   @usage
      LDA   shell_name_len
      BEQ   @usage
      LDA   shell_name2_len
      BNE   @usage
      LDA   #NPP_OP_DEL_PROJECT
      JSR   shell_launch_project_op_with_user_fallback
      BCC   @done
      JSR   shell_print_tool_error
      BRA   @done
@usage:
      JSR   shell_print_usage
@done:
      JMP   shell_loop

shell_cmd_edit:
      JSR   shell_require_name
      BCS   @done
      STZ   shell_project_scoped
      JSR   shell_name_has_dot
      BCS   @launch
      JSR   shell_enter_project
      BCS   @launch
      JSR   shell_save_project_file
      STZ   shell_name2_len
      STZ   shell_name2
      LDA   #NPP_OP_PARSE
      JSR   shell_launch_project_op
      BCS   @tool_error
      JSR   shell_copy_arg0_to_name
@launch:
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
      JSR   shell_leave_project
      JMP   shell_loop

shell_cmd_compile:
      JSR   shell_require_name
      BCS   @done
      STZ   shell_project_target
      STZ   shell_local_unit_count
      STZ   shell_project_file_len
      STZ   shell_unit_path_count
      LDA   #<shell_ext_s
      STA   p_word
      LDA   #>shell_ext_s
      STA   p_word+1
      JSR   shell_derive_name
      JSR   shell_compile_cached
      BCC   :+
      JMP   @tool_error
:
      JSR   shell_preprocess
      BCC   :+
      JMP   @tool_error
:
      JSR   shell_optimize
      BCC   :+
      JMP   @tool_error
:
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
      STZ   shell_project_scoped
      STZ   shell_project_out_len
      STZ   shell_project_obj_len
      STZ   shell_project_asm
      STZ   shell_nas_define_len
      STZ   shell_project_cfg_len
      STZ   shell_project_map_len
      STZ   shell_project_label_len
      STZ   shell_project_optimize
      STZ   shell_project_inline
      STZ   shell_project_target
      STZ   shell_local_unit_count
      STZ   shell_project_file_len
      STZ   shell_unit_path_count
      STZ   shell_unit_path1_len
      STZ   shell_unit_path1
      STZ   shell_unit_path2_len
      STZ   shell_unit_path2
      JSR   shell_name_has_dot
      BCS   @check_project_file
      JSR   shell_enter_project
      BCC   @project
      JMP   @file_error
@check_project_file:
      JSR   shell_name_is_npp
      long_bcc @source_build
@project:
      JSR   shell_save_project_file
      STZ   shell_name2_len
      STZ   shell_name2
      LDA   #NPP_OP_PARSE
      JSR   shell_launch_project_op
      BCC   @parsed
      JMP   @tool_error
@parsed:
      JSR   shell_copy_arg0_to_name
      LDA   NPP_PLAN_BASE+NPP_PLAN_LOAD_LO
      STA   NPTOOL_LINK_BASE
      LDA   NPP_PLAN_BASE+NPP_PLAN_LOAD_HI
      STA   NPTOOL_LINK_BASE+1
      JSR   shell_use_inline_config
      LDA   shell_project_cfg_len
      BEQ   @compile_project
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
      BCC   @compile_project
      JMP   @tool_error
@compile_project:
      JSR   shell_compile_unoptimized
      BCC   @units
      JMP   @tool_error
@units:
      STZ   shell_project_unit_index
@unit:
      LDA   shell_project_unit_index
      CMP   NPP_PLAN_BASE+NPP_PLAN_UNIT_COUNT
      BCS   @combine
      JSR   shell_restore_project_file
      STZ   shell_name2_len
      STZ   shell_name2
      JSR   shell_prepare_tool_args
      LDA   #NPP_OP_GET_UNIT
      STA   NPTOOL_FLAGS
      LDA   shell_project_unit_index
      STA   NPTOOL_DETAIL
      LDA   #<shell_tool_project
      STA   p_word
      LDA   #>shell_tool_project
      STA   p_word+1
      LDA   #shell_tool_project_end-shell_tool_project
      JSR   shell_launch_tool
      long_bcs @tool_error
      JSR   shell_copy_arg0_to_name
      JSR   shell_compile_unoptimized
      long_bcs @tool_error
      INC   shell_project_unit_index
      BRA   @unit
@combine:
      JSR   shell_restore_project_file
      LDA   shell_project_target
      CMP   #NPP_TARGET_UNIT
      BNE   @combine_default_name
      JSR   shell_use_project_output
      JSR   shell_promote_name2
      LDA   #<shell_ext_asm
      STA   p_word
      LDA   #>shell_ext_asm
      STA   p_word+1
      JSR   shell_derive_name
      JSR   shell_restore_project_file
      BRA   @combine_named
@combine_default_name:
      LDA   #<shell_ext_asm
      STA   p_word
      LDA   #>shell_ext_asm
      STA   p_word+1
      JSR   shell_derive_name
@combine_named:
      JSR   shell_prepare_tool_args
      LDA   #NPP_OP_COMBINE
      STA   NPTOOL_FLAGS
      LDA   #<shell_tool_project
      STA   p_word
      LDA   #>shell_tool_project
      STA   p_word+1
      LDA   #shell_tool_project_end-shell_tool_project
      JSR   shell_launch_tool
      long_bcs @tool_error
      JSR   shell_use_inline_config
      JSR   shell_preprocess
      long_bcs @tool_error
      JSR   shell_optimize
      long_bcs @tool_error
      JSR   shell_promote_name2
      JMP   @assemble

@source_build:
      STZ   shell_local_unit_count
      LDA   #<shell_ext_s
      STA   p_word
      LDA   #>shell_ext_s
      STA   p_word+1
      JSR   shell_derive_name
      JSR   shell_compile_cached
      BCC   :+
      JMP   @tool_error
:
      JSR   shell_preprocess
      BCC   :+
      JMP   @tool_error
:
      JSR   shell_optimize
      BCC   :+
      JMP   @tool_error
:
      JSR   shell_promote_name2
@assemble:
      LDA   #<shell_ext_obj
      STA   p_word
      LDA   #>shell_ext_obj
      STA   p_word+1
      JSR   shell_derive_name
      JSR   shell_assemble_cached_preprocessed
      BCS   @tool_error

      JSR   shell_promote_name2
      LDA   shell_project_target
      CMP   #NPP_TARGET_UNIT
      BEQ   @build_complete
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
      LDA   shell_project_target
      CMP   #NPP_TARGET_OVERLAY
      BNE   :+
      LDA   #NPTOOL_FLAG_OVERLAY_OUTPUT
      STA   NPTOOL_FLAGS
:
      LDA   #<shell_tool_linker
      STA   p_word
      LDA   #>shell_tool_linker
      STA   p_word+1
      LDA   #shell_tool_linker_end-shell_tool_linker
      JSR   shell_launch_tool
      BCS   @tool_error
@build_complete:
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
@done:
      JSR   shell_leave_project
      JMP   shell_loop

shell_cmd_run:
      JSR   shell_require_name
      long_bcs @done
      STZ   shell_project_scoped
      JSR   shell_stash_run_cwd
      long_bcs @memory_error
      JSR   shell_name_has_dot
      BCS   @check_manifest
      JSR   shell_enter_project
      BCC   @manifest
      BRA   @file_error
@check_manifest:
      JSR   shell_name_is_npp
      BCC   @load
@manifest:
      JSR   shell_save_project_file
      STZ   shell_name2_len
      STZ   shell_name2
      LDA   #NPP_OP_PARSE
      JSR   shell_launch_project_op
      BCS   @tool_error
      LDA   shell_project_target
      BNE   @bad_project
      LDA   shell_project_out_len
      BEQ   @bad_project
      JSR   shell_use_project_output
      JSR   shell_promote_name2
@load:
      ; The application owns keyboard focus after RUN. Do not leak the command
      ; line's Enter byte into its first ReadKey call.
      JSR   nui_drain_keys
      JSR   shell_load_program
      CMP   #1
      BEQ   @file_error
      CMP   #2
      BEQ   @bad_binary
      CMP   #3
      BEQ   @memory_error
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
      JSR   shell_exec_program
      BRA   @bad_binary
@bad_binary:
      LDA   #<shell_binary_error
      STA   p_word
      LDA   #>shell_binary_error
      STA   p_word+1
      JSR   print_z
      BRA   @done
@file_error:
      JSR   shell_print_file_error
      BRA   @done
@memory_error:
      JSR   shell_release_program
      LDA   #<shell_memory_error
      STA   p_word
      LDA   #>shell_memory_error
      STA   p_word+1
      JSR   print_z
      BRA   @done
@tool_error:
      JSR   shell_print_tool_error
      BRA   @done
@bad_project:
      LDA   #<shell_project_error
      STA   p_word
      LDA   #>shell_project_error
      STA   p_word+1
      JSR   print_z
@done:
      JSR   shell_leave_project
      JSR   shell_delete_run_cwd
      JMP   shell_loop

; ---------------------------------------------------------------------
; External tool execution.
; ---------------------------------------------------------------------
shell_print_usage:
      LDA   #<shell_usage
      STA   p_word
      LDA   #>shell_usage
      STA   p_word+1
      JMP   print_z

shell_name_has_dot:
      LDY   #0
@byte:
      CPY   shell_name_len
      BCS   @no
      LDA   shell_name,Y
      CMP   #'.'
      BEQ   @yes
      INY
      BRA   @byte
@yes:
      SEC
      RTS
@no:
      CLC
      RTS

shell_save_cwd:
      LDA   #MODULE_ID_FILES
      STA   LIB_MOD_ID
      LDA   #FILE_PWD
      STA   LIB_FN_ID
      JSR   LIB_LOADER_BAND
      LDA   LIB_STATUS
      BNE   @bad
      LDA   FIO_NAMELEN
      CMP   #SHELL_NAME_CAP
      BCS   @bad
      STA   shell_saved_cwd_len
      TAX
      STZ   shell_saved_cwd,X
@copy:
      DEX
      BMI   @ok
      LDA   FIO_NAME,X
      STA   shell_saved_cwd,X
      BRA   @copy
@ok:
      CLC
      RTS
@bad:
      LDA   FIO_ERRCODE
      STA   shell_fio_error
      SEC
      RTS

shell_enter_project:
      JSR   shell_save_project_name
      JSR   shell_save_cwd
      BCS   @bad
      LDA   #FILE_CD
      JSR   shell_call_file_name
      LDA   LIB_STATUS
      BEQ   @entered
      JSR   shell_set_user_project_path
      BCS   @bad
      LDA   #FILE_CD
      JSR   shell_call_file_name
      LDA   LIB_STATUS
      BNE   @bad
@entered:
      JSR   shell_restore_project_name
      INC   shell_project_scoped
      LDA   #<shell_ext_npp
      STA   p_word
      LDA   #>shell_ext_npp
      STA   p_word+1
      JSR   shell_derive_name
      JSR   shell_promote_name2
      CLC
      RTS
@bad:
      SEC
      RTS

; NEWUNIT always creates source projects below the shared /USER library root.
shell_enter_user_root:
      JSR   shell_save_project_name
      JSR   shell_save_cwd
      BCS   @bad
      LDA   #shell_user_root_end-shell_user_root
      STA   shell_name_len
      LDX   #0
@path:
      LDA   shell_user_root,X
      STA   shell_name,X
      INX
      CPX   #shell_user_root_end-shell_user_root
      BCC   @path
      STZ   shell_name,X
      LDA   #FILE_CD
      JSR   shell_call_file_name
      LDA   LIB_STATUS
      BNE   @bad
      JSR   shell_restore_project_name
      INC   shell_project_scoped
      CLC
      RTS
@bad:
      LDA   FIO_ERRCODE
      STA   shell_fio_error
      SEC
      RTS

shell_save_project_name:
      LDA   shell_name_len
      STA   shell_project_dir_len
      TAX
      STZ   shell_project_dir,X
@copy:
      DEX
      BMI   @done
      LDA   shell_name,X
      STA   shell_project_dir,X
      BRA   @copy
@done:
      RTS

shell_restore_project_name:
      LDA   shell_project_dir_len
      STA   shell_name_len
      TAX
      STZ   shell_name,X
@copy:
      DEX
      BMI   @done
      LDA   shell_project_dir,X
      STA   shell_name,X
      BRA   @copy
@done:
      RTS

shell_set_user_project_path:
      LDA   shell_project_dir_len
      CLC
      ADC   #shell_user_prefix_end-shell_user_prefix
      CMP   #SHELL_NAME_CAP
      BCS   @bad
      STA   shell_name_len
      LDX   #0
@prefix:
      LDA   shell_user_prefix,X
      STA   shell_name,X
      INX
      CPX   #shell_user_prefix_end-shell_user_prefix
      BCC   @prefix
      LDY   #0
@name:
      CPY   shell_project_dir_len
      BCS   @done
      LDA   shell_project_dir,Y
      STA   shell_name,X
      INX
      INY
      BRA   @name
@done:
      STZ   shell_name,X
      CLC
      RTS
@bad:
      SEC
      RTS

shell_leave_project:
      LDA   shell_project_scoped
      BEQ   @done
      STZ   shell_project_scoped
      LDA   shell_saved_cwd_len
      STA   shell_name_len
      TAX
      STZ   shell_name,X
@copy:
      DEX
      BMI   @cd
      LDA   shell_saved_cwd,X
      STA   shell_name,X
      BRA   @copy
@cd:
      LDA   #FILE_CD
      JSR   shell_call_file_name
      LDA   LIB_STATUS
      BEQ   @done
      JSR   shell_print_file_error
@done:
      RTS

; Standalone programs own lower RAM, including all shell BSS. Preserve the
; caller's directory through the NDK named-XRAM API so a project executable
; can use relative data files while it runs and the shell can still return to
; the directory from which RUN was issued.
shell_stash_run_cwd:
      JSR   shell_save_cwd
      BCS   @bad
      JSR   shell_run_cwd_args
      LDA   #<shell_saved_cwd
      STA   LIB_ARG1+2
      LDA   #>shell_saved_cwd
      STA   LIB_ARG1+3
      LDA   #SHELL_RUN_CWD_SIZE
      STA   LIB_ARG2+0
      LDA   #MODULE_ID_MEMORY
      STA   LIB_MOD_ID
      LDA   #MEM_NAMED_STASH
      STA   LIB_FN_ID
      JSR   LIB_LOADER_BAND
      LDA   LIB_STATUS
      BNE   @bad
      CLC
      RTS
@bad:
      SEC
      RTS

shell_restore_run_cwd:
      JSR   shell_run_cwd_args
      LDA   #<shell_saved_cwd
      STA   LIB_ARG1+2
      LDA   #>shell_saved_cwd
      STA   LIB_ARG1+3
      LDA   #SHELL_RUN_CWD_SIZE
      STA   LIB_ARG2+0
      LDA   #MODULE_ID_MEMORY
      STA   LIB_MOD_ID
      LDA   #MEM_NAMED_FETCH
      STA   LIB_FN_ID
      JSR   LIB_LOADER_BAND
      LDA   LIB_STATUS
      BNE   @delete
      LDA   shell_saved_cwd_len
      CMP   #SHELL_NAME_CAP
      BCS   @delete
      STA   shell_name_len
      TAX
      STZ   shell_name,X
@copy:
      DEX
      BMI   @cd
      LDA   shell_saved_cwd,X
      STA   shell_name,X
      BRA   @copy
@cd:
      LDA   #FILE_CD
      JSR   shell_call_file_name
@delete:
      ; The context has served its purpose even if the directory disappeared.
      ; Always release the allocator-owned named block.
shell_delete_run_cwd:
      JSR   shell_run_cwd_args
      LDA   #MODULE_ID_MEMORY
      STA   LIB_MOD_ID
      LDA   #MEM_NAMED_DELETE
      STA   LIB_FN_ID
      JSR   LIB_LOADER_BAND
      RTS

shell_run_cwd_args:
      JSR   shell_clear_lib_args
      LDX   #SHELL_RUN_CWD_NAME_SIZE-1
@name:
      LDA   shell_run_cwd_name,X
      STA   shell_run_cwd_name_buf,X
      DEX
      BPL   @name
      LDA   #<shell_run_cwd_name_buf
      STA   LIB_ARG0+0
      LDA   #>shell_run_cwd_name_buf
      STA   LIB_ARG0+1
      LDA   #SHELL_RUN_CWD_NAME_SIZE
      STA   LIB_ARG1+0
      RTS

shell_restore_project_file:
      LDA   shell_project_file_len
      STA   shell_name_len
      TAX
      STZ   shell_name,X
@copy:
      DEX
      BMI   @done
      LDA   shell_project_file,X
      STA   shell_name,X
      BRA   @copy
@done:
      RTS

shell_copy_arg0_to_name:
      LDA   NPTOOL_ARG0_LEN
      STA   shell_name_len
      TAX
      STZ   shell_name,X
@copy:
      DEX
      BMI   @done
      LDA   NPTOOL_ARG0,X
      STA   shell_name,X
      BRA   @copy
@done:
      RTS

shell_compile_unoptimized:
      LDA   #<shell_ext_asm
      STA   p_word
      LDA   #>shell_ext_asm
      STA   p_word+1
      JSR   shell_derive_name
      JMP   shell_compile_cached

shell_compile_cached:
      LDA   #<shell_identity_compiler
      STA   p_word
      LDA   #>shell_identity_compiler
      STA   p_word+1
      LDA   #shell_identity_compiler_end-shell_identity_compiler
      JSR   shell_cache_check
      BCS   @fail
      CMP   #NBUILD_CLEAN
      BEQ   @ok
      JSR   shell_prepare_tool_args
      JSR   shell_prepare_project_arg4
      JSR   shell_prepare_dependency_arg5
      LDA   #NPTOOL_FLAG_DEPENDENCIES
      STA   NPTOOL_FLAGS
      JSR   npc_compile_file
      BNE   @fail
      LDA   #<shell_identity_compiler
      STA   p_word
      LDA   #>shell_identity_compiler
      STA   p_word+1
      LDA   #shell_identity_compiler_end-shell_identity_compiler
      JSR   shell_cache_commit
      BCS   @fail
@ok:
      CLC
      RTS
@fail:
      SEC
      RTS

shell_assemble_cached_preprocessed:
      LDA   #NPTOOL_FLAG_SOURCE_PREPROCESSED
      BRA   shell_assemble_cached_common

shell_assemble_cached:
      LDA   #0
shell_assemble_cached_common:
      STA   shell_digit
      BEQ   @raw_identity
      LDA   #<shell_identity_assembler_preprocessed
      LDX   #>shell_identity_assembler_preprocessed
      LDY   #shell_identity_assembler_preprocessed_end-shell_identity_assembler_preprocessed
      BRA   @identity_ready
@raw_identity:
      LDA   #<shell_identity_assembler
      LDX   #>shell_identity_assembler
      LDY   #shell_identity_assembler_end-shell_identity_assembler
@identity_ready:
      STA   p_word
      STX   p_word+1
      TYA
      JSR   shell_cache_check
      BCS   @fail
      CMP   #NBUILD_CLEAN
      BEQ   @ok
      JSR   shell_prepare_tool_args
      JSR   shell_prepare_nas_project
      JSR   shell_prepare_dependency_arg2
      LDA   shell_digit
      ORA   #NPTOOL_FLAG_DEPENDENCIES
      STA   NPTOOL_FLAGS
      LDA   #<shell_tool_assembler
      STA   p_word
      LDA   #>shell_tool_assembler
      STA   p_word+1
      LDA   #shell_tool_assembler_end-shell_tool_assembler
      JSR   shell_launch_tool
      BCS   @fail
      LDA   shell_digit
      BEQ   @raw_commit_identity
      LDA   #<shell_identity_assembler_preprocessed
      LDX   #>shell_identity_assembler_preprocessed
      LDY   #shell_identity_assembler_preprocessed_end-shell_identity_assembler_preprocessed
      BRA   @commit_identity
@raw_commit_identity:
      LDA   #<shell_identity_assembler
      LDX   #>shell_identity_assembler
      LDY   #shell_identity_assembler_end-shell_identity_assembler
@commit_identity:
      STA   p_word
      STX   p_word+1
      TYA
      JSR   shell_cache_commit
      BCS   @fail
@ok:
      CLC
      RTS
@fail:
      SEC
      RTS

; A selects an NPP_OP_* operation. The two parsed shell arguments are passed
; through the ordinary disk-tool mailbox.
shell_launch_project_op:
      PHA
      JSR   shell_prepare_tool_args
      PLA
      STA   NPTOOL_FLAGS
      LDA   #<shell_tool_project
      STA   p_word
      LDA   #>shell_tool_project
      STA   p_word+1
      LDA   #shell_tool_project_end-shell_tool_project
      JMP   shell_launch_tool

; Project mutation commands first honor the current directory, then look in
; /USER so reusable unit projects behave like ordinary named projects.
shell_launch_project_op_with_user_fallback:
      STA   shell_digit
      JSR   shell_launch_project_op
      BCC   @ok
      LDA   NPTOOL_STATUS
      CMP   #NPTOOL_ERR_IO
      BNE   @fail
      LDA   NPTOOL_DETAIL
      CMP   #FIO_ERR_NOTFOUND
      BNE   @fail
      STZ   shell_project_scoped
      JSR   shell_enter_user_root
      BCS   @fail
      LDA   shell_digit
      JSR   shell_launch_project_op
      BCS   @leave_fail
      JSR   shell_leave_project
@ok:
      CLC
      RTS
@leave_fail:
      JSR   shell_leave_project
@fail:
      SEC
      RTS

; Exact-content cache shared by NPC, NAS, and later language shells. The
; output filename is also the stable node key because each stage has its own
; extension. A/p_word supplies the stage identity.
shell_cache_check:
      JSR   shell_prepare_cache_args
      LDA   #NBUILD_OP_CHECK
      STA   NPTOOL_FLAGS
      LDA   #<shell_tool_build
      STA   p_word
      LDA   #>shell_tool_build
      STA   p_word+1
      LDA   #shell_tool_build_end-shell_tool_build
      JSR   shell_launch_tool
      BCS   @fail
      LDA   NPTOOL_DETAIL
      CLC
      RTS
@fail:
      SEC
      RTS

shell_cache_commit:
      JSR   shell_prepare_cache_args
      LDA   #shell_build_dependencies_end-shell_build_dependencies
      STA   NPTOOL_ARG4_LEN
      LDY   #0
@manifest:
      LDA   shell_build_dependencies,Y
      STA   NPTOOL_ARG4,Y
      INY
      CPY   #shell_build_dependencies_end-shell_build_dependencies
      BCC   @manifest
      LDA   #0
      STA   NPTOOL_ARG4,Y
      LDA   #shell_build_temp_end-shell_build_temp
      STA   NPTOOL_ARG5_LEN
      LDY   #0
@temp:
      LDA   shell_build_temp,Y
      STA   NPTOOL_ARG5,Y
      INY
      CPY   #shell_build_temp_end-shell_build_temp
      BCC   @temp
      LDA   #0
      STA   NPTOOL_ARG5,Y
      LDA   #NBUILD_OP_COMMIT
      STA   NPTOOL_FLAGS
      LDA   #<shell_tool_build
      STA   p_word
      LDA   #>shell_tool_build
      STA   p_word+1
      LDA   #shell_tool_build_end-shell_tool_build
      JSR   shell_launch_tool
      BCS   @done
      JSR   shell_delete_dependency_manifest
      CLC
@done:
      RTS

; Preserve the identity pointer before filling the fixed NBUILD mailbox.
shell_prepare_cache_args:
      STA   shell_io_len
      LDA   p_word
      STA   p_copy_dst
      LDA   p_word+1
      STA   p_copy_dst+1
      STZ   NPTOOL_STATUS
      STZ   NPTOOL_DETAIL
      LDA   #shell_build_state_end-shell_build_state
      STA   NPTOOL_ARG0_LEN
      LDY   #0
@state:
      LDA   shell_build_state,Y
      STA   NPTOOL_ARG0,Y
      INY
      CPY   #shell_build_state_end-shell_build_state
      BCC   @state
      LDA   #0
      STA   NPTOOL_ARG0,Y
      LDA   shell_name2_len
      STA   NPTOOL_ARG1_LEN
      STA   NPTOOL_ARG2_LEN
      LDY   #0
@output:
      LDA   shell_name2,Y
      STA   NPTOOL_ARG1,Y
      STA   NPTOOL_ARG2,Y
      INY
      CPY   shell_name2_len
      BCC   @output
      LDA   #0
      STA   NPTOOL_ARG1,Y
      STA   NPTOOL_ARG2,Y
      LDA   shell_io_len
      STA   NPTOOL_ARG3_LEN
      LDY   #0
@identity:
      CPY   shell_io_len
      BCS   @identity_done
      LDA   (p_copy_dst),Y
      STA   NPTOOL_ARG3,Y
      INY
      BRA   @identity
@identity_done:
      LDA   #0
      STA   NPTOOL_ARG3,Y
      STZ   NPTOOL_ARG4_LEN
      STZ   NPTOOL_ARG4
      STZ   NPTOOL_ARG5_LEN
      STZ   NPTOOL_ARG5
      STZ   NPTOOL_ARG6_LEN
      STZ   NPTOOL_ARG6
      RTS

shell_prepare_dependency_arg2:
      LDA   #shell_build_dependencies_end-shell_build_dependencies
      STA   NPTOOL_ARG2_LEN
      LDY   #0
@copy:
      LDA   shell_build_dependencies,Y
      STA   NPTOOL_ARG2,Y
      INY
      CPY   #shell_build_dependencies_end-shell_build_dependencies
      BCC   @copy
      LDA   #0
      STA   NPTOOL_ARG2,Y
      RTS

shell_prepare_dependency_arg5:
      LDA   #shell_build_dependencies_end-shell_build_dependencies
      STA   NPTOOL_ARG5_LEN
      LDY   #0
@copy:
      LDA   shell_build_dependencies,Y
      STA   NPTOOL_ARG5,Y
      INY
      CPY   #shell_build_dependencies_end-shell_build_dependencies
      BCC   @copy
      LDA   #0
      STA   NPTOOL_ARG5,Y
      RTS

shell_delete_dependency_manifest:
      JSR   shell_clear_lib_args
      LDA   #<shell_build_dependencies
      STA   LIB_ARG0
      LDA   #>shell_build_dependencies
      STA   LIB_ARG0+1
      LDA   #shell_build_dependencies_end-shell_build_dependencies
      STA   LIB_ARG1
      LDA   #MODULE_ID_FILES
      STA   LIB_MOD_ID
      LDA   #FILE_FDELETE
      STA   LIB_FN_ID
      JSR   LIB_LOADER_BAND
      RTS

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

; Expand language-neutral NAS includes while the source is still typed IR.
; A standalone unit keeps that expanded stream in its .S artifact; ordinary
; programs preprocess in place. This lets O2 optimize across unit boundaries.
shell_preprocess:
      STZ   shell_stage_cached
      JSR   shell_promote_name2
      LDA   #<shell_ext_s
      STA   p_word
      LDA   #>shell_ext_s
      STA   p_word+1
      JSR   shell_derive_name
      LDA   #<shell_identity_preprocess
      STA   p_word
      LDA   #>shell_identity_preprocess
      STA   p_word+1
      LDA   #shell_identity_preprocess_end-shell_identity_preprocess
      JSR   shell_cache_check
      BCS   @done
      CMP   #NBUILD_CLEAN
      BNE   @build
      INC   shell_stage_cached
      CLC
      RTS
@build:
      JSR   shell_prepare_tool_args
      JSR   shell_prepare_nas_project
      JSR   shell_prepare_dependency_arg2
      LDA   #NPTOOL_FLAG_PREPROCESS_ONLY | NPTOOL_FLAG_DEPENDENCIES
      STA   NPTOOL_FLAGS
      LDA   #<shell_tool_assembler
      STA   p_word
      LDA   #>shell_tool_assembler
      STA   p_word+1
      LDA   #shell_tool_assembler_end-shell_tool_assembler
      JSR   shell_launch_tool
@done:
      RTS

; Every compile lowers compact typed IR and runs machine peepholes before the
; language-neutral assembler sees the resulting 65C02 source.
shell_optimize:
      LDA   shell_stage_cached
      BNE   @ok
      LDA   #<shell_tool_optimizer
      STA   p_word
      LDA   #>shell_tool_optimizer
      STA   p_word+1
      LDA   #shell_tool_optimizer_end-shell_tool_optimizer
      JSR   shell_launch_tool
      BCS   @done
      LDA   #<shell_identity_preprocess
      STA   p_word
      LDA   #>shell_identity_preprocess
      STA   p_word+1
      LDA   #shell_identity_preprocess_end-shell_identity_preprocess
      JSR   shell_cache_commit
@done:
      RTS
@ok:
      CLC
      RTS

shell_print_tool_error:
      LDA   NPTOOL_STATUS
      CMP   #NPTOOL_ERR_IO
      BEQ   @io
      CMP   #NPTOOL_ERR_COMPILE
      BEQ   @compile
      CMP   #NPTOOL_ERR_ASSEMBLE
      BNE   :+
      JMP   @assemble
:
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
      CMP   #NPTOOL_ERR_PROJECT
      BNE   :+
      JMP   @project
:
      CMP   #NPTOOL_ERR_BUILD
      BNE   :+
      JMP   @build
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
      LDA   NPTOOL_ARG6_LEN
      BEQ   @compile_primary
      LDA   #<NPTOOL_ARG6
      LDX   #>NPTOOL_ARG6
      BRA   @compile_name
@compile_primary:
      LDA   #<NPTOOL_ARG0
      LDX   #>NPTOOL_ARG0
@compile_name:
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
      CMP   #7
      BEQ   @assemble_capacity
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
      LDA   NPTOOL_ARG6_LEN
      BEQ   @assemble_symbol_message
      LDA   #<NPTOOL_ARG6
      STA   p_word
      LDA   #>NPTOOL_ARG6
      STA   p_word+1
      JSR   print_z
      LDA   #':'
      STA   VGC_CHAROUT
      LDA   #' '
      STA   VGC_CHAROUT
@assemble_symbol_message:
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
      BRA   @assemble_message
@assemble_capacity:
      LDA   #<shell_assemble_capacity
      LDX   #>shell_assemble_capacity
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
@project:
      LDA   NPTOOL_DETAIL
      CMP   #NPP_DETAIL_EXISTS
      BNE   :+
      LDA   #<shell_project_exists
      LDX   #>shell_project_exists
      BRA   @project_message
:
      CMP   #NPP_DETAIL_NOT_MEMBER
      BNE   :+
      LDA   #<shell_unit_not_member
      LDX   #>shell_unit_not_member
      BRA   @project_message
:
      CMP   #NPP_DETAIL_NESTED_DIR
      BNE   :+
      LDA   #<shell_project_nested
      LDX   #>shell_project_nested
      BRA   @project_message
:
      CMP   #NPP_DETAIL_TOO_MANY
      BNE   :+
      LDA   #<shell_too_many_units
      LDX   #>shell_too_many_units
      BRA   @project_message
:
      LDA   #<shell_project_error
      LDX   #>shell_project_error
@project_message:
      STA   p_word
      STX   p_word+1
      JMP   print_z
@build:
      LDA   NPTOOL_DETAIL
      CMP   #NBUILD_DETAIL_CAPACITY
      BNE   @build_generic
      LDA   #<shell_build_capacity
      LDX   #>shell_build_capacity
      BRA   @build_message
@build_generic:
      LDA   #<shell_build_error
      LDX   #>shell_build_error
@build_message:
      STA   p_word
      STX   p_word+1
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

; Stage a raw Nova executable in allocator-owned XRAM without consuming the
; shell document buffer or overwriting the resident shell's lower-RAM state.
; The NDK Memory module owns the destructive copy/transfer after validation.
; Returns A=0 success, A=1 file error, A=2 invalid executable, or A=3 XRAM error.
shell_load_program:
      STZ   shell_load_error
      STZ   shell_fio_error
      STZ   shell_program_allocated
      LDA   #<shell_name
      STA   p_word
      LDA   #>shell_name
      STA   p_word+1
      LDA   shell_name_len
      STA   shell_io_len
      JSR   shell_copy_fio_name
      BEQ   :+
      JMP   @open_failed
:
      LDA   #FIO_FILE_ACCESS_READ
      STA   FIO_DIRTYPE
      JSR   fio_fopen
      BEQ   :+
      JMP   @open_failed
:
      LDA   FIO_SRCL
      STA   shell_file_id
      LDA   FIO_SRCH
      STA   shell_file_id+1
      JSR   fio_fsize
      BEQ   :+
      JMP   @io_failed
:
      LDA   FIO_SIZE2
      BEQ   :+
      JMP   @too_large
:
      LDA   FIO_SIZEL
      STA   shell_file_len
      LDA   FIO_SIZEH
      STA   shell_file_len+1
      BNE   @read_header
      LDA   shell_file_len
      CMP   #3
      BCS   @read_header
      JMP   @bad
@read_header:
      JSR   shell_close_file
      BEQ   :+
      JMP   @open_failed
:
      JSR   shell_clear_lib_args
      LDA   shell_file_len+0
      STA   LIB_ARG2+0
      LDA   shell_file_len+1
      STA   LIB_ARG2+1
      LDA   #MODULE_ID_MEMORY
      STA   LIB_MOD_ID
      LDA   #MEM_ALLOC
      STA   LIB_FN_ID
      JSR   LIB_LOADER_BAND
      LDA   LIB_STATUS
      BEQ   :+
      JMP   @memory_failed
:
      LDX   #2
@save_xaddr:
      LDA   LIB_RESULT,X
      STA   shell_program_xaddr,X
      DEX
      BPL   @save_xaddr
      INC   shell_program_allocated

      JSR   shell_clear_lib_args
      LDA   #<shell_name
      STA   LIB_ARG0+0
      LDA   #>shell_name
      STA   LIB_ARG0+1
      LDA   shell_name_len
      STA   LIB_ARG1+0
      LDX   #2
@xload_addr:
      LDA   shell_program_xaddr,X
      STA   LIB_ARG2,X
      DEX
      BPL   @xload_addr
      LDA   shell_file_len+0
      STA   LIB_ARG3+0
      LDA   shell_file_len+1
      STA   LIB_ARG3+1
      LDA   #MODULE_ID_MEMORY
      STA   LIB_MOD_ID
      LDA   #MEM_XLOAD
      STA   LIB_FN_ID
      JSR   LIB_LOADER_BAND
      LDA   LIB_STATUS
      BEQ   :+
      JMP   @memory_failed
:

      JSR   shell_clear_lib_args
      LDX   #2
@header_addr:
      LDA   shell_program_xaddr,X
      STA   LIB_ARG0,X
      DEX
      BPL   @header_addr
      LDA   #MODULE_ID_MEMORY
      STA   LIB_MOD_ID
      LDA   #MEM_READ8
      STA   LIB_FN_ID
      JSR   LIB_LOADER_BAND
      LDA   LIB_STATUS
      BNE   @memory_failed
      LDA   LIB_RESULT
      STA   shell_program_entry
      INC   LIB_ARG0+0
      BNE   :+
      INC   LIB_ARG0+1
      BNE   :+
      INC   LIB_ARG0+2
:
      LDA   #MEM_READ8
      STA   LIB_FN_ID
      JSR   LIB_LOADER_BAND
      LDA   LIB_STATUS
      BNE   @memory_failed
      LDA   LIB_RESULT
      STA   shell_program_entry+1
      CMP   #>NOVA_APP_RAM_START
      BCC   @bad
      BNE   @base_ok
      LDA   shell_program_entry
      CMP   #<NOVA_APP_RAM_START
      BCC   @bad
@base_ok:
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
      TAX
      LDA   shell_program_entry+1
      ADC   p_left+1
      BCS   @bad
      CMP   #>NOVA_APP_RAM_END
      BCC   @load
      BNE   @bad
      TXA
      CMP   #<NOVA_APP_RAM_END
      BNE   @bad
@load:
      LDA   #0
      RTS
@bad:
      JSR   shell_release_program
      LDA   #2
      RTS
@too_large:
      LDA   #1
      STA   shell_load_error
      BRA   @file_failed
@io_failed:
      LDA   FIO_ERRCODE
      STA   shell_fio_error
@file_failed:
      JSR   shell_close_file
      LDA   #1
      RTS
@open_failed:
      LDA   FIO_ERRCODE
      STA   shell_fio_error
      LDA   #1
      RTS
@memory_failed:
      JSR   shell_release_program
      LDA   #3
      RTS

shell_release_program:
      LDA   shell_program_allocated
      BEQ   @done
      STZ   shell_program_allocated
      JSR   shell_clear_lib_args
      LDX   #2
@address:
      LDA   shell_program_xaddr,X
      STA   LIB_ARG0,X
      DEX
      BPL   @address
      LDA   shell_file_len+0
      STA   LIB_ARG2+0
      LDA   shell_file_len+1
      STA   LIB_ARG2+1
      LDA   #MODULE_ID_MEMORY
      STA   LIB_MOD_ID
      LDA   #MEM_RELEASE
      STA   LIB_FN_ID
      JSR   LIB_LOADER_BAND
@done:
      RTS

; MEM_EXEC_IMAGE consumes the staging allocation. Success transfers to the
; application; its final RTS flows through lib_call and cold-starts this shell.
shell_exec_program:
      STZ   shell_program_allocated
      JSR   shell_clear_lib_args
      LDX   #2
@address:
      LDA   shell_program_xaddr,X
      STA   LIB_ARG0,X
      DEX
      BPL   @address
      LDA   shell_file_len+0
      STA   LIB_ARG1+0
      LDA   shell_file_len+1
      STA   LIB_ARG1+1
      LDA   #<shell_warm_start
      STA   LIB_ARG2+0
      LDA   #>shell_warm_start
      STA   LIB_ARG2+1
      LDA   #MODULE_ID_MEMORY
      STA   LIB_MOD_ID
      LDA   #MEM_EXEC_IMAGE
      STA   LIB_FN_ID
      JSR   LIB_LOADER_BAND
      LDA   LIB_STATUS
      RTS

shell_reset_link_base:
      LDA   #<OUTPUT_BASE
      STA   NPTOOL_LINK_BASE
      LDA   #>OUTPUT_BASE
      STA   NPTOOL_LINK_BASE+1
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
      JSR   shell_assemble_cached
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
      JSR   shell_prepare_project_arg4
      LDA   shell_unit_path_count
      BNE   @configured_paths
      LDA   #shell_default_system_end-shell_default_system-1
      STA   NPTOOL_ARG3_LEN
      LDX   #0
@default_system:
      LDA   shell_default_system,X
      STA   NPTOOL_ARG3,X
      BEQ   @default_user_start
      INX
      BRA   @default_system
@default_user_start:
      LDA   #shell_default_user_end-shell_default_user-1
      STA   NPTOOL_ARG5_LEN
      LDX   #0
@default_user:
      LDA   shell_default_user,X
      STA   NPTOOL_ARG5,X
      BEQ   @paths_done
      INX
      BRA   @default_user
@configured_paths:
      LDA   shell_unit_path1_len
      STA   NPTOOL_ARG3_LEN
      LDX   #0
@path1:
      LDA   shell_unit_path1,X
      STA   NPTOOL_ARG3,X
      BEQ   @path2_start
      INX
      BRA   @path1
@path2_start:
      LDA   shell_unit_path2_len
      STA   NPTOOL_ARG5_LEN
      LDX   #0
@path2:
      LDA   shell_unit_path2,X
      STA   NPTOOL_ARG5,X
      BEQ   @paths_done
      INX
      BRA   @path2
@paths_done:
      RTS

shell_prepare_project_arg4:
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
      LDA   shell_name_len
      STA   shell_scan
      LDY   #0
@base:
      CPY   shell_name_len
      BCS   @extension
      LDA   shell_name,Y
      STA   shell_name2,Y
      CMP   #'/'
      BNE   @dot
      LDA   shell_name_len
      STA   shell_scan
      BRA   @next
@dot:
      CMP   #'.'
      BNE   @next
      STY   shell_scan
@next:
      INY
      BRA   @base
@extension:
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
shell_assemble_capacity:.byte "assembler table capacity exceeded.", $0D, $0A, 0
shell_link_error: .byte "Linker error.", $0D, $0A, 0
shell_link_config_error: .byte "Linker configuration error.", $0D, $0A, 0
shell_project_error: .byte "Invalid Pascal project.", $0D, $0A, 0
shell_project_exists: .byte "Project or unit already exists.", $0D, $0A, 0
shell_unit_not_member: .byte "Unit is not in the project.", $0D, $0A, 0
shell_project_nested: .byte "Project contains a nested directory.", $0D, $0A, 0
shell_too_many_units: .byte "Project has too many units.", $0D, $0A, 0
shell_build_error: .byte "Build state error.", $0D, $0A, 0
shell_build_capacity: .byte "Build state capacity exceeded.", $0D, $0A, 0
shell_binary_error: .byte "Not a runnable Nova binary.", $0D, $0A, 0
shell_tool_load_error: .byte "Tool load error $", 0
shell_decimal_divisors:
      .word 10000, 1000, 100, 10, 1

shell_kw_help:     .byte "HELP", 0
shell_kw_dir:      .byte "DIR", 0
shell_kw_pwd:      .byte "PWD", 0
shell_kw_cd:       .byte "CD", 0
shell_kw_type:     .byte "TYPE", 0
shell_kw_new:      .byte "NEW", 0
shell_kw_newunit:  .byte "NEWUNIT", 0
shell_kw_addunit:  .byte "ADDUNIT", 0
shell_kw_delunit:  .byte "DELUNIT", 0
shell_kw_delproject:.byte "DELPROJECT", 0
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

shell_ext_s:   .byte ".S", 0
shell_ext_asm: .byte ".ASM", 0
shell_ext_obj: .byte ".OBJ", 0
shell_ext_bin: .byte ".BIN", 0
shell_ext_npp: .byte ".NPP", 0
shell_project_inline_kw: .byte "INLINE"
shell_default_system: .byte "SYSTEM", 0
shell_default_system_end:
shell_default_user: .byte "USER", 0
shell_default_user_end:
shell_user_root: .byte "/USER"
shell_user_root_end:
shell_user_prefix: .byte "/USER/"
shell_user_prefix_end:
shell_run_cwd_name: .byte "__NPSH.CWD"
shell_run_cwd_name_end:
shell_help_file: .byte "/NPHELP.TXT"
shell_help_file_end:

shell_pascal_library: .byte "/SYSTEM/PASCAL.NLIB"
shell_pascal_library_end:
shell_build_state: .byte "BUILD.NBS"
shell_build_state_end:
shell_build_temp: .byte "BUILD.TMP"
shell_build_temp_end:
shell_build_dependencies: .byte "BUILD.DEP"
shell_build_dependencies_end:
shell_identity_compiler: .byte "NPC/1"
shell_identity_compiler_end:
shell_identity_preprocess: .byte "NASPP+NPO2/1"
shell_identity_preprocess_end:
shell_identity_assembler: .byte "NAS/1"
shell_identity_assembler_end:
shell_identity_assembler_preprocessed: .byte "NAS/1/PRE"
shell_identity_assembler_preprocessed_end:
shell_tool_editor: .byte "/NPEDIT.BIN"
shell_tool_editor_end:
shell_tool_build: .byte "/NBUILD.BIN"
shell_tool_build_end:
shell_tool_optimizer: .byte "/NPO2.BIN"
shell_tool_optimizer_end:
shell_tool_project: .byte "/NPPROJ.BIN"
shell_tool_project_end:
shell_tool_assembler: .byte "/NAS.BIN"
shell_tool_assembler_end:
shell_tool_linker: .byte "/NL.BIN"
shell_tool_linker_end:

      .segment "CODE"

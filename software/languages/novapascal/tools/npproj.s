; NovaPascal project service: NPP validation, project directories, unit
; membership, and whole-project assembly stream construction.

      .setcpu "w65c02"
      .include "nova.inc"
      .include "libfiles.inc"
      .include "nptool.inc"
      .include "longbranch.inc"
      .include "npproj.inc"

PROJECT_BUFFER_CAP = $1000

      .segment "ZEROPAGE"
project_src:       .res 2
project_left:      .res 2
project_ptr:       .res 2
project_aux:       .res 2
project_count:     .res 2

      .segment "BSS"
manifest_name:     .res NPP_NAME_CAP
manifest_len:      .res 1
project_name:      .res NPP_NAME_CAP
project_name_len:  .res 1
main_name:         .res NPP_NAME_CAP
main_name_len:     .res 1
temp_name:         .res NPP_NAME_CAP
temp_name_len:     .res 1
cwd_name:          .res NPP_NAME_CAP
cwd_name_len:      .res 1
token_buf:         .res NPP_NAME_CAP
token_len:         .res 1
unit_lens:         .res NPP_UNIT_CAP
unit_names:        .res NPP_UNIT_CAP * NPP_UNIT_FILE_CAP
unit_start_lo:     .res NPP_UNIT_CAP
unit_start_hi:     .res NPP_UNIT_CAP
unit_end_lo:       .res NPP_UNIT_CAP
unit_end_hi:       .res NPP_UNIT_CAP
main_insert:       .res 2
keyword_start:     .res 2
project_file_len:  .res 2
project_version:   .res 1
project_unit_count:.res 1
project_output_seen:.res 1
project_object_seen:.res 1
project_target_seen:.res 1
project_new_unit:  .res 1
project_dir_entered:.res 1
project_created:   .res 1
project_source_created:.res 1
project_manifest_created:.res 1
project_read_id:   .res 2
project_write_id:  .res 2
project_requested_unit:.res 1
project_requested_index:.res 1
project_error_detail:.res 1
project_hash:      .res 3
project_hash_saved:.res 3

      .segment "NOINIT"
project_buffer:    .res PROJECT_BUFFER_CAP

      .segment "CODE"
      .export tool_main

tool_main:
      LDA   NPTOOL_DETAIL
      PHA
      JSR   nptool_clear_bss
      PLA
      STA   project_requested_index
      STZ   NPTOOL_STATUS
      STZ   NPTOOL_DETAIL
      LDA   NPTOOL_FLAGS
      CMP   #NPP_OP_PARSE
      long_beq project_op_parse
      CMP   #NPP_OP_NEW
      long_beq project_op_new
      CMP   #NPP_OP_NEW_UNIT_PROJECT
      long_beq project_op_new_unit
      CMP   #NPP_OP_ADD_UNIT
      long_beq project_op_add_unit
      CMP   #NPP_OP_DEL_UNIT
      long_beq project_op_del_unit
      CMP   #NPP_OP_DEL_PROJECT
      long_beq project_op_del_project
      CMP   #NPP_OP_GET_UNIT
      long_beq project_op_get_unit
      CMP   #NPP_OP_COMBINE
      long_beq project_op_combine
      JMP   project_bad_args

project_op_parse:
      JSR   project_copy_manifest_arg
      long_bcs project_bad_args
      JSR   project_load_and_parse
      long_bcs project_fail
      JSR   project_publish_main
      LDA   #0
      RTS

project_op_get_unit:
      JSR   project_copy_manifest_arg
      long_bcs project_bad_args
      JSR   project_load_and_parse
      long_bcs project_fail
      LDX   project_requested_index
      CPX   project_unit_count
      long_bcs project_bad_args
      JSR   project_unit_pointer
      LDX   project_requested_index
      LDA   unit_lens,X
      STA   NPTOOL_ARG0_LEN
      TAY
      LDA   #0
      STA   NPTOOL_ARG0,Y
@copy:
      DEY
      BMI   @done
      LDA   (project_ptr),Y
      STA   NPTOOL_ARG0,Y
      BRA   @copy
@done:
      LDA   #0
      RTS

project_op_new_unit:
      INC   project_new_unit

project_op_new:
      JSR   project_require_one_identifier
      long_bcs project_bad_args
      JSR   project_copy_arg0_name
      JSR   project_save_cwd
      long_bcs project_io_fail
      JSR   project_cd_project
      BCS   @directory_absent
      LDA   #<cwd_name
      LDX   #>cwd_name
      LDY   cwd_name_len
      JSR   project_cd_named
      long_bcs project_io_fail
      LDA   #NPP_DETAIL_EXISTS
      STA   project_error_detail
      JMP   project_project_fail
@directory_absent:
      LDA   FIO_ERRCODE
      CMP   #FIO_ERR_NOTFOUND
      long_bne project_io_fail
      LDA   #<project_name
      LDX   #>project_name
      LDY   project_name_len
      JSR   project_mkdir_named
      long_bcs project_io_fail
      INC   project_created
      JSR   project_cd_project
      long_bcs @cleanup
      INC   project_dir_entered

      LDA   #<main_file_name
      LDX   #>main_file_name
      LDY   #main_file_name_end-main_file_name
      JSR   project_require_absent
      long_bcs @cleanup
      JSR   project_make_manifest_name
      LDA   #<temp_name
      LDX   #>temp_name
      LDY   temp_name_len
      JSR   project_require_absent
      long_bcs @cleanup

      JSR   project_build_main_source
      LDA   #<main_file_name
      LDX   #>main_file_name
      LDY   #main_file_name_end-main_file_name
      JSR   project_save_buffer
      long_bcs @cleanup
      INC   project_source_created

      JSR   project_build_manifest
      LDA   #<temp_name
      LDX   #>temp_name
      LDY   temp_name_len
      JSR   project_save_buffer
      long_bcs @cleanup
      INC   project_manifest_created
      JSR   project_restore_cwd
      long_bcs project_io_fail
      STZ   project_dir_entered
      LDA   #<message_created
      LDX   #>message_created
      JSR   project_print_arg0
      LDA   #0
      RTS
@cleanup:
      JSR   project_cleanup_new
      JMP   project_io_fail

project_op_add_unit:
      JSR   project_require_two_identifiers
      long_bcs project_bad_args
      JSR   project_reject_nova_unit
      long_bcs project_bad_args
      JSR   project_copy_arg0_name
      JSR   project_enter_named_project
      long_bcs project_io_fail
      JSR   project_load_and_parse_v2
      long_bcs @restore_fail
      LDA   project_unit_count
      CMP   #NPP_UNIT_CAP
      BCC   :+
      LDA   #NPP_DETAIL_TOO_MANY
      STA   project_error_detail
      JMP   @restore_project_fail
:
      JSR   project_find_arg1_unit
      BCC   @not_member
      LDA   #NPP_DETAIL_EXISTS
      STA   project_error_detail
      JMP   @restore_project_fail
@not_member:
      JSR   project_make_arg1_pas_name
      LDA   #<temp_name
      LDX   #>temp_name
      LDY   temp_name_len
      JSR   project_require_absent
      long_bcs @restore_fail
      JSR   project_build_unit_source
      LDA   #<temp_name
      LDX   #>temp_name
      LDY   temp_name_len
      JSR   project_save_buffer
      long_bcs @restore_fail
      JSR   project_reload_manifest
      long_bcs @delete_new
      JSR   project_insert_unit_line
      long_bcs @delete_new_project
      JSR   project_save_manifest_buffer
      long_bcs @delete_new
      JSR   project_delete_outputs
      JSR   project_restore_cwd
      long_bcs project_io_fail
      STZ   project_dir_entered
      LDA   #<message_added
      LDX   #>message_added
      JSR   project_print_arg1
      LDA   #0
      RTS
@delete_new_project:
      LDA   #NPP_DETAIL_INVALID
      STA   project_error_detail
@delete_new:
      LDA   #<temp_name
      LDX   #>temp_name
      LDY   temp_name_len
      JSR   project_delete_named_optional
@restore_fail:
      JSR   project_restore_cwd
      JMP   project_io_fail
@restore_project_fail:
      JSR   project_restore_cwd
      JMP   project_project_fail

project_op_del_unit:
      JSR   project_require_two_identifiers
      long_bcs project_bad_args
      JSR   project_copy_arg0_name
      JSR   project_enter_named_project
      long_bcs project_io_fail
      JSR   project_load_and_parse_v2
      long_bcs @restore_fail
      JSR   project_find_arg1_unit
      BCS   @found
      LDA   #NPP_DETAIL_NOT_MEMBER
      STA   project_error_detail
      JMP   @restore_project_fail
@found:
      STX   project_requested_unit
      JSR   project_remove_unit_line
      long_bcs @restore_fail
      JSR   project_save_manifest_buffer
      long_bcs @restore_fail
      JSR   project_make_arg1_pas_name
      LDA   #<temp_name
      LDX   #>temp_name
      LDY   temp_name_len
      JSR   project_delete_named
      long_bcs @restore_fail
      LDA   #<ext_asm
      LDX   #>ext_asm
      JSR   project_replace_temp_extension
      LDA   #<temp_name
      LDX   #>temp_name
      LDY   temp_name_len
      JSR   project_delete_named_optional
      LDA   #<ext_npi
      LDX   #>ext_npi
      JSR   project_replace_temp_extension
      LDA   #<temp_name
      LDX   #>temp_name
      LDY   temp_name_len
      JSR   project_delete_named_optional
      JSR   project_delete_outputs
      JSR   project_restore_cwd
      long_bcs project_io_fail
      STZ   project_dir_entered
      LDA   #<message_deleted_unit
      LDX   #>message_deleted_unit
      JSR   project_print_arg1
      LDA   #0
      RTS
@restore_fail:
      JSR   project_restore_cwd
      JMP   project_io_fail
@restore_project_fail:
      JSR   project_restore_cwd
      JMP   project_project_fail

project_op_del_project:
      JSR   project_require_one_identifier
      long_bcs project_bad_args
      JSR   project_copy_arg0_name
      JSR   project_enter_named_project
      long_bcs project_io_fail
      JSR   project_load_and_parse_v2
      long_bcs @restore_project_fail
      JSR   project_reject_nested_directories
      long_bcs @restore_fail
      JSR   project_delete_outputs
      JSR   project_delete_directory_files
      long_bcs @restore_fail
      JSR   project_restore_cwd
      long_bcs project_io_fail
      STZ   project_dir_entered
      LDA   #<project_name
      LDX   #>project_name
      LDY   project_name_len
      JSR   project_rmdir_named
      long_bcs project_io_fail
      LDA   #<message_deleted_project
      LDX   #>message_deleted_project
      JSR   project_print_arg0
      LDA   #0
      RTS
@restore_fail:
      JSR   project_restore_cwd
      JMP   project_io_fail
@restore_project_fail:
      JSR   project_restore_cwd
      JMP   project_project_fail

project_op_combine:
      LDA   NPTOOL_ARG1_LEN
      long_beq project_bad_args
      JSR   project_copy_manifest_arg
      long_bcs project_bad_args
      JSR   project_load_and_parse
      long_bcs project_fail
      JSR   project_open_combined_output
      long_bcs project_io_fail
      LDA   project_unit_count
      BEQ   @main
      LDA   NPP_PLAN_BASE+NPP_PLAN_TARGET
      CMP   #NPP_TARGET_UNIT
      BEQ   @units
      JSR   project_write_entry_jump
      BCS   @close_fail
@units:
      STZ   project_requested_unit
@unit:
      LDX   project_requested_unit
      CPX   project_unit_count
      BCS   @main
      JSR   project_unit_pointer
      LDX   project_requested_unit
      LDY   unit_lens,X
      LDA   project_ptr
      LDX   project_ptr+1
      JSR   project_copy_named_to_temp
      LDA   #<ext_asm
      LDX   #>ext_asm
      JSR   project_replace_temp_extension
      JSR   project_append_temp_file
      BCS   @close_fail
      INC   project_requested_unit
      BRA   @unit
@main:
      LDA   #<main_name
      LDX   #>main_name
      LDY   main_name_len
      JSR   project_copy_named_to_temp
      LDA   #<ext_asm
      LDX   #>ext_asm
      JSR   project_replace_temp_extension
      JSR   project_append_temp_file
      BCS   @close_fail
@done:
      JSR   project_close_write
      long_bcs project_io_fail
      LDA   #<message_combining
      LDX   #>message_combining
      JSR   nptool_print_z
      LDA   #<NPTOOL_ARG1
      LDX   #>NPTOOL_ARG1
      JSR   nptool_print_z
      JSR   nptool_newline
      LDA   #0
      RTS
@close_fail:
      JSR   project_close_write
      JMP   project_io_fail

; ---------------------------------------------------------------------
; Project parsing.
; ---------------------------------------------------------------------
project_copy_manifest_arg:
      LDA   NPTOOL_ARG0_LEN
      BEQ   @bad
      CMP   #NPP_NAME_CAP
      BCS   @bad
      STA   manifest_len
      TAX
      LDA   #0
      STA   manifest_name,X
@copy:
      DEX
      BMI   @ok
      LDA   NPTOOL_ARG0,X
      STA   manifest_name,X
      BRA   @copy
@ok:
      CLC
      RTS
@bad:
      SEC
      RTS

project_load_and_parse_v2:
      JSR   project_make_manifest_name
      LDA   temp_name_len
      STA   manifest_len
      TAX
      LDA   #0
      STA   manifest_name,X
@copy:
      DEX
      BMI   @load
      LDA   temp_name,X
      STA   manifest_name,X
      BRA   @copy
@load:
      JSR   project_load_and_parse
      BCS   @fail
      LDA   project_version
      CMP   #NPP_VERSION_2
      BEQ   @ok
      LDA   #NPP_DETAIL_VERSION
      STA   project_error_detail
@fail:
      SEC
      RTS
@ok:
      CLC
      RTS

project_reload_manifest:
      LDA   #<manifest_name
      STA   project_ptr
      LDA   #>manifest_name
      STA   project_ptr+1
      LDA   manifest_len
      JMP   project_load_named

project_load_and_parse:
      JSR   project_reload_manifest
      BCS   @fail
      JMP   project_parse
@fail:
      RTS

project_load_named:
      STA   NPTOOL_ARG0_LEN
      TAY
      LDA   #0
      STA   NPTOOL_ARG0,Y
@copy:
      DEY
      BMI   @load
      LDA   (project_ptr),Y
      STA   NPTOOL_ARG0,Y
      BRA   @copy
@load:
      LDA   #<project_buffer
      STA   NPTOOL_IO_ADDR
      LDA   #>project_buffer
      STA   NPTOOL_IO_ADDR+1
      LDA   #<PROJECT_BUFFER_CAP
      STA   NPTOOL_IO_CAP
      LDA   #>PROJECT_BUFFER_CAP
      STA   NPTOOL_IO_CAP+1
      JSR   nptool_load_arg0
      BNE   @bad
      LDA   NPTOOL_IO_LEN
      STA   project_file_len
      LDA   NPTOOL_IO_LEN+1
      STA   project_file_len+1
      CLC
      RTS
@bad:
      SEC
      RTS

project_parse:
      JSR   project_clear_plan
      STZ   project_unit_count
      STZ   project_output_seen
      STZ   project_object_seen
      LDA   #<project_buffer
      STA   project_src
      LDA   #>project_buffer
      STA   project_src+1
      LDA   project_file_len
      STA   project_left
      LDA   project_file_len+1
      STA   project_left+1
      JSR   parser_next_token
      long_bcs @bad
      LDA   #<word_npp
      LDX   #>word_npp
      JSR   token_is
      long_bcc @bad
      JSR   parser_next_token
      long_bcs @bad
      LDA   token_len
      CMP   #1
      long_bne @bad
      LDA   token_buf
      CMP   #'1'
      BEQ   @v1
      CMP   #'2'
      long_bne @version_bad
      LDA   #NPP_VERSION_2
      BRA   @version
@v1:
      LDA   #NPP_VERSION_1
@version:
      STA   project_version
      STA   NPP_PLAN_BASE+NPP_PLAN_VERSION
      JSR   parser_finish_line
      long_bcs @bad
      JSR   parser_next_token
      long_bcs @bad
      LDA   #<word_main
      LDX   #>word_main
      JSR   token_is
      long_bcc @bad
      JSR   parser_next_token
      long_bcs @bad
      LDA   #<main_name
      LDX   #>main_name
      JSR   parser_copy_token
      STA   main_name_len
      JSR   project_validate_pas_filename
      long_bcs @bad
      JSR   parser_finish_line
      long_bcs @bad
      SEC
      LDA   project_src
      SBC   #<project_buffer
      STA   main_insert
      LDA   project_src+1
      SBC   #>project_buffer
      STA   main_insert+1

@directive:
      JSR   parser_skip_ws
      LDA   project_left
      ORA   project_left+1
      long_beq @finish
      LDA   project_src
      STA   keyword_start
      LDA   project_src+1
      STA   keyword_start+1
      JSR   parser_read_token
      long_bcs @bad
      LDA   #<word_memory
      LDX   #>word_memory
      JSR   token_is
      long_bcs @memory
      LDA   #<word_unit
      LDX   #>word_unit
      JSR   token_is
      long_bcs @unit
      LDA   #<word_unitpath
      LDX   #>word_unitpath
      JSR   token_is
      long_bcs @unitpath
      LDA   #<word_overlay
      LDX   #>word_overlay
      JSR   token_is
      long_bcs @overlay
      LDA   #<word_target
      LDX   #>word_target
      JSR   token_is
      long_bcs @target
      LDA   #<word_output
      LDX   #>word_output
      JSR   token_is
      long_bcs @output
      LDA   #<word_optimize
      LDX   #>word_optimize
      JSR   token_is
      long_bcs @optimize
      LDA   #<word_define
      LDX   #>word_define
      JSR   token_is
      long_bcs @define
      LDA   #<word_object
      LDX   #>word_object
      JSR   token_is
      long_bcs @object
      LDA   #<word_asm
      LDX   #>word_asm
      JSR   token_is
      long_bcs @asm
      LDA   #<word_load
      LDX   #>word_load
      JSR   token_is
      long_bcs @load
      LDA   #<word_config
      LDX   #>word_config
      JSR   token_is
      long_bcs @config
      LDA   #<word_map
      LDX   #>word_map
      JSR   token_is
      long_bcs @map
      LDA   #<word_label
      LDX   #>word_label
      JSR   token_is
      long_bcs @label
      JMP   @bad

@unitpath:
      LDX   NPP_PLAN_BASE+NPP_PLAN_UNIT_PATH_COUNT
      CPX   #NPP_UNIT_PATH_CAP
      long_bcs @bad
      LDA   NPP_PLAN_BASE+NPP_PLAN_UNIT_PATH_COUNT
      BNE   @unitpath_second
      LDA   #<(NPP_PLAN_BASE+NPP_PLAN_UNIT_PATHS+1)
      LDX   #>(NPP_PLAN_BASE+NPP_PLAN_UNIT_PATHS+1)
      BRA   @unitpath_parse
@unitpath_second:
      LDA   #<(NPP_PLAN_BASE+NPP_PLAN_UNIT_PATHS+NPP_UNIT_PATH_SIZE+1)
      LDX   #>(NPP_PLAN_BASE+NPP_PLAN_UNIT_PATHS+NPP_UNIT_PATH_SIZE+1)
@unitpath_parse:
      JSR   project_parse_plan_value
      long_bcs @bad
      INC   NPP_PLAN_BASE+NPP_PLAN_UNIT_PATH_COUNT
      JMP   @directive

@overlay:
      LDA   project_version
      CMP   #NPP_VERSION_2
      long_bne @bad
      LDX   NPP_PLAN_BASE+NPP_PLAN_OVERLAY_COUNT
      CPX   #NPP_OVERLAY_CAP
      long_bcs @too_many
      JSR   parser_next_token
      long_bcs @bad
      LDA   token_len
      CMP   #1
      long_bne @bad
      LDA   token_buf
      CMP   #'1'
      long_bcc @bad
      CMP   #'0'+NPP_OVERLAY_GROUP_CAP+1
      long_bcs @bad
      SEC
      SBC   #'0'
      STA   project_requested_index
      JSR   parser_next_token
      long_bcs @bad
      LDA   token_len
      long_beq @bad
      CMP   #NPP_OVERLAY_FILE_CAP
      long_bcs @bad
      LDX   NPP_PLAN_BASE+NPP_PLAN_OVERLAY_COUNT
      LDA   project_requested_index
      STA   NPP_PLAN_BASE+NPP_PLAN_OVERLAY_GROUPS,X
      JSR   project_publish_overlay_hash
      JSR   parser_finish_line
      long_bcs @bad
      INC   NPP_PLAN_BASE+NPP_PLAN_OVERLAY_COUNT
      JMP   @directive

@target:
      LDA   project_version
      CMP   #NPP_VERSION_2
      long_bne @bad
      LDA   project_target_seen
      long_bne @bad
      INC   project_target_seen
      JSR   parser_next_token
      long_bcs @bad
      LDA   #<word_program
      LDX   #>word_program
      JSR   token_is
      BCS   @target_end
      LDA   #<word_unit_target
      LDX   #>word_unit_target
      JSR   token_is
      long_bcc @bad
      LDA   #NPP_TARGET_UNIT
      STA   NPP_PLAN_BASE+NPP_PLAN_TARGET
@target_end:
      JSR   parser_finish_line
      long_bcs @bad
      JMP   @directive

@unit:
      LDA   project_version
      CMP   #NPP_VERSION_2
      long_bne @bad
      LDX   project_unit_count
      CPX   #NPP_UNIT_CAP
      long_bcs @too_many
      SEC
      LDA   keyword_start
      SBC   #<project_buffer
      STA   unit_start_lo,X
      LDA   keyword_start+1
      SBC   #>project_buffer
      STA   unit_start_hi,X
      JSR   parser_next_token
      long_bcs @bad
      JSR   project_validate_unit_filename
      long_bcs @bad
      JSR   project_store_unit_token
      long_bcs @bad
      JSR   project_publish_unit_hash
      JSR   parser_finish_line
      long_bcs @bad
      LDX   project_unit_count
      SEC
      LDA   project_src
      SBC   #<project_buffer
      STA   unit_end_lo,X
      LDA   project_src+1
      SBC   #>project_buffer
      STA   unit_end_hi,X
      INC   project_unit_count
      JMP   @directive

@output:
      LDA   project_output_seen
      long_bne @bad
      INC   project_output_seen
      LDA   #<(NPP_PLAN_BASE+NPP_PLAN_OUTPUT)
      LDX   #>(NPP_PLAN_BASE+NPP_PLAN_OUTPUT)
      JSR   project_parse_plan_value
      long_bcs @bad
      JMP   @directive
@optimize:
      LDA   NPP_PLAN_BASE+NPP_PLAN_OPTIMIZE
      long_bne @bad
      JSR   parser_next_token
      long_bcs @bad
      LDA   #<word_o2
      LDX   #>word_o2
      JSR   token_is
      long_bcc @bad
      LDA   #2
      STA   NPP_PLAN_BASE+NPP_PLAN_OPTIMIZE
      JSR   parser_finish_line
      long_bcs @bad
      JMP   @directive
@define:
      LDA   NPP_PLAN_BASE+NPP_PLAN_DEFINE_LEN
      long_bne @bad
      LDA   #<(NPP_PLAN_BASE+NPP_PLAN_DEFINE)
      LDX   #>(NPP_PLAN_BASE+NPP_PLAN_DEFINE)
      JSR   project_parse_plan_value
      long_bcs @bad
      JMP   @directive
@object:
      STZ   NPP_PLAN_BASE+NPP_PLAN_ASM
      BRA   @object_common
@asm:
      LDA   #1
      STA   NPP_PLAN_BASE+NPP_PLAN_ASM
@object_common:
      LDA   project_object_seen
      long_bne @bad
      INC   project_object_seen
      LDA   #<(NPP_PLAN_BASE+NPP_PLAN_OBJECT)
      LDX   #>(NPP_PLAN_BASE+NPP_PLAN_OBJECT)
      JSR   project_parse_plan_value
      long_bcs @bad
      JMP   @directive
@load:
      JSR   parser_next_token
      long_bcs @bad
      JSR   project_parse_load
      long_bcs @bad
      JSR   parser_finish_line
      long_bcs @bad
      JMP   @directive
@config:
      LDA   NPP_PLAN_BASE+NPP_PLAN_CONFIG_LEN
      long_bne @bad
      LDA   #<(NPP_PLAN_BASE+NPP_PLAN_CONFIG)
      LDX   #>(NPP_PLAN_BASE+NPP_PLAN_CONFIG)
      JSR   project_parse_plan_value
      long_bcs @bad
      LDA   #<word_inline
      LDX   #>word_inline
      JSR   token_is
      long_bcc @directive
      INC   NPP_PLAN_BASE+NPP_PLAN_INLINE
      JMP   @directive
@map:
      LDA   NPP_PLAN_BASE+NPP_PLAN_MAP_LEN
      long_bne @bad
      LDA   #<(NPP_PLAN_BASE+NPP_PLAN_MAP)
      LDX   #>(NPP_PLAN_BASE+NPP_PLAN_MAP)
      JSR   project_parse_plan_value
      long_bcs @bad
      JMP   @directive
@label:
      LDA   NPP_PLAN_BASE+NPP_PLAN_LABEL_LEN
      long_bne @bad
      LDA   #<(NPP_PLAN_BASE+NPP_PLAN_LABEL)
      LDX   #>(NPP_PLAN_BASE+NPP_PLAN_LABEL)
      JSR   project_parse_plan_value
      long_bcs @bad
      JMP   @directive
@memory:
      LDA   NPP_PLAN_BASE+NPP_PLAN_INLINE
      long_beq @bad
      JMP   @finish
@finish:
      LDA   project_output_seen
      long_beq @bad
      LDA   project_unit_count
      STA   NPP_PLAN_BASE+NPP_PLAN_UNIT_COUNT
      STA   NPP_PLAN_BASE+NPP_PLAN_LOCAL_COUNT
      CLC
      RTS
@version_bad:
      LDA   #NPP_DETAIL_VERSION
      STA   project_error_detail
      SEC
      RTS
@too_many:
      LDA   #NPP_DETAIL_TOO_MANY
      STA   project_error_detail
      SEC
      RTS
@bad:
      LDA   #NPP_DETAIL_INVALID
      STA   project_error_detail
      SEC
      RTS

project_clear_plan:
      STZ   NPP_PLAN_BASE+NPP_PLAN_OUTPUT_LEN
      STZ   NPP_PLAN_BASE+NPP_PLAN_OBJECT_LEN
      STZ   NPP_PLAN_BASE+NPP_PLAN_DEFINE_LEN
      STZ   NPP_PLAN_BASE+NPP_PLAN_CONFIG_LEN
      STZ   NPP_PLAN_BASE+NPP_PLAN_MAP_LEN
      STZ   NPP_PLAN_BASE+NPP_PLAN_LABEL_LEN
      STZ   NPP_PLAN_BASE+NPP_PLAN_VERSION
      STZ   NPP_PLAN_BASE+NPP_PLAN_UNIT_COUNT
      STZ   NPP_PLAN_BASE+NPP_PLAN_OPTIMIZE
      STZ   NPP_PLAN_BASE+NPP_PLAN_ASM
      STZ   NPP_PLAN_BASE+NPP_PLAN_INLINE
      STZ   NPP_PLAN_BASE+NPP_PLAN_UNIT_PATH_COUNT
      STZ   NPP_PLAN_BASE+NPP_PLAN_TARGET
      STZ   NPP_PLAN_BASE+NPP_PLAN_LOCAL_COUNT
      STZ   NPP_PLAN_BASE+NPP_PLAN_OVERLAY_COUNT
      STZ   NPP_PLAN_BASE+NPP_PLAN_UNIT_PATHS
      STZ   NPP_PLAN_BASE+NPP_PLAN_UNIT_PATHS+NPP_UNIT_PATH_SIZE
      LDA   #$00
      STA   NPP_PLAN_BASE+NPP_PLAN_LOAD_LO
      LDA   #$70
      STA   NPP_PLAN_BASE+NPP_PLAN_LOAD_HI
      RTS

; A/X is the plan string destination; its length byte immediately precedes it.
project_parse_plan_value:
      STA   project_ptr
      STX   project_ptr+1
      SEC
      LDA   project_ptr
      SBC   #1
      STA   project_aux
      LDA   project_ptr+1
      SBC   #0
      STA   project_aux+1
      JSR   parser_next_token
      BCS   @bad
      LDA   project_ptr
      LDX   project_ptr+1
      JSR   parser_copy_token
      LDY   #0
      STA   (project_aux),Y
      JSR   parser_finish_line
@bad:
      RTS

project_parse_load:
      LDA   token_len
      CMP   #5
      BNE   @bad
      LDA   token_buf
      CMP   #'$'
      BNE   @bad
      STZ   project_count
      STZ   project_count+1
      LDX   #1
@digit:
      LDA   token_buf,X
      JSR   project_hex_nibble
      BCS   @bad
      STA   project_requested_unit
      ASL   project_count
      ROL   project_count+1
      ASL   project_count
      ROL   project_count+1
      ASL   project_count
      ROL   project_count+1
      ASL   project_count
      ROL   project_count+1
      LDA   project_count
      ORA   project_requested_unit
      STA   project_count
      INX
      CPX   #5
      BCC   @digit
      LDA   project_count
      STA   NPP_PLAN_BASE+NPP_PLAN_LOAD_LO
      LDA   project_count+1
      STA   NPP_PLAN_BASE+NPP_PLAN_LOAD_HI
      CLC
      RTS
@bad:
      SEC
      RTS

project_publish_main:
      LDA   main_name_len
      STA   NPTOOL_ARG0_LEN
      TAX
      LDA   #0
      STA   NPTOOL_ARG0,X
@copy:
      DEX
      BMI   @done
      LDA   main_name,X
      STA   NPTOOL_ARG0,X
      BRA   @copy
@done:
      RTS

; ---------------------------------------------------------------------
; Tokenizer and validators.
; ---------------------------------------------------------------------
parser_next_token:
      JSR   parser_skip_ws
parser_read_token:
      STZ   token_len
@byte:
      LDA   project_left
      ORA   project_left+1
      BEQ   @done
      LDY   #0
      LDA   (project_src),Y
      CMP   #' '
      BEQ   @done
      CMP   #$09
      BEQ   @done
      CMP   #$0D
      BEQ   @done
      CMP   #$0A
      BEQ   @done
      LDX   token_len
      CPX   #NPP_NAME_CAP-1
      BCS   @bad
      STA   token_buf,X
      INC   token_len
      JSR   parser_consume
      BRA   @byte
@done:
      LDA   token_len
      BEQ   @bad
      TAX
      STZ   token_buf,X
      CLC
      RTS
@bad:
      SEC
      RTS

parser_skip_ws:
@byte:
      LDA   project_left
      ORA   project_left+1
      BEQ   @done
      LDY   #0
      LDA   (project_src),Y
      CMP   #' '
      BEQ   @eat
      CMP   #$09
      BEQ   @eat
      CMP   #$0D
      BEQ   @eat
      CMP   #$0A
      BNE   @done
@eat:
      JSR   parser_consume
      BRA   @byte
@done:
      RTS

parser_finish_line:
@byte:
      LDA   project_left
      ORA   project_left+1
      BEQ   @ok
      LDY   #0
      LDA   (project_src),Y
      CMP   #' '
      BEQ   @eat
      CMP   #$09
      BEQ   @eat
      CMP   #$0D
      BEQ   @eat
      CMP   #$0A
      BEQ   @lf
      SEC
      RTS
@eat:
      JSR   parser_consume
      BRA   @byte
@lf:
      JSR   parser_consume
@ok:
      CLC
      RTS

parser_consume:
      INC   project_src
      BNE   :+
      INC   project_src+1
:     LDA   project_left
      BNE   :+
      DEC   project_left+1
:     DEC   project_left
      RTS

; A/X points at an uppercase Z word. Carry set on case-insensitive equality.
token_is:
      STA   project_ptr
      STX   project_ptr+1
      LDY   #0
@byte:
      LDA   (project_ptr),Y
      BEQ   @end
      CPY   token_len
      BCS   @no
      STA   project_requested_unit
      LDA   token_buf,Y
      JSR   project_upper
      CMP   project_requested_unit
      BNE   @no
      INY
      BRA   @byte
@end:
      CPY   token_len
      BNE   @no
      SEC
      RTS
@no:
      CLC
      RTS

; A/X destination, returns token length in A.
parser_copy_token:
      STA   project_ptr
      STX   project_ptr+1
      LDY   #0
@copy:
      CPY   token_len
      BCS   @done
      LDA   token_buf,Y
      STA   (project_ptr),Y
      INY
      BRA   @copy
@done:
      LDA   #0
      STA   (project_ptr),Y
      TYA
      RTS

project_validate_pas_filename:
      LDA   token_len
      CMP   #5
      BCC   @bad
      SEC
      SBC   #4
      CMP   #NPP_UNIT_NAME_CAP
      BCS   @bad
      TAX
      LDA   token_buf,X
      CMP   #'.'
      BNE   @bad
      INX
      LDA   token_buf,X
      JSR   project_upper
      CMP   #'P'
      BNE   @bad
      INX
      LDA   token_buf,X
      JSR   project_upper
      CMP   #'A'
      BNE   @bad
      INX
      LDA   token_buf,X
      JSR   project_upper
      CMP   #'S'
      BNE   @bad
      LDA   #<token_buf
      LDX   #>token_buf
      LDY   token_len
      DEY
      DEY
      DEY
      DEY
      JMP   project_validate_identifier_named
@bad:
      SEC
      RTS

project_validate_unit_filename:
      JSR   project_validate_pas_filename
      BCS   @bad
      LDA   token_len
      SEC
      SBC   #4
      CMP   #5
      BCC   @ok
      LDA   token_buf
      JSR   project_upper
      CMP   #'N'
      BNE   @ok
      LDA   token_buf+1
      JSR   project_upper
      CMP   #'O'
      BNE   @ok
      LDA   token_buf+2
      JSR   project_upper
      CMP   #'V'
      BNE   @ok
      LDA   token_buf+3
      JSR   project_upper
      CMP   #'A'
      BEQ   @bad
@ok:
      CLC
      RTS
@bad:
      SEC
      RTS

project_store_unit_token:
      LDX   #0
@duplicate:
      CPX   project_unit_count
      BCS   @store
      STX   project_requested_unit
      JSR   project_unit_pointer
      LDX   project_requested_unit
      LDA   unit_lens,X
      CMP   token_len
      BNE   @next
      LDY   #0
@compare:
      CPY   token_len
      BCS   @bad
      LDA   (project_ptr),Y
      JSR   project_upper
      STA   project_aux
      LDA   token_buf,Y
      JSR   project_upper
      CMP   project_aux
      BNE   @next
      INY
      BRA   @compare
@next:
      LDX   project_requested_unit
      INX
      BRA   @duplicate
@store:
      STX   project_requested_unit
      JSR   project_unit_pointer
      LDX   project_requested_unit
      LDA   token_len
      STA   unit_lens,X
      LDY   #0
@copy:
      CPY   token_len
      BCS   @done
      LDA   token_buf,Y
      STA   (project_ptr),Y
      INY
      BRA   @copy
@done:
      CLC
      RTS
@bad:
      SEC
      RTS

; Publish the compiler's case-insensitive 24-bit DJB2 hash for the UNIT
; filename stem. NPC uses this compact set to distinguish project-local
; source units from independently published units found through UNITPATH.
project_publish_unit_hash:
      LDA   token_len
      SEC
      SBC   #4
      STA   project_requested_unit
      JSR   project_hash_token
      LDA   project_unit_count
      ASL
      CLC
      ADC   project_unit_count
      TAX
      LDA   project_hash+0
      STA   NPP_PLAN_BASE+NPP_PLAN_LOCAL_HASHES+0,X
      LDA   project_hash+1
      STA   NPP_PLAN_BASE+NPP_PLAN_LOCAL_HASHES+1,X
      LDA   project_hash+2
      STA   NPP_PLAN_BASE+NPP_PLAN_LOCAL_HASHES+2,X
      RTS

project_publish_overlay_hash:
      LDA   token_len
      STA   project_requested_unit
      JSR   project_hash_token
      LDA   NPP_PLAN_BASE+NPP_PLAN_OVERLAY_COUNT
      ASL
      CLC
      ADC   NPP_PLAN_BASE+NPP_PLAN_OVERLAY_COUNT
      TAX
      LDA   project_hash+0
      STA   NPP_PLAN_BASE+NPP_PLAN_OVERLAY_HASHES+0,X
      LDA   project_hash+1
      STA   NPP_PLAN_BASE+NPP_PLAN_OVERLAY_HASHES+1,X
      LDA   project_hash+2
      STA   NPP_PLAN_BASE+NPP_PLAN_OVERLAY_HASHES+2,X
      RTS

; DJB2 truncated to 24 bits, matching NPC's case-insensitive source-name hash.
; project_requested_unit is the number of token bytes to include.
project_hash_token:
      LDA   #$05
      STA   project_hash+0
      LDA   #$15
      STA   project_hash+1
      STZ   project_hash+2
      LDY   #0
@char:
      CPY   project_requested_unit
      BCS   @done
      LDA   token_buf,Y
      JSR   project_upper
      PHA
      LDX   #2
@save:
      LDA   project_hash,X
      STA   project_hash_saved,X
      DEX
      BPL   @save
      LDX   #5
@shift:
      ASL   project_hash+0
      ROL   project_hash+1
      ROL   project_hash+2
      DEX
      BNE   @shift
      CLC
      LDA   project_hash+0
      ADC   project_hash_saved+0
      STA   project_hash+0
      LDA   project_hash+1
      ADC   project_hash_saved+1
      STA   project_hash+1
      LDA   project_hash+2
      ADC   project_hash_saved+2
      STA   project_hash+2
      PLA
      CLC
      ADC   project_hash+0
      STA   project_hash+0
      BCC   @next
      INC   project_hash+1
      BNE   @next
      INC   project_hash+2
@next:
      INY
      BRA   @char
@done:
      RTS

; X selects one complete unit-filename slot; returns its pointer in project_ptr.
project_unit_pointer:
      TXA
      STZ   project_ptr+1
      ASL
      ROL   project_ptr+1
      ASL
      ROL   project_ptr+1
      STA   project_aux
      LDA   project_ptr+1
      STA   project_aux+1
      TXA
      STZ   project_ptr+1
      ASL
      ROL   project_ptr+1
      ASL
      ROL   project_ptr+1
      ASL
      ROL   project_ptr+1
      ASL
      ROL   project_ptr+1
      CLC
      ADC   project_aux
      STA   project_ptr
      LDA   project_ptr+1
      ADC   project_aux+1
      STA   project_ptr+1
      CLC
      LDA   project_ptr
      ADC   #<unit_names
      STA   project_ptr
      LDA   project_ptr+1
      ADC   #>unit_names
      STA   project_ptr+1
      RTS

project_require_one_identifier:
      LDA   NPTOOL_ARG1_LEN
      BNE   @bad
      LDA   #<NPTOOL_ARG0
      LDX   #>NPTOOL_ARG0
      LDY   NPTOOL_ARG0_LEN
      JMP   project_validate_identifier_named
@bad:
      SEC
      RTS

project_require_two_identifiers:
      LDA   #<NPTOOL_ARG0
      LDX   #>NPTOOL_ARG0
      LDY   NPTOOL_ARG0_LEN
      JSR   project_validate_identifier_named
      BCS   @bad
      LDA   #<NPTOOL_ARG1
      LDX   #>NPTOOL_ARG1
      LDY   NPTOOL_ARG1_LEN
      JMP   project_validate_identifier_named
@bad:
      RTS

project_validate_identifier_named:
      CPY   #1
      BCC   @bad
      CPY   #NPP_UNIT_NAME_CAP
      BCS   @bad
      STA   project_ptr
      STX   project_ptr+1
      STY   project_requested_unit
      LDY   #0
      LDA   (project_ptr),Y
      JSR   project_upper
      CMP   #'_'
      BEQ   @rest
      CMP   #'A'
      BCC   @bad
      CMP   #'Z'+1
      BCS   @bad
@rest:
      LDY   #1
@byte:
      CPY   project_requested_unit
      BCS   @ok
      LDA   (project_ptr),Y
      JSR   project_upper
      CMP   #'_'
      BEQ   @next
      CMP   #'A'
      BCC   @digit
      CMP   #'Z'+1
      BCC   @next
@digit:
      CMP   #'0'
      BCC   @bad
      CMP   #'9'+1
      BCS   @bad
@next:
      INY
      BRA   @byte
@ok:
      CLC
      RTS
@bad:
      SEC
      RTS

; Validate the identifier portion of token_buf (Y bytes).
project_validate_identifier_token:
      LDA   #<token_buf
      LDX   #>token_buf
      JMP   project_validate_identifier_named

project_reject_nova_unit:
      LDA   NPTOOL_ARG1_LEN
      CMP   #4
      BCC   @ok
      LDX   #0
@byte:
      LDA   NPTOOL_ARG1,X
      JSR   project_upper
      CMP   word_nova,X
      BNE   @ok
      INX
      CPX   #4
      BCC   @byte
      SEC
      RTS
@ok:
      CLC
      RTS

project_upper:
      CMP   #'a'
      BCC   @done
      CMP   #'z'+1
      BCS   @done
      AND   #$DF
@done:
      RTS

project_hex_nibble:
      JSR   project_upper
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

; ---------------------------------------------------------------------
; Project mutation helpers.
; ---------------------------------------------------------------------
project_copy_arg0_name:
      LDA   NPTOOL_ARG0_LEN
      STA   project_name_len
      TAX
      STZ   project_name,X
@copy:
      DEX
      BMI   @done
      LDA   NPTOOL_ARG0,X
      STA   project_name,X
      BRA   @copy
@done:
      RTS

project_make_manifest_name:
      LDA   #<project_name
      LDX   #>project_name
      LDY   project_name_len
      JSR   project_copy_upper_to_temp
      LDA   #<ext_npp
      LDX   #>ext_npp
      JMP   project_append_temp_z

project_make_arg1_pas_name:
      LDA   #<NPTOOL_ARG1
      LDX   #>NPTOOL_ARG1
      LDY   NPTOOL_ARG1_LEN
      JSR   project_copy_upper_to_temp
      LDA   #<ext_pas
      LDX   #>ext_pas
      JMP   project_append_temp_z

project_copy_upper_to_temp:
      STA   project_ptr
      STX   project_ptr+1
      STY   temp_name_len
      LDX   #0
@copy:
      CPX   temp_name_len
      BCS   @done
      TXA
      TAY
      LDA   (project_ptr),Y
      JSR   project_upper
      STA   temp_name,X
      INX
      BRA   @copy
@done:
      STZ   temp_name,X
      RTS

project_copy_named_to_temp:
      STA   project_ptr
      STX   project_ptr+1
      STY   temp_name_len
      LDX   #0
@copy:
      CPX   temp_name_len
      BCS   @done
      TXA
      TAY
      LDA   (project_ptr),Y
      STA   temp_name,X
      INX
      BRA   @copy
@done:
      STZ   temp_name,X
      RTS

; A/X is a Z extension. Replace temp_name's final extension.
project_replace_temp_extension:
      STA   project_ptr
      STX   project_ptr+1
      LDX   temp_name_len
@find:
      DEX
      BMI   @base
      LDA   temp_name,X
      CMP   #'.'
      BNE   @find
      STX   temp_name_len
@base:
      LDA   project_ptr
      LDX   project_ptr+1
      JMP   project_append_temp_z

project_append_temp_z:
      STA   project_ptr
      STX   project_ptr+1
      LDY   #0
@copy:
      LDA   (project_ptr),Y
      BEQ   @done
      LDX   temp_name_len
      CPX   #NPP_NAME_CAP-1
      BCS   @done
      STA   temp_name,X
      INC   temp_name_len
      INY
      BRA   @copy
@done:
      LDX   temp_name_len
      STZ   temp_name,X
      RTS

project_find_arg1_unit:
      LDX   #0
@unit:
      CPX   project_unit_count
      BCS   @missing
      STX   project_requested_unit
      JSR   project_unit_pointer
      LDX   project_requested_unit
      LDA   unit_lens,X
      SEC
      SBC   #4
      CMP   NPTOOL_ARG1_LEN
      BNE   @next
      TAY
      DEY
@compare:
      BMI   @found
      LDA   (project_ptr),Y
      JSR   project_upper
      STA   project_aux
      LDA   NPTOOL_ARG1,Y
      JSR   project_upper
      CMP   project_aux
      BNE   @next
      DEY
      BRA   @compare
@next:
      LDX   project_requested_unit
      INX
      BRA   @unit
@found:
      LDX   project_requested_unit
      SEC
      RTS
@missing:
      CLC
      RTS

project_insert_unit_line:
      LDA   NPTOOL_ARG1_LEN
      CLC
      ADC   #10                  ; "UNIT " + ".PAS" + LF
      STA   project_requested_unit
      LDA   project_file_len+1
      CMP   #>PROJECT_BUFFER_CAP
      BCC   @room
      BNE   @full
      LDA   project_file_len
      CLC
      ADC   project_requested_unit
      BCS   @full
@room:
      CLC
      LDA   project_file_len
      ADC   project_requested_unit
      STA   project_count
      LDA   project_file_len+1
      ADC   #0
      STA   project_count+1
      LDA   project_count+1
      CMP   #>PROJECT_BUFFER_CAP
      BCC   @shift_setup
      BNE   @full
      LDA   project_count
      BEQ   @shift_setup
@full:
      SEC
      RTS
@shift_setup:
      LDA   project_file_len
      STA   project_aux
      LDA   project_file_len+1
      STA   project_aux+1
@shift:
      LDA   project_aux
      BNE   :+
      DEC   project_aux+1
:     DEC   project_aux
      LDA   project_aux
      CMP   main_insert
      LDA   project_aux+1
      SBC   main_insert+1
      BCC   @write
      CLC
      LDA   #<project_buffer
      ADC   project_aux
      STA   project_ptr
      LDA   #>project_buffer
      ADC   project_aux+1
      STA   project_ptr+1
      LDY   #0
      LDA   (project_ptr),Y
      LDY   project_requested_unit
      STA   (project_ptr),Y
      BRA   @shift
@write:
      CLC
      LDA   #<project_buffer
      ADC   main_insert
      STA   project_ptr
      LDA   #>project_buffer
      ADC   main_insert+1
      STA   project_ptr+1
      LDY   #0
      LDX   #0
@prefix:
      LDA   unit_line_prefix,X
      BEQ   @name
      STA   (project_ptr),Y
      INX
      INY
      BRA   @prefix
@name:
      LDX   #0
@name_byte:
      CPX   NPTOOL_ARG1_LEN
      BCS   @suffix
      LDA   NPTOOL_ARG1,X
      JSR   project_upper
      STA   (project_ptr),Y
      INX
      INY
      BRA   @name_byte
@suffix:
      LDX   #0
@suffix_byte:
      LDA   unit_line_suffix,X
      STA   (project_ptr),Y
      INX
      INY
      CPX   #5
      BCC   @suffix_byte
      LDA   project_count
      STA   project_file_len
      LDA   project_count+1
      STA   project_file_len+1
      CLC
      RTS

project_remove_unit_line:
      LDX   project_requested_unit
      SEC
      LDA   unit_end_lo,X
      SBC   unit_start_lo,X
      STA   token_len
      LDA   unit_end_hi,X
      SBC   unit_start_hi,X
      BNE   @bad
      LDA   unit_start_lo,X
      STA   project_count
      LDA   unit_start_hi,X
      STA   project_count+1
      LDA   unit_end_lo,X
      STA   project_aux
      LDA   unit_end_hi,X
      STA   project_aux+1
@move:
      LDA   project_aux+1
      CMP   project_file_len+1
      BCC   @copy
      BNE   @done
      LDA   project_aux
      CMP   project_file_len
      BCS   @done
@copy:
      CLC
      LDA   #<project_buffer
      ADC   project_aux
      STA   project_ptr
      LDA   #>project_buffer
      ADC   project_aux+1
      STA   project_ptr+1
      LDY   #0
      LDA   (project_ptr),Y
      PHA
      CLC
      LDA   #<project_buffer
      ADC   project_count
      STA   project_ptr
      LDA   #>project_buffer
      ADC   project_count+1
      STA   project_ptr+1
      PLA
      STA   (project_ptr),Y
      INC   project_aux
      BNE   :+
      INC   project_aux+1
:     INC   project_count
      BNE   @move
      INC   project_count+1
      BRA   @move
@done:
      SEC
      LDA   project_file_len
      SBC   token_len
      STA   project_file_len
      LDA   project_file_len+1
      SBC   #0
      STA   project_file_len+1
      CLC
      RTS
@bad:
      SEC
      RTS

; ---------------------------------------------------------------------
; File and directory helpers, exclusively through the FILES NDK module.
; ---------------------------------------------------------------------
project_clear_lib_args:
      LDX   #15
@clear:
      STZ   LIB_ARG0,X
      DEX
      BPL   @clear
      RTS

project_call:
      STA   LIB_FN_ID
      LDA   #MODULE_ID_FILES
      STA   LIB_MOD_ID
      JSR   LIB_LOADER_BAND
      LDA   LIB_STATUS
      RTS

; A/X name, Y length, project_requested_unit already contains FILE_* fn.
project_call_named:
      STA   project_ptr
      STX   project_ptr+1
      STY   token_len
      JSR   project_clear_lib_args
      LDA   project_ptr
      STA   LIB_ARG0
      LDA   project_ptr+1
      STA   LIB_ARG0+1
      LDA   token_len
      STA   LIB_ARG1
      LDA   project_requested_unit
      JMP   project_call

project_mkdir_named:
      PHA
      LDA   #FILE_MKDIR
      STA   project_requested_unit
      PLA
      JSR   project_call_named
      BEQ   @ok
      SEC
      RTS
@ok:
      CLC
      RTS

project_rmdir_named:
      PHA
      LDA   #FILE_RMDIR
      STA   project_requested_unit
      PLA
      JSR   project_call_named
      BEQ   @ok
      SEC
      RTS
@ok:
      CLC
      RTS

project_cd_named:
      PHA
      LDA   #FILE_CD
      STA   project_requested_unit
      PLA
      JSR   project_call_named
      BEQ   @ok
      SEC
      RTS
@ok:
      CLC
      RTS

project_delete_named:
      PHA
      LDA   #FILE_FDELETE
      STA   project_requested_unit
      PLA
      JSR   project_call_named
      BEQ   @ok
      SEC
      RTS
@ok:
      CLC
      RTS

project_delete_named_optional:
      JSR   project_delete_named
      BCC   @ok
      LDA   FIO_ERRCODE
      CMP   #FIO_ERR_NOTFOUND
      BNE   @bad
@ok:
      CLC
      RTS
@bad:
      SEC
      RTS

project_save_cwd:
      JSR   project_clear_lib_args
      LDA   #FILE_PWD
      JSR   project_call
      BNE   @bad
      LDA   FIO_NAMELEN
      CMP   #NPP_NAME_CAP
      BCS   @bad
      STA   cwd_name_len
      TAX
      STZ   cwd_name,X
@copy:
      DEX
      BMI   @ok
      LDA   FIO_NAME,X
      STA   cwd_name,X
      BRA   @copy
@ok:
      CLC
      RTS
@bad:
      SEC
      RTS

project_restore_cwd:
      LDA   project_dir_entered
      BEQ   @ok
      LDA   #<cwd_name
      LDX   #>cwd_name
      LDY   cwd_name_len
      JSR   project_cd_named
      BCS   @bad
      STZ   project_dir_entered
@ok:
      CLC
      RTS
@bad:
      SEC
      RTS

project_cd_project:
      LDA   cwd_name_len
      CLC
      ADC   project_name_len
      ADC   #1
      CMP   #NPP_NAME_CAP
      BCS   @bad
      LDX   #0
@cwd:
      CPX   cwd_name_len
      BCS   @separator
      LDA   cwd_name,X
      STA   temp_name,X
      INX
      BRA   @cwd
@separator:
      CPX   #0
      BEQ   @slash
      LDA   temp_name-1,X
      CMP   #'/'
      BEQ   @project
@slash:
      LDA   #'/'
      STA   temp_name,X
      INX
@project:
      LDY   #0
@name:
      CPY   project_name_len
      BCS   @done
      LDA   project_name,Y
      STA   temp_name,X
      INX
      INY
      BRA   @name
@done:
      STX   temp_name_len
      STZ   temp_name,X
      LDA   #<temp_name
      LDX   #>temp_name
      LDY   temp_name_len
      JMP   project_cd_named
@bad:
      SEC
      RTS

project_enter_named_project:
      JSR   project_save_cwd
      BCS   @bad
      JSR   project_cd_project
      BCS   @bad
      INC   project_dir_entered
      CLC
      RTS
@bad:
      SEC
      RTS

project_require_absent:
      STA   project_ptr
      STX   project_ptr+1
      STY   token_len
      JSR   project_clear_lib_args
      LDA   project_ptr
      STA   LIB_ARG0
      LDA   project_ptr+1
      STA   LIB_ARG0+1
      LDA   token_len
      STA   LIB_ARG1
      LDA   #FIO_FILE_ACCESS_READ
      STA   LIB_ARG2
      LDA   #FILE_FOPEN
      JSR   project_call
      BNE   @open_error
      LDA   LIB_RESULT
      STA   project_read_id
      LDA   LIB_RESULT+1
      STA   project_read_id+1
      JSR   project_close_read
      LDA   #NPP_DETAIL_EXISTS
      STA   project_error_detail
      SEC
      RTS
@open_error:
      LDA   FIO_ERRCODE
      CMP   #FIO_ERR_NOTFOUND
      BEQ   @absent
      SEC
      RTS
@absent:
      CLC
      RTS

project_save_buffer:
      STA   project_ptr
      STX   project_ptr+1
      STY   token_len
      LDA   #<project_buffer
      STA   NPTOOL_IO_ADDR
      LDA   #>project_buffer
      STA   NPTOOL_IO_ADDR+1
      LDA   project_file_len
      STA   NPTOOL_IO_LEN
      LDA   project_file_len+1
      STA   NPTOOL_IO_LEN+1
      LDA   project_ptr
      LDX   project_ptr+1
      LDY   token_len
      JSR   nptool_save_named
      BEQ   @ok
      SEC
      RTS
@ok:
      CLC
      RTS

project_save_manifest_buffer:
      LDA   #<manifest_name
      LDX   #>manifest_name
      LDY   manifest_len
      JMP   project_save_buffer

project_close_read:
      JSR   project_clear_lib_args
      LDA   project_read_id
      STA   LIB_ARG0
      LDA   project_read_id+1
      STA   LIB_ARG0+1
      LDA   #FILE_FCLOSE
      JMP   project_call

project_close_write:
      JSR   project_clear_lib_args
      LDA   project_write_id
      STA   LIB_ARG0
      LDA   project_write_id+1
      STA   LIB_ARG0+1
      LDA   #FILE_FCLOSE
      JSR   project_call
      BEQ   @ok
      SEC
      RTS
@ok:
      CLC
      RTS

project_open_combined_output:
      JSR   project_clear_lib_args
      LDA   #<NPTOOL_ARG1
      STA   LIB_ARG0
      LDA   #>NPTOOL_ARG1
      STA   LIB_ARG0+1
      LDA   NPTOOL_ARG1_LEN
      STA   LIB_ARG1
      LDA   #FIO_FILE_ACCESS_WRITE
      STA   LIB_ARG2
      LDA   #FILE_FCREATE
      JSR   project_call
      BNE   @bad
      LDA   LIB_RESULT
      STA   project_write_id
      LDA   LIB_RESULT+1
      STA   project_write_id+1
      CLC
      RTS
@bad:
      SEC
      RTS

project_write_entry_jump:
      JSR   project_clear_lib_args
      LDA   project_write_id
      STA   LIB_ARG0
      LDA   project_write_id+1
      STA   LIB_ARG0+1
      LDA   #<project_entry_jump
      STA   LIB_ARG1
      LDA   #>project_entry_jump
      STA   LIB_ARG1+1
      LDA   #project_entry_jump_end-project_entry_jump
      STA   LIB_ARG2
      LDA   #FILE_FWRITE
      JSR   project_call
      BNE   @bad
      LDA   LIB_RESULT+1
      BNE   @bad
      LDA   LIB_RESULT
      CMP   #project_entry_jump_end-project_entry_jump
      BNE   @bad
      CLC
      RTS
@bad:
      SEC
      RTS

project_append_temp_file:
      JSR   project_clear_lib_args
      LDA   #<temp_name
      STA   LIB_ARG0
      LDA   #>temp_name
      STA   LIB_ARG0+1
      LDA   temp_name_len
      STA   LIB_ARG1
      LDA   #FIO_FILE_ACCESS_READ
      STA   LIB_ARG2
      LDA   #FILE_FOPEN
      JSR   project_call
      long_bne @bad
      LDA   LIB_RESULT
      STA   project_read_id
      LDA   LIB_RESULT+1
      STA   project_read_id+1
@read:
      JSR   project_clear_lib_args
      LDA   project_read_id
      STA   LIB_ARG0
      LDA   project_read_id+1
      STA   LIB_ARG0+1
      LDA   #<project_buffer
      STA   LIB_ARG1
      LDA   #>project_buffer
      STA   LIB_ARG1+1
      LDA   #<PROJECT_BUFFER_CAP
      STA   LIB_ARG2
      LDA   #>PROJECT_BUFFER_CAP
      STA   LIB_ARG2+1
      LDA   #FILE_FREAD
      JSR   project_call
      BNE   @close_bad
      LDA   LIB_RESULT
      STA   project_count
      LDA   LIB_RESULT+1
      STA   project_count+1
      ORA   project_count
      BEQ   @done
      JSR   project_clear_lib_args
      LDA   project_write_id
      STA   LIB_ARG0
      LDA   project_write_id+1
      STA   LIB_ARG0+1
      LDA   #<project_buffer
      STA   LIB_ARG1
      LDA   #>project_buffer
      STA   LIB_ARG1+1
      LDA   project_count
      STA   LIB_ARG2
      LDA   project_count+1
      STA   LIB_ARG2+1
      LDA   #FILE_FWRITE
      JSR   project_call
      BNE   @close_bad
      LDA   LIB_RESULT
      CMP   project_count
      BNE   @close_bad
      LDA   LIB_RESULT+1
      CMP   project_count+1
      BNE   @close_bad
      BRA   @read
@done:
      JSR   project_close_read
      BEQ   @ok
@bad:
      SEC
      RTS
@close_bad:
      JSR   project_close_read
      SEC
      RTS
@ok:
      CLC
      RTS

project_reject_nested_directories:
      JSR   project_dir_open_all
      BCS   @bad
@read:
      JSR   project_dir_read
      BCS   @end
      LDA   FIO_DIRTYPE
      CMP   #FIO_TYPE_DIR
      BNE   @read
      LDA   #NPP_DETAIL_NESTED_DIR
      STA   project_error_detail
      SEC
      RTS
@end:
      LDA   FIO_ERRCODE
      CMP   #FIO_ERR_EOD
      BNE   @bad
      CLC
      RTS
@bad:
      SEC
      RTS

project_delete_directory_files:
      JSR   project_dir_open_all
      BCS   @bad
@read:
      JSR   project_dir_read
      BCS   @end
      LDA   FIO_DIRTYPE
      CMP   #FIO_TYPE_DIR
      BEQ   @bad
      LDA   FIO_NAMELEN
      STA   temp_name_len
      TAX
      STZ   temp_name,X
@copy:
      DEX
      BMI   @delete
      LDA   FIO_NAME,X
      STA   temp_name,X
      BRA   @copy
@delete:
      LDA   #<temp_name
      LDX   #>temp_name
      LDY   temp_name_len
      JSR   project_delete_named
      BCS   @bad
      BRA   @read
@end:
      LDA   FIO_ERRCODE
      CMP   #FIO_ERR_EOD
      BNE   @bad
      CLC
      RTS
@bad:
      SEC
      RTS

project_dir_open_all:
      JSR   project_clear_lib_args
      LDA   #FIO_DIR_FLAG_FULLNAME
      STA   FIO_DIRTYPE
      LDA   #FILE_DIR_OPEN
      JSR   project_call
      BEQ   @ok
      SEC
      RTS
@ok:
      CLC
      RTS

project_dir_read:
      JSR   project_clear_lib_args
      LDA   #FILE_DIR_READ
      JSR   project_call
      BEQ   @ok
      SEC
      RTS
@ok:
      CLC
      RTS

project_delete_outputs:
      JSR   project_make_manifest_name
      LDA   #<ext_s
      LDX   #>ext_s
      JSR   project_replace_temp_extension
      JSR   project_delete_temp_optional
      LDA   #<ext_obj
      LDX   #>ext_obj
      JSR   project_replace_temp_extension
      JSR   project_delete_temp_optional
      LDA   NPP_PLAN_BASE+NPP_PLAN_OUTPUT_LEN
      BEQ   @map
      LDA   #<(NPP_PLAN_BASE+NPP_PLAN_OUTPUT)
      LDX   #>(NPP_PLAN_BASE+NPP_PLAN_OUTPUT)
      LDY   NPP_PLAN_BASE+NPP_PLAN_OUTPUT_LEN
      JSR   project_delete_named_optional
      LDA   NPP_PLAN_BASE+NPP_PLAN_TARGET
      CMP   #NPP_TARGET_UNIT
      BNE   @map
      LDA   #<(NPP_PLAN_BASE+NPP_PLAN_OUTPUT)
      LDX   #>(NPP_PLAN_BASE+NPP_PLAN_OUTPUT)
      LDY   NPP_PLAN_BASE+NPP_PLAN_OUTPUT_LEN
      JSR   project_copy_named_to_temp
      LDA   #<ext_s
      LDX   #>ext_s
      JSR   project_replace_temp_extension
      JSR   project_delete_temp_optional
      LDA   #<ext_asm
      LDX   #>ext_asm
      JSR   project_replace_temp_extension
      JSR   project_delete_temp_optional
@map:
      LDA   NPP_PLAN_BASE+NPP_PLAN_MAP_LEN
      BEQ   @label
      LDA   #<(NPP_PLAN_BASE+NPP_PLAN_MAP)
      LDX   #>(NPP_PLAN_BASE+NPP_PLAN_MAP)
      LDY   NPP_PLAN_BASE+NPP_PLAN_MAP_LEN
      JSR   project_delete_named_optional
@label:
      LDA   NPP_PLAN_BASE+NPP_PLAN_LABEL_LEN
      BEQ   @done
      LDA   #<(NPP_PLAN_BASE+NPP_PLAN_LABEL)
      LDX   #>(NPP_PLAN_BASE+NPP_PLAN_LABEL)
      LDY   NPP_PLAN_BASE+NPP_PLAN_LABEL_LEN
      JSR   project_delete_named_optional
@done:
      RTS

project_delete_temp_optional:
      LDA   #<temp_name
      LDX   #>temp_name
      LDY   temp_name_len
      JMP   project_delete_named_optional

project_cleanup_new:
      LDA   project_dir_entered
      BEQ   @remove_dir
      LDA   project_manifest_created
      BEQ   @source
      JSR   project_make_manifest_name
      JSR   project_delete_temp_optional
@source:
      LDA   project_source_created
      BEQ   @restore
      LDA   #<main_file_name
      LDX   #>main_file_name
      LDY   #main_file_name_end-main_file_name
      JSR   project_delete_named_optional
@restore:
      JSR   project_restore_cwd
@remove_dir:
      LDA   project_created
      BEQ   @done
      LDA   #<project_name
      LDX   #>project_name
      LDY   project_name_len
      JSR   project_rmdir_named
@done:
      RTS

; ---------------------------------------------------------------------
; Text generation.
; ---------------------------------------------------------------------
project_buffer_reset:
      STZ   project_file_len
      STZ   project_file_len+1
      RTS

project_buffer_append_arg0:
      LDA   #<NPTOOL_ARG0
      LDX   #>NPTOOL_ARG0
      LDY   NPTOOL_ARG0_LEN
      BRA   project_buffer_append

project_buffer_append_arg1_upper:
      LDA   #<NPTOOL_ARG1
      STA   project_ptr
      LDA   #>NPTOOL_ARG1
      STA   project_ptr+1
      LDY   #0
@byte:
      CPY   NPTOOL_ARG1_LEN
      BCS   @ok
      LDA   (project_ptr),Y
      JSR   project_upper
      PHY
      JSR   project_buffer_append_byte
      PLY
      BCS   @bad
      INY
      BRA   @byte
@ok:
      CLC
@bad:
      RTS

project_buffer_append_project_upper:
      LDA   #<project_name
      STA   project_ptr
      LDA   #>project_name
      STA   project_ptr+1
      LDY   #0
@byte:
      CPY   project_name_len
      BCS   @ok
      LDA   (project_ptr),Y
      JSR   project_upper
      PHY
      JSR   project_buffer_append_byte
      PLY
      BCS   @bad
      INY
      BRA   @byte
@ok:
      CLC
@bad:
      RTS

project_buffer_append_z:
      STA   project_ptr
      STX   project_ptr+1
      LDY   #0
@byte:
      LDA   (project_ptr),Y
      BEQ   @ok
      PHY
      JSR   project_buffer_append_byte
      PLY
      BCS   @bad
      INY
      BNE   @byte
@bad:
      SEC
      RTS
@ok:
      CLC
      RTS

project_buffer_append:
      STA   project_ptr
      STX   project_ptr+1
      STY   token_len
      LDY   #0
@byte:
      CPY   token_len
      BCS   @ok
      LDA   (project_ptr),Y
      PHY
      JSR   project_buffer_append_byte
      PLY
      BCS   @bad
      INY
      BRA   @byte
@ok:
      CLC
@bad:
      RTS

project_buffer_append_byte:
      PHA
      LDA   project_file_len+1
      CMP   #>PROJECT_BUFFER_CAP
      BCS   @full
      CLC
      LDA   #<project_buffer
      ADC   project_file_len
      STA   project_aux
      LDA   #>project_buffer
      ADC   project_file_len+1
      STA   project_aux+1
      PLA
      LDY   #0
      STA   (project_aux),Y
      INC   project_file_len
      BNE   :+
      INC   project_file_len+1
:     CLC
      RTS
@full:
      PLA
      SEC
      RTS

project_build_main_source:
      JSR   project_buffer_reset
      LDA   project_new_unit
      BNE   @unit
      LDA   #<new_program_prefix
      LDX   #>new_program_prefix
      JSR   project_buffer_append_z
      JSR   project_buffer_append_arg0
      LDA   #<new_program_suffix
      LDX   #>new_program_suffix
      JMP   project_buffer_append_z
@unit:
      LDA   #<new_library_prefix
      LDX   #>new_library_prefix
      JSR   project_buffer_append_z
      JSR   project_buffer_append_project_upper
      LDA   #<new_library_suffix
      LDX   #>new_library_suffix
      JMP   project_buffer_append_z

project_build_unit_source:
      JSR   project_buffer_reset
      LDA   #<new_unit_prefix
      LDX   #>new_unit_prefix
      JSR   project_buffer_append_z
      JSR   project_buffer_append_arg1_upper
      LDA   #<new_unit_suffix
      LDX   #>new_unit_suffix
      JMP   project_buffer_append_z

project_build_manifest:
      JSR   project_buffer_reset
      LDA   project_new_unit
      BNE   @unit
      LDA   #<new_manifest_prefix
      LDX   #>new_manifest_prefix
      JSR   project_buffer_append_z
      JSR   project_buffer_append_project_upper
      LDA   #<new_manifest_output
      LDX   #>new_manifest_output
      JSR   project_buffer_append_z
      JSR   project_buffer_append_project_upper
      LDA   #<new_manifest_bin
      LDX   #>new_manifest_bin
      JSR   project_buffer_append_z
      JSR   project_buffer_append_project_upper
      LDA   #<new_manifest_map
      LDX   #>new_manifest_map
      JMP   project_buffer_append_z
@unit:
      LDA   #<new_unit_manifest_prefix
      LDX   #>new_unit_manifest_prefix
      JSR   project_buffer_append_z
      JSR   project_buffer_append_project_upper
      LDA   #<new_unit_manifest_suffix
      LDX   #>new_unit_manifest_suffix
      JMP   project_buffer_append_z

; ---------------------------------------------------------------------
; Messages and errors.
; ---------------------------------------------------------------------
project_print_arg0:
      PHA
      PHX
      JSR   nptool_print_z
      LDA   #<NPTOOL_ARG0
      LDX   #>NPTOOL_ARG0
      JSR   nptool_print_z
      JSR   nptool_newline
      PLX
      PLA
      RTS

project_print_arg1:
      JSR   nptool_print_z
      LDA   #<NPTOOL_ARG1
      LDX   #>NPTOOL_ARG1
      JSR   nptool_print_z
      JMP   nptool_newline

project_bad_args:
      LDA   #NPTOOL_ERR_ARGS
      STA   NPTOOL_STATUS
      LDA   #1
      RTS

project_project_fail:
      LDA   project_error_detail
      BNE   :+
      LDA   #NPP_DETAIL_INVALID
:     STA   NPTOOL_DETAIL
      LDA   #NPTOOL_ERR_PROJECT
      STA   NPTOOL_STATUS
      LDA   #1
      RTS

project_io_fail:
      LDA   FIO_ERRCODE
      STA   NPTOOL_DETAIL
      LDA   #NPTOOL_ERR_IO
      STA   NPTOOL_STATUS
      LDA   #1
      RTS

project_fail:
      LDA   NPTOOL_STATUS
      BNE   @return
      JMP   project_project_fail
@return:
      LDA   #1
      RTS

      .segment "RODATA"
word_npp:      .byte "NPP", 0
word_main:     .byte "MAIN", 0
word_unit:     .byte "UNIT", 0
word_unitpath: .byte "UNITPATH", 0
word_overlay:  .byte "OVERLAY", 0
word_target:   .byte "TARGET", 0
word_output:   .byte "OUTPUT", 0
word_optimize: .byte "OPTIMIZE", 0
word_define:   .byte "DEFINE", 0
word_object:   .byte "OBJECT", 0
word_asm:      .byte "ASM", 0
word_load:     .byte "LOAD", 0
word_config:   .byte "CONFIG", 0
word_map:      .byte "MAP", 0
word_label:    .byte "LABEL", 0
word_memory:   .byte "MEMORY", 0
word_inline:   .byte "INLINE", 0
word_o2:       .byte "O2", 0
word_program:  .byte "PROGRAM", 0
word_unit_target: .byte "UNIT", 0
word_nova:     .byte "NOVA"

ext_pas: .byte ".PAS", 0
ext_npp: .byte ".NPP", 0
ext_s:   .byte ".S", 0
ext_asm: .byte ".ASM", 0
ext_npi: .byte ".NPI", 0
ext_obj: .byte ".OBJ", 0

main_file_name: .byte "MAIN.PAS"
main_file_name_end:
unit_line_prefix: .byte "UNIT ", 0
unit_line_suffix: .byte ".PAS", $0A

new_program_prefix: .byte "program ", 0
new_program_suffix:
      .byte ";", $0A
      .byte "begin", $0A
      .byte "  writeln('Hello, world!');", $0A
      .byte "end.", $0A, 0
new_library_prefix: .byte "unit ", 0
new_library_suffix:
      .byte ";", $0A, $0A
      .byte "interface", $0A
      .byte "procedure Hello;", $0A, $0A
      .byte "implementation", $0A
      .byte "procedure Hello;", $0A
      .byte "begin", $0A
      .byte "  writeln('Hello from this unit');", $0A
      .byte "end;", $0A, $0A
      .byte "end.", $0A, 0
new_unit_prefix: .byte "unit ", 0
new_unit_suffix:
      .byte ";", $0A, $0A
      .byte "interface", $0A, $0A
      .byte "implementation", $0A, $0A
      .byte "end.", $0A, 0
new_manifest_prefix:
      .byte "NPP 2", $0A, "MAIN MAIN.PAS", $0A
      .byte "UNITPATH SYSTEM", $0A, "UNITPATH USER", $0A
      .byte "OUTPUT ", 0
new_manifest_output:
      .byte ".BIN", $0A
      .byte "OPTIMIZE O2", $0A
      .byte "DEFINE NOVA=1", $0A
      .byte "CONFIG INLINE", $0A
      .byte "MAP ", 0
new_manifest_bin: .byte ".MAP", $0A, "LABEL ", 0
new_manifest_map:
      .byte ".LBL", $0A
      .byte "MEMORY {", $0A
      .byte "    RAM: start = $0900, size = $9700, file = %O;", $0A
      .byte "}", $0A, $0A
      .byte "SEGMENTS {", $0A
      .byte "    CODE: load = RAM, type = ro;", $0A
      .byte "    RODATA: load = RAM, type = ro;", $0A
      .byte "    BSS: load = RAM, type = bss;", $0A
      .byte "}", $0A, 0
new_unit_manifest_prefix:
      .byte "NPP 2", $0A, "MAIN MAIN.PAS", $0A
      .byte "TARGET UNIT", $0A
      .byte "UNITPATH SYSTEM", $0A, "UNITPATH USER", $0A
      .byte "OUTPUT /USER/", 0
new_unit_manifest_suffix:
      .byte ".OBJ", $0A
      .byte "OPTIMIZE O2", $0A
      .byte "DEFINE NOVA=1", $0A, 0
project_entry_jump: .byte "JMP __NP_MAIN", $0A
project_entry_jump_end:
message_created:         .byte "Created project ", 0
message_added:           .byte "Added unit ", 0
message_deleted_unit:    .byte "Deleted unit ", 0
message_deleted_project: .byte "Deleted project ", 0
message_combining:       .byte "Combining ", 0

; NL executable frontend: ordered NOBJ inputs + one NLIB -> Nova binary.

      .setcpu "w65c02"
      .include "novalink.inc"
      .include "nptool.inc"
      .include "longbranch.inc"
      .include "libsystem.inc"

LIBRARY_CAP = 4096
OUTPUT_CAP  = $1800
CONFIG_CAP  = 512

config_ptr  = nptool_io_ptr
config_word = nptool_io_aux
config_left = nptool_io_left

      .segment "BSS"
config_value:    .res 2
config_start:    .res 2
config_size:     .res 2
config_digits:   .res 1
config_expected: .res 1
config_name_len: .res 1
config_name:     .res NLINK_NAME_CAP
config_rule_index:.res 1
config_match_index:.res 1
config_min:      .res 2
config_max:      .res 2
worker_loaded:   .res 1

      .segment "NOINIT"
library_buf:.res LIBRARY_CAP
output_buf: .res OUTPUT_CAP
config_buf: .res CONFIG_CAP

      .segment "CODE"
      .export tool_main

tool_main:
      JSR   nptool_clear_bss
      STZ   NPTOOL_STATUS
      STZ   NPTOOL_DETAIL
      LDA   #<nl_banner
      LDX   #>nl_banner
      JSR   nptool_print_z
      JSR   tool_default_options
      LDA   NPTOOL_ARG4_LEN
      BEQ   @objects
      LDA   #<nl_config
      LDX   #>nl_config
      JSR   nptool_print_z
      LDA   #<NPTOOL_ARG4
      LDX   #>NPTOOL_ARG4
      JSR   nptool_print_z
      JSR   nptool_newline
      LDA   #<config_buf
      STA   NPTOOL_IO_ADDR+0
      LDA   #>config_buf
      STA   NPTOOL_IO_ADDR+1
      LDA   #<CONFIG_CAP
      STA   NPTOOL_IO_CAP+0
      LDA   #>CONFIG_CAP
      STA   NPTOOL_IO_CAP+1
      JSR   nptool_load_arg4
      long_bne @fail
      JSR   nptool_validate_text
      long_bne @config_error
      JSR   config_parse
      long_bne @config_error

@objects:
      LDA   NPTOOL_FLAGS
      AND   #NPTOOL_FLAG_VALIDATE
      BEQ   :+
      LDA   #<nl_config_valid
      LDX   #>nl_config_valid
      JSR   nptool_print_z
      LDA   #0
      RTS
:
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
      JSR   tool_preserve_label
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
      LDA   #<NLW_OBJECT0
      STA   NPTOOL_IO_ADDR+0
      LDA   #>NLW_OBJECT0
      STA   NPTOOL_IO_ADDR+1
      LDA   #<NLW_OBJECT0_CAP
      STA   NPTOOL_IO_CAP+0
      LDA   #>NLW_OBJECT0_CAP
      STA   NPTOOL_IO_CAP+1
      JSR   nptool_load_arg0
      BEQ   :+
      JMP   @fail
:
      LDA   NPTOOL_IO_LEN+0
      STA   nlink_object_len_l+0
      LDA   NPTOOL_IO_LEN+1
      STA   nlink_object_len_h+0
      LDA   #<NLW_OBJECT0
      STA   nlink_object_ptr_l+0
      LDA   #>NLW_OBJECT0
      STA   nlink_object_ptr_h+0
      LDA   #1
      STA   nlink_object_count

      LDA   NPTOOL_ARG3_LEN
      BEQ   @library
      LDA   #<nl_linking
      LDX   #>nl_linking
      JSR   nptool_print_z
      LDA   #<NPTOOL_ARG3
      LDX   #>NPTOOL_ARG3
      JSR   nptool_print_z
      JSR   nptool_newline
      LDA   #<NLW_OBJECT1
      STA   NPTOOL_IO_ADDR+0
      LDA   #>NLW_OBJECT1
      STA   NPTOOL_IO_ADDR+1
      LDA   #<NLW_OBJECT1_CAP
      STA   NPTOOL_IO_CAP+0
      LDA   #>NLW_OBJECT1_CAP
      STA   NPTOOL_IO_CAP+1
      JSR   nptool_load_arg3
      BEQ   :+
      JMP   @fail
:
      LDA   NPTOOL_IO_LEN+0
      STA   nlink_object_len_l+1
      LDA   NPTOOL_IO_LEN+1
      STA   nlink_object_len_h+1
      LDA   #<NLW_OBJECT1
      STA   nlink_object_ptr_l+1
      LDA   #>NLW_OBJECT1
      STA   nlink_object_ptr_h+1
      INC   nlink_object_count

@library:
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

      LDA   #<library_buf
      STA   nlink_library_ptr+0
      LDA   #>library_buf
      STA   nlink_library_ptr+1
      LDA   #<output_buf
      STA   nlink_output_ptr+0
      LDA   #>output_buf
      STA   nlink_output_ptr+1
      LDA   nlink_object_count
      STA   NLW_ROOT_COUNT
      JSR   nlink_prepare
      long_bne @link_error
      JSR   nlink_export_worker_state
      LDA   #<nl_stripping
      LDX   #>nl_stripping
      JSR   nptool_print_z
      LDA   #NLW_COMMAND_GC
      STA   NLW_COMMAND
      JSR   tool_run_worker
      long_bne @gc_error
      JSR   nlink_import_worker_objects
      long_bne @gc_error
      JSR   nlink_link
      long_bne @link_error

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
      LDA   NPTOOL_ARG5_LEN
      BEQ   @labels
      LDA   #NLW_COMMAND_MAP
      JSR   tool_write_report
      BNE   @report_error
@labels:
      LDA   NPTOOL_ARG6_LEN
      BEQ   @success
      LDA   #NLW_COMMAND_LABEL
      JSR   tool_write_report
      BNE   @report_error
@success:
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

@report_error:
      LDA   #NLINK_ERR_OUTPUT
      STA   NPTOOL_DETAIL
      LDA   #NPTOOL_ERR_LINK
      STA   NPTOOL_STATUS
      LDA   #1
      RTS

@gc_error:
      LDA   #NLINK_ERR_OBJECT
      STA   NPTOOL_DETAIL
      LDA   #NPTOOL_ERR_LINK
      STA   NPTOOL_STATUS
      LDA   #1
      RTS

@config_error:
      LDA   #NLINK_ERR_CONFIG
      STA   NPTOOL_DETAIL
      LDA   #NPTOOL_ERR_LINK
      STA   NPTOOL_STATUS
      LDA   #1
      RTS

tool_bad_args:
      LDA   #NPTOOL_ERR_ARGS
      STA   NPTOOL_STATUS
      LDA   #1
      RTS

tool_default_options:
      STZ   NLW_GC_READY
      LDX   #NLINK_OBJECT_CAP-1
@mask:
      STZ   NLW_LIVE_MASK,X
      DEX
      BPL   @mask
      STZ   nlink_config_enabled
      STZ   nlink_region_count
      STZ   nlink_rule_count
      STZ   nlink_config_symbol_count
      LDA   #<OUTPUT_CAP
      STA   nlink_output_cap+0
      LDA   #>OUTPUT_CAP
      STA   nlink_output_cap+1
      LDA   NPTOOL_LINK_BASE
      STA   nlink_load_base+0
      LDA   NPTOOL_LINK_BASE+1
      STA   nlink_load_base+1
      RTS

; NLWORK owns $0900-$0A87, including ARG6. Preserve the label filename in the
; configuration buffer after parsing has finished and before loading the worker.
tool_preserve_label:
      LDA   NPTOOL_ARG6_LEN
      BEQ   @done
      LDY   #0
@copy:
      LDA   NPTOOL_ARG6,Y
      STA   config_buf,Y
      BEQ   @done
      INY
      BRA   @copy
@done:
      RTS

; A selects map or VICE-label output; both reuse the worker's live-export walk.
tool_write_report:
      PHA
      JSR   nlink_export_worker_state
      LDA   #<output_buf
      STA   NLW_MAP_PTR
      LDA   #>output_buf
      STA   NLW_MAP_PTR+1
      LDA   #<OUTPUT_CAP
      STA   NLW_MAP_CAP
      LDA   #>OUTPUT_CAP
      STA   NLW_MAP_CAP+1
      PLA
      STA   NLW_COMMAND
      JSR   tool_run_worker
      BNE   @done
      LDA   #<output_buf
      STA   NPTOOL_IO_ADDR+0
      LDA   #>output_buf
      STA   NPTOOL_IO_ADDR+1
      LDA   NLW_MAP_LEN
      STA   NPTOOL_IO_LEN+0
      LDA   NLW_MAP_LEN+1
      STA   NPTOOL_IO_LEN+1
      LDA   NLW_COMMAND
      CMP   #NLW_COMMAND_MAP
      BNE   @labels
      LDA   #<nl_map
      LDX   #>nl_map
      JSR   nptool_print_z
      LDA   #<NPTOOL_ARG5
      LDX   #>NPTOOL_ARG5
      JSR   nptool_print_z
      JSR   nptool_newline
      LDA   #<NPTOOL_ARG5
      LDX   #>NPTOOL_ARG5
      LDY   NPTOOL_ARG5_LEN
      JMP   nptool_save_named
@labels:
      LDA   #<nl_labels
      LDX   #>nl_labels
      JSR   nptool_print_z
      LDA   #<config_buf
      LDX   #>config_buf
      JSR   nptool_print_z
      JSR   nptool_newline
      LDA   #<config_buf
      LDX   #>config_buf
      LDY   NPTOOL_ARG6_LEN
      JMP   nptool_save_named
@done:
      RTS

tool_run_worker:
      JSR   tool_clear_args
      LDA   #<nl_worker_name
      STA   LIB_ARG0
      LDA   #>nl_worker_name
      STA   LIB_ARG0+1
      LDA   #NLW_NAME_LEN
      STA   LIB_ARG1
      LDA   #<NLW_LOAD
      STA   LIB_ARG2
      LDA   #>NLW_LOAD
      STA   LIB_ARG2+1
      LDA   #<NLW_MAX
      STA   LIB_ARG3
      LDA   #>NLW_MAX
      STA   LIB_ARG3+1
      LDA   #SYS_OVL_LOAD
      JSR   tool_sys_call
      BNE   @done
      INC   worker_loaded
      JSR   tool_clear_args
      LDA   #SYS_OVL_MAIN
      JSR   tool_sys_call
      STA   config_expected
      JSR   tool_release_worker
      ORA   config_expected
@done:
      RTS

tool_release_worker:
      LDA   worker_loaded
      BEQ   @done
      STZ   worker_loaded
      JSR   tool_clear_args
      LDA   #SYS_OVL_UNLOAD
      JMP   tool_sys_call
@done:
      LDA   #0
      RTS

tool_clear_args:
      LDX   #15
@clear:
      STZ   LIB_ARG0,X
      DEX
      BPL   @clear
      RTS

tool_sys_call:
      STA   LIB_FN_ID
      LDA   #MODULE_ID_SYSTEM
      STA   LIB_MOD_ID
      JSR   LIB_LOADER_BAND
      LDA   LIB_STATUS
      RTS

; Supported ld65 subset:
; MEMORY { name: start=$hhhh, size=$hhhh[, file=%O]; ... }
; SEGMENTS { name: load=region[, type=...][, optional=...]; ... }
; SYMBOLS { name: value=$hhhh; ... }
config_parse:
      LDA   #<config_buf
      STA   config_ptr
      LDA   #>config_buf
      STA   config_ptr+1
      LDA   NPTOOL_IO_LEN
      STA   config_left
      LDA   NPTOOL_IO_LEN+1
      STA   config_left+1
      JSR   config_skip_ws
      JSR   config_peek
      long_bcc @bad
      JSR   config_upper
      CMP   #'N'
      BNE   @standalone
      LDA   #<cfg_npp
      LDX   #>cfg_npp
      JSR   config_expect_word
      long_bcs @bad
      LDA   #'1'
      JSR   config_expect_char
      long_bcs @bad
      JSR   config_seek_memory
      long_bcs @bad
      BRA   @memory_word
@standalone:
      LDA   #<cfg_memory
      LDX   #>cfg_memory
      JSR   config_expect_word
      long_bcs @bad
@memory_word:
      LDA   #$7B
      JSR   config_expect_char
      long_bcs @bad

@memory_region:
      JSR   config_skip_ws
      JSR   config_peek
      long_bcc @bad
      CMP   #'}'
      long_beq @memory_done
      JSR   config_parse_identifier
      long_bcs @bad
      LDA   #':'
      JSR   config_expect_char
      long_bcs @bad
      LDA   #<cfg_start
      LDX   #>cfg_start
      JSR   config_expect_word
      long_bcs @bad
      LDA   #'='
      JSR   config_expect_char
      long_bcs @bad
      JSR   config_parse_hex
      long_bcs @bad
      LDA   config_value
      STA   config_start
      LDA   config_value+1
      STA   config_start+1
      LDA   #','
      JSR   config_expect_char
      long_bcs @bad
      LDA   #<cfg_size
      LDX   #>cfg_size
      JSR   config_expect_word
      long_bcs @bad
      LDA   #'='
      JSR   config_expect_char
      long_bcs @bad
      JSR   config_parse_hex
      long_bcs @bad
      LDA   config_value
      STA   config_size
      LDA   config_value+1
      STA   config_size+1
      JSR   config_skip_ws
      JSR   config_peek
      long_bcc @bad
      CMP   #','
      BNE   @end_region
      JSR   config_next
      LDA   #<cfg_file
      LDX   #>cfg_file
      JSR   config_expect_word
      long_bcs @bad
      LDA   #'='
      JSR   config_expect_char
      long_bcs @bad
      LDA   #'%'
      JSR   config_expect_char
      long_bcs @bad
      JSR   config_skip_ws
      JSR   config_next
      long_bcc @bad
      JSR   config_upper
      CMP   #'O'
      long_bne @bad
@end_region:
      LDA   #';'
      JSR   config_expect_char
      long_bcs @bad
      JSR   config_add_region
      long_bcs @bad
      JMP   @memory_region

@memory_done:
      LDA   #'}'
      JSR   config_expect_char
      long_bcs @bad
      LDA   nlink_region_count
      long_beq @bad
      JSR   config_skip_ws
      LDA   config_left
      ORA   config_left+1
      long_beq @legacy

      LDA   #<cfg_segments
      LDX   #>cfg_segments
      JSR   config_expect_word
      long_bcs @bad
      LDA   #'{'
      JSR   config_expect_char
      long_bcs @bad
@segment:
      JSR   config_skip_ws
      JSR   config_peek
      long_bcc @bad
      CMP   #'}'
      long_beq @segments_done
      JSR   config_parse_identifier
      long_bcs @bad
      JSR   config_add_rule
      long_bcs @bad
      LDA   #':'
      JSR   config_expect_char
      long_bcs @bad
      LDA   #<cfg_load
      LDX   #>cfg_load
      JSR   config_expect_word
      long_bcs @bad
      LDA   #'='
      JSR   config_expect_char
      long_bcs @bad
      JSR   config_parse_identifier
      long_bcs @bad
      JSR   config_find_region
      long_bcs @bad
      TXA
      LDX   config_rule_index
      STA   nlink_rule_region,X
@segment_attrs:
      JSR   config_skip_ws
      JSR   config_peek
      long_bcc @bad
      CMP   #';'
      BEQ   @segment_end
      LDA   #','
      JSR   config_expect_char
      long_bcs @bad
      JSR   config_parse_identifier
      long_bcs @bad
      LDA   #<cfg_type
      LDX   #>cfg_type
      JSR   config_name_is
      BCC   :+
      BRA   @known_attr
:     LDA   #<cfg_optional
      LDX   #>cfg_optional
      JSR   config_name_is
      BCC   :+
      BRA   @known_attr
:     LDA   #<cfg_define
      LDX   #>cfg_define
      JSR   config_name_is
      long_bcc @bad
@known_attr:
      LDA   #'='
      JSR   config_expect_char
      long_bcs @bad
      JSR   config_parse_identifier
      long_bcs @bad
      BRA   @segment_attrs
@segment_end:
      LDA   #';'
      JSR   config_expect_char
      long_bcs @bad
      JMP   @segment

@segments_done:
      LDA   #'}'
      JSR   config_expect_char
      long_bcs @bad
      LDA   nlink_rule_count
      long_beq @bad
      JSR   config_skip_ws
      LDA   config_left
      ORA   config_left+1
      long_beq @configured
      LDA   #<cfg_symbols
      LDX   #>cfg_symbols
      JSR   config_expect_word
      long_bcs @bad
      LDA   #'{'
      JSR   config_expect_char
      long_bcs @bad
@symbol:
      JSR   config_skip_ws
      JSR   config_peek
      long_bcc @bad
      CMP   #'}'
      long_beq @symbols_done
      JSR   config_parse_identifier
      long_bcs @bad
      LDA   #':'
      JSR   config_expect_char
      long_bcs @bad
      LDA   #<cfg_value
      LDX   #>cfg_value
      JSR   config_expect_word
      long_bcs @bad
      LDA   #'='
      JSR   config_expect_char
      long_bcs @bad
      JSR   config_parse_hex
      long_bcs @bad
      LDA   #';'
      JSR   config_expect_char
      long_bcs @bad
      JSR   config_add_symbol
      long_bcs @bad
      JMP   @symbol
@symbols_done:
      LDA   #'}'
      JSR   config_expect_char
      long_bcs @bad
      JSR   config_skip_ws
      LDA   config_left
      ORA   config_left+1
      long_bne @bad
@configured:
      LDA   #1
      STA   nlink_config_enabled
      JSR   config_finish_layout
      long_bcs @bad
      LDA   #0
      RTS

@legacy:
      LDA   nlink_region_count
      CMP   #1
      long_bne @bad
      JSR   config_finish_layout
      long_bcs @bad
      LDA   #0
      RTS
@bad:
      LDA   #1
      RTS

config_add_region:
      LDA   nlink_region_count
      CMP   #NLINK_REGION_CAP
      BCS   @bad
      JSR   config_find_region
      BCC   @bad
      LDX   nlink_region_count
      LDA   config_name_len
      STA   nlink_region_name_len,X
      LDA   config_start
      STA   nlink_region_start_l,X
      LDA   config_start+1
      STA   nlink_region_start_h,X
      LDA   config_size
      STA   nlink_region_size_l,X
      LDA   config_size+1
      STA   nlink_region_size_h,X
      TXA
      ASL
      ASL
      ASL
      ASL
      CLC
      ADC   #<nlink_region_names
      STA   config_word
      LDA   #>nlink_region_names
      ADC   #0
      STA   config_word+1
      LDY   #0
@copy:
      CPY   config_name_len
      BCS   @done
      LDA   config_name,Y
      STA   (config_word),Y
      INY
      BRA   @copy
@done:
      INC   nlink_region_count
      CLC
      RTS
@bad:
      SEC
      RTS

; Inline project configs retain NL's ordinary grammar; only locate its first
; MEMORY block. NPC/NAS lines remain opaque to the linker.
config_seek_memory:
@line:
      JSR   config_skip_ws
      JSR   config_peek
      BCC   @bad
      JSR   config_upper
      CMP   #'M'
      BNE   @skip
      LDA   #<cfg_memory
      LDX   #>cfg_memory
      JSR   config_expect_word
      BCC   @ok
@skip:
      JSR   config_next
      BCC   @bad
      CMP   #$0A
      BNE   @skip
      BRA   @line
@ok:
      CLC
      RTS
@bad:
      SEC
      RTS

config_add_rule:
      LDA   nlink_rule_count
      CMP   #NLINK_RULE_CAP
      BCS   @bad
      JSR   config_find_rule
      BCC   @bad
      LDX   nlink_rule_count
      STX   config_rule_index
      LDA   config_name_len
      STA   nlink_rule_name_len,X
      TXA
      ASL
      ASL
      ASL
      ASL
      CLC
      ADC   #<nlink_rule_names
      STA   config_word
      LDA   #>nlink_rule_names
      ADC   #0
      STA   config_word+1
      LDY   #0
@copy:
      CPY   config_name_len
      BCS   @done
      LDA   config_name,Y
      STA   (config_word),Y
      INY
      BRA   @copy
@done:
      INC   nlink_rule_count
      CLC
      RTS
@bad:
      SEC
      RTS

config_add_symbol:
      LDA   nlink_config_symbol_count
      CMP   #NLINK_SYMBOL_CAP
      BCS   @bad
      JSR   config_find_symbol
      BCC   @bad
      LDX   nlink_config_symbol_count
      LDA   config_name_len
      STA   nlink_config_symbol_name_len,X
      LDA   config_value
      STA   nlink_config_symbol_value_l,X
      LDA   config_value+1
      STA   nlink_config_symbol_value_h,X
      TXA
      ASL
      ASL
      ASL
      ASL
      CLC
      ADC   #<nlink_config_symbol_names
      STA   config_word
      LDA   #>nlink_config_symbol_names
      ADC   #0
      STA   config_word+1
      LDY   #0
@copy:
      CPY   config_name_len
      BCS   @done
      LDA   config_name,Y
      STA   (config_word),Y
      INY
      BRA   @copy
@done:
      INC   nlink_config_symbol_count
      CLC
      RTS
@bad:
      SEC
      RTS

config_find_region:
      LDX   #0
@entry:
      CPX   nlink_region_count
      BCS   @missing
      LDA   nlink_region_name_len,X
      CMP   config_name_len
      BNE   @next
      STX   config_match_index
      TXA
      ASL
      ASL
      ASL
      ASL
      CLC
      ADC   #<nlink_region_names
      STA   config_word
      LDA   #>nlink_region_names
      ADC   #0
      STA   config_word+1
      LDY   #0
@compare:
      CPY   config_name_len
      BCS   @found
      LDA   config_name,Y
      CMP   (config_word),Y
      BNE   @next_saved
      INY
      BRA   @compare
@next_saved:
      LDX   config_match_index
@next:
      INX
      BRA   @entry
@found:
      LDX   config_match_index
      CLC
      RTS
@missing:
      SEC
      RTS

config_find_rule:
      LDX   #0
@entry:
      CPX   nlink_rule_count
      BCS   @missing
      LDA   nlink_rule_name_len,X
      CMP   config_name_len
      BNE   @next
      STX   config_match_index
      TXA
      ASL
      ASL
      ASL
      ASL
      CLC
      ADC   #<nlink_rule_names
      STA   config_word
      LDA   #>nlink_rule_names
      ADC   #0
      STA   config_word+1
      LDY   #0
@compare:
      CPY   config_name_len
      BCS   @found
      LDA   config_name,Y
      CMP   (config_word),Y
      BNE   @next_saved
      INY
      BRA   @compare
@next_saved:
      LDX   config_match_index
@next:
      INX
      BRA   @entry
@found:
      LDX   config_match_index
      CLC
      RTS
@missing:
      SEC
      RTS

config_find_symbol:
      LDX   #0
@entry:
      CPX   nlink_config_symbol_count
      BCS   @missing
      LDA   nlink_config_symbol_name_len,X
      CMP   config_name_len
      BNE   @next
      STX   config_match_index
      TXA
      ASL
      ASL
      ASL
      ASL
      CLC
      ADC   #<nlink_config_symbol_names
      STA   config_word
      LDA   #>nlink_config_symbol_names
      ADC   #0
      STA   config_word+1
      LDY   #0
@compare:
      CPY   config_name_len
      BCS   @found
      LDA   config_name,Y
      CMP   (config_word),Y
      BNE   @next_saved
      INY
      BRA   @compare
@next_saved:
      LDX   config_match_index
@next:
      INX
      BRA   @entry
@found:
      LDX   config_match_index
      CLC
      RTS
@missing:
      SEC
      RTS

; A/X -> uppercase NUL word; carry set when it equals config_name.
config_name_is:
      STA   config_word
      STX   config_word+1
      LDY   #0
@compare:
      CPY   config_name_len
      BCS   @end
      LDA   (config_word),Y
      BEQ   @no
      CMP   config_name,Y
      BNE   @no
      INY
      BRA   @compare
@end:
      LDA   (config_word),Y
      BNE   @no
      SEC
      RTS
@no:
      CLC
      RTS

config_finish_layout:
      LDA   #$FF
      STA   config_min
      STA   config_min+1
      STZ   config_max
      STZ   config_max+1
      LDX   #0
@region:
      CPX   nlink_region_count
      long_bcs @span
      LDA   nlink_region_size_l,X
      ORA   nlink_region_size_h,X
      long_beq @bad
      LDA   nlink_region_start_h,X
      CMP   #$70
      long_bcc @bad
      CLC
      LDA   nlink_region_start_l,X
      ADC   nlink_region_size_l,X
      STA   config_value
      LDA   nlink_region_start_h,X
      ADC   nlink_region_size_h,X
      STA   config_value+1
      long_bcs @bad
      CMP   #$A0
      BCC   @bounds_ok
      long_bne @bad
      LDA   config_value
      long_bne @bad
@bounds_ok:
      LDA   nlink_region_start_h,X
      CMP   config_min+1
      BCC   @new_min
      BNE   @max
      LDA   nlink_region_start_l,X
      CMP   config_min
      BCS   @max
@new_min:
      LDA   nlink_region_start_l,X
      STA   config_min
      LDA   nlink_region_start_h,X
      STA   config_min+1
@max:
      LDA   config_value+1
      CMP   config_max+1
      BCC   @next
      BNE   @new_max
      LDA   config_value
      CMP   config_max
      BCC   @next
      BEQ   @next
@new_max:
      LDA   config_value
      STA   config_max
      LDA   config_value+1
      STA   config_max+1
@next:
      INX
      JMP   @region

@span:
      LDA   config_min
      STA   nlink_load_base
      LDA   config_min+1
      STA   nlink_load_base+1
      SEC
      LDA   config_max
      SBC   config_min
      STA   config_size
      LDA   config_max+1
      SBC   config_min+1
      STA   config_size+1
      BCC   @bad
      LDA   nlink_config_enabled
      BNE   @configured_cap
      LDA   nlink_region_size_l
      STA   config_size
      LDA   nlink_region_size_h
      STA   config_size+1
@configured_cap:
      LDA   config_size+1
      CMP   #>(OUTPUT_CAP-2)
      BCC   @small
      BNE   @too_wide
      LDA   config_size
      CMP   #<(OUTPUT_CAP-2)
      BCC   @small
      BEQ   @full
@too_wide:
      LDA   nlink_config_enabled
      BNE   @bad
@full:
      LDA   #<OUTPUT_CAP
      STA   nlink_output_cap
      LDA   #>OUTPUT_CAP
      STA   nlink_output_cap+1
      CLC
      RTS
@small:
      CLC
      LDA   config_size
      ADC   #2
      STA   nlink_output_cap
      LDA   config_size+1
      ADC   #0
      STA   nlink_output_cap+1
      CLC
      RTS
@bad:
      SEC
      RTS

config_expect_word:
      STA   config_word
      STX   config_word+1
      JSR   config_skip_ws
      LDY   #0
@loop:
      LDA   (config_word),Y
      BEQ   @ok
      STA   config_expected
      PHY
      JSR   config_next
      PLY
      BCC   @bad
      JSR   config_upper
      CMP   config_expected
      BNE   @bad
      INY
      BRA   @loop
@ok:
      CLC
      RTS
@bad:
      SEC
      RTS

config_expect_char:
      STA   config_expected
      JSR   config_skip_ws
      JSR   config_next
      BCC   @bad
      CMP   config_expected
      BNE   @bad
      CLC
      RTS
@bad:
      SEC
      RTS

config_parse_identifier:
      JSR   config_skip_ws
      STZ   config_name_len
@loop:
      JSR   config_peek
      BCC   @ok
      JSR   config_upper
      LDX   config_name_len
      BNE   @continuation
      JSR   config_is_ident_start
      BRA   @classed
@continuation:
      JSR   config_is_ident
@classed:
      BCC   @ok
      LDX   config_name_len
      CPX   #NLINK_NAME_CAP
      BCS   @bad
      STA   config_name,X
      INC   config_name_len
      JSR   config_next
      BRA   @loop
@ok:
      LDA   config_name_len
      BEQ   @bad
      CLC
      RTS
@bad:
      SEC
      RTS

config_parse_hex:
      LDA   #'$'
      JSR   config_expect_char
      BCS   @bad
      STZ   config_value
      STZ   config_value+1
      STZ   config_digits
@digit:
      JSR   config_peek
      BCC   @done
      JSR   config_hex_nibble
      BCC   @done
      STA   config_expected
      LDA   config_digits
      CMP   #4
      BCS   @bad
      JSR   config_next
      ASL   config_value
      ROL   config_value+1
      ASL   config_value
      ROL   config_value+1
      ASL   config_value
      ROL   config_value+1
      ASL   config_value
      ROL   config_value+1
      LDA   config_value
      ORA   config_expected
      STA   config_value
      INC   config_digits
      BRA   @digit
@done:
      LDA   config_digits
      BEQ   @bad
      CLC
      RTS
@bad:
      SEC
      RTS

config_skip_ws:
@again:
      JSR   config_peek
      BCC   @done
      CMP   #' '
      BEQ   @eat
      CMP   #$09
      BEQ   @eat
      CMP   #$0D
      BEQ   @eat
      CMP   #$0A
      BEQ   @eat
      CMP   #'#'
      BNE   @done
@comment:
      JSR   config_next
      BCC   @done
      CMP   #$0A
      BNE   @comment
      BRA   @again
@eat:
      JSR   config_next
      BRA   @again
@done:
      RTS

config_peek:
      LDA   config_left
      ORA   config_left+1
      BEQ   @eof
      LDY   #0
      LDA   (config_ptr),Y
      SEC
      RTS
@eof:
      CLC
      RTS

config_next:
      JSR   config_peek
      BCC   @eof
      PHA
      INC   config_ptr
      BNE   :+
      INC   config_ptr+1
:     LDA   config_left
      BNE   :+
      DEC   config_left+1
:     DEC   config_left
      PLA
      SEC
@eof:
      RTS

config_upper:
      CMP   #'a'
      BCC   @done
      CMP   #'z'+1
      BCS   @done
      AND   #$DF
@done:
      RTS

config_is_ident_start:
      CMP   #'_'
      BEQ   @yes
      CMP   #'A'
      BCC   @no
      CMP   #'Z'+1
      BCS   @no
@yes:
      SEC
      RTS
@no:
      CLC
      RTS

config_is_ident:
      JSR   config_is_ident_start
      BCS   @yes
      CMP   #'0'
      BCC   @no
      CMP   #'9'+1
      BCS   @no
@yes:
      SEC
      RTS
@no:
      CLC
      RTS

config_hex_nibble:
      JSR   config_upper
      CMP   #'0'
      BCC   @bad
      CMP   #'9'+1
      BCS   @letter
      SEC
      SBC   #'0'
      SEC
      RTS
@letter:
      CMP   #'A'
      BCC   @bad
      CMP   #'F'+1
      BCS   @bad
      SEC
      SBC   #'A'-10
      SEC
      RTS
@bad:
      CLC
      RTS

      .segment "RODATA"
nl_banner:  .byte "Nova Linker v1.0", $0D, $0A, 0
nl_linking: .byte "Linking ", 0
nl_library: .byte "Library ", 0
nl_config:  .byte "Config ", 0
nl_config_valid:.byte "Configuration valid", $0D, $0A, 0
nl_stripping:.byte "Stripping unused sections", $0D, $0A, 0
nl_writing: .byte "Writing ", 0
nl_map:     .byte "Map ", 0
nl_labels:  .byte "Labels ", 0
nl_worker_name:.byte "NLWORK.OVL"
nl_ok:      .byte "Link successful", $0D, $0A, 0
cfg_npp:    .byte "NPP", 0
cfg_memory: .byte "MEMORY", 0
cfg_segments:.byte "SEGMENTS", 0
cfg_symbols:.byte "SYMBOLS", 0
cfg_start:  .byte "START", 0
cfg_size:   .byte "SIZE", 0
cfg_file:   .byte "FILE", 0
cfg_load:   .byte "LOAD", 0
cfg_type:   .byte "TYPE", 0
cfg_optional:.byte "OPTIONAL", 0
cfg_define: .byte "DEFINE", 0
cfg_value:  .byte "VALUE", 0

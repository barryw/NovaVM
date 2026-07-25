; Nova Build State: exact-content incremental checks shared by every language.

      .setcpu "w65c02"
      .include "nova.inc"
      .include "libfiles.inc"
      .include "nptool.inc"
      .include "nbuild.inc"

NBUILD_STATE_CAP    = $2000
NBUILD_MANIFEST_CAP = $2000
NBUILD_RECORD_CAP   = $1000
NBUILD_DEP_CAP      = $FF

      .segment "ZEROPAGE"
nb_scan_ptr:     .res 2
nb_scan_left:    .res 2
nb_record_ptr:   .res 2
nb_record_left:  .res 2
nb_out_ptr:      .res 2
nb_copy_ptr:     .res 2
nb_dep_count_ptr:.res 2
nb_dep_list_ptr: .res 2
nb_dep_scan_ptr: .res 2

      .segment "BSS"
nb_state_len:    .res 2
nb_manifest_len: .res 2
nb_record_len:   .res 2
nb_out_len:      .res 2
nb_payload_len:  .res 2
nb_record_count: .res 1
nb_out_count:    .res 1
nb_dep_count:    .res 1
nb_dep_scan_count:.res 1
nb_dep_scan_len: .res 1
nb_line_len:     .res 1
nb_saved_hash:   .res 4
nb_line:         .res NPTOOL_ARG_CAP

      .segment "NOINIT"
nb_state:        .res NBUILD_STATE_CAP
nb_work:         .res NBUILD_MANIFEST_CAP
nb_record:       .res NBUILD_RECORD_CAP

      .segment "CODE"
      .export tool_main

tool_main:
      JSR   nptool_clear_bss
      STZ   NPTOOL_STATUS
      STZ   NPTOOL_DETAIL
      LDA   #<nb_banner
      LDX   #>nb_banner
      JSR   nptool_print_z
      LDA   NPTOOL_FLAGS
      CMP   #NBUILD_OP_CHECK
      BNE   :+
      JMP   nb_check
:
      CMP   #NBUILD_OP_COMMIT
      BNE   :+
      JMP   nb_commit
:
      CMP   #NBUILD_OP_CLEAN
      BNE   :+
      JMP   nb_clean
:
      JMP   nb_bad_args

nb_check:
      JSR   nb_require_common
      BCC   :+
      JMP   nb_bad_args
:
      JSR   nb_load_state
      CMP   #0
      BNE   nb_dirty
      JSR   nb_find_record
      BCS   nb_dirty
      JSR   nb_check_record
      BCS   nb_dirty
      STZ   NPTOOL_DETAIL
      LDA   #<nb_hit
      LDX   #>nb_hit
      JSR   nb_print_node
      LDA   #0
      RTS

nb_dirty:
      LDA   #NBUILD_DIRTY
      STA   NPTOOL_DETAIL
      STZ   NPTOOL_STATUS
      LDA   #<nb_miss
      LDX   #>nb_miss
      JSR   nb_print_node
      LDA   #0
      RTS

nb_commit:
      JSR   nb_require_common
      BCC   :+
      JMP   nb_bad_args
:
      LDA   NPTOOL_ARG4_LEN
      BNE   :+
      JMP   nb_bad_args
:
      LDA   NPTOOL_ARG5_LEN
      BNE   :+
      JMP   nb_bad_args
:
      JSR   nb_build_record
      BCC   :+
      JMP   nb_fail
:
      JSR   nb_rewrite_state
      BCC   :+
      JMP   nb_fail
:
      STZ   NPTOOL_DETAIL
      LDA   #<nb_updated
      LDX   #>nb_updated
      JSR   nb_print_node
      LDA   #0
      RTS

nb_clean:
      LDA   NPTOOL_ARG0_LEN
      BNE   :+
      JMP   nb_bad_args
:
      JSR   nptool_clear_args
      LDA   #<NPTOOL_ARG0
      STA   LIB_ARG0
      LDA   #>NPTOOL_ARG0
      STA   LIB_ARG0+1
      LDA   NPTOOL_ARG0_LEN
      STA   LIB_ARG1
      LDA   #FILE_FDELETE
      JSR   nptool_files_call
      BEQ   @ok
      LDA   FIO_ERRCODE
      CMP   #FIO_ERR_NOTFOUND
      BEQ   @ok
      JMP   nb_io_fail
@ok:
      STZ   NPTOOL_STATUS
      STZ   NPTOOL_DETAIL
      LDA   #0
      RTS

nb_require_common:
      LDA   NPTOOL_ARG0_LEN
      BEQ   @bad
      LDA   NPTOOL_ARG1_LEN
      BEQ   @bad
      LDA   NPTOOL_ARG2_LEN
      BEQ   @bad
      LDA   NPTOOL_ARG3_LEN
      BEQ   @bad
      CLC
      RTS
@bad:
      SEC
      RTS

; Return A=0 loaded, A=1 absent/corrupt, A=2 hard I/O failure.
nb_load_state:
      LDA   #<nb_state
      STA   NPTOOL_IO_ADDR
      LDA   #>nb_state
      STA   NPTOOL_IO_ADDR+1
      LDA   #<NBUILD_STATE_CAP
      STA   NPTOOL_IO_CAP
      LDA   #>NBUILD_STATE_CAP
      STA   NPTOOL_IO_CAP+1
      JSR   nptool_load_arg0
      BEQ   @loaded
      LDA   NPTOOL_DETAIL
      CMP   #FIO_ERR_NOTFOUND
      BEQ   @missing
      LDA   #2
      RTS
@missing:
      STZ   NPTOOL_STATUS
      STZ   NPTOOL_DETAIL
      LDA   #1
      RTS
@loaded:
      LDA   NPTOOL_IO_LEN
      STA   nb_state_len
      LDA   NPTOOL_IO_LEN+1
      STA   nb_state_len+1
      JSR   nb_validate_header
      BCS   @missing
      LDA   #0
      RTS

nb_validate_header:
      LDA   nb_state_len+1
      BNE   @size_ok
      LDA   nb_state_len
      CMP   #5
      BCC   @bad
@size_ok:
      LDX   #0
@magic:
      LDA   nb_state,X
      CMP   nb_magic,X
      BNE   @bad
      INX
      CPX   #4
      BCC   @magic
      LDA   nb_state+4
      STA   nb_record_count
      CLC
      RTS
@bad:
      SEC
      RTS

nb_reset_scan:
      LDA   #<(nb_state+5)
      STA   nb_scan_ptr
      LDA   #>(nb_state+5)
      STA   nb_scan_ptr+1
      SEC
      LDA   nb_state_len
      SBC   #5
      STA   nb_scan_left
      LDA   nb_state_len+1
      SBC   #0
      STA   nb_scan_left+1
      RTS

; Carry clear leaves nb_record_ptr/left on the matching record payload.
nb_find_record:
      JSR   nb_reset_scan
      LDX   nb_record_count
@record:
      CPX   #0
      BEQ   @missing
      PHX
      JSR   nb_next_record
      BCS   @corrupt_pop
      JSR   nb_record_matches_node
      BCC   @found_pop
      JSR   nb_skip_record
      PLX
      DEX
      BRA   @record
@found_pop:
      PLX
      CLC
      RTS
@corrupt_pop:
      PLX
@missing:
      SEC
      RTS

; Decode the next record length without consuming its payload.
nb_next_record:
      LDA   #2
      JSR   nb_scan_need
      BCS   @bad
      LDY   #0
      LDA   (nb_scan_ptr),Y
      STA   nb_payload_len
      INY
      LDA   (nb_scan_ptr),Y
      STA   nb_payload_len+1
      JSR   nb_scan_advance2
      LDA   nb_payload_len+1
      CMP   nb_scan_left+1
      BCC   @fits
      BNE   @bad
      LDA   nb_payload_len
      CMP   nb_scan_left
      BCC   @fits
      BNE   @bad
@fits:
      LDA   nb_scan_ptr
      STA   nb_record_ptr
      LDA   nb_scan_ptr+1
      STA   nb_record_ptr+1
      LDA   nb_payload_len
      STA   nb_record_left
      LDA   nb_payload_len+1
      STA   nb_record_left+1
      CLC
      RTS
@bad:
      SEC
      RTS

nb_skip_record:
      CLC
      LDA   nb_scan_ptr
      ADC   nb_payload_len
      STA   nb_scan_ptr
      LDA   nb_scan_ptr+1
      ADC   nb_payload_len+1
      STA   nb_scan_ptr+1
      SEC
      LDA   nb_scan_left
      SBC   nb_payload_len
      STA   nb_scan_left
      LDA   nb_scan_left+1
      SBC   nb_payload_len+1
      STA   nb_scan_left+1
      RTS

nb_record_matches_node:
      JSR   nb_record_get
      BCS   @no
      CMP   NPTOOL_ARG2_LEN
      BNE   @no
      TAX
      LDY   #0
@byte:
      CPY   NPTOOL_ARG2_LEN
      BCS   @yes
      JSR   nb_record_get
      BCS   @no
      CMP   NPTOOL_ARG2,Y
      BNE   @no
      INY
      BRA   @byte
@yes:
      CLC
      RTS
@no:
      SEC
      RTS

; The matching record pointer already follows its node name.
nb_check_record:
      JSR   nb_record_get
      BCC   :+
      JMP   @dirty
:
      CMP   NPTOOL_ARG3_LEN
      BNE   @dirty
      LDY   #0
@identity:
      CPY   NPTOOL_ARG3_LEN
      BCS   @output_hash
      JSR   nb_record_get
      BCS   @dirty
      CMP   NPTOOL_ARG3,Y
      BNE   @dirty
      INY
      BRA   @identity
@output_hash:
      JSR   nb_read_saved_hash
      BCS   @dirty
      LDA   #<NPTOOL_ARG1
      LDX   #>NPTOOL_ARG1
      LDY   NPTOOL_ARG1_LEN
      JSR   nptool_hash_named
      BNE   @dirty
      JSR   nb_hash_matches
      BCS   @dirty
      JSR   nb_record_get
      BCS   @dirty
      STA   nb_dep_count
@dependency:
      LDA   nb_dep_count
      BEQ   @end
      JSR   nb_read_record_name
      BCS   @dirty
      JSR   nb_read_saved_hash
      BCS   @dirty
      LDA   #<nb_line
      LDX   #>nb_line
      LDY   nb_line_len
      JSR   nptool_hash_named
      BNE   @dirty
      JSR   nb_hash_matches
      BCS   @dirty
      DEC   nb_dep_count
      BRA   @dependency
@end:
      LDA   nb_record_left
      ORA   nb_record_left+1
      BNE   @dirty
      CLC
      RTS
@dirty:
      SEC
      RTS

nb_read_record_name:
      JSR   nb_record_get
      BCS   @bad
      BEQ   @bad
      CMP   #NPTOOL_ARG_CAP
      BCS   @bad
      STA   nb_line_len
      TAX
      LDY   #0
@copy:
      CPY   nb_line_len
      BCS   @done
      JSR   nb_record_get
      BCS   @bad
      STA   nb_line,Y
      INY
      BRA   @copy
@done:
      LDA   #0
      STA   nb_line,Y
      CLC
      RTS
@bad:
      SEC
      RTS

nb_read_saved_hash:
      LDX   #0
@byte:
      JSR   nb_record_get
      BCS   @bad
      STA   nb_saved_hash,X
      INX
      CPX   #4
      BCC   @byte
      CLC
      RTS
@bad:
      SEC
      RTS

nb_hash_matches:
      LDX   #0
@byte:
      LDA   LIB_RESULT,X
      CMP   nb_saved_hash,X
      BNE   @no
      INX
      CPX   #4
      BCC   @byte
      CLC
      RTS
@no:
      SEC
      RTS

; Build one complete replacement record from ARG2/ARG3/ARG4 and ARG1's hash.
nb_build_record:
      LDA   #<(nb_record+2)
      STA   nb_out_ptr
      LDA   #>(nb_record+2)
      STA   nb_out_ptr+1
      LDA   #2
      STA   nb_record_len
      STZ   nb_record_len+1
      LDA   NPTOOL_ARG2_LEN
      JSR   nb_record_append_a
      BCS   @capacity
      LDA   #<NPTOOL_ARG2
      LDX   #>NPTOOL_ARG2
      LDY   NPTOOL_ARG2_LEN
      JSR   nb_record_append_named
      BCS   @capacity
      LDA   NPTOOL_ARG3_LEN
      JSR   nb_record_append_a
      BCS   @capacity
      LDA   #<NPTOOL_ARG3
      LDX   #>NPTOOL_ARG3
      LDY   NPTOOL_ARG3_LEN
      JSR   nb_record_append_named
      BCS   @capacity
      LDA   #<NPTOOL_ARG1
      LDX   #>NPTOOL_ARG1
      LDY   NPTOOL_ARG1_LEN
      JSR   nptool_hash_named
      BNE   @io
      JSR   nb_append_result_hash
      BCS   @capacity
      ; Reserve the dependency count byte and remember its address.
      LDA   nb_out_ptr
      STA   nb_dep_count_ptr
      LDA   nb_out_ptr+1
      STA   nb_dep_count_ptr+1
      LDA   #0
      JSR   nb_record_append_a
      BCS   @capacity
      LDA   nb_out_ptr
      STA   nb_dep_list_ptr
      LDA   nb_out_ptr+1
      STA   nb_dep_list_ptr+1
      STZ   nb_dep_count
      JSR   nb_load_manifest
      BCS   @fail
      JSR   nb_append_manifest_dependencies
      BCS   @fail
      LDA   nb_dep_count
      BEQ   @manifest
      LDY   #0
      STA   (nb_dep_count_ptr),Y
      SEC
      LDA   nb_record_len
      SBC   #2
      STA   nb_record
      LDA   nb_record_len+1
      SBC   #0
      STA   nb_record+1
      CLC
      RTS
@manifest:
      LDA   #NBUILD_DETAIL_MANIFEST
      JMP   nb_project_fail
@capacity:
      LDA   #NBUILD_DETAIL_CAPACITY
      JMP   nb_project_fail
@io:
      JMP   nb_io_fail
@fail:
      SEC
      RTS

nb_load_manifest:
      LDA   #<nb_work
      STA   NPTOOL_IO_ADDR
      LDA   #>nb_work
      STA   NPTOOL_IO_ADDR+1
      LDA   #<NBUILD_MANIFEST_CAP
      STA   NPTOOL_IO_CAP
      LDA   #>NBUILD_MANIFEST_CAP
      STA   NPTOOL_IO_CAP+1
      JSR   nptool_load_arg4
      BNE   @bad
      LDA   NPTOOL_IO_LEN
      STA   nb_manifest_len
      LDA   NPTOOL_IO_LEN+1
      STA   nb_manifest_len+1
      CLC
      RTS
@bad:
      SEC
      RTS

nb_append_manifest_dependencies:
      LDA   #<nb_work
      STA   nb_scan_ptr
      LDA   #>nb_work
      STA   nb_scan_ptr+1
      LDA   nb_manifest_len
      STA   nb_scan_left
      LDA   nb_manifest_len+1
      STA   nb_scan_left+1
      STZ   nb_line_len
@byte:
      LDA   nb_scan_left
      ORA   nb_scan_left+1
      BEQ   @eof
      LDY   #0
      LDA   (nb_scan_ptr),Y
      PHA
      JSR   nb_scan_advance1
      PLA
      CMP   #$0A
      BEQ   @line
      CMP   #$0D
      BEQ   @byte
      CMP   #' '
      BNE   @store
      LDA   nb_line_len
      BEQ   @byte
      LDA   #' '
@store:
      LDX   nb_line_len
      CPX   #NPTOOL_ARG_CAP-1
      BCS   @manifest
      STA   nb_line,X
      INC   nb_line_len
      BRA   @byte
@line:
      JSR   nb_finish_manifest_line
      BCS   @bad
      BRA   @byte
@eof:
      JSR   nb_finish_manifest_line
      RTS
@manifest:
      LDA   #NBUILD_DETAIL_MANIFEST
      JSR   nb_project_fail
@bad:
      SEC
      RTS

nb_finish_manifest_line:
@trim:
      LDX   nb_line_len
      BEQ   @empty
      DEX
      LDA   nb_line,X
      CMP   #' '
      BEQ   @drop
      CMP   #$09
      BNE   @ready
@drop:
      STX   nb_line_len
      BRA   @trim
@ready:
      LDA   nb_dep_count
      CMP   #NBUILD_DEP_CAP
      BCS   @capacity
      JSR   nb_dependency_exists
      BCS   @empty
      LDY   nb_line_len
      LDA   #0
      STA   nb_line,Y
      LDA   #<nb_line
      LDX   #>nb_line
      JSR   nptool_hash_named
      BNE   @io
      LDA   nb_line_len
      JSR   nb_record_append_a
      BCS   @capacity
      LDA   #<nb_line
      LDX   #>nb_line
      LDY   nb_line_len
      JSR   nb_record_append_named
      BCS   @capacity
      JSR   nb_append_result_hash
      BCS   @capacity
      INC   nb_dep_count
@empty:
      STZ   nb_line_len
      CLC
      RTS
@capacity:
      LDA   #NBUILD_DETAIL_CAPACITY
      JMP   nb_project_fail
@io:
      JMP   nb_io_fail

; Carry set when the current manifest line is already in this record. NAS may
; encounter a guarded NDK include more than once; one exact hash is sufficient.
nb_dependency_exists:
      LDA   nb_dep_list_ptr
      STA   nb_dep_scan_ptr
      LDA   nb_dep_list_ptr+1
      STA   nb_dep_scan_ptr+1
      LDA   nb_dep_count
      STA   nb_dep_scan_count
@dependency:
      LDA   nb_dep_scan_count
      BEQ   @missing
      LDY   #0
      LDA   (nb_dep_scan_ptr),Y
      STA   nb_dep_scan_len
      CMP   nb_line_len
      BNE   @next
      LDX   #0
      LDY   #1
@name:
      CPX   nb_line_len
      BCS   @found
      LDA   (nb_dep_scan_ptr),Y
      CMP   nb_line,X
      BNE   @next
      INX
      INY
      BRA   @name
@next:
      CLC
      LDA   nb_dep_scan_len
      ADC   #5                    ; length byte + CRC-32
      STA   nb_dep_scan_len
      CLC
      LDA   nb_dep_scan_ptr
      ADC   nb_dep_scan_len
      STA   nb_dep_scan_ptr
      LDA   nb_dep_scan_ptr+1
      ADC   #0
      STA   nb_dep_scan_ptr+1
      DEC   nb_dep_scan_count
      BRA   @dependency
@found:
      SEC
      RTS
@missing:
      CLC
      RTS

nb_rewrite_state:
      JSR   nb_load_state
      CMP   #2
      BEQ   @hard_io
      CMP   #0
      BEQ   @existing
      STZ   nb_record_count
@existing:
      JSR   nb_output_header
      LDA   nb_record_count
      BEQ   @append
      JSR   nb_reset_scan
      LDX   nb_record_count
@old:
      CPX   #0
      BEQ   @append
      PHX
      LDA   nb_scan_ptr
      STA   nb_copy_ptr
      LDA   nb_scan_ptr+1
      STA   nb_copy_ptr+1
      JSR   nb_next_record
      BCS   @corrupt_pop
      JSR   nb_record_matches_node
      BCC   @skip
      ; Copy the length word plus payload from the original state.
      LDA   nb_payload_len
      CLC
      ADC   #2
      STA   nb_record_left
      LDA   nb_payload_len+1
      ADC   #0
      STA   nb_record_left+1
      JSR   nb_output_copy
      BCS   @capacity_pop
      INC   nb_out_count
@skip:
      JSR   nb_skip_record
      PLX
      DEX
      BRA   @old
@corrupt_pop:
      PLX
      ; A corrupt cache is disposable; rebuild it from this successful node.
      JSR   nb_output_header
@append:
      LDA   #<nb_record
      STA   nb_copy_ptr
      LDA   #>nb_record
      STA   nb_copy_ptr+1
      LDA   nb_record_len
      STA   nb_record_left
      LDA   nb_record_len+1
      STA   nb_record_left+1
      JSR   nb_output_copy
      BCS   @capacity
      INC   nb_out_count
      LDA   nb_out_count
      STA   nb_work+4
      JSR   nb_save_state_atomic
      RTS
@capacity_pop:
      PLX
@capacity:
      LDA   #NBUILD_DETAIL_CAPACITY
      JMP   nb_project_fail
@hard_io:
      JMP   nb_io_fail

nb_output_header:
      LDX   #0
@magic:
      LDA   nb_magic,X
      STA   nb_work,X
      INX
      CPX   #4
      BCC   @magic
      STZ   nb_work+4
      STZ   nb_out_count
      LDA   #<(nb_work+5)
      STA   nb_out_ptr
      LDA   #>(nb_work+5)
      STA   nb_out_ptr+1
      LDA   #5
      STA   nb_out_len
      STZ   nb_out_len+1
      RTS

nb_output_copy:
@byte:
      LDA   nb_record_left
      ORA   nb_record_left+1
      BEQ   @done
      JSR   nb_output_has_space
      BCS   @bad
      LDY   #0
      LDA   (nb_copy_ptr),Y
      STA   (nb_out_ptr),Y
      JSR   nb_copy_advance
      JSR   nb_out_advance
      BRA   @byte
@done:
      CLC
      RTS
@bad:
      SEC
      RTS

nb_save_state_atomic:
      LDA   #<nb_work
      STA   NPTOOL_IO_ADDR
      LDA   #>nb_work
      STA   NPTOOL_IO_ADDR+1
      LDA   nb_out_len
      STA   NPTOOL_IO_LEN
      LDA   nb_out_len+1
      STA   NPTOOL_IO_LEN+1
      LDA   #<NPTOOL_ARG5
      LDX   #>NPTOOL_ARG5
      LDY   NPTOOL_ARG5_LEN
      JSR   nptool_save_named
      BNE   @bad
      JSR   nptool_clear_args
      LDA   #<NPTOOL_ARG5
      STA   LIB_ARG0
      LDA   #>NPTOOL_ARG5
      STA   LIB_ARG0+1
      LDA   NPTOOL_ARG5_LEN
      STA   LIB_ARG1
      LDA   #<NPTOOL_ARG0
      STA   LIB_ARG2
      LDA   #>NPTOOL_ARG0
      STA   LIB_ARG2+1
      LDA   NPTOOL_ARG0_LEN
      STA   LIB_ARG3
      LDA   #FILE_FRENAME
      JSR   nptool_files_call
      BEQ   @ok
@bad:
      JSR   nb_delete_temp
      JMP   nb_io_fail
@ok:
      CLC
      RTS

nb_delete_temp:
      JSR   nptool_clear_args
      LDA   #<NPTOOL_ARG5
      STA   LIB_ARG0
      LDA   #>NPTOOL_ARG5
      STA   LIB_ARG0+1
      LDA   NPTOOL_ARG5_LEN
      STA   LIB_ARG1
      LDA   #FILE_FDELETE
      JMP   nptool_files_call

; Append helpers for nb_record.
nb_record_append_named:
      STA   nb_copy_ptr
      STX   nb_copy_ptr+1
      STY   nb_record_left
      STZ   nb_record_left+1
@byte:
      LDA   nb_record_left
      BEQ   @done
      LDY   #0
      LDA   (nb_copy_ptr),Y
      JSR   nb_record_append_a
      BCS   @bad
      INC   nb_copy_ptr
      BNE   :+
      INC   nb_copy_ptr+1
:     DEC   nb_record_left
      BRA   @byte
@done:
      CLC
      RTS
@bad:
      SEC
      RTS

nb_append_result_hash:
      LDX   #0
@byte:
      LDA   LIB_RESULT,X
      JSR   nb_record_append_a
      BCS   @bad
      INX
      CPX   #4
      BCC   @byte
      CLC
      RTS
@bad:
      SEC
      RTS

nb_record_append_a:
      PHA
      LDA   nb_record_len+1
      CMP   #>NBUILD_RECORD_CAP
      BCC   @store
      BNE   @bad
      LDA   nb_record_len
      CMP   #<NBUILD_RECORD_CAP
      BCS   @bad
@store:
      PLA
      LDY   #0
      STA   (nb_out_ptr),Y
      INC   nb_out_ptr
      BNE   :+
      INC   nb_out_ptr+1
:     INC   nb_record_len
      BNE   :+
      INC   nb_record_len+1
:     CLC
      RTS
@bad:
      PLA
      SEC
      RTS

; Consume one matching-record byte.
nb_record_get:
      LDA   nb_record_left
      ORA   nb_record_left+1
      BEQ   @bad
      LDA   (nb_record_ptr)
      PHA
      INC   nb_record_ptr
      BNE   :+
      INC   nb_record_ptr+1
:     LDA   nb_record_left
      BNE   :+
      DEC   nb_record_left+1
:     DEC   nb_record_left
      PLA
      CLC
      RTS
@bad:
      SEC
      RTS

nb_scan_need:
      STA   nb_saved_hash
      LDA   nb_scan_left+1
      BNE   @ok
      LDA   nb_scan_left
      CMP   nb_saved_hash
      BCC   @bad
@ok:
      CLC
      RTS
@bad:
      SEC
      RTS

nb_scan_advance2:
      JSR   nb_scan_advance1
nb_scan_advance1:
      INC   nb_scan_ptr
      BNE   :+
      INC   nb_scan_ptr+1
:     LDA   nb_scan_left
      BNE   :+
      DEC   nb_scan_left+1
:     DEC   nb_scan_left
      RTS

nb_copy_advance:
      INC   nb_copy_ptr
      BNE   :+
      INC   nb_copy_ptr+1
:     LDA   nb_record_left
      BNE   :+
      DEC   nb_record_left+1
:     DEC   nb_record_left
      RTS

nb_out_advance:
      INC   nb_out_ptr
      BNE   :+
      INC   nb_out_ptr+1
:     INC   nb_out_len
      BNE   :+
      INC   nb_out_len+1
:     RTS

nb_output_has_space:
      LDA   nb_out_len+1
      CMP   #>NBUILD_STATE_CAP
      BCC   @ok
      BNE   @bad
      LDA   nb_out_len
      CMP   #<NBUILD_STATE_CAP
      BCS   @bad
@ok:
      CLC
      RTS
@bad:
      SEC
      RTS

nb_print_node:
      JSR   nptool_print_z
      LDA   #<NPTOOL_ARG2
      LDX   #>NPTOOL_ARG2
      JSR   nptool_print_z
      JMP   nptool_newline

nb_bad_args:
      LDA   #NPTOOL_ERR_ARGS
      STA   NPTOOL_STATUS
      LDA   #1
      RTS

nb_project_fail:
      STA   NPTOOL_DETAIL
      LDA   #NPTOOL_ERR_BUILD
      STA   NPTOOL_STATUS
      SEC
      RTS

nb_io_fail:
      LDA   FIO_ERRCODE
      BNE   :+
      LDA   LIB_STATUS
:     STA   NPTOOL_DETAIL
      LDA   #NPTOOL_ERR_IO
      STA   NPTOOL_STATUS
nb_fail:
      LDA   #1
      RTS

      .segment "RODATA"
nb_magic:   .byte "NBS1"
nb_banner:  .byte "Nova Build State v1.0", $0D, $0A, 0
nb_hit:     .byte "Up to date: ", 0
nb_miss:    .byte "Rebuild: ", 0
nb_updated: .byte "Recorded: ", 0

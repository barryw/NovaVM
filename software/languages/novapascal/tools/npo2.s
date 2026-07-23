; NovaPascal O2 optimizer: compact typed IR -> optimized textual 65C02.

      .setcpu "w65c02"
      .include "nova.inc"
      .include "libfiles.inc"
      .include "libmemory.inc"
      .include "pager.inc"
      .include "nptool.inc"
      .include "longbranch.inc"
      .include "xram.inc"
      .include "xramstream.inc"
      .include "npproj.inc"

ASM_CAP = $4000
ASM_LOAD_CAP = ASM_CAP + 1
LARGE_CHUNK_TARGET = $1000
LARGE_XRAM_PAGE_SIZE = $4000
LARGE_XRAM_PAGE_CAP = 32
INLINE_ROUTINE_CAP = 64
INLINE_ROUTINE_NAME_CAP = 64

      .segment "ZEROPAGE"
text_ptr:        .res 2

      .segment "BSS"
buffer_a:        .res 3
buffer_b:        .res 3
buffer_a_owned:  .res 1
buffer_b_owned:  .res 1
input_base:      .res 3
input_addr:      .res 3
input_pos:       .res 3
input_len:       .res 3
output_base:     .res 3
output_addr:     .res 3
output_len:      .res 3
output_cap:      .res 2
saved_pos:       .res 3
lookahead_pos:   .res 3
line_len:        .res 1
line_has_lf:     .res 1
line_buf:        .res 256
large_page_buf:  .res 256
pass_id:         .res 1
pending_compare: .res 1
operand_pos:     .res 1
immediate_hi:    .res 1
immediate_lo:    .res 1
word_operation:  .res 1
symbol_len:      .res 1
symbol_buf:      .res 64
byte_value:      .res 1
opt_error:       .res 1
release_error:   .res 1
saved_text:      .res 2
rhs_start:       .res 3
rhs_stop:        .res 3
rhs_end:         .res 3
compare_op:      .res 1
result_fast:     .res 1
result_seen:     .res 1
target_len:      .res 1
target_buf:      .res 64
scan_return:     .res 3
nested_return:   .res 3
routine_body:    .res 3
routine_type:    .res 1
call_count:      .res 1
candidate_index_len: .res 1
candidate_index_buf: .res 64
candidate_array_len: .res 1
candidate_array_buf: .res 64
cache_index_len: .res 1
cache_index_buf: .res 64
cache_array_len: .res 1
cache_array_buf: .res 64
cache_valid:     .res 1
window_index_len: .res 1
window_index_buf: .res 64
window_array_len: .res 1
window_array_buf: .res 64
window_active:   .res 1
window_count:    .res 1
window_min:      .res 2
window_max:      .res 2
candidate_offset: .res 2
offset_value:    .res 1
hex_value:       .res 1
parse_return:    .res 3
large_mode:      .res 1
large_input_id:  .res 2
large_output_id: .res 2
large_input_open:.res 1
large_output_open:.res 1
large_page_left: .res 2
large_page_index:.res 1
large_chunk_len: .res 2
large_write_addr:.res 3
large_read_addr:.res 3
large_remaining:.res 2
large_saved_status:.res 1
large_cache_base:.res 3
large_cache_len: .res 2
large_cache_valid:.res 1
large_output_buf:.res 256
large_output_used:.res 1
large_input_name:.res 2
large_input_name_len:.res 1
large_output_name:.res 2
large_output_name_len:.res 1
large_xram_active:.res 1
large_xram_count:.res 1
large_xram_page: .res 1
large_xram_remaining:.res 3
large_xram_offset:.res 3
large_xram_len:  .res 2
large_xram_addr_l:.res LARGE_XRAM_PAGE_CAP
large_xram_addr_m:.res LARGE_XRAM_PAGE_CAP
large_xram_addr_h:.res LARGE_XRAM_PAGE_CAP
large_xram_size_l:.res LARGE_XRAM_PAGE_CAP
large_xram_size_h:.res LARGE_XRAM_PAGE_CAP
large_routine_count:.res 1
large_routine_current:.res 1
large_routine_index:.res 1
large_routine_name_len:.res INLINE_ROUTINE_CAP
large_routine_type:.res INLINE_ROUTINE_CAP
large_routine_body_l:.res INLINE_ROUTINE_CAP
large_routine_body_m:.res INLINE_ROUTINE_CAP
large_routine_body_h:.res INLINE_ROUTINE_CAP
large_routine_calls:.res INLINE_ROUTINE_CAP
large_routine_leaf:.res INLINE_ROUTINE_CAP
large_routine_names:.res INLINE_ROUTINE_CAP * INLINE_ROUTINE_NAME_CAP

      .segment "CODE"
      .export tool_main

tool_main:
      JSR   nptool_clear_bss
      STZ   NPTOOL_STATUS
      STZ   NPTOOL_DETAIL
      LDA   #<optimizer_banner
      LDX   #>optimizer_banner
      JSR   nptool_print_z
      LDA   NPTOOL_ARG1_LEN
      BNE   :+
      JMP   bad_args
:
      LDA   #<optimizer_file
      LDX   #>optimizer_file
      JSR   nptool_print_z
      LDA   #<NPTOOL_ARG1
      LDX   #>NPTOOL_ARG1
      JSR   nptool_print_z
      JSR   nptool_newline
      JSR   allocate_buffers
      BEQ   :+
      JMP   memory_error
:
      JSR   load_source
      BEQ   :+
      JMP   io_error
:
      LDA   XRAM_LENH
      CMP   #>ASM_LOAD_CAP
      BNE   @small_source
      LDA   XRAM_LENL
      CMP   #<ASM_LOAD_CAP
      BNE   @small_source
      JSR   optimize_large_file
      BEQ   @large_done
      CMP   #2
      BNE   @large_io_error
      JMP   output_error
@large_io_error:
      JMP   io_error
@large_done:
      JSR   release_buffers
      long_bne memory_error
      LDA   #<optimizer_ok
      LDX   #>optimizer_ok
      JSR   nptool_print_z
      LDA   #0
      RTS
@small_source:
      LDA   XRAM_LENL
      STA   input_len
      LDA   XRAM_LENH
      STA   input_len+1

      LDA   #1
      STA   pass_id
@pass:
      LDX   pass_id
      DEX
      LDA   pass_message_lo,X
      LDY   pass_message_hi,X
      TAX
      TYA
      PHA
      TXA
      PLX
      JSR   nptool_print_z
      LDA   pass_id
      AND   #1
      BEQ   @even
      LDX   #2
@odd_address:
      LDA   buffer_a,X
      STA   input_base,X
      LDA   buffer_b,X
      STA   output_base,X
      DEX
      BPL   @odd_address
      BRA   @run
@even:
      LDX   #2
@even_address:
      LDA   buffer_b,X
      STA   input_base,X
      LDA   buffer_a,X
      STA   output_base,X
      DEX
      BPL   @even_address
@run:
      JSR   run_pass
      BEQ   :+
      JMP   output_error
:
      LDA   output_len
      STA   input_len
      LDA   output_len+1
      STA   input_len+1
      INC   pass_id
      LDA   pass_id
      CMP   #7
      BCC   @pass

      JSR   save_result
      BNE   io_error
      JSR   release_buffers
      BNE   memory_error
      LDA   #<optimizer_ok
      LDX   #>optimizer_ok
      JSR   nptool_print_z
      LDA   #0
      RTS

bad_args:
      LDA   #NPTOOL_ERR_ARGS
      STA   NPTOOL_STATUS
      LDA   #1
      RTS

memory_error:
      LDA   LIB_STATUS
      STA   NPTOOL_DETAIL
      LDA   #NPTOOL_ERR_MEMORY
      BRA   fail

io_error:
      LDA   NPTOOL_DETAIL
      BNE   :+
      LDA   LIB_STATUS
      STA   NPTOOL_DETAIL
:
      LDA   #NPTOOL_ERR_IO
      BRA   fail

output_error:
      LDA   opt_error
      BNE   :+
      LDA   #1
:
      STA   NPTOOL_DETAIL
      LDA   #NPTOOL_ERR_COMPILE
fail:
      STA   NPTOOL_STATUS
      JSR   release_buffers
      LDA   #1
      RTS

; ---------------------------------------------------------------------
; XRAM ownership and file streaming.
; ---------------------------------------------------------------------

clear_lib_args:
      LDX   #15
@clear:
      STZ   LIB_ARG0,X
      DEX
      BPL   @clear
      RTS

memory_call:
      STA   LIB_FN_ID
      LDA   #MODULE_ID_MEMORY
      STA   LIB_MOD_ID
      JSR   LIB_LOADER_BAND
      LDA   LIB_STATUS
      RTS

files_call:
      STA   LIB_FN_ID
      LDA   #MODULE_ID_FILES
      STA   LIB_MOD_ID
      JSR   LIB_LOADER_BAND
      LDA   LIB_STATUS
      RTS

allocate_one:
      JSR   clear_lib_args
      LDA   #<ASM_LOAD_CAP
      STA   LIB_ARG2
      LDA   #>ASM_LOAD_CAP
      STA   LIB_ARG2+1
      LDA   #MEM_ALLOC
      JMP   memory_call

allocate_buffers:
      JSR   allocate_one
      BNE   @done
      LDX   #2
@a:
      LDA   LIB_RESULT,X
      STA   buffer_a,X
      DEX
      BPL   @a
      INC   buffer_a_owned
      JSR   allocate_one
      BNE   @done
      LDX   #2
@b:
      LDA   LIB_RESULT,X
      STA   buffer_b,X
      DEX
      BPL   @b
      INC   buffer_b_owned
      LDA   #0
@done:
      RTS

release_one:
      JSR   clear_lib_args
      LDX   #2
@address:
      LDA   input_base,X
      STA   LIB_ARG0,X
      DEX
      BPL   @address
      LDA   #<ASM_LOAD_CAP
      STA   LIB_ARG2
      LDA   #>ASM_LOAD_CAP
      STA   LIB_ARG2+1
      LDA   #MEM_RELEASE
      JMP   memory_call

release_buffers:
      STZ   release_error
      LDA   buffer_b_owned
      BEQ   @a
      STZ   buffer_b_owned
      LDX   #2
@copy_b:
      LDA   buffer_b,X
      STA   input_base,X
      DEX
      BPL   @copy_b
      JSR   release_one
      BEQ   @a
      STA   release_error
@a:
      LDA   buffer_a_owned
      BEQ   @done
      STZ   buffer_a_owned
      LDX   #2
@copy_a:
      LDA   buffer_a,X
      STA   input_base,X
      DEX
      BPL   @copy_a
      JSR   release_one
@done:
      ORA   release_error
      RTS

load_source:
      JSR   clear_lib_args
      LDA   #<NPTOOL_ARG1
      STA   LIB_ARG0
      LDA   #>NPTOOL_ARG1
      STA   LIB_ARG0+1
      LDA   NPTOOL_ARG1_LEN
      STA   LIB_ARG1
      LDX   #2
@address:
      LDA   buffer_a,X
      STA   LIB_ARG2,X
      DEX
      BPL   @address
      LDA   #<ASM_LOAD_CAP
      STA   LIB_ARG3
      LDA   #>ASM_LOAD_CAP
      STA   LIB_ARG3+1
      LDA   #MEM_XLOAD
      JMP   memory_call

save_result:
      LDA   output_len
      STA   $0D0D
      LDA   output_len+1
      STA   $0D0E
      JSR   clear_lib_args
      LDA   #<NPTOOL_ARG1
      STA   LIB_ARG0
      LDA   #>NPTOOL_ARG1
      STA   LIB_ARG0+1
      LDA   NPTOOL_ARG1_LEN
      STA   LIB_ARG1
      LDX   #2
@address:
      LDA   buffer_a,X
      STA   LIB_ARG2,X
      DEX
      BPL   @address
      LDA   output_len
      STA   LIB_ARG3
      LDA   output_len+1
      STA   LIB_ARG3+1
      LDA   #MEM_XSAVE
      JMP   memory_call

; Large inputs run the same six whole-file passes through a seekable 24-bit
; disk stream. Only a single input page and output page are resident in RAM.
optimize_large_file:
      INC   large_mode
      JSR   release_buffers
      BNE   @fail
      LDA   #1
      STA   pass_id
@pass:
      JSR   print_pass_message
      JSR   large_select_names
      JSR   large_open_input
      BNE   @fail
      JSR   large_open_output
      BNE   @fail
      JSR   run_pass
      BNE   @compile_fail
      JSR   large_flush_output
      BNE   @fail
      JSR   large_close_output
      BNE   @fail
      JSR   large_close_input
      BNE   @fail
      INC   pass_id
      LDA   pass_id
      CMP   #7
      BCC   @pass
      JSR   large_replace_source
      BNE   @fail
      JSR   large_delete_a
      LDA   #0
      RTS
@fail:
      LDA   FIO_ERRCODE
      BNE   :+
      LDA   LIB_STATUS
:     STA   large_saved_status
      JSR   large_close_input
      JSR   large_close_output
      JSR   large_delete_a
      JSR   large_delete_b
      LDA   large_saved_status
      STA   NPTOOL_DETAIL
      LDA   #1
      RTS
@compile_fail:
      JSR   large_close_input
      JSR   large_close_output
      JSR   large_delete_a
      JSR   large_delete_b
      LDA   #2
      RTS

print_pass_message:
      LDX   pass_id
      DEX
      LDA   pass_message_lo,X
      LDY   pass_message_hi,X
      TAX
      TYA
      PHA
      TXA
      PLX
      JMP   nptool_print_z

large_select_names:
      LDA   pass_id
      CMP   #1
      BNE   @temporary_input
      LDA   #<NPTOOL_ARG1
      LDX   #>NPTOOL_ARG1
      LDY   NPTOOL_ARG1_LEN
      BRA   @input
@temporary_input:
      AND   #1
      BEQ   @input_a
      LDA   #<large_temp_b
      LDX   #>large_temp_b
      LDY   #large_temp_b_end-large_temp_b
      BRA   @input
@input_a:
      LDA   #<large_temp_a
      LDX   #>large_temp_a
      LDY   #large_temp_a_end-large_temp_a
@input:
      STA   large_input_name
      STX   large_input_name+1
      STY   large_input_name_len
      LDA   pass_id
      AND   #1
      BEQ   @output_b
      LDA   #<large_temp_a
      LDX   #>large_temp_a
      LDY   #large_temp_a_end-large_temp_a
      BRA   @output
@output_b:
      LDA   #<large_temp_b
      LDX   #>large_temp_b
      LDY   #large_temp_b_end-large_temp_b
@output:
      STA   large_output_name
      STX   large_output_name+1
      STY   large_output_name_len
      RTS

large_open_input:
      JSR   clear_lib_args
      LDA   large_input_name
      STA   LIB_ARG0
      LDA   large_input_name+1
      STA   LIB_ARG0+1
      LDA   large_input_name_len
      STA   LIB_ARG1
      LDA   #FIO_FILE_ACCESS_READ
      STA   LIB_ARG2
      LDA   #FILE_FOPEN
      JSR   files_call
      BNE   @done
      LDA   LIB_RESULT
      STA   large_input_id
      LDA   LIB_RESULT+1
      STA   large_input_id+1
      INC   large_input_open
      JSR   clear_lib_args
      LDA   large_input_id
      STA   LIB_ARG0
      LDA   large_input_id+1
      STA   LIB_ARG0+1
      LDA   #FILE_FSIZE
      JSR   files_call
      BNE   @done
      LDA   LIB_RESULT
      STA   input_len
      LDA   LIB_RESULT+1
      STA   input_len+1
      LDA   LIB_RESULT+2
      STA   input_len+2
      STZ   large_cache_valid
      JSR   large_cache_input_xram
      BNE   @done
      LDA   #0
@done:
      RTS

large_open_output:
      JSR   clear_lib_args
      LDA   large_output_name
      STA   LIB_ARG0
      LDA   large_output_name+1
      STA   LIB_ARG0+1
      LDA   large_output_name_len
      STA   LIB_ARG1
      LDA   #FIO_FILE_ACCESS_WRITE
      STA   LIB_ARG2
      LDA   #FILE_FCREATE
      JSR   files_call
      BNE   @done
      LDA   LIB_RESULT
      STA   large_output_id
      LDA   LIB_RESULT+1
      STA   large_output_id+1
      INC   large_output_open
      STZ   large_output_used
      LDA   #0
@done:
      RTS

; Cache the current pass input in independently allocated 16 KiB XRAM pages.
; If XRAM is exhausted, release the partial cache and retain the disk pager.
large_cache_input_xram:
      STZ   large_xram_active
      STZ   large_xram_count
      LDX   #2
@copy_length:
      LDA   input_len,X
      STA   large_xram_remaining,X
      STZ   large_xram_offset,X
      DEX
      BPL   @copy_length
@page:
      LDA   large_xram_remaining
      ORA   large_xram_remaining+1
      ORA   large_xram_remaining+2
      long_beq @ready
      LDX   large_xram_count
      CPX   #LARGE_XRAM_PAGE_CAP
      long_bcs @fallback
      STZ   large_xram_len
      LDA   #>LARGE_XRAM_PAGE_SIZE
      STA   large_xram_len+1
      LDA   large_xram_remaining+2
      BNE   @allocate
      LDA   large_xram_remaining+1
      CMP   #>LARGE_XRAM_PAGE_SIZE
      BCS   @allocate
      LDA   large_xram_remaining
      STA   large_xram_len
      LDA   large_xram_remaining+1
      STA   large_xram_len+1
@allocate:
      JSR   clear_lib_args
      LDA   large_xram_len
      STA   LIB_ARG2
      LDA   large_xram_len+1
      STA   LIB_ARG2+1
      LDA   #MEM_ALLOC
      JSR   memory_call
      long_bne @fallback
      LDX   large_xram_count
      LDA   LIB_RESULT
      STA   large_xram_addr_l,X
      LDA   LIB_RESULT+1
      STA   large_xram_addr_m,X
      LDA   LIB_RESULT+2
      STA   large_xram_addr_h,X
      LDA   large_xram_len
      STA   large_xram_size_l,X
      LDA   large_xram_len+1
      STA   large_xram_size_h,X
      STX   large_xram_page
      INC   large_xram_count
      JSR   clear_lib_args
      LDA   large_input_name
      STA   LIB_ARG0
      LDA   large_input_name+1
      STA   LIB_ARG0+1
      LDA   large_input_name_len
      STA   LIB_ARG0+2
      LDX   #2
@offset:
      LDA   large_xram_offset,X
      STA   LIB_ARG1,X
      DEX
      BPL   @offset
      LDA   #PAGER_TARGET_XRAM
      STA   LIB_ARG1+3
      LDX   large_xram_page
      LDA   large_xram_addr_l,X
      STA   LIB_ARG2
      LDA   large_xram_addr_m,X
      STA   LIB_ARG2+1
      LDA   large_xram_addr_h,X
      STA   LIB_ARG2+2
      LDA   large_xram_len
      STA   LIB_ARG3
      LDA   large_xram_len+1
      STA   LIB_ARG3+1
      LDA   #FILE_PAGE
      JSR   files_call
      BNE   @fallback
      LDA   LIB_RESULT
      CMP   large_xram_len
      BNE   @fallback
      LDA   LIB_RESULT+1
      CMP   large_xram_len+1
      BNE   @fallback
      CLC
      LDA   large_xram_offset
      ADC   large_xram_len
      STA   large_xram_offset
      LDA   large_xram_offset+1
      ADC   large_xram_len+1
      STA   large_xram_offset+1
      LDA   large_xram_offset+2
      ADC   #0
      STA   large_xram_offset+2
      SEC
      LDA   large_xram_remaining
      SBC   large_xram_len
      STA   large_xram_remaining
      LDA   large_xram_remaining+1
      SBC   large_xram_len+1
      STA   large_xram_remaining+1
      LDA   large_xram_remaining+2
      SBC   #0
      STA   large_xram_remaining+2
      JMP   @page
@ready:
      INC   large_xram_active
      LDA   #0
      RTS
@fallback:
      JMP   large_release_xram

large_release_xram:
      STZ   large_xram_active
      STZ   release_error
@page:
      LDX   large_xram_count
      BEQ   @done
      DEX
      STX   large_xram_count
      STX   large_xram_page
      JSR   clear_lib_args
      LDX   large_xram_page
      LDA   large_xram_addr_l,X
      STA   LIB_ARG0
      LDA   large_xram_addr_m,X
      STA   LIB_ARG0+1
      LDA   large_xram_addr_h,X
      STA   LIB_ARG0+2
      LDA   large_xram_size_l,X
      STA   LIB_ARG2
      LDA   large_xram_size_h,X
      STA   LIB_ARG2+1
      LDA   #MEM_RELEASE
      JSR   memory_call
      BEQ   @page
      STA   release_error
      BRA   @page
@done:
      LDA   release_error
      RTS

large_input_at_end:
      LDA   input_pos+2
      CMP   input_len+2
      BCC   @no
      BNE   @yes
      LDA   input_pos+1
      CMP   input_len+1
      BCC   @no
      BNE   @yes
      LDA   input_pos
      CMP   input_len
      BCC   @no
@yes:
      SEC
      RTS
@no:
      CLC
      RTS

large_map_input:
      LDA   large_xram_active
      BNE   @ok
      JMP   large_ensure_input_page
@ok:
      LDA   #0
      RTS

large_read_input_byte:
      LDA   large_xram_active
      BEQ   @disk
      JSR   large_map_xram_input
      LDX   input_addr
      LDA   XRAM_WIN2_BASE,X
      RTS
@disk:
      JSR   large_ensure_input_page
      BNE   @fail
      LDX   input_pos
      LDA   large_page_buf,X
      RTS
@fail:
      LDA   #0
      RTS

large_map_xram_input:
      LDA   input_pos+1
      LSR
      LSR
      LSR
      LSR
      LSR
      LSR
      STA   large_xram_page
      LDA   input_pos+2
      ASL
      ASL
      ORA   large_xram_page
      TAX
      CLC
      LDA   large_xram_addr_l,X
      ADC   input_pos
      STA   input_addr
      LDA   large_xram_addr_m,X
      STA   input_addr+1
      LDA   input_pos+1
      AND   #$3F
      ADC   input_addr+1
      STA   input_addr+1
      LDA   large_xram_addr_h,X
      ADC   #0
      STA   input_addr+2
      STZ   XMC_W2AL
      LDA   input_addr+1
      STA   XMC_W2AM
      LDA   input_addr+2
      STA   XMC_W2AH
      RTS

large_increment_input:
      INC   input_pos
      BNE   @done
      INC   input_pos+1
      BNE   @done
      INC   input_pos+2
@done:
      RTS

large_ensure_input_page:
      LDA   large_cache_valid
      BEQ   @load
      LDA   input_pos+1
      CMP   large_cache_base+1
      BNE   @load
      LDA   input_pos+2
      CMP   large_cache_base+2
      BEQ   @ok
@load:
      STZ   large_cache_base
      LDA   input_pos+1
      STA   large_cache_base+1
      LDA   input_pos+2
      STA   large_cache_base+2
      JSR   clear_lib_args
      LDA   large_input_id
      STA   LIB_ARG0
      LDA   large_input_id+1
      STA   LIB_ARG0+1
      LDX   #2
@position:
      LDA   large_cache_base,X
      STA   LIB_ARG1,X
      DEX
      BPL   @position
      LDA   #FILE_FSEEK
      JSR   files_call
      BNE   @fail
      JSR   clear_lib_args
      LDA   large_input_id
      STA   LIB_ARG0
      LDA   large_input_id+1
      STA   LIB_ARG0+1
      LDA   #<large_page_buf
      STA   LIB_ARG1
      LDA   #>large_page_buf
      STA   LIB_ARG1+1
      STZ   LIB_ARG2
      LDA   #1
      STA   LIB_ARG2+1
      LDA   #FILE_FREAD
      JSR   files_call
      BNE   @fail
      LDA   LIB_RESULT
      STA   large_cache_len
      LDA   LIB_RESULT+1
      STA   large_cache_len+1
      ORA   large_cache_len
      BEQ   @fail
      INC   large_cache_valid
@ok:
      LDA   #0
      RTS
@fail:
      LDA   #5
      STA   opt_error
      LDA   #1
      RTS

large_emit_byte:
      LDX   large_output_used
      STA   large_output_buf,X
      INC   large_output_used
      INC   output_len
      BNE   @flush
      INC   output_len+1
      BNE   @flush
      INC   output_len+2
      BNE   @flush
      LDA   #3
      STA   opt_error
      LDA   #1
      RTS
@flush:
      LDA   large_output_used
      BNE   @ok
      STZ   large_chunk_len
      LDA   #1
      STA   large_chunk_len+1
      BRA   large_write_output
@ok:
      LDA   #0
      RTS

large_flush_output:
      LDA   large_output_used
      BEQ   large_flush_done
      STA   large_chunk_len
      STZ   large_chunk_len+1
large_write_output:
      JSR   clear_lib_args
      LDA   large_output_id
      STA   LIB_ARG0
      LDA   large_output_id+1
      STA   LIB_ARG0+1
      LDA   #<large_output_buf
      STA   LIB_ARG1
      LDA   #>large_output_buf
      STA   LIB_ARG1+1
      LDA   large_chunk_len
      STA   LIB_ARG2
      LDA   large_chunk_len+1
      STA   LIB_ARG2+1
      LDA   #FILE_FWRITE
      JSR   files_call
      BNE   large_write_fail
      LDA   LIB_RESULT
      CMP   large_chunk_len
      BNE   large_write_fail
      LDA   LIB_RESULT+1
      CMP   large_chunk_len+1
      BNE   large_write_fail
      STZ   large_output_used
large_flush_done:
      LDA   #0
      RTS
large_write_fail:
      LDA   #3
      STA   opt_error
      LDA   #1
      RTS

large_read_page:
      JSR   clear_lib_args
      LDA   large_input_id
      STA   LIB_ARG0
      LDA   large_input_id+1
      STA   LIB_ARG0+1
      LDA   #<large_page_buf
      STA   LIB_ARG1
      LDA   #>large_page_buf
      STA   LIB_ARG1+1
      STZ   LIB_ARG2
      LDA   #1
      STA   LIB_ARG2+1
      LDA   #FILE_FREAD
      JSR   files_call
      BNE   @done
      LDA   LIB_RESULT
      STA   large_page_left
      LDA   LIB_RESULT+1
      STA   large_page_left+1
      STZ   large_page_index
      LDA   #0
@done:
      RTS

; Return 0 with a non-empty chunk, 2 at clean EOF, or 1 on failure.
large_load_chunk:
      STZ   large_chunk_len
      STZ   large_chunk_len+1
      LDX   #2
@address:
      LDA   buffer_a,X
      STA   large_write_addr,X
      DEX
      BPL   @address
      LDA   XMC_WINCTL
      ORA   #XRAM_WIN2_ENABLE
      STA   XMC_WINCTL
@next:
      LDA   large_page_left
      ORA   large_page_left+1
      BNE   @byte
      JSR   large_read_page
      BNE   @fail
      LDA   large_page_left
      ORA   large_page_left+1
      BEQ   @eof
@byte:
      LDX   large_page_index
      LDA   large_page_buf,X
      STA   byte_value
      JSR   large_store_byte
      INC   large_page_index
      LDA   large_page_left
      BNE   :+
      DEC   large_page_left+1
:     DEC   large_page_left
      INC   large_chunk_len
      BNE   :+
      INC   large_chunk_len+1
:     LDA   byte_value
      CMP   #$0A
      BNE   @next
      LDA   large_chunk_len+1
      CMP   #>LARGE_CHUNK_TARGET
      BCC   @next
      BNE   @ready
      LDA   large_chunk_len
      CMP   #<LARGE_CHUNK_TARGET
      BCC   @next
@ready:
      LDA   #0
      RTS
@eof:
      LDA   large_chunk_len
      ORA   large_chunk_len+1
      BEQ   @clean_eof
      LDA   #0
      RTS
@clean_eof:
      LDA   #2
      RTS
@fail:
      LDA   #1
      RTS

large_store_byte:
      PHA
      LDA   large_write_addr+1
      STA   XMC_W2AM
      LDA   large_write_addr+2
      STA   XMC_W2AH
      LDY   large_write_addr
      PLA
      STA   XRAM_WIN2_BASE,Y
      INC   large_write_addr
      BNE   @done
      INC   large_write_addr+1
      BNE   @done
      INC   large_write_addr+2
@done:
      RTS

large_append_result:
      LDA   output_len
      STA   large_remaining
      LDA   output_len+1
      STA   large_remaining+1
      LDX   #2
@base:
      LDA   buffer_a,X
      STA   large_read_addr,X
      DEX
      BPL   @base
@page:
      LDA   large_remaining
      ORA   large_remaining+1
      long_beq @done
      STZ   large_chunk_len
      LDA   #1
      STA   large_chunk_len+1
      LDA   large_remaining+1
      BNE   @copy
      LDA   large_remaining
      STA   large_chunk_len
      STZ   large_chunk_len+1
@copy:
      JSR   clear_lib_args
      LDX   #2
@source:
      LDA   large_read_addr,X
      STA   LIB_ARG0,X
      DEX
      BPL   @source
      LDA   #<line_buf
      STA   LIB_ARG1
      LDA   #>line_buf
      STA   LIB_ARG1+1
      LDA   large_chunk_len
      STA   LIB_ARG2
      LDA   large_chunk_len+1
      STA   LIB_ARG2+1
      LDA   #MEM_COPY_TO_RAM
      JSR   memory_call
      BNE   @fail
      JSR   clear_lib_args
      LDA   large_output_id
      STA   LIB_ARG0
      LDA   large_output_id+1
      STA   LIB_ARG0+1
      LDA   #<line_buf
      STA   LIB_ARG1
      LDA   #>line_buf
      STA   LIB_ARG1+1
      LDA   large_chunk_len
      STA   LIB_ARG2
      LDA   large_chunk_len+1
      STA   LIB_ARG2+1
      LDA   #FILE_FWRITE
      JSR   files_call
      BNE   @fail
      LDA   LIB_RESULT
      CMP   large_chunk_len
      BNE   @fail
      LDA   LIB_RESULT+1
      CMP   large_chunk_len+1
      BNE   @fail
      CLC
      LDA   large_read_addr
      ADC   large_chunk_len
      STA   large_read_addr
      LDA   large_read_addr+1
      ADC   large_chunk_len+1
      STA   large_read_addr+1
      BCC   :+
      INC   large_read_addr+2
:     SEC
      LDA   large_remaining
      SBC   large_chunk_len
      STA   large_remaining
      LDA   large_remaining+1
      SBC   large_chunk_len+1
      STA   large_remaining+1
      JMP   @page
@done:
      LDA   #0
      RTS
@fail:
      LDA   #1
      RTS

large_close_input:
      JSR   large_release_xram
      STA   release_error
      LDA   large_input_open
      BEQ   @released
      STZ   large_input_open
      JSR   clear_lib_args
      LDA   large_input_id
      STA   LIB_ARG0
      LDA   large_input_id+1
      STA   LIB_ARG0+1
      LDA   #FILE_FCLOSE
      JSR   files_call
      ORA   release_error
      RTS
@released:
      LDA   release_error
      RTS

large_close_output:
      LDA   large_output_open
      BEQ   @ok
      STZ   large_output_open
      JSR   clear_lib_args
      LDA   large_output_id
      STA   LIB_ARG0
      LDA   large_output_id+1
      STA   LIB_ARG0+1
      LDA   #FILE_FCLOSE
      JMP   files_call
@ok:
      LDA   #0
      RTS

large_replace_source:
      JSR   clear_lib_args
      LDA   #<large_temp_b
      STA   LIB_ARG0
      LDA   #>large_temp_b
      STA   LIB_ARG0+1
      LDA   #large_temp_b_end-large_temp_b
      STA   LIB_ARG1
      LDA   #<NPTOOL_ARG1
      STA   LIB_ARG2
      LDA   #>NPTOOL_ARG1
      STA   LIB_ARG2+1
      LDA   NPTOOL_ARG1_LEN
      STA   LIB_ARG3
      LDA   #FILE_FRENAME
      JMP   files_call

large_delete_a:
      LDA   #<large_temp_a
      LDX   #>large_temp_a
      LDY   #large_temp_a_end-large_temp_a
      BRA   large_delete_named

large_delete_b:
      LDA   #<large_temp_b
      LDX   #>large_temp_b
      LDY   #large_temp_b_end-large_temp_b

large_delete_named:
      STA   large_input_name
      STX   large_input_name+1
      STY   large_input_name_len
      JSR   clear_lib_args
      LDA   large_input_name
      STA   LIB_ARG0
      LDA   large_input_name+1
      STA   LIB_ARG0+1
      LDA   large_input_name_len
      STA   LIB_ARG1
      LDA   #FILE_FDELETE
      JMP   files_call

; ---------------------------------------------------------------------
; Pass driver and XRAM line stream.
; ---------------------------------------------------------------------

run_pass:
      STZ   input_pos
      STZ   input_pos+1
      STZ   input_pos+2
      STZ   output_len
      STZ   output_len+1
      STZ   output_len+2
      STZ   pending_compare
      STZ   opt_error
      STZ   cache_valid
      LDA   large_mode
      BEQ   @analysis_done
      LDA   pass_id
      CMP   #2
      BCC   @analysis_done
      CMP   #5
      BCS   @analysis_done
      JSR   large_analyze_routines
      BNE   @fail
@analysis_done:
      LDA   #<ASM_CAP
      STA   output_cap
      LDA   #>ASM_CAP
      STA   output_cap+1
      LDX   #2
@output_address:
      LDA   output_base,X
      STA   output_addr,X
      DEX
      BPL   @output_address
      LDA   XMC_WINCTL
      ORA   #XRAM_WIN2_ENABLE | XRAM_WIN3_ENABLE
      STA   XMC_WINCTL
      JSR   map_output
@next:
      LDA   pass_id
      STA   $0D0A
      LDA   input_pos
      STA   $0D0B
      LDA   input_pos+1
      STA   $0D0C
      JSR   read_line
      BEQ   @line
      LDA   opt_error
      BNE   @fail
      LDA   pending_compare
      BEQ   @done
      JSR   emit_materialized_compare
      BNE   @fail
      STZ   pending_compare
@done:
      LDA   #0
      RTS
@line:
      LDA   pass_id
      CMP   #1
      BNE   @inline
      JSR   process_typed
      BRA   @result
@inline:
      LDA   pass_id
      CMP   #5
      BCS   @late
      JSR   process_inline
      BRA   @result
@late:
      CMP   #5
      BNE   @pass6
      JSR   process_lowering
      BRA   @result
@pass6:
      JSR   process_peephole
@result:
      BEQ   @next
@fail:
      LDA   #1
      RTS

read_line:
      JSR   input_at_end
      BCC   :+
      LDA   #1
      RTS
:
      JSR   map_input
      LDA   opt_error
      BNE   @error
      STZ   line_len
      STZ   line_has_lf
      LDX   #0
@byte:
      JSR   input_at_end
      BCS   @done
      PHX
      JSR   read_input_byte
      STA   byte_value
      LDA   opt_error
      BNE   @read_error
      JSR   increment_input
      PLX
      LDA   byte_value
      CMP   #$0A
      BEQ   @lf
      STA   line_buf,X
      INX
      BNE   @byte
      LDA   #4
      STA   opt_error
      LDA   #1
      RTS
@read_error:
      PLX
@error:
      LDA   #1
      RTS
@lf:
      INC   line_has_lf
@done:
      STX   line_len
      LDA   #0
      RTS

      xram_stream_impl opt_error, 3, large_mode

emit_text:
      STA   text_ptr
      STX   text_ptr+1
      LDY   #0
@byte:
      LDA   (text_ptr),Y
      BEQ   @done
      JSR   emit_byte
      BNE   @done
      INC   text_ptr
      BNE   @byte
      INC   text_ptr+1
      BRA   @byte
@done:
      RTS

emit_line:
      LDX   #0
@byte:
      CPX   line_len
      BEQ   @lf
      LDA   line_buf,X
      PHX
      JSR   emit_byte
      PLX
      CMP   #0
      BNE   @done
      INX
      BRA   @byte
@lf:
      LDA   line_has_lf
      BEQ   @ok
      LDA   #$0A
      JMP   emit_byte
@ok:
      LDA   #0
@done:
      RTS

; A/X = zero-terminated text. A=0 exact match, A=1 otherwise.
line_equals:
      STA   text_ptr
      STX   text_ptr+1
      LDY   #0
@char:
      LDA   (text_ptr),Y
      BEQ   @end
      CPY   line_len
      BCS   @no
      CMP   line_buf,Y
      BNE   @no
      INY
      BRA   @char
@end:
      CPY   line_len
      BNE   @no
      LDA   #0
      RTS
@no:
      LDA   #1
      RTS

; A/X = zero-terminated prefix. A=0 match, A=1 otherwise.
line_starts:
      STA   text_ptr
      STX   text_ptr+1
      LDY   #0
@char:
      LDA   (text_ptr),Y
      BEQ   @yes
      CPY   line_len
      BCS   @no
      CMP   line_buf,Y
      BNE   @no
      INY
      BRA   @char
@yes:
      LDA   #0
      RTS
@no:
      LDA   #1
      RTS

emit_operand:
@char:
      LDX   operand_pos
      CPX   line_len
      BEQ   @done
      LDA   line_buf,X
      INC   operand_pos
      JSR   emit_byte
      BEQ   @char
@done:
      RTS

; ---------------------------------------------------------------------
; Passes 1-4: typed IR canonicalization followed by leaf inlining.
; ---------------------------------------------------------------------

process_typed:
      JSR   invalidate_array_cache
      LDA   NPP_PLAN_BASE+NPP_PLAN_TARGET
      CMP   #NPP_TARGET_UNIT
      BEQ   @unit_export_done
      JSR   line_is_unit_export
      BNE   @unit_export_done
      LDA   #0
      RTS
@unit_export_done:
      LDA   #<ir_routine
      LDX   #>ir_routine
      JSR   line_starts
      BNE   @not_routine
      LDA   line_buf+5
      STA   routine_type
      LDA   #7
      JSR   capture_target
      long_bne @raw
      JSR   analyze_array_window
      JSR   emit_routine_target
      long_bne @done
      LDA   window_active
      long_beq @done
      JMP   emit_window_pointer
@not_routine:
      LDA   #<ir_routine_end
      LDX   #>ir_routine_end
      JSR   line_starts
      BNE   @not_end
      STZ   window_active
@not_end:
      LDA   #<ir_function_entry_wide
      LDX   #>ir_function_entry_wide
      JSR   line_equals
      BNE   @byte_function_entry
      JSR   analyze_function_result
      BEQ   @fast_result
      LDA   #<ir_function_entry_wide_line
      LDX   #>ir_function_entry_wide_line
      JMP   emit_text
@byte_function_entry:
      LDA   #<ir_function_entry
      LDX   #>ir_function_entry
      JSR   line_equals
      BNE   @function_store
      JSR   analyze_function_result
      BEQ   @fast_result
      LDA   #<ir_function_entry_line
      LDX   #>ir_function_entry_line
      JMP   emit_text
@fast_result:
      INC   result_fast
      LDA   #0
      RTS
@function_store:
      LDA   #<ir_function_store_wide
      LDX   #>ir_function_store_wide
      JSR   line_equals
      BNE   @byte_function_store
      LDA   result_fast
      long_beq @raw
      LDA   #<fast_result_store_wide
      LDX   #>fast_result_store_wide
      JMP   emit_text
@byte_function_store:
      LDA   #<ir_function_store
      LDX   #>ir_function_store
      JSR   line_equals
      BNE   @function_end
      LDA   result_fast
      long_beq @raw
      LDA   #<fast_result_store
      LDX   #>fast_result_store
      JMP   emit_text
@function_end:
      LDA   #<ir_routine_end
      LDX   #>ir_routine_end
      JSR   line_starts
      BNE   @byte
      LDA   line_buf+5
      CMP   #'F'
      BEQ   @fast_byte_end
      CMP   #'W'
      long_bne @raw
      LDA   result_fast
      long_beq @raw
      STZ   result_fast
      LDA   #<ir_fast_function_end_wide
      LDX   #>ir_fast_function_end_wide
      JMP   emit_text
@fast_byte_end:
      LDA   result_fast
      BEQ   @raw
      STZ   result_fast
      LDA   #<ir_fast_function_end
      LDX   #>ir_fast_function_end
      JMP   emit_text
@byte:
      LDA   #<ir_load_byte
      LDX   #>ir_load_byte
      JSR   line_starts
      BNE   @word
      LDA   #6
      JSR   capture_symbol
      BNE   @raw
      JSR   save_input_position
      JSR   try_byte_self_update
      BEQ   @done
      JSR   restore_input_position
      JSR   try_zero_branch
      BEQ   @done
      JSR   restore_input_position
      JSR   try_accumulate
      BEQ   @done
      JSR   restore_input_position
      LDA   #<ir_load_byte
      LDX   #>ir_load_byte
      JMP   emit_typed_symbol
@word:
      LDA   #<ir_load_word
      LDX   #>ir_load_word
      JSR   line_starts
      BNE   @raw
      LDA   #6
      JSR   capture_symbol
      BNE   @raw
      JSR   copy_symbol_to_candidate_index
      JSR   save_input_position
      JSR   try_window_access
      BEQ   @done
      JSR   restore_input_position
      JSR   try_word_self_update
      BEQ   @done
      JSR   restore_input_position
      JSR   try_array_cache
      BEQ   @done
      JSR   restore_input_position
      JSR   copy_candidate_index_to_symbol
      LDA   #<ir_load_word
      LDX   #>ir_load_word
      JMP   emit_typed_symbol
@raw:
      JMP   emit_line
@done:
      RTS

; Compiler-published unit globals keep every interface routine in a standalone
; NOBJ. A consuming program drops those markers before reachability analysis,
; so only routines actually called by the application survive O2.
line_is_unit_export:
      LDA   line_len
      CMP   #unit_export_marker_end-unit_export_marker
      BCC   @no
      SEC
      SBC   #unit_export_marker_end-unit_export_marker
      TAX
      LDY   #0
@byte:
      CPY   #unit_export_marker_end-unit_export_marker
      BCS   @yes
      LDA   line_buf,X
      CMP   unit_export_marker,Y
      BNE   @no
      INX
      INY
      BRA   @byte
@yes:
      LDA   #0
      RTS
@no:
      LDA   #1
      RTS

emit_routine_target:
      LDA   #<ir_routine
      LDX   #>ir_routine
      JSR   emit_text
      BNE   @done
      LDA   routine_type
      JSR   emit_byte
      BNE   @done
      LDA   #' '
      JSR   emit_byte
      BNE   @done
      JSR   emit_target
      BNE   @done
      LDA   #$0A
      JMP   emit_byte
@done:
      RTS

copy_symbol_to_candidate_index:
      LDA   symbol_len
      STA   candidate_index_len
      LDY   #0
@byte:
      CPY   candidate_index_len
      BEQ   @done
      LDA   symbol_buf,Y
      STA   candidate_index_buf,Y
      INY
      BRA   @byte
@done:
      RTS

copy_candidate_index_to_symbol:
      LDA   candidate_index_len
      STA   symbol_len
      LDY   #0
@byte:
      CPY   symbol_len
      BEQ   @done
      LDA   candidate_index_buf,Y
      STA   symbol_buf,Y
      INY
      BRA   @byte
@done:
      RTS

copy_symbol_to_candidate_array:
      LDA   symbol_len
      STA   candidate_array_len
      LDY   #0
@byte:
      CPY   candidate_array_len
      BEQ   @done
      LDA   symbol_buf,Y
      STA   candidate_array_buf,Y
      INY
      BRA   @byte
@done:
      RTS

candidate_matches_cache:
      LDA   candidate_index_len
      CMP   cache_index_len
      BNE   @no
      LDA   candidate_array_len
      CMP   cache_array_len
      BNE   @no
      LDY   #0
@index:
      CPY   candidate_index_len
      BEQ   @array_start
      LDA   candidate_index_buf,Y
      CMP   cache_index_buf,Y
      BNE   @no
      INY
      BRA   @index
@array_start:
      LDY   #0
@array:
      CPY   candidate_array_len
      BEQ   @yes
      LDA   candidate_array_buf,Y
      CMP   cache_array_buf,Y
      BNE   @no
      INY
      BRA   @array
@yes:
      LDA   #0
      RTS
@no:
      LDA   #1
      RTS

save_candidate_cache:
      LDA   candidate_index_len
      STA   cache_index_len
      LDA   candidate_array_len
      STA   cache_array_len
      LDY   #0
@index:
      CPY   candidate_index_len
      BEQ   @array_start
      LDA   candidate_index_buf,Y
      STA   cache_index_buf,Y
      INY
      BRA   @index
@array_start:
      LDY   #0
@array:
      CPY   candidate_array_len
      BEQ   @done
      LDA   candidate_array_buf,Y
      STA   cache_array_buf,Y
      INY
      BRA   @array
@done:
      INC   cache_valid
      RTS

emit_candidate_index:
      LDY   #0
@byte:
      CPY   candidate_index_len
      BEQ   @done
      LDA   candidate_index_buf,Y
      PHY
      JSR   emit_byte
      PLY
      CMP   #0
      BNE   @done
      INY
      BRA   @byte
@done:
      RTS

emit_candidate_array:
      LDY   #0
@byte:
      CPY   candidate_array_len
      BEQ   @done
      LDA   candidate_array_buf,Y
      PHY
      JSR   emit_byte
      PLY
      CMP   #0
      BNE   @done
      INY
      BRA   @byte
@done:
      RTS

symbol_is_candidate_index:
      LDA   symbol_len
      CMP   candidate_index_len
      BNE   @no
      LDY   #0
@byte:
      CPY   symbol_len
      BEQ   @yes
      LDA   symbol_buf,Y
      CMP   candidate_index_buf,Y
      BNE   @no
      INY
      BRA   @byte
@yes:
      LDA   #0
      RTS
@no:
      LDA   #1
      RTS

symbol_is_candidate_array:
      LDA   symbol_len
      CMP   candidate_array_len
      BNE   @no
      LDY   #0
@byte:
      CPY   symbol_len
      BEQ   @yes
      LDA   symbol_buf,Y
      CMP   candidate_array_buf,Y
      BNE   @no
      INY
      BRA   @byte
@yes:
      LDA   #0
      RTS
@no:
      LDA   #1
      RTS

try_array_cache:
      JSR   read_line
      long_bne @no
      LDA   #<ir_array_get
      LDX   #>ir_array_get
      JSR   line_starts
      long_bne @no
      LDA   #5
      JSR   capture_symbol
      long_bne @no
      JSR   copy_symbol_to_candidate_array
      LDA   cache_valid
      BEQ   @search
      JSR   candidate_matches_cache
      BNE   @search
      LDA   #<cache_load
      LDX   #>cache_load
      JMP   emit_text
@search:
      LDA   input_pos
      STA   rhs_end
      LDA   input_pos+1
      STA   rhs_end+1
      LDA   input_pos+2
      STA   rhs_end+2
@next:
      JSR   read_line
      BNE   @no
      JSR   line_is_cache_barrier
      BNE   @no
      LDA   #<ir_load_word
      LDX   #>ir_load_word
      JSR   line_starts
      BNE   @next
      LDA   #6
      JSR   capture_symbol
      BNE   @next
      JSR   symbol_is_candidate_index
      BNE   @next
      JSR   read_line
      BNE   @no
      LDA   #<ir_array_get
      LDX   #>ir_array_get
      JSR   line_starts
      BNE   @next
      LDA   #5
      JSR   capture_symbol
      BNE   @next
      JSR   symbol_is_candidate_array
      BNE   @next
      LDA   rhs_end
      STA   input_pos
      LDA   rhs_end+1
      STA   input_pos+1
      LDA   rhs_end+2
      STA   input_pos+2
      LDA   #<ir_load_word
      LDX   #>ir_load_word
      JSR   emit_text
      BNE   @done
      JSR   emit_candidate_index
      BNE   @done
      LDA   #$0A
      JSR   emit_byte
      BNE   @done
      LDA   #<ir_array_get
      LDX   #>ir_array_get
      JSR   emit_text
      BNE   @done
      JSR   emit_candidate_array
      BNE   @done
      LDA   #$0A
      JSR   emit_byte
      BNE   @done
      LDA   #<cache_store
      LDX   #>cache_store
      JSR   emit_text
      BNE   @done
      JSR   save_candidate_cache
      LDA   #0
      RTS
@no:
      LDA   #1
@done:
      RTS

line_is_cache_barrier:
      JSR   line_is_inline_asm
      BEQ   @yes
      LDA   line_len
      BEQ   @structured
      TAX
      DEX
      LDA   line_buf,X
      CMP   #':'
      BEQ   @yes
      LDA   line_buf
      CMP   #'B'
      BEQ   @yes
@structured:
      LDA   #<ir_call
      LDX   #>ir_call
      JSR   line_starts
      BEQ   @yes
      LDA   #<ir_routine
      LDX   #>ir_routine
      JSR   line_starts
      BEQ   @yes
      LDA   #<ir_routine_end
      LDX   #>ir_routine_end
      JSR   line_starts
      BEQ   @yes
      LDA   #<ir_array_byte
      LDX   #>ir_array_byte
      JSR   line_starts
      BEQ   @yes
      LDA   #<ir_array_word
      LDX   #>ir_array_word
      JSR   line_starts
      BEQ   @yes
      LDA   #<call_prefix
      LDX   #>call_prefix
      JSR   line_starts
      BEQ   @yes
      LDA   #<jump_prefix
      LDX   #>jump_prefix
      JSR   line_starts
      BEQ   @yes
      LDA   #<return_text
      LDX   #>return_text
      JSR   line_starts
      BEQ   @yes
      LDA   #<named_label_prefix
      LDX   #>named_label_prefix
      JSR   line_starts
      BEQ   @yes
      LDA   #0
      RTS
@yes:
      LDA   #1
      RTS

; NPC indents inline NAS source. Zero means this line is opaque assembly.
line_is_inline_asm:
      LDA   line_len
      BEQ   @no
      LDA   line_buf
      CMP   #' '
      BEQ   @yes
      CMP   #$09
      BEQ   @yes
@no:
      LDA   #1
      RTS
@yes:
      LDA   #0
      RTS

invalidate_array_cache:
      LDA   cache_valid
      BEQ   @done
      JSR   line_is_cache_barrier
      BNE   @clear
      LDA   #<ir_store_byte
      LDX   #>ir_store_byte
      JSR   line_starts
      BEQ   @store
      LDA   #<ir_store_word
      LDX   #>ir_store_word
      JSR   line_starts
      BEQ   @store
      LDA   #<ir_store_extend
      LDX   #>ir_store_extend
      JSR   line_starts
      BNE   @done
@store:
      LDA   line_len
      SEC
      SBC   #6
      CMP   cache_index_len
      BNE   @done
      LDY   #0
@byte:
      CPY   cache_index_len
      BEQ   @clear
      LDA   line_buf+6,Y
      CMP   cache_index_buf,Y
      BNE   @done
      INY
      BRA   @byte
@clear:
      STZ   cache_valid
@done:
      RTS

parse_offset_array:
      LDA   input_pos
      STA   parse_return
      LDA   input_pos+1
      STA   parse_return+1
      LDA   input_pos+2
      STA   parse_return+2
      LDA   #<line_phx
      LDX   #>line_phx
      JSR   expect_line
      long_bne @no
      LDA   #<line_pha
      LDX   #>line_pha
      JSR   expect_line
      long_bne @no
      JSR   read_literal
      long_bne @no
      JSR   immediate_to_byte
      BCC   :+
      JMP   @no
:
      STA   offset_value
      LDA   #<line_sta_nvr0l
      LDX   #>line_sta_nvr0l
      JSR   expect_line
      long_bne @no
      LDA   #<line_stz_nvr0h
      LDX   #>line_stz_nvr0h
      JSR   expect_line
      long_bne @no
      LDA   #<line_pla
      LDX   #>line_pla
      JSR   expect_line
      long_bne @no
      LDA   #<line_plx
      LDX   #>line_plx
      JSR   expect_line
      long_bne @no
      JSR   read_line
      long_bne @no
      LDA   #<ir_sub_word
      LDX   #>ir_sub_word
      JSR   line_equals
      BEQ   @subtract
      LDA   #<ir_add_word
      LDX   #>ir_add_word
      JSR   line_equals
      long_bne @no
      LDA   offset_value
      STA   candidate_offset
      LDA   #1
      STA   candidate_offset+1
      BRA   @array
@subtract:
      SEC
      LDA   #0
      SBC   offset_value
      STA   candidate_offset
      LDA   #1
      SBC   #0
      STA   candidate_offset+1
@array:
      JSR   read_line
      long_bne @no
      LDA   #<ir_array_get
      LDX   #>ir_array_get
      JSR   line_starts
      long_bne @no
      LDA   #5
      JSR   capture_symbol
      BNE   @no
      JSR   copy_symbol_to_candidate_array
      LDA   #0
      RTS
@no:
      LDA   parse_return
      STA   input_pos
      LDA   parse_return+1
      STA   input_pos+1
      LDA   parse_return+2
      STA   input_pos+2
      LDA   #1
      RTS

immediate_to_byte:
      LDA   immediate_hi
      JSR   hex_nibble
      BCS   @bad
      ASL
      ASL
      ASL
      ASL
      STA   byte_value
      LDA   immediate_lo
      JSR   hex_nibble
      BCS   @bad
      ORA   byte_value
      CLC
      RTS
@bad:
      LDA   #1
      RTS

hex_nibble:
      CMP   #'0'
      BCC   @bad
      CMP   #'9'+1
      BCC   @digit
      AND   #$DF
      CMP   #'A'
      BCC   @bad
      CMP   #'F'+1
      BCS   @bad
      SEC
      SBC   #'A'-10
      CLC
      RTS
@digit:
      SEC
      SBC   #'0'
      CLC
      RTS
@bad:
      SEC
      RTS

copy_candidate_to_window:
      LDA   candidate_index_len
      STA   window_index_len
      LDA   candidate_array_len
      STA   window_array_len
      LDY   #0
@index:
      CPY   window_index_len
      BEQ   @array_start
      LDA   candidate_index_buf,Y
      STA   window_index_buf,Y
      INY
      BRA   @index
@array_start:
      LDY   #0
@array:
      CPY   window_array_len
      BEQ   @done
      LDA   candidate_array_buf,Y
      STA   window_array_buf,Y
      INY
      BRA   @array
@done:
      RTS

candidate_matches_window:
      LDA   candidate_index_len
      CMP   window_index_len
      BNE   @no
      LDA   candidate_array_len
      CMP   window_array_len
      BNE   @no
      LDY   #0
@index:
      CPY   window_index_len
      BEQ   @array_start
      LDA   candidate_index_buf,Y
      CMP   window_index_buf,Y
      BNE   @no
      INY
      BRA   @index
@array_start:
      LDY   #0
@array:
      CPY   window_array_len
      BEQ   @yes
      LDA   candidate_array_buf,Y
      CMP   window_array_buf,Y
      BNE   @no
      INY
      BRA   @array
@yes:
      LDA   #0
      RTS
@no:
      LDA   #1
      RTS

analyze_array_window:
      LDA   input_pos
      STA   scan_return
      LDA   input_pos+1
      STA   scan_return+1
      LDA   input_pos+2
      STA   scan_return+2
      STZ   window_active
      STZ   window_count
@scan:
      JSR   read_line
      long_bne @restore
      JSR   line_is_inline_asm
      long_beq @restore
      LDA   #<ir_routine_end
      LDX   #>ir_routine_end
      JSR   line_starts
      long_beq @candidate_done
      LDA   #<ir_load_word
      LDX   #>ir_load_word
      JSR   line_starts
      BNE   @scan
      LDA   #6
      JSR   capture_symbol
      BNE   @scan
      JSR   copy_symbol_to_candidate_index
      JSR   parse_offset_array
      BNE   @scan
      LDA   window_count
      BNE   @existing
      JSR   copy_candidate_to_window
      LDA   candidate_offset
      STA   window_min
      STA   window_max
      LDA   candidate_offset+1
      STA   window_min+1
      STA   window_max+1
      INC   window_count
      BRA   @scan
@existing:
      JSR   candidate_matches_window
      BNE   @scan
      INC   window_count
      LDA   candidate_offset+1
      CMP   window_min+1
      BCC   @new_min
      BNE   @max
      LDA   candidate_offset
      CMP   window_min
      BCC   @new_min
@max:
      LDA   candidate_offset+1
      CMP   window_max+1
      BCC   @scan
      BNE   @new_max
      LDA   candidate_offset
      CMP   window_max
      BCC   @scan
@new_max:
      LDA   candidate_offset
      STA   window_max
      LDA   candidate_offset+1
      STA   window_max+1
      JMP   @scan
@new_min:
      LDA   candidate_offset
      STA   window_min
      LDA   candidate_offset+1
      STA   window_min+1
      JMP   @scan
@candidate_done:
      LDA   window_count
      CMP   #2
      BCC   @restore
      SEC
      LDA   window_max
      SBC   window_min
      LDA   window_max+1
      SBC   window_min+1
      BNE   @restore
      LDA   scan_return
      STA   input_pos
      LDA   scan_return+1
      STA   input_pos+1
      LDA   scan_return+2
      STA   input_pos+2
@validate:
      JSR   read_line
      BNE   @restore
      LDA   #<ir_routine_end
      LDX   #>ir_routine_end
      JSR   line_starts
      BEQ   @valid
      LDA   #<ir_call
      LDX   #>ir_call
      JSR   line_starts
      BEQ   @restore
      LDA   #<call_prefix
      LDX   #>call_prefix
      JSR   line_starts
      BEQ   @restore
      JSR   line_mutates_window
      BNE   @restore
      BRA   @validate
@valid:
      INC   window_active
@restore:
      LDA   scan_return
      STA   input_pos
      LDA   scan_return+1
      STA   input_pos+1
      LDA   scan_return+2
      STA   input_pos+2
      RTS

line_mutates_window:
      LDA   #<ir_store_byte
      LDX   #>ir_store_byte
      JSR   line_starts
      BEQ   @index
      LDA   #<ir_store_word
      LDX   #>ir_store_word
      JSR   line_starts
      BEQ   @index
      LDA   #<ir_store_extend
      LDX   #>ir_store_extend
      JSR   line_starts
      BNE   @array_store
@index:
      LDA   #6
      JMP   operand_is_window_index
@array_store:
      LDA   #<ir_array_byte
      LDX   #>ir_array_byte
      JSR   line_starts
      BEQ   @array
      LDA   #<ir_array_word
      LDX   #>ir_array_word
      JSR   line_starts
      BNE   @no
@array:
      LDA   #5
      JMP   operand_is_window_array
@no:
      LDA   #0
      RTS

operand_is_window_index:
      STA   operand_pos
      LDA   line_len
      SEC
      SBC   operand_pos
      CMP   window_index_len
      BNE   @no
      LDY   #0
@byte:
      CPY   window_index_len
      BEQ   @yes
      LDX   operand_pos
      LDA   line_buf,X
      CMP   window_index_buf,Y
      BNE   @no
      INC   operand_pos
      INY
      BRA   @byte
@yes:
      LDA   #1
      RTS
@no:
      LDA   #0
      RTS

operand_is_window_array:
      STA   operand_pos
      LDA   line_len
      SEC
      SBC   operand_pos
      CMP   window_array_len
      BNE   @no
      LDY   #0
@byte:
      CPY   window_array_len
      BEQ   @yes
      LDX   operand_pos
      LDA   line_buf,X
      CMP   window_array_buf,Y
      BNE   @no
      INC   operand_pos
      INY
      BRA   @byte
@yes:
      LDA   #1
      RTS
@no:
      LDA   #0
      RTS

try_window_access:
      LDA   window_active
      BEQ   @no
      JSR   parse_offset_array
      BNE   @no
      JSR   candidate_matches_window
      BNE   @no
      SEC
      LDA   candidate_offset
      SBC   window_min
      STA   offset_value
      LDA   candidate_offset+1
      SBC   window_min+1
      BNE   @no
      LDA   #<ir_window_load
      LDX   #>ir_window_load
      JSR   emit_text
      BNE   @done
      LDA   offset_value
      JSR   emit_hex_value
      BNE   @done
      LDA   #$0A
      JMP   emit_byte
@no:
      LDA   #1
@done:
      RTS

emit_window_pointer:
      LDA   #<ir_window_pointer
      LDX   #>ir_window_pointer
      JSR   emit_text
      BNE   @done
      LDA   window_min+1
      BEQ   @negative
      LDA   #'+'
      JSR   emit_byte
      BNE   @done
      LDA   window_min
      BRA   @magnitude
@negative:
      LDA   #'-'
      JSR   emit_byte
      BNE   @done
      SEC
      LDA   #0
      SBC   window_min
@magnitude:
      JSR   emit_hex_value
      BNE   @done
      LDA   #' '
      JSR   emit_byte
      BNE   @done
      LDY   #0
@index:
      CPY   window_index_len
      BEQ   @array_space
      LDA   window_index_buf,Y
      PHY
      JSR   emit_byte
      PLY
      CMP   #0
      BNE   @done
      INY
      BRA   @index
@array_space:
      LDA   #' '
      JSR   emit_byte
      BNE   @done
      LDY   #0
@array:
      CPY   window_array_len
      BEQ   @lf
      LDA   window_array_buf,Y
      PHY
      JSR   emit_byte
      PLY
      CMP   #0
      BNE   @done
      INY
      BRA   @array
@lf:
      LDA   #$0A
      JMP   emit_byte
@done:
      RTS

emit_hex_value:
      STA   hex_value
      LSR
      LSR
      LSR
      LSR
      TAX
      LDA   hex_digits,X
      JSR   emit_byte
      BNE   @done
      LDA   hex_value
      AND   #$0F
      TAX
      LDA   hex_digits,X
      JMP   emit_byte
@done:
      RTS

analyze_function_result:
      JSR   save_input_position
      STZ   result_seen
@line:
      JSR   read_line
      BNE   @unsafe
      JSR   line_is_inline_asm
      BEQ   @unsafe
      LDA   #<ir_function_store
      LDX   #>ir_function_store
      JSR   line_starts
      BNE   @call
      INC   result_seen
      BRA   @line
@call:
      LDA   result_seen
      BEQ   @end
      LDA   #<ir_call
      LDX   #>ir_call
      JSR   line_starts
      BEQ   @unsafe
      LDA   #<call_prefix
      LDX   #>call_prefix
      JSR   line_starts
      BEQ   @unsafe
@end:
      LDA   #<ir_routine_end
      LDX   #>ir_routine_end
      JSR   line_starts
      BNE   @line
      LDA   result_seen
      BEQ   @unsafe
      JSR   restore_input_position
      LDA   #0
      RTS
@unsafe:
      JSR   restore_input_position
      LDA   #1
      RTS

; A is the first operand byte in line_buf. Symbols are exact line tails.
capture_symbol:
      STA   operand_pos
      LDA   line_len
      SEC
      SBC   operand_pos
      BEQ   @fail
      CMP   #64
      BCS   @fail
      STA   symbol_len
      LDY   #0
@copy:
      CPY   symbol_len
      BEQ   @ok
      LDX   operand_pos
      LDA   line_buf,X
      STA   symbol_buf,Y
      INC   operand_pos
      INY
      BRA   @copy
@ok:
      LDA   #0
      RTS
@fail:
      LDA   #1
      RTS

; A is the operand offset. Return zero when the current line tail is symbol_buf.
operand_is_symbol:
      STA   operand_pos
      LDA   line_len
      SEC
      SBC   operand_pos
      CMP   symbol_len
      BNE   @no
      LDY   #0
@compare:
      CPY   symbol_len
      BEQ   @yes
      LDX   operand_pos
      LDA   line_buf,X
      CMP   symbol_buf,Y
      BNE   @no
      INC   operand_pos
      INY
      BRA   @compare
@yes:
      LDA   #0
      RTS
@no:
      LDA   #1
      RTS

emit_symbol:
      LDY   #0
@byte:
      CPY   symbol_len
      BEQ   @done
      LDA   symbol_buf,Y
      PHY
      JSR   emit_byte
      PLY
      CMP   #0
      BNE   @done
      INY
      BRA   @byte
@done:
      RTS

; A/X is a typed prefix; append symbol_buf and LF.
emit_typed_symbol:
      JSR   emit_text
      BNE   @done
      JSR   emit_symbol
      BNE   @done
      LDA   #$0A
      JMP   emit_byte
@done:
      RTS

try_byte_self_update:
      LDA   #<line_pha
      LDX   #>line_pha
      JSR   expect_line
      long_bne @no
      LDA   #<line_literal_one
      LDX   #>line_literal_one
      JSR   expect_line
      BNE   @no
      JSR   read_line
      long_bne @no
      LDA   #<ir_add_byte
      LDX   #>ir_add_byte
      JSR   line_equals
      BEQ   @increment
      LDA   #<ir_sub_byte
      LDX   #>ir_sub_byte
      JSR   line_equals
      BNE   @no
      LDA   #'D'
      BRA   @store
@increment:
      LDA   #'I'
@store:
      STA   compare_op
      JSR   read_line
      BNE   @no
      LDA   #<ir_store_byte
      LDX   #>ir_store_byte
      JSR   line_starts
      BNE   @no
      LDA   #6
      JSR   operand_is_symbol
      BNE   @no
      LDA   compare_op
      CMP   #'I'
      BNE   :+
      LDA   #<increment_prefix
      LDX   #>increment_prefix
      BRA   :++
:
      LDA   #<decrement_prefix
      LDX   #>decrement_prefix
:
      JSR   emit_text
      BNE   @done
      JSR   emit_symbol
      BNE   @done
      LDA   #$0A
      JMP   emit_byte
@no:
      LDA   #1
@done:
      RTS

try_word_self_update:
      LDA   #<line_phx
      LDX   #>line_phx
      JSR   expect_line
      long_bne @no
      LDA   #<line_pha
      LDX   #>line_pha
      JSR   expect_line
      long_bne @no
      LDA   #<line_literal_one
      LDX   #>line_literal_one
      JSR   expect_line
      long_bne @no
      LDA   #<line_sta_nvr0l
      LDX   #>line_sta_nvr0l
      JSR   expect_line
      long_bne @no
      LDA   #<line_stz_nvr0h
      LDX   #>line_stz_nvr0h
      JSR   expect_line
      long_bne @no
      LDA   #<line_pla
      LDX   #>line_pla
      JSR   expect_line
      long_bne @no
      LDA   #<line_plx
      LDX   #>line_plx
      JSR   expect_line
      long_bne @no
      JSR   read_line
      long_bne @no
      LDA   #<ir_add_word
      LDX   #>ir_add_word
      JSR   line_equals
      BEQ   @increment
      LDA   #<ir_sub_word
      LDX   #>ir_sub_word
      JSR   line_equals
      long_bne @no
      LDA   #'D'
      BRA   @store
@increment:
      LDA   #'I'
@store:
      STA   compare_op
      JSR   read_line
      long_bne @no
      LDA   #<ir_store_word
      LDX   #>ir_store_word
      JSR   line_starts
      long_bne @no
      LDA   #6
      JSR   operand_is_symbol
      long_bne @no
      LDA   compare_op
      CMP   #'I'
      BNE   @decrement
      LDA   #<increment_prefix
      LDX   #>increment_prefix
      JSR   emit_text
      BNE   @done
      JSR   emit_symbol
      BNE   @done
      LDA   #<increment_word_middle
      LDX   #>increment_word_middle
      JSR   emit_text
      BNE   @done
      JSR   emit_symbol
      BNE   @done
      LDA   #<plus_one_label_suffix
      LDX   #>plus_one_label_suffix
      JMP   emit_text
@decrement:
      LDA   #<load_byte_prefix
      LDX   #>load_byte_prefix
      JSR   emit_text
      BNE   @done
      JSR   emit_symbol
      BNE   @done
      LDA   #<decrement_word_middle
      LDX   #>decrement_word_middle
      JSR   emit_text
      BNE   @done
      JSR   emit_symbol
      BNE   @done
      LDA   #<decrement_word_tail
      LDX   #>decrement_word_tail
      JSR   emit_text
      BNE   @done
      JSR   emit_symbol
      BNE   @done
      LDA   #$0A
      JMP   emit_byte
@no:
      LDA   #1
@done:
      RTS

try_zero_branch:
      LDA   #<line_pha
      LDX   #>line_pha
      JSR   expect_line
      BNE   @no
      LDA   #<line_literal_zero
      LDX   #>line_literal_zero
      JSR   expect_line
      BNE   @no
      LDA   #<ir_compare_byte
      LDX   #>ir_compare_byte
      JSR   expect_line
      BNE   @no
      JSR   read_line
      BNE   @no
      LDA   #<ir_compare
      LDX   #>ir_compare
      JSR   line_starts
      BNE   @no
      LDA   line_buf+5
      STA   compare_op
      JSR   read_line
      BNE   @no
      LDA   #<ir_branch
      LDX   #>ir_branch
      JSR   line_starts
      BNE   @no
      LDA   #<ir_zero_branch
      LDX   #>ir_zero_branch
      JSR   emit_text
      BNE   @done
      LDA   compare_op
      JSR   emit_byte
      BNE   @done
      LDA   #' '
      JSR   emit_byte
      BNE   @done
      JSR   emit_symbol
      BNE   @done
      LDA   #' '
      JSR   emit_byte
      BNE   @done
      LDA   #5
      STA   operand_pos
      JSR   emit_operand
      BNE   @done
      LDA   #$0A
      JMP   emit_byte
@no:
      LDA   #1
@done:
      RTS

try_accumulate:
      LDA   symbol_len
      STA   target_len
      LDY   #0
@save_symbol:
      CPY   target_len
      BEQ   @saved_symbol
      LDA   symbol_buf,Y
      STA   target_buf,Y
      INY
      BRA   @save_symbol
@saved_symbol:
      LDA   #<line_pha
      LDX   #>line_pha
      JSR   expect_line
      long_bne @no
      LDA   input_pos
      STA   rhs_start
      LDA   input_pos+1
      STA   rhs_start+1
      LDA   input_pos+2
      STA   rhs_start+2
@scan:
      LDA   input_pos
      STA   rhs_stop
      LDA   input_pos+1
      STA   rhs_stop+1
      LDA   input_pos+2
      STA   rhs_stop+2
      JSR   read_line
      long_bne @no
      LDA   #<ir_add_byte
      LDX   #>ir_add_byte
      JSR   line_equals
      long_beq @possible_end
      JSR   line_is_pure_expression
      BEQ   @scan
      JMP   @no
@possible_end:
      JSR   read_line
      long_bne @no
      LDA   #<ir_store_byte
      LDX   #>ir_store_byte
      JSR   line_starts
      long_bne @no
      LDA   #6
      JSR   operand_is_symbol
      long_bne @no
      LDA   input_pos
      STA   rhs_end
      LDA   input_pos+1
      STA   rhs_end+1
      LDA   input_pos+2
      STA   rhs_end+2
      LDA   rhs_start
      STA   input_pos
      LDA   rhs_start+1
      STA   input_pos+1
      LDA   rhs_start+2
      STA   input_pos+2
@copy:
      LDA   input_pos+2
      CMP   rhs_stop+2
      BNE   @copy_line
      LDA   input_pos+1
      CMP   rhs_stop+1
      BNE   @copy_line
      LDA   input_pos
      CMP   rhs_stop
      BEQ   @tail
@copy_line:
      JSR   read_line
      BNE   @no
      JSR   process_typed
      BNE   @done
      BRA   @copy
@tail:
      LDA   rhs_end
      STA   input_pos
      LDA   rhs_end+1
      STA   input_pos+1
      LDA   rhs_end+2
      STA   input_pos+2
      LDA   target_len
      STA   symbol_len
      LDY   #0
@restore_symbol:
      CPY   symbol_len
      BEQ   @emit_tail
      LDA   target_buf,Y
      STA   symbol_buf,Y
      INY
      BRA   @restore_symbol
@emit_tail:
      LDA   #<ir_accumulate_byte
      LDX   #>ir_accumulate_byte
      JMP   emit_typed_symbol
@no:
      LDA   #1
@done:
      RTS

; Only typed loads/arithmetic and their known register shuffles are movable.
line_is_pure_expression:
      LDA   line_len
      BEQ   @yes
      LDA   line_buf
      CMP   #';'
      BEQ   @yes
      LDA   #<ir_load_byte
      LDX   #>ir_load_byte
      JSR   line_starts
      BEQ   @yes
      LDA   #<ir_load_word
      LDX   #>ir_load_word
      JSR   line_starts
      BEQ   @yes
      LDA   #<ir_array_get
      LDX   #>ir_array_get
      JSR   line_starts
      BEQ   @yes
      LDA   #<ir_add_word
      LDX   #>ir_add_word
      JSR   line_equals
      BEQ   @yes
      LDA   #<ir_sub_word
      LDX   #>ir_sub_word
      JSR   line_equals
      BEQ   @yes
      LDA   #<literal_prefix
      LDX   #>literal_prefix
      JSR   line_starts
      BEQ   @yes
      LDA   #<literal_x_prefix
      LDX   #>literal_x_prefix
      JSR   line_starts
      BEQ   @yes
      LDX   #0
@known:
      LDA   pure_line_lo,X
      STA   text_ptr
      LDA   pure_line_hi,X
      STA   text_ptr+1
      LDA   text_ptr
      ORA   text_ptr+1
      BEQ   @no
      PHX
      LDA   text_ptr
      LDX   text_ptr+1
      JSR   line_equals
      PLX
      CMP   #0
      BEQ   @yes
      INX
      BRA   @known
@yes:
      LDA   #0
      RTS
@no:
      LDA   #1
      RTS

; Build one whole-file routine/call index for each large inlining pass. The
; existing transformer then performs constant-time target analysis instead of
; rescanning a multi-page file for every routine and call site.
large_analyze_routines:
      STZ   large_routine_count
      STZ   input_pos
      STZ   input_pos+1
      STZ   input_pos+2
@collect:
      JSR   read_line
      BNE   @collected
      LDA   #<ir_routine
      LDX   #>ir_routine
      JSR   line_starts
      BNE   @collect
      LDA   #7
      JSR   capture_target
      long_bne @fail
      JSR   large_routine_add_target
      long_bne @fail
      BRA   @collect
@collected:
      LDA   opt_error
      long_bne @fail
      STZ   input_pos
      STZ   input_pos+1
      STZ   input_pos+2
      LDA   #$FF
      STA   large_routine_current
@scan:
      JSR   read_line
      long_bne @done
      LDA   #<ir_routine
      LDX   #>ir_routine
      JSR   line_starts
      BNE   @routine_end
      LDA   #7
      JSR   capture_target
      long_bne @fail
      JSR   large_routine_find_target
      long_bcc @fail
      STX   large_routine_current
      BRA   @scan
@routine_end:
      LDA   #<ir_routine_end
      LDX   #>ir_routine_end
      JSR   line_starts
      BNE   @opaque
      LDA   #$FF
      STA   large_routine_current
      BRA   @scan
@opaque:
      LDA   #<frame_enter_line
      LDX   #>frame_enter_line
      JSR   line_equals
      BNE   @not_frame
      LDY   large_routine_current
      BMI   @scan
      LDA   #0
      STA   large_routine_leaf,Y
      JMP   @scan
@not_frame:
      JSR   line_is_inline_asm
      BEQ   @opaque_line
      JSR   line_is_unit_export
      BEQ   @opaque_line
      LDA   #<ir_call
      LDX   #>ir_call
      JSR   line_starts
      BNE   @scan
      LDA   #5
      JSR   capture_target
      BNE   @fail
      JSR   large_routine_find_target
      BCC   @scan
      LDA   large_routine_calls,X
      CMP   #2
      BCS   :+
      INC   large_routine_calls,X
:     LDY   large_routine_current
      BMI   @scan
      LDA   #0
      STA   large_routine_leaf,Y
      JMP   @scan
@opaque_line:
      LDY   large_routine_current
      BMI   :+
      LDA   #0
      STA   large_routine_leaf,Y
:     JSR   large_mark_opaque_references
      JMP   @scan
@done:
      LDA   opt_error
      BNE   @fail
      STZ   input_pos
      STZ   input_pos+1
      STZ   input_pos+2
      LDA   #0
      RTS
@fail:
      LDA   #1
      RTS

large_routine_add_target:
      LDX   large_routine_count
      CPX   #INLINE_ROUTINE_CAP
      BCS   @fail
      STX   large_routine_index
      LDA   target_len
      STA   large_routine_name_len,X
      LDA   line_buf+5
      STA   large_routine_type,X
      LDA   input_pos
      STA   large_routine_body_l,X
      LDA   input_pos+1
      STA   large_routine_body_m,X
      LDA   input_pos+2
      STA   large_routine_body_h,X
      STZ   large_routine_calls,X
      LDA   #1
      STA   large_routine_leaf,X
      JSR   large_routine_name_pointer
      LDY   #0
@name:
      CPY   target_len
      BEQ   @done
      LDA   target_buf,Y
      STA   (text_ptr),Y
      INY
      BRA   @name
@done:
      INC   large_routine_count
      LDA   #0
      RTS
@fail:
      LDA   #6
      STA   opt_error
      LDA   #1
      RTS

; C set and X=index when target_buf names a known local routine.
large_routine_find_target:
      LDX   #0
@candidate:
      CPX   large_routine_count
      BCS   @missing
      LDA   large_routine_name_len,X
      CMP   target_len
      BNE   @next
      STX   large_routine_index
      JSR   large_routine_name_pointer
      LDY   #0
@byte:
      CPY   target_len
      BEQ   @found
      LDA   (text_ptr),Y
      CMP   target_buf,Y
      BNE   @next_saved
      INY
      BRA   @byte
@next_saved:
      LDX   large_routine_index
@next:
      INX
      BRA   @candidate
@found:
      LDX   large_routine_index
      SEC
      RTS
@missing:
      CLC
      RTS

large_routine_name_pointer:
      STX   large_routine_index
      TXA
      STA   text_ptr
      STZ   text_ptr+1
      LDY   #6
@times64:
      ASL   text_ptr
      ROL   text_ptr+1
      DEY
      BNE   @times64
      CLC
      LDA   text_ptr
      ADC   #<large_routine_names
      STA   text_ptr
      LDA   text_ptr+1
      ADC   #>large_routine_names
      STA   text_ptr+1
      LDX   large_routine_index
      RTS

large_copy_routine_to_target:
      STX   large_routine_index
      LDA   large_routine_name_len,X
      STA   target_len
      JSR   large_routine_name_pointer
      LDY   #0
@byte:
      CPY   target_len
      BEQ   @done
      LDA   (text_ptr),Y
      STA   target_buf,Y
      INY
      BRA   @byte
@done:
      LDX   large_routine_index
      RTS

large_mark_opaque_references:
      LDX   #0
@routine:
      CPX   large_routine_count
      BCS   @done
      JSR   large_copy_routine_to_target
      JSR   inline_asm_references_target
      BNE   @next
      LDX   large_routine_index
      LDA   #$FF
      STA   large_routine_calls,X
@next:
      LDX   large_routine_index
      INX
      BRA   @routine
@done:
      RTS

large_analyze_inline_target:
      LDA   #$FF
      STA   call_count
      JSR   large_routine_find_target
      BCC   @no
      LDA   large_routine_calls,X
      STA   call_count
      LDA   large_routine_type,X
      STA   routine_type
      LDA   large_routine_body_l,X
      STA   routine_body
      LDA   large_routine_body_m,X
      STA   routine_body+1
      LDA   large_routine_body_h,X
      STA   routine_body+2
      LDA   large_routine_leaf,X
      BEQ   @no
      LDA   call_count
      CMP   #1
      BNE   @no
      LDA   #0
      RTS
@no:
      LDA   #1
      RTS

process_inline:
      LDA   #<ir_routine
      LDX   #>ir_routine
      JSR   line_starts
      BNE   @call
      LDA   #7
      JSR   capture_target
      long_bne @raw
      JSR   analyze_inline_target
      BEQ   @skip
      LDA   pass_id
      CMP   #4
      BNE   @emit_routine
      LDA   call_count
      BNE   @emit_routine
@skip:
      JSR   read_line
      long_bne @bad
      LDA   #<ir_routine_end
      LDX   #>ir_routine_end
      JSR   line_starts
      BNE   @skip
      LDA   #0
      RTS
@emit_routine:
      LDA   #<ir_routine
      LDX   #>ir_routine
      JSR   emit_text
      long_bne @done
      LDA   routine_type
      JSR   emit_byte
      long_bne @done
      LDA   #' '
      JSR   emit_byte
      long_bne @done
      JSR   emit_target
      long_bne @done
      LDA   #$0A
      JMP   emit_byte
@call:
      LDA   #<ir_call
      LDX   #>ir_call
      JSR   line_starts
      long_bne @raw
      LDA   #5
      JSR   capture_target
      long_bne @raw
      LDA   input_pos
      STA   rhs_end
      LDA   input_pos+1
      STA   rhs_end+1
      LDA   input_pos+2
      STA   rhs_end+2
      JSR   analyze_inline_target
      BNE   @emit_call
      LDA   routine_body
      STA   input_pos
      LDA   routine_body+1
      STA   input_pos+1
      LDA   routine_body+2
      STA   input_pos+2
@body:
      JSR   read_line
      BNE   @bad
      LDA   #<ir_routine_end
      LDX   #>ir_routine_end
      JSR   line_starts
      BEQ   @inline_end
      JSR   emit_line
      BNE   @done
      BRA   @body
@inline_end:
      LDA   line_buf+5
      STA   routine_type
      LDA   rhs_end
      STA   input_pos
      LDA   rhs_end+1
      STA   input_pos+1
      LDA   rhs_end+2
      STA   input_pos+2
      LDA   #<ir_inline_end
      LDX   #>ir_inline_end
      JSR   emit_text
      BNE   @done
      LDA   routine_type
      JSR   emit_byte
      BNE   @done
      LDA   #$0A
      JMP   emit_byte
@emit_call:
      LDA   #<ir_call
      LDX   #>ir_call
      JSR   emit_text
      BNE   @done
      JSR   emit_target
      BNE   @done
      LDA   #$0A
      JMP   emit_byte
@raw:
      JMP   emit_line
@bad:
      INC   opt_error
      LDA   #1
@done:
      RTS

capture_target:
      JSR   capture_symbol
      BNE   @done
      LDA   symbol_len
      STA   target_len
      LDY   #0
@copy:
      CPY   target_len
      BEQ   @ok
      LDA   symbol_buf,Y
      STA   target_buf,Y
      INY
      BRA   @copy
@ok:
      LDA   #0
@done:
      RTS

emit_target:
      LDY   #0
@byte:
      CPY   target_len
      BEQ   @done
      LDA   target_buf,Y
      PHY
      JSR   emit_byte
      PLY
      CMP   #0
      BNE   @done
      INY
      BRA   @byte
@done:
      RTS

; A is the target operand offset in the current line.
operand_is_target:
      STA   operand_pos
      LDA   line_len
      SEC
      SBC   operand_pos
      CMP   target_len
      BNE   @no
      LDY   #0
@byte:
      CPY   target_len
      BEQ   @yes
      LDX   operand_pos
      LDA   line_buf,X
      CMP   target_buf,Y
      BNE   @no
      INC   operand_pos
      INY
      BRA   @byte
@yes:
      LDA   #0
      RTS
@no:
      LDA   #1
      RTS

analyze_inline_target:
      LDA   large_mode
      BEQ   @small
      JMP   large_analyze_inline_target
@small:
      LDA   #$FF
      STA   call_count
      LDA   input_pos
      STA   scan_return
      LDA   input_pos+1
      STA   scan_return+1
      LDA   input_pos+2
      STA   scan_return+2
      STZ   input_pos
      STZ   input_pos+1
      STZ   input_pos+2
@find:
      JSR   read_line
      long_bne @no
      LDA   #<ir_routine
      LDX   #>ir_routine
      JSR   line_starts
      BNE   @find
      LDA   #7
      JSR   operand_is_target
      BNE   @find
      LDA   line_buf+5
      STA   routine_type
      LDA   input_pos
      STA   routine_body
      LDA   input_pos+1
      STA   routine_body+1
      LDA   input_pos+2
      STA   routine_body+2
@leaf:
      JSR   read_line
      BNE   @no
      LDA   #<frame_enter_line
      LDX   #>frame_enter_line
      JSR   line_equals
      BEQ   @no
      JSR   line_is_inline_asm
      BEQ   @no
      LDA   #<ir_routine_end
      LDX   #>ir_routine_end
      JSR   line_starts
      BEQ   @count
      LDA   #<ir_call
      LDX   #>ir_call
      JSR   line_starts
      BNE   @leaf
      LDA   #5
      JSR   capture_symbol
      BNE   @no
      JSR   symbol_is_local_routine
      BEQ   @no
      BRA   @leaf
@count:
      STZ   call_count
      STZ   input_pos
      STZ   input_pos+1
      STZ   input_pos+2
@call:
      JSR   read_line
      BNE   @counted
      JSR   inline_asm_references_target
      BEQ   @no
      LDA   #<ir_call
      LDX   #>ir_call
      JSR   line_starts
      BNE   @call
      LDA   #5
      JSR   operand_is_target
      BNE   @call
      INC   call_count
      LDA   call_count
      CMP   #2
      BCC   @call
@counted:
      LDA   call_count
      CMP   #1
      BNE   @no
      JSR   restore_scan_return
      LDA   #0
      RTS
@no:
      JSR   restore_scan_return
      LDA   #1
      RTS

; Zero when an opaque NAS line or standalone-unit export marker contains
; target_buf as a case-insensitive symbol token. Either keeps it callable.
inline_asm_references_target:
      JSR   line_is_unit_export
      BEQ   @candidate_start
      JSR   line_is_inline_asm
      BNE   @no
@candidate_start:
      STZ   operand_pos
@candidate:
      LDX   operand_pos
      CPX   line_len
      BCS   @no
      CPX   #0
      BEQ   @compare_start
      DEX
      LDA   line_buf,X
      JSR   inline_symbol_char
      BCS   @next
@compare_start:
      LDX   operand_pos
      LDY   #0
@compare:
      CPY   target_len
      BEQ   @after
      CPX   line_len
      BCS   @next
      LDA   line_buf,X
      JSR   inline_upper
      CMP   target_buf,Y
      BNE   @next
      INX
      INY
      BRA   @compare
@after:
      CPX   line_len
      BEQ   @yes
      LDA   line_buf,X
      JSR   inline_symbol_char
      BCC   @yes
@next:
      INC   operand_pos
      BRA   @candidate
@yes:
      LDA   #$FF
      STA   call_count
      LDA   #0
      RTS
@no:
      LDA   #1
      RTS

inline_symbol_char:
      JSR   inline_upper
      CMP   #'A'
      BCC   @digit
      CMP   #'Z'+1
      BCC   @yes
@digit:
      CMP   #'0'
      BCC   @special
      CMP   #'9'+1
      BCC   @yes
@special:
      CMP   #'_'
      BEQ   @yes
      CMP   #'@'
      BEQ   @yes
      CMP   #'.'
      BEQ   @yes
      CLC
      RTS
@yes:
      SEC
      RTS

inline_upper:
      CMP   #'a'
      BCC   @done
      CMP   #'z'+1
      BCS   @done
      AND   #$DF
@done:
      RTS

; symbol_buf is a call operand. Zero means a matching local routine exists.
symbol_is_local_routine:
      LDA   input_pos
      STA   nested_return
      LDA   input_pos+1
      STA   nested_return+1
      LDA   input_pos+2
      STA   nested_return+2
      STZ   input_pos
      STZ   input_pos+1
      STZ   input_pos+2
@line:
      JSR   read_line
      BNE   @missing
      LDA   #<ir_routine
      LDX   #>ir_routine
      JSR   line_starts
      BNE   @line
      LDA   #7
      JSR   operand_is_symbol
      BNE   @line
      LDA   #0
      BRA   @restore
@missing:
      LDA   #1
@restore:
      PHA
      LDA   nested_return
      STA   input_pos
      LDA   nested_return+1
      STA   input_pos+1
      LDA   nested_return+2
      STA   input_pos+2
      PLA
      RTS

restore_scan_return:
      LDA   scan_return
      STA   input_pos
      LDA   scan_return+1
      STA   input_pos+1
      LDA   scan_return+2
      STA   input_pos+2
      RTS

; ---------------------------------------------------------------------
; Pass 1: typed instruction selection and direct condition lowering.
; ---------------------------------------------------------------------

process_lowering:
      LDA   #<ir_compare
      LDX   #>ir_compare
      JSR   line_starts
      BNE   @pending
      LDA   line_len
      CMP   #6
      long_bne @bad
      LDA   pending_compare
      long_bne @bad
      LDA   line_buf+5
      STA   pending_compare
      LDA   #0
      RTS

@pending:
      LDA   pending_compare
      BEQ   @dispatch
      LDA   #<ir_branch
      LDX   #>ir_branch
      JSR   line_starts
      BNE   @flush
      JSR   emit_compare_branch
      BNE   @done
      STZ   pending_compare
      LDA   #0
@done:
      RTS
@flush:
      JSR   emit_materialized_compare
      BNE   @done
      STZ   pending_compare

@dispatch:
      LDA   #<ir_branch
      LDX   #>ir_branch
      JSR   line_starts
      BNE   @add
      JMP   emit_boolean_branch
@add:
      LDA   #<ir_add_word
      LDX   #>ir_add_word
      JSR   line_equals
      BNE   @subtract
      LDA   #<lower_add_word
      LDX   #>lower_add_word
      JMP   emit_text
@subtract:
      LDA   #<ir_sub_word
      LDX   #>ir_sub_word
      JSR   line_equals
      BNE   @compare_word
      LDA   #<lower_sub_word
      LDX   #>lower_sub_word
      JMP   emit_text
@compare_word:
      LDA   #<ir_compare_word
      LDX   #>ir_compare_word
      JSR   line_equals
      BNE   @array_get
      LDA   #<lower_compare_word
      LDX   #>lower_compare_word
      JMP   emit_text
@array_get:
      LDA   #<ir_array_get
      LDX   #>ir_array_get
      JSR   line_starts
      BNE   @array_byte
      LDA   #5
      STA   operand_pos
      JMP   emit_array_get
@array_byte:
      LDA   #<ir_array_byte
      LDX   #>ir_array_byte
      JSR   line_starts
      BNE   @array_word
      LDA   #5
      STA   operand_pos
      JMP   emit_array_byte
@array_word:
      LDA   #<ir_array_word
      LDX   #>ir_array_word
      JSR   line_starts
      BNE   @load_byte
      LDA   #5
      STA   operand_pos
      JMP   emit_array_word
@load_byte:
      LDA   #<ir_load_byte
      LDX   #>ir_load_byte
      JSR   line_starts
      BNE   @load_word
      LDA   #6
      STA   operand_pos
      LDA   #<load_byte_prefix
      LDX   #>load_byte_prefix
      JMP   emit_named_line
@load_word:
      LDA   #<ir_load_word
      LDX   #>ir_load_word
      JSR   line_starts
      BNE   @store_byte
      JMP   emit_load_word
@store_byte:
      LDA   #<ir_store_byte
      LDX   #>ir_store_byte
      JSR   line_starts
      BNE   @store_word
      LDA   #6
      STA   operand_pos
      LDA   #<store_byte_prefix
      LDX   #>store_byte_prefix
      JMP   emit_named_line
@store_word:
      LDA   #<ir_store_word
      LDX   #>ir_store_word
      JSR   line_starts
      BNE   @store_extend
      LDA   #<store_word_high
      LDX   #>store_word_high
      JMP   emit_store_word
@store_extend:
      LDA   #<ir_store_extend
      LDX   #>ir_store_extend
      JSR   line_starts
      BNE   @byte_add
      LDA   #<store_extend_high
      LDX   #>store_extend_high
      JMP   emit_store_word
@byte_add:
      LDA   #<ir_add_byte
      LDX   #>ir_add_byte
      JSR   line_equals
      BNE   @byte_subtract
      LDA   #<lower_add_byte
      LDX   #>lower_add_byte
      JMP   emit_text
@byte_subtract:
      LDA   #<ir_sub_byte
      LDX   #>ir_sub_byte
      JSR   line_equals
      BNE   @compare_byte
      LDA   #<lower_sub_byte
      LDX   #>lower_sub_byte
      JMP   emit_text
@compare_byte:
      LDA   #<ir_compare_byte
      LDX   #>ir_compare_byte
      JSR   line_equals
      BNE   @call
      LDA   #<lower_compare_byte
      LDX   #>lower_compare_byte
      JMP   emit_text
@call:
      LDA   #<ir_call
      LDX   #>ir_call
      JSR   line_starts
      BNE   @routine
      LDA   #5
      STA   operand_pos
      LDA   #<call_prefix
      LDX   #>call_prefix
      JMP   emit_named_line
@routine:
      LDA   #<ir_routine
      LDX   #>ir_routine
      JSR   line_starts
      BNE   @routine_end
      LDA   #7
      STA   operand_pos
      LDA   #<empty_text
      LDX   #>empty_text
      JSR   emit_named
      long_bne @raw_done
      LDA   #<label_suffix
      LDX   #>label_suffix
      JMP   emit_text
@routine_end:
      LDA   #<ir_routine_end
      LDX   #>ir_routine_end
      JSR   line_starts
      long_bne @function_entry
      LDA   line_buf+5
      CMP   #'w'
      BNE   :+
      LDA   #<fast_result_return_wide
      LDX   #>fast_result_return_wide
      JMP   emit_text
:
      LDA   line_buf+5
      CMP   #'f'
      BNE   :+
      LDA   #<fast_result_return
      LDX   #>fast_result_return
      JMP   emit_text
:
      LDA   line_buf+5
      CMP   #'W'
      BNE   :+
      LDA   #<function_return_wide_text
      LDX   #>function_return_wide_text
      JMP   emit_text
:
      LDA   line_buf+5
      CMP   #'F'
      BNE   :+
      LDA   #<function_return_text
      LDX   #>function_return_text
      JMP   emit_text
:
      LDA   #<return_text
      LDX   #>return_text
      JMP   emit_text
@function_entry:
      LDA   #<ir_inline_end
      LDX   #>ir_inline_end
      JSR   line_starts
      BNE   @not_inline_end
      LDA   line_buf+5
      CMP   #'w'
      BNE   :+
      LDA   #<fast_result_load_wide
      LDX   #>fast_result_load_wide
      JMP   emit_text
:
      LDA   line_buf+5
      CMP   #'f'
      BNE   @inline_stack
      LDA   #<fast_result_load
      LDX   #>fast_result_load
      JMP   emit_text
@inline_stack:
      CMP   #'W'
      BNE   :+
      LDA   #<inline_function_return_wide
      LDX   #>inline_function_return_wide
      JMP   emit_text
:
      LDA   line_buf+5
      CMP   #'F'
      BNE   @inline_procedure
      LDA   #<inline_function_return
      LDX   #>inline_function_return
      JMP   emit_text
@inline_procedure:
      LDA   #0
      RTS
@not_inline_end:
      LDA   #<ir_function_entry_wide
      LDX   #>ir_function_entry_wide
      JSR   line_equals
      BNE   @byte_function_entry
      LDA   #<function_entry_wide_text
      LDX   #>function_entry_wide_text
      JMP   emit_text
@byte_function_entry:
      LDA   #<ir_function_entry
      LDX   #>ir_function_entry
      JSR   line_equals
      BNE   @function_store
      LDA   #<function_entry_text
      LDX   #>function_entry_text
      JMP   emit_text
@function_store:
      LDA   #<ir_function_store_wide
      LDX   #>ir_function_store_wide
      JSR   line_equals
      BNE   @byte_function_store
      LDA   #<function_store_wide_text
      LDX   #>function_store_wide_text
      JMP   emit_text
@byte_function_store:
      LDA   #<ir_function_store
      LDX   #>ir_function_store
      JSR   line_equals
      BNE   @accumulate_byte
      LDA   #<function_store_text
      LDX   #>function_store_text
      JMP   emit_text
@accumulate_byte:
      LDA   #<ir_accumulate_byte
      LDX   #>ir_accumulate_byte
      JSR   line_starts
      BNE   @zero_branch
      JMP   emit_accumulate_byte
@zero_branch:
      LDA   #<ir_zero_branch
      LDX   #>ir_zero_branch
      JSR   line_starts
      BNE   @window_pointer
      JMP   emit_zero_branch
@window_pointer:
      LDA   #<ir_window_pointer
      LDX   #>ir_window_pointer
      JSR   line_starts
      BNE   @window_load
      JMP   lower_window_pointer
@window_load:
      LDA   #<ir_window_load
      LDX   #>ir_window_load
      JSR   line_starts
      BNE   @raw
      JMP   lower_window_load
@raw:
      JMP   emit_line
@raw_done:
      RTS
@bad:
      LDA   #2
      STA   opt_error
      LDA   #1
      RTS

emit_array_get:
      LDA   #<array_get_low
      LDX   #>array_get_low
      JSR   emit_text
      BNE   @done
      JSR   emit_operand
      BNE   @done
      LDA   #<array_get_high
      LDX   #>array_get_high
      JSR   emit_text
      BNE   @done
      LDA   #5
      STA   operand_pos
      JSR   emit_operand
      BNE   @done
      LDA   #<array_get_end
      LDX   #>array_get_end
      JMP   emit_text
@done:
      RTS

emit_array_byte:
      LDA   #<array_byte_low
      LDX   #>array_byte_low
      JSR   emit_text
      BNE   @done
      JSR   emit_operand
      BNE   @done
      LDA   #<array_byte_high
      LDX   #>array_byte_high
      JSR   emit_text
      BNE   @done
      LDA   #5
      STA   operand_pos
      JSR   emit_operand
      BNE   @done
      LDA   #<array_store_end
      LDX   #>array_store_end
      JMP   emit_text
@done:
      RTS

emit_array_word:
      LDA   #<array_word_low
      LDX   #>array_word_low
      JSR   emit_text
      BNE   @done
      JSR   emit_operand
      BNE   @done
      LDA   #<array_word_high
      LDX   #>array_word_high
      JSR   emit_text
      BNE   @done
      LDA   #5
      STA   operand_pos
      JSR   emit_operand
      BNE   @done
      LDA   #<array_store_end
      LDX   #>array_store_end
      JMP   emit_text
@done:
      RTS

emit_named_line:
      JSR   emit_named
      BNE   @done
      LDA   #$0A
      JMP   emit_byte
@done:
      RTS

; A/X is an assembly prefix; operand_pos selects the typed IR operand.
emit_named:
      JSR   emit_text
      BNE   @done
      JMP   emit_operand
@done:
      RTS

emit_load_word:
      LDA   #6
      STA   operand_pos
      LDA   #<load_byte_prefix
      LDX   #>load_byte_prefix
      JSR   emit_named
      BNE   @done
      LDA   #<load_word_high
      LDX   #>load_word_high
      JSR   emit_text
      BNE   @done
      LDA   #6
      STA   operand_pos
      JSR   emit_operand
      BNE   @done
      LDA   #<plus_one_suffix
      LDX   #>plus_one_suffix
      JMP   emit_text
@done:
      RTS

; A/X is STX or STZ for the high byte.
emit_store_word:
      STA   saved_text
      STX   saved_text+1
      LDA   #6
      STA   operand_pos
      LDA   #<store_byte_prefix
      LDX   #>store_byte_prefix
      JSR   emit_named
      BNE   @done
      LDA   saved_text
      LDX   saved_text+1
      JSR   emit_text
      BNE   @done
      LDA   #6
      STA   operand_pos
      JSR   emit_operand
      BNE   @done
      LDA   #<plus_one_suffix
      LDX   #>plus_one_suffix
      JMP   emit_text
@done:
      RTS

emit_accumulate_byte:
      LDA   #6
      JSR   capture_symbol
      BNE   @done
      LDA   #<accumulate_prefix
      LDX   #>accumulate_prefix
      JSR   emit_text
      BNE   @done
      JSR   emit_symbol
      BNE   @done
      LDA   #<accumulate_store
      LDX   #>accumulate_store
      JSR   emit_text
      BNE   @done
      JSR   emit_symbol
      BNE   @done
      LDA   #$0A
      JMP   emit_byte
@done:
      RTS

; .O2Z <comparison> <byte-symbol> <false-label>
emit_zero_branch:
      LDA   line_buf+5
      STA   compare_op
      LDA   #7
      JSR   capture_token
      BNE   @bad
      INC   operand_pos
      LDA   compare_op
      CMP   #'g'
      BEQ   @ok
      CMP   #'L'
      BEQ   @always
      LDA   #<load_byte_prefix
      LDX   #>load_byte_prefix
      JSR   emit_text
      BNE   @done
      JSR   emit_symbol
      BNE   @done
      LDA   #$0A
      JSR   emit_byte
      BNE   @done
      LDA   compare_op
      CMP   #'E'
      BEQ   @false_nonzero
      CMP   #'l'
      BEQ   @false_nonzero
      LDA   #<branch_zero_prefix
      LDX   #>branch_zero_prefix
      BRA   @branch
@false_nonzero:
      LDA   #<branch_nonzero_prefix
      LDX   #>branch_nonzero_prefix
@branch:
      JSR   emit_text
      BNE   @done
      JSR   emit_operand
      BNE   @done
      LDA   #<label_end
      LDX   #>label_end
      JMP   emit_text
@always:
      LDA   #<jump_prefix
      LDX   #>jump_prefix
      JSR   emit_text
      BNE   @done
      JSR   emit_operand
      BNE   @done
      LDA   #$0A
      JMP   emit_byte
@bad:
      INC   opt_error
      LDA   #1
      RTS
@ok:
      LDA   #0
@done:
      RTS

; Capture a space-delimited token, leaving operand_pos on its delimiter.
capture_token:
      STA   operand_pos
      LDY   #0
@byte:
      LDX   operand_pos
      CPX   line_len
      BCS   @bad
      LDA   line_buf,X
      CMP   #' '
      BEQ   @done
      CPY   #63
      BCS   @bad
      STA   symbol_buf,Y
      INC   operand_pos
      INY
      BRA   @byte
@done:
      TYA
      BEQ   @bad
      STA   symbol_len
      LDA   #0
      RTS
@bad:
      LDA   #1
      RTS

lower_window_load:
      LDA   line_buf+5
      STA   immediate_hi
      LDA   line_buf+6
      STA   immediate_lo
      LDA   #<window_load_prefix
      LDX   #>window_load_prefix
      JSR   emit_text
      BNE   @done
      JSR   emit_immediate
      BNE   @done
      LDA   #<window_load_tail
      LDX   #>window_load_tail
      JMP   emit_text
@done:
      RTS

lower_window_pointer:
      LDA   line_buf+5
      STA   compare_op
      LDA   line_buf+6
      STA   immediate_hi
      LDA   line_buf+7
      STA   immediate_lo
      LDA   #9
      JSR   capture_token
      BNE   @bad
      JSR   copy_symbol_to_candidate_index
      INC   operand_pos
      LDA   operand_pos
      JSR   capture_symbol
      BNE   @bad
      JSR   copy_symbol_to_candidate_array
      LDA   #<load_byte_prefix
      LDX   #>load_byte_prefix
      JSR   emit_text
      BNE   @done
      JSR   emit_candidate_index
      BNE   @done
      LDA   #<load_word_high
      LDX   #>load_word_high
      JSR   emit_text
      BNE   @done
      JSR   emit_candidate_index
      BNE   @done
      LDA   #<window_pointer_low
      LDX   #>window_pointer_low
      JSR   emit_text
      BNE   @done
      JSR   emit_window_expression
      BNE   @done
      LDA   #<window_pointer_high
      LDX   #>window_pointer_high
      JSR   emit_text
      BNE   @done
      JSR   emit_window_expression
      BNE   @done
      LDA   #<window_pointer_end
      LDX   #>window_pointer_end
      JMP   emit_text
@bad:
      INC   opt_error
      LDA   #1
@done:
      RTS

emit_window_expression:
      JSR   emit_candidate_array
      BNE   @done
      LDA   compare_op
      JSR   emit_byte
      BNE   @done
      LDA   #'$'
      JSR   emit_byte
      BNE   @done
      LDA   immediate_hi
      JSR   emit_byte
      BNE   @done
      LDA   immediate_lo
      JMP   emit_byte
@done:
      RTS

emit_label_operand:
      LDA   #5
      STA   operand_pos
      JMP   emit_operand

emit_jump_operand:
      LDA   #<jump_prefix
      LDX   #>jump_prefix
      JSR   emit_text
      BNE   @done
      JSR   emit_label_operand
      BNE   @done
      LDA   #<label_end
      LDX   #>label_end
      JMP   emit_text
@done:
      RTS

emit_boolean_branch:
      LDA   #<boolean_branch
      LDX   #>boolean_branch
      JSR   emit_text
      BNE   @done
      JMP   emit_jump_operand
@done:
      RTS

emit_compare_branch:
      LDA   pending_compare
      CMP   #'E'
      BEQ   @eq
      CMP   #'N'
      BEQ   @ne
      CMP   #'L'
      BEQ   @lt
      CMP   #'l'
      BEQ   @le
      CMP   #'G'
      BEQ   @gt
      CMP   #'g'
      BEQ   @ge
      INC   opt_error
      LDA   #1
      RTS
@eq:
      LDA   #<branch_eq
      LDX   #>branch_eq
      BRA   @simple
@ne:
      LDA   #<branch_ne
      LDX   #>branch_ne
      BRA   @simple
@lt:
      LDA   #<branch_lt
      LDX   #>branch_lt
      BRA   @simple
@ge:
      LDA   #<branch_ge
      LDX   #>branch_ge
@simple:
      JSR   emit_text
      BNE   @done
      JMP   emit_jump_operand
@le:
      LDA   #<branch_le
      LDX   #>branch_le
      BRA   @compound
@gt:
      LDA   #<branch_gt
      LDX   #>branch_gt
@compound:
      JSR   emit_text
      BNE   @done
      JSR   emit_jump_operand
      BNE   @done
      LDA   #<compound_end
      LDX   #>compound_end
      JMP   emit_text
@done:
      RTS

emit_materialized_compare:
      LDA   pending_compare
      CMP   #'E'
      BEQ   @eq
      CMP   #'N'
      BEQ   @ne
      CMP   #'L'
      BEQ   @lt
      CMP   #'l'
      BEQ   @le
      CMP   #'G'
      BEQ   @gt
      CMP   #'g'
      BEQ   @ge
      INC   opt_error
      LDA   #1
      RTS
@eq:
      LDA   #<materialize_eq
      LDX   #>materialize_eq
      JMP   emit_text
@ne:
      LDA   #<materialize_ne
      LDX   #>materialize_ne
      JMP   emit_text
@lt:
      LDA   #<materialize_lt
      LDX   #>materialize_lt
      JMP   emit_text
@le:
      LDA   #<materialize_le
      LDX   #>materialize_le
      JMP   emit_text
@gt:
      LDA   #<materialize_gt
      LDX   #>materialize_gt
      JMP   emit_text
@ge:
      LDA   #<materialize_ge
      LDX   #>materialize_ge
      JMP   emit_text

; ---------------------------------------------------------------------
; Pass 2: fold constant stack temporaries into immediate operations.
; ---------------------------------------------------------------------

process_peephole:
      LDA   #<load_byte_prefix
      LDX   #>load_byte_prefix
      JSR   line_starts
      BNE   @stack
      LDA   #4
      JSR   capture_symbol
      BNE   @stack
      JSR   symbol_is_nvr
      BNE   @stack
      JSR   save_input_position
      JSR   read_line
      BNE   @restore_load
      LDA   #<store_byte_prefix
      LDX   #>store_byte_prefix
      JSR   line_starts
      BNE   @restore_load
      LDA   #4
      JSR   operand_is_symbol
      BNE   @restore_load
      LDA   #<load_byte_prefix
      LDX   #>load_byte_prefix
      JMP   emit_typed_symbol
@restore_load:
      JSR   restore_input_position
      LDA   #<load_byte_prefix
      LDX   #>load_byte_prefix
      JMP   emit_typed_symbol
@stack:
      LDA   #<line_pha
      LDX   #>line_pha
      JSR   line_equals
      BNE   @word
      JSR   save_input_position
      JSR   try_byte_immediate
      BEQ   @done
      JSR   restore_input_position
      JSR   try_cancel_pha_pla
      BEQ   @done
      LDA   #<line_pha_lf
      LDX   #>line_pha_lf
      JMP   emit_text
@word:
      LDA   #<line_phx
      LDX   #>line_phx
      JSR   line_equals
      BNE   @raw
      JSR   save_input_position
      JSR   try_word_immediate
      BEQ   @done
      JSR   restore_input_position
      LDA   #<line_phx_lf
      LDX   #>line_phx_lf
      JMP   emit_text
@raw:
      JMP   emit_line
@done:
      RTS

; PHA/PLA merely shuttle an unchanged A in the generated pointer sequence.
; PLX supplies the live flags, so retaining the pair costs two bytes and cycles.
try_cancel_pha_pla:
      LDA   #<line_pla
      LDX   #>line_pla
      JSR   expect_line
      BNE   @no
      LDA   #<line_sta_nvr0l
      LDX   #>line_sta_nvr0l
      JSR   expect_line
      BNE   @no
      LDA   #<line_plx
      LDX   #>line_plx
      JSR   expect_line
      BNE   @no
      LDA   #<stack_shuttle_tail
      LDX   #>stack_shuttle_tail
      JMP   emit_text
@no:
      JSR   restore_input_position
      LDA   #1
      RTS

; Only compiler scratch cells are eligible for load/store self-assignment
; removal. Raw inline assembly may intentionally touch memory-mapped I/O.
symbol_is_nvr:
      LDA   symbol_len
      CMP   #5
      BNE   @no
      LDA   symbol_buf
      CMP   #'N'
      BNE   @no
      LDA   symbol_buf+1
      CMP   #'V'
      BNE   @no
      LDA   symbol_buf+2
      CMP   #'R'
      BNE   @no
      LDA   symbol_buf+3
      CMP   #'0'
      BCC   @no
      CMP   #'8'
      BCS   @no
      LDA   symbol_buf+4
      CMP   #'L'
      BEQ   @yes
      CMP   #'H'
      BNE   @no
@yes:
      LDA   #0
      RTS
@no:
      LDA   #1
      RTS

save_input_position:
      LDA   input_pos
      STA   saved_pos
      LDA   input_pos+1
      STA   saved_pos+1
      LDA   input_pos+2
      STA   saved_pos+2
      RTS

restore_input_position:
      LDA   saved_pos
      STA   input_pos
      LDA   saved_pos+1
      STA   input_pos+1
      LDA   saved_pos+2
      STA   input_pos+2
      RTS

read_literal:
      JSR   read_line
      BNE   @no
      LDA   #<literal_prefix
      LDX   #>literal_prefix
      JSR   line_starts
      BNE   @no
      LDA   line_len
      CMP   #8
      BNE   @no
      LDA   line_buf+6
      STA   immediate_hi
      LDA   line_buf+7
      STA   immediate_lo
      LDA   #0
      RTS
@no:
      LDA   #1
      RTS

expect_line:
      STA   text_ptr
      STX   text_ptr+1
      JSR   read_line
      BNE   @no
      LDA   text_ptr
      LDX   text_ptr+1
      JMP   line_equals
@no:
      LDA   #1
      RTS

try_byte_immediate:
      JSR   read_literal
      long_bne @no
      LDA   #<line_sta_nvr0l
      LDX   #>line_sta_nvr0l
      JSR   expect_line
      long_bne @no
      LDA   #<line_pla
      LDX   #>line_pla
      JSR   expect_line
      long_bne @no
      JSR   read_line
      long_bne @no
      LDA   #<line_cmp_nvr0l
      LDX   #>line_cmp_nvr0l
      JSR   line_equals
      BEQ   @compare
      LDA   #<line_clc
      LDX   #>line_clc
      JSR   line_equals
      BEQ   @add
      LDA   #<line_sec
      LDX   #>line_sec
      JSR   line_equals
      BNE   @no
      LDA   #<line_sbc_nvr0l
      LDX   #>line_sbc_nvr0l
      JSR   expect_line
      long_bne @no
      LDA   #<fold_sub
      LDX   #>fold_sub
      BRA   @emit
@add:
      LDA   #<line_adc_nvr0l
      LDX   #>line_adc_nvr0l
      JSR   expect_line
      long_bne @no
      LDA   #<fold_add
      LDX   #>fold_add
      BRA   @emit
@compare:
      LDA   #<fold_compare
      LDX   #>fold_compare
@emit:
      JSR   emit_text
      BNE   @no
      JSR   emit_immediate
      RTS
@no:
      LDA   #1
      RTS

try_word_immediate:
      LDA   #<line_pha
      LDX   #>line_pha
      JSR   expect_line
      long_bne @no
      JSR   read_literal
      long_bne @no
      LDA   #<line_sta_nvr0l
      LDX   #>line_sta_nvr0l
      JSR   expect_line
      long_bne @no
      LDA   #<line_stz_nvr0h
      LDX   #>line_stz_nvr0h
      JSR   expect_line
      long_bne @no
      LDA   #<line_pla
      LDX   #>line_pla
      JSR   expect_line
      long_bne @no
      LDA   #<line_plx
      LDX   #>line_plx
      JSR   expect_line
      long_bne @no
      JSR   read_line
      long_bne @no
      LDA   #<line_clc
      LDX   #>line_clc
      JSR   line_equals
      BEQ   @add
      LDA   #<line_sec
      LDX   #>line_sec
      JSR   line_equals
      long_bne @no
      LDA   #<line_sbc_nvr0l
      LDX   #>line_sbc_nvr0l
      JSR   expect_line
      long_bne @no
      LDA   #1
      STA   word_operation
      BRA   @tail
@add:
      LDA   #<line_adc_nvr0l
      LDX   #>line_adc_nvr0l
      JSR   expect_line
      long_bne @no
      STZ   word_operation
@tail:
      LDA   #<line_sta_nvr0l
      LDX   #>line_sta_nvr0l
      JSR   expect_line
      long_bne @no
      LDA   #<line_txa
      LDX   #>line_txa
      JSR   expect_line
      long_bne @no
      LDA   word_operation
      BNE   @high_sub
      LDA   #<line_adc_nvr0h
      LDX   #>line_adc_nvr0h
      BRA   @high
@high_sub:
      LDA   #<line_sbc_nvr0h
      LDX   #>line_sbc_nvr0h
@high:
      JSR   expect_line
      long_bne @no
      LDA   #<line_tax
      LDX   #>line_tax
      JSR   expect_line
      long_bne @no
      LDA   #<line_lda_nvr0l
      LDX   #>line_lda_nvr0l
      JSR   expect_line
      long_bne @no
      JSR   try_word_array
      BEQ   @done
      JSR   skip_redundant_result_store
      LDA   word_operation
      BNE   @emit_sub_prefix
      LDA   #<word_add_tail
      LDX   #>word_add_tail
      BRA   @emit_prefix
@emit_sub_prefix:
      LDA   #<word_sub_tail
      LDX   #>word_sub_tail
@emit_prefix:
      JSR   emit_text
      long_bne @no
      JSR   emit_immediate
      long_bne @no
      LDA   word_operation
      BNE   @emit_sub_high
      LDA   #<word_add_high
      LDX   #>word_add_high
      JMP   emit_text
@emit_sub_high:
      LDA   #<word_sub_high
      LDX   #>word_sub_high
      JMP   emit_text
@done:
      RTS
@no:
      LDA   #1
      RTS

; The folded word operation already leaves its low byte in both A and NVR0L.
; Consume an immediately following compiler-generated self-store.
skip_redundant_result_store:
      JSR   save_input_position
      LDA   #<line_sta_nvr0l
      LDX   #>line_sta_nvr0l
      JSR   expect_line
      BEQ   @done
      JSR   restore_input_position
@done:
      RTS

try_word_array:
      LDA   input_pos
      STA   lookahead_pos
      LDA   input_pos+1
      STA   lookahead_pos+1
      LDA   input_pos+2
      STA   lookahead_pos+2
      LDA   #<line_clc
      LDX   #>line_clc
      JSR   expect_line
      long_bne @no
      JSR   read_line
      long_bne @no
      LDA   #<line_adc_low
      LDX   #>line_adc_low
      JSR   line_starts
      long_bne @no
      LDA   line_len
      SEC
      SBC   #6
      long_beq @no
      CMP   #64
      BCC   :+
      JMP   @no
:
      STA   symbol_len
      TAY
@copy_symbol:
      DEY
      LDA   line_buf+6,Y
      STA   symbol_buf,Y
      TYA
      BNE   @copy_symbol
      LDA   #<line_sta_nvr1l
      LDX   #>line_sta_nvr1l
      JSR   expect_line
      BNE   @no
      LDA   #<line_txa
      LDX   #>line_txa
      JSR   expect_line
      BNE   @no
      JSR   read_line
      BNE   @no
      LDA   #<line_adc_high
      LDX   #>line_adc_high
      JSR   line_starts
      BNE   @no
      LDA   line_len
      SEC
      SBC   #6
      CMP   symbol_len
      BNE   @no
      LDY   #0
@compare_symbol:
      CPY   symbol_len
      BEQ   @tail
      LDA   line_buf+6,Y
      CMP   symbol_buf,Y
      BNE   @no
      INY
      BRA   @compare_symbol
@tail:
      LDA   #<line_sta_nvr1h
      LDX   #>line_sta_nvr1h
      JSR   expect_line
      BNE   @no
      LDA   #<line_lda_indirect
      LDX   #>line_lda_indirect
      JSR   expect_line
      BNE   @no
      LDA   #<array_fold_low
      LDX   #>array_fold_low
      JSR   emit_text
      BNE   @done
      JSR   emit_symbol_offset
      BNE   @done
      LDA   #<array_fold_high
      LDX   #>array_fold_high
      JSR   emit_text
      BNE   @done
      JSR   emit_symbol_offset
      BNE   @done
      LDA   #<array_fold_end
      LDX   #>array_fold_end
      JMP   emit_text
@no:
      LDA   lookahead_pos
      STA   input_pos
      LDA   lookahead_pos+1
      STA   input_pos+1
      LDA   lookahead_pos+2
      STA   input_pos+2
      LDA   #1
@done:
      RTS

emit_symbol_offset:
      LDY   #0
@symbol:
      CPY   symbol_len
      BEQ   @operator
      LDA   symbol_buf,Y
      PHY
      JSR   emit_byte
      PLY
      CMP   #0
      BNE   @done
      INY
      BRA   @symbol
@operator:
      LDA   word_operation
      BEQ   :+
      LDA   #'-'
      BRA   @emit_operator
:
      LDA   #'+'
@emit_operator:
      JSR   emit_byte
      BNE   @done
      LDA   #'$'
      JSR   emit_byte
      BNE   @done
      LDA   immediate_hi
      JSR   emit_byte
      BNE   @done
      LDA   immediate_lo
      JMP   emit_byte
@done:
      RTS

emit_immediate:
      LDA   immediate_hi
      JSR   emit_byte
      BNE   @done
      LDA   immediate_lo
      JSR   emit_byte
      BNE   @done
      LDA   #$0A
      JMP   emit_byte
@done:
      RTS

      .segment "RODATA"
optimizer_banner: .byte "Nova Pascal Optimizer v1.0", $0D, $0A, 0
optimizer_file:   .byte "Optimizing ", 0
pass_typed:       .byte "Pass 1: typed IR optimization", $0D, $0A, 0
pass_inline1:     .byte "Pass 2: leaf routine inlining", $0D, $0A, 0
pass_inline2:     .byte "Pass 3: caller routine inlining", $0D, $0A, 0
pass_inline3:     .byte "Pass 4: dead routine cleanup", $0D, $0A, 0
pass_lowering:    .byte "Pass 5: instruction selection", $0D, $0A, 0
pass_peephole:    .byte "Pass 6: machine peepholes", $0D, $0A, 0
optimizer_ok:     .byte "Optimization complete", $0D, $0A, 0
large_temp_a:  .byte "__NPO2.A"
large_temp_a_end:
large_temp_b:  .byte "__NPO2.B"
large_temp_b_end:

pass_message_lo: .byte <pass_typed, <pass_inline1, <pass_inline2, <pass_inline3, <pass_lowering, <pass_peephole
pass_message_hi: .byte >pass_typed, >pass_inline1, >pass_inline2, >pass_inline3, >pass_lowering, >pass_peephole

ir_compare:      .byte ".O2C ", 0
ir_branch:       .byte ".O2F ", 0
ir_add_word:     .byte ".O2A", 0
ir_sub_word:     .byte ".O2S", 0
ir_compare_word: .byte ".O2X", 0
ir_array_get:    .byte ".O2G ", 0
ir_array_byte:   .byte ".O2B ", 0
ir_array_word:   .byte ".O2W ", 0
ir_load_byte:    .byte ".O2LB ", 0
ir_load_word:    .byte ".O2LW ", 0
ir_store_byte:   .byte ".O2SB ", 0
ir_store_word:   .byte ".O2SW ", 0
ir_store_extend: .byte ".O2SZ ", 0
ir_add_byte:     .byte ".O2+", 0
ir_sub_byte:     .byte ".O2-", 0
ir_compare_byte: .byte ".O2Q", 0
ir_call:         .byte ".O2J ", 0
ir_routine:      .byte ".O2R ", 0
frame_enter_line:.byte "JSR P_FENTER", 0
unit_export_marker: .byte " ;@NPUNIT"
unit_export_marker_end:
ir_routine_end:  .byte ".O2E ", 0
ir_inline_end:   .byte ".O2I ", 0
ir_function_entry: .byte ".O2K", 0
ir_function_entry_line: .byte ".O2K", $0A, 0
ir_function_entry_wide: .byte ".O2KW", 0
ir_function_entry_wide_line: .byte ".O2KW", $0A, 0
ir_function_store: .byte ".O2V", 0
ir_function_store_wide: .byte ".O2VW", 0
ir_accumulate_byte: .byte ".O2AB ", 0
ir_zero_branch:   .byte ".O2Z ", 0
ir_fast_function_end: .byte ".O2E f", $0A, 0
ir_fast_function_end_wide: .byte ".O2E w", $0A, 0
ir_window_pointer: .byte ".O2P ", 0
ir_window_load: .byte ".O2Y ", 0

lower_add_word: .byte "CLC", $0A, "ADC NVR0L", $0A, "STA NVR0L", $0A, "TXA", $0A, "ADC NVR0H", $0A, "TAX", $0A, "LDA NVR0L", $0A, 0
lower_sub_word: .byte "SEC", $0A, "SBC NVR0L", $0A, "STA NVR0L", $0A, "TXA", $0A, "SBC NVR0H", $0A, "TAX", $0A, "LDA NVR0L", $0A, 0
lower_compare_word: .byte "CPX NVR0H", $0A, "BNE :+", $0A, "CMP NVR0L", $0A, ":", $0A, 0
lower_add_byte: .byte "STA NVR0L", $0A, "PLA", $0A, "CLC", $0A, "ADC NVR0L", $0A, 0
lower_sub_byte: .byte "STA NVR0L", $0A, "PLA", $0A, "SEC", $0A, "SBC NVR0L", $0A, 0
lower_compare_byte: .byte "STA NVR0L", $0A, "PLA", $0A, "CMP NVR0L", $0A, 0

empty_text: .byte 0
load_byte_prefix: .byte "LDA ", 0
load_word_high: .byte $0A, "LDX ", 0
store_byte_prefix: .byte "STA ", 0
store_word_high: .byte $0A, "STX ", 0
store_extend_high: .byte $0A, "STZ ", 0
plus_one_suffix: .byte "+1", $0A, 0
call_prefix: .byte "JSR ", 0
label_suffix: .byte ":", $0A, 0
return_text: .byte "RTS", $0A, 0
function_entry_text: .byte "LDA #$00", $0A, "PHA", $0A, 0
function_store_text: .byte "TSX", $0A, "STA $0101,X", $0A, 0
function_return_text: .byte "PLA", $0A, "RTS", $0A, 0
function_entry_wide_text: .byte 0
function_store_wide_text: .byte "LDY #$02", $0A, "STA (__NP_FP),Y", $0A, "INY", $0A, "TXA", $0A, "STA (__NP_FP),Y", $0A, 0
function_return_wide_text: .byte "RTS", $0A, 0
fast_result_store: .byte "STA NVR3L", $0A, 0
fast_result_load: .byte "LDA NVR3L", $0A, 0
fast_result_return: .byte "LDA NVR3L", $0A, "RTS", $0A, 0
fast_result_store_wide: .byte "STA NVR3L", $0A, "STX NVR3H", $0A, 0
fast_result_load_wide: .byte "LDA NVR3L", $0A, "LDX NVR3H", $0A, 0
fast_result_return_wide: .byte "LDA NVR3L", $0A, "LDX NVR3H", $0A, "RTS", $0A, 0
inline_function_return: .byte "PLA", $0A, 0
inline_function_return_wide: .byte "PLA", $0A, "STA NVR3L", $0A, "PLA", $0A, "TAX", $0A, "LDA NVR3L", $0A, 0
cache_store: .byte "STA NVR2L", $0A, 0
cache_load: .byte "LDA NVR2L", $0A, 0
named_label_prefix: .byte "__NP_", 0
window_load_prefix: .byte "LDY #$", 0
window_load_tail: .byte "LDA (NVR4L),Y", $0A, 0
window_pointer_low: .byte "+1", $0A, "CLC", $0A, "ADC #<(", 0
window_pointer_high: .byte ")", $0A, "STA NVR4L", $0A, "TXA", $0A, "ADC #>(", 0
window_pointer_end: .byte ")", $0A, "STA NVR4H", $0A, 0
hex_digits: .byte "0123456789ABCDEF"
increment_prefix: .byte "INC ", 0
decrement_prefix: .byte "DEC ", 0
increment_word_middle: .byte $0A, "BNE :+", $0A, "INC ", 0
plus_one_label_suffix: .byte "+1", $0A, ":", $0A, 0
decrement_word_middle: .byte $0A, "BNE :+", $0A, "DEC ", 0
decrement_word_tail: .byte "+1", $0A, ":", $0A, "DEC ", 0
accumulate_prefix: .byte "CLC", $0A, "ADC ", 0
accumulate_store: .byte $0A, "STA ", 0
branch_zero_prefix: .byte "BNE :+", $0A, "JMP ", 0
branch_nonzero_prefix: .byte "BEQ :+", $0A, "JMP ", 0

array_get_low: .byte "CLC", $0A, "ADC #<", 0
array_get_high: .byte $0A, "STA NVR1L", $0A, "TXA", $0A, "ADC #>", 0
array_get_end: .byte $0A, "STA NVR1H", $0A, "LDA (NVR1L)", $0A, 0
array_byte_low: .byte "STA NVR0L", $0A, "PLA", $0A, "CLC", $0A, "ADC #<", 0
array_byte_high: .byte $0A, "STA NVR1L", $0A, "LDA #$00", $0A, "ADC #>", 0
array_word_low: .byte "STA NVR0L", $0A, "PLA", $0A, "CLC", $0A, "ADC #<", 0
array_word_high: .byte $0A, "STA NVR1L", $0A, "PLA", $0A, "ADC #>", 0
array_store_end: .byte $0A, "STA NVR1H", $0A, "LDA NVR0L", $0A, "STA (NVR1L)", $0A, 0

boolean_branch: .byte "CMP #$00", $0A, "BNE :+", $0A, 0
jump_prefix:    .byte "JMP ", 0
label_end:      .byte $0A, ":", $0A, 0
branch_eq:      .byte "BEQ :+", $0A, 0
branch_ne:      .byte "BNE :+", $0A, 0
branch_lt:      .byte "BCC :+", $0A, 0
branch_ge:      .byte "BCS :+", $0A, 0
branch_le:      .byte "BCC :+", $0A, "BEQ :+", $0A, 0
branch_gt:      .byte "BCC :+", $0A, "BEQ :+", $0A, "BRA :++", $0A, ":", $0A, 0
compound_end:   .byte ":", $0A, 0

materialize_eq: .byte "BNE :+", $0A, "LDA #$01", $0A, "BRA :++", $0A, ":", $0A, "LDA #$00", $0A, ":", $0A, 0
materialize_ne: .byte "BEQ :+", $0A, "LDA #$01", $0A, "BRA :++", $0A, ":", $0A, "LDA #$00", $0A, ":", $0A, 0
materialize_lt: .byte "BCS :+", $0A, "LDA #$01", $0A, "BRA :++", $0A, ":", $0A, "LDA #$00", $0A, ":", $0A, 0
materialize_le: .byte "BCC :+", $0A, "BEQ :+", $0A, "LDA #$00", $0A, "BRA :++", $0A, ":", $0A, "LDA #$01", $0A, ":", $0A, 0
materialize_gt: .byte "BCC :+", $0A, "BEQ :+", $0A, "LDA #$01", $0A, "BRA :++", $0A, ":", $0A, "LDA #$00", $0A, ":", $0A, 0
materialize_ge: .byte "BCC :+", $0A, "LDA #$01", $0A, "BRA :++", $0A, ":", $0A, "LDA #$00", $0A, ":", $0A, 0

line_pha:       .byte "PHA", 0
line_pha_lf:    .byte "PHA", $0A, 0
line_phx:       .byte "PHX", 0
line_phx_lf:    .byte "PHX", $0A, 0
literal_prefix: .byte "LDA #$", 0
literal_x_prefix: .byte "LDX #$", 0
line_literal_zero: .byte "LDA #$00", 0
line_literal_one: .byte "LDA #$01", 0
line_sta_nvr0l: .byte "STA NVR0L", 0
line_sta_nvr1l: .byte "STA NVR1L", 0
line_sta_nvr1h: .byte "STA NVR1H", 0
line_stz_nvr0h: .byte "STZ NVR0H", 0
line_pla:       .byte "PLA", 0
line_plx:       .byte "PLX", 0
stack_shuttle_tail: .byte "STA NVR0L", $0A, "PLX", $0A, 0
line_cmp_nvr0l: .byte "CMP NVR0L", 0
line_clc:       .byte "CLC", 0
line_sec:       .byte "SEC", 0
line_adc_nvr0l: .byte "ADC NVR0L", 0
line_sbc_nvr0l: .byte "SBC NVR0L", 0
line_adc_nvr0h: .byte "ADC NVR0H", 0
line_sbc_nvr0h: .byte "SBC NVR0H", 0
line_txa:       .byte "TXA", 0
line_tax:       .byte "TAX", 0
line_lda_nvr0l: .byte "LDA NVR0L", 0
line_lda_indirect: .byte "LDA (NVR1L)", 0
line_adc_low:   .byte "ADC #<", 0
line_adc_high:  .byte "ADC #>", 0

pure_line_lo: .byte <line_pha, <line_phx, <line_pla, <line_plx, <line_sta_nvr0l, <line_stz_nvr0h, 0
pure_line_hi: .byte >line_pha, >line_phx, >line_pla, >line_plx, >line_sta_nvr0l, >line_stz_nvr0h, 0

fold_compare: .byte "CMP #$", 0
fold_add:     .byte "CLC", $0A, "ADC #$", 0
fold_sub:     .byte "SEC", $0A, "SBC #$", 0
word_add_tail:.byte "CLC", $0A, "ADC #$", 0
word_sub_tail:.byte "SEC", $0A, "SBC #$", 0
word_add_high:.byte "STA NVR0L", $0A, "TXA", $0A, "ADC #$00", $0A, "TAX", $0A, "LDA NVR0L", $0A, 0
word_sub_high:.byte "STA NVR0L", $0A, "TXA", $0A, "SBC #$00", $0A, "TAX", $0A, "LDA NVR0L", $0A, 0
array_fold_low: .byte "CLC", $0A, "ADC #<(", 0
array_fold_high:.byte ")", $0A, "STA NVR1L", $0A, "TXA", $0A, "ADC #>(", 0
array_fold_end: .byte ")", $0A, "STA NVR1H", $0A, "LDA (NVR1L)", $0A, 0

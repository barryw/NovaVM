; NovaPascal O2 optimizer: compact typed IR -> optimized textual 65C02.

      .setcpu "w65c02"
      .include "nova.inc"
      .include "libmemory.inc"
      .include "nptool.inc"
      .include "longbranch.inc"
      .include "xram.inc"
      .include "xramstream.inc"

ASM_CAP = $4000
ASM_LOAD_CAP = ASM_CAP + 1

      .segment "ZEROPAGE"
text_ptr:        .res 2

      .segment "BSS"
buffer_a:        .res 3
buffer_b:        .res 3
buffer_a_owned:  .res 1
buffer_b_owned:  .res 1
input_base:      .res 3
input_addr:      .res 3
input_pos:       .res 2
input_len:       .res 2
output_base:     .res 3
output_addr:     .res 3
output_len:      .res 2
output_cap:      .res 2
saved_pos:       .res 2
lookahead_pos:   .res 2
line_len:        .res 1
line_has_lf:     .res 1
line_buf:        .res 256
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
rhs_start:       .res 2
rhs_stop:        .res 2
rhs_end:         .res 2
compare_op:      .res 1
result_fast:     .res 1
result_seen:     .res 1
target_len:      .res 1
target_buf:      .res 64
scan_return:     .res 2
nested_return:   .res 2
routine_body:    .res 2
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
parse_return:    .res 2

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
      BNE   :+
      LDA   XRAM_LENL
      CMP   #<ASM_LOAD_CAP
      BNE   :+
      JMP   output_error
:
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
      LDA   LIB_STATUS
      STA   NPTOOL_DETAIL
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

; ---------------------------------------------------------------------
; Pass driver and XRAM line stream.
; ---------------------------------------------------------------------

run_pass:
      STZ   input_pos
      STZ   input_pos+1
      STZ   output_len
      STZ   output_len+1
      STZ   pending_compare
      STZ   opt_error
      STZ   cache_valid
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
      STZ   line_len
      STZ   line_has_lf
      LDX   #0
@byte:
      JSR   input_at_end
      BCS   @done
      PHX
      JSR   read_input_byte
      STA   byte_value
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
@lf:
      INC   line_has_lf
@done:
      STX   line_len
      LDA   #0
      RTS

      xram_stream_impl opt_error, 3

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
      BNE   @raw
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
      JSR   line_equals
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
@scan:
      LDA   input_pos
      STA   rhs_stop
      LDA   input_pos+1
      STA   rhs_stop+1
      JSR   read_line
      long_bne @no
      LDA   #<ir_add_byte
      LDX   #>ir_add_byte
      JSR   line_equals
      BEQ   @possible_end
      JSR   line_is_pure_expression
      BEQ   @scan
      BRA   @no
@possible_end:
      JSR   read_line
      BNE   @no
      LDA   #<ir_store_byte
      LDX   #>ir_store_byte
      JSR   line_starts
      BNE   @no
      LDA   #6
      JSR   operand_is_symbol
      BNE   @no
      LDA   input_pos
      STA   rhs_end
      LDA   input_pos+1
      STA   rhs_end+1
      LDA   rhs_start
      STA   input_pos
      LDA   rhs_start+1
      STA   input_pos+1
@copy:
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

process_inline:
      LDA   #<ir_routine
      LDX   #>ir_routine
      JSR   line_starts
      BNE   @call
      LDA   #7
      JSR   capture_target
      long_bne @raw
      JSR   analyze_inline_target
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
      BNE   @raw
      LDA   #5
      JSR   capture_target
      BNE   @raw
      LDA   input_pos
      STA   rhs_end
      LDA   input_pos+1
      STA   rhs_end+1
      JSR   analyze_inline_target
      BNE   @emit_call
      LDA   routine_body
      STA   input_pos
      LDA   routine_body+1
      STA   input_pos+1
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
      LDA   input_pos
      STA   scan_return
      LDA   input_pos+1
      STA   scan_return+1
      STZ   input_pos
      STZ   input_pos+1
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
@leaf:
      JSR   read_line
      BNE   @no
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

; Zero when an opaque NAS line contains target_buf as a case-insensitive
; symbol token. Any such reference keeps the Pascal routine callable.
inline_asm_references_target:
      JSR   line_is_inline_asm
      BNE   @no
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
      STZ   input_pos
      STZ   input_pos+1
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
      PLA
      RTS

restore_scan_return:
      LDA   scan_return
      STA   input_pos
      LDA   scan_return+1
      STA   input_pos+1
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
      CMP   #'f'
      BNE   :+
      LDA   #<fast_result_return
      LDX   #>fast_result_return
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
      BNE   :+
      LDA   line_buf+5
      CMP   #'f'
      BNE   @inline_stack
      LDA   #<fast_result_load
      LDX   #>fast_result_load
      JMP   emit_text
@inline_stack:
      CMP   #'F'
      BNE   @inline_procedure
      LDA   #<inline_function_return
      LDX   #>inline_function_return
      JMP   emit_text
@inline_procedure:
      LDA   #0
      RTS
:
      LDA   #<ir_function_entry
      LDX   #>ir_function_entry
      JSR   line_equals
      BNE   @function_store
      LDA   #<function_entry_text
      LDX   #>function_entry_text
      JMP   emit_text
@function_store:
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
      LDA   #<line_pha
      LDX   #>line_pha
      JSR   line_equals
      BNE   @word
      JSR   save_input_position
      JSR   try_byte_immediate
      BEQ   @done
      JSR   restore_input_position
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

save_input_position:
      LDA   input_pos
      STA   saved_pos
      LDA   input_pos+1
      STA   saved_pos+1
      RTS

restore_input_position:
      LDA   saved_pos
      STA   input_pos
      LDA   saved_pos+1
      STA   input_pos+1
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

try_word_array:
      LDA   input_pos
      STA   lookahead_pos
      LDA   input_pos+1
      STA   lookahead_pos+1
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
ir_routine_end:  .byte ".O2E ", 0
ir_inline_end:   .byte ".O2I ", 0
ir_function_entry: .byte ".O2K", 0
ir_function_entry_line: .byte ".O2K", $0A, 0
ir_function_store: .byte ".O2V", 0
ir_accumulate_byte: .byte ".O2AB ", 0
ir_zero_branch:   .byte ".O2Z ", 0
ir_fast_function_end: .byte ".O2E f", $0A, 0
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
fast_result_store: .byte "STA NVR3L", $0A, 0
fast_result_load: .byte "LDA NVR3L", $0A, 0
fast_result_return: .byte "LDA NVR3L", $0A, "RTS", $0A, 0
inline_function_return: .byte "PLA", $0A, 0
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

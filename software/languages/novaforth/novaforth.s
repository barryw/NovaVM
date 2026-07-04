; Nova Forth v0.1 - Nova-native threaded Forth runtime.

      .setcpu "w65c02"

      .include "nova.inc"
      .include "libabi.inc"
      .include "libsystem.inc"
      .include "libeditor.inc"
      .include "replline.inc"

; Dictionary base = first free byte above ALL linked BSS. replline's input_buf
; now links from nova.lib and lands at the top of BSS, so the old `dict_start`
; label (end of this module's own BSS) no longer marks the true end of RAM
; reservations. Use the linker's __BSS_LAST__ so the dictionary never overlaps
; input_buf regardless of segment link order.
      .import __BSS_LAST__

.ifndef ROMSWAP_FORTH
ROMSWAP_FORTH = $06
.endif

DICT_LIMIT    = $9800
FLAG_IMM      = $80
LEN_MASK      = $1F
STACK_BYTES   = 128
RSTACK_BYTES  = 64
CELL_BYTES    = 2
PICTURED_SIZE = 34
SOURCE_STACK_DEPTH = 8
FIO_NAME_LIMIT = 63
ENV_KIND_SINGLE = 1
ENV_KIND_DOUBLE = 2
ENV_KIND_FLAG   = 3
FORTH_XRAM_STAGE_L = $00
FORTH_XRAM_STAGE_M = $00
EDITOR_HOOK_WORK = $9800
editor_hooks     = EDITOR_HOOK_WORK
editor_type      = EDITOR_HOOK_WORK + $10
editor_hook_code = EDITOR_HOOK_WORK + $30
editor_hook_tmp0 = EDITOR_HOOK_WORK + $03F0
editor_hook_tmp1 = EDITOR_HOOK_WORK + $03F1
editor_hook_tmp2 = EDITOR_HOOK_WORK + $03F2
editor_hook_tmp3 = EDITOR_HOOK_WORK + $03F3
editor_hook_tmp4 = EDITOR_HOOK_WORK + $03F4
editor_hook_tmp5 = EDITOR_HOOK_WORK + $03F5
editor_hook_tmp6 = EDITOR_HOOK_WORK + $03F6
editor_hook_tmp7 = EDITOR_HOOK_WORK + $03F7
EDITOR_HOOK_COLOR_WORD    = $63
EDITOR_HOOK_COLOR_NUMBER  = $67
EDITOR_HOOK_COLOR_COMMENT = $6C
.ifndef XRAM_WIN3_ENABLE
XRAM_WIN3_ENABLE = $08
.endif

; =====================================================================
; ZEROPAGE
; =====================================================================
      .segment "ZEROPAGE"

ptr_lo:        .res 1
ptr_hi:        .res 1
tok_idx:       .res 1
tok_idx_hi:    .res 1
tok_start_lo:  .res 1
tok_start_hi:  .res 1
tok_len:       .res 1
state:         .res 1
abort_flag:    .res 1
dsp:           .res 1
rsp:           .res 1
here_lo:       .res 1
here_hi:       .res 1
latest_lo:     .res 1
latest_hi:     .res 1
current_def_lo:.res 1
current_def_hi:.res 1
ip_lo:         .res 1
ip_hi:         .res 1
exec_hdr_lo:   .res 1
exec_hdr_hi:   .res 1
pfa_lo:        .res 1
pfa_hi:        .res 1
xt_lo:         .res 1
xt_hi:         .res 1
tmp_lo:        .res 1
tmp_hi:        .res 1
tmp2_lo:       .res 1
tmp2_hi:       .res 1
value_lo:      .res 1
value_hi:      .res 1
num_lo:        .res 1
num_hi:        .res 1
num_tmp_lo:    .res 1
num_tmp_hi:    .res 1
found_flags:   .res 1
parse_idx:     .res 1
parse_base:    .res 1
parse_digits:  .res 1
sign_flag:     .res 1
char_tmp:      .res 1

      .export repl_line_complete

; =====================================================================
; BSS
; =====================================================================
      .segment "BSS"

dstack:        .res STACK_BYTES
rstack:        .res RSTACK_BYTES
base_cell:     .res 2
pn_buf:        .res 5
word_buf:      .res 256
sbackslash_buf0:.res 128
sbackslash_buf1:.res 128
sbackslash_toggle:.res 1
arith0:       .res 1
arith1:       .res 1
arith2:       .res 1
arith3:       .res 1
arith4:       .res 1
arith5:       .res 1
arith6:       .res 1
arith7:       .res 1
pictured_idx: .res 1
pictured_buf: .res 34
loop_cs_sp:   .res 1
loop_start_lo:.res 8
loop_start_hi:.res 8
loop_leave_lo:.res 8
loop_leave_hi:.res 8
source_depth: .res 1
source_addr_lo:.res 1
source_addr_hi:.res 1
source_len:   .res 1
source_id_lo: .res 1
source_id_hi: .res 1
source_save_idx_lo:.res SOURCE_STACK_DEPTH
source_save_idx_hi:.res SOURCE_STACK_DEPTH
source_save_len:.res SOURCE_STACK_DEPTH
source_save_addr_lo:.res SOURCE_STACK_DEPTH
source_save_addr_hi:.res SOURCE_STACK_DEPTH
source_save_id_lo:.res SOURCE_STACK_DEPTH
source_save_id_hi:.res SOURCE_STACK_DEPTH
source_save_buf:.res SOURCE_STACK_DEPTH * 128
xram_src_lo:  .res 1
xram_src_mid: .res 1
xram_src_hi:  .res 1
noname_pending:.res 1
paren_skip:   .res 1
file_line_lo: .res 1
file_line_mid:.res 1
file_line_hi: .res 1
file_line_rem_lo:.res 1
file_line_rem_hi:.res 1
restore_pending:.res 1
restore_keep_current:.res 1
restore_resume_pending:.res 1
restore_tok_lo:.res 1
restore_tok_hi:.res 1
restore_xram_lo:.res 1
restore_xram_mid:.res 1
restore_xram_hi:.res 1
restore_rem_lo:.res 1
restore_rem_hi:.res 1
; Dictionary starts at __BSS_LAST__ (see cold_start) — the true top of BSS after
; the linker appends nova.lib's input_buf. No label here: a label at this point
; would only mark the end of THIS module's BSS, which is below input_buf.

; =====================================================================
; CODE
; =====================================================================
      .segment "CODE"

cold_start:
      CLD
      LDX   #$FF
      TXS

      ; The shared autoboot launcher LOADRUNTIMEs us into the PRIMARY $C000 bank
      ; and enters with REG_ROMSWAP = ROMSWAP_PRIMARY, so that is the bank we run
      ; in and the one lib_call must restore after a module call. (Using
      ; ROMSWAP_FORTH here left the FILES module mapped at $C000 on return -> the
      ; CPU RTS'd into module code and hung on the first lib call, e.g. PWD/DIR.)
      LDA   #ROMSWAP_PRIMARY
      STA   LIB_HOME_BANK
      LDA   #MODULE_ID_NONE
      STA   LIB_RESIDENT

      LDA   #VCMD_GCLS
      STA   VGC_CMD

      STZ   VGC_CURSEN
      STZ   state
      STZ   abort_flag
      STZ   dsp
      STZ   rsp
      STZ   ip_lo
      STZ   ip_hi
      STZ   current_def_lo
      STZ   current_def_hi
      STZ   loop_cs_sp
      STZ   source_depth
      STZ   source_id_lo
      STZ   source_id_hi
      STZ   noname_pending
      STZ   paren_skip
      STZ   restore_pending
      STZ   restore_keep_current
      STZ   restore_resume_pending
      STZ   sbackslash_toggle
      LDA   #10
      STA   base_cell
      STZ   base_cell+1

      LDA   #<__BSS_LAST__
      STA   here_lo
      LDA   #>__BSS_LAST__
      STA   here_hi
      LDA   #<ROM_LATEST
      STA   latest_lo
      LDA   #>ROM_LATEST
      STA   latest_hi

      JSR   repl_init
      JSR   autoload_libraries
      JSR   print_banner
      JSR   print_prompt

main_loop:
      JSR   repl_read_line
      STZ   VGC_CURSEN
      STZ   abort_flag
      JSR   set_input_buf_source
      STZ   source_id_lo
      STZ   source_id_hi
      JSR   interpret_line
      JSR   print_prompt
      BRA   main_loop

repl_line_complete:
      LDA   #0
      RTS

autoload_libraries:
      LDA   #<autoload_core_name
      STA   ptr_lo
      LDA   #>autoload_core_name
      STA   ptr_hi
      LDA   #autoload_core_name_len
      STA   char_tmp
      JSR   load_and_evaluate_forth_file
      STZ   abort_flag
      LDA   #<autoload_autoexec_name
      STA   ptr_lo
      LDA   #>autoload_autoexec_name
      STA   ptr_hi
      LDA   #autoload_autoexec_name_len
      STA   char_tmp
      JSR   load_and_evaluate_forth_file
      STZ   abort_flag
      RTS

print_banner:
      JSR   print_inl
      .byte "Nova Forth v0.1", $0D, $0A, 0
      SEC
      LDA   #<DICT_LIMIT
      SBC   here_lo
      STA   value_lo
      LDA   #>DICT_LIMIT
      SBC   here_hi
      STA   value_hi
      JSR   print_uint16
      JSR   print_inl
      .byte " BYTES FREE", $0D, $0A, $0D, $0A, 0
      RTS

print_prompt:
      JSR   print_inl
      .byte "OK> ", 0
      LDA   #$01
      STA   VGC_CURSEN
      RTS

interpret_line:
      LDA   restore_resume_pending
      BEQ   @fresh
      LDA   restore_tok_lo
      STA   tok_idx
      LDA   restore_tok_hi
      STA   tok_idx_hi
      STZ   restore_resume_pending
      BRA   @check_paren
@fresh:
      STZ   tok_idx
      STZ   tok_idx_hi
@check_paren:
      LDA   paren_skip
      BEQ   @loop
      JSR   skip_paren_in_line
      BCC   @done
@loop:
      LDA   abort_flag
      BNE   @done
      JSR   next_token
      BCC   @done
      JSR   interpret_token
      BRA   @loop
@done:
      RTS

interpret_token:
      JSR   find_word
      BCC   @try_number
      LDA   state
      BEQ   @exec
      LDA   found_flags
      AND   #FLAG_IMM
      BNE   @exec
      JMP   compile_exec_header
@exec:
      JSR   execute_header
      RTS
@try_number:
      JSR   parse_number
      BCC   @unknown
      LDA   state
      BEQ   @push_number
      JMP   compile_literal
@push_number:
      JMP   push_value
@unknown:
      JMP   emit_unknown_word

next_token:
      LDX   tok_idx
@skip:
      LDA   input_buf,X
      BEQ   @none
      CMP   #' '
      BEQ   @skip_one
      CMP   #$09
      BEQ   @skip_one
      CMP   #$0A
      BEQ   @skip_one
      CMP   #$0D
      BNE   @start
@skip_one:
      INX
      BRA   @skip
@start:
      TXA
      CLC
      ADC   #<input_buf
      STA   tok_start_lo
      LDA   #>input_buf
      ADC   #0
      STA   tok_start_hi
      STZ   tok_len
@scan:
      LDA   input_buf,X
      BEQ   @finish
      CMP   #' '
      BEQ   @finish
      CMP   #$09
      BEQ   @finish
      CMP   #$0A
      BEQ   @finish
      CMP   #$0D
      BEQ   @finish
      INX
      INC   tok_len
      BRA   @scan
@finish:
      LDA   input_buf,X
      BEQ   @store_idx
      INX
@store_idx:
      STX   tok_idx
      STZ   tok_idx_hi
      SEC
      RTS
@none:
      STX   tok_idx
      STZ   tok_idx_hi
      CLC
      RTS

find_word:
      LDA   latest_lo
      STA   ptr_lo
      LDA   latest_hi
      STA   ptr_hi
@loop:
      LDA   ptr_lo
      ORA   ptr_hi
      BEQ   @not_found
      LDY   #2
      LDA   (ptr_lo),Y
      STA   found_flags
      AND   #LEN_MASK
      CMP   tok_len
      BNE   @next
      LDX   #0
@cmp:
      CPX   tok_len
      BEQ   @found
      TXA
      CLC
      ADC   #3
      TAY
      LDA   (ptr_lo),Y
      STA   char_tmp
      TXA
      TAY
      LDA   (tok_start_lo),Y
      CMP   #'a'
      BCC   @compare
      CMP   #'z'+1
      BCS   @compare
      AND   #$DF
@compare:
      CMP   char_tmp
      BNE   @next
      INX
      BRA   @cmp
@found:
      LDA   ptr_lo
      STA   exec_hdr_lo
      LDA   ptr_hi
      STA   exec_hdr_hi
      SEC
      RTS
@next:
      LDY   #0
      LDA   (ptr_lo),Y
      STA   tmp_lo
      INY
      LDA   (ptr_lo),Y
      STA   tmp_hi
      LDA   tmp_lo
      STA   ptr_lo
      LDA   tmp_hi
      STA   ptr_hi
      BRA   @loop
@not_found:
      CLC
      RTS

emit_unknown_word:
      JSR   print_inl
      .byte "UNKNOWN WORD: ", 0
      JSR   print_token
      JSR   newline
      JMP   raise_error

need_word_name:
      JSR   print_inl
      .byte "WORD NAME REQUIRED", $0D, $0A, 0
      JSR   raise_error
      SEC
      RTS

parse_known_word:
      JSR   next_token
      BCS   @have_name
      JMP   need_word_name
@have_name:
      JSR   find_word
      BCS   @found
      JMP   emit_unknown_word
@found:
      CLC
      RTS

header_to_value:
      LDA   exec_hdr_lo
      STA   value_lo
      LDA   exec_hdr_hi
      STA   value_hi
      RTS

value_to_exec_header:
      LDA   value_lo
      STA   exec_hdr_lo
      LDA   value_hi
      STA   exec_hdr_hi
      RTS

here_to_value:
      LDA   here_lo
      STA   value_lo
      LDA   here_hi
      STA   value_hi
      RTS

value_to_tmp:
      LDA   value_lo
      STA   tmp_lo
      LDA   value_hi
      STA   tmp_hi
      RTS

value_to_tmp2:
      LDA   value_lo
      STA   tmp2_lo
      LDA   value_hi
      STA   tmp2_hi
      RTS

tmp_to_value:
      LDA   tmp_lo
      STA   value_lo
      LDA   tmp_hi
      STA   value_hi
      RTS

tmp2_to_value:
      LDA   tmp2_lo
      STA   value_lo
      LDA   tmp2_hi
      STA   value_hi
      RTS

value_to_ptr:
      LDA   value_lo
      STA   ptr_lo
      LDA   value_hi
      STA   ptr_hi
      RTS

ptr_to_value:
      LDA   ptr_lo
      STA   value_lo
      LDA   ptr_hi
      STA   value_hi
      RTS

zero_value:
      STZ   value_lo
      STZ   value_hi
      RTS

compile_word_lit:
      LDA   #<word_lit
      STA   exec_hdr_lo
      LDA   #>word_lit
      STA   exec_hdr_hi
      JMP   compile_exec_header

compile_word_branch:
      LDA   #<word_branch
      STA   exec_hdr_lo
      LDA   #>word_branch
      STA   exec_hdr_hi
      JMP   compile_exec_header

compile_word_0branch:
      LDA   #<word_0branch
      STA   exec_hdr_lo
      LDA   #>word_0branch
      STA   exec_hdr_hi
      JMP   compile_exec_header

compile_zero_cell:
      JSR   zero_value
      JMP   compile_value_cell

compile_placeholder:
      JSR   here_to_value
      JSR   push_value
      JMP   compile_zero_cell

execute_header:
      LDA   exec_hdr_lo
      STA   ptr_lo
      LDA   exec_hdr_hi
      STA   ptr_hi
      LDY   #2
      LDA   (ptr_lo),Y
      AND   #LEN_MASK
      CLC
      ADC   #3
      CLC
      ADC   ptr_lo
      STA   tmp_lo
      LDA   ptr_hi
      ADC   #0
      STA   tmp_hi
      LDY   #0
      LDA   (tmp_lo),Y
      STA   xt_lo
      INY
      LDA   (tmp_lo),Y
      STA   xt_hi
      CLC
      LDA   tmp_lo
      ADC   #2
      STA   pfa_lo
      LDA   tmp_hi
      ADC   #0
      STA   pfa_hi
      JMP   (xt_lo)

compile_exec_header:
      JSR   ensure_dict_2
      LDA   abort_flag
      BNE   @done
      LDY   #0
      LDA   exec_hdr_lo
      STA   (here_lo),Y
      INY
      LDA   exec_hdr_hi
      STA   (here_lo),Y
      JSR   here_plus_2
@done:
      RTS

compile_literal:
      JSR   compile_word_lit
      LDA   abort_flag
      BNE   @done
      JSR   compile_value_cell
@done:
      RTS

compile_value_cell:
      JSR   ensure_dict_2
      LDA   abort_flag
      BNE   @done
      LDY   #0
      LDA   value_lo
      STA   (here_lo),Y
      INY
      LDA   value_hi
      STA   (here_lo),Y
      JSR   here_plus_2
@done:
      RTS

compile_byte_value:
      LDA   #1
      JSR   ensure_dict_a
      LDA   abort_flag
      BNE   @done
      LDY   #0
      LDA   value_lo
      STA   (here_lo),Y
      INC   here_lo
      BNE   @done
      INC   here_hi
@done:
      RTS

patch_value_to_here:
      LDA   value_lo
      STA   ptr_lo
      LDA   value_hi
      STA   ptr_hi
      LDY   #0
      LDA   here_lo
      STA   (ptr_lo),Y
      INY
      LDA   here_hi
      STA   (ptr_lo),Y
      RTS

require_compiling:
      LDA   state
      BNE   @ok
      JSR   print_inl
      .byte "COMPILE-ONLY WORD", $0D, $0A, 0
      JSR   raise_error
      SEC
      RTS
@ok:
      CLC
      RTS

require_definition_context:
      LDA   state
      BNE   @ok
      LDA   current_def_lo
      ORA   current_def_hi
      BNE   @ok
      JSR   print_inl
      .byte "COMPILE-ONLY WORD", $0D, $0A, 0
      JSR   raise_error
      SEC
      RTS
@ok:
      CLC
      RTS

create_header:
      LDA   tok_len
      BEQ   @bad
      CMP   #32
      BCS   @bad
      CLC
      ADC   #5
      JSR   ensure_dict_a
      LDA   abort_flag
      BNE   @done

      LDA   here_lo
      STA   exec_hdr_lo
      STA   ptr_lo
      LDA   here_hi
      STA   exec_hdr_hi
      STA   ptr_hi

      LDY   #0
      LDA   latest_lo
      STA   (ptr_lo),Y
      INY
      LDA   latest_hi
      STA   (ptr_lo),Y
      INY
      LDA   tok_len
      STA   (ptr_lo),Y

      LDX   #0
@copy_name:
      CPX   tok_len
      BEQ   @write_code
      TXA
      TAY
      LDA   (tok_start_lo),Y
      STA   char_tmp
      CMP   #'a'
      BCC   @store_name
      CMP   #'z'+1
      BCS   @store_name
      AND   #$DF
      STA   char_tmp
@store_name:
      TXA
      CLC
      ADC   #3
      TAY
      LDA   char_tmp
      STA   (ptr_lo),Y
      INX
      BRA   @copy_name

@write_code:
      LDA   tok_len
      CLC
      ADC   #3
      TAY
      LDA   xt_lo
      STA   (ptr_lo),Y
      INY
      LDA   xt_hi
      STA   (ptr_lo),Y

      LDA   exec_hdr_lo
      STA   latest_lo
      LDA   exec_hdr_hi
      STA   latest_hi

      LDA   tok_len
      CLC
      ADC   #5
      JSR   here_plus_a
      CLC
@done:
      RTS
@bad:
      JSR   print_inl
      .byte "BAD NAME", $0D, $0A, 0
      JMP   raise_error

ensure_dict_2:
      LDA   #2
ensure_dict_a:
      CLC
      ADC   here_lo
      STA   tmp_lo
      LDA   here_hi
      ADC   #0
      STA   tmp_hi
      LDA   tmp_hi
      CMP   #>DICT_LIMIT
      BCC   @ok
      BNE   @full
      LDA   tmp_lo
      CMP   #<DICT_LIMIT
      BCC   @ok
@full:
      JSR   print_inl
      .byte "DICTIONARY FULL", $0D, $0A, 0
      JMP   raise_error
@ok:
      RTS

here_plus_2:
      LDA   #2
here_plus_a:
      CLC
      ADC   here_lo
      STA   here_lo
      BCC   @done
      INC   here_hi
@done:
      RTS

push_value:
      LDX   dsp
      CPX   #STACK_BYTES
      BCC   @ok
      JMP   stack_overflow_error
@ok:
      LDA   value_lo
      STA   dstack,X
      INX
      LDA   value_hi
      STA   dstack,X
      INX
      STX   dsp
      RTS

pop_value:
      LDX   dsp
      CPX   #2
      BCS   @ok
      JMP   stack_underflow_error
@ok:
      DEX
      LDA   dstack,X
      STA   value_hi
      DEX
      LDA   dstack,X
      STA   value_lo
      STX   dsp
      CLC
      RTS

rpush_value:
      LDX   rsp
      CPX   #RSTACK_BYTES
      BCC   @ok
      JMP   return_stack_overflow_error
@ok:
      LDA   value_lo
      STA   rstack,X
      INX
      LDA   value_hi
      STA   rstack,X
      INX
      STX   rsp
      CLC
      RTS

rpop_value:
      LDX   rsp
      CPX   #2
      BCS   @ok
      JMP   return_stack_underflow_error
@ok:
      DEX
      LDA   rstack,X
      STA   value_hi
      DEX
      LDA   rstack,X
      STA   value_lo
      STX   rsp
      CLC
      RTS

rpeek_value:
      LDX   rsp
      CPX   #2
      BCS   @ok
      JMP   return_stack_underflow_error
@ok:
      LDA   rstack-2,X
      STA   value_lo
      LDA   rstack-1,X
      STA   value_hi
      CLC
      RTS

pop_value_to_ptr:
      JSR   pop_value
      BCS   @err
      LDA   value_lo
      STA   ptr_lo
      LDA   value_hi
      STA   ptr_hi
      CLC
      RTS
@err:
      SEC
      RTS

tmp2_is_zero:
      LDA   tmp2_lo
      ORA   tmp2_hi
      RTS

dec_tmp2:
      LDA   tmp2_lo
      BNE   @lo
      DEC   tmp2_hi
@lo:
      DEC   tmp2_lo
      RTS

inc_ptr:
      INC   ptr_lo
      BNE   @done
      INC   ptr_hi
@done:
      RTS

dec_ptr:
      LDA   ptr_lo
      BNE   @lo
      DEC   ptr_hi
@lo:
      DEC   ptr_lo
      RTS

inc_tmp:
      INC   tmp_lo
      BNE   @done
      INC   tmp_hi
@done:
      RTS

dec_tmp:
      LDA   tmp_lo
      BNE   @lo
      DEC   tmp_hi
@lo:
      DEC   tmp_lo
      RTS

add_tmp2_to_ptr:
      CLC
      LDA   ptr_lo
      ADC   tmp2_lo
      STA   ptr_lo
      LDA   ptr_hi
      ADC   tmp2_hi
      STA   ptr_hi
      RTS

add_tmp2_to_tmp:
      CLC
      LDA   tmp_lo
      ADC   tmp2_lo
      STA   tmp_lo
      LDA   tmp_hi
      ADC   tmp2_hi
      STA   tmp_hi
      RTS

stack_overflow_error:
      JSR   print_inl
      .byte "STACK OVERFLOW", $0D, $0A, 0
      JSR   raise_error
      SEC
      RTS

stack_underflow_error:
      JSR   print_inl
      .byte "STACK UNDERFLOW", $0D, $0A, 0
      JSR   raise_error
      SEC
      RTS

return_stack_overflow_error:
      JSR   print_inl
      .byte "RETURN STACK OVERFLOW", $0D, $0A, 0
      JSR   raise_error
      SEC
      RTS

return_stack_underflow_error:
      JSR   print_inl
      .byte "RETURN STACK UNDERFLOW", $0D, $0A, 0
      JSR   raise_error
      SEC
      RTS

source_nesting_error:
      JSR   print_inl
      .byte "SOURCE NESTING TOO DEEP", $0D, $0A, 0
      JSR   raise_error
      SEC
      RTS

source_line_too_long_error:
      JSR   print_inl
      .byte "SOURCE LINE TOO LONG", $0D, $0A, 0
      JSR   raise_error
      SEC
      RTS

include_failed_error:
      JSR   print_inl
      .byte "INCLUDE FAILED", $0D, $0A, 0
      JSR   raise_error
      SEC
      RTS

edit_failed_error:
      JSR   print_inl
      .byte "EDIT FAILED", $0D, $0A, 0
      JSR   raise_error
      SEC
      RTS

raise_error:
      LDA   #1
      STA   abort_flag
      STZ   state
      STZ   dsp
      STZ   rsp
      STZ   ip_lo
      STZ   ip_hi
      RTS

; ---------------------------------------------------------------------
; Colon execution
; ---------------------------------------------------------------------
code_docol:
      LDA   ip_lo
      STA   value_lo
      LDA   ip_hi
      STA   value_hi
      JSR   rpush_value
      BCS   @done
      LDA   pfa_lo
      STA   ip_lo
      LDA   pfa_hi
      STA   ip_hi
      JMP   next_thread
@done:
      RTS

next_thread:
      LDA   abort_flag
      BNE   @done
      LDA   ip_lo
      ORA   ip_hi
      BEQ   @done
      LDY   #0
      LDA   (ip_lo),Y
      STA   exec_hdr_lo
      INY
      LDA   (ip_lo),Y
      STA   exec_hdr_hi
      CLC
      LDA   ip_lo
      ADC   #2
      STA   ip_lo
      BCC   @exec
      INC   ip_hi
@exec:
      JSR   execute_header
      BRA   next_thread
@done:
      RTS

code_exit:
      LDX   rsp
      CPX   #2
      BCS   @ok
      STZ   ip_lo
      STZ   ip_hi
      RTS
@ok:
      DEX
      LDA   rstack,X
      STA   ip_hi
      DEX
      LDA   rstack,X
      STA   ip_lo
      STX   rsp
      PLA
      PLA
      RTS

code_lit:
      LDY   #0
      LDA   (ip_lo),Y
      STA   value_lo
      INY
      LDA   (ip_lo),Y
      STA   value_hi
      CLC
      LDA   ip_lo
      ADC   #2
      STA   ip_lo
      BCC   @push
      INC   ip_hi
@push:
      JMP   push_value

code_branch:
      LDY   #0
      LDA   (ip_lo),Y
      STA   tmp_lo
      INY
      LDA   (ip_lo),Y
      STA   tmp_hi
      LDA   tmp_lo
      STA   ip_lo
      LDA   tmp_hi
      STA   ip_hi
      RTS

code_0branch:
      LDY   #0
      LDA   (ip_lo),Y
      STA   tmp_lo
      INY
      LDA   (ip_lo),Y
      STA   tmp_hi
      JSR   pop_value
      BCS   @done
      LDA   value_lo
      ORA   value_hi
      BEQ   @branch
      CLC
      LDA   ip_lo
      ADC   #2
      STA   ip_lo
      BCC   @done
      INC   ip_hi
@done:
      RTS
@branch:
      LDA   tmp_lo
      STA   ip_lo
      LDA   tmp_hi
      STA   ip_hi
      RTS

read_inline_target:
      LDY   #0
      LDA   (ip_lo),Y
      STA   tmp_lo
      INY
      LDA   (ip_lo),Y
      STA   tmp_hi
      RTS

skip_inline_cell:
      CLC
      LDA   ip_lo
      ADC   #2
      STA   ip_lo
      BCC   @done
      INC   ip_hi
@done:
      RTS

branch_to_tmp:
      LDA   tmp_lo
      STA   ip_lo
      LDA   tmp_hi
      STA   ip_hi
      RTS

code_do_runtime:
      JSR   pop_value            ; start
      BCS   @done
      JSR   value_to_tmp
      JSR   pop_value            ; limit
      BCS   @done
      JSR   rpush_value
      BCS   @done
      JSR   tmp_to_value
      JSR   rpush_value
@done:
      RTS

code_qdo_runtime:
      JSR   read_inline_target
      JSR   pop_value            ; start
      BCS   @done
      LDA   value_lo
      STA   arith0
      LDA   value_hi
      STA   arith1
      JSR   pop_value            ; limit
      BCS   @done
      LDA   value_lo
      CMP   arith0
      BNE   @enter
      LDA   value_hi
      CMP   arith1
      BNE   @enter
      JMP   branch_to_tmp
@enter:
      JSR   rpush_value
      BCS   @done
      LDA   arith0
      STA   value_lo
      LDA   arith1
      STA   value_hi
      JSR   rpush_value
      BCS   @done
      JMP   skip_inline_cell
@done:
      RTS

code_loop_runtime:
      JSR   read_inline_target
      LDX   rsp
      CPX   #4
      BCS   @ok
      JMP   return_stack_underflow_error
@ok:
      INC   rstack-2,X
      BNE   @compare
      INC   rstack-1,X
@compare:
      LDA   rstack-2,X
      CMP   rstack-4,X
      BNE   @branch
      LDA   rstack-1,X
      CMP   rstack-3,X
      BNE   @branch
      TXA
      SEC
      SBC   #4
      STA   rsp
      JMP   skip_inline_cell
@branch:
      JMP   branch_to_tmp

code_plus_loop_runtime:
      JSR   read_inline_target
      LDA   tmp_lo
      STA   ptr_lo
      LDA   tmp_hi
      STA   ptr_hi
      JSR   pop_value            ; increment
      BCC   @got_increment
      RTS
@got_increment:
      LDA   value_lo
      STA   tmp2_lo
      LDA   value_hi
      STA   tmp2_hi
      LDX   rsp
      CPX   #4
      BCS   @ok
      JMP   return_stack_underflow_error
@ok:
      LDA   rstack-2,X           ; old index
      STA   arith0
      LDA   rstack-1,X
      STA   arith1
      CLC
      LDA   arith0
      ADC   tmp2_lo
      STA   rstack-2,X
      STA   arith2               ; new index
      LDA   arith1
      ADC   tmp2_hi
      STA   rstack-1,X
      STA   arith3

      LDA   tmp2_hi
      BMI   @negative_step
      LDA   arith0
      STA   value_lo
      LDA   arith1
      STA   value_hi
      LDA   rstack-4,X
      STA   tmp_lo
      LDA   rstack-3,X
      STA   tmp_hi
      JSR   signed_less_value_tmp
      BCC   @branch
      LDA   arith2
      STA   value_lo
      LDA   arith3
      STA   value_hi
      JSR   signed_less_value_tmp
      BCC   @exit
      BRA   @branch

@negative_step:
      LDA   arith0
      STA   value_lo
      LDA   arith1
      STA   value_hi
      LDA   rstack-4,X
      STA   tmp_lo
      LDA   rstack-3,X
      STA   tmp_hi
      JSR   signed_less_value_tmp
      BCS   @branch
      LDA   arith2
      STA   value_lo
      LDA   arith3
      STA   value_hi
      JSR   signed_less_value_tmp
      BCS   @exit
@branch:
      LDA   ptr_lo
      STA   tmp_lo
      LDA   ptr_hi
      STA   tmp_hi
      JMP   branch_to_tmp
@exit:
      TXA
      SEC
      SBC   #4
      STA   rsp
      JMP   skip_inline_cell
@done:
      RTS

code_i:
      LDX   rsp
      CPX   #4
      BCS   @ok
      JMP   return_stack_underflow_error
@ok:
      LDA   rstack-2,X
      STA   value_lo
      LDA   rstack-1,X
      STA   value_hi
      JMP   push_value

code_j:
      LDX   rsp
      CPX   #8
      BCS   @ok
      JMP   return_stack_underflow_error
@ok:
      LDA   rstack-6,X
      STA   value_lo
      LDA   rstack-5,X
      STA   value_hi
      JMP   push_value

code_unloop:
      LDX   rsp
      CPX   #4
      BCS   @ok
      JMP   return_stack_underflow_error
@ok:
      TXA
      SEC
      SBC   #4
      STA   rsp
      RTS

code_dot_quote_runtime:
      LDY   #0
      LDA   (ip_lo),Y
      STA   char_tmp
      CLC
      LDA   ip_lo
      ADC   #1
      STA   ip_lo
      BCC   @print
      INC   ip_hi
@print:
      LDY   #0
@loop:
      CPY   char_tmp
      BCS   @advance
      LDA   (ip_lo),Y
      STA   VGC_CHAROUT
      INY
      BRA   @loop
@advance:
      CLC
      LDA   ip_lo
      ADC   char_tmp
      STA   ip_lo
      BCC   @done
      INC   ip_hi
@done:
      RTS

code_s_quote_runtime:
      LDY   #0
      LDA   (ip_lo),Y
      STA   char_tmp
      CLC
      LDA   ip_lo
      ADC   #1
      STA   ip_lo
      BCC   @push_addr
      INC   ip_hi
@push_addr:
      LDA   ip_lo
      STA   value_lo
      LDA   ip_hi
      STA   value_hi
      JSR   push_value
      LDA   char_tmp
      STA   value_lo
      STZ   value_hi
      JSR   push_value
      CLC
      LDA   ip_lo
      ADC   char_tmp
      STA   ip_lo
      BCC   @done
      INC   ip_hi
@done:
      RTS

code_c_quote_runtime:
      LDA   ip_lo
      STA   value_lo
      LDA   ip_hi
      STA   value_hi
      JSR   push_value
      LDY   #0
      LDA   (ip_lo),Y
      STA   char_tmp
      CLC
      LDA   ip_lo
      ADC   char_tmp
      STA   ip_lo
      BCC   @add_count
      INC   ip_hi
@add_count:
      INC   ip_lo
      BNE   @done
      INC   ip_hi
@done:
      RTS

code_abort_quote_runtime:
      LDY   #0
      LDA   (ip_lo),Y
      STA   char_tmp
      CLC
      LDA   ip_lo
      ADC   #1
      STA   ip_lo
      BCC   @check
      INC   ip_hi
@check:
      JSR   pop_value
      BCS   @skip
      LDA   value_lo
      ORA   value_hi
      BEQ   @skip
      LDY   #0
@print:
      CPY   char_tmp
      BCS   @abort
      LDA   (ip_lo),Y
      STA   VGC_CHAROUT
      INY
      BRA   @print
@abort:
      JSR   newline
      JMP   raise_error
@skip:
      CLC
      LDA   ip_lo
      ADC   char_tmp
      STA   ip_lo
      BCC   @done
      INC   ip_hi
@done:
      RTS

code_created:
      CLC
      LDA   pfa_lo
      ADC   #2
      STA   value_lo
      LDA   pfa_hi
      ADC   #0
      STA   value_hi
      JMP   push_value

code_marker_runtime:
      LDY   #0
      LDA   (pfa_lo),Y
      STA   here_lo
      INY
      LDA   (pfa_lo),Y
      STA   here_hi
      INY
      LDA   (pfa_lo),Y
      STA   latest_lo
      INY
      LDA   (pfa_lo),Y
      STA   latest_hi
      STZ   current_def_lo
      STZ   current_def_hi
      STZ   noname_pending
      RTS

code_dodoes:
      CLC
      LDA   pfa_lo
      ADC   #2
      STA   value_lo
      LDA   pfa_hi
      ADC   #0
      STA   value_hi
      JSR   push_value
      LDA   ip_lo
      STA   value_lo
      LDA   ip_hi
      STA   value_hi
      JSR   rpush_value
      BCS   @done
      LDY   #0
      LDA   (pfa_lo),Y
      STA   ip_lo
      INY
      LDA   (pfa_lo),Y
      STA   ip_hi
      JMP   next_thread
@done:
      RTS

; ---------------------------------------------------------------------
; Compile-time control flow
; ---------------------------------------------------------------------
code_if:
      JSR   require_compiling
      BCS   @done
      JSR   compile_word_0branch
      LDA   abort_flag
      BNE   @done
      JSR   compile_placeholder
@done:
      RTS

code_else:
      JSR   require_compiling
      BCS   @done
      JSR   compile_word_branch
      LDA   abort_flag
      BNE   @done
      JSR   here_to_value
      JSR   value_to_tmp2
      JSR   compile_zero_cell
      LDA   abort_flag
      BNE   @done
      JSR   pop_value
      BCS   @done
      JSR   patch_value_to_here
      JSR   tmp2_to_value
      JSR   push_value
@done:
      RTS

code_then:
      JSR   require_compiling
      BCS   @done
      JSR   pop_value
      BCS   @done
      JSR   patch_value_to_here
@done:
      RTS

code_begin:
      JSR   require_compiling
      BCS   @done
      JSR   here_to_value
      JSR   push_value
@done:
      RTS

code_until:
      JSR   require_compiling
      BCS   @done
      JSR   pop_value
      BCS   @done
      JSR   value_to_tmp2
      JSR   compile_word_0branch
      LDA   abort_flag
      BNE   @done
      JSR   tmp2_to_value
      JSR   compile_value_cell
@done:
      RTS

code_again:
      JSR   require_compiling
      BCS   @done
      JSR   pop_value
      BCS   @done
      JSR   value_to_tmp2
      JSR   compile_word_branch
      LDA   abort_flag
      BNE   @done
      JSR   tmp2_to_value
      JSR   compile_value_cell
@done:
      RTS

code_while:
      JSR   require_compiling
      BCS   @done
      JSR   pop_value             ; BEGIN destination
      BCS   @done
      JSR   value_to_tmp2
      JSR   compile_word_0branch
      LDA   abort_flag
      BNE   @done
      JSR   here_to_value
      JSR   push_value            ; unresolved WHILE exit
      JSR   compile_zero_cell
      LDA   abort_flag
      BNE   @done
      JSR   tmp2_to_value         ; BEGIN destination
      JSR   push_value
@done:
      RTS

code_repeat:
      JSR   require_compiling
      BCS   @done
      JSR   pop_value             ; BEGIN destination
      BCS   @done
      JSR   value_to_tmp2
      JSR   pop_value             ; WHILE placeholder
      BCS   @done
      JSR   value_to_ptr
      JSR   compile_word_branch
      LDA   abort_flag
      BNE   @done
      JSR   tmp2_to_value
      JSR   compile_value_cell
      LDA   abort_flag
      BNE   @done
      JSR   ptr_to_value
      JSR   patch_value_to_here
@done:
      RTS

patch_leave_list:
      LDA   tmp2_lo
      ORA   tmp2_hi
      BEQ   @done
      LDA   tmp2_lo
      STA   value_lo
      STA   ptr_lo
      LDA   tmp2_hi
      STA   value_hi
      STA   ptr_hi
      LDY   #0
      LDA   (ptr_lo),Y
      STA   tmp2_lo
      INY
      LDA   (ptr_lo),Y
      STA   tmp2_hi
      JSR   patch_value_to_here
      BRA   patch_leave_list
@done:
      RTS

loop_control_error:
      JSR   print_inl
      .byte "LOOP CONTROL ERROR", $0D, $0A, 0
      JMP   raise_error

code_do:
      JSR   require_compiling
      BCS   @done
      LDX   loop_cs_sp
      CPX   #8
      BCC   @space
      JMP   loop_control_error
@space:
      LDA   #<word_do_runtime
      STA   exec_hdr_lo
      LDA   #>word_do_runtime
      STA   exec_hdr_hi
      JSR   compile_exec_header
      LDA   abort_flag
      BNE   @done
      JSR   here_to_value
      LDX   loop_cs_sp
      LDA   value_lo
      STA   loop_start_lo,X
      LDA   value_hi
      STA   loop_start_hi,X
      STZ   loop_leave_lo,X
      STZ   loop_leave_hi,X
      INC   loop_cs_sp
@done:
      RTS

code_qdo:
      JSR   require_compiling
      BCS   @done
      LDX   loop_cs_sp
      CPX   #8
      BCC   @space
      JMP   loop_control_error
@space:
      LDA   #<word_qdo_runtime
      STA   exec_hdr_lo
      LDA   #>word_qdo_runtime
      STA   exec_hdr_hi
      JSR   compile_exec_header
      LDA   abort_flag
      BNE   @done
      JSR   here_to_value
      LDA   value_lo
      STA   arith0
      LDA   value_hi
      STA   arith1
      JSR   compile_zero_cell
      LDA   abort_flag
      BNE   @done
      JSR   here_to_value
      LDX   loop_cs_sp
      LDA   value_lo
      STA   loop_start_lo,X
      LDA   value_hi
      STA   loop_start_hi,X
      LDA   arith0
      STA   loop_leave_lo,X
      LDA   arith1
      STA   loop_leave_hi,X
      INC   loop_cs_sp
@done:
      RTS

code_leave:
      JSR   require_compiling
      BCS   @done
      LDX   loop_cs_sp
      BNE   @have_loop
      JMP   loop_control_error
@have_loop:
      DEX
      LDA   loop_leave_lo,X
      STA   arith0
      LDA   loop_leave_hi,X
      STA   arith1
      LDA   #<word_unloop
      STA   exec_hdr_lo
      LDA   #>word_unloop
      STA   exec_hdr_hi
      JSR   compile_exec_header
      LDA   #<word_branch
      STA   exec_hdr_lo
      LDA   #>word_branch
      STA   exec_hdr_hi
      JSR   compile_exec_header
      LDA   abort_flag
      BNE   @done
      JSR   here_to_value
      LDA   value_lo
      STA   arith2
      LDA   value_hi
      STA   arith3
      LDA   arith0
      STA   value_lo
      LDA   arith1
      STA   value_hi
      JSR   compile_value_cell    ; link to previous leave
      LDA   arith2
      STA   value_lo
      LDA   arith3
      STA   value_hi
      LDX   loop_cs_sp
      DEX
      LDA   value_lo
      STA   loop_leave_lo,X
      LDA   value_hi
      STA   loop_leave_hi,X
@done:
      RTS

code_loop:
      JSR   require_compiling
      BCS   @done
      LDX   loop_cs_sp
      BNE   @have_loop
      JMP   loop_control_error
@have_loop:
      DEC   loop_cs_sp
      LDX   loop_cs_sp
      LDA   loop_leave_lo,X
      STA   arith0
      LDA   loop_leave_hi,X
      STA   arith1
      LDA   loop_start_lo,X
      STA   arith2
      LDA   loop_start_hi,X
      STA   arith3
      LDA   #<word_loop_runtime
      STA   exec_hdr_lo
      LDA   #>word_loop_runtime
      STA   exec_hdr_hi
      JSR   compile_exec_header
      LDA   arith2
      STA   value_lo
      LDA   arith3
      STA   value_hi
      JSR   compile_value_cell
      LDA   arith0
      STA   tmp2_lo
      LDA   arith1
      STA   tmp2_hi
      JSR   patch_leave_list
@done:
      RTS

code_plus_loop:
      JSR   require_compiling
      BCS   @done
      LDX   loop_cs_sp
      BNE   @have_loop
      JMP   loop_control_error
@have_loop:
      DEC   loop_cs_sp
      LDX   loop_cs_sp
      LDA   loop_leave_lo,X
      STA   arith0
      LDA   loop_leave_hi,X
      STA   arith1
      LDA   loop_start_lo,X
      STA   arith2
      LDA   loop_start_hi,X
      STA   arith3
      LDA   #<word_plus_loop_runtime
      STA   exec_hdr_lo
      LDA   #>word_plus_loop_runtime
      STA   exec_hdr_hi
      JSR   compile_exec_header
      LDA   arith2
      STA   value_lo
      LDA   arith3
      STA   value_hi
      JSR   compile_value_cell
      LDA   arith0
      STA   tmp2_lo
      LDA   arith1
      STA   tmp2_hi
      JSR   patch_leave_list
@done:
      RTS

; ---------------------------------------------------------------------
; Defining words
; ---------------------------------------------------------------------
code_colon:
      LDA   state
      BNE   @bad
      JSR   next_token
      BCC   @need_name
      LDA   #<code_docol
      STA   xt_lo
      LDA   #>code_docol
      STA   xt_hi
      JSR   create_header
      LDA   abort_flag
      BNE   @done
      LDA   exec_hdr_lo
      STA   current_def_lo
      STA   ptr_lo
      LDA   exec_hdr_hi
      STA   current_def_hi
      STA   ptr_hi
      LDY   #0
      LDA   (ptr_lo),Y
      STA   latest_lo
      INY
      LDA   (ptr_lo),Y
      STA   latest_hi
      LDA   #1
      STA   state
@done:
      RTS
@need_name:
      JMP   need_word_name
@bad:
      JSR   print_inl
      .byte "ALREADY COMPILING", $0D, $0A, 0
      JMP   raise_error

code_noname:
      LDA   state
      BNE   @bad
      LDA   #5
      JSR   ensure_dict_a
      LDA   abort_flag
      BNE   @done

      LDA   here_lo
      STA   exec_hdr_lo
      STA   current_def_lo
      STA   ptr_lo
      LDA   here_hi
      STA   exec_hdr_hi
      STA   current_def_hi
      STA   ptr_hi

      LDY   #0
      LDA   latest_lo
      STA   (ptr_lo),Y
      INY
      LDA   latest_hi
      STA   (ptr_lo),Y
      INY
      LDA   #0
      STA   (ptr_lo),Y
      INY
      LDA   #<code_docol
      STA   (ptr_lo),Y
      INY
      LDA   #>code_docol
      STA   (ptr_lo),Y

      LDA   #5
      JSR   here_plus_a
      LDA   #1
      STA   noname_pending
      STA   state
@done:
      RTS
@bad:
      JSR   print_inl
      .byte "ALREADY COMPILING", $0D, $0A, 0
      JMP   raise_error

code_semicolon:
      LDA   state
      BNE   @ok
      JSR   print_inl
      .byte "; OUTSIDE DEFINITION", $0D, $0A, 0
      JMP   raise_error
@ok:
      LDA   #<word_exit
      STA   exec_hdr_lo
      LDA   #>word_exit
      STA   exec_hdr_hi
      JSR   compile_exec_header
      LDA   noname_pending
      BNE   @noname
      LDA   current_def_lo
      STA   latest_lo
      LDA   current_def_hi
      STA   latest_hi
      STZ   current_def_lo
      STZ   current_def_hi
      STZ   state
      RTS
@noname:
      LDA   current_def_lo
      STA   value_lo
      LDA   current_def_hi
      STA   value_hi
      STZ   current_def_lo
      STZ   current_def_hi
      STZ   state
      STZ   noname_pending
      JMP   push_value

code_create:
      JSR   next_token
      BCC   @need_name
      LDA   #<code_created
      STA   xt_lo
      LDA   #>code_created
      STA   xt_hi
      JSR   create_header
      LDA   abort_flag
      BNE   @done
      JSR   ensure_dict_2
      LDY   #0
      LDA   #0
      STA   (here_lo),Y
      INY
      STA   (here_lo),Y
      JSR   here_plus_2
@done:
      RTS
@need_name:
      JMP   need_word_name

code_marker:
      JSR   next_token
      BCC   @need_name
      LDA   here_lo
      STA   arith0
      LDA   here_hi
      STA   arith1
      LDA   latest_lo
      STA   arith2
      LDA   latest_hi
      STA   arith3
      LDA   #<code_marker_runtime
      STA   xt_lo
      LDA   #>code_marker_runtime
      STA   xt_hi
      JSR   create_header
      LDA   abort_flag
      BNE   @done
      LDA   #4
      JSR   ensure_dict_a
      LDA   abort_flag
      BNE   @done
      LDY   #0
      LDA   arith0
      STA   (here_lo),Y
      INY
      LDA   arith1
      STA   (here_lo),Y
      INY
      LDA   arith2
      STA   (here_lo),Y
      INY
      LDA   arith3
      STA   (here_lo),Y
      LDA   #4
      JSR   here_plus_a
@done:
      RTS
@need_name:
      JMP   need_word_name

code_allot:
      JSR   pop_value
      BCS   @done
      LDA   value_hi
      BMI   @negative

      CLC
      LDA   here_lo
      ADC   value_lo
      STA   tmp_lo
      LDA   here_hi
      ADC   value_hi
      STA   tmp_hi
      BCS   @full
      LDA   tmp_hi
      CMP   #>DICT_LIMIT
      BCC   @store
      BNE   @full
      LDA   tmp_lo
      CMP   #<DICT_LIMIT
      BCC   @store
@full:
      JSR   print_inl
      .byte "DICTIONARY FULL", $0D, $0A, 0
      JMP   raise_error

@negative:
      CLC
      LDA   here_lo
      ADC   value_lo
      STA   tmp_lo
      LDA   here_hi
      ADC   value_hi
      STA   tmp_hi
      BCC   @bad
      LDA   tmp_hi
      CMP   #>__BSS_LAST__
      BCC   @bad
      BNE   @store
      LDA   tmp_lo
      CMP   #<__BSS_LAST__
      BCC   @bad
@store:
      LDA   tmp_lo
      STA   here_lo
      LDA   tmp_hi
      STA   here_hi
@done:
      RTS
@bad:
      JSR   print_inl
      .byte "BAD ALLOT", $0D, $0A, 0
      JMP   raise_error

code_here:
      LDA   here_lo
      STA   value_lo
      LDA   here_hi
      STA   value_hi
      JMP   push_value

code_unused:
      SEC
      LDA   #<DICT_LIMIT
      SBC   here_lo
      STA   value_lo
      LDA   #>DICT_LIMIT
      SBC   here_hi
      STA   value_hi
      JMP   push_value

code_comma:
      JSR   pop_value
      BCS   @done
      JSR   compile_value_cell
@done:
      RTS

code_ccomma:
      JSR   pop_value
      BCS   @done
      JSR   compile_byte_value
@done:
      RTS

code_base:
      LDA   #<base_cell
      STA   value_lo
      LDA   #>base_cell
      STA   value_hi
      JMP   push_value

code_state_word:
      LDA   #<state
      STA   value_lo
      STZ   value_hi
      JMP   push_value

code_source:
      LDA   source_addr_lo
      STA   value_lo
      LDA   source_addr_hi
      STA   value_hi
      JSR   push_value
      LDA   source_len
      STA   value_lo
      STZ   value_hi
      JMP   push_value

code_to_in:
      LDA   #<tok_idx
      STA   value_lo
      STZ   value_hi
      JMP   push_value

code_source_id:
      LDA   source_id_lo
      STA   value_lo
      LDA   source_id_hi
      STA   value_hi
      JMP   push_value

code_refill:
      LDA   source_id_lo
      CMP   #1
      BNE   @false
      LDA   source_id_hi
      BNE   @false
      JSR   refill_xram_line
      BCC   @false
      LDA   #1
      STA   restore_keep_current
      JMP   push_true
@false:
      JMP   push_false

code_save_input:
      LDA   source_id_lo
      CMP   #1
      BNE   @simple
      LDA   source_id_hi
      BNE   @simple
      LDA   tok_idx
      STA   value_lo
      LDA   tok_idx_hi
      STA   value_hi
      JSR   push_value
      LDA   file_line_lo
      STA   value_lo
      LDA   file_line_mid
      STA   value_hi
      JSR   push_value
      LDA   file_line_hi
      STA   value_lo
      STZ   value_hi
      JSR   push_value
      LDA   file_line_rem_lo
      STA   value_lo
      LDA   file_line_rem_hi
      STA   value_hi
      JSR   push_value
      LDA   #1
      STA   value_lo
      STZ   value_hi
      JSR   push_value
      LDA   #5
      STA   value_lo
      STZ   value_hi
      JMP   push_value
@simple:
      LDA   tok_idx
      STA   value_lo
      LDA   tok_idx_hi
      STA   value_hi
      JSR   push_value
      LDA   #1
      STA   value_lo
      STZ   value_hi
      JMP   push_value

code_restore_input:
      JSR   pop_value            ; save-item count
      BCS   @done
      LDA   value_hi
      BNE   @fail
      LDA   value_lo
      CMP   #1
      BEQ   @simple
      CMP   #5
      BEQ   @file
      BRA   @fail
@simple:
      JSR   pop_value            ; saved >IN
      BCS   @done
      LDA   value_lo
      STA   tok_idx
      LDA   value_hi
      STA   tok_idx_hi
      JMP   push_false
@file:
      JSR   pop_value            ; source kind
      BCS   @done
      LDA   value_hi
      BNE   @fail
      LDA   value_lo
      CMP   #1
      BNE   @fail
      JSR   pop_value            ; remaining bytes from saved line start
      BCS   @done
      LDA   value_lo
      STA   restore_rem_lo
      LDA   value_hi
      STA   restore_rem_hi
      JSR   pop_value            ; XRAM high byte
      BCS   @done
      LDA   value_hi
      BNE   @fail
      LDA   value_lo
      STA   restore_xram_hi
      JSR   pop_value            ; XRAM low/mid bytes
      BCS   @done
      LDA   value_lo
      STA   restore_xram_lo
      LDA   value_hi
      STA   restore_xram_mid
      JSR   pop_value            ; saved >IN
      BCS   @done
      LDA   value_lo
      STA   restore_tok_lo
      LDA   value_hi
      STA   restore_tok_hi
      LDA   #1
      STA   restore_pending
      STA   restore_resume_pending
      LDA   buf_idx
      STA   tok_idx
      STZ   tok_idx_hi
      JMP   push_false
@fail:
      JMP   push_true
@done:
      RTS

code_parse_name:
      LDX   tok_idx
@skip:
      LDA   input_buf,X
      BEQ   @none
      CMP   #' '
      BEQ   @skip_one
      CMP   #$09
      BEQ   @skip_one
      CMP   #$0A
      BEQ   @skip_one
      CMP   #$0D
      BNE   @start
@skip_one:
      INX
      BRA   @skip
@start:
      TXA
      CLC
      ADC   #<input_buf
      STA   tok_start_lo
      LDA   #>input_buf
      ADC   #0
      STA   tok_start_hi
      STZ   tok_len
@scan:
      LDA   input_buf,X
      BEQ   @finish
      CMP   #' '
      BEQ   @finish
      CMP   #$09
      BEQ   @finish
      CMP   #$0A
      BEQ   @finish
      CMP   #$0D
      BEQ   @finish
      INX
      INC   tok_len
      BRA   @scan
@finish:
      STX   tok_idx
      STZ   tok_idx_hi
      LDA   tok_start_lo
      STA   value_lo
      LDA   tok_start_hi
      STA   value_hi
      JSR   push_value
      LDA   tok_len
      STA   value_lo
      STZ   value_hi
      JMP   push_value
@none:
      STZ   value_lo
      STZ   value_hi
      JSR   push_value
      JMP   push_value

code_word:
      JSR   pop_value
      BCS   @done
      LDA   value_lo
      STA   char_tmp
      LDX   tok_idx
@skip:
      LDA   input_buf,X
      BEQ   @empty
      CMP   #' '
      BEQ   @skip_one
      CMP   #$09
      BEQ   @skip_one
      CMP   #$0A
      BEQ   @skip_one
      CMP   #$0D
      BEQ   @skip_one
      CMP   char_tmp
      BNE   @start
@skip_one:
      INX
      BRA   @skip
@start:
      LDY   #0
@scan:
      LDA   input_buf,X
      BEQ   @finish
      CMP   char_tmp
      BEQ   @finish
      CPY   #$FF
      BCS   @finish
      INY
      STA   word_buf,Y
      INX
      BRA   @scan
@finish:
      LDA   input_buf,X
      BEQ   @store_word
      CMP   char_tmp
      BNE   @store_word
      INX
@store_word:
      STX   tok_idx
      STZ   tok_idx_hi
      TYA
      STA   word_buf
      LDA   #<word_buf
      STA   value_lo
      LDA   #>word_buf
      STA   value_hi
      JMP   push_value
@empty:
      STX   tok_idx
      STZ   tok_idx_hi
      STZ   word_buf
      LDA   #<word_buf
      STA   value_lo
      LDA   #>word_buf
      STA   value_hi
      JMP   push_value
@done:
      RTS

code_find:
      JSR   pop_value
      BCS   @done
      LDA   value_lo
      STA   tmp2_lo
      STA   ptr_lo
      LDA   value_hi
      STA   tmp2_hi
      STA   ptr_hi
      LDY   #0
      LDA   (ptr_lo),Y
      STA   tok_len
      CLC
      LDA   ptr_lo
      ADC   #1
      STA   tok_start_lo
      LDA   ptr_hi
      ADC   #0
      STA   tok_start_hi
      JSR   find_word
      BCC   @not_found
      JSR   header_to_value
      JSR   push_value
      LDA   found_flags
      AND   #FLAG_IMM
      BEQ   @normal
      LDA   #1
      STA   value_lo
      STZ   value_hi
      JMP   push_value
@normal:
      LDA   #$FF
      STA   value_lo
      STA   value_hi
      JMP   push_value
@not_found:
      JSR   tmp2_to_value
      JSR   push_value
      STZ   value_lo
      STZ   value_hi
      JSR   push_value
@done:
      RTS

code_evaluate:
      JSR   pop_value            ; length
      BCS   @done
      LDA   value_lo
      STA   tmp2_lo
      LDA   value_hi
      STA   tmp2_hi
      JSR   pop_value_to_ptr     ; source address
      BCS   @done
      JSR   evaluate_buffer
@done:
      RTS

source_slot_addr:
      LDA   #<source_save_buf
      STA   tmp_lo
      LDA   #>source_save_buf
      STA   tmp_hi
      LDA   source_depth
      BEQ   @done
      TAX
@loop:
      CLC
      LDA   tmp_lo
      ADC   #128
      STA   tmp_lo
      BCC   @next
      INC   tmp_hi
@next:
      DEX
      BNE   @loop
@done:
      RTS

push_input_state:
      LDA   source_depth
      CMP   #SOURCE_STACK_DEPTH
      BCC   @ok
      JMP   source_nesting_error
@ok:
      TAX
      LDA   tok_idx
      STA   source_save_idx_lo,X
      LDA   tok_idx_hi
      STA   source_save_idx_hi,X
      LDA   buf_idx
      STA   source_save_len,X
      LDA   source_addr_lo
      STA   source_save_addr_lo,X
      LDA   source_addr_hi
      STA   source_save_addr_hi,X
      LDA   source_id_lo
      STA   source_save_id_lo,X
      LDA   source_id_hi
      STA   source_save_id_hi,X
      JSR   source_slot_addr
      LDY   #0
@copy:
      LDA   input_buf,Y
      STA   (tmp_lo),Y
      INY
      CPY   #128
      BNE   @copy
      INC   source_depth
      CLC
      RTS

pop_input_state:
      LDA   source_depth
      BEQ   @done
      DEC   source_depth
      JSR   source_slot_addr
      LDY   #0
@copy:
      LDA   (tmp_lo),Y
      STA   input_buf,Y
      INY
      CPY   #128
      BNE   @copy
      LDX   source_depth
      LDA   source_save_idx_lo,X
      STA   tok_idx
      LDA   source_save_idx_hi,X
      STA   tok_idx_hi
      LDA   source_save_len,X
      STA   buf_idx
      STA   source_len
      LDA   source_save_addr_lo,X
      STA   source_addr_lo
      LDA   source_save_addr_hi,X
      STA   source_addr_hi
      LDA   source_save_id_lo,X
      STA   source_id_lo
      LDA   source_save_id_hi,X
      STA   source_id_hi
@done:
      RTS

set_input_buf_source:
      LDA   #<input_buf
      STA   source_addr_lo
      LDA   #>input_buf
      STA   source_addr_hi
      LDA   buf_idx
      STA   source_len
      RTS

evaluate_buffer:
      JSR   push_input_state
      BCC   @next_line
      JMP   @done
@next_line:
      LDA   abort_flag
      BEQ   :+
      JMP   @restore
:
      LDA   tmp2_lo
      ORA   tmp2_hi
      BNE   :+
      JMP   @restore
:
      LDA   ptr_lo
      STA   arith0
      LDA   ptr_hi
      STA   arith1
      LDX   #0
@copy_line:
      LDA   tmp2_lo
      ORA   tmp2_hi
      BEQ   @line_ready
      LDY   #0
      LDA   (ptr_lo),Y
      STA   char_tmp
      JSR   inc_ptr
      JSR   dec_tmp2
      LDA   char_tmp
      CMP   #$0D
      BEQ   @line_ready
      CMP   #$0A
      BEQ   @line_ready
      CPX   #127
      BCC   @store_char
      JSR   source_line_too_long_error
      JMP   @restore
@store_char:
      STA   input_buf,X
      INX
      BRA   @copy_line
@line_ready:
      STZ   input_buf,X
      STX   buf_idx
      STX   source_len
      LDA   arith0
      STA   source_addr_lo
      LDA   arith1
      STA   source_addr_hi
      LDA   #$FF
      STA   source_id_lo
      STA   source_id_hi
      STZ   tok_idx
      STZ   tok_idx_hi
      LDA   ptr_lo
      PHA
      LDA   ptr_hi
      PHA
      LDA   tmp2_lo
      PHA
      LDA   tmp2_hi
      PHA
      JSR   interpret_line
      PLA
      STA   tmp2_hi
      PLA
      STA   tmp2_lo
      PLA
      STA   ptr_hi
      PLA
      STA   ptr_lo
      JMP   @next_line
@restore:
      JSR   pop_input_state
      LDA   abort_flag
      BNE   @error
      CLC
      RTS
@error:
      SEC
@done:
      RTS

read_xram_source_byte:
      LDA   XMC_WINCTL
      ORA   #XRAM_WIN3_ENABLE
      STA   XMC_WINCTL
      STZ   WIN3_LO
      LDA   xram_src_mid
      STA   WIN3_MI
      LDA   xram_src_hi
      STA   WIN3_HI
      LDX   xram_src_lo
      LDA   WIN3_BASE,X
      STA   char_tmp
      INC   xram_src_lo
      BNE   @done
      INC   xram_src_mid
      BNE   @done
      INC   xram_src_hi
@done:
      RTS

refill_xram_line:
      LDA   tmp2_lo
      ORA   tmp2_hi
      BNE   @have_data
      CLC
      RTS
@have_data:
      LDA   xram_src_lo
      STA   file_line_lo
      LDA   xram_src_mid
      STA   file_line_mid
      LDA   xram_src_hi
      STA   file_line_hi
      LDA   tmp2_lo
      STA   file_line_rem_lo
      LDA   tmp2_hi
      STA   file_line_rem_hi
      LDX   #0
@copy_line:
      LDA   tmp2_lo
      ORA   tmp2_hi
      BEQ   @line_ready
      PHX
      JSR   read_xram_source_byte
      PLX
      JSR   dec_tmp2
      LDA   char_tmp
      CMP   #$0D
      BEQ   @line_ready
      CMP   #$0A
      BEQ   @line_ready
      CPX   #127
      BCC   @store_char
      JSR   source_line_too_long_error
      CLC
      RTS
@store_char:
      STA   input_buf,X
      INX
      BRA   @copy_line
@line_ready:
      STZ   input_buf,X
      STX   buf_idx
      JSR   set_input_buf_source
      LDA   #1
      STA   source_id_lo
      STZ   source_id_hi
      STZ   tok_idx
      STZ   tok_idx_hi
      SEC
      RTS

evaluate_xram_buffer:
      JSR   push_input_state
      BCC   @next_line
      JMP   @done
@next_line:
      LDA   abort_flag
      BEQ   :+
      JMP   @restore
:
      LDA   tmp2_lo
      ORA   tmp2_hi
      BNE   :+
      JMP   @restore
:
      LDA   xram_src_lo
      STA   file_line_lo
      LDA   xram_src_mid
      STA   file_line_mid
      LDA   xram_src_hi
      STA   file_line_hi
      LDA   tmp2_lo
      STA   file_line_rem_lo
      LDA   tmp2_hi
      STA   file_line_rem_hi
      LDX   #0
@copy_line:
      LDA   tmp2_lo
      ORA   tmp2_hi
      BEQ   @line_ready
      PHX
      JSR   read_xram_source_byte
      PLX
      JSR   dec_tmp2
      LDA   char_tmp
      CMP   #$0D
      BEQ   @line_ready
      CMP   #$0A
      BEQ   @line_ready
      CPX   #127
      BCC   @store_char
      JSR   source_line_too_long_error
      JMP   @restore
@store_char:
      STA   input_buf,X
      INX
      BRA   @copy_line
@line_ready:
      STZ   input_buf,X
      STX   buf_idx
      JSR   set_input_buf_source
      LDA   #1
      STA   source_id_lo
      STZ   source_id_hi
      STZ   tok_idx
      STZ   tok_idx_hi
      LDA   xram_src_lo
      PHA
      LDA   xram_src_mid
      PHA
      LDA   xram_src_hi
      PHA
      LDA   tmp2_lo
      PHA
      LDA   tmp2_hi
      PHA
      JSR   interpret_line
      LDA   restore_pending
      BNE   @apply_restore
      LDA   restore_keep_current
      BNE   @keep_current
      PLA
      STA   tmp2_hi
      PLA
      STA   tmp2_lo
      PLA
      STA   xram_src_hi
      PLA
      STA   xram_src_mid
      PLA
      STA   xram_src_lo
      JMP   @next_line
@apply_restore:
      PLA
      PLA
      PLA
      PLA
      PLA
      LDA   restore_xram_lo
      STA   xram_src_lo
      LDA   restore_xram_mid
      STA   xram_src_mid
      LDA   restore_xram_hi
      STA   xram_src_hi
      LDA   restore_rem_lo
      STA   tmp2_lo
      LDA   restore_rem_hi
      STA   tmp2_hi
      STZ   restore_pending
      STZ   restore_keep_current
      JMP   @next_line
@keep_current:
      PLA
      PLA
      PLA
      PLA
      PLA
      STZ   restore_keep_current
      JMP   @next_line
@restore:
      JSR   pop_input_state
      LDA   abort_flag
      BNE   @error
      CLC
      RTS
@error:
      SEC
@done:
      RTS

fio_exec_forth:
      PHA
      STZ   FIO_STATUS
      STZ   FIO_ERRCODE
      PLA
      STA   FIO_CMD
@wait:
      LDA   FIO_STATUS
      BEQ   @wait
      CMP   #FIO_STATUS_OK
      BEQ   @ok
      SEC
      RTS
@ok:
      CLC
      RTS

copy_fio_name_from_ptr:
      LDA   char_tmp
      BEQ   @bad
      CMP   #FIO_NAME_LIMIT + 1
      BCS   @bad
      STA   FIO_NAMELEN
      TAX
      LDY   #0
@copy:
      LDA   (ptr_lo),Y
      STA   FIO_NAME,Y
      INY
      DEX
      BNE   @copy
      CLC
      RTS
@bad:
      JMP   include_failed_error

copy_fio_name_to_word_buf:
      LDY   #0
@copy:
      CPY   FIO_NAMELEN
      BCS   @done
      LDA   FIO_NAME,Y
      STA   word_buf,Y
      INY
      BRA   @copy
@done:
      LDA   #0
      STA   word_buf,Y
      RTS

load_and_evaluate_forth_file:
      JSR   copy_fio_name_from_ptr
      BCS   @done
      LDA   source_depth
      CMP   #SOURCE_STACK_DEPTH
      BCC   @have_slot
      JSR   source_nesting_error
      BRA   @done
@have_slot:
      LDA   #FORTH_XRAM_STAGE_L
      STA   FIO_GADDRL
      LDA   #FORTH_XRAM_STAGE_M
      STA   FIO_GADDRH
      LDA   source_depth
      STA   FIO_GSPACE
      LDA   #$FF
      STA   FIO_GLENL
      STA   FIO_GLENH
      LDA   #FIO_TYPE_FORTH
      STA   FIO_DIRTYPE
      LDA   #FIO_CMD_XLOAD
      JSR   fio_exec_forth
      BCS   @failed
      LDA   FIO_SIZEL
      STA   tmp2_lo
      LDA   FIO_SIZEH
      STA   tmp2_hi
      LDA   #FORTH_XRAM_STAGE_L
      STA   xram_src_lo
      LDA   #FORTH_XRAM_STAGE_M
      STA   xram_src_mid
      LDA   source_depth
      STA   xram_src_hi
      JMP   evaluate_xram_buffer
@failed:
      LDA   FIO_ERRCODE
      CMP   #FIO_ERR_NOTFOUND
      BEQ   @not_found
      JMP   include_failed_error
@not_found:
      SEC
@done:
      RTS

code_included:
      JSR   pop_value            ; filename length
      BCS   @done
      LDA   value_hi
      BNE   @failed
      LDA   value_lo
      STA   char_tmp
      JSR   pop_value_to_ptr     ; filename address
      BCS   @done
      JSR   load_and_evaluate_forth_file
      BCC   @done
@failed:
      JSR   include_failed_error
@done:
      RTS

code_include:
      JSR   parse_raw_name
      BCC   @need_name
      JSR   load_and_evaluate_forth_file
      BCC   @done
      JSR   include_failed_error
@done:
      RTS
@need_name:
      JMP   need_word_name

editor_install_hooks:
      LDA   #<editor_hook_code_start
      STA   ptr_lo
      LDA   #>editor_hook_code_start
      STA   ptr_hi
      LDA   #<editor_hook_code
      STA   tmp_lo
      LDA   #>editor_hook_code
      STA   tmp_hi
      LDA   #<editor_hook_code_size
      STA   value_lo
      LDA   #>editor_hook_code_size
      STA   value_hi
      LDY   #0
@copy_code:
      LDA   (ptr_lo),Y
      STA   (tmp_lo),Y
      INC   ptr_lo
      BNE   :+
      INC   ptr_hi
:     INC   tmp_lo
      BNE   :+
      INC   tmp_hi
:     LDA   value_lo
      BNE   :+
      DEC   value_hi
:     DEC   value_lo
      LDA   value_lo
      ORA   value_hi
      BNE   @copy_code

      LDX   #0
@copy_type:
      LDA   editor_type_text,X
      STA   editor_type,X
      BEQ   @type_done
      INX
      BRA   @copy_type
@type_done:
      LDX   #0
      LDA   #0
@clear_hooks:
      STA   editor_hooks,X
      INX
      CPX   #EDITOR_HOOKS_SIZE
      BNE   @clear_hooks

      LDA   #<editor_type
      STA   editor_hooks+EDITOR_HOOKS_TYPEL
      LDA   #>editor_type
      STA   editor_hooks+EDITOR_HOOKS_TYPEH
      LDA   #<editor_hook_save
      STA   editor_hooks+EDITOR_HOOKS_SAVE_VECL
      LDA   #>editor_hook_save
      STA   editor_hooks+EDITOR_HOOKS_SAVE_VECH
      LDA   #<editor_hook_indent
      STA   editor_hooks+EDITOR_HOOKS_INDENT_VECL
      LDA   #>editor_hook_indent
      STA   editor_hooks+EDITOR_HOOKS_INDENT_VECH
      LDA   #<editor_hook_hilite
      STA   editor_hooks+EDITOR_HOOKS_HILITE_VECL
      LDA   #>editor_hook_hilite
      STA   editor_hooks+EDITOR_HOOKS_HILITE_VECH
      LDA   #<editor_hook_noop
      STA   editor_hooks+EDITOR_HOOKS_MENU_VECL
      STA   editor_hooks+EDITOR_HOOKS_COMMAND_VECL
      STA   editor_hooks+EDITOR_HOOKS_CHANGED_VECL
      LDA   #>editor_hook_noop
      STA   editor_hooks+EDITOR_HOOKS_MENU_VECH
      STA   editor_hooks+EDITOR_HOOKS_COMMAND_VECH
      STA   editor_hooks+EDITOR_HOOKS_CHANGED_VECH
      RTS

editor_type_text:
      .byte "Forth", 0

editor_hook_save = editor_hook_code + (editor_hook_save_rom - editor_hook_code_start)
editor_hook_indent = editor_hook_code + (editor_hook_indent_rom - editor_hook_code_start)
editor_hook_hilite = editor_hook_code + (editor_hook_hilite_rom - editor_hook_code_start)
editor_hook_noop = editor_hook_code + (editor_hook_noop_rom - editor_hook_code_start)
editor_hook_load_from_offset = editor_hook_code + (editor_hook_load_from_offset_rom - editor_hook_code_start)
editor_hl_color_to_eol = editor_hook_code + (editor_hl_color_to_eol_rom - editor_hook_code_start)
editor_hl_color_token = editor_hook_code + (editor_hl_color_token_rom - editor_hook_code_start)
editor_hl_is_delim = editor_hook_code + (editor_hl_is_delim_rom - editor_hook_code_start)
editor_hl_is_digit = editor_hook_code + (editor_hl_is_digit_rom - editor_hook_code_start)
editor_hl_is_alpha = editor_hook_code + (editor_hl_is_alpha_rom - editor_hook_code_start)
editor_hook_code_size = editor_hook_code_end - editor_hook_code_start
.assert editor_hook_code_size <= $0400, error, "NovaForth editor hook code must fit the editor hook workspace"

editor_hook_code_start:
editor_hook_save_rom:
      LDA   #0
      RTS

editor_hook_noop_rom:
      RTS

editor_hook_indent_rom:
      LDA   EDITOR_HOOK_ABI_CURL
      ORA   EDITOR_HOOK_ABI_CURL+1
      BNE   :+
      LDA   #0
      RTS
:     LDA   EDITOR_HOOK_ABI_CURL
      STA   editor_hook_tmp2
      LDA   EDITOR_HOOK_ABI_CURL+1
      STA   editor_hook_tmp3
      LDA   editor_hook_tmp2
      BNE   :+
      DEC   editor_hook_tmp3
:     DEC   editor_hook_tmp2
      LDA   editor_hook_tmp2
      ORA   editor_hook_tmp3
      BNE   :+
      LDA   #0
      RTS
:     STZ   editor_hook_tmp0
      STZ   editor_hook_tmp1
      STZ   editor_hook_tmp4
      STZ   editor_hook_tmp5
@find_line:
      LDA   editor_hook_tmp0
      CMP   editor_hook_tmp2
      BNE   :+
      LDA   editor_hook_tmp1
      CMP   editor_hook_tmp3
      BEQ   @count_indent
:     JSR   editor_hook_load_from_offset
      CMP   #$0A
      BNE   @advance_find
      LDA   editor_hook_tmp0
      CLC
      ADC   #1
      STA   editor_hook_tmp4
      LDA   editor_hook_tmp1
      ADC   #0
      STA   editor_hook_tmp5
@advance_find:
      INC   editor_hook_tmp0
      BNE   @find_line
      INC   editor_hook_tmp1
      BRA   @find_line
@count_indent:
      LDA   editor_hook_tmp4
      STA   editor_hook_tmp0
      LDA   editor_hook_tmp5
      STA   editor_hook_tmp1
      STZ   editor_hook_tmp4          ; last non-space char
      STZ   editor_hook_tmp6          ; indent count
      STZ   editor_hook_tmp7          ; first non-space char
@count_loop:
      LDA   editor_hook_tmp0
      CMP   editor_hook_tmp2
      BNE   :+
      LDA   editor_hook_tmp1
      CMP   editor_hook_tmp3
      BEQ   @indent_done
:     JSR   editor_hook_load_from_offset
      CMP   #' '
      BNE   @nonspace
      LDA   editor_hook_tmp7
      BNE   @advance_count
      LDA   editor_hook_tmp6
      CMP   #16
      BCS   @advance_count
      INC   editor_hook_tmp6
      BRA   @advance_count
@nonspace:
      CMP   #$09
      BEQ   @advance_count
      LDX   editor_hook_tmp7
      BNE   :+
      STA   editor_hook_tmp7
:     STA   editor_hook_tmp4
@advance_count:
      INC   editor_hook_tmp0
      BNE   @count_loop
      INC   editor_hook_tmp1
      BRA   @count_loop
@indent_done:
      LDA   editor_hook_tmp7
      CMP   #':'
      BNE   @maybe_dedent
      LDA   editor_hook_tmp6
      CLC
      ADC   #2
      CMP   #17
      BCC   :+
      LDA   #16
:     STA   editor_hook_tmp6
@maybe_dedent:
      LDA   editor_hook_tmp4
      CMP   #';'
      BNE   @return_indent
      LDA   editor_hook_tmp6
      CMP   #2
      BCC   @zero_indent
      SEC
      SBC   #2
      RTS
@zero_indent:
      LDA   #0
      RTS
@return_indent:
      LDA   editor_hook_tmp6
      RTS

editor_hook_hilite_rom:
      LDY   #0
@hl_loop:
      CPY   EDITOR_HOOK_ABI_HL_LEN
      BCC   :+
      RTS
:     LDA   (EDITOR_HOOK_ABI_HL_PTRL),Y
      CMP   #' '
      BEQ   @hl_advance
      CMP   #$09
      BEQ   @hl_advance
      CMP   #$5C
      BEQ   @hl_comment
      CMP   #':'
      BEQ   @hl_punct
      CMP   #';'
      BEQ   @hl_punct
      JSR   editor_hl_is_digit
      BCS   @hl_number
      JSR   editor_hl_is_alpha
      BCS   @hl_word
@hl_advance:
      INY
      BRA   @hl_loop
@hl_punct:
      LDA   #EDITOR_HOOK_COLOR_WORD
      STA   EDITOR_HOOK_ABI_HL_COLORS,Y
      INY
      BRA   @hl_loop
@hl_word:
      LDA   #EDITOR_HOOK_COLOR_WORD
      JSR   editor_hl_color_token
      BRA   @hl_loop
@hl_number:
      LDA   #EDITOR_HOOK_COLOR_NUMBER
      JSR   editor_hl_color_token
      BRA   @hl_loop
@hl_comment:
      LDA   #EDITOR_HOOK_COLOR_COMMENT
      JMP   editor_hl_color_to_eol

editor_hook_load_from_offset_rom:
      LDA   EDITOR_HOOK_ABI_BUFL
      CLC
      ADC   editor_hook_tmp0
      STA   LIB_ZP
      LDA   EDITOR_HOOK_ABI_BUFL+1
      ADC   editor_hook_tmp1
      STA   LIB_ZP+1
      LDY   #0
      LDA   (LIB_ZP),Y
      RTS

editor_hl_color_to_eol_rom:
      STA   editor_hook_tmp0
@loop:
      CPY   EDITOR_HOOK_ABI_HL_LEN
      BCS   @done
      LDA   editor_hook_tmp0
      STA   EDITOR_HOOK_ABI_HL_COLORS,Y
      INY
      BRA   @loop
@done:
      RTS

editor_hl_color_token_rom:
      STA   editor_hook_tmp0
@loop:
      CPY   EDITOR_HOOK_ABI_HL_LEN
      BCS   @done
      LDA   (EDITOR_HOOK_ABI_HL_PTRL),Y
      JSR   editor_hl_is_delim
      BCS   @done
      LDA   editor_hook_tmp0
      STA   EDITOR_HOOK_ABI_HL_COLORS,Y
      INY
      BRA   @loop
@done:
      RTS

editor_hl_is_delim_rom:
      CMP   #' '
      BEQ   @yes
      CMP   #$09
      BEQ   @yes
      CMP   #'['
      BEQ   @yes
      CMP   #']'
      BEQ   @yes
      CLC
      RTS
@yes:
      SEC
      RTS

editor_hl_is_digit_rom:
      CMP   #'0'
      BCC   @no
      CMP   #'9' + 1
      BCC   @yes
@no:
      CLC
      RTS
@yes:
      SEC
      RTS

editor_hl_is_alpha_rom:
      CMP   #'A'
      BCC   @lower
      CMP   #'Z' + 1
      BCC   @yes
@lower:
      CMP   #'a'
      BCC   @no
      CMP   #'z' + 1
      BCC   @yes
@no:
      CLC
      RTS
@yes:
      SEC
      RTS
editor_hook_code_end:

code_edit:
      JSR   parse_raw_name
      BCS   @have_name
      JMP   need_word_name
@have_name:
      JSR   copy_fio_name_from_ptr
      BCC   @name_ready
      RTS
@name_ready:
      JSR   copy_fio_name_to_word_buf

      SEC
      LDA   #<DICT_LIMIT
      SBC   here_lo
      STA   LIB_ARG2+0
      LDA   #>DICT_LIMIT
      SBC   here_hi
      STA   LIB_ARG2+1
      LDA   LIB_ARG2+0
      ORA   LIB_ARG2+1
      BNE   @load
      JSR   print_inl
      .byte "NO EDIT BUFFER", $0D, $0A, 0
      JMP   raise_error

@load:
      LDA   here_lo
      STA   FIO_SRCL
      LDA   here_hi
      STA   FIO_SRCH
      LDA   LIB_ARG2+0
      STA   FIO_GLENL
      LDA   LIB_ARG2+1
      STA   FIO_GLENH
      LDA   #FIO_TYPE_FORTH
      STA   FIO_DIRTYPE
      LDA   #FIO_CMD_LOAD
      JSR   fio_exec_forth
      BCC   @loaded
      LDA   FIO_ERRCODE
      CMP   #FIO_ERR_NOTFOUND
      BEQ   @new_file
      JMP   edit_failed_error
@loaded:
      LDA   FIO_SIZEL
      STA   LIB_ARG1+0
      LDA   FIO_SIZEH
      STA   LIB_ARG1+1
      STZ   LIB_ARG1+2
      STZ   LIB_ARG1+3
      BRA   @run_editor
@new_file:
      STZ   LIB_ARG1+0
      STZ   LIB_ARG1+1
      STZ   LIB_ARG1+2
      STZ   LIB_ARG1+3

@run_editor:
      LDA   here_lo
      STA   LIB_ARG0+0
      LDA   here_hi
      STA   LIB_ARG0+1
      STZ   LIB_ARG0+2
      STZ   LIB_ARG0+3
      STZ   LIB_ARG2+2
      STZ   LIB_ARG2+3
      LDA   #<word_buf
      STA   LIB_ARG3+0
      LDA   #>word_buf
      STA   LIB_ARG3+1

      JSR   editor_install_hooks
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
      CMP   #LERR_OK
      BEQ   @maybe_save
      JMP   edit_failed_error

@maybe_save:
      LDA   LIB_RESULT+1
      BEQ   @done
      LDA   here_lo
      STA   FIO_SRCL
      LDA   here_hi
      STA   FIO_SRCH
      CLC
      LDA   here_lo
      ADC   LIB_ARG1+0
      STA   FIO_ENDL
      LDA   here_hi
      ADC   LIB_ARG1+1
      BCS   @save_failed
      STA   FIO_ENDH
      LDA   #FIO_TYPE_FORTH
      STA   FIO_DIRTYPE
      LDA   #FIO_CMD_SAVE
      JSR   fio_exec_forth
      BCC   @done
@save_failed:
      JMP   edit_failed_error
@done:
      RTS

parse_raw_name:
      LDX   tok_idx
@skip:
      LDA   input_buf,X
      BEQ   @none
      CMP   #' '
      BEQ   @skip_one
      CMP   #$09
      BEQ   @skip_one
      CMP   #$0A
      BEQ   @skip_one
      CMP   #$0D
      BNE   @start
@skip_one:
      INX
      BRA   @skip
@start:
      TXA
      CLC
      ADC   #<input_buf
      STA   ptr_lo
      LDA   #>input_buf
      ADC   #0
      STA   ptr_hi
      STZ   char_tmp
@scan:
      LDA   input_buf,X
      BEQ   @finish
      CMP   #' '
      BEQ   @finish
      CMP   #$09
      BEQ   @finish
      CMP   #$0A
      BEQ   @finish
      CMP   #$0D
      BEQ   @finish
      INX
      INC   char_tmp
      BRA   @scan
@finish:
      STX   tok_idx
      STZ   tok_idx_hi
      SEC
      RTS
@none:
      STX   tok_idx
      STZ   tok_idx_hi
      CLC
      RTS

code_save_forth:
      JSR   pop_value            ; filename length
      BCS   @done
      LDA   value_hi
      BNE   @failed
      LDA   value_lo
      STA   char_tmp
      JSR   pop_value_to_ptr     ; filename address
      BCS   @done
      JSR   copy_fio_name_from_ptr
      BCS   @done
      JSR   pop_value            ; source length
      BCS   @done
      LDA   value_lo
      STA   tmp2_lo
      LDA   value_hi
      STA   tmp2_hi
      JSR   pop_value            ; source address
      BCS   @done
      LDA   value_lo
      STA   FIO_SRCL
      LDA   value_hi
      STA   FIO_SRCH
      CLC
      LDA   value_lo
      ADC   tmp2_lo
      STA   FIO_ENDL
      LDA   value_hi
      ADC   tmp2_hi
      BCS   @failed
      STA   FIO_ENDH
      LDA   #FIO_TYPE_FORTH
      STA   FIO_DIRTYPE
      LDA   #FIO_CMD_SAVE
      JSR   fio_exec_forth
      BCC   @done
@failed:
      JSR   include_failed_error
@done:
      RTS

code_nova_libcall:
      JSR   pop_value            ; function id
      BCS   @done
      LDA   value_hi
      BNE   @bad
      LDA   value_lo
      STA   LIB_FN_ID
      JSR   pop_value            ; module id
      BCS   @done
      LDA   value_hi
      BNE   @bad
      LDA   value_lo
      STA   LIB_MOD_ID
      JSR   LIB_LOADER_BAND
      LDA   LIB_STATUS
      STA   value_lo
      STZ   value_hi
      JMP   push_value
@bad:
      JSR   include_failed_error
@done:
      RTS

code_accept:
      JSR   pop_value            ; max length
      BCS   @done
      LDA   value_hi
      BNE   @cap
      LDA   value_lo
      CMP   #128
      BCC   @set_len
@cap:
      LDA   #127
@set_len:
      STA   char_tmp
      JSR   pop_value_to_ptr     ; destination address
      BCS   @done
      LDY   #0
@poll:
      LDA   VGC_CHARIN
      BEQ   @poll
      CMP   #$0D
      BEQ   @finish
      CPY   char_tmp
      BCS   @poll
      STA   (ptr_lo),Y
      STA   VGC_CHAROUT
      INY
      BRA   @poll
@finish:
      TYA
      STA   value_lo
      STZ   value_hi
      JMP   push_value
@done:
      RTS

code_quit:
      JMP   raise_error

code_environment_query:
      JSR   pop_value            ; length
      BCS   @done
      LDA   value_lo
      STA   tok_len
      LDA   value_hi
      STA   tmp2_hi
      JSR   pop_value            ; address
      BCS   @done
      LDA   value_lo
      STA   ptr_lo
      LDA   value_hi
      STA   ptr_hi
      LDA   tmp2_hi
      BNE   @unknown
      JSR   find_env_query
      BCC   @unknown
      LDY   #1
      LDA   (tmp_lo),Y
      CMP   #ENV_KIND_DOUBLE
      BEQ   @double
      JSR   push_env_value1
      BRA   @true
@double:
      JSR   push_env_value1
      JSR   push_env_value2
@true:
      LDA   #$FF
      STA   value_lo
      STA   value_hi
      JMP   push_value
@unknown:
      STZ   value_lo
      STZ   value_hi
      JSR   push_value
@done:
      RTS

find_env_query:
      LDA   #<env_query_table
      STA   tmp_lo
      LDA   #>env_query_table
      STA   tmp_hi
@entry:
      LDY   #0
      LDA   (tmp_lo),Y
      BEQ   @not_found
      STA   char_tmp
      CMP   tok_len
      BNE   @next
      LDX   #0
@cmp:
      CPX   tok_len
      BEQ   @found
      TXA
      CLC
      ADC   #6
      TAY
      LDA   (tmp_lo),Y
      STA   tmp2_lo
      TXA
      TAY
      LDA   (ptr_lo),Y
      CMP   #'a'
      BCC   @compare
      CMP   #'z'+1
      BCS   @compare
      AND   #$DF
@compare:
      CMP   tmp2_lo
      BNE   @next
      INX
      BRA   @cmp
@next:
      CLC
      LDA   char_tmp
      ADC   #6
      STA   tmp2_lo
      STZ   tmp2_hi
      BCC   @advance
      INC   tmp2_hi
@advance:
      CLC
      LDA   tmp_lo
      ADC   tmp2_lo
      STA   tmp_lo
      LDA   tmp_hi
      ADC   tmp2_hi
      STA   tmp_hi
      BRA   @entry
@found:
      SEC
      RTS
@not_found:
      CLC
      RTS

push_env_value1:
      LDY   #2
      LDA   (tmp_lo),Y
      STA   value_lo
      INY
      LDA   (tmp_lo),Y
      STA   value_hi
      JMP   push_value

push_env_value2:
      LDY   #4
      LDA   (tmp_lo),Y
      STA   value_lo
      INY
      LDA   (tmp_lo),Y
      STA   value_hi
      JMP   push_value

code_char:
      JSR   next_token
      BCC   @need_name
      LDY   #0
      LDA   (tok_start_lo),Y
      STA   value_lo
      STZ   value_hi
      JMP   push_value
@need_name:
      JMP   need_word_name

code_tick:
      JSR   parse_known_word
      BCS   @done
      JSR   header_to_value
      JMP   push_value
@done:
      RTS

code_bracket_tick:
      JSR   require_compiling
      BCS   @done
      JSR   parse_known_word
      BCS   @done
      JSR   header_to_value
      JSR   compile_literal
@done:
      RTS

code_execute:
      JSR   pop_value
      BCS   @done
      JSR   value_to_exec_header
      JSR   execute_header
@done:
      RTS

code_compile_comma:
      JSR   require_definition_context
      BCS   @done
      JSR   pop_value
      BCS   @done
      JSR   value_to_exec_header
      JSR   compile_exec_header
@done:
      RTS

code_literal_word:
      JSR   require_compiling
      BCS   @done
      JSR   pop_value
      BCS   @done
      JSR   compile_literal
@done:
      RTS

code_immediate:
      LDA   latest_lo
      STA   ptr_lo
      LDA   latest_hi
      STA   ptr_hi
      LDA   ptr_lo
      ORA   ptr_hi
      BEQ   @done
      LDY   #2
      LDA   (ptr_lo),Y
      ORA   #FLAG_IMM
      STA   (ptr_lo),Y
@done:
      RTS

code_left_bracket:
      STZ   state
      RTS

code_right_bracket:
      LDA   #1
      STA   state
      RTS

code_postpone:
      JSR   require_compiling
      BCS   @done
      JSR   parse_known_word
      BCS   @done
      LDA   found_flags
      AND   #FLAG_IMM
      BNE   @compile_now
      JSR   header_to_value
      JSR   compile_literal
      LDA   #<word_compile_comma
      STA   exec_hdr_lo
      LDA   #>word_compile_comma
      STA   exec_hdr_hi
      JSR   compile_exec_header
      RTS
@compile_now:
      JSR   compile_exec_header
@done:
      RTS

code_recurse:
      JSR   require_compiling
      BCS   @done
      LDA   current_def_lo
      STA   exec_hdr_lo
      LDA   current_def_hi
      STA   exec_hdr_hi
      JSR   compile_exec_header
@done:
      RTS

code_to_body:
      JSR   pop_value
      BCS   @done
      JSR   value_to_exec_header
      JSR   exec_header_body_to_value
      JSR   push_value
@done:
      RTS

exec_header_body_to_value:
      LDA   exec_hdr_lo
      STA   ptr_lo
      LDA   exec_hdr_hi
      STA   ptr_hi
      LDY   #2
      LDA   (ptr_lo),Y
      AND   #LEN_MASK
      CLC
      ADC   #3
      CLC
      ADC   ptr_lo
      STA   tmp_lo              ; code field address
      LDA   ptr_hi
      ADC   #0
      STA   tmp_hi
      LDY   #0
      LDA   (tmp_lo),Y
      STA   arith0
      INY
      LDA   (tmp_lo),Y
      STA   arith1
      CLC
      LDA   tmp_lo
      ADC   #2
      STA   value_lo
      LDA   tmp_hi
      ADC   #0
      STA   value_hi
      LDA   arith0
      CMP   #<code_created
      BNE   @check_dodoes
      LDA   arith1
      CMP   #>code_created
      BEQ   @skip_hidden
      BRA   @check_dodoes
@check_dodoes:
      LDA   arith0
      CMP   #<code_dodoes
      BNE   @push
      LDA   arith1
      CMP   #>code_dodoes
      BNE   @push
@skip_hidden:
      CLC
      LDA   value_lo
      ADC   #2
      STA   value_lo
      BCC   @push
      INC   value_hi
@push:
      RTS

code_to:
      JSR   parse_known_word
      BCS   @done
      JSR   exec_header_body_to_value
      LDA   state
      BNE   @compile
      JSR   push_value
      JMP   code_store
@compile:
      JSR   compile_literal
      LDA   #<word_store
      STA   exec_hdr_lo
      LDA   #>word_store
      STA   exec_hdr_hi
      JSR   compile_exec_header
@done:
      RTS

code_does_runtime:
      LDA   latest_lo
      STA   ptr_lo
      LDA   latest_hi
      STA   ptr_hi
      LDY   #2
      LDA   (ptr_lo),Y
      AND   #LEN_MASK
      CLC
      ADC   #3
      CLC
      ADC   ptr_lo
      STA   tmp_lo              ; code field address
      LDA   ptr_hi
      ADC   #0
      STA   tmp_hi
      LDY   #0
      LDA   #<code_dodoes
      STA   (tmp_lo),Y
      INY
      LDA   #>code_dodoes
      STA   (tmp_lo),Y
      CLC
      LDA   tmp_lo
      ADC   #2
      STA   pfa_lo
      LDA   tmp_hi
      ADC   #0
      STA   pfa_hi
      LDY   #0
      LDA   ip_lo
      STA   (pfa_lo),Y
      INY
      LDA   ip_hi
      STA   (pfa_lo),Y
      JMP   code_exit

code_does:
      JSR   require_compiling
      BCS   @done
      LDA   #<word_does_runtime
      STA   exec_hdr_lo
      LDA   #>word_does_runtime
      STA   exec_hdr_hi
      JSR   compile_exec_header
@done:
      RTS

code_paren:
      JMP   skip_paren_in_line

skip_paren_in_line:
      LDX   tok_idx
@loop:
      LDA   input_buf,X
      BEQ   @open
      CMP   #')'
      BEQ   @closed
      INX
      BRA   @loop
@closed:
      INX
      STX   tok_idx
      STZ   tok_idx_hi
      STZ   paren_skip
      SEC
      RTS
@open:
      STX   tok_idx
      STZ   tok_idx_hi
      LDA   #1
      STA   paren_skip
      CLC
      RTS

code_backslash:
      LDA   buf_idx
      STA   tok_idx
      STZ   tok_idx_hi
      RTS

code_bracket_char:
      JSR   require_compiling
      BCS   @done
      JSR   next_token
      BCC   @need_name
      LDY   #0
      LDA   (tok_start_lo),Y
      STA   value_lo
      STZ   value_hi
      JSR   compile_literal
@done:
      RTS
@need_name:
      JMP   need_word_name

parse_quote_string:
      LDX   tok_idx
      STX   parse_idx
      STZ   tok_len
@scan:
      LDA   input_buf,X
      BEQ   @finish
      CMP   #'"'
      BEQ   @finish_quote
      INX
      INC   tok_len
      BRA   @scan
@finish_quote:
      INX
@finish:
      STX   tok_idx
      STZ   tok_idx_hi
      RTS

code_parse:
      JSR   pop_value
      BCS   @done
      LDA   value_lo
      STA   char_tmp
      LDX   tok_idx
      STX   parse_idx
      STZ   tok_len
@scan:
      LDA   input_buf,X
      BEQ   @finish_end
      CMP   char_tmp
      BEQ   @finish_delim
      INX
      INC   tok_len
      BRA   @scan
@finish_delim:
      INX
@finish_end:
      STX   tok_idx
      STZ   tok_idx_hi
      LDA   #<input_buf
      CLC
      ADC   parse_idx
      STA   value_lo
      LDA   #>input_buf
      ADC   #0
      STA   value_hi
      JSR   push_value
      LDA   tok_len
      STA   value_lo
      STZ   value_hi
      JSR   push_value
@done:
      RTS

code_dot_paren:
      LDX   tok_idx
      STX   parse_idx
      STZ   tok_len
@scan:
      LDA   input_buf,X
      BEQ   @print
      CMP   #')'
      BEQ   @finish_delim
      INX
      INC   tok_len
      BRA   @scan
@finish_delim:
      INX
@print:
      STX   tok_idx
      STZ   tok_idx_hi
      LDX   parse_idx
      LDY   #0
@print_loop:
      CPY   tok_len
      BCS   @done
      LDA   input_buf,X
      STA   VGC_CHAROUT
      INX
      INY
      BRA   @print_loop
@done:
      RTS

compile_quote_payload:
      LDA   tok_len
      STA   value_lo
      STZ   value_hi
      JSR   compile_byte_value
      LDX   parse_idx
      LDY   #0
@copy:
      CPY   tok_len
      BCS   @done
      LDA   input_buf,X
      STA   value_lo
      STZ   value_hi
      PHX
      PHY
      JSR   compile_byte_value
      PLY
      PLX
      INX
      INY
      BRA   @copy
@done:
      RTS

compile_wordbuf_payload:
      LDA   tok_len
      STA   value_lo
      STZ   value_hi
      JSR   compile_byte_value
      LDY   #0
@copy:
      CPY   tok_len
      BCS   @done
      LDA   word_buf,Y
      STA   value_lo
      STZ   value_hi
      PHY
      JSR   compile_byte_value
      PLY
      INY
      BRA   @copy
@done:
      RTS

compile_ptr_payload:
      LDA   tok_len
      STA   value_lo
      STZ   value_hi
      JSR   compile_byte_value
      LDY   #0
@copy:
      CPY   tok_len
      BCS   @done
      LDA   (ptr_lo),Y
      STA   value_lo
      STZ   value_hi
      PHY
      JSR   compile_byte_value
      PLY
      INY
      BRA   @copy
@done:
      RTS

hex_char_to_nibble:
      CMP   #'0'
      BCC   @no
      CMP   #'9'+1
      BCC   @digit
      CMP   #'a'
      BCC   @upper
      CMP   #'z'+1
      BCS   @upper
      AND   #$DF
@upper:
      CMP   #'A'
      BCC   @no
      CMP   #'F'+1
      BCS   @no
      SEC
      SBC   #'A'-10
      SEC
      RTS
@digit:
      SEC
      SBC   #'0'
      SEC
      RTS
@no:
      CLC
      RTS

parse_escaped_quote_string:
      LDX   tok_idx
      LDY   #0
@scan:
      LDA   input_buf,X
      BNE   @not_end
      JMP   @finish
@not_end:
      CMP   #'"'
      BNE   @not_quote
      JMP   @finish_quote
@not_quote:
      CMP   #$5C
      BEQ   @escape
      STA   word_buf,Y
      INY
      INX
      BRA   @scan
@escape:
      INX
      LDA   input_buf,X
      BNE   @have_escape
      JMP   @finish
@have_escape:
      CMP   #'a'
      BEQ   @bel
      CMP   #'A'
      BEQ   @bel
      CMP   #'b'
      BEQ   @bs
      CMP   #'B'
      BEQ   @bs
      CMP   #'e'
      BEQ   @esc
      CMP   #'E'
      BEQ   @esc
      CMP   #'f'
      BEQ   @ff
      CMP   #'F'
      BEQ   @ff
      CMP   #'l'
      BEQ   @lf
      CMP   #'L'
      BEQ   @lf
      CMP   #'m'
      BEQ   @crlf
      CMP   #'M'
      BEQ   @crlf
      CMP   #'n'
      BEQ   @crlf
      CMP   #'N'
      BEQ   @crlf
      CMP   #'q'
      BEQ   @quote
      CMP   #'Q'
      BEQ   @quote
      CMP   #'r'
      BEQ   @cr
      CMP   #'R'
      BEQ   @cr
      CMP   #'t'
      BEQ   @tab
      CMP   #'T'
      BEQ   @tab
      CMP   #'v'
      BEQ   @vt
      CMP   #'V'
      BEQ   @vt
      CMP   #'x'
      BEQ   @hex
      CMP   #'X'
      BEQ   @hex
      CMP   #'z'
      BEQ   @nul
      CMP   #'Z'
      BEQ   @nul
      BRA   @store_escape_char
@bel:
      LDA   #7
      BRA   @store_escape_char
@bs:
      LDA   #8
      BRA   @store_escape_char
@esc:
      LDA   #27
      BRA   @store_escape_char
@ff:
      LDA   #12
      BRA   @store_escape_char
@lf:
      LDA   #10
      BRA   @store_escape_char
@quote:
      LDA   #'"'
      BRA   @store_escape_char
@cr:
      LDA   #13
      BRA   @store_escape_char
@tab:
      LDA   #9
      BRA   @store_escape_char
@vt:
      LDA   #11
      BRA   @store_escape_char
@nul:
      LDA   #0
      BRA   @store_escape_char
@crlf:
      LDA   #13
      STA   word_buf,Y
      INY
      LDA   #10
      BRA   @store_escape_char
@hex:
      INX
      LDA   input_buf,X
      JSR   hex_char_to_nibble
      BCC   @store_x
      STA   tmp2_lo
      INX
      LDA   input_buf,X
      JSR   hex_char_to_nibble
      BCC   @store_hex
      ASL   tmp2_lo
      ASL   tmp2_lo
      ASL   tmp2_lo
      ASL   tmp2_lo
      ORA   tmp2_lo
      STA   tmp2_lo
      INX
@store_hex:
      LDA   tmp2_lo
      STA   word_buf,Y
      INY
      JMP   @scan
@store_x:
      LDA   #'x'
      STA   word_buf,Y
      INY
      JMP   @scan
@store_escape_char:
      STA   word_buf,Y
      INY
      INX
      JMP   @scan
@finish_quote:
      INX
@finish:
      STX   tok_idx
      STZ   tok_idx_hi
      STY   tok_len
      RTS

code_dot_quote:
      JSR   parse_quote_string
      LDA   state
      BNE   @compile
      LDX   parse_idx
      LDY   #0
@print:
      CPY   tok_len
      BCS   @done
      LDA   input_buf,X
      STA   VGC_CHAROUT
      INX
      INY
      BRA   @print
@compile:
      LDA   #<word_dot_quote_runtime
      STA   exec_hdr_lo
      LDA   #>word_dot_quote_runtime
      STA   exec_hdr_hi
      JSR   compile_exec_header
      JSR   compile_quote_payload
@done:
      RTS

code_s_quote:
      JSR   parse_quote_string
      LDA   state
      BNE   @compile
      LDA   #<input_buf
      CLC
      ADC   parse_idx
      STA   value_lo
      LDA   #>input_buf
      ADC   #0
      STA   value_hi
      JSR   push_value
      LDA   tok_len
      STA   value_lo
      STZ   value_hi
      JSR   push_value
      RTS
@compile:
      LDA   #<word_s_quote_runtime
      STA   exec_hdr_lo
      LDA   #>word_s_quote_runtime
      STA   exec_hdr_hi
      JSR   compile_exec_header
      JSR   compile_quote_payload
      RTS

code_s_backslash_quote:
      JSR   parse_escaped_quote_string
      LDA   state
      BNE   @compile
      LDA   sbackslash_toggle
      EOR   #1
      STA   sbackslash_toggle
      BNE   @buf1
      LDA   #<sbackslash_buf0
      STA   value_lo
      STA   tmp_lo
      LDA   #>sbackslash_buf0
      STA   value_hi
      STA   tmp_hi
      BRA   @copy
@buf1:
      LDA   #<sbackslash_buf1
      STA   value_lo
      STA   tmp_lo
      LDA   #>sbackslash_buf1
      STA   value_hi
      STA   tmp_hi
@copy:
      LDY   #0
@copy_loop:
      CPY   tok_len
      BCS   @push
      LDA   word_buf,Y
      STA   (tmp_lo),Y
      INY
      BRA   @copy_loop
@push:
      JSR   push_value
      LDA   tok_len
      STA   value_lo
      STZ   value_hi
      JMP   push_value
@compile:
      LDA   #<word_s_quote_runtime
      STA   exec_hdr_lo
      LDA   #>word_s_quote_runtime
      STA   exec_hdr_hi
      JSR   compile_exec_header
      JSR   compile_wordbuf_payload
      RTS

code_sliteral:
      JSR   require_compiling
      BCS   @done
      JSR   pop_value
      BCS   @done
      LDA   value_hi
      BNE   @bad
      LDA   value_lo
      STA   tok_len
      JSR   pop_value
      BCS   @done
      LDA   value_lo
      STA   ptr_lo
      LDA   value_hi
      STA   ptr_hi
      LDA   #<word_s_quote_runtime
      STA   exec_hdr_lo
      LDA   #>word_s_quote_runtime
      STA   exec_hdr_hi
      JSR   compile_exec_header
      JSR   compile_ptr_payload
@done:
      RTS
@bad:
      JSR   print_inl
      .byte "BAD SLITERAL", $0D, $0A, 0
      JMP   raise_error

code_c_quote:
      JSR   require_compiling
      BCS   @done
      JSR   parse_quote_string
      LDA   #<word_c_quote_runtime
      STA   exec_hdr_lo
      LDA   #>word_c_quote_runtime
      STA   exec_hdr_hi
      JSR   compile_exec_header
      JSR   compile_quote_payload
@done:
      RTS

code_abort_quote:
      JSR   require_compiling
      BCS   @done
      JSR   parse_quote_string
      LDA   #<word_abort_quote_runtime
      STA   exec_hdr_lo
      LDA   #>word_abort_quote_runtime
      STA   exec_hdr_hi
      JSR   compile_exec_header
      JSR   compile_quote_payload
@done:
      RTS

; ---------------------------------------------------------------------
; Stack words
; ---------------------------------------------------------------------
code_dup:
      LDX   dsp
      CPX   #2
      BCS   @ok
      JMP   underflow_direct
@ok:
      LDA   dstack-2,X
      STA   value_lo
      LDA   dstack-1,X
      STA   value_hi
      JMP   push_value

code_drop:
      JSR   pop_value
      RTS

code_swap:
      LDX   dsp
      CPX   #4
      BCS   @ok
      JMP   underflow_direct
@ok:
      LDA   dstack-4,X
      STA   tmp_lo
      LDA   dstack-3,X
      STA   tmp_hi
      LDA   dstack-2,X
      STA   dstack-4,X
      LDA   dstack-1,X
      STA   dstack-3,X
      LDA   tmp_lo
      STA   dstack-2,X
      LDA   tmp_hi
      STA   dstack-1,X
      RTS

code_over:
      LDX   dsp
      CPX   #4
      BCS   @ok
      JMP   underflow_direct
@ok:
      LDA   dstack-4,X
      STA   value_lo
      LDA   dstack-3,X
      STA   value_hi
      JMP   push_value

code_rot:
      LDX   dsp
      CPX   #6
      BCS   @ok
      JMP   underflow_direct
@ok:
      LDA   dstack-6,X
      STA   tmp_lo
      LDA   dstack-5,X
      STA   tmp_hi
      LDA   dstack-4,X
      STA   dstack-6,X
      LDA   dstack-3,X
      STA   dstack-5,X
      LDA   dstack-2,X
      STA   dstack-4,X
      LDA   dstack-1,X
      STA   dstack-3,X
      LDA   tmp_lo
      STA   dstack-2,X
      LDA   tmp_hi
      STA   dstack-1,X
      RTS

code_depth:
      LDA   dsp
      LSR
      STA   value_lo
      STZ   value_hi
      JMP   push_value

code_2over:
      LDX   dsp
      CPX   #8
      BCS   @ok
      JMP   underflow_direct
@ok:
      LDA   dstack-8,X
      STA   value_lo
      LDA   dstack-7,X
      STA   value_hi
      JSR   push_value
      LDX   dsp
      LDA   dstack-8,X
      STA   value_lo
      LDA   dstack-7,X
      STA   value_hi
      JMP   push_value

code_2swap:
      LDX   dsp
      CPX   #8
      BCS   @ok
      JMP   underflow_direct
@ok:
      LDA   dstack-8,X
      STA   tmp_lo
      LDA   dstack-7,X
      STA   tmp_hi
      LDA   dstack-6,X
      STA   tmp2_lo
      LDA   dstack-5,X
      STA   tmp2_hi
      LDA   dstack-4,X
      STA   dstack-8,X
      LDA   dstack-3,X
      STA   dstack-7,X
      LDA   dstack-2,X
      STA   dstack-6,X
      LDA   dstack-1,X
      STA   dstack-5,X
      LDA   tmp_lo
      STA   dstack-4,X
      LDA   tmp_hi
      STA   dstack-3,X
      LDA   tmp2_lo
      STA   dstack-2,X
      LDA   tmp2_hi
      STA   dstack-1,X
      RTS

code_nip:
      LDX   dsp
      CPX   #4
      BCS   @ok
      JMP   underflow_direct
@ok:
      LDA   dstack-2,X
      STA   dstack-4,X
      LDA   dstack-1,X
      STA   dstack-3,X
      TXA
      SEC
      SBC   #2
      STA   dsp
      RTS

code_tuck:
      LDX   dsp
      CPX   #4
      BCS   @check_space
      JMP   underflow_direct
@check_space:
      CPX   #STACK_BYTES
      BCC   @ok
      JMP   stack_overflow_error
@ok:
      LDA   dstack-2,X
      STA   tmp_lo
      LDA   dstack-1,X
      STA   tmp_hi
      LDA   dstack-4,X
      STA   dstack-2,X
      LDA   dstack-3,X
      STA   dstack-1,X
      LDA   tmp_lo
      STA   dstack-4,X
      STA   dstack,X
      LDA   tmp_hi
      STA   dstack-3,X
      STA   dstack+1,X
      INX
      INX
      STX   dsp
      RTS

code_pick:
      JSR   pop_value
      BCS   @done
      LDA   value_hi
      BNE   @underflow
      LDA   value_lo
      ASL
      BCS   @underflow
      STA   char_tmp
      LDA   dsp
      SEC
      SBC   char_tmp
      BCC   @underflow
      SEC
      SBC   #2
      BCC   @underflow
      TAX
      LDA   dstack,X
      STA   value_lo
      LDA   dstack+1,X
      STA   value_hi
      JSR   push_value
@done:
      RTS
@underflow:
      JMP   stack_underflow_error

code_roll:
      JSR   pop_value
      BCS   @done
      LDA   value_hi
      BNE   @underflow
      LDA   value_lo
      BEQ   @done
      ASL
      BCS   @underflow
      STA   char_tmp
      LDA   dsp
      SEC
      SBC   char_tmp
      BCC   @underflow
      SEC
      SBC   #2
      BCC   @underflow
      TAX
      LDA   dstack,X
      STA   tmp_lo
      LDA   dstack+1,X
      STA   tmp_hi
      LDA   dsp
      SEC
      SBC   #2
      STA   arith0
@shift:
      CPX   arith0
      BEQ   @store_top
      LDA   dstack+2,X
      STA   dstack,X
      LDA   dstack+3,X
      STA   dstack+1,X
      INX
      INX
      BRA   @shift
@store_top:
      LDA   tmp_lo
      STA   dstack,X
      LDA   tmp_hi
      STA   dstack+1,X
@done:
      RTS
@underflow:
      JMP   stack_underflow_error

code_to_r:
      JSR   pop_value
      BCS   @done
      JSR   rpush_value
@done:
      RTS

code_2_to_r:
      LDX   dsp
      CPX   #4
      BCS   @check_space
      JMP   underflow_direct
@check_space:
      LDX   rsp
      CPX   #(RSTACK_BYTES - 2)
      BCC   @ok
      JMP   return_stack_overflow_error
@ok:
      JSR   pop_value
      BCS   @done
      JSR   value_to_tmp
      JSR   pop_value
      BCS   @done
      JSR   rpush_value
      BCS   @done
      JSR   tmp_to_value
      JSR   rpush_value
@done:
      RTS

code_r_from:
      JSR   rpop_value
      BCS   @done
      JMP   push_value
@done:
      RTS

code_2r_from:
      LDX   rsp
      CPX   #4
      BCS   @check_space
      JMP   return_stack_underflow_error
@check_space:
      LDX   dsp
      CPX   #(STACK_BYTES - 2)
      BCC   @ok
      JMP   stack_overflow_error
@ok:
      JSR   rpop_value
      BCS   @done
      JSR   value_to_tmp
      JSR   rpop_value
      BCS   @done
      JSR   push_value
      JSR   tmp_to_value
      JSR   push_value
@done:
      RTS

code_r_fetch:
      JSR   rpeek_value
      BCS   @done
      JMP   push_value
@done:
      RTS

code_2r_fetch:
      LDX   rsp
      CPX   #4
      BCS   @check_space
      JMP   return_stack_underflow_error
@check_space:
      LDX   dsp
      CPX   #(STACK_BYTES - 2)
      BCC   @ok
      JMP   stack_overflow_error
@ok:
      LDX   rsp
      LDA   rstack-4,X
      STA   value_lo
      LDA   rstack-3,X
      STA   value_hi
      JSR   push_value
      LDX   rsp
      LDA   rstack-2,X
      STA   value_lo
      LDA   rstack-1,X
      STA   value_hi
      JMP   push_value

underflow_direct:
      JMP   stack_underflow_error

; ---------------------------------------------------------------------
; Arithmetic and logic
; ---------------------------------------------------------------------
pop_binary:
      JSR   pop_value
      BCS   @err
      LDA   value_lo
      STA   tmp_lo
      LDA   value_hi
      STA   tmp_hi
      JSR   pop_value
      BCS   @err
      CLC
      RTS
@err:
      SEC
      RTS

divide_by_zero_error:
      JSR   print_inl
      .byte "DIVIDE BY ZERO", $0D, $0A, 0
      JSR   raise_error
      SEC
      RTS

signed_div_arith_by_tmp:
      LDA   tmp_lo
      ORA   tmp_hi
      BNE   @ok
      JMP   divide_by_zero_error
@ok:
      LDA   arith0
      STA   MATH_DIV_N_LO
      LDA   arith1
      STA   MATH_DIV_N_1
      LDA   arith2
      STA   MATH_DIV_N_2
      LDA   arith3
      STA   MATH_DIV_N_HI
      LDA   tmp_lo
      STA   MATH_DIV_D_LO
      LDA   tmp_hi
      STA   MATH_DIV_D_HI
      LDA   MATH_RES0
      STA   arith4             ; quotient low
      LDA   MATH_RES1
      STA   arith5             ; quotient high
      LDA   MATH_RES2
      STA   arith6             ; remainder low
      LDA   MATH_RES3
      STA   arith7             ; remainder high
      CLC
      RTS

unsigned_div_arith_by_tmp:
      LDA   tmp_lo
      ORA   tmp_hi
      BNE   @have_divisor
      JMP   divide_by_zero_error
@have_divisor:
      STZ   arith4             ; quotient
      STZ   arith5
      STZ   arith6
      STZ   arith7
      STZ   tmp2_lo            ; remainder
      STZ   tmp2_hi
      LDX   #32
@loop:
      ASL   arith0
      ROL   arith1
      ROL   arith2
      ROL   arith3
      ROL   tmp2_lo
      ROL   tmp2_hi
      LDA   #0
      ADC   #0
      STA   char_tmp
      ASL   arith4
      ROL   arith5
      ROL   arith6
      ROL   arith7
      LDA   char_tmp
      BNE   @subtract
      LDA   tmp2_hi
      CMP   tmp_hi
      BCC   @next
      BNE   @subtract
      LDA   tmp2_lo
      CMP   tmp_lo
      BCC   @next
@subtract:
      SEC
      LDA   tmp2_lo
      SBC   tmp_lo
      STA   tmp2_lo
      LDA   tmp2_hi
      SBC   tmp_hi
      STA   tmp2_hi
      INC   arith4
@next:
      DEX
      BNE   @loop
      CLC
      RTS

push_div_result:
      LDA   arith6
      STA   value_lo
      LDA   arith7
      STA   value_hi
      JSR   push_value
      LDA   arith4
      STA   value_lo
      LDA   arith5
      STA   value_hi
      JMP   push_value

code_plus:
      JSR   pop_binary
      BCS   @done
      CLC
      LDA   value_lo
      ADC   tmp_lo
      STA   value_lo
      LDA   value_hi
      ADC   tmp_hi
      STA   value_hi
      JSR   push_value
@done:
      RTS

code_minus:
      JSR   pop_binary
      BCS   @done
      SEC
      LDA   value_lo
      SBC   tmp_lo
      STA   value_lo
      LDA   value_hi
      SBC   tmp_hi
      STA   value_hi
      JSR   push_value
@done:
      RTS

code_star:
      JSR   pop_binary
      BCS   @done
      LDA   value_lo
      STA   MATH_MUL16_A_LO
      LDA   value_hi
      STA   MATH_MUL16_A_HI
      LDA   tmp_lo
      STA   MATH_MUL16_B_LO
      LDA   tmp_hi
      STA   MATH_MUL16_B_HI
      LDA   MATH_RES0
      STA   value_lo
      LDA   MATH_RES1
      STA   value_hi
      JSR   push_value
@done:
      RTS

code_umstar:
      JSR   pop_binary
      BCS   @done
      STZ   arith0             ; product
      STZ   arith1
      STZ   arith2
      STZ   arith3
      LDA   value_lo           ; shifted multiplicand
      STA   arith4
      LDA   value_hi
      STA   arith5
      STZ   arith6
      STZ   arith7
      LDX   #16
@loop:
      LDA   tmp_lo
      AND   #1
      BEQ   @shift
      CLC
      LDA   arith0
      ADC   arith4
      STA   arith0
      LDA   arith1
      ADC   arith5
      STA   arith1
      LDA   arith2
      ADC   arith6
      STA   arith2
      LDA   arith3
      ADC   arith7
      STA   arith3
@shift:
      ASL   arith4
      ROL   arith5
      ROL   arith6
      ROL   arith7
      LSR   tmp_hi
      ROR   tmp_lo
      DEX
      BNE   @loop
      LDA   arith0
      STA   value_lo
      LDA   arith1
      STA   value_hi
      JSR   push_value
      LDA   arith2
      STA   value_lo
      LDA   arith3
      STA   value_hi
      JSR   push_value
@done:
      RTS

code_mstar:
      JSR   pop_binary
      BCS   @done
      LDA   value_lo
      STA   MATH_MUL16_A_LO
      LDA   value_hi
      STA   MATH_MUL16_A_HI
      LDA   tmp_lo
      STA   MATH_MUL16_B_LO
      LDA   tmp_hi
      STA   MATH_MUL16_B_HI
      LDA   MATH_RES0
      STA   value_lo
      LDA   MATH_RES1
      STA   value_hi
      JSR   push_value
      LDA   MATH_RES2
      STA   value_lo
      LDA   MATH_RES3
      STA   value_hi
      JSR   push_value
@done:
      RTS

div_common:
      JSR   pop_binary
      BCS   @err
      LDA   tmp_lo
      ORA   tmp_hi
      BNE   @ok
      JMP   divide_by_zero_error
@ok:
      LDA   value_lo
      STA   MATH_DIV_N_LO
      LDA   value_hi
      STA   MATH_DIV_N_1
      LDA   value_hi
      BMI   @neg
      LDA   #0
      BRA   @ext
@neg:
      LDA   #$FF
@ext:
      STA   MATH_DIV_N_2
      STA   MATH_DIV_N_HI
      LDA   tmp_lo
      STA   MATH_DIV_D_LO
      LDA   tmp_hi
      STA   MATH_DIV_D_HI
      CLC
      RTS
@err:
      SEC
      RTS

code_slash_mod:
      JSR   div_common
      BCS   @done
      LDA   MATH_RES2
      STA   value_lo
      LDA   MATH_RES3
      STA   value_hi
      JSR   push_value
      LDA   MATH_RES0
      STA   value_lo
      LDA   MATH_RES1
      STA   value_hi
      JSR   push_value
@done:
      RTS

code_umslash_mod:
      JSR   pop_value            ; divisor
      BCC   @got_divisor
      RTS
@got_divisor:
      LDA   value_lo
      STA   tmp_lo
      LDA   value_hi
      STA   tmp_hi
      LDA   tmp_lo
      ORA   tmp_hi
      BNE   @have_divisor
      JMP   divide_by_zero_error
@have_divisor:
      JSR   pop_value            ; high cell
      BCC   @got_high
      RTS
@got_high:
      LDA   value_lo
      STA   arith2
      LDA   value_hi
      STA   arith3
      JSR   pop_value            ; low cell
      BCC   @got_low
      RTS
@got_low:
      LDA   value_lo
      STA   arith0
      LDA   value_hi
      STA   arith1
      JSR   unsigned_div_arith_by_tmp
      BCS   @done
      LDA   tmp2_lo
      STA   value_lo
      LDA   tmp2_hi
      STA   value_hi
      JSR   push_value
      LDA   arith4
      STA   value_lo
      LDA   arith5
      STA   value_hi
      JSR   push_value
@done:
      RTS

code_smrem:
      JSR   pop_value            ; divisor
      BCS   @done
      LDA   value_lo
      STA   tmp_lo
      LDA   value_hi
      STA   tmp_hi
      JSR   pop_value            ; high cell
      BCS   @done
      LDA   value_lo
      STA   arith2
      LDA   value_hi
      STA   arith3
      JSR   pop_value            ; low cell
      BCS   @done
      LDA   value_lo
      STA   arith0
      LDA   value_hi
      STA   arith1
      JSR   signed_div_arith_by_tmp
      BCS   @done
      JSR   push_div_result
@done:
      RTS

code_fmmod:
      JSR   pop_value            ; divisor
      BCS   @done
      LDA   value_lo
      STA   tmp_lo
      LDA   value_hi
      STA   tmp_hi
      JSR   pop_value            ; high cell
      BCS   @done
      LDA   value_lo
      STA   arith2
      LDA   value_hi
      STA   arith3
      JSR   pop_value            ; low cell
      BCS   @done
      LDA   value_lo
      STA   arith0
      LDA   value_hi
      STA   arith1
      JSR   signed_div_arith_by_tmp
      BCS   @done
      LDA   arith6
      ORA   arith7
      BEQ   @push
      LDA   arith3
      EOR   tmp_hi
      BPL   @push
      SEC
      LDA   arith4
      SBC   #1
      STA   arith4
      LDA   arith5
      SBC   #0
      STA   arith5
      CLC
      LDA   arith6
      ADC   tmp_lo
      STA   arith6
      LDA   arith7
      ADC   tmp_hi
      STA   arith7
@push:
      JSR   push_div_result
@done:
      RTS

code_star_slash_mod:
      JSR   pop_value            ; divisor
      BCS   @done
      LDA   value_lo
      STA   tmp_lo
      LDA   value_hi
      STA   tmp_hi
      JSR   pop_value            ; n2
      BCS   @done
      LDA   value_lo
      STA   tmp2_lo
      LDA   value_hi
      STA   tmp2_hi
      JSR   pop_value            ; n1
      BCS   @done
      LDA   value_lo
      STA   MATH_MUL16_A_LO
      LDA   value_hi
      STA   MATH_MUL16_A_HI
      LDA   tmp2_lo
      STA   MATH_MUL16_B_LO
      LDA   tmp2_hi
      STA   MATH_MUL16_B_HI
      LDA   MATH_RES0
      STA   arith0
      LDA   MATH_RES1
      STA   arith1
      LDA   MATH_RES2
      STA   arith2
      LDA   MATH_RES3
      STA   arith3
      JSR   signed_div_arith_by_tmp
      BCS   @done
      JSR   push_div_result
@done:
      RTS

code_twoslash:
      JSR   pop_value
      BCS   @done
      LDA   value_hi
      ASL
      ROR   value_hi
      ROR   value_lo
      JSR   push_value
@done:
      RTS

code_lshift:
      JSR   pop_binary
      BCS   @done
      LDA   tmp_hi
      BNE   @zero
      LDX   tmp_lo
      CPX   #16
      BCS   @zero
@loop:
      CPX   #0
      BEQ   @push
      ASL   value_lo
      ROL   value_hi
      DEX
      BRA   @loop
@zero:
      STZ   value_lo
      STZ   value_hi
@push:
      JSR   push_value
@done:
      RTS

code_rshift:
      JSR   pop_binary
      BCS   @done
      LDA   tmp_hi
      BNE   @zero
      LDX   tmp_lo
      CPX   #16
      BCS   @zero
@loop:
      CPX   #0
      BEQ   @push
      LSR   value_hi
      ROR   value_lo
      DEX
      BRA   @loop
@zero:
      STZ   value_lo
      STZ   value_hi
@push:
      JSR   push_value
@done:
      RTS

code_s_to_d:
      JSR   pop_value
      BCS   @done
      LDA   value_lo
      STA   tmp_lo
      LDA   value_hi
      STA   tmp_hi
      JSR   push_value
      LDA   tmp_hi
      BMI   @neg
      STZ   value_lo
      STZ   value_hi
      JMP   push_value
@neg:
      LDA   #$FF
      STA   value_lo
      STA   value_hi
      JSR   push_value
@done:
      RTS

push_true:
      LDA   #$FF
      STA   value_lo
      STA   value_hi
      JMP   push_value

push_false:
      STZ   value_lo
      STZ   value_hi
      JMP   push_value

code_equal:
      JSR   pop_binary
      BCS   @done
      LDA   value_lo
      CMP   tmp_lo
      BNE   @false
      LDA   value_hi
      CMP   tmp_hi
      BNE   @false
      JSR   push_true
      RTS
@false:
      JSR   push_false
@done:
      RTS

code_less:
      JSR   pop_binary
      BCS   @done
      JSR   signed_less_value_tmp
      BCS   @true
      JSR   push_false
      RTS
@true:
      JSR   push_true
@done:
      RTS

code_uless:
      JSR   pop_binary
      BCS   @done
      JSR   unsigned_less_value_tmp
      BCS   @true
      JSR   push_false
      RTS
@true:
      JSR   push_true
@done:
      RTS

signed_less_value_tmp:
      LDA   value_hi
      EOR   tmp_hi
      BMI   @diff_sign
      LDA   value_hi
      CMP   tmp_hi
      BCC   @true
      BNE   @false
      LDA   value_lo
      CMP   tmp_lo
      BCC   @true
@false:
      CLC
      RTS
@diff_sign:
      LDA   value_hi
      BMI   @true
      CLC
      RTS
@true:
      SEC
      RTS

unsigned_less_value_tmp:
      LDA   value_hi
      CMP   tmp_hi
      BCC   @true
      BNE   @false
      LDA   value_lo
      CMP   tmp_lo
      BCC   @true
@false:
      CLC
      RTS
@true:
      SEC
      RTS

code_and:
      JSR   pop_binary
      BCS   @done
      LDA   value_lo
      AND   tmp_lo
      STA   value_lo
      LDA   value_hi
      AND   tmp_hi
      STA   value_hi
      JSR   push_value
@done:
      RTS

code_or:
      JSR   pop_binary
      BCS   @done
      LDA   value_lo
      ORA   tmp_lo
      STA   value_lo
      LDA   value_hi
      ORA   tmp_hi
      STA   value_hi
      JSR   push_value
@done:
      RTS

code_xor:
      JSR   pop_binary
      BCS   @done
      LDA   value_lo
      EOR   tmp_lo
      STA   value_lo
      LDA   value_hi
      EOR   tmp_hi
      STA   value_hi
      JSR   push_value
@done:
      RTS

code_invert:
      JSR   pop_value
      BCS   @done
      LDA   value_lo
      EOR   #$FF
      STA   value_lo
      LDA   value_hi
      EOR   #$FF
      STA   value_hi
      JSR   push_value
@done:
      RTS

; ---------------------------------------------------------------------
; Memory and I/O
; ---------------------------------------------------------------------
code_fetch:
      JSR   pop_value_to_ptr
      BCS   @done
      LDY   #0
      LDA   (ptr_lo),Y
      STA   value_lo
      INY
      LDA   (ptr_lo),Y
      STA   value_hi
      JSR   push_value
@done:
      RTS

code_store:
      JSR   pop_value_to_ptr
      BCS   @done
      JSR   pop_value
      BCS   @done
      LDY   #0
      LDA   value_lo
      STA   (ptr_lo),Y
      INY
      LDA   value_hi
      STA   (ptr_lo),Y
@done:
      RTS

code_cfetch:
      JSR   pop_value_to_ptr
      BCS   @done
      LDY   #0
      LDA   (ptr_lo),Y
      STA   value_lo
      STZ   value_hi
      JSR   push_value
@done:
      RTS

code_cstore:
      JSR   pop_value_to_ptr
      BCS   @done
      JSR   pop_value
      BCS   @done
      LDY   #0
      LDA   value_lo
      STA   (ptr_lo),Y
@done:
      RTS

code_fill:
      JSR   pop_value            ; char
      BCS   @done
      LDA   value_lo
      STA   char_tmp
      JSR   pop_value            ; length
      BCS   @done
      JSR   value_to_tmp2
      JSR   pop_value_to_ptr     ; destination
      BCS   @done
      LDY   #0
@loop:
      JSR   tmp2_is_zero
      BEQ   @done
      LDA   char_tmp
      STA   (ptr_lo),Y
      JSR   inc_ptr
      JSR   dec_tmp2
      BRA   @loop
@done:
      RTS

code_move:
      JSR   pop_value            ; length
      BCS   @done
      JSR   value_to_tmp2
      JSR   pop_value_to_ptr     ; destination
      BCS   @done
      JSR   pop_value            ; source
      BCS   @done
      JSR   value_to_tmp
      JSR   tmp2_is_zero
      BEQ   @done

      LDA   ptr_hi
      CMP   tmp_hi
      BCC   @forward
      BNE   @backward
      LDA   ptr_lo
      CMP   tmp_lo
      BEQ   @done
      BCC   @forward

@backward:
      JSR   add_tmp2_to_ptr
      JSR   add_tmp2_to_tmp
      LDY   #0
@back_loop:
      JSR   tmp2_is_zero
      BEQ   @done
      JSR   dec_ptr
      JSR   dec_tmp
      LDA   (tmp_lo),Y
      STA   (ptr_lo),Y
      JSR   dec_tmp2
      BRA   @back_loop

@forward:
      LDY   #0
@fwd_loop:
      JSR   tmp2_is_zero
      BEQ   @done
      LDA   (tmp_lo),Y
      STA   (ptr_lo),Y
      JSR   inc_tmp
      JSR   inc_ptr
      JSR   dec_tmp2
      BRA   @fwd_loop
@done:
      RTS

code_emit:
      JSR   pop_value
      BCS   @done
      LDA   value_lo
      STA   VGC_CHAROUT
@done:
      RTS

code_spaces:
      JSR   pop_value
      BCS   @done
      LDA   value_hi
      BMI   @done
@loop:
      LDA   value_lo
      ORA   value_hi
      BEQ   @done
      LDA   #' '
      STA   VGC_CHAROUT
      LDA   value_lo
      BNE   @dec_lo
      DEC   value_hi
@dec_lo:
      DEC   value_lo
      BRA   @loop
@done:
      RTS

code_type:
      JSR   pop_value            ; length
      BCS   @done
      LDA   value_lo
      STA   char_tmp
      JSR   pop_value_to_ptr     ; address
      BCS   @done
      LDY   #0
@loop:
      CPY   char_tmp
      BCS   @done
      LDA   (ptr_lo),Y
      STA   VGC_CHAROUT
      INY
      BRA   @loop
@done:
      RTS

code_key:
      LDA   VGC_CHARIN
      BEQ   code_key
      STA   value_lo
      STZ   value_hi
      JMP   push_value

code_bye:
      STZ   VGC_CURSEN
@halt:
      BRA   @halt

code_abort:
      JMP   raise_error

; ---------------------------------------------------------------------
; Display words
; ---------------------------------------------------------------------
code_dot:
      JSR   pop_value
      BCS   @done
      JSR   print_cell
      LDA   #' '
      STA   VGC_CHAROUT
@done:
      RTS

code_udot:
      JSR   pop_value
      BCS   @done
      JSR   print_uint16
      LDA   #' '
      STA   VGC_CHAROUT
@done:
      RTS

hold_char_tmp:
      LDA   pictured_idx
      BEQ   @done
      DEC   pictured_idx
      LDY   pictured_idx
      LDA   char_tmp
      STA   pictured_buf,Y
@done:
      RTS

digit_to_char_tmp:
      LDA   tmp2_lo
      CMP   #10
      BCC   @digit
      CLC
      ADC   #'A'-10
      STA   char_tmp
      RTS
@digit:
      CLC
      ADC   #'0'
      STA   char_tmp
      RTS

multiply_arith_by_base_add_digit:
      LDA   arith0
      STA   arith4
      LDA   arith1
      STA   arith5
      LDA   arith2
      STA   arith6
      LDA   arith3
      STA   arith7
      STZ   arith0
      STZ   arith1
      STZ   arith2
      STZ   arith3
      LDX   base_cell
@mul_loop:
      CPX   #0
      BEQ   @add_digit
      CLC
      LDA   arith0
      ADC   arith4
      STA   arith0
      LDA   arith1
      ADC   arith5
      STA   arith1
      LDA   arith2
      ADC   arith6
      STA   arith2
      LDA   arith3
      ADC   arith7
      STA   arith3
      DEX
      BRA   @mul_loop
@add_digit:
      CLC
      LDA   arith0
      ADC   char_tmp
      STA   arith0
      LDA   arith1
      ADC   #0
      STA   arith1
      LDA   arith2
      ADC   #0
      STA   arith2
      LDA   arith3
      ADC   #0
      STA   arith3
      RTS

code_less_number_sign:
      LDA   #PICTURED_SIZE
      STA   pictured_idx
      RTS

code_hold:
      JSR   pop_value
      BCS   @done
      LDA   value_lo
      STA   char_tmp
      JSR   hold_char_tmp
@done:
      RTS

code_number_sign:
      JSR   pop_value            ; high cell
      BCS   @done
      LDA   value_lo
      STA   arith2
      LDA   value_hi
      STA   arith3
      JSR   pop_value            ; low cell
      BCS   @done
      LDA   value_lo
      STA   arith0
      LDA   value_hi
      STA   arith1
      LDA   base_cell
      STA   tmp_lo
      LDA   base_cell+1
      STA   tmp_hi
      JSR   unsigned_div_arith_by_tmp
      BCS   @done
      JSR   digit_to_char_tmp
      JSR   hold_char_tmp
      LDA   arith4
      STA   value_lo
      LDA   arith5
      STA   value_hi
      JSR   push_value
      LDA   arith6
      STA   value_lo
      LDA   arith7
      STA   value_hi
      JSR   push_value
@done:
      RTS

code_number_sign_s:
@loop:
      JSR   code_number_sign
      LDA   abort_flag
      BNE   @done
      LDX   dsp
      CPX   #4
      BCC   @done
      LDA   dstack-4,X
      ORA   dstack-3,X
      ORA   dstack-2,X
      ORA   dstack-1,X
      BNE   @loop
@done:
      RTS

code_number_sign_greater:
      JSR   pop_value            ; high cell
      BCS   @done
      JSR   pop_value            ; low cell
      BCS   @done
      LDA   #<pictured_buf
      CLC
      ADC   pictured_idx
      STA   value_lo
      LDA   #>pictured_buf
      ADC   #0
      STA   value_hi
      JSR   push_value
      LDA   #PICTURED_SIZE
      SEC
      SBC   pictured_idx
      STA   value_lo
      STZ   value_hi
      JSR   push_value
@done:
      RTS

code_sign:
      JSR   pop_value
      BCS   @done
      LDA   value_hi
      BPL   @done
      LDA   #'-'
      STA   char_tmp
      JSR   hold_char_tmp
@done:
      RTS

code_to_number:
      JSR   pop_value            ; length
      BCC   @got_len
      RTS
@got_len:
      LDA   value_lo
      STA   tmp2_lo
      LDA   value_hi
      STA   tmp2_hi
      JSR   pop_value_to_ptr     ; address
      BCC   @got_addr
      RTS
@got_addr:
      JSR   pop_value            ; high cell
      BCC   @got_high
      RTS
@got_high:
      LDA   value_lo
      STA   arith2
      LDA   value_hi
      STA   arith3
      JSR   pop_value            ; low cell
      BCC   @got_low
      RTS
@got_low:
      LDA   value_lo
      STA   arith0
      LDA   value_hi
      STA   arith1
@loop:
      JSR   tmp2_is_zero
      BEQ   @push
      LDY   #0
      LDA   (ptr_lo),Y
      CMP   #'0'
      BCC   @push
      CMP   #'9'+1
      BCC   @decimal_digit
      CMP   #'a'
      BCC   @upper
      CMP   #'z'+1
      BCS   @upper
      AND   #$DF
@upper:
      CMP   #'A'
      BCC   @push
      CMP   #'Z'+1
      BCS   @push
      SEC
      SBC   #'A'-10
      BRA   @have_digit
@decimal_digit:
      SEC
      SBC   #'0'
@have_digit:
      STA   char_tmp
      LDA   base_cell+1
      BNE   @push
      LDA   char_tmp
      CMP   base_cell
      BCS   @push
      JSR   multiply_arith_by_base_add_digit
      JSR   inc_ptr
      JSR   dec_tmp2
      BRA   @loop
@push:
      LDA   arith0
      STA   value_lo
      LDA   arith1
      STA   value_hi
      JSR   push_value
      LDA   arith2
      STA   value_lo
      LDA   arith3
      STA   value_hi
      JSR   push_value
      LDA   ptr_lo
      STA   value_lo
      LDA   ptr_hi
      STA   value_hi
      JSR   push_value
      LDA   tmp2_lo
      STA   value_lo
      LDA   tmp2_hi
      STA   value_hi
      JSR   push_value
@done:
      RTS

code_dots:
      LDA   #'<'
      STA   VGC_CHAROUT
      LDA   dsp
      LSR
      STA   value_lo
      STZ   value_hi
      JSR   print_uint16
      JSR   print_inl
      .byte "> ", 0
      LDX   #0
@loop:
      CPX   dsp
      BCS   @done
      LDA   dstack,X
      STA   value_lo
      INX
      LDA   dstack,X
      STA   value_hi
      INX
      PHX
      JSR   print_cell
      LDA   #' '
      STA   VGC_CHAROUT
      PLX
      BRA   @loop
@done:
      RTS

code_words:
      LDA   latest_lo
      STA   ptr_lo
      LDA   latest_hi
      STA   ptr_hi
@loop:
      LDA   ptr_lo
      ORA   ptr_hi
      BEQ   @done
      LDY   #2
      LDA   (ptr_lo),Y
      AND   #LEN_MASK
      STA   tok_len
      LDX   #0
@name:
      CPX   tok_len
      BEQ   @space
      TXA
      CLC
      ADC   #3
      TAY
      LDA   (ptr_lo),Y
      STA   VGC_CHAROUT
      INX
      BRA   @name
@space:
      LDA   #' '
      STA   VGC_CHAROUT
      LDY   #0
      LDA   (ptr_lo),Y
      STA   tmp_lo
      INY
      LDA   (ptr_lo),Y
      STA   tmp_hi
      LDA   tmp_lo
      STA   ptr_lo
      LDA   tmp_hi
      STA   ptr_hi
      BRA   @loop
@done:
      JMP   newline

; ---------------------------------------------------------------------
; Number parsing and printing
; ---------------------------------------------------------------------
parse_number:
      STZ   value_lo
      STZ   value_hi
      STZ   sign_flag
      STZ   parse_digits
      LDA   base_cell
      STA   parse_base
      STZ   parse_idx
      LDA   tok_len
      BEQ   @bad

      LDY   #0
      LDA   (tok_start_lo),Y
      CMP   #'-'
      BNE   @base
      INC   sign_flag
      INC   parse_idx
@base:
      LDY   parse_idx
      CPY   tok_len
      BCS   @bad
      LDA   (tok_start_lo),Y
      CMP   #'$'
      BNE   @digits
      LDA   #16
      STA   parse_base
      INC   parse_idx

@digits:
      LDY   parse_idx
      CPY   tok_len
      BCS   @maybe_done
      LDA   (tok_start_lo),Y
      JSR   digit_value
      BCC   @bad
      LDA   char_tmp
      CMP   parse_base
      BCS   @bad
      JSR   value_times_base
@add:
      CLC
      LDA   value_lo
      ADC   char_tmp
      STA   value_lo
      LDA   value_hi
      ADC   #0
      STA   value_hi
      INC   parse_digits
      INC   parse_idx
      BRA   @digits

@maybe_done:
      LDA   parse_digits
      BEQ   @bad
      LDA   sign_flag
      BEQ   @good
      SEC
      LDA   #0
      SBC   value_lo
      STA   value_lo
      LDA   #0
      SBC   value_hi
      STA   value_hi
@good:
      SEC
      RTS

@bad:
      CLC
      RTS

digit_value:
      CMP   #'0'
      BCC   @bad
      CMP   #'9'+1
      BCC   @dec
      LDX   parse_base
      CPX   #16
      BNE   @bad
      CMP   #'A'
      BCC   @bad
      CMP   #'F'+1
      BCS   @bad
      SEC
      SBC   #'A'-10
      STA   char_tmp
      SEC
      RTS
@dec:
      SEC
      SBC   #'0'
      STA   char_tmp
      SEC
      RTS
@bad:
      CLC
      RTS

value_times_base:
      LDA   parse_base
      CMP   #16
      BEQ   value_times_16
      CMP   #10
      BEQ   value_times_10
      LDA   value_lo
      STA   tmp_lo
      LDA   value_hi
      STA   tmp_hi
      STZ   value_lo
      STZ   value_hi
      LDX   parse_base
@loop:
      BEQ   @done
      CLC
      LDA   value_lo
      ADC   tmp_lo
      STA   value_lo
      LDA   value_hi
      ADC   tmp_hi
      STA   value_hi
      DEX
      BRA   @loop
@done:
      RTS

value_times_10:
      LDA   value_lo
      STA   tmp_lo
      LDA   value_hi
      STA   tmp_hi
      ASL   value_lo
      ROL   value_hi          ; *2
      ASL   tmp_lo
      ROL   tmp_hi
      ASL   tmp_lo
      ROL   tmp_hi
      ASL   tmp_lo
      ROL   tmp_hi            ; original *8
      CLC
      LDA   value_lo
      ADC   tmp_lo
      STA   value_lo
      LDA   value_hi
      ADC   tmp_hi
      STA   value_hi
      RTS

value_times_16:
      ASL   value_lo
      ROL   value_hi
      ASL   value_lo
      ROL   value_hi
      ASL   value_lo
      ROL   value_hi
      ASL   value_lo
      ROL   value_hi
      RTS

print_token:
      LDY   #0
@loop:
      CPY   tok_len
      BCS   @done
      LDA   (tok_start_lo),Y
      STA   VGC_CHAROUT
      INY
      BRA   @loop
@done:
      RTS

print_sint16:
      LDA   value_hi
      BPL   print_uint16
      LDA   #'-'
      STA   VGC_CHAROUT
      SEC
      LDA   #0
      SBC   value_lo
      STA   value_lo
      LDA   #0
      SBC   value_hi
      STA   value_hi
      JMP   print_uint16

print_cell:
      LDA   base_cell
      CMP   #16
      BEQ   print_hex16
      JMP   print_sint16

print_hex16:
      LDA   value_hi
      BNE   @word
      LDA   value_lo
      CMP   #$10
      BCS   @byte
      JMP   print_hex_nibble
@word:
      LDA   value_hi
      JSR   print_hex_byte
@byte:
      LDA   value_lo
      JMP   print_hex_byte

print_hex_byte:
      PHA
      LSR
      LSR
      LSR
      LSR
      JSR   print_hex_nibble
      PLA
      AND   #$0F
print_hex_nibble:
      CMP   #10
      BCC   @digit
      CLC
      ADC   #'A'-10
      STA   VGC_CHAROUT
      RTS
@digit:
      CLC
      ADC   #'0'
      STA   VGC_CHAROUT
      RTS

print_uint16:
      LDA   value_lo
      STA   num_lo
      LDA   value_hi
      STA   num_hi
      LDX   #0
      LDY   #0
@div_loop:
      STZ   num_tmp_lo
@sub_loop:
      SEC
      LDA   num_lo
      SBC   pow10_lo,Y
      PHA
      LDA   num_hi
      SBC   pow10_hi,Y
      BCC   @sub_done
      STA   num_hi
      PLA
      STA   num_lo
      INC   num_tmp_lo
      BRA   @sub_loop
@sub_done:
      PLA
      LDA   num_tmp_lo
      STA   pn_buf,X
      INX
      INY
      CPY   #5
      BCC   @div_loop
      LDY   #0
      LDX   #0
@print_loop:
      LDA   pn_buf,X
      BNE   @nonzero
      CPY   #0
      BEQ   @maybe_skip
@nonzero:
      LDY   #1
      ORA   #'0'
      STA   VGC_CHAROUT
@maybe_skip:
      INX
      CPX   #4
      BCC   @print_loop
      LDA   pn_buf+4
      ORA   #'0'
      STA   VGC_CHAROUT
      RTS

newline:
      LDA   #$0D
      STA   VGC_CHAROUT
      LDA   #$0A
      STA   VGC_CHAROUT
      RTS

print_inl:
      PLA
      STA   ptr_lo
      PLA
      STA   ptr_hi
      LDY   #0
@loop:
      INY
      LDA   (ptr_lo),Y
      BEQ   @done
      STA   VGC_CHAROUT
      BRA   @loop
@done:
      TYA
      CLC
      ADC   ptr_lo
      STA   ptr_lo
      LDA   ptr_hi
      ADC   #0
      PHA
      LDA   ptr_lo
      PHA
      RTS

; =====================================================================
; RODATA dictionary
; =====================================================================
      .segment "RODATA"

autoload_core_name:
      .byte "forth/lib/core.4th"
autoload_core_name_len = * - autoload_core_name
autoload_autoexec_name:
      .byte "forth/autoexec.4th"
autoload_autoexec_name_len = * - autoload_autoexec_name

; length, kind, value cell 1, value cell 2, uppercase query string
env_query_table:
      .byte 15, ENV_KIND_SINGLE
      .word 255, 0
      .byte "/COUNTED-STRING"
      .byte 5, ENV_KIND_SINGLE
      .word PICTURED_SIZE, 0
      .byte "/HOLD"
      .byte 17, ENV_KIND_SINGLE
      .word 8, 0
      .byte "ADDRESS-UNIT-BITS"
      .byte 7, ENV_KIND_FLAG
      .word 0, 0
      .byte "FLOORED"
      .byte 8, ENV_KIND_SINGLE
      .word 255, 0
      .byte "MAX-CHAR"
      .byte 5, ENV_KIND_DOUBLE
      .word $FFFF, $7FFF
      .byte "MAX-D"
      .byte 5, ENV_KIND_SINGLE
      .word $7FFF, 0
      .byte "MAX-N"
      .byte 5, ENV_KIND_SINGLE
      .word $FFFF, 0
      .byte "MAX-U"
      .byte 6, ENV_KIND_DOUBLE
      .word $FFFF, $FFFF
      .byte "MAX-UD"
      .byte 18, ENV_KIND_SINGLE
      .word RSTACK_BYTES / CELL_BYTES, 0
      .byte "RETURN-STACK-CELLS"
      .byte 11, ENV_KIND_SINGLE
      .word STACK_BYTES / CELL_BYTES, 0
      .byte "STACK-CELLS"
      .byte 4, ENV_KIND_FLAG
      .word $FFFF, 0
      .byte "CORE"
      .byte 8, ENV_KIND_FLAG
      .word $FFFF, 0
      .byte "CORE-EXT"
      .byte 5, ENV_KIND_FLAG
      .word 0, 0
      .byte "BLOCK"
      .byte 9, ENV_KIND_FLAG
      .word 0, 0
      .byte "BLOCK-EXT"
      .byte 6, ENV_KIND_FLAG
      .word 0, 0
      .byte "DOUBLE"
      .byte 10, ENV_KIND_FLAG
      .word 0, 0
      .byte "DOUBLE-EXT"
      .byte 9, ENV_KIND_FLAG
      .word 0, 0
      .byte "EXCEPTION"
      .byte 13, ENV_KIND_FLAG
      .word 0, 0
      .byte "EXCEPTION-EXT"
      .byte 8, ENV_KIND_FLAG
      .word 0, 0
      .byte "FACILITY"
      .byte 12, ENV_KIND_FLAG
      .word 0, 0
      .byte "FACILITY-EXT"
      .byte 4, ENV_KIND_FLAG
      .word $FFFF, 0
      .byte "FILE"
      .byte 8, ENV_KIND_FLAG
      .word $FFFF, 0
      .byte "FILE-EXT"
      .byte 8, ENV_KIND_FLAG
      .word 0, 0
      .byte "FLOATING"
      .byte 12, ENV_KIND_FLAG
      .word 0, 0
      .byte "FLOATING-EXT"
      .byte 6, ENV_KIND_FLAG
      .word 0, 0
      .byte "LOCALS"
      .byte 10, ENV_KIND_FLAG
      .word 0, 0
      .byte "LOCALS-EXT"
      .byte 12, ENV_KIND_FLAG
      .word 0, 0
      .byte "MEMORY-ALLOC"
      .byte 16, ENV_KIND_FLAG
      .word 0, 0
      .byte "MEMORY-ALLOC-EXT"
      .byte 5, ENV_KIND_FLAG
      .word 0, 0
      .byte "TOOLS"
      .byte 9, ENV_KIND_FLAG
      .word 0, 0
      .byte "TOOLS-EXT"
      .byte 12, ENV_KIND_FLAG
      .word 0, 0
      .byte "SEARCH-ORDER"
      .byte 16, ENV_KIND_FLAG
      .word 0, 0
      .byte "SEARCH-ORDER-EXT"
      .byte 6, ENV_KIND_FLAG
      .word $FFFF, 0
      .byte "STRING"
      .byte 10, ENV_KIND_FLAG
      .word $FFFF, 0
      .byte "STRING-EXT"
      .byte 0

pow10_lo: .byte <10000, <1000, <100, <10, <1
pow10_hi: .byte >10000, >1000, >100, >10, >1

word_lit:       .word 0          ; first word
                .byte 5
                .byte "(LIT)"
                .word code_lit
word_branch:    .word word_lit
                .byte 8
                .byte "(BRANCH)"
                .word code_branch
word_0branch:   .word word_branch
                .byte 9
                .byte "(0BRANCH)"
                .word code_0branch
word_dot_quote_runtime:
                .word word_0branch
                .byte 4
                .byte '(', '.', '"', ')'
                .word code_dot_quote_runtime
word_s_quote_runtime:
                .word word_dot_quote_runtime
                .byte 4
                .byte '(', 'S', '"', ')'
                .word code_s_quote_runtime
word_c_quote_runtime:
                .word word_s_quote_runtime
                .byte 4
                .byte '(', 'C', '"', ')'
                .word code_c_quote_runtime
word_abort_quote_runtime:
                .word word_c_quote_runtime
                .byte 8
                .byte '(', 'A', 'B', 'O', 'R', 'T', '"', ')'
                .word code_abort_quote_runtime
word_do_runtime:
                .word word_abort_quote_runtime
                .byte 4
                .byte '(', 'D', 'O', ')'
                .word code_do_runtime
word_qdo_runtime:
                .word word_do_runtime
                .byte 5
                .byte '(', '?', 'D', 'O', ')'
                .word code_qdo_runtime
word_loop_runtime:
                .word word_qdo_runtime
                .byte 6
                .byte '(', 'L', 'O', 'O', 'P', ')'
                .word code_loop_runtime
word_plus_loop_runtime:
                .word word_loop_runtime
                .byte 7
                .byte '(', '+', 'L', 'O', 'O', 'P', ')'
                .word code_plus_loop_runtime
word_does_runtime:
                .word word_plus_loop_runtime
                .byte 7
                .byte '(', 'D', 'O', 'E', 'S', '>', ')'
                .word code_does_runtime
word_exit:      .word word_does_runtime
                .byte 4
                .byte "EXIT"
                .word code_exit
word_dup:       .word word_exit
                .byte 3
                .byte "DUP"
                .word code_dup
word_drop:      .word word_dup
                .byte 4
                .byte "DROP"
                .word code_drop
word_swap:      .word word_drop
                .byte 4
                .byte "SWAP"
                .word code_swap
word_over:      .word word_swap
                .byte 4
                .byte "OVER"
                .word code_over
word_rot:       .word word_over
                .byte 3
                .byte "ROT"
                .word code_rot
word_depth:     .word word_rot
                .byte 5
                .byte "DEPTH"
                .word code_depth
word_2over:     .word word_depth
                .byte 5
                .byte "2OVER"
                .word code_2over
word_2swap:     .word word_2over
                .byte 5
                .byte "2SWAP"
                .word code_2swap
word_nip:       .word word_2swap
                .byte 3
                .byte "NIP"
                .word code_nip
word_tuck:      .word word_nip
                .byte 4
                .byte "TUCK"
                .word code_tuck
word_pick:      .word word_tuck
                .byte 4
                .byte "PICK"
                .word code_pick
word_roll:      .word word_pick
                .byte 4
                .byte "ROLL"
                .word code_roll
word_to_r:      .word word_roll
                .byte 2
                .byte ">R"
                .word code_to_r
word_2_to_r:    .word word_to_r
                .byte 3
                .byte "2>R"
                .word code_2_to_r
word_r_from:    .word word_2_to_r
                .byte 2
                .byte "R>"
                .word code_r_from
word_2r_from:   .word word_r_from
                .byte 3
                .byte "2R>"
                .word code_2r_from
word_r_fetch:   .word word_2r_from
                .byte 2
                .byte "R@"
                .word code_r_fetch
word_2r_fetch:  .word word_r_fetch
                .byte 3
                .byte "2R@"
                .word code_2r_fetch
word_i:         .word word_2r_fetch
                .byte 1
                .byte "I"
                .word code_i
word_j:         .word word_i
                .byte 1
                .byte "J"
                .word code_j
word_unloop:    .word word_j
                .byte 6
                .byte "UNLOOP"
                .word code_unloop
word_dot:       .word word_unloop
                .byte 1
                .byte "."
                .word code_dot
word_udot:      .word word_dot
                .byte 2
                .byte "U."
                .word code_udot
word_less_number_sign:
                .word word_udot
                .byte 2
                .byte "<#"
                .word code_less_number_sign
word_number_sign:
                .word word_less_number_sign
                .byte 1
                .byte "#"
                .word code_number_sign
word_number_sign_s:
                .word word_number_sign
                .byte 2
                .byte "#S"
                .word code_number_sign_s
word_number_sign_greater:
                .word word_number_sign_s
                .byte 2
                .byte "#>"
                .word code_number_sign_greater
word_hold:      .word word_number_sign_greater
                .byte 4
                .byte "HOLD"
                .word code_hold
word_sign:      .word word_hold
                .byte 4
                .byte "SIGN"
                .word code_sign
word_to_number:
                .word word_sign
                .byte 7
                .byte ">NUMBER"
                .word code_to_number
word_dots:      .word word_to_number
                .byte 2
                .byte ".S"
                .word code_dots
word_plus:      .word word_dots
                .byte 1
                .byte "+"
                .word code_plus
word_minus:     .word word_plus
                .byte 1
                .byte "-"
                .word code_minus
word_star:      .word word_minus
                .byte 1
                .byte "*"
                .word code_star
word_umstar:    .word word_star
                .byte 3
                .byte "UM*"
                .word code_umstar
word_mstar:     .word word_umstar
                .byte 2
                .byte "M*"
                .word code_mstar
word_slash_mod:
                .word word_mstar
                .byte 4
                .byte "/MOD"
                .word code_slash_mod
word_umslash_mod:
                .word word_slash_mod
                .byte 6
                .byte "UM/MOD"
                .word code_umslash_mod
word_smrem:     .word word_umslash_mod
                .byte 6
                .byte "SM/REM"
                .word code_smrem
word_fmmod:     .word word_smrem
                .byte 6
                .byte "FM/MOD"
                .word code_fmmod
word_star_slash_mod:
                .word word_fmmod
                .byte 5
                .byte "*/MOD"
                .word code_star_slash_mod
word_twoslash:  .word word_star_slash_mod
                .byte 2
                .byte "2/"
                .word code_twoslash
word_lshift:    .word word_twoslash
                .byte 6
                .byte "LSHIFT"
                .word code_lshift
word_rshift:    .word word_lshift
                .byte 6
                .byte "RSHIFT"
                .word code_rshift
word_s_to_d:    .word word_rshift
                .byte 3
                .byte "S>D"
                .word code_s_to_d
word_equal:     .word word_s_to_d
                .byte 1
                .byte "="
                .word code_equal
word_less:      .word word_equal
                .byte 1
                .byte "<"
                .word code_less
word_uless:     .word word_less
                .byte 2
                .byte "U<"
                .word code_uless
word_and:       .word word_uless
                .byte 3
                .byte "AND"
                .word code_and
word_or:        .word word_and
                .byte 2
                .byte "OR"
                .word code_or
word_xor:       .word word_or
                .byte 3
                .byte "XOR"
                .word code_xor
word_invert:    .word word_xor
                .byte 6
                .byte "INVERT"
                .word code_invert
word_fetch:     .word word_invert
                .byte 1
                .byte "@"
                .word code_fetch
word_store:     .word word_fetch
                .byte 1
                .byte "!"
                .word code_store
word_cfetch:    .word word_store
                .byte 2
                .byte "C@"
                .word code_cfetch
word_cstore:    .word word_cfetch
                .byte 2
                .byte "C!"
                .word code_cstore
word_fill:      .word word_cstore
                .byte 4
                .byte "FILL"
                .word code_fill
word_move:      .word word_fill
                .byte 4
                .byte "MOVE"
                .word code_move
word_colon:     .word word_move
                .byte 1
                .byte ":"
                .word code_colon
word_noname:    .word word_colon
                .byte 7
                .byte ":NONAME"
                .word code_noname
word_semicolon: .word word_noname
                .byte FLAG_IMM | 1
                .byte ";"
                .word code_semicolon
word_create:    .word word_semicolon
                .byte 6
                .byte "CREATE"
                .word code_create
word_marker:    .word word_create
                .byte 6
                .byte "MARKER"
                .word code_marker
word_allot:     .word word_marker
                .byte 5
                .byte "ALLOT"
                .word code_allot
word_here:      .word word_allot
                .byte 4
                .byte "HERE"
                .word code_here
word_unused:    .word word_here
                .byte 6
                .byte "UNUSED"
                .word code_unused
word_comma:     .word word_unused
                .byte 1
                .byte ","
                .word code_comma
word_ccomma:    .word word_comma
                .byte 2
                .byte "C,"
                .word code_ccomma
word_base:      .word word_ccomma
                .byte 4
                .byte "BASE"
                .word code_base
word_state:     .word word_base
                .byte 5
                .byte "STATE"
                .word code_state_word
word_source:    .word word_state
                .byte 6
                .byte "SOURCE"
                .word code_source
word_to_in:     .word word_source
                .byte 3
                .byte ">IN"
                .word code_to_in
word_source_id:
                .word word_to_in
                .byte 9
                .byte "SOURCE-ID"
                .word code_source_id
word_refill:    .word word_source_id
                .byte 6
                .byte "REFILL"
                .word code_refill
word_save_input:
                .word word_refill
                .byte 10
                .byte "SAVE-INPUT"
                .word code_save_input
word_restore_input:
                .word word_save_input
                .byte 13
                .byte "RESTORE-INPUT"
                .word code_restore_input
word_parse_name:
                .word word_restore_input
                .byte 10
                .byte "PARSE-NAME"
                .word code_parse_name
word_parse:     .word word_parse_name
                .byte 5
                .byte "PARSE"
                .word code_parse
word_word:      .word word_parse
                .byte 4
                .byte "WORD"
                .word code_word
word_find:      .word word_word
                .byte 4
                .byte "FIND"
                .word code_find
word_evaluate:  .word word_find
                .byte 8
                .byte "EVALUATE"
                .word code_evaluate
word_included:  .word word_evaluate
                .byte 8
                .byte "INCLUDED"
                .word code_included
word_include:   .word word_included
                .byte 7
                .byte "INCLUDE"
                .word code_include
word_edit:      .word word_include
                .byte 4
                .byte "EDIT"
                .word code_edit
word_save_forth:
                .word word_edit
                .byte 10
                .byte "SAVE-FORTH"
                .word code_save_forth
word_nova_libcall:
                .word word_save_forth
                .byte 12
                .byte "NOVA-LIBCALL"
                .word code_nova_libcall
word_accept:    .word word_nova_libcall
                .byte 6
                .byte "ACCEPT"
                .word code_accept
word_quit:      .word word_accept
                .byte 4
                .byte "QUIT"
                .word code_quit
word_environment_query:
                .word word_quit
                .byte 12
                .byte "ENVIRONMENT?"
                .word code_environment_query
word_char:      .word word_environment_query
                .byte 4
                .byte "CHAR"
                .word code_char
word_tick:      .word word_char
                .byte 1
                .byte "'"
                .word code_tick
word_bracket_tick:
                .word word_tick
                .byte FLAG_IMM | 3
                .byte "[']"
                .word code_bracket_tick
word_execute:   .word word_bracket_tick
                .byte 7
                .byte "EXECUTE"
                .word code_execute
word_compile_comma:
                .word word_execute
                .byte 8
                .byte "COMPILE,"
                .word code_compile_comma
word_literal:   .word word_compile_comma
                .byte FLAG_IMM | 7
                .byte "LITERAL"
                .word code_literal_word
word_immediate: .word word_literal
                .byte 9
                .byte "IMMEDIATE"
                .word code_immediate
word_left_bracket:
                .word word_immediate
                .byte FLAG_IMM | 1
                .byte "["
                .word code_left_bracket
word_right_bracket:
                .word word_left_bracket
                .byte 1
                .byte "]"
                .word code_right_bracket
word_postpone:  .word word_right_bracket
                .byte FLAG_IMM | 8
                .byte "POSTPONE"
                .word code_postpone
word_recurse:   .word word_postpone
                .byte FLAG_IMM | 7
                .byte "RECURSE"
                .word code_recurse
word_to_body:   .word word_recurse
                .byte 5
                .byte ">BODY"
                .word code_to_body
word_to:        .word word_to_body
                .byte FLAG_IMM | 2
                .byte "TO"
                .word code_to
word_does:      .word word_to
                .byte FLAG_IMM | 5
                .byte "DOES>"
                .word code_does
word_paren:     .word word_does
                .byte FLAG_IMM | 1
                .byte "("
                .word code_paren
word_backslash: .word word_paren
                .byte FLAG_IMM | 1
                .byte $5C
                .word code_backslash
word_bracket_char:
                .word word_backslash
                .byte FLAG_IMM | 6
                .byte "[CHAR]"
                .word code_bracket_char
word_dot_paren: .word word_bracket_char
                .byte FLAG_IMM | 2
                .byte '.', '('
                .word code_dot_paren
word_dot_quote: .word word_dot_paren
                .byte FLAG_IMM | 2
                .byte '.', '"'
                .word code_dot_quote
word_s_quote:   .word word_dot_quote
                .byte FLAG_IMM | 2
                .byte 'S', '"'
                .word code_s_quote
word_s_backslash_quote:
                .word word_sliteral
                .byte FLAG_IMM | 3
                .byte 'S', $5C, '"'
                .word code_s_backslash_quote
word_sliteral:  .word word_s_quote
                .byte FLAG_IMM | 8
                .byte "SLITERAL"
                .word code_sliteral
word_c_quote:   .word word_s_backslash_quote
                .byte FLAG_IMM | 2
                .byte 'C', '"'
                .word code_c_quote
word_abort_quote:
                .word word_c_quote
                .byte FLAG_IMM | 6
                .byte 'A', 'B', 'O', 'R', 'T', '"'
                .word code_abort_quote
word_if:        .word word_abort_quote
                .byte FLAG_IMM | 2
                .byte "IF"
                .word code_if
word_else:      .word word_if
                .byte FLAG_IMM | 4
                .byte "ELSE"
                .word code_else
word_then:      .word word_else
                .byte FLAG_IMM | 4
                .byte "THEN"
                .word code_then
word_begin:     .word word_then
                .byte FLAG_IMM | 5
                .byte "BEGIN"
                .word code_begin
word_do:        .word word_begin
                .byte FLAG_IMM | 2
                .byte "DO"
                .word code_do
word_qdo:       .word word_do
                .byte FLAG_IMM | 3
                .byte "?DO"
                .word code_qdo
word_loop:      .word word_qdo
                .byte FLAG_IMM | 4
                .byte "LOOP"
                .word code_loop
word_plus_loop: .word word_loop
                .byte FLAG_IMM | 5
                .byte "+LOOP"
                .word code_plus_loop
word_leave:     .word word_plus_loop
                .byte FLAG_IMM | 5
                .byte "LEAVE"
                .word code_leave
word_until:     .word word_leave
                .byte FLAG_IMM | 5
                .byte "UNTIL"
                .word code_until
word_again:     .word word_until
                .byte FLAG_IMM | 5
                .byte "AGAIN"
                .word code_again
word_while:     .word word_again
                .byte FLAG_IMM | 5
                .byte "WHILE"
                .word code_while
word_repeat:    .word word_while
                .byte FLAG_IMM | 6
                .byte "REPEAT"
                .word code_repeat
word_words:     .word word_repeat
                .byte 5
                .byte "WORDS"
                .word code_words
word_emit:      .word word_words
                .byte 4
                .byte "EMIT"
                .word code_emit
word_spaces:    .word word_emit
                .byte 6
                .byte "SPACES"
                .word code_spaces
word_type:      .word word_spaces
                .byte 4
                .byte "TYPE"
                .word code_type
word_key:       .word word_type
                .byte 3
                .byte "KEY"
                .word code_key
word_bye:       .word word_key
                .byte 3
                .byte "BYE"
                .word code_bye
word_abort:     .word word_bye
                .byte 5
                .byte "ABORT"
                .word code_abort

ROM_LATEST = word_abort

; =====================================================================
; MONITOR / VECTORS
; =====================================================================
      .segment "MONITOR"
      .org  $FFD7

RES_vec:
      CLD
      LDX   #$FF
      TXS
      JMP   cold_start

NMI_stub:
      RTI

IRQ_stub:
      RTI

      .segment "VECTORS"
      .org  $FFFA

      .word NMI_stub
      .word RES_vec
      .word IRQ_stub

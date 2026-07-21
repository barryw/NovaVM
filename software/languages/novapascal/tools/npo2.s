; NovaPascal O2 optimizer: compact typed IR -> optimized textual 65C02.

      .setcpu "w65c02"
      .include "nova.inc"
      .include "libmemory.inc"
      .include "nptool.inc"
      .include "xram.inc"

ASM_CAP = $4000
ASM_LOAD_CAP = ASM_CAP + 1

.macro long_beq target
      BNE   :+
      JMP   target
:
.endmacro

.macro long_bne target
      BEQ   :+
      JMP   target
:
.endmacro

      .segment "ZEROPAGE"
text_ptr:        .res 2

      .segment "BSS"
buffer_a:        .res 3
buffer_b:        .res 3
buffer_a_owned:  .res 1
buffer_b_owned:  .res 1
input_base:      .res 3
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

      LDA   #<pass_lowering
      LDX   #>pass_lowering
      JSR   nptool_print_z
      LDX   #2
@pass1_addresses:
      LDA   buffer_a,X
      STA   input_base,X
      LDA   buffer_b,X
      STA   output_base,X
      DEX
      BPL   @pass1_addresses
      LDA   #1
      STA   pass_id
      JSR   run_pass
      BEQ   :+
      JMP   output_error
:

      LDA   #<pass_peephole
      LDX   #>pass_peephole
      JSR   nptool_print_z
      LDA   output_len
      STA   input_len
      LDA   output_len+1
      STA   input_len+1
      LDX   #2
@pass2_addresses:
      LDA   buffer_b,X
      STA   input_base,X
      LDA   buffer_a,X
      STA   output_base,X
      DEX
      BPL   @pass2_addresses
      LDA   #2
      STA   pass_id
      JSR   run_pass
      BNE   output_error

      JSR   save_result
      BNE   io_error
      JSR   release_buffers
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
      ORA   #XRAM_WIN3_ENABLE
      STA   XMC_WINCTL
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
      BNE   @pass2
      JSR   process_lowering
      BRA   @result
@pass2:
      JSR   process_peephole
@result:
      BEQ   @next
@fail:
      LDA   #1
      RTS

input_at_end:
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

read_input_byte:
      CLC
      LDA   input_base
      ADC   input_pos
      STA   byte_value
      LDA   input_base+1
      ADC   input_pos+1
      STA   WIN3_MI
      LDA   input_base+2
      ADC   #0
      STA   WIN3_HI
      LDX   byte_value
      LDA   WIN3_BASE,X
      RTS

increment_input:
      INC   input_pos
      BNE   :+
      INC   input_pos+1
:
      RTS

read_line:
      JSR   input_at_end
      BCC   :+
      LDA   #1
      RTS
:
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

emit_byte:
      STA   byte_value
      LDA   output_len+1
      CMP   output_cap+1
      BCC   @write
      BNE   @full
      LDA   output_len
      CMP   output_cap
      BCS   @full
@write:
      LDA   output_addr+1
      STA   WIN3_MI
      LDA   output_addr+2
      STA   WIN3_HI
      LDX   output_addr
      LDA   byte_value
      STA   WIN3_BASE,X
      INC   output_addr
      BNE   :+
      INC   output_addr+1
      BNE   :+
      INC   output_addr+2
:
      INC   output_len
      BNE   :+
      INC   output_len+1
:
      LDA   #0
      RTS
@full:
      LDA   #3
      STA   opt_error
      LDA   #1
      RTS

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
      BNE   @raw
      LDA   #5
      STA   operand_pos
      JMP   emit_array_word
@raw:
      JMP   emit_line
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
pass_lowering:    .byte "Pass 1: typed instruction selection", $0D, $0A, 0
pass_peephole:    .byte "Pass 2: constant peepholes", $0D, $0A, 0
optimizer_ok:     .byte "Optimization complete", $0D, $0A, 0

ir_compare:      .byte ".O2C ", 0
ir_branch:       .byte ".O2F ", 0
ir_add_word:     .byte ".O2A", 0
ir_sub_word:     .byte ".O2S", 0
ir_compare_word: .byte ".O2X", 0
ir_array_get:    .byte ".O2G ", 0
ir_array_byte:   .byte ".O2B ", 0
ir_array_word:   .byte ".O2W ", 0

lower_add_word: .byte "CLC", $0A, "ADC NVR0L", $0A, "STA NVR0L", $0A, "TXA", $0A, "ADC NVR0H", $0A, "TAX", $0A, "LDA NVR0L", $0A, 0
lower_sub_word: .byte "SEC", $0A, "SBC NVR0L", $0A, "STA NVR0L", $0A, "TXA", $0A, "SBC NVR0H", $0A, "TAX", $0A, "LDA NVR0L", $0A, 0
lower_compare_word: .byte "CPX NVR0H", $0A, "BNE :+", $0A, "CMP NVR0L", $0A, ":", $0A, 0

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

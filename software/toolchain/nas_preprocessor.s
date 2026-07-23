; NAS disk-loaded preprocessor: case-insensitive ca65-style parameterized
; macros plus nested .if/.ifdef/.ifndef/.else/.endif conditionals.

      .setcpu "w65c02"
      .include "xram.inc"
      .include "longbranch.inc"
      .include "wordmath.inc"
      .include "xramstream.inc"
      .include "nas_preprocessor.inc"
      .include "nas_core.inc"

LINE_CAP       = 255
MACRO_CAP      = 24
NAME_CAP       = 32
PARAM_CAP      = 4
ARG_CAP        = 48
COND_CAP       = 8
EXPR_DEPTH     = 8
INCLUDE_DEPTH  = 8
INCLUDE_NAME_CAP = 64

EXPR_BOOL_OR = 1
EXPR_BOOL_AND = 2
EXPR_EQ      = 3
EXPR_NE      = 4
EXPR_LT      = 5
EXPR_LE      = 6
EXPR_GT      = 7
EXPR_GE      = 8
EXPR_BIT_OR  = 9
EXPR_BIT_XOR = 10
EXPR_BIT_AND = 11
EXPR_SHL     = 12
EXPR_SHR     = 13
EXPR_ADD     = 14
EXPR_SUB     = 15
EXPR_MUL     = 16
EXPR_DIV     = 17
EXPR_MOD     = 18
EXPR_BOOL_XOR = 19

      .segment "ZEROPAGE"
string_ptr: .res 2
work_ptr:   .res 2

      .segment "BSS"
input_base:       .res 3
input_addr:       .res 3
input_pos:        .res 2
input_len:        .res 2
output_addr:      .res 3
output_len:       .res 2
output_cap:       .res 2
line_start:       .res 2
current_line:     .res 2
line_len:         .res 1
line_has_lf:      .res 1
line_index:       .res 1
line_buf:         .res 256
parse_pos:        .res 1
token_start:      .res 1
token_len:        .res 1
compare_count:    .res 1
byte_value:       .res 1
current_active:   .res 1
condition_value:  .res 1
expr_len:         .res 1
expr_pos:         .res 1
expr_depth:       .res 1
expr_op_len:      .res 1
expr_value_l:     .res 1
expr_value_h:     .res 1
expr_rhs_l:       .res 1
expr_rhs_h:       .res 1
expr_acc_l:       .res 1
expr_acc_h:       .res 1
expr_rem_l:       .res 1
expr_rem_h:       .res 1
expr_digit:       .res 1
expr_digits:      .res 1
expr_min:         .res EXPR_DEPTH
expr_prec:        .res EXPR_DEPTH
expr_op:          .res EXPR_DEPTH
expr_lhs_l:       .res EXPR_DEPTH
expr_lhs_h:       .res EXPR_DEPTH
expr_buf:         .res 256
cond_depth:       .res 1
cond_parent:      .res COND_CAP
cond_taken:       .res COND_CAP
cond_else:        .res COND_CAP
macro_count:      .res 1
macro_slot:       .res 1
macro_kind:       .res MACRO_CAP ; 0=macro, 1=.define, 2=undefined tombstone
macro_names:      .res MACRO_CAP * NAME_CAP
macro_name_lens:  .res MACRO_CAP
macro_param_count:.res MACRO_CAP
macro_param_names:.res MACRO_CAP * PARAM_CAP * NAME_CAP
macro_param_lens: .res MACRO_CAP * PARAM_CAP
macro_body_start_l:.res MACRO_CAP
macro_body_start_h:.res MACRO_CAP
macro_body_end_l: .res MACRO_CAP
macro_body_end_h: .res MACRO_CAP
macro_body_base_l:.res MACRO_CAP
macro_body_base_m:.res MACRO_CAP
macro_body_base_h:.res MACRO_CAP
macro_source_len_l:.res MACRO_CAP
macro_source_len_h:.res MACRO_CAP
param_index:      .res 1
flat_slot:        .res 1
arg_count:        .res 1
arg_start:        .res 1
arg_end:          .res 1
arg_lens:         .res PARAM_CAP
arg_text:         .res PARAM_CAP * ARG_CAP
saved_input_pos:  .res 2
saved_line:       .res 2
saved_input_base: .res 3
saved_input_len:  .res 2
saved_define_base:.res 3
expansion_end:    .res 2
quote_char:       .res 1
params_active:    .res 1
define_slot:      .res 1
saved_macro_slot: .res 1
command_value_len:.res 1
command_value:    .res ARG_CAP
include_depth:    .res 1
include_need_lf:  .res 1
include_close_error:.res 1
include_stack_base_l:.res INCLUDE_DEPTH
include_stack_base_m:.res INCLUDE_DEPTH
include_stack_base_h:.res INCLUDE_DEPTH
include_stack_pos_l:.res INCLUDE_DEPTH
include_stack_pos_h:.res INCLUDE_DEPTH
include_stack_len_l:.res INCLUDE_DEPTH
include_stack_len_h:.res INCLUDE_DEPTH
include_stack_line_l:.res INCLUDE_DEPTH
include_stack_line_h:.res INCLUDE_DEPTH
include_stack_cond:.res INCLUDE_DEPTH
include_stack_macro:.res INCLUDE_DEPTH
include_stack_output_l:.res INCLUDE_DEPTH
include_stack_output_h:.res INCLUDE_DEPTH
include_stack_lf: .res INCLUDE_DEPTH

      .segment "CODE"
      .export naspp_main

naspp_main:
      STZ   NASPP_ERROR
      STZ   macro_count
      STZ   cond_depth
      STZ   include_depth
      STZ   output_len
      STZ   output_len+1
      STZ   current_line
      STZ   current_line+1
      STZ   NASPP_ERROR_LINE
      STZ   NASPP_ERROR_LINE+1
      STZ   NASPP_ERROR_COL
      STZ   NASPP_ERROR_COL+1
      STZ   input_pos
      STZ   input_pos+1
      JSR   seed_command_define
      BEQ   :+
      JMP   @fail
:
      LDA   #1
      STA   current_active
      LDX   #2
@copy_addresses:
      LDA   NASPP_INPUT_XADDR,X
      STA   input_base,X
      LDA   NASPP_OUTPUT_XADDR,X
      STA   output_addr,X
      DEX
      BPL   @copy_addresses
      LDA   NASPP_INPUT_LEN
      STA   input_len
      LDA   NASPP_INPUT_LEN+1
      STA   input_len+1
      LDA   NASPP_OUTPUT_CAP
      STA   output_cap
      LDA   NASPP_OUTPUT_CAP+1
      STA   output_cap+1
      LDA   XMC_WINCTL
      ORA   #XRAM_WIN2_ENABLE | XRAM_WIN3_ENABLE
      STA   XMC_WINCTL
      JSR   map_output

@next_line:
      JSR   read_line
      BEQ   @have_line
      LDA   NASPP_ERROR
      BEQ   :+
      JMP   @fail
:
      LDA   include_depth
      BEQ   :+
      JSR   leave_include_text
      BEQ   @next_line
      JMP   @fail
:
      LDA   cond_depth
      BEQ   :+
      JMP   @nesting_error
:
      LDA   output_len
      STA   NASPP_INPUT_LEN
      LDA   output_len+1
      STA   NASPP_INPUT_LEN+1
      LDA   #0
      RTS

@have_line:
      JSR   prepare_token
      LDA   token_len
      long_beq @ordinary

      LDA   #<kw_if
      LDX   #>kw_if
      JSR   token_equals
      BNE   :+
      JMP   @handle_if
:
      LDA   #<kw_ifdef
      LDX   #>kw_ifdef
      JSR   token_equals
      BNE   :+
      JMP   @handle_ifdef
:
      LDA   #<kw_ifndef
      LDX   #>kw_ifndef
      JSR   token_equals
      BNE   :+
      JMP   @handle_ifndef
:
      LDA   #<kw_elseif
      LDX   #>kw_elseif
      JSR   token_equals
      BNE   :+
      JMP   @handle_elseif
:
      LDA   #<kw_else
      LDX   #>kw_else
      JSR   token_equals
      BNE   :+
      JMP   @handle_else
:
      LDA   #<kw_endif
      LDX   #>kw_endif
      JSR   token_equals
      BNE   :+
      JMP   @handle_endif
:
      LDA   #<kw_macro
      LDX   #>kw_macro
      JSR   token_equals
      BNE   :+
      JMP   @handle_macro
:
      LDA   #<kw_define
      LDX   #>kw_define
      JSR   token_equals
      BNE   :+
      JMP   @handle_define
:
      LDA   #<kw_undefine
      LDX   #>kw_undefine
      JSR   token_equals
      BNE   :+
      JMP   @handle_undefine
:
      LDA   #<kw_endmacro
      LDX   #>kw_endmacro
      JSR   token_equals
      BNE   :+
      JMP   @syntax_error
:
      LDA   #<kw_includetext
      LDX   #>kw_includetext
      JSR   token_equals
      BNE   :+
      JMP   @handle_includetext
:

@ordinary:
      LDA   current_active
      BEQ   @emit_skipped
      LDA   token_len
      BEQ   @emit_raw
      JSR   find_macro
      BCC   @emit_raw
      LDX   macro_slot
      LDA   macro_kind,X
      BNE   @emit_raw
      JSR   parse_arguments
      BEQ   :+
      JMP   @syntax_error
:
      JSR   expand_macro
      BEQ   :+
      JMP   @fail
:
      JMP   @next_line

@emit_raw:
      STZ   params_active
      JSR   expand_body_line
      BEQ   :+
      JMP   @fail
:
      JMP   @next_line
@emit_skipped:
      JSR   emit_skipped_line
      BEQ   :+
      JMP   @fail
:
      JMP   @next_line

@handle_includetext:
      LDA   current_active
      BEQ   @emit_skipped
      JSR   parse_include_text_name
      long_bne @syntax_error
      JSR   enter_include_text
      long_beq @next_line
      LDA   #NASPP_ERR_INCLUDE
      STA   NASPP_ERROR
      JMP   @fail

@handle_if:
      JSR   parse_condition_expression
      long_bne @syntax_error
      JSR   push_condition
      long_bne @nesting_error
      BRA   @finish_directive

@handle_ifdef:
      JSR   parse_defined_condition
      BNE   @syntax_error
      JSR   push_condition
      BNE   @nesting_error
      BRA   @finish_directive

@handle_ifndef:
      JSR   parse_defined_condition
      BNE   @syntax_error
      LDA   condition_value
      EOR   #1
      STA   condition_value
      JSR   push_condition
      BNE   @nesting_error
      BRA   @finish_directive

@handle_elseif:
      JSR   parse_condition_expression
      BNE   @syntax_error
      JSR   switch_elseif
      BNE   @nesting_error
      BRA   @finish_directive

@handle_else:
      JSR   ensure_line_end
      BNE   @syntax_error
      JSR   switch_else
      BNE   @nesting_error
      BRA   @finish_directive

@handle_endif:
      JSR   ensure_line_end
      BNE   @syntax_error
      JSR   pop_condition
      BNE   @nesting_error
@finish_directive:
      JSR   emit_skipped_line
      BNE   @fail
      JMP   @next_line

@handle_macro:
      LDA   current_active
      BEQ   @skip_macro
      JSR   define_macro
      BNE   @fail
      JMP   @next_line

@handle_define:
      LDA   current_active
      long_beq @emit_skipped
      JSR   define_constant
      BNE   @fail
      JMP   @next_line

@handle_undefine:
      LDA   current_active
      long_beq @emit_skipped
      JSR   undefine_constant
      BNE   @fail
      JMP   @next_line
@skip_macro:
      JSR   skip_macro_definition
      BNE   @fail
      JMP   @next_line

@syntax_error:
      LDA   #NASPP_ERR_SYNTAX
      BRA   @set_error
@nesting_error:
      LDA   #NASPP_ERR_NESTING
@set_error:
      STA   NASPP_ERROR
@fail:
      LDA   current_line
      STA   NASPP_ERROR_LINE
      LDA   current_line+1
      STA   NASPP_ERROR_LINE+1
      LDA   #1
      STA   NASPP_ERROR_COL
      LDA   #1
      RTS

; ---------------------------------------------------------------------
; XRAM line input and output
; ---------------------------------------------------------------------

read_line:
      JSR   input_at_end
      BCC   @start
      LDA   #1
      RTS
@start:
      JSR   map_input
      INC   current_line
      BNE   :+
      INC   current_line+1
:
      LDA   input_pos
      STA   line_start
      LDA   input_pos+1
      STA   line_start+1
      STZ   line_len
      STZ   line_has_lf
      LDX   #0
@next:
      JSR   input_at_end
      BCS   @done
      PHX
      JSR   read_input_byte
      STA   byte_value
      PLX
      JSR   increment_input
      LDA   byte_value
      CMP   #$0A
      BEQ   @lf
      STA   line_buf,X
      INX
      BNE   @next
      LDA   #NASPP_ERR_LINE
      STA   NASPP_ERROR
      LDA   #1
      RTS
@lf:
      INC   line_has_lf
@done:
      STX   line_len
      LDA   #0
      RTS

      xram_stream_impl NASPP_ERROR, NASPP_ERR_OUTPUT

emit_line_raw:
      STZ   line_index
@loop:
      LDX   line_index
      CPX   line_len
      BCS   emit_line_lf
      LDA   line_buf,X
      JSR   emit_byte
      BNE   @done
      INC   line_index
      BRA   @loop
@done:
      RTS

emit_line_lf:
      LDA   line_has_lf
      BEQ   @ok
      LDA   #$0A
      JMP   emit_byte
@ok:
      LDA   #0
      RTS

emit_skipped_line:
      JMP   emit_line_lf

; ---------------------------------------------------------------------
; Tokens and case-insensitive comparisons
; ---------------------------------------------------------------------

prepare_token:
      STZ   parse_pos
      JSR   scan_next_token
      RTS

scan_next_token:
      JSR   skip_spaces
      LDX   parse_pos
      STX   token_start
      STZ   token_len
@scan:
      CPX   line_len
      BCS   @done
      LDA   line_buf,X
      CMP   #';'
      BEQ   @done
      CMP   #','
      BEQ   @done
      CMP   #' '
      BEQ   @done
      CMP   #$09
      BEQ   @done
      INX
      INC   token_len
      BRA   @scan
@done:
      STX   parse_pos
      RTS

skip_spaces:
      LDX   parse_pos
@loop:
      CPX   line_len
      BCS   @done
      LDA   line_buf,X
      CMP   #' '
      BEQ   @skip
      CMP   #$09
      BNE   @done
@skip:
      INX
      BRA   @loop
@done:
      STX   parse_pos
      RTS

ensure_line_end:
      JSR   skip_spaces
      LDX   parse_pos
      CPX   line_len
      BCS   @ok
      LDA   line_buf,X
      CMP   #';'
      BEQ   @ok
      LDA   #1
      RTS
@ok:
      LDA   #0
      RTS

; .INCLUDETEXT performs an early, recursive text splice. Unlike ordinary
; .INCLUDE, it is resolved by the preprocessor so downstream source-to-source
; tools can see the combined stream before NAS assembles it.
parse_include_text_name:
      JSR   skip_spaces
      LDX   parse_pos
      CPX   line_len
      BCS   @bad
      LDA   line_buf,X
      CMP   #'"'
      BNE   @bad
      INX
      STZ   nasm_include_name_len
@char:
      CPX   line_len
      BCS   @bad
      LDA   line_buf,X
      CMP   #'"'
      BEQ   @close
      CMP   #$20
      BCC   @bad
      LDY   nasm_include_name_len
      CPY   #INCLUDE_NAME_CAP-1
      BCS   @bad
      STA   nasm_include_name,Y
      INC   nasm_include_name_len
      INX
      BRA   @char
@close:
      LDA   nasm_include_name_len
      BEQ   @bad
      TAY
      LDA   #0
      STA   nasm_include_name,Y
      INX
      STX   parse_pos
      JMP   ensure_line_end
@bad:
      LDA   #1
      RTS

preprocessor_include_open:
      LDA   #NASCORE_INCLUDE_BUFFERED
      STA   NASCORE_INCLUDE_MODE
      JSR   @call
      PHA
      STZ   NASCORE_INCLUDE_MODE
      PLA
      RTS
@call:
      JMP   (NASCORE_INCLUDE_OPEN)

preprocessor_include_close:
      JMP   (NASCORE_INCLUDE_CLOSE)

enter_include_text:
      LDX   include_depth
      CPX   #INCLUDE_DEPTH
      long_bcs @bad
      JSR   preprocessor_include_open
      long_bne @bad
      JSR   map_output
      LDX   include_depth
      LDA   input_base
      STA   include_stack_base_l,X
      LDA   input_base+1
      STA   include_stack_base_m,X
      LDA   input_base+2
      STA   include_stack_base_h,X
      LDA   input_pos
      STA   include_stack_pos_l,X
      LDA   input_pos+1
      STA   include_stack_pos_h,X
      LDA   input_len
      STA   include_stack_len_l,X
      LDA   input_len+1
      STA   include_stack_len_h,X
      LDA   current_line
      STA   include_stack_line_l,X
      LDA   current_line+1
      STA   include_stack_line_h,X
      LDA   cond_depth
      STA   include_stack_cond,X
      LDA   macro_count
      STA   include_stack_macro,X
      LDA   output_len
      STA   include_stack_output_l,X
      LDA   output_len+1
      STA   include_stack_output_h,X
      LDA   line_has_lf
      STA   include_stack_lf,X
      INC   include_depth
      LDA   nasm_include_ptr
      STA   input_base
      LDA   nasm_include_ptr+1
      STA   input_base+1
      LDA   nasm_include_ptr+2
      STA   input_base+2
      STZ   input_pos
      STZ   input_pos+1
      LDA   nasm_include_len
      STA   input_len
      LDA   nasm_include_len+1
      STA   input_len+1
      STZ   current_line
      STZ   current_line+1
      LDA   #0
      RTS
@bad:
      LDA   #1
      RTS

leave_include_text:
      LDX   include_depth
      long_beq @bad
      DEX
      STZ   include_need_lf
      LDA   output_len+1
      CMP   include_stack_output_h,X
      BNE   @has_output
      LDA   output_len
      CMP   include_stack_output_l,X
      BEQ   @outer_lf
@has_output:
      LDA   line_has_lf
      BNE   @check_nesting
@outer_lf:
      LDA   include_stack_lf,X
      BEQ   @check_nesting
      INC   include_need_lf
@check_nesting:
      STZ   include_close_error
      LDA   cond_depth
      CMP   include_stack_cond,X
      BEQ   :+
      INC   include_close_error
:
      JSR   preprocessor_include_close
      BEQ   :+
      LDA   #2
      STA   include_close_error
:
      JSR   map_output
      DEC   include_depth
      LDX   include_depth
      LDA   include_stack_base_l,X
      STA   input_base
      LDA   include_stack_base_m,X
      STA   input_base+1
      LDA   include_stack_base_h,X
      STA   input_base+2
      LDA   include_stack_pos_l,X
      STA   input_pos
      LDA   include_stack_pos_h,X
      STA   input_pos+1
      LDA   include_stack_len_l,X
      STA   input_len
      LDA   include_stack_len_h,X
      STA   input_len+1
      LDA   include_stack_line_l,X
      STA   current_line
      LDA   include_stack_line_h,X
      STA   current_line+1
      LDA   include_stack_macro,X
      STA   macro_count
      LDA   include_close_error
      BEQ   @separator
      CMP   #1
      BNE   @include_bad
      LDA   #NASPP_ERR_NESTING
      STA   NASPP_ERROR
      LDA   #1
      RTS
@include_bad:
      LDA   #NASPP_ERR_INCLUDE
      STA   NASPP_ERROR
@bad:
      LDA   #1
      RTS
@separator:
      LDA   include_need_lf
      BEQ   @ok
      LDA   #$0A
      JMP   emit_byte
@ok:
      LDA   #0
      RTS

; A/X -> length-prefixed lowercase keyword.
token_equals:
      STA   string_ptr
      STX   string_ptr+1
      LDY   #0
      LDA   (string_ptr),Y
      CMP   token_len
      BEQ   :+
      LDA   #1
      RTS
:
      STZ   compare_count
      INY
      LDX   token_start
@loop:
      LDA   compare_count
      CMP   token_len
      BCS   @equal
      LDA   line_buf,X
      JSR   uppercase
      STA   byte_value
      LDA   (string_ptr),Y
      JSR   uppercase
      CMP   byte_value
      BNE   @different
      INX
      INY
      INC   compare_count
      BRA   @loop
@equal:
      LDA   #0
      RTS
@different:
      LDA   #1
      RTS

uppercase:
      CMP   #'a'
      BCC   @done
      CMP   #'z'+1
      BCS   @done
      AND   #$DF
@done:
      RTS

is_ident_start:
      CMP   #'_'
      BEQ   @yes
      CMP   #'@'
      BEQ   @yes
      JSR   uppercase
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

is_ident_char:
      PHA
      JSR   is_ident_start
      BCS   @yes_pop
      PLA
      CMP   #'0'
      BCC   @no
      CMP   #'9'+1
      BCS   @no
      SEC
      RTS
@yes_pop:
      PLA
      SEC
      RTS
@no:
      CLC
      RTS

; ---------------------------------------------------------------------
; Conditional assembly
; ---------------------------------------------------------------------

; Expand one level of `.DEFINE` values, then evaluate a 16-bit constant
; expression. Undefined identifiers and malformed expressions fail loudly.
parse_condition_expression:
      JSR   prepare_condition_expression
      BNE   @error
      STZ   expr_depth
      LDA   #1
      JSR   expr_parse
      BCS   @error
      JSR   expr_skip_spaces
      LDA   expr_pos
      CMP   expr_len
      BNE   @error
      LDA   expr_value_l
      ORA   expr_value_h
      BEQ   @false
      LDA   #1
@false:
      STA   condition_value
      LDA   #0
      RTS
@error:
      LDA   #1
      RTS

prepare_condition_expression:
      JSR   skip_spaces
      STZ   expr_len
@next:
      LDX   parse_pos
      CPX   line_len
      long_bcs @done
      LDA   line_buf,X
      CMP   #';'
      long_beq @done
      CMP   #39
      BEQ   @quoted
      CMP   #'"'
      BEQ   @quoted
      JSR   is_ident_start
      BCS   @identifier
      LDX   parse_pos
      LDA   line_buf,X
      JSR   expr_append
      long_bne @error
      INC   parse_pos
      BRA   @next
@quoted:
      STA   quote_char
      JSR   expr_append
      BNE   @error
      INC   parse_pos
@quote_byte:
      LDX   parse_pos
      CPX   line_len
      BCS   @done
      LDA   line_buf,X
      STA   byte_value
      JSR   expr_append
      BNE   @error
      INC   parse_pos
      LDA   byte_value
      CMP   quote_char
      BNE   @quote_byte
      BRA   @next
@identifier:
      LDX   parse_pos
      STX   token_start
      STZ   token_len
@ident_byte:
      CPX   line_len
      BCS   @ident_done
      LDA   line_buf,X
      JSR   is_ident_char
      BCC   @ident_done
      INX
      INC   token_len
      BRA   @ident_byte
@ident_done:
      STX   parse_pos
      JSR   find_define
      BCC   @copy_identifier
      JSR   expr_copy_define
      BNE   @error
      JMP   @next
@copy_identifier:
      LDA   token_start
      STA   compare_count
@copy_ident_byte:
      LDA   compare_count
      CMP   parse_pos
      long_bcs @next
      TAX
      LDA   line_buf,X
      JSR   expr_append
      BNE   @error
      INC   compare_count
      BRA   @copy_ident_byte
@done:
      LDA   expr_len
      BEQ   @error
      STZ   expr_pos
      LDA   #0
      RTS
@error:
      LDA   #1
      RTS

expr_append:
      LDX   expr_len
      CPX   #$FF
      BCS   @full
      STA   expr_buf,X
      INC   expr_len
      LDA   #0
      RTS
@full:
      LDA   #1
      RTS

expr_copy_define:
      LDA   #'('
      JSR   expr_append
      long_bne @error
      LDX   define_slot
      LDA   macro_kind,X
      CMP   #3
      BEQ   @command
      LDA   input_pos
      PHA
      LDA   input_pos+1
      PHA
      JSR   use_define_source
      LDX   define_slot
      LDA   macro_body_start_l,X
      STA   input_pos
      LDA   macro_body_start_h,X
      STA   input_pos+1
      JSR   map_input
@source_byte:
      LDX   define_slot
      LDA   input_pos+1
      CMP   macro_body_end_h,X
      BCC   @source_emit
      BNE   @source_done
      LDA   input_pos
      CMP   macro_body_end_l,X
      BCS   @source_done
@source_emit:
      JSR   read_input_byte
      STA   byte_value
      JSR   increment_input
      LDA   byte_value
      JSR   expr_append
      BNE   @source_error
      BRA   @source_byte
@source_done:
      JSR   restore_define_source
      PLA
      STA   input_pos+1
      PLA
      STA   input_pos
      BRA   @close
@source_error:
      JSR   restore_define_source
      PLA
      STA   input_pos+1
      PLA
      STA   input_pos
      BRA   @error
@command:
      LDY   #0
@command_byte:
      CPY   command_value_len
      BCS   @close
      LDA   command_value,Y
      JSR   expr_append
      BNE   @error
      INY
      BRA   @command_byte
@close:
      LDA   #')'
      JMP   expr_append
@error:
      LDA   #1
      RTS

use_define_source:
      LDX   #2
@save:
      LDA   input_base,X
      STA   saved_define_base,X
      DEX
      BPL   @save
      LDX   define_slot
      LDA   macro_body_base_l,X
      STA   input_base
      LDA   macro_body_base_m,X
      STA   input_base+1
      LDA   macro_body_base_h,X
      STA   input_base+2
      RTS

restore_define_source:
      LDX   #2
@restore:
      LDA   saved_define_base,X
      STA   input_base,X
      DEX
      BPL   @restore
      RTS

; Precedence-climbing parser. Larger precedence values bind more tightly.
expr_parse:
      LDX   expr_depth
      CPX   #EXPR_DEPTH
      BCC   :+
      JMP   @full
:
      STA   expr_min,X
      INC   expr_depth
      JSR   expr_unary
      BCS   @bad
@operator:
      JSR   expr_peek_operator
      BCC   @done
      LDY   expr_depth
      DEY
      STA   expr_op,Y
      TXA
      CMP   expr_min,Y
      BCC   @done
      STA   expr_prec,Y
      LDA   expr_value_l
      STA   expr_lhs_l,Y
      LDA   expr_value_h
      STA   expr_lhs_h,Y
      JSR   expr_consume_operator
      LDY   expr_depth
      DEY
      LDA   expr_prec,Y
      INC
      JSR   expr_parse
      BCS   @bad
      LDA   expr_value_l
      STA   expr_rhs_l
      LDA   expr_value_h
      STA   expr_rhs_h
      LDY   expr_depth
      DEY
      LDA   expr_lhs_l,Y
      STA   expr_value_l
      LDA   expr_lhs_h,Y
      STA   expr_value_h
      LDA   expr_op,Y
      JSR   expr_apply
      BCS   @bad
      BRA   @operator
@done:
      DEC   expr_depth
      CLC
      RTS
@bad:
      DEC   expr_depth
@full:
      SEC
      RTS

; Return operator id in A and precedence in X, with byte length in expr_op_len.
expr_peek_operator:
      JSR   expr_skip_spaces
      JSR   expr_peek
      BCC   @no
      STZ   expr_op_len
      INC   expr_op_len
      CMP   #'|'
      long_beq @or
      CMP   #'&'
      long_beq @and
      CMP   #'^'
      long_beq @bit_xor
      CMP   #'='
      long_beq @eq
      CMP   #'<'
      long_beq @less
      CMP   #'>'
      long_beq @greater
      CMP   #'+'
      long_beq @add
      CMP   #'-'
      long_beq @sub
      CMP   #'*'
      long_beq @mul
      CMP   #'/'
      long_beq @div
      CMP   #'%'
      long_beq @mod
      CMP   #'.'
      long_beq @word
@no:
      CLC
      RTS
@word:
      LDA   #<op_or
      LDX   #>op_or
      JSR   expr_word_equals
      BEQ   @bool_or
      LDA   #<op_and
      LDX   #>op_and
      JSR   expr_word_equals
      BEQ   @bool_and
      LDA   #<op_xor
      LDX   #>op_xor
      JSR   expr_word_equals
      BEQ   @bool_xor
      LDA   #<op_bitor
      LDX   #>op_bitor
      JSR   expr_word_equals
      BEQ   @bit_or
      LDA   #<op_bitxor
      LDX   #>op_bitxor
      JSR   expr_word_equals
      BEQ   @bit_xor
      LDA   #<op_bitand
      LDX   #>op_bitand
      JSR   expr_word_equals
      BEQ   @bit_and
      LDA   #<op_shl
      LDX   #>op_shl
      JSR   expr_word_equals
      long_beq @shl
      LDA   #<op_shr
      LDX   #>op_shr
      JSR   expr_word_equals
      long_beq @shr
      LDA   #<op_mod
      LDX   #>op_mod
      JSR   expr_word_equals
      long_beq @mod
      BRA   @no
@bool_or:
      LDA   #EXPR_BOOL_OR
      LDX   #1
      JMP   @yes
@bool_and:
      LDA   #EXPR_BOOL_AND
      LDX   #2
      JMP   @yes
@bool_xor:
      LDA   #EXPR_BOOL_XOR
      LDX   #2
      JMP   @yes
@or:
      JSR   expr_peek_second
      BCC   @bit_or
      CMP   #'|'
      BNE   @bit_or
      INC   expr_op_len
      LDA   #EXPR_BOOL_OR
      LDX   #1
      JMP   @yes
@bit_or:
      LDA   #EXPR_BIT_OR
      LDX   #4
      JMP   @yes
@and:
      JSR   expr_peek_second
      BCC   @bit_and
      CMP   #'&'
      BNE   @bit_and
      INC   expr_op_len
      LDA   #EXPR_BOOL_AND
      LDX   #2
      JMP   @yes
@bit_and:
      LDA   #EXPR_BIT_AND
      LDX   #6
      JMP   @yes
@bit_xor:
      LDA   #EXPR_BIT_XOR
      LDX   #5
      JMP   @yes
@eq:
      LDA   #EXPR_EQ
      LDX   #3
      JMP   @yes
@less:
      JSR   expr_peek_second
      BCC   @lt
      CMP   #'='
      BEQ   @le
      CMP   #'>'
      BEQ   @ne
      CMP   #'<'
      BEQ   @shl
@lt:  LDA   #EXPR_LT
      LDX   #3
      JMP   @yes
@le:  INC   expr_op_len
      LDA   #EXPR_LE
      LDX   #3
      JMP   @yes
@ne:  INC   expr_op_len
      LDA   #EXPR_NE
      LDX   #3
      JMP   @yes
@shl: INC   expr_op_len
      LDA   #EXPR_SHL
      LDX   #7
      JMP   @yes
@greater:
      JSR   expr_peek_second
      BCC   @gt
      CMP   #'='
      BEQ   @ge
      CMP   #'>'
      BEQ   @shr
@gt:  LDA   #EXPR_GT
      LDX   #3
      JMP   @yes
@ge:  INC   expr_op_len
      LDA   #EXPR_GE
      LDX   #3
      JMP   @yes
@shr: INC   expr_op_len
      LDA   #EXPR_SHR
      LDX   #7
      JMP   @yes
@add: LDA   #EXPR_ADD
      LDX   #8
      JMP   @yes
@sub: LDA   #EXPR_SUB
      LDX   #8
      JMP   @yes
@mul: LDA   #EXPR_MUL
      LDX   #9
      JMP   @yes
@div: LDA   #EXPR_DIV
      LDX   #9
      JMP   @yes
@mod: LDA   #EXPR_MOD
      LDX   #9
@yes:
      SEC
      RTS

expr_peek_second:
      LDX   expr_pos
      INX
      CPX   expr_len
      BCS   @none
      LDA   expr_buf,X
      SEC
      RTS
@none:
      CLC
      RTS

; Compare the expression at expr_pos with a length-prefixed operator name.
expr_word_equals:
      STA   string_ptr
      STX   string_ptr+1
      LDY   #0
      LDA   (string_ptr),Y
      STA   expr_op_len
      CLC
      ADC   expr_pos
      BCS   @no
      CMP   expr_len
      BCC   @compare
      BNE   @no
@compare:
      LDX   expr_pos
      LDY   #1
@byte:
      CPY   expr_op_len
      BEQ   @last
      BCS   @boundary
      LDA   expr_buf,X
      JSR   uppercase
      CMP   (string_ptr),Y
      BNE   @no
      INX
      INY
      BRA   @byte
@last:
      LDA   expr_buf,X
      JSR   uppercase
      CMP   (string_ptr),Y
      BNE   @no
      INX
@boundary:
      CPX   expr_len
      BCS   @yes
      LDA   expr_buf,X
      JSR   is_ident_char
      BCS   @no
@yes:
      LDA   #0
      RTS
@no:
      LDA   #1
      RTS

expr_consume_operator:
      CLC
      LDA   expr_pos
      ADC   expr_op_len
      STA   expr_pos
      RTS

expr_unary:
      JSR   expr_skip_spaces
      JSR   expr_peek
      long_bcc @bad
      CMP   #'+'
      BEQ   @plus
      CMP   #'-'
      BEQ   @minus
      CMP   #'~'
      BEQ   @bit_not
      CMP   #'!'
      long_beq @bool_not
      CMP   #'<'
      long_beq @low
      CMP   #'>'
      long_beq @high
      CMP   #'.'
      BEQ   @word
      JMP   expr_primary
@word:
      LDA   #<op_not
      LDX   #>op_not
      JSR   expr_word_equals
      BEQ   @word_bool_not
      LDA   #<op_bitnot
      LDX   #>op_bitnot
      JSR   expr_word_equals
      BEQ   @word_bit_not
      LDA   #<op_lobyte
      LDX   #>op_lobyte
      JSR   expr_word_equals
      BEQ   @word_low
      LDA   #<op_hibyte
      LDX   #>op_hibyte
      JSR   expr_word_equals
      BEQ   @word_high
      JMP   @bad
@word_bool_not:
      JSR   expr_consume_operator
      BRA   @bool_not_apply
@word_bit_not:
      JSR   expr_consume_operator
      BRA   @bit_not_apply
@word_low:
      JSR   expr_consume_operator
      BRA   @low_apply
@word_high:
      JSR   expr_consume_operator
      BRA   @high_apply
@plus:
      JSR   expr_next
      JMP   expr_unary
@minus:
      JSR   expr_next
      JSR   expr_unary
      BCS   @bad
      SEC
      LDA   #0
      SBC   expr_value_l
      STA   expr_value_l
      LDA   #0
      SBC   expr_value_h
      STA   expr_value_h
      CLC
      RTS
@bit_not:
      JSR   expr_next
@bit_not_apply:
      JSR   expr_unary
      BCS   @bad
      LDA   expr_value_l
      EOR   #$FF
      STA   expr_value_l
      LDA   expr_value_h
      EOR   #$FF
      STA   expr_value_h
      CLC
      RTS
@bool_not:
      JSR   expr_next
@bool_not_apply:
      JSR   expr_unary
      BCS   @bad
      LDA   expr_value_l
      ORA   expr_value_h
      BEQ   @true
      LDA   #0
      BRA   @set_bool
@true:
      LDA   #1
@set_bool:
      STA   expr_value_l
      STZ   expr_value_h
      CLC
      RTS
@low:
      JSR   expr_next
@low_apply:
      JSR   expr_unary
      BCS   @bad
      STZ   expr_value_h
      CLC
      RTS
@high:
      JSR   expr_next
@high_apply:
      JSR   expr_unary
      BCS   @bad
      LDA   expr_value_h
      STA   expr_value_l
      STZ   expr_value_h
      CLC
      RTS
@bad:
      SEC
      RTS

expr_primary:
      JSR   expr_skip_spaces
      JSR   expr_peek_upper
      long_bcc @bad
      CMP   #'$'
      long_beq expr_hex
      CMP   #'%'
      long_beq expr_binary
      CMP   #39
      long_beq expr_character
      CMP   #'('
      BEQ   @group
      CMP   #'0'
      BCC   @bad
      CMP   #'9'+1
      long_bcc expr_decimal
      BRA   @bad
@group:
      JSR   expr_next
      LDA   #1
      JSR   expr_parse
      BCS   @bad
      JSR   expr_skip_spaces
      JSR   expr_next
      BCC   @bad
      CMP   #')'
      BNE   @bad
      CLC
      RTS
@bad:
      SEC
      RTS

expr_hex:
      JSR   expr_next
      STZ   expr_value_l
      STZ   expr_value_h
      STZ   expr_digits
@digit:
      JSR   expr_peek_upper
      BCC   @done
      JSR   expr_decode_hex
      BCS   @done
      STA   expr_digit
      LDA   expr_digits
      CMP   #4
      BCS   @bad
      ASL   expr_value_l
      ROL   expr_value_h
      ASL   expr_value_l
      ROL   expr_value_h
      ASL   expr_value_l
      ROL   expr_value_h
      ASL   expr_value_l
      ROL   expr_value_h
      LDA   expr_value_l
      ORA   expr_digit
      STA   expr_value_l
      JSR   expr_next
      INC   expr_digits
      BRA   @digit
@done:
      LDA   expr_digits
      BEQ   @bad
      CLC
      RTS
@bad:
      SEC
      RTS

expr_binary:
      JSR   expr_next
      STZ   expr_value_l
      STZ   expr_value_h
      STZ   expr_digits
@digit:
      JSR   expr_peek
      BCC   @done
      CMP   #'0'
      BEQ   @bit
      CMP   #'1'
      BNE   @done
@bit:
      STA   expr_digit
      LDA   expr_digits
      CMP   #16
      BCS   @bad
      LDA   expr_digit
      CMP   #'1'
      PHP
      ASL   expr_value_l
      ROL   expr_value_h
      PLP
      BNE   :+
      INC   expr_value_l
:
      JSR   expr_next
      INC   expr_digits
      BRA   @digit
@done:
      LDA   expr_digits
      BEQ   @bad
      CLC
      RTS
@bad:
      SEC
      RTS

expr_decimal:
      STZ   expr_value_l
      STZ   expr_value_h
      STZ   expr_digits
@digit:
      JSR   expr_peek
      BCC   @done
      CMP   #'0'
      BCC   @done
      CMP   #'9'+1
      BCS   @done
      SEC
      SBC   #'0'
      STA   expr_digit
      JSR   expr_mul10_add
      BCS   @bad
      JSR   expr_next
      INC   expr_digits
      BRA   @digit
@done:
      LDA   expr_digits
      BEQ   @bad
      CLC
      RTS
@bad:
      SEC
      RTS

expr_mul10_add:
      LDA   expr_value_l
      STA   expr_acc_l
      LDA   expr_value_h
      STA   expr_acc_h
      ASL   expr_acc_l
      ROL   expr_acc_h
      BCS   @bad
      LDX   #3
@times8:
      ASL   expr_value_l
      ROL   expr_value_h
      BCS   @bad
      DEX
      BNE   @times8
      CLC
      LDA   expr_value_l
      ADC   expr_acc_l
      STA   expr_value_l
      LDA   expr_value_h
      ADC   expr_acc_h
      BCS   @bad
      STA   expr_value_h
      CLC
      LDA   expr_value_l
      ADC   expr_digit
      STA   expr_value_l
      LDA   expr_value_h
      ADC   #0
      BCS   @bad
      STA   expr_value_h
      CLC
      RTS
@bad:
      SEC
      RTS

expr_character:
      JSR   expr_next
      JSR   expr_next
      BCC   @bad
      CMP   #$0A
      BEQ   @bad
      STA   expr_digit
      JSR   expr_next
      BCC   @bad
      CMP   #39
      BNE   @bad
      LDA   expr_digit
      STA   expr_value_l
      STZ   expr_value_h
      CLC
      RTS
@bad:
      SEC
      RTS

expr_decode_hex:
      CMP   #'0'
      BCC   @bad
      CMP   #'9'+1
      BCC   @digit
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

expr_apply:
      CMP   #EXPR_BOOL_OR
      long_beq @bool_or
      CMP   #EXPR_BOOL_AND
      long_beq @bool_and
      CMP   #EXPR_BOOL_XOR
      long_beq @bool_xor
      CMP   #EXPR_EQ
      long_beq @eq
      CMP   #EXPR_NE
      long_beq @ne
      CMP   #EXPR_LT
      long_beq @lt
      CMP   #EXPR_LE
      long_beq @le
      CMP   #EXPR_GT
      long_beq @gt
      CMP   #EXPR_GE
      long_beq @ge
      CMP   #EXPR_BIT_OR
      long_beq @bit_or
      CMP   #EXPR_BIT_XOR
      long_beq @bit_xor
      CMP   #EXPR_BIT_AND
      long_beq @bit_and
      CMP   #EXPR_SHL
      long_beq @shl
      CMP   #EXPR_SHR
      long_beq @shr
      CMP   #EXPR_ADD
      long_beq @add
      CMP   #EXPR_SUB
      long_beq @sub
      CMP   #EXPR_MUL
      long_beq @mul
      CMP   #EXPR_DIV
      long_beq @div
      CMP   #EXPR_MOD
      long_beq @mod
      JMP   @bad
@bool_or:
      LDA   expr_value_l
      ORA   expr_value_h
      ORA   expr_rhs_l
      ORA   expr_rhs_h
      BRA   @set_nonzero
@bool_and:
      LDA   expr_value_l
      ORA   expr_value_h
      BEQ   @false
      LDA   expr_rhs_l
      ORA   expr_rhs_h
@set_nonzero:
      BEQ   @false
@true:
      LDA   #1
      BRA   @set_bool
@bool_xor:
      LDA   expr_value_l
      ORA   expr_value_h
      BEQ   :+
      LDA   #1
:
      STA   expr_digit
      LDA   expr_rhs_l
      ORA   expr_rhs_h
      BEQ   :+
      LDA   #1
:
      EOR   expr_digit
      BRA   @set_bool
@false:
      LDA   #0
@set_bool:
      STA   expr_value_l
      STZ   expr_value_h
      CLC
      RTS
@eq:
      LDA   expr_value_h
      CMP   expr_rhs_h
      BNE   @false
      LDA   expr_value_l
      CMP   expr_rhs_l
      BEQ   @true
      BRA   @false
@ne:
      LDA   expr_value_h
      CMP   expr_rhs_h
      BNE   @true
      LDA   expr_value_l
      CMP   expr_rhs_l
      BNE   @true
      BRA   @false
@lt:
      JSR   expr_compare
      BCC   @true
      BRA   @false
@le:
      JSR   expr_compare
      BCC   @true
      BEQ   @true
      BRA   @false
@gt:
      JSR   expr_compare
      BCC   @false
      BEQ   @false
      BRA   @true
@ge:
      JSR   expr_compare
      BCC   @false
      BRA   @true
@bit_or:
      LDA   expr_value_l
      ORA   expr_rhs_l
      STA   expr_value_l
      LDA   expr_value_h
      ORA   expr_rhs_h
      STA   expr_value_h
      CLC
      RTS
@bit_xor:
      LDA   expr_value_l
      EOR   expr_rhs_l
      STA   expr_value_l
      LDA   expr_value_h
      EOR   expr_rhs_h
      STA   expr_value_h
      CLC
      RTS
@bit_and:
      LDA   expr_value_l
      AND   expr_rhs_l
      STA   expr_value_l
      LDA   expr_value_h
      AND   expr_rhs_h
      STA   expr_value_h
      CLC
      RTS
@add:
      CLC
      LDA   expr_value_l
      ADC   expr_rhs_l
      STA   expr_value_l
      LDA   expr_value_h
      ADC   expr_rhs_h
      STA   expr_value_h
      CLC
      RTS
@sub:
      SEC
      LDA   expr_value_l
      SBC   expr_rhs_l
      STA   expr_value_l
      LDA   expr_value_h
      SBC   expr_rhs_h
      STA   expr_value_h
      CLC
      RTS
@shl:
      LDX   expr_rhs_l
      LDA   expr_rhs_h
      BNE   @zero
      CPX   #16
      BCS   @zero
@shl_loop:
      CPX   #0
      BEQ   @ok
      ASL   expr_value_l
      ROL   expr_value_h
      DEX
      BRA   @shl_loop
@shr:
      LDX   expr_rhs_l
      LDA   expr_rhs_h
      BNE   @zero
      CPX   #16
      BCS   @zero
@shr_loop:
      CPX   #0
      BEQ   @ok
      LSR   expr_value_h
      ROR   expr_value_l
      DEX
      BRA   @shr_loop
@zero:
      STZ   expr_value_l
      STZ   expr_value_h
@ok:
      CLC
      RTS
@mul:
      JSR   expr_multiply
      CLC
      RTS
@div:
      LDA   #EXPR_DIV
      BRA   @divide
@mod:
      LDA   #EXPR_MOD
@divide:
      STA   expr_digit
      LDA   expr_rhs_l
      ORA   expr_rhs_h
      BEQ   @bad
      JSR   expr_divide
      CLC
      RTS
@bad:
      SEC
      RTS

; Unsigned compare: flags reflect lhs - rhs, carry means lhs >= rhs.
expr_compare:
      LDA   expr_value_h
      CMP   expr_rhs_h
      BNE   @done
      LDA   expr_value_l
      CMP   expr_rhs_l
@done:
      RTS

expr_multiply:
      word_multiply expr_value_l, expr_value_h, expr_rhs_l, expr_rhs_h, expr_acc_l, expr_acc_h
      RTS

expr_divide:
      word_divide expr_value_l, expr_value_h, expr_rhs_l, expr_rhs_h, expr_acc_l, expr_acc_h, expr_rem_l, expr_rem_h, expr_digit, EXPR_MOD
      RTS

expr_skip_spaces:
      LDX   expr_pos
@loop:
      CPX   expr_len
      BCS   @done
      LDA   expr_buf,X
      CMP   #' '
      BEQ   @skip
      CMP   #$09
      BNE   @done
@skip:
      INX
      BRA   @loop
@done:
      STX   expr_pos
      RTS

expr_peek:
      LDX   expr_pos
      CPX   expr_len
      BCS   @eof
      LDA   expr_buf,X
      SEC
      RTS
@eof:
      CLC
      RTS

expr_peek_upper:
      JSR   expr_peek
      BCC   @done
      JSR   uppercase
      SEC
@done:
      RTS

expr_next:
      JSR   expr_peek
      BCC   @done
      INC   expr_pos
      SEC
@done:
      RTS

parse_defined_condition:
      JSR   scan_next_token
      LDA   token_len
      BEQ   @error
      JSR   find_macro
      LDA   #0
      ROL
      STA   condition_value
      JSR   ensure_line_end
      RTS
@error:
      LDA   #1
      RTS

push_condition:
      LDX   cond_depth
      CPX   #COND_CAP
      BCS   @error
      LDA   current_active
      STA   cond_parent,X
      LDA   condition_value
      AND   #1
      STA   cond_taken,X
      STZ   cond_else,X
      AND   current_active
      STA   current_active
      INC   cond_depth
      LDA   #0
      RTS
@error:
      LDA   #1
      RTS

switch_else:
      LDX   cond_depth
      BEQ   @error
      DEX
      LDA   cond_else,X
      BNE   @error
      INC   cond_else,X
      LDA   cond_taken,X
      EOR   #1
      AND   cond_parent,X
      STA   current_active
      LDA   #0
      RTS
@error:
      LDA   #1
      RTS

switch_elseif:
      LDX   cond_depth
      BEQ   @error
      DEX
      LDA   cond_else,X
      BNE   @error
      LDA   cond_taken,X
      BNE   @already_taken
      LDA   condition_value
      AND   #1
      STA   cond_taken,X
      AND   cond_parent,X
      STA   current_active
      LDA   #0
      RTS
@already_taken:
      STZ   current_active
      LDA   #0
      RTS
@error:
      LDA   #1
      RTS

pop_condition:
      LDX   cond_depth
      BEQ   @error
      DEX
      STX   cond_depth
      LDA   cond_parent,X
      STA   current_active
      LDA   #0
      RTS
@error:
      LDA   #1
      RTS

; ---------------------------------------------------------------------
; Macro definitions and invocation arguments
; ---------------------------------------------------------------------

; Seed one `-DNAME=VALUE` definition from the resident tool mailbox.
seed_command_define:
      LDA   NASPP_DEFINE_LEN
      BEQ   @ok
      LDX   #0
      STZ   token_len
@name:
      CPX   NASPP_DEFINE_LEN
      BCS   @syntax
      LDA   NASPP_DEFINE,X
      CMP   #'='
      BEQ   @value_start
      JSR   uppercase
      CPX   #0
      BNE   :+
      JSR   is_ident_start
      BCC   @syntax
      BRA   @store_name
:
      JSR   is_ident_char
      BCC   @syntax
@store_name:
      LDY   token_len
      CPY   #NAME_CAP-1
      BCS   @capacity
      STA   macro_names,Y
      INC   token_len
      INX
      BRA   @name
@value_start:
      LDA   token_len
      BEQ   @syntax
      STA   macro_name_lens
      LDA   #3
      STA   macro_kind
      STZ   macro_param_count
      INX
      STZ   command_value_len
@value:
      CPX   NASPP_DEFINE_LEN
      BCS   @value_done
      LDY   command_value_len
      CPY   #ARG_CAP
      BCS   @capacity
      LDA   NASPP_DEFINE,X
      STA   command_value,Y
      INC   command_value_len
      INX
      BRA   @value
@value_done:
      LDA   command_value_len
      BEQ   @syntax
      INC   macro_count
@ok:
      LDA   #0
      RTS
@capacity:
      LDA   #NASPP_ERR_CAPACITY
      BRA   @error
@syntax:
      LDA   #NASPP_ERR_SYNTAX
@error:
      STA   NASPP_ERROR
      LDA   #1
      STA   NASPP_ERROR_LINE
      STA   NASPP_ERROR_COL
      RTS

define_macro:
      LDA   macro_count
      CMP   #MACRO_CAP
      BCC   :+
      LDA   #NASPP_ERR_CAPACITY
      STA   NASPP_ERROR
      LDA   #1
      RTS
:
      JSR   scan_next_token
      LDA   token_len
      BEQ   @syntax
      CMP   #NAME_CAP
      BCS   @capacity
      JSR   find_macro
      BCS   @syntax
      LDA   macro_count
      STA   macro_slot
      TAX
      STZ   macro_kind,X
      JSR   store_macro_name
      JSR   parse_macro_params
      BNE   @syntax
      LDX   macro_slot
      LDA   input_pos
      STA   macro_body_start_l,X
      LDA   input_pos+1
      STA   macro_body_start_h,X
      JSR   store_macro_source
      JSR   emit_skipped_line
      BNE   @error
@body:
      JSR   read_line
      BNE   @syntax
      JSR   prepare_token
      LDA   #<kw_endmacro
      LDX   #>kw_endmacro
      JSR   token_equals
      BEQ   @end
      JSR   emit_skipped_line
      BNE   @error
      BRA   @body
@end:
      JSR   ensure_line_end
      BNE   @syntax
      LDX   macro_slot
      LDA   line_start
      STA   macro_body_end_l,X
      LDA   line_start+1
      STA   macro_body_end_h,X
      JSR   emit_skipped_line
      BNE   @error
      INC   macro_count
      LDA   #0
      RTS
@capacity:
      LDA   #NASPP_ERR_CAPACITY
      STA   NASPP_ERROR
@error:
      LDA   #1
      RTS
@syntax:
      LDA   #NASPP_ERR_SYNTAX
      STA   NASPP_ERROR
      LDA   #1
      RTS

define_constant:
      LDA   macro_count
      CMP   #MACRO_CAP
      long_bcs @capacity
      JSR   scan_next_token
      LDA   token_len
      long_beq @syntax
      CMP   #NAME_CAP
      long_bcs @capacity
      JSR   find_macro
      long_bcs @syntax
      LDA   macro_count
      STA   macro_slot
      TAX
      LDA   #1
      STA   macro_kind,X
      STZ   macro_param_count,X
      JSR   store_macro_name
      JSR   skip_spaces
      LDX   parse_pos
      STX   arg_start
      STX   arg_end
@value:
      CPX   line_len
      BCS   @value_done
      LDA   line_buf,X
      CMP   #';'
      BEQ   @value_done
      INX
      CMP   #' '
      BEQ   @value
      CMP   #$09
      BEQ   @value
      STX   arg_end
      BRA   @value
@value_done:
      LDA   arg_end
      CMP   arg_start
      BEQ   @syntax
      LDX   macro_slot
      CLC
      LDA   line_start
      ADC   arg_start
      STA   macro_body_start_l,X
      LDA   line_start+1
      ADC   #0
      STA   macro_body_start_h,X
      CLC
      LDA   line_start
      ADC   arg_end
      STA   macro_body_end_l,X
      LDA   line_start+1
      ADC   #0
      STA   macro_body_end_h,X
      JSR   store_macro_source
      INC   macro_count
      JMP   emit_skipped_line
@capacity:
      LDA   #NASPP_ERR_CAPACITY
      STA   NASPP_ERROR
      LDA   #1
      RTS
@syntax:
      LDA   #NASPP_ERR_SYNTAX
      STA   NASPP_ERROR
      LDA   #1
      RTS

store_macro_source:
      LDX   macro_slot
      LDA   input_base
      STA   macro_body_base_l,X
      LDA   input_base+1
      STA   macro_body_base_m,X
      LDA   input_base+2
      STA   macro_body_base_h,X
      LDA   input_len
      STA   macro_source_len_l,X
      LDA   input_len+1
      STA   macro_source_len_h,X
      RTS

undefine_constant:
      JSR   scan_next_token
      LDA   token_len
      BEQ   @syntax
      JSR   find_macro
      BCC   @syntax
      LDX   macro_slot
      LDA   macro_kind,X
      CMP   #1
      BNE   @syntax
      LDA   #2
      STA   macro_kind,X
      JSR   ensure_line_end
      BNE   @syntax
      JMP   emit_skipped_line
@syntax:
      LDA   #NASPP_ERR_SYNTAX
      STA   NASPP_ERROR
      LDA   #1
      RTS

skip_macro_definition:
      JSR   emit_skipped_line
      BNE   @error
@body:
      JSR   read_line
      BNE   @syntax
      JSR   prepare_token
      LDA   #<kw_endmacro
      LDX   #>kw_endmacro
      JSR   token_equals
      BEQ   @end
      JSR   emit_skipped_line
      BNE   @error
      BRA   @body
@end:
      JSR   ensure_line_end
      BNE   @syntax
      JMP   emit_skipped_line
@syntax:
      LDA   #NASPP_ERR_SYNTAX
      STA   NASPP_ERROR
@error:
      LDA   #1
      RTS

store_macro_name:
      LDX   macro_slot
      LDA   token_len
      STA   macro_name_lens,X
      TXA
      ASL
      ASL
      ASL
      ASL
      TAX
      LDY   token_start
      STZ   compare_count
@copy:
      LDA   compare_count
      CMP   token_len
      BCS   @done
      LDA   line_buf,Y
      STA   macro_names,X
      INX
      INY
      INC   compare_count
      BRA   @copy
@done:
      RTS

parse_macro_params:
      STZ   param_index
@next:
      JSR   skip_spaces
      LDX   parse_pos
      CPX   line_len
      BCS   @done
      LDA   line_buf,X
      CMP   #';'
      BEQ   @done
      LDA   param_index
      CMP   #PARAM_CAP
      BCS   @capacity
      JSR   scan_next_token
      LDA   token_len
      BEQ   @syntax
      CMP   #NAME_CAP
      BCS   @capacity
      JSR   store_param_name
      INC   param_index
      JSR   skip_spaces
      LDX   parse_pos
      CPX   line_len
      BCS   @done
      LDA   line_buf,X
      CMP   #';'
      BEQ   @done
      CMP   #','
      BNE   @syntax
      INX
      STX   parse_pos
      BRA   @next
@done:
      LDX   macro_slot
      LDA   param_index
      STA   macro_param_count,X
      LDA   #0
      RTS
@capacity:
      LDA   #NASPP_ERR_CAPACITY
      STA   NASPP_ERROR
      LDA   #1
      RTS
@syntax:
      LDA   #1
      RTS

store_param_name:
      JSR   compute_param_slot
      LDX   flat_slot
      LDA   token_len
      STA   macro_param_lens,X
      JSR   point_to_param_name
      LDY   #0
      LDX   token_start
@copy:
      CPY   token_len
      BCS   @done
      LDA   line_buf,X
      STA   (work_ptr),Y
      INX
      INY
      BRA   @copy
@done:
      RTS

compute_param_slot:
      LDA   macro_slot
      ASL
      ASL
      CLC
      ADC   param_index
      STA   flat_slot
      RTS

point_to_param_name:
      LDA   flat_slot
      STA   work_ptr
      STZ   work_ptr+1
      ASL   work_ptr
      ROL   work_ptr+1
      ASL   work_ptr
      ROL   work_ptr+1
      ASL   work_ptr
      ROL   work_ptr+1
      ASL   work_ptr
      ROL   work_ptr+1
      CLC
      LDA   work_ptr
      ADC   #<macro_param_names
      STA   work_ptr
      LDA   work_ptr+1
      ADC   #>macro_param_names
      STA   work_ptr+1
      RTS

find_macro:
      STZ   macro_slot
@slot:
      LDX   macro_slot
      CPX   macro_count
      BCS   @miss
      LDA   macro_kind,X
      CMP   #2
      BEQ   @next
      LDA   macro_name_lens,X
      CMP   token_len
      BNE   @next
      TXA
      ASL
      ASL
      ASL
      ASL
      STA   compare_count
      LDY   token_start
      LDX   #0
@compare:
      CPX   token_len
      BCS   @hit
      LDA   line_buf,Y
      JSR   uppercase
      STA   byte_value
      PHX
      TXA
      CLC
      ADC   compare_count
      TAX
      LDA   macro_names,X
      JSR   uppercase
      PLX
      CMP   byte_value
      BNE   @next
      INX
      INY
      BRA   @compare
@next:
      INC   macro_slot
      BRA   @slot
@hit:
      SEC
      RTS
@miss:
      CLC
      RTS

parse_arguments:
      STZ   arg_count
      LDX   macro_slot
      LDA   macro_param_count,X
      BEQ   @no_args
@next:
      JSR   skip_spaces
      LDX   parse_pos
      STX   arg_start
      STX   arg_end
@scan:
      CPX   line_len
      BCS   @captured
      LDA   line_buf,X
      CMP   #';'
      BEQ   @captured
      CMP   #','
      BEQ   @captured
      INX
      CMP   #' '
      BEQ   @scan
      CMP   #$09
      BEQ   @scan
      STX   arg_end
      BRA   @scan
@captured:
      STX   parse_pos
      LDA   arg_end
      SEC
      SBC   arg_start
      BEQ   @error
      CMP   #ARG_CAP+1
      BCS   @error
      JSR   store_argument
      INC   arg_count
      LDX   parse_pos
      LDY   macro_slot
      LDA   arg_count
      CMP   macro_param_count,Y
      BEQ   @last
      CPX   line_len
      BCS   @error
      LDA   line_buf,X
      CMP   #','
      BNE   @error
      INX
      STX   parse_pos
      BRA   @next
@last:
      STX   parse_pos
      JSR   ensure_line_end
      RTS
@no_args:
      JMP   ensure_line_end
@error:
      LDA   #1
      RTS

store_argument:
      LDX   arg_count
      STX   param_index
      LDA   arg_end
      SEC
      SBC   arg_start
      STA   arg_lens,X
      JSR   point_to_argument
      LDY   #0
      LDX   arg_start
@copy:
      CPX   arg_end
      BCS   @done
      LDA   line_buf,X
      STA   (work_ptr),Y
      INX
      INY
      BRA   @copy
@done:
      RTS

point_to_argument:
      LDA   param_index
      ASL
      ASL
      ASL
      ASL
      STA   work_ptr
      ASL
      CLC
      ADC   work_ptr
      STA   work_ptr
      STZ   work_ptr+1
      CLC
      LDA   work_ptr
      ADC   #<arg_text
      STA   work_ptr
      LDA   work_ptr+1
      ADC   #>arg_text
      STA   work_ptr+1
      RTS

; ---------------------------------------------------------------------
; Macro body expansion
; ---------------------------------------------------------------------

expand_macro:
      LDA   #1
      STA   params_active
      LDX   #2
@save_base:
      LDA   input_base,X
      STA   saved_input_base,X
      DEX
      BPL   @save_base
      LDA   input_len
      STA   saved_input_len
      LDA   input_len+1
      STA   saved_input_len+1
      LDA   input_pos
      STA   saved_input_pos
      LDA   input_pos+1
      STA   saved_input_pos+1
      LDA   current_line
      STA   saved_line
      LDA   current_line+1
      STA   saved_line+1
      LDX   macro_slot
      LDA   macro_body_start_l,X
      STA   input_pos
      LDA   macro_body_start_h,X
      STA   input_pos+1
      LDA   macro_body_end_l,X
      STA   expansion_end
      LDA   macro_body_end_h,X
      STA   expansion_end+1
      LDA   macro_body_base_l,X
      STA   input_base
      LDA   macro_body_base_m,X
      STA   input_base+1
      LDA   macro_body_base_h,X
      STA   input_base+2
      LDA   macro_source_len_l,X
      STA   input_len
      LDA   macro_source_len_h,X
      STA   input_len+1
@line:
      LDA   input_pos+1
      CMP   expansion_end+1
      BCC   @read
      BNE   @done
      LDA   input_pos
      CMP   expansion_end
      BCS   @done
@read:
      JSR   read_line
      BNE   @error
      JSR   expand_body_line
      BNE   @error
      BRA   @line
@done:
      JSR   restore_expansion_source
      LDA   saved_input_pos
      STA   input_pos
      LDA   saved_input_pos+1
      STA   input_pos+1
      LDA   saved_line
      STA   current_line
      LDA   saved_line+1
      STA   current_line+1
      STZ   params_active
      LDA   #0
      RTS
@error:
      JSR   restore_expansion_source
      LDA   saved_input_pos
      STA   input_pos
      LDA   saved_input_pos+1
      STA   input_pos+1
      LDA   saved_line
      STA   current_line
      LDA   saved_line+1
      STA   current_line+1
      STZ   params_active
      LDA   #1
      RTS

restore_expansion_source:
      LDX   #2
@base:
      LDA   saved_input_base,X
      STA   input_base,X
      DEX
      BPL   @base
      LDA   saved_input_len
      STA   input_len
      LDA   saved_input_len+1
      STA   input_len+1
      RTS

; ponytail: expansion is deliberately one level; recurse through the same
; line dispatcher when real-world macros require nested macro invocations.
expand_body_line:
      STZ   line_index
@next:
      LDX   line_index
      CPX   line_len
      BCC   :+
      JMP   @lf
:
      LDA   line_buf,X
      CMP   #';'
      BNE   :+
      JMP   @raw_tail
:
      CMP   #'"'
      BEQ   @quoted
      CMP   #$27
      BEQ   @quoted
      CMP   #'.'
      BEQ   @operator_alias
      JSR   is_ident_start
      long_bcs @identifier
      LDX   line_index
      LDA   line_buf,X
      JSR   emit_byte
      BEQ   :+
      JMP   @error
:
      INC   line_index
      BRA   @next

@operator_alias:
      STX   token_start
      INX
@alias_scan:
      CPX   line_len
      BCS   @alias_ready
      LDA   line_buf,X
      JSR   is_ident_char
      BCC   @alias_ready
      INX
      BRA   @alias_scan
@alias_ready:
      STX   parse_pos
      TXA
      SEC
      SBC   token_start
      STA   token_len
      JSR   emit_operator_alias
      BCC   @emit_plain_dot
      long_bne @error
      LDA   parse_pos
      STA   line_index
      JMP   @next
@emit_plain_dot:
      LDA   #'.'
      JSR   emit_byte
      long_bne @error
      INC   line_index
      JMP   @next

@quoted:
      STA   quote_char
      LDX   line_index
      LDA   line_buf,X
      JSR   emit_byte
      BEQ   :+
      JMP   @error
:
      INC   line_index
@quote_loop:
      LDX   line_index
      CPX   line_len
      BCC   :+
      JMP   @lf
:
      LDA   line_buf,X
      STA   byte_value
      JSR   emit_byte
      BEQ   :+
      JMP   @error
:
      INC   line_index
      LDA   byte_value
      CMP   quote_char
      BNE   @quote_loop
      JMP   @next

@identifier:
      LDA   line_index
      STA   token_start
      STZ   token_len
@ident_loop:
      LDX   line_index
      CPX   line_len
      BCS   @ident_done
      LDA   line_buf,X
      JSR   is_ident_char
      BCC   @ident_done
      INC   line_index
      INC   token_len
      BRA   @ident_loop
@ident_done:
      LDA   params_active
      BEQ   @check_define
      JSR   find_param
      BCC   @check_define
      JSR   emit_argument
      BNE   @error
      JMP   @next
@check_define:
      JSR   find_define
      BCC   @emit_identifier
      JSR   emit_define
      BNE   @error
      JMP   @next
@emit_identifier:
      LDA   token_start
      STA   compare_count
@emit_ident_loop:
      LDA   compare_count
      CMP   line_index
      BCC   :+
      JMP   @next
:
      TAX
      LDA   line_buf,X
      JSR   emit_byte
      BNE   @error
      INC   compare_count
      BRA   @emit_ident_loop

@raw_tail:
      LDX   line_index
@raw_loop:
      CPX   line_len
      BCS   @lf
      LDA   line_buf,X
      PHX
      JSR   emit_byte
      BEQ   :+
      PLX
      BRA   @error
:
      PLX
      INX
      STX   line_index
      BRA   @raw_loop
@lf:
      JMP   emit_line_lf
@error:
      LDA   #1
      RTS

; Normalize ca65 operator names before the language-neutral backend sees them.
emit_operator_alias:
      LDA   #<op_or
      LDX   #>op_or
      JSR   token_equals
      BEQ   @bool_or
      LDA   #<op_and
      LDX   #>op_and
      JSR   token_equals
      BEQ   @bool_and
      LDA   #<op_xor
      LDX   #>op_xor
      JSR   token_equals
      BEQ   @bool_xor
      LDA   #<op_bitor
      LDX   #>op_bitor
      JSR   token_equals
      BEQ   @bit_or
      LDA   #<op_bitxor
      LDX   #>op_bitxor
      JSR   token_equals
      BEQ   @bit_xor
      LDA   #<op_bitand
      LDX   #>op_bitand
      JSR   token_equals
      BEQ   @bit_and
      LDA   #<op_shl
      LDX   #>op_shl
      JSR   token_equals
      BEQ   @shl
      LDA   #<op_shr
      LDX   #>op_shr
      JSR   token_equals
      BEQ   @shr
      LDA   #<op_mod
      LDX   #>op_mod
      JSR   token_equals
      BEQ   @mod
      LDA   #<op_not
      LDX   #>op_not
      JSR   token_equals
      BEQ   @bool_not
      LDA   #<op_bitnot
      LDX   #>op_bitnot
      JSR   token_equals
      BEQ   @bit_not
      LDA   #<op_lobyte
      LDX   #>op_lobyte
      JSR   token_equals
      BEQ   @low
      LDA   #<op_hibyte
      LDX   #>op_hibyte
      JSR   token_equals
      BEQ   @high
      CLC
      RTS
@bool_or:
      LDA   #'|'
      LDX   #'|'
      BRA   @emit
@bool_and:
      LDA   #'&'
      LDX   #'&'
      BRA   @emit
@bool_xor:
      LDA   #'^'
      LDX   #'^'
      BRA   @emit
@bit_or:
      LDA   #'|'
      BRA   @one
@bit_xor:
      LDA   #'^'
      BRA   @one
@bit_and:
      LDA   #'&'
      BRA   @one
@shl:
      LDA   #'<'
      LDX   #'<'
      BRA   @emit
@shr:
      LDA   #'>'
      LDX   #'>'
      BRA   @emit
@mod:
      LDA   #'%'
      BRA   @one
@bool_not:
      LDA   #'!'
      BRA   @one
@bit_not:
      LDA   #'~'
      BRA   @one
@low:
      LDA   #'<'
      BRA   @one
@high:
      LDA   #'>'
@one:
      LDX   #0
@emit:
      STX   quote_char
      JSR   emit_byte
      BNE   @error
      LDA   quote_char
      BEQ   @ok
      JSR   emit_byte
      BNE   @error
@ok:
      SEC
      LDA   #0
      RTS
@error:
      SEC
      LDA   #1
      RTS

find_define:
      LDA   macro_slot
      STA   saved_macro_slot
      JSR   find_macro
      BCC   @miss
      LDX   macro_slot
      LDA   macro_kind,X
      CMP   #1
      BEQ   @hit
      CMP   #3
      BNE   @miss
@hit:
      STX   define_slot
      LDA   saved_macro_slot
      STA   macro_slot
      SEC
      RTS
@miss:
      LDA   saved_macro_slot
      STA   macro_slot
      CLC
      RTS

emit_define:
      LDX   define_slot
      LDA   macro_kind,X
      CMP   #3
      BEQ   @command
      LDA   input_pos
      PHA
      LDA   input_pos+1
      PHA
      JSR   use_define_source
      LDX   define_slot
      LDA   macro_body_start_l,X
      STA   input_pos
      LDA   macro_body_start_h,X
      STA   input_pos+1
      JSR   map_input
@loop:
      LDA   input_pos+1
      CMP   macro_body_end_h,X
      BCC   @emit
      BNE   @done
      LDA   input_pos
      CMP   macro_body_end_l,X
      BCS   @done
@emit:
      PHX
      JSR   read_input_byte
      STA   byte_value
      JSR   increment_input
      LDA   byte_value
      JSR   emit_byte
      BEQ   :+
      PLX
      BRA   @error
:
      PLX
      BRA   @loop
@done:
      JSR   restore_define_source
      PLA
      STA   input_pos+1
      PLA
      STA   input_pos
      LDA   #0
      RTS
@error:
      JSR   restore_define_source
      PLA
      STA   input_pos+1
      PLA
      STA   input_pos
      LDA   #1
      RTS
@command:
      LDY   #0
@command_byte:
      CPY   command_value_len
      BCS   @command_done
      LDA   command_value,Y
      PHY
      JSR   emit_byte
      BEQ   :+
      PLY
      LDA   #1
      RTS
:
      PLY
      INY
      BRA   @command_byte
@command_done:
      LDA   #0
      RTS

find_param:
      STZ   param_index
@next:
      LDX   macro_slot
      LDA   param_index
      CMP   macro_param_count,X
      BCS   @miss
      JSR   compute_param_slot
      LDX   flat_slot
      LDA   macro_param_lens,X
      CMP   token_len
      BNE   @advance
      JSR   point_to_param_name
      LDY   #0
      LDX   token_start
@compare:
      CPY   token_len
      BCS   @hit
      LDA   line_buf,X
      JSR   uppercase
      STA   byte_value
      LDA   (work_ptr),Y
      JSR   uppercase
      CMP   byte_value
      BNE   @advance
      INX
      INY
      BRA   @compare
@advance:
      INC   param_index
      BRA   @next
@hit:
      SEC
      RTS
@miss:
      CLC
      RTS

emit_argument:
      LDX   param_index
      LDA   arg_lens,X
      STA   compare_count
      JSR   point_to_argument
      LDY   #0
@loop:
      CPY   compare_count
      BCS   @done
      LDA   (work_ptr),Y
      PHY
      JSR   emit_byte
      BEQ   :+
      PLY
      BRA   @error
:
      PLY
      INY
      BRA   @loop
@done:
      LDA   #0
      RTS
@error:
      LDA   #1
      RTS

      .segment "RODATA"
kw_if:       .byte 3, ".if"
kw_ifdef:    .byte 6, ".ifdef"
kw_ifndef:   .byte 7, ".ifndef"
kw_elseif:   .byte 7, ".elseif"
kw_else:     .byte 5, ".else"
kw_endif:    .byte 6, ".endif"
kw_macro:    .byte 6, ".macro"
kw_define:   .byte 7, ".define"
kw_undefine: .byte 9, ".undefine"
kw_endmacro: .byte 9, ".endmacro"
kw_includetext:.byte 12, ".includetext"
op_or:       .byte 3, ".OR"
op_and:      .byte 4, ".AND"
op_xor:      .byte 4, ".XOR"
op_bitor:    .byte 6, ".BITOR"
op_bitxor:   .byte 7, ".BITXOR"
op_bitand:   .byte 7, ".BITAND"
op_shl:      .byte 4, ".SHL"
op_shr:      .byte 4, ".SHR"
op_mod:      .byte 4, ".MOD"
op_not:      .byte 4, ".NOT"
op_bitnot:   .byte 7, ".BITNOT"
op_lobyte:   .byte 7, ".LOBYTE"
op_hibyte:   .byte 7, ".HIBYTE"

; Nova Ozmoo minimal Z3 interpreter.
;
; Executes real Z-code until the first read/sread opcode. Input parsing is not
; implemented yet; this milestone is enough to let the story print its banner,
; initial room text, prompt, and status line.

.include "zstory.inc"
.include "ztext.inc"
.include "zobject.inc"
.include "zvm.inc"
.include "vtext.inc"

.ifndef NOVA_OZMOO_ZVM_IMPLEMENTATION_INCLUDED
NOVA_OZMOO_ZVM_IMPLEMENTATION_INCLUDED = 1

.segment "ZEROPAGE"

zvm_pc_l:             .res 1
zvm_pc_h:             .res 1
zvm_opcode_pc_l:      .res 1
zvm_opcode_pc_h:      .res 1
zvm_opcode:           .res 1
zvm_form:             .res 1
zvm_opnum:            .res 1
zvm_type_byte:        .res 1
zvm_operand_count:    .res 1
zvm_operand_limit:    .res 1
zvm_raw_first:        .res 1
zvm_value_lo:         .res 1
zvm_value_hi:         .res 1
zvm_tmp:              .res 1
zvm_tmp2:             .res 1
zvm_varnum:           .res 1
zvm_store_var:        .res 1
zvm_branch_if:        .res 1
zvm_branch_off_l:     .res 1
zvm_branch_off_h:     .res 1
zvm_frame_base:       .res 1
zvm_frame_bank:       .res 1
zvm_frame_lo_ptr_l:   .res 1
zvm_frame_lo_ptr_h:   .res 1
zvm_frame_hi_ptr_l:   .res 1
zvm_frame_hi_ptr_h:   .res 1
zvm_ret_lo:           .res 1
zvm_ret_hi:           .res 1
zvm_status_x:         .res 1
zvm_status_y:         .res 1
zvm_jump_l:           .res 1
zvm_jump_h:           .res 1
zvm_pc_b:             .res 1
zvm_opcode_pc_b:      .res 1

.segment "BSS"

zvm_stop_reason:      .res 1
zvm_sp:               .res 1
zvm_frame_count:      .res 1
zvm_operand_lo:       .res 4
zvm_operand_hi:       .res 4
zvm_locals_lo:        .res ZVM_LOCAL_MAX
zvm_locals_hi:        .res ZVM_LOCAL_MAX
zvm_stack_lo:         .res ZVM_STACK_MAX
zvm_stack_hi:         .res ZVM_STACK_MAX
zvm_frame_ret_lo:     .res ZVM_FRAME_MAX
zvm_frame_ret_hi:     .res ZVM_FRAME_MAX
zvm_frame_ret_bank:   .res ZVM_FRAME_MAX
zvm_frame_store:      .res ZVM_FRAME_MAX
zvm_frame_sp:         .res ZVM_FRAME_MAX
zvm_frame_locals_lo:  .res ZVM_FRAME_MAX * ZVM_LOCAL_MAX
zvm_frame_locals_hi:  .res ZVM_FRAME_MAX * ZVM_LOCAL_MAX
zvm_math_lhs_lo:      .res 1
zvm_math_lhs_hi:      .res 1
zvm_math_rhs_lo:      .res 1
zvm_math_rhs_hi:      .res 1
zvm_math_res_lo:      .res 1
zvm_math_res_hi:      .res 1
zvm_math_rem_lo:      .res 1
zvm_math_rem_hi:      .res 1
zvm_math_sign:        .res 1
zvm_math_rem_sign:    .res 1
zvm_math_count:       .res 1
zvm_rng_lo:           .res 1
zvm_rng_hi:           .res 1
zvm_text_lo:          .res 1
zvm_text_hi:          .res 1
zvm_parse_lo:         .res 1
zvm_parse_hi:         .res 1
zvm_input_max:        .res 1
zvm_input_len:        .res 1
zvm_input_pos:        .res 1
zvm_parse_max:        .res 1
zvm_parse_count:      .res 1
zvm_token_start:      .res 1
zvm_token_len:        .res 1
zvm_dict_lo:          .res 1
zvm_dict_hi:          .res 1
zvm_dict_sep_count:   .res 1
zvm_dict_entry_len:   .res 1
zvm_dict_count_lo:    .res 1
zvm_dict_count_hi:    .res 1
zvm_dict_entry_lo:    .res 1
zvm_dict_entry_hi:    .res 1
zvm_dict_scan_lo:     .res 1
zvm_dict_scan_hi:     .res 1
zvm_dict_found_lo:    .res 1
zvm_dict_found_hi:    .res 1
zvm_encode_slot:      .res 1
zvm_encode_zchars:    .res 6
zvm_encode_bytes:     .res 4
zvm_char_tmp:         .res 1
zvm_stream_flags:     .res 1
zvm_stream3_lo:       .res 1
zvm_stream3_hi:       .res 1
zvm_stream3_len_lo:   .res 1
zvm_stream3_len_hi:   .res 1

.segment "CODE"

.export zvm_run_until_read
.export zvm_sp
.export zvm_frame_count
.export zvm_operand_lo
.export zvm_operand_hi
.export zvm_locals_lo
.export zvm_locals_hi
.export zvm_pc_b
.export zvm_stream_flags
.export zvm_stream3_lo
.export zvm_stream3_hi
.export zvm_stream3_len_lo
.export zvm_stream3_len_hi

zvm_run_until_read:
        STZ zvm_stop_reason
        STZ zvm_sp
        STZ zvm_frame_count
        STZ zvm_stream_flags
        LDA #$4D
        STA zvm_rng_lo
        LDA #$21
        STA zvm_rng_hi
        JSR zvm_clear_locals
        LDA zstory_initial_pc_lo
        STA zvm_pc_l
        LDA zstory_initial_pc_hi
        STA zvm_pc_h
        STZ zvm_pc_b
@loop:
        JSR zvm_step
        LDA zvm_stop_reason
        BEQ @loop
        RTS

zvm_clear_locals:
        LDX #$00
@loop:
        STZ zvm_locals_lo,X
        STZ zvm_locals_hi,X
        INX
        CPX #ZVM_LOCAL_MAX
        BCC @loop
        RTS

zvm_step:
        LDA zvm_pc_l
        STA zvm_opcode_pc_l
        LDA zvm_pc_h
        STA zvm_opcode_pc_h
        LDA zvm_pc_b
        STA zvm_opcode_pc_b
        JSR zvm_fetch
        STA zvm_opcode

        CMP #$80
        BCC zvm_decode_long
        CMP #$C0
        BCC zvm_decode_short
        JMP zvm_decode_variable

zvm_decode_long:
        LDA #$02
        STA zvm_form
        LDA zvm_opcode
        AND #$1F
        STA zvm_opnum
        LDA #$02
        STA zvm_operand_limit
        STZ zvm_operand_count
        JSR zvm_set_raw_first

        LDA zvm_opcode
        AND #$40
        BEQ @op0_small
        LDA #$02
        BRA @op0
@op0_small:
        LDA #$01
@op0:
        LDX #$00
        JSR zvm_read_operand_indexed

        LDA zvm_opcode
        AND #$20
        BEQ @op1_small
        LDA #$02
        BRA @op1
@op1_small:
        LDA #$01
@op1:
        LDX #$01
        JSR zvm_read_operand_indexed
        JMP zvm_dispatch

zvm_decode_short:
        LDA zvm_opcode
        AND #$0F
        STA zvm_opnum
        LDA zvm_opcode
        LSR
        LSR
        LSR
        LSR
        AND #$03
        CMP #$03
        BNE @one_op
        LDA #$00
        STA zvm_form
        STZ zvm_operand_count
        JMP zvm_dispatch
@one_op:
        PHA
        LDA #$01
        STA zvm_form
        STA zvm_operand_limit
        STZ zvm_operand_count
        JSR zvm_set_raw_first
        PLA
        LDX #$00
        JSR zvm_read_operand_indexed
        JMP zvm_dispatch

zvm_decode_variable:
        LDA zvm_opcode
        AND #$1F
        STA zvm_opnum
        JSR zvm_fetch
        STA zvm_type_byte
        STZ zvm_operand_count

        LDA zvm_opcode
        AND #$20
        BEQ @two_op
        LDA #$03
        STA zvm_form
        JSR zvm_var_operand_limit
        STA zvm_operand_limit
        BRA @types
@two_op:
        LDA #$02
        STA zvm_form
        LDA zvm_opnum
        CMP #$01              ; je may take 2-4 operands in variable form.
        BNE :+
        LDA #$04
        BRA @set_limit
:
        LDA #$02
@set_limit:
        STA zvm_operand_limit

@types:
        JSR zvm_set_raw_first
        LDX #$00
@type_loop:
        CPX zvm_operand_limit
        BCS @done
        TXA
        BEQ @shift6
        CMP #$01
        BEQ @shift4
        CMP #$02
        BEQ @shift2
        LDA zvm_type_byte
        BRA @have_type
@shift6:
        LDA zvm_type_byte
        LSR
        LSR
        LSR
        LSR
        LSR
        LSR
        BRA @have_type
@shift4:
        LDA zvm_type_byte
        LSR
        LSR
        LSR
        LSR
        BRA @have_type
@shift2:
        LDA zvm_type_byte
        LSR
        LSR
@have_type:
        AND #$03
        CMP #$03
        BEQ @done
        PHX
        JSR zvm_read_operand_indexed
        PLX
        INX
        BRA @type_loop
@done:
        JMP zvm_dispatch

zvm_var_operand_limit:
        LDA #$04
        RTS

zvm_set_raw_first:
        STZ zvm_raw_first
        LDA zvm_form
        CMP #$01
        BNE @not_1op
        LDA zvm_opnum
        CMP #$05
        BEQ @raw
        CMP #$06
        BEQ @raw
        CMP #$0E
        BEQ @raw
        RTS
@not_1op:
        CMP #$02
        BNE @not_2op
        LDA zvm_opnum
        CMP #$04
        BEQ @raw
        CMP #$05
        BEQ @raw
        CMP #$0D
        BEQ @raw
        RTS
@not_2op:
        CMP #$03
        BNE @done
        LDA zvm_opnum
        CMP #$09
        BEQ @raw
@done:
        RTS
@raw:
        LDA #$01
        STA zvm_raw_first
        RTS

; A = operand type, X = operand index.
zvm_read_operand_indexed:
        STA zvm_tmp
        CPX #$00
        BNE @read
        LDA zvm_raw_first
        STA zvm_tmp2
        BRA @have_raw
@read:
        STZ zvm_tmp2
@have_raw:
        LDA zvm_tmp
        BEQ @large
        CMP #$01
        BEQ @small
        ; variable operand
        PHX
        JSR zvm_fetch
        PLX
        LDY zvm_tmp2
        BEQ @deref
        STA zvm_operand_lo,X
        STZ zvm_operand_hi,X
        BRA @stored
@deref:
        PHX
        JSR zvm_get_var
        PLX
        LDA zvm_value_lo
        STA zvm_operand_lo,X
        LDA zvm_value_hi
        STA zvm_operand_hi,X
        BRA @stored
@small:
        PHX
        JSR zvm_fetch
        PLX
        STA zvm_operand_lo,X
        STZ zvm_operand_hi,X
        BRA @stored
@large:
        PHX
        JSR zvm_fetch_word
        PLX
        LDA zvm_value_lo
        STA zvm_operand_lo,X
        LDA zvm_value_hi
        STA zvm_operand_hi,X
@stored:
        INC zvm_operand_count
        RTS

zvm_dispatch:
        LDA zvm_form
        BEQ zvm_dispatch_0op
        CMP #$01
        BEQ zvm_dispatch_1op
        CMP #$02
        BEQ zvm_dispatch_2op
        JMP zvm_dispatch_var

zvm_dispatch_0op:
        LDA zvm_opnum
        CMP #zvm_0op_count
        BCC :+
        JMP zvm_unsupported
:
        ASL
        TAX
        LDA zvm_0op_table,X
        STA zvm_jump_l
        LDA zvm_0op_table + 1,X
        STA zvm_jump_h
        JMP (zvm_jump_l)

zvm_dispatch_1op:
        LDA zvm_opnum
        CMP #zvm_1op_count
        BCC :+
        JMP zvm_unsupported
:
        ASL
        TAX
        LDA zvm_1op_table,X
        STA zvm_jump_l
        LDA zvm_1op_table + 1,X
        STA zvm_jump_h
        JMP (zvm_jump_l)

zvm_dispatch_2op:
        LDA zvm_opnum
        CMP #zvm_2op_count
        BCC :+
        JMP zvm_unsupported
:
        ASL
        TAX
        LDA zvm_2op_table,X
        STA zvm_jump_l
        LDA zvm_2op_table + 1,X
        STA zvm_jump_h
        JMP (zvm_jump_l)

zvm_dispatch_var:
        LDA zvm_opnum
        CMP #zvm_var_count
        BCC :+
        JMP zvm_unsupported
:
        ASL
        TAX
        LDA zvm_var_table,X
        STA zvm_jump_l
        LDA zvm_var_table + 1,X
        STA zvm_jump_h
        JMP (zvm_jump_l)

zvm_fetch:
        LDA zvm_pc_l
        STA zstory_addr_l
        LDA zvm_pc_h
        STA zstory_addr_m
        LDA zvm_pc_b
        STA zstory_addr_h
        JSR zstory_read8
        LDA XRAM_DATA
        PHA
        INC zvm_pc_l
        BNE :+
        INC zvm_pc_h
        BNE :+
        INC zvm_pc_b
:
        PLA
        RTS

zvm_fetch_word:
        JSR zvm_fetch
        STA zvm_value_hi
        JSR zvm_fetch
        STA zvm_value_lo
        RTS

zvm_store_result:
        JSR zvm_fetch
        JMP zvm_set_var

zvm_get_var:
        STA zvm_varnum
        CMP #$00
        BNE :+
        JMP zvm_stack_pop
:
        CMP #$10
        BCS @global
        DEC
        TAX
        LDA zvm_locals_lo,X
        STA zvm_value_lo
        LDA zvm_locals_hi,X
        STA zvm_value_hi
        RTS
@global:
        SEC
        SBC #$10
        ASL
        STA zvm_tmp
        LDA #$00
        ROL
        STA zvm_tmp2
        LDA zstory_globals_lo
        CLC
        ADC zvm_tmp
        STA zstory_addr_l
        LDA zstory_globals_hi
        ADC zvm_tmp2
        STA zstory_addr_m
        STZ zstory_addr_h
        JSR zstory_read16
        LDA zstory_word_lo
        STA zvm_value_lo
        LDA zstory_word_hi
        STA zvm_value_hi
        RTS

zvm_set_var:
        STA zvm_varnum
        CMP #$00
        BNE :+
        JMP zvm_stack_push
:
        CMP #$10
        BCS @global
        DEC
        TAX
        LDA zvm_value_lo
        STA zvm_locals_lo,X
        LDA zvm_value_hi
        STA zvm_locals_hi,X
        RTS
@global:
        SEC
        SBC #$10
        ASL
        STA zvm_tmp
        LDA #$00
        ROL
        STA zvm_tmp2
        LDA zstory_globals_lo
        CLC
        ADC zvm_tmp
        STA zstory_addr_l
        LDA zstory_globals_hi
        ADC zvm_tmp2
        STA zstory_addr_m
        STZ zstory_addr_h
        LDA zvm_value_hi
        STA zstory_word_hi
        LDA zvm_value_lo
        STA zstory_word_lo
        JMP zstory_write16

zvm_get_var_indirect:
        STA zvm_varnum
        BNE @normal
        LDA zvm_sp
        BEQ @empty
        SEC
        SBC #$01
        TAX
        LDA zvm_stack_lo,X
        STA zvm_value_lo
        LDA zvm_stack_hi,X
        STA zvm_value_hi
        RTS
@normal:
        LDA zvm_varnum
        JMP zvm_get_var
@empty:
        JMP zvm_unsupported

zvm_set_var_indirect:
        STA zvm_varnum
        BNE @normal
        LDA zvm_sp
        BEQ @empty
        SEC
        SBC #$01
        TAX
        LDA zvm_value_lo
        STA zvm_stack_lo,X
        LDA zvm_value_hi
        STA zvm_stack_hi,X
        RTS
@normal:
        LDA zvm_varnum
        JMP zvm_set_var
@empty:
        JMP zvm_unsupported

zvm_stack_push:
        LDX zvm_sp
        LDA zvm_value_lo
        STA zvm_stack_lo,X
        LDA zvm_value_hi
        STA zvm_stack_hi,X
        INC zvm_sp
        RTS

zvm_stack_pop:
        DEC zvm_sp
        LDX zvm_sp
        LDA zvm_stack_lo,X
        STA zvm_value_lo
        LDA zvm_stack_hi,X
        STA zvm_value_hi
        RTS

zvm_select_frame_local_ptrs:
        LDA #<zvm_frame_locals_lo
        STA zvm_frame_lo_ptr_l
        LDA #>zvm_frame_locals_lo
        STA zvm_frame_lo_ptr_h
        LDA #<zvm_frame_locals_hi
        STA zvm_frame_hi_ptr_l
        LDA #>zvm_frame_locals_hi
        STA zvm_frame_hi_ptr_h
        LDA zvm_frame_lo_ptr_l
        CLC
        ADC zvm_frame_base
        STA zvm_frame_lo_ptr_l
        LDA zvm_frame_lo_ptr_h
        ADC zvm_frame_bank
        STA zvm_frame_lo_ptr_h
        LDA zvm_frame_hi_ptr_l
        CLC
        ADC zvm_frame_base
        STA zvm_frame_hi_ptr_l
        LDA zvm_frame_hi_ptr_h
        ADC zvm_frame_bank
        STA zvm_frame_hi_ptr_h
        RTS

zvm_rtrue:
        LDA #$01
        STA zvm_value_lo
        STZ zvm_value_hi
        JMP zvm_return

zvm_rfalse:
        STZ zvm_value_lo
        STZ zvm_value_hi
        JMP zvm_return

zvm_ret:
        LDA zvm_operand_lo
        STA zvm_value_lo
        LDA zvm_operand_hi
        STA zvm_value_hi
        JMP zvm_return

zvm_ret_popped:
        JSR zvm_stack_pop

zvm_return:
        LDA zvm_value_lo
        STA zvm_ret_lo
        LDA zvm_value_hi
        STA zvm_ret_hi
        LDA zvm_frame_count
        BNE @have_frame
        LDA #ZVM_STOP_UNSUPPORTED
        STA zvm_stop_reason
        RTS
@have_frame:
        DEC zvm_frame_count
        LDX zvm_frame_count
        LDA zvm_frame_ret_lo,X
        STA zvm_pc_l
        LDA zvm_frame_ret_hi,X
        STA zvm_pc_h
        LDA zvm_frame_ret_bank,X
        STA zvm_pc_b
        JSR zvm_check_code_target
        LDA zvm_stop_reason
        BEQ :+
        RTS
:
        LDA zvm_frame_store,X
        STA zvm_store_var
        LDA zvm_frame_sp,X
        STA zvm_sp
        TXA
        AND #$0F
        ASL
        ASL
        ASL
        ASL
        STA zvm_frame_base
        TXA
        LSR
        LSR
        LSR
        LSR
        STA zvm_frame_bank
        JSR zvm_select_frame_local_ptrs
        LDY #$00
@restore:
        LDA (zvm_frame_lo_ptr_l),Y
        STA zvm_locals_lo,Y
        LDA (zvm_frame_hi_ptr_l),Y
        STA zvm_locals_hi,Y
        INY
        CPY #ZVM_LOCAL_MAX
        BCC @restore
        LDA zvm_ret_lo
        STA zvm_value_lo
        LDA zvm_ret_hi
        STA zvm_value_hi
        LDA zvm_store_var
        JMP zvm_set_var

zvm_call_vs:
        JSR zvm_fetch
        STA zvm_store_var
        JMP zvm_call_common

zvm_call_1s:
        JSR zvm_fetch
        STA zvm_store_var
        ; fall through
zvm_call_common:
        LDA zvm_operand_lo
        ORA zvm_operand_hi
        BNE @real_call
        STZ zvm_value_lo
        STZ zvm_value_hi
        LDA zvm_store_var
        JMP zvm_set_var
@real_call:
        LDX zvm_frame_count
        CPX #ZVM_FRAME_MAX
        BCC :+
        JMP zvm_unsupported
:
        LDA zvm_pc_l
        STA zvm_frame_ret_lo,X
        LDA zvm_pc_h
        STA zvm_frame_ret_hi,X
        LDA zvm_pc_b
        STA zvm_frame_ret_bank,X
        LDA zvm_store_var
        STA zvm_frame_store,X
        LDA zvm_sp
        STA zvm_frame_sp,X
        TXA
        AND #$0F
        ASL
        ASL
        ASL
        ASL
        STA zvm_frame_base
        TXA
        LSR
        LSR
        LSR
        LSR
        STA zvm_frame_bank
        JSR zvm_select_frame_local_ptrs
        LDY #$00
@save_locals:
        LDA zvm_locals_lo,Y
        STA (zvm_frame_lo_ptr_l),Y
        LDA zvm_locals_hi,Y
        STA (zvm_frame_hi_ptr_l),Y
        INY
        CPY #ZVM_LOCAL_MAX
        BCC @save_locals
        INC zvm_frame_count
        STZ zvm_sp

        LDA zvm_operand_lo
        ASL
        STA zvm_pc_l
        LDA zvm_operand_hi
        ROL
        STA zvm_pc_h
        LDA #$00
        ROL
        STA zvm_pc_b
        JSR zvm_check_code_target
        LDA zvm_stop_reason
        BEQ :+
        RTS
:
        JSR zvm_fetch
        STA zvm_tmp              ; local count
        JSR zvm_clear_locals
        LDX #$00
@init_locals:
        CPX zvm_tmp
        BCS @copy_args
        PHX
        JSR zvm_fetch_word
        PLX
        LDA zvm_value_lo
        STA zvm_locals_lo,X
        LDA zvm_value_hi
        STA zvm_locals_hi,X
        INX
        BRA @init_locals
@copy_args:
        LDA zvm_operand_count
        BEQ @done
        DEC
        BEQ @done
        STA zvm_tmp
        LDX #$00
@arg_loop:
        LDA zvm_operand_lo + 1,X
        STA zvm_locals_lo,X
        LDA zvm_operand_hi + 1,X
        STA zvm_locals_hi,X
        INX
        CPX zvm_tmp
        BCC @arg_loop
@done:
        RTS

zvm_print:
        LDA zvm_pc_l
        STA zstory_addr_l
        LDA zvm_pc_h
        STA zstory_addr_m
        LDA zvm_pc_b
        STA zstory_addr_h
        LDA #ZTEXT_UNLIMITED
        STA ztext_word_limit
        JSR ztext_print_packed
        LDA zstory_addr_l
        STA zvm_pc_l
        LDA zstory_addr_m
        STA zvm_pc_h
        LDA zstory_addr_h
        STA zvm_pc_b
        RTS

zvm_print_ret:
        JSR zvm_print
        JSR newline
        JMP zvm_rtrue

zvm_print_addr:
        LDA zvm_operand_lo
        STA zstory_addr_l
        LDA zvm_operand_hi
        STA zstory_addr_m
        STZ zstory_addr_h
        LDA #ZTEXT_UNLIMITED
        STA ztext_word_limit
        JMP ztext_print_packed

zvm_print_paddr:
        LDA zvm_operand_lo
        ASL
        STA zstory_addr_l
        LDA zvm_operand_hi
        ROL
        STA zstory_addr_m
        LDA #$00
        ROL
        STA zstory_addr_h
        LDA #ZTEXT_UNLIMITED
        STA ztext_word_limit
        JMP ztext_print_packed

zvm_print_obj:
        LDA zvm_operand_lo
        JMP zobject_print_short_name

zvm_new_line:
        JMP newline

zvm_print_char:
        LDA zvm_operand_lo
        JMP print_char

zvm_print_num:
        LDA zvm_operand_hi
        LDX zvm_operand_lo
        JMP print_u16_dec

zvm_nop:
        RTS

zvm_pop:
        JSR zvm_stack_pop
        RTS

zvm_quit:
        LDA #ZVM_STOP_READ
        STA zvm_stop_reason
        RTS

zvm_restart:
        JMP reset

zvm_save:
        ; Save/restore persistence is a later milestone; report failure.
        LDA #$00
        JMP zvm_branch

zvm_restore:
        LDA #$00
        JMP zvm_branch

zvm_verify:
        LDA #$01
        JMP zvm_branch

zvm_jz:
        LDA zvm_operand_lo
        ORA zvm_operand_hi
        BEQ @true
        LDA #$00
        JMP zvm_branch
@true:
        LDA #$01
        JMP zvm_branch

zvm_je:
        LDA #$01
        STA zvm_tmp
@loop:
        LDX zvm_tmp
        CPX zvm_operand_count
        BCS @false
        LDA zvm_operand_lo
        CMP zvm_operand_lo,X
        BNE @next
        LDA zvm_operand_hi
        CMP zvm_operand_hi,X
        BEQ @true
@next:
        INC zvm_tmp
        BRA @loop
@true:
        LDA #$01
        JMP zvm_branch
@false:
        LDA #$00
        JMP zvm_branch

zvm_jl:
        JSR zvm_cmp_signed
        LDA zvm_tmp
        JMP zvm_branch

zvm_jg:
        JSR zvm_cmp_signed
        LDA zvm_tmp2
        JMP zvm_branch

zvm_cmp_signed:
        STZ zvm_tmp
        STZ zvm_tmp2
        LDA zvm_operand_hi
        EOR #$80
        STA zvm_value_hi
        LDA zvm_operand_hi + 1
        EOR #$80
        CMP zvm_value_hi
        BCC @greater
        BNE @less
        LDA zvm_operand_lo
        CMP zvm_operand_lo + 1
        BCC @less
        BEQ @done
@greater:
        LDA #$01
        STA zvm_tmp2
        RTS
@less:
        LDA #$01
        STA zvm_tmp
@done:
        RTS

zvm_branch:
        STA zvm_tmp              ; actual condition, 0/1
        JSR zvm_fetch
        STA zvm_branch_off_l
        AND #$80
        BEQ :+
        LDA #$01
        BRA @want
:
        LDA #$00
@want:
        STA zvm_branch_if
        LDA zvm_branch_off_l
        AND #$40
        BEQ @long
        LDA zvm_branch_off_l
        AND #$3F
        STA zvm_branch_off_l
        STZ zvm_branch_off_h
        BRA @test
@long:
        LDA zvm_branch_off_l
        AND #$3F
        STA zvm_branch_off_h
        JSR zvm_fetch
        STA zvm_branch_off_l
        LDA zvm_branch_off_h
        AND #$20
        BEQ @test
        LDA zvm_branch_off_h
        ORA #$C0
        STA zvm_branch_off_h
@test:
        LDA zvm_tmp
        CMP zvm_branch_if
        BEQ @take
        RTS
@take:
        LDA zvm_branch_off_h
        ORA zvm_branch_off_l
        BNE @not_zero
        JMP zvm_rfalse
@not_zero:
        LDA zvm_branch_off_h
        BNE @not_one
        LDA zvm_branch_off_l
        CMP #$01
        BNE @not_one
        JMP zvm_rtrue
@not_one:
        LDA zvm_pc_l
        CLC
        ADC zvm_branch_off_l
        STA zvm_pc_l
        LDA zvm_pc_h
        ADC zvm_branch_off_h
        STA zvm_pc_h
        LDA #$00
        BIT zvm_branch_off_h
        BPL :+
        LDA #$FF
:
        ADC zvm_pc_b
        STA zvm_pc_b
        SEC
        LDA zvm_pc_l
        SBC #$02
        STA zvm_pc_l
        LDA zvm_pc_h
        SBC #$00
        STA zvm_pc_h
        LDA zvm_pc_b
        SBC #$00
        STA zvm_pc_b
        JMP zvm_check_code_target

zvm_jump:
        LDA zvm_pc_l
        CLC
        ADC zvm_operand_lo
        STA zvm_pc_l
        LDA zvm_pc_h
        ADC zvm_operand_hi
        STA zvm_pc_h
        LDA #$00
        BIT zvm_operand_hi
        BPL :+
        LDA #$FF
:
        ADC zvm_pc_b
        STA zvm_pc_b
        SEC
        LDA zvm_pc_l
        SBC #$02
        STA zvm_pc_l
        LDA zvm_pc_h
        SBC #$00
        STA zvm_pc_h
        LDA zvm_pc_b
        SBC #$00
        STA zvm_pc_b
        JMP zvm_check_code_target

zvm_load:
        LDA zvm_operand_lo
        JSR zvm_get_var_indirect
        JMP zvm_store_result

zvm_store:
        LDA zvm_operand_lo + 1
        STA zvm_value_lo
        LDA zvm_operand_hi + 1
        STA zvm_value_hi
        LDA zvm_operand_lo
        JMP zvm_set_var_indirect

zvm_inc:
        LDA zvm_operand_lo
        JSR zvm_get_var_indirect
        INC zvm_value_lo
        BNE :+
        INC zvm_value_hi
:
        LDA zvm_operand_lo
        JMP zvm_set_var_indirect

zvm_dec:
        LDA zvm_operand_lo
        JSR zvm_get_var_indirect
        LDA zvm_value_lo
        BNE :+
        DEC zvm_value_hi
:
        DEC zvm_value_lo
        LDA zvm_operand_lo
        JMP zvm_set_var_indirect

zvm_inc_chk:
        LDA zvm_operand_lo
        JSR zvm_get_var_indirect
        INC zvm_value_lo
        BNE :+
        INC zvm_value_hi
:
        LDA zvm_operand_lo
        JSR zvm_set_var_indirect
        LDA zvm_value_lo
        STA zvm_operand_lo
        LDA zvm_value_hi
        STA zvm_operand_hi
        JMP zvm_jg

zvm_dec_chk:
        LDA zvm_operand_lo
        JSR zvm_get_var_indirect
        LDA zvm_value_lo
        BNE :+
        DEC zvm_value_hi
:
        DEC zvm_value_lo
        LDA zvm_operand_lo
        JSR zvm_set_var_indirect
        LDA zvm_value_lo
        STA zvm_operand_lo
        LDA zvm_value_hi
        STA zvm_operand_hi
        JMP zvm_jl

zvm_test:
        LDA zvm_operand_lo
        AND zvm_operand_lo + 1
        CMP zvm_operand_lo + 1
        BNE @false
        LDA zvm_operand_hi
        AND zvm_operand_hi + 1
        CMP zvm_operand_hi + 1
        BNE @false
        LDA #$01
        JMP zvm_branch
@false:
        LDA #$00
        JMP zvm_branch

zvm_or:
        LDA zvm_operand_lo
        ORA zvm_operand_lo + 1
        STA zvm_value_lo
        LDA zvm_operand_hi
        ORA zvm_operand_hi + 1
        STA zvm_value_hi
        JMP zvm_store_result

zvm_and:
        LDA zvm_operand_lo
        AND zvm_operand_lo + 1
        STA zvm_value_lo
        LDA zvm_operand_hi
        AND zvm_operand_hi + 1
        STA zvm_value_hi
        JMP zvm_store_result

zvm_not:
        LDA zvm_operand_lo
        EOR #$FF
        STA zvm_value_lo
        LDA zvm_operand_hi
        EOR #$FF
        STA zvm_value_hi
        JMP zvm_store_result

zvm_add:
        CLC
        LDA zvm_operand_lo
        ADC zvm_operand_lo + 1
        STA zvm_value_lo
        LDA zvm_operand_hi
        ADC zvm_operand_hi + 1
        STA zvm_value_hi
        JMP zvm_store_result

zvm_sub:
        SEC
        LDA zvm_operand_lo
        SBC zvm_operand_lo + 1
        STA zvm_value_lo
        LDA zvm_operand_hi
        SBC zvm_operand_hi + 1
        STA zvm_value_hi
        JMP zvm_store_result

zvm_mul:
        JSR zvm_prepare_signed_operands
        STZ zvm_math_res_lo
        STZ zvm_math_res_hi
        LDA #$10
        STA zvm_math_count
@loop:
        LDA zvm_math_rhs_lo
        AND #$01
        BEQ @skip_add
        CLC
        LDA zvm_math_res_lo
        ADC zvm_math_lhs_lo
        STA zvm_math_res_lo
        LDA zvm_math_res_hi
        ADC zvm_math_lhs_hi
        STA zvm_math_res_hi
@skip_add:
        ASL zvm_math_lhs_lo
        ROL zvm_math_lhs_hi
        LSR zvm_math_rhs_hi
        ROR zvm_math_rhs_lo
        DEC zvm_math_count
        BNE @loop
        JSR zvm_apply_math_sign
        JMP zvm_store_math_result

zvm_div:
        JSR zvm_prepare_signed_operands
        LDA zvm_math_rhs_lo
        ORA zvm_math_rhs_hi
        BNE :+
        JMP zvm_unsupported
:
        JSR zvm_divide_unsigned
        JSR zvm_apply_math_sign
        JMP zvm_store_math_result

zvm_mod:
        JSR zvm_prepare_signed_operands
        LDA zvm_math_rhs_lo
        ORA zvm_math_rhs_hi
        BNE :+
        JMP zvm_unsupported
:
        JSR zvm_divide_unsigned
        LDA zvm_math_rem_lo
        STA zvm_math_res_lo
        LDA zvm_math_rem_hi
        STA zvm_math_res_hi
        LDA zvm_math_rem_sign
        STA zvm_math_sign
        JSR zvm_apply_math_sign
        JMP zvm_store_math_result

zvm_prepare_signed_operands:
        LDA zvm_operand_lo
        STA zvm_math_lhs_lo
        LDA zvm_operand_hi
        STA zvm_math_lhs_hi
        LDA zvm_operand_lo + 1
        STA zvm_math_rhs_lo
        LDA zvm_operand_hi + 1
        STA zvm_math_rhs_hi
        STZ zvm_math_sign
        STZ zvm_math_rem_sign

        LDA zvm_math_lhs_hi
        BPL @rhs
        LDA #$01
        STA zvm_math_sign
        STA zvm_math_rem_sign
        JSR zvm_negate_lhs
@rhs:
        LDA zvm_math_rhs_hi
        BPL @done
        LDA zvm_math_sign
        EOR #$01
        STA zvm_math_sign
        JSR zvm_negate_rhs
@done:
        RTS

zvm_negate_lhs:
        LDA zvm_math_lhs_lo
        EOR #$FF
        CLC
        ADC #$01
        STA zvm_math_lhs_lo
        LDA zvm_math_lhs_hi
        EOR #$FF
        ADC #$00
        STA zvm_math_lhs_hi
        RTS

zvm_negate_rhs:
        LDA zvm_math_rhs_lo
        EOR #$FF
        CLC
        ADC #$01
        STA zvm_math_rhs_lo
        LDA zvm_math_rhs_hi
        EOR #$FF
        ADC #$00
        STA zvm_math_rhs_hi
        RTS

zvm_apply_math_sign:
        LDA zvm_math_sign
        BEQ @done
        LDA zvm_math_res_lo
        EOR #$FF
        CLC
        ADC #$01
        STA zvm_math_res_lo
        LDA zvm_math_res_hi
        EOR #$FF
        ADC #$00
        STA zvm_math_res_hi
@done:
        RTS

zvm_store_math_result:
        LDA zvm_math_res_lo
        STA zvm_value_lo
        LDA zvm_math_res_hi
        STA zvm_value_hi
        JMP zvm_store_result

zvm_divide_unsigned:
        STZ zvm_math_res_lo
        STZ zvm_math_res_hi
        STZ zvm_math_rem_lo
        STZ zvm_math_rem_hi
        LDA #$10
        STA zvm_math_count
@loop:
        ASL zvm_math_lhs_lo
        ROL zvm_math_lhs_hi
        ROL zvm_math_rem_lo
        ROL zvm_math_rem_hi
        ASL zvm_math_res_lo
        ROL zvm_math_res_hi

        LDA zvm_math_rem_hi
        CMP zvm_math_rhs_hi
        BCC @next
        BNE @subtract
        LDA zvm_math_rem_lo
        CMP zvm_math_rhs_lo
        BCC @next
@subtract:
        SEC
        LDA zvm_math_rem_lo
        SBC zvm_math_rhs_lo
        STA zvm_math_rem_lo
        LDA zvm_math_rem_hi
        SBC zvm_math_rhs_hi
        STA zvm_math_rem_hi
        INC zvm_math_res_lo
@next:
        DEC zvm_math_count
        BNE @loop
        RTS

zvm_loadw:
        LDA zvm_operand_lo + 1
        ASL
        STA zvm_tmp
        LDA zvm_operand_hi + 1
        ROL
        STA zvm_tmp2
        LDA zvm_operand_lo
        CLC
        ADC zvm_tmp
        STA zstory_addr_l
        LDA zvm_operand_hi
        ADC zvm_tmp2
        STA zstory_addr_m
        STZ zstory_addr_h
        JSR zstory_read16
        LDA zstory_word_lo
        STA zvm_value_lo
        LDA zstory_word_hi
        STA zvm_value_hi
        JMP zvm_store_result

zvm_loadb:
        LDA zvm_operand_lo
        CLC
        ADC zvm_operand_lo + 1
        STA zstory_addr_l
        LDA zvm_operand_hi
        ADC zvm_operand_hi + 1
        STA zstory_addr_m
        STZ zstory_addr_h
        JSR zstory_read8
        LDA XRAM_DATA
        STA zvm_value_lo
        STZ zvm_value_hi
        JMP zvm_store_result

zvm_storew:
        LDA zvm_operand_lo + 1
        ASL
        STA zvm_tmp
        LDA zvm_operand_hi + 1
        ROL
        STA zvm_tmp2
        LDA zvm_operand_lo
        CLC
        ADC zvm_tmp
        STA zstory_addr_l
        LDA zvm_operand_hi
        ADC zvm_tmp2
        STA zstory_addr_m
        STZ zstory_addr_h
        LDA zvm_operand_hi + 2
        STA zstory_word_hi
        LDA zvm_operand_lo + 2
        STA zstory_word_lo
        JMP zstory_write16

zvm_storeb:
        LDA zvm_operand_lo
        CLC
        ADC zvm_operand_lo + 1
        STA zstory_addr_l
        LDA zvm_operand_hi
        ADC zvm_operand_hi + 1
        STA zstory_addr_m
        STZ zstory_addr_h
        LDA zvm_operand_lo + 2
        STA XRAM_DATA
        JMP zstory_write8

zvm_get_parent:
        LDA zvm_operand_lo
        JSR zobject_get_parent
        JMP zvm_store_object_value

zvm_get_sibling:
        LDA zvm_operand_lo
        JSR zobject_get_sibling
        JSR zvm_store_object_value
        LDA zvm_value_lo
        ORA zvm_value_hi
        BEQ @false
        LDA #$01
        JMP zvm_branch
@false:
        LDA #$00
        JMP zvm_branch

zvm_get_child:
        LDA zvm_operand_lo
        JSR zobject_get_child
        JSR zvm_store_object_value
        LDA zvm_value_lo
        ORA zvm_value_hi
        BEQ @false
        LDA #$01
        JMP zvm_branch
@false:
        LDA #$00
        JMP zvm_branch

zvm_store_object_value:
        LDA zobject_value_lo
        STA zvm_value_lo
        LDA zobject_value_hi
        STA zvm_value_hi
        JMP zvm_store_result

zvm_jin:
        LDA zvm_operand_lo
        JSR zobject_get_parent
        LDA zobject_value_lo
        CMP zvm_operand_lo + 1
        BEQ @true
        LDA #$00
        JMP zvm_branch
@true:
        LDA #$01
        JMP zvm_branch

zvm_test_attr:
        LDA zvm_operand_lo
        STA zobject_num
        LDA zvm_operand_lo + 1
        STA zobject_attr_num
        JSR zobject_test_attr
        JMP zvm_branch

zvm_set_attr:
        LDA zvm_operand_lo
        STA zobject_num
        LDA zvm_operand_lo + 1
        STA zobject_attr_num
        JMP zobject_set_attr

zvm_clear_attr:
        LDA zvm_operand_lo
        STA zobject_num
        LDA zvm_operand_lo + 1
        STA zobject_attr_num
        JMP zobject_clear_attr

zvm_remove_obj:
        LDA zvm_operand_lo
        STA zobject_num
        JMP zobject_remove

zvm_insert_obj:
        LDA zvm_operand_lo
        STA zobject_num
        LDA zvm_operand_lo + 1
        STA zobject_value_lo
        JMP zobject_insert

zvm_get_prop:
        LDA zvm_operand_lo
        STA zobject_num
        LDA zvm_operand_lo + 1
        STA zobject_prop_num
        JSR zobject_get_prop
        LDA zobject_value_lo
        STA zvm_value_lo
        LDA zobject_value_hi
        STA zvm_value_hi
        JMP zvm_store_result

zvm_get_prop_addr:
        LDA zvm_operand_lo
        STA zobject_num
        LDA zvm_operand_lo + 1
        STA zobject_prop_num
        JSR zobject_get_prop_addr
        LDA zobject_value_lo
        STA zvm_value_lo
        LDA zobject_value_hi
        STA zvm_value_hi
        JMP zvm_store_result

zvm_get_next_prop:
        LDA zvm_operand_lo
        STA zobject_num
        LDA zvm_operand_lo + 1
        STA zobject_prop_num
        JSR zobject_get_next_prop
        LDA zobject_value_lo
        STA zvm_value_lo
        LDA zobject_value_hi
        STA zvm_value_hi
        JMP zvm_store_result

zvm_get_prop_len:
        LDA zvm_operand_lo
        STA zobject_value_lo
        LDA zvm_operand_hi
        STA zobject_value_hi
        JSR zobject_get_prop_len
        LDA zobject_value_lo
        STA zvm_value_lo
        LDA zobject_value_hi
        STA zvm_value_hi
        JMP zvm_store_result

zvm_put_prop:
        LDA zvm_operand_lo
        STA zobject_num
        LDA zvm_operand_lo + 1
        STA zobject_prop_num
        JSR zobject_get_prop_addr
        LDA zobject_value_lo
        STA zstory_addr_l
        LDA zobject_value_hi
        STA zstory_addr_m
        STZ zstory_addr_h
        JSR zobject_get_prop_len
        LDA zobject_value_lo
        CMP #$01
        BNE @word
        LDA zvm_operand_lo + 2
        STA XRAM_DATA
        JMP zstory_write8
@word:
        LDA zvm_operand_hi + 2
        STA zstory_word_hi
        LDA zvm_operand_lo + 2
        STA zstory_word_lo
        JMP zstory_write16

zvm_push:
        LDA zvm_operand_lo
        STA zvm_value_lo
        LDA zvm_operand_hi
        STA zvm_value_hi
        JMP zvm_stack_push

zvm_pull:
        JSR zvm_stack_pop
        LDA zvm_operand_lo
        JMP zvm_set_var_indirect

zvm_random:
        LDA zvm_operand_lo
        ORA zvm_operand_hi
        BEQ @zero
        LDA zvm_operand_hi
        BMI @seed
        JSR zvm_next_random
        LDA zvm_rng_hi
        STA zvm_math_lhs_hi
        LDA zvm_rng_lo
        STA zvm_math_lhs_lo
        LDA zvm_operand_hi
        STA zvm_math_rhs_hi
        LDA zvm_operand_lo
        STA zvm_math_rhs_lo
        JSR zvm_divide_unsigned
        LDA zvm_math_rem_lo
        CLC
        ADC #$01
        STA zvm_value_lo
        STZ zvm_value_hi
        BRA @store
@seed:
        LDA zvm_operand_lo
        STA zvm_rng_lo
        LDA zvm_operand_hi
        STA zvm_rng_hi
        STZ zvm_value_lo
        STZ zvm_value_hi
        BRA @store
@zero:
        LDA #$4D
        STA zvm_rng_lo
        LDA #$21
        STA zvm_rng_hi
        STZ zvm_value_lo
        STZ zvm_value_hi
@store:
        JMP zvm_store_result

zvm_next_random:
        LDA zvm_rng_lo
        ASL
        TAX
        LDA zvm_rng_hi
        ROL
        STA zvm_rng_hi
        STX zvm_rng_lo
        BCC @done
        LDA zvm_rng_lo
        EOR #$39
        STA zvm_rng_lo
        LDA zvm_rng_hi
        EOR #$30
        STA zvm_rng_hi
@done:
        RTS

zvm_noop:
        RTS

zvm_false_store:
        STZ zvm_value_lo
        STZ zvm_value_hi
        JMP zvm_store_result

zvm_true_branch:
        LDA #$01
        JMP zvm_branch

zvm_read:
        JSR zvm_show_status
        LDA zvm_operand_lo
        STA zvm_text_lo
        LDA zvm_operand_hi
        STA zvm_text_hi
        LDA zvm_operand_lo + 1
        STA zvm_parse_lo
        LDA zvm_operand_hi + 1
        STA zvm_parse_hi

        STZ zvm_input_len
        JSR zvm_read_text_max
@key_loop:
        LDA VGC_CHARIN
        BEQ @key_loop
        CMP #$0A
        BEQ @done
        CMP #$0D
        BEQ @done
        CMP #$08
        BEQ @backspace
        CMP #$7F
        BEQ @backspace
        CMP #$20
        BCC @key_loop
        CMP #$7F
        BCS @key_loop
        LDX zvm_input_len
        CPX zvm_input_max
        BCS @key_loop
        JSR zvm_to_lower
        PHA
        JSR print_char
        PLA
        JSR zvm_store_input_char
        INC zvm_input_len
        BRA @key_loop
@backspace:
        LDA zvm_input_len
        BEQ @key_loop
        DEC zvm_input_len
        LDA #$08
        JSR print_char
        LDA #' '
        JSR print_char
        LDA #$08
        JSR print_char
        BRA @key_loop
@done:
        LDA #$00
        JSR zvm_store_input_terminator
        JSR newline
        LDA zvm_parse_lo
        ORA zvm_parse_hi
        BEQ @return
        JSR zvm_tokenize_input
@return:
        RTS

zvm_read_text_max:
        LDA zvm_text_lo
        STA zstory_addr_l
        LDA zvm_text_hi
        STA zstory_addr_m
        STZ zstory_addr_h
        JSR zstory_read8
        LDA XRAM_DATA
        STA zvm_input_max
        RTS

zvm_store_input_char:
        PHA
        LDA zvm_input_len
        INC
        JSR zvm_set_text_addr_offset
        PLA
        STA XRAM_DATA
        JMP zstory_write8

zvm_store_input_terminator:
        PHA
        LDA zvm_input_len
        INC
        JSR zvm_set_text_addr_offset
        PLA
        STA XRAM_DATA
        JMP zstory_write8

zvm_to_lower:
        CMP #'A'
        BCC @done
        CMP #'Z' + 1
        BCS @done
        ORA #$20
@done:
        RTS

zvm_read_text_byte_offset:
        CLC
        ADC zvm_text_lo
        STA zstory_addr_l
        LDA zvm_text_hi
        ADC #$00
        STA zstory_addr_m
        STZ zstory_addr_h
        JSR zstory_read8
        LDA XRAM_DATA
        RTS

zvm_set_text_addr_offset:
        CLC
        ADC zvm_text_lo
        STA zstory_addr_l
        LDA zvm_text_hi
        ADC #$00
        STA zstory_addr_m
        STZ zstory_addr_h
        RTS

zvm_set_parse_addr_offset:
        CLC
        ADC zvm_parse_lo
        STA zstory_addr_l
        LDA zvm_parse_hi
        ADC #$00
        STA zstory_addr_m
        STZ zstory_addr_h
        RTS

zvm_tokenize_input:
        JSR zvm_prepare_dictionary
        STZ zvm_parse_count
        LDA #$00
        JSR zvm_set_parse_addr_offset
        JSR zstory_read8
        LDA XRAM_DATA
        STA zvm_parse_max
        LDA #$00
        JSR zvm_write_parse_count
        LDA #$01
        STA zvm_input_pos
@scan:
        LDA zvm_input_pos
        CMP zvm_input_len
        BEQ @maybe_last
        BCC @read_char
        JMP @done
@maybe_last:
        LDA zvm_input_len
        BEQ @done
@read_char:
        LDA zvm_input_pos
        JSR zvm_read_input_at
        CMP #' '
        BNE @not_space
        INC zvm_input_pos
        BRA @scan
@not_space:
        JSR zvm_is_separator
        BEQ @word
        LDA zvm_input_pos
        STA zvm_token_start
        LDA #$01
        STA zvm_token_len
        JSR zvm_emit_parse_token
        INC zvm_input_pos
        BRA @scan
@word:
        LDA zvm_input_pos
        STA zvm_token_start
        STZ zvm_token_len
@word_loop:
        LDA zvm_input_pos
        CMP zvm_input_len
        BEQ @word_char
        BCS @word_done
@word_char:
        LDA zvm_input_pos
        JSR zvm_read_input_at
        CMP #' '
        BEQ @word_done
        JSR zvm_is_separator
        BNE @word_done
        INC zvm_token_len
        INC zvm_input_pos
        BRA @word_loop
@word_done:
        LDA zvm_token_len
        BEQ @scan
        JSR zvm_emit_parse_token
        BRA @scan
@done:
        LDA zvm_parse_count
        JMP zvm_write_parse_count

zvm_read_input_at = zvm_read_text_byte_offset

zvm_write_parse_count:
        PHA
        LDA #$01
        JSR zvm_set_parse_addr_offset
        PLA
        STA XRAM_DATA
        JMP zstory_write8

zvm_emit_parse_token:
        LDA zvm_parse_count
        CMP zvm_parse_max
        BCS @done
        JSR zvm_encode_token
        JSR zvm_lookup_token
        JSR zvm_write_parse_entry
        INC zvm_parse_count
@done:
        RTS

zvm_write_parse_entry:
        LDA zvm_parse_count
        ASL
        ASL
        CLC
        ADC #$02
        STA zvm_tmp
        JSR zvm_set_parse_addr_offset
        LDA zvm_dict_found_hi
        STA XRAM_DATA
        JSR zstory_write8
        INC zvm_tmp
        LDA zvm_tmp
        JSR zvm_set_parse_addr_offset
        LDA zvm_dict_found_lo
        STA XRAM_DATA
        JSR zstory_write8
        INC zvm_tmp
        LDA zvm_tmp
        JSR zvm_set_parse_addr_offset
        LDA zvm_token_len
        STA XRAM_DATA
        JSR zstory_write8
        INC zvm_tmp
        LDA zvm_tmp
        JSR zvm_set_parse_addr_offset
        LDA zvm_token_start
        STA XRAM_DATA
        JMP zstory_write8

zvm_prepare_dictionary:
        LDA zstory_dictionary_lo
        STA zvm_dict_lo
        LDA zstory_dictionary_hi
        STA zvm_dict_hi
        LDA #$00
        JSR zvm_set_dict_addr_offset
        JSR zstory_read8
        LDA XRAM_DATA
        STA zvm_dict_sep_count
        INC
        JSR zvm_set_dict_addr_offset
        JSR zstory_read8
        LDA XRAM_DATA
        STA zvm_dict_entry_len
        LDA zvm_dict_sep_count
        CLC
        ADC #$02
        JSR zvm_set_dict_addr_offset
        JSR zstory_read8
        LDA XRAM_DATA
        STA zvm_dict_count_hi
        LDA zvm_dict_sep_count
        CLC
        ADC #$03
        JSR zvm_set_dict_addr_offset
        JSR zstory_read8
        LDA XRAM_DATA
        STA zvm_dict_count_lo
        LDA zvm_dict_sep_count
        CLC
        ADC #$04
        CLC
        ADC zvm_dict_lo
        STA zvm_dict_entry_lo
        LDA zvm_dict_hi
        ADC #$00
        STA zvm_dict_entry_hi
        RTS

zvm_set_dict_addr_offset:
        CLC
        ADC zvm_dict_lo
        STA zstory_addr_l
        LDA zvm_dict_hi
        ADC #$00
        STA zstory_addr_m
        STZ zstory_addr_h
        RTS

; Returns Z=0 when A is a dictionary word separator.
zvm_is_separator:
        STA zvm_char_tmp
        LDA zvm_dict_sep_count
        BEQ @false
        STA zvm_tmp
        LDA #$01
        STA zvm_tmp2
@loop:
        LDA zvm_tmp2
        JSR zvm_set_dict_addr_offset
        JSR zstory_read8
        LDA XRAM_DATA
        CMP zvm_char_tmp
        BEQ @true
        INC zvm_tmp2
        DEC zvm_tmp
        BNE @loop
@false:
        LDA #$00
        RTS
@true:
        LDA #$01
        RTS

zvm_encode_token:
        LDX #$00
        LDA #$05
@pad:
        STA zvm_encode_zchars,X
        INX
        CPX #$06
        BCC @pad
        STZ zvm_encode_slot
        STZ zvm_tmp
@loop:
        LDA zvm_tmp
        CMP zvm_token_len
        BCS @pack
        LDA zvm_encode_slot
        CMP #$06
        BCS @pack
        LDA zvm_token_start
        CLC
        ADC zvm_tmp
        JSR zvm_read_input_at
        JSR zvm_to_lower
        JSR zvm_encode_char
        INC zvm_tmp
        BRA @loop
@pack:
        LDA zvm_encode_zchars
        ASL
        ASL
        STA zvm_encode_bytes
        LDA zvm_encode_zchars + 1
        LSR
        LSR
        LSR
        ORA zvm_encode_bytes
        STA zvm_encode_bytes
        LDA zvm_encode_zchars + 1
        AND #$07
        ASL
        ASL
        ASL
        ASL
        ASL
        ORA zvm_encode_zchars + 2
        STA zvm_encode_bytes + 1

        LDA zvm_encode_zchars + 3
        ASL
        ASL
        ORA #$80
        STA zvm_encode_bytes + 2
        LDA zvm_encode_zchars + 4
        LSR
        LSR
        LSR
        ORA zvm_encode_bytes + 2
        STA zvm_encode_bytes + 2
        LDA zvm_encode_zchars + 4
        AND #$07
        ASL
        ASL
        ASL
        ASL
        ASL
        ORA zvm_encode_zchars + 5
        STA zvm_encode_bytes + 3
        RTS

zvm_encode_char:
        CMP #'a'
        BCC @special
        CMP #'z' + 1
        BCS @special
        SEC
        SBC #'a' - 6
        JMP zvm_emit_encoded_zchar
@special:
        CMP #'0'
        BCC @punct
        CMP #'9' + 1
        BCS @punct
        PHA
        LDA #$05
        JSR zvm_emit_encoded_zchar
        PLA
        SEC
        SBC #'0' - 8
        JMP zvm_emit_encoded_zchar
@punct:
        LDX #$00
@punct_loop:
        CMP zvm_a2_punct_chars,X
        BEQ @punct_found
        INX
        CPX #zvm_a2_punct_count
        BCC @punct_loop
        RTS
@punct_found:
        PHA
        LDA #$05
        JSR zvm_emit_encoded_zchar
        PLA
        TXA
        CLC
        ADC #18
        JMP zvm_emit_encoded_zchar

zvm_emit_encoded_zchar:
        LDX zvm_encode_slot
        CPX #$06
        BCS @done
        STA zvm_encode_zchars,X
        INC zvm_encode_slot
@done:
        RTS

zvm_lookup_token:
        STZ zvm_dict_found_lo
        STZ zvm_dict_found_hi
        LDA zvm_dict_entry_lo
        STA zvm_dict_scan_lo
        LDA zvm_dict_entry_hi
        STA zvm_dict_scan_hi
        LDA zvm_dict_count_lo
        STA zvm_math_lhs_lo
        LDA zvm_dict_count_hi
        STA zvm_math_lhs_hi
@loop:
        LDA zvm_math_lhs_lo
        ORA zvm_math_lhs_hi
        BEQ @done
        JSR zvm_compare_dict_entry
        BEQ @found
        JSR zvm_advance_dict_entry
        LDA zvm_math_lhs_lo
        BNE :+
        DEC zvm_math_lhs_hi
:
        DEC zvm_math_lhs_lo
        BRA @loop
@found:
        LDA zvm_dict_scan_lo
        STA zvm_dict_found_lo
        LDA zvm_dict_scan_hi
        STA zvm_dict_found_hi
@done:
        RTS

zvm_compare_dict_entry:
        STZ zvm_tmp
@loop:
        LDA zvm_dict_scan_lo
        CLC
        ADC zvm_tmp
        STA zstory_addr_l
        LDA zvm_dict_scan_hi
        ADC #$00
        STA zstory_addr_m
        STZ zstory_addr_h
        JSR zstory_read8
        LDX zvm_tmp
        LDA XRAM_DATA
        CMP zvm_encode_bytes,X
        BNE @no
        INC zvm_tmp
        LDA zvm_tmp
        CMP #$04
        BCC @loop
        LDA #$00
        RTS
@no:
        LDA #$01
        RTS

zvm_advance_dict_entry:
        LDA zvm_dict_scan_lo
        CLC
        ADC zvm_dict_entry_len
        STA zvm_dict_scan_lo
        LDA zvm_dict_scan_hi
        ADC #$00
        STA zvm_dict_scan_hi
        RTS

zvm_show_status:
        LDA VTEXT_CURX
        STA zvm_status_x
        LDA VTEXT_CURY
        STA zvm_status_y
        STZ VTEXT_LEFT
        STZ VTEXT_TOP
        LDA #80
        STA VTEXT_WIDTH
        LDA #1
        STA VTEXT_HEIGHT
        STZ VTEXT_CURX
        STZ VTEXT_CURY
        LDA #$F0
        STA VTEXT_COLOR
        STZ VTEXT_ATTR
        STZ VTEXT_FLAGS
        JSR vtext_clear_region
        JSR vtext_home

        LDA #$10
        JSR zvm_get_var
        LDA zvm_value_lo
        JSR zobject_print_short_name

        LDA #55
        STA VTEXT_CURX
        STZ VTEXT_CURY
        JSR vtext_set_cursor
        LDA #<msg_score
        LDY #>msg_score
        JSR print_text
        LDA #$11
        JSR zvm_get_var
        LDA zvm_value_hi
        LDX zvm_value_lo
        JSR print_u16_dec
        LDA #<msg_moves
        LDY #>msg_moves
        JSR print_text
        LDA #$12
        JSR zvm_get_var
        LDA zvm_value_hi
        LDX zvm_value_lo
        JSR print_u16_dec

        STZ VTEXT_LEFT
        LDA #1
        STA VTEXT_TOP
        LDA #80
        STA VTEXT_WIDTH
        LDA #49
        STA VTEXT_HEIGHT
        LDA zvm_status_x
        STA VTEXT_CURX
        LDA zvm_status_y
        STA VTEXT_CURY
        LDA #$0F
        STA VTEXT_COLOR
        STZ VTEXT_ATTR
        LDA #(VTEXT_FLAG_WRAP | VTEXT_FLAG_SCROLL)
        STA VTEXT_FLAGS
        JMP vtext_set_cursor

zvm_split_window:
        RTS

zvm_set_window:
        RTS

zvm_set_text_style:
        LDA zvm_operand_lo
        BEQ @roman
        AND #$01
        BEQ @done
        LDA #$F0
        STA VTEXT_COLOR
        RTS
@roman:
        LDA #$0F
        STA VTEXT_COLOR
@done:
        RTS

zvm_buffer_mode:
        RTS

zvm_output_stream:
        LDA zvm_operand_hi
        BEQ @positive
        CMP #$FF
        BNE @done
        LDA zvm_operand_lo
        CMP #$FF
        BEQ @screen_off
        CMP #$FE
        BEQ @done
        CMP #$FD
        BEQ @stream3_off
        BRA @done

@positive:
        LDA zvm_operand_lo
        CMP #$01
        BEQ @screen_on
        CMP #$02
        BEQ @done
        CMP #$03
        BEQ @stream3_on
        BRA @done

@screen_on:
        LDA zvm_stream_flags
        AND #($FF - ZVM_STREAM_SCREEN_OFF)
        STA zvm_stream_flags
        RTS

@screen_off:
        LDA zvm_stream_flags
        ORA #ZVM_STREAM_SCREEN_OFF
        STA zvm_stream_flags
        RTS

@stream3_on:
        LDA zvm_operand_lo + 1
        STA zvm_stream3_lo
        LDA zvm_operand_hi + 1
        STA zvm_stream3_hi
        STZ zvm_stream3_len_lo
        STZ zvm_stream3_len_hi
        JSR zvm_stream3_write_count
        LDA zvm_stream_flags
        ORA #ZVM_STREAM_3_ACTIVE
        STA zvm_stream_flags
        RTS

@stream3_off:
        JSR zvm_stream3_write_count
        LDA zvm_stream_flags
        AND #($FF - ZVM_STREAM_3_ACTIVE)
        STA zvm_stream_flags
@done:
        RTS

zvm_input_stream:
        RTS

zvm_sound_effect:
        RTS

zvm_stream3_put_char:
        CMP #$0A
        BEQ @done
        STA zvm_char_tmp
        LDA zstory_addr_l
        PHA
        LDA zstory_addr_m
        PHA
        LDA zstory_addr_h
        PHA

        LDA zvm_stream3_len_lo
        CLC
        ADC #$02
        STA zvm_tmp
        LDA zvm_stream3_len_hi
        ADC #$00
        STA zvm_tmp2
        JSR zvm_set_stream3_addr_offset
        LDA zvm_char_tmp
        STA XRAM_DATA
        JSR zstory_write8
        INC zvm_stream3_len_lo
        BNE @write_count
        INC zvm_stream3_len_hi
@write_count:
        JSR zvm_stream3_write_count

        PLA
        STA zstory_addr_h
        PLA
        STA zstory_addr_m
        PLA
        STA zstory_addr_l
@done:
        RTS

zvm_stream3_write_count:
        LDA zstory_addr_l
        PHA
        LDA zstory_addr_m
        PHA
        LDA zstory_addr_h
        PHA

        LDA zvm_stream3_lo
        STA zstory_addr_l
        LDA zvm_stream3_hi
        STA zstory_addr_m
        STZ zstory_addr_h
        LDA zvm_stream3_len_hi
        STA zstory_word_hi
        LDA zvm_stream3_len_lo
        STA zstory_word_lo
        JSR zstory_write16

        PLA
        STA zstory_addr_h
        PLA
        STA zstory_addr_m
        PLA
        STA zstory_addr_l
        RTS

zvm_set_stream3_addr_offset:
        LDA zvm_stream3_lo
        CLC
        ADC zvm_tmp
        STA zstory_addr_l
        LDA zvm_stream3_hi
        ADC zvm_tmp2
        STA zstory_addr_m
        STZ zstory_addr_h
        RTS

zvm_check_code_target:
        LDA zvm_pc_b
        BNE @ok
        LDA zvm_pc_h
        CMP zstory_highmem_hi
        BCC zvm_bad_target
        BNE @ok
        LDA zvm_pc_l
        CMP zstory_highmem_lo
        BCC zvm_bad_target
@ok:
        RTS

zvm_bad_target:
        LDA #<msg_bad_target
        LDY #>msg_bad_target
        JSR print_text
        LDA zvm_opcode_pc_h
        LDX zvm_opcode_pc_l
        JSR print_hex16
        LDA #'-'
        JSR print_char
        LDA #'>'
        JSR print_char
        LDA #'$'
        JSR print_char
        LDA zvm_pc_b
        BEQ :+
        JSR print_hex8
:
        LDA zvm_pc_h
        LDX zvm_pc_l
        JSR print_hex16
        JSR newline
        LDA #ZVM_STOP_UNSUPPORTED
        STA zvm_stop_reason
        RTS

zvm_unsupported:
        LDA #<msg_unsupported_opcode
        LDY #>msg_unsupported_opcode
        JSR print_text
        LDA zvm_opcode
        JSR print_hex8
        LDA #' '
        JSR print_char
        LDA #'$'
        JSR print_char
        LDA zvm_opcode_pc_h
        LDX zvm_opcode_pc_l
        JSR print_hex16
        JSR newline
        LDA #ZVM_STOP_UNSUPPORTED
        STA zvm_stop_reason
        RTS

.segment "RODATA"

zvm_0op_table:
        .word zvm_rtrue
        .word zvm_rfalse
        .word zvm_print
        .word zvm_print_ret
        .word zvm_nop
        .word zvm_save
        .word zvm_restore
        .word zvm_restart
        .word zvm_ret_popped
        .word zvm_pop
        .word zvm_quit
        .word zvm_new_line
        .word zvm_show_status
        .word zvm_verify
zvm_0op_count = (* - zvm_0op_table) / 2

zvm_1op_table:
        .word zvm_jz
        .word zvm_get_sibling
        .word zvm_get_child
        .word zvm_get_parent
        .word zvm_get_prop_len
        .word zvm_inc
        .word zvm_dec
        .word zvm_print_addr
        .word zvm_call_1s
        .word zvm_remove_obj
        .word zvm_print_obj
        .word zvm_ret
        .word zvm_jump
        .word zvm_print_paddr
        .word zvm_load
        .word zvm_not
zvm_1op_count = (* - zvm_1op_table) / 2

zvm_2op_table:
        .word zvm_unsupported
        .word zvm_je
        .word zvm_jl
        .word zvm_jg
        .word zvm_dec_chk
        .word zvm_inc_chk
        .word zvm_jin
        .word zvm_test
        .word zvm_or
        .word zvm_and
        .word zvm_test_attr
        .word zvm_set_attr
        .word zvm_clear_attr
        .word zvm_store
        .word zvm_insert_obj
        .word zvm_loadw
        .word zvm_loadb
        .word zvm_get_prop
        .word zvm_get_prop_addr
        .word zvm_get_next_prop
        .word zvm_add
        .word zvm_sub
        .word zvm_mul
        .word zvm_div
        .word zvm_mod
        .word zvm_unsupported
        .word zvm_unsupported
        .word zvm_unsupported
zvm_2op_count = (* - zvm_2op_table) / 2

zvm_var_table:
        .word zvm_call_vs
        .word zvm_storew
        .word zvm_storeb
        .word zvm_put_prop
        .word zvm_read
        .word zvm_print_char
        .word zvm_print_num
        .word zvm_random
        .word zvm_push
        .word zvm_pull
        .word zvm_split_window
        .word zvm_set_window
        .word zvm_unsupported
        .word zvm_unsupported
        .word zvm_unsupported
        .word zvm_set_text_style
        .word zvm_buffer_mode
        .word zvm_output_stream
        .word zvm_input_stream
        .word zvm_sound_effect
zvm_var_count = (* - zvm_var_table) / 2

msg_score:
        .byte "Score: ", 0
msg_moves:
        .byte " Moves: ", 0
msg_unsupported_opcode:
        .byte "UNSUPPORTED Z-OPCODE $", 0
msg_bad_target:
        .byte "BAD Z-PC $", 0
zvm_a2_punct_chars:
        .byte ".,!?_#'", $22, "/", $5C, "-:()"
zvm_a2_punct_count = * - zvm_a2_punct_chars

.endif

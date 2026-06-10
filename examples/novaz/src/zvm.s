; NovaZ minimal Z3 interpreter.
;
; Executes real Z-code until the first read/sread opcode. Input parsing is not
; implemented yet; this milestone is enough to let the story print its banner,
; initial room text, prompt, and status line.

.include "zstory.inc"
.include "ztext.inc"
.include "zobject.inc"
.include "zvm.inc"
.include "zvm6.inc"
.include "vtext.inc"
.include "xram.inc"

.ifndef NOVA_NOVAZ_ZVM_IMPLEMENTATION_INCLUDED
NOVA_NOVAZ_ZVM_IMPLEMENTATION_INCLUDED = 1

; Nova Timer Controller, used as the time base for V4+ timed keyboard input.
; Polled (not IRQ-driven): TimerStatus bit 0 is set once per programmed period
; and cleared on read. The period is counted in 100-cycle ticks.
.ifndef TimerCtrl
TimerCtrl   = $BA40     ; bit 0 = enable
TimerStatus = $BA41     ; read: bit 0 = period elapsed (clears on read)
TimerDivL   = $BA42     ; period divisor low (ticks)
TimerDivH   = $BA43     ; period divisor high
.endif

; 0.1 s at the 12 MHz Nova CPU clock = 1,200,000 cycles = 12,000 ticks ($2EE0).
ZVM_TENTH_DIV_LO = $E0
ZVM_TENTH_DIV_HI = $2E

; sound_effect bleeps (number 1 = high, 2 = low) are short one-shots played
; through the existing FIO_CMD_SOUND path (MIDI note / duration-in-frames /
; instrument slot). Sampled sounds (number >= 3) go through FIO_CMD_ZSOUND.
NZ_BLEEP_HI_NOTE = 84          ; C6
NZ_BLEEP_LO_NOTE = 60          ; C4
NZ_BLEEP_DURATION = 10         ; ~1/6 s at 60 Hz
NZ_BLEEP_INSTR    = 0          ; default SID instrument slot

ZVM_SAVE_HEADER_SIZE       = $40
ZVM_SAVE_PREFIX_SIZE       = $08
ZVM_SAVE_FORMAT_VERSION    = $02
ZVM_SAVE_STATE_ADDRL       = ZVM_SAVE_HEADER_SIZE
ZVM_SAVE_STATE_ADDRM       = $00
ZVM_COLOR_NORMAL           = $0C
ZVM_COLOR_BOLD             = $0F
ZVM_COLOR_REVERSE_NORMAL   = $C0
ZVM_COLOR_REVERSE_BOLD     = $F0

ZVM_SAVE_MAGIC0            = $00
ZVM_SAVE_MAGIC1            = $01
ZVM_SAVE_MAGIC2            = $02
ZVM_SAVE_MAGIC3            = $03
ZVM_SAVE_FORMAT            = $04
ZVM_SAVE_HDR_SIZE          = $05
ZVM_SAVE_STATE_SIZE_LO     = $06
ZVM_SAVE_STATE_SIZE_HI     = $07
ZVM_SAVE_DYNAMIC_SIZE_LO   = $08
ZVM_SAVE_DYNAMIC_SIZE_HI   = $09
ZVM_SAVE_STORY_VERSION     = $0A
ZVM_SAVE_RELEASE_HI        = $0B
ZVM_SAVE_RELEASE_LO        = $0C
ZVM_SAVE_SERIAL            = $0D
ZVM_SAVE_CHECKSUM_HI       = $13
ZVM_SAVE_CHECKSUM_LO       = $14
ZVM_SAVE_BRANCH_IF         = $15
ZVM_SAVE_BRANCH_OFF_H      = $16
ZVM_SAVE_BRANCH_OFF_L      = $17
ZVM_SAVE_PC_B              = $18
ZVM_SAVE_PC_H              = $19
ZVM_SAVE_PC_L              = $1A
ZVM_SAVE_RETURN_KIND       = $1B
ZVM_SAVE_STORE_VAR         = $1C
ZVM_SAVE_RETURN_BRANCH     = $00
ZVM_SAVE_RETURN_STORE      = $01

.segment "ZEROPAGE"

zvm_pc_l:             .res 1
zvm_pc_h:             .res 1
zvm_opcode_pc_l:      .res 1
zvm_opcode_pc_h:      .res 1
zvm_opcode:           .res 1
zvm_form:             .res 1
zvm_opnum:            .res 1
zvm_type_byte:        .res 1
zvm_type_byte2:       .res 1
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
zvm_status_color:     .res 1
zvm_status_attr:      .res 1
zvm_jump_l:           .res 1
zvm_jump_h:           .res 1
zvm_pc_b:             .res 1
zvm_opcode_pc_b:      .res 1

.segment "BSS"

zvm_state_start:
zvm_stop_reason:      .res 1
zvm_sp:               .res 1
zvm_frame_count:      .res 1
zvm_arg_count:        .res 1
zvm_operand_lo:       .res 8
zvm_operand_hi:       .res 8
zvm_locals_lo:        .res ZVM_LOCAL_MAX
zvm_locals_hi:        .res ZVM_LOCAL_MAX
zvm_stack_lo:         .res ZVM_STACK_MAX
zvm_stack_hi:         .res ZVM_STACK_MAX
zvm_frame_ret_lo:     .res ZVM_FRAME_MAX
zvm_frame_ret_hi:     .res ZVM_FRAME_MAX
zvm_frame_ret_bank:   .res ZVM_FRAME_MAX
zvm_frame_store:      .res ZVM_FRAME_MAX
zvm_frame_sp:         .res ZVM_FRAME_MAX
zvm_frame_arg_count:  .res ZVM_FRAME_MAX
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
zvm_text_buffering:   .res 1
zvm_text_style:       .res 1
zvm_timed_active:     .res 1
zvm_timed_limit:      .res 1
zvm_timed_count:      .res 1
zvm_timed_routine_lo: .res 1
zvm_timed_routine_hi: .res 1
zvm_timed_abort:      .res 1
zvm_timed_depth:      .res 1
zvm_sound_active:     .res 1   ; nonzero when a sampled sound with a V5 finish
                                ; routine is in flight and must be polled in read
zvm_sound_routine_lo: .res 1   ; packed address of the finish routine (0 = none)
zvm_sound_routine_hi: .res 1
zvm_window_current:   .res 1
zvm_split_lines:      .res 1
zvm_lower_x:          .res 1
zvm_lower_y:          .res 1
zvm_upper_x:          .res 1
zvm_upper_y:          .res 1
zvm_text_lo:          .res 1
zvm_text_hi:          .res 1
zvm_parse_lo:         .res 1
zvm_parse_hi:         .res 1
zvm_input_max:        .res 1
zvm_input_len:        .res 1
zvm_read_terminator:  .res 1
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
zvm_encode_limit:     .res 1
zvm_encode_byte_count:.res 1
zvm_encode_zchars:    .res 9
zvm_encode_bytes:     .res 6
zvm_char_tmp:         .res 1
zvm_stream_flags:     .res 1
zvm_stream3_lo:       .res 1
zvm_stream3_hi:       .res 1
zvm_stream3_len_lo:   .res 1
zvm_stream3_len_hi:   .res 1
zvm_verify_end_l:     .res 1
zvm_verify_end_m:     .res 1
zvm_verify_end_h:     .res 1
zvm_verify_sum_lo:    .res 1
zvm_verify_sum_hi:    .res 1
zvm_mem_src_lo:       .res 1
zvm_mem_src_hi:       .res 1
zvm_mem_dst_lo:       .res 1
zvm_mem_dst_hi:       .res 1
zvm_mem_count_lo:     .res 1
zvm_mem_count_hi:     .res 1
zvm_mem_stride:       .res 1
zvm_table_cols:       .res 1
zvm_table_rows:       .res 1
zvm_table_skip:       .res 1
zvm_save_return_kind: .res 1
zvm_save_store_var:   .res 1
zvm_state_end:

ZVM_SAVE_STATE_SIZE = zvm_state_end - zvm_state_start
ZVM_SAVE_TOTAL_SIZE = ZVM_SAVE_HEADER_SIZE + ZVM_SAVE_STATE_SIZE

zvm_save_header:      .res ZVM_SAVE_HEADER_SIZE
zvm_save_scratch_base_l: .res 1
zvm_save_scratch_base_m: .res 1
zvm_save_scratch_base_h: .res 1

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
.export zvm_verify
.export zvm_verify_end_l
.export zvm_verify_end_m
.export zvm_verify_end_h
.export zvm_verify_sum_lo
.export zvm_verify_sum_hi
.export zvm_read_key_loop
.export zvm_read_timed_poll

zvm_run_until_read:
        STZ zvm_stop_reason
        STZ zvm_sp
        STZ zvm_frame_count
        STZ zvm_arg_count
        STZ zvm_stream_flags
        STZ zvm_window_current
        STZ zvm_split_lines
        STZ zvm_lower_x
        STZ zvm_lower_y
        STZ zvm_upper_x
        STZ zvm_upper_y
        LDA #$01
        STA zvm_text_buffering
        STZ zvm_text_style
        STZ zvm_timed_active
        STZ zvm_timed_abort
        STZ zvm_sound_active
        STZ zvm_sound_routine_lo
        STZ zvm_sound_routine_hi
        LDA #$4D
        STA zvm_rng_lo
        LDA #$21
        STA zvm_rng_hi
        JSR zvm_clear_locals
        JSR zvm_select_active_window
        LDA zstory_version
        CMP #$06
        BNE @direct_pc
        ; V6: reset the segment's 8-window model. Fresh boots AND restarts
        ; both come through here, and the segment is already resident (the
        ; boot path loads it before zvm_run_until_read).
        LDA #NZ6_OP_RESET
        JSR NZ6_ENTRY
        ; V6: header $06 is the packed address of the main routine. Call it
        ; with no arguments through the normal call machinery; if it ever
        ; returns (frame 0), zvm_return stops with ZVM_STOP_QUIT.
        LDA zstory_initial_pc_lo
        STA zvm_operand_lo
        LDA zstory_initial_pc_hi
        STA zvm_operand_hi
        LDA #$01
        STA zvm_operand_count    ; counts the packed-addr operand itself:
                                 ; 1 = address only -> arg_count = 0.
                                 ; (0 would trip the null-call guard.)
        LDA #$FF
        STA zvm_store_var        ; call_vn semantics: no store
        STZ zvm_pc_l             ; sentinel return PC (frame 0 return = quit);
                                 ; never dispatched, so zvm_check_code_target
                                 ; never validates it
        STZ zvm_pc_h
        STZ zvm_pc_b
        JSR zvm_call_common
        BRA @loop
@direct_pc:
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

        CMP #$BE
        BNE @not_extended
        LDA zstory_version
        CMP #$05
        BCC @not_extended
        JMP zvm_decode_extended
@not_extended:
        LDA zvm_opcode
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
        CMP #$08
        BNE @types
        JSR zvm_fetch
        STA zvm_type_byte2
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
        CMP #$1B              ; V6 set_colour: fg bg [window] — 3 operands.
        BNE :+
        LDA #$03
        BRA @set_limit
:
        LDA #$02
@set_limit:
        STA zvm_operand_limit

@types:
        JSR zvm_decode_typed_operands
        JMP zvm_dispatch

zvm_decode_extended:
        LDA #$04
        STA zvm_form
        JSR zvm_fetch
        STA zvm_opnum
        JSR zvm_fetch
        STA zvm_type_byte
        STZ zvm_operand_count
        LDA #$04
        STA zvm_operand_limit
        JSR zvm_decode_typed_operands
        JMP zvm_dispatch_ext

zvm_decode_typed_operands:
        JSR zvm_set_raw_first
        LDX #$00
@type_loop:
        CPX zvm_operand_limit
        BCS @done
        TXA
        CMP #$04
        BCC @type_byte1
        SEC
        SBC #$04
        STA zvm_tmp
        LDA zvm_type_byte2
        BRA @shift
@type_byte1:
        STA zvm_tmp
        LDA zvm_type_byte
@shift:
        LDY zvm_tmp
        CPY #$00
        BEQ @shift6
        CPY #$01
        BEQ @shift4
        CPY #$02
        BEQ @shift2
        BRA @have_type
@shift6:
        LSR
        LSR
        LSR
        LSR
        LSR
        LSR
        BRA @have_type
@shift4:
        LSR
        LSR
        LSR
        LSR
        BRA @have_type
@shift2:
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
        RTS

zvm_var_operand_limit:
        LDA zvm_opnum
        CMP #$0C
        BEQ @eight
        CMP #$1A
        BEQ @eight
        LDA #$04
        RTS
@eight:
        LDA #$08
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
        ; pull (VAR:9) takes an indirect variable reference in v1-5 ("pull
        ; (variable)"), but in V6 its optional operand is a real VALUE — the
        ; user-stack address — so a variable-type operand must be evaluated,
        ; not kept raw (Zork Zero peeks its input stack via "pull L00 -> L03").
        LDA zstory_version
        CMP #$06
        BCS @done
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

zvm_dispatch_ext:
        LDA zvm_opnum
        CMP #zvm_ext_count
        BCC :+
        JMP zvm_unsupported
:
        ASL
        TAX
        LDA zvm_ext_table,X
        STA zvm_jump_l
        LDA zvm_ext_table + 1,X
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
        JSR zvm_set_global_var_addr
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
        JSR zvm_set_global_var_addr
        LDA zvm_value_hi
        STA zstory_word_hi
        LDA zvm_value_lo
        STA zstory_word_lo
        JMP zstory_write16

zvm_set_global_var_addr:
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
        RTS

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
        LDA zstory_version
        CMP #$06
        BEQ @main_returned
        LDA #ZVM_STOP_UNSUPPORTED
        STA zvm_stop_reason
        RTS
@main_returned:
        ; V6: the main routine returned -- the machine halts cleanly.
        LDA #ZVM_STOP_QUIT
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
        ; V6: frame 0 carries the all-zero sentinel return PC planted by
        ; zvm_run_until_read — restoring it means the MAIN ROUTINE returned,
        ; which halts the machine cleanly (it must not reach the bad-target
        ; diagnostic below).
        CPX #$00
        BNE @check_target
        LDA zstory_version
        CMP #$06
        BNE @check_target
        LDA zvm_pc_l
        ORA zvm_pc_h
        ORA zvm_pc_b
        BNE @check_target
        BRA @main_returned
@check_target:
        JSR zvm_check_code_target
        LDA zvm_stop_reason
        BEQ :+
        RTS
:
        LDA zvm_frame_store,X
        STA zvm_store_var
        LDA zvm_frame_sp,X
        STA zvm_sp
        LDA zvm_frame_arg_count,X
        STA zvm_arg_count
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
        CMP #$FF
        BEQ @return_discarded
        JMP zvm_set_var
@return_discarded:
        RTS

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
        LDA zvm_store_var
        CMP #$FF
        BEQ @null_done
        STZ zvm_value_lo
        STZ zvm_value_hi
        LDA zvm_store_var
        JMP zvm_set_var
@null_done:
        RTS
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
        LDA zvm_arg_count
        STA zvm_frame_arg_count,X
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

        JSR zvm_unpack_routine_addr
        LDA zstory_addr_l
        STA zvm_pc_l
        LDA zstory_addr_m
        STA zvm_pc_h
        LDA zstory_addr_h
        STA zvm_pc_b
        JSR zvm_check_code_target
        LDA zvm_stop_reason
        BEQ :+
        RTS
:
        JSR zvm_fetch
        STA zvm_tmp              ; local count
        JSR zvm_clear_locals
        LDA zstory_version
        CMP #$05
        BCS @copy_args
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
        STA zvm_arg_count
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

zvm_call_1n:
        LDA #$FF
        STA zvm_store_var
        JMP zvm_call_common

zvm_call_1op_15:
        LDA zstory_version
        CMP #$05
        BCS zvm_call_1n
        JMP zvm_not

zvm_call_2s:
        JSR zvm_fetch
        STA zvm_store_var
        JMP zvm_call_common

zvm_call_2n:
        LDA #$FF
        STA zvm_store_var
        JMP zvm_call_common

zvm_call_vn:
        LDA #$FF
        STA zvm_store_var
        JMP zvm_call_common

zvm_unpack_operand_packed_addr:
        LDA zvm_operand_lo
        ASL
        STA zstory_addr_l
        LDA zvm_operand_hi
        ROL
        STA zstory_addr_m
        LDA #$00
        ROL
        STA zstory_addr_h
        LDA zstory_version
        CMP #$04
        BCC @done
        ASL zstory_addr_l
        ROL zstory_addr_m
        ROL zstory_addr_h
@done:
        RTS

; zvm_unpack_routine_addr / zvm_unpack_string_addr: unpack the packed address
; in zvm_operand_lo/hi into zstory_addr_l/m/h, then (V6 only) add the 8x
; routines/strings byte offset read from the header. The offsets live in a
; contiguous BSS block (routine l/m/h then string l/m/h), so X=0 selects the
; routine offset and X=3 the string offset -- same convention as zstory.s.
zvm_unpack_routine_addr:
        LDX #$00
        BRA zvm_unpack_offset_addr
zvm_unpack_string_addr:
        LDX #$03
zvm_unpack_offset_addr:
        JSR zvm_unpack_operand_packed_addr
        LDA zstory_version
        CMP #$06
        BNE @done
        CLC
        LDA zstory_addr_l
        ADC zstory_routine_off_l,X
        STA zstory_addr_l
        LDA zstory_addr_m
        ADC zstory_routine_off_m,X
        STA zstory_addr_m
        LDA zstory_addr_h
        ADC zstory_routine_off_h,X
        STA zstory_addr_h
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
        JSR zvm_unpack_string_addr
        LDA #ZTEXT_UNLIMITED
        STA ztext_word_limit
        JMP ztext_print_packed

zvm_print_obj:
        JSR zvm_set_object_num_operand0
        JMP zobject_print_short_name

zvm_new_line:
        JMP newline

zvm_print_char:
        LDA zvm_operand_lo
        JMP print_char

zvm_print_num:
        LDA zvm_operand_hi
        BPL @positive
        LDA #'-'
        JSR print_char
        SEC
        LDA #$00
        SBC zvm_operand_lo
        TAX
        LDA #$00
        SBC zvm_operand_hi
        JMP print_u16_dec
@positive:
        LDA zvm_operand_hi
        LDX zvm_operand_lo
        JMP print_u16_dec

zvm_nop:
        RTS

zvm_pop:
        JSR zvm_stack_pop
        RTS

zvm_pop_or_catch:
        LDA zstory_version
        CMP #$05
        BCC zvm_pop
        LDA zvm_frame_count
        JMP zvm_store_a_lo_zero_hi

zvm_quit:
        LDA #ZVM_STOP_QUIT
        STA zvm_stop_reason
        RTS

zvm_restart:
        JMP reset

zvm_save:
        LDA zstory_version
        CMP #$04
        BCS zvm_save_store
        JSR zvm_branch_decode
        STZ zvm_save_return_kind
        JSR zvm_save_game
        BEQ @ok
        LDA #$00
        JMP zvm_branch_apply
@ok:
        LDA #$01
        JMP zvm_branch_apply

zvm_save_store:
        LDA #ZVM_SAVE_RETURN_STORE
        STA zvm_save_return_kind
        JSR zvm_fetch
        STA zvm_save_store_var
        JSR zvm_save_game
        BEQ @ok
        STZ zvm_value_lo
        BRA @store
@ok:
        LDA #$01
        STA zvm_value_lo
@store:
        STZ zvm_value_hi
        LDA zvm_save_store_var
        JMP zvm_set_var

zvm_restore:
        LDA zstory_version
        CMP #$04
        BCS zvm_restore_store
        JSR zvm_restore_game
        BEQ @ok
        JMP zvm_branch_false
@ok:
        LDA #$01
        JMP zvm_branch_apply

zvm_restore_store:
        JSR zvm_fetch
        STA zvm_save_store_var
        JSR zvm_restore_game
        BEQ zvm_apply_saved_store_return
        STZ zvm_value_lo
        STZ zvm_value_hi
        LDA zvm_save_store_var
        JMP zvm_set_var

zvm_apply_saved_store_return:
        LDA zvm_save_header + ZVM_SAVE_RETURN_KIND
        CMP #ZVM_SAVE_RETURN_STORE
        BNE @branch_return
        LDA #$02
        STA zvm_value_lo
        STZ zvm_value_hi
        LDA zvm_save_header + ZVM_SAVE_STORE_VAR
        JMP zvm_set_var
@branch_return:
        LDA #$01
        JMP zvm_branch_apply

zvm_verify:
        JSR zvm_compute_checksum
        BNE @fail
        LDA zvm_verify_sum_hi
        CMP zstory_checksum_hi
        BNE @fail
        LDA zvm_verify_sum_lo
        CMP zstory_checksum_lo
        BNE @fail
        JMP zvm_branch_true
@fail:
        JMP zvm_branch_false

zvm_compute_checksum:
        LDA zstory_filebytes_l
        STA zvm_verify_end_l
        LDA zstory_filebytes_m
        STA zvm_verify_end_m
        LDA zstory_filebytes_h
        STA zvm_verify_end_h

        LDA #$40
        STA zstory_addr_l
        STZ zstory_addr_m
        STZ zstory_addr_h
        STZ zvm_verify_sum_lo
        STZ zvm_verify_sum_hi

@loop:
        LDA zstory_addr_h
        CMP zvm_verify_end_h
        BNE @read
        LDA zstory_addr_m
        CMP zvm_verify_end_m
        BNE @read
        LDA zstory_addr_l
        CMP zvm_verify_end_l
        BEQ @ok
@read:
        JSR zstory_read_file8
        BNE @error
        LDA zvm_verify_sum_lo
        CLC
        ADC XRAM_DATA
        STA zvm_verify_sum_lo
        LDA zvm_verify_sum_hi
        ADC #$00
        STA zvm_verify_sum_hi
        JSR zstory_inc_addr
        BRA @loop
@ok:
        LDA #$00
        RTS
@error:
        LDA #$01
        RTS

zvm_verify_shift_end:
        ASL zvm_verify_end_l
        ROL zvm_verify_end_m
        ROL zvm_verify_end_h
        RTS

zvm_save_game:
        JSR zvm_save_dynamic
        BNE @done
        JSR zvm_alloc_save_scratch
        BNE @done
        JSR zvm_fill_save_header
        JSR zvm_copy_save_header_to_xram
        BNE @cleanup
        JSR zvm_copy_vm_state_to_xram
        BNE @cleanup
        JSR zvm_save_state_file
@cleanup:
        PHA
        JSR zvm_free_save_scratch
        PLA
@done:
        RTS

zvm_restore_game:
        JSR zvm_alloc_save_scratch
        BNE @done
        JSR zvm_load_state_file
        BNE @cleanup

        LDA XRAM_LENL
        CMP #<ZVM_SAVE_TOTAL_SIZE
        BNE @bad
        LDA XRAM_LENH
        CMP #>ZVM_SAVE_TOTAL_SIZE
        BNE @bad

        JSR zvm_copy_save_header_from_xram
        BNE @cleanup
        JSR zvm_validate_save_header
        BNE @bad

        JSR zvm_restore_dynamic
        BNE @cleanup
        LDA XRAM_LENL
        CMP zstory_static_lo
        BNE @bad
        LDA XRAM_LENH
        CMP zstory_static_hi
        BNE @bad

        JSR zvm_copy_vm_state_from_xram
        BNE @cleanup
        JSR zvm_restore_display_after_load
        LDA zvm_save_header + ZVM_SAVE_BRANCH_IF
        STA zvm_branch_if
        LDA zvm_save_header + ZVM_SAVE_BRANCH_OFF_H
        STA zvm_branch_off_h
        LDA zvm_save_header + ZVM_SAVE_BRANCH_OFF_L
        STA zvm_branch_off_l
        LDA zvm_save_header + ZVM_SAVE_PC_B
        STA zvm_pc_b
        LDA zvm_save_header + ZVM_SAVE_PC_H
        STA zvm_pc_h
        LDA zvm_save_header + ZVM_SAVE_PC_L
        STA zvm_pc_l
        LDA #$00
        BRA @cleanup
@bad:
        LDA #$01
@cleanup:
        PHA
        JSR zvm_free_save_scratch
        PLA
@done:
        RTS

zvm_save_dynamic:
        JSR zvm_use_dynamic_save_name
        JSR zstory_set_dynamic_xram_origin
        LDA zstory_static_lo
        STA XRAM_LENL
        LDA zstory_static_hi
        STA XRAM_LENH
        JMP xram_xsave

zvm_restore_dynamic:
        JSR zvm_use_dynamic_save_name
        JSR zstory_set_dynamic_xram_origin
        STZ XRAM_LENL
        STZ XRAM_LENH
        JMP xram_xload

zvm_restore_display_after_load:
        LDA zstory_version
        CMP #$04
        BCS @v4_or_newer
        JMP zvm_collapse_upper_window
@v4_or_newer:
        JSR zvm_clamp_window_cursors
        JMP zvm_select_active_window

zvm_save_state_file:
        JSR zvm_use_state_save_name
        JSR zvm_set_save_scratch_addr
        LDA #<ZVM_SAVE_TOTAL_SIZE
        STA XRAM_LENL
        LDA #>ZVM_SAVE_TOTAL_SIZE
        STA XRAM_LENH
        JMP xram_xsave

zvm_load_state_file:
        JSR zvm_use_state_save_name
        JSR zvm_set_save_scratch_addr
        STZ XRAM_LENL
        STZ XRAM_LENH
        JMP xram_xload

zvm_copy_save_header_to_xram:
        JSR zvm_prepare_save_header_copy
        JMP xram_copy_from_ram

zvm_copy_save_header_from_xram:
        JSR zvm_prepare_save_header_copy
        JMP xram_copy_to_ram

zvm_copy_vm_state_to_xram:
        JSR zvm_prepare_save_state_copy
        JMP xram_copy_from_ram

zvm_copy_vm_state_from_xram:
        JSR zvm_prepare_save_state_copy
        JMP xram_copy_to_ram

zvm_prepare_save_header_copy:
        JSR zvm_set_save_scratch_addr
        LDA #<zvm_save_header
        STA XRAM_RAML
        LDA #>zvm_save_header
        STA XRAM_RAMH
        LDA #<ZVM_SAVE_HEADER_SIZE
        STA XRAM_LENL
        LDA #>ZVM_SAVE_HEADER_SIZE
        STA XRAM_LENH
        RTS

zvm_prepare_save_state_copy:
        JSR zvm_set_save_state_addr
        LDA #<zvm_state_start
        STA XRAM_RAML
        LDA #>zvm_state_start
        STA XRAM_RAMH
        LDA #<ZVM_SAVE_STATE_SIZE
        STA XRAM_LENL
        LDA #>ZVM_SAVE_STATE_SIZE
        STA XRAM_LENH
        RTS

zvm_set_save_scratch_addr:
        LDA zvm_save_scratch_base_l
        STA XRAM_ADDRL
        LDA zvm_save_scratch_base_m
        STA XRAM_ADDRM
        LDA zvm_save_scratch_base_h
        STA XRAM_ADDRH
        RTS

zvm_set_save_state_addr:
        LDA zvm_save_scratch_base_l
        CLC
        ADC #ZVM_SAVE_STATE_ADDRL
        STA XRAM_ADDRL
        LDA zvm_save_scratch_base_m
        ADC #ZVM_SAVE_STATE_ADDRM
        STA XRAM_ADDRM
        LDA zvm_save_scratch_base_h
        ADC #$00
        STA XRAM_ADDRH
        RTS

zvm_alloc_save_scratch:
        LDA #XRAM_NOVAZ_SAVE_L
        STA zvm_save_scratch_base_l
        LDA #XRAM_NOVAZ_SAVE_M
        STA zvm_save_scratch_base_m
        LDA #XRAM_NOVAZ_SAVE_H
        STA zvm_save_scratch_base_h
        LDA #$00
        RTS

zvm_free_save_scratch:
        LDA #$00
        RTS

zvm_use_state_save_name:
        LDA #<zvm_state_save_name
        LDY #>zvm_state_save_name
        JMP zvm_use_save_name

zvm_use_dynamic_save_name:
        LDA #<zvm_dynamic_save_name
        LDY #>zvm_dynamic_save_name
        JMP zvm_use_save_name

zvm_use_story_name:
        LDA #<zvm_story_name
        LDY #>zvm_story_name
        JMP zvm_use_save_name

zvm_use_save_name:
        STA XRAM_NAMEPTR_L
        STY XRAM_NAMEPTR_H
        LDA #$09
        STA XRAM_NAMELEN
        RTS

zvm_fill_save_header:
        LDX #$00
@clear:
        STZ zvm_save_header,X
        INX
        CPX #ZVM_SAVE_HEADER_SIZE
        BCC @clear

        LDX #$00
@prefix:
        LDA zvm_save_header_prefix,X
        STA zvm_save_header,X
        INX
        CPX #ZVM_SAVE_PREFIX_SIZE
        BCC @prefix

        LDA zstory_static_lo
        STA zvm_save_header + ZVM_SAVE_DYNAMIC_SIZE_LO
        LDA zstory_static_hi
        STA zvm_save_header + ZVM_SAVE_DYNAMIC_SIZE_HI
        LDA zstory_version
        STA zvm_save_header + ZVM_SAVE_STORY_VERSION
        LDA zstory_release_hi
        STA zvm_save_header + ZVM_SAVE_RELEASE_HI
        LDA zstory_release_lo
        STA zvm_save_header + ZVM_SAVE_RELEASE_LO
        LDX #$00
@serial:
        LDA zstory_serial,X
        STA zvm_save_header + ZVM_SAVE_SERIAL,X
        INX
        CPX #$06
        BCC @serial
        LDA zstory_checksum_hi
        STA zvm_save_header + ZVM_SAVE_CHECKSUM_HI
        LDA zstory_checksum_lo
        STA zvm_save_header + ZVM_SAVE_CHECKSUM_LO
        LDA zvm_branch_if
        STA zvm_save_header + ZVM_SAVE_BRANCH_IF
        LDA zvm_branch_off_h
        STA zvm_save_header + ZVM_SAVE_BRANCH_OFF_H
        LDA zvm_branch_off_l
        STA zvm_save_header + ZVM_SAVE_BRANCH_OFF_L
        LDA zvm_pc_b
        STA zvm_save_header + ZVM_SAVE_PC_B
        LDA zvm_pc_h
        STA zvm_save_header + ZVM_SAVE_PC_H
        LDA zvm_pc_l
        STA zvm_save_header + ZVM_SAVE_PC_L
        LDA zvm_save_return_kind
        STA zvm_save_header + ZVM_SAVE_RETURN_KIND
        LDA zvm_save_store_var
        STA zvm_save_header + ZVM_SAVE_STORE_VAR
        RTS

zvm_validate_save_header:
        LDX #$00
@prefix:
        LDA zvm_save_header,X
        CMP zvm_save_header_prefix,X
        BNE @bad
        INX
        CPX #ZVM_SAVE_PREFIX_SIZE
        BCC @prefix

        LDA zvm_save_header + ZVM_SAVE_DYNAMIC_SIZE_LO
        CMP zstory_static_lo
        BNE @bad
        LDA zvm_save_header + ZVM_SAVE_DYNAMIC_SIZE_HI
        CMP zstory_static_hi
        BNE @bad
        LDA zvm_save_header + ZVM_SAVE_STORY_VERSION
        CMP zstory_version
        BNE @bad
        LDA zvm_save_header + ZVM_SAVE_RELEASE_HI
        CMP zstory_release_hi
        BNE @bad
        LDA zvm_save_header + ZVM_SAVE_RELEASE_LO
        CMP zstory_release_lo
        BNE @bad
        LDA zvm_save_header + ZVM_SAVE_CHECKSUM_HI
        CMP zstory_checksum_hi
        BNE @bad
        LDA zvm_save_header + ZVM_SAVE_CHECKSUM_LO
        CMP zstory_checksum_lo
        BNE @bad
        LDX #$00
@serial:
        LDA zvm_save_header + ZVM_SAVE_SERIAL,X
        CMP zstory_serial,X
        BNE @bad
        INX
        CPX #$06
        BCC @serial
        LDA #$00
        RTS
@bad:
        LDA #$01
        RTS

zvm_jz:
        LDA zvm_operand_lo
        ORA zvm_operand_hi
        BEQ @true
        JMP zvm_branch_false
@true:
        JMP zvm_branch_true

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
        JMP zvm_branch_true
@false:
        JMP zvm_branch_false

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

zvm_branch_false:
        LDA #$00
        BRA zvm_branch

zvm_branch_true:
        LDA #$01

zvm_branch:
        STA zvm_tmp              ; actual condition, 0/1
        JSR zvm_branch_decode
        LDA zvm_tmp
        JMP zvm_branch_apply

zvm_branch_decode:
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
        RTS

zvm_branch_apply:
        STA zvm_tmp              ; actual condition, 0/1
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
        JMP zvm_branch_true
@false:
        JMP zvm_branch_false

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

zvm_log_shift:
        LDA zvm_operand_lo
        STA zvm_value_lo
        LDA zvm_operand_hi
        STA zvm_value_hi
        LDA zvm_operand_hi + 1
        BMI @right
        LDA zvm_operand_lo + 1
        BEQ @store
        STA zvm_tmp
@left_loop:
        ASL zvm_value_lo
        ROL zvm_value_hi
        DEC zvm_tmp
        BNE @left_loop
        BRA @store
@right:
        SEC
        LDA #$00
        SBC zvm_operand_lo + 1
        STA zvm_tmp
        BEQ @store
@right_loop:
        LSR zvm_value_hi
        ROR zvm_value_lo
        DEC zvm_tmp
        BNE @right_loop
@store:
        JMP zvm_store_result

zvm_art_shift:
        LDA zvm_operand_lo
        STA zvm_value_lo
        LDA zvm_operand_hi
        STA zvm_value_hi
        LDA zvm_operand_hi + 1
        BMI @right
        LDA zvm_operand_lo + 1
        BEQ @store
        STA zvm_tmp
@left_loop:
        ASL zvm_value_lo
        ROL zvm_value_hi
        DEC zvm_tmp
        BNE @left_loop
        BRA @store
@right:
        SEC
        LDA #$00
        SBC zvm_operand_lo + 1
        STA zvm_tmp
        BEQ @store
@right_loop:
        LDA zvm_value_hi
        AND #$80
        STA zvm_tmp2
        LSR zvm_value_hi
        ROR zvm_value_lo
        LDA zvm_tmp2
        BEQ :+
        ORA zvm_value_hi
        STA zvm_value_hi
:
        DEC zvm_tmp
        BNE @right_loop
@store:
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

zvm_set_operand0_word_index1_addr:
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
        RTS

zvm_set_operand0_byte_index1_addr:
        LDA zvm_operand_lo
        CLC
        ADC zvm_operand_lo + 1
        STA zstory_addr_l
        LDA zvm_operand_hi
        ADC zvm_operand_hi + 1
        STA zstory_addr_m
        STZ zstory_addr_h
        RTS

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
        JSR zvm_set_operand0_word_index1_addr
        JSR zstory_read16
        LDA zstory_word_lo
        STA zvm_value_lo
        LDA zstory_word_hi
        STA zvm_value_hi
        JMP zvm_store_result

zvm_loadb:
        JSR zvm_set_operand0_byte_index1_addr
        JSR zstory_read8
        LDA XRAM_DATA
        JMP zvm_store_a_lo_zero_hi

zvm_storew:
        JSR zvm_set_operand0_word_index1_addr
        LDA zvm_operand_hi + 2
        STA zstory_word_hi
        LDA zvm_operand_lo + 2
        STA zstory_word_lo
        JMP zstory_write16

zvm_storeb:
        JSR zvm_set_operand0_byte_index1_addr
        LDA zvm_operand_lo + 2
        STA XRAM_DATA
        JMP zstory_write8

zvm_get_parent:
        JSR zvm_set_object_num_operand0
        JSR zobject_get_parent
        BEQ :+
        STZ zobject_value_lo
        STZ zobject_value_hi
:
        JMP zvm_store_object_value

zvm_get_sibling:
        JSR zvm_set_object_num_operand0
        JSR zobject_get_sibling
        BEQ :+
        STZ zobject_value_lo
        STZ zobject_value_hi
:
        JSR zvm_store_object_value
        LDA zvm_value_lo
        ORA zvm_value_hi
        BEQ @false
        JMP zvm_branch_true
@false:
        JMP zvm_branch_false

zvm_get_child:
        JSR zvm_set_object_num_operand0
        JSR zobject_get_child
        BEQ :+
        STZ zobject_value_lo
        STZ zobject_value_hi
:
        JSR zvm_store_object_value
        LDA zvm_value_lo
        ORA zvm_value_hi
        BEQ @false
        JMP zvm_branch_true
@false:
        JMP zvm_branch_false

zvm_store_object_value:
        LDA zobject_value_lo
        STA zvm_value_lo
        LDA zobject_value_hi
        STA zvm_value_hi
        JMP zvm_store_result

zvm_set_object_num_operand0:
        LDA zvm_operand_lo
        STA zobject_num
        LDA zvm_operand_hi
        STA zobject_num_hi
        LDA zobject_num
        RTS

zvm_set_object_prop_operand01:
        JSR zvm_set_object_num_operand0
        LDA zvm_operand_lo + 1
        STA zobject_prop_num
        RTS

zvm_jin:
        JSR zvm_set_object_num_operand0
        JSR zobject_get_parent
        LDA zobject_value_hi
        CMP zvm_operand_hi + 1
        BNE @false
        LDA zobject_value_lo
        CMP zvm_operand_lo + 1
        BEQ @true
@false:
        JMP zvm_branch_false
@true:
        JMP zvm_branch_true

zvm_test_attr:
        JSR zvm_set_object_num_operand0
        LDA zvm_operand_lo + 1
        STA zobject_attr_num
        JSR zobject_test_attr
        JMP zvm_branch

zvm_set_attr:
        JSR zvm_set_object_num_operand0
        LDA zvm_operand_lo + 1
        STA zobject_attr_num
        JMP zobject_set_attr

zvm_clear_attr:
        JSR zvm_set_object_num_operand0
        LDA zvm_operand_lo + 1
        STA zobject_attr_num
        JMP zobject_clear_attr

zvm_remove_obj:
        JSR zvm_set_object_num_operand0
        JMP zobject_remove

zvm_insert_obj:
        JSR zvm_set_object_num_operand0
        LDA zvm_operand_lo + 1
        STA zobject_value_lo
        LDA zvm_operand_hi + 1
        STA zobject_value_hi
        JMP zobject_insert

zvm_get_prop:
        JSR zvm_set_object_prop_operand01
        JSR zobject_get_prop
        JMP zvm_store_object_value

zvm_get_prop_addr:
        JSR zvm_set_object_prop_operand01
        JSR zobject_get_prop_addr
        JMP zvm_store_object_value

zvm_get_next_prop:
        JSR zvm_set_object_prop_operand01
        JSR zobject_get_next_prop
        JMP zvm_store_object_value

zvm_get_prop_len:
        LDA zvm_operand_lo
        STA zobject_value_lo
        LDA zvm_operand_hi
        STA zobject_value_hi
        JSR zobject_get_prop_len
        JMP zvm_store_object_value

zvm_put_prop:
        JSR zvm_set_object_prop_operand01
        JSR zobject_get_prop_addr
        LDA zobject_value_lo
        STA zvm_tmp
        LDA zobject_value_hi
        STA zvm_tmp2
        JSR zobject_get_prop_len
        LDA zvm_tmp
        STA zstory_addr_l
        LDA zvm_tmp2
        STA zstory_addr_m
        STZ zstory_addr_h
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

; pull (VAR:9). v<6: "pull (variable)" — pop the game stack into the named
; variable, no store byte. V6: "pull stack -> (result)" — a STORE op with an
; optional user-stack operand; routed to the segment, which MUST consume the
; store byte or the instruction stream derails (Zork Zero hits this at its
; first prompt).
zvm_pull:
        LDX #NZ6_OP_PULL
        JSR zvm_v6_maybe_route
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
        JMP zvm_store_a_lo_zero_hi
@seed:
        LDA #$00
        SEC
        SBC zvm_operand_lo
        STA zvm_rng_lo
        LDA #$00
        SBC zvm_operand_hi
        STA zvm_rng_hi
        JMP zvm_false_store
@zero:
        JSR rng_get16
        BNE @fallback_seed
        LDA RNG_VALUE0
        EOR VGC_FRAME
        STA zvm_rng_lo
        LDA RNG_VALUE1
        EOR zvm_opcode_pc_l
        STA zvm_rng_hi
        JMP zvm_false_store
@fallback_seed:
        LDA #$4D
        STA zvm_rng_lo
        LDA #$21
        STA zvm_rng_hi
        JMP zvm_false_store

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

zvm_false_store:
        STZ zvm_value_lo
        STZ zvm_value_hi
        JMP zvm_store_result

zvm_store_a_lo_zero_hi:
        STA zvm_value_lo
        STZ zvm_value_hi
        JMP zvm_store_result

zvm_true_branch:
        LDA #$01
        JMP zvm_branch

zvm_read:
        LDA zstory_version
        CMP #$04
        BCS @skip_status
        JSR zvm_show_status
@skip_status:
        LDA zstory_version
        CMP #$05
        BCC @no_store
        JSR zvm_fetch
        STA zvm_store_var
@no_store:
        JSR nz_screen_flush_word
        LDA #$01
        STA nz_raw_input_mode
        LDA zvm_operand_lo
        STA zvm_text_lo
        LDA zvm_operand_hi
        STA zvm_text_hi
        LDA zvm_operand_lo + 1
        STA zvm_parse_lo
        LDA zvm_operand_hi + 1
        STA zvm_parse_hi

        LDX #$02                ; read operands: [text, parse, time, routine]
        JSR nz_timed_arm

        STZ zvm_input_len
        JSR nz_cursor_on
        JSR zvm_read_text_max
        ; Dispatch once on interrupt-driven vs untimed. These use full labels
        ; (not cheap locals) so control flow can cross the exported
        ; zvm_read_key_loop label, which ca65 treats as a cheap-local scope
        ; boundary. The poll loop is entered when a timer OR a sampled sound
        ; with a finish routine is armed.
        JSR nz_read_irq_pending
        BNE zvm_read_timed_poll
zvm_read_key_loop:
        ; Untimed idle spin. Kept to exactly two instructions so the smoke
        ; harness, which samples the PC at a fixed power-of-two interval, always
        ; observes the read loop here (it matches this address or this+3).
        LDA VGC_CHARIN
        BEQ zvm_read_key_loop
zvm_read_have_key:
        CMP #$0A
        BEQ zvm_read_done
        CMP #$0D
        BEQ zvm_read_done
        CMP #$08
        BEQ zvm_read_backspace
        CMP #$7F
        BEQ zvm_read_backspace
        CMP #$20
        BCC zvm_read_recheck
        CMP #$7F
        BCS zvm_read_recheck
        LDX zvm_input_len
        CPX zvm_input_max
        BCS zvm_read_recheck
        JSR zvm_to_lower
        PHA
        JSR print_char
        PLA
        JSR zvm_store_input_char
        INC zvm_input_len
zvm_read_recheck:
        JSR nz_read_irq_pending
        BNE zvm_read_timed_poll
        BRA zvm_read_key_loop
zvm_read_backspace:
        LDA zvm_input_len
        BEQ zvm_read_recheck
        DEC zvm_input_len
        LDA #$08
        JSR print_char
        LDA #' '
        JSR print_char
        LDA #$08
        JSR print_char
        BRA zvm_read_recheck
zvm_read_timed_poll:
        LDA VGC_CHARIN
        BNE zvm_read_have_key
        LDA zvm_timed_active
        BEQ @poll_sound
        JSR nz_timed_tick
        LDA zvm_timed_abort
        BNE zvm_read_timed_done
@poll_sound:
        JSR nz_sound_poll
        BRA zvm_read_timed_poll
zvm_read_timed_done:
        STZ zvm_read_terminator     ; interrupt routine cancelled input
        BRA zvm_read_finish
zvm_read_done:
        STA zvm_read_terminator
zvm_read_finish:
        JSR nz_timed_disable
        JSR nz_cursor_off
        LDA #$00
        JSR zvm_store_input_terminator
        JSR newline
        STZ nz_raw_input_mode
        STZ nz_more_line_count
        STZ nz_auto_wrap
        LDA zvm_parse_lo
        ORA zvm_parse_hi
        BEQ zvm_read_no_token
        JSR zvm_tokenize_input
zvm_read_no_token:
        LDA zstory_version
        CMP #$05
        BCC zvm_read_no_result
        LDA zvm_read_terminator
        STA zvm_value_lo
        STZ zvm_value_hi
        LDA zvm_store_var
        JMP zvm_set_var
zvm_read_no_result:
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
        JSR zvm_input_text_offset
        JSR zvm_set_text_addr_offset
        PLA
        STA XRAM_DATA
        JMP zstory_write8

zvm_store_input_terminator:
        PHA
        LDA zstory_version
        CMP #$05
        BCC @v1_v4
        LDA #$01
        JSR zvm_set_text_addr_offset
        PLA
        LDA zvm_input_len
        STA XRAM_DATA
        JMP zstory_write8
@v1_v4:
        LDA zvm_input_len
        INC
        JSR zvm_set_text_addr_offset
        PLA
        STA XRAM_DATA
        JMP zstory_write8

zvm_input_text_offset:
        INC
        LDX zstory_version
        CPX #$05
        BCC @done
        INC
@done:
        RTS

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

zvm_read_input_at:
        LDX zstory_version
        CPX #$05
        BCC :+
        INC
:
        JMP zvm_read_text_byte_offset

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
        LDX zstory_version
        CPX #$05
        BCC :+
        INC
:
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
        JSR zvm_encode_init
        STZ zvm_tmp
@loop:
        LDA zvm_tmp
        CMP zvm_token_len
        BCS @pack
        LDA zvm_encode_slot
        CMP zvm_encode_limit
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
        JMP zvm_encode_pack

zvm_encode_init:
        LDA #$06
        STA zvm_encode_limit
        LDA #$04
        STA zvm_encode_byte_count
        LDA zstory_version
        CMP #$04
        BCC :+
        LDA #$09
        STA zvm_encode_limit
        LDA #$06
        STA zvm_encode_byte_count
:
        LDX #$00
        LDA #$05
@pad:
        STA zvm_encode_zchars,X
        INX
        CPX zvm_encode_limit
        BCC @pad
        STZ zvm_encode_slot
        RTS

zvm_encode_pack:
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
        LDX zstory_version
        CPX #$04
        BCS :+
        ORA #$80
:
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
        LDA zstory_version
        CMP #$04
        BCC @done

        LDA zvm_encode_zchars + 6
        ASL
        ASL
        ORA #$80
        STA zvm_encode_bytes + 4
        LDA zvm_encode_zchars + 7
        LSR
        LSR
        LSR
        ORA zvm_encode_bytes + 4
        STA zvm_encode_bytes + 4
        LDA zvm_encode_zchars + 7
        AND #$07
        ASL
        ASL
        ASL
        ASL
        ASL
        ORA zvm_encode_zchars + 8
        STA zvm_encode_bytes + 5
@done:
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
        PHX
        LDA #$05
        JSR zvm_emit_encoded_zchar
        PLX
        TXA
        CLC
        ADC #18
        JMP zvm_emit_encoded_zchar

zvm_emit_encoded_zchar:
        LDX zvm_encode_slot
        CPX zvm_encode_limit
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
        CMP zvm_encode_byte_count
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
        JSR nz_screen_flush_word
        LDA zstory_version
        CMP #$04
        BCC :+
        JMP @done
:
        LDA VTEXT_CURX
        STA zvm_status_x
        LDA VTEXT_CURY
        STA zvm_status_y
        LDA VTEXT_COLOR
        STA zvm_status_color
        LDA VTEXT_ATTR
        STA zvm_status_attr
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
        STZ zobject_num_hi
        JSR zobject_print_short_name

        LDA #55
        STA VTEXT_CURX
        STZ VTEXT_CURY
        JSR vtext_set_cursor
        LDA zstory_version
        CMP #$03
        BNE @score_status
        LDA zstory_flags1
        AND #$02
        BEQ @score_status
        LDA #<msg_time
        LDY #>msg_time
        JSR print_text
        LDA #$11
        JSR zvm_print_global_u16
        LDA #':'
        JSR print_char
        LDA #$12
        JSR zvm_get_var
        LDA zvm_value_hi
        BNE :+
        LDA zvm_value_lo
        CMP #10
        BCS :+
        LDA #'0'
        JSR print_char
:
        LDA zvm_value_hi
        LDX zvm_value_lo
        JSR print_u16_dec
        BRA @restore_region
@score_status:
        LDA #<msg_score
        LDY #>msg_score
        JSR print_text
        LDA #$11
        JSR zvm_print_global_u16
        LDA #<msg_moves
        LDY #>msg_moves
        JSR print_text
        LDA #$12
        JSR zvm_print_global_u16

@restore_region:
        LDA zvm_status_x
        LDX zvm_window_current
        BEQ :+
        STA zvm_upper_x
        BRA :++
:
        STA zvm_lower_x
:
        LDA zvm_status_y
        LDX zvm_window_current
        BEQ :+
        STA zvm_upper_y
        BRA :++
:
        STA zvm_lower_y
:
        LDA zvm_status_color
        STA VTEXT_COLOR
        LDA zvm_status_attr
        STA VTEXT_ATTR
        JMP zvm_select_active_window
@done:
        RTS

zvm_print_global_u16:
        JSR zvm_get_var
        LDA zvm_value_hi
        LDX zvm_value_lo
        JMP print_u16_dec

zvm_split_window:
        LDX #NZ6_OP_SPLIT_WINDOW
        JSR zvm_v6_maybe_route  ; V6: handled by the NOVAZ6 segment
        JSR nz_screen_flush_word
        JSR zvm_window_save_cursor
        LDA zvm_operand_lo
        CMP #49
        BCC :+
        LDA #48
:
        STA zvm_split_lines
        BNE :+
        STZ zvm_window_current
:
        JSR zvm_clamp_window_cursors
        LDA zvm_split_lines
        BEQ @select
        LDA zstory_version
        CMP #$04
        BCS @select
        JSR zvm_clear_upper_window
@select:
        JMP zvm_select_active_window

zvm_set_window:
        LDX #NZ6_OP_SET_WINDOW
        JSR zvm_v6_maybe_route  ; V6: handled by the NOVAZ6 segment
        JSR nz_screen_flush_word
        JSR zvm_window_save_cursor
        LDA zvm_operand_lo
        BEQ @lower
        LDA zvm_split_lines
        BEQ @done
        LDA #$01
        STA zvm_window_current
        STZ zvm_upper_x
        STZ zvm_upper_y
        JMP zvm_select_active_window
@lower:
        STZ zvm_window_current
        JMP zvm_select_active_window
@done:
        RTS

zvm_reset_windows:
        STZ zvm_window_current
        STZ zvm_split_lines
        STZ zvm_lower_x
        STZ zvm_lower_y
        STZ zvm_upper_x
        STZ zvm_upper_y
        JMP zvm_select_active_window

zvm_collapse_upper_window:
        STZ zvm_window_current
        STZ zvm_split_lines
        STZ zvm_upper_x
        STZ zvm_upper_y
        JMP zvm_select_active_window

zvm_window_save_cursor:
        LDA zvm_window_current
        BEQ @lower
        LDA VTEXT_CURX
        STA zvm_upper_x
        LDA VTEXT_CURY
        STA zvm_upper_y
        RTS
@lower:
        LDA VTEXT_CURX
        STA zvm_lower_x
        LDA VTEXT_CURY
        STA zvm_lower_y
        RTS

zvm_select_active_window:
        LDA zstory_version
        CMP #$06
        BNE @classic
        ; V6: the NOVAZ6 segment owns the live vtext region — windows are
        ; real rectangles built from the segment's prop table, not the
        ; classic lower/upper split (zvm_split_lines stays v1-5-only).
        LDA #NZ6_OP_SELECT
        JMP NZ6_ENTRY
@classic:
        LDA zvm_window_current
        BEQ zvm_select_lower_window
        LDA zvm_split_lines
        BEQ zvm_select_lower_window
        JMP zvm_select_upper_window

zvm_select_lower_window:
        STZ VTEXT_LEFT
        LDA zstory_version
        CMP #$04
        BCC @v1_v3
        LDA zvm_split_lines
        STA VTEXT_TOP
        LDA #50
        SEC
        SBC zvm_split_lines
        BRA @height
@v1_v3:
        LDA zvm_split_lines
        CLC
        ADC #$01
        STA VTEXT_TOP
        LDA #80
        STA VTEXT_WIDTH
        LDA #49
        SEC
        SBC zvm_split_lines
@height:
        STA VTEXT_HEIGHT
        LDA #80
        STA VTEXT_WIDTH
        LDA #(VTEXT_FLAG_WRAP | VTEXT_FLAG_SCROLL)
        STA VTEXT_FLAGS
        JSR zvm_clamp_lower_cursor
        LDA zvm_lower_x
        STA VTEXT_CURX
        LDA zvm_lower_y
        STA VTEXT_CURY
        JMP vtext_set_cursor

zvm_select_upper_window:
        STZ VTEXT_LEFT
        LDA zstory_version
        CMP #$04
        BCS @top_zero
        LDA #$01
        BRA @top_ready
@top_zero:
        LDA #$00
@top_ready:
        STA VTEXT_TOP
        LDA #80
        STA VTEXT_WIDTH
        LDA zvm_split_lines
        STA VTEXT_HEIGHT
        LDA #VTEXT_FLAG_WRAP
        STA VTEXT_FLAGS
        JSR zvm_clamp_upper_cursor
        LDA zvm_upper_x
        STA VTEXT_CURX
        LDA zvm_upper_y
        STA VTEXT_CURY
        JMP vtext_set_cursor

zvm_clamp_window_cursors:
        JSR zvm_clamp_lower_cursor
        JMP zvm_clamp_upper_cursor

zvm_clamp_lower_cursor:
        LDA zvm_lower_x
        CMP #80
        BCC :+
        LDA #79
        STA zvm_lower_x
:
        LDA zstory_version
        CMP #$04
        BCC @v1_v3
        LDA #50
        BRA @have_base
@v1_v3:
        LDA #49
@have_base:
        SEC
        SBC zvm_split_lines
        STA zvm_tmp
        LDA zvm_lower_y
        CMP zvm_tmp
        BCC :+
        STZ zvm_lower_y
:
        RTS

zvm_clamp_upper_cursor:
        LDA zvm_upper_x
        CMP #80
        BCC :+
        LDA #79
        STA zvm_upper_x
:
        LDA zvm_split_lines
        BEQ @reset_y
        CMP zvm_upper_y
        BCC @reset_y
        BEQ @reset_y
        RTS
@reset_y:
        STZ zvm_upper_y
        RTS

zvm_clear_upper_window:
        LDA VTEXT_COLOR
        PHA
        LDA VTEXT_ATTR
        PHA
        STZ VTEXT_LEFT
        LDA zstory_version
        CMP #$04
        BCS @top_zero
        LDA #$01
        BRA @top_ready
@top_zero:
        LDA #$00
@top_ready:
        STA VTEXT_TOP
        LDA #80
        STA VTEXT_WIDTH
        LDA zvm_split_lines
        STA VTEXT_HEIGHT
        LDA #ZVM_COLOR_NORMAL
        STA VTEXT_COLOR
        STZ VTEXT_ATTR
        LDA #VTEXT_FLAG_WRAP
        STA VTEXT_FLAGS
        STZ VTEXT_CURX
        STZ VTEXT_CURY
        JSR vtext_clear_region
        PLA
        STA VTEXT_ATTR
        PLA
        STA VTEXT_COLOR
        RTS

zvm_set_text_style:
        LDX #NZ6_OP_TEXT_STYLE
        JSR zvm_v6_maybe_route  ; V6: colour-relative styles in the segment
        JSR nz_screen_flush_word
        ; Styles are cumulative: style 0 resets, any other value adds its bits.
        ; Only roman/reverse (bit0) and bold (bit1) change the colour cell;
        ; italic (bit2) and fixed (bit3) accumulate but the VGC ignores them.
        LDA zvm_operand_lo
        BNE @accumulate
        STZ zvm_text_style
        BRA @apply
@accumulate:
        ORA zvm_text_style
        STA zvm_text_style
@apply:
        AND #$03
        TAX
        LDA zvm_style_color_table,X
        STA VTEXT_COLOR
        RTS

zvm_set_colour:
        LDX #NZ6_OP_SET_COLOUR
        JSR zvm_v6_maybe_route  ; V6: handled by the NOVAZ6 segment
        JSR nz_screen_flush_word
        LDA zvm_operand_lo
        JSR zvm_map_z_colour
        STA zvm_tmp
        LDA zvm_operand_lo + 1
        JSR zvm_map_z_colour
        ASL
        ASL
        ASL
        ASL
        ORA zvm_tmp
        STA VTEXT_COLOR
        RTS

zvm_map_z_colour:
        CMP #$01
        BEQ @default
        CMP #$02
        BCC @default
        CMP #$0F
        BCS @default
        SEC
        SBC #$02
        RTS
@default:
        LDA #ZVM_COLOR_NORMAL
        AND #$0F
        RTS

zvm_buffer_mode:
        LDA zvm_operand_lo
        ORA zvm_operand_hi
        BEQ @off
        LDA #$01
        STA zvm_text_buffering
        RTS
@off:
        JSR nz_screen_flush_word
        STZ zvm_text_buffering
        RTS

zvm_output_stream:
        JSR nz_screen_flush_word
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

; sound_effect number effect volume routine   (VAR:21)
;   operand[0] = number   (1 = high bleep, 2 = low bleep, >= 3 = sampled)
;   operand[1] = effect   (1 = prepare, 2 = start, 3 = stop, 4 = finish-with)
;   operand[2] = volume   (low byte = level 1..8/255, high byte = repeats)
;   operand[3] = routine  (V5+ packed address, called when the sound finishes)
zvm_sound_effect:
        LDA zvm_operand_lo              ; number (low byte)
        BNE @go
        RTS                             ; number 0 -> nothing to do
@go:
        CMP #$03
        BCS zvm_snd_sampled             ; number >= 3 -> sampled sound
        ; --- bleep (number 1 or 2) ---
        JSR nz_snd_effect_a             ; A = effect (defaults to 2 = start)
        CMP #$03
        BEQ @bleep_ret                  ; "stop" a one-shot bleep -> no-op
        LDA zvm_operand_lo
        CMP #$01
        BEQ @bleep_hi
        LDA #NZ_BLEEP_LO_NOTE
        BRA @bleep_play
@bleep_hi:
        LDA #NZ_BLEEP_HI_NOTE
@bleep_play:
        STA FIO_SRCL                    ; MIDI note
        LDA #NZ_BLEEP_DURATION
        STA FIO_SRCH                    ; duration in frames
        LDA #NZ_BLEEP_INSTR
        STA FIO_ENDL                    ; instrument slot
        LDA #FIO_CMD_SOUND
        STA FIO_CMD
@bleep_ret:
        RTS

zvm_snd_sampled:
        JSR nz_snd_effect_a             ; A = effect
        CMP #$03
        BEQ @stop
        CMP #$01
        BEQ @prepare                    ; prepare: load the sound bank now
        ; effect 2 (start) or 4 (finish-with)
        LDA zvm_operand_lo              ; number
        STA FIO_SRCL
        LDA #$02
        STA FIO_SRCH                    ; effect = start
        ; volume level (operand[2] low byte, default 255 = loudest)
        LDX zvm_operand_count
        CPX #$03
        BCC @level_default
        LDA zvm_operand_lo + 2
        BRA @level_set
@level_default:
        LDA #$FF
@level_set:
        STA FIO_ENDL
        ; repeats (operand[2] high byte, default 0 = play once)
        LDX zvm_operand_count
        CPX #$03
        BCC @repeats_default
        LDA zvm_operand_hi + 2
        BRA @repeats_set
@repeats_default:
        LDA #$00
@repeats_set:
        STA FIO_ENDH
        LDA #FIO_CMD_ZSOUND
        STA FIO_CMD
        ; --- register the V5 finish routine (operand[3]) ---
        STZ zvm_sound_active
        STZ zvm_sound_routine_lo
        STZ zvm_sound_routine_hi
        LDX zvm_operand_count
        CPX #$04
        BCC @ret                        ; no routine operand supplied
        LDA zvm_operand_lo + 3
        STA zvm_sound_routine_lo
        LDA zvm_operand_hi + 3
        STA zvm_sound_routine_hi
        ORA zvm_sound_routine_lo
        BEQ @ret                        ; routine 0 -> no callback
        LDA #$01
        STA zvm_sound_active
@ret:
        RTS
@stop:
        LDA zvm_operand_lo              ; number
        STA FIO_SRCL
        LDA #$03
        STA FIO_SRCH                    ; effect = stop
        LDA #FIO_CMD_ZSOUND
        STA FIO_CMD
        STZ zvm_sound_active
        RTS
@prepare:
        LDA zvm_operand_lo              ; number (host ignores it for prepare)
        STA FIO_SRCL
        LDA #$01
        STA FIO_SRCH                    ; effect = prepare (load the bank)
        LDA #FIO_CMD_ZSOUND
        STA FIO_CMD
        RTS

; nz_sound_preload: issue a sampled-sound "prepare" at story start so the host
; loads this image's soundfont (SOUND.PAK on Avalonia, ZSOUND.NSF into the WTS
; bank on hardware) before the game ever calls sound_effect. Soundless images
; just get ERR_NOT_FOUND/ERR_IO back, which we ignore.
;
; This MUST go through fio_exec (which clears FIO_STATUS, issues the command,
; and spins until the host signals completion). A bare fire-and-forget store to
; FIO_CMD leaves the slow ZSOUND command in flight on hardware; the story's very
; first init-time FIO call (e.g. @random via FIO_CMD_RNG) then races it, reads a
; garbage result, computes a bad routine address, and CALLs into data -> an
; "UNSUPPORTED Z-OPCODE" crash before any text is drawn. Avalonia's FIO is
; synchronous so it never raced, which is why this was hardware-only.
nz_sound_preload:
        STZ FIO_SRCL                    ; number 0 (unused for prepare)
        LDA #$01
        STA FIO_SRCH                    ; effect = prepare
        LDA #FIO_CMD_ZSOUND
        JMP fio_exec                    ; issue AND wait for completion; A ignored

; Return the sound effect operand in A, defaulting to 2 (start) when the opcode
; was called with only a number.
nz_snd_effect_a:
        LDX zvm_operand_count
        CPX #$02
        BCC @default
        LDA zvm_operand_lo + 1
        RTS
@default:
        LDA #$02
        RTS

zvm_set_font:
        JSR nz_screen_flush_word
        LDA zvm_operand_lo
        CMP #$01
        BEQ @font1
        JMP zvm_false_store
@font1:
        LDA #$01
        JMP zvm_store_a_lo_zero_hi

zvm_read_char:
        LDX #$01                ; read_char operands: [1, time, routine]
        JSR nz_timed_arm
        JSR nz_cursor_on
@wait:
        JSR nz_input_poll
        CMP #$00
        BNE @got
        LDA zvm_timed_abort
        BNE @abort
        BRA @wait
@got:
        PHA
        JSR nz_timed_disable
        JSR nz_cursor_off
        PLA
        JMP zvm_store_a_lo_zero_hi
@abort:
        JSR nz_timed_disable
        JSR nz_cursor_off
        LDA #$00                ; timed routine cancelled input
        JMP zvm_store_a_lo_zero_hi

; --- Timed keyboard input support (V4+) -----------------------------------
;
; nz_timed_arm: X = operand index of the `time` value (routine follows at X+1).
; Arms the timer for a 0.1 s period and records the interrupt routine when both
; a nonzero time and a nonzero routine address are supplied; otherwise input is
; untimed.
nz_timed_arm:
        STZ zvm_timed_active
        STZ zvm_timed_abort
        TXA
        CLC
        ADC #$02                ; operands needed = time index + 2
        CMP zvm_operand_count
        BEQ @have
        BCS @none               ; fewer operands than required
@have:
        LDA zvm_operand_lo,X    ; time (tenths)
        STA zvm_timed_limit
        ORA zvm_operand_hi,X
        BEQ @none               ; time 0 -> untimed
        INX
        LDA zvm_operand_lo,X    ; routine (packed address)
        STA zvm_timed_routine_lo
        LDA zvm_operand_hi,X
        STA zvm_timed_routine_hi
        ORA zvm_timed_routine_lo
        BEQ @none               ; routine 0 -> untimed
        ; arm the timer for one tenth of a second
        STZ TimerCtrl           ; disable + reset counter
        LDA #ZVM_TENTH_DIV_LO
        STA TimerDivL
        LDA #ZVM_TENTH_DIV_HI
        STA TimerDivH
        LDA #$01
        STA TimerCtrl           ; enable
        LDA TimerStatus         ; clear any stale elapsed flag
        STZ zvm_timed_count
        LDA #$01
        STA zvm_timed_active
@none:
        RTS

nz_timed_disable:
        STZ zvm_timed_active
        STZ TimerCtrl           ; disable + reset
        RTS

; Poll for input. Returns A = key (nonzero) when one is ready, else A = 0.
; In timed mode it advances the timer and fires the interrupt routine when the
; programmed number of tenths elapses, setting zvm_timed_abort if the routine
; returns true.
nz_input_poll:
        LDA VGC_CHARIN
        BNE @done
        LDA zvm_timed_active
        BEQ @sound
        JSR nz_timed_tick
@sound:
        JSR nz_sound_poll
        LDA #$00
@done:
        RTS

; Advance the timer and fire the interrupt routine when `time` tenths elapse.
; Assumes timed mode is active. Caller inspects zvm_timed_abort afterwards.
nz_timed_tick:
        LDA TimerStatus
        AND #$01
        BEQ @ret                ; period not elapsed yet
        INC zvm_timed_count
        LDA zvm_timed_count
        CMP zvm_timed_limit
        BCC @ret                ; fewer than `time` tenths so far
        STZ zvm_timed_count
        JMP nz_fire_timed_routine
@ret:
        RTS

; Call the timed interrupt routine to completion and record an abort if it
; returns true. The interrupt routine is ordinary Z-code and may print or move
; the cursor; the call/return machinery preserves the interpreter PC.
nz_fire_timed_routine:
        LDA zvm_timed_routine_lo
        LDX zvm_timed_routine_hi
        JSR nz_call_z_routine
        LDA zvm_value_lo
        ORA zvm_value_hi
        BEQ @done               ; routine returned false (or could not run)
        LDA #$01
        STA zvm_timed_abort     ; true return cancels input
@done:
        RTS

; nz_sound_poll: while a sampled sound with a V5 finish routine is in flight,
; check the host completion flag (ZSOUND_STATUS bit 0, which clears on read) and
; fire the routine exactly once when the sound finishes. A sound finishing does
; not abort the in-progress input.
nz_sound_poll:
        LDA zvm_sound_active
        BEQ @ret
        LDA ZSOUND_STATUS       ; read clears the finished bit
        AND #$01
        BEQ @ret                ; sound has not finished yet
        STZ zvm_sound_active    ; one-shot: fire the finish routine once
        LDA zvm_sound_routine_lo
        LDX zvm_sound_routine_hi
        JSR nz_call_z_routine   ; return value is ignored for sound
@ret:
        RTS

; nz_read_irq_pending: returns A != 0 when read must use the interrupt-poll
; loop rather than the pure idle spin -- i.e. when a timer or a sampled sound
; with a finish routine is armed. Keeping this off the untimed path lets the
; idle spin stay exactly two instructions for the smoke's PC sampler.
nz_read_irq_pending:
        LDA zvm_timed_active
        BNE @yes
        LDA zvm_sound_active
@yes:
        RTS

; nz_call_z_routine: A = packed routine address low byte, X = high byte. Calls
; the routine with no arguments, runs it to completion, and leaves its return
; value in zvm_value_lo/hi (zero if the call could not run). Preserves the
; in-progress read's store var; the call/return machinery preserves the
; interpreter PC so input can resume afterwards.
nz_call_z_routine:
        STA zvm_operand_lo
        STX zvm_operand_hi
        STZ zvm_value_lo
        STZ zvm_value_hi
        LDA zvm_store_var       ; preserve the in-progress read's store var
        PHA
        LDA zvm_timed_depth     ; reentrant: a CR interrupt can fire inside
        PHA                     ; a sound/timed routine's own output
        ; The CR interrupt fires MID-OUTPUT (between two lines of a wrapped
        ; print): preserve the string-decode state — the ztext block and the
        ; shared zstory_addr cursor — so the outer print resumes exactly
        ; where it stopped after the routine's Z-code ran.
        LDX #0
@save_ztext:
        LDA ztext_word_limit,X
        PHA
        INX
        CPX #15
        BNE @save_ztext
        LDA zstory_addr_l
        PHA
        LDA zstory_addr_m
        PHA
        LDA zstory_addr_h
        PHA
        LDA #$01
        STA zvm_operand_count   ; routine only, no arguments
        LDA #$FF
        STA zvm_store_var       ; discard result; read it from zvm_value_*
        LDA zvm_frame_count
        STA zvm_timed_depth
        JSR zvm_call_common
@run:
        LDA zvm_stop_reason
        BNE @done
        LDA zvm_frame_count
        CMP zvm_timed_depth
        BEQ @done
        JSR zvm_step
        BRA @run
@done:
        PLA
        STA zstory_addr_h
        PLA
        STA zstory_addr_m
        PLA
        STA zstory_addr_l
        LDX #14
@restore_ztext:
        PLA
        STA ztext_word_limit,X
        DEX
        BPL @restore_ztext
        PLA
        STA zvm_timed_depth
        PLA
        STA zvm_store_var
        RTS

zvm_erase_window:
        LDX #NZ6_OP_ERASE_WINDOW
        JSR zvm_v6_maybe_route  ; V6: handled by the NOVAZ6 segment
        JSR nz_screen_flush_word
        LDA zvm_operand_hi
        CMP #$FF
        BNE @positive
        LDA zvm_operand_lo
        CMP #$FF
        BEQ @minus_one
        CMP #$FE
        BEQ @minus_two
        RTS
@positive:
        LDA zvm_operand_lo
        BEQ @lower
        CMP #$01
        BEQ @upper
        RTS
@minus_one:
        JSR zvm_collapse_upper_window
        JMP zvm_clear_whole_screen
@minus_two:
        JMP zvm_clear_whole_screen
@lower:
        LDA zvm_window_current
        PHA
        STZ zvm_window_current
        JSR zvm_select_lower_window
        JSR zvm_clear_selected_window
        PLA
        STA zvm_window_current
        JMP zvm_select_active_window
@upper:
        LDA zvm_split_lines
        BEQ @done
        LDA zvm_window_current
        PHA
        LDA #$01
        STA zvm_window_current
        JSR zvm_select_upper_window
        JSR zvm_clear_selected_window
        PLA
        STA zvm_window_current
        JMP zvm_select_active_window
@done:
        RTS

zvm_clear_whole_screen:
        STZ VTEXT_LEFT
        STZ VTEXT_TOP
        LDA #80
        STA VTEXT_WIDTH
        LDA #50
        STA VTEXT_HEIGHT
        LDA zstory_version
        CMP #$06
        BEQ :+                  ; V6: clear in the game's live colours
        LDA #ZVM_COLOR_NORMAL
        STA VTEXT_COLOR
:
        STZ VTEXT_ATTR
        LDA #(VTEXT_FLAG_WRAP | VTEXT_FLAG_SCROLL)
        STA VTEXT_FLAGS
        STZ VTEXT_CURX
        STZ VTEXT_CURY
        JSR vtext_clear_region
        STZ zvm_lower_x
        STZ zvm_lower_y
        STZ zvm_upper_x
        STZ zvm_upper_y
        JMP zvm_select_active_window

zvm_clear_selected_window:
        STZ VTEXT_CURX
        STZ VTEXT_CURY
        JSR vtext_clear_region
        LDA zvm_window_current
        BEQ @lower
        STZ zvm_upper_x
        STZ zvm_upper_y
        RTS
@lower:
        STZ zvm_lower_x
        STZ zvm_lower_y
        RTS

zvm_erase_line:
        JSR nz_screen_flush_word
        LDA zvm_operand_hi
        BNE @done
        LDA zvm_operand_lo
        CMP #$01
        BNE @done
        LDA VTEXT_CURX
        STA zvm_status_x
        LDA VTEXT_CURY
        STA zvm_status_y
        LDA VTEXT_FLAGS
        STA zvm_status_attr
        LDA VTEXT_WIDTH
        SEC
        SBC VTEXT_CURX
        STA zvm_tmp
        BEQ @restore
        STZ VTEXT_FLAGS
@loop:
        LDA #' '
        STA VTEXT_CHAR
        JSR vtext_put_char
        DEC zvm_tmp
        BNE @loop
@restore:
        LDA zvm_status_attr
        STA VTEXT_FLAGS
        LDA zvm_status_x
        STA VTEXT_CURX
        LDA zvm_status_y
        STA VTEXT_CURY
        JMP vtext_set_cursor
@done:
        RTS

zvm_set_cursor:
        LDX #NZ6_OP_SET_CURSOR
        JSR zvm_v6_maybe_route  ; V6: handled by the NOVAZ6 segment
        JSR nz_screen_flush_word
        LDA zvm_window_current
        BNE @selected_upper
        LDA zstory_version
        CMP #$04
        BCS @done
@selected_upper:
        LDA zvm_operand_lo + 1
        BEQ :+
        DEC
:
        STA VTEXT_CURX
        LDA zvm_operand_lo
        BEQ :+
        DEC
:
        STA VTEXT_CURY
        LDA zvm_window_current
        BEQ @lower
        LDA VTEXT_CURX
        STA zvm_upper_x
        LDA VTEXT_CURY
        STA zvm_upper_y
        JSR zvm_clamp_upper_cursor
        JMP zvm_select_upper_window
@lower:
        LDA VTEXT_CURX
        STA zvm_lower_x
        LDA VTEXT_CURY
        STA zvm_lower_y
        JSR zvm_clamp_lower_cursor
        JMP zvm_select_lower_window
@done:
        RTS

zvm_get_cursor:
        LDX #NZ6_OP_GET_CURSOR
        JSR zvm_v6_maybe_route  ; V6: handled by the NOVAZ6 segment
        LDA VTEXT_CURY
        CLC
        ADC #$01
        STA zstory_word_lo
        STZ zstory_word_hi
        LDA zvm_operand_lo
        STA zstory_addr_l
        LDA zvm_operand_hi
        STA zstory_addr_m
        STZ zstory_addr_h
        JSR zstory_write16
        LDA zvm_operand_lo
        CLC
        ADC #$02
        STA zstory_addr_l
        LDA zvm_operand_hi
        ADC #$00
        STA zstory_addr_m
        STZ zstory_addr_h
        LDA VTEXT_CURX
        CLC
        ADC #$01
        STA zstory_word_lo
        STZ zstory_word_hi
        JMP zstory_write16

zvm_check_arg_count:
        LDA zvm_arg_count
        CMP zvm_operand_lo
        BCS @true
        JMP zvm_branch_false
@true:
        JMP zvm_branch_true

zvm_tokenise:
        LDA zvm_operand_lo
        STA zvm_text_lo
        LDA zvm_operand_hi
        STA zvm_text_hi
        LDA zvm_operand_lo + 1
        STA zvm_parse_lo
        LDA zvm_operand_hi + 1
        STA zvm_parse_hi
        JSR zvm_prepare_tokenise_input_len

        LDA zvm_operand_count
        CMP #$03
        BCC @default_dict
        LDA zvm_operand_lo + 2
        ORA zvm_operand_hi + 2
        BEQ @default_dict

        LDA zstory_dictionary_lo
        PHA
        LDA zstory_dictionary_hi
        PHA
        LDA zvm_operand_lo + 2
        STA zstory_dictionary_lo
        LDA zvm_operand_hi + 2
        STA zstory_dictionary_hi
        JSR zvm_tokenize_input
        PLA
        STA zstory_dictionary_hi
        PLA
        STA zstory_dictionary_lo
        RTS
@default_dict:
        JMP zvm_tokenize_input

zvm_prepare_tokenise_input_len:
        LDA zstory_version
        CMP #$05
        BCC @v1_v4
        LDA #$01
        JSR zvm_read_text_byte_offset
        STA zvm_input_len
        RTS
@v1_v4:
        STZ zvm_input_len
        LDA #$00
        JSR zvm_read_text_byte_offset
        STA zvm_input_max
        LDA #$01
        STA zvm_tmp
@loop:
        LDA zvm_tmp
        JSR zvm_read_text_byte_offset
        BEQ @done
        INC zvm_input_len
        INC zvm_tmp
        LDA zvm_input_len
        CMP zvm_input_max
        BCC @loop
@done:
        RTS

zvm_encode_text:
        JSR zvm_encode_init
        LDA zvm_operand_lo
        CLC
        ADC zvm_operand_lo + 2
        STA zvm_mem_src_lo
        LDA zvm_operand_hi
        ADC zvm_operand_hi + 2
        STA zvm_mem_src_hi
        LDA zvm_operand_lo + 3
        STA zvm_mem_dst_lo
        LDA zvm_operand_hi + 3
        STA zvm_mem_dst_hi
        STZ zvm_tmp
@read_loop:
        LDA zvm_tmp
        CMP zvm_operand_lo + 1
        BCS @pack
        LDA zvm_encode_slot
        CMP zvm_encode_limit
        BCS @pack
        LDA zvm_mem_src_lo
        STA zstory_addr_l
        LDA zvm_mem_src_hi
        STA zstory_addr_m
        STZ zstory_addr_h
        JSR zstory_read8
        BNE @done
        LDA XRAM_DATA
        JSR zvm_to_lower
        JSR zvm_encode_char
        JSR zvm_inc_mem_src
        INC zvm_tmp
        BRA @read_loop

@pack:
        JSR zvm_encode_pack
        STZ zvm_tmp
@write_loop:
        LDA zvm_tmp
        CMP zvm_encode_byte_count
        BCS @done
        CLC
        ADC zvm_mem_dst_lo
        STA zstory_addr_l
        LDA zvm_mem_dst_hi
        ADC #$00
        STA zstory_addr_m
        STZ zstory_addr_h
        LDX zvm_tmp
        LDA zvm_encode_bytes,X
        STA XRAM_DATA
        JSR zstory_write8
        BNE @done
        INC zvm_tmp
        BRA @write_loop
@done:
        RTS

zvm_scan_table:
        LDA zvm_operand_lo + 1
        STA zvm_mem_src_lo
        LDA zvm_operand_hi + 1
        STA zvm_mem_src_hi
        LDA zvm_operand_lo + 2
        STA zvm_mem_count_lo
        LDA zvm_operand_hi + 2
        STA zvm_mem_count_hi
        LDA #$82
        STA zvm_mem_stride
        LDA zvm_operand_count
        CMP #$04
        BCC @scan_loop
        LDA zvm_operand_lo + 3
        STA zvm_mem_stride

@scan_loop:
        LDA zvm_mem_count_lo
        ORA zvm_mem_count_hi
        BEQ @not_found

        LDA zvm_mem_src_lo
        STA zstory_addr_l
        LDA zvm_mem_src_hi
        STA zstory_addr_m
        STZ zstory_addr_h
        LDA zvm_mem_stride
        AND #$80
        BEQ @byte_entry
        JSR zstory_read16
        BNE @not_found
        LDA zstory_word_hi
        CMP zvm_operand_hi
        BNE @advance
        LDA zstory_word_lo
        CMP zvm_operand_lo
        BNE @advance
        BRA @found

@byte_entry:
        JSR zstory_read8
        BNE @not_found
        LDA XRAM_DATA
        CMP zvm_operand_lo
        BNE @advance

@found:
        LDA zvm_mem_src_lo
        STA zvm_value_lo
        LDA zvm_mem_src_hi
        STA zvm_value_hi
        JSR zvm_store_result
        JMP zvm_branch_true

@advance:
        LDA zvm_mem_stride
        AND #$7F
        BNE :+
        LDA #$01
:
        CLC
        ADC zvm_mem_src_lo
        STA zvm_mem_src_lo
        LDA zvm_mem_src_hi
        ADC #$00
        STA zvm_mem_src_hi
        JSR zvm_dec_mem_count
        BRA @scan_loop

@not_found:
        STZ zvm_value_lo
        STZ zvm_value_hi
        JSR zvm_store_result
        JMP zvm_branch_false

zvm_copy_table:
        LDA zvm_operand_lo
        STA zvm_mem_src_lo
        LDA zvm_operand_hi
        STA zvm_mem_src_hi
        LDA zvm_operand_lo + 1
        STA zvm_mem_dst_lo
        LDA zvm_operand_hi + 1
        STA zvm_mem_dst_hi
        LDA zvm_operand_lo + 2
        STA zvm_mem_count_lo
        LDA zvm_operand_hi + 2
        STA zvm_mem_count_hi
        BPL @count_ready
        LDA #$80
        STA zvm_mem_stride
        SEC
        LDA #$00
        SBC zvm_mem_count_lo
        STA zvm_mem_count_lo
        LDA #$00
        SBC zvm_mem_count_hi
        STA zvm_mem_count_hi
        BRA @count_ready
@count_ready:
        LDA zvm_mem_dst_lo
        ORA zvm_mem_dst_hi
        BNE :+
        JMP @zero_table
:
        LDA zvm_mem_count_lo
        ORA zvm_mem_count_hi
        BNE :+
        RTS
:

        LDA zvm_mem_stride
        BMI @copy_forward
        LDA zvm_mem_count_hi
        BMI @copy_forward
        STZ zvm_mem_stride
        JSR zvm_copy_table_overlap
        LDA zvm_mem_stride
        BNE @copy_backward

@copy_forward:
@copy_loop:
        LDA zvm_mem_count_lo
        ORA zvm_mem_count_hi
        BNE :+
        RTS
:
        LDA zvm_mem_src_lo
        STA zstory_addr_l
        LDA zvm_mem_src_hi
        STA zstory_addr_m
        STZ zstory_addr_h
        JSR zstory_read8
        BNE @done
        LDA zvm_mem_dst_lo
        STA zstory_addr_l
        LDA zvm_mem_dst_hi
        STA zstory_addr_m
        STZ zstory_addr_h
        JSR zstory_write8
        BNE @done
        JSR zvm_inc_mem_src
        JSR zvm_inc_mem_dst
        JSR zvm_dec_mem_count
        BRA @copy_loop

@copy_backward:
        JSR zvm_copy_table_seek_last
@copy_backward_loop:
        LDA zvm_mem_count_lo
        ORA zvm_mem_count_hi
        BEQ @done
        LDA zvm_mem_src_lo
        STA zstory_addr_l
        LDA zvm_mem_src_hi
        STA zstory_addr_m
        STZ zstory_addr_h
        JSR zstory_read8
        BNE @done
        LDA zvm_mem_dst_lo
        STA zstory_addr_l
        LDA zvm_mem_dst_hi
        STA zstory_addr_m
        STZ zstory_addr_h
        JSR zstory_write8
        BNE @done
        JSR zvm_dec_mem_src
        JSR zvm_dec_mem_dst
        JSR zvm_dec_mem_count
        BRA @copy_backward_loop

@zero_table:
        LDA zvm_mem_count_lo
        ORA zvm_mem_count_hi
        BEQ @done
        LDA zvm_mem_src_lo
        STA zstory_addr_l
        LDA zvm_mem_src_hi
        STA zstory_addr_m
        STZ zstory_addr_h
        STZ XRAM_DATA
        JSR zstory_write8
        BNE @done
        JSR zvm_inc_mem_src
        JSR zvm_dec_mem_count
        BRA @zero_table
@done:
        RTS

zvm_copy_table_overlap:
        LDA zvm_mem_dst_hi
        CMP zvm_mem_src_hi
        BCC @forward
        BNE @dst_after_src
        LDA zvm_mem_dst_lo
        CMP zvm_mem_src_lo
        BCC @forward
        BEQ @forward

@dst_after_src:
        LDA zvm_mem_src_lo
        CLC
        ADC zvm_mem_count_lo
        STA zvm_math_res_lo
        LDA zvm_mem_src_hi
        ADC zvm_mem_count_hi
        STA zvm_math_res_hi

        LDA zvm_mem_dst_hi
        CMP zvm_math_res_hi
        BCC @backward
        BNE @forward
        LDA zvm_mem_dst_lo
        CMP zvm_math_res_lo
        BCS @forward
@backward:
        LDA #$01
        STA zvm_mem_stride
@forward:
        RTS

zvm_copy_table_seek_last:
        CLC
        LDA zvm_mem_src_lo
        ADC zvm_mem_count_lo
        STA zvm_mem_src_lo
        LDA zvm_mem_src_hi
        ADC zvm_mem_count_hi
        STA zvm_mem_src_hi
        CLC
        LDA zvm_mem_dst_lo
        ADC zvm_mem_count_lo
        STA zvm_mem_dst_lo
        LDA zvm_mem_dst_hi
        ADC zvm_mem_count_hi
        STA zvm_mem_dst_hi
        JSR zvm_dec_mem_src
        JMP zvm_dec_mem_dst

zvm_print_table:
        LDA zvm_operand_lo
        STA zvm_mem_src_lo
        LDA zvm_operand_hi
        STA zvm_mem_src_hi
        LDA zvm_operand_lo + 1
        STA zvm_table_cols
        LDA #$01
        STA zvm_table_rows
        STZ zvm_table_skip
        LDA zvm_operand_count
        CMP #$03
        BCC @rows_ready
        LDA zvm_operand_lo + 2
        STA zvm_table_rows
        LDA zvm_operand_count
        CMP #$04
        BCC @rows_ready
        LDA zvm_operand_lo + 3
        STA zvm_table_skip
@rows_ready:
        LDA zvm_table_rows
        BEQ @done
@row_loop:
        LDA zvm_table_cols
        STA zvm_tmp
@col_loop:
        LDA zvm_tmp
        BEQ @row_done
        LDA zvm_mem_src_lo
        STA zstory_addr_l
        LDA zvm_mem_src_hi
        STA zstory_addr_m
        STZ zstory_addr_h
        JSR zstory_read8
        BNE @done
        LDA XRAM_DATA
        JSR print_char
        JSR zvm_inc_mem_src
        DEC zvm_tmp
        BRA @col_loop
@row_done:
        DEC zvm_table_rows
        BEQ @done
        JSR newline
        LDA zvm_table_skip
        CLC
        ADC zvm_mem_src_lo
        STA zvm_mem_src_lo
        LDA zvm_mem_src_hi
        ADC #$00
        STA zvm_mem_src_hi
        BRA @row_loop
@done:
        RTS

zvm_inc_mem_src:
        INC zvm_mem_src_lo
        BNE @done
        INC zvm_mem_src_hi
@done:
        RTS

zvm_inc_mem_dst:
        INC zvm_mem_dst_lo
        BNE @done
        INC zvm_mem_dst_hi
@done:
        RTS

zvm_dec_mem_src:
        LDA zvm_mem_src_lo
        BNE :+
        DEC zvm_mem_src_hi
:
        DEC zvm_mem_src_lo
        RTS

zvm_dec_mem_dst:
        LDA zvm_mem_dst_lo
        BNE :+
        DEC zvm_mem_dst_hi
:
        DEC zvm_mem_dst_lo
        RTS

zvm_dec_mem_count:
        LDA zvm_mem_count_lo
        BNE :+
        DEC zvm_mem_count_hi
:
        DEC zvm_mem_count_lo
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
        LDA zvm_pc_b
        CMP zstory_filebytes_h
        BCC @in_range
        BNE zvm_bad_target
        LDA zvm_pc_h
        CMP zstory_filebytes_m
        BCC @in_range
        BNE zvm_bad_target
        LDA zvm_pc_l
        CMP zstory_filebytes_l
        BCS zvm_bad_target
@in_range:
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

; --- V6 dispatch routing into the RAM-resident NOVAZ6 segment ---------------
;
; zvm_ext_v6: EXT-table entry for opcodes that only exist (or only change
; meaning) in V6. For V6 stories the decoded opcode is forwarded to the
; segment at NZ6_ENTRY with A = NZ6_EXT_BASE + ext opnum; the segment RTSes
; back to the dispatcher exactly like a ROM handler. For v<6 the opcode is
; unsupported, same as before the table grew.
zvm_ext_v6:
        LDA zstory_version
        CMP #$06
        BNE @unsup
        LDA zvm_opnum
        CLC
        ADC #NZ6_EXT_BASE
        JMP NZ6_ENTRY
@unsup:
        JMP zvm_unsupported

; zvm_v6_maybe_route: shared prologue helper for the six VAR screen handlers
; whose semantics change in V6. Call with X = NZ6_OP_* id immediately on
; handler entry. STACK PRECONDITION: must be the handler's FIRST instruction
; pair (LDX #id / JSR here) — the V6 path pops exactly one return address,
; assuming the only frame above the handler's caller is this JSR. Any code
; (or JSR) inserted before the prologue breaks that and corrupts the stack:
;   - v<6: plain RTS — the classic handler body runs as always.
;   - V6:  pop the prologue's JSR return address (so the segment's RTS goes
;          to the handler's caller, like any handler RTS) and JMP into the
;          segment with A = id.
zvm_v6_maybe_route:
        LDA zstory_version
        CMP #$06
        BEQ @route
        RTS
@route:
        PLA                     ; drop the JSR return address (lo)
        PLA                     ; (hi) — segment now returns to handler's caller
        TXA
        JMP NZ6_ENTRY

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
        .word zvm_pop_or_catch
        .word zvm_quit
        .word zvm_new_line
        .word zvm_show_status
        .word zvm_verify
        .word zvm_unsupported
        .word zvm_branch_false
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
        .word zvm_call_1op_15
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
        .word zvm_call_2s
        .word zvm_call_2n
        .word zvm_set_colour
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
        .word zvm_call_vs
        .word zvm_erase_window
        .word zvm_erase_line
        .word zvm_set_cursor
        .word zvm_get_cursor
        .word zvm_set_text_style
        .word zvm_buffer_mode
        .word zvm_output_stream
        .word zvm_input_stream
        .word zvm_sound_effect
        .word zvm_read_char
        .word zvm_scan_table
        .word zvm_not
        .word zvm_call_vn
        .word zvm_call_vn
        .word zvm_tokenise
        .word zvm_encode_text
        .word zvm_copy_table
        .word zvm_print_table
        .word zvm_check_arg_count
zvm_var_count = (* - zvm_var_table) / 2

zvm_ext_table:
        .word zvm_save_store
        .word zvm_restore_store
        .word zvm_log_shift
        .word zvm_art_shift
        .word zvm_set_font
        .word zvm_ext_v6           ;  5 draw_picture (V6 only; v<6 unsupported)
        .word zvm_ext_v6           ;  6 picture_data (V6 only, branches)
        .word zvm_ext_v6           ;  7 erase_picture (V6 only)
        .word zvm_ext_v6           ;  8 set_margins (V6 only)
        .word zvm_false_store      ;  9 save_undo: report unavailable.
        .word zvm_false_store      ; 10 restore_undo: report unavailable.
        .word zvm_unsupported      ; 11 print_unicode (unsupported here)
        .word zvm_unsupported      ; 12 check_unicode
        .word zvm_unsupported      ; 13 set_true_colour
        .word zvm_unsupported      ; 14 (unassigned)
        .word zvm_unsupported      ; 15 (unassigned)
        .word zvm_ext_v6           ; 16 move_window
        .word zvm_ext_v6           ; 17 window_size
        .word zvm_ext_v6           ; 18 window_style
        .word zvm_ext_v6           ; 19 get_wind_prop (stores)
        .word zvm_ext_v6           ; 20 scroll_window
        .word zvm_ext_v6           ; 21 pop_stack
        .word zvm_ext_v6           ; 22 read_mouse
        .word zvm_ext_v6           ; 23 mouse_window
        .word zvm_ext_v6           ; 24 push_stack (branches)
        .word zvm_ext_v6           ; 25 put_wind_prop
        .word zvm_ext_v6           ; 26 print_form
        .word zvm_ext_v6           ; 27 make_menu (branches)
        .word zvm_ext_v6           ; 28 picture_table
        .word zvm_ext_v6           ; 29 buffer_screen (stores)
zvm_ext_count = (* - zvm_ext_table) / 2

zvm_style_color_table:
        .byte ZVM_COLOR_NORMAL, ZVM_COLOR_REVERSE_NORMAL
        .byte ZVM_COLOR_BOLD, ZVM_COLOR_REVERSE_BOLD

msg_score:
        .byte "Score: ", 0
msg_moves:
        .byte " Moves: ", 0
msg_time:
        .byte "Time: ", 0
msg_unsupported_opcode:
        .byte "UNSUPPORTED Z-OPCODE $", 0
msg_bad_target:
        .byte "BAD Z-PC $", 0
zvm_state_save_name:
        .byte "NOVAZ.SAV"
zvm_state_save_name_end:
zvm_dynamic_save_name:
        .byte "NOVAZ.DYN"
zvm_dynamic_save_name_end:
zvm_story_name:
        .byte "story.bin"
zvm_story_name_end:
zvm_save_header_prefix:
        .byte "NZSV", ZVM_SAVE_FORMAT_VERSION, ZVM_SAVE_HEADER_SIZE
        .byte <ZVM_SAVE_STATE_SIZE, >ZVM_SAVE_STATE_SIZE
zvm_a2_punct_chars:
        .byte ".,!?_#'", $22, "/", $5C, "-:()"
zvm_a2_punct_count = * - zvm_a2_punct_chars

.endif

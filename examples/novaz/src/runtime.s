; NovaZ runtime ROM.
;
; This is a runtime placeholder, not the interpreter yet. It proves the boot
; boundary we need for NovaZ/Forth-style runtimes: BASIC autoboot loads this
; ROM into the primary $C000 bank, this reset entry owns the machine, and story
; data is loaded from the mounted image into XRAM.

.setcpu "65c02"

.include "xram.inc"
.include "vtext.inc"
.include "zstory.inc"
.include "ztext.inc"
.include "zobject.inc"
.include "zvm.inc"
.include "zstory.s"
.include "ztext.s"
.include "zobject.s"
.include "zvm.s"

.define NOVAZ_WORD_MAX 80
.define NOVAZ_MORE_LEN 8

.segment "BSS"
nz_char:               .res 1
nz_word_len:           .res 1
nz_raw_input_mode:     .res 1
nz_more_enabled:       .res 1
nz_more_line_count:    .res 1
nz_auto_wrap:          .res 1
nz_saved_x:            .res 1
nz_saved_y:            .res 1
nz_saved_color:        .res 1
nz_saved_attr:         .res 1
nz_saved_flags:        .res 1
nz_word_buf:           .res NOVAZ_WORD_MAX

.segment "ZEROPAGE"
msg_ptr:        .res 2
hex_value:      .res 1
dec_hi:         .res 1
dec_lo:         .res 1
dec_digits:     .res 5
dec_first:      .res 1

.segment "CODE"

reset:
        SEI
        CLD
        LDX #$FF
        TXS

        JSR init_screen

        LDA #<msg_title
        LDY #>msg_title
        JSR print_line
        LDA #<msg_loading
        LDY #>msg_loading
        JSR print_line

        JSR zstory_load_default
        PHA
        JSR setup_text_region
        PLA
        BEQ @loaded

        LDA #<msg_load_failed
        LDY #>msg_load_failed
        JSR print_line
        BRA halt

@loaded:
        LDA #<msg_loaded
        LDY #>msg_loaded
        JSR print_line
        JSR zstory_read_header
        BEQ @header_ok
        CMP #ZSTORY_ERR_UNSUPPORTED
        BEQ @unsupported

        LDA #<msg_header_failed
        LDY #>msg_header_failed
        JSR print_line
        BRA halt

@unsupported:
        LDA #<msg_unsupported
        LDY #>msg_unsupported
        JSR print_text
        LDA zstory_version
        JSR print_u8_dec
        JSR newline
        BRA halt

@header_ok:
        JSR init_game_screen
        JSR zvm_run_until_read
        BRA halt

halt:
        WAI
        BRA halt

init_screen:
        STZ nz_word_len
        STZ nz_raw_input_mode
        STZ nz_more_enabled
        STZ nz_more_line_count
        STZ nz_auto_wrap
        JSR setup_text_region
        STZ VTEXT_CURX
        STZ VTEXT_CURY
        JSR vtext_clear_region
        JMP vtext_home

setup_text_region:
        LDA #$00
        STA VTEXT_LEFT
        STA VTEXT_TOP
        LDA #80
        STA VTEXT_WIDTH
        LDA #50
        STA VTEXT_HEIGHT
        LDA #$0F
        STA VTEXT_COLOR
        STZ VTEXT_ATTR
        LDA #(VTEXT_FLAG_WRAP | VTEXT_FLAG_SCROLL)
        STA VTEXT_FLAGS
        RTS

setup_story_region:
        LDA #$00
        STA VTEXT_LEFT
        LDA #$01
        STA VTEXT_TOP
        LDA #80
        STA VTEXT_WIDTH
        LDA #49
        STA VTEXT_HEIGHT
        LDA #$0F
        STA VTEXT_COLOR
        STZ VTEXT_ATTR
        LDA #(VTEXT_FLAG_WRAP | VTEXT_FLAG_SCROLL)
        STA VTEXT_FLAGS
        RTS

init_game_screen:
        STZ nz_word_len
        STZ nz_more_line_count
        STZ nz_auto_wrap
        LDA #$01
        STA nz_more_enabled
        JSR setup_text_region
        STZ VTEXT_CURX
        STZ VTEXT_CURY
        JSR vtext_clear_region
        JSR setup_story_region
        STZ VTEXT_CURX
        STZ VTEXT_CURY
        JMP vtext_set_cursor

print_header:
        JSR setup_text_region

        LDA #<msg_version
        LDY #>msg_version
        JSR print_text
        LDA zstory_version
        JSR print_u8_dec
        JSR newline

        LDA #<msg_release
        LDY #>msg_release
        JSR print_text
        LDA zstory_release_hi
        LDX zstory_release_lo
        JSR print_u16_dec
        JSR newline

        LDA #<msg_serial
        LDY #>msg_serial
        JSR print_text
        JSR print_serial
        JSR newline

        LDA #<msg_initial_pc
        LDY #>msg_initial_pc
        JSR print_text
        LDA zstory_initial_pc_hi
        LDX zstory_initial_pc_lo
        JSR print_hex16
        JSR newline

        LDA #<msg_static
        LDY #>msg_static
        JSR print_text
        LDA zstory_static_hi
        LDX zstory_static_lo
        JSR print_hex16
        JSR newline

        LDA #<msg_globals
        LDY #>msg_globals
        JSR print_text
        LDA zstory_globals_hi
        LDX zstory_globals_lo
        JSR print_hex16
        JSR newline

        LDA #<msg_dictionary
        LDY #>msg_dictionary
        JSR print_text
        LDA zstory_dictionary_hi
        LDX zstory_dictionary_lo
        JSR print_hex16
        JSR newline

        LDA #<msg_object_table
        LDY #>msg_object_table
        JSR print_text
        LDA zstory_object_hi
        LDX zstory_object_lo
        JSR print_hex16
        JSR newline

        LDA #<msg_abbrev
        LDY #>msg_abbrev
        JSR print_text
        LDA zstory_abbrev_hi
        LDX zstory_abbrev_lo
        JSR print_hex16
        JSR newline

        LDA #<msg_checksum
        LDY #>msg_checksum
        JSR print_text
        LDA zstory_checksum_hi
        LDX zstory_checksum_lo
        JSR print_hex16
        JSR newline

        LDA #<msg_object1
        LDY #>msg_object1
        JSR print_text
        LDA #$01
        JSR zobject_print_short_name
        JSR newline

        LDA #<msg_platform_ready
        LDY #>msg_platform_ready
        JSR print_line
        RTS

; A/Y = pointer to null-terminated string.
print_line:
        JSR print_text
        JMP newline

print_text:
        STA msg_ptr
        STY msg_ptr + 1
        LDY #$00
@loop:
        LDA (msg_ptr),Y
        BEQ @done
        PHY
        JSR print_char
        PLY
        INY
        BNE @loop
        INC msg_ptr + 1
        BRA @loop
@done:
        RTS

print_char:
        STA nz_char
        PHX
        PHY
        LDA zvm_stream_flags
        AND #ZVM_STREAM_3_ACTIVE
        BEQ @screen
        LDA nz_char
        JSR zvm_stream3_put_char
@screen:
        LDA zvm_stream_flags
        AND #ZVM_STREAM_SCREEN_OFF
        BNE @done
        LDA nz_char
        JSR nz_screen_put_char
@done:
        PLY
        PLX
        RTS

nz_screen_put_char:
        STA nz_char
        LDA nz_raw_input_mode
        BNE nz_screen_put_raw_saved
        LDA VTEXT_FLAGS
        AND #VTEXT_FLAG_WRAP
        BEQ nz_screen_put_raw_saved

        LDA nz_char
        CMP #$20
        BEQ @space
        CMP #$0D
        BEQ @control
        CMP #$0A
        BEQ @control
        CMP #$08
        BEQ @control
        CMP #$0C
        BEQ @control
        CMP #$20
        BCC @control

        LDX nz_word_len
        CPX #NOVAZ_WORD_MAX
        BCC @store_word
        LDA nz_char
        PHA
        JSR nz_screen_flush_word
        PLA
        JMP nz_screen_put_raw

@store_word:
        LDA nz_char
        STA nz_word_buf,X
        INC nz_word_len
        RTS

@space:
        JSR nz_screen_flush_word
        LDA nz_auto_wrap
        BEQ @emit_space
        LDA VTEXT_CURX
        BNE @emit_space
        STZ nz_auto_wrap
        RTS
@emit_space:
        LDA #' '
        JMP nz_screen_put_raw

@control:
        LDA nz_char
        PHA
        JSR nz_screen_flush_word
        PLA
        JMP nz_screen_put_raw

nz_screen_put_raw_saved:
        LDA nz_char

nz_screen_put_raw:
        STA nz_char
        CMP #$0A
        BEQ nz_screen_linefeed
        CMP #$0D
        BEQ @vtext_control
        CMP #$20
        BCC @vtext_control

        LDA VTEXT_FLAGS
        AND #VTEXT_FLAG_WRAP
        BEQ @vtext_printable
        LDA VTEXT_WIDTH
        BEQ @vtext_printable
        DEC
        CMP VTEXT_CURX
        BNE @vtext_printable

        LDA VTEXT_FLAGS
        STA nz_saved_flags
        AND #($FF - VTEXT_FLAG_WRAP)
        STA VTEXT_FLAGS
        LDA nz_char
        STA VTEXT_CHAR
        JSR vtext_put_char
        PHA
        LDA nz_saved_flags
        STA VTEXT_FLAGS
        PLA
        BNE @done
        JSR nz_screen_linefeed
        LDA #$01
        STA nz_auto_wrap
        JMP vtext_ok

@vtext_printable:
        STZ nz_auto_wrap
        LDA nz_char
        STA VTEXT_CHAR
        JMP vtext_put_char

@vtext_control:
        STZ nz_auto_wrap
        LDA nz_char
        STA VTEXT_CHAR
        JMP vtext_put_char

@done:
        RTS

nz_screen_linefeed:
        STZ nz_auto_wrap
        JSR nz_screen_maybe_more
        LDA #$0A
        STA VTEXT_CHAR
        JSR vtext_put_char
        BNE @done
        JSR nz_screen_count_line
@done:
        RTS

nz_screen_flush_word:
        LDA nz_word_len
        BEQ @done
        LDA nz_raw_input_mode
        BNE @emit
        LDA VTEXT_FLAGS
        AND #VTEXT_FLAG_WRAP
        BEQ @emit
        LDA VTEXT_CURX
        BEQ @emit
        LDA VTEXT_WIDTH
        SEC
        SBC VTEXT_CURX
        CMP nz_word_len
        BCS @emit
        JSR nz_screen_linefeed

@emit:
        LDX #$00
@loop:
        CPX nz_word_len
        BCS @clear
        LDA nz_word_buf,X
        PHX
        JSR nz_screen_put_raw
        PLX
        INX
        BRA @loop
@clear:
        STZ nz_word_len
@done:
        RTS

nz_screen_count_line:
        LDA nz_more_enabled
        BEQ @done
        LDA VTEXT_FLAGS
        AND #VTEXT_FLAG_SCROLL
        BEQ @done
        LDA nz_more_line_count
        CMP #$FF
        BEQ @done
        INC nz_more_line_count
@done:
        RTS

nz_screen_maybe_more:
        LDA nz_more_enabled
        BEQ @done
        LDA nz_raw_input_mode
        BNE @done
        LDA VTEXT_FLAGS
        AND #VTEXT_FLAG_SCROLL
        BEQ @done
        LDA VTEXT_HEIGHT
        CMP #$02
        BCC @done
        SEC
        SBC #$01
        CMP VTEXT_CURY
        BNE @done
        STA nz_char
        LDA nz_more_line_count
        CMP nz_char
        BCC @done
        JSR nz_screen_more_prompt
        STZ nz_more_line_count
@done:
        RTS

nz_screen_more_prompt:
        LDA VTEXT_CURX
        STA nz_saved_x
        LDA VTEXT_CURY
        STA nz_saved_y
        LDA VTEXT_COLOR
        STA nz_saved_color
        LDA VTEXT_ATTR
        STA nz_saved_attr
        LDA VTEXT_FLAGS
        STA nz_saved_flags

        LDA #$F0
        STA VTEXT_COLOR
        STZ VTEXT_ATTR
        STZ VTEXT_FLAGS
        JSR nz_screen_more_position
        LDX #$00
@draw_loop:
        LDA msg_more,X
        BEQ @draw_done
        STA VTEXT_CHAR
        PHX
        JSR vtext_put_char
        PLX
        INX
        BRA @draw_loop

@draw_done:
        JSR nz_screen_restore_saved
@wait:
        LDA VGC_CHARIN
        BEQ @wait
        CMP #$0D
        BEQ @erase
        CMP #$0A
        BNE @wait

@erase:
        LDA nz_saved_color
        STA VTEXT_COLOR
        LDA nz_saved_attr
        STA VTEXT_ATTR
        STZ VTEXT_FLAGS
        JSR nz_screen_more_position
        LDX #NOVAZ_MORE_LEN
@erase_loop:
        LDA #' '
        STA VTEXT_CHAR
        PHX
        JSR vtext_put_char
        PLX
        DEX
        BNE @erase_loop
        JMP nz_screen_restore_saved

nz_screen_more_position:
        LDA VTEXT_WIDTH
        SEC
        SBC #NOVAZ_MORE_LEN
        BCS @x_ok
        LDA #$00
@x_ok:
        STA VTEXT_CURX
        LDA VTEXT_HEIGHT
        SEC
        SBC #$01
        STA VTEXT_CURY
        JMP vtext_set_cursor

nz_screen_restore_saved:
        LDA nz_saved_x
        STA VTEXT_CURX
        LDA nz_saved_y
        STA VTEXT_CURY
        LDA nz_saved_color
        STA VTEXT_COLOR
        LDA nz_saved_attr
        STA VTEXT_ATTR
        LDA nz_saved_flags
        STA VTEXT_FLAGS
        JMP vtext_set_cursor

newline:
        LDA #$0D
        JSR print_char
        LDA #$0A
        JMP print_char

print_hex8:
        STA hex_value
        LSR
        LSR
        LSR
        LSR
        JSR print_nibble
        LDA hex_value
        AND #$0F
print_nibble:
        CMP #$0A
        BCC @digit
        CLC
        ADC #('A' - '0' - 10)
@digit:
        CLC
        ADC #'0'
        JMP print_char

print_hex16:
        PHA
        JSR print_hex8
        TXA
        JSR print_hex8
        PLA
        RTS

print_u8_dec:
        TAX
        LDA #$00
        JMP print_u16_dec

; A:X = unsigned 16-bit value. Prints without leading spaces.
print_u16_dec:
        STA dec_hi
        STX dec_lo

        LDX #$00
@d10k:
        SEC
        LDA dec_lo
        SBC #<10000
        PHA
        LDA dec_hi
        SBC #>10000
        BCC @d10k_done
        STA dec_hi
        PLA
        STA dec_lo
        INX
        BCS @d10k
@d10k_done:
        PLA
        STX dec_digits

        LDX #$00
@d1k:
        SEC
        LDA dec_lo
        SBC #<1000
        PHA
        LDA dec_hi
        SBC #>1000
        BCC @d1k_done
        STA dec_hi
        PLA
        STA dec_lo
        INX
        BCS @d1k
@d1k_done:
        PLA
        STX dec_digits + 1

        LDX #$00
@d100:
        SEC
        LDA dec_lo
        SBC #100
        BCC @d100_done
        STA dec_lo
        INX
        BNE @d100
@d100_done:
        STX dec_digits + 2

        LDX #$00
@d10:
        LDA dec_lo
        CMP #10
        BCC @d10_done
        SBC #10
        STA dec_lo
        INX
        BNE @d10
@d10_done:
        STX dec_digits + 3
        LDA dec_lo
        STA dec_digits + 4

        LDX #$00
@find:
        CPX #$04
        BCS @print
        LDA dec_digits,X
        BNE @print
        INX
        BNE @find
@print:
        STX dec_first
        LDX dec_first
@loop:
        LDA dec_digits,X
        ORA #'0'
        JSR print_char
        INX
        CPX #$05
        BCC @loop
        RTS

print_serial:
        LDX #$00
@loop:
        LDA zstory_serial,X
        JSR print_char
        INX
        CPX #$06
        BCC @loop
        RTS

.segment "RODATA"

msg_title:
        .byte "NOVAZ RUNTIME", 0
msg_loading:
        .byte "LOADING story.bin INTO XRAM...", 0
msg_loaded:
        .byte "STORY LOADED.", 0
msg_load_failed:
        .byte "STORY LOAD FAILED.", 0
msg_header_failed:
        .byte "STORY HEADER READ FAILED.", 0
msg_unsupported:
        .byte "UNSUPPORTED Z-MACHINE VERSION: ", 0
msg_version:
        .byte "Z-MACHINE VERSION: ", 0
msg_release:
        .byte "RELEASE: ", 0
msg_serial:
        .byte "SERIAL: ", 0
msg_initial_pc:
        .byte "INITIAL PC: $", 0
msg_static:
        .byte "STATIC MEMORY: $", 0
msg_globals:
        .byte "GLOBALS: $", 0
msg_dictionary:
        .byte "DICTIONARY: $", 0
msg_object_table:
        .byte "OBJECT TABLE: $", 0
msg_abbrev:
        .byte "ABBREVIATIONS: $", 0
msg_checksum:
        .byte "CHECKSUM: $", 0
msg_object1:
        .byte "OBJECT #1: ", 0
msg_platform_ready:
        .byte "NOVA Z-MEMORY PLATFORM READY.", 0
msg_more:
        .byte "[ MORE ]", 0

.include "xram.s"
.include "vtext.s"

.segment "VECTORS"
        .word reset
        .word reset
        .word reset

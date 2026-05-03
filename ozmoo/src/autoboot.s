; Nova Ozmoo autoboot shell.
;
; This is not the interpreter yet. It proves the disk/application boundary by
; loading STORY.BIN from the mounted NDI directly into XRAM, then reading Z-story
; header bytes back through the shared flat-address XRAM runtime.

.setcpu "65c02"

.include "xram.inc"
.include "vtext.inc"

.segment "ZEROPAGE"
msg_ptr:        .res 2
hex_value:      .res 1

.segment "HEADER"
        .byte $00, $72

.segment "CODE"

start:
        CLD
        JSR init_screen

        LDA #<msg_title
        LDY #>msg_title
        JSR print_line
        LDA #<msg_loading
        LDY #>msg_loading
        JSR print_line

        JSR load_story
        BEQ @loaded

        LDA #<msg_load_failed
        LDY #>msg_load_failed
        JSR print_line
        BRA halt

@loaded:
        LDA #<msg_loaded
        LDY #>msg_loaded
        JSR print_line
        JSR print_header

halt:
        WAI
        BRA halt

init_screen:
        LDA #$00
        STA VTEXT_LEFT
        STA VTEXT_TOP
        STA VTEXT_CURX
        STA VTEXT_CURY
        LDA #80
        STA VTEXT_WIDTH
        LDA #50
        STA VTEXT_HEIGHT
        LDA #$0F
        STA VTEXT_COLOR
        STZ VTEXT_ATTR
        LDA #(VTEXT_FLAG_WRAP | VTEXT_FLAG_SCROLL)
        STA VTEXT_FLAGS
        JSR vtext_clear_region
        JMP vtext_home

load_story:
        LDA #<story_name
        STA XRAM_NAMEPTR_L
        LDA #>story_name
        STA XRAM_NAMEPTR_H
        LDA #(story_name_end - story_name)
        STA XRAM_NAMELEN

        STZ XRAM_ADDRL
        STZ XRAM_ADDRM
        STZ XRAM_ADDRH
        STZ XRAM_LENL
        STZ XRAM_LENH
        JMP xram_xload

print_header:
        LDA #<msg_version
        LDY #>msg_version
        JSR print_text
        STZ XRAM_ADDRL
        STZ XRAM_ADDRM
        STZ XRAM_ADDRH
        JSR xram_read8
        BNE @done
        LDA #'Z'
        JSR print_char
        LDA XRAM_DATA
        JSR print_hex8
        JSR newline

        LDA #<msg_release
        LDY #>msg_release
        JSR print_text
        LDA #$02
        STA XRAM_ADDRL
        STZ XRAM_ADDRM
        STZ XRAM_ADDRH
        JSR xram_read8
        BNE @done
        LDA XRAM_DATA
        JSR print_hex8
        INC XRAM_ADDRL
        JSR xram_read8
        BNE @done
        LDA XRAM_DATA
        JSR print_hex8
        JSR newline

        LDA #<msg_smoke_ok
        LDY #>msg_smoke_ok
        JSR print_line
@done:
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
        STA VTEXT_CHAR
        JMP vtext_put_char

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

.segment "RODATA"

story_name:
        .byte "STORY.BIN"
story_name_end:

msg_title:
        .byte "NOVA OZMOO Z-MACHINE BOOT", 0
msg_loading:
        .byte "LOADING STORY.BIN INTO XRAM...", 0
msg_loaded:
        .byte "STORY LOADED.", 0
msg_load_failed:
        .byte "STORY LOAD FAILED. BUILD WITH: make -C ozmoo ndi PROJECT=zork-i", 0
msg_version:
        .byte "STORY VERSION: ", 0
msg_release:
        .byte "RELEASE: $", 0
msg_smoke_ok:
        .byte "XRAM HEADER SMOKE PASSED.", 0

.include "xram.s"
.include "vtext.s"

; Nova Ozmoo packed Z-string decoder.

.include "zstory.inc"
.include "ztext.inc"

.ifndef NOVA_OZMOO_ZTEXT_IMPLEMENTATION_INCLUDED
NOVA_OZMOO_ZTEXT_IMPLEMENTATION_INCLUDED = 1

.segment "ZEROPAGE"

ztext_word_limit:      .res 1
ztext_word_hi:         .res 1
ztext_word_lo:         .res 1
ztext_zchars:          .res 3
ztext_index:           .res 1
ztext_done:            .res 1
ztext_alphabet:        .res 1
ztext_tmp:             .res 1
ztext_escape_count:    .res 1
ztext_escape_value:    .res 1
ztext_abbrev_command:  .res 1
ztext_abbrev_index:    .res 1
ztext_abbrev_offset:   .res 1

.segment "CODE"

.export ztext_print_packed

; Print a packed Z-string at zstory_addr_l/m/h.
; If ztext_word_limit is ZTEXT_UNLIMITED, decode until the high stop bit.
; Otherwise decode at most ztext_word_limit words, which is needed for object
; short names because their stored length is authoritative.
ztext_print_packed:
        STZ ztext_index
        STZ ztext_done
        STZ ztext_alphabet
        STZ ztext_escape_count
        STZ ztext_escape_value
        STZ ztext_abbrev_command

@next:
        JSR ztext_next_zchar
        BCS @done
        JSR ztext_emit_zchar
        BRA @next
@done:
        RTS

ztext_next_zchar:
        LDA ztext_index
        BNE @have_word

        LDA ztext_done
        BNE @eof

        LDA ztext_word_limit
        CMP #ZTEXT_UNLIMITED
        BEQ @read_word
        CMP #$00
        BEQ @eof
        DEC ztext_word_limit

@read_word:
        JSR zstory_read16
        BNE @eof
        LDA zstory_word_hi
        STA ztext_word_hi
        LDA zstory_word_lo
        STA ztext_word_lo
        JSR ztext_unpack_word

@have_word:
        LDX ztext_index
        LDA ztext_zchars,X
        INX
        CPX #$03
        BCC @store_index
        LDX #$00
@store_index:
        STX ztext_index
@ok:
        CLC
        RTS

@eof:
        SEC
        RTS

ztext_unpack_word:
        LDA ztext_word_hi
        AND #$80
        STA ztext_done

        LDA ztext_word_hi
        AND #$7C
        LSR
        LSR
        STA ztext_zchars

        LDA ztext_word_hi
        AND #$03
        ASL
        ASL
        ASL
        STA ztext_tmp
        LDA ztext_word_lo
        LSR
        LSR
        LSR
        LSR
        LSR
        ORA ztext_tmp
        STA ztext_zchars + 1

        LDA ztext_word_lo
        AND #$1F
        STA ztext_zchars + 2
        RTS

ztext_emit_zchar:
        PHA
        LDA ztext_escape_count
        BEQ @normal
        PLA
        JSR ztext_accumulate_escape
        RTS

@normal:
        PLA
        LDX ztext_abbrev_command
        BEQ @not_abbrev_index
        JSR ztext_print_abbreviation
        STZ ztext_abbrev_command
        STZ ztext_alphabet
        RTS

@not_abbrev_index:
        CMP #$00
        BNE @not_space
        LDA #' '
        JMP print_char

@not_space:
        CMP #$04
        BNE @not_shift_a1
        LDA #$01
        STA ztext_alphabet
        RTS

@not_shift_a1:
        CMP #$05
        BNE @not_shift_a2
        LDA #$02
        STA ztext_alphabet
        RTS

@not_shift_a2:
        CMP #$06
        BCC @abbrev
        LDX ztext_alphabet
        CPX #$02
        BNE @plain
        CMP #$06
        BNE @a2_not_escape
        LDA #$02
        STA ztext_escape_count
        STZ ztext_escape_value
        STZ ztext_alphabet
        RTS
@a2_not_escape:
        CMP #$07
        BNE @plain
        JSR newline
        STZ ztext_alphabet
        RTS

@plain:
        SEC
        SBC #$06
        STA ztext_tmp
        LDA ztext_alphabet
        BEQ @alphabet_ready
        CMP #$01
        BNE @a2_offset
        LDA ztext_tmp
        CLC
        ADC #26
        BRA @lookup
@a2_offset:
        LDA ztext_tmp
        CLC
        ADC #52
        BRA @lookup
@alphabet_ready:
        LDA ztext_tmp
@lookup:
        TAX
        LDA ztext_default_alphabet,X
        JSR print_char
        STZ ztext_alphabet
        RTS

@abbrev:
        STA ztext_abbrev_command
        STZ ztext_alphabet
        RTS

ztext_accumulate_escape:
        ; A is one 5-bit escape zchar. Two of them form a 10-bit ZSCII code.
        DEC ztext_escape_count
        BNE @first
        ORA ztext_escape_value
        JSR print_char
        RTS

@first:
        ASL
        ASL
        ASL
        ASL
        ASL
        STA ztext_escape_value
        RTS

; A = abbreviation zchar index, ztext_abbrev_command = 1..3.
ztext_print_abbreviation:
        STA ztext_abbrev_index
        LDA zstory_addr_l
        PHA
        LDA zstory_addr_m
        PHA
        LDA zstory_addr_h
        PHA
        LDA ztext_word_limit
        PHA
        LDA ztext_word_hi
        PHA
        LDA ztext_word_lo
        PHA
        LDA ztext_zchars
        PHA
        LDA ztext_zchars + 1
        PHA
        LDA ztext_zchars + 2
        PHA
        LDA ztext_index
        PHA
        LDA ztext_done
        PHA
        LDA ztext_alphabet
        PHA
        LDA ztext_escape_count
        PHA
        LDA ztext_escape_value
        PHA
        LDA ztext_abbrev_command
        PHA

        LDA ztext_abbrev_command
        DEC
        ASL
        ASL
        ASL
        ASL
        ASL
        STA ztext_abbrev_offset
        LDA ztext_abbrev_index
        CLC
        ADC ztext_abbrev_offset
        ASL
        STA ztext_abbrev_offset

        LDA zstory_abbrev_lo
        CLC
        ADC ztext_abbrev_offset
        STA zstory_addr_l
        LDA zstory_abbrev_hi
        ADC #$00
        STA zstory_addr_m
        STZ zstory_addr_h
        JSR zstory_read16
        BNE @restore

        ; Abbreviation table entries are packed string addresses.
        LDA zstory_word_lo
        ASL
        STA zstory_addr_l
        LDA zstory_word_hi
        ROL
        STA zstory_addr_m
        LDA #$00
        ROL
        STA zstory_addr_h
        LDA #ZTEXT_UNLIMITED
        STA ztext_word_limit
        STZ ztext_done
        STZ ztext_index
        STZ ztext_alphabet
        STZ ztext_escape_count
        STZ ztext_escape_value
        STZ ztext_abbrev_command
        JSR ztext_print_packed

@restore:
        PLA
        STA ztext_abbrev_command
        PLA
        STA ztext_escape_value
        PLA
        STA ztext_escape_count
        PLA
        STA ztext_alphabet
        PLA
        STA ztext_done
        PLA
        STA ztext_index
        PLA
        STA ztext_zchars + 2
        PLA
        STA ztext_zchars + 1
        PLA
        STA ztext_zchars
        PLA
        STA ztext_word_lo
        PLA
        STA ztext_word_hi
        PLA
        STA ztext_word_limit
        PLA
        STA zstory_addr_h
        PLA
        STA zstory_addr_m
        PLA
        STA zstory_addr_l
        RTS

.segment "RODATA"

ztext_default_alphabet:
        .byte "abcdefghijklmnopqrstuvwxyz"
        .byte "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        .byte " ", $0D, "0123456789.,!?_#'", $22, "/", $5C, "-:()"

.endif

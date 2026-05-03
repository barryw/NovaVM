; Nova Ozmoo Z-story/XRAM interface.

.include "xram.inc"
.include "zstory.inc"

.ifndef NOVA_OZMOO_ZSTORY_IMPLEMENTATION_INCLUDED
NOVA_OZMOO_ZSTORY_IMPLEMENTATION_INCLUDED = 1

.segment "ZEROPAGE"

zstory_addr_l:         .res 1
zstory_addr_m:         .res 1
zstory_addr_h:         .res 1
zstory_word_hi:        .res 1
zstory_word_lo:        .res 1
zstory_version:        .res 1
zstory_release_hi:     .res 1
zstory_release_lo:     .res 1
zstory_highmem_hi:     .res 1
zstory_highmem_lo:     .res 1
zstory_initial_pc_hi:  .res 1
zstory_initial_pc_lo:  .res 1
zstory_dictionary_hi:  .res 1
zstory_dictionary_lo:  .res 1
zstory_object_hi:      .res 1
zstory_object_lo:      .res 1
zstory_globals_hi:     .res 1
zstory_globals_lo:     .res 1
zstory_static_hi:      .res 1
zstory_static_lo:      .res 1
zstory_abbrev_hi:      .res 1
zstory_abbrev_lo:      .res 1
zstory_filelen_hi:     .res 1
zstory_filelen_lo:     .res 1
zstory_checksum_hi:    .res 1
zstory_checksum_lo:    .res 1
zstory_serial:         .res 6

.segment "CODE"

.export zstory_load_default
.export zstory_read_header
.export zstory_read_header_byte
.export zstory_read_header_word
.export zstory_read8
.export zstory_read16

; Load the default story file into flat XRAM at $000000.
; Returns A=0 on success, A=1 on FIO/XRAM error.
zstory_load_default:
        LDA #<zstory_default_name
        STA XRAM_NAMEPTR_L
        LDA #>zstory_default_name
        STA XRAM_NAMEPTR_H
        LDA #(zstory_default_name_end - zstory_default_name)
        STA XRAM_NAMELEN

        STZ XRAM_ADDRL
        STZ XRAM_ADDRM
        STZ XRAM_ADDRH
        STZ XRAM_LENL
        STZ XRAM_LENH
        JMP xram_xload

; Read and decode the Z-machine header from XRAM.
; Returns:
;   A=0 when the header was decoded and the version is supported.
;   A=1 on XRAM read error.
;   A=2 for unsupported story versions.
zstory_read_header:
        LDA #ZHEADER_VERSION
        JSR zstory_read_header_byte
        BEQ :+
        JMP @io_error
:
        LDA XRAM_DATA
        STA zstory_version

        JSR zstory_validate_version
        BEQ :+
        RTS
:

        LDA #ZHEADER_RELEASE
        JSR zstory_read_header_word
        BEQ :+
        JMP @io_error
:
        JSR zstory_store_release

        LDA #ZHEADER_HIGH_MEMORY
        JSR zstory_read_header_word
        BEQ :+
        JMP @io_error
:
        JSR zstory_store_highmem

        LDA #ZHEADER_INITIAL_PC
        JSR zstory_read_header_word
        BEQ :+
        JMP @io_error
:
        JSR zstory_store_initial_pc

        LDA #ZHEADER_DICTIONARY
        JSR zstory_read_header_word
        BEQ :+
        JMP @io_error
:
        JSR zstory_store_dictionary

        LDA #ZHEADER_OBJECT_TABLE
        JSR zstory_read_header_word
        BEQ :+
        JMP @io_error
:
        JSR zstory_store_object

        LDA #ZHEADER_GLOBALS
        JSR zstory_read_header_word
        BEQ :+
        JMP @io_error
:
        JSR zstory_store_globals

        LDA #ZHEADER_STATIC_MEMORY
        JSR zstory_read_header_word
        BEQ :+
        JMP @io_error
:
        JSR zstory_store_static

        LDA #ZHEADER_ABBREVIATIONS
        JSR zstory_read_header_word
        BEQ :+
        JMP @io_error
:
        JSR zstory_store_abbrev

        LDA #ZHEADER_FILE_LENGTH
        JSR zstory_read_header_word
        BEQ :+
        JMP @io_error
:
        JSR zstory_store_filelen

        LDA #ZHEADER_CHECKSUM
        JSR zstory_read_header_word
        BEQ :+
        JMP @io_error
:
        JSR zstory_store_checksum

        LDX #$00
@serial:
        TXA
        CLC
        ADC #ZHEADER_SERIAL
        PHX
        JSR zstory_read_header_byte
        PLX
        CMP #ZSTORY_ERR_NONE
        BEQ :+
        JMP @io_error
:
        LDA XRAM_DATA
        STA zstory_serial,X
        INX
        CPX #$06
        BCC @serial

        LDA #ZSTORY_ERR_NONE
@done:
        RTS

@io_error:
        LDA #ZSTORY_ERR_IO
        RTS

zstory_validate_version:
        LDA zstory_version
        CMP #$01
        BCC @unsupported
        CMP #$06
        BEQ @unsupported
        CMP #$09
        BCS @unsupported
        LDA #ZSTORY_ERR_NONE
        RTS
@unsupported:
        LDA #ZSTORY_ERR_UNSUPPORTED
        RTS

; A = header byte offset. Returns XRAM_DATA and A=0 on success.
zstory_read_header_byte:
        STA XRAM_ADDRL
        STZ XRAM_ADDRM
        STZ XRAM_ADDRH
        JMP xram_read8

; A = header word offset. Returns zstory_word_hi/lo and A=0 on success.
zstory_read_header_word:
        STA XRAM_ADDRL
        STZ XRAM_ADDRM
        STZ XRAM_ADDRH
        JSR xram_read8
        BNE @done
        LDA XRAM_DATA
        STA zstory_word_hi
        INC XRAM_ADDRL
        JSR xram_read8
        BNE @done
        LDA XRAM_DATA
        STA zstory_word_lo
        LDA #ZSTORY_ERR_NONE
@done:
        RTS

; Read a byte from zstory_addr_l/m/h into XRAM_DATA.
zstory_read8:
        LDA zstory_addr_l
        STA XRAM_ADDRL
        LDA zstory_addr_m
        STA XRAM_ADDRM
        LDA zstory_addr_h
        STA XRAM_ADDRH
        JMP xram_read8

; Read a big-endian word from zstory_addr_l/m/h into zstory_word_hi/lo.
; The address is advanced by two bytes.
zstory_read16:
        JSR zstory_read8
        BNE @done
        LDA XRAM_DATA
        STA zstory_word_hi
        JSR zstory_inc_addr
        JSR zstory_read8
        BNE @done
        LDA XRAM_DATA
        STA zstory_word_lo
        JSR zstory_inc_addr
        LDA #ZSTORY_ERR_NONE
@done:
        RTS

zstory_inc_addr:
        INC zstory_addr_l
        BNE @done
        INC zstory_addr_m
        BNE @done
        INC zstory_addr_h
@done:
        RTS

zstory_store_release:
        LDA zstory_word_hi
        STA zstory_release_hi
        LDA zstory_word_lo
        STA zstory_release_lo
        RTS

zstory_store_highmem:
        LDA zstory_word_hi
        STA zstory_highmem_hi
        LDA zstory_word_lo
        STA zstory_highmem_lo
        RTS

zstory_store_initial_pc:
        LDA zstory_word_hi
        STA zstory_initial_pc_hi
        LDA zstory_word_lo
        STA zstory_initial_pc_lo
        RTS

zstory_store_dictionary:
        LDA zstory_word_hi
        STA zstory_dictionary_hi
        LDA zstory_word_lo
        STA zstory_dictionary_lo
        RTS

zstory_store_object:
        LDA zstory_word_hi
        STA zstory_object_hi
        LDA zstory_word_lo
        STA zstory_object_lo
        RTS

zstory_store_globals:
        LDA zstory_word_hi
        STA zstory_globals_hi
        LDA zstory_word_lo
        STA zstory_globals_lo
        RTS

zstory_store_static:
        LDA zstory_word_hi
        STA zstory_static_hi
        LDA zstory_word_lo
        STA zstory_static_lo
        RTS

zstory_store_abbrev:
        LDA zstory_word_hi
        STA zstory_abbrev_hi
        LDA zstory_word_lo
        STA zstory_abbrev_lo
        RTS

zstory_store_filelen:
        LDA zstory_word_hi
        STA zstory_filelen_hi
        LDA zstory_word_lo
        STA zstory_filelen_lo
        RTS

zstory_store_checksum:
        LDA zstory_word_hi
        STA zstory_checksum_hi
        LDA zstory_word_lo
        STA zstory_checksum_lo
        RTS

.segment "RODATA"

zstory_default_name:
        .byte "story.bin"
zstory_default_name_end:

.endif

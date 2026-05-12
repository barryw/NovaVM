; NovaZ Z-story/XRAM interface.

.include "xram.inc"
.include "pager.inc"
.include "zstory.inc"

.ifndef NOVA_NOVAZ_ZSTORY_IMPLEMENTATION_INCLUDED
NOVA_NOVAZ_ZSTORY_IMPLEMENTATION_INCLUDED = 1

ZSTORY_HEADER_LOAD_LEN = $40
ZSTORY_PAGE_SIZE_HI    = $04        ; 1024-byte pages.
ZSTORY_CACHE_SLOTS     = 16

.segment "ZEROPAGE"

zstory_addr_l:         .res 1
zstory_addr_m:         .res 1
zstory_addr_h:         .res 1
zstory_word_hi:        .res 1
zstory_word_lo:        .res 1
zstory_version:        .res 1
zstory_flags1:         .res 1
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
zstory_filebytes_h:    .res 1
zstory_filebytes_m:    .res 1
zstory_filebytes_l:    .res 1
zstory_checksum_hi:    .res 1
zstory_checksum_lo:    .res 1
zstory_page_no:        .res 1
zstory_slot:           .res 1
zstory_next_slot:      .res 1
zstory_tmp:            .res 1
zstory_serial:         .res 6

.segment "BSS"

zstory_cache_valid:    .res ZSTORY_CACHE_SLOTS
zstory_cache_page:     .res ZSTORY_CACHE_SLOTS
zstory_dynamic_base_l: .res 1
zstory_dynamic_base_m: .res 1
zstory_dynamic_base_h: .res 1
zstory_cache_base_l:   .res 1
zstory_cache_base_m:   .res 1
zstory_cache_base_h:   .res 1

.segment "CODE"

.export zstory_load_default
.export zstory_read_header
.export zstory_read_header_byte
.export zstory_read_header_word
.export zstory_read8
.export zstory_read16
.export zstory_write8
.export zstory_write16
.export zstory_set_dynamic_xram_origin

; Initialize fixed XRAM workspaces and load the default story header.
; Returns A=0 on success, A=1 on FIO/XRAM error.
zstory_load_default:
        JSR zstory_init_fixed_xram_bases
        JSR zstory_clear_cache
        LDA #<zstory_default_name
        STA XRAM_NAMEPTR_L
        LDA #>zstory_default_name
        STA XRAM_NAMEPTR_H
        LDA #(zstory_default_name_end - zstory_default_name)
        STA XRAM_NAMELEN

        JSR zstory_set_dynamic_xram_origin
        LDA #ZSTORY_HEADER_LOAD_LEN
        STA XRAM_LENL
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

        LDA #ZHEADER_FLAGS1
        JSR zstory_read_header_byte
        BEQ :+
        JMP @io_error
:
        LDA XRAM_DATA
        STA zstory_flags1

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
        JSR zstory_load_dynamic
        BEQ :+
        JMP @io_error
:
        JSR zstory_configure_flags1
        BEQ :+
        JMP @io_error
:

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
        BCS @unsupported
        LDA #ZSTORY_ERR_NONE
        RTS
@unsupported:
        LDA #ZSTORY_ERR_UNSUPPORTED
        RTS

zstory_configure_flags1:
        LDA zstory_version
        CMP #$03
        BEQ @v3
        CMP #$04
        BCS @v4_plus
        JMP @ok
@v3:
        LDA zstory_flags1
        AND #%00000110          ; Preserve time-game and split-file story bits.
        ORA #%00100000          ; Status line yes, upper window yes, fixed pitch.
        STA zstory_flags1
        STA XRAM_DATA
        LDA #ZHEADER_FLAGS1
        STA zstory_addr_l
        STZ zstory_addr_m
        STZ zstory_addr_h
        JMP zstory_write8
@v4_plus:
        LDA #$20                ; screen height in lines
        LDX #50
        JSR zstory_write_header_byte_value
        BNE @done

        LDA #$21                ; screen width in characters
        LDX #80
        JSR zstory_write_header_byte_value
        BNE @done

        LDA #$22                ; screen width in units
        LDX #80
        JSR zstory_write_header_word_value
        BNE @done

        LDA #$24                ; screen height in units
        LDX #50
        JSR zstory_write_header_word_value
        BNE @done

        LDA #$26                ; font height in units
        LDX #$01
        JSR zstory_write_header_byte_value
        BNE @done

        LDA #$27                ; font width in units
        LDX #$01
        JSR zstory_write_header_byte_value
        BNE @done

        LDA #$2C                ; default background colour
        LDX #$02
        JSR zstory_write_header_byte_value
        BNE @done

        LDA #$2D                ; default foreground colour
        LDX #$09
        JSR zstory_write_header_byte_value
        BNE @done

        LDA #$32                ; Standards revision major
        LDX #$01
        JSR zstory_write_header_byte_value
        BNE @done

        LDA #$1E                ; interpreter number
        LDX #$06
        JSR zstory_write_header_byte_value
        BNE @done

        LDA #$1F                ; interpreter version
        LDX #'N'
        JSR zstory_write_header_byte_value
        BNE @done

        LDA #$33                ; Standards revision minor
        LDX #$01
        JSR zstory_write_header_byte_value
@done:
        RTS
@ok:
        LDA #ZSTORY_ERR_NONE
        RTS

zstory_write_header_byte_value:
        STA zstory_addr_l
        STZ zstory_addr_m
        STZ zstory_addr_h
        STX XRAM_DATA
        JMP zstory_write8

zstory_write_header_word_value:
        STA zstory_addr_l
        STZ zstory_addr_m
        STZ zstory_addr_h
        STZ zstory_word_hi
        STX zstory_word_lo
        JMP zstory_write16

; A = header byte offset. Returns XRAM_DATA and A=0 on success.
zstory_read_header_byte:
        STA zstory_addr_l
        STZ zstory_addr_m
        STZ zstory_addr_h
        JSR zstory_set_dynamic_xram_addr
        JMP xram_read8

; A = header word offset. Returns zstory_word_hi/lo and A=0 on success.
zstory_read_header_word:
        STA zstory_addr_l
        STZ zstory_addr_m
        STZ zstory_addr_h
        JSR zstory_set_dynamic_xram_addr
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
        JSR zstory_addr_is_dynamic
        BNE @paged
        JSR zstory_set_dynamic_xram_addr
        JMP xram_read8
@paged:
        PHY
        JSR zstory_resolve_page
        PLY
        CMP #ZSTORY_ERR_NONE
        BNE @done
        JSR zstory_set_cache_xram_addr
        JMP xram_read8
@done:
        RTS

; Read the original story-file byte, ignoring dynamic-memory mutations.
zstory_read_file8:
        PHY
        JSR zstory_resolve_page
        PLY
        CMP #ZSTORY_ERR_NONE
        BNE @done
        JSR zstory_set_cache_xram_addr
        JMP xram_read8
@done:
        RTS

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

; Write XRAM_DATA to zstory_addr_l/m/h.
zstory_write8:
        JSR zstory_addr_is_dynamic
        BNE @readonly
        JSR zstory_set_dynamic_xram_addr
        JMP xram_write8
@readonly:
        LDA #ZSTORY_ERR_READONLY
        RTS

; Write zstory_word_hi/lo to zstory_addr_l/m/h as a big-endian word.
; The address is advanced by two bytes.
zstory_write16:
        LDA zstory_word_hi
        STA XRAM_DATA
        JSR zstory_write8
        BNE @done
        JSR zstory_inc_addr
        LDA zstory_word_lo
        STA XRAM_DATA
        JSR zstory_write8
        BNE @done
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

zstory_addr_is_dynamic:
        LDA zstory_addr_h
        BNE @readonly
        LDA zstory_addr_m
        CMP zstory_static_hi
        BCC @ok
        BNE @readonly
        LDA zstory_addr_l
        CMP zstory_static_lo
        BCC @ok
@readonly:
        LDA #ZSTORY_ERR_READONLY
        RTS
@ok:
        LDA #ZSTORY_ERR_NONE
        RTS

zstory_load_dynamic:
        LDA zstory_static_lo
        ORA zstory_static_hi
        BEQ @error

        LDA #<zstory_default_name
        STA XRAM_NAMEPTR_L
        LDA #>zstory_default_name
        STA XRAM_NAMEPTR_H
        LDA #(zstory_default_name_end - zstory_default_name)
        STA XRAM_NAMELEN

        LDA zstory_static_lo
        STA XRAM_LENL
        LDA zstory_static_hi
        STA XRAM_LENH
        JSR zstory_set_dynamic_xram_origin
        JSR xram_xload
        BEQ @done
@error:
        LDA #ZSTORY_ERR_IO
        RTS
@done:
        LDA #ZSTORY_ERR_NONE
        RTS

zstory_clear_cache:
        STZ zstory_next_slot
        LDX #$00
@loop:
        STZ zstory_cache_valid,X
        INX
        CPX #ZSTORY_CACHE_SLOTS
        BCC @loop
        RTS

zstory_resolve_page:
        LDA zstory_addr_m
        LSR
        LSR
        STA zstory_page_no
        LDA zstory_addr_h
        ASL
        ASL
        ASL
        ASL
        ASL
        ASL
        ORA zstory_page_no
        STA zstory_page_no

        LDX #$00
@find:
        LDA zstory_cache_valid,X
        BEQ @next
        LDA zstory_cache_page,X
        CMP zstory_page_no
        BEQ @hit
@next:
        INX
        CPX #ZSTORY_CACHE_SLOTS
        BCC @find

        LDX zstory_next_slot
        STX zstory_slot
        INX
        CPX #ZSTORY_CACHE_SLOTS
        BCC @store_next
        LDX #$00
@store_next:
        STX zstory_next_slot
        JSR zstory_load_cache_slot
        BNE @io_error
        LDX zstory_slot
        LDA zstory_page_no
        STA zstory_cache_page,X
        LDA #$01
        STA zstory_cache_valid,X
        LDA #ZSTORY_ERR_NONE
        RTS
@hit:
        STX zstory_slot
        LDA #ZSTORY_ERR_NONE
        RTS
@io_error:
        LDA #ZSTORY_ERR_IO
        RTS

zstory_load_cache_slot:
        LDA #<zstory_default_name
        STA PAGER_NAMEPTR_L
        LDA #>zstory_default_name
        STA PAGER_NAMEPTR_H
        LDA #(zstory_default_name_end - zstory_default_name)
        STA PAGER_NAMELEN

        LDA zstory_cache_base_l
        STA PAGER_ADDRL
        LDA zstory_slot
        ASL
        ASL
        CLC
        ADC zstory_cache_base_m
        STA PAGER_ADDRM
        LDA zstory_cache_base_h
        ADC #$00
        STA PAGER_ADDRH
        STZ PAGER_LENL
        LDA #ZSTORY_PAGE_SIZE_HI
        STA PAGER_LENH
        STZ PAGER_TARGET

        STZ PAGER_FILEL
        LDA zstory_page_no
        ASL
        ASL
        STA PAGER_FILEM
        LDA zstory_page_no
        LSR
        LSR
        LSR
        LSR
        LSR
        LSR
        STA PAGER_FILEH
        JMP pager_load_file_page

zstory_set_cache_xram_addr:
        LDA zstory_addr_l
        CLC
        ADC zstory_cache_base_l
        STA XRAM_ADDRL
        LDA zstory_slot
        ASL
        ASL
        STA zstory_tmp
        LDA zstory_addr_m
        AND #$03
        CLC
        ADC zstory_tmp
        STA zstory_tmp
        LDA zstory_cache_base_m
        CLC
        ADC zstory_tmp
        STA XRAM_ADDRM
        LDA zstory_cache_base_h
        ADC #$00
        STA XRAM_ADDRH
        RTS

zstory_set_dynamic_xram_origin:
        LDA zstory_dynamic_base_l
        STA XRAM_ADDRL
        LDA zstory_dynamic_base_m
        STA XRAM_ADDRM
        LDA zstory_dynamic_base_h
        STA XRAM_ADDRH
        RTS

zstory_set_dynamic_xram_addr:
        LDA zstory_dynamic_base_l
        CLC
        ADC zstory_addr_l
        STA XRAM_ADDRL
        LDA zstory_dynamic_base_m
        ADC zstory_addr_m
        STA XRAM_ADDRM
        LDA zstory_dynamic_base_h
        ADC zstory_addr_h
        STA XRAM_ADDRH
        RTS

zstory_init_fixed_xram_bases:
        LDA #XRAM_NOVAZ_DYNAMIC_L
        STA zstory_dynamic_base_l
        LDA #XRAM_NOVAZ_DYNAMIC_M
        STA zstory_dynamic_base_m
        LDA #XRAM_NOVAZ_DYNAMIC_H
        STA zstory_dynamic_base_h
        LDA #XRAM_NOVAZ_CACHE_L
        STA zstory_cache_base_l
        LDA #XRAM_NOVAZ_CACHE_M
        STA zstory_cache_base_m
        LDA #XRAM_NOVAZ_CACHE_H
        STA zstory_cache_base_h
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
        STA zstory_filebytes_m
        LDA zstory_word_lo
        STA zstory_filelen_lo
        STA zstory_filebytes_l
        STZ zstory_filebytes_h

        JSR zstory_shift_filebytes
        LDA zstory_version
        CMP #$04
        BCC :+
        JSR zstory_shift_filebytes
:
        RTS

zstory_shift_filebytes:
        ASL zstory_filebytes_l
        ROL zstory_filebytes_m
        ROL zstory_filebytes_h
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

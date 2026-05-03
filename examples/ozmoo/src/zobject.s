; Nova Ozmoo object-table helpers.

.include "zstory.inc"
.include "ztext.inc"
.include "zobject.inc"

.ifndef NOVA_OZMOO_ZOBJECT_IMPLEMENTATION_INCLUDED
NOVA_OZMOO_ZOBJECT_IMPLEMENTATION_INCLUDED = 1

.segment "ZEROPAGE"

zobject_num:           .res 1
zobject_offset_lo:     .res 1
zobject_offset_hi:     .res 1
zobject_prop_lo:       .res 1
zobject_prop_hi:       .res 1
zobject_name_words:    .res 1

.segment "CODE"

.export zobject_get_prop_table
.export zobject_print_short_name

; A = object number (v1-3 object table form). Returns A=0 on success.
zobject_get_prop_table:
        STA zobject_num
        LDA zstory_version
        CMP #$04
        BCC :+
        LDA #ZOBJECT_ERR_UNSUPPORTED
        RTS
:
        LDA zobject_num
        BEQ @unsupported
        DEC
        STA zobject_num
        STA zobject_offset_lo
        STZ zobject_offset_hi

        ; offset = (object - 1) * 9 + 62
        ASL zobject_offset_lo
        ROL zobject_offset_hi
        ASL zobject_offset_lo
        ROL zobject_offset_hi
        ASL zobject_offset_lo
        ROL zobject_offset_hi
        LDA zobject_offset_lo
        CLC
        ADC zobject_num
        STA zobject_offset_lo
        LDA zobject_offset_hi
        ADC #$00
        STA zobject_offset_hi

        LDA zobject_offset_lo
        CLC
        ADC #62 + 7
        STA zobject_offset_lo
        LDA zobject_offset_hi
        ADC #$00
        STA zobject_offset_hi

        LDA zstory_object_lo
        CLC
        ADC zobject_offset_lo
        STA zstory_addr_l
        LDA zstory_object_hi
        ADC zobject_offset_hi
        STA zstory_addr_m
        STZ zstory_addr_h

        JSR zstory_read8
        BNE @done
        LDA XRAM_DATA
        STA zobject_prop_hi
        JSR zobject_inc_story_addr
        JSR zstory_read8
        BNE @done
        LDA XRAM_DATA
        STA zobject_prop_lo
        LDA #ZOBJECT_ERR_NONE
@done:
        RTS

@unsupported:
        LDA #ZOBJECT_ERR_UNSUPPORTED
        RTS

; A = object number. Prints its short name.
zobject_print_short_name:
        JSR zobject_get_prop_table
        BNE @done
        LDA zobject_prop_lo
        STA zstory_addr_l
        LDA zobject_prop_hi
        STA zstory_addr_m
        STZ zstory_addr_h
        JSR zstory_read8
        BNE @done
        LDA XRAM_DATA
        STA zobject_name_words
        BEQ @ok
        JSR zobject_inc_story_addr
        LDA zobject_name_words
        STA ztext_word_limit
        JSR ztext_print_packed
@ok:
        LDA #ZOBJECT_ERR_NONE
@done:
        RTS

zobject_inc_story_addr:
        INC zstory_addr_l
        BNE @done
        INC zstory_addr_m
        BNE @done
        INC zstory_addr_h
@done:
        RTS

.endif

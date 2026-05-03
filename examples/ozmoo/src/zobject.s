; Nova Ozmoo object-table helpers.

.include "zstory.inc"
.include "ztext.inc"
.include "zobject.inc"

.ifndef NOVA_OZMOO_ZOBJECT_IMPLEMENTATION_INCLUDED
NOVA_OZMOO_ZOBJECT_IMPLEMENTATION_INCLUDED = 1

.segment "ZEROPAGE"

zobject_num:           .res 1
zobject_prop_num:      .res 1
zobject_attr_num:      .res 1
zobject_value_hi:      .res 1
zobject_value_lo:      .res 1
zobject_offset_lo:     .res 1
zobject_offset_hi:     .res 1
zobject_prop_lo:       .res 1
zobject_prop_hi:       .res 1
zobject_name_words:    .res 1
zobject_prop_len:      .res 1
zobject_tmp:           .res 1
zobject_mask:          .res 1
zobject_child_tmp:     .res 1
zobject_target:        .res 1

.segment "CODE"

.export zobject_get_prop_table
.export zobject_get_parent
.export zobject_get_sibling
.export zobject_get_child
.export zobject_set_parent
.export zobject_set_sibling
.export zobject_set_child
.export zobject_test_attr
.export zobject_set_attr
.export zobject_clear_attr
.export zobject_remove
.export zobject_insert
.export zobject_get_prop
.export zobject_get_prop_addr
.export zobject_get_next_prop
.export zobject_get_prop_len
.export zobject_print_short_name

; A = object number (v1-3 object table form). zstory_addr = object entry.
zobject_get_entry:
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
        ADC #62
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
        LDA #ZOBJECT_ERR_NONE
        RTS

@unsupported:
        LDA #ZOBJECT_ERR_UNSUPPORTED
        RTS

; A = object number (v1-3 object table form). Returns A=0 on success.
zobject_get_prop_table:
        JSR zobject_get_entry
        BNE @done

        LDA zstory_addr_l
        CLC
        ADC #$07
        STA zstory_addr_l
        LDA zstory_addr_m
        ADC #$00
        STA zstory_addr_m

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

zobject_add_offset:
        CLC
        ADC zstory_addr_l
        STA zstory_addr_l
        BCC @done
        INC zstory_addr_m
@done:
        RTS

; A = object number. Returns zobject_value_lo = parent.
zobject_get_parent:
        JSR zobject_get_entry
        BNE @done
        LDA #$04
        JSR zobject_add_offset
        JSR zstory_read8
        BNE @done
        STZ zobject_value_hi
        LDA XRAM_DATA
        STA zobject_value_lo
        LDA #ZOBJECT_ERR_NONE
@done:
        RTS

; A = object number. Returns zobject_value_lo = sibling.
zobject_get_sibling:
        JSR zobject_get_entry
        BNE @done
        LDA #$05
        JSR zobject_add_offset
        JSR zstory_read8
        BNE @done
        STZ zobject_value_hi
        LDA XRAM_DATA
        STA zobject_value_lo
        LDA #ZOBJECT_ERR_NONE
@done:
        RTS

; A = object number. Returns zobject_value_lo = child.
zobject_get_child:
        JSR zobject_get_entry
        BNE @done
        LDA #$06
        JSR zobject_add_offset
        JSR zstory_read8
        BNE @done
        STZ zobject_value_hi
        LDA XRAM_DATA
        STA zobject_value_lo
        LDA #ZOBJECT_ERR_NONE
@done:
        RTS

zobject_write_field:
        PHA
        LDA zobject_num
        JSR zobject_get_entry
        BNE @drop
        PLA
        JSR zobject_add_offset
        LDA zobject_value_lo
        STA XRAM_DATA
        JMP zstory_write8
@drop:
        PLA
        RTS

; zobject_num = object number; zobject_value_lo = parent.
zobject_set_parent:
        LDA #$04
        JMP zobject_write_field

; zobject_num = object number; zobject_value_lo = sibling.
zobject_set_sibling:
        LDA #$05
        JMP zobject_write_field

; zobject_num = object number; zobject_value_lo = child.
zobject_set_child:
        LDA #$06
        JMP zobject_write_field

zobject_attr_prepare:
        LDA zobject_attr_num
        LSR
        LSR
        LSR
        STA zobject_tmp
        LDA zobject_attr_num
        AND #$07
        TAX
        LDA #$80
@mask:
        CPX #$00
        BEQ @mask_done
        LSR
        DEX
        BRA @mask
@mask_done:
        STA zobject_mask
        LDA zobject_num
        JSR zobject_get_entry
        BNE @done
        LDA zobject_tmp
        JSR zobject_add_offset
        LDA #ZOBJECT_ERR_NONE
@done:
        RTS

; zobject_num/zobject_attr_num. Returns A=1 when set, A=0 when clear.
zobject_test_attr:
        JSR zobject_attr_prepare
        BNE @false
        JSR zstory_read8
        BNE @false
        LDA XRAM_DATA
        AND zobject_mask
        BEQ @false
        LDA #$01
        RTS
@false:
        LDA #$00
        RTS

zobject_set_attr:
        JSR zobject_attr_prepare
        BNE @done
        JSR zstory_read8
        BNE @done
        LDA XRAM_DATA
        ORA zobject_mask
        STA XRAM_DATA
        JSR zstory_write8
@done:
        RTS

zobject_clear_attr:
        JSR zobject_attr_prepare
        BNE @done
        JSR zstory_read8
        BNE @done
        LDA zobject_mask
        EOR #$FF
        AND XRAM_DATA
        STA XRAM_DATA
        JSR zstory_write8
@done:
        RTS

; zobject_num = object to remove.
zobject_remove:
        LDA zobject_num
        STA zobject_target
        LDA zobject_num
        JSR zobject_get_parent
        BNE @done
        LDA zobject_value_lo
        BEQ @clear_target
        STA zobject_tmp          ; parent

        LDA zobject_target
        JSR zobject_get_sibling
        BNE @done
        LDA zobject_value_lo
        STA zobject_child_tmp    ; object's next sibling

        LDA zobject_tmp
        JSR zobject_get_child
        BNE @done
        LDA zobject_value_lo
        CMP zobject_target
        BNE @scan_siblings

        LDA zobject_tmp
        STA zobject_num
        LDA zobject_child_tmp
        STA zobject_value_lo
        JSR zobject_set_child
        BRA @clear_self_by_tmp

@scan_siblings:
        STA zobject_tmp          ; current sibling
@scan_loop:
        LDA zobject_tmp
        BEQ @clear_target
        JSR zobject_get_sibling
        BNE @done
        LDA zobject_value_lo
        CMP zobject_target
        BEQ @unlink_after_current
        STA zobject_tmp
        BRA @scan_loop

@unlink_after_current:
        LDA zobject_tmp
        STA zobject_num
        LDA zobject_child_tmp
        STA zobject_value_lo
        JSR zobject_set_sibling

@clear_self_by_tmp:
@clear_target:
        LDA zobject_target
        STA zobject_num
        STZ zobject_value_lo
        JSR zobject_set_parent
        LDA zobject_target
        STA zobject_num
        STZ zobject_value_lo
        JSR zobject_set_sibling
        LDA #ZOBJECT_ERR_NONE
@done:
        RTS

; zobject_num = object, zobject_value_lo = destination parent.
zobject_insert:
        LDA zobject_num
        STA zobject_target
        LDA zobject_value_lo
        STA zobject_tmp          ; destination
        JSR zobject_remove

        LDA zobject_tmp
        JSR zobject_get_child
        BNE @done
        LDA zobject_value_lo
        STA zobject_child_tmp

        LDA zobject_target
        STA zobject_num
        LDA zobject_tmp
        STA zobject_value_lo
        JSR zobject_set_parent
        LDA zobject_target
        STA zobject_num
        LDA zobject_child_tmp
        STA zobject_value_lo
        JSR zobject_set_sibling

        LDA zobject_tmp
        STA zobject_num
        LDA zobject_target
        STA zobject_value_lo
        JSR zobject_set_child
        LDA #ZOBJECT_ERR_NONE
@done:
        RTS

zobject_skip_name:
        LDA zobject_prop_lo
        STA zstory_addr_l
        LDA zobject_prop_hi
        STA zstory_addr_m
        STZ zstory_addr_h
        JSR zstory_read8
        BNE @done
        LDA XRAM_DATA
        ASL
        CLC
        ADC #$01
        JSR zobject_add_offset
        LDA #ZOBJECT_ERR_NONE
@done:
        RTS

; zobject_num/zobject_prop_num. Returns A=0 when found and
; zobject_prop_hi/lo points at property data.
zobject_find_prop:
        LDA zobject_num
        JSR zobject_get_prop_table
        BNE @not_found
        JSR zobject_skip_name
        BNE @not_found
@loop:
        JSR zstory_read8
        BNE @not_found
        LDA XRAM_DATA
        BEQ @not_found
        STA zobject_tmp
        AND #$1F
        CMP zobject_prop_num
        BEQ @found
        LDA zobject_tmp
        LSR
        LSR
        LSR
        LSR
        LSR
        CLC
        ADC #$02              ; size byte plus property length
        JSR zobject_add_offset
        BRA @loop
@found:
        LDA zobject_tmp
        LSR
        LSR
        LSR
        LSR
        LSR
        INC
        STA zobject_prop_len
        JSR zobject_inc_story_addr
        LDA zstory_addr_l
        STA zobject_prop_lo
        LDA zstory_addr_m
        STA zobject_prop_hi
        LDA #ZOBJECT_ERR_NONE
        RTS
@not_found:
        LDA #ZOBJECT_ERR_UNSUPPORTED
        RTS

; zobject_num/zobject_prop_num. Returns zobject_value_hi/lo.
zobject_get_prop_addr:
        JSR zobject_find_prop
        BNE @missing
        LDA zobject_prop_hi
        STA zobject_value_hi
        LDA zobject_prop_lo
        STA zobject_value_lo
        LDA #ZOBJECT_ERR_NONE
        RTS
@missing:
        STZ zobject_value_hi
        STZ zobject_value_lo
        LDA #ZOBJECT_ERR_NONE
        RTS

; zobject_num/zobject_prop_num. Returns zobject_value_hi/lo.
zobject_get_prop:
        JSR zobject_find_prop
        BNE @default
        LDA zobject_prop_lo
        STA zstory_addr_l
        LDA zobject_prop_hi
        STA zstory_addr_m
        STZ zstory_addr_h
        LDA zobject_prop_len
        CMP #$01
        BNE @read_word
        JSR zstory_read8
        BNE @done
        STZ zobject_value_hi
        LDA XRAM_DATA
        STA zobject_value_lo
        LDA #ZOBJECT_ERR_NONE
@done:
        RTS
@read_word:
        JSR zstory_read16
        BNE @done
        LDA zstory_word_hi
        STA zobject_value_hi
        LDA zstory_word_lo
        STA zobject_value_lo
        LDA #ZOBJECT_ERR_NONE
        RTS
@default:
        LDA zobject_prop_num
        BEQ @zero
        DEC
        ASL
        CLC
        ADC zstory_object_lo
        STA zstory_addr_l
        LDA zstory_object_hi
        ADC #$00
        STA zstory_addr_m
        STZ zstory_addr_h
        JSR zstory_read16
        BNE @zero
        LDA zstory_word_hi
        STA zobject_value_hi
        LDA zstory_word_lo
        STA zobject_value_lo
        LDA #ZOBJECT_ERR_NONE
        RTS
@zero:
        STZ zobject_value_hi
        STZ zobject_value_lo
        LDA #ZOBJECT_ERR_NONE
        RTS

; zobject_num/zobject_prop_num. Returns next property number in zobject_value_lo.
zobject_get_next_prop:
        LDA zobject_prop_num
        BNE @after_current
        LDA zobject_num
        JSR zobject_get_prop_table
        BNE @zero
        JSR zobject_skip_name
        BNE @zero
        JSR zstory_read8
        BNE @zero
        LDA XRAM_DATA
        AND #$1F
        STA zobject_value_lo
        STZ zobject_value_hi
        LDA #ZOBJECT_ERR_NONE
        RTS
@after_current:
        JSR zobject_find_prop
        BNE @zero
        LDA zobject_prop_len
        JSR zobject_add_offset
        JSR zstory_read8
        BNE @zero
        LDA XRAM_DATA
        AND #$1F
        STA zobject_value_lo
        STZ zobject_value_hi
        LDA #ZOBJECT_ERR_NONE
        RTS
@zero:
        STZ zobject_value_hi
        STZ zobject_value_lo
        LDA #ZOBJECT_ERR_NONE
        RTS

; zobject_value_hi/lo = property data address. Returns length in value_lo.
zobject_get_prop_len:
        LDA zobject_value_hi
        ORA zobject_value_lo
        BNE @nonzero
        STZ zobject_value_hi
        STZ zobject_value_lo
        RTS
@nonzero:
        LDA zobject_value_lo
        BNE @no_borrow
        DEC zobject_value_hi
@no_borrow:
        DEC zobject_value_lo
        LDA zobject_value_lo
        STA zstory_addr_l
        LDA zobject_value_hi
        STA zstory_addr_m
        STZ zstory_addr_h
        JSR zstory_read8
        BNE @zero
        LDA XRAM_DATA
        LSR
        LSR
        LSR
        LSR
        LSR
        INC
        STA zobject_value_lo
        STZ zobject_value_hi
        LDA #ZOBJECT_ERR_NONE
        RTS
@zero:
        STZ zobject_value_hi
        STZ zobject_value_lo
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

; Nova-native NOBJ/NLIB linker. Produces a load-address-prefixed Nova binary.
;
; This core consumes ordered section-aware NOBJ v2 objects, resolves indexed
; local and cross-object global symbols, and extracts complete NOBJ members from
; NLIB v2 archives only when an unresolved global requires them.

      .setcpu "w65c02"
      .include "novalink.inc"

.macro long_bcs target
      BCC   :+
      JMP   target
:
.endmacro

.macro long_bcc target
      BCS   :+
      JMP   target
:
.endmacro

.macro long_bne target
      BEQ   :+
      JMP   target
:
.endmacro

.macro long_beq target
      BNE   :+
      JMP   target
:
.endmacro

NLINK_SECTION_CAP = 8
NLINK_TOTAL_SECTION_CAP = NLINK_OBJECT_CAP * NLINK_SECTION_CAP

      .segment "ZEROPAGE"
l_obj:          .res 2
l_src:          .res 2
l_dst:          .res 2
l_left:         .res 2
l_cap:          .res 2
l_payload_len:  .res 2
l_reloc:        .res 2
l_reloc_bytes:  .res 2
l_reloc_count:  .res 2
l_name:         .res 2
l_name_len:     .res 1
l_offset:       .res 2
l_library:      .res 2
l_member:       .res 2
l_member_name:  .res 2
l_member_code:  .res 2
l_member_len:   .res 2
l_member_count: .res 1
l_archive_ptr:  .res 2
l_address:      .res 2
l_tmp:          .res 2

      .segment "BSS"
      .export nlink_object_count
      .export nlink_object_ptr_l
      .export nlink_object_ptr_h
      .export nlink_object_len_l
      .export nlink_object_len_h
      .export nlink_library_ptr
      .export nlink_library_len
      .export nlink_output_ptr
      .export nlink_output_cap
      .export nlink_load_base
      .export nlink_output_len
      .export nlink_entry
      .export nlink_error
      .export nlink_config_enabled
      .export nlink_region_count
      .export nlink_region_name_len
      .export nlink_region_names
      .export nlink_region_start_l
      .export nlink_region_start_h
      .export nlink_region_size_l
      .export nlink_region_size_h
      .export nlink_rule_count
      .export nlink_rule_name_len
      .export nlink_rule_names
      .export nlink_rule_region
      .export nlink_config_symbol_count
      .export nlink_config_symbol_name_len
      .export nlink_config_symbol_names
      .export nlink_config_symbol_value_l
      .export nlink_config_symbol_value_h
nlink_object_count:.res 1
nlink_object_ptr_l:.res NLINK_OBJECT_CAP
nlink_object_ptr_h:.res NLINK_OBJECT_CAP
nlink_object_len_l:.res NLINK_OBJECT_CAP
nlink_object_len_h:.res NLINK_OBJECT_CAP
nlink_library_ptr: .res 2
nlink_library_len: .res 2
nlink_output_ptr:  .res 2
nlink_output_cap:  .res 2
nlink_load_base:   .res 2
nlink_output_len:  .res 2
nlink_entry:       .res 2
nlink_error:       .res 1
nlink_config_enabled:.res 1
nlink_region_count:.res 1
nlink_region_name_len:.res NLINK_REGION_CAP
nlink_region_names:.res NLINK_REGION_CAP * NLINK_NAME_CAP
nlink_region_start_l:.res NLINK_REGION_CAP
nlink_region_start_h:.res NLINK_REGION_CAP
nlink_region_size_l:.res NLINK_REGION_CAP
nlink_region_size_h:.res NLINK_REGION_CAP
nlink_rule_count:.res 1
nlink_rule_name_len:.res NLINK_RULE_CAP
nlink_rule_names:.res NLINK_RULE_CAP * NLINK_NAME_CAP
nlink_rule_region:.res NLINK_RULE_CAP
nlink_config_symbol_count:.res 1
nlink_config_symbol_name_len:.res NLINK_SYMBOL_CAP
nlink_config_symbol_names:.res NLINK_SYMBOL_CAP * NLINK_NAME_CAP
nlink_config_symbol_value_l:.res NLINK_SYMBOL_CAP
nlink_config_symbol_value_h:.res NLINK_SYMBOL_CAP
l_section_len:     .res 2
l_addend:          .res 2
l_section_count:   .res 1
l_section_index:   .res 1
l_patch_flat:      .res 1
l_section_base:    .res 1
l_object_iter:     .res 1
l_scan_object:     .res 1
l_reloc_object:    .res 1
l_reloc_type:      .res 1
l_global_found:    .res 1
l_obj_len:         .res 2
l_archive_left:    .res 2
l_archive_len:     .res 2
l_archive_index:   .res 1
l_rule_index:      .res 1
l_region_cursor_l: .res NLINK_REGION_CAP
l_region_cursor_h: .res NLINK_REGION_CAP
l_sec_offset_l:    .res NLINK_TOTAL_SECTION_CAP
l_sec_offset_h:    .res NLINK_TOTAL_SECTION_CAP
l_sec_size_l:      .res NLINK_TOTAL_SECTION_CAP
l_sec_size_h:      .res NLINK_TOTAL_SECTION_CAP
l_prepared:        .res 1

      .segment "CODE"
      .export nlink_link
      .export nlink_prepare
      .export nlink_export_worker_state
      .export nlink_import_worker_objects

; Validate direct objects and the archive, then select required members without
; assigning addresses. The worker consumes this stable object set before link.
nlink_prepare:
      STZ   nlink_error
      STZ   l_prepared
      STZ   nlink_output_len
      STZ   nlink_output_len+1
      LDA   nlink_object_count
      long_beq l_bad_object
      CMP   #NLINK_OBJECT_CAP+1
      long_bcs l_bad_object
      STZ   l_object_iter
@validate_object:
      LDA   l_object_iter
      CMP   nlink_object_count
      BEQ   @validate_library
      JSR   l_select_object
      long_bcs l_bad_object
      JSR   l_validate_object
      long_bcs l_bad_object
      INC   l_object_iter
      BRA   @validate_object
@validate_library:
      JSR   l_validate_library
      long_bcs l_bad_relocation
      INC   l_prepared
      LDA   #NLINK_OK
      RTS

; Return A=0 on success, A=1 on error.
nlink_link:
      STZ   nlink_output_len
      STZ   nlink_output_len+1
      LDA   l_prepared
      BNE   @prepared
      JSR   nlink_prepare
      BEQ   @prepared
      RTS
@prepared:
      STZ   l_prepared
      LDA   nlink_config_enabled
      BEQ   @config_ready
      JSR   l_init_config
      long_bcs l_bad_config
@config_ready:

      LDA   nlink_output_cap
      STA   l_cap
      LDA   nlink_output_cap+1
      STA   l_cap+1
      SEC
      LDA   l_cap
      SBC   #2
      STA   l_cap
      LDA   l_cap+1
      SBC   #0
      STA   l_cap+1
      long_bcc l_bad_output

      LDA   nlink_output_ptr
      STA   l_dst
      LDA   nlink_output_ptr+1
      STA   l_dst+1
      LDY   #0
      LDA   nlink_load_base
      STA   (l_dst),Y
      INY
      LDA   nlink_load_base+1
      STA   (l_dst),Y
      CLC
      LDA   l_dst
      ADC   #2
      STA   l_dst
      BCC   :+
      INC   l_dst+1
:     JSR   l_zero_output
      STZ   l_payload_len
      STZ   l_payload_len+1

      STZ   l_object_iter
@place_object:
      LDA   l_object_iter
      CMP   nlink_object_count
      BEQ   @relocations
      JSR   l_select_object
      long_bcs l_bad_object
      LDY   #NOBJ_SECTION_COUNT
      LDA   (l_obj),Y
      STA   l_section_count
      JSR   l_place_sections
      BCC   @next_place_object
      CMP   #NLINK_ERR_OBJECT
      long_beq l_bad_object
      CMP   #NLINK_ERR_CONFIG
      long_beq l_bad_config
      JMP   l_bad_output
@next_place_object:
      INC   l_object_iter
      BRA   @place_object

@relocations:
      STZ   l_object_iter
@relocation_object:
      LDA   l_object_iter
      CMP   nlink_object_count
      long_beq @linked
      STA   l_reloc_object
      JSR   l_select_object
      long_bcs l_bad_object
      LDY   #NOBJ_SECTION_COUNT
      LDA   (l_obj),Y
      STA   l_section_count
      LDY   #NOBJ_RELOC_OFFSET
      LDA   (l_obj),Y
      STA   l_tmp
      INY
      LDA   (l_obj),Y
      STA   l_tmp+1
      CLC
      LDA   l_obj
      ADC   l_tmp
      STA   l_reloc
      LDA   l_obj+1
      ADC   l_tmp+1
      STA   l_reloc+1
      SEC
      LDA   l_obj_len
      SBC   l_tmp
      STA   l_reloc_bytes
      LDA   l_obj_len+1
      SBC   l_tmp+1
      STA   l_reloc_bytes+1
      long_bcc l_bad_object
      LDY   #NOBJ_RELOC_COUNT
      LDA   (l_obj),Y
      STA   l_reloc_count
      INY
      LDA   (l_obj),Y
      STA   l_reloc_count+1

@next_reloc:
      LDA   l_reloc_count
      ORA   l_reloc_count+1
      long_beq @object_linked
      LDA   l_reloc_object
      JSR   l_select_object
      long_bcs l_bad_object
      LDY   #NOBJ_SECTION_COUNT
      LDA   (l_obj),Y
      STA   l_section_count
      JSR   l_take_reloc
      long_bcs l_bad_relocation
      CMP   l_section_count
      long_bcs l_bad_relocation
      STA   l_section_index
      JSR   l_section_is_live
      BCS   :+
      JMP   @skip_reloc
:     LDA   l_section_index
      CLC
      ADC   l_section_base
      STA   l_patch_flat
      JSR   l_take_reloc
      long_bcs l_bad_relocation
      STA   l_reloc_type
      CMP   #NOBJ_RELOC_ABS16
      long_bcc l_bad_relocation
      CMP   #NOBJ_RELOC_REL8+1
      long_bcs l_bad_relocation
      JSR   l_take_reloc
      long_bcs l_bad_relocation
      STA   l_offset
      JSR   l_take_reloc
      long_bcs l_bad_relocation
      STA   l_offset+1
      JSR   l_take_reloc
      long_bcs l_bad_relocation
      STA   l_tmp
      JSR   l_take_reloc
      long_bcs l_bad_relocation
      STA   l_tmp+1
      JSR   l_take_reloc
      long_bcs l_bad_relocation
      STA   l_addend
      JSR   l_take_reloc
      long_bcs l_bad_relocation
      STA   l_addend+1
      JSR   l_resolve_symbol
      long_bcs l_bad_relocation
      CLC
      LDA   l_address
      ADC   l_addend
      STA   l_address
      LDA   l_address+1
      ADC   l_addend+1
      STA   l_address+1
      JSR   l_apply_relocation
      long_bcs l_bad_relocation
@reloc_done:
      LDA   l_reloc_count
      BNE   :+
      DEC   l_reloc_count+1
:     DEC   l_reloc_count
      JMP   @next_reloc

@skip_reloc:
      LDX   #NOBJ_RELOC_SIZE-1
@skip_reloc_byte:
      JSR   l_take_reloc
      long_bcs l_bad_relocation
      DEX
      BNE   @skip_reloc_byte
      BRA   @reloc_done

@object_linked:
      LDA   l_reloc_bytes
      ORA   l_reloc_bytes+1
      long_bne l_bad_relocation
      INC   l_object_iter
      JMP   @relocation_object

@linked:
      CLC
      LDA   l_payload_len
      ADC   #2
      STA   nlink_output_len
      LDA   l_payload_len+1
      ADC   #0
      STA   nlink_output_len+1
      LDA   #0
      JSR   l_select_object
      long_bcs l_bad_object
      LDY   #NOBJ_SECTION_COUNT
      LDA   (l_obj),Y
      STA   l_section_count
      LDY   #NOBJ_ENTRY_SECTION
      LDA   (l_obj),Y
      CMP   l_section_count
      long_bcs l_bad_object
      CLC
      ADC   l_section_base
      TAX
      CLC
      LDA   nlink_load_base
      ADC   l_sec_offset_l,X
      STA   nlink_entry
      LDA   nlink_load_base+1
      ADC   l_sec_offset_h,X
      STA   nlink_entry+1
      INY
      CLC
      LDA   nlink_entry
      ADC   (l_obj),Y
      STA   nlink_entry
      INY
      LDA   nlink_entry+1
      ADC   (l_obj),Y
      STA   nlink_entry+1
      LDA   #NLINK_OK
      RTS

; Select object A and its reserved section-placement slice.
l_select_object:
      CMP   nlink_object_count
      BCS   @bad
      TAX
      LDA   nlink_object_ptr_l,X
      STA   l_obj
      LDA   nlink_object_ptr_h,X
      STA   l_obj+1
      LDA   nlink_object_len_l,X
      STA   l_obj_len
      LDA   nlink_object_len_h,X
      STA   l_obj_len+1
      TXA
      ASL
      ASL
      ASL
      STA   l_section_base
      CLC
      RTS
@bad:
      SEC
      RTS

; Carry set when the current object's current section survives GC. Before the
; worker publishes a mask, every section is live for the validation link.
l_section_is_live:
      LDA   NLW_GC_READY
      BEQ   @yes
      LDX   l_section_index
      LDA   l_section_bits,X
      LDX   l_object_iter
      AND   NLW_LIVE_MASK,X
      BEQ   @no
@yes:
      SEC
      RTS
@no:
      CLC
      RTS

nlink_export_worker_state:
      LDA   nlink_object_count
      STA   NLW_OBJECT_COUNT
      LDX   #NLINK_OBJECT_CAP-1
@object:
      LDA   nlink_object_ptr_l,X
      STA   NLW_OBJECT_PTR_L,X
      LDA   nlink_object_ptr_h,X
      STA   NLW_OBJECT_PTR_H,X
      LDA   nlink_object_len_l,X
      STA   NLW_OBJECT_LEN_L,X
      LDA   nlink_object_len_h,X
      STA   NLW_OBJECT_LEN_H,X
      DEX
      BPL   @object
      LDA   nlink_library_ptr
      STA   NLW_LIBRARY_PTR
      LDA   nlink_library_ptr+1
      STA   NLW_LIBRARY_PTR+1
      LDA   nlink_library_len
      STA   NLW_LIBRARY_LEN
      LDA   nlink_library_len+1
      STA   NLW_LIBRARY_LEN+1
      LDA   nlink_config_symbol_count
      STA   NLW_CONFIG_SYMBOL_COUNT
      LDX   #NLINK_SYMBOL_CAP-1
@config_symbol:
      LDA   nlink_config_symbol_name_len,X
      STA   NLW_CONFIG_SYMBOL_NAME_LEN,X
      LDA   nlink_config_symbol_value_l,X
      STA   NLW_CONFIG_SYMBOL_VALUE_L,X
      LDA   nlink_config_symbol_value_h,X
      STA   NLW_CONFIG_SYMBOL_VALUE_H,X
      DEX
      BPL   @config_symbol
      LDX   #NLINK_SYMBOL_CAP * NLINK_NAME_CAP-1
@config_name:
      LDA   nlink_config_symbol_names,X
      STA   NLW_CONFIG_SYMBOL_NAMES,X
      DEX
      BPL   @config_name
      LDA   nlink_load_base
      STA   NLW_LOAD_BASE
      LDA   nlink_load_base+1
      STA   NLW_LOAD_BASE+1
      LDA   nlink_output_len
      STA   NLW_OUTPUT_LEN
      LDA   nlink_output_len+1
      STA   NLW_OUTPUT_LEN+1
      LDX   #NLINK_TOTAL_SECTION_CAP-1
@section:
      LDA   l_sec_offset_l,X
      STA   NLW_SEC_OFFSET_L,X
      LDA   l_sec_offset_h,X
      STA   NLW_SEC_OFFSET_H,X
      LDA   l_sec_size_l,X
      STA   NLW_SEC_SIZE_L,X
      LDA   l_sec_size_h,X
      STA   NLW_SEC_SIZE_H,X
      DEX
      BPL   @section
      RTS

nlink_import_worker_objects:
      LDA   NLW_OBJECT_COUNT
      BEQ   @bad
      CMP   #NLINK_OBJECT_CAP+1
      BCS   @bad
      CMP   NLW_ROOT_COUNT
      BCC   @bad
      STA   nlink_object_count
      LDX   #NLINK_OBJECT_CAP-1
@object:
      LDA   NLW_OBJECT_PTR_L,X
      STA   nlink_object_ptr_l,X
      LDA   NLW_OBJECT_PTR_H,X
      STA   nlink_object_ptr_h,X
      LDA   NLW_OBJECT_LEN_L,X
      STA   nlink_object_len_l,X
      LDA   NLW_OBJECT_LEN_H,X
      STA   nlink_object_len_h,X
      DEX
      BPL   @object
      LDA   #0
      RTS
@bad:
      LDA   #1
      RTS

l_init_config:
      LDA   nlink_region_count
      BEQ   @bad
      CMP   #NLINK_REGION_CAP+1
      BCS   @bad
      LDA   nlink_rule_count
      BEQ   @bad
      CMP   #NLINK_RULE_CAP+1
      BCS   @bad
      LDX   #0
@region:
      CPX   nlink_region_count
      BCS   @rules
      LDA   nlink_region_size_l,X
      ORA   nlink_region_size_h,X
      BEQ   @bad
      LDA   nlink_region_start_l,X
      STA   l_region_cursor_l,X
      LDA   nlink_region_start_h,X
      STA   l_region_cursor_h,X
      INX
      BRA   @region
@rules:
      JSR   l_validate_region_layout
      BCS   @bad
      LDX   #0
@rule:
      CPX   nlink_rule_count
      BCS   @ok
      LDA   nlink_rule_region,X
      CMP   nlink_region_count
      BCS   @bad
      INX
      BRA   @rule
@ok:
      CLC
      RTS
@bad:
      SEC
      RTS

; MEMORY regions are half-open intervals and may touch, but never overlap.
l_validate_region_layout:
      LDX   #0
@left:
      CPX   nlink_region_count
      BCS   @ok
      CLC
      LDA   nlink_region_start_l,X
      ADC   nlink_region_size_l,X
      STA   l_address
      LDA   nlink_region_start_h,X
      ADC   nlink_region_size_h,X
      STA   l_address+1
      BCS   @bad
      TXA
      TAY
      INY
@right:
      CPY   nlink_region_count
      BCS   @next_left
      CLC
      LDA   nlink_region_start_l,Y
      ADC   nlink_region_size_l,Y
      STA   l_tmp
      LDA   nlink_region_start_h,Y
      ADC   nlink_region_size_h,Y
      STA   l_tmp+1
      BCS   @bad
      LDA   nlink_region_start_h,X
      CMP   l_tmp+1
      BCC   @left_before_right_end
      BNE   @next_right
      LDA   nlink_region_start_l,X
      CMP   l_tmp
      BCS   @next_right
@left_before_right_end:
      LDA   nlink_region_start_h,Y
      CMP   l_address+1
      BCC   @bad
      BNE   @next_right
      LDA   nlink_region_start_l,Y
      CMP   l_address
      BCC   @bad
@next_right:
      INY
      BRA   @right
@next_left:
      INX
      BRA   @left
@ok:
      CLC
      RTS
@bad:
      SEC
      RTS

; Deterministic gaps and alignment padding for both sequential and configured
; layouts. l_cap excludes the two-byte executable header.
l_zero_output:
      LDA   l_cap
      STA   l_left
      LDA   l_cap+1
      STA   l_left+1
@byte:
      LDA   l_left
      ORA   l_left+1
      BEQ   @restore
      LDY   #0
      LDA   #0
      STA   (l_dst),Y
      INC   l_dst
      BNE   :+
      INC   l_dst+1
:     LDA   l_left
      BNE   :+
      DEC   l_left+1
:     DEC   l_left
      BRA   @byte
@restore:
      CLC
      LDA   nlink_output_ptr
      ADC   #2
      STA   l_dst
      LDA   nlink_output_ptr+1
      ADC   #0
      STA   l_dst+1
      RTS

; Place all object sections in declaration order and remember output offsets.
l_place_sections:
      CLC
      LDA   l_obj
      ADC   #NOBJ_HEADER_SIZE
      STA   l_member
      LDA   l_obj+1
      ADC   #0
      STA   l_member+1
      STZ   l_section_index

@section:
      LDA   l_section_index
      CMP   l_section_count
      long_beq @done
      LDA   #NOBJ_SEC_HEADER_SIZE
      JSR   l_object_has
      long_bcc @object
      LDY   #NOBJ_SEC_ALIGN
      LDA   (l_member),Y
      CMP   #9
      long_bcs @object
      STA   l_archive_index

      CLC
      LDA   l_section_base
      ADC   l_section_index
      TAX
      STX   l_patch_flat
      LDY   #NOBJ_SEC_SIZE
      LDA   (l_member),Y
      STA   l_section_len
      STA   l_sec_size_l,X
      INY
      LDA   (l_member),Y
      STA   l_section_len+1
      STA   l_sec_size_h,X
      STZ   l_global_found
      JSR   l_section_is_live
      BCC   @dead_section
      INC   l_global_found
      LDA   nlink_config_enabled
      BEQ   @sequential
      JSR   l_place_configured_section
      long_bcs @config
      BRA   @data_length
@sequential:
      LDA   l_archive_index
      JSR   l_align_output
      long_bcs @output
      LDX   l_patch_flat
      LDA   l_payload_len
      STA   l_sec_offset_l,X
      LDA   l_payload_len+1
      STA   l_sec_offset_h,X
      BRA   @data_length
@dead_section:
      LDX   l_patch_flat
      STZ   l_sec_offset_l,X
      STZ   l_sec_offset_h,X
      STZ   l_sec_size_l,X
      STZ   l_sec_size_h,X
@data_length:
      LDY   #NOBJ_SEC_DATA_LEN
      LDA   (l_member),Y
      STA   l_member_len
      INY
      LDA   (l_member),Y
      STA   l_member_len+1
      CMP   l_section_len+1
      BCC   @data_fits
      long_bne @object
      LDA   l_member_len
      CMP   l_section_len
      BCC   @data_fits
      long_bne @object
@data_fits:
      LDY   #NOBJ_SEC_FLAGS
      LDA   (l_member),Y
      AND   #NOBJ_SEC_ZEROFILL
      BEQ   @source
      LDA   l_member_len
      ORA   l_member_len+1
      long_bne @object

@source:
      LDY   #NOBJ_SEC_NAME_LEN
      LDA   (l_member),Y
      CLC
      ADC   #NOBJ_SEC_HEADER_SIZE
      STA   l_tmp
      LDA   #0
      ADC   #0
      STA   l_tmp+1
      CLC
      LDA   l_member
      ADC   l_tmp
      STA   l_src
      LDA   l_member+1
      ADC   l_tmp+1
      STA   l_src+1
      CLC
      LDA   l_src
      ADC   l_member_len
      STA   l_tmp
      LDA   l_src+1
      ADC   l_member_len+1
      STA   l_tmp+1
      SEC
      LDA   l_tmp
      SBC   l_obj
      STA   l_offset
      LDA   l_tmp+1
      SBC   l_obj+1
      STA   l_offset+1
      long_bcc @object
      LDY   #NOBJ_SYMBOL_OFFSET+1
      LDA   l_offset+1
      CMP   (l_obj),Y
      BCC   @copy_data
      long_bne @object
      DEY
      LDA   l_offset
      CMP   (l_obj),Y
      BCC   @copy_data
      long_bne @object

@copy_data:
      LDA   l_global_found
      BNE   :+
      LDA   l_tmp
      STA   l_src
      LDA   l_tmp+1
      STA   l_src+1
      BRA   @next
:
      LDA   l_member_len
      STA   l_left
      LDA   l_member_len+1
      STA   l_left+1
@copy:
      LDA   l_left
      ORA   l_left+1
      BEQ   @fill_count
      LDY   #0
      LDA   (l_src),Y
      JSR   l_append
      long_bcs @output
      INC   l_src
      BNE   :+
      INC   l_src+1
:     LDA   l_left
      BNE   :+
      DEC   l_left+1
:     DEC   l_left
      BRA   @copy

@fill_count:
      SEC
      LDA   l_section_len
      SBC   l_member_len
      STA   l_left
      LDA   l_section_len+1
      SBC   l_member_len+1
      STA   l_left+1
@fill:
      LDA   l_left
      ORA   l_left+1
      BEQ   @next
      LDA   #0
      JSR   l_append
      long_bcs @output
      LDA   l_left
      BNE   :+
      DEC   l_left+1
:     DEC   l_left
      BRA   @fill

@next:
      LDA   l_src
      STA   l_member
      LDA   l_src+1
      STA   l_member+1
      INC   l_section_index
      JMP   @section

@done:
      SEC
      LDA   l_member
      SBC   l_obj
      STA   l_tmp
      LDA   l_member+1
      SBC   l_obj+1
      STA   l_tmp+1
      long_bcc @object
      LDY   #NOBJ_SYMBOL_OFFSET
      LDA   l_tmp
      CMP   (l_obj),Y
      long_bne @object
      INY
      LDA   l_tmp+1
      CMP   (l_obj),Y
      long_bne @object
      CLC
      RTS
@object:
      LDA   #NLINK_ERR_OBJECT
      SEC
      RTS
@output:
      LDA   #NLINK_ERR_OUTPUT
      SEC
      RTS
@config:
      LDA   #NLINK_ERR_CONFIG
      SEC
      RTS

; Match the current NOBJ section to a configured rule, align it inside that
; rule's MEMORY region, and select its output address.
l_place_configured_section:
      JSR   l_find_section_rule
      long_bcs @bad
      LDA   nlink_rule_region,X
      STA   l_rule_index
      TAX
      LDA   l_region_cursor_l,X
      STA   l_address
      LDA   l_region_cursor_h,X
      STA   l_address+1

      LDX   l_archive_index
      BEQ   @aligned
      LDA   #1
      STA   l_offset
      STZ   l_offset+1
@mask:
      ASL   l_offset
      ROL   l_offset+1
      DEX
      BNE   @mask
      LDA   l_offset
      BNE   :+
      DEC   l_offset+1
:     DEC   l_offset
@align:
      LDA   l_address
      AND   l_offset
      BNE   @pad
      LDA   l_address+1
      AND   l_offset+1
      BEQ   @aligned
@pad:
      INC   l_address
      BNE   @align
      INC   l_address+1
      BNE   @align
      JMP   @bad

@aligned:
      LDX   l_rule_index
      CLC
      LDA   nlink_region_start_l,X
      ADC   nlink_region_size_l,X
      STA   l_tmp
      LDA   nlink_region_start_h,X
      ADC   nlink_region_size_h,X
      STA   l_tmp+1
      long_bcs @bad
      CLC
      LDA   l_address
      ADC   l_section_len
      STA   l_src
      LDA   l_address+1
      ADC   l_section_len+1
      STA   l_src+1
      long_bcs @bad
      CMP   l_tmp+1
      BCC   @region_ok
      long_bne @bad
      LDA   l_src
      CMP   l_tmp
      BCC   @region_ok
      long_bne @bad
@region_ok:
      SEC
      LDA   l_address
      SBC   nlink_load_base
      STA   l_offset
      LDA   l_address+1
      SBC   nlink_load_base+1
      STA   l_offset+1
      long_bcc @bad
      CLC
      LDA   l_offset
      ADC   l_section_len
      STA   l_tmp
      LDA   l_offset+1
      ADC   l_section_len+1
      STA   l_tmp+1
      long_bcs @bad
      CMP   l_cap+1
      BCC   @output_ok
      long_bne @bad
      LDA   l_tmp
      CMP   l_cap
      BCC   @output_ok
      long_bne @bad
@output_ok:
      LDX   l_patch_flat
      LDA   l_offset
      STA   l_sec_offset_l,X
      LDA   l_offset+1
      STA   l_sec_offset_h,X
      CLC
      LDA   nlink_output_ptr
      ADC   #2
      STA   l_dst
      LDA   nlink_output_ptr+1
      ADC   #0
      STA   l_dst+1
      CLC
      LDA   l_dst
      ADC   l_offset
      STA   l_dst
      LDA   l_dst+1
      ADC   l_offset+1
      STA   l_dst+1
      LDX   l_rule_index
      LDA   l_src
      STA   l_region_cursor_l,X
      LDA   l_src+1
      STA   l_region_cursor_h,X
      LDA   l_tmp+1
      CMP   l_payload_len+1
      BCC   @ok
      BNE   @grow
      LDA   l_tmp
      CMP   l_payload_len
      BCC   @ok
      BEQ   @ok
@grow:
      LDA   l_tmp
      STA   l_payload_len
      LDA   l_tmp+1
      STA   l_payload_len+1
@ok:
      CLC
      RTS
@bad:
      SEC
      RTS

l_find_section_rule:
      LDY   #NOBJ_SEC_NAME_LEN
      LDA   (l_member),Y
      STA   l_name_len
      CLC
      LDA   l_member
      ADC   #NOBJ_SEC_HEADER_SIZE
      STA   l_name
      LDA   l_member+1
      ADC   #0
      STA   l_name+1
      LDX   #0
@rule:
      CPX   nlink_rule_count
      BCS   @missing
      LDA   nlink_rule_name_len,X
      CMP   l_name_len
      BNE   @next
      STX   l_rule_index
      TXA
      ASL
      ASL
      ASL
      ASL
      CLC
      ADC   #<nlink_rule_names
      STA   l_tmp
      LDA   #>nlink_rule_names
      ADC   #0
      STA   l_tmp+1
      LDY   #0
@name:
      CPY   l_name_len
      BCS   @found
      LDA   (l_name),Y
      CMP   (l_tmp),Y
      BNE   @next_saved
      INY
      BRA   @name
@next_saved:
      LDX   l_rule_index
@next:
      INX
      BRA   @rule
@found:
      LDX   l_rule_index
      CLC
      RTS
@missing:
      SEC
      RTS

; Pad until load_base + payload_len satisfies a log2 alignment in A.
l_align_output:
      TAX
      BEQ   @done
      LDA   #1
      STA   l_offset
      STZ   l_offset+1
@mask:
      ASL   l_offset
      ROL   l_offset+1
      DEX
      BNE   @mask
      LDA   l_offset
      BNE   :+
      DEC   l_offset+1
:     DEC   l_offset
@check:
      CLC
      LDA   nlink_load_base
      ADC   l_payload_len
      STA   l_tmp
      LDA   nlink_load_base+1
      ADC   l_payload_len+1
      STA   l_tmp+1
      LDA   l_tmp
      AND   l_offset
      BNE   @pad
      LDA   l_tmp+1
      AND   l_offset+1
      BEQ   @done
@pad:
      LDA   #0
      JSR   l_append
      BCS   @bad
      BRA   @check
@done:
      CLC
      RTS
@bad:
      SEC
      RTS

l_validate_object:
      LDA   l_obj_len+1
      BNE   @magic
      LDA   l_obj_len
      CMP   #NOBJ_HEADER_SIZE
      BCC   @bad
@magic:
      LDY   #NOBJ_MAGIC
      LDA   (l_obj),Y
      CMP   #NOBJ_MAGIC0
      BNE   @bad
      INY
      LDA   (l_obj),Y
      CMP   #NOBJ_MAGIC1
      BNE   @bad
      INY
      LDA   (l_obj),Y
      CMP   #NOBJ_MAGIC2
      BNE   @bad
      INY
      LDA   (l_obj),Y
      CMP   #NOBJ_MAGIC3
      BNE   @bad
      LDY   #NOBJ_VERSION_OFF
      LDA   (l_obj),Y
      CMP   #NOBJ_VERSION
      BNE   @bad
      LDY   #NOBJ_SECTION_COUNT
      LDA   (l_obj),Y
      BEQ   @bad
      CMP   #NLINK_SECTION_CAP+1
      BCS   @bad
      STA   l_section_count
      LDY   #NOBJ_RESERVED
      LDA   (l_obj),Y
      INY
      ORA   (l_obj),Y
      BNE   @bad
      ; header <= symbol offset <= relocation offset <= object length
      LDY   #NOBJ_SYMBOL_OFFSET+1
      LDA   (l_obj),Y
      BNE   @symbol_min_ok
      DEY
      LDA   (l_obj),Y
      CMP   #NOBJ_HEADER_SIZE
      BCC   @bad
@symbol_min_ok:
      LDY   #NOBJ_SYMBOL_OFFSET
      LDA   (l_obj),Y
      STA   l_tmp
      INY
      LDA   (l_obj),Y
      STA   l_tmp+1
      LDY   #NOBJ_RELOC_OFFSET
      SEC
      LDA   (l_obj),Y
      SBC   l_tmp
      INY
      LDA   (l_obj),Y
      SBC   l_tmp+1
      BCC   @bad
@reloc_order_ok:
      LDY   #NOBJ_RELOC_OFFSET
      LDA   (l_obj),Y
      STA   l_tmp
      INY
      LDA   (l_obj),Y
      STA   l_tmp+1
      LDA   l_tmp+1
      CMP   l_obj_len+1
      BCC   @ok
      BNE   @bad
      LDA   l_tmp
      CMP   l_obj_len
      BCC   @ok
      BEQ   @ok
@bad:
      SEC
      RTS
@ok:
      CLC
      RTS

l_validate_library:
      LDA   nlink_library_ptr
      STA   l_library
      LDA   nlink_library_ptr+1
      STA   l_library+1
      LDA   nlink_library_len+1
      BNE   @magic
      LDA   nlink_library_len
      CMP   #NLIB_HEADER_SIZE
      BCC   @bad
@magic:
      LDY   #0
      LDA   (l_library),Y
      CMP   #NLIB_MAGIC0
      BNE   @bad
      INY
      LDA   (l_library),Y
      CMP   #NLIB_MAGIC1
      BNE   @bad
      INY
      LDA   (l_library),Y
      CMP   #NLIB_MAGIC2
      BNE   @bad
      INY
      LDA   (l_library),Y
      CMP   #NLIB_MAGIC3
      BNE   @bad
      LDY   #NLIB_VERSION_OFF
      LDA   (l_library),Y
      CMP   #NLIB_VERSION
      BNE   @bad
      INY
      LDA   (l_library),Y
      STA   l_member_count
      JSR   l_begin_archive
      BCS   @bad
@member:
      LDA   l_archive_index
      BEQ   @done
      JSR   l_take_archive_member
      BCS   @bad
      LDA   l_member_code
      STA   l_obj
      LDA   l_member_code+1
      STA   l_obj+1
      LDA   l_archive_len
      STA   l_obj_len
      LDA   l_archive_len+1
      STA   l_obj_len+1
      STZ   l_section_base
      JSR   l_validate_object
      BCS   @bad
      DEC   l_archive_index
      BRA   @member
@done:
      LDA   l_archive_left
      ORA   l_archive_left+1
      BNE   @bad
      CLC
      RTS
@bad:
      SEC
      RTS

; Reset the archive cursor to its first length-prefixed NOBJ member.
l_begin_archive:
      CLC
      LDA   l_library
      ADC   #NLIB_HEADER_SIZE
      STA   l_archive_ptr
      LDA   l_library+1
      ADC   #0
      STA   l_archive_ptr+1
      SEC
      LDA   nlink_library_len
      SBC   #NLIB_HEADER_SIZE
      STA   l_archive_left
      LDA   nlink_library_len+1
      SBC   #0
      STA   l_archive_left+1
      BCC   @bad
      LDA   l_member_count
      STA   l_archive_index
      CLC
      RTS
@bad:
      SEC
      RTS

; Read one [object-length:u16, NOBJ] member and advance the archive cursor.
l_take_archive_member:
      LDA   l_archive_left+1
      BNE   @length
      LDA   l_archive_left
      CMP   #2
      BCC   @bad
@length:
      LDY   #0
      LDA   (l_archive_ptr),Y
      STA   l_archive_len
      INY
      LDA   (l_archive_ptr),Y
      STA   l_archive_len+1
      CLC
      LDA   l_archive_ptr
      ADC   #2
      STA   l_member_code
      LDA   l_archive_ptr+1
      ADC   #0
      STA   l_member_code+1
      SEC
      LDA   l_archive_left
      SBC   #2
      STA   l_archive_left
      LDA   l_archive_left+1
      SBC   #0
      STA   l_archive_left+1
      LDA   l_archive_len+1
      CMP   l_archive_left+1
      BCC   @fits
      BNE   @bad
      LDA   l_archive_len
      CMP   l_archive_left
      BCC   @fits
      BNE   @bad
@fits:
      SEC
      LDA   l_archive_left
      SBC   l_archive_len
      STA   l_archive_left
      LDA   l_archive_left+1
      SBC   l_archive_len+1
      STA   l_archive_left+1
      CLC
      LDA   l_member_code
      ADC   l_archive_len
      STA   l_archive_ptr
      LDA   l_member_code+1
      ADC   l_archive_len+1
      STA   l_archive_ptr+1
      CLC
      RTS
@bad:
      SEC
      RTS

; Resolve symbol index l_tmp from the current object or another selected object.
l_resolve_symbol:
      LDY   #NOBJ_SYMBOL_COUNT
      LDA   l_tmp
      CMP   (l_obj),Y
      INY
      LDA   l_tmp+1
      SBC   (l_obj),Y
      long_bcs @bad
      LDY   #NOBJ_SYMBOL_OFFSET
      LDA   (l_obj),Y
      STA   l_member
      INY
      LDA   (l_obj),Y
      STA   l_member+1
      CLC
      LDA   l_member
      ADC   l_obj
      STA   l_member
      LDA   l_member+1
      ADC   l_obj+1
      STA   l_member+1
      LDA   l_tmp+1
      long_bne @bad              ; native table cap is below 256 today
      LDX   l_tmp
@skip:
      CPX   #0
      BEQ   @target
      LDA   #NOBJ_SYM_HEADER_SIZE
      JSR   l_object_has
      BCC   @bad
      LDY   #NOBJ_SYM_NAME_LEN
      LDA   (l_member),Y
      CLC
      ADC   #NOBJ_SYM_HEADER_SIZE
      JSR   l_advance_member
      DEX
      BRA   @skip
@target:
      LDA   #NOBJ_SYM_HEADER_SIZE
      JSR   l_object_has
      BCC   @bad
      LDY   #NOBJ_SYM_NAME_LEN
      LDA   (l_member),Y
      STA   l_name_len
      CLC
      ADC   #NOBJ_SYM_HEADER_SIZE
      JSR   l_object_has
      BCC   @bad
      CLC
      LDA   l_member
      ADC   #NOBJ_SYM_HEADER_SIZE
      STA   l_name
      LDA   l_member+1
      ADC   #0
      STA   l_name+1
      LDY   #NOBJ_SYM_VALUE
      LDA   (l_member),Y
      STA   l_address
      INY
      LDA   (l_member),Y
      STA   l_address+1
      INY
      LDA   (l_member),Y
      CMP   l_section_count
      BCC   @section
      CMP   #NOBJ_SYM_ABSOLUTE
      BEQ   @ok
      CMP   #NOBJ_SYM_UNDEFINED
      BNE   @bad
      LDY   #NOBJ_SYM_FLAGS
      LDA   (l_member),Y
      AND   #NOBJ_SYM_GLOBAL
      BEQ   @bad
      JMP   l_resolve_global
@section:
      CLC
      ADC   l_section_base
      TAX
      CLC
      LDA   l_address
      ADC   l_sec_offset_l,X
      STA   l_address
      LDA   l_address+1
      ADC   l_sec_offset_h,X
      STA   l_address+1
      CLC
      LDA   l_address
      ADC   nlink_load_base
      STA   l_address
      LDA   l_address+1
      ADC   nlink_load_base+1
      STA   l_address+1
@ok:
      CLC
      RTS
@bad:
      SEC
      RTS

; Resolve l_name against exported definitions in the final selected object set.
l_resolve_global:
      STZ   l_global_found
      STZ   l_scan_object
@object:
      LDA   l_scan_object
      CMP   nlink_object_count
      BEQ   @objects_done
      JSR   l_select_object
      BCS   @bad
      LDY   #NOBJ_SECTION_COUNT
      LDA   (l_obj),Y
      STA   l_section_count
      JSR   l_find_global_definition
      BCC   @found
      CMP   #1
      BEQ   @bad
@next:
      INC   l_scan_object
      BRA   @object
@found:
      LDA   l_global_found
      BNE   @bad                  ; duplicate exported definition
      INC   l_global_found
      BRA   @next
@objects_done:
      JSR   l_find_config_definition
      BCS   @config_missing
      LDA   l_global_found
      BNE   @bad                  ; object and config both define the name
      INC   l_global_found
@config_missing:
      LDA   l_global_found
      BEQ   @bad
@ok:
      CLC
      RTS
@bad:
      SEC
      RTS

; Find l_name in configured absolute symbols and publish its exact address.
l_find_config_definition:
      LDX   #0
@symbol:
      CPX   nlink_config_symbol_count
      BCS   @missing
      LDA   nlink_config_symbol_name_len,X
      CMP   l_name_len
      BNE   @next
      STX   l_rule_index
      TXA
      ASL
      ASL
      ASL
      ASL
      CLC
      ADC   #<nlink_config_symbol_names
      STA   l_tmp
      LDA   #>nlink_config_symbol_names
      ADC   #0
      STA   l_tmp+1
      LDY   #0
@name:
      CPY   l_name_len
      BCS   @found
      LDA   (l_tmp),Y
      CMP   (l_name),Y
      BNE   @next_saved
      INY
      BRA   @name
@next_saved:
      LDX   l_rule_index
@next:
      INX
      BRA   @symbol
@found:
      LDX   l_rule_index
      LDA   nlink_config_symbol_value_l,X
      STA   l_address
      LDA   nlink_config_symbol_value_h,X
      STA   l_address+1
      CLC
      RTS
@missing:
      SEC
      RTS

; Find exported l_name in the selected object.
; Carry clear = found, carry set/A=0 = absent, carry set/A=1 = malformed.
l_find_global_definition:
      LDY   #NOBJ_SYMBOL_COUNT
      LDA   (l_obj),Y
      STA   l_left
      INY
      LDA   (l_obj),Y
      long_bne @bad
      STZ   l_left+1
      LDY   #NOBJ_SYMBOL_OFFSET
      LDA   (l_obj),Y
      STA   l_member
      INY
      LDA   (l_obj),Y
      STA   l_member+1
      CLC
      LDA   l_member
      ADC   l_obj
      STA   l_member
      LDA   l_member+1
      ADC   l_obj+1
      STA   l_member+1
@symbol:
      LDA   l_left
      long_beq @missing
      LDA   #NOBJ_SYM_HEADER_SIZE
      JSR   l_object_has
      long_bcc @bad
      LDY   #NOBJ_SYM_NAME_LEN
      LDA   (l_member),Y
      STA   l_tmp
      CLC
      ADC   #NOBJ_SYM_HEADER_SIZE
      JSR   l_object_has
      long_bcc @bad
      LDY   #NOBJ_SYM_FLAGS
      LDA   (l_member),Y
      AND   #NOBJ_SYM_GLOBAL
      long_beq @advance
      LDY   #NOBJ_SYM_SECTION
      LDA   (l_member),Y
      CMP   #NOBJ_SYM_UNDEFINED
      long_beq @advance
      CMP   l_section_count
      BCC   @compare_name
      CMP   #NOBJ_SYM_ABSOLUTE
      BNE   @bad
@compare_name:
      LDA   l_tmp
      CMP   l_name_len
      long_bne @advance
      CLC
      LDA   l_member
      ADC   #NOBJ_SYM_HEADER_SIZE
      STA   l_member_name
      LDA   l_member+1
      ADC   #0
      STA   l_member_name+1
      LDY   #0
@compare:
      CPY   l_name_len
      BCS   @matched
      LDA   (l_member_name),Y
      CMP   (l_name),Y
      long_bne @advance
      INY
      BRA   @compare
@matched:
      LDY   #NOBJ_SYM_VALUE
      LDA   (l_member),Y
      STA   l_address
      INY
      LDA   (l_member),Y
      STA   l_address+1
      INY
      LDA   (l_member),Y
      CMP   #NOBJ_SYM_ABSOLUTE
      BEQ   @found
      CLC
      ADC   l_section_base
      TAX
      CLC
      LDA   l_address
      ADC   l_sec_offset_l,X
      STA   l_address
      LDA   l_address+1
      ADC   l_sec_offset_h,X
      STA   l_address+1
      CLC
      LDA   l_address
      ADC   nlink_load_base
      STA   l_address
      LDA   l_address+1
      ADC   nlink_load_base+1
      STA   l_address+1
@found:
      CLC
      RTS
@advance:
      CLC
      LDA   l_tmp
      ADC   #NOBJ_SYM_HEADER_SIZE
      JSR   l_advance_member
      DEC   l_left
      JMP   @symbol
@missing:
      LDA   #0
      SEC
      RTS
@bad:
      LDA   #1
      SEC
      RTS

; Advance l_member by A bytes.
l_advance_member:
      CLC
      ADC   l_member
      STA   l_member
      BCC   :+
      INC   l_member+1
:     RTS

; Carry set if A bytes beginning at l_member fit in the object.
l_object_has:
      STA   l_member_len
      SEC
      LDA   l_member
      SBC   l_obj
      STA   l_src
      LDA   l_member+1
      SBC   l_obj+1
      STA   l_src+1
      CLC
      LDA   l_src
      ADC   l_member_len
      STA   l_src
      LDA   l_src+1
      ADC   #0
      STA   l_src+1
      CMP   l_obj_len+1
      BCC   @yes
      BNE   @no
      LDA   l_src
      CMP   l_obj_len
      BCC   @yes
      BEQ   @yes
@no:
      CLC
      RTS
@yes:
      SEC
      RTS

; Apply the selected relocation after symbol resolution and addend adjustment.
l_apply_relocation:
      LDA   l_reloc_type
      CMP   #NOBJ_RELOC_ABS16
      long_beq @abs16
      CMP   #NOBJ_RELOC_ABS8
      BEQ   @abs8
      CMP   #NOBJ_RELOC_LO8
      BEQ   @byte
      CMP   #NOBJ_RELOC_HI8
      BEQ   @high
      CMP   #NOBJ_RELOC_REL8
      long_bne @bad

      ; REL8 is relative to the byte after the one-byte patch.
      LDX   l_patch_flat
      CLC
      LDA   nlink_load_base
      ADC   l_sec_offset_l,X
      STA   l_addend
      LDA   nlink_load_base+1
      ADC   l_sec_offset_h,X
      STA   l_addend+1
      CLC
      LDA   l_addend
      ADC   l_offset
      STA   l_addend
      LDA   l_addend+1
      ADC   l_offset+1
      STA   l_addend+1
      INC   l_addend
      BNE   :+
      INC   l_addend+1
:     SEC
      LDA   l_address
      SBC   l_addend
      STA   l_address
      LDA   l_address+1
      SBC   l_addend+1
      STA   l_address+1
      BEQ   @positive_rel
      CMP   #$FF
      BNE   @bad
      LDA   l_address
      CMP   #$80
      BCC   @bad
      BRA   @byte
@positive_rel:
      LDA   l_address
      CMP   #$80
      BCS   @bad
      BRA   @byte

@abs8:
      LDA   l_address+1
      BNE   @bad
      BRA   @byte
@high:
      LDA   l_address+1
      STA   l_address
@byte:
      LDA   #0
      JSR   l_patch_pointer
      BCS   @bad
      LDY   #0
      LDA   l_address
      STA   (l_tmp),Y
      CLC
      RTS
@abs16:
      LDA   #1
      JSR   l_patch_pointer
      BCS   @bad
      LDY   #0
      LDA   l_address
      STA   (l_tmp),Y
      INY
      LDA   l_address+1
      STA   (l_tmp),Y
      CLC
      RTS
@bad:
      SEC
      RTS

; A is the final byte relative to l_offset (0 for byte, 1 for word).
; Carry clear returns the output patch pointer in l_tmp.
l_patch_pointer:
      STA   l_member_len
      CLC
      LDA   l_offset
      ADC   l_member_len
      STA   l_src
      LDA   l_offset+1
      ADC   #0
      STA   l_src+1
      BCS   @bad
      LDX   l_patch_flat
      LDA   l_src+1
      CMP   l_sec_size_h,X
      BCC   @in_range
      BNE   @bad
      LDA   l_src
      CMP   l_sec_size_l,X
      BCS   @bad
@in_range:
      CLC
      LDA   nlink_output_ptr
      ADC   #2
      STA   l_tmp
      LDA   nlink_output_ptr+1
      ADC   #0
      STA   l_tmp+1
      CLC
      LDA   l_tmp
      ADC   l_sec_offset_l,X
      STA   l_tmp
      LDA   l_tmp+1
      ADC   l_sec_offset_h,X
      STA   l_tmp+1
      CLC
      LDA   l_tmp
      ADC   l_offset
      STA   l_tmp
      LDA   l_tmp+1
      ADC   l_offset+1
      STA   l_tmp+1
      CLC
      RTS
@bad:
      SEC
      RTS

l_take_reloc:
      LDA   l_reloc_bytes
      ORA   l_reloc_bytes+1
      BEQ   @empty
      LDY   #0
      LDA   (l_reloc),Y
      PHA
      INC   l_reloc
      BNE   :+
      INC   l_reloc+1
:     LDA   l_reloc_bytes
      BNE   :+
      DEC   l_reloc_bytes+1
:     DEC   l_reloc_bytes
      PLA
      CLC
      RTS
@empty:
      SEC
      RTS

l_append:
      PHA
      LDA   nlink_config_enabled
      BNE   @configured
      LDA   l_cap
      ORA   l_cap+1
      BEQ   @full
      PLA
      LDY   #0
      STA   (l_dst),Y
      INC   l_dst
      BNE   :+
      INC   l_dst+1
:     LDA   l_cap
      BNE   :+
      DEC   l_cap+1
:     DEC   l_cap
      INC   l_payload_len
      BNE   :+
      INC   l_payload_len+1
:     CLC
      RTS
@configured:
      PLA
      LDY   #0
      STA   (l_dst),Y
      INC   l_dst
      BNE   :+
      INC   l_dst+1
:     CLC
      RTS
@full:
      PLA
      SEC
      RTS

l_bad_object:
      LDA   #NLINK_ERR_OBJECT
      BRA   l_fail
l_bad_output:
      LDA   #NLINK_ERR_OUTPUT
      BRA   l_fail
l_bad_relocation:
      LDA   #NLINK_ERR_RELOCATION
      BRA   l_fail
l_bad_config:
      LDA   #NLINK_ERR_CONFIG
l_fail:
      STA   nlink_error
      LDA   #1
      RTS

      .segment "RODATA"
l_section_bits:
      .byte $01, $02, $04, $08, $10, $20, $40, $80

; NL disk-loaded worker. Section reachability and map generation live here to
; keep lower RAM free for the resident linker.

      .setcpu "w65c02"
      .include "nobj.inc"
      .include "novalink.inc"
      .include "nl_worker.inc"

.macro long_bcs target
      BCC   :+
      JMP   target
:
.endmacro

.macro long_beq target
      BNE   :+
      JMP   target
:
.endmacro

.macro long_bne target
      BEQ   :+
      JMP   target
:
.endmacro

.macro long_bcc target
      BCS   :+
      JMP   target
:
.endmacro

      .segment "ZEROPAGE"
w_obj:          .res 2
w_dst:          .res 2
w_left:         .res 2
w_member:       .res 2
w_reloc:        .res 2
w_name:         .res 2
w_tmp:          .res 2
w_address:      .res 2

      .segment "BSS"
w_object:       .res 1
w_section:      .res 1
w_section_count:.res 1
w_section_base: .res 1
w_name_len:     .res 1
w_symbol_count: .res 2
w_symbol_index: .res 2
w_reloc_count:  .res 2
w_changed:      .res 1
w_source_object:.res 1
w_scan_object:  .res 1
w_found:        .res 1
w_archive_members:.res 1
w_find_mode:    .res 1
w_saved_member: .res 2
w_saved_count:  .res 2

      .segment "CODE"
      .export nlw_main

nlw_main:
      STZ   NLW_ERROR
      LDA   NLW_COMMAND
      CMP   #NLW_COMMAND_MAP
      BEQ   @report
      CMP   #NLW_COMMAND_LABEL
      BNE   @gc
@report:
      JSR   w_write_map
      BCS   w_output_error
      LDA   #0
      RTS
@gc:
      CMP   #NLW_COMMAND_GC
      BNE   @command_error
      JSR   w_collect_sections
      BCS   w_object_error
      LDA   #0
      RTS
@command_error:
      LDA   #NLW_ERROR_COMMAND
      BRA   w_fail
w_output_error:
      LDA   #NLW_ERROR_OUTPUT
      BRA   w_fail
w_object_error:
      LDA   #NLW_ERROR_OBJECT
w_fail:
      STA   NLW_ERROR
      LDA   #1
      RTS

; Seed direct-entry/export roots, then close over relocations until no new
; section becomes reachable. One byte covers each object's eight sections.
w_collect_sections:
      STZ   NLW_GC_READY
      LDX   #NLINK_OBJECT_CAP-1
@clear:
      STZ   NLW_LIVE_MASK,X
      DEX
      BPL   @clear
      LDA   NLW_OBJECT_COUNT
      long_beq @bad
      CMP   #NLINK_OBJECT_CAP+1
      long_bcs @bad
      LDA   NLW_ROOT_COUNT
      long_beq @bad
      CMP   NLW_OBJECT_COUNT
      BCC   @seed
      long_bne @bad
@seed:
      STZ   w_object
@seed_object:
      LDA   w_object
      CMP   NLW_ROOT_COUNT
      BEQ   @pass
      JSR   w_select_object
      long_bcs @bad
      LDY   #NOBJ_SECTION_COUNT
      LDA   (w_obj),Y
      STA   w_section_count
      LDY   #NOBJ_ENTRY_SECTION
      LDA   (w_obj),Y
      CMP   #NOBJ_SYM_UNDEFINED
      BEQ   @exports
      CMP   w_section_count
      long_bcs @bad
      LDX   w_object
      JSR   w_mark_section
@exports:
      JSR   w_seed_export_sections
      long_bcs @bad
      INC   w_object
      BRA   @seed_object

@pass:
      STZ   w_changed
      STZ   w_object
@pass_object:
      LDA   w_object
      CMP   NLW_OBJECT_COUNT
      long_beq @pass_done
      JSR   w_select_object
      long_bcs @bad
      LDY   #NOBJ_RELOC_OFFSET
      LDA   (w_obj),Y
      CLC
      ADC   w_obj
      STA   w_reloc
      INY
      LDA   (w_obj),Y
      ADC   w_obj+1
      STA   w_reloc+1
      LDY   #NOBJ_RELOC_COUNT
      LDA   (w_obj),Y
      STA   w_reloc_count
      INY
      LDA   (w_obj),Y
      STA   w_reloc_count+1
@reloc:
      LDA   w_reloc_count
      ORA   w_reloc_count+1
      BEQ   @next_pass_object
      LDA   w_object
      JSR   w_select_object
      BCS   @bad
      LDY   #NOBJ_SECTION_COUNT
      LDA   (w_obj),Y
      STA   w_section_count
      LDY   #NOBJ_REL_SECTION
      LDA   (w_reloc),Y
      CMP   w_section_count
      BCS   @bad
      LDX   w_object
      JSR   w_mask_has_section
      BCC   @next_reloc
      LDY   #NOBJ_REL_SYMBOL
      LDA   (w_reloc),Y
      STA   w_symbol_index
      INY
      LDA   (w_reloc),Y
      STA   w_symbol_index+1
      LDA   w_object
      STA   w_source_object
      JSR   w_follow_symbol
      BCS   @bad
@next_reloc:
      CLC
      LDA   w_reloc
      ADC   #NOBJ_RELOC_SIZE
      STA   w_reloc
      BCC   :+
      INC   w_reloc+1
:     LDA   w_reloc_count
      BNE   :+
      DEC   w_reloc_count+1
:     DEC   w_reloc_count
      BRA   @reloc
@next_pass_object:
      INC   w_object
      JMP   @pass_object
@pass_done:
      LDA   w_changed
      long_bne @pass
      JSR   w_check_duplicate_globals
      long_bcs @bad
      LDA   #1
      STA   NLW_GC_READY
      CLC
      RTS
@bad:
      SEC
      RTS

; Exported sections in direct objects are the explicit keep mechanism.
w_seed_export_sections:
      LDY   #NOBJ_SYMBOL_COUNT
      LDA   (w_obj),Y
      STA   w_symbol_count
      INY
      LDA   (w_obj),Y
      STA   w_symbol_count+1
      LDY   #NOBJ_SYMBOL_OFFSET
      LDA   (w_obj),Y
      CLC
      ADC   w_obj
      STA   w_member
      INY
      LDA   (w_obj),Y
      ADC   w_obj+1
      STA   w_member+1
@symbol:
      LDA   w_symbol_count
      ORA   w_symbol_count+1
      BEQ   @ok
      LDY   #NOBJ_SYM_FLAGS
      LDA   (w_member),Y
      AND   #NOBJ_SYM_GLOBAL
      BEQ   @next
      LDY   #NOBJ_SYM_SECTION
      LDA   (w_member),Y
      CMP   w_section_count
      BCC   @mark
      CMP   #NOBJ_SYM_ABSOLUTE
      BEQ   @next
      CMP   #NOBJ_SYM_UNDEFINED
      BNE   @bad
      BRA   @next
@mark:
      LDX   w_object
      JSR   w_mark_section
@next:
      JSR   w_next_symbol
      LDA   w_symbol_count
      BNE   :+
      DEC   w_symbol_count+1
:     DEC   w_symbol_count
      BRA   @symbol
@ok:
      CLC
      RTS
@bad:
      SEC
      RTS

; Follow one relocation's symbol to a local section, an absolute value, or the
; single selected object that defines its global name.
w_follow_symbol:
      LDY   #NOBJ_SYMBOL_COUNT+1
      LDA   w_symbol_index+1
      CMP   (w_obj),Y
      BCC   @index_ok
      BNE   @bad
      DEY
      LDA   w_symbol_index
      CMP   (w_obj),Y
      BCS   @bad
@index_ok:
      LDY   #NOBJ_SYMBOL_OFFSET
      LDA   (w_obj),Y
      CLC
      ADC   w_obj
      STA   w_member
      INY
      LDA   (w_obj),Y
      ADC   w_obj+1
      STA   w_member+1
@skip:
      LDA   w_symbol_index
      ORA   w_symbol_index+1
      BEQ   @target
      JSR   w_next_symbol
      LDA   w_symbol_index
      BNE   :+
      DEC   w_symbol_index+1
:     DEC   w_symbol_index
      BRA   @skip
@target:
      LDY   #NOBJ_SYM_SECTION
      LDA   (w_member),Y
      CMP   w_section_count
      BCC   @local
      CMP   #NOBJ_SYM_ABSOLUTE
      BEQ   @ok
      CMP   #NOBJ_SYM_UNDEFINED
      BNE   @bad
      INY
      LDA   (w_member),Y
      AND   #NOBJ_SYM_GLOBAL
      BEQ   @bad
      LDY   #NOBJ_SYM_NAME_LEN
      LDA   (w_member),Y
      STA   w_name_len
      CLC
      LDA   w_member
      ADC   #NOBJ_SYM_HEADER_SIZE
      STA   w_name
      LDA   w_member+1
      ADC   #0
      STA   w_name+1
      LDA   #1
      STA   w_find_mode
      JMP   w_find_global_definition
@local:
      LDX   w_source_object
      JSR   w_mark_section
@ok:
      CLC
      RTS
@bad:
      SEC
      RTS

w_find_global_definition:
      STZ   w_found
      STZ   w_scan_object
@object:
      LDA   w_scan_object
      CMP   NLW_OBJECT_COUNT
      long_beq @done
      JSR   w_select_object
      long_bcs @bad
      LDY   #NOBJ_SECTION_COUNT
      LDA   (w_obj),Y
      STA   w_section_count
      LDY   #NOBJ_SYMBOL_COUNT
      LDA   (w_obj),Y
      STA   w_symbol_count
      INY
      LDA   (w_obj),Y
      STA   w_symbol_count+1
      LDY   #NOBJ_SYMBOL_OFFSET
      LDA   (w_obj),Y
      CLC
      ADC   w_obj
      STA   w_member
      INY
      LDA   (w_obj),Y
      ADC   w_obj+1
      STA   w_member+1
@symbol:
      LDA   w_symbol_count
      ORA   w_symbol_count+1
      BEQ   @next_object
      LDY   #NOBJ_SYM_FLAGS
      LDA   (w_member),Y
      AND   #NOBJ_SYM_GLOBAL
      BEQ   @next_symbol
      LDY   #NOBJ_SYM_SECTION
      LDA   (w_member),Y
      CMP   #NOBJ_SYM_UNDEFINED
      BEQ   @next_symbol
      LDY   #NOBJ_SYM_NAME_LEN
      LDA   (w_member),Y
      CMP   w_name_len
      BNE   @next_symbol
      CLC
      LDA   w_member
      ADC   #NOBJ_SYM_HEADER_SIZE
      STA   w_tmp
      LDA   w_member+1
      ADC   #0
      STA   w_tmp+1
      LDY   #0
@name:
      CPY   w_name_len
      BCS   @matched
      LDA   (w_tmp),Y
      CMP   (w_name),Y
      BNE   @next_symbol
      INY
      BRA   @name
@matched:
      LDA   w_found
      BNE   @bad
      INC   w_found
      LDA   w_find_mode
      BEQ   @next_symbol
      LDY   #NOBJ_SYM_SECTION
      LDA   (w_member),Y
      CMP   #NOBJ_SYM_ABSOLUTE
      BEQ   @next_symbol
      CMP   w_section_count
      BCS   @bad
      LDX   w_scan_object
      JSR   w_mark_section
@next_symbol:
      JSR   w_next_symbol
      LDA   w_symbol_count
      BNE   :+
      DEC   w_symbol_count+1
:     DEC   w_symbol_count
      BRA   @symbol
@next_object:
      INC   w_scan_object
      JMP   @object
@done:
      JSR   w_find_config_definition
      BCS   @config_missing
      LDA   w_found
      BNE   @bad
      INC   w_found
@config_missing:
      LDA   w_found
      BNE   @ok
      LDA   w_find_mode
      BEQ   @bad
      JSR   w_extract_archive_definition
      BCS   @bad
      JMP   w_find_global_definition
@ok:
      CLC
      RTS
@bad:
      SEC
      RTS

; Config symbols are absolute definitions. They satisfy reachability without
; marking a section or extracting an archive member.
w_find_config_definition:
      LDX   #0
@symbol:
      CPX   NLW_CONFIG_SYMBOL_COUNT
      BCS   @missing
      LDA   NLW_CONFIG_SYMBOL_NAME_LEN,X
      CMP   w_name_len
      BNE   @next
      STX   w_scan_object
      TXA
      ASL
      ASL
      ASL
      ASL
      CLC
      ADC   #<NLW_CONFIG_SYMBOL_NAMES
      STA   w_tmp
      LDA   #>NLW_CONFIG_SYMBOL_NAMES
      ADC   #0
      STA   w_tmp+1
      LDY   #0
@name:
      CPY   w_name_len
      BCS   @found
      LDA   (w_tmp),Y
      CMP   (w_name),Y
      BNE   @next_saved
      INY
      BRA   @name
@next_saved:
      LDX   w_scan_object
@next:
      INX
      BRA   @symbol
@found:
      CLC
      RTS
@missing:
      SEC
      RTS

; Reject duplicate globals across the final selected object set, including
; exports that no live relocation happens to reference. Lookup mode zero only
; counts definitions: it neither marks sections nor extracts archive members.
w_check_duplicate_globals:
      STZ   w_object
@object:
      LDA   w_object
      CMP   NLW_OBJECT_COUNT
      long_beq @ok
      JSR   w_select_object
      long_bcs @bad
      LDY   #NOBJ_SYMBOL_COUNT
      LDA   (w_obj),Y
      STA   w_symbol_count
      INY
      LDA   (w_obj),Y
      STA   w_symbol_count+1
      LDY   #NOBJ_SYMBOL_OFFSET
      LDA   (w_obj),Y
      CLC
      ADC   w_obj
      STA   w_member
      INY
      LDA   (w_obj),Y
      ADC   w_obj+1
      STA   w_member+1
@symbol:
      LDA   w_symbol_count
      ORA   w_symbol_count+1
      BEQ   @next_object
      LDY   #NOBJ_SYM_FLAGS
      LDA   (w_member),Y
      AND   #NOBJ_SYM_GLOBAL
      BEQ   @next_symbol
      LDY   #NOBJ_SYM_SECTION
      LDA   (w_member),Y
      CMP   #NOBJ_SYM_UNDEFINED
      BEQ   @next_symbol
      LDY   #NOBJ_SYM_NAME_LEN
      LDA   (w_member),Y
      STA   w_name_len
      CLC
      LDA   w_member
      ADC   #NOBJ_SYM_HEADER_SIZE
      STA   w_name
      LDA   w_member+1
      ADC   #0
      STA   w_name+1
      LDA   w_member
      STA   w_saved_member
      LDA   w_member+1
      STA   w_saved_member+1
      LDA   w_symbol_count
      STA   w_saved_count
      LDA   w_symbol_count+1
      STA   w_saved_count+1
      STZ   w_find_mode
      JSR   w_find_global_definition
      BCS   @bad
      LDA   w_saved_member
      STA   w_member
      LDA   w_saved_member+1
      STA   w_member+1
      LDA   w_saved_count
      STA   w_symbol_count
      LDA   w_saved_count+1
      STA   w_symbol_count+1
@next_symbol:
      JSR   w_next_symbol
      LDA   w_symbol_count
      BNE   :+
      DEC   w_symbol_count+1
:     DEC   w_symbol_count
      BRA   @symbol
@next_object:
      INC   w_object
      JMP   @object
@ok:
      CLC
      RTS
@bad:
      SEC
      RTS

; Select the first archive member exporting w_name. nlink_prepare has already
; validated every length-prefixed NOBJ, so this worker only performs lookup.
w_extract_archive_definition:
      LDA   NLW_OBJECT_COUNT
      CMP   #NLINK_OBJECT_CAP
      long_bcs @bad
      LDA   NLW_LIBRARY_PTR
      STA   w_obj
      LDA   NLW_LIBRARY_PTR+1
      STA   w_obj+1
      LDY   #NLIB_MEMBER_COUNT
      LDA   (w_obj),Y
      STA   w_archive_members
      CLC
      LDA   w_obj
      ADC   #NLIB_HEADER_SIZE
      STA   w_dst
      LDA   w_obj+1
      ADC   #0
      STA   w_dst+1
@archive:
      LDA   w_archive_members
      long_beq @bad
      LDY   #0
      LDA   (w_dst),Y
      STA   w_address
      INY
      LDA   (w_dst),Y
      STA   w_address+1
      CLC
      LDA   w_dst
      ADC   #2
      STA   w_obj
      LDA   w_dst+1
      ADC   #0
      STA   w_obj+1
      LDY   #NOBJ_SYMBOL_COUNT
      LDA   (w_obj),Y
      STA   w_symbol_count
      INY
      LDA   (w_obj),Y
      STA   w_symbol_count+1
      LDY   #NOBJ_SYMBOL_OFFSET
      LDA   (w_obj),Y
      CLC
      ADC   w_obj
      STA   w_member
      INY
      LDA   (w_obj),Y
      ADC   w_obj+1
      STA   w_member+1
@symbol:
      LDA   w_symbol_count
      ORA   w_symbol_count+1
      BEQ   @next_archive
      LDY   #NOBJ_SYM_FLAGS
      LDA   (w_member),Y
      AND   #NOBJ_SYM_GLOBAL
      BEQ   @next_symbol
      LDY   #NOBJ_SYM_SECTION
      LDA   (w_member),Y
      CMP   #NOBJ_SYM_UNDEFINED
      BEQ   @next_symbol
      LDY   #NOBJ_SYM_NAME_LEN
      LDA   (w_member),Y
      CMP   w_name_len
      BNE   @next_symbol
      CLC
      LDA   w_member
      ADC   #NOBJ_SYM_HEADER_SIZE
      STA   w_tmp
      LDA   w_member+1
      ADC   #0
      STA   w_tmp+1
      LDY   #0
@name:
      CPY   w_name_len
      BCS   @select
      LDA   (w_tmp),Y
      CMP   (w_name),Y
      BNE   @next_symbol
      INY
      BRA   @name
@select:
      LDX   NLW_OBJECT_COUNT
      LDA   w_obj
      STA   NLW_OBJECT_PTR_L,X
      LDA   w_obj+1
      STA   NLW_OBJECT_PTR_H,X
      LDA   w_address
      STA   NLW_OBJECT_LEN_L,X
      LDA   w_address+1
      STA   NLW_OBJECT_LEN_H,X
      INC   NLW_OBJECT_COUNT
      CLC
      RTS
@next_symbol:
      JSR   w_next_symbol
      LDA   w_symbol_count
      BNE   :+
      DEC   w_symbol_count+1
:     DEC   w_symbol_count
      BRA   @symbol
@next_archive:
      CLC
      LDA   w_obj
      ADC   w_address
      STA   w_dst
      LDA   w_obj+1
      ADC   w_address+1
      STA   w_dst+1
      DEC   w_archive_members
      JMP   @archive
@bad:
      SEC
      RTS

w_mark_section:
      TAY
      LDA   w_section_bits,Y
      AND   NLW_LIVE_MASK,X
      BNE   @done
      LDA   w_section_bits,Y
      ORA   NLW_LIVE_MASK,X
      STA   NLW_LIVE_MASK,X
      INC   w_changed
@done:
      RTS

; A=section, X=object. Carry reports the raw in-progress reachability mask.
w_mask_has_section:
      TAY
      LDA   w_section_bits,Y
      AND   NLW_LIVE_MASK,X
      BEQ   @no
      SEC
      RTS
@no:
      CLC
      RTS

; Map output predates GC, so an unset ready byte means show all sections.
w_section_is_live:
      PHA
      LDA   NLW_GC_READY
      BNE   @mask
      PLA
      SEC
      RTS
@mask:
      PLA
      JMP   w_mask_has_section

w_write_map:
      LDA   NLW_MAP_PTR
      STA   w_dst
      LDA   NLW_MAP_PTR+1
      STA   w_dst+1
      LDA   NLW_MAP_CAP
      STA   w_left
      LDA   NLW_MAP_CAP+1
      STA   w_left+1
      STZ   NLW_MAP_LEN
      STZ   NLW_MAP_LEN+1
      LDA   NLW_COMMAND
      CMP   #NLW_COMMAND_LABEL
      long_beq @exports
      LDA   #<w_title
      LDX   #>w_title
      JSR   w_text
      long_bcs @bad
      LDA   NLW_LOAD_BASE
      STA   w_address
      LDA   NLW_LOAD_BASE+1
      STA   w_address+1
      JSR   w_hex_word
      long_bcs @bad
      LDA   #<w_size
      LDX   #>w_size
      JSR   w_text
      long_bcs @bad
      SEC
      LDA   NLW_OUTPUT_LEN
      SBC   #2
      STA   w_address
      LDA   NLW_OUTPUT_LEN+1
      SBC   #0
      STA   w_address+1
      JSR   w_hex_word
      long_bcs @bad
      LDA   #<w_sections
      LDX   #>w_sections
      JSR   w_text
      long_bcs @bad

      STZ   w_object
@section_object:
      LDA   w_object
      CMP   NLW_OBJECT_COUNT
      long_beq @exports
      JSR   w_select_object
      long_bcs @bad
      LDY   #NOBJ_SECTION_COUNT
      LDA   (w_obj),Y
      STA   w_section_count
      CLC
      LDA   w_obj
      ADC   #NOBJ_HEADER_SIZE
      STA   w_member
      LDA   w_obj+1
      ADC   #0
      STA   w_member+1
      STZ   w_section
@section:
      LDA   w_section
      CMP   w_section_count
      long_beq @next_section_object
      LDX   w_object
      JSR   w_section_is_live
      long_bcc @skip_section
      LDA   w_section
      CLC
      ADC   w_section_base
      TAX
      CLC
      LDA   NLW_LOAD_BASE
      ADC   NLW_SEC_OFFSET_L,X
      STA   w_address
      LDA   NLW_LOAD_BASE+1
      ADC   NLW_SEC_OFFSET_H,X
      STA   w_address+1
      LDA   #'$'
      JSR   w_append
      long_bcs @bad
      JSR   w_hex_word
      long_bcs @bad
      LDA   #' '
      JSR   w_append
      long_bcs @bad
      LDA   #'$'
      JSR   w_append
      long_bcs @bad
      LDX   w_section
      TXA
      CLC
      ADC   w_section_base
      TAX
      LDA   NLW_SEC_SIZE_L,X
      STA   w_address
      LDA   NLW_SEC_SIZE_H,X
      STA   w_address+1
      JSR   w_hex_word
      long_bcs @bad
      LDA   #' '
      JSR   w_append
      long_bcs @bad
      LDY   #NOBJ_SEC_NAME_LEN
      LDA   (w_member),Y
      STA   w_name_len
      CLC
      LDA   w_member
      ADC   #NOBJ_SEC_HEADER_SIZE
      STA   w_name
      LDA   w_member+1
      ADC   #0
      STA   w_name+1
      JSR   w_copy_name
      long_bcs @bad
      LDA   #$0A
      JSR   w_append
      long_bcs @bad
@skip_section:
      JSR   w_next_section
      INC   w_section
      JMP   @section
@next_section_object:
      INC   w_object
      JMP   @section_object

@exports:
      LDA   NLW_COMMAND
      CMP   #NLW_COMMAND_LABEL
      BEQ   @export_start
      LDA   #<w_exports
      LDX   #>w_exports
      JSR   w_text
      long_bcs @bad
@export_start:
      STZ   w_object
@export_object:
      LDA   w_object
      CMP   NLW_OBJECT_COUNT
      long_beq @ok
      JSR   w_select_object
      long_bcs @bad
      LDY   #NOBJ_SECTION_COUNT
      LDA   (w_obj),Y
      STA   w_section_count
      LDY   #NOBJ_SYMBOL_COUNT
      LDA   (w_obj),Y
      STA   w_symbol_count
      INY
      LDA   (w_obj),Y
      STA   w_symbol_count+1
      LDY   #NOBJ_SYMBOL_OFFSET
      LDA   (w_obj),Y
      CLC
      ADC   w_obj
      STA   w_member
      INY
      LDA   (w_obj),Y
      ADC   w_obj+1
      STA   w_member+1
@export:
      LDA   w_symbol_count
      ORA   w_symbol_count+1
      long_beq @next_export_object
      LDY   #NOBJ_SYM_FLAGS
      LDA   (w_member),Y
      AND   #NOBJ_SYM_GLOBAL
      long_beq @skip_export
      LDY   #NOBJ_SYM_SECTION
      LDA   (w_member),Y
      CMP   #NOBJ_SYM_UNDEFINED
      long_beq @skip_export
      CMP   #NOBJ_SYM_ABSOLUTE
      BNE   @section_export
      LDY   #NOBJ_SYM_VALUE
      LDA   (w_member),Y
      STA   w_address
      INY
      LDA   (w_member),Y
      STA   w_address+1
      BRA   @write_export
@section_export:
      CMP   w_section_count
      long_bcs @bad
      STA   w_section
      LDX   w_object
      JSR   w_section_is_live
      long_bcc @skip_export
      LDA   w_section
      CLC
      ADC   w_section_base
      TAX
      LDY   #NOBJ_SYM_VALUE
      CLC
      LDA   (w_member),Y
      ADC   NLW_SEC_OFFSET_L,X
      STA   w_address
      INY
      LDA   (w_member),Y
      ADC   NLW_SEC_OFFSET_H,X
      STA   w_address+1
      CLC
      LDA   w_address
      ADC   NLW_LOAD_BASE
      STA   w_address
      LDA   w_address+1
      ADC   NLW_LOAD_BASE+1
      STA   w_address+1
@write_export:
      LDA   NLW_COMMAND
      CMP   #NLW_COMMAND_LABEL
      BEQ   @write_label
      LDA   #'$'
      JSR   w_append
      long_bcs @bad
      JSR   w_hex_word
      long_bcs @bad
      LDA   #' '
      JSR   w_append
      long_bcs @bad
      BRA   @write_name
@write_label:
      LDA   #<w_label_prefix
      LDX   #>w_label_prefix
      JSR   w_text
      long_bcs @bad
      JSR   w_hex_word
      long_bcs @bad
      LDA   #<w_label_name
      LDX   #>w_label_name
      JSR   w_text
      long_bcs @bad
@write_name:
      LDY   #NOBJ_SYM_NAME_LEN
      LDA   (w_member),Y
      STA   w_name_len
      CLC
      LDA   w_member
      ADC   #NOBJ_SYM_HEADER_SIZE
      STA   w_name
      LDA   w_member+1
      ADC   #0
      STA   w_name+1
      JSR   w_copy_name
      long_bcs @bad
      LDA   #$0A
      JSR   w_append
      long_bcs @bad
@skip_export:
      JSR   w_next_symbol
      LDA   w_symbol_count
      BNE   :+
      DEC   w_symbol_count+1
:     DEC   w_symbol_count
      JMP   @export
@next_export_object:
      INC   w_object
      JMP   @export_object
@ok:
      CLC
      RTS
@bad:
      SEC
      RTS

w_select_object:
      CMP   NLW_OBJECT_COUNT
      BCS   @bad
      TAX
      LDA   NLW_OBJECT_PTR_L,X
      STA   w_obj
      LDA   NLW_OBJECT_PTR_H,X
      STA   w_obj+1
      TXA
      ASL
      ASL
      ASL
      STA   w_section_base
      CLC
      RTS
@bad:
      SEC
      RTS

w_next_section:
      LDY   #NOBJ_SEC_NAME_LEN
      LDA   (w_member),Y
      CLC
      ADC   #NOBJ_SEC_HEADER_SIZE
      STA   w_tmp
      LDA   #0
      ADC   #0
      STA   w_tmp+1
      LDY   #NOBJ_SEC_DATA_LEN
      LDA   (w_member),Y
      CLC
      ADC   w_tmp
      STA   w_tmp
      INY
      LDA   (w_member),Y
      ADC   w_tmp+1
      STA   w_tmp+1
      BRA   w_advance_member

w_next_symbol:
      LDY   #NOBJ_SYM_NAME_LEN
      LDA   (w_member),Y
      CLC
      ADC   #NOBJ_SYM_HEADER_SIZE
      STA   w_tmp
      LDA   #0
      ADC   #0
      STA   w_tmp+1
w_advance_member:
      CLC
      LDA   w_member
      ADC   w_tmp
      STA   w_member
      LDA   w_member+1
      ADC   w_tmp+1
      STA   w_member+1
      RTS

w_text:
      STA   w_name
      STX   w_name+1
@byte:
      LDY   #0
      LDA   (w_name),Y
      BEQ   @done
      JSR   w_append
      BCS   @done
      INC   w_name
      BNE   @byte
      INC   w_name+1
      BRA   @byte
@done:
      RTS

w_copy_name:
      LDA   w_name_len
      BEQ   @done
      LDY   #0
      LDA   (w_name),Y
      JSR   w_append
      BCS   @done
      INC   w_name
      BNE   :+
      INC   w_name+1
:     DEC   w_name_len
      BRA   w_copy_name
@done:
      RTS

w_hex_word:
      LDA   w_address+1
      JSR   w_hex_byte
      BCS   w_hex_done
      LDA   w_address
      JMP   w_hex_byte
w_hex_done:
      RTS

w_hex_byte:
      PHA
      LSR
      LSR
      LSR
      LSR
      JSR   w_nibble
      BCS   @discard
      PLA
      AND   #$0F
      JMP   w_nibble
@discard:
      PLA
      RTS

w_nibble:
      TAX
      LDA   w_digits,X
      BRA   w_append

w_append:
      PHA
      LDA   w_left
      ORA   w_left+1
      BEQ   @full
      PLA
      LDY   #0
      STA   (w_dst),Y
      INC   w_dst
      BNE   :+
      INC   w_dst+1
:     LDA   w_left
      BNE   :+
      DEC   w_left+1
:     DEC   w_left
      INC   NLW_MAP_LEN
      BNE   :+
      INC   NLW_MAP_LEN+1
:     CLC
      RTS
@full:
      PLA
      SEC
      RTS

      .segment "RODATA"
w_title:    .byte "Nova Linker Map v1", $0A, "Load $", 0
w_size:     .byte $0A, "Size $", 0
w_sections: .byte $0A, "Sections", $0A, 0
w_exports:  .byte "Exports", $0A, 0
w_label_prefix:.byte "al 00", 0
w_label_name:.byte " .", 0
w_digits:   .byte "0123456789ABCDEF"
w_section_bits:.byte $01, $02, $04, $08, $10, $20, $40, $80

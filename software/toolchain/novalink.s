; Nova-native NOBJ/NLIB linker. Produces a load-address-prefixed Nova binary.
;
; The linker is language-neutral: it resolves ABS16 names against ordinary
; NLIB members and copies each referenced member once. Unreferenced members
; never enter the output image.

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

NLINK_IMPORT_CAP = 16

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
l_import_count: .res 1
l_address:      .res 2
l_tmp:          .res 2

      .segment "BSS"
      .export nlink_object_ptr
      .export nlink_object_len
      .export nlink_library_ptr
      .export nlink_library_len
      .export nlink_output_ptr
      .export nlink_output_cap
      .export nlink_load_base
      .export nlink_output_len
      .export nlink_entry
      .export nlink_error
nlink_object_ptr:  .res 2
nlink_object_len:  .res 2
nlink_library_ptr: .res 2
nlink_library_len: .res 2
nlink_output_ptr:  .res 2
nlink_output_cap:  .res 2
nlink_load_base:   .res 2
nlink_output_len:  .res 2
nlink_entry:       .res 2
nlink_error:       .res 1
l_import_member_l: .res NLINK_IMPORT_CAP
l_import_member_h: .res NLINK_IMPORT_CAP
l_import_address_l:.res NLINK_IMPORT_CAP
l_import_address_h:.res NLINK_IMPORT_CAP

      .segment "CODE"
      .export nlink_link

; Return A=0 on success, A=1 on error.
nlink_link:
      STZ   nlink_error
      STZ   nlink_output_len
      STZ   nlink_output_len+1
      STZ   l_import_count
      LDA   nlink_object_ptr
      STA   l_obj
      LDA   nlink_object_ptr+1
      STA   l_obj+1
      JSR   l_validate_object
      long_bcs l_bad_object
      JSR   l_validate_library
      long_bcs l_bad_relocation

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
:
      STZ   l_payload_len
      STZ   l_payload_len+1

      CLC
      LDA   l_obj
      ADC   #NOBJ_HEADER_SIZE
      STA   l_src
      LDA   l_obj+1
      ADC   #0
      STA   l_src+1
      LDY   #NOBJ_CODE_LEN
      LDA   (l_obj),Y
      STA   l_left
      INY
      LDA   (l_obj),Y
      STA   l_left+1
@copy_code:
      LDA   l_left
      ORA   l_left+1
      BEQ   @relocations
      LDY   #0
      LDA   (l_src),Y
      JSR   l_append
      long_bcs l_bad_output
      INC   l_src
      BNE   :+
      INC   l_src+1
:     LDA   l_left
      BNE   :+
      DEC   l_left+1
:     DEC   l_left
      BRA   @copy_code

@relocations:
      LDA   l_src
      STA   l_reloc
      LDA   l_src+1
      STA   l_reloc+1
      SEC
      LDA   nlink_object_len
      SBC   #NOBJ_HEADER_SIZE
      STA   l_reloc_bytes
      LDA   nlink_object_len+1
      SBC   #0
      STA   l_reloc_bytes+1
      long_bcc l_bad_object
      LDY   #NOBJ_CODE_LEN
      SEC
      LDA   l_reloc_bytes
      SBC   (l_obj),Y
      STA   l_reloc_bytes
      INY
      LDA   l_reloc_bytes+1
      SBC   (l_obj),Y
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
      BEQ   @linked
      JSR   l_take_reloc
      BCC   :+
      JMP   l_bad_relocation
:     STA   l_offset
      JSR   l_take_reloc
      long_bcs l_bad_relocation
      STA   l_offset+1
      JSR   l_take_reloc
      long_bcs l_bad_relocation
      CMP   #NOBJ_RELOC_ABS16
      long_bne l_bad_relocation
      JSR   l_take_reloc
      long_bcs l_bad_relocation
      STA   l_name_len
      BNE   :+
      JMP   l_bad_relocation
:
      LDA   l_reloc_bytes+1
      BNE   @name_fits
      LDA   l_reloc_bytes
      CMP   l_name_len
      long_bcc l_bad_relocation
@name_fits:
      LDA   l_reloc
      STA   l_name
      LDA   l_reloc+1
      STA   l_name+1
      JSR   l_skip_name
      JSR   l_find_member
      long_bcs l_bad_relocation
      JSR   l_import_member
      long_bcs l_bad_output
      JSR   l_patch_abs16
      long_bcs l_bad_relocation
      LDA   l_reloc_count
      BNE   :+
      DEC   l_reloc_count+1
:     DEC   l_reloc_count
      BRA   @next_reloc

@linked:
      LDA   l_reloc_bytes
      ORA   l_reloc_bytes+1
      long_bne l_bad_relocation
      CLC
      LDA   l_payload_len
      ADC   #2
      STA   nlink_output_len
      LDA   l_payload_len+1
      ADC   #0
      STA   nlink_output_len+1
      LDY   #NOBJ_ENTRY
      LDA   (l_obj),Y
      CLC
      ADC   nlink_load_base
      STA   nlink_entry
      INY
      LDA   (l_obj),Y
      ADC   nlink_load_base+1
      STA   nlink_entry+1
      LDA   #NLINK_OK
      RTS

l_validate_object:
      LDA   nlink_object_len+1
      BNE   @magic
      LDA   nlink_object_len
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
      CLC
      RTS
@bad:
      SEC
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
      CLC
      RTS
@bad:
      SEC
      RTS

; Find l_name in the library, returning member identity/code/length.
l_find_member:
      CLC
      LDA   l_library
      ADC   #NLIB_HEADER_SIZE
      STA   l_member
      LDA   l_library+1
      ADC   #0
      STA   l_member+1
      LDX   l_member_count
@member:
      CPX   #0
      BEQ   @missing
      LDY   #0
      LDA   (l_member),Y
      STA   l_tmp
      INC   l_member
      BNE   :+
      INC   l_member+1
:     LDA   l_member
      STA   l_member_name
      LDA   l_member+1
      STA   l_member_name+1
      CLC
      LDA   l_member
      ADC   l_tmp
      STA   l_member_code
      LDA   l_member+1
      ADC   #0
      STA   l_member_code+1
      LDY   #0
      LDA   (l_member_code),Y
      STA   l_member_len
      INC   l_member_code
      BNE   :+
      INC   l_member_code+1
:     LDY   #0
      LDA   (l_member_code),Y
      STA   l_member_len+1
      INC   l_member_code
      BNE   :+
      INC   l_member_code+1
:     LDA   l_tmp
      CMP   l_name_len
      BNE   @advance
      LDY   #0
@compare:
      CPY   l_name_len
      BCS   @found
      LDA   (l_member_name),Y
      CMP   (l_name),Y
      BNE   @advance
      INY
      BRA   @compare
@found:
      ; Identity is the member-name pointer, stable for import de-duplication.
      CLC
      RTS
@advance:
      CLC
      LDA   l_member_code
      ADC   l_member_len
      STA   l_member
      LDA   l_member_code+1
      ADC   l_member_len+1
      STA   l_member+1
      DEX
      BRA   @member
@missing:
      SEC
      RTS

; Reuse an imported member or append its code exactly once.
l_import_member:
      LDX   #0
@lookup:
      CPX   l_import_count
      BCS   @new
      LDA   l_import_member_l,X
      CMP   l_member_name
      BNE   @next
      LDA   l_import_member_h,X
      CMP   l_member_name+1
      BNE   @next
      LDA   l_import_address_l,X
      STA   l_address
      LDA   l_import_address_h,X
      STA   l_address+1
      CLC
      RTS
@next:
      INX
      BRA   @lookup
@new:
      CPX   #NLINK_IMPORT_CAP
      BCS   @full
      LDA   l_member_name
      STA   l_import_member_l,X
      LDA   l_member_name+1
      STA   l_import_member_h,X
      CLC
      LDA   nlink_load_base
      ADC   l_payload_len
      STA   l_address
      STA   l_import_address_l,X
      LDA   nlink_load_base+1
      ADC   l_payload_len+1
      STA   l_address+1
      STA   l_import_address_h,X
      INC   l_import_count
      LDA   l_member_code
      STA   l_src
      LDA   l_member_code+1
      STA   l_src+1
      LDA   l_member_len
      STA   l_left
      LDA   l_member_len+1
      STA   l_left+1
@copy:
      LDA   l_left
      ORA   l_left+1
      BEQ   @done
      LDY   #0
      LDA   (l_src),Y
      JSR   l_append
      BCS   @full
      INC   l_src
      BNE   :+
      INC   l_src+1
:     LDA   l_left
      BNE   :+
      DEC   l_left+1
:     DEC   l_left
      BRA   @copy
@done:
      CLC
      RTS
@full:
      SEC
      RTS

l_patch_abs16:
      ; The two-byte operand must be wholly inside the original code section.
      LDY   #NOBJ_CODE_LEN+1
      LDA   l_offset+1
      CMP   (l_obj),Y
      BCC   @in_range
      BNE   @bad
      DEY
      LDA   l_offset
      CLC
      ADC   #1
      CMP   (l_obj),Y
      BCS   @bad
@in_range:
      CLC
      LDA   nlink_output_ptr
      ADC   #2
      ADC   l_offset
      STA   l_tmp
      LDA   nlink_output_ptr+1
      ADC   #0
      ADC   l_offset+1
      STA   l_tmp+1
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

l_skip_name:
      CLC
      LDA   l_reloc
      ADC   l_name_len
      STA   l_reloc
      LDA   l_reloc+1
      ADC   #0
      STA   l_reloc+1
      SEC
      LDA   l_reloc_bytes
      SBC   l_name_len
      STA   l_reloc_bytes
      LDA   l_reloc_bytes+1
      SBC   #0
      STA   l_reloc_bytes+1
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
l_fail:
      STA   nlink_error
      LDA   #1
      RTS

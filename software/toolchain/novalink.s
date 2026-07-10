; Nova-native single-object linker. Produces a Nova load-address-prefixed binary.

      .setcpu "w65c02"
      .include "novalink.inc"

      .segment "ZEROPAGE"
l_obj:       .res 2
l_src:       .res 2
l_dst:       .res 2
l_left:      .res 2
l_cap:       .res 2

      .segment "BSS"
      .export nlink_object_ptr
      .export nlink_output_ptr
      .export nlink_output_cap
      .export nlink_load_base
      .export nlink_output_len
      .export nlink_entry
      .export nlink_error
nlink_object_ptr: .res 2
nlink_output_ptr: .res 2
nlink_output_cap: .res 2
nlink_load_base:  .res 2
nlink_output_len: .res 2
nlink_entry:      .res 2
nlink_error:      .res 1

      .segment "CODE"
      .export nlink_link

; Return A=0 on success, A=1 on error.
nlink_link:
      STZ   nlink_error
      STZ   nlink_output_len
      STZ   nlink_output_len+1
      LDA   nlink_object_ptr
      STA   l_obj
      LDA   nlink_object_ptr+1
      STA   l_obj+1

      LDY   #NOBJ_MAGIC
      LDA   (l_obj),Y
      CMP   #NOBJ_MAGIC0
      BEQ   :+
      JMP   l_bad_object
:
      INY
      LDA   (l_obj),Y
      CMP   #NOBJ_MAGIC1
      BEQ   :+
      JMP   l_bad_object
:
      INY
      LDA   (l_obj),Y
      CMP   #NOBJ_MAGIC2
      BEQ   :+
      JMP   l_bad_object
:
      INY
      LDA   (l_obj),Y
      CMP   #NOBJ_MAGIC3
      BEQ   :+
      JMP   l_bad_object
:
      LDY   #NOBJ_VERSION_OFF
      LDA   (l_obj),Y
      CMP   #NOBJ_VERSION
      BEQ   :+
      JMP   l_bad_object
:

      ; ponytail: v1's first linker accepts a relocation-free object. Add ABS16
      ; and external symbol records when the assembler IDE introduces labels.
      LDY   #NOBJ_RELOC_COUNT
      LDA   (l_obj),Y
      INY
      ORA   (l_obj),Y
      BEQ   :+
      JMP   l_bad_relocation
:

      LDY   #NOBJ_CODE_LEN
      LDA   (l_obj),Y
      STA   l_left
      INY
      LDA   (l_obj),Y
      STA   l_left+1

      LDA   nlink_output_cap
      STA   l_cap
      LDA   nlink_output_cap+1
      STA   l_cap+1
      ; Require code length + two-byte Nova load header.
      SEC
      LDA   l_cap
      SBC   #2
      STA   l_cap
      LDA   l_cap+1
      SBC   #0
      STA   l_cap+1
      BCS   :+
      JMP   l_bad_output
:
      LDA   l_cap+1
      CMP   l_left+1
      BCS   :+
      JMP   l_bad_output
:
      BNE   @fits
      LDA   l_cap
      CMP   l_left
      BCS   @fits
      JMP   l_bad_output
@fits:
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
      CLC
      LDA   l_obj
      ADC   #NOBJ_HEADER_SIZE
      STA   l_src
      LDA   l_obj+1
      ADC   #0
      STA   l_src+1

@copy:
      LDA   l_left
      ORA   l_left+1
      BEQ   @copied
      LDY   #0
      LDA   (l_src),Y
      STA   (l_dst),Y
      INC   l_src
      BNE   :+
      INC   l_src+1
:     INC   l_dst
      BNE   :+
      INC   l_dst+1
:     LDA   l_left
      BNE   :+
      DEC   l_left+1
:     DEC   l_left
      BRA   @copy

@copied:
      LDY   #NOBJ_CODE_LEN
      LDA   (l_obj),Y
      CLC
      ADC   #2
      STA   nlink_output_len
      INY
      LDA   (l_obj),Y
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

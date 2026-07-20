; Nova-native 65C02 assembler, initial vertical-slice instruction set.
;
; Accepted source is case-insensitive and line-oriented:
;   LDA #$nn
;   STA $nnnn
;   JSR symbol
;   RTS
; Blank lines and ';' comments are ignored.
;
; This first slice supports external ABS16 relocations. NAS has no knowledge of
; Pascal, the NDK, or any library implementation.

      .setcpu "w65c02"
      .include "novaasm.inc"

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

      .segment "ZEROPAGE"
a_src:       .res 2
a_left:      .res 2
a_dst:       .res 2
a_cap:       .res 2
a_header:    .res 2
a_code_len:  .res 2
a_reloc_ptr: .res 2
a_reloc_len: .res 2
a_reloc_count:.res 2
a_symbol_len:.res 1
a_tmp0:      .res 1
a_tmp1:      .res 1

      .segment "BSS"
NASM_SYMBOL_CAP = 32
NASM_RELOC_CAP  = 768
a_symbol:     .res NASM_SYMBOL_CAP
a_reloc_buf:  .res NASM_RELOC_CAP
      .export nasm_source_ptr
      .export nasm_source_len
      .export nasm_object_ptr
      .export nasm_object_cap
      .export nasm_object_len
      .export nasm_error
nasm_source_ptr: .res 2
nasm_source_len: .res 2
nasm_object_ptr: .res 2
nasm_object_cap: .res 2
nasm_object_len: .res 2
nasm_error:      .res 1

      .segment "CODE"
      .export nasm_assemble

; Return A=0 on success, A=1 on error; nasm_error carries the reason.
nasm_assemble:
      STZ   nasm_error
      STZ   nasm_object_len
      STZ   nasm_object_len+1
      STZ   a_reloc_len
      STZ   a_reloc_len+1
      STZ   a_reloc_count
      STZ   a_reloc_count+1
      LDA   #<a_reloc_buf
      STA   a_reloc_ptr
      LDA   #>a_reloc_buf
      STA   a_reloc_ptr+1

      LDA   nasm_source_ptr
      STA   a_src
      LDA   nasm_source_ptr+1
      STA   a_src+1
      LDA   nasm_source_len
      STA   a_left
      LDA   nasm_source_len+1
      STA   a_left+1
      LDA   nasm_object_ptr
      STA   a_header
      STA   a_dst
      LDA   nasm_object_ptr+1
      STA   a_header+1
      STA   a_dst+1
      LDA   nasm_object_cap
      STA   a_cap
      LDA   nasm_object_cap+1
      STA   a_cap+1

      LDA   a_cap+1
      BNE   @header_fits
      LDA   a_cap
      CMP   #NOBJ_HEADER_SIZE
      BCS   @header_fits
      JMP   a_fail_output
@header_fits:
      LDY   #NOBJ_MAGIC
      LDA   #NOBJ_MAGIC0
      STA   (a_header),Y
      INY
      LDA   #NOBJ_MAGIC1
      STA   (a_header),Y
      INY
      LDA   #NOBJ_MAGIC2
      STA   (a_header),Y
      INY
      LDA   #NOBJ_MAGIC3
      STA   (a_header),Y
      LDY   #NOBJ_VERSION_OFF
      LDA   #NOBJ_VERSION
      STA   (a_header),Y
      INY
      LDA   #0
      STA   (a_header),Y
      LDY   #NOBJ_CODE_LEN
      STA   (a_header),Y
      INY
      STA   (a_header),Y
      INY
      STA   (a_header),Y          ; entry offset = 0
      INY
      STA   (a_header),Y
      INY
      STA   (a_header),Y          ; relocation count = 0
      INY
      STA   (a_header),Y

      CLC
      LDA   a_dst
      ADC   #NOBJ_HEADER_SIZE
      STA   a_dst
      BCC   :+
      INC   a_dst+1
:     SEC
      LDA   a_cap
      SBC   #NOBJ_HEADER_SIZE
      STA   a_cap
      LDA   a_cap+1
      SBC   #0
      STA   a_cap+1
      STZ   a_code_len
      STZ   a_code_len+1

@statement:
      JSR   a_skip_layout
      BCS   :+
      JMP   @done
:
      JSR   a_read_upper
      BCS   :+
      JMP   a_fail_syntax
:
      CMP   #'L'
      BEQ   @lda
      CMP   #'S'
      BEQ   @sta
      CMP   #'J'
      long_beq @jsr
      CMP   #'R'
      long_beq @rts
      JMP   a_fail_syntax

@lda:
      JSR   a_read_upper
      BCS   :+
      JMP   a_fail_syntax
:
      CMP   #'D'
      BEQ   :+
      JMP   a_fail_syntax
:
      JSR   a_read_upper
      BCS   :+
      JMP   a_fail_syntax
:
      CMP   #'A'
      BEQ   :+
      JMP   a_fail_syntax
:
      JSR   a_skip_hspace
      JSR   a_next
      BCS   :+
      JMP   a_fail_syntax
:
      CMP   #'#'
      BEQ   :+
      JMP   a_fail_syntax
:
      JSR   a_parse_hex_byte
      long_bcs a_fail_syntax
      STA   a_tmp0
      LDA   #$A9                  ; LDA #imm
      JSR   a_emit
      long_bcs a_fail_output
      LDA   a_tmp0
      JSR   a_emit
      long_bcs a_fail_output
      JMP   @statement

@sta:
      JSR   a_read_upper
      long_bcc a_fail_syntax
      CMP   #'T'
      long_bne a_fail_syntax
      JSR   a_read_upper
      long_bcc a_fail_syntax
      CMP   #'A'
      long_bne a_fail_syntax
      JSR   a_skip_hspace
      JSR   a_parse_hex_word
      long_bcs a_fail_syntax
      LDA   #$8D                  ; STA abs
      JSR   a_emit
      long_bcs a_fail_output
      LDA   a_tmp0                ; little-endian operand
      JSR   a_emit
      long_bcs a_fail_output
      LDA   a_tmp1
      JSR   a_emit
      long_bcs a_fail_output
      JMP   @statement

@jsr:
      JSR   a_read_upper
      long_bcc a_fail_syntax
      CMP   #'S'
      long_bne a_fail_syntax
      JSR   a_read_upper
      long_bcc a_fail_syntax
      CMP   #'R'
      long_bne a_fail_syntax
      JSR   a_skip_hspace
      JSR   a_parse_identifier
      long_bcs a_fail_syntax
      JSR   a_add_relocation
      long_bcs a_fail_output
      LDA   #$20                  ; JSR abs
      JSR   a_emit
      long_bcs a_fail_output
      LDA   #0
      JSR   a_emit
      long_bcs a_fail_output
      LDA   #0
      JSR   a_emit
      long_bcs a_fail_output
      JMP   @statement

@rts:
      JSR   a_read_upper
      long_bcc a_fail_syntax
      CMP   #'T'
      long_bne a_fail_syntax
      JSR   a_read_upper
      long_bcc a_fail_syntax
      CMP   #'S'
      long_bne a_fail_syntax
      LDA   #$60
      JSR   a_emit
      long_bcs a_fail_output
      JMP   @statement

@done:
      LDY   #NOBJ_CODE_LEN
      LDA   a_code_len
      STA   (a_header),Y
      INY
      LDA   a_code_len+1
      STA   (a_header),Y
      LDY   #NOBJ_RELOC_COUNT
      LDA   a_reloc_count
      STA   (a_header),Y
      INY
      LDA   a_reloc_count+1
      STA   (a_header),Y

      LDA   #<a_reloc_buf
      STA   a_reloc_ptr
      LDA   #>a_reloc_buf
      STA   a_reloc_ptr+1
@copy_reloc:
      LDA   a_reloc_len
      ORA   a_reloc_len+1
      BEQ   @length
      LDY   #0
      LDA   (a_reloc_ptr),Y
      JSR   a_emit_tail
      long_bcs a_fail_output
      INC   a_reloc_ptr
      BNE   :+
      INC   a_reloc_ptr+1
:     LDA   a_reloc_len
      BNE   :+
      DEC   a_reloc_len+1
:     DEC   a_reloc_len
      BRA   @copy_reloc
@length:
      SEC
      LDA   a_dst
      SBC   a_header
      STA   nasm_object_len
      LDA   a_dst+1
      SBC   a_header+1
      STA   nasm_object_len+1
      LDA   #NASM_OK
      RTS

a_fail_syntax:
      LDA   #NASM_ERR_SYNTAX
      BRA   a_fail
a_fail_output:
      LDA   #NASM_ERR_OUTPUT
a_fail:
      STA   nasm_error
      LDA   #1
      RTS

; Skip whitespace and comments. Carry set means a statement remains.
a_skip_layout:
@again:
      JSR   a_peek
      BCC   @eof
      CMP   #' '
      BEQ   @eat
      CMP   #$09
      BEQ   @eat
      CMP   #$0D
      BEQ   @eat
      CMP   #$0A
      BEQ   @eat
      CMP   #';'
      BEQ   @comment
      SEC
      RTS
@eat:
      JSR   a_next
      BRA   @again
@comment:
      JSR   a_next
@comment_loop:
      JSR   a_next
      BCC   @eof
      CMP   #$0A
      BNE   @comment_loop
      BRA   @again
@eof:
      CLC
      RTS

a_skip_hspace:
@loop:
      JSR   a_peek
      BCC   @done
      CMP   #' '
      BEQ   @eat
      CMP   #$09
      BNE   @done
@eat:
      JSR   a_next
      BRA   @loop
@done:
      RTS

a_parse_identifier:
      STZ   a_symbol_len
      JSR   a_peek_upper
      BCC   @bad
      JSR   a_is_ident_start
      BCC   @bad
@loop:
      LDA   a_symbol_len
      CMP   #NASM_SYMBOL_CAP
      BCS   @bad
      JSR   a_read_upper
      LDX   a_symbol_len
      STA   a_symbol,X
      INC   a_symbol_len
      JSR   a_peek_upper
      BCC   @ok
      JSR   a_is_ident
      BCS   @loop
@ok:
      CLC
      RTS
@bad:
      SEC
      RTS

a_peek_upper:
      JSR   a_peek
      BCC   @eof
      CMP   #'a'
      BCC   @ok
      CMP   #'z'+1
      BCS   @ok
      AND   #$DF
@ok:
      SEC
      RTS
@eof:
      CLC
      RTS

a_is_ident_start:
      CMP   #'A'
      BCC   @underscore
      CMP   #'Z'+1
      BCC   @yes
@underscore:
      CMP   #'_'
      BEQ   @yes
      CLC
      RTS
@yes:
      SEC
      RTS

a_is_ident:
      JSR   a_is_ident_start
      BCS   @yes
      CMP   #'0'
      BCC   @no
      CMP   #'9'+1
      BCC   @yes
@no:
      CLC
      RTS
@yes:
      SEC
      RTS

; Record an ABS16 external at the JSR operand which is about to be emitted.
a_add_relocation:
      CLC
      LDA   a_code_len
      ADC   #1
      JSR   a_reloc_emit
      BCS   @bad
      LDA   a_code_len+1
      ADC   #0
      JSR   a_reloc_emit
      BCS   @bad
      LDA   #NOBJ_RELOC_ABS16
      JSR   a_reloc_emit
      BCS   @bad
      LDA   a_symbol_len
      JSR   a_reloc_emit
      BCS   @bad
      LDX   #0
@name:
      CPX   a_symbol_len
      BCS   @done
      LDA   a_symbol,X
      JSR   a_reloc_emit
      BCS   @bad
      INX
      BRA   @name
@done:
      INC   a_reloc_count
      BNE   :+
      INC   a_reloc_count+1
:     CLC
      RTS
@bad:
      SEC
      RTS

a_reloc_emit:
      PHA
      LDA   a_reloc_len+1
      CMP   #>NASM_RELOC_CAP
      BCC   @room
      PLA
      SEC
      RTS
@room:
      PLA
      LDY   #0
      STA   (a_reloc_ptr),Y
      INC   a_reloc_ptr
      BNE   :+
      INC   a_reloc_ptr+1
:     INC   a_reloc_len
      BNE   :+
      INC   a_reloc_len+1
:     CLC
      RTS

; Parse $nn, returning the byte in A with carry clear.
a_parse_hex_byte:
      JSR   a_next
      BCC   @bad
      CMP   #'$'
      BNE   @bad
      JSR   a_hex_nibble
      BCS   @bad
      ASL
      ASL
      ASL
      ASL
      STA   a_tmp0
      JSR   a_hex_nibble
      BCS   @bad
      ORA   a_tmp0
      CLC
      RTS
@bad:
      SEC
      RTS

; Parse $hhhh into a_tmp1:a_tmp0, returning carry clear.
a_parse_hex_word:
      JSR   a_next
      BCC   @bad
      CMP   #'$'
      BNE   @bad
      JSR   a_hex_pair
      BCS   @bad
      STA   a_tmp1
      JSR   a_hex_pair
      BCS   @bad
      STA   a_tmp0
      CLC
      RTS
@bad:
      SEC
      RTS

a_hex_pair:
      JSR   a_hex_nibble
      BCS   @bad
      ASL
      ASL
      ASL
      ASL
      STA   a_tmp0
      JSR   a_hex_nibble
      BCS   @bad
      ORA   a_tmp0
      CLC
      RTS
@bad:
      SEC
      RTS

a_hex_nibble:
      JSR   a_read_upper
      BCC   @bad
      CMP   #'0'
      BCC   @bad
      CMP   #'9'+1
      BCC   @digit
      CMP   #'A'
      BCC   @bad
      CMP   #'F'+1
      BCS   @bad
      SEC
      SBC   #'A'-10
      CLC
      RTS
@digit:
      SEC
      SBC   #'0'
      CLC
      RTS
@bad:
      SEC
      RTS

a_read_upper:
      JSR   a_next
      BCC   @eof
      CMP   #'a'
      BCC   @ok
      CMP   #'z'+1
      BCS   @ok
      AND   #$DF
@ok:
      SEC
      RTS
@eof:
      CLC
      RTS

a_peek:
      LDA   a_left
      ORA   a_left+1
      BEQ   @eof
      LDY   #0
      LDA   (a_src),Y
      SEC
      RTS
@eof:
      CLC
      RTS

a_next:
      JSR   a_peek
      BCC   @eof
      PHA
      INC   a_src
      BNE   :+
      INC   a_src+1
:     LDA   a_left
      BNE   :+
      DEC   a_left+1
:     DEC   a_left
      PLA
      SEC
@eof:
      RTS

; Append A to the object code section. Carry set means output full.
a_emit:
      PHA
      LDA   a_cap
      ORA   a_cap+1
      BEQ   @full
      PLA
      LDY   #0
      STA   (a_dst),Y
      INC   a_dst
      BNE   :+
      INC   a_dst+1
:     LDA   a_cap
      BNE   :+
      DEC   a_cap+1
:     DEC   a_cap
      INC   a_code_len
      BNE   :+
      INC   a_code_len+1
:     CLC
      RTS
@full:
      PLA
      SEC
      RTS

; Append relocation bytes without extending the code-section length.
a_emit_tail:
      PHA
      LDA   a_cap
      ORA   a_cap+1
      BEQ   @full
      PLA
      LDY   #0
      STA   (a_dst),Y
      INC   a_dst
      BNE   :+
      INC   a_dst+1
:     LDA   a_cap
      BNE   :+
      DEC   a_cap+1
:     DEC   a_cap
      CLC
      RTS
@full:
      PLA
      SEC
      RTS

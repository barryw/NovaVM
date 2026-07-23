; Pascal runtime archive generated from the Nova NDK hardware definitions.
; NL treats this as an ordinary NLIB and has no knowledge of these symbols.

      .setcpu "w65c02"
      .include "nova.inc"
      .include "libsystem.inc"
      .include "libfiles.inc"
      .include "libmemory.inc"
      .include "liblangrt.inc"
      .include "nobj.inc"
      .include "pascal.inc"

.macro PASCAL_FILE_CALL function_id
      LDA   #MODULE_ID_FILES
      STA   LIB_MOD_ID
      LDA   #function_id
      STA   LIB_FN_ID
      JSR   LIB_LOADER_BAND
.endmacro

.macro PASCAL_SYSTEM_CALL function_id
      LDA   #MODULE_ID_SYSTEM
      STA   LIB_MOD_ID
      LDA   #function_id
      STA   LIB_FN_ID
      JSR   LIB_LOADER_BAND
.endmacro

.macro PASCAL_MEMORY_CALL function_id
      LDA   #MODULE_ID_MEMORY
      STA   LIB_MOD_ID
      LDA   #function_id
      STA   LIB_FN_ID
      JSR   LIB_LOADER_BAND
.endmacro

; LIB_ARG3 is ordinary RAM, so it cannot be used with the 65C02's
; zero-page-indirect addressing modes. Reload this scratch pointer after
; every NDK call because the NVR registers are caller-clobbered by design.
.macro PASCAL_LOAD_FILE_DATA_PTR
      LDA   LIB_ARG3
      STA   NVR6L
      LDA   LIB_ARG3+1
      STA   NVR6H
.endmacro

      .segment "LIBRARY"
      .byte NLIB_MAGIC0, NLIB_MAGIC1, NLIB_MAGIC2, NLIB_MAGIC3
      .byte NLIB_VERSION, 20

      ; P_WRITE_CHAR deliberately imports P_CHAR_DEVICE from the next member.
      ; This keeps the Pascal-facing routine independent of the hardware shim
      ; and exercises NL's recursive archive extraction.
      .word write_object_end-write_object
write_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 1, $FF
      .word 0
      .word 2
      .word write_symbols-write_object
      .word 1
      .word write_relocations-write_object
      .word 0

      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_EXEC, 0, 4, 0
      .word write_code_end-write_code
      .word write_code_end-write_code
      .byte "CODE"
write_code:
      .byte $20, $00, $00                  ; JSR P_CHAR_DEVICE
      .byte $60                            ; RTS
write_code_end:

write_symbols:
      .word 0
      .byte 0, NOBJ_SYM_GLOBAL, 12, "P_WRITE_CHAR"
      .word 0
      .byte NOBJ_SYM_UNDEFINED, NOBJ_SYM_GLOBAL, 13, "P_CHAR_DEVICE"

write_relocations:
      .byte 0, NOBJ_RELOC_ABS16
      .word 1, 1, 0
write_object_end:

      ; I_ marks Nova's inline-parameter ABI. The zero-terminated bytes after
      ; the JSR are printed, then the saved return address is advanced past
      ; them. This keeps literal output dense and leaves no pointer setup in
      ; the caller.
      .word line_object_end-line_object
line_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 1, $FF
      .word 0
      .word 2
      .word line_symbols-line_object
      .word 2
      .word line_relocations-line_object
      .word 0

      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_EXEC, 0, 4, 0
      .word line_code_end-line_code
      .word line_code_end-line_code
      .byte "CODE"
line_code:
      .byte $BA                            ; TSX
      .byte $FE,$01,$01,$D0,$03            ; advance saved return address
      .byte $FE,$02,$01
      .byte $BD,$01,$01,$8D,$00,$00        ; patch inline-byte load address
      .byte $BD,$02,$01,$8D,$00,$00
      .byte $AD,$FF,$FF,$F0,$05            ; load until zero
      .byte $8D,<VGC_CHAROUT,>VGC_CHAROUT   ; NDK character device / loop
      .byte $80,$E2
      .byte $A9,$0A                         ; LF
      .byte $8D,<VGC_CHAROUT,>VGC_CHAROUT,$60
line_code_end:

line_symbols:
      .word 0
      .byte 0, NOBJ_SYM_GLOBAL, 14, "I_P_WRITE_LINE"
      .word $0015
      .byte 0, 0, 8, "PWL_LOAD"

line_relocations:
      .byte 0, NOBJ_RELOC_ABS16
      .word $000D, 1, 1
      .byte 0, NOBJ_RELOC_ABS16
      .word $0013, 1, 2
line_object_end:

      ; Byte-sized decimal output. Keeping this in its own archive member means
      ; ordinary string-only programs never pay for it.
      .word byte_object_end-byte_object
byte_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 1, $FF
      .word 0
      .word 5
      .word byte_symbols-byte_object
      .word 1
      .word byte_relocations-byte_object
      .word 0

      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_EXEC, 0, 4, 0
      .word byte_code_end-byte_code
      .word byte_code_end-byte_code
      .byte "CODE"
byte_code:
      .byte $20,$00,$00                     ; JSR P_WRITE_BYTE
      .byte $A9,$0A,$8D,<VGC_CHAROUT,>VGC_CHAROUT,$60
      .byte $A0,$00,$A2,$30,$C9,$64,$90,$06,$38,$E9,$64,$E8,$80,$F6,$E0,$30
      .byte $F0,$07,$48,$8A,$8D,<VGC_CHAROUT,>VGC_CHAROUT,$68,$C8,$A2,$30,$C9,$0A,$90,$06,$38
      .byte $E9,$0A,$E8,$80,$F6,$C0,$00,$D0,$04,$E0,$30,$F0,$06,$48,$8A,$8D
      .byte <VGC_CHAROUT,>VGC_CHAROUT,$68,$18,$69,$30,$8D,<VGC_CHAROUT,>VGC_CHAROUT,$60

write_word:
      STZ   NVR2L                         ; no minimum field width
      STA   NVR0L
      STX   NVR0H
      BRA   write_word_common

; Stack arguments below the return address are width, then value (u16 each).
write_word_field:
      TSX
      LDA   $0103,X
      STA   NVR2L
      LDA   $0105,X
      STA   NVR0L
      LDA   $0106,X
      STA   NVR0H
write_word_common:
      STZ   NVR2H                         ; sign character count
      LDA   NVR0H
      BPL   @magnitude
      INC   NVR2H
      SEC
      LDA   #0
      SBC   NVR0L
      STA   NVR0L
      LDA   #0
      SBC   NVR0H
      STA   NVR0H
@magnitude:
      STZ   NVR1L
@divide:
      LDA   #0
      LDX   #16
@bit:
      ASL   NVR0L
      ROL   NVR0H
      ROL
      CMP   #10
      BCC   :+
      SBC   #10
      INC   NVR0L
:     DEX
      BNE   @bit
      CLC
      ADC   #'0'
      PHA
      INC   NVR1L
      LDA   NVR0L
      ORA   NVR0H
      BNE   @divide
      CLC
      LDA   NVR1L
      ADC   NVR2H
      STA   NVR3L
      LDA   NVR2L
      CMP   NVR3L
      BCC   @sign
      BEQ   @sign
      SEC
      SBC   NVR3L
      TAX
      LDA   #' '
@padding:
      STA   VGC_CHAROUT
      DEX
      BNE   @padding
@sign:
      LDA   NVR2H
      BEQ   @digits
      LDA   #'-'
      STA   VGC_CHAROUT
@digits:
      PLA
      STA   VGC_CHAROUT
      DEC   NVR1L
      BNE   @digits
      RTS

write_string:
      STA   NVR0L
      STX   NVR0H
      LDY   #0
      LDA   (NVR0L),Y
      TAX
      BEQ   @done
@character:
      INY
      LDA   (NVR0L),Y
      STA   VGC_CHAROUT
      DEX
      BNE   @character
@done:
      RTS
byte_code_end:

byte_symbols:
      .word 0
      .byte 0, NOBJ_SYM_GLOBAL, 15, "P_WRITE_BYTE_LN"
      .word 9
      .byte 0, NOBJ_SYM_GLOBAL, 12, "P_WRITE_BYTE"
      .word write_word-byte_code
      .byte 0, NOBJ_SYM_GLOBAL, 12, "P_WRITE_WORD"
      .word write_string-byte_code
      .byte 0, NOBJ_SYM_GLOBAL, 14, "P_WRITE_STRING"
      .word write_word_field-byte_code
      .byte 0, NOBJ_SYM_GLOBAL, 18, "P_WRITE_WORD_FIELD"

byte_relocations:
      .byte 0, NOBJ_RELOC_ABS16
      .word 1, 1, 0
byte_object_end:

      ; Formatted Real output depends on the paged numeric runtime. Keeping it
      ; separate prevents byte-only programs from inheriting those objects.
      .word format_object_end-format_object
format_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 1, $FF
      .word 0
      .word 7
      .word format_symbols-format_object
      .word 13
      .word format_relocations-format_object
      .word 0

      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_EXEC, 0, 4, 0
      .word format_code_end-format_code
      .word format_code_end-format_code
      .byte "CODE"
format_code:
; A selects word field (0), Real field (1), or Real field+precision (2).
write_format:
      CMP   #1
      BCC   @word
      BEQ   write_real_field
      BRA   write_real_field_precision
@word:
      .byte $4C
write_format_word_jump: .word 0

; Reuse the canonical Real-to-string service. The existing caller arguments
; remain in place; these compact adapters copy them above their own return
; address so P_STR_REAL sees its normal Pascal stack ABI.
write_real_field:
      TSX
      LDA   $0106,X                       ; value high
      PHA
      LDA   $0105,X                       ; value low
      PHA
      PHA                                 ; unused width high
      LDA   $0103,X                       ; width low
      PHA
      .byte $A9
write_real_field_buffer_lo: .byte 0
      .byte $A2
write_real_field_buffer_hi: .byte 0
      .byte $20
write_real_field_call: .word 0
      PLA
      PLA
      PLA
      PLA
      BRA   write_real_buffer

write_real_field_precision:
      TSX
      LDA   $0108,X                       ; value high
      PHA
      LDA   $0107,X                       ; value low
      PHA
      PHA                                 ; unused width high
      LDA   $0105,X                       ; width low
      PHA
      PHA                                 ; unused precision high
      LDA   $0103,X                       ; precision low
      PHA
      .byte $A9
write_real_precision_buffer_lo: .byte 0
      .byte $A2
write_real_precision_buffer_hi: .byte 0
      .byte $20
write_real_precision_call: .word 0
      PLA
      PLA
      PLA
      PLA
      PLA
      PLA

write_real_buffer:
      .byte $A9
write_real_buffer_lo: .byte 0
      .byte $A2
write_real_buffer_hi: .byte 0
      .byte $4C
write_real_buffer_write: .word 0

write_real:
      PHX
      PHA
      LDA   #0
      PHA                                 ; unused width high
      PHA                                 ; zero field width
      .byte $A9
write_real_default_buffer_lo: .byte 0
      .byte $A2
write_real_default_buffer_hi: .byte 0
      .byte $20
write_real_default_call: .word 0
      PLA
      PLA
      PLA
      PLA
      BRA   write_real_buffer
format_code_end:

format_symbols:
      .word write_format-format_code
      .byte 0, NOBJ_SYM_GLOBAL, 14, "P_WRITE_FORMAT"
      .word write_real-format_code
      .byte 0, NOBJ_SYM_GLOBAL, 12, "P_WRITE_REAL"
      .word 0
      .byte NOBJ_SYM_UNDEFINED, NOBJ_SYM_GLOBAL, 18, "P_WRITE_WORD_FIELD"
      .word 0
      .byte NOBJ_SYM_UNDEFINED, NOBJ_SYM_GLOBAL, 14, "P_WRITE_STRING"
      .word 0
      .byte NOBJ_SYM_UNDEFINED, NOBJ_SYM_GLOBAL, 10, "P_STR_REAL"
      .word 0
      .byte NOBJ_SYM_UNDEFINED, NOBJ_SYM_GLOBAL, 20, "P_STR_REAL_PRECISION"
      .word 0
      .byte NOBJ_SYM_UNDEFINED, NOBJ_SYM_GLOBAL, 12, "__NP_STRBUF0"

format_relocations:
      .byte 0, NOBJ_RELOC_ABS16
      .word write_format_word_jump-format_code, 2, 0
      .byte 0, NOBJ_RELOC_LO8
      .word write_real_field_buffer_lo-format_code, 6, 0
      .byte 0, NOBJ_RELOC_HI8
      .word write_real_field_buffer_hi-format_code, 6, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word write_real_field_call-format_code, 4, 0
      .byte 0, NOBJ_RELOC_LO8
      .word write_real_precision_buffer_lo-format_code, 6, 0
      .byte 0, NOBJ_RELOC_HI8
      .word write_real_precision_buffer_hi-format_code, 6, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word write_real_precision_call-format_code, 5, 0
      .byte 0, NOBJ_RELOC_LO8
      .word write_real_buffer_lo-format_code, 6, 0
      .byte 0, NOBJ_RELOC_HI8
      .word write_real_buffer_hi-format_code, 6, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word write_real_buffer_write-format_code, 3, 0
      .byte 0, NOBJ_RELOC_LO8
      .word write_real_default_buffer_lo-format_code, 6, 0
      .byte 0, NOBJ_RELOC_HI8
      .word write_real_default_buffer_hi-format_code, 6, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word write_real_default_call-format_code, 4, 0
format_object_end:

      .word device_object_end-device_object
device_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 1, $FF
      .word 0
      .word 1
      .word device_symbols-device_object
      .word 0
      .word device_object_end-device_object
      .word 0

      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_EXEC, 0, 4, 0
      .word device_code_end-device_code
      .word device_code_end-device_code
      .byte "CODE"
device_code:
      .byte $8D, <VGC_CHAROUT, >VGC_CHAROUT ; STA VGC_CHAROUT
      .byte $60                             ; RTS
device_code_end:

device_symbols:
      .word 0
      .byte 0, NOBJ_SYM_GLOBAL, 13, "P_CHAR_DEVICE"
device_object_end:

      ; Deliberately unused. Its marker bytes let the emulator test prove that
      ; NL copied the referenced member rather than concatenating the archive.
      .word unused_object_end-unused_object
unused_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 1, $FF
      .word 0
      .word 1
      .word unused_symbols-unused_object
      .word 0
      .word unused_object_end-unused_object
      .word 0

      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_EXEC, 0, 4, 0
      .word unused_code_end-unused_code
      .word unused_code_end-unused_code
      .byte "CODE"
unused_code:
      .byte $A9, $DE, $A2, $AD, $A0, $BE, $60
unused_code_end:

unused_symbols:
      .word 0
      .byte 0, NOBJ_SYM_GLOBAL, 8, "P_UNUSED"
unused_object_end:

      ; Legacy byte-array helpers use A/X as a 16-bit index. Keep them in their
      ; own member so newer typed and general array operations do not pull them.
      .word array_object_end-array_object
array_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 1, $FF
      .word 0
      .word 3
      .word array_symbols-array_object
      .word 0
      .word array_object_end-array_object
      .word 0

      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_EXEC, 0, 4, 0
      .word array_code_end-array_code
      .word array_code_end-array_code
      .byte "CODE"
array_code:
array_get:
      STA   NVR1L
      STX   NVR1H
      STZ   NVR4L
      BRA   array_inline

array_set_byte:
      STA   NVR0L
      TSX
      LDA   $0103,X
      STA   NVR1L
      STZ   NVR1H
      LDA   $0102,X
      STA   $0103,X
      LDA   $0101,X
      STA   $0102,X
      INX
      TXS
      LDA   #1
      STA   NVR4L
      BRA   array_inline

array_set_word:
      STA   NVR0L
      TSX
      LDA   $0103,X
      STA   NVR1L
      LDA   $0104,X
      STA   NVR1H
      LDA   $0102,X
      STA   $0104,X
      LDA   $0101,X
      STA   $0103,X
      INX
      INX
      TXS
      LDA   #1
      STA   NVR4L

array_inline:
      TSX
      CLC
      LDA   $0101,X
      ADC   #3
      STA   NVR2L
      LDA   $0102,X
      ADC   #0
      STA   NVR2H
      LDY   #0
      LDA   (NVR2L),Y
      STA   NVR3L
      INY
      LDA   (NVR2L),Y
      STA   NVR3H
      CLC
      LDA   $0101,X
      ADC   #2
      STA   $0101,X
      LDA   $0102,X
      ADC   #0
      STA   $0102,X
      CLC
      LDA   NVR1L
      ADC   NVR3L
      STA   NVR1L
      LDA   NVR1H
      ADC   NVR3H
      STA   NVR1H
      LDA   NVR4L
      BNE   array_store
      LDA   (NVR1L)
      RTS
array_store:
      LDA   NVR0L
      STA   (NVR1L)
      RTS

array_code_end:

array_symbols:
      .word array_get-array_code
      .byte 0, NOBJ_SYM_GLOBAL, 8, "I_P_AGET"
      .word array_set_byte-array_code
      .byte 0, NOBJ_SYM_GLOBAL, 9, "I_P_ASETB"
      .word array_set_word-array_code
      .byte 0, NOBJ_SYM_GLOBAL, 9, "I_P_ASETW"
array_object_end:

      ; Fully general lower-bound and stride addressing is independent of the
      ; common zero-based byte-offset fast path.
      .word index_object_end-index_object
index_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 1, $FF
      .word 0
      .word 2
      .word index_symbols-index_object
      .word 0
      .word index_object_end-index_object
      .word 0

      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_EXEC, 0, 4, 0
      .word index_code_end-index_code
      .word index_code_end-index_code
      .byte "CODE"
index_code:
      .include "array_index_core.body"
index_code_end:

index_symbols:
      .word i_array_index_addr-index_code
      .byte 0, NOBJ_SYM_GLOBAL, 14, "I_P_INDEX_ADDR"
      .word i_array_index_addr_stack-index_code
      .byte 0, NOBJ_SYM_GLOBAL, 24, "I_ARRAY_INDEX_ADDR_STACK"
index_object_end:

      ; Zero-based byte arrays are common enough to deserve a dense NDK entry.
      .word byte_index_object_end-byte_index_object
byte_index_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 1, $FF
      .word 0
      .word 1
      .word byte_index_symbols-byte_index_object
      .word 0
      .word byte_index_object_end-byte_index_object
      .word 0

      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_EXEC, 0, 4, 0
      .word byte_index_code_end-byte_index_code
      .word byte_index_code_end-byte_index_code
      .byte "CODE"
byte_index_code:
      .include "array_index_byte_stack.body"
byte_index_code_end:

byte_index_symbols:
      .word i_array_index_byte_stack-byte_index_code
      .byte 0, NOBJ_SYM_GLOBAL, 24, "I_ARRAY_INDEX_BYTE_STACK"
byte_index_object_end:

      ; Aggregate copying is unrelated to indexing and must remain removable.
      .word block_object_end-block_object
block_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 1, $FF
      .word 0
      .word 1
      .word block_symbols-block_object
      .word 0
      .word block_object_end-block_object
      .word 0

      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_EXEC, 0, 4, 0
      .word block_code_end-block_code
      .word block_code_end-block_code
      .byte "CODE"
block_code:
block_copy:
      STA   NVR2L
      STX   NVR2H
      TSX
      CLC
      LDA   $0101,X
      ADC   #1
      STA   NVR4L
      LDA   $0102,X
      ADC   #0
      STA   NVR4H
      LDY   #0
      LDA   (NVR4L),Y
      STA   NVR5L
      INY
      LDA   (NVR4L),Y
      STA   NVR5H
      CLC
      LDA   $0101,X
      ADC   #2
      STA   $0101,X
      LDA   $0102,X
      ADC   #0
      STA   $0102,X
@copy:
      LDA   NVR5L
      ORA   NVR5H
      BEQ   @copied
      LDY   #0
      LDA   NVR2L
      BNE   :+
      LDA   #0
      BRA   :++
:     LDA   (NVR2L),Y
:     STA   (NVR0L),Y
      INC   NVR0L
      BNE   :+
      INC   NVR0H
:     LDA   NVR2L
      BEQ   :+
      INC   NVR2L
      BNE   :+
      INC   NVR2H
:     LDA   NVR5L
      BNE   :+
      DEC   NVR5H
:     DEC   NVR5L
      BRA   @copy
@copied:
      RTS
block_code_end:

block_symbols:
      .word block_copy-block_code
      .byte 0, NOBJ_SYM_GLOBAL, 14, "I_P_BLOCK_COPY"
block_object_end:

      ; Word arithmetic and comparison materialization share one compact
      ; member. JSR and RTS preserve the incoming comparison flags.
      .word ordinal_object_end-ordinal_object
ordinal_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 1, $FF
      .word 0
      .word 12
      .word ordinal_symbols-ordinal_object
      .word 0
      .word ordinal_object_end-ordinal_object
      .word 0

      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_EXEC, 0, 4, 0
      .word ordinal_code_end-ordinal_code
      .word ordinal_code_end-ordinal_code
      .byte "CODE"
ordinal_code:
ordinal_add:
      CLC
      ADC   NVR0L
      STA   NVR0L
      TXA
      ADC   NVR0H
      TAX
      LDA   NVR0L
      RTS
ordinal_subtract:
      SEC
      SBC   NVR0L
      STA   NVR0L
      TXA
      SBC   NVR0H
      TAX
      LDA   NVR0L
      RTS
ordinal_multiply:
      STA   MATH_MUL16_A_LO
      STX   MATH_MUL16_A_HI
      LDA   NVR0L
      STA   MATH_MUL16_B_LO
      LDA   NVR0H
      STA   MATH_MUL16_B_HI
      LDA   MATH_RES0
      LDX   MATH_RES1
      RTS
ordinal_divide:
      JSR   ordinal_divide_common
      LDA   MATH_RES0
      LDX   MATH_RES1
      RTS
ordinal_modulo:
      JSR   ordinal_divide_common
      LDA   MATH_RES2
      LDX   MATH_RES3
      RTS
ordinal_divide_common:
      STA   MATH_DIV_N_LO
      STX   MATH_DIV_N_1
      TXA
      BPL   :+
      LDA   #$FF
      BRA   :++
:     LDA   #0
:     STA   MATH_DIV_N_2
      STA   MATH_DIV_N_HI
      LDA   NVR0L
      ORA   NVR0H
      BNE   :+
      STP
:     LDA   NVR0L
      STA   MATH_DIV_D_LO
      LDA   NVR0H
      STA   MATH_DIV_D_HI
      RTS
ordinal_compare:
      CPX   NVR0H
      BNE   ordinal_compare_done
      CMP   NVR0L
ordinal_compare_done:
      RTS
ordinal_eq:
      BEQ   ordinal_true
      BRA   ordinal_false
ordinal_ne:
      BNE   ordinal_true
      BRA   ordinal_false
ordinal_lt:
      BCC   ordinal_true
      BRA   ordinal_false
ordinal_le:
      BCC   ordinal_true
      BEQ   ordinal_true
      BRA   ordinal_false
ordinal_gt:
      BCC   ordinal_false
      BEQ   ordinal_false
      BRA   ordinal_true
ordinal_ge:
      BCS   ordinal_true
      BRA   ordinal_false
ordinal_true:
      LDA   #1
      RTS
ordinal_false:
      LDA   #0
      RTS
ordinal_code_end:

ordinal_symbols:
      .word ordinal_add-ordinal_code
      .byte 0, NOBJ_SYM_GLOBAL, 6, "P_ADDW"
      .word ordinal_subtract-ordinal_code
      .byte 0, NOBJ_SYM_GLOBAL, 6, "P_SUBW"
      .word ordinal_multiply-ordinal_code
      .byte 0, NOBJ_SYM_GLOBAL, 6, "P_MULW"
      .word ordinal_divide-ordinal_code
      .byte 0, NOBJ_SYM_GLOBAL, 6, "P_DIVW"
      .word ordinal_modulo-ordinal_code
      .byte 0, NOBJ_SYM_GLOBAL, 6, "P_MODW"
      .word ordinal_compare-ordinal_code
      .byte 0, NOBJ_SYM_GLOBAL, 6, "P_CMPW"
      .word ordinal_eq-ordinal_code
      .byte 0, NOBJ_SYM_GLOBAL, 4, "P_EQ"
      .word ordinal_ne-ordinal_code
      .byte 0, NOBJ_SYM_GLOBAL, 4, "P_NE"
      .word ordinal_lt-ordinal_code
      .byte 0, NOBJ_SYM_GLOBAL, 4, "P_LT"
      .word ordinal_le-ordinal_code
      .byte 0, NOBJ_SYM_GLOBAL, 4, "P_LE"
      .word ordinal_gt-ordinal_code
      .byte 0, NOBJ_SYM_GLOBAL, 4, "P_GT"
      .word ordinal_ge-ordinal_code
      .byte 0, NOBJ_SYM_GLOBAL, 4, "P_GE"
ordinal_object_end:

      ; One shared lexical-frame arena is extracted only when compiled code
      ; references a routine frame. Programs without routines pay nothing.
      .word frame_object_end-frame_object
frame_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 2, $FF
      .word 0
      .word 8
      .word frame_symbols-frame_object
      .word 20
      .word frame_relocations-frame_object
      .word 0

      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_WRITE | NOBJ_SEC_ZEROFILL, 0, 3, 0
      .word $0412
      .word 0
      .byte "BSS"

      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_EXEC, 0, 4, 0
      .word frame_code_end-frame_code
      .word frame_code_end-frame_code
      .byte "CODE"

frame_code:
; Enter a lexical frame. Inline metadata is display offset ($FF for none),
; little-endian frame size, parameter count, and a width mask whose low bit
; describes the last parameter. Bit 7 of the count reserves frame bytes two
; and three for a wide function result. One parameter arrives in NVR1; two or
; more arrive on the hardware stack.
frame_enter:
      .byte $AD
frame_enter_load_sp: .word 0               ; LDA __NP_SP
      .byte $0D
frame_enter_ora_sp: .word 0                ; ORA __NP_SP+1
      BNE   frame_enter_stack_ready
      .byte $A9
frame_enter_stack_lo: .byte 0              ; LDA #<__NP_STACK
      .byte $8D
frame_enter_store_sp_init: .word 0          ; STA __NP_SP
      .byte $A9
frame_enter_stack_hi: .byte 0              ; LDA #>__NP_STACK
      .byte $8D
frame_enter_store_sp_hi_init: .word 0       ; STA __NP_SP+1
frame_enter_stack_ready:
      .byte $AD
frame_enter_load_sp_again: .word 0          ; LDA __NP_SP
      STA   NVR0L
      .byte $AD
frame_enter_load_sp_hi_again: .word 0       ; LDA __NP_SP+1
      STA   NVR0H

      PLA
      STA   NVR6L
      PLA
      STA   NVR6H
      CLC
      LDA   NVR6L
      ADC   #1
      STA   NVR7L
      LDA   NVR6H
      ADC   #0
      STA   NVR7H
      LDY   #0
      LDA   (NVR7L),Y
      STA   NVR4L                         ; display offset
      INY
      LDA   (NVR7L),Y
      STA   NVR4H                         ; frame size low
      INY
      LDA   (NVR7L),Y
      STA   NVR5L                         ; frame size high
      INY
      LDA   (NVR7L),Y
      STA   NVR2L                         ; parameter count
      AND   #$80
      STA   NVR3H                         ; wide-result frame flag
      LDA   NVR2L
      AND   #$7F
      STA   NVR2L
      INY
      LDA   (NVR7L),Y
      STA   NVR2H                         ; 1 = two-byte parameter
      CLC
      LDA   NVR6L
      ADC   #5
      STA   NVR6L
      LDA   NVR6H
      ADC   #0
      STA   NVR6H

      LDA   NVR2L
      BEQ   frame_enter_parameters_done
      CMP   #1
      BNE   frame_enter_stacked_parameters
      LDY   #2
      LDA   NVR3H
      BPL   :+
      INY
      INY
:
      LDA   NVR1L
      STA   (NVR0L),Y
      LSR   NVR2H
      BCC   frame_enter_parameters_done
      INY
      LDA   NVR1H
      STA   (NVR0L),Y
      BRA   frame_enter_parameters_done

frame_enter_stacked_parameters:
      ; Find the byte immediately beyond the packed frame parameters.
      LDA   NVR2L
      STA   NVR3L
      STA   NVR7L
      LDA   NVR3H
      BPL   :+
      INC   NVR7L
      INC   NVR7L
:
      LDA   NVR2H
      STA   NVR3H
frame_enter_count_parameter_bytes:
      LSR   NVR3H
      BCC   :+
      INC   NVR7L
:
      DEC   NVR3L
      BNE   frame_enter_count_parameter_bytes
      INC   NVR7L
      INC   NVR7L
      TSX
      TXA
      CLC
      ; P_FENTER has removed its own return address, but the enclosing
      ; routine's JSR return pair still precedes the packed arguments.
      ADC   #3
      STA   NVR7H                         ; last argument low byte on stack
      LDA   NVR2L
      STA   NVR3L
frame_enter_copy_parameter:
      LSR   NVR2H
      BCS   frame_enter_copy_word
      DEC   NVR7L
      LDY   NVR7H
      LDA   $0100,Y
      INC   NVR7H
      LDY   NVR7L
      STA   (NVR0L),Y
      BRA   frame_enter_next_parameter
frame_enter_copy_word:
      DEC   NVR7L
      DEC   NVR7L
      LDY   NVR7H
      LDA   $0100,Y
      INC   NVR7H
      LDY   NVR7L
      STA   (NVR0L),Y
      LDY   NVR7H
      LDA   $0100,Y
      INC   NVR7H
      LDY   NVR7L
      INY
      STA   (NVR0L),Y
frame_enter_next_parameter:
      DEC   NVR3L
      BNE   frame_enter_copy_parameter
frame_enter_parameters_done:
      LDA   __NP_FP
      PHA
      LDA   __NP_FP+1
      PHA
      LDA   NVR6H
      PHA
      LDA   NVR6L
      PHA
      LDA   NVR0L
      STA   __NP_FP
      LDA   NVR0H
      STA   __NP_FP+1

      LDA   NVR4L
      CMP   #$FF
      BEQ   frame_enter_size
      TAY
      .byte $B9
frame_enter_load_display_low: .word 0       ; LDA __NP_DISPLAY,Y
      LDY   #0
      STA   (NVR0L),Y
      LDY   NVR4L
      INY
      .byte $B9
frame_enter_load_display_high: .word 0      ; LDA __NP_DISPLAY,Y
      LDY   #1
      STA   (NVR0L),Y
      LDY   NVR4L
      LDA   NVR0L
      .byte $99
frame_enter_store_display_low: .word 0      ; STA __NP_DISPLAY,Y
      INY
      LDA   NVR0H
      .byte $99
frame_enter_store_display_high: .word 0     ; STA __NP_DISPLAY,Y
frame_enter_size:
      CLC
      LDA   NVR0L
      ADC   NVR4H
      .byte $8D
frame_enter_store_new_sp: .word 0           ; STA __NP_SP
      LDA   NVR0H
      ADC   NVR5L
      .byte $8D
frame_enter_store_new_sp_hi: .word 0        ; STA __NP_SP+1
      RTS

; Leave a lexical frame while preserving the routine's original return frame.
; Inline metadata is the display offset followed by the number of caller stack
; bytes to discard. Bit 7 marks a function result above the saved frame and
; bit 6 marks a two-byte A/X result.
frame_leave:
      PLA
      STA   NVR5L
      PLA
      STA   NVR5H
      CLC
      LDA   NVR5L
      ADC   #1
      STA   NVR7L
      LDA   NVR5H
      ADC   #0
      STA   NVR7H
      LDY   #0
      LDA   (NVR7L),Y
      STA   NVR4L
      INY
      LDA   (NVR7L),Y
      STA   NVR4H
      CLC
      LDA   NVR5L
      ADC   #2
      STA   NVR5L
      LDA   NVR5H
      ADC   #0
      STA   NVR5H
      LDA   NVR4H
      BPL   frame_leave_no_result
      AND   #$40
      BEQ   frame_leave_byte_result
      LDY   #2
      LDA   (__NP_FP),Y
      STA   NVR3L
      INY
      LDA   (__NP_FP),Y
      STA   NVR3H
      BRA   frame_leave_no_result
frame_leave_byte_result:
      PLA
      STA   NVR3L
frame_leave_no_result:
      LDA   NVR4L
      CMP   #$FF
      BEQ   frame_leave_restore_stack
      LDY   #0
      LDA   (__NP_FP),Y
      LDY   NVR4L
      .byte $99
frame_leave_store_display_low: .word 0      ; STA __NP_DISPLAY,Y
      LDY   #1
      LDA   (__NP_FP),Y
      LDY   NVR4L
      INY
      .byte $99
frame_leave_store_display_high: .word 0     ; STA __NP_DISPLAY,Y
frame_leave_restore_stack:
      LDA   __NP_FP
      .byte $8D
frame_leave_store_sp: .word 0               ; STA __NP_SP
      LDA   __NP_FP+1
      .byte $8D
frame_leave_store_sp_hi: .word 0            ; STA __NP_SP+1
      PLA
      STA   NVR6H
      PLA
      STA   NVR6L
      PLA
      STA   NVR7L                         ; original routine return low
      PLA
      STA   NVR7H                         ; original routine return high
      LDA   NVR4H
      AND   #$3F
      TAX
      BEQ   frame_leave_arguments_done
frame_leave_drop_argument:
      PLA
      DEX
      BNE   frame_leave_drop_argument
frame_leave_arguments_done:
      LDA   NVR7H
      PHA
      LDA   NVR7L
      PHA
      LDA   NVR4H
      BPL   frame_leave_result_done
      AND   #$40
      BNE   frame_leave_result_done
      LDA   NVR3L
      PHA
frame_leave_result_done:
      LDA   NVR5H
      PHA
      LDA   NVR5L
      PHA
      LDA   NVR6L
      STA   __NP_FP
      LDA   NVR6H
      STA   __NP_FP+1
      LDA   NVR4H
      CMP   #$C0
      BCC   :+
      LDA   NVR3L
      LDX   NVR3H
:
      RTS

; Resolve a lexical frame plus a 16-bit offset. Returns the address in A/X and
; NVR0, matching the compiler's existing indirect load/store convention.
frame_address:
      PLA
      STA   NVR5L
      PLA
      STA   NVR5H
      CLC
      LDA   NVR5L
      ADC   #1
      STA   NVR6L
      LDA   NVR5H
      ADC   #0
      STA   NVR6H
      LDY   #0
      LDA   (NVR6L),Y
      STA   NVR4L                         ; display offset
      INY
      LDA   (NVR6L),Y
      STA   NVR4H                         ; address offset low
      INY
      LDA   (NVR6L),Y
      STA   NVR7L                         ; address offset high
      CLC
      LDA   NVR5L
      ADC   #3
      STA   NVR5L
      LDA   NVR5H
      ADC   #0
      STA   NVR5H
      LDA   NVR5H
      PHA
      LDA   NVR5L
      PHA
      LDY   NVR4L
      BMI   frame_address_current
      .byte $B9
frame_address_load_display_low: .word 0     ; LDA __NP_DISPLAY,Y
      PHA
      INY
      .byte $BE
frame_address_load_display_high: .word 0    ; LDX __NP_DISPLAY,Y
      PLA
      BRA   frame_address_base_ready
frame_address_current:
      LDA   __NP_FP
      LDX   __NP_FP+1
frame_address_base_ready:
      CLC
      ADC   NVR4H
      STA   NVR0L
      TXA
      ADC   NVR7L
      STA   NVR0H
      TAX
      LDA   NVR0L
      RTS
frame_code_end:

frame_symbols:
      .word 0
      .byte 0, NOBJ_SYM_GLOBAL, 7, "__NP_SP"
      .word 2
      .byte 0, NOBJ_SYM_GLOBAL, 12, "__NP_DISPLAY"
      .word 18
      .byte 0, NOBJ_SYM_GLOBAL, 10, "__NP_STACK"
      .word $0212
      .byte 0, NOBJ_SYM_GLOBAL, 12, "__NP_STRBUF0"
      .word $0312
      .byte 0, NOBJ_SYM_GLOBAL, 12, "__NP_STRBUF1"
      .word frame_enter-frame_code
      .byte 1, NOBJ_SYM_GLOBAL, 8, "P_FENTER"
      .word frame_leave-frame_code
      .byte 1, NOBJ_SYM_GLOBAL, 8, "P_FLEAVE"
      .word frame_address-frame_code
      .byte 1, NOBJ_SYM_GLOBAL, 7, "P_FADDR"

frame_relocations:
      .byte 1, NOBJ_RELOC_ABS16
      .word frame_enter_load_sp-frame_code, 0, 0
      .byte 1, NOBJ_RELOC_ABS16
      .word frame_enter_ora_sp-frame_code, 0, 1
      .byte 1, NOBJ_RELOC_LO8
      .word frame_enter_stack_lo-frame_code, 2, 0
      .byte 1, NOBJ_RELOC_ABS16
      .word frame_enter_store_sp_init-frame_code, 0, 0
      .byte 1, NOBJ_RELOC_HI8
      .word frame_enter_stack_hi-frame_code, 2, 0
      .byte 1, NOBJ_RELOC_ABS16
      .word frame_enter_store_sp_hi_init-frame_code, 0, 1
      .byte 1, NOBJ_RELOC_ABS16
      .word frame_enter_load_sp_again-frame_code, 0, 0
      .byte 1, NOBJ_RELOC_ABS16
      .word frame_enter_load_sp_hi_again-frame_code, 0, 1
      .byte 1, NOBJ_RELOC_ABS16
      .word frame_enter_load_display_low-frame_code, 1, 0
      .byte 1, NOBJ_RELOC_ABS16
      .word frame_enter_load_display_high-frame_code, 1, 0
      .byte 1, NOBJ_RELOC_ABS16
      .word frame_enter_store_display_low-frame_code, 1, 0
      .byte 1, NOBJ_RELOC_ABS16
      .word frame_enter_store_display_high-frame_code, 1, 0
      .byte 1, NOBJ_RELOC_ABS16
      .word frame_enter_store_new_sp-frame_code, 0, 0
      .byte 1, NOBJ_RELOC_ABS16
      .word frame_enter_store_new_sp_hi-frame_code, 0, 1
      .byte 1, NOBJ_RELOC_ABS16
      .word frame_leave_store_display_low-frame_code, 1, 0
      .byte 1, NOBJ_RELOC_ABS16
      .word frame_leave_store_display_high-frame_code, 1, 0
      .byte 1, NOBJ_RELOC_ABS16
      .word frame_leave_store_sp-frame_code, 0, 0
      .byte 1, NOBJ_RELOC_ABS16
      .word frame_leave_store_sp_hi-frame_code, 0, 1
      .byte 1, NOBJ_RELOC_ABS16
      .word frame_address_load_display_low-frame_code, 1, 0
      .byte 1, NOBJ_RELOC_ABS16
      .word frame_address_load_display_high-frame_code, 1, 0
frame_object_end:

      ; WITH uses its own address stack rather than the 6502 hardware stack.
      ; That keeps record selection stable across expression temporaries,
      ; nested calls, and nested WITH statements.
      .word with_code_object_end-with_code_object
with_code_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 1, $FF
      .word 0
      .word 4
      .word with_code_symbols-with_code_object
      .word 6
      .word with_code_relocations-with_code_object
      .word 0

      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_EXEC, 0, 4, 0
      .word with_code_end-with_code
      .word with_code_end-with_code
      .byte "CODE"
with_code:
with_push:
      .byte $AC,$00,$00                    ; LDY __NP_WITH_SP
      .byte $99,$00,$00                    ; STA __NP_WITH_STACK,Y
      .byte $C8,$8A                        ; INY / TXA
      .byte $99,$00,$00                    ; STA __NP_WITH_STACK,Y
      .byte $C8,$8C,$00,$00,$60            ; INY / STY __NP_WITH_SP / RTS
with_pop:
      .byte $CE,$00,$00,$CE,$00,$00,$60    ; DEC SP / DEC SP / RTS
with_code_end:

with_code_symbols:
      .word with_push-with_code
      .byte 0, NOBJ_SYM_GLOBAL, 11, "P_WITH_PUSH"
      .word with_pop-with_code
      .byte 0, NOBJ_SYM_GLOBAL, 10, "P_WITH_POP"
      .word 0
      .byte NOBJ_SYM_UNDEFINED, NOBJ_SYM_GLOBAL, 12, "__NP_WITH_SP"
      .word 0
      .byte NOBJ_SYM_UNDEFINED, NOBJ_SYM_GLOBAL, 15, "__NP_WITH_STACK"

with_code_relocations:
      .byte 0, NOBJ_RELOC_ABS16
      .word 1, 2, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word 4, 3, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word 9, 3, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word 13, 2, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word 17, 2, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word 20, 2, 0
with_code_object_end:

      .word with_state_object_end-with_state_object
with_state_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 1, $FF
      .word 0
      .word 2
      .word with_state_symbols-with_state_object
      .word 0
      .word with_state_object_end-with_state_object
      .word 0

      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_WRITE | NOBJ_SEC_ZEROFILL, 0, 3, 0
      .word 65
      .word 0
      .byte "BSS"

with_state_symbols:
      .word 0
      .byte 0, NOBJ_SYM_GLOBAL, 12, "__NP_WITH_SP"
      .word 1
      .byte 0, NOBJ_SYM_GLOBAL, 15, "__NP_WITH_STACK"
with_state_object_end:

      ; Membership helpers consume the byte member saved beneath their JSR
      ; return address. Small enum sets arrive as a byte; set-of-Char values
      ; arrive as a pointer to a packed 256-bit bitmap.
      .word set_object_end-set_object
set_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 1, $FF
      .word 0
      .word 2
      .word set_symbols-set_object
      .word 0
      .word set_object_end-set_object
      .word 0

      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_EXEC, 0, 4, 0
      .word set_code_end-set_code
      .word set_code_end-set_code
      .byte "CODE"
set_code:
set_in8:
      STA   NVR0L
      STZ   NVR2L
      BRA   set_take_member
set_in256:
      STA   NVR0L
      STX   NVR0H
      LDA   #1
      STA   NVR2L
set_take_member:
      TSX
      LDA   $0103,X
      STA   NVR1L
      LDA   $0102,X
      STA   $0103,X
      LDA   $0101,X
      STA   $0102,X
      INX
      TXS
      LDA   NVR2L
      BNE   set_large
      LDA   NVR1L
      CMP   #8
      BCS   set_false
      TAX
      LDA   #1
set_small_shift:
      CPX   #0
      BEQ   set_test_small
      ASL
      DEX
      BRA   set_small_shift
set_test_small:
      AND   NVR0L
      BEQ   set_false
      BRA   set_true
set_large:
      LDA   NVR1L
      AND   #7
      TAX
      LDA   #1
set_large_shift:
      CPX   #0
      BEQ   set_large_masked
      ASL
      DEX
      BRA   set_large_shift
set_large_masked:
      STA   NVR1H
      LDA   NVR1L
      LSR
      LSR
      LSR
      TAY
      LDA   (NVR0L),Y
      AND   NVR1H
      BEQ   set_false
set_true:
      LDA   #1
      RTS
set_false:
      LDA   #0
      RTS
set_code_end:

set_symbols:
      .word set_in8-set_code
      .byte 0, NOBJ_SYM_GLOBAL, 9, "P_SET_IN8"
      .word set_in256-set_code
      .byte 0, NOBJ_SYM_GLOBAL, 11, "P_SET_IN256"
set_object_end:

      ; Copy a Turbo-style short string (length byte followed by data). The
      ; destination capacity is the byte immediately following the call.
      .word string_object_end-string_object
string_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 1, $FF
      .word 0
      .word 12
      .word string_symbols-string_object
      .word 11
      .word string_relocations-string_object
      .word 0

      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_EXEC, 0, 4, 0
      .word string_code_end-string_code
      .word string_code_end-string_code
      .byte "CODE"
string_code:
string_copy:
      STA   NVR1L
      STX   NVR1H
      TSX
      CLC
      LDA   $0101,X
      ADC   #1
      STA   NVR3L
      LDA   $0102,X
      ADC   #0
      STA   NVR3H
      LDY   #0
      LDA   (NVR3L),Y
      STA   NVR4L
      INC   $0101,X
      BNE   :+
      INC   $0102,X
:     LDA   (NVR1L),Y
      CMP   NVR4L
      BCC   @length
      BEQ   @length
      LDA   NVR4L
@length:
      STA   (NVR0L),Y
      TAX
      BEQ   @done
      INY
@copy:
      LDA   (NVR1L),Y
      STA   (NVR0L),Y
      INY
      DEX
      BNE   @copy
@done:
      RTS

string_compare:
      STA   NVR1L
      STX   NVR1H
      LDY   #0
      LDA   (NVR0L),Y
      STA   NVR2L
      LDA   (NVR1L),Y
      STA   NVR2H
      INY
@compare:
      LDA   NVR2L
      BEQ   @left_end
      LDA   NVR2H
      BEQ   @right_end
      LDA   (NVR0L),Y
      CMP   (NVR1L),Y
      BNE   @compared
      INC   NVR0L
      BNE   :+
      INC   NVR0H
:     INC   NVR1L
      BNE   :+
      INC   NVR1H
:     DEC   NVR2L
      DEC   NVR2H
      BRA   @compare
@left_end:
      LDA   #0
      CMP   NVR2H
      RTS
@right_end:
      LDA   NVR2L
      CMP   #0
@compared:
      RTS

string_select_temp:
      LDA   NVR0L
      .byte $C9
string_cmp_buf0_lo: .byte 0
      BNE   string_use_buf0
      LDA   NVR0H
      .byte $C9
string_cmp_buf0_hi: .byte 0
      BNE   string_use_buf0
      .byte $A9
string_load_buf1_lo: .byte 0
      .byte $A2
string_load_buf1_hi: .byte 0
      RTS
string_use_buf0:
      .byte $A9
string_load_buf0_lo: .byte 0
      .byte $A2
string_load_buf0_hi: .byte 0
      RTS

string_concat:
      STA   NVR1L
      STX   NVR1H
      .byte $20
string_concat_select: .word 0
      STA   NVR5L
      STA   NVR2L
      STX   NVR5H
      STX   NVR2H
      LDY   #0
      LDA   (NVR0L),Y
      STA   NVR3L
      LDA   (NVR1L),Y
      STA   NVR3H
      CLC
      ADC   NVR3L
      BCC   :+
      LDA   #$FF
:     STA   (NVR2L),Y
      SEC
      SBC   NVR3L
      STA   NVR4L
      LDX   NVR3L
      BEQ   @right
      LDY   #1
@left:
      LDA   (NVR0L),Y
      STA   (NVR2L),Y
      INY
      DEX
      BNE   @left
@right:
      CLC
      LDA   NVR2L
      ADC   NVR3L
      STA   NVR2L
      BCC   :+
      INC   NVR2H
:     LDX   NVR4L
      BEQ   @concat_done
      LDY   #1
@right_copy:
      LDA   (NVR1L),Y
      STA   (NVR2L),Y
      INY
      DEX
      BNE   @right_copy
@concat_done:
      LDA   NVR5L
      LDX   NVR5H
      RTS

string_concat_char:
      STA   NVR3H
      .byte $20
string_concat_char_select: .word 0
      STA   NVR5L
      STA   NVR2L
      STX   NVR5H
      STX   NVR2H
      LDY   #0
      LDA   (NVR0L),Y
      STA   NVR3L
      CMP   #$FF
      BEQ   @char_length
      INC
@char_length:
      STA   (NVR2L),Y
      LDX   NVR3L
      BEQ   @append_char
      LDY   #1
@char_left:
      LDA   (NVR0L),Y
      STA   (NVR2L),Y
      INY
      DEX
      BNE   @char_left
@append_char:
      LDA   NVR3L
      CMP   #$FF
      BEQ   @char_done
      TAY
      INY
      LDA   NVR3H
      STA   (NVR2L),Y
@char_done:
      LDA   NVR5L
      LDX   NVR5H
      RTS

char_concat_string:
      STA   NVR1L
      STX   NVR1H
      LDA   NVR0L
      STA   NVR3H
      LDA   NVR1L
      STA   NVR0L
      LDA   NVR1H
      STA   NVR0H
      .byte $20
char_concat_select: .word 0
      STA   NVR5L
      STA   NVR2L
      STX   NVR5H
      STX   NVR2H
      LDY   #0
      LDA   (NVR1L),Y
      CMP   #$FF
      BEQ   :+
      INC
:     STA   (NVR2L),Y
      INY
      LDA   NVR3H
      STA   (NVR2L),Y
      LDA   (NVR1L)
      CMP   #$FF
      BCC   :+
      LDA   #$FE
:     TAX
      BEQ   @char_string_done
      INC   NVR2L
      BNE   :+
      INC   NVR2H
:     LDY   #1
@char_string_copy:
      LDA   (NVR1L),Y
      STA   (NVR2L),Y
      INY
      DEX
      BNE   @char_string_copy
@char_string_done:
      LDA   NVR5L
      LDX   NVR5H
      RTS

char_to_string:
      STA   NVR3H
      STZ   NVR0L
      STZ   NVR0H
      .byte $20
char_to_string_select: .word 0
      STA   NVR5L
      STA   NVR2L
      STX   NVR5H
      STX   NVR2H
      LDY   #0
      LDA   #1
      STA   (NVR2L),Y
      INY
      LDA   NVR3H
      STA   (NVR2L),Y
      LDA   NVR5L
      LDX   NVR5H
      RTS

string_copy_part:
      STA   NVR4L
      STX   NVR4H
      TSX
      LDA   $0103,X
      STA   NVR3H
      LDA   $0104,X
      STA   NVR3L
      LDA   $0105,X
      STA   NVR0L
      LDA   $0106,X
      STA   NVR0H
      .byte $20
string_copy_part_select: .word 0
      STA   NVR5L
      STA   NVR2L
      STX   NVR5H
      STX   NVR2H
      LDY   #0
      LDA   NVR3L
      BNE   @empty_part
      LDA   NVR3H
      BEQ   @empty_part
      LDA   (NVR0L),Y
      CMP   NVR3H
      BCC   @empty_part
      SEC
      SBC   NVR3H
      INC
      CMP   NVR4L
      BCC   :+
      LDA   NVR4L
:     STA   (NVR2L),Y
      TAX
      BEQ   @part_done
      CLC
      LDA   NVR0L
      ADC   NVR3H
      STA   NVR0L
      BCC   :+
      INC   NVR0H
:     INC   NVR2L
      BNE   :+
      INC   NVR2H
:     LDY   #0
@part_copy:
      LDA   (NVR0L),Y
      STA   (NVR2L),Y
      INC   NVR0L
      BNE   :+
      INC   NVR0H
:     INC   NVR2L
      BNE   :+
      INC   NVR2H
:     DEX
      BNE   @part_copy
      BRA   @part_done
@empty_part:
      LDA   #0
      STA   (NVR2L),Y
@part_done:
      LDA   NVR5L
      LDX   NVR5H
      RTS

string_delete:
      TSX
      LDA   $0107,X
      STA   NVR0L
      LDA   $0108,X
      STA   NVR0H
      LDA   $0106,X
      BNE   @delete_done
      LDA   $0105,X
      BEQ   @delete_done
      STA   NVR1L
      LDA   $0104,X
      BNE   @delete_to_end
      LDA   $0103,X
      STA   NVR1H
      BRA   @delete_count
@delete_to_end:
      LDA   #$FF
      STA   NVR1H
@delete_count:
      LDY   #0
      LDA   (NVR0L),Y
      CMP   NVR1L
      BCC   @delete_done
      SEC
      SBC   NVR1L
      INC
      CMP   NVR1H
      BCS   :+
      STA   NVR1H
:     LDA   (NVR0L),Y
      SEC
      SBC   NVR1H
      STA   (NVR0L),Y
      SEC
      SBC   NVR1L
      INC
      TAX
      BEQ   @delete_done
      LDA   NVR1L
      CLC
      ADC   NVR1H
      TAY
@delete_move:
      LDA   (NVR0L),Y
      PHY
      PHA
      TYA
      SEC
      SBC   NVR1H
      TAY
      PLA
      STA   (NVR0L),Y
      PLY
      INY
      DEX
      BNE   @delete_move
@delete_done:
      RTS

string_insert_char:
      TSX
      LDA   $0105,X
      STA   NVR0L
      LDA   $0106,X
      STA   NVR0H
      LDA   $0107,X
      STA   NVR1L
      LDA   $0104,X
      BNE   @insert_done
      LDA   $0103,X
      BNE   :+
      INC
:     STA   NVR1H
      LDY   #0
      LDA   (NVR0L),Y
      CMP   #$FF
      BEQ   @insert_done
      TAX
      INC
      CMP   NVR1H
      BCS   :+
      STA   NVR1H
:     TXA
      TAY
@insert_shift:
      CPY   NVR1H
      BCC   @insert_store
      LDA   (NVR0L),Y
      INY
      STA   (NVR0L),Y
      DEY
      DEY
      BRA   @insert_shift
@insert_store:
      LDY   NVR1H
      LDA   NVR1L
      STA   (NVR0L),Y
      LDY   #0
      LDA   (NVR0L),Y
      INC
      STA   (NVR0L),Y
@insert_done:
      RTS
string_code_end:

string_symbols:
      .word string_copy-string_code
      .byte 0, NOBJ_SYM_GLOBAL, 15, "I_P_STRING_COPY"
      .word string_compare-string_code
      .byte 0, NOBJ_SYM_GLOBAL, 16, "P_STRING_COMPARE"
      .word string_concat-string_code
      .byte 0, NOBJ_SYM_GLOBAL, 15, "P_STRING_CONCAT"
      .word string_concat_char-string_code
      .byte 0, NOBJ_SYM_GLOBAL, 20, "P_STRING_CONCAT_CHAR"
      .word char_concat_string-string_code
      .byte 0, NOBJ_SYM_GLOBAL, 20, "P_CHAR_CONCAT_STRING"
      .word char_to_string-string_code
      .byte 0, NOBJ_SYM_GLOBAL, 16, "P_CHAR_TO_STRING"
      .word string_copy_part-string_code
      .byte 0, NOBJ_SYM_GLOBAL, 18, "P_STRING_COPY_PART"
      .word string_delete-string_code
      .byte 0, NOBJ_SYM_GLOBAL, 6, "DELETE"
      .word string_insert_char-string_code
      .byte 0, NOBJ_SYM_GLOBAL, 6, "INSERT"
      .word string_select_temp-string_code
      .byte 0, 0, 10, "STR_SELECT"
      .word 0
      .byte NOBJ_SYM_UNDEFINED, NOBJ_SYM_GLOBAL, 12, "__NP_STRBUF0"
      .word 0
      .byte NOBJ_SYM_UNDEFINED, NOBJ_SYM_GLOBAL, 12, "__NP_STRBUF1"

string_relocations:
      .byte 0, NOBJ_RELOC_LO8
      .word string_cmp_buf0_lo-string_code, 10, 0
      .byte 0, NOBJ_RELOC_HI8
      .word string_cmp_buf0_hi-string_code, 10, 0
      .byte 0, NOBJ_RELOC_LO8
      .word string_load_buf1_lo-string_code, 11, 0
      .byte 0, NOBJ_RELOC_HI8
      .word string_load_buf1_hi-string_code, 11, 0
      .byte 0, NOBJ_RELOC_LO8
      .word string_load_buf0_lo-string_code, 10, 0
      .byte 0, NOBJ_RELOC_HI8
      .word string_load_buf0_hi-string_code, 10, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word string_concat_select-string_code, 9, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word string_concat_char_select-string_code, 9, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word char_concat_select-string_code, 9, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word char_to_string_select-string_code, 9, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word string_copy_part_select-string_code, 9, 0
string_object_end:

      ; Resident adapter between Pascal's compact register/stack ABI and the
      ; shared paged LANGRT module. One caller-owned result cell remains valid
      ; after the $C000 module has been paged back out.
      .word rtcall_object_end-rtcall_object
rtcall_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 2, $FF
      .word 0
      .word 5
      .word rtcall_symbols-rtcall_object
      .word 8
      .word rtcall_relocations-rtcall_object
      .word 0

      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_WRITE | NOBJ_SEC_ZEROFILL, 0, 3, 0
      .word 6
      .word 0
      .byte "BSS"

      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_EXEC, 0, 4, 0
      .word rtcall_code_end-rtcall_code
      .word rtcall_code_end-rtcall_code
      .byte "CODE"
rtcall_code:
rtcall:
      STA   LIB_ARG0
      STX   LIB_ARG0+1
      LDX   #3
@save_native_arg:
      LDA   NVR0L,X
      STA   LIB_ARG1,X
      DEX
      BPL   @save_native_arg
      STY   LIB_FN_ID
      STY   LANGRT_FN_SAVED
      CPY   #LANGRT_STR_REAL
      BEQ   rtcall_marshal_str
      CPY   #LANGRT_STR_REAL_PRECISION
      BEQ   rtcall_marshal_str_precision
      CPY   #LANGRT_REAL_VAL
      BNE   :+
      .byte $4C
rtcall_marshal_val_jump: .word 0
:
      .byte $4C
rtcall_args_ready_jump: .word 0
rtcall_marshal_str:
      TSX
      LDA   $0105,X                    ; value pointer
      STA   LIB_ARG1
      LDA   $0106,X
      STA   LIB_ARG1+1
      STZ   LIB_ARG1+2
      STZ   LIB_ARG1+3
      LDA   $0103,X                    ; width
      STA   LIB_ARG2
      STZ   LIB_ARG2+1
      STZ   LIB_ARG2+2
      STZ   LIB_ARG2+3
      STZ   LIB_ARG3                   ; default precision is selected by fn
      STZ   LIB_ARG3+1
      STZ   LIB_ARG3+2
      STZ   LIB_ARG3+3
      BRA   rtcall_args_ready
rtcall_marshal_str_precision:
      TSX
      LDA   $0107,X                    ; value pointer
      STA   LIB_ARG1
      LDA   $0108,X
      STA   LIB_ARG1+1
      STZ   LIB_ARG1+2
      STZ   LIB_ARG1+3
      LDA   $0105,X                    ; width
      STA   LIB_ARG2
      STZ   LIB_ARG2+1
      STZ   LIB_ARG2+2
      STZ   LIB_ARG2+3
      LDA   $0103,X                    ; precision
      STA   LIB_ARG3
      STZ   LIB_ARG3+1
      STZ   LIB_ARG3+2
      STZ   LIB_ARG3+3
      BRA   rtcall_args_ready
rtcall_marshal_val:
      TSX
      LDA   $0108,X                    ; source string pointer
      STA   LIB_ARG0
      LDA   $0109,X
      STA   LIB_ARG0+1
      STZ   LIB_ARG0+2
      STZ   LIB_ARG0+3
      LDA   $0106,X                    ; destination value pointer
      STA   LIB_ARG1
      LDA   $0107,X
      STA   LIB_ARG1+1
      STZ   LIB_ARG1+2
      STZ   LIB_ARG1+3
      LDA   $0104,X                    ; error-code pointer
      STA   LIB_ARG2
      LDA   $0105,X
      STA   LIB_ARG2+1
      STZ   LIB_ARG2+2
      STZ   LIB_ARG2+3
      LDA   $0103,X                    ; destination width, 2 or 4 bytes
      STA   LIB_ARG3
      STZ   LIB_ARG3+1
      STZ   LIB_ARG3+2
      STZ   LIB_ARG3+3
rtcall_args_ready:
      PLA
      .byte $8D
rtcall_store_return_low: .word 0           ; STA rtcall_return
      PLA
      .byte $8D
rtcall_store_return_high: .word 0          ; STA rtcall_return+1
      LDA   #MODULE_ID_LANGRT
      STA   LIB_MOD_ID
rtcall_invoke:
      JSR   LIB_LOADER_BAND
      .byte $AD
rtcall_load_return_high: .word 0           ; LDA rtcall_return+1
      PHA
      .byte $AD
rtcall_load_return_low: .word 0            ; LDA rtcall_return
      PHA
      LDA   LANGRT_FN_SAVED
      CMP   #LANGRT_REAL_CMP
      BEQ   rtcall_restore_compare
      CMP   #LANGRT_REAL_TRUNC
      BEQ   rtcall_return_word
      CMP   #LANGRT_STR_REAL
      BEQ   rtcall_return_void
      CMP   #LANGRT_STR_REAL_PRECISION
      BEQ   rtcall_return_void
      CMP   #LANGRT_REAL_VAL
      BEQ   rtcall_return_void
      .byte $A9
rtcall_return_result_low: .byte 0          ; LDA #<rtcall_result
      STA   NVR6L
      .byte $A2
rtcall_return_result_high: .byte 0         ; LDX #>rtcall_result
      STX   NVR6H
      LDY   #0
      LDA   LIB_RESULT
      STA   (NVR6L),Y
      INY
      LDA   LIB_RESULT+1
      STA   (NVR6L),Y
      INY
      LDA   LIB_RESULT+2
      STA   (NVR6L),Y
      INY
      LDA   LIB_RESULT+3
      STA   (NVR6L),Y
      LDA   NVR6L
      LDX   NVR6H
rtcall_return_void:
      RTS
rtcall_return_word:
      LDA   LIB_RESULT
      LDX   LIB_RESULT+1
      RTS
rtcall_restore_compare:
      PHP
      PLA
      AND   #$3C                          ; preserve caller I/D/B/reserved bits
      STA   LANGRT_FLAGS_TMP
      LDA   LIB_RESULT+2
      AND   #$C3                          ; restore module N/V/Z/C results
      ORA   LANGRT_FLAGS_TMP
      PHA
      PLP
      RTS
rtcall_code_end:

rtcall_symbols:
      .word rtcall-rtcall_code
      .byte 1, NOBJ_SYM_GLOBAL, 8, "P_RTCALL"
      .word 0
      .byte 0, 0, 13, "RTCALL_RETURN"
      .word 2
      .byte 0, 0, 13, "RTCALL_RESULT"
      .word rtcall_marshal_val-rtcall_code
      .byte 1, 0, 18, "RTCALL_MARSHAL_VAL"
      .word rtcall_args_ready-rtcall_code
      .byte 1, 0, 17, "RTCALL_ARGS_READY"

rtcall_relocations:
      .byte 1, NOBJ_RELOC_ABS16
      .word rtcall_store_return_low-rtcall_code, 1, 0
      .byte 1, NOBJ_RELOC_ABS16
      .word rtcall_store_return_high-rtcall_code, 1, 1
      .byte 1, NOBJ_RELOC_LO8
      .word rtcall_return_result_low-rtcall_code, 2, 0
      .byte 1, NOBJ_RELOC_HI8
      .word rtcall_return_result_high-rtcall_code, 2, 0
      .byte 1, NOBJ_RELOC_ABS16
      .word rtcall_load_return_high-rtcall_code, 1, 1
      .byte 1, NOBJ_RELOC_ABS16
      .word rtcall_load_return_low-rtcall_code, 1, 0
      .byte 1, NOBJ_RELOC_ABS16
      .word rtcall_marshal_val_jump-rtcall_code, 3, 0
      .byte 1, NOBJ_RELOC_ABS16
      .word rtcall_args_ready_jump-rtcall_code, 4, 0
rtcall_object_end:

      .include "pascal_real_thunks.inc"

      ; Turbo-compatible console services. The implementation uses Nova's
      ; canonical VGC registers and SYSTEM module mailbox; no private device
      ; addresses or wait loops are introduced by Pascal.
      .word system_object_end-system_object
system_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 1, $FF
      .word 0
      .word 20
      .word system_symbols-system_object
      .word 0
      .word system_object_end-system_object
      .word 0

      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_EXEC, 0, 4, 0
      .word system_code_end-system_code
      .word system_code_end-system_code
      .byte "CODE"
system_code:
system_gotoxy:
      TSX
      LDA   $0105,X
      DEC
      STA   VGC_CURSX
      LDA   $0103,X
      DEC
      STA   VGC_CURSY
      RTS

system_clreol:
      PASCAL_SYSTEM_CALL SYS_CONSOLE_CLEAR_EOL
      RTS

system_highvideo:
      LDA   #$00
      STA   VGC_FGCOL
      LDA   #$0F
      STA   VGC_TEXT_BG
      RTS
system_lowvideo:
      LDA   #$07
      STA   VGC_FGCOL
      LDA   #VGC_TEXT_BG_TRANSPARENT
      STA   VGC_TEXT_BG
      RTS

system_delay:
      STA   NVR0L
      STX   NVR0H
      AND   #$0F
      STA   NVR1L
      LSR   NVR0H
      ROR   NVR0L
      LSR   NVR0H
      ROR   NVR0L
      LSR   NVR0H
      ROR   NVR0L
      LSR   NVR0H
      ROR   NVR0L
      LDA   NVR1L
      BEQ   @wait
      INC   NVR0L
      BNE   @wait
      INC   NVR0H
@wait:
      LDA   NVR0H
      BNE   @chunk
      LDA   NVR0L
      BEQ   @done
      STA   LIB_ARG0
      STZ   LIB_ARG0+1
      STZ   LIB_ARG0+2
      STZ   LIB_ARG0+3
      PASCAL_SYSTEM_CALL SYS_FN_WAIT
@done:
      RTS
@chunk:
      LDA   NVR0H
      PHA
      LDA   NVR0L
      PHA
      LDA   #$FF
      STA   LIB_ARG0
      STZ   LIB_ARG0+1
      STZ   LIB_ARG0+2
      STZ   LIB_ARG0+3
      PASCAL_SYSTEM_CALL SYS_FN_WAIT
      PLA
      STA   NVR0L
      PLA
      STA   NVR0H
      SEC
      LDA   NVR0L
      SBC   #$FF
      STA   NVR0L
      LDA   NVR0H
      SBC   #0
      STA   NVR0H
      BRA   @wait

system_upcase:
      CMP   #'a'
      BCC   :+
      CMP   #'z'+1
      BCS   :+
      AND   #$DF
:     RTS

system_clrscr:
      LDA   #$0C
      STA   VGC_CHAROUT
      RTS

system_readkey:
      PASCAL_SYSTEM_CALL SYS_WAIT_KEY
      LDA   LIB_RESULT
      LDX   #0
      RTS

system_wherex:
      LDA   VGC_CURSX
      INC
      LDX   #0
      RTS

system_wherey:
      LDA   VGC_CURSY
      INC
      LDX   #0
      RTS

system_textcolor:
      AND   #$0F
      STA   VGC_FGCOL
      RTS

system_textbackground:
      AND   #$0F
      STA   VGC_TEXT_BG
      RTS

system_succ:
      INC
      RTS
system_pred:
      DEC
      RTS
system_length:
      STA   NVR0L
      STX   NVR0H
      LDA   (NVR0L)
      LDX   #0
      RTS

system_halt:
      PASCAL_MEMORY_CALL MEM_EXIT_IMAGE
      RTS
system_code_end:

system_symbols:
      .word system_gotoxy-system_code
      .byte 0, NOBJ_SYM_GLOBAL, 6, "GOTOXY"
      .word system_clreol-system_code
      .byte 0, NOBJ_SYM_GLOBAL, 6, "CLREOL"
      .word system_highvideo-system_code
      .byte 0, NOBJ_SYM_GLOBAL, 9, "HIGHVIDEO"
      .word system_lowvideo-system_code
      .byte 0, NOBJ_SYM_GLOBAL, 8, "LOWVIDEO"
      .word system_lowvideo-system_code
      .byte 0, NOBJ_SYM_GLOBAL, 9, "NORMVIDEO"
      .word system_delay-system_code
      .byte 0, NOBJ_SYM_GLOBAL, 5, "DELAY"
      .word system_upcase-system_code
      .byte 0, NOBJ_SYM_GLOBAL, 6, "UPCASE"
      .word system_clrscr-system_code
      .byte 0, NOBJ_SYM_GLOBAL, 6, "CLRSCR"
      .word system_succ-system_code
      .byte 0, NOBJ_SYM_GLOBAL, 4, "SUCC"
      .word system_pred-system_code
      .byte 0, NOBJ_SYM_GLOBAL, 4, "PRED"
      .word system_length-system_code
      .byte 0, NOBJ_SYM_GLOBAL, 6, "LENGTH"
      .word system_halt-system_code
      .byte 0, NOBJ_SYM_GLOBAL, 4, "HALT"
      .word system_readkey-system_code
      .byte 0, NOBJ_SYM_GLOBAL, 7, "READKEY"
      .word system_wherex-system_code
      .byte 0, NOBJ_SYM_GLOBAL, 6, "WHEREX"
      .word system_wherey-system_code
      .byte 0, NOBJ_SYM_GLOBAL, 6, "WHEREY"
      .word system_textcolor-system_code
      .byte 0, NOBJ_SYM_GLOBAL, 9, "TEXTCOLOR"
      .word system_textbackground-system_code
      .byte 0, NOBJ_SYM_GLOBAL, 14, "TEXTBACKGROUND"
      .word VGC_CHARIN
      .byte NOBJ_SYM_ABSOLUTE, NOBJ_SYM_GLOBAL, 3, "KBD"
      .word VGC_CHARIN
      .byte NOBJ_SYM_ABSOLUTE, NOBJ_SYM_GLOBAL, 10, "KEYPRESSED"
      .word FIO_ERRCODE
      .byte NOBJ_SYM_ABSOLUTE, NOBJ_SYM_GLOBAL, 8, "IORESULT"
system_object_end:

      ; Pascal file variables are compact 16-byte descriptors:
      ;   +0 file id, +1 mode/logical-EOF flag, +2 name length,
      ;   +3..+15 CP/M-style name.
      ; All I/O is routed through the canonical FILES/SYSTEM NDK modules.
      .word file_object_end-file_object
file_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 1, $FF
      .word 0
      .word 18
      .word file_symbols-file_object
      .word 5
      .word file_relocations-file_object
      .word 0

      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_EXEC, 0, 4, 0
      .word file_code_end-file_code
      .word file_code_end-file_code
      .byte "CODE"
file_code:
file_assign:
      TSX
      LDA   $0105,X
      STA   NVR0L
      LDA   $0106,X
      STA   NVR0H
      LDA   $0103,X
      STA   NVR1L
      LDA   $0104,X
      STA   NVR1H
      LDY   #0
      LDA   #$FF
      STA   (NVR0L),Y
      INY
      LDA   #0
      STA   (NVR0L),Y
      LDY   #0
      LDA   (NVR1L),Y
      CMP   #14
      BCC   :+
      LDA   #13
:     TAX
      LDY   #2
      STA   (NVR0L),Y
      TXA
      BEQ   @assigned
      LDY   #1
@assign_name:
      LDA   (NVR1L),Y
      PHY
      INY
      INY
      STA   (NVR0L),Y
      PLY
      INY
      DEX
      BNE   @assign_name
@assigned:
      RTS

file_reset:
      PHA
      PHX
      STA   NVR0L
      STX   NVR0H
      CLC
      ADC   #3
      STA   LIB_ARG0
      TXA
      ADC   #0
      STA   LIB_ARG0+1
      STZ   LIB_ARG0+2
      STZ   LIB_ARG0+3
      LDY   #2
      LDA   (NVR0L),Y
      STA   LIB_ARG1
      STZ   LIB_ARG1+1
      STZ   LIB_ARG1+2
      STZ   LIB_ARG1+3
      LDA   #FIO_FILE_ACCESS_READ
      STA   LIB_ARG2
      STZ   LIB_ARG2+1
      STZ   LIB_ARG2+2
      STZ   LIB_ARG2+3
      PASCAL_FILE_CALL FILE_FOPEN
      PLX
      PLA
      STA   NVR0L
      STX   NVR0H
      LDY   #0
      LDA   LIB_RESULT
      STA   (NVR0L),Y
      INY
      LDA   #1
      STA   (NVR0L),Y
      RTS

file_rewrite:
      PHA
      PHX
      STA   NVR0L
      STX   NVR0H
      CLC
      ADC   #3
      STA   LIB_ARG0
      TXA
      ADC   #0
      STA   LIB_ARG0+1
      STZ   LIB_ARG0+2
      STZ   LIB_ARG0+3
      LDY   #2
      LDA   (NVR0L),Y
      STA   LIB_ARG1
      STZ   LIB_ARG1+1
      STZ   LIB_ARG1+2
      STZ   LIB_ARG1+3
      LDA   #FIO_FILE_ACCESS_WRITE
      STA   LIB_ARG2
      STZ   LIB_ARG2+1
      STZ   LIB_ARG2+2
      STZ   LIB_ARG2+3
      PASCAL_FILE_CALL FILE_FCREATE
      PLX
      PLA
      STA   NVR0L
      STX   NVR0H
      LDY   #0
      LDA   LIB_RESULT
      STA   (NVR0L),Y
      INY
      LDA   #3
      STA   (NVR0L),Y
      RTS

file_close:
      STA   NVR0L
      STX   NVR0H
      LDY   #0
      LDA   (NVR0L),Y
      STA   LIB_ARG0
      STZ   LIB_ARG0+1
      STZ   LIB_ARG0+2
      STZ   LIB_ARG0+3
      PASCAL_FILE_CALL FILE_FCLOSE
      RTS

file_eof:
      STA   NVR0L
      STX   NVR0H
      LDY   #1
      LDA   (NVR0L),Y
      BMI   @eof_true
      DEY
      LDA   (NVR0L),Y
      STA   LIB_ARG0
      STZ   LIB_ARG0+1
      STZ   LIB_ARG0+2
      STZ   LIB_ARG0+3
      PASCAL_FILE_CALL FILE_FTELL
      LDA   LIB_RESULT
      PHA
      LDA   LIB_RESULT+1
      PHA
      LDA   LIB_RESULT+2
      PHA
      PASCAL_FILE_CALL FILE_FSIZE
      PLA
      CMP   LIB_RESULT+2
      BNE   @not_eof
      PLA
      CMP   LIB_RESULT+1
      BNE   @not_eof_low
      PLA
      CMP   LIB_RESULT
      BNE   @eof_false
@eof_true:
      LDA   #1
      RTS
@not_eof:
      PLA
@not_eof_low:
      PLA
@eof_false:
      LDA   #0
      RTS

file_read:
      TSX
      LDA   $0103,X
      STA   LIB_ARG2
      LDA   $0104,X
      STA   LIB_ARG2+1
      LDA   $0105,X
      STA   LIB_ARG3
      LDA   $0106,X
      STA   LIB_ARG3+1
      LDA   $0107,X
      STA   NVR0L
      LDA   $0108,X
      STA   NVR0H
      CMP   #>VGC_CHARIN
      BNE   file_read_file
      LDA   NVR0L
      CMP   #<VGC_CHARIN
      BEQ   file_read_keyboard
file_read_file:
      LDY   #0
      LDA   (NVR0L),Y
      STA   LIB_ARG0
      STZ   LIB_ARG0+1
      STZ   LIB_ARG0+2
      STZ   LIB_ARG0+3
      LDA   LIB_ARG3
      STA   LIB_ARG1
      LDA   LIB_ARG3+1
      STA   LIB_ARG1+1
      STZ   LIB_ARG1+2
      STZ   LIB_ARG1+3
      STZ   LIB_ARG2+2
      STZ   LIB_ARG2+3
      PASCAL_FILE_CALL FILE_FREAD
      RTS
file_read_keyboard:
file_read_keyboard_target = file_read_keyboard
      LDA   LIB_ARG2+1
      BEQ   file_keyboard_size_low
file_read_jump_string_high = *
      JMP   file_keyboard_string
file_keyboard_size_low:
      LDA   LIB_ARG2
      CMP   #1
      BNE   file_keyboard_not_char
file_read_jump_char = *
      JMP   file_keyboard_char
file_keyboard_not_char:
      CMP   #2
      BEQ   file_keyboard_word
file_read_jump_string_other = *
      JMP   file_keyboard_string
file_keyboard_word:
      PASCAL_LOAD_FILE_DATA_PTR
      LDY   #0
      LDA   #0
      STA   (NVR6L),Y
      INY
      STA   (NVR6L),Y
file_keyboard_number_key:
      PASCAL_SYSTEM_CALL SYS_WAIT_KEY
      LDA   LIB_RESULT
      CMP   #$0D
      BNE   :+
      RTS
:
      CMP   #'0'
      BCC   file_keyboard_number_key
      CMP   #'9'+1
      BCS   file_keyboard_number_key
      PHA
      STA   VGC_CHAROUT
      PASCAL_LOAD_FILE_DATA_PTR
      LDY   #0
      LDA   (NVR6L),Y
      STA   NVR1L
      INY
      LDA   (NVR6L),Y
      STA   NVR1H
      ASL   NVR1L
      ROL   NVR1H
      LDA   NVR1L
      STA   NVR2L
      LDA   NVR1H
      STA   NVR2H
      ASL   NVR1L
      ROL   NVR1H
      ASL   NVR1L
      ROL   NVR1H
      CLC
      LDA   NVR1L
      ADC   NVR2L
      STA   NVR1L
      LDA   NVR1H
      ADC   NVR2H
      STA   NVR1H
      PLA
      SEC
      SBC   #'0'
      CLC
      ADC   NVR1L
      LDY   #0
      STA   (NVR6L),Y
      INY
      LDA   NVR1H
      ADC   #0
      STA   (NVR6L),Y
      BRA   file_keyboard_number_key
file_keyboard_char:
file_keyboard_char_target = file_keyboard_char
      PASCAL_SYSTEM_CALL SYS_WAIT_KEY
      PASCAL_LOAD_FILE_DATA_PTR
      LDY   #0
      LDA   LIB_RESULT
      CMP   #SYS_KEY_UP
      BNE   :+
      LDA   #$05                         ; Turbo/WordStar ^E: cursor up
      BRA   file_keyboard_char_store
:     CMP   #SYS_KEY_DOWN
      BNE   :+
      LDA   #$18                         ; Turbo/WordStar ^X: cursor down
      BRA   file_keyboard_char_store
:     CMP   #SYS_KEY_RIGHT
      BNE   :+
      LDA   #$04                         ; Turbo/WordStar ^D: cursor right
      BRA   file_keyboard_char_store
:     CMP   #SYS_KEY_LEFT
      BNE   file_keyboard_char_store
      LDA   #$13                         ; Turbo/WordStar ^S: cursor left
file_keyboard_char_store:
      STA   (NVR6L),Y
      RTS
file_keyboard_string:
file_keyboard_string_target = file_keyboard_string
      SEC
      LDA   LIB_ARG2
      SBC   #1
      STA   LIB_ARG3+2
      PASCAL_LOAD_FILE_DATA_PTR
      LDY   #0
      LDA   #0
      STA   (NVR6L),Y
file_keyboard_line:
      PASCAL_SYSTEM_CALL SYS_WAIT_KEY
      LDA   LIB_RESULT
      CMP   #$0D
      BNE   :+
      RTS
:
      PHA
      STA   VGC_CHAROUT
      PASCAL_LOAD_FILE_DATA_PTR
      LDY   #0
      LDA   (NVR6L),Y
      CMP   LIB_ARG3+2
      BCS   file_keyboard_line_full
      INC
      STA   (NVR6L),Y
      TAY
      PLA
      STA   (NVR6L),Y
      BRA   file_keyboard_line
file_keyboard_line_full:
      PLA
      BRA   file_keyboard_line

file_readln:
      TSX
      LDA   $0103,X
      STA   LIB_ARG2
      LDA   $0104,X
      STA   LIB_ARG2+1
      LDA   $0105,X
      STA   LIB_ARG3
      LDA   $0106,X
      STA   LIB_ARG3+1
      LDA   $0107,X
      STA   NVR0L
      LDA   $0108,X
      STA   NVR0H
      CMP   #>VGC_CHARIN
      BNE   file_readln_file
      LDA   NVR0L
      CMP   #<VGC_CHARIN
      BNE   file_readln_file
file_readln_jump_keyboard = *
      JMP   file_read_keyboard
file_readln_file:
      LDY   #0
      LDA   (NVR0L),Y
      STA   LIB_ARG0
      STZ   LIB_ARG0+1
      STZ   LIB_ARG0+2
      STZ   LIB_ARG0+3
      LDA   LIB_ARG2
      CMP   #2
      BCS   file_readln_string
file_readln_jump_byte = *
      JMP   file_readln_byte
file_readln_string:
      DEC
      STA   LIB_ARG3+2
      PASCAL_LOAD_FILE_DATA_PTR
      LDY   #0
      LDA   #0
      STA   (NVR6L),Y
file_readln_line:
      PASCAL_LOAD_FILE_DATA_PTR
      LDY   #0
      LDA   (NVR6L),Y
      INC
      CLC
      ADC   LIB_ARG3
      STA   LIB_ARG1
      LDA   LIB_ARG3+1
      ADC   #0
      STA   LIB_ARG1+1
      STZ   LIB_ARG1+2
      STZ   LIB_ARG1+3
      LDA   #1
      STA   LIB_ARG2
      STZ   LIB_ARG2+1
      STZ   LIB_ARG2+2
      STZ   LIB_ARG2+3
      PASCAL_FILE_CALL FILE_FREAD
      PASCAL_LOAD_FILE_DATA_PTR
      LDA   LIB_RESULT
      BEQ   file_readln_done
      LDY   #0
      LDA   (NVR6L),Y
      TAY
      INY
      LDA   (NVR6L),Y
      CMP   #$1A
      BEQ   file_readln_logical_eof
      CMP   #$0A
      BEQ   file_readln_done
      ; Nova text is LF-delimited. Ignore a compatibility CR so imported
      ; CP/M CRLF text still produces one logical line without adopting CRLF
      ; as the native format.
      CMP   #$0D
      BEQ   file_readln_line
      DEY
      TYA
      CMP   LIB_ARG3+2
      BCS   file_readln_line
      INC
      LDY   #0
      STA   (NVR6L),Y
      BRA   file_readln_line
file_readln_logical_eof:
      TSX
      LDA   $0107,X
      STA   NVR0L
      LDA   $0108,X
      STA   NVR0H
      LDY   #1
      LDA   #$81
      STA   (NVR0L),Y
      BRA   file_readln_done
file_readln_byte:
file_readln_byte_target = file_readln_byte
      LDA   LIB_ARG3
      STA   LIB_ARG1
      LDA   LIB_ARG3+1
      STA   LIB_ARG1+1
      STZ   LIB_ARG1+2
      STZ   LIB_ARG1+3
      LDA   #1
      STA   LIB_ARG2
      STZ   LIB_ARG2+1
      STZ   LIB_ARG2+2
      STZ   LIB_ARG2+3
      PASCAL_FILE_CALL FILE_FREAD
file_readln_done:
      RTS

file_write_block:
      STA   LIB_ARG1
      STX   LIB_ARG1+1
      STZ   LIB_ARG1+2
      STZ   LIB_ARG1+3
      TSX
      CLC
      LDA   $0101,X
      ADC   #1
      STA   NVR2L
      LDA   $0102,X
      ADC   #0
      STA   NVR2H
      LDY   #0
      LDA   (NVR2L),Y
      STA   LIB_ARG2
      INY
      LDA   (NVR2L),Y
      STA   LIB_ARG2+1
      CLC
      LDA   $0101,X
      ADC   #2
      STA   $0101,X
      LDA   $0102,X
      ADC   #0
      STA   $0102,X
      BRA   file_write_common

file_write_char:
file_write_byte:
      STA   LIB_ARG3
      LDA   #<LIB_ARG3
      STA   LIB_ARG1
      LDA   #>LIB_ARG3
      STA   LIB_ARG1+1
      LDA   #1
      STA   LIB_ARG2
      STZ   LIB_ARG2+1
      BRA   file_write_common
file_write_word:
      STA   LIB_ARG3
      STX   LIB_ARG3+1
      LDA   #<LIB_ARG3
      STA   LIB_ARG1
      LDA   #>LIB_ARG3
      STA   LIB_ARG1+1
      LDA   #2
      STA   LIB_ARG2
      STZ   LIB_ARG2+1
      BRA   file_write_common
file_write_string:
      STA   NVR1L
      STX   NVR1H
      LDY   #0
      LDA   (NVR1L),Y
      STA   LIB_ARG2
      STZ   LIB_ARG2+1
      CLC
      LDA   NVR1L
      ADC   #1
      STA   LIB_ARG1
      LDA   NVR1H
      ADC   #0
      STA   LIB_ARG1+1
      BRA   file_write_common
file_write_real:
      STA   LIB_ARG1
      STX   LIB_ARG1+1
      LDA   #4
      STA   LIB_ARG2
      STZ   LIB_ARG2+1
      BRA   file_write_common
file_write_newline:
      LDA   #$0A
      STA   LIB_ARG3
      LDA   #<LIB_ARG3
      STA   LIB_ARG1
      LDA   #>LIB_ARG3
      STA   LIB_ARG1+1
      LDA   #1
      STA   LIB_ARG2
      STZ   LIB_ARG2+1
file_write_common:
      STZ   LIB_ARG1+2
      STZ   LIB_ARG1+3
      STZ   LIB_ARG2+2
      STZ   LIB_ARG2+3
      LDY   #0
      LDA   (NVR0L),Y
      STA   LIB_ARG0
      STZ   LIB_ARG0+1
      STZ   LIB_ARG0+2
      STZ   LIB_ARG0+3
      PASCAL_FILE_CALL FILE_FWRITE
      RTS
file_code_end:

file_symbols:
      .word file_assign-file_code
      .byte 0, NOBJ_SYM_GLOBAL, 6, "ASSIGN"
      .word file_reset-file_code
      .byte 0, NOBJ_SYM_GLOBAL, 5, "RESET"
      .word file_rewrite-file_code
      .byte 0, NOBJ_SYM_GLOBAL, 7, "REWRITE"
      .word file_close-file_code
      .byte 0, NOBJ_SYM_GLOBAL, 5, "CLOSE"
      .word file_eof-file_code
      .byte 0, NOBJ_SYM_GLOBAL, 3, "EOF"
      .word file_read-file_code
      .byte 0, NOBJ_SYM_GLOBAL, 4, "READ"
      .word file_readln-file_code
      .byte 0, NOBJ_SYM_GLOBAL, 6, "READLN"
      .word file_write_block-file_code
      .byte 0, NOBJ_SYM_GLOBAL, 20, "I_P_FILE_WRITE_BLOCK"
      .word file_write_char-file_code
      .byte 0, NOBJ_SYM_GLOBAL, 17, "P_FILE_WRITE_CHAR"
      .word file_write_byte-file_code
      .byte 0, NOBJ_SYM_GLOBAL, 17, "P_FILE_WRITE_BYTE"
      .word file_write_word-file_code
      .byte 0, NOBJ_SYM_GLOBAL, 17, "P_FILE_WRITE_WORD"
      .word file_write_string-file_code
      .byte 0, NOBJ_SYM_GLOBAL, 19, "P_FILE_WRITE_STRING"
      .word file_write_real-file_code
      .byte 0, NOBJ_SYM_GLOBAL, 17, "P_FILE_WRITE_REAL"
      .word file_write_newline-file_code
      .byte 0, NOBJ_SYM_GLOBAL, 20, "P_FILE_WRITE_NEWLINE"

      .word file_read_keyboard_target-file_code
      .byte 0, 0, 11, "FR_KEYBOARD"
      .word file_keyboard_string_target-file_code
      .byte 0, 0, 9, "FR_STRING"
      .word file_keyboard_char_target-file_code
      .byte 0, 0, 7, "FR_CHAR"
      .word file_readln_byte_target-file_code
      .byte 0, 0, 9, "FRLN_BYTE"

file_relocations:
      .byte 0, NOBJ_RELOC_ABS16
      .word file_read_jump_string_high+1-file_code, 15, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word file_read_jump_char+1-file_code, 16, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word file_read_jump_string_other+1-file_code, 15, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word file_readln_jump_keyboard+1-file_code, 14, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word file_readln_jump_byte+1-file_code, 17, 0
file_object_end:

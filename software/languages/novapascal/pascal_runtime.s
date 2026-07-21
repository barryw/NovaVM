; Pascal runtime archive generated from the Nova NDK hardware definitions.
; NL treats this as an ordinary NLIB and has no knowledge of these symbols.

      .setcpu "w65c02"
      .include "nova.inc"
      .include "nobj.inc"

      .segment "LIBRARY"
      .byte NLIB_MAGIC0, NLIB_MAGIC1, NLIB_MAGIC2, NLIB_MAGIC3
      .byte NLIB_VERSION, 7

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
      .word 2
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
byte_code_end:

byte_symbols:
      .word 0
      .byte 0, NOBJ_SYM_GLOBAL, 15, "P_WRITE_BYTE_LN"
      .word 9
      .byte 0, NOBJ_SYM_GLOBAL, 12, "P_WRITE_BYTE"

byte_relocations:
      .byte 0, NOBJ_RELOC_ABS16
      .word 1, 1, 0
byte_object_end:

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

      ; Byte arrays use A/X as a 16-bit index. The I_ helpers consume the
      ; following base-address word and, for stores, remove the saved index
      ; from beneath the return address.
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
      ADC   #1
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

      ; Word arithmetic and comparison materialization share one compact
      ; member. JSR and RTS preserve the incoming comparison flags.
      .word ordinal_object_end-ordinal_object
ordinal_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 1, $FF
      .word 0
      .word 9
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

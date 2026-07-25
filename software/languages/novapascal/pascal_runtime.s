; Pascal runtime archive generated from the Nova NDK hardware definitions.
; NL treats this as an ordinary NLIB and has no knowledge of these symbols.

      .setcpu "w65c02"
      .include "nova.inc"
      .include "libsystem.inc"
      .include "libfiles.inc"
      .include "libmemory.inc"
      .include "liblangrt.inc"
      .include "math_copro.inc"
      .include "nobj.inc"
.include "pascal.inc"

NP_WINDOW_ACTIVE = $A5

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
; Shared steps for the file-services member. See that member for why these are
; macros rather than subroutines.
.macro FILEX_ONE_ARG
      STA   NVR0L
      STX   NVR0H
.endmacro

.macro FILEX_HANDLE
      LDY   #0
      LDA   (NVR0L),Y
      STA   LIB_ARG0
      STZ   LIB_ARG0+1
      STZ   LIB_ARG0+2
      STZ   LIB_ARG0+3
.endmacro

.macro FILEX_NAME
      CLC
      LDA   NVR0L
      ADC   #3
      STA   LIB_ARG0
      LDA   NVR0H
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
.endmacro

.macro FILEX_CAPTURE_U24
      LDA   LIB_RESULT
      STA   NVR1L
      LDA   LIB_RESULT+1
      STA   NVR1H
      LDA   LIB_RESULT+2
      STA   NVR2L
.endmacro

.macro FILEX_U24_TO_ARG1
      LDA   NVR1L
      STA   LIB_ARG1
      LDA   NVR1H
      STA   LIB_ARG1+1
      LDA   NVR2L
      STA   LIB_ARG1+2
      STZ   LIB_ARG1+3
.endmacro

.macro FILEX_BLOCK_ARGS
      TSX
      LDA   $0107,X
      STA   NVR0L
      LDA   $0108,X
      STA   NVR0H
      LDA   $0105,X
      STA   LIB_ARG1
      LDA   $0106,X
      STA   LIB_ARG1+1
      STZ   LIB_ARG1+2
      STZ   LIB_ARG1+3
      LDA   $0103,X
      STA   LIB_ARG2
      LDA   $0104,X
      STA   LIB_ARG2+1
      STZ   LIB_ARG2+2
      STZ   LIB_ARG2+3
      FILEX_HANDLE
.endmacro

.macro PASCAL_LOAD_FILE_DATA_PTR
      LDA   LIB_ARG3
      STA   NVR6L
      LDA   LIB_ARG3+1
      STA   NVR6H
.endmacro

      .segment "LIBRARY"
      .byte NLIB_MAGIC0, NLIB_MAGIC1, NLIB_MAGIC2, NLIB_MAGIC3
        .byte NLIB_VERSION, 45

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
      .word 3
      .word line_symbols-line_object
      .word 4
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
      .byte $20
line_char_call = *
      .word 0                              ; JSR P_CHAR_DEVICE
      .byte $80,$E2
      .byte $A9,$0A                         ; LF
      .byte $20
line_newline_call = *
      .word 0                              ; JSR P_CHAR_DEVICE
      .byte $60
line_code_end:

line_symbols:
      .word 0
      .byte 0, NOBJ_SYM_GLOBAL, 14, "I_P_WRITE_LINE"
      .word $0015
      .byte 0, 0, 8, "PWL_LOAD"
      .word 0
      .byte NOBJ_SYM_UNDEFINED, NOBJ_SYM_GLOBAL, 13, "P_CHAR_DEVICE"

line_relocations:
      .byte 0, NOBJ_RELOC_ABS16
      .word $000D, 1, 1
      .byte 0, NOBJ_RELOC_ABS16
      .word $0013, 1, 2
      .byte 0, NOBJ_RELOC_ABS16
      .word line_char_call-line_code, 2, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word line_newline_call-line_code, 2, 0
line_object_end:

      ; Byte-sized decimal output. Keeping this in its own archive member means
      ; ordinary string-only programs never pay for it.
      .word byte_object_end-byte_object
byte_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 1, $FF
      .word 0
      .word 8
      .word byte_symbols-byte_object
      .word 9
      .word byte_relocations-byte_object
      .word 0

      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_EXEC, 0, 4, 0
      .word byte_code_end-byte_code
      .word byte_code_end-byte_code
      .byte "CODE"
byte_code:
      .byte $20,$00,$00                     ; JSR P_WRITE_BYTE
      .byte $A9,$0A,$20
write_byte_newline_call = *
      .word 0                                ; JSR P_CHAR_DEVICE
      .byte $60
      .byte $A0,$00,$A2,$30,$C9,$64,$90,$06,$38,$E9,$64,$E8,$80,$F6,$E0,$30
      .byte $F0,$07,$48,$8A,$20
write_byte_hundreds_call = *
      .word 0                                ; JSR P_CHAR_DEVICE
      .byte $68,$C8,$A2,$30,$C9,$0A,$90,$06,$38
      .byte $E9,$0A,$E8,$80,$F6,$C0,$00,$D0,$04,$E0,$30,$F0,$06,$48,$8A,$20
write_byte_tens_call = *
      .word 0                                ; JSR P_CHAR_DEVICE
      .byte $68,$18,$69,$30,$20
write_byte_ones_call = *
      .word 0                                ; JSR P_CHAR_DEVICE
      .byte $60

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
      BPL   write_word_magnitude
      INC   NVR2H
      SEC
      LDA   #0
      SBC   NVR0L
      STA   NVR0L
      LDA   #0
      SBC   NVR0H
      STA   NVR0H
write_word_magnitude:
      STZ   NVR1L
write_word_divide:
      LDA   #0
      LDX   #16
write_word_bit:
      ASL   NVR0L
      ROL   NVR0H
      ROL
      CMP   #10
      BCC   :+
      SBC   #10
      INC   NVR0L
:     DEX
      BNE   write_word_bit
      CLC
      ADC   #'0'
      PHA
      INC   NVR1L
      LDA   NVR0L
      ORA   NVR0H
      BNE   write_word_divide
      CLC
      LDA   NVR1L
      ADC   NVR2H
      STA   NVR3L
      LDA   NVR2L
      CMP   NVR3L
      BCC   write_word_sign
      BEQ   write_word_sign
      SEC
      SBC   NVR3L
      TAX
      LDA   #' '
write_word_padding:
      .byte $20
write_word_padding_call = *
      .word 0                                ; JSR P_CHAR_DEVICE
      DEX
      BNE   write_word_padding
write_word_sign:
      LDA   NVR2H
      BEQ   write_word_digits
      LDA   #'-'
      .byte $20
write_word_sign_call = *
      .word 0                                ; JSR P_CHAR_DEVICE
write_word_digits:
      PLA
      .byte $20
write_word_digit_call = *
      .word 0                                ; JSR P_CHAR_DEVICE
      DEC   NVR1L
      BNE   write_word_digits
      RTS

write_uword:
      STZ   NVR2L                         ; no minimum field width
      STA   NVR0L
      STX   NVR0H
      STZ   NVR2H
      BRA   write_word_magnitude

write_uword_field:
      TSX
      LDA   $0103,X
      STA   NVR2L
      LDA   $0104,X
      STA   NVR0L
      LDA   $0106,X
      STA   NVR0H
      STZ   NVR2H
      BRA   write_word_magnitude

write_string:
      STA   NVR0L
      STX   NVR0H
      LDY   #0
      LDA   (NVR0L),Y
      TAX
      BEQ   write_string_done
write_string_character:
      INY
      LDA   (NVR0L),Y
      .byte $20
write_string_char_call = *
      .word 0                                ; JSR P_CHAR_DEVICE
      DEX
      BNE   write_string_character
write_string_done:
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
      .word write_uword-byte_code
      .byte 0, NOBJ_SYM_GLOBAL, 13, "P_WRITE_UWORD"
      .word write_uword_field-byte_code
      .byte 0, NOBJ_SYM_GLOBAL, 19, "P_WRITE_UWORD_FIELD"
      .word 0
      .byte NOBJ_SYM_UNDEFINED, NOBJ_SYM_GLOBAL, 13, "P_CHAR_DEVICE"

byte_relocations:
      .byte 0, NOBJ_RELOC_ABS16
      .word 1, 1, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word write_byte_newline_call-byte_code, 7, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word write_byte_hundreds_call-byte_code, 7, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word write_byte_tens_call-byte_code, 7, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word write_byte_ones_call-byte_code, 7, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word write_word_padding_call-byte_code, 7, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word write_word_sign_call-byte_code, 7, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word write_word_digit_call-byte_code, 7, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word write_string_char_call-byte_code, 7, 0
byte_object_end:

      ; STR(Integer) shares the native signed-word representation but writes
      ; into a Turbo short string instead of a device.
      ; Random-access, bulk-transfer, and file-management operations over the
      ; byte-stream FILES handles. Positions and counts are byte offsets in a
      ; 16-bit range rather than Turbo's record counts: Nova's FIO layer is
      ; byte-addressed, and a Pascal-held file cannot exceed the 64 KB address
      ; space anyway. Scale by SizeOf(record) for Turbo-style record addressing.
      ;
      ; The shared steps are assembler macros rather than internal subroutines:
      ; a NOBJ member can only relocate calls that go through its own exported
      ; symbols, so private helpers would need to be published just to be
      ; callable. Inlining keeps the member self-contained.
      .word filex_object_end-filex_object
filex_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 1, $FF
      .word 0
      .word 11
      .word filex_symbols-filex_object
      .word 0
      .word filex_relocations-filex_object
      .word 0

      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_EXEC, 0, 4, 0
      .word filex_code_end-filex_code
      .word filex_code_end-filex_code
      .byte "CODE"
filex_code:
filex_seek:
      TSX
      LDA   $0105,X
      STA   NVR0L
      LDA   $0106,X
      STA   NVR0H
      LDA   $0103,X
      STA   NVR1L
      LDA   $0104,X
      STA   NVR1H
      STZ   NVR2L
      FILEX_HANDLE
      FILEX_U24_TO_ARG1
      PASCAL_FILE_CALL FILE_FSEEK
      RTS

filex_filepos:
      FILEX_ONE_ARG
      FILEX_HANDLE
      PASCAL_FILE_CALL FILE_FTELL
      LDA   LIB_RESULT
      LDX   LIB_RESULT+1
      RTS

filex_filesize:
      FILEX_ONE_ARG
      FILEX_HANDLE
      PASCAL_FILE_CALL FILE_FSIZE
      LDA   LIB_RESULT
      LDX   LIB_RESULT+1
      RTS

; Truncate cuts the file at the current position.
filex_truncate:
      FILEX_ONE_ARG
      FILEX_HANDLE
      PASCAL_FILE_CALL FILE_FTELL
      FILEX_CAPTURE_U24
      FILEX_HANDLE
      FILEX_U24_TO_ARG1
      PASCAL_FILE_CALL FILE_FRESIZE
      RTS

filex_flush:
      FILEX_ONE_ARG
      FILEX_HANDLE
      PASCAL_FILE_CALL FILE_FFLUSH
      RTS

filex_erase:
      FILEX_ONE_ARG
      FILEX_NAME
      PASCAL_FILE_CALL FILE_FDELETE
      RTS

; Append reopens for read/write and parks the cursor at end of file.
filex_append:
      FILEX_ONE_ARG
      FILEX_NAME
      LDA   #FIO_FILE_ACCESS_RW
      STA   LIB_ARG2
      STZ   LIB_ARG2+1
      STZ   LIB_ARG2+2
      STZ   LIB_ARG2+3
      PASCAL_FILE_CALL FILE_FOPEN
      LDY   #0
      LDA   LIB_RESULT
      STA   (NVR0L),Y
      INY
      LDA   #1
      STA   (NVR0L),Y
      FILEX_HANDLE
      PASCAL_FILE_CALL FILE_FSIZE
      FILEX_CAPTURE_U24
      FILEX_HANDLE
      FILEX_U24_TO_ARG1
      PASCAL_FILE_CALL FILE_FSEEK
      RTS

; Rename also adopts the new name, so a later Erase or Append follows the file.
filex_rename:
      TSX
      LDA   $0105,X
      STA   NVR0L
      LDA   $0106,X
      STA   NVR0H
      LDA   $0103,X
      STA   NVR3L
      LDA   $0104,X
      STA   NVR3H
      FILEX_NAME
      CLC
      LDA   NVR3L
      ADC   #1
      STA   LIB_ARG2
      LDA   NVR3H
      ADC   #0
      STA   LIB_ARG2+1
      STZ   LIB_ARG2+2
      STZ   LIB_ARG2+3
      LDY   #0
      LDA   (NVR3L),Y
      STA   LIB_ARG3
      STZ   LIB_ARG3+1
      STZ   LIB_ARG3+2
      STZ   LIB_ARG3+3
      PASCAL_FILE_CALL FILE_FRENAME
      LDY   #0
      LDA   (NVR3L),Y
      CMP   #14
      BCC   :+
      LDA   #13
:     TAX
      LDY   #2
      STA   (NVR0L),Y
      TXA
      BEQ   @renamed
      LDY   #1
@copy:
      LDA   (NVR3L),Y
      PHY
      INY
      INY
      STA   (NVR0L),Y
      PLY
      INY
      DEX
      BNE   @copy
@renamed:
      RTS

filex_blockread:
      FILEX_BLOCK_ARGS
      PASCAL_FILE_CALL FILE_FREAD
      RTS

filex_blockwrite:
      FILEX_BLOCK_ARGS
      PASCAL_FILE_CALL FILE_FWRITE
      RTS

; Eoln peeks one byte and restores the cursor, so the caller's position is
; unchanged whether or not a line ending is next.
filex_eoln:
      FILEX_ONE_ARG
      FILEX_HANDLE
      PASCAL_FILE_CALL FILE_FTELL
      FILEX_CAPTURE_U24
      FILEX_HANDLE
      LDA   #<LIB_ARG3
      STA   LIB_ARG1
      LDA   #>LIB_ARG3
      STA   LIB_ARG1+1
      STZ   LIB_ARG1+2
      STZ   LIB_ARG1+3
      LDA   #1
      STA   LIB_ARG2
      STZ   LIB_ARG2+1
      STZ   LIB_ARG2+2
      STZ   LIB_ARG2+3
      STZ   LIB_ARG3
      PASCAL_FILE_CALL FILE_FREAD
      LDA   LIB_RESULT
      ORA   LIB_RESULT+1
      BEQ   @line_end                     ; nothing left: end of file is eoln
      LDA   LIB_ARG3
      CMP   #$0D
      BEQ   @restore_end
      CMP   #$0A
      BEQ   @restore_end
      LDX   #0
      BRA   @restore
@restore_end:
      LDX   #1
@restore:
      PHX
      FILEX_HANDLE
      FILEX_U24_TO_ARG1
      PASCAL_FILE_CALL FILE_FSEEK
      PLA
      RTS
@line_end:
      LDA   #1
      RTS
filex_code_end:

filex_symbols:
      .word filex_seek-filex_code
      .byte 0, NOBJ_SYM_GLOBAL, 4, "SEEK"
      .word filex_filepos-filex_code
      .byte 0, NOBJ_SYM_GLOBAL, 7, "FILEPOS"
      .word filex_filesize-filex_code
      .byte 0, NOBJ_SYM_GLOBAL, 8, "FILESIZE"
      .word filex_truncate-filex_code
      .byte 0, NOBJ_SYM_GLOBAL, 8, "TRUNCATE"
      .word filex_flush-filex_code
      .byte 0, NOBJ_SYM_GLOBAL, 5, "FLUSH"
      .word filex_erase-filex_code
      .byte 0, NOBJ_SYM_GLOBAL, 5, "ERASE"
      .word filex_rename-filex_code
      .byte 0, NOBJ_SYM_GLOBAL, 6, "RENAME"
      .word filex_append-filex_code
      .byte 0, NOBJ_SYM_GLOBAL, 6, "APPEND"
      .word filex_blockread-filex_code
      .byte 0, NOBJ_SYM_GLOBAL, 9, "BLOCKREAD"
      .word filex_blockwrite-filex_code
      .byte 0, NOBJ_SYM_GLOBAL, 10, "BLOCKWRITE"
      .word filex_eoln-filex_code
      .byte 0, NOBJ_SYM_GLOBAL, 4, "EOLN"
filex_relocations:
filex_object_end:

      ; Turbo Random/Randomize. Nova's RNG is host- or hardware-backed entropy
      ; rather than a seeded sequence, so Randomize has no seed to install and
      ; RandSeed has no meaning here; it exists so ported sources still compile.
      .word random_object_end-random_object
random_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 1, $FF
      .word 0
      .word 5
      .word random_symbols-random_object
      .word 0
      .word random_relocations-random_object
      .word 0

      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_EXEC, 0, 4, 0
      .word random_code_end-random_code
      .word random_code_end-random_code
      .byte "CODE"
random_code:
; Random(N) scales 16 random bits into 0..N-1 with the hardware multiplier,
; which stays even across the range instead of carrying a modulo bias.
pascal_random:
      STA   NVR0L
      STX   NVR0H
      ORA   NVR0H
      BNE   :+
      TAX                                 ; Random(0) is 0, as in Turbo
      RTS
:     PASCAL_FILE_CALL FILE_RNG
      MATHC_MUL16_U LIB_RESULT, LIB_RESULT+1, NVR0L, NVR0H, NVR1L, NVR1H, NVR2L, NVR2H
      LDA   NVR2L
      LDX   NVR2H
      RTS

pascal_randomize:
      RTS

; Turbo byte helpers share this member: NL does not cope with another archive
; member here, and these are the same shape of small System scalar helper.
pascal_hi:
      TXA
      RTS
pascal_lo:
      RTS
pascal_swap:
      PHA
      TXA
      PLX
      RTS
random_code_end:

random_symbols:
      .word pascal_random-random_code
      .byte 0, NOBJ_SYM_GLOBAL, 6, "RANDOM"
      .word pascal_randomize-random_code
      .byte 0, NOBJ_SYM_GLOBAL, 9, "RANDOMIZE"
      .word pascal_hi-random_code
      .byte 0, NOBJ_SYM_GLOBAL, 2, "HI"
      .word pascal_lo-random_code
      .byte 0, NOBJ_SYM_GLOBAL, 2, "LO"
      .word pascal_swap-random_code
      .byte 0, NOBJ_SYM_GLOBAL, 4, "SWAP"
random_relocations:
random_object_end:

      .word str_integer_object_end-str_integer_object
str_integer_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 1, $FF
      .word 0
      .word 2
      .word str_integer_symbols-str_integer_object
      .word 0
      .word str_integer_relocations-str_integer_object
      .word 0

      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_EXEC, 0, 4, 0
      .word str_integer_code_end-str_integer_code
      .word str_integer_code_end-str_integer_code
      .byte "CODE"
str_integer_code:
str_integer:
      STA   NVR4L
      STX   NVR4H
      TSX
      LDA   $0103,X
      STA   NVR2L                         ; minimum field width
      LDA   $0105,X                       ; signed value low byte
      STA   NVR0L
      LDA   $0106,X
      STA   NVR0H
      STZ   NVR2H                         ; sign character count
      LDA   NVR0H
      BPL   str_integer_magnitude
      INC   NVR2H
      SEC
      LDA   #0
      SBC   NVR0L
      STA   NVR0L
      LDA   #0
      SBC   NVR0H
      STA   NVR0H
str_integer_magnitude:
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
      BCC   @no_padding
      BEQ   @no_padding
      SEC
      SBC   NVR3L
      STA   NVR3H
      LDA   NVR2L
      BRA   @length
@no_padding:
      STZ   NVR3H
      LDA   NVR3L
@length:
      LDY   #0
      STA   (NVR4L),Y
@padding:
      LDA   NVR3H
      BEQ   @sign
      INY
      LDA   #' '
      STA   (NVR4L),Y
      DEC   NVR3H
      BRA   @padding
@sign:
      LDA   NVR2H
      BEQ   @digits
      INY
      LDA   #'-'
      STA   (NVR4L),Y
@digits:
      PLA
      INY
      STA   (NVR4L),Y
      DEC   NVR1L
      BNE   @digits
      RTS

str_uword:
      STA   NVR4L
      STX   NVR4H
      TSX
      LDA   $0103,X
      STA   NVR2L
      LDA   $0105,X
      STA   NVR0L
      LDA   $0106,X
      STA   NVR0H
      STZ   NVR2H
      BRA   str_integer_magnitude
str_integer_code_end:

str_integer_symbols:
      .word str_integer-str_integer_code
      .byte 0, NOBJ_SYM_GLOBAL, 13, "P_STR_INTEGER"
      .word str_uword-str_integer_code
      .byte 0, NOBJ_SYM_GLOBAL, 11, "P_STR_UWORD"
str_integer_relocations:
str_integer_object_end:

      ; Formatted Real output depends on the paged numeric runtime. Keeping it
      ; separate prevents byte-only programs from inheriting those objects.
      .word format_object_end-format_object
format_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 1, $FF
      .word 0
      .word 10
      .word format_symbols-format_object
      .word 17
      .word format_relocations-format_object
      .word 0

      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_EXEC, 0, 4, 0
      .word format_code_end-format_code
      .word format_code_end-format_code
      .byte "CODE"
format_code:
; A selects signed word (0), Real (1/2), or unsigned word (3).
write_format:
      CMP   #3
      BEQ   write_format_uword
      CMP   #1
      BCC   write_format_word
      BEQ   write_real_field
      BRA   write_real_field_precision
write_format_uword:
      .byte $4C
write_format_uword_jump: .word 0
write_format_word:
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

; LongInt shares the Real shape: format through the paged runtime into the
; shared string buffer, then hand that string to the ordinary writer.
write_long:
      PHX
      PHA
      LDA   #0
      PHA                                 ; unused width high
      PHA                                 ; zero field width
      .byte $A9
write_long_buffer_lo: .byte 0
      .byte $A2
write_long_buffer_hi: .byte 0
      .byte $20
write_long_call: .word 0
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
      .word 0
      .byte NOBJ_SYM_UNDEFINED, NOBJ_SYM_GLOBAL, 19, "P_WRITE_UWORD_FIELD"
      .word write_long-format_code
      .byte 0, NOBJ_SYM_GLOBAL, 12, "P_WRITE_LONG"
      .word 0
      .byte NOBJ_SYM_UNDEFINED, NOBJ_SYM_GLOBAL, 10, "P_STR_LONG"

format_relocations:
      .byte 0, NOBJ_RELOC_ABS16
      .word write_format_word_jump-format_code, 2, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word write_format_uword_jump-format_code, 7, 0
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
      .byte 0, NOBJ_RELOC_LO8
      .word write_long_buffer_lo-format_code, 6, 0
      .byte 0, NOBJ_RELOC_HI8
      .word write_long_buffer_hi-format_code, 6, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word write_long_call-format_code, 9, 0
format_object_end:

      .word device_object_end-device_object
device_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 2, $FF
      .word 0
      .word 2
      .word device_symbols-device_object
      .word 2
      .word device_relocations-device_object
      .word 0

      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_WRITE | NOBJ_SEC_ZEROFILL, 0, 3, 0
      .word 7
      .word 0
      .byte "BSS"

      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_EXEC, 0, 4, 0
      .word char_device_code_end-char_device_code
      .word char_device_code_end-char_device_code
      .byte "CODE"
char_device_code:
char_device:
      PHP
      PHA
      .byte $AD
char_device_active: .word 0
      CMP   #NP_WINDOW_ACTIVE
      BEQ   char_device_window
      PLA
      PLP
      STA   VGC_CHAROUT
      RTS
char_device_window:
      PLA
      PLP
      .byte $6C                           ; JMP (__NP_WINDOW+5)
char_device_vector: .word 0
char_device_code_end:

device_symbols:
      .word 0
      .byte 0, NOBJ_SYM_GLOBAL, 11, "__NP_WINDOW"
      .word char_device-char_device_code
      .byte 1, NOBJ_SYM_GLOBAL, 13, "P_CHAR_DEVICE"

device_relocations:
      .byte 1, NOBJ_RELOC_ABS16
      .word char_device_active-char_device_code, 0, 4
      .byte 1, NOBJ_RELOC_ABS16
      .word char_device_vector-char_device_code, 0, 5
device_object_end:

      ; Window's bounded writer is a separate archive member. Programs that do
      ; not reference WINDOW keep the direct character device above.
      .word window_object_end-window_object
window_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 1, $FF
      .word 0
      .word 14
      .word window_symbols-window_object
      .word 23
      .word window_relocations-window_object
      .word 0

      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_EXEC, 0, 4, 0
      .word device_code_end-device_code
      .word device_code_end-device_code
      .byte "CODE"
device_code:
device_window_pointer:
      .byte $A9
device_window_low: .byte 0
      STA   NVR6L
      .byte $A9
device_window_high: .byte 0
      STA   NVR6H
      RTS

device_stage_window:
      LDY   #0
@copy:
      LDA   (NVR6L),Y
      STA   LIB_ARG0,Y
      INY
      CPY   #4
      BNE   @copy
      RTS

; SYSTEM owns bounded VTEXT operations. Preserve the pseudo-registers used by
; numeric/string emitters; this slow path runs only for clear or window scroll.
device_system_call:
      STA   NVR7H
      LDX   #11
@save:
      LDA   NVR0L,X
      PHA
      DEX
      BPL   @save
      LDA   #MODULE_ID_SYSTEM
      STA   LIB_MOD_ID
      LDA   NVR7H
      STA   LIB_FN_ID
      JSR   LIB_LOADER_BAND
      LDX   #0
@restore:
      PLA
      STA   NVR0L,X
      INX
      CPX   #12
      BNE   @restore
      BRA   device_window_pointer

device_home:
      LDY   #0
      LDA   (NVR6L),Y
      STA   VGC_CURSX
      INY
      LDA   (NVR6L),Y
      STA   VGC_CURSY
      RTS

device_scroll:
      ; Delete the window's top row, then park at its bottom-left cell.
      .byte $20
window_scroll_home_call: .word 0
      .byte $20
window_scroll_stage_call: .word 0
      LDA   #SYS_CONSOLE_DELETE_LINE
      .byte $20
window_scroll_system_call: .word 0
      LDY   #0
      LDA   (NVR6L),Y
      STA   VGC_CURSX
      INY
      LDA   (NVR6L),Y
      INY
      INY
      CLC
      ADC   (NVR6L),Y
      DEC
      STA   VGC_CURSY
      RTS

device_advance_row:
      INC   NVR7L
      LDY   #1
      LDA   (NVR6L),Y
      INY
      INY
      CLC
      ADC   (NVR6L),Y
      CMP   NVR7L
      BEQ   device_scroll
      LDA   NVR7L
      STA   VGC_CURSY
      RTS

device_clear:
      .byte $20
window_clear_stage_call: .word 0
      LDA   #SYS_CONSOLE_CLEAR_REGION
      .byte $20
window_clear_system_call: .word 0
      BRA   device_home

; Window-aware character device. Only a bottom-edge scroll pages SYSTEM and
; invokes VTEXT; ordinary cells still use direct VGC MMIO.
window_char:
      PHP
      PHA
      PHX
      PHY
      LDA   NVR6L
      PHA
      LDA   NVR6H
      PHA
      LDA   NVR7L
      PHA
      LDA   NVR7H
      PHA
      .byte $20
window_char_pointer_call: .word 0
      TSX
      LDA   $0107,X                       ; original character
      CMP   #$20
      BCS   window_char_printable
      CMP   #$08
      BNE   :+
      BRA   window_char_backspace
:
      CMP   #$0A
      BNE   :+
      .byte $4C
window_char_newline_jump: .word 0
:
      CMP   #$0C
      BNE   :+
      .byte $4C
window_char_clear_jump: .word 0
: 
      CMP   #$0D
      BNE   :+
      .byte $4C
window_char_carriage_jump: .word 0
:
      CMP   #$13
      BNE   :+
      .byte $4C
window_char_home_jump: .word 0
:
      STA   VGC_CHAROUT
      .byte $4C
window_char_done_jump: .word 0
window_char_printable:
      LDY   #0
      LDA   (NVR6L),Y
      INY
      INY
      CLC
      ADC   (NVR6L),Y
      DEC
      CMP   VGC_CURSX
      BNE   window_char_write
      CMP   #NOVA_SCREEN_COLS-1
      BNE   window_char_right_edge
      LDA   VGC_CURSY
      CMP   #NOVA_SCREEN_ROWS-1
      BNE   window_char_right_edge
      TSX
      LDA   $0107,X
      STA   LIB_ARG1
      STZ   LIB_ARG1+1
      STZ   LIB_ARG1+2
      STZ   LIB_ARG1+3
      .byte $20
window_char_stage_call: .word 0
      LDA   #SYS_CONSOLE_PUT_CHAR
      .byte $20
window_char_system_call: .word 0
      BRA   window_char_done
window_char_right_edge:
      LDA   VGC_CURSY
      STA   NVR7L
      TSX
      LDA   $0107,X
      STA   VGC_CHAROUT
      LDY   #0
      LDA   (NVR6L),Y
      STA   VGC_CURSX
      .byte $20
window_char_advance_right_call: .word 0
      BRA   window_char_done
window_char_write:
      TSX
      LDA   $0107,X
      STA   VGC_CHAROUT
      BRA   window_char_done
window_char_backspace:
      LDY   #0
      LDA   VGC_CURSX
      CMP   (NVR6L),Y
      BEQ   window_char_done
      LDA   #$08
      STA   VGC_CHAROUT
      BRA   window_char_done
window_char_newline:
      LDA   VGC_CURSY
      STA   NVR7L
      LDY   #0
      LDA   (NVR6L),Y
      STA   VGC_CURSX
      .byte $20
window_char_advance_line_call: .word 0
      BRA   window_char_done
window_char_clear:
      .byte $20
window_char_clear_call: .word 0
      BRA   window_char_done
window_char_carriage_return:
      LDY   #0
      LDA   (NVR6L),Y
      STA   VGC_CURSX
      BRA   window_char_done
window_char_home:
      .byte $20
window_char_home_call: .word 0
window_char_done:
      PLA
      STA   NVR7H
      PLA
      STA   NVR7L
      PLA
      STA   NVR6H
      PLA
      STA   NVR6L
      PLY
      PLX
      PLA
      PLP
      RTS

; Turbo Window uses absolute, one-based corners and homes the relative cursor.
; Invalid rectangles leave both the current window and cursor unchanged.
window_set:
      TSX
      LDA   $0106,X                       ; X1
      BEQ   window_set_done
      CMP   #NOVA_SCREEN_COLS+1
      BCS   window_set_done
      STA   NVR0L
      LDA   $0105,X                       ; Y1
      BEQ   window_set_done
      CMP   #NOVA_SCREEN_ROWS+1
      BCS   window_set_done
      STA   NVR0H
      LDA   $0104,X                       ; X2
      CMP   NVR0L
      BCC   window_set_done
      CMP   #NOVA_SCREEN_COLS+1
      BCS   window_set_done
      STA   NVR1L
      LDA   $0103,X                       ; Y2
      CMP   NVR0H
      BCC   window_set_done
      CMP   #NOVA_SCREEN_ROWS+1
      BCS   window_set_done
      STA   NVR1H
      LDA   #NP_WINDOW_ACTIVE
      STA   NVR2L
      LDA   NVR0L
      CMP   #1
      BNE   @active_ready
      LDA   NVR0H
      CMP   #1
      BNE   @active_ready
      LDA   NVR1L
      CMP   #NOVA_SCREEN_COLS
      BNE   @active_ready
      LDA   NVR1H
      CMP   #NOVA_SCREEN_ROWS
      BNE   @active_ready
      STZ   NVR2L
@active_ready:
      .byte $20
window_set_pointer_call: .word 0
      LDY   #0
      LDA   NVR0L
      DEC
      STA   (NVR6L),Y
      INY
      LDA   NVR0H
      DEC
      STA   (NVR6L),Y
      INY
      SEC
      LDA   NVR1L
      SBC   NVR0L
      INC
      STA   (NVR6L),Y
      INY
      SEC
      LDA   NVR1H
      SBC   NVR0H
      INC
      STA   (NVR6L),Y
      INY
      LDA   NVR2L
      STA   (NVR6L),Y
      INY
      .byte $A9
window_vector_low: .byte 0
      STA   (NVR6L),Y
      INY
      .byte $A9
window_vector_high: .byte 0
      STA   (NVR6L),Y
      .byte $4C
window_set_home_jump: .word 0
window_set_done:
      RTS
device_code_end:

window_symbols:
      .word window_set-device_code
      .byte 0, NOBJ_SYM_GLOBAL, 6, "WINDOW"
      .word window_char-device_code
      .byte 0, 0, 11, "WINDOW_CHAR"
      .word device_window_pointer-device_code
      .byte 0, 0, 3, "DWP"
      .word device_stage_window-device_code
      .byte 0, 0, 6, "DSTAGE"
      .word device_system_call-device_code
      .byte 0, 0, 5, "DSYSC"
      .word device_home-device_code
      .byte 0, 0, 5, "DHOME"
      .word device_advance_row-device_code
      .byte 0, 0, 4, "DADV"
      .word device_clear-device_code
      .byte 0, 0, 6, "DCLEAR"
      .word window_char_clear-device_code
      .byte 0, 0, 6, "WCLEAR"
      .word window_char_carriage_return-device_code
      .byte 0, 0, 3, "WCR"
      .word window_char_home-device_code
      .byte 0, 0, 5, "WHOME"
      .word window_char_done-device_code
      .byte 0, 0, 5, "WDONE"
      .word window_char_newline-device_code
      .byte 0, 0, 3, "WNL"
      .word 0
      .byte NOBJ_SYM_UNDEFINED, NOBJ_SYM_GLOBAL, 11, "__NP_WINDOW"

window_relocations:
      .byte 0, NOBJ_RELOC_LO8
      .word device_window_low-device_code, 13, 0
      .byte 0, NOBJ_RELOC_HI8
      .word device_window_high-device_code, 13, 0
      .byte 0, NOBJ_RELOC_LO8
      .word window_vector_low-device_code, 1, 0
      .byte 0, NOBJ_RELOC_HI8
      .word window_vector_high-device_code, 1, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word window_scroll_home_call-device_code, 5, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word window_scroll_stage_call-device_code, 3, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word window_scroll_system_call-device_code, 4, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word window_clear_stage_call-device_code, 3, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word window_clear_system_call-device_code, 4, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word window_char_pointer_call-device_code, 2, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word window_char_stage_call-device_code, 3, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word window_char_system_call-device_code, 4, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word window_char_advance_right_call-device_code, 6, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word window_char_advance_line_call-device_code, 6, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word window_char_clear_call-device_code, 7, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word window_char_home_call-device_code, 5, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word window_set_pointer_call-device_code, 2, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word window_set_home_jump-device_code, 5, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word window_char_clear_jump-device_code, 8, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word window_char_carriage_jump-device_code, 9, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word window_char_home_jump-device_code, 10, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word window_char_done_jump-device_code, 11, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word window_char_newline_jump-device_code, 12, 0
window_object_end:

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
      .word 3
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
      LDA   NVR5L
      ORA   NVR5H
      BEQ   @done
      LDA   NVR2L
      ORA   NVR2H
      BEQ   @fill
      LDY   #0
@copy:
      LDA   (NVR2L),Y
      STA   (NVR0L),Y
      INC   NVR2L
      BNE   :+
      INC   NVR2H
:     INC   NVR0L
      BNE   :+
      INC   NVR0H
:     LDA   NVR5L
      BNE   :+
      DEC   NVR5H
:     DEC   NVR5L
      LDA   NVR5L
      ORA   NVR5H
      BNE   @copy
@done:
      RTS
@fill:
      LDY   #0
@fill_zero:
      LDA   #0
      STA   (NVR0L),Y
      INC   NVR0L
      BNE   :+
      INC   NVR0H
:     LDA   NVR5L
      BNE   :+
      DEC   NVR5H
:     DEC   NVR5L
      LDA   NVR5L
      ORA   NVR5H
      BNE   @fill_zero
      RTS

move:
      TSX
      LDA   $0107,X
      STA   NVR2L
      LDA   $0108,X
      STA   NVR2H
      LDA   $0105,X
      STA   NVR0L
      LDA   $0106,X
      STA   NVR0H
      LDA   $0103,X
      STA   NVR5L
      LDA   $0104,X
      STA   NVR5H
      LDA   NVR5L
      ORA   NVR5H
      BNE   :+
      RTS
:
      LDA   NVR0H
      CMP   NVR2H
      BCC   @forward
      BNE   @backward
      LDA   NVR0L
      CMP   NVR2L
      BCC   @forward
      BNE   @backward
      RTS
@forward:
      LDY   #0
@forward_copy:
      LDA   (NVR2L),Y
      STA   (NVR0L),Y
      INC   NVR2L
      BNE   :+
      INC   NVR2H
:     INC   NVR0L
      BNE   :+
      INC   NVR0H
:     LDA   NVR5L
      BNE   :+
      DEC   NVR5H
:     DEC   NVR5L
      LDA   NVR5L
      ORA   NVR5H
      BNE   @forward_copy
      RTS
@backward:
      SEC
      LDA   NVR5L
      SBC   #1
      STA   NVR3L
      LDA   NVR5H
      SBC   #0
      STA   NVR3H
      CLC
      LDA   NVR2L
      ADC   NVR3L
      STA   NVR2L
      LDA   NVR2H
      ADC   NVR3H
      STA   NVR2H
      CLC
      LDA   NVR0L
      ADC   NVR3L
      STA   NVR0L
      LDA   NVR0H
      ADC   NVR3H
      STA   NVR0H
      LDY   #0
@backward_copy:
      LDA   (NVR2L),Y
      STA   (NVR0L),Y
      LDA   NVR2L
      BNE   :+
      DEC   NVR2H
:     DEC   NVR2L
      LDA   NVR0L
      BNE   :+
      DEC   NVR0H
:     DEC   NVR0L
      LDA   NVR5L
      BNE   :+
      DEC   NVR5H
:     DEC   NVR5L
      LDA   NVR5L
      ORA   NVR5H
      BNE   @backward_copy
      RTS

fill_char:
      TSX
      LDA   $0107,X
      STA   NVR0L
      LDA   $0108,X
      STA   NVR0H
      LDA   $0105,X
      STA   NVR5L
      LDA   $0106,X
      STA   NVR5H
      LDA   $0103,X
      STA   NVR3L
      LDA   NVR5L
      ORA   NVR5H
      BEQ   @done
      LDY   #0
@fill:
      LDA   NVR3L
      STA   (NVR0L),Y
      INC   NVR0L
      BNE   :+
      INC   NVR0H
:     LDA   NVR5L
      BNE   :+
      DEC   NVR5H
:     DEC   NVR5L
      LDA   NVR5L
      ORA   NVR5H
      BNE   @fill
@done:
      RTS
block_code_end:

block_symbols:
      .word block_copy-block_code
      .byte 0, NOBJ_SYM_GLOBAL, 14, "I_P_BLOCK_COPY"
      .word move-block_code
      .byte 0, NOBJ_SYM_GLOBAL, 4, "MOVE"
      .word fill_char-block_code
      .byte 0, NOBJ_SYM_GLOBAL, 8, "FILLCHAR"
block_object_end:

      ; Signed word comparison is isolated so NL retains its 18 bytes only
      ; when O2 selects it. A/X is the left operand, NVR0 is the right; the
      ; returned flags describe the signed left-minus-right relation.
      .word signed_compare_object_end-signed_compare_object
signed_compare_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 1, $FF
      .word 0
      .word 1
      .word signed_compare_symbols-signed_compare_object
      .word 0
      .word signed_compare_object_end-signed_compare_object
      .word 0

      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_EXEC, 0, 4, 0
      .word signed_compare_code_end-signed_compare_code
      .word signed_compare_code_end-signed_compare_code
      .byte "CODE"
signed_compare_code:
      TAY
      LDA   NVR0H
      EOR   #$80
      STA   NVR1H
      TXA
      EOR   #$80
      CMP   NVR1H
      BNE   signed_compare_done
      TYA
      CMP   NVR0L
signed_compare_done:
      RTS
signed_compare_code_end:

signed_compare_symbols:
      .word signed_compare_code-signed_compare_code
      .byte 0, NOBJ_SYM_GLOBAL, 7, "P_CMPSW"
signed_compare_object_end:

      ; Word arithmetic and comparison materialization share one compact
      ; member. JSR and RTS preserve the incoming comparison flags.
      .word ordinal_object_end-ordinal_object
ordinal_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 1, $FF
      .word 0
      .word 14
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
      LDY   #0
      BRA   ordinal_divide_common
ordinal_modulo:
      LDY   #1
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
      TYA
      BNE   @modulo
      LDA   MATH_RES0
      LDX   MATH_RES1
      RTS
@modulo:
      LDA   MATH_RES2
      LDX   MATH_RES3
      RTS
ordinal_divide_unsigned:
      LDY   #0
      BRA   ordinal_divide_unsigned_common
ordinal_modulo_unsigned:
      LDY   #1
ordinal_divide_unsigned_common:
      STA   NVR1L
      STX   NVR1H
      LDA   NVR0L
      ORA   NVR0H
      BNE   :+
      STP
:     LDA   NVR0H
      BMI   @large_divisor
      LDA   NVR1L
      STA   MATH_DIV_N_LO
      LDA   NVR1H
      STA   MATH_DIV_N_1
      STZ   MATH_DIV_N_2
      STZ   MATH_DIV_N_HI
      LDA   NVR0L
      STA   MATH_DIV_D_LO
      LDA   NVR0H
      STA   MATH_DIV_D_HI
      LDA   MATH_RES0
      STA   NVR2L
      LDA   MATH_RES1
      STA   NVR2H
      LDA   MATH_RES2
      STA   NVR3L
      LDA   MATH_RES3
      STA   NVR3H
      BRA   @result
@large_divisor:
      STZ   NVR2L
      STZ   NVR2H
      LDA   NVR1H
      CMP   NVR0H
      BCC   @unchanged_remainder
      BNE   @subtract
      LDA   NVR1L
      CMP   NVR0L
      BCC   @unchanged_remainder
@subtract:
      INC   NVR2L
      SEC
      LDA   NVR1L
      SBC   NVR0L
      STA   NVR3L
      LDA   NVR1H
      SBC   NVR0H
      STA   NVR3H
      BRA   @result
@unchanged_remainder:
      LDA   NVR1L
      STA   NVR3L
      LDA   NVR1H
      STA   NVR3H
@result:
      TYA
      BNE   @modulo
      LDA   NVR2L
      LDX   NVR2H
      RTS
@modulo:
      LDA   NVR3L
      LDX   NVR3H
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
      .word ordinal_divide_unsigned-ordinal_code
      .byte 0, NOBJ_SYM_GLOBAL, 7, "P_DIVUW"
      .word ordinal_modulo_unsigned-ordinal_code
      .byte 0, NOBJ_SYM_GLOBAL, 7, "P_MODUW"
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

      ; Runtime checks are separate archive members. NL therefore retains
      ; only the enabled checks plus this shared error formatter.
      .word runtime_error_object_end-runtime_error_object
runtime_error_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 1, $FF
      .word 0
      .word 7
      .word runtime_error_symbols-runtime_error_object
      .word 10
      .word runtime_error_relocations-runtime_error_object
      .word 0

      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_EXEC, 0, 4, 0
      .word runtime_error_code_end-runtime_error_code
      .word runtime_error_code_end-runtime_error_code
      .byte "CODE"

runtime_error_code:
; A is the Turbo runtime error number; X/Y is the source line.
runtime_error:
      PHY
      PHX
      PHA
      .byte $A9
runtime_prefix_low: .byte 0
      .byte $A2
runtime_prefix_high: .byte 0
      .byte $20
runtime_prefix_call: .word 0
      PLA
      .byte $20
runtime_byte_call: .word 0
      .byte $A9
runtime_middle_low: .byte 0
      .byte $A2
runtime_middle_high: .byte 0
      .byte $20
runtime_middle_call: .word 0
      PLA
      PLX
      .byte $20
runtime_word_call: .word 0
      LDA   #$0A
      .byte $20
runtime_char_call: .word 0
      PASCAL_MEMORY_CALL MEM_EXIT_IMAGE
      RTS

; A/X points at a zero-terminated string. Preserve the cursor and pointer
; across P_WRITE_CHAR because Pascal runtime calls are caller-clobbered.
runtime_puts:
      STA   NVR0L
      STX   NVR0H
      LDY   #0
runtime_puts_character:
      LDA   (NVR0L),Y
      BEQ   runtime_puts_done
      LDA   NVR0H
      PHA
      LDA   NVR0L
      PHA
      PHY
      LDA   (NVR0L),Y
      .byte $20
runtime_puts_char_call: .word 0
      PLY
      PLA
      STA   NVR0L
      PLA
      STA   NVR0H
      INY
      BRA   runtime_puts_character
runtime_puts_done:
      RTS
runtime_prefix: .byte "Runtime error ", 0
runtime_middle: .byte " at line ", 0
runtime_error_code_end:

runtime_error_symbols:
      .word runtime_error-runtime_error_code
      .byte 0, NOBJ_SYM_GLOBAL, 15, "P_RUNTIME_ERROR"
      .word runtime_puts-runtime_error_code
      .byte 0, 0, 5, "RPuts"
      .word runtime_prefix-runtime_error_code
      .byte 0, 0, 7, "RPrefix"
      .word runtime_middle-runtime_error_code
      .byte 0, 0, 7, "RMiddle"
      .word 0
      .byte NOBJ_SYM_UNDEFINED, NOBJ_SYM_GLOBAL, 12, "P_WRITE_CHAR"
      .word 0
      .byte NOBJ_SYM_UNDEFINED, NOBJ_SYM_GLOBAL, 12, "P_WRITE_BYTE"
      .word 0
      .byte NOBJ_SYM_UNDEFINED, NOBJ_SYM_GLOBAL, 12, "P_WRITE_WORD"

runtime_error_relocations:
      .byte 0, NOBJ_RELOC_LO8
      .word runtime_prefix_low-runtime_error_code, 2, 0
      .byte 0, NOBJ_RELOC_HI8
      .word runtime_prefix_high-runtime_error_code, 2, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word runtime_prefix_call-runtime_error_code, 1, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word runtime_byte_call-runtime_error_code, 5, 0
      .byte 0, NOBJ_RELOC_LO8
      .word runtime_middle_low-runtime_error_code, 3, 0
      .byte 0, NOBJ_RELOC_HI8
      .word runtime_middle_high-runtime_error_code, 3, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word runtime_middle_call-runtime_error_code, 1, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word runtime_word_call-runtime_error_code, 6, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word runtime_char_call-runtime_error_code, 4, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word runtime_puts_char_call-runtime_error_code, 4, 0
runtime_error_object_end:

      ; Basic checks preserve A/X and share the inline-line decoder.
      .word check_line_object_end-check_line_object
check_line_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 1, $FF
      .word 0
      .word 2
      .word check_line_symbols-check_line_object
      .word 1
      .word check_line_relocations-check_line_object
      .word 0

      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_EXEC, 0, 4, 0
      .word check_line_code_end-check_line_code
      .word check_line_code_end-check_line_code
      .byte "CODE"

check_line_code:
check_line:
      PLA
      STA   NVR0L
      PLA
      STA   NVR0H
      CLC
      LDA   NVR0L
      ADC   #1
      STA   NVR1L
      LDA   NVR0H
      ADC   #0
      STA   NVR1H
      LDY   #0
      LDA   (NVR1L),Y
      STA   NVR7L
      INY
      LDA   (NVR1L),Y
      STA   NVR7H
      CLC
      LDA   NVR0L
      ADC   #2
      STA   NVR0L
      LDA   NVR0H
      ADC   #0
      PHA
      LDA   NVR0L
      PHA
      LDA   NVR5L
      BNE   @line_fail
      LDA   NVR6L
      LDX   NVR6H
      ORA   #0                            ; return Z/N for the value in A
      RTS
@line_fail:
      LDX   NVR7L
      LDY   NVR7H
      .byte $4C
check_line_error_jump: .word 0
check_line_code_end:

check_line_symbols:
      .word check_line-check_line_code
      .byte 0, NOBJ_SYM_GLOBAL, 14, "__P_CHECK_LINE"
      .word 0
      .byte NOBJ_SYM_UNDEFINED, NOBJ_SYM_GLOBAL, 15, "P_RUNTIME_ERROR"

check_line_relocations:
      .byte 0, NOBJ_RELOC_ABS16
      .word check_line_error_jump-check_line_code, 1, 0
check_line_object_end:

      .word check_overflow_object_end-check_overflow_object
check_overflow_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 1, $FF
      .word 0
      .word 2
      .word check_overflow_symbols-check_overflow_object
      .word 1
      .word check_overflow_relocations-check_overflow_object
      .word 0
      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_EXEC, 0, 4, 0
      .word check_overflow_code_end-check_overflow_code
      .word check_overflow_code_end-check_overflow_code
      .byte "CODE"
check_overflow_code:
check_overflow:
      STA   NVR6L
      STX   NVR6H
      BVS   @failed
      STZ   NVR5L
      BRA   @check
@failed:
      LDA   #215
      STA   NVR5L
@check:
      .byte $4C
check_overflow_jump: .word 0
check_overflow_code_end:
check_overflow_symbols:
      .word check_overflow-check_overflow_code
      .byte 0, NOBJ_SYM_GLOBAL, 18, "I_P_CHECK_OVERFLOW"
      .word 0
      .byte NOBJ_SYM_UNDEFINED, NOBJ_SYM_GLOBAL, 14, "__P_CHECK_LINE"
check_overflow_relocations:
      .byte 0, NOBJ_RELOC_ABS16
      .word check_overflow_jump-check_overflow_code, 1, 0
check_overflow_object_end:

      .word check_uadd_object_end-check_uadd_object
check_uadd_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 1, $FF
      .word 0
      .word 2
      .word check_uadd_symbols-check_uadd_object
      .word 1
      .word check_uadd_relocations-check_uadd_object
      .word 0
      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_EXEC, 0, 4, 0
      .word check_uadd_code_end-check_uadd_code
      .word check_uadd_code_end-check_uadd_code
      .byte "CODE"
check_uadd_code:
check_unsigned_add:
      STA   NVR6L
      STX   NVR6H
      BCS   @failed
      STZ   NVR5L
      BRA   @check
@failed:
      LDA   #215
      STA   NVR5L
@check:
      .byte $4C
check_uadd_jump: .word 0
check_uadd_code_end:
check_uadd_symbols:
      .word check_unsigned_add-check_uadd_code
      .byte 0, NOBJ_SYM_GLOBAL, 14, "I_P_CHECK_UADD"
      .word 0
      .byte NOBJ_SYM_UNDEFINED, NOBJ_SYM_GLOBAL, 14, "__P_CHECK_LINE"
check_uadd_relocations:
      .byte 0, NOBJ_RELOC_ABS16
      .word check_uadd_jump-check_uadd_code, 1, 0
check_uadd_object_end:

      .word check_usub_object_end-check_usub_object
check_usub_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 1, $FF
      .word 0
      .word 2
      .word check_usub_symbols-check_usub_object
      .word 1
      .word check_usub_relocations-check_usub_object
      .word 0
      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_EXEC, 0, 4, 0
      .word check_usub_code_end-check_usub_code
      .word check_usub_code_end-check_usub_code
      .byte "CODE"
check_usub_code:
check_unsigned_subtract:
      STA   NVR6L
      STX   NVR6H
      BCC   @failed
      STZ   NVR5L
      BRA   @check
@failed:
      LDA   #215
      STA   NVR5L
@check:
      .byte $4C
check_usub_jump: .word 0
check_usub_code_end:
check_usub_symbols:
      .word check_unsigned_subtract-check_usub_code
      .byte 0, NOBJ_SYM_GLOBAL, 14, "I_P_CHECK_USUB"
      .word 0
      .byte NOBJ_SYM_UNDEFINED, NOBJ_SYM_GLOBAL, 14, "__P_CHECK_LINE"
check_usub_relocations:
      .byte 0, NOBJ_RELOC_ABS16
      .word check_usub_jump-check_usub_code, 1, 0
check_usub_object_end:

      .word check_muls_object_end-check_muls_object
check_muls_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 1, $FF
      .word 0
      .word 2
      .word check_muls_symbols-check_muls_object
      .word 1
      .word check_muls_relocations-check_muls_object
      .word 0
      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_EXEC, 0, 4, 0
      .word check_muls_code_end-check_muls_code
      .word check_muls_code_end-check_muls_code
      .byte "CODE"
check_muls_code:
check_multiply_signed:
      STA   NVR6L
      STX   NVR6H
      TXA
      BMI   @negative
      LDA   MATH_RES2
      ORA   MATH_RES3
      BEQ   @ok
      BRA   @failed
@negative:
      LDA   MATH_RES2
      CMP   #$FF
      BNE   @failed
      LDA   MATH_RES3
      CMP   #$FF
      BEQ   @ok
@failed:
      LDA   #215
      STA   NVR5L
      BRA   @check
@ok:
      STZ   NVR5L
@check:
      .byte $4C
check_muls_jump: .word 0
check_muls_code_end:
check_muls_symbols:
      .word check_multiply_signed-check_muls_code
      .byte 0, NOBJ_SYM_GLOBAL, 14, "I_P_CHECK_MULS"
      .word 0
      .byte NOBJ_SYM_UNDEFINED, NOBJ_SYM_GLOBAL, 14, "__P_CHECK_LINE"
check_muls_relocations:
      .byte 0, NOBJ_RELOC_ABS16
      .word check_muls_jump-check_muls_code, 1, 0
check_muls_object_end:

      .word check_mulu_object_end-check_mulu_object
check_mulu_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 1, $FF
      .word 0
      .word 2
      .word check_mulu_symbols-check_mulu_object
      .word 1
      .word check_mulu_relocations-check_mulu_object
      .word 0
      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_EXEC, 0, 4, 0
      .word check_mulu_code_end-check_mulu_code
      .word check_mulu_code_end-check_mulu_code
      .byte "CODE"
check_mulu_code:
check_multiply_unsigned:
      STA   NVR6L
      STX   NVR6H
      LDA   MATH_RES2
      ORA   MATH_RES3
      BEQ   @ok
      LDA   #215
      STA   NVR5L
      BRA   @check
@ok:
      STZ   NVR5L
@check:
      .byte $4C
check_mulu_jump: .word 0
check_mulu_code_end:
check_mulu_symbols:
      .word check_multiply_unsigned-check_mulu_code
      .byte 0, NOBJ_SYM_GLOBAL, 14, "I_P_CHECK_MULU"
      .word 0
      .byte NOBJ_SYM_UNDEFINED, NOBJ_SYM_GLOBAL, 14, "__P_CHECK_LINE"
check_mulu_relocations:
      .byte 0, NOBJ_RELOC_ABS16
      .word check_mulu_jump-check_mulu_code, 1, 0
check_mulu_object_end:

      .word check_negs_object_end-check_negs_object
check_negs_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 1, $FF
      .word 0
      .word 2
      .word check_negs_symbols-check_negs_object
      .word 1
      .word check_negs_relocations-check_negs_object
      .word 0
      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_EXEC, 0, 4, 0
      .word check_negs_code_end-check_negs_code
      .word check_negs_code_end-check_negs_code
      .byte "CODE"
check_negs_code:
check_negate_signed:
      STA   NVR6L
      STX   NVR6H
      CPX   #$80
      BNE   @ok
      CMP   #$00
      BEQ   @failed
@ok:
      STZ   NVR5L
      BRA   @check
@failed:
      LDA   #215
      STA   NVR5L
@check:
      .byte $4C
check_negs_jump: .word 0
check_negs_code_end:
check_negs_symbols:
      .word check_negate_signed-check_negs_code
      .byte 0, NOBJ_SYM_GLOBAL, 14, "I_P_CHECK_NEGS"
      .word 0
      .byte NOBJ_SYM_UNDEFINED, NOBJ_SYM_GLOBAL, 14, "__P_CHECK_LINE"
check_negs_relocations:
      .byte 0, NOBJ_RELOC_ABS16
      .word check_negs_jump-check_negs_code, 1, 0
check_negs_object_end:

      .word check_negu_object_end-check_negu_object
check_negu_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 1, $FF
      .word 0
      .word 2
      .word check_negu_symbols-check_negu_object
      .word 1
      .word check_negu_relocations-check_negu_object
      .word 0
      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_EXEC, 0, 4, 0
      .word check_negu_code_end-check_negu_code
      .word check_negu_code_end-check_negu_code
      .byte "CODE"
check_negu_code:
check_negate_unsigned:
      STA   NVR6L
      STX   NVR6H
      CPX   #$80
      BCC   @ok
      BNE   @failed
      CMP   #$00
      BEQ   @ok
@failed:
      LDA   #215
      STA   NVR5L
      BRA   @check
@ok:
      STZ   NVR5L
@check:
      .byte $4C
check_negu_jump: .word 0
check_negu_code_end:
check_negu_symbols:
      .word check_negate_unsigned-check_negu_code
      .byte 0, NOBJ_SYM_GLOBAL, 14, "I_P_CHECK_NEGU"
      .word 0
      .byte NOBJ_SYM_UNDEFINED, NOBJ_SYM_GLOBAL, 14, "__P_CHECK_LINE"
check_negu_relocations:
      .byte 0, NOBJ_RELOC_ABS16
      .word check_negu_jump-check_negu_code, 1, 0
check_negu_object_end:

      .word check_divs_object_end-check_divs_object
check_divs_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 1, $FF
      .word 0
      .word 2
      .word check_divs_symbols-check_divs_object
      .word 1
      .word check_divs_relocations-check_divs_object
      .word 0
      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_EXEC, 0, 4, 0
      .word check_divs_code_end-check_divs_code
      .word check_divs_code_end-check_divs_code
      .byte "CODE"
check_divs_code:
check_divide_signed:
      STA   NVR6L
      STX   NVR6H
      CPX   #$FF
      BNE   @ok
      CMP   #$FF
      BNE   @ok
      TSX
      LDA   $0104,X
      CMP   #$80
      BNE   @ok
      LDA   $0103,X
      BEQ   @failed
@ok:
      STZ   NVR5L
      BRA   @check
@failed:
      LDA   #215
      STA   NVR5L
@check:
      .byte $4C
check_divs_jump: .word 0
check_divs_code_end:
check_divs_symbols:
      .word check_divide_signed-check_divs_code
      .byte 0, NOBJ_SYM_GLOBAL, 14, "I_P_CHECK_DIVS"
      .word 0
      .byte NOBJ_SYM_UNDEFINED, NOBJ_SYM_GLOBAL, 14, "__P_CHECK_LINE"
check_divs_relocations:
      .byte 0, NOBJ_RELOC_ABS16
      .word check_divs_jump-check_divs_code, 1, 0
check_divs_object_end:

      .word check_zero8_object_end-check_zero8_object
check_zero8_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 1, $FF
      .word 0
      .word 2
      .word check_zero8_symbols-check_zero8_object
      .word 1
      .word check_zero8_relocations-check_zero8_object
      .word 0
      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_EXEC, 0, 4, 0
      .word check_zero8_code_end-check_zero8_code
      .word check_zero8_code_end-check_zero8_code
      .byte "CODE"
check_zero8_code:
check_zero8:
      STA   NVR6L
      STX   NVR6H
      ORA   #0
      BEQ   @failed
      STZ   NVR5L
      BRA   @check
@failed:
      LDA   #200
      STA   NVR5L
@check:
      .byte $4C
check_zero8_jump: .word 0
check_zero8_code_end:
check_zero8_symbols:
      .word check_zero8-check_zero8_code
      .byte 0, NOBJ_SYM_GLOBAL, 15, "I_P_CHECK_ZERO8"
      .word 0
      .byte NOBJ_SYM_UNDEFINED, NOBJ_SYM_GLOBAL, 14, "__P_CHECK_LINE"
check_zero8_relocations:
      .byte 0, NOBJ_RELOC_ABS16
      .word check_zero8_jump-check_zero8_code, 1, 0
check_zero8_object_end:

      .word check_zero16_object_end-check_zero16_object
check_zero16_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 1, $FF
      .word 0
      .word 2
      .word check_zero16_symbols-check_zero16_object
      .word 1
      .word check_zero16_relocations-check_zero16_object
      .word 0
      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_EXEC, 0, 4, 0
      .word check_zero16_code_end-check_zero16_code
      .word check_zero16_code_end-check_zero16_code
      .byte "CODE"
check_zero16_code:
check_zero16:
      STA   NVR6L
      STX   NVR6H
      ORA   NVR6H
      BEQ   @failed
      STZ   NVR5L
      BRA   @check
@failed:
      LDA   #200
      STA   NVR5L
@check:
      .byte $4C
check_zero16_jump: .word 0
check_zero16_code_end:
check_zero16_symbols:
      .word check_zero16-check_zero16_code
      .byte 0, NOBJ_SYM_GLOBAL, 16, "I_P_CHECK_ZERO16"
      .word 0
      .byte NOBJ_SYM_UNDEFINED, NOBJ_SYM_GLOBAL, 14, "__P_CHECK_LINE"
check_zero16_relocations:
      .byte 0, NOBJ_RELOC_ABS16
      .word check_zero16_jump-check_zero16_code, 1, 0
check_zero16_object_end:

      .word check_io_read_object_end-check_io_read_object
check_io_read_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 1, $FF
      .word 0
      .word 2
      .word check_io_read_symbols-check_io_read_object
      .word 1
      .word check_io_read_relocations-check_io_read_object
      .word 0
      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_EXEC, 0, 4, 0
      .word check_io_read_code_end-check_io_read_code
      .word check_io_read_code_end-check_io_read_code
      .byte "CODE"
check_io_read_code:
check_io_read:
      STA   NVR6L
      STX   NVR6H
      LDA   FIO_ERRCODE
      BEQ   @ok
      LDA   #100
      BRA   @check
@ok:
      LDA   #0
@check:
      STA   NVR5L
      .byte $4C
check_io_read_jump: .word 0
check_io_read_code_end:
check_io_read_symbols:
      .word check_io_read-check_io_read_code
      .byte 0, NOBJ_SYM_GLOBAL, 16, "I_P_IOCHECK_READ"
      .word 0
      .byte NOBJ_SYM_UNDEFINED, NOBJ_SYM_GLOBAL, 14, "__P_CHECK_LINE"
check_io_read_relocations:
      .byte 0, NOBJ_RELOC_ABS16
      .word check_io_read_jump-check_io_read_code, 1, 0
check_io_read_object_end:

      .word check_io_write_object_end-check_io_write_object
check_io_write_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 1, $FF
      .word 0
      .word 2
      .word check_io_write_symbols-check_io_write_object
      .word 1
      .word check_io_write_relocations-check_io_write_object
      .word 0
      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_EXEC, 0, 4, 0
      .word check_io_write_code_end-check_io_write_code
      .word check_io_write_code_end-check_io_write_code
      .byte "CODE"
check_io_write_code:
check_io_write:
      STA   NVR6L
      STX   NVR6H
      LDA   FIO_ERRCODE
      BEQ   @ok
      LDA   #101
      BRA   @check
@ok:
      LDA   #0
@check:
      STA   NVR5L
      .byte $4C
check_io_write_jump: .word 0
check_io_write_code_end:
check_io_write_symbols:
      .word check_io_write-check_io_write_code
      .byte 0, NOBJ_SYM_GLOBAL, 17, "I_P_IOCHECK_WRITE"
      .word 0
      .byte NOBJ_SYM_UNDEFINED, NOBJ_SYM_GLOBAL, 14, "__P_CHECK_LINE"
check_io_write_relocations:
      .byte 0, NOBJ_RELOC_ABS16
      .word check_io_write_jump-check_io_write_code, 1, 0
check_io_write_object_end:

      .word check_range_object_end-check_range_object
check_range_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 1, $FF
      .word 0
      .word 4
      .word check_range_symbols-check_range_object
      .word 1
      .word check_range_relocations-check_range_object
      .word 0
      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_EXEC, 0, 4, 0
      .word check_range_code_end-check_range_code
      .word check_range_code_end-check_range_code
      .byte "CODE"
check_range_code:

; A/X is checked against two inclusive inline bounds followed by a source
; line: .WORD low, high, line. The signed entry biases all high bytes by $80.
check_range_unsigned:
      STA   NVR6L
      STX   NVR6H
      STZ   NVR5H
      BRA   check_range
check_range_signed:
      STA   NVR6L
      STX   NVR6H
      LDA   #1
      STA   NVR5H
check_range:
      PLA
      STA   NVR0L
      PLA
      STA   NVR0H
      CLC
      LDA   NVR0L
      ADC   #1
      STA   NVR3L
      LDA   NVR0H
      ADC   #0
      STA   NVR3H
      LDY   #0
      LDA   (NVR3L),Y
      STA   NVR1L
      INY
      LDA   (NVR3L),Y
      STA   NVR1H
      INY
      LDA   (NVR3L),Y
      STA   NVR2L
      INY
      LDA   (NVR3L),Y
      STA   NVR2H
      INY
      LDA   (NVR3L),Y
      STA   NVR7L
      INY
      LDA   (NVR3L),Y
      STA   NVR7H
      CLC
      LDA   NVR0L
      ADC   #6
      STA   NVR0L
      LDA   NVR0H
      ADC   #0
      PHA
      LDA   NVR0L
      PHA
      LDA   NVR5H
      BEQ   @range_compare
      LDA   NVR6H
      EOR   #$80
      STA   NVR6H
      LDA   NVR1H
      EOR   #$80
      STA   NVR1H
      LDA   NVR2H
      EOR   #$80
      STA   NVR2H
@range_compare:
      LDA   NVR6H
      CMP   NVR1H
      BCC   check_range_fail
      BNE   @range_lower_ok
      LDA   NVR6L
      CMP   NVR1L
      BCC   check_range_fail
@range_lower_ok:
      LDA   NVR6H
      CMP   NVR2H
      BCC   @range_ok
      BNE   check_range_fail
      LDA   NVR6L
      CMP   NVR2L
      BCC   @range_ok
      BNE   check_range_fail
@range_ok:
      LDA   NVR5H
      BEQ   :+
      LDA   NVR6H
      EOR   #$80
      STA   NVR6H
:     LDA   NVR6L
      LDX   NVR6H
      RTS

check_range_fail:
      LDA   #201
      LDX   NVR7L
      LDY   NVR7H
      .byte $4C
check_range_error_jump: .word 0

; A/X is checked against the inclusive dynamic high bound in NVR0. The only
; inline word is the source line, matching the other removable range checks.
check_range_open:
      STA   NVR6L
      STX   NVR6H
      PLA
      STA   NVR3L
      PLA
      STA   NVR3H
      CLC
      LDA   NVR3L
      ADC   #1
      STA   NVR1L
      LDA   NVR3H
      ADC   #0
      STA   NVR1H
      LDY   #0
      LDA   (NVR1L),Y
      STA   NVR7L
      INY
      LDA   (NVR1L),Y
      STA   NVR7H
      CLC
      LDA   NVR3L
      ADC   #2
      STA   NVR3L
      LDA   NVR3H
      ADC   #0
      PHA
      LDA   NVR3L
      PHA
      LDA   NVR6H
      CMP   NVR0H
      BCC   @open_ok
      BNE   check_range_fail
      LDA   NVR6L
      CMP   NVR0L
      BCC   @open_ok
      BNE   check_range_fail
@open_ok:
      LDA   NVR6L
      LDX   NVR6H
      RTS
check_range_code_end:

check_range_symbols:
      .word check_range_unsigned-check_range_code
      .byte 0, NOBJ_SYM_GLOBAL, 17, "I_P_CHECK_RANGE_U"
      .word check_range_signed-check_range_code
      .byte 0, NOBJ_SYM_GLOBAL, 17, "I_P_CHECK_RANGE_S"
      .word 0
      .byte NOBJ_SYM_UNDEFINED, NOBJ_SYM_GLOBAL, 15, "P_RUNTIME_ERROR"
      .word check_range_open-check_range_code
      .byte 0, NOBJ_SYM_GLOBAL, 20, "I_P_CHECK_RANGE_OPEN"

check_range_relocations:
      .byte 0, NOBJ_RELOC_ABS16
      .word check_range_error_jump-check_range_code, 2, 0
check_range_object_end:

      ; Stack checking is separate so range/I/O checks do not pull the 1 KiB
      ; lexical-frame arena into otherwise frameless programs.
      .word stack_check_object_end-stack_check_object
stack_check_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 1, $FF
      .word 0
      .word 4
      .word stack_check_symbols-stack_check_object
      .word 7
      .word stack_check_relocations-stack_check_object
      .word 0

      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_EXEC, 0, 4, 0
      .word stack_check_code_end-stack_check_code
      .word stack_check_code_end-stack_check_code
      .byte "CODE"

stack_check_code:
stack_check:
      PLA
      STA   NVR0L
      PLA
      STA   NVR0H
      CLC
      LDA   NVR0L
      ADC   #1
      STA   NVR5L
      LDA   NVR0H
      ADC   #0
      STA   NVR5H
      LDY   #0
      LDA   (NVR5L),Y
      STA   NVR2L
      INY
      LDA   (NVR5L),Y
      STA   NVR2H
      INY
      LDA   (NVR5L),Y
      STA   NVR7L
      INY
      LDA   (NVR5L),Y
      STA   NVR7H
      CLC
      LDA   NVR0L
      ADC   #4
      STA   NVR0L
      LDA   NVR0H
      ADC   #0
      PHA
      LDA   NVR0L
      PHA
      .byte $AD
stack_check_sp_low: .word 0
      STA   NVR3L
      .byte $AD
stack_check_sp_high: .word 0
      STA   NVR3H
      ORA   NVR3L
      BNE   stack_check_have_sp
      .byte $A9
stack_check_base_low: .byte 0
      STA   NVR3L
      .byte $A9
stack_check_base_high: .byte 0
      STA   NVR3H
stack_check_have_sp:
      CLC
      LDA   NVR3L
      ADC   NVR2L
      STA   NVR4L
      LDA   NVR3H
      ADC   NVR2H
      BCS   stack_check_fail
      STA   NVR4H
      .byte $C9
stack_check_end_high: .byte 0
      BCC   stack_check_ok
      BNE   stack_check_fail
      LDA   NVR4L
      .byte $C9
stack_check_end_low: .byte 0
      BCC   stack_check_ok
      BEQ   stack_check_ok
stack_check_fail:
      LDA   #202
      LDX   NVR7L
      LDY   NVR7H
      .byte $4C
stack_check_error_jump: .word 0
stack_check_ok:
      RTS
stack_check_code_end:

stack_check_symbols:
      .word stack_check-stack_check_code
      .byte 0, NOBJ_SYM_GLOBAL, 15, "I_P_STACK_CHECK"
      .word 0
      .byte NOBJ_SYM_UNDEFINED, NOBJ_SYM_GLOBAL, 7, "__NP_SP"
      .word 0
      .byte NOBJ_SYM_UNDEFINED, NOBJ_SYM_GLOBAL, 10, "__NP_STACK"
      .word 0
      .byte NOBJ_SYM_UNDEFINED, NOBJ_SYM_GLOBAL, 15, "P_RUNTIME_ERROR"

stack_check_relocations:
      .byte 0, NOBJ_RELOC_ABS16
      .word stack_check_sp_low-stack_check_code, 1, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word stack_check_sp_high-stack_check_code, 1, 1
      .byte 0, NOBJ_RELOC_LO8
      .word stack_check_base_low-stack_check_code, 2, 0
      .byte 0, NOBJ_RELOC_HI8
      .word stack_check_base_high-stack_check_code, 2, 0
      .byte 0, NOBJ_RELOC_HI8
      .word stack_check_end_high-stack_check_code, 2, $0400
      .byte 0, NOBJ_RELOC_LO8
      .word stack_check_end_low-stack_check_code, 2, $0400
      .byte 0, NOBJ_RELOC_ABS16
      .word stack_check_error_jump-stack_check_code, 3, 0
stack_check_object_end:

      ; Value open arrays are copied into the current lexical-frame arena.
      ; Keeping this in its own member makes CONST/VAR open arrays free.
      .word open_copy_object_end-open_copy_object
open_copy_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 1, $FF
      .word 0
      .word 5
      .word open_copy_symbols-open_copy_object
      .word 9
      .word open_copy_relocations-open_copy_object
      .word 0

      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_EXEC, 0, 4, 0
      .word open_copy_code_end-open_copy_code
      .word open_copy_code_end-open_copy_code
      .byte "CODE"

open_copy_code:
open_copy:
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
      STA   NVR4L                         ; descriptor offset
      INY
      LDA   (NVR7L),Y
      STA   NVR4H
      INY
      LDA   (NVR7L),Y
      STA   MATH_MUL16_B_LO               ; element size
      INY
      LDA   (NVR7L),Y
      STA   MATH_MUL16_B_HI
      INY
      LDA   (NVR7L),Y
      STA   NVR3L                         ; source line
      INY
      LDA   (NVR7L),Y
      STA   NVR3H
      CLC
      LDA   NVR6L
      ADC   #6
      STA   NVR6L
      LDA   NVR6H
      ADC   #0
      PHA
      LDA   NVR6L
      PHA

      .byte $AD
open_copy_load_fp: .word 0                ; LDA __NP_FP
      STA   NVR1L
      .byte $AD
open_copy_load_fp_hi: .word 0             ; LDA __NP_FP+1
      STA   NVR1H
      CLC
      LDA   NVR1L
      ADC   NVR4L
      STA   NVR1L
      LDA   NVR1H
      ADC   NVR4H
      STA   NVR1H
      LDY   #0
      LDA   (NVR1L),Y
      STA   NVR2L                         ; source pointer
      INY
      LDA   (NVR1L),Y
      STA   NVR2H
      INY
      LDA   (NVR1L),Y
      CLC
      ADC   #1
      STA   MATH_MUL16_A_LO               ; High + 1
      INY
      LDA   (NVR1L),Y
      ADC   #0
      STA   MATH_MUL16_A_HI
      LDA   MATH_RES2
      ORA   MATH_RES3
      BNE   open_copy_fail
      LDA   MATH_RES0
      STA   NVR5L                         ; byte count
      LDA   MATH_RES1
      STA   NVR5H

      .byte $AD
open_copy_load_sp: .word 0                ; LDA __NP_SP
      STA   NVR0L
      .byte $AD
open_copy_load_sp_hi: .word 0             ; LDA __NP_SP+1
      STA   NVR0H
      CLC
      LDA   NVR0L
      ADC   NVR5L
      STA   NVR4L
      LDA   NVR0H
      ADC   NVR5H
      BCS   open_copy_fail
      STA   NVR4H
      .byte $C9
open_copy_end_high: .byte 0               ; CMP #>(__NP_STACK+$0400)
      BCC   open_copy_space
      BNE   open_copy_fail
      LDA   NVR4L
      .byte $C9
open_copy_end_low: .byte 0                ; CMP #<(__NP_STACK+$0400)
      BCC   open_copy_space
      BNE   open_copy_fail
open_copy_space:
      LDY   #0
      LDA   NVR0L
      STA   (NVR1L),Y
      INY
      LDA   NVR0H
      STA   (NVR1L),Y
      LDA   NVR5L
      ORA   NVR5H
      BEQ   open_copy_store_sp
      LDY   #0
open_copy_byte:
      LDA   (NVR2L),Y
      STA   (NVR0L),Y
      INC   NVR2L
      BNE   :+
      INC   NVR2H
:     INC   NVR0L
      BNE   :+
      INC   NVR0H
:     LDA   NVR5L
      BNE   :+
      DEC   NVR5H
:     DEC   NVR5L
      LDA   NVR5L
      ORA   NVR5H
      BNE   open_copy_byte
open_copy_store_sp:
      LDA   NVR0L
      .byte $8D
open_copy_store_sp_low: .word 0            ; STA __NP_SP
      LDA   NVR0H
      .byte $8D
open_copy_store_sp_high: .word 0           ; STA __NP_SP+1
      RTS
open_copy_fail:
      LDA   #202
      LDX   NVR3L
      LDY   NVR3H
      .byte $4C
open_copy_error_jump: .word 0
open_copy_code_end:

open_copy_symbols:
      .word open_copy-open_copy_code
      .byte 0, NOBJ_SYM_GLOBAL, 11, "P_OPEN_COPY"
      .word 0
      .byte NOBJ_SYM_UNDEFINED, NOBJ_SYM_GLOBAL, 7, "__NP_FP"
      .word 0
      .byte NOBJ_SYM_UNDEFINED, NOBJ_SYM_GLOBAL, 7, "__NP_SP"
      .word 0
      .byte NOBJ_SYM_UNDEFINED, NOBJ_SYM_GLOBAL, 10, "__NP_STACK"
      .word 0
      .byte NOBJ_SYM_UNDEFINED, NOBJ_SYM_GLOBAL, 15, "P_RUNTIME_ERROR"

open_copy_relocations:
      .byte 0, NOBJ_RELOC_ABS16
      .word open_copy_load_fp-open_copy_code, 1, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word open_copy_load_fp_hi-open_copy_code, 1, 1
      .byte 0, NOBJ_RELOC_ABS16
      .word open_copy_load_sp-open_copy_code, 2, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word open_copy_load_sp_hi-open_copy_code, 2, 1
      .byte 0, NOBJ_RELOC_HI8
      .word open_copy_end_high-open_copy_code, 3, $0400
      .byte 0, NOBJ_RELOC_LO8
      .word open_copy_end_low-open_copy_code, 3, $0400
      .byte 0, NOBJ_RELOC_ABS16
      .word open_copy_store_sp_low-open_copy_code, 2, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word open_copy_store_sp_high-open_copy_code, 2, 1
      .byte 0, NOBJ_RELOC_ABS16
      .word open_copy_error_jump-open_copy_code, 4, 0
open_copy_object_end:

      ; One shared lexical-frame arena is extracted only when compiled code
      ; references a routine frame. Programs without routines pay nothing.
      .word frame_object_end-frame_object
frame_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 2, $FF
      .word 0
      .word 8
      .word frame_symbols-frame_object
      .word 21
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
; little-endian frame size, parameter count, width mask, and open-array mask;
; each mask's low bit describes the last parameter. Bit 7 of the count
; reserves frame bytes two and three for a wide function result. One ordinary
; parameter arrives in NVR1; multiple or descriptor parameters use the stack.
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
      INY
      LDA   (NVR7L),Y
      STA   NVR5H                         ; 1 = four-byte open-array descriptor
      CLC
      LDA   NVR6L
      ADC   #6
      STA   NVR6L
      LDA   NVR6H
      ADC   #0
      STA   NVR6H

      LDA   NVR2L
      BEQ   frame_enter_unstacked_done
      CMP   #1
      BNE   frame_enter_stacked_parameters
      LDA   NVR5H
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
      BCC   frame_enter_unstacked_done
      INY
      LDA   NVR1H
      STA   (NVR0L),Y
      BRA   frame_enter_unstacked_done

frame_enter_unstacked_done:
      .byte $4C
frame_enter_parameters_jump: .word 0

frame_enter_stacked_parameters:
      ; Find the byte immediately beyond the packed frame parameters.
      LDA   NVR2L
      STA   NVR3L
      STA   NVR1L                         ; caller stack bytes to consume
      STA   NVR7L
      LDA   NVR3H
      BPL   :+
      INC   NVR7L
      INC   NVR7L
:
      LDA   NVR2H
      STA   NVR3H
      LDA   NVR5H
      PHA
frame_enter_count_parameter_bytes:
      LSR   NVR5H
      BCC   frame_enter_count_width
      LSR   NVR3H
      CLC
      LDA   NVR7L
      ADC   #3
      STA   NVR7L
      CLC
      LDA   NVR1L
      ADC   #3
      STA   NVR1L
      BRA   frame_enter_count_next
frame_enter_count_width:
      LSR   NVR3H
      BCC   :+
      INC   NVR7L
      INC   NVR1L
:
frame_enter_count_next:
      DEC   NVR3L
      BNE   frame_enter_count_parameter_bytes
      PLA
      STA   NVR5H
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
      LSR   NVR5H
      BCS   frame_enter_copy_open
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
      BRA   frame_enter_next_parameter
frame_enter_copy_open:
      LSR   NVR2H
      SEC
      LDA   NVR7L
      SBC   #4
      STA   NVR7L
      LDX   #0
@copy:
      LDY   NVR7H
      LDA   $0100,Y
      INC   NVR7H
      PHA
      TXA
      CLC
      ADC   NVR7L
      TAY
      PLA
      STA   (NVR0L),Y
      INX
      CPX   #4
      BNE   @copy
frame_enter_next_parameter:
      DEC   NVR3L
      BNE   frame_enter_copy_parameter
      BRA   frame_enter_drop_parameters
frame_enter_parameters_done:
      BRA   frame_enter_save_context
frame_enter_drop_parameters:
      ; Stacked parameters now live in the software frame. Release their
      ; packed bytes before the routine body runs.
      PLA
      STA   NVR7L                         ; original routine return low
      PLA
      STA   NVR7H                         ; original routine return high
      LDX   NVR1L
frame_enter_drop_argument:
      PLA
      DEX
      BNE   frame_enter_drop_argument
      LDA   NVR7H
      PHA
      LDA   NVR7L
      PHA
frame_enter_save_context:
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
; P_FENTER has already consumed caller arguments after copying them. Inline
; metadata retains their byte count for compatibility; only bits 7 and 6 are
; needed here to mark byte and two-byte function results.
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
      .word frame_enter_parameters_jump-frame_code, 5, frame_enter_parameters_done-frame_enter
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
      .word 13
      .word string_symbols-string_object
      .word 12
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

string_insert:
      TSX
      LDA   $0107,X
      STA   NVR0L
      LDA   $0108,X
      STA   NVR0H
      LDA   $0105,X
      STA   NVR1L
      LDA   $0106,X
      STA   NVR1H
      LDA   $0103,X
      STA   NVR2L
      LDA   $0104,X
      STA   NVR2H
      LDY   #0
      LDA   (NVR0L),Y
      STA   NVR3L
      BNE   :+
      RTS
:
      .byte $20
string_insert_select: .word 0
      STA   NVR5L
      STX   NVR5H
      LDY   #0
      LDA   NVR3L
      STA   (NVR5L),Y
      TAX
      BEQ   @source_staged
      LDY   #1
@stage_source:
      LDA   (NVR0L),Y
      STA   (NVR5L),Y
      INY
      DEX
      BNE   @stage_source
@source_staged:
      LDY   #0
      LDA   (NVR1L),Y
      STA   NVR3H
      CMP   NVR7L
      BCC   :+
      RTS
:
      LDA   NVR7L
      SEC
      SBC   NVR3H
      CMP   NVR3L
      BCC   :+
      LDA   NVR3L
:     STA   NVR4L
      BNE   :+
      RTS
:
      LDA   NVR2H
      BNE   @append
      LDA   NVR2L
      BNE   @position_nonzero
      INC
@position_nonzero:
      STA   NVR2L
      LDA   NVR3H
      INC
      CMP   NVR2L
      BCS   @position_ready
@append:
      LDA   NVR3H
      INC
      STA   NVR2L
@position_ready:
      CLC
      LDA   NVR3H
      ADC   NVR4L
      STA   NVR7H
      LDY   #0
      STA   (NVR1L),Y
      LDA   NVR3H
      STA   NVR2H
      LDA   NVR7H
      STA   NVR4H
@insert_shift:
      LDA   NVR2H
      CMP   NVR2L
      BCC   @insert_copy
      TAY
      LDA   (NVR1L),Y
      LDY   NVR4H
      STA   (NVR1L),Y
      DEC   NVR2H
      DEC   NVR4H
      BRA   @insert_shift
@insert_copy:
      LDA   NVR2L
      STA   NVR3H
      LDA   #1
      STA   NVR4H
      LDX   NVR4L
@insert_store:
      LDY   NVR4H
      LDA   (NVR5L),Y
      STA   NVR7H
      LDY   NVR3H
      LDA   NVR7H
      STA   (NVR1L),Y
      INC   NVR4H
      INC   NVR3H
      DEX
      BNE   @insert_store
string_insert_done:
      RTS

; Return the one-based location of NVR0's short string in A/X's short string,
; or zero when it is absent. Empty needles match nowhere, as in Turbo Pascal.
string_pos:
      STA   NVR1L
      STX   NVR1H
      LDY   #0
      LDA   (NVR0L),Y
      STA   NVR2L
      BEQ   @missing
      LDA   (NVR1L),Y
      CMP   NVR2L
      BCC   @missing
      SEC
      SBC   NVR2L
      INC
      STA   NVR2H
      LDA   #1
      STA   NVR3L
@candidate:
      LDX   NVR2L
      LDY   #1
@character:
      LDA   (NVR0L),Y
      CMP   (NVR1L),Y
      BNE   @next
      INY
      DEX
      BNE   @character
      LDA   NVR3L
      LDX   #0
      RTS
@next:
      INC   NVR1L
      BNE   :+
      INC   NVR1H
:     INC   NVR3L
      LDA   NVR3L
      CMP   NVR2H
      BCC   @candidate
      BEQ   @candidate
@missing:
      LDA   #0
      TAX
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
      .word string_insert-string_code
      .byte 0, NOBJ_SYM_GLOBAL, 6, "INSERT"
      .word string_pos-string_code
      .byte 0, NOBJ_SYM_GLOBAL, 12, "P_STRING_POS"
      .word string_select_temp-string_code
      .byte 0, 0, 10, "STR_SELECT"
      .word 0
      .byte NOBJ_SYM_UNDEFINED, NOBJ_SYM_GLOBAL, 12, "__NP_STRBUF0"
      .word 0
      .byte NOBJ_SYM_UNDEFINED, NOBJ_SYM_GLOBAL, 12, "__NP_STRBUF1"

string_relocations:
      .byte 0, NOBJ_RELOC_LO8
      .word string_cmp_buf0_lo-string_code, 11, 0
      .byte 0, NOBJ_RELOC_HI8
      .word string_cmp_buf0_hi-string_code, 11, 0
      .byte 0, NOBJ_RELOC_LO8
      .word string_load_buf1_lo-string_code, 12, 0
      .byte 0, NOBJ_RELOC_HI8
      .word string_load_buf1_hi-string_code, 12, 0
      .byte 0, NOBJ_RELOC_LO8
      .word string_load_buf0_lo-string_code, 11, 0
      .byte 0, NOBJ_RELOC_HI8
      .word string_load_buf0_hi-string_code, 11, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word string_concat_select-string_code, 10, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word string_concat_char_select-string_code, 10, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word char_concat_select-string_code, 10, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word char_to_string_select-string_code, 10, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word string_insert_select-string_code, 10, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word string_copy_part_select-string_code, 10, 0
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
      CPY   #LANGRT_STR_LONG
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
      CMP   #LANGRT_LONG_CMP
      BEQ   rtcall_restore_compare
      CMP   #LANGRT_REAL_TRUNC
      BEQ   rtcall_return_word
      CMP   #LANGRT_REAL_ROUND
      BEQ   rtcall_return_word
      CMP   #LANGRT_STR_REAL
      BEQ   rtcall_return_void
      CMP   #LANGRT_STR_LONG
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

      .include "pascal_langrt_thunk.inc"
      .include "pascal_real_thunks.inc"
      .include "pascal_longint_thunks.inc"

      ; Turbo-compatible console services. The implementation uses Nova's
      ; canonical VGC registers and SYSTEM module mailbox; no private device
      ; addresses or wait loops are introduced by Pascal.
      .word system_object_end-system_object
system_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 1, $FF
      .word 0
      .word 18
      .word system_symbols-system_object
      .word 16
      .word system_relocations-system_object
      .word 0

      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_EXEC, 0, 4, 0
      .word system_code_end-system_code
      .word system_code_end-system_code
      .byte "CODE"
system_code:
system_window_args:
      .byte $AD
system_args_active: .word 0
      CMP   #NP_WINDOW_ACTIVE
      BEQ   system_args_state
      STZ   LIB_ARG0
      STZ   LIB_ARG0+1
      LDA   #NOVA_SCREEN_COLS
      STA   LIB_ARG0+2
      LDA   #NOVA_SCREEN_ROWS
      STA   LIB_ARG0+3
      RTS
system_args_state:
      .byte $AD
system_args_left: .word 0
      STA   LIB_ARG0
      .byte $AD
system_args_top: .word 0
      STA   LIB_ARG0+1
      .byte $AD
system_args_width: .word 0
      STA   LIB_ARG0+2
      .byte $AD
system_args_height: .word 0
      STA   LIB_ARG0+3
      RTS

system_window_home:
      .byte $AD
system_home_left: .word 0
      STA   VGC_CURSX
      .byte $AD
system_home_top: .word 0
      STA   VGC_CURSY
      RTS

system_gotoxy:
      .byte $AD
system_gotoxy_active: .word 0
      CMP   #NP_WINDOW_ACTIVE
      BEQ   system_gotoxy_window
      TSX
      LDA   $0105,X
      DEC
      STA   VGC_CURSX
      LDA   $0103,X
      DEC
      STA   VGC_CURSY
      RTS
system_gotoxy_window:
      TSX
      LDA   $0103,X
      BEQ   system_gotoxy_done
      .byte $CD
system_gotoxy_height: .word 0
      BEQ   system_gotoxy_y_ok
      BCS   system_gotoxy_done
system_gotoxy_y_ok:
      LDA   $0105,X
      BEQ   system_gotoxy_done
      .byte $CD
system_gotoxy_width: .word 0
      BEQ   system_gotoxy_x_ok
      BCS   system_gotoxy_done
system_gotoxy_x_ok:
      LDA   $0105,X
      DEC
      CLC
      .byte $6D
system_gotoxy_left: .word 0
      STA   VGC_CURSX
      LDA   $0103,X
      DEC
      CLC
      .byte $6D
system_gotoxy_top: .word 0
      STA   VGC_CURSY
system_gotoxy_done:
      RTS

system_clreol:
      .byte $20
system_clreol_args_call: .word 0
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
      .byte $AD
system_clrscr_active: .word 0
      CMP   #NP_WINDOW_ACTIVE
      BEQ   @window
      LDA   #$0C
      STA   VGC_CHAROUT
      RTS
@window:
      .byte $20
system_clrscr_args_call: .word 0
      PASCAL_SYSTEM_CALL SYS_CONSOLE_CLEAR_REGION
      .byte $4C
system_clrscr_home_jump: .word 0

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

system_ioresult:
      LDA   FIO_ERRCODE
      PHA
      STZ   FIO_ERRCODE
      LDX   #0
      PLA
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
      .word VGC_CHARIN
      .byte NOBJ_SYM_ABSOLUTE, NOBJ_SYM_GLOBAL, 3, "KBD"
      .word VGC_CHARIN
      .byte NOBJ_SYM_ABSOLUTE, NOBJ_SYM_GLOBAL, 10, "KEYPRESSED"
      .word system_ioresult-system_code
      .byte 0, NOBJ_SYM_GLOBAL, 8, "IORESULT"

      .word 0
      .byte NOBJ_SYM_UNDEFINED, NOBJ_SYM_GLOBAL, 11, "__NP_WINDOW"
      .word system_window_args-system_code
      .byte 0, NOBJ_SYM_GLOBAL, 7, "SYSARGS"
      .word system_window_home-system_code
      .byte 0, 0, 7, "SYSHOME"

system_relocations:
      .byte 0, NOBJ_RELOC_ABS16
      .word system_args_active-system_code, 15, 4
      .byte 0, NOBJ_RELOC_ABS16
      .word system_args_left-system_code, 15, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word system_args_top-system_code, 15, 1
      .byte 0, NOBJ_RELOC_ABS16
      .word system_args_width-system_code, 15, 2
      .byte 0, NOBJ_RELOC_ABS16
      .word system_args_height-system_code, 15, 3
      .byte 0, NOBJ_RELOC_ABS16
      .word system_home_left-system_code, 15, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word system_home_top-system_code, 15, 1
      .byte 0, NOBJ_RELOC_ABS16
      .word system_gotoxy_active-system_code, 15, 4
      .byte 0, NOBJ_RELOC_ABS16
      .word system_gotoxy_height-system_code, 15, 3
      .byte 0, NOBJ_RELOC_ABS16
      .word system_gotoxy_width-system_code, 15, 2
      .byte 0, NOBJ_RELOC_ABS16
      .word system_gotoxy_left-system_code, 15, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word system_gotoxy_top-system_code, 15, 1
      .byte 0, NOBJ_RELOC_ABS16
      .word system_clrscr_active-system_code, 15, 4
      .byte 0, NOBJ_RELOC_ABS16
      .word system_clreol_args_call-system_code, 16, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word system_clrscr_args_call-system_code, 16, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word system_clrscr_home_jump-system_code, 17, 0
system_object_end:

      ; Less common CRT queries and line-editing operations live in their own
      ; member so ordinary console programs do not pay for them.
      .word system_extra_object_end-system_extra_object
system_extra_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 1, $FF
      .word 0
      .word 9
      .word system_extra_symbols-system_extra_object
      .word 6
      .word system_extra_relocations-system_extra_object
      .word 0

      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_EXEC, 0, 4, 0
      .word system_extra_code_end-system_extra_code
      .word system_extra_code_end-system_extra_code
      .byte "CODE"
system_extra_code:
system_readkey:
      PASCAL_SYSTEM_CALL SYS_WAIT_KEY
      LDA   LIB_RESULT
      LDX   #0
      RTS

system_wherex:
      .byte $AD
system_wherex_active: .word 0
      CMP   #NP_WINDOW_ACTIVE
      BEQ   system_wherex_relative
      LDA   VGC_CURSX
      INC
      LDX   #0
      RTS
system_wherex_relative:
      LDA   VGC_CURSX
      SEC
      .byte $ED
system_wherex_left: .word 0
      INC
      LDX   #0
      RTS

system_wherey:
      .byte $AD
system_wherey_active: .word 0
      CMP   #NP_WINDOW_ACTIVE
      BEQ   system_wherey_relative
      LDA   VGC_CURSY
      INC
      LDX   #0
      RTS
system_wherey_relative:
      LDA   VGC_CURSY
      SEC
      .byte $ED
system_wherey_top: .word 0
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

system_delline:
      .byte $20
system_delline_args_call: .word 0
      PASCAL_SYSTEM_CALL SYS_CONSOLE_DELETE_LINE
      RTS
system_insline:
      .byte $20
system_insline_args_call: .word 0
      PASCAL_SYSTEM_CALL SYS_CONSOLE_INSERT_LINE
      RTS
system_extra_code_end:

system_extra_symbols:
      .word system_readkey-system_extra_code
      .byte 0, NOBJ_SYM_GLOBAL, 7, "READKEY"
      .word system_wherex-system_extra_code
      .byte 0, NOBJ_SYM_GLOBAL, 6, "WHEREX"
      .word system_wherey-system_extra_code
      .byte 0, NOBJ_SYM_GLOBAL, 6, "WHEREY"
      .word system_textcolor-system_extra_code
      .byte 0, NOBJ_SYM_GLOBAL, 9, "TEXTCOLOR"
      .word system_textbackground-system_extra_code
      .byte 0, NOBJ_SYM_GLOBAL, 14, "TEXTBACKGROUND"
      .word system_delline-system_extra_code
      .byte 0, NOBJ_SYM_GLOBAL, 7, "DELLINE"
      .word system_insline-system_extra_code
      .byte 0, NOBJ_SYM_GLOBAL, 7, "INSLINE"
      .word 0
      .byte NOBJ_SYM_UNDEFINED, NOBJ_SYM_GLOBAL, 11, "__NP_WINDOW"
      .word 0
      .byte NOBJ_SYM_UNDEFINED, NOBJ_SYM_GLOBAL, 7, "SYSARGS"

system_extra_relocations:
      .byte 0, NOBJ_RELOC_ABS16
      .word system_wherex_active-system_extra_code, 7, 4
      .byte 0, NOBJ_RELOC_ABS16
      .word system_wherex_left-system_extra_code, 7, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word system_wherey_active-system_extra_code, 7, 4
      .byte 0, NOBJ_RELOC_ABS16
      .word system_wherey_top-system_extra_code, 7, 1
      .byte 0, NOBJ_RELOC_ABS16
      .word system_delline_args_call-system_extra_code, 8, 0
      .byte 0, NOBJ_RELOC_ABS16
      .word system_insline_args_call-system_extra_code, 8, 0
system_extra_object_end:

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
      .word 6
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
      LDA   #FIO_FILE_ACCESS_RW
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
      LDA   #FIO_FILE_ACCESS_RW
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
      LDY   #1
      LDA   (NVR0L),Y
      CMP   #$7F                         ; Printer.Lst: create spool on first write
      BNE   file_write_ready
      LDA   LIB_ARG1
      PHA
      LDA   LIB_ARG1+1
      PHA
      LDA   LIB_ARG2
      PHA
      LDA   LIB_ARG2+1
      PHA
      LDA   NVR0L
      LDX   NVR0H
file_lazy_rewrite_call = * + 1
      JSR   file_rewrite
      PLA
      STA   LIB_ARG2+1
      PLA
      STA   LIB_ARG2
      PLA
      STA   LIB_ARG1+1
      PLA
      STA   LIB_ARG1
file_write_ready:
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
      .byte 0, NOBJ_RELOC_ABS16
      .word file_lazy_rewrite_call-file_code, 2, 0
file_object_end:

      ; Procedural-variable calls keep the target below any stacked arguments.
      ; This trampoline recovers it, restores the one-argument register ABI,
      ; and tail-jumps so the target's RTS returns directly to the caller.
      .word indirect_object_end-indirect_object
indirect_object:
      .byte NOBJ_MAGIC0, NOBJ_MAGIC1, NOBJ_MAGIC2, NOBJ_MAGIC3
      .byte NOBJ_VERSION, 0, 1, $FF
      .word 0
      .word 2
      .word indirect_symbols-indirect_object
      .word 1
      .word indirect_relocations-indirect_object
      .word 0

      .byte NOBJ_SEC_ALLOC | NOBJ_SEC_EXEC, 0, 4, 0
      .word indirect_code_end-indirect_code
      .word indirect_code_end-indirect_code
      .byte "CODE"
indirect_code:
indirect_call:
      STA   NVR1L
      AND   #$3F
      STA   NVR1H
      TSX
      TXA
      CLC
      ADC   NVR1H
      TAX
      LDA   $0103,X
      STA   NVR0L
      LDA   $0104,X
      STA   NVR0H
      LDA   NVR0L
      ORA   NVR0H
      BNE   indirect_valid
      LDA   #204
      .byte $4C
indirect_error_jump:
      .word 0
indirect_valid:
      BIT   NVR1L
      BPL   @invoke
      TSX
      LDA   NVR1L
      AND   #$40
      BEQ   @byte
      LDA   $0104,X
      PHA
      LDA   $0103,X
      PLX
      BRA   @invoke
@byte:
      LDA   $0103,X
@invoke:
      JMP   (NVR0L)
indirect_code_end:

indirect_symbols:
      .word indirect_call-indirect_code
      .byte 0, NOBJ_SYM_GLOBAL, 15, "P_CALL_INDIRECT"
      .word 0
      .byte NOBJ_SYM_UNDEFINED, NOBJ_SYM_GLOBAL, 15, "P_RUNTIME_ERROR"

indirect_relocations:
      .byte 0, NOBJ_RELOC_ABS16
      .word indirect_error_jump-indirect_code, 1, 0
indirect_object_end:

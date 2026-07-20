; Pascal runtime archive generated from the Nova NDK hardware definitions.
; NL treats this as an ordinary NLIB and has no knowledge of these symbols.

      .setcpu "w65c02"
      .include "nova.inc"
      .include "nobj.inc"

      .segment "LIBRARY"
      .byte NLIB_MAGIC0, NLIB_MAGIC1, NLIB_MAGIC2, NLIB_MAGIC3
      .byte NLIB_VERSION, 3

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

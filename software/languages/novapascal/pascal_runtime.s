; Pascal runtime archive generated from the Nova NDK hardware definitions.
; NL treats this as an ordinary NLIB and has no knowledge of these symbols.

      .setcpu "w65c02"
      .include "nova.inc"
      .include "nobj.inc"

      .segment "LIBRARY"
      .byte NLIB_MAGIC0, NLIB_MAGIC1, NLIB_MAGIC2, NLIB_MAGIC3
      .byte NLIB_VERSION, 2

      .byte 12, "P_WRITE_CHAR"
      .word @write_end-@write
@write:
      .byte $8D, <VGC_CHAROUT, >VGC_CHAROUT ; STA VGC_CHAROUT
      .byte $60                            ; RTS
@write_end:

      ; Deliberately unused. Its marker bytes let the emulator test prove that
      ; NL copied the referenced member rather than concatenating the archive.
      .byte 8, "P_UNUSED"
      .word @unused_end-@unused
@unused:
      .byte $A9, $DE, $A2, $AD, $A0, $BE, $60
@unused_end:

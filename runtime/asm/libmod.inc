; libmod.inc — emit the standard module preamble at $C000.
; Invoke `lib_module_header <id>,<ver>,<fncount>` as the FIRST thing in the CODE
; segment of a module (cfg ORGs CODE at $C000). `dispatch` must be a label that
; follows (the module's fn-id dispatcher).
      .include "libabi.inc"

.macro lib_module_header mod_id, ver, fncount
      jmp     dispatch                 ; $C000  entry
      .byte   MOD_MAGIC0, MOD_MAGIC1   ; $C003  "NL"
      .byte   mod_id                   ; $C005
      .byte   ver                      ; $C006
      .byte   fncount                  ; $C007
      .byte   $00, $00                 ; $C008  reserved
.endmacro

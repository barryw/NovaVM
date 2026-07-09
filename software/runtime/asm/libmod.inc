; =====================================================================
;  Nova NDK — libmod.inc
;
;  Emit the standard module preamble at $C000. Invoke
;  `lib_module_header <id>,<ver>,<fncount>` as the FIRST thing in a module's
;  CODE segment (cfg ORGs CODE at $C000); a `dispatch` label must follow
;  (the module's fn-id dispatcher).
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

      .include "libabi.inc"

.macro lib_module_header mod_id, ver, fncount
      jmp     dispatch                 ; $C000  entry
      .byte   MOD_MAGIC0, MOD_MAGIC1   ; $C003  "NL"
      .byte   mod_id                   ; $C005
      .byte   ver                      ; $C006
      .byte   fncount                  ; $C007
      .byte   $00, $00                 ; $C008  reserved
.endmacro

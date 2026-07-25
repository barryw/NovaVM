; An initialized Pascal Text descriptor. The shared file runtime recognizes
; mode $7F and creates PRINTER.TXT lazily on the first Write/Writeln.

.if .referenced(lst)
      .segment "RODATA"
      .export lst
lst:
      .byte $FF, $7F, 11, "PRINTER.TXT", 0, 0
.endif

; =====================================================================
;  Nova NDK — nvg_named.s
;
;  Pointer-name convenience wrappers kept in a separate archive member so
;  applications that prepare FIO.NAME directly do not pay for them.
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

.include "nvg.inc"

.ifndef NVG_NAMED_IMPLEMENTATION_INCLUDED
NVG_NAMED_IMPLEMENTATION_INCLUDED = 1

      .segment "CODE"

      .export nvg_load_named
      .export nvg_load_named_at
      .export nvg_draw_named
      .export nvg_draw_named_at

; @label NVG.LOAD_NAMED
; @kind routine
; @symbol nvg_load_named
; @library nvg
; @summary Copy NVG.NAMEPTR/NVG.NAMELEN into FIO.NAME, then load the NVG image at offset 0.
; @requires NVG_NAMELEN NVG_NAMEPTR_L NVG_NAMEPTR_H
; @out A: 0 on success, 1 on error.
nvg_load_named:
      JSR   fio_copy_name
      BNE   @done
      JMP   nvg_load
@done:
      RTS

; @label NVG.LOAD_NAMED_AT
; @kind routine
; @symbol nvg_load_named_at
; @library nvg
; @summary Copy NVG.NAMEPTR/NVG.NAMELEN into FIO.NAME, then load the NVG image at NVG.ADDRL/H.
; @requires NVG_NAMELEN NVG_NAMEPTR_L NVG_NAMEPTR_H NVG_ADDRL NVG_ADDRH
; @out A: 0 on success, 1 on error.
nvg_load_named_at:
      JSR   fio_copy_name
      BNE   @done
      JMP   nvg_load_at
@done:
      RTS

; @label NVG.DRAW_NAMED
; @kind routine
; @symbol nvg_draw_named
; @library nvg
; @summary Copy NVG.NAMEPTR/NVG.NAMELEN into FIO.NAME, then draw the NVG image at offset 0 without clearing.
; @requires NVG_NAMELEN NVG_NAMEPTR_L NVG_NAMEPTR_H
; @out A: 0 on success, 1 on error.
nvg_draw_named:
      JSR   fio_copy_name
      BNE   @done
      JMP   nvg_draw
@done:
      RTS

; @label NVG.DRAW_NAMED_AT
; @kind routine
; @symbol nvg_draw_named_at
; @library nvg
; @summary Copy NVG.NAMEPTR/NVG.NAMELEN into FIO.NAME, then draw the NVG image at NVG.ADDRL/H without clearing.
; @requires NVG_NAMELEN NVG_NAMEPTR_L NVG_NAMEPTR_H NVG_ADDRL NVG_ADDRH NVG_FILEL NVG_FILEM NVG_FILEH
; @out A: 0 on success, 1 on error.
nvg_draw_named_at:
      JSR   fio_copy_name
      BNE   @done
      JMP   nvg_draw_at
@done:
      RTS

.endif

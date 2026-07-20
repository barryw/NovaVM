; =====================================================================
;  Nova NDK — vtext_inline.s
;
;  Dense, inline-parameter text output for static assembly call sites.
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

.include "vtext.inc"

.ifndef VTEXT_INLINE_IMPLEMENTATION_INCLUDED
VTEXT_INLINE_IMPLEMENTATION_INCLUDED = 1

      .segment "ZEROPAGE"

vtext_inline_scan:  .res 2

      .segment "BSS"

vtext_inline_text:  .res 2

      .segment "CODE"

      .export i_vtext_puts
      .export i_vtext_print_at

; @label I_VTEXT_PUTS
; @kind routine
; @symbol i_vtext_puts
; @library vtext
; @abi inline
; @summary Print the zero-terminated text immediately after the JSR in the current region.
; @in Inline: Text bytes followed by zero.
; @requires VTEXT_LEFT VTEXT_TOP VTEXT_WIDTH VTEXT_HEIGHT VTEXT_CURX VTEXT_CURY VTEXT_COLOR VTEXT_ATTR
; @out A: 0 on success, 1 on error.
; @note The saved return address is advanced past the terminating zero.
i_vtext_puts:
      TSX
      CLC
      LDA   $0101,X
      ADC   #$01
      STA   vtext_inline_scan
      STA   vtext_inline_text
      LDA   $0102,X
      ADC   #$00
      STA   vtext_inline_scan + 1
      STA   vtext_inline_text + 1
      JSR   vtext_inline_find_end
      TSX
      LDA   vtext_inline_scan
      STA   $0101,X
      LDA   vtext_inline_scan + 1
      STA   $0102,X
      LDA   vtext_inline_text
      LDY   vtext_inline_text + 1
      JMP   vtext_puts

; @label I_VTEXT_PRINT_AT
; @kind routine
; @symbol i_vtext_print_at
; @library vtext
; @abi inline
; @summary Set the region cursor from inline X/Y bytes, then print the following zero-terminated text.
; @in Inline: X byte, Y byte, text bytes, then zero.
; @requires VTEXT_LEFT VTEXT_TOP VTEXT_WIDTH VTEXT_HEIGHT VTEXT_COLOR VTEXT_ATTR
; @out A: 0 on success, 1 if the cursor or region is invalid.
; @note The saved return address is advanced past the terminating zero.
i_vtext_print_at:
      TSX
      CLC
      LDA   $0101,X
      ADC   #$01
      STA   vtext_inline_scan
      LDA   $0102,X
      ADC   #$00
      STA   vtext_inline_scan + 1

      LDY   #$00
      LDA   (vtext_inline_scan),Y
      STA   VTEXT_CURX
      INY
      LDA   (vtext_inline_scan),Y
      STA   VTEXT_CURY
      CLC
      LDA   vtext_inline_scan
      ADC   #$02
      STA   vtext_inline_scan
      STA   vtext_inline_text
      LDA   vtext_inline_scan + 1
      ADC   #$00
      STA   vtext_inline_scan + 1
      STA   vtext_inline_text + 1

      JSR   vtext_inline_find_end
      TSX
      LDA   vtext_inline_scan
      STA   $0101,X
      LDA   vtext_inline_scan + 1
      STA   $0102,X
      JSR   vtext_set_cursor
      BNE   @done
      LDA   vtext_inline_text
      LDY   vtext_inline_text + 1
      JMP   vtext_puts
@done:
      RTS

vtext_inline_find_end:
      LDY   #$00
@next:
      LDA   (vtext_inline_scan),Y
      BEQ   @done
      INC   vtext_inline_scan
      BNE   @next
      INC   vtext_inline_scan + 1
      BRA   @next
@done:
      RTS

.endif

; =====================================================================
;  Nova NDK — editui_box.s
;
;  Generic EDITUI box helpers: draw a single/double-line framed box with an
;  optional title, and select its interior body region for text. Split from
;  editui.s so the menu path can use its small frame without linking these.
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

.include "editui.inc"

      .segment "CODE"

      .export editui_draw_box
      .export editui_select_box_body

editui_draw_box:
      LDA   EDITUI_BOXW
      CMP   #2
      BCS   @width_ok
      RTS
@width_ok:
      LDA   EDITUI_BOXH
      CMP   #2
      BCS   @height_ok
      RTS
@height_ok:
      JSR   editui_load_box_chars
      JSR   editui_full_region
      LDA   #EDITUI_COLOR_FRAME
      STA   VTEXT_COLOR
      LDA   EDITUI_BOXX
      STA   VTEXT_CURX
      LDA   EDITUI_BOXY
      STA   VTEXT_CURY
      JSR   vtext_set_cursor
      LDA   EDITUI_CH_TL
      JSR   editui_putc
      LDA   EDITUI_BOXW
      SEC
      SBC   #2
      STA   EDITUI_COL
@top:
      LDA   EDITUI_CH_H
      JSR   editui_putc
      DEC   EDITUI_COL
      BNE   @top
      LDA   EDITUI_CH_TR
      JSR   editui_putc

      LDA   EDITUI_BOXH
      SEC
      SBC   #1
      STA   EDITUI_TMP
      LDA   #1
      STA   EDITUI_ROW
@sides:
      LDA   EDITUI_ROW
      CMP   EDITUI_TMP
      BEQ   @bottom
      LDA   EDITUI_BOXX
      STA   VTEXT_CURX
      LDA   EDITUI_BOXY
      CLC
      ADC   EDITUI_ROW
      STA   VTEXT_CURY
      JSR   vtext_set_cursor
      LDA   EDITUI_CH_V
      JSR   editui_putc
      LDA   EDITUI_BOXX
      CLC
      ADC   EDITUI_BOXW
      SEC
      SBC   #1
      STA   VTEXT_CURX
      LDA   EDITUI_BOXY
      CLC
      ADC   EDITUI_ROW
      STA   VTEXT_CURY
      JSR   vtext_set_cursor
      LDA   EDITUI_CH_V
      JSR   editui_putc
      INC   EDITUI_ROW
      BRA   @sides

@bottom:
      LDA   EDITUI_BOXX
      STA   VTEXT_CURX
      LDA   EDITUI_BOXY
      CLC
      ADC   EDITUI_TMP
      STA   VTEXT_CURY
      JSR   vtext_set_cursor
      LDA   EDITUI_CH_BL
      JSR   editui_putc
      LDA   EDITUI_BOXW
      SEC
      SBC   #2
      STA   EDITUI_COL
@bottom_line:
      LDA   EDITUI_CH_H
      JSR   editui_putc
      DEC   EDITUI_COL
      BNE   @bottom_line
      LDA   EDITUI_CH_BR
      JSR   editui_putc

      LDA   EDITUI_BOX_TITLEL
      ORA   EDITUI_BOX_TITLEH
      BEQ   @done
      LDA   EDITUI_BOX_TITLEL
      STA   EDITUI_PRINTL
      LDA   EDITUI_BOX_TITLEH
      STA   EDITUI_PRINTH
      LDA   EDITUI_BOXX
      CLC
      ADC   #2
      STA   VTEXT_CURX
      LDA   EDITUI_BOXY
      STA   VTEXT_CURY
      JSR   vtext_set_cursor
      LDA   #EDITUI_COLOR_FRAME
      STA   VTEXT_COLOR
      LDA   EDITUI_CH_TITLE_L
      JSR   editui_putc
      LDA   #EDITUI_COLOR_TITLE
      STA   VTEXT_COLOR
      LDA   #' '
      JSR   editui_putc
      LDA   EDITUI_BOX_TITLEL
      LDY   EDITUI_BOX_TITLEH
      JSR   vtext_puts
      LDA   #' '
      JSR   editui_putc
      LDA   #EDITUI_COLOR_FRAME
      STA   VTEXT_COLOR
      LDA   EDITUI_CH_TITLE_R
      JSR   editui_putc
@done:
      RTS

editui_load_box_chars:
      LDA   EDITUI_BOX_STYLE
      CMP   #EDITUI_BOX_DOUBLE
      BEQ   @double
      LDA   #EDITUI_CH_SINGLE_TL
      STA   EDITUI_CH_TL
      LDA   #EDITUI_CH_SINGLE_TR
      STA   EDITUI_CH_TR
      LDA   #EDITUI_CH_SINGLE_BL
      STA   EDITUI_CH_BL
      LDA   #EDITUI_CH_SINGLE_BR
      STA   EDITUI_CH_BR
      LDA   #EDITUI_CH_SINGLE_H
      STA   EDITUI_CH_H
      LDA   #EDITUI_CH_SINGLE_V
      STA   EDITUI_CH_V
      LDA   #EDITUI_CH_SINGLE_TITLE_L
      STA   EDITUI_CH_TITLE_L
      LDA   #EDITUI_CH_SINGLE_TITLE_R
      STA   EDITUI_CH_TITLE_R
      RTS
@double:
      LDA   #EDITUI_CH_DOUBLE_TL
      STA   EDITUI_CH_TL
      LDA   #EDITUI_CH_DOUBLE_TR
      STA   EDITUI_CH_TR
      LDA   #EDITUI_CH_DOUBLE_BL
      STA   EDITUI_CH_BL
      LDA   #EDITUI_CH_DOUBLE_BR
      STA   EDITUI_CH_BR
      LDA   #EDITUI_CH_DOUBLE_H
      STA   EDITUI_CH_H
      LDA   #EDITUI_CH_DOUBLE_V
      STA   EDITUI_CH_V
      LDA   #EDITUI_CH_DOUBLE_TITLE_L
      STA   EDITUI_CH_TITLE_L
      LDA   #EDITUI_CH_DOUBLE_TITLE_R
      STA   EDITUI_CH_TITLE_R
      RTS

editui_select_box_body:
      LDA   EDITUI_BOXW
      CMP   #3
      BCS   @width_ok
      RTS
@width_ok:
      LDA   EDITUI_BOXH
      CMP   #3
      BCS   @height_ok
      RTS
@height_ok:
      LDA   EDITUI_BOXX
      CLC
      ADC   #1
      STA   VTEXT_LEFT
      LDA   EDITUI_BOXY
      CLC
      ADC   #1
      STA   VTEXT_TOP
      LDA   EDITUI_BOXW
      SEC
      SBC   #2
      STA   VTEXT_WIDTH
      LDA   EDITUI_BOXH
      SEC
      SBC   #2
      STA   VTEXT_HEIGHT
      STZ   VTEXT_CURX
      STZ   VTEXT_CURY
      LDA   #EDITUI_COLOR_PANEL
      STA   VTEXT_COLOR
      STZ   VTEXT_ATTR
      LDA   #VTEXT_FLAG_WRAP
      STA   VTEXT_FLAGS
      JMP   vtext_set_cursor

editui_putc:
      STA   VTEXT_CHAR
      JMP   vtext_put_char

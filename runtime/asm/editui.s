; Shared Nova editor-shell UI helpers.

.include "editui.inc"
.include "vtext.s"

.ifndef EDITUI_IMPLEMENTATION_INCLUDED
EDITUI_IMPLEMENTATION_INCLUDED = 1

EDITUI_MENU_SAVE_MAX_W = EDITUI_SCREEN_COLS
EDITUI_MENU_SAVE_MAX_H = 16
EDITUI_MENU_SAVE_BYTES = EDITUI_MENU_SAVE_MAX_W * EDITUI_MENU_SAVE_MAX_H

      .segment "BSS"

EDITUI_TITLEL:      .res 1
EDITUI_TITLEH:      .res 1
EDITUI_MENUL:       .res 1
EDITUI_MENUH:       .res 1
EDITUI_MENUSL:      .res 1
EDITUI_MENUSH:      .res 1
EDITUI_HELPL:       .res 1
EDITUI_HELPH:       .res 1
EDITUI_STATUSL:     .res 1
EDITUI_STATUSH:     .res 1
EDITUI_BOXX:        .res 1
EDITUI_BOXY:        .res 1
EDITUI_BOXW:        .res 1
EDITUI_BOXH:        .res 1
EDITUI_BOX_TITLEL:  .res 1
EDITUI_BOX_TITLEH:  .res 1
EDITUI_BOX_STYLE:   .res 1
EDITUI_PRINTX:      .res 1
EDITUI_PRINTY:      .res 1
EDITUI_PRINTL:      .res 1
EDITUI_PRINTH:      .res 1
EDITUI_FILLROW:     .res 1
EDITUI_TMP:         .res 1
EDITUI_COL:         .res 1
EDITUI_ROW:         .res 1
EDITUI_LEN:         .res 1
EDITUI_PRINT_COLOR: .res 1
EDITUI_HOT_COLOR:   .res 1
EDITUI_CH_TL:       .res 1
EDITUI_CH_TR:       .res 1
EDITUI_CH_BL:       .res 1
EDITUI_CH_BR:       .res 1
EDITUI_CH_H:        .res 1
EDITUI_CH_V:        .res 1
EDITUI_CH_TITLE_L:  .res 1
EDITUI_CH_TITLE_R:  .res 1
EDITUI_MENU_INDEX:  .res 1
EDITUI_MENU_COUNT:  .res 1
EDITUI_MENU_X:      .res 1
EDITUI_MENU_Y:      .res 1
EDITUI_MENU_WIDTH:  .res 1
EDITUI_MENU_ITEMS_L:.res 1
EDITUI_MENU_ITEMS_H:.res 1
EDITUI_MENU_ITEM_COUNT: .res 1
EDITUI_MENU_SELECTED: .res 1
EDITUI_MENU_ACTIVE: .res 1
EDITUI_MENU_CMD:    .res 1
EDITUI_MENU_KEY:    .res 1
EDITUI_MENU_SAVE_VALID: .res 1
EDITUI_MENU_SAVE_X: .res 1
EDITUI_MENU_SAVE_Y: .res 1
EDITUI_MENU_SAVE_W: .res 1
EDITUI_MENU_SAVE_H: .res 1
EDITUI_MENU_BUF_L: .res 1
EDITUI_MENU_BUF_H: .res 1
EDITUI_SAVE_BOXX:   .res 1
EDITUI_SAVE_BOXY:   .res 1
EDITUI_SAVE_BOXW:   .res 1
EDITUI_SAVE_BOXH:   .res 1
EDITUI_SAVE_TITL:   .res 1
EDITUI_SAVE_TITH:   .res 1
EDITUI_SAVE_STYLE:  .res 1
EDITUI_MENU_SAVE_CHAR: .res EDITUI_MENU_SAVE_BYTES
EDITUI_MENU_SAVE_COLOR: .res EDITUI_MENU_SAVE_BYTES
EDITUI_MENU_SAVE_ATTR: .res EDITUI_MENU_SAVE_BYTES

      .segment "ZEROPAGE"

EDITUI_SCANL:       .res 1
EDITUI_SCANH:       .res 1
EDITUI_MENU_PTRL:   .res 1
EDITUI_MENU_PTRH:   .res 1
EDITUI_MENU_ITEML:  .res 1
EDITUI_MENU_ITEMH:  .res 1

      .segment "CODE"

      .export editui_init
      .export editui_full_region
      .export editui_clear_screen
      .export editui_shutdown
      .export editui_draw_shell
      .export editui_draw_status
      .export editui_draw_box
      .export editui_select_box_body
      .export editui_print_ptr
      .export editui_print_marked
      .export editui_menu_open_hotkey
      .export editui_default_menus

editui_ok:
      LDA   #EDITUI_OK
      RTS

editui_init:
      LDA   #$00
      STA   VGC_MODE
      LDA   #EDITUI_PALETTE_MODE
      STA   VGC_PALETTE
      LDA   #EDITUI_COLOR_BG
      STA   VGC_BGCOL
      STA   VGC_BORDER
      LDA   #EDITUI_COLOR_FG
      STA   VGC_FGCOL
      STZ   VGC_CURSEN
      STZ   EDITUI_BOX_STYLE
      STZ   EDITUI_MENUL
      STZ   EDITUI_MENUH
      STZ   EDITUI_MENU_SAVE_VALID
      LDA   #<editui_default_menus
      STA   EDITUI_MENUSL
      LDA   #>editui_default_menus
      STA   EDITUI_MENUSH
      JMP   editui_ok

editui_full_region:
      STZ   VTEXT_LEFT
      STZ   VTEXT_TOP
      LDA   #EDITUI_SCREEN_COLS
      STA   VTEXT_WIDTH
      LDA   #EDITUI_SCREEN_ROWS
      STA   VTEXT_HEIGHT
      STZ   VTEXT_CURX
      STZ   VTEXT_CURY
      LDA   #EDITUI_COLOR_SHELL
      STA   VTEXT_COLOR
      STZ   VTEXT_ATTR
      STZ   VTEXT_FLAGS
      RTS

editui_clear_screen:
      JSR   editui_full_region
      LDA   #' '
      STA   VTEXT_CHAR
      JMP   vtext_clear_region

editui_shutdown:
      LDA   #$0C
      STA   VGC_CHAROUT
@wait:
      LDA   VGC_CMD
      AND   #$01
      BNE   @wait
      STZ   VGC_CURSX
      STZ   VGC_CURSY
      LDA   #$01
      STA   VGC_CURSEN
      RTS

editui_fill_row:
      LDA   VTEXT_COLOR
      PHA
      JSR   editui_full_region
      PLA
      STA   VTEXT_COLOR
      LDA   EDITUI_FILLROW
      STA   VTEXT_TOP
      LDA   #1
      STA   VTEXT_HEIGHT
      STZ   VTEXT_CURX
      STZ   VTEXT_CURY
      LDA   #' '
      STA   VTEXT_CHAR
      JMP   vtext_clear_region

editui_print_ptr:
      LDA   VTEXT_COLOR
      PHA
      JSR   editui_full_region
      PLA
      STA   VTEXT_COLOR
      LDA   EDITUI_PRINTX
      STA   VTEXT_CURX
      LDA   EDITUI_PRINTY
      STA   VTEXT_CURY
      JSR   vtext_set_cursor
      LDA   EDITUI_PRINTL
      LDY   EDITUI_PRINTH
      JMP   vtext_puts

editui_print_marked:
      LDA   EDITUI_PRINTL
      STA   EDITUI_SCANL
      LDA   EDITUI_PRINTH
      STA   EDITUI_SCANH
      JSR   editui_full_region
      LDA   EDITUI_PRINT_COLOR
      STA   VTEXT_COLOR
      LDA   EDITUI_PRINTX
      STA   VTEXT_CURX
      LDA   EDITUI_PRINTY
      STA   VTEXT_CURY
      JSR   vtext_set_cursor
@loop:
      LDY   #$00
      LDA   (EDITUI_SCANL),Y
      BEQ   @done
      CMP   #'&'
      BNE   @normal
      JSR   editui_inc_scan
      LDY   #$00
      LDA   (EDITUI_SCANL),Y
      BEQ   @done
      STA   VTEXT_CHAR
      LDA   EDITUI_HOT_COLOR
      STA   VTEXT_COLOR
      JSR   vtext_put_char
      LDA   EDITUI_PRINT_COLOR
      STA   VTEXT_COLOR
      JSR   editui_inc_scan
      BRA   @loop
@normal:
      STA   VTEXT_CHAR
      JSR   vtext_put_char
      JSR   editui_inc_scan
      BRA   @loop
@done:
      RTS

editui_inc_scan:
      INC   EDITUI_SCANL
      BNE   @done
      INC   EDITUI_SCANH
@done:
      RTS

editui_strlen:
      LDA   EDITUI_PRINTL
      STA   EDITUI_SCANL
      LDA   EDITUI_PRINTH
      STA   EDITUI_SCANH
      STZ   EDITUI_LEN
@loop:
      LDY   #$00
      LDA   (EDITUI_SCANL),Y
      BEQ   @done
      CMP   #'&'
      BEQ   @skip
      INC   EDITUI_LEN
@skip:
      JSR   editui_inc_scan
      BRA   @loop
@done:
      LDA   EDITUI_LEN
      RTS

editui_center_ptr:
      JSR   editui_strlen
      STA   EDITUI_TMP
      LDA   #EDITUI_SCREEN_COLS
      SEC
      SBC   EDITUI_TMP
      LSR
      STA   EDITUI_PRINTX
      JMP   editui_print_ptr

editui_print_at_2:
      LDA   #2
      STA   EDITUI_PRINTX
      JMP   editui_print_ptr

editui_draw_shell:
      JSR   editui_clear_screen
      JSR   editui_draw_menu
      JSR   editui_draw_title_band
      JMP   editui_draw_status

editui_draw_menu:
      LDA   #EDITUI_COLOR_MENU
      STA   VTEXT_COLOR
      STZ   EDITUI_FILLROW
      JSR   editui_fill_row
      LDA   EDITUI_MENUSL
      ORA   EDITUI_MENUSH
      BEQ   @legacy
      JMP   editui_draw_menu_table
@legacy:
      LDA   EDITUI_MENUL
      ORA   EDITUI_MENUH
      BEQ   @done
      LDA   EDITUI_MENUL
      STA   EDITUI_PRINTL
      LDA   EDITUI_MENUH
      STA   EDITUI_PRINTH
      LDA   #2
      STA   EDITUI_PRINTX
      STZ   EDITUI_PRINTY
      LDA   #EDITUI_COLOR_MENU
      STA   EDITUI_PRINT_COLOR
      LDA   #EDITUI_COLOR_MENU_HOT
      STA   EDITUI_HOT_COLOR
      JSR   editui_print_marked
@done:
      RTS

editui_draw_menu_table:
      LDA   EDITUI_MENUSL
      STA   EDITUI_MENU_PTRL
      LDA   EDITUI_MENUSH
      STA   EDITUI_MENU_PTRH
      LDY   #$00
      LDA   (EDITUI_MENU_PTRL),Y
      STA   EDITUI_MENU_COUNT
      STZ   EDITUI_MENU_INDEX
      LDA   #2
      STA   EDITUI_MENU_X
@loop:
      LDA   EDITUI_MENU_INDEX
      CMP   EDITUI_MENU_COUNT
      BEQ   @done
      JSR   editui_menu_entry_ptr
      LDY   #2
      LDA   (EDITUI_MENU_PTRL),Y
      STA   EDITUI_PRINTL
      INY
      LDA   (EDITUI_MENU_PTRL),Y
      STA   EDITUI_PRINTH
      LDA   EDITUI_MENU_X
      STA   EDITUI_PRINTX
      STZ   EDITUI_PRINTY
      LDA   #EDITUI_COLOR_MENU
      STA   EDITUI_PRINT_COLOR
      LDA   #EDITUI_COLOR_MENU_HOT
      STA   EDITUI_HOT_COLOR
      JSR   editui_print_marked
      JSR   editui_strlen
      CLC
      ADC   EDITUI_MENU_X
      CLC
      ADC   #3
      STA   EDITUI_MENU_X
      INC   EDITUI_MENU_INDEX
      BRA   @loop
@done:
      RTS

editui_draw_title_band:
      LDA   EDITUI_TITLEL
      ORA   EDITUI_TITLEH
      BNE   @draw
      LDA   EDITUI_HELPL
      ORA   EDITUI_HELPH
      BEQ   @done
@draw:
      JSR   editui_push_box_state
      LDA   #1
      STA   EDITUI_BOXX
      LDA   #2
      STA   EDITUI_BOXY
      LDA   #78
      STA   EDITUI_BOXW
      LDA   #4
      STA   EDITUI_BOXH
      LDA   #EDITUI_BOX_DOUBLE
      STA   EDITUI_BOX_STYLE
      STZ   EDITUI_BOX_TITLEL
      STZ   EDITUI_BOX_TITLEH
      JSR   editui_draw_box

      LDA   EDITUI_TITLEL
      ORA   EDITUI_TITLEH
      BEQ   @help
      LDA   EDITUI_TITLEL
      STA   EDITUI_PRINTL
      LDA   EDITUI_TITLEH
      STA   EDITUI_PRINTH
      LDA   #3
      STA   EDITUI_PRINTY
      LDA   #EDITUI_COLOR_TITLE
      STA   VTEXT_COLOR
      JSR   editui_center_ptr

@help:
      LDA   EDITUI_HELPL
      ORA   EDITUI_HELPH
      BEQ   @restore
      LDA   EDITUI_HELPL
      STA   EDITUI_PRINTL
      LDA   EDITUI_HELPH
      STA   EDITUI_PRINTH
      LDA   #4
      STA   EDITUI_PRINTY
      LDA   #EDITUI_COLOR_DIM
      STA   VTEXT_COLOR
      JSR   editui_center_ptr

@restore:
      JSR   editui_pop_box_state
@done:
      RTS

editui_draw_status:
      JSR   editui_full_region
      LDA   #1
      STA   VTEXT_LEFT
      LDA   #EDITUI_STATUS_ROW
      STA   VTEXT_TOP
      LDA   #78
      STA   VTEXT_WIDTH
      LDA   #1
      STA   VTEXT_HEIGHT
      STZ   VTEXT_CURX
      STZ   VTEXT_CURY
      LDA   #EDITUI_COLOR_STATUS
      STA   VTEXT_COLOR
      STZ   VTEXT_ATTR
      STZ   VTEXT_FLAGS
      LDA   #' '
      STA   VTEXT_CHAR
      JSR   vtext_clear_region
      LDA   EDITUI_STATUSL
      ORA   EDITUI_STATUSH
      BEQ   @done
      LDA   EDITUI_STATUSL
      STA   EDITUI_PRINTL
      LDA   EDITUI_STATUSH
      STA   EDITUI_PRINTH
      LDA   #2
      STA   EDITUI_PRINTX
      LDA   #EDITUI_STATUS_ROW
      STA   EDITUI_PRINTY
      LDA   #EDITUI_COLOR_STATUS
      STA   EDITUI_PRINT_COLOR
      LDA   #EDITUI_COLOR_STATUS_HOT
      STA   EDITUI_HOT_COLOR
      JSR   editui_print_marked
@done:
      RTS

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

editui_menu_open_hotkey:
      STA   EDITUI_MENU_KEY
      JSR   editui_menu_normalize_key
      STZ   EDITUI_MENU_CMD
      LDA   EDITUI_MENUSL
      ORA   EDITUI_MENUSH
      BNE   @has_table
      LDA   #EDITUI_CMD_NONE
      RTS
@has_table:
      LDA   EDITUI_MENUSL
      STA   EDITUI_MENU_PTRL
      LDA   EDITUI_MENUSH
      STA   EDITUI_MENU_PTRH
      LDY   #$00
      LDA   (EDITUI_MENU_PTRL),Y
      STA   EDITUI_MENU_COUNT
      STZ   EDITUI_MENU_INDEX
      LDA   #2
      STA   EDITUI_MENU_X
@loop:
      LDA   EDITUI_MENU_INDEX
      CMP   EDITUI_MENU_COUNT
      BEQ   @none
      JSR   editui_menu_entry_ptr
      LDY   #0
      LDA   (EDITUI_MENU_PTRL),Y
      CMP   EDITUI_MENU_KEY
      BEQ   @found
      LDY   #2
      LDA   (EDITUI_MENU_PTRL),Y
      STA   EDITUI_PRINTL
      INY
      LDA   (EDITUI_MENU_PTRL),Y
      STA   EDITUI_PRINTH
      JSR   editui_strlen
      CLC
      ADC   EDITUI_MENU_X
      CLC
      ADC   #3
      STA   EDITUI_MENU_X
      INC   EDITUI_MENU_INDEX
      BRA   @loop
@found:
      LDA   EDITUI_MENU_INDEX
      STA   EDITUI_MENU_ACTIVE
      LDY   #1
      LDA   (EDITUI_MENU_PTRL),Y
      STA   EDITUI_MENU_WIDTH
      LDY   #4
      LDA   (EDITUI_MENU_PTRL),Y
      STA   EDITUI_MENU_ITEMS_L
      INY
      LDA   (EDITUI_MENU_PTRL),Y
      STA   EDITUI_MENU_ITEMS_H
      JSR   editui_menu_open_current
      LDA   EDITUI_MENU_CMD
      RTS
@none:
      LDA   #EDITUI_CMD_NONE
      RTS

editui_menu_entry_ptr:
      LDA   EDITUI_MENUSL
      STA   EDITUI_MENU_PTRL
      LDA   EDITUI_MENUSH
      STA   EDITUI_MENU_PTRH
      INC   EDITUI_MENU_PTRL
      BNE   @skip_hi
      INC   EDITUI_MENU_PTRH
@skip_hi:
      LDX   EDITUI_MENU_INDEX
@loop:
      CPX   #0
      BEQ   @done
      CLC
      LDA   EDITUI_MENU_PTRL
      ADC   #EDITUI_MENU_ENTRY_SIZE
      STA   EDITUI_MENU_PTRL
      BCC   @next
      INC   EDITUI_MENU_PTRH
@next:
      DEX
      BRA   @loop
@done:
      RTS

editui_menu_item_ptr:
      LDA   EDITUI_MENU_ITEMS_L
      STA   EDITUI_MENU_ITEML
      LDA   EDITUI_MENU_ITEMS_H
      STA   EDITUI_MENU_ITEMH
      INC   EDITUI_MENU_ITEML
      BNE   @skip_hi
      INC   EDITUI_MENU_ITEMH
@skip_hi:
      LDX   EDITUI_MENU_INDEX
@loop:
      CPX   #0
      BEQ   @done
      CLC
      LDA   EDITUI_MENU_ITEML
      ADC   #EDITUI_MENU_ITEM_SIZE
      STA   EDITUI_MENU_ITEML
      BCC   @next
      INC   EDITUI_MENU_ITEMH
@next:
      DEX
      BRA   @loop
@done:
      RTS

editui_menu_open_current:
      LDA   EDITUI_MENU_ITEMS_L
      STA   EDITUI_MENU_ITEML
      LDA   EDITUI_MENU_ITEMS_H
      STA   EDITUI_MENU_ITEMH
      LDY   #$00
      LDA   (EDITUI_MENU_ITEML),Y
      STA   EDITUI_MENU_ITEM_COUNT
      STZ   EDITUI_MENU_SELECTED
      STZ   EDITUI_MENU_CMD
@redraw:
      JSR   editui_menu_draw_dropdown
@wait_key:
      LDA   VGC_CHARIN
      BEQ   @wait_key
      CMP   #EDITUI_KEY_ESC
      BEQ   @cancel
      CMP   #EDITUI_KEY_DOWN
      BEQ   @down
      CMP   #EDITUI_KEY_UP
      BEQ   @up
      CMP   #EDITUI_KEY_RIGHT
      BEQ   @right
      CMP   #EDITUI_KEY_LEFT
      BEQ   @left
      CMP   #EDITUI_KEY_ENTER
      BEQ   @choose
      STA   EDITUI_MENU_KEY
      JSR   editui_menu_normalize_key
      JSR   editui_menu_find_item_hotkey
      LDA   EDITUI_MENU_CMD
      BNE   @done
      BRA   @wait_key
@down:
      LDA   EDITUI_MENU_SELECTED
      STA   EDITUI_TMP
      INC   EDITUI_MENU_SELECTED
      LDA   EDITUI_MENU_SELECTED
      CMP   EDITUI_MENU_ITEM_COUNT
      BCC   @redraw_selection
      STZ   EDITUI_MENU_SELECTED
      BRA   @redraw_selection
@up:
      LDA   EDITUI_MENU_SELECTED
      STA   EDITUI_TMP
      LDA   EDITUI_MENU_SELECTED
      BNE   @dec
      LDA   EDITUI_MENU_ITEM_COUNT
      STA   EDITUI_MENU_SELECTED
@dec:
      DEC   EDITUI_MENU_SELECTED
@redraw_selection:
      JSR   editui_menu_redraw_selection
      BRA   @wait_key
@right:
      JSR   editui_menu_next_active
      JSR   editui_menu_switch_active
      BRA   @wait_key
@left:
      JSR   editui_menu_prev_active
      JSR   editui_menu_switch_active
      BRA   @wait_key
@choose:
      JSR   editui_menu_select_current
      BRA   @done
@cancel:
      STZ   EDITUI_MENU_CMD
@done:
      JSR   editui_menu_restore_under
      RTS

editui_menu_redraw_selection:
      LDA   EDITUI_TMP
      STA   EDITUI_MENU_INDEX
      JSR   editui_menu_draw_item
      LDA   EDITUI_MENU_SELECTED
      STA   EDITUI_MENU_INDEX
      JMP   editui_menu_draw_item

editui_menu_next_active:
      INC   EDITUI_MENU_ACTIVE
      LDA   EDITUI_MENU_ACTIVE
      CMP   EDITUI_MENU_COUNT
      BCC   @done
      STZ   EDITUI_MENU_ACTIVE
@done:
      RTS

editui_menu_prev_active:
      LDA   EDITUI_MENU_ACTIVE
      BNE   @dec
      LDA   EDITUI_MENU_COUNT
      STA   EDITUI_MENU_ACTIVE
@dec:
      DEC   EDITUI_MENU_ACTIVE
      RTS

editui_menu_switch_active:
      JSR   editui_menu_restore_under
      JSR   editui_menu_load_active
      JMP   editui_menu_draw_dropdown

editui_menu_load_active:
      LDA   EDITUI_MENUSL
      STA   EDITUI_MENU_PTRL
      LDA   EDITUI_MENUSH
      STA   EDITUI_MENU_PTRH
      LDY   #$00
      LDA   (EDITUI_MENU_PTRL),Y
      STA   EDITUI_MENU_COUNT
      STZ   EDITUI_MENU_INDEX
      LDA   #2
      STA   EDITUI_MENU_X
@loop:
      LDA   EDITUI_MENU_INDEX
      CMP   EDITUI_MENU_ACTIVE
      BEQ   @found
      JSR   editui_menu_entry_ptr
      LDY   #2
      LDA   (EDITUI_MENU_PTRL),Y
      STA   EDITUI_PRINTL
      INY
      LDA   (EDITUI_MENU_PTRL),Y
      STA   EDITUI_PRINTH
      JSR   editui_strlen
      CLC
      ADC   EDITUI_MENU_X
      CLC
      ADC   #3
      STA   EDITUI_MENU_X
      INC   EDITUI_MENU_INDEX
      BRA   @loop
@found:
      JSR   editui_menu_entry_ptr
      LDY   #1
      LDA   (EDITUI_MENU_PTRL),Y
      STA   EDITUI_MENU_WIDTH
      LDY   #4
      LDA   (EDITUI_MENU_PTRL),Y
      STA   EDITUI_MENU_ITEMS_L
      INY
      LDA   (EDITUI_MENU_PTRL),Y
      STA   EDITUI_MENU_ITEMS_H
      LDA   EDITUI_MENU_ITEMS_L
      STA   EDITUI_MENU_ITEML
      LDA   EDITUI_MENU_ITEMS_H
      STA   EDITUI_MENU_ITEMH
      LDY   #$00
      LDA   (EDITUI_MENU_ITEML),Y
      STA   EDITUI_MENU_ITEM_COUNT
      STZ   EDITUI_MENU_SELECTED
      STZ   EDITUI_MENU_CMD
      RTS

editui_menu_draw_dropdown:
      JSR   editui_push_box_state
      LDA   EDITUI_MENU_X
      STA   EDITUI_BOXX
      LDA   #1
      STA   EDITUI_BOXY
      LDA   EDITUI_MENU_WIDTH
      STA   EDITUI_BOXW
      LDA   EDITUI_MENU_ITEM_COUNT
      CLC
      ADC   #2
      STA   EDITUI_BOXH
      LDA   #EDITUI_BOX_SINGLE
      STA   EDITUI_BOX_STYLE
      STZ   EDITUI_BOX_TITLEL
      STZ   EDITUI_BOX_TITLEH
      JSR   editui_menu_save_under
      JSR   editui_draw_box
      STZ   EDITUI_MENU_INDEX
@loop:
      LDA   EDITUI_MENU_INDEX
      CMP   EDITUI_MENU_ITEM_COUNT
      BEQ   @done
      JSR   editui_menu_draw_item
      INC   EDITUI_MENU_INDEX
      BRA   @loop
@done:
      JSR   editui_pop_box_state
      RTS

editui_menu_save_under:
      STZ   EDITUI_MENU_SAVE_VALID
      LDA   EDITUI_MENU_WIDTH
      BEQ   @done
      CMP   #EDITUI_MENU_SAVE_MAX_W + 1
      BCS   @done
      STA   EDITUI_MENU_SAVE_W
      LDA   EDITUI_MENU_ITEM_COUNT
      CLC
      ADC   #2
      BEQ   @done
      CMP   #EDITUI_MENU_SAVE_MAX_H + 1
      BCS   @done
      STA   EDITUI_MENU_SAVE_H
      LDA   EDITUI_MENU_X
      STA   EDITUI_MENU_SAVE_X
      LDA   #1
      STA   EDITUI_MENU_SAVE_Y
      JSR   editui_menu_select_saved_region
      BNE   @done
      JSR   vtext_calc_region_addr

      LDA   #<EDITUI_MENU_SAVE_CHAR
      STA   EDITUI_MENU_BUF_L
      LDA   #>EDITUI_MENU_SAVE_CHAR
      STA   EDITUI_MENU_BUF_H
      LDA   #BLT_SPACE_VGC_CHAR
      JSR   editui_menu_save_plane
      BNE   @done

      LDA   #<EDITUI_MENU_SAVE_COLOR
      STA   EDITUI_MENU_BUF_L
      LDA   #>EDITUI_MENU_SAVE_COLOR
      STA   EDITUI_MENU_BUF_H
      LDA   #BLT_SPACE_VGC_COLOR
      JSR   editui_menu_save_plane
      BNE   @done

      LDA   #<EDITUI_MENU_SAVE_ATTR
      STA   EDITUI_MENU_BUF_L
      LDA   #>EDITUI_MENU_SAVE_ATTR
      STA   EDITUI_MENU_BUF_H
      LDA   #BLT_SPACE_VGC_TEXTATTR
      JSR   editui_menu_save_plane
      BNE   @done

      LDA   #1
      STA   EDITUI_MENU_SAVE_VALID
@done:
      RTS

editui_menu_restore_under:
      LDA   EDITUI_MENU_SAVE_VALID
      BEQ   @done
      JSR   editui_menu_select_saved_region
      BNE   @clear
      JSR   vtext_calc_region_addr

      LDA   #<EDITUI_MENU_SAVE_CHAR
      STA   EDITUI_MENU_BUF_L
      LDA   #>EDITUI_MENU_SAVE_CHAR
      STA   EDITUI_MENU_BUF_H
      LDA   #BLT_SPACE_VGC_CHAR
      JSR   editui_menu_restore_plane
      BNE   @clear

      LDA   #<EDITUI_MENU_SAVE_COLOR
      STA   EDITUI_MENU_BUF_L
      LDA   #>EDITUI_MENU_SAVE_COLOR
      STA   EDITUI_MENU_BUF_H
      LDA   #BLT_SPACE_VGC_COLOR
      JSR   editui_menu_restore_plane
      BNE   @clear

      LDA   #<EDITUI_MENU_SAVE_ATTR
      STA   EDITUI_MENU_BUF_L
      LDA   #>EDITUI_MENU_SAVE_ATTR
      STA   EDITUI_MENU_BUF_H
      LDA   #BLT_SPACE_VGC_TEXTATTR
      JSR   editui_menu_restore_plane
@clear:
      STZ   EDITUI_MENU_SAVE_VALID
@done:
      RTS

editui_menu_select_saved_region:
      LDA   EDITUI_MENU_SAVE_X
      STA   VTEXT_LEFT
      LDA   EDITUI_MENU_SAVE_Y
      STA   VTEXT_TOP
      LDA   EDITUI_MENU_SAVE_W
      STA   VTEXT_WIDTH
      LDA   EDITUI_MENU_SAVE_H
      STA   VTEXT_HEIGHT
      STZ   VTEXT_CURX
      STZ   VTEXT_CURY
      JMP   vtext_validate_region

editui_menu_save_plane:
      STA   BLT_SRCSPACE
      LDA   #BLT_SPACE_CPU
      STA   BLT_DSTSPACE
      LDA   VTEXT_ADDRL
      STA   BLT_SRCL
      LDA   VTEXT_ADDRH
      STA   BLT_SRCM
      STZ   BLT_SRCH
      LDA   EDITUI_MENU_BUF_L
      STA   BLT_DSTL
      LDA   EDITUI_MENU_BUF_H
      STA   BLT_DSTM
      STZ   BLT_DSTH
      LDA   EDITUI_MENU_SAVE_W
      STA   BLT_WIDTHL
      STZ   BLT_WIDTHH
      LDA   EDITUI_MENU_SAVE_H
      STA   BLT_HEIGHTL
      STZ   BLT_HEIGHTH
      LDA   #<EDITUI_SCREEN_COLS
      STA   BLT_SRCSTRL
      LDA   #>EDITUI_SCREEN_COLS
      STA   BLT_SRCSTRH
      LDA   EDITUI_MENU_SAVE_W
      STA   BLT_DSTSTRL
      STZ   BLT_DSTSTRH
      JMP   blitter_start_copy

editui_menu_restore_plane:
      STA   BLT_DSTSPACE
      LDA   #BLT_SPACE_CPU
      STA   BLT_SRCSPACE
      LDA   EDITUI_MENU_BUF_L
      STA   BLT_SRCL
      LDA   EDITUI_MENU_BUF_H
      STA   BLT_SRCM
      STZ   BLT_SRCH
      LDA   VTEXT_ADDRL
      STA   BLT_DSTL
      LDA   VTEXT_ADDRH
      STA   BLT_DSTM
      STZ   BLT_DSTH
      LDA   EDITUI_MENU_SAVE_W
      STA   BLT_WIDTHL
      STZ   BLT_WIDTHH
      LDA   EDITUI_MENU_SAVE_H
      STA   BLT_HEIGHTL
      STZ   BLT_HEIGHTH
      LDA   EDITUI_MENU_SAVE_W
      STA   BLT_SRCSTRL
      STZ   BLT_SRCSTRH
      LDA   #<EDITUI_SCREEN_COLS
      STA   BLT_DSTSTRL
      LDA   #>EDITUI_SCREEN_COLS
      STA   BLT_DSTSTRH
      JMP   blitter_start_copy

editui_menu_draw_item:
      JSR   editui_menu_item_ptr
      LDA   EDITUI_MENU_X
      CLC
      ADC   #1
      STA   VTEXT_LEFT
      LDA   EDITUI_MENU_INDEX
      CLC
      ADC   #2
      STA   VTEXT_TOP
      LDA   EDITUI_MENU_WIDTH
      SEC
      SBC   #2
      STA   VTEXT_WIDTH
      LDA   #1
      STA   VTEXT_HEIGHT
      STZ   VTEXT_CURX
      STZ   VTEXT_CURY
      LDA   EDITUI_MENU_INDEX
      CMP   EDITUI_MENU_SELECTED
      BEQ   @selected
      LDA   #EDITUI_COLOR_MENU
      BRA   @have_color
@selected:
      LDA   #EDITUI_COLOR_MENU_SEL
@have_color:
      STA   VTEXT_COLOR
      STZ   VTEXT_ATTR
      STZ   VTEXT_FLAGS
      LDA   #' '
      STA   VTEXT_CHAR
      JSR   vtext_clear_region

      LDY   #2
      LDA   (EDITUI_MENU_ITEML),Y
      STA   EDITUI_PRINTL
      INY
      LDA   (EDITUI_MENU_ITEML),Y
      STA   EDITUI_PRINTH
      LDA   EDITUI_MENU_X
      CLC
      ADC   #2
      STA   EDITUI_PRINTX
      LDA   EDITUI_MENU_INDEX
      CLC
      ADC   #2
      STA   EDITUI_PRINTY
      LDA   EDITUI_MENU_INDEX
      CMP   EDITUI_MENU_SELECTED
      BEQ   @selected_text
      LDA   #EDITUI_COLOR_MENU
      STA   EDITUI_PRINT_COLOR
      LDA   #EDITUI_COLOR_MENU_HOT
      STA   EDITUI_HOT_COLOR
      JMP   editui_print_marked
@selected_text:
      LDA   #EDITUI_COLOR_MENU_SEL
      STA   EDITUI_PRINT_COLOR
      LDA   #EDITUI_COLOR_MENU_SEL_HOT
      STA   EDITUI_HOT_COLOR
      JMP   editui_print_marked

editui_menu_select_current:
      LDA   EDITUI_MENU_SELECTED
      STA   EDITUI_MENU_INDEX
      JSR   editui_menu_item_ptr
      LDY   #0
      LDA   (EDITUI_MENU_ITEML),Y
      STA   EDITUI_MENU_CMD
      RTS

editui_menu_find_item_hotkey:
      STZ   EDITUI_MENU_INDEX
@loop:
      LDA   EDITUI_MENU_INDEX
      CMP   EDITUI_MENU_ITEM_COUNT
      BEQ   @none
      JSR   editui_menu_item_ptr
      LDY   #1
      LDA   (EDITUI_MENU_ITEML),Y
      CMP   EDITUI_MENU_KEY
      BEQ   @found
      INC   EDITUI_MENU_INDEX
      BRA   @loop
@found:
      LDA   EDITUI_MENU_INDEX
      STA   EDITUI_MENU_SELECTED
      JMP   editui_menu_select_current
@none:
      STZ   EDITUI_MENU_CMD
      RTS

editui_menu_normalize_key:
      LDA   EDITUI_MENU_KEY
      CMP   #'A'
      BCC   @done
      CMP   #'Z' + 1
      BCS   @done
      ORA   #$20
      STA   EDITUI_MENU_KEY
@done:
      RTS

editui_push_box_state:
      LDA   EDITUI_BOXX
      STA   EDITUI_SAVE_BOXX
      LDA   EDITUI_BOXY
      STA   EDITUI_SAVE_BOXY
      LDA   EDITUI_BOXW
      STA   EDITUI_SAVE_BOXW
      LDA   EDITUI_BOXH
      STA   EDITUI_SAVE_BOXH
      LDA   EDITUI_BOX_TITLEL
      STA   EDITUI_SAVE_TITL
      LDA   EDITUI_BOX_TITLEH
      STA   EDITUI_SAVE_TITH
      LDA   EDITUI_BOX_STYLE
      STA   EDITUI_SAVE_STYLE
      RTS

editui_pop_box_state:
      LDA   EDITUI_SAVE_STYLE
      STA   EDITUI_BOX_STYLE
      LDA   EDITUI_SAVE_TITH
      STA   EDITUI_BOX_TITLEH
      LDA   EDITUI_SAVE_TITL
      STA   EDITUI_BOX_TITLEL
      LDA   EDITUI_SAVE_BOXH
      STA   EDITUI_BOXH
      LDA   EDITUI_SAVE_BOXW
      STA   EDITUI_BOXW
      LDA   EDITUI_SAVE_BOXY
      STA   EDITUI_BOXY
      LDA   EDITUI_SAVE_BOXX
      STA   EDITUI_BOXX
      RTS

editui_putc:
      STA   VTEXT_CHAR
      JMP   vtext_put_char

      .segment "RODATA"

editui_default_menus:
      .byte 3
      .byte 'f', 12, <editui_menu_title_file, >editui_menu_title_file, <editui_menu_file_items, >editui_menu_file_items
      .byte 'e', 12, <editui_menu_title_edit, >editui_menu_title_edit, <editui_menu_edit_items, >editui_menu_edit_items
      .byte 'h', 12, <editui_menu_title_help, >editui_menu_title_help, <editui_menu_help_items, >editui_menu_help_items

editui_menu_file_items:
      .byte 4
      .byte EDITUI_CMD_NEW,  'n', <editui_item_new, >editui_item_new
      .byte EDITUI_CMD_OPEN, 'o', <editui_item_open, >editui_item_open
      .byte EDITUI_CMD_SAVE, 's', <editui_item_save, >editui_item_save
      .byte EDITUI_CMD_QUIT, 'q', <editui_item_quit, >editui_item_quit

editui_menu_edit_items:
      .byte 4
      .byte EDITUI_CMD_UNDO,  'u', <editui_item_undo, >editui_item_undo
      .byte EDITUI_CMD_CUT,   't', <editui_item_cut, >editui_item_cut
      .byte EDITUI_CMD_COPY,  'c', <editui_item_copy, >editui_item_copy
      .byte EDITUI_CMD_PASTE, 'p', <editui_item_paste, >editui_item_paste

editui_menu_help_items:
      .byte 2
      .byte EDITUI_CMD_HELP,  'h', <editui_item_help, >editui_item_help
      .byte EDITUI_CMD_ABOUT, 'a', <editui_item_about, >editui_item_about

editui_menu_title_file:
      .byte "&File",0
editui_menu_title_edit:
      .byte "&Edit",0
editui_menu_title_help:
      .byte "&Help",0

editui_item_new:
      .byte "&New",0
editui_item_open:
      .byte "&Open",0
editui_item_save:
      .byte "&Save",0
editui_item_quit:
      .byte "&Quit",0
editui_item_undo:
      .byte "&Undo",0
editui_item_cut:
      .byte "Cu&t",0
editui_item_copy:
      .byte "&Copy",0
editui_item_paste:
      .byte "&Paste",0
editui_item_help:
      .byte "&Help",0
editui_item_about:
      .byte "&About",0

.endif

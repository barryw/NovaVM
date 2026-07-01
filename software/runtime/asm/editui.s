; Shared Nova editor-shell UI helpers.

.include "editui.inc"
.include "vtext.inc"
.include "blitter.inc"

.ifndef EDITUI_IMPLEMENTATION_INCLUDED
EDITUI_IMPLEMENTATION_INCLUDED = 1

EDITUI_MENU_SAVE_MAX_W = EDITUI_SCREEN_COLS
EDITUI_MENU_SAVE_MAX_H = 16
EDITUI_MENU_SAVE_BYTES = EDITUI_MENU_SAVE_MAX_W * EDITUI_MENU_SAVE_MAX_H

; Overlay save-under staging lives in XRAM (the editor stays RAM-light). Flat
; 24-bit base $05_4000 (XRAM transient staging region, free when no Z-machine /
; NVG job is running). Three planes of EDITUI_MENU_SAVE_BYTES each.
EDITUI_XRAM_SAVE_H = $05
.ifndef EDITUI_XRAM_SAVE_BASE
EDITUI_XRAM_SAVE_BASE = $4000          ; low 16 bits of $05_4000
.endif

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
EDITUI_DIRTY:       .res 1
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
EDITUI_SAVE_BOXX:   .res 1
EDITUI_SAVE_BOXY:   .res 1
EDITUI_SAVE_BOXW:   .res 1
EDITUI_SAVE_BOXH:   .res 1
EDITUI_SAVE_TITL:   .res 1
EDITUI_SAVE_TITH:   .res 1
EDITUI_SAVE_STYLE:  .res 1
; The overlay save-under planes are staged in XRAM (not CPU BSS) so the editor
; is RAM-light and reusable in any runtime. Each plane occupies
; EDITUI_MENU_SAVE_BYTES at EDITUI_XRAM_SAVE + index*EDITUI_MENU_SAVE_BYTES.

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
      .export editui_draw_menu
      .export editui_draw_status
      .export editui_draw_box
      .export editui_select_box_body
      .export editui_print_ptr
      .export editui_print_marked
      .export editui_menu_open_hotkey
      .export editui_default_menus
      .export editui_draw_title_band
      .export editui_refresh_dirty
      .export editui_save_under
      .export editui_restore_under

editui_ok:
      LDA   #EDITUI_OK
      RTS

editui_init:
      LDA   #$00
      STA   VGC_MODE
      ; The editor paints every cell via per-cell color RAM, so it does NOT
      ; dictate the global background/border/foreground — the host runtime owns
      ; those and the active palette.
      STZ   VTEXT_TOPROW
      STZ   VGC_TEXT_TOPROW
      STZ   VTEXT_SCROLL_TOP
      STZ   VGC_TEXT_SCROLL_START
      STZ   VTEXT_SCROLL_ROWS
      LDA   #EDITUI_SCREEN_ROWS
      STA   VGC_TEXT_SCROLL_ROWS
      STZ   VGC_CURSEN
      STZ   EDITUI_BOX_STYLE
      STZ   EDITUI_MENUL
      STZ   EDITUI_MENUH
      STZ   EDITUI_DIRTY
      STZ   EDITUI_MENU_SAVE_VALID
      LDA   #<editui_default_menus
      STA   EDITUI_MENUSL
      LDA   #>editui_default_menus
      STA   EDITUI_MENUSH
      JMP   editui_ok

; editui_set_menus — A = menu-table ptr low byte, Y = high byte. Point the menu
; bar at a host-supplied table. Editor-owned menu-management entry point so a
; runtime can install a tailored ("dumb") menu instead of the File/Edit/Help
; default. Pass A=Y=0 to fall back to editui_default_menus.
editui_set_menus:
      STA   EDITUI_MENUSL
      STY   EDITUI_MENUSH
      RTS

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
      STZ   VTEXT_TOPROW
      STZ   VGC_TEXT_TOPROW
      STZ   VTEXT_SCROLL_TOP
      STZ   VGC_TEXT_SCROLL_START
      STZ   VTEXT_SCROLL_ROWS
      LDA   #EDITUI_SCREEN_ROWS
      STA   VGC_TEXT_SCROLL_ROWS
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
      LDA   EDITUI_PRINTL
      LDY   EDITUI_PRINTH
      JMP   vtext_print_at

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

editui_print_at_2:
      LDA   #2
      STA   EDITUI_PRINTX
      JMP   editui_print_ptr

editui_draw_shell:
      JSR   editui_clear_screen
      JSR   editui_draw_menu
      JMP   editui_draw_status

editui_draw_menu:
      LDA   #EDITUI_COLOR_MENU
      STA   VTEXT_COLOR
      STZ   EDITUI_FILLROW
      JSR   editui_fill_row
      LDA   EDITUI_MENUSL
      ORA   EDITUI_MENUSH
      BEQ   @legacy
      JSR   editui_draw_menu_table
      BRA   @title
@legacy:
      LDA   EDITUI_MENUL
      ORA   EDITUI_MENUH
      BEQ   @title
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
@title:
      JMP   editui_draw_menu_title

; editui_draw_menu_title — draw the title right-aligned on the menu bar, just
; left of the dirty-marker cell, then refresh the marker.
editui_draw_menu_title:
      LDA   EDITUI_TITLEL
      ORA   EDITUI_TITLEH
      BEQ   editui_refresh_dirty        ; no title: still stamp the marker cell
      LDA   EDITUI_TITLEL
      STA   EDITUI_PRINTL
      LDA   EDITUI_TITLEH
      STA   EDITUI_PRINTH
      JSR   editui_strlen               ; A / EDITUI_LEN = title length
      LDA   #EDITUI_DIRTY_COL-1         ; one space gap before the marker
      SEC
      SBC   EDITUI_LEN
      STA   EDITUI_PRINTX
      STZ   EDITUI_PRINTY
      LDA   #EDITUI_COLOR_MENU
      STA   VTEXT_COLOR
      JSR   editui_print_ptr
      ; fall through to stamp the dirty marker

; editui_refresh_dirty — stamp '*' (dirty) or ' ' (clean) at the menu-bar marker
; cell. Cheap single-cell update; safe to call on every dirty-state change.
editui_refresh_dirty:
      JSR   editui_full_region
      LDA   #EDITUI_DIRTY_COL
      STA   VTEXT_CURX
      STZ   VTEXT_CURY
      JSR   vtext_set_cursor
      LDA   #EDITUI_COLOR_MENU
      STA   VTEXT_COLOR
      LDA   EDITUI_DIRTY
      BEQ   @clean
      LDA   #'*'
      BRA   @put
@clean:
      LDA   #' '
@put:
      STA   VTEXT_CHAR
      JMP   vtext_put_char

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
      RTS

editui_draw_status:
      JSR   editui_full_region
      STZ   VTEXT_LEFT
      LDA   #EDITUI_STATUS_ROW
      STA   VTEXT_TOP
      LDA   #EDITUI_SCREEN_COLS
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
      LDA   #1
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
      ; Derive the save rectangle from the active dropdown, then capture it
      ; through the generic save-under path.
      LDA   EDITUI_MENU_WIDTH
      STA   EDITUI_MENU_SAVE_W
      LDA   EDITUI_MENU_ITEM_COUNT
      CLC
      ADC   #2
      STA   EDITUI_MENU_SAVE_H
      LDA   EDITUI_MENU_X
      STA   EDITUI_MENU_SAVE_X
      LDA   #1
      STA   EDITUI_MENU_SAVE_Y
      ; fall through

; editui_save_under — capture the three text planes under the rectangle in
; EDITUI_MENU_SAVE_X/Y/W/H into the save buffers; sets EDITUI_MENU_SAVE_VALID.
; Reusable by dialogs and any overlapping overlay, not just menus.
editui_save_under:
      STZ   EDITUI_MENU_SAVE_VALID
      LDA   EDITUI_MENU_SAVE_W
      BEQ   @done
      CMP   #EDITUI_MENU_SAVE_MAX_W + 1
      BCS   @done
      LDA   EDITUI_MENU_SAVE_H
      BEQ   @done
      CMP   #EDITUI_MENU_SAVE_MAX_H + 1
      BCS   @done
      JSR   editui_menu_select_saved_region
      BNE   @done
      JSR   vtext_calc_region_addr

      LDX   #0
      LDA   #BLT_SPACE_VGC_CHAR
      JSR   editui_menu_save_plane
      BNE   @done
      LDX   #1
      LDA   #BLT_SPACE_VGC_COLOR
      JSR   editui_menu_save_plane
      BNE   @done
      LDX   #2
      LDA   #BLT_SPACE_VGC_TEXTATTR
      JSR   editui_menu_save_plane
      BNE   @done

      LDA   #1
      STA   EDITUI_MENU_SAVE_VALID
@done:
      RTS

editui_restore_under:
editui_menu_restore_under:
      LDA   EDITUI_MENU_SAVE_VALID
      BEQ   @done
      JSR   editui_menu_select_saved_region
      BNE   @clear
      JSR   vtext_calc_region_addr

      LDX   #0
      LDA   #BLT_SPACE_VGC_CHAR
      JSR   editui_menu_restore_plane
      BNE   @clear
      LDX   #1
      LDA   #BLT_SPACE_VGC_COLOR
      JSR   editui_menu_restore_plane
      BNE   @clear
      LDX   #2
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

; editui_menu_save_plane — A = VGC plane space, X = plane index (0/1/2).
; Blits the W x H rect from the VGC plane to XRAM staging (packed, stride W).
editui_menu_save_plane:
      STA   BLT_SRCSPACE
      LDA   #BLT_SPACE_XRAM
      STA   BLT_DSTSPACE
      LDA   VTEXT_ADDRL
      STA   BLT_SRCL
      LDA   VTEXT_ADDRH
      STA   BLT_SRCM
      STZ   BLT_SRCH
      LDA   editui_xram_plane_l,X
      STA   BLT_DSTL
      LDA   editui_xram_plane_m,X
      STA   BLT_DSTM
      LDA   #EDITUI_XRAM_SAVE_H
      STA   BLT_DSTH
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

; editui_menu_restore_plane — A = VGC plane space, X = plane index (0/1/2).
editui_menu_restore_plane:
      STA   BLT_DSTSPACE
      LDA   #BLT_SPACE_XRAM
      STA   BLT_SRCSPACE
      LDA   editui_xram_plane_l,X
      STA   BLT_SRCL
      LDA   editui_xram_plane_m,X
      STA   BLT_SRCM
      LDA   #EDITUI_XRAM_SAVE_H
      STA   BLT_SRCH
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

; Low/high bytes of the XRAM save-under address for each plane index (0/1/2),
; = EDITUI_XRAM_SAVE_BASE + index * EDITUI_MENU_SAVE_BYTES (full address is
; EDITUI_XRAM_SAVE_H : m : l).
editui_xram_plane_l:
      .byte <(EDITUI_XRAM_SAVE_BASE + 0 * EDITUI_MENU_SAVE_BYTES)
      .byte <(EDITUI_XRAM_SAVE_BASE + 1 * EDITUI_MENU_SAVE_BYTES)
      .byte <(EDITUI_XRAM_SAVE_BASE + 2 * EDITUI_MENU_SAVE_BYTES)
editui_xram_plane_m:
      .byte >(EDITUI_XRAM_SAVE_BASE + 0 * EDITUI_MENU_SAVE_BYTES)
      .byte >(EDITUI_XRAM_SAVE_BASE + 1 * EDITUI_MENU_SAVE_BYTES)
      .byte >(EDITUI_XRAM_SAVE_BASE + 2 * EDITUI_MENU_SAVE_BYTES)

editui_default_menus:
      .byte 3
      .byte 'f', 18, <editui_menu_title_file, >editui_menu_title_file, <editui_menu_file_items, >editui_menu_file_items
      .byte 'e', 18, <editui_menu_title_edit, >editui_menu_title_edit, <editui_menu_edit_items, >editui_menu_edit_items
      .byte 's', 18, <editui_menu_title_search, >editui_menu_title_search, <editui_menu_search_items, >editui_menu_search_items

editui_menu_file_items:
      .byte 2
      .byte EDITUI_CMD_SAVE, 's', <editui_item_save, >editui_item_save
      .byte EDITUI_CMD_QUIT, 'x', <editui_item_quit, >editui_item_quit

editui_menu_edit_items:
      .byte 3
      .byte EDITUI_CMD_CUT,   't', <editui_item_cut, >editui_item_cut
      .byte EDITUI_CMD_COPY,  'c', <editui_item_copy, >editui_item_copy
      .byte EDITUI_CMD_PASTE, 'p', <editui_item_paste, >editui_item_paste

editui_menu_search_items:
      .byte 2
      .byte EDITUI_CMD_FIND,      'f', <editui_item_find, >editui_item_find
      .byte EDITUI_CMD_GOTO_LINE, 'g', <editui_item_goto_line, >editui_item_goto_line

editui_menu_title_file:
      .byte "&File",0
editui_menu_title_edit:
      .byte "&Edit",0
editui_menu_title_search:
      .byte "&Search",0

editui_item_save:
      .byte "&Save ^K S",0
editui_item_quit:
      .byte "E&xit Alt-X",0
editui_item_cut:
      .byte "Cu&t ^X",0
editui_item_copy:
      .byte "&Copy ^C",0
editui_item_paste:
      .byte "&Paste ^V",0
editui_item_find:
      .byte "&Find ^Q F",0
editui_item_goto_line:
      .byte "&Goto ^G",0

.endif

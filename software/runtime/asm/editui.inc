; =====================================================================
;  Nova NDK — editui.inc
;
;  Editor-shell UI ABI: a text-mode chrome shell for native tools — menu bar,
;  title/help rows, framed panels, and status lines. Apps own input dispatch,
;  document state, and editor-specific drawing.
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

.ifndef EDITUI_INC_INCLUDED
EDITUI_INC_INCLUDED = 1

.include "vtext.inc"

EDITUI_OK  = $00
EDITUI_ERR = $01

EDITUI_SCREEN_COLS = VTEXT_SCREEN_COLS
EDITUI_SCREEN_ROWS = VTEXT_SCREEN_ROWS

; Black background with light-grey bars. bg = high nibble, fg = low nibble.
; Authored for the Nova/C64 active palette: 0=black, 1=white, 2=red,
; 12=grey-med, 15=grey-light.
; Legacy source-compatibility constant; editui_init does not write VGC.PALETTE.
EDITUI_PALETTE_MODE     = $00
EDITUI_COLOR_BG         = $00      ; black
EDITUI_COLOR_FG         = $01      ; white
EDITUI_COLOR_SHELL      = $61      ; blue editor workspace, white text
EDITUI_COLOR_TITLE      = $61      ; title text on blue workspace
EDITUI_COLOR_MENU       = $F0      ; light-grey menu bar, black text
EDITUI_COLOR_MENU_HOT   = $F2      ; light-grey bar, red hotkey
EDITUI_COLOR_FRAME      = $61      ; white frame lines on blue workspace
EDITUI_COLOR_DIM        = $6C      ; grey-med dim text on blue workspace
EDITUI_COLOR_MENU_SEL   = $1F      ; selected dropdown item: white bg
EDITUI_COLOR_MENU_SEL_HOT = $12    ; selected item hotkey
EDITUI_COLOR_STATUS     = $F0      ; light-grey status bar
EDITUI_COLOR_STATUS_HOT = $F6      ; status bar accent
EDITUI_COLOR_PANEL      = $61      ; editor panels on blue workspace

EDITUI_STATUS_ROW = EDITUI_SCREEN_ROWS - 1
; Dirty marker ('*') cell on the menu bar (right side). The title is drawn just
; to its left; toggling the marker is a single-cell write.
EDITUI_DIRTY_COL  = EDITUI_SCREEN_COLS - 2

EDITUI_BOX_SINGLE = $00
EDITUI_BOX_DOUBLE = $01

; CP437 box-drawing characters from Nova's default font.
EDITUI_CH_SINGLE_TL = $DA
EDITUI_CH_SINGLE_TR = $BF
EDITUI_CH_SINGLE_BL = $C0
EDITUI_CH_SINGLE_BR = $D9
EDITUI_CH_SINGLE_H  = $C4
EDITUI_CH_SINGLE_V  = $B3
EDITUI_CH_SINGLE_TITLE_L = $B4
EDITUI_CH_SINGLE_TITLE_R = $C3
EDITUI_CH_DOUBLE_TL = $C9
EDITUI_CH_DOUBLE_TR = $BB
EDITUI_CH_DOUBLE_BL = $C8
EDITUI_CH_DOUBLE_BR = $BC
EDITUI_CH_DOUBLE_H  = $CD
EDITUI_CH_DOUBLE_V  = $BA
EDITUI_CH_DOUBLE_TITLE_L = $B9
EDITUI_CH_DOUBLE_TITLE_R = $CC
EDITUI_CH_DOT       = $FA

EDITUI_KEY_BACKSPACE = $08
EDITUI_KEY_TAB       = $09
EDITUI_KEY_ENTER     = $0D
EDITUI_KEY_ESC       = $1B
EDITUI_KEY_LEFT      = $1C
EDITUI_KEY_RIGHT     = $1D
EDITUI_KEY_UP        = $1E
EDITUI_KEY_DOWN      = $1F
EDITUI_KEY_DELETE    = $7F
EDITUI_KEY_ALT_PREFIX = EDITUI_KEY_ESC

EDITUI_KEY_CTRL_A    = $01
EDITUI_KEY_CTRL_B    = $02
EDITUI_KEY_CTRL_C    = $03
EDITUI_KEY_CTRL_F    = $06
EDITUI_KEY_CTRL_G    = $07
EDITUI_KEY_CTRL_K    = $0B
EDITUI_KEY_CTRL_N    = $0E
EDITUI_KEY_CTRL_O    = $0F
EDITUI_KEY_CTRL_Q    = $11
EDITUI_KEY_CTRL_S    = $13
EDITUI_KEY_CTRL_V    = $16
EDITUI_KEY_CTRL_W    = $17
EDITUI_KEY_CTRL_X    = $18
EDITUI_KEY_CTRL_Y    = $19
EDITUI_KEY_CTRL_Z    = $1A

; Editing navigation keys (defined by Nova; the host keyboard maps physical
; Home/End/PgUp/PgDn to these bytes).
EDITUI_KEY_HOME      = $83
EDITUI_KEY_END       = $05
EDITUI_KEY_PGUP      = $10
EDITUI_KEY_PGDN      = $12
EDITUI_KEY_CTRL_HOME = $80
EDITUI_KEY_CTRL_END  = $81
EDITUI_KEY_F3        = $82
EDITUI_KEY_F6        = $84
EDITUI_KEY_SHIFT_F6  = $85

EDITUI_CMD_NONE      = $00
EDITUI_CMD_NEW       = $01
EDITUI_CMD_OPEN      = $02
EDITUI_CMD_SAVE      = $03
EDITUI_CMD_SAVE_AS   = $04
EDITUI_CMD_QUIT      = $05
EDITUI_CMD_UNDO      = $06
EDITUI_CMD_CUT       = $07
EDITUI_CMD_COPY      = $08
EDITUI_CMD_PASTE     = $09
EDITUI_CMD_CLOSE     = $0A
EDITUI_CMD_HELP      = $0B
EDITUI_CMD_ABOUT     = $0C
EDITUI_CMD_FIND      = $0D
EDITUI_CMD_GOTO_LINE = $0E
EDITUI_CMD_SELECT_ALL = $0F
EDITUI_CMD_REPLACE   = $10
EDITUI_CMD_FIND_NEXT = $11
EDITUI_CMD_REDO      = $12
EDITUI_CMD_REPLACE_ALL = $13
EDITUI_CMD_BUFFER_NEXT = $14
EDITUI_CMD_BUFFER_PREVIOUS = $15
EDITUI_CMD_BUFFER_LIST = $16
EDITUI_CMD_WINDOW_NEXT = $17
EDITUI_CMD_WINDOW_PREVIOUS = $18

EDITUI_MENU_ENTRY_SIZE = 6
EDITUI_MENU_ITEM_SIZE  = 4

.global EDITUI_TITLEL
.global EDITUI_TITLEH
.global EDITUI_MENUL
.global EDITUI_MENUH
.global EDITUI_MENUSL
.global EDITUI_MENUSH
.global EDITUI_HELPL
.global EDITUI_HELPH
.global EDITUI_STATUSL
.global EDITUI_STATUSH
.global EDITUI_DIRTY
.global EDITUI_BOXX
.global EDITUI_BOXY
.global EDITUI_BOXW
.global EDITUI_BOXH
.global EDITUI_BOX_TITLEL
.global EDITUI_BOX_TITLEH
.global EDITUI_BOX_STYLE
.global EDITUI_PRINTL
.global EDITUI_PRINTH
.global EDITUI_PRINTX
.global EDITUI_PRINTY
.global EDITUI_PRINT_COLOR
.global EDITUI_HOT_COLOR
.global EDITUI_TMP
.global EDITUI_COL
.global EDITUI_ROW
.global EDITUI_CH_TL
.global EDITUI_CH_TR
.global EDITUI_CH_BL
.global EDITUI_CH_BR
.global EDITUI_CH_H
.global EDITUI_CH_V
.global EDITUI_CH_TITLE_L
.global EDITUI_CH_TITLE_R
.global EDITUI_MENU_CMD
.global EDITUI_MENU_SELECTED

; Save-under region rectangle (generic overlay save/restore for dialogs).
.global EDITUI_MENU_SAVE_X
.global EDITUI_MENU_SAVE_Y
.global EDITUI_MENU_SAVE_W
.global EDITUI_MENU_SAVE_H

.global editui_init
.global editui_full_region
.global editui_clear_screen
.global editui_shutdown
.global editui_draw_shell
.global editui_draw_title_band
.global editui_draw_menu
.global editui_refresh_dirty
.global editui_draw_status
.global editui_draw_box
.global editui_select_box_body
.global editui_print_ptr
.global editui_print_marked
.global editui_menu_open_hotkey
.global editui_set_menus
.global editui_save_under
.global editui_restore_under

.global editui_default_menus

.endif

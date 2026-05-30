; Shared Nova editor-shell UI declarations.
;
; EDITUI is a small text-mode shell for native tools: menu bar, title/help
; rows, framed panels, and status lines. Applications own input dispatch,
; document state, and editor-specific drawing.

.ifndef EDITUI_INC_INCLUDED
EDITUI_INC_INCLUDED = 1

.include "vtext.inc"

EDITUI_OK  = $00
EDITUI_ERR = $01

EDITUI_SCREEN_COLS = VTEXT_SCREEN_COLS
EDITUI_SCREEN_ROWS = VTEXT_SCREEN_ROWS

EDITUI_PALETTE_MODE     = $00
EDITUI_COLOR_BG         = $06
EDITUI_COLOR_FG         = $0F
EDITUI_COLOR_SHELL      = $6F
EDITUI_COLOR_TITLE      = $67
EDITUI_COLOR_MENU       = $F0
EDITUI_COLOR_MENU_HOT   = $F2
EDITUI_COLOR_FRAME      = $6F
EDITUI_COLOR_DIM        = $6C
EDITUI_COLOR_MENU_SEL   = $6F
EDITUI_COLOR_MENU_SEL_HOT = $67
EDITUI_COLOR_STATUS     = $F0
EDITUI_COLOR_STATUS_HOT = $F6
EDITUI_COLOR_PANEL      = $6F

EDITUI_STATUS_ROW = EDITUI_SCREEN_ROWS - 1

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
EDITUI_KEY_CTRL_C    = $03
EDITUI_KEY_CTRL_N    = $0E
EDITUI_KEY_CTRL_O    = $0F
EDITUI_KEY_CTRL_Q    = $11
EDITUI_KEY_CTRL_S    = $13
EDITUI_KEY_CTRL_W    = $17
EDITUI_KEY_CTRL_X    = $18
EDITUI_KEY_CTRL_Z    = $1A

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
.global EDITUI_MENU_CMD
.global EDITUI_MENU_SELECTED

.global editui_init
.global editui_full_region
.global editui_clear_screen
.global editui_shutdown
.global editui_draw_shell
.global editui_draw_status
.global editui_draw_box
.global editui_select_box_body
.global editui_print_ptr
.global editui_print_marked
.global editui_menu_open_hotkey

.global editui_default_menus

.endif

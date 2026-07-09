; =====================================================================
;  Nova NDK — nui.inc
;
;  Shared Nova application UI helpers: modal/dialog chrome drawn with
;  graphics-plane fills and text via vtext, plus save-under/restore of the
;  text and graphics planes covered by a dialog (including its shadow).
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

.ifndef NUI_INC_INCLUDED
NUI_INC_INCLUDED = 1

.include "vtext.inc"

NUI_OK  = $00
NUI_ERR = $01

NUI_RESULT_OK     = $00
NUI_RESULT_CANCEL = $01

NUI_FILE_MODE_OPEN = $00
NUI_FILE_MODE_SAVE = $01

NUI_FILE_TYPE_BAS    = $0001
NUI_FILE_TYPE_SID    = $0002
NUI_FILE_TYPE_BIN    = $0004
NUI_FILE_TYPE_MID    = $0008
NUI_FILE_TYPE_GFX    = $0010
NUI_FILE_TYPE_DIR    = $0020
NUI_FILE_TYPE_FORTH  = $0040
NUI_FILE_TYPE_LOGO   = $0080
NUI_FILE_TYPE_PASCAL = $0100
NUI_FILE_TYPE_ASM    = $0200

NUI_KEY_TAB = $09
NUI_KEY_BACKTAB = $8F
NUI_KEY_ENTER = $0D
NUI_KEY_ESCAPE = $1B
NUI_KEY_LEFT = $1C
NUI_KEY_RIGHT = $1D
NUI_KEY_UP = $1E
NUI_KEY_DOWN = $1F

NUI_DIALOG_DEFAULT_LEFT   = 20
NUI_DIALOG_DEFAULT_TOP    = 6
NUI_DIALOG_DEFAULT_WIDTH  = 40
NUI_DIALOG_DEFAULT_HEIGHT = 14

NUI_COLOR_SHADOW = $00
NUI_COLOR_BORDER = $01
NUI_COLOR_PANEL  = $0F
NUI_TEXT_NORMAL  = $F0
NUI_TEXT_FRAME   = $F1
NUI_TEXT_FIELD   = $30
NUI_TEXT_BUTTON  = $50
NUI_TEXT_SHADOW  = $00

; NUI_STYLE_* are palette-index settings consumed by shared modal controls.
; NUI_STYLE_BORDER is the one-pixel top/left highlight. NUI_STYLE_SHADOW is
; drawn outside the right/bottom edge.
; NUI_STYLE_TEXT is packed bg/fg in the same format as VTEXT_COLOR.

.global NUI_DIALOG_LEFT
.global NUI_DIALOG_TOP
.global NUI_DIALOG_WIDTH
.global NUI_DIALOG_HEIGHT
.global NUI_TITLEL
.global NUI_TITLEH
.global NUI_MSGL
.global NUI_MSGH
.global NUI_FOOTERL
.global NUI_FOOTERH
.global NUI_RESULT
.global NUI_SAVE_LEFT
.global NUI_SAVE_TOP
.global NUI_SAVE_WIDTH
.global NUI_SAVE_HEIGHT
.global NUI_SAVE_ADDRL
.global NUI_SAVE_ADDRM
.global NUI_SAVE_ADDRH
.global NUI_SAVE_VALID
.global NUI_LIST_ITEMSL
.global NUI_LIST_ITEMSH
.global NUI_LIST_ROW_WIDTH
.global NUI_LIST_ROW_COUNT
.global NUI_LIST_SELECTED
.global NUI_FILE_OUTL
.global NUI_FILE_OUTH
.global NUI_FILE_OUT_MAX
.global NUI_FILE_OUT_LEN
.global NUI_FILE_MODE
.global NUI_FILE_TYPE_MASK
.global NUI_FILE_TYPE_MASKH
.global NUI_FILE_ROWSL
.global NUI_FILE_ROWSH
.global NUI_INPUT_OUTL
.global NUI_INPUT_OUTH
.global NUI_INPUT_OUT_MAX
.global NUI_INPUT_OUT_LEN
.global NUI_INPUT_LABELL
.global NUI_INPUT_LABELH
.global NUI_STYLE_SHADOW
.global NUI_STYLE_BORDER
.global NUI_STYLE_PANEL
.global NUI_STYLE_TEXT

.global nui_dialog_defaults
.global nui_show_dialog
.global nui_show_dialog_wait
.global nui_show_error
.global nui_read_key
.global nui_drain_keys
.global nui_wait_key
.global nui_save_under
.global nui_restore_under
.global nui_save_under_full
.global nui_restore_under_full
.global nui_pick_list
.global nui_file_picker
.global nui_text_input

; Cross-object internal helpers (referenced between feature-group objects when
; nui is built as separate library objects).
.global nui_ok
.global nui_error
.global nui_validate_dialog
.global nui_set_screen_text
.global nui_set_x_pixels
.global nui_set_y_pixels
.global nui_set_width_pixels
.global nui_set_height_pixels

; Remaining shared state: allocated once in nui_state.s, referenced by the
; feature-group objects.
.global NUI_PRINT_X
.global NUI_TEXT_LEN
.global NUI_LIST_FIRST
.global NUI_LIST_VISIBLE
.global NUI_LIST_INDEX
.global NUI_LIST_ROWL
.global NUI_LIST_ROWH
.global NUI_LIST_TMP
.global NUI_STYLE_VALID
.globalzp NUI_PRINTL
.globalzp NUI_PRINTH

.endif

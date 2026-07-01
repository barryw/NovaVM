; Shared Nova application UI helpers.
;
; First-pass scope: modal/dialog chrome drawn with graphics-plane virtual
; sprite fills, plus text rendered through vtext. Applications provide
; zero-terminated title/message strings and can either use the default geometry
; or set NUI_DIALOG_* before drawing.
;
; Text-only applications can save the three text planes under a dialog. Mixed
; text/graphics applications should use the full save-under routines, which
; also preserve the graphics-plane pixels covered by the dialog. NUI chrome
; includes its shadow: callers that intend to restore the covered area should
; save one extra text cell to the right and bottom when the dialog is not flush
; with the screen edge. The caller provides XRAM buffers; text buffers are
; NUI_SAVE_WIDTH*NUI_SAVE_HEIGHT bytes each, and the graphics buffer is
; (NUI_SAVE_WIDTH*4)*(NUI_SAVE_HEIGHT*4) bytes.

.ifndef NUI_INC_INCLUDED
NUI_INC_INCLUDED = 1

.include "vtext.inc"
.include "vsprite.inc"

NUI_OK  = $00
NUI_ERR = $01

NUI_RESULT_OK     = $00
NUI_RESULT_CANCEL = $01

NUI_KEY_ENTER = $0D
NUI_KEY_ESCAPE = $1B
NUI_KEY_LEFT = $1C
NUI_KEY_RIGHT = $1D
NUI_KEY_UP = $1E
NUI_KEY_DOWN = $1F

NUI_DIALOG_DEFAULT_LEFT   = 16
NUI_DIALOG_DEFAULT_TOP    = 17
NUI_DIALOG_DEFAULT_WIDTH  = 48
NUI_DIALOG_DEFAULT_HEIGHT = 12

NUI_COLOR_SHADOW = $00
NUI_COLOR_BORDER = $0F
NUI_COLOR_PANEL  = $0B
NUI_TEXT_NORMAL  = $01

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
.global NUI_STYLE_SHADOW
.global NUI_STYLE_BORDER
.global NUI_STYLE_PANEL
.global NUI_STYLE_TEXT

.global nui_dialog_defaults
.global nui_show_dialog
.global nui_show_dialog_wait
.global nui_show_error
.global nui_read_key
.global nui_wait_key
.global nui_save_under
.global nui_restore_under
.global nui_save_under_full
.global nui_restore_under_full
.global nui_pick_list

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

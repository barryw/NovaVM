; Shared Nova application UI helpers.
;
; First-pass scope: modal/dialog chrome drawn with graphics-plane virtual
; sprite fills, plus text rendered through vtext. Applications provide
; zero-terminated title/message strings and can either use the default geometry
; or set NUI_DIALOG_* before drawing.

.ifndef NUI_INC_INCLUDED
NUI_INC_INCLUDED = 1

.include "vtext.inc"
.include "vsprite.inc"

NUI_OK  = $00
NUI_ERR = $01

NUI_DIALOG_DEFAULT_LEFT   = 16
NUI_DIALOG_DEFAULT_TOP    = 17
NUI_DIALOG_DEFAULT_WIDTH  = 48
NUI_DIALOG_DEFAULT_HEIGHT = 12

NUI_COLOR_SHADOW = $00
NUI_COLOR_BORDER = $0F
NUI_COLOR_PANEL  = $0B
NUI_TEXT_NORMAL  = $01

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

.global nui_dialog_defaults
.global nui_show_dialog
.global nui_show_dialog_wait
.global nui_show_error
.global nui_wait_key

.endif

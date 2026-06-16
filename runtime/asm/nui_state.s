; nui shared state.
;
; Allocates the NUI_* state bytes once. The nui feature-group objects
; (nui_dialog/nui_input/nui_list/nui_uisave) import these via the .global
; declarations in nui.inc. This object holds no code, so linking it costs only
; the BSS/zero-page bytes, never ROM. It is pulled whenever any nui object is
; used and dropped when none are.

.include "nui.inc"

      .segment "BSS"

NUI_DIALOG_LEFT:    .res 1
NUI_DIALOG_TOP:     .res 1
NUI_DIALOG_WIDTH:   .res 1
NUI_DIALOG_HEIGHT:  .res 1
NUI_TITLEL:         .res 1
NUI_TITLEH:         .res 1
NUI_MSGL:           .res 1
NUI_MSGH:           .res 1
NUI_FOOTERL:        .res 1
NUI_FOOTERH:        .res 1
NUI_RESULT:         .res 1
NUI_PRINT_X:        .res 1
NUI_TEXT_LEN:       .res 1
NUI_SAVE_LEFT:      .res 1
NUI_SAVE_TOP:       .res 1
NUI_SAVE_WIDTH:     .res 1
NUI_SAVE_HEIGHT:    .res 1
NUI_SAVE_VALID:     .res 1
NUI_SAVE_ADDRL:     .res 4       ; char, color, textattr, gfx XRAM low bytes
NUI_SAVE_ADDRM:     .res 4       ; char, color, textattr, gfx XRAM mid bytes
NUI_SAVE_ADDRH:     .res 4       ; char, color, textattr, gfx XRAM high bytes
NUI_LIST_ITEMSL:    .res 1
NUI_LIST_ITEMSH:    .res 1
NUI_LIST_ROW_WIDTH: .res 1
NUI_LIST_ROW_COUNT: .res 1
NUI_LIST_SELECTED:  .res 1
NUI_LIST_FIRST:     .res 1
NUI_LIST_VISIBLE:   .res 1
NUI_LIST_INDEX:     .res 1
NUI_LIST_ROWL:      .res 1
NUI_LIST_ROWH:      .res 1
NUI_LIST_TMP:       .res 1
NUI_STYLE_SHADOW:   .res 1
NUI_STYLE_BORDER:   .res 1
NUI_STYLE_PANEL:    .res 1
NUI_STYLE_TEXT:     .res 1
NUI_STYLE_VALID:    .res 1

      .segment "ZEROPAGE"

NUI_PRINTL:         .res 1
NUI_PRINTH:         .res 1

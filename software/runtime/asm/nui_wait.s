; =====================================================================
;  Nova NDK — nui_wait.s
;
;  nui blocking wait/error helpers: show-dialog-and-wait, show-error,
;  and wait-key. Kept out of nui_dialog.o so callers that only draw
;  nonblocking dialogs do not pull cursor wait/error code in.
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

.include "nui.inc"

      .segment "CODE"

; @label NUI.SHOW_DIALOG_WAIT
; @kind routine
; @symbol nui_show_dialog_wait
; @summary Draw the configured modal dialog and wait for one key.
nui_show_dialog_wait:
      JSR   nui_show_dialog
      BNE   @done
      JSR   nui_wait_key
      JMP   nui_ok
@done:
      RTS

; @label NUI.SHOW_ERROR
; @kind routine
; @symbol nui_show_error
; @summary Draw a modal error dialog and wait for one key.
nui_show_error:
      LDA   NUI_DIALOG_WIDTH
      ORA   NUI_DIALOG_HEIGHT
      BNE   @have_geometry
      JSR   nui_dialog_defaults
@have_geometry:
      JMP   nui_show_dialog_wait

; @label NUI.WAIT_KEY
; @kind routine
; @symbol nui_wait_key
; @summary Wait for one keyboard byte and return it in A.
nui_wait_key:
      LDA   #$01
      STA   VGC_CURSEN
      JSR   nui_read_key
      PHA
      STZ   VGC_CURSEN
      PLA
      RTS

; =====================================================================
;  Nova NDK — nui_drain.s
;
;  Tiny independently linkable keyboard-drain helper.
; =====================================================================

.include "nui.inc"

      .segment "CODE"

; @label NUI.DRAIN_KEYS
; @kind routine
; @symbol nui_drain_keys
; @summary Drain pending keyboard bytes before handing focus to a new control.
nui_drain_keys:
@loop:
      LDA   VGC_CHARIN
      BNE   @loop
      LDA   #NUI_OK
      RTS

; editui shutdown helper.
;
; Split from editui.o so the editor module does not link shutdown code it never
; calls while standalone EDITUI demos can still use the public routine.

.include "editui.inc"

      .segment "CODE"

      .export editui_shutdown

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

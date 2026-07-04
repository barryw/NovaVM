; Shared Nova mouse pointer helper routines.

.include "mouse.inc"

.ifndef MOUSE_IMPLEMENTATION_INCLUDED
MOUSE_IMPLEMENTATION_INCLUDED = 1

      .segment "CODE"

      .export mouse_show
      .export mouse_hide
      .export mouse_set_ctrl
      .export mouse_set_shape
      .export mouse_set_hotspot
      .export mouse_set_color
      .export mouse_get_buttons
      .export mouse_get_x
      .export mouse_get_y

; Show the hardware mouse pointer using the default auto-contrast mode.
; @label MOUSE.SHOW
; @kind routine
; @symbol mouse_show
; @summary Show the hardware mouse pointer using auto-contrast colour.
mouse_show:
      LDA   #MOUSE_DEFAULT_CTRL
      STA   VGC_MOUSE_CTRL
      RTS

; Hide the hardware mouse pointer.
; @label MOUSE.HIDE
; @kind routine
; @symbol mouse_hide
; @summary Hide the hardware mouse pointer.
mouse_hide:
      LDA   #$00
      STA   VGC_MOUSE_CTRL
      RTS

; Set raw pointer control bits.
; @label MOUSE.SET_CTRL
; @kind routine
; @symbol mouse_set_ctrl
; @summary Set VGC mouse control bits from A.
; @in A: VGC_MOUSE_CTRL_* bitmask.
mouse_set_ctrl:
      STA   VGC_MOUSE_CTRL
      RTS

; Select the 16x16 4bpp shape slot used by the pointer.
; @label MOUSE.SET_SHAPE
; @kind routine
; @symbol mouse_set_shape
; @summary Select the hardware mouse pointer shape slot.
; @in A: Shape slot index.
mouse_set_shape:
      STA   VGC_MOUSE_SHAPE
      RTS

; Set the pointer hotspot.
; @label MOUSE.SET_HOTSPOT
; @kind routine
; @symbol mouse_set_hotspot
; @summary Set pointer hotspot using A=x and X=y.
; @in A: Hotspot X, 0..15.
; @in X: Hotspot Y, 0..15.
mouse_set_hotspot:
      STA   VGC_MOUSE_HOTX
      STX   VGC_MOUSE_HOTY
      RTS

; Set fixed pointer colour used when VGC_MOUSE_CTRL_COLOR is enabled.
; @label MOUSE.SET_COLOR
; @kind routine
; @symbol mouse_set_color
; @summary Set fixed pointer colour from A.
; @in A: Palette index 0..15.
mouse_set_color:
      STA   VGC_MOUSE_COLOR
      RTS

; Return current mouse button bits.
; @label MOUSE.GET_BUTTONS
; @kind routine
; @symbol mouse_get_buttons
; @summary Return current mouse button mask.
; @out A: VGC_MOUSE_BTN_* bitmask.
mouse_get_buttons:
      LDA   VGC_MOUSE_BUTTONS
      RTS

; Return current mouse X position.
; @label MOUSE.GET_X
; @kind routine
; @symbol mouse_get_x
; @summary Return current mouse X coordinate.
; @out A: X low byte.
; @out X: X high bit in bit 0.
mouse_get_x:
      LDA   VGC_MOUSE_XL
      LDX   VGC_MOUSE_XH
      RTS

; Return current mouse Y position.
; @label MOUSE.GET_Y
; @kind routine
; @symbol mouse_get_y
; @summary Return current mouse Y coordinate.
; @out A: Y coordinate.
mouse_get_y:
      LDA   VGC_MOUSE_Y
      RTS

.endif

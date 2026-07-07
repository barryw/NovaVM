; Nova mouse event dispatch: click callbacks + areas-of-interest.
;
; A polled layer over the raw VGC mouse registers (there is no mouse IRQ). Call
; mouse_poll() once per frame; it edge-detects button presses, records the event
; context (button + x/y + which registered area was hit), and invokes the
; client's click handler. Areas-of-interest let the handler switch on a region
; tag instead of hit-testing coordinates itself.
;
; State lives in BSS, so this module links into NDK apps (which have RAM), NOT
; the resident EhBASIC mouse.s (ZP-only). Include mouse.inc for the raw helpers.
;
; Usage:
;     JSR mouse_events_init
;     LDA #<on_click : LDX #>on_click : JSR mouse_set_click_handler
;     ; register a button rect (10,30)-(110,50) inclusive, tagged 1
;     LDA #10  : STA mouse_area_desc+0     ; x0 low
;     LDA #0   : STA mouse_area_desc+1     ; x0 high bit
;     LDA #30  : STA mouse_area_desc+2     ; y0
;     LDA #110 : STA mouse_area_desc+3     ; x1 low
;     LDA #0   : STA mouse_area_desc+4     ; x1 high bit
;     LDA #50  : STA mouse_area_desc+5     ; y1
;     LDA #1   : STA mouse_area_desc+6     ; tag
;     JSR mouse_add_area
;     ; then once per frame:
;     JSR mouse_poll
;     ; on_click reads mouse_evt_button, mouse_evt_xl/xh/y, mouse_evt_area
;     ; (mouse_evt_area = the button's tag, or MOUSE_NO_AREA elsewhere), RTS.

.ifndef MOUSE_EVENTS_INC_INCLUDED
MOUSE_EVENTS_INC_INCLUDED = 1

.include "mouse.inc"

MOUSE_MAX_AREAS = 8       ; registered areas-of-interest (raise + rebuild to grow)
MOUSE_NO_AREA   = $FF     ; mouse_evt_area value when a click hit no area

.global mouse_events_init
.global mouse_poll
.global mouse_set_click_handler
.global mouse_add_area
.global mouse_clear_areas

; Event context — valid while the click handler runs.
.global mouse_evt_button  ; mask of buttons newly pressed this poll (VGC_MOUSE_BTN_*)
.global mouse_evt_xl      ; click X low byte
.global mouse_evt_xh      ; click X high bit (bit0)
.global mouse_evt_y       ; click Y
.global mouse_evt_area    ; tag of the first area hit, or MOUSE_NO_AREA

; Area descriptor input: fill these 7 bytes, then call mouse_add_area.
.global mouse_area_desc   ; x0l, x0h, y0, x1l, x1h, y1, tag  (inclusive bounds)

.endif

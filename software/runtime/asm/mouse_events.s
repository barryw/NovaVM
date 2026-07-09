; =====================================================================
;  Nova NDK — mouse_events.s
;
;  Nova mouse event dispatch: per-frame polling with button edge
;  detection, a registered click handler, and areas-of-interest hit
;  testing. See mouse_events.inc.
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

.include "mouse_events.inc"

.ifndef MOUSE_EVENTS_IMPLEMENTATION_INCLUDED
MOUSE_EVENTS_IMPLEMENTATION_INCLUDED = 1

      .segment "BSS"

mouse_prev_btn:   .res 1              ; button state at the previous poll (edge detect)
mouse_click_vecl: .res 1              ; click handler vector (0000 = none)
mouse_click_vech: .res 1

mouse_evt_button: .res 1
mouse_evt_xl:     .res 1
mouse_evt_xh:     .res 1
mouse_evt_y:      .res 1
mouse_evt_area:   .res 1

mouse_area_desc:  .res 7              ; caller input for mouse_add_area

mouse_area_count: .res 1
mouse_area_x0l:   .res MOUSE_MAX_AREAS
mouse_area_x0h:   .res MOUSE_MAX_AREAS
mouse_area_y0:    .res MOUSE_MAX_AREAS
mouse_area_x1l:   .res MOUSE_MAX_AREAS
mouse_area_x1h:   .res MOUSE_MAX_AREAS
mouse_area_y1:    .res MOUSE_MAX_AREAS
mouse_area_tag:   .res MOUSE_MAX_AREAS

      .segment "CODE"

      .export mouse_events_init
      .export mouse_poll
      .export mouse_set_click_handler
      .export mouse_add_area
      .export mouse_clear_areas
      .export mouse_evt_button
      .export mouse_evt_xl
      .export mouse_evt_xh
      .export mouse_evt_y
      .export mouse_evt_area
      .export mouse_area_desc

; Reset event state: no handler, no areas, buttons released.
; @label MOUSE.EVENTS_INIT
; @kind routine
; @symbol mouse_events_init
; @summary Reset mouse event state (handler, areas, edge detector).
mouse_events_init:
      LDA   #$00
      STA   mouse_prev_btn
      STA   mouse_click_vecl
      STA   mouse_click_vech
      STA   mouse_area_count
      RTS

; Register the click handler invoked by mouse_poll on each button press.
; @label MOUSE.SET_CLICK_HANDLER
; @kind routine
; @symbol mouse_set_click_handler
; @summary Set the click event handler address.
; @in A: Handler address low byte.
; @in X: Handler address high byte.
mouse_set_click_handler:
      STA   mouse_click_vecl
      STX   mouse_click_vech
      RTS

; Drop all registered areas-of-interest.
; @label MOUSE.CLEAR_AREAS
; @kind routine
; @symbol mouse_clear_areas
; @summary Remove all registered areas-of-interest.
mouse_clear_areas:
      LDA   #$00
      STA   mouse_area_count
      RTS

; Register an area-of-interest from the mouse_area_desc block
; (x0l,x0h,y0,x1l,x1h,y1,tag; bounds inclusive). Carry set = table full.
; @label MOUSE.ADD_AREA
; @kind routine
; @symbol mouse_add_area
; @summary Register an area-of-interest; carry set if the table is full.
; @out C: Set when the area table is full (area not added).
mouse_add_area:
      LDX   mouse_area_count
      CPX   #MOUSE_MAX_AREAS
      BCS   @full
      LDA   mouse_area_desc+0
      STA   mouse_area_x0l,X
      LDA   mouse_area_desc+1
      STA   mouse_area_x0h,X
      LDA   mouse_area_desc+2
      STA   mouse_area_y0,X
      LDA   mouse_area_desc+3
      STA   mouse_area_x1l,X
      LDA   mouse_area_desc+4
      STA   mouse_area_x1h,X
      LDA   mouse_area_desc+5
      STA   mouse_area_y1,X
      LDA   mouse_area_desc+6
      STA   mouse_area_tag,X
      INC   mouse_area_count
      CLC
      RTS
@full:
      SEC
      RTS

; Poll the mouse once (call per frame). On any button newly pressed since the
; last poll, records the event context and calls the click handler.
; @label MOUSE.POLL
; @kind routine
; @symbol mouse_poll
; @summary Edge-detect button presses and dispatch the click handler.
mouse_poll:
      ; newly pressed = current AND NOT prev, via current XOR (current AND prev)
      LDA   VGC_MOUSE_BUTTONS         ; current buttons
      TAX                             ; keep current for prev update
      AND   mouse_prev_btn            ; current AND prev = still-held
      STA   mouse_evt_button          ; scratch: still-held set
      TXA                             ; current
      EOR   mouse_evt_button          ; current XOR still-held = newly pressed
      STX   mouse_prev_btn            ; save current for next poll
      STA   mouse_evt_button          ; newly-pressed mask
      BEQ   @done                     ; nothing new
      ; capture click position
      LDA   VGC_MOUSE_XL
      STA   mouse_evt_xl
      LDA   VGC_MOUSE_XH
      STA   mouse_evt_xh
      LDA   VGC_MOUSE_Y
      STA   mouse_evt_y
      JSR   mouse_hit_area            ; sets mouse_evt_area
      JSR   mouse_call_handler
@done:
      RTS

; Find the first area containing (mouse_evt_x, mouse_evt_y); sets mouse_evt_area
; to its tag, or MOUSE_NO_AREA.
mouse_hit_area:
      LDX   #$00
@loop:
      CPX   mouse_area_count
      BCS   @none
      ; y >= y0 ?
      LDA   mouse_evt_y
      CMP   mouse_area_y0,X
      BCC   @next                     ; y < y0
      ; y <= y1 ?
      LDA   mouse_area_y1,X
      CMP   mouse_evt_y
      BCC   @next                     ; y1 < y
      JSR   mouse_x_ge_x0             ; carry set if x >= x0
      BCC   @next
      JSR   mouse_x_le_x1             ; carry set if x <= x1
      BCC   @next
      LDA   mouse_area_tag,X
      STA   mouse_evt_area
      RTS
@next:
      INX
      JMP   @loop
@none:
      LDA   #MOUSE_NO_AREA
      STA   mouse_evt_area
      RTS

; carry set if evt_x >= area x0 (16-bit compare, xh:xl)
mouse_x_ge_x0:
      LDA   mouse_evt_xh
      CMP   mouse_area_x0h,X
      BNE   @hi
      LDA   mouse_evt_xl
      CMP   mouse_area_x0l,X
@hi:
      RTS

; carry set if evt_x <= area x1 (i.e. x1 >= evt_x)
mouse_x_le_x1:
      LDA   mouse_area_x1h,X
      CMP   mouse_evt_xh
      BNE   @hi
      LDA   mouse_area_x1l,X
      CMP   mouse_evt_xl
@hi:
      RTS

; Tail-call the client click handler (no-op if none registered). The handler's
; RTS returns to mouse_poll.
mouse_call_handler:
      LDA   mouse_click_vecl
      ORA   mouse_click_vech
      BEQ   @none
      JMP   (mouse_click_vecl)
@none:
      RTS

.endif

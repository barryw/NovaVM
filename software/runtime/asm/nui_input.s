; =====================================================================
;  Nova NDK — nui_input.s
;
;  nui keyboard input: blocking key read with ANSI arrow-key decoding,
;  plus a key-drain helper. Pulled when a caller (dialog wait_key, list
;  picker) uses nui_read_key; dropped otherwise.
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

.include "nui.inc"

      .segment "CODE"

nui_read_key_wait_byte:
      LDY   #$20
@outer:
      LDX   #$FF
@inner:
      LDA   VGC_CHARIN
      BNE   @done
      DEX
      BNE   @inner
      DEY
      BNE   @outer
      LDA   #$00
@done:
      RTS

nui_read_key:
@loop:
      LDA   VGC_CHARIN
      BEQ   @loop
      CMP   #NUI_KEY_ESCAPE
      BEQ   @escape
      RTS
@escape:
      JSR   nui_read_key_wait_byte
      BEQ   @plain_escape
      CMP   #'['
      BEQ   @csi
      CMP   #'O'
      BEQ   @csi
@plain_escape:
      LDA   #NUI_KEY_ESCAPE
      RTS
@csi:
      JSR   nui_read_key_wait_byte
      BEQ   @plain_escape
      CMP   #'A'
      BEQ   @ansi_up
      CMP   #'a'
      BEQ   @ansi_up
      CMP   #'B'
      BEQ   @ansi_down
      CMP   #'b'
      BEQ   @ansi_down
      CMP   #'C'
      BEQ   @ansi_right
      CMP   #'c'
      BEQ   @ansi_right
      CMP   #'D'
      BEQ   @ansi_left
      CMP   #'d'
      BEQ   @ansi_left
      CMP   #'Z'
      BEQ   @ansi_backtab
      CMP   #'z'
      BEQ   @ansi_backtab
      BRA   @plain_escape
@ansi_up:
      LDA   #NUI_KEY_UP
      RTS
@ansi_down:
      LDA   #NUI_KEY_DOWN
      RTS
@ansi_right:
      LDA   #NUI_KEY_RIGHT
      RTS
@ansi_left:
      LDA   #NUI_KEY_LEFT
      RTS
@ansi_backtab:
      LDA   #NUI_KEY_BACKTAB
      RTS

; @label NUI.DRAIN_KEYS
; @kind routine
; @symbol nui_drain_keys
; @summary Drain pending keyboard bytes before handing focus to a new modal control.
nui_drain_keys:
@loop:
      LDA   VGC_CHARIN
      BNE   @loop
      LDA   #NUI_OK
      RTS

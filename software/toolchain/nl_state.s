; Shared XRAM state access for resident NL and its worker overlay.

      .segment "BSS"
nlstate_offset:       .res 2
nlstate_index:        .res 1
nlstate_m:            .res 1
nlstate_h:            .res 1
nlstate_window_m:     .res 1
nlstate_window_h:     .res 1
nlstate_window_valid: .res 1
nlstate_saved_x:      .res 1
nlstate_saved_y:      .res 1
nlstate_value:        .res 1

      .segment "CODE"

; Read/write one byte at NLW_STATE_XADDR + nlstate_offset. Both calls preserve
; X/Y so callers can use ordinary object and section loop indexes.
nlstate_read:
      STX   nlstate_saved_x
      STY   nlstate_saved_y
      JSR   nlstate_map
      LDX   nlstate_index
      LDA   XRAM_WIN1_BASE,X
      PHA
      LDX   nlstate_saved_x
      LDY   nlstate_saved_y
      PLA
      RTS

nlstate_write:
      STA   nlstate_value
      STX   nlstate_saved_x
      STY   nlstate_saved_y
      JSR   nlstate_map
      LDX   nlstate_index
      LDA   nlstate_value
      STA   XRAM_WIN1_BASE,X
      LDX   nlstate_saved_x
      LDY   nlstate_saved_y
      RTS

nlstate_map:
      CLC
      LDA   NLW_STATE_XADDR_L
      ADC   nlstate_offset
      STA   nlstate_index
      LDA   NLW_STATE_XADDR_M
      ADC   nlstate_offset+1
      STA   nlstate_m
      LDA   NLW_STATE_XADDR_H
      ADC   #0
      STA   nlstate_h
      LDA   nlstate_window_valid
      BEQ   @map
      LDA   nlstate_m
      CMP   nlstate_window_m
      BNE   @map
      LDA   nlstate_h
      CMP   nlstate_window_h
      BEQ   @ready
@map:
      STZ   XMC_W1AL
      LDA   nlstate_m
      STA   nlstate_window_m
      STA   XMC_W1AM
      LDA   nlstate_h
      STA   nlstate_window_h
      STA   XMC_W1AH
      LDA   XMC_WINCTL
      ORA   #XRAM_WIN1_ENABLE
      STA   XMC_WINCTL
      INC   nlstate_window_valid
@ready:
      RTS

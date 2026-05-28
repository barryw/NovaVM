; NovaLogo v1.0 — Logo interpreter for the Nova 6502 platform
; Task 1.1: skeleton ROM — boots, prints banner, echoes input

      .include "nova.inc"

; =====================================================================
; CODE segment — cold start and main loop
; =====================================================================
      .segment "CODE"

cold_start:
      ; Print banner
      LDX   #0
@banner:
      LDA   str_banner,X
      BEQ   @prompt            ; null terminator — done
      STA   VGC_CHAROUT
      INX
      BNE   @banner            ; always taken (string < 256)

@prompt:
      JSR   print_prompt

main_loop:
      LDA   VGC_CHARIN         ; poll keyboard
      BEQ   main_loop          ; no key waiting — spin

      ; --- handle CR ($0D) ---
      CMP   #$0D
      BNE   @not_cr
      LDA   #$0D
      STA   VGC_CHAROUT        ; carriage return
      LDA   #$0A
      STA   VGC_CHAROUT        ; line feed
      JSR   print_prompt
      BRA   main_loop

@not_cr:
      ; --- ignore backspace/delete for now (Task 1.4) ---
      CMP   #$08
      BEQ   main_loop
      CMP   #$14
      BEQ   main_loop
      CMP   #$7F
      BEQ   main_loop

      ; --- printable character: echo it ---
      STA   VGC_CHAROUT
      BRA   main_loop

; ---------------------------------------------------------------------
; print_prompt — prints "? " to the screen
; ---------------------------------------------------------------------
print_prompt:
      LDX   #0
@lp:  LDA   str_prompt,X
      BEQ   @done
      STA   VGC_CHAROUT
      INX
      BNE   @lp
@done:
      RTS

; =====================================================================
; RODATA segment — string constants
; =====================================================================
      .segment "RODATA"

str_banner:
      .byte "NOVALOGO v1.0", $0D, $0A
      .byte "39934 BYTES FREE", $0D, $0A, $0D, $0A
      .byte 0

str_prompt:
      .byte "? ", 0

; =====================================================================
; MONITOR segment — reset handler at $FFD7
; =====================================================================
      .segment "MONITOR"
      .org  $FFD7

RES_vec:
      CLD                     ; clear decimal mode
      LDX   #$FF              ; empty stack
      TXS                     ; set the stack
      JMP   cold_start

; IRQ/NMI stubs
NMI_stub:
      RTI

IRQ_stub:
      RTI

; =====================================================================
; VECTORS segment — 6502 hardware vectors at $FFFA
; =====================================================================
      .segment "VECTORS"
      .org  $FFFA

      .word NMI_stub           ; $FFFA — NMI vector
      .word RES_vec            ; $FFFC — RESET vector
      .word IRQ_stub           ; $FFFE — IRQ vector

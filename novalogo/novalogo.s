; NovaLogo v1.0 — Logo interpreter for the Nova 6502 platform
; Task 1.4: line input buffer with backspace handling

      .include "nova.inc"
      .include "heap.s"
      .include "tokens.s"
      .include "tokenizer.s"
      .include "eval.s"
      .include "builtins.s"
      .include "lists.s"
      .include "vars.s"
      .include "procedures.s"
      .include "gc.s"

; =====================================================================
; ZEROPAGE segment — interpreter zero-page variables
; =====================================================================
      .segment "ZEROPAGE"

buf_idx:      .res 1          ; current position in input_buf

; =====================================================================
; BSS segment — runtime buffers
; =====================================================================
      .segment "BSS"

input_buf:    .res 128        ; input line buffer

; =====================================================================
; CODE segment — cold start and main loop
; =====================================================================
      .segment "CODE"

cold_start:
      JSR   heap_init
      JSR   var_init
      JSR   proc_init
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
      JSR   read_line
      JSR   tokenize_line
      JSR   check_to_command
      BCS   @was_to               ; carry set = TO was handled
      JSR   eval_line
@was_to:
      ; Reclaim dead heap objects (tokens, list cells, etc.)
      JSR   gc_collect
      JSR   print_prompt
      BRA   main_loop

; ---------------------------------------------------------------------
; check_to_command — check if first token is TO; if so, collect procedure
;   Output: carry set = was TO (proc_collect called), carry clear = not TO
;   Clobbers: A, Y, ptr_lo/hi, eval_cur
; ---------------------------------------------------------------------
check_to_command:
      ; Is there a token list?
      LDA   tok_head_lo
      ORA   tok_head_hi
      BEQ   @not_to

      ; Check first token: must be TOK_WORD
      LDA   tok_head_lo
      STA   ptr_lo
      LDA   tok_head_hi
      STA   ptr_hi
      LDY   #TOK_TAG
      LDA   (ptr_lo),Y
      CMP   #TOK_WORD
      BNE   @not_to

      ; Check if word is "TO" (2 chars, 'T','O')
      LDY   #TOK_PAYLOAD
      LDA   (ptr_lo),Y           ; length
      CMP   #2
      BNE   @not_to
      LDY   #TOK_PAYLOAD+1
      LDA   (ptr_lo),Y
      CMP   #'T'
      BNE   @not_to
      INY
      LDA   (ptr_lo),Y
      CMP   #'O'
      BNE   @not_to

      ; It's TO — set up eval_cur past the TO token and call proc_collect
      LDA   tok_head_lo
      STA   eval_cur_lo
      LDA   tok_head_hi
      STA   eval_cur_hi
      JSR   eval_advance          ; advance past TO token
      JSR   proc_collect
      SEC
      RTS

@not_to:
      CLC
      RTS

; ---------------------------------------------------------------------
; read_line — read a line of input into input_buf, null-terminated
;   On return: input_buf contains the line, buf_idx = length
; ---------------------------------------------------------------------
read_line:
      STZ   buf_idx            ; reset buffer index to 0

@poll:
      LDA   VGC_CHARIN         ; poll keyboard
      BEQ   @poll              ; no key waiting — spin

      ; --- handle CR ($0D) ---
      CMP   #$0D
      BNE   @not_cr
      LDX   buf_idx
      STZ   input_buf,X        ; null-terminate
      LDA   #$0D
      STA   VGC_CHAROUT        ; carriage return
      LDA   #$0A
      STA   VGC_CHAROUT        ; line feed
      RTS

@not_cr:
      ; --- handle backspace ($08, $14, $7F) ---
      CMP   #$08
      BEQ   @backspace
      CMP   #$14
      BEQ   @backspace
      CMP   #$7F
      BEQ   @backspace

      ; --- buffer full? ---
      LDX   buf_idx
      CPX   #127
      BCS   @poll              ; at capacity — ignore keystroke

      ; --- store and echo printable character ---
      STA   input_buf,X        ; store in buffer
      STA   VGC_CHAROUT        ; echo to screen
      INC   buf_idx
      BRA   @poll

@backspace:
      LDX   buf_idx
      BEQ   @poll              ; nothing to delete — ignore
      DEX
      STX   buf_idx
      LDA   #$08
      STA   VGC_CHAROUT        ; move cursor left and erase
      BRA   @poll

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

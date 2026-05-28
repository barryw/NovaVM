; eval.s — evaluator: walk token list and dispatch commands

; =====================================================================
; ZEROPAGE segment — evaluator variables
; =====================================================================
      .segment "ZEROPAGE"

eval_type:      .res 1    ; result type: $00=number, $01=word
eval_val_hi:    .res 1    ; number high byte or word ptr high
eval_val_lo:    .res 1    ; number low byte or word ptr low
eval_val_frac:  .res 1    ; number fractional byte
eval_cur_lo:    .res 1    ; current token pointer (low)
eval_cur_hi:    .res 1    ; current token pointer (high)

; =====================================================================
; BSS segment — scratch for number printing
; =====================================================================
      .segment "BSS"

pn_buf:         .res 8    ; decimal digit buffer for print_number

; =====================================================================
; CODE segment — evaluator routines
; =====================================================================
      .segment "CODE"

; ---------------------------------------------------------------------
; eval_line — evaluate the token list built by tokenize_line
;   Input:  tok_head_lo/hi = first token
;   Output: commands executed, errors printed
; ---------------------------------------------------------------------
eval_line:
      LDA   tok_head_lo
      STA   eval_cur_lo
      LDA   tok_head_hi
      STA   eval_cur_hi

eval_loop:
      ; Check for end of list
      LDA   eval_cur_lo
      ORA   eval_cur_hi
      BEQ   @done

      ; Read tag of current token
      LDA   eval_cur_lo
      STA   ptr_lo
      LDA   eval_cur_hi
      STA   ptr_hi
      LDY   #TOK_TAG
      LDA   (ptr_lo),Y

      ; Dispatch based on tag
      CMP   #TOK_WORD
      BEQ   @do_command

      ; Bare expression — skip it for now (could error)
      ; Advance to next token
      JSR   eval_advance
      BRA   eval_loop

@do_command:
      ; Look up the word in the builtin table
      JSR   lookup_builtin
      BCS   @unknown

      ; Found: handler addr is in ptr2_lo/hi, arity in A
      ; Save arity
      TAX
      ; Advance past the command word token
      JSR   eval_advance

      ; Evaluate arguments (X = arity count)
      CPX   #0
      BEQ   @call_handler
@eval_args:
      PHX                   ; save remaining arg count
      JSR   eval_expr
      BCS   @arg_error
      PLX
      DEX
      BNE   @eval_args

@call_handler:
      ; Call the handler via indirect JMP
      JMP   (ptr2_lo)
      ; handlers must JMP back to eval_loop via eval_continue

@arg_error:
      PLX                   ; discard saved arity counter
      ; Print error and abort line
      LDX   #0
@ae_lp:
      LDA   str_notenough,X
      BEQ   @ae_done
      STA   VGC_CHAROUT
      INX
      BNE   @ae_lp
@ae_done:
      JSR   eval_newline
      RTS

@unknown:
      ; Print "I don't know how to " + the word
      LDX   #0
@unk_lp:
      LDA   str_idk,X
      BEQ   @unk_word
      STA   VGC_CHAROUT
      INX
      BNE   @unk_lp
@unk_word:
      ; Print the word from the token payload
      LDA   eval_cur_lo
      STA   ptr_lo
      LDA   eval_cur_hi
      STA   ptr_hi
      LDY   #TOK_PAYLOAD        ; length byte
      LDA   (ptr_lo),Y
      TAX                       ; X = length
      LDY   #TOK_PAYLOAD+1      ; first char
@unk_ch:
      LDA   (ptr_lo),Y
      STA   VGC_CHAROUT
      INY
      DEX
      BNE   @unk_ch
      JSR   eval_newline
      RTS

@done:
      RTS

; ---------------------------------------------------------------------
; eval_continue — handlers JMP here to resume the eval loop
; ---------------------------------------------------------------------
eval_continue:
      JMP   eval_loop

; ---------------------------------------------------------------------
; eval_advance — advance eval_cur to the next token in the list
;   Reads next pointer from current token, stores in eval_cur
; ---------------------------------------------------------------------
eval_advance:
      LDA   eval_cur_lo
      STA   ptr_lo
      LDA   eval_cur_hi
      STA   ptr_hi
      LDY   #TOK_NEXT_LO
      LDA   (ptr_lo),Y
      TAX
      INY
      LDA   (ptr_lo),Y
      STA   eval_cur_hi
      STX   eval_cur_lo
      RTS

; ---------------------------------------------------------------------
; eval_expr — evaluate one expression from the current token
;   Output: eval_type, eval_val_hi/lo/frac set
;           Carry clear = success, Carry set = error (no token)
;   Advances eval_cur past the consumed token
; ---------------------------------------------------------------------
eval_expr:
      ; Check for end of list
      LDA   eval_cur_lo
      ORA   eval_cur_hi
      BNE   @have_token
      SEC                       ; no more tokens
      RTS

@have_token:
      LDA   eval_cur_lo
      STA   ptr_lo
      LDA   eval_cur_hi
      STA   ptr_hi
      LDY   #TOK_TAG
      LDA   (ptr_lo),Y

      CMP   #TOK_NUMBER
      BEQ   @number
      CMP   #TOK_QUOTE
      BEQ   @quote
      ; For now, anything else is an error
      SEC
      RTS

@number:
      ; Copy 3-byte payload to eval_val
      LDY   #TOK_PAYLOAD
      LDA   (ptr_lo),Y         ; int_hi
      STA   eval_val_hi
      INY
      LDA   (ptr_lo),Y         ; int_lo
      STA   eval_val_lo
      INY
      LDA   (ptr_lo),Y         ; frac
      STA   eval_val_frac
      STZ   eval_type           ; $00 = number
      JSR   eval_advance
      CLC
      RTS

@quote:
      ; Point eval_val at the payload (length byte + chars)
      ; Pointer = ptr_lo/hi + TOK_PAYLOAD
      CLC
      LDA   ptr_lo
      ADC   #TOK_PAYLOAD
      STA   eval_val_lo
      LDA   ptr_hi
      ADC   #0
      STA   eval_val_hi
      STZ   eval_val_frac
      LDA   #$01
      STA   eval_type           ; $01 = word
      JSR   eval_advance
      CLC
      RTS

; ---------------------------------------------------------------------
; eval_newline — print CR+LF
; ---------------------------------------------------------------------
eval_newline:
      LDA   #$0D
      STA   VGC_CHAROUT
      LDA   #$0A
      STA   VGC_CHAROUT
      RTS

; ---------------------------------------------------------------------
; print_number — print eval_val as a signed decimal number
;   Input: eval_val_hi/lo/frac (16.8 fixed-point, signed)
;   Clobbers: A, X, Y, num_val_lo/hi, num_tmp_lo/hi
; ---------------------------------------------------------------------
print_number:
      ; Check sign — if negative, print '-' and negate
      LDA   eval_val_hi
      BPL   @positive
      ; Print minus sign
      LDA   #'-'
      STA   VGC_CHAROUT
      ; Negate 3-byte value (two's complement)
      LDA   eval_val_frac
      EOR   #$FF
      CLC
      ADC   #1
      STA   eval_val_frac
      LDA   eval_val_lo
      EOR   #$FF
      ADC   #0
      STA   eval_val_lo
      LDA   eval_val_hi
      EOR   #$FF
      ADC   #0
      STA   eval_val_hi

@positive:
      ; Print integer part (eval_val_hi:eval_val_lo as 16-bit unsigned)
      JSR   print_uint16

      ; Print fractional part if non-zero
      LDA   eval_val_frac
      BNE   @has_frac
      RTS
@has_frac:

      ; Print decimal point
      LDA   #'.'
      STA   VGC_CHAROUT

      ; Convert frac byte to decimal digits
      ; frac * 10 / 256 = first digit, repeat with remainder
      LDA   eval_val_frac
      STA   num_tmp_lo
      STZ   num_tmp_hi

      ; First digit: (frac * 10) >> 8
      ; frac * 10 = (frac * 8) + (frac * 2)
      LDA   num_tmp_lo
      ASL
      STA   num_val_lo          ; frac * 2
      LDA   num_tmp_hi
      ROL
      STA   num_val_hi

      ASL   num_val_lo
      ROL   num_val_hi
      ASL   num_val_lo
      ROL   num_val_hi          ; frac * 8

      CLC
      LDA   num_tmp_lo
      ASL                       ; frac * 2 (low byte)
      ADC   num_val_lo
      STA   num_val_lo
      LDA   num_tmp_hi
      ROL
      ADC   num_val_hi
      STA   num_val_hi          ; num_val = frac * 10

      ; High byte = first decimal digit
      LDA   num_val_hi
      ORA   #'0'
      STA   VGC_CHAROUT

      ; Remainder = low byte → second digit
      LDA   num_val_lo
      STA   num_tmp_lo
      STZ   num_tmp_hi

      ; Second digit: (remainder * 10) >> 8
      LDA   num_tmp_lo
      ASL
      STA   num_val_lo
      LDA   num_tmp_hi
      ROL
      STA   num_val_hi

      ASL   num_val_lo
      ROL   num_val_hi
      ASL   num_val_lo
      ROL   num_val_hi

      CLC
      LDA   num_tmp_lo
      ASL
      ADC   num_val_lo
      STA   num_val_lo
      LDA   num_tmp_hi
      ROL
      ADC   num_val_hi
      STA   num_val_hi

      ; High byte = second digit; skip if zero (trailing)
      LDA   num_val_hi
      BEQ   @done
      ORA   #'0'
      STA   VGC_CHAROUT

@done:
      RTS

; ---------------------------------------------------------------------
; print_uint16 — print eval_val_hi:eval_val_lo as unsigned decimal
;   No leading zeros (except value 0 prints "0")
;   Clobbers: A, X, Y, num_val_lo/hi, num_tmp_lo/hi
; ---------------------------------------------------------------------
print_uint16:
      ; Copy to working regs
      LDA   eval_val_lo
      STA   num_val_lo
      LDA   eval_val_hi
      STA   num_val_hi

      LDX   #0                  ; digit buffer index
      LDY   #0                  ; power-of-10 table index

@div_loop:
      STZ   num_tmp_lo          ; digit counter for this power
@sub_loop:
      ; Subtract current power of 10
      SEC
      LDA   num_val_lo
      SBC   pow10_lo,Y
      PHA
      LDA   num_val_hi
      SBC   pow10_hi,Y
      BCC   @sub_done           ; underflow — done with this digit
      STA   num_val_hi
      PLA
      STA   num_val_lo
      INC   num_tmp_lo
      BRA   @sub_loop
@sub_done:
      PLA                       ; discard low byte from failed subtract
      LDA   num_tmp_lo
      STA   pn_buf,X
      INX
      INY
      CPY   #5                  ; 5 powers (10000,1000,100,10,1)
      BCC   @div_loop

      ; Print digits, suppressing leading zeros
      LDY   #0                  ; suppress flag (0=suppress)
      LDX   #0
@print_loop:
      LDA   pn_buf,X
      BNE   @nonzero
      CPY   #0
      BEQ   @maybe_skip        ; still suppressing
@nonzero:
      LDY   #1                  ; seen nonzero digit
      ORA   #'0'
      STA   VGC_CHAROUT
@maybe_skip:
      INX
      CPX   #4
      BCC   @print_loop
      ; Always print the last digit (ones place)
      LDA   pn_buf+4
      ORA   #'0'
      STA   VGC_CHAROUT
      RTS

; =====================================================================
; RODATA — evaluator strings and tables
; =====================================================================
      .segment "RODATA"

str_idk:
      .byte "I DON'T KNOW HOW TO ", 0

str_notenough:
      .byte "NOT ENOUGH INPUTS", 0

; Powers of 10 table (16-bit): 10000, 1000, 100, 10, 1
pow10_lo:
      .byte <10000, <1000, <100, <10, <1
pow10_hi:
      .byte >10000, >1000, >100, >10, >1

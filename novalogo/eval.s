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
eval_left_hi:   .res 1    ; saved left operand high byte
eval_left_lo:   .res 1    ; saved left operand low byte
eval_left_frac: .res 1    ; saved left operand fractional byte
eval_op:        .res 1    ; infix operator character
handler_lo:     .res 1    ; saved handler address for JMP
handler_hi:     .res 1

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
      ; Save handler address — eval_expr may clobber ptr2
      LDX   ptr2_lo
      STX   handler_lo
      LDX   ptr2_hi
      STX   handler_hi
      ; Save arity across eval_advance (which clobbers X)
      TAX
      PHX
      ; Advance past the command word token
      JSR   eval_advance
      PLX

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
      JMP   (handler_lo)
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
      CMP   #TOK_VARREF
      BEQ   @varref
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
      JMP   eval_check_infix    ; check for infix operator

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

@varref:
      ; Variable reference — :NAME
      ; Payload is length-prefixed name at token + TOK_PAYLOAD
      CLC
      LDA   ptr_lo
      ADC   #TOK_PAYLOAD
      STA   ptr_lo
      LDA   ptr_hi
      ADC   #0
      STA   ptr_hi
      JSR   var_get
      BCC   @var_ok

      ; Not found — print "NAME has no value"
      ; ptr_lo/hi was clobbered by var_get, reload from eval_cur
      LDA   eval_cur_lo
      STA   ptr_lo
      LDA   eval_cur_hi
      STA   ptr_hi
      LDY   #TOK_PAYLOAD          ; length byte
      LDA   (ptr_lo),Y
      TAX                         ; X = name length
      LDY   #TOK_PAYLOAD+1
@vr_name:
      LDA   (ptr_lo),Y
      STA   VGC_CHAROUT
      INY
      DEX
      BNE   @vr_name
      LDX   #0
@vr_msg:
      LDA   str_no_value,X
      BEQ   @vr_done
      STA   VGC_CHAROUT
      INX
      BNE   @vr_msg
@vr_done:
      JSR   eval_newline
      JSR   eval_advance
      SEC
      RTS

@var_ok:
      ; eval_type and eval_val already set by var_get
      JSR   eval_advance
      JMP   eval_check_infix      ; allow :X + 5 etc.

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
; eval_check_infix — if next token is TOK_INFIX, evaluate binary op
;   Entry: eval_val holds the left operand (primary value)
;   Exit:  eval_val holds the result, carry clear = success
; ---------------------------------------------------------------------
eval_check_infix:
      ; Is there a next token?
      LDA   eval_cur_lo
      ORA   eval_cur_hi
      BEQ   @no_infix

      ; Peek at the next token's tag
      LDA   eval_cur_lo
      STA   ptr_lo
      LDA   eval_cur_hi
      STA   ptr_hi
      LDY   #TOK_TAG
      LDA   (ptr_lo),Y
      CMP   #TOK_INFIX
      BNE   @no_infix

      ; Save the left operand to ZP scratch
      LDA   eval_val_hi
      STA   eval_left_hi
      LDA   eval_val_lo
      STA   eval_left_lo
      LDA   eval_val_frac
      STA   eval_left_frac

      ; Read operator character from payload
      LDY   #TOK_PAYLOAD
      LDA   (ptr_lo),Y
      STA   eval_op

      ; Advance past the infix token
      JSR   eval_advance

      ; Evaluate the right operand (recursive call)
      JSR   eval_expr
      BCS   @err_rhs

      ; Dispatch based on operator character
      LDA   eval_op
      CMP   #'+'
      BEQ   @do_add
      CMP   #'-'
      BEQ   @do_sub
      CMP   #'*'
      BEQ   @do_mul
      CMP   #'/'
      BEQ   @do_div
      ; Unknown operator — return left value unchanged
      LDA   eval_left_hi
      STA   eval_val_hi
      LDA   eval_left_lo
      STA   eval_val_lo
      LDA   eval_left_frac
      STA   eval_val_frac

@no_infix:
      CLC
      RTS

@err_rhs:
      SEC
      RTS

      ; ----- Addition: left + right (24-bit: frac, lo, hi) -----
@do_add:
      CLC
      LDA   eval_left_frac
      ADC   eval_val_frac
      STA   eval_val_frac
      LDA   eval_left_lo
      ADC   eval_val_lo
      STA   eval_val_lo
      LDA   eval_left_hi
      ADC   eval_val_hi
      STA   eval_val_hi
      CLC
      RTS

      ; ----- Subtraction: left - right (24-bit) -----
@do_sub:
      SEC
      LDA   eval_left_frac
      SBC   eval_val_frac
      STA   eval_val_frac
      LDA   eval_left_lo
      SBC   eval_val_lo
      STA   eval_val_lo
      LDA   eval_left_hi
      SBC   eval_val_hi
      STA   eval_val_hi
      CLC
      RTS

      ; ----- Multiplication: left * right via coprocessor -----
      ; Integer-only for this pass (frac ignored)
@do_mul:
      LDA   eval_left_lo
      STA   MATH_MUL16_A_LO
      LDA   eval_left_hi
      STA   MATH_MUL16_A_HI
      LDA   eval_val_lo
      STA   MATH_MUL16_B_LO
      LDA   eval_val_hi
      STA   MATH_MUL16_B_HI     ; write triggers multiply
      LDA   MATH_RES0
      STA   eval_val_lo
      LDA   MATH_RES1
      STA   eval_val_hi
      STZ   eval_val_frac
      CLC
      RTS

      ; ----- Division: left / right via coprocessor -----
      ; Signed 32÷16: dividend = sign-extended left integer, divisor = right integer
@do_div:
      ; Check for divide by zero
      LDA   eval_val_lo
      ORA   eval_val_hi
      BNE   @div_ok
      ; Division by zero — return 0
      STZ   eval_val_frac
      STZ   eval_val_lo
      STZ   eval_val_hi
      CLC
      RTS
@div_ok:
      ; Write 32-bit dividend (sign-extend left integer to 32 bits)
      LDA   eval_left_lo
      STA   MATH_DIV_N_LO
      LDA   eval_left_hi
      STA   MATH_DIV_N_1
      ; Sign-extend high word
      LDA   eval_left_hi
      ORA   #$7F                 ; set bits 0-6
      BMI   @neg_ext             ; if bit 7 was set, A=$FF (negative)
      LDA   #$00                 ; positive: extend with $00
      BRA   @write_ext
@neg_ext:                        ; A is already $FF
@write_ext:
      STA   MATH_DIV_N_2
      STA   MATH_DIV_N_HI
      ; Write 16-bit divisor (right operand integer)
      LDA   eval_val_lo
      STA   MATH_DIV_D_LO
      LDA   eval_val_hi
      STA   MATH_DIV_D_HI       ; write triggers divide
      ; Read quotient from result
      LDA   MATH_RES0
      STA   eval_val_lo
      LDA   MATH_RES1
      STA   eval_val_hi
      STZ   eval_val_frac
      CLC
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

str_no_value:
      .byte " HAS NO VALUE", 0

; Powers of 10 table (16-bit): 10000, 1000, 100, 10, 1
pow10_lo:
      .byte <10000, <1000, <100, <10, <1
pow10_hi:
      .byte >10000, >1000, >100, >10, >1

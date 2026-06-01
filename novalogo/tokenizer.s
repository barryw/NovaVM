; tokenizer.s — scan input_buf into a linked list of token nodes

; =====================================================================
; ZEROPAGE segment — tokenizer variables
; =====================================================================
      .segment "ZEROPAGE"

tok_head_lo:  .res 1          ; head of token list (first token)
tok_head_hi:  .res 1
tok_tail_lo:  .res 1          ; tail of token list (for appending)
tok_tail_hi:  .res 1
tok_src:      .res 1          ; current scan position in input_buf
ptr2_lo:      .res 1          ; second general-purpose pointer (low byte)
ptr2_hi:      .res 1          ; second general-purpose pointer (high byte)
tok_sign:     .res 1          ; sign flag for number parsing (0=pos, 1=neg)

; =====================================================================
; BSS segment — scratch for number parsing
; =====================================================================
      .segment "BSS"

num_val_lo:   .res 1          ; integer accumulator low
num_val_hi:   .res 1          ; integer accumulator high
num_tmp_lo:   .res 1          ; temp for multiply / frac result
num_tmp_hi:   .res 1          ; temp for multiply / frac raw

; =====================================================================
; CODE segment — tokenizer routines
; =====================================================================
      .segment "CODE"

; ---------------------------------------------------------------------
; tokenize_line — scan input_buf and build a linked list of tokens
;   Input:  input_buf contains null-terminated text
;   Output: tok_head_lo/hi = first token ($0000 if empty line)
;   Clobbers: A, X, Y, ptr_lo/hi, ptr2_lo/hi
; ---------------------------------------------------------------------
tokenize_line:
      STZ   tok_head_lo
      STZ   tok_head_hi
      STZ   tok_tail_lo
      STZ   tok_tail_hi
      STZ   tok_src
      ; fall through to scan loop

; Non-local entry for scan loop — reachable from all routines
tok_scan_next:
      ; --- skip whitespace ---
      LDX   tok_src
@skip_ws:
      LDA   input_buf,X
      CMP   #$20                 ; space
      BEQ   @ws_next
      CMP   #$09                 ; tab
      BNE   @ws_done
@ws_next:
      INX
      BRA   @skip_ws
@ws_done:
      STX   tok_src

      ; --- check for end of input ---
      CMP   #0
      BNE   @not_end
      RTS                        ; done — list complete
@not_end:

      ; --- colon → TOK_VARREF ---
      CMP   #':'
      BNE   @not_colon
      INC   tok_src              ; skip ':'
      JMP   tok_scan_varref
@not_colon:

      ; --- quote → TOK_QUOTE ---
      CMP   #'"'
      BNE   @not_quote
      INC   tok_src              ; skip '"'
      JMP   tok_scan_quote
@not_quote:

      ; --- brackets ---
      CMP   #'['
      BNE   @not_lb
      LDA   #TOK_LBRACKET
      JMP   tok_emit_simple
@not_lb:
      CMP   #']'
      BNE   @not_rb
      LDA   #TOK_RBRACKET
      JMP   tok_emit_simple
@not_rb:

      ; --- parens ---
      CMP   #'('
      BNE   @not_lp
      LDA   #TOK_LPAREN
      JMP   tok_emit_simple
@not_lp:
      CMP   #')'
      BNE   @not_rp
      LDA   #TOK_RPAREN
      JMP   tok_emit_simple
@not_rp:

      ; --- infix operators (not minus) ---
      CMP   #'+'
      BEQ   @is_infix
      CMP   #'*'
      BEQ   @is_infix
      CMP   #'/'
      BEQ   @is_infix
      CMP   #'<'
      BEQ   @is_infix
      CMP   #'>'
      BEQ   @is_infix
      CMP   #'='
      BEQ   @is_infix
      BRA   @not_infix
@is_infix:
      JMP   tok_emit_infix
@not_infix:

      ; --- minus: could be infix or negative number ---
      CMP   #'-'
      BNE   @not_minus
      ; peek at next character
      LDX   tok_src
      LDA   input_buf+1,X
      CMP   #'0'
      BCC   @minus_infix
      CMP   #'9'+1
      BCC   @minus_number
@minus_infix:
      LDA   #'-'
      JMP   tok_emit_infix
@minus_number:
      JMP   tok_parse_number
@not_minus:

      ; --- digit → TOK_NUMBER ---
      CMP   #'0'
      BCC   @not_digit
      CMP   #'9'+1
      BCC   @is_digit
@not_digit:

      ; --- default: TOK_WORD ---
      JMP   tok_scan_word

@is_digit:
      JMP   tok_parse_number

; ---------------------------------------------------------------------
; tok_emit_simple — emit a 3-byte node (tag only, no payload)
;   Input: A = tag byte, tok_src points at the character
; ---------------------------------------------------------------------
tok_emit_simple:
      PHA                        ; save tag
      LDA   #3
      LDX   #ATYPE_TOKEN
      JSR   heap_alloc
      BCS   @oom
      PLA
      LDY   #TOK_TAG
      STA   (ptr_lo),Y
      LDY   #TOK_NEXT_LO
      LDA   #0
      STA   (ptr_lo),Y
      INY
      STA   (ptr_lo),Y
      INC   tok_src              ; advance past the character
      JMP   tok_append_node
@oom:
      PLA
      RTS

; ---------------------------------------------------------------------
; tok_emit_infix — emit a 4-byte node (tag + operator char)
;   Input: A = operator character, tok_src points at the operator
; ---------------------------------------------------------------------
tok_emit_infix:
      PHA                        ; save operator char
      LDA   #4
      LDX   #ATYPE_TOKEN
      JSR   heap_alloc
      BCS   @oom
      LDY   #TOK_TAG
      LDA   #TOK_INFIX
      STA   (ptr_lo),Y
      LDY   #TOK_NEXT_LO
      LDA   #0
      STA   (ptr_lo),Y
      INY
      STA   (ptr_lo),Y
      LDY   #TOK_PAYLOAD
      PLA
      STA   (ptr_lo),Y
      INC   tok_src
      JMP   tok_append_node
@oom:
      PLA
      RTS

; ---------------------------------------------------------------------
; tok_scan_word — scan a word into TOK_WORD (uppercased)
;   tok_src points at first character
; ---------------------------------------------------------------------
tok_scan_word:
      LDX   tok_src
      LDY   #0                   ; length counter
@count:
      LDA   input_buf,X
      JSR   is_word_char
      BCC   @counted
      INX
      INY
      BRA   @count
@counted:
      TYA
      BEQ   @empty
      PHA                        ; save length
      CLC
      ADC   #4                   ; node = 3 header + 1 length + chars
      LDX   #ATYPE_TOKEN
      JSR   heap_alloc
      BCS   @oom
      ; tag
      LDY   #TOK_TAG
      LDA   #TOK_WORD
      STA   (ptr_lo),Y
      ; next = null
      LDY   #TOK_NEXT_LO
      LDA   #0
      STA   (ptr_lo),Y
      INY
      STA   (ptr_lo),Y
      ; length byte
      PLA
      PHA
      LDY   #TOK_PAYLOAD
      STA   (ptr_lo),Y
      ; copy chars with uppercasing
      PLA
      TAX                        ; X = remaining
      LDY   #TOK_PAYLOAD+1       ; Y = dest offset
      STZ   num_tmp_lo           ; source offset from tok_src
@copy:
      PHX
      LDA   num_tmp_lo
      CLC
      ADC   tok_src
      TAX
      LDA   input_buf,X
      CMP   #'a'
      BCC   @noup
      CMP   #'z'+1
      BCS   @noup
      SEC
      SBC   #$20
@noup:
      STA   (ptr_lo),Y
      INY
      INC   num_tmp_lo
      PLX
      DEX
      BNE   @copy
      ; advance tok_src
      LDA   num_tmp_lo
      CLC
      ADC   tok_src
      STA   tok_src
      JMP   tok_append_node
@oom:
      PLA
@empty:
      RTS

; ---------------------------------------------------------------------
; tok_scan_varref — scan TOK_VARREF after ':'
;   tok_src points at first char after ':'
; ---------------------------------------------------------------------
tok_scan_varref:
      LDX   tok_src
      LDY   #0
@count:
      LDA   input_buf,X
      JSR   is_word_char
      BCC   @counted
      INX
      INY
      BRA   @count
@counted:
      TYA
      BEQ   @empty
      PHA
      CLC
      ADC   #4
      LDX   #ATYPE_TOKEN
      JSR   heap_alloc
      BCS   @oom
      LDY   #TOK_TAG
      LDA   #TOK_VARREF
      STA   (ptr_lo),Y
      LDY   #TOK_NEXT_LO
      LDA   #0
      STA   (ptr_lo),Y
      INY
      STA   (ptr_lo),Y
      PLA
      PHA
      LDY   #TOK_PAYLOAD
      STA   (ptr_lo),Y
      PLA
      TAX
      LDY   #TOK_PAYLOAD+1
      STZ   num_tmp_lo
@copy:
      PHX
      LDA   num_tmp_lo
      CLC
      ADC   tok_src
      TAX
      LDA   input_buf,X
      CMP   #'a'
      BCC   @noup
      CMP   #'z'+1
      BCS   @noup
      SEC
      SBC   #$20
@noup:
      STA   (ptr_lo),Y
      INY
      INC   num_tmp_lo
      PLX
      DEX
      BNE   @copy
      LDA   num_tmp_lo
      CLC
      ADC   tok_src
      STA   tok_src
      JMP   tok_append_node
@oom:
      PLA
@empty:
      RTS

; ---------------------------------------------------------------------
; tok_scan_quote — scan TOK_QUOTE after '"'
;   tok_src points at first char after '"'
; ---------------------------------------------------------------------
tok_scan_quote:
      LDX   tok_src
      LDY   #0
@count:
      LDA   input_buf,X
      JSR   is_word_char
      BCC   @counted
      INX
      INY
      BRA   @count
@counted:
      TYA
      BEQ   @empty
      PHA
      CLC
      ADC   #4
      LDX   #ATYPE_TOKEN
      JSR   heap_alloc
      BCS   @oom
      LDY   #TOK_TAG
      LDA   #TOK_QUOTE
      STA   (ptr_lo),Y
      LDY   #TOK_NEXT_LO
      LDA   #0
      STA   (ptr_lo),Y
      INY
      STA   (ptr_lo),Y
      PLA
      PHA
      LDY   #TOK_PAYLOAD
      STA   (ptr_lo),Y
      ; Copy characters — no uppercasing for quotes
      PLA
      TAX
      LDY   #TOK_PAYLOAD+1
      STZ   num_tmp_lo
@copy:
      PHX
      LDA   num_tmp_lo
      CLC
      ADC   tok_src
      TAX
      LDA   input_buf,X
      STA   (ptr_lo),Y
      INY
      INC   num_tmp_lo
      PLX
      DEX
      BNE   @copy
      LDA   num_tmp_lo
      CLC
      ADC   tok_src
      STA   tok_src
      JMP   tok_append_node
@oom:
      PLA
@empty:
      RTS

; ---------------------------------------------------------------------
; tok_parse_number — parse decimal integer[.fraction] into TOK_NUMBER
;   tok_src points at first digit or '-'
;   Payload: 3 bytes = int_hi, int_lo, frac (16.8 fixed-point)
; ---------------------------------------------------------------------
tok_parse_number:
      STZ   num_val_lo
      STZ   num_val_hi
      STZ   tok_sign              ; 0 = positive
      LDX   tok_src
      LDA   input_buf,X
      CMP   #'-'
      BNE   @parse_int
      LDA   #1
      STA   tok_sign
      INC   tok_src

      ; Parse integer digits
@parse_int:
      LDX   tok_src
      LDA   input_buf,X
      CMP   #'0'
      BCC   @int_done
      CMP   #'9'+1
      BCS   @int_done
      SEC
      SBC   #'0'                 ; A = digit 0..9
      PHA                        ; save digit

      ; Multiply num_val by 10: (val << 3) + (val << 1)
      ; Compute val << 1
      LDA   num_val_lo
      ASL
      STA   num_tmp_lo
      LDA   num_val_hi
      ROL
      STA   num_tmp_hi           ; tmp = val * 2

      ; Compute val << 3 = (val * 2) << 2
      ASL   num_tmp_lo
      ROL   num_tmp_hi
      ASL   num_tmp_lo
      ROL   num_tmp_hi           ; tmp = val * 8

      ; val*10 = val*8 + val*2. Recompute val*2 into num_val first, with a
      ; proper 16-bit shift (ASL low, ROL high) so the carry out of the low
      ; byte reaches the high byte — then add val*8. (The previous version
      ; fused the shift and add and CLC'd away that carry, dropping 256 from
      ; any value whose low byte had bit 7 set, e.g. 4242 -> 3986.)
      LDA   num_val_lo
      ASL
      STA   num_val_lo
      LDA   num_val_hi
      ROL
      STA   num_val_hi           ; num_val = val * 2

      CLC
      LDA   num_val_lo
      ADC   num_tmp_lo
      STA   num_val_lo
      LDA   num_val_hi
      ADC   num_tmp_hi
      STA   num_val_hi           ; num_val = val*2 + val*8 = val*10

      ; Add digit
      PLA
      CLC
      ADC   num_val_lo
      STA   num_val_lo
      BCC   @no_carry
      INC   num_val_hi
@no_carry:
      INC   tok_src
      BRA   @parse_int

@int_done:
      ; Parse fractional part
      STZ   num_tmp_lo           ; frac result
      LDX   tok_src
      LDA   input_buf,X
      CMP   #'.'
      BNE   @apply_sign
      INC   tok_src

      ; Parse up to 2 fractional digits
      STZ   num_tmp_hi           ; frac_raw accumulator
      LDY   #0                   ; digit count
@frac_loop:
      CPY   #2
      BCS   @frac_skip
      LDX   tok_src
      LDA   input_buf,X
      CMP   #'0'
      BCC   @frac_conv
      CMP   #'9'+1
      BCS   @frac_conv
      SEC
      SBC   #'0'
      PHA
      ; frac_raw = frac_raw * 10 + digit
      LDA   num_tmp_hi
      ASL
      STA   num_tmp_lo           ; *2
      ASL
      ASL                        ; A = val*8
      CLC
      ADC   num_tmp_lo           ; A = val*10
      STA   num_tmp_hi
      PLA
      CLC
      ADC   num_tmp_hi
      STA   num_tmp_hi
      INC   tok_src
      INY
      BRA   @frac_loop

@frac_skip:
      ; skip remaining digits after '.'
      LDX   tok_src
      LDA   input_buf,X
      CMP   #'0'
      BCC   @frac_conv
      CMP   #'9'+1
      BCS   @frac_conv
      INC   tok_src
      BRA   @frac_skip

@frac_conv:
      ; If only 1 digit, multiply by 10
      CPY   #1
      BNE   @frac_2d
      LDA   num_tmp_hi
      ASL
      STA   num_tmp_lo
      ASL
      ASL
      CLC
      ADC   num_tmp_lo
      STA   num_tmp_hi
@frac_2d:
      ; Convert centesimal (0..99) → 8-bit fraction
      LDA   num_tmp_hi
      JSR   frac_to_byte
      STA   num_tmp_lo

@apply_sign:
      ; Apply sign (negate 16.8 via two's complement if negative)
      LDA   tok_sign
      BEQ   @emit_num
      ; Negate: complement and add 1 across all 3 bytes
      LDA   num_tmp_lo           ; frac
      EOR   #$FF
      CLC
      ADC   #1
      STA   num_tmp_lo
      LDA   num_val_lo
      EOR   #$FF
      ADC   #0
      STA   num_val_lo
      LDA   num_val_hi
      EOR   #$FF
      ADC   #0
      STA   num_val_hi

@emit_num:
      ; Allocate 6-byte node
      LDA   #6
      LDX   #ATYPE_TOKEN
      JSR   heap_alloc
      BCS   @oom
      LDY   #TOK_TAG
      LDA   #TOK_NUMBER
      STA   (ptr_lo),Y
      LDY   #TOK_NEXT_LO
      LDA   #0
      STA   (ptr_lo),Y
      INY
      STA   (ptr_lo),Y
      ; Payload: [hi, lo, frac]
      LDY   #TOK_PAYLOAD
      LDA   num_val_hi
      STA   (ptr_lo),Y
      INY
      LDA   num_val_lo
      STA   (ptr_lo),Y
      INY
      LDA   num_tmp_lo
      STA   (ptr_lo),Y
      JMP   tok_append_node
@oom:
      RTS

; ---------------------------------------------------------------------
; frac_to_byte — convert centesimal fraction (0..99) to 8-bit (0..252)
;   Input: A = centesimal value (0..99)
;   Output: A = 8-bit fractional byte
;   Method: (A * 256) / 100 via 16÷8 long division
;   Clobbers: X, Y
; ---------------------------------------------------------------------
frac_to_byte:
      ; dividend = A:$00 (A*256), divisor = 100
      STA   num_tmp_hi
      STZ   num_tmp_lo
      LDX   #0                   ; remainder
      LDY   #16                  ; 16-bit division
@div:
      ASL   num_tmp_lo
      ROL   num_tmp_hi
      TXA
      ROL
      TAX
      CPX   #100
      BCC   @nosub
      TXA
      SEC
      SBC   #100
      TAX
      INC   num_tmp_lo
@nosub:
      DEY
      BNE   @div
      LDA   num_tmp_lo           ; quotient low byte
      RTS

; ---------------------------------------------------------------------
; tok_append_node — link ptr_lo/hi into the list, continue scanning
; ---------------------------------------------------------------------
tok_append_node:
      LDA   tok_head_lo
      ORA   tok_head_hi
      BNE   @link
      ; Empty list: head = tail = new node
      LDA   ptr_lo
      STA   tok_head_lo
      STA   tok_tail_lo
      LDA   ptr_hi
      STA   tok_head_hi
      STA   tok_tail_hi
      JMP   tok_scan_next
@link:
      ; tail->next = new node
      LDA   tok_tail_lo
      STA   ptr2_lo
      LDA   tok_tail_hi
      STA   ptr2_hi
      LDY   #TOK_NEXT_LO
      LDA   ptr_lo
      STA   (ptr2_lo),Y
      INY
      LDA   ptr_hi
      STA   (ptr2_lo),Y
      ; tail = new node
      LDA   ptr_lo
      STA   tok_tail_lo
      LDA   ptr_hi
      STA   tok_tail_hi
      JMP   tok_scan_next

; ---------------------------------------------------------------------
; is_word_char — test if A is a valid word character
;   Returns: carry set = word char, carry clear = delimiter
;   Preserves: X
; ---------------------------------------------------------------------
is_word_char:
      CMP   #0
      BEQ   @no
      CMP   #$20
      BEQ   @no
      CMP   #$09
      BEQ   @no
      CMP   #'['
      BEQ   @no
      CMP   #']'
      BEQ   @no
      CMP   #'('
      BEQ   @no
      CMP   #')'
      BEQ   @no
      CMP   #'+'
      BEQ   @no
      CMP   #'-'
      BEQ   @no
      CMP   #'*'
      BEQ   @no
      CMP   #'/'
      BEQ   @no
      CMP   #'<'
      BEQ   @no
      CMP   #'>'
      BEQ   @no
      CMP   #'='
      BEQ   @no
      SEC
      RTS
@no:
      CLC
      RTS

; =====================================================================
; print_token_count — walk the token list, count, print "N TOKENS\n"
; =====================================================================
print_token_count:
      LDX   #0
      LDA   tok_head_lo
      STA   ptr2_lo
      LDA   tok_head_hi
      STA   ptr2_hi
@walk:
      LDA   ptr2_lo
      ORA   ptr2_hi
      BEQ   @print
      INX
      LDY   #TOK_NEXT_LO
      LDA   (ptr2_lo),Y
      PHA
      INY
      LDA   (ptr2_lo),Y
      STA   ptr2_hi
      PLA
      STA   ptr2_lo
      BRA   @walk
@print:
      TXA
      JSR   print_byte_decimal
      LDX   #0
@str:
      LDA   str_tokens,X
      BEQ   @nl
      STA   VGC_CHAROUT
      INX
      BNE   @str
@nl:
      LDA   #$0D
      STA   VGC_CHAROUT
      LDA   #$0A
      STA   VGC_CHAROUT
      RTS

; ---------------------------------------------------------------------
; print_byte_decimal — print A as 1-3 decimal digits (no leading zeros)
;   Input: A = value 0..255
; ---------------------------------------------------------------------
print_byte_decimal:
      LDX   #0                   ; suppress leading zeros
      LDY   #0
@hundreds:
      CMP   #100
      BCC   @tens_setup
      SEC
      SBC   #100
      INY
      BRA   @hundreds
@tens_setup:
      PHA
      TYA
      BEQ   @skip_h
      ORA   #'0'
      STA   VGC_CHAROUT
      LDX   #1
@skip_h:
      PLA
      LDY   #0
@tens:
      CMP   #10
      BCC   @ones
      SEC
      SBC   #10
      INY
      BRA   @tens
@ones:
      PHA
      TYA
      BNE   @print_t
      CPX   #0
      BEQ   @skip_t
@print_t:
      ORA   #'0'
      STA   VGC_CHAROUT
@skip_t:
      PLA
      ORA   #'0'
      STA   VGC_CHAROUT
      RTS

; =====================================================================
; RODATA — tokenizer strings
; =====================================================================
      .segment "RODATA"

str_tokens:
      .byte " TOKENS", 0

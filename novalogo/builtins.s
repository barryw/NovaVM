; builtins.s — built-in command table and handlers

; =====================================================================
; CODE segment — builtin lookup and handlers
; =====================================================================
      .segment "CODE"

; ---------------------------------------------------------------------
; lookup_builtin — search builtin table for the current TOK_WORD
;   Input: eval_cur_lo/hi points to a TOK_WORD token
;   Output: Carry clear = found, ptr2_lo/hi = handler addr, A = arity
;           Carry set = not found
;   Clobbers: A, X, Y, num_tmp_lo/hi
; ---------------------------------------------------------------------
lookup_builtin:
      ; Save table pointer in num_tmp_lo/hi
      LDA   #<builtin_table
      STA   num_tmp_lo
      LDA   #>builtin_table
      STA   num_tmp_hi

@scan:
      ; Read name pointer from table
      LDA   num_tmp_lo
      STA   ptr2_lo
      LDA   num_tmp_hi
      STA   ptr2_hi
      LDY   #0
      LDA   (ptr2_lo),Y        ; name_ptr low
      TAX
      INY
      LDA   (ptr2_lo),Y        ; name_ptr high
      BNE   @have_entry
      CPX   #0
      BNE   @have_entry
      ; Null pointer — end of table
      SEC
      RTS

@have_entry:
      ; X = name_ptr_lo, A = name_ptr_hi
      ; Compare the table name against the token payload
      ; Token payload: at eval_cur + TOK_PAYLOAD = length byte + chars
      STA   ptr2_hi
      STX   ptr2_lo             ; ptr2 now points to table name string

      ; Get token payload pointer
      LDA   eval_cur_lo
      STA   ptr_lo
      LDA   eval_cur_hi
      STA   ptr_hi

      ; Compare length bytes
      LDY   #0
      LDA   (ptr2_lo),Y        ; table name length
      LDY   #TOK_PAYLOAD
      CMP   (ptr_lo),Y         ; token payload length
      BNE   @next

      ; Lengths match — compare chars
      TAX                       ; X = length
      LDY   #1                  ; offset into table name string (after length)
      STZ   tok_sign            ; reuse as token char offset counter
@cmp_loop:
      LDA   (ptr2_lo),Y        ; table name char
      PHY
      ; Compute token char offset: TOK_PAYLOAD + 1 + tok_sign
      LDA   tok_sign
      CLC
      ADC   #TOK_PAYLOAD+1
      TAY
      LDA   (ptr_lo),Y         ; token char
      PLY
      CMP   (ptr2_lo),Y        ; compare
      BNE   @next
      INY
      INC   tok_sign
      DEX
      BNE   @cmp_loop

      ; Match found — read handler address and arity
      ; Table entry: name_ptr(2) + handler_addr(2) + arity(1)
      ; handler is at table_base + 2, arity at table_base + 4
      LDA   num_tmp_lo
      STA   ptr2_lo
      LDA   num_tmp_hi
      STA   ptr2_hi
      LDY   #2
      LDA   (ptr2_lo),Y        ; handler low
      TAX
      INY
      LDA   (ptr2_lo),Y        ; handler high
      STA   ptr2_hi
      STX   ptr2_lo             ; ptr2 = handler address
      LDY   #4
      LDA   (ptr2_lo),Y        ; — wrong, we already overwrote ptr2
      ; Fix: re-read arity from table using num_tmp as base
      LDA   num_tmp_lo
      STA   ptr_lo
      LDA   num_tmp_hi
      STA   ptr_hi
      LDY   #4
      LDA   (ptr_lo),Y         ; arity
      CLC                       ; found
      RTS

@next:
      STZ   tok_sign            ; reset comparison counter
      ; Advance to next entry: +5 bytes (2 name + 2 handler + 1 arity)
      CLC
      LDA   num_tmp_lo
      ADC   #5
      STA   num_tmp_lo
      BCC   @next2
      INC   num_tmp_hi
@next2:
      JMP   @scan

; ---------------------------------------------------------------------
; do_print — PRINT <expr>: evaluate arg and print with newline
;   Entry: eval_val/eval_type already set with the argument
;   Must JMP to eval_continue when done
; ---------------------------------------------------------------------
do_print:
      LDA   eval_type
      BNE   @word

      ; Print number
      JSR   print_number
      JSR   eval_newline
      JMP   eval_continue

@word:
      ; Print quoted word — eval_val_lo/hi points at length+chars
      LDA   eval_val_lo
      STA   ptr_lo
      LDA   eval_val_hi
      STA   ptr_hi
      LDY   #0
      LDA   (ptr_lo),Y         ; length
      TAX
      INY
@wch:
      LDA   (ptr_lo),Y
      STA   VGC_CHAROUT
      INY
      DEX
      BNE   @wch
      JSR   eval_newline
      JMP   eval_continue

; ---------------------------------------------------------------------
; do_type — TYPE <expr>: like PRINT but no newline
; ---------------------------------------------------------------------
do_type:
      LDA   eval_type
      BNE   @word

      ; Print number (no newline)
      JSR   print_number
      JMP   eval_continue

@word:
      LDA   eval_val_lo
      STA   ptr_lo
      LDA   eval_val_hi
      STA   ptr_hi
      LDY   #0
      LDA   (ptr_lo),Y
      TAX
      INY
@wch:
      LDA   (ptr_lo),Y
      STA   VGC_CHAROUT
      INY
      DEX
      BNE   @wch
      JMP   eval_continue

; ---------------------------------------------------------------------
; do_make — MAKE "name value: store a variable
;   Arity 0 — we do our own argument evaluation
;   1. eval_expr → name (must be TOK_QUOTE → word type)
;   2. save name pointer
;   3. eval_expr → value
;   4. var_set(saved name, current value)
; ---------------------------------------------------------------------
do_make:
      ; Evaluate first argument: the name (a quoted word)
      JSR   eval_expr
      BCS   @err

      ; Must be a word (type $01)
      LDA   eval_type
      CMP   #$01
      BNE   @err

      ; Save the name pointer (points into token payload: len + chars)
      LDA   eval_val_lo
      PHA
      LDA   eval_val_hi
      PHA

      ; Evaluate second argument: the value
      JSR   eval_expr
      BCS   @err_pop

      ; Set up ptr_lo/hi → saved name, eval_val holds value
      PLA
      STA   ptr_hi
      PLA
      STA   ptr_lo

      JSR   var_set
      JMP   eval_continue

@err_pop:
      PLA
      PLA
@err:
      ; Print error
      LDX   #0
@ep:
      LDA   str_make_err,X
      BEQ   @ep_done
      STA   VGC_CHAROUT
      INX
      BNE   @ep
@ep_done:
      JSR   eval_newline
      JMP   eval_continue

; =====================================================================
; RODATA — builtin table and name strings
; =====================================================================
      .segment "RODATA"

str_make_err:
      .byte "NOT ENOUGH INPUTS TO MAKE", 0

; Name strings: length-prefixed
str_print_name:
      .byte 5, "PRINT"
str_type_name:
      .byte 4, "TYPE"
str_make_name:
      .byte 4, "MAKE"

; Builtin table: name_ptr(2) + handler_addr(2) + arity(1)
builtin_table:
      .word str_print_name
      .word do_print
      .byte 1

      .word str_type_name
      .word do_type
      .byte 1

      .word str_make_name
      .word do_make
      .byte 0                    ; arity 0: do_make handles its own args

      .word $0000               ; end sentinel

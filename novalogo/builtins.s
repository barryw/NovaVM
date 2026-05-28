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
      CMP   #VAL_LIST
      BEQ   @list
      CMP   #VAL_WORD
      BEQ   @word

      ; Print number
      JSR   print_number
      JSR   eval_newline
      JMP   eval_continue

@list:
      JSR   print_list
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
      CMP   #VAL_LIST
      BEQ   @list
      CMP   #VAL_WORD
      BEQ   @word

      ; Print number (no newline)
      JSR   print_number
      JMP   eval_continue

@list:
      JSR   print_list
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

; ---------------------------------------------------------------------
; do_repeat — REPEAT count [body]
;   Arity 0 — handles its own argument parsing
;   1. Evaluate count expression
;   2. Locate the bracketed body [...]
;   3. Execute the body count times
;   4. Resume after the closing ]
;
;   Nesting: saves/restores repeat state (body_start, body_resume,
;   repeat_count, eval_in_body) on the hardware stack so nested
;   REPEAT works correctly.
; ---------------------------------------------------------------------

      .segment "ZEROPAGE"
repeat_count_lo:  .res 1
repeat_count_hi:  .res 1
body_start_lo:    .res 1       ; first token after [
body_start_hi:    .res 1
body_resume_lo:   .res 1       ; token after ]
body_resume_hi:   .res 1

      .segment "CODE"

do_repeat:
      ; --- Evaluate the count argument ---
      JSR   eval_expr
      BCC   @count_ok
      JMP   @err_args             ; carry set = no token
@count_ok:
      ; Count must be a number
      LDA   eval_type
      BEQ   @type_ok
      JMP   @err_args
@type_ok:

      ; --- Save outer repeat state for nesting ---
      LDA   repeat_count_lo
      PHA
      LDA   repeat_count_hi
      PHA
      LDA   body_start_lo
      PHA
      LDA   body_start_hi
      PHA
      LDA   body_resume_lo
      PHA
      LDA   body_resume_hi
      PHA
      LDA   eval_in_body
      PHA

      ; Store count (integer part only, clamped to 0 if negative)
      LDA   eval_val_hi
      BMI   @zero_count           ; negative = treat as 0
      STA   repeat_count_hi
      LDA   eval_val_lo
      STA   repeat_count_lo
      BRA   @find_body

@zero_count:
      STZ   repeat_count_lo
      STZ   repeat_count_hi

@find_body:
      ; --- Current token must be TOK_LBRACKET ---
      LDA   eval_cur_lo
      ORA   eval_cur_hi
      BNE   @have_tok
      JMP   @err_bracket_pop
@have_tok:
      LDA   eval_cur_lo
      STA   ptr_lo
      LDA   eval_cur_hi
      STA   ptr_hi
      LDY   #TOK_TAG
      LDA   (ptr_lo),Y
      CMP   #TOK_LBRACKET
      BEQ   @is_bracket
      JMP   @err_bracket_pop
@is_bracket:

      ; Advance past the [
      JSR   eval_advance

      ; Save body start (first token after [)
      LDA   eval_cur_lo
      STA   body_start_lo
      LDA   eval_cur_hi
      STA   body_start_hi

      ; --- Scan forward to find the matching ] ---
      LDX   #1                    ; depth (we've consumed the [)
@scan:
      LDA   eval_cur_lo
      ORA   eval_cur_hi
      BNE   @scan_ok
      JMP   @err_bracket_pop
@scan_ok:
      LDA   eval_cur_lo
      STA   ptr_lo
      LDA   eval_cur_hi
      STA   ptr_hi
      LDY   #TOK_TAG
      LDA   (ptr_lo),Y

      CMP   #TOK_LBRACKET
      BNE   @scan_not_lb
      INX
      BRA   @scan_next
@scan_not_lb:
      CMP   #TOK_RBRACKET
      BNE   @scan_next
      DEX
      BEQ   @scan_found           ; depth == 0, found matching ]
@scan_next:
      PHX                        ; save depth across eval_advance
      JSR   eval_advance
      PLX
      BRA   @scan

@scan_found:
      ; eval_cur is at the matching ]. Save resume point (token after ]).
      JSR   eval_advance
      LDA   eval_cur_lo
      STA   body_resume_lo
      LDA   eval_cur_hi
      STA   body_resume_hi

      ; --- Execute the body repeat_count times ---
@loop:
      ; Check if STOP/OUTPUT was signaled
      LDA   proc_stopped
      BNE   @loop_done

      ; Check if count is zero
      LDA   repeat_count_lo
      ORA   repeat_count_hi
      BEQ   @loop_done

      ; Set eval_cur to body start
      LDA   body_start_lo
      STA   eval_cur_lo
      LDA   body_start_hi
      STA   eval_cur_hi

      ; Set body mode and call eval_body
      LDA   #$01
      STA   eval_in_body
      JSR   eval_body

      ; Decrement count (16-bit)
      LDA   repeat_count_lo
      BNE   @dec_lo
      DEC   repeat_count_hi
@dec_lo:
      DEC   repeat_count_lo
      BRA   @loop

@loop_done:
      ; --- Restore outer state and resume after ] ---
      JSR   repeat_restore_state

      ; Set eval_cur past the ] to resume
      LDA   body_resume_lo
      STA   eval_cur_lo
      LDA   body_resume_hi
      STA   eval_cur_hi
      JMP   eval_continue

@err_bracket_pop:
      ; Restore stack before printing error
      JSR   repeat_restore_state
      ; Fall through to error message
      LDX   #0
@eb_lp:
      LDA   str_repeat_bracket,X
      BEQ   @eb_done
      STA   VGC_CHAROUT
      INX
      BNE   @eb_lp
@eb_done:
      JSR   eval_newline
      JMP   eval_continue

@err_args:
      LDX   #0
@ea_lp:
      LDA   str_repeat_err,X
      BEQ   @ea_done
      STA   VGC_CHAROUT
      INX
      BNE   @ea_lp
@ea_done:
      JSR   eval_newline
      JMP   eval_continue

; ---------------------------------------------------------------------
; repeat_restore_state — pop 7 saved bytes from stack to ZP
;   Restores eval_in_body, body_resume, body_start, repeat_count
;   Must be called via JSR (uses return address on stack correctly)
; ---------------------------------------------------------------------
repeat_restore_state:
      ; The JSR pushes 2 bytes (return address). We need to pop our
      ; return address first, then the 7 saved bytes, then re-push
      ; the return address. Simpler: store RA in ZP temporaries.
      ; Actually, use a different approach: pull RA, pull state, push RA back.
      ; But that's awkward. Let's just inline the restore... no, better:
      ; Pull our return address, then do the pops, then JMP indirect.
      ;
      ; Actually simplest: the 7 bytes are deeper on the stack below our
      ; JSR return address. Pull RA into handler_lo/hi temp, do pops, push RA, RTS.

      PLA
      STA   handler_lo            ; save return address low
      PLA
      STA   handler_hi            ; save return address high

      PLA
      STA   eval_in_body
      PLA
      STA   body_resume_hi
      PLA
      STA   body_resume_lo
      PLA
      STA   body_start_hi
      PLA
      STA   body_start_lo
      PLA
      STA   repeat_count_hi
      PLA
      STA   repeat_count_lo

      LDA   handler_hi
      PHA
      LDA   handler_lo
      PHA
      RTS

; ---------------------------------------------------------------------
; print_byte_hex — print A as 2 hex digits
; ---------------------------------------------------------------------
print_byte_hex:
      PHA
      LSR
      LSR
      LSR
      LSR
      JSR   @nibble
      PLA
      AND   #$0F
      JSR   @nibble
      RTS
@nibble:
      CMP   #$0A
      BCC   @digit
      ADC   #$06               ; carry is set, so adds 7
@digit:
      ADC   #'0'
      STA   VGC_CHAROUT
      RTS

; ---------------------------------------------------------------------
; skip_list_body — advance eval_cur past a [...] block without executing
;   Entry: eval_cur must point at a TOK_LBRACKET token
;   Exit:  eval_cur points at the token after the matching TOK_RBRACKET
;          Carry clear = success, Carry set = missing bracket
;   Clobbers: A, X, Y, ptr_lo/hi
; ---------------------------------------------------------------------
skip_list_body:
      ; Verify current token is TOK_LBRACKET
      LDA   eval_cur_lo
      ORA   eval_cur_hi
      BEQ   @err
      LDA   eval_cur_lo
      STA   ptr_lo
      LDA   eval_cur_hi
      STA   ptr_hi
      LDY   #TOK_TAG
      LDA   (ptr_lo),Y
      CMP   #TOK_LBRACKET
      BNE   @err

      ; Advance past the [
      JSR   eval_advance
      LDX   #1                    ; depth = 1

@scan:
      LDA   eval_cur_lo
      ORA   eval_cur_hi
      BEQ   @err                  ; ran out of tokens
      LDA   eval_cur_lo
      STA   ptr_lo
      LDA   eval_cur_hi
      STA   ptr_hi
      LDY   #TOK_TAG
      LDA   (ptr_lo),Y

      CMP   #TOK_LBRACKET
      BNE   @not_lb
      INX
      BRA   @scan_next
@not_lb:
      CMP   #TOK_RBRACKET
      BNE   @scan_next
      DEX
      BEQ   @found                 ; depth == 0 -> matched
@scan_next:
      PHX
      JSR   eval_advance
      PLX
      BRA   @scan

@found:
      ; eval_cur is at the matching ]. Advance past it.
      JSR   eval_advance
      CLC
      RTS

@err:
      SEC
      RTS

; ---------------------------------------------------------------------
; exec_body_block — execute a [...] body once
;   Entry: eval_cur must point at a TOK_LBRACKET token
;   Exit:  eval_cur points past the matching ], carry clear = ok
;          Carry set = bracket error
;   Saves/restores body_resume, eval_in_body for nesting.
;   Clobbers: A, X, Y, ptr_lo/hi, body_start/resume, eval_in_body
; ---------------------------------------------------------------------
exec_body_block:
      ; Verify current token is [
      LDA   eval_cur_lo
      ORA   eval_cur_hi
      BEQ   @err
      LDA   eval_cur_lo
      STA   ptr_lo
      LDA   eval_cur_hi
      STA   ptr_hi
      LDY   #TOK_TAG
      LDA   (ptr_lo),Y
      CMP   #TOK_LBRACKET
      BNE   @err

      ; Advance past [
      JSR   eval_advance

      ; Save outer state for nesting
      LDA   body_resume_lo
      PHA
      LDA   body_resume_hi
      PHA
      LDA   eval_in_body
      PHA

      ; Save body start
      LDA   eval_cur_lo
      PHA
      LDA   eval_cur_hi
      PHA

      ; Scan forward for matching ]
      LDX   #1
@scan:
      LDA   eval_cur_lo
      ORA   eval_cur_hi
      BEQ   @err_pop5
      LDA   eval_cur_lo
      STA   ptr_lo
      LDA   eval_cur_hi
      STA   ptr_hi
      LDY   #TOK_TAG
      LDA   (ptr_lo),Y
      CMP   #TOK_LBRACKET
      BNE   @not_lb
      INX
      BRA   @scan_next
@not_lb:
      CMP   #TOK_RBRACKET
      BNE   @scan_next
      DEX
      BEQ   @found
@scan_next:
      PHX
      JSR   eval_advance
      PLX
      BRA   @scan

@found:
      ; Advance past ] for resume point
      JSR   eval_advance
      LDA   eval_cur_lo
      STA   body_resume_lo
      LDA   eval_cur_hi
      STA   body_resume_hi

      ; Restore body start
      PLA
      STA   eval_cur_hi
      PLA
      STA   eval_cur_lo

      ; Execute the body
      LDA   #$01
      STA   eval_in_body
      JSR   eval_body

      ; Restore outer state
      PLA
      STA   eval_in_body
      PLA
      STA   body_resume_hi
      PLA
      STA   body_resume_lo

      ; Set eval_cur past the ]
      LDA   body_resume_lo
      STA   eval_cur_lo
      LDA   body_resume_hi
      STA   eval_cur_hi
      CLC
      RTS

@err_pop5:
      PLA
      PLA
      PLA
      PLA
      PLA
@err:
      SEC
      RTS

; ---------------------------------------------------------------------
; do_if — IF condition [body]
;   Arity 0 — handles its own argument parsing
; ---------------------------------------------------------------------
do_if:
      JSR   eval_expr
      BCS   @err

      ; Nonzero integer part = true
      LDA   eval_val_lo
      ORA   eval_val_hi
      BNE   @true

      ; False — skip the body
      JSR   skip_list_body
      BCS   @err_bracket
      JMP   eval_continue

@true:
      JSR   exec_body_block
      BCS   @err_bracket
      JMP   eval_continue

@err_bracket:
      LDX   #<str_if_bracket
      LDY   #>str_if_bracket
      BRA   if_print_err
@err:
      LDX   #<str_if_err
      LDY   #>str_if_err
      ; fall through

if_print_err:
      STX   ptr_lo
      STY   ptr_hi
      LDY   #0
@lp:
      LDA   (ptr_lo),Y
      BEQ   @done
      STA   VGC_CHAROUT
      INY
      BNE   @lp
@done:
      JSR   eval_newline
      JMP   eval_continue

; ---------------------------------------------------------------------
; do_ifelse — IFELSE condition [true-body] [false-body]
;   Arity 0 — handles its own argument parsing
; ---------------------------------------------------------------------
do_ifelse:
      JSR   eval_expr
      BCS   @err

      LDA   eval_val_lo
      ORA   eval_val_hi
      BNE   @is_true

      ; FALSE: skip first body, execute second
      JSR   skip_list_body
      BCS   @err_bracket
      JSR   exec_body_block
      BCS   @err_bracket
      JMP   eval_continue

@is_true:
      ; TRUE: execute first body, skip second
      JSR   exec_body_block
      BCS   @err_bracket
      JSR   skip_list_body
      BCS   @err_bracket
      JMP   eval_continue

@err_bracket:
      LDX   #<str_ifelse_bracket
      LDY   #>str_ifelse_bracket
      BRA   ifelse_print_err
@err:
      LDX   #<str_ifelse_err
      LDY   #>str_ifelse_err
      ; fall through

ifelse_print_err:
      STX   ptr_lo
      STY   ptr_hi
      LDY   #0
@lp:
      LDA   (ptr_lo),Y
      BEQ   @done
      STA   VGC_CHAROUT
      INY
      BNE   @lp
@done:
      JSR   eval_newline
      JMP   eval_continue

; ---------------------------------------------------------------------
; do_stop — STOP: exit current procedure immediately (no return value)
;   Arity 0, no arguments
; ---------------------------------------------------------------------
do_stop:
      LDA   #$01
      STA   proc_stopped
      JMP   eval_continue

; ---------------------------------------------------------------------
; do_output — OUTPUT expr: exit procedure with a return value
;   Arity 0 — handles its own argument evaluation
; ---------------------------------------------------------------------
do_output:
      JSR   eval_expr           ; evaluate the expression
      LDA   #$02
      STA   proc_stopped        ; signal OUTPUT
      ; eval_type/eval_val holds the return value
      JMP   eval_continue

; =====================================================================
; RODATA — builtin table and name strings
; =====================================================================
      .segment "RODATA"

str_make_err:
      .byte "NOT ENOUGH INPUTS TO MAKE", 0

str_repeat_err:
      .byte "NOT ENOUGH INPUTS TO REPEAT", 0

str_repeat_bracket:
      .byte "REPEAT NEEDS [ BODY ]", 0

str_if_err:
      .byte "NOT ENOUGH INPUTS TO IF", 0

str_if_bracket:
      .byte "IF NEEDS [ BODY ]", 0

str_ifelse_err:
      .byte "NOT ENOUGH INPUTS TO IFELSE", 0

str_ifelse_bracket:
      .byte "IFELSE NEEDS [ BODY ]", 0

; Name strings: length-prefixed
str_print_name:
      .byte 5, "PRINT"
str_type_name:
      .byte 4, "TYPE"
str_make_name:
      .byte 4, "MAKE"
str_repeat_name:
      .byte 6, "REPEAT"
str_if_name:
      .byte 2, "IF"
str_ifelse_name:
      .byte 6, "IFELSE"
str_stop_name:
      .byte 4, "STOP"
str_output_name:
      .byte 6, "OUTPUT"

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

      .word str_repeat_name
      .word do_repeat
      .byte 0                    ; arity 0: do_repeat handles its own args

      .word str_ifelse_name
      .word do_ifelse
      .byte 0                    ; arity 0: do_ifelse handles its own args

      .word str_if_name
      .word do_if
      .byte 0                    ; arity 0: do_if handles its own args

      .word str_stop_name
      .word do_stop
      .byte 0                    ; arity 0: no arguments

      .word str_output_name
      .word do_output
      .byte 0                    ; arity 0: do_output handles its own args

      .word str_first_name
      .word do_first
      .byte 0                    ; arity 0: handles its own args

      .word str_butfirst_name
      .word do_butfirst
      .byte 0

      .word str_bf_name
      .word do_butfirst
      .byte 0                    ; BF = alias for BUTFIRST

      .word str_count_name
      .word do_count
      .byte 0

      .word str_emptyp_name
      .word do_emptyp
      .byte 0

      .word str_last_name
      .word do_last
      .byte 0

      .word str_butlast_name
      .word do_butlast
      .byte 0

      .word str_bl_name
      .word do_butlast
      .byte 0                    ; BL = alias for BUTLAST

      .word str_item_name
      .word do_item
      .byte 0

      .word str_memberp_name
      .word do_memberp
      .byte 0

      .word str_show_name
      .word do_show
      .byte 0

      .word str_fput_name
      .word do_fput
      .byte 0

      .word str_lput_name
      .word do_lput
      .byte 0

      .word str_list_name
      .word do_list
      .byte 0

      .word str_sentence_name
      .word do_sentence
      .byte 0

      .word str_se_name
      .word do_sentence
      .byte 0                    ; SE = alias for SENTENCE

      .word str_word_name
      .word do_word
      .byte 0

      .word str_run_name
      .word do_run
      .byte 0

      .word $0000               ; end sentinel

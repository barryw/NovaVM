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
      STX   ptr2_lo             ; ptr2 = handler address (return value)
      ; Read arity from the table (num_tmp = entry base)
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
; do_print / do_type share one body. PRINT appends a newline; TYPE does not.
; The newline flag is carried on the stack across the (balanced) print calls.
do_print:
      LDA   #$FF                 ; PRINT: append trailing newline
      BRA   pt_common
do_type:
      LDA   #$00                 ; TYPE: no trailing newline
pt_common:
      PHA                        ; save newline flag
      LDA   eval_type
      CMP   #VAL_LIST
      BEQ   pt_list
      CMP   #VAL_WORD
      BEQ   pt_word

      ; Print number
      JSR   print_number
      BRA   pt_done

pt_list:
      JSR   print_list
      BRA   pt_done

pt_word:
      ; Print quoted word — eval_val_lo/hi points at length+chars
      LDA   eval_val_lo
      STA   ptr_lo
      LDA   eval_val_hi
      STA   ptr_hi
      LDY   #0
      LDA   (ptr_lo),Y         ; length
      TAX
      INY
pt_wch:
      LDA   (ptr_lo),Y
      STA   VGC_CHAROUT
      INY
      DEX
      BNE   pt_wch

pt_done:
      PLA                        ; restore newline flag
      BEQ   pt_no_nl
      JSR   eval_newline
pt_no_nl:
      JMP   eval_continue

; ---------------------------------------------------------------------
; do_type — TYPE <expr>: like PRINT but no newline
; ---------------------------------------------------------------------
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
      LDX   #<str_make_name
      LDY   #>str_make_name
      JMP   err_nei

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
repcount:         .res 1       ; 1-based REPCOUNT within REPEAT loop
for_cur_lo:       .res 1       ; FOR loop current value low
for_cur_hi:       .res 1       ; FOR loop current value high
for_end_lo:       .res 1       ; FOR loop end value low
for_end_hi:       .res 1       ; FOR loop end value high
cond_start_lo:    .res 1       ; WHILE/UNTIL condition start low
cond_start_hi:    .res 1       ; WHILE/UNTIL condition start high

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
      LDA   repcount
      PHA
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

      ; --- Initialize repcount and execute the body repeat_count times ---
      LDA   #1
      STA   repcount
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

      ; Increment repcount (capped at 255)
      LDA   repcount
      CMP   #$FF
      BCS   :+
      INC   repcount
:
      ; Decrement count (16-bit)
      LDA   repeat_count_lo
      BNE   @dec_lo
      DEC   repeat_count_hi
@dec_lo:
      DEC   repeat_count_lo
      BRA   @loop

@loop_done:
      ; Resume past THIS REPEAT's ] BEFORE restoring outer state — restore pops
      ; body_resume back to the enclosing REPEAT's value, so reading it after the
      ; restore would resume at the wrong token (trailing commands never run).
      LDA   body_resume_lo
      STA   eval_cur_lo
      LDA   body_resume_hi
      STA   eval_cur_hi
      ; --- Restore outer state ---
      JSR   repeat_restore_state
      JMP   eval_continue

@err_bracket_pop:
      ; Restore stack before printing error
      JSR   repeat_restore_state
      ; Fall through to error message
      JSR   print_inl
      .byte "REPEAT NEEDS [ BODY ]", 0
      JSR   eval_newline
      JMP   eval_continue

@err_args:
      LDX   #<str_repeat_name
      LDY   #>str_repeat_name
      JMP   err_nei

; ---------------------------------------------------------------------
; repeat_restore_state — pop 7 saved bytes from stack to ZP
;   Restores eval_in_body, body_resume, body_start, repeat_count
;   Must be called via JSR (uses return address on stack correctly)
; ---------------------------------------------------------------------
repeat_restore_state:
      ; Pull our return address, pop 8 saved bytes, push RA, RTS.
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
      PLA
      STA   repcount

      LDA   handler_hi
      PHA
      LDA   handler_lo
      PHA
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
      JMP   list_print_err
@err:
      LDX   #<str_if_name
      LDY   #>str_if_name
      JMP   err_nei

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
      JMP   list_print_err
@err:
      LDX   #<str_ifelse_name
      LDY   #>str_ifelse_name
      JMP   err_nei

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

; ---------------------------------------------------------------------
; do_catch — CATCH "tag [body]: execute body, catching errors/throws
;   Arity 0 — handles its own argument parsing
;   1. Evaluate tag argument (quoted word)
;   2. Save current catch frame (for nesting) on the 6502 stack
;   3. Set up new catch frame with tag and stack pointer
;   4. Execute the body
;   5. On normal completion: restore previous catch frame, continue
;   On THROW match: try_throw_error or do_throw restores SP from
;   catch frame, which unwinds back to step 5's stack context.
; ---------------------------------------------------------------------
do_catch:
      ; Evaluate tag argument (e.g., "ERROR)
      JSR   eval_expr
      BCC   @tag_ok
      JMP   @err_tag
@tag_ok:

      ; Must be a word
      LDA   eval_type
      CMP   #VAL_WORD
      BEQ   @type_ok
      JMP   @err_tag
@type_ok:

      ; Save tag pointer and eval_in_body temporarily (3 bytes)
      LDA   eval_val_lo
      PHA
      LDA   eval_val_hi
      PHA
      LDA   eval_in_body
      PHA

      ; Push previous catch frame onto the 6502 stack (6 bytes)
      LDA   catch_active
      PHA
      LDA   catch_tag_lo
      PHA
      LDA   catch_tag_hi
      PHA
      LDA   catch_sp
      PHA
      LDA   catch_resume_lo
      PHA
      LDA   catch_resume_hi
      PHA

      ; Set up the tag pointer from saved values
      ; Stack (top to bottom): prev_frame(6), eval_in_body, eval_val_hi, eval_val_lo
      ; $0101+X = SP+1 = top. eval_in_body at SP+7, eval_val_hi at SP+8, eval_val_lo at SP+9
      TSX
      LDA   $0108,X               ; eval_val_hi = tag ptr high
      STA   catch_tag_hi
      LDA   $0109,X               ; eval_val_lo = tag ptr low
      STA   catch_tag_lo

      ; Save stack pointer NOW — this is the unwind target.
      ; When a THROW matches, TXS restores to this SP, and the
      ; 6 PLA instructions below will pop the previous frame.
      TSX
      STX   catch_sp

      ; Activate the catch
      LDA   #$01
      STA   catch_active

      ; We need to find the resume point PAST the body's ].
      ; Scan ahead to find it, then come back and execute the body.
      ; Save body start location first.
      LDA   eval_cur_lo
      PHA                         ; save body-start lo
      LDA   eval_cur_hi
      PHA                         ; save body-start hi

      ; Scan past the body [...] to find resume point
      JSR   skip_list_body
      BCS   @err_bracket_pop

      ; eval_cur is now past the ]. Save as resume point.
      LDA   eval_cur_lo
      STA   catch_resume_lo
      LDA   eval_cur_hi
      STA   catch_resume_hi

      ; Restore body start
      PLA
      STA   eval_cur_hi
      PLA
      STA   eval_cur_lo

      ; Execute the body
      JSR   exec_body_block

      ; Body completed normally — restore previous catch frame
      ; (Stack still has: prev_frame(6), tag_hi, tag_lo at the bottom)
      PLA
      STA   catch_resume_hi
      PLA
      STA   catch_resume_lo
      PLA
      STA   catch_sp
      PLA
      STA   catch_tag_hi
      PLA
      STA   catch_tag_lo
      PLA
      STA   catch_active

      ; Discard saved eval_in_body + tag ptr (3 bytes)
      PLA                         ; eval_in_body
      PLA                         ; tag_hi
      PLA                         ; tag_lo

      ; Set eval_cur past the body
      LDA   catch_resume_lo
      STA   eval_cur_lo
      LDA   catch_resume_hi
      STA   eval_cur_hi
      JMP   eval_continue

@err_bracket_pop:
      ; Clean up: body-start was partially popped by skip_list_body error
      PLA                         ; body_start_hi
      PLA                         ; body_start_lo
      ; Restore previous catch frame
      PLA
      STA   catch_resume_hi
      PLA
      STA   catch_resume_lo
      PLA
      STA   catch_sp
      PLA
      STA   catch_tag_hi
      PLA
      STA   catch_tag_lo
      PLA
      STA   catch_active
      ; Discard saved eval_in_body + tag ptr (3 bytes)
      PLA
      PLA
      PLA
      ; Print bracket error
      LDX   #<str_catch_bracket
      LDY   #>str_catch_bracket
      JMP   list_print_err

@err_tag:
      LDX   #<str_catch_name
      LDY   #>str_catch_name
      JMP   err_nei

; ---------------------------------------------------------------------
; do_throw — THROW "tag: unwind to matching CATCH
;   Arity 0 — handles its own argument parsing
;   1. Evaluate tag argument
;   2. If catch is active and tags match: unwind via TXS
;   3. If no match: print error
; ---------------------------------------------------------------------
do_throw:
      ; Evaluate tag argument
      JSR   eval_expr
      BCS   @err_tag

      ; Must be a word
      LDA   eval_type
      CMP   #VAL_WORD
      BNE   @err_tag

      ; Is a catch frame active?
      LDA   catch_active
      BEQ   @no_catch

      ; Compare tags: eval_val_lo/hi vs catch_tag_lo/hi
      LDA   eval_val_lo
      STA   ptr_lo
      LDA   eval_val_hi
      STA   ptr_hi
      LDA   catch_tag_lo
      STA   ptr2_lo
      LDA   catch_tag_hi
      STA   ptr2_hi
      JSR   catch_tags_equal
      BCC   @no_catch

      ; Match! Unwind stack to catch point.
      LDX   catch_sp
      TXS

      ; Restore previous catch frame from the restored stack
      PLA
      STA   catch_resume_hi
      PLA
      STA   catch_resume_lo
      PLA
      STA   catch_sp
      PLA
      STA   catch_tag_hi
      PLA
      STA   catch_tag_lo
      PLA
      STA   catch_active

      ; Restore eval_in_body and discard saved tag pointer (3 bytes)
      PLA
      STA   eval_in_body
      PLA                         ; tag_hi
      PLA                         ; tag_lo

      ; Set eval_cur to resume point (past the caught body)
      LDA   catch_resume_lo
      STA   eval_cur_lo
      LDA   catch_resume_hi
      STA   eval_cur_hi

      JMP   eval_continue

@no_catch:
      ; Print "CAN'T FIND CATCH TAG " + the tag
      JSR   print_inl
      .byte "CAN'T FIND CATCH TAG ", 0
@nc_word:
      ; Print the tag word
      LDA   eval_val_lo
      STA   ptr_lo
      LDA   eval_val_hi
      STA   ptr_hi
      LDY   #0
      LDA   (ptr_lo),Y           ; length
      TAX
      BEQ   @nc_done
      INY
@nc_ch:
      LDA   (ptr_lo),Y
      STA   VGC_CHAROUT
      INY
      DEX
      BNE   @nc_ch
@nc_done:
      JSR   eval_newline
      JMP   eval_continue

@err_tag:
      LDX   #<str_throw_name
      LDY   #>str_throw_name
      JMP   err_nei

; ---------------------------------------------------------------------
; do_repcount — REPCOUNT: reporter returning current 1-based repeat index
;   Arity 0, no arguments. Returns repcount as a number.
; ---------------------------------------------------------------------
do_repcount:
      STZ   eval_val_hi
      LDA   repcount
      STA   eval_val_lo
      STZ   eval_val_frac
      STZ   eval_type              ; VAL_NUMBER
      JMP   eval_continue

; ---------------------------------------------------------------------
; do_for — FOR [var start end] [body]
;   Arity 0 — handles its own argument parsing
;   1. Parse control list [varname start end]
;   2. Parse body [...]
;   3. Loop: set var=current, execute body, increment, check end
; ---------------------------------------------------------------------
do_for:
      ; Current token must be [ for the control list
      LDA   eval_cur_lo
      ORA   eval_cur_hi
      BNE   :+
      JMP   for_err_ctrl
:     LDA   eval_cur_lo
      STA   ptr_lo
      LDA   eval_cur_hi
      STA   ptr_hi
      LDY   #TOK_TAG
      LDA   (ptr_lo),Y
      CMP   #TOK_LBRACKET
      BEQ   :+
      JMP   for_err_ctrl
:
      ; Advance past [
      JSR   eval_advance

      ; Read variable name token (must be TOK_WORD)
      LDA   eval_cur_lo
      ORA   eval_cur_hi
      BNE   :+
      JMP   for_err_ctrl
:     LDA   eval_cur_lo
      STA   ptr_lo
      LDA   eval_cur_hi
      STA   ptr_hi
      LDY   #TOK_TAG
      LDA   (ptr_lo),Y
      CMP   #TOK_WORD
      BEQ   :+
      JMP   for_err_ctrl
:
      ; Save variable name pointer (points at TOK_PAYLOAD = len+chars)
      CLC
      LDA   eval_cur_lo
      ADC   #TOK_PAYLOAD
      PHA                           ; name ptr lo
      LDA   eval_cur_hi
      ADC   #0
      PHA                           ; name ptr hi

      ; Advance past variable name
      JSR   eval_advance

      ; Evaluate start expression
      JSR   eval_expr
      BCC   :+
      JMP   for_err_ctrl_pop2
:     LDA   eval_type
      BEQ   :+
      JMP   for_err_ctrl_pop2
:
      ; Save start value
      LDA   eval_val_lo
      STA   for_cur_lo
      LDA   eval_val_hi
      STA   for_cur_hi

      ; Evaluate end expression
      JSR   eval_expr
      BCC   :+
      JMP   for_err_ctrl_pop2
:     LDA   eval_type
      BEQ   :+
      JMP   for_err_ctrl_pop2
:
      ; Save end value
      LDA   eval_val_lo
      STA   for_end_lo
      LDA   eval_val_hi
      STA   for_end_hi

      ; Current token should be ] closing the control list
      LDA   eval_cur_lo
      ORA   eval_cur_hi
      BNE   :+
      JMP   for_err_ctrl_pop2
:     LDA   eval_cur_lo
      STA   ptr_lo
      LDA   eval_cur_hi
      STA   ptr_hi
      LDY   #TOK_TAG
      LDA   (ptr_lo),Y
      CMP   #TOK_RBRACKET
      BEQ   :+
      JMP   for_err_ctrl_pop2
:
      ; Advance past ]
      JSR   eval_advance

      ; --- Save outer loop state for nesting ---
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

      ; --- Locate the body [...] ---
      LDA   eval_cur_lo
      ORA   eval_cur_hi
      BNE   :+
      JMP   for_err_bracket_pop7
:     LDA   eval_cur_lo
      STA   ptr_lo
      LDA   eval_cur_hi
      STA   ptr_hi
      LDY   #TOK_TAG
      LDA   (ptr_lo),Y
      CMP   #TOK_LBRACKET
      BEQ   :+
      JMP   for_err_bracket_pop7
:
      ; Advance past [
      JSR   eval_advance
      LDA   eval_cur_lo
      STA   body_start_lo
      LDA   eval_cur_hi
      STA   body_start_hi

      ; Scan for matching ]
      LDX   #1
@for_scan:
      LDA   eval_cur_lo
      ORA   eval_cur_hi
      BNE   :+
      JMP   for_err_bracket_pop7
:     LDA   eval_cur_lo
      STA   ptr_lo
      LDA   eval_cur_hi
      STA   ptr_hi
      LDY   #TOK_TAG
      LDA   (ptr_lo),Y
      CMP   #TOK_LBRACKET
      BNE   @for_not_lb
      INX
      BRA   @for_scan_next
@for_not_lb:
      CMP   #TOK_RBRACKET
      BNE   @for_scan_next
      DEX
      BEQ   @for_scan_found
@for_scan_next:
      PHX
      JSR   eval_advance
      PLX
      BRA   @for_scan

@for_scan_found:
      JSR   eval_advance
      LDA   eval_cur_lo
      STA   body_resume_lo
      LDA   eval_cur_hi
      STA   body_resume_hi

      ; --- Loop: for_cur from start to end ---
@for_loop:
      LDA   proc_stopped
      BNE   @for_done

      ; Check if for_cur > for_end (signed 16-bit)
      LDA   for_cur_hi
      SEC
      SBC   for_end_hi
      BVC   @for_sign_ok
      EOR   #$80
@for_sign_ok:
      BMI   @for_in_range            ; for_cur < for_end
      LDA   for_cur_hi
      CMP   for_end_hi
      BNE   @for_done                ; hi differs and cur > end
      LDA   for_cur_lo
      CMP   for_end_lo
      BEQ   @for_in_range            ; equal = last iteration
      BCS   @for_done                ; cur_lo > end_lo = done

@for_in_range:
      ; Set the variable to for_cur value
      ; Name pointer on stack under 5 saved state bytes + 2 name ptr bytes
      TSX
      LDA   $0106,X                 ; name ptr hi
      STA   ptr_hi
      LDA   $0107,X                 ; name ptr lo
      STA   ptr_lo

      LDA   for_cur_lo
      STA   eval_val_lo
      LDA   for_cur_hi
      STA   eval_val_hi
      STZ   eval_val_frac
      STZ   eval_type                ; VAL_NUMBER
      JSR   var_set

      ; Execute body
      LDA   body_start_lo
      STA   eval_cur_lo
      LDA   body_start_hi
      STA   eval_cur_hi
      LDA   #$01
      STA   eval_in_body
      JSR   eval_body

      ; Increment for_cur (16-bit)
      INC   for_cur_lo
      BNE   @for_loop
      INC   for_cur_hi
      BRA   @for_loop

@for_done:
      ; Restore outer state
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
      PLA                           ; name_hi
      PLA                           ; name_lo

      LDA   body_resume_lo
      STA   eval_cur_lo
      LDA   body_resume_hi
      STA   eval_cur_hi
      JMP   eval_continue

for_err_ctrl_pop2:
      PLA
      PLA
for_err_ctrl:
      JSR   print_inl
      .byte "FOR NEEDS [VAR START END]", 0
      JSR   eval_newline
      JMP   eval_continue

for_err_bracket_pop7:
      PLA
      STA   eval_in_body
      PLA
      PLA
      PLA
      PLA
      PLA
      PLA
      JSR   print_inl
      .byte "FOR NEEDS [BODY]", 0
      JSR   eval_newline
      JMP   eval_continue

; ---------------------------------------------------------------------
; do_while — WHILE [condition] [body]
;   Arity 0 — loop while condition is nonzero
; ---------------------------------------------------------------------
do_while:
      LDA   #0                      ; 0 = WHILE (loop while true)
      BRA   while_until_common

; ---------------------------------------------------------------------
; do_until — UNTIL [condition] [body]
;   Arity 0 — loop until condition becomes nonzero
; ---------------------------------------------------------------------
do_until:
      LDA   #1                      ; 1 = UNTIL (loop until true)
      ; fall through

; ---------------------------------------------------------------------
; while_until_common — shared logic for WHILE/UNTIL
;   Entry: A = 0 for WHILE, 1 for UNTIL
; ---------------------------------------------------------------------
while_until_common:
      PHA                           ; save mode (0=WHILE, 1=UNTIL)

      ; --- Save outer loop state ---
      LDA   body_start_lo
      PHA
      LDA   body_start_hi
      PHA
      LDA   body_resume_lo
      PHA
      LDA   body_resume_hi
      PHA
      LDA   cond_start_lo
      PHA
      LDA   cond_start_hi
      PHA
      LDA   eval_in_body
      PHA

      ; --- Locate condition [...] ---
      LDA   eval_cur_lo
      ORA   eval_cur_hi
      BNE   :+
      JMP   wu_err_bracket
:     LDA   eval_cur_lo
      STA   ptr_lo
      LDA   eval_cur_hi
      STA   ptr_hi
      LDY   #TOK_TAG
      LDA   (ptr_lo),Y
      CMP   #TOK_LBRACKET
      BEQ   :+
      JMP   wu_err_bracket
:
      ; Advance past [
      JSR   eval_advance
      LDA   eval_cur_lo
      STA   cond_start_lo
      LDA   eval_cur_hi
      STA   cond_start_hi

      ; Scan forward past condition [...] to find body
      LDX   #1
@wu_cscan:
      LDA   eval_cur_lo
      ORA   eval_cur_hi
      BNE   :+
      JMP   wu_err_bracket
:     LDA   eval_cur_lo
      STA   ptr_lo
      LDA   eval_cur_hi
      STA   ptr_hi
      LDY   #TOK_TAG
      LDA   (ptr_lo),Y
      CMP   #TOK_LBRACKET
      BNE   @wu_cscan_nlb
      INX
      BRA   @wu_cscan_next
@wu_cscan_nlb:
      CMP   #TOK_RBRACKET
      BNE   @wu_cscan_next
      DEX
      BEQ   @wu_cscan_found
@wu_cscan_next:
      PHX
      JSR   eval_advance
      PLX
      BRA   @wu_cscan

@wu_cscan_found:
      ; Advance past ] of condition
      JSR   eval_advance

      ; --- Locate body [...] ---
      LDA   eval_cur_lo
      ORA   eval_cur_hi
      BNE   :+
      JMP   wu_err_bracket
:     LDA   eval_cur_lo
      STA   ptr_lo
      LDA   eval_cur_hi
      STA   ptr_hi
      LDY   #TOK_TAG
      LDA   (ptr_lo),Y
      CMP   #TOK_LBRACKET
      BEQ   :+
      JMP   wu_err_bracket
:
      ; Advance past [
      JSR   eval_advance
      LDA   eval_cur_lo
      STA   body_start_lo
      LDA   eval_cur_hi
      STA   body_start_hi

      ; Scan for matching ] of body
      LDX   #1
@wu_bscan:
      LDA   eval_cur_lo
      ORA   eval_cur_hi
      BNE   :+
      JMP   wu_err_bracket
:     LDA   eval_cur_lo
      STA   ptr_lo
      LDA   eval_cur_hi
      STA   ptr_hi
      LDY   #TOK_TAG
      LDA   (ptr_lo),Y
      CMP   #TOK_LBRACKET
      BNE   @wu_bscan_nlb
      INX
      BRA   @wu_bscan_next
@wu_bscan_nlb:
      CMP   #TOK_RBRACKET
      BNE   @wu_bscan_next
      DEX
      BEQ   @wu_bscan_found
@wu_bscan_next:
      PHX
      JSR   eval_advance
      PLX
      BRA   @wu_bscan

@wu_bscan_found:
      JSR   eval_advance
      LDA   eval_cur_lo
      STA   body_resume_lo
      LDA   eval_cur_hi
      STA   body_resume_hi

      ; --- Main loop ---
@wu_loop:
      LDA   proc_stopped
      BNE   @wu_done

      ; Evaluate condition: set eval_cur to cond_start, run body
      LDA   cond_start_lo
      STA   eval_cur_lo
      LDA   cond_start_hi
      STA   eval_cur_hi
      LDA   #$01
      STA   eval_in_body

      ; We need to evaluate an expression from the condition body.
      ; The condition block contains an expression, so use eval_expr.
      JSR   eval_expr
      BCS   @wu_done                ; eval error = stop

      ; Check result: nonzero integer = true
      LDA   eval_val_lo
      ORA   eval_val_hi
      ; Result in A: nonzero = true condition

      ; Read mode from stack to decide behavior
      ; Mode is at the bottom of our saved state block.
      ; Stack layout (top-down): eval_in_body, cond_start_hi/lo,
      ;   body_resume_hi/lo, body_start_hi/lo, mode
      ; That's 8 bytes deep from current SP.
      TSX
      LDY   $0108,X                 ; mode: 0=WHILE, 1=UNTIL

      CPY   #0
      BNE   @wu_until_check

      ; WHILE: if condition is false (zero), stop
      CMP   #0                      ; A still has lo|hi result
      BEQ   @wu_done
      BRA   @wu_exec_body

@wu_until_check:
      ; UNTIL: if condition is true (nonzero), stop
      CMP   #0
      BNE   @wu_done

@wu_exec_body:
      ; Execute body
      LDA   body_start_lo
      STA   eval_cur_lo
      LDA   body_start_hi
      STA   eval_cur_hi
      LDA   #$01
      STA   eval_in_body
      JSR   eval_body

      BRA   @wu_loop

@wu_done:
      ; Restore outer state
      PLA
      STA   eval_in_body
      PLA
      STA   cond_start_hi
      PLA
      STA   cond_start_lo
      PLA
      STA   body_resume_hi
      PLA
      STA   body_resume_lo
      PLA
      STA   body_start_hi
      PLA
      STA   body_start_lo
      PLA                           ; discard mode byte

      ; Resume past the body
      LDA   body_resume_lo
      STA   eval_cur_lo
      LDA   body_resume_hi
      STA   eval_cur_hi
      JMP   eval_continue

wu_err_bracket:
      ; Restore whatever state we pushed + discard mode
      PLA
      STA   eval_in_body
      PLA
      STA   cond_start_hi
      PLA
      STA   cond_start_lo
      PLA
      STA   body_resume_hi
      PLA
      STA   body_resume_lo
      PLA
      STA   body_start_hi
      PLA
      STA   body_start_lo
      PLA                           ; discard mode

      JSR   print_inl
      .byte "WHILE/UNTIL NEEDS [COND] [BODY]", 0
      JSR   eval_newline
      JMP   eval_continue

; ---------------------------------------------------------------------
; do_readchar — READCHAR: wait for keypress, return as 1-char word
;   Arity 0, no arguments. Reporter.
; ---------------------------------------------------------------------
do_readchar:
      ; Poll until a key is pressed
@wait:
      LDA   VGC_CHARIN
      BEQ   @wait
      ; A = character — fall through to alloc_one_char_word

; ---------------------------------------------------------------------
; alloc_one_char_word — allocate a 1-char word on heap from A
;   Input: A = character byte
;   Output: eval_type=VAL_WORD, eval_val=pointer to len+char
;   Jumps to eval_continue
; ---------------------------------------------------------------------
alloc_one_char_word:
      PHA                       ; save character
      LDA   #2                  ; 1 byte length + 1 byte char
      LDX   #ATYPE_STRING
      JSR   heap_alloc
      PLA
      BCS   @oom
      LDY   #1
      STA   (ptr_lo),Y          ; store char
      LDA   #1
      LDY   #0
      STA   (ptr_lo),Y          ; store length = 1
      LDA   ptr_lo
      STA   eval_val_lo
      LDA   ptr_hi
      STA   eval_val_hi
      LDA   #VAL_WORD
      STA   eval_type
      STZ   eval_val_frac
      JMP   eval_continue
@oom:
      LDX   #<str_oom
      LDY   #>str_oom
      JMP   list_print_err

; ---------------------------------------------------------------------
; do_readword — READWORD: read chars until space/CR, return as word
;   Arity 0, no arguments. Reporter.
;   Uses input_buf as scratch (safe — main input already consumed).
; ---------------------------------------------------------------------
do_readword:
      STZ   z:buf_idx
@rw_poll:
      LDA   VGC_CHARIN
      BEQ   @rw_poll
      ; Check for CR
      CMP   #$0D
      BEQ   @rw_done
      ; Check for space
      CMP   #' '
      BEQ   @rw_done
      ; Check for backspace
      CMP   #$08
      BEQ   @rw_bs
      CMP   #$14
      BEQ   @rw_bs
      CMP   #$7F
      BEQ   @rw_bs
      ; Store and echo
      LDX   z:buf_idx
      CPX   #126
      BCS   @rw_poll             ; buffer full
      STA   input_buf,X
      STA   VGC_CHAROUT
      INC   z:buf_idx
      BRA   @rw_poll

@rw_bs:
      LDX   z:buf_idx
      BEQ   @rw_poll
      DEX
      STX   z:buf_idx
      LDA   #$08
      STA   VGC_CHAROUT
      BRA   @rw_poll

@rw_done:
      ; Echo CR/LF
      PHA
      LDA   #$0D
      STA   VGC_CHAROUT
      LDA   #$0A
      STA   VGC_CHAROUT
      PLA
      ; Allocate word: 1 (len) + buf_idx (chars)
      LDA   z:buf_idx
      INC                        ; +1 for length byte
      LDX   #ATYPE_STRING
      JSR   heap_alloc
      BCS   @rw_oom
      ; Write length
      LDA   z:buf_idx
      LDY   #0
      STA   (ptr_lo),Y
      ; Copy chars
      TAX
      BEQ   @rw_set
      LDY   #1
      STZ   num_tmp_lo
@rw_copy:
      PHY
      LDY   num_tmp_lo
      LDA   input_buf,Y
      PLY
      STA   (ptr_lo),Y
      INY
      INC   num_tmp_lo
      DEX
      BNE   @rw_copy
@rw_set:
      LDA   ptr_lo
      STA   eval_val_lo
      LDA   ptr_hi
      STA   eval_val_hi
      LDA   #VAL_WORD
      STA   eval_type
      STZ   eval_val_frac
      JMP   eval_continue
@rw_oom:
      LDX   #<str_oom
      LDY   #>str_oom
      JMP   list_print_err

; ---------------------------------------------------------------------
; do_readlist — READLIST: read a line, return as a list
;   Arity 0, no arguments. Reporter.
;   Calls repl_read_line, tokenize_line, then walks tokens building a list.
; ---------------------------------------------------------------------
do_readlist:
      ; Save tokenizer state (we'll overwrite tok_head/tok_tail)
      LDA   tok_head_lo
      PHA
      LDA   tok_head_hi
      PHA
      LDA   tok_tail_lo
      PHA
      LDA   tok_tail_hi
      PHA
      LDA   eval_cur_lo
      PHA
      LDA   eval_cur_hi
      PHA

      ; Read a line from keyboard into input_buf
      JSR   repl_read_line
      ; Tokenize it
      JSR   tokenize_line

      ; Walk tokens and build a list
      STZ   z:list_head_lo
      STZ   z:list_head_hi
      STZ   z:list_tail_lo
      STZ   z:list_tail_hi

      LDA   tok_head_lo
      STA   eval_cur_lo
      LDA   tok_head_hi
      STA   eval_cur_hi

@rl_next:
      LDA   eval_cur_lo
      ORA   eval_cur_hi
      BNE   @rl_have_tok
      JMP   @rl_build_done
@rl_have_tok:

      LDA   eval_cur_lo
      STA   ptr_lo
      LDA   eval_cur_hi
      STA   ptr_hi
      LDY   #TOK_TAG
      LDA   (ptr_lo),Y

      CMP   #TOK_NUMBER
      BNE   :+
      JMP   @rl_number
:     CMP   #TOK_WORD
      BEQ   @rl_word_jmp
      CMP   #TOK_QUOTE
      BEQ   @rl_word_jmp
      CMP   #TOK_LBRACKET
      BNE   :+
      JMP   @rl_sublist
:     ; Skip other tokens (brackets, infix, etc.)
      JSR   eval_advance
      BRA   @rl_next

@rl_word_jmp:
      JMP   @rl_word

@rl_number:
      LDY   #TOK_PAYLOAD
      LDA   (ptr_lo),Y
      STA   eval_val_hi
      INY
      LDA   (ptr_lo),Y
      STA   eval_val_lo
      INY
      LDA   (ptr_lo),Y
      STA   eval_val_frac
      STZ   eval_type
      JSR   eval_advance
      JMP   @rl_alloc_cell

@rl_word:
      ; Allocate string on heap and copy word from token payload
      LDY   #TOK_PAYLOAD
      LDA   (ptr_lo),Y           ; length
      PHA
      INC                        ; +1 for length byte
      LDX   #ATYPE_STRING
      JSR   heap_alloc
      BCC   :+
      PLA
      JMP   @rl_oom
:     ; Re-read source token (ptr_lo clobbered by heap_alloc)
      LDA   eval_cur_lo
      STA   ptr2_lo
      LDA   eval_cur_hi
      STA   ptr2_hi
      PLA
      PHA
      LDY   #0
      STA   (ptr_lo),Y           ; store length
      PLA
      TAX
      BEQ   @rl_word_done
      LDY   #1
      STZ   num_tmp_lo
@rl_wch:
      PHX
      PHY
      LDA   num_tmp_lo
      CLC
      ADC   #TOK_PAYLOAD+1
      TAY
      LDA   (ptr2_lo),Y
      PLY
      STA   (ptr_lo),Y
      INY
      INC   num_tmp_lo
      PLX
      DEX
      BNE   @rl_wch
@rl_word_done:
      LDA   ptr_lo
      STA   eval_val_lo
      LDA   ptr_hi
      STA   eval_val_hi
      STZ   eval_val_frac
      LDA   #VAL_WORD
      STA   eval_type
      JSR   eval_advance
      JMP   @rl_alloc_cell

@rl_sublist:
      ; Use eval_list to build the sublist (advances past matching ])
      JSR   eval_advance
      LDA   z:list_head_lo
      PHA
      LDA   z:list_head_hi
      PHA
      LDA   z:list_tail_lo
      PHA
      LDA   z:list_tail_hi
      PHA
      JSR   eval_list
      PLA
      STA   z:list_tail_hi
      PLA
      STA   z:list_tail_lo
      PLA
      STA   z:list_head_hi
      PLA
      STA   z:list_head_lo
      ; eval_type = VAL_LIST, eval_val = sublist head
      JMP   @rl_alloc_cell

@rl_alloc_cell:
      LDA   #CONS_SIZE
      LDX   #ATYPE_CONS
      JSR   heap_alloc
      BCC   @rl_alloc_ok
      JMP   @rl_oom
@rl_alloc_ok:
      JSR   fill_cons_from_eval

      ; Link into list
      LDA   z:list_head_lo
      ORA   z:list_head_hi
      BNE   @rl_link_tail
      LDA   ptr_lo
      STA   z:list_head_lo
      STA   z:list_tail_lo
      LDA   ptr_hi
      STA   z:list_head_hi
      STA   z:list_tail_hi
      JMP   @rl_next

@rl_link_tail:
      LDA   z:list_tail_lo
      STA   ptr2_lo
      LDA   z:list_tail_hi
      STA   ptr2_hi
      LDY   #CONS_CDR_LO
      LDA   ptr_lo
      STA   (ptr2_lo),Y
      LDY   #CONS_CDR_HI
      LDA   ptr_hi
      STA   (ptr2_lo),Y
      LDA   ptr_lo
      STA   z:list_tail_lo
      LDA   ptr_hi
      STA   z:list_tail_hi
      JMP   @rl_next

@rl_build_done:
      ; Restore tokenizer state
      PLA
      STA   eval_cur_hi
      PLA
      STA   eval_cur_lo
      PLA
      STA   tok_tail_hi
      PLA
      STA   tok_tail_lo
      PLA
      STA   tok_head_hi
      PLA
      STA   tok_head_lo

      ; Return the list
      LDA   #VAL_LIST
      STA   eval_type
      LDA   z:list_head_lo
      STA   eval_val_lo
      LDA   z:list_head_hi
      STA   eval_val_hi
      STZ   eval_val_frac
      JMP   eval_continue

@rl_oom:
      ; Restore tokenizer state before error
      PLA
      STA   eval_cur_hi
      PLA
      STA   eval_cur_lo
      PLA
      STA   tok_tail_hi
      PLA
      STA   tok_tail_lo
      PLA
      STA   tok_head_hi
      PLA
      STA   tok_head_lo
      LDX   #<str_oom
      LDY   #>str_oom
      JMP   list_print_err

; ---------------------------------------------------------------------
; do_char — CHAR n: convert ASCII code to 1-char word
;   Arity 0: evaluates its own argument
; ---------------------------------------------------------------------
do_char:
      JSR   eval_expr
      BCS   @err
      ; Must be a number
      LDA   eval_type
      BNE   @err
      ; Low byte of number is the ASCII code
      LDA   eval_val_lo
      JMP   alloc_one_char_word
@err:
      LDX   #<str_char_name
      LDY   #>str_char_name
      JMP   err_nei

; ---------------------------------------------------------------------
; do_ascii — ASCII word: return ASCII code of first character
;   Arity 0: evaluates its own argument
; ---------------------------------------------------------------------
do_ascii:
      JSR   eval_expr
      BCS   @err
      ; Must be a word
      LDA   eval_type
      CMP   #VAL_WORD
      BNE   @err_type
      ; eval_val_lo/hi points at length-prefixed string
      LDA   eval_val_lo
      STA   ptr_lo
      LDA   eval_val_hi
      STA   ptr_hi
      LDY   #0
      LDA   (ptr_lo),Y           ; length
      BEQ   @err_empty            ; empty word
      LDY   #1
      LDA   (ptr_lo),Y           ; first character
      STA   eval_val_lo
      STZ   eval_val_hi
      STZ   eval_val_frac
      STZ   eval_type             ; VAL_NUMBER
      JMP   eval_continue
@err_type:
@err_empty:
@err:
      LDX   #<str_ascii_name
      LDY   #>str_ascii_name
      JMP   err_nei

; =====================================================================
; Math functions and type predicates
; =====================================================================

; ---------------------------------------------------------------------
; return_number — set eval_val from eval_val_lo/hi, clear frac+type
;   Entry: eval_val_lo/hi already set
;   Jumps to eval_continue
; ---------------------------------------------------------------------
return_number:
      STZ   eval_val_frac
      STZ   eval_type
      JMP   eval_continue

; ---------------------------------------------------------------------
; return_signed_byte — sign-extend A into eval_val as 16.8 number
;   Entry: A = signed byte (-128..+127)
;   Jumps to eval_continue
; ---------------------------------------------------------------------
return_signed_byte:
      STA   eval_val_lo
      BPL   @pos
      LDA   #$FF
      BRA   @set
@pos: LDA   #$00
@set: STA   eval_val_hi
      BRA   return_number

; ---------------------------------------------------------------------
; predicate_result — set eval_val to 1 (Z set) or 0 (Z clear)
;   Entry: Z flag from CMP
;   Jumps to eval_continue
; ---------------------------------------------------------------------
predicate_result:
      BEQ   @true
      STZ   eval_val_lo
      BRA   @done
@true:
      LDA   #1
      STA   eval_val_lo
@done:
      STZ   eval_val_hi
      BRA   return_number

; ---------------------------------------------------------------------
; degrees_to_u8 — convert eval_val (degrees 0-360) to u8 angle (0-255)
;   Uses MUL16 coprocessor: angle = degrees * 91 / 128
;   Actually: write degrees to MUL16 A, 91 to B, read RES1 (high byte)
;   Then shift right once (divide by 2) since 91/256=0.355, we want 91/128
;   Wait, let me recalculate: degrees * 91 >> 7
;   MUL16 gives degrees*91 in RES1:RES0. We want bits [14:7] of that.
;   RES1 is bits [15:8]. But we want >>7 which is RES1<<1 | RES0>>7.
;   Simpler: degrees * 182 / 256 = RES1 of (degrees * 182). Let's use 182.
;   Output: A = u8 angle
;   Clobbers: uses math coprocessor regs
; ---------------------------------------------------------------------
degrees_to_u8:
      LDA   eval_val_lo
      STA   MATH_MUL16_A_LO
      LDA   eval_val_hi
      STA   MATH_MUL16_A_HI
      LDA   #182
      STA   MATH_MUL16_B_LO
      STZ   MATH_MUL16_B_HI       ; trigger multiply
      LDA   MATH_RES1              ; high byte = degrees * 182 / 256
      RTS

; ---------------------------------------------------------------------
; do_sin — SIN degrees: return sin as signed byte (-128..+127)
; ---------------------------------------------------------------------
do_sin:
      JSR   degrees_to_u8
      STA   MATH_SINCOS_ANGLE
      LDA   MATH_RES0              ; sin result (signed 1.7)
      JMP   return_signed_byte

; ---------------------------------------------------------------------
; do_cos — COS degrees: return cos as signed byte (-128..+127)
; ---------------------------------------------------------------------
do_cos:
      JSR   degrees_to_u8
      STA   MATH_SINCOS_ANGLE
      LDA   MATH_RES1              ; cos result (signed 1.7)
      JMP   return_signed_byte

; ---------------------------------------------------------------------
; do_abs — ABS n: return absolute value
; ---------------------------------------------------------------------
do_abs:
      LDA   eval_val_hi
      BPL   @done                  ; already positive
      ; Negate 24-bit value
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
@done:
      STZ   eval_type
      JMP   eval_continue

; ---------------------------------------------------------------------
; do_int — INT n: truncate fractional part
; ---------------------------------------------------------------------
do_int:
      STZ   eval_val_frac
      STZ   eval_type
      JMP   eval_continue

; ---------------------------------------------------------------------
; do_round — ROUND n: round to nearest integer
; ---------------------------------------------------------------------
do_round:
      LDA   eval_val_frac
      CMP   #128
      BCC   @trunc
      ; Round up: increment integer part
      LDA   eval_val_hi
      BPL   @pos_round
      ; Negative: actually round toward zero = truncate
      BRA   @trunc
@pos_round:
      INC   eval_val_lo
      BNE   @trunc
      INC   eval_val_hi
@trunc:
      STZ   eval_val_frac
      STZ   eval_type
      JMP   eval_continue

; ---------------------------------------------------------------------
; do_random — RANDOM n: return random number 0..n-1
;   Uses math coprocessor RNG + MUL16 to scale
; ---------------------------------------------------------------------
do_random:
      ; RNG byte * n / 256 gives 0..n-1
      LDA   MATH_RNG
      STA   MATH_MUL16_A_LO
      STZ   MATH_MUL16_A_HI
      LDA   eval_val_lo
      STA   MATH_MUL16_B_LO
      LDA   eval_val_hi
      STA   MATH_MUL16_B_HI       ; trigger
      LDA   MATH_RES1              ; high byte of (rng * n) = rng*n/256
      STA   eval_val_lo
      STZ   eval_val_hi
      JMP   return_number

; ---------------------------------------------------------------------
; do_sqrt — SQRT n: approximate sqrt via DIST_APPROX(n, 0)
; ---------------------------------------------------------------------
do_sqrt:
      LDA   eval_val_lo
      STA   MATH_DIST_DX_LO
      LDA   eval_val_hi
      STA   MATH_DIST_DX_HI
      STZ   MATH_DIST_DY_LO
      STZ   MATH_DIST_DY_HI       ; trigger (write to DY_HI triggers)
      LDA   MATH_RES0
      STA   eval_val_lo
      LDA   MATH_RES1
      STA   eval_val_hi
      JMP   return_number

; ---------------------------------------------------------------------
; do_remainder — REMAINDER a b: return a mod b via DIV coprocessor
; ---------------------------------------------------------------------
do_remainder:
      JSR   eval_expr              ; first arg (a)
      BCS   @err
      LDA   eval_val_lo
      PHA
      LDA   eval_val_hi
      PHA
      JSR   eval_expr              ; second arg (b)
      BCS   @err_pop
      ; b in eval_val, a on stack
      ; Check divide by zero
      LDA   eval_val_lo
      ORA   eval_val_hi
      BEQ   @zero_pop
      ; Set up 32÷16 division: dividend=a, divisor=b
      PLA
      STA   MATH_DIV_N_1           ; a_hi
      TAX                          ; save for sign-extend
      PLA
      STA   MATH_DIV_N_LO          ; a_lo
      ; Sign-extend a
      TXA
      ORA   #$7F
      BMI   @neg_a
      LDA   #$00
@neg_a:                            ; A=$FF if negative
      STA   MATH_DIV_N_2
      STA   MATH_DIV_N_HI
      LDA   eval_val_lo
      STA   MATH_DIV_D_LO
      LDA   eval_val_hi
      STA   MATH_DIV_D_HI          ; trigger
      ; Remainder is in RES2:RES3 (actually check what the copro returns)
      ; The math copro returns quotient in RES0:RES1, remainder in RES2:RES3
      LDA   MATH_RES2
      STA   eval_val_lo
      LDA   MATH_RES3
      STA   eval_val_hi
      JMP   return_number
@zero_pop:
      PLA
      PLA
      STZ   eval_val_lo
      STZ   eval_val_hi
      JMP   return_number
@err_pop:
      PLA
      PLA
@err:
      SEC
      RTS

; ---------------------------------------------------------------------
; do_not — NOT expr: return 1 if expr is 0, else 0
; ---------------------------------------------------------------------
do_not:
      LDA   eval_val_lo
      ORA   eval_val_hi
      ; If zero, set result to 1. If nonzero, set to 0.
      BNE   @false
      LDA   #1
      STA   eval_val_lo
      BRA   @done
@false:
      STZ   eval_val_lo
@done:
      STZ   eval_val_hi
      JMP   return_number

; ---------------------------------------------------------------------
; do_and — AND a b: return 1 if both nonzero, else 0
; ---------------------------------------------------------------------
do_and:
      JSR   eval_expr
      BCS   @err
      LDA   eval_val_lo
      ORA   eval_val_hi
      BEQ   @false                 ; first arg is 0 → short-circuit
      JSR   eval_expr
      BCS   @err
      LDA   eval_val_lo
      ORA   eval_val_hi
      BEQ   @false
      LDA   #1
      STA   eval_val_lo
      BRA   @done
@false:
      STZ   eval_val_lo
@done:
      STZ   eval_val_hi
      JMP   return_number
@err:
      SEC
      RTS

; ---------------------------------------------------------------------
; do_or — OR a b: return 1 if either nonzero, else 0
; ---------------------------------------------------------------------
do_or:
      JSR   eval_expr
      BCS   @err
      LDA   eval_val_lo
      ORA   eval_val_hi
      BNE   @true                  ; first arg nonzero → short-circuit
      JSR   eval_expr
      BCS   @err
      LDA   eval_val_lo
      ORA   eval_val_hi
      BNE   @true
      STZ   eval_val_lo
      BRA   @done
@true:
      LDA   #1
      STA   eval_val_lo
@done:
      STZ   eval_val_hi
      JMP   return_number
@err:
      SEC
      RTS

; ---------------------------------------------------------------------
; Type predicates — NUMBER?, WORD?, LIST?
; ---------------------------------------------------------------------
do_numberp:
      LDA   eval_type
      CMP   #VAL_NUMBER
      JMP   predicate_result

do_wordp:
      LDA   eval_type
      CMP   #VAL_WORD
      JMP   predicate_result

do_listp:
      LDA   eval_type
      CMP   #VAL_LIST
      JMP   predicate_result

; ---------------------------------------------------------------------
; do_setcharbg — SETCHARBG <color>: opaque per-character text background.
;   Arity 1: dispatcher pre-evaluates the arg into eval_val (low byte = color
;   0-15). Store it straight to the VGC per-char background register so every
;   subsequently-printed char paints its cell with this color.
; ---------------------------------------------------------------------
do_setcharbg:
      LDA   eval_val_lo
      STA   VGC_TEXT_BG
      JMP   eval_continue

; ---------------------------------------------------------------------
; do_clearcharbg — CLEARCHARBG: revert to transparent per-char background.
;   Arity 0, no arguments. Writes VGC_TEXT_BG_TRANSPARENT ($10) so printed
;   chars again show the live screen background (the default).
; ---------------------------------------------------------------------
do_clearcharbg:
      LDA   #VGC_TEXT_BG_TRANSPARENT
      STA   VGC_TEXT_BG
      JMP   eval_continue

; ---------------------------------------------------------------------
; file_set_name_from_eval — marshal the quoted-word filename in eval_val into the
;   FILES mailbox: LIB_ARG0 = pointer to the name chars, LIB_ARG1 byte0 = length.
;   eval_val_lo/hi points at a length-prefixed string ([len][chars]). Caller must
;   have already verified eval_type = VAL_WORD. Clobbers A/Y, ptr_lo/hi.
; ---------------------------------------------------------------------
file_set_name_from_eval:
      LDA   eval_val_lo
      STA   ptr_lo
      LDA   eval_val_hi
      STA   ptr_hi
      LDY   #0
      LDA   (ptr_lo),Y              ; length byte
      STA   LIB_ARG1+0
      CLC
      LDA   eval_val_lo
      ADC   #1                      ; skip the length byte -> first char
      STA   LIB_ARG0+0
      LDA   eval_val_hi
      ADC   #0
      STA   LIB_ARG0+1
      RTS

; ---------------------------------------------------------------------
; do_catalog — CATALOG / DIR: list the current directory. Arity 0. namelen=0
;   makes the FILES module list everything; the host prints the formatted table.
; ---------------------------------------------------------------------
do_catalog:
      STZ   LIB_ARG1+0              ; namelen 0 -> list the current directory
      LDA   #MODULE_ID_FILES
      LDX   #FILE_DIR_LIST
      JSR   do_lib_call
      JMP   eval_continue

; ---------------------------------------------------------------------
; do_save — SAVE "name: serialize all user procedures to a Logo file. Arity 1
;   (the dispatcher pre-evaluates the filename word into eval_val).
; ---------------------------------------------------------------------
do_save:
      LDA   eval_type
      CMP   #VAL_WORD
      BEQ   @ok
      LDX   #<str_save_name
      LDY   #>str_save_name
      JMP   err_nei
@ok:
      JSR   file_set_name_from_eval ; ARG0 = nameptr, ARG1 byte0 = namelen
      LDA   #FIO_TYPE_LOGO
      STA   LIB_ARG1+1             ; file type rides the namelen cell's high byte
      JSR   proc_serialize_all     ; fills ARG2 = src, ARG3 = end (proc text)
      LDA   #MODULE_ID_FILES
      LDX   #FILE_SAVE
      JSR   do_lib_call
      JMP   eval_continue

; ---------------------------------------------------------------------
; do_load — LOAD "name: read a Logo file and define the procedures it contains.
;   Arity 1. Loads the text into proc_body_buf then feeds it to proc_load_text.
; ---------------------------------------------------------------------
do_load:
      LDA   eval_type
      CMP   #VAL_WORD
      BEQ   @ok
      LDX   #<str_load_name
      LDY   #>str_load_name
      JMP   err_nei
@ok:
      JSR   file_set_name_from_eval
      LDA   #FIO_TYPE_LOGO
      STA   LIB_ARG1+1
      STA   FIO_DIRTYPE            ; ask the host to treat it as a Logo file
      ; Load destination = save_buf (separate from proc_body_buf, which the
      ; per-procedure body builder reuses while parsing).
      LDA   #<save_buf
      STA   LIB_ARG2+0
      LDA   #>save_buf
      STA   LIB_ARG2+1
      LDA   #MODULE_ID_FILES
      LDX   #FILE_LOAD
      JSR   do_lib_call
      BEQ   @loaded
      JMP   eval_continue          ; load failed -> nothing defined
@loaded:
      LDA   FIO_SIZEL              ; bytes loaded -> scan length
      STA   load_rem_lo
      LDA   FIO_SIZEH
      STA   load_rem_hi
      LDA   #<save_buf
      STA   load_pos_lo
      LDA   #>save_buf
      STA   load_pos_hi
      ; Save the bits of outer eval state that proc_load_text's nested tokenize/
      ; eval clobber and that eval_continue needs back. tok_head/tok_tail are
      ; rebuilt by the next REPL line, and eval_reporter is 0 for a command, so
      ; only eval_cur (resume point) and eval_in_body (body context) need saving.
      LDA   eval_cur_lo
      PHA
      LDA   eval_cur_hi
      PHA
      LDA   eval_in_body
      PHA
      LDA   #1
      STA   proc_load_active       ; route TO through the non-interactive collector
      JSR   proc_load_text
      STZ   proc_load_active
      PLA
      STA   eval_in_body
      PLA
      STA   eval_cur_hi
      PLA
      STA   eval_cur_lo
      JMP   eval_continue

; =====================================================================
; RODATA — builtin table and name strings
; =====================================================================
      .segment "RODATA"

str_oom:
      .byte "OUT OF MEMORY", 0




str_if_bracket:
      .byte "IF NEEDS [ BODY ]", 0


str_ifelse_bracket:
      .byte "IFELSE NEEDS [ BODY ]", 0


str_catch_bracket:
      .byte "CATCH NEEDS TAG [BODY]", 0


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
str_catch_name:
      .byte 5, "CATCH"
str_throw_name:
      .byte 5, "THROW"
str_repcount_name:
      .byte 8, "REPCOUNT"
str_for_name:
      .byte 3, "FOR"
str_while_name:
      .byte 5, "WHILE"
str_until_name:
      .byte 5, "UNTIL"
str_readchar_name:
      .byte 8, "READCHAR"
str_readword_name:
      .byte 8, "READWORD"
str_readlist_name:
      .byte 8, "READLIST"
str_char_name:
      .byte 4, "CHAR"
str_ascii_name:
      .byte 5, "ASCII"
str_sin_name:
      .byte 3, "SIN"
str_cos_name:
      .byte 3, "COS"
str_abs_name:
      .byte 3, "ABS"
str_int_name:
      .byte 3, "INT"
str_round_name:
      .byte 5, "ROUND"
str_random_name:
      .byte 6, "RANDOM"
str_sqrt_name:
      .byte 4, "SQRT"
str_remainder_name:
      .byte 9, "REMAINDER"
str_not_name:
      .byte 3, "NOT"
str_and_name:
      .byte 3, "AND"
str_or_name:
      .byte 2, "OR"
str_numberp_name:
      .byte 7, "NUMBER?"
str_wordp_name:
      .byte 5, "WORD?"
str_listp_name:
      .byte 5, "LIST?"
str_setcharbg_name:
      .byte 9, "SETCHARBG"
str_clearcharbg_name:
      .byte 11, "CLEARCHARBG"
str_save_name:
      .byte 4, "SAVE"
str_load_name:
      .byte 4, "LOAD"
str_catalog_name:
      .byte 7, "CATALOG"
str_dir_name:
      .byte 3, "DIR"

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

      .word str_catch_name
      .word do_catch
      .byte 0                    ; arity 0: do_catch handles its own args

      .word str_throw_name
      .word do_throw
      .byte 0                    ; arity 0: do_throw handles its own args

      .word str_repcount_name
      .word do_repcount
      .byte 0                    ; arity 0: reporter

      .word str_for_name
      .word do_for
      .byte 0                    ; arity 0: do_for handles its own args

      .word str_while_name
      .word do_while
      .byte 0                    ; arity 0: do_while handles its own args

      .word str_until_name
      .word do_until
      .byte 0                    ; arity 0: do_until handles its own args

      .word str_readchar_name
      .word do_readchar
      .byte 0                    ; arity 0: reporter

      .word str_readword_name
      .word do_readword
      .byte 0                    ; arity 0: reporter

      .word str_readlist_name
      .word do_readlist
      .byte 0                    ; arity 0: reporter

      .word str_char_name
      .word do_char
      .byte 0                    ; arity 0: handles its own arg

      .word str_ascii_name
      .word do_ascii
      .byte 0                    ; arity 0: handles its own arg

      .word str_sin_name
      .word do_sin
      .byte 1                    ; arity 1: dispatcher pre-evaluates the arg

      .word str_cos_name
      .word do_cos
      .byte 1                    ; arity 1: dispatcher pre-evaluates the arg

      .word str_abs_name
      .word do_abs
      .byte 1                    ; arity 1: dispatcher pre-evaluates the arg

      .word str_int_name
      .word do_int
      .byte 1                    ; arity 1: dispatcher pre-evaluates the arg

      .word str_round_name
      .word do_round
      .byte 1                    ; arity 1: dispatcher pre-evaluates the arg

      .word str_random_name
      .word do_random
      .byte 1                    ; arity 1: dispatcher pre-evaluates the arg

      .word str_sqrt_name
      .word do_sqrt
      .byte 1                    ; arity 1: dispatcher pre-evaluates the arg

      .word str_remainder_name
      .word do_remainder
      .byte 0

      .word str_not_name
      .word do_not
      .byte 1                    ; arity 1: dispatcher pre-evaluates the arg

      .word str_and_name
      .word do_and
      .byte 0

      .word str_or_name
      .word do_or
      .byte 0

      .word str_numberp_name
      .word do_numberp
      .byte 1                    ; arity 1: dispatcher pre-evaluates the arg

      .word str_wordp_name
      .word do_wordp
      .byte 1                    ; arity 1: dispatcher pre-evaluates the arg

      .word str_listp_name
      .word do_listp
      .byte 1                    ; arity 1: dispatcher pre-evaluates the arg

      .word str_po_name
      .word do_po
      .byte 0

      .word str_printout_name
      .word do_po
      .byte 0                    ; PRINTOUT = PO alias

      .word str_pots_name
      .word do_pots
      .byte 0

      .word str_printtitles_name
      .word do_pots
      .byte 0                    ; PRINTTITLES = POTS alias

      .word str_erase_name
      .word do_erase
      .byte 0

      .word str_apply_name
      .word do_apply
      .byte 0

      .word str_setcharbg_name
      .word do_setcharbg
      .byte 1                    ; arity 1: dispatcher pre-evaluates the color

      .word str_clearcharbg_name
      .word do_clearcharbg
      .byte 0                    ; arity 0: no arguments

      .word str_save_name
      .word do_save
      .byte 1                    ; arity 1: dispatcher pre-evaluates the filename

      .word str_load_name
      .word do_load
      .byte 1                    ; arity 1: dispatcher pre-evaluates the filename

      .word str_catalog_name
      .word do_catalog
      .byte 0                    ; arity 0: list the current directory

      .word str_dir_name
      .word do_catalog
      .byte 0                    ; DIR = alias for CATALOG

      .word $0000               ; end sentinel

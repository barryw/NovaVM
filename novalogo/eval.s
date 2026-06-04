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
eval_in_body:   .res 1    ; $00 = normal eval_loop, $01 = in body
proc_stopped:   .res 1    ; $00 = running, $01 = STOP, $02 = OUTPUT
eval_reporter:  .res 1    ; $00 = normal, $01 = reporter mode (return from eval_continue)
ext_mod_id:     .res 1    ; looked-up ext_cmd_table module_id (MODULE_ID_NONE=legacy path)

; --- Catch frame for CATCH/THROW error handling ---
catch_active:   .res 1    ; $00 = no catch, $01 = catch active
catch_tag_lo:   .res 1    ; pointer to tag word (length-prefixed)
catch_tag_hi:   .res 1
catch_sp:       .res 1    ; saved 6502 stack pointer for unwinding
catch_resume_lo: .res 1   ; eval_cur to resume at (past the body's ])
catch_resume_hi: .res 1

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
      STZ   eval_in_body
      STZ   proc_stopped
      STZ   eval_reporter
      LDA   tok_head_lo
      STA   eval_cur_lo
      LDA   tok_head_hi
      STA   eval_cur_hi

eval_loop:
      ; Check if STOP/OUTPUT was signaled — bail out
      LDA   proc_stopped
      BNE   @bail_out

      ; Check for end of list
      LDA   eval_cur_lo
      ORA   eval_cur_hi
      BEQ   @bail_out

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

@bail_out:
      RTS

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
      ; Save handler address — eval_expr may invoke proc_invoke which
      ; clobbers handler_lo/hi via inner eval_loop dispatch
      LDA   handler_hi
      PHA
      LDA   handler_lo
      PHA
@eval_args:
      PHX                   ; save remaining arg count
      JSR   eval_expr
      BCS   @arg_error
      PLX
      DEX
      BNE   @eval_args
      ; Restore handler address
      PLA
      STA   handler_lo
      PLA
      STA   handler_hi

@call_handler:
      ; Call the handler via indirect JMP
      JMP   (handler_lo)
      ; handlers must JMP back to eval_loop via eval_continue

@arg_error:
      PLX                   ; discard saved arity counter
      ; Discard saved handler address
      PLA
      PLA
      ; Print error and abort line
      JMP   err_notenough_throw

@unknown:
      ; Try user-defined procedures before erroring
      JSR   proc_lookup
      BCS   @try_ext_cmd
      ; Found a procedure — invoke it (proc_stopped restored internally)
      JSR   proc_invoke
      JMP   eval_loop

@try_ext_cmd:
      ; Try extension command table before giving up
      JSR   lookup_ext_cmd
      BCS   @truly_unknown
      ; Found: A = arity, ext_cmd set.
      ; Advance past the command word token.
      TAX                       ; X = arity
      PHX
      JSR   eval_advance
      PLX
      ; Evaluate arguments and copy to ext_arg slots
      JSR   ext_eval_args
      BCS   @ext_arg_err
      ; Call extension via RAM trampoline
      JSR   EXT_TRAMPOLINE
      ; Copy result back to eval_val
      LDA   EXT_RESULT_TYPE
      STA   eval_type
      LDA   EXT_RESULT_HI
      STA   eval_val_hi
      LDA   EXT_RESULT_LO
      STA   eval_val_lo
      LDA   EXT_RESULT_FRAC
      STA   eval_val_frac
      JMP   eval_continue

@ext_arg_err:
      JMP   err_notenough_throw

@truly_unknown:
      ; Print "I don't know how to " + the word
      JMP   err_idk_word

@done:
      RTS

; ---------------------------------------------------------------------
; eval_continue — handlers JMP here to resume the eval loop
;   If eval_in_body != 0, resumes body execution instead
; ---------------------------------------------------------------------
; ---------------------------------------------------------------------
; jmp_handler — trampoline for calling builtins from reporter position
; ---------------------------------------------------------------------
jmp_handler:
      JMP   (handler_lo)

eval_continue:
      ; If in reporter mode, return to eval_expr instead of looping
      LDA   eval_reporter
      BNE   @reporter_return
      LDA   eval_in_body
      BNE   eval_body
      JMP   eval_loop

@reporter_return:
      STZ   eval_reporter
      CLC
      RTS

; ---------------------------------------------------------------------
; eval_body — execute tokens within a body until TOK_RBRACKET at depth 0
;   Entry: eval_cur is at the next token to execute inside the body
;   Exit:  eval_cur points at the TOK_RBRACKET that ended the body
;   eval_in_body must be $01 on entry
; ---------------------------------------------------------------------
eval_body:
      ; Check if STOP/OUTPUT was signaled — bail out immediately
      LDA   proc_stopped
      BNE   @done

      ; Check for end of list (safety — malformed input)
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

      ; Hit the closing bracket? We're done with this body pass.
      CMP   #TOK_RBRACKET
      BEQ   @done

      ; Dispatch command words (same as eval_loop)
      CMP   #TOK_WORD
      BEQ   @do_command

      ; Skip non-word tokens (bare numbers, brackets inside expressions, etc.)
      JSR   eval_advance
      BRA   eval_body

@do_command:
      ; Same dispatch as eval_loop — look up builtin, eval args, call handler
      JSR   lookup_builtin
      BCS   @unknown

      LDX   ptr2_lo
      STX   handler_lo
      LDX   ptr2_hi
      STX   handler_hi
      TAX
      PHX
      JSR   eval_advance
      PLX

      CPX   #0
      BEQ   @call_handler
      ; Save handler address — eval_expr may invoke proc_invoke
      LDA   handler_hi
      PHA
      LDA   handler_lo
      PHA
@eval_args:
      PHX
      JSR   eval_expr
      BCS   @arg_error
      PLX
      DEX
      BNE   @eval_args
      ; Restore handler address
      PLA
      STA   handler_lo
      PLA
      STA   handler_hi

@call_handler:
      JMP   (handler_lo)
      ; handler JMPs to eval_continue, which routes back here

@arg_error:
      PLX
      PLA
      PLA
      JMP   err_notenough_throw

@done:
      RTS

@unknown:
      ; Try user-defined procedures before erroring (inside body)
      JSR   proc_lookup
      BCS   @try_ext_cmd
      JSR   proc_invoke
      JMP   eval_body

@try_ext_cmd:
      ; Try extension command table before giving up (inside body)
      JSR   lookup_ext_cmd
      BCS   @truly_unknown
      TAX
      PHX
      JSR   eval_advance
      PLX
      JSR   ext_eval_args
      BCS   @body_ext_err
      JSR   EXT_TRAMPOLINE
      LDA   EXT_RESULT_TYPE
      STA   eval_type
      LDA   EXT_RESULT_HI
      STA   eval_val_hi
      LDA   EXT_RESULT_LO
      STA   eval_val_lo
      LDA   EXT_RESULT_FRAC
      STA   eval_val_frac
      JMP   eval_continue

@body_ext_err:
      JMP   err_notenough_throw

@truly_unknown:
      ; Print "I don't know how to " + the word (inside body)
      JMP   err_idk_word

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
      BNE   :+
      JMP   @number
:     CMP   #TOK_QUOTE
      BNE   :+
      JMP   @quote
:     CMP   #TOK_VARREF
      BNE   :+
      JMP   @varref
:     CMP   #TOK_WORD
      BEQ   @word_expr
      CMP   #TOK_LBRACKET
      BEQ   @list_literal
      ; Anything else is an error
      SEC
      RTS

@word_expr:
      ; Could be a reporter procedure (returns a value via OUTPUT)
      JSR   proc_lookup
      BCS   @word_not_proc        ; not a procedure — try builtins
      ; It's a procedure — invoke it
      JSR   proc_invoke
      ; X = exit status: $02 = OUTPUT (eval_val has value), else no value
      CPX   #$02
      BEQ   @word_has_val
      ; No OUTPUT — set result to 0
      STZ   eval_val_hi
      STZ   eval_val_lo
      STZ   eval_val_frac
      STZ   eval_type
@word_has_val:
      JMP   eval_check_infix      ; allow DOUBLE 5 + 3 etc.

@list_literal:
      ; Advance past the [
      JSR   eval_advance
      ; Build list from tokens until ]
      JSR   eval_list
      ; eval_type = VAL_LIST, eval_val = head pointer
      ; Carry already set/clear from eval_list
      RTS

@word_not_proc:
      ; Not a user proc — try built-in reporter
      JSR   lookup_builtin
      BCS   @word_unknown
      ; Found: ptr2_lo/hi = handler addr, A = arity
      ; Save handler address
      LDX   ptr2_lo
      STX   handler_lo
      LDX   ptr2_hi
      STX   handler_hi
      TAX                       ; X = arity
      PHX                       ; save arity across eval_advance
      ; Advance past the command word
      JSR   eval_advance
      PLX                       ; restore arity
      ; Evaluate arguments (X = arity count)
      CPX   #0
      BEQ   @call_reporter
      ; Save handler address — eval_expr may clobber it
      LDA   handler_hi
      PHA
      LDA   handler_lo
      PHA
@rpt_eval_args:
      PHX
      JSR   eval_expr
      BCS   @rpt_arg_error
      PLX
      DEX
      BNE   @rpt_eval_args
      ; Restore handler address
      PLA
      STA   handler_lo
      PLA
      STA   handler_hi
@call_reporter:
      ; Set reporter mode so eval_continue returns here instead of looping
      LDA   #$01
      STA   eval_reporter
      ; Call via JSR to trampoline — handler JMPs to eval_continue
      ; which sees eval_reporter and does CLC + RTS back here.
      JSR   jmp_handler
      ; eval_val now set by the handler
      JMP   eval_check_infix

@rpt_arg_error:
      PLX                       ; discard saved arity counter
      PLA
      PLA                       ; discard saved handler address
      SEC
      RTS

@word_unknown:
      ; Try extension command table as reporter
      JSR   lookup_ext_cmd
      BCS   @word_really_unknown
      ; Found: A = arity, EXT_CMD set
      TAX
      PHX
      JSR   eval_advance
      PLX
      JSR   ext_eval_args
      BCS   @word_really_unknown
      JSR   EXT_TRAMPOLINE
      ; Copy result back to eval_val
      LDA   EXT_RESULT_TYPE
      STA   eval_type
      LDA   EXT_RESULT_HI
      STA   eval_val_hi
      LDA   EXT_RESULT_LO
      STA   eval_val_lo
      LDA   EXT_RESULT_FRAC
      STA   eval_val_frac
      CLC
      JMP   eval_check_infix

@word_really_unknown:
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
      JSR   print_inl
      .byte " HAS NO VALUE", 0
      JSR   eval_newline
      JSR   try_throw_error
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
; print_inl — print the null-terminated string that immediately follows the
;   JSR in the instruction stream, then resume execution after the 0.
;   Usage:  JSR print_inl
;           .byte "TEXT", 0
;   Clobbers: A, Y, ptr_lo, ptr_hi.
; ---------------------------------------------------------------------
print_inl:
      PLA
      STA   ptr_lo
      PLA
      STA   ptr_hi          ; ptr = (return addr)-1 = (inline string addr)-1
      LDY   #0
@pi_lp:
      INY
      LDA   (ptr_lo),Y
      BEQ   @pi_done
      STA   VGC_CHAROUT
      BRA   @pi_lp
@pi_done:
      TYA
      CLC
      ADC   ptr_lo
      STA   ptr_lo          ; ptr_lo = low(ptr+Y)
      LDA   ptr_hi
      ADC   #0              ; A = high(ptr+Y)
      PHA                   ; push high of (ptr+Y)
      LDA   ptr_lo
      PHA                   ; push low
      RTS                   ; RTS jumps to (ptr+Y)+1 = byte after the 0

; =====================================================================
; Message composer (4a) — no error string is stored whole. Every error
; message is composed at print time from shared fragments + the command's
; existing length-prefixed *_name string. Family helpers fall through into
; list_print_err's shared newline+continue tail (err_nl_continue).
; =====================================================================

; print_cstr_xy — print a null-terminated string.
;   In: X/Y = lo/hi pointer.  Clobbers: A, Y, ptr_lo/hi.
print_cstr_xy:
      STX   ptr_lo
      STY   ptr_hi
      LDY   #0
@pc_lp:
      LDA   (ptr_lo),Y
      BEQ   @pc_done
      STA   VGC_CHAROUT
      INY
      BNE   @pc_lp
@pc_done:
      RTS

; print_name_xy — print a length-prefixed name ( .byte len,"TEXT" ).
;   In: X/Y = lo/hi pointer.  Clobbers: A, X, Y, ptr_lo/hi.
print_name_xy:
      STX   ptr_lo
      STY   ptr_hi
      LDY   #0
      LDA   (ptr_lo),Y      ; length
      BEQ   @pn_done
      TAX
@pn_lp:
      INY
      LDA   (ptr_lo),Y
      STA   VGC_CHAROUT
      DEX
      BNE   @pn_lp
@pn_done:
      RTS

; --- family error helpers: enter with X/Y = command's *_name string ptr ---

; "NOT ENOUGH INPUTS TO <name>"  (reuses str_notenough so "NOT ENOUGH INPUTS"
; is never stored twice)
err_nei:
      STX   num_tmp_lo            ; save name ptr across the fragment prints
      STY   num_tmp_hi
      LDX   #<str_notenough
      LDY   #>str_notenough
      JSR   print_cstr_xy         ; "NOT ENOUGH INPUTS"
      LDX   #<frag_to
      LDY   #>frag_to
      JSR   print_cstr_xy         ; " TO "
      LDX   num_tmp_lo
      LDY   num_tmp_hi
      JSR   print_name_xy         ; the command name
      JMP   err_nl_continue       ; newline + abandon line

; "<name> DOESN'T LIKE "  (note: trailing space, no value — matches the ROM)
err_doesnt_like:
      JSR   print_name_xy
      LDX   #<frag_dl
      LDY   #>frag_dl
      JMP   list_print_err

; "<name> NEEDS A LIST"
err_needs_list:
      JSR   print_name_xy
      LDX   #<frag_needs_list
      LDY   #>frag_needs_list
      JMP   list_print_err

; "<name> INDEX OUT OF RANGE"
err_index_range:
      JSR   print_name_xy
      LDX   #<frag_index_range
      LDY   #>frag_index_range
      JMP   list_print_err

; "<name> DOESN'T LIKE [] AS INPUT"
err_dl_empty:
      JSR   print_name_xy
      LDX   #<frag_dl
      LDY   #>frag_dl
      JSR   print_cstr_xy         ; " DOESN'T LIKE "
      LDX   #<frag_as_input
      LDY   #>frag_as_input
      JMP   list_print_err        ; "[] AS INPUT" + newline + continue

; "<name> DOESN'T LIKE THIS INPUT"
err_dl_this:
      JSR   print_name_xy
      LDX   #<frag_dl
      LDY   #>frag_dl
      JSR   print_cstr_xy         ; " DOESN'T LIKE "
      LDX   #<frag_this_input
      LDY   #>frag_this_input
      JMP   list_print_err        ; "THIS INPUT" + newline + continue

; --- throw-tailed error helpers (arg/unknown paths inside eval_loop/eval_body;
;     these are catchable, so they end in try_throw_error, NOT eval_continue) ---

; "NOT ENOUGH INPUTS" (generic, no command name) + throw-tail
err_notenough_throw:
      LDX   #<str_notenough
      LDY   #>str_notenough
      JSR   print_cstr_xy
      JMP   err_throw

; "I DON'T KNOW HOW TO " + the current token's word + throw-tail
err_idk_word:
      LDX   #<str_idk
      LDY   #>str_idk
      JSR   print_cstr_xy
      JSR   print_token_word
err_throw:                        ; <- fall-through from err_idk_word
      JSR   eval_newline
      JSR   try_throw_error
      RTS

; print_token_word — print the length-prefixed word in the current token
;   (eval_cur + TOK_PAYLOAD).  Clobbers A, X, Y, ptr_lo/hi.
print_token_word:
      LDA   eval_cur_lo
      STA   ptr_lo
      LDA   eval_cur_hi
      STA   ptr_hi
      LDY   #TOK_PAYLOAD          ; length byte
      LDA   (ptr_lo),Y
      TAX
      LDY   #TOK_PAYLOAD+1        ; first char
@ptw_lp:
      LDA   (ptr_lo),Y
      STA   VGC_CHAROUT
      INY
      DEX
      BNE   @ptw_lp
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

      ; Push left operand and operator onto hardware stack so recursive
      ; proc_invoke calls (which may evaluate their own infix expressions)
      ; cannot clobber them.
      LDA   eval_left_hi
      PHA
      LDA   eval_left_lo
      PHA
      LDA   eval_left_frac
      PHA
      LDA   eval_op
      PHA

      ; Evaluate the right operand (recursive call)
      JSR   eval_expr
      BCS   @err_rhs_pop

      ; Restore left operand and operator from stack
      PLA
      STA   eval_op
      PLA
      STA   eval_left_frac
      PLA
      STA   eval_left_lo
      PLA
      STA   eval_left_hi

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
      CMP   #'>'
      BNE   :+
      JMP   @do_gt
:     CMP   #'<'
      BNE   :+
      JMP   @do_lt
:     CMP   #'='
      BNE   :+
      JMP   @do_eq
:
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

@err_rhs_pop:
      ; Discard saved left operand + operator (4 bytes)
      PLA
      PLA
      PLA
      PLA
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

      ; ----- Equality: left = right (all 3 bytes must match) -----
@do_eq:
      LDA   eval_left_hi
      CMP   eval_val_hi
      BNE   @set_false
      LDA   eval_left_lo
      CMP   eval_val_lo
      BNE   @set_false
      LDA   eval_left_frac
      CMP   eval_val_frac
      BNE   @set_false
      BRA   @set_true

      ; ----- Greater than: left > right (signed 24-bit) -----
@do_gt:
      ; Compare hi bytes (signed)
      LDA   eval_left_hi
      SEC
      SBC   eval_val_hi
      BVC   @gt_sign_ok           ; no overflow — N flag valid
      EOR   #$80                  ; overflow — invert N to get true sign
@gt_sign_ok:
      BMI   @set_false            ; left_hi < right_hi → false
      ; If the subtraction was nonzero and positive, left > right
      LDA   eval_left_hi
      CMP   eval_val_hi
      BNE   @set_true             ; hi bytes differ, and left > right
      ; Hi bytes equal — compare lo bytes (unsigned)
      LDA   eval_left_lo
      CMP   eval_val_lo
      BNE   @gt_lo_diff
      ; Lo bytes equal — compare frac bytes (unsigned)
      LDA   eval_left_frac
      CMP   eval_val_frac
      BEQ   @set_false            ; all equal → not greater
      BCS   @set_true
      BRA   @set_false
@gt_lo_diff:
      BCS   @set_true
      BRA   @set_false

      ; ----- Less than: left < right (signed 24-bit) -----
@do_lt:
      ; Compare hi bytes (signed)
      LDA   eval_left_hi
      SEC
      SBC   eval_val_hi
      BVC   @lt_sign_ok
      EOR   #$80
@lt_sign_ok:
      BMI   @set_true             ; left_hi < right_hi → true
      LDA   eval_left_hi
      CMP   eval_val_hi
      BNE   @set_false            ; left_hi > right_hi → false
      ; Hi bytes equal — compare lo bytes (unsigned)
      LDA   eval_left_lo
      CMP   eval_val_lo
      BNE   @lt_lo_diff
      ; Lo bytes equal — compare frac (unsigned)
      LDA   eval_left_frac
      CMP   eval_val_frac
      BEQ   @set_false            ; equal → not less
      BCC   @set_true
      BRA   @set_false
@lt_lo_diff:
      BCC   @set_true
      BRA   @set_false

      ; ----- Set result to TRUE (1) or FALSE (0) -----
@set_true:
      STZ   eval_val_hi
      LDA   #1
      STA   eval_val_lo
      STZ   eval_val_frac
      STZ   eval_type
      CLC
      RTS

@set_false:
      STZ   eval_val_hi
      STZ   eval_val_lo
      STZ   eval_val_frac
      STZ   eval_type
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

; ---------------------------------------------------------------------
; try_throw_error — if a CATCH with tag "ERROR" is active, unwind to it
;   Called after error messages are printed. If no catch is active or
;   the tag doesn't match "ERROR", this is a no-op (returns normally).
;   If a match is found, this NEVER RETURNS — it unwinds the stack
;   via TXS and jumps to the catch resume point.
; ---------------------------------------------------------------------
try_throw_error:
      LDA   catch_active
      BEQ   @no_catch

      ; Compare catch_tag against "ERROR" (5 chars, uppercased)
      LDA   catch_tag_lo
      STA   ptr2_lo
      LDA   catch_tag_hi
      STA   ptr2_hi
      LDA   #<str_error_tag
      STA   ptr_lo
      LDA   #>str_error_tag
      STA   ptr_hi
      JSR   catch_tags_equal
      BCC   @no_catch

      ; Match! Unwind stack to catch point and resume.
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
      RTS

; ---------------------------------------------------------------------
; catch_tags_equal — compare two length-prefixed strings (case-insensitive)
;   Input: ptr_lo/hi = string A, ptr2_lo/hi = string B
;   Output: carry set = equal, carry clear = not equal
;   Clobbers: A, X, Y
; ---------------------------------------------------------------------
catch_tags_equal:
      LDY   #0
      LDA   (ptr_lo),Y           ; length of A
      STA   num_tmp_lo            ; temp: len A
      LDA   (ptr2_lo),Y          ; length of B
      CMP   num_tmp_lo
      BNE   @ne

      ; Lengths match — compare chars
      TAX                         ; X = length
      BEQ   @eq                   ; both zero = equal
      LDY   #1
@cmp:
      LDA   (ptr_lo),Y
      ; Uppercase A char
      CMP   #'a'
      BCC   @a_up
      CMP   #'z'+1
      BCS   @a_up
      SEC
      SBC   #$20
@a_up:
      STA   num_tmp_lo            ; uppercased A char

      LDA   (ptr2_lo),Y
      ; Uppercase B char
      CMP   #'a'
      BCC   @b_up
      CMP   #'z'+1
      BCS   @b_up
      SEC
      SBC   #$20
@b_up:
      CMP   num_tmp_lo
      BNE   @ne
      INY
      DEX
      BNE   @cmp

@eq:
      SEC
      RTS
@ne:
      CLC
      RTS

; =====================================================================
; RODATA — evaluator strings and tables
; =====================================================================
; ---------------------------------------------------------------------
; lookup_ext_cmd — search ext_cmd_table for the current TOK_WORD
;   Input: eval_cur_lo/hi points to a TOK_WORD token
;   Output: Carry clear = found, A = arity, EXT_CMD set
;           Carry set = not found
;   Clobbers: A, X, Y, num_tmp_lo/hi
; ---------------------------------------------------------------------
lookup_ext_cmd:
      ; Save table pointer in num_tmp_lo/hi
      LDA   #<ext_cmd_table
      STA   num_tmp_lo
      LDA   #>ext_cmd_table
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

      ; Match found — read module_id, fn_id and arity
      ; Table entry: name_ptr(2) + module_id(1) + fn_id(1) + arity(1)
      LDA   num_tmp_lo
      STA   ptr_lo
      LDA   num_tmp_hi
      STA   ptr_hi
      LDY   #2
      LDA   (ptr_lo),Y         ; module_id
      STA   ext_mod_id
      INY
      LDA   (ptr_lo),Y         ; fn_id (legacy EXT_CMD value)
      STA   EXT_CMD
      INY
      LDA   (ptr_lo),Y         ; arity
      CLC                       ; found
      RTS

@next:
      STZ   tok_sign
      ; Advance to next entry: +5 bytes (2 name + 1 module_id + 1 fn_id + 1 arity)
      CLC
      LDA   num_tmp_lo
      ADC   #5
      STA   num_tmp_lo
      BCC   :+
      INC   num_tmp_hi
:
      JMP   @scan

; ---------------------------------------------------------------------
; ext_eval_args — evaluate X arguments and copy to EXT_ARG0..2 slots
;   Input: X = argument count (0-3)
;   Output: Carry clear = success, Carry set = error
;           EXT_ARGC set, EXT_ARG0..2 populated
;   Clobbers: A, X, Y
; ---------------------------------------------------------------------
ext_eval_args:
      STX   EXT_ARGC
      CPX   #0
      BEQ   @done

      ; Evaluate arg 0
      PHX
      JSR   eval_expr
      PLX
      BCS   @err
      LDA   eval_type
      STA   EXT_ARG0_TYPE
      LDA   eval_val_hi
      STA   EXT_ARG0_HI
      LDA   eval_val_lo
      STA   EXT_ARG0_LO
      LDA   eval_val_frac
      STA   EXT_ARG0_FRAC
      DEX
      BEQ   @done

      ; Evaluate arg 1
      PHX
      JSR   eval_expr
      PLX
      BCS   @err
      LDA   eval_type
      STA   EXT_ARG1_TYPE
      LDA   eval_val_hi
      STA   EXT_ARG1_HI
      LDA   eval_val_lo
      STA   EXT_ARG1_LO
      LDA   eval_val_frac
      STA   EXT_ARG1_FRAC
      DEX
      BEQ   @done

      ; Evaluate arg 2
      PHX
      JSR   eval_expr
      PLX
      BCS   @err
      LDA   eval_type
      STA   EXT_ARG2_TYPE
      LDA   eval_val_hi
      STA   EXT_ARG2_HI
      LDA   eval_val_lo
      STA   EXT_ARG2_LO
      LDA   eval_val_frac
      STA   EXT_ARG2_FRAC
      DEX
      BEQ   @done

      ; Evaluate arg 3 (for LINE, RECT, FILL etc.)
      PHX
      JSR   eval_expr
      PLX
      BCS   @err
      LDA   eval_type
      STA   EXT_ARG3_TYPE
      LDA   eval_val_hi
      STA   EXT_ARG3_HI
      LDA   eval_val_lo
      STA   EXT_ARG3_LO
      LDA   eval_val_frac
      STA   EXT_ARG3_FRAC

@done:
      CLC
      RTS
@err:
      SEC
      RTS

      .segment "RODATA"

; Extension command table: name_ptr(2) + module_id(1) + fn_id(1) + arity(1)
; module_id is MODULE_ID_NONE for every entry (legacy RAM-trampoline path);
; 4c.1-2 will set it per-command to route through the paged-library lib_call.
; Terminated by $0000 sentinel.
ext_cmd_table:
      .word str_ext_test_name
      .byte MODULE_ID_NONE
      .byte EXT_CMD_TEST
      .byte 1                    ; arity: 1 argument
      .word str_ext_fd
      .byte MODULE_ID_NONE
      .byte EXT_CMD_FD
      .byte 1                    ; arity: 1 (distance)
      .word str_ext_forward
      .byte MODULE_ID_NONE
      .byte EXT_CMD_FD
      .byte 1                    ; FORWARD alias
      .word str_ext_bk
      .byte MODULE_ID_NONE
      .byte EXT_CMD_BK
      .byte 1                    ; arity: 1 (distance)
      .word str_ext_back
      .byte MODULE_ID_NONE
      .byte EXT_CMD_BK
      .byte 1                    ; BACK alias
      .word str_ext_backward
      .byte MODULE_ID_NONE
      .byte EXT_CMD_BK
      .byte 1                    ; BACKWARD alias
      .word str_ext_rt
      .byte MODULE_ID_NONE
      .byte EXT_CMD_RT
      .byte 1                    ; arity: 1 (degrees)
      .word str_ext_right
      .byte MODULE_ID_NONE
      .byte EXT_CMD_RT
      .byte 1                    ; RIGHT alias
      .word str_ext_lt
      .byte MODULE_ID_NONE
      .byte EXT_CMD_LT
      .byte 1                    ; arity: 1 (degrees)
      .word str_ext_left
      .byte MODULE_ID_NONE
      .byte EXT_CMD_LT
      .byte 1                    ; LEFT alias
      .word str_ext_cs
      .byte MODULE_ID_NONE
      .byte EXT_CMD_CS
      .byte 0                    ; arity: 0
      .word str_ext_clearscreen
      .byte MODULE_ID_NONE
      .byte EXT_CMD_CS
      .byte 0                    ; CLEARSCREEN alias
      .word str_ext_draw
      .byte MODULE_ID_NONE
      .byte EXT_CMD_CS
      .byte 0                    ; DRAW = CS alias
      .word str_ext_pu
      .byte MODULE_ID_NONE
      .byte EXT_CMD_PU
      .byte 0
      .word str_ext_penup
      .byte MODULE_ID_NONE
      .byte EXT_CMD_PU
      .byte 0                    ; PENUP alias
      .word str_ext_pd
      .byte MODULE_ID_NONE
      .byte EXT_CMD_PD
      .byte 0
      .word str_ext_pendown
      .byte MODULE_ID_NONE
      .byte EXT_CMD_PD
      .byte 0                    ; PENDOWN alias
      .word str_ext_st
      .byte MODULE_ID_NONE
      .byte EXT_CMD_ST
      .byte 0
      .word str_ext_showturtle
      .byte MODULE_ID_NONE
      .byte EXT_CMD_ST
      .byte 0                    ; SHOWTURTLE alias
      .word str_ext_ht
      .byte MODULE_ID_NONE
      .byte EXT_CMD_HT
      .byte 0
      .word str_ext_hideturtle
      .byte MODULE_ID_NONE
      .byte EXT_CMD_HT
      .byte 0                    ; HIDETURTLE alias
      .word str_ext_home
      .byte MODULE_ID_NONE
      .byte EXT_CMD_HOME
      .byte 0
      ; --- Screen modes ---
      .word str_ext_textscreen
      .byte MODULE_ID_NONE
      .byte EXT_CMD_TS
      .byte 0
      .word str_ext_ts
      .byte MODULE_ID_NONE
      .byte EXT_CMD_TS
      .byte 0
      .word str_ext_splitscreen
      .byte MODULE_ID_NONE
      .byte EXT_CMD_SS
      .byte 0
      .word str_ext_ss
      .byte MODULE_ID_NONE
      .byte EXT_CMD_SS
      .byte 0
      .word str_ext_fullscreen
      .byte MODULE_ID_NONE
      .byte EXT_CMD_FS
      .byte 0
      .word str_ext_fs
      .byte MODULE_ID_NONE
      .byte EXT_CMD_FS
      .byte 0
      ; --- Turtle position ---
      .word str_ext_setxy
      .byte MODULE_ID_NONE
      .byte EXT_CMD_SETXY
      .byte 2                    ; arity: 2 (x, y)
      .word str_ext_setpos
      .byte MODULE_ID_NONE
      .byte EXT_CMD_SETPOS
      .byte 1                    ; arity: 1 ([x y])
      .word str_ext_setx
      .byte MODULE_ID_NONE
      .byte EXT_CMD_SETX
      .byte 1                    ; arity: 1 (x)
      .word str_ext_sety
      .byte MODULE_ID_NONE
      .byte EXT_CMD_SETY
      .byte 1                    ; arity: 1 (y)
      .word str_ext_setheading
      .byte MODULE_ID_NONE
      .byte EXT_CMD_SETH
      .byte 1                    ; arity: 1 (degrees)
      .word str_ext_seth
      .byte MODULE_ID_NONE
      .byte EXT_CMD_SETH
      .byte 1                    ; SETH alias
      ; --- Turtle query reporters ---
      .word str_ext_xcor
      .byte MODULE_ID_NONE
      .byte EXT_CMD_XCOR
      .byte 0                    ; arity: 0 (reporter)
      .word str_ext_ycor
      .byte MODULE_ID_NONE
      .byte EXT_CMD_YCOR
      .byte 0
      .word str_ext_heading
      .byte MODULE_ID_NONE
      .byte EXT_CMD_HEADING
      .byte 0
      .word str_ext_pendownp
      .byte MODULE_ID_NONE
      .byte EXT_CMD_PENDOWNP
      .byte 0
      .word str_ext_shownp
      .byte MODULE_ID_NONE
      .byte EXT_CMD_SHOWNP
      .byte 0
      ; --- Pen commands ---
      .word str_ext_setpc
      .byte MODULE_ID_NONE
      .byte EXT_CMD_SETPC
      .byte 1                    ; arity: 1 (color)
      .word str_ext_setpencolor
      .byte MODULE_ID_NONE
      .byte EXT_CMD_SETPC
      .byte 1                    ; SETPENCOLOR alias
      .word str_ext_setbg
      .byte MODULE_ID_NONE
      .byte EXT_CMD_SETBG
      .byte 1
      .word str_ext_setbackground
      .byte MODULE_ID_NONE
      .byte EXT_CMD_SETBG
      .byte 1                    ; SETBACKGROUND alias
      ; --- TOWARDS reporter ---
      .word str_ext_towards
      .byte MODULE_ID_NONE
      .byte EXT_CMD_TOWARDS
      .byte 2                    ; arity: 2 (x, y)
      ; --- VGC graphics commands ---
      .word str_ext_setcolor
      .byte MODULE_ID_NONE
      .byte EXT_CMD_SETCOLOR
      .byte 1                    ; arity: 1 (color)
      .word str_ext_plot
      .byte MODULE_ID_NONE
      .byte EXT_CMD_PLOT
      .byte 2                    ; arity: 2 (x, y)
      .word str_ext_unplot
      .byte MODULE_ID_NONE
      .byte EXT_CMD_UNPLOT
      .byte 2                    ; arity: 2 (x, y)
      .word str_ext_line
      .byte MODULE_ID_NONE
      .byte EXT_CMD_LINE
      .byte 4                    ; arity: 4 (x1, y1, x2, y2)
      .word str_ext_circle
      .byte MODULE_ID_NONE
      .byte EXT_CMD_CIRCLE
      .byte 3                    ; arity: 3 (x, y, r)
      .word str_ext_rect
      .byte MODULE_ID_NONE
      .byte EXT_CMD_RECT
      .byte 4                    ; arity: 4 (x1, y1, x2, y2)
      .word str_ext_rectangle
      .byte MODULE_ID_NONE
      .byte EXT_CMD_RECT
      .byte 4                    ; RECTANGLE alias
      .word str_ext_fill
      .byte MODULE_ID_NONE
      .byte EXT_CMD_FILLRECT
      .byte 4                    ; arity: 4 (x1, y1, x2, y2)
      .word str_ext_fillrect
      .byte MODULE_ID_NONE
      .byte EXT_CMD_FILLRECT
      .byte 4                    ; FILLRECT alias
      .word str_ext_paint
      .byte MODULE_ID_NONE
      .byte EXT_CMD_PAINT
      .byte 2                    ; arity: 2 (x, y)
      ; --- Sprite commands ---
      .word str_ext_sprite
      .byte MODULE_ID_NONE
      .byte EXT_CMD_SPRITE
      .byte 3                    ; arity: 3 (n, x, y)
      .word str_ext_spritepos
      .byte MODULE_ID_NONE
      .byte EXT_CMD_SPRITEPOS
      .byte 3                    ; arity: 3 (n, x, y)
      .word str_ext_spriteon
      .byte MODULE_ID_NONE
      .byte EXT_CMD_SPRITEON
      .byte 1                    ; arity: 1 (n)
      .word str_ext_spriteoff
      .byte MODULE_ID_NONE
      .byte EXT_CMD_SPRITEOFF
      .byte 1                    ; arity: 1 (n)
      .word str_ext_sprcollp
      .byte MODULE_ID_NONE
      .byte EXT_CMD_SPRCOLLP
      .byte 1                    ; arity: 1 (n) — reporter
      ; --- Sound commands ---
      .word str_ext_tone
      .byte MODULE_ID_NONE
      .byte EXT_CMD_TONE
      .byte 2                    ; arity: 2 (freq, dur)
      .word str_ext_noise
      .byte MODULE_ID_NONE
      .byte EXT_CMD_NOISE
      .byte 1                    ; arity: 1 (dur)
      .word str_ext_volume
      .byte MODULE_ID_NONE
      .byte EXT_CMD_VOLUME
      .byte 1                    ; arity: 1 (vol)
      ; --- Timing commands ---
      .word str_ext_wait
      .byte MODULE_ID_NONE
      .byte EXT_CMD_WAIT
      .byte 1                    ; arity: 1 (n)
      .word str_ext_waitvbl
      .byte MODULE_ID_NONE
      .byte EXT_CMD_WAITVBL
      .byte 0                    ; arity: 0
      .word str_ext_timer
      .byte MODULE_ID_NONE
      .byte EXT_CMD_TIMER
      .byte 0                    ; arity: 0 — reporter
      .word $0000               ; end sentinel

str_ext_test_name:
      .byte 8, "EXT.TEST"
str_ext_fd:
      .byte 2, "FD"
str_ext_forward:
      .byte 7, "FORWARD"
str_ext_bk:
      .byte 2, "BK"
str_ext_back:
      .byte 4, "BACK"
str_ext_backward:
      .byte 8, "BACKWARD"
str_ext_rt:
      .byte 2, "RT"
str_ext_right:
      .byte 5, "RIGHT"
str_ext_lt:
      .byte 2, "LT"
str_ext_left:
      .byte 4, "LEFT"
str_ext_cs:
      .byte 2, "CS"
str_ext_clearscreen:
      .byte 11, "CLEARSCREEN"
str_ext_draw:
      .byte 4, "DRAW"
str_ext_pu:
      .byte 2, "PU"
str_ext_penup:
      .byte 5, "PENUP"
str_ext_pd:
      .byte 2, "PD"
str_ext_pendown:
      .byte 7, "PENDOWN"
str_ext_st:
      .byte 2, "ST"
str_ext_showturtle:
      .byte 10, "SHOWTURTLE"
str_ext_ht:
      .byte 2, "HT"
str_ext_hideturtle:
      .byte 10, "HIDETURTLE"
str_ext_home:
      .byte 4, "HOME"
str_ext_textscreen:
      .byte 10, "TEXTSCREEN"
str_ext_ts:
      .byte 2, "TS"
str_ext_splitscreen:
      .byte 11, "SPLITSCREEN"
str_ext_ss:
      .byte 2, "SS"
str_ext_fullscreen:
      .byte 10, "FULLSCREEN"
str_ext_fs:
      .byte 2, "FS"
str_ext_setxy:
      .byte 5, "SETXY"
str_ext_setpos:
      .byte 6, "SETPOS"
str_ext_setx:
      .byte 4, "SETX"
str_ext_sety:
      .byte 4, "SETY"
str_ext_setheading:
      .byte 10, "SETHEADING"
str_ext_seth:
      .byte 4, "SETH"
str_ext_xcor:
      .byte 4, "XCOR"
str_ext_ycor:
      .byte 4, "YCOR"
str_ext_heading:
      .byte 7, "HEADING"
str_ext_pendownp:
      .byte 8, "PENDOWN?"
str_ext_shownp:
      .byte 6, "SHOWN?"
str_ext_setpc:
      .byte 5, "SETPC"
str_ext_setpencolor:
      .byte 11, "SETPENCOLOR"
str_ext_setbg:
      .byte 5, "SETBG"
str_ext_setbackground:
      .byte 13, "SETBACKGROUND"
str_ext_towards:
      .byte 7, "TOWARDS"
str_ext_setcolor:
      .byte 8, "SETCOLOR"
str_ext_plot:
      .byte 4, "PLOT"
str_ext_unplot:
      .byte 6, "UNPLOT"
str_ext_line:
      .byte 4, "LINE"
str_ext_circle:
      .byte 6, "CIRCLE"
str_ext_rect:
      .byte 4, "RECT"
str_ext_rectangle:
      .byte 9, "RECTANGLE"
str_ext_fill:
      .byte 4, "FILL"
str_ext_fillrect:
      .byte 8, "FILLRECT"
str_ext_paint:
      .byte 5, "PAINT"
str_ext_sprite:
      .byte 6, "SPRITE"
str_ext_spritepos:
      .byte 9, "SPRITEPOS"
str_ext_spriteon:
      .byte 8, "SPRITEON"
str_ext_spriteoff:
      .byte 9, "SPRITEOFF"
str_ext_sprcollp:
      .byte 16, "SPRITECOLLISION?"
str_ext_tone:
      .byte 4, "TONE"
str_ext_noise:
      .byte 5, "NOISE"
str_ext_volume:
      .byte 6, "VOLUME"
str_ext_wait:
      .byte 4, "WAIT"
str_ext_waitvbl:
      .byte 7, "WAITVBL"
str_ext_timer:
      .byte 5, "TIMER"

      .segment "RODATA"

str_idk:
      .byte "I DON'T KNOW HOW TO ", 0

str_notenough:
      .byte "NOT ENOUGH INPUTS", 0

; Shared error-message fragments (4a composer). Each stored exactly once;
; composed with a command's existing *_name string by the err_* helpers.
frag_to:
      .byte " TO ", 0
frag_dl:
      .byte " DOESN'T LIKE ", 0
frag_as_input:
      .byte "[] AS INPUT", 0
frag_this_input:
      .byte "THIS INPUT", 0
frag_needs_list:
      .byte " NEEDS A LIST", 0
frag_index_range:
      .byte " INDEX OUT OF RANGE", 0

str_error_tag:
      .byte 5, "ERROR"

; Powers of 10 table (16-bit): 10000, 1000, 100, 10, 1
pow10_lo:
      .byte <10000, <1000, <100, <10, <1
pow10_hi:
      .byte >10000, >1000, >100, >10, >1

; =====================================================================
;  NovaLogo — eval.s
;
;  Evaluator: walks the parsed token list and dispatches Logo commands,
;  resolving numbers, words, and procedure calls.
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

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
lat_was_visible: .res 1   ; logo_adapt_turtle: pre-call TURTLE_GFX_VISIBLE snapshot

; =====================================================================
; CODE segment — evaluator routines
; =====================================================================
      .segment "CODE"

; Point ptr at the current token / the last evaluated value. Thirty-five call
; sites paid twelve bytes each to inline these; A and the flags still come back
; holding the high byte, exactly as the inline sequence left them.
logo_ptr_from_eval_cur:
      LDA   eval_cur_lo
      STA   ptr_lo
      LDA   eval_cur_hi
      STA   ptr_hi
      RTS

logo_ptr_from_proc_entry:
      LDA   proc_entry_lo
      STA   ptr_lo
      LDA   proc_entry_hi
      STA   ptr_hi
      RTS

logo_ptr_from_list_ptr:
      LDA   list_ptr_lo
      STA   ptr_lo
      LDA   list_ptr_hi
      STA   ptr_hi
      RTS

logo_ptr_from_gc_ptr:
      LDA   gc_ptr_lo
      STA   ptr_lo
      LDA   gc_ptr_hi
      STA   ptr_hi
      RTS

logo_ptr_from_eval_val:
      LDA   eval_val_lo
      STA   ptr_lo
      LDA   eval_val_hi
      STA   ptr_hi
      RTS


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
      JSR   logo_ptr_from_eval_cur
      LDY   #TOK_TAG
      LDA   (ptr_lo),Y

      ; Dispatch based on tag
      CMP   #TOK_WORD
      BEQ   dispatch_command

      ; Bare expression — skip it for now (could error)
      ; Advance to next token
      JSR   eval_advance
      BRA   eval_loop

@bail_out:
      RTS

; ---------------------------------------------------------------------
; dispatch_command — shared command-word dispatch for eval_loop and eval_body.
;   On a builtin: evaluate args then JMP (handler); the handler ends at
;   eval_continue, which routes back to whichever loop is active (eval_in_body
;   decides). On an unknown word: try user procedures, then the extension table.
;   Reached by branch from eval_loop and by JMP from eval_body.
; ---------------------------------------------------------------------
dispatch_command:
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
      JMP   eval_continue        ; routes back to eval_loop or eval_body

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
      ; Dispatch: legacy trampoline or paged-library lib_call (branches on
      ; ext_mod_id). Result left in eval_val.
      JSR   ext_invoke
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
      JSR   logo_ptr_from_eval_cur
      LDY   #TOK_TAG
      LDA   (ptr_lo),Y

      ; Hit the closing bracket? We're done with this body pass.
      CMP   #TOK_RBRACKET
      BEQ   @done

      ; Command words go through the shared dispatch_command (used by eval_loop
      ; too). Builtins + proc_invoke + ext commands all finish at eval_continue,
      ; which returns here automatically while eval_in_body is set — so no
      ; body-specific dispatch copy is needed.
      CMP   #TOK_WORD
      BNE   @skip
      JMP   dispatch_command
@skip:
      ; Skip non-word tokens (bare numbers, brackets inside expressions, etc.)
      JSR   eval_advance
      BRA   eval_body

@done:
      RTS

; ---------------------------------------------------------------------
; eval_advance — advance eval_cur to the next token in the list
;   Reads next pointer from current token, stores in eval_cur
; ---------------------------------------------------------------------
eval_advance:
      JSR   logo_ptr_from_eval_cur
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
      JSR   logo_ptr_from_eval_cur
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
      ; Dispatch: legacy trampoline or paged-library lib_call (branches on
      ; ext_mod_id). Result left in eval_val.
      JSR   ext_invoke
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
      JSR   logo_ptr_from_eval_cur
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
      JSR   logo_ptr_from_eval_cur
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
      JSR   logo_ptr_from_eval_cur
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
; Thin wrapper: render the number into input_buf with the shared
; render_number_to_buf (identical signed integer + .frac formatting), then copy
; it to the screen. Numbers are short (<= ~9 chars) so input_buf never truncates.
print_number:
      STZ   z:buf_idx
      JSR   render_number_to_buf
      LDX   #0
@pn:
      CPX   z:buf_idx
      BCS   @pn_done
      LDA   input_buf,X
      STA   VGC_CHAROUT
      INX
      BRA   @pn
@pn_done:
      RTS

; print_uint16 — print eval_val_hi:eval_val_lo as an UNSIGNED decimal (banner
; free-bytes line, where the value can exceed 32767). Reuses the unsigned entry
; into render_number_to_buf so no signed '-' is emitted.
print_uint16:
      STZ   z:buf_idx
      JSR   render_uint16_to_buf
      LDX   #0
@pu:
      CPX   z:buf_idx
      BCS   @pu_done
      LDA   input_buf,X
      STA   VGC_CHAROUT
      INX
      BRA   @pu
@pu_done:
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
      LDA   num_tmp_lo
      STA   ptr2_lo
      LDA   num_tmp_hi
      STA   ptr2_hi
      LDY   #0
      LDA   (ptr2_lo),Y        ; inline name length, 0 = end
      BNE   @have_entry
      SEC
      RTS

@have_entry:
      ; Get token payload pointer
      TAX                       ; X = length
      JSR   logo_ptr_from_eval_cur

      ; Compare length bytes
      LDY   #0
      LDA   (ptr2_lo),Y        ; table name length
      LDY   #TOK_PAYLOAD
      CMP   (ptr_lo),Y         ; token payload length
      BNE   @next

      ; Lengths match — compare chars
      LDY   #0
@cmp_loop:
      INY                       ; table name char offset (1..length)
      LDA   (ptr2_lo),Y
      PHY
      TYA
      CLC
      ADC   #TOK_PAYLOAD
      TAY
      LDA   (ptr_lo),Y         ; token char
      PLY
      CMP   (ptr2_lo),Y        ; compare
      BNE   @next
      DEX
      BNE   @cmp_loop

      ; Match found — payload follows the length-prefixed inline name.
      INY
      LDA   (ptr2_lo),Y        ; module_id
      STA   ext_mod_id
      INY
      LDA   (ptr2_lo),Y        ; fn_id (legacy EXT_CMD value)
      STA   EXT_CMD
      INY
      LDA   (ptr2_lo),Y        ; arity
      CLC                       ; found
      RTS

@next:
      ; Advance by length byte + name + module_id + fn_id + arity.
      LDY   #0
      LDA   (ptr2_lo),Y
      CLC
      ADC   #4
      ADC   num_tmp_lo
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

; --- Logo-foundation adapter sentinel (4c.1-3) ---
; A pseudo-module-id used in ext_cmd_table for Logo commands that do NOT 1:1-map
; to a single GFN. ext_invoke recognizes it and runs a local adapter routine that
; issues one-or-more real lib_call(GRAPHICS) sequences. $FE is distinct from real
; module ids ($01-$7F), from MODULE_ID_NONE ($00) and from LIB_RESIDENT_HOSTEXT
; ($FF). EXT_CMD holds the adapter index for these entries.
MODULE_ID_GFXADAPTER = $FE
GADAPT_SPRITE        = $00        ; SPRITE n x y  -> GFN_SPR_POS then GFN_SPR_ENABLE
GADAPT_SPRCOLLP      = $01        ; SPRITECOLLISION? n -> GFN_SPR_COLL_MASK, test bit n

; --- Logo turtle adapter ROUTING SENTINEL (4c.2-3-ii) ---
; A foundation-local pseudo-module-id (an ext_cmd_table marker) for the 26 turtle
; commands+reporters, whose whole engine now lives in the dedicated TURTLE module
; as TUR_OP. ext_invoke recognizes this marker BEFORE the s16 @convert_args (which
; drops the fraction byte) and runs logo_adapt_turtle, a FRAC-preserving marshal
; feeding TUR_OP. The op rides in LIB_ARG2 byte0 = the EXT_CMD value. $FD is
; distinct from real module ids ($01-$7F), MODULE_ID_NONE ($00),
; MODULE_ID_GFXADAPTER ($FE) and HOSTEXT ($FF). It is a Logo-internal routing
; marker only — NOT the real libabi MODULE_ID_TURTLE ($07), which is what the
; adapter actually lib_calls.
TURTLE_ROUTE_MARK    = $FD

; Turtle screen-mode geometry — the FOUNDATION owns the split text window (the
; module's turtle_enter_split deviation note: it enables gfx mode but leaves the
; runtime-owned TEXTWIN/cursor split to the adapter). Mirrors extension.s.
TURTLE_GFX_VISIBLE   = $9F11      ; module-set flag: 1 = split/full gfx active
TADP_SPLIT_TEXT_ROW  = 40         ; 160px gfx band / 4px text cell = row 40
TADP_SPLIT_TEXT_HEIGHT = 10       ; rows 40..49 = the bottom text band

; ext_invoke — dispatch the looked-up extension command and leave its
;   result in eval_type/eval_val_hi/eval_val_lo/eval_val_frac, then RTS.
;
;   Entry: args already evaluated (EXT_ARG0..3 + EXT_ARGC populated),
;          EXT_CMD = fn_id, ext_mod_id = module_id.
;
;   Branches on ext_mod_id:
;     TURTLE_ROUTE_MARK    -> Logo turtle adapter (FRAC-preserving marshal).
;     MODULE_ID_GFXADAPTER -> Logo-foundation adapter (multi-call composite).
;     real module id       -> canonical paged-library lib_call mailbox.
;
;   The arg cells are converted to LIB_ARGn FIRST (so both the single-lib_call
;   path and the adapters share the same converted cells), then we branch.
;   (The legacy MODULE_ID_NONE RAM-trampoline path was removed in Phase B when the
;   NovaLogo extension ROM was deleted — every command now routes to a module.)
;
;   Clobbers: A, X, Y. May JMP err_idk_word on a lib error (no return).
; ---------------------------------------------------------------------
ext_invoke:
      LDA   ext_mod_id
      CMP   #TURTLE_ROUTE_MARK
      BNE   @convert_args         ; real module id -> paged-library / adapter path
      JMP   logo_adapt_turtle     ; turtle: FRAC-preserving marshal (SKIP s16 @convert_args)

; --- lib_call / adapter path: convert args, then branch on module id ---
@convert_args:
      ; Convert EXT_ARGn (Logo 16.8: TYPE/HI/LO/FRAC) -> LIB_ARGn (32-bit LE).
      ; The GRAPHICS module reads each cell's low word as a signed s16, so map
      ; HI:LO into the cell low word, sign-extend into the high word, and DROP
      ; the fractional byte.
      ;   LIB_ARGn+0 = EXT_ARGn_LO   LIB_ARGn+1 = EXT_ARGn_HI
      ;   LIB_ARGn+2 = LIB_ARGn+3 = $FF if EXT_ARGn_HI bit7 set else $00
      ;
      ; FIRST zero all 16 LIB_ARG bytes so any cell n >= EXT_ARGC reads as 0.
      ; This is what makes CIRCLE (arity 3) leave ARG3(ry)=0 -> ry=rx (a circle),
      ; and is harmless for every other command.
      LDX   #15
@zero_args:
      STZ   LIB_ARG0,X
      DEX
      BPL   @zero_args

      LDX   EXT_ARGC
      ; arg 0
      CPX   #1
      BCC   @args_done
      LDA   EXT_ARG0_LO
      STA   LIB_ARG0+0
      LDA   EXT_ARG0_HI
      STA   LIB_ARG0+1            ; STA does not alter A; A still = HI byte
      JSR   @sign_of_a            ; A = $FF if HI bit7 set, else $00
      STA   LIB_ARG0+2
      STA   LIB_ARG0+3
      ; arg 1
      CPX   #2
      BCC   @args_done
      LDA   EXT_ARG1_LO
      STA   LIB_ARG1+0
      LDA   EXT_ARG1_HI
      STA   LIB_ARG1+1
      JSR   @sign_of_a
      STA   LIB_ARG1+2
      STA   LIB_ARG1+3
      ; arg 2
      CPX   #3
      BCC   @args_done
      LDA   EXT_ARG2_LO
      STA   LIB_ARG2+0
      LDA   EXT_ARG2_HI
      STA   LIB_ARG2+1
      JSR   @sign_of_a
      STA   LIB_ARG2+2
      STA   LIB_ARG2+3
      ; arg 3
      CPX   #4
      BCC   @args_done
      LDA   EXT_ARG3_LO
      STA   LIB_ARG3+0
      LDA   EXT_ARG3_HI
      STA   LIB_ARG3+1
      JSR   @sign_of_a
      STA   LIB_ARG3+2
      STA   LIB_ARG3+3

@args_done:
      ; Branch: adapter (multi-call composite) vs. plain single lib_call.
      LDA   ext_mod_id
      CMP   #MODULE_ID_GFXADAPTER
      BEQ   @adapter

; --- plain single lib_call: drive the canonical mailbox at $0300 ---
@lib_call:
      ; Populate selectors and call the resident loader via the shared helper.
      LDA   ext_mod_id
      LDX   EXT_CMD
      JSR   do_lib_call
      BNE   @lib_err

      ; Copy LIB_RESULT low word into eval_val as a Logo integer.
      LDA   LIB_RESULT+0
      STA   eval_val_lo
      LDA   LIB_RESULT+1
      STA   eval_val_hi
      STZ   eval_val_frac
      STZ   eval_type
      RTS

@lib_err:
      JMP   err_idk_word

; --- adapter dispatch: EXT_CMD holds the adapter index ---
@adapter:
      LDA   EXT_CMD
      CMP   #GADAPT_SPRITE
      BEQ   logo_adapt_sprite
      CMP   #GADAPT_SPRCOLLP
      BEQ   logo_adapt_sprcollp
      ; Unknown adapter index — treat as unknown word.
      JMP   err_idk_word

; @sign_of_a — sign-extend the s8 high byte in A to a full byte:
;   returns A = $FF if A bit7 was set, else A = $00. Preserves X/Y.
@sign_of_a:
      AND   #$80                  ; isolate bit7
      BEQ   @sign_zero
      LDA   #$FF
      RTS
@sign_zero:
      LDA   #$00
      RTS

; ---------------------------------------------------------------------
; logo_adapt_sprite — SPRITE n x y: position the sprite then enable it.
;   Args (n,x,y) are already in LIB_ARG0/1/2. GFN_SPR_POS reads ARG0/1/2 and
;   GFN_SPR_ENABLE reads ARG0, so the SAME converted cells serve both calls.
;   Command (no value): leaves eval_type=VAL_NUMBER, eval_val=0.
;   Clobbers A, X, Y. May JMP err_idk_word on a lib error (no return).
; ---------------------------------------------------------------------
; ---------------------------------------------------------------------
; do_lib_call — issue a resident lib_call. A = module id, X = function id.
;   STAs the selectors, calls the resident loader band, returns A = LIB_STATUS
;   (Z set when OK). The band call clobbers A/X/Y; callers BNE to their own error.
; ---------------------------------------------------------------------
do_lib_call:
      STA   LIB_MOD_ID
      STX   LIB_FN_ID
      JSR   LIB_LOADER_BAND
      LDA   LIB_STATUS
      RTS

; ---------------------------------------------------------------------
; fill_cons_from_eval — fill a freshly-allocated cons cell (ptr_lo/hi) with
;   TAG = CONS_PAIR, CAR = eval_type/eval_val_hi/lo/frac, CDR = nil. Clobbers A/Y
;   (callers already treat A/Y as dead after the fill). The alloc + OOM handling
;   stays at each call site (they differ in stack depth / error target).
; ---------------------------------------------------------------------
fill_cons_from_eval:
      LDY   #CONS_TAG
      LDA   #CONS_PAIR
      STA   (ptr_lo),Y
      LDY   #CONS_CAR_TYPE
      LDA   eval_type
      STA   (ptr_lo),Y
      LDY   #CONS_CAR_HI
      LDA   eval_val_hi
      STA   (ptr_lo),Y
      LDY   #CONS_CAR_LO
      LDA   eval_val_lo
      STA   (ptr_lo),Y
      LDY   #CONS_CAR_FRAC
      LDA   eval_val_frac
      STA   (ptr_lo),Y
      LDY   #CONS_CDR_LO
      LDA   #0
      STA   (ptr_lo),Y
      LDY   #CONS_CDR_HI
      STA   (ptr_lo),Y
      RTS

logo_adapt_sprite:
      LDA   #MODULE_ID_GRAPHICS
      LDX   #GFN_SPR_POS
      JSR   do_lib_call
      BNE   @adapt_err
      ; re-arm selectors and enable
      LDA   #MODULE_ID_GRAPHICS
      LDX   #GFN_SPR_ENABLE
      JSR   do_lib_call
      BNE   @adapt_err
      STZ   eval_val_lo
      STZ   eval_val_hi
      STZ   eval_val_frac
      STZ   eval_type
      RTS
@adapt_err:
      JMP   err_idk_word

; ---------------------------------------------------------------------
; logo_adapt_sprcollp — SPRITECOLLISION? n: report whether sprite n collided.
;   lib_call GFN_SPR_COLL_MASK -> LIB_RESULT low word = 16-bit collision mask
;   (byte0 = sprites 0-7, byte1 = sprites 8-15). Test bit n (n = EXT_ARG0_LO,
;   the converted ARG0 low byte) and set eval_val = 0/1 bool.
;   Reporter: leaves eval_type=VAL_NUMBER, eval_val = the bool.
;   Clobbers A, X, Y. May JMP err_idk_word on a lib error (no return).
; ---------------------------------------------------------------------
logo_adapt_sprcollp:
      LDA   #MODULE_ID_GRAPHICS
      LDX   #GFN_SPR_COLL_MASK
      JSR   do_lib_call
      BNE   @adapt_err
      ; Select the mask byte holding sprite n, and the bit index within it.
      LDA   EXT_ARG0_LO           ; sprite index n
      CMP   #8
      BCS   @hi_byte
      ; sprites 0-7: low byte of mask = LIB_RESULT+0
      TAX
      LDA   LIB_RESULT+0
      BRA   @test_bit
@hi_byte:
      ; sprites 8-15: high byte of mask = LIB_RESULT+1, bit index n-8
      SEC
      SBC   #8
      TAX
      LDA   LIB_RESULT+1
@test_bit:
      CPX   #0
      BEQ   @check
@shift:
      LSR
      DEX
      BNE   @shift
@check:
      AND   #$01
      STA   eval_val_lo
      STZ   eval_val_hi
      STZ   eval_val_frac
      STZ   eval_type
      RTS
@adapt_err:
      JMP   err_idk_word

; ---------------------------------------------------------------------
; logo_adapt_turtle — route a Logo turtle command/reporter to the TURTLE
;   module's TUR_OP ($07/$03). The whole turtle engine (move-math, render,
;   reporters) lives in the module since step i; this thin adapter only marshals
;   the foundation's evaluated args into the lib_call mailbox and copies the
;   result back.
;
;   Entry: EXT_CMD = the turtle op id ($10..$29), EXT_ARGC + EXT_ARGn populated.
;          Reached DIRECTLY from ext_invoke (it SKIPS @convert_args, whose s16
;          conversion DROPS the fraction byte — fatal for the turtle's sub-pixel
;          accumulation, e.g. FD 1 must accumulate, not snap).
;
;   Arg marshalling — the module reads value args as Logo 16.8 (cell byte0=FRAC,
;   byte1=LO, byte2=HI), so copy EXT_ARGn FRAC/LO/HI -> LIB_ARGn +0/+1/+2:
;       LIB_ARGn+0 = EXT_ARGn_FRAC   LIB_ARGn+1 = EXT_ARGn_LO
;       LIB_ARGn+2 = EXT_ARGn_HI     LIB_ARGn+3 = 0
;   This preserves the fraction the move handlers (t_fd/t_bk read TA0_FRAC/LO/HI)
;   need. EXCEPTION: SETPC/SETBG (color) and SETPOS (cons-list ptr) read the value
;   from LIB_ARG0 byte0/byte1 directly (t_setpc/t_setbg: LIB_ARG0+0 = color;
;   t_setpos: LIB_ARG0+0/+1 = ptr lo/hi). For those three ops, overwrite ARG0
;   byte0/byte1 with EXT_ARG0_LO/EXT_ARG0_HI (the unshifted low word). These ops
;   carry no fraction, so clobbering byte0(FRAC) is harmless.
;
;   Op id rides in LIB_ARG2 byte0 = EXT_CMD. Result (reporters): LIB_RESULT
;   FRAC:LO:HI -> eval_val_frac/lo/hi, type VAL_NUMBER. Commands set LIB_RESULT 0
;   -> eval_val 0 (matches logo_adapt_sprite's command convention).
;   Clobbers A, X, Y. May JMP err_idk_word on a lib error (no return).
; ---------------------------------------------------------------------
logo_adapt_turtle:
      ; Zero all 16 LIB_ARG bytes so any unused cell reads 0.
      LDX   #15
@zero:
      STZ   LIB_ARG0,X
      DEX
      BPL   @zero

      ; FRAC-preserving marshal of ARG0/ARG1 based on EXT_ARGC.
      LDX   EXT_ARGC
      CPX   #1
      BCC   @op                   ; argc 0 -> no value args
      LDA   EXT_ARG0_FRAC
      STA   LIB_ARG0+0
      LDA   EXT_ARG0_LO
      STA   LIB_ARG0+1
      LDA   EXT_ARG0_HI
      STA   LIB_ARG0+2
      CPX   #2
      BCC   @op                   ; argc 1 -> only ARG0
      LDA   EXT_ARG1_FRAC
      STA   LIB_ARG1+0
      LDA   EXT_ARG1_LO
      STA   LIB_ARG1+1
      LDA   EXT_ARG1_HI
      STA   LIB_ARG1+2

@op:
      ; Op id -> ARG2 byte0.
      LDA   EXT_CMD
      STA   LIB_ARG2+0

      ; SETPOS [x y] is a Logo-list op the module does NOT understand (the module
      ; is 100% numeric). Unpack the [x y] cons list HERE with Logo's own cons
      ; accessors and drive the module's generic SETXY op instead (x->ARG0,
      ; y->ARG1 as Logo 16.8). See @setpos below.
      CMP   #EXT_CMD_SETPOS
      BEQ   @setpos

      ; SETPC/SETBG read ARG0 as an UNshifted low word (byte0=lo,byte1=hi);
      ; rewrite ARG0 byte0/byte1 with EXT_ARG0_LO/HI for those two ops.
      CMP   #EXT_CMD_SETPC
      BEQ   @arg0_unshifted
      CMP   #EXT_CMD_SETBG
      BNE   @call
@arg0_unshifted:
      LDA   EXT_ARG0_LO
      STA   LIB_ARG0+0
      LDA   EXT_ARG0_HI
      STA   LIB_ARG0+1
      BRA   @call

      ; --- SETPOS [x y]: unpack the Logo cons list into ARG0=x, ARG1=y (16.8) and
      ;     rewrite the op to the module's generic SETXY ($1D). The module never
      ;     sees a Logo data type. EXT_ARG0_LO/HI = the list head cons-pair ptr. ---
@setpos:
      ; ptr_lo:ptr_hi = list head; nil/empty list -> "DOESN'T LIKE THIS INPUT".
      LDA   EXT_ARG0_LO
      STA   ptr_lo
      LDA   EXT_ARG0_HI
      STA   ptr_hi
      ORA   ptr_lo
      BEQ   @setpos_bad           ; nil list (no X)
      ; first element -> ARG0 (must be a number)
      LDY   #CONS_CAR_TYPE
      LDA   (ptr_lo),Y
      CMP   #VAL_NUMBER
      BNE   @setpos_bad
      LDY   #CONS_CAR_FRAC
      LDA   (ptr_lo),Y
      STA   LIB_ARG0+0            ; X frac
      LDY   #CONS_CAR_LO
      LDA   (ptr_lo),Y
      STA   LIB_ARG0+1            ; X lo
      LDY   #CONS_CAR_HI
      LDA   (ptr_lo),Y
      STA   LIB_ARG0+2            ; X hi
      ; cdr -> second cell
      LDY   #CONS_CDR_LO
      LDA   (ptr_lo),Y
      TAX                         ; cdr lo
      LDY   #CONS_CDR_HI
      LDA   (ptr_lo),Y            ; cdr hi
      STA   ptr_hi
      STX   ptr_lo
      ORA   ptr_lo
      BEQ   @setpos_bad           ; no second element (no Y)
      ; second element -> ARG1 (must be a number)
      LDY   #CONS_CAR_TYPE
      LDA   (ptr_lo),Y
      CMP   #VAL_NUMBER
      BNE   @setpos_bad
      LDY   #CONS_CAR_FRAC
      LDA   (ptr_lo),Y
      STA   LIB_ARG1+0            ; Y frac
      LDY   #CONS_CAR_LO
      LDA   (ptr_lo),Y
      STA   LIB_ARG1+1            ; Y lo
      LDY   #CONS_CAR_HI
      LDA   (ptr_lo),Y
      STA   LIB_ARG1+2            ; Y hi
      ; drive the module's generic SETXY op instead of SETPOS
      LDA   #EXT_CMD_SETXY
      STA   LIB_ARG2+0
      BRA   @call
@setpos_bad:
      JMP   err_dl_this           ; "<name> DOESN'T LIKE THIS INPUT" (Logo bad-list path)

@call:
      ; Snapshot the module's gfx-visible flag so we can detect a text->split
      ; transition the module makes inside ensure_gfx_mode (it enables gfx mode
      ; but leaves the foundation-owned text-window split to us).
      LDA   TURTLE_GFX_VISIBLE
      STA   lat_was_visible

      LDA   #MODULE_ID_TURTLE     ; real libabi id $07 (TURTLE module, demand-loaded)
      STA   LIB_MOD_ID
      LDA   #TUR_OP
      STA   LIB_FN_ID
      JSR   LIB_LOADER_BAND
      LDA   LIB_STATUS
      BNE   @turtle_err

      JSR   logo_turtle_textwin   ; foundation-side split text-window upkeep

      ; Result: LIB_RESULT FRAC:LO:HI -> eval_val (matches reporter layout;
      ; commands leave RESULT 0 -> eval_val 0).
      LDA   LIB_RESULT+0
      STA   eval_val_frac
      LDA   LIB_RESULT+1
      STA   eval_val_lo
      LDA   LIB_RESULT+2
      STA   eval_val_hi
      STZ   eval_type
      RTS
@turtle_err:
      JMP   err_idk_word

; ---------------------------------------------------------------------
; logo_turtle_textwin — keep Logo's prompt in the visible split text band when a
;   turtle op enters (or is) split mode. The GRAPHICS module enables gfx display
;   but does NOT carve the runtime's text window / park the cursor (its
;   turtle_enter_split deviation note hands that to the foundation). This mirrors
;   the legacy ext_cs/ext_ss/ensure_gfx_mode->prepare_split_text behavior:
;     - CS or SS op           -> always (re)apply the split text window
;     - any other turtle op    -> apply it only on a text->split transition
;       (module set TURTLE_GFX_VISIBLE 0->1 inside ensure_gfx_mode)
;   TS/FS and the reporters never need it (TS/FS leave the text window as legacy
;   ext_ts/ext_fs did; reporters never enter gfx mode).
;   Entry: EXT_CMD = op, lat_was_visible = pre-call TURTLE_GFX_VISIBLE.
;   Clobbers A.
; ---------------------------------------------------------------------
logo_turtle_textwin:
      LDA   EXT_CMD
      CMP   #EXT_CMD_CS
      BEQ   @apply               ; CS always re-applies (legacy ext_cs)
      CMP   #EXT_CMD_SS
      BEQ   @apply               ; SS always re-applies (legacy ext_ss)
      ; otherwise only on a 0->1 transition into split
      LDA   TURTLE_GFX_VISIBLE
      BEQ   @done                ; still not in gfx mode -> nothing to do
      LDA   lat_was_visible
      BNE   @done                ; already was visible -> no transition
@apply:
      ; prepare_split_text: form-feed clear, then text window = bottom band,
      ; cursor parked at the band start (cursor disabled while a command runs;
      ; the REPL re-enables it at the prompt).
      LDA   #$0C
      STA   VGC_CHAROUT          ; form-feed: clear text screen
@ff_wait:
      ; Inlined VGC busy-poll (mirrors legacy prepare_split_text's JSR wait_vgc).
      ; We cannot JSR vgc_wait_cmd here: that helper is extension-ROM code, not
      ; linked into this foundation bank, so it is unreachable from foundation code.
      LDA   VGC_CMD
      AND   #$01
      BNE   @ff_wait
      STZ   TEXTWIN_LEFT
      LDA   #TADP_SPLIT_TEXT_ROW
      STA   TEXTWIN_TOP
      LDA   #80
      STA   TEXTWIN_WIDTH
      LDA   #TADP_SPLIT_TEXT_HEIGHT
      STA   TEXTWIN_HEIGHT
      STZ   VGC_CURSX
      LDA   #TADP_SPLIT_TEXT_ROW
      STA   VGC_CURSY
      STZ   VGC_CURSEN
@done:
      RTS

      .segment "RODATA"

; Extension command table: len + name + module_id(1) + fn_id(1) + arity(1)
; module_id routes each command: a real module id ($01-$7F) -> lib_call(module);
; TURTLE_ROUTE_MARK/GFXADAPTER -> foundation adapters. Terminated by $0000 sentinel.
.macro EXT_ENTRY label, len, text, module, fn, arity
label:
      .byte len, text
      .byte module, fn, arity
.endmacro

ext_cmd_table:
      ; --- Turtle commands+reporters (4c.2-3-ii: routed to TUR_OP via the
      ;     TURTLE_ROUTE_MARK adapter; fn_id stays the EXT_CMD op id, arity unchanged) ---
      EXT_ENTRY str_ext_fd, 2, "FD", TURTLE_ROUTE_MARK, EXT_CMD_FD, 1
      EXT_ENTRY str_ext_forward, 7, "FORWARD", TURTLE_ROUTE_MARK, EXT_CMD_FD, 1
      EXT_ENTRY str_ext_bk, 2, "BK", TURTLE_ROUTE_MARK, EXT_CMD_BK, 1
      EXT_ENTRY str_ext_back, 4, "BACK", TURTLE_ROUTE_MARK, EXT_CMD_BK, 1
      EXT_ENTRY str_ext_backward, 8, "BACKWARD", TURTLE_ROUTE_MARK, EXT_CMD_BK, 1
      EXT_ENTRY str_ext_rt, 2, "RT", TURTLE_ROUTE_MARK, EXT_CMD_RT, 1
      EXT_ENTRY str_ext_right, 5, "RIGHT", TURTLE_ROUTE_MARK, EXT_CMD_RT, 1
      EXT_ENTRY str_ext_lt, 2, "LT", TURTLE_ROUTE_MARK, EXT_CMD_LT, 1
      EXT_ENTRY str_ext_left, 4, "LEFT", TURTLE_ROUTE_MARK, EXT_CMD_LT, 1
      EXT_ENTRY str_ext_cs, 2, "CS", TURTLE_ROUTE_MARK, EXT_CMD_CS, 0
      EXT_ENTRY str_ext_clearscreen, 11, "CLEARSCREEN", TURTLE_ROUTE_MARK, EXT_CMD_CS, 0
      EXT_ENTRY str_ext_draw, 4, "DRAW", TURTLE_ROUTE_MARK, EXT_CMD_CS, 0
      EXT_ENTRY str_ext_pu, 2, "PU", TURTLE_ROUTE_MARK, EXT_CMD_PU, 0
      EXT_ENTRY str_ext_penup, 5, "PENUP", TURTLE_ROUTE_MARK, EXT_CMD_PU, 0
      EXT_ENTRY str_ext_pd, 2, "PD", TURTLE_ROUTE_MARK, EXT_CMD_PD, 0
      EXT_ENTRY str_ext_pendown, 7, "PENDOWN", TURTLE_ROUTE_MARK, EXT_CMD_PD, 0
      EXT_ENTRY str_ext_st, 2, "ST", TURTLE_ROUTE_MARK, EXT_CMD_ST, 0
      EXT_ENTRY str_ext_showturtle, 10, "SHOWTURTLE", TURTLE_ROUTE_MARK, EXT_CMD_ST, 0
      EXT_ENTRY str_ext_ht, 2, "HT", TURTLE_ROUTE_MARK, EXT_CMD_HT, 0
      EXT_ENTRY str_ext_hideturtle, 10, "HIDETURTLE", TURTLE_ROUTE_MARK, EXT_CMD_HT, 0
      EXT_ENTRY str_ext_home, 4, "HOME", TURTLE_ROUTE_MARK, EXT_CMD_HOME, 0
      ; --- Screen modes ---
      EXT_ENTRY str_ext_textscreen, 10, "TEXTSCREEN", TURTLE_ROUTE_MARK, EXT_CMD_TS, 0
      EXT_ENTRY str_ext_ts, 2, "TS", TURTLE_ROUTE_MARK, EXT_CMD_TS, 0
      EXT_ENTRY str_ext_splitscreen, 11, "SPLITSCREEN", TURTLE_ROUTE_MARK, EXT_CMD_SS, 0
      EXT_ENTRY str_ext_ss, 2, "SS", TURTLE_ROUTE_MARK, EXT_CMD_SS, 0
      EXT_ENTRY str_ext_fullscreen, 10, "FULLSCREEN", TURTLE_ROUTE_MARK, EXT_CMD_FS, 0
      EXT_ENTRY str_ext_fs, 2, "FS", TURTLE_ROUTE_MARK, EXT_CMD_FS, 0
      ; --- Turtle position ---
      EXT_ENTRY str_ext_setxy, 5, "SETXY", TURTLE_ROUTE_MARK, EXT_CMD_SETXY, 2
      EXT_ENTRY str_ext_setpos, 6, "SETPOS", TURTLE_ROUTE_MARK, EXT_CMD_SETPOS, 1
      EXT_ENTRY str_ext_setx, 4, "SETX", TURTLE_ROUTE_MARK, EXT_CMD_SETX, 1
      EXT_ENTRY str_ext_sety, 4, "SETY", TURTLE_ROUTE_MARK, EXT_CMD_SETY, 1
      EXT_ENTRY str_ext_setheading, 10, "SETHEADING", TURTLE_ROUTE_MARK, EXT_CMD_SETH, 1
      EXT_ENTRY str_ext_seth, 4, "SETH", TURTLE_ROUTE_MARK, EXT_CMD_SETH, 1
      ; --- Turtle query reporters ---
      EXT_ENTRY str_ext_xcor, 4, "XCOR", TURTLE_ROUTE_MARK, EXT_CMD_XCOR, 0
      EXT_ENTRY str_ext_ycor, 4, "YCOR", TURTLE_ROUTE_MARK, EXT_CMD_YCOR, 0
      EXT_ENTRY str_ext_heading, 7, "HEADING", TURTLE_ROUTE_MARK, EXT_CMD_HEADING, 0
      EXT_ENTRY str_ext_pendownp, 8, "PENDOWN?", TURTLE_ROUTE_MARK, EXT_CMD_PENDOWNP, 0
      EXT_ENTRY str_ext_shownp, 6, "SHOWN?", TURTLE_ROUTE_MARK, EXT_CMD_SHOWNP, 0
      ; --- Pen commands ---
      EXT_ENTRY str_ext_setpc, 5, "SETPC", TURTLE_ROUTE_MARK, EXT_CMD_SETPC, 1
      EXT_ENTRY str_ext_setpencolor, 11, "SETPENCOLOR", TURTLE_ROUTE_MARK, EXT_CMD_SETPC, 1
      EXT_ENTRY str_ext_setbg, 5, "SETBG", TURTLE_ROUTE_MARK, EXT_CMD_SETBG, 1
      EXT_ENTRY str_ext_setbackground, 13, "SETBACKGROUND", TURTLE_ROUTE_MARK, EXT_CMD_SETBG, 1
      ; --- TOWARDS reporter ---
      EXT_ENTRY str_ext_towards, 7, "TOWARDS", TURTLE_ROUTE_MARK, EXT_CMD_TOWARDS, 2
      ; --- VGC graphics commands (4c.1-3: routed through lib_call(GRAPHICS)) ---
      EXT_ENTRY str_ext_setcolor, 8, "SETCOLOR", MODULE_ID_GRAPHICS, GFN_GCOLOR, 1
      EXT_ENTRY str_ext_plot, 4, "PLOT", MODULE_ID_GRAPHICS, GFN_PLOT, 2
      EXT_ENTRY str_ext_unplot, 6, "UNPLOT", MODULE_ID_GRAPHICS, GFN_UNPLOT, 2
      EXT_ENTRY str_ext_line, 4, "LINE", MODULE_ID_GRAPHICS, GFN_LINE, 4
      EXT_ENTRY str_ext_circle, 6, "CIRCLE", MODULE_ID_GRAPHICS, GFN_CIRCLE, 3
      EXT_ENTRY str_ext_rect, 4, "RECT", MODULE_ID_GRAPHICS, GFN_RECT, 4
      EXT_ENTRY str_ext_rectangle, 9, "RECTANGLE", MODULE_ID_GRAPHICS, GFN_RECT, 4
      EXT_ENTRY str_ext_fill, 4, "FILL", MODULE_ID_GRAPHICS, GFN_FILL, 4
      EXT_ENTRY str_ext_fillrect, 8, "FILLRECT", MODULE_ID_GRAPHICS, GFN_FILL, 4
      EXT_ENTRY str_ext_paint, 5, "PAINT", MODULE_ID_GRAPHICS, GFN_PAINT, 2
      ; --- Sprite commands (4c.1-3: routed through lib_call(GRAPHICS)) ---
      EXT_ENTRY str_ext_sprite, 6, "SPRITE", MODULE_ID_GFXADAPTER, GADAPT_SPRITE, 3
      EXT_ENTRY str_ext_spritepos, 9, "SPRITEPOS", MODULE_ID_GRAPHICS, GFN_SPR_POS, 3
      EXT_ENTRY str_ext_spriteon, 8, "SPRITEON", MODULE_ID_GRAPHICS, GFN_SPR_ENABLE, 1
      EXT_ENTRY str_ext_spriteoff, 9, "SPRITEOFF", MODULE_ID_GRAPHICS, GFN_SPR_DISABLE, 1
      EXT_ENTRY str_ext_sprcollp, 16, "SPRITECOLLISION?", MODULE_ID_GFXADAPTER, GADAPT_SPRCOLLP, 1
      ; --- Sound commands (Phase B: SOUND module $02) ---
      EXT_ENTRY str_ext_tone, 4, "TONE", MODULE_ID_SOUND, SND_TONE, 2
      EXT_ENTRY str_ext_noise, 5, "NOISE", MODULE_ID_SOUND, SND_NOISE, 1
      EXT_ENTRY str_ext_volume, 6, "VOLUME", MODULE_ID_SOUND, SND_VOLUME, 1
      ; --- Timing commands (Phase B: SYSTEM module $03) ---
      EXT_ENTRY str_ext_wait, 4, "WAIT", MODULE_ID_SYSTEM, SYS_FN_WAIT, 1
      EXT_ENTRY str_ext_waitvbl, 7, "WAITVBL", MODULE_ID_SYSTEM, SYS_FN_WAITVBL, 0
      EXT_ENTRY str_ext_timer, 5, "TIMER", MODULE_ID_SYSTEM, SYS_FN_TIMER, 0
      .byte 0                    ; end sentinel

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

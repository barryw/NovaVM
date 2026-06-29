; procedures.s — user-defined procedure storage and invocation
;
; Procedure record layout (heap-allocated linked list):
;   +0: next_lo
;   +1: next_hi    (pointer to next procedure, or $0000)
;   +2: name_len   (1 byte)
;   +3..3+NL-1: name chars (uppercased)
;   +3+NL: param_count (1 byte)
;   For each param:
;     1 byte: param_name_len
;     N bytes: param_name chars (uppercased)
;   After all params:
;     2 bytes: body_len_lo, body_len_hi (16-bit)
;     body_len bytes: body text (lines separated by $0A)

; =====================================================================
; ZEROPAGE segment — procedure variables
; =====================================================================
      .segment "ZEROPAGE"

proc_head_lo:     .res 1        ; head of procedure directory
proc_head_hi:     .res 1
proc_entry_lo:    .res 1        ; current entry pointer (walk scratch)
proc_entry_hi:    .res 1
proc_ptr_lo:      .res 1        ; scratch pointer for proc operations
proc_ptr_hi:      .res 1
proc_body_len_lo: .res 1        ; body length during collection / invocation
proc_body_len_hi: .res 1
proc_param_cnt:   .res 1        ; param count during define / invoke
proc_name_len:    .res 1        ; name length scratch
proc_rec_off:     .res 1        ; offset into record during invoke

; =====================================================================
; BSS segment — temporary buffers
; =====================================================================
      .segment "BSS"

proc_body_buf:    .res 2048     ; editor text buffer: "TO ..\n<body>\nEND\n"
proc_name_buf:    .res 32       ; procedure name: length-prefixed
proc_param_buf:   .res 128      ; packed: count, [len,chars]... each param
proc_param_end:   .res 1        ; write offset into proc_param_buf
proc_editor_title:.res 36       ; "TO <name>" + NUL, passed to the shared editor
proc_body_src_lo: .res 1        ; pointer to the body text proc_build_record copies
proc_body_src_hi: .res 1
proc_edit_cur_lo: .res 1        ; initial editor cursor offset (start of body line)
proc_edit_cur_hi: .res 1
proc_scan_off_lo: .res 1        ; 16-bit scan offset for body extraction
proc_scan_off_hi: .res 1
proc_brk_depth:   .res 1        ; bracket/paren nesting depth while accumulating a statement
proc_tmp:         .res 1        ; scratch byte (EDIT record→buffer copy)

; Save area for eval state during procedure invocation
save_eval_cur_lo: .res 1
save_eval_cur_hi: .res 1
save_tok_head_lo: .res 1
save_tok_head_hi: .res 1
save_tok_tail_lo: .res 1
save_tok_tail_hi: .res 1
save_input_buf:   .res 128      ; save outer input_buf during proc body exec
save_heap_ptr_lo: .res 1        ; save heap for token re-use
save_heap_ptr_hi: .res 1

; Param-binding scratch: length-prefixed name built here for var_set
param_name_tmp:   .res 32

; LOAD scan state: absolute position + remaining bytes within proc_body_buf
; (which holds the freshly-loaded file text). Durable across tokenize_line /
; proc_build_record calls (those clobber ZP scratch but not BSS).
load_pos_lo:      .res 1
load_pos_hi:      .res 1
load_rem_lo:      .res 1
load_rem_hi:      .res 1

; SAVE accumulation buffer: all procedures serialized to "TO ... END\n" text.
save_buf:         .res 4096
save_pos_lo:      .res 1
save_pos_hi:      .res 1

; LOAD mode flag: when nonzero, proc_collect defines from the load buffer instead
; of opening the editor (set around proc_load_text by do_load).
proc_load_active: .res 1

; Predecessor "next"-field pointer recorded by proc_find_by_name for ERASE.
proc_prev_lo:     .res 1
proc_prev_hi:     .res 1

; Scratch name length for the SAVE variable serializer.
sb_namelen:       .res 1

; =====================================================================
; CODE segment — procedure routines
; =====================================================================
      .segment "CODE"

; ---------------------------------------------------------------------
; proc_init — initialize procedure directory to empty
; ---------------------------------------------------------------------
proc_init:
      STZ   proc_head_lo
      STZ   proc_head_hi
      STZ   proc_stopped
      STZ   proc_load_active
      RTS

; ---------------------------------------------------------------------
; proc_collect — enter TO/END multi-line collection mode
;   Called from main_loop after detecting TO as first token.
;   eval_cur_lo/hi has been advanced past the TO token.
; ---------------------------------------------------------------------
proc_collect:
      JSR   proc_parse_header
      BCS   @err_noname
      LDA   proc_load_active
      BEQ   @interactive
      ; LOAD path: collect the body from the load buffer (proc_load_body sets
      ; proc_body_src/proc_body_len directly) and build the record.
      JSR   proc_load_body
      JSR   proc_build_record
      RTS
@interactive:
      ; Initialize body buffer, then open the shared EDITUI editor on it. Carry
      ; set means the user asked to save before exiting.
      STZ   proc_body_len_lo
      STZ   proc_body_len_hi
      JSR   proc_open_editor
      BCS   @body_done
      RTS
@body_done:
      JSR   proc_print_defined
      RTS
@err_noname:
      JSR   print_inl
      .byte "TO NEEDS A NAME", 0
      JSR   eval_newline
      RTS

; ---------------------------------------------------------------------
; proc_parse_header — parse "<name> :p1 :p2 ..." from the token stream at
;   eval_cur (already advanced past the TO command token) into the collect
;   buffers: proc_name_buf/proc_name_len and proc_param_buf/proc_param_cnt/
;   proc_param_end. Returns carry clear = ok, carry set = no name token (no
;   print — caller reports). Shared by the interactive TO collector and LOAD.
; ---------------------------------------------------------------------
proc_parse_header:
      ; Next token should be the procedure name (TOK_WORD)
      LDA   eval_cur_lo
      ORA   eval_cur_hi
      BNE   @have_name
      SEC
      RTS
@have_name:
      LDA   eval_cur_lo
      STA   ptr_lo
      LDA   eval_cur_hi
      STA   ptr_hi
      LDY   #TOK_TAG
      LDA   (ptr_lo),Y
      CMP   #TOK_WORD
      BEQ   @name_ok
      SEC
      RTS
@name_ok:
      ; Copy procedure name to proc_name_buf (length-prefixed)
      LDY   #TOK_PAYLOAD
      LDA   (ptr_lo),Y           ; name length
      STA   proc_name_buf
      STA   proc_name_len
      TAX                         ; X = char count
      LDY   #TOK_PAYLOAD+1
      LDA   #0
      STA   proc_rec_off          ; dest index in name_buf
@copy_name:
      CPX   #0
      BEQ   @name_copied
      LDA   (ptr_lo),Y           ; already uppercased by tokenizer
      PHY
      LDY   proc_rec_off
      STA   proc_name_buf+1,Y
      INC   proc_rec_off
      PLY
      INY
      DEX
      BRA   @copy_name
@name_copied:

      ; Advance past the name token
      JSR   eval_advance

      ; Parse parameter names (TOK_VARREF tokens: :SIZE :COLOR etc.)
      STZ   proc_param_cnt
      LDA   #1                    ; dest offset (skip count byte at [0])
      STA   proc_param_end

@parse_params:
      LDA   eval_cur_lo
      ORA   eval_cur_hi
      BEQ   @params_done
      LDA   eval_cur_lo
      STA   ptr_lo
      LDA   eval_cur_hi
      STA   ptr_hi
      LDY   #TOK_TAG
      LDA   (ptr_lo),Y
      CMP   #TOK_VARREF
      BNE   @params_done

      INC   proc_param_cnt
      LDY   #TOK_PAYLOAD
      LDA   (ptr_lo),Y           ; param name length
      LDX   proc_param_end
      STA   proc_param_buf,X     ; store length
      INX
      TAY                         ; Y = char count to copy
      STZ   proc_rec_off          ; source char index

@copy_param_ch:
      CPY   #0
      BEQ   @param_ch_done
      ; Read source char from token
      LDA   eval_cur_lo
      STA   ptr_lo
      LDA   eval_cur_hi
      STA   ptr_hi
      PHY
      LDA   proc_rec_off
      CLC
      ADC   #TOK_PAYLOAD+1
      TAY
      LDA   (ptr_lo),Y           ; already uppercased
      PLY
      ; Write to proc_param_buf
      STA   proc_param_buf,X
      INX
      INC   proc_rec_off
      DEY
      BRA   @copy_param_ch
@param_ch_done:
      STX   proc_param_end

      JSR   eval_advance
      BRA   @parse_params

@params_done:
      LDA   proc_param_cnt
      STA   proc_param_buf        ; count byte at [0]
      CLC
      RTS

; ---------------------------------------------------------------------
; proc_print_defined — print "<name> DEFINED\n" (name from proc_name_buf).
; ---------------------------------------------------------------------
proc_print_defined:
      LDX   proc_name_len
      LDY   #0
@pn:
      CPX   #0
      BEQ   @pd
      LDA   proc_name_buf+1,Y
      STA   VGC_CHAROUT
      INY
      DEX
      BRA   @pn
@pd:
      JSR   print_inl
      .byte " DEFINED", 0
      JSR   eval_newline
      RTS

; ---------------------------------------------------------------------
; proc_build_header — write the "TO <name> :params" first line into
;   proc_body_buf (no trailing newline). Built with 8-bit Y since the header
;   is always < 256 bytes (name<=31, params<=127). Returns Y = header length.
;   Inputs: proc_name_buf/proc_name_len, proc_param_buf/proc_param_end.
; ---------------------------------------------------------------------
proc_build_header:
      LDY   #0
      LDA   #'T'
      STA   proc_body_buf,Y
      INY
      LDA   #'O'
      STA   proc_body_buf,Y
      INY
      LDA   #' '
      STA   proc_body_buf,Y
      INY
      LDX   #0                     ; name source index
@nm:
      CPX   proc_name_len
      BCS   @nm_done
      LDA   proc_name_buf+1,X
      STA   proc_body_buf,Y
      INY
      INX
      BRA   @nm
@nm_done:
      ; params: " :" + chars, walking proc_param_buf[1..proc_param_end)
      LDX   #1
@pp:
      CPX   proc_param_end
      BCS   @pp_done
      LDA   #' '
      STA   proc_body_buf,Y
      INY
      LDA   #':'
      STA   proc_body_buf,Y
      INY
      LDA   proc_param_buf,X       ; this param's char count
      STA   proc_rec_off
      INX
@ppc:
      LDA   proc_rec_off
      BEQ   @pp
      LDA   proc_param_buf,X
      STA   proc_body_buf,Y
      INY
      INX
      DEC   proc_rec_off
      BRA   @ppc
@pp_done:
      RTS

proc_open_editor:
      ; build "TO <name> :params" header, then append an empty body "  ".
      JSR   proc_build_header
      ; Y = line-1 length. The body line is line 1's '\n' + 2 indent spaces,
      ; so the cursor lands at offset (line1 + 3).
      TYA
      CLC
      ADC   #3
      STA   proc_edit_cur_lo
      LDA   #0
      ADC   #0
      STA   proc_edit_cur_hi
      ; append "\n  \nEND\n"
      LDA   #$0A
      STA   proc_body_buf,Y
      INY
      LDA   #' '
      STA   proc_body_buf,Y
      INY
      STA   proc_body_buf,Y
      INY
      LDA   #$0A
      STA   proc_body_buf,Y
      INY
      LDA   #'E'
      STA   proc_body_buf,Y
      INY
      LDA   #'N'
      STA   proc_body_buf,Y
      INY
      LDA   #'D'
      STA   proc_body_buf,Y
      INY
      LDA   #$0A
      STA   proc_body_buf,Y
      INY
      STY   proc_body_len_lo
      STZ   proc_body_len_hi

; ---------------------------------------------------------------------
; proc_edit_run — shared tail: build the editor title, hand proc_body_buf
;   (with proc_body_len + proc_edit_cur already set) to the EDITUI editor, and
;   on save extract the body + (re)build the procedure record.
;   Returns: carry set = saved, carry clear = abandoned.
; ---------------------------------------------------------------------
proc_edit_run:
      ; --- title "TO <name>" + NUL for the editor's title band ---
      LDA   #'T'
      STA   proc_editor_title+0
      LDA   #'O'
      STA   proc_editor_title+1
      LDA   #' '
      STA   proc_editor_title+2
      LDX   #0
      LDY   #3
@tt:
      CPX   proc_name_len
      BCS   @tt_done
      LDA   proc_name_buf+1,X
      STA   proc_editor_title,Y
      INX
      INY
      BRA   @tt
@tt_done:
      LDA   #0
      STA   proc_editor_title,Y

      ; --- lib_call(EDITOR, EDITOR_FN_EDIT): the editor lives in its own
      ;     shared module. 32-bit LE mailbox cells at $0303+:
      ;       ARG0 = buffer ptr (low word) + initial cursor offset (high word)
      ;       ARG1 = current length   ARG2 = capacity   ARG3 = NUL title ptr
      ;     The editor edits the buffer in place and runs entirely within the call
      ;     (no module->runtime callback); the proc record is built from the final
      ;     buffer after it returns.
      LDA   #<proc_body_buf
      STA   LIB_ARG0+0
      LDA   #>proc_body_buf
      STA   LIB_ARG0+1
      LDA   proc_edit_cur_lo          ; initial cursor offset -> ARG0 high word
      STA   LIB_ARG0+2
      LDA   proc_edit_cur_hi
      STA   LIB_ARG0+3
      LDA   proc_body_len_lo
      STA   LIB_ARG1+0
      LDA   proc_body_len_hi
      STA   LIB_ARG1+1
      LDA   #<2048
      STA   LIB_ARG2+0
      LDA   #>2048
      STA   LIB_ARG2+1
      LDA   #<proc_editor_title
      STA   LIB_ARG3+0
      LDA   #>proc_editor_title
      STA   LIB_ARG3+1
      LDA   #EDITOR_EDIT_PROFILE_LOGO
      STA   LIB_ARG3+2
      STZ   LIB_ARG3+3

      LDA   #MODULE_ID_EDITOR
      LDX   #EDITOR_FN_EDIT
      JSR   do_lib_call               ; page in EDITOR, run editor, restore host bank

      ; editor returns the final buffer length in ARG1, save flag in RESULT byte1
      LDA   LIB_ARG1+0
      STA   proc_body_len_lo
      LDA   LIB_ARG1+1
      STA   proc_body_len_hi

      LDA   LIB_RESULT+1
      BEQ   @abandoned
      ; Strip line 1 + the END line -> proc_body_src/proc_body_len = body only.
      JSR   proc_extract_body
      JSR   proc_build_record
      ; Re-establish the turtle split BEFORE returning carry (the helper clobbers
      ; A/X/Y/eval state but does not touch proc_body_* / EXT_RESULT). proc_build_
      ; record reads only proc_body_*, so it is safe to run it first.
      JSR   proc_edit_restore_split
      SEC
      RTS
@abandoned:
      JSR   proc_edit_restore_split
      CLC
      RTS

; ---------------------------------------------------------------------
; proc_edit_restore_split — after EDIT returns, re-establish the turtle split-
;   screen if it was active before EDIT. ext_edit tore down the copper split
;   (copper_off + full 80x25 text window) for full-screen editing and, on exit,
;   restored ee_saved_mode/colors/cursor + a full text window — but it can NO
;   LONGER rebuild the split (setup_copper/prepare_split_text moved into the
;   GRAPHICS module and were deleted from the extension). ext_edit also leaves
;   the module's TURTLE_GFX_VISIBLE flag ($9F11) stale = 1, so the next turtle
;   op short-circuits ensure_gfx_mode (no copper split) and logo_turtle_textwin
;   sees no 0->1 transition (no bottom band) — the REPL is left in a broken
;   full-screen display. The fix MUST live here in the FOUNDATION: ext_edit runs
;   in the extension (bank 1) and cannot lib_call (a lib_call returns to the
;   home/foundation bank, mis-mapping bank-1 code), and the split-setup helper no
;   longer exists in the extension. The foundation CAN lib_call the module.
;
;   When $9F11 != 0 (ext_edit preserves the pre-EDIT flag), synthesize a
;   SPLITSCREEN (SS) turtle op so the split + bottom text band come back exactly
;   as a normal SS would, with the user's drawing intact:
;     1. STZ $9F11 so turtle_enter_split is re-entered (ensure_gfx_mode /
;        logo_turtle_textwin do not short-circuit on a stale-visible flag).
;     2. EXT_CMD=SS, EXT_ARGC=0, ext_mod_id=MODULE_ID_TURTLE, JSR
;        logo_adapt_turtle: lib_calls GFN_TURTLE_OP[SS] -> t_ss ->
;        turtle_enter_split (copper split + $9F11=1, does NOT clear gfx), then
;        logo_turtle_textwin (SS always re-applies: form-feed clears the TEXT
;        plane only, carves the bottom band, parks the cursor). The gfx plane —
;        the user's picture and the turtle stamp — survives.
;   No-op when no split was active before EDIT.
;   Clobbers A/X/Y + eval_val/eval_type (proc_edit does not need them after).
; ---------------------------------------------------------------------
; TURTLE_GFX_VISIBLE ($9F11) — the GRAPHICS module's turtle-state flag at
; TURTLE_STATE_BASE+17 ($9F00+17 = $9F11): 1 = split/full gfx display active.
; Equated in eval.s (same translation unit), so it is visible here.
proc_edit_restore_split:
      LDA   TURTLE_GFX_VISIBLE
      BEQ   @no_split             ; no gfx/split active before EDIT -> nothing to do
      STZ   TURTLE_GFX_VISIBLE    ; clear stale flag so SS re-enters turtle_enter_split
      STZ   EXT_ARGC              ; SS takes no value args
      LDA   #EXT_CMD_SS
      STA   EXT_CMD
      LDA   #MODULE_ID_TURTLE
      STA   ext_mod_id
      JMP   logo_adapt_turtle     ; marshal op->ARG2, lib_call GFN_TURTLE_OP[SS],
                                  ; then logo_turtle_textwin re-carves the band; RTS
@no_split:
      RTS

; ---------------------------------------------------------------------
; proc_buf_put — append A to proc_body_buf at ptr2, advancing ptr2 and the
;   16-bit proc_body_len counter. Preserves A; clobbers Y.
; ---------------------------------------------------------------------
proc_buf_put:
      LDY   #0
      STA   (ptr2_lo),Y
      INC   ptr2_lo
      BNE   :+
      INC   ptr2_hi
:     INC   proc_body_len_lo
      BNE   :+
      INC   proc_body_len_hi
:     RTS

; ---------------------------------------------------------------------
; proc_record_to_buffers — unpack the procedure record at proc_entry into the
;   collect buffers so it can be edited and rebuilt:
;     proc_name_buf/proc_name_len, proc_param_buf/proc_param_cnt/proc_param_end,
;     and proc_body_src/proc_body_len = the record's body text (ptr + length).
;   The name+param region is < 256 bytes so 8-bit Y indexing is safe there.
; ---------------------------------------------------------------------
proc_record_to_buffers:
      LDA   proc_entry_lo
      STA   ptr_lo
      LDA   proc_entry_hi
      STA   ptr_hi
      ; --- name (record +2 = len, +3.. = chars) ---
      LDY   #2
      LDA   (ptr_lo),Y
      STA   proc_name_buf
      STA   proc_name_len
      INY                         ; Y=3 -> first name char
      LDX   #0
@rb_nm:
      CPX   proc_name_len
      BCS   @rb_nm_done
      LDA   (ptr_lo),Y
      STA   proc_name_buf+1,X
      INY
      INX
      BRA   @rb_nm
@rb_nm_done:
      ; --- param_count then packed params ---
      LDA   (ptr_lo),Y           ; param_count
      STA   proc_param_cnt
      STA   proc_param_buf        ; [0] = count
      STA   proc_tmp              ; remaining params
      INY                         ; Y -> first param len
      LDX   #1                    ; dest index into proc_param_buf
@rb_param:
      LDA   proc_tmp
      BEQ   @rb_params_done
      DEC   proc_tmp
      LDA   (ptr_lo),Y           ; param name length
      STA   proc_param_buf,X
      STA   proc_rec_off          ; char counter
      INY
      INX
@rb_pchars:
      LDA   proc_rec_off
      BEQ   @rb_param
      DEC   proc_rec_off
      LDA   (ptr_lo),Y
      STA   proc_param_buf,X
      INY
      INX
      BRA   @rb_pchars
@rb_params_done:
      STX   proc_param_end
      ; Y -> body_len_lo within the record. Compute absolute pointer.
      TYA
      CLC
      ADC   proc_entry_lo
      STA   ptr_lo
      LDA   proc_entry_hi
      ADC   #0
      STA   ptr_hi               ; ptr -> body_len_lo
      LDY   #0
      LDA   (ptr_lo),Y
      STA   proc_body_len_lo     ; record body length
      INY
      LDA   (ptr_lo),Y
      STA   proc_body_len_hi
      CLC                         ; body src = ptr + 2
      LDA   ptr_lo
      ADC   #2
      STA   proc_body_src_lo
      LDA   ptr_hi
      ADC   #0
      STA   proc_body_src_hi
      RTS

; ---------------------------------------------------------------------
; proc_edit_reconstruct — build the full editable text "TO <name> :params\n
;   <body>END\n" into proc_body_buf from the collect buffers + the record body
;   (proc_body_src/proc_body_len set by proc_record_to_buffers). Sets
;   proc_body_len = total buffer length and proc_edit_cur = start of the body.
; ---------------------------------------------------------------------
proc_edit_reconstruct:
      ; Save the record body src/len before proc_body_len is repurposed.
      LDA   proc_body_src_lo
      STA   proc_ptr_lo
      LDA   proc_body_src_hi
      STA   proc_ptr_hi
      LDA   proc_body_len_lo
      STA   num_tmp_lo
      LDA   proc_body_len_hi
      STA   num_tmp_hi
      ; Header "TO name params" (8-bit), then a newline.
      JSR   proc_build_header     ; Y = header length
      LDA   #$0A
      STA   proc_body_buf,Y
      INY                         ; Y = body-line start offset
      STY   proc_edit_cur_lo
      STZ   proc_edit_cur_hi
      STY   proc_body_len_lo
      STZ   proc_body_len_hi
      ; dest pointer ptr2 = proc_body_buf + Y
      TYA
      CLC
      ADC   #<proc_body_buf
      STA   ptr2_lo
      LDA   #>proc_body_buf
      ADC   #0
      STA   ptr2_hi
      ; copy the record body (num_tmp bytes from proc_ptr)
@rc_body:
      LDA   num_tmp_lo
      ORA   num_tmp_hi
      BEQ   @rc_body_done
      LDY   #0
      LDA   (proc_ptr_lo),Y
      JSR   proc_buf_put
      INC   proc_ptr_lo
      BNE   :+
      INC   proc_ptr_hi
:     LDA   num_tmp_lo
      BNE   @rc_dec
      DEC   num_tmp_hi
@rc_dec:
      DEC   num_tmp_lo
      BRA   @rc_body
@rc_body_done:
      ; append "END" + newline
      LDA   #'E'
      JSR   proc_buf_put
      LDA   #'N'
      JSR   proc_buf_put
      LDA   #'D'
      JSR   proc_buf_put
      LDA   #$0A
      JSR   proc_buf_put
      RTS

; ---------------------------------------------------------------------
; proc_edit — handle "EDIT <name>" / "ED <name>". eval_cur is at the name
;   token. If the procedure exists, reopen it in the editor pre-filled with its
;   current definition; otherwise fall back to the TO collector (create it).
; ---------------------------------------------------------------------
proc_edit:
      LDA   eval_cur_lo
      ORA   eval_cur_hi
      BEQ   @to_collect
      JSR   proc_lookup
      BCS   @to_collect          ; unknown -> create via the TO collector
      JSR   proc_record_to_buffers
      JSR   proc_edit_reconstruct
      JSR   proc_edit_run
      BCC   @done                ; abandoned
      JSR   proc_print_defined
@done:
      RTS
@to_collect:
      JMP   proc_collect

; ---------------------------------------------------------------------
; proc_extract_body — narrow proc_body_buf/proc_body_len down to just the body:
;   everything after the first '\n' (line 1) up to (not including) the line that
;   reads "END". Sets proc_body_src_lo/hi + proc_body_len_lo/hi.
; ---------------------------------------------------------------------
proc_extract_body:
      LDA   #<proc_body_buf
      STA   proc_ptr_lo
      LDA   #>proc_body_buf
      STA   proc_ptr_hi
      STZ   proc_scan_off_lo
      STZ   proc_scan_off_hi
@find_nl:
      JSR   proc_scan_at_end
      BCS   @whole                ; no newline -> whole buffer is the body
      LDY   #0
      LDA   (proc_ptr_lo),Y
      JSR   proc_scan_advance
      CMP   #$0A
      BNE   @find_nl
      ; body starts here
      LDA   proc_ptr_lo
      STA   proc_body_src_lo
      LDA   proc_ptr_hi
      STA   proc_body_src_hi
      LDA   proc_scan_off_lo
      STA   num_tmp_lo            ; body-start offset
      LDA   proc_scan_off_hi
      STA   num_tmp_hi
@line:
      JSR   proc_scan_at_end
      BCS   @body_end             ; ran off the end -> body = rest
      JSR   proc_line_is_end
      BCS   @body_end             ; this line is "END" -> stop
@skip:
      JSR   proc_scan_at_end
      BCS   @body_end
      LDY   #0
      LDA   (proc_ptr_lo),Y
      JSR   proc_scan_advance
      CMP   #$0A
      BNE   @skip
      BRA   @line
@body_end:
      SEC
      LDA   proc_scan_off_lo
      SBC   num_tmp_lo
      STA   proc_body_len_lo
      LDA   proc_scan_off_hi
      SBC   num_tmp_hi
      STA   proc_body_len_hi
      RTS
@whole:
      LDA   #<proc_body_buf
      STA   proc_body_src_lo
      LDA   #>proc_body_buf
      STA   proc_body_src_hi
      RTS

; carry set if proc_scan_off >= proc_body_len
proc_scan_at_end:
      LDA   proc_scan_off_lo
      CMP   proc_body_len_lo
      LDA   proc_scan_off_hi
      SBC   proc_body_len_hi
      RTS

; advance the scan pointer + offset by one byte
proc_scan_advance:
      INC   proc_ptr_lo
      BNE   :+
      INC   proc_ptr_hi
:     INC   proc_scan_off_lo
      BNE   :+
      INC   proc_scan_off_hi
:     RTS

; proc_line_is_end — carry set if the line at proc_ptr is exactly "END" followed
; by '\n' or end-of-buffer. Needs room for 3 chars (off+3 <= len). Non-destructive.
proc_line_is_end:
      ; proc_entry = proc_scan_off + 3
      CLC
      LDA   proc_scan_off_lo
      ADC   #3
      STA   proc_entry_lo
      LDA   proc_scan_off_hi
      ADC   #0
      STA   proc_entry_hi
      ; need off+3 <= len, i.e. NOT (off+3 > len) == NOT (len < off+3)
      LDA   proc_body_len_lo
      CMP   proc_entry_lo
      LDA   proc_body_len_hi
      SBC   proc_entry_hi
      BCC   @no                   ; len < off+3 -> not enough room
      LDY   #0
      LDA   (proc_ptr_lo),Y
      CMP   #'E'
      BNE   @no
      INY
      LDA   (proc_ptr_lo),Y
      CMP   #'N'
      BNE   @no
      INY
      LDA   (proc_ptr_lo),Y
      CMP   #'D'
      BNE   @no
      ; 4th byte: '\n', or off+3 == len (END at buffer end)
      LDA   proc_entry_lo
      CMP   proc_body_len_lo
      BNE   @check_nl
      LDA   proc_entry_hi
      CMP   proc_body_len_hi
      BEQ   @yes                  ; off+3 == len
@check_nl:
      INY
      LDA   (proc_ptr_lo),Y
      CMP   #$0A
      BNE   @no
@yes:
      SEC
      RTS
@no:
      CLC
      RTS

; ---------------------------------------------------------------------
; heap_alloc_big — allocate for procedure records (16-bit payload)
;   Input:  proc_ptr_lo/hi = record data size (16-bit)
;   Output: proc_entry_lo/hi = pointer to record data (payload+2)
;           Carry clear = success, Carry set = OOM
;
;   Layout: [GC_HDR_TAG, GC_HDR_SIZE=0, total_lo, total_hi, ...data...]
;   GC header (2 bytes) + 16-bit size (2 bytes) + record data.
;   Total allocation = record_data_size + 4.
;   GC_HDR_SIZE = 0 tells GC to read 16-bit total from payload[0..1].
;   proc_entry points to data (base+4).
;
;   Clobbers: A, Y, ptr_lo/hi
; ---------------------------------------------------------------------
heap_alloc_big:
      ; Total allocation = data_size + 4 (2 hdr + 2 size field)
      CLC
      LDA   proc_ptr_lo
      ADC   #4
      PHA                        ; save total_lo
      LDA   proc_ptr_hi
      ADC   #0
      PHA                        ; save total_hi

      ; Save current heap_ptr as base
      LDA   heap_ptr
      STA   proc_entry_lo
      LDA   heap_ptr+1
      STA   proc_entry_hi

      ; Advance heap_ptr by total
      TSX
      CLC
      LDA   heap_ptr
      ADC   $0102,X              ; total_lo
      STA   heap_ptr
      LDA   heap_ptr+1
      ADC   $0101,X              ; total_hi
      STA   heap_ptr+1

      ; OOM check
      CMP   #>HEAP_END
      BCC   @big_ok
      BNE   @big_oom
      LDA   heap_ptr
      CMP   #<HEAP_END
      BCC   @big_ok
      BEQ   @big_ok
@big_oom:
      LDA   proc_entry_lo
      STA   heap_ptr
      LDA   proc_entry_hi
      STA   heap_ptr+1
      PLA
      PLA
      SEC
      RTS
@big_ok:
      ; Write header at base (proc_entry = base here)
      LDA   proc_entry_lo
      STA   ptr_lo
      LDA   proc_entry_hi
      STA   ptr_hi

      LDY   #GC_HDR_TAG
      LDA   #ATYPE_PROC
      STA   (ptr_lo),Y

      LDY   #GC_HDR_SIZE
      LDA   #0                    ; 0 = 16-bit size at payload[0..1]
      STA   (ptr_lo),Y

      ; Write 16-bit total at payload offsets 0,1 (base+2, base+3)
      PLA                        ; total_hi
      LDY   #GC_HDR_BYTES+1       ; base+3
      STA   (ptr_lo),Y
      PLA                        ; total_lo
      LDY   #GC_HDR_BYTES         ; base+2
      STA   (ptr_lo),Y

      ; Advance proc_entry to point past header+size field = base+4
      ; This is where the record data starts
      CLC
      LDA   proc_entry_lo
      ADC   #4
      STA   proc_entry_lo
      LDA   proc_entry_hi
      ADC   #0
      STA   proc_entry_hi

      CLC
      RTS

; ---------------------------------------------------------------------
; proc_build_record — allocate and populate a procedure heap record
;   Uses: proc_name_buf, proc_param_buf/proc_param_end,
;         proc_body_buf/proc_body_len
; ---------------------------------------------------------------------
proc_build_record:
      ; Calculate total record size
      ; 2 (next) + 1 (name_len) + name_len
      ; + 1 (param_count) + (proc_param_end - 1) param packed bytes
      ; + 2 (body_len) + body_len
      ;
      ; We build the total in proc_ptr_lo:proc_ptr_hi (16-bit)

      LDA   #4                    ; 2(next) + 1(name_len) + 1(param_count)
      CLC
      ADC   proc_name_buf         ; + name_len
      STA   proc_ptr_lo
      LDA   #0
      STA   proc_ptr_hi

      ; + param packed bytes (proc_param_end - 1)
      LDA   proc_param_end
      SEC
      SBC   #1
      CLC
      ADC   proc_ptr_lo
      STA   proc_ptr_lo
      BCC   :+
      INC   proc_ptr_hi
:
      ; + 2 (body_len field)
      CLC
      LDA   proc_ptr_lo
      ADC   #2
      STA   proc_ptr_lo
      BCC   :+
      INC   proc_ptr_hi
:
      ; + body_len
      CLC
      LDA   proc_ptr_lo
      ADC   proc_body_len_lo
      STA   proc_ptr_lo
      LDA   proc_ptr_hi
      ADC   proc_body_len_hi
      STA   proc_ptr_hi

      ; Allocate with GC header (16-bit size via heap_alloc_big)
      ; proc_ptr_lo/hi = payload size
      JSR   heap_alloc_big
      BCC   @ok
      RTS                        ; OOM
@ok:
      ; Fill the record using (ptr_lo),Y indirect addressing
      ; We track Y as the write offset. For records > 256 bytes
      ; we bump ptr_hi when Y wraps.
      LDA   proc_entry_lo
      STA   ptr_lo
      LDA   proc_entry_hi
      STA   ptr_hi

      ; +0,+1: next pointer = current proc_head
      LDY   #0
      LDA   proc_head_lo
      STA   (ptr_lo),Y
      INY
      LDA   proc_head_hi
      STA   (ptr_lo),Y

      ; +2: name_len
      INY
      LDA   proc_name_buf
      STA   (ptr_lo),Y

      ; +3..+3+NL-1: name chars
      LDX   proc_name_buf         ; X = char count
      STZ   proc_rec_off          ; source index
@bld_name:
      CPX   #0
      BEQ   @bld_name_done
      INY
      BNE   :+
      INC   ptr_hi
:     PHX
      LDX   proc_rec_off
      LDA   proc_name_buf+1,X
      PLX
      STA   (ptr_lo),Y
      INC   proc_rec_off
      DEX
      BRA   @bld_name
@bld_name_done:

      ; +3+NL: param_count
      INY
      BNE   :+
      INC   ptr_hi
:     LDA   proc_param_cnt
      STA   (ptr_lo),Y

      ; Copy packed param data from proc_param_buf[1..proc_param_end-1]
      LDA   proc_param_end
      CMP   #2                    ; at least 1 = there are params
      BCC   @bld_params_done
      LDX   #1                    ; source index (skip count byte)
@bld_param:
      CPX   proc_param_end
      BCS   @bld_params_done
      INY
      BNE   :+
      INC   ptr_hi
:     LDA   proc_param_buf,X
      STA   (ptr_lo),Y
      INX
      BRA   @bld_param
@bld_params_done:

      ; body_len_lo, body_len_hi
      INY
      BNE   :+
      INC   ptr_hi
:     LDA   proc_body_len_lo
      STA   (ptr_lo),Y
      INY
      BNE   :+
      INC   ptr_hi
:     LDA   proc_body_len_hi
      STA   (ptr_lo),Y

      ; Copy body text (proc_body_src points at the body region of the buffer)
      LDA   proc_body_len_lo
      ORA   proc_body_len_hi
      BEQ   @bld_body_done

      LDA   proc_body_src_lo
      STA   proc_ptr_lo
      LDA   proc_body_src_hi
      STA   proc_ptr_hi
      ; 16-bit counter
      LDA   proc_body_len_lo
      STA   num_tmp_lo
      LDA   proc_body_len_hi
      STA   num_tmp_hi

@bld_body:
      INY
      BNE   :+
      INC   ptr_hi
:     PHY
      LDY   #0
      LDA   (proc_ptr_lo),Y
      PLY
      STA   (ptr_lo),Y
      ; Advance source
      INC   proc_ptr_lo
      BNE   :+
      INC   proc_ptr_hi
:     ; Decrement counter
      LDA   num_tmp_lo
      BNE   @bld_dec_lo
      DEC   num_tmp_hi
@bld_dec_lo:
      DEC   num_tmp_lo
      LDA   num_tmp_lo
      ORA   num_tmp_hi
      BNE   @bld_body

@bld_body_done:
      ; Prepend to directory
      LDA   proc_entry_lo
      STA   proc_head_lo
      LDA   proc_entry_hi
      STA   proc_head_hi
      RTS

; ---------------------------------------------------------------------
; proc_lookup — search for a procedure by name
;   Input: eval_cur_lo/hi points to a TOK_WORD token
;   Output: carry clear = found, proc_entry_lo/hi = record
;           carry set = not found
; ---------------------------------------------------------------------
proc_lookup:
      LDA   proc_head_lo
      STA   proc_entry_lo
      LDA   proc_head_hi
      STA   proc_entry_hi

@walk:
      LDA   proc_entry_lo
      ORA   proc_entry_hi
      BEQ   @not_found

      JSR   proc_names_equal
      BCS   @found

      ; Follow next pointer
      JSR   proc_next
      BRA   @walk

@found:
      CLC
      RTS
@not_found:
      SEC
      RTS

; ---------------------------------------------------------------------
; proc_next — advance proc_entry_lo/hi to the next record it links to.
;   In/out: proc_entry_lo/hi.  Clobbers: A, X, Y, ptr_lo/hi.
; ---------------------------------------------------------------------
proc_next:
      LDA   proc_entry_lo
      STA   ptr_lo
      LDA   proc_entry_hi
      STA   ptr_hi
      LDY   #0
      LDA   (ptr_lo),Y
      TAX
      INY
      LDA   (ptr_lo),Y
      STA   proc_entry_hi
      STX   proc_entry_lo
      RTS

; ---------------------------------------------------------------------
; proc_names_equal — compare token word vs record name
;   Input: eval_cur_lo/hi = TOK_WORD, proc_entry_lo/hi = record
;   Returns: carry set = equal, carry clear = not equal
; ---------------------------------------------------------------------
proc_names_equal:
      ; Token name length
      LDA   eval_cur_lo
      STA   ptr_lo
      LDA   eval_cur_hi
      STA   ptr_hi
      LDY   #TOK_PAYLOAD
      LDA   (ptr_lo),Y
      STA   proc_name_len         ; token name length

      ; Record name length
      LDA   proc_entry_lo
      STA   ptr_lo
      LDA   proc_entry_hi
      STA   ptr_hi
      LDY   #2
      LDA   (ptr_lo),Y

      CMP   proc_name_len
      BNE   @ne

      ; Lengths match — compare chars
      TAX                         ; X = length
      BEQ   @eq
      LDY   #0                    ; char index
@cmp:
      PHX
      PHY
      ; Token char at eval_cur + TOK_PAYLOAD + 1 + charIdx
      LDA   eval_cur_lo
      STA   ptr_lo
      LDA   eval_cur_hi
      STA   ptr_hi
      TYA
      CLC
      ADC   #TOK_PAYLOAD+1
      TAY
      LDA   (ptr_lo),Y
      STA   proc_rec_off          ; temp: token char

      ; Record char at proc_entry + 3 + charIdx
      PLY
      PHY
      LDA   proc_entry_lo
      STA   ptr_lo
      LDA   proc_entry_hi
      STA   ptr_hi
      TYA
      CLC
      ADC   #3
      TAY
      LDA   (ptr_lo),Y

      CMP   proc_rec_off
      BNE   @ne_pop
      PLY
      PLX
      INY
      DEX
      BNE   @cmp
@eq:
      SEC
      RTS
@ne_pop:
      PLY
      PLX
@ne:
      CLC
      RTS

; ---------------------------------------------------------------------
; proc_invoke — invoke a user-defined procedure
;   Input: eval_cur_lo/hi = TOK_WORD token (the procedure name)
;          proc_entry_lo/hi = matching procedure record
;   On exit: JMPs to eval_loop or eval_body to continue.
; ---------------------------------------------------------------------
proc_invoke:
      ; Save outer tokenizer and eval state on the hardware stack.
      ; We push: tok_head(2), tok_tail(2), eval_in_body(1),
      ;          proc_stopped(1) = 6 bytes.
      ; input_buf is saved to BSS (128 bytes too large for stack).
      LDA   tok_head_lo
      PHA
      LDA   tok_head_hi
      PHA
      LDA   tok_tail_lo
      PHA
      LDA   tok_tail_hi
      PHA
      LDA   eval_in_body
      PHA
      LDA   proc_stopped
      PHA

      ; Save input_buf to BSS save area
      LDX   #0
@save_ib:
      LDA   input_buf,X
      STA   save_input_buf,X
      INX
      CPX   #128
      BNE   @save_ib

      ; Advance eval_cur past the procedure name token
      JSR   eval_advance

      ; --- Bind parameters ---
      ; Navigate to param_count in the record:
      ;   offset = 3 + name_len
      LDA   proc_entry_lo
      STA   ptr_lo
      LDA   proc_entry_hi
      STA   ptr_hi
      LDY   #2
      LDA   (ptr_lo),Y           ; name_len
      CLC
      ADC   #3
      TAY                         ; Y = offset to param_count
      LDA   (ptr_lo),Y           ; param_count
      STA   proc_param_cnt
      INY                         ; Y = offset to first param name
      STY   proc_rec_off          ; save record offset

      LDA   proc_param_cnt
      BEQ   @bind_done

      LDX   proc_param_cnt
@bind_loop:
      PHX                         ; save remaining param count

      ; First: evaluate the next argument from the caller's token stream.
      ; eval_cur still points into the caller's token list.
      JSR   eval_expr
      BCC   @arg_ok
      JMP   @bind_err
@arg_ok:

      ; eval_val now holds the argument value.
      ; Push value onto stack (type + 3 value bytes = 4 bytes)
      LDA   eval_val_frac
      PHA
      LDA   eval_val_lo
      PHA
      LDA   eval_val_hi
      PHA
      LDA   eval_type
      PHA

      ; Build the param name as a length-prefixed string in param_name_tmp
      ; Read from record at proc_entry + proc_rec_off
      LDA   proc_entry_lo
      STA   ptr_lo
      LDA   proc_entry_hi
      STA   ptr_hi
      LDY   proc_rec_off
      LDA   (ptr_lo),Y           ; param name length
      STA   param_name_tmp        ; length byte
      TAX                         ; X = char count
      LDA   #0
      STA   proc_name_len         ; dest offset in param_name_tmp
@bind_ch:
      CPX   #0
      BEQ   @bind_ch_done
      INY
      LDA   (ptr_lo),Y
      PHX
      LDX   proc_name_len
      STA   param_name_tmp+1,X
      INC   proc_name_len
      PLX
      DEX
      BRA   @bind_ch
@bind_ch_done:
      ; Advance proc_rec_off past this param
      INY
      STY   proc_rec_off

      ; Pop value from stack back into eval_val
      PLA
      STA   eval_type
      PLA
      STA   eval_val_hi
      PLA
      STA   eval_val_lo
      PLA
      STA   eval_val_frac

      ; Call var_set with ptr_lo/hi → param_name_tmp
      LDA   #<param_name_tmp
      STA   ptr_lo
      LDA   #>param_name_tmp
      STA   ptr_hi
      JSR   var_set

      PLX                         ; restore remaining count
      DEX
      BNE   @bind_loop

@bind_done:
      ; Save heap pointer AFTER param binding so var_set allocations persist.
      ; Body tokenization will reuse heap from this point each line.
      LDA   heap_ptr
      STA   save_heap_ptr_lo
      LDA   heap_ptr+1
      STA   save_heap_ptr_hi

      ; Save eval_cur (resume point after all args consumed from caller)
      LDA   eval_cur_lo
      STA   save_eval_cur_lo
      LDA   eval_cur_hi
      STA   save_eval_cur_hi

      ; --- Execute the body ---
      ; Navigate to body in the record:
      ;   proc_rec_off now points past all params.
      ;   Next 2 bytes = body_len, then body text.
      LDA   proc_entry_lo
      STA   ptr_lo
      LDA   proc_entry_hi
      STA   ptr_hi
      LDY   proc_rec_off
      LDA   (ptr_lo),Y           ; body_len_lo
      STA   proc_body_len_lo
      INY
      LDA   (ptr_lo),Y           ; body_len_hi
      STA   proc_body_len_hi
      INY                         ; Y = offset to body text start
      STY   proc_rec_off

      ; Compute absolute pointer to body text start
      CLC
      LDA   proc_entry_lo
      ADC   proc_rec_off
      STA   proc_ptr_lo
      LDA   proc_entry_hi
      ADC   #0
      STA   proc_ptr_hi

      ; Process body line by line: find each $0A, copy line to input_buf,
      ; tokenize, eval, repeat.
      ; proc_ptr = current position in body text
      ; proc_body_len = remaining bytes

      ; Clear STOP/OUTPUT flag before executing body
      STZ   proc_stopped

@exec_line:
      ; Any body text remaining?
      LDA   proc_body_len_lo
      ORA   proc_body_len_hi
      BNE   @have_body
      JMP   @exec_done
@have_body:

      ; Reset heap for this statement's tokens (reuse token heap each statement)
      LDA   save_heap_ptr_lo
      STA   heap_ptr
      LDA   save_heap_ptr_hi
      STA   heap_ptr+1

      ; Accumulate physical lines into one logical statement: keep copying lines
      ; into input_buf until bracket/paren depth returns to 0, so a [ ... ] or
      ; ( ... ) that spans several lines is pieced together before we tokenize.
      LDX   #0                    ; input_buf index
      STZ   proc_brk_depth
@line_ch:
      LDA   proc_body_len_lo
      ORA   proc_body_len_hi
      BEQ   @line_eol             ; no more body text -> end of last line

      LDY   #0
      LDA   (proc_ptr_lo),Y      ; read body char
      CMP   #$0A
      BEQ   @line_nl
      CMP   #';'                 ; comment -> skip rest of physical line
      BEQ   @line_comment

      ; Store char in input_buf, tracking bracket/paren nesting depth.
      STA   input_buf,X
      INX
      CMP   #'['
      BEQ   @brk_inc
      CMP   #'('
      BEQ   @brk_inc
      CMP   #']'
      BEQ   @brk_dec
      CMP   #')'
      BEQ   @brk_dec
@brk_done:
      ; Advance body pointer
      INC   proc_ptr_lo
      BNE   :+
      INC   proc_ptr_hi
:     ; Decrement remaining
      LDA   proc_body_len_lo
      BNE   @ln_dec_lo
      DEC   proc_body_len_hi
@ln_dec_lo:
      DEC   proc_body_len_lo
      ; Guard against input_buf overflow (~120 chars per statement).
      CPX   #120
      BCS   @line_eol
      BRA   @line_ch

@brk_inc:
      INC   proc_brk_depth
      BRA   @brk_done
@brk_dec:
      LDA   proc_brk_depth
      BEQ   @brk_done             ; floor at 0
      DEC   proc_brk_depth
      BRA   @brk_done

@line_comment:
      ; ';' begins a comment: advance past the rest of this physical line
      ; without copying, so comment text (and any brackets in it) is ignored.
      LDA   proc_body_len_lo
      ORA   proc_body_len_hi
      BEQ   @line_eol             ; comment runs to end of body
      LDY   #0
      LDA   (proc_ptr_lo),Y
      PHA                         ; remember if this was the newline
      INC   proc_ptr_lo
      BNE   :+
      INC   proc_ptr_hi
:     LDA   proc_body_len_lo
      BNE   @cmt_dec
      DEC   proc_body_len_hi
@cmt_dec:
      DEC   proc_body_len_lo
      PLA
      CMP   #$0A
      BNE   @line_comment         ; keep skipping comment chars
      BRA   @line_eol             ; consumed the newline -> end of physical line

@line_nl:
      ; Skip the $0A
      INC   proc_ptr_lo
      BNE   :+
      INC   proc_ptr_hi
:     LDA   proc_body_len_lo
      BNE   @ln2_dec_lo
      DEC   proc_body_len_hi
@ln2_dec_lo:
      DEC   proc_body_len_lo
      ; fall through to end-of-physical-line decision

@line_eol:
      ; End of a physical line. If brackets/parens are still open AND there is
      ; more body to read AND room in the buffer, append a separating space and
      ; keep accumulating; otherwise the statement is complete.
      LDA   proc_brk_depth
      BEQ   @line_end             ; balanced -> execute
      LDA   proc_body_len_lo
      ORA   proc_body_len_hi
      BEQ   @line_end             ; no more lines -> execute (eval will report)
      CPX   #120
      BCS   @line_end             ; buffer full -> execute
      LDA   #' '                  ; join lines with whitespace
      STA   input_buf,X
      INX
      JMP   @line_ch

@line_end:
      ; Null-terminate input_buf
      STZ   input_buf,X

      ; Tokenize this line
      JSR   tokenize_line

      ; Save body-loop state that recursive proc_invoke would clobber.
      ; BSS save areas (save_heap_ptr, save_eval_cur) plus ZP body-loop
      ; variables (proc_ptr, proc_body_len) all need protection.
      LDA   proc_ptr_lo
      PHA
      LDA   proc_ptr_hi
      PHA
      LDA   proc_body_len_lo
      PHA
      LDA   proc_body_len_hi
      PHA
      LDA   save_heap_ptr_lo
      PHA
      LDA   save_heap_ptr_hi
      PHA
      LDA   save_eval_cur_lo
      PHA
      LDA   save_eval_cur_hi
      PHA

      ; Eval this line (full eval — not body mode)
      JSR   eval_line

      ; Restore body-loop state
      PLA
      STA   save_eval_cur_hi
      PLA
      STA   save_eval_cur_lo
      PLA
      STA   save_heap_ptr_hi
      PLA
      STA   save_heap_ptr_lo
      PLA
      STA   proc_body_len_hi
      PLA
      STA   proc_body_len_lo
      PLA
      STA   proc_ptr_hi
      PLA
      STA   proc_ptr_lo

      ; Check if STOP or OUTPUT was called — stop processing lines
      LDA   proc_stopped
      BNE   @exec_done

      JMP   @exec_line

@exec_done:
      ; Restore heap pointer (procedures stay, but per-line tokens freed)
      LDA   save_heap_ptr_lo
      STA   heap_ptr
      LDA   save_heap_ptr_hi
      STA   heap_ptr+1

      ; Restore input_buf
      LDX   #0
@rest_ib:
      LDA   save_input_buf,X
      STA   input_buf,X
      INX
      CPX   #128
      BNE   @rest_ib

      ; Restore eval_cur to the resume point in the caller's token stream
      LDA   save_eval_cur_lo
      STA   eval_cur_lo
      LDA   save_eval_cur_hi
      STA   eval_cur_hi

      ; Restore outer state from stack.
      ; X returns this invocation's proc_stopped to the caller.
      ; Stack layout (top to bottom):
      ;   saved_proc_stopped, eval_in_body, tok_tail_hi, tok_tail_lo,
      ;   tok_head_hi, tok_head_lo
      LDX   proc_stopped          ; X = this proc's exit state
      PLA                         ; pop saved outer proc_stopped
      STA   proc_stopped          ; restore outer proc_stopped
      PLA
      STA   eval_in_body
      PLA
      STA   tok_tail_hi
      PLA
      STA   tok_tail_lo
      PLA
      STA   tok_head_hi
      PLA
      STA   tok_head_lo

      ; Return with X = exit status ($00=normal, $01=STOP, $02=OUTPUT)
      RTS

@bind_err:
      PLX                         ; discard remaining param count
      ; Print error
      LDX   #0
@be_lp:
      LDA   str_notenough,X
      BEQ   @be_done
      STA   VGC_CHAROUT
      INX
      BNE   @be_lp
@be_done:
      JSR   eval_newline
      ; Restore input_buf and stack, skip body execution
      LDX   #0
@be_rest_ib:
      LDA   save_input_buf,X
      STA   input_buf,X
      INX
      CPX   #128
      BNE   @be_rest_ib
      ; Restore stack (6 bytes: proc_stopped, eval_in_body, tok_tail, tok_head)
      LDX   #$00                  ; X = normal exit status
      PLA                         ; saved proc_stopped
      STA   proc_stopped
      PLA
      STA   eval_in_body
      PLA
      STA   tok_tail_hi
      PLA
      STA   tok_tail_lo
      PLA
      STA   tok_head_hi
      PLA
      STA   tok_head_lo
      RTS

; ---------------------------------------------------------------------
; proc_find_by_name — search proc_head for a procedure by name string
;   Input: ptr_lo/hi = length-prefixed name string (e.g. from eval_val)
;   Output: carry clear = found, proc_entry_lo/hi = record
;           carry set = not found
;   Clobbers: A, X, Y, proc_entry, proc_name_len, proc_rec_off
; ---------------------------------------------------------------------
proc_find_by_name:
      ; Save search name pointer
      LDA   ptr_lo
      STA   var_name_lo             ; reuse var scratch as search ptr
      LDA   ptr_hi
      STA   var_name_hi

      ; Track the predecessor "next" field to patch (for ERASE). It starts as
      ; &proc_head_lo (a ZP address, so high byte = 0).
      LDA   #<proc_head_lo
      STA   proc_prev_lo
      STZ   proc_prev_hi
      LDA   proc_head_lo
      STA   proc_entry_lo
      LDA   proc_head_hi
      STA   proc_entry_hi

@walk:
      LDA   proc_entry_lo
      ORA   proc_entry_hi
      BEQ   @not_found

      ; Compare name lengths
      LDA   var_name_lo
      STA   ptr_lo
      LDA   var_name_hi
      STA   ptr_hi
      LDY   #0
      LDA   (ptr_lo),Y             ; search name length
      STA   proc_name_len

      LDA   proc_entry_lo
      STA   ptr_lo
      LDA   proc_entry_hi
      STA   ptr_hi
      LDY   #2
      LDA   (ptr_lo),Y             ; record name length
      CMP   proc_name_len
      BNE   @next

      ; Lengths match — compare chars
      TAX                           ; X = length
      BEQ   @found                  ; both zero = match
      LDY   #0
@cmp:
      PHX
      PHY
      ; Search char
      LDA   var_name_lo
      STA   ptr_lo
      LDA   var_name_hi
      STA   ptr_hi
      INY                           ; +1 for length byte
      LDA   (ptr_lo),Y
      STA   proc_rec_off            ; temp: search char

      ; Record char
      PLY
      LDA   proc_entry_lo
      STA   ptr_lo
      LDA   proc_entry_hi
      STA   ptr_hi
      PHY
      TYA
      CLC
      ADC   #3                      ; record name starts at offset 3
      TAY
      LDA   (ptr_lo),Y
      PLY
      CMP   proc_rec_off
      BNE   @ne_pop
      PLX
      INY
      DEX
      BNE   @cmp

@found:
      CLC
      RTS

@ne_pop:
      PLX
@next:
      ; prev = &current->next (current record), then current = current->next
      LDA   proc_entry_lo
      STA   proc_prev_lo
      LDA   proc_entry_hi
      STA   proc_prev_hi
      JSR   proc_next
      BRA   @walk

@not_found:
      SEC
      RTS

; ---------------------------------------------------------------------
; do_pots — POTS: print all procedure names
;   Arity 0, no arguments.
; ---------------------------------------------------------------------
do_pots:
      LDA   proc_head_lo
      STA   proc_entry_lo
      LDA   proc_head_hi
      STA   proc_entry_hi

@walk:
      LDA   proc_entry_lo
      ORA   proc_entry_hi
      BEQ   @done

      ; Print name
      LDA   proc_entry_lo
      STA   ptr_lo
      LDA   proc_entry_hi
      STA   ptr_hi
      LDY   #2
      LDA   (ptr_lo),Y             ; name_len
      TAX
      BEQ   @skip_name
      LDY   #3
@pch:
      LDA   (ptr_lo),Y
      STA   VGC_CHAROUT
      INY
      DEX
      BNE   @pch
@skip_name:
      LDA   #' '
      STA   VGC_CHAROUT

      ; Follow next
      JSR   proc_next
      BRA   @walk

@done:
      JSR   eval_newline
      JMP   eval_continue

; ---------------------------------------------------------------------
; do_po — PO "name: print procedure definition
;   Arity 0 — evaluates its own argument
; ---------------------------------------------------------------------
do_po:
      JSR   eval_expr
      BCS   @err
      LDA   eval_type
      CMP   #VAL_WORD
      BNE   @err
      ; Look up by name -> proc_entry
      LDA   eval_val_lo
      STA   ptr_lo
      LDA   eval_val_hi
      STA   ptr_hi
      JSR   proc_find_by_name
      BCS   @err_notfound
      ; Reuse the editor's record->text reconstruction, then print the buffer
      ; (the same "TO ... END" text SAVE writes), converting $0A to a newline.
      JSR   proc_record_to_buffers
      JSR   proc_edit_reconstruct       ; -> proc_body_buf[0..proc_body_len)
      LDA   #<proc_body_buf
      STA   proc_ptr_lo
      LDA   #>proc_body_buf
      STA   proc_ptr_hi
      LDA   proc_body_len_lo
      STA   num_tmp_lo
      LDA   proc_body_len_hi
      STA   num_tmp_hi
@pb:
      LDA   num_tmp_lo
      ORA   num_tmp_hi
      BEQ   @pb_done
      LDY   #0
      LDA   (proc_ptr_lo),Y
      CMP   #$0A
      BNE   @pb_ch
      JSR   eval_newline
      BRA   @pb_adv
@pb_ch:
      STA   VGC_CHAROUT
@pb_adv:
      INC   proc_ptr_lo
      BNE   :+
      INC   proc_ptr_hi
:     LDA   num_tmp_lo
      BNE   :+
      DEC   num_tmp_hi
:     DEC   num_tmp_lo
      BRA   @pb
@pb_done:
      JMP   eval_continue

@err:
      LDX   #<str_po_name
      LDY   #>str_po_name
      JMP   err_nei
@err_notfound:
      LDX   #<str_idk
      LDY   #>str_idk
      JMP   list_print_err

; ---------------------------------------------------------------------
; do_erase — ERASE "name: remove procedure from directory
;   Arity 0 — evaluates its own argument
; ---------------------------------------------------------------------
do_erase:
      JSR   eval_expr
      BCS   @err
      LDA   eval_type
      CMP   #VAL_WORD
      BNE   @err
      ; Locate the procedure; proc_find_by_name also records proc_prev = the
      ; predecessor "next" field to patch for the unlink.
      LDA   eval_val_lo
      STA   ptr_lo
      LDA   eval_val_hi
      STA   ptr_hi
      JSR   proc_find_by_name
      BCS   @not_found
      ; Unlink: *proc_prev = proc_entry->next
      LDA   proc_entry_lo
      STA   ptr_lo
      LDA   proc_entry_hi
      STA   ptr_hi
      LDY   #0
      LDA   (ptr_lo),Y             ; next_lo -> X
      TAX
      INY
      LDA   (ptr_lo),Y             ; next_hi -> stack
      PHA
      LDA   proc_prev_lo           ; ptr = prev "next" field
      STA   ptr_lo
      LDA   proc_prev_hi
      STA   ptr_hi
      LDY   #0
      TXA
      STA   (ptr_lo),Y             ; prev->next_lo = next_lo
      INY
      PLA
      STA   (ptr_lo),Y             ; prev->next_hi = next_hi
      JMP   eval_continue

@not_found:
      LDX   #<str_idk
      LDY   #>str_idk
      JMP   list_print_err
@err:
      LDX   #<str_erase_name
      LDY   #>str_erase_name
      JMP   err_nei

; ---------------------------------------------------------------------
; do_apply — APPLY "proc [args]: call procedure with args from list
;   Arity 0 — evaluates its own arguments
;   Strategy: build "name arg1 arg2 ..." in input_buf, tokenize+eval.
; ---------------------------------------------------------------------
do_apply:
      ; Evaluate first arg: procedure name (quoted word)
      JSR   eval_expr
      BCC   @ap_type_ok
      JMP   @err
@ap_type_ok:
      LDA   eval_type
      CMP   #VAL_WORD
      BEQ   @ap_word_ok
      JMP   @err
@ap_word_ok:

      ; Copy proc name to input_buf
      LDA   eval_val_lo
      STA   ptr_lo
      LDA   eval_val_hi
      STA   ptr_hi
      STZ   z:buf_idx
      LDY   #0
      LDA   (ptr_lo),Y             ; name length
      TAX
      BEQ   @apply_args
      INY
@apply_name:
      LDA   (ptr_lo),Y
      PHY
      LDY   z:buf_idx
      STA   input_buf,Y
      INC   z:buf_idx
      PLY
      INY
      DEX
      BNE   @apply_name

@apply_args:
      ; Evaluate second arg: list of arguments
      JSR   eval_expr
      BCS   @err
      LDA   eval_type
      CMP   #VAL_LIST
      BNE   @err

      ; Render list elements to input_buf (after name)
      ; eval_val_lo/hi = list head
      LDA   eval_val_lo
      ORA   eval_val_hi
      BEQ   @apply_run              ; empty arg list, just run name

      ; Use render_list_to_buf — it prepends space before elements
      ; but we need a space before the first arg too
      LDA   z:buf_idx
      CMP   #126
      BCS   @apply_run
      TAY
      LDA   #' '
      STA   input_buf,Y
      INC   z:buf_idx

      ; render_list_to_buf expects eval_val set
      JSR   render_list_to_buf

@apply_run:
      ; Null-terminate
      LDX   z:buf_idx
      STZ   input_buf,X

      ; Save eval state and tokenize+eval (same pattern as do_run)
      LDA   eval_cur_lo
      PHA
      LDA   eval_cur_hi
      PHA
      LDA   tok_head_lo
      PHA
      LDA   tok_head_hi
      PHA
      LDA   tok_tail_lo
      PHA
      LDA   tok_tail_hi
      PHA
      LDA   eval_in_body
      PHA
      LDA   eval_reporter
      PHA

      JSR   tokenize_line
      JSR   eval_line

      PLA
      STA   eval_reporter
      PLA
      STA   eval_in_body
      PLA
      STA   tok_tail_hi
      PLA
      STA   tok_tail_lo
      PLA
      STA   tok_head_hi
      PLA
      STA   tok_head_lo
      PLA
      STA   eval_cur_hi
      PLA
      STA   eval_cur_lo

      JMP   eval_continue

@err:
      LDX   #<str_apply_name
      LDY   #>str_apply_name
      JMP   err_nei

; =====================================================================
; SAVE / LOAD support — serialize procedures to text and parse it back.
; =====================================================================

; ---------------------------------------------------------------------
; save_append — append proc_body_buf[0..proc_body_len) to save_buf at save_pos,
;   advancing save_pos. Run after proc_edit_reconstruct rendered one procedure's
;   "TO ... END\n" text into proc_body_buf. Clobbers A/Y, ptr2, proc_ptr,num_tmp.
; ---------------------------------------------------------------------
save_append:
      CLC                             ; dest = save_buf + save_pos -> ptr2
      LDA   #<save_buf
      ADC   save_pos_lo
      STA   ptr2_lo
      LDA   #>save_buf
      ADC   save_pos_hi
      STA   ptr2_hi
      LDA   #<proc_body_buf           ; src = proc_body_buf -> proc_ptr
      STA   proc_ptr_lo
      LDA   #>proc_body_buf
      STA   proc_ptr_hi
      LDA   proc_body_len_lo          ; count -> num_tmp
      STA   num_tmp_lo
      LDA   proc_body_len_hi
      STA   num_tmp_hi
@cp:
      LDA   num_tmp_lo
      ORA   num_tmp_hi
      BEQ   @done
      LDY   #0
      LDA   (proc_ptr_lo),Y
      STA   (ptr2_lo),Y
      INC   proc_ptr_lo
      BNE   :+
      INC   proc_ptr_hi
:     INC   ptr2_lo
      BNE   :+
      INC   ptr2_hi
:     LDA   num_tmp_lo
      BNE   :+
      DEC   num_tmp_hi
:     DEC   num_tmp_lo
      BRA   @cp
@done:
      CLC                             ; save_pos += proc_body_len
      LDA   save_pos_lo
      ADC   proc_body_len_lo
      STA   save_pos_lo
      LDA   save_pos_hi
      ADC   proc_body_len_hi
      STA   save_pos_hi
      RTS

; ---------------------------------------------------------------------
; sb_put — append A to save_buf at save_pos, advancing it. Drops the byte once
;   save_pos reaches 4096 (the save_buf size), guarding the BSS. Preserves X;
;   clobbers Y, ptr2.
; ---------------------------------------------------------------------
sb_put:
      PHA
      LDA   save_pos_hi
      CMP   #$10                      ; save_buf is 4096 ($1000) bytes -> full
      BCS   @full
      CLC
      LDA   #<save_buf
      ADC   save_pos_lo
      STA   ptr2_lo
      LDA   #>save_buf
      ADC   save_pos_hi
      STA   ptr2_hi
      PLA
      LDY   #0
      STA   (ptr2_lo),Y
      INC   save_pos_lo
      BNE   :+
      INC   save_pos_hi
:     RTS
@full:
      PLA
      RTS

; ---------------------------------------------------------------------
; sb_copybuf — append input_buf[0..buf_idx) to save_buf via sb_put. Used to flush
;   the output of render_number_to_buf / render_list_to_buf (which target
;   input_buf) into save_buf. Clobbers A/X/Y, ptr2.
; ---------------------------------------------------------------------
sb_copybuf:
      LDX   #0
@l:
      CPX   z:buf_idx
      BCS   @d
      LDA   input_buf,X
      JSR   sb_put
      INX
      BRA   @l
@d:
      RTS

; ---------------------------------------------------------------------
; sb_value — append a re-parseable rendering of eval_type/eval_val to save_buf:
;   number -> bare digits; word -> "word (quoted, so it reloads as a word, safe
;   for numeric words); list -> [ ... ] with bare elements (nested lists keep
;   their brackets). Reuses render_number_to_buf / render_list_to_buf (which
;   render into input_buf) + sb_copybuf. Clobbers A/X/Y, ptr/ptr2, list scratch.
; ---------------------------------------------------------------------
sb_value:
      LDA   eval_type
      CMP   #VAL_LIST
      BEQ   @list
      CMP   #VAL_WORD
      BEQ   @word
      ; number
      STZ   z:buf_idx
      JSR   render_number_to_buf
      JMP   sb_copybuf
@word:
      LDA   #'"'
      JSR   sb_put
      LDA   eval_val_lo
      STA   ptr_lo
      LDA   eval_val_hi
      STA   ptr_hi
      LDY   #0
      LDA   (ptr_lo),Y                ; word length
      TAX
      BEQ   @wd
      LDY   #1
@wc:
      LDA   (ptr_lo),Y
      PHY
      JSR   sb_put
      PLY
      INY
      DEX
      BNE   @wc
@wd:
      RTS
@list:
      LDA   #'['
      JSR   sb_put
      STZ   z:buf_idx
      JSR   render_list_to_buf        ; elements (with nested brackets) -> input_buf
      JSR   sb_copybuf
      LDA   #']'
      JMP   sb_put

; ---------------------------------------------------------------------
; sb_emit_var — append one "MAKE \"<name> <value>\n" line for the variable record
;   at var_entry. Reuses sb_value for the round-trippable value. Clobbers
;   A/X/Y, ptr/ptr2, eval_*, list scratch.
; ---------------------------------------------------------------------
sb_emit_var:
      LDX   #0                        ; emit "MAKE \""
@k:
      LDA   str_make_kw,X
      JSR   sb_put
      INX
      CPX   #6
      BNE   @k
      LDA   var_entry_lo              ; emit the name chars
      STA   ptr_lo
      LDA   var_entry_hi
      STA   ptr_hi
      LDY   #VAR_NAME_LEN
      LDA   (ptr_lo),Y
      STA   sb_namelen
      LDX   #0
@nm:
      CPX   sb_namelen
      BCS   @nmd
      TXA
      CLC
      ADC   #VAR_NAME
      TAY
      LDA   (ptr_lo),Y
      JSR   sb_put
      INX
      BRA   @nm
@nmd:
      LDA   #' '
      JSR   sb_put
      ; load the value (entry + 3 + name_len) into eval_*
      LDA   var_entry_lo
      STA   ptr_lo
      LDA   var_entry_hi
      STA   ptr_hi
      LDA   sb_namelen
      CLC
      ADC   #VAR_NAME
      TAY
      LDA   (ptr_lo),Y
      STA   eval_type
      INY
      LDA   (ptr_lo),Y
      STA   eval_val_hi
      INY
      LDA   (ptr_lo),Y
      STA   eval_val_lo
      INY
      LDA   (ptr_lo),Y
      STA   eval_val_frac
      JSR   sb_value
      LDA   #$0A
      JMP   sb_put

; ---------------------------------------------------------------------
; proc_serialize_all — serialize every user procedure into save_buf by reusing
;   the editor's proc_record_to_buffers + proc_edit_reconstruct (which renders
;   one "TO <name> :params\n<body>END\n" block into proc_body_buf). Sets LIB_ARG2
;   = save_buf start and LIB_ARG3 = end (one past the last byte) for FILE_SAVE.
;   Preserves LIB_ARG0/ARG1 (filename). Clobbers scratch. ~4 KB output cap.
; ---------------------------------------------------------------------
proc_serialize_all:
      STZ   save_pos_lo
      STZ   save_pos_hi
      LDA   proc_head_lo
      STA   proc_entry_lo
      LDA   proc_head_hi
      STA   proc_entry_hi
@walk:
      LDA   proc_entry_lo
      ORA   proc_entry_hi
      BEQ   @vars
      JSR   proc_record_to_buffers
      JSR   proc_edit_reconstruct      ; -> proc_body_buf[0..proc_body_len)
      JSR   save_append
      JSR   proc_next
      BRA   @walk
@vars:
      ; Append the global variables as "MAKE \"name value" lines AFTER the
      ; procedures, so LOAD defines procs first and then restores the variables.
      LDA   var_head_lo
      STA   var_entry_lo
      LDA   var_head_hi
      STA   var_entry_hi
@vwalk:
      LDA   var_entry_lo
      ORA   var_entry_hi
      BEQ   @done
      JSR   sb_emit_var
      LDA   var_entry_lo               ; advance var_entry = entry->next
      STA   ptr_lo
      LDA   var_entry_hi
      STA   ptr_hi
      LDY   #VAR_NEXT_LO
      LDA   (ptr_lo),Y
      TAX
      INY
      LDA   (ptr_lo),Y
      STA   var_entry_hi
      STX   var_entry_lo
      BRA   @vwalk
@done:
      LDA   #<save_buf
      STA   LIB_ARG2+0
      LDA   #>save_buf
      STA   LIB_ARG2+1
      CLC
      LDA   #<save_buf
      ADC   save_pos_lo
      STA   LIB_ARG3+0
      LDA   #>save_buf
      ADC   save_pos_hi
      STA   LIB_ARG3+1
      RTS

; ---------------------------------------------------------------------
; load_advance — advance proc_ptr by one byte and decrement load_rem (16-bit).
;   Preserves A; clobbers nothing else.
; ---------------------------------------------------------------------
load_advance:
      PHA
      INC   proc_ptr_lo
      BNE   :+
      INC   proc_ptr_hi
:     LDA   load_rem_lo
      BNE   :+
      DEC   load_rem_hi
:     DEC   load_rem_lo
      PLA
      RTS

; ---------------------------------------------------------------------
; load_next_line — copy the next text line from the load buffer (load_pos /
;   load_rem) into input_buf: NUL-terminated, CR stripped, capped at ~120 chars,
;   consuming the terminating '\n'. Returns carry set if a line was produced,
;   carry clear at end of buffer. Clobbers A/X/Y, proc_ptr.
; ---------------------------------------------------------------------
load_next_line:
      LDA   load_rem_lo
      ORA   load_rem_hi
      BNE   @go
      CLC
      RTS
@go:
      LDA   load_pos_lo
      STA   proc_ptr_lo
      LDA   load_pos_hi
      STA   proc_ptr_hi
      LDX   #0
@lp:
      LDA   load_rem_lo
      ORA   load_rem_hi
      BEQ   @eol
      LDY   #0
      LDA   (proc_ptr_lo),Y
      JSR   load_advance             ; consume the char (preserves A)
      CMP   #$0A
      BEQ   @eol                     ; end of line (newline consumed)
      CMP   #$0D
      BEQ   @lp                      ; drop CR (tokenizer treats it as space)
      CPX   #119
      BCS   @lp                      ; buffer full -> drop extra chars
      STA   input_buf,X
      INX
      BRA   @lp
@eol:
      LDA   #0
      STA   input_buf,X
      LDA   proc_ptr_lo
      STA   load_pos_lo
      LDA   proc_ptr_hi
      STA   load_pos_hi
      SEC
      RTS

; ---------------------------------------------------------------------
; proc_load_body — (LOAD path) collect the body lines that follow a "TO" header
;   from the load buffer into proc_body_buf, stopping at (and consuming) a line
;   that is exactly "END". Each line is stored followed by '\n', matching the
;   record's stored body format, so proc_collect can hand proc_body_src/
;   proc_body_len straight to proc_build_record. proc_buf_put advances ptr2 + the
;   16-bit proc_body_len. Clobbers A/X/Y, ptr2, proc_ptr.
; ---------------------------------------------------------------------
proc_load_body:
      LDA   #<proc_body_buf
      STA   ptr2_lo
      STA   proc_body_src_lo
      LDA   #>proc_body_buf
      STA   ptr2_hi
      STA   proc_body_src_hi
      STZ   proc_body_len_lo
      STZ   proc_body_len_hi
@bl:
      JSR   load_next_line
      BCC   @end                       ; EOF before END
      LDA   input_buf+0                 ; this line == "END"?
      CMP   #'E'
      BNE   @copy
      LDA   input_buf+1
      CMP   #'N'
      BNE   @copy
      LDA   input_buf+2
      CMP   #'D'
      BNE   @copy
      LDA   input_buf+3
      BEQ   @end
@copy:
      LDX   #0
@cl:
      LDA   input_buf,X
      BEQ   @cl_done
      JSR   proc_buf_put
      INX
      BRA   @cl
@cl_done:
      LDA   #$0A
      JSR   proc_buf_put
      BRA   @bl
@end:
      RTS

; ---------------------------------------------------------------------
; proc_load_text — (LOAD) drive the loaded text in the load buffer line by line
;   exactly like the REPL main loop: tokenize each line and route "TO" through
;   check_to_command, which — with proc_load_active set — defines the procedure
;   from the buffer via proc_load_body instead of opening the editor. Any other
;   non-empty line runs through eval_line, as if typed at the prompt. The caller
;   saves/restores outer eval state and sets/clears proc_load_active. Clobbers
;   everything.
; ---------------------------------------------------------------------
proc_load_text:
@line:
      JSR   load_next_line
      BCC   @done
      LDA   input_buf+0
      BEQ   @line                       ; blank line -> skip
      JSR   tokenize_line
      JSR   check_to_command            ; carry set = TO/EDIT handled
      BCS   @line
      JSR   eval_line
      BRA   @line
@done:
      RTS

; =====================================================================
; RODATA — procedure strings
; =====================================================================
      .segment "RODATA"




; "MAKE " + a double-quote — the prefix of each serialized global-variable line.
str_make_kw:
      .byte "MAKE ", $22

; Builtin name strings for workspace commands
str_po_name:
      .byte 2, "PO"
str_printout_name:
      .byte 8, "PRINTOUT"
str_pots_name:
      .byte 4, "POTS"
str_printtitles_name:
      .byte 11, "PRINTTITLES"
str_erase_name:
      .byte 5, "ERASE"
str_apply_name:
      .byte 5, "APPLY"

; lists.s — list data type, cons pairs, and list operations for NovaLogo
;
; Cons pair format on heap (7 bytes):
;   +0: tag byte ($10 = CONS_PAIR)
;   +1: car_type ($00=number, $01=word, $02=list)
;   +2: car_hi
;   +3: car_lo
;   +4: car_frac (only meaningful for numbers)
;   +5: cdr_lo (pointer to next cons pair, or $0000 for empty)
;   +6: cdr_hi

CONS_PAIR  = $10
CONS_SIZE  = 7

; Value type constants
.ifndef VAL_NUMBER
VAL_NUMBER = $00
.endif
.ifndef VAL_WORD
VAL_WORD   = $01
.endif
.ifndef VAL_LIST
VAL_LIST   = $02
.endif

; Cons pair field offsets
CONS_TAG      = 0
CONS_CAR_TYPE = 1
CONS_CAR_HI   = 2
CONS_CAR_LO   = 3
CONS_CAR_FRAC = 4
CONS_CDR_LO   = 5
CONS_CDR_HI   = 6

; =====================================================================
; ZEROPAGE segment — list scratch variables
; =====================================================================
      .segment "ZEROPAGE"

list_head_lo:   .res 1    ; head of list being built
list_head_hi:   .res 1
list_tail_lo:   .res 1    ; tail of list being built
list_tail_hi:   .res 1
list_ptr_lo:    .res 1    ; general list walk pointer
list_ptr_hi:    .res 1
list_count_lo:  .res 1    ; 16-bit counter for COUNT / ITEM
list_count_hi:  .res 1

; =====================================================================
; CODE segment — list building and operations
; =====================================================================
      .segment "CODE"

; ---------------------------------------------------------------------
; eval_list — build a list from tokens between [ and ]
;   Entry: eval_cur points at the first token AFTER the [
;   Exit:  eval_type = VAL_LIST, eval_val_lo/hi = head cons pair ptr
;          eval_cur advanced past the matching ]
;          Carry clear = success
; ---------------------------------------------------------------------
eval_list:
      STZ   list_head_lo
      STZ   list_head_hi
      STZ   list_tail_lo
      STZ   list_tail_hi

@next_element:
      LDA   eval_cur_lo
      ORA   eval_cur_hi
      BNE   :+
      JMP   @err_no_bracket
:
      LDA   eval_cur_lo
      STA   ptr_lo
      LDA   eval_cur_hi
      STA   ptr_hi
      LDY   #TOK_TAG
      LDA   (ptr_lo),Y

      CMP   #TOK_RBRACKET
      BNE   :+
      JMP   @end_list
:
      CMP   #TOK_NUMBER
      BNE   :+
      JMP   @list_number
:     CMP   #TOK_WORD
      BEQ   @list_word_jmp
      CMP   #TOK_QUOTE
      BEQ   @list_word_jmp
      CMP   #TOK_LBRACKET
      BNE   :+
      JMP   @list_sublist
:     ; Unknown token type in list — skip it
      JSR   eval_advance
      BRA   @next_element

@list_word_jmp:
      JMP   @list_word

@list_number:
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
      JMP   @alloc_cell

@list_word:
      LDY   #TOK_PAYLOAD
      LDA   (ptr_lo),Y
      PHA
      INC
      LDX   #ATYPE_STRING
      JSR   heap_alloc
      BCC   :+
      PLA
      JMP   @oom
:     LDA   eval_cur_lo
      STA   ptr2_lo
      LDA   eval_cur_hi
      STA   ptr2_hi
      PLA
      PHA
      LDY   #0
      STA   (ptr_lo),Y
      PLA
      TAX
      BEQ   @word_done
      LDY   #1
      STZ   num_tmp_lo
@copy_word_ch:
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
      BNE   @copy_word_ch
@word_done:
      LDA   ptr_lo
      STA   eval_val_lo
      LDA   ptr_hi
      STA   eval_val_hi
      STZ   eval_val_frac
      LDA   #VAL_WORD
      STA   eval_type
      JSR   eval_advance
      JMP   @alloc_cell

@list_sublist:
      JSR   eval_advance
      LDA   list_head_lo
      PHA
      LDA   list_head_hi
      PHA
      LDA   list_tail_lo
      PHA
      LDA   list_tail_hi
      PHA
      JSR   eval_list
      BCC   @sublist_ok
      ; Error — clean up stack
      PLA
      PLA
      PLA
      PLA
      SEC
      RTS
@sublist_ok:
      PLA
      STA   list_tail_hi
      PLA
      STA   list_tail_lo
      PLA
      STA   list_head_hi
      PLA
      STA   list_head_lo
      ; eval_type already VAL_LIST, eval_val has sublist ptr
      ; fall through to @alloc_cell

@alloc_cell:
      LDA   #CONS_SIZE
      LDX   #ATYPE_CONS
      JSR   heap_alloc
      BCS   @oom
      JSR   fill_cons_from_eval

      ; Link into list
      LDA   list_head_lo
      ORA   list_head_hi
      BNE   @link_tail

      LDA   ptr_lo
      STA   list_head_lo
      STA   list_tail_lo
      LDA   ptr_hi
      STA   list_head_hi
      STA   list_tail_hi
      JMP   @next_element

@link_tail:
      LDA   list_tail_lo
      STA   ptr2_lo
      LDA   list_tail_hi
      STA   ptr2_hi
      LDY   #CONS_CDR_LO
      LDA   ptr_lo
      STA   (ptr2_lo),Y
      LDY   #CONS_CDR_HI
      LDA   ptr_hi
      STA   (ptr2_lo),Y
      LDA   ptr_lo
      STA   list_tail_lo
      LDA   ptr_hi
      STA   list_tail_hi
      JMP   @next_element

@end_list:
      JSR   eval_advance
      LDA   #VAL_LIST
      STA   eval_type
      LDA   list_head_lo
      STA   eval_val_lo
      LDA   list_head_hi
      STA   eval_val_hi
      STZ   eval_val_frac
      CLC
      RTS

@err_no_bracket:
      SEC
      RTS

@oom:
      SEC
      RTS

; ---------------------------------------------------------------------
; print_list — print a list value to screen with brackets
;   Entry: eval_val_lo/hi = head cons pair pointer (or $0000)
;   Clobbers: A, X, Y, ptr_lo/hi, ptr2_lo/hi, list_ptr
; ---------------------------------------------------------------------
; Thin wrapper: render the list elements into input_buf via render_list_to_buf
; (same element formatting + nested brackets), wrap them in [ ] on the screen.
; Long lists (> ~126 chars of text) truncate at the input_buf bound.
print_list:
      LDA   #'['
      STA   VGC_CHAROUT
      STZ   z:buf_idx
      JSR   render_list_to_buf
      LDX   #0
@pl_emit:
      CPX   z:buf_idx
      BCS   @pl_done
      LDA   input_buf,X
      STA   VGC_CHAROUT
      INX
      BRA   @pl_emit
@pl_done:
      LDA   #']'
      STA   VGC_CHAROUT
      RTS

; ---------------------------------------------------------------------
; do_first — FIRST list: return the first element
;   Arity 0: evaluates its own argument
; ---------------------------------------------------------------------
do_first:
      JSR   eval_expr
      BCC   :+
      JMP   @err_args
:
      LDA   eval_type
      CMP   #VAL_LIST
      BNE   @err_type

      LDA   eval_val_lo
      ORA   eval_val_hi
      BEQ   @err_empty

      LDA   eval_val_lo
      STA   ptr_lo
      LDA   eval_val_hi
      STA   ptr_hi
      LDY   #CONS_CAR_TYPE
      LDA   (ptr_lo),Y
      STA   eval_type
      LDY   #CONS_CAR_HI
      LDA   (ptr_lo),Y
      STA   eval_val_hi
      LDY   #CONS_CAR_LO
      LDA   (ptr_lo),Y
      STA   eval_val_lo
      LDY   #CONS_CAR_FRAC
      LDA   (ptr_lo),Y
      STA   eval_val_frac
      JMP   eval_continue

@err_args:
      LDX   #<str_first_name
      LDY   #>str_first_name
      JMP   err_nei
@err_type:
      LDX   #<str_first_name
      LDY   #>str_first_name
      JMP   err_doesnt_like
@err_empty:
      LDX   #<str_first_name
      LDY   #>str_first_name
      JMP   err_dl_empty

; ---------------------------------------------------------------------
; do_butfirst / do_bf — BUTFIRST list: return all but first element
;   Arity 0: evaluates its own argument
; ---------------------------------------------------------------------
do_butfirst:
      JSR   eval_expr
      BCC   :+
      JMP   @err_args
:
      LDA   eval_type
      CMP   #VAL_LIST
      BNE   @err_type

      LDA   eval_val_lo
      ORA   eval_val_hi
      BEQ   @err_empty

      LDA   eval_val_lo
      STA   ptr_lo
      LDA   eval_val_hi
      STA   ptr_hi
      LDY   #CONS_CDR_LO
      LDA   (ptr_lo),Y
      STA   eval_val_lo
      LDY   #CONS_CDR_HI
      LDA   (ptr_lo),Y
      STA   eval_val_hi
      LDA   #VAL_LIST
      STA   eval_type
      STZ   eval_val_frac
      JMP   eval_continue

@err_args:
      LDX   #<str_butfirst_name
      LDY   #>str_butfirst_name
      JMP   err_nei
@err_type:
      LDX   #<str_butfirst_name
      LDY   #>str_butfirst_name
      JMP   err_doesnt_like
@err_empty:
      LDX   #<str_butfirst_name
      LDY   #>str_butfirst_name
      JMP   err_dl_empty

; ---------------------------------------------------------------------
; do_count — COUNT list: return the number of elements
;   Arity 0: evaluates its own argument
; ---------------------------------------------------------------------
do_count:
      JSR   eval_expr
      BCC   :+
      JMP   @err_args
:
      LDA   eval_type
      CMP   #VAL_LIST
      BNE   @err_type

      STZ   list_count_lo
      STZ   list_count_hi
      LDA   eval_val_lo
      STA   list_ptr_lo
      LDA   eval_val_hi
      STA   list_ptr_hi

@walk:
      LDA   list_ptr_lo
      ORA   list_ptr_hi
      BEQ   @walk_done

      INC   list_count_lo
      BNE   :+
      INC   list_count_hi
:
      LDA   list_ptr_lo
      STA   ptr_lo
      LDA   list_ptr_hi
      STA   ptr_hi
      LDY   #CONS_CDR_LO
      LDA   (ptr_lo),Y
      TAX
      LDY   #CONS_CDR_HI
      LDA   (ptr_lo),Y
      STA   list_ptr_hi
      STX   list_ptr_lo
      BRA   @walk

@walk_done:
      STZ   eval_type
      LDA   list_count_hi
      STA   eval_val_hi
      LDA   list_count_lo
      STA   eval_val_lo
      STZ   eval_val_frac
      JMP   eval_continue

@err_args:
      LDX   #<str_count_name
      LDY   #>str_count_name
      JMP   err_nei
@err_type:
      LDX   #<str_count_name
      LDY   #>str_count_name
      JMP   err_doesnt_like

; ---------------------------------------------------------------------
; do_emptyp — EMPTY? value: return 1 if empty list, else 0
;   Arity 0: evaluates its own argument
; ---------------------------------------------------------------------
do_emptyp:
      JSR   eval_expr
      BCC   :+
      LDX   #<str_emptyp_name
      LDY   #>str_emptyp_name
      JMP   err_nei
:
      ; Check for empty list (type=list and pointer=$0000)
      LDA   eval_type
      CMP   #VAL_LIST
      BNE   @not_empty

      LDA   eval_val_lo
      ORA   eval_val_hi
      BNE   @not_empty

      ; Empty list
      STZ   eval_type
      LDA   #1
      STA   eval_val_lo
      STZ   eval_val_hi
      STZ   eval_val_frac
      JMP   eval_continue

@not_empty:
      STZ   eval_type
      STZ   eval_val_lo
      STZ   eval_val_hi
      STZ   eval_val_frac
      JMP   eval_continue

; ---------------------------------------------------------------------
; do_last — LAST list: return the last element
;   Arity 0: evaluates its own argument
; ---------------------------------------------------------------------
do_last:
      JSR   eval_expr
      BCC   :+
      JMP   @err_args
:
      LDA   eval_type
      CMP   #VAL_LIST
      BNE   @err_type

      LDA   eval_val_lo
      ORA   eval_val_hi
      BEQ   @err_empty

      LDA   eval_val_lo
      STA   list_ptr_lo
      LDA   eval_val_hi
      STA   list_ptr_hi

@walk:
      LDA   list_ptr_lo
      STA   ptr_lo
      LDA   list_ptr_hi
      STA   ptr_hi
      LDY   #CONS_CDR_LO
      LDA   (ptr_lo),Y
      TAX
      LDY   #CONS_CDR_HI
      LDA   (ptr_lo),Y
      ORA   #0
      BNE   @not_last
      CPX   #0
      BEQ   @found_last
@not_last:
      STA   list_ptr_hi
      STX   list_ptr_lo
      BRA   @walk

@found_last:
      LDY   #CONS_CAR_TYPE
      LDA   (ptr_lo),Y
      STA   eval_type
      LDY   #CONS_CAR_HI
      LDA   (ptr_lo),Y
      STA   eval_val_hi
      LDY   #CONS_CAR_LO
      LDA   (ptr_lo),Y
      STA   eval_val_lo
      LDY   #CONS_CAR_FRAC
      LDA   (ptr_lo),Y
      STA   eval_val_frac
      JMP   eval_continue

@err_args:
      LDX   #<str_last_name
      LDY   #>str_last_name
      JMP   err_nei
@err_type:
      LDX   #<str_last_name
      LDY   #>str_last_name
      JMP   err_doesnt_like
@err_empty:
      LDX   #<str_last_name
      LDY   #>str_last_name
      JMP   err_dl_empty

; ---------------------------------------------------------------------
; do_butlast — BUTLAST list: all but the last element
;   Arity 0: evaluates its own argument
; ---------------------------------------------------------------------
do_butlast:
      JSR   eval_expr
      BCC   :+
      JMP   @err_args
:
      LDA   eval_type
      CMP   #VAL_LIST
      BEQ   :+
      JMP   @err_type
:
      LDA   eval_val_lo
      ORA   eval_val_hi
      BNE   :+
      JMP   @err_empty
:

      ; Single element? (cdr = null → return empty list)
      LDA   eval_val_lo
      STA   ptr_lo
      LDA   eval_val_hi
      STA   ptr_hi
      LDY   #CONS_CDR_LO
      LDA   (ptr_lo),Y
      TAX
      LDY   #CONS_CDR_HI
      LDA   (ptr_lo),Y
      ORA   #0
      BNE   @multi
      CPX   #0
      BNE   @multi
      JMP   @return_empty

@multi:
      STZ   list_head_lo
      STZ   list_head_hi
      STZ   list_tail_lo
      STZ   list_tail_hi

      LDA   eval_val_lo
      STA   list_ptr_lo
      LDA   eval_val_hi
      STA   list_ptr_hi

@bl_loop:
      ; If cdr of current cell is null, this is the last — stop
      LDA   list_ptr_lo
      STA   ptr_lo
      LDA   list_ptr_hi
      STA   ptr_hi
      LDY   #CONS_CDR_LO
      LDA   (ptr_lo),Y
      TAX
      LDY   #CONS_CDR_HI
      LDA   (ptr_lo),Y
      STX   num_tmp_lo
      STA   num_tmp_hi
      ORA   num_tmp_lo
      BNE   :+
      JMP   @bl_done
:
      ; Allocate new cons pair and copy car
      LDA   #CONS_SIZE
      LDX   #ATYPE_CONS
      JSR   heap_alloc
      BCC   :+
      JMP   @err_args           ; OOM
:
      ; Re-read source cell (ptr_lo/hi was clobbered by heap_alloc)
      LDA   list_ptr_lo
      STA   ptr2_lo
      LDA   list_ptr_hi
      STA   ptr2_hi
      LDY   #CONS_TAG
      LDA   #CONS_PAIR
      STA   (ptr_lo),Y
      LDY   #CONS_CAR_TYPE
      LDA   (ptr2_lo),Y
      STA   (ptr_lo),Y
      LDY   #CONS_CAR_HI
      LDA   (ptr2_lo),Y
      STA   (ptr_lo),Y
      LDY   #CONS_CAR_LO
      LDA   (ptr2_lo),Y
      STA   (ptr_lo),Y
      LDY   #CONS_CAR_FRAC
      LDA   (ptr2_lo),Y
      STA   (ptr_lo),Y
      LDY   #CONS_CDR_LO
      LDA   #0
      STA   (ptr_lo),Y
      LDY   #CONS_CDR_HI
      STA   (ptr_lo),Y

      ; Link into new list
      LDA   list_head_lo
      ORA   list_head_hi
      BNE   @bl_link
      ; First cell
      LDA   ptr_lo
      STA   list_head_lo
      STA   list_tail_lo
      LDA   ptr_hi
      STA   list_head_hi
      STA   list_tail_hi
      BRA   @bl_next
@bl_link:
      LDA   list_tail_lo
      STA   ptr2_lo
      LDA   list_tail_hi
      STA   ptr2_hi
      LDY   #CONS_CDR_LO
      LDA   ptr_lo
      STA   (ptr2_lo),Y
      LDY   #CONS_CDR_HI
      LDA   ptr_hi
      STA   (ptr2_lo),Y
      LDA   ptr_lo
      STA   list_tail_lo
      LDA   ptr_hi
      STA   list_tail_hi

@bl_next:
      LDA   num_tmp_lo
      STA   list_ptr_lo
      LDA   num_tmp_hi
      STA   list_ptr_hi
      JMP   @bl_loop

@bl_done:
      LDA   #VAL_LIST
      STA   eval_type
      LDA   list_head_lo
      STA   eval_val_lo
      LDA   list_head_hi
      STA   eval_val_hi
      STZ   eval_val_frac
      JMP   eval_continue

@return_empty:
      LDA   #VAL_LIST
      STA   eval_type
      STZ   eval_val_lo
      STZ   eval_val_hi
      STZ   eval_val_frac
      JMP   eval_continue

@err_args:
      LDX   #<str_butlast_name
      LDY   #>str_butlast_name
      JMP   err_nei
@err_type:
      LDX   #<str_butlast_name
      LDY   #>str_butlast_name
      JMP   err_doesnt_like
@err_empty:
      LDX   #<str_butlast_name
      LDY   #>str_butlast_name
      JMP   err_dl_empty

; ---------------------------------------------------------------------
; do_item — ITEM index list: return the Nth element (1-based)
;   Arity 0: evaluates its own arguments
; ---------------------------------------------------------------------
do_item:
      JSR   eval_expr
      BCC   :+
      JMP   @err_args
:     LDA   eval_type
      BNE   @err_args_j         ; must be number

      LDA   eval_val_lo
      PHA
      LDA   eval_val_hi
      PHA

      JSR   eval_expr
      BCC   :+
      PLA
      PLA
      JMP   @err_args
:
      LDA   eval_type
      CMP   #VAL_LIST
      BNE   @err_type_pop2

      PLA
      STA   list_count_hi
      PLA
      STA   list_count_lo

      ; Index must be >= 1
      LDA   list_count_lo
      ORA   list_count_hi
      BEQ   @err_range

      LDA   eval_val_lo
      STA   list_ptr_lo
      LDA   eval_val_hi
      STA   list_ptr_hi

@item_walk:
      LDA   list_ptr_lo
      ORA   list_ptr_hi
      BEQ   @err_range

      ; Decrement index
      LDA   list_count_lo
      BNE   :+
      DEC   list_count_hi
:     DEC   list_count_lo

      LDA   list_count_lo
      ORA   list_count_hi
      BEQ   @item_found

      ; Follow cdr
      LDA   list_ptr_lo
      STA   ptr_lo
      LDA   list_ptr_hi
      STA   ptr_hi
      LDY   #CONS_CDR_LO
      LDA   (ptr_lo),Y
      TAX
      LDY   #CONS_CDR_HI
      LDA   (ptr_lo),Y
      STA   list_ptr_hi
      STX   list_ptr_lo
      BRA   @item_walk

@item_found:
      LDA   list_ptr_lo
      STA   ptr_lo
      LDA   list_ptr_hi
      STA   ptr_hi
      LDY   #CONS_CAR_TYPE
      LDA   (ptr_lo),Y
      STA   eval_type
      LDY   #CONS_CAR_HI
      LDA   (ptr_lo),Y
      STA   eval_val_hi
      LDY   #CONS_CAR_LO
      LDA   (ptr_lo),Y
      STA   eval_val_lo
      LDY   #CONS_CAR_FRAC
      LDA   (ptr_lo),Y
      STA   eval_val_frac
      JMP   eval_continue

@err_args_j:
      JMP   @err_args

@err_type_pop2:
      PLA
      PLA
      LDX   #<str_item_name
      LDY   #>str_item_name
      JMP   err_needs_list
@err_args:
      LDX   #<str_item_name
      LDY   #>str_item_name
      JMP   err_nei
@err_range:
      LDX   #<str_item_name
      LDY   #>str_item_name
      JMP   err_index_range

; ---------------------------------------------------------------------
; do_memberp — MEMBER? value list: return 1 if value in list, else 0
;   Arity 0: evaluates its own arguments
; ---------------------------------------------------------------------
do_memberp:
      JSR   eval_expr
      BCC   :+
      JMP   @err
:
      ; Save search value on stack (type + 3 value bytes)
      LDA   eval_val_frac
      PHA
      LDA   eval_val_lo
      PHA
      LDA   eval_val_hi
      PHA
      LDA   eval_type
      PHA

      JSR   eval_expr
      BCC   :+
      PLA
      PLA
      PLA
      PLA
      JMP   @err
:
      LDA   eval_type
      CMP   #VAL_LIST
      BEQ   :+
      JMP   @err_type_pop4
:
      LDA   eval_val_lo
      STA   list_ptr_lo
      LDA   eval_val_hi
      STA   list_ptr_hi

@mem_walk:
      LDA   list_ptr_lo
      ORA   list_ptr_hi
      BEQ   @mem_false

      LDA   list_ptr_lo
      STA   ptr_lo
      LDA   list_ptr_hi
      STA   ptr_hi

      ; Compare type
      LDY   #CONS_CAR_TYPE
      LDA   (ptr_lo),Y
      TSX
      CMP   $0101,X
      BNE   @mem_next

      ; Types match — compare values
      CMP   #VAL_NUMBER
      BNE   @mem_cmp_ptr

      ; Compare number: hi, lo, frac
      LDY   #CONS_CAR_HI
      LDA   (ptr_lo),Y
      CMP   $0102,X
      BNE   @mem_next
      LDY   #CONS_CAR_LO
      LDA   (ptr_lo),Y
      CMP   $0103,X
      BNE   @mem_next
      LDY   #CONS_CAR_FRAC
      LDA   (ptr_lo),Y
      CMP   $0104,X
      BNE   @mem_next
      BRA   @mem_true

@mem_cmp_ptr:
      LDY   #CONS_CAR_HI
      LDA   (ptr_lo),Y
      CMP   $0102,X
      BNE   @mem_next
      LDY   #CONS_CAR_LO
      LDA   (ptr_lo),Y
      CMP   $0103,X
      BNE   @mem_next
      ; fall through to @mem_true

@mem_true:
      PLA
      PLA
      PLA
      PLA
      STZ   eval_type
      LDA   #1
      STA   eval_val_lo
      STZ   eval_val_hi
      STZ   eval_val_frac
      JMP   eval_continue

@mem_next:
      LDA   list_ptr_lo
      STA   ptr_lo
      LDA   list_ptr_hi
      STA   ptr_hi
      LDY   #CONS_CDR_LO
      LDA   (ptr_lo),Y
      TAX
      LDY   #CONS_CDR_HI
      LDA   (ptr_lo),Y
      STA   list_ptr_hi
      STX   list_ptr_lo
      BRA   @mem_walk

@mem_false:
      PLA
      PLA
      PLA
      PLA
      STZ   eval_type
      STZ   eval_val_lo
      STZ   eval_val_hi
      STZ   eval_val_frac
      JMP   eval_continue

@err:
      LDX   #<str_memberp_name
      LDY   #>str_memberp_name
      JMP   err_nei
@err_type_pop4:
      PLA
      PLA
      PLA
      PLA
      LDX   #<str_memberp_name
      LDY   #>str_memberp_name
      JMP   err_needs_list

; ---------------------------------------------------------------------
; do_show — SHOW value: like PRINT, adds brackets for lists
;   Arity 0: evaluates its own argument
; ---------------------------------------------------------------------
do_show:
      JSR   eval_expr
      BCC   :+
      LDX   #<str_show_name
      LDY   #>str_show_name
      JMP   err_nei
:
      LDA   eval_type
      CMP   #VAL_LIST
      BEQ   @show_list
      CMP   #VAL_WORD
      BEQ   @show_word

      ; Number
      JSR   print_number
      JSR   eval_newline
      JMP   eval_continue

@show_word:
      LDA   eval_val_lo
      STA   ptr_lo
      LDA   eval_val_hi
      STA   ptr_hi
      LDY   #0
      LDA   (ptr_lo),Y
      TAX
      BEQ   @sw_done
      INY
@sw_ch:
      LDA   (ptr_lo),Y
      STA   VGC_CHAROUT
      INY
      DEX
      BNE   @sw_ch
@sw_done:
      JSR   eval_newline
      JMP   eval_continue

@show_list:
      JSR   print_list
      JSR   eval_newline
      JMP   eval_continue

; ---------------------------------------------------------------------
; list_print_err — shared error printer for list operations
;   Entry: X/Y = lo/hi pointer to null-terminated error string
;   JMPs to eval_continue
; ---------------------------------------------------------------------
list_print_err:
      JSR   print_cstr_xy        ; print the cstring at X/Y
err_nl_continue:                 ; shared tail: newline + abandon line (no throw)
      JSR   eval_newline
      JMP   eval_continue

; ---------------------------------------------------------------------
; do_fput — FPUT value list: prepend value to front of list
;   Arity 0: evaluates its own arguments
; ---------------------------------------------------------------------
do_fput:
      JSR   eval_expr
      BCC   :+
      JMP   @err
:
      ; Save value on stack (type, hi, lo, frac)
      LDA   eval_type
      PHA
      LDA   eval_val_hi
      PHA
      LDA   eval_val_lo
      PHA
      LDA   eval_val_frac
      PHA

      ; Evaluate second arg: must be a list
      JSR   eval_expr
      BCC   :+
      JMP   @err_pop4
:
      LDA   eval_type
      CMP   #VAL_LIST
      BNE   @err_type_pop4

      ; Save the list pointer (will be cdr of new cell)
      LDA   eval_val_lo
      PHA
      LDA   eval_val_hi
      PHA

      ; Allocate cons pair
      LDA   #CONS_SIZE
      LDX   #ATYPE_CONS
      JSR   heap_alloc
      BCS   @oom_pop6

      ; Pop list pointer → cdr
      PLA
      STA   num_tmp_hi              ; list_hi
      PLA
      STA   num_tmp_lo              ; list_lo

      ; Pop value → car
      PLA
      STA   eval_val_frac
      PLA
      STA   eval_val_lo
      PLA
      STA   eval_val_hi
      PLA
      STA   eval_type

      ; Fill the cons pair
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
      LDA   num_tmp_lo
      STA   (ptr_lo),Y
      LDY   #CONS_CDR_HI
      LDA   num_tmp_hi
      STA   (ptr_lo),Y

      ; Return the new list
      LDA   #VAL_LIST
      STA   eval_type
      LDA   ptr_lo
      STA   eval_val_lo
      LDA   ptr_hi
      STA   eval_val_hi
      STZ   eval_val_frac
      JMP   eval_continue

@oom_pop6:
      PLA
      PLA
@err_type_pop4:
@err_pop4:
      PLA
      PLA
      PLA
      PLA
@err:
      LDX   #<str_fput_name
      LDY   #>str_fput_name
      JMP   err_nei

; ---------------------------------------------------------------------
; do_lput — LPUT value list: append value to end of list
;   Arity 0: evaluates its own arguments
; ---------------------------------------------------------------------
do_lput:
      JSR   eval_expr
      BCC   :+
      JMP   @err
:
      ; Save value on stack
      LDA   eval_type
      PHA
      LDA   eval_val_hi
      PHA
      LDA   eval_val_lo
      PHA
      LDA   eval_val_frac
      PHA

      ; Evaluate second arg: must be a list
      JSR   eval_expr
      BCC   :+
      JMP   @err_pop4
:
      LDA   eval_type
      CMP   #VAL_LIST
      BEQ   :+
      JMP   @err_type_pop4
:
      ; Save list pointer
      LDA   eval_val_lo
      STA   list_head_lo
      LDA   eval_val_hi
      STA   list_head_hi

      ; Pop value back
      PLA
      STA   eval_val_frac
      PLA
      STA   eval_val_lo
      PLA
      STA   eval_val_hi
      PLA
      STA   eval_type

      ; Allocate new tail cons pair for the value
      LDA   eval_type
      PHA
      LDA   eval_val_hi
      PHA
      LDA   eval_val_lo
      PHA
      LDA   eval_val_frac
      PHA

      LDA   #CONS_SIZE
      LDX   #ATYPE_CONS
      JSR   heap_alloc
      BCC   :+
      JMP   @oom2_pop4
:

      ; Pop value
      PLA
      STA   eval_val_frac
      PLA
      STA   eval_val_lo
      PLA
      STA   eval_val_hi
      PLA
      STA   eval_type

      ; Fill the new tail cell
      JSR   fill_cons_from_eval

      ; Save new cell pointer
      LDA   ptr_lo
      STA   list_tail_lo
      LDA   ptr_hi
      STA   list_tail_hi

      ; If list was empty, new cell IS the list
      LDA   list_head_lo
      ORA   list_head_hi
      BEQ   @lput_single

      ; Walk to last cell of existing list
      LDA   list_head_lo
      STA   list_ptr_lo
      LDA   list_head_hi
      STA   list_ptr_hi
@lput_walk:
      LDA   list_ptr_lo
      STA   ptr_lo
      LDA   list_ptr_hi
      STA   ptr_hi
      LDY   #CONS_CDR_LO
      LDA   (ptr_lo),Y
      TAX
      LDY   #CONS_CDR_HI
      LDA   (ptr_lo),Y
      ORA   #0
      BNE   @lput_next
      CPX   #0
      BEQ   @lput_found_last
@lput_next:
      STA   list_ptr_hi
      STX   list_ptr_lo
      BRA   @lput_walk

@lput_found_last:
      ; Patch last cell's cdr to point to new tail
      LDY   #CONS_CDR_LO
      LDA   list_tail_lo
      STA   (ptr_lo),Y
      LDY   #CONS_CDR_HI
      LDA   list_tail_hi
      STA   (ptr_lo),Y

      ; Return original list head
      LDA   #VAL_LIST
      STA   eval_type
      LDA   list_head_lo
      STA   eval_val_lo
      LDA   list_head_hi
      STA   eval_val_hi
      STZ   eval_val_frac
      JMP   eval_continue

@lput_single:
      ; Empty list → just return the new cell
      LDA   #VAL_LIST
      STA   eval_type
      LDA   list_tail_lo
      STA   eval_val_lo
      LDA   list_tail_hi
      STA   eval_val_hi
      STZ   eval_val_frac
      JMP   eval_continue

@oom2_pop4:
      PLA
      PLA
      PLA
      PLA
@err_type_pop4:
@err_pop4:
      PLA
      PLA
      PLA
      PLA
@err:
      LDX   #<str_lput_name
      LDY   #>str_lput_name
      JMP   err_nei

; ---------------------------------------------------------------------
; do_list — LIST val1 val2: create a two-element list
;   Arity 0: evaluates its own arguments
; ---------------------------------------------------------------------
do_list:
      ; Evaluate first arg
      JSR   eval_expr
      BCC   :+
      JMP   @err
:
      ; Save first value
      LDA   eval_type
      PHA
      LDA   eval_val_hi
      PHA
      LDA   eval_val_lo
      PHA
      LDA   eval_val_frac
      PHA

      ; Evaluate second arg
      JSR   eval_expr
      BCC   :+
      JMP   @err_pop4
:
      ; Save second value
      LDA   eval_type
      PHA
      LDA   eval_val_hi
      PHA
      LDA   eval_val_lo
      PHA
      LDA   eval_val_frac
      PHA

      ; Allocate second cell first (it has cdr=null)
      LDA   #CONS_SIZE
      LDX   #ATYPE_CONS
      JSR   heap_alloc
      BCC   :+
      JMP   @oom_pop8
:

      ; Pop second value into cell
      PLA
      PHA                           ; keep frac on stack briefly
      LDY   #CONS_TAG
      LDA   #CONS_PAIR
      STA   (ptr_lo),Y
      ; type
      LDY   #8                      ; stack offset for second type
      TSX
      LDA   $0104,X                 ; second val type (stack: frac,lo,hi,type,...)
      LDY   #CONS_CAR_TYPE
      STA   (ptr_lo),Y
      ; hi
      TSX
      LDA   $0103,X
      LDY   #CONS_CAR_HI
      STA   (ptr_lo),Y
      ; lo
      TSX
      LDA   $0102,X
      LDY   #CONS_CAR_LO
      STA   (ptr_lo),Y
      ; frac
      PLA                           ; restore frac
      LDY   #CONS_CAR_FRAC
      STA   (ptr_lo),Y
      ; cdr = null
      LDY   #CONS_CDR_LO
      LDA   #0
      STA   (ptr_lo),Y
      LDY   #CONS_CDR_HI
      STA   (ptr_lo),Y

      ; Clean remaining 3 bytes of second value from stack
      PLA
      PLA
      PLA

      ; Save second cell pointer
      LDA   ptr_lo
      STA   list_tail_lo
      LDA   ptr_hi
      STA   list_tail_hi

      ; Allocate first cell
      LDA   #CONS_SIZE
      LDX   #ATYPE_CONS
      JSR   heap_alloc
      BCS   @oom_pop4

      ; Pop first value into cell
      PLA
      STA   eval_val_frac
      PLA
      STA   eval_val_lo
      PLA
      STA   eval_val_hi
      PLA
      STA   eval_type

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
      ; cdr = second cell
      LDY   #CONS_CDR_LO
      LDA   list_tail_lo
      STA   (ptr_lo),Y
      LDY   #CONS_CDR_HI
      LDA   list_tail_hi
      STA   (ptr_lo),Y

      ; Return first cell as list head
      LDA   #VAL_LIST
      STA   eval_type
      LDA   ptr_lo
      STA   eval_val_lo
      LDA   ptr_hi
      STA   eval_val_hi
      STZ   eval_val_frac
      JMP   eval_continue

@oom_pop8:
      PLA
      PLA
      PLA
      PLA
@oom_pop4:
@err_pop4:
      PLA
      PLA
      PLA
      PLA
@err:
      LDX   #<str_list_name
      LDY   #>str_list_name
      JMP   err_nei

; ---------------------------------------------------------------------
; do_sentence — SENTENCE val1 val2: concatenate two values as flat list
;   Non-list args are wrapped as single-element lists first.
;   Arity 0: evaluates its own arguments
; ---------------------------------------------------------------------
do_sentence:
      ; Evaluate first arg
      JSR   eval_expr
      BCC   :+
      JMP   @err
:
      ; Save first value
      LDA   eval_type
      PHA
      LDA   eval_val_hi
      PHA
      LDA   eval_val_lo
      PHA
      LDA   eval_val_frac
      PHA

      ; Evaluate second arg
      JSR   eval_expr
      BCC   :+
      JMP   @err_pop4
:
      ; Save second value
      LDA   eval_type
      PHA
      LDA   eval_val_hi
      PHA
      LDA   eval_val_lo
      PHA
      LDA   eval_val_frac
      PHA

      ; Process first arg: if not list, wrap in single-element list
      ; First value is at stack offset 4..7 (under second value)
      TSX
      LDA   $0108,X                 ; first type
      CMP   #VAL_LIST
      BEQ   @first_is_list

      ; Wrap first value as single-element list
      LDA   #CONS_SIZE
      LDX   #ATYPE_CONS
      JSR   heap_alloc
      BCC   :+
      JMP   @oom_pop8
:
      LDY   #CONS_TAG
      LDA   #CONS_PAIR
      STA   (ptr_lo),Y
      TSX
      LDA   $0108,X                 ; first type
      LDY   #CONS_CAR_TYPE
      STA   (ptr_lo),Y
      LDA   $0107,X                 ; first hi
      LDY   #CONS_CAR_HI
      STA   (ptr_lo),Y
      LDA   $0106,X                 ; first lo
      LDY   #CONS_CAR_LO
      STA   (ptr_lo),Y
      LDA   $0105,X                 ; first frac
      LDY   #CONS_CAR_FRAC
      STA   (ptr_lo),Y
      LDY   #CONS_CDR_LO
      LDA   #0
      STA   (ptr_lo),Y
      LDY   #CONS_CDR_HI
      STA   (ptr_lo),Y
      ; Update first value on stack to be VAL_LIST
      LDA   #VAL_LIST
      STA   $0108,X                 ; type = list
      LDA   ptr_hi
      STA   $0107,X                 ; hi = ptr_hi
      LDA   ptr_lo
      STA   $0106,X                 ; lo = ptr_lo
      LDA   #0
      STA   $0105,X                 ; frac = 0

@first_is_list:
      ; Process second arg: if not list, wrap it
      TSX
      LDA   $0104,X                 ; second type
      CMP   #VAL_LIST
      BEQ   @second_is_list

      ; Wrap second value
      LDA   #CONS_SIZE
      LDX   #ATYPE_CONS
      JSR   heap_alloc
      BCC   :+
      JMP   @oom_pop8
:
      LDY   #CONS_TAG
      LDA   #CONS_PAIR
      STA   (ptr_lo),Y
      TSX
      LDA   $0104,X                 ; second type
      LDY   #CONS_CAR_TYPE
      STA   (ptr_lo),Y
      LDA   $0103,X                 ; second hi
      LDY   #CONS_CAR_HI
      STA   (ptr_lo),Y
      LDA   $0102,X                 ; second lo
      LDY   #CONS_CAR_LO
      STA   (ptr_lo),Y
      LDA   $0101,X                 ; second frac
      LDY   #CONS_CAR_FRAC
      STA   (ptr_lo),Y
      LDY   #CONS_CDR_LO
      LDA   #0
      STA   (ptr_lo),Y
      LDY   #CONS_CDR_HI
      STA   (ptr_lo),Y
      ; Update second value on stack
      LDA   #VAL_LIST
      STA   $0104,X
      LDA   ptr_hi
      STA   $0103,X
      LDA   ptr_lo
      STA   $0102,X
      LDA   #0
      STA   $0101,X

@second_is_list:
      ; Pop second list pointer
      PLA                           ; frac (unused)
      PLA                           ; lo
      STA   list_tail_lo
      PLA                           ; hi
      STA   list_tail_hi
      PLA                           ; type (unused, known VAL_LIST)

      ; Pop first list pointer
      PLA                           ; frac (unused)
      PLA                           ; lo
      STA   list_head_lo
      PLA                           ; hi
      STA   list_head_hi
      PLA                           ; type (unused)

      ; If first list is empty, return second
      LDA   list_head_lo
      ORA   list_head_hi
      BEQ   @se_return_second

      ; Walk to end of first list, patch cdr to second
      LDA   list_head_lo
      STA   list_ptr_lo
      LDA   list_head_hi
      STA   list_ptr_hi
@se_walk:
      LDA   list_ptr_lo
      STA   ptr_lo
      LDA   list_ptr_hi
      STA   ptr_hi
      LDY   #CONS_CDR_LO
      LDA   (ptr_lo),Y
      TAX
      LDY   #CONS_CDR_HI
      LDA   (ptr_lo),Y
      ORA   #0
      BNE   @se_next
      CPX   #0
      BEQ   @se_found_last
@se_next:
      STA   list_ptr_hi
      STX   list_ptr_lo
      BRA   @se_walk

@se_found_last:
      ; Patch last cell's cdr to second list
      LDY   #CONS_CDR_LO
      LDA   list_tail_lo
      STA   (ptr_lo),Y
      LDY   #CONS_CDR_HI
      LDA   list_tail_hi
      STA   (ptr_lo),Y

      ; Return first list head
      LDA   #VAL_LIST
      STA   eval_type
      LDA   list_head_lo
      STA   eval_val_lo
      LDA   list_head_hi
      STA   eval_val_hi
      STZ   eval_val_frac
      JMP   eval_continue

@se_return_second:
      LDA   #VAL_LIST
      STA   eval_type
      LDA   list_tail_lo
      STA   eval_val_lo
      LDA   list_tail_hi
      STA   eval_val_hi
      STZ   eval_val_frac
      JMP   eval_continue

@oom_pop8:
      PLA
      PLA
      PLA
      PLA
@err_pop4:
      PLA
      PLA
      PLA
      PLA
@err:
      LDX   #<str_sentence_name
      LDY   #>str_sentence_name
      JMP   err_nei

; ---------------------------------------------------------------------
; do_word — WORD val1 val2: concatenate two words into one
;   Arity 0: evaluates its own arguments
; ---------------------------------------------------------------------
do_word:
      ; Evaluate first arg (must be a word)
      JSR   eval_expr
      BCC   :+
      JMP   @err
:
      LDA   eval_type
      CMP   #VAL_WORD
      BEQ   :+
      JMP   @err_type
:
      ; Save first word pointer
      LDA   eval_val_lo
      PHA
      LDA   eval_val_hi
      PHA

      ; Evaluate second arg (must be a word)
      JSR   eval_expr
      BCC   :+
      PLA
      PLA
      JMP   @err
:
      LDA   eval_type
      CMP   #VAL_WORD
      BEQ   :+
      JMP   @err_type_pop2
:

      ; eval_val = second word, stack has first word ptr
      ; Read lengths
      LDA   eval_val_lo
      STA   ptr2_lo
      LDA   eval_val_hi
      STA   ptr2_hi
      LDY   #0
      LDA   (ptr2_lo),Y            ; second word length
      STA   list_count_lo           ; save len2

      ; Read first word length from stack
      PLA
      STA   list_head_hi            ; first word ptr hi
      PLA
      STA   list_head_lo            ; first word ptr lo
      LDA   list_head_lo
      STA   ptr_lo
      LDA   list_head_hi
      STA   ptr_hi
      LDY   #0
      LDA   (ptr_lo),Y             ; first word length
      STA   list_count_hi           ; save len1

      ; Allocate new string: 1 (length byte) + len1 + len2
      CLC
      ADC   list_count_lo
      INC                           ; +1 for length byte
      LDX   #ATYPE_STRING
      JSR   heap_alloc
      BCS   @err_oom

      ; Write combined length
      LDA   list_count_hi
      CLC
      ADC   list_count_lo
      LDY   #0
      STA   (ptr_lo),Y

      ; Copy first word's chars
      LDX   list_count_hi           ; len1
      BEQ   @w_copy2
      LDA   list_head_lo
      STA   ptr2_lo
      LDA   list_head_hi
      STA   ptr2_hi
      LDY   #1                     ; dest offset in new string
      STZ   num_tmp_lo              ; source offset
@w_c1:
      PHX
      PHY
      LDA   num_tmp_lo
      INC                           ; skip length byte in source
      TAY
      LDA   (ptr2_lo),Y            ; source char
      PLY
      STA   (ptr_lo),Y             ; dest char
      INY
      INC   num_tmp_lo
      PLX
      DEX
      BNE   @w_c1

      ; Y is already at the next dest offset
      BRA   @w_copy2_start

@w_copy2:
      LDY   #1                     ; start at offset 1 if len1 was 0
@w_copy2_start:
      ; Copy second word's chars
      LDX   list_count_lo           ; len2
      BEQ   @w_done
      LDA   eval_val_lo
      STA   ptr2_lo
      LDA   eval_val_hi
      STA   ptr2_hi
      STZ   num_tmp_lo
@w_c2:
      PHX
      PHY
      LDA   num_tmp_lo
      INC
      TAY
      LDA   (ptr2_lo),Y
      PLY
      STA   (ptr_lo),Y
      INY
      INC   num_tmp_lo
      PLX
      DEX
      BNE   @w_c2

@w_done:
      ; Return the new word
      LDA   #VAL_WORD
      STA   eval_type
      LDA   ptr_lo
      STA   eval_val_lo
      LDA   ptr_hi
      STA   eval_val_hi
      STZ   eval_val_frac
      JMP   eval_continue

@err_type_pop2:
      PLA
      PLA
@err_type:
      LDX   #<str_word_name
      LDY   #>str_word_name
      JMP   err_dl_this
@err_oom:
@err:
      LDX   #<str_word_name
      LDY   #>str_word_name
      JMP   err_nei

; ---------------------------------------------------------------------
; do_run — RUN list: evaluate a list as Logo code
;   Arity 0: evaluates its own argument
;
;   Strategy: convert list to text in input_buf, then save evaluator
;   state, tokenize_line + eval_line, restore state.
; ---------------------------------------------------------------------
do_run:
      JSR   eval_expr
      BCC   :+
      JMP   @err
:
      LDA   eval_type
      CMP   #VAL_LIST
      BNE   @err_type

      ; Save eval_cur on stack (we'll resume here after RUN)
      LDA   eval_cur_lo
      PHA
      LDA   eval_cur_hi
      PHA

      ; Render the list into input_buf as text
      ; eval_val_lo/hi = list head
      STZ   z:buf_idx               ; reset write position
      JSR   render_list_to_buf

      ; Null-terminate input_buf
      LDX   z:buf_idx
      STZ   input_buf,X

      ; Save tokenizer and eval state (same pattern as proc_invoke)
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

      ; Tokenize the reconstructed line
      JSR   tokenize_line

      ; Evaluate it
      JSR   eval_line

      ; Restore tokenizer/eval state
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

      ; Restore eval_cur
      PLA
      STA   eval_cur_hi
      PLA
      STA   eval_cur_lo

      JMP   eval_continue

@err_type:
      LDX   #<str_run_name
      LDY   #>str_run_name
      JMP   err_needs_list
@err:
      LDX   #<str_run_name
      LDY   #>str_run_name
      JMP   err_nei

; ---------------------------------------------------------------------
; render_list_to_buf — render list elements into input_buf as text
;   Entry: eval_val_lo/hi = list head pointer
;          buf_idx = current write position in input_buf
;   Exit:  buf_idx updated
;   Clobbers: A, X, Y, ptr_lo/hi, ptr2_lo/hi, list_ptr, list_count
; ---------------------------------------------------------------------
render_list_to_buf:
      LDA   eval_val_lo
      STA   list_ptr_lo
      LDA   eval_val_hi
      STA   list_ptr_hi
      LDX   #0                     ; 0 = first element (no leading space)

@rl_loop:
      LDA   list_ptr_lo
      ORA   list_ptr_hi
      BNE   :+
      RTS                           ; end of list
:
      ; Add space before non-first elements
      CPX   #0
      BEQ   @rl_no_space
      LDA   z:buf_idx
      CMP   #126
      BCC   :+
      RTS                           ; buffer full — truncate
:
      TAY
      LDA   #' '
      STA   input_buf,Y
      INC   z:buf_idx
@rl_no_space:
      LDX   #1                     ; subsequent elements get space

      ; Read car type
      LDA   list_ptr_lo
      STA   ptr_lo
      LDA   list_ptr_hi
      STA   ptr_hi
      LDY   #CONS_CAR_TYPE
      LDA   (ptr_lo),Y

      CMP   #VAL_LIST
      BEQ   @rl_sublist
      CMP   #VAL_WORD
      BEQ   @rl_word

      ; Number: render to input_buf
      PHX
      LDA   list_ptr_lo
      PHA
      LDA   list_ptr_hi
      PHA
      ; Load the number value
      LDY   #CONS_CAR_HI
      LDA   (ptr_lo),Y
      STA   eval_val_hi
      LDY   #CONS_CAR_LO
      LDA   (ptr_lo),Y
      STA   eval_val_lo
      LDY   #CONS_CAR_FRAC
      LDA   (ptr_lo),Y
      STA   eval_val_frac
      JSR   render_number_to_buf
      PLA
      STA   list_ptr_hi
      PLA
      STA   list_ptr_lo
      PLX
      BRA   @rl_advance

@rl_word:
      PHX
      LDA   list_ptr_lo
      PHA
      LDA   list_ptr_hi
      PHA
      ; Get word string pointer
      LDY   #CONS_CAR_LO
      LDA   (ptr_lo),Y
      STA   ptr2_lo
      LDY   #CONS_CAR_HI
      LDA   (ptr_lo),Y
      STA   ptr2_hi
      ; Copy word chars to buf
      LDY   #0
      LDA   (ptr2_lo),Y            ; length
      TAX
      BEQ   @rl_word_done
      INY
@rl_wch:
      LDA   z:buf_idx
      CMP   #126
      BCS   @rl_word_done
      LDA   (ptr2_lo),Y
      PHY
      LDY   z:buf_idx
      STA   input_buf,Y
      INC   z:buf_idx
      PLY
      INY
      DEX
      BNE   @rl_wch
@rl_word_done:
      PLA
      STA   list_ptr_hi
      PLA
      STA   list_ptr_lo
      PLX
      BRA   @rl_advance

@rl_sublist:
      PHX
      LDA   list_ptr_lo
      PHA
      LDA   list_ptr_hi
      PHA
      ; Write '[' to buf
      LDA   z:buf_idx
      CMP   #126
      BCS   @rl_sub_done
      TAY
      LDA   #'['
      STA   input_buf,Y
      INC   z:buf_idx
      ; Get sublist pointer and recurse
      LDY   #CONS_CAR_LO
      LDA   (ptr_lo),Y
      STA   eval_val_lo
      LDY   #CONS_CAR_HI
      LDA   (ptr_lo),Y
      STA   eval_val_hi
      JSR   render_list_to_buf
      ; Write ']' to buf
      LDA   z:buf_idx
      CMP   #126
      BCS   @rl_sub_done
      TAY
      LDA   #']'
      STA   input_buf,Y
      INC   z:buf_idx
@rl_sub_done:
      PLA
      STA   list_ptr_hi
      PLA
      STA   list_ptr_lo
      PLX

@rl_advance:
      ; Follow cdr
      LDA   list_ptr_lo
      STA   ptr_lo
      LDA   list_ptr_hi
      STA   ptr_hi
      LDY   #CONS_CDR_LO
      LDA   (ptr_lo),Y
      TAX
      LDY   #CONS_CDR_HI
      LDA   (ptr_lo),Y
      STA   list_ptr_hi
      STX   list_ptr_lo
      LDX   #1                     ; always space after first element
      JMP   @rl_loop

@rl_trunc:
      RTS

; ---------------------------------------------------------------------
; render_number_to_buf — render eval_val as decimal into input_buf
;   Entry: eval_val_hi/lo/frac = number value
;          buf_idx = current write position
;   Exit:  buf_idx updated
;   Clobbers: A, X, Y, num_val, num_tmp, pn_buf
; ---------------------------------------------------------------------
render_number_to_buf:
      ; Handle sign
      LDA   eval_val_hi
      BPL   @rn_positive
      ; Write '-'
      LDY   z:buf_idx
      LDA   #'-'
      STA   input_buf,Y
      INC   z:buf_idx
      ; Negate
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

@rn_positive:
      ; Render integer part using power-of-10 subtraction
      LDA   eval_val_lo
      STA   num_val_lo
      LDA   eval_val_hi
      STA   num_val_hi

      LDX   #0                     ; digit buffer index
      LDY   #0                     ; power-of-10 index
@rn_div:
      STZ   num_tmp_lo
@rn_sub:
      SEC
      LDA   num_val_lo
      SBC   pow10_lo,Y
      PHA
      LDA   num_val_hi
      SBC   pow10_hi,Y
      BCC   @rn_sub_done
      STA   num_val_hi
      PLA
      STA   num_val_lo
      INC   num_tmp_lo
      BRA   @rn_sub
@rn_sub_done:
      PLA
      LDA   num_tmp_lo
      STA   pn_buf,X
      INX
      INY
      CPY   #5
      BCC   @rn_div

      ; Write digits suppressing leading zeros
      LDY   #0                     ; suppress flag
      LDX   #0
@rn_print:
      LDA   pn_buf,X
      BNE   @rn_nonzero
      CPY   #0
      BEQ   @rn_skip
@rn_nonzero:
      LDY   #1
      ORA   #'0'
      PHX
      LDX   z:buf_idx
      STA   input_buf,X
      INC   z:buf_idx
      PLX
@rn_skip:
      INX
      CPX   #4
      BCC   @rn_print
      ; Always write ones digit
      LDA   pn_buf+4
      ORA   #'0'
      LDX   z:buf_idx
      STA   input_buf,X
      INC   z:buf_idx

      ; Fractional part
      LDA   eval_val_frac
      BNE   @rn_has_frac
      RTS
@rn_has_frac:
      ; Write '.'
      LDX   z:buf_idx
      LDA   #'.'
      STA   input_buf,X
      INC   z:buf_idx
      ; First frac digit
      LDA   eval_val_frac
      STA   num_tmp_lo
      STZ   num_tmp_hi
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
      ; High byte = digit
      LDA   num_val_hi
      ORA   #'0'
      LDX   z:buf_idx
      STA   input_buf,X
      INC   z:buf_idx
      ; Second frac digit
      LDA   num_val_lo
      STA   num_tmp_lo
      STZ   num_tmp_hi
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
      LDA   num_val_hi
      BEQ   @rn_done                ; skip trailing zero
      ORA   #'0'
      LDX   z:buf_idx
      STA   input_buf,X
      INC   z:buf_idx
@rn_done:
      RTS

; =====================================================================
; RODATA — list error strings
; =====================================================================
      .segment "RODATA"











; Builtin name strings for list operations
str_first_name:
      .byte 5, "FIRST"
str_butfirst_name:
      .byte 8, "BUTFIRST"
str_bf_name:
      .byte 2, "BF"
str_count_name:
      .byte 5, "COUNT"
str_emptyp_name:
      .byte 6, "EMPTY?"
str_last_name:
      .byte 4, "LAST"
str_butlast_name:
      .byte 7, "BUTLAST"
str_bl_name:
      .byte 2, "BL"
str_item_name:
      .byte 4, "ITEM"
str_memberp_name:
      .byte 7, "MEMBER?"
str_show_name:
      .byte 4, "SHOW"
str_fput_name:
      .byte 4, "FPUT"
str_lput_name:
      .byte 4, "LPUT"
str_list_name:
      .byte 4, "LIST"
str_sentence_name:
      .byte 8, "SENTENCE"
str_se_name:
      .byte 2, "SE"
str_word_name:
      .byte 4, "WORD"
str_run_name:
      .byte 3, "RUN"

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
VAL_NUMBER = $00
VAL_WORD   = $01
VAL_LIST   = $02

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
      LDA   #VAL_NUMBER
      STA   eval_type
      JSR   eval_advance
      JMP   @alloc_cell

@list_word:
      LDY   #TOK_PAYLOAD
      LDA   (ptr_lo),Y
      PHA
      INC
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
      JSR   heap_alloc
      BCS   @oom
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
print_list:
      LDA   #'['
      STA   VGC_CHAROUT

      LDA   eval_val_lo
      STA   list_ptr_lo
      LDA   eval_val_hi
      STA   list_ptr_hi

      LDX   #0                  ; 0 = first element (no leading space)

@pl_loop:
      LDA   list_ptr_lo
      ORA   list_ptr_hi
      BNE   :+
      JMP   @pl_done
:
      CPX   #0
      BEQ   @pl_no_space
      LDA   #' '
      STA   VGC_CHAROUT
@pl_no_space:
      LDX   #1

      LDA   list_ptr_lo
      STA   ptr_lo
      LDA   list_ptr_hi
      STA   ptr_hi
      LDY   #CONS_CAR_TYPE
      LDA   (ptr_lo),Y
      CMP   #VAL_LIST
      BEQ   @pl_sublist
      CMP   #VAL_WORD
      BNE   :+
      JMP   @pl_word
:
      ; Number
      PHX
      LDA   list_ptr_lo
      PHA
      LDA   list_ptr_hi
      PHA
      LDY   #CONS_CAR_HI
      LDA   (ptr_lo),Y
      STA   eval_val_hi
      LDY   #CONS_CAR_LO
      LDA   (ptr_lo),Y
      STA   eval_val_lo
      LDY   #CONS_CAR_FRAC
      LDA   (ptr_lo),Y
      STA   eval_val_frac
      JSR   print_number
      PLA
      STA   list_ptr_hi
      PLA
      STA   list_ptr_lo
      PLX
      BRA   @pl_advance

@pl_sublist:
      PHX
      LDA   list_ptr_lo
      PHA
      LDA   list_ptr_hi
      PHA
      LDY   #CONS_CAR_LO
      LDA   (ptr_lo),Y
      STA   eval_val_lo
      LDY   #CONS_CAR_HI
      LDA   (ptr_lo),Y
      STA   eval_val_hi
      JSR   print_list
      PLA
      STA   list_ptr_hi
      PLA
      STA   list_ptr_lo
      PLX
      BRA   @pl_advance

@pl_word:
      PHX
      LDA   list_ptr_lo
      PHA
      LDA   list_ptr_hi
      PHA
      LDY   #CONS_CAR_LO
      LDA   (ptr_lo),Y
      STA   ptr2_lo
      LDY   #CONS_CAR_HI
      LDA   (ptr_lo),Y
      STA   ptr2_hi
      LDY   #0
      LDA   (ptr2_lo),Y
      TAX
      BEQ   @pl_word_done
      INY
@pl_word_ch:
      LDA   (ptr2_lo),Y
      STA   VGC_CHAROUT
      INY
      DEX
      BNE   @pl_word_ch
@pl_word_done:
      PLA
      STA   list_ptr_hi
      PLA
      STA   list_ptr_lo
      PLX

@pl_advance:
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
      JMP   @pl_loop

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
      LDX   #<str_first_err
      LDY   #>str_first_err
      JMP   list_print_err
@err_type:
      LDX   #<str_first_type
      LDY   #>str_first_type
      JMP   list_print_err
@err_empty:
      LDX   #<str_first_empty
      LDY   #>str_first_empty
      JMP   list_print_err

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
      LDX   #<str_bf_err
      LDY   #>str_bf_err
      JMP   list_print_err
@err_type:
      LDX   #<str_bf_type
      LDY   #>str_bf_type
      JMP   list_print_err
@err_empty:
      LDX   #<str_bf_empty
      LDY   #>str_bf_empty
      JMP   list_print_err

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
      LDA   #VAL_NUMBER
      STA   eval_type
      LDA   list_count_hi
      STA   eval_val_hi
      LDA   list_count_lo
      STA   eval_val_lo
      STZ   eval_val_frac
      JMP   eval_continue

@err_args:
      LDX   #<str_count_err
      LDY   #>str_count_err
      JMP   list_print_err
@err_type:
      LDX   #<str_count_type
      LDY   #>str_count_type
      JMP   list_print_err

; ---------------------------------------------------------------------
; do_emptyp — EMPTY? value: return 1 if empty list, else 0
;   Arity 0: evaluates its own argument
; ---------------------------------------------------------------------
do_emptyp:
      JSR   eval_expr
      BCC   :+
      LDX   #<str_emptyp_err
      LDY   #>str_emptyp_err
      JMP   list_print_err
:
      ; Check for empty list (type=list and pointer=$0000)
      LDA   eval_type
      CMP   #VAL_LIST
      BNE   @not_empty

      LDA   eval_val_lo
      ORA   eval_val_hi
      BNE   @not_empty

      ; Empty list
      LDA   #VAL_NUMBER
      STA   eval_type
      LDA   #1
      STA   eval_val_lo
      STZ   eval_val_hi
      STZ   eval_val_frac
      JMP   eval_continue

@not_empty:
      LDA   #VAL_NUMBER
      STA   eval_type
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
      LDX   #<str_last_err
      LDY   #>str_last_err
      JMP   list_print_err
@err_type:
      LDX   #<str_last_type
      LDY   #>str_last_type
      JMP   list_print_err
@err_empty:
      LDX   #<str_last_empty
      LDY   #>str_last_empty
      JMP   list_print_err

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
      LDX   #<str_bl_err
      LDY   #>str_bl_err
      JMP   list_print_err
@err_type:
      LDX   #<str_bl_type
      LDY   #>str_bl_type
      JMP   list_print_err
@err_empty:
      LDX   #<str_bl_empty
      LDY   #>str_bl_empty
      JMP   list_print_err

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
      LDX   #<str_item_type
      LDY   #>str_item_type
      JMP   list_print_err
@err_args:
      LDX   #<str_item_err
      LDY   #>str_item_err
      JMP   list_print_err
@err_range:
      LDX   #<str_item_range
      LDY   #>str_item_range
      JMP   list_print_err

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
      LDA   #VAL_NUMBER
      STA   eval_type
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
      LDA   #VAL_NUMBER
      STA   eval_type
      STZ   eval_val_lo
      STZ   eval_val_hi
      STZ   eval_val_frac
      JMP   eval_continue

@err:
      LDX   #<str_memberp_err
      LDY   #>str_memberp_err
      JMP   list_print_err
@err_type_pop4:
      PLA
      PLA
      PLA
      PLA
      LDX   #<str_memberp_type
      LDY   #>str_memberp_type
      JMP   list_print_err

; ---------------------------------------------------------------------
; do_show — SHOW value: like PRINT, adds brackets for lists
;   Arity 0: evaluates its own argument
; ---------------------------------------------------------------------
do_show:
      JSR   eval_expr
      BCC   :+
      LDX   #<str_show_err
      LDY   #>str_show_err
      JMP   list_print_err
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

; =====================================================================
; RODATA — list error strings
; =====================================================================
      .segment "RODATA"

str_first_err:
      .byte "NOT ENOUGH INPUTS TO FIRST", 0
str_first_type:
      .byte "FIRST DOESN'T LIKE ", 0
str_first_empty:
      .byte "FIRST DOESN'T LIKE [] AS INPUT", 0

str_bf_err:
      .byte "NOT ENOUGH INPUTS TO BUTFIRST", 0
str_bf_type:
      .byte "BUTFIRST DOESN'T LIKE ", 0
str_bf_empty:
      .byte "BUTFIRST DOESN'T LIKE [] AS INPUT", 0

str_count_err:
      .byte "NOT ENOUGH INPUTS TO COUNT", 0
str_count_type:
      .byte "COUNT DOESN'T LIKE ", 0

str_emptyp_err:
      .byte "NOT ENOUGH INPUTS TO EMPTY?", 0

str_last_err:
      .byte "NOT ENOUGH INPUTS TO LAST", 0
str_last_type:
      .byte "LAST DOESN'T LIKE ", 0
str_last_empty:
      .byte "LAST DOESN'T LIKE [] AS INPUT", 0

str_bl_err:
      .byte "NOT ENOUGH INPUTS TO BUTLAST", 0
str_bl_type:
      .byte "BUTLAST DOESN'T LIKE ", 0
str_bl_empty:
      .byte "BUTLAST DOESN'T LIKE [] AS INPUT", 0

str_item_err:
      .byte "NOT ENOUGH INPUTS TO ITEM", 0
str_item_type:
      .byte "ITEM NEEDS A LIST", 0
str_item_range:
      .byte "ITEM INDEX OUT OF RANGE", 0

str_memberp_err:
      .byte "NOT ENOUGH INPUTS TO MEMBER?", 0
str_memberp_type:
      .byte "MEMBER? NEEDS A LIST", 0

str_show_err:
      .byte "NOT ENOUGH INPUTS TO SHOW", 0

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

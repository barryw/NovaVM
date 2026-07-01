; gc.s — mark-sweep garbage collector for NovaLogo
;
; Allocation header (prepended by heap_alloc):
;   +0: alloc_tag  (bit 7 = mark, bits 6-0 = type)
;   +1: alloc_size (total size including header; 0 = use 16-bit size at +2/+3)
;
; Type constants (bits 6-0 of alloc_tag):
ATYPE_FREE   = $00
ATYPE_TOKEN  = $01
ATYPE_CONS   = $02
ATYPE_VAR    = $03
ATYPE_PROC   = $04
ATYPE_STRING = $05

; Header layout offsets (relative to allocation base, NOT payload)
GC_HDR_TAG   = 0
GC_HDR_SIZE  = 1
GC_HDR_BYTES = 2              ; overhead per allocation

; Mark bit
ALLOC_MARK_BIT = $80

; =====================================================================
; ZEROPAGE segment — GC variables
; =====================================================================
      .segment "ZEROPAGE"

gc_ptr_lo:    .res 1          ; GC walk pointer low
gc_ptr_hi:    .res 1          ; GC walk pointer high
gc_tmp_lo:    .res 1          ; GC temp low
gc_tmp_hi:    .res 1          ; GC temp high
gc_mark_lo:   .res 1          ; node-to-mark pointer low
gc_mark_hi:   .res 1          ; node-to-mark pointer high

; =====================================================================
; CODE segment — GC routines
; =====================================================================
      .segment "CODE"

; ---------------------------------------------------------------------
; gc_collect — run a full mark-sweep garbage collection cycle
;   1. Clear all mark bits
;   2. Mark all reachable objects from roots
;   3. Sweep: coalesce all unmarked blocks and reset heap_ptr
;
;   Clobbers: A, X, Y, gc_ptr, gc_tmp, gc_mark, ptr_lo/hi, ptr2_lo/hi
; ---------------------------------------------------------------------
gc_collect:
      ; Phase 1: Clear all mark bits by walking the heap linearly
      LDA   #<HEAP_START
      STA   gc_ptr_lo
      LDA   #>HEAP_START
      STA   gc_ptr_hi

@clear_loop:
      ; Have we reached heap_ptr (end of allocated space)?
      LDA   gc_ptr_hi
      CMP   heap_ptr+1
      BCC   @clear_one
      BNE   @clear_done
      LDA   gc_ptr_lo
      CMP   heap_ptr
      BCS   @clear_done

@clear_one:
      ; Clear mark bit on this allocation
      LDA   gc_ptr_lo
      STA   ptr_lo
      LDA   gc_ptr_hi
      STA   ptr_hi
      LDY   #GC_HDR_TAG
      LDA   (ptr_lo),Y
      AND   #<~ALLOC_MARK_BIT    ; clear bit 7
      STA   (ptr_lo),Y

      ; Advance by alloc_size
      LDY   #GC_HDR_SIZE
      LDA   (ptr_lo),Y
      BEQ   @clear_big           ; size=0 means 16-bit size at +2/+3
      CLC
      ADC   gc_ptr_lo
      STA   gc_ptr_lo
      BCC   @clear_loop
      INC   gc_ptr_hi
      BRA   @clear_loop

@clear_big:
      ; 16-bit size at payload offset 0 and 1 (header +2 and +3)
      LDY   #GC_HDR_BYTES
      LDA   (ptr_lo),Y
      STA   gc_tmp_lo
      INY
      LDA   (ptr_lo),Y
      STA   gc_tmp_hi
      CLC
      LDA   gc_ptr_lo
      ADC   gc_tmp_lo
      STA   gc_ptr_lo
      LDA   gc_ptr_hi
      ADC   gc_tmp_hi
      STA   gc_ptr_hi
      BRA   @clear_loop

@clear_done:

      ; Phase 2: Mark all reachable objects
      ; Root 1: variable list (var_head)
      JSR   gc_mark_vars

      ; Root 2: procedure list (proc_head)
      JSR   gc_mark_procs

      ; Root 3: current token list (tok_head)
      JSR   gc_mark_token_list

      ; Phase 3: Sweep — find the highest marked allocation,
      ; reset heap_ptr to just past it. This works because we
      ; use a bump allocator with no free list; all unmarked
      ; allocations between marked ones become wasted space,
      ; but in practice the permanent objects (vars, procs) are
      ; allocated early and the ephemeral ones (tokens, cons,
      ; strings) are allocated later and all unmarked — so
      ; heap_ptr shrinks back dramatically.
      ;
      ; Walk the heap forward; track the highest end-address
      ; of any marked allocation.
      LDA   #<HEAP_START
      STA   gc_ptr_lo
      LDA   #>HEAP_START
      STA   gc_ptr_hi
      ; gc_tmp = highest marked end (start at HEAP_START = nothing marked)
      LDA   #<HEAP_START
      STA   gc_tmp_lo
      LDA   #>HEAP_START
      STA   gc_tmp_hi

@sweep_loop:
      ; Reached end of allocated space?
      LDA   gc_ptr_hi
      CMP   heap_ptr+1
      BCC   @sweep_one
      BNE   @sweep_done
      LDA   gc_ptr_lo
      CMP   heap_ptr
      BCS   @sweep_done

@sweep_one:
      LDA   gc_ptr_lo
      STA   ptr_lo
      LDA   gc_ptr_hi
      STA   ptr_hi

      ; Get size of this allocation
      LDY   #GC_HDR_SIZE
      LDA   (ptr_lo),Y
      BEQ   @sweep_big
      STA   gc_mark_lo            ; use gc_mark_lo as temp for size
      STZ   gc_mark_hi
      BRA   @sweep_check_mark

@sweep_big:
      LDY   #GC_HDR_BYTES
      LDA   (ptr_lo),Y
      STA   gc_mark_lo
      INY
      LDA   (ptr_lo),Y
      STA   gc_mark_hi

@sweep_check_mark:
      ; Is this allocation marked?
      LDY   #GC_HDR_TAG
      LDA   (ptr_lo),Y
      AND   #ALLOC_MARK_BIT
      BEQ   @sweep_advance        ; not marked — skip

      ; Marked — compute end address = gc_ptr + size
      CLC
      LDA   gc_ptr_lo
      ADC   gc_mark_lo
      STA   gc_tmp_lo             ; update highest end
      LDA   gc_ptr_hi
      ADC   gc_mark_hi
      STA   gc_tmp_hi

@sweep_advance:
      ; Advance gc_ptr by size
      CLC
      LDA   gc_ptr_lo
      ADC   gc_mark_lo
      STA   gc_ptr_lo
      LDA   gc_ptr_hi
      ADC   gc_mark_hi
      STA   gc_ptr_hi
      BRA   @sweep_loop

@sweep_done:
      ; Set heap_ptr to highest marked end
      LDA   gc_tmp_lo
      STA   heap_ptr
      LDA   gc_tmp_hi
      STA   heap_ptr+1
      RTS

; ---------------------------------------------------------------------
; gc_mark_node — mark a single allocation given a payload pointer
;   Input: gc_mark_lo/hi = pointer to payload (header is at -2)
;   If pointer is $0000 or below HEAP_START, skip.
;   Clobbers: A, Y (preserves gc_mark_lo/hi)
; ---------------------------------------------------------------------
gc_mark_node:
      ; Null check
      LDA   gc_mark_lo
      ORA   gc_mark_hi
      BEQ   @done

      ; Compute header address = payload - 2
      LDA   gc_mark_lo
      SEC
      SBC   #GC_HDR_BYTES
      STA   ptr_lo
      LDA   gc_mark_hi
      SBC   #0
      STA   ptr_hi

      ; Range check: must be >= HEAP_START
      CMP   #>HEAP_START
      BCC   @done
      BNE   @in_range
      LDA   ptr_lo
      CMP   #<HEAP_START
      BCC   @done
@in_range:

      ; Set mark bit
      LDY   #GC_HDR_TAG
      LDA   (ptr_lo),Y
      ORA   #ALLOC_MARK_BIT
      STA   (ptr_lo),Y
@done:
      RTS

; ---------------------------------------------------------------------
; gc_mark_vars — walk the variable linked list and mark each entry
;   Also marks any list values pointed to by variables.
;   Clobbers: A, X, Y, gc_ptr, gc_mark, ptr_lo/hi, ptr2_lo/hi
; ---------------------------------------------------------------------
gc_mark_vars:
      LDA   var_head_lo
      STA   gc_ptr_lo
      LDA   var_head_hi
      STA   gc_ptr_hi

@walk:
      LDA   gc_ptr_lo
      ORA   gc_ptr_hi
      BNE   @have_var
      RTS
@have_var:

      ; Mark this var entry
      LDA   gc_ptr_lo
      STA   gc_mark_lo
      LDA   gc_ptr_hi
      STA   gc_mark_hi
      JSR   gc_mark_node

      ; Read the variable's value — if it's a list, mark the list
      LDA   gc_ptr_lo
      STA   ptr_lo
      LDA   gc_ptr_hi
      STA   ptr_hi
      LDY   #VAR_NAME_LEN
      LDA   (ptr_lo),Y           ; name_len
      CLC
      ADC   #VAR_NAME             ; offset to value_type
      TAY
      LDA   (ptr_lo),Y           ; value_type
      CMP   #VAL_LIST
      BNE   @var_check_word
      ; It's a list — get the list head pointer and mark the list
      INY
      LDA   (ptr_lo),Y           ; val_hi (actually car_hi)
      STA   gc_tmp_hi
      INY
      LDA   (ptr_lo),Y           ; val_lo
      STA   gc_tmp_lo
      ; Save gc_ptr across recursive mark
      LDA   gc_ptr_lo
      PHA
      LDA   gc_ptr_hi
      PHA
      LDA   gc_tmp_lo
      STA   gc_mark_lo
      LDA   gc_tmp_hi
      STA   gc_mark_hi
      JSR   gc_mark_list
      PLA
      STA   gc_ptr_hi
      PLA
      STA   gc_ptr_lo
      BRA   @var_next

@var_check_word:
      CMP   #VAL_WORD
      BNE   @var_next
      ; It's a word — mark the string allocation
      INY
      LDA   (ptr_lo),Y           ; ptr_hi for word
      STA   gc_tmp_hi
      INY
      LDA   (ptr_lo),Y           ; ptr_lo for word
      STA   gc_tmp_lo
      LDA   gc_ptr_lo
      PHA
      LDA   gc_ptr_hi
      PHA
      LDA   gc_tmp_lo
      STA   gc_mark_lo
      LDA   gc_tmp_hi
      STA   gc_mark_hi
      JSR   gc_mark_node
      PLA
      STA   gc_ptr_hi
      PLA
      STA   gc_ptr_lo

@var_next:
      ; Follow next pointer
      LDA   gc_ptr_lo
      STA   ptr_lo
      LDA   gc_ptr_hi
      STA   ptr_hi
      LDY   #VAR_NEXT_LO
      LDA   (ptr_lo),Y
      TAX
      INY
      LDA   (ptr_lo),Y
      STA   gc_ptr_hi
      STX   gc_ptr_lo
      JMP   @walk

; ---------------------------------------------------------------------
; gc_mark_procs — walk the procedure list and mark each record
;   Clobbers: A, Y, gc_ptr, gc_mark, ptr_lo/hi
; ---------------------------------------------------------------------
gc_mark_procs:
      LDA   proc_head_lo
      STA   gc_ptr_lo
      LDA   proc_head_hi
      STA   gc_ptr_hi

@walk:
      LDA   gc_ptr_lo
      ORA   gc_ptr_hi
      BEQ   @done

      ; Mark this procedure record.
      ; proc_entry points to record data (base+4); header is at base+0.
      ; Subtract 4 to get the header address for marking.
      LDA   gc_ptr_lo
      SEC
      SBC   #4
      STA   ptr_lo
      LDA   gc_ptr_hi
      SBC   #0
      STA   ptr_hi
      ; Set mark bit directly at the header
      LDY   #GC_HDR_TAG
      LDA   (ptr_lo),Y
      ORA   #ALLOC_MARK_BIT
      STA   (ptr_lo),Y

      ; Follow next pointer (at record data offset 0)
      LDA   gc_ptr_lo
      STA   ptr_lo
      LDA   gc_ptr_hi
      STA   ptr_hi
      LDY   #0                    ; next_lo at offset 0
      LDA   (ptr_lo),Y
      TAX
      INY
      LDA   (ptr_lo),Y
      STA   gc_ptr_hi
      STX   gc_ptr_lo
      BRA   @walk

@done:
      RTS

; ---------------------------------------------------------------------
; gc_mark_token_list — walk tok_head linked list and mark each token
;   Clobbers: A, Y, gc_ptr, gc_mark, ptr_lo/hi
; ---------------------------------------------------------------------
gc_mark_token_list:
      LDA   tok_head_lo
      STA   gc_ptr_lo
      LDA   tok_head_hi
      STA   gc_ptr_hi

@walk:
      LDA   gc_ptr_lo
      ORA   gc_ptr_hi
      BEQ   @done

      ; Mark this token
      LDA   gc_ptr_lo
      STA   gc_mark_lo
      LDA   gc_ptr_hi
      STA   gc_mark_hi
      JSR   gc_mark_node

      ; Follow next pointer (at payload offsets TOK_NEXT_LO/HI)
      LDA   gc_ptr_lo
      STA   ptr_lo
      LDA   gc_ptr_hi
      STA   ptr_hi
      LDY   #TOK_NEXT_LO
      LDA   (ptr_lo),Y
      TAX
      INY
      LDA   (ptr_lo),Y
      STA   gc_ptr_hi
      STX   gc_ptr_lo
      BRA   @walk

@done:
      RTS

; ---------------------------------------------------------------------
; gc_mark_list — recursively mark a cons-pair list
;   Input: gc_mark_lo/hi = head cons pair pointer (payload)
;   Marks each cons cell, and recursively marks sublists and word strings.
;   Clobbers: A, X, Y, gc_mark, gc_ptr, ptr_lo/hi
; ---------------------------------------------------------------------
gc_mark_list:
      ; Use gc_ptr to walk the list, gc_mark for marking
      LDA   gc_mark_lo
      STA   gc_ptr_lo
      LDA   gc_mark_hi
      STA   gc_ptr_hi

@walk:
      LDA   gc_ptr_lo
      ORA   gc_ptr_hi
      BEQ   @done

      ; Mark this cons pair
      LDA   gc_ptr_lo
      STA   gc_mark_lo
      LDA   gc_ptr_hi
      STA   gc_mark_hi
      JSR   gc_mark_node

      ; Check car_type: if list, recursively mark sublist
      ; if word, mark the string
      LDA   gc_ptr_lo
      STA   ptr_lo
      LDA   gc_ptr_hi
      STA   ptr_hi
      LDY   #CONS_CAR_TYPE
      LDA   (ptr_lo),Y
      CMP   #VAL_LIST
      BEQ   @car_list
      CMP   #VAL_WORD
      BEQ   @car_word
      BRA   @follow_cdr

@car_word:
      ; Mark the word string allocation
      LDY   #CONS_CAR_LO
      LDA   (ptr_lo),Y
      STA   gc_mark_lo
      LDY   #CONS_CAR_HI
      LDA   (ptr_lo),Y
      STA   gc_mark_hi
      ; Save gc_ptr across mark
      LDA   gc_ptr_lo
      PHA
      LDA   gc_ptr_hi
      PHA
      JSR   gc_mark_node
      PLA
      STA   gc_ptr_hi
      PLA
      STA   gc_ptr_lo
      BRA   @follow_cdr

@car_list:
      ; Recursively mark the sublist
      LDY   #CONS_CAR_LO
      LDA   (ptr_lo),Y
      STA   gc_mark_lo
      LDY   #CONS_CAR_HI
      LDA   (ptr_lo),Y
      STA   gc_mark_hi
      ; Save gc_ptr across recursive call
      LDA   gc_ptr_lo
      PHA
      LDA   gc_ptr_hi
      PHA
      JSR   gc_mark_list          ; recursive
      PLA
      STA   gc_ptr_hi
      PLA
      STA   gc_ptr_lo

@follow_cdr:
      ; Follow cdr to next cons pair
      LDA   gc_ptr_lo
      STA   ptr_lo
      LDA   gc_ptr_hi
      STA   ptr_hi
      LDY   #CONS_CDR_LO
      LDA   (ptr_lo),Y
      TAX
      LDY   #CONS_CDR_HI
      LDA   (ptr_lo),Y
      STA   gc_ptr_hi
      STX   gc_ptr_lo
      BRA   @walk

@done:
      RTS

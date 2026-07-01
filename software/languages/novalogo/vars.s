; vars.s — variable storage for NovaLogo (MAKE / :reference)
;
; Variables are stored as a linked list of heap entries.
; Entry layout:
;   +0: next_lo  (2 bytes — pointer to next entry, or $0000)
;   +1: next_hi
;   +2: name_len (1 byte)
;   +3: name chars (name_len bytes, uppercased)
;   +3+name_len: value_type (1 byte: $00=number, $01=word)
;   +4+name_len: val_hi  (or ptr_lo for word)
;   +5+name_len: val_lo  (or ptr_hi for word)
;   +6+name_len: val_frac (0 for word)
;   +7+name_len: pad     (0)
;
; Total entry size = 8 + name_len bytes.

; Entry field offsets (relative to entry base)
VAR_NEXT_LO  = 0
VAR_NEXT_HI  = 1
VAR_NAME_LEN = 2
VAR_NAME     = 3              ; first char of name

; =====================================================================
; ZEROPAGE segment — variable list head + scratch
; =====================================================================
      .segment "ZEROPAGE"

var_head_lo:    .res 1        ; head of variable linked list
var_head_hi:    .res 1
var_entry_lo:   .res 1        ; current entry pointer (walk scratch)
var_entry_hi:   .res 1
var_name_lo:    .res 1        ; saved name pointer for set/get
var_name_hi:    .res 1

; =====================================================================
; CODE segment — variable routines
; =====================================================================
      .segment "CODE"

; ---------------------------------------------------------------------
; var_init — initialize variable list to empty
;   Called once at cold_start
; ---------------------------------------------------------------------
var_init:
      STZ   var_head_lo
      STZ   var_head_hi
      RTS

; ---------------------------------------------------------------------
; var_set — store a value under a variable name
;   Input: ptr_lo/hi → length-prefixed name string (in token node)
;          eval_type, eval_val_hi/lo/frac = value to store
;   Clobbers: A, X, Y, ptr_lo/hi, ptr2_lo/hi, var_entry, var_name
; ---------------------------------------------------------------------
var_set:
      ; Save name pointer — we need it after the walk
      LDA   ptr_lo
      STA   var_name_lo
      LDA   ptr_hi
      STA   var_name_hi

      ; Walk the list looking for an existing entry with this name
      LDA   var_head_lo
      STA   var_entry_lo
      LDA   var_head_hi
      STA   var_entry_hi

@walk:
      ; End of list?
      LDA   var_entry_lo
      ORA   var_entry_hi
      BEQ   @not_found

      ; Compare name in entry against var_name
      JSR   var_names_equal
      BCS   @found

      ; Advance to next entry
      LDA   var_entry_lo
      STA   ptr2_lo
      LDA   var_entry_hi
      STA   ptr2_hi
      LDY   #VAR_NEXT_LO
      LDA   (ptr2_lo),Y
      TAX
      INY
      LDA   (ptr2_lo),Y
      STA   var_entry_hi
      STX   var_entry_lo
      BRA   @walk

@found:
      ; Overwrite value in place
      ; Value starts at entry + 3 + name_len
      LDA   var_entry_lo
      STA   ptr2_lo
      LDA   var_entry_hi
      STA   ptr2_hi
      LDY   #VAR_NAME_LEN
      LDA   (ptr2_lo),Y          ; name_len
      CLC
      ADC   #VAR_NAME             ; A = 3 + name_len = offset to value_type
      TAY
      LDA   eval_type
      STA   (ptr2_lo),Y
      INY
      LDA   eval_val_hi
      STA   (ptr2_lo),Y
      INY
      LDA   eval_val_lo
      STA   (ptr2_lo),Y
      INY
      LDA   eval_val_frac
      STA   (ptr2_lo),Y
      INY
      LDA   #0
      STA   (ptr2_lo),Y          ; pad
      RTS

@not_found:
      ; Allocate a new entry: 8 + name_len bytes
      ; Read name_len from the name string
      LDA   var_name_lo
      STA   ptr_lo
      LDA   var_name_hi
      STA   ptr_hi
      LDY   #0
      LDA   (ptr_lo),Y           ; name_len
      PHA                         ; save name_len
      CLC
      ADC   #8                    ; total entry size
      LDX   #ATYPE_VAR
      JSR   heap_alloc
      BCS   @oom

      ; ptr_lo/hi now points to new entry
      ; Set next → current head
      LDY   #VAR_NEXT_LO
      LDA   var_head_lo
      STA   (ptr_lo),Y
      INY
      LDA   var_head_hi
      STA   (ptr_lo),Y

      ; Copy name_len
      PLA                         ; A = name_len
      PHA                         ; keep it on stack
      LDY   #VAR_NAME_LEN
      STA   (ptr_lo),Y

      ; Copy name chars (uppercased) from var_name source
      PLA                         ; A = name_len
      TAX                         ; X = remaining chars
      BEQ   @name_done            ; zero-length name (shouldn't happen)
      LDY   #VAR_NAME             ; dest offset in entry
      STZ   num_tmp_lo            ; source offset counter

@copy_name:
      PHX
      PHY
      ; Read source char
      LDA   var_name_lo
      STA   ptr2_lo
      LDA   var_name_hi
      STA   ptr2_hi
      LDA   num_tmp_lo
      INC                         ; skip length byte → offset = idx + 1
      TAY
      LDA   (ptr2_lo),Y          ; source char
      ; Uppercase
      CMP   #'a'
      BCC   @noup
      CMP   #'z'+1
      BCS   @noup
      SEC
      SBC   #$20
@noup:
      PLY                         ; restore dest offset
      STA   (ptr_lo),Y           ; write to entry
      INY
      INC   num_tmp_lo
      PLX
      DEX
      BNE   @copy_name

@name_done:
      ; Write value after name
      ; Y is already at VAR_NAME + name_len = value_type offset
      LDA   eval_type
      STA   (ptr_lo),Y
      INY
      LDA   eval_val_hi
      STA   (ptr_lo),Y
      INY
      LDA   eval_val_lo
      STA   (ptr_lo),Y
      INY
      LDA   eval_val_frac
      STA   (ptr_lo),Y
      INY
      LDA   #0
      STA   (ptr_lo),Y           ; pad

      ; Prepend to list: head = new entry
      LDA   ptr_lo
      STA   var_head_lo
      LDA   ptr_hi
      STA   var_head_hi
      RTS

@oom:
      PLA                         ; discard saved name_len
      RTS

; ---------------------------------------------------------------------
; var_get — look up a variable by name
;   Input: ptr_lo/hi → length-prefixed name string
;   Output: Carry clear = found, eval_type/eval_val set
;           Carry set = not found
;   Clobbers: A, X, Y, ptr2, var_entry, var_name
; ---------------------------------------------------------------------
var_get:
      ; Save name pointer
      LDA   ptr_lo
      STA   var_name_lo
      LDA   ptr_hi
      STA   var_name_hi

      ; Walk the list
      LDA   var_head_lo
      STA   var_entry_lo
      LDA   var_head_hi
      STA   var_entry_hi

@walk:
      LDA   var_entry_lo
      ORA   var_entry_hi
      BEQ   @not_found

      JSR   var_names_equal
      BCS   @found

      ; Advance to next
      LDA   var_entry_lo
      STA   ptr2_lo
      LDA   var_entry_hi
      STA   ptr2_hi
      LDY   #VAR_NEXT_LO
      LDA   (ptr2_lo),Y
      TAX
      INY
      LDA   (ptr2_lo),Y
      STA   var_entry_hi
      STX   var_entry_lo
      BRA   @walk

@found:
      ; Read value from entry + 3 + name_len
      LDA   var_entry_lo
      STA   ptr2_lo
      LDA   var_entry_hi
      STA   ptr2_hi
      LDY   #VAR_NAME_LEN
      LDA   (ptr2_lo),Y
      CLC
      ADC   #VAR_NAME
      TAY
      LDA   (ptr2_lo),Y          ; value_type
      STA   eval_type
      INY
      LDA   (ptr2_lo),Y          ; val_hi
      STA   eval_val_hi
      INY
      LDA   (ptr2_lo),Y          ; val_lo
      STA   eval_val_lo
      INY
      LDA   (ptr2_lo),Y          ; val_frac
      STA   eval_val_frac
      CLC
      RTS

@not_found:
      SEC
      RTS

; ---------------------------------------------------------------------
; var_names_equal — compare name at var_name_lo/hi against entry at var_entry
;   The name source is length-prefixed (from token / quote).
;   The entry stores: name_len at +2, chars at +3 (already uppercased).
;   The source name chars are uppercased during comparison.
;   Returns: Carry set = equal, Carry clear = not equal
;   Clobbers: A, X, Y, ptr2
; ---------------------------------------------------------------------
var_names_equal:
      ; Read source name length
      LDA   var_name_lo
      STA   ptr2_lo
      LDA   var_name_hi
      STA   ptr2_hi
      LDY   #0
      LDA   (ptr2_lo),Y          ; source name_len
      TAX                         ; X = source name_len

      ; Read entry name length
      LDA   var_entry_lo
      STA   ptr2_lo
      LDA   var_entry_hi
      STA   ptr2_hi
      LDY   #VAR_NAME_LEN
      LDA   (ptr2_lo),Y          ; entry name_len

      ; Compare lengths
      STX   num_tmp_lo            ; save source length
      CMP   num_tmp_lo
      BNE   @ne

      ; Lengths match — compare chars
      CPX   #0
      BEQ   @eq                   ; both zero-length — equal
      LDY   #0                    ; char index

@cmp_loop:
      PHX
      PHY
      ; Read source char (var_name + 1 + Y)
      LDA   var_name_lo
      STA   ptr2_lo
      LDA   var_name_hi
      STA   ptr2_hi
      TYA
      INC                         ; skip length byte
      TAY
      LDA   (ptr2_lo),Y          ; source char
      ; Uppercase it
      CMP   #'a'
      BCC   @noup
      CMP   #'z'+1
      BCS   @noup
      SEC
      SBC   #$20
@noup:
      STA   num_tmp_hi            ; temp: uppercased source char

      ; Read entry char (var_entry + VAR_NAME + Y)
      PLY                         ; restore char index
      PHY
      LDA   var_entry_lo
      STA   ptr2_lo
      LDA   var_entry_hi
      STA   ptr2_hi
      TYA
      CLC
      ADC   #VAR_NAME
      TAY
      LDA   (ptr2_lo),Y          ; entry char (already uppercase)

      CMP   num_tmp_hi
      ; 65C02: PLY/PLX clobber flags! Must branch BEFORE pulling.
      BNE   @ne_pull

      PLY
      PLX
      INY
      DEX
      BNE   @cmp_loop

@eq:
      SEC                         ; equal
      RTS

@ne_pull:
      PLY                         ; clean up stack
      PLX
@ne:
      CLC                         ; not equal
      RTS

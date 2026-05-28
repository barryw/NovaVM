; heap.s — bump allocator for NovaLogo
; Heap range: $0400 to $9FFF (~39KB)

HEAP_START = $0400
HEAP_END   = $A000          ; one past last usable byte

; =====================================================================
; ZEROPAGE segment — heap and general-purpose pointer variables
; =====================================================================
      .segment "ZEROPAGE"

heap_ptr:     .res 2          ; current heap allocation pointer
ptr_lo:       .res 1          ; general-purpose pointer (low byte)
ptr_hi:       .res 1          ; general-purpose pointer (high byte)

; =====================================================================
; CODE segment — heap routines
; =====================================================================
      .segment "CODE"

; ---------------------------------------------------------------------
; heap_init — reset heap to empty
;   Clobbers: A
; ---------------------------------------------------------------------
heap_init:
      LDA   #<HEAP_START
      STA   heap_ptr
      LDA   #>HEAP_START
      STA   heap_ptr+1
      RTS

; ---------------------------------------------------------------------
; heap_alloc — allocate A bytes from the heap
;   Input:  A = number of bytes to allocate
;   Output: ptr_lo/ptr_hi = pointer to allocated block
;           Carry clear = success, Carry set = out of memory
;   Clobbers: A, X
; ---------------------------------------------------------------------
heap_alloc:
      ; Save current heap_ptr as the return value
      LDX   heap_ptr
      STX   ptr_lo
      LDX   heap_ptr+1
      STX   ptr_hi
      ; Add size to heap_ptr
      CLC
      ADC   heap_ptr
      STA   heap_ptr
      LDA   #0
      ADC   heap_ptr+1
      STA   heap_ptr+1
      ; Check if we exceeded HEAP_END
      CMP   #>HEAP_END
      BCC   @ok              ; high byte < HEAP_END high -> ok
      BNE   @oom             ; high byte > HEAP_END high -> OOM
      LDA   heap_ptr
      CMP   #<HEAP_END
      BCC   @ok              ; low byte < HEAP_END low -> ok
      BEQ   @ok              ; exactly at end is still ok
@oom:
      SEC                    ; signal out of memory
      RTS
@ok:
      CLC                    ; signal success
      RTS

; heap.s — bump allocator with GC headers for NovaLogo
; Heap range: $0400 to HEAP_END-1.

HEAP_START = $0400
HEAP_END   = $9C00          ; one past last usable byte; $9C00-$9FFF are NDK/extension BSS

; =====================================================================
; ZEROPAGE segment — heap and general-purpose pointer variables
; =====================================================================
      .segment "ZEROPAGE"

heap_ptr:     .res 2          ; current heap allocation pointer
ptr_lo:       .res 1          ; general-purpose pointer (low byte)
ptr_hi:       .res 1          ; general-purpose pointer (high byte)

; =====================================================================
; BSS segment — heap scratch
; =====================================================================
      .segment "BSS"

ha_type:      .res 1          ; saved alloc type for heap_alloc
ha_paysz:     .res 1          ; saved payload size for heap_alloc
ha_gc_ran:    .res 1          ; flag: GC already attempted

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
; heap_alloc — allocate A bytes with a 2-byte GC header
;   Input:  A = payload size (bytes the caller wants)
;           X = allocation type (ATYPE_*)
;   Output: ptr_lo/ptr_hi = pointer to payload (header is at -2)
;           Carry clear = success, Carry set = out of memory
;   Clobbers: A, X, Y
;
;   Header layout at (returned_ptr - 2):
;     +0: alloc_tag  = type (bits 6-0), mark cleared (bit 7 = 0)
;     +1: alloc_size = payload + 2
; ---------------------------------------------------------------------
heap_alloc:
      STA   ha_paysz
      STX   ha_type
      STZ   ha_gc_ran

@retry:
      ; total = payload + 2
      LDA   ha_paysz
      CLC
      ADC   #GC_HDR_BYTES

      ; Save current heap_ptr → ptr_lo/hi (allocation base)
      LDX   heap_ptr
      STX   ptr_lo
      LDX   heap_ptr+1
      STX   ptr_hi

      ; Advance heap_ptr by total
      CLC
      ADC   heap_ptr
      STA   heap_ptr
      LDA   #0
      ADC   heap_ptr+1
      STA   heap_ptr+1

      ; Check against HEAP_END
      CMP   #>HEAP_END
      BCC   @ok
      BNE   @oom
      LDA   heap_ptr
      CMP   #<HEAP_END
      BCC   @ok
      BEQ   @ok

@oom:
      ; Restore heap_ptr
      LDA   ptr_lo
      STA   heap_ptr
      LDA   ptr_hi
      STA   heap_ptr+1

      ; Already tried GC?
      LDA   ha_gc_ran
      BNE   @fail
      INC   ha_gc_ran

      ; Run GC and retry
      JSR   gc_collect
      BRA   @retry

@fail:
      SEC
      RTS

@ok:
      ; Write header at allocation base (ptr_lo/hi)
      LDY   #GC_HDR_TAG
      LDA   ha_type
      STA   (ptr_lo),Y

      LDY   #GC_HDR_SIZE
      LDA   ha_paysz
      CLC
      ADC   #GC_HDR_BYTES
      STA   (ptr_lo),Y

      ; Advance ptr_lo/hi past header → payload pointer
      CLC
      LDA   ptr_lo
      ADC   #GC_HDR_BYTES
      STA   ptr_lo
      LDA   ptr_hi
      ADC   #0
      STA   ptr_hi

      CLC
      RTS

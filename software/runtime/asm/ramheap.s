; Dynamic allocator over __NOVA_IMAGE_END..__NOVA_RAM_END.
;
; Blocks are [u16 payload size, u8 used, payload]. Allocation linearly scans
; and coalesces adjacent free blocks before testing a fit; release validates an
; exact block boundary and clears the used byte. No fixed allocation table or
; reserved arena exists.
;
; ponytail: O(n) scans are bounded by Nova's 38 KiB application window; add
; segregated bins only if real Pascal workloads demonstrate allocator latency.

.ifndef LIBABI_INC_INCLUDED
.include "libabi.inc"
.endif
.ifndef NVR_INC_INCLUDED
.include "nvr.inc"
.endif
.ifndef RAMHEAP_INC_INCLUDED
.include "ramheap.inc"
.endif

.ifndef NOVA_EMIT_ALL_RUNTIME
NOVA_EMIT_ALL_RUNTIME = 0
.endif

RAMHEAP_HEADER = 3
RAMHEAP_USED = 1

.if NOVA_EMIT_ALL_RUNTIME .OR .referenced(ndk_ram_alloc) .OR .referenced(ndk_ram_free) .OR .referenced(ndk_ram_status)

      .segment "BSS"
ramheap_floor:       .res 2
ramheap_initialized:.res 1
ramheap_last_status:.res 1

      .segment "CODE"
      .export ndk_ram_alloc
      .export ndk_ram_free
      .export ndk_ram_status

; A/X = requested bytes. Returns A/X = payload pointer or zero on failure.
; @label RAMHEAP.ALLOC
; @kind routine
; @symbol ndk_ram_alloc
; @summary Allocate a contiguous block from the unused application-RAM tail.
; @in A/X: Requested byte count (little-endian).
; @out A/X: Payload pointer, or zero when allocation fails.
ndk_ram_alloc:
      STA   NVR2L
      STX   NVR2H
      ORA   NVR2L
      BNE   :+
      JMP   ramheap_bad_size
:
      JSR   ramheap_init
      BCC   :+
      JMP   ramheap_no_memory
:
      LDA   ramheap_floor
      STA   NVR0L                       ; current header
      LDA   ramheap_floor+1
      STA   NVR0H
@block:
      JSR   ramheap_load_block
      BCC   :+
      JMP   ramheap_no_memory
:
      LDY   #2
      LDA   (NVR0L),Y
      BEQ   @coalesce
@advance:
      LDA   NVR3L                       ; next header
      STA   NVR0L
      LDA   NVR3H
      STA   NVR0H
      LDA   NVR0H
      CMP   #>__NOVA_RAM_END
      BCC   @block
      BEQ   :+
      JMP   ramheap_no_memory
:
      LDA   NVR0L
      CMP   #<__NOVA_RAM_END
      BCC   @block
      JMP   ramheap_no_memory

; Merge all immediately following free blocks into the current block.
@coalesce:
      LDA   NVR3H
      CMP   #>__NOVA_RAM_END
      BNE   @next_exists
      LDA   NVR3L
      CMP   #<__NOVA_RAM_END
      BEQ   @fit
@next_exists:
      LDY   #2
      LDA   (NVR3L),Y
      BNE   @fit
      LDY   #0
      LDA   (NVR3L),Y
      STA   NVR6L
      INY
      LDA   (NVR3L),Y
      STA   NVR6H
      ORA   NVR6L
      BNE   :+
      JMP   ramheap_no_memory
:
      CLC
      LDA   NVR3L
      ADC   #RAMHEAP_HEADER
      ADC   NVR6L
      STA   NVR4L                       ; header after merged neighbor
      LDA   NVR3H
      ADC   NVR6H
      STA   NVR4H
      BCC   :+
      JMP   ramheap_no_memory
:
      CMP   #>__NOVA_RAM_END
      BCC   @merge
      BEQ   :+
      JMP   ramheap_no_memory
:
      LDA   NVR4L
      CMP   #<__NOVA_RAM_END
      BCC   @merge
      BEQ   :+
      JMP   ramheap_no_memory
:
@merge:
      CLC
      LDA   NVR5L
      ADC   #RAMHEAP_HEADER
      ADC   NVR6L
      STA   NVR5L
      LDA   NVR5H
      ADC   NVR6H
      STA   NVR5H
      BCC   :+
      JMP   ramheap_no_memory
:
      LDY   #0
      LDA   NVR5L
      STA   (NVR0L),Y
      INY
      LDA   NVR5H
      STA   (NVR0L),Y
      LDA   NVR4L
      STA   NVR3L
      LDA   NVR4H
      STA   NVR3H
      BRA   @coalesce

@fit:
      LDA   NVR5H
      CMP   NVR2H
      BCS   :+
      JMP   @advance
:
      BNE   @fits
      LDA   NVR5L
      CMP   NVR2L
      BCS   :+
      JMP   @advance
:
@fits:
      ; Split only when at least one payload byte remains after a new header.
      CLC
      LDA   NVR2L
      ADC   #RAMHEAP_HEADER+1
      STA   NVR6L
      LDA   NVR2H
      ADC   #0
      STA   NVR6H
      BCS   @whole
      LDA   NVR5H
      CMP   NVR6H
      BCC   @whole
      BNE   @split
      LDA   NVR5L
      CMP   NVR6L
      BCC   @whole
@split:
      CLC
      LDA   NVR0L
      ADC   #RAMHEAP_HEADER
      ADC   NVR2L
      STA   NVR3L                       ; new free header
      LDA   NVR0H
      ADC   NVR2H
      STA   NVR3H
      SEC
      LDA   NVR5L
      SBC   NVR2L
      STA   NVR6L
      LDA   NVR5H
      SBC   NVR2H
      STA   NVR6H
      SEC
      LDA   NVR6L
      SBC   #RAMHEAP_HEADER
      STA   NVR6L
      LDA   NVR6H
      SBC   #0
      STA   NVR6H
      LDY   #0
      LDA   NVR6L
      STA   (NVR3L),Y
      INY
      LDA   NVR6H
      STA   (NVR3L),Y
      INY
      LDA   #0
      STA   (NVR3L),Y
      LDY   #0
      LDA   NVR2L
      STA   (NVR0L),Y
      INY
      LDA   NVR2H
      STA   (NVR0L),Y
@whole:
      LDY   #2
      LDA   #RAMHEAP_USED
      STA   (NVR0L),Y
      STZ   ramheap_last_status
      CLC
      LDA   NVR0L
      ADC   #RAMHEAP_HEADER
      PHA
      LDA   NVR0H
      ADC   #0
      TAX
      PLA
      CLC
      RTS

; A/X = payload pointer. The exact boundary scan rejects invalid/double frees.
; @label RAMHEAP.FREE
; @kind routine
; @symbol ndk_ram_free
; @summary Release an exact payload pointer returned by RAMHEAP.ALLOC.
; @in A/X: Payload pointer, or zero for a no-op.
; @out Carry: Clear on success, set for an invalid or repeated release.
ndk_ram_free:
      STA   NVR4L                       ; requested payload pointer
      STX   NVR4H
      ORA   NVR4L
      BEQ   @ok
      JSR   ramheap_init
      BCC   :+
      JMP   ramheap_bad_pointer
:
      LDA   ramheap_floor
      STA   NVR0L
      LDA   ramheap_floor+1
      STA   NVR0H
@block:
      JSR   ramheap_load_block
      BCC   :+
      JMP   ramheap_bad_pointer
:
      CLC
      LDA   NVR0L
      ADC   #RAMHEAP_HEADER
      STA   NVR1L
      LDA   NVR0H
      ADC   #0
      STA   NVR1H
      CMP   NVR4H
      BCC   @advance
      BEQ   :+
      JMP   ramheap_bad_pointer
:
      LDA   NVR1L
      CMP   NVR4L
      BCC   @advance
      BEQ   :+
      JMP   ramheap_bad_pointer
:
      LDY   #2
      LDA   (NVR0L),Y
      BNE   :+
      JMP   ramheap_bad_pointer
:
      LDA   #0
      STA   (NVR0L),Y
@ok:
      STZ   ramheap_last_status
      CLC
      RTS
@advance:
      LDA   NVR3L
      STA   NVR0L
      LDA   NVR3H
      STA   NVR0H
      CMP   #>__NOVA_RAM_END
      BCC   @block
      BEQ   :+
      JMP   ramheap_bad_pointer
:
      LDA   NVR0L
      CMP   #<__NOVA_RAM_END
      BCC   @block
      JMP   ramheap_bad_pointer

; Load current size into NVR5 and exclusive next-header address into NVR3.
ramheap_load_block:
      LDY   #0
      LDA   (NVR0L),Y
      STA   NVR5L
      INY
      LDA   (NVR0L),Y
      STA   NVR5H
      ORA   NVR5L
      BEQ   @bad
      INY
      LDA   (NVR0L),Y
      CMP   #2
      BCS   @bad
      CLC
      LDA   NVR0L
      ADC   #RAMHEAP_HEADER
      ADC   NVR5L
      STA   NVR3L
      LDA   NVR0H
      ADC   NVR5H
      STA   NVR3H
      BCS   @bad
      CMP   #>__NOVA_RAM_END
      BCC   @ok
      BNE   @bad
      LDA   NVR3L
      CMP   #<__NOVA_RAM_END
      BCC   @ok
      BNE   @bad
@ok:
      CLC
      RTS
@bad:
      SEC
      RTS

ramheap_init:
      LDA   ramheap_initialized
      BNE   @ok
      LDA   #<__NOVA_IMAGE_END
      STA   ramheap_floor
      STA   NVR0L
      LDA   #>__NOVA_IMAGE_END
      STA   ramheap_floor+1
      STA   NVR0H
      SEC
      LDA   #<__NOVA_RAM_END
      SBC   ramheap_floor
      STA   NVR5L
      LDA   #>__NOVA_RAM_END
      SBC   ramheap_floor+1
      STA   NVR5H
      SEC
      LDA   NVR5L
      SBC   #RAMHEAP_HEADER
      STA   NVR5L
      LDA   NVR5H
      SBC   #0
      STA   NVR5H
      BCC   @bad
      ORA   NVR5L
      BEQ   @bad
      LDY   #0
      LDA   NVR5L
      STA   (NVR0L),Y
      INY
      LDA   NVR5H
      STA   (NVR0L),Y
      INY
      LDA   #0
      STA   (NVR0L),Y
      INC   ramheap_initialized
@ok:
      CLC
      RTS
@bad:
      SEC
      RTS

; @label RAMHEAP.STATUS
; @kind routine
; @symbol ndk_ram_status
; @summary Return the most recent RAM heap status code.
; @out A: RAMHEAP_OK or a RAMHEAP_ERR_* code.
ndk_ram_status:
      LDA   ramheap_last_status
      RTS

ramheap_no_memory:
      LDA   #RAMHEAP_ERR_NOMEM
      BRA   ramheap_alloc_error
ramheap_bad_size:
      LDA   #RAMHEAP_ERR_SIZE
ramheap_alloc_error:
      STA   ramheap_last_status
      LDA   #0
      TAX
      SEC
      RTS
ramheap_bad_pointer:
      LDA   #RAMHEAP_ERR_POINTER
      STA   ramheap_last_status
      SEC
      RTS

.endif

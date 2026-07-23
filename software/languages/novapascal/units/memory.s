; Pascal-shaped, bounded XRAM blocks over the canonical MEMORY module.

.include "memory.inc"
.include "nvr.inc"
.include "dma.inc"

.ifndef NOVA_MEMORY_UNIT_IMPLEMENTATION_INCLUDED
NOVA_MEMORY_UNIT_IMPLEMENTATION_INCLUDED = 1

      .segment "CODE"

pascal_memory_call:
      STA   LIB_FN_ID
      LDA   #MODULE_ID_MEMORY
      STA   LIB_MOD_ID
      JMP   LIB_LOADER_BAND

.if .referenced(ramalloc)
      .export ramalloc
ramalloc:
      TSX
      LDA   $0103,X
      PHA
      LDA   $0104,X
      TAX
      PLA
      JSR   ndk_ram_alloc
      STA   NVR6L
      STX   NVR6H
      TSX
      LDA   $0105,X
      STA   NVR0L
      LDA   $0106,X
      STA   NVR0H
      LDA   NVR6L
      ORA   NVR6H
      BEQ   @clear
      LDY   #0
      LDA   NVR6L
      STA   (NVR0L),Y
      INY
      LDA   NVR6H
      STA   (NVR0L),Y
      INY
      LDA   $0103,X
      STA   (NVR0L),Y
      INY
      LDA   $0104,X
      STA   (NVR0L),Y
      STZ   LIB_STATUS
      RTS
@clear:
      LDA   #LERR_MEM_FAIL
      STA   LIB_STATUS
      LDY   #3
      LDA   #0
@clear_byte:
      STA   (NVR0L),Y
      DEY
      BPL   @clear_byte
      RTS
.endif

.if .referenced(ramfree)
      .export ramfree
ramfree:
      STA   NVR0L
      STX   NVR0H
      LDY   #0
      LDA   (NVR0L),Y
      STA   NVR1L
      INY
      LDA   (NVR0L),Y
      TAX
      LDA   NVR0H
      PHA
      LDA   NVR0L
      PHA
      LDA   NVR1L
      JSR   ndk_ram_free
      PLA
      STA   NVR0L
      PLA
      STA   NVR0H
      BCS   @failed
      LDY   #3
      LDA   #0
@clear:
      STA   (NVR0L),Y
      DEY
      BPL   @clear
      STZ   LIB_STATUS
      RTS
@failed:
      LDA   #LERR_MEM_FAIL
      STA   LIB_STATUS
      RTS
.endif

.if .referenced(ramread) .OR .referenced(ramwrite)
      .export ramread
      .export ramwrite
ramread:
      STZ   NVR7H
      BRA   ram_transfer
ramwrite:
      LDA   #1
      STA   NVR7H
ram_transfer:
      TSX
      STX   NVR7L
      LDA   $0109,X
      STA   NVR0L
      LDA   $010A,X
      STA   NVR0H
      LDA   $0107,X
      STA   NVR1L
      LDA   $0108,X
      STA   NVR1H
      CLC
      LDA   NVR1L
      ADC   $0103,X
      STA   NVR2L
      LDA   NVR1H
      ADC   $0104,X
      STA   NVR2H
      BCC   :+
      JMP   @bounds
:
      LDY   #3
      LDA   NVR2H
      CMP   (NVR0L),Y
      BCC   @in_bounds
      BEQ   :+
      JMP   @bounds
:
      DEY
      LDA   NVR2L
      CMP   (NVR0L),Y
      BCC   @in_bounds
      BEQ   :+
      JMP   @bounds
:
@in_bounds:
      LDA   $0103,X
      ORA   $0104,X
      BNE   :+
      STZ   LIB_STATUS
      RTS
:
      LDY   #0
      CLC
      LDA   (NVR0L),Y
      ADC   NVR1L
      STA   NVR3L
      INY
      LDA   (NVR0L),Y
      ADC   NVR1H
      STA   NVR3H
      LDX   NVR7L
      LDA   $0105,X
      STA   NVR4L
      LDA   $0106,X
      STA   NVR4H
      STZ   DMA_SRCSPACE
      STZ   DMA_DSTSPACE
      STZ   DMA_SRCH
      STZ   DMA_DSTH
      STZ   DMA_LENH
      LDA   NVR7H
      BNE   @write
      LDA   NVR3L
      STA   DMA_SRCL
      LDA   NVR3H
      STA   DMA_SRCM
      LDA   NVR4L
      STA   DMA_DSTL
      LDA   NVR4H
      STA   DMA_DSTM
      BRA   @length
@write:
      LDA   NVR4L
      STA   DMA_SRCL
      LDA   NVR4H
      STA   DMA_SRCM
      LDA   NVR3L
      STA   DMA_DSTL
      LDA   NVR3H
      STA   DMA_DSTM
@length:
      LDA   $0103,X
      STA   DMA_LENL
      LDA   $0104,X
      STA   DMA_LENM
      JSR   dma_copy
      BEQ   :+
      JMP   @bounds
:
      STZ   LIB_STATUS
      RTS
@bounds:
      LDA   #LERR_MEM_FAIL
      STA   LIB_STATUS
      RTS
.endif

.if .referenced(ramallocated)
      .export ramallocated
ramallocated:
      STA   NVR0L
      STX   NVR0H
      LDY   #2
      LDA   (NVR0L),Y
      INY
      ORA   (NVR0L),Y
      BEQ   :+
      LDA   #1
:     RTS
.endif

.if .referenced(ramstatus)
      .export ramstatus
ramstatus:
      JMP   ndk_ram_status
.endif

.if .referenced(xramalloc)
      .export xramalloc
xramalloc:
      TSX
      LDA   $0103,X
      STA   LIB_ARG2
      LDA   $0104,X
      STA   LIB_ARG2+1
      ORA   LIB_ARG2
      BEQ   @failed
      LDA   #MEM_ALLOC
      JSR   pascal_memory_call
      TSX
      LDA   $0105,X
      STA   NVR0L
      LDA   $0106,X
      STA   NVR0H
      LDA   LIB_STATUS
      BNE   @clear
      LDY   #0
      LDA   LIB_RESULT
      STA   (NVR0L),Y
      INY
      LDA   LIB_RESULT+1
      STA   (NVR0L),Y
      INY
      LDA   LIB_RESULT+2
      STA   (NVR0L),Y
      INY
      LDA   $0103,X
      STA   (NVR0L),Y
      INY
      LDA   $0104,X
      STA   (NVR0L),Y
      RTS
@failed:
      LDA   #LERR_MEM_FAIL
      STA   LIB_STATUS
      LDA   $0105,X
      STA   NVR0L
      LDA   $0106,X
      STA   NVR0H
@clear:
      LDY   #4
      LDA   #0
@clear_byte:
      STA   (NVR0L),Y
      DEY
      BPL   @clear_byte
      RTS
.endif

.if .referenced(xramfree)
      .export xramfree
xramfree:
      STA   NVR0L
      STX   NVR0H
      LDY   #3
      LDA   (NVR0L),Y
      STA   LIB_ARG2
      INY
      LDA   (NVR0L),Y
      STA   LIB_ARG2+1
      ORA   LIB_ARG2
      BEQ   @invalid
      LDY   #0
      LDA   (NVR0L),Y
      STA   LIB_ARG0
      INY
      LDA   (NVR0L),Y
      STA   LIB_ARG0+1
      INY
      LDA   (NVR0L),Y
      STA   LIB_ARG0+2
      LDA   NVR0H
      PHA
      LDA   NVR0L
      PHA
      LDA   #MEM_RELEASE
      JSR   pascal_memory_call
      PLA
      STA   NVR0L
      PLA
      STA   NVR0H
      LDA   LIB_STATUS
      BNE   @done
      LDY   #4
      LDA   #0
@clear:
      STA   (NVR0L),Y
      DEY
      BPL   @clear
@done:
      RTS
@invalid:
      LDA   #LERR_MEM_FAIL
      STA   LIB_STATUS
      RTS
.endif

.if .referenced(xramread) .OR .referenced(xramwrite)
      .export xramread
      .export xramwrite
xramread:
      LDA   #MEM_COPY_TO_RAM
      BRA   xram_transfer
xramwrite:
      LDA   #MEM_COPY_FROM_RAM
xram_transfer:
      STA   LIB_FN_ID
      TSX
      LDA   $0109,X
      STA   NVR0L
      LDA   $010A,X
      STA   NVR0H
      LDA   $0107,X
      STA   NVR1L
      LDA   $0108,X
      STA   NVR1H
      CLC
      LDA   NVR1L
      ADC   $0103,X
      STA   NVR2L
      LDA   NVR1H
      ADC   $0104,X
      STA   NVR2H
      BCC   :+
      JMP   @bounds
:
      LDY   #4
      LDA   NVR2H
      CMP   (NVR0L),Y
      BCC   @in_bounds
      BEQ   :+
      JMP   @bounds
:
      DEY
      LDA   NVR2L
      CMP   (NVR0L),Y
      BCC   @in_bounds
      BEQ   :+
      JMP   @bounds
:
@in_bounds:
      LDA   $0103,X
      ORA   $0104,X
      BNE   :+
      STZ   LIB_STATUS
      RTS
:
      LDY   #0
      CLC
      LDA   (NVR0L),Y
      ADC   NVR1L
      STA   LIB_ARG0
      INY
      LDA   (NVR0L),Y
      ADC   NVR1H
      STA   LIB_ARG0+1
      INY
      LDA   (NVR0L),Y
      ADC   #0
      STA   LIB_ARG0+2
      LDA   $0105,X
      STA   LIB_ARG1
      LDA   $0106,X
      STA   LIB_ARG1+1
      LDA   $0103,X
      STA   LIB_ARG2
      LDA   $0104,X
      STA   LIB_ARG2+1
      LDA   LIB_FN_ID
      JSR   pascal_memory_call
      RTS
@bounds:
      LDA   #LERR_MEM_FAIL
      STA   LIB_STATUS
      RTS
.endif

.if .referenced(xramallocated)
      .export xramallocated
xramallocated:
      STA   NVR0L
      STX   NVR0H
      LDY   #3
      LDA   (NVR0L),Y
      INY
      ORA   (NVR0L),Y
      BEQ   :+
      LDA   #1
:     RTS
.endif

.if .referenced(xramstatus)
      .export xramstatus
xramstatus:
      LDA   LIB_STATUS
      RTS
.endif

      .include "ramheap.s"
.if .referenced(ramread) .OR .referenced(ramwrite)
      .include "dma.s"
.endif

.endif

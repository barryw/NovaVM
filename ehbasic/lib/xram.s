; Shared NovaVM XRAM runtime.
;
; This file is intentionally BASIC-agnostic. BASIC enters through the XMC
; register contract, while assembly code can set XRAM_ADDRL/M/H directly to
; use XRAM as a flat 24-bit address space.

.include "xram.inc"
.include "dma.s"
.include "fio.s"

.ifndef XRAM_IMPLEMENTATION_INCLUDED
XRAM_IMPLEMENTATION_INCLUDED = 1

      .segment "CODE"

      .export xram_set_ok
      .export xram_set_error
      .export xram_addr_is_in_range
      .export xram_map_window3
      .export xram_read8
      .export xram_write8
      .export xram_copy_from_ram
      .export xram_copy_to_ram
      .export xram_fill
      .export xram_xload
      .export xram_xsave
      .export xram_wait_dma
      .export xram_xmc_read8
      .export xram_xmc_write8
      .export xram_xmc_copy_from_ram
      .export xram_xmc_copy_to_ram
      .export xram_xmc_fill

; ---------------------------------------------------------------------
; Status helpers
; ---------------------------------------------------------------------
; @label XRAM.SET_OK
; @kind routine
; @symbol xram_set_ok
; @summary Set XMC-compatible XRAM status to OK and return A=0.
xram_set_ok:
      LDA   #XMC_OK
      STA   XMC_STATUS
      LDA   #$00
      STA   XMC_ERRCODE
      RTS

; Input: A = XMC-compatible error code.
; @label XRAM.SET_ERROR
; @kind routine
; @symbol xram_set_error
; @summary Set XMC-compatible XRAM error status.
; @in A: XMC-compatible error code.
; @out A: 1.
xram_set_error:
      PHA
      LDA   #XMC_STATUS_ERROR
      STA   XMC_STATUS
      PLA
      STA   XMC_ERRCODE
      LDA   #$01
      RTS

; ---------------------------------------------------------------------
; XMC MMIO wrappers
; ---------------------------------------------------------------------
xram_xmc_load_addr:
      LDA   XMC_XAL
      STA   XRAM_ADDRL
      LDA   XMC_XAM
      STA   XRAM_ADDRM
      LDA   XMC_XAH
      STA   XRAM_ADDRH
      RTS

xram_xmc_load_ram:
      LDA   XMC_RAML
      STA   XRAM_RAML
      LDA   XMC_RAMH
      STA   XRAM_RAMH
      RTS

xram_xmc_load_len:
      LDA   XMC_LENL
      STA   XRAM_LENL
      LDA   XMC_LENH
      STA   XRAM_LENH
      RTS

; @label XRAM.XMC_READ8
; @kind routine
; @symbol xram_xmc_read8
; @summary Read one byte from XMC_XAL/M/H into XMC_DATA.
; @requires XMC_XAL XMC_XAM XMC_XAH
xram_xmc_read8:
      JSR   xram_xmc_load_addr
      JSR   xram_read8
      BNE   @done
      LDA   XRAM_DATA
      STA   XMC_DATA
      LDA   #$00
@done:
      RTS

; @label XRAM.XMC_WRITE8
; @kind routine
; @symbol xram_xmc_write8
; @summary Write XMC_DATA to XMC_XAL/M/H.
; @requires XMC_XAL XMC_XAM XMC_XAH XMC_DATA
xram_xmc_write8:
      JSR   xram_xmc_load_addr
      LDA   XMC_DATA
      STA   XRAM_DATA
      JMP   xram_write8

; @label XRAM.XMC_COPY_FROM_RAM
; @kind routine
; @symbol xram_xmc_copy_from_ram
; @summary Copy RAM to XRAM using the XMC register ABI.
; @requires XMC_RAML XMC_RAMH XMC_XAL XMC_XAM XMC_XAH XMC_LENL XMC_LENH
xram_xmc_copy_from_ram:
      JSR   xram_xmc_load_addr
      JSR   xram_xmc_load_ram
      JSR   xram_xmc_load_len
      JMP   xram_copy_from_ram

; @label XRAM.XMC_COPY_TO_RAM
; @kind routine
; @symbol xram_xmc_copy_to_ram
; @summary Copy XRAM to RAM using the XMC register ABI.
; @requires XMC_XAL XMC_XAM XMC_XAH XMC_RAML XMC_RAMH XMC_LENL XMC_LENH
xram_xmc_copy_to_ram:
      JSR   xram_xmc_load_addr
      JSR   xram_xmc_load_ram
      JSR   xram_xmc_load_len
      JMP   xram_copy_to_ram

; @label XRAM.XMC_FILL
; @kind routine
; @symbol xram_xmc_fill
; @summary Fill XRAM using the XMC register ABI.
; @requires XMC_XAL XMC_XAM XMC_XAH XMC_LENL XMC_LENH XMC_DATA
xram_xmc_fill:
      JSR   xram_xmc_load_addr
      JSR   xram_xmc_load_len
      LDA   XMC_DATA
      STA   XRAM_DATA
      JMP   xram_fill

; ---------------------------------------------------------------------
; Direct file streaming
;
; Assembly ABI:
;   XRAM_ADDRL/M/H    flat 24-bit XRAM source/destination
;   XRAM_LENL/H       transfer length; zero means full file for xram_xload
;   XRAM_NAMELEN      filename length, 1..63
;   XRAM_NAMEPTR_L/H  CPU pointer to filename bytes
;
; On success, XRAM_LENL/H is updated with FIO_SIZEL/H, the actual byte count.
; ---------------------------------------------------------------------
; @label XRAM.XLOAD
; @kind routine
; @symbol xram_xload
; @summary Stream a file directly into XRAM using the XRAM pseudo-register ABI.
; @requires XRAM_ADDRL XRAM_ADDRM XRAM_ADDRH XRAM_LENL XRAM_LENH XRAM_NAMELEN XRAM_NAMEPTR_L XRAM_NAMEPTR_H
xram_xload:
      JSR   fio_prepare_xram_transfer
      BNE   @done
      LDA   #FIO_CMD_XLOAD
      JSR   fio_run
      BNE   @done
      JSR   xram_store_fio_size
      JMP   xram_set_ok
@done:
      JMP   xram_map_fio_error

; @label XRAM.XSAVE
; @kind routine
; @symbol xram_xsave
; @summary Stream XRAM directly to a file using the XRAM pseudo-register ABI.
; @requires XRAM_ADDRL XRAM_ADDRM XRAM_ADDRH XRAM_LENL XRAM_LENH XRAM_NAMELEN XRAM_NAMEPTR_L XRAM_NAMEPTR_H
xram_xsave:
      JSR   fio_prepare_xram_transfer
      BNE   @done
      LDA   #FIO_CMD_XSAVE
      JSR   fio_run
      BNE   @done
      JSR   xram_store_fio_size
      JMP   xram_set_ok
@done:
      JMP   xram_map_fio_error

xram_map_fio_error:
      LDA   FIO_ERRCODE
      CMP   #FIO_ERR_NOTFOUND
      BEQ   @not_found
      LDA   #XRAM_XMC_ERR_BADARGS
      JMP   xram_set_error
@not_found:
      LDA   #XRAM_XMC_ERR_NOTFOUND
      JMP   xram_set_error

xram_store_fio_size:
      LDA   FIO_SIZEL
      STA   XRAM_LENL
      LDA   FIO_SIZEH
      STA   XRAM_LENH
      RTS

; Validate a single-byte flat XRAM address. This prevents hardware-window
; users from accidentally wrapping high addresses back into the 512KB device.
; @label XRAM.ADDR_IN_RANGE
; @kind routine
; @symbol xram_addr_is_in_range
; @summary Validate XRAM_ADDRH is inside the 512KB XRAM device.
; @requires XRAM_ADDRH
xram_addr_is_in_range:
      LDA   XRAM_ADDRH
      CMP   #XRAM_CAPACITY_HIGH
      BCC   @ok
      LDA   #XRAM_XMC_ERR_RANGE
      JMP   xram_set_error
@ok:
      LDA   #$00
      RTS

; ---------------------------------------------------------------------
; Windowed byte access
; ---------------------------------------------------------------------
; @label XRAM.MAP_WINDOW3
; @kind routine
; @symbol xram_map_window3
; @summary Map XRAM window 3 to XRAM_ADDRM/H.
; @requires XRAM_ADDRM XRAM_ADDRH
xram_map_window3:
      LDA   XMC_WINCTL
      ORA   #XRAM_WIN3_ENABLE
      STA   XMC_WINCTL
      STZ   WIN3_LO
      LDA   XRAM_ADDRM
      STA   WIN3_MI
      LDA   XRAM_ADDRH
      STA   WIN3_HI
      RTS

; @label XRAM.READ8
; @kind routine
; @symbol xram_read8
; @summary Read one byte from XRAM_ADDRL/M/H into XRAM_DATA.
; @requires XRAM_ADDRL XRAM_ADDRM XRAM_ADDRH
; @out XRAM_DATA: Byte read from XRAM.
xram_read8:
      JSR   xram_addr_is_in_range
      BNE   @done
      JSR   xram_map_window3
      LDX   XRAM_ADDRL
      LDA   XRAM_WIN3_BASE,X
      STA   XRAM_DATA
      JMP   xram_set_ok
@done:
      RTS

; @label XRAM.WRITE8
; @kind routine
; @symbol xram_write8
; @summary Write XRAM_DATA to XRAM_ADDRL/M/H.
; @requires XRAM_ADDRL XRAM_ADDRM XRAM_ADDRH XRAM_DATA
xram_write8:
      JSR   xram_addr_is_in_range
      BNE   @done
      JSR   xram_map_window3
      LDX   XRAM_ADDRL
      LDA   XRAM_DATA
      STA   XRAM_WIN3_BASE,X
      JMP   xram_set_ok
@done:
      RTS

; ---------------------------------------------------------------------
; DMA bulk transfers
; ---------------------------------------------------------------------
; @label XRAM.COPY_FROM_RAM
; @kind routine
; @symbol xram_copy_from_ram
; @summary Copy CPU RAM to XRAM using the XRAM pseudo-register ABI.
; @requires XRAM_RAML XRAM_RAMH XRAM_ADDRL XRAM_ADDRM XRAM_ADDRH XRAM_LENL XRAM_LENH
xram_copy_from_ram:
      LDA   #DMA_SPACE_CPU
      STA   DMA_SRCSPACE
      STZ   DMA_SRCH
      LDA   XRAM_RAML
      STA   DMA_SRCL
      LDA   XRAM_RAMH
      STA   DMA_SRCM

      LDA   #DMA_SPACE_XRAM
      STA   DMA_DSTSPACE
      LDA   XRAM_ADDRL
      STA   DMA_DSTL
      LDA   XRAM_ADDRM
      STA   DMA_DSTM
      LDA   XRAM_ADDRH
      STA   DMA_DSTH

      JSR   xram_set_dma_len16
      JSR   dma_start_copy
      JMP   xram_dma_result

; @label XRAM.COPY_TO_RAM
; @kind routine
; @symbol xram_copy_to_ram
; @summary Copy XRAM to CPU RAM using the XRAM pseudo-register ABI.
; @requires XRAM_ADDRL XRAM_ADDRM XRAM_ADDRH XRAM_RAML XRAM_RAMH XRAM_LENL XRAM_LENH
xram_copy_to_ram:
      LDA   #DMA_SPACE_XRAM
      STA   DMA_SRCSPACE
      LDA   XRAM_ADDRL
      STA   DMA_SRCL
      LDA   XRAM_ADDRM
      STA   DMA_SRCM
      LDA   XRAM_ADDRH
      STA   DMA_SRCH

      LDA   #DMA_SPACE_CPU
      STA   DMA_DSTSPACE
      STZ   DMA_DSTH
      LDA   XRAM_RAML
      STA   DMA_DSTL
      LDA   XRAM_RAMH
      STA   DMA_DSTM

      JSR   xram_set_dma_len16
      JSR   dma_start_copy
      JMP   xram_dma_result

; @label XRAM.FILL
; @kind routine
; @symbol xram_fill
; @summary Fill XRAM using XRAM_ADDRL/M/H, XRAM_LENL/H, and XRAM_DATA.
; @requires XRAM_ADDRL XRAM_ADDRM XRAM_ADDRH XRAM_LENL XRAM_LENH XRAM_DATA
xram_fill:
      LDA   #DMA_SPACE_CPU
      STA   DMA_SRCSPACE
      STZ   DMA_SRCL
      STZ   DMA_SRCM
      STZ   DMA_SRCH

      LDA   #DMA_SPACE_XRAM
      STA   DMA_DSTSPACE
      LDA   XRAM_ADDRL
      STA   DMA_DSTL
      LDA   XRAM_ADDRM
      STA   DMA_DSTM
      LDA   XRAM_ADDRH
      STA   DMA_DSTH

      JSR   xram_set_dma_len16
      LDA   XRAM_DATA
      STA   DMA_FILLVALUE
      JSR   dma_start_fill
      JMP   xram_dma_result

xram_set_dma_len16:
      LDA   XRAM_LENL
      STA   DMA_LENL
      LDA   XRAM_LENH
      STA   DMA_LENM
      STZ   DMA_LENH
      RTS

; @label XRAM.WAIT_DMA
; @kind routine
; @symbol xram_wait_dma
; @summary Wait for the shared DMA controller and map the result to XRAM status.
xram_wait_dma:
      JSR   dma_wait

xram_dma_result:
      CMP   #DMA_RESULT_OK
      BEQ   @ok

      LDA   DMA_ERRCODE_REG
      CMP   #DMA_ERR_RANGE
      BEQ   @range
      LDA   #XRAM_XMC_ERR_BADARGS
      JMP   xram_set_error
@range:
      LDA   #XRAM_XMC_ERR_RANGE
      JMP   xram_set_error
@ok:
      JMP   xram_set_ok

.endif

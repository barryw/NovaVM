; Shared NovaVM XRAM runtime.
;
; This file is intentionally BASIC-agnostic. BASIC enters through the XMC
; register contract, while assembly code can set XRAM_ADDRL/M/H directly to
; use XRAM as a flat 24-bit address space.

.include "xram.inc"

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
xram_set_ok:
      LDA   #XMC_OK
      STA   XMC_STATUS
      LDA   #$00
      STA   XMC_ERRCODE
      RTS

; Input: A = XMC-compatible error code.
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

xram_xmc_read8:
      JSR   xram_xmc_load_addr
      JSR   xram_read8
      BNE   @done
      LDA   XRAM_DATA
      STA   XMC_DATA
      LDA   #$00
@done:
      RTS

xram_xmc_write8:
      JSR   xram_xmc_load_addr
      LDA   XMC_DATA
      STA   XRAM_DATA
      JMP   xram_write8

xram_xmc_copy_from_ram:
      JSR   xram_xmc_load_addr
      JSR   xram_xmc_load_ram
      JSR   xram_xmc_load_len
      JMP   xram_copy_from_ram

xram_xmc_copy_to_ram:
      JSR   xram_xmc_load_addr
      JSR   xram_xmc_load_ram
      JSR   xram_xmc_load_len
      JMP   xram_copy_to_ram

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
xram_xload:
      JSR   xram_prepare_fio_file
      BNE   @done
      LDA   #FIO_CMD_XLOAD
      JSR   xram_run_fio
      BNE   @done
      JSR   xram_store_fio_size
      JMP   xram_set_ok
@done:
      RTS

xram_xsave:
      JSR   xram_prepare_fio_file
      BNE   @done
      LDA   #FIO_CMD_XSAVE
      JSR   xram_run_fio
      BNE   @done
      JSR   xram_store_fio_size
      JMP   xram_set_ok
@done:
      RTS

xram_prepare_fio_file:
      LDA   XRAM_NAMELEN
      BEQ   @bad
      CMP   #$40              ; FIO filename limit is 63 bytes
      BCS   @bad
      STA   FIO_NAMELEN
      TAX
      LDY   #$00
@copy_name:
      LDA   (XRAM_NAMEPTR_L),Y
      STA   FIO_NAME,Y
      INY
      DEX
      BNE   @copy_name

      LDA   XRAM_ADDRL
      STA   FIO_GADDRL
      LDA   XRAM_ADDRM
      STA   FIO_GADDRH
      LDA   XRAM_ADDRH
      STA   FIO_GSPACE
      LDA   XRAM_LENL
      STA   FIO_GLENL
      LDA   XRAM_LENH
      STA   FIO_GLENH
      LDA   #$00
      RTS
@bad:
      LDA   #FIO_STATUS_ERROR
      STA   FIO_STATUS
      LDA   #FIO_ERR_IO
      STA   FIO_ERRCODE
      LDA   #XRAM_XMC_ERR_BADARGS
      JMP   xram_set_error

xram_run_fio:
      PHA
      STZ   FIO_STATUS
      STZ   FIO_ERRCODE
      PLA
      STA   FIO_CMD
@wait_status:
      LDA   FIO_STATUS
      BEQ   @wait_status
      CMP   #FIO_OK
      BEQ   @ok
      LDA   FIO_ERRCODE
      CMP   #FIO_ERR_NOTFOUND
      BEQ   @not_found
      LDA   #XRAM_XMC_ERR_BADARGS
      JMP   xram_set_error
@not_found:
      LDA   #XRAM_XMC_ERR_NOTFOUND
      JMP   xram_set_error
@ok:
      LDA   #$00
      RTS

xram_store_fio_size:
      LDA   FIO_SIZEL
      STA   XRAM_LENL
      LDA   FIO_SIZEH
      STA   XRAM_LENH
      RTS

; Validate a single-byte flat XRAM address. This prevents hardware-window
; users from accidentally wrapping high addresses back into the 512KB device.
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
      STZ   DMA_MODE_REG
      LDA   #DMA_CMD_START
      STA   DMA_CMD_REG
      JMP   xram_wait_dma

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
      STZ   DMA_MODE_REG
      LDA   #DMA_CMD_START
      STA   DMA_CMD_REG
      JMP   xram_wait_dma

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
      LDA   #DMA_MODE_FILL
      STA   DMA_MODE_REG
      LDA   #DMA_CMD_START
      STA   DMA_CMD_REG
      JMP   xram_wait_dma

xram_set_dma_len16:
      LDA   XRAM_LENL
      STA   DMA_LENL
      LDA   XRAM_LENH
      STA   DMA_LENM
      STZ   DMA_LENH
      RTS

xram_wait_dma:
@loop:
      LDA   DMA_STATUS_REG
      CMP   #DMA_STATUS_BUSY
      BEQ   @loop
      CMP   #DMA_STATUS_OK
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

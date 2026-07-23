; memory.s — MEMORY module (MODULE_ID_MEMORY = $05). THIN lib_call wrappers over
; the expansion-RAM NDK: the XRAM core runtime (runtime/asm/xram.s) and the XMC
; named-block allocator + window controller (runtime/asm/xmc.s). The NDK owns all
; XRAM/XMC logic; this module only marshals the mailbox args into the NDK
; routine's inputs (the XRAM_* ZP pseudo-registers or the XMC_* MMIO registers)
; and JSRs it.
;
; Coverage: flat 24-bit XRAM byte read/write/fill, RAM<->XRAM DMA copy, direct
; file streaming (xload/xsave), the XMC-register byte/DMA path, bank select, raw
; put/stash/fetch/release, the page allocator + reset, named-block stash/fetch/
; delete + directory enumeration, the four hardware mapping windows, and the raw
; XMC command-status reporter. Per ModuleNdkContractTests this module's own code
; never pokes SID registers or busy-waits the VGC frame counter — every fn wraps
; an NDK routine.
;
; Naming: ids are MEM_* (the memory-domain library, see libmemory.inc). XRAM_*
; and XMC_* are the ZP-pseudo-register / MMIO-register prefixes (xram.inc,
; nova.inc) — different namespaces.

      .include "libabi.inc"
      .include "libmod.inc"
      .include "libmemory.inc"
      .include "nova.inc"              ; NVR* scratch + XMC MMIO registers
      .include "xram.inc"            ; XRAM_* ZP pseudo-register aliases
      .include "dma.inc"
      .include "novaexec.inc"

      .segment "CODE"
      lib_module_header MODULE_ID_MEMORY, LIB_ABI_VERSION, MEM_FN_COUNT

; ===========================================================================
; NDOC documentation (extracted by nova module pack into memory.nmod).
;
; Each ;@ndk line maps the fn to the NDK routine it wraps; the human summary is
; single-sourced from that routine (feedback-ndk-source-of-truth). The build-time
; + CI drift guard requires the wrapper body to actually JSR that routine. Mailbox
; @arg bindings stay here — they are this wrapper's marshalling, not duplicated.
; ===========================================================================
;@module MEMORY
;@version 1.0
;@brief Shared expansion-memory services: the xram + xmc NDK (thin wrappers over xram.s/xmc.s).
;
;@fn MEM_READ8
;@ndk xram_read8
;@arg xaddr u24 flat XRAM address -> XRAM_ADDRL/M/H (ARG0 byte0,1,2)
;@ret u8 byte read from XRAM_DATA (RESULT byte0)
;@status LERR_OK, LERR_MEM_FAIL
;
;@fn MEM_WRITE8
;@ndk xram_write8
;@arg xaddr u24 flat XRAM address -> XRAM_ADDRL/M/H (ARG0 byte0,1,2)
;@arg val u8 byte value -> XRAM_DATA (ARG3 byte0)
;@ret void
;@status LERR_OK, LERR_MEM_FAIL
;
;@fn MEM_COPY_FROM_RAM
;@ndk xram_copy_from_ram
;@arg xaddr u24 destination XRAM address -> XRAM_ADDRL/M/H (ARG0 byte0,1,2)
;@arg ram u16 source CPU RAM address -> XRAM_RAML/H (ARG1 byte0,1)
;@arg len u16 byte count -> XRAM_LENL/H (ARG2 byte0,1)
;@ret void
;@status LERR_OK, LERR_MEM_FAIL
;
;@fn MEM_COPY_TO_RAM
;@ndk xram_copy_to_ram
;@arg xaddr u24 source XRAM address -> XRAM_ADDRL/M/H (ARG0 byte0,1,2)
;@arg ram u16 destination CPU RAM address -> XRAM_RAML/H (ARG1 byte0,1)
;@arg len u16 byte count -> XRAM_LENL/H (ARG2 byte0,1)
;@ret void
;@status LERR_OK, LERR_MEM_FAIL
;
;@fn MEM_FILL
;@ndk xram_fill
;@arg xaddr u24 destination XRAM address -> XRAM_ADDRL/M/H (ARG0 byte0,1,2)
;@arg len u16 byte count -> XRAM_LENL/H (ARG2 byte0,1)
;@arg val u8 fill byte -> XRAM_DATA (ARG3 byte0)
;@ret void
;@status LERR_OK, LERR_MEM_FAIL
;
;@fn MEM_XLOAD
;@ndk xram_xload
;@arg nameptr u16 pointer to filename bytes -> XRAM_NAMEPTR_L/H (ARG0 byte0,1)
;@arg namelen u8 filename length 1..63 -> XRAM_NAMELEN (ARG1 byte0)
;@arg xaddr u24 destination XRAM address -> XRAM_ADDRL/M/H (ARG2 byte0,1,2)
;@arg len u16 byte count, 0=full file -> XRAM_LENL/H (ARG3 byte0,1)
;@ret void
;@status LERR_OK, LERR_MEM_FAIL
;
;@fn MEM_XSAVE
;@ndk xram_xsave
;@arg nameptr u16 pointer to filename bytes -> XRAM_NAMEPTR_L/H (ARG0 byte0,1)
;@arg namelen u8 filename length 1..63 -> XRAM_NAMELEN (ARG1 byte0)
;@arg xaddr u24 source XRAM address -> XRAM_ADDRL/M/H (ARG2 byte0,1,2)
;@arg len u16 byte count -> XRAM_LENL/H (ARG3 byte0,1)
;@ret void
;@status LERR_OK, LERR_MEM_FAIL
;
;@fn MEM_XMC_READ8
;@ndk xram_xmc_read8
;@arg xaddr u24 flat XRAM address -> XMC_XAL/M/H (ARG0 byte0,1,2)
;@ret u8 byte read from XMC_DATA (RESULT byte0)
;@status LERR_OK, LERR_MEM_FAIL
;
;@fn MEM_XMC_WRITE8
;@ndk xram_xmc_write8
;@arg xaddr u24 flat XRAM address -> XMC_XAL/M/H (ARG0 byte0,1,2)
;@arg val u8 byte value -> XMC_DATA (ARG3 byte0)
;@ret void
;@status LERR_OK, LERR_MEM_FAIL
;
;@fn MEM_XMC_COPY_FROM_RAM
;@ndk xram_xmc_copy_from_ram
;@arg xaddr u24 destination XRAM address -> XMC_XAL/M/H (ARG0 byte0,1,2)
;@arg ram u16 source CPU RAM address -> XMC_RAML/H (ARG1 byte0,1)
;@arg len u16 byte count -> XMC_LENL/H (ARG2 byte0,1)
;@ret void
;@status LERR_OK, LERR_MEM_FAIL
;
;@fn MEM_XMC_COPY_TO_RAM
;@ndk xram_xmc_copy_to_ram
;@arg xaddr u24 source XRAM address -> XMC_XAL/M/H (ARG0 byte0,1,2)
;@arg ram u16 destination CPU RAM address -> XMC_RAML/H (ARG1 byte0,1)
;@arg len u16 byte count -> XMC_LENL/H (ARG2 byte0,1)
;@ret void
;@status LERR_OK, LERR_MEM_FAIL
;
;@fn MEM_XMC_FILL
;@ndk xram_xmc_fill
;@arg xaddr u24 destination XRAM address -> XMC_XAL/M/H (ARG0 byte0,1,2)
;@arg len u16 byte count -> XMC_LENL/H (ARG2 byte0,1)
;@arg val u8 fill byte -> XMC_DATA (ARG3 byte0)
;@ret void
;@status LERR_OK, LERR_MEM_FAIL
;
;@fn MEM_SELECT_BANK
;@ndk xmc_select_bank
;@arg bank u8 64KB XRAM bank index -> X (ARG3 byte0)
;@ret void
;@status LERR_OK, LERR_MEM_FAIL
;
;@fn MEM_PUT_BYTE
;@ndk xmc_put_byte
;@arg xaddr u24 flat XRAM address -> XMC_XAL/M/H (ARG0 byte0,1,2)
;@arg val u8 byte value -> XMC_DATA (ARG3 byte0)
;@ret void
;@status LERR_OK, LERR_MEM_FAIL
;
;@fn MEM_STASH_RAW
;@ndk xmc_stash_raw
;@arg xaddr u24 destination XRAM address -> XMC_XAL/M/H (ARG0 byte0,1,2)
;@arg ram u16 source CPU RAM address -> XMC_RAML/H (ARG1 byte0,1)
;@arg len u16 byte count -> XMC_LENL/H (ARG2 byte0,1)
;@ret void
;@status LERR_OK, LERR_MEM_FAIL
;
;@fn MEM_FETCH_RAW
;@ndk xmc_fetch_raw
;@arg xaddr u24 source XRAM address -> XMC_XAL/M/H (ARG0 byte0,1,2)
;@arg ram u16 destination CPU RAM address -> XMC_RAML/H (ARG1 byte0,1)
;@arg len u16 byte count -> XMC_LENL/H (ARG2 byte0,1)
;@ret void
;@status LERR_OK, LERR_MEM_FAIL
;
;@fn MEM_RELEASE
;@ndk xmc_release
;@arg xaddr u24 range start XRAM address -> XMC_XAL/M/H (ARG0 byte0,1,2)
;@arg len u16 range byte count -> XMC_LENL/H (ARG2 byte0,1)
;@ret void
;@status LERR_OK, LERR_MEM_FAIL
;
;@fn MEM_RESET_USAGE
;@ndk xmc_reset_usage
;@ret void
;@status LERR_OK, LERR_MEM_FAIL
;
;@fn MEM_ALLOC
;@ndk xmc_alloc_block
;@arg len u16 bytes to allocate -> XMC_LENL/H (ARG2 byte0,1)
;@ret u24 allocated XRAM address from XMC_XAL/M/H (RESULT byte0,1,2)
;@status LERR_OK, LERR_MEM_FAIL
;
;@fn MEM_NAMED_STASH
;@ndk xmc_named_stash
;@arg nameptr u16 pointer to name bytes -> XMC_NAME (ARG0 byte0,1)
;@arg namelen u8 name length 1..28 -> XMC_NAMELEN (ARG1 byte0)
;@arg ram u16 source CPU RAM address -> XMC_RAML/H (ARG1 byte2,3)
;@arg len u16 byte count -> XMC_LENL/H (ARG2 byte0,1)
;@ret void
;@status LERR_OK, LERR_MEM_FAIL
;
;@fn MEM_NAMED_FETCH
;@ndk xmc_named_fetch
;@arg nameptr u16 pointer to name bytes -> XMC_NAME (ARG0 byte0,1)
;@arg namelen u8 name length 1..28 -> XMC_NAMELEN (ARG1 byte0)
;@arg ram u16 destination CPU RAM address -> XMC_RAML/H (ARG1 byte2,3)
;@arg len u16 byte count, 0=full block -> XMC_LENL/H (ARG2 byte0,1)
;@ret void
;@status LERR_OK, LERR_MEM_FAIL
;
;@fn MEM_NAMED_DELETE
;@ndk xmc_named_delete
;@arg nameptr u16 pointer to name bytes -> XMC_NAME (ARG0 byte0,1)
;@arg namelen u8 name length 1..28 -> XMC_NAMELEN (ARG1 byte0)
;@ret void
;@status LERR_OK, LERR_MEM_FAIL
;
;@fn MEM_DIR_OPEN
;@ndk xmc_dir_open
;@ret void (first named entry lands in XMC_HANDLE/XAL/M/H/LENL/H + XMC_NAME)
;@status LERR_OK, LERR_MEM_FAIL
;
;@fn MEM_DIR_READ
;@ndk xmc_dir_read
;@ret void (next named entry lands in XMC_HANDLE/XAL/M/H/LENL/H + XMC_NAME)
;@status LERR_OK, LERR_MEM_FAIL
;
;@fn MEM_MAP_WINDOW
;@ndk xmc_map_window
;@arg xaddr u24 flat window base -> XMC_XAL/M + XMC_BANK (ARG0 byte0,1,2)
;@arg win u8 window index 0..3 -> X (ARG3 byte0)
;@ret void
;@status LERR_OK, LERR_MEM_FAIL
;
;@fn MEM_UNMAP_WINDOW
;@ndk xmc_unmap_window
;@arg win u8 window index 0..3 -> X (ARG3 byte0)
;@ret void
;@status LERR_OK, LERR_MEM_FAIL
;
;@fn MEM_COMMAND_STATUS
;@ndk xmc_command_status
;@ret u8 0 on XMC_OK else 1 (RESULT byte0); processes the preloaded XMC_CMD register
;@status LERR_OK
;
;@fn MEM_EXEC_IMAGE
;@brief Launch an allocator-backed Nova executable staged in XRAM.
;@arg xaddr u24 allocation base containing the two-byte Nova load/entry header (ARG0 byte0,1,2)
;@arg len u16 complete image length including the header (ARG1 byte0,1)
;@arg restart u16 stable cold-start vector used when the application returns (ARG2 byte0,1)
;@ret void on failure; success transfers control and does not return to the caller
;@effect Validates the image against NOVA_APP_RAM_START..NOVA_APP_RAM_END.
;@effect Copies initialized bytes, zero-fills a validated NL tail-BSS trailer, and releases the allocation.
;@effect On application RTS, restores the caller's ROM bank and returns through restart.
;@status LERR_OK, LERR_MEM_FAIL (RESULT byte1 = MEM_EXEC_ERR_*)
;
;@fn MEM_EXIT_IMAGE
;@brief Exit the active standalone executable from any hardware-stack depth.
;@ret void; success transfers through the restart vector supplied to MEM_EXEC_IMAGE
;@effect Discards application call frames and resumes the launching runtime.
;@status LERR_OK, LERR_MEM_FAIL (RESULT byte1 = MEM_EXEC_ERR_INACTIVE)

; ---------------------------------------------------------------------------
; dispatch — fn-id router (RTS-trick). MEM_FN_COUNT is small so fn*2 < 256.
; ---------------------------------------------------------------------------
dispatch:
      lda     LIB_FN_ID
      cmp     #MEM_FN_COUNT
      bcs     @bad
      asl
      tax
      lda     mem_jtable+1,x
      pha
      lda     mem_jtable,x
      pha
      rts
@bad:
      lda     #LERR_NO_FN
      sta     LIB_STATUS
      rts

mem_jtable:
      .word   mem_read8-1              ; $00 MEM_READ8
      .word   mem_write8-1             ; $01 MEM_WRITE8
      .word   mem_copy_from_ram-1      ; $02 MEM_COPY_FROM_RAM
      .word   mem_copy_to_ram-1        ; $03 MEM_COPY_TO_RAM
      .word   mem_fill-1               ; $04 MEM_FILL
      .word   mem_xload-1              ; $05 MEM_XLOAD
      .word   mem_xsave-1              ; $06 MEM_XSAVE
      .word   mem_xmc_read8-1          ; $07 MEM_XMC_READ8
      .word   mem_xmc_write8-1         ; $08 MEM_XMC_WRITE8
      .word   mem_xmc_copy_from_ram-1  ; $09 MEM_XMC_COPY_FROM_RAM
      .word   mem_xmc_copy_to_ram-1    ; $0A MEM_XMC_COPY_TO_RAM
      .word   mem_xmc_fill-1           ; $0B MEM_XMC_FILL
      .word   mem_select_bank-1        ; $0C MEM_SELECT_BANK
      .word   mem_put_byte-1           ; $0D MEM_PUT_BYTE
      .word   mem_stash_raw-1          ; $0E MEM_STASH_RAW
      .word   mem_fetch_raw-1          ; $0F MEM_FETCH_RAW
      .word   mem_release-1            ; $10 MEM_RELEASE
      .word   mem_reset_usage-1        ; $11 MEM_RESET_USAGE
      .word   mem_alloc-1              ; $12 MEM_ALLOC
      .word   mem_named_stash-1        ; $13 MEM_NAMED_STASH
      .word   mem_named_fetch-1        ; $14 MEM_NAMED_FETCH
      .word   mem_named_delete-1       ; $15 MEM_NAMED_DELETE
      .word   mem_dir_open-1           ; $16 MEM_DIR_OPEN
      .word   mem_dir_read-1           ; $17 MEM_DIR_READ
      .word   mem_map_window-1         ; $18 MEM_MAP_WINDOW
      .word   mem_unmap_window-1       ; $19 MEM_UNMAP_WINDOW
      .word   mem_command_status-1     ; $1A MEM_COMMAND_STATUS
      .word   mem_exec_image-1         ; $1B MEM_EXEC_IMAGE
      .word   mem_exit_image-1         ; $1C MEM_EXIT_IMAGE

; ===========================================================================
; Shared epilogues. mem_finish_status maps the NDK A result (0=OK / nonzero=err)
; to LIB_STATUS and zeroes RESULT. These are NOT ;@ndk-mapped, so the drift
; guard's per-wrapper JSR check ignores them.
; ===========================================================================
mem_finish_status:
      cmp     #$00
      bne     @fail
      stz     LIB_RESULT
      stz     LIB_RESULT+1
      stz     LIB_RESULT+2
      stz     LIB_RESULT+3
      lda     #LERR_OK
      sta     LIB_STATUS
      rts
@fail:
      lda     #LERR_MEM_FAIL
      sta     LIB_STATUS
      rts

; ---------------------------------------------------------------------------
; Shared marshalling helpers (NOT ;@ndk-mapped; ignored by the drift JSR check).
; ---------------------------------------------------------------------------

; mem_marshal_xaddr — ARG0 byte0/1/2 -> XRAM_ADDRL/M/H (24-bit flat address).
mem_marshal_xaddr:
      LDA   LIB_ARG0+0
      STA   XRAM_ADDRL
      LDA   LIB_ARG0+1
      STA   XRAM_ADDRM
      LDA   LIB_ARG0+2
      STA   XRAM_ADDRH
      RTS

; mem_marshal_xmc_xaddr — ARG0 byte0/1/2 -> XMC_XAL/M/H (24-bit flat address).
mem_marshal_xmc_xaddr:
      LDA   LIB_ARG0+0
      STA   XMC_XAL
      LDA   LIB_ARG0+1
      STA   XMC_XAM
      LDA   LIB_ARG0+2
      STA   XMC_XAH
      RTS

; mem_marshal_xram_copy — ARG0 xaddr -> XRAM_ADDR*, ARG1 ram -> XRAM_RAML/H,
; ARG2 len -> XRAM_LENL/H. Shared by the XRAM-register copy/fill fns.
mem_marshal_xram_copy:
      JSR   mem_marshal_xaddr
      LDA   LIB_ARG1+0
      STA   XRAM_RAML
      LDA   LIB_ARG1+1
      STA   XRAM_RAMH
      LDA   LIB_ARG2+0
      STA   XRAM_LENL
      LDA   LIB_ARG2+1
      STA   XRAM_LENH
      RTS

; mem_marshal_xmc_copy — ARG0 xaddr -> XMC_XA*, ARG1 ram -> XMC_RAML/H,
; ARG2 len -> XMC_LENL/H. Shared by the XMC-register copy/fill fns.
mem_marshal_xmc_copy:
      JSR   mem_marshal_xmc_xaddr
      LDA   LIB_ARG1+0
      STA   XMC_RAML
      LDA   LIB_ARG1+1
      STA   XMC_RAMH
      LDA   LIB_ARG2+0
      STA   XMC_LENL
      LDA   LIB_ARG2+1
      STA   XMC_LENH
      RTS

; mem_marshal_name — copy the pointer-named string (ARG0 = CPU pointer, ARG1
; byte0 = length 1..28) into the XMC_NAME MMIO buffer and set XMC_NAMELEN. The
; name pointer is staged through LIB_ZP for (zp),Y deref.
mem_marshal_name:
      LDA   LIB_ARG0+0
      STA   LIB_ZP
      LDA   LIB_ARG0+1
      STA   LIB_ZP+1
      LDA   LIB_ARG1+0
      STA   XMC_NAMELEN
      LDY   #$00
@cp:
      CPY   XMC_NAMELEN
      BCS   @done
      LDA   (LIB_ZP),Y
      STA   XMC_NAME,Y
      INY
      BNE   @cp
@done:
      RTS

; ===========================================================================
; Flat 24-bit XRAM byte + DMA access (XRAM_* ZP pseudo-register ABI).
; ===========================================================================

; --- $00 MEM_READ8: ARG0 xaddr -> XRAM_ADDR* -> xram_read8 -> RESULT = XRAM_DATA ---
mem_read8:
      JSR   mem_marshal_xaddr
      JSR   xram_read8
      CMP   #$00
      BNE   @fail
      LDA   XRAM_DATA
      STA   LIB_RESULT
      STZ   LIB_RESULT+1
      STZ   LIB_RESULT+2
      STZ   LIB_RESULT+3
      LDA   #LERR_OK
      STA   LIB_STATUS
      RTS
@fail:
      LDA   #LERR_MEM_FAIL
      STA   LIB_STATUS
      RTS

; --- $01 MEM_WRITE8: ARG0 xaddr, ARG3 b0 val -> xram_write8 ---
mem_write8:
      JSR   mem_marshal_xaddr
      LDA   LIB_ARG3+0
      STA   XRAM_DATA
      JSR   xram_write8
      JMP   mem_finish_status

; --- $02 MEM_COPY_FROM_RAM: ARG0 xaddr, ARG1 ram, ARG2 len -> xram_copy_from_ram ---
mem_copy_from_ram:
      JSR   mem_marshal_xram_copy
      JSR   xram_copy_from_ram
      JMP   mem_finish_status

; --- $03 MEM_COPY_TO_RAM: ARG0 xaddr, ARG1 ram, ARG2 len -> xram_copy_to_ram ---
mem_copy_to_ram:
      JSR   mem_marshal_xram_copy
      JSR   xram_copy_to_ram
      JMP   mem_finish_status

; --- $04 MEM_FILL: ARG0 xaddr, ARG2 len, ARG3 b0 val -> xram_fill ---
mem_fill:
      JSR   mem_marshal_xaddr
      LDA   LIB_ARG2+0
      STA   XRAM_LENL
      LDA   LIB_ARG2+1
      STA   XRAM_LENH
      LDA   LIB_ARG3+0
      STA   XRAM_DATA
      JSR   xram_fill
      JMP   mem_finish_status

; ===========================================================================
; Direct file streaming. The name rides as a CPU pointer (ARG0) + length
; (ARG1 b0); xram_xload/xsave copy it into FIO.NAME via fio_prepare_xram_transfer.
; ===========================================================================

; mem_marshal_xfile — name ptr/len + 24-bit xaddr + u16 len into the XRAM_*
; pseudo-registers shared by xload/xsave. (Not ;@ndk-mapped.)
mem_marshal_xfile:
      LDA   LIB_ARG0+0
      STA   XRAM_NAMEPTR_L
      LDA   LIB_ARG0+1
      STA   XRAM_NAMEPTR_H
      LDA   LIB_ARG1+0
      STA   XRAM_NAMELEN
      LDA   LIB_ARG2+0
      STA   XRAM_ADDRL
      LDA   LIB_ARG2+1
      STA   XRAM_ADDRM
      LDA   LIB_ARG2+2
      STA   XRAM_ADDRH
      LDA   LIB_ARG3+0
      STA   XRAM_LENL
      LDA   LIB_ARG3+1
      STA   XRAM_LENH
      RTS

; --- $05 MEM_XLOAD: name + ARG2 xaddr + ARG3 len -> xram_xload ---
mem_xload:
      JSR   mem_marshal_xfile
      JSR   xram_xload
      JMP   mem_finish_status

; --- $06 MEM_XSAVE: name + ARG2 xaddr + ARG3 len -> xram_xsave ---
mem_xsave:
      JSR   mem_marshal_xfile
      JSR   xram_xsave
      JMP   mem_finish_status

; ===========================================================================
; XMC-register byte + DMA access (xram_xmc_*; XMC_* MMIO ABI).
; ===========================================================================

; --- $07 MEM_XMC_READ8: ARG0 xaddr -> XMC_XA* -> xram_xmc_read8 -> RESULT=XMC_DATA ---
mem_xmc_read8:
      JSR   mem_marshal_xmc_xaddr
      JSR   xram_xmc_read8
      CMP   #$00
      BNE   @fail
      LDA   XMC_DATA
      STA   LIB_RESULT
      STZ   LIB_RESULT+1
      STZ   LIB_RESULT+2
      STZ   LIB_RESULT+3
      LDA   #LERR_OK
      STA   LIB_STATUS
      RTS
@fail:
      LDA   #LERR_MEM_FAIL
      STA   LIB_STATUS
      RTS

; --- $08 MEM_XMC_WRITE8: ARG0 xaddr, ARG3 b0 val -> xram_xmc_write8 ---
mem_xmc_write8:
      JSR   mem_marshal_xmc_xaddr
      LDA   LIB_ARG3+0
      STA   XMC_DATA
      JSR   xram_xmc_write8
      JMP   mem_finish_status

; --- $09 MEM_XMC_COPY_FROM_RAM: ARG0 xaddr, ARG1 ram, ARG2 len -> xram_xmc_copy_from_ram ---
mem_xmc_copy_from_ram:
      JSR   mem_marshal_xmc_copy
      JSR   xram_xmc_copy_from_ram
      JMP   mem_finish_status

; --- $0A MEM_XMC_COPY_TO_RAM: ARG0 xaddr, ARG1 ram, ARG2 len -> xram_xmc_copy_to_ram ---
mem_xmc_copy_to_ram:
      JSR   mem_marshal_xmc_copy
      JSR   xram_xmc_copy_to_ram
      JMP   mem_finish_status

; --- $0B MEM_XMC_FILL: ARG0 xaddr, ARG2 len, ARG3 b0 val -> xram_xmc_fill ---
mem_xmc_fill:
      JSR   mem_marshal_xmc_xaddr
      LDA   LIB_ARG2+0
      STA   XMC_LENL
      LDA   LIB_ARG2+1
      STA   XMC_LENH
      LDA   LIB_ARG3+0
      STA   XMC_DATA
      JSR   xram_xmc_fill
      JMP   mem_finish_status

; ===========================================================================
; XMC bank select + raw byte/block ops (XMC_* MMIO ABI).
; ===========================================================================

; --- $0C MEM_SELECT_BANK: ARG3 b0 bank -> X -> xmc_select_bank ---
mem_select_bank:
      LDX   LIB_ARG3+0
      JSR   xmc_select_bank
      JMP   mem_finish_status

; --- $0D MEM_PUT_BYTE: ARG0 xaddr, ARG3 b0 val -> xmc_put_byte ---
mem_put_byte:
      JSR   mem_marshal_xmc_xaddr
      LDA   LIB_ARG3+0
      STA   XMC_DATA
      JSR   xmc_put_byte
      JMP   mem_finish_status

; --- $0E MEM_STASH_RAW: ARG0 xaddr, ARG1 ram, ARG2 len -> xmc_stash_raw ---
mem_stash_raw:
      JSR   mem_marshal_xmc_copy
      JSR   xmc_stash_raw
      JMP   mem_finish_status

; --- $0F MEM_FETCH_RAW: ARG0 xaddr, ARG1 ram, ARG2 len -> xmc_fetch_raw ---
mem_fetch_raw:
      JSR   mem_marshal_xmc_copy
      JSR   xmc_fetch_raw
      JMP   mem_finish_status

; --- $10 MEM_RELEASE: ARG0 xaddr, ARG2 len -> xmc_release ---
mem_release:
      JSR   mem_marshal_xmc_xaddr
      LDA   LIB_ARG2+0
      STA   XMC_LENL
      LDA   LIB_ARG2+1
      STA   XMC_LENH
      JSR   xmc_release
      JMP   mem_finish_status

; ===========================================================================
; XMC allocator + stats.
; ===========================================================================

; --- $11 MEM_RESET_USAGE: () -> xmc_reset_usage ---
mem_reset_usage:
      JSR   xmc_reset_usage
      JMP   mem_finish_status

; --- $12 MEM_ALLOC: ARG2 len -> XMC_LENL/H -> xmc_alloc_block -> RESULT=XADDR ---
mem_alloc:
      LDA   LIB_ARG2+0
      STA   XMC_LENL
      LDA   LIB_ARG2+1
      STA   XMC_LENH
      JSR   xmc_alloc_block
      CMP   #$00
      BNE   @fail
      LDA   XMC_XAL
      STA   LIB_RESULT
      LDA   XMC_XAM
      STA   LIB_RESULT+1
      LDA   XMC_XAH
      STA   LIB_RESULT+2
      STZ   LIB_RESULT+3
      LDA   #LERR_OK
      STA   LIB_STATUS
      RTS
@fail:
      LDA   #LERR_MEM_FAIL
      STA   LIB_STATUS
      RTS

; ===========================================================================
; XMC named blocks. The name rides as a CPU pointer (ARG0) + length (ARG1 b0)
; copied into the XMC_NAME MMIO buffer; the RAM pointer rides in ARG1 b2/b3.
; ===========================================================================

; mem_marshal_named — name into XMC_NAME, ram ptr (ARG1 b2/b3) -> XMC_RAML/H,
; len (ARG2) -> XMC_LENL/H. Shared by named_stash/named_fetch. (Not ;@ndk-mapped.)
mem_marshal_named:
      JSR   mem_marshal_name
      LDA   LIB_ARG1+2
      STA   XMC_RAML
      LDA   LIB_ARG1+3
      STA   XMC_RAMH
      LDA   LIB_ARG2+0
      STA   XMC_LENL
      LDA   LIB_ARG2+1
      STA   XMC_LENH
      RTS

; --- $13 MEM_NAMED_STASH: name + ram + len -> xmc_named_stash ---
mem_named_stash:
      JSR   mem_marshal_named
      JSR   xmc_named_stash
      JMP   mem_finish_status

; --- $14 MEM_NAMED_FETCH: name + ram + len(0=all) -> xmc_named_fetch ---
mem_named_fetch:
      JSR   mem_marshal_named
      JSR   xmc_named_fetch
      JMP   mem_finish_status

; --- $15 MEM_NAMED_DELETE: name -> xmc_named_delete ---
mem_named_delete:
      JSR   mem_marshal_name
      JSR   xmc_named_delete
      JMP   mem_finish_status

; ===========================================================================
; XMC named-block directory enumeration. Entry fields land in the XMC_* result
; registers (XMC_HANDLE/XAL/M/H/LENL/H + XMC_NAME/XMC_NAMELEN).
; ===========================================================================

; --- $16 MEM_DIR_OPEN: () -> xmc_dir_open ---
mem_dir_open:
      JSR   xmc_dir_open
      JMP   mem_finish_status

; --- $17 MEM_DIR_READ: () -> xmc_dir_read ---
mem_dir_read:
      JSR   xmc_dir_read
      JMP   mem_finish_status

; ===========================================================================
; XMC hardware mapping windows.
; ===========================================================================

; --- $18 MEM_MAP_WINDOW: ARG0 u24 base -> XMC_XAL/M + bank, ARG3 b0 win -> xmc_map_window ---
mem_map_window:
      LDA   LIB_ARG0+0
      STA   XMC_XAL
      LDA   LIB_ARG0+1
      STA   XMC_XAM
      LDA   LIB_ARG0+2
      STA   XMC_BANK
      LDX   LIB_ARG3+0
      JSR   xmc_map_window
      JMP   mem_finish_status

; --- $19 MEM_UNMAP_WINDOW: ARG3 b0 win -> X -> xmc_unmap_window ---
mem_unmap_window:
      LDX   LIB_ARG3+0
      JSR   xmc_unmap_window
      JMP   mem_finish_status

; ===========================================================================
; Raw XMC command-status reporter — runs the preloaded XMC_CMD and reports the
; 0/1 result to LIB_RESULT byte0 (always LERR_OK).
; ===========================================================================

; --- $1A MEM_COMMAND_STATUS: () -> RESULT = 0/1 -> xmc_command_status ---
mem_command_status:
      JSR   xmc_command_status
      STA   LIB_RESULT
      STZ   LIB_RESULT+1
      STZ   LIB_RESULT+2
      STZ   LIB_RESULT+3
      LDA   #LERR_OK
      STA   LIB_STATUS
      RTS

; ===========================================================================
; Destructive executable transfer. The caller must enter through lib_call:
; beneath the module-return address, its saved status byte and caller-return
; address form a stable stack frame. Replacing that caller return with ARG2-1
; makes a normal application RTS flow through lib_call's bank/flag restoration
; and then into the host cold-start vector.
;
; ARG0 is preserved as the allocation base. ARG3 receives the full allocation
; length for release; ARG1 becomes the payload length; RESULT receives the load
; address. The nine-byte RAM trampoline performs the ROM-bank switch because a
; module cannot execute its next instruction from $C000 after changing banks.
; ===========================================================================

      .segment "BSS"
mem_exec_trampoline: .res 9
      .segment "CODE"

mem_exec_image:
      LDA   LIB_ARG1+0
      STA   LIB_ARG3+0
      LDA   LIB_ARG1+1
      STA   LIB_ARG3+1
      BNE   @length_ok
      LDA   LIB_ARG1+0
      CMP   #3
      BCS   :+
      JMP   @bad_image
:
@length_ok:
      JSR   mem_marshal_xaddr
      JSR   mem_exec_read_next
      BCC   :+
      JMP   @xram_error
:
      STA   LIB_RESULT+0
      JSR   mem_exec_read_next
      BCC   :+
      JMP   @xram_error
:
      STA   LIB_RESULT+1
      STZ   LIB_SCRATCH+3
      STZ   LIB_SCRATCH+4
      SEC
      LDA   LIB_ARG1+0
      SBC   #2
      STA   LIB_ARG1+0
      LDA   LIB_ARG1+1
      SBC   #0
      STA   LIB_ARG1+1

      ; Legacy images are [load:u16, payload]. NL appends NBS1 plus a
      ; length/complement pair only when omitting a larger zero-filled tail.
      LDA   LIB_ARG3+1
      BNE   @check_trailer
      LDA   LIB_ARG3
      CMP   #2+NOVA_EXEC_BSS_TRAILER_SIZE
      BCS   @check_trailer
      JMP   @source_ready
@check_trailer:
      SEC
      LDA   LIB_ARG3
      SBC   #NOVA_EXEC_BSS_TRAILER_SIZE
      STA   LIB_SCRATCH+0
      LDA   LIB_ARG3+1
      SBC   #0
      STA   LIB_SCRATCH+1
      CLC
      LDA   LIB_ARG0+0
      ADC   LIB_SCRATCH+0
      STA   XRAM_ADDRL
      LDA   LIB_ARG0+1
      ADC   LIB_SCRATCH+1
      STA   XRAM_ADDRM
      LDA   LIB_ARG0+2
      ADC   #0
      STA   XRAM_ADDRH
      JSR   mem_exec_read_next
      BCC   :+
      JMP   @xram_error
:
      CMP   #NOVA_EXEC_BSS_MAGIC0
      BEQ   :+
      JMP   @source_ready
:
      JSR   mem_exec_read_next
      BCC   :+
      JMP   @xram_error
:
      CMP   #NOVA_EXEC_BSS_MAGIC1
      BEQ   :+
      JMP   @source_ready
:
      JSR   mem_exec_read_next
      BCC   :+
      JMP   @xram_error
:
      CMP   #NOVA_EXEC_BSS_MAGIC2
      BEQ   :+
      JMP   @source_ready
:
      JSR   mem_exec_read_next
      BCC   :+
      JMP   @xram_error
:
      CMP   #NOVA_EXEC_BSS_MAGIC3
      BEQ   :+
      JMP   @source_ready
:
      JSR   mem_exec_read_next
      BCC   :+
      JMP   @xram_error
:
      STA   LIB_SCRATCH+3
      JSR   mem_exec_read_next
      BCC   :+
      JMP   @xram_error
:
      STA   LIB_SCRATCH+4
      JSR   mem_exec_read_next
      BCC   :+
      JMP   @xram_error
:
      EOR   #$FF
      CMP   LIB_SCRATCH+3
      BEQ   :+
      JMP   @bad_image
:
      JSR   mem_exec_read_next
      BCC   :+
      JMP   @xram_error
:
      EOR   #$FF
      CMP   LIB_SCRATCH+4
      BEQ   :+
      JMP   @bad_image
:
      LDA   LIB_SCRATCH+4
      BNE   @trailer_size_ok
      LDA   LIB_SCRATCH+3
      CMP   #NOVA_EXEC_BSS_TRAILER_SIZE+1
      BCS   @trailer_size_ok
      JMP   @bad_image
@trailer_size_ok:
      SEC
      LDA   LIB_ARG1
      SBC   #NOVA_EXEC_BSS_TRAILER_SIZE
      STA   LIB_ARG1
      LDA   LIB_ARG1+1
      SBC   #0
      STA   LIB_ARG1+1
      BCS   @source_ready
      JMP   @bad_image

@source_ready:
      JSR   mem_marshal_xaddr
      INC   XRAM_ADDRL
      BNE   :+
      INC   XRAM_ADDRM
      BNE   :+
      INC   XRAM_ADDRH
:
      INC   XRAM_ADDRL
      BNE   :+
      INC   XRAM_ADDRM
      BNE   :+
      INC   XRAM_ADDRH
:

      LDA   LIB_RESULT+1
      CMP   #>NOVA_APP_RAM_START
      BCS   :+
      JMP   @bad_range
:
      BNE   @start_ok
      LDA   LIB_RESULT+0
      CMP   #<NOVA_APP_RAM_START
      BCS   @start_ok
      JMP   @bad_range
@start_ok:
      CLC
      LDA   LIB_RESULT+0
      ADC   LIB_ARG1+0
      STA   LIB_SCRATCH+0
      LDA   LIB_RESULT+1
      ADC   LIB_ARG1+1
      STA   LIB_SCRATCH+1
      BCC   :+
      JMP   @bad_range
:
      CLC
      LDA   LIB_SCRATCH+0
      ADC   LIB_SCRATCH+3
      STA   LIB_SCRATCH+0
      LDA   LIB_SCRATCH+1
      ADC   LIB_SCRATCH+4
      STA   LIB_SCRATCH+1
      BCC   :+
      JMP   @bad_range
:
      CMP   #>NOVA_APP_RAM_END
      BCC   @copy
      BEQ   :+
      JMP   @bad_range
:
      LDA   LIB_SCRATCH+0
      BEQ   @copy
      JMP   @bad_range

@copy:
      LDA   LIB_RESULT+0
      STA   XRAM_RAML
      LDA   LIB_RESULT+1
      STA   XRAM_RAMH
      LDA   LIB_ARG1+0
      STA   XRAM_LENL
      LDA   LIB_ARG1+1
      STA   XRAM_LENH
      JSR   xram_copy_to_ram
      BEQ   :+
      JMP   @copy_error
:
      LDA   LIB_SCRATCH+3
      ORA   LIB_SCRATCH+4
      BEQ   @release
      LDA   #DMA_SPACE_CPU
      STA   DMA_DSTSPACE
      CLC
      LDA   LIB_RESULT
      ADC   LIB_ARG1
      STA   DMA_DSTL
      LDA   LIB_RESULT+1
      ADC   LIB_ARG1+1
      STA   DMA_DSTM
      STZ   DMA_DSTH
      LDA   LIB_SCRATCH+3
      STA   DMA_LENL
      LDA   LIB_SCRATCH+4
      STA   DMA_LENM
      STZ   DMA_LENH
      STZ   DMA_FILLVALUE
      JSR   dma_fill
      BEQ   @release
      JMP   @copy_error
@release:
      JSR   mem_exec_release

      ; LDA LIB_HOME_BANK / STA REG_ROMSWAP / JMP (LIB_RESULT)
      LDA   #$AD
      STA   mem_exec_trampoline+0
      LDA   #<LIB_HOME_BANK
      STA   mem_exec_trampoline+1
      LDA   #>LIB_HOME_BANK
      STA   mem_exec_trampoline+2
      LDA   #$8D
      STA   mem_exec_trampoline+3
      LDA   #<REG_ROMSWAP
      STA   mem_exec_trampoline+4
      LDA   #>REG_ROMSWAP
      STA   mem_exec_trampoline+5
      LDA   #$6C
      STA   mem_exec_trampoline+6
      LDA   #<LIB_RESULT
      STA   mem_exec_trampoline+7
      LDA   #>LIB_RESULT
      STA   mem_exec_trampoline+8

      LDA   #LERR_OK
      STA   LIB_STATUS
      SEC
      LDA   LIB_ARG2+0
      SBC   #1
      STA   LIB_SCRATCH+0
      LDA   LIB_ARG2+1
      SBC   #0
      STA   LIB_SCRATCH+1
      TSX
      LDA   LIB_SCRATCH+0
      STA   $0104,X
      LDA   LIB_SCRATCH+1
      STA   $0105,X
      INX
      STX   LIB_EXEC_SP
      DEX
      LDA   $0103,X
      PHA
      PLP
      JMP   mem_exec_trampoline

@bad_image:
      LDA   #MEM_EXEC_ERR_IMAGE
      BRA   @fail
@bad_range:
      LDA   #MEM_EXEC_ERR_RANGE
      BRA   @fail
@xram_error:
      LDA   #MEM_EXEC_ERR_XRAM
@fail:
      STA   LIB_SCRATCH+2
      JSR   mem_exec_release
      STZ   LIB_RESULT+0
      LDA   LIB_SCRATCH+2
      STA   LIB_RESULT+1
      STZ   LIB_RESULT+2
      STZ   LIB_RESULT+3
      LDA   #LERR_MEM_FAIL
      STA   LIB_STATUS
      RTS

; A failed DMA may have modified application RAM, including the host's BSS.
; Consume the staging allocation and return through the cold-start vector
; without attempting to resume that state.
@copy_error:
      JSR   mem_exec_release
      SEC
      LDA   LIB_ARG2+0
      SBC   #1
      STA   LIB_SCRATCH+0
      LDA   LIB_ARG2+1
      SBC   #0
      STA   LIB_SCRATCH+1
      TSX
      LDA   LIB_SCRATCH+0
      STA   $0104,X
      LDA   LIB_SCRATCH+1
      STA   $0105,X
      RTS

mem_exec_release:
      JSR   mem_marshal_xmc_xaddr
      LDA   LIB_ARG3+0
      STA   XMC_LENL
      LDA   LIB_ARG3+1
      STA   XMC_LENH
      JSR   xmc_release
      RTS

mem_exec_read_next:
      JSR   xram_read8
      BNE   @bad
      LDA   XRAM_DATA
      PHA
      INC   XRAM_ADDRL
      BNE   :+
      INC   XRAM_ADDRM
      BNE   :+
      INC   XRAM_ADDRH
:
      PLA
      CLC
      RTS
@bad:
      SEC
      RTS

; --- $1C MEM_EXIT_IMAGE: discard application frames and return to host ---
; LIB_EXEC_SP stores launch S+1 so zero remains an unambiguous inactive marker.
; Rebuild the RAM trampoline because another paged module may have reused the
; shared module-BSS band while the application was running.
mem_exit_image:
      LDX   LIB_EXEC_SP
      BEQ   @inactive
      STZ   LIB_EXEC_SP
      DEX

      ; LDA LIB_HOME_BANK / STA REG_ROMSWAP / RTS
      LDA   #$AD
      STA   mem_exec_trampoline+0
      LDA   #<LIB_HOME_BANK
      STA   mem_exec_trampoline+1
      LDA   #>LIB_HOME_BANK
      STA   mem_exec_trampoline+2
      LDA   #$8D
      STA   mem_exec_trampoline+3
      LDA   #<REG_ROMSWAP
      STA   mem_exec_trampoline+4
      LDA   #>REG_ROMSWAP
      STA   mem_exec_trampoline+5
      LDA   #$60
      STA   mem_exec_trampoline+6

      LDA   #LERR_OK
      STA   LIB_STATUS
      TXS
      JMP   mem_exec_trampoline

@inactive:
      STZ   LIB_RESULT+0
      LDA   #MEM_EXEC_ERR_INACTIVE
      STA   LIB_RESULT+1
      STZ   LIB_RESULT+2
      STZ   LIB_RESULT+3
      LDA   #LERR_MEM_FAIL
      STA   LIB_STATUS
      RTS

; ===========================================================================
; NDK driver bodies, included AFTER the wrappers so .referenced(xram_*/xmc_*) is
; true (selective emit keeps the image small). xram.s is included first so its
; selective-emit guards see this module's xram_* references and it defines
; XRAM_IMPLEMENTATION_INCLUDED; xmc.s then re-includes xram.inc (a no-op via its
; .ifndef guard) and, seeing the implementation present, emits its own body
; calling the already-resident xram_xmc_* routines.
; ===========================================================================
      .include "xram.inc"
      .include "xmc.inc"

      .segment "VECTORS"
      .word   $C000, $C000, $C000      ; NMI, RESET, IRQ (module runs under SEI)

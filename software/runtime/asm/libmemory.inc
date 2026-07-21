; =====================================================================
;  Nova NDK — libmemory.inc
;
;  MEMORY module (MODULE_ID_MEMORY = $05) fn-id contract. Home of the XRAM
;  core runtime (xram.s) and the XMC named-block allocator + window
;  controller (xmc.s): flat 24-bit XRAM byte/DMA access, direct file
;  streaming, the page allocator, named-block stash/fetch/delete + directory,
;  bank select, and the four hardware mapping windows. Every fn is a THIN
;  lib_call wrapper that marshals the mailbox and JSRs an NDK routine.
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

; Note the prefix: ids use MEM_* (the memory-domain library). XRAM_*/XMC_* are
; reserved for the ZP pseudo-register / MMIO register blocks (xram.inc, nova.inc)
; — those are different namespaces.
;
; CALLING CONVENTION (caller side, see libabi.inc):
;   LIB_MOD_ID ($0300) = MODULE_ID_MEMORY
;   LIB_FN_ID  ($0301) = one of the MEM_* ids below
;   LIB_ARG0..3 ($0303/$0307/$030B/$030F) = 32-bit LE arg cells
;   JSR LIB_LOADER_BAND
;   -> LIB_RESULT ($0313) = 32-bit LE result (reporters only)
;   -> LIB_STATUS ($0302) = LERR_OK ($00) or an error code
;
; MAILBOX MARSHALLING (consistent across the whole module; each fn's ;@arg in
; memory.s documents exactly what it uses):
;   ARG0 = 24-bit flat XRAM address (byte0=lo, byte1=mid, byte2=hi)
;          ...OR a 16-bit CPU pointer to a name (byte0/byte1) for named XMC ops.
;   ARG1 = 16-bit CPU RAM address (byte0=lo, byte1=hi) for copy/stash/fetch ops
;          ...OR the name length (byte0) for named XMC ops.
;   ARG2 = 16-bit transfer length / byte count (byte0=lo, byte1=hi).
;   ARG3 = byte value (byte0) for write/fill ops, or a bank/window index (byte0).
; The xram_xmc_* / xmc_* fns talk to the XMC_* MMIO block; the wrapper copies the
; same mailbox cells into XMC_XAL/M/H, XMC_RAML/H, XMC_LENL/H, XMC_DATA before the
; JSR. Named XMC ops additionally copy the pointer name into the XMC_NAME MMIO
; buffer + XMC_NAMELEN via the module's mem_marshal_name helper.
;
; STATUS MAPPING: fallible NDK routines return A=0 on success / A=1 on error and
;   also leave detail in the XMC_STATUS/XMC_ERRCODE MMIO registers; the wrapper
;   maps A!=0 -> LIB_STATUS = LERR_MEM_FAIL, A==0 -> LERR_OK. Reporters publish
;   their result cells to LIB_RESULT with LIB_STATUS=OK.

.ifndef LIBMEMORY_INC
LIBMEMORY_INC = 1

.include "libabi.inc"            ; MODULE_ID_MEMORY, LIB_* mailbox cells, LERR_*

; --- Status code returned when a wrapped NDK xram_*/xmc_* routine reports failure ---
LERR_MEM_FAIL  = $8B   ; xram_*/xmc_* NDK routine returned A!=0 (XMC error / bad arg)

; =====================================================================
; Flat 24-bit XRAM byte + DMA access (xram.s, XRAM_* ZP pseudo-register ABI)
; =====================================================================
MEM_READ8       = $00  ; (xaddr:ARG0 u24)                                    -> RESULT b0 = byte at XRAM[xaddr]
MEM_WRITE8      = $01  ; (xaddr:ARG0 u24, val:ARG3 b0)                       -> write one byte to XRAM
MEM_COPY_FROM_RAM = $02 ; (xaddr:ARG0 u24, ram:ARG1 u16, len:ARG2 u16)      -> DMA CPU RAM -> XRAM
MEM_COPY_TO_RAM = $03  ; (xaddr:ARG0 u24, ram:ARG1 u16, len:ARG2 u16)       -> DMA XRAM -> CPU RAM
MEM_FILL        = $04  ; (xaddr:ARG0 u24, len:ARG2 u16, val:ARG3 b0)        -> DMA fill XRAM

; =====================================================================
; Direct file streaming (xram.s, XRAM_* ABI; name by CPU pointer)
; =====================================================================
MEM_XLOAD       = $05  ; (nameptr:ARG0 u16, namelen:ARG1 b0, xaddr:ARG2 u24, len:ARG3 u16) -> stream file -> XRAM (len 0=all)
MEM_XSAVE       = $06  ; (nameptr:ARG0 u16, namelen:ARG1 b0, xaddr:ARG2 u24, len:ARG3 u16) -> stream XRAM -> file

; =====================================================================
; XMC-register byte + DMA access (xram.s xram_xmc_*; XMC_* MMIO ABI)
; These differ from MEM_READ8.. in that they marshal into the XMC_* MMIO
; registers (the XMC command path), exercising the XMC-compatible status regs.
; =====================================================================
MEM_XMC_READ8   = $07  ; (xaddr:ARG0 u24)                                    -> RESULT b0 = byte at XRAM[xaddr]
MEM_XMC_WRITE8  = $08  ; (xaddr:ARG0 u24, val:ARG3 b0)                       -> write one byte (XMC path)
MEM_XMC_COPY_FROM_RAM = $09 ; (xaddr:ARG0 u24, ram:ARG1 u16, len:ARG2 u16)  -> DMA CPU RAM -> XRAM (XMC path)
MEM_XMC_COPY_TO_RAM = $0A ; (xaddr:ARG0 u24, ram:ARG1 u16, len:ARG2 u16)    -> DMA XRAM -> CPU RAM (XMC path)
MEM_XMC_FILL    = $0B  ; (xaddr:ARG0 u24, len:ARG2 u16, val:ARG3 b0)        -> DMA fill XRAM (XMC path)

; =====================================================================
; XMC bank select + raw byte/block ops (xmc.s; XMC_* MMIO ABI)
; =====================================================================
MEM_SELECT_BANK = $0C  ; (bank:ARG3 b0 -> X)                                 -> select current 64KB XRAM bank
MEM_PUT_BYTE    = $0D  ; (xaddr:ARG0 u24, val:ARG3 b0)                       -> put byte + usage tracking
MEM_STASH_RAW   = $0E  ; (xaddr:ARG0 u24, ram:ARG1 u16, len:ARG2 u16)       -> RAM -> XRAM + usage tracking
MEM_FETCH_RAW   = $0F  ; (xaddr:ARG0 u24, ram:ARG1 u16, len:ARG2 u16)       -> XRAM -> RAM
MEM_RELEASE     = $10  ; (xaddr:ARG0 u24, len:ARG2 u16)                      -> release an allocation range

; =====================================================================
; XMC allocator + stats (xmc.s)
; =====================================================================
MEM_RESET_USAGE = $11  ; ()                                                  -> clear all allocations + directory
MEM_ALLOC       = $12  ; (len:ARG2 u16)                                      -> RESULT = allocated XADDR (u24)

; =====================================================================
; XMC named blocks (xmc.s; name by CPU pointer -> XMC_NAME buffer)
; =====================================================================
MEM_NAMED_STASH = $13  ; (nameptr:ARG0 u16, namelen:ARG1 b0, ram:ARG1 b2.b3 u16, len:ARG2 u16) -> named RAM -> XRAM
MEM_NAMED_FETCH = $14  ; (nameptr:ARG0 u16, namelen:ARG1 b0, ram:ARG1 b2.b3 u16, len:ARG2 u16) -> named XRAM -> RAM (len 0=all)
MEM_NAMED_DELETE = $15 ; (nameptr:ARG0 u16, namelen:ARG1 b0)                 -> delete a named block

; =====================================================================
; XMC named-block directory enumeration (xmc.s)
; =====================================================================
MEM_DIR_OPEN    = $16  ; ()                  -> open dir, emit first entry into XMC_* result regs
MEM_DIR_READ    = $17  ; ()                  -> emit next dir entry into XMC_* result regs

; =====================================================================
; XMC hardware mapping windows (xmc.s)
; =====================================================================
MEM_MAP_WINDOW  = $18  ; (xaddr:ARG0 u24, win:ARG3 b0 -> X)                   -> map window at flat XRAM address
MEM_UNMAP_WINDOW = $19 ; (win:ARG3 b0 -> X)                                  -> disable a mapped window

; =====================================================================
; XMC command-status reporter (xmc.s) — runs the raw XMC_CMD register
; =====================================================================
MEM_COMMAND_STATUS = $1A ; ()             -> RESULT b0 = 0 on XMC_OK else 1 (preloaded XMC_CMD path)

; Highest implemented fn-id + 1. Gaps / ids >= MEM_FN_COUNT -> LERR_NO_FN.
MEM_FN_COUNT   = $1B

.endif

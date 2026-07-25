; =====================================================================
;  Nova NDK — libfiles.inc
;
;  FILES module (MODULE_ID_FILES = $04) fn-id contract. Home of the
;  file-I/O NDK (fio.s) and the file pager (pager.s): save/load, directory
;  listing/delete, graphics save/load, directory navigation, runtime-ROM
;  load, host RNG, byte-stream handles, and the file-slice pager. Every fn
;  is a THIN lib_call wrapper that marshals the mailbox and JSRs an NDK routine.
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

; Note the prefix: ids use FILE_* (the file-domain library). FIO_* is reserved
; for the MMIO register block in nova.inc — these are different namespaces.
;
; CALLING CONVENTION (caller side, see libabi.inc):
;   LIB_MOD_ID ($0300) = MODULE_ID_FILES
;   LIB_FN_ID  ($0301) = one of the FILE_* ids below
;   LIB_ARG0..3 ($0303/$0307/$030B/$030F) = 32-bit LE arg cells
;   JSR LIB_LOADER_BAND
;   -> LIB_RESULT ($0313) = 32-bit LE result (reporters only)
;   -> LIB_STATUS ($0302) = LERR_OK ($00) or an error code
;
; FILENAME CONVENTION: every name-taking fn passes the filename by pointer:
;   LIB_ARG0 (u16) = pointer to the filename bytes in CPU RAM
;   LIB_ARG1 byte0 = filename length, 1..63
; The wrapper copies that into FIO.NAME/FIO.NAMELEN via fio_copy_name before
; issuing the op (fio_* expect FIO.NAME pre-filled — they never deref a pointer
; themselves). Extra args (offsets, lengths, targets) ride in ARG2/ARG3.
; FILE_PAGE is the one exception: it has more inputs than four cells allow with
; namelen in its own cell, so it packs namelen into ARG0 byte2 (see its @arg).
;
; STATUS MAPPING: fallible NDK routines return A=0 on success / A=1 on error;
;   the wrapper maps A!=0 -> LIB_STATUS = LERR_FIO_FAIL, A==0 -> LERR_OK.
;   Detailed status remains in the FIO_STATUS/FIO_ERRCODE MMIO registers.
;   The fio_rng reporter publishes FIO_RNG0..3 to LIB_RESULT (LIB_STATUS=OK).

.ifndef LIBFILES_INC
LIBFILES_INC = 1

.include "libabi.inc"            ; MODULE_ID_FILES, LIB_* mailbox cells, LERR_*

; --- Status code returned when a wrapped NDK fio_*/pager_* routine reports failure ---
; Named LERR_FIO_FAIL (distinct from libgraphics.inc's graphics-domain LERR_FILE_FAIL
; for gsave/gload) so the two headers co-include with no symbol collision.
LERR_FIO_FAIL = $8A   ; fio_*/pager_* NDK routine returned A!=0 (FIO error / bad arg)

; =====================================================================
; File save / load (fio.s, FIO.NAME + FIO_SRCL/H/FIO_ENDL/H ABI)
; =====================================================================
FILE_SAVE      = $00   ; (nameptr:ARG0, namelen:ARG1, src:ARG2 u16, end:ARG3 u16) -> save RAM range
FILE_LOAD      = $01   ; (nameptr:ARG0, namelen:ARG1, dest:ARG2 u16)              -> load file to RAM

; =====================================================================
; Directory listing + delete (fio.s)
; =====================================================================
FILE_DIR_OPEN  = $02   ; (nameptr:ARG0, namelen:ARG1 optional filter)            -> open directory listing
FILE_DIR_READ  = $03   ; ()                                                      -> name/type/size + attr/time in FIO regs
FILE_DELETE    = $04   ; (nameptr:ARG0, namelen:ARG1)                            -> delete file/program

; =====================================================================
; Graphics / VGC memory save + load (fio.s, FIO graphics register ABI)
; =====================================================================
FILE_GSAVE     = $05   ; (nameptr:ARG0, namelen:ARG1, space:ARG2 b0, gaddr:ARG2 b1.b2 u16, glen:ARG3 u16) -> save VGC memory
FILE_GLOAD     = $06   ; (nameptr:ARG0, namelen:ARG1, space:ARG2 b0, gaddr:ARG2 b1.b2 u16, glen:ARG3 u16) -> load VGC memory

; =====================================================================
; Directory navigation (fio.s)
; =====================================================================
FILE_CD        = $07   ; (nameptr:ARG0, namelen:ARG1)                            -> change directory
FILE_MKDIR     = $08   ; (nameptr:ARG0, namelen:ARG1)                            -> create directory
FILE_RMDIR     = $09   ; (nameptr:ARG0, namelen:ARG1)                            -> remove directory
FILE_PWD       = $0A   ; ()                                                      -> read CWD into FIO.NAME/FIO.NAMELEN

; =====================================================================
; Runtime ROM load + compat exec alias (fio.s)
; =====================================================================
FILE_LOAD_RUNTIME = $0B ; (nameptr:ARG0, namelen:ARG1)                           -> stream 16K image into primary ROM bank
FILE_RUN          = $0C ; (cmd:ARG0 b0 FIO command)                              -> issue/wait/check a raw FIO command (fio_run alias)

; =====================================================================
; Host RNG reporter (fio.s; writes LIB_RESULT bytes 0..3)
; =====================================================================
FILE_RNG       = $0D   ; ()              -> RESULT = 32 host-backed random bits (FIO_RNG0..3)

; =====================================================================
; File pager (pager.s) — stream a file slice into a chosen target
; =====================================================================
FILE_PAGE      = $0E   ; (nameptr:ARG0 b0.b1, namelen:ARG0 b2,
                       ;  offset:ARG1 u24 b0.b1.b2, target:ARG1 b3,
                       ;  destaddr:ARG2 u24 b0.b1.b2, len:ARG3 u16 b0.b1)
                       ;  -> RESULT=actual bytes streamed; zero at exact EOF

; =====================================================================
; Low-level byte-stream file handles (fio.s)
; =====================================================================
FILE_FOPEN     = $0F   ; (nameptr:ARG0, namelen:ARG1, fam:ARG2 b0)              -> RESULT=fileid
FILE_FCREATE   = $10   ; (nameptr:ARG0, namelen:ARG1, fam:ARG2 b0)              -> RESULT=fileid
FILE_FCLOSE    = $11   ; (fileid:ARG0 b0)                                       -> close/flush
FILE_FREAD     = $12   ; (fileid:ARG0 b0, dst:ARG1 u16, len:ARG2 u16)           -> RESULT=bytes read
FILE_FWRITE    = $13   ; (fileid:ARG0 b0, src:ARG1 u16, len:ARG2 u16)           -> RESULT=bytes written
FILE_FSEEK     = $14   ; (fileid:ARG0 b0, pos:ARG1 u24)                         -> reposition
FILE_FTELL     = $15   ; (fileid:ARG0 b0)                                       -> RESULT=position low24
FILE_FSIZE     = $16   ; (fileid:ARG0 b0)                                       -> RESULT=size low24
FILE_FRESIZE   = $17   ; (fileid:ARG0 b0, size:ARG1 u24)                        -> resize
FILE_FFLUSH    = $18   ; (fileid:ARG0 b0)                                       -> flush
FILE_FSTATUS   = $19   ; (nameptr:ARG0, namelen:ARG1)                           -> RESULT=implementation fam
FILE_FDELETE   = $1A   ; (nameptr:ARG0, namelen:ARG1)                           -> exact delete
FILE_FRENAME   = $1B   ; (oldptr:ARG0, oldlen:ARG1, newptr:ARG2, newlen:ARG3)   -> exact rename
FILE_DIR_LIST  = $1C   ; (nameptr:ARG0, namelen:ARG1 optional filter)            -> open+iterate+print whole listing (bank-0 handler stays thin)
FILE_DISK_FREE = $1D   ; (nameptr:ARG0, namelen:ARG1 device prefix)              -> RESULT=free bytes u32
FILE_DISK_SIZE = $1E   ; (nameptr:ARG0, namelen:ARG1 device prefix)              -> RESULT=capacity bytes u32
FILE_INFO_GET  = $1F   ; (nameptr:ARG0, namelen:ARG1)                            -> RESULT=attributes; FIO_FILE_* outputs
FILE_INFO_SET  = $20   ; (nameptr:ARG0, namelen:ARG1, attr:ARG2 b0, time:ARG3)   -> set attributes/timestamp
FILE_CLOCK_GET = $21   ; ()                                                       -> FIO_CLOCK_* outputs including DOW
FILE_CLOCK_SET = $22   ; (year/month/day:ARG0, hour/min/sec/hsec:ARG1)            -> set clock
FILE_HASH      = $23   ; (nameptr:ARG0, namelen:ARG1)                             -> RESULT=CRC-32 of exact file bytes

; Highest implemented fn-id + 1. Gaps / ids >= FILE_FN_COUNT -> LERR_NO_FN.
FILE_FN_COUNT  = $24

.endif

; libcall_driver_stub.s — Task T10 lib_call hardware-smoke driver.
;
; A tiny RAM stub (loaded + run at $9F80, below the BASIC string/end-of-mem cap
; the smoke script carves out) that drives the REAL paged-library loader on
; physical silicon. It:
;   1. fills the canonical mailbox FRESH (so the proof does not depend on any
;      host poke of $0300+ surviving until the CPU runs the stub),
;   2. JSRs lib_call at $9C00 (the actual loader binary, POKEd to RAM by the
;      smoke script) which pages the 16 KB TEST module from XRAM into bank-1
;      ext_rom, validates its header, flips ROMSWAP, and dispatches FN 0 (ECHO),
;   3. stashes the outcome (RESULT/STATUS/RESIDENT) into PROTECTED high RAM at
;      $9FE0.. so no later low-memory activity (BASIC line entry, GC, etc.) can
;      clobber the bytes the host reads back via peek_block.
;
; ECHO copies LIB_ARG0 -> LIB_RESULT, so with ARG0 = $DEADBEEF a correct load
; yields RESULT=$DEADBEEF, STATUS=$00 (LERR_OK), RESIDENT=$7F (MODULE_ID_TEST).
;
; Called via the BASIC USR vector (smoke script points JMP $0A at $9F80, then
; runs "?USR(0)"). Runs from RAM because lib_call maps bank 1 over $C000-$FFFF
; mid-call; the stub's own fetch + return address must stay below $A000.
;
; Assemble (mirrors tools/page_in_checksum_stub.{s,cfg}):
;   ca65 --cpu 65c02 tools/libcall_driver_stub.s -o /tmp/libdrv.o
;   ld65 -C tools/libcall_driver_stub.cfg /tmp/libdrv.o -o tools/libcall_driver_stub.bin
;
; The 6502-side flow is unit-proven (MSTest) and Verilator-proven (23/23 on the
; same RTL); this stub is the on-hardware driver. No RTL/firmware/flash change.

.segment "CODE"

; --- Canonical mailbox (matches runtime/asm/libabi.inc) ---
LIB_MOD_ID    = $0300
LIB_FN_ID     = $0301
LIB_STATUS    = $0302
LIB_ARG0      = $0303          ; 32-bit LE
LIB_RESULT    = $0313          ; 32-bit LE
LIB_HOME_BANK = $0317
LIB_RESIDENT  = $0318

; --- Test fixtures ---
MODULE_ID_TEST = $7F
FN_ECHO        = $00
ROMSWAP_BASIC  = $02           ; runtime's home ROMSWAP value (set at boot)

LIB_CALL       = $9C00         ; real loader entry (POKEd here by smoke script)

; --- Protected high-RAM stash (host reads these back) ---
;   $9FE0..$9FE3  RESULT (4 bytes, LE)
;   $9FE4         STATUS
;   $9FE5         RESIDENT
STASH          = $9FE0

stub:
        ; --- mailbox setup: FRESH, independent of host pokes ---
        lda     #MODULE_ID_TEST
        sta     LIB_MOD_ID
        lda     #FN_ECHO
        sta     LIB_FN_ID
        lda     #$EF                    ; ARG0 = $DEADBEEF (little-endian)
        sta     LIB_ARG0
        lda     #$BE
        sta     LIB_ARG0+1
        lda     #$AD
        sta     LIB_ARG0+2
        lda     #$DE
        sta     LIB_ARG0+3
        lda     #ROMSWAP_BASIC
        sta     LIB_HOME_BANK
        lda     #$00
        sta     LIB_RESIDENT            ; none resident → force a real page-in

        ; --- drive the real loader ---
        jsr     LIB_CALL                ; page-in + validate + ROMSWAP + ECHO

        ; --- stash outcome to protected high RAM ---
        ldx     #$03
@cp:    lda     LIB_RESULT,x
        sta     STASH,x                 ; RESULT[0..3]
        dex
        bpl     @cp
        lda     LIB_STATUS
        sta     STASH+4
        lda     LIB_RESIDENT
        sta     STASH+5                 ; should be $7F after a successful load
        rts

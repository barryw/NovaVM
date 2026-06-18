; graphics_smoke_driver.s — Phase 4b GRAPHICS-module hardware smoke driver.
;
; A tiny RAM stub (loaded + run at $9F80, below the BASIC string/end-of-mem cap
; the smoke script carves out) that drives the REAL paged-library loader on
; physical silicon to load + dispatch the GRAPHICS module. It:
;   1. fills the canonical mailbox FRESH (MOD_ID=$01 GRAPHICS, FN=$00 GCLS),
;   2. JSRs lib_call at $9C00 (the actual loader binary, POKEd to RAM by the
;      smoke script — its modtab now resolves MODULE_ID_GRAPHICS -> shelf slot 0)
;      which pages the 16 KB GRAPHICS module from XRAM into bank-1 ext_rom,
;      validates its "NL"/$01 header, flips ROMSWAP, and dispatches FN $00
;      (gfn_gcls: issues VCMD_GCLS, waits, sets STATUS=LERR_OK),
;   3. stashes the outcome (STATUS/RESIDENT) into PROTECTED high RAM at $9FE0..
;
; A correct load yields STATUS=$00 (LERR_OK) and RESIDENT=$01 (MODULE_ID_GRAPHICS),
; proving the GRAPHICS module pages in + dispatches a real VGC op on silicon.
; (RESULT is don't-care: GCLS is a void op.)
;
; GCLS uses VGC_P/VGC_CMD MMIO + LIB_SCRATCH only — it does NOT touch the module
; BSS band ($0320+), so this smoke is safe under a live BASIC program at Ram_base
; without the (deferred) cross-runtime band reservation.
;
; Assemble:
;   ca65 --cpu w65c02 tools/graphics_smoke_driver.s -o /tmp/gfxdrv.o
;   ld65 -C tools/libcall_driver_stub.cfg /tmp/gfxdrv.o -o tools/graphics_smoke_driver.bin

.segment "CODE"

; --- Canonical mailbox (matches runtime/asm/libabi.inc) ---
LIB_MOD_ID    = $0300
LIB_FN_ID     = $0301
LIB_STATUS    = $0302
LIB_RESULT    = $0313          ; 32-bit LE (unused by GCLS)
LIB_HOME_BANK = $0317
LIB_RESIDENT  = $0318

MODULE_ID_GRAPHICS = $01
GFN_GCLS           = $00
ROMSWAP_BASIC      = $02       ; runtime's home ROMSWAP value (set at boot)

LIB_CALL       = $9C00         ; real loader entry (POKEd here by the smoke script)

; --- Protected high-RAM stash (host reads these back) ---
;   $9FE0..$9FE3  RESULT (4 bytes, LE) — don't-care for GCLS
;   $9FE4         STATUS
;   $9FE5         RESIDENT
STASH          = $9FE0

stub:
        ; --- mailbox setup: FRESH, independent of host pokes ---
        lda     #MODULE_ID_GRAPHICS
        sta     LIB_MOD_ID
        lda     #GFN_GCLS
        sta     LIB_FN_ID
        lda     #ROMSWAP_BASIC
        sta     LIB_HOME_BANK
        lda     #$00
        sta     LIB_RESIDENT            ; none resident → force a real page-in

        ; --- drive the real loader ---
        jsr     LIB_CALL                ; page-in + validate + ROMSWAP + dispatch GCLS

        ; --- stash outcome to protected high RAM ---
        ldx     #$03
@cp:    lda     LIB_RESULT,x
        sta     STASH,x                 ; RESULT[0..3] (don't-care for GCLS)
        dex
        bpl     @cp
        lda     LIB_STATUS
        sta     STASH+4                 ; expect $00 (LERR_OK)
        lda     LIB_RESIDENT
        sta     STASH+5                 ; expect $01 (MODULE_ID_GRAPHICS)
        rts

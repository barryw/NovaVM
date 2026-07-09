; =====================================================================
;  Nova NDK — libcall.s
;
;  Resident paged-library loader. Caller fills LIB_MOD_ID/LIB_FN_ID/args
;  then JSRs lib_call (LIB_HOME_BANK = caller's REG_ROMSWAP). On return
;  LIB_STATUS=0 on success, LIB_RESULT set by module; A/X/Y clobbered.
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

      .include "libabi.inc"
      .include "nova.inc"                 ; REG_ROMSWAP, ROMSWAP_EXTENSION

      ; stz-based PGD programming below assumes these low bytes are $00.
      .assert SHELF_BASE_L = $00, error, "SHELF_BASE_L must be $00 for stz PGD_SRCL"
      .assert (<SHELF_SLOT_WORDS) = $00, error, "SHELF_SLOT_WORDS low byte must be $00 for stz PGD_WORDSL"

      .segment "CODE"
lib_call:
      lda     LIB_MOD_ID
      beq     lc_bad_module               ; id $00 is never a module; reject before the
                                          ; RESIDENT short-circuit ($00 also = "none").
      cmp     LIB_RESIDENT
      beq     lc_invoke                   ; HIT → dispatch, no page-in
      jsr     modtab_lookup               ; A=id -> PGD_SRC/WORDS; C=1 unknown
      bcs     lc_bad_module
      lda     #PGD_START
      sta     PGD_CMD                     ; page-in; CPU STALLS until ext_rom loaded
      jsr     lc_validate                 ; C=1 on bad header (LIB_STATUS set)
      bcs     lc_done
      ; fall through to invoke
lc_invoke:
      php
      sei                                 ; vectors come from module while bank1 mapped
      lda     #ROMSWAP_EXTENSION
      sta     REG_ROMSWAP
      jsr     MOD_ENTRY                   ; JSR $C000 — module dispatches on LIB_FN_ID
      lda     LIB_HOME_BANK
      sta     REG_ROMSWAP
      plp
lc_done:
      rts
lc_bad_module:
      lda     #LERR_BAD_MODULE
      sta     LIB_STATUS                  ; EXT never mapped — no bank restore needed
      rts

; lc_validate — bank1 mapped, check header; cache LIB_RESIDENT or set error.
; C=0: header ok, HOME restored, LIB_RESIDENT cached.
; C=1: bad header, HOME restored, LIB_STATUS set.
lc_validate:
      php
      sei
      lda     #ROMSWAP_EXTENSION
      sta     REG_ROMSWAP
      lda     MOD_MAGIC
      cmp     #MOD_MAGIC0
      bne     lcv_magic
      lda     MOD_MAGIC+1
      cmp     #MOD_MAGIC1
      bne     lcv_magic
      lda     MOD_MOD_ID
      cmp     LIB_MOD_ID
      bne     lcv_mod
      lda     MOD_VERSION
      cmp     #LIB_ABI_VERSION
      bne     lcv_ver
      lda     LIB_MOD_ID
      sta     LIB_RESIDENT
      lda     LIB_HOME_BANK
      sta     REG_ROMSWAP
      plp
      clc
      rts
; BIT-abs ($2C) skip chain: relies on each LERR_* being a 2-byte `LDA #imm`
; ($80/$81/$82 sequential), so $2C swallows the following LDA and falls to sta.
lcv_magic: lda #LERR_BAD_MAGIC
      .byte $2C                            ; BIT abs — skip next LDA #imm
lcv_mod:   lda #LERR_BAD_MODULE
      .byte $2C
lcv_ver:   lda #LERR_BAD_VER
      sta     LIB_STATUS
      lda     LIB_HOME_BANK
      sta     REG_ROMSWAP
      plp
      sec
      rts

; modtab_lookup — A = module id. Scan shelf_tag[] for a resident slot.
; HIT : program PGD_SRC = SHELF_BASE + slot*$4000, PGD_WORDS = SHELF_SLOT_WORDS,
;       bump the slot to MRU in shelf_lru[], return C=0.
; MISS: demand-load from SD via the FIO controller into a victim slot
;       (empty-first, else LRU back), record the tag, then page in (C=0). On host
;       load error the victim tag is cleared and we return C=1 (-> LERR_BAD_MODULE).
; The compile-time map is gone: slot is assigned by the host (firmware/test harness),
; which seeds shelf_tag[]/shelf_lru[]. See docs/plans/2026-06-05-dynamic-module-shelf-design.md.
; id $00 ("none") can never reach here: the sole caller (lib_call) rejects it
; before the JSR, so no explicit MODULE_ID_NONE guard is needed.
modtab_lookup:
      ldx     #SHELF_N-1               ; match search is order-independent: count down
mt_scan:
      cmp     SHELF_TAG,x
      beq     mt_hit
      dex
      bpl     mt_scan
; MISS — demand-load. A = wanted id. Pick a victim slot (empty-first, else LRU
; back), ask the host to stream it from SD into that slot via the FIO controller,
; poll for completion, record the tag, then fall into the shared page-in tail.
; On host error: leave the victim tag $00 (untrusted) and return C=1.
mt_miss:
      ldx     #0                       ; --- find an empty slot ---
mv_scan:
      ldy     SHELF_TAG,x
      beq     mv_have                  ; tag==0 -> empty slot X
      inx
      cpx     #SHELF_N
      bne     mv_scan
      ldx     SHELF_LRU+SHELF_N-1      ; none empty -> evict LRU back (a slot index)
mv_have:
      ; X = victim slot, held in X across the whole FIO call (nothing below
      ; touches X). Stash the id in LIB_SCRATCH so it survives the FIO writes.
      sta     LIB_SCRATCH              ; id
      sta     FIO_SRC_LO               ; param: module id
      stx     FIO_END_LO               ; param: dest slot
      stz     FIO_STATUS               ; clear before trigger (no stale read)
      lda     #FIO_CMD_LOAD_MODULE
      sta     FIO_CMD                  ; fire; host streams SD->slot (CPU stalls)
ml_poll:
      lda     FIO_STATUS
      beq     ml_poll                  ; 0 = busy
      ; STATUS is now OK ($02) or ERR ($03). lsr: OK->C=0, ERR (or any odd)->C=1.
      lsr     a
      bcc     ml_ok
      stz     SHELF_TAG,x              ; host error: leave victim empty/untrusted
      sec
      rts
ml_ok:
      lda     LIB_SCRATCH              ; success: record id in the victim slot tag
      sta     SHELF_TAG,x              ; X still = victim slot; fall into page-in tail

mt_hit:
      ; X = slot index (from the scan). Fall into the shared page-in tail.
mt_program_pgd:
      stx     LIB_SCRATCH              ; shelf_touch reads the slot from LIB_SCRATCH
      stz     PGD_SRCL                 ; SHELF_BASE_L = $00
      txa                              ; slot index -> mid byte = slot*$40 (slot<<6)
      asl
      asl
      asl
      asl
      asl
      asl
      sta     PGD_SRCM
      lda     #SHELF_BASE_H
      sta     PGD_SRCH
      stz     PGD_WORDSL               ; <SHELF_SLOT_WORDS = $00
      lda     #>SHELF_SLOT_WORDS
      sta     PGD_WORDSH
      jsr     shelf_touch
      clc
      rts

; shelf_touch — make slot (in LIB_SCRATCH) the MRU entry of shelf_lru[].
; Finds it, shifts the preceding entries up one index, writes it at [0].
shelf_touch:
      ldx     #SHELF_N-1               ; locating the slot is order-independent: count down
st_find:
      lda     SHELF_LRU,x
      cmp     LIB_SCRATCH
      beq     st_found
      dex
      bpl     st_find
      rts                              ; not present (shouldn't happen) — leave as-is
st_found:
      txa
      beq     st_done                  ; already MRU (position 0)
st_shift:
      lda     SHELF_LRU-1,x
      sta     SHELF_LRU,x
      dex
      bne     st_shift
      lda     LIB_SCRATCH
      sta     SHELF_LRU                ; [0] = slot
st_done:
      rts

; The resident loader shares its band with the shelf directory: in the canonical
; $0320 image (libcall_resident.cfg) the code must end below SHELF_TAG ($0418) or a
; POKE of this image would clobber shelf_tag[]/shelf_lru[]. This is currently EXACTLY
; full (248 bytes; resident code ends at $0417). The check is on byte length, not
; absolute PC, so it holds for the $9C00 harness blob (tests/asm/libcall_blob.cfg)
; too. If you need more loader bytes, relocate the 8-byte directory (SHELF_TAG/
; SHELF_LRU) into the module-BSS band ($0420-$08FF) and free the full $0320-$041F.
.assert (* - lib_call) <= (SHELF_TAG - LIB_LOADER_BAND), error, "libcall loader overruns the shelf directory at $0418"

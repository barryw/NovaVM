; libcall.s — resident paged-library loader. ORG via cfg (RAM blob for tests).
; Caller fills LIB_MOD_ID/LIB_FN_ID/args, then JSR lib_call.
; LIB_HOME_BANK must hold the caller's REG_ROMSWAP value (set at boot).
; On return: LIB_STATUS (0=ok), LIB_RESULT set by module. A/X/Y clobbered.
      .include "libabi.inc"
      .include "nova.inc"                 ; REG_ROMSWAP, ROMSWAP_EXTENSION

      .segment "CODE"
lib_call:
      lda     LIB_MOD_ID
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
; MISS: return C=1 (caller -> LERR_BAD_MODULE). Phase B adds the SD load handshake.
; The compile-time map is gone: slot is assigned by the host (firmware/test harness),
; which seeds shelf_tag[]/shelf_lru[]. See docs/plans/2026-06-05-dynamic-module-shelf-design.md.
modtab_lookup:
      ldx     #0
mt_scan:
      cmp     SHELF_TAG,x
      beq     mt_hit
      inx
      cpx     #SHELF_N
      bne     mt_scan
      sec                              ; not resident
      rts
mt_hit:
      stx     LIB_SCRATCH              ; save slot index for the LRU touch
      lda     #SHELF_BASE_L
      sta     PGD_SRCL
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
      lda     #<SHELF_SLOT_WORDS
      sta     PGD_WORDSL
      lda     #>SHELF_SLOT_WORDS
      sta     PGD_WORDSH
      jsr     shelf_touch
      clc
      rts

; shelf_touch — make slot (in LIB_SCRATCH) the MRU entry of shelf_lru[].
; Finds it, shifts the preceding entries down one, writes it at [0].
shelf_touch:
      ldx     #0
st_find:
      lda     SHELF_LRU,x
      cmp     LIB_SCRATCH
      beq     st_found
      inx
      cpx     #SHELF_N
      bne     st_find
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

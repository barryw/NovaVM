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

; modtab_lookup — A = module id in; program PGD_SRC*/PGD_WORDS*; C=1 if unknown.
; 3b: single TEST module at shelf slot 0. Phase 1/B replaces with directory lookup.
;
; XRAM library shelf (option A, fixed): SHELF_BASE=$060000, slot i = +i*$4000 (16KB).
; The compile-time map below IS the contract — NovaHost's boot manifest MUST stage each
; module to the same slot base (single source of truth). See docs/help/guides/library-shelf.md.
;   id  $7F TEST     -> slot 0  $060000   (3b proof; real modules will reclaim slot 0)
;   id  $01 GRAPHICS -> slot 0  $060000   (Phase 4)   ] add a cmp/bne row per module,
;   id  $02 SOUND    -> slot 1  $064000   (Phase 5)   ] or a table walk, when these land.
;   id  $03 SYSTEM   -> slot 2  $068000   (Phase 5)   ]
; Phase 1/System-B replaces this with an XRAM-directory lookup by name (modtab filled at
; boot); lib_call is unchanged — modtab is the abstraction that hides constants-vs-directory.
modtab_lookup:
      cmp     #MODULE_ID_TEST          ; $7F TEST     -> slot 0 $060000
      beq     mt_slot0
      cmp     #MODULE_ID_GRAPHICS      ; $01 GRAPHICS -> slot 0 $060000 (shares the slot)
      beq     mt_slot0
      bne     mt_unknown
mt_slot0:
      lda     #SHELF_BASE_L
      sta     PGD_SRCL
      lda     #SHELF_BASE_M
      sta     PGD_SRCM
      lda     #SHELF_BASE_H
      sta     PGD_SRCH
      lda     #<SHELF_SLOT_WORDS
      sta     PGD_WORDSL
      lda     #>SHELF_SLOT_WORDS
      sta     PGD_WORDSH
      clc
      rts
mt_unknown:
      sec
      rts

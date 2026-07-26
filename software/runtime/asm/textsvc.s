; =====================================================================
;  Nova NDK — textsvc.s
;
;  Shared XRAM-backed text-editing services: a language-neutral clipboard and
;  undo/redo stack, so every editor gets the same behaviour. Intentionally
;  does not link the general XMC allocator — the editor is a 16K paged library,
;  so it uses fixed NDK-owned XRAM reservations documented in textsvc.inc.
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

.include "textsvc.inc"

.ifndef TEXTSVC_IMPLEMENTATION_INCLUDED
TEXTSVC_IMPLEMENTATION_INCLUDED = 1

; Undo/redo records are deltas: each stores only the bytes its edit removed, so
; history depth does not scale with document size.
;
;   0,1   prev_top     back-link, $FFFF at the bottom of the stack
;   2,3   target_len   buffer length after this record is restored
;   4,5   target_cur   cursor to restore
;   6,7   prefix       splice point: leading bytes shared with the live buffer
;   8,9   new_span     bytes to cut out of the live buffer at prefix
;   10,11 old_span     payload length: bytes to insert at prefix
;   12,13 reserved     keeps the header even-sized
TEXTSVC_STACK_HEADER_BYTES = 14
TEXTSVC_STACK_EMPTY_L      = $FF
TEXTSVC_STACK_EMPTY_H      = $FF
TEXTSVC_WIN3_ENABLE       = $08
ts_raml                  = NVR6L
ts_ramh                  = NVR6H
; Every 16-bit field lives in one array so a single set of primitives
; (ts_mov16/ts_add16/ts_sub16/ts_cmp16/ts_dec16) serves all of them; the
; record header is the tail of the same array so its fields index too.
; This is what keeps the delta code inside the editor module's 16K.
TSV_SAVE    = 0                  ; live buffer pointer
TSV_BUFLEN  = 2                  ; live buffer length
TSV_RECORD  = 4                  ; size of the record being pushed
TSV_END     = 6                  ; arena offset just past it
TSV_OFF     = 8                  ; arena offset fed to ts_xaddr_from_active_off
TSV_WORK    = 10                 ; copy length
TSV_USED    = 12                 ; active stack: first free arena offset
TSV_TOP     = 14                 ; active stack: newest record offset
TSV_VALID   = 16                 ; active stack: nonzero when it holds a record
TSV_TMP     = 18
TSV_PSRC    = 20                 ; RAM payload source
TSV_SPL_OFF = 22                 ; published splice: where it applies
TSV_PEND    = 24                 ; end of the touched region, original coordinates
TSV_SHIFT   = 26                 ; bytes this group has added so far, signed
TSV_SPAN    = 28                 ; caller mailbox: offset of one mutation
TSV_CNT     = 30                 ; caller mailbox: its byte count
TSV_SIZE    = 32
TSV_HDR_PREV   = TSV_SIZE + 0
TSV_HDR_LEN    = TSV_SIZE + 2
TSV_HDR_CUR    = TSV_SIZE + 4
TSV_HDR_PREFIX = TSV_SIZE + 6
TSV_HDR_NEW    = TSV_SIZE + 8
TSV_HDR_OLD    = TSV_SIZE + 10
; Published splice, indexed like everything else so it lands past the header.
TSV_SPL_CUT = TSV_SIZE + TEXTSVC_STACK_HEADER_BYTES + 0
TSV_SPL_INS = TSV_SIZE + TEXTSVC_STACK_HEADER_BYTES + 2

; The pre-image and the span tracker write straight into the header fields they
; end up occupying, so committing a record copies almost nothing. All of it is
; dead once a record is pushed, and capture refills it before the next commit.
TSV_PRELEN  = TSV_HDR_LEN        ; pre-image length   -> record target_len
TSV_PRECUR  = TSV_HDR_CUR        ; pre-image cursor   -> record target_cursor
TSV_PMIN    = TSV_HDR_PREFIX     ; first touched byte -> splice point
TSV_NS      = TSV_HDR_NEW        ; bytes the group added
TSV_OS      = TSV_HDR_OLD        ; bytes the group removed = payload length
TSV_UNTOUCHED = $FF              ; both bytes of PMIN while nothing has changed

; Shadow copies of the two stacks, one 5-byte block each (used, top, valid) in
; the same order as v[TSV_USED..TSV_VALID].
TSS_BLOCK   = 5
TSS_UNDO    = 0
TSS_REDO    = TSS_BLOCK

; 24-bit XRAM cursors, indexed the same way.
TSA_X       = 0                  ; the cursor every copy helper walks
TSA_SRC     = 3
TSA_DST     = 6
TSA_SIZE    = 9

      .segment "BSS"

TEXTSVC_CLIPLENL:   .res 1
TEXTSVC_CLIPLENH:   .res 1
TEXTSVC_CURL:       .res 1
TEXTSVC_CURH:       .res 1
TEXTSVC_REST_LENL:  .res 1
TEXTSVC_REST_LENH:  .res 1
TEXTSVC_REST_CURL:  .res 1
TEXTSVC_REST_CURH:  .res 1

ts_shadow:          .res 2*TSS_BLOCK

ts_base_xm:         .res 1
ts_base_xh:         .res 1
ts_sel:             .res 1       ; which stack an apply is reading from
ts_pre_valid:       .res 1       ; a pre-image is waiting for its commit
ts_from_xram:       .res 1       ; push payload source: 1 = XRAM, 0 = RAM

ts_v:               .res TSV_SPL_OFF
TEXTSVC_SPLICE_OFFL:.res 1       ; the mailboxes live inside ts_v so the 16-bit
TEXTSVC_SPLICE_OFFH:.res 1       ; primitives can index them like everything else
                    .res TSV_SPAN - TSV_SPL_OFF - 2
TEXTSVC_SPANL:      .res 1
TEXTSVC_SPANH:      .res 1
TEXTSVC_CNTL:       .res 1
TEXTSVC_CNTH:       .res 1
ts_header:          .res TEXTSVC_STACK_HEADER_BYTES
TEXTSVC_SPLICE_CUTL:.res 1       ; bytes of live text the popped record replaces
TEXTSVC_SPLICE_CUTH:.res 1
TEXTSVC_SPLICE_INSL:.res 1       ; bytes it puts there instead
TEXTSVC_SPLICE_INSH:.res 1
ts_a:               .res TSA_SIZE
ts_note_del:        .res 1

ts_savel            = ts_v+TSV_SAVE
ts_saveh            = ts_v+TSV_SAVE+1
ts_buflenl          = ts_v+TSV_BUFLEN
ts_buflenh          = ts_v+TSV_BUFLEN+1
ts_offl             = ts_v+TSV_OFF
ts_offh             = ts_v+TSV_OFF+1
ts_worklenl         = ts_v+TSV_WORK
ts_worklenh         = ts_v+TSV_WORK+1
ts_usedl            = ts_v+TSV_USED
ts_usedh            = ts_v+TSV_USED+1
ts_topl             = ts_v+TSV_TOP
ts_toph             = ts_v+TSV_TOP+1
ts_valid            = ts_v+TSV_VALID
ts_xl               = ts_a+TSA_X
ts_xm               = ts_a+TSA_X+1
ts_xh               = ts_a+TSA_X+2

      .segment "CODE"

      .export textsvc_init
      .export textsvc_clip_release
      .export textsvc_clip_copy_from_ram
      .export textsvc_clip_fetch_to_ram
      .export textsvc_undo_capture
      .export textsvc_undo_commit
      .export textsvc_undo_reset
      .export textsvc_undo_inserted
      .export textsvc_undo_deleted
      .export textsvc_undo_paste
      .export textsvc_undo_apply
      .export textsvc_redo_apply

textsvc_init:
      JSR   textsvc_clip_release
      JSR   textsvc_undo_reset
      LDA   #0
      RTS

; ---------------------------------------------------------------------
; 16-bit primitives over ts_v. X indexes the destination, Y the source.
; ---------------------------------------------------------------------

ts_mov16:
      LDA   ts_v+0,Y
      STA   ts_v+0,X
      LDA   ts_v+1,Y
      STA   ts_v+1,X
      RTS

ts_add16:
      CLC
      LDA   ts_v+0,X
      ADC   ts_v+0,Y
      STA   ts_v+0,X
      LDA   ts_v+1,X
      ADC   ts_v+1,Y
      STA   ts_v+1,X
      RTS

ts_sub16:
      SEC
      LDA   ts_v+0,X
      SBC   ts_v+0,Y
      STA   ts_v+0,X
      LDA   ts_v+1,X
      SBC   ts_v+1,Y
      STA   ts_v+1,X
      RTS

; Carry set when v[X] >= v[Y]; zero set when they are equal.
ts_cmp16:
      LDA   ts_v+1,X
      CMP   ts_v+1,Y
      BNE   :+
      LDA   ts_v+0,X
      CMP   ts_v+0,Y
:     RTS

ts_dec16:
      LDA   ts_v+0,X
      BNE   :+
      DEC   ts_v+1,X
:     DEC   ts_v+0,X
      RTS

; Zero flag set when v[X] is zero.
ts_test16:
      LDA   ts_v+0,X
      ORA   ts_v+1,X
      RTS

; RAM pointer <- v[X]
ts_set_ram:
      LDA   ts_v+0,X
      STA   ts_raml
      LDA   ts_v+1,X
      STA   ts_ramh
      RTS

; a[X] <- a[Y]
ts_mov24:
      LDA   ts_a+0,Y
      STA   ts_a+0,X
      LDA   ts_a+1,Y
      STA   ts_a+1,X
      LDA   ts_a+2,Y
      STA   ts_a+2,X
      RTS

; ---------------------------------------------------------------------
; Clipboard
; ---------------------------------------------------------------------

textsvc_clip_release:
      STZ   TEXTSVC_CLIPLENL
      STZ   TEXTSVC_CLIPLENH
      LDA   #0
      RTS

; Copy RAM[XMC_RAML/H, XMC_LENL/H] into the shared XRAM clipboard.
textsvc_clip_copy_from_ram:
      JSR   ts_save_current_args
      LDX   #TSV_BUFLEN
      JSR   ts_test16
      BNE   :+
      JMP   textsvc_clip_release
:     JSR   ts_clip_len_fits
      BCC   :+
      LDA   #1
      RTS
:     LDA   #TEXTSVC_CLIP_XRAM_L
      STA   ts_xl
      LDA   #TEXTSVC_CLIP_XRAM_M
      STA   ts_xm
      LDA   #TEXTSVC_CLIP_XRAM_H
      STA   ts_xh
      LDX   #TSV_SAVE
      JSR   ts_set_ram
      LDX   #TSV_WORK
      LDY   #TSV_BUFLEN
      JSR   ts_mov16
      JSR   ts_copy_ram_to_xram
      LDA   ts_buflenl
      STA   TEXTSVC_CLIPLENL
      LDA   ts_buflenh
      STA   TEXTSVC_CLIPLENH
      LDA   #0
      RTS

; Fetch the shared clipboard into RAM at XMC_RAML/H.
textsvc_clip_fetch_to_ram:
      LDA   TEXTSVC_CLIPLENL
      ORA   TEXTSVC_CLIPLENH
      BNE   :+
      LDA   #1
      RTS
:     LDA   #TEXTSVC_CLIP_XRAM_L
      STA   ts_xl
      LDA   #TEXTSVC_CLIP_XRAM_M
      STA   ts_xm
      LDA   #TEXTSVC_CLIP_XRAM_H
      STA   ts_xh
      LDA   XMC_RAML
      STA   ts_raml
      LDA   XMC_RAMH
      STA   ts_ramh
      LDA   TEXTSVC_CLIPLENL
      STA   ts_worklenl
      LDA   TEXTSVC_CLIPLENH
      STA   ts_worklenh
      JSR   ts_copy_xram_to_ram
      LDA   #0
      RTS

ts_clip_len_fits:
      LDA   ts_buflenh
      CMP   #>TEXTSVC_CLIP_CAP_BYTES
      BCC   @fits
      BNE   @too_big
      LDA   ts_buflenl
      BEQ   @fits
@too_big:
      SEC
      RTS
@fits:
      CLC
      RTS

; ---------------------------------------------------------------------
; Undo / redo
; ---------------------------------------------------------------------

; textsvc_undo_reset — drop all history. The editor calls this whenever the RAM
; window stops describing the same text (a pager window swap), because record
; offsets are window-relative and applying one to a different window would
; splice unrelated bytes into the buffer.
textsvc_undo_reset:
      STZ   ts_pre_valid
      JSR   ts_reset_undo
      JMP   ts_reset_redo

; textsvc_undo_capture — open an undo group: keep a pre-image of the buffer to
; take the record's payload from, and start tracking which part of it the edits
; touch. Everything between here and the next commit becomes one undo step.
textsvc_undo_capture:
      JSR   ts_save_current_args
      JSR   ts_reset_redo
      STZ   ts_pre_valid
      LDA   ts_buflenh
      CMP   #TEXTSVC_PRE_CAP_HI
      BCC   :+
      LDA   #1                        ; too big to shadow: this edit gets no undo
      RTS
:     JSR   ts_point_pre_image
      LDX   #TSV_SAVE
      JSR   ts_set_ram
      LDX   #TSV_WORK
      LDY   #TSV_BUFLEN
      JSR   ts_mov16
      JSR   ts_copy_ram_to_xram
      LDX   #TSV_PRELEN
      LDY   #TSV_BUFLEN
      JSR   ts_mov16
      LDA   TEXTSVC_CURL
      STA   ts_v+TSV_PRECUR
      LDA   TEXTSVC_CURH
      STA   ts_v+TSV_PRECUR+1
      LDA   #TSV_UNTOUCHED
      STA   ts_v+TSV_PMIN
      STA   ts_v+TSV_PMIN+1
      STZ   ts_v+TSV_PEND
      STZ   ts_v+TSV_PEND+1
      STZ   ts_v+TSV_SHIFT
      STZ   ts_v+TSV_SHIFT+1
      LDA   #1
      STA   ts_pre_valid
      LDA   #0
      RTS

; textsvc_undo_commit — close the group and push its record. Every after-change
; path calls this, so it must be a no-op both when no group is open and when
; nothing inside the group actually moved a byte.
textsvc_undo_commit:
      LDA   ts_pre_valid
      BNE   :+
      LDA   #1
      RTS
:     STZ   ts_pre_valid
      LDA   ts_v+TSV_PMIN
      AND   ts_v+TSV_PMIN+1
      CMP   #TSV_UNTOUCHED
      BNE   :+
      LDA   #1                        ; nothing was touched: nothing to record
      RTS
      ; old_span = pend - pmin; new_span is that plus what the group added
:     LDX   #TSV_OS
      LDY   #TSV_PEND
      JSR   ts_mov16
      LDX   #TSV_OS
      LDY   #TSV_PMIN
      JSR   ts_sub16
      LDX   #TSV_NS
      LDY   #TSV_OS
      JSR   ts_mov16
      LDX   #TSV_NS
      LDY   #TSV_SHIFT
      JSR   ts_add16
      ; payload = the removed run, still sitting in the pre-image at prefix
      JSR   ts_point_pre_image
      LDX   #TSA_SRC
      LDY   #TSA_X
      JSR   ts_mov24
      CLC
      LDA   ts_a+TSA_SRC+0
      ADC   ts_v+TSV_PMIN
      STA   ts_a+TSA_SRC+0
      LDA   ts_a+TSA_SRC+1
      ADC   ts_v+TSV_PMIN+1
      STA   ts_a+TSA_SRC+1
      LDA   ts_a+TSA_SRC+2
      ADC   #0
      STA   ts_a+TSA_SRC+2
      LDA   #1
      STA   ts_from_xram
      JSR   ts_load_undo
      JSR   ts_stack_push
      PHP
      JSR   ts_store_undo
      PLP
      JMP   ts_finish

; Undo and redo are one operation: pop the source stack, push the inverse onto
; the other one, and publish the splice for the caller to perform.
textsvc_undo_apply:
      LDA   #0
      BRA   ts_apply
textsvc_redo_apply:
      LDA   #1
ts_apply:
      STA   ts_sel
      JSR   ts_save_current_args
      STZ   ts_pre_valid              ; an undo is not an edit: never record it
      JSR   ts_load_src
      LDA   ts_valid
      BNE   :+
      LDA   #1
      RTS
:     JSR   ts_read_top_header
      JSR   ts_build_inverse
      JSR   ts_load_dst
      JSR   ts_stack_push
      JSR   ts_store_dst
      JSR   ts_load_src
      JSR   ts_stack_restore
      PHP
      JSR   ts_store_src
      PLP
      JMP   ts_finish

ts_point_pre_image:
      LDA   #TEXTSVC_PRE_XRAM_L
      STA   ts_xl
      LDA   #TEXTSVC_PRE_XRAM_M
      STA   ts_xm
      LDA   #TEXTSVC_PRE_XRAM_H
      STA   ts_xh
      RTS

; ts_build_inverse — rewrite ts_header as the record that undoes the one it
; currently holds, with the payload read from the live buffer before the
; splice. Undo and redo are the same operation in opposite directions.
ts_build_inverse:
      LDA   ts_v+TSV_HDR_NEW
      LDX   ts_v+TSV_HDR_OLD
      STA   ts_v+TSV_HDR_OLD
      STX   ts_v+TSV_HDR_NEW
      LDA   ts_v+TSV_HDR_NEW+1
      LDX   ts_v+TSV_HDR_OLD+1
      STA   ts_v+TSV_HDR_OLD+1
      STX   ts_v+TSV_HDR_NEW+1
      LDX   #TSV_HDR_LEN
      LDY   #TSV_BUFLEN
      JSR   ts_mov16
      LDA   TEXTSVC_CURL
      STA   ts_v+TSV_HDR_CUR
      LDA   TEXTSVC_CURH
      STA   ts_v+TSV_HDR_CUR+1
      LDX   #TSV_PSRC
      LDY   #TSV_SAVE
      JSR   ts_mov16
      LDX   #TSV_PSRC
      LDY   #TSV_HDR_PREFIX
      JSR   ts_add16
      STZ   ts_from_xram
      RTS

ts_save_current_args:
      LDA   XMC_RAML
      STA   ts_savel
      LDA   XMC_RAMH
      STA   ts_saveh
      LDA   XMC_LENL
      STA   ts_buflenl
      LDA   XMC_LENH
      STA   ts_buflenh
      RTS

; A = 0 (undo) or 1 (redo): point the arena base at that stack and return its
; shadow block index in X.
ts_stack_index:
      TAY
      LDA   ts_arena_m,Y
      STA   ts_base_xm
      LDA   ts_arena_h,Y
      STA   ts_base_xh
      LDX   #TSS_UNDO
      CPY   #0
      BEQ   :+
      LDX   #TSS_REDO
:     RTS

ts_arena_m:
      .byte TEXTSVC_UNDO_XRAM_M, TEXTSVC_REDO_XRAM_M
ts_arena_h:
      .byte TEXTSVC_UNDO_XRAM_H, TEXTSVC_REDO_XRAM_H

ts_load_stack:
      JSR   ts_stack_index
      LDY   #0
:     LDA   ts_shadow,X
      STA   ts_v+TSV_USED,Y
      INX
      INY
      CPY   #TSS_BLOCK
      BNE   :-
      RTS

ts_store_stack:
      JSR   ts_stack_index
      LDY   #0
:     LDA   ts_v+TSV_USED,Y
      STA   ts_shadow,X
      INX
      INY
      CPY   #TSS_BLOCK
      BNE   :-
      RTS

ts_reset_stack:
      JSR   ts_stack_index
      LDY   #TSS_BLOCK
:     STZ   ts_shadow,X
      INX
      DEY
      BNE   :-
      RTS

ts_reset_undo:
      LDA   #0
      BRA   ts_reset_stack
ts_reset_redo:
      LDA   #1
      BRA   ts_reset_stack
ts_load_undo:
      LDA   #0
      BRA   ts_load_stack
ts_store_undo:
      LDA   #0
      BRA   ts_store_stack

; The stack an apply reads from, and the one its inverse is pushed onto.
ts_load_src:
      LDA   ts_sel
      BRA   ts_load_stack
ts_store_src:
      LDA   ts_sel
      BRA   ts_store_stack
ts_load_dst:
      LDA   ts_sel
      EOR   #1
      BRA   ts_load_stack
ts_store_dst:
      LDA   ts_sel
      EOR   #1
      BRA   ts_store_stack

ts_reset_active:
      LDX   #TSS_BLOCK
:     STZ   ts_v+TSV_USED-1,X
      DEX
      BNE   :-
      RTS

; Carry -> A: 0 when the operation succeeded, 1 when it did not.
ts_finish:
      LDA   #0
      BCC   :+
      LDA   #1
:     RTS

; Push the record described by ts_header (fields 2..11 filled by the caller)
; onto the active stack, its payload read from XRAM at a[TSA_SRC] or from RAM
; at v[TSV_PSRC] depending on ts_from_xram. If the arena is full, older history
; is discarded and this record becomes the only entry.
ts_stack_push:
      LDX   #TSV_RECORD
      LDY   #TSV_HDR_OLD
      JSR   ts_mov16
      CLC
      LDA   ts_v+TSV_RECORD
      ADC   #TEXTSVC_STACK_HEADER_BYTES
      STA   ts_v+TSV_RECORD
      BCC   :+
      INC   ts_v+TSV_RECORD+1
:     LDX   #TSV_RECORD
      JSR   ts_fits_arena
      BCC   :+
      SEC
      RTS
:     JSR   ts_compute_end
      LDX   #TSV_END
      JSR   ts_fits_arena
      BCC   @room
      JSR   ts_reset_active
      LDX   #TSV_END
      LDY   #TSV_RECORD
      JSR   ts_mov16
@room:
      LDA   ts_valid
      BEQ   @empty_prev
      LDX   #TSV_HDR_PREV
      LDY   #TSV_TOP
      JSR   ts_mov16
      BRA   @have_prev
@empty_prev:
      LDA   #TEXTSVC_STACK_EMPTY_L
      STA   ts_v+TSV_HDR_PREV
      LDA   #TEXTSVC_STACK_EMPTY_H
      STA   ts_v+TSV_HDR_PREV+1
@have_prev:
      STZ   ts_header+12
      STZ   ts_header+13

      LDX   #TSV_OFF
      LDY   #TSV_USED
      JSR   ts_mov16
      JSR   ts_xaddr_from_active_off
      LDA   #<ts_header
      STA   ts_raml
      LDA   #>ts_header
      STA   ts_ramh
      LDA   #TEXTSVC_STACK_HEADER_BYTES
      STA   ts_worklenl
      STZ   ts_worklenh
      JSR   ts_copy_ram_to_xram

      LDX   #TSV_HDR_OLD
      JSR   ts_test16
      BEQ   @stored                   ; pure insertion: nothing was removed
      JSR   ts_payload_offset
      LDX   #TSV_WORK
      LDY   #TSV_HDR_OLD
      JSR   ts_mov16
      LDA   ts_from_xram
      BEQ   @from_ram
      JSR   ts_copy_xram_to_xram
      BRA   @stored
@from_ram:
      LDX   #TSV_PSRC
      JSR   ts_set_ram
      JSR   ts_copy_ram_to_xram
@stored:
      LDX   #TSV_TOP
      LDY   #TSV_USED
      JSR   ts_mov16
      LDX   #TSV_USED
      LDY   #TSV_END
      JSR   ts_mov16
      LDA   #1
      STA   ts_valid
      CLC
      RTS

; Point a[TSA_X] at the payload of the record whose header sits at v[TSV_USED]
; (push) or v[TSV_TOP] (restore) — both leave the offset in v[TSV_OFF].
ts_payload_offset:
      CLC
      LDA   ts_v+TSV_OFF
      ADC   #TEXTSVC_STACK_HEADER_BYTES
      STA   ts_v+TSV_OFF
      BCC   :+
      INC   ts_v+TSV_OFF+1
:     JMP   ts_xaddr_from_active_off

ts_read_top_header:
      LDX   #TSV_OFF
      LDY   #TSV_TOP
      JSR   ts_mov16
      JSR   ts_xaddr_from_active_off
      LDA   #<ts_header
      STA   ts_raml
      LDA   #>ts_header
      STA   ts_ramh
      LDA   #TEXTSVC_STACK_HEADER_BYTES
      STA   ts_worklenl
      STZ   ts_worklenh
      JMP   ts_copy_xram_to_ram

; Pop the top record and publish the splice it describes. The caller owns the
; buffer, so it performs the move with the gap routines it already has, then
; asks for the payload with textsvc_undo_paste.
ts_stack_restore:
      LDA   ts_valid
      BNE   :+
      SEC
      RTS
:     JSR   ts_read_top_header
      LDX   #TSV_SPL_OFF
      LDY   #TSV_HDR_PREFIX
      JSR   ts_mov16
      LDX   #TSV_SPL_CUT
      LDY   #TSV_HDR_NEW
      JSR   ts_mov16
      LDX   #TSV_SPL_INS
      LDY   #TSV_HDR_OLD
      JSR   ts_mov16
      ; The run it replaces has to be inside the live buffer, or this record
      ; describes different text (a window swap that missed textsvc_undo_reset).
      LDX   #TSV_TMP
      LDY   #TSV_SPL_OFF
      JSR   ts_mov16
      LDX   #TSV_TMP
      LDY   #TSV_SPL_CUT
      JSR   ts_add16
      LDX   #TSV_BUFLEN
      LDY   #TSV_TMP
      JSR   ts_cmp16
      BCS   :+
      JSR   textsvc_undo_reset
      JSR   ts_reset_active
      SEC
      RTS
      ; Where the payload sits, for the paste that follows the caller's move.
:     LDX   #TSV_OFF
      LDY   #TSV_TOP
      JSR   ts_mov16
      JSR   ts_payload_offset
      LDX   #TSA_SRC
      LDY   #TSA_X
      JSR   ts_mov24
      LDA   ts_v+TSV_HDR_LEN
      STA   TEXTSVC_REST_LENL
      LDA   ts_v+TSV_HDR_LEN+1
      STA   TEXTSVC_REST_LENH
      LDA   ts_v+TSV_HDR_CUR
      STA   TEXTSVC_REST_CURL
      LDA   ts_v+TSV_HDR_CUR+1
      STA   TEXTSVC_REST_CURH

      LDX   #TSV_USED
      LDY   #TSV_TOP
      JSR   ts_mov16
      LDA   ts_v+TSV_HDR_PREV
      CMP   #TEXTSVC_STACK_EMPTY_L
      BNE   @has_prev
      LDA   ts_v+TSV_HDR_PREV+1
      CMP   #TEXTSVC_STACK_EMPTY_H
      BNE   @has_prev
      STZ   ts_topl
      STZ   ts_toph
      STZ   ts_valid
      CLC
      RTS
@has_prev:
      LDX   #TSV_TOP
      LDY   #TSV_HDR_PREV
      JSR   ts_mov16
      LDA   #1
      STA   ts_valid
      CLC
      RTS

; textsvc_undo_paste — write the popped record's payload into RAM at
; XMC_RAML/H, once the caller has made room for it.
textsvc_undo_paste:
      LDX   #TSA_X
      LDY   #TSA_SRC
      JSR   ts_mov24
      LDA   XMC_RAML
      STA   ts_raml
      LDA   XMC_RAMH
      STA   ts_ramh
      LDX   #TSV_WORK
      LDY   #TSV_SPL_INS
      JSR   ts_mov16
      JMP   ts_copy_xram_to_ram

ts_compute_end:
      LDX   #TSV_END
      LDY   #TSV_USED
      JSR   ts_mov16
      LDX   #TSV_END
      LDY   #TSV_RECORD
      JSR   ts_add16
      BCC   @done
      LDA   #$FF
      STA   ts_v+TSV_END
      STA   ts_v+TSV_END+1
@done:
      RTS

; Carry set when v[X] does not fit inside one arena.
ts_fits_arena:
      LDA   ts_v+1,X
      CMP   #>TEXTSVC_UNDO_ARENA_BYTES
      BCC   @fits
      BNE   @too_big
      LDA   ts_v+0,X
      BEQ   @fits
@too_big:
      SEC
      RTS
@fits:
      CLC
      RTS

ts_xaddr_from_active_off:
      LDA   ts_offl
      STA   ts_xl
      CLC
      LDA   ts_base_xm
      ADC   ts_offh
      STA   ts_xm
      LDA   ts_base_xh
      ADC   #0
      STA   ts_xh
      RTS

; ---------------------------------------------------------------------
; Minimal XRAM window copy helpers
; ---------------------------------------------------------------------

ts_map_xram:
      LDA   XMC_WINCTL
      ORA   #TEXTSVC_WIN3_ENABLE
      STA   XMC_WINCTL
      STZ   WIN3_LO
      LDA   ts_xm
      STA   WIN3_MI
      LDA   ts_xh
      STA   WIN3_HI
      RTS

ts_advance_xram:
      INC   ts_xl
      BNE   @done
      INC   ts_xm
      BNE   @remap
      INC   ts_xh
@remap:
      JSR   ts_map_xram
@done:
      RTS

ts_advance_ram:
      INC   ts_raml
      BNE   @done
      INC   ts_ramh
@done:
      RTS

ts_dec_worklen:
      LDX   #TSV_WORK
      JMP   ts_dec16

ts_copy_ram_to_xram:
      LDX   #TSV_WORK
      JSR   ts_test16
      BEQ   @done
      JSR   ts_map_xram
@loop:
      LDY   #0
      LDA   (ts_raml),Y
      LDX   ts_xl
      STA   WIN3_BASE,X
      JSR   ts_advance_ram
      JSR   ts_advance_xram
      JSR   ts_dec_worklen
      LDX   #TSV_WORK
      JSR   ts_test16
      BNE   @loop
@done:
      RTS

ts_copy_xram_to_ram:
      LDX   #TSV_WORK
      JSR   ts_test16
      BEQ   @done
      JSR   ts_map_xram
@loop:
      LDX   ts_xl
      LDA   WIN3_BASE,X
      LDY   #0
      STA   (ts_raml),Y
      JSR   ts_advance_ram
      JSR   ts_advance_xram
      JSR   ts_dec_worklen
      LDX   #TSV_WORK
      JSR   ts_test16
      BNE   @loop
@done:
      RTS

; ts_copy_xram_to_xram — v[TSV_WORK] bytes from a[TSA_SRC] to a[TSA_X]. Only
; one XMC window is mapped at a time, so each byte swaps the window twice;
; payloads are single edits, so the extra maps are cheaper than a staging
; buffer would be in ROM.
ts_copy_xram_to_xram:
      LDX   #TSA_DST
      LDY   #TSA_X
      JSR   ts_mov24
@loop:
      LDX   #TSV_WORK
      JSR   ts_test16
      BEQ   @done
      LDX   #TSA_X
      LDY   #TSA_SRC
      JSR   ts_mov24
      JSR   ts_map_xram
      LDX   ts_xl
      LDA   WIN3_BASE,X
      PHA
      JSR   ts_advance_xram
      LDX   #TSA_SRC
      LDY   #TSA_X
      JSR   ts_mov24
      LDX   #TSA_X
      LDY   #TSA_DST
      JSR   ts_mov24
      JSR   ts_map_xram
      PLA
      LDX   ts_xl
      STA   WIN3_BASE,X
      JSR   ts_advance_xram
      LDX   #TSA_DST
      LDY   #TSA_X
      JSR   ts_mov24
      JSR   ts_dec_worklen
      BRA   @loop
@done:
      RTS

; ---------------------------------------------------------------------
; Recording: what each mutation did to the buffer
; ---------------------------------------------------------------------

; textsvc_undo_inserted / textsvc_undo_deleted — the editor calls one of these
; from the two primitives every mutation goes through, passing the offset in
; TEXTSVC_SPAN and the byte count in TEXTSVC_CNT. Tracking the bounds of the
; touched region costs one pass over four 16-bit values, so a compound edit
; (block indent, replace-all, auto-indent) still commits as a single record:
; its bounds are simply the union.
textsvc_undo_inserted:
      LDA   #0
      BRA   ts_note
textsvc_undo_deleted:
      LDA   #1
ts_note:
      STA   ts_note_del
      LDA   ts_pre_valid
      BNE   :+
      RTS                             ; no group open: nothing to record
      ; the offset is in current coordinates; the record speaks in the
      ; pre-image's, so undo the shift this group has applied so far
:     LDX   #TSV_TMP
      LDY   #TSV_SPAN
      JSR   ts_mov16
      LDX   #TSV_TMP
      LDY   #TSV_SHIFT
      JSR   ts_sub16
      BCS   :+
      ; The shift counts everything the group has inserted, including bytes at
      ; or after this offset — a block indent puts both of a line's spaces at
      ; the same place. That can push the offset below the start of the
      ; document; widening the record to the front is always safe.
      STZ   ts_v+TSV_TMP
      STZ   ts_v+TSV_TMP+1
:     LDX   #TSV_PMIN
      LDY   #TSV_TMP
      JSR   ts_cmp16
      BCC   :+
      LDX   #TSV_PMIN
      JSR   ts_mov16                  ; pmin = min(pmin, offset)
:     LDA   ts_note_del
      BEQ   :+
      LDX   #TSV_TMP
      LDY   #TSV_CNT
      JSR   ts_add16                  ; a delete consumes CNT original bytes
:     LDX   #TSV_PEND
      LDY   #TSV_TMP
      JSR   ts_cmp16
      BCS   :+
      LDX   #TSV_PEND
      JSR   ts_mov16                  ; pend = max(pend, end of this edit)
:     LDX   #TSV_SHIFT
      LDY   #TSV_CNT
      LDA   ts_note_del
      BEQ   :+
      JMP   ts_sub16
:     JMP   ts_add16
.endif

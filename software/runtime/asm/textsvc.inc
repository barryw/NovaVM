; =====================================================================
;  Nova NDK — textsvc.inc
;
;  Shared NDK text-editing services. TEXTSVC owns editor-wide clipboard
;  and undo/redo payload storage in XRAM, so callers use this API instead
;  of owning editor-local clipboard/undo memory. Undo records are deltas:
;  the caller brackets each edit with capture/commit and reports the byte
;  movements in between, and TEXTSVC keeps only what the edit removed.
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

.ifndef TEXTSVC_INC_INCLUDED
TEXTSVC_INC_INCLUDED = 1

.include "nova.inc"

; Per-stack arena size. Stack offsets are 16-bit throughout textsvc.s, so this
; can never reach $10000 without widening ts_v's offsets first — which is why
; snapshots could not be made deep enough by growing the arena, and deltas are
; the only way out.
TEXTSVC_UNDO_ARENA_BYTES = $8000
TEXTSVC_CLIP_CAP_BYTES   = $4000

; NDK-owned XRAM reservations for shared text editor services.
;   $040000-$047FFF undo records
;   $048000-$04FFFF redo records
;   $050000-$053FFF shared clipboard
;   $070000-$07BFFF pre-image of the live buffer
; $054000+ is editor/NUI scratch; docbuf owns $055000-$05CFFF. The library shelf
; owns SHELF_N=4 slots at $060000-$06FFFF and the host extension stages at
; $07C000, which leaves $070000-$07BFFF as the largest free block. Redo used to
; sit at $06C000 — on top of shelf slot 3 — and only got away with it because
; nothing wrote there until the user pressed redo.
;
; Records are deltas (14-byte header + the bytes the edit removed), so depth is
; independent of document size: typing one character costs ~15 bytes, not one
; whole buffer. A 32K arena is therefore thousands of steps; the only edit that
; cannot be recorded is one whose removed run alone exceeds the arena.
TEXTSVC_UNDO_XRAM_L = $00
TEXTSVC_UNDO_XRAM_M = $00
TEXTSVC_UNDO_XRAM_H = $04
TEXTSVC_REDO_XRAM_L = $00
TEXTSVC_REDO_XRAM_M = $80
TEXTSVC_REDO_XRAM_H = $04
TEXTSVC_CLIP_XRAM_L = $00
TEXTSVC_CLIP_XRAM_M = $00
TEXTSVC_CLIP_XRAM_H = $05
TEXTSVC_PRE_XRAM_L  = $00
TEXTSVC_PRE_XRAM_M  = $00
TEXTSVC_PRE_XRAM_H  = $07
; The pre-image reservation ends at the host extension staging area, so a
; buffer bigger than this cannot be captured and simply gets no undo.
TEXTSVC_PRE_CAP_HI  = $C0

.global TEXTSVC_CLIPLENL
.global TEXTSVC_CLIPLENH
.global TEXTSVC_SPANL
.global TEXTSVC_SPANH
.global TEXTSVC_CNTL
.global TEXTSVC_CNTH
.global TEXTSVC_SPLICE_OFFL
.global TEXTSVC_SPLICE_OFFH
.global TEXTSVC_SPLICE_CUTL
.global TEXTSVC_SPLICE_CUTH
.global TEXTSVC_SPLICE_INSL
.global TEXTSVC_SPLICE_INSH
.global TEXTSVC_CURL
.global TEXTSVC_CURH
.global TEXTSVC_REST_LENL
.global TEXTSVC_REST_LENH
.global TEXTSVC_REST_CURL
.global TEXTSVC_REST_CURH

.global textsvc_init
.global textsvc_clip_release
.global textsvc_clip_copy_from_ram
.global textsvc_clip_fetch_to_ram
.global textsvc_undo_capture
.global textsvc_undo_commit
.global textsvc_undo_reset
.global textsvc_undo_inserted
.global textsvc_undo_deleted
.global textsvc_undo_paste
.global textsvc_undo_apply
.global textsvc_redo_apply

.endif

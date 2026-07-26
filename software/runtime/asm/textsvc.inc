; =====================================================================
;  Nova NDK — textsvc.inc
;
;  Shared NDK text-editing services. TEXTSVC owns editor-wide clipboard
;  and undo/redo payload storage; the first backend is XRAM-backed
;  snapshots for the RAM edit buffer, so callers use this API instead of
;  owning editor-local clipboard/undo memory.
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

.ifndef TEXTSVC_INC_INCLUDED
TEXTSVC_INC_INCLUDED = 1

.include "nova.inc"

; Stack offsets are 16-bit throughout textsvc.s, so this cannot reach $10000
; without widening ts_record*/ts_end* first. $FF00 is the largest value the
; existing bound checks handle correctly.
TEXTSVC_UNDO_ARENA_BYTES = $FF00
TEXTSVC_CLIP_CAP_BYTES   = $4000

; NDK-owned XRAM reservations for shared text editor services.
;   $040000-$04FFFF undo snapshots
;   $06C000-$07BFFF redo snapshots
;   $050000-$053FFF shared clipboard
; $054000+ is editor/NUI scratch; docbuf owns $055000-$05CFFF. Module slots 0-2
; own $060000-$06BFFF and the host extension stages at $07C000, so $06C000 is
; the only free 64K block; redo moved there to let undo take all of $04xxxx.
;
; Records are whole-buffer snapshots, so depth is arena/document-size: this
; buys roughly 15 steps on a 4K source where there were 7. Making depth
; independent of document size needs delta records, not a bigger arena.
TEXTSVC_UNDO_XRAM_L = $00
TEXTSVC_UNDO_XRAM_M = $00
TEXTSVC_UNDO_XRAM_H = $04
TEXTSVC_REDO_XRAM_L = $00
TEXTSVC_REDO_XRAM_M = $C0
TEXTSVC_REDO_XRAM_H = $06
TEXTSVC_CLIP_XRAM_L = $00
TEXTSVC_CLIP_XRAM_M = $00
TEXTSVC_CLIP_XRAM_H = $05

.global TEXTSVC_CLIPLENL
.global TEXTSVC_CLIPLENH
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
.global textsvc_undo_apply
.global textsvc_redo_apply

.endif

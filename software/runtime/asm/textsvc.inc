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

TEXTSVC_UNDO_ARENA_BYTES = $8000
TEXTSVC_CLIP_CAP_BYTES   = $4000

; NDK-owned XRAM reservations for shared text editor services.
;   $040000-$047FFF undo snapshots
;   $048000-$04FFFF redo snapshots
;   $050000-$053FFF shared clipboard
; $054000+ is editor/NUI scratch; docbuf owns $055000-$05CFFF.
TEXTSVC_UNDO_XRAM_L = $00
TEXTSVC_UNDO_XRAM_M = $00
TEXTSVC_UNDO_XRAM_H = $04
TEXTSVC_REDO_XRAM_L = $00
TEXTSVC_REDO_XRAM_M = $80
TEXTSVC_REDO_XRAM_H = $04
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

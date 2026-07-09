; =====================================================================
;  Nova NDK — nvg.inc
;
;  Shared NVG image-loader declarations. NVG2 files are native Nova images
;  (a small header plus row-packed 4bpp pixels) that the blitter expands
;  into the graphics bitmap plane.
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

.ifndef NVG_INC_INCLUDED
NVG_INC_INCLUDED = 1

.include "fio.inc"
.include "pager.inc"
.include "xram.inc"
.include "vgc_palette.inc"

NVG_MAGIC_0       = 'N'
NVG_MAGIC_1       = 'V'
NVG_MAGIC_2       = 'G'
NVG_MAGIC_2_VER   = '2'
NVG_HEADER_SIZE   = $10
NVG_PALETTE_BYTES = VGC_CUSTOM_PALETTE_BYTES
NVG_GFX_STRIDE    = 320
NVG_SCREEN_WIDTH  = 320
NVG_SCREEN_HEIGHT = 200
NVG_FLAG_TRANSPARENT = $01
NVG_FLAG_PALETTE     = $02

; @label NVG.ADDRL
; @kind u8
; @symbol NVG_ADDRL
; @summary Low byte of the graphics bitmap destination offset for NVG.LOAD_AT.
NVG_ADDRL = FIO_GADDRL
; @label NVG.ADDRH
; @kind u8
; @symbol NVG_ADDRH
; @summary High byte of the graphics bitmap destination offset for NVG.LOAD_AT.
NVG_ADDRH = FIO_GADDRH
; @label NVG.NAMELEN
; @kind zp
; @symbol NVG_NAMELEN
; @summary Filename length argument for NVG.LOAD_NAMED.
NVG_NAMELEN = FIO_ARG_NAMELEN
; @label NVG.NAMEPTR_L
; @kind zp
; @symbol NVG_NAMEPTR_L
; @summary Low byte of filename pointer argument for NVG.LOAD_NAMED.
NVG_NAMEPTR_L = FIO_ARG_NAMEPTR_L
; @label NVG.NAMEPTR_H
; @kind zp
; @symbol NVG_NAMEPTR_H
; @summary High byte of filename pointer argument for NVG.LOAD_NAMED.
NVG_NAMEPTR_H = FIO_ARG_NAMEPTR_H

; @label NVG.FILEL
; @kind zp
; @symbol NVG_FILEL
; @summary Low byte of the source image offset inside the current NVG file.
NVG_FILEL = PAGER_FILEL
; @label NVG.FILEM
; @kind zp
; @symbol NVG_FILEM
; @summary Middle byte of the source image offset inside the current NVG file.
NVG_FILEM = PAGER_FILEM
; @label NVG.FILEH
; @kind zp
; @symbol NVG_FILEH
; @summary High byte of the source image offset inside the current NVG file.
NVG_FILEH = PAGER_FILEH

.global nvg_load
.global nvg_load_at
.global nvg_load_named
.global nvg_load_named_at
.global nvg_draw
.global nvg_draw_at
.global nvg_draw_named
.global nvg_draw_named_at
.endif

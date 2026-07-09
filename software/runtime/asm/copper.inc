; =====================================================================
;  Nova NDK — copper.inc
;
;  Copper helper ABI: build/enable/disable per-scanline copper lists that
;  rewrite VGC registers (mode, background color, scroll) mid-frame.
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

.ifndef COPPER_INC_INCLUDED
COPPER_INC_INCLUDED = 1

.include "vgc.inc"

COPPER_RESULT_OK    = $00
COPPER_RESULT_ERROR = $01

COPPER_REG_MODE     = $00
COPPER_REG_BGCOL    = $01
COPPER_REG_SCROLLX  = $05
COPPER_REG_SCROLLY  = $06

.global copper_command
.global copper_clear
.global copper_on
.global copper_off
.global copper_list
.global copper_list_end
.global copper_use
.global copper_add
.global copper_set_reg_index
.global copper_set_sprite_reg

.endif

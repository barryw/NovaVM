; =====================================================================
;  Nova NDK — mouse.inc
;
;  Declarations for the raw VGC mouse-pointer helper routines (show/hide,
;  control flags, shape, hotspot, color, and button/X/Y readback).
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

.ifndef MOUSE_INC_INCLUDED
MOUSE_INC_INCLUDED = 1

.include "nova.inc"

MOUSE_DEFAULT_CTRL = VGC_MOUSE_CTRL_ENABLE | VGC_MOUSE_CTRL_AUTO

.global mouse_show
.global mouse_hide
.global mouse_set_ctrl
.global mouse_set_shape
.global mouse_set_hotspot
.global mouse_set_color
.global mouse_get_buttons
.global mouse_get_x
.global mouse_get_y

.endif

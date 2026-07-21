; =====================================================================
;  Nova NDK — vgc.inc
;
;  Shared Nova VGC helper declarations. Command helpers use the VGC
;  hardware parameter registers directly: callers load VGC_P0..VGC_P14 as
;  needed, then call a vgc_* routine. Completion is checked with
;  vgc_wait_cmd when a synchronous operation is required.
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

.ifndef VGC_INC_INCLUDED
VGC_INC_INCLUDED = 1

.include "nova.inc"

VGC_RESULT_OK    = $00
VGC_RESULT_ERROR = $01

.global vgc_issue
.global vgc_wait_cmd
.global vgc_exec
.global vgc_vsync
.global vgc_wait_frames
.global vgc_cls
.global vgc_set_fg
.global vgc_set_bg
.global vgc_set_border
.global vgc_set_charbg
.global vgc_charbg_off
.global vgc_display_on
.global vgc_display_off
.global vgc_irq_install
.global vgc_irq_enable
.global vgc_irq_disable
.global vgc_irq_ack
.global vgc_locate
.global vgc_set_mode
.global vgc_set_font
.global vgc_mem_read
.global vgc_mem_write
.global vgc_reverse_default
.global vgc_reverse_explicit
.global vgc_reverse_off
.global vgc_flash_on
.global vgc_flash_off
.global vgc_gcls
.global vgc_gcolor
.global vgc_plot
.global vgc_unplot
.global vgc_line
.global vgc_circle
.global vgc_gtext
.global vgc_rect
.global vgc_fill
.global vgc_paint

.endif

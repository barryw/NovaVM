; =====================================================================
;  Nova NDK — array.inc
;
;  Language-neutral 16-bit indexed-address helpers.
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

.ifndef ARRAY_INC_INCLUDED
ARRAY_INC_INCLUDED = 1

.include "nova.inc"

; The general helpers consume two inline words after JSR: lower bound, then
; byte stride. A/X contains the unsigned 16-bit index and returns the address.
.global i_array_index_addr
.global i_array_index_addr_stack

; The byte-offset helper computes base + A/X and consumes a base address saved
; on the CPU stack beneath the JSR return address. It has no inline operands.
.global i_array_index_byte_stack

.endif

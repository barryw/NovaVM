; =====================================================================
;  Nova NDK — array.s
;
;  Language-neutral 16-bit indexed-address helpers.
;
;  Copyright (C) 2026 Barry Walker
;  SPDX-License-Identifier: MIT
; =====================================================================

.include "array.inc"

.ifndef ARRAY_IMPLEMENTATION_INCLUDED
ARRAY_IMPLEMENTATION_INCLUDED = 1

      .segment "CODE"

      .export i_array_index_addr
      .export i_array_index_addr_stack
      .export i_array_index_byte_stack

; @label I_ARRAY_INDEX_ADDR
; @kind routine
; @symbol i_array_index_addr
; @library array
; @abi inline
; @summary Compute base + (index - lower bound) * stride.
; @in A/X: Unsigned 16-bit index; NVR3L/H: base address.
; @in Inline: 16-bit lower bound, then 16-bit byte stride.
; @out A/X: Effective address.
; @note Advances the saved return address past both inline words; NVR0-NVR5 and Y are clobbered.

; @label I_ARRAY_INDEX_ADDR_STACK
; @kind routine
; @symbol i_array_index_addr_stack
; @library array
; @abi inline
; @summary Compute a strided address and consume a base address saved on the CPU stack.
; @in A/X: Unsigned 16-bit index; stack: base high then base low beneath the return address.
; @in Inline: 16-bit lower bound, then 16-bit byte stride.
; @out A/X: Effective address; saved base word removed from the CPU stack.
; @note Advances the saved return address past both inline words; NVR0-NVR5 and Y are clobbered.

; @label I_ARRAY_INDEX_BYTE_STACK
; @kind routine
; @symbol i_array_index_byte_stack
; @library array
; @abi register
; @summary Compute base + an unsigned 16-bit byte offset and consume the stacked base.
; @in A/X: Unsigned 16-bit byte offset; stack: base high then base low beneath the return address.
; @out A/X: Effective address; saved base word removed from the CPU stack.
; @note NVR0-NVR1 and Y are clobbered.

.include "array_index_core.body"
.include "array_index_byte_stack.body"

.endif

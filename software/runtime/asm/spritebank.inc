; Nova sprite-bank (NSPR) loader declarations.
;
; Parses an in-memory .nsp metasprite bank (docs/sprite-editor/NSPR.md) and
; indexes it so the shape pool can be streamed to XRAM and each character spawned
; as a metasprite (msprite.s) + animated (anim.s). This first stage validates the
; header and computes the shape-pool and character-table pointers; later stages
; walk the character/animation records off spritebank_chars_ptr.

.ifndef SPRITEBANK_INC_INCLUDED
SPRITEBANK_INC_INCLUDED = 1

.include "nova.inc"

SPRITEBANK_OK          = $00
SPRITEBANK_ERR_MAGIC   = $01
SPRITEBANK_ERR_VERSION = $02

SPRITEBANK_VERSION     = $01
SPRITEBANK_SHAPE_BYTES = 128         ; 16x16 4bpp cell

.global spritebank_open
.global spritebank_load_shapes       ; DMA the shape pool to XRAM at spritebank_xram_base
.globalzp spritebank_src             ; ZP: caller sets this -> the NSPR buffer
.global spritebank_result            ; SPRITEBANK_* after spritebank_open
.global spritebank_shape_count
.global spritebank_char_count
.global spritebank_flags             ; bit0 = palette present
.global spritebank_shapes_ptr        ; -> shape pool (shape_count * 128 bytes)
.global spritebank_shapes_len        ; shape_count * 128 (byte length of the pool)
.global spritebank_chars_ptr         ; -> first character record
.global spritebank_xram_base         ; 3-byte flat XRAM dest (caller sets before load_shapes)

.endif

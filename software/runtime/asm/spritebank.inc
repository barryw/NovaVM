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
.global spritebank_load_to_sprites   ; copy the XRAM pool into sprite RAM slots 0..N-1
.globalzp spritebank_src             ; ZP: caller sets this -> the NSPR buffer
.global spritebank_result            ; SPRITEBANK_* after spritebank_open
.global spritebank_shape_count
.global spritebank_char_count
.global spritebank_flags             ; bit0 = palette present
.global spritebank_shapes_ptr        ; -> shape pool (shape_count * 128 bytes)
.global spritebank_shapes_len        ; shape_count * 128 (byte length of the pool)
.global spritebank_chars_ptr         ; -> first character record
.global spritebank_xram_base         ; 3-byte flat XRAM dest (caller sets before load_shapes)

; spritebank_char_seek (A = character index) walks the (variable-length)
; character table and fills the fields below for that character.
.global spritebank_char_seek
.globalzp spritebank_walk            ; ZP scratch pointer used while walking
.global spritebank_char_ptr          ; -> the sought character record
.global spritebank_char_part_count
.global spritebank_char_parts_ptr    ; -> parts block: { dx, dy, flags } * part_count
.global spritebank_char_anim_count
.global spritebank_char_anims_ptr    ; -> animations block

; spritebank_build_vis writes an msprite visual descriptor for the seeked
; character (call spritebank_char_seek first) into the buffer at spritebank_dst:
;   [0]=part_count [1]=flags then part_count * { dx, dy, shape_base, flags }.
; shape_base per part = that part's shape in frame 0 of the character's first
; animation (0 if the character has no animations).
.global spritebank_build_vis
.globalzp spritebank_dst             ; ZP: caller sets -> destination VIS buffer

; spritebank_spawn (A = character index) seeks the character, builds its VIS into
; spritebank_vis, and msprite_spawns it. Requires spritebank_open +
; spritebank_load_to_sprites + msprite_init to have run. Returns msprite_spawn's
; result in A (the object handle path). Call msprite_commit to push it live.
.global spritebank_spawn
.global spritebank_vis                ; VIS buffer spritebank_spawn builds into
.global spritebank_handle             ; msprite handle of the last spritebank_spawn

; spritebank_set_frame (A = frame index) re-poses the spawned object with that
; frame's exact per-part shapes from animation 0 (NSPR's arbitrary frame table),
; then marks it dirty. Call msprite_commit to show it. Honors NSPR's arbitrary
; frames — it does NOT use msprite's shape_base+frame tick.
.global spritebank_set_frame

.endif

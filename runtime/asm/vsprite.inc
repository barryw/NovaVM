; Shared Nova virtual-sprite helper declarations.
;
; A virtual sprite is a caller-owned rectangular pixel stamp copied by the
; blitter into a VGC memory space. Unlike hardware sprites, the count is limited
; by memory and redraw cost rather than VGC sprite registers.
;
; Transparency is split into two explicit layers:
; - VSPRITE_COLORKEY controls which source byte VSPRITE.BLIT skips.
; - VGC_GFXTRANS controls which color in the VGC graphics plane is transparent
;   during final display composition.
;
; The default for both is 0. If a runtime needs visible black graphics-plane
; pixels, choose another unused color as the transparent color, store it in
; VGC_GFXTRANS, write that value into transparent source pixels, and set
; VSPRITE_COLORKEY to the same value before a color-keyed blit.
;
; For moving objects, use the graphics-plane saved-background helpers in the
; same style as an Amiga BOB:
; 1. Set VSPRITE.X/Y, VSPRITE.WIDTH/HEIGHT, and VSPRITE.BG* to caller-owned RAM.
; 2. Call VSPRITE.GFX_SAVE_BG before drawing the object.
; 3. Draw with VSPRITE.GFX_BLIT.
; 4. Before the next frame, call VSPRITE.GFX_RESTORE_BG at the saved X/Y, move
;    the object, save the new background, and draw again.
; Static objects that were not first drawn with a saved background must be
; erased by the caller before entering this loop.
;
; For multiple moving virtual sprites, use the scene helpers instead of calling
; the single-sprite save/restore loop for each object. A scene descriptor list
; restores every old background first, lets the caller update positions, saves
; every new background from the clean playfield, then draws the list in order.
; List order is z-order: later descriptors draw in front of earlier ones.
;
; For scenes where multiple virtual sprites overlap or cross paths, use
; VSPRITE.SCENE_COMMIT_ATOMIC. The caller supplies a dirty graphics rectangle
; and a work buffer. The helper composes the clean old backgrounds and new
; sprites into that offscreen buffer, then copies the completed rectangle to
; the graphics plane as one visible blit. For a truly atomic visual update, the
; final dirty rectangle must be small enough to fit in one vblank window.
;
; For in-place rotated virtual sprites on the transparent graphics plane, use
; VSPRITE.GFX_ROTATE_BLIT. It rotates into VSPRITE.ROT* offscreen, then copies
; the full rotated rectangle to graphics memory in one blit so old pixels are
; cleared without a separate visible erase.

.ifndef VSPRITE_INC_INCLUDED
VSPRITE_INC_INCLUDED = 1

.include "blitter.inc"
.include "nova.inc"

VSPRITE_RESULT_OK        = BLITTER_RESULT_OK
VSPRITE_RESULT_ERROR     = BLITTER_RESULT_ERROR

; @label VSPRITE.FLAG_COLORKEY
; @kind const
; @symbol VSPRITE_FLAG_COLORKEY
; @summary Enable source color-key transparency for VSPRITE.BLIT and VSPRITE.GFX_BLIT.
VSPRITE_FLAG_COLORKEY    = $01

; @label VSPRITE.DESC_FLAG_COLORKEY
; @kind const
; @symbol VSPRITE_DESC_FLAG_COLORKEY
; @summary Descriptor flag enabling source color-key transparency.
VSPRITE_DESC_FLAG_COLORKEY = VSPRITE_FLAG_COLORKEY

; @label VSPRITE.DESC_SAVED
; @kind const
; @symbol VSPRITE_DESC_SAVED
; @summary Internal descriptor flag set when old X/Y has a saved background to restore.
VSPRITE_DESC_SAVED       = $40

; @label VSPRITE.DESC_VISIBLE
; @kind const
; @symbol VSPRITE_DESC_VISIBLE
; @summary Descriptor flag enabling scene save/draw for this virtual sprite.
VSPRITE_DESC_VISIBLE     = $80

VSPRITE_GFX_STRIDE       = 320

; Scene descriptor layout. Descriptors are caller-owned RAM because scene
; helpers update OLD_* and SAVED as part of the frame protocol.
VSPRITE_DESC_FLAGS       = $00
VSPRITE_DESC_XL          = $01
VSPRITE_DESC_XH          = $02
VSPRITE_DESC_Y           = $03
VSPRITE_DESC_OLD_XL      = $04
VSPRITE_DESC_OLD_XH      = $05
VSPRITE_DESC_OLD_Y       = $06
VSPRITE_DESC_WIDTHL      = $07
VSPRITE_DESC_WIDTHH      = $08
VSPRITE_DESC_HEIGHTL     = $09
VSPRITE_DESC_HEIGHTH     = $0A
VSPRITE_DESC_SRCSPACE    = $0B
VSPRITE_DESC_SRCADDRL    = $0C
VSPRITE_DESC_SRCADDRM    = $0D
VSPRITE_DESC_SRCADDRH    = $0E
VSPRITE_DESC_SRCSTRL     = $0F
VSPRITE_DESC_SRCSTRH     = $10
VSPRITE_DESC_BGSPACE     = $11
VSPRITE_DESC_BGADDRL     = $12
VSPRITE_DESC_BGADDRM     = $13
VSPRITE_DESC_BGADDRH     = $14
VSPRITE_DESC_BGSTRL      = $15
VSPRITE_DESC_BGSTRH      = $16
VSPRITE_DESC_COLORKEY    = $17
VSPRITE_DESC_SIZE        = $18

; Assembly-facing state ABI. Callers set these before invoking the helpers.
; @label VSPRITE.SRCSPACE
; @kind u8
; @symbol VSPRITE_SRCSPACE
; @summary Blitter memory space for the virtual-sprite source rectangle.
.global VSPRITE_SRCSPACE
; @label VSPRITE.DSTSPACE
; @kind u8
; @symbol VSPRITE_DSTSPACE
; @summary Blitter memory space for the virtual-sprite destination rectangle.
.global VSPRITE_DSTSPACE
; @label VSPRITE.SRCADDRL
; @kind u8
; @symbol VSPRITE_SRCADDRL
; @summary Low byte of the virtual-sprite source address.
.global VSPRITE_SRCADDRL
; @label VSPRITE.SRCADDRM
; @kind u8
; @symbol VSPRITE_SRCADDRM
; @summary Middle byte of the virtual-sprite source address.
.global VSPRITE_SRCADDRM
; @label VSPRITE.SRCADDRH
; @kind u8
; @symbol VSPRITE_SRCADDRH
; @summary High byte of the virtual-sprite source address.
.global VSPRITE_SRCADDRH
; @label VSPRITE.DSTADDRL
; @kind u8
; @symbol VSPRITE_DSTADDRL
; @summary Low byte of the virtual-sprite destination address. VSPRITE.GFX_* routines compute this from VSPRITE.X/Y.
.global VSPRITE_DSTADDRL
; @label VSPRITE.DSTADDRM
; @kind u8
; @symbol VSPRITE_DSTADDRM
; @summary Middle byte of the virtual-sprite destination address. VSPRITE.GFX_* routines compute this from VSPRITE.X/Y.
.global VSPRITE_DSTADDRM
; @label VSPRITE.DSTADDRH
; @kind u8
; @symbol VSPRITE_DSTADDRH
; @summary High byte of the virtual-sprite destination address. VSPRITE.GFX_* routines compute this from VSPRITE.X/Y.
.global VSPRITE_DSTADDRH
; @label VSPRITE.WIDTHL
; @kind u8
; @symbol VSPRITE_WIDTHL
; @summary Low byte of the rectangle width in pixels/bytes.
.global VSPRITE_WIDTHL
; @label VSPRITE.WIDTHH
; @kind u8
; @symbol VSPRITE_WIDTHH
; @summary High byte of the rectangle width in pixels/bytes.
.global VSPRITE_WIDTHH
; @label VSPRITE.HEIGHTL
; @kind u8
; @symbol VSPRITE_HEIGHTL
; @summary Low byte of the rectangle height in rows.
.global VSPRITE_HEIGHTL
; @label VSPRITE.HEIGHTH
; @kind u8
; @symbol VSPRITE_HEIGHTH
; @summary High byte of the rectangle height in rows.
.global VSPRITE_HEIGHTH
; @label VSPRITE.SRCSTRL
; @kind u8
; @symbol VSPRITE_SRCSTRL
; @summary Low byte of the source row stride.
.global VSPRITE_SRCSTRL
; @label VSPRITE.SRCSTRH
; @kind u8
; @symbol VSPRITE_SRCSTRH
; @summary High byte of the source row stride.
.global VSPRITE_SRCSTRH
; @label VSPRITE.DSTSTRL
; @kind u8
; @symbol VSPRITE_DSTSTRL
; @summary Low byte of the destination row stride.
.global VSPRITE_DSTSTRL
; @label VSPRITE.DSTSTRH
; @kind u8
; @symbol VSPRITE_DSTSTRH
; @summary High byte of the destination row stride.
.global VSPRITE_DSTSTRH
; @label VSPRITE.BGSPACE
; @kind u8
; @symbol VSPRITE_BGSPACE
; @summary Blitter memory space for the saved-background buffer. Use BLT_SPACE_CPU for normal RAM buffers.
.global VSPRITE_BGSPACE
; @label VSPRITE.BGADDRL
; @kind u8
; @symbol VSPRITE_BGADDRL
; @summary Low byte of the caller-owned saved-background buffer address.
.global VSPRITE_BGADDRL
; @label VSPRITE.BGADDRM
; @kind u8
; @symbol VSPRITE_BGADDRM
; @summary Middle byte of the caller-owned saved-background buffer address.
.global VSPRITE_BGADDRM
; @label VSPRITE.BGADDRH
; @kind u8
; @symbol VSPRITE_BGADDRH
; @summary High byte of the caller-owned saved-background buffer address. Use 0 for CPU RAM.
.global VSPRITE_BGADDRH
; @label VSPRITE.BGSTRL
; @kind u8
; @symbol VSPRITE_BGSTRL
; @summary Low byte of the saved-background buffer row stride.
.global VSPRITE_BGSTRL
; @label VSPRITE.BGSTRH
; @kind u8
; @symbol VSPRITE_BGSTRH
; @summary High byte of the saved-background buffer row stride.
.global VSPRITE_BGSTRH
; @label VSPRITE.COLORKEY
; @kind u8
; @symbol VSPRITE_COLORKEY
; @summary Source byte skipped by color-keyed blits. VSPRITE.ROTATE also uses this value to fill output pixels outside the rotated source bounds.
.global VSPRITE_COLORKEY
; @label VSPRITE.ORIGSPACE
; @kind u8
; @symbol VSPRITE_ORIGSPACE
; @summary Blitter memory space for the immutable source shape used by VSPRITE.ROTATE.
.global VSPRITE_ORIGSPACE
; @label VSPRITE.ORIGADDRL
; @kind u8
; @symbol VSPRITE_ORIGADDRL
; @summary Low byte of the immutable source shape address used by VSPRITE.ROTATE.
.global VSPRITE_ORIGADDRL
; @label VSPRITE.ORIGADDRM
; @kind u8
; @symbol VSPRITE_ORIGADDRM
; @summary Middle byte of the immutable source shape address used by VSPRITE.ROTATE.
.global VSPRITE_ORIGADDRM
; @label VSPRITE.ORIGADDRH
; @kind u8
; @symbol VSPRITE_ORIGADDRH
; @summary High byte of the immutable source shape address used by VSPRITE.ROTATE. Use 0 for CPU RAM.
.global VSPRITE_ORIGADDRH
; @label VSPRITE.ORIGSTRL
; @kind u8
; @symbol VSPRITE_ORIGSTRL
; @summary Low byte of the immutable source shape row stride used by VSPRITE.ROTATE.
.global VSPRITE_ORIGSTRL
; @label VSPRITE.ORIGSTRH
; @kind u8
; @symbol VSPRITE_ORIGSTRH
; @summary High byte of the immutable source shape row stride used by VSPRITE.ROTATE.
.global VSPRITE_ORIGSTRH
; @label VSPRITE.ROTSPACE
; @kind u8
; @symbol VSPRITE_ROTSPACE
; @summary Blitter memory space for the caller-owned rotated output buffer used by VSPRITE.ROTATE.
.global VSPRITE_ROTSPACE
; @label VSPRITE.ROTADDRL
; @kind u8
; @symbol VSPRITE_ROTADDRL
; @summary Low byte of the caller-owned rotated output buffer address.
.global VSPRITE_ROTADDRL
; @label VSPRITE.ROTADDRM
; @kind u8
; @symbol VSPRITE_ROTADDRM
; @summary Middle byte of the caller-owned rotated output buffer address.
.global VSPRITE_ROTADDRM
; @label VSPRITE.ROTADDRH
; @kind u8
; @symbol VSPRITE_ROTADDRH
; @summary High byte of the caller-owned rotated output buffer address. Use 0 for CPU RAM.
.global VSPRITE_ROTADDRH
; @label VSPRITE.ROTSTRL
; @kind u8
; @symbol VSPRITE_ROTSTRL
; @summary Low byte of the caller-owned rotated output buffer row stride.
.global VSPRITE_ROTSTRL
; @label VSPRITE.ROTSTRH
; @kind u8
; @symbol VSPRITE_ROTSTRH
; @summary High byte of the caller-owned rotated output buffer row stride.
.global VSPRITE_ROTSTRH
; @label VSPRITE.ROTANGLE
; @kind u8
; @symbol VSPRITE_ROTANGLE
; @summary Hardware rotation angle, where 256 steps is one full turn.
.global VSPRITE_ROTANGLE
; @label VSPRITE.FILLVALUE
; @kind u8
; @symbol VSPRITE_FILLVALUE
; @summary Byte value used by VSPRITE.FILL and VSPRITE.GFX_FILL.
.global VSPRITE_FILLVALUE
; @label VSPRITE.FLAGS
; @kind u8
; @symbol VSPRITE_FLAGS
; @summary Bitfield controlling virtual-sprite blits. Set VSPRITE.FLAG_COLORKEY to enable source transparency.
.global VSPRITE_FLAGS
; @label VSPRITE.XL
; @kind u8
; @symbol VSPRITE_XL
; @summary Low byte of the graphics-plane X coordinate used by VSPRITE.GFX_* helpers.
.global VSPRITE_XL
; @label VSPRITE.XH
; @kind u8
; @symbol VSPRITE_XH
; @summary High byte of the graphics-plane X coordinate used by VSPRITE.GFX_* helpers.
.global VSPRITE_XH
; @label VSPRITE.Y
; @kind u8
; @symbol VSPRITE_Y
; @summary Graphics-plane Y coordinate used by VSPRITE.GFX_* helpers.
.global VSPRITE_Y

; @label VSPRITE.SCENE_ADDRL
; @kind u8
; @symbol VSPRITE_SCENE_ADDRL
; @summary Low byte of the caller-owned scene descriptor table.
.global VSPRITE_SCENE_ADDRL
; @label VSPRITE.SCENE_ADDRH
; @kind u8
; @symbol VSPRITE_SCENE_ADDRH
; @summary High byte of the caller-owned scene descriptor table.
.global VSPRITE_SCENE_ADDRH
; @label VSPRITE.SCENE_COUNT
; @kind u8
; @symbol VSPRITE_SCENE_COUNT
; @summary Number of descriptors in the scene table.
.global VSPRITE_SCENE_COUNT

; @label VSPRITE.SCENE_XL
; @kind u8
; @symbol VSPRITE_SCENE_XL
; @summary Low byte of the graphics-plane dirty rectangle X coordinate for atomic scene commits.
.global VSPRITE_SCENE_XL
; @label VSPRITE.SCENE_XH
; @kind u8
; @symbol VSPRITE_SCENE_XH
; @summary High byte of the graphics-plane dirty rectangle X coordinate for atomic scene commits.
.global VSPRITE_SCENE_XH
; @label VSPRITE.SCENE_Y
; @kind u8
; @symbol VSPRITE_SCENE_Y
; @summary Graphics-plane dirty rectangle Y coordinate for atomic scene commits.
.global VSPRITE_SCENE_Y
; @label VSPRITE.SCENE_WIDTHL
; @kind u8
; @symbol VSPRITE_SCENE_WIDTHL
; @summary Low byte of the dirty rectangle width for atomic scene commits.
.global VSPRITE_SCENE_WIDTHL
; @label VSPRITE.SCENE_WIDTHH
; @kind u8
; @symbol VSPRITE_SCENE_WIDTHH
; @summary High byte of the dirty rectangle width for atomic scene commits.
.global VSPRITE_SCENE_WIDTHH
; @label VSPRITE.SCENE_HEIGHTL
; @kind u8
; @symbol VSPRITE_SCENE_HEIGHTL
; @summary Low byte of the dirty rectangle height for atomic scene commits.
.global VSPRITE_SCENE_HEIGHTL
; @label VSPRITE.SCENE_HEIGHTH
; @kind u8
; @symbol VSPRITE_SCENE_HEIGHTH
; @summary High byte of the dirty rectangle height for atomic scene commits.
.global VSPRITE_SCENE_HEIGHTH
; @label VSPRITE.SCENE_WORKSPACE
; @kind u8
; @symbol VSPRITE_SCENE_WORKSPACE
; @summary Blitter memory space for the atomic scene work buffer.
.global VSPRITE_SCENE_WORKSPACE
; @label VSPRITE.SCENE_WORKADDRL
; @kind u8
; @symbol VSPRITE_SCENE_WORKADDRL
; @summary Low byte of the atomic scene work buffer address.
.global VSPRITE_SCENE_WORKADDRL
; @label VSPRITE.SCENE_WORKADDRM
; @kind u8
; @symbol VSPRITE_SCENE_WORKADDRM
; @summary Middle byte of the atomic scene work buffer address.
.global VSPRITE_SCENE_WORKADDRM
; @label VSPRITE.SCENE_WORKADDRH
; @kind u8
; @symbol VSPRITE_SCENE_WORKADDRH
; @summary High byte of the atomic scene work buffer address.
.global VSPRITE_SCENE_WORKADDRH
; @label VSPRITE.SCENE_WORKSTRL
; @kind u8
; @symbol VSPRITE_SCENE_WORKSTRL
; @summary Low byte of the atomic scene work buffer row stride.
.global VSPRITE_SCENE_WORKSTRL
; @label VSPRITE.SCENE_WORKSTRH
; @kind u8
; @symbol VSPRITE_SCENE_WORKSTRH
; @summary High byte of the atomic scene work buffer row stride.
.global VSPRITE_SCENE_WORKSTRH

; Public helper routines.
.global vsprite_blit
.global vsprite_fill
.global vsprite_gfx_addr
.global vsprite_gfx_blit
.global vsprite_gfx_rotate_blit
.global vsprite_gfx_fill
.global vsprite_gfx_save_bg
.global vsprite_gfx_restore_bg
.global vsprite_rotate
.global vsprite_use_original
.global vsprite_use_rotated
.global vsprite_scene_begin
.global vsprite_scene_commit
.global vsprite_scene_commit_atomic
.global vsprite_scene_draw

.endif

; graphics.s — GRAPHICS module (MODULE_ID_GRAPHICS = $01), the canonical paged
; graphics library. Built once, staged to XRAM $060000, paged into bank 1 on the
; first lib_call. Header "NL"/$01 at $C000 (lib_module_header); RTS-trick dispatch
; on LIB_FN_ID; thin leaf wrappers over the shared NDK driver bodies (vgc.s).
;
; This skeleton (Stage 4b.1) implements one fn — GFN_GCLS — proven end-to-end on
; both test axes (loader path + hardware VGC effect). Remaining domains land in
; 4b.2..4b.11; their jtable slots resolve to gfn_unimpl until then.

      .include "libabi.inc"
      .include "libmod.inc"
      .include "libgraphics.inc"
      .include "vgc.inc"               ; nova.inc -> NVR0L/NVR0H ZP scratch + VGC_* regs
                                       ; (guarded; re-included by vgc.s below)
      .include "vsprite.inc"           ; VSPRITE_* BSS symbols + helper .globals
                                       ; (guarded; vsprite.s body included below)
      .include "msprite.inc"           ; MSPRITE_* BSS symbols + helper .globals
                                       ; (guarded; msprite.s body included below;
                                       ; pulls sprite.inc, dedup'd)
      .include "nvg.inc"               ; NVG_ADDRL/H + NVG_NAMEPTR/NAMELEN; pulls
                                       ; fio.inc (FIO_GSPACE/GADDRL/H/GLENL/H,
                                       ; FIO_NAME/NAMELEN, FIO_RESULT_OK) + nova.inc
                                       ; (all guarded; nvg.s + fio.s bodies below)
      .include "anim.inc"              ; ANIM_* BSS symbols + ANIM_RESULT_*/
                                       ; ANIM_INVALID_HANDLE + helper .globals;
                                       ; pulls dma/msprite/pager/xram.inc (guarded;
                                       ; anim.s body included below)
      .include "tween.inc"             ; TWEEN_* BSS symbols + TWEEN_MODE_* consts
                                       ; + helper .globals (guarded; tween.s below)

; Highest implemented fn-id + 1. Grows per domain batch. The draw domain $00-$09
; is live; the text/mode domain $10-$1B is live (batch 4b.3); the hw-sprite domain
; $20-$3B is live (batch 4b.4); the copper domain $40-$49 is live (batch 4b.5);
; the blit/dma domain $50-$5B is live (batch 4b.6); the vsprite domain $60-$71 is
; live (batch 4b.7); the msprite/meta-sprite domain $80-$8B is live (batch 4b.8);
; the image/mem domain $A0-$A9 is live (batch 4b.9); the anim domain $C0-$C7 and
; the tween domain $D0-$DA are live (batch 4b.11). The $B0-$B2 turtle-render fns
; (INIT/DRAW/ERASE) are live (batch 4c.2-2); $B3 TURTLE_OP (the turtle command
; engine) is live (batch 4c.2-3); $B4-$BF stay reserved.
; The jtable is dense from $00..GFX_FN_COUNT-1: implemented ids point at their
; wrapper, every gap id ($0A-$0F, $19 CLSWIN, $1C-$1F, $2E-$2F, $3C-$3F, $4A-$4F,
; $5C-$5F, $72-$7F, $8C-$9F, $AA-$AF, $B4-$BF, $C8-$CF, $DB) points at gfn_unimpl so it
; resolves to LERR_NO_FN. ids >= GFX_FN_COUNT ($DB+) resolve to LERR_NO_FN via the
; dispatch bounds-check.
GFX_FN_COUNT = $DB

; GTEXT copies its BYTES string into the VGC FIO_NAME buffer ($B9B0-$B9EF, 64
; bytes). Mirror fio.inc's FIO_NAME_LIMIT here (fio.inc isn't pulled into the
; module) to clamp the copy length to the buffer.
GFX_FIO_NAME_LIMIT = $3F

      .segment "CODE"
      lib_module_header MODULE_ID_GRAPHICS, LIB_ABI_VERSION, GFX_FN_COUNT

; ===========================================================================
; NDOC documentation (extracted by tools/nmod_pack.py into graphics.nmod).
; Function ids resolve symbolically from runtime/asm/libgraphics.inc. Every
; implemented (non-gfn_unimpl) GFN is documented here.
; ===========================================================================
;@module GRAPHICS
;@version 1.0
;@brief Nova virtual graphics controller: drawing, text, sprites, copper, blitter, vsprites, meta-sprites, images, animation, tweening, turtle.
;
;@fn GFN_GCLS
;@brief Clear the graphics plane to the background colour.
;@ret void
;
;@fn GFN_GCOLOR
;@brief Set the current draw colour for subsequent primitives.
;@arg color u8 palette index (0-15)
;@ret void
;@effect Persists as the pen colour used by later primitives.
;
;@fn GFN_PLOT
;@brief Plot a single pixel in the current colour.
;@arg x s16 x coordinate
;@arg y s16 y coordinate
;@ret void
;
;@fn GFN_UNPLOT
;@brief Clear a single pixel to the background colour.
;@arg x s16 x coordinate
;@arg y s16 y coordinate
;@ret void
;
;@fn GFN_LINE
;@brief Draw a line in the current colour.
;@arg x0 s16 start x
;@arg y0 s16 start y
;@arg x1 s16 end x
;@arg y1 s16 end y
;@ret void
;
;@fn GFN_CIRCLE
;@brief Draw a circle (or ellipse) outline in the current colour.
;@arg cx s16 centre x
;@arg cy s16 centre y
;@arg rx s16 x radius
;@arg ry s16 y radius (0 = circle, ry = rx)
;@ret void
;
;@fn GFN_RECT
;@brief Draw a rectangle outline in the current colour.
;@arg x0 s16 left
;@arg y0 s16 top
;@arg x1 s16 right
;@arg y1 s16 bottom
;@ret void
;
;@fn GFN_FILL
;@brief Draw a filled rectangle in the current colour.
;@arg x0 s16 left
;@arg y0 s16 top
;@arg x1 s16 right
;@arg y1 s16 bottom
;@ret void
;
;@fn GFN_PAINT
;@brief Flood-fill the connected region at (x,y) with the current colour.
;@arg x s16 seed x
;@arg y s16 seed y
;@ret void
;
;@fn GFN_GTEXT
;@brief Draw a text string at a pixel position in the current colour.
;@arg x s16 x coordinate
;@arg y s16 y coordinate
;@arg str bytes pointer+length of the string (copied via the VGC FIO_NAME buffer)
;@ret void
;
;@fn GFN_COLOR
;@brief Set the text foreground, background, and border colours.
;@arg fg u8 foreground palette index
;@arg bg u8 background palette index
;@arg border u8 border palette index
;@ret void
;
;@fn GFN_FONT
;@brief Select the active text font slot.
;@arg slot u8 font slot index
;@ret void
;
;@fn GFN_MODE
;@brief Set the graphics/text display mode.
;@arg mode u8 VGC mode value
;@ret void
;
;@fn GFN_REVERSE
;@brief Enable reverse-video text using the current fg/bg.
;@ret void
;
;@fn GFN_REVERSEOFF
;@brief Disable reverse-video text.
;@ret void
;
;@fn GFN_FLASH
;@brief Enable flashing text.
;@ret void
;
;@fn GFN_FLASHOFF
;@brief Disable flashing text.
;@ret void
;
;@fn GFN_LOCATE
;@brief Move the text cursor to a column/row.
;@arg col u8 cursor column
;@arg row u8 cursor row
;@ret void
;
;@fn GFN_CLS
;@brief Clear the text screen.
;@ret void
;
;@fn GFN_DISPLAYON
;@brief Restore full display brightness (VGC_DIMMER = $0F).
;@ret void
;
;@fn GFN_DISPLAYOFF
;@brief Blank the display output while keeping timing (VGC_DIMMER = $00).
;@ret void
;
;@fn GFN_SPR_DEFINE
;@brief Set one shape pixel of a sprite to a colour.
;@arg idx u8 sprite index (0-15)
;@arg x u8 pixel x within the 16x16 shape (0-15)
;@arg y u8 pixel y within the 16x16 shape (0-15)
;@arg color u8 palette index
;@ret void
;
;@fn GFN_SPR_ROW
;@brief Write one 16-pixel sprite shape row (8 nibble-packed bytes).
;@arg idx u8 sprite index
;@arg row u8 shape row (0-15)
;@arg bytes0_3 u32 first 4 packed shape bytes
;@arg bytes4_7 u32 last 4 packed shape bytes
;@ret void
;
;@fn GFN_SPR_CLEAR
;@brief Clear a sprite's shape data.
;@arg idx u8 sprite index
;@ret void
;
;@fn GFN_SPR_COPY
;@brief Copy shape data from one sprite to another.
;@arg src u8 source sprite index
;@arg dst u8 destination sprite index
;@ret void
;
;@fn GFN_SPR_POS
;@brief Move a sprite to a position (16-bit signed X, byte Y).
;@arg idx u8 sprite index
;@arg x s16 sprite x position
;@arg y u8 sprite y position
;@ret void
;
;@fn GFN_SPR_ENABLE
;@brief Enable (show) a sprite.
;@arg idx u8 sprite index
;@ret void
;
;@fn GFN_SPR_DISABLE
;@brief Disable (hide) a sprite.
;@arg idx u8 sprite index
;@ret void
;
;@fn GFN_SPR_FLIP
;@brief Set a sprite's horizontal/vertical flip flags.
;@arg idx u8 sprite index
;@arg flags u8 flip flags
;@ret void
;
;@fn GFN_SPR_PRIORITY
;@brief Set a sprite's render priority.
;@arg idx u8 sprite index
;@arg pri u8 priority level
;@ret void
;
;@fn GFN_SPR_SHAPE
;@brief Set a sprite's shape-slot index (attribute register write).
;@arg idx u8 sprite index
;@arg shape u8 shape slot index
;@ret void
;@effect Writes the VGC_SPR_SHAPE attribute register directly (no command issued).
;
;@fn GFN_SPR_SETREG
;@brief Write one 8-bit sprite attribute field.
;@arg idx u8 sprite index
;@arg field u8 attribute field offset
;@arg val u8 value
;@ret void
;@effect Writes the $A040 sprite attribute block directly (no command issued).
;
;@fn GFN_SPR_SETREG16
;@brief Write one 16-bit sprite attribute field.
;@arg idx u8 sprite index
;@arg field u8 attribute field offset
;@arg val16 u16 16-bit value
;@ret void
;@effect Writes the $A040 sprite attribute block directly (no command issued).
;
;@fn GFN_SPR_GETX
;@brief Read a sprite's 16-bit X position (reporter).
;@arg idx u8 sprite index
;@ret u16 sprite X position
;@effect Returns the X position in RESULT.
;
;@fn GFN_SPR_GETY
;@brief Read a sprite's Y byte (reporter).
;@arg idx u8 sprite index
;@ret u8 sprite Y position
;@effect Returns the Y byte in RESULT.
;
;@fn GFN_SPR_COLL_STATUS
;@brief Read the sprite-sprite collision status low byte (reporter).
;@ret u8 collision status byte
;@effect Returns the status byte in RESULT.
;
;@fn GFN_SPR_COLL_MASK
;@brief Read the 16-bit sprite-sprite collision mask (reporter).
;@ret u16 collision mask (one bit per sprite)
;@effect Returns the mask in RESULT.
;
;@fn GFN_SPR_COLL_CLEAR
;@brief Clear the sprite-sprite collision mask and acknowledge its IRQ.
;@ret void
;
;@fn GFN_SPR_COLL_READCLEAR
;@brief Read the 16-bit sprite-sprite collision mask, then clear it (reporter).
;@ret u16 collision mask before clearing
;@effect Returns the mask in RESULT and clears it.
;
;@fn GFN_SPR_COLL_IRQON
;@brief Enable the sprite-sprite collision IRQ (and CLI).
;@ret void
;
;@fn GFN_SPR_COLL_IRQOFF
;@brief Disable the sprite-sprite collision IRQ.
;@ret void
;
;@fn GFN_SPR_BG_STATUS
;@brief Read the sprite-background collision status low byte (reporter).
;@ret u8 collision status byte
;@effect Returns the status byte in RESULT.
;
;@fn GFN_SPR_BG_MASK
;@brief Read the 16-bit sprite-background collision mask (reporter).
;@ret u16 collision mask (one bit per sprite)
;@effect Returns the mask in RESULT.
;
;@fn GFN_SPR_BG_CLEAR
;@brief Clear the sprite-background collision mask and acknowledge its IRQ.
;@ret void
;
;@fn GFN_SPR_BG_READCLEAR
;@brief Read the 16-bit sprite-background collision mask, then clear it (reporter).
;@ret u16 collision mask before clearing
;@effect Returns the mask in RESULT and clears it.
;
;@fn GFN_SPR_BG_IRQON
;@brief Enable the sprite-background collision IRQ (and CLI).
;@ret void
;
;@fn GFN_SPR_BG_IRQOFF
;@brief Disable the sprite-background collision IRQ.
;@ret void
;
;@fn GFN_COPPER_LIST
;@brief Select the copper list that ADD/CLEAR will edit.
;@arg idx u8 copper list index (0-127)
;@ret void
;@effect Sets state read by later COPPER_ADD/COPPER_CLEAR.
;
;@fn GFN_COPPER_ADD
;@brief Add a register-write event to the target copper list.
;@arg x s16 column (raster x position)
;@arg y u8 scanline (raster y position)
;@arg reg u16 VGC register specifier
;@arg val u8 value to write
;@ret void
;
;@fn GFN_COPPER_CLEAR
;@brief Empty the target copper list.
;@ret void
;
;@fn GFN_COPPER_ON
;@brief Enable copper execution.
;@ret void
;
;@fn GFN_COPPER_OFF
;@brief Disable copper execution.
;@ret void
;
;@fn GFN_COPPER_USE
;@brief Make a copper list active at the next vblank.
;@arg idx u8 copper list index
;@ret void
;
;@fn GFN_COPPER_END
;@brief Finish editing the current list (target becomes the active list).
;@ret void
;
;@fn GFN_COPPER_SPLIT
;@brief Build a split-screen copper list with two VGC_MODE rules at a scanline.
;@arg idx u8 copper list index
;@arg splitY u8 scanline of the mode split
;@arg mode0 u8 VGC mode above the split
;@arg mode1 u8 VGC mode below the split
;@ret void
;@effect Runs the full off/list/clear/add/add/use/on sequence and leaves the copper enabled.
;
;@fn GFN_COPPER_SET_REG
;@brief Add a copper write to a direct VGC register (observable CopperEvent).
;@arg x s16 column (raster x position)
;@arg y u8 scanline (raster y position)
;@arg regIndex u8 VGC register index
;@arg val u8 value to write
;@ret void
;
;@fn GFN_COPPER_SET_SPRITE_REG
;@brief Add a copper write to a sprite attribute register (observable CopperEvent).
;@arg x s16 column (raster x position)
;@arg y u8 scanline (raster y position)
;@arg sprfield u8 byte0 = sprite index, byte1 = VGC_SPR_*_OFF field
;@arg val u8 value to write
;@ret void
;
;@fn GFN_BLITCOPY
;@brief 2-D rectangular copy between memory spaces (tightly packed, stride = width).
;@arg spaces u8 byte0 = srcSpace, byte1 = dstSpace
;@arg src u32 source offset (24-bit)
;@arg dst u32 destination offset (24-bit)
;@arg wh u32 width (bytes 0-1) and height (bytes 2-3)
;@ret void
;@effect Self-waits for the blit to finish before returning.
;
;@fn GFN_BLITFILL
;@brief Fill a tightly-packed rectangle with a byte value.
;@arg spacefill u8 byte1 = dstSpace, byte2 = fill value
;@arg dst u32 destination offset (24-bit)
;@arg wh u32 width (bytes 0-1) and height (bytes 2-3)
;@ret void
;@effect Self-waits for the blit to finish before returning.
;
;@fn GFN_BLIT_START
;@brief Issue a blit copy on caller-preloaded BLT_* registers, then wait.
;@ret void
;@effect Self-waits for the blit to finish before returning.
;
;@fn GFN_BLIT_WAIT
;@brief Poll BLT_STATUS to completion for a caller-issued blit.
;@ret void
;@effect Self-waits for the blit to finish before returning.
;
;@fn GFN_DMACOPY
;@brief 1-D bulk copy between memory spaces.
;@arg spaces u8 byte0 = srcSpace, byte1 = dstSpace
;@arg src u32 source offset (24-bit)
;@arg dst u32 destination offset (24-bit)
;@arg len u32 length in bytes (24-bit)
;@ret void
;@effect Self-waits for the DMA to finish before returning.
;
;@fn GFN_DMAFILL
;@brief Fill a DMA range with a byte value.
;@arg spacefill u8 byte1 = dstSpace, byte2 = fill value
;@arg dst u32 destination offset (24-bit)
;@arg len u32 length in bytes (24-bit)
;@ret void
;@effect Self-waits for the DMA to finish before returning.
;
;@fn GFN_BLIT_STATUS
;@brief Read the blitter controller status byte (reporter).
;@ret u8 blitter status (BUSY/OK/ERROR)
;@effect Returns the status byte in RESULT.
;
;@fn GFN_BLIT_ERR
;@brief Read the blitter controller error byte (reporter).
;@ret u8 blitter error code (0 = none)
;@effect Returns the error byte in RESULT.
;
;@fn GFN_BLIT_COUNT
;@brief Read the 24-bit blitter bytes-written counter (reporter).
;@ret u32 bytes written (24-bit)
;@effect Returns the count in RESULT.
;
;@fn GFN_DMA_STATUS
;@brief Read the DMA controller status byte (reporter).
;@ret u8 DMA status (BUSY/OK/ERROR)
;@effect Returns the status byte in RESULT.
;
;@fn GFN_DMA_ERR
;@brief Read the DMA controller error byte (reporter).
;@ret u8 DMA error code (0 = none)
;@effect Returns the error byte in RESULT.
;
;@fn GFN_DMA_COUNT
;@brief Read the 24-bit DMA bytes-moved counter (reporter).
;@ret u32 bytes moved (24-bit)
;@effect Returns the count in RESULT.
;
;@fn GFN_VS_BLIT
;@brief Virtual-sprite rectangular copy (self-waits).
;@arg cfg bytes pointer+length of the VSPRITE config struct
;@ret void
;@effect Self-waits for the blit to finish before returning.
;@status LERR_OK, LERR_VSPRITE_FAIL
;
;@fn GFN_VS_BLIT_START
;@brief Virtual-sprite rectangular copy, issue only (no wait).
;@arg cfg bytes pointer+length of the VSPRITE config struct
;@ret void
;@status LERR_OK, LERR_VSPRITE_FAIL
;
;@fn GFN_VS_FILL
;@brief Virtual-sprite rectangular fill (self-waits).
;@arg cfg bytes pointer+length of the VSPRITE config struct
;@ret void
;@effect Self-waits for the fill to finish before returning.
;@status LERR_OK, LERR_VSPRITE_FAIL
;
;@fn GFN_VS_USE_ORIGINAL
;@brief Point the vsprite source at its original (unrotated) buffer.
;@arg cfg bytes pointer+length of the VSPRITE config struct
;@ret void
;@effect Sets SRC* state read by a later blit.
;@status LERR_OK, LERR_VSPRITE_FAIL
;
;@fn GFN_VS_USE_ROTATED
;@brief Point the vsprite source at its rotated buffer.
;@arg cfg bytes pointer+length of the VSPRITE config struct
;@ret void
;@effect Sets SRC* state read by a later blit.
;@status LERR_OK, LERR_VSPRITE_FAIL
;
;@fn GFN_VS_ROTATE
;@brief Rotate the vsprite original buffer into its rotated buffer (self-waits).
;@arg cfg bytes pointer+length of the VSPRITE config struct
;@ret void
;@effect Self-waits for the blit to finish before returning.
;@status LERR_OK, LERR_VSPRITE_FAIL
;
;@fn GFN_VS_GFX_BLIT
;@brief Blit the vsprite onto the gfx plane at its X/Y (self-waits).
;@arg cfg bytes pointer+length of the VSPRITE config struct
;@ret void
;@effect Self-waits for the blit to finish before returning.
;@status LERR_OK, LERR_VSPRITE_FAIL
;
;@fn GFN_VS_GFX_BLIT_START
;@brief Blit the vsprite onto the gfx plane, issue only (no wait).
;@arg cfg bytes pointer+length of the VSPRITE config struct
;@ret void
;@status LERR_OK, LERR_VSPRITE_FAIL
;
;@fn GFN_VS_GFX_FILL
;@brief Fill a region of the gfx plane at the vsprite X/Y (self-waits).
;@arg cfg bytes pointer+length of the VSPRITE config struct
;@ret void
;@effect Self-waits for the fill to finish before returning.
;@status LERR_OK, LERR_VSPRITE_FAIL
;
;@fn GFN_VS_GFX_SAVE_BG
;@brief Save the gfx-plane background under the vsprite into its BG buffer (self-waits).
;@arg cfg bytes pointer+length of the VSPRITE config struct
;@ret void
;@effect Self-waits for the blit to finish before returning.
;@status LERR_OK, LERR_VSPRITE_FAIL
;
;@fn GFN_VS_GFX_RESTORE_BG
;@brief Restore the saved background back onto the gfx plane (self-waits).
;@arg cfg bytes pointer+length of the VSPRITE config struct
;@ret void
;@effect Self-waits for the blit to finish before returning.
;@status LERR_OK, LERR_VSPRITE_FAIL
;
;@fn GFN_VS_GFX_ROTATE_BLIT
;@brief Rotate then frame-wait then blit the vsprite onto the gfx plane.
;@arg cfg bytes pointer+length of the VSPRITE config struct
;@ret void
;@effect PUMPED: frame-waits on VGC_FRAME — the caller must pump the bus.
;@status LERR_OK, LERR_VSPRITE_FAIL
;
;@fn GFN_VS_GFX_ROTATE_BLIT_KEYED
;@brief Rotate, frame-wait, then colour-keyed blit the vsprite onto the gfx plane.
;@arg cfg bytes pointer+length of the VSPRITE config struct
;@ret void
;@effect PUMPED: frame-waits on VGC_FRAME — the caller must pump the bus.
;@status LERR_OK, LERR_VSPRITE_FAIL
;
;@fn GFN_VS_GFX_ROTATE_BLIT_NOWAIT
;@brief Rotate then blit the vsprite onto the gfx plane with no frame wait.
;@arg cfg bytes pointer+length of the VSPRITE config struct
;@ret void
;@status LERR_OK, LERR_VSPRITE_FAIL
;
;@fn GFN_VS_SCENE_BEGIN
;@brief Begin a vsprite scene: restore old backgrounds for the descriptor list.
;@arg cfg bytes pointer+length of the VSPRITE config struct (incl scene table)
;@ret void
;@effect Self-waits for the blits to finish before returning.
;@status LERR_OK, LERR_VSPRITE_FAIL
;
;@fn GFN_VS_SCENE_COMMIT
;@brief Commit a vsprite scene: save then draw all descriptors.
;@arg cfg bytes pointer+length of the VSPRITE config struct (incl scene table)
;@ret void
;@effect Self-waits for the blits to finish before returning.
;@status LERR_OK, LERR_VSPRITE_FAIL
;
;@fn GFN_VS_SCENE_DRAW
;@brief Draw all vsprite scene descriptors (draw only, no save).
;@arg cfg bytes pointer+length of the VSPRITE config struct (incl scene table)
;@ret void
;@effect Self-waits for the blits to finish before returning.
;@status LERR_OK, LERR_VSPRITE_FAIL
;
;@fn GFN_VS_SCENE_COMMIT_ATOMIC
;@brief Atomically commit a vsprite scene via an off-screen work buffer.
;@arg cfg bytes pointer+length of the VSPRITE config struct (incl scene+work)
;@ret void
;@effect Self-waits for the blits to finish before returning.
;@status LERR_OK, LERR_VSPRITE_FAIL
;
;@fn GFN_MS_SPAWN
;@brief Spawn a meta-sprite object from a visual descriptor.
;@arg desc bytes pointer+length of the meta-sprite descriptor (ptr written to MSPRITE_DESC)
;@ret u8 new object handle ($FF on failure)
;@effect Returns the object handle in RESULT.
;@status LERR_OK, LERR_MSPRITE_FAIL
;
;@fn GFN_MS_DESTROY
;@brief Destroy a meta-sprite object and free its hardware sprites.
;@arg handle u8 meta-sprite object handle
;@ret void
;@status LERR_OK, LERR_MSPRITE_FAIL
;
;@fn GFN_MS_SHOW
;@brief Show a meta-sprite object.
;@arg handle u8 meta-sprite object handle
;@ret void
;@status LERR_OK, LERR_MSPRITE_FAIL
;
;@fn GFN_MS_HIDE
;@brief Hide a meta-sprite object.
;@arg handle u8 meta-sprite object handle
;@ret void
;@status LERR_OK, LERR_MSPRITE_FAIL
;
;@fn GFN_MS_POS
;@brief Set a meta-sprite object's position (16-bit signed X, byte Y).
;@arg handle u8 meta-sprite object handle
;@arg x s16 object x position
;@arg y u8 object y position
;@ret void
;@status LERR_OK, LERR_MSPRITE_FAIL
;
;@fn GFN_MS_FRAME
;@brief Set a meta-sprite object's animation frame.
;@arg handle u8 meta-sprite object handle
;@arg frame u8 frame number
;@ret void
;@status LERR_OK, LERR_MSPRITE_FAIL
;
;@fn GFN_MS_ANIM
;@brief Attach an animation descriptor to a meta-sprite object.
;@arg handle u8 meta-sprite object handle
;@arg anim bytes pointer+length of the animation descriptor (ptr to MSPRITE_ANIM)
;@ret void
;@status LERR_OK, LERR_MSPRITE_FAIL
;
;@fn GFN_MS_PRIORITY
;@brief Set a meta-sprite object's render priority.
;@arg handle u8 meta-sprite object handle
;@arg pri u8 priority level
;@ret void
;@status LERR_OK, LERR_MSPRITE_FAIL
;
;@fn GFN_MS_TRANSCOLOR
;@brief Set a meta-sprite object's transparent colour.
;@arg handle u8 meta-sprite object handle
;@arg color u8 transparent palette index
;@ret void
;@status LERR_OK, LERR_MSPRITE_FAIL
;
;@fn GFN_MS_TICK
;@brief Advance all meta-sprite animation timers by one tick (engine-wide).
;@ret void
;@status LERR_OK
;
;@fn GFN_MS_COMMIT
;@brief Write all dirty visible meta-sprite objects to the hardware sprites.
;@ret void
;@status LERR_OK
;
;@fn GFN_MS_COMMIT_ONE
;@brief Write one meta-sprite object to its hardware sprites.
;@arg handle u8 meta-sprite object handle
;@ret void
;@status LERR_OK, LERR_MSPRITE_FAIL
;
;@fn GFN_MEMREAD
;@brief Read one byte from a VGC memory space (reporter).
;@arg space u8 VGC plane / space id
;@arg addr u16 16-bit address within the space
;@ret u8 byte read
;@effect Returns the byte in RESULT.
;
;@fn GFN_MEMWRITE
;@brief Write one byte to a VGC memory space.
;@arg space u8 VGC plane / space id
;@arg addr u16 16-bit address within the space
;@arg value u8 byte to write
;@ret void
;
;@fn GFN_VPEEK
;@brief Read one byte from the gfx plane (reporter).
;@arg offset u16 gfx-plane offset
;@ret u8 gfx byte
;@effect Returns the byte in RESULT.
;
;@fn GFN_VPOKE
;@brief Write one byte to the gfx plane.
;@arg offset u16 gfx-plane offset
;@arg value u8 byte to write
;@ret void
;
;@fn GFN_GSAVE
;@brief Save a VGC memory space to a .gfx file on disk.
;@arg space u8 VGC plane / space id
;@arg addr u16 16-bit start address
;@arg len u16 length in bytes
;@arg name bytes pointer+length of the filename
;@ret void
;@status LERR_OK, LERR_FILE_FAIL
;
;@fn GFN_GLOAD
;@brief Load a .gfx file from disk into a VGC memory space.
;@arg space u8 VGC plane / space id
;@arg addr u16 16-bit destination address
;@arg len u16 length in bytes
;@arg name bytes pointer+length of the filename
;@ret void
;@status LERR_OK, LERR_FILE_FAIL
;
;@fn GFN_NVGLOAD
;@brief Load an NVG image into the gfx plane at offset 0.
;@arg name bytes pointer+length of the filename
;@ret void
;@status LERR_OK, LERR_IMAGE_FAIL
;
;@fn GFN_NVGLOAD_AT
;@brief Load an NVG image into the gfx plane at a destination offset.
;@arg name bytes pointer+length of the filename
;@arg dest u16 gfx-plane destination offset
;@ret void
;@status LERR_OK, LERR_IMAGE_FAIL
;
;@fn GFN_NVGLOAD_NAMED
;@brief Load an NVG image into the gfx plane via the named-arg filename path.
;@arg name bytes pointer+length of the filename
;@ret void
;@status LERR_OK, LERR_IMAGE_FAIL
;
;@fn GFN_NVGLOAD_NAMED_AT
;@brief Load an NVG image (named-arg path) into the gfx plane at a destination offset.
;@arg name bytes pointer+length of the filename
;@arg dest u16 gfx-plane destination offset
;@ret void
;@status LERR_OK, LERR_IMAGE_FAIL
;
;@fn GFN_TURTLE_INIT
;@brief Install the built-in turtle icon into the module source buffer and reset bg bookkeeping.
;@ret void
;
;@fn GFN_TURTLE_DRAW
;@brief Erase the old turtle, save the background, then rotate-and-keyed-blit the turtle at a new centre.
;@arg x s16 turtle centre x
;@arg y s16 turtle centre y
;@arg angle u8 pre-computed rotation angle byte
;@ret void
;@effect PUMPED: frame-waits inside the rotate-blit — the caller must pump the bus.
;
;@fn GFN_TURTLE_ERASE
;@brief Restore the saved background, erasing the turtle.
;@ret void
;
;@fn GFN_TURTLE_OP
;@brief NovaLogo turtle command engine (op-dispatched: ARG2 byte0 = a turtle command id; value args in ARG0/ARG1 as Logo 16.8).
;@arg op u8 turtle command id (TOP_*)
;@arg arg0 fix16.8 first value argument (Logo 16.8 fixed-point)
;@arg arg1 fix16.8 second value argument (Logo 16.8 fixed-point)
;@ret fix16.8 reporter result for XCOR/YCOR/HEADING/PENDOWN?/SHOWN?/TOWARDS (void for command ops)
;@effect PUMPED: turtle DRAW move ops frame-wait — pump the bus.
;
;@fn GFN_ANIM_INIT
;@brief Initialise the sprite-animation engine (clear all tracks).
;@ret void
;@status LERR_OK
;
;@fn GFN_ANIM_START
;@brief Start an animation track from a descriptor against a target.
;@arg desc bytes pointer+length of the animation descriptor (ptr to ANIM_DESC)
;@arg target u8 target index
;@arg type u8 target type
;@ret u8 new track handle ($FF on failure)
;@effect Returns the track handle in RESULT.
;@status LERR_OK, LERR_ANIM_FAIL
;
;@fn GFN_ANIM_STOP
;@brief Stop an animation track.
;@arg handle u8 animation track handle
;@ret void
;@status LERR_OK, LERR_ANIM_FAIL
;
;@fn GFN_ANIM_TICK
;@brief Advance all animation tracks by one tick (engine-wide).
;@ret void
;@status LERR_OK
;
;@fn GFN_ANIM_TICK_ONE
;@brief Advance one animation track by one tick.
;@arg handle u8 animation track handle
;@ret void
;@status LERR_OK, LERR_ANIM_FAIL
;
;@fn GFN_ANIM_SET_FRAME
;@brief Set an animation track's current frame.
;@arg handle u8 animation track handle
;@arg frame u8 frame number
;@ret void
;@status LERR_OK, LERR_ANIM_FAIL
;
;@fn GFN_ANIM_LOAD_XRAM
;@brief Load animation shape frames from XRAM.
;@arg shape u8 shape slot
;@arg count u8 number of frames
;@arg xaddr24 u32 24-bit XRAM source address
;@ret void
;@status LERR_OK, LERR_ANIM_FAIL
;
;@fn GFN_ANIM_LOAD_DISK
;@brief Load animation shape frames from a disk file.
;@arg shape u8 shape slot
;@arg count u8 number of frames
;@arg file24 u32 24-bit file offset
;@arg name bytes pointer+length of the filename
;@ret void
;@status LERR_OK, LERR_ANIM_FAIL
;
;@fn GFN_TWEEN_BEGIN
;@brief Begin a tween between two values over a duration (reporter).
;@arg start s16 start value
;@arg end s16 end value
;@arg duration u8 frames (0 snaps to end)
;@ret u16 computed value (RESULT also carries done + progress bytes)
;@status LERR_OK
;
;@fn GFN_TWEEN_EVAL
;@brief Evaluate a tween at a frame with a selectable easing mode (reporter).
;@arg start s16 start value
;@arg end s16 end value
;@arg durmode u16 byte0 = duration, byte1 = ease mode (0 lin/1 in/2 out/3 in-out)
;@arg frame u8 current frame
;@ret u16 computed value (RESULT also carries done + progress bytes)
;@status LERR_OK
;
;@fn GFN_TWEEN_EVAL_LINEAR
;@brief Evaluate a linear tween at a frame (reporter).
;@arg start s16 start value
;@arg end s16 end value
;@arg duration u8 frames
;@arg frame u8 current frame
;@ret u16 computed value (RESULT also carries done + progress bytes)
;@status LERR_OK
;
;@fn GFN_TWEEN_EVAL_EASE_IN
;@brief Evaluate an ease-in tween at a frame (reporter).
;@arg start s16 start value
;@arg end s16 end value
;@arg duration u8 frames
;@arg frame u8 current frame
;@ret u16 computed value (RESULT also carries done + progress bytes)
;@status LERR_OK
;
;@fn GFN_TWEEN_EVAL_EASE_OUT
;@brief Evaluate an ease-out tween at a frame (reporter).
;@arg start s16 start value
;@arg end s16 end value
;@arg duration u8 frames
;@arg frame u8 current frame
;@ret u16 computed value (RESULT also carries done + progress bytes)
;@status LERR_OK
;
;@fn GFN_TWEEN_EVAL_EASE_IN_OUT
;@brief Evaluate an ease-in-out tween at a frame (reporter).
;@arg start s16 start value
;@arg end s16 end value
;@arg duration u8 frames
;@arg frame u8 current frame
;@ret u16 computed value (RESULT also carries done + progress bytes)
;@status LERR_OK
;
;@fn GFN_TWEEN_STEP
;@brief Step a tween forward one frame with a selectable easing mode (reporter).
;@arg start s16 start value
;@arg end s16 end value
;@arg durmode u16 byte0 = duration, byte1 = ease mode (0 lin/1 in/2 out/3 in-out)
;@arg frame u8 current frame (incremented by the step)
;@ret u16 computed value (RESULT also carries done + progress bytes)
;@status LERR_OK
;
;@fn GFN_TWEEN_STEP_LINEAR
;@brief Step a linear tween forward one frame (reporter).
;@arg start s16 start value
;@arg end s16 end value
;@arg duration u8 frames
;@arg frame u8 current frame (incremented by the step)
;@ret u16 computed value (RESULT also carries done + progress bytes)
;@status LERR_OK
;
;@fn GFN_TWEEN_STEP_EASE_IN
;@brief Step an ease-in tween forward one frame (reporter).
;@arg start s16 start value
;@arg end s16 end value
;@arg duration u8 frames
;@arg frame u8 current frame (incremented by the step)
;@ret u16 computed value (RESULT also carries done + progress bytes)
;@status LERR_OK
;
;@fn GFN_TWEEN_STEP_EASE_OUT
;@brief Step an ease-out tween forward one frame (reporter).
;@arg start s16 start value
;@arg end s16 end value
;@arg duration u8 frames
;@arg frame u8 current frame (incremented by the step)
;@ret u16 computed value (RESULT also carries done + progress bytes)
;@status LERR_OK
;
;@fn GFN_TWEEN_STEP_EASE_IN_OUT
;@brief Step an ease-in-out tween forward one frame (reporter).
;@arg start s16 start value
;@arg end s16 end value
;@arg duration u8 frames
;@arg frame u8 current frame (incremented by the step)
;@ret u16 computed value (RESULT also carries done + progress bytes)
;@status LERR_OK

; dispatch — fn-id router. RTS-trick: push (target-1) hi/lo, RTS jumps to target.
;
; The jtable index is fn*2. With fn-ids now reaching $8B the index exceeds 255
; ($8B*2 = $116), so an 8-bit `asl; tax` would wrap and dispatch the wrong slot
; (this is why batches up to $71 worked but $80+ did not). Compute a full 16-bit
; table pointer in the module ZP window (LIB_ZP, free before any wrapper runs)
; and read the target via (zp),Y. Y walks 0/1 for the lo/hi target bytes.
dispatch:
      lda     LIB_FN_ID
      cmp     #GFX_FN_COUNT
      bcs     @bad
      ; ptr = gfx_jtable + fn*2  (16-bit: fn*2 can carry past 255)
      asl                              ; A = (fn*2) low byte, C = (fn*2) bit 8
      sta     LIB_ZP
      lda     #0
      rol                              ; A = (fn*2) high byte (the shifted-out carry)
      sta     LIB_ZP+1
      clc
      lda     LIB_ZP
      adc     #<gfx_jtable
      sta     LIB_ZP
      lda     LIB_ZP+1
      adc     #>gfx_jtable
      sta     LIB_ZP+1
      ldy     #1                       ; target hi byte
      lda     (LIB_ZP),y
      pha
      ldy     #0                       ; target lo byte
      lda     (LIB_ZP),y
      pha
      rts
@bad:
      lda     #LERR_NO_FN
      sta     LIB_STATUS
      rts

gfx_jtable:
      .word   gfn_gcls-1               ; $00 GCLS
      .word   gfn_gcolor-1             ; $01 GCOLOR
      .word   gfn_plot-1               ; $02 PLOT
      .word   gfn_unplot-1             ; $03 UNPLOT
      .word   gfn_line-1               ; $04 LINE
      .word   gfn_circle-1             ; $05 CIRCLE
      .word   gfn_rect-1               ; $06 RECT
      .word   gfn_fill-1               ; $07 FILL
      .word   gfn_paint-1              ; $08 PAINT
      .word   gfn_gtext-1              ; $09 GTEXT
      .word   gfn_unimpl-1             ; $0A gap -> LERR_NO_FN
      .word   gfn_unimpl-1             ; $0B gap
      .word   gfn_unimpl-1             ; $0C gap
      .word   gfn_unimpl-1             ; $0D gap
      .word   gfn_unimpl-1             ; $0E gap
      .word   gfn_unimpl-1             ; $0F gap
      .word   gfn_color-1              ; $10 COLOR
      .word   gfn_font-1               ; $11 FONT
      .word   gfn_mode-1               ; $12 MODE
      .word   gfn_reverse-1            ; $13 REVERSE
      .word   gfn_reverseoff-1         ; $14 REVERSEOFF
      .word   gfn_flash-1              ; $15 FLASH
      .word   gfn_flashoff-1           ; $16 FLASHOFF
      .word   gfn_locate-1             ; $17 LOCATE
      .word   gfn_cls-1                ; $18 CLS
      .word   gfn_unimpl-1             ; $19 CLSWIN -> LERR_NO_FN (no vgc driver entry)
      .word   gfn_displayon-1          ; $1A DISPLAYON
      .word   gfn_displayoff-1         ; $1B DISPLAYOFF
      .word   gfn_unimpl-1             ; $1C gap
      .word   gfn_unimpl-1             ; $1D gap
      .word   gfn_unimpl-1             ; $1E gap
      .word   gfn_unimpl-1             ; $1F gap
      .word   gfn_spr_define-1         ; $20 SPRDEFINE
      .word   gfn_spr_row-1            ; $21 SPRROW
      .word   gfn_spr_clear-1          ; $22 SPRCLEAR
      .word   gfn_spr_copy-1           ; $23 SPRCOPY
      .word   gfn_spr_pos-1            ; $24 SPRPOS
      .word   gfn_spr_enable-1         ; $25 SPRENABLE
      .word   gfn_spr_disable-1        ; $26 SPRDISABLE
      .word   gfn_spr_flip-1           ; $27 SPRFLIP
      .word   gfn_spr_priority-1       ; $28 SPRPRIORITY
      .word   gfn_spr_shape-1          ; $29 SPRSHAPE
      .word   gfn_spr_setreg-1         ; $2A SPRSETREG  (reg8)
      .word   gfn_spr_setreg16-1       ; $2B SPRSETREG16
      .word   gfn_spr_getx-1           ; $2C SPRGETX  (reporter)
      .word   gfn_spr_gety-1           ; $2D SPRGETY  (reporter)
      .word   gfn_unimpl-1             ; $2E gap
      .word   gfn_unimpl-1             ; $2F gap
      .word   gfn_spr_coll_status-1    ; $30 SPRCOLLSTATUS (reporter)
      .word   gfn_spr_coll_mask-1      ; $31 SPRCOLLMASK   (reporter)
      .word   gfn_spr_coll_clear-1     ; $32 SPRCOLLCLEAR
      .word   gfn_spr_coll_readclear-1 ; $33 SPRCOLLREADCLEAR (reporter)
      .word   gfn_spr_coll_irqon-1     ; $34 SPRCOLLIRQON
      .word   gfn_spr_coll_irqoff-1    ; $35 SPRCOLLIRQOFF
      .word   gfn_spr_bg_status-1      ; $36 SPRBGSTATUS (reporter)
      .word   gfn_spr_bg_mask-1        ; $37 SPRBGMASK   (reporter)
      .word   gfn_spr_bg_clear-1       ; $38 SPRBGCLEAR
      .word   gfn_spr_bg_readclear-1   ; $39 SPRBGREADCLEAR (reporter)
      .word   gfn_spr_bg_irqon-1       ; $3A SPRBGIRQON
      .word   gfn_spr_bg_irqoff-1      ; $3B SPRBGIRQOFF
      .word   gfn_unimpl-1             ; $3C gap
      .word   gfn_unimpl-1             ; $3D gap
      .word   gfn_unimpl-1             ; $3E gap
      .word   gfn_unimpl-1             ; $3F gap
      .word   gfn_copper_list-1        ; $40 COPPER_LIST
      .word   gfn_copper_add-1         ; $41 COPPER_ADD
      .word   gfn_copper_clear-1       ; $42 COPPER_CLEAR
      .word   gfn_copper_on-1          ; $43 COPPER_ON
      .word   gfn_copper_off-1         ; $44 COPPER_OFF
      .word   gfn_copper_use-1         ; $45 COPPER_USE
      .word   gfn_copper_end-1         ; $46 COPPER_END
      .word   gfn_copper_split-1       ; $47 COPPER_SPLIT
      .word   gfn_copper_set_reg-1     ; $48 COPPER_SET_REG
      .word   gfn_copper_set_sprite_reg-1 ; $49 COPPER_SET_SPRITE_REG
      .word   gfn_unimpl-1             ; $4A gap
      .word   gfn_unimpl-1             ; $4B gap
      .word   gfn_unimpl-1             ; $4C gap
      .word   gfn_unimpl-1             ; $4D gap
      .word   gfn_unimpl-1             ; $4E gap
      .word   gfn_unimpl-1             ; $4F gap
      .word   gfn_blitcopy-1           ; $50 BLITCOPY
      .word   gfn_blitfill-1           ; $51 BLITFILL
      .word   gfn_blit_start-1         ; $52 BLIT_START
      .word   gfn_blit_wait-1          ; $53 BLIT_WAIT
      .word   gfn_dmacopy-1            ; $54 DMACOPY
      .word   gfn_dmafill-1            ; $55 DMAFILL
      .word   gfn_blit_status-1        ; $56 BLIT_STATUS (reporter)
      .word   gfn_blit_err-1           ; $57 BLIT_ERR    (reporter)
      .word   gfn_blit_count-1         ; $58 BLIT_COUNT  (reporter)
      .word   gfn_dma_status-1         ; $59 DMA_STATUS  (reporter)
      .word   gfn_dma_err-1            ; $5A DMA_ERR     (reporter)
      .word   gfn_dma_count-1          ; $5B DMA_COUNT   (reporter)
      .word   gfn_unimpl-1              ; $5C gap
      .word   gfn_unimpl-1              ; $5D gap
      .word   gfn_unimpl-1              ; $5E gap
      .word   gfn_unimpl-1              ; $5F gap
      .word   gfn_vs_blit-1             ; $60 VS_BLIT
      .word   gfn_vs_blit_start-1       ; $61 VS_BLIT_START
      .word   gfn_vs_fill-1             ; $62 VS_FILL
      .word   gfn_vs_use_original-1     ; $63 VS_USE_ORIGINAL
      .word   gfn_vs_use_rotated-1      ; $64 VS_USE_ROTATED
      .word   gfn_vs_rotate-1           ; $65 VS_ROTATE
      .word   gfn_vs_gfx_blit-1         ; $66 VS_GFX_BLIT
      .word   gfn_vs_gfx_blit_start-1   ; $67 VS_GFX_BLIT_START
      .word   gfn_vs_gfx_fill-1         ; $68 VS_GFX_FILL
      .word   gfn_vs_gfx_save_bg-1      ; $69 VS_GFX_SAVE_BG
      .word   gfn_vs_gfx_restore_bg-1   ; $6A VS_GFX_RESTORE_BG
      .word   gfn_vs_gfx_rotate_blit-1  ; $6B VS_GFX_ROTATE_BLIT
      .word   gfn_vs_gfx_rotate_blit_keyed-1  ; $6C VS_GFX_ROTATE_BLIT_KEYED
      .word   gfn_vs_gfx_rotate_blit_nowait-1 ; $6D VS_GFX_ROTATE_BLIT_NOWAIT
      .word   gfn_vs_scene_begin-1      ; $6E VS_SCENE_BEGIN
      .word   gfn_vs_scene_commit-1     ; $6F VS_SCENE_COMMIT
      .word   gfn_vs_scene_draw-1       ; $70 VS_SCENE_DRAW
      .word   gfn_vs_scene_commit_atomic-1 ; $71 VS_SCENE_COMMIT_ATOMIC
      .word   gfn_unimpl-1             ; $72 gap -> LERR_NO_FN
      .word   gfn_unimpl-1             ; $73 gap
      .word   gfn_unimpl-1             ; $74 gap
      .word   gfn_unimpl-1             ; $75 gap
      .word   gfn_unimpl-1             ; $76 gap
      .word   gfn_unimpl-1             ; $77 gap
      .word   gfn_unimpl-1             ; $78 gap
      .word   gfn_unimpl-1             ; $79 gap
      .word   gfn_unimpl-1             ; $7A gap
      .word   gfn_unimpl-1             ; $7B gap
      .word   gfn_unimpl-1             ; $7C gap
      .word   gfn_unimpl-1             ; $7D gap
      .word   gfn_unimpl-1             ; $7E gap
      .word   gfn_unimpl-1             ; $7F gap
      .word   gfn_ms_spawn-1           ; $80 MS_SPAWN
      .word   gfn_ms_destroy-1         ; $81 MS_DESTROY
      .word   gfn_ms_show-1            ; $82 MS_SHOW
      .word   gfn_ms_hide-1            ; $83 MS_HIDE
      .word   gfn_ms_pos-1             ; $84 MS_POS
      .word   gfn_ms_frame-1           ; $85 MS_FRAME
      .word   gfn_ms_anim-1            ; $86 MS_ANIM
      .word   gfn_ms_priority-1        ; $87 MS_PRIORITY
      .word   gfn_ms_transcolor-1      ; $88 MS_TRANSCOLOR
      .word   gfn_ms_tick-1            ; $89 MS_TICK
      .word   gfn_ms_commit-1          ; $8A MS_COMMIT
      .word   gfn_ms_commit_one-1      ; $8B MS_COMMIT_ONE
      .word   gfn_unimpl-1             ; $8C gap -> LERR_NO_FN
      .word   gfn_unimpl-1             ; $8D gap
      .word   gfn_unimpl-1             ; $8E gap
      .word   gfn_unimpl-1             ; $8F gap
      .word   gfn_unimpl-1             ; $90 gap
      .word   gfn_unimpl-1             ; $91 gap
      .word   gfn_unimpl-1             ; $92 gap
      .word   gfn_unimpl-1             ; $93 gap
      .word   gfn_unimpl-1             ; $94 gap
      .word   gfn_unimpl-1             ; $95 gap
      .word   gfn_unimpl-1             ; $96 gap
      .word   gfn_unimpl-1             ; $97 gap
      .word   gfn_unimpl-1             ; $98 gap
      .word   gfn_unimpl-1             ; $99 gap
      .word   gfn_unimpl-1             ; $9A gap
      .word   gfn_unimpl-1             ; $9B gap
      .word   gfn_unimpl-1             ; $9C gap
      .word   gfn_unimpl-1             ; $9D gap
      .word   gfn_unimpl-1             ; $9E gap
      .word   gfn_unimpl-1             ; $9F gap
      .word   gfn_memread-1            ; $A0 MEMREAD  (reporter)
      .word   gfn_memwrite-1           ; $A1 MEMWRITE
      .word   gfn_vpeek-1              ; $A2 VPEEK    (reporter)
      .word   gfn_vpoke-1              ; $A3 VPOKE
      .word   gfn_gsave-1              ; $A4 GSAVE
      .word   gfn_gload-1              ; $A5 GLOAD
      .word   gfn_nvgload-1            ; $A6 NVGLOAD
      .word   gfn_nvgload_at-1         ; $A7 NVGLOAD_AT
      .word   gfn_nvgload_named-1      ; $A8 NVGLOAD_NAMED
      .word   gfn_nvgload_named_at-1   ; $A9 NVGLOAD_NAMED_AT
      .word   gfn_unimpl-1             ; $AA gap -> LERR_NO_FN
      .word   gfn_unimpl-1             ; $AB gap
      .word   gfn_unimpl-1             ; $AC gap
      .word   gfn_unimpl-1             ; $AD gap
      .word   gfn_unimpl-1             ; $AE gap
      .word   gfn_unimpl-1             ; $AF gap
      .word   gfn_turtle_init-1        ; $B0 TURTLE_INIT  (batch 4c.2-2)
      .word   gfn_turtle_draw-1        ; $B1 TURTLE_DRAW  (batch 4c.2-2)
      .word   gfn_turtle_erase-1       ; $B2 TURTLE_ERASE (batch 4c.2-2)
      .word   gfn_turtle_op-1          ; $B3 TURTLE_OP — turtle command engine (4c.2-3)
      .word   gfn_unimpl-1             ; $B4 turtle (reserved)
      .word   gfn_unimpl-1             ; $B5 turtle-render (reserved)
      .word   gfn_unimpl-1             ; $B6 turtle-render (reserved)
      .word   gfn_unimpl-1             ; $B7 turtle-render (reserved)
      .word   gfn_unimpl-1             ; $B8 turtle-render (reserved)
      .word   gfn_unimpl-1             ; $B9 turtle-render (reserved)
      .word   gfn_unimpl-1             ; $BA turtle-render (reserved)
      .word   gfn_unimpl-1             ; $BB turtle-render (reserved)
      .word   gfn_unimpl-1             ; $BC turtle-render (reserved)
      .word   gfn_unimpl-1             ; $BD turtle-render (reserved)
      .word   gfn_unimpl-1             ; $BE turtle-render (reserved)
      .word   gfn_unimpl-1             ; $BF turtle-render (reserved)
      .word   gfn_anim_init-1          ; $C0 ANIM_INIT
      .word   gfn_anim_start-1         ; $C1 ANIM_START
      .word   gfn_anim_stop-1          ; $C2 ANIM_STOP
      .word   gfn_anim_tick-1          ; $C3 ANIM_TICK
      .word   gfn_anim_tick_one-1      ; $C4 ANIM_TICK_ONE
      .word   gfn_anim_set_frame-1     ; $C5 ANIM_SET_FRAME
      .word   gfn_anim_load_xram-1     ; $C6 ANIM_LOAD_XRAM
      .word   gfn_anim_load_disk-1     ; $C7 ANIM_LOAD_DISK
      .word   gfn_unimpl-1             ; $C8 gap
      .word   gfn_unimpl-1             ; $C9 gap
      .word   gfn_unimpl-1             ; $CA gap
      .word   gfn_unimpl-1             ; $CB gap
      .word   gfn_unimpl-1             ; $CC gap
      .word   gfn_unimpl-1             ; $CD gap
      .word   gfn_unimpl-1             ; $CE gap
      .word   gfn_unimpl-1             ; $CF gap
      .word   gfn_tween_begin-1            ; $D0 TWEEN_BEGIN
      .word   gfn_tween_eval-1             ; $D1 TWEEN_EVAL
      .word   gfn_tween_eval_linear-1      ; $D2 TWEEN_EVAL_LINEAR
      .word   gfn_tween_eval_ease_in-1     ; $D3 TWEEN_EVAL_EASE_IN
      .word   gfn_tween_eval_ease_out-1    ; $D4 TWEEN_EVAL_EASE_OUT
      .word   gfn_tween_eval_ease_in_out-1 ; $D5 TWEEN_EVAL_EASE_IN_OUT
      .word   gfn_tween_step-1             ; $D6 TWEEN_STEP
      .word   gfn_tween_step_linear-1      ; $D7 TWEEN_STEP_LINEAR
      .word   gfn_tween_step_ease_in-1     ; $D8 TWEEN_STEP_EASE_IN
      .word   gfn_tween_step_ease_out-1    ; $D9 TWEEN_STEP_EASE_OUT
      .word   gfn_tween_step_ease_in_out-1 ; $DA TWEEN_STEP_EASE_IN_OUT
      ; $DB.. grow here per domain; ids >= GFX_FN_COUNT -> LERR_NO_FN (bounds check)

; Any reachable-but-unimplemented fn-id: report LERR_NO_FN.
gfn_unimpl:
      lda     #LERR_NO_FN
      sta     LIB_STATUS
      rts

; copy_args_to_p — copy A arg-words (A in 1..4) from LIB_ARG0.. into VGC_P0..
;   Each arg cell is 4 bytes (LE 32-bit); only its low word (2 bytes) is used and
;   lands in 2 consecutive VGC_P regs. arg cell i (i=0..A-1): LIB_ARG0+i*4 low
;   byte -> VGC_P0+i*2, +1 high byte -> VGC_P0+i*2+1. The module runs from the
;   write-protected ROM window, so loop state lives in RAM scratch (LIB_SCRATCH),
;   never in CODE space. X walks the 4-byte source stride, Y the 2-byte dest
;   stride; the word counter in LIB_SCRATCH drives the loop A times.
GFX_WORDCNT = LIB_SCRATCH            ; remaining arg-words to copy
copy_args_to_p:
      sta     GFX_WORDCNT              ; word count (1..4)
      ldx     #0                       ; X = source byte index (0,4,8,12)
      ldy     #0                       ; Y = dest byte index (0,2,4,6)
@loop:
      lda     LIB_ARG0,x               ; arg word low byte -> VGC_P[2i]
      sta     VGC_P0,y
      lda     LIB_ARG0+1,x             ; arg word high byte -> VGC_P[2i+1]
      sta     VGC_P0+1,y
      iny                              ; dest += 2
      iny
      inx                              ; source += 4
      inx
      inx
      inx
      dec     GFX_WORDCNT
      bne     @loop
      rts

; finish_ok — common tail: wait for the VGC command to complete, then STATUS=OK.
; For ops that issue a VCMD_* via vgc_cmd (the draw primitives).
finish_ok:
      jsr     vgc_wait_cmd
      ; fall through to finish_ok_nowait

; finish_ok_nowait — STATUS=OK with no command wait. For pure register-store ops
; (set_fg/bg/border, set_font/mode, locate, reverse/flash, display_on/off) that
; never issue a VGC command, so there is no busy bit to poll.
finish_ok_nowait:
      lda     #LERR_OK
      sta     LIB_STATUS
      rts

; --- $00 GCLS: clear the gfx plane ---
; Issues VCMD_GCLS via vgc_gcls (does not wait) then vgc_wait_cmd so the caller
; (and the test) observes the cleared plane before the fn returns.
gfn_gcls:
      jsr     vgc_gcls
      jmp     finish_ok

; --- $01 GCOLOR: set the gfx draw colour ---
; color = low byte of ARG0 -> VGC_P0 (1-byte special; VCMD_GCOLOR reads only P0).
gfn_gcolor:
      lda     LIB_ARG0
      sta     VGC_P0
      jsr     vgc_gcolor
      jmp     finish_ok

; --- $02 PLOT: set pixel (x,y) ---  ARG0->P0/P1 (x s16), ARG1->P2/P3 (y s16)
gfn_plot:
      lda     #2
      jsr     copy_args_to_p
      jsr     vgc_plot
      jmp     finish_ok

; --- $03 UNPLOT: clear pixel (x,y) ---  ARG0->P0/P1, ARG1->P2/P3
gfn_unplot:
      lda     #2
      jsr     copy_args_to_p
      jsr     vgc_unplot
      jmp     finish_ok

; --- $04 LINE: (x0,y0)-(x1,y1) ---  ARG0->P0/P1, ARG1->P2/P3, ARG2->P4/P5, ARG3->P6/P7
gfn_line:
      lda     #4
      jsr     copy_args_to_p
      jsr     vgc_line
      jmp     finish_ok

; --- $05 CIRCLE: centre (cx,cy), radii (rx,ry) ---  ARG0..3 -> P0..P7
; ry=0 degenerates to a circle of radius rx (handled downstream in vgc/RTL).
gfn_circle:
      lda     #4
      jsr     copy_args_to_p
      jsr     vgc_circle
      jmp     finish_ok

; --- $06 RECT: outline (x0,y0)-(x1,y1) ---  ARG0..3 -> P0..P7
gfn_rect:
      lda     #4
      jsr     copy_args_to_p
      jsr     vgc_rect
      jmp     finish_ok

; --- $07 FILL: filled rect (x0,y0)-(x1,y1) ---  ARG0..3 -> P0..P7
gfn_fill:
      lda     #4
      jsr     copy_args_to_p
      jsr     vgc_fill
      jmp     finish_ok

; --- $08 PAINT: flood fill from (x,y) ---  ARG0->P0/P1, ARG1->P2/P3
gfn_paint:
      lda     #2
      jsr     copy_args_to_p
      jsr     vgc_paint
      jmp     finish_ok

; --- $09 GTEXT: draw BYTES string at (x,y) ---
; ARG0->P0/P1 (x), ARG1->P2/P3 (y). ARG2 packs ptr16 (low word) + len16 (high
; word) of the source string; the contract carries no font/scale args, so this
; wrapper uses the defaults the driver expects: P4 = font slot 0, P5 = scale 1.
; The string is copied into FIO_NAME (len bytes) with FIO_NAMELEN = len, matching
; the vgc_gtext @requires (VGC_P0..P5 + FIO_NAME/FIO_NAMELEN).
gfn_gtext:
      lda     #2                       ; x,y -> P0/P1, P2/P3
      jsr     copy_args_to_p
      lda     #0                       ; P4 = font slot 0 (default)
      sta     VGC_P4
      lda     #1                       ; P5 = scale 1 (default)
      sta     VGC_P5
      ; src ptr16 = ARG2 low word -> module-private ZP pointer LIB_ZP for (zp),Y
      lda     LIB_ARG2
      sta     LIB_ZP
      lda     LIB_ARG2+1
      sta     LIB_ZP+1
      ; len = ARG2 high word low byte; clamp to the FIO name buffer limit
      lda     LIB_ARG2+2
      cmp     #GFX_FIO_NAME_LIMIT+1
      bcc     @lenok
      lda     #GFX_FIO_NAME_LIMIT
@lenok:
      sta     FIO_NAMELEN
      cmp     #0
      beq     @copied                  ; empty string -> nothing to copy
      tax                              ; X = byte count
      ldy     #0                       ; Y = shared src/dest index
@copyloop:
      lda     (LIB_ZP),y
      sta     FIO_NAME,y
      iny
      dex
      bne     @copyloop
@copied:
      jsr     vgc_gtext
      jmp     finish_ok

; =====================================================================
; $10-$1F  text / mode domain (batch 4b.3). All register-store ops (no VGC
; command), so each tail is finish_ok_nowait — except CLS, which issues a
; char-out command and waits inside vgc_cls before we set OK.
; =====================================================================

; --- $10 COLOR: set text fg/bg/border ---  fg=ARG0, bg=ARG1, border=ARG2.
; Each driver entry takes the colour byte in X (STX VGC_FGCOL/BGCOL/BORDER).
gfn_color:
      ldx     LIB_ARG0
      jsr     vgc_set_fg
      ldx     LIB_ARG1
      jsr     vgc_set_bg
      ldx     LIB_ARG2
      jsr     vgc_set_border
      jmp     finish_ok_nowait

; --- $11 FONT: select text font slot ---  slot=ARG0 -> X; vgc_set_font.
gfn_font:
      ldx     LIB_ARG0
      jsr     vgc_set_font
      jmp     finish_ok_nowait

; --- $12 MODE: set graphics/text mode ---  mode=ARG0 -> X; vgc_set_mode.
gfn_mode:
      ldx     LIB_ARG0
      jsr     vgc_set_mode
      jmp     finish_ok_nowait

; --- $13 REVERSE: enable reverse text using current fg/bg ---
gfn_reverse:
      jsr     vgc_reverse_default
      jmp     finish_ok_nowait

; --- $14 REVERSEOFF: disable reverse text ---
gfn_reverseoff:
      jsr     vgc_reverse_off
      jmp     finish_ok_nowait

; --- $15 FLASH: enable flashing text ---
gfn_flash:
      jsr     vgc_flash_on
      jmp     finish_ok_nowait

; --- $16 FLASHOFF: disable flashing text ---
gfn_flashoff:
      jsr     vgc_flash_off
      jmp     finish_ok_nowait

; --- $17 LOCATE: set text cursor (col,row) ---  col=ARG0 -> VGC_P0, row=ARG1 -> VGC_P1.
; vgc_locate copies VGC_P0 -> VGC_CURSX and VGC_P1 -> VGC_CURSY.
gfn_locate:
      lda     LIB_ARG0
      sta     VGC_P0
      lda     LIB_ARG1
      sta     VGC_P1
      jsr     vgc_locate
      jmp     finish_ok_nowait

; --- $18 CLS: clear the text screen ---  vgc_cls issues the $0C form-feed via
; VGC_CHAROUT and waits internally; we just set OK afterwards (no extra wait).
gfn_cls:
      jsr     vgc_cls
      jmp     finish_ok_nowait

; --- $1A DISPLAYON: restore full brightness ---  VGC_DIMMER = $0F.
gfn_displayon:
      jsr     vgc_display_on
      jmp     finish_ok_nowait

; --- $1B DISPLAYOFF: blank output, keep timing ---  VGC_DIMMER = $00.
gfn_displayoff:
      jsr     vgc_display_off
      jmp     finish_ok_nowait

; =====================================================================
; $20-$3B  hw-sprite domain (batch 4b.4). Driver: sprite.s (included below).
;
; Command-issuing ops load VGC_P* then issue the command and wait (finish_ok).
; The sprite engine reads P0 = sprite index for every command. Register-store
; ops write the $A040 attribute block directly (finish_ok_nowait). Reporter ops
; copy the driver's A/Y return into LIB_RESULT then set OK (finish_result_*).
; =====================================================================

; --- reporter result helpers (RAM scratch only — they write the LIB_RESULT cell) ---
; finish_result16 — Y = result low byte, A = result high byte -> LIB_RESULT
;   (4-byte LE; bytes 2-3 zeroed), STATUS=OK.
finish_result16:
      sty     LIB_RESULT
      sta     LIB_RESULT+1
      lda     #0
      sta     LIB_RESULT+2
      sta     LIB_RESULT+3
      jmp     finish_ok_nowait

; finish_result8 — A = result byte -> LIB_RESULT low byte (bytes 1-3 zeroed), OK.
finish_result8:
      sta     LIB_RESULT
      lda     #0
      sta     LIB_RESULT+1
      sta     LIB_RESULT+2
      sta     LIB_RESULT+3
      jmp     finish_ok_nowait

; --- $20 SPRDEFINE: set one shape pixel of sprite to a colour ---
; idx:ARG0->P0, x(0-15):ARG1->P1, y(0-15):ARG2->P2, color:ARG3->P3. VCMD_SPRDEF.
gfn_spr_define:
      lda     LIB_ARG0
      sta     VGC_P0
      lda     LIB_ARG1
      sta     VGC_P1
      lda     LIB_ARG2
      sta     VGC_P2
      lda     LIB_ARG3
      sta     VGC_P3
      lda     #VCMD_SPRDEF
      jsr     sprite_command
      jmp     finish_ok

; --- $21 SPRROW: write one 16px shape row (8 nibble-packed bytes) ---
; idx:ARG0->P0, row:ARG1->P1, then 8 shape bytes ARG2[0..3]->P2..P5,
; ARG3[0..3]->P6..P9. sprite_row issues VCMD_SPRROW (reads P0..P9).
gfn_spr_row:
      lda     LIB_ARG0
      sta     VGC_P0
      lda     LIB_ARG1
      sta     VGC_P1
      ; ARG2 (4 bytes) -> P2..P5, ARG3 (4 bytes) -> P6..P9
      ldx     #0
@rowloop:
      lda     LIB_ARG2,x
      sta     VGC_P2,x
      inx
      cpx     #8
      bne     @rowloop
      jsr     sprite_row
      jmp     finish_ok

; --- $22 SPRCLEAR: clear sprite shape data ---  idx:ARG0->P0.
gfn_spr_clear:
      lda     LIB_ARG0
      sta     VGC_P0
      jsr     sprite_clear
      jmp     finish_ok

; --- $23 SPRCOPY: copy shape data src->dst ---  src:ARG0->P0, dst:ARG1->P1.
gfn_spr_copy:
      lda     LIB_ARG0
      sta     VGC_P0
      lda     LIB_ARG1
      sta     VGC_P1
      jsr     sprite_copy
      jmp     finish_ok

; --- $24 SPRPOS: move sprite ---  idx:ARG0->P0, x(s16):ARG1->P1/P2, y:ARG2->P3.
gfn_spr_pos:
      lda     LIB_ARG0
      sta     VGC_P0
      lda     LIB_ARG1                 ; x low  -> P1
      sta     VGC_P1
      lda     LIB_ARG1+1               ; x high -> P2
      sta     VGC_P2
      lda     LIB_ARG2                 ; y      -> P3
      sta     VGC_P3
      jsr     sprite_pos
      jmp     finish_ok

; --- $25 SPRENABLE: enable sprite ---  idx:ARG0->P0.
gfn_spr_enable:
      lda     LIB_ARG0
      sta     VGC_P0
      jsr     sprite_enable
      jmp     finish_ok

; --- $26 SPRDISABLE: disable sprite ---  idx:ARG0->P0.
gfn_spr_disable:
      lda     LIB_ARG0
      sta     VGC_P0
      jsr     sprite_disable
      jmp     finish_ok

; --- $27 SPRFLIP: set flip flags ---  idx:ARG0->P0, flags:ARG1->P1.
gfn_spr_flip:
      lda     LIB_ARG0
      sta     VGC_P0
      lda     LIB_ARG1
      sta     VGC_P1
      jsr     sprite_flip
      jmp     finish_ok

; --- $28 SPRPRIORITY: set priority ---  idx:ARG0->P0, pri:ARG1->P1.
gfn_spr_priority:
      lda     LIB_ARG0
      sta     VGC_P0
      lda     LIB_ARG1
      sta     VGC_P1
      jsr     sprite_priority
      jmp     finish_ok

; --- $29 SPRSHAPE: set sprite shape-slot index ---  idx:ARG0->X, shape:ARG1->A.
; sprite_set_shape writes the VGC_SPR_SHAPE attribute register (no command).
gfn_spr_shape:
      ldx     LIB_ARG0
      lda     LIB_ARG1
      jsr     sprite_set_shape
      jmp     finish_ok_nowait

; --- $2A SPRSETREG: write one 8-bit sprite attribute field ---
; idx:ARG0->X, field:ARG1->Y, value:ARG2->A. sprite_set_reg8 (no command).
gfn_spr_setreg:
      ldx     LIB_ARG0
      ldy     LIB_ARG1
      lda     LIB_ARG2
      jsr     sprite_set_reg8
      jmp     finish_ok_nowait

; --- $2B SPRSETREG16: write one 16-bit sprite attribute field ---
; idx:ARG0->X, field:ARG1->Y, value16:ARG2->NVR0L/NVR0H. sprite_set_reg16.
gfn_spr_setreg16:
      ldx     LIB_ARG0
      ldy     LIB_ARG1
      lda     LIB_ARG2
      sta     NVR0L
      lda     LIB_ARG2+1
      sta     NVR0H
      jsr     sprite_set_reg16
      jmp     finish_ok_nowait

; --- $2C SPRGETX: read sprite 16-bit X (reporter) ---  idx:ARG0->X.
; sprite_get_x returns A=X high, Y=X low -> LIB_RESULT 16-bit.
gfn_spr_getx:
      ldx     LIB_ARG0
      jsr     sprite_get_x
      jmp     finish_result16

; --- $2D SPRGETY: read sprite Y byte (reporter) ---  idx:ARG0->X.
; sprite_get_y returns Y = Y byte -> move to A -> LIB_RESULT 8-bit.
gfn_spr_gety:
      ldx     LIB_ARG0
      jsr     sprite_get_y
      tya
      jmp     finish_result8

; --- $30 SPRCOLLSTATUS: sprite-sprite collision low byte (reporter) ---
gfn_spr_coll_status:
      jsr     sprite_collision_status
      jmp     finish_result8

; --- $31 SPRCOLLMASK: sprite-sprite 16-bit collision mask (reporter) ---
; sprite_collision_mask returns A=hi (sprites 8-15), Y=lo (sprites 0-7).
gfn_spr_coll_mask:
      jsr     sprite_collision_mask
      jmp     finish_result16

; --- $32 SPRCOLLCLEAR: clear sprite-sprite mask + ack IRQ ---
gfn_spr_coll_clear:
      jsr     sprite_collision_clear
      jmp     finish_ok_nowait

; --- $33 SPRCOLLREADCLEAR: read 16-bit mask then clear (reporter) ---
gfn_spr_coll_readclear:
      jsr     sprite_collision_read_clear
      jmp     finish_result16

; --- $34 SPRCOLLIRQON: enable sprite-sprite collision IRQ (+CLI) ---
gfn_spr_coll_irqon:
      jsr     sprite_collision_irq_enable
      jmp     finish_ok_nowait

; --- $35 SPRCOLLIRQOFF: disable sprite-sprite collision IRQ ---
gfn_spr_coll_irqoff:
      jsr     sprite_collision_irq_disable
      jmp     finish_ok_nowait

; --- $36 SPRBGSTATUS: sprite-background collision low byte (reporter) ---
gfn_spr_bg_status:
      jsr     sprite_background_collision_status
      jmp     finish_result8

; --- $37 SPRBGMASK: sprite-background 16-bit collision mask (reporter) ---
gfn_spr_bg_mask:
      jsr     sprite_background_collision_mask
      jmp     finish_result16

; --- $38 SPRBGCLEAR: clear sprite-background mask + ack IRQ ---
gfn_spr_bg_clear:
      jsr     sprite_background_clear
      jmp     finish_ok_nowait

; --- $39 SPRBGREADCLEAR: read 16-bit bg mask then clear (reporter) ---
gfn_spr_bg_readclear:
      jsr     sprite_background_read_clear
      jmp     finish_result16

; --- $3A SPRBGIRQON: enable sprite-bg collision IRQ (+CLI) ---
gfn_spr_bg_irqon:
      jsr     sprite_background_irq_enable
      jmp     finish_ok_nowait

; --- $3B SPRBGIRQOFF: disable sprite-bg collision IRQ ---
gfn_spr_bg_irqoff:
      jsr     sprite_background_irq_disable
      jmp     finish_ok_nowait

; =====================================================================
; $40-$49  copper domain (batch 4b.5). Drivers: copper.s + copper_split.s.
;
; The copper edits/plays raster display lists. Every command-issuing entry goes
; through copper_command -> STA VGC_CMD; the wrapper waits via vgc_wait_cmd and
; sets STATUS=OK (finish_ok). copper_add reads P0/P1=x, P2=y, P3/P4=register
; specifier, P5=value. List-select ops (list/use) read P0. set_reg/set_sprite_reg
; expose the COMPLETE add-a-register-write op: prep P3/P4 via the matching
; register helper, load x/y/value, then issue copper_add (see libgraphics.inc).
; =====================================================================

; --- $40 COPPER_LIST: select the list that ADD/CLEAR edit ---  idx:ARG0->P0.
gfn_copper_list:
      lda     LIB_ARG0
      sta     VGC_P0
      jsr     copper_list
      jmp     finish_ok

; --- $41 COPPER_ADD: add a register-write event to the target list ---
; x(s16):ARG0->P0/P1, y:ARG1->P2, reg(16):ARG2->P3/P4, value:ARG3->P5.
gfn_copper_add:
      lda     LIB_ARG0                 ; x low  -> P0
      sta     VGC_P0
      lda     LIB_ARG0+1               ; x high -> P1
      sta     VGC_P1
      lda     LIB_ARG1                 ; y      -> P2
      sta     VGC_P2
      lda     LIB_ARG2                 ; reg low  -> P3
      sta     VGC_P3
      lda     LIB_ARG2+1               ; reg high -> P4
      sta     VGC_P4
      lda     LIB_ARG3                 ; value  -> P5
      sta     VGC_P5
      jsr     copper_add
      jmp     finish_ok

; --- $42 COPPER_CLEAR: empty the target list ---  ().
gfn_copper_clear:
      jsr     copper_clear
      jmp     finish_ok

; --- $43 COPPER_ON: enable copper execution ---  ().
gfn_copper_on:
      jsr     copper_on
      jmp     finish_ok

; --- $44 COPPER_OFF: disable copper execution ---  ().
gfn_copper_off:
      jsr     copper_off
      jmp     finish_ok

; --- $45 COPPER_USE: make a list active at the next vblank ---  idx:ARG0->P0.
gfn_copper_use:
      lda     LIB_ARG0
      sta     VGC_P0
      jsr     copper_use
      jmp     finish_ok

; --- $46 COPPER_END: finish the current list (target := active) ---  ().
gfn_copper_end:
      jsr     copper_list_end
      jmp     finish_ok

; --- $47 COPPER_SPLIT: one list, two VGC_MODE rules at a scanline split ---
; idx:ARG0->P0, splitY:ARG1->P1, mode0:ARG2->P2, mode1:ARG3->P3.
; copper_split_mode runs the full off/list/clear/add/add/use/on sequence,
; waiting internally between commands; it leaves the copper enabled.
gfn_copper_split:
      lda     LIB_ARG0
      sta     VGC_P0
      lda     LIB_ARG1
      sta     VGC_P1
      lda     LIB_ARG2
      sta     VGC_P2
      lda     LIB_ARG3
      sta     VGC_P3
      jsr     copper_split_mode
      jmp     finish_ok

; --- $48 COPPER_SET_REG: add a copper write to a direct VGC register ---
; x(s16):ARG0->P0/P1, y:ARG1->P2, regIndex:ARG2->A (copper_set_reg_index sets
; P3/P4), value:ARG3->P5; then copper_add. Observable CopperEvent.
gfn_copper_set_reg:
      lda     LIB_ARG0                 ; x low  -> P0
      sta     VGC_P0
      lda     LIB_ARG0+1               ; x high -> P1
      sta     VGC_P1
      lda     LIB_ARG1                 ; y      -> P2
      sta     VGC_P2
      lda     LIB_ARG3                 ; value  -> P5
      sta     VGC_P5
      lda     LIB_ARG2                 ; register index -> A
      jsr     copper_set_reg_index     ; -> P3/P4
      jsr     copper_add
      jmp     finish_ok

; --- $49 COPPER_SET_SPRITE_REG: add a copper write to a sprite attribute reg ---
; x(s16):ARG0->P0/P1, y:ARG1->P2, sprIdx:ARG2 low->X, field:ARG2 byte1->A
; (copper_set_sprite_reg sets P3/P4 = absolute sprite reg addr), value:ARG3->P5;
; then copper_add. Observable CopperEvent.
gfn_copper_set_sprite_reg:
      lda     LIB_ARG0                 ; x low  -> P0
      sta     VGC_P0
      lda     LIB_ARG0+1               ; x high -> P1
      sta     VGC_P1
      lda     LIB_ARG1                 ; y      -> P2
      sta     VGC_P2
      lda     LIB_ARG3                 ; value  -> P5
      sta     VGC_P5
      ldx     LIB_ARG2                 ; sprite index -> X
      lda     LIB_ARG2+1               ; field offset -> A
      jsr     copper_set_sprite_reg    ; -> P3/P4
      jsr     copper_add
      jmp     finish_ok

; =====================================================================
; $50-$5B  blit / dma domain (batch 4b.6). Drivers: blitter.s + dma.s.
;
; Each move op marshals the ARG cells into the BLT_*/DMA_* hardware registers
; (memory-mapped at $BA83.. / $BA63.., NOT code space — RAM-side stores), then
; JSRs the driver entry, which issues the *_CMD_START and self-waits on the
; controller status register. The wrapper then sets STATUS=OK (finish_ok_nowait):
; the driver's A=0/1 ok/error return is exposed separately via the status/err
; reporters, matching the established "wrapper always OK on dispatch; reporters
; carry the detail" pattern. Reporters read the controller registers into
; LIB_RESULT.  See libgraphics.inc for the full arg-cell -> register contract.
;
; Arg byte layout (LE 32-bit cells; only the documented bytes are read):
;   ARG0: byte0 = srcSpace, byte1 = dstSpace, byte2 = fillValue
;   ARG1: src offset (bytes 0..2 = 24-bit)
;   ARG2: dst offset (bytes 0..2 = 24-bit)
;   ARG3: copy/fill MOVE -> 24-bit length; BLIT -> width(b0/b1)|height(b2/b3)
; =====================================================================

; --- $50 BLITCOPY: 2-D rectangular copy, tightly packed (stride = width). ---
gfn_blitcopy:
      lda     LIB_ARG0                 ; srcSpace
      sta     BLT_SRCSPACE
      lda     LIB_ARG0+1               ; dstSpace
      sta     BLT_DSTSPACE
      lda     LIB_ARG1                 ; src offset (24-bit)
      sta     BLT_SRCL
      lda     LIB_ARG1+1
      sta     BLT_SRCM
      lda     LIB_ARG1+2
      sta     BLT_SRCH
      lda     LIB_ARG2                 ; dst offset (24-bit)
      sta     BLT_DSTL
      lda     LIB_ARG2+1
      sta     BLT_DSTM
      lda     LIB_ARG2+2
      sta     BLT_DSTH
      lda     LIB_ARG3                 ; width (16-bit) -> WIDTH + both strides
      sta     BLT_WIDTHL
      sta     BLT_SRCSTRL
      sta     BLT_DSTSTRL
      lda     LIB_ARG3+1
      sta     BLT_WIDTHH
      sta     BLT_SRCSTRH
      sta     BLT_DSTSTRH
      lda     LIB_ARG3+2               ; height (16-bit)
      sta     BLT_HEIGHTL
      lda     LIB_ARG3+3
      sta     BLT_HEIGHTH
      jsr     blitter_copy             ; STZ MODE/CKEY; CMD_START; wait
      jmp     finish_ok_nowait

; --- $51 BLITFILL: fill a tightly-packed rectangle with ARG0.byte2. ---
; blitter_fill zeros the src regs + CKEY itself; we set dst/geometry + fill value.
gfn_blitfill:
      lda     LIB_ARG0+1               ; dstSpace
      sta     BLT_DSTSPACE
      lda     LIB_ARG0+2               ; fill value
      sta     BLT_FILLVALUE
      lda     LIB_ARG2                 ; dst offset (24-bit)
      sta     BLT_DSTL
      lda     LIB_ARG2+1
      sta     BLT_DSTM
      lda     LIB_ARG2+2
      sta     BLT_DSTH
      lda     LIB_ARG3                 ; width (16-bit) -> WIDTH + dst stride
      sta     BLT_WIDTHL
      sta     BLT_DSTSTRL
      lda     LIB_ARG3+1
      sta     BLT_WIDTHH
      sta     BLT_DSTSTRH
      lda     LIB_ARG3+2               ; height (16-bit)
      sta     BLT_HEIGHTL
      lda     LIB_ARG3+3
      sta     BLT_HEIGHTH
      jsr     blitter_fill
      jmp     finish_ok_nowait

; --- $52 BLIT_START: issue a copy on caller-preloaded BLT_* registers + wait. ---
gfn_blit_start:
      jsr     blitter_start_copy
      jmp     finish_ok_nowait

; --- $53 BLIT_WAIT: poll BLT_STATUS to completion (caller-issued op). ---
gfn_blit_wait:
      jsr     blitter_wait
      jmp     finish_ok_nowait

; --- $54 DMACOPY: 1-D bulk copy. ---
gfn_dmacopy:
      lda     LIB_ARG0                 ; srcSpace
      sta     DMA_SRCSPACE
      lda     LIB_ARG0+1               ; dstSpace
      sta     DMA_DSTSPACE
      lda     LIB_ARG1                 ; src offset (24-bit)
      sta     DMA_SRCL
      lda     LIB_ARG1+1
      sta     DMA_SRCM
      lda     LIB_ARG1+2
      sta     DMA_SRCH
      lda     LIB_ARG2                 ; dst offset (24-bit)
      sta     DMA_DSTL
      lda     LIB_ARG2+1
      sta     DMA_DSTM
      lda     LIB_ARG2+2
      sta     DMA_DSTH
      lda     LIB_ARG3                 ; length (24-bit)
      sta     DMA_LENL
      lda     LIB_ARG3+1
      sta     DMA_LENM
      lda     LIB_ARG3+2
      sta     DMA_LENH
      jsr     dma_copy                 ; STZ MODE; CMD_START; wait
      jmp     finish_ok_nowait

; --- $55 DMAFILL: fill DMA range with ARG0.byte2. ---
; dma_fill zeros the src regs itself; we set dst/length/space + fill value.
gfn_dmafill:
      lda     LIB_ARG0+1               ; dstSpace
      sta     DMA_DSTSPACE
      lda     LIB_ARG0+2               ; fill value
      sta     DMA_FILLVALUE
      lda     LIB_ARG2                 ; dst offset (24-bit)
      sta     DMA_DSTL
      lda     LIB_ARG2+1
      sta     DMA_DSTM
      lda     LIB_ARG2+2
      sta     DMA_DSTH
      lda     LIB_ARG3                 ; length (24-bit)
      sta     DMA_LENL
      lda     LIB_ARG3+1
      sta     DMA_LENM
      lda     LIB_ARG3+2
      sta     DMA_LENH
      jsr     dma_fill
      jmp     finish_ok_nowait

; --- reporters: copy a controller register into LIB_RESULT, then STATUS=OK. ---
; finish_result24 — copy 3 consecutive register bytes at (ABS,X) base into the
;   low 24 bits of LIB_RESULT (top byte zeroed). X holds the per-fn base low byte
;   via an absolute,Y-free path: callers JSR with the count-low address in NVR0
;   pointer would need ZP; the count regs are fixed addresses, so each reporter
;   loads them directly and falls into finish_result_store24.
; finish_result_store24 — A=byte0(low), Y=byte1(mid), X=byte2(high) -> LIB_RESULT.
finish_result_store24:
      sta     LIB_RESULT
      sty     LIB_RESULT+1
      stx     LIB_RESULT+2
      lda     #0
      sta     LIB_RESULT+3
      jmp     finish_ok_nowait

; --- $56 BLIT_STATUS / $57 BLIT_ERR: single controller byte -> RESULT. ---
gfn_blit_status:
      lda     BLT_STATUS_REG
      jmp     finish_result8

gfn_blit_err:
      lda     BLT_ERRCODE_REG
      jmp     finish_result8

; --- $58 BLIT_COUNT: 24-bit bytes-written counter -> RESULT low 24 bits. ---
gfn_blit_count:
      lda     BLT_CNTL
      ldy     BLT_CNTM
      ldx     BLT_CNTH
      jmp     finish_result_store24

; --- $59 DMA_STATUS / $5A DMA_ERR: single controller byte -> RESULT. ---
gfn_dma_status:
      lda     DMA_STATUS_REG
      jmp     finish_result8

gfn_dma_err:
      lda     DMA_ERRCODE_REG
      jmp     finish_result8

; --- $5B DMA_COUNT: 24-bit bytes-moved counter -> RESULT low 24 bits. ---
gfn_dma_count:
      lda     DMA_CNTL
      ldy     DMA_CNTM
      ldx     DMA_CNTH
      jmp     finish_result_store24

; =====================================================================
; $60-$71  vsprite domain (batch 4b.7). Driver: vsprite.s (included below;
; it pulls blitter.s, dedup'd by the .ifndef guard).
;
; Every op shares ONE marshalling shape (see libgraphics.inc): ARG0 is a BYTES
; arg (ptr16 low word, len16 high word) pointing at a caller struct laid out
; byte-for-byte as the VSPRITE_* BSS config block. vs_load_cfg copies up to
; GFX_VS_CFG_MAX bytes from that struct into the BSS block via (LIB_ZP),Y (RAM
; scratch only — the module runs from write-protected ROM), then the wrapper
; JSRs the driver entry and falls into finish_vs.
;
; finish_vs maps the driver's A=0/1 result: A -> LIB_RESULT byte 0 (bytes 1-3
; zeroed) AND A!=0 -> LIB_STATUS=LERR_VSPRITE_FAIL, A==0 -> LERR_OK. None of
; these wrappers wait on the VGC command bus: the blitter self-waits inside the
; driver (blitter_wait), and the two frame-pacing rotate variants poll VGC_FRAME
; inside the driver. So the wrapper has nothing extra to wait on.

; vs_load_cfg — copy the caller's config struct (ARG0 BYTES) into the VSPRITE_*
; BSS block. ARG0 low word = ptr16 -> LIB_ZP; ARG0 high word low byte = len,
; clamped to GFX_VS_CFG_MAX. Walks (LIB_ZP),Y -> VSPRITE_SRCSPACE,Y. Y is the
; shared src/dest index (the struct and the BSS block share the same field
; order, so a single Y indexes both). Loop counter lives in LIB_SCRATCH.
GFX_VS_CFGCNT = LIB_SCRATCH+1        ; remaining config bytes to copy (distinct
                                     ; from GFX_WORDCNT at LIB_SCRATCH+0)
vs_load_cfg:
      lda     LIB_ARG0                 ; struct ptr16 -> LIB_ZP
      sta     LIB_ZP
      lda     LIB_ARG0+1
      sta     LIB_ZP+1
      lda     LIB_ARG0+2               ; len low byte; clamp to GFX_VS_CFG_MAX
      cmp     #GFX_VS_CFG_MAX+1
      bcc     @lenok
      lda     #GFX_VS_CFG_MAX
@lenok:
      sta     GFX_VS_CFGCNT
      beq     @done                    ; len 0 -> nothing to copy
      ldy     #0
@loop:
      lda     (LIB_ZP),y
      sta     VSPRITE_SRCSPACE,y       ; BSS block base; Y indexes both struct + block
      iny
      cpy     GFX_VS_CFGCNT
      bne     @loop
@done:
      rts

; finish_vs — driver result in A: publish to LIB_RESULT byte 0, map !=0 to a
; vsprite-fail status, ==0 to OK.
finish_vs:
      sta     LIB_RESULT               ; raw driver A -> RESULT byte 0
      ldx     #0
      stx     LIB_RESULT+1
      stx     LIB_RESULT+2
      stx     LIB_RESULT+3
      cmp     #VSPRITE_RESULT_OK
      bne     @fail
      lda     #LERR_OK
      sta     LIB_STATUS
      rts
@fail:
      lda     #LERR_VSPRITE_FAIL
      sta     LIB_STATUS
      rts

; --- $60 VS_BLIT: rectangular copy with the configured src/dst (self-waits). ---
gfn_vs_blit:
      jsr     vs_load_cfg
      jsr     vsprite_blit
      jmp     finish_vs

; --- $61 VS_BLIT_START: issue the configured blit without waiting. ---
gfn_vs_blit_start:
      jsr     vs_load_cfg
      jsr     vsprite_blit_start
      jmp     finish_vs

; --- $62 VS_FILL: fill the configured destination rectangle (self-waits). ---
gfn_vs_fill:
      jsr     vs_load_cfg
      jsr     vsprite_fill
      jmp     finish_vs

; --- $63 VS_USE_ORIGINAL: point SRC* at the immutable ORIG* shape. ---
gfn_vs_use_original:
      jsr     vs_load_cfg
      jsr     vsprite_use_original
      jmp     finish_vs

; --- $64 VS_USE_ROTATED: point SRC* at the ROT* output buffer. ---
gfn_vs_use_rotated:
      jsr     vs_load_cfg
      jsr     vsprite_use_rotated
      jmp     finish_vs

; --- $65 VS_ROTATE: rotate ORIG->ROT by ROTANGLE; repoints SRC*->ROT* (self-waits). ---
gfn_vs_rotate:
      jsr     vs_load_cfg
      jsr     vsprite_rotate
      jmp     finish_vs

; --- $66 VS_GFX_BLIT: copy the virtual sprite to the gfx plane at X/Y (self-waits). ---
gfn_vs_gfx_blit:
      jsr     vs_load_cfg
      jsr     vsprite_gfx_blit
      jmp     finish_vs

; --- $67 VS_GFX_BLIT_START: start the gfx-plane blit without waiting. ---
gfn_vs_gfx_blit_start:
      jsr     vs_load_cfg
      jsr     vsprite_gfx_blit_start
      jmp     finish_vs

; --- $68 VS_GFX_FILL: fill a gfx-plane rectangle at X/Y (self-waits). ---
gfn_vs_gfx_fill:
      jsr     vs_load_cfg
      jsr     vsprite_gfx_fill
      jmp     finish_vs

; --- $69 VS_GFX_SAVE_BG: save the gfx rect under X/Y into BG* (self-waits). ---
gfn_vs_gfx_save_bg:
      jsr     vs_load_cfg
      jsr     vsprite_gfx_save_bg
      jmp     finish_vs

; --- $6A VS_GFX_RESTORE_BG: restore BG* back into the gfx plane at X/Y (self-waits). ---
gfn_vs_gfx_restore_bg:
      jsr     vs_load_cfg
      jsr     vsprite_gfx_restore_bg
      jmp     finish_vs

; --- $6B VS_GFX_ROTATE_BLIT: rotate offscreen, wait a frame, blit (PUMPED). ---
gfn_vs_gfx_rotate_blit:
      jsr     vs_load_cfg
      jsr     vsprite_gfx_rotate_blit
      jmp     finish_vs

; --- $6C VS_GFX_ROTATE_BLIT_KEYED: rotate, wait a frame, color-keyed blit (PUMPED). ---
gfn_vs_gfx_rotate_blit_keyed:
      jsr     vs_load_cfg
      jsr     vsprite_gfx_rotate_blit_keyed
      jmp     finish_vs

; --- $6D VS_GFX_ROTATE_BLIT_NOWAIT: rotate then start the blit, no frame wait. ---
gfn_vs_gfx_rotate_blit_nowait:
      jsr     vs_load_cfg
      jsr     vsprite_gfx_rotate_blit_nowait
      jmp     finish_vs

; --- $6E VS_SCENE_BEGIN: restore every saved old background in the scene list. ---
; vsprite_scene_begin has no A return (void); finish_vs treats A as the result,
; so we force A=OK before the tail. The scene-table ptr lives in the cfg struct
; (SCENE_ADDRL/H/COUNT at offsets 41-43).
gfn_vs_scene_begin:
      jsr     vs_load_cfg
      jsr     vsprite_scene_begin
      lda     #VSPRITE_RESULT_OK
      jmp     finish_vs

; --- $6F VS_SCENE_COMMIT: save all backgrounds, then draw all visible descriptors. ---
gfn_vs_scene_commit:
      jsr     vs_load_cfg
      jsr     vsprite_scene_commit
      lda     #VSPRITE_RESULT_OK
      jmp     finish_vs

; --- $70 VS_SCENE_DRAW: draw all visible descriptors (no background save). ---
gfn_vs_scene_draw:
      jsr     vs_load_cfg
      jsr     vsprite_scene_draw
      lda     #VSPRITE_RESULT_OK
      jmp     finish_vs

; --- $71 VS_SCENE_COMMIT_ATOMIC: compose into a work buffer, commit one blit. ---
gfn_vs_scene_commit_atomic:
      jsr     vs_load_cfg
      jsr     vsprite_scene_commit_atomic
      lda     #VSPRITE_RESULT_OK
      jmp     finish_vs

; =====================================================================
; $80-$8B  msprite / meta-sprite domain (batch 4b.8). Driver: msprite.s
; (included below; it pulls sprite.s, dedup'd by the .ifndef guard).
;
; A meta-sprite object owns a range of hardware sprites; commit writes the owned
; $A040+ attribute registers, so the effect is observable on the real VGC. State
; lives in the module-owned MSPRITE_* BSS table, persistent across lib_calls and
; assumed zeroed at cold boot (no init fn in §2 scope).
;
; Result mapping. SPAWN returns A = object handle ($FF on failure); the wrapper
; publishes A to LIB_RESULT byte 0 and maps $FF -> LERR_MSPRITE_FAIL, else OK
; (finish_ms_spawn). Per-object ops return A = MSPRITE_RESULT_* code (0 = OK); the
; wrapper publishes A to LIB_RESULT and maps A!=0 -> LERR_MSPRITE_FAIL
; (finish_ms). tick/commit always return A=0 -> OK (finish_ms too).
;
; None of these issue a VGC *command* (commit is a sequence of direct register
; stores, completing synchronously), so the wrappers need no command wait.

; finish_ms_spawn — A = object handle. Publish to LIB_RESULT byte 0 (bytes 1-3
; zeroed); $FF -> LERR_MSPRITE_FAIL, any valid handle -> LERR_OK.
finish_ms_spawn:
      sta     LIB_RESULT               ; raw handle -> RESULT byte 0
      ldx     #0
      stx     LIB_RESULT+1
      stx     LIB_RESULT+2
      stx     LIB_RESULT+3
      cmp     #MSPRITE_INVALID_HANDLE
      beq     @fail
      lda     #LERR_OK
      sta     LIB_STATUS
      rts
@fail:
      lda     #LERR_MSPRITE_FAIL
      sta     LIB_STATUS
      rts

; finish_ms — A = driver result code (0 = OK). Publish to LIB_RESULT byte 0
; (bytes 1-3 zeroed); A!=0 -> LERR_MSPRITE_FAIL, A==0 -> LERR_OK.
finish_ms:
      sta     LIB_RESULT               ; raw result code -> RESULT byte 0
      ldx     #0
      stx     LIB_RESULT+1
      stx     LIB_RESULT+2
      stx     LIB_RESULT+3
      cmp     #MSPRITE_RESULT_OK
      bne     @fail
      lda     #LERR_OK
      sta     LIB_STATUS
      rts
@fail:
      lda     #LERR_MSPRITE_FAIL
      sta     LIB_STATUS
      rts

; --- $80 MS_SPAWN: allocate hw sprites for a visual descriptor -> object handle ---
; ARG0 = BYTES(ptr16 low word = descriptor address; len word ignored). The ptr16
; goes straight into MSPRITE_DESC_L/H; the descriptor stays resident in caller RAM
; (msprite_spawn reads its part count + part records via the pointer).
gfn_ms_spawn:
      lda     LIB_ARG0                 ; descriptor ptr low
      sta     MSPRITE_DESC_L
      lda     LIB_ARG0+1               ; descriptor ptr high
      sta     MSPRITE_DESC_H
      jsr     msprite_spawn
      jmp     finish_ms_spawn

; --- $81 MS_DESTROY: destroy object, free its hw sprites ---  handle:ARG0 -> A.
gfn_ms_destroy:
      lda     LIB_ARG0
      jsr     msprite_destroy
      jmp     finish_ms

; --- $82 MS_SHOW: mark object visible + dirty ---  handle:ARG0 -> A.
gfn_ms_show:
      lda     LIB_ARG0
      jsr     msprite_show
      jmp     finish_ms

; --- $83 MS_HIDE: hide object, disable its hw sprites now ---  handle:ARG0 -> A.
gfn_ms_hide:
      lda     LIB_ARG0
      jsr     msprite_hide
      jmp     finish_ms

; --- $84 MS_POS: set object position ---
; handle:ARG0 -> A, x(s16):ARG1 -> NVR0L/NVR0H, y:ARG2 -> Y. msprite_set_pos
; consumes Y before selecting the handle, so load A last.
gfn_ms_pos:
      lda     LIB_ARG1                 ; x low  -> NVR0L
      sta     NVR0L
      lda     LIB_ARG1+1               ; x high -> NVR0H
      sta     NVR0H
      ldy     LIB_ARG2                 ; y      -> Y
      lda     LIB_ARG0                 ; handle -> A
      jsr     msprite_set_pos
      jmp     finish_ms

; --- $85 MS_FRAME: set object animation frame ---  handle:ARG0 -> A, frame:ARG1 -> X.
gfn_ms_frame:
      ldx     LIB_ARG1                 ; frame -> X
      lda     LIB_ARG0                 ; handle -> A
      jsr     msprite_set_frame
      jmp     finish_ms

; --- $86 MS_ANIM: attach an animation descriptor ---
; handle:ARG0 -> A, anim descriptor ptr16 = ARG1 BYTES low word -> MSPRITE_ANIM_L/H.
gfn_ms_anim:
      lda     LIB_ARG1                 ; anim ptr low  -> MSPRITE_ANIM_L
      sta     MSPRITE_ANIM_L
      lda     LIB_ARG1+1               ; anim ptr high -> MSPRITE_ANIM_H
      sta     MSPRITE_ANIM_H
      lda     LIB_ARG0                 ; handle -> A
      jsr     msprite_set_anim
      jmp     finish_ms

; --- $87 MS_PRIORITY: set shared sprite priority ---  handle:ARG0 -> A, pri:ARG1 -> X.
gfn_ms_priority:
      ldx     LIB_ARG1                 ; priority -> X
      lda     LIB_ARG0                 ; handle -> A
      jsr     msprite_set_priority
      jmp     finish_ms

; --- $88 MS_TRANSCOLOR: set shared transparent colour ---  handle:ARG0 -> A, color:ARG1 -> X.
gfn_ms_transcolor:
      ldx     LIB_ARG1                 ; transcolor -> X
      lda     LIB_ARG0                 ; handle -> A
      jsr     msprite_set_transcolor
      jmp     finish_ms

; --- $89 MS_TICK: advance every active object's animation timer ---  ().
gfn_ms_tick:
      jsr     msprite_tick
      jmp     finish_ms

; --- $8A MS_COMMIT: write all dirty visible objects to hw sprite registers ---  ().
gfn_ms_commit:
      jsr     msprite_commit
      jmp     finish_ms

; --- $8B MS_COMMIT_ONE: write one visible object to hw, even if not dirty ---  handle:ARG0 -> A.
gfn_ms_commit_one:
      lda     LIB_ARG0
      jsr     msprite_commit_one
      jmp     finish_ms

; =====================================================================
; $A0-$A9  image / mem domain (batch 4b.9). Drivers: vgc.s mem-I/O,
; fio.s GSAVE/GLOAD, nvg.s (all included below; dedup'd by .ifndef guards).
;
; THREE shapes (see libgraphics.inc for the full ABI):
;   - VGC byte I/O: VGC_P0 = space, VGC_P1/P2 = 16-bit addr, VGC_P3 = value
;     (write in / read OUT), VGC_P4 = autoinc (we clear it -> one byte). The
;     drivers self-wait via vgc_wait_cmd, so MEMWRITE/VPOKE end finish_ok_nowait;
;     MEMREAD/VPEEK publish VGC_P3 to LIB_RESULT (finish_result8).
;   - File GSAVE/GLOAD: marshal the BYTES filename into FIO_NAME/FIO_NAMELEN
;     (as GTEXT does), load FIO_GSPACE/GADDRL/H/GLENL/H, JSR fio_gsave/fio_gload.
;     A=0/1 -> finish_file (publish + map to LERR_FILE_FAIL).
;   - NVG load: marshal the filename, set the dest offset for the _AT variants,
;     JSR the nvg_load* driver. A=0/1 -> finish_image (LERR_IMAGE_FAIL).

; gfx_copy_name — copy a BYTES filename into FIO_NAME with FIO_NAMELEN set.
; Source pointer must already be in LIB_ZP (the module-private (zp),Y pointer)
; and the length in A. Clamps len to the FIO name buffer (GFX_FIO_NAME_LIMIT),
; mirroring the GTEXT marshal / fio_copy_name. RAM scratch only (LIB_ZP + X/Y).
gfx_copy_name:
      cmp     #GFX_FIO_NAME_LIMIT+1
      bcc     @lenok
      lda     #GFX_FIO_NAME_LIMIT
@lenok:
      sta     FIO_NAMELEN
      cmp     #0
      beq     @done                    ; empty name -> nothing to copy
      tax                              ; X = byte count
      ldy     #0
@loop:
      lda     (LIB_ZP),y
      sta     FIO_NAME,y
      iny
      dex
      bne     @loop
@done:
      rts

; finish_file — fio_gsave/gload result in A: publish to LIB_RESULT byte 0
; (bytes 1-3 zeroed); A!=0 -> LERR_FILE_FAIL, A==0 -> LERR_OK.
finish_file:
      sta     LIB_RESULT
      ldx     #0
      stx     LIB_RESULT+1
      stx     LIB_RESULT+2
      stx     LIB_RESULT+3
      cmp     #FIO_RESULT_OK
      bne     @fail
      lda     #LERR_OK
      sta     LIB_STATUS
      rts
@fail:
      lda     #LERR_FILE_FAIL
      sta     LIB_STATUS
      rts

; finish_image — nvg_load* result in A: publish to LIB_RESULT byte 0
; (bytes 1-3 zeroed); A!=0 -> LERR_IMAGE_FAIL, A==0 -> LERR_OK.
finish_image:
      sta     LIB_RESULT
      ldx     #0
      stx     LIB_RESULT+1
      stx     LIB_RESULT+2
      stx     LIB_RESULT+3
      cmp     #FIO_RESULT_OK
      bne     @fail
      lda     #LERR_OK
      sta     LIB_STATUS
      rts
@fail:
      lda     #LERR_IMAGE_FAIL
      sta     LIB_STATUS
      rts

; gfx_mem_prep — load space:A -> VGC_P0, addr16 from (LIB_ZP cell) ... no — the
; address comes from the ARG cells directly. Helper loads VGC_P1/P2 = 16-bit
; address from the ARG cell whose low/high bytes are in A (low) and X (high), and
; clears VGC_P4 (autoinc off). Space must already be in VGC_P0.
gfx_mem_addr:
      sta     VGC_P1                   ; addr low
      stx     VGC_P2                   ; addr high
      stz     VGC_P4                   ; autoinc off -> single byte
      rts

; --- $A0 MEMREAD: read one byte from a VGC space (reporter) ---
; space:ARG0 -> P0, addr:ARG1 -> P1/P2; vgc_mem_read returns the byte in P3.
gfn_memread:
      lda     LIB_ARG0                 ; space -> P0
      sta     VGC_P0
      lda     LIB_ARG1                 ; addr low
      ldx     LIB_ARG1+1               ; addr high
      jsr     gfx_mem_addr
      jsr     vgc_mem_read
      lda     VGC_P3                   ; read byte
      jmp     finish_result8

; --- $A1 MEMWRITE: write one byte to a VGC space ---
; space:ARG0 -> P0, addr:ARG1 -> P1/P2, value:ARG2 -> P3; vgc_mem_write self-waits.
gfn_memwrite:
      lda     LIB_ARG0                 ; space -> P0
      sta     VGC_P0
      lda     LIB_ARG1                 ; addr low
      ldx     LIB_ARG1+1               ; addr high
      jsr     gfx_mem_addr
      lda     LIB_ARG2                 ; value -> P3
      sta     VGC_P3
      jsr     vgc_mem_write
      jmp     finish_ok_nowait

; --- $A2 VPEEK: read one byte from the gfx plane (reporter) ---
; offset:ARG0 -> P1/P2, space fixed to gfx; vgc_mem_read returns the byte in P3.
gfn_vpeek:
      lda     #GFX_MEMSPACE_GFX        ; space = gfx -> P0
      sta     VGC_P0
      lda     LIB_ARG0                 ; offset low
      ldx     LIB_ARG0+1               ; offset high
      jsr     gfx_mem_addr
      jsr     vgc_mem_read
      lda     VGC_P3
      jmp     finish_result8

; --- $A3 VPOKE: write one byte to the gfx plane ---
; offset:ARG0 -> P1/P2, value:ARG1 -> P3, space fixed to gfx.
gfn_vpoke:
      lda     #GFX_MEMSPACE_GFX        ; space = gfx -> P0
      sta     VGC_P0
      lda     LIB_ARG0                 ; offset low
      ldx     LIB_ARG0+1               ; offset high
      jsr     gfx_mem_addr
      lda     LIB_ARG1                 ; value -> P3
      sta     VGC_P3
      jsr     vgc_mem_write
      jmp     finish_ok_nowait

; gfx_load_gregs — load the FIO graphics registers from the GSAVE/GLOAD args:
; space:ARG0 -> FIO_GSPACE, addr:ARG1 -> FIO_GADDRL/H, len:ARG2 -> FIO_GLENL/H.
gfx_load_gregs:
      lda     LIB_ARG0
      sta     FIO_GSPACE
      lda     LIB_ARG1                 ; addr low
      sta     FIO_GADDRL
      lda     LIB_ARG1+1               ; addr high
      sta     FIO_GADDRH
      lda     LIB_ARG2                 ; len low
      sta     FIO_GLENL
      lda     LIB_ARG2+1               ; len high
      sta     FIO_GLENH
      rts

; gfx_name_from_arg3 — marshal the ARG3 BYTES filename into FIO_NAME/FIO_NAMELEN.
; ARG3 low word = ptr16 -> LIB_ZP; ARG3 high word low byte = len (clamped).
gfx_name_from_arg3:
      lda     LIB_ARG3                 ; name ptr16 -> LIB_ZP
      sta     LIB_ZP
      lda     LIB_ARG3+1
      sta     LIB_ZP+1
      lda     LIB_ARG3+2               ; len
      jmp     gfx_copy_name            ; -> FIO_NAME/FIO_NAMELEN

; --- $A4 GSAVE: save a VGC memory space region to a .gfx file ---
; space:ARG0, addr:ARG1, len:ARG2, BYTES name:ARG3 -> fio_gsave.
gfn_gsave:
      jsr     gfx_load_gregs
      jsr     gfx_name_from_arg3
      jsr     fio_gsave
      jmp     finish_file

; --- $A5 GLOAD: load a .gfx file into a VGC memory space region ---
; space:ARG0, addr:ARG1, len:ARG2, BYTES name:ARG3 -> fio_gload.
gfn_gload:
      jsr     gfx_load_gregs
      jsr     gfx_name_from_arg3
      jsr     fio_gload
      jmp     finish_file

; gfx_name_from_arg0 — marshal the ARG0 BYTES filename into FIO_NAME/FIO_NAMELEN.
gfx_name_from_arg0:
      lda     LIB_ARG0                 ; name ptr16 -> LIB_ZP
      sta     LIB_ZP
      lda     LIB_ARG0+1
      sta     LIB_ZP+1
      lda     LIB_ARG0+2               ; len
      jmp     gfx_copy_name

; gfx_argptr_to_zp — copy a CPU pointer (ARG0 low word) into the FIO named-arg
; ZP cells (NVG_NAMEPTR_L/H) and set NVG_NAMELEN from ARG0 high word low byte.
; Used by the NVG _NAMED variants which self-copy the name via fio_copy_name.
gfx_nvg_named_args:
      lda     LIB_ARG0                 ; ptr low
      sta     NVG_NAMEPTR_L
      lda     LIB_ARG0+1               ; ptr high
      sta     NVG_NAMEPTR_H
      lda     LIB_ARG0+2               ; len
      sta     NVG_NAMELEN
      rts

; --- $A6 NVGLOAD: clear gfx + decode the named NVG file at offset 0 ---
; BYTES name:ARG0 -> FIO_NAME/FIO_NAMELEN; nvg_load.
gfn_nvgload:
      jsr     gfx_name_from_arg0
      jsr     nvg_load
      jmp     finish_image

; --- $A7 NVGLOAD_AT: clear gfx + decode at dest offset NVG_ADDRL/H ---
; BYTES name:ARG0 -> FIO_NAME/FIO_NAMELEN, dest:ARG1 -> NVG_ADDRL/H; nvg_load_at.
gfn_nvgload_at:
      jsr     gfx_name_from_arg0
      lda     LIB_ARG1                 ; dest low
      sta     NVG_ADDRL
      lda     LIB_ARG1+1               ; dest high
      sta     NVG_ADDRH
      jsr     nvg_load_at
      jmp     finish_image

; --- $A8 NVGLOAD_NAMED: named-arg path (driver self-copies the filename) ---
; BYTES name:ARG0 -> NVG_NAMEPTR_L/H + NVG_NAMELEN; nvg_load_named.
gfn_nvgload_named:
      jsr     gfx_nvg_named_args
      jsr     nvg_load_named
      jmp     finish_image

; --- $A9 NVGLOAD_NAMED_AT: named-arg path + dest offset ---
; BYTES name:ARG0 -> NVG_NAMEPTR_L/H + NVG_NAMELEN, dest:ARG1 -> NVG_ADDRL/H.
gfn_nvgload_named_at:
      jsr     gfx_nvg_named_args
      lda     LIB_ARG1                 ; dest low
      sta     NVG_ADDRL
      lda     LIB_ARG1+1               ; dest high
      sta     NVG_ADDRH
      jsr     nvg_load_named_at
      jmp     finish_image

; =====================================================================
; $C0-$C7  anim domain (batch 4b.11). Driver: anim.s (included below; it pulls
; dma.s/fio.s/pager.s/msprite.s, all dedup'd by the .ifndef guards). State lives
; in the module-owned ANIM_* BSS block (track table + descriptor/target/shape-
; load pseudo-registers), persistent across lib_calls and assumed zeroed at cold
; boot — exactly as msprite (no init fn is required, though ANIM_INIT is exposed).
;
; Result mapping (see libgraphics.inc): START returns A = track handle ($FF on
; failure) -> finish_anim_start (publish + map $FF -> LERR_ANIM_FAIL). Every other
; op returns A = result code (0 = OK) -> finish_anim (publish + map A!=0 ->
; LERR_ANIM_FAIL). None issue a VGC command on the wrapper's behalf (the shape
; loaders self-wait inside the driver), so no wrapper-side command wait.

; finish_anim_start — A = track handle. Publish to LIB_RESULT byte 0 (bytes 1-3
; zeroed); $FF -> LERR_ANIM_FAIL, any valid handle -> LERR_OK.
finish_anim_start:
      sta     LIB_RESULT               ; raw handle -> RESULT byte 0
      ldx     #0
      stx     LIB_RESULT+1
      stx     LIB_RESULT+2
      stx     LIB_RESULT+3
      cmp     #ANIM_INVALID_HANDLE
      beq     @fail
      lda     #LERR_OK
      sta     LIB_STATUS
      rts
@fail:
      lda     #LERR_ANIM_FAIL
      sta     LIB_STATUS
      rts

; finish_anim — A = driver result code (0 = OK). Publish to LIB_RESULT byte 0
; (bytes 1-3 zeroed); A!=0 -> LERR_ANIM_FAIL, A==0 -> LERR_OK.
finish_anim:
      sta     LIB_RESULT               ; raw result code -> RESULT byte 0
      ldx     #0
      stx     LIB_RESULT+1
      stx     LIB_RESULT+2
      stx     LIB_RESULT+3
      cmp     #ANIM_RESULT_OK
      bne     @fail
      lda     #LERR_OK
      sta     LIB_STATUS
      rts
@fail:
      lda     #LERR_ANIM_FAIL
      sta     LIB_STATUS
      rts

; --- $C0 ANIM_INIT: clear the track table ---  (). anim_init returns A=0.
gfn_anim_init:
      jsr     anim_init
      jmp     finish_anim

; --- $C1 ANIM_START: attach a descriptor to a target -> track handle ---
; BYTES desc:ARG0 ptr16 -> ANIM_DESC_L/H; target:ARG1 -> ANIM_TARGET;
; type:ARG2 -> ANIM_TARGET_TYPE. The descriptor stays resident in caller RAM.
gfn_anim_start:
      lda     LIB_ARG0                 ; descriptor ptr low
      sta     ANIM_DESC_L
      lda     LIB_ARG0+1               ; descriptor ptr high
      sta     ANIM_DESC_H
      lda     LIB_ARG1                 ; target (sprite index / msprite handle)
      sta     ANIM_TARGET
      lda     LIB_ARG2                 ; target type (0 sprite / 1 msprite)
      sta     ANIM_TARGET_TYPE
      jsr     anim_start
      jmp     finish_anim_start

; --- $C2 ANIM_STOP: stop+release one track ---  handle:ARG0 -> A.
gfn_anim_stop:
      lda     LIB_ARG0
      jsr     anim_stop
      jmp     finish_anim

; --- $C3 ANIM_TICK: advance every running track ---  (). anim_tick returns A=0.
gfn_anim_tick:
      jsr     anim_tick
      jmp     finish_anim

; --- $C4 ANIM_TICK_ONE: advance one running track ---  handle:ARG0 -> A.
gfn_anim_tick_one:
      lda     LIB_ARG0
      jsr     anim_tick_one
      jmp     finish_anim

; --- $C5 ANIM_SET_FRAME: set one track to a frame index ---
; handle:ARG0 -> A, frame:ARG1 -> X. anim_set_frame consumes X before selecting
; the handle, so load A last.
gfn_anim_set_frame:
      ldx     LIB_ARG1                 ; frame -> X
      lda     LIB_ARG0                 ; handle -> A
      jsr     anim_set_frame
      jmp     finish_anim

; --- $C6 ANIM_LOAD_XRAM: DMA shape slots from XRAM into VGC sprite shape RAM ---
; shape:ARG0 -> ANIM_SHAPE, count:ARG1 -> ANIM_COUNT, xaddr24:ARG2 -> ANIM_XADDRL/M/H.
gfn_anim_load_xram:
      lda     LIB_ARG0                 ; dest shape slot
      sta     ANIM_SHAPE
      lda     LIB_ARG1                 ; slot count
      sta     ANIM_COUNT
      lda     LIB_ARG2                 ; xram src offset (24-bit)
      sta     ANIM_XADDRL
      lda     LIB_ARG2+1
      sta     ANIM_XADDRM
      lda     LIB_ARG2+2
      sta     ANIM_XADDRH
      jsr     anim_load_xram_shapes
      jmp     finish_anim

; --- $C7 ANIM_LOAD_DISK: stream shape slots from a file slice into shape RAM ---
; shape:ARG0 -> ANIM_SHAPE, count:ARG1 -> ANIM_COUNT, file24:ARG2 -> ANIM_FILEL/M/H,
; BYTES name:ARG3 ptr16 -> ANIM_NAMEPTR_L/H + ANIM_NAMELEN (name stays in caller
; RAM; the driver self-copies it through the pager).
gfn_anim_load_disk:
      lda     LIB_ARG0                 ; dest shape slot
      sta     ANIM_SHAPE
      lda     LIB_ARG1                 ; slot count
      sta     ANIM_COUNT
      lda     LIB_ARG2                 ; file src offset (24-bit)
      sta     ANIM_FILEL
      lda     LIB_ARG2+1
      sta     ANIM_FILEM
      lda     LIB_ARG2+2
      sta     ANIM_FILEH
      lda     LIB_ARG3                 ; name ptr low
      sta     ANIM_NAMEPTR_L
      lda     LIB_ARG3+1               ; name ptr high
      sta     ANIM_NAMEPTR_H
      lda     LIB_ARG3+2               ; name length
      sta     ANIM_NAMELEN
      jsr     anim_load_disk_shapes
      jmp     finish_anim

; =====================================================================
; $D0-$DA  tween domain (batch 4b.11). Driver: tween.s (included below; pure
; 16-bit easing math, no hardware, no extra deps). State lives in the module-
; owned TWEEN_* BSS cells. Every op is reporter-style: marshal the five input
; cells from the ARG cells, JSR the driver entry, then pack the readbacks into
; LIB_RESULT and set OK (the math never fails). See libgraphics.inc for the ABI.

; tween_load_args — marshal the five tween input cells from the ARG cells:
;   ARG0 low word -> TWEEN_STARTL/H, ARG1 low word -> TWEEN_ENDL/H,
;   ARG2 byte0 -> TWEEN_DURATION, ARG2 byte1 -> TWEEN_MODE,
;   ARG3 byte0 -> TWEEN_FRAME. RAM scratch only (driver BSS cells).
tween_load_args:
      lda     LIB_ARG0                 ; start low
      sta     TWEEN_STARTL
      lda     LIB_ARG0+1               ; start high
      sta     TWEEN_STARTH
      lda     LIB_ARG1                 ; end low
      sta     TWEEN_ENDL
      lda     LIB_ARG1+1               ; end high
      sta     TWEEN_ENDH
      lda     LIB_ARG2                 ; duration
      sta     TWEEN_DURATION
      lda     LIB_ARG2+1               ; mode (don't-care for fixed-curve variants)
      sta     TWEEN_MODE
      lda     LIB_ARG3                 ; frame
      sta     TWEEN_FRAME
      rts

; finish_tween — pack the tween readbacks into LIB_RESULT (4-byte LE) then OK:
;   byte0 = TWEEN_VALUEL, byte1 = TWEEN_VALUEH, byte2 = TWEEN_DONE,
;   byte3 = TWEEN_PROGRESS. (TWEEN_EASE stays available via a direct cell peek.)
finish_tween:
      lda     TWEEN_VALUEL
      sta     LIB_RESULT
      lda     TWEEN_VALUEH
      sta     LIB_RESULT+1
      lda     TWEEN_DONE
      sta     LIB_RESULT+2
      lda     TWEEN_PROGRESS
      sta     LIB_RESULT+3
      jmp     finish_ok_nowait

; --- $D0 TWEEN_BEGIN: reset frame/done, set VALUE := START ---
; tween_begin reads START/END/DURATION; we load all five inputs uniformly.
gfn_tween_begin:
      jsr     tween_load_args
      jsr     tween_begin
      jmp     finish_tween

; --- $D1 TWEEN_EVAL: evaluate the current frame using TWEEN_MODE ---
gfn_tween_eval:
      jsr     tween_load_args
      jsr     tween_eval
      jmp     finish_tween

; --- $D2 TWEEN_EVAL_LINEAR ---
gfn_tween_eval_linear:
      jsr     tween_load_args
      jsr     tween_eval_linear
      jmp     finish_tween

; --- $D3 TWEEN_EVAL_EASE_IN ---
gfn_tween_eval_ease_in:
      jsr     tween_load_args
      jsr     tween_eval_ease_in
      jmp     finish_tween

; --- $D4 TWEEN_EVAL_EASE_OUT ---
gfn_tween_eval_ease_out:
      jsr     tween_load_args
      jsr     tween_eval_ease_out
      jmp     finish_tween

; --- $D5 TWEEN_EVAL_EASE_IN_OUT ---
gfn_tween_eval_ease_in_out:
      jsr     tween_load_args
      jsr     tween_eval_ease_in_out
      jmp     finish_tween

; --- $D6 TWEEN_STEP: advance one frame then evaluate using TWEEN_MODE ---
gfn_tween_step:
      jsr     tween_load_args
      jsr     tween_step
      jmp     finish_tween

; --- $D7 TWEEN_STEP_LINEAR ---
gfn_tween_step_linear:
      jsr     tween_load_args
      jsr     tween_step_linear
      jmp     finish_tween

; --- $D8 TWEEN_STEP_EASE_IN ---
gfn_tween_step_ease_in:
      jsr     tween_load_args
      jsr     tween_step_ease_in
      jmp     finish_tween

; --- $D9 TWEEN_STEP_EASE_OUT ---
gfn_tween_step_ease_out:
      jsr     tween_load_args
      jsr     tween_step_ease_out
      jmp     finish_tween

; --- $DA TWEEN_STEP_EASE_IN_OUT ---
gfn_tween_step_ease_in_out:
      jsr     tween_load_args
      jsr     tween_step_ease_in_out
      jmp     finish_tween

; =====================================================================
; $B0-$B2  turtle-render domain (batch 4c.2-2). The turtle is a 16x16
; color-keyed virtual sprite (an upward triangle icon) drawn on the gfx plane
; with Amiga-BOB save/restore of the background. The module owns the source/
; rotated/saved-bg buffers + bg bookkeeping in its BSS (declared below). This is
; a faithful port of draw_turtle_sprite / erase_turtle_sprite / the init shape
; copy from novalogo/extension.s, but the buffers live in the module's own BSS
; band and DRAW reads the center from the mailbox (ARG0=x, ARG1=y, ARG2=angle)
; instead of the resident TURTLE_* cells. The vsprite driver helpers
; (vsprite_gfx_save_bg/restore_bg/rotate_blit_keyed) are linked from vsprite.s
; (included below). All loop/temp state is RAM (module BSS) — the module runs
; from write-protected ROM.
TURTLE_SPR_HALF = 8
TURTLE_MAX_X    = 304            ; 320 - 16
TURTLE_MAX_Y    = 184            ; 200 - 16
TURTLE_COL_WHITE = 1             ; Nova palette white index

; cfg_turtle_vsprite — configure the VSPRITE_* cells for the 16x16 turtle.
; ORIG/ROT/BG all point at the module-BSS buffers (all CPU RAM, bank-independent
; — the PROVEN path). Mirrors configure_turtle_vsprite (extension.s:666-705).
cfg_turtle_vsprite:
      lda     #16
      sta     VSPRITE_WIDTHL
      stz     VSPRITE_WIDTHH
      sta     VSPRITE_HEIGHTL
      stz     VSPRITE_HEIGHTH
      sta     VSPRITE_ORIGSTRL
      stz     VSPRITE_ORIGSTRH
      sta     VSPRITE_ROTSTRL
      stz     VSPRITE_ROTSTRH
      sta     VSPRITE_BGSTRL
      stz     VSPRITE_BGSTRH

      lda     #BLT_SPACE_CPU
      sta     VSPRITE_ORIGSPACE
      sta     VSPRITE_ROTSPACE
      sta     VSPRITE_BGSPACE

      lda     #<turtle_source_shape
      sta     VSPRITE_ORIGADDRL
      lda     #>turtle_source_shape
      sta     VSPRITE_ORIGADDRM
      stz     VSPRITE_ORIGADDRH

      lda     #<turtle_rotated_shape
      sta     VSPRITE_ROTADDRL
      lda     #>turtle_rotated_shape
      sta     VSPRITE_ROTADDRM
      stz     VSPRITE_ROTADDRH

      lda     #<turtle_saved_bg
      sta     VSPRITE_BGADDRL
      lda     #>turtle_saved_bg
      sta     VSPRITE_BGADDRM
      stz     VSPRITE_BGADDRH

      stz     VSPRITE_COLORKEY
      lda     #VSPRITE_FLAG_COLORKEY
      sta     VSPRITE_FLAGS
      rts

; set_turtle_pos — center the 16x16 stamp on (x,y) read from ARG0/ARG1 and clamp
; to the plane. Verbatim port of set_turtle_vsprite_pos (extension.s:710-764)
; with the center taken from the mailbox (LIB_ARG0 = x lo/hi, LIB_ARG1 = y lo/hi)
; and the clamp scratch (tmp0/tmp1) in module BSS. Result -> VSPRITE_XL/XH/Y.
set_turtle_pos:
      sec
      lda     LIB_ARG0                 ; x low
      sbc     #TURTLE_SPR_HALF
      sta     turtle_tmp0
      lda     LIB_ARG0+1               ; x high
      sbc     #0
      sta     turtle_tmp1
      bmi     @x_zero
      cmp     #>TURTLE_MAX_X
      bcc     @x_ok
      bne     @x_max
      lda     turtle_tmp0
      cmp     #<TURTLE_MAX_X
      bcc     @x_ok
      beq     @x_ok
@x_max:
      lda     #<TURTLE_MAX_X
      sta     VSPRITE_XL
      lda     #>TURTLE_MAX_X
      sta     VSPRITE_XH
      bra     @y
@x_zero:
      stz     VSPRITE_XL
      stz     VSPRITE_XH
      bra     @y
@x_ok:
      lda     turtle_tmp0
      sta     VSPRITE_XL
      lda     turtle_tmp1
      sta     VSPRITE_XH
@y:
      ; Y max (184) is single-byte, so the high borrow stays in A and is tested
      ; in place (bmi/bne) rather than stashed like the two-byte X clamp above.
      sec
      lda     LIB_ARG1                 ; y low
      sbc     #TURTLE_SPR_HALF
      sta     turtle_tmp0
      lda     LIB_ARG1+1               ; y high
      sbc     #0
      bmi     @y_zero
      bne     @y_max
      lda     turtle_tmp0
      cmp     #(TURTLE_MAX_Y + 1)
      bcc     @y_ok
@y_max:
      lda     #TURTLE_MAX_Y
      sta     VSPRITE_Y
      rts
@y_zero:
      stz     VSPRITE_Y
      rts
@y_ok:
      lda     turtle_tmp0
      sta     VSPRITE_Y
      rts

; turtle_do_erase — restore the saved background if one is pending, then clear
; the flag. Shared by ERASE and the erase-old step of DRAW. Mirrors
; erase_turtle_sprite (extension.s:788-801).
turtle_do_erase:
      lda     turtle_bg_saved
      beq     @done
      jsr     cfg_turtle_vsprite
      lda     turtle_bg_x_lo
      sta     VSPRITE_XL
      lda     turtle_bg_x_hi
      sta     VSPRITE_XH
      lda     turtle_bg_y
      sta     VSPRITE_Y
      jsr     vsprite_gfx_restore_bg
      stz     turtle_bg_saved
@done:
      rts

; --- $B0 TURTLE_INIT: install the built-in icon + reset bg bookkeeping. ---
; Copies the 256-byte turtle_icon ROM constant into turtle_source_shape and zeros
; turtle_bg_saved (so a stale saved-background is never restored onto a freshly
; cleared plane). One-time setup AND the CLEARSCREEN re-init hook. Always OK.
gfn_turtle_init:
      ldx     #0
@copy:
      lda     turtle_icon,x
      sta     turtle_source_shape,x
      inx
      bne     @copy                    ; exactly 256 bytes
      stz     turtle_bg_saved
      jmp     finish_ok_nowait

; --- $B1 TURTLE_DRAW: erase-old + save-bg + rotate-blit-keyed. ---
; x:ARG0 s16 center, y:ARG1 s16 center, angle:ARG2 byte0 u8. Faithful port of
; draw_turtle_sprite (extension.s:806-827): restore any pending bg, configure the
; vsprite cells, center+clamp the stamp, record the stamp position, save the new
; bg (bail to finish_vs on failure — no blit, no bg_saved), then stamp the angle
; and rotate-blit-keyed. PUMPED on the test side (rotate-blit waits on VGC_FRAME).
gfn_turtle_draw:
      jsr     turtle_do_erase          ; restore previously-saved bg (if any)
      jsr     cfg_turtle_vsprite
      jsr     set_turtle_pos           ; center+clamp -> VSPRITE_XL/XH/Y
      lda     VSPRITE_XL
      sta     turtle_bg_x_lo
      lda     VSPRITE_XH
      sta     turtle_bg_x_hi
      lda     VSPRITE_Y
      sta     turtle_bg_y
      jsr     vsprite_gfx_save_bg
      cmp     #VSPRITE_RESULT_OK
      beq     @saved                   ; save-bg ok -> stamp the turtle
      jmp     finish_vs                ; save-bg failed: publish A, don't blit
@saved:
      lda     #$01
      sta     turtle_bg_saved
      lda     LIB_ARG2                 ; angle byte 0
      sta     VSPRITE_ROTANGLE
      jsr     vsprite_gfx_rotate_blit_keyed
      jmp     finish_vs

; --- $B2 TURTLE_ERASE: restore the saved bg (no-op if nothing saved). ---
; Always OK (if nothing to erase, still OK). Port of erase_turtle_sprite.
gfn_turtle_erase:
      jsr     turtle_do_erase
      jmp     finish_ok_nowait

; =====================================================================
; $B3 TURTLE_OP — the whole NovaLogo turtle command engine, INSIDE the module
; (batch 4c.2-3-i). Op-dispatched: ARG2 byte0 = a turtle command id (TOP_*,
; mirroring the legacy EXT_CMD_* values). Value args ride in ARG0/ARG1 as Logo
; 16.8 fixed point — cell byte0 = FRAC, byte1 = LO, byte2 = HI (the layout the
; foundation adapter marshals; see libgraphics.inc GFN_TURTLE_OP). The move-math
; (sincos / 16x16 multiply / 24-bit accumulate) is a FAITHFUL port of
; novalogo/turtle.s (itself faithful to novalogo/extension.s); only the render
; path differs — it calls the module's OWN vgc.s helpers + $B0/$B1/$B2 render
; subs directly instead of nesting a lib_call.
;
; Turtle state stays at $9F00 (shared RAM, accessible while the module is paged).
; Commands set LIB_STATUS = LERR_OK; reporters write a Logo 16.8 value to the
; LIB_RESULT low word and set OK.
; =====================================================================

; --- Turtle state at $9F00 (shared RAM; tests read X/Y/HEADING here). Mirrors
;     novalogo/extension.s:75-93 / turtle.s:50-64. ---
TURTLE_STATE_BASE  = $9F00
TURTLE_X_FRAC      = TURTLE_STATE_BASE + 0
TURTLE_X_LO        = TURTLE_STATE_BASE + 1
TURTLE_X_HI        = TURTLE_STATE_BASE + 2
TURTLE_Y_FRAC      = TURTLE_STATE_BASE + 3
TURTLE_Y_LO        = TURTLE_STATE_BASE + 4
TURTLE_Y_HI        = TURTLE_STATE_BASE + 5
TURTLE_HEADING_LO  = TURTLE_STATE_BASE + 6
TURTLE_HEADING_HI  = TURTLE_STATE_BASE + 7
TURTLE_PEN         = TURTLE_STATE_BASE + 8   ; 0 = down, 1 = up
TURTLE_SHOWN       = TURTLE_STATE_BASE + 9
TURTLE_COLOR       = TURTLE_STATE_BASE + 10
TURTLE_SPRITE      = TURTLE_STATE_BASE + 11
TURTLE_INITED      = TURTLE_STATE_BASE + 12
; $9F0D-$9F10 = bg bookkeeping owned by the module render BSS — leave it.
TURTLE_GFX_VISIBLE = TURTLE_STATE_BASE + 17  ; 1 = split/full graphics active

; --- Turtle constants ---
TURTLE_CENTER_X    = 160         ; center of 320-wide graphics
TURTLE_CENTER_Y    = 80          ; center of the split graphics viewport
TURTLE_SPLIT_Y     = 160         ; copper split: gfx rows 0-159, text below
TURTLE_MODE_GFXSPR = 3           ; VGC mode: graphics + sprites
TURTLE_MODE_TEXT   = 0           ; VGC mode: text only

; --- Logo value-type + cons-cell offsets, for SETPOS [x y]. ---
;     (novalogo/ext_iface.inc isn't pulled into the module — define locally.)
TVAL_LIST          = $02
TVAL_NUMBER        = $00
TCONS_CAR_TYPE     = 1
TCONS_CAR_HI       = 2
TCONS_CAR_LO       = 3
TCONS_CDR_LO       = 5
TCONS_CDR_HI       = 6

; --- ARG cell sub-byte aliases (Logo 16.8: [0]=FRAC, [1]=LO, [2]=HI). ---
TA0_FRAC = LIB_ARG0+0
TA0_LO   = LIB_ARG0+1
TA0_HI   = LIB_ARG0+2
TA1_FRAC = LIB_ARG1+0
TA1_LO   = LIB_ARG1+1
TA1_HI   = LIB_ARG1+2

; --- $B3 dispatcher: read op from ARG2 byte0, route via an internal RTS-trick
;     jump table to the per-op handler. Op ids are EXT_CMD values ($10..$29); we
;     subtract $10 to index a dense 26-entry table. Out-of-range op -> OK no-op
;     (commands always OK; an unknown op is a harmless no-op). ---
gfn_turtle_op:
      cld                              ; turtle math is binary, never BCD (ext_entry)
      lda     LIB_ARG2                 ; op id (byte 0)
      sec
      sbc     #$10                     ; TOP_FD ($10) -> index 0
      bcc     @oknoop                  ; op < $10 -> no-op OK
      cmp     #(top_table_end - top_table) / 2
      bcs     @oknoop                  ; op past the table -> no-op OK
      asl                              ; *2 for the word table (max index 25 -> $32, fits 8-bit)
      tax
      lda     top_table+1,x            ; handler-1 hi
      pha
      lda     top_table,x              ; handler-1 lo
      pha
      rts                              ; jump to handler (each ends in jmp finish_*)
@oknoop:
      jmp     finish_ok_nowait

top_table:
      .word   t_fd-1                   ; $10 FD
      .word   t_bk-1                   ; $11 BK
      .word   t_rt-1                   ; $12 RT
      .word   t_lt-1                   ; $13 LT
      .word   t_cs-1                   ; $14 CS
      .word   t_pu-1                   ; $15 PU
      .word   t_pd-1                   ; $16 PD
      .word   t_st-1                   ; $17 ST
      .word   t_ht-1                   ; $18 HT
      .word   t_home-1                 ; $19 HOME
      .word   t_ts-1                   ; $1A TS
      .word   t_ss-1                   ; $1B SS
      .word   t_fs-1                   ; $1C FS
      .word   t_setxy-1                ; $1D SETXY
      .word   t_setx-1                 ; $1E SETX
      .word   t_sety-1                 ; $1F SETY
      .word   t_seth-1                 ; $20 SETH
      .word   t_xcor-1                 ; $21 XCOR (reporter)
      .word   t_ycor-1                 ; $22 YCOR (reporter)
      .word   t_heading-1              ; $23 HEADING (reporter)
      .word   t_pendownp-1             ; $24 PENDOWN? (reporter)
      .word   t_shownp-1               ; $25 SHOWN? (reporter)
      .word   t_setpc-1                ; $26 SETPC
      .word   t_setbg-1                ; $27 SETBG
      .word   t_towards-1              ; $28 TOWARDS (reporter)
      .word   t_setpos-1               ; $29 SETPOS
top_table_end:

; =====================================================================
; turtle_init_state — initialize persistent turtle state, then install the icon
;   into the module source buffer + reset bg bookkeeping (via gfn_turtle_init).
;   Mirrors turtle_init_state (turtle.s:98-119) / turtle_init (extension.s:562).
; =====================================================================
turtle_init_state:
      lda     #TURTLE_CENTER_X
      sta     TURTLE_X_LO
      stz     TURTLE_X_HI
      stz     TURTLE_X_FRAC
      lda     #TURTLE_CENTER_Y
      sta     TURTLE_Y_LO
      stz     TURTLE_Y_HI
      stz     TURTLE_Y_FRAC
      stz     TURTLE_HEADING_LO
      stz     TURTLE_HEADING_HI
      stz     TURTLE_PEN               ; pen down
      lda     #$01
      sta     TURTLE_SHOWN
      lda     #TURTLE_COL_WHITE
      sta     TURTLE_COLOR
      stz     TURTLE_SPRITE
      lda     #$01
      sta     TURTLE_INITED
      jmp     gfn_turtle_init          ; install icon + reset bg (sets STATUS OK; we ignore)

; =====================================================================
; turtle_enter_split — copper split: gfx+sprites top, text below; mark visible.
;   Direct call into the module's copper_split_mode (the GFN_COPPER_SPLIT body).
;   NOTE (deviation): the legacy ext_ss also runs prepare_split_text to carve the
;   text window + park the cursor. That touches runtime-owned TEXTWIN/cursor state
;   the module does not own, so it is intentionally NOT ported here — the gfx-mode
;   enable (the part that makes drawing land) is the module's concern; the text
;   window split is the foundation adapter's. The gfx plane (what the tests read)
;   is written regardless of display mode.
; =====================================================================
turtle_enter_split:
      stz     VGC_P0                   ; idx = 0
      lda     #TURTLE_SPLIT_Y
      sta     VGC_P1                   ; splitY = 160
      lda     #TURTLE_MODE_GFXSPR
      sta     VGC_P2                   ; mode0 = gfx+sprites (top)
      lda     #TURTLE_MODE_TEXT
      sta     VGC_P3                   ; mode1 = text (bottom)
      jsr     copper_split_mode
      lda     #$01
      sta     TURTLE_GFX_VISIBLE
      rts

; =====================================================================
; ensure_gfx_mode — init the turtle if needed; if still in full text mode, auto
;   switch to split screen so drawing is visible + show the turtle there.
;   Mirrors ensure_gfx_mode (turtle.s:126-136).
; =====================================================================
ensure_gfx_mode:
      lda     TURTLE_INITED
      bne     @check_mode
      jsr     turtle_init_state
@check_mode:
      lda     TURTLE_GFX_VISIBLE
      bne     @done
      jsr     turtle_enter_split
      jsr     draw_turtle
@done:
      rts

; =====================================================================
; turtle_save_old — snapshot current integer position to BSS old_x/y (the line
;   start for the post-move pen line). Mirrors turtle_save_old (turtle.s:160).
; =====================================================================
turtle_save_old:
      lda     TURTLE_X_LO
      sta     turtle_old_x_lo
      lda     TURTLE_X_HI
      sta     turtle_old_x_hi
      lda     TURTLE_Y_LO
      sta     turtle_old_y_lo
      lda     TURTLE_Y_HI
      sta     turtle_old_y_hi
      rts

; =====================================================================
; draw_turtle — redraw the rotated turtle at the current position via the
;   module's own $B1 render sub. gfn_turtle_draw erases the previous stamp
;   internally, so do NOT pre-erase. No-op when hidden. Mirrors draw_turtle
;   (turtle.s:176-192).
;
;   gfn_turtle_draw consumes the mailbox cells (ARG0/ARG1 lo+hi, ARG2 byte0),
;   so draw_turtle SAVES the caller's value-arg bytes first and RESTORES them
;   afterward — it is called both as a final render step AND as a pre-step inside
;   ensure_gfx_mode (first gfx entry), where it would otherwise clobber the value
;   args a move/set handler has not yet read. Keeping draw_turtle args-transparent
;   removes that ordering hazard for every handler. (The legacy draw_turtle_sprite
;   was immune because its args lived in ZP it never touched.)
; =====================================================================
draw_turtle:
      lda     TURTLE_SHOWN
      beq     @done
      ; snapshot the value-arg bytes gfn_turtle_draw overwrites
      lda     LIB_ARG0+0
      sta     t_save_a0_0
      lda     LIB_ARG0+1
      sta     t_save_a0_1
      lda     LIB_ARG1+0
      sta     t_save_a1_0
      lda     LIB_ARG1+1
      sta     t_save_a1_1
      lda     LIB_ARG2+0
      sta     t_save_a2_0
      ; marshal X/Y/angle and stamp the turtle
      lda     TURTLE_X_LO
      sta     LIB_ARG0+0
      lda     TURTLE_X_HI
      sta     LIB_ARG0+1
      lda     TURTLE_Y_LO
      sta     LIB_ARG1+0
      lda     TURTLE_Y_HI
      sta     LIB_ARG1+1
      jsr     turtle_heading_u8        ; A = NDK angle byte
      sta     LIB_ARG2+0
      jsr     gfn_turtle_draw          ; erase-old + save-bg + rotate-blit (sets STATUS)
      ; restore the caller's value-arg bytes
      lda     t_save_a0_0
      sta     LIB_ARG0+0
      lda     t_save_a0_1
      sta     LIB_ARG0+1
      lda     t_save_a1_0
      sta     LIB_ARG1+0
      lda     t_save_a1_1
      sta     LIB_ARG1+1
      lda     t_save_a2_0
      sta     LIB_ARG2+0
@done:
      rts

; =====================================================================
; turtle_render — shared post-move render: lift the old turtle, draw the pen line
;   (if pen down) old->new, then redraw the turtle at the new position. Reads BSS
;   old_x/y for the line start; TURTLE_X/Y for the end. Mirrors turtle_render
;   (turtle.s:206-246) but draws the line DIRECTLY via the module's vgc.s
;   (= draw_line, extension.s:627).
;
;   ORDER MATTERS: erase the old turtle FIRST, BEFORE the line. The turtle is a
;   gfx-plane BOB (save-bg/restore-bg), so its saved background under the old
;   footprint predates this line. If we drew the line first, the post-line
;   draw_turtle's internal restore-bg would write that stale background back over
;   the ~16px of fresh line passing under the old position — gapping the line at
;   every waypoint. The legacy ext_fd erased before draw_line for exactly this
;   reason (extension.s:273 erase precedes :398 line). With bg_saved cleared here,
;   draw_turtle's own internal erase then no-ops.
; =====================================================================
turtle_render:
      jsr     gfn_turtle_erase         ; lift the old turtle before the line (see ORDER note)
      lda     TURTLE_PEN
      bne     @no_line                 ; pen up ($01) -> skip line
      ; --- direct VGC line (draw_line, extension.s:627): color then endpoints ---
      jsr     vgc_wait_cmd
      lda     TURTLE_COLOR
      sta     VGC_P0
      jsr     vgc_gcolor
      jsr     vgc_wait_cmd
      lda     turtle_old_x_lo
      sta     VGC_P0
      lda     turtle_old_x_hi
      sta     VGC_P1
      lda     turtle_old_y_lo
      sta     VGC_P2
      lda     turtle_old_y_hi
      sta     VGC_P3
      lda     TURTLE_X_LO
      sta     VGC_P4
      lda     TURTLE_X_HI
      sta     VGC_P5
      lda     TURTLE_Y_LO
      sta     VGC_P6
      lda     TURTLE_Y_HI
      sta     VGC_P7
      jsr     vgc_line
      jsr     vgc_wait_cmd
@no_line:
      jmp     draw_turtle              ; tail: redraw turtle at the new position

; =====================================================================
; heading_mod360 — reduce TURTLE_HEADING to 0-359. (turtle.s:251)
; =====================================================================
heading_mod360:
@loop:
      lda     TURTLE_HEADING_HI
      beq     @done                    ; hi = 0 -> always < 360
      cmp     #>360                    ; hi > 1 -> definitely >= 360
      bne     @subtract
      lda     TURTLE_HEADING_LO
      cmp     #<360
      bcc     @done
@subtract:
      sec
      lda     TURTLE_HEADING_LO
      sbc     #<360
      sta     TURTLE_HEADING_LO
      lda     TURTLE_HEADING_HI
      sbc     #>360
      sta     TURTLE_HEADING_HI
      bra     @loop
@done:
      rts

; =====================================================================
; turtle_heading_u8 — Logo degrees -> NDK/blitter angle byte.
;   A = floor((heading * 256) / 360). Cardinals land exactly (90->64 etc).
;   (turtle.s:276 / extension.s:771) — division via the MATH copro.
; =====================================================================
turtle_heading_u8:
      stz     MATH_DIV_N_LO
      lda     TURTLE_HEADING_LO
      sta     MATH_DIV_N_1
      lda     TURTLE_HEADING_HI
      sta     MATH_DIV_N_2
      stz     MATH_DIV_N_HI
      lda     #<360
      sta     MATH_DIV_D_LO
      lda     #>360
      sta     MATH_DIV_D_HI            ; writing D_HI triggers the divide
      lda     MATH_RES0
      rts

; =====================================================================
; t_fd — FD: move forward by distance (ARG0 = 16.8 distance).
;   Faithful port of t_fd (turtle.s:293) / ext_fd (extension.s:262). Scratch in
;   module BSS (the render subs run inline, not via lib_call, so no clobber risk).
; =====================================================================
t_fd:
      ; Snapshot the distance (ARG0 16.8 integer part) to BSS up front so the
      ; move-math below reads it from a stable home. draw_turtle (reached via
      ; ensure_gfx_mode on first gfx-mode entry) is already args-transparent — it
      ; saves/restores LIB_ARG0/1/2 around its X/Y/angle use — so TA0 would survive
      ; regardless; this snapshot is independent defense-in-depth, not relied upon.
      lda     TA0_LO
      sta     t_dist_lo
      lda     TA0_HI
      sta     t_dist_hi

      jsr     ensure_gfx_mode
      jsr     turtle_save_old

      ; heading -> NDK angle -> sincos
      jsr     turtle_heading_u8
      sta     MATH_SINCOS_ANGLE        ; triggers sincos
      lda     MATH_RES0                ; sin (1.7 signed)
      sta     t_sin_val
      lda     MATH_RES1                ; cos (1.7 signed)
      sta     t_cos_val

      ; dx = distance * sin / 128, kept as 16.8 (frac:lo:hi) = result << 1
      lda     t_dist_lo
      sta     MATH_MUL16_A_LO
      lda     t_dist_hi
      sta     MATH_MUL16_A_HI
      lda     t_sin_val
      cmp     #127                     ; +1.0 is 127/128; use 128 so cardinals stay exact
      bne     :+
      lda     #128
:
      sta     MATH_MUL16_B_LO
      lda     t_sin_val
      ora     #$7F
      bmi     @sin_neg
      lda     #$00
      bra     @sin_ext
@sin_neg:
      lda     #$FF
@sin_ext:
      sta     MATH_MUL16_B_HI          ; triggers multiply
      lda     MATH_RES0
      asl
      sta     t_dx_frac
      lda     MATH_RES1
      rol
      sta     t_dx_lo
      lda     MATH_RES2
      rol
      sta     t_dx_hi

      ; raw_dy = distance * cos / 128 (16.8), then dy = -raw_dy
      lda     t_dist_lo
      sta     MATH_MUL16_A_LO
      lda     t_dist_hi
      sta     MATH_MUL16_A_HI
      lda     t_cos_val
      cmp     #127
      bne     :+
      lda     #128
:
      sta     MATH_MUL16_B_LO
      lda     t_cos_val
      ora     #$7F
      bmi     @cos_neg
      lda     #$00
      bra     @cos_ext
@cos_neg:
      lda     #$FF
@cos_ext:
      sta     MATH_MUL16_B_HI          ; triggers multiply
      lda     MATH_RES0
      asl
      sta     t_tmp1                   ; raw_dy frac
      lda     MATH_RES1
      rol
      sta     t_tmp2                   ; raw_dy lo
      lda     MATH_RES2
      rol
      sta     t_tmp3                   ; raw_dy hi
      ; dy = -raw_dy (negate 24-bit)
      sec
      lda     #0
      sbc     t_tmp1
      sta     t_dy_frac
      lda     #0
      sbc     t_tmp2
      sta     t_dy_lo
      lda     #0
      sbc     t_tmp3
      sta     t_dy_hi

      ; X += dx (24-bit fixed frac:lo:hi)
      clc
      lda     TURTLE_X_FRAC
      adc     t_dx_frac
      sta     TURTLE_X_FRAC
      lda     TURTLE_X_LO
      adc     t_dx_lo
      sta     TURTLE_X_LO
      lda     TURTLE_X_HI
      adc     t_dx_hi
      sta     TURTLE_X_HI
      ; Y += dy
      clc
      lda     TURTLE_Y_FRAC
      adc     t_dy_frac
      sta     TURTLE_Y_FRAC
      lda     TURTLE_Y_LO
      adc     t_dy_lo
      sta     TURTLE_Y_LO
      lda     TURTLE_Y_HI
      adc     t_dy_hi
      sta     TURTLE_Y_HI

      jsr     turtle_render            ; pen line (if down) + redraw
      jmp     finish_ok_nowait

; =====================================================================
; t_bk — BK: negate the 24-bit distance, then fall into FD. (turtle.s:404)
; =====================================================================
t_bk:
      sec
      lda     #0
      sbc     TA0_FRAC
      sta     TA0_FRAC
      lda     #0
      sbc     TA0_LO
      sta     TA0_LO
      lda     #0
      sbc     TA0_HI
      sta     TA0_HI
      jmp     t_fd

; =====================================================================
; t_rt — RT: turn right by degrees. (turtle.s:420)
; =====================================================================
t_rt:
      jsr     ensure_gfx_mode
      clc
      lda     TURTLE_HEADING_LO
      adc     TA0_LO
      sta     TURTLE_HEADING_LO
      lda     TURTLE_HEADING_HI
      adc     TA0_HI
      sta     TURTLE_HEADING_HI
      jsr     heading_mod360
      jsr     draw_turtle
      jmp     finish_ok_nowait

; =====================================================================
; t_lt — LT: turn left by degrees (add 360 if it goes negative). (turtle.s:435)
; =====================================================================
t_lt:
      jsr     ensure_gfx_mode
      sec
      lda     TURTLE_HEADING_LO
      sbc     TA0_LO
      sta     TURTLE_HEADING_LO
      lda     TURTLE_HEADING_HI
      sbc     TA0_HI
      sta     TURTLE_HEADING_HI
      bmi     @add360
      jsr     heading_mod360
      jsr     draw_turtle
      jmp     finish_ok_nowait
@add360:
      clc
      lda     TURTLE_HEADING_LO
      adc     #<360
      sta     TURTLE_HEADING_LO
      lda     TURTLE_HEADING_HI
      adc     #>360
      sta     TURTLE_HEADING_HI
      bmi     @add360
      jsr     heading_mod360
      jsr     draw_turtle
      jmp     finish_ok_nowait

; =====================================================================
; t_home — HOME: move to center, heading 0 (draw line if pen down). (turtle.s:462)
; =====================================================================
t_home:
      jsr     ensure_gfx_mode
      lda     TURTLE_INITED
      bne     @go
      jsr     turtle_init_state
      jsr     draw_turtle
      jmp     finish_ok_nowait
@go:
      jsr     turtle_save_old
      lda     #TURTLE_CENTER_X
      sta     TURTLE_X_LO
      stz     TURTLE_X_HI
      stz     TURTLE_X_FRAC
      lda     #TURTLE_CENTER_Y
      sta     TURTLE_Y_LO
      stz     TURTLE_Y_HI
      stz     TURTLE_Y_FRAC
      stz     TURTLE_HEADING_LO
      stz     TURTLE_HEADING_HI
      jsr     turtle_render
      jmp     finish_ok_nowait

; =====================================================================
; t_setxy — SETXY x y: move to (x,y), draw line if pen down. (turtle.s:485)
; =====================================================================
t_setxy:
      jsr     ensure_gfx_mode
      lda     TURTLE_INITED
      bne     @go
      jsr     turtle_init_state
@go:
      jsr     turtle_save_old
      lda     TA0_LO
      sta     TURTLE_X_LO
      lda     TA0_HI
      sta     TURTLE_X_HI
      stz     TURTLE_X_FRAC
      lda     TA1_LO
      sta     TURTLE_Y_LO
      lda     TA1_HI
      sta     TURTLE_Y_HI
      stz     TURTLE_Y_FRAC
      jsr     turtle_render
      jmp     finish_ok_nowait

; =====================================================================
; t_setpos — SETPOS [x y]: walk the cons list (ARG0 = list ptr), load x/y into
;   ARG0/ARG1 low words, then JMP setxy. (turtle.s:507 / extension.s:1108)
;   The cons cells live in heap RAM, deref'd via the module ZP pointer LIB_ZP.
; =====================================================================
t_setpos:
      jsr     ensure_gfx_mode
      lda     LIB_ARG0+0               ; list ptr low
      sta     LIB_ZP
      lda     LIB_ARG0+1               ; list ptr high
      sta     LIB_ZP+1
      ora     LIB_ZP
      beq     @ok                      ; nil list -> no-op OK
      ; first element -> X
      ldy     #TCONS_CAR_TYPE
      lda     (LIB_ZP),y
      cmp     #TVAL_NUMBER
      bne     @ok
      ldy     #TCONS_CAR_LO
      lda     (LIB_ZP),y
      sta     TA0_LO
      ldy     #TCONS_CAR_HI
      lda     (LIB_ZP),y
      sta     TA0_HI
      stz     TA0_FRAC
      ; cdr -> second cell
      ldy     #TCONS_CDR_LO
      lda     (LIB_ZP),y
      sta     t_tmp2
      ldy     #TCONS_CDR_HI
      lda     (LIB_ZP),y
      sta     t_tmp3
      ora     t_tmp2
      beq     @ok                      ; no second element -> bail (still OK)
      lda     t_tmp2
      sta     LIB_ZP
      lda     t_tmp3
      sta     LIB_ZP+1
      ldy     #TCONS_CAR_TYPE
      lda     (LIB_ZP),y
      cmp     #TVAL_NUMBER
      bne     @ok
      ldy     #TCONS_CAR_LO
      lda     (LIB_ZP),y
      sta     TA1_LO
      ldy     #TCONS_CAR_HI
      lda     (LIB_ZP),y
      sta     TA1_HI
      stz     TA1_FRAC
      jmp     t_setxy                  ; reuse SETXY (tail; sets STATUS)
@ok:
      jmp     finish_ok_nowait

; =====================================================================
; t_set_axis_init — shared SETX/SETY prologue: init if needed + save old pos.
; =====================================================================
t_set_axis_init:
      lda     TURTLE_INITED
      bne     :+
      jsr     turtle_init_state
:
      jmp     turtle_save_old

; t_setx — SETX x: set X only, draw line if pen down. (turtle.s:562)
t_setx:
      jsr     ensure_gfx_mode
      jsr     t_set_axis_init
      lda     TA0_LO
      sta     TURTLE_X_LO
      lda     TA0_HI
      sta     TURTLE_X_HI
      stz     TURTLE_X_FRAC
      jsr     turtle_render
      jmp     finish_ok_nowait

; t_sety — SETY y: set Y only, draw line if pen down. (turtle.s:572)
t_sety:
      jsr     ensure_gfx_mode
      jsr     t_set_axis_init
      lda     TA0_LO
      sta     TURTLE_Y_LO
      lda     TA0_HI
      sta     TURTLE_Y_HI
      stz     TURTLE_Y_FRAC
      jsr     turtle_render
      jmp     finish_ok_nowait

; =====================================================================
; t_seth — SETH degrees: set heading, normalize mod 360. (turtle.s:593)
; =====================================================================
t_seth:
      jsr     ensure_gfx_mode
      lda     TA0_LO
      sta     TURTLE_HEADING_LO
      lda     TA0_HI
      sta     TURTLE_HEADING_HI
      lda     TURTLE_HEADING_HI
      bmi     @add360
      jsr     heading_mod360
      jsr     draw_turtle
      jmp     finish_ok_nowait
@add360:
      clc
      lda     TURTLE_HEADING_LO
      adc     #<360
      sta     TURTLE_HEADING_LO
      lda     TURTLE_HEADING_HI
      adc     #>360
      sta     TURTLE_HEADING_HI
      bmi     @add360
      jsr     heading_mod360
      jsr     draw_turtle
      jmp     finish_ok_nowait

; =====================================================================
; t_cs — CS / CLEARSCREEN / DRAW: reset VGC, clear gfx, split, init + show turtle.
;   (turtle.s:618 / extension.s:215)
; =====================================================================
t_cs:
      stz     VGC_BGCOL
      lda     #TURTLE_COL_WHITE
      sta     VGC_FGCOL
      stz     VGC_BORDER
      stz     VGC_GFXTRANS
      stz     VGC_SCROLLX
      stz     VGC_SCROLLY
      stz     VGC_SCROLLCTL
      lda     #TURTLE_MODE_TEXT
      sta     VGC_MODE
      jsr     vgc_gcls                 ; clear the gfx plane
      jsr     vgc_wait_cmd
      jsr     turtle_enter_split       ; copper split + GFX_VISIBLE=1
      jsr     turtle_init_state        ; reset state + reinstall icon + clear bg bookkeeping
      jsr     draw_turtle
      jmp     finish_ok_nowait

; =====================================================================
; t_pu / t_pd — pen up / pen down. (turtle.s:638)
; =====================================================================
t_pu:
      jsr     ensure_gfx_mode
      lda     #$01
      sta     TURTLE_PEN
      jmp     finish_ok_nowait
t_pd:
      jsr     ensure_gfx_mode
      stz     TURTLE_PEN
      jmp     finish_ok_nowait

; =====================================================================
; t_st / t_ht — show / hide turtle. (turtle.s:651)
; =====================================================================
t_st:
      jsr     ensure_gfx_mode
      lda     #$01
      sta     TURTLE_SHOWN
      jsr     draw_turtle
      jmp     finish_ok_nowait
t_ht:
      jsr     ensure_gfx_mode
      jsr     gfn_turtle_erase         ; restore saved bg (module $B2 sub)
      stz     TURTLE_SHOWN
      jmp     finish_ok_nowait

; =====================================================================
; t_setpc — SETPC color: set pen color. Color in ARG0 byte0. (turtle.s:665)
; =====================================================================
t_setpc:
      jsr     ensure_gfx_mode
      lda     LIB_ARG0+0
      sta     TURTLE_COLOR
      jmp     finish_ok_nowait

; =====================================================================
; t_setbg — SETBG color: set VGC bg, clear gfx to it, redraw turtle. Color in
;   ARG0 byte0. GCLS clears to the current draw color, so set GCOLOR(bg) first.
;   (turtle.s:675 / extension.s:1348)
; =====================================================================
t_setbg:
      jsr     ensure_gfx_mode
      jsr     gfn_turtle_erase         ; lift the turtle before repainting
      lda     LIB_ARG0+0
      sta     VGC_BGCOL
      jsr     vgc_wait_cmd
      lda     LIB_ARG0+0
      sta     VGC_P0
      jsr     vgc_gcolor               ; draw color = bg, so GCLS paints bg
      jsr     vgc_wait_cmd
      jsr     vgc_gcls
      jsr     vgc_wait_cmd
      jsr     draw_turtle
      jmp     finish_ok_nowait

; =====================================================================
; t_ts / t_ss / t_fs — screen-mode ops. (extension.s:851/861/871)
;   TS: full text mode (copper off, mode 0). SS: split (gfx+spr top / text below).
;   FS: full graphics mode (copper off, mode 3). Text-window setup (prepare_split_
;   text) stays the adapter's job — see turtle_enter_split deviation note.
; =====================================================================
t_ts:
      jsr     vgc_wait_cmd
      jsr     copper_off
      stz     VGC_MODE
      stz     TURTLE_GFX_VISIBLE
      jmp     finish_ok_nowait
t_ss:
      jsr     turtle_enter_split
      jmp     finish_ok_nowait
t_fs:
      jsr     vgc_wait_cmd
      jsr     copper_off
      lda     #TURTLE_MODE_GFXSPR
      sta     VGC_MODE
      lda     #$01
      sta     TURTLE_GFX_VISIBLE
      jmp     finish_ok_nowait

; =====================================================================
; Reporters — write a Logo 16.8 value to the LIB_RESULT low word + STATUS OK.
;   t_report16: A = value HI, X = value LO -> LIB_RESULT [FRAC=0, LO, HI, 0].
;   Result cell carries the integer in LO/HI with FRAC=0 (matches the legacy
;   reporters which set EXT_RESULT_FRAC=0).
; =====================================================================
t_report16:
      stz     LIB_RESULT+0             ; FRAC = 0
      stx     LIB_RESULT+1             ; LO
      sta     LIB_RESULT+2             ; HI
      stz     LIB_RESULT+3
      jmp     finish_ok_nowait

t_xcor:
      ldx     TURTLE_X_LO
      lda     TURTLE_X_HI
      jmp     t_report16
t_ycor:
      ldx     TURTLE_Y_LO
      lda     TURTLE_Y_HI
      jmp     t_report16
t_heading:
      ldx     TURTLE_HEADING_LO
      lda     TURTLE_HEADING_HI
      jmp     t_report16
t_pendownp:
      lda     TURTLE_PEN
      bne     @up                      ; pen up ($01) -> 0
      ldx     #1                       ; pen down ($00) -> 1
      lda     #0
      jmp     t_report16
@up:
      ldx     #0
      lda     #0
      jmp     t_report16
t_shownp:
      ldx     TURTLE_SHOWN
      lda     #0
      jmp     t_report16

; =====================================================================
; t_towards — TOWARDS x y: heading angle (degrees) towards (x,y) via ATAN2.
;   ARG0 = target x, ARG1 = target y. (extension.s:1368)
; =====================================================================
t_towards:
      ; dy = turtle_y - target_y (screen Y is inverted) — set FIRST so DX_HI
      ; is the LAST write (the math copro triggers ATAN2 on MATH_ATAN_DX_HI;
      ; the legacy ext_towards wrote DX before DY, latching a stale DY — fixed
      ; here so TOWARDS computes against the intended operands).
      sec
      lda     TURTLE_Y_LO
      sbc     TA1_LO
      sta     MATH_ATAN_DY_LO
      lda     TURTLE_Y_HI
      sbc     TA1_HI
      sta     MATH_ATAN_DY_HI
      ; dx = target_x - turtle_x (signed 16-bit) — DX_HI last -> triggers ATAN2
      sec
      lda     TA0_LO
      sbc     TURTLE_X_LO
      sta     MATH_ATAN_DX_LO
      lda     TA0_HI
      sbc     TURTLE_X_HI
      sta     MATH_ATAN_DX_HI          ; writing DX_HI triggers ATAN2
      ; angle u8 (0-255) -> degrees: u8 * 360 / 256 = hi byte of (u8 * 360)
      lda     MATH_RES0
      sta     MATH_MUL16_A_LO
      stz     MATH_MUL16_A_HI
      lda     #<360
      sta     MATH_MUL16_B_LO
      lda     #>360
      sta     MATH_MUL16_B_HI          ; triggers multiply
      ldx     MATH_RES1                ; (u8*360) >> 8 low
      lda     MATH_RES2                ; high
      jmp     t_report16

; turtle_icon — the built-in 16x16 upward-triangle turtle, one color (white).
; Replicated verbatim from novalogo/extension.s turtle_shape_data. ROM constant
; (CODE segment); GFN_TURTLE_INIT copies these 256 bytes into the BSS source
; buffer at runtime (the module reads/blits from RAM, not ROM).
turtle_icon:
.repeat 16, yy
  .repeat 16, xx
    .if yy < 1
      .byte 0
    .elseif yy > 14
      .byte 0
    .elseif yy >= 13
      .if xx >= (8 - ((((yy - 1) * 7) + 6) / 13)) && xx <= (8 + ((((yy - 1) * 6) + 6) / 13))
        .byte TURTLE_COL_WHITE
      .else
        .byte 0
      .endif
    .elseif xx = (8 - ((((yy - 1) * 7) + 6) / 13))
      .byte TURTLE_COL_WHITE
    .elseif xx = (8 + ((((yy - 1) * 6) + 6) / 13))
      .byte TURTLE_COL_WHITE
    .else
      .byte 0
    .endif
  .endrepeat
.endrepeat

; Module-BSS turtle-render state (batch 4c.2-2). Lives in the MODBSS band
; ($0420-$08FF) carved by 4c.2-1; graphics.cfg grows MODBSS size to fit. The 3
; 256-byte buffers (source/rotated/saved-bg) dominate; the bg bookkeeping +
; clamp scratch are single bytes.
      .segment "BSS"
turtle_source_shape:  .res 256       ; built-in icon, installed by TURTLE_INIT
turtle_rotated_shape: .res 256       ; blitter rotate output
turtle_saved_bg:      .res 256       ; Amiga-BOB saved background under the stamp
turtle_bg_x_lo:       .res 1         ; stamp top-left X (for the matching erase)
turtle_bg_x_hi:       .res 1
turtle_bg_y:          .res 1         ; stamp top-left Y
turtle_bg_saved:      .res 1         ; nonzero when turtle_saved_bg holds a region
turtle_tmp0:          .res 1         ; set_turtle_pos clamp scratch (tmp0)
turtle_tmp1:          .res 1         ; set_turtle_pos clamp scratch (tmp1)
; --- Turtle command-engine move-math scratch (batch 4c.2-3). Module BSS, not ZP:
;     the render runs inline (no lib_call), so plain RAM scratch is safe. old_x/y
;     are the pre-move integer position the post-move pen line reads. ---
t_tmp1:               .res 1         ; raw_dy frac / SETPOS cdr scratch
t_tmp2:               .res 1         ; raw_dy lo  / SETPOS cdr ptr lo
t_tmp3:               .res 1         ; raw_dy hi  / SETPOS cdr ptr hi
t_dx_frac:            .res 1
t_dx_lo:              .res 1
t_dx_hi:              .res 1
t_dy_frac:            .res 1
t_dy_lo:              .res 1
t_dy_hi:              .res 1
t_sin_val:            .res 1
t_cos_val:            .res 1
t_dist_lo:            .res 1         ; FD/BK distance snapshot (mailbox-clobber safe)
t_dist_hi:            .res 1
turtle_old_x_lo:      .res 1
turtle_old_x_hi:      .res 1
turtle_old_y_lo:      .res 1
turtle_old_y_hi:      .res 1
; draw_turtle saves the caller's value-arg bytes here while it borrows the
; mailbox cells for the gfn_turtle_draw call (keeps draw_turtle args-transparent).
t_save_a0_0:          .res 1
t_save_a0_1:          .res 1
t_save_a1_0:          .res 1
t_save_a1_1:          .res 1
t_save_a2_0:          .res 1
      .segment "CODE"

; Shared NDK driver bodies. vgc.s sets its own `.segment "CODE"` and pulls nova.inc
; (VGC_CMD/VCMD_GCLS) via vgc.inc; co-assembles cleanly under its .ifndef guards.
; sprite.s provides the hw-sprite command/register/collision driver entries.
; copper.s adds the copper command/list/add/register-prep entries; copper_split.s
; adds copper_split_mode (it pulls vgc.s + copper.s, all dedup'd by .ifndef guards).
; blitter.s + dma.s provide the $BA-range move-engine entries (batch 4b.6); their
; .inc helpers pull nova.inc for the BLT_*/DMA_* register addresses, dedup'd by
; the same .ifndef guards.
      .include "vgc.s"
      .include "sprite.s"
      .include "copper.s"
      .include "copper_split.s"
      .include "blitter.s"
      .include "dma.s"
; vsprite.s provides the virtual-sprite blit/fill/rotate/save-restore-bg/gfx_*
; + scene-compositor driver entries (batch 4b.7). It .includes blitter.s (already
; pulled above; dedup'd by the .ifndef guard) and declares the VSPRITE_* state
; in the module-owned BSS/ZEROPAGE bands (see graphics.cfg).
      .include "vsprite.s"
; msprite.s provides the meta-sprite spawn/destroy/show/hide/pos/frame/anim/
; priority/transcolor/tick/commit driver entries (batch 4b.8). It .includes
; sprite.s (already pulled above; dedup'd by the .ifndef guard) and declares the
; MSPRITE_* object table in the module-owned BSS + ZEROPAGE bands (see graphics.cfg).
      .include "msprite.s"
; nvg.s provides the NVG image-load driver entries (batch 4b.9); it pulls fio.s
; (with FIO_EMIT_ALL_RUNTIME so fio_gsave/fio_gload are emitted for GSAVE/GLOAD).
; fio.s pulls nova.inc for the FIO_*/VGC_* register addresses, all dedup'd by the
; same .ifndef guards.
      .include "nvg.s"
; anim.s provides the track-based sprite-animation driver entries (batch 4b.11);
; it pulls dma.s + fio.s + pager.s + msprite.s, all already co-assembled above and
; dedup'd by the .ifndef guards (pager.s is the one new body — CODE only, ZP via
; NVR* aliases). anim declares the ANIM_* track table + pseudo-registers in the
; module-owned BSS band (see graphics.cfg).
      .include "anim.s"
; tween.s provides the pure-math 16-bit easing driver entries (batch 4b.11). No
; hardware, no extra deps; it declares the TWEEN_* state cells in the module BSS.
      .include "tween.s"

      .segment "VECTORS"             ; $FFFA — don't-care under SEI; fills the 16KB image
      .word   MOD_ENTRY, MOD_ENTRY, MOD_ENTRY

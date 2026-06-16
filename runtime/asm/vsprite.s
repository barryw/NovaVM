; Shared Nova virtual-sprite helper routines.
;
; The helpers intentionally expose a small blitter-shaped ABI. Runtimes can use
; them for games, cursors, software sprites, board pieces, and any other
; rectangular graphic that should not consume scarce hardware sprite slots.
;
; Moving virtual sprites should use the saved-background helpers instead of
; redrawing their whole scene every frame. Save the graphics-plane rectangle
; under the object, draw the object, restore the saved rectangle before moving,
; then repeat at the new position.

.include "vsprite.inc"
.include "blitter.s"

.ifndef VSPRITE_IMPLEMENTATION_INCLUDED
VSPRITE_IMPLEMENTATION_INCLUDED = 1

      .segment "BSS"

VSPRITE_SRCSPACE:       .res 1
VSPRITE_DSTSPACE:       .res 1
VSPRITE_SRCADDRL:       .res 1
VSPRITE_SRCADDRM:       .res 1
VSPRITE_SRCADDRH:       .res 1
VSPRITE_DSTADDRL:       .res 1
VSPRITE_DSTADDRM:       .res 1
VSPRITE_DSTADDRH:       .res 1
VSPRITE_WIDTHL:         .res 1
VSPRITE_WIDTHH:         .res 1
VSPRITE_HEIGHTL:        .res 1
VSPRITE_HEIGHTH:        .res 1
VSPRITE_SRCSTRL:        .res 1
VSPRITE_SRCSTRH:        .res 1
VSPRITE_DSTSTRL:        .res 1
VSPRITE_DSTSTRH:        .res 1
VSPRITE_BGSPACE:        .res 1
VSPRITE_BGADDRL:        .res 1
VSPRITE_BGADDRM:        .res 1
VSPRITE_BGADDRH:        .res 1
VSPRITE_BGSTRL:         .res 1
VSPRITE_BGSTRH:         .res 1
VSPRITE_COLORKEY:       .res 1
VSPRITE_ORIGSPACE:      .res 1
VSPRITE_ORIGADDRL:      .res 1
VSPRITE_ORIGADDRM:      .res 1
VSPRITE_ORIGADDRH:      .res 1
VSPRITE_ORIGSTRL:       .res 1
VSPRITE_ORIGSTRH:       .res 1
VSPRITE_ROTSPACE:       .res 1
VSPRITE_ROTADDRL:       .res 1
VSPRITE_ROTADDRM:       .res 1
VSPRITE_ROTADDRH:       .res 1
VSPRITE_ROTSTRL:        .res 1
VSPRITE_ROTSTRH:        .res 1
VSPRITE_ROTANGLE:       .res 1
VSPRITE_FILLVALUE:      .res 1
VSPRITE_FLAGS:          .res 1
VSPRITE_XL:             .res 1
VSPRITE_XH:             .res 1
VSPRITE_Y:              .res 1
VSPRITE_SCENE_ADDRL:    .res 1
VSPRITE_SCENE_ADDRH:    .res 1
VSPRITE_SCENE_COUNT:    .res 1
VSPRITE_SCENE_REMAIN:   .res 1
VSPRITE_SCENE_XL:       .res 1
VSPRITE_SCENE_XH:       .res 1
VSPRITE_SCENE_Y:        .res 1
VSPRITE_SCENE_WIDTHL:   .res 1
VSPRITE_SCENE_WIDTHH:   .res 1
VSPRITE_SCENE_HEIGHTL:  .res 1
VSPRITE_SCENE_HEIGHTH:  .res 1
VSPRITE_SCENE_WORKSPACE:.res 1
VSPRITE_SCENE_WORKADDRL:.res 1
VSPRITE_SCENE_WORKADDRM:.res 1
VSPRITE_SCENE_WORKADDRH:.res 1
VSPRITE_SCENE_WORKSTRL: .res 1
VSPRITE_SCENE_WORKSTRH: .res 1
VSPRITE_SCENE_OFFL:     .res 1
VSPRITE_SCENE_OFFH:     .res 1

      .segment "ZEROPAGE"

VSPRITE_TMP:            .res 1
VSPRITE_DESC_PTR:       .res 2

      .segment "CODE"

      .export vsprite_blit
      .export vsprite_blit_start
      .export vsprite_fill
      .export vsprite_gfx_addr
      .export vsprite_gfx_blit
      .export vsprite_gfx_blit_start
      .export vsprite_gfx_rotate_blit
      .export vsprite_gfx_rotate_blit_keyed
      .export vsprite_gfx_rotate_blit_keyed_nowf
      .export vsprite_gfx_rotate_blit_nowait
      .export vsprite_gfx_fill
      .export vsprite_gfx_save_bg
      .export vsprite_gfx_restore_bg
      .export vsprite_rotate
      .export vsprite_use_original
      .export vsprite_use_rotated
      .export vsprite_scene_begin
      .export vsprite_scene_commit
      .export vsprite_scene_commit_atomic
      .export vsprite_scene_draw

; @label VSPRITE.BLIT
; @kind routine
; @symbol vsprite_blit
; @summary Copy the configured virtual sprite rectangle with the blitter.
; @requires VSPRITE_SRCSPACE VSPRITE_DSTSPACE VSPRITE_SRCADDR* VSPRITE_DSTADDR* VSPRITE_WIDTH* VSPRITE_HEIGHT* VSPRITE_SRCSTR* VSPRITE_DSTSTR*
; @in VSPRITE_FLAGS: Set bit 0 to enable color-key transparency.
; @in VSPRITE_COLORKEY: Source byte skipped when color-key mode is enabled.
; @in VGC_GFXTRANS: Graphics-plane color treated as transparent by display composition; set this to a non-black color when blitting visible palette-0 black pixels.
; @out A: 0 on success, 1 on error.
vsprite_blit:
      JSR   vsprite_blit_start
      CMP   #VSPRITE_RESULT_OK
      BNE   @done
      JMP   blitter_wait
@done:
      RTS

; @label VSPRITE.BLIT_START
; @kind routine
; @symbol vsprite_blit_start
; @summary Start the configured virtual sprite blit without waiting for completion.
; @requires VSPRITE_SRCSPACE VSPRITE_DSTSPACE VSPRITE_SRCADDR* VSPRITE_DSTADDR* VSPRITE_WIDTH* VSPRITE_HEIGHT* VSPRITE_SRCSTR* VSPRITE_DSTSTR*
; @in VSPRITE_FLAGS: Set bit 0 to enable color-key transparency.
; @in VSPRITE_COLORKEY: Source byte skipped when color-key mode is enabled.
; @out A: 0 after the blit command is issued.
vsprite_blit_start:
      LDA   VSPRITE_SRCSPACE
      STA   BLT_SRCSPACE
      LDA   VSPRITE_DSTSPACE
      STA   BLT_DSTSPACE
      LDA   VSPRITE_SRCADDRL
      STA   BLT_SRCL
      LDA   VSPRITE_SRCADDRM
      STA   BLT_SRCM
      LDA   VSPRITE_SRCADDRH
      STA   BLT_SRCH
      LDA   VSPRITE_DSTADDRL
      STA   BLT_DSTL
      LDA   VSPRITE_DSTADDRM
      STA   BLT_DSTM
      LDA   VSPRITE_DSTADDRH
      STA   BLT_DSTH
      LDA   VSPRITE_WIDTHL
      STA   BLT_WIDTHL
      LDA   VSPRITE_WIDTHH
      STA   BLT_WIDTHH
      LDA   VSPRITE_HEIGHTL
      STA   BLT_HEIGHTL
      LDA   VSPRITE_HEIGHTH
      STA   BLT_HEIGHTH
      LDA   VSPRITE_SRCSTRL
      STA   BLT_SRCSTRL
      LDA   VSPRITE_SRCSTRH
      STA   BLT_SRCSTRH
      LDA   VSPRITE_DSTSTRL
      STA   BLT_DSTSTRL
      LDA   VSPRITE_DSTSTRH
      STA   BLT_DSTSTRH
      LDA   VSPRITE_COLORKEY
      STA   BLT_CKEY
      LDA   VSPRITE_FLAGS
      AND   #VSPRITE_FLAG_COLORKEY
      BEQ   @no_key
      LDA   #BLT_MODE_COLORKEY
      BRA   @mode
@no_key:
      LDA   #$00
@mode:
      STA   BLT_MODE_REG
      LDA   #BLT_CMD_START
      STA   BLT_CMD_REG
      LDA   #VSPRITE_RESULT_OK
      RTS

; @label VSPRITE.FILL
; @kind routine
; @symbol vsprite_fill
; @summary Fill the configured destination rectangle with VSPRITE.FILLVALUE.
; @requires VSPRITE_DSTSPACE VSPRITE_DSTADDR* VSPRITE_WIDTH* VSPRITE_HEIGHT* VSPRITE_DSTSTR*
; @out A: 0 on success, 1 on error.
vsprite_fill:
      LDA   VSPRITE_DSTSPACE
      STA   BLT_DSTSPACE
      LDA   VSPRITE_DSTADDRL
      STA   BLT_DSTL
      LDA   VSPRITE_DSTADDRM
      STA   BLT_DSTM
      LDA   VSPRITE_DSTADDRH
      STA   BLT_DSTH
      LDA   VSPRITE_WIDTHL
      STA   BLT_WIDTHL
      LDA   VSPRITE_WIDTHH
      STA   BLT_WIDTHH
      LDA   VSPRITE_HEIGHTL
      STA   BLT_HEIGHTL
      LDA   VSPRITE_HEIGHTH
      STA   BLT_HEIGHTH
      LDA   VSPRITE_DSTSTRL
      STA   BLT_DSTSTRL
      LDA   VSPRITE_DSTSTRH
      STA   BLT_DSTSTRH
      LDA   VSPRITE_FILLVALUE
      STA   BLT_FILLVALUE
      JMP   blitter_fill

; @label VSPRITE.USE_ORIGINAL
; @kind routine
; @symbol vsprite_use_original
; @summary Point VSPRITE.SRC* at the configured immutable source shape.
; @requires VSPRITE_ORIGSPACE VSPRITE_ORIGADDR* VSPRITE_ORIGSTR*
; @out A: 0.
vsprite_use_original:
      LDA   VSPRITE_ORIGSPACE
      STA   VSPRITE_SRCSPACE
      LDA   VSPRITE_ORIGADDRL
      STA   VSPRITE_SRCADDRL
      LDA   VSPRITE_ORIGADDRM
      STA   VSPRITE_SRCADDRM
      LDA   VSPRITE_ORIGADDRH
      STA   VSPRITE_SRCADDRH
      LDA   VSPRITE_ORIGSTRL
      STA   VSPRITE_SRCSTRL
      LDA   VSPRITE_ORIGSTRH
      STA   VSPRITE_SRCSTRH
      LDA   #VSPRITE_RESULT_OK
      RTS

; @label VSPRITE.USE_ROTATED
; @kind routine
; @symbol vsprite_use_rotated
; @summary Point VSPRITE.SRC* at the configured rotated output buffer.
; @requires VSPRITE_ROTSPACE VSPRITE_ROTADDR* VSPRITE_ROTSTR*
; @out A: 0.
vsprite_use_rotated:
      LDA   VSPRITE_ROTSPACE
      STA   VSPRITE_SRCSPACE
      LDA   VSPRITE_ROTADDRL
      STA   VSPRITE_SRCADDRL
      LDA   VSPRITE_ROTADDRM
      STA   VSPRITE_SRCADDRM
      LDA   VSPRITE_ROTADDRH
      STA   VSPRITE_SRCADDRH
      LDA   VSPRITE_ROTSTRL
      STA   VSPRITE_SRCSTRL
      LDA   VSPRITE_ROTSTRH
      STA   VSPRITE_SRCSTRH
      LDA   #VSPRITE_RESULT_OK
      RTS

; @label VSPRITE.ROTATE
; @kind routine
; @symbol vsprite_rotate
; @summary Rotate the configured square virtual sprite from immutable source into the caller-owned rotated buffer.
; @requires VSPRITE_ORIGSPACE VSPRITE_ORIGADDR* VSPRITE_ORIGSTR* VSPRITE_ROTSPACE VSPRITE_ROTADDR* VSPRITE_ROTSTR* VSPRITE_WIDTH* VSPRITE_HEIGHT* VSPRITE_ROTANGLE
; @in VSPRITE_COLORKEY: Fill value for rotated output pixels outside the source bounds.
; @desc On success, VSPRITE.SRC* is automatically repointed at VSPRITE.ROT* so VSPRITE.GFX_BLIT draws the rotated copy. Hardware requires WIDTH == HEIGHT and both dimensions <= 256.
; @out A: 0 on success, 1 on error.
vsprite_rotate:
      LDA   VSPRITE_ORIGSPACE
      STA   BLT_SRCSPACE
      LDA   VSPRITE_ROTSPACE
      STA   BLT_DSTSPACE
      LDA   VSPRITE_ORIGADDRL
      STA   BLT_SRCL
      LDA   VSPRITE_ORIGADDRM
      STA   BLT_SRCM
      LDA   VSPRITE_ORIGADDRH
      STA   BLT_SRCH
      LDA   VSPRITE_ROTADDRL
      STA   BLT_DSTL
      LDA   VSPRITE_ROTADDRM
      STA   BLT_DSTM
      LDA   VSPRITE_ROTADDRH
      STA   BLT_DSTH
      LDA   VSPRITE_WIDTHL
      STA   BLT_WIDTHL
      LDA   VSPRITE_WIDTHH
      STA   BLT_WIDTHH
      LDA   VSPRITE_HEIGHTL
      STA   BLT_HEIGHTL
      LDA   VSPRITE_HEIGHTH
      STA   BLT_HEIGHTH
      LDA   VSPRITE_ORIGSTRL
      STA   BLT_SRCSTRL
      LDA   VSPRITE_ORIGSTRH
      STA   BLT_SRCSTRH
      LDA   VSPRITE_ROTSTRL
      STA   BLT_DSTSTRL
      LDA   VSPRITE_ROTSTRH
      STA   BLT_DSTSTRH
      LDA   VSPRITE_COLORKEY
      STA   BLT_CKEY
      LDA   VSPRITE_ROTANGLE
      STA   BLT_ROTANGLE
      LDA   #BLT_MODE_ROTATE
      STA   BLT_MODE_REG
      LDA   #BLT_CMD_START
      STA   BLT_CMD_REG
      JSR   blitter_wait
      CMP   #VSPRITE_RESULT_OK
      BNE   @done
      JSR   vsprite_use_rotated
@done:
      RTS

; @label VSPRITE.GFX_ADDR
; @kind routine
; @symbol vsprite_gfx_addr
; @summary Convert VSPRITE.X/Y to a linear VGC graphics-plane destination address.
; @requires VSPRITE_XL VSPRITE_XH VSPRITE_Y
; @out VSPRITE_DSTADDR*: y * 320 + x.
vsprite_gfx_addr:
      STZ   VSPRITE_DSTADDRL
      LDA   VSPRITE_Y
      STA   VSPRITE_DSTADDRM        ; y * 256
      STZ   VSPRITE_DSTADDRH

      LDA   VSPRITE_Y               ; add y * 64
      LSR
      LSR
      STA   VSPRITE_TMP
      LDA   VSPRITE_Y
      ASL
      ASL
      ASL
      ASL
      ASL
      ASL
      CLC
      ADC   VSPRITE_DSTADDRL
      STA   VSPRITE_DSTADDRL
      LDA   VSPRITE_DSTADDRM
      ADC   VSPRITE_TMP
      STA   VSPRITE_DSTADDRM
      BCC   @add_x
      INC   VSPRITE_DSTADDRH

@add_x:
      LDA   VSPRITE_DSTADDRL
      CLC
      ADC   VSPRITE_XL
      STA   VSPRITE_DSTADDRL
      LDA   VSPRITE_DSTADDRM
      ADC   VSPRITE_XH
      STA   VSPRITE_DSTADDRM
      BCC   @done
      INC   VSPRITE_DSTADDRH
@done:
      RTS

; @label VSPRITE.GFX_BLIT
; @kind routine
; @symbol vsprite_gfx_blit
; @summary Copy the configured virtual sprite to the VGC graphics plane at X/Y.
; @requires VSPRITE_XL VSPRITE_XH VSPRITE_Y VSPRITE_SRCSPACE VSPRITE_SRCADDR* VSPRITE_WIDTH* VSPRITE_HEIGHT* VSPRITE_SRCSTR*
; @in VSPRITE_COLORKEY: Source transparent color when VSPRITE_FLAG_COLORKEY is set; normally match this to VGC_GFXTRANS for graphics-plane virtual sprites.
; @out A: 0 on success, 1 on error.
vsprite_gfx_blit:
      JSR   vsprite_gfx_addr
      LDA   #BLT_SPACE_VGC_GFX
      STA   VSPRITE_DSTSPACE
      LDA   #<VSPRITE_GFX_STRIDE
      STA   VSPRITE_DSTSTRL
      LDA   #>VSPRITE_GFX_STRIDE
      STA   VSPRITE_DSTSTRH
      JMP   vsprite_blit

; @label VSPRITE.GFX_BLIT_START
; @kind routine
; @symbol vsprite_gfx_blit_start
; @summary Start copying the configured virtual sprite to the VGC graphics plane at X/Y without waiting for completion.
; @requires VSPRITE_XL VSPRITE_XH VSPRITE_Y VSPRITE_SRCSPACE VSPRITE_SRCADDR* VSPRITE_WIDTH* VSPRITE_HEIGHT* VSPRITE_SRCSTR*
; @out A: 0 after the blit command is issued.
vsprite_gfx_blit_start:
      JSR   vsprite_gfx_addr
      LDA   #BLT_SPACE_VGC_GFX
      STA   VSPRITE_DSTSPACE
      LDA   #<VSPRITE_GFX_STRIDE
      STA   VSPRITE_DSTSTRL
      LDA   #>VSPRITE_GFX_STRIDE
      STA   VSPRITE_DSTSTRH
      JMP   vsprite_blit_start

; @label VSPRITE.GFX_ROTATE_BLIT
; @kind routine
; @symbol vsprite_gfx_rotate_blit
; @summary Rotate the configured virtual sprite offscreen, wait for the next frame, then copy the full rotated bounds to the graphics plane at X/Y.
; @requires VSPRITE_XL VSPRITE_XH VSPRITE_Y VSPRITE_ORIGSPACE VSPRITE_ORIGADDR* VSPRITE_ORIGSTR* VSPRITE_ROTSPACE VSPRITE_ROTADDR* VSPRITE_ROTSTR* VSPRITE_WIDTH* VSPRITE_HEIGHT* VSPRITE_ROTANGLE
; @in VSPRITE_COLORKEY: Fill value for rotated output pixels outside the source bounds; normally match this to VGC_GFXTRANS.
; @desc The final graphics blit is intentionally non-color-keyed so transparent pixels from the rotated buffer clear the previous frame in the same visible copy.
; @out A: 0 on success, 1 on error.
vsprite_gfx_rotate_blit:
      JSR   vsprite_rotate
      CMP   #VSPRITE_RESULT_OK
      BNE   @done
      JSR   vsprite_wait_frame
      JMP   vsprite_gfx_blit_rotated
@done:
      RTS

; @label VSPRITE.GFX_ROTATE_BLIT_KEYED
; @kind routine
; @symbol vsprite_gfx_rotate_blit_keyed
; @summary Rotate the configured virtual sprite, wait for the next frame, then blit it with the caller's VSPRITE.FLAGS.
; @requires VSPRITE.X/Y VSPRITE.ORIG* VSPRITE.ROT* VSPRITE.WIDTH/HEIGHT VSPRITE.ROTANGLE
; @in VSPRITE_FLAGS: Set VSPRITE_FLAG_COLORKEY to preserve existing graphics-plane pixels where the rotated sprite is transparent.
; @out A: 0 on success, 1 on error.
vsprite_gfx_rotate_blit_keyed:
      JSR   vsprite_rotate
      CMP   #VSPRITE_RESULT_OK
      BNE   @done
      JSR   vsprite_wait_frame
      JMP   vsprite_gfx_blit
@done:
      RTS

; @label VSPRITE.GFX_ROTATE_BLIT_KEYED_NOWF
; @kind routine
; @symbol vsprite_gfx_rotate_blit_keyed_nowf
; @summary Like vsprite_gfx_rotate_blit_keyed but does NOT wait for VGC.FRAME — it
;   still waits for the blitter to finish, so the graphics plane is consistent on
;   return. Use this when the CALLER owns frame pacing and wants the erase+blit of a
;   moving stamp to land within a single inter-frame gap (so the 60Hz display never
;   samples a half-erased stamp). The turtle's per-move render uses this and does its
;   own vsprite_wait_frame BEFORE erasing.
; @requires VSPRITE.X/Y VSPRITE.ORIG* VSPRITE.ROT* VSPRITE.WIDTH/HEIGHT VSPRITE.ROTANGLE
; @out A: 0 on success, 1 on error.
vsprite_gfx_rotate_blit_keyed_nowf:
      JSR   vsprite_rotate
      CMP   #VSPRITE_RESULT_OK
      BNE   @done
      JMP   vsprite_gfx_blit         ; blit (waits for the blitter), but NO frame-wait
@done:
      RTS

; @label VSPRITE.GFX_ROTATE_BLIT_NOWAIT
; @kind routine
; @symbol vsprite_gfx_rotate_blit_nowait
; @summary Rotate the configured virtual sprite offscreen, then start copying the full rotated bounds to graphics memory without waiting for VGC.FRAME or final blit completion.
; @requires VSPRITE_XL VSPRITE_XH VSPRITE_Y VSPRITE_ORIGSPACE VSPRITE_ORIGADDR* VSPRITE_ORIGSTR* VSPRITE_ROTSPACE VSPRITE_ROTADDR* VSPRITE_ROTSTR* VSPRITE_WIDTH* VSPRITE_HEIGHT* VSPRITE_ROTANGLE
; @in VSPRITE_COLORKEY: Fill value for rotated output pixels outside the source bounds; normally match this to VGC_GFXTRANS.
; @desc Use this in command/REPL paths or runtimes that already own frame pacing.
; @out A: 0 on success, 1 on error.
vsprite_gfx_rotate_blit_nowait:
      JSR   vsprite_rotate
      CMP   #VSPRITE_RESULT_OK
      BNE   vsprite_gfx_rotate_done
      LDA   VSPRITE_FLAGS
      PHA
      STZ   VSPRITE_FLAGS
      JSR   vsprite_gfx_blit_start
      STA   VSPRITE_TMP
      PLA
      STA   VSPRITE_FLAGS
      LDA   VSPRITE_TMP
      RTS

vsprite_gfx_blit_rotated:
      LDA   VSPRITE_FLAGS
      PHA
      STZ   VSPRITE_FLAGS
      JSR   vsprite_gfx_blit
      STA   VSPRITE_TMP
      PLA
      STA   VSPRITE_FLAGS
      LDA   VSPRITE_TMP
vsprite_gfx_rotate_done:
      RTS

vsprite_wait_frame:
      LDA   VGC_FRAME
@wait:
      CMP   VGC_FRAME
      BEQ   @wait
      RTS

; @label VSPRITE.GFX_FILL
; @kind routine
; @symbol vsprite_gfx_fill
; @summary Fill a graphics-plane rectangle at VSPRITE.X/Y.
; @requires VSPRITE_XL VSPRITE_XH VSPRITE_Y VSPRITE_WIDTH* VSPRITE_HEIGHT* VSPRITE_FILLVALUE
; @out A: 0 on success, 1 on error.
vsprite_gfx_fill:
      JSR   vsprite_gfx_addr
      LDA   #BLT_SPACE_VGC_GFX
      STA   VSPRITE_DSTSPACE
      LDA   #<VSPRITE_GFX_STRIDE
      STA   VSPRITE_DSTSTRL
      LDA   #>VSPRITE_GFX_STRIDE
      STA   VSPRITE_DSTSTRH
      JMP   vsprite_fill

; @label VSPRITE.GFX_SAVE_BG
; @kind routine
; @symbol vsprite_gfx_save_bg
; @summary Save a graphics-plane rectangle under VSPRITE.X/Y into the caller-owned VSPRITE.BG* buffer.
; @requires VSPRITE_XL VSPRITE_XH VSPRITE_Y VSPRITE_WIDTH* VSPRITE_HEIGHT* VSPRITE_BGSPACE VSPRITE_BGADDR* VSPRITE_BGSTR*
; @out A: 0 on success, 1 on error.
vsprite_gfx_save_bg:
      JSR   vsprite_gfx_addr
      LDA   #BLT_SPACE_VGC_GFX
      STA   BLT_SRCSPACE
      LDA   VSPRITE_BGSPACE
      STA   BLT_DSTSPACE
      LDA   VSPRITE_DSTADDRL
      STA   BLT_SRCL
      LDA   VSPRITE_DSTADDRM
      STA   BLT_SRCM
      LDA   VSPRITE_DSTADDRH
      STA   BLT_SRCH
      LDA   VSPRITE_BGADDRL
      STA   BLT_DSTL
      LDA   VSPRITE_BGADDRM
      STA   BLT_DSTM
      LDA   VSPRITE_BGADDRH
      STA   BLT_DSTH
      LDA   VSPRITE_WIDTHL
      STA   BLT_WIDTHL
      LDA   VSPRITE_WIDTHH
      STA   BLT_WIDTHH
      LDA   VSPRITE_HEIGHTL
      STA   BLT_HEIGHTL
      LDA   VSPRITE_HEIGHTH
      STA   BLT_HEIGHTH
      LDA   #<VSPRITE_GFX_STRIDE
      STA   BLT_SRCSTRL
      LDA   #>VSPRITE_GFX_STRIDE
      STA   BLT_SRCSTRH
      LDA   VSPRITE_BGSTRL
      STA   BLT_DSTSTRL
      LDA   VSPRITE_BGSTRH
      STA   BLT_DSTSTRH
      JMP   blitter_start_copy

; @label VSPRITE.GFX_RESTORE_BG
; @kind routine
; @symbol vsprite_gfx_restore_bg
; @summary Restore a caller-owned VSPRITE.BG* buffer into the graphics plane at VSPRITE.X/Y.
; @requires VSPRITE_XL VSPRITE_XH VSPRITE_Y VSPRITE_WIDTH* VSPRITE_HEIGHT* VSPRITE_BGSPACE VSPRITE_BGADDR* VSPRITE_BGSTR*
; @out A: 0 on success, 1 on error.
vsprite_gfx_restore_bg:
      JSR   vsprite_gfx_addr
      LDA   VSPRITE_BGSPACE
      STA   BLT_SRCSPACE
      LDA   #BLT_SPACE_VGC_GFX
      STA   BLT_DSTSPACE
      LDA   VSPRITE_BGADDRL
      STA   BLT_SRCL
      LDA   VSPRITE_BGADDRM
      STA   BLT_SRCM
      LDA   VSPRITE_BGADDRH
      STA   BLT_SRCH
      LDA   VSPRITE_DSTADDRL
      STA   BLT_DSTL
      LDA   VSPRITE_DSTADDRM
      STA   BLT_DSTM
      LDA   VSPRITE_DSTADDRH
      STA   BLT_DSTH
      LDA   VSPRITE_WIDTHL
      STA   BLT_WIDTHL
      LDA   VSPRITE_WIDTHH
      STA   BLT_WIDTHH
      LDA   VSPRITE_HEIGHTL
      STA   BLT_HEIGHTL
      LDA   VSPRITE_HEIGHTH
      STA   BLT_HEIGHTH
      LDA   VSPRITE_BGSTRL
      STA   BLT_SRCSTRL
      LDA   VSPRITE_BGSTRH
      STA   BLT_SRCSTRH
      LDA   #<VSPRITE_GFX_STRIDE
      STA   BLT_DSTSTRL
      LDA   #>VSPRITE_GFX_STRIDE
      STA   BLT_DSTSTRH
      JMP   blitter_start_copy

; @label VSPRITE.SCENE_BEGIN
; @kind routine
; @symbol vsprite_scene_begin
; @summary Restore all saved old backgrounds in the configured scene descriptor list.
; @requires VSPRITE_SCENE_ADDRL VSPRITE_SCENE_ADDRH VSPRITE_SCENE_COUNT
; @desc Call this once per animation frame before updating descriptor X/Y values. Descriptors with VSPRITE.DESC_SAVED are restored and have that internal flag cleared.
vsprite_scene_begin:
      JSR   vsprite_scene_rewind
@loop:
      LDA   VSPRITE_SCENE_REMAIN
      BEQ   @done
      LDY   #VSPRITE_DESC_FLAGS
      LDA   (VSPRITE_DESC_PTR),Y
      AND   #VSPRITE_DESC_SAVED
      BEQ   @next
      JSR   vsprite_scene_load_old
      JSR   vsprite_gfx_restore_bg
      LDY   #VSPRITE_DESC_FLAGS
      LDA   (VSPRITE_DESC_PTR),Y
      AND   #$BF
      STA   (VSPRITE_DESC_PTR),Y
@next:
      JSR   vsprite_scene_next
      BRA   @loop
@done:
      RTS

; @label VSPRITE.SCENE_COMMIT
; @kind routine
; @symbol vsprite_scene_commit
; @summary Save all current backgrounds, then draw all visible descriptors in list order.
; @requires VSPRITE_SCENE_ADDRL VSPRITE_SCENE_ADDRH VSPRITE_SCENE_COUNT
; @desc List order is z-order: later descriptors draw in front of earlier descriptors.
vsprite_scene_commit:
      JSR   vsprite_scene_save
      ; fall through

; @label VSPRITE.SCENE_DRAW
; @kind routine
; @symbol vsprite_scene_draw
; @summary Draw all visible descriptors in list order without saving backgrounds.
; @requires VSPRITE_SCENE_ADDRL VSPRITE_SCENE_ADDRH VSPRITE_SCENE_COUNT
vsprite_scene_draw:
      JSR   vsprite_scene_rewind
@loop:
      LDA   VSPRITE_SCENE_REMAIN
      BEQ   @done
      LDY   #VSPRITE_DESC_FLAGS
      LDA   (VSPRITE_DESC_PTR),Y
      AND   #VSPRITE_DESC_VISIBLE
      BEQ   @next
      JSR   vsprite_scene_load_current
      JSR   vsprite_gfx_blit
@next:
      JSR   vsprite_scene_next
      BRA   @loop
@done:
      RTS

; @label VSPRITE.SCENE_COMMIT_ATOMIC
; @kind routine
; @symbol vsprite_scene_commit_atomic
; @summary Compose a scene into a caller-owned work buffer, then commit the completed dirty rectangle to graphics memory.
; @requires VSPRITE_SCENE_ADDRL VSPRITE_SCENE_ADDRH VSPRITE_SCENE_COUNT VSPRITE_SCENE_X* VSPRITE_SCENE_Y VSPRITE_SCENE_WIDTH* VSPRITE_SCENE_HEIGHT* VSPRITE_SCENE_WORK*
; @desc The final visible commit is one graphics-plane blit. It is visually atomic only when the dirty rectangle fits in one vblank-safe write window.
vsprite_scene_commit_atomic:
      JSR   vsprite_scene_gfx_to_work
      JSR   vsprite_scene_restore_old_to_work
      JSR   vsprite_scene_save_current_from_work
      JSR   vsprite_scene_draw_current_to_work
      JMP   vsprite_scene_work_to_gfx

vsprite_scene_gfx_to_work:
      LDA   VSPRITE_SCENE_XL
      STA   VSPRITE_XL
      LDA   VSPRITE_SCENE_XH
      STA   VSPRITE_XH
      LDA   VSPRITE_SCENE_Y
      STA   VSPRITE_Y
      JSR   vsprite_gfx_addr
      LDA   #BLT_SPACE_VGC_GFX
      STA   BLT_SRCSPACE
      LDA   VSPRITE_SCENE_WORKSPACE
      STA   BLT_DSTSPACE
      LDA   VSPRITE_DSTADDRL
      STA   BLT_SRCL
      LDA   VSPRITE_DSTADDRM
      STA   BLT_SRCM
      LDA   VSPRITE_DSTADDRH
      STA   BLT_SRCH
      LDA   VSPRITE_SCENE_WORKADDRL
      STA   BLT_DSTL
      LDA   VSPRITE_SCENE_WORKADDRM
      STA   BLT_DSTM
      LDA   VSPRITE_SCENE_WORKADDRH
      STA   BLT_DSTH
      JSR   vsprite_scene_load_dirty_size
      LDA   #<VSPRITE_GFX_STRIDE
      STA   BLT_SRCSTRL
      LDA   #>VSPRITE_GFX_STRIDE
      STA   BLT_SRCSTRH
      LDA   VSPRITE_SCENE_WORKSTRL
      STA   BLT_DSTSTRL
      LDA   VSPRITE_SCENE_WORKSTRH
      STA   BLT_DSTSTRH
      JMP   blitter_start_copy

vsprite_scene_work_to_gfx:
      LDA   VSPRITE_SCENE_XL
      STA   VSPRITE_XL
      LDA   VSPRITE_SCENE_XH
      STA   VSPRITE_XH
      LDA   VSPRITE_SCENE_Y
      STA   VSPRITE_Y
      JSR   vsprite_gfx_addr
      LDA   VSPRITE_SCENE_WORKSPACE
      STA   BLT_SRCSPACE
      LDA   #BLT_SPACE_VGC_GFX
      STA   BLT_DSTSPACE
      LDA   VSPRITE_SCENE_WORKADDRL
      STA   BLT_SRCL
      LDA   VSPRITE_SCENE_WORKADDRM
      STA   BLT_SRCM
      LDA   VSPRITE_SCENE_WORKADDRH
      STA   BLT_SRCH
      LDA   VSPRITE_DSTADDRL
      STA   BLT_DSTL
      LDA   VSPRITE_DSTADDRM
      STA   BLT_DSTM
      LDA   VSPRITE_DSTADDRH
      STA   BLT_DSTH
      JSR   vsprite_scene_load_dirty_size
      LDA   VSPRITE_SCENE_WORKSTRL
      STA   BLT_SRCSTRL
      LDA   VSPRITE_SCENE_WORKSTRH
      STA   BLT_SRCSTRH
      LDA   #<VSPRITE_GFX_STRIDE
      STA   BLT_DSTSTRL
      LDA   #>VSPRITE_GFX_STRIDE
      STA   BLT_DSTSTRH
      JMP   blitter_start_copy

vsprite_scene_load_dirty_size:
      LDA   VSPRITE_SCENE_WIDTHL
      STA   BLT_WIDTHL
      LDA   VSPRITE_SCENE_WIDTHH
      STA   BLT_WIDTHH
      LDA   VSPRITE_SCENE_HEIGHTL
      STA   BLT_HEIGHTL
      LDA   VSPRITE_SCENE_HEIGHTH
      STA   BLT_HEIGHTH
      RTS

vsprite_scene_restore_old_to_work:
      JSR   vsprite_scene_rewind
@loop:
      LDA   VSPRITE_SCENE_REMAIN
      BEQ   @done
      LDY   #VSPRITE_DESC_FLAGS
      LDA   (VSPRITE_DESC_PTR),Y
      AND   #VSPRITE_DESC_SAVED
      BEQ   @next
      JSR   vsprite_scene_load_old
      JSR   vsprite_scene_calc_work_addr
      LDA   VSPRITE_BGSPACE
      STA   BLT_SRCSPACE
      LDA   VSPRITE_SCENE_WORKSPACE
      STA   BLT_DSTSPACE
      LDA   VSPRITE_BGADDRL
      STA   BLT_SRCL
      LDA   VSPRITE_BGADDRM
      STA   BLT_SRCM
      LDA   VSPRITE_BGADDRH
      STA   BLT_SRCH
      LDA   VSPRITE_DSTADDRL
      STA   BLT_DSTL
      LDA   VSPRITE_DSTADDRM
      STA   BLT_DSTM
      LDA   VSPRITE_DSTADDRH
      STA   BLT_DSTH
      JSR   vsprite_scene_load_sprite_size
      LDA   VSPRITE_BGSTRL
      STA   BLT_SRCSTRL
      LDA   VSPRITE_BGSTRH
      STA   BLT_SRCSTRH
      LDA   VSPRITE_SCENE_WORKSTRL
      STA   BLT_DSTSTRL
      LDA   VSPRITE_SCENE_WORKSTRH
      STA   BLT_DSTSTRH
      JSR   blitter_start_copy
@next:
      JSR   vsprite_scene_next
      BRA   @loop
@done:
      RTS

vsprite_scene_save_current_from_work:
      JSR   vsprite_scene_rewind
@loop:
      LDA   VSPRITE_SCENE_REMAIN
      BEQ   @done
      LDY   #VSPRITE_DESC_FLAGS
      LDA   (VSPRITE_DESC_PTR),Y
      AND   #VSPRITE_DESC_VISIBLE
      BEQ   @next
      JSR   vsprite_scene_load_current
      JSR   vsprite_scene_calc_work_addr
      LDA   VSPRITE_SCENE_WORKSPACE
      STA   BLT_SRCSPACE
      LDA   VSPRITE_BGSPACE
      STA   BLT_DSTSPACE
      LDA   VSPRITE_DSTADDRL
      STA   BLT_SRCL
      LDA   VSPRITE_DSTADDRM
      STA   BLT_SRCM
      LDA   VSPRITE_DSTADDRH
      STA   BLT_SRCH
      LDA   VSPRITE_BGADDRL
      STA   BLT_DSTL
      LDA   VSPRITE_BGADDRM
      STA   BLT_DSTM
      LDA   VSPRITE_BGADDRH
      STA   BLT_DSTH
      JSR   vsprite_scene_load_sprite_size
      LDA   VSPRITE_SCENE_WORKSTRL
      STA   BLT_SRCSTRL
      LDA   VSPRITE_SCENE_WORKSTRH
      STA   BLT_SRCSTRH
      LDA   VSPRITE_BGSTRL
      STA   BLT_DSTSTRL
      LDA   VSPRITE_BGSTRH
      STA   BLT_DSTSTRH
      JSR   blitter_start_copy
      JSR   vsprite_scene_store_old
      LDY   #VSPRITE_DESC_FLAGS
      LDA   (VSPRITE_DESC_PTR),Y
      ORA   #VSPRITE_DESC_SAVED
      STA   (VSPRITE_DESC_PTR),Y
@next:
      JSR   vsprite_scene_next
      BRA   @loop
@done:
      RTS

vsprite_scene_draw_current_to_work:
      JSR   vsprite_scene_rewind
@loop:
      LDA   VSPRITE_SCENE_REMAIN
      BEQ   @done
      LDY   #VSPRITE_DESC_FLAGS
      LDA   (VSPRITE_DESC_PTR),Y
      AND   #VSPRITE_DESC_VISIBLE
      BEQ   @next
      JSR   vsprite_scene_load_current
      JSR   vsprite_scene_calc_work_addr
      LDA   VSPRITE_SCENE_WORKSPACE
      STA   VSPRITE_DSTSPACE
      LDA   VSPRITE_SCENE_WORKSTRL
      STA   VSPRITE_DSTSTRL
      LDA   VSPRITE_SCENE_WORKSTRH
      STA   VSPRITE_DSTSTRH
      JSR   vsprite_blit
@next:
      JSR   vsprite_scene_next
      BRA   @loop
@done:
      RTS

vsprite_scene_load_sprite_size:
      LDA   VSPRITE_WIDTHL
      STA   BLT_WIDTHL
      LDA   VSPRITE_WIDTHH
      STA   BLT_WIDTHH
      LDA   VSPRITE_HEIGHTL
      STA   BLT_HEIGHTL
      LDA   VSPRITE_HEIGHTH
      STA   BLT_HEIGHTH
      RTS

vsprite_scene_calc_work_addr:
      LDA   VSPRITE_SCENE_WORKADDRL
      STA   VSPRITE_DSTADDRL
      LDA   VSPRITE_SCENE_WORKADDRM
      STA   VSPRITE_DSTADDRM
      LDA   VSPRITE_SCENE_WORKADDRH
      STA   VSPRITE_DSTADDRH

      LDA   VSPRITE_Y
      SEC
      SBC   VSPRITE_SCENE_Y
      BCS   @row_have             ; no borrow: VSPRITE_Y >= VSPRITE_SCENE_Y
      LDA   #$00                  ; sprite above dirty-rect top: clamp delta to 0
@row_have:                        ; so it stays in the work buffer (no 256-row walk)
      TAX
      BEQ   @add_x
@row_loop:
      LDA   VSPRITE_DSTADDRL
      CLC
      ADC   VSPRITE_SCENE_WORKSTRL
      STA   VSPRITE_DSTADDRL
      LDA   VSPRITE_DSTADDRM
      ADC   VSPRITE_SCENE_WORKSTRH
      STA   VSPRITE_DSTADDRM
      BCC   @row_no_carry
      INC   VSPRITE_DSTADDRH
@row_no_carry:
      DEX
      BNE   @row_loop

@add_x:
      LDA   VSPRITE_XL
      SEC
      SBC   VSPRITE_SCENE_XL
      STA   VSPRITE_SCENE_OFFL
      LDA   VSPRITE_XH
      SBC   VSPRITE_SCENE_XH
      STA   VSPRITE_SCENE_OFFH
      LDA   VSPRITE_DSTADDRL
      CLC
      ADC   VSPRITE_SCENE_OFFL
      STA   VSPRITE_DSTADDRL
      LDA   VSPRITE_DSTADDRM
      ADC   VSPRITE_SCENE_OFFH
      STA   VSPRITE_DSTADDRM
      BCC   @done
      INC   VSPRITE_DSTADDRH
@done:
      RTS

vsprite_scene_save:
      JSR   vsprite_scene_rewind
@loop:
      LDA   VSPRITE_SCENE_REMAIN
      BEQ   @done
      LDY   #VSPRITE_DESC_FLAGS
      LDA   (VSPRITE_DESC_PTR),Y
      AND   #VSPRITE_DESC_VISIBLE
      BEQ   @next
      JSR   vsprite_scene_load_current
      JSR   vsprite_gfx_save_bg
      JSR   vsprite_scene_store_old
      LDY   #VSPRITE_DESC_FLAGS
      LDA   (VSPRITE_DESC_PTR),Y
      ORA   #VSPRITE_DESC_SAVED
      STA   (VSPRITE_DESC_PTR),Y
@next:
      JSR   vsprite_scene_next
      BRA   @loop
@done:
      RTS

vsprite_scene_rewind:
      LDA   VSPRITE_SCENE_ADDRL
      STA   VSPRITE_DESC_PTR
      LDA   VSPRITE_SCENE_ADDRH
      STA   VSPRITE_DESC_PTR + 1
      LDA   VSPRITE_SCENE_COUNT
      STA   VSPRITE_SCENE_REMAIN
      RTS

vsprite_scene_next:
      LDA   VSPRITE_DESC_PTR
      CLC
      ADC   #VSPRITE_DESC_SIZE
      STA   VSPRITE_DESC_PTR
      BCC   @same_page
      INC   VSPRITE_DESC_PTR + 1
@same_page:
      DEC   VSPRITE_SCENE_REMAIN
      RTS

vsprite_scene_load_common:
      LDY   #VSPRITE_DESC_WIDTHL
      LDA   (VSPRITE_DESC_PTR),Y
      STA   VSPRITE_WIDTHL
      LDY   #VSPRITE_DESC_WIDTHH
      LDA   (VSPRITE_DESC_PTR),Y
      STA   VSPRITE_WIDTHH
      LDY   #VSPRITE_DESC_HEIGHTL
      LDA   (VSPRITE_DESC_PTR),Y
      STA   VSPRITE_HEIGHTL
      LDY   #VSPRITE_DESC_HEIGHTH
      LDA   (VSPRITE_DESC_PTR),Y
      STA   VSPRITE_HEIGHTH
      LDY   #VSPRITE_DESC_BGSPACE
      LDA   (VSPRITE_DESC_PTR),Y
      STA   VSPRITE_BGSPACE
      LDY   #VSPRITE_DESC_BGADDRL
      LDA   (VSPRITE_DESC_PTR),Y
      STA   VSPRITE_BGADDRL
      LDY   #VSPRITE_DESC_BGADDRM
      LDA   (VSPRITE_DESC_PTR),Y
      STA   VSPRITE_BGADDRM
      LDY   #VSPRITE_DESC_BGADDRH
      LDA   (VSPRITE_DESC_PTR),Y
      STA   VSPRITE_BGADDRH
      LDY   #VSPRITE_DESC_BGSTRL
      LDA   (VSPRITE_DESC_PTR),Y
      STA   VSPRITE_BGSTRL
      LDY   #VSPRITE_DESC_BGSTRH
      LDA   (VSPRITE_DESC_PTR),Y
      STA   VSPRITE_BGSTRH
      RTS

vsprite_scene_load_current:
      JSR   vsprite_scene_load_common
      LDY   #VSPRITE_DESC_XL
      LDA   (VSPRITE_DESC_PTR),Y
      STA   VSPRITE_XL
      LDY   #VSPRITE_DESC_XH
      LDA   (VSPRITE_DESC_PTR),Y
      STA   VSPRITE_XH
      LDY   #VSPRITE_DESC_Y
      LDA   (VSPRITE_DESC_PTR),Y
      STA   VSPRITE_Y
      LDY   #VSPRITE_DESC_SRCSPACE
      LDA   (VSPRITE_DESC_PTR),Y
      STA   VSPRITE_SRCSPACE
      LDY   #VSPRITE_DESC_SRCADDRL
      LDA   (VSPRITE_DESC_PTR),Y
      STA   VSPRITE_SRCADDRL
      LDY   #VSPRITE_DESC_SRCADDRM
      LDA   (VSPRITE_DESC_PTR),Y
      STA   VSPRITE_SRCADDRM
      LDY   #VSPRITE_DESC_SRCADDRH
      LDA   (VSPRITE_DESC_PTR),Y
      STA   VSPRITE_SRCADDRH
      LDY   #VSPRITE_DESC_SRCSTRL
      LDA   (VSPRITE_DESC_PTR),Y
      STA   VSPRITE_SRCSTRL
      LDY   #VSPRITE_DESC_SRCSTRH
      LDA   (VSPRITE_DESC_PTR),Y
      STA   VSPRITE_SRCSTRH
      LDY   #VSPRITE_DESC_COLORKEY
      LDA   (VSPRITE_DESC_PTR),Y
      STA   VSPRITE_COLORKEY
      LDY   #VSPRITE_DESC_FLAGS
      LDA   (VSPRITE_DESC_PTR),Y
      AND   #VSPRITE_DESC_FLAG_COLORKEY
      STA   VSPRITE_FLAGS
      RTS

vsprite_scene_load_old:
      JSR   vsprite_scene_load_common
      LDY   #VSPRITE_DESC_OLD_XL
      LDA   (VSPRITE_DESC_PTR),Y
      STA   VSPRITE_XL
      LDY   #VSPRITE_DESC_OLD_XH
      LDA   (VSPRITE_DESC_PTR),Y
      STA   VSPRITE_XH
      LDY   #VSPRITE_DESC_OLD_Y
      LDA   (VSPRITE_DESC_PTR),Y
      STA   VSPRITE_Y
      RTS

vsprite_scene_store_old:
      LDY   #VSPRITE_DESC_XL
      LDA   (VSPRITE_DESC_PTR),Y
      LDY   #VSPRITE_DESC_OLD_XL
      STA   (VSPRITE_DESC_PTR),Y
      LDY   #VSPRITE_DESC_XH
      LDA   (VSPRITE_DESC_PTR),Y
      LDY   #VSPRITE_DESC_OLD_XH
      STA   (VSPRITE_DESC_PTR),Y
      LDY   #VSPRITE_DESC_Y
      LDA   (VSPRITE_DESC_PTR),Y
      LDY   #VSPRITE_DESC_OLD_Y
      STA   (VSPRITE_DESC_PTR),Y
      RTS

.endif

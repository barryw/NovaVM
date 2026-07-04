; nui dialog save/restore-under.
;
; Feature-group object: preserves the screen contents a dialog will cover (three
; text planes, optionally the graphics plane) in caller-owned XRAM and restores
; them when the dialog closes. Pulled only when a caller uses the save-under
; routines; dropped otherwise (e.g. an app that only shows transient dialogs).

.include "nui.inc"
.include "vsprite.inc"

      .segment "CODE"

nui_select_saved_region:
      LDA   NUI_SAVE_LEFT
      STA   VTEXT_LEFT
      LDA   NUI_SAVE_TOP
      STA   VTEXT_TOP
      LDA   NUI_SAVE_WIDTH
      STA   VTEXT_WIDTH
      LDA   NUI_SAVE_HEIGHT
      STA   VTEXT_HEIGHT
      STZ   VTEXT_CURX
      STZ   VTEXT_CURY
      JMP   vtext_validate_region

; nui_save_plane — A = VGC plane space, X = saved-plane index (0/1/2).
; Copies the current VTEXT rectangle into caller-owned XRAM staging.
nui_save_plane:
      STA   BLT_SRCSPACE
      LDA   #BLT_SPACE_XRAM
      STA   BLT_DSTSPACE
      LDA   VTEXT_ADDRL
      STA   BLT_SRCL
      LDA   VTEXT_ADDRH
      STA   BLT_SRCM
      STZ   BLT_SRCH
      LDA   NUI_SAVE_ADDRL,X
      STA   BLT_DSTL
      LDA   NUI_SAVE_ADDRM,X
      STA   BLT_DSTM
      LDA   NUI_SAVE_ADDRH,X
      STA   BLT_DSTH
      LDA   VTEXT_WIDTH
      STA   BLT_WIDTHL
      STZ   BLT_WIDTHH
      LDA   VTEXT_HEIGHT
      STA   BLT_HEIGHTL
      STZ   BLT_HEIGHTH
      LDA   #<VTEXT_SCREEN_COLS
      STA   BLT_SRCSTRL
      LDA   #>VTEXT_SCREEN_COLS
      STA   BLT_SRCSTRH
      LDA   VTEXT_WIDTH
      STA   BLT_DSTSTRL
      STZ   BLT_DSTSTRH
      JMP   blitter_start_copy

; nui_restore_plane — A = VGC plane space, X = saved-plane index (0/1/2).
; Copies caller-owned XRAM staging back over the current VTEXT rectangle.
nui_restore_plane:
      STA   BLT_DSTSPACE
      LDA   #BLT_SPACE_XRAM
      STA   BLT_SRCSPACE
      LDA   NUI_SAVE_ADDRL,X
      STA   BLT_SRCL
      LDA   NUI_SAVE_ADDRM,X
      STA   BLT_SRCM
      LDA   NUI_SAVE_ADDRH,X
      STA   BLT_SRCH
      LDA   VTEXT_ADDRL
      STA   BLT_DSTL
      LDA   VTEXT_ADDRH
      STA   BLT_DSTM
      STZ   BLT_DSTH
      LDA   VTEXT_WIDTH
      STA   BLT_WIDTHL
      STZ   BLT_WIDTHH
      LDA   VTEXT_HEIGHT
      STA   BLT_HEIGHTL
      STZ   BLT_HEIGHTH
      LDA   VTEXT_WIDTH
      STA   BLT_SRCSTRL
      STZ   BLT_SRCSTRH
      LDA   #<VTEXT_SCREEN_COLS
      STA   BLT_DSTSTRL
      LDA   #>VTEXT_SCREEN_COLS
      STA   BLT_DSTSTRH
      JMP   blitter_start_copy

; @label NUI.SAVE_UNDER
; @kind routine
; @symbol nui_save_under
; @summary Save the text planes under NUI_SAVE_* into caller-provided XRAM buffers.
; @requires NUI_SAVE_LEFT NUI_SAVE_TOP NUI_SAVE_WIDTH NUI_SAVE_HEIGHT NUI_SAVE_ADDRL NUI_SAVE_ADDRM NUI_SAVE_ADDRH
; @out A: 0 on success, 1 on error.
nui_save_under:
      STZ   NUI_SAVE_VALID
      JSR   nui_select_saved_region
      BNE   @error
      JSR   vtext_calc_region_addr

      LDX   #0
      LDA   #BLT_SPACE_VGC_CHAR
      JSR   nui_save_plane
      BNE   @error
      LDX   #1
      LDA   #BLT_SPACE_VGC_COLOR
      JSR   nui_save_plane
      BNE   @error
      LDX   #2
      LDA   #BLT_SPACE_VGC_TEXTATTR
      JSR   nui_save_plane
      BNE   @error

      LDA   #1
      STA   NUI_SAVE_VALID
      JMP   nui_ok
@error:
      JMP   nui_error

; @label NUI.RESTORE_UNDER
; @kind routine
; @symbol nui_restore_under
; @summary Restore text planes previously saved by NUI.SAVE_UNDER.
; @requires NUI_SAVE_LEFT NUI_SAVE_TOP NUI_SAVE_WIDTH NUI_SAVE_HEIGHT NUI_SAVE_ADDRL NUI_SAVE_ADDRM NUI_SAVE_ADDRH
; @out A: 0 on success, 1 on error.
nui_restore_under:
      LDA   NUI_SAVE_VALID
      BEQ   @error
      JSR   nui_select_saved_region
      BNE   @error_clear
      JSR   vtext_calc_region_addr

      LDX   #0
      LDA   #BLT_SPACE_VGC_CHAR
      JSR   nui_restore_plane
      BNE   @error_clear
      LDX   #1
      LDA   #BLT_SPACE_VGC_COLOR
      JSR   nui_restore_plane
      BNE   @error_clear
      LDX   #2
      LDA   #BLT_SPACE_VGC_TEXTATTR
      JSR   nui_restore_plane
      BNE   @error_clear

      STZ   NUI_SAVE_VALID
      JMP   nui_ok
@error_clear:
      STZ   NUI_SAVE_VALID
@error:
      JMP   nui_error

; @label NUI.SAVE_UNDER_FULL
; @kind routine
; @symbol nui_save_under_full
; @summary Save the text and graphics planes under NUI_SAVE_* into caller-provided XRAM buffers.
; @requires NUI_SAVE_LEFT NUI_SAVE_TOP NUI_SAVE_WIDTH NUI_SAVE_HEIGHT NUI_SAVE_ADDRL NUI_SAVE_ADDRM NUI_SAVE_ADDRH
; @out A: 0 on success, 1 on error.
nui_save_under_full:
      JSR   nui_save_under
      BNE   @error
      JSR   nui_set_save_gfx_args
      JSR   vsprite_gfx_save_bg
      BNE   @error_clear
      LDA   #1
      STA   NUI_SAVE_VALID
      JMP   nui_ok
@error_clear:
      STZ   NUI_SAVE_VALID
@error:
      JMP   nui_error

; @label NUI.RESTORE_UNDER_FULL
; @kind routine
; @symbol nui_restore_under_full
; @summary Restore text and graphics planes previously saved by NUI.SAVE_UNDER_FULL.
; @requires NUI_SAVE_LEFT NUI_SAVE_TOP NUI_SAVE_WIDTH NUI_SAVE_HEIGHT NUI_SAVE_ADDRL NUI_SAVE_ADDRM NUI_SAVE_ADDRH
; @out A: 0 on success, 1 on error.
nui_restore_under_full:
      LDA   NUI_SAVE_VALID
      BEQ   @error
      JSR   nui_set_save_gfx_args
      JSR   vsprite_gfx_restore_bg
      BNE   @error_clear
      JMP   nui_restore_under
@error_clear:
      STZ   NUI_SAVE_VALID
@error:
      JMP   nui_error

nui_save_set_x_pixels:
      STZ   VSPRITE_XH
      ASL
      ROL   VSPRITE_XH
      ASL
      ROL   VSPRITE_XH
      STA   VSPRITE_XL
      RTS

nui_save_set_width_pixels:
      STZ   VSPRITE_WIDTHH
      ASL
      ROL   VSPRITE_WIDTHH
      ASL
      ROL   VSPRITE_WIDTHH
      STA   VSPRITE_WIDTHL
      RTS

nui_save_set_y_pixels:
      ASL
      ASL
      STA   VSPRITE_Y
      RTS

nui_save_set_height_pixels:
      ASL
      ASL
      STA   VSPRITE_HEIGHTL
      STZ   VSPRITE_HEIGHTH
      RTS

nui_set_save_gfx_args:
      LDA   NUI_SAVE_LEFT
      JSR   nui_save_set_x_pixels
      LDA   NUI_SAVE_TOP
      JSR   nui_save_set_y_pixels
      LDA   NUI_SAVE_WIDTH
      JSR   nui_save_set_width_pixels
      LDA   NUI_SAVE_HEIGHT
      JSR   nui_save_set_height_pixels
      LDA   #BLT_SPACE_XRAM
      STA   VSPRITE_BGSPACE
      LDA   NUI_SAVE_ADDRL+3
      STA   VSPRITE_BGADDRL
      LDA   NUI_SAVE_ADDRM+3
      STA   VSPRITE_BGADDRM
      LDA   NUI_SAVE_ADDRH+3
      STA   VSPRITE_BGADDRH
      LDA   VSPRITE_WIDTHL
      STA   VSPRITE_BGSTRL
      LDA   VSPRITE_WIDTHH
      STA   VSPRITE_BGSTRH
      RTS

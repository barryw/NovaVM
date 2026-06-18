; NOVAZ6.BIN segment interface.
;
; Shared between the runtime ROM (loader + dispatch trampolines) and the
; RAM-resident V6 segment (src/zvm6.s, linked by src/novaz6.cfg).
;
; Layout contract (the ROM checks this at boot):
;   $2000  .byte 'N','6'      magic word
;   $2002  nz6_entry          JMP/JSR target, A = dispatch id

.ifndef NOVA_NOVAZ_ZVM6_INC_INCLUDED
NOVA_NOVAZ_ZVM6_INC_INCLUDED = 1

; Dispatch ids (A on entry to NZ6_ENTRY).
NZ6_OP_RESET        = $00
NZ6_OP_SPLIT_WINDOW = $01
NZ6_OP_SET_WINDOW   = $02
NZ6_OP_ERASE_WINDOW = $03
NZ6_OP_SET_CURSOR   = $04
NZ6_OP_GET_CURSOR   = $05
NZ6_OP_SET_COLOUR   = $06
NZ6_OP_PULL         = $07
NZ6_OP_SELECT       = $08    ; ROM-only id (no Z opcode): rebuild the live
                             ; vtext region from the current window's props —
                             ; the V6 path of zvm_select_active_window.
NZ6_OP_TEXT_STYLE   = $09    ; set_text_style (VAR:17): V6 derives bold/
                             ; reverse from the current colour pair instead
                             ; of the ROM's fixed Nova-palette style table.
NZ6_OP_NEWLINE      = $0A    ; ROM-only id: a newline went out to the live
                             ; window (nz_screen_linefeed) - run the V6
                             ; carriage-return-interrupt countdown.
NZ6_OP_PRE_NEWLINE_SCROLL = $0B ; ROM-only id: a newline is about to scroll
                                ; the live text window; scroll the matching
                                ; gfx rect first so the split MCGA-style
                                ; framebuffer moves as one surface.
NZ6_OP_RESTORE_DISPLAY = $0C ; overlay-only id: rebuild the retained V6
                             ; graphics plane after save restore.
; Ids $0D-$1F are reserved for future VAR-op routing; EXT ids start at $20
; so the two ranges can never collide.
NZ6_EXT_BASE        = $20    ; ext dispatch id = NZ6_EXT_BASE + ext opnum
NZ6_EXT_COUNT       = 30     ; ext opnums 0-29; nz6_ext_table length asserted

NZ6_BASE            = $2000  ; segment load address (magic word lives here)
NZ6_ENTRY           = $2002  ; ROM calls JMP/JSR here with A = dispatch id
NZ6_SIZE            = $2000  ; fixed image size (novaz6.cfg fills to 8KB)
NZ6_STATE_BASE      = $4000  ; RAM-resident V6 segment BSS base
NZ6_STATE_SIZE      = $0800  ; full BSS reservation from novaz6.cfg
NZ6_SAVE_DISPLAY_STATE_BASE = $4000 ; V6 window table/current colour. This is
                                    ; semantic interpreter state: after restore,
                                    ; the next text output must land in the same
                                    ; live window that the story had selected.
NZ6_SAVE_DISPLAY_STATE_SIZE = $0110
NZ6_SAVE_GFX_STATE_BASE = $4136     ; retained native-picture replay state.
NZ6_SAVE_GFX_STATE_SIZE = $0545
NZ6_SAVE_STATE_SIZE = NZ6_SAVE_DISPLAY_STATE_SIZE + NZ6_SAVE_GFX_STATE_SIZE
NZ6_MAGIC0          = 'N'
NZ6_MAGIC1          = '6'

.endif
